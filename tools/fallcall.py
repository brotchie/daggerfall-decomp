#!/usr/bin/env python3
"""Call any FALL.EXE function directly, in a live game, and see what it does.

From a snapshot, the game runs on to a safe point: the entry of its per-frame update
(func_0001025B, which main's loop calls once a frame, with no arguments and a clean stack).
There the CPU is saved, the function is called with the given arguments (Watcom's register
convention: EAX, EDX, EBX, ECX, then the stack; or all on the stack for functions declared
`#pragma aux ... parm routine []`), and the call runs, timer interrupts and DOS calls and
all, until it returns to a trap address. The call's effects are reported:
  - the value it returned (EAX);
  - the functions that ran (coverage);
  - the files it opened;
  - the globals and player-record fields it changed (named from config/names.csv);
  - optionally, the screen a few frames later.
The CPU is then put back, so the game carries on from the safe point with the call's effects
in memory.

Arguments are numbers (0x.. or decimal) or:
  @player   the player object (*D_00195AA4)    @char      the player character (*D_00195BE0)
  @entity   the player's creature entity (*D_00195AA0; the character record is at +0x47)
  @str:TEXT a C string in scratch memory        @buf:N     N zeroed bytes of scratch memory

usage:
  fallcall.py SNAP FUNC [ARG ...] [--stack] [--ticks N] [--after N] [--shot PNG] [--save SNAP2]
      SNAP: a name in build/emu/snap or a path; FUNC: 0xADDR, func_XXXXXXXX or a name in
      config/names.csv. --ticks: give up after N timer ticks (default 2000); --after: run
      N ticks after the call before the screenshot (default 60); --save: snapshot after.
  fallcall.py sweep SNAP [--funcs unrun|UNIT.c|0xA,0xB] [--policy zero|entity] [--ticks N] [-j N]
      call each function from a fresh game at the safe point (arguments 0, or the player's
      entity first): build/call/sweep_*.jsonl (returned?, functions run, files, globals
      changed), build/call/shots/ADDR.png when it drew on the screen, build/cov/call_*.cov
  fallcall.py report
      what the sweep found: coverage new to the evidence table, screens, files per function
"""
import argparse
import csv
import ctypes
import glob
import json
import os
import re
import struct
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402  (first: it picks the Unicorn build)
import fallcov  # noqa: E402
import names as namesmod  # noqa: E402
from unicorn import UcError  # noqa: E402

ROOT = fallemu.ROOT
L = fallemu.LOAD
SAFE = 0x1025B                  # the per-frame update main() calls once a loop
TRAP = 0xBBF00                  # page padding after object 1's code: never runs
SCRATCH = 0x3F00000             # the last MB of program memory, for @str/@buf
O3, O3_END = 0x170000, 0x1B5520


def run_until(emu, addr, max_ticks):
    """emu.run, stopping as soon as EIP reaches LOAD + addr. True if it got there."""
    target = L + addr
    end = emu.ticks + max_ticks
    while emu.ticks < end and emu.exit_code is None:
        try:
            emu.uc.emu_start(emu.r("eip"), target, count=fallemu.TICK)
            while emu.r("eip") != target and emu.clear_exception_state():
                emu.uc.emu_start(emu.r("eip"), target, count=fallemu.TICK)
        except UcError as e:
            print("CPU error: %s at %08X" % (e, emu.r("eip") - L))
            return False
        except fallemu.Stop:
            return False
        if emu.r("eip") == target:
            return True
        emu.insns += fallemu.TICK
        if not emu.r("eflags") & 0x200:
            continue
        emu.ticks += 1
        emu.pit_reads = 0
        emu.irq(8)
    return False


def resolve(func):
    if re.fullmatch(r"(0x)?[0-9A-Fa-f]{5,8}", func):
        return int(func, 16)
    m = re.fullmatch(r"func_([0-9A-F]{8})", func)
    if m:
        return int(m.group(1), 16)
    for r in namesmod.load():
        if r["kind"] == "func" and r["name"] == func:
            return int(r["address"], 16)
    raise SystemExit("no function %s" % func)


def stack_convention(va):
    """True if the C declares the function `parm routine []` (all arguments on the stack)."""
    pat = re.compile(r"#pragma aux func_%08X parm routine \[\]" % va)
    for p in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        with open(p, errors="replace") as f:
            if pat.search(f.read()):
                return True
    return False


