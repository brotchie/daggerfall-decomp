#!/usr/bin/env python3
"""XnGine's readable C (src/engine/): skeletons, the build, routing, tests and the report.

The readable C replaces object 2's asm one function at a time. Each C function keeps its asm
interface (config/xngine_abi.csv, tools/xn_abi.py), is compiled by Watcom C32 10.0a and runs
inside the game on the game's own stack (src/engine/xngine.h says how). The asm stays loaded:
an asm caller reaches the C through a code hook at the function's asm entry, and the C reaches
asm code by calling its address.

  skel SUBSYS  src/engine/x<subsys>.h and <subsys>.c to start from (build/xn_readable/skel/
               when they exist): a declaration per function with the pragma its ABI row
               gives, its name, evidence and asm in a comment; bodies under `#if 0`.
  build        compile src/engine/*.c and *.asm (tools/wcc10.py, one DOSBox-X session) and link
               them (tools/xn_cload.py link) at RBASE, a region the emulator maps outside the
               memory records compare. Names the objects do not define resolve to the loaded
               game: functions and globals of config/names.csv (NAME_ and _NAME as Watcom
               decorates them), asm_NAME (always the asm entry), xn_data_XXXXXX and
               xn_code_XXXXXX (any address), func_XXXXXXXX and D_XXXXXXXX. A C function named
               like an object-2 function is converted; NAME_r makes it a glue route.
               -> build/xn_readable/image.pkl
  route        the routing table: each converted function's asm entry goes straight to its C;
               or, when its callers need EAX kept (Watcom's code never keeps it), to a stub
               `push eax; call C; pop eax; ret N` (a result in AL, AH or AX is merged into
               the kept EAX before the pop: `mov [esp], al`...); or (NAME_r defined: several
               outputs, flags out) to a stub `pushfd; pushad; mov eax, esp; call NAME_r;
               popad; popfd; ret N`. An interrupt or exception handler (ABI convention
               interrupt, asm returning with iretd or retf) goes to a stub (kind isr) that
               saves every register, loads DS and ES with the data selector the asm's
               prologue loads (when it does), calls NAME_r(r) or NAME(), restores them and
               returns as the asm does (iretd / retf).
               Checks each route's declared interface (its #pragma aux, or Watcom's default)
               against the ABI row.
  test         replay records with converted functions routed to C and compare as the
               interface sees them (xn_abi.abi_compare): memory writes exactly except the dead
               stack below the exit ESP; registers except the row's clobbers (preserved ones
               against their entry values); flags only flags_out; port I/O and interrupts
               exactly. FUNC/SUBSYS: their records, one function routed at a time;
               --all: the same records with every converted function routed at once;
               --corpus: every record of the corpus with every converted function routed
               (each compared by its own function's row).
  diff         differential tests (tools/xn_cload.py diff_test): made-up entry states, the asm
               recorded from each, the C replaying it; --untested: converted functions with no
               records.
  play         the game with every converted function in C; --compare runs the asm too from
               the same snapshot and compares the screens pixel for pixel.
  frames       lockstep frames from a snapshot (asm and C from the same machine, a pass of
               main's loop each); --boundary compares as the boundary does (below).
  scenarios    the scripted lockstep scenarios of tools/xn_scenarios.py (a save, a prelude, N
               frames with per-frame input), compared at the boundary; per scenario: frames
               identical / total. --continuous: the C machine keeps its own state.

Canonical C (docs/xngine_canonical.md): a function is canonical when src/engine_test/ (the
test shims, built into the image only for the tools) defines its shim NAME_r. Its C has a
plain prototype; record tests route its asm entry to the shim (kind shim), which maps the
asm's registers to the C call and back; config/xngine_dropped.csv excuses the memory (and
registers) the canonical C no longer writes on purpose. In the game (frames, scenarios, play)
a canonical function is reached only at the boundary: an entry the game calls
(config/xngine_boundary.csv) goes straight to its C (kind boundary), or through a stub that
keeps the registers the game reads after the call that the prototype may change (kind
boundary-keep); every other canonical function has no route there (its callers are C).

  test --boundary  the records of the boundary's entries whose caller is the game (the return
               address in object 1, a direct call, or an interrupt), each entry routed as the
               game reaches it: compared on the registers the game reads after the call (the
               entry's game_reads), ESP, the game-visible memory written (engine-private memory
               and the dead stack masked: tools/xn_boundary.py) and the port and DOS I/O in
               order (with the bytes of DOS writes; CPU exceptions left out).
  report       build/xngine/rc_report.csv: per function, converted, records passed/total, all
               routed, diff passed/total, the clobber and input tests (tools/xn_abi.py).
  asm FUNC     the function's asm with names; abi FUNC: its ABI row.

An agent converting a subsystem works in its own folder: XN_RC_SRC=DIR adds its C files and
headers to src/engine's; XN_RC_OUT=DIR keeps its image, results, report and proposed names
(names.csv) apart; XN_ABI and XN_ABI_OVERRIDES (tools/xn_abi.py) give it its own copy of the
ABI table. The coordinator merges finished work into src/engine and config/.

usage: xn_rc.py skel SUBSYS [--force] [--out DIR]
       xn_rc.py build
       xn_rc.py route
       xn_rc.py test [FUNC|SUBSYS ...] [--all] [--corpus] [--boundary] [-j N] [--max-per N] [-v]
       xn_rc.py diff [FUNC|SUBSYS ...] [--untested] [--trials N]
       xn_rc.py play SNAP [--ticks N] [--shot PNG] [--asm] [--compare] [--script INPUT]
       xn_rc.py frames SNAP [--frames N] [--ticks N] [--script S] [--irqs N] [--boundary]
       xn_rc.py scenarios [NAME ...] [-j N] [--continuous] [--frames N] [-v]
       xn_rc.py report
       xn_rc.py asm FUNC | abi FUNC
"""
import argparse
import collections
import csv
import glob
import json
import os
import pickle
import re
import shutil
import struct
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src", "engine")
TEST_SRC = os.path.join(ROOT, "src", "engine_test")     # the test shims (canonical C)
DROPPED_CSV = os.path.join(ROOT, "config", "xngine_dropped.csv")
BASE_OUT = os.path.join(ROOT, "build", "xn_readable")
# A conversion agent works in a folder of its own: XN_RC_OUT holds its image, results and
# report (and names.csv, its proposed names); XN_RC_SRC (folders separated by ':') adds its
# C files and headers to src/engine's, replacing any of the same name.
OUT = os.environ.get("XN_RC_OUT") or BASE_OUT
EXTRA_SRC = [d for d in os.environ.get("XN_RC_SRC", "").split(os.pathsep) if d]
WORK = os.path.join(OUT, "work")
IMAGE = os.path.join(OUT, "image.pkl")
REPORT = os.path.join(ROOT, "build", "xngine", "rc_report.csv") if OUT == BASE_OUT else \
    os.path.join(OUT, "rc_report.csv")
RESULTS = os.path.join(OUT, "results.json")
LOAD = 0x01000000
RBASE = 0x11000000          # above the literal C's region (0x10000000, 16 MB)
RSIZE = 0x00400000
FLAGS = "-mf -4r -s -zl -zld -ox -bt=dos -w3 -zq".split()


# --------------------------------------------------------------------------------------------
# Names

_names = None
# config/names.csv, then the names the conversion proposes (merged into it by the coordinator)
# config/xngine_aliases.csv: second names for an address that the C views two ways (a struct
# and its first field); names.csv holds one name per address
NAME_FILES = [os.path.join(ROOT, "config", "names.csv"),
              os.path.join(ROOT, "config", "xngine_aliases.csv"), os.path.join(BASE_OUT, "names.csv")] + \
    ([os.path.join(OUT, "names.csv")] if OUT != BASE_OUT else [])


def names():
    """{name: (kind, address)} of config/names.csv and build/xn_readable/names.csv (proposed
    names, for the coordinator to merge), and {address: (name, confidence, evidence)} for
    functions."""
    global _names
    if _names is None:
        by_name, funcs = {}, {}
        for p in NAME_FILES:
            if not os.path.exists(p):
                continue
            with open(p, newline="") as f:
                for r in csv.DictReader(f):
                    if r["kind"] not in ("func", "global"):
                        continue                # struct fields: "record+0xNN"
                    a = int(r["address"], 16)
                    if r["name"] in by_name:
                        continue                # config/names.csv first
                    by_name[r["name"]] = (r["kind"], a)
                    if r["kind"] == "func":
                        funcs.setdefault(a, (r["name"], r["confidence"], r["evidence"]))
        # the patch fields' names the self-modifying-code designs propose
        pf = os.path.join(BASE_OUT, "smc", "patch_fields.csv")
        if os.path.exists(pf):
            with open(pf, newline="") as f:
                for r in csv.DictReader(f):
                    if r.get("proposed_name") and r["proposed_name"] not in by_name:
                        by_name[r["proposed_name"]] = ("global", int(r["field"], 16))
        _names = (by_name, funcs)
    return _names


def func_va(text):
    """A function given as a name, VA (hex) or func_XXXXXXXX."""
    by_name, _f = names()
    if text in by_name and by_name[text][0] == "func":
        return by_name[text][1]
    return int(text.replace("func_", ""), 16)


def subsystem_funcs(sub):
    import xn_abi
    abi = xn_abi.read_abi()
    return sorted(va for va, r in abi.items() if r["subsystem"] == sub)


def expand(args):
    """FUNC and SUBSYS arguments -> function VAs."""
    import xn_abi
    abi = xn_abi.read_abi()
    subs = {r["subsystem"] for r in abi.values()}
    out = []
    for a in args:
        if a in subs:
            out += subsystem_funcs(a)
        else:
            out.append(func_va(a))
    return out


def resolver():
    """name -> linear address in the loaded game, for symbols the C does not define."""
    by_name, _f = names()

    def resolve(sym):
        base = sym
        if sym.startswith("_"):
            base = sym[1:]
        elif sym.endswith("_"):
            base = sym[:-1]
        asm = base.startswith("asm_")
        if asm:
            base = base[4:]
        m = re.match(r"(?:xn_data_|xn_code_|D_|func_)([0-9A-Fa-f]{6,8})$", base)
        if m:
            return LOAD + int(m.group(1), 16)
        e = by_name.get(base)
        if e is not None:
            return LOAD + e[1]
        return None
    return resolve


# --------------------------------------------------------------------------------------------
# Build

def _files(pattern, test=False):
    """src/engine's files matching pattern, those of the XN_RC_SRC folders replacing any of
    the same name. test: src/engine_test's instead (and each XN_RC_SRC folder's test/)."""
    by = {}
    dirs = [TEST_SRC] + [os.path.join(d, "test") for d in EXTRA_SRC] if test else [SRC] + EXTRA_SRC
    for d in dirs:
        for p in glob.glob(os.path.join(d, pattern)):
            by[os.path.basename(p).lower()] = p
    return [by[k] for k in sorted(by)]


def test_sources():
    return _files("*.c", test=True) + _files("*.asm", test=True)


def sources():
    """The engine's sources, then the test shims (built into the image for the tools only)."""
    eng = _files("*.c") + _files("*.asm")
    names_ = {os.path.basename(p).lower() for p in eng}
    tst = [p for p in test_sources()]
    clash = [p for p in tst if os.path.basename(p).lower() in names_]
    if clash:
        raise SystemExit("test shim files named like engine files (8.3, one folder in DOSBox): %s"
                         % " ".join(clash))
    return eng + tst


def headers():
    hs = _files("*.h") + _files("*.h", test=True)
    bad = [h for h in hs if not re.match(r"^[a-z0-9_]{1,8}\.h$", os.path.basename(h))]
    if bad:
        raise SystemExit("header names must be 8.3 (DOSBox): %s" % " ".join(bad))
    return hs


