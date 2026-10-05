#!/usr/bin/env python3
"""Evidence for naming: what each function of FALL.EXE does, from what made it run.

An episode is a stretch of play with a label: a fallplay step (its command), a kept state of
the random play (fallfuzz.py: its keys and clicks), or a fallcov run. `collect` runs each one
again from its starting snapshot with coverage on (everything is deterministic: the replay
is the original), and keeps the functions that ran. `analyze` then gives every function:

  - how many episodes ran it, and the episode features that go with it running (keys,
    clicks by screen region, commands, indoors/outdoors), by lift: P(feature | it ran) /
    P(feature);
  - its cluster: the functions that ran in exactly the same episodes (one feature's code);
  - static evidence from the C: callers, callees, the globals it uses and the strings
    among them;
  - asset evidence (tools/fallassets.py, tools/asset_ids.py): the files it reads, the
    TEXT.RSC records and sounds it asks for, the button boxes and text macros it handles.
    The assets an episode read are features too (asset:MONSTER.BSA/ASCR0025, rsc:354,
    snd:GoldPieces).

usage:
  fallevidence.py collect [-j N] [--resume]   build/evidence/episodes.jsonl (replays, N at
      a time; tools/memwatch.py kills workers if memory runs short)
  fallevidence.py gather           episodes.jsonl again from the replays made, the direct
      calls, and the asset tokens of build/assets/reads.jsonl
  fallevidence.py analyze          build/evidence/functions.csv, clusters.md, units/UNIT.md
"""
import argparse
import collections
import csv
import glob
import json
import math
import os
import re
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "build", "evidence")
SNAPS = os.path.join(ROOT, "build", "emu", "snap")
FUZZ = os.path.join(ROOT, "build", "fuzz")
ENVS = {1: "outside", 2: "building", 3: "dungeon"}
ASSET_LINES = ("buttons", "texts", "sounds", "macros", "assets")   # fallassets.evidence()
ASSET_TOKENS = ("asset:", "rsc:", "snd:")                           # fallassets.episode_tokens()


# ---- episodes ------------------------------------------------------------------------------
def todo():
    """Every episode there is to replay: (kind, id, ...)."""
    out = []
    for p in sorted(glob.glob(os.path.join(ROOT, "build", "play", "*", "steps.json"))):
        s = os.path.basename(os.path.dirname(p))
        steps = json.load(open(p))
        for st in steps:
            if st.get("n", 0) > 0 and os.path.exists(os.path.join(os.path.dirname(p), "%03d.snap" % (st["n"] - 1))):
                out.append(("play", s, st["n"]))
    snaps = {}
    for p in sorted(glob.glob(os.path.join(FUZZ, "corpus_*.jsonl"))):
        for line in open(p):
            try:
                e = json.loads(line)
            except ValueError:
                continue
            snaps[e["id"]] = e.get("snap")
            par = e["parent"]
            ps = os.path.join(SNAPS, par[5:] + ".snap") if par.startswith("seed_") else snaps.get(par)
            if ps and os.path.exists(ps):
                out.append(("fuzz", e["id"], ps, e["events"], e["ticks"], e["tick"], e.get("macro")))
    return out


def tokens_of_events(events):
    """Features of an input list [(tick, action, arg)] or step-event text."""
    toks = set()
    if isinstance(events, str):
        for item in events.split(";"):
            w = item.split()
            if len(w) < 3:
                continue
            if w[1] in ("press", "key", "down", "hold"):
                toks.add("key:" + w[2].split("+")[-1])
                for m in w[2].split("+")[:-1]:
                    toks.add("key:" + m)
            elif w[1] in ("click", "dclick", "rclick"):
                x, y = (int(v) for v in w[2].split(",")[:2])
                toks.add("%s:%s" % ("click" if w[1] == "click" else w[1], region(x, y)))
            elif w[1] == "mouse":
                x, y, b = (int(v) for v in w[2].split(","))
                if b:
                    toks.add("%s:%s" % ("click" if b == 1 else "rbutton", region(x, y)))
        return toks
    for _t, act, arg in events:
        if act in ("down", "key"):
            toks.add("key:" + arg)
        elif act == "mouse" and arg[2]:
            toks.add("%s:%s" % ("click" if arg[2] == 1 else "rbutton", region(arg[0], arg[1])))
    return toks


