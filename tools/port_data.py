#!/usr/bin/env python3
"""The game's data for the native build (docs/port.md, phase 2): FALL.EXE's initialised data
as definitions the host can link, with real pointers.

The matching build never defines the game's globals: the code is spliced into FALL.EXE and
uses the data where it lies. The native build needs definitions, and pointers there are 8
bytes. This tool writes them as one data section (file-scope assembly in a generated C file),
in FALL.EXE's address order, so data that the code reaches past the end of one global (a
string table read as one array, a struct read a byte early) still lies where the code
expects it:
  - a global whose type holds no pointers is FALL.EXE's bytes, unchanged;
  - a global whose type holds pointers (or iptr/uptr, ints that hold addresses) is laid out
    as the native compiler lays out its type, field by field: each pointer FALL.EXE
    relocates becomes an 8-byte relocation to the symbol it points into (with the offset
    translated to the target's native layout), other values are widened;
  - a global's extent runs to the next known symbol; bytes past its type's size (an unnamed
    neighbour) follow it unchanged.

Types come from the game's own declarations (`extern T name[...];` in src/ and include/).
When files disagree, the declaration with the most pointer-wide parts wins, and the
disagreement is reported. Layouts come from clang: each type is wrapped in a struct and
dumped with -fdump-record-layouts for i386 (FALL.EXE's layout: DAGGER_PORT's iptr is a
32-bit long there) and for the host.

  port_data.py report [--objs DIR]   what the generator would do: typed globals, problems
  (tools/port_build.py calls generate() on every build)

Problems it reports, for the 64-bit pass to fix in the source:
  - a relocated pointer inside a global whose declared type has no pointer there (the
    declaration is too narrow: an int or a byte array holding addresses);
  - declarations that disagree across files about where pointers are;
  - a non-zero value in a pointer field that FALL.EXE does not relocate.
"""
import argparse
import bisect
import collections
import csv
import os
import re
import struct
import subprocess
import sys
import tempfile
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import le  # noqa: E402

EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")
# real-mode memory the game points at (VGA 0xA0000, the BIOS data area 0x400...): natively the
# virtual PC's port_low_memory_block
LOWMEM_FIRST, LOWMEM_END = 0x400, 0x110000
CFLAGS = ["-std=gnu89", "-include", os.path.join(ROOT, "port", "include", "port.h"),
          "-DPORT_ENGINE", "-funsigned-char", "-fpack-struct=1", "-I" + os.path.join(ROOT, "port", "include"),
          "-I" + os.path.join(ROOT, "include"), "-I" + os.path.join(ROOT, "src", "engine"),
          "-fsyntax-only", "-w", "-ferror-limit=0",
          "-Xclang", "-fdump-record-layouts-canonical"]
TARGETS = {"32": ["-target", "i386-unknown-linux-gnu"], "64": []}

# ---- declarations ---------------------------------------------------------------------------

# Globals whose declarations do not say what FALL.EXE keeps there (compiled with records.h):
# copies of a record's 71-byte header, which hold its six pointer slots (95 bytes natively;
# the names inside them, like found_marker's x at +7 or D_0019615F, D_00196120's children,
# become labels at their native offsets), and the NPC buffer that talk fills as a character
# up to its career (char.c: 560 bytes, 672 natively), and scratch_190de4 (below).
RECORD_HEADER = ("struct __attribute__((packed)) { char plain[0x2F]; struct record *caster, *twin, *next, *prev, "
                 "*children, *parent; } __v")
CHARACTER_TO_CAREER = ("struct __attribute__((packed)) { char a[0x70]; struct record *target; char b[0xFB]; "
                       "struct record *equipped[27]; char c[0x55]; } __v")
