#!/usr/bin/env python3
"""Asset tracing: which code reads which game data (docs/asset_tracing.md).

Every DOS read the game makes is logged with the guest call chain that asked for it and
named by the record it falls in (tools/assets.py): MONSTER.BSA/ASCR0025.ANC, TEXT.RSC/354,
TEXTURE.280. The chain comes from a validated stack walk (vstack): a return address on the
guest stack counts only if it follows a call into the function of the frame inside it, so the
stale return addresses that the int 21h's frameless library and XnGine code leave about are
dropped. A sound is read once and then cached, so every sound the game asks for is caught at
its loader instead (objlib.c func_00085A51, the DAGGER.SND record id in EAX). Tracing costs
about 15%.

usage:
  fallassets.py collect [-j N] [--resume]   replay every fallevidence episode (play steps and
      fuzz states, as `fallevidence.py collect` does) with reads traced:
      build/assets/reads.jsonl, one line per episode (then `fallevidence.py gather` adds
      the asset tokens to the episodes)
  fallassets.py loads [-j N] [--resume]     load each classic save (orig/saves/*.zip) from
      the load screen (lt_load.snap) with reads traced: build/assets/loads.jsonl
  fallassets.py report                      build/assets/report.md: per asset class, the
      loader APIs and the consumer chains; per function, the assets it reads, directly or
      within 3 calls
"""
import argparse
import bisect
import collections
import csv
import glob
import json
import os
import struct
import subprocess
import sys
import zipfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402  (first: it picks the Unicorn build)
import assets  # noqa: E402
from unicorn import UC_HOOK_CODE  # noqa: E402

ROOT = fallemu.ROOT
OUT = os.path.join(ROOT, "build", "assets")
L = fallemu.LOAD
CODE = (0x10000, 0x161568)              # objects 1 and 2
SOUND = 0x85A51                         # objlib.c: load a sound by DAGGER.SND record id
DEPTH = 3                               # a function reads what is read within 3 calls of it
LOAD_SCRIPT = "5 mouse 80,28,0; 10 click 80,28; 120 click 160,10"   # SAVE0, LOAD GAME
LOAD_TICKS = 2500

_FN = None
_VAS = None


# ---- functions and the stack walk ----------------------------------------------------------
def funcs():
    """Sorted [(va, end, name, group)]: game C by unit, the library, XnGine by module."""
    global _FN
    if _FN is not None:
        return _FN
    from units import load_units, unit_of, GAME_END
    units = load_units()
    out = []
    with open(os.path.join(ROOT, "config", "xngine_modules.csv"), newline="") as f:
        mods = [(int(r["start"], 16), int(r["end"], 16), r["name"]) for r in csv.DictReader(f)]
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        for r in csv.DictReader(f):
            va = int(r["va"], 16)
            if va >= 0xC0000:
                continue                    # object 2: from xngine_functions.csv
            g = unit_of(va, units)[0] if va < GAME_END else "library"
            out.append((va, va + int(r["span"] or r["size"] or 1), r["name"], g or "?"))
    with open(os.path.join(ROOT, "config", "xngine_functions.csv"), newline="") as f:
        xs = sorted(int(r["va"], 16) for r in csv.DictReader(f) if r["found_by"] != "run time")
    for i, va in enumerate(xs):
        end = xs[i + 1] if i + 1 < len(xs) else CODE[1]
        out.append((va, end, "func_%08X" % va, next((n for s, e, n in mods if s <= va < e), "xngine")))
    _FN = sorted(out)
    return _FN


def func_of(va):
    """(name, group, start) of the function holding va, or None."""
    global _VAS
    fn = funcs()
    if _VAS is None:
        _VAS = [f[0] for f in fn]
    i = bisect.bisect_right(_VAS, va) - 1
    if i >= 0 and fn[i][0] <= va < fn[i][1] + 16:
        return fn[i][2], fn[i][3], fn[i][0]
    return None


def group_of(va):
    f = func_of(va)
    return f[1] if f else "?"


