#!/usr/bin/env python3
"""How evenly the OpenGL view moves from present to present (docs/port.md, "Smooth motion").

`run` plays a classic save (orig/saves/NAME.zip) with --gl, walks forward for a few seconds,
then turns with the keyboard, and records every present (PORT_GL_RECORD: the eye each present
was drawn from, and the middle rows of the picture); then the record is measured as below.
`measure FILE` measures a record made some other way.

For each stretch of motion (the walk, the turn), between its first and last presents that
moved, less 0.3 s at each end:
  - the eye's step per present (world units walking, 2048ths of a turn turning): its mean,
    and its variation (standard deviation / mean). The game draws about 54 frames a second
    and the display shows 60: without smooth motion a present repeats a frame now and then
    (a step of 0) and the variation is large; with it each step is about the same;
  - turning, the picture's own sideways shift per present (pixels), found by matching each
    present's column brightness against the last's;
  - the presents' spacing (ms).

usage: port_gl_motion.py run [--interp on|off] [--aspect 4:3|fill] [NAME]   (default: orcs)
       port_gl_motion.py measure FILE
"""
import argparse
import math
import os
import shutil
import struct
import subprocess
import sys
import zipfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.environ.get("DAGGER_GAME") or os.path.join(ROOT, "build", "game")
BUILD = os.environ.get("PORT_BUILD") or os.path.join(ROOT, "build", "port")
FALL = os.path.join(BUILD, "fall")
PACKED = os.path.join(ROOT, "build", "emu", "packed")
OUT = os.path.join(BUILD, "motion")

HEAD = struct.Struct("<12q")
# after the load (28 s): walk forward 4 s, wait, turn left 3 s
SCRIPT = ("22 key L; 26 click 80 28; 28 click 160 10; 32 down Up; 36 up Up; "
          "38 down Left; 41 up Left")
EXIT_AFTER = 43


def read_record(path):
    """[(header dict, rgba rows bytes, width, rows)]"""
    out = []
    with open(path, "rb") as f:
        while True:
            h = f.read(HEAD.size)
            if len(h) < HEAD.size:
                break
            v = HEAD.unpack(h)
            w, rows = v[4], v[5]
            px = f.read(w * rows * 4)
            if len(px) < w * rows * 4:
                break
            out.append(({"ns": v[0], "serial": v[1], "x": v[6] / 1000, "y": v[7] / 1000,
                         "z": v[8] / 1000, "yaw": v[9] / 1000, "pitch": v[10] / 1000,
                         "game_ms": v[11] / 1000}, px, w, rows))
    return out


def profile(px, w, rows):
    """each column's brightness, summed down the rows"""
    p = [0] * w
    for r in range(rows):
        base = r * w * 4
        for x in range(w):
            i = base + x * 4
            p[x] += px[i] + px[i + 1] + px[i + 2]
    return p


def best_shift(a, b, most):
    """the shift s (pixels) for which b[x] best matches a[x - s]"""
    w = len(a)
    best, best_s = None, 0
    for s in range(-most, most + 1):
        lo, hi = max(0, s) + w // 8, min(w, w + s) - w // 8
        if hi - lo < w // 4:
            continue
        d = sum(abs(b[x] - a[x - s]) for x in range(lo, hi, 2)) / ((hi - lo) / 2)
        if best is None or d < best:
            best, best_s = d, s
    return best_s


def stats(values):
    if not values:
        return 0.0, 0.0, 0.0
    m = sum(values) / len(values)
    sd = math.sqrt(sum((v - m) ** 2 for v in values) / len(values))
    return m, sd, (sd / m if m else 0.0)


def wrap(a):
    return (a + 1024) % 2048 - 1024


def stretch(rec, moved):
    """the presents of a stretch of motion: the longest run of presents that moved (moved(i):
    present i differs from i - 1; gaps under 0.25 s kept), less 0.3 s at each end"""
    idx = [i for i in range(1, len(rec)) if moved(i)]
    if not idx:
        return []
    runs, run = [], [idx[0]]
    for i in idx[1:]:
        if rec[i][0]["ns"] - rec[run[-1]][0]["ns"] < 2.5e8:
            run.append(i)
        else:
            runs.append(run)
            run = [i]
    runs.append(run)
    run = max(runs, key=lambda r: rec[r[-1]][0]["ns"] - rec[r[0]][0]["ns"])
    t0, t1 = rec[run[0]][0]["ns"] + 3e8, rec[run[-1]][0]["ns"] - 3e8
    return [i for i in range(run[0], run[-1] + 1) if t0 <= rec[i][0]["ns"] <= t1]


