#!/usr/bin/env python3
"""Equivalence of XnGine's pure helpers, asm against canonical C, at scale.

Each helper is run on its asm entry and through its test shim (the canonical C, the routes of
tools/xn_rc.py's image) with the same made-up registers (and memory, for those that take
pointers), millions of samples a function, and their outputs compared: the registers its ABI
row outputs (config/xngine_abi.csv), CF when it outputs CF, and the memory it writes through
its pointers. Where the input domain is small (an angle, a 16- or 24-bit value) it is covered
exhaustively; the rest is random over mixed magnitudes, with the edges (0, +-1, the extremes)
always in.

The samples run in batches inside one machine (a save's snapshot: the engine's tables are
loaded), through a harness of a few instructions at HBASE: for each input record it loads
EAX EBX ECX EDX ESI EDI EBP, calls the target, and stores the registers and EFLAGS. Nothing
else runs (no timer, no routes on the asm), so a batch of 65,536 samples costs one
emu_start. A divide that faults goes to XnGine's own divide-error handler, as in the game.

usage: xn_equiv.py [FUNC ...] [--scale K] [--list] [-j N]
       results -> build/xn_canon/equiv/results.json (and printed)
"""
import argparse
import array
import json
import os
import random
import struct
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.environ.get("XN_EQUIV_OUT") or os.path.join(ROOT, "build", "xn_canon", "equiv")
LOAD = 0x01000000
HBASE = 0x12000000
HSIZE = 0x01000000
VARS = HBASE + 0x100            # cur_in, cur_out, count, target
STACK = HBASE + 0x10000
INP = HBASE + 0x10000
OUTP = HBASE + 0x200000
SCR = HBASE + 0x400000          # per-sample memory (64 bytes each)
BATCH = 1 << 16
REGS = ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp")
SNAP = "save_blades"


def harness():
    """The batch loop's machine code at HBASE (see the module doc); its last byte is a hlt."""
    v = lambda k: struct.pack("<I", VARS + 4 * k)          # noqa: E731
    cur_in, cur_out, count, target = v(0), v(1), v(2), v(3)
    b = bytearray()
    b += b"\x8B\x2D" + cur_in                                # mov ebp, [cur_in]
    for op, d in ((0x45, 0), (0x5D, 4), (0x4D, 8), (0x55, 12), (0x75, 16), (0x7D, 20),
                  (0x6D, 24)):
        b += bytes((0x8B, op, d))                            # mov reg, [ebp+d]
    b += b"\xFF\x15" + target                                # call [target]
    b += b"\x55"                                             # push ebp
    b += b"\x8B\x2D" + cur_out                               # mov ebp, [cur_out]
    for op, d in ((0x45, 0), (0x5D, 4), (0x4D, 8), (0x55, 12), (0x75, 16), (0x7D, 20)):
        b += bytes((0x89, op, d))                            # mov [ebp+d], reg
    b += b"\x8F\x45\x18"                                     # pop dword [ebp+24]
    b += b"\x9C\x8F\x45\x1C"                                 # pushfd; pop dword [ebp+28]
    b += b"\x83\x05" + cur_out + b"\x20"                     # add dword [cur_out], 32
    b += b"\x83\x05" + cur_in + b"\x1C"                      # add dword [cur_in], 28
    b += b"\xFF\x0D" + count                                 # dec dword [count]
    rel = 0 - (len(b) + 6)
    b += b"\x0F\x85" + struct.pack("<i", rel)                # jnz start
    b += b"\xF4"                                             # hlt
    return bytes(b)


