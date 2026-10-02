#!/usr/bin/env python3
"""Which instruction shapes does our compiler emit more or less often than Watcom 10 did?

For every game function that lifted and compiled (build/lift/<func>.bin from
tools/lift_all.py), disassemble our bytes and the original's, normalise each instruction to a
shape (mnemonic + operand kinds: r32, r8, imm, [ebp-x], [ebp+x], [mem], [reg+d], ...), and
rank the shapes by the difference in counts. A shape we overuse that Watcom 10 avoided (or the
reverse) points at a code-generator difference, like `cdq` (original 2, ours hundreds).

usage: idiom_diff.py [N]
"""
import csv
import os
import re
import sys
from collections import Counter

import capstone

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from match import Target  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIFT = os.path.join(ROOT, "build", "lift")
REG32 = r"e[abcd]x|esi|edi|esp|ebp"


def shape(ins):
    ops = []
    for op in ins.op_str.split(", ") if ins.op_str else []:
        op = op.strip()
        if re.fullmatch(REG32, op):
            ops.append("r32" if op not in ("esp", "ebp") else op)
        elif re.fullmatch(r"[abcd]x|si|di", op):
            ops.append("r16")
        elif re.fullmatch(r"[abcd][lh]", op):
            ops.append("r8")
        elif re.fullmatch(r"-?0x[0-9a-f]+|-?\d+", op):
            ops.append("imm")
        elif "ptr" in op:
            size = op.split()[0]
            inner = op[op.index("[") + 1:-1]
            if re.fullmatch(r"ebp - 0x[0-9a-f]+", inner):
                kind = "[ebp-x]"
            elif re.fullmatch(r"ebp \+ 0x[0-9a-f]+", inner):
                kind = "[ebp+x]"
            elif re.fullmatch(r"0x[0-9a-f]+|0", inner):
                kind = "[mem]"
            elif "*" in inner:
                kind = "[idx]"
            else:
                kind = "[reg+d]"
            ops.append(size + kind)
        else:
            ops.append("?")
    if ins.mnemonic.startswith("j") or ins.mnemonic == "call":
        ops = []
    return " ".join([ins.mnemonic] + [", ".join(ops)]).strip()


def main():
    n = int(sys.argv[1]) if len(sys.argv) > 1 else 30
    tgt = Target()
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    funcs = {r["name"]: (int(r["va"], 16), int(r["size"]))
             for r in csv.DictReader(open(os.path.join(ROOT, "config", "functions.csv")))}
    ours, theirs = Counter(), Counter()
    count = 0
    for r in csv.DictReader(open(os.path.join(LIFT, "report.csv"))):
        if r["status"] not in ("diff",):
            continue
        p = os.path.join(LIFT, r["func"] + ".bin")
        if not os.path.exists(p):
            continue
        va, size = funcs[r["func"]]
        ours.update(shape(i) for i in md.disasm(open(p, "rb").read(), 0))
        theirs.update(shape(i) for i in md.disasm(tgt.bytes_at(va, size), va))
        count += 1
    rows = sorted(set(ours) | set(theirs), key=lambda k: -abs(ours[k] - theirs[k]))
    print("%d differing functions; shapes by |ours - original|:" % count)
    print("%7s %7s %7s  %s" % ("ours", "orig", "diff", "shape"))
    for k in rows[:n]:
        print("%7d %7d %+7d  %s" % (ours[k], theirs[k], ours[k] - theirs[k], k))


if __name__ == "__main__":
    main()
