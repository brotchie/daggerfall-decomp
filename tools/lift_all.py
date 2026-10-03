#!/usr/bin/env python3
"""Lift every game function, compile it and check it against FALL.EXE: the batch loop.

For each function in the game region (config/regions.csv) this runs tools/lift.py, compiles
the C with the project compiler and applies the build's checks (identical bytes outside
relocations, same length, every relocation resolving to the original target). Runs on all
cores; nothing here needs a model in the loop.

Output:
  build/lift/<func>.c      the lifted C (matching or not)
  build/lift/report.csv    func, status (ok / diff / unsupported / error), detail
  a summary: match count, and the most common unsupported reasons and first differences,
  which is the to-do list for the lifter and the compiler patch

The compiler is the game's own, Watcom C32 10.0a, under DOSBox-X (tools/wcc10.py). After a
run, tools/settle_lifted.py writes the matching functions into src/lifted/.

usage: lift_all.py [-j N] [--only func_X,func_Y] [--limit N]
"""
import argparse
import csv
import json
import multiprocessing as mp
import os
import re
import sys
import tempfile
import time
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.environ.get("LIFT_OUT") or os.path.join(ROOT, "build", "lift")
os.makedirs(OUT, exist_ok=True)

W = {}  # per-worker state


def init_worker():
    import capstone
    import lift
    import match
    import build_fall
    lift.init()
    W["lift"] = lift
    W["flips"] = {}
    # the committed seed (config/lift_flips.json), then this checkout's newer finds
    for p in (os.path.join(ROOT, "config", "lift_flips.json"), os.path.join(OUT, "flips.json")):
        try:
            with open(p) as f:
                W["flips"].update(json.load(f))
        except (OSError, ValueError):
            pass
    W["match"] = match
    W["build"] = build_fall
    W["tgt"] = match.Target()
    W["funcs"] = match.symbol_map()
    W["fix_at"] = {f.src_va: f for f in W["tgt"].le.fixups()}
    W["flags"] = match.default_flags()
    W["md"] = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)


def norm(ins):
    t = "%s %s" % (ins.mnemonic, ins.op_str)
    return re.sub(r"0x[0-9a-f]{5,}", "ADDR", t)


def decode(code, va):
    """Instructions of a function, stepping over its switch tables (data in the code)."""
    md = W["md"]
    tables = sorted((t, n) for t, n in W["lift"].IMG.tables if va <= t < va + len(code))
    out, pos = [], 0
    for t, n in tables + [(va + len(code), 0)]:
        out += list(md.disasm(code[pos:t - va], va + pos))
        pos = t - va + n
    return out


def first_diff(ours, theirs, va, mask_ours, mask_theirs):
    """Short description of the first instruction pair that differs outside relocations,
    walking both instruction lists in step: a branch that only differs in its displacement
    or size (from a difference further on) counts as equal."""
    a = decode(ours, va)
    b = decode(theirs, va)
    for x, y in zip(a, b):
        if x.mnemonic.startswith("j") and x.mnemonic == y.mnemonic and \
                x.op_str.startswith("0x") and y.op_str.startswith("0x"):
            continue
        ko, kt = x.address - va, y.address - va
        if x.size != y.size or any(ko + j not in mask_ours and kt + j not in mask_theirs and
                                   ours[ko + j] != theirs[kt + j] for j in range(y.size)):
            return "%s | %s" % (norm(x), norm(y)), y.address
    return "length %d | %d bytes" % (len(ours.rstrip(b"\x00")), len(theirs)), va + len(theirs)


def check(name, c):
    """Compile `c` and compare: (status, detail, address of the first difference or None)."""
    from omf import OMF
    match, build = W["match"], W["build"]
    path = os.path.join(OUT, name + ".c")
    with open(path, "w") as f:
        f.write(c)
    with tempfile.TemporaryDirectory() as td:
        objp = os.path.join(td, "m.obj")
        try:
            import io
            import contextlib
            err = io.StringIO()
            with contextlib.redirect_stderr(err):
                match.compile_c(path, W["flags"], objp)
        except SystemExit:
            msg = [l for l in err.getvalue().splitlines() if "Error" in l]
            msg = [re.sub(r"^.*?Error! E\d+: ", "", l) for l in msg]
            return "error", "compile: " + (msg[0][:150] if msg else "failed"), None
        obj = OMF(objp)
    fns = {n.strip("_"): (si, off, sz) for n, si, off, sz in obj.functions()}
    if name not in fns:
        return "error", "function not in object", None
    tva, tsize = W["funcs"][name]
    ok, diff = match.compare(W["tgt"], obj, name, tva, tsize, quiet=True)
    si, off, csz = fns[name]
    with open(os.path.join(OUT, name + ".bin"), "wb") as f:   # for tools/idiom_diff.py
        f.write(bytes(obj.data[si][off:off + csz]).rstrip(b"\x00"))
    if not ok:
        ours = bytes(obj.data[si][off:off + csz])
        mask = {fx.offset - off + j for fx in obj.fixups if fx.seg == si
                and off <= fx.offset < off + csz for j in range(fx.size)}
        tmask = {k for k in range(tsize) if tva + k in W["tgt"].fix}
        desc, at = first_diff(ours, W["tgt"].bytes_at(tva, tsize), tva, mask, tmask)
        return "diff", "%d bytes (ours %d, target %d): %s" % (
            diff, len(ours.rstrip(b"\x00")), tsize, desc), at
    fields = {}
    bad = build.check_relocs(obj, si, off, off, csz, tva, W["tgt"], W["fix_at"], W["funcs"],
                             {}, W["tgt"].le, fields)
    if bad:
        at = fields.get("bad_at")      # lets the choice point search run
        return "diff", "relocation: " + bad, at
    return "ok", "", None