def region(x, y):
    """A coarse place on the 320x200 screen: 8 x 5 cells of 40 pixels."""
    return "x%dy%d" % (min(7, max(0, x // 40)) * 40, min(4, max(0, y // 40)) * 40)


def episode_id(item):
    """The id an episode of todo() has in episodes.jsonl."""
    return item[1] if item[0] == "fuzz" else "%s/%d" % item[1:]


def replay(item, ov):
    """Run an episode of todo() again from its snapshot, with coverage on: (the machine
    after, the coverage map of fallcov.LO..HI, its record: id, kind, label, exact, tokens),
    or None if it can't be rebuilt. The caller closes the machine."""
    import ctypes
    import fallemu
    import fallplay
    import fallcov
    lib = fallplay.cov_lib()
    L, LO, HI = fallemu.LOAD, fallcov.LO, fallcov.HI
    if item[0] == "play":
        _kind, s, num = item
        r = fallplay.replay_step(s, num, ov)
        if r is None:
            return None
        cov, emu, exact, env0 = r
        st = next(x for x in fallplay.load_steps(s) if x["n"] == num)
        argv = (fallplay.argv_of(st, s) or [[]])[0]
        first = fallplay.load_steps(s)[0].get("from", "")
        toks = {"cmd:" + (argv[0] if argv else "?"), "from:" + first}
        if argv and argv[0] == "step":
            toks |= tokens_of_events(argv[-1])
        rec = {"id": episode_id(item), "kind": "play", "label": st.get("events", ""), "exact": exact}
    else:
        _kind, eid, snap, events, ticks, tick, macro = item
        emu = fallemu.Emu.load(snap, overlay=ov)
        lib.uc_dagger_coverage(emu.uc._uch, L + LO, L + HI, None)
        env0 = fallplay.env(emu)
        try:
            if macro:
                import fallfuzz
                fallfuzz.run_macro(emu, macro)
            s0 = emu.ticks
            emu.run(s0 + ticks, [(s0 + t, a, tuple(g) if isinstance(g, list) else g)
                                 for t, a, g in events], None)
        except BaseException:
            emu.close()                             # no machine left behind
            raise
        buf = ctypes.create_string_buffer(HI - LO)
        lib.uc_dagger_coverage(emu.uc._uch, L + LO, L + HI, buf)
        cov = buf.raw
        toks = tokens_of_events([(t, a, g) for t, a, g in events]) | {"cmd:fuzz"}
        if macro:
            toks.add("cmd:" + macro[0])
        rec = {"id": eid, "kind": "fuzz", "label": "random input from " + os.path.basename(snap),
               "exact": emu.ticks == tick}
    env1 = fallplay.env(emu)
    toks |= {"env0:" + ENVS.get(env0, str(env0)), "env1:" + ENVS.get(env1, str(env1))}
    rec["tokens"] = sorted(toks)
    return emu, cov, rec


def work(k, n):
    """Replay the episodes i with i % n == k; append them to build/evidence/part_K.jsonl."""
    import fallemu
    import fallcov
    fns = [va for va, _nm, _g, _k in fallcov.functions() if fallcov.LO <= va < fallcov.HI]
    ov = os.path.join(OUT, "ov_%d" % k)
    part = os.path.join(OUT, "part_%d.jsonl" % k)
    done = {json.loads(line)["id"] for line in open(part)} if os.path.exists(part) else set()
    out = open(part, "a")                           # a restarted worker carries on
    emu = None
    for i, item in enumerate(todo()):
        if i % n != k:
            continue
        if emu is not None:
            emu.close()                             # ~500 MB each: never two at once
            emu = None
        if fallemu.rss_mb() > 2000:                 # a slow leak: start afresh (collect restarts it)
            sys.exit(3)
        if episode_id(item) in done:
            continue
        try:
            r = replay(item, ov)
            if r is None:
                continue
            emu, cov, rec = r
            rec["functions"] = ["%X" % va for va in fns if cov[va - fallcov.LO]]
            out.write(json.dumps(rec) + "\n")
            out.flush()
        except Exception as e:                      # noqa: BLE001  (report, carry on)
            print("episode %s: %s" % (item[:3], e), flush=True)


def collect(j, resume=False):
    import memwatch
    memwatch.start()                                # kills workers before memory runs out
    os.makedirs(OUT, exist_ok=True)
    for p in glob.glob(os.path.join(OUT, "part_*.jsonl")):
        if not resume:
            os.remove(p)
    print("%d episodes to replay" % len(todo()), flush=True)
    procs = {k: subprocess.Popen([sys.executable, __file__, "work", str(k), str(j)]) for k in range(j)}
    if j == 0:                                      # gather: nothing to replay
        procs = {}
    while procs:
        for k, p in list(procs.items()):
            r = p.wait()
            if r == 3:                              # stopped at its memory limit: carry on
                procs[k] = subprocess.Popen([sys.executable, __file__, "work", str(k), str(j)])
            else:
                if r:
                    print("worker %d ended (%d): `collect --resume` replays what it missed" % (k, r))
                del procs[k]
    import fallcov
    eps = []
    for p in sorted(glob.glob(os.path.join(OUT, "part_*.jsonl"))):
        eps += [json.loads(line) for line in open(p)]
    fns = fallcov.functions()
    for p in sorted(glob.glob(os.path.join(fallcov.COV, "*.cov"))):  # the batch runs
        base = os.path.basename(p)[:-4]
        if base.startswith(("play_", "fuzz_", "call_")):
            continue
        _meta, m = fallcov.load_cov(p)
        eps.append({"id": "cov/" + base, "kind": "explore", "exact": True,
                    "label": "fallcov EXPLORE script from save_" + base,
                    "tokens": ["cmd:explore", "from:save_" + base],
                    "functions": ["%X" % va for va, _n, _g, _k in fns
                                  if fallcov.LO <= va < fallcov.HI and m[va - fallcov.LO]]})
    for p in sorted(glob.glob(os.path.join(ROOT, "build", "call*", "sweep_*.jsonl"))):  # direct calls
        tag = os.path.basename(os.path.dirname(p))[4:].lstrip("_")
        for line in open(p):
            r = json.loads(line)
            if tag:
                r["va"] = r["va"] + "/" + tag
            if not r.get("ran"):
                continue
            toks = {"cmd:call", "called:" + r["va"]} | {
                "file:" + os.path.basename(x.replace("\\", "/")).lower() for x in r.get("files", [])}
            if r.get("shot"):
                toks.add("drew")
            if r.get("returned") is False:
                toks.add("no-return")
            eps.append({"id": "call/" + r["va"], "kind": "call", "exact": True,
                        "label": "direct call of func_%s(%s)%s" % (
                            r["va"].zfill(8), ", ".join("%X" % x for x in r.get("args", [])),
                            ", drew " + r["shot"] if r.get("shot") else ""),
                        "tokens": sorted(toks), "functions": r["ran"]})
    import fallassets
    at = fallassets.episode_tokens()                # what the episode read (fallassets collect)
    for e in eps:
        if at.get(e["id"]):
            e["tokens"] = sorted(set(e["tokens"]) | at[e["id"]])
    with open(os.path.join(OUT, "episodes.jsonl"), "w") as f:
        for e in eps:
            f.write(json.dumps(e) + "\n")
    print("%d episodes (%d play, %d fuzz, %d explore, %d direct calls), %d not exact replays, "
          "%d with asset tokens" % (
              len(eps), sum(e["kind"] == "play" for e in eps), sum(e["kind"] == "fuzz" for e in eps),
              sum(e["kind"] == "explore" for e in eps), sum(e["kind"] == "call" for e in eps),
              sum(not e["exact"] for e in eps), sum(1 for e in eps if at.get(e["id"]))))


# ---- static evidence -------------------------------------------------------------------------
DEF = re.compile(r"^[A-Za-z_][\w \*]*?\b(func_[0-9A-F]{8})\s*\(([^;]*)$")


def static():
    """{va: {"calls": set, "globals": set}} from every C file of the game."""
    import names as namesmod
    out = {}
    files = glob.glob(os.path.join(ROOT, "src", "*.c")) + glob.glob(os.path.join(ROOT, "src", "lifted", "*.c")) + \
        glob.glob(os.path.join(ROOT, "src", "hand", "*.c"))
    for p in files:
        cur = None
        for line in namesmod.canonical(open(p, errors="replace").read()).splitlines(True):
            m = DEF.match(line)
            if m and not line.startswith("extern"):
                cur = int(m.group(1)[5:], 16)
                out.setdefault(cur, {"calls": set(), "globals": set()})
                continue
            if cur is None or line.startswith(("extern", "#pragma")):
                continue
            for c in re.findall(r"\bfunc_([0-9A-F]{8})\b", line):
                out[cur]["calls"].add(int(c, 16))
            for g in re.findall(r"\bD_([0-9A-F]{8})\b", line):
                out[cur]["globals"].add(int(g, 16))
    return out


def strings_at(addrs):
    """{address: text} for the globals that hold a C string (read from a snapshot)."""
    import fallemu
    emu = fallemu.Emu.load(os.path.join(SNAPS, "save_tlalac_s.snap"),
                           overlay=os.path.join(OUT, "ov_strings"))
    out = {}
    for a in addrs:
        try:
            b = bytes(emu.uc.mem_read(fallemu.LOAD + a, 96))
        except Exception:                           # noqa: BLE001
            continue
        s = b.split(b"\0")[0]
        if len(s) >= 3 and all(32 <= c < 127 or c in (9, 10, 13) for c in s) and \
                sum(chr(c).isalpha() for c in s) >= 2:
            out[a] = s.decode("latin-1").replace("\n", "\\n").replace("\r", "\\r")
    emu.close()
    return out


# ---- analysis --------------------------------------------------------------------------------
def called_text(e):
    """One line on what a direct call did (an episode of kind call), or ''."""
    if not e:
        return ""
    files = [x[5:] for x in e["tokens"] if x.startswith("file:")]
    return "%s; ran %d functions%s%s" % (
        "did not return" if "no-return" in e["tokens"] else "returned", len(e["functions"]),
        "; files " + " ".join(files) if files else "",
        "; drew " + e["label"].split(", drew ")[1] if ", drew " in e["label"] else "")


def analyze():
    import fallcov
    import names as namesmod
    known = namesmod.by_address()
    alleps = [json.loads(line) for line in open(os.path.join(OUT, "episodes.jsonl"))]
    # direct calls (fallcall sweep) say what a function does when called, not what play
    # reaches: they are kept apart from the played episodes
    calls = {e["id"][5:]: e for e in alleps if e["kind"] == "call"}
    called_in = collections.Counter(f for e in calls.values() for f in e["functions"])
    eps = [e for e in alleps if e["kind"] != "call"]
    N = len(eps)
    fns = fallcov.functions()
    info = {va: (name, group, kind) for va, name, group, kind in fns}
    ran = collections.defaultdict(set)              # va -> episode indices
    for i, e in enumerate(eps):
        for f in e["functions"]:
            ran[int(f, 16)].add(i)
    tokn = collections.Counter(t for e in eps for t in e["tokens"])
    st = static()
    callers = collections.defaultdict(set)
    for va, d in st.items():
        for c in d["calls"]:
            callers[c].add(va)
    strs = strings_at(sorted({g for d in st.values() for g in d["globals"]}))
    import fallassets
    ev = fallassets.evidence()

    # clusters: functions run in exactly the same episodes
    groups = collections.defaultdict(list)
    for va, s in ran.items():
        groups[frozenset(s)].append(va)
    clusters = sorted(groups.items(), key=lambda kv: (-len(kv[1]), len(kv[0])))
    cid = {}
    for k, (s, vas) in enumerate(clusters):
        for va in vas:
            cid[va] = k

    def assoc(s, asset=False):
        """The features that go with running in episode set s: (token, n, lift), best first.
        The inputs, or (asset=True) the assets read: kept apart, as rare assets would
        outrank every key and click."""
        c = collections.Counter(t for i in s for t in eps[i]["tokens"])
        out = []
        for t, n in c.items():
            lift = (n / len(s)) / (tokn[t] / N)
            if n >= 2 and lift >= 1.5 and not t.startswith("from:") and t.startswith(ASSET_TOKENS) == asset:
                out.append((t, n, lift))
        out.sort(key=lambda x: -(math.log(x[2]) * min(x[1], 8)))
        return out[:6]

    def feats(s, asset=False):
        if not s or len(s) >= N * 0.8:
            return "always" if s and not asset else ""
        return "; ".join("%s (%d, x%.1f)" % x for x in assoc(s, asset))

    def examples(s, k=4):
        """Labels of a few episodes: play steps first (they have commands), shortest set."""
        idx = sorted(s, key=lambda i: (eps[i]["kind"] != "play", len(eps[i]["functions"])))
        return ["%s: %s" % (eps[i]["id"], eps[i]["label"][:70]) for i in idx[:k]]

    os.makedirs(os.path.join(OUT, "units"), exist_ok=True)
    rows = []
    for va, (name, group, kind) in sorted(info.items()):
        s = ran.get(va, set())
        d = st.get(va, {"calls": set(), "globals": set()})
        rows.append({
            "va": "0x%08X" % va, "name": known.get(name, name), "unit": group, "kind": kind,
            "episodes": len(s), "share": "%.3f" % (len(s) / N),
            "calls": called_in.get("%X" % va, 0),
            "called": called_text(calls.get("%X" % va)),
            "called with the player entity": called_text(calls.get("%X/entity" % va)),
            "cluster": cid.get(va, ""),
            "features": feats(s),
            "asset features": feats(s, True),
            "examples": " | ".join(examples(s)) if s and len(s) < N * 0.8 else "",
            "callers": " ".join(sorted(known.get("func_%08X" % c, "%X" % c) for c in callers.get(va, ())))[:300],
            "callees": " ".join(sorted(known.get("func_%08X" % c, "%X" % c) for c in d["calls"]))[:300],
            "strings": " | ".join(repr(strs[g])[1:-1][:60] for g in sorted(d["globals"]) if g in strs)[:400],
        })
        rows[-1].update({k: ev.get(va, {}).get(k, "") for k in ASSET_LINES})
    with open(os.path.join(OUT, "functions.csv"), "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]))
        w.writeheader()
        w.writerows(rows)

    with open(os.path.join(OUT, "clusters.md"), "w") as f:
        f.write("# Clusters: functions that ran in exactly the same episodes\n\n"
                "%d episodes. Largest clusters first; a cluster run by nearly every episode is "
                "the game's core loop.\n\n" % N)
        for k, (s, vas) in enumerate(clusters):
            if len(vas) < 2 and len(s) > 3:
                continue
            units = collections.Counter(info[v][1] for v in vas if v in info)
            f.write("## cluster %d: %d functions, %d episodes\n\n" % (k, len(vas), len(s)))
            f.write("units: %s\n\n" % ", ".join("%s %d" % kv for kv in units.most_common(8)))
            if len(s) < N * 0.8:
                f.write("features: %s\n\n" % feats(s))
                if feats(s, True):
                    f.write("asset features: %s\n\n" % feats(s, True))
                f.write("examples:\n%s\n\n" % "\n".join("- " + x for x in examples(s, 6)))
            f.write("functions: %s\n\n" % " ".join(info[v][0] if v in info else "%X" % v for v in sorted(vas)))

    byunit = collections.defaultdict(list)
    for r in rows:
        byunit[r["unit"]].append(r)
    for u, rs in byunit.items():
        with open(os.path.join(OUT, "units", "%s.md" % u.replace("/", "_")), "w") as f:
            hit = sum(1 for r in rs if r["episodes"])
            f.write("# %s: %d functions, %d ran in play (episodes); `called` is what a direct call "
                    "with all-zero arguments did (tools/fallcall.py sweep)\n\n" % (u, len(rs), hit))
            f.write("`asset features`: like `features`, for what the episodes read (asset:FILE/RECORD, "
                    "rsc:TEXT.RSC id, snd:sound); "
                    "`buttons`: the button-table entries (table[index] and screen box) it handles; "
                    "`texts`, `sounds`: TEXT.RSC records and DAGGER.SND sounds it asks for, by "
                    "constant ids at its calls (tools/asset_ids.py) and in the traced episodes "
                    "(\"in play\"); `macros`: text macros it expands; `assets`: files read within "
                    "3 calls of it in the traced episodes and save loads (tools/fallassets.py), "
                    "[n] = calls between it and the read.\n\n")
            for r in rs:
                f.write("## %s (%s episodes, cluster %s)\n\n" % (r["name"], r["episodes"], r["cluster"]))
                for k in ("features", "asset features", "examples", "called", "called with the player entity",
                          "buttons", "texts", "sounds", "macros", "assets", "strings", "callers",
                          "callees"):
                    if r[k]:
                        f.write("- %s: %s\n" % (k, r[k]))
                f.write("\n")
    print("%d episodes; %d functions ran in some episode; %d clusters; build/evidence/" % (
        N, len(ran), len(clusters)))


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    c = sub.add_parser("collect")
    c.add_argument("-j", type=int, default=None, help="workers (default and cap: by cores and memory)")
    c.add_argument("--resume", action="store_true", help="keep the episodes already replayed")
    w = sub.add_parser("work")
    w.add_argument("k", type=int)
    w.add_argument("n", type=int)
    sub.add_parser("analyze")
    sub.add_parser("gather", help="rebuild episodes.jsonl from the replays already made, plus calls")
    a = ap.parse_args()
    if a.cmd == "collect":
        import fallemu
        collect(fallemu.workers(a.j), a.resume)
    elif a.cmd == "gather":
        collect(0, resume=True)
    elif a.cmd == "work":
        work(a.k, a.n)
    else:
        analyze()


if __name__ == "__main__":
    main()
