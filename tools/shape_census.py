#!/usr/bin/env python3
"""Compare instruction shapes (mnemonic + operand kinds/sizes) between our compiled output
(build/lift/<func>.bin from tools/lift_all.py) and FALL.EXE over the functions that don't
match yet. Shapes much more common on one side point at a code generation habit of
Watcom 10 that the compiler doesn't reproduce.

usage: shape_census.py [N]
"""
import collections
import csv
import os
import sys

import capstone
from capstone import x86 as cx

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import lift  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def shape(i):
    parts = [i.mnemonic]
    for op in i.operands:
        if op.type == cx.X86_OP_REG:
            parts.append("r%d" % (op.size * 8))
        elif op.type == cx.X86_OP_IMM:
            v = op.imm & 0xFFFFFFFF
            parts.append("i" + ("neg" if v >= 0x80000000 else "1" if v == 1 else ""))
        else:
            m = op.mem
            b = i.reg_name(m.base) if m.base else ""
            parts.append("m%d%s%s" % (op.size * 8, "[ebp]" if b == "ebp" else "[r]" if b else "[abs]",
                                      "+idx" if m.index else ""))
    return " ".join(parts)


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 40
    lift.init()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    ours, theirs = collections.Counter(), collections.Counter()
    with open(os.path.join(ROOT, "build", "lift", "report.csv"), newline="") as f:
        rows = [r for r in csv.DictReader(f) if r["status"] == "diff"]
    for r in rows:
        va = int(r["func"][5:], 16)
        p = os.path.join(ROOT, "build", "lift", r["func"] + ".bin")
        if not os.path.exists(p):
            continue
        for i in md.disasm(open(p, "rb").read(), va):
            ours[shape(i)] += 1
        for i in lift.IMG.insns(va):
            theirs[shape(i)] += 1
    keys = set(ours) | set(theirs)
    diff = sorted(keys, key=lambda k: -abs(ours[k] - theirs[k]))
    print("%-40s %7s %7s" % ("shape", "ours", "FALL"))
    for k in diff[:n]:
        print("%-40s %7d %7d" % (k, ours[k], theirs[k]))


if __name__ == "__main__":
    main()