def keeps_eax(row):
    """True when the row's callers read EAX after the call without it being an output: a C
    function cannot keep it (Watcom's code uses EAX freely, `modify exact` or not)."""
    import xn_abi
    return (row["clob"] | row["out"]) & xn_abi.RMASK["eax"] != xn_abi.RMASK["eax"]


# The keep-eax stub's merge of a result in a part of EAX (the rest kept for the callers):
# mov [esp], al / mov [esp+1], ah / mov [esp], ax, between the call and the pop.
EAX_MERGE = {"al": b"\x88\x04\x24", "ah": b"\x88\x64\x24\x01", "ax": b"\x66\x89\x04\x24"}


def eax_merge(row):
    """The part of EAX the row outputs while callers need the rest kept ("al", "ah", "ax"),
    or None."""
    import xn_abi
    part = row["out"] & xn_abi.RMASK["eax"]
    if not part or not keeps_eax(row):
        return None
    for name in ("al", "ah", "ax"):
        if part == xn_abi.mask_of(name):
            return name
    return None


_obj2 = None


def handler_entry(va):
    """For an interrupt or exception handler (asm that returns with iretd, iret or retf): (the
    return instruction's bytes, the address of the data selector its prologue loads into DS,
    or None).
    The prologue's `mov ax, SEL; mov ds, eax` holds the loader's selector for the data object
    in its immediate; the route's stub loads DS and ES from there, as the asm would."""
    global _obj2
    if _obj2 is None:
        import xn_link
        _obj2 = xn_link.Image()
    code = _obj2.bytes_at(va, 0x200)
    ret = None
    # a straight scan: a handler's run-time blocks follow its entry (and are functions of
    # their own in the map, so its listing alone may not reach the return)
    import capstone
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for i in md.disasm(code, va):
        if i.mnemonic in ("iretd", "iret", "retf"):
            ret = bytes(i.bytes)        # as the asm has it (66 CF: a 16-bit iret, which it is)
            break
        if i.mnemonic == "ret":
            break
    if ret is None:
        return None, None
    k = code[:24].find(b"\x66\xB8")
    sel = None
    if k >= 0 and code[k + 4:k + 6] == b"\x8E\xD8":
        sel = va + k + 2
    return ret, sel


def isr_stub(a, target, ret, sel):
    """An interrupt handler's route: `pushfd; pushad; cld`, with sel: `push ds; push es`,
    DS = ES = the selector at sel (read through CS: the asm prologue's immediate), then
    `call target` with EAX pointing at the saved registers (an xn_regs: a glue NAME_r may
    change them; a plain NAME ignores it), and back: segments, `popad; popfd`, and the asm's
    own return (iretd, or retf for a DPMI exception handler)."""
    b = b"\x9C\x60\xFC"
    if sel is not None:
        b += b"\x1E\x06" + b"\x2E\x66\xA1" + struct.pack("<I", LOAD + sel)
        b += b"\x8E\xD8\x8E\xC0" + b"\x8D\x44\x24\x08"
    else:
        b += b"\x89\xE0"
    b += b"\xE8" + struct.pack("<i", target - (a + len(b) + 5))
    if sel is not None:
        b += b"\x07\x1F"
    return b + b"\x61\x9D" + ret


def build(flags=None):
    import wcc10
    import xn_cload
    t0 = time.time()
    srcs = sources()
    if not srcs:
        print("nothing in src/engine")
        return 1
    flags = flags.split() if isinstance(flags, str) else (flags or FLAGS)
    shutil.rmtree(WORK, ignore_errors=True)
    os.makedirs(WORK)
    for h in headers():
        shutil.copyfile(h, os.path.join(WORK, os.path.basename(h).upper()))
    objs, td = wcc10.compile_many(srcs, flags, workdir=WORK)
    failed = 0
    warnings = []
    for k, s in enumerate(srcs):
        e = os.path.join(td, "N%04d.ERR" % k)
        msg = open(e, errors="replace").read().strip() if os.path.exists(e) else ""
        name = os.path.relpath(s, ROOT)
        msg = msg.replace("N%04d.C" % k, name).replace("N%04d.ASM" % k, name)
        if objs[s] is None or "Error!" in msg:
            failed += 1
            print("FAILED", name)
            print("\n".join(msg.splitlines()[-15:]))
        elif msg:
            warnings.append(msg)
    for w in warnings:
        print(w)
    if failed:
        return 1
    try:
        img, syms = xn_cload.link([objs[s] for s in srcs], RBASE, resolver())
    except SystemExit as e:
        print("link:", e)
        return 1
    by_name, funcs = names()
    obj2 = {n: a for n, (k, a) in by_name.items() if k == "func" and 0xC0000 <= a < 0x161568}
    # the symbols the test shims define (src/engine_test): NAME_r there is a shim
    test_syms = set()
    tsrc = set(test_sources())
    for s_ in srcs:
        if s_ in tsrc and objs[s_]:
            test_syms |= set(xn_cload.Obj(objs[s_]).pubs)
    conv = {}
    for sym, addr in syms.items():
        if not sym.endswith("_"):
            continue
        n = sym[:-1]
        if n.endswith("_r") and n[:-2] in obj2:
            conv.setdefault(obj2[n[:-2]], {})["shim" if sym in test_syms else "glue"] = addr
        elif n.endswith("_b") and n[:-2] in obj2 and sym not in test_syms:
            conv.setdefault(obj2[n[:-2]], {})["adapter"] = addr     # the game's registers
        elif n in obj2:
            conv.setdefault(obj2[n], {})["c"] = addr
    import xn_abi
    abi = xn_abi.read_abi()
    stubs = bytearray()
    pos = RBASE + ((len(img) + 0xFF) & ~0xFF)
    routes = {}
    problems = []

    def glue_stub(target, rp):
        a = pos + len(stubs)
        b = b"\x9C\x60\x89\xE0\xE8" + struct.pack("<i", target - (a + 9)) + b"\x61\x9D"
        b += b"\xC2" + struct.pack("<H", rp) if rp else b"\xC3"
        stubs.extend(b + b"\x90" * (-len(b) % 16))
        return a
    for va in sorted(conv):
        c = conv[va]
        if "shim" in c and "glue" in c:
            problems.append("%s (%06X): both a shim (src/engine_test) and engine glue NAME_r" % (
                funcs.get(va, ("?",))[0], va))
        ret, sel = handler_entry(va) if va in abi and abi[va]["convention"] == "interrupt" \
            else (None, None)
        if ret is not None:
            # an interrupt or exception handler: its stub saves everything, loads the data
            # segments the asm loads, and returns as the asm does (iretd / retf). A canonical
            # handler's stub calls its C (the boundary: interrupt entry stays a stub)
            a = pos + len(stubs)
            b = isr_stub(a, c.get("glue") or c.get("c") or c["shim"], ret, sel)
            stubs.extend(b + b"\x90" * (-len(b) % 16))
            routes[va] = {"to": a, "kind": "isr", "c": c.get("c"), "glue": c.get("glue"),
                          "canonical": "shim" in c}
        elif "shim" in c:
            # canonical: the records reach the C through its test shim
            rp = abi[va]["ret_pop"] if va in abi else 0
            routes[va] = {"to": glue_stub(c["shim"], rp), "kind": "shim", "c": c.get("c"),
                          "glue": c["shim"], "canonical": True}
            if "adapter" in c:
                routes[va]["adapter"] = glue_stub(c["adapter"], rp)
        elif "glue" in c:
            rp = abi[va]["ret_pop"] if va in abi else 0
            routes[va] = {"to": glue_stub(c["glue"], rp), "kind": "glue", "c": c.get("c"),
                          "glue": c["glue"]}
        elif va in abi and keeps_eax(abi[va]) and not abi[va]["stack_args"]:
            # the callers need EAX kept, and Watcom's code never keeps it: a stub does
            a = pos + len(stubs)
            rp = abi[va]["ret_pop"]
            merge = eax_merge(abi[va])          # a result in AL, AH or AX: into the kept EAX
            b = b"\x50\xE8" + struct.pack("<i", c["c"] - (a + 6))
            b += (EAX_MERGE[merge] if merge else b"") + b"\x58"
            b += b"\xC2" + struct.pack("<H", rp) if rp else b"\xC3"
            stubs.extend(b + b"\x90" * (-len(b) % 16))
            routes[va] = {"to": a, "kind": "keep-eax", "c": c["c"]}
        else:
            routes[va] = {"to": c["c"], "kind": "direct", "c": c["c"]}
    # the game's routes to canonical functions: only the boundary's entries, each straight to
    # its C or through a stub that keeps what the game reads and the prototype may change
    game_routes = {}
    ents = boundary_entries()
    decl, protos = declared_interfaces()
    for va, r in sorted(routes.items()):
        if not r.get("canonical"):
            continue
        name = funcs.get(va, ("?",))[0]
        if name in decl:
            problems.append("%s (%06X): canonical, but it has a #pragma aux" % (name, va))
        if r["kind"] == "isr":
            game_routes[va] = dict(r, kind="isr")
            continue
        e = ents.get(va)
        if e is None:
            continue                    # internal only: its callers are C
        if r.get("adapter"):
            # a boundary adapter NAME_b(xn_regs *): the game's registers, for an entry whose
            # game-visible behaviour depends on more than the prototype passes (a quirk)
            game_routes[va] = {"to": r["adapter"], "kind": "boundary-adapter", "c": r["c"]}
            continue
        gr = boundary_route(va, name, e, abi.get(va), protos, r["c"])
        if gr.get("problem"):
            problems.append(gr["problem"])
        if gr.get("keep"):
            a = pos + len(stubs)
            regs = gr["keep"]
            b = bytes(PUSH[x] for x in regs)
            b += b"\xE8" + struct.pack("<i", r["c"] - (a + len(b) + 5))
            b += bytes(POP[x] for x in reversed(regs))
            b += b"\xC2" + struct.pack("<H", gr["ret_pop"]) if gr["ret_pop"] else b"\xC3"
            stubs.extend(b + b"\x90" * (-len(b) % 16))
            game_routes[va] = {"to": a, "kind": "boundary-keep", "c": r["c"], "keep": regs}
            if gr.get("evidence"):
                game_routes[va]["evidence"] = gr["evidence"]
                print("  %s (%06X): boundary-keep %s; EAX: %s" % (name, va, " ".join(regs),
                                                                 gr["evidence"]))
        else:
            game_routes[va] = {"to": r["c"], "kind": "boundary", "c": r["c"]}
    data = bytes(img) + b"\0" * (pos - RBASE - len(img)) + bytes(stubs)
    if len(data) > RSIZE:
        raise SystemExit("the image is bigger than the region (%d bytes)" % len(data))
    os.makedirs(OUT, exist_ok=True)
    with open(IMAGE, "wb") as f:
        pickle.dump({"base": RBASE, "bytes": data, "syms": syms, "routes": routes,
                     "game_routes": game_routes, "test_syms": sorted(test_syms),
                     "flags": flags, "sources": [os.path.relpath(s, ROOT) for s in srcs]}, f)
    kinds = collections.Counter(r["kind"] for r in routes.values())
    gk = collections.Counter(r["kind"] for r in game_routes.values())
    ncan = sum(1 for r in routes.values() if r.get("canonical"))
    print("compiled %d files in %.0f s; image %d bytes; %d functions converted (%s) -> %s" % (
        len(srcs), time.time() - t0, len(data), len(routes),
        ", ".join("%d %s" % (n, k) for k, n in sorted(kinds.items())), os.path.relpath(IMAGE, ROOT)))
    if ncan:
        print("  %d canonical (test shims); the game reaches %d of them (%s)" % (
            ncan, len(game_routes), ", ".join("%d %s" % (n, k) for k, n in sorted(gk.items()))))
    problems += check_routes(routes)
    problems += check_dropped()
    for p in problems:
        print("ROUTE", p)
    return 1 if problems else 0