TYPES = {
    "D_00196120": RECORD_HEADER,  # a container's header (run_0009401E.c says struct record)
    "D_00196167": RECORD_HEADER,  # monster.c's copy of a creature's header
    "found_marker": RECORD_HEADER,  # objcode.c: a marker found by a search
    "saved_player_object": RECORD_HEADER,  # the player's header (SAVEVARS.DAT)
    "object_debug_watch_copy": RECORD_HEADER,
    "npc_record_buffer": CHARACTER_TO_CAREER,
    # 256 bytes of scratch: a table of 64 pointers (<< 2 under Watcom), and screens' named slots
    # (scratch_190de8 ..., D_00190E18 ...) inside it, all zero in FALL.EXE: natively 512 bytes,
    # the slots at twice their offsets
    "scratch_190de4": "iptr __v[64]",
    # the next 256 bytes: a table of 64 pointers too (spell and item-maker lists)
    "scratch_190ee4": "iptr __v[64]",
    # (scratch_190be4 is an int slot that also keeps a pointer: talk.c's faction target, and
    # potions' item list starts there and runs over the slots after it. It stays in place as
    # bytes, so the slots after it are where the code reaches them from it; a pointer there
    # takes the next slot too, as the list does)
    # XnGine's: the vertex-pointer tables its polygon rings point at (xn_poly_ring_a/_b, 3n+1
    # pointers for n vertices), which the C reaches only through the rings: pointers up to
    # the next global ("all": the 0xDB87DB87 filler between the tables too)
    "xn_poly_ring_tables": ("void *__v[1]", "all"),
    # the per-object light lists: 400 declared, but the pool runs to xn_render_light_list_pool_start
    # (1,600 entries of 24 bytes) and a frame may use them all: every entry at native size
    "xn_render_light_list_pool": ("struct xn_light_ref __v[1]", "all", "src/engine/xpipe.h"),
    # pointers no code reads, so no source declares them (config/names.csv says what each
    # is): they lie beside data the game reads, and are pointers natively like their
    # neighbours. XnGine's span pool bounds (after xn_render_poly_pool's 1000 polygons):
    "xn_render_span_pool_start": "void *__v",
    "xn_render_span_pool_end": "void *__v",
    # the C runtime's math error messages, by error type (after _IsTable, D_00178630)
    "clib_matherr_messages": "char *__v[7]",
    "region_event_names": "char *__v[26]",
    "talk_place_names": "char *__v[8]",
    "intro_keys_na": "char *__v",
    "font_names": "char *__v[5]",
    "gender_names": "char *__v[3]",
    "prompt_how_many": "char *__v",
    "sheet_labels": "char *__v[2]",
    "lock_text_unlocked": "char *__v",
    "poison_text_no_weapon": "char *__v",
    "poison_text_already": "char *__v",
    "travel_text_no_fast_travel": "char *__v",
    "travel_text_no_destination": "char *__v",
    "identify_text_done": "char *__v",
    "identify_text_cost": "char *__v",
    "region_event_phrases": "char *__v[26]",
    "world_cell_header_ptr": "void *__v",
    # read only through enchant_power_params[15] (itemmaker_show_param_list walks it to the 0)
    "enchant_artifact_names": "char *__v[11]",
}

# FALL.EXE's library data (MemCheck's and the Watcom runtime's tables and messages, from 0x1889E0):
# only library code reads it, and the native build replaces that code with the host's, so its
# pointers stay as FALL.EXE's 32-bit values ("library" rows, for information)
LIBRARY_ONLY = {"library_data"}

DECL = re.compile(r"^extern\s+((?:const\s+|volatile\s+|signed\s+|unsigned\s+|struct\s+|union\s+)*"
                  r"[A-Za-z_]\w*(?:\s*\*)*)\s*(\**)\s*([A-Za-z_]\w*)\s*((?:\[[^\]]*\])*)\s*;")
FPTR_DECL = re.compile(r"^extern\s+(.+?)\(\s*\*\s*([A-Za-z_]\w*)\s*((?:\[[^\]]*\])*)\s*\)\s*(\(.*\))\s*;")
# several declarators on one line: `extern struct v **ring_a[], **ring_b[];`
MULTI_DECL = re.compile(r"^extern\s+((?:const\s+|volatile\s+|signed\s+|unsigned\s+|struct\s+|union\s+)*"
                        r"[A-Za-z_]\w*)\s+([^;(]*,[^;(]*);\s*$")
DECLARATOR = re.compile(r"^\s*(\**)\s*([A-Za-z_]\w*)\s*((?:\[[^\]]*\])*)\s*$")


def source_files():
    out = []
    for d in ("src/lifted", "src/hand"):
        out += sorted(os.path.join(ROOT, d, f) for f in os.listdir(os.path.join(ROOT, d))
                      if f.endswith(".c"))
    out += sorted(os.path.join(ROOT, "src", f) for f in os.listdir(os.path.join(ROOT, "src"))
                  if f.endswith(".c"))
    out += sorted(os.path.join(ROOT, "include", f) for f in os.listdir(os.path.join(ROOT, "include"))
                  if f.endswith(".h"))
    # XnGine's (object 2): its headers declare its globals, a few .c files their own
    engine = os.path.join(ROOT, "src", "engine")
    out += sorted(os.path.join(engine, f) for f in os.listdir(engine) if f.endswith((".h", ".c")))
    return out


