#!/usr/bin/env python3
"""XnGine's data as C: the inventory of what the canonical engine (src/engine/) references and
does not define, the data module that defines the engine's own (src/engine_data/), and the
proofs that it equals FALL.EXE's.

The canonical C declares the globals it uses as `extern`s; the test harness's linker
(tools/xn_rc.py, tools/xn_cload.py) resolves them to their addresses in the loaded FALL.EXE,
because the game (object 1) reads the engine's data there. A build of the engine outside
FALL.EXE (the SDL port) needs that data in C: src/engine_data/ (docs/engine/data.md).

  inventory  compile src/engine/ (Watcom C32 10.0a, tools/wcc10.py), list every symbol its
             objects reference and do not define, with the C declaration the headers give it
             (type and size: the compiler measures them, and checks every declaration a .c
             file repeats against the headers'), its address in FALL.EXE, its owner and, for
             the engine's data, its kind and the evidence:
               owner   engine    object-2 data
                       game      object-1 functions and object-3 data (the game's own C)
                       runtime   the libraries FALL.EXE links (the C runtime, MemCheck, SOS),
                                 the build's interrupt entries and code addresses
                       hardware  absolute addresses the engine reads or writes (VGA, the BIOS
                                 data area, the zero page): not symbols, listed for the port
               kind    zero      all zero in FALL.EXE's load image (BSS)
                       computed  a table code generates: src/engine_data/dcompute.c (init
                                 compute), or the engine's own init at run time (init runtime,
                                 filled_by), zero in the image
                       initial   FALL.EXE's own bytes: xn_data_load copies them from the
                                 user's FALL.EXE (init load), or a C initializer (init c: a
                                 scalar default, or a table of code addresses that names the
                                 functions FALL.EXE's fixups point at)
                       alias     a name for part of another symbol's storage (the headers'
                                 macros)
             Also the object-2 data the engine reaches only through its pointer tables, the
             game functions those tables name, and object-2 data only the game references.
             -> config/xngine_data.csv
  gen        the generated parts of src/engine_data/ from the inventory: ddefs.c (the
             definitions, with the declared types and the initializers) and dtable.c (the
             loader's table of initial symbols: storage, address, size)
  link       proof (a): compile src/engine/ and src/engine_data/ with Watcom 10.0a and link
             them standalone with a resolver that refuses every FALL.EXE address: only the
             game's and the runtime's symbols (stubs) may be outside
             -> build/xn_canon/data/standalone.pkl
  check      proof (b): in a bare machine (Unicorn, no game, no DOS), the standalone image's
             xn_data_object2 reads object 2 from FALL.EXE's bytes, xn_data_load and
             xn_data_compute run; every engine symbol's bytes are compared with FALL.EXE's
             load image at its address (pointers by what they point at); then the engine's
             own inits run (xn_mem_init, xn_render_init, the noise, water and stars) and the
             tables they fill are compared with the original game's after its boot (a
             snapshot) -> build/xn_canon/data/check.json
  all        the four in turn

usage: xn_datagen.py inventory | gen | link | check | all     (decls: the parsed declarations)
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

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
ENGINE = os.path.join(ROOT, "src", "engine")
DATA_SRC = os.path.join(ROOT, "src", "engine_data")
WORK = os.environ.get("XN_DATA_WORK") or os.path.join(ROOT, "build", "xn_canon", "data")
CSV_PATH = os.path.join(ROOT, "config", "xngine_data.csv")
NAMES_CSV = os.path.join(ROOT, "config", "names.csv")

OBJ1 = (0x010000, 0x0BB27F)
OBJ2 = (0x0C0000, 0x161568)
OBJ3 = (0x170000, 0x1B5520)
LIBRARY = (0x09DA1C, 0x0BB27F)      # config/regions.csv: MemCheck, the Watcom C runtime, SOS
LOAD = 0x01000000                   # tools/fallemu.py: objects at LOAD + preferred address


def rel(p):
    return os.path.relpath(p, ROOT)


# --------------------------------------------------------------------------------------------
# Compiling

def flags():
    import xn_rc
    return list(xn_rc.FLAGS)


def compile_dir(srcs, workdir, header_dirs):
    """Compile srcs (C and WASM) in one DOSBox-X session with the headers of header_dirs (8.3,
    upper case, as tools/xn_rc.py copies them). Returns {src: obj path}; exits on an error."""
    import wcc10
    shutil.rmtree(workdir, ignore_errors=True)
    os.makedirs(workdir)
    for d in header_dirs:
        for h in glob.glob(os.path.join(d, "*.h")):
            shutil.copyfile(h, os.path.join(workdir, os.path.basename(h).upper()))
    objs, td = wcc10.compile_many(srcs, flags(), workdir=workdir)
    bad = []
    for k, s in enumerate(srcs):
        e = os.path.join(td, "N%04d.ERR" % k)
        msg = open(e, errors="replace").read().strip() if os.path.exists(e) else ""
        msg = msg.replace("N%04d.C" % k, rel(s)).replace("N%04d.ASM" % k, rel(s))
        if objs[s] is None or "Error!" in msg:
            bad.append("%s:\n%s" % (rel(s), "\n".join(msg.splitlines()[-20:])))
        elif "Warning!" in msg and s.startswith(DATA_SRC):
            print(msg)
    if bad:
        raise SystemExit("compile failed:\n" + "\n".join(bad))
    return objs


def engine_sources():
    return sorted(glob.glob(os.path.join(ENGINE, "*.c"))) + \
        sorted(glob.glob(os.path.join(ENGINE, "*.asm")))


def data_sources():
    return sorted(glob.glob(os.path.join(DATA_SRC, "*.c")))


def link_name(sym):
    """A Watcom link name -> (C name, 'data' | 'func'): _NAME is data, NAME_ a function."""
    if sym.startswith("_"):
        return sym[1:], "data"
    if sym.endswith("_"):
        return sym[:-1], "func"
    return sym, "func"


def references(objs):
    """{link name: sorted source files} of the symbols the objects reference and none defines."""
    import xn_cload
    parsed = [(s, xn_cload.Obj(o)) for s, o in objs.items()]
    defined = set()
    for _s, o in parsed:
        defined |= set(o.pubs)
    refs = collections.defaultdict(set)
    for s, o in parsed:
        for e in o.exts[1:]:
            if e not in defined and e not in o.lpubs:
                refs[e].add(os.path.basename(s))
    return {k: sorted(v) for k, v in refs.items()}


# --------------------------------------------------------------------------------------------
# Declarations

TYPE_WORDS = {"extern", "static", "const", "volatile", "struct", "union", "enum", "unsigned",
              "signed", "char", "short", "int", "long", "void", "float", "double"}


def strip_comments(t):
    """Comments and preprocessor lines blanked (line numbers kept)."""
    def blank(m):
        return re.sub(r"[^\n]", " ", m.group(0))
    t = re.sub(r"/\*.*?\*/", blank, t, flags=re.S)
    t = re.sub(r"//[^\n]*", blank, t)
    t = re.sub(r"^[ \t]*#(?:[^\n]*\\\n)*[^\n]*", blank, t, flags=re.M)
    return t


def split_top(s, sep=","):
    out, depth, cur = [], 0, ""
    for ch in s:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == sep and depth == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    out.append(cur)
    return out


Decl = collections.namedtuple("Decl", "name file line base declarator func incomplete text")


def declarator_name(d):
    """The identifier a declarator declares (`*(*f)(char *p)` -> f, `t[N]` -> t)."""
    for m in re.finditer(r"[A-Za-z_]\w*", d):
        if m.group(0) not in TYPE_WORDS:
            return m.group(0), m.end()
    return None, 0


def parse_decls(paths):
    """{C name: [Decl]}: the file-scope declarations (extern data and function prototypes) of
    the files; definitions (with a body or an initializer) are left out."""
    out = collections.defaultdict(list)
    for p in paths:
        raw = open(p, encoding="latin-1").read()
        t = strip_comments(raw)
        depth = 0
        start = 0
        k = 0
        while k < len(t):
            ch = t[k]
            if ch == "{":
                depth += 1
            elif ch == "}":
                depth -= 1
                if depth == 0:
                    start = k + 1           # a function body or a struct definition ended
            elif ch == ";" and depth == 0:
                stmt = t[start:k]
                line = raw.count("\n", 0, start + len(stmt) - len(stmt.lstrip())) + 1
                start = k + 1
                _stmt_decls(out, p, line, stmt)
            k += 1
    return out


def _stmt_decls(out, path, line, stmt):
    s = " ".join(stmt.split())
    if not s or s.startswith("typedef") or "{" in s or "=" in s or s.startswith("#"):
        return
    # the declaration specifiers end where the first declarator starts: qualifiers, basic type
    # words, struct/union/enum tags and at most one typedef name
    parts = split_top(s)
    first = parts[0]
    toks = list(re.finditer(r"[A-Za-z_]\w*|\S", first))
    i, typed = 0, False
    while i < len(toks):
        w = toks[i].group(0)
        if w in ("extern", "static", "const", "volatile", "register"):
            i += 1
        elif w in ("struct", "union", "enum"):
            i, typed = i + 2, True
        elif w in ("unsigned", "signed", "char", "short", "int", "long", "void", "float",
                   "double"):
            i, typed = i + 1, True
        elif re.match(r"[A-Za-z_]", w) and not typed:
            i, typed = i + 1, True
        else:
            break
    if i >= len(toks) or not typed:
        return
    cut = toks[i].start()
    words = first[:cut].split()
    decls = [first[cut:]] + parts[1:]
    is_extern = "extern" in words
    for d in decls:
        d = d.strip()
        name, end = declarator_name(d)
        if not name:
            continue
        rest = d[end:].lstrip()
        # a function: the name followed by its parameter list (not a pointer to one)
        func = rest.startswith("(") and not re.match(r"^[\*\s]*\(", d[:end - len(name)] or "")
        if not func and not is_extern:
            continue                        # a definition of storage, not ours to list
        if "static" in words:
            continue
        incomplete = bool(re.search(r"\b" + name + r"\s*\[\s*\]", d))
        btxt = " ".join(w for w in words if w != "extern")
        out[name].append(Decl(name, rel(path), line, btxt, d, func, incomplete,
                              "%s %s" % (btxt, d)))


def engine_decls():
    return parse_decls(sorted(glob.glob(os.path.join(ENGINE, "*.h"))) +
                       sorted(glob.glob(os.path.join(ENGINE, "*.c"))))


def norm_decl(d):
    """A declaration normalised for comparison (parameter names and array sizes kept)."""
    return re.sub(r"\s+", " ", d.text.replace("volatile ", "")).strip()


def header_names():
    return sorted(os.path.basename(h) for h in glob.glob(os.path.join(ENGINE, "*.h")))


def probe_sizes(names, decls, workdir):
    """{C name: (sizeof, sizeof element or 0, incomplete)} of data symbols, measured by Watcom
    10.0a in a file that includes every engine header (and the .c-only declarations)."""
    lines = ['#include "%s"' % h for h in header_names()]
    for n in names:
        if not any(d.file.endswith(".h") for d in decls[n]):
            d = decls[n][0]
            lines.append("extern %s %s;" % (d.base, d.declarator))
    order = []
    lines.append("const unsigned long xn_probe_size[] = {")
    for n in names:
        d = decls[n][0]
        inc = all(x.incomplete for x in decls[n])
        arr = "[" in d.declarator and not re.search(r"\(\s*\*", d.declarator)
        lines.append("    %s, %s," % ("0" if inc else "sizeof(%s)" % n,
                                     "sizeof(%s[0])" % n if arr else "0"))
        order.append((n, inc))
    lines.append("};")
    os.makedirs(workdir, exist_ok=True)
    src = os.path.join(workdir, "probe.c")
    with open(src, "w") as f:
        f.write("\n".join(lines) + "\n")
    objs = compile_dir([src], os.path.join(workdir, "w"), [ENGINE])
    import xn_cload
    o = xn_cload.Obj(objs[src])
    si, off = o.pubs["_xn_probe_size"]
    data = o.data[si]
    out = {}
    for k, (n, inc) in enumerate(order):
        size, elem = struct.unpack_from("<II", data, off + 8 * k)
        out[n] = (size, elem, inc)
    return out




# --------------------------------------------------------------------------------------------
# FALL.EXE: object 2's load image, its fixups, the names

class Exe:
    """FALL.EXE's object 2 as loaded (pointers relocated to preferred addresses), its LE
    fixups, and the code ranges of its functions."""

    def __init__(self):
        import match
        from le import LE
        self.le = LE(match.EXE)
        objs = self.le.load(relocate=True)
        self.obj = {k: (self.le.objs[k - 1].base, objs[k]) for k in objs}
        self.fixups = {f.src_va: f for f in self.le.fixups()}
        self.code = []
        with open(os.path.join(ROOT, "config", "xngine_functions.csv"), newline="") as f:
            for r in csv.DictReader(f):
                a = int(r["va"], 16)
                self.code.append((a, a + int(r["code_bytes"])))
        self.code.sort()

    def bytes_at(self, va, n):
        for base, buf in self.obj.values():
            if base <= va < base + len(buf):
                return bytes(buf[va - base:va - base + n])
        raise ValueError("%06X" % va)

    def fixups_in(self, lo, hi):
        return [self.fixups[a] for a in range(lo, hi) if a in self.fixups]

    def in_code(self, va):
        """True when va is inside (or at the end of) a function's code bytes."""
        import bisect
        k = bisect.bisect_right(self.code, (va, 1 << 40)) - 1
        return k >= 0 and self.code[k][0] <= va <= self.code[k][1]

    def object2(self, raw=False):
        """Object 2's bytes from its base (0xC0000): relocated, or as the file holds them."""
        if raw:
            return bytes(self.le.load(relocate=False)[2])
        return bytes(self.obj[2][1])