def call(emu, va, args, stack=False, max_ticks=2000):
    """Call function va with args (ints) at the current point; the CPU is put back after.
    Returns (EAX or None if it did not return, ticks it took)."""
    ctx = emu.uc.context_save()
    t0 = emu.ticks
    esp = (emu.r("esp") - 0x400) & ~3              # well below the live frame
    regs = [] if stack else list(args[:4])
    onstack = list(args) if stack else list(args[4:])
    for v in reversed(onstack):
        esp -= 4
        emu.uc.mem_write(esp, struct.pack("<I", v & 0xFFFFFFFF))
    esp -= 4
    emu.uc.mem_write(esp, struct.pack("<I", L + TRAP))
    emu.w("esp", esp)
    for reg, v in zip(("eax", "edx", "ebx", "ecx"), regs):
        emu.w(reg, v & 0xFFFFFFFF)
    emu.w("eip", L + va)
    ok = run_until(emu, TRAP, max_ticks)
    ret = emu.r("eax") if ok else None
    took = emu.ticks - t0
    emu.uc.context_restore(ctx)
    return ret, took


class Scratch:
    def __init__(self, emu):
        self.emu, self.at = emu, SCRATCH

    def put(self, data):
        a = self.at
        self.emu.uc.mem_write(L + a, data)
        self.at += (len(data) + 15) & ~15
        return L + a


def arg_value(emu, s, scratch):
    if s == "@player":
        return struct.unpack("<I", bytes(emu.uc.mem_read(L + 0x195AA4, 4)))[0]
    if s == "@entity":
        return struct.unpack("<I", bytes(emu.uc.mem_read(L + 0x195AA0, 4)))[0]
    if s == "@char":
        return struct.unpack("<I", bytes(emu.uc.mem_read(L + 0x195BE0, 4)))[0]
    if s.startswith("@str:"):
        return scratch.put(s[5:].encode("latin-1") + b"\0")
    if s.startswith("@buf:"):
        return scratch.put(bytes(int(s[5:], 0)))
    return int(s, 0)


def changed_words(before, after, lo):
    """[(address, old, new)] of the dwords that differ."""
    out = []
    for a in range(0, len(before) - 3, 4):
        if before[a:a + 4] != after[a:a + 4]:
            out.append((lo + a, struct.unpack_from("<I", before, a)[0], struct.unpack_from("<I", after, a)[0]))
    return out


# ---- sweep: call many functions, one fresh game each --------------------------------------
CALL = os.path.join(ROOT, "build", "call")


def prototypes():
    """{va: (number of arguments, all on the stack)} from the C definitions and pragmas."""
    out, stackp = {}, set()
    defn = re.compile(r"^[A-Za-z_][\w \*]*?\bfunc_([0-9A-F]{8})\s*\(([^;]*)\)\s*$")
    for p in glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True):
        for line in open(p, errors="replace"):
            m = re.match(r"#pragma aux func_([0-9A-F]{8}) parm routine \[\]", line)
            if m:
                stackp.add(int(m.group(1), 16))
                continue
            m = defn.match(line)
            if m and not line.startswith("extern"):
                ps = m.group(2).strip()
                n = 0 if ps in ("", "void") else len([x for x in ps.split(",") if x.strip() != "..."])
                out[int(m.group(1), 16)] = (n, "..." in ps)
    return {va: (n, var or va in stackp) for va, (n, var) in out.items()}


def safe_snapshot(snap):
    """A snapshot of the game standing at the safe point (made once, then reused)."""
    base = os.path.basename(snap)[:-5]
    path = os.path.join(CALL, "safe_%s.snap" % base)
    if not os.path.exists(path):
        os.makedirs(CALL, exist_ok=True)
        emu = fallemu.Emu.load(snap, overlay=os.path.join(CALL, "ov_safe"))
        if not run_until(emu, SAFE, 3000):
            raise SystemExit("%s did not reach the frame loop" % snap)
        emu.save(path + ".tmp")
        os.replace(path + ".tmp", path)
        emu.close()
    return path


