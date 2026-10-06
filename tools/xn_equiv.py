#!/usr/bin/env python3
"""Equivalence of XnGine's pure helpers, asm against canonical C, at scale.

Each helper is run on its asm entry and through its test shim (the canonical C, the routes of
tools/xn_rc.py's image) with the same made-up registers (and memory, for those that take
pointers), millions of samples a function, and their outputs compared: the registers its ABI
row outputs (config/xngine_abi.csv), CF when it outputs CF, and the memory it writes through
its pointers. As in the record tests, the register and flag outputs config/xngine_dropped.csv
(and XN_DROPPED) excuses for the function are not compared: a leftover the canonical C no
longer computes. Where the input domain is small (an angle, a 16- or 24-bit value) it is covered
exhaustively; the rest is random over mixed magnitudes, with the edges (0, +-1, the extremes)
always in.

The specs, by group: the pilot's (vec, mat, math; their corners too: a matrix with no
angles, a direction between the same points, an advance too far), group A's (rand, mem, str, bits, spell),
group B's (pal, font, the clippers of draw), group C's (the S-buffer's span insert, the
lights' falloff and cosine terms, the distance fog, the frame's pair sort), group D's (the
projection, the outcodes and the clip intersections, the view matrix's scaling, the sphere
cull and its side-plane distances, the flats' corner offsets), group E's (collide's
primitives, world's cells).

The samples run in batches inside one machine (a save's snapshot: the engine's tables are
loaded), through a harness of a few instructions at CODE: for each input record it loads
EAX EBX ECX EDX ESI EDI EBP, calls the target, and stores the registers and EFLAGS. Nothing
else runs (no timer, no routes on the asm), so a batch of 65,536 samples costs one
emu_start. A divide that faults goes to XnGine's own divide-error handler, as in the game.
A spec that takes pointers gets its own memory per sample (64 bytes, or its `stride`), and
can bind globals to it (`binds`: the harness copies a global from the sample's memory before
the call and back after it, so the global is an input and an output, compared with the rest
of the sample's memory: the S-buffer's pool pointer, the lights' slots). `setup` calls run on
the asm before a spec's batches (the fog's settings, which the asm patches into its code and
the C reads from the globals the same call sets), `teardown` calls after them.

With --cover, the asm's coverage of object 2 (the patched Unicorn's map; the C image lies
outside it) is OR-ed into build/xn_canon/coverage/equiv.bin (a run of some specs adds to it;
remove it to start afresh), a source of tools/xn_cover.py report.

usage: xn_equiv.py [FUNC ...] [--scale K] [--list] [-j N] [--cover]
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
OBJ2 = (0xC0000, 0x161568)      # object 2's code and data (tools/xn_cover.py)
HBASE = 0x12000000
HSIZE = 0x01000000
VARS = HBASE + 0x100            # cur_in, cur_out, count, target, cur_mem
HALT = HBASE + 0xF00            # a hlt: the harnesses end there
CODE = HBASE + 0x1000           # the harnesses, one after the other (to CODE_END): a write over
CODE_END = HBASE + 0x8000       # code Unicorn has translated would not take effect
STACK = HBASE + 0x10000
INP = HBASE + 0x10000
OUTP = HBASE + 0x200000
SCR = HBASE + 0x400000          # per-sample memory (64 bytes each, or the spec's stride)
MEMMAX = 0x800000               # ... at most this much a batch
BATCH = 1 << 16
REGS = ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp")
SNAP = "save_blades"


def harness(binds=(), stride=64, at=CODE):
    """The batch loop's machine code, to go at `at` (see the module doc); it ends with a jump
    to the hlt at HALT. binds: (global's preferred address, offset in the sample's memory):
    copied into the global before the call and back after it (the sample's memory advancing
    by stride)."""
    v = lambda k: struct.pack("<I", VARS + 4 * k)          # noqa: E731
    cur_in, cur_out, count, target, cur_mem = v(0), v(1), v(2), v(3), v(4)
    b = bytearray()
    if binds:
        b += b"\xA1" + cur_mem                               # mov eax, [cur_mem]
        for g, off in binds:
            b += b"\x8B\x90" + struct.pack("<i", off)        # mov edx, [eax+off]
            b += b"\x89\x15" + struct.pack("<I", LOAD + g)   # mov [g], edx
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
    if binds:
        b += b"\xA1" + cur_mem                               # mov eax, [cur_mem]
        for g, off in binds:
            b += b"\x8B\x15" + struct.pack("<I", LOAD + g)   # mov edx, [g]
            b += b"\x89\x90" + struct.pack("<i", off)        # mov [eax+off], edx
        b += b"\x81\x05" + cur_mem + struct.pack("<I", stride)  # add dword [cur_mem], stride
    b += b"\x83\x05" + cur_out + b"\x20"                     # add dword [cur_out], 32
    b += b"\x83\x05" + cur_in + b"\x1C"                      # add dword [cur_in], 28
    b += b"\xFF\x0D" + count                                 # dec dword [count]
    rel = 0 - (len(b) + 6)
    b += b"\x0F\x85" + struct.pack("<i", rel)                # jnz start
    b += b"\xE9" + struct.pack("<i", HALT - (at + len(b) + 5))      # jmp HALT
    return bytes(b)


class Bench:
    """A machine with the harness and the canonical C image loaded."""

    def __init__(self, snap=SNAP, cover=False):
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
        self.emu.uc.mem_write(HALT, b"\xF4")
        self.harnesses = {}             # (binds, stride) -> its address
        self.code_next = CODE
        self.done = []
        self.cov = None
        if cover:
            import xn_record
            self.cov = xn_record.coverage_lib()
            if self.cov is None:
                raise SystemExit("--cover needs the patched Unicorn (tools/build_unicorn.sh)")
            self.cov.uc_dagger_coverage(self.emu.uc._uch, LOAD + OBJ2[0], LOAD + OBJ2[1], None)

        def stop(uc, address, size, _):
            self.done.append(1)
            uc.emu_stop()
        self.emu.uc.hook_add(UC_HOOK_CODE, stop, None, HALT, HALT)
        self.emu.w("eflags", 0x202 & ~0x200)    # no interrupts: nothing but the samples runs

    def coverage(self):
        """object 2's coverage map so far (bytes, 1 where code ran), or None"""
        import ctypes
        if self.cov is None:
            return None
        buf = ctypes.create_string_buffer(OBJ2[1] - OBJ2[0])
        self.cov.uc_dagger_coverage(self.emu.uc._uch, LOAD + OBJ2[0], LOAD + OBJ2[1], buf)
        return buf.raw

    def close(self):
        import shutil
        self.emu.close()
        shutil.rmtree(self.overlay, ignore_errors=True)

    def run(self, target, inputs, mem=None, binds=(), stride=64):
        """inputs: an array('I') of 7 registers per sample; mem: bytes to write at SCR first
        (stride bytes a sample); binds: see harness(). Returns (outputs array('I'), 8 per
        sample, and the scratch memory after)."""
        emu = self.emu
        n = len(inputs) // 7
        key = (tuple(binds), stride) if binds else ()
        at = self.harnesses.get(key)
        if at is None:
            at = self.harnesses[key] = self.code_next
            code = harness(binds or (), stride, at)
            self.code_next = (at + len(code) + 15) & ~15
            assert self.code_next <= CODE_END, "too many harnesses"
            emu.uc.mem_write(at, code)
        emu.uc.mem_write(INP, inputs.tobytes())
        if mem is not None:
            emu.uc.mem_write(SCR, mem)
        emu.uc.mem_write(VARS, struct.pack("<5I", INP, OUTP, n, target, SCR))
        emu.w("esp", STACK - 0x100)
        emu.w("eip", at)
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
      poke=None, stride=64, binds=None, setup=None, teardown=None, ignore=None):
    """poke: [(preferred address, bytes)] written into the machine for this spec's batches (and
    put back after): a state the helper reads (the clip window). mem: a kind of gen_mem, or
    fn(rng, address) -> (registers, stride bytes at address). binds: [(global's preferred
    address, offset in the sample's memory)] (harness()). setup, teardown: [(preferred
    address, registers)]: asm calls made before the spec's batches and after them.
    ignore: fn(input registers, the asm's output registers) -> [(offset, length)] of the
    sample's memory not compared (a leftover the asm leaves in scratch, its reason in the
    spec's note)."""
    return {"regs": regs, "n": n, "exhaustive": exhaustive, "mem": mem, "outs": outs, "cf": cf,
            "note": note, "poke": poke, "stride": stride, "binds": binds, "setup": setup,
            "teardown": teardown, "ignore": ignore}


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
    out["xn_mat_to_angles"] = S(None, n=1_000_000, mem=gen_mat_to_angles,
                                note="rotations (pitch at and about 90 degrees too) and any "
                                     "entries: cos(pitch) 0 or a ratio above 1 (CF)")
    out["xn_vec_unit_direction"] = S(None, n=500_000, mem=gen_unit_direction,
                                     note="from and to near, apart or the same (0)")
    out["xn_vec_advance"] = S(None, n=500_000, mem=gen_advance,
                              note="distances below 10000h and beyond (0: pos unchanged)")
    out.update(specs_system())
    out.update(specs_screen())
    out.update(specs_world())
    out.update(specs_raster())
    out.update(specs_objects())
    return out


