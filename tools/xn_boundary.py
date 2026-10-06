#!/usr/bin/env python3
"""XnGine's boundary with the game: where control enters and leaves the engine, and which of
the memory the engine writes the game can see. config/xngine_boundary.csv.

The boundary is what "equivalent" means for canonical C (docs/xngine_canonical.md): a
canonical engine may change everything inside it (registers between its own functions, scratch
globals, code it patched, pools nobody else reads) and must keep everything the game sees.

  static   entries and exits from the code (tools/xn_abi.py's analysis, 40 s):
           - entries: object-1 call sites of object-2 functions (LE fixups) with what the game
             reads after each call (its liveness: tools/xn_abi.py), object-1 code and data
             holding object-2 function addresses, the vectors the engine installs (from a
             snapshot: the int 9 and 1Ch handlers, the divide-error handler...), and object-1
             indirect call sites (traced by `survey`);
           - exits: object-2 calls of the game's functions (malloc, free, the SOS sound and
             timer calls...), its `int` services (AH/AX as tools/xn_abi.py follows them) and
             port instructions;
           - static references: object-1 fixups into object-2 data (globals the game addresses
             directly);
           -> build/xn_canon/boundary/static.json
  allocs   the engine's heap allocations, from a boot through character creation into the
           first dungeon (tools/scenarios/newgame.txt, asm, 3 min): each malloc and free call
           from object 2, its size and block, and the object-2 global that keeps the pointer
           -> build/xn_canon/boundary/allocs.json
  survey   the game-visible memory, found dynamically: each scenario (tools/xn_scenarios.py,
           asm only) runs with memory hooks; per byte of low and program memory: the engine
           wrote it (E), the game read it (G), the game read a byte whose last writer was the
           engine (F, a flow: object 2's data counts as written by the engine at the start), the
           game read a byte nobody wrote in the window (U), the game wrote it (W). Code is
           attributed by EIP: object 1 is the game; object 2 and code run from the heap
           (generated code) are the engine. Also the object-1 indirect calls that land in
           object 2, the ports and services, and the vectors.
           --corpus adds the bytes each record of the corpus writes (E, by function).
           -> build/xn_canon/boundary/survey/NAME.pkl
  write    classify and write config/xngine_boundary.csv and build/xn_canon/boundary/summary.md
  masks    the engine-private ranges as resolved in a snapshot (what a test masks)

Classes of memory (kind memory):
  visible   compared: the game reads what the engine writes there (a flow, or a static
            reference from object-1 code), or it is the screen (VGA memory; the palette goes
            through ports), or game data (object 3, the game's heap)
  private   masked: the engine writes it and the game never reads it (no static reference, no
            flow in any survey). object 2's code (patch fields, planted rets), scratch and pools,
            the engine's own allocations
  unobserved  object-2 data no survey saw written or read: compared (nothing tells it apart)
Anything not listed is compared.

Rules (column `space`): obj (preferred addresses, LOAD added), low (linear, the first MB),
alloc (offsets into the block an object-2 global points to: rule "ptr=GLOBAL size=N", resolved
in the machine at comparison time).

Library: load_masks() -> Masks; Masks.resolve(emu) -> Private (contains(addr), ranges);
entries() -> {va: entry row}.

usage: xn_boundary.py static | allocs | survey [NAME ...] [-j N] [--corpus] | write
       xn_boundary.py masks SNAP | show ADDR|NAME
"""
import argparse
import bisect
import collections
import csv
import glob
import json
import os
import pickle
import re
import struct
import sys
import time
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "build", "xn_canon", "boundary")
SURVEY = os.path.join(OUT, "survey")
STATIC = os.path.join(OUT, "static.json")
ALLOCS = os.path.join(OUT, "allocs.json")
BOUNDARY_CSV = os.environ.get("XN_BOUNDARY") or os.path.join(ROOT, "config", "xngine_boundary.csv")
SUMMARY = os.path.join(OUT, "summary.md")
LOAD = 0x01000000
LOW = 0x110000
OBJ1 = (0x10000, 0xBB27F)
OBJ2 = (0xC0000, 0x161568)
OBJ3 = (0x170000, 0x1B5520)
HEAP = 0x200000
MALLOC, FREE = 0xA10A8, 0xA117E
COLUMNS = ("kind", "space", "start", "end", "name", "class", "rule", "evidence")
# per-byte survey flags
F_E, F_G, F_F, F_U, F_W = 1, 2, 4, 8, 16


# ---- names ------------------------------------------------------------------------------------
_names = None


def names():
    """({address: name} of functions and globals, sorted addresses)."""
    global _names
    if _names is None:
        out = {}
        for p in (os.path.join(ROOT, "config", "names.csv"),
                  os.path.join(ROOT, "build", "xn_readable", "names.csv")):
            if not os.path.exists(p):
                continue
            with open(p, newline="") as f:
                for r in csv.DictReader(f):
                    if r["kind"] in ("func", "global"):
                        out.setdefault(int(r["address"], 16), r["name"])
        _names = (out, sorted(out))
    return _names


# the C library and SOS routines the engine calls that config/names.csv does not name (their
# use in src/engine: malloc and free in mem.c, _dos_setvect in kbd.c, the samples in vid.c)
LIBRARY = {0xA10A8: "malloc", 0xA117E: "free", 0xA12A6: "_dos_setvect",
           0xA2504: "sos_start_sample", 0xA2687: "sos_stop_sample"}


def name_at(a):
    if a in LIBRARY:
        return LIBRARY[a]
    by, ks = names()
    k = bisect.bisect_right(ks, a) - 1
    if k < 0:
        return "%06X" % a
    b = ks[k]
    return by[b] if a == b else "%s+%X" % (by[b], a - b)


def subsystem(name):
    return name.split("_")[1] if name.startswith("xn_") and name.count("_") >= 1 else ""