def probe(safe, va, args, stack, max_ticks, shot_dir, fvas, lib):
    """One call from a fresh game at the safe point: what it did."""
    import fallplay
    emu = fallemu.Emu.load(safe, overlay=os.path.join(CALL, "ov_%d" % os.getpid()))
    try:
        stack_lo = emu.r("esp") - L - 0x8000
        o3 = bytes(emu.uc.mem_read(L + O3, O3_END - O3))
        vga0 = bytes(emu.uc.mem_read(0xA0000, 64000))
        nfiles = len(emu.files.log)
        lib.uc_dagger_coverage(emu.uc._uch, L + fallcov.LO, L + fallcov.HI, None)
        ret, took = call(emu, va, args, stack, max_ticks)
        buf = ctypes.create_string_buffer(fallcov.HI - fallcov.LO)
        lib.uc_dagger_coverage(emu.uc._uch, L + fallcov.LO, L + fallcov.HI, buf)
        o3b = bytes(emu.uc.mem_read(L + O3, O3_END - O3))
        vga1 = bytes(emu.uc.mem_read(0xA0000, 64000))
        drawn = sum(1 for i in range(0, 64000, 4) if vga0[i] != vga1[i]) * 4
        rec = {"va": "%X" % va, "args": args, "stack": stack, "returned": ret is not None,
               "ret": ret, "ticks": took, "exit": emu.exit_code,
               "ran": ["%X" % v for v in fvas if buf.raw[v - fallcov.LO]],
               "files": [e[1] for e in emu.files.log[nfiles:nfiles + 30] if len(e) > 1],
               "globals": ["%X" % a for a, _o, _n in changed_words(o3, o3b, O3) if a < stack_lo][:40],
               "drawn": drawn}
        if drawn > 3000 and emu.mode == 0x13:
            rec["shot"] = os.path.join(shot_dir, "%X.png" % va)
            fallplay.shot(emu, rec["shot"])
        rec["cov"] = buf.raw
        return rec
    finally:
        emu.close()


def sweep_work(k, n, snap, targets, max_ticks, policy):
    from unicorn.unicorn_py3 import unicorn as ucmod
    lib = ucmod.uclib
    lib.uc_dagger_coverage.argtypes = [ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_void_p]
    protos = prototypes()
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        fvas = sorted(v for v in (int(r["va"], 16) for r in csv.DictReader(f)) if fallcov.LO <= v < fallcov.HI)
    safe = safe_snapshot(snap)
    shots = os.path.join(CALL, "shots")
    os.makedirs(shots, exist_ok=True)
    part = os.path.join(CALL, "sweep_%d.jsonl" % k)
    done = set()
    if os.path.exists(part):
        done = {json.loads(line)["va"] for line in open(part)}
    out = open(part, "a")
    acc = 0
    emu0 = fallemu.Emu.load(safe, overlay=os.path.join(CALL, "ov_%d" % os.getpid()))
    entity = struct.unpack("<I", bytes(emu0.uc.mem_read(L + 0x195AA0, 4)))[0]
    emu0.close()
    for i, va in enumerate(targets):
        if i % n != k or "%X" % va in done:
            continue
        if fallemu.rss_mb() > 2000:
            sys.exit(3)
        nargs, stack = protos.get(va, (0, False))
        args = [0] * nargs
        if policy == "entity" and nargs:
            args[0] = entity
        try:
            rec = probe(safe, va, args, stack, max_ticks, shots, fvas, lib)
        except Exception as e:                      # noqa: BLE001  (a bad call: note it)
            rec = {"va": "%X" % va, "error": str(e)[:200]}
        cov = rec.pop("cov", None)
        if cov:
            acc |= int.from_bytes(cov, "little")
        out.write(json.dumps(rec) + "\n")
        out.flush()
    if acc:
        write = os.path.join(fallcov.COV, "call_%d.cov" % k)
        with open(write, "wb") as f:
            f.write(zlib.compress(json.dumps({"call": k}).encode() + b"\n" + acc.to_bytes(fallcov.HI - fallcov.LO, "little")))