def measure(path):
    rec = read_record(path)
    if len(rec) < 10:
        print("%s: %d presents: too few" % (path, len(rec)))
        return
    gaps = [(rec[i][0]["ns"] - rec[i - 1][0]["ns"]) / 1e6 for i in range(1, len(rec))]
    m, sd, _ = stats(gaps)
    print("%s: %d presents, every %.2f ms (sd %.2f)" % (path, len(rec), m, sd))

    def step(i):
        a, b = rec[i - 1][0], rec[i][0]
        return math.hypot(b["x"] - a["x"], b["z"] - a["z"])

    def turn(i):
        return abs(wrap(rec[i][0]["yaw"] - rec[i - 1][0]["yaw"]))

    # the walk: the eye moves with the yaw still; the turn: the yaw moves (a jump, the load's
    # or a door's, is neither)
    walk = stretch(rec, lambda i: 0.01 < step(i) < 200 and turn(i) < 0.001)
    walk = [i for i in walk if turn(i) < 0.001 and step(i) < 200]
    turning = stretch(rec, lambda i: 0.001 < turn(i) < 100 and step(i) < 200)
    turning = [i for i in turning if turn(i) < 100]
    if walk:
        steps = [step(i) for i in walk]
        m, sd, cv = stats(steps)
        print("  walk: %d presents, a step of %.2f units (sd %.2f, variation %.1f%%), %d repeated "
              "(a step under a fifth of the mean)" % (len(steps), m, sd, cv * 100,
                                                      sum(s < m / 5 for s in steps)))
    else:
        print("  walk: no motion found")
    if turning:
        turns = [turn(i) for i in turning]
        m, sd, cv = stats(turns)
        print("  turn: %d presents, %.3f of 2048 a present (sd %.3f, variation %.1f%%), %d repeated"
              % (len(turns), m, sd, cv * 100, sum(t < m / 5 for t in turns)))
        prof = {}
        shifts = []
        for i in turning:
            for k in (i - 1, i):
                if k not in prof:
                    h, px, w, rows = rec[k]
                    prof[k] = profile(px, w, rows)
            shifts.append(abs(best_shift(prof[i - 1], prof[i], 48)))
        m, sd, cv = stats(shifts)
        print("  turn, the picture: %.1f px a present (sd %.1f, variation %.1f%%), %d presents "
              "unmoved" % (m, sd, cv * 100, sum(s == 0 for s in shifts)))
    else:
        print("  turn: no motion found")


def run(name, interp, aspect):
    out = os.path.join(OUT, "%s_%s_%s" % (name, interp, aspect.replace(":", "x")))
    if os.path.exists(out):
        shutil.rmtree(out)
    save0 = os.path.join(out, "overlay", "SAVE0")
    os.makedirs(save0)
    with zipfile.ZipFile(os.path.join(ROOT, "orig", "saves", name + ".zip")) as z:
        for info in z.infolist():
            if not info.is_dir():
                open(os.path.join(save0, os.path.basename(info.filename)), "wb").write(
                    z.read(info))
    os.makedirs(os.path.join(out, "overlay", "ARENA2"))
    for a in ("ARCH3D.BSA", "DAGGER.SND"):
        if os.path.exists(os.path.join(PACKED, a)):
            os.link(os.path.join(PACKED, a), os.path.join(out, "overlay", "ARENA2", a))
    record = os.path.join(out, "record.bin")
    env = dict(os.environ, DAGGER_GAME=GAME, PORT_GL="1", PORT_GL_INTERP=interp,
               PORT_GL_ASPECT=aspect, PORT_GL_RECORD=record, PORT_SCRIPT=SCRIPT,
               PORT_EXIT_AFTER=str(EXIT_AFTER), PORT_GL_DEBUG="1")
    if aspect == "fill":
        env.setdefault("PORT_WINDOW", "1280x720")
    r = subprocess.run([FALL, "--game", GAME, "--overlay", os.path.join(out, "overlay"),
                        "--nosound"], env=env, capture_output=True, text=True,
                       timeout=EXIT_AFTER + 60)
    open(os.path.join(out, "log.txt"), "w").write(r.stdout + r.stderr)
    print("%s, smooth motion %s, view %s:" % (name, interp, aspect))
    measure(record)


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("run")
    r.add_argument("--interp", default="on")
    r.add_argument("--aspect", default="4:3")
    r.add_argument("name", nargs="?", default="orcs")
    m = sub.add_parser("measure")
    m.add_argument("file")
    a = ap.parse_args()
    if a.cmd == "run":
        run(a.name, a.interp, a.aspect)
    else:
        measure(a.file)


if __name__ == "__main__":
    sys.exit(main())
