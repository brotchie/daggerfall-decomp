#!/usr/bin/env python3
"""Which functions of FALL.EXE ran: code coverage of the headless game (tools/fallemu.py).

Needs the patched Unicorn (tools/build_unicorn.sh): it marks every guest code byte it
translates, and code is translated just before it first runs, so coverage costs nothing at
run time. Covered = a function's first byte was translated.

usage:
  fallcov.py run NAME[,NAME...] [--script FILE|"EVENTS"] [--ticks N] [-j N]
      start from build/emu/snap/save_NAME.snap (or a .snap path), run the script (fallemu
      --script syntax, ticks from the snapshot; default: EXPLORE below) and write
      build/cov/NAME.cov; one process per snapshot, N at a time
  fallcov.py report [FILE.cov ...] [--list UNIT]
      the union of the runs (default: all of build/cov): functions run, per game source unit
      and per XnGine module; --list prints a unit's functions that never ran
"""
import argparse
import csv
import glob
import json
import os
import subprocess
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
COV = os.path.join(ROOT, "build", "cov")
LO, HI = 0x10000, 0x162000          # objects 1 and 2 (preferred addresses)

# Walk, turn, jump, then open the screens bound to keys in the 3D world (F5 character sheet,
# F6 inventory, M automap, L logbook, U magic items, V travel map, I location, R rest, F10 the
# large HUD twice, Esc the options menu), each closed with Esc. Keys are held 20 ticks: a
# press shorter than a frame is missed.
EXPLORE = ["10 down up; 200 up up", "210 down left; 260 up left", "270 down up; 470 up up",
           "480 down right; 560 up right", "570 down j; 590 up j", "600 down up; 800 up up"]
_t = 820
for _k in ("f5", "f6", "m", "l", "u", "v", "i", "r", "f10", "f10", "esc"):
    EXPLORE.append("%d down %s; %d up %s; %d down esc; %d up esc" % (
        _t, _k, _t + 20, _k, _t + 150, _t + 170) if _k != "f10" else
        "%d down f10; %d up f10" % (_t, _t + 20))
    _t += 250
EXPLORE += ["%d down down; %d up down" % (_t, _t + 100), "%d down left; %d up left" % (
    _t + 110, _t + 200), "%d down up; %d up up" % (_t + 210, _t + 500)]
EXPLORE_TICKS = _t + 520
EXPLORE = "; ".join(EXPLORE)


def snap_path(name):
    return name if name.endswith(".snap") else os.path.join(
        ROOT, "build", "emu", "snap", "save_%s.snap" % name)


def one(name, script, ticks, shots):
    """Run one snapshot with coverage on; write build/cov/NAME.cov."""
    import fallemu
    import ctypes
    from unicorn.unicorn_py3 import unicorn as ucmod
    lib = ucmod.uclib
    if not hasattr(lib, "uc_dagger_coverage"):
        raise SystemExit("coverage needs the patched Unicorn: tools/build_unicorn.sh")
    lib.uc_dagger_coverage.argtypes = [ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64,
                                       ctypes.c_void_p]
    base = os.path.basename(name)[:-5] if name.endswith(".snap") else name
    emu = fallemu.Emu.load(snap_path(name), overlay=os.path.join(
        ROOT, "build", "emu", "overlay_cov", base))
    uch = emu.uc._uch
    lo, hi = fallemu.LOAD + LO, fallemu.LOAD + HI
    lib.uc_dagger_coverage(uch, lo, hi, None)
    start = emu.ticks
    emu.run(start + ticks, [(start + t, a, g) for t, a, g in fallemu.parse_script(script)], shots)
    buf = ctypes.create_string_buffer(hi - lo)
    lib.uc_dagger_coverage(uch, lo, hi, buf)
    os.makedirs(COV, exist_ok=True)
    with open(os.path.join(COV, base + ".cov"), "wb") as f:
        f.write(zlib.compress(json.dumps({"snap": name, "ticks": ticks}).encode() + b"\n" +
                              buf.raw))
    if shots:
        emu.screenshot(os.path.join(shots, "cov_%s.png" % base))
    print("%s: %d ticks, exit %s" % (base, ticks, emu.exit_code), flush=True)