# ---- static -----------------------------------------------------------------------------------
def static():
    """Entries, exits and static references from the code; build/xn_canon/boundary/static.json."""
    import xn_abi
    from capstone import x86 as cx
    from le import SRC_OFF32, SRC_REL32
    t0 = time.time()
    an = xn_abi.Analysis()
    an.run()
    code = an.code
    abi = xn_abi.read_abi()
    funcs2 = set(code.funcs2)
    # entries: the game's direct calls, and what the game reads after each
    game_calls = collections.defaultdict(list)
    reads_after = collections.defaultdict(int)
    icalls1 = []
    for f in an.game_funcs:
        b = an.bodies[f]
        for a in b.order:
            x = b.insns[a]
            if x.kind != "call":
                continue
            op = x.i.operands[0]
            if op.type != cx.X86_OP_IMM:
                icalls1.append(a)
            for t in x.callees or ():
                if t in funcs2:
                    game_calls[t].append(a)
                    after = an.after.get(a, (0, frozenset()))[0]
                    # Watcom 10.0a code never reads, after a call, the argument registers it
                    # passed nor the flags (tools/xn_abi.py compute_outputs)
                    after &= ~sum(xn_abi.RMASK[r] for r in xn_abi.WATCOM_ARGS[1:min(x.nargs or 0, 4)])
                    after &= ~xn_abi.ARITH
                    reads_after[t] |= after
    # function addresses the game holds (object-1 code or object-3 data)
    held = collections.defaultdict(list)
    data_refs = collections.defaultdict(list)       # object-2 data the game addresses
    for fx in code.le.fixups():
        s, t = fx.src_va, fx.target_va
        if OBJ2[0] <= s < OBJ2[1] or not OBJ2[0] <= t < OBJ2[1]:
            continue
        if fx.kind == SRC_OFF32:
            if t in funcs2:
                held[t].append(s)
            else:
                data_refs[t].append(s)
    # how the game uses each object-2 data address: the instruction holding the fixup
    ins1 = {}
    for f in an.game_funcs:
        for a in an.bodies[f].order:
            ins1[a] = an.bodies[f].insns[a].i
    starts1 = sorted(ins1)
    ref_use = {}
    for t, srcs in data_refs.items():
        uses = collections.Counter()
        for s in srcs:
            use = "data"
            if OBJ1[0] <= s < OBJ1[1]:
                use = "code"
                k = bisect.bisect_right(starts1, s) - 1
                i = ins1.get(starts1[k]) if k >= 0 else None
                if i is not None and i.address < s < i.address + i.size:
                    use = "addr"
                    for op in i.operands:
                        if op.type == cx.X86_OP_MEM and (op.mem.disp & 0xFFFFFFFF) == t:
                            use = "read" if op.access & 1 else "write" if op.access & 2 else "addr"
            uses[use] += 1
        ref_use[t] = dict(uses)
    # exits: what object 2 calls in the game, its services and ports
    exits = collections.defaultdict(list)
    services = collections.defaultdict(set)
    ports = collections.defaultdict(set)
    for va in code.funcs2:
        b = an.bodies[va]
        prev = []
        for a in b.order:
            x = b.insns[a]
            m = x.m
            if x.kind == "call":
                for t in x.callees or ():
                    if OBJ1[0] <= t < OBJ1[1]:
                        exits[t].append(a)
            if x.kind == "int" and x.service:
                vec, ax = x.service
                key = "%02X" % vec + (":%04X" % ax if ax is not None and ax < 0x10000 else
                                      ":%02Xh" % ((ax >> 8) & 0xFF) if ax is not None else ":?")
                services[key].add(va)
            if m.split()[-1] in ("in", "out", "insb", "insw", "insd", "outsb", "outsw", "outsd"):
                port = None
                for op in x.i.operands:
                    if op.type == cx.X86_OP_IMM:
                        port = op.imm & 0xFFFF
                if port is None:            # dx: a constant loaded just before
                    for p in reversed(prev[-8:]):
                        i = p.i
                        if i.mnemonic == "mov" and len(i.operands) == 2 and \
                                i.operands[0].type == cx.X86_OP_REG and \
                                i.reg_name(i.operands[0].reg) in ("dx", "edx") and \
                                i.operands[1].type == cx.X86_OP_IMM:
                            port = i.operands[1].imm & 0xFFFF
                            break
                ports["%04X" % port if port is not None else "dx"].add(va)
            prev.append(x)
    entries = {}
    for t in sorted(set(game_calls) | set(held)):
        row = abi.get(t, {})
        entries["%06X" % t] = {
            "name": name_at(t), "how": " ".join(k for k, v in (("call", game_calls.get(t)),
                                                               ("pointer", held.get(t))) if v),
            "sites": len(game_calls.get(t, ())), "held": len(held.get(t, ())),
            "game_reads": xn_abi.names_of(reads_after.get(t, 0) & xn_abi.GPR),
            "convention": row.get("convention", ""), "abi_inputs": row.get("inputs", ""),
            "abi_outputs": row.get("outputs", ""), "site_list": ["%06X" % s for s in
                                                                 game_calls.get(t, ())[:40]]}
    out = {
        "entries": entries,
        "icalls1": ["%06X" % a for a in sorted(icalls1)],
        "exits": {"%06X" % t: {"name": name_at(t), "sites": len(v),
                               "callers": sorted({name_at(code.prog.func_of(s) or s) for s in v})}
                  for t, v in sorted(exits.items())},
        "services": {k: sorted(name_at(v) for v in vs) for k, vs in sorted(services.items())},
        "ports": {k: sorted(name_at(v) for v in vs) for k, vs in sorted(ports.items())},
        "data_refs": {"%06X" % t: {"refs": len(v), "use": ref_use.get(t, {})}
                      for t, v in sorted(data_refs.items())},
        "seconds": round(time.time() - t0, 1),
    }
    os.makedirs(OUT, exist_ok=True)
    with open(STATIC, "w") as f:
        json.dump(out, f, indent=0)
    print("%d entries (%d with game call sites, %d held as pointers), %d exits, %d services, "
          "%d port forms, %d object-2 data addresses the game references, %d object-1 indirect "
          "call sites -> %s in %.0f s" % (
              len(entries), len(game_calls), len(held), len(out["exits"]), len(services),
              len(ports), len(data_refs), len(icalls1), os.path.relpath(STATIC, ROOT),
              time.time() - t0))
    return out


def load_static():
    if not os.path.exists(STATIC):
        raise SystemExit("run xn_boundary.py static first")
    with open(STATIC) as f:
        return json.load(f)