def sweep(snap, targets, j, max_ticks, policy):
    import memwatch
    import subprocess
    memwatch.start()
    os.makedirs(CALL, exist_ok=True)
    safe_snapshot(snap)
    tfile = os.path.join(CALL, "targets.txt")
    with open(tfile, "w") as f:
        f.write(" ".join("%X" % v for v in targets))
    j = fallemu.workers(j)

    def start(k):
        return subprocess.Popen([sys.executable, __file__, "sweep-work", str(k), str(j), snap, tfile,
                                 str(max_ticks), policy])
    procs = {k: start(k) for k in range(j)}
    while procs:
        for k, p in list(procs.items()):
            if p.wait() == 3:
                procs[k] = start(k)
            else:
                del procs[k]
    recs = [json.loads(line) for p in sorted(glob.glob(os.path.join(CALL, "sweep_*.jsonl"))) for line in open(p)]
    ok = sum(1 for r in recs if r.get("returned"))
    hung = sum(1 for r in recs if "returned" in r and not r["returned"])
    err = sum(1 for r in recs if "error" in r or r.get("exit") is not None)
    ran = set()
    for r in recs:
        ran |= set(r.get("ran", ()))
    print("%d calls: %d returned, %d did not return in %d ticks, %d failed; %d functions ran; %d screens in %s" % (
        len(recs), ok, hung, max_ticks, err, len(ran), sum(1 for r in recs if r.get("shot")),
        os.path.join(CALL, "shots")))


def report():
    """What the sweep found: coverage new to the evidence table, screens drawn, files read."""
    import collections
    ev = {int(r["va"], 16): r for r in csv.DictReader(open(os.path.join(ROOT, "build", "evidence", "functions.csv")))}
    recs = [json.loads(line) for p in sorted(glob.glob(os.path.join(CALL, "sweep_*.jsonl"))) for line in open(p)]
    names = namesmod.by_address()
    ran = set()
    for r in recs:
        ran |= {int(x, 16) for x in r.get("ran", ())}
    new = {v for v in ran if v in ev and ev[v]["episodes"] == "0"}
    game_new = {v for v in new if ev[v]["kind"] == "game"}
    print("%d calls: %d returned, %d did not return, %d failed or exited" % (
        len(recs), sum(1 for r in recs if r.get("returned")),
        sum(1 for r in recs if r.get("returned") is False), sum(1 for r in recs if "error" in r or r.get("exit") is not None)))
    print("functions never seen running before that ran in a call: %d (game %d)" % (len(new), len(game_new)))
    by = collections.Counter(ev[v]["unit"] for v in game_new)
    print("  by unit: " + ", ".join("%s %d" % kv for kv in by.most_common(20)))
    print("screens drawn by a call:")
    for r in recs:
        if r.get("shot"):
            print("  %-26s %s  files: %s" % (names.get("func_" + r["va"].zfill(8), "func_" + r["va"].zfill(8)),
                                             r["shot"], " ".join(os.path.basename(f.replace("\\", "/")) for f in r.get("files", [])[:6])))
    print("files opened by a call (asset -> code):")
    for r in recs:
        fs = sorted({os.path.basename(f.replace("\\", "/")).lower() for f in r.get("files", [])})
        if fs and not r.get("shot"):
            print("  %-26s %s" % (names.get("func_" + r["va"].zfill(8), "func_" + r["va"].zfill(8)), " ".join(fs[:8])))


