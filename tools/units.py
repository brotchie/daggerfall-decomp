#!/usr/bin/env python3
"""Which original source unit a function belongs to (config/units.csv).

A function inside a unit's run of __FILE__ references belongs to that unit. A function between
two runs belongs to one of the two neighbours; until someone settles it, it is filed with the
earlier one and reported as uncertain.

usage: units.py 0xVA [0xVA ...]
"""
import csv
import os
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME_END = 0x9DA1C


def load_units():
    with open(os.path.join(ROOT, "config", "units.csv"), newline="") as f:
        return [(r["unit"], int(r["first_ref"], 16), int(r["last_ref"], 16))
                for r in csv.DictReader(f) if int(r["first_ref"], 16) < GAME_END]


def unit_of(va, units=None):
    """Return (unit, certain, next_unit_or_None) for a game function address."""
    units = units or load_units()
    prev = None
    for i, (name, lo, hi) in enumerate(units):
        if lo <= va <= hi:
            return name, True, None
        if va < lo:
            nxt = name
            return (prev or name), prev is None and False, nxt
        prev = name
    return prev, False, None


def main():
    units = load_units()
    for a in sys.argv[1:]:
        name, certain, nxt = unit_of(int(a, 16), units)
        print("%s %s%s" % (a, name, "" if certain else " (or %s)" % (nxt or "a later unit")))


if __name__ == "__main__":
    main()