# ---- allocations from a boot ------------------------------------------------------------------
def alloc_sites():
    """Object-2 call sites of malloc and free (LE fixups of their rel32 calls): {site: kind}."""
    import fallemu
    from le import LE, SRC_REL32
    out = {}
    for fx in LE(fallemu.EXE).fixups():
        if fx.kind == SRC_REL32 and OBJ2[0] <= fx.src_va < OBJ2[1]:
            if fx.target_va == MALLOC:
                out[fx.src_va - 1] = "malloc"
            elif fx.target_va == FREE:
                out[fx.src_va - 1] = "free"
    return out


def allocs(max_lines=None):
    """Boot into the first dungeon with hooks on the engine's malloc and free calls."""
    import fallemu
    import xn_record
    from unicorn import UC_HOOK_CODE
    t0 = time.time()
    sites = alloc_sites()
    ov = os.path.join(ROOT, "build", "emu", "ov_boundary_allocs")
    emu = fallemu.Emu.load(os.path.join(xn_record.BASES, "newgame_0.snap"), overlay=ov) \
        if os.path.exists(os.path.join(xn_record.BASES, "newgame_0.snap")) else \
        fallemu.Emu(overlay=ov)
    log = []
    pending = {}

    def at_site(uc, address, size, _):
        a = address - LOAD
        if sites.get(a) == "malloc":
            pending[a] = uc.reg_read(fallemu.R["eax"])
        else:
            log.append({"op": "free", "site": "%06X" % a, "ptr": uc.reg_read(fallemu.R["eax"]) - LOAD,
                        "tick": emu.ticks})

    def after_site(uc, address, size, _):
        a = address - 5 - LOAD
        if a in pending:
            log.append({"op": "malloc", "site": "%06X" % a, "size": pending.pop(a),
                        "ptr": (uc.reg_read(fallemu.R["eax"]) - LOAD) & 0xFFFFFFFF,
                        "tick": emu.ticks, "func": name_at(a)})
    for a, kind in sites.items():
        emu.uc.hook_add(UC_HOOK_CODE, at_site, None, LOAD + a, LOAD + a)
        if kind == "malloc":
            emu.uc.hook_add(UC_HOOK_CODE, after_site, None, LOAD + a + 5, LOAD + a + 5)
    lines = xn_record.newgame_segments()
    n = 0
    try:
        for _name, seg in lines:
            xn_record.run_lines(emu, seg)
            n += 1
            if max_lines and n >= max_lines:
                break
        emu.run(emu.ticks + 600, [])
        live = {}
        for e in log:
            if e["op"] == "malloc":
                live[e["ptr"]] = e
            else:
                live.pop(e["ptr"], None)
        mem = emu.read(LOAD, fallemu.MEM)
        # the object-2 globals that hold each live block's address (or its 32-aligned form)
        holders = collections.defaultdict(list)
        for g in range(OBJ2[0], OBJ2[1] - 3, 4):
            v = struct.unpack_from("<I", mem, g)[0] - LOAD
            holders[v].append(g)
        for p, e in live.items():
            e["live"] = True
            hs = []
            for delta in range(0, 33):
                for g in holders.get(p + delta, ()):
                    hs.append({"global": "%06X" % g, "name": name_at(g), "offset": delta})
                if hs:
                    break
            e["holders"] = hs
    finally:
        emu.close()
        import shutil
        shutil.rmtree(ov, ignore_errors=True)
    out = {"sites": {"%06X" % a: k for a, k in sorted(sites.items())}, "log": log,
           "ticks": emu.ticks, "seconds": round(time.time() - t0, 1)}
    os.makedirs(OUT, exist_ok=True)
    with open(ALLOCS, "w") as f:
        json.dump(out, f, indent=0)
    nl = sum(1 for e in log if e.get("live"))
    print("%d engine malloc/free calls (%d blocks live at the end) in %.0f s -> %s" % (
        len(log), nl, time.time() - t0, os.path.relpath(ALLOCS, ROOT)))
    for e in log:
        if e.get("live"):
            print("  %s %-34s %8d bytes at %08X held by %s" % (
                e["site"], e["func"], e["size"], e["ptr"],
                ", ".join("%s%s" % (h["name"], "+%d" % h["offset"] if h["offset"] else "")
                          for h in e["holders"]) or "-"))
    return out


# blocks whose layout follows their contents (a cache's): no part of them is private by offset
DYNAMIC_ALLOCS = {0x1343DC}         # xn_tex_heap_base: the texture cache's heap


# The engine's blocks as rules: (name, pointer global, size: a constant or a global, why).
# From `allocs` (which block each object-2 malloc site makes and which global keeps it).
def alloc_rules():
    """[(name, pointer global va, size (int) or ("global", va), evidence)] from allocs.json:
    every live block of the boot that an object-2 global points at."""
    if not os.path.exists(ALLOCS):
        return []
    with open(ALLOCS) as f:
        d = json.load(f)
    out, seen = [], set()
    sizes = collections.defaultdict(set)
    for e in d["log"]:
        if e["op"] == "malloc":
            sizes[e["site"]].add(e["size"])
    for e in d["log"]:
        if not e.get("live") or not e.get("holders"):
            continue
        h = e["holders"][0]
        g = int(h["global"], 16)
        if g in seen:
            continue
        seen.add(g)
        size = e["size"] - h["offset"]
        out.append((h["name"], g, size, "malloc at %s (%s), %d bytes%s" % (
            e["site"], e["func"], e["size"],
            "; sizes seen at that site: %s" % sorted(sizes[e["site"]])
            if len(sizes[e["site"]]) > 1 else "")))
    return out


