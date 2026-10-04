#!/usr/bin/env python3
"""Coverage-guided random play: keep the game states whose random input ran new code.

Workers loop: pick a state from the corpus (the save snapshots to begin with, then whatever
they found), load it, play random input for a while (walking, turning, the keys bound in the
world, clicks, right-button swings) with coverage on, and if code ran that no worker had seen
run, add the end state to the corpus. A state's path is its parent and the events played
from it; the emulator is deterministic, so any state replays from its seed snapshot.

Needs the patched Unicorn (tools/build_unicorn.sh) for speed and coverage.

usage:
  fallfuzz.py run [-j N] [--hours H] [--max-corpus N] [--seeds NAME,...]
      N workers (default 4) for H hours (default 2), in build/fuzz; seeds default to every
      save_* and cheat_* snapshot in build/emu/snap
  fallfuzz.py report
      the corpus, and the functions the fuzzing ran (also in build/cov/fuzz_*.cov, so
      fallcov.py report counts them)
"""
import argparse
import ctypes
import glob
import json
import math
import os
import random
import subprocess
import sys
import time
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402  (first: it picks the Unicorn build)
import fallcov  # noqa: E402

ROOT = fallemu.ROOT
FUZZ = os.path.join(ROOT, "build", "fuzz")
CORPUS = os.path.join(FUZZ, "corpus")
LO, HI = fallcov.LO, fallcov.HI
SIZE = HI - LO
MIN_NEW = 8                     # new code bytes that make a state worth keeping
MAX_MB = 2000                   # a worker restarts if it ever holds more than this
MACRO_P = 0.35                  # episodes that start with a fallplay command

# Keys pressed one at a time, weighted: the screens bound in the world, answers to prompts,
# the interaction modes, movement extras (jump, crouch, run), digits for counts and slots.
KEYS = (["esc"] * 6 + ["enter"] * 4 + ["y", "n"] * 2 + ["space"] * 2 +
        ["f1", "f2", "f3", "f4", "f5", "f6", "f10", "m", "l", "i", "v", "u", "r", "t", "j",
         "c", "b", "e", "h", "a", "s", "d", "w", "g", "k", "o", "p", "q", "x", "z", "tab",
         "bksp", "del", "ins", "home", "end", "pgup", "pgdn", "1", "2", "3", "4", "5",
         "[", "]", "-", "=", "kp+", "kp-", "lshift", "f7", "f8", "f9", "f11", "f12"])
MOVES = ["up", "up", "up", "down", "left", "right"]
# Places worth clicking more often than chance: the screen centre (doors, people), dialog
# buttons (yes/no, OKAY, GOODBYE), menu columns, the HUD row of a large-HUD save.
HOT = [(165, 112), (160, 100), (128, 108), (192, 108), (60, 191), (151, 187), (161, 55),
       (161, 64), (161, 73), (161, 87), (240, 64), (240, 140), (240, 189), (295, 66),
       (45, 174), (260, 174), (297, 180), (160, 60), (283, 183), (190, 168), (210, 168),
       (230, 168), (250, 168), (270, 168), (190, 186), (210, 186), (230, 186), (250, 186)]


