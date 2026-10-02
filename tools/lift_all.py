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

usage: lift_all.py [-j N] [--only func_X,func_Y] [--limit N]
"""
import argparse
import csv
import multiprocessing as mp
import os
import re
import sys
import tempfile
import time
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "build", "lift")

W = {}  # per-worker state


def init_worker():
    import capstone
    import lift
    import match
    import build_fall
    lift.init()
    W["lift"] = lift
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


def first_diff(ours, theirs, va, mask):
    """Short description of the first instruction pair that differs outside relocations.
    Relative branches count as equal when mnemonic and length agree: their displacement only
    differs because of a size difference further on, which is the thing to report."""
    md = W["md"]
    a = list(md.disasm(ours, va))
    b = list(md.disasm(theirs, va))
    for x, y in zip(a, b):
        k = y.address - va
        if x.mnemonic.startswith("j") and y.mnemonic.startswith("j") and \
                x.mnemonic == y.mnemonic and x.size == y.size and x.op_str.startswith("0x"):
            continue
        if x.size != y.size or any(k + j not in mask and ours[k + j] != theirs[k + j]
                                   for j in range(y.size)):
            return "%s | %s" % (norm(x), norm(y))
    return "length %d | %d bytes" % (len(ours.rstrip(b"\x00")), len(theirs))


def work(va):
    from omf import OMF
    lift, match, build = W["lift"], W["match"], W["build"]
    name = "func_%08X" % va
    try:
        c = lift.lift(va)
    except lift.Unsupported as e:
        return name, "unsupported", re.sub(r" at [0-9a-f]+$", "", str(e))
    except Exception as e:  # a lifter bug: report, keep going
        return name, "error", "lifter %s: %s" % (type(e).__name__, e)
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
            return name, "error", "compile: " + (msg[0][:150] if msg else "failed")
        obj = OMF(objp)
    fns = {n.strip("_"): (si, off, sz) for n, si, off, sz in obj.functions()}
    if name not in fns:
        return name, "error", "function not in object"
    tva, tsize = W["funcs"][name]
    ok, diff = match.compare(W["tgt"], obj, name, tva, tsize, quiet=True)
    si, off, csz = fns[name]
    with open(os.path.join(OUT, name + ".bin"), "wb") as f:   # for tools/idiom_diff.py
        f.write(bytes(obj.data[si][off:off + csz]).rstrip(b"\x00"))
    if not ok:
        ours = bytes(obj.data[si][off:off + csz])
        mask = {fx.offset - off + j for fx in obj.fixups if fx.seg == si
                and off <= fx.offset < off + csz for j in range(fx.size)}
        mask |= {k for k in range(tsize) if tva + k in W["tgt"].fix}
        return name, "diff", "%d bytes (ours %d, target %d): %s" % (
            diff, len(ours.rstrip(b"\x00")), tsize,
            first_diff(ours, W["tgt"].bytes_at(tva, tsize), tva, mask))
    fields = {}
    bad = build.check_relocs(obj, si, off, off, csz, tva, W["tgt"], W["fix_at"], W["funcs"],
                             {}, W["tgt"].le, fields)
    if bad:
        return name, "diff", "relocation: " + bad
    return name, "ok", ""


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
    t0 = time.time()
    with mp.Pool(a.j, initializer=init_worker) as pool:
        results = sorted(pool.imap_unordered(work, vas, chunksize=8))
    # keep the previous full run's report to show what this change gained and lost
    prev = {}
    rp = os.path.join(OUT, "report.csv")
    if not a.only and not a.limit and os.path.exists(rp):
        with open(rp, newline="") as f:
            prev = {r["func"]: r["status"] for r in csv.DictReader(f)}
        os.replace(rp, os.path.join(OUT, "report.prev.csv"))
    with open(rp, "w", newline="") as f:
        w = csv.writer(f)
        w.writerow(["func", "status", "detail"])
        w.writerows(results)
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