# ---- the survey -------------------------------------------------------------------------------
class Survey:
    """Memory hooks on an asm machine: per-byte flags (F_E, F_G, F_F, F_U, F_W) over low memory
    and program memory up to `top`; who wrote each byte last (0 the game or the system, 1 the
    engine, 2 unknown). Plus object-1 indirect calls into object 2, and the I/O."""

    def __init__(self, emu, icalls1=()):
        import fallemu
        self.emu = emu
        self.top = min(fallemu.MEM, ((emu.brk + (8 << 20) + 0xFFFFF) >> 20) << 20)
        n = LOW + self.top
        self.M = bytearray(n)
        self.W = bytearray(b"\x02") * n
        o = LOW + OBJ2[0]
        self.W[o:LOW + OBJ2[1]] = b"\x01" * (OBJ2[1] - OBJ2[0])     # the engine's own data
        self.hooks = []
        self.icalls = collections.defaultdict(set)
        self.icall_sites = [LOAD + int(a, 16) for a in icalls1]
        self.io = collections.Counter()
        self.frames = 0

    def idx(self, a):
        if a < LOW:
            return a
        a -= LOAD
        if 0 <= a < self.top:
            return LOW + a
        return None

    def install(self):
        import fallemu
        from unicorn import UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE, UC_HOOK_CODE
        import capstone
        emu, uc = self.emu, self.emu.uc
        EIP = fallemu.R["eip"]
        M, W = self.M, self.W
        g_lo, g_hi = LOAD + OBJ1[0], LOAD + OBJ1[1]
        e_lo, e_hi = LOAD + OBJ2[0], LOAD + OBJ2[1]
        heap = LOAD + HEAP
        low, top = LOW, self.top
        one = [b"", b"\x01", b"\x01\x01", b"\x01\x01\x01", b"\x01" * 4]
        zero = [b"", b"\x00", b"\x00\x00", b"\x00" * 3, b"\x00" * 4]

        def index(a):
            if a < low:
                return a
            a -= LOAD
            return low + a if 0 <= a < top else -1

        nW = len(W)

        def on_write(uc_, acc, addr, size, val, _):
            i = index(addr)
            if i < 0:
                return
            if i + size > nW:
                size = nW - i
            e = uc_.reg_read(EIP)
            if e_lo <= e < e_hi or e >= heap:
                W[i:i + size] = one[size] if size <= 4 else b"\x01" * size
                for j in range(i, i + size):
                    M[j] |= 1
            else:
                W[i:i + size] = zero[size] if size <= 4 else b"\x00" * size
                if g_lo <= e < g_hi:
                    for j in range(i, i + size):
                        M[j] |= 16

        def on_read(uc_, acc, addr, size, val, _):
            e = uc_.reg_read(EIP)
            if not g_lo <= e < g_hi:
                return
            i = index(addr)
            if i < 0:
                return
            if i + size > nW:
                size = nW - i
            for j in range(i, i + size):
                w = W[j]
                M[j] |= 2 | (4 if w == 1 else 8 if w == 2 else 0)
        self.hooks += [uc.hook_add(UC_HOOK_MEM_WRITE, on_write), uc.hook_add(UC_HOOK_MEM_READ, on_read)]
        # object-1 indirect calls: where they go
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        md.detail = True
        from capstone import x86 as cx

        def on_icall(uc_, address, size, _):
            i = next(md.disasm(bytes(uc_.mem_read(address, 16)), address), None)
            if i is None:
                return
            op = i.operands[0]
            try:
                if op.type == cx.X86_OP_REG:
                    t = uc_.reg_read(fallemu.R[i.reg_name(op.reg)])
                else:
                    ea = op.mem.disp
                    if op.mem.base:
                        ea += uc_.reg_read(fallemu.R[i.reg_name(op.mem.base)])
                    if op.mem.index:
                        ea += uc_.reg_read(fallemu.R[i.reg_name(op.mem.index)]) * op.mem.scale
                    t = struct.unpack("<I", bytes(uc_.mem_read(ea & 0xFFFFFFFF, 4)))[0]
            except Exception:       # noqa: BLE001
                return
            t = (t - LOAD) & 0xFFFFFFFF
            if OBJ2[0] <= t < OBJ2[1]:
                self.icalls[address - LOAD].add(t)
        for a in self.icall_sites:
            self.hooks.append(uc.hook_add(UC_HOOK_CODE, on_icall, None, a, a))
        # services write memory from Python (no hook): a DOS read the engine asked for is the
        # engine's write (the game reading that buffer reads what the engine fetched); the
        # rest (the game's own services, interrupt frames) are the system's
        orig = emu.write

        def write(lin, data, _orig=orig):
            _orig(lin, data)
            i = index(lin)
            if i < 0:
                return
            n_ = min(len(data), nW - i)
            e = uc.reg_read(EIP)
            if e_lo <= e < e_hi:
                W[i:i + n_] = b"\x01" * n_
                for j in range(i, i + n_):
                    M[j] |= 1
            else:
                W[i:i + n_] = b"\x00" * n_
        emu.write = write
        fallemu.flush_caches(uc)

    def remove(self):
        import fallemu
        for h in self.hooks:
            self.emu.uc.hook_del(h)
        self.hooks = []
        if "write" in self.emu.__dict__:
            del self.emu.__dict__["write"]
        fallemu.flush_caches(self.emu.uc)

    def result(self, name):
        import fallemu
        emu = self.emu
        return {"name": name, "top": self.top, "M": zlib.compress(bytes(self.M), 6),
                "obj2": zlib.compress(emu.read(LOAD + OBJ2[0], OBJ2[1] - OBJ2[0]), 6),
                "icalls": {"%06X" % k: sorted("%06X" % t for t in v) for k, v in self.icalls.items()},
                "vectors": {"pm": {"%02X" % n: (s, (o - LOAD) & 0xFFFFFFFF) for n, (s, o) in
                                   emu.pm_vec.items() if s != fallemu.SEL_STUB},
                            "exc": {"%02X" % n: (s, (o - LOAD) & 0xFFFFFFFF) for n, (s, o) in
                                    emu.exc.items()}},
                "io": dict(self.io)}


def note_io(survey, log):
    for e in log or ():
        if e[0] in ("in", "out"):
            survey.io["%s %04X" % (e[0], e[1])] += 1
        elif e[0] == "int":
            survey.io["int %02X:%04X" % (e[1], e[2] & 0xFFFF)] += 1
        elif e[0] == "exc":
            survey.io["exc %02X" % e[1]] += 1


def _survey_task(names_, sp):
    """One worker: survey these scenarios (asm only, memory hooks during the lockstep frames)."""
    import xn_scenarios
    st = load_static()
    out = []
    for n in names_:
        spec = xn_scenarios.get(n)
        holder = {}

        def on_asm(emu, phase, data=None, holder=holder):
            if phase == "frames":
                s = Survey(emu, st["icalls1"])
                s.install()
                holder["s"] = s
            elif phase == "frame":
                note_io(holder["s"], data)
                holder["s"].frames += 1
            else:
                s = holder["s"]
                s.remove()
                holder["r"] = s.result(n)
        t0 = time.time()
        try:
            r = xn_scenarios.run(spec, asm_only=True, on_asm=on_asm, verbose=False,
                                 frames=sp.get("frames"))
        except Exception as e:      # noqa: BLE001
            print("%s: %r" % (n, e), flush=True)
            continue
        res = holder.get("r")
        if res is None:
            continue
        res["frames"] = r["total"]
        os.makedirs(SURVEY, exist_ok=True)
        with open(os.path.join(SURVEY, n + ".pkl"), "wb") as f:
            pickle.dump(res, f)
        print("%-14s %3d frames surveyed in %.0f s" % (n, r["total"], time.time() - t0), flush=True)
        out.append(n)
    return out