def gen_mat_to_angles(rng, a):
    """a 2.28 matrix at a: a rotation from angles (now and then with the pitch at 90 degrees or
    within a hair of it), or entries of any size"""
    import math
    r = rng.random()
    if r < 0.6:
        m = matrix_mem(rng)
    elif r < 0.8:
        p = rng.choice([math.pi / 2, -math.pi / 2]) + rng.choice([0, 1e-9, -1e-9, 1e-5, -1e-5])
        y, rl = rng.random() * 2 * math.pi, rng.random() * 2 * math.pi
        sp, cp, sy, cy, sr, cr = (math.sin(p), math.cos(p), math.sin(y), math.cos(y),
                                  math.sin(rl), math.cos(rl))
        m = [int(v * (1 << 28)) & 0xFFFFFFFF for v in
             (cy * cr + sp * sy * sr, -cp * sr, 0, 0, cp * cr, 0, sy * cp, sp, cp * cy)]
    else:
        m = [rng.choice([unit28(rng), mixed(rng)]) for _ in range(9)]
    return {"eax": a}, struct.pack("<9I", *m) + bytes(28)


def gen_unit_direction(rng, a):
    """from at a, to at a + 12 (near it, far from it, or the same point), out at a + 24 (its
    old contents random: a 0 result leaves them)"""
    f = vec_vals(rng, "world")
    r = rng.random()
    if r < 0.1:
        t = list(f)
    elif r < 0.5:
        t = [(v + rng.randrange(-64, 65)) & 0xFFFFFFFF for v in f]
    else:
        t = vec_vals(rng, "world")
    return {"eax": a, "edx": a + 12, "ebx": a + 24}, \
        struct.pack("<6I", *(f + t)) + rng.randbytes(12) + bytes(28)


def gen_advance(rng, a):
    """dir (2.14) at a, pos at a + 12; the distance in EDX: below 10000h mostly, then any"""
    import math
    if rng.random() < 0.9:
        th, ph = rng.uniform(0, 2 * math.pi), math.acos(rng.uniform(-1, 1))
        d = [int(0x4000 * v) & 0xFFFFFFFF for v in
             (math.sin(ph) * math.cos(th), math.cos(ph), math.sin(ph) * math.sin(th))]
    else:
        d = [mixed(rng) for _ in range(3)]
    dist = rng.choice([rng.randrange(0, 0x10000), rng.randrange(-0x100, 0x20000), mixed(rng)])
    return {"eax": a, "edx": dist & 0xFFFFFFFF, "ebx": a + 12}, \
        struct.pack("<6I", *(d + vec_vals(rng, "world"))) + bytes(40)


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


# ---- group A (system and input) --------------------------------------------------------------
def _text(rng, n, alphabet):
    return bytes(rng.choice(alphabet) for _ in range(n))


ALNUM = b"0123456789ABCXYZabcxyz"
TEXT = ALNUM + b" .,-+\r\n\t\x80\xff_!"


