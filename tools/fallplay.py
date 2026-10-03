#!/usr/bin/env python3
"""Play FALL.EXE a step at a time, for a model (or a person) at the controls.

The game only runs when told to, so there is no real time: each step loads the session's last
snapshot, plays some input for some timer ticks, saves a new snapshot, and shows the screen
(2x, with a coordinate ruler) and the code that ran for the first time. Any step can be
rewound. Needs tools/build_unicorn.sh (speed and coverage).

usage:
  fallplay.py new SESSION FROM          FROM: a snapshot in build/emu/snap (e.g. cheat_tlalac_s)
  fallplay.py step SESSION "EVENTS" [--ticks N] [--frames K]
  fallplay.py rewind SESSION N          drop the steps after step N
  fallplay.py log SESSION               the steps so far
  fallplay.py cov SESSION               the session's coverage per source unit

EVENTS (';'-separated, ticks counted from the start of the step):
  T press KEY        hold KEY 20 ticks (a shorter press can fall between two frames)
  T press ctrl+f1    a chord: modifiers held around the key
  T hold KEY N       hold KEY for N ticks (walking: hold up 200)
  T click X,Y        left click at game pixel X,Y (0-319, 0-199; the screenshot is 2x)
  T rclick X,Y       right click;  T dclick X,Y  double click
  T mouse X,Y,B      move the mouse with buttons B (1 left, 2 right)
  T down KEY / T up KEY / T key KEY   as in tools/fallemu.py
Keys: a-z 0-9 f1-f12 up down left right esc enter space tab bksp ins del home end pgup pgdn
ctrl alt lshift rshift [ ] ; ' ` \\ , . / - = kp+ kp- kp*.
--ticks: how long the step runs (default: 60 after the last event); --frames K saves K evenly
spaced screenshots during the step as well as the last.
"""
import argparse
import ctypes
import json
import os
import re
import shutil
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402  (first: it picks the Unicorn build)
import fallcov  # noqa: E402

ROOT = fallemu.ROOT
PLAY = os.path.join(ROOT, "build", "play")
LO, HI = fallcov.LO, fallcov.HI


def sdir(session):
    return os.path.join(PLAY, session)


def load_steps(session):
    p = os.path.join(sdir(session), "steps.json")
    return json.load(open(p)) if os.path.exists(p) else []


def save_steps(session, steps):
    json.dump(steps, open(os.path.join(sdir(session), "steps.json"), "w"), indent=1)


def expand(events):
    """The step's events -> fallemu script text."""
    out = []
    for item in events.replace("\n", ";").split(";"):
        w = item.split()
        if not w:
            continue
        t, act = int(w[0]), w[1]
        if act == "press":
            *mods, key = w[2].split("+")
            for m in mods:
                out.append("%d down %s" % (t, m))
            out.append("%d down %s" % (t + 1, key))
            out.append("%d up %s" % (t + 21, key))
            for m in mods:
                out.append("%d up %s" % (t + 22, m))
        elif act == "hold":
            out += ["%d down %s" % (t, w[2]), "%d up %s" % (t + int(w[3]), w[2])]
        elif act == "rclick":
            x, y = w[2].split(",")
            out += ["%d mouse %s,%s,0" % (t, x, y), "%d mouse %s,%s,2" % (t + 20, x, y),
                    "%d mouse %s,%s,0" % (t + 50, x, y)]
        else:
            out.append(item.strip())
    return "; ".join(out)


