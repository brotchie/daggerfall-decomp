#!/usr/bin/env python3
"""Show one example of each of the top first-difference clusters in build/lift/report.csv,
with a few instructions of context on both sides (tools/match.py output).

usage: cluster_peek.py [N clusters] [skip]
"""
import csv
import os
import re
import subprocess
import sys
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
n = int(sys.argv[1]) if len(sys.argv) > 1 else 8
skip = int(sys.argv[2]) if len(sys.argv) > 2 else 0
size = {r["name"]: int(r["size"]) for r in csv.DictReader(open(os.path.join(ROOT, "config", "functions.csv")))}
groups = defaultdict(list)
for r in csv.DictReader(open(os.path.join(ROOT, "build", "lift", "report.csv"))):
    if r["status"] == "diff":
        groups[re.sub(r"^\d+ bytes \(ours \d+, target \d+\): ", "", r["detail"])].append(r["func"])
ranked = sorted(groups.items(), key=lambda kv: -sum(size[f] for f in kv[1]))
for key, funcs in ranked[skip:skip + n]:
    f = min(funcs, key=lambda x: size[x])
    out = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "match.py"),
                          os.path.join(ROOT, "build", "lift", f + ".c")],
                         capture_output=True, text=True).stdout.splitlines()
    k = next((i for i, l in enumerate(out) if l.startswith("! ")), None)
    print("=== %s  (%d functions, %d bytes)  e.g. %s" % (key, len(funcs), sum(size[x] for x in funcs), f))
    if k is not None:
        for l in out[max(1, k - 3):k + 4]:
            print("   " + l[:150])