def check_dropped():
    """Dropped memory (config/xngine_dropped.csv) must be engine-private in the boundary map:
    canonical C may stop writing only what the game never reads."""
    try:
        import xn_boundary
        rows = [r for r in xn_boundary.rows_csv() if r["kind"] == "memory" and r["space"] == "obj"]
    except (ImportError, OSError):
        return []
    out = []
    wp = None
    for d in load_dropped()[3]:
        if d["kind"].strip() != "memory":
            continue
        a = alloc_ref(d["start"])
        if a is not None:
            # relative to an allocation: the bytes must be writer-private there for this
            # function (tools/xn_scenarios.py writer_private_rules: the game never reads what
            # it wrote), whatever class their chunks have
            import xn_scenarios
            if wp is None:
                wp = xn_scenarios.writer_private_rules(resolve_code=False)
            b = alloc_ref(d["end"])
            f = func_va(d["function"].strip())
            if not any(w["alloc"] == a[0] and w["lo"] <= a[1] and b is not None and
                       b[1] <= w["hi"] and f in w["writers"] for w in wp):
                out.append("dropped %s (%s..%s, %s): not writer-private for %s in the boundary "
                           "map (class writer-private, writers=...)" % (
                               d["name"], d["start"], d["end"], d["function"], d["function"]))
            continue
        vis = [(rg, r) for r in rows if r["class"] == "visible" for rg in xn_boundary.row_ranges(r)]
        for lo, hi in xn_boundary.row_ranges(d):
            for (a, b), r in vis:
                if a < hi and b > lo:
                    out.append("dropped %s (%06X-%06X, %s) overlaps game-visible %s (%s)" % (
                        d["name"], lo, hi, d["function"], r["name"], r["evidence"][:80]))
    return out


# x86 push/pop of the 32-bit registers
PUSH = {"eax": 0x50, "ecx": 0x51, "edx": 0x52, "ebx": 0x53, "ebp": 0x55, "esi": 0x56, "edi": 0x57}
POP = {k: v + 8 for k, v in PUSH.items()}


def boundary_entries():
    """{va: entry} of config/xngine_boundary.csv (tools/xn_boundary.py), or {} without it."""
    try:
        import xn_boundary
        return xn_boundary.entries()
    except (ImportError, OSError):
        return {}


def prototype(name, protos):
    """(returns a value, number of parameters) of a plain prototype, or None."""
    params = protos.get(name)
    if params is None:
        return None
    n = 0 if params.strip() in ("", "void") else params.count(",") + 1
    return protos.get("__ret_" + name, "s32") != "void", n