def main():
    if len(sys.argv) > 1 and sys.argv[1] == "sweep-work":
        k, n, snap, tfile, max_ticks, policy = sys.argv[2:8]
        targets = [int(x, 16) for x in open(tfile).read().split()]
        sweep_work(int(k), int(n), snap, targets, int(max_ticks), policy)
        return
    if len(sys.argv) > 1 and sys.argv[1] == "report":
        report()
        return
    if len(sys.argv) > 1 and sys.argv[1] == "sweep":
        sp = argparse.ArgumentParser()
        sp.add_argument("cmd")
        sp.add_argument("snap")
        sp.add_argument("--funcs", default="unrun", help="unrun (never seen running), UNIT, or 0xA,0xB")
        sp.add_argument("--policy", default="zero", choices=["zero", "entity"],
                        help="arguments: all 0, or the player's entity first")
        sp.add_argument("--ticks", type=int, default=300)
        sp.add_argument("-j", type=int, default=None)
        a = sp.parse_args()
        snap = a.snap if a.snap.endswith(".snap") else os.path.join(ROOT, "build", "emu", "snap", a.snap + ".snap")
        ev = list(csv.DictReader(open(os.path.join(ROOT, "build", "evidence", "functions.csv"))))
        if a.funcs == "unrun":
            targets = [int(r["va"], 16) for r in ev if r["kind"] == "game" and r["episodes"] == "0"]
        elif a.funcs.endswith(".c"):
            targets = [int(r["va"], 16) for r in ev if r["unit"] == a.funcs]
        else:
            targets = [int(x, 16) for x in a.funcs.split(",")]
        sweep(snap, targets, a.j, a.ticks, a.policy)
        return
    ap = argparse.ArgumentParser()
    ap.add_argument("snap")
    ap.add_argument("func")
    ap.add_argument("args", nargs="*")
    ap.add_argument("--stack", action="store_true", help="all arguments on the stack")
    ap.add_argument("--ticks", type=int, default=2000)
    ap.add_argument("--after", type=int, default=60)
    ap.add_argument("--shot")
    ap.add_argument("--save")
    a = ap.parse_args()
    snap = a.snap if a.snap.endswith(".snap") else os.path.join(ROOT, "build", "emu", "snap", a.snap + ".snap")
    va = resolve(a.func)
    stack = a.stack or stack_convention(va)
    emu = fallemu.Emu.load(snap, overlay=os.path.join(ROOT, "build", "call", "ov_%d" % os.getpid()))
    if not run_until(emu, SAFE, 3000):
        raise SystemExit("the game did not reach its frame loop (func_%08X) in 3000 ticks" % SAFE)
    scratch = Scratch(emu)
    args = [arg_value(emu, s, scratch) for s in a.args]
    names = namesmod.by_address()
    po = struct.unpack("<I", bytes(emu.uc.mem_read(L + 0x195AA4, 4)))[0] - L
    o3 = bytes(emu.uc.mem_read(L + O3, O3_END - O3))
    stack_lo = emu.r("esp") - L - 0x8000            # object 3 ends in the stack
    rec = bytes(emu.uc.mem_read(L + po, 0x600))
    nfiles = len(emu.files.log)
    from unicorn.unicorn_py3 import unicorn as ucmod
    lib = ucmod.uclib
    lib.uc_dagger_coverage.argtypes = [ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64, ctypes.c_void_p]
    lib.uc_dagger_coverage(emu.uc._uch, L + fallcov.LO, L + fallcov.HI, None)
    ret, took = call(emu, va, args, stack, a.ticks)
    buf = ctypes.create_string_buffer(fallcov.HI - fallcov.LO)
    lib.uc_dagger_coverage(emu.uc._uch, L + fallcov.LO, L + fallcov.HI, buf)
    fname = names.get("func_%08X" % va, "func_%08X" % va)
    print("%s(%s)%s: %s after %d ticks" % (
        fname, ", ".join(a.args), " [stack]" if stack else "",
        "returned %d (0x%X)" % (ret if ret < 0x80000000 else ret - (1 << 32), ret) if ret is not None
        else "did not return", took))
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        fvas = sorted(int(r["va"], 16) for r in csv.DictReader(f))
    ran = [v for v in fvas if fallcov.LO <= v < fallcov.HI and buf.raw[v - fallcov.LO]]
    print("ran %d functions: %s" % (len(ran), " ".join(names.get("func_%08X" % v, "%X" % v) for v in ran[:40])
                                     + (" ..." if len(ran) > 40 else "")))
    for e in emu.files.log[nfiles:nfiles + 20]:
        print("file:", *e)
    o3b = bytes(emu.uc.mem_read(L + O3, O3_END - O3))
    ch = [c for c in changed_words(o3, o3b, O3) if c[0] < stack_lo]     # not the stack
    print("globals changed: %d" % len(ch))
    for addr, old, new in ch[:30]:
        print("  %-28s %08X -> %08X" % (names.get("D_%08X" % addr, "D_%08X" % addr), old, new))
    recb = bytes(emu.uc.mem_read(L + po, 0x600))
    fields = {int(r["address"].split("+")[1], 16) + (0x1CF if r["address"].startswith("character") else 0): r["name"]
              for r in namesmod.load() if r["kind"] == "field"}
    diffs = [o for o in range(0x600) if rec[o] != recb[o]]
    if diffs:
        print("player object bytes changed: %s" % " ".join(
            "+%X%s" % (o, "(" + fields[o] + ")" if o in fields else "") for o in diffs[:40]))
    if a.shot:
        emu.run(emu.ticks + a.after, [], None)
        import fallplay
        fallplay.shot(emu, a.shot)
        print("screen:", a.shot)
    if a.save:
        emu.save(a.save)
    emu.close()


if __name__ == "__main__":
    main()