def region(va):
    if OBJ2[0] <= va < OBJ2[1]:
        return "obj2"
    if OBJ3[0] <= va < OBJ3[1]:
        return "obj3"
    if LIBRARY[0] <= va < LIBRARY[1]:
        return "library"
    if OBJ1[0] <= va < OBJ1[1]:
        return "obj1"
    return "low"


_globals = None


def object2_globals():
    """[(address, name)] of every object-2 global and function config/names.csv and the aliases
    name (for a symbol's extent: up to the next one), sorted."""
    global _globals
    if _globals is None:
        import xn_rc
        by_name, _f = xn_rc.names()
        _globals = sorted((a, n) for n, (k, a) in by_name.items()
                          if k in ("global", "func") and OBJ2[0] <= a < OBJ2[1])
    return _globals


def name_at(va, kind="func"):
    """The config/names.csv name of the function (or global) at va, else func_XXXXXXXX."""
    import xn_rc
    by_name, funcs = xn_rc.names()
    if kind == "func" and va in funcs:
        return funcs[va][0]
    for n, (k, a) in sorted(by_name.items()):
        if a == va and k == kind:
            return n
    return ("func_%08X" if kind == "func" else "D_%08X") % va


def names_evidence(name):
    with open(NAMES_CSV, newline="") as f:
        for r in csv.DictReader(f):
            if r["name"] == name and r["kind"] in ("global", "func"):
                return r["evidence"]
    p = os.path.join(ROOT, "config", "xngine_aliases.csv")
    with open(p, newline="") as f:
        for r in csv.DictReader(f):
            if r["name"] == name:
                return r["evidence"]
    return ""


# --------------------------------------------------------------------------------------------
# What the inventory knows besides the code: the evidence a person settled

# Names of the engine's data that are views of another symbol's storage (src/engine's headers
# define them as macros, so a standalone build keeps one storage): name -> (symbol, offset,
# the header's expression, why).
ALIASES = {
    "xn_cos_table": ("xn_sin_table", 0x800, "xn_sin_table + XN_ANGLES / 4",
                     "the cosine is the sine table a quarter turn on: one table of 2560 "
                     "entries (names.csv: it runs on into the cosine)"),
    "xn_math_asin_table": ("xn_math_asin_values", 0x800, "xn_math_asin_values + 512",
                           "centred: the asm indexes it -512..511 (xn_math_asin_coarse); the "
                           "negative half lies before 0x15DC00"),
    "xn_cam_dir_x_mid": ("xn_cam_dir_x_table", 0x800, "xn_cam_dir_x_table + 512",
                         "the middle of the camera's x rays (k = -512..511), which "
                         "xn_cam_set_focal fills through the table's start"),
    "xn_cam_dir_y_mid": ("xn_cam_dir_y_table", 0x600, "xn_cam_dir_y_table + 384",
                         "the middle of the y rays (k = -384..383)"),
    "xn_squares_table_mid": ("xn_squares_table", 0x4000, "xn_squares_table + 4096",
                             "the middle of the squares (k = -4096..4095), filled through "
                             "the start by xn_render_init, read through the middle"),
    "xn_cam_forward_x": ("xn_cam_rotation", 0x18, "xn_cam_rotation.m[2][0]",
                         "the rotation's row 2: the view's forward direction"),
    "xn_cam_forward_y": ("xn_cam_rotation", 0x1C, "xn_cam_rotation.m[2][1]", "row 2"),
    "xn_cam_forward_z": ("xn_cam_rotation", 0x20, "xn_cam_rotation.m[2][2]", "row 2"),
    "xn_pick_view_x": ("xn_scratch_vecs", 0x00, "xn_scratch_vecs.a.x",
                       "the shared scratch vector a (struct xn_scratch): the pick's point; "
                       "the model code writes a as an edge (Q-MODEL-03)"),
    "xn_pick_view_y": ("xn_scratch_vecs", 0x04, "xn_scratch_vecs.a.y", "scratch vector a"),
    "pick_distance": ("xn_scratch_vecs", 0x08, "xn_scratch_vecs.a.z",
                      "scratch vector a's z: game-visible (the game reads 0x120290)"),
    "xn_scratch_vec_b": ("xn_scratch_vecs", 0x0C, "xn_scratch_vecs.b", "scratch vector b"),
    "xn_pick_flat_x": ("xn_scratch_vecs", 0x18, "xn_scratch_vecs.flat.x", "the flat pick's point"),
    "xn_pick_flat_y": ("xn_scratch_vecs", 0x1C, "xn_scratch_vecs.flat.y", "the flat pick's point"),
    "xn_pick_flat_z": ("xn_scratch_vecs", 0x20, "xn_scratch_vecs.flat.z", "the flat pick's point"),
    "xn_world_nature_archive": ("xn_world_cell_header", 0x04,
                                "xn_world_cell_header.nature_archive",
                                "a field of the cell header read last (struct xn_wld_cell_header)"),
    "xn_world_ground_archive": ("xn_world_cell_header", 0x06,
                                "xn_world_cell_header.ground_archive", "a field of the cell header"),
}