def boundary_route(va, name, e, row, protos, c):
    """How the game reaches a canonical entry: {"keep": [registers the stub keeps],
    "ret_pop": N} or {"problem": text}. The game reads e["game_reads"] after its calls; a
    plain Watcom prototype may change EAX and its argument registers and keeps the rest."""
    import xn_abi
    pr = prototype(name, protos)
    if pr is None:
        return {"problem": "%s (%06X): canonical entry without a prototype" % (name, va)}
    returns, n = pr
    ret_pop = 4 * max(0, n - 4)
    gr = e["game_reads"]
    if gr in ("all", "watcom"):
        return {"ret_pop": ret_pop}     # a pointer the game calls: Watcom's convention itself
    reads = xn_abi.mask_of(gr) if gr else 0
    may = xn_abi.RMASK["eax"] | sum(xn_abi.RMASK[r] for r in xn_abi.WATCOM_ARGS[:min(n, 4)])
    result = xn_abi.RMASK["eax"] if returns else 0
    need = reads & may & ~result
    keep = [r for r in ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp") if need & xn_abi.RMASK[r]]
    out = {"ret_pop": ret_pop, "keep": keep}
    row_out, row_clob = (row["out"], row["clob"]) if row is not None else (0, 0)
    if row is not None and need & xn_abi.RMASK["eax"] & (row_out | row_clob):
        # the parts of EAX the asm gives back as they came (push ax/pop ax, pushad/popad): an
        # "output" there is the caller's value passing through, which push eax/pop eax keeps
        kept, ev = eax_preserved(va)
        if kept & need & xn_abi.RMASK["eax"]:
            row_out &= ~kept
            row_clob &= ~kept
            out["evidence"] = ev
    if row is not None and need & row_out:
        out["problem"] = "%s (%06X): the game reads %s after the call, which the asm sets (an " \
            "output the prototype does not return): a boundary adapter is needed" % (
                name, va, xn_abi.names_of(need & row_out))
    elif "eax" in keep and (row is None or
                            need & xn_abi.RMASK["eax"] & (row_out | row_clob)):
        # the game reads a part of EAX the asm changes, and the prototype returns nothing.
        # (A part the asm keeps, the stub keeps too: push eax; call; pop eax.)
        out["problem"] = "%s (%06X): the game reads EAX after the call and the prototype " \
            "returns nothing" % (name, va)
    elif keep and n > 4:
        out["problem"] = "%s (%06X): stack arguments and registers to keep" % (name, va)
    return out


# What the asm keeps of EAX: tools/xn_abi.py's analysis (maydef: the register parts some path
# may leave changed; push/pop pairs, 16-bit ones too, and pushad/popad leave a part as it came),
# cross-checked on the records (exit EAX = entry EAX in every record of the function). Cached
# by the analysis' source and inputs (XN_MAYDEF: another cache file).
MAYDEF = os.environ.get("XN_MAYDEF") or os.path.join(ROOT, "build", "xn_canon", "boundary",
                                                     "maydef.json")


def _maydef_key():
    import hashlib
    import xn_abi
    h = hashlib.sha1(open(xn_abi.__file__, "rb").read())
    for p in (xn_abi.INDIRECT, os.path.join(ROOT, "config", "functions.csv")):
        if os.path.exists(p):
            h.update(open(p, "rb").read())
    return h.hexdigest()


_maydef = None


def maydef_table():
    """{va: (maydef mask, mustdef mask)} of every analysed function (cached)."""
    global _maydef
    if _maydef is not None:
        return _maydef
    key = _maydef_key()
    if os.path.exists(MAYDEF):
        with open(MAYDEF) as f:
            d = json.load(f)
        if d.get("key") == key:
            _maydef = {int(k, 16): tuple(v) for k, v in d["maydef"].items()}
            return _maydef
    import xn_abi
    print("tools/xn_abi.py analysis for the boundary routes' EAX (30 s, cached in %s)" %
          os.path.relpath(MAYDEF, ROOT), flush=True)
    an = xn_abi.Analysis()
    an.run()
    _maydef = {va: (s.maydef, s.mustdef) for va, s in an.summ.items()}
    os.makedirs(os.path.dirname(MAYDEF), exist_ok=True)
    with open(MAYDEF + ".tmp", "w") as f:
        json.dump({"key": key, "maydef": {"%06X" % va: list(v) for va, v in _maydef.items()},
                   "records": {}}, f)
    os.replace(MAYDEF + ".tmp", MAYDEF)
    return _maydef


def eax_preserved(va):
    """(the parts of EAX the asm leaves as its caller had them, evidence): every part outside
    the analysis' maydef that also held its entry value at the exit of every record of the
    function. (0, why) when the analysis says a part may change or a record shows it did."""
    import xn_abi
    md = maydef_table().get(va)
    if md is None:
        return 0, "not analysed"
    kept = xn_abi.RMASK["eax"] & ~md[0]
    if not kept:
        return 0, "the analysis: EAX may change (maydef %s)" % xn_abi.names_of(md[0])
    with open(MAYDEF) as f:
        d = json.load(f)
    files = files_with([va])
    ck = "%06X" % va
    got = d.get("records", {}).get(ck)
    if got is None or got[0] != len(files):
        import xn_cload
        import xn_record
        n, bad = 0, 0
        for path in files:
            for rec in xn_cload.read_records(xn_record, path):
                if rec["func"] != va or not rec.get("exit"):
                    continue
                n += 1
                x = (rec["entry"]["eax"] ^ rec["exit"]["eax"]) & 0xFFFFFFFF
                for part, m in (("eaxL", 0xFF), ("eaxH", 0xFF00), ("eaxU", 0xFFFF0000)):
                    if x & m:
                        bad |= xn_abi.BIT[part]
        got = [len(files), n, bad]
        d.setdefault("records", {})[ck] = got
        with open(MAYDEF + ".tmp", "w") as f:
            json.dump(d, f)
        os.replace(MAYDEF + ".tmp", MAYDEF)
    _nf, n, bad = got
    kept &= ~bad
    if bad & xn_abi.RMASK["eax"] & ~md[0]:
        return kept, "the analysis keeps %s, but a record changed %s" % (
            xn_abi.names_of(xn_abi.RMASK["eax"] & ~md[0]), xn_abi.names_of(bad))
    return kept, "the analysis keeps %s; %d records agree" % (xn_abi.names_of(kept), n)


# --------------------------------------------------------------------------------------------
# Routing

class RImage:
    """The readable C in a machine: the image at RBASE, and code hooks that send asm entries
    to their C (the interface xn_cload.replay_c / diff_test / play expect of an image)."""

    def __init__(self, path=IMAGE):
        with open(path, "rb") as f:
            d = pickle.load(f)
        self.bytes, self.syms = d["bytes"], d["syms"]
        self.routes = d["routes"]
        self.game = d.get("game_routes", {})
        self.funcs = {va: r["to"] for va, r in self.routes.items()}
        # how the game runs: every function on its route, but the canonical ones only at the
        # boundary (their callers are C)
        self.game_funcs = {va: r["to"] for va, r in self.routes.items() if not r.get("canonical")}
        self.game_funcs.update({va: r["to"] for va, r in self.game.items()})
        self.canonical = {va for va, r in self.routes.items() if r.get("canonical")}
        self.mode = "test"
        self.hits = 0
        self.entered = set()

    def install(self, emu):
        uc = emu.uc
        if not getattr(emu, "_xn_rc_mapped", False):
            uc.mem_map(RBASE, RSIZE)
            emu._xn_rc_mapped = True
        uc.mem_write(RBASE, self.bytes)         # the data and BSS as built, every time
        self.adopt(emu)

    def adopt(self, emu):
        """Seed canonical C state from the asm's: config/xngine_adopt.csv (and the files in
        XN_ADOPT) map a C variable to the asm's bytes that hold the same value (c_symbol,
        asm_address, size, reason). A machine made from asm state (a record's, a snapshot's)
        has run the asm's initialisation, not the C's: the C's own storage starts as built,
        so state an earlier call set (the 1/z table, the view) is taken from where the asm
        keeps it. A row's `kind` is `copy` (the default: the bytes as they are) or `codeptr`
        (a dword holding an asm entry's address becomes that function's C address). A
        test-harness migration, never part of the engine."""
        for r in adopt_rows():
            name = r["c_symbol"].strip()
            a = next((self.syms[n] for n in ("_" + name, name, name + "_") if n in self.syms), None)
            if a is None:
                continue                # not in this image (another group's, not built)
            n = int(r["size"], 0)
            data = bytes(emu.uc.mem_read(LOAD + int(r["asm_address"], 16), n))
            if (r.get("kind") or "copy").strip() == "codeptr":
                # a dword holding an asm entry's address (0: none) becomes the C address of
                # that entry's function
                v = struct.unpack("<I", data[:4])[0]
                if v:
                    rt = self.routes.get(v - LOAD)
                    if rt is None or rt.get("c") is None:
                        raise SystemExit("adopt %s: %08X is no converted function's entry" %
                                         (name, v))
                    v = rt["c"]
                data = struct.pack("<I", v)
            emu.uc.mem_write(a, data)

    def game_routes(self):
        """The functions routed when the game runs (route_game)."""
        return sorted(self.game_funcs)

    def route_game(self, emu):
        """Route as the game runs: the boundary routes of canonical functions, the others' as
        in the tests."""
        old, self.mode = self.mode, "game"
        try:
            return self.route(emu, self.game_routes())
        finally:
            self.mode = old

    def route(self, emu, funcs):
        from unicorn import UC_HOOK_CODE
        import fallemu
        hooks = []
        eip = fallemu.R["eip"]
        table = self.game_funcs if self.mode == "game" else self.funcs
        for va in funcs:
            to = table.get(va)
            if to is None:
                continue

            def hit(uc, address, size, to, va=va):
                self.hits += 1
                self.entered.add(va)
                uc.reg_write(eip, to)
            a = LOAD + va
            hooks.append(emu.uc.hook_add(UC_HOOK_CODE, hit, to, a, a))
        return hooks

    @staticmethod
    def unroute(emu, hooks):
        for hk in hooks:
            emu.uc.hook_del(hk)


PRAGMA_RE = re.compile(r"#pragma\s+aux\s+(\w+)\s+(?!=)(.*?);", re.S)


def declared_interfaces():
    """{function name: (parm registers, value register, modify registers, caller/routine)}
    from the #pragma aux lines of src/engine/*.h and *.c (and the XN_RC_SRC folders', which
    replace files of the same name); prototypes without one: Watcom's."""
    out = {}
    protos = {}
    for p in _files("*.h") + _files("*.c"):
        text = re.sub(r"/\*.*?\*/", " ", open(p, errors="replace").read(), flags=re.S)
        text = text.replace("\\\n", " ")
        for m in PRAGMA_RE.finditer(text):
            name, body = m.group(1), " ".join(m.group(2).split())
            if body.startswith("="):
                continue
            parm = re.search(r"parm\s+(?:(routine|caller)\s*)?((?:\[[^\]]*\]\s*)*)", body)
            value = re.search(r"value\s+\[([^\]]*)\]", body)
            modify = re.search(r"modify\s+(?:exact\s+)?\[([^\]]*)\]", body)
            regs = re.findall(r"\[([^\]]*)\]", parm.group(2)) if parm else []
            out[name] = (regs, value.group(1).strip() if value else None,
                         modify.group(1).split() if modify else [],
                         parm.group(1) if parm and parm.group(1) else "routine")
        for m in re.finditer(r"^([\w\s\*]*?)\b(\w+)\s*\(([^;{)]*)\)\s*;", text, re.M):
            ret = " ".join(m.group(1).split())
            # a declaration has a type before the name (a call statement does not); the
            # headers come first and win over the .c files
            if not ret or ret.split()[0] in ("return", "else", "case", "goto", "do") or \
                    m.group(2) in protos:
                continue
            protos[m.group(2)] = m.group(3)
            protos["__ret_" + m.group(2)] = ret
    return out, protos


def check_routes(routes):
    """Problems with the routing: a direct route whose ABI row needs glue, or whose declared
    interface may change a register the row says callers read."""
    import xn_abi
    abi = xn_abi.read_abi()
    decl, protos = declared_interfaces()
    _b, funcs = names()
    out = []
    for va, r in sorted(routes.items()):
        row = abi.get(va)
        name = funcs.get(va, ("?",))[0]
        if row is None:
            continue
        if r["kind"] in ("glue", "isr", "shim"):
            continue
        # segment registers are not the C's concern: Watcom's code never changes them (the
        # record test still sees a function whose asm does)
        outs = row["out"] & ~xn_abi.SEG
        if row["fout"] or bin(outs).count("1") and len(xn_abi.names_of(outs).split()) > 1:
            out.append("%s (%06X): outputs %s%s need a glue function %s_r" % (
                name, va, row["outputs"], " flags " + row["flags_out"] if row["flags_out"] else "",
                name))
            continue
        if keeps_eax(row) and row["stack_args"]:
            out.append("%s (%06X): its callers need EAX kept and it takes stack arguments: "
                       "write the glue %s_r" % (name, va, name))
            continue
        may = 0
        if name in decl:
            parm, value, modify, _how = decl[name]
            for reg in modify + ([value] if value else []):
                may |= xn_abi.mask_of(reg) if reg in xn_abi.NAME else 0
            pregs = set()
            for g in parm:
                pregs |= set(g.split())
            need = set(xn_abi.names_of(row["in"] & ~xn_abi.SEG).split())
            # a part (dl, ax) is covered by its whole register (edx, eax) as a parameter
            pmask = sum(xn_abi.mask_of(g) for g in pregs if g in xn_abi.NAME)
            missing = [g for g in need if g not in pregs and not g.endswith(".u") and
                       not (g in xn_abi.NAME and xn_abi.mask_of(g) & ~pmask == 0)]
            if missing:
                print("note: %s (%06X): the row's inputs %s are not all parameters (%s): fine "
                      "when the C provably does not need them (say why in its comment)" % (
                          name, va, row["inputs"], " ".join(sorted(pregs))))
        else:
            params = protos.get(name, "")
            n = 0 if params.strip() in ("", "void") else params.count(",") + 1
            may = xn_abi.RMASK["eax"] | sum(xn_abi.RMASK[g] for g in xn_abi.WATCOM_ARGS[:min(n, 4)])
        if r["kind"] == "keep-eax":
            may &= ~xn_abi.RMASK["eax"]         # the stub keeps it
        may &= ~xn_abi.SEG
        bad = may & ~(row["clob"] | row["out"])
        if bad:
            out.append("%s (%06X): may change %s, which callers read (clobber: %s)" % (
                name, va, xn_abi.names_of(bad), row["clobber"] or "none"))
    return out


def route_table():
    img = RImage()
    _b, funcs = names()
    for va, r in sorted(img.routes.items()):
        print("%06X %-36s %-6s -> %08X%s" % (va, funcs.get(va, ("?",))[0], r["kind"], r["to"],
                                             "  (C %08X)" % r["c"] if r.get("c") and
                                             r["kind"] in ("glue", "isr") else ""))
    problems = check_routes(img.routes)
    for p in problems:
        print("ROUTE", p)
    print("%d functions routed, %d problems" % (len(img.routes), len(problems)))
    return 1 if problems else 0


# --------------------------------------------------------------------------------------------
# Tests

def load_dropped(path=DROPPED_CSV):
    """config/xngine_dropped.csv (and the files in XN_DROPPED): what canonical C no longer
    writes on purpose.
    Returns (memory: a sorted list of linear (lo, hi), registers: {va: mask}, flags: {va: mask},
    rows). Memory rows are excused in every record (a canonical function's scratch is
    nobody's input); register and flag rows only in the records of their function."""
    import xn_abi
    mem, regs, flags, rows = [], {}, {}, []
    # XN_DROPPED=path[:path]: an agent's own rows, read after config/xngine_dropped.csv's
    paths = [path] + ([q for q in os.environ.get("XN_DROPPED", "").split(os.pathsep) if q]
                      if path == DROPPED_CSV else [])
    for q in paths:
        if os.path.exists(q):
            with open(q, newline="") as f:
                rows += list(csv.DictReader(f))
    for r in rows:
        kind = r["kind"].strip()
        if kind == "memory-own":
            continue                # excused only in their function's records (own_drops)
        if kind == "memory":
            if alloc_ref(r["start"]) is None:
                import xn_boundary
                mem += [(LOAD + lo, LOAD + hi) for lo, hi in xn_boundary.row_ranges(r)]
            continue                # (ALLOC+0xOFF: relative to an allocation, alloc_drops)
        if kind == "exceptions":
            # CPU exceptions the engine's own handler took (a divide error and the return
            # from its handler): flags[None] lists the vectors excused everywhere
            flags.setdefault(None, set()).update(int(x, 16) for x in r["name"].split())
            continue
        va = func_va(r["function"].strip())
        m = xn_abi.mask_of(r["name"])
        if kind == "register":
            regs[va] = regs.get(va, 0) | m
        elif kind == "flags":
            flags[va] = flags.get(va, 0) | m
    return merge_ranges(mem), regs, flags, rows


ADOPT_CSV = os.path.join(ROOT, "config", "xngine_adopt.csv")


def adopt_rows():
    """config/xngine_adopt.csv and the files in XN_ADOPT=path[:path] (RImage.adopt)."""
    rows = []
    for q in [ADOPT_CSV] + [q for q in os.environ.get("XN_ADOPT", "").split(os.pathsep) if q]:
        if os.path.exists(q):
            with open(q, newline="") as f:
                rows += list(csv.DictReader(f))
    return rows


def merge_ranges(ranges):
    """Sorted, with overlapping and touching (lo, hi) ranges merged, so that _in_ranges'
    search by start finds every address (the rows' order and overlaps do not matter)."""
    out = []
    for lo, hi in sorted(ranges):
        if out and lo <= out[-1][1]:
            out[-1] = (out[-1][0], max(out[-1][1], hi))
        else:
            out.append((lo, hi))
    return out


# A dropped row's start and end can be relative to an engine allocation: ALLOC+0xOFF, ALLOC
# named as config/xngine_boundary.csv names it (the global that holds its pointer, e.g.
# xn_mem_work_block+0x8). Such rows are excused only in the records of their own function (a
# shared buffer: the excuse must not hide another function's writes there).
ALLOC_REF = re.compile(r"^\s*([A-Za-z_]\w*)\s*\+\s*(0x[0-9A-Fa-f]+|[0-9]+)\s*$")


def alloc_ref(text):
    """'ALLOC+0xOFF' -> (ALLOC, offset), or None (a plain object-2 address)."""
    m = ALLOC_REF.match(text or "")
    return (m.group(1), int(m.group(2), 0)) if m else None


def alloc_pointer(name):
    """The object-2 global holding the pointer of the allocation `name` (its boundary row's
    rule ptr=VA), or the global of that name."""
    import xn_boundary
    for r in xn_boundary.rows_csv():
        if r["kind"] == "memory" and r["space"] == "alloc" and r["name"] == name:
            m = re.match(r"ptr=([0-9A-Fa-f]+)", r["rule"])
            if m:
                return int(m.group(1), 16)
    e = names()[0].get(name)
    if e is not None and e[0] == "global":
        return e[1]
    raise SystemExit("dropped row: no allocation or global named %s" % name)


def alloc_drops(rows=None):
    """{function va: [(pointer global va, lo, hi)]} of the allocation-relative dropped rows."""
    out = {}
    for r in (rows if rows is not None else load_dropped()[3]):
        if r["kind"].strip() != "memory":
            continue
        a, b = alloc_ref(r["start"]), alloc_ref(r["end"])
        if a is None:
            continue
        if b is None or b[0] != a[0]:
            raise SystemExit("dropped row %s: start %s and end %s must name the same allocation" % (
                r["name"], r["start"], r["end"]))
        out.setdefault(func_va(r["function"].strip()), []).append((alloc_pointer(a[0]), a[1], b[1]))
    return out


def own_drops(rows=None):
    """{function va: [(lo, hi)]} of the `memory-own` dropped rows: preferred addresses (any
    object's) excused only in the records of their function: what a deviation's own calls
    change in memory nobody reads (D-VID-01: MemCheck's last-call record)."""
    out = {}
    for r in (rows if rows is not None else load_dropped()[3]):
        if r["kind"].strip() == "memory-own":
            import xn_boundary
            out.setdefault(func_va(r["function"].strip()), []).extend(
                (LOAD + lo, LOAD + hi) for lo, hi in xn_boundary.row_ranges(r))
    return out


def code_pointers(img):
    """{the C address of a boundary entry: its asm entry's linear address}: the code pointers
    canonical C hands the game (a callback) count as the asm's when they name the same entry.
    Only the boundary's entries (config/xngine_boundary.csv kind entry)."""
    out = {}
    ents = boundary_entries()
    for va, r in getattr(img, "routes", {}).items():
        if va in ents and r.get("c"):
            out[r["c"]] = LOAD + va
    return out


def rec_byte(rec, lin, _cache={}):
    """A byte of a record's memory before the call (its pages, else its base machine)."""
    import xn_record
    if _cache.get("rec") is not rec:        # the record itself, held (an id can be reused)
        _cache.clear()
        _cache["rec"] = rec
        _cache["pages"] = pickle.loads(__import__("zlib").decompress(rec["pages"]))
    pages = _cache["pages"]
    pg = lin & ~0xFFF
    if pg in pages:
        return pages[pg][lin - pg]
    _emu, (low, mem) = xn_record.base_machine(rec["base"])
    return mem[lin - LOAD] if lin >= LOAD else low[lin]


def translate_code_pointers(rec, gw, cps):
    """The C's writes gw with each code pointer of cps (a dword written whole, any alignment)
    put back as the asm entry's address it stands for, where the record shows the asm leaving
    that entry's asm address there (so data that merely equals a C address, as WOODS.WLD's
    cell bytes can, is not taken for a pointer); a byte that then holds what memory held
    before the call is no write (the record lists only bytes that changed)."""
    if not cps:
        return gw
    rw = rec.get("writes", {})
    out = dict(gw)
    for a in sorted(gw):
        if a not in out or any((a + k) not in gw for k in range(4)):
            continue
        v = gw[a] | gw[a + 1] << 8 | gw[a + 2] << 16 | gw[a + 3] << 24
        if v not in cps:
            continue
        want = struct.pack("<I", cps[v])
        if any(rw.get(a + k, rec_byte(rec, a + k)) != want[k] for k in range(4)):
            continue                # the asm did not leave the entry's address here: data
        for k, byte in enumerate(struct.pack("<I", cps[v])):
            if byte == rec_byte(rec, a + k):
                out.pop(a + k, None)
            else:
                out[a + k] = byte
    return out


def rec_dword(rec, lin, _cache={}):
    """A dword of a record's entry memory: from its own pages, else its base machine's."""
    import xn_record
    if _cache.get("rec") is not rec:        # the record itself, held (an id can be reused)
        _cache.clear()
        _cache["rec"] = rec
    key = lin
    if key in _cache:
        return _cache[key]
    pages = pickle.loads(__import__("zlib").decompress(rec["pages"]))
    pg = lin & ~0xFFF
    if pg in pages and lin - pg <= 0xFFC:
        v = struct.unpack_from("<I", pages[pg], lin - pg)[0]
    else:
        _emu, (low, mem) = xn_record.base_machine(rec["base"])
        v = struct.unpack_from("<I", mem, lin - LOAD)[0] if lin >= LOAD else \
            struct.unpack_from("<I", low, lin)[0]
    if len(_cache) > 64:
        _cache.clear()
        _cache["rec"] = rec
    _cache[key] = v
    return v


def rec_alloc_ranges(rec, drops):
    """The linear ranges of a record's function's allocation-relative dropped rows."""
    return [(rec_dword(rec, LOAD + ptr) + lo, rec_dword(rec, LOAD + ptr) + hi)
            for ptr, lo, hi in drops.get(rec["func"], ())]


def _in_ranges(ranges, starts, a):
    import bisect
    k = bisect.bisect_right(starts, a) - 1
    return k >= 0 and a < ranges[k][1]


def drop_page_locks(log):
    """The I/O log without DPMI page locking (int 31h 0600h/0601h and their returns): it has
    no effect the game or the machine can see (D-VID-01: the asm VID player left its locks in
    place, the canonical C unlocks them)."""
    import fallemu
    out, skip = [], False
    for e in log:
        if skip and e[0] == "int-ret":
            skip = False
            continue
        skip = False
        if e[0] == "int" and fallemu.is_page_lock(e[1], e[2]):
            skip = True
            continue
        out.append(e)
    return out


def service_ax(e):
    """An I/O log entry as compared: a service call ("int", vector, eax) by AX, or by AH alone
    for a service whose AL is no input (xn_services.AH_ONLY). Every DOS, DPMI, BIOS and mouse
    service the engine and the game call takes its function in AH or AX (tools/xn_services.py
    keys on them), so EAX's upper half is whatever the caller left there, which canonical C
    keeps differently from the asm."""
    if e[0] != "int":
        return e
    import xn_services
    return (e[0], e[1], e[2] & xn_services.eax_mask(e[1], e[2]))   # AH alone when AL is no input


def comparer(abi, dropped=None, img=None):
    """compare(rec, got) per function: tools/xn_abi.py's abi_compare by the record's function's
    row, with config/xngine_dropped.csv's excuses (memory everywhere; registers and flags in
    their function's records)."""
    import xn_abi
    mem, regs, flags, _rows = dropped if dropped is not None else load_dropped()
    starts = [lo for lo, _hi in mem]
    excs = flags.get(None, set())
    adrops = alloc_drops(_rows)
    odrops = own_drops(_rows)
    cps = code_pointers(img) if img is not None else {}

    def drop(writes):
        return {a: v for a, v in writes.items() if not _in_ranges(mem, starts, a)}

    def io(log):
        return [service_ax(e) for e in drop_page_locks(log)
                if not (e[0] == "exc" and e[1] in excs)]

    def for_func(va):
        row = abi.get(va)

        def cmp(rec, got, row=row):
            r = abi.get(rec["func"], row)
            x, fl = regs.get(rec["func"], 0), flags.get(rec["func"], 0)
            if r is not None and (x or fl):
                r = dict(r, clob=r["clob"] | x, out=r["out"] & ~x, fout=r["fout"] & ~fl)
            if mem and "writes" in got:
                rec = dict(rec, writes=drop(rec["writes"]))
                got = dict(got, writes=drop(got["writes"]))
            if cps and "writes" in got:
                got = dict(got, writes=translate_code_pointers(rec, got["writes"], cps))
            if rec["func"] in odrops and "writes" in got:
                own = odrops[rec["func"]]

                def odrop(ws, own=own):
                    return {a: v for a, v in ws.items() if not any(lo <= a < hi for lo, hi in own)}
                rec = dict(rec, writes=odrop(rec["writes"]))
                got = dict(got, writes=odrop(got["writes"]))
            if rec["func"] in adrops and "writes" in got:
                ar = rec_alloc_ranges(rec, adrops)

                def adrop(ws, ar=ar):
                    return {a: v for a, v in ws.items() if not any(lo <= a < hi for lo, hi in ar)}
                rec = dict(rec, writes=adrop(rec["writes"]))
                got = dict(got, writes=adrop(got["writes"]))
            if "io" in got:
                rec = dict(rec, io=io(rec["io"]))
                got = dict(got, io=io(got["io"]))
            return xn_abi.abi_compare(rec, got, r)
        return cmp
    return for_func


# ---- the boundary ------------------------------------------------------------------------------
OBJ1 = (0x10000, 0xBB27F)
OBJ2 = (0xC0000, 0x161568)
TRAP = 0xBBF00


def record_return(rec):
    """The return address a record's call was made with (its entry ESP's dword)."""
    import xn_record
    esp = rec["entry"]["esp"]
    pages = pickle.loads(__import__("zlib").decompress(rec["pages"]))
    pg = esp & ~0xFFF
    if pg in pages and esp - pg <= 0xFFC:
        return struct.unpack_from("<I", pages[pg], esp - pg)[0]
    _emu, (low, mem) = xn_record.base_machine(rec["base"])
    if esp >= LOAD:
        return struct.unpack_from("<I", mem, esp - LOAD)[0]
    return struct.unpack_from("<I", low, esp)[0]


def caller_kind(rec):
    """"game" (a return into object 1), "direct" (a call from the safe point: the game's
    side), "vector" (an interrupt or exception frame), "engine" (object 2) or "other"."""
    if rec.get("via") in ("handler", "interrupt"):
        return "vector"
    ret = (record_return(rec) - LOAD) & 0xFFFFFFFF
    if ret == TRAP:
        return "direct"
    if OBJ1[0] <= ret < OBJ1[1]:
        return "game"
    if OBJ2[0] <= ret < OBJ2[1] or ret >= 0x200000:
        return "engine"
    return "other"


def install_dos_write_log():
    """Log the bytes of each DOS write (int 21h AH=40h) in the I/O log: what a file gets is
    game-visible. (Process-wide: the boundary tests' workers.)"""
    import fallemu
    if getattr(fallemu.Emu, "_dos_write_logged", False):
        return
    orig = fallemu.Emu.on_int

    def on_int(self, uc, intno, user, _orig=orig):
        if self.io_log is not None and intno == 0x21 and self.r("ah") == 0x40:
            n = self.r("ecx")
            self.io_log.append(("dos-write", self.r("bx"), bytes(self.read(self.ds_edx(), n))))
        return _orig(self, uc, intno, user)
    fallemu.Emu.on_int = on_int
    fallemu.Emu._dos_write_logged = True


def boundary_compare(rec, got, entry, priv, row, ref_io=None):
    """The differences the game can see: the registers it reads after the call (entry's
    game_reads; all of them for an interrupt or exception handler; EAX when the row outputs it
    and ESI EDI EBP for a call through a pointer), ESP, the memory written but the dead stack
    and the engine's private memory, the I/O but CPU exceptions."""
    import xn_abi
    diffs = []
    if not got["returned"]:
        diffs.append("did not return")
    gr = entry["game_reads"]
    if gr == "all":
        # an interrupt or exception handler: the interrupted code gets every register back;
        # its flags come from the frame (iretd, or the DPMI host after a retf), not from what
        # the handler leaves
        masks = {r: 0xFFFFFFFF for r in xn_abi.EXIT_REGS}
        fm = 0
    else:
        if gr == "watcom":
            m = xn_abi.mask_of("esi edi ebp") | (row["out"] & xn_abi.RMASK["eax"] if row else 0)
        else:
            m = xn_abi.mask_of(gr) if gr else 0
        masks = {r: xn_abi.value_mask(r, m) for r in xn_abi.EXIT_REGS[:-1]}
        masks["esp"] = 0xFFFFFFFF
        fm = 0
    for r, mk in masks.items():
        if (got["exit"][r] ^ rec["exit"][r]) & mk:
            diffs.append("%s %08X != %08X (the game reads it)" % (r, got["exit"][r], rec["exit"][r]))
    if fm and (got["exit"]["eflags"] ^ rec["exit"]["eflags"]) & fm:
        diffs.append("flags %03X != %03X" % (got["exit"]["eflags"] & fm, rec["exit"]["eflags"] & fm))
    lo, hi = rec["exit"]["esp"] - xn_abi.STACK_DEAD, rec["exit"]["esp"]

    def vis(ws):
        return {a: v for a, v in ws.items() if not lo <= a < hi and not (priv and priv.contains(a))}
    gw, rw = vis(got["writes"]), vis(rec["writes"])
    if gw != rw:
        extra = set(gw) ^ set(rw)
        wrong = [a for a in set(gw) & set(rw) if gw[a] != rw[a]]
        first = min(extra | set(wrong))
        diffs.append("game-visible writes: %d addresses differ in the set, %d in value (first %#x)"
                     % (len(extra), len(wrong), first))
    io_g = [service_ax(e) for e in drop_page_locks(got["io"]) if e[0] != "exc"]
    io_r = [service_ax(e) for e in drop_page_locks(ref_io if ref_io is not None else rec["io"])
            if e[0] != "exc"]
    if io_g != io_r:
        diffs.append("port or DOS I/O differs")
    return diffs


def _rc_boundary_task(files, sp):
    """Replay the records of the boundary's entries whose caller is the game, each entry routed
    as the game reaches it (all of them with sp["together"])."""
    import xn_abi
    import xn_boundary
    import xn_cload
    import xn_record
    xn_cload.longer_calls(xn_record)
    install_dos_write_log()
    abi = xn_abi.read_abi()
    ents = boundary_entries()
    masks = xn_boundary.load_masks()
    img = RImage()
    img.mode = "game"
    want = set(sp["funcs"]) if sp.get("funcs") else None
    every = img.game_routes()
    stats, callers = {}, collections.Counter()
    adrops = alloc_drops()
    odrops = own_drops()
    cps = code_pointers(img)
    for path in files:
        for rec in xn_cload.read_records(xn_record, path):
            va = rec["func"]
            if va not in ents or (want is not None and va not in want) or va not in img.game_funcs:
                continue
            if sp.get("max_per") and stats.get(va, [0, 0])[1] >= sp["max_per"]:
                continue
            try:
                kind = caller_kind(rec)
            except Exception:       # noqa: BLE001
                kind = "other"
            callers[kind] += 1
            if kind in ("engine", "other"):
                continue
            ref_io = None
            if any(e[0] == "int" and e[1] == 0x21 and (e[2] >> 8) & 0xFF == 0x40 for e in rec["io"]):
                got_a = xn_record.replay_run(rec)       # the asm: what its DOS writes wrote
                ref_io = got_a.get("io")
            route = every if sp.get("together") else [va]
            hooks = []

            def patch(emu, route=route):
                img.install(emu)
                hooks.extend(img.route(emu, route))
            try:
                got = xn_record.replay_run(rec, patch=patch)
                emu = xn_cload.machine_of(xn_record, rec["base"])
                priv = masks.resolve(emu) if masks is not None and emu is not None else None
                if va in adrops or va in odrops:
                    # the dropped bytes of this function's own rows (allocation-relative,
                    # memory-own)
                    priv = xn_boundary.Private((list(priv.ranges) if priv is not None else []) +
                                               rec_alloc_ranges(rec, adrops) + odrops.get(va, []))
                if cps and "writes" in got:
                    got = dict(got, writes=translate_code_pointers(rec, got["writes"], cps))
                diffs = [got["stopped"]] if "stopped" in got else                     boundary_compare(rec, got, ents[va], priv, abi.get(va), ref_io)
            except Exception as e:      # noqa: BLE001
                diffs = ["error: %r" % e]
                xn_cload.drop_machine(xn_record, rec["base"])
            finally:
                emu = xn_cload.machine_of(xn_record, rec["base"])
                if emu is not None:
                    img.unroute(emu, hooks)
            st = stats.setdefault(va, [0, 0, [], 0])
            st[1] += 1
            if not diffs:
                st[0] += 1
            elif len(st[2]) < 3:
                st[2].append(["%s@%d" % (rec.get("job", ""), rec.get("tick", 0))] + diffs[:4])
            if diffs and sp.get("verbose"):
                print("FAIL %06X: %s" % (va, "; ".join(diffs[:4])), flush=True)
    return {"stats": stats, "hits": img.hits, "callers": dict(callers)}


def _rc_test_task(files, sp):
    """Replay the records of these files: those of sp["funcs"] (each with its own function
    routed, or all of sp["route"] with sp["together"]), or every record (sp["corpus"])."""
    import xn_abi
    import xn_cload
    import xn_record
    xn_cload.longer_calls(xn_record)
    abi = xn_abi.read_abi()
    img = RImage()
    want = set(sp["funcs"]) if sp.get("funcs") else None
    every = sorted(img.funcs)
    stats, seen = {}, collections.Counter()
    cmp = comparer(abi, img=img)
    for path in files:
        for rec in xn_cload.read_records(xn_record, path):
            va = rec["func"]
            if not sp.get("corpus") and (want is None or va not in want or va not in img.funcs):
                continue
            if sp.get("max_per") and seen[va] >= sp["max_per"]:
                continue
            seen[va] += 1
            img.entered = set()
            route = every if sp.get("together") or sp.get("corpus") else [va]
            diffs = xn_cload.replay_c(xn_record, img, rec, route, cmp(va))
            st = stats.setdefault(va, [0, 0, [], 0])
            st[1] += 1
            st[3] += bool(img.entered)
            if not diffs:
                st[0] += 1
            elif len(st[2]) < 3:
                st[2].append(["%s@%d" % (rec.get("job", ""), rec.get("tick", 0))] + diffs[:4])
            if diffs and sp.get("verbose"):
                print("FAIL %06X: %s" % (va, "; ".join(diffs[:4])), flush=True)
    return {"stats": stats, "hits": img.hits}


def files_with(funcs):
    """The record files holding records of these functions (the job summaries list them)."""
    import xn_cload
    want = {"%X" % f for f in funcs}
    out = []
    for sp in sorted(p for d in xn_cload.record_dirs() for p in glob.glob(os.path.join(d, "*.json"))):
        try:
            sm = json.load(open(sp))
        except (OSError, ValueError):
            continue
        if any(r["func"] in want for r in sm.get("records", ())):
            out.append(sp[:-5] + ".pkl")
    return out


def test(funcs, together=False, corpus=False, jobs=None, max_per=0, verbose=False,
         boundary=False):
    import xn_cload
    t0 = time.time()
    img = RImage()
    if boundary:
        return test_boundary(funcs, together, jobs, max_per, verbose)
    if corpus:
        files = xn_cload.record_files(None)
        funcs = None
    else:
        funcs = [f for f in (funcs or sorted(img.funcs)) if f in img.funcs]
        if not funcs:
            print("no converted function to test")
            return 1
        files = files_with(funcs)
    spec = {"funcs": funcs, "together": together, "corpus": corpus, "max_per": max_per,
            "verbose": verbose}
    res = xn_cload.parallel("xn_rc:_rc_test_task", files, spec, jobs)
    stats, hits = {}, 0
    for r in res:
        hits += r["hits"]
        for k, v in r["stats"].items():
            st = stats.setdefault(int(k), [0, 0, [], 0])
            for j in (0, 1, 3):
                st[j] += v[j]
            st[2] += v[2]
    mode = "corpus" if corpus else ("together" if together else "alone")
    save_results(mode, stats)
    _b, names_ = names()
    npass = sum(1 for s in stats.values() if s[0] == s[1])
    print("%s: %d / %d functions pass all their records (%d / %d records) in %.0f s; C entered "
          "%d times" % (mode, npass, len(stats), sum(s[0] for s in stats.values()),
                        sum(s[1] for s in stats.values()), time.time() - t0, hits))
    if corpus:
        ran = sum(s[3] for s in stats.values())
        print("  %d records ran some C" % ran)
    for va, s in sorted(stats.items()):
        if s[0] != s[1]:
            print("  %06X %-34s %d/%d  %s" % (va, names_.get(va, ("?",))[0], s[0], s[1],
                                             " | ".join("; ".join(d) for d in s[2][:2])[:300]))
    if not corpus:
        missing = [f for f in funcs if f not in stats]
        if missing:
            print("  no records: %s (run diff)" % " ".join("%06X" % f for f in missing))
    write_report()
    return 0 if npass == len(stats) else 1


def test_boundary(funcs, together=False, jobs=None, max_per=0, verbose=False):
    """`test --boundary`: the boundary's entries, as the game calls them."""
    import xn_cload
    t0 = time.time()
    img = RImage()
    ents = boundary_entries()
    if not ents:
        print("no boundary map: run tools/xn_boundary.py (static, survey, write)")
        return 1
    cand = [va for va in (funcs or sorted(ents)) if va in ents and va in img.game_funcs]
    if not cand:
        print("none of these functions is a converted boundary entry")
        return 1
    files = files_with(cand)
    spec = {"funcs": cand, "together": together, "max_per": max_per, "verbose": verbose}
    res = xn_cload.parallel("xn_rc:_rc_boundary_task", files, spec, jobs)
    stats, callers = {}, collections.Counter()
    for r in res:
        callers.update(r.get("callers", {}))
        for k, v in r["stats"].items():
            st = stats.setdefault(int(k), [0, 0, [], 0])
            st[0] += v[0]
            st[1] += v[1]
            st[2] += v[2]
    save_results("boundary", stats)
    _b, names_ = names()
    npass = sum(1 for s_ in stats.values() if s_[0] == s_[1])
    print("boundary: %d / %d entries pass all their game-called records (%d / %d records) in "
          "%.0f s" % (npass, len(stats), sum(s_[0] for s_ in stats.values()),
                      sum(s_[1] for s_ in stats.values()), time.time() - t0))
    print("  records by caller: %s (only the game's are the boundary's)" % ", ".join(
        "%s %d" % kv for kv in sorted(callers.items())))
    for va, s_ in sorted(stats.items()):
        kind = img.game.get(va, img.routes.get(va, {})).get("kind", "?")
        if s_[0] != s_[1] or verbose:
            print("  %06X %-34s %-13s %d/%d  %s" % (va, names_.get(va, ("?",))[0], kind, s_[0], s_[1],
                                                   " | ".join("; ".join(d) for d in s_[2][:2])[:300]))
    missing = [f for f in cand if f not in stats]
    if missing and verbose:
        print("  no game-called records: %s" % " ".join("%06X" % f for f in missing))
    write_report()
    return 0 if npass == len(stats) else 1


def save_results(mode, stats):
    res = load_results()
    d = res.setdefault(mode, {})
    for va, s in stats.items():
        d["%06X" % va] = [s[0], s[1], "; ".join(s[2][0][1:3]) if s[2] else "", s[3]]
    res.setdefault("image", {})["built"] = os.path.getmtime(IMAGE)
    os.makedirs(OUT, exist_ok=True)
    with open(RESULTS, "w") as f:
        json.dump(res, f, indent=0)


def load_results():
    if os.path.exists(RESULTS):
        with open(RESULTS) as f:
            return json.load(f)
    return {}


def diff(funcs, untested=False, trials=8, verbose=False):
    import xn_abi
    import xn_cload
    img = RImage()
    funcs = funcs or sorted(img.funcs)
    if untested:
        res = load_results().get("alone", {})
        funcs = [f for f in funcs if not res.get("%06X" % f, [0, 0])[1]]
    funcs = [f for f in funcs if f in img.funcs]
    if not funcs:
        print("nothing to test")
        return 0
    abi = xn_abi.read_abi()
    out = {}

    def report(res):
        out.update(res)
        r = load_results()
        d = r.setdefault("diff", {})
        for va, v in res.items():
            d["%06X" % va] = list(v)
        with open(RESULTS, "w") as f:
            json.dump(r, f, indent=0)
    rc = xn_cload.diff_test(funcs, trials=trials, verbose=verbose, img=img,
                            compare_for=comparer(abi), report=report,
                            fail_dir=os.path.join(OUT, "fail"))
    write_report()
    return rc


def play(snap, ticks, shot=None, asm=False, compare=False, script=""):
    import xn_cload
    if compare:
        import fallemu
        screens = []
        for use_c in (False, True):
            img = RImage() if use_c else None
            path = snap if os.path.exists(snap) else os.path.join(fallemu.SNAPS, snap + ".snap")
            emu = fallemu.Emu.load(path, overlay=os.path.join(OUT, "overlay_play"))
            try:
                if img:
                    img.install(emu)
                    img.route(emu, sorted(img.funcs))
                start = emu.ticks
                ok = emu.run(start + ticks, [(start + t, a, g) for t, a, g in
                                             fallemu.parse_script(script)])
                screens.append((ok, emu.mode, bytes(emu.read(0xA0000, 64000)),
                                bytes(bytearray(sum((list(c) for c in emu.palette), [])))
                                if hasattr(emu, "palette") else b"", img.hits if img else 0))
                if shot:
                    p = shot if not use_c else shot.replace(".png", "") + "_c.png"
                    if not use_c:
                        p = shot.replace(".png", "") + "_asm.png"
                    emu.screenshot(p)
            finally:
                emu.close()
        (ok1, m1, s1, p1, _h), (ok2, m2, s2, p2, hits) = screens
        same = s1 == s2 and p1 == p2
        diff_px = sum(1 for a, b in zip(s1, s2) if a != b)
        print("%s: %d ticks; asm %s, readable C %s (C entered %d times); screens %s" % (
            os.path.basename(snap), ticks, "ok" if ok1 else "stopped", "ok" if ok2 else "stopped",
            hits, "identical" if same else "differ in %d pixels%s" % (
                diff_px, "" if p1 == p2 else " and the palette")))
        return 0 if same and ok1 and ok2 else 1
    return xn_cload.play(snap, ticks, all_c=not asm, shot=shot, script=script,
                         img=None if asm else RImage(), what="the readable C")


def frames_boundary(snap, n, ticks=0, script="", irqs=0, continuous=False):
    """`frames --boundary`: the same run through tools/xn_scenarios.py's lockstep, compared at
    the boundary (the screen, the game-visible memory, the frame's port and int log); the
    game's routes (canonical functions only at the boundary). Timer interrupts are given one
    at a time, each run to completion, at the frame's start."""
    import xn_boundary
    import xn_scenarios
    spec = {"name": os.path.basename(snap).replace(".snap", ""), "snap": snap, "ticks": ticks,
            "script": script, "frames": n, "irqs": irqs, "input": ""}
    r = xn_scenarios.run(spec, img=RImage(), masks=xn_boundary.load_masks(),
                         continuous=continuous)
    for f in r["frames"]:
        print("frame %d: asm %d M insns, C %d M (C entered %d times): %s%s" % (
            f["frame"], f["asm_insns"] // 1000000, f["c_insns"] // 1000000, f.get("c_entered", 0),
            "identical" if f.get("boundary") else "DIFFERS", "" if f.get("full") else
            " (all of memory: %d bytes differ, %d of them game-visible)" % (
                f.get("diff_bytes", 0), f.get("visible_bytes", 0))))
    print("%s: %d / %d frames identical at the boundary (%d in all of memory)" % (
        spec["name"], r["identical"], r["total"], r["full_identical"]))
    return 0 if r["identical"] == r["total"] == n else 1


def scenarios(names_=None, jobs=None, continuous=None, frames=None, verbose=False,
              coverage=False):
    """Run the scripted lockstep scenarios (tools/xn_scenarios.py), compared at the boundary;
    $XN_RC_OUT/scenarios.json."""
    import xn_cload
    import xn_scenarios
    t0 = time.time()
    names_ = names_ or [s_["name"] for s_ in xn_scenarios.SCENARIOS]
    for n_ in names_:
        xn_scenarios.get(n_)
    spec = {"image": IMAGE, "continuous": continuous, "frames": frames, "verbose": verbose,
            "coverage": coverage}
    res = xn_cload.parallel("xn_scenarios:_task", names_, spec, jobs)
    out = [r for rs in res for r in rs]
    by = {r["name"]: r for r in out}
    tot = ok = 0
    print("%-14s %-6s %-11s %s" % ("scenario", "frames", "identical", "covers"))
    for n_ in names_:
        r = by.get(n_)
        if r is None:
            print("%-14s (no result)" % n_)
            continue
        tot += r["total"]
        ok += r["identical"]
        note = r.get("error") or ("" if r.get("complete") else "incomplete: " + "; ".join(
            f.get("note", "") for f in r["frames"] if f.get("note"))[:80])
        print("%-14s %6d %5d / %-3d %s%s" % (n_, r["total"], r["identical"], r["total"],
                                           r.get("covers", ""), "  " + note if note else ""))
        bad = [f for f in r["frames"] if not f.get("boundary")]
        for f in bad[:3]:
            print("    frame %d: %s" % (f["frame"], f.get("note") or "%s%s%s %d game-visible bytes "
                                          "differ (%s)" % ("" if f.get("screen") else "screen, ",
                                                            "" if f.get("io") else "I/O, ", "memory",
                                                            f.get("visible_bytes", 0),
                                                            " ".join(f.get("first", [])))))
    print("%d / %d frames identical over %d scenarios in %.0f s" % (ok, tot, len(out), time.time() - t0))
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "scenarios.json"), "w") as f:
        json.dump([{k: v for k, v in r.items() if k != "coverage"} for r in out], f, indent=0)
    return 0 if ok == tot and all(by.get(n_, {}).get("complete") for n_ in names_) else 1