def is_ret(emu, va):
    """Does va (a preferred address) follow a CALL instruction?"""
    if not CODE[0] + 7 <= va < CODE[1]:
        return False
    b = emu.read(L + va - 7, 7)
    if b[2] == 0xE8:                        # call rel32
        return True
    for k in (2, 3, 4, 6, 7):               # call r/m32: FF /2
        if b[7 - k] == 0xFF and (b[8 - k] >> 3) & 7 == 2:
            mod, rm = b[8 - k] >> 6, b[8 - k] & 7
            n = 2 + (rm == 4 and mod != 3) + {0: 4 if rm == 5 else 0, 1: 1, 2: 4, 3: 0}[mod]
            if n == k:
                return True
    return False


def call_target(emu, ret):
    """The target of the direct `call rel32` ending at ret, else None (an indirect call)."""
    b = emu.read(L + ret - 5, 5)
    if b[0] == 0xE8:
        return (ret + struct.unpack_from("<i", b, 1)[0]) & 0xFFFFFFFF
    return None


def vstack(emu, depth=10, scan=1024):
    """The return addresses on the guest stack, innermost first, each checked to be a call
    into the function of the frame inside it (starting from the function at EIP); a call
    through a pointer, or through a jmp thunk to that function, is taken on trust. Stale
    return addresses left by earlier calls fail the check."""
    raw = emu.read(emu.lin(emu.r("ss"), emu.r("esp")), 4 * scan)
    cur = func_of(emu.r("eip") - L)
    out = []
    for i in range(scan):
        v = struct.unpack_from("<I", raw, 4 * i)[0] - L
        if not is_ret(emu, v):
            continue
        t = call_target(emu, v)
        if t is not None and cur is not None:
            tf = func_of(t)
            if tf is None or tf[0] != cur[0]:
                b = emu.read(L + t, 5) if CODE[0] <= t < CODE[1] else b""
                if not (b[:1] == b"\xe9" and func_of((t + 5 + struct.unpack_from("<i", b, 1)[0])
                                                    & 0xFFFFFFFF) == cur):
                    continue
        out.append(v)
        cur = func_of(v)
        if len(out) >= depth:
            break
    return out


def chain(emu, depth=10):
    """The game and XnGine functions on the call chain now, innermost first, as hex
    addresses: library frames and repeats dropped."""
    out = []
    for v in [emu.r("eip") - L] + vstack(emu, depth):
        f = func_of(v)
        if f is None or f[1] == "library":
            continue
        a = "%08X" % f[2]
        if not out or out[-1] != a:
            out.append(a)
    return out


class Trace:
    """Log a machine's DOS reads (int 21h AH=3Fh) and the sounds it asks for, each with its
    call chain: self.reads [{file, pos, n, tick, chain}], self.sounds [{id, tick, chain}]."""

    def __init__(self, emu):
        self.emu = emu
        self.reads, self.sounds = [], []
        orig = emu.int21

        def int21():
            if emu.r("ah") != 0x3F:
                return orig()
            fh = emu.files.handles.get(emu.r("bx"))
            ev = {"file": assets.base(fh.name) if fh else None, "pos": fh.tell() if fh else None,
                  "tick": emu.ticks, "chain": chain(emu)}
            orig()
            ev["n"] = 0 if emu.r("eflags") & 1 else emu.r("eax")
            if ev["n"] and ev["file"]:
                self.reads.append(ev)
        emu.int21 = int21

        def sound(uc, address, size, _):
            self.sounds.append({"id": emu.r("eax"), "tick": emu.ticks, "chain": chain(emu)})
        emu.uc.hook_add(UC_HOOK_CODE, sound, None, L + SOUND, L + SOUND)

    def record(self):
        """The episode's reads, named and merged (a run of reads straight through one record
        by the same chain is one entry, with k reads), and its sounds."""
        reads = []
        for e in self.reads:
            rec = assets.record(e["file"], e["pos"])
            p = reads[-1] if reads else None
            if p and p["file"] == e["file"] and p["rec"] == rec and p["chain"] == e["chain"] \
                    and p["pos"] + p["n"] == e["pos"]:
                p["n"] += e["n"]
                p["k"] += 1
                continue
            reads.append({"file": e["file"], "rec": rec, "pos": e["pos"], "n": e["n"], "k": 1,
                          "tick": e["tick"], "chain": e["chain"]})
        return {"reads": reads, "sounds": [dict(s, name=assets.sound_name(s["id"])) for s in self.sounds]}


