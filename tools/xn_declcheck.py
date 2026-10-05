#!/usr/bin/env python3
"""Declarations across the readable XnGine C: for each function defined in C, the `#pragma aux`
interfaces other files give it, where they differ from its own file's. A caller declaring
another function with a different register interface calls it wrong once both are C (the
linker sends the call to the C definition). A difference is harmless when the parameters and
result sit in the same registers and the caller assumes at least the callee's `modify` set.

usage: xn_declcheck.py [DIR ...]      (default: src/engine)
"""
import collections
import glob
import os
import re
import sys
dirs = sys.argv[1:] or ["src/engine"]
files = [f for d in dirs for f in glob.glob(d + "/*.[ch]")]
prag = collections.defaultdict(dict)      # name -> {file: normalised pragma}
defs = {}
for f in files:
    t = open(f, encoding="latin-1").read()
    t = re.sub(r"/\*.*?\*/", " ", t, flags=re.S)
    for m in re.finditer(r"#pragma aux\s+(\w+)\s+([^;\n]*(?:\n[^#;\n][^;\n]*)*);", t):
        name, body = m.group(1), m.group(2)
        if body.lstrip().startswith(("=", '"')):
            continue                              # inline code or alias, not an interface
        prag[name][f] = " ".join(body.split())
    for m in re.finditer(r"^[A-Za-z_][\w \*]*?\b(\w+)\s*\([^;{]*\)\s*\{", t, re.M):
        if m.group(1) not in ("if", "while", "for", "switch"):
            defs.setdefault(m.group(1), f)
# the pragmas that apply to a definition: its own file's and those of the headers it includes
incl = {}
for f in files:
    d = os.path.dirname(f)
    incl[f] = {f} | {os.path.join(d, h) for h in
                     re.findall(r'#include\s+"([^"]+)"', open(f, encoding="latin-1").read())}
bad = 0
for name, df in sorted(defs.items()):
    p = prag.get(name, {})
    own = incl.get(df, {df})
    mine = {v for f, v in p.items() if f in own}
    for f, v in sorted(p.items()):
        if f not in own and v not in mine:
            bad += 1
            print("%-36s defined in %-20s %-44s | %s: %s" % (
                name, os.path.basename(df), (sorted(mine) or ["(watcom default)"])[0][:44],
                os.path.basename(f), v[:70]))
print(bad, "declarations differ from the definition's")