def frames(snap, n, ticks=0, script="", irqs=0):
    """The game a frame at a time in lockstep: from the same machine, one pass of main's loop
    (func_0001025B to func_0001025B) with the asm and with every converted function in C,
    the timer held (no interrupts in the frame), then all of low and program memory and the
    screen compared. The asm machine goes on to the next frame; the C machine starts each
    frame from a copy of it. `play --compare` runs by timer ticks, which come every so many
    instructions: the C's other instruction counts move the ticks, and a moving scene ends
    differently. This compares the frames themselves. irqs: timer interrupts given to both
    machines at the start of each frame, at the same instruction, so game time moves (walking,
    animation, flicker) the same in both."""
    import fallemu
    import fallcall
    safe = fallemu.LOAD + fallcall.SAFE
    path = snap if os.path.exists(snap) else os.path.join(fallemu.SNAPS, snap + ".snap")
    state = os.path.join(OUT, "frames_state.snap")
    os.makedirs(OUT, exist_ok=True)
    asm = fallemu.Emu.load(path, overlay=os.path.join(OUT, "overlay_frames_asm"))
    img = RImage()

    def one_frame(emu, cap=2_000_000_000):
        """Run main's loop once, interrupts held; the instructions it took, or None. A code
        hook at the safe point stops the CPU (emu_start's `until` is compiled into translated
        blocks, and a block translated before is not stopped by it)."""
        from unicorn import UC_HOOK_CODE
        hit = []

        def stop(uc, address, size, _user):
            if armed:
                hit.append(address)
                uc.emu_stop()
        armed = False
        hk = emu.uc.hook_add(UC_HOOK_CODE, stop, None, safe, safe)
        try:
            emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=1)   # off the safe point
            for _ in range(irqs):           # the same timer interrupts at the same place
                if emu.r("eflags") & 0x200:
                    emu.ticks += 1
                    emu.pit_reads = 0
                    emu.irq(8)
            armed = True
            done = 0
            while done < cap and not hit:
                emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=fallemu.TICK)
                while not hit and emu.clear_exception_state():
                    emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=fallemu.TICK)
                done += fallemu.TICK
            return done if hit else None
        finally:
            emu.uc.hook_del(hk)

    bad = 0
    try:
        if ticks:
            asm.run(asm.ticks + ticks, fallemu.parse_script(script))
        if not fallcall.run_until(asm, fallcall.SAFE, 3000):
            print("%s: the game does not reach its frame loop" % os.path.basename(path))
            return 1
        for k in range(n):
            asm.save(state)
            c = fallemu.Emu.load(state, overlay=os.path.join(OUT, "overlay_frames_c"))
            try:
                img.install(c)
                img.route(c, sorted(img.funcs))
                hits0 = img.hits
                na, nc = one_frame(asm), one_frame(c)
                if na is None or nc is None:
                    print("frame %d: %s did not finish" % (k, "asm" if na is None else "C"))
                    bad += 1
                    break
                esp = asm.r("esp")
                diffs = []
                for base, size in ((0, fallemu.LOW), (fallemu.LOAD, fallemu.MEM)):
                    ma, mc = asm.read(base, size), c.read(base, size)
                    if ma == mc:
                        continue
                    for off in range(0, size, 4096):
                        if ma[off:off + 4096] != mc[off:off + 4096]:
                            a = base + off
                            if esp - (1 << 20) <= a < esp:      # the dead stack below ESP
                                continue
                            diffs.append(a)
                regs = [r for r in ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp")
                        if asm.r(r) != c.r(r)]
                print("frame %d: asm %d M insns, C %d M (C entered %d times): %s" % (
                    k, na // 1000000, nc // 1000000, img.hits - hits0,
                    "identical" if not diffs else "%d pages differ (first %08X)" % (
                        len(diffs), diffs[0])) + (" [registers %s]" % " ".join(regs) if regs else ""))
                bad += bool(diffs)
            finally:
                c.close()
        return 1 if bad else 0
    finally:
        asm.close()


