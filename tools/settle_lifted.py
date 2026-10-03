#!/usr/bin/env python3
"""Promote the lifted functions and settle src/lifted/ until the build accepts all of it.

tools/lift_all.py checks every function compiled on its own; src/lifted/ groups them by
source unit, and Watcom 10.0a's code for a function can depend on the declarations around
it (a callee declared with or without a prototype). So: promote, build, and move every
function that fails inside a unit file into a file of its own (config/lift_alone.txt); one
that fails even there is marked `diff` in build/lift/report.csv. Repeat until the build has
no errors in src/lifted/.

usage: settle_lifted.py
"""
import csv
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
PY = sys.executable
ALONE = os.path.join(ROOT, "config", "lift_alone.txt")
REPORT = os.path.join(ROOT, "build", "lift", "report.csv")


def read_alone():
    if not os.path.exists(ALONE):
        return set()
    return {w for line in open(ALONE) if not line.startswith("#") for w in line.split()}


def write_alone(names):
    with open(ALONE, "w") as f:
        f.write("# lifted functions that match only in a file of their own "
                "(tools/settle_lifted.py)\n")
        for n in sorted(names):
            f.write(n + "\n")


def main():
    for rnd in range(1, 11):
        subprocess.run([PY, os.path.join(ROOT, "tools", "promote_lifted.py")], check=True)
        r = subprocess.run([PY, os.path.join(ROOT, "tools", "build_fall.py")],
                           capture_output=True, text=True)
        bad = re.findall(r"^ERROR src/lifted/(\S+?)\.c: (func_[0-9A-F]{8})", r.stdout, re.M)
        print("round %d: %d lifted functions fail" % (rnd, len(bad)))
        if not bad:
            print("\n".join(r.stdout.strip().splitlines()[-2:]))
            return 0
        alone = read_alone()
        drop = set()
        for fname, func in bad:
            if fname.endswith("_" + func[5:]):
                drop.add(func)          # already on its own: it doesn't match
            else:
                alone.add(func)
        write_alone(alone - drop)
        if drop:
            with open(REPORT, newline="") as f:
                rows = list(csv.DictReader(f))
            for row in rows:
                if row["func"] in drop:
                    row["status"], row["detail"] = "diff", "fails in the build (10.0a)"
            with open(REPORT, "w", newline="") as f:
                w = csv.DictWriter(f, fieldnames=["func", "status", "detail"])
                w.writeheader()
                w.writerows(rows)
            print("  dropped: " + " ".join(sorted(drop)))
    return 1


if __name__ == "__main__":
    sys.exit(main())