# The extent of an array the headers declare without a size (else: up to the next named
# global): name -> (elements, evidence).
SIZES = {
    "xn_render_span_rows": (9001, "names.csv: a head per screen row, then the span node pool, up "
                            "to xn_render_light_list_pool (0x116C80): 9001 nodes of 16 bytes"),
    "xn_shade_blend_tables": (2, "docs/engine/structs.md: unsigned char *[2] (blend_index is 0 "
                              "or 1: CDC99 sets 1); [1] is the game's translucency table"),
    "xn_collide_flat_anchor_shift": (32, "names.csv: byte pairs indexed by ((flags >> 1) & 15) "
                                     "* 2: 16 pairs (code follows from 0x14A300)"),
    "xn_poly_ring_a": (29, "rings for n = 0..28: the asm's [26..28] are ring_b's [0..2], "
                       "which ring b never uses (n < 3); ring_b ends at 0x159370 (names.csv)"),
    "xn_poly_ring_b": (29, "rings for n = 0..28 (0x1592FC..0x159370)"),
}

# Object-2 data the game references and the engine's C does not: name -> (type, evidence)
GAME_ONLY = {
    "xn_cam_roll": ("s32 xn_cam_roll", "between xn_cam_yaw and xn_cam_x: the game sets the "
                    "eye's roll (15 sites); the canonical engine never reads it"),
}

# Engine data the data module generates (src/engine_data/dcompute.c, or an initializer):
# name -> (how, evidence).
COMPUTED = {
    "xn_sin_table": ("xn_data_compute", "round(sin(2 pi k / 2048) * 2^28) for k = 0..2559, "
                     "except k = 1718, which the original rounds the other way (its exact "
                     "value is -227665571.50015: the table holds -227665571; Q-DATA-01)"),
    "xn_math_sqrt_table": ("xn_data_compute", "round(4 sqrt(i)) for i = 0..4095, at most 255"),
    "xn_pow10_table": ("initializer", "10^k for k = 0..9"),
    "xn_tex_record_offsets": ("xn_data_compute", "20 k for k = 0..511: a record's directory "
                              "entry (20 bytes) in an archive"),
    "xn_poly_ring_a": ("xn_data_compute", "ring n of buffer A for n = 3..28 (0 for n < 3)"),
    "xn_poly_ring_b": ("xn_data_compute", "ring n of buffer B for n = 3..28; [0..2]: ring a's "
                       "[26..28], as the asm's overlapping arrays have them"),
    "xn_poly_ring_tables": ("xn_data_compute", "the rings: for each buffer three lead entries "
                            "(v0 v1 v2), then ring n for n = 3..27, 3n + 1 vertex addresses "
                            "(v0..v(n-1) three times, then vn), and ring 28's first 56; the "
                            "walker reads ring n at -n..2n-1 (the entries before a ring are the "
                            "last of the one before it); 7 unused entries end each block (the "
                            "asm's padding: 0xDB87DB87)"),
    "xn_face_edge_tables": ("xn_data_compute", "by point count n = 3..24: the slot offsets of "
                            "an n-gon's points (0 for n < 3)"),
    "xn_face_edge_slots": ("xn_data_compute", "for n = 3..24: the point slot offsets 0, 8, .., "
                           "8(n - 1), each table closed by the next one's first slot (0): 298 "
                           "used"),
}

# Engine data the engine's own init functions fill at run time (zero in FALL.EXE's image):
# name -> (function, evidence).
RUNTIME = {
    "xn_recip16_table": ("xn_mem_init", "0FFFFh / k (mem.c); entry 3FFh left (Q-MEM-01)"),
    "xn_recip32_table": ("xn_mem_init", "0FFFFFFFFh / k (mem.c); entry 3FFh left (Q-MEM-01)"),
    "xn_colour_fill_table": ("xn_mem_init", "k * 01010101h (mem.c)"),
    "xn_squares_table": ("xn_render_init", "k * k << 8 for k = -4096..4095 (render.c)"),
    "xn_tex_size_mask": ("xn_render_init", "n - 1 for a power of two n, else FFh (render.c)"),
    "xn_render_recip_table": ("xn_render_init", "a block of 2^24 / k for k = 1..65536 from the "
                              "game's allocator (render.c)"),
    "xn_cam_dir_x_table": ("xn_cam_set_focal", "(k << 18) / focal_x (cam.c), from "
                           "xn_render_init and whenever the view changes"),
    "xn_cam_dir_y_table": ("xn_cam_set_focal", "(k << 18) / focal_y (cam.c)"),
    "xn_noise_values": ("xn_rand_noise_init", "256 random bytes twice from seed 1 (rand.c)"),
    "xn_noise_fade_table": ("xn_rand_noise_init", "the fade curve (rand.c "
                            "xn_rand_noise_fade_init)"),
    "xn_water_jitter": ("xn_water_init", "random -1, 0, +1 (water.c)"),
    "xn_water_row_phase": ("xn_water_init", "random (water.c)"),
    "xn_water_row_base": ("xn_water_init", "random (water.c)"),
    "xn_water_row_velocity": ("xn_water_init", "random (water.c)"),
    "xn_sky_stars": ("xn_sky_init_stars", "random directions (sky.c)"),
    "xn_sky_star_colours": ("xn_sky_init_stars", "random colours of xn_sky_star_palette (sky.c)"),
}

# Engine data that is not computed although a formula is near: name -> evidence.
NOT_COMPUTED = {
    "xn_math_asin_values": "no formula found: trunc(asin(i/512) * 2048 / 2 pi) except 22 "
                           "entries (|i| = 165, 168, 171...) rounded up, with fractions from "
                           ".950 to .998 while others up to .994 are not",
}

# The asm's IRQ handlers for COM2-4 the serial table holds (copies of COM1's with their own
# port; no C: nothing installs them, the game opens COM1 only).
SERIAL_ENTRIES = {0x160AE0: "xn_serial_irq_com1_entry", 0x160B60: "xn_serial_irq_com2_entry",
                  0x160BE0: "xn_serial_irq_com3_entry", 0x160C60: "xn_serial_irq_com4_entry"}

# Code addresses the engine declares as data (u8 name[]) to lock a range of code in memory
# (DPMI 0600h): name -> evidence.
LABELS = {
    "xn_mem_game_lock_start": "the game's code (object 1, 0xA12B8) xn_mem_init locks 400h "
                              "bytes of",
    "xn_kbd_code_start": "the keyboard module's code, which xn_kbd_install locks with its data",
    "xn_sys_divide_error_end": "the end of the divide-error handler's asm: "
                               "xn_sys_install_divide_handler locks the entry up to here",
    "xn_joy_timer_end": "the end of the int 1Ch handler's asm: xn_joy_init locks the entry up "
                        "to here",
}

# Absolute addresses the engine's C reads or writes (not symbols): what a port must map.
HARDWARE = [
    (0x000000, 0x400, "xn_zero_page", r"u32 \*zero = 0",
     "the zero page: the real-mode interrupt vectors (CauseWay maps the first megabyte at 0); "
     "xn_sys_zero_page_save / _check"),
    (0x00000C, 2, "linear 0Ch", r"\*\(volatile u16 \*\)0x0C",
     "Q-ANIM-01: xn_anim_reset_with_rate's speed goes through the reset's 0 to linear 0Ch"),
    (0x000417, 1, "XN_BIOS_KBD_FLAGS", r"0x417", "the BIOS keyboard flags (0040:0017)"),
    (0x00046C, 4, "XN_BIOS_TICKS", r"0x46C", "the BIOS tick count (0040:006C), 18.2 a second"),
    (0x0A0000, 0x10000, "VGA_MEMORY", r"0xA0000",
     "VGA memory: the screen in mode 13h and the VESA bank window (gfx.c); screen_buffer and "
     "xn_gfx_buffer_base start there"),
    (0x40006C, 4, "linear 40006Ch", r"0x40006C",
     "Q-RAND-01: xn_rand_seed_from_ticks reads 0040:006C as a flat address"),
]


# --------------------------------------------------------------------------------------------
# The inventory

FIELDS = ["symbol", "owner", "kind", "address", "size", "ctype", "init", "filled_by", "refs",
          "declared", "boundary", "evidence"]


def boundary_class(lo, hi):
    """The boundary map's class (visible, private, unobserved) of object-2 bytes lo..hi."""
    try:
        import xn_boundary
        rows = xn_boundary.rows_csv()
    except (ImportError, OSError):
        return ""
    out = set()
    for r in rows:
        if r["kind"] != "memory" or r["space"] != "obj" or not r["end"]:
            continue
        a, b = int(r["start"], 16), int(r["end"], 16)
        if a < max(hi, lo + 1) and b > lo:
            out.add(r["class"])
    return "+".join(sorted(out))


def definition(decl, name, elems):
    """The definition's declaration text: the declared one with the array's size filled in."""
    d = decl.declarator
    if elems is not None:
        d = re.sub(r"\b" + name + r"\s*\[\s*\]", "%s[%d]" % (name, elems), d, count=1)
    return "%s %s" % (decl.base, d)


def collect():
    """Compile src/engine and gather what the inventory needs."""
    objs = compile_dir(engine_sources(), os.path.join(WORK, "eng"), [ENGINE])
    refs = references(objs)
    decls = engine_decls()
    return refs, decls