# --------------------------------------------------------------------------------------------
# Report

COLUMNS = ("function", "name", "subsystem", "convention", "converted", "records_passed",
           "records_total", "first_difference", "together_passed", "together_total",
           "corpus_passed", "corpus_total", "diff_passed", "diff_total", "clobber_test",
           "inputs_test", "game_route", "boundary_passed", "boundary_total")


def write_report():
    import xn_abi
    abi = xn_abi.read_abi()
    res = load_results()
    routes, game = {}, {}
    if os.path.exists(IMAGE):
        im = RImage()
        routes, game = im.routes, im.game
    clob = {}
    inp = {}
    for kind, d in (("clobber", clob), ("inputs", inp)):
        p = os.path.join(OUT, kind + ".json")
        if os.path.exists(p):
            j = json.load(open(p))
            for k, v in j.get("stats", {}).items():
                d[k] = v
            if kind == "clobber":
                d["_fired"] = j.get("fired", {})
    rows = []
    for va, r in sorted(abi.items()):
        k = "%06X" % va
        row = {c: "" for c in COLUMNS}
        row.update({"function": k, "name": r["name"], "subsystem": r["subsystem"],
                    "convention": r["convention"],
                    "converted": routes[va]["kind"] if va in routes else "no"})
        a = res.get("alone", {}).get(k)
        if a:
            row["records_passed"], row["records_total"], row["first_difference"] = a[0], a[1], a[2]
        t = res.get("together", {}).get(k)
        if t:
            row["together_passed"], row["together_total"] = t[0], t[1]
        c = res.get("corpus", {}).get(k)
        if c:
            row["corpus_passed"], row["corpus_total"] = c[0], c[1]
        d = res.get("diff", {}).get(k)
        if d:
            row["diff_passed"], row["diff_total"] = d[0], d[1]
        if va in game:
            row["game_route"] = game[va]["kind"]
        bnd = res.get("boundary", {}).get(k)
        if bnd:
            row["boundary_passed"], row["boundary_total"] = bnd[0], bnd[1]
        if k in clob or k in clob.get("_fired", {}):
            fired = clob.get("_fired", {}).get(k, 0)
            st = clob.get(k)
            okc = st is None or st[0] == st[1]
            row["clobber_test"] = "%s (scrambled at %d returns)" % ("pass" if okc else "FAIL",
                                                                  fired)
        if k in inp:
            st = inp[k]
            row["inputs_test"] = "%s %d/%d" % ("pass" if st[0] == st[1] else "FAIL", st[0], st[1])
        rows.append(row)
    os.makedirs(os.path.dirname(REPORT), exist_ok=True)
    with open(REPORT, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=COLUMNS)
        w.writeheader()
        w.writerows(rows)
    return rows


