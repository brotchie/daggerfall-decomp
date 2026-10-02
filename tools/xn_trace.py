#!/usr/bin/env python3
"""Watch XnGine while the game runs headless (tools/fallemu.py).

- Coverage: how often each function in config/xngine_functions.csv runs.
- Self-modification: every write that lands on XnGine code. A write to a named patch field
  (config/xngine_patches.csv) is expected; so is a write anywhere in a function whose opcodes
  are patched (the unrolled loops, where a ret is planted at a computed place). Anything else
  is reported with the instruction that made it: code the static census missed.

usage: xn_trace.py --load SNAPSHOT [--ticks N] [--script ...] [--out FILE]
"""
import argparse
import bisect
import collections
import csv
import json
import os
import sys
import time

from unicorn import UC_HOOK_BLOCK, UC_HOOK_CODE, UC_HOOK_MEM_WRITE

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402
import xn_disasm  # noqa: E402
import xn_link  # noqa: E402

ROOT = fallemu.ROOT
LOAD = fallemu.LOAD


class Tracer:
    def __init__(self, emu):
        self.emu = emu
        an = xn_disasm.Analysis(xn_link.Image())
        self.lo, self.hi = an.lo, an.hi
        # code bytes of object 2, as a mask over the object
        self.code = bytearray(self.hi - self.lo)
        for a, i in an.insns.items():
            for k in range(i.size):
                self.code[a + k - self.lo] = 1
        self.funcs = an.funcs
        with open(os.path.join(ROOT, "config", "xngine_patches.csv"), newline="") as f:
            rows = list(csv.DictReader(f))
        self.fields = {}            # byte va -> field va
        self.opcode_funcs = set()
        for r in rows:
            fva = int(r["field"], 16)
            if r["kind"] == "opcode":
                self.opcode_funcs.add(int(r["func"], 16))
                continue
            for k in range((len(r["original"]) - 2) // 2):
                self.fields[fva + k] = fva
        self.calls = collections.Counter()
        self.field_writes = collections.Counter()
        self.opcode_writes = collections.Counter()
        self.unknown = collections.Counter()    # (writer, target) -> count
        self.code_starts = set(an.insns)
        self.blocks = set()         # executed basic blocks in object 2
        uc = emu.uc
        uc.hook_add(UC_HOOK_MEM_WRITE, self.on_write, None, LOAD + self.lo, LOAD + self.hi - 1)
        uc.hook_add(UC_HOOK_BLOCK, self.on_block, None, LOAD + self.lo, LOAD + self.hi - 1)
        for f in self.funcs:
            uc.hook_add(UC_HOOK_CODE, self.on_call, None, LOAD + f, LOAD + f)

    def func_of(self, va):
        k = bisect.bisect_right(self.funcs, va) - 1
        return self.funcs[k] if k >= 0 else None

    def on_block(self, uc, address, size, _):
        self.blocks.add(address - LOAD)

    def seeds(self):
        """Code the static walk missed: executed blocks that start outside the code map, and
        interrupt and exception handlers the game registered in object 2."""
        out = {}
        for a in self.blocks:
            if a not in self.code_starts:
                out[a] = "executed"
        for kind, table in (("interrupt handler", self.emu.pm_vec),
                            ("exception handler", self.emu.exc)):
            for _n, (_sel, off) in table.items():
                a = off - LOAD
                if self.lo <= a < self.hi and a not in self.code_starts:
                    out[a] = kind
        return out

    def on_call(self, uc, address, size, _):
        self.calls[address - LOAD] += 1

    def on_write(self, uc, access, address, size, value, _):
        va = address - LOAD
        if not any(self.code[va - self.lo + k] for k in range(size) if va + k < self.hi):
            return                  # engine data, not code
        if va in self.fields:
            self.field_writes[self.fields[va]] += 1
            return
        f = self.func_of(va)
        if f in self.opcode_funcs:
            self.opcode_writes[f] += 1
            return
        writer = self.emu.r("eip") - LOAD
        self.unknown[(writer, va)] += 1

    def report(self):
        ran = [f for f in self.funcs if self.calls[f]]
        out = {
            "functions": len(self.funcs),
            "functions_run": len(ran),
            "calls": {"0x%08X" % f: n for f, n in sorted(self.calls.items())},
            "patch_field_writes": {"0x%08X" % f: n for f, n in sorted(self.field_writes.items())},
            "opcode_writes": {"0x%08X" % f: n for f, n in sorted(self.opcode_writes.items())},
            "unknown_code_writes": [
                {"writer": "0x%08X" % w, "target": "0x%08X" % t, "count": n}
                for (w, t), n in sorted(self.unknown.items())],
        }
        return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--load", required=True)
    ap.add_argument("--ticks", type=int, default=500)
    ap.add_argument("--script", default="")
    ap.add_argument("--shots", default=os.path.join(ROOT, "build", "emu"))
    ap.add_argument("--save")
    ap.add_argument("--out", default=os.path.join(ROOT, "build", "xngine", "trace.json"))
    a = ap.parse_args()
    emu = fallemu.Emu.load(a.load, overlay=os.path.join(ROOT, "build", "emu", "overlay_trace"))
    tr = Tracer(emu)
    t0 = time.time()
    start = emu.ticks
    script = [(start + t, act, arg) for t, act, arg in fallemu.parse_script(a.script)]
    emu.run(start + a.ticks, script, a.shots)
    dt = time.time() - t0
    if a.save:
        emu.save(a.save)
    rep = tr.report()
    # what the static walk missed, for tools/xn_disasm.py (merged with earlier runs)
    seeds_path = os.path.join(ROOT, "config", "xngine_seeds.csv")
    seeds = {}
    if os.path.exists(seeds_path):
        with open(seeds_path, newline="") as f:
            seeds = {int(r["va"], 16): r["source"] for r in csv.DictReader(f)}
    new = {va: k for va, k in tr.seeds().items() if va not in seeds}
    seeds.update(new)
    if new:
        with open(seeds_path, "w", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            w.writerow(["va", "source"])
            for va in sorted(seeds):
                w.writerow(["0x%08X" % va, seeds[va]])
    print("%d new code seeds for xn_disasm.py (%d in all)" % (len(new), len(seeds)))
    os.makedirs(os.path.dirname(a.out), exist_ok=True)
    with open(a.out, "w") as f:
        json.dump(rep, f, indent=1)
    print("%d ticks in %.0f s; %d / %d functions ran; %d patch-field writes, %d opcode writes, "
          "%d unknown writes into code" % (
              a.ticks, dt, rep["functions_run"], rep["functions"],
              sum(rep["patch_field_writes"].values()), sum(rep["opcode_writes"].values()),
              len(rep["unknown_code_writes"])))
    for u in rep["unknown_code_writes"][:20]:
        print("  unknown: %(writer)s writes %(target)s (%(count)d times)" % u)


if __name__ == "__main__":
    main()