class Bench:
    """A machine with the harness and the canonical C image loaded."""

    def __init__(self, snap=SNAP):
        import fallemu
        import xn_rc
        from unicorn import UC_HOOK_CODE
        self.fallemu = fallemu
        path = os.path.join(fallemu.SNAPS, snap + ".snap")
        self.overlay = os.path.join(ROOT, "build", "emu", "ov_equiv_%d" % os.getpid())
        self.emu = fallemu.Emu.load(path, overlay=self.overlay)
        self.img = xn_rc.RImage()
        self.img.install(self.emu)
        self.emu.uc.mem_map(HBASE, HSIZE)
        code = harness()
        self.emu.uc.mem_write(HBASE, code)
        self.stop_at = HBASE + len(code) - 1
        self.done = []

        def stop(uc, address, size, _):
            self.done.append(1)
            uc.emu_stop()
        self.emu.uc.hook_add(UC_HOOK_CODE, stop, None, self.stop_at, self.stop_at)
        self.emu.w("eflags", 0x202 & ~0x200)    # no interrupts: nothing but the samples runs

    def close(self):
        import shutil
        self.emu.close()
        shutil.rmtree(self.overlay, ignore_errors=True)

    def run(self, target, inputs, mem=None):
        """inputs: an array('I') of 7 registers per sample; mem: bytes to write at SCR first.
        Returns (outputs array('I'), 8 per sample, and the scratch memory after)."""
        emu = self.emu
        n = len(inputs) // 7
        emu.uc.mem_write(INP, inputs.tobytes())
        if mem is not None:
            emu.uc.mem_write(SCR, mem)
        emu.uc.mem_write(VARS, struct.pack("<4I", INP, OUTP, n, target))
        emu.w("esp", STACK - 0x100)
        emu.w("eip", HBASE)
        self.done = []
        steps = 0
        while not self.done:
            emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=50_000_000)
            emu.clear_exception_state()
            steps += 1
            if steps > 4000:
                raise RuntimeError("the batch did not finish")
        out = array.array("I")
        out.frombytes(bytes(emu.uc.mem_read(OUTP, 32 * n)))
        after = bytes(emu.uc.mem_read(SCR, len(mem))) if mem is not None else b""
        return out, after


# ---- input generators ----------------------------------------------------------------------------
EDGES = [0, 1, 2, 3, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF, 0xFFFFFFFE, 0x10000000, 0xF0000000,
         0x10000, 0xFFFF0000, 0x8000, 0xFFFF8000, 0x7FF, 0x800]


def mixed(rng):
    """A 32-bit value of a random magnitude (signed), or an edge."""
    r = rng.random()
    if r < 0.05:
        return rng.choice(EDGES)
    bits = rng.randrange(1, 33)
    v = rng.getrandbits(bits)
    if rng.random() < 0.5:
        v = -v
    return v & 0xFFFFFFFF


def unit28(rng):
    """A 2.28 value mostly within +-1.1."""
    r = rng.random()
    if r < 0.05:
        return rng.choice([0, 0x10000000, 0xF0000000, 0x10000001, 0xEFFFFFFF, 0x7FFFFFFF,
                           0x80000000]) & 0xFFFFFFFF
    return int(rng.uniform(-1.1, 1.1) * (1 << 28)) & 0xFFFFFFFF


def world(rng):
    """A world coordinate: small, a town's, a province's, or anything."""
    r = rng.random()
    if r < 0.3:
        return rng.randrange(-4096, 4096) & 0xFFFFFFFF
    if r < 0.6:
        return rng.randrange(-200000, 200000) & 0xFFFFFFFF
    if r < 0.9:
        return rng.randrange(-40000000, 40000000) & 0xFFFFFFFF
    return mixed(rng)


def vec_vals(rng, kind):
    return [(world(rng) if kind == "world" else unit28(rng) if kind == "unit28" else mixed(rng))
            for _ in range(3)]


# Specs: inputs (registers -> generator name or an exhaustive range), memory inputs, outputs.
# "x" generators: fn(rng, i) -> {reg: value}; exhaustive specs give a count and fn(i).
def S(regs=None, n=1_000_000, exhaustive=None, mem=None, outs=None, cf=False, note="",
      poke=None):
    """poke: [(preferred address, bytes)] written into the machine for this spec's batches (and
    put back after): a state the helper reads (the clip window)."""
    return {"regs": regs, "n": n, "exhaustive": exhaustive, "mem": mem, "outs": outs, "cf": cf,
            "note": note, "poke": poke}


def r_regs(**gens):
    def f(rng, i):
        return {k: g(rng) for k, g in gens.items()}
    return f