def survey(names_=None, jobs=None, frames=None):
    import xn_scenarios
    import xn_cload
    names_ = names_ or [s["name"] for s in xn_scenarios.SCENARIOS]
    t0 = time.time()
    res = xn_cload.parallel("xn_boundary:_survey_task", names_, {"frames": frames}, jobs)
    done = [n for r in res for n in r]
    print("%d / %d scenarios surveyed in %.0f s -> %s" % (len(done), len(names_), time.time() - t0,
                                                          os.path.relpath(SURVEY, ROOT)))
    return 0


def _corpus_task(files, sp):
    """The bytes each record writes, by object-2 address and low memory (E evidence)."""
    import xn_record
    M = bytearray(OBJ2[1] - OBJ2[0])
    L = bytearray(LOW)
    for path in files:
        try:
            recs, _store = xn_record.read_raw(path)
        except Exception:       # noqa: BLE001
            continue
        for rec in recs:
            esp = rec["exit"]["esp"]
            for a in rec["writes"]:
                if esp - 0x10000 <= a < esp:
                    continue
                if a < LOW:
                    L[a] = 1
                elif OBJ2[0] <= a - LOAD < OBJ2[1]:
                    M[a - LOAD - OBJ2[0]] = 1
    import base64
    return {"obj2": base64.b64encode(zlib.compress(bytes(M))).decode(),
            "low": base64.b64encode(zlib.compress(bytes(L))).decode()}


def survey_corpus(jobs=None):
    import xn_cload
    import base64
    t0 = time.time()
    files = xn_cload.record_files(None)
    res = xn_cload.parallel("xn_boundary:_corpus_task", files, {}, jobs)
    M = bytearray(OBJ2[1] - OBJ2[0])
    L = bytearray(LOW)
    for r in res:
        m = zlib.decompress(base64.b64decode(r["obj2"]))
        lo = zlib.decompress(base64.b64decode(r["low"]))
        M = bor(M, m)
        L = bor(L, lo)
    os.makedirs(SURVEY, exist_ok=True)
    with open(os.path.join(SURVEY, "_corpus.pkl"), "wb") as f:
        pickle.dump({"name": "_corpus", "obj2": zlib.compress(bytes(M)), "low": zlib.compress(bytes(L))}, f)
    print("corpus: %d object-2 bytes and %d low-memory bytes written by records, in %.0f s" % (
        sum(M), sum(L), time.time() - t0))


# ---- classification ---------------------------------------------------------------------------
LOW_REGIONS = [
    (0x00000, 0x00400, "real-mode interrupt vectors"),
    (0x00400, 0x00500, "BIOS data area"),
    (0x00500, 0x00800, "low DOS memory"),
    (0x00800, 0x01000, "descriptor table (GDT)"),
    (0x01000, 0x01800, "PSP and environment"),
    (0x01800, 0x02000, "the emulator's interrupt stubs"),
    (0x02000, 0x9F000, "DOS memory (DPMI 0100h blocks)"),
    (0x9F000, 0xA0000, "the extender's transfer buffer"),
    (0xA0000, 0xB0000, "VGA memory (the screen)"),
    (0xB0000, LOW, "upper memory and the HMA"),
]


def load_surveys():
    out = []
    for p in sorted(glob.glob(os.path.join(SURVEY, "*.pkl"))):
        with open(p, "rb") as f:
            out.append(pickle.load(f))
    return out


def obj2_items():
    """[(start, end, name, kind)] covering object 2: each name runs to the next; kind "code"
    when most of its bytes are instructions."""
    import xn_record
    by, ks = names()
    mask = xn_record.code_mask()
    starts = sorted({a for a in ks if OBJ2[0] <= a < OBJ2[1]} | {OBJ2[0]})
    out = []
    for k, a in enumerate(starts):
        e = starts[k + 1] if k + 1 < len(starts) else OBJ2[1]
        m = mask[a - OBJ2[0]:e - OBJ2[0]]
        kind = "code" if sum(m) * 2 > len(m) else "data"
        out.append((a, e, by.get(a, "xn_data_%06X" % a), kind))
    return out


_TBL = {f: bytes(1 if b & f else 0 for b in range(256)) for f in (F_E, F_G, F_F, F_U, F_W)}
_ANY = bytes(1 if b else 0 for b in range(256))


def flags_count(buf):
    """{flag: bytes with it} of a flag buffer."""
    c = collections.Counter()
    for f, t in _TBL.items():
        c[f] = bytes(buf).translate(t).count(1)
    return c


def has(buf, f):
    return 1 in bytes(buf).translate(_TBL[f])


def nonzero(buf):
    return bytes(buf).count(0) != len(buf)


def bor(a, b):
    """The byte-wise OR of two equally long buffers."""
    n = len(a)
    return bytearray((int.from_bytes(a, "little") | int.from_bytes(b, "little")).to_bytes(n, "little"))


def runs(bits, base, step):
    """[(start, end)] of the consecutive set entries."""
    out = []
    k = 0
    while k < len(bits):
        if bits[k]:
            j = k
            while j < len(bits) and bits[j]:
                j += 1
            out.append((base + k * step, base + j * step))
            k = j
        else:
            k += 1
    return out