def load_cov(path):
    data = zlib.decompress(open(path, "rb").read())
    meta, _, cmap = data.partition(b"\n")
    return json.loads(meta), cmap


def functions():
    """[(va, name, group, kind)]: game functions by source unit, library, XnGine by module."""
    from units import load_units, unit_of
    with open(os.path.join(ROOT, "config", "regions.csv"), newline="") as f:
        regions = {r["region"]: (int(r["start"], 16), int(r["end"], 16)) for r in csv.DictReader(f)}
    units = load_units()
    out = []
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        for r in csv.DictReader(f):
            va = int(r["va"], 16)
            if regions["game"][0] <= va < regions["game"][1]:
                out.append((va, r["name"], unit_of(va, units)[0], "game"))
            elif regions["library"][0] <= va < regions["library"][1]:
                out.append((va, r["name"], "library", "library"))
    with open(os.path.join(ROOT, "config", "xngine_modules.csv"), newline="") as f:
        mods = [(int(r["start"], 16), int(r["end"], 16), r["name"]) for r in csv.DictReader(f)]
    with open(os.path.join(ROOT, "config", "xngine_functions.csv"), newline="") as f:
        for r in csv.DictReader(f):
            va = int(r["va"], 16)
            mod = next((n for s, e, n in mods if s <= va < e), "xngine")
            out.append((va, "func_%08X" % va, mod, "xngine"))
    return out


def report(paths, list_unit=None):
    acc = 0
    for p in paths:
        _meta, m = load_cov(p)
        acc |= int.from_bytes(m, "little")
    cmap = acc.to_bytes(HI - LO, "little")
    fns = functions()
    by = {}
    for va, name, group, kind in fns:
        hit = bool(cmap[va - LO]) if LO <= va < HI else False
        by.setdefault((kind, group), []).append((va, name, hit))
    for kind in ("game", "xngine", "library"):
        rows = [(g, v) for (k, g), v in by.items() if k == kind]
        tot = sum(len(v) for _g, v in rows)
        hit = sum(sum(h for _a, _n, h in v) for _g, v in rows)
        print("%-8s %4d / %4d functions ran (%.0f%%)" % (kind, hit, tot, 100.0 * hit / max(1, tot)))
    print()
    print("game units, least covered first:")
    rows = sorted(((g, v) for (k, g), v in by.items() if k == "game"),
                  key=lambda gv: (sum(h for _a, _n, h in gv[1]) / len(gv[1]), gv[0]))
    for g, v in rows:
        h = sum(x[2] for x in v)
        print("  %-14s %3d / %3d" % (g, h, len(v)))
    if list_unit:
        for (k, g), v in sorted(by.items()):
            if g == list_unit:
                for va, name, hit in v:
                    if not hit:
                        print("  never ran: %s" % name)


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("run")
    r.add_argument("names")
    r.add_argument("--script", default=None)
    r.add_argument("--ticks", type=int, default=None)
    r.add_argument("-j", type=int, default=os.cpu_count())
    r.add_argument("--shots", default=os.path.join(ROOT, "build", "cov"))
    o = sub.add_parser("one")
    o.add_argument("name")
    o.add_argument("script")
    o.add_argument("ticks", type=int)
    o.add_argument("shots")
    rp = sub.add_parser("report")
    rp.add_argument("files", nargs="*")
    rp.add_argument("--list", default=None)
    a = ap.parse_args()
    if a.cmd == "one":
        one(a.name, a.script, a.ticks, a.shots)
    elif a.cmd == "run":
        script = EXPLORE if a.script is None else (
            open(a.script).read() if os.path.exists(a.script) else a.script)
        ticks = a.ticks or (EXPLORE_TICKS if a.script is None else 2000)
        names = a.names.split(",")
        procs = []
        for n in names:
            while sum(p.poll() is None for p in procs) >= a.j:
                procs[[p.poll() is None for p in procs].index(True)].wait()
            procs.append(subprocess.Popen([sys.executable, __file__, "one", n, script,
                                           str(ticks), a.shots]))
        for p in procs:
            p.wait()
    else:
        report(a.files or sorted(glob.glob(os.path.join(COV, "*.cov"))), a.list)


if __name__ == "__main__":
    main()