def inventory(refs=None, decls=None, quiet=False):
    import xn_rc
    if refs is None:
        refs, decls = collect()
    exe = Exe()
    resolve = xn_rc.resolver()
    rows = {}
    problems = []
    data_names = sorted(link_name(s)[0] for s in refs if link_name(s)[1] == "data")
    # declarations: every referenced engine data symbol must be declared in a header, and all
    # its declarations must agree
    for n in data_names:
        hs = [d for d in decls[n] if d.file.endswith(".h")]
        a = resolve("_" + n)
        if a is not None and region(a - LOAD) == "obj2" and not hs:
            problems.append("%s: declared only in %s (the data module needs a header's)" % (
                n, ", ".join(sorted({d.file for d in decls[n]}))))
    sizes = probe_sizes(data_names, decls, os.path.join(WORK, "probe"))
    problems += check_c_declarations(refs, decls, os.path.join(WORK, "declcheck"))
    for n in data_names:
        texts = {norm_decl(d) for d in decls[n]}
        if len(texts) > 1 and len({sizes[n]}) == 1:
            # the same type spelt two ways (XN_LIGHTS + 1 and 33) is fine; different types not
            base = {d.base for d in decls[n]}
            if len(base) > 1:
                problems.append("%s: declarations disagree: %s" % (n, " | ".join(sorted(texts))))
    globs = object2_globals()
    import bisect
    gaddr = [a for a, _n in globs]

    def gap(va):
        k = bisect.bisect_right(gaddr, va)
        while k < len(globs) and globs[k][0] <= va:
            k += 1
        return (globs[k][0] if k < len(globs) else OBJ2[1]) - va

    for sym in sorted(refs):
        n, lk = link_name(sym)
        a = resolve(sym)
        if a is None:
            problems.append("%s: no address (names.csv)" % sym)
            continue
        va = a - LOAD
        reg = region(va)
        d0 = decls[n][0] if n in decls else None
        declared = " ".join(sorted({"%s:%d" % (os.path.basename(d.file), d.line)
                                    for d in decls.get(n, [])}))
        row = {"symbol": n, "address": "%06X" % va, "refs": " ".join(refs[sym]),
               "declared": declared, "ctype": d0.text if d0 else "", "size": "",
               "init": "", "filled_by": "", "boundary": "", "evidence": ""}
        if lk == "func":
            if reg == "obj1":
                row.update(owner="game", kind="function",
                           evidence="the game's C (src/lifted, src/hand)")
            elif reg == "library":
                row.update(owner="runtime", kind="library",
                           evidence="a library FALL.EXE links (config/regions.csv: MemCheck, "
                           "the Watcom C runtime, HMI SOS): " + names_evidence(n)[:120])
            elif reg == "obj2":
                row.update(owner="runtime", kind="entry",
                           evidence="an interrupt vector's entry: the build's stub that calls "
                           "the C handler (tools/xn_rc.py isr_stub); " + names_evidence(n)[:120])
            else:
                problems.append("%s: function at %06X" % (n, va))
            rows[n] = row
            continue
        size, elem, inc = sizes[n]
        if reg in ("obj3", "obj1") or (reg == "library"):
            if reg == "obj3":
                row.update(owner="game", kind="data", size=str(size or ""),
                           evidence="the game's data (object 3)")
            elif n in LABELS:
                row.update(owner="runtime", kind="label", evidence="a code address: " +
                           LABELS[n])
            else:
                problems.append("%s: data at %06X (object 1)" % (n, va))
            rows[n] = row
            continue
        if reg != "obj2":
            problems.append("%s: data at %06X" % (n, va))
            continue
        if n in LABELS:
            row.update(owner="runtime", kind="label", evidence="a code address in object 2: "
                       + LABELS[n])
            rows[n] = row
            continue
        # engine data: its extent
        elems = None
        if inc:
            if n in SIZES:
                elems, why = SIZES[n]
            else:
                elems, why = gap(va) // elem, "up to the next named global"
            size = elems * elem
            row["evidence"] = "size: " + why + "; "
        row["size"] = str(size)
        row["ctype"] = definition(d0, n, elems if inc else None)
        rows[n] = dict(row, owner="engine")
    # data the engine's data points at (pointer tables): their targets
    targets = {}
    for n, row in list(rows.items()):
        if row["owner"] != "engine":
            continue
        va, size = int(row["address"], 16), int(row["size"])
        for f in exe.fixups_in(va, va + size):
            t = f.target_va
            if region(t) == "obj1":
                tn = name_at(t)
                targets.setdefault(tn, ("game", t, []))[2].append("%s+%d" % (n, f.src_va - va))
            elif t in SERIAL_ENTRIES:
                targets.setdefault(SERIAL_ENTRIES[t], ("entry", t, []))[2].append(
                    "%s+%d" % (n, f.src_va - va))
            elif region(t) == "obj2":
                if any(int(r["address"], 16) <= t < int(r["address"], 16) + int(r["size"] or 0)
                       for r in rows.values() if r["owner"] == "engine"):
                    continue
                k = bisect.bisect_right(gaddr, t) - 1
                gn = globs[k][1]
                targets.setdefault(gn, ("data", globs[k][0], []))[2].append(
                    "%s+%d" % (n, f.src_va - va))
            else:
                problems.append("%s+%d: a pointer to %06X" % (n, f.src_va - va, t))
    for tn, (what, t, frm) in sorted(targets.items()):
        if tn in rows:
            continue
        refs_txt = "pointer: " + " ".join(frm[:3]) + (" (+%d)" % (len(frm) - 3) if len(frm) > 3
                                                        else "")
        if what == "game":
            rows[tn] = {"symbol": tn, "owner": "game", "kind": "function", "address": "%06X" % t,
                        "size": "", "ctype": "", "init": "", "filled_by": "", "refs": refs_txt,
                        "declared": "", "boundary": "",
                        "evidence": "the game's C; the engine's data holds its address"}
        elif what == "entry":
            rows[tn] = {"symbol": tn, "owner": "runtime", "kind": "entry", "address": "%06X" % t,
                        "size": "", "ctype": "void %s(void)" % tn, "init": "", "filled_by": "",
                        "refs": refs_txt, "declared": "", "boundary": "",
                        "evidence": "an IRQ handler's entry the serial table holds: "
                        + ("xn_serial_irq_com1's (the build's interrupt stub)" if t == 0x160AE0
                           else "the asm's copy of COM1's handler for another port (no C: "
                           "nothing installs it; the game opens COM1 only)")}
        else:
            rows[tn] = {"symbol": tn, "owner": "engine", "kind": "", "address": "%06X" % t,
                        "size": "", "ctype": "", "init": "", "filled_by": "", "refs": refs_txt,
                        "declared": "src/engine_data/xndata.h", "boundary": "",
                        "evidence": ""}
    # the pointer-reached data's extents and types (the data module declares them)
    extra = {"xn_poly_ring_tables": ("struct xn_poly_vertex *xn_poly_ring_tables[%d]", 4, None,
                                     "up to the next named global (xn_span_dzdx)"),
             "xn_face_edge_slots": ("s32 xn_face_edge_slots[%d]", 4, 319,
                                    "names.csv: int[319], the 22 tables for 3..24 points")}
    for tn, row in rows.items():
        if row["owner"] == "engine" and not row["size"]:
            t = int(row["address"], 16)
            if tn not in extra:
                problems.append("%s: reached through a pointer; give it a type" % tn)
                continue
            fmt, el, n_el, why = extra[tn]
            n_el = n_el or gap(t) // el
            row["size"] = str(n_el * el)
            row["ctype"] = fmt % n_el
            row["evidence"] = "size: %s; " % why
    # object-2 data the game references (object-1 fixups) that no row covers yet
    covered = sorted((int(r["address"], 16), int(r["size"] or 0)) for r in rows.values()
                     if r["owner"] == "engine" and r["size"])
    import xn_rc
    _by, funcs = xn_rc.names()
    game_refs = collections.Counter(
        f.target_va for f in exe.fixups.values()
        if region(f.src_va) in ("obj1", "library") and region(f.target_va) == "obj2" and
        f.kind == 7 and f.target_va not in funcs)
    for tva, nsites in sorted(game_refs.items()):
        if any(a <= tva < a + s for a, s in covered):
            continue
        gn = name_at(tva, "global")
        if gn not in GAME_ONLY:
            problems.append("%06X (%s): the game references it, no row covers it" % (tva, gn))
            continue
        ctype, why = GAME_ONLY[gn]
        rows[gn] = {"symbol": gn, "owner": "engine", "kind": "", "address": "%06X" % tva,
                    "size": "4", "ctype": ctype, "init": "", "filled_by": "",
                    "refs": "game: %d sites" % nsites, "declared": "src/engine_data/xndata.h",
                    "boundary": "", "evidence": why + "; "}
    # the asin table's whole (the header's centred name is an alias of it)
    if "xn_math_asin_values" not in rows:
        problems.append("xn_math_asin_values: not referenced (xmath.h's alias)")
    # kinds
    for n, row in rows.items():
        if row["owner"] != "engine":
            continue
        va, size = int(row["address"], 16), int(row["size"])
        b = exe.bytes_at(va, size)
        fx = exe.fixups_in(va, va + size)
        row["boundary"] = boundary_class(va, va + size)
        ev = row["evidence"]
        if n in COMPUTED:
            how, why = COMPUTED[n]
            row.update(kind="computed", init="compute" if how == "xn_data_compute" else "c",
                       evidence=ev + why)
        elif n in RUNTIME:
            fn, why = RUNTIME[n]
            if any(b) or fx:
                problems.append("%s: filled at run time but not zero in the image" % n)
            row.update(kind="computed", init="runtime", filled_by=fn,
                       evidence=ev + "zero in the image; " + why)
        elif not any(b) and not fx:
            row.update(kind="zero", init="bss", evidence=ev + "all zero in the image")
        else:
            row["kind"] = "initial"
            scalar = "[" not in row["ctype"] and not row["ctype"].startswith(("struct", "xn_")) \
                and size <= 4
            if fx:
                slots = size // 4
                if len(fx) != slots or any(f.src_va % 4 != va % 4 for f in fx):
                    problems.append("%s: pointers among other bytes (%d of %d slots)" % (
                        n, len(fx), slots))
                row.update(init="c", evidence=ev + "a table of %d code addresses: an "
                           "initializer names them (FALL.EXE's fixups)" % len(fx))
            elif scalar:
                row.update(init="c", evidence=ev + "a scalar default: a C initializer (%s)" %
                           scalar_value(row["ctype"], b))
            else:
                why = NOT_COMPUTED.get(n, "")
                row.update(init="load", evidence=ev + ("%d non-zero bytes" % sum(1 for x in b if x))
                           + ("; " + why if why else "") + _text_hint(b))
    # aliases
    for n, (target, off, expr, why) in sorted(ALIASES.items()):
        t = rows.get(target)
        if t is None:
            problems.append("alias %s: %s is not engine data" % (n, target))
            continue
        a = resolve("_" + n)
        exp = int(t["address"], 16) + off
        if a is not None and a - LOAD != exp:
            problems.append("alias %s: names.csv has %06X, %s + %d is %06X" % (
                n, a - LOAD, target, off, exp))
        if "_" + n in refs:
            problems.append("alias %s: still referenced as a symbol (the header's macro?)" % n)
        rows[n] = {"symbol": n, "owner": "engine", "kind": "alias", "address": "%06X" % exp,
                   "size": "", "ctype": "#define %s (%s)" % (n, expr), "init": "",
                   "filled_by": "", "refs": "", "declared": "", "boundary": "",
                   "evidence": "%s + %d: %s" % (target, off, why)}
    # the absolute addresses
    srcs = {p: open(p, encoding="latin-1").read() for p in
            sorted(glob.glob(os.path.join(ENGINE, "*.[ch]")))}
    for va, size, n, pat, why in HARDWARE:
        where = sorted("%s:%d" % (os.path.basename(p), t[:m.start()].count("\n") + 1)
                       for p, t in srcs.items() for m in re.finditer(pat, t))
        if not where:
            problems.append("hardware %s: not found in src/engine" % n)
        rows[n] = {"symbol": n, "owner": "hardware", "kind": "memory", "address": "%06X" % va,
                   "size": str(size), "ctype": "", "init": "", "filled_by": "",
                   "refs": " ".join(where[:4]), "declared": "", "boundary": "", "evidence": why}
    # overlaps between engine symbols (aliases aside)
    eng = sorted((int(r["address"], 16), int(r["size"]), n) for n, r in rows.items()
                 if r["owner"] == "engine" and r["kind"] != "alias")
    overlaps = []
    for (a1, s1, n1), (a2, s2, n2) in zip(eng, eng[1:]):
        if a1 + s1 > a2:
            overlaps.append((n1, n2, a1 + s1 - a2))
    for n1, n2, k in overlaps:
        rows[n1]["evidence"] += "; its last %d bytes are %s's in the asm (separate in C)" % (k, n2)
    out = [rows[n] for n in sorted(rows, key=lambda n: (rows[n]["owner"], rows[n]["address"], n))]
    return out, problems, overlaps