def declarations():
    """name -> [(file, member_decl)] where member_decl declares `__v` with the global's type,
    e.g. `struct rect __v[1]` for `extern struct rect bank_buttons[];`"""
    decls = collections.defaultdict(list)
    for path in source_files():
        for line in open(path, errors="replace"):
            m = DECL.match(line)
            if m:
                base, stars, name, dims = m.group(1).strip(), m.group(2), m.group(3), m.group(4)
                dims = re.sub(r"\[\s*\]", "[1]", dims, count=1)
                decls[name].append((path, "%s %s__v%s" % (base, stars, dims), "[]" in m.group(4)
                                    or "[ ]" in m.group(4)))
                continue
            m = MULTI_DECL.match(re.sub(r"/\*.*?\*/|//.*", "", line).rstrip())
            if m:
                base = m.group(1).strip()
                for part in m.group(2).split(","):
                    d = DECLARATOR.match(part)
                    if d:
                        stars, name, dims = d.group(1), d.group(2), d.group(3)
                        unsized = "[]" in dims or "[ ]" in dims
                        dims = re.sub(r"\[\s*\]", "[1]", dims, count=1)
                        decls[name].append((path, "%s %s__v%s" % (base, stars, dims), unsized))
                continue
            m = FPTR_DECL.match(line)
            if m:
                ret, name, dims, args = m.group(1).strip(), m.group(2), m.group(3), m.group(4)
                dims = re.sub(r"\[\s*\]", "[1]", dims, count=1)
                decls[name].append((path, "%s (*__v%s)%s" % (ret, dims, args), "[]" in dims))
    return decls


# ---- layouts --------------------------------------------------------------------------------

LINE = re.compile(r"^\s*(\d+)(?::(\d+)-(\d+))? \| (\s*)(.*?)\s*$")
SIZE = re.compile(r"^\s*\| \[sizeof=(\d+)")


class Node:
    __slots__ = ("off", "type", "name", "depth", "kids", "size", "bitfield")

    def __init__(self, off, typ, name, depth, bitfield):
        self.off, self.type, self.name, self.depth = off, typ, name, depth
        self.kids, self.size, self.bitfield = [], None, bitfield


def parse_layouts(text):
    """record type -> root Node (children nested; root.size set)"""
    out = {}
    for block in text.split("*** Dumping AST Record Layout")[1:]:
        lines = block.strip("\n").splitlines()
        root, stack = None, []
        for ln in lines:
            m = SIZE.match(ln)
            if m and root is not None:
                root.size = int(m.group(1))
                break
            m = LINE.match(ln)
            if not m:
                continue
            off, b0 = int(m.group(1)), m.group(2)
            depth = len(m.group(4)) // 2
            body = m.group(5)
            if root is None:
                root = Node(off, body, "", 0, False)
                stack = [root]
                continue
            # "TYPE NAME": the name is the last word; anonymous members have none
            mm = re.match(r"^(.*\S)\s+([A-Za-z_]\w*)$", body)
            typ, name = (mm.group(1), mm.group(2)) if mm and not body.startswith(("struct (", "union (")) \
                else (body, "")
            if body.startswith(("struct ", "union ")) and mm and "(anonymous" not in body:
                typ, name = mm.group(1), mm.group(2)
            node = Node(off, typ, name, depth, b0 is not None)
            while stack and stack[-1].depth >= depth:
                stack.pop()
            stack[-1].kids.append(node)
            stack.append(node)
        if root is not None:
            out.setdefault(root.type, root)
    return out