# ---- replays -----------------------------------------------------------------------------
def saves():
    return sorted(os.path.basename(p)[:-4] for p in glob.glob(os.path.join(ROOT, "orig", "saves", "*.zip")))


def todo(what):
    import fallevidence
    return fallevidence.todo() if what == "reads" else [("load", s) for s in saves()]


def load_save(name, ov):
    """Load a classic save from the load screen: its files go in as SAVE0, then SAVE0 is
    clicked and LOAD GAME. Returns the machine LOAD_TICKS later."""
    emu = fallemu.Emu.load(os.path.join(fallemu.SNAPS, "lt_load.snap"), overlay=ov)
    d = os.path.join(ov, "SAVE0")
    os.makedirs(d, exist_ok=True)
    with zipfile.ZipFile(os.path.join(ROOT, "orig", "saves", name + ".zip")) as z:
        for n in z.namelist():
            if not n.endswith("/"):
                with open(os.path.join(d, os.path.basename(n).upper()), "wb") as f:
                    f.write(z.read(n))
    s = emu.ticks
    try:
        emu.run(s + LOAD_TICKS, [(s + t, a, g) for t, a, g in fallemu.parse_script(LOAD_SCRIPT)])
    except BaseException:
        emu.close()
        raise
    return emu


def work(what, k, n):
    """Trace the items i with i % n == k of todo(what); append to build/assets/WHAT_K.jsonl."""
    import fallevidence
    import fallplay
    ov = os.path.join(OUT, "ov_%d" % k)
    part = os.path.join(OUT, "%s_%d.jsonl" % (what, k))
    done = {json.loads(line)["id"] for line in open(part)} if os.path.exists(part) else set()
    out = open(part, "a")                           # a restarted worker carries on
    traces = []
    fallemu.LOAD_HOOKS.append(lambda emu: traces.append(Trace(emu)))
    emu = None
    for i, item in enumerate(todo(what)):
        if i % n != k:
            continue
        if emu is not None:
            emu.close()                             # ~500 MB each: never two at once
            emu = None
        if fallemu.rss_mb() > 2000:                 # a slow leak: start afresh (collect restarts it)
            sys.exit(3)
        eid = "load/" + item[1] if what == "loads" else fallevidence.episode_id(item)
        if eid in done:
            continue
        del traces[:]
        try:
            if what == "loads":
                emu = load_save(item[1], ov)
                rec = {"id": eid, "kind": "load", "env": fallplay.env(emu)}
            else:
                r = fallevidence.replay(item, ov)
                if r is None:
                    continue
                emu, _cov, ep = r
                rec = {"id": eid, "kind": ep["kind"], "exact": ep["exact"]}
            rec.update(next(t for t in traces if t.emu is emu).record())
            out.write(json.dumps(rec) + "\n")
            out.flush()
        except Exception as e:                      # noqa: BLE001  (report, carry on)
            print("%s: %s" % (eid, e), flush=True)
    if emu is not None:
        emu.close()


def collect(what, j, resume=False):
    import memwatch
    memwatch.start()                                # kills workers before memory runs out
    os.makedirs(OUT, exist_ok=True)
    if not resume:
        for p in glob.glob(os.path.join(OUT, "%s_*.jsonl" % what)):
            os.remove(p)
    print("%d to trace, %d workers" % (len(todo(what)), j), flush=True)
    cmd = [sys.executable, __file__, "work", what]
    procs = {k: subprocess.Popen(cmd + [str(k), str(j)]) for k in range(j)}
    while procs:
        for k, p in list(procs.items()):
            r = p.wait()
            if r == 3:                              # stopped at its memory limit: carry on
                procs[k] = subprocess.Popen(cmd + [str(k), str(j)])
            else:
                if r:
                    print("worker %d ended (%d): `%s --resume` traces what it missed" % (
                        k, r, "collect" if what == "reads" else "loads"))
                del procs[k]
    rows = []
    for p in sorted(glob.glob(os.path.join(OUT, "%s_*.jsonl" % what))):
        rows += [json.loads(line) for line in open(p)]
    with open(os.path.join(OUT, what + ".jsonl"), "w") as f:
        for r in rows:
            f.write(json.dumps(r) + "\n")
    print("%d traced, %d reads, %d sounds: build/assets/%s.jsonl" % (
        len(rows), sum(len(r["reads"]) for r in rows), sum(len(r["sounds"]) for r in rows), what))