def gen_mem_text(kind, rng, k):
    """Group A's memory inputs: strings, flag words, tables and a spell's effect slots at
    SCR + 64 * k (registers, 64 bytes)."""
    a = SCR + 64 * k
    if kind == "effects":           # a spell's three effect slots: types 0..60 or FFh
        b = bytes(rng.choice([0xFF, 0xFF, rng.randrange(0, 61)]) if i % 2 == 0 else rng.getrandbits(8)
                  for i in range(6))
        t = rng.choice([0xFF, b[0], b[2], b[4], rng.randrange(0, 61)])
        return {"eax": a, "edx": t | (rng.getrandbits(24) << 8)}, b + bytes(58)
    if kind == "flags8":
        return {"eax": a, "edx": rng.getrandbits(32), "ebx": rng.choice([0, 1, mixed(rng)])}, \
            bytes([rng.getrandbits(8)]) + bytes(63)
    if kind == "flags16":
        return {"eax": a, "edx": rng.getrandbits(32), "ebx": rng.choice([0, 1, mixed(rng)])}, \
            struct.pack("<H", rng.getrandbits(16)) + bytes(62)
    if kind == "number":            # an optional sign, digits, then anything
        s = rng.choice([b"", b"-", b"+", b"--"]) + _text(rng, rng.randrange(0, 12), b"0123456789") \
            + _text(rng, rng.randrange(0, 3), TEXT)
        return {"eax": a}, (s + b"\0").ljust(64, b"\0")
    if kind == "bytes_n":           # n bytes (1..60), mostly zeros: count / find non-zero
        n = rng.randrange(1, 61)
        b = bytes(rng.choice([0, 0, 0, rng.getrandbits(8)]) for _ in range(64))
        return {"eax": a, "edx": n}, b
    if kind == "dwords":            # 15 dwords and one of them (or not) to find
        v = [rng.choice([0, 1, rng.getrandbits(32)]) for _ in range(15)]
        n = rng.randrange(0, 16)
        x = rng.choice(v + [rng.getrandbits(32)])
        return {"eax": a, "edx": x, "ebx": n}, struct.pack("<15I", *v) + bytes(4)
    if kind == "words":             # 31 words, a count of 1..31
        v = [rng.choice([0, 1, rng.getrandbits(16)]) for _ in range(32)]
        n = rng.randrange(1, 32)
        x = rng.choice(v[:n] + [rng.getrandbits(16)])
        return {"eax": a, "edx": x | (rng.getrandbits(16) << 16), "ebx": n}, struct.pack("<32H", *v)
    if kind == "pairs":             # find a byte pair in n bytes (n + 1 readable)
        b = _text(rng, 64, b"ABCD\0")
        n = rng.randrange(1, 63)
        p = rng.choice([b[rng.randrange(0, n)] | b[rng.randrange(0, n)] << 8, rng.getrandbits(16)])
        return {"eax": a, "edx": p, "ebx": n}, b
    if kind == "fields":            # skip n fields
        d = rng.choice(b",.;\0")
        s = _text(rng, rng.randrange(0, 40), b"ab,.;")
        return {"eax": a, "edx": d, "ebx": rng.randrange(0, 6)}, (s + b"\0").ljust(64, b"\0")
    if kind == "copy":              # (dst, src): src a string at +0 (< 32), dst at +32
        s = _text(rng, rng.randrange(0, 30), TEXT.replace(b"\0", b""))
        return {"eax": a + 32, "edx": a, "ebx": rng.choice(b".,\xfc" + s[:1] or b"x")}, \
            (s + b"\0").ljust(32, b"\0") + bytes(32)
    if kind == "copy_rev":          # xn_str_copy (src EAX, dst EDX)
        s = _text(rng, rng.randrange(0, 30), TEXT.replace(b"\0", b""))
        return {"eax": a, "edx": a + 32}, (s + b"\0").ljust(32, b"\0") + bytes(32)
    if kind == "copy80":            # xn_str_copy_word_max80 (src ESI, dst EDI)
        s = _text(rng, rng.randrange(0, 30), TEXT.replace(b"\0", b""))
        return {"esi": a, "edi": a + 32}, (s + b"\0").ljust(32, b"\0") + bytes(32)
    if kind == "string":            # (s, ch): a string and a character in it or not
        s = _text(rng, rng.randrange(0, 40), ALNUM)
        ch = rng.choice(list(s[:3]) + [0, ord("Q"), rng.getrandbits(8)])
        return {"eax": a, "edx": ch | (rng.getrandbits(24) << 8),
                "ebx": rng.randrange(1, 50)}, (s + b"\0").ljust(64, b"\0")
    if kind == "append":
        s = _text(rng, rng.randrange(0, 40), ALNUM)
        return {"eax": a, "edx": rng.getrandbits(32)}, (s + b"\0").ljust(64, b"\0")
    if kind == "fill_asc":          # dst[k] = first + k, n = 1..60
        return {"eax": a, "edx": rng.getrandbits(32), "ebx": rng.randrange(1, 61)}, bytes(64)
    if kind == "fill16":            # bytes / 2 words of value, bytes 0..64
        return {"eax": a, "edx": rng.getrandbits(32),
                "ebx": rng.randrange(0, 65) | (rng.getrandbits(16) << 16)}, bytes(64)
    if kind == "from_int":          # value as ndigits (2..10) digits at dst
        nd = rng.randrange(2, 11)
        v = rng.choice([rng.randrange(-10 ** (nd - 1) + 1, 10 ** (nd - 1)), mixed(rng)])
        return {"eax": v & 0xFFFFFFFF, "edx": a, "ebx": nd}, bytes(64)
    if kind == "insert":            # c AL at s[at] (ESI s, EDI at)
        s = _text(rng, rng.randrange(1, 40), ALNUM)
        return {"eax": rng.getrandbits(32) | 1, "esi": a, "edi": rng.randrange(0, len(s))}, \
            (s + b"\0").ljust(64, b"\0")
    if kind == "delete":            # (at EAX, s EDX)
        s = _text(rng, rng.randrange(1, 40), ALNUM)
        return {"eax": rng.randrange(0, len(s)), "edx": a}, (s + b"\0").ljust(64, b"\0")
    raise SystemExit("unknown memory spec %r" % (kind,))


def specs_system():
    out = {}
    out["xn_rand_noise_2d"] = S(r_regs(eax=mixed, edx=mixed), 1_000_000, exhaustive=(
        1 << 24, lambda i: {"eax": i & 0xFFF, "edx": (i >> 12) & 0xFFF}),
        note="every x and y in 0..FFFh (16 lattice cells each way, every fraction), then random")
    out["xn_mem_align_up"] = S(r_regs(eax=mixed, edx=lambda rng: rng.choice(
        [1 << rng.randrange(0, 32), mixed(rng)])), 2_000_000,
        note="powers of 2 and any value (the asm's formula either way)")
    out["xn_str_char_lower"] = S(None, exhaustive=(1 << 16, lambda i: {
        "eax": (i & 0xFF) | ((i >> 8) * 0x01010100)}), n=100_000,
        note="every character with every repeated upper byte, then random")
    out["xn_str_char_upper"] = S(None, exhaustive=(1 << 16, lambda i: {
        "eax": (i & 0xFF) | ((i >> 8) * 0x01010100)}), n=100_000)
    for name, kind, n in (("spell_find_effect_type", "effects", 500_000),
                          ("spell_has_no_effects", "effects", 500_000),
                          ("xn_bits_set_or_clear_u8", "flags8", 500_000),
                          ("xn_bits_set_or_clear_u16", "flags16", 500_000),
                          ("xn_str_to_int", "number", 500_000),
                          ("xn_str_count_nonzero", "bytes_n", 300_000),
                          ("xn_str_find_nonzero", "bytes_n", 300_000),
                          ("xn_str_find_u32", "dwords", 300_000),
                          ("xn_str_find_u16", "words", 300_000),
                          ("xn_str_find_byte_pair", "pairs", 300_000),
                          ("xn_str_skip_fields", "fields", 300_000),
                          ("xn_str_copy_line", "copy", 300_000),
                          ("xn_str_copy_word", "copy", 300_000),
                          ("xn_str_copy_alnum", "copy", 300_000),
                          ("xn_str_copy_until", "copy", 300_000),
                          ("xn_str_copy", "copy_rev", 300_000),
                          ("xn_str_copy_word_max80", "copy80", 300_000),
                          ("xn_str_length", "string", 300_000),
                          ("xn_str_find_char", "string", 300_000),
                          ("xn_str_find_char_n", "string", 300_000),
                          ("xn_str_append_char", "append", 300_000),
                          ("xn_str_fill_ascending", "fill_asc", 300_000),
                          ("xn_str_fill_ascending_v2", "fill_asc", 300_000),
                          ("xn_str_fill_u16", "fill16", 300_000),
                          ("xn_str_from_int", "from_int", 300_000),
                          ("xn_str_insert_char", "insert", 300_000),
                          ("xn_str_delete_char", "delete", 300_000)):
        out[name] = S(None, n=n, mem=kind)
    return out


# ---- group E (world and collision) -----------------------------------------------------------
def unit16(rng):
    """a 16.16 normal component, mostly within +-1.1, or anything"""
    r = rng.random()
    if r < 0.1:
        return mixed(rng)
    if r < 0.15:
        return rng.choice([0, 0x10000, 0xFFFF0000, 1, 0xFFFFFFFF])
    return int(rng.uniform(-1.1, 1.1) * 0x10000) & 0xFFFFFFFF


def model24(rng):
    """a model-space coordinate (24.8): small, a building's, or anything"""
    r = rng.random()
    if r < 0.4:
        return rng.randrange(-0x40000, 0x40000) & 0xFFFFFFFF
    if r < 0.85:
        return rng.randrange(-0x1000000, 0x1000000) & 0xFFFFFFFF
    return mixed(rng)


def radius(rng):
    r = rng.random()
    if r < 0.8:
        return rng.randrange(0, 0x20000)
    return mixed(rng)


def _near(rng, c, spread):
    return [(c[j] + rng.randrange(-spread, spread)) & 0xFFFFFFFF for j in range(3)]


def _vecs(*vs):
    return b"".join(struct.pack("<3I", *v) for v in vs)