def episode_events(rng, length):
    """Random input for `length` ticks: [(tick, action, arg)], every key up by the end."""
    ev, t = [], rng.randint(5, 30)
    while t < length - 60:
        r = rng.random()
        if r < 0.28:                                    # walk or turn, sometimes both
            ks = [rng.choice(MOVES)] + ([rng.choice(["left", "right"])] if rng.random() < .3 else [])
            d = rng.randint(20, 300)
            for k in ks:
                ev += [(t, "down", k), (t + d, "up", k)]
            t += d + rng.randint(5, 40)
        elif r < 0.58:                                  # a key, held 20 ticks
            k = rng.choice(KEYS)
            ev += [(t, "down", k), (t + 20, "up", k)]
            t += 20 + rng.randint(20, 150)
        elif r < 0.88:                                  # a click
            x, y = rng.choice(HOT) if rng.random() < .4 else (rng.randint(0, 319), rng.randint(0, 199))
            ev += [(t, "mouse", (x, y, 0)), (t + 20, "mouse", (x, y, 1)), (t + 50, "mouse", (x, y, 0))]
            t += 50 + rng.randint(40, 150)              # apart: XnGine pairs close presses
        elif r < 0.93:                                  # a double click
            x, y = rng.choice(HOT) if rng.random() < .4 else (rng.randint(0, 319), rng.randint(0, 199))
            ev += [(t, "mouse", (x, y, 0)), (t + 20, "mouse", (x, y, 1)), (t + 26, "mouse", (x, y, 0)),
                   (t + 32, "mouse", (x, y, 1)), (t + 38, "mouse", (x, y, 0))]
            t += 40 + rng.randint(60, 150)
        elif r < 0.97:                                  # a right-button swing across the view
            x0, y0, x1, y1 = (rng.randint(60, 260), rng.randint(40, 160),
                              rng.randint(60, 260), rng.randint(40, 160))
            for i in range(6):
                ev.append((t + 4 * i, "mouse", (x0 + (x1 - x0) * i // 5, y0 + (y1 - y0) * i // 5, 2)))
            ev.append((t + 30, "mouse", (x1, y1, 0)))
            t += 30 + rng.randint(20, 100)
        else:                                           # let the game run
            t += rng.randint(50, 300)
    # clamped: a key still down at the end would stay down in the saved state
    return [(min(t, length - 10), a, g) for t, a, g in ev] + [(length - 10, "mouse", (160, 100, 0))]


def pick_macro(rng, emu):
    """A fallplay command to start an episode with, or None: the doors and walks random keys
    rarely manage. Outside: enter (or walk up to) a building, shops and halls more often than
    houses; indoors: walk, turn, or use what is ahead (a door, a person)."""
    import fallplay
    if rng.random() > MACRO_P:
        return None
    if fallplay.env(emu) == 1:
        bs = [(i, b) for i, b in enumerate(fallplay.buildings(emu)) if b["doors"] and b["dist"] < 400]
        if not bs:
            return None
        i, b = rng.choices(bs, [3 if b["type"] < 17 else 1 for _i, b in bs])[0]
        return ["enter" if rng.random() < 0.8 else "goto", str(i)]
    r = rng.random()
    if r < 0.4:
        return ["walk", str(rng.choice([2, 4, 8, 12]))]
    if r < 0.7:
        return ["turn", str(rng.choice([-135, -90, -45, 45, 90, 135, 180]))]
    return ["step", rng.choice(["1 press f2; 40 click 165,112", "1 press f4; 40 click 165,112"])]


def run_macro(emu, argv):
    """Run a fallplay command (deterministic, so the episode replays)."""
    import fallplay
    a = fallplay.build_parser().parse_args([argv[0], "fuzz"] + list(argv[1:]))
    sa = fallplay.step_action(a, None)
    try:
        sa[1](emu)
    except SystemExit:                              # e.g. no such building any more
        pass


def signature(emu):
    """The game state the sensors see: where (1 outside, 2 building, 3 dungeon), the mode
    (which screen has the input) and the interaction mode."""
    import fallplay
    return "%d/%d/%d" % (fallplay.env(emu), fallplay.rd(emu, 0x196274, "<B"),
                         fallplay.rd(emu, 0x196276, "<B"))


def read_states():
    out = set()
    for p in glob.glob(os.path.join(FUZZ, "states_*.txt")):
        out |= set(open(p).read().split())
    return out


def read_cov(path):
    try:
        data = zlib.decompress(open(path, "rb").read())
        return int.from_bytes(data.partition(b"\n")[2], "little")
    except (OSError, zlib.error):
        return 0


def write_cov(path, meta, acc):
    tmp = path + ".tmp"
    with open(tmp, "wb") as f:
        f.write(zlib.compress(json.dumps(meta).encode() + b"\n" + acc.to_bytes(SIZE, "little"), 1))
    os.replace(tmp, path)


def load_corpus(seeds):
    """[(entry)] from every worker's log, seeds first."""
    out = [{"id": "seed_" + os.path.basename(s)[:-5], "snap": s, "new": 1000, "seed": True}
           for s in seeds]
    for p in sorted(glob.glob(os.path.join(FUZZ, "corpus_*.jsonl"))):
        for line in open(p):
            try:
                e = json.loads(line)
            except ValueError:
                continue
            if e.get("snap") and os.path.exists(e["snap"]):
                out.append(e)
    return out


def worker(wid, seeds, hours, max_corpus, seed):
    lib = fallcov_lib()
    rng = random.Random(seed)
    fns = [(va, g) for va, _n, g, _k in fallcov.functions() if LO <= va < HI]
    mine = os.path.join(FUZZ, "cov_%d.bin" % wid)
    acc = read_cov(mine)
    union = acc
    for p in glob.glob(os.path.join(FUZZ, "cov_*.bin")):
        union |= read_cov(p)
    log = open(os.path.join(FUZZ, "corpus_%d.jsonl" % wid), "a")
    states = read_states()
    states_log = open(os.path.join(FUZZ, "states_%d.txt" % wid), "a")
    corpus, picks = load_corpus(seeds), {}
    t_end = time.time() + hours * 3600
    n, emu = 0, None
    for e in corpus:                                # a restarted worker numbers on
        if e["id"].startswith("w%d_" % wid):
            n = max(n, int(e["id"].split("_")[1]))
    while time.time() < t_end:
        n += 1
        if n % 10 == 0:                             # share with the other workers
            corpus = load_corpus(seeds)
            states |= read_states()
            for p in glob.glob(os.path.join(FUZZ, "cov_*.bin")):
                union |= read_cov(p)
        weights = [(1 + math.log2(1 + e.get("new", 0))) / (1 + picks.get(e["id"], 0)) ** 0.7
                   for e in corpus]
        parent = rng.choices(corpus, weights)[0]
        picks[parent["id"]] = picks.get(parent["id"], 0) + 1
        length = rng.randint(300, 1500)
        events = episode_events(rng, length)
        if emu is not None:
            emu.close()                             # ~500 MB each: never two at once
        if fallemu.rss_mb() > MAX_MB:               # a slow leak: start afresh (run restarts it)
            write_cov(mine, {"fuzz": wid}, acc)
            sys.exit(3)
        try:
            emu = fallemu.Emu.load(parent["snap"], overlay=os.path.join(FUZZ, "ov_%d" % wid))
            uch = emu.uc._uch
            lib.uc_dagger_coverage(uch, fallemu.LOAD + LO, fallemu.LOAD + HI, None)
            macro = pick_macro(rng, emu)
            if macro:
                run_macro(emu, macro)
            s = emu.ticks
            ok = emu.run(s + length, [(s + t, a, g) for t, a, g in events], None)
            buf = ctypes.create_string_buffer(SIZE)
            lib.uc_dagger_coverage(uch, fallemu.LOAD + LO, fallemu.LOAD + HI, buf)
        except Exception as e:                      # noqa: BLE001  (a bad state: drop it)
            print("worker %d: %s from %s" % (wid, e, parent["id"]), flush=True)
            continue
        cov = int.from_bytes(buf.raw, "little")
        new = cov & ~union
        acc |= cov
        union |= cov
        nb = new.bit_count()
        sig = signature(emu) if ok and emu.exit_code is None else None
        new_state = sig is not None and sig not in states
        if new_state:
            states.add(sig)
            states_log.write(sig + "\n")
            states_log.flush()
        if (nb < MIN_NEW and not new_state) or not ok or emu.exit_code is not None:
            continue
        newf = [g for va, g in fns if new >> (va - LO) & 1]
        entry = {"id": "w%d_%05d" % (wid, n), "parent": parent["id"], "new": max(nb, 64 * new_state),
                 "new_functions": len(newf), "units": sorted(set(newf)), "ticks": length,
                 "macro": macro, "events": events, "tick": emu.ticks, "state": sig,
                 "new_state": new_state, "time": time.time()}
        if len(glob.glob(os.path.join(CORPUS, "*.snap"))) < max_corpus or newf or new_state:
            entry["snap"] = os.path.join(CORPUS, entry["id"] + ".snap")
            emu.save(entry["snap"])
        log.write(json.dumps(entry) + "\n")
        log.flush()
        write_cov(mine, {"fuzz": wid}, acc)
        write_cov(os.path.join(fallcov.COV, "fuzz_%d.cov" % wid), {"fuzz": wid}, acc)
        print("worker %d ep %d: +%d bytes, +%d functions (%s)%s%s from %s" % (
            wid, n, nb, len(newf), ",".join(sorted(set(newf)))[:80],
            " new state " + sig if new_state else "", " after " + " ".join(macro) if macro else "",
            parent["id"]), flush=True)
    write_cov(mine, {"fuzz": wid}, acc)


def fallcov_lib():
    from unicorn.unicorn_py3 import unicorn as ucmod
    lib = ucmod.uclib
    if not hasattr(lib, "uc_dagger_coverage"):
        raise SystemExit("needs the patched Unicorn: tools/build_unicorn.sh")
    lib.uc_dagger_coverage.argtypes = [ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64,
                                       ctypes.c_void_p]
    return lib


def report():
    entries = load_corpus([])
    print("%d states kept (%d with a snapshot)" % (len(entries), sum(1 for e in entries if e.get("snap"))))
    acc = 0
    for p in glob.glob(os.path.join(FUZZ, "cov_*.bin")):
        acc |= read_cov(p)
    covs = sorted(glob.glob(os.path.join(fallcov.COV, "fuzz_*.cov")))
    if covs:
        fallcov.report(covs)
    by = {}
    for e in entries:
        for u in e.get("units", []):
            by[u] = by.get(u, 0) + 1
    print("units where the fuzzing found new code (states):",
          ", ".join("%s %d" % kv for kv in sorted(by.items(), key=lambda kv: -kv[1])[:30]))


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("run")
    r.add_argument("-j", type=int, default=4, help="workers (capped by memory)")
    r.add_argument("--hours", type=float, default=2.0)
    r.add_argument("--max-corpus", type=int, default=400)
    r.add_argument("--seeds", default=None)
    w = sub.add_parser("worker")
    w.add_argument("wid", type=int)
    w.add_argument("hours", type=float)
    w.add_argument("max_corpus", type=int)
    w.add_argument("seeds")
    sub.add_parser("report")
    a = ap.parse_args()
    snaps = os.path.join(ROOT, "build", "emu", "snap")
    if a.cmd == "run":
        import memwatch
        memwatch.start()                            # kills workers before memory runs out
        os.makedirs(CORPUS, exist_ok=True)
        os.makedirs(fallcov.COV, exist_ok=True)
        seeds = ([os.path.join(snaps, n + ".snap") for n in a.seeds.split(",")] if a.seeds else
                 sorted(glob.glob(os.path.join(snaps, "save_*.snap")) +
                        glob.glob(os.path.join(snaps, "cheat_*.snap"))))
        seeds = [s for s in seeds if not s.endswith("cheat_load.snap")]
        t_end = time.time() + a.hours * 3600

        def start(i):
            return subprocess.Popen([sys.executable, __file__, "worker", str(i),
                                     str(max(0.0, t_end - time.time()) / 3600), str(a.max_corpus),
                                     ",".join(seeds)])
        procs = {i: start(i) for i in range(fallemu.workers(a.j))}
        while procs:
            time.sleep(5)
            for i, p in list(procs.items()):
                if p.poll() is None:
                    continue
                if p.returncode == 3 and time.time() < t_end - 60:
                    procs[i] = start(i)             # it stopped at its memory limit
                else:
                    del procs[i]
    elif a.cmd == "worker":
        worker(a.wid, a.seeds.split(","), a.hours, a.max_corpus, a.wid * 7919 + int(time.time()))
    else:
        report()


if __name__ == "__main__":
    main()