# ---- evidence ----------------------------------------------------------------------------
def traced():
    """Every traced episode and save load (build/assets/reads.jsonl, loads.jsonl)."""
    out = []
    for w in ("reads", "loads"):
        p = os.path.join(OUT, w + ".jsonl")
        if os.path.exists(p):
            out += [json.loads(line) for line in open(p)]
    return out


def episode_tokens():
    """{episode id: set of features}: asset:MONSTER.BSA/ASCR0025, rsc:354, snd:GoldPieces, for
    the fallevidence episodes (`fallevidence.py gather` adds them)."""
    out = {}
    p = os.path.join(OUT, "reads.jsonl")
    for line in (open(p) if os.path.exists(p) else ()):
        r = json.loads(line)
        t = {assets.token(x["file"], x["rec"]) for x in r["reads"]
             if x["file"] != "DAGGER.SND" and not (x["file"] == "TEXT.RSC" and x["rec"] is None)}
        t |= {assets.token("DAGGER.SND", s["id"]) for s in r["sounds"]}
        out[r["id"]] = t
    return out


def _apis():
    import asset_ids
    a = asset_ids.apis()
    return set(a["TEXT.RSC"]), set(a["DAGGER.SND"]), a


def shown_by(ch, skip):
    """The function a text or sound is for: the first in the chain that isn't an API."""
    return next((int(c, 16) for c in ch if int(c, 16) not in skip), None)


def per_function(eps=None):
    """{va: {"assets": {(file, record): (depth, reads)}, "texts": Counter(id), "sounds":
    Counter(id)}} from the traces: the files read within DEPTH calls of each function (TEXT.RSC
    and DAGGER.SND as texts and sounds, for the first function on the chain that isn't a text
    or sound API)."""
    eps = traced() if eps is None else eps
    text_api, sound_api, _ = _apis()
    out = collections.defaultdict(lambda: {"assets": {}, "texts": collections.Counter(),
                                           "sounds": collections.Counter()})
    for r in eps:
        for x in r["reads"]:
            if x["file"] == "DAGGER.SND":
                continue
            if x["file"] == "TEXT.RSC":
                if x["rec"] is not None:
                    va = shown_by(x["chain"], text_api)
                    if va is not None:
                        out[va]["texts"][x["rec"]] += 1
                continue
            for d, c in enumerate(x["chain"][:DEPTH + 1]):
                a = out[int(c, 16)]["assets"]
                key = (x["file"], x["rec"])
                old = a.get(key, (d, 0))
                a[key] = (min(old[0], d), old[1] + x["k"])
        for s in r["sounds"]:
            va = shown_by(s["chain"], sound_api)
            if va is not None:
                out[va]["sounds"][s["id"]] += 1
    return out


def assets_text(a, width=400, per=6):
    """One line for a function's assets, by asset class, nearest first: the records (of an
    archive) or the files (of a class like *.IMG) and, in brackets, the depth: the calls
    between the function and the read (0: it reads the file itself)."""
    by = collections.defaultdict(lambda: [99, 0, set()])
    for (f, rec), (d, n) in a.items():
        cls = assets.asset_class(f)
        b = by[cls]
        b[0], b[1] = min(b[0], d), b[1] + n
        if f.startswith("TEXTURE."):
            b[2].add("%s %s" % (f[8:], assets.texture(f)[0] or "?"))
        elif f.endswith((".BSA", ".SND", ".RSC")):
            if rec is not None:
                b[2].add(str(rec))
        elif cls != f:
            b[2].add(f)
    parts = []
    for cls, (d, n, items) in sorted(by.items(), key=lambda kv: (kv[1][0], -kv[1][1])):
        xs = sorted(items, key=lambda x: (0, int(x), "") if x.isdigit() else (1, 0, x))
        parts.append("%s%s [%d]" % (cls, ": " + ", ".join(xs[:per]) + (
            " +%d more" % (len(xs) - per) if len(xs) > per else "") if xs else "", d))
    out = "; ".join(parts)
    return out if len(out) <= width else out[:width - 3] + "..."