def scalar_value(ctype, b):
    v = int.from_bytes(b, "little")
    signed = ctype.split()[0] in ("s32", "s16", "s8", "int", "short", "char") and "*" not in ctype
    if signed and v >= 1 << (8 * len(b) - 1):
        v -= 1 << (8 * len(b))
    return "%d" % v if -1000 < v < 1000 else ("-0x%X" % -v if v < 0 else "0x%X" % v)


def _text_hint(b):
    """'; text' when the bytes read as text ('$'- or NUL-ended)."""
    s = bytes(b).rstrip(b"\0")
    if s and all(32 <= c < 127 or c in (9, 10, 13) for c in s):
        return "; text (%d characters)" % len(s)
    return ""


def write_csv(rows, path=CSV_PATH):
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=FIELDS, lineterminator="\n")
        w.writeheader()
        for r in rows:
            w.writerow({k: r.get(k, "") for k in FIELDS})


def read_csv(path=CSV_PATH):
    with open(path, newline="") as f:
        return list(csv.DictReader(f))


def cmd_inventory(a):
    rows, problems, overlaps = inventory()
    write_csv(rows)
    c = collections.Counter((r["owner"], r["kind"]) for r in rows)
    print("%d rows -> %s" % (len(rows), rel(CSV_PATH)))
    for (o, k), v in sorted(c.items()):
        print("  %-9s %-9s %4d" % (o, k, v))
    ini = collections.Counter(r["init"] for r in rows if r["owner"] == "engine")
    print("  engine data by init:", ", ".join("%s %d" % kv for kv in sorted(ini.items()) if kv[0]))
    for n1, n2, k in overlaps:
        print("  overlap: %s's last %d bytes are %s's in the asm" % (n1, k, n2))
    for p in problems:
        print("PROBLEM", p)
    return 1 if problems else 0


# --------------------------------------------------------------------------------------------
# The generated parts of the data module

GEN_NOTE = ("Generated by tools/xn_datagen.py gen from config/xngine_data.csv (and FALL.EXE's\n"
            "   fixups, for the code-address tables): do not edit.")


def home_header(row):
    """The header a symbol's definition is grouped under: the declaring header whose name the
    symbol starts with (xn_cam_* -> xcam.h), else the first."""
    hs = [h.split(":")[0] for h in row["declared"].split()]
    hs = [os.path.basename(h) for h in hs if h.endswith(".h")]
    stem = row["symbol"][3:] if row["symbol"].startswith("xn_") else row["symbol"]
    for h in sorted(hs, key=len, reverse=True):
        if stem.startswith(h[1:-2] + "_") or stem.startswith(h[1:-2]):
            return h
    return sorted(hs)[0] if hs else "xndata.h"


def fn_table_type(ctype, name):
    """For a table of function pointers `R (*name[N])(P)`: (R, P)."""
    m = re.match(r"^(.*?)\(\s*\*\s*" + name + r"\s*\[\s*\d+\s*\]\s*\)\s*(\(.*\))\s*$", ctype)
    return (m.group(1).strip(), m.group(2)) if m else (None, None)


