#!/usr/bin/env python3
"""Write every lifted function that matched into src/lifted/<unit>.c.

Reads build/lift/report.csv (from tools/lift_all.py) and groups the `ok` functions by their
original source unit (tools/units.py). Functions already written by hand in src/*.c are left
out. src/lifted/ is generated: don't edit it; to work on a function by hand, move it into
src/<unit>.c and it will be dropped from here on the next run.

The lifter declares every callee from one signature table and every global as `char D_X[]`,
so functions from different lifts agree and can share a file. The build
(tools/build-and-verify.sh) is still the judge: run it after promoting.

usage: promote_lifted.py
"""
import csv
import glob
import os
import re
import shutil
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from units import load_units, unit_of  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIFT = os.path.join(ROOT, "build", "lift")
OUT = os.path.join(ROOT, "src", "lifted")
DEF = re.compile(r"^(?!extern\b)[A-Za-z][^;\n(]*\b(func_[0-9A-F]{8})\s*\(", re.M)


def proto_line(text, name):
    """The prototype a function's own definition implies, as an extern line minus 'extern '."""
    m = re.search(r"^(\w+(?: \w+)?) %s\((.*)\)$" % name, text, re.M)
    if not m:
        return ""
    types = ", ".join(re.sub(r"\s*a\d+$", "", p.strip()) for p in m.group(2).split(","))
    return "%s %s(%s);" % (m.group(1), name, types)


def main():
    hand = set()
    for path in glob.glob(os.path.join(ROOT, "src", "*.c")) + \
            glob.glob(os.path.join(ROOT, "src", "hand", "*.c")):
        hand |= set(DEF.findall(open(path).read()))
    with open(os.path.join(LIFT, "report.csv"), newline="") as f:
        ok = [r["func"] for r in csv.DictReader(f) if r["status"] == "ok"]
    ok = [n for n in ok if n not in hand]
    units = load_units()
    by_unit = {}
    for name in ok:
        unit, _certain, _nxt = unit_of(int(name[5:], 16), units)
        by_unit.setdefault(unit, []).append(name)

    if os.path.isdir(OUT):
        shutil.rmtree(OUT)
    os.makedirs(OUT)
    # A function whose definition declares parameters differently from the prototype its
    # callers use (the lifter types a parameter `short` from the frame layout but keeps the
    # callers' view) gets a file of its own, so neither side changes.
    texts = {n: open(os.path.join(LIFT, n + ".c")).read() for n in ok}
    proto = {}
    for n, text in texts.items():
        for line in text.splitlines():
            m = re.match(r"extern (\w+(?: \w+)?) (func_[0-9A-F]{8})\((.*)\);", line)
            if m:
                proto.setdefault(m.group(2), set()).add((m.group(1), m.group(3)))
    alone = set()
    for n, text in texts.items():
        m = re.search(r"^(\w+(?: \w+)?) %s\((.*)\)$" % n, text, re.M)
        if m and n in proto:
            types = ", ".join(re.sub(r"\s*a\d+$", "", p.strip()) for p in m.group(2).split(","))
            # (the return type too: a short function its callers declare int)
            if any((types != pt and pt != "") or (rt != m.group(1) and pt != "")
                   for rt, pt in proto[n]):
                alone.add(n)
    # A function that declares a callee differently from the rest of its unit (an implicit
    # `int f()` for a void function whose result it uses) also gets a file of its own.
    decls_of = {}
    for n, text in texts.items():
        for line in text.splitlines():
            m = re.match(r"extern \w+ (func_[0-9A-F]{8})\(", line)
            if m:
                decls_of.setdefault(n, {})[m.group(1)] = line
    # functions that match only on their own (the unit's other declarations change their
    # code): config/lift_alone.txt
    p = os.path.join(ROOT, "config", "lift_alone.txt")
    if os.path.exists(p):
        alone |= {w for line in open(p) if not line.startswith("#") for w in line.split()} & set(ok)
    for unit, names in by_unit.items():
        members = [n for n in names if n not in alone]
        votes = {}
        for n in members:
            for callee, line in decls_of.get(n, {}).items():
                votes.setdefault(callee, {}).setdefault(line, []).append(n)
        for callee, lines in votes.items():
            if len(lines) > 1:
                defined_here = callee in members
                keep = None
                if defined_here:
                    keep = next((l for l in lines if l == "extern " + proto_line(texts[callee], callee)), None)
                if keep is None:
                    keep = max(lines, key=lambda l: len(lines[l]))
                for line, users in lines.items():
                    if line != keep:
                        alone.update(users)
    groups = []
    for unit, names in sorted(by_unit.items()):
        shared = [n for n in names if n not in alone]
        if shared:
            groups.append((unit, unit, shared))
        for n in names:
            if n in alone:
                groups.append((unit, unit[:-2] + "_" + n[5:] + ".c", [n]))
    for unit, fname, names in groups:
        decls, bodies = [], []
        for name in sorted(names):
            text = open(os.path.join(LIFT, name + ".c")).read()
            head, body = text.split("\n\n", 1)
            for line in head.splitlines():
                if line.startswith("#pragma dagger"):
                    continue        # (an old lift's knob for a compiler we no longer use)
                if line.startswith(("extern", "#pragma", "struct")) and line not in decls:
                    decls.append(line)
            bodies.append(body.strip())
        defined = set(names)
        # a function defined here needs no extern, but keep its prototype for earlier callers
        decls = [d.replace("extern ", "", 1) if any(n in d for n in defined) else d
                 for d in decls]
        structs = sorted(d for d in decls if d.startswith("struct"))
        decls = [d for d in decls if not d.startswith("struct")]
        data = structs + sorted(d for d in decls if " D_" in d)
        code = [d for d in decls if " D_" not in d and not d.startswith("#pragma")]
        pragmas = [d for d in decls if d.startswith("#pragma")]
        # a calling convention's definition before the functions that use it
        code = sorted(code) + sorted(pragmas, key=lambda d: (d.startswith("#pragma aux ("), d))
        out = ["/* %s: lifted functions that match (generated by tools/promote_lifted.py;" % unit,
               " * do not edit: move a function to src/%s to work on it by hand) */" % unit, ""]
        out += data + [""] + code + [""]
        out += ["\n\n".join(bodies), ""]
        open(os.path.join(OUT, fname), "w").write("\n".join(out))
    print("promoted %d functions into %d units (src/lifted/, %d in files of their own)"
          % (len(ok), len(by_unit), len(alone)))


if __name__ == "__main__":
    main()