def ex_grid(bits_a, bits_b, ra="eax", rb="edx", signed=True):
    """Exhaustive over a 2^bits_a x 2^bits_b grid of signed values (centred on 0)."""
    na, nb = 1 << bits_a, 1 << bits_b

    def f(i):
        a = (i % na) - (na // 2 if signed else 0)
        b = (i // na) - (nb // 2 if signed else 0)
        return {ra: a & 0xFFFFFFFF, rb: b & 0xFFFFFFFF}
    return na * nb, f


def matrix_mem(rng):
    """A 3x3 2.28 matrix: a rotation (from angles) or random entries."""
    import math
    if rng.random() < 0.7:
        p, y, r = (rng.random() * 2 * math.pi for _ in range(3))
        sp, cp, sy, cy, sr, cr = math.sin(p), math.cos(p), math.sin(y), math.cos(y), math.sin(r), math.cos(r)
        m = [cy * cr + sp * sy * sr, -cp * sr, 0, 0, cp * cr, 0, sy * cp, sp, cp * cy]
        return [int(v * (1 << 28)) & 0xFFFFFFFF for v in m]
    return [unit28(rng) for _ in range(9)]


def specs():
    out = {}
    out["xn_math_fixmul28"] = S(r_regs(eax=mixed, edx=mixed), 2_000_000)
    out["xn_math_fixmul28_v2"] = S(r_regs(eax=mixed, edx=mixed), 1_000_000)
    out["xn_math_mul_sin"] = S(r_regs(eax=mixed, edx=lambda rng: rng.randrange(-2048, 4096) & 0xFFFFFFFF),
                               exhaustive=(2048 * 1024, lambda i: {
        "edx": i % 2048, "eax": [0, 1, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF][i // 2048 % 5]
        if i // 2048 < 5 else ((i // 2048) * 0x9E3779B1) & 0xFFFFFFFF}), n=500_000,
        note="every angle 0..2047 with 1,024 factors; then angles -2048..4095 (unmasked: Q-MATH-05)")
    out["xn_math_approx_dist2d"] = S(r_regs(eax=world, edx=world, ebx=world, ecx=world), 2_000_000)
    out["xn_math_approx_hypot"] = S(None, exhaustive=ex_grid(12, 12), note="a 4096 x 4096 grid "
                                    "around 0, then random", n=1_000_000)
    out["xn_math_isqrt"] = S(None, exhaustive=(1 << 24, lambda i: {"eax": i, "ecx": (i * 0x2545F491) & 0xFFFFFFFF}),
                             n=2_000_000, note="every v < 2^24 (and v = 0 with ECX varied), then random v")
    out["xn_math_isqrt@0"] = S(None, exhaustive=(1 << 21, lambda i: {
        "eax": 0, "ecx": (i - (1 << 20)) & 0xFFFFFFFF}), n=1_000_000,
        note="v = 0 with every ECX in +-2^20, then random ECX (Q-MATH-02)")
    out["xn_math_isqrt64"] = S(r_regs(eax=mixed, edx=lambda rng: rng.choice(
        [0, 0, rng.getrandbits(8), rng.getrandbits(20), rng.getrandbits(30)])), 1_000_000)
    out["xn_math_isqrt_lookup"] = S(None, exhaustive=(1 << 24, lambda i: {"eax": i}), n=2_000_000,
                                    note="every v < 2^24, then random")
    out["xn_math_asin"] = S(r_regs(eax=unit28), 2_000_000)
    out["xn_math_acos"] = S(r_regs(eax=unit28), 2_000_000)
    out["xn_math_asin_coarse"] = S(None, exhaustive=(1 << 14, lambda i: {
        "eax": ((i - (1 << 13)) << 19) & 0xFFFFFFFF}), n=500_000,
        note="every 2^19 step of the domain, then random")
    out["xn_math_acos_coarse"] = S(None, exhaustive=(1 << 14, lambda i: {
        "eax": ((i - (1 << 13)) << 19) & 0xFFFFFFFF}), n=500_000)
    out["xn_math_angle_to_point"] = S(r_regs(eax=world, edx=world, ebx=world, ecx=world), 2_000_000)
    out["xn_math_angle_xy"] = S(r_regs(eax=world, edx=world), 1_000_000)
    out["xn_math_rotate_xz"] = S(r_regs(eax=world, edx=world, ebx=mixed), 1_000_000)
    out["xn_math_angles_to_vector"] = S(r_regs(eax=mixed, edx=mixed, ebx=world), 1_000_000)
    out["xn_vec_length"] = S(r_regs(eax=world, edx=world, ebx=world), 2_000_000)
    out["xn_vec_length_approx"] = S(r_regs(eax=mixed, edx=mixed, ebx=mixed), 2_000_000)
    out["xn_vec_normalize"] = S(r_regs(eax=world, edx=world, ebx=world), 1_000_000)
    out["xn_vec_normalize_q28"] = S(r_regs(eax=world, edx=world, ebx=world), 500_000)
    out["xn_vec_normalize_shift"] = S(r_regs(eax=world, edx=world, ebx=world,
                                             ecx=lambda rng: rng.randrange(0, 32)), 500_000)
    out["xn_vec_unit_to_angles_regs"] = S(r_regs(eax=unit28, edx=unit28, ebx=unit28), 1_000_000)
    out["xn_vec_dir_to_angles_regs"] = S(r_regs(eax=world, edx=world, ebx=world), 1_000_000)
    out["xn_vec_scale_unit14"] = S(r_regs(eax=mixed, edx=mixed, ebx=mixed, ecx=lambda rng: rng.choice(
        [rng.randrange(0, 0x10000), rng.randrange(0, 0x20000), mixed(rng)])), 1_000_000, cf=True)
    # pointer arguments: per-sample memory at SCR + 64 * k
    out["xn_vec_cross"] = S(None, n=500_000, mem=("vec2", "world"), outs=None)
    out["xn_mat_transform"] = S(None, n=500_000, mem=("mat_vec", "world"))
    out["xn_mat_transform_transposed"] = S(None, n=500_000, mem=("mat_vec", "world"))
    out["xn_mat_transform_wide"] = S(None, n=500_000, mem=("mat_vec", "world"))
    out["xn_math_triangle_y_at"] = S(None, n=500_000, mem=("tri", "world"))
    out["xn_math_yaw_offset_xz"] = S(None, n=500_000, mem=("yaw_out", "world"))
    out["xn_mat_from_angles"] = S(None, exhaustive=None, n=500_000, mem=("angles_mat", None))
    out["xn_math_advance_pitch_yaw"] = S(None, n=500_000, mem=("pitch_yaw_vec", "world"))
    out.update(specs_screen())
    return out


# ---- group B (screen and 2D) -----------------------------------------------------------------
CLIP = 0x142940                 # xn_gfx_clip_left, _top, _right, _bottom


def clip_poke(l, t, r, b):
    return [(CLIP, struct.pack("<4i", l, t, r, b))]


def screen_coord(rng):
    """a coordinate around a 320 x 200 screen, now and then far off or anything"""
    r = rng.random()
    if r < 0.8:
        return rng.randrange(-400, 720) & 0xFFFFFFFF
    if r < 0.95:
        return rng.randrange(-100000, 100000) & 0xFFFFFFFF
    return mixed(rng)


def low8(gen_hi=None):
    """a register whose low byte is i's and whose other bytes are noise"""
    def f(i, b):
        return ((i * 0x9E3779B1) & 0xFFFFFF00) | (b & 0xFF)
    return f


def specs_screen():
    out = {}
    noise = low8()
    out["xn_pal_rgb_to_hsv"] = S(None, exhaustive=(1 << 24, lambda i: {
        "ebx": noise(i, i), "ecx": noise(i + 1, i >> 8), "edx": noise(i + 2, i >> 16)}), n=0,
        note="every colour of 8-bit components (BL CL DL), the other bytes noise")
    out["xn_pal_hsv_to_rgb"] = S(None, exhaustive=(256 * 63 * 64, lambda i: {
        "ebx": ((i * 0x9E3779B1) & 0xFFFF0000) | (0x140 + i % 256),
        "ecx": ((i * 0x85EBCA6B) & 0xFFFF0000) | (1 + (i // 256) % 63),
        "edx": ((i * 0xC2B2AE35) & 0xFFFF0000) | (i // (256 * 63))}), n=0,
        note="hue 140h-23Fh (sectors 5 and up: for the others and a grey the asm returns astray, "
             "Q-PAL-05), every saturation 1-63 and value 0-63")
    out["xn_pal_find_nearest"] = S(lambda rng, i: {
        "ebx": rng.getrandbits(32), "ecx": rng.getrandbits(32), "edx": rng.getrandbits(32)},
        n=200_000, note="random colours against the save's palette (BL CL DL)")
    out["xn_font_glyph_width"] = S(None, exhaustive=(4096, lambda i: {"eax": (i - 2048) & 0xFFFFFFFF}),
                                   n=0, note="characters -2048..2047 in the save's font")
    lines = lambda rng, i: {"eax": screen_coord(rng), "edx": screen_coord(rng),   # noqa: E731
                            "ebx": screen_coord(rng), "ecx": screen_coord(rng)}
    for name, win in (("", (0, 0, 320, 200)), ("@window", (40, 30, 280, 170))):
        out["xn_draw_line_clip" + name] = S(lines, n=1_000_000, poke=clip_poke(*win), cf=True,
                                            note="lines around the clip window %r" % (win,))
        out["xn_draw_clip_rect_xyxy" + name] = S(lambda rng, i: dict(zip(
            ("eax", "ebx"), sorted([screen_coord(rng), screen_coord(rng)], key=lambda v: v - (1 << 32) if v >> 31 else v))) | dict(zip(
            ("edx", "ecx"), sorted([screen_coord(rng), screen_coord(rng)], key=lambda v: v - (1 << 32) if v >> 31 else v))),
            n=1_000_000, poke=clip_poke(*win), cf=True)
        for f in ("xn_draw_clip_rect", "xn_draw_clip_image_rect"):
            out[f + name] = S(lambda rng, i: {
                "eax": screen_coord(rng), "edx": screen_coord(rng),
                "ebx": rng.choice([rng.randrange(-20, 400), screen_coord(rng)]) & 0xFFFFFFFF,
                "ecx": rng.choice([rng.randrange(-20, 260), screen_coord(rng)]) & 0xFFFFFFFF,
                "ebp": rng.randrange(-64, 400) & 0xFFFFFFFF, "esi": rng.getrandbits(32)},
                n=1_000_000, poke=clip_poke(*win), cf=True)
    return out


def gen_mem(kind, rng, k, base):
    """(registers, 64 bytes of memory) for sample k of a pointer spec."""
    a = SCR + 64 * k
    if kind[0] == "vec2":
        va, vb = vec_vals(rng, kind[1]), vec_vals(rng, kind[1])
        return {"eax": a, "edx": a + 12}, struct.pack("<6I", *(va + vb)) + bytes(40)
    if kind[0] == "mat_vec":
        m = matrix_mem(rng)
        v = vec_vals(rng, kind[1])
        return {"eax": v[0], "edx": v[1], "ebx": v[2], "ecx": a}, struct.pack("<9I", *m) + bytes(28)
    if kind[0] == "tri":
        t = []
        c = vec_vals(rng, "world")
        for _ in range(3):
            t += [(c[j] + rng.randrange(-5000, 5000)) & 0xFFFFFFFF for j in range(3)]
        x = (c[0] + rng.randrange(-5000, 5000)) & 0xFFFFFFFF
        z = (c[2] + rng.randrange(-5000, 5000)) & 0xFFFFFFFF
        return {"eax": a, "edx": x, "ebx": z}, struct.pack("<9I", *t) + bytes(28)
    if kind[0] == "yaw_out":
        return {"eax": mixed(rng), "edx": world(rng), "ebx": a, "ecx": a + 4}, bytes(64)
    if kind[0] == "angles_mat":
        return {"eax": mixed(rng), "edx": mixed(rng), "ebx": mixed(rng), "ecx": a}, bytes(64)
    if kind[0] == "pitch_yaw_vec":
        v = vec_vals(rng, "world")
        return {"eax": mixed(rng), "edx": mixed(rng), "ebx": world(rng), "ecx": a}, \
            struct.pack("<3I", *v) + bytes(52)
    raise SystemExit("unknown memory spec %r" % (kind,))


def compare_func(name, spec, bench, scale=1.0, seed=1, verbose=False):
    import xn_abi
    import xn_rc
    abi = xn_abi.read_abi()
    fname = name.split("@")[0]
    va = xn_rc.func_va(fname)
    row = abi[va]
    route = bench.img.routes.get(va)
    if route is None or route["kind"] != "shim":
        return {"name": name, "error": "not canonical (no test shim)"}
    outs = [r for r in ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp")
            if row["out"] & xn_abi.RMASK[r]]
    masks = {r: xn_abi.value_mask(r, row["out"]) for r in outs}
    cf = spec["cf"] or bool(row["fout"] & xn_abi.BIT["CF"])
    rng = random.Random(seed)
    total = 0
    bad = 0
    first = []
    t0 = time.time()
    saved = []
    for a, data in spec.get("poke") or ():
        saved.append((a, bytes(bench.emu.uc.mem_read(LOAD + a, len(data)))))
        bench.emu.uc.mem_write(LOAD + a, data)
    try:
        return _compare(name, spec, bench, va, route, outs, masks, cf, rng, scale, verbose, t0)
    finally:
        for a, data in saved:
            bench.emu.uc.mem_write(LOAD + a, data)


def _compare(name, spec, bench, va, route, outs, masks, cf, rng, scale, verbose, t0):
    total = 0
    bad = 0
    first = []
    ex = spec["exhaustive"]
    nrand = int(spec["n"] * scale)
    nex = ex[0] if ex else 0
    todo = nex + nrand
    k = 0
    gen = spec["regs"] or (lambda rng, i: {r: mixed(rng) for r in ("eax", "edx", "ebx", "ecx")})
    while k < todo:
        n = min(BATCH, todo - k)
        inp = array.array("I", bytes(28 * n))
        mem = bytearray(64 * n) if spec["mem"] else None
        for j in range(n):
            i = k + j
            if i < nex:
                regs = ex[1](i)
            elif spec["mem"]:
                regs, m = gen_mem(spec["mem"], rng, j, SCR)
                mem[64 * j:64 * j + 64] = m
            else:
                regs = gen(rng, i)
            # registers that are not inputs: noise (a function must not depend on them)
            for c, r in enumerate(REGS):
                v = regs.get(r)
                if v is None:
                    v = rng.getrandbits(32) if r not in ("esp",) else 0
                    if r == "ebp":
                        v = rng.getrandbits(32)
                inp[7 * j + c] = v & 0xFFFFFFFF
        a_out, a_mem = bench.run(LOAD + va, inp, bytes(mem) if mem is not None else None)
        c_out, c_mem = bench.run(route["to"], inp, bytes(mem) if mem is not None else None)
        idx = {r: REGS.index(r) for r in outs}
        for j in range(n):
            o = 8 * j
            diff = []
            for r in outs:
                c_ = idx[r]
                if (a_out[o + c_] ^ c_out[o + c_]) & masks[r]:
                    diff.append("%s %08X != %08X" % (r, c_out[o + c_], a_out[o + c_]))
            if cf and (a_out[o + 7] ^ c_out[o + 7]) & 1:
                diff.append("CF")
            if mem is not None and a_mem[64 * j:64 * j + 64] != c_mem[64 * j:64 * j + 64]:
                diff.append("memory")
            if diff:
                bad += 1
                if len(first) < 5:
                    first.append({"in": {r: "%08X" % inp[7 * j + c] for c, r in enumerate(REGS)},
                                  "diff": diff})
        total += n
        k += n
        if verbose:
            print("  %s: %d / %d (%d differ)" % (name, k, todo, bad), flush=True)
    return {"name": name, "samples": total, "exhaustive": nex, "random": nrand, "differ": bad,
            "outputs": outs + (["CF"] if cf else []) + (["memory"] if spec["mem"] else []),
            "first": first, "seconds": round(time.time() - t0, 1), "note": spec.get("note", "")}


def _task(names, sp):
    bench = Bench()
    sps = specs()
    out = []
    try:
        for n in names:
            r = compare_func(n, sps[n], bench, sp.get("scale", 1.0), verbose=sp.get("verbose"))
            print("%-30s %9d samples (%d exhaustive) %s  %.0f s" % (
                n, r.get("samples", 0), r.get("exhaustive", 0),
                r.get("error") or ("all agree" if not r["differ"] else "%d DIFFER" % r["differ"]),
                r.get("seconds", 0)), flush=True)
            out.append(r)
    finally:
        bench.close()
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("funcs", nargs="*")
    ap.add_argument("--scale", type=float, default=1.0, help="random samples times this")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("-j", "--jobs", type=int, default=None)
    ap.add_argument("-v", action="store_true")
    a = ap.parse_args()
    sps = specs()
    if a.list:
        for n, s in sps.items():
            print("%-30s %s" % (n, s.get("note", "")))
        return 0
    names = a.funcs or list(sps)
    for n in names:
        if n not in sps:
            raise SystemExit("no spec for %s (--list)" % n)
    import xn_cload
    t0 = time.time()
    res = xn_cload.parallel("xn_equiv:_task", names, {"scale": a.scale, "verbose": a.v}, a.jobs)
    out = [r for rs in res for r in rs]
    os.makedirs(OUT, exist_ok=True)
    old = {}
    p = os.path.join(OUT, "results.json")
    if os.path.exists(p):
        old = {r["name"]: r for r in json.load(open(p))}
    for r in out:
        old[r["name"]] = r
    with open(p, "w") as f:
        json.dump(list(old.values()), f, indent=1)
    tot = sum(r.get("samples", 0) for r in out)
    nb = sum(1 for r in out if r.get("differ"))
    print("%d functions, %d samples, %d with differences, in %.0f s -> %s" % (
        len(out), tot, nb, time.time() - t0, os.path.relpath(p, ROOT)))
    for r in out:
        if r.get("differ"):
            print("  %s: %d differ; first %s" % (r["name"], r["differ"], r["first"][:2]))
    return 1 if nb else 0


if __name__ == "__main__":
    sys.exit(main())
