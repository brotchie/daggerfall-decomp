#!/usr/bin/env python3
"""Record XnGine calls from the running game, and replay them in isolation.

A record is everything one call depends on and everything it does:
  - the registers and descriptors at entry,
  - every byte it read before writing it (its input), anywhere in memory,
  - the bytes of XnGine code that differ from FALL.EXE at entry (the self-modified state),
  - the bytes it wrote (their final values), the registers at return, and its port I/O and
    interrupts in order.
Interrupts are held off while a call is recorded, so it runs as one piece.

Replay loads FALL.EXE fresh (tools/fallemu.py), puts back the input bytes, code state and
registers, runs from the entry to the return, and compares writes, registers and I/O with the
record. The asm must replay itself exactly; later the C versions must too.

usage: xn_record.py record --load SNAPSHOT [--ticks N] [--script ...] [--per 2] [--out DIR]
       xn_record.py replay [DIR]
"""
import argparse
import bisect
import collections
import csv
import glob
import os
import pickle
import struct
import sys
import time

from unicorn import UC_HOOK_CODE, UC_HOOK_MEM_READ, UC_HOOK_MEM_WRITE

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402

ROOT = fallemu.ROOT
LOAD = fallemu.LOAD
OUT = os.path.join(ROOT, "build", "xngine", "records")
REGS = ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp", "eflags",
        "cs", "ds", "es", "fs", "gs", "ss")
EXIT_REGS = ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp")
FLAGS = 0x8D5           # CF PF AF ZF SF OF: what a caller may test after the call
MAX_INSNS = 2_000_000       # longer calls are noted, not recorded


def xngine_functions():
    with open(os.path.join(ROOT, "config", "xngine_functions.csv"), newline="") as f:
        return [int(r["va"], 16) for r in csv.DictReader(f)]


OBJ2 = (0xC0000, 0x161568)


def pristine_obj2():
    """Object 2 as FALL.EXE loads it (relocated to LOAD), to diff the live code against."""
    emu = fallemu.Emu.__new__(fallemu.Emu)
    le = fallemu.LE(fallemu.EXE)
    img = le.load(relocate=True)
    o = le.objs[1]
    buf = img[2]
    for f in le.fixups():
        if o.base <= f.src_va < o.base + o.vsize:
            k = f.src_va - o.base
            if f.kind == fallemu.SRC_OFF32:
                v = struct.unpack_from("<I", buf, k)[0]
                struct.pack_into("<I", buf, k, (v + LOAD) & 0xFFFFFFFF)
            elif f.kind == fallemu.SRC_SEL16:
                struct.pack_into("<H", buf, k, fallemu.SEL_DATA)
    del emu
    return bytes(buf)


def call_once(emu, entry_regs=None):
    """Run the call that starts at the current EIP to its return, with memory hooks that
    collect its input bytes and writes. Returns the record body."""
    uc = emu.uc
    esp0 = emu.r("esp")
    ret = struct.unpack("<I", emu.read(esp0, 4))[0]
    reads, written = {}, set()
    done = []

    def on_read(uc_, access, address, size, value, _):
        for a in range(address, address + size):
            if a not in written and a not in reads:
                reads[a] = None          # filled in below, before the call changes it

    def on_write(uc_, access, address, size, value, _):
        for a in range(address, address + size):
            if a not in written and a not in reads:
                reads.setdefault(a, None)
            written.add(a)

    def on_ret(uc_, address, size, _):
        if uc_.reg_read(fallemu.R["esp"]) > esp0:
            done.append(True)
            uc_.emu_stop()

    # the input bytes must be the values before the call; read hooks fire before the access,
    # so take them from memory as the hook sees it
    def on_read_value(uc_, access, address, size, value, _):
        for a in range(address, address + size):
            if a not in written and reads.get(a, 0) is None:
                reads[a] = bytes(uc_.mem_read(a, 1))[0]

    def on_write_value(uc_, access, address, size, value, _):
        for a in range(address, address + size):
            if reads.get(a, 0) is None:
                reads[a] = bytes(uc_.mem_read(a, 1))[0]

    hooks = [uc.hook_add(UC_HOOK_MEM_READ, on_read), uc.hook_add(UC_HOOK_MEM_READ, on_read_value),
             uc.hook_add(UC_HOOK_MEM_WRITE, on_write_value),
             uc.hook_add(UC_HOOK_MEM_WRITE, on_write),
             uc.hook_add(UC_HOOK_CODE, on_ret, None, ret, ret)]
    uc.ctl_flush_tb()       # code already translated would not call hooks added after it
    emu.io_log = []
    n = 0
    try:
        while not done and n < MAX_INSNS:
            uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=1_000_000)
            n += 1_000_000
    finally:
        for h in hooks:
            uc.hook_del(h)
        uc.ctl_flush_tb()
        io, emu.io_log = emu.io_log, None
    writes = {a: emu.read(a, 1)[0] for a in sorted(written)}
    return {
        "returned": bool(done),
        "reads": {a: v for a, v in reads.items() if v is not None},
        "writes": writes,
        "exit": {r: emu.r(r) for r in EXIT_REGS + ("eflags",)},
        "io": io,
    }


