#!/usr/bin/env python3
"""Replays a feedback or crash report's session in the native build (docs/port.md, "Feedback and
crash reports"): the report's input.script (every key, click and mouse motion that reached the
game, on the script clock) as PORT_SCRIPT, its first srand seed (log.txt) as PORT_SRAND, and
screenshots every second up to the moment of the report, into <report>/replay/.

usage: port_replay.py REPORT_DIR [--game DIR] [--gl] [--every SECONDS] [--extra SECONDS]

The game's own timing (the frame clock) can still differ between two runs: a replay follows the
player's input, not always every frame of the game. The report's save/ (when it was taken in the
world) loads directly: copy it to an overlay's SAVE0.
"""
import argparse
import json
import os
import re
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def find_report(path):
    """the folder with meta.json (the service unpacks a report one level down)"""
    if os.path.exists(os.path.join(path, "meta.json")):
        return path
    for name in sorted(os.listdir(path)):
        sub = os.path.join(path, name)
        if os.path.isdir(sub) and os.path.exists(os.path.join(sub, "meta.json")):
            return sub
    sys.exit("no meta.json under %s" % path)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("report")
    ap.add_argument("--game", default=os.environ.get("DAGGER_GAME") or os.path.join(ROOT, "build", "game"))
    ap.add_argument("--gl", action="store_true")
    ap.add_argument("--every", type=float, default=1.0)
    ap.add_argument("--extra", type=float, default=3.0)
    a = ap.parse_args()
    rep = find_report(a.report)
    meta = json.load(open(os.path.join(rep, "meta.json")))
    steps = [l.strip() for l in open(os.path.join(rep, "input.script")) if l.strip()]
    seed = None
    for line in open(os.path.join(rep, "log.txt"), errors="replace"):
        m = re.search(r"srand (\d+) \(the session's first seed", line)
        if m:
            seed = m.group(1)
            break
    end = float(meta.get("session_seconds") or (float(steps[-1].split()[0]) if steps else 30.0))
    out = os.path.join(rep, "replay")
    shutil.rmtree(out, ignore_errors=True)
    os.makedirs(os.path.join(out, "overlay", "ARENA2"))
    packed = os.path.join(ROOT, "build", "emu", "packed")
    for f in ("ARCH3D.BSA", "DAGGER.SND"):
        if os.path.exists(os.path.join(packed, f)):
            os.link(os.path.join(packed, f), os.path.join(out, "overlay", "ARENA2", f))
    shots, t = [], a.every
    while t <= end + a.extra:
        shots.append("%.2f shot %s" % (t, os.path.join(out, "t%07.2f.bmp" % t)))
        t += a.every
    env = dict(os.environ, PORT_SCRIPT="; ".join(steps + shots),
               PORT_EXIT_AFTER=str(end + a.extra + 1))
    if seed:
        env["PORT_SRAND"] = seed
    if a.gl:
        env["PORT_GL"] = "1"
    print("replay: %d steps, seed %s, %.1f s (the report at %.1f s) -> %s"
          % (len(steps), seed, end + a.extra, end, out))
    r = subprocess.run([os.path.join(ROOT, "build", "port", "fall"), "--game", a.game, "--overlay",
                        os.path.join(out, "overlay"), "--nosound"], env=env,
                       capture_output=True, text=True, timeout=end + a.extra + 120)
    open(os.path.join(out, "log.txt"), "w").write(r.stdout + r.stderr)
    print("replay: exit %d, %d screenshots" % (r.returncode, len([f for f in os.listdir(out) if f.endswith(".bmp")])))


if __name__ == "__main__":
    main()
