#!/usr/bin/env python3
"""Record XnGine calls from the running game, and replay them in isolation.

A record is one call's exact entry state and everything it does:
  - its input: the 4 KB pages of memory that differ from the snapshot the recording started
    from (that snapshot is the replay's base), plus registers, descriptors and the game's
    exception handlers,
  - its effect: every byte that differs after the call (memory before and after, compared),
    the registers at return, and its port I/O and interrupts, with what each service returned.
Interrupts are held off while a call is recorded, so it runs as one piece. Read hooks also
note which bytes it read before writing them (a footprint of its inputs; Unicorn misses some
reads inside self-modified blocks, so replay does not depend on it).

Replay restores the base snapshot and the record's pages, runs from the entry to the return
with services and port reads answered as recorded, and compares. The asm must replay itself
exactly; later the C versions must too.

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
import zlib

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
    """Function entries to record: not the blocks seen only at run time (xngine_seeds.csv),
    which are mostly places inside functions that the static walk missed."""
    with open(os.path.join(ROOT, "config", "xngine_functions.csv"), newline="") as f:
        return [int(r["va"], 16) for r in csv.DictReader(f) if r["found_by"] != "run time"]


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


PAGE = 4096


def memory(emu):
    """(low memory, program memory) as bytes."""
    return emu.read(0, fallemu.LOW), emu.read(LOAD, fallemu.MEM)


def regions(mem):
    return ((0, mem[0]), (LOAD, mem[1]))


def changed_pages(mem, base):
    """{linear address: page bytes} where mem differs from base."""
    out = {}
    for (origin, cur), (_o, ref) in zip(regions(mem), regions(base)):
        a, b = memoryview(cur), memoryview(ref)
        for k in range(0, len(cur), PAGE):
            if a[k:k + PAGE] != b[k:k + PAGE]:
                out[origin + k] = bytes(a[k:k + PAGE])
    return out


def changed_bytes(before, after):
    """{linear address: new byte} for every byte that differs."""
    out = {}
    for page, data in changed_pages(after, before).items():
        origin, buf = (0, before[0]) if page < LOAD else (LOAD, before[1])
        old = buf[page - origin:page - origin + PAGE]
        for j in range(len(data)):
            if data[j] != old[j]:
                out[page + j] = data[j]
    return out


def call_once(emu, entry_regs=None):
    """Run the call that starts at the current EIP to its return, with memory hooks that
    collect its input bytes and writes. Returns the record body."""
    uc = emu.uc
    esp0 = emu.r("esp")
    ret = struct.unpack("<I", emu.read(esp0, 4))[0]
    reads, written = {}, set()
    done = []

    # one hook each: Unicorn calls hooks of a kind newest-first, so a pair of read hooks
    # (mark, then fill in) loses bytes read only once. Read hooks run before the access, so
    # memory still holds the value the instruction is about to read.
    def on_read(uc_, access, address, size, value, _):
        for a in range(address, address + size):
            if a not in written and a not in reads:
                reads[a] = bytes(uc_.mem_read(a, 1))[0]

    def on_write(uc_, access, address, size, value, _):
        for a in range(address, address + size):
            written.add(a)

    def on_ret(uc_, address, size, _):
        if uc_.reg_read(fallemu.R["esp"]) > esp0:
            done.append(True)
            uc_.emu_stop()

    before = memory(emu)
    hooks = [uc.hook_add(UC_HOOK_MEM_READ, on_read), uc.hook_add(UC_HOOK_MEM_WRITE, on_write),
             uc.hook_add(UC_HOOK_CODE, on_ret, None, ret, ret)]
    fallemu.flush_caches(uc)    # translated code and the TLB would skip new hooks
    emu.io_log = []
    n = 0
    try:
        while not done and n < MAX_INSNS:
            uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=1_000_000)
            if emu.clear_exception_state():     # a fault the game's handler took
                continue
            n += 1_000_000
    finally:
        for h in hooks:
            uc.hook_del(h)
        fallemu.flush_caches(uc)
        io, emu.io_log = emu.io_log, None
    writes = changed_bytes(before, memory(emu))
    return {
        "returned": bool(done),
        "reads": reads,
        "writes": writes,
        "exit": {r: emu.r(r) for r in EXIT_REGS + ("eflags",)},
        "io": io,
    }


class Recorder:
    def __init__(self, emu, per=2, only=None, base=None):
        self.emu, self.per = emu, per
        self.base_path = base            # the snapshot emu was loaded from
        self.base = memory(emu)
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
        pages = changed_pages(memory(emu), self.base)
        rec = {"func": f, "entry": {r: emu.r(r) for r in REGS}, "eip": emu.r("eip"),
               "sels": {s: tuple(v) for s, v in emu.sels.items()}, "smc": smc,
               "exc": dict(emu.exc), "base": self.base_path,
               "pages": zlib.compress(pickle.dumps(pages), 1),
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


_bases = {}


def base_machine(path):
    """The snapshot a record was taken from, loaded once: (machine, its memory)."""
    if path not in _bases:
        emu = fallemu.Emu.load(path, overlay=os.path.join(ROOT, "build", "emu", "overlay_replay"))
        _bases.clear()
        _bases[path] = (emu, memory(emu))
    return _bases[path]


def replay(rec, patch=None):
    """Run one record again from its exact entry state. `patch(emu)` may replace the
    function's code first (a C version). Returns a list of differences (empty: it matched)."""
    emu, (low, mem) = base_machine(rec["base"])
    emu.write(0, low)
    emu.write(LOAD, mem)
    for a, page in pickle.loads(zlib.decompress(rec["pages"])).items():
        emu.write(a, page)
    emu.sels = {}
    for s, (b, lim, acc) in rec["sels"].items():
        emu.setsel(s, b, lim, acc)
    emu.exc = dict(rec.get("exc", {}))
    for r, v in rec["entry"].items():
        emu.w(r, v)
    emu.w("eip", rec["eip"])
    if patch:
        patch(emu)
    # services (DOS, BIOS, the mouse) answer as they did in the recording
    emu.int_replay = [(e[1], e[2]) for e in rec["io"] if e[0] == "int-ret"]
    emu.in_replay = [e[2] for e in rec["io"] if e[0] == "in"]   # and ports read as they were
    got = call_once(emu)
    emu.int_replay = emu.in_replay = None
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
        rec = Recorder(emu, a.per, base=os.path.abspath(a.load))
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
