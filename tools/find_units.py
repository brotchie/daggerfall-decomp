#!/usr/bin/env python3
"""Recover FALL.EXE's source units (the original .c files) and write config/units.csv.

Many game functions pass __FILE__ to checked-memory and assert helpers, so the data object
holds strings like "faction.c", and each one is referenced only from one contiguous run of
functions. Those runs are in link order, so they give the original object files and their
order. For each name this tool records the first and last function that reference it; the
functions between two runs belong to one of the two neighbouring units (unknown which until
someone looks).

usage: find_units.py [FALL.EXE]
"""
import bisect
import csv
import os
import re
import sys
from collections import Counter, defaultdict

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE, SRC_OFF32  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
NAME = re.compile(rb"[A-Za-z0-9_]+\.(?:c|cpp|asm)", re.I)


def main():
    exe = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")
    le = LE(exe)
    img = le.load()
    data = le.objs[2]
    dbuf = img[data.index]

    def source_name(va):
        if not (data.base <= va < data.base + data.vsize):
            return None
        o = va - data.base
        end = dbuf.find(b"\x00", o, o + 64)
        if end < 0:
            return None
        s = bytes(dbuf[o:end])
        return s.decode("latin1").lower() if NAME.fullmatch(s) else None

    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        funcs = sorted((int(r["va"], 16), int(r["obj"])) for r in csv.DictReader(f))
    starts = [va for va, _ in funcs]
    refs = defaultdict(Counter)       # function va -> source name -> count
    for fx in le.fixups():
        if fx.kind != SRC_OFF32:
            continue
        name = source_name(fx.target_va)
        if name is None:
            continue
        i = bisect.bisect_right(starts, fx.src_va) - 1
        if i >= 0:
            refs[starts[i]][name] += 1

    units = []                        # [name, first va, last va, functions with refs]
    conflicts = []
    for va in starts:
        if va not in refs:
            continue
        name = refs[va].most_common(1)[0][0]
        if len(refs[va]) > 1:
            conflicts.append((va, dict(refs[va])))
        if units and units[-1][0] == name:
            units[-1][2] = va
            units[-1][3] += 1
        else:
            units.append([name, va, va, 1])
    seen = Counter(u[0] for u in units)
    split = [n for n, c in seen.items() if c > 1]

    with open(os.path.join(ROOT, "config", "units.csv"), "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["unit", "first_ref", "last_ref", "funcs_with_ref"])
        for name, lo, hi, n in units:
            w.writerow([name, "0x%08X" % lo, "0x%08X" % hi, n])
    print("units: %d (%d functions reference a source name)" % (len(units), len(refs)))
    if split:
        print("names in more than one run: %s" % ", ".join(sorted(split)))
    for va, c in conflicts:
        print("function 0x%08X references several names: %s" % (va, c))


if __name__ == "__main__":
    main()