def scalar_size(typ, bits):
    t = typ.replace("const ", "").replace("volatile ", "").strip()
    if "(*)" in t or t.endswith("*"):
        return bits // 8
    base = {"char": 1, "signed char": 1, "unsigned char": 1, "short": 2, "unsigned short": 2,
            "int": 4, "unsigned int": 4, "long": bits // 8, "unsigned long": bits // 8,
            "float": 4, "double": 8, "long long": 8, "unsigned long long": 8,
            "iptr": bits // 8, "uptr": bits // 8, "_Bool": 1}
    if t in base:
        return base[t]
    if t.startswith("enum "):
        return 4
    return None


def is_pointerish(typ):
    t = typ.strip()
    return "(*)" in t or t.endswith("*") or t in ("long", "unsigned long", "iptr", "uptr")


ARRAY = re.compile(r"^(.*?)\s*\[(\d+)\]((?:\[\d+\])*)$")
# an array of function pointers as clang writes its type: `void (*[9])(void)`
FPTR_ARRAY = re.compile(r"^.*\(\*\[(\d+)\]((?:\[\d+\])*)\)\s*\(.*\)$")


def fptr_array(typ):
    """`void (*[9])(void)` -> (9, "void (*)(void)"); None for other types"""
    m = FPTR_ARRAY.match(typ)
    if not m:
        return None
    n = int(m.group(1))
    for d in re.findall(r"\[(\d+)\]", m.group(2)):
        n *= int(d)
    return n, "void (*)(void)"


class Layouts:
    """layouts of one translation unit for both targets"""

    def __init__(self, recs32, recs64):
        self.r = {"32": recs32, "64": recs64}

    def size(self, typ, t):
        typ = typ.strip()
        fa = fptr_array(typ)
        if fa:
            return fa[0] * (4 if t == "32" else 8)
        m = ARRAY.match(typ)
        if m:
            inner = m.group(1) + m.group(3)
            s = self.size(inner, t)
            return None if s is None else s * int(m.group(2))
        s = scalar_size(typ, 32 if t == "32" else 64)
        if s is not None:
            return s
        rec = self.r[t].get(typ)
        return rec.size if rec else None

    def leaves(self, typ, t, base=0, path=""):
        """[(offset, size, type, path)] of the scalar parts of typ, in declaration order"""
        typ = typ.strip()
        fa = fptr_array(typ)
        if fa:
            z = 4 if t == "32" else 8
            return [(base + i * z, z, fa[1], "%s[%d]" % (path, i)) for i in range(fa[0])]
        m = ARRAY.match(typ)
        if m:
            inner = m.group(1) + m.group(3)
            es = self.size(inner, t)
            out = []
            for i in range(int(m.group(2))):
                out += self.leaves(inner, t, base + i * es, "%s[%d]" % (path, i))
            return out
        if scalar_size(typ, 32) is not None:
            return [(base, scalar_size(typ, 32 if t == "32" else 64), typ, path)]
        rec = self.r[t].get(typ)
        if rec is None:
            raise KeyError(typ)
        return self._node_leaves(rec, t, base, path)

    def _node_leaves(self, node, t, base, path):
        out = []
        if node.type.startswith("union"):
            # a union: its widest member at native size (the pointer one, if any)
            best = None
            for k in node.kids:
                kl = self._member_leaves(k, t, base, path)
                if best is None or sum(s for _o, s, _t, _p in kl) > sum(s for _o, s, _t, _p in best):
                    best = kl
            return best or []
        for k in node.kids:
            out += self._member_leaves(k, t, base, path)
        return out

    def _member_leaves(self, k, t, base, path):
        p = "%s.%s" % (path, k.name) if k.name else path
        if k.bitfield:
            return [(base + k.off, 1, "bitfield", p)]
        if k.kids:
            return self._node_leaves(k, t, base + k.off, p)
        return self.leaves(k.type, t, base + k.off, p)


def layouts_for(groups):
    """groups: {file: [(name, member_decl)]} -> {name: (size32, size64, leaves32, leaves64)}
    or {name: error string}"""
    def one(item):
        path, members = item
        src = []
        if path.endswith(".c"):
            src.append('#include "%s"' % path)
        else:
            src.append('#include "%s"' % path)
        for name, decl in members:
            src.append("struct __pd_%s { %s; };" % (name, decl))
            src.append("typedef char __pdz_%s[sizeof(struct __pd_%s)];" % (name, name))
        text = "\n".join(src) + "\n"
        res = {}
        dumps = {}
        with tempfile.NamedTemporaryFile("w", suffix=".c", delete=False) as f:
            f.write(text)
            tmp = f.name
        try:
            for t, targ in TARGETS.items():
                r = subprocess.run(["clang"] + targ + CFLAGS + [tmp], capture_output=True,
                                   text=True)
                dumps[t] = parse_layouts(r.stdout)
        finally:
            os.unlink(tmp)
        lay = Layouts(dumps["32"], dumps["64"])
        for name, decl in members:
            key = "struct __pd_%s" % name
            try:
                r32, r64 = lay.r["32"][key], lay.r["64"][key]
                l32 = lay._node_leaves(r32, "32", 0, "")
                l64 = lay._node_leaves(r64, "64", 0, "")
                if len(l32) != len(l64):
                    raise ValueError("layouts differ in shape")
                res[name] = (r32.size, r64.size, l32, l64)
            except Exception as e:  # noqa: BLE001
                res[name] = "%s: %s" % (type(e).__name__, e)
        return res

    out = {}
    with ThreadPoolExecutor(os.cpu_count()) as ex:
        for r in ex.map(one, groups.items()):
            out.update(r)
    return out


# ---- the image ------------------------------------------------------------------------------

def symbol_table():
    table = {}
    for line in open(os.path.join(ROOT, "config", "symbols.txt")):
        m = re.match(r"(\w+) = (0x[0-9A-Fa-f]+); (\w+)", line)
        if m:
            table[m.group(1)] = (int(m.group(2), 16), m.group(3))
    return table


def code_ranges():
    """[(start, end)] of code in objects 1 and 2: functions are never data"""
    out = []
    for r in csv.DictReader(open(os.path.join(ROOT, "config", "functions.csv"))):
        a = int(r["va"], 16)
        if 0xC0000 <= a < 0x170000:
            continue        # XnGine's: its span there runs over its data; code_bytes below
        out.append((a, a + int(r["span"] or r["size"])))
    for r in csv.DictReader(open(os.path.join(ROOT, "config", "xngine_functions.csv"))):
        a = int(r["va"], 16)
        out.append((a, a + int(r["code_bytes"])))
    return sorted(out)


class Plan:
    """what generate() decided: the globals laid out, in address order"""

    def __init__(self):
        self.items = []         # (addr, end, name, typed) in address order
        self.problems = collections.defaultdict(list)
        self.functions = set()  # code symbols the data points at


def plan_data(need, table, image=None, report_only=False, defined=frozenset()):
    """need: {name: addr} of the data symbols the game names; defined: names something else
    defines (their bytes keep their place, without the label). Returns (C text, Plan)."""
    image = image or le.LE(EXE)
    objs = image.load(relocate=True)
    fix = {}
    for f in image.fixups():
        if f.kind == le.SRC_OFF32:
            fix[f.src_va] = f.target_va
    code = code_ranges()
    code_starts = [a for a, _e in code]
    func_name = {}
    for name, (addr, kind) in table.items():
        if kind == "func":
            func_name.setdefault(addr, name)
    for name, (addr, kind) in table.items():
        # a name the build defines first (a game function its source still calls func_X
        # while names.csv has a candidate name for it)
        if kind == "func" and name in defined and func_name[addr] not in defined:
            func_name[addr] = name
    for addr in list(func_name):
        if func_name[addr] not in defined and "func_%08X" % addr in defined:
            func_name[addr] = "func_%08X" % addr

    def in_code(a):
        i = bisect.bisect_right(code_starts, a) - 1
        return i >= 0 and code[i][0] <= a < code[i][1]

    plan = Plan()
    declared_size = {}
    # the data symbols to lay out: the game's, and those of every data object it touches;
    # extents run to the next known data address
    starts = {}
    for name, (addr, kind) in table.items():
        if kind == "global" and not in_code(addr):
            starts.setdefault(addr, name)
    need_at = collections.defaultdict(list)    # names the code uses for one address
    for name, addr in sorted(need.items()):
        need_at[addr].append(name)
        starts[addr] = need_at[addr][0]
    for o in image.objs:
        if o.index == 3 and o.base not in starts:
            starts[o.base] = "D_%08X" % o.base
    # XnGine's object: its data lies between its functions; all of it, in order, as object 3
    eng = next(o for o in image.objs if o.index == 2)
    for a in [eng.base] + [e for _s, e in code]:
        if eng.base <= a < eng.base + eng.vsize and not in_code(a):
            starts.setdefault(a, "D_%08X" % a)
    addrs = sorted(starts)

    def extent(addr):
        o = image.obj_of_va(addr)
        i = bisect.bisect_right(addrs, addr)
        end = addrs[i] if i < len(addrs) else o.base + o.vsize
        end = min(end, o.base + o.vsize)
        j = bisect.bisect_right(code_starts, addr)
        if o.index != 3 and j < len(code_starts):
            end = min(end, code_starts[j])
        if o.index == 1:
            # data the code names in object 1 (code): what its declaration says
            end = min(end, addr + declared_size.get(starts[addr], 64))
        return end

    # emit all of objects 3 (the game's data) and 2 (XnGine's), in order, and whatever else
    # the code names
    chosen = set(a for a in addrs if image.obj_of_va(a) and image.obj_of_va(a).index in (2, 3)
                 and not in_code(a))
    chosen |= set(need.values())

    decls = declarations()

    # two names for one address (XnGine's aliases: a struct and its first field): the one
    # whose declaration is the largest is laid out, the others are labels at its start
    def score(d):
        return (d[1].count("*") + len(re.findall(r"\b(iptr|uptr|struct|union)\b", d[1])))
    alias_labels = {}
    multi = {a: ns for a, ns in need_at.items() if len(ns) > 1}
    if multi:
        g = collections.defaultdict(list)
        for ns in multi.values():
            for n in ns:
                if decls.get(n):
                    d = max(decls[n], key=lambda d: (score(d), not d[2]))
                    g[d[0]].append((n, d[1]))
        sizes = layouts_for(g)
        for a, ns in multi.items():
            def size(n):
                v = sizes.get(n)
                return v[0] if v is not None and not isinstance(v, str) else 0
            ns = sorted(ns, key=lambda n: (-size(n), n))
            starts[a] = ns[0]
            alias_labels[a] = ns[1:]

    # types
    groups = collections.defaultdict(list)
    pick = {}
    for addr in sorted(chosen):
        name = starts[addr]
        if name in TYPES:
            t = TYPES[name] if isinstance(TYPES[name], tuple) else (TYPES[name], False)
            header = os.path.join(ROOT, t[2] if len(t) > 2 else "include/records.h")
            pick[name] = (header, t[0], t[1])
            groups[pick[name][0]].append((name, pick[name][1]))
            continue
        ds = decls.get(name)
        if not ds:
            continue
        # most pointer-wide declaration first (a disagreement is reported below); of those, one
        # that gives the array's size
        ds = sorted(ds, key=lambda d: (score(d), not d[2]), reverse=True)
        pick[name] = ds[0]
        if len({re.sub(r"\s+", " ", d[1]) for d in ds}) > 1 and \
                len({score(d) for d in ds}) > 1:
            plan.problems["disagree"].append(
                "%s: %s" % (name, "; ".join(sorted({"%s (%s)" % (d[1], os.path.relpath(d[0], ROOT))
                                                     for d in ds}))))
        groups[ds[0][0]].append((name, ds[0][1]))
    lay = layouts_for(groups)
    for name, v in lay.items():
        if not isinstance(v, str) and not pick[name][2]:
            declared_size[name] = v[0]

    # the layout of each chosen global: identity unless its type is wider natively
    typed = {}
    for name, v in lay.items():
        if isinstance(v, str):
            plan.problems["layout"].append("%s: %s" % (name, v))
            continue
        typed[name] = v

    layout_of = {}   # name -> (addr, end, count, size32, size64, l32, l64)
    inner = collections.defaultdict(list)   # outer name -> [(addr, inner name)]
    order = sorted(chosen)
    skip = set()
    for i, addr in enumerate(order):
        if addr in skip:
            continue
        name = starts[addr]
        end = extent(addr)
        layout_of[name] = (addr, end, 0, 0, 0, None, None)
        if name in typed:
            size32, size64, l32, l64 = typed[name]
            wide = size32 != size64 or any(is_pointerish(t) for _o, _s, t, _p in l32)
            if wide and size32:
                if pick[name][2] == "all":
                    count = max(1, (end - addr) // size32)
                elif pick[name][2]:
                    # [] arrays: the elements before the next global, up to the first whose
                    # pointer parts are neither relocated nor zero (data of another kind)
                    count = 0
                    o = image.obj_of_va(addr)
                    for k in range(max(1, (end - addr) // size32)):
                        ok = True
                        for o32, z32, t, _p in l32:
                            a = addr + k * size32 + o32
                            if is_pointerish(t) and z32 == 4 and a not in fix and "*" in t:
                                if struct.unpack_from("<I", objs[o.index], a - o.base)[0]:
                                    ok = False
                                    break
                        if not ok:
                            break
                        count += 1
                    count = max(1, count)
                else:
                    count = 1
                o = image.obj_of_va(addr)
                span_end = min(addr + count * size32, o.base + o.vsize)
                # a declared array that runs over later globals takes them in, as labels
                for later in order[i + 1:]:
                    if later >= span_end:
                        break
                    skip.add(later)
                    inner[name].append((later, starts[later]))
                end = max(end, span_end) if not inner[name] else max(span_end, extent(inner[name][-1][0]))
                layout_of[name] = (addr, end, count, size32, size64, l32, l64)
        plan.items.append((addr, end, name, layout_of[name][2] > 0))
    for outer, ins in inner.items():
        for a, n in ins:
            layout_of[n] = ("inner", outer, a - layout_of[outer][0])

    item_starts = [a for a, _e, _n, _t in plan.items]

    def native_offset(name, off):
        if layout_of[name][0] == "inner":
            _i, outer, base = layout_of[name]
            return native_offset(outer, base + off)
        addr, end, count, s32, s64, l32, l64 = layout_of[name]
        if not count:
            return off
        if off >= count * s32:
            return count * s64 + (off - count * s32)
        k, r = divmod(off, s32)
        for (o32, z32, t, p), (o64, z64, _t, _p) in zip(l32, l64):
            if o32 <= r < o32 + z32:
                return k * s64 + o64 + (r - o32 if z32 == z64 or r == o32 else 0)
        return k * s64 + r

    def target(ta):
        """a relocation target: (symbol, addend) in the native image"""
        o = image.obj_of_va(ta)
        if o is not None and (o.index == 1 or in_code(ta)):
            if ta in func_name:
                plan.functions.add(func_name[ta])
                return func_name[ta], 0
            if in_code(ta) and ta in code_starts:
                n = "func_%08X" % ta
                plan.functions.add(n)
                return n, 0
            plan.problems["target"].append("0x%08X: inside code, not a function start" % ta)
            return None, 0
        i = bisect.bisect_right(item_starts, ta) - 1
        if i >= 0:
            a, e, n, typed = plan.items[i]
            if a <= ta < e or (ta == e):
                count, s32 = layout_of[n][2], layout_of[n][3]
                if typed and ta - a >= count * s32:
                    return "L__pd_" + n, ta - a         # its tail: FALL.EXE's bytes in place
                return n, native_offset(n, ta - a)
        plan.problems["target"].append("0x%08X: not in any global laid out" % ta)
        return None, 0

    # emission. FALL.EXE's data stays in place, byte for byte, so every global keeps its
    # distance from every other: the code reaches tables through shifted bases (climate's
    # categories from 223 bytes before them, names minus 1...). A global whose type holds
    # pointers is laid out natively out of line, after it all (its name, its inner names, the
    # relocations to it), its FALL.EXE bytes left in place under a local label, and a copy
    # of its tail (the unnamed bytes after it) follows it there.
    out = []
    out.append("/* generated by tools/port_data.py: FALL.EXE's data in address order, the globals\n"
               "   holding pointers in native layouts after it with 8-byte relocations\n"
               "   (docs/port.md, phase 2) */\n")
    asm = [".section __DATA,__data", ".p2align 4"]
    in_place = asm
    out_of_line = []

    def emit_bytes(b):
        i = 0
        while i < len(b):
            if b[i] == 0:
                j = i
                while j < len(b) and b[j] == 0:
                    j += 1
                if j - i >= 16:
                    asm.append(".zero %d" % (j - i))
                    i = j
                    continue
            j = min(len(b), i + 32)
            k = i
            while k < j and not (b[k] == 0 and b[k:k + 16] == bytes(min(16, len(b) - k))
                                 and len(b) - k >= 16):
                k += 1
            k = max(k, i + 1)
            asm.append(".byte " + ",".join(str(x) for x in b[i:k]))
            i = k

    for addr, end, name, is_typed in plan.items:
        o = image.obj_of_va(addr)
        raw = bytes(objs[o.index][addr - o.base:end - o.base])
        a2, e2, count, s32, s64, l32, l64 = layout_of[name]
        if is_typed:
            asm = in_place
            asm.append("L__pd_%s:" % name)
            emit_bytes(raw)
            asm = out_of_line
            asm.append(".p2align 3")
        else:
            asm = in_place
        for n in [name] + alias_labels.get(addr, []):
            if n not in defined:
                asm.append(".globl _%s" % n)
                asm.append("_%s:" % n)
        if not is_typed:
            # relocations inside an untyped global: the declaration is too narrow
            for a in range(addr, end):
                if a in fix:
                    kind = "library" if name in LIBRARY_ONLY else "narrow" if o.index == 3 else "narrow-engine"
                    plan.problems[kind].append("%s+0x%X (0x%08X) -> 0x%08X" % (name, a - addr, a, fix[a]))
            emit_bytes(raw)
            continue
        buf = bytearray(count * s64)
        events = []     # (native offset, kind, value): labels and relocations
        for k in range(count):
            for (o32, z32, t, p), (o64, z64, _t, _p) in zip(l32, l64):
                src = addr + k * s32 + o32
                dst = k * s64 + o64
                v = raw[k * s32 + o32:k * s32 + o32 + z32]
                if z32 == z64 and not (z32 == 4 and is_pointerish(t) and src in fix):
                    buf[dst:dst + z64] = v
                    if src in fix and not is_pointerish(t):
                        plan.problems["narrow"].append("%s%s (0x%08X) -> 0x%08X" % (name, p, src, fix[src]))
                    continue
                if src in fix:
                    sym, add = target(fix[src])
                    if sym:
                        events.append((dst, 1, (sym, add)))
                    continue
                (val,) = struct.unpack("<I", v) if len(v) == 4 else (0,)
                # a pointer anywhere in low memory; an iptr only as a lone variable where
                # real-mode data lives (an iptr table can hold plain numbers too:
                # D_00178848's powers of 16 run up to 0x10000000)
                if (("*" in t) and LOWMEM_FIRST <= val < LOWMEM_END) or \
                        (t in ("long", "unsigned long") and count == 1 and p == ".__v" and
                         re.match(r"(volatile\s+)?[iu]ptr\b", pick[name][1]) and
                         (0x400 <= val < 0x500 or 0xA0000 <= val < LOWMEM_END)):
                    # a real-mode address (VGA's 0xA0000, the BIOS data area): the native
                    # build's low memory block (port/host/vpc.c)
                    events.append((dst, 1, ("port_low_memory_block", val)))
                    continue
                if val and ("*" in t):
                    plan.problems["raw-pointer"].append("%s%s = 0x%08X" % (name, p, val))
                    if val == 0xFFFFFFFF:
                        val = -1        # a pointer at -1 (XnGine's "no font" slots): all ones
                if t in ("long", "iptr") and val & 0x80000000:
                    val -= 1 << 32
                buf[dst:dst + 8] = struct.pack("<q" if val < 0 else "<Q", val)
        tail = raw[count * s32:]
        for a in range(addr + count * s32, end):
            if a in fix:
                plan.problems["narrow"].append("%s+0x%X tail (0x%08X) -> 0x%08X" % (name, a - addr, a, fix[a]))
        buf += tail
        for a, n in inner.get(name, []):
            events.append((native_offset(name, a - addr), 0, n))
        pos = 0
        for off, kind, val in sorted(events):
            emit_bytes(buf[pos:off])
            pos = max(pos, off)
            if kind == 0:
                if val not in defined:
                    asm.append(".globl _%s" % val)
                    asm.append("_%s:" % val)
            else:
                sym, add = val
                label = sym if sym.startswith("L__pd_") else "_" + sym     # local: in place
                asm.append(".quad %s%s" % (label, ("+%d" % add) if add else ""))
                pos = off + 8
        emit_bytes(buf[pos:])

    if report_only:
        return None, plan
    out.append("__asm__(\n")
    for line in in_place + out_of_line:
        out.append('    "%s\\n"\n' % line)
    out.append(");\n")
    return "".join(out), plan


def need_from_objects(objdir):
    """the data names the game's objects use, from nm (for `report`)"""
    objs = []
    for d, _, files in os.walk(objdir):
        objs += [os.path.join(d, f) for f in files if f.endswith(".o") and "game.dir" in d]
    table = symbol_table()
    need = {}
    for i in range(0, len(objs), 200):
        r = subprocess.run(["nm", "-gu"] + objs[i:i + 200], capture_output=True, text=True)
        for line in r.stdout.split():
            n = line.lstrip("_")
            if n in table and table[n][1] == "global":
                need[n] = table[n][0]
            m = re.fullmatch(r"(D|xn_data)_([0-9A-F]{6,8})", n)
            if m:
                need[n] = int(m.group(2), 16)
    return need


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("cmd", choices=["report"])
    ap.add_argument("--objs", default=os.path.join(ROOT, "build", "port", "CMakeFiles"))
    ap.add_argument("--out", help="also write the generated C here")
    a = ap.parse_args()
    need = need_from_objects(a.objs)
    text, plan = plan_data(need, symbol_table(), report_only=not a.out)
    if a.out:
        open(a.out, "w").write(text)
    typed = sum(1 for _a, _e, _n, t in plan.items if t)
    print("%d globals laid out (%d typed with native layouts); %d functions pointed at"
          % (len(plan.items), typed, len(plan.functions)))
    for kind, rows in sorted(plan.problems.items()):
        print("\n%s: %d" % (kind, len(rows)))
        for r in rows[:15]:
            print("  " + r)


if __name__ == "__main__":
    main()