class Recorder:
    def __init__(self, emu, per=2, only=None):
        self.emu, self.per = emu, per
        self.funcs = only or xngine_functions()
        self.count = collections.Counter()
        self.records = []
        self.long = set()
        self.recording = False
        self.pending = None
        self.pristine = pristine_obj2()
        emu.write(self.HALT, b"\xf4")
        self.hooks = {}
        for f in self.funcs:
            self.hooks[f] = emu.uc.hook_add(UC_HOOK_CODE, self.on_entry, None, LOAD + f, LOAD + f)
        emu.on_stop = self.after_slice

    HALT = 0x1F00       # a hlt in low memory: entry hooks divert here to stop the CPU

    def on_entry(self, uc, address, size, _):
        f = address - LOAD
        if self.pending is None and not self.recording and self.count[f] < self.per:
            # stop before the function's first instruction runs: a hook that moves EIP
            # skips the instruction it was called for
            self.pending = (f, address)
            uc.reg_write(fallemu.R["eip"], self.HALT)

    def after_slice(self):
        if self.pending is None:
            return False
        (f, address), self.pending = self.pending, None
        emu = self.emu
        emu.w("eip", address)
        self.count[f] += 1
        if self.count[f] >= self.per:
            emu.uc.hook_del(self.hooks.pop(f))
        live = emu.read(LOAD + OBJ2[0], OBJ2[1] - OBJ2[0])
        smc = {OBJ2[0] + k: live[k] for k in range(len(live)) if live[k] != self.pristine[k]}
        rec = {"func": f, "entry": {r: emu.r(r) for r in REGS}, "eip": emu.r("eip"),
               "sels": {s: tuple(v) for s, v in emu.sels.items()}, "smc": smc,
               "tick": emu.ticks}
        t0 = time.time()
        self.recording = True
        try:
            rec.update(call_once(emu))
        finally:
            self.recording = False
        rec["seconds"] = time.time() - t0
        if rec["returned"]:
            self.records.append(rec)
        else:
            self.long.add(f)    # ran past MAX_INSNS (a main loop, a wait): not a unit
        return True


def load_records(d):
    out = []
    for p in sorted(glob.glob(os.path.join(d, "*.pkl"))):
        with open(p, "rb") as f:
            out += pickle.load(f)
    return out


def replay(rec, base=None, patch=None):
    """Run one record again on a fresh machine. `patch(emu)` may replace the function's code
    first (a C version). Returns a list of differences (empty: it matched)."""
    emu = base or fallemu.Emu(overlay=os.path.join(ROOT, "build", "emu", "overlay_replay"))
    for s, (b, lim, acc) in rec["sels"].items():
        emu.setsel(s, b, lim, acc)
    for a, v in rec["smc"].items():
        emu.write(LOAD + a, bytes([v]))
    for a, v in rec["reads"].items():
        emu.write(a, bytes([v]))
    for r, v in rec["entry"].items():
        emu.w(r, v)
    emu.w("eip", rec["eip"])
    if patch:
        patch(emu)
    # services (DOS, BIOS, the mouse) answer as they did in the recording
    emu.int_replay = [(e[1], e[2]) for e in rec["io"] if e[0] == "int-ret"]
    got = call_once(emu)
    emu.int_replay = None
    diffs = []
    if not got["returned"]:
        diffs.append("did not return")
    for r in EXIT_REGS:
        if got["exit"][r] != rec["exit"][r]:
            diffs.append("%s %08X != %08X" % (r, got["exit"][r], rec["exit"][r]))
    if (got["exit"]["eflags"] ^ rec["exit"]["eflags"]) & FLAGS:
        diffs.append("flags %03X != %03X" % (got["exit"]["eflags"] & FLAGS,
                                              rec["exit"]["eflags"] & FLAGS))
    if got["writes"] != rec["writes"]:
        extra = set(got["writes"]) ^ set(rec["writes"])
        wrong = [a for a in set(got["writes"]) & set(rec["writes"])
                 if got["writes"][a] != rec["writes"][a]]
        diffs.append("writes: %d addresses differ in the set, %d in value%s" % (
            len(extra), len(wrong), " (first %#x)" % min(extra | set(wrong))
            if extra or wrong else ""))
    if got["io"] != rec["io"]:
        diffs.append("port I/O or interrupts differ")
    return diffs


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("record")
    r.add_argument("--load", required=True)
    r.add_argument("--ticks", type=int, default=300)
    r.add_argument("--script", default="")
    r.add_argument("--per", type=int, default=2, help="calls to record per function")
    r.add_argument("--out", default=OUT)
    p = sub.add_parser("replay")
    p.add_argument("dir", nargs="?", default=OUT)
    a = ap.parse_args()
    if a.cmd == "record":
        emu = fallemu.Emu.load(a.load, overlay=os.path.join(ROOT, "build", "emu", "overlay_record"))
        rec = Recorder(emu, a.per)
        start = emu.ticks
        t0 = time.time()
        emu.run(start + a.ticks,
                [(start + t, x, g) for t, x, g in fallemu.parse_script(a.script)])
        os.makedirs(a.out, exist_ok=True)
        name = os.path.splitext(os.path.basename(a.load))[0]
        with open(os.path.join(a.out, name + ".pkl"), "wb") as f:
            pickle.dump(rec.records, f)
        funcs = {x["func"] for x in rec.records}
        print("%d calls of %d functions recorded in %.0f s -> %s; %d ran too long: %s" % (
            len(rec.records), len(funcs), time.time() - t0, a.out, len(rec.long),
            " ".join("%06X" % f for f in sorted(rec.long))))
        return 0
    recs = load_records(a.dir)
    ok = bad = 0
    for rec in recs:
        diffs = replay(rec)
        if diffs:
            bad += 1
            print("func_%08X (tick %d): %s" % (rec["func"], rec["tick"], "; ".join(diffs[:3])))
        else:
            ok += 1
    print("%d / %d records replay exactly" % (ok, ok + bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