def shot(emu, path):
    """The screen at 2x with a ruler every 20 game pixels (labels every 40)."""
    if emu.mode != 0x13:
        return False
    src = emu.read(0xA0000, 64000)
    pal = emu.palette
    # the ruler in the palette's brightest and darkest colours
    lum = [sum(c) for c in pal]
    hi_c, lo_c = lum.index(max(lum)), lum.index(min(lum))
    W, H = 640, 400
    px = bytearray(W * H)
    for y in range(H):
        row = src[(y // 2) * 320:(y // 2) * 320 + 320]
        px[y * W:(y + 1) * W] = bytes(c for c in row for _ in (0, 1))
    for g in range(0, 320, 20):          # ticks along the top and left edges
        for k in range(6 if g % 40 == 0 else 3):
            px[k * W + 2 * g] = hi_c
            if 2 * g < H:
                px[2 * g * W + k] = hi_c
    for g in range(0, 320, 40):
        for k in range(6):
            if k * W + 2 * g + 1 < len(px):
                px[k * W + 2 * g + 1] = lo_c
    fallemu.png(path, W, H, bytes(px), pal)
    return True


def cov_lib():
    from unicorn.unicorn_py3 import unicorn as ucmod
    lib = ucmod.uclib
    if not hasattr(lib, "uc_dagger_coverage"):
        raise SystemExit("needs the patched Unicorn: tools/build_unicorn.sh")
    lib.uc_dagger_coverage.argtypes = [ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64,
                                       ctypes.c_void_p]
    return lib


def session_cov(session):
    p = os.path.join(sdir(session), "cov.bin")
    return bytearray(zlib.decompress(open(p, "rb").read())) if os.path.exists(p) \
        else bytearray(HI - LO)


def new(session, frm):
    d = sdir(session)
    if os.path.exists(d):
        shutil.rmtree(d)
    os.makedirs(d)
    src = frm if frm.endswith(".snap") else os.path.join(ROOT, "build", "emu", "snap", frm + ".snap")
    shutil.copyfile(src, os.path.join(d, "000.snap"))
    emu = fallemu.Emu.load(os.path.join(d, "000.snap"), overlay=os.path.join(d, "overlay"))
    shot(emu, os.path.join(d, "000.png"))
    save_steps(session, [{"n": 0, "from": frm, "tick": emu.ticks}])
    print("session %s from %s; screen: %s" % (session, frm, os.path.join(d, "000.png")))


def step(session, events, ticks, frames):
    d = sdir(session)
    steps = load_steps(session)
    if not steps:
        raise SystemExit("no session %s: fallplay.py new %s FROM" % (session, session))
    n = steps[-1]["n"]
    emu = fallemu.Emu.load(os.path.join(d, "%03d.snap" % n), overlay=os.path.join(d, "overlay"))
    lib = cov_lib()
    lo, hi = fallemu.LOAD + LO, fallemu.LOAD + HI
    lib.uc_dagger_coverage(emu.uc._uch, lo, hi, None)
    script = fallemu.parse_script(expand(events))
    last = max([t for t, _a, _g in script] + [0])
    ticks = ticks or last + 60
    start = emu.ticks
    shots = []
    marks = [start + ticks * (k + 1) // (frames + 1) for k in range(frames)] if frames else []
    pending = [(start + t, a, g) for t, a, g in script]
    for k, m in enumerate(marks):
        emu.run(m, [e for e in pending if e[0] <= m], None)
        pending = [e for e in pending if e[0] > m]
        p = os.path.join(d, "%03d_%d.png" % (n + 1, k + 1))
        if shot(emu, p):
            shots.append(p)
    emu.run(start + ticks, pending, None)
    emu.save(os.path.join(d, "%03d.snap" % (n + 1)))
    final = os.path.join(d, "%03d.png" % (n + 1))
    shot(emu, final)
    buf = ctypes.create_string_buffer(hi - lo)
    lib.uc_dagger_coverage(emu.uc._uch, lo, hi, buf)
    cov = session_cov(session)
    fns = fallcov.functions()
    before = {va for va, _n, _g, _k in fns if LO <= va < HI and cov[va - LO]}
    cov = bytearray((int.from_bytes(cov, 'little') | int.from_bytes(buf.raw, 'little')).to_bytes(HI - LO, 'little'))
    open(os.path.join(d, "cov.bin"), "wb").write(zlib.compress(bytes(cov)))
    newf = [(g, k) for va, _n, g, k in fns if LO <= va < HI and cov[va - LO] and va not in before]
    steps.append({"n": n + 1, "events": events, "ticks": ticks, "tick": emu.ticks,
                  "new_functions": len(newf)})
    save_steps(session, steps)
    by = {}
    for g, k in newf:
        by[g] = by.get(g, 0) + 1
    tot = {}
    for va, _n, g, k in fns:
        if LO <= va < HI and cov[va - LO]:
            tot[k] = tot.get(k, 0) + 1
    print("step %d: %d ticks (tick %d)%s" % (n + 1, ticks, emu.ticks,
                                            "" if emu.exit_code is None else
                                            ", the game exited (%s)" % emu.exit_code))
    print("screen: %s" % final + ("" if not shots else "  (also %s)" % ", ".join(shots)))
    print("new code: %d functions%s" % (len(newf), "" if not by else ": " + ", ".join(
        "%s %d" % kv for kv in sorted(by.items(), key=lambda kv: -kv[1])[:12])))
    print("session total: game %d, xngine %d, library %d" % (
        tot.get("game", 0), tot.get("xngine", 0), tot.get("library", 0)))


def rewind(session, n):
    d = sdir(session)
    steps = [s for s in load_steps(session) if s["n"] <= n]
    for f in os.listdir(d):
        m = re.match(r"(\d{3})(_\d+)?\.(snap|png)$", f)
        if m and int(m.group(1)) > n:
            os.remove(os.path.join(d, f))
    save_steps(session, steps)
    print("session %s back at step %d (coverage keeps what the dropped steps ran)" % (session, n))


def log(session):
    for s in load_steps(session):
        print("%3d  tick %-6s %s" % (s["n"], s.get("tick"), s.get("events", "from " + s.get("from", ""))))


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    a = sub.add_parser("new")
    a.add_argument("session")
    a.add_argument("frm")
    a = sub.add_parser("step")
    a.add_argument("session")
    a.add_argument("events")
    a.add_argument("--ticks", type=int, default=None)
    a.add_argument("--frames", type=int, default=0)
    a = sub.add_parser("rewind")
    a.add_argument("session")
    a.add_argument("n", type=int)
    a = sub.add_parser("log")
    a.add_argument("session")
    a = sub.add_parser("cov")
    a.add_argument("session")
    a = ap.parse_args()
    if a.cmd == "new":
        new(a.session, a.frm)
    elif a.cmd == "step":
        step(a.session, a.events, a.ticks, a.frames)
    elif a.cmd == "rewind":
        rewind(a.session, a.n)
    elif a.cmd == "log":
        log(a.session)
    else:
        tmp = os.path.join(sdir(a.session), "session.cov")
        open(tmp, "wb").write(zlib.compress(b'{"snap": "session"}\n' + bytes(session_cov(a.session))))
        fallcov.report([tmp])


if __name__ == "__main__":
    main()
