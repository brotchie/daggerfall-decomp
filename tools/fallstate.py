#!/usr/bin/env python3
"""Find where FALL.EXE keeps a piece of game state, from controlled experiments.

Each experiment plays the same short input from several starting snapshots and reads the
static data (object 3, 0x170000-0x1B5520) or all memory afterwards, then looks for what
tracks the change:
  screens   which screen is open (F5 sheet, F6 inventory, M automap, W travel map, L logbook,
            Esc options, R rest, T transport, Backspace spellbook): a byte or word that is the
            same from every base for one screen and differs between screens
  modes     the F1-F4 interaction mode (steal, grab, info, talk), the same way
  values    numbers the play agents read off the character sheet in several saves (gold,
            health, fatigue, magicka): where each save holds its value
  gold      the gold cheat (Ctrl+F9) pressed 0-2 times: what goes up by the same step
  time      resting 0-3 hours: what goes up by the same step per hour

usage: fallstate.py EXPERIMENT [--bases a,b,c]
"""
import argparse
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402  (first: it picks the Unicorn build)
import fallplay  # noqa: E402

ROOT = fallemu.ROOT
SNAPS = os.path.join(ROOT, "build", "emu", "snap")
OV = os.path.join(ROOT, "build", "state", "ov_%d" % os.getpid())   # one per process
O3, O3_END = 0x170000, 0x1B5520
BASES = ["cheat_tlalac_s", "cheat_morthag1", "cheat_dorian"]

SCREENS = {"world": "", "sheet": "5 press f5", "inventory": "5 press f6", "automap": "5 press m",
           "travelmap": "5 press w", "logbook": "5 press l", "options": "5 press esc",
           "rest": "5 press r", "transport": "5 press t", "spellbook": "5 press bksp"}
MODES = {"steal": "5 press f1", "grab": "5 press f2", "info": "5 press f3", "talk": "5 press f4"}

# Read off the character sheet by the play agents (build/play/*_scratch/NOTES.md)
VALUES = {
    "gold": {"save_blades": 146359, "save_keophex": 200101, "cheat_morthag1": 91226},
    "health": {"save_blades": 356, "save_keophex": 309},
    "max health": {"save_blades": 356, "save_keophex": 489},
    "fatigue": {"save_blades": 190, "save_keophex": 187},
    "magicka": {"cheat_morthag1": 28},
    "max magicka": {"cheat_morthag1": 160},
}


def run(base, events, extra=200, whole=False):
    """Memory after playing events (step syntax) from a snapshot: object 3, or everything."""
    emu = fallemu.Emu.load(os.path.join(SNAPS, base + ".snap"), overlay=OV)
    if events:
        fallplay.run_events(emu, events, extra)
    else:
        emu.run(emu.ticks + extra, [], None)
    if whole:
        mem = bytes(emu.uc.mem_read(fallemu.LOAD, fallemu.MEM))
        lo = 0
    else:
        mem = bytes(emu.uc.mem_read(fallemu.LOAD + O3, O3_END - O3))
        lo = O3
    emu.close()
    return lo, mem


def census(groups, width=1):
    """groups: {label: [memory, ...]} (one per base). Addresses whose value is the same in
    every memory of a group and not the same for all groups: [(address, {label: value})],
    those telling most groups apart first."""
    labels = list(groups)
    mems = [groups[g] for g in labels]
    n = len(mems[0][0])
    fmt = {1: "B", 2: "<H", 4: "<I"}[width]
    out = []
    for a in range(0, n - width + 1, width if width > 1 else 1):
        vals = []
        for ms in mems:
            v0 = ms[0][a:a + width]
            if any(m[a:a + width] != v0 for m in ms[1:]):
                break
            vals.append(v0)
        else:
            k = len(set(vals))
            if k > 1:
                out.append((k, a, {g: struct.unpack(fmt, v)[0] for g, v in zip(labels, vals)}))
    out.sort(key=lambda x: (-x[0], x[1]))
    return [(a, d) for _k, a, d in out]


def show(cands, lo, top=25):
    for a, d in cands[:top]:
        print("  0x%06X  %s" % (lo + a, "  ".join("%s=%d" % kv for kv in d.items())))
    print("  (%d candidates)" % len(cands))


def progression(series, width=4):
    """series: {base: [memory after 0, 1, 2, ... steps]}. Addresses that change by the same
    nonzero step each time, the step the same from every base: [(address, step, {base: v0})]."""
    fmt = {2: "<h", 4: "<i"}[width]
    bases = list(series)
    n = len(series[bases[0]][0])
    out = []
    for a in range(0, n - width + 1):
        step = None
        v0s = {}
        for b in bases:
            vs = [struct.unpack_from(fmt, m, a)[0] for m in series[b]]
            d = vs[1] - vs[0]
            if d == 0 or any(vs[i + 1] - vs[i] != d for i in range(len(vs) - 1)) or \
                    (step is not None and d != step):
                break
            step = d
            v0s[b] = vs[0]
        else:
            out.append((a, step, v0s))
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("experiment", choices=["screens", "modes", "values", "gold", "time"])
    ap.add_argument("--bases", default=",".join(BASES))
    a = ap.parse_args()
    bases = a.bases.split(",")
    os.makedirs(OV, exist_ok=True)
    if a.experiment in ("screens", "modes"):
        acts = SCREENS if a.experiment == "screens" else MODES
        groups = {}
        for g, ev in acts.items():
            groups[g] = [run(b, ev)[1] for b in bases]
            print("ran %s" % g, flush=True)
        for w in (1, 2, 4):
            print("width %d:" % w)
            show(census(groups, w), O3, 15)
    elif a.experiment == "values":
        mems = {}
        for name, per in VALUES.items():
            for save in per:
                if save not in mems:
                    mems[save] = run(save, "", 50, whole=True)[1]
        for name, per in VALUES.items():
            print("%s:" % name)
            for save, v in per.items():
                m = mems[save]
                hits = []
                for w, fmt in ((4, "<i"), (2, "<h")):
                    if w == 2 and not -32768 <= v < 32768:
                        continue
                    pat = struct.pack(fmt, v)
                    i = m.find(pat)
                    while i >= 0 and len(hits) < 40:
                        hits.append((i, w))
                        i = m.find(pat, i + 1)
                static = [h for h in hits if O3 <= h[0] < O3_END]
                print("  %s = %d: %d places, in object 3: %s" % (
                    save, v, len(hits), " ".join("0x%06X/%d" % h for h in static[:12])))
    elif a.experiment == "gold":
        series = {b: [run(b, "; ".join("%d press ctrl+f9" % (5 + 60 * i) for i in range(k)) if k else "")[1]
                      for k in range(3)] for b in bases}
        res = progression(series)
        for addr, step, v0 in res[:20]:
            print("  0x%06X  +%d per press  %s" % (O3 + addr, step, v0))
        print("  (%d candidates)" % len(res))
    elif a.experiment == "time":
        def rest(h):
            if not h:
                return ""
            return "5 press r; 80 click 110,70; 160 press %d; 200 press enter" % h
        series = {b: [run(b, rest(h), 300 + 220 * h)[1] for h in range(4)] for b in bases}
        res = progression(series)
        for addr, step, v0 in res[:20]:
            print("  0x%06X  +%d per hour  %s" % (O3 + addr, step, v0))
        print("  (%d candidates)" % len(res))


if __name__ == "__main__":
    main()