def evidence():
    """{va: {line: text}} for fallevidence's pages: assets (traced reads), texts and sounds
    (constant ids at API calls, asset_ids.py, and those seen in play), buttons (the boxes of
    button-table entries a handler serves) and macros (text macros it expands)."""
    import asset_ids
    import names as namesmod
    known = namesmod.by_address()
    text_api, sound_api, api = _apis()
    out = collections.defaultdict(dict)
    pf = per_function()
    static = collections.defaultdict(lambda: collections.defaultdict(list))
    for va, _api, asset, rid, mean in asset_ids.ids():
        static[va][asset].append((rid, mean))
    for va in set(pf) | set(static) | set(api["TEXT.RSC"]) | set(api["DAGGER.SND"]):
        p = pf.get(va)
        if p and p["assets"]:
            out[va]["assets"] = assets_text(p["assets"])
        for line, asset, apiset in (("texts", "TEXT.RSC", text_api), ("sounds", "DAGGER.SND", sound_api)):
            parts = []
            if va in apiset:
                parts.append("API: argument %s is a %s record id" % (" or ".join(
                    "%d%s" % (i + 1, " %+d" % o if o else "") for i, o in sorted(api[asset][va])), asset))
            ids = [x for x, _m in static[va][asset]]
            parts += ['%s "%s"' % (x, m[:60]) if asset == "TEXT.RSC" else "%s %s" % (x, m)
                      for x, m in static[va][asset]]
            seen = p[line] if p else {}
            new = [x for x in sorted(seen) if str(x) not in ids]
            if new:
                parts.append("in play: " + "; ".join(
                    '%d "%s"' % (x, (assets.rsc_text(x, 60) or "")) if asset == "TEXT.RSC" else
                    "%d %s" % (x, assets.sound_name(x)) for x in new))
            if parts:
                s = "; ".join(parts)
                out[va][line] = s if len(s) <= 600 else s[:597] + "..."
    bt = collections.defaultdict(list)
    for t, i, b, h in asset_ids.buttons():
        if h:
            bt[h].append("%s[%d] (%d,%d)-(%d,%d)" % ((known.get("D_%08X" % t, "D_%08X" % t), i) + b))
    for h, xs in bt.items():
        out[h]["buttons"] = "; ".join(xs[:8]) + (" +%d more" % (len(xs) - 8) if len(xs) > 8 else "")
    mc = collections.defaultdict(list)
    for _t, _i, n, h in asset_ids.macros():
        if h:
            mc[h].append("%" + n)
    for h, xs in mc.items():
        out[h]["macros"] = " ".join(xs)
    return out