# operand-order search: how many choice points before a mismatch to try, and compiles at most
NEAR = 8
BUDGET = int(os.environ.get("LIFT_BUDGET", "600"))


def work(va):
    """lift_one from the cached flips, and (LIFT_SCRATCH=1, after compiler changes) when
    that doesn't match, also from scratch (the greedy search can be led astray by a stale
    cache): the result that gets further."""
    r = lift_one(va)
    name = "func_%08X" % va
    if r[1] != "ok" and len(r) > 4 and W.get("flips", {}).get(name) and \
            not os.environ.get("LIFT_NOFLIPCACHE") and os.environ.get("LIFT_SCRATCH"):
        os.environ["LIFT_NOFLIPCACHE"] = "1"
        try:
            r2 = lift_one(va)
        finally:
            del os.environ["LIFT_NOFLIPCACHE"]
        if len(r2) > 4 and (r2[1] == "ok" or (r2[4] or 0) > (r[4] or 0)):
            name_ = r2[0]
            check(name_, W["lift"].lift(va, frozenset(r2[3])))   # its files on disk
            return r2
        # (leave the cached run's best version on disk)
        try:
            check(name, W["lift"].lift(va, frozenset(r[3])))
        except Exception:
            pass
    return r


def lift_one(va):
    lift = W["lift"]
    name = "func_%08X" % va
    info = {}
    start = frozenset()
    try:
        c = lift.lift(va, info=info)
    except lift.Unsupported as e:
        # a choice point taken the other way may get past it (a register kept across a
        # call, say): the choice points seen before the failure, nearest first
        c = None
        m = re.search(r" at ([0-9a-f]+)$", str(e))
        where = int(m.group(1), 16) if m else info.get("where", va)
        for a in sorted(info.get("choices", []), key=lambda a: abs(a - where))[:24]:
            info2 = {}
            try:
                c = lift.lift(va, frozenset({a}), info=info2)
            except Exception:
                continue
            start, info = frozenset({a}), info2
            break
        if c is None:
            return name, "unsupported", re.sub(r" at [0-9a-f]+$", "", str(e))
    except Exception as e:  # a lifter bug: report, keep going
        return name, "error", "lifter %s: %s" % (type(e).__name__, e)
    status, detail, at = check(name, c)
    if status == "error" and detail.startswith("compile"):
        # the C doesn't compile (a void result used): a choice point taken the other way
        # may fix it
        cands = [x for x in info.get("choices", []) if x >= 0]
        # call arity choices first (a void result passed on, an argument too many)
        cands.sort(key=lambda x: (round(x - int(x), 4) not in (0.5, 0.875, 0.25, 0.0), x))
        for a in cands[:200]:
            info2 = {}
            try:
                c2 = lift.lift(va, start | {a}, info=info2)
            except Exception:
                continue
            r2 = check(name, c2)
            if r2[0] != "error":
                status, detail, at = r2
                c, info, start = c2, info2, start | {a}
                break
    # Choice points (operand orders and spellings the code cannot tell apart): try
    # flipping each one near the first difference and keep the flip that moves the first
    # difference furthest; when no single flip helps, try pairs of the nearest.
    flips, tries = start, 0
    tried_pairs = set()      # first differences pairs were tried at
    best = (status, detail, at, c)

    def nbytes(d):
        m = re.match(r"(\d+) bytes", d or "")
        return int(m.group(1)) if m else 1 << 30

    def attempt(fl):
        info2 = {}
        try:
            c2 = lift.lift(va, fl, info=info2)
        except Exception:
            return None
        s2, d2, at2 = check(name, c2)
        return s2, d2, at2, c2, info2

    # the flips a previous run found (the search is greedy: a new choice point can lead it
    # astray): start from them when they get further
    cached = frozenset(W.get("flips", {}).get(name, []))
    if status != "ok" and cached and cached != flips and \
            not os.environ.get("LIFT_NOFLIPCACHE"):
        r = attempt(cached)
        if r is not None and (r[0] == "ok" or (r[0] == "diff" and r[2] is not None and
                                               (at is None or r[2] > at))):
            flips = cached
            status, detail, at, c, info = r
            best = (status, detail, at, c)
    while status == "diff" and at is not None and tries < BUDGET and not os.environ.get("LIFT_NOSEARCH"):
        sites = info.get("sites", {})
        near = sorted((a for a in info["choices"] if a >= 0 and a not in flips),
                      key=lambda a: min(abs(x - at) for x in sites.get(a, [a])))[:NEAR]
        m_ = re.search(r": (.*) \| (.*)$", detail or "")
        slot_diff = m_ is not None and "ebp - 0x" in m_.group(1) and "ebp - 0x" in m_.group(2) \
            and re.sub(r"0x[0-9a-f]+", "K", m_.group(1)) == re.sub(r"0x[0-9a-f]+", "K", m_.group(2))
        if (at - va < 16 or slot_diff) and not os.environ.get("LIFT_NOFRAMEALL"):
            # a difference in the frame size or a variable's slot: any choice point can
            # change it (a temp, a variable's type, an array's extent), not just those near
            # (the frame's own choices first: an array's extent, a variable's width)
            near = sorted((a for a in info["choices"] if a >= 0 and a not in flips),
                          key=lambda a: (a % 1 not in (0.5, 0.0625, 0.15625), a))[:3 * NEAR]
        # function-level choices too
        func_level = [a for a in info["choices"] if a < 0 and a not in flips]
        found = None
        for a in near + func_level:
            if tries >= BUDGET:
                break
            tries += 1
            r = attempt(flips | {a})
            if r is None:
                continue
            s2, d2, at2, c2, info2 = r
            if s2 == "ok":
                found = (1 << 40, 0, flips | {a}, r)
                break
            if s2 == "diff" and at2 is not None and at2 > at:
                key = (at2, -nbytes(d2))
                if found is None or key > found[:2]:
                    found = (at2, -nbytes(d2), flips | {a}, r)
        if found is None and at not in tried_pairs:
            tried_pairs.add(at)
            pairs = [(x, y) for i, x in enumerate(near[:4]) for y in near[i + 1:5]
                     if x >= 0 and y >= 0]
            if not os.environ.get("LIFT_NOWIDEPAIRS"):
                # then a little further out, choices close to each other first (two
                # choices of one statement)
                more = [(x, y) for i, x in enumerate(near[:7]) for y in near[i + 1:7]
                        if (x, y) not in pairs and x >= 0 and y >= 0]
                pairs += sorted(more, key=lambda p_: abs(p_[0] - p_[1]))
            # a nearby choice with a function-level one
            pairs += [(x, y) for x in near[:3] for y in func_level]
            for x, y in pairs:
                if tries >= BUDGET:
                    break
                tries += 1
                r = attempt(flips | {x, y})
                if r is None:
                    continue
                s2, d2, at2, c2, info2 = r
                if s2 == "ok" or (s2 == "diff" and at2 is not None and at2 > at):
                    found = (at2 or 0, 0, flips | {x, y}, r)
                    break
        if found is None and flips and not os.environ.get("LIFT_NOBACKTRACK") and \
                ("bt", at) not in tried_pairs:
            # backtrack: an earlier flip taken back, alone or with a nearby new one
            tried_pairs.add(("bt", at))
            for f in sorted(flips, key=lambda f: -abs(f)):
                for x in [None] + near[:4] + func_level[:0]:
                    if tries >= BUDGET:
                        break
                    f2 = (flips - {f}) | ({x} if x is not None else set())
                    if f2 == flips:
                        continue
                    tries += 1
                    r = attempt(f2)
                    if r is None:
                        continue
                    s2, d2, at2 = r[0], r[1], r[2]
                    if s2 == "ok" or (s2 == "diff" and at2 is not None and at2 > at):
                        found = (at2 or 0, 0, frozenset(f2), r)
                        break
                if found is not None:
                    break
        if found is None:
            break
        flips = found[2]
        status, detail, at, c, info = found[3]
        best = (status, detail, at, c)
    status, detail, at, c = best
    if os.environ.get("LIFT_SHOWFLIPS"):
        print(name, sorted(flips), file=sys.stderr)
    if c is not None:
        check(name, c)          # leave the best version's .c and .bin on disk
    return name, status, detail, sorted(flips), at


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-j", type=int, default=os.cpu_count())
    ap.add_argument("--only", default=None)
    ap.add_argument("--limit", type=int, default=None)
    a = ap.parse_args()
    with open(os.path.join(ROOT, "config", "regions.csv"), newline="") as f:
        game = next(r for r in csv.DictReader(f) if r["region"] == "game")
    lo, hi = int(game["start"], 16), int(game["end"], 16)
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        vas = [int(r["va"], 16) for r in csv.DictReader(f)]
    vas = [v for v in vas if lo <= v < hi]
    if a.only:
        want = {int(x.replace("func_", ""), 16) for x in a.only.split(",")}
        vas = [v for v in vas if v in want]
    if a.limit:
        vas = vas[:a.limit]
    os.makedirs(OUT, exist_ok=True)
    # fail here, not in every pool worker (a broken lift.py would respawn them forever),
    # and build the shared caches once before the workers start
    import lift
    lift.init()
    lift.caller_types()
    lift.unsigned_globals()
    lift.ret_uses()
    t0 = time.time()
    with mp.Pool(a.j, initializer=init_worker) as pool:
        results = sorted(pool.imap_unordered(work, vas, chunksize=8))
    fp = os.path.join(OUT, "flips.json")
    try:
        with open(fp) as f:
            allflips = json.load(f)
    except (OSError, ValueError):
        allflips = {}
    ap_ = os.path.join(OUT, "flips_at.json")
    try:
        with open(ap_) as f:
            allat = json.load(f)
    except (OSError, ValueError):
        allat = {}
    for r in results:
        if len(r) > 3:
            # keep each function's furthest progress: matched, or a later first difference
            at_ = r[4] if len(r) > 4 and r[4] is not None else 0
            if r[1] == "ok" or r[0] not in allflips or at_ > allat.get(r[0], 0):
                allflips[r[0]] = r[3]
                allat[r[0]] = (1 << 40) if r[1] == "ok" else at_
    with open(fp, "w") as f:
        json.dump(allflips, f)
    with open(ap_, "w") as f:
        json.dump(allat, f)
    results = [r[:3] for r in results]
    # keep the previous full run's report to show what this change gained and lost
    prev = {}
    rp = os.path.join(OUT, "report.csv")
    if not a.only and not a.limit and os.path.exists(rp):
        with open(rp, newline="") as f:
            prev = {r["func"]: r["status"] for r in csv.DictReader(f)}
        os.replace(rp, os.path.join(OUT, "report.prev.csv"))
    rows = results
    if (a.only or a.limit) and os.path.exists(rp):
        # a partial run updates its functions' rows and keeps the rest
        with open(rp, newline="") as f:
            old = {r["func"]: (r["func"], r["status"], r["detail"]) for r in csv.DictReader(f)}
        old.update({r[0]: r for r in results})
        rows = sorted(old.values())
    with open(rp, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["func", "status", "detail"])
        w.writerows(rows)
    st = Counter(r[1] for r in results)
    print("%d functions in %.0fs: %s" % (len(results), time.time() - t0,
                                          ", ".join("%s %d" % kv for kv in st.most_common())))
    if prev:
        gained = [r[0] for r in results if r[1] == "ok" and prev.get(r[0]) != "ok"]
        lost = [r for r in results if r[1] != "ok" and prev.get(r[0]) == "ok"]
        print("since the previous run: +%d -%d" % (len(gained), len(lost)))
        for r in lost[:10]:
            print("  LOST %s: %s %s" % (r[0], r[1], r[2][:100]))
    for status in ("unsupported", "error"):
        c = Counter(re.sub(r"0x[0-9a-f]+|\d+", "N", r[2])[:90] for r in results if r[1] == status)
        if c:
            print("\ntop %s reasons:" % status)
            for k, n in c.most_common(15):
                print("  %5d  %s" % (n, k))
    c = Counter(re.sub(r"^\d+ bytes \(ours \d+, target \d+\): ", "", r[2])[:110]
                for r in results if r[1] == "diff")
    if c:
        print("\ntop first differences (ours | original):")
        for k, n in c.most_common(25):
            print("  %5d  %s" % (n, k))
    # the same, weighted by function size: big functions fail on their first mismatch
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        size = {r["name"]: int(r["size"]) for r in csv.DictReader(f)}
    cb = Counter()
    for r in results:
        if r[1] == "diff":
            cb[re.sub(r"^\d+ bytes \(ours \d+, target \d+\): ", "", r[2])[:110]] += size[r[0]]
        elif r[1] in ("unsupported", "error"):
            cb["[%s] %s" % (r[1], re.sub(r"0x[0-9a-f]+|\d+", "N", r[2])[:90])] += size[r[0]]
    print("\ntop causes by bytes of code blocked:")
    for k, n in cb.most_common(25):
        print("  %7d  %s" % (n, k))


if __name__ == "__main__":
    main()