def write():
    """Classify and write config/xngine_boundary.csv and the summary."""
    t0 = time.time()
    st = load_static()
    surveys = [s for s in load_surveys() if s["name"] != "_corpus"]
    corpus = next((s for s in load_surveys() if s["name"] == "_corpus"), None)
    if not surveys:
        raise SystemExit("no surveys (xn_boundary.py survey)")
    rows = []
    # -- entries
    vectors = {}
    for s in surveys:
        for kind in ("pm", "exc"):
            for n, (sel, off) in s["vectors"][kind].items():
                if OBJ2[0] <= off < OBJ2[1]:
                    vectors[(kind, n)] = off
    icalls = collections.defaultdict(set)
    for s in surveys:
        for site, ts in s["icalls"].items():
            for t in ts:
                icalls[int(t, 16)].add(site)
    entry_rows = {}
    for k, e in st["entries"].items():
        va = int(k, 16)
        entry_rows[va] = {"kind": "entry", "space": "obj", "start": "%06X" % va, "end": "",
                          "name": e["name"], "class": e["how"].replace(" ", "+"),
                          "rule": "game_reads=%s" % e["game_reads"].replace(" ", ","),
                          "evidence": "%d game call sites%s; convention %s" % (
                              e["sites"], ", held by %d object-1 pointer(s)" % e["held"]
                              if e["held"] else "", e["convention"])}
    for (kind, n), off in sorted(vectors.items()):
        r = entry_rows.setdefault(off, {"kind": "entry", "space": "obj", "start": "%06X" % off,
                                        "end": "", "name": name_at(off), "class": "",
                                        "rule": "game_reads=all", "evidence": ""})
        r["class"] = "+".join(x for x in (r["class"], "vector") if x)
        r["rule"] = "game_reads=all"
        r["evidence"] = "; ".join(x for x in (r["evidence"], "%s %sh installed (a snapshot's %s "
                                              "table)" % ("exception" if kind == "exc" else "interrupt",
                                                          n, "DPMI exception" if kind == "exc" else
                                                          "protected-mode vector")) if x)
    for t, sites in sorted(icalls.items()):
        r = entry_rows.setdefault(t, {"kind": "entry", "space": "obj", "start": "%06X" % t,
                                      "end": "", "name": name_at(t), "class": "",
                                      "rule": "game_reads=all", "evidence": ""})
        if "icall" not in r["class"]:
            r["class"] = "+".join(x for x in (r["class"], "icall") if x)
            r["evidence"] = "; ".join(x for x in (r["evidence"], "called through a pointer from "
                                                  "object-1 site(s) %s (survey)" %
                                                  " ".join(sorted(sites)[:4])) if x)
            if "call" not in r["class"].split("+"):
                r["rule"] = "game_reads=watcom"
    rows += [entry_rows[k] for k in sorted(entry_rows)]
    # -- exits
    for k, e in st["exits"].items():
        rows.append({"kind": "exit", "space": "obj", "start": k, "end": "", "name": e["name"],
                     "class": "game-function", "rule": "",
                     "evidence": "%d object-2 call sites: %s" % (e["sites"], " ".join(e["callers"][:6]))})
    for k, fs in st["services"].items():
        rows.append({"kind": "exit", "space": "int", "start": k, "end": "", "name": "int " + k,
                     "class": "service", "rule": "", "evidence": "by " + " ".join(fs[:6]) +
                     (" (+%d)" % (len(fs) - 6) if len(fs) > 6 else "")})
    for k, fs in st["ports"].items():
        rows.append({"kind": "exit", "space": "port", "start": k, "end": "", "name": "port " + k,
                     "class": "port", "rule": "", "evidence": "by " + " ".join(fs[:6]) +
                     (" (+%d)" % (len(fs) - 6) if len(fs) > 6 else "")})
    # -- memory: object 2, by item
    n2 = OBJ2[1] - OBJ2[0]
    M2 = bytearray(n2)
    for s in surveys:
        M = zlib.decompress(s["M"])
        seg = M[LOW + OBJ2[0]:LOW + OBJ2[1]]
        M2 = bor(M2, seg)
    C2 = zlib.decompress(corpus["obj2"]) if corpus else bytes(n2)
    refs = {int(k, 16): v for k, v in st["data_refs"].items()}
    ref_at = sorted(refs)
    mem_rows = []
    for a, e, nm, kind in obj2_items():
        seg = M2[a - OBJ2[0]:e - OBJ2[0]]
        cseg = C2[a - OBJ2[0]:e - OBJ2[0]]
        c = flags_count(seg)
        ce = len(cseg) - bytes(cseg).count(0)
        k0 = bisect.bisect_left(ref_at, a)
        sref = []
        while k0 < len(ref_at) and ref_at[k0] < e:
            sref.append(ref_at[k0])
            k0 += 1
        nref = sum(refs[x]["refs"] for x in sref)
        if nref or c[F_F]:
            cls = "visible"
        elif c[F_E] or ce:
            cls = "private"
        elif kind == "code":
            cls = "private"         # code: the game never reads it
        else:
            cls = "unobserved"
        why = []
        if nref:
            uses = collections.Counter()
            for x in sref:
                uses.update(refs[x]["use"])
            why.append("%d object-1 references (%s)" % (nref, ", ".join(
                "%s %d" % kv for kv in sorted(uses.items()))))
        if c[F_F]:
            why.append("the game reads %d bytes the engine wrote" % c[F_F])
        if c[F_G] and not c[F_F]:
            why.append("the game reads %d bytes it wrote itself" % c[F_G])
        if c[F_E] or ce:
            why.append("the engine writes %d bytes (surveys) / %d (records)" % (c[F_E], ce))
        if c[F_W]:
            why.append("the game writes %d bytes" % c[F_W])
        if not why:
            why.append("neither seen" if kind == "data" else "code")
        mem_rows.append({"kind": "memory", "space": "obj", "start": "%06X" % a, "end": "%06X" % e,
                         "name": nm, "class": cls, "rule": kind, "evidence": "; ".join(why)})
    # merge neighbouring private code items into runs (the CSV stays readable)
    merged = []
    for r in mem_rows:
        if merged and r["rule"] == "code" and merged[-1]["rule"] == "code" and \
                r["class"] == merged[-1]["class"] == "private" and \
                merged[-1]["end"] == r["start"] and "references" not in r["evidence"]:
            m = merged[-1]
            m["end"] = r["end"]
            m["evidence"] = _merge_ev(m["evidence"], r["evidence"])
            m["name"] = m["name"].split(" .. ")[0] + " .. " + r["name"]
            continue
        merged.append(dict(r))
    rows += merged
    # -- memory: low memory, by region and page
    ML = bytearray(LOW)
    for s in surveys:
        M = zlib.decompress(s["M"])
        ML = bor(ML, M[:LOW])
    CL = zlib.decompress(corpus["low"]) if corpus else bytes(LOW)
    for lo, hi, what in LOW_REGIONS:
        seg = ML[lo:hi]
        c = flags_count(seg)
        ce = (hi - lo) - bytes(CL[lo:hi]).count(0)
        if lo == 0xA0000:
            cls = "visible"
        elif lo == 0x800:
            cls = "private"         # descriptors: the CPU reads them on selector loads and sets
                                    # their accessed bits; no program code reads them
        elif c[F_F] or c[F_U] and (c[F_E] or ce):
            cls = "visible"
        elif c[F_E] or ce:
            cls = "private"
        else:
            cls = "visible" if c[F_G] else "unobserved"
        ev = "the game reads %d bytes (%d the engine wrote, %d from before); the engine writes "\
             "%d bytes (surveys) / %d (records)" % (c[F_G], c[F_F], c[F_U], c[F_E], ce)
        if lo == 0xA0000:
            ev = "the screen; " + ev
        if lo == 0x800:
            ev += "; the CPU sets descriptors' accessed bits when the code loads selectors"
        rows.append({"kind": "memory", "space": "low", "start": "%06X" % lo, "end": "%06X" % hi,
                     "name": what, "class": cls, "rule": "region", "evidence": ev})
        if cls == "visible" and lo != 0xA0000 and (c[F_E] or ce):
            # pages of it the engine writes and the game never reads
            pages = []
            for p in range(lo, hi, 0x1000):
                sp = ML[p:p + 0x1000]
                ep = has(sp, F_E) or nonzero(CL[p:p + 0x1000])
                gp = has(sp, F_F) or has(sp, F_U) or has(sp, F_G)
                pages.append(ep and not gp)
            for a, e in runs(pages, lo, 0x1000):
                rows.append({"kind": "memory", "space": "low", "start": "%06X" % a,
                             "end": "%06X" % e, "name": what + " (engine pages)",
                             "class": "private", "rule": "pages",
                             "evidence": "the engine writes these pages and the game reads none of them"})
    # -- memory: the game's data and heap
    rows.append({"kind": "memory", "space": "obj", "start": "%06X" % OBJ3[0], "end": "%06X" % OBJ3[1],
                 "name": "the game's data (object 3)", "class": "visible", "rule": "region",
                 "evidence": "game memory: compared"})
    # -- memory: the engine's allocations, by 256-byte chunk of the block. A chunk is private
    # when the engine writes it and the game never reads a byte the engine (or nobody in the
    # window) wrote there; a block whose layout changes with its contents (the texture heap)
    # is classified as a whole
    CH = 0x100
    for name, g, size, ev in alloc_rules():
        nch = (size + CH - 1) // CH
        E = bytearray(nch)
        V = bytearray(nch)
        seen = 0
        for s_ in surveys:
            o2 = zlib.decompress(s_["obj2"])
            ptr = struct.unpack_from("<I", o2, g - OBJ2[0])[0] - LOAD
            if not HEAP <= ptr < s_["top"] - size:
                continue
            seen += 1
            M = zlib.decompress(s_["M"])
            blk = M[LOW + ptr:LOW + ptr + size]
            te = blk.translate(_TBL[F_E])
            tv = bytes(x | y for x, y in zip(blk.translate(_TBL[F_F]), blk.translate(_TBL[F_U])))
            for k in range(nch):
                if not E[k] and 1 in te[k * CH:(k + 1) * CH]:
                    E[k] = 1
                if not V[k] and 1 in tv[k * CH:(k + 1) * CH]:
                    V[k] = 1
        ne, nv = sum(E), sum(V)
        dynamic = g in DYNAMIC_ALLOCS
        if not seen or not ne and not nv:
            cls = "unobserved"          # nothing written in the window: no evidence either way
        elif not nv:
            cls = "private"
        else:
            cls = "visible"
        rows.append({"kind": "memory", "space": "alloc", "start": "0", "end": "%X" % size,
                     "name": name, "class": cls, "rule": "ptr=%06X size=%X" % (g, size),
                     "evidence": "%s; %d surveys saw it: of its %d 256-byte chunks the engine "
                                 "writes %d, the game reads what the engine wrote in %d%s" % (
                                     ev, seen, nch, ne, nv, "; its layout follows its contents "
                                     "(a cache): classified whole" if dynamic else "")})
        if cls == "visible" and not dynamic:
            for a, e in runs([en and not vi for en, vi in zip(E, V)], 0, CH):
                rows.append({"kind": "memory", "space": "alloc", "start": "%X" % a,
                             "end": "%X" % min(e, size), "name": name + " (engine part)",
                             "class": "private", "rule": "ptr=%06X size=%X" % (g, size),
                             "evidence": "the engine writes these chunks and the game never "
                                         "reads what it wrote there (%d surveys)" % seen})
    with open(BOUNDARY_CSV, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=COLUMNS)
        w.writeheader()
        w.writerows(rows)
    summary(rows, st, surveys, corpus)
    cnt = collections.Counter((r["kind"], r["class"]) for r in rows)
    print("%d rows -> %s in %.0f s: %s" % (len(rows), os.path.relpath(BOUNDARY_CSV, ROOT),
                                          time.time() - t0, ", ".join(
                                              "%s %s %d" % (k[0], k[1], n) for k, n in sorted(cnt.items()))))
    return rows