def report():
    eps = traced()
    import names as namesmod
    known = namesmod.by_address()
    reads = [(r, x) for r in eps for x in r["reads"]]
    text_api, sound_api, _ = _apis()
    with open(os.path.join(OUT, "report.md"), "w") as f:
        f.write("# Asset reads by code\n\n%d traced runs (%d episodes, %d save loads), %d reads "
                "(%d bytes), %d sounds asked for. Chains are innermost first, library frames "
                "left out; `[unit]` is the function's unit or XnGine module.\n\n" % (
                    len(eps), sum(r["kind"] != "load" for r in eps), sum(r["kind"] == "load" for r in eps),
                    len(reads), sum(x["n"] for _r, x in reads), sum(len(r["sounds"]) for r in eps)))

        def name(c):
            n = known.get("func_" + c, c)
            return "%s[%s]" % (n, group_of(int(c, 16)).replace(".c", ""))
        f.write("## Asset classes\n\n")
        by = collections.defaultdict(list)
        for r, x in reads:
            by[assets.asset_class(x["file"])].append((r, x))
        for cls, rs in sorted(by.items(), key=lambda kv: -len(kv[1])):
            files = collections.Counter(assets.label(x["file"], x["rec"]) for _r, x in rs)
            loaders = collections.Counter(x["chain"][0] for _r, x in rs if x["chain"])
            chains = collections.Counter(" <- ".join(name(c) for c in x["chain"][:5]) for _r, x in rs)
            f.write("### %s: %d reads, %d bytes, %d distinct, in %d runs\n\n" % (
                cls, sum(x["k"] for _r, x in rs), sum(x["n"] for _r, x in rs), len(files),
                len({r["id"] for r, _x in rs})))
            f.write("- read by: %s\n" % ", ".join("%s (%d)" % (name(c), n) for c, n in loaders.most_common(4)))
            f.write("- examples: %s\n" % ", ".join("%s (%d)" % kv for kv in files.most_common(6)))
            f.write("- chains:\n%s\n\n" % "\n".join("  - %s (%d)" % kv for kv in chains.most_common(6)))
        snd = collections.Counter(s["id"] for r in eps for s in r["sounds"])
        f.write("### sounds asked for: %d plays of %d sounds\n\n%s\n\n" % (
            sum(snd.values()), len(snd), ", ".join("%d %s (%d)" % (i, assets.sound_name(i), n)
                                                    for i, n in snd.most_common(40))))
        loads = [r for r in eps if r["kind"] == "load"]
        if loads:
            f.write("## Save loads\n\n| save | reads | env after | archives |\n|---|---|---|---|\n")
            for r in loads:
                c = collections.Counter(assets.asset_class(x["file"]) for x in r["reads"])
                f.write("| %s | %d | %s | %s |\n" % (r["id"][5:], len(r["reads"]), r.get("env"),
                                                   ", ".join("%s %d" % kv for kv in c.most_common(8))))
            f.write("\n")
        f.write("## Functions\n\nThe assets read within %d calls ([depth]: calls between the "
                "function and the read), and the texts and sounds asked for in play (by the "
                "first function on the chain that is not a text or sound API).\n\n" % DEPTH)
        pf = per_function(eps)
        for va in sorted(pf, key=lambda v: (group_of(v), v)):
            p = pf[va]
            nm = known.get("func_%08X" % va, "func_%08X" % va)
            lines = []
            if p["assets"]:
                lines.append("assets: " + assets_text(p["assets"], 600))
            if p["texts"]:
                lines.append("texts: " + "; ".join('%d "%s"' % (x, assets.rsc_text(x, 50)) for x in sorted(p["texts"])))
            if p["sounds"]:
                lines.append("sounds: " + ", ".join("%d %s" % (x, assets.sound_name(x)) for x in sorted(p["sounds"])))
            f.write("- **%s** [%s]: %s\n" % (nm, group_of(va), " | ".join(lines)))
    n_assets = sum(1 for p in pf.values() if p["assets"] or p["texts"] or p["sounds"])
    print("%d runs, %d reads; %d functions with asset evidence: build/assets/report.md" % (
        len(eps), len(reads), n_assets))


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    for c in ("collect", "loads"):
        p = sub.add_parser(c)
        p.add_argument("-j", type=int, default=None, help="workers (default and cap: by cores and memory)")
        p.add_argument("--resume", action="store_true", help="keep what was traced already")
    w = sub.add_parser("work")
    w.add_argument("what", choices=("reads", "loads"))
    w.add_argument("k", type=int)
    w.add_argument("n", type=int)
    sub.add_parser("report")
    a = ap.parse_args()
    if a.cmd in ("collect", "loads"):
        collect("reads" if a.cmd == "collect" else "loads", fallemu.workers(a.j), a.resume)
    elif a.cmd == "work":
        work(a.what, a.k, a.n)
    else:
        report()


if __name__ == "__main__":
    main()
