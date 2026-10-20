#!/usr/bin/env python3
"""Random input on the classic saves (orig/saves/*.zip), for crashes and AddressSanitizer
reports: each save loads from slot 0 of an overlay of its own, then gets a key, a click, some
mouse motion or (with --resize) a new window size every few tenths of a second. A run's log
goes to build/port/monkey/NAME/log.txt; the summary says, per save, whether the game was still
running when the time was up, and the AddressSanitizer errors and crashes its log shows.

Build with PORT_SANITIZE=address PORT_BUILD=build/port-asan first for the sanitizer, and run
with --build build/port-asan. --env K=V adds to the game's environment (PORT_GL=1,
PORT_GL_ASPECT=fill ...).

usage: port_monkey.py [--build DIR] [--seconds N] [--jobs N] [--seed N] [--resize]
                      [--env K=V ...] [NAME ...]   (default: every save)
"""
import argparse
import os
import random
import shutil
import subprocess
import sys
import zipfile
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.environ.get("DAGGER_GAME") or os.path.join(ROOT, "build", "game")
PACKED = os.path.join(ROOT, "build", "emu", "packed")

KEYS = ["Up", "Down", "Left", "Right", "Space", "Return", "Escape", "I", "M", "V", "J", "R",
        "F1", "F2", "F3", "F4", "F5", "F10", "F11", "A", "S", "W", "D", "Tab", "Backspace",
        "Y", "N", "Page Up", "Page Down", "Home", "End", "Insert", "Delete", "Q", "E", "T"]
SIZES = [(960, 720), (1280, 720), (1680, 720), (800, 900), (2400, 720), (640, 480)]


def script(rng, seconds, resize):
    steps = ["22 key L", "26 click 80 28", "28 click 160 10"]
    t = 30.0
    while t < 30 + seconds:
        r = rng.random()
        if r < 0.40:
            steps.append("%.2f key %s" % (t, rng.choice(KEYS)))
        elif r < 0.55:
            k = rng.choice(["Up", "Left", "Right", "Down"])
            steps.append("%.2f down %s; %.2f up %s" % (t, k, t + rng.uniform(0.2, 1.5), k))
        elif r < 0.75:
            steps.append("%.2f click %d %d" % (t, rng.randrange(320), rng.randrange(200)))
        elif r < 0.80:
            steps.append("%.2f rclick %d %d" % (t, rng.randrange(320), rng.randrange(200)))
        elif r < 0.94:
            steps.append("%.2f mrel %.1f %.1f" % (t, rng.uniform(-60, 60), rng.uniform(-30, 30)))
        elif resize:
            steps.append("%.2f resize %d %d" % ((t,) + rng.choice(SIZES)))
        else:
            steps.append("%.2f key Escape" % t)
        t += rng.uniform(0.1, 0.5)
    return "; ".join(steps)


def one(name, a, env_extra, seed):
    out = os.path.join(a.build, "monkey", name)
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
    for f in ("ARCH3D.BSA", "DAGGER.SND"):
        if os.path.exists(os.path.join(PACKED, f)):
            os.link(os.path.join(PACKED, f), os.path.join(out, "overlay", "ARENA2", f))
    rng = random.Random("%s-%d" % (name, seed))
    env = dict(os.environ, DAGGER_GAME=GAME, PORT_SCRIPT=script(rng, a.seconds, a.resize),
               PORT_EXIT_AFTER=str(30 + a.seconds),
               ASAN_OPTIONS="handle_segv=0:handle_sigbus=0:detect_leaks=0:halt_on_error=0")
    env.update(env_extra)
    try:
        r = subprocess.run([os.path.join(a.build, "fall"), "--game", GAME, "--overlay",
                            os.path.join(out, "overlay"), "--nosound"], env=env,
                           capture_output=True, text=True, timeout=30 + a.seconds + 120)
        log, code = r.stdout + r.stderr, r.returncode
    except subprocess.TimeoutExpired as e:
        log, code = (e.stdout or b"").decode(errors="replace") + \
            (e.stderr or b"").decode(errors="replace"), "timeout"
    open(os.path.join(out, "log.txt"), "w").write(log)
    asan = [ln for ln in log.splitlines() if "ERROR: AddressSanitizer" in ln]
    where = []
    lines = log.splitlines()
    for i, ln in enumerate(lines):
        if "ERROR: AddressSanitizer" in ln:
            for k in lines[i + 1:i + 12]:
                if " in " in k and "#" in k:
                    where.append(k.split(" in ", 1)[1].split(" ")[0])
                    if len(where) % 3 == 0:
                        break
    crash = [ln for ln in lines if "port: crash" in ln or "port: fault" in ln or
             "Segmentation" in ln]
    return name, code, asan, sorted(set(where)), crash


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--build", default=os.path.join(ROOT, "build", "port"))
    ap.add_argument("--seconds", type=int, default=60)
    ap.add_argument("--jobs", type=int, default=3)
    ap.add_argument("--seed", type=int, default=1)
    ap.add_argument("--resize", action="store_true")
    ap.add_argument("--env", action="append", default=[])
    ap.add_argument("names", nargs="*")
    a = ap.parse_args()
    a.build = os.path.abspath(a.build)
    env_extra = dict(e.split("=", 1) for e in a.env)
    names = a.names or sorted(f[:-4] for f in os.listdir(os.path.join(ROOT, "orig", "saves"))
                              if f.endswith(".zip"))
    bad = 0
    with ThreadPoolExecutor(a.jobs) as ex:
        for name, code, asan, where, crash in ex.map(lambda n: one(n, a, env_extra, a.seed),
                                                     names):
            ok = code == 0 and not asan and not crash
            bad += not ok
            print("%-10s exit %s, %d sanitizer errors%s%s" % (
                name, code, len(asan), (" (" + ", ".join(where[:6]) + ")") if where else "",
                ("; " + crash[0]) if crash else ""), flush=True)
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
