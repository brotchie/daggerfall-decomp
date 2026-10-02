#!/usr/bin/env python3
"""Copy functions the real Watcom C32 10.0a matched (build/lift10/report.csv, from
`DAGGER_CC=w10 DAGGER_W10EXTRA=-d2 tools/lift_all.py`) into src/w10/<func>.c.

Only functions the build doesn't already match (build/matched.txt) are copied, and an
existing src/w10/ file is left alone (it may have been tidied by hand). The build compiles
src/w10/ with that compiler (tools/build_fall.py).

usage: promote_w10.py
"""
import csv
import os

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIFT = os.path.join(ROOT, "build", "lift10")
OUT = os.path.join(ROOT, "src", "w10")


def main():
    with open(os.path.join(LIFT, "report.csv"), newline="") as f:
        ok = [r["func"] for r in csv.DictReader(f) if r["status"] == "ok"]
    matched = set()
    p = os.path.join(ROOT, "build", "matched.txt")
    if os.path.exists(p):
        matched = {line.split()[1] for line in open(p)}
    os.makedirs(OUT, exist_ok=True)
    added = []
    for name in ok:
        dst = os.path.join(OUT, name + ".c")
        if name in matched or os.path.exists(dst):
            continue
        text = open(os.path.join(LIFT, name + ".c")).read()
        first, rest = text.split("\n", 1)
        head = first.replace("/* lifted from", "/* matched by the real Watcom C32 10.0a (-d2), "
                             "lifted from")
        with open(dst, "w") as f:
            f.write(head + "\n" + rest)
        added.append(name)
    print("src/w10/: +%d (%s)" % (len(added), " ".join(added)))


if __name__ == "__main__":
    main()