def initializer(row, exe, fnames):
    """The C initializer of a row whose init is 'c' (or None)."""
    n = row["symbol"]
    va, size = int(row["address"], 16), int(row["size"])
    b = exe.bytes_at(va, size)
    if n == "xn_pow10_table":
        return wrap_items([str(10 ** k) for k in range(10)])
    fx = exe.fixups_in(va, va + size)
    if fx:
        ret, params = fn_table_type(row["ctype"], n)
        if ret is None:
            raise SystemExit("%s: a pointer table that is not of function pointers" % n)
        items = []
        for k in range(size // 4):
            f = exe.fixups.get(va + 4 * k)
            tn = fnames[f.target_va]
            items.append(tn if fn_decl_type(tn) == (ret, params)
                         else "(%s (*)%s)%s" % (ret, params, tn))
        return wrap_items(items)
    v = scalar_value(row["ctype"], b)
    if "*" in row["ctype"]:
        base = row["ctype"].split("*")[0].strip()
        return "(%s *)%s" % (base, v)
    return v


def wrap_items(items):
    lines, cur = [], "    "
    for it in items:
        if len(cur) + len(it) + 2 > 96:
            lines.append(cur.rstrip())
            cur = "    "
        cur += it + ", "
    lines.append(cur.rstrip().rstrip(","))
    return "{\n" + "\n".join(lines) + "\n}"


_fn_types = {}


def fn_decl_type(name):
    return _fn_types.get(name)


def table_functions(rows, exe):
    """{target address: name} of the code-address tables' functions, and the declarations
    ddefs.c gives them: each with the type of the smallest table that holds it."""
    fnames, decl_from = {}, {}
    by_addr = {int(r["address"], 16): r["symbol"] for r in rows
               if (r["owner"], r["kind"]) in (("game", "function"), ("runtime", "entry"))}
    for r in rows:
        if r["owner"] != "engine" or r["init"] != "c":
            continue
        va, size = int(r["address"], 16), int(r["size"])
        fx = exe.fixups_in(va, va + size)
        if not fx:
            continue
        ret, params = fn_table_type(r["ctype"], r["symbol"])
        for f in fx:
            tn = by_addr.get(f.target_va)
            if tn is None:
                raise SystemExit("%s: no inventory row for the function at %06X" % (
                    r["symbol"], f.target_va))
            fnames[f.target_va] = tn
            if tn not in decl_from or decl_from[tn][0] > size:
                decl_from[tn] = (size, ret, params)
    _fn_types.clear()
    for tn, (_s, ret, params) in decl_from.items():
        _fn_types[tn] = (ret, params)
    return fnames


def all_headers_include():
    return "\n".join('#include "%s"' % h for h in header_names())


HOW = {"bss": "", "load": "xn_data_load", "compute": "xn_data_compute", "c": ""}


def gen_defs(rows, exe):
    eng = [r for r in rows if r["owner"] == "engine" and r["kind"] != "alias"]
    fnames = table_functions(rows, exe)
    groups = collections.defaultdict(list)
    for r in eng:
        groups[home_header(r)].append(r)
    order = sorted(groups, key=lambda h: min(int(r["address"], 16) for r in groups[h]))
    out = ["/* ddefs.c: the definitions of XnGine's data, for a build of the engine outside",
           "   FALL.EXE (xndata.h, docs/engine/data.md).",
           "   " + GEN_NOTE,
           "",
           "   Every object-2 symbol src/engine references, with the type its headers declare",
           "   (all of them are included: the compiler checks each definition against every",
           "   declaration). The comment gives the symbol's address in FALL.EXE and how it gets",
           "   its value: nothing (zero), an initializer (a default equal to FALL.EXE's, or the",
           "   functions FALL.EXE's fixups name), xn_data_load (FALL.EXE's bytes), xn_data_compute,",
           "   or the engine function that fills it at run time. */",
           all_headers_include(), '#include "xndata.h"', ""]
    decls = sorted(_fn_types.items())
    if decls:
        out.append("/* ---- the functions the code-address tables name: the game's (src/lifted,")
        out.append("   src/hand) and the IRQ entries (the platform's) ------------------------------ */")
        for tn, (ret, params) in decls:
            out.append("%s %s%s;" % (ret, tn, params))
        out.append("")
    for h in order:
        out.append("/* ---- %s %s */" % (h, "-" * max(3, 86 - len(h))))
        for r in sorted(groups[h], key=lambda r: int(r["address"], 16)):
            d = r["ctype"]
            how = r["filled_by"] if r["init"] == "runtime" else HOW.get(r["init"], "")
            com = "/* %s%s */" % (r["address"], ": " + how if how else "")
            if r["init"] == "c":
                ini = initializer(r, exe, fnames)
                if "\n" in ini:
                    out.append("%s = %s;   %s" % (d, ini, com))
                    continue
                d = "%s = %s" % (d, ini)
            if len(d) > 62:
                out.append(com)
                out.append(d + ";")
                continue
            out.append("%-63s %s" % (d + ";", com))
        out.append("")
    return "\n".join(out).rstrip() + "\n"


def gen_table(rows):
    items = [r for r in rows if r["owner"] == "engine" and r["init"] == "load"]
    out = ["/* dtable.c: the initial data xn_data_load copies from the user's FALL.EXE (xndata.h,",
           "   docs/engine/data.md): each symbol's storage, address in FALL.EXE and size.",
           "   " + GEN_NOTE + " */",
           all_headers_include(), '#include "xndata.h"', "",
           "const xn_data_item xn_data_items[] = {"]
    total = 0
    for r in sorted(items, key=lambda r: int(r["address"], 16)):
        out.append("    { (void *)&%s, 0x%s, %s }," % (r["symbol"], r["address"], r["size"]))
        total += int(r["size"])
    out.append("};")
    out.append("")
    out.append("const u32 xn_data_item_count = %d;     /* %d bytes */" % (len(items), total))
    return "\n".join(out) + "\n"


def cmd_gen(a):
    rows = read_csv()
    exe = Exe()
    os.makedirs(DATA_SRC, exist_ok=True)
    for name, text in (("ddefs.c", gen_defs(rows, exe)), ("dtable.c", gen_table(rows))):
        p = os.path.join(DATA_SRC, name)
        old = open(p).read() if os.path.exists(p) else None
        if old != text:
            with open(p, "w") as f:
                f.write(text)
        print("%s %s (%d lines)" % ("wrote" if old != text else "unchanged", rel(p),
                                    text.count("\n")))
    return 0


# --------------------------------------------------------------------------------------------
# Proof (a): the standalone link

SBASE = 0x20000000                  # the standalone image (outside the emulator's game memory)
STUB_SIZE = 0x200                   # a stub: `ret` for a function, zeroed storage for data
STANDALONE = os.path.join(WORK, "standalone.pkl")
# the compiler's support for floating point (dcompute.c's sine): Watcom's 80x87 emulator
# hooks; not called (the code runs on an 80x87), a port's compiler has none
COMPILER_SUPPORT = {"__8087": "Watcom's flag of an 80x87 (data)",
                    "__init_387_emulator": "Watcom's 80x87 emulator start-up (pulled in by "
                                           "floating-point code; never called)"}


def outside_symbols(rows):
    """{link name: (owner, kind)} the standalone link may leave outside: the game's and the
    runtime's (stubs), and the compiler's floating-point support."""
    out = {}
    for r in rows:
        if r["owner"] not in ("game", "runtime"):
            continue
        func = r["kind"] in ("function", "library", "entry")
        out[(r["symbol"] + "_") if func else ("_" + r["symbol"])] = (r["owner"], r["kind"])
    for s in COMPILER_SUPPORT:
        out[s] = ("runtime", "compiler")
    return out


def cmd_link(a):
    import xn_cload
    rows = read_csv()
    srcs = engine_sources() + data_sources()
    objs = compile_dir(srcs, os.path.join(WORK, "standalone"), [ENGINE, DATA_SRC])
    paths = [objs[s] for s in srcs]
    undefined = references({s: objs[s] for s in srcs})
    allowed = outside_symbols(rows)
    stubs = {}
    refused = []
    base_stubs = None

    def resolve(name):
        # never FALL.EXE: the game's and the runtime's symbols get stubs, nothing else resolves
        if name not in allowed:
            if name not in refused:
                refused.append(name)
            return None
        if name not in stubs:
            stubs[name] = len(stubs)
        return base_stubs + STUB_SIZE * stubs[name]

    # lay out once to learn the image's end, then link with the stubs after it
    try:
        img, syms = xn_cload.link(paths, SBASE, lambda n: SBASE if n in allowed else None)
    except SystemExit as e:
        img, syms = None, {}
        print("link (sizing):", str(e)[:300])
    base_stubs = SBASE + ((len(img or b"") + 0xFFF) & ~0xFFF)
    try:
        img, syms = xn_cload.link(paths, SBASE, resolve)
    except SystemExit as e:
        print("LINK FAILED:", str(e)[:2000])
        img = None
    # the data module defines every engine symbol; nothing it defines is outside the inventory
    data_objs = {s for s in srcs if s.startswith(DATA_SRC)}
    data_pubs = set()
    for s in data_objs:
        data_pubs |= set(xn_cload.Obj(objs[s]).pubs)
    eng = [r for r in rows if r["owner"] == "engine" and r["kind"] != "alias"]
    missing = [r["symbol"] for r in eng if "_" + r["symbol"] not in data_pubs]
    interface = {"xn_data_object2_", "xn_data_load_", "xn_data_compute_", "_xn_data_items",
                 "_xn_data_item_count"}
    extra = sorted(p for p in data_pubs if p not in interface and
                   p[1:] not in {r["symbol"] for r in eng})
    by_owner = collections.Counter(allowed[n] for n in stubs)
    unfixed = sorted(n for n in undefined if n not in stubs and n in allowed)
    print("standalone link: %d objects (%d engine, %d data module), image %d bytes at %08X" % (
        len(paths), len(srcs) - len(data_objs), len(data_objs), len(img or b""), SBASE))
    print("  undefined by the objects: %d symbols, all outside FALL.EXE: %s" % (
        len(undefined), ", ".join("%d %s %s" % (v, o, k) for (o, k), v in
                                  sorted(by_owner.items()))))
    for n in unfixed:
        print("  declared, no fixup: %s (%s; %s)" % (n, COMPILER_SUPPORT.get(n, ""),
                                                    ", ".join(undefined[n])))
    print("  engine symbols the data module defines: %d of %d" % (len(eng) - len(missing),
                                                                  len(eng)))
    for n in missing:
        print("  NOT DEFINED", n)
    for n in extra:
        print("  EXTRA (not in the inventory)", n)
    for n in refused:
        print("  REFUSED", n)
    ok = img is not None and not missing and not extra and not refused
    if img is not None:
        stub_area = bytearray(STUB_SIZE * len(stubs))
        for n, k in stubs.items():
            if allowed[n][1] in ("function", "library", "entry"):
                stub_area[STUB_SIZE * k] = 0xC3            # ret
        data = bytes(img) + b"\0" * (base_stubs - SBASE - len(img)) + bytes(stub_area)
        with open(STANDALONE, "wb") as f:
            pickle.dump({"base": SBASE, "bytes": data, "syms": syms,
                         "stubs": {n: base_stubs + STUB_SIZE * k for n, k in stubs.items()},
                         "stub_kinds": {n: allowed[n] for n in stubs},
                         "sources": [rel(s) for s in srcs]}, f)
        print("  -> %s (%d bytes with %d stubs)" % (rel(STANDALONE), len(data), len(stubs)))
    print("PROOF (a): %s" % ("OK: no FALL.EXE address; zero unresolved engine symbols" if ok
                             else "FAILED"))
    return 0 if ok else 1


# --------------------------------------------------------------------------------------------
# Proof (b): the data equal to FALL.EXE's

CHECK_JSON = os.path.join(WORK, "check.json")
HALT = 0x30000000                   # a page whose first byte, when executed, ends a call
HEAP = 0x28000000                   # malloc, FALL.EXE's bytes, object 2's buffer
HEAP_SIZE = 0x02000000
STACK_TOP = 0x2FF00000
STACK_SIZE = 0x00100000
LOW = 0x110000                      # the first megabyte (the zero page, the BIOS data area)


class Bare:
    """A bare 32-bit machine (Unicorn, no game, no DOS) running the standalone image: the
    game's and the runtime's symbols are stubs a hook answers (the game's allocator gives
    heap blocks; the others return 0), DPMI and DOS calls succeed."""

    def __init__(self, image):
        import fallemu      # noqa: F401  (the patched Unicorn, when built)
        from unicorn import Uc, UC_ARCH_X86, UC_MODE_32, UC_HOOK_CODE, UC_HOOK_INTR
        from unicorn import x86_const as X
        self.X = X
        self.uc = uc = Uc(UC_ARCH_X86, UC_MODE_32)
        self.img = image
        base, data = image["base"], image["bytes"]
        uc.mem_map(0, LOW)
        uc.mem_map(base, (len(data) + 0xFFFF) & ~0xFFFF)
        uc.mem_write(base, data)
        uc.mem_map(HEAP, HEAP_SIZE)
        uc.mem_map(STACK_TOP - STACK_SIZE, STACK_SIZE)
        uc.mem_map(HALT, 0x1000)
        uc.mem_write(HALT, b"\xF4")
        self.heap = HEAP + 0x01000000       # the allocator's blocks (below: buffers)
        self.calls = collections.Counter()
        self.by_addr = {a: n for n, a in image["stubs"].items()}
        lo, hi = min(self.by_addr), max(self.by_addr) + STUB_SIZE
        uc.hook_add(UC_HOOK_CODE, self._stub, None, lo, hi)
        uc.hook_add(UC_HOOK_CODE, lambda u, a, s, d: u.emu_stop(), None, HALT, HALT)
        uc.hook_add(UC_HOOK_INTR, self._int)
        self.ints = collections.Counter()

    def r(self, reg):
        return self.uc.reg_read(getattr(self.X, "UC_X86_REG_" + reg.upper()))

    def w(self, reg, v):
        self.uc.reg_write(getattr(self.X, "UC_X86_REG_" + reg.upper()), v & 0xFFFFFFFF)

    def read(self, a, n):
        return bytes(self.uc.mem_read(a, n))

    def write(self, a, b):
        self.uc.mem_write(a, bytes(b))

    def u32(self, a):
        return struct.unpack("<I", self.read(a, 4))[0]

    def malloc(self, n):
        a = (self.heap + 0x1F) & ~0x1F
        self.heap = a + n
        if self.heap > HEAP + HEAP_SIZE:
            raise SystemExit("the bare machine's heap is full")
        return a

    def _stub(self, uc, address, size, _):
        name = self.by_addr.get(address)
        if name is None:
            return
        self.calls[name] += 1
        ret = 0
        if name == "func_000A10A8_":            # the game's allocator (malloc)
            ret = self.malloc(self.r("eax"))
        elif name in ("fatal_error_", "exit_"):
            raise SystemExit("the engine called %s" % name)
        esp = self.r("esp")
        self.w("eax", ret)
        self.w("eip", self.u32(esp))
        self.w("esp", esp + 4)

    def _int(self, uc, intno, _):
        """DPMI (31h) and DOS (21h): every call succeeds (carry clear); a selector or segment
        asked for is 0x28, a base 0."""
        ax = self.r("eax") & 0xFFFF
        self.ints[(intno, ax)] += 1
        if intno == 0x31 and ax in (0x0002, 0x0100):
            self.w("eax", 0x28)
            self.w("edx", 0x28)
        elif intno == 0x21 and ax >> 8 in (0x51, 0x62):
            self.w("ebx", 0x1000)
        else:
            self.w("ecx", 0)
            self.w("edx", 0)
        self.w("eflags", self.r("eflags") & ~1)

    def sym(self, name):
        return self.img["syms"][name]

    def call(self, name, *args):
        """Calls C function `name` with Watcom's register arguments; returns EAX."""
        for reg, v in zip(("eax", "edx", "ebx", "ecx"), args):
            self.w(reg, v)
        esp = STACK_TOP - 0x100
        self.write(esp, struct.pack("<I", HALT))
        self.w("esp", esp)
        self.w("ebp", 0)
        self.uc.emu_start(self.sym(name + "_"), HALT + 1)
        if self.r("eip") != HALT:
            raise SystemExit("%s did not return (EIP %08X)" % (name, self.r("eip")))
        return self.r("eax")

    def close(self):
        if self.uc is not None:
            self.uc._Uc__finalizer()
            self.uc = None


class Owners:
    """Address -> (symbol, offset) in FALL.EXE (by the inventory) and in the standalone image
    (by its link map): pointers compare by what they point at."""

    def __init__(self, rows, img):
        import bisect
        self.bisect = bisect
        ex, cx = [], []
        for r in rows:
            if r["kind"] == "alias" or r["owner"] == "hardware":
                continue
            va = int(r["address"], 16)
            size = int(r["size"]) if r["size"] else 1
            ex.append((va, size, r["symbol"]))
            func = r["kind"] in ("function", "library", "entry")
            ln = (r["symbol"] + "_") if func else ("_" + r["symbol"])
            a = img["syms"].get(ln, img["stubs"].get(ln))
            if a is not None:
                cx.append((a, size if r["owner"] == "engine" else STUB_SIZE, r["symbol"]))
        self._e, self._c = sorted(ex), sorted(cx)
        self._ek, self._ck = [x[0] for x in self._e], [x[0] for x in self._c]

    @staticmethod
    def _find(tab, keys, a):
        import bisect
        k = bisect.bisect_right(keys, a) - 1
        if k >= 0 and tab[k][0] <= a < tab[k][0] + max(tab[k][1], 1):
            return tab[k][2], a - tab[k][0]
        return None

    def exe(self, va):
        return self._find(self._e, self._ek, va)

    def c(self, a):
        return self._find(self._c, self._ck, a)


def expected_exceptions(rows):
    """Bytes where the C differs from FALL.EXE on purpose: {symbol: [(offset, length, why)]}."""
    out = collections.defaultdict(list)
    # the asm's padding between the ring blocks (TASM's filler, never read: the walker reads
    # ring n at -n..2n-1, and the n = 28 rings hold 60 and 56 entries)
    out["xn_poly_ring_tables"] += [(1209 * 4, 7 * 4, "the asm's padding after ring block "
                                    "A (TASM's filler 0xDB87DB87; never read)"),
                                   (2425 * 4, 7 * 4, "the asm's padding after ring block B "
                                    "(never read)")]
    return out


def compare_symbol(row, exe, own, c_addr, cmem, exc):
    """Differences of one symbol: [(offset, what)]."""
    va, size = int(row["address"], 16), int(row["size"])
    e = exe.bytes_at(va, size)
    done = bytearray(size)
    diffs = []
    for f in exe.fixups_in(va, va + size):
        o = f.src_va - va
        if o + 4 > size:
            continue
        want = own.exe(f.target_va) or ("%06X" % f.target_va, 0)
        cv = struct.unpack_from("<I", cmem, o)[0]
        got = own.c(cv) or ("%08X" % cv, 0)
        if want != got:
            diffs.append((o, "pointer to %s+%d, C has %s+%d" % (want + got)))
        done[o:o + 4] = b"\1\1\1\1"
    for o in range(size):
        if done[o] or e[o] == cmem[o]:
            continue
        diffs.append((o, "byte %02X, C has %02X" % (e[o], cmem[o])))
    # the expected ones
    keep = []
    for o, what in diffs:
        why = next((w for (eo, n, w) in exc.get(row["symbol"], []) if eo <= o < eo + n), None)
        if why is None:
            keep.append((o, what))
    explained = len(diffs) - len(keep)
    return keep, explained


SNAPSHOT = os.path.join(ROOT, "build", "emu", "snap", "save_blades.snap")


def rand_stream(seed):
    """xn_rand_next's numbers from `seed` (a model of rand.c, only to find the seed an init
    ran from: the comparison then runs the C)."""
    while True:
        seed = (seed & 0xFFFF) * 797 % 4099
        yield seed


def water_model(seed):
    g = rand_stream(seed)
    jit = [max(-1, min(1, (next(g) & 0x3F) - 0x20)) for _ in range(512)]
    phase, base, vel = [], [], []
    for _ in range(200):
        phase.append((next(g) & 7) - 4)
        v = next(g) & 0x7F
        base.append(8 if v < 8 else v)
        vel.append(((next(g) & 1) - 1) | 1)
    return jit, phase, base, vel


def star_colours_from(seed, palette, want):
    """True when xn_sky_init_stars from `seed` gives the colours `want` (stopping at the first
    that differs; a draw that never ends, as from a seed in a short cycle, is no match)."""
    g = rand_stream(seed)
    for k in range(768):
        for _tries in range(10000):
            c = palette[next(g) & 0xF]
            polar, az = next(g) & 0x7FF, next(g) & 0x7FF
            if not (az < 0x400 or polar >= 0x400):
                break
        else:
            return False
        if c != want[k]:
            return False
    return True


def check_runtime(m, rows, exe, img, report):
    """The tables the engine's own init functions fill at run time, compared after running
    those inits in both worlds: the asm's ran at the game's start (a snapshot of the original
    game in tools/fallemu.py, after its boot); the C's run in the bare machine from the data
    module's start-up state."""
    import fallemu
    snap = fallemu.snapshot_memory(SNAPSHOT)
    by = {r["symbol"]: r for r in rows}

    def asm(name, n=None):
        r = by[name]
        va = int(r["address"], 16)
        return snap[va:va + (n or int(r["size"]))]

    def c(name, n=None):
        return m.read(m.sym("_" + name), n or int(by[name]["size"]))

    def s32s(b):
        return list(struct.unpack("<%di" % (len(b) // 4), b))

    results = {}

    def same(label, name, a_bytes, c_bytes, how):
        eq = a_bytes == c_bytes
        nd = sum(1 for x, y in zip(a_bytes, c_bytes) if x != y) if not eq else 0
        results[label] = {"symbol": name, "equal": eq, "bytes": len(a_bytes),
                          "different_bytes": nd, "how": how}
        print("  %-28s %-10s %s (%d bytes)%s" % (label, "equal" if eq else "DIFFERENT", how,
                                                 len(a_bytes), "" if eq else ": %d differ" % nd))
        return eq

    print("run-time tables: the C's inits in the bare machine against the original's after its "
          "boot (%s)" % rel(SNAPSHOT))
    ok = True
    m.call("xn_mem_init", 0x10000)
    for n in ("xn_recip16_table", "xn_recip32_table", "xn_colour_fill_table"):
        ok &= same(n, n, asm(n), c(n), "xn_mem_init")
    m.call("xn_rand_noise_init")
    for n in ("xn_noise_values", "xn_noise_fade_table"):
        ok &= same(n, n, asm(n), c(n), "xn_rand_noise_init (seed 1)")
    m.call("xn_render_init")
    for n in ("xn_squares_table", "xn_tex_size_mask"):
        ok &= same(n, n, asm(n), c(n), "xn_render_init")
    # the depth reciprocals: a block from the game's allocator; entry 0 is never written
    pa = struct.unpack("<I", asm("xn_render_recip_table"))[0] - LOAD
    pc = struct.unpack("<I", c("xn_render_recip_table"))[0]
    ok &= same("*xn_render_recip_table[1..]", "xn_render_recip_table",
               snap[pa + 4:pa + 0x40004], m.read(pc + 4, 0x40000), "xn_render_init")
    # the camera's rays and scales for the view the snapshot has
    view = s32s(snap[0xCEA28:0xCEA38]) + s32s(snap[0xCEA38:0xCEA3C]) + s32s(snap[0xCEA40:0xCEA44])
    m.call("xn_cam_set_view_window", *view[:4])
    m.call("xn_cam_set_focal", view[4], view[5])
    how = "xn_cam_set_view_window(%d, %d, %d, %d), xn_cam_set_focal(%d, %d)" % tuple(view)
    for n in ("xn_cam_dir_x_table", "xn_cam_dir_y_table"):
        ok &= same(n, n, asm(n), c(n), how)
    for n in ("xn_cam_scale_x", "xn_cam_inv_scale_x", "xn_cam_scale_y", "xn_cam_inv_scale_y",
              "xn_cam_hfov_cos", "xn_cam_hfov_sin", "xn_cam_vfov_cos", "xn_cam_vfov_sin",
              "xn_cam_inv_focal_x", "xn_cam_inv_focal_y", "xn_cam_flat_scale_x",
              "xn_cam_flat_scale_y"):
        ok &= same(n, n, asm(n), c(n), "the same calls")
    # the random tables: the seed each init ran from (found by a model of xn_rand_next), then
    # the C's init from it
    jit = s32s(asm("xn_water_jitter"))
    rows_ = [s32s(asm(n)) for n in ("xn_water_row_phase", "xn_water_row_base",
                                    "xn_water_row_velocity")]
    seeds = [s for s in range(1, 4099) if water_model(s) == (jit, rows_[0], rows_[1], rows_[2])]
    if seeds:
        m.write(m.sym("_xn_rand_seed"), struct.pack("<I", seeds[0]))
        m.call("xn_water_init")
        for n in ("xn_water_jitter", "xn_water_row_phase", "xn_water_row_base",
                  "xn_water_row_velocity"):
            ok &= same(n, n, asm(n), c(n), "xn_water_init from seed %d" % seeds[0])
    else:
        print("  water: no seed reproduces the snapshot's tables")
        ok = False
    pal = list(asm("xn_sky_star_palette"))
    cols = list(asm("xn_sky_star_colours"))
    seeds = [s for s in range(1, 4099) if star_colours_from(s, pal, cols)]
    if seeds:
        m.write(m.sym("_xn_rand_seed"), struct.pack("<I", seeds[0]))
        m.call("xn_sky_init_stars")
        for n in ("xn_sky_stars", "xn_sky_star_colours"):
            ok &= same(n, n, asm(n), c(n), "xn_sky_init_stars from seed %d" % seeds[0])
    else:
        print("  sky: no seed reproduces the snapshot's colours")
        ok = False
    report["runtime"] = results
    report["runtime_snapshot"] = rel(SNAPSHOT)
    report["stub_calls"] = dict(m.calls)
    report["services"] = {"%02Xh %04Xh" % k: v for k, v in m.ints.items()}
    n_eq = sum(1 for v in results.values() if v["equal"])
    print("PROOF (b), at run time: %d of %d tables and values equal after the inits" % (
        n_eq, len(results)))
    return ok


def cmd_check(a):
    rows = read_csv()
    exe = Exe()
    img = pickle.load(open(STANDALONE, "rb"))
    own = Owners(rows, img)
    exc = expected_exceptions(rows)
    report = {"load": {}, "symbols": {}, "runtime": {}}
    m = Bare(img)
    ok = True
    try:
        # FALL.EXE's bytes -> object 2's image (in C), then the loader and the generator
        raw = open(exe.le.path, "rb").read()
        exe_at, buf_at = HEAP, HEAP + 0x00800000
        m.write(exe_at, raw)
        r = m.call("xn_data_object2", exe_at, len(raw), buf_at)
        obj2 = m.read(buf_at, 0xA1568)
        same = obj2 == exe.object2(raw=True)
        print("xn_data_object2(FALL.EXE): %d; object 2's image %s FALL.EXE's pages" % (
            r, "equals" if same else "DIFFERS FROM"))
        report["load"]["object2"] = {"result": r, "equal": same}
        ok &= r == 1 and same
        m.call("xn_data_load", buf_at)
        m.call("xn_data_compute")
        # every engine symbol against the load image
        counts = collections.Counter()
        bad = []
        eng = [r for r in rows if r["owner"] == "engine" and r["kind"] != "alias"]
        for row in eng:
            ca = img["syms"]["_" + row["symbol"]]
            cmem = m.read(ca, int(row["size"]))
            diffs, explained = compare_symbol(row, exe, own, ca, cmem, exc)
            st = "equal" if not diffs and not explained else ("equal but the expected" if not diffs
                                                             else "DIFFERENT")
            counts[(row["kind"], row["init"], not diffs)] += 1
            report["symbols"][row["symbol"]] = {"kind": row["kind"], "init": row["init"],
                                                "size": int(row["size"]), "equal": not diffs,
                                                "expected_differences": explained,
                                                "differences": diffs[:8]}
            if diffs:
                bad.append(row["symbol"])
                print("  %-30s %s: %s" % (row["symbol"], st, "; ".join(
                    "+%d %s" % d for d in diffs[:4])))
            elif explained:
                print("  %-30s equal but %d bytes the asm's padding (%s)" % (
                    row["symbol"], explained, exc[row["symbol"]][0][2]))
        n_eq = sum(v for (k, i, e), v in counts.items() if e)
        print("PROOF (b), at start-up: %d of %d engine symbols equal to FALL.EXE's load image" % (
            n_eq, len(eng)))
        for (k, i, e), v in sorted(counts.items()):
            print("  %-9s %-8s %-9s %d" % (k, i, "equal" if e else "DIFFERENT", v))
        ok &= not bad
        report["at_start"] = {"equal": n_eq, "total": len(eng), "different": bad}
        # the tables the engine's init functions fill at run time, in both worlds
        ok &= check_runtime(m, rows, exe, img, report)
    finally:
        m.close()
    with open(CHECK_JSON, "w") as f:
        json.dump(report, f, indent=1, default=str)
    print("PROOF (b): %s -> %s" % ("OK" if ok else "FAILED", rel(CHECK_JSON)))
    return 0 if ok else 1


def check_c_declarations(refs, decls, workdir):
    """Every declaration a .c file gives a symbol src/engine references, against the headers':
    a file per declaration that includes every engine header and repeats it (the compiler
    rejects one that disagrees). Returns [problem]."""
    import wcc10
    srcs, what = [], []
    os.makedirs(workdir, exist_ok=True)
    head = "\n".join('#include "%s"' % h for h in header_names())
    typedefs = {}
    for sym in sorted(refs):
        n, _lk = link_name(sym)
        for d in decls.get(n, []):
            if not d.file.endswith(".c"):
                continue
            # the .c's own typedefs before the declaration (pc.c's far_handler)
            if d.file not in typedefs:
                txt = strip_comments(open(os.path.join(ROOT, d.file), encoding="latin-1").read())
                typedefs[d.file] = [(txt.count("\n", 0, m.start()) + 1, m.group(0)) for m in
                                    re.finditer(r"^typedef\b[^;{]*;", txt, re.M)]
            local = "\n".join(s for ln, s in typedefs[d.file] if ln < d.line)
            p = os.path.join(workdir, "dc%03d.c" % len(srcs))
            with open(p, "w") as f:
                f.write("%s\n%s\n%s%s;\n" % (head, local, "" if d.func else "extern ", d.text))
            srcs.append(p)
            what.append("%s (%s:%d)" % (n, os.path.basename(d.file), d.line))
    if not srcs:
        return []
    w = os.path.join(workdir, "w")
    shutil.rmtree(w, ignore_errors=True)
    os.makedirs(w)
    for h in glob.glob(os.path.join(ENGINE, "*.h")):
        shutil.copyfile(h, os.path.join(w, os.path.basename(h).upper()))
    objs, td = wcc10.compile_many(srcs, flags(), workdir=w)
    out = []
    for k, s in enumerate(srcs):
        e = os.path.join(td, "N%04d.ERR" % k)
        msg = open(e, errors="replace").read() if os.path.exists(e) else ""
        if objs[s] is None or "Error!" in msg:
            err = [ln for ln in msg.splitlines() if "Error!" in ln][:1]
            out.append("%s: the .c's declaration disagrees with a header's: %s" % (
                what[k], err[0].split("Error!")[1].strip() if err else "?"))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("cmd")
    a = ap.parse_args()
    if a.cmd == "decls":
        ds = engine_decls()
        for n in sorted(ds):
            for d in ds[n]:
                print("%-36s %-5s %s:%d  %s" % (n, "func" if d.func else "data", d.file, d.line,
                                                d.text))
        return 0
    if a.cmd == "inventory":
        return cmd_inventory(a)
    if a.cmd == "gen":
        return cmd_gen(a)
    if a.cmd == "link":
        return cmd_link(a)
    if a.cmd == "check":
        return cmd_check(a)
    if a.cmd == "all":
        for f in (cmd_inventory, cmd_gen, cmd_link, cmd_check):
            r = f(a)
            if r:
                return r
        return 0
    raise SystemExit("unknown command " + a.cmd)


if __name__ == "__main__":
    sys.exit(main())