def _merge_ev(a, b):
    """Two evidence strings of merged code items: add the counts."""
    def nums(s):
        m = re.search(r"the engine writes (\d+) bytes \(surveys\) / (\d+)", s)
        return (int(m.group(1)), int(m.group(2))) if m else (0, 0)
    x, y = nums(a), nums(b)
    if x == (0, 0) and y == (0, 0):
        return "code"
    return "code; the engine writes %d bytes (surveys) / %d (records)" % (x[0] + y[0], x[1] + y[1])


def summary(rows, st, surveys, corpus):
    lines = ["# XnGine's boundary (generated by tools/xn_boundary.py write)", ""]
    ent = [r for r in rows if r["kind"] == "entry"]
    lines.append("Entries: %d object-2 addresses the game enters." % len(ent))
    by = collections.Counter()
    for r in ent:
        for k in r["class"].split("+"):
            by[k] += 1
    lines.append("  " + ", ".join("%s %d" % kv for kv in sorted(by.items())))
    ex = [r for r in rows if r["kind"] == "exit"]
    lines.append("Exits: %d game functions, %d services, %d port forms." % (
        sum(1 for r in ex if r["class"] == "game-function"), sum(1 for r in ex if r["class"] == "service"),
        sum(1 for r in ex if r["class"] == "port")))
    mem = [r for r in rows if r["kind"] == "memory"]
    cnt = collections.Counter()
    for r in mem:
        try:
            n = int(r["end"], 16) - int(r["start"], 16)
        except ValueError:
            n = 0
        cnt[(r["space"], r["class"])] += n
    lines.append("Memory (bytes): " + ", ".join("%s %s %d" % (k[0], k[1], v) for k, v in sorted(cnt.items())))
    lines.append("Surveys: %d scenarios, %d frames%s." % (
        len(surveys), sum(s.get("frames", 0) for s in surveys), "; the record corpus' writes"
        if corpus else ""))
    os.makedirs(OUT, exist_ok=True)
    with open(SUMMARY, "w") as f:
        f.write("\n".join(lines) + "\n")
    print("\n".join(lines))