def report():
    rows = write_report()
    conv = [r for r in rows if r["converted"] != "no"]
    by = collections.defaultdict(lambda: [0, 0, 0, 0, 0])
    for r in rows:
        b = by[r["subsystem"]]
        b[0] += 1
        if r["converted"] != "no":
            b[1] += 1
            if r["records_total"] and r["records_passed"] == r["records_total"]:
                b[2] += 1
            if r["diff_total"] and r["diff_passed"] == r["diff_total"]:
                b[3] += 1
            if not r["records_total"] and not r["diff_total"]:
                b[4] += 1
    print("%d of %d functions converted -> %s" % (len(conv), len(rows), os.path.relpath(REPORT, ROOT)))
    print("%-10s %5s %9s %13s %10s %9s" % ("subsystem", "funcs", "converted", "records pass",
                                          "diff pass", "untested"))
    for s, b in sorted(by.items(), key=lambda kv: -kv[1][1]):
        if b[1]:
            print("%-10s %5d %9d %13d %10d %9d" % (s, *b))
    return 0


# --------------------------------------------------------------------------------------------
# The asm, with names; skeletons

_prog = None


def asm_listing(va):
    """The asm of a function (as tools/xn_c.py walks it), with names for its targets and the
    globals it addresses."""
    import xn_c
    by_name, funcs = names()
    glob_at = {a: n for n, (k, a) in by_name.items()}
    global _prog
    if _prog is None:
        _prog = xn_c.Program()
    fn = xn_c.Func(_prog, va)
    lines = []
    for a in fn.order:
        x = fn.insns[a]
        i = x.i
        text = "%s %s" % (i.mnemonic, i.op_str)

        def sub(m):
            v = int(m.group(0), 16)
            n = glob_at.get(v)
            if n and (v >= 0xC0000 or v >= 0x10000 and by_name[n][0] == "func"):
                return n
            return m.group(0)
        text = re.sub(r"0x[0-9a-f]{5,8}", sub, text)
        lab = "%06X" % a
        if a in funcs and a != va:
            lab += " <%s>" % funcs[a][0]
        lines.append("%s  %s" % (lab, text))
    return lines