COORDS = {"world": world, "model": model24, "unit16": unit16}
COLLIDE_SHAPES = {"creg_p", "creg_p_r", "creg_seg", "preg_c_dh", "preg_e_b", "preg_ev_b",
                  "nreg_c_s_r", "c_s_r", "nreg_c_p0_p1", "planes"}


def gen_mem_collide(kind, rng, k):
    """Group E's memory inputs: the vectors and points a collision primitive takes through
    pointers, at SCR + 64 * k, pointed at by the registers the asm takes them in. The shape's
    name says the registers: creg = a centre in EAX EDX EBX, preg = a point there, nreg = a
    normal there; then what ECX, ESI, EDI hold (p a point, r a radius, c a centre, s a sphere's
    centre, seg two ends, e extents, b a base)."""
    a = SCR + 64 * k
    t, vk = kind[0], kind[1]
    g = COORDS.get(vk, mixed)
    vec = lambda: [g(rng) for _ in range(3)]            # noqa: E731
    if t == "creg_p":           # centre in EAX EDX EBX, ECX r, ESI -> a point near it
        c = vec()
        p = _near(rng, c, 0x4000) if rng.random() < 0.7 else vec()
        return {"eax": c[0], "edx": c[1], "ebx": c[2], "ecx": radius(rng), "esi": a}, \
            _vecs(p) + bytes(52)
    if t == "creg_p_r":         # the same and EDI r2
        c = vec()
        p = _near(rng, c, 0x4000) if rng.random() < 0.7 else vec()
        return {"eax": c[0], "edx": c[1], "ebx": c[2], "ecx": radius(rng), "esi": a,
                "edi": radius(rng)}, _vecs(p) + bytes(52)
    if t == "creg_seg":         # centre in regs, ECX r, ESI -> p0, EDI -> p1
        c = vec()
        p0 = _near(rng, c, 0x8000) if rng.random() < 0.7 else vec()
        p1 = _near(rng, c, 0x8000) if rng.random() < 0.7 else vec()
        if rng.random() < 0.05:
            p1 = list(p0)
        return {"eax": c[0], "edx": c[1], "ebx": c[2], "ecx": radius(rng), "esi": a,
                "edi": a + 12}, _vecs(p0, p1) + bytes(40)
    if t == "preg_c_dh":        # p in regs, ECX -> c, ESI d, EDI h
        c = vec()
        p = _near(rng, c, 0x400) if rng.random() < 0.7 else vec()
        return {"eax": p[0], "edx": p[1], "ebx": p[2], "ecx": a, "esi": radius(rng),
                "edi": radius(rng)}, _vecs(c) + bytes(52)
    if t == "preg_e_b":         # p in regs, ECX -> extents, ESI -> base
        b = vec()
        e = [rng.randrange(-0x800, 0x800) & 0xFFFFFFFF for _ in range(3)]
        p = _near(rng, b, 0x800)
        return {"eax": p[0], "edx": p[1], "ebx": p[2], "ecx": a, "esi": a + 12}, \
            _vecs(e, b) + bytes(40)
    if t == "preg_ev_b":        # p in regs, ECX the extent, ESI -> base
        b = vec()
        p = _near(rng, b, 0x800)
        return {"eax": p[0], "edx": p[1], "ebx": p[2], "ecx": rng.randrange(-0x800, 0x800) &
                0xFFFFFFFF, "esi": a}, _vecs(b) + bytes(52)
    if t == "nreg_c_s_r":       # normal in regs, ECX -> c, ESI -> s, EDI r
        n = [unit16(rng) for _ in range(3)]
        c = vec()
        s = _near(rng, c, 0x8000) if rng.random() < 0.8 else vec()
        return {"eax": n[0], "edx": n[1], "ebx": n[2], "ecx": a, "esi": a + 12,
                "edi": radius(rng)}, _vecs(c, s) + bytes(40)
    if t == "c_s_r":            # ECX -> c, ESI -> s, EDI r (no normal)
        c = vec()
        s = _near(rng, c, 0x8000)
        return {"ecx": a, "esi": a + 12, "edi": radius(rng)}, _vecs(c, s) + bytes(40)
    if t == "nreg_c_p0_p1":     # normal in regs, ECX -> c, ESI -> p0, EDI -> p1
        n = [unit16(rng) for _ in range(3)]
        c = vec()
        p0 = _near(rng, c, 0x10000)
        p1 = _near(rng, c, 0x10000) if rng.random() < 0.8 else vec()
        if rng.random() < 0.2:      # vertical segments
            p1[0], p1[2] = p0[0], p0[2]
        if rng.random() < 0.05:     # nearly parallel
            n = [0, 0x10000, 0]
            p1[1] = (p0[1] + rng.randrange(-16, 16)) & 0xFFFFFFFF
        return {"eax": n[0], "edx": n[1], "ebx": n[2], "ecx": a, "esi": a + 12,
                "edi": a + 24}, _vecs(c, p0, p1) + bytes(28)
    if t == "planes":           # EAX -> n1, EDX -> p1, EBX -> n2, ECX -> p2
        n1 = [unit16(rng) for _ in range(3)]
        n2 = [unit16(rng) for _ in range(3)]
        if rng.random() < 0.1:
            n2 = _near(rng, n1, 0x800)
        return {"eax": a, "edx": a + 12, "ebx": a + 24, "ecx": a + 36}, \
            _vecs(n1, vec(), n2, vec()) + bytes(16)
    raise SystemExit("unknown memory spec %r" % (kind,))


def specs_world():
    s = {}
    for name, shape, coords in (
            ("xn_collide_sphere_sphere", "creg_p_r", "world"),
            ("xn_collide_sphere_sphere_approx", "creg_p_r", "world"),
            ("xn_collide_point_in_sphere", "creg_p", "world"),
            ("xn_collide_point_in_sphere_approx", "creg_p", "world"),
            ("xn_collide_point_in_cylinder", "preg_c_dh", "world"),
            ("xn_collide_point_in_box", "preg_e_b", "world"),
            ("xn_collide_point_in_cube", "preg_ev_b", "world"),
            ("xn_collide_segment_sphere", "creg_seg", "world"),
            ("xn_collide_segment_sphere_fx", "creg_seg", "model"),
            ("xn_collide_segment_sphere_approx", "creg_seg", "world"),
            ("xn_collide_sphere_plane", "nreg_c_s_r", "model"),
            ("xn_collide_sphere_plane_xz", "nreg_c_s_r", "model"),
            ("xn_collide_sphere_plane_yz", "nreg_c_s_r", "model"),
            ("xn_collide_plane_distance", "nreg_c_s_r", "model"),
            ("xn_collide_segment_plane", "nreg_c_p0_p1", "model"),
            ("xn_collide_vsegment_plane", "nreg_c_p0_p1", "model"),
            ("xn_collide_line_plane_point", "nreg_c_p0_p1", "model"),
            ("xn_collide_vline_plane_point", "nreg_c_p0_p1", "model"),
            ("xn_collide_plane_plane_line", "planes", "model")):
        s[name] = S(None, 2_000_000, mem=(shape, coords))
    s["xn_collide_sphere_plane_z"] = S(None, 800_000, mem=("c_s_r", "world"))
    s["xn_world_cell_at"] = S(r_regs(eax=world, edx=world), 4_000_000)
    s["xn_world_cell_index"] = S(r_regs(eax=world, ebx=world), 2_000_000)
    s["xn_world_cell_slot"] = S(r_regs(eax=world, ebx=world), 2_000_000)
    return s