# ---- reading the boundary (tests) ----------------------------------------------------------------
class Private:
    """Resolved engine-private ranges (linear addresses)."""

    def __init__(self, ranges):
        rs = sorted(ranges)
        merged = []
        for lo, hi in rs:
            if merged and lo <= merged[-1][1]:
                merged[-1][1] = max(merged[-1][1], hi)
            else:
                merged.append([lo, hi])
        self.ranges = [tuple(r) for r in merged]
        self.starts = [r[0] for r in self.ranges]

    def contains(self, a):
        k = bisect.bisect_right(self.starts, a) - 1
        return k >= 0 and a < self.ranges[k][1]

    def filter(self, addrs):
        return [a for a in addrs if not self.contains(a)]


class Masks:
    """The private rows of config/xngine_boundary.csv (and extra ranges: dropped memory)."""

    def __init__(self, rows):
        self.static = []
        self.allocs = []
        for r in rows:
            if r["kind"] != "memory" or r["class"] != "private":
                continue
            if r["space"] == "obj":
                self.static.append((LOAD + int(r["start"], 16), LOAD + int(r["end"], 16)))
            elif r["space"] == "low":
                self.static.append((int(r["start"], 16), int(r["end"], 16)))
            elif r["space"] == "alloc":
                m = re.match(r"ptr=([0-9A-Fa-f]+) size=([0-9A-Fa-f]+)", r["rule"])
                if m:
                    self.allocs.append((int(m.group(1), 16), int(r["start"], 16),
                                        int(r["end"], 16)))
        self._cache = None

    def resolve(self, emu=None, read=None):
        """The private ranges in a machine (emu, or read(addr, n) -> bytes)."""
        rd = read or (lambda a, n: emu.read(a, n))
        out = list(self.static)
        for g, lo, hi in self.allocs:
            try:
                p = struct.unpack("<I", rd(LOAD + g, 4))[0]
            except Exception:       # noqa: BLE001
                continue
            if LOAD + HEAP <= p < LOAD + (64 << 20):
                out.append((p + lo, p + hi))
        return Private(out)


_rows = None


def rows_csv(path=BOUNDARY_CSV):
    global _rows
    if _rows is None or _rows[0] != path:
        if not os.path.exists(path):
            _rows = (path, [])
        else:
            with open(path, newline="") as f:
                _rows = (path, list(csv.DictReader(f)))
    return _rows[1]


def load_masks(path=BOUNDARY_CSV):
    """The masks a test applies, or None when there is no boundary map yet."""
    rows = rows_csv(path)
    if not rows:
        return None
    return Masks(rows)


def entries(path=BOUNDARY_CSV):
    """{va: {"name", "how" (set), "game_reads" (register names or "all"/"watcom")}}."""
    out = {}
    for r in rows_csv(path):
        if r["kind"] != "entry":
            continue
        gr = r["rule"].split("=", 1)[1] if "=" in r["rule"] else "watcom"
        out[int(r["start"], 16)] = {"name": r["name"], "how": set(r["class"].split("+")),
                                    "game_reads": gr.replace(",", " ")}
    return out


def show(what):
    rows = rows_csv()
    try:
        a = int(what, 16)
    except ValueError:
        a = None
    for r in rows:
        if r["name"] == what or what in r["name"].split(" .. "):
            print(r)
            continue
        if a is not None and r["space"] in ("obj", "low") and r["end"]:
            try:
                if int(r["start"], 16) <= a < int(r["end"], 16):
                    print(r)
            except ValueError:
                pass
        elif a is not None and r["start"] == "%06X" % a:
            print(r)


def masks_cmd(snap):
    import fallemu
    path = snap if os.path.exists(snap) else os.path.join(fallemu.SNAPS, snap + ".snap")
    mem = fallemu.snapshot_memory(path)
    m = load_masks()

    def read(a, n):
        return mem[a - LOAD:a - LOAD + n]
    p = m.resolve(read=read)
    tot = 0
    for lo, hi in p.ranges:
        tot += hi - lo
        print("%08X-%08X %8d  %s" % (lo, hi, hi - lo, name_at(lo - LOAD) if lo >= LOAD else "low"))
    print("%d private ranges, %d bytes" % (len(p.ranges), tot))


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("static")
    al = sub.add_parser("allocs")
    al.add_argument("--segments", type=int, default=None)
    sv = sub.add_parser("survey")
    sv.add_argument("names", nargs="*")
    sv.add_argument("-j", "--jobs", type=int, default=None)
    sv.add_argument("--corpus", action="store_true")
    sv.add_argument("--frames", type=int, default=None)
    sub.add_parser("write")
    mk = sub.add_parser("masks")
    mk.add_argument("snap")
    sh = sub.add_parser("show")
    sh.add_argument("what")
    a = ap.parse_args()
    if a.cmd == "static":
        static()
    elif a.cmd == "allocs":
        allocs(a.segments)
    elif a.cmd == "survey":
        if a.corpus:
            survey_corpus(a.jobs)
        if a.names or not a.corpus:
            survey(a.names, a.jobs, a.frames)
    elif a.cmd == "write":
        write()
    elif a.cmd == "masks":
        masks_cmd(a.snap)
    elif a.cmd == "show":
        show(a.what)
    return 0


if __name__ == "__main__":
    sys.exit(main())