def skel(sub, force=False, out=None):
    """A header and a stub .c for a subsystem, from config/xngine_abi.csv and names.csv."""
    import xn_abi
    abi = xn_abi.read_abi()
    _b, funcs = names()
    vas = subsystem_funcs(sub)
    if not vas:
        print("no subsystem %s" % sub)
        return 1
    hname = ("x" + sub)[:8] + ".h"
    cname = sub[:8] + ".c"
    dest = out or SRC
    if not out and not force and (os.path.exists(os.path.join(SRC, hname)) or
                                  os.path.exists(os.path.join(SRC, cname))):
        dest = os.path.join(OUT, "skel", sub)
    os.makedirs(dest, exist_ok=True)
    guard = "X%s_H" % sub.upper()
    h = ["/* %s: XnGine's %s functions (readable C, src/engine/%s; see xngine.h). */" % (
        hname, sub, cname), "#ifndef %s" % guard, "#define %s" % guard, "",
        '#include "xngine.h"', ""]
    c = ["/* %s: XnGine's %s functions as readable C (see xngine.h, docs/xngine_readable.md)."
         % (cname, sub), "   Functions under #if 0 are still asm; take one out when it passes "
         "`tools/xn_rc.py test`. */", '#include "%s"' % hname, ""]
    for va in vas:
        row = abi[va]
        name, conf, ev = funcs.get(va, ("func_%08X" % va, "", ""))
        decl, pragma, glue = declaration(row, name)
        note = "%s (%06X, %s): %s" % (name, va, conf, ev)
        abi_line = "ABI %s: in %s%s; out %s%s; clobber %s; callers %s%s" % (
            row["convention"], row["inputs"] or "-",
            " +%d stack bytes" % row["stack_args"] if row["stack_args"] else "",
            row["outputs"] or "-", " flags " + row["flags_out"] if row["flags_out"] else "",
            row["clobber"] or "-", row["callers"], "; " + row["notes"] if row["notes"] else "")
        h.append("/* %s\n   %s */" % (xn_c_wrap(note), xn_c_wrap(abi_line)))
        h.append(decl)
        if pragma:
            h.append(pragma)
        if glue:
            h.append("void %s_r(xn_regs *r);      /* the asm interface: see xngine.h */" % name)
        h.append("")
        c.append("#if 0    /* %s %06X */" % (name, va))
        c.append("/* asm:")
        c += ["   " + ln for ln in asm_listing(va)]
        c.append("*/")
        c.append(decl[:-1])
        c.append("{")
        c.append("}")
        if glue:
            c.append("")
            c.append("void %s_r(xn_regs *r)" % name)
            c.append("{")
            c.append("    /* in: %s; out: %s%s */" % (row["inputs"] or "-", row["outputs"] or "-",
                                                    " flags " + row["flags_out"]
                                                    if row["flags_out"] else ""))
            c.append("}")
        c.append("#endif")
        c.append("")
    h.append("#endif")
    with open(os.path.join(dest, hname), "w") as f:
        f.write("\n".join(h) + "\n")
    with open(os.path.join(dest, cname), "w") as f:
        f.write("\n".join(c) + "\n")
    print("%d functions -> %s, %s" % (len(vas), os.path.relpath(os.path.join(dest, hname), ROOT),
                                     os.path.relpath(os.path.join(dest, cname), ROOT)))
    return 0


def xn_c_wrap(text, width=92):
    import xn_c
    return xn_c.wrap(text, width)


REG_TYPE = {"eax": "s32", "ax": "s16", "al": "u8", "ah": "u8"}


def declaration(row, name):
    """(prototype, #pragma aux or "", needs glue) for a function's ABI row."""
    import xn_abi
    ins = xn_abi.names_of(row["in"] & xn_abi.GPR).split()
    outs = xn_abi.names_of(row["out"] & ~xn_abi.SEG).split()
    glue = len(outs) > 1 or bool(row["fout"]) or any(o.endswith(".u") for o in outs) or \
        keeps_eax(row) and row["stack_args"]
    params = []
    for k, r in enumerate(ins):
        if r.endswith(".u"):
            continue
        t = "u8" if r[-1] in "lh" and len(r) == 2 else ("u16" if len(r) == 2 else "s32")
        params.append(("%s a%d" % (t, k + 1), r))
    for k in range(row["stack_args"] // 4):
        params.append(("s32 s%d" % (k + 1), None))
    if glue:
        ret = "void"
        proto = "%s %s(%s);" % (ret, name, ", ".join(p for p, _r in params) or "void")
        return proto, "", True
    ret = "void"
    if outs:
        o = outs[0]
        ret = "u8" if o in ("al", "ah", "bl", "bh", "cl", "ch", "dl", "dh") else (
            "u16" if len(o) == 2 else "s32")
    proto = "%s %s(%s);" % (ret, name, ", ".join(p for p, _r in params) or "void")
    if row["convention"] == "watcom":
        return proto, "", False
    regs = " ".join("[%s]" % r for _p, r in params if r)
    mod = [r for r in ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp")
           if xn_abi.RMASK[r] & ~row["clob"] == 0 and (not outs or r != outs[0])]
    if keeps_eax(row) and "eax" not in mod and (not outs or outs[0] != "eax"):
        mod = ["eax"] + mod             # the route's stub keeps EAX for the callers
    how = "routine" if row["ret_pop"] == row["stack_args"] else "caller"
    if outs:
        mod = [outs[0]] + mod
    pragma = "#pragma aux %s parm %s%s%s modify exact [%s];" % (
        name, "%s " % how if row["stack_args"] else "", regs or "[]",
        " value [%s]" % outs[0] if outs else "", " ".join(mod))
    return proto, pragma, False


# --------------------------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("skel")
    s.add_argument("subsys")
    s.add_argument("--force", action="store_true")
    s.add_argument("--out", default=None, help="another folder than src/engine")
    b = sub.add_parser("build")
    b.add_argument("--flags", default=None)
    sub.add_parser("route")
    t = sub.add_parser("test")
    t.add_argument("funcs", nargs="*")
    t.add_argument("--all", action="store_true", help="every converted function routed at once")
    t.add_argument("--corpus", action="store_true", help="every record, all converted routed")
    t.add_argument("--boundary", action="store_true",
                   help="the boundary's entries as the game calls them (game-visible compare)")
    t.add_argument("-j", "--jobs", type=int, default=None)
    t.add_argument("--max-per", type=int, default=0)
    t.add_argument("-v", action="store_true")
    d = sub.add_parser("diff")
    d.add_argument("funcs", nargs="*")
    d.add_argument("--untested", action="store_true")
    d.add_argument("--trials", type=int, default=8)
    d.add_argument("-v", action="store_true")
    p = sub.add_parser("play")
    p.add_argument("snap")
    p.add_argument("--ticks", type=int, default=100)
    p.add_argument("--shot", default=None)
    p.add_argument("--asm", action="store_true")
    p.add_argument("--compare", action="store_true")
    p.add_argument("--script", default="")
    fr = sub.add_parser("frames")
    fr.add_argument("snap")
    fr.add_argument("--frames", type=int, default=10)
    fr.add_argument("--ticks", type=int, default=0, help="play this many ticks (asm) first")
    fr.add_argument("--script", default="")
    fr.add_argument("--irqs", type=int, default=0, help="timer interrupts at each frame's start")
    fr.add_argument("--boundary", action="store_true",
                    help="compare at the boundary (game-visible memory, the screen, the I/O)")
    fr.add_argument("--continuous", action="store_true",
                    help="(--boundary) the C machine keeps its own state between frames")
    sc = sub.add_parser("scenarios")
    sc.add_argument("names", nargs="*")
    sc.add_argument("-j", "--jobs", type=int, default=None)
    sc.add_argument("--continuous", action="store_true", default=None)
    sc.add_argument("--resync", action="store_true", help="each frame from the asm's state")
    sc.add_argument("--frames", type=int, default=None)
    sc.add_argument("-v", action="store_true")
    sub.add_parser("report")
    for n in ("asm", "abi"):
        q = sub.add_parser(n)
        q.add_argument("func")
    a = ap.parse_args()
    if a.cmd == "skel":
        return skel(a.subsys, a.force, a.out)
    if a.cmd == "build":
        return build(a.flags)
    if a.cmd == "route":
        return route_table()
    if a.cmd == "test":
        return test(expand(a.funcs), together=a.all, corpus=a.corpus, jobs=a.jobs,
                    max_per=a.max_per, verbose=a.v, boundary=a.boundary)
    if a.cmd == "diff":
        return diff(expand(a.funcs), untested=a.untested, trials=a.trials, verbose=a.v)
    if a.cmd == "play":
        return play(a.snap, a.ticks, a.shot, a.asm, a.compare, a.script)
    if a.cmd == "frames":
        if a.boundary:
            return frames_boundary(a.snap, a.frames, a.ticks, a.script, a.irqs, a.continuous)
        return frames(a.snap, a.frames, a.ticks, a.script, a.irqs)
    if a.cmd == "scenarios":
        cont = False if a.resync else a.continuous
        return scenarios(a.names, a.jobs, cont, a.frames, a.v)
    if a.cmd == "report":
        return report()
    if a.cmd == "asm":
        print("\n".join(asm_listing(func_va(a.func))))
        return 0
    if a.cmd == "abi":
        import xn_abi
        row = xn_abi.read_abi().get(func_va(a.func))
        for k in xn_abi.COLUMNS:
            print("%-11s %s" % (k, row[k]))
        return 0


if __name__ == "__main__":
    sys.exit(main())