# ---- group D (3D objects) -------------------------------------------------------------------
# They read the view's globals (near and far planes, focal lengths, half size, the matrices) as
# the snapshot (save_blades) has them.
NEAR, FAR = 2560, 0x60000               # about the snapshot's near and far planes (24.8)
PICK_VIEW_X, PICK_VIEW_Y, PICK_DISTANCE = 0x120288, 0x12028C, 0x120290


def cam(rng):
    """a camera-space coordinate (24.8): mostly within the view's reach, now and then anything"""
    r = rng.random()
    if r < 0.6:
        return rng.randrange(-FAR, FAR) & 0xFFFFFFFF
    if r < 0.9:
        return rng.randrange(-0x1000000, 0x1000000) & 0xFFFFFFFF
    return mixed(rng)


def depth(rng):
    """a z: in front mostly, about the near plane, behind, or anything"""
    r = rng.random()
    if r < 0.6:
        return rng.randrange(NEAR // 2, FAR)
    if r < 0.75:
        return rng.randrange(-0x400, 0x400) & 0xFFFFFFFF
    if r < 0.9:
        return rng.randrange(-FAR, 0) & 0xFFFFFFFF
    return mixed(rng)


def half(rng):
    return rng.choice([rng.randrange(0, 0x400), rng.randrange(0, 0x10000), mixed(rng)])


def s32(v):
    v &= 0xFFFFFFFF
    return v - (1 << 32) if v & 0x80000000 else v


# the clipper's six planes: inside when side(x, y, z) >= 0 (and the far plane: z <= far)
PLANES = {
    "left": lambda x, y, z: x + z,
    "right": lambda x, y, z: z - x,
    "bottom": lambda x, y, z: z - y,
    "top": lambda x, y, z: y + z,
    "near": lambda x, y, z: z - NEAR,
    "far": lambda x, y, z: FAR - z,
}


def _cam_point(rng):
    return [s32(cam(rng)), s32(cam(rng)), s32(depth(rng))]


def gen_mem_edge(plane, rng, k):
    """An edge for a clip intersection, at SCR + 64 * k: a vertex inside the plane at EBX, one
    outside it at ESI (now and then any two points: the cut's divide may overflow, Q-SYS-01);
    the new vertex goes to EDI."""
    a = SCR + 64 * k
    side = PLANES[plane]
    pin = pout = None
    if rng.random() < 0.9:
        for _ in range(64):
            p = _cam_point(rng)
            if pin is None and side(*p) >= 0:
                pin = p
            elif pout is None and side(*p) < 0:
                pout = p
            if pin and pout:
                break
    pin = pin or _cam_point(rng)
    pout = pout or _cam_point(rng)

    def vertex(p, code):
        return struct.pack("<iiiB3x", p[0], p[1], p[2], code)
    return {"ebx": a, "esi": a + 16, "edi": a + 32}, \
        vertex(pin, rng.getrandbits(6)) + vertex(pout, rng.getrandbits(6)) + bytes(32)


def gen_view_matrix(where):
    """the view matrix's scaling: a matrix at the sample's address (2.28: a rotation or any
    entries); xn_cam_scale_matrix's destination 48 bytes on (or the source itself)"""
    def gen(rng, a):
        m = struct.pack("<9I", *matrix_mem(rng)) + bytes(12)
        junk = rng.randbytes(48)
        regs = {"eax": a}
        if where == "src_dst":
            regs["edx"] = a if rng.random() < 0.2 else a + 48
        return regs, m + junk + bytes(32)
    return gen


def specs_objects():
    s = {}
    s["xn_poly_outcode"] = S(r_regs(eax=cam, edx=cam, ebx=depth), 2_000_000)
    s["xn_poly_outcode_eax"] = S(r_regs(eax=cam, edx=cam, ebx=depth), 500_000)
    for plane in PLANES:
        s["xn_poly_clip_intersect_" + plane] = S(None, 1_000_000, mem=("edge", plane))
    s["xn_cam_project"] = S(r_regs(eax=cam, edx=cam, ebx=depth), 2_000_000)
    s["xn_cam_project_scaled"] = S(r_regs(eax=cam, edx=cam, ebx=depth), 1_000_000)
    s["xn_cam_scale_matrix"] = S(None, 500_000, mem=gen_view_matrix("src_dst"), stride=128,
                                 note="into another matrix or in place (the same address)")
    s["xn_cam_scale_matrix_in_place"] = S(None, 300_000, mem=gen_view_matrix("in_place"),
                                          stride=128)
    s["xn_cam_unscale_matrix"] = S(None, 300_000, mem=gen_view_matrix("in_place"), stride=128)
    s["xn_cam_cull_sphere"] = S(r_regs(eax=cam, edx=cam, ebx=cam,
                                       ecx=lambda rng: rng.choice([rng.randrange(0, 0x4000),
                                                                   rng.randrange(0, 0x40000),
                                                                   mixed(rng)])), 2_000_000)
    for k, (vx, vy, pd) in enumerate(((0x1000, -0x2000, 0x8000), (-0x30000, 0x20000, 0x40000),
                                      (0x7FFF0000, -0x7FFF0000, 0x7FFFFFFF),
                                      (0, 0, 0), (-0x800, 0x800, -0x1000))):
        poke = [(PICK_VIEW_X, struct.pack("<i", vx)), (PICK_VIEW_Y, struct.pack("<i", vy)),
                (PICK_DISTANCE, struct.pack("<i", pd))]
        for f in ("xn_cam_sphere_dist_x", "xn_cam_sphere_dist_y"):
            s["%s@%d" % (f, k)] = S(r_regs(eax=mixed, ebx=mixed, edi=mixed), 200_000,
                                    poke=poke, note="the pick's point (%d, %d, %d)" % (vx, vy, pd))
    s["xn_flat_rotate_offset"] = S(r_regs(ecx=half, ebp=half), 1_000_000)
    return s


# ---- group C (the rasteriser and the frame) ----------------------------------------------------
# object-2 globals (config/names.csv)
SPAN_NEXT, POLY_NEXT, SPAN_DZDX = 0x0CEA60, 0x0CEA64, 0x15B980
LIGHT_SLOTS = 0x158C28          # xn_light_point_intensity, _falloff, _x, _y, _z: 3 dwords each
SHADE_ROW = 0x158C64            # xn_light_shade_row: the row a face's lighting accumulates
SHADE_LAST_ROW = 0x0124BF00     # xn_shade_table_last_row in the snapshot (linear)
SET_FOG = 0x14D23C              # xn_shade_set_fog
SNAP_FOG_START = 0x400          # the snapshot's fog start (far >> 8 = 600h)


def _dzdx(rng, inv_z=0x1000000):
    """a polygon's d(1/z)/dx: 0, small, about inv_z / 100 a pixel, or anything"""
    r = rng.random()
    if r < 0.1:
        return 0
    if r < 0.4:
        return rng.randrange(-0x400, 0x400)
    if r < 0.95:
        return int(inv_z * rng.uniform(-0.02, 0.02))
    return s32(mixed(rng))


def gen_span_insert(rng, a):
    """A screen row's S-buffer and a span to insert (512 bytes at a):
      +00  the bound globals: xn_render_span_next (the pool: +C0h), xn_render_poly_next (the
           polygon being built), xn_span_dzdx (its d(1/z)/dx, mostly its +5Ch)
      +10  the row's head (its .next)
      +20  0 to 6 spans, sorted, apart or touching, in [0, 320); +80 the sentinel (next =
           itself, 320..321)
      +90  eight polygons' +5Ch (polygon i = a + 90h + 4i - 5Ch)
      +C0  the pool (20 nodes)
    The new span [x0, x1) (EBP, EBX) starts at a span's start or end or anywhere, with EAX
    ECX's 1/z now and then that span's (the ties)."""
    m = bytearray(0xC0) + rng.randbytes(512 - 0xC0)
    base = rng.randrange(0x100000, 0x20000000)
    inv = lambda: int(base * rng.uniform(0.5, 2.0)) if rng.random() < 0.95 \
        else s32(mixed(rng))                                    # noqa: E731
    dz = [_dzdx(rng, base) for _ in range(8)]
    for i, d in enumerate(dz):
        struct.pack_into("<i", m, 0x90 + 4 * i, d)
    poly = lambda i: a + 0x90 + 4 * i - 0x5C                    # noqa: E731
    k = rng.choice([0, 1, 1, 2, 2, 3, 3, 4, 5, 6])
    cuts = sorted(rng.sample(range(0, 320), 2 * k))
    spans = [[cuts[2 * i], cuts[2 * i + 1]] for i in range(k)]
    for i in range(k - 1):
        if rng.random() < 0.4:
            spans[i][1] = spans[i + 1][0]                       # touching
    zs = [inv() for _ in range(k)]
    sentinel = a + 0x80
    for i, (x0, x1) in enumerate(spans):
        nxt = a + 0x20 + 16 * (i + 1) if i + 1 < k else sentinel
        struct.pack_into("<IHHiI", m, 0x20 + 16 * i, nxt, x1, x0, zs[i], poly(rng.randrange(7)))
    struct.pack_into("<IHHiI", m, 0x80, sentinel, 321, 320, rng.choice([0, inv()]), poly(7))
    struct.pack_into("<I", m, 0x10, a + 0x20 if k else sentinel)
    cur = rng.randrange(8)
    struct.pack_into("<IIi", m, 0, a + 0xC0, poly(cur),
                     dz[cur] if rng.random() < 0.9 else _dzdx(rng, base))
    ends = [x for sp in spans for x in sp]
    x0 = rng.choice(ends + [rng.randrange(0, 320)]) if ends and rng.random() < 0.6 \
        else rng.randrange(0, 320)
    x0 = min(x0, 319)
    later = [x for x in ends if x > x0] + [320]
    x1 = rng.choice(later) if rng.random() < 0.5 else rng.randrange(x0 + 1, 321)
    z = inv()
    for i, (s0, _s1) in enumerate(spans):
        if s0 == x0 and rng.random() < 0.5:
            z = zs[i]                                           # a tie at a shared start
    return {"esi": a + 0x10, "ebp": x0, "ebx": x1, "ecx": z & 0xFFFFFFFF}, bytes(m)


def _unit_normal(rng, scale):
    import math
    if rng.random() < 0.15:
        v = [0, 0, 0]
        v[rng.randrange(3)] = rng.choice([scale, -scale])
        return v
    th, ph = rng.uniform(0, 2 * math.pi), math.acos(rng.uniform(-1, 1))
    return [int(scale * math.sin(ph) * math.cos(th)), int(scale * math.cos(ph)),
            int(scale * math.sin(ph) * math.sin(th))]


def gen_light_point(rng, a):
    """A point light of a model face's list against the face (256 bytes at a): the asm's
    three slots (bound: +00, 15 dwords, anything before the call), the face normal (+40, 8-bit
    fraction), the light ref (+4C: the light in the model's axes, 24.8, from 0 to 3000 units
    off the face plane and beside it; intensity 1..32; range_sq radius^2 * (intensity / 2) <<
    4, radius 1..512), the face (plane d at +64: the plane through a point of the face), the
    handle (+60: the matrix pointer at +68, rel at +74), the pool's light matrix (+80: a
    rotation, 2.28) and the polygon (+B0: face, handle, normal at +48h). EBP the slots filled
    * 4: 0 or 4 (the third slot's jump into the builder returns past the harness)."""
    m = bytearray(rng.randbytes(60)) + bytearray(196)
    n = _unit_normal(rng, 256) if rng.random() < 0.95 else [s32(mixed(rng)) for _ in range(3)]
    p0 = [rng.randrange(-0x200000, 0x200000) for _ in range(3)]
    d = sum(p * q for p, q in zip(p0, n))
    if rng.random() < 0.9:
        dist = rng.choice([rng.randrange(-100, 300), rng.randrange(-100, 3000)]) << 8
        side = [rng.randrange(-0x40000, 0x40000) for _ in range(3)]
        light = [p0[j] + (n[j] * dist >> 8) + side[j] for j in range(3)]
    else:
        light = [s32(world(rng)) for _ in range(3)]
    intensity = rng.choice([rng.randrange(1, 33), rng.randrange(1, 33), 0, s32(mixed(rng))])
    radius = rng.randrange(1, 513)
    rsq = (radius * radius * (abs(intensity) // 2)) << 4 if rng.random() < 0.9 \
        else s32(mixed(rng))
    struct.pack_into("<iii", m, 0x40, *n)
    struct.pack_into("<iiiii", m, 0x50, *(s32(v) for v in light + [intensity, rsq]))
    struct.pack_into("<i", m, 0x64, s32(d) if rng.random() < 0.95 else s32(mixed(rng)))
    struct.pack_into("<I", m, 0x68, (a + 0x80 - 0x1C20) & 0xFFFFFFFF)
    struct.pack_into("<iii", m, 0x74, *(rng.randrange(-0x1000000, 0x1000000) for _ in range(3)))
    struct.pack_into("<9I", m, 0x80, *matrix_mem(rng))
    struct.pack_into("<II", m, 0xB0, a + 0x50, a + 0x60)
    struct.pack_into("<I", m, 0xB0 + 0x48, a + 0x40)
    return {"edi": a + 0xB0, "esi": a + 0x4C, "ebp": rng.choice([0, 4])}, bytes(m)


def gen_light_directional(terrain):
    """A directional light against a face (128 bytes at a): the row being accumulated (bound:
    +00, a row of the shade table, its fraction byte too), the light ref (+10: its direction
    and intensity), the face normal (+40, 8-bit fraction: models through the polygon's +48h,
    the polygon at +24; terrain cells in its +50h..+58h)."""
    def gen(rng, a):
        m = bytearray(128)
        row = SHADE_LAST_ROW - rng.randrange(-0x200, 0x4000) if rng.random() < 0.95 \
            else rng.getrandbits(32)
        struct.pack_into("<I", m, 0, row & 0xFFFFFFFF)
        n = _unit_normal(rng, 256) if rng.random() < 0.95 else [s32(mixed(rng)) for _ in range(3)]
        struct.pack_into("<iii", m, 0x40, *n)
        dscale = rng.choice([256, 1 << 14, 1 << 16])
        dr = _unit_normal(rng, dscale) if rng.random() < 0.9 else [s32(mixed(rng)) for _ in range(3)]
        inten = rng.choice([rng.randrange(1, 33), rng.randrange(-32, 256), s32(mixed(rng))])
        struct.pack_into("<iiii", m, 0x14, *dr, inten)
        if terrain:
            return {"edi": a + 0x40 - 0x50, "esi": a + 0x10}, bytes(m)
        struct.pack_into("<I", m, 0x24 + 0x48, a + 0x40)
        return {"edi": a + 0x24, "esi": a + 0x10}, bytes(m)
    return gen


def gen_fog_span(start):
    """A span for the fog (512 bytes at a): the polygon's d(1/z)/dx (+00, its +5Ch) and its
    inverse (+04, its +60h: 2^32 / +5Ch, 0 for 0, as the light setup stores it), the span
    node's 1/z (+08: 2^40 / z, z from the near plane to the far one, beyond it too when the
    depth changes along the span; now and then the clamp's, the fog start's, their
    neighbours, 1 or 7FFFFFFFh), its pixels (+10, EBP of them: 1..320, now and then up to
    496). The domain is the projectors': a positive 1/z, and a span at the same depth within
    the far plane (the clipper's far cut). Outside it the asm reads beyond its memory (the C
    as well): the same-depth remap does not clamp 1/z, so far beyond the far plane its rows
    lie far before the fog table; a negative 1/z wraps the fog start's difference and makes
    a negative count, which indexes the reciprocal and unroll tables backwards."""
    inv_start = (1 << 32) // start if start > 1 else 0
    min_inv = (1 << 40) // FAR

    def gen(rng, a):
        m = bytearray(rng.randbytes(512))
        n = rng.randrange(1, 321) if rng.random() < 0.95 else rng.randrange(321, 497)
        dz0 = rng.random() < 0.1
        if rng.random() < 0.95:
            inv_z = int((1 << 40) / rng.uniform(0xA00, FAR if dz0 else FAR * 1.3))
        else:
            inv_z = rng.choice([min_inv, min_inv + 1, inv_start - 1, inv_start, inv_start + 1,
                                0x7FFFFFFF] + ([] if dz0 else [1, min_inv - 1]))
            inv_z = max(inv_z, min_inv) if dz0 else max(inv_z, 1)
        dz = 0 if dz0 else _dzdx(rng, inv_z) or 3
        if abs(dz) < 3:
            dz = 3 if dz > 0 else -3            # its inverse fits (2^32 / 2 does not)
        inv = 0 if dz == 0 else int((1 << 32) / dz)
        struct.pack_into("<iii", m, 0, dz, inv, s32(inv_z))
        return {"ebx": a - 0x5C, "esi": a, "ebp": n, "edi": a + 15}, bytes(m)
    return gen


def gen_sort_pairs(which):
    """Pairs (key, value) to sort (256 bytes at a): 1 to 30 of them, keys from a few values
    (ties) or any; lo and hi the first and last pair's byte offsets (now and then a part of
    the array; the range step: lo < hi)."""
    def gen(rng, a):
        m = bytearray(256)
        n = rng.randrange(1, 31)
        pool = [s32(mixed(rng)) for _ in range(rng.choice([2, 4, 30]))]
        for i in range(32):
            key = rng.choice(pool) if rng.random() < 0.8 else s32(mixed(rng))
            struct.pack_into("<iI", m, 8 * i, key, rng.getrandbits(32))
        lo, hi = 0, 8 * (n - 1)
        if rng.random() < 0.2 and n > 2:
            lo = 8 * rng.randrange(0, n - 1)
        if which == "range" and hi <= lo:
            hi = lo + 8
        return {"eax": a, "edx": lo, "ebx": hi}, bytes(m)
    return gen


def specs_raster():
    s = {}
    s["xn_span_insert"] = S(None, 1_000_000, mem=gen_span_insert, stride=512,
                            binds=[(SPAN_NEXT, 0), (POLY_NEXT, 4), (SPAN_DZDX, 8)],
                            note="a row of 0-6 spans (apart, touching, ties at a shared start) "
                                 "and a span put into it; the row, the pool and the globals "
                                 "compared")
    s["xn_light_add_point"] = S(
        None, 1_000_000, mem=gen_light_point, stride=256,
        binds=[(LIGHT_SLOTS + 4 * i, 4 * i) for i in range(15)],
        ignore=lambda r, out: [(r["ebp"], 4)] if out[6] == r["ebp"] else [],
        note="a point light's intensity and falloff at a face and its foot point (slots 1 and "
             "2); a rejected light's intensity slot is not compared: the asm stores the "
             "intensity before its falloff test, a leftover in a slot nothing counts (with the "
             "game's intensity <= 32 and radius <= 512 a light that fails the falloff test "
             "fails the intensity test first)")
    s["xn_light_add_directional"] = S(None, 1_000_000, mem=gen_light_directional(False),
                                      stride=128, binds=[(SHADE_ROW, 0)])
    s["xn_light_terrain_add_directional"] = S(None, 1_000_000, mem=gen_light_directional(True),
                                              stride=128, binds=[(SHADE_ROW, 0)])
    for start in (SNAP_FOG_START, 0x100, 0x5C0, 0x5FF, 0x10, 1, 0):
        s["xn_shade_fog_span@%X" % start] = S(
            None, 300_000 if start == SNAP_FOG_START else 100_000, mem=gen_fog_span(start),
            stride=512,
            setup=[(SET_FOG, {"eax": start})], teardown=[(SET_FOG, {"eax": SNAP_FOG_START})],
            note="fog start %Xh (far >> 8 = 600h), set by the asm's xn_shade_set_fog%s" % (
                start, " (Q-SHADE-02)" if start < 2 else ""))
    s["xn_render_sort_pairs"] = S(None, 300_000, mem=gen_sort_pairs("sort"), stride=256)
    s["xn_render_sort_pairs_range"] = S(None, 300_000, mem=gen_sort_pairs("range"), stride=256)
    return s


def gen_mem(kind, rng, k, base):
    """(registers, 64 bytes of memory) for sample k of a pointer spec. kind: a name (group A's
    strings and tables: gen_mem_text) or (shape, coordinate kind) (the pilot's vectors and
    matrices below; group D's clip edges: gen_mem_edge; group E's collision shapes:
    gen_mem_collide). Specs with memory of another size give a function instead (S())."""
    if isinstance(kind, str):
        return gen_mem_text(kind, rng, k)
    if kind[0] == "edge":
        return gen_mem_edge(kind[1], rng, k)
    if kind[0] in COLLIDE_SHAPES:
        return gen_mem_collide(kind, rng, k)
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


_ABI = None
_DROPPED = None


def abi_rows():
    global _ABI
    if _ABI is None:
        import xn_abi
        _ABI = xn_abi.read_abi()
    return _ABI


def dropped_outputs():
    """({va: register parts}, {va: flags}) config/xngine_dropped.csv (and XN_DROPPED) excuses
    in a function's own records: outputs the canonical C no longer computes."""
    global _DROPPED
    if _DROPPED is None:
        import xn_rc
        _mem, regs, flags, _rows = xn_rc.load_dropped()
        _DROPPED = (regs, {k: v for k, v in flags.items() if k is not None})
    return _DROPPED


def outputs_of(va, spec):
    """(registers compared, their value masks, CF compared, what was dropped) for a function:
    its ABI row's outputs less the dropped ones; CF when the row outputs it or the spec asks,
    unless dropped."""
    import xn_abi
    row = abi_rows()[va]
    dregs, dflags = dropped_outputs()
    out = row["out"] & ~dregs.get(va, 0)
    cfbit = xn_abi.BIT["CF"]
    outs = [r for r in ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp") if out & xn_abi.RMASK[r]]
    masks = {r: xn_abi.value_mask(r, out) for r in outs}
    cf = (spec["cf"] or bool(row["fout"] & cfbit)) and not dflags.get(va, 0) & cfbit
    gone = [r for r in ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp")
            if row["out"] & dregs.get(va, 0) & xn_abi.RMASK[r]]
    if row["fout"] & dflags.get(va, 0) & cfbit:
        gone.append("CF")
    return outs, masks, cf, gone


def compare_func(name, spec, bench, scale=1.0, seed=1, verbose=False):
    import xn_rc
    fname = name.split("@")[0]
    va = xn_rc.func_va(fname)
    route = bench.img.routes.get(va)
    if route is None or route["kind"] != "shim":
        return {"name": name, "error": "not canonical (no test shim)"}
    outs, masks, cf, gone = outputs_of(va, spec)
    spec = dict(spec, dropped=gone)
    rng = random.Random(seed)
    t0 = time.time()
    saved = []
    for a, data in spec.get("poke") or ():
        saved.append((a, bytes(bench.emu.uc.mem_read(LOAD + a, len(data)))))
        bench.emu.uc.mem_write(LOAD + a, data)
    try:
        asm_calls(bench, spec.get("setup"))
        return _compare(name, spec, bench, va, route, outs, masks, cf, rng, scale, verbose, t0)
    finally:
        asm_calls(bench, spec.get("teardown"))
        for a, data in saved:
            bench.emu.uc.mem_write(LOAD + a, data)


def asm_calls(bench, calls):
    """A spec's setup or teardown: each (preferred address, registers) called on the asm."""
    for va, regs in calls or ():
        inp = array.array("I", [regs.get(r, 0) & 0xFFFFFFFF for r in REGS])
        bench.run(LOAD + va, inp)


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
    stride = spec.get("stride") or 64
    binds = spec.get("binds") or ()
    while k < todo:
        n = min(BATCH, MEMMAX // stride, todo - k)
        inp = array.array("I", bytes(28 * n))
        mem = bytearray(stride * n) if spec["mem"] else None
        for j in range(n):
            i = k + j
            if i < nex:
                regs = ex[1](i)
            elif callable(spec["mem"]):
                regs, m = spec["mem"](rng, SCR + stride * j)
                assert len(m) == stride, (name, len(m))
                mem[stride * j:stride * (j + 1)] = m
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
        a_out, a_mem = bench.run(LOAD + va, inp, bytes(mem) if mem is not None else None,
                                 binds, stride)
        c_out, c_mem = bench.run(route["to"], inp, bytes(mem) if mem is not None else None,
                                 binds, stride)
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
            if mem is not None:
                am, cm = a_mem[stride * j:stride * (j + 1)], c_mem[stride * j:stride * (j + 1)]
                if am != cm and spec.get("ignore"):
                    for off, ln in spec["ignore"](
                            {r: inp[7 * j + c] for c, r in enumerate(REGS)}, a_out[o:o + 8]):
                        am = am[:off] + bytes(ln) + am[off + ln:]
                        cm = cm[:off] + bytes(ln) + cm[off + ln:]
                if am != cm:
                    off = next(q for q in range(stride) if am[q] != cm[q])
                    diff.append("memory +%X: %s != %s" % (off, cm[off & ~3:(off & ~3) + 8].hex(),
                                                          am[off & ~3:(off & ~3) + 8].hex()))
            if diff:
                bad += 1
                if len(first) < 5:
                    first.append({"in": {r: "%08X" % inp[7 * j + c] for c, r in enumerate(REGS)},
                                  "diff": diff})
                    if mem is not None and stride > 64:
                        first[-1]["mem"] = bytes(mem[stride * j:stride * (j + 1)]).hex()
        total += n
        k += n
        if verbose:
            print("  %s: %d / %d (%d differ)" % (name, k, todo, bad), flush=True)
    return {"name": name, "samples": total, "exhaustive": nex, "random": nrand, "differ": bad,
            "outputs": outs + (["CF"] if cf else []) + (["memory"] if spec["mem"] else []),
            "dropped": spec.get("dropped") or [],
            "first": first, "seconds": round(time.time() - t0, 1), "note": spec.get("note", "")}


def _task(names, sp):
    bench = Bench(cover=sp.get("cover", False))
    sps = specs()
    out = []
    try:
        for n in names:
            try:
                r = compare_func(n, sps[n], bench, sp.get("scale", 1.0),
                                 verbose=sp.get("verbose"))
            except Exception as e:          # noqa: BLE001  (a fault: the spec's inputs)
                r = {"name": n, "error": "%s: %s" % (type(e).__name__, e)}
            print("%-30s %9d samples (%d exhaustive) %s  %.0f s" % (
                n, r.get("samples", 0), r.get("exhaustive", 0),
                r.get("error") or ("all agree" if not r["differ"] else "%d DIFFER" % r["differ"]),
                r.get("seconds", 0)), flush=True)
            out.append(r)
        cov = bench.coverage()
        if cov is not None:
            import base64
            import zlib
            out.append({"name": "__coverage__",
                        "cov": base64.b64encode(zlib.compress(cov, 6)).decode()})
    finally:
        bench.close()
    return out


def save_coverage(res):
    """The workers' coverage maps, OR-ed into build/xn_canon/coverage/equiv.bin (xn_cover.py's
    folder: XN_COVER_OUT or the shared one; what is there stays, so runs of some specs add
    up). Returns the path, or None."""
    import base64
    import zlib
    maps = [zlib.decompress(base64.b64decode(r["cov"])) for r in res
            if r.get("name") == "__coverage__"]
    if not maps:
        return None
    acc = 0
    for m in maps:
        acc |= int.from_bytes(m, "little")
    d = os.environ.get("XN_COVER_OUT") or os.path.join(ROOT, "build", "xn_canon", "coverage")
    os.makedirs(d, exist_ok=True)
    p = os.path.join(d, "equiv.bin")
    if os.path.exists(p):           # a run of some specs adds to the map (remove it to restart)
        with open(p, "rb") as f:
            acc |= int.from_bytes(zlib.decompress(f.read()), "little")
    with open(p, "wb") as f:
        f.write(zlib.compress(acc.to_bytes(len(maps[0]), "little"), 6))
    return p


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("funcs", nargs="*")
    ap.add_argument("--scale", type=float, default=1.0, help="random samples times this")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("-j", "--jobs", type=int, default=None)
    ap.add_argument("-v", action="store_true")
    ap.add_argument("--cover", action="store_true",
                    help="the asm's coverage to build/xn_canon/coverage/equiv.bin")
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
    # the workers take contiguous runs of the names: deal them out by their cost, so each run
    # has a like share of the big ones
    jobs = max(1, a.jobs or 1)

    def cost(n):
        s = sps[n]
        return ((s["exhaustive"] or (0,))[0] + s["n"] * a.scale) * (
            4 * max(1, (s.get("stride") or 64) // 64) if s["mem"] else 1)
    by_cost = sorted(names, key=cost, reverse=True)
    names = [n for k in range(jobs) for n in by_cost[k::jobs]]
    res = xn_cload.parallel("xn_equiv:_task", names, {"scale": a.scale, "verbose": a.v,
                                                       "cover": a.cover}, a.jobs)
    out = [r for rs in res for r in rs]
    covp = save_coverage(out)
    out = [r for r in out if r.get("name") != "__coverage__"]
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
    nex = sum(r.get("exhaustive", 0) for r in out)
    nb = sum(1 for r in out if r.get("differ"))
    errs = [r for r in out if r.get("error")]
    missing = sorted(set(names) - {r["name"] for r in out})
    print("%d specs, %d samples (%d exhaustive), %d with differences, in %.0f s -> %s" % (
        len(out), tot, nex, nb, time.time() - t0, os.path.relpath(p, ROOT)))
    if covp:
        print("the asm's coverage -> %s" % os.path.relpath(covp, ROOT))
    for r in out:
        if r.get("differ"):
            print("  %s: %d differ; first %s" % (r["name"], r["differ"], r["first"][:2]))
    for r in errs:
        print("  %s: %s" % (r["name"], r["error"]))
    if missing:
        print("  not run (a worker failed): %s" % " ".join(missing))
    return 1 if nb or errs or missing else 0


if __name__ == "__main__":
    sys.exit(main())
