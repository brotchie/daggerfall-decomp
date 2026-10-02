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
OUT = os.path.join(ROOT, "build", "lift")

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


REG_RE = re.compile(r"\b(e[abcd]x|e[sd]i|[abcd]x|[sd]i)\b")


def reg_log(c):
    """The function's register choices in order (DAGGER_REGLOG): [chosen register]."""
    match = W["match"]
    with tempfile.TemporaryDirectory() as td:
        src = os.path.join(td, "r.c")
        with open(src, "w") as f:
            f.write(c)
        cmd = [match.compiler(), "-q", "-zq"] + W["flags"] + [
            "-i=" + os.path.join(ROOT, "include"), "-i=" + os.path.join(ROOT, "src"),
            "-fo=r.obj", "r.c"]
        import subprocess
        r = subprocess.run(cmd, capture_output=True, text=True, cwd=td,
                           env=dict(os.environ, DAGGER_REGLOG="1"))
    W["confs"] = sum(int(ln.split()[2]) for ln in r.stderr.splitlines()
                     if ln.startswith("confsort "))
    return [ln.split()[2] for ln in r.stderr.splitlines() if ln.startswith("reg ")]


def pin_search(name, va, c, detail, at, flips, attempt):
    """Last resort at a register-only difference: pin one of the allocator's choices to the
    register the original used there (#pragma dagger reg). Tries the choices that picked
    our register first. Returns (at2, 0, flips, result, tries) or None."""
    lift = W["lift"]
    m = re.search(r": (.*) \| (.*)$", detail or "")
    if not m or at is None:
        return None
    ours, theirs = m.group(1), m.group(2)
    if REG_RE.sub("R", ours) != REG_RE.sub("R", theirs) and os.environ.get("LIFT_PINSAME"):
        return None
    pairs = [(a, b) for a, b in zip(REG_RE.findall(ours), REG_RE.findall(theirs)) if a != b]
    log = reg_log(c)
    if not log:
        return None
    used = {lift.pin_decode(f)[0] for f in flips if -200000 < f <= -3000}
    if os.environ.get("LIFT_DEEP"):
        # offline: every window, then every single pin to every register, nearest first
        nconf = W.get("confs", 0)
        tva, tsize = W["funcs"][name]
        share = (at - tva) / max(tsize, 1)
        cands = []
        if not any(f <= -200000 for f in flips):
            cands += sorted((lift.win_encode(k1, k1 + L) for L in range(1, 9)
                             for k1 in range(0, nconf) if k1 + L < nconf),
                            key=lambda f: abs(sum(lift.win_decode(f)) / 2 - nconf * share))
        cands += sorted((lift.pin_encode(k, r) for k in range(len(log)) if k not in used
                         for r in ("eax", "ebx", "ecx", "edx", "esi", "edi") if r != log[k]),
                        key=lambda f: abs(lift.pin_decode(f)[0] - len(log) * share))
        tries_d = 0
        for f in cands:
            tries_d += 1
            r = attempt(flips | {f})
            if r is None:
                continue
            s2, d2, at2 = r[0], r[1], r[2]
            if s2 == "ok" or (s2 == "diff" and at2 is not None and at2 > at):
                return ((1 << 40) if s2 == "ok" else at2, 0, flips | {f}, r, tries_d)
        return None
    # first: a window of the allocation order taken latest-starting first
    nconf = W.get("confs", 0)
    if nconf and not os.environ.get("LIFT_NOCONFWIN") and \
            not any(f <= -200000 for f in flips):
        tva, tsize = W["funcs"][name]
        c_est = nconf * (at - tva) / max(tsize, 1)
        wins = sorted(((k1, k1 + L) for L in range(1, 6) for k1 in range(0, nconf)
                       if k1 + L < nconf), key=lambda w: (abs((w[0] + w[1]) / 2 - c_est), w[1] - w[0]))
        tries_w = 0
        for k1, k2 in wins[:60]:
            tries_w += 1
            f2 = flips | {lift.win_encode(k1, k2)}
            r = attempt(f2)
            if r is None:
                continue
            s2, d2, at2 = r[0], r[1], r[2]
            if s2 == "ok" or (s2 == "diff" and at2 is not None and at2 > at):
                return ((1 << 40) if s2 == "ok" else at2, 0, f2, r, tries_w)
    if not pairs:
        # the registers don't line up (`mov edx,[g]` against `mov edx,eax`): any choice to
        # any register either instruction names
        regs = sorted(set(REG_RE.findall(ours + " " + theirs)))
        if not regs:
            return None
        mine = want = None
        near_k = []
        tva, tsize = W["funcs"][name]
        k_est = len(log) * (at - tva) / max(tsize, 1)
        order = sorted([(k, r) for k in range(len(log)) if k not in used for r in regs
                        if r != log[k]], key=lambda kr: abs(kr[0] - k_est))
    else:
        mine, want = pairs[0]
        # a choice that took our register gets theirs, or one that took theirs gets ours;
        # nearest first to where the difference is, as a share of the function
        tva, tsize = W["funcs"][name]
        k_est = len(log) * (at - tva) / max(tsize, 1)
        near_k = sorted([k for k, r in enumerate(log) if r == mine and k not in used],
                        key=lambda k: abs(k - k_est))
        order = [(k, want) for k in near_k] + \
            sorted([(k, mine) for k, r in enumerate(log) if r == want and k not in used],
                   key=lambda kr: abs(kr[0] - k_est))
    tries = 0
    for k, reg in order[:64]:
        if reg not in lift.PIN_REGS:
            continue
        # the pin alone, then with the next choice held at what it took before (moving
        # one choice frees or takes a register the next one wanted)
        variants = [flips | {lift.pin_encode(k, reg)}]
        if k + 1 < len(log) and log[k + 1] in lift.PIN_REGS and k + 1 not in used:
            variants.append(variants[0] | {lift.pin_encode(k + 1, log[k + 1])})
        for f2 in variants:
            tries += 1
            r = attempt(f2)
            if r is None:
                continue
            s2, d2, at2 = r[0], r[1], r[2]
            if s2 == "ok" or (s2 == "diff" and at2 is not None and at2 > at):
                return ((1 << 40) if s2 == "ok" else at2, 0, f2, r, tries)
    if mine is None:
        return None
    # a swap: one choice to theirs and another to ours
    took_want = [k for k, r in enumerate(log) if r == want and k not in used]
    if os.environ.get("LIFT_PINWIDE"):
        # (offline, slow) every nearby pair of choices, each to ours, theirs or its own
        for k1 in near_k + took_want:
            for k2 in range(max(0, k1 - 3), min(len(log), k1 + 4)):
                if k2 == k1 or k2 in used:
                    continue
                for r1 in (want, mine):
                    for r2 in {mine, want, log[k2]}:
                        if r1 not in lift.PIN_REGS or r2 not in lift.PIN_REGS or \
                                (r1 == log[k1] and r2 == log[k2]):
                            continue
                        tries += 1
                        f2 = flips | {lift.pin_encode(k1, r1), lift.pin_encode(k2, r2)}
                        r = attempt(f2)
                        if r is None:
                            continue
                        s2, d2, at2 = r[0], r[1], r[2]
                        if s2 == "ok" or (s2 == "diff" and at2 is not None and at2 > at):
                            return ((1 << 40) if s2 == "ok" else at2, 0, f2, r, tries)
    for k1 in near_k[:8]:
        for k2 in took_want[:8]:
            if want not in lift.PIN_REGS or mine not in lift.PIN_REGS:
                return None
            tries += 1
            f2 = flips | {lift.pin_encode(k1, want), lift.pin_encode(k2, mine)}
            r = attempt(f2)
            if r is None:
                continue
            s2, d2, at2 = r[0], r[1], r[2]
            if s2 == "ok" or (s2 == "diff" and at2 is not None and at2 > at):
                return ((1 << 40) if s2 == "ok" else at2, 0, f2, r, tries)
    return None


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
    # Choice points (operand orders the code can't tell apart, code generator knobs): try
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
        if at - va < 16 and not os.environ.get("LIFT_NOFRAMEALL"):
            # a difference in the frame size: any choice point can change it (a temp, a
            # variable's type), not just those near the prologue
            near = sorted(a for a in info["choices"] if a >= 0 and a not in flips)[:3 * NEAR]
        # function-level choices (compiler knobs) too
        func_level = [a for a in info["choices"] if a < 0 and a not in flips]
        if os.environ.get("DAGGER_CC") == "w10":
            # (the real Watcom 10.0a has no knobs and ignores slot pins)
            func_level = [a for a in func_level if not (-100 < a < 0 or a == -1000)]
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
            if not os.environ.get("LIFT_NOKNOBPAIRS"):
                # a nearby choice with a compiler knob
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
            for f in sorted(flips, key=lambda f: (f <= -3000, -abs(f))):
                if f <= -3000 and os.environ.get("LIFT_KEEPPINS"):
                    continue        # (register pins are positional; tried last)
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
        if found is None and not os.environ.get("LIFT_NOREGPIN"):
            found = pin_search(name, va, c, detail, at, flips, attempt)
            if found is not None:
                tries += found[4]
                found = found[:4]
        if found is None:
            break
        flips = found[2]
        status, detail, at, c, info = found[3]
        best = (status, detail, at, c)
    status, detail, at, c = best
    if status == "diff" and cached and not os.environ.get("LIFT_NOCACHEKNOBS"):
        # a previous run's flips with one more compiler knob (a knob added since, say)
        for k in [a for a in info.get("choices", []) if -100 < a < 0 and a not in cached]:
            r = attempt(cached | {k})
            if r is not None and r[0] == "ok":
                flips = cached | {k}
                status, detail, at, c = r[0], r[1], r[2], r[3]
                break
    if status == "diff" and not os.environ.get("LIFT_NOKNOBSTART"):
        # a compiler knob alone that leaves far fewer differing bytes (though the first one
        # comes earlier): a short greedy search from it
        nb = nbytes(detail)
        for k in [a for a in info.get("choices", []) if -100 < a < 0]:
            r = attempt(frozenset({k}))
            if r is None or r[0] == "error" or (r[0] == "diff" and nbytes(r[1]) * 3 > nb):
                continue
            fl, (s2, d2, at2, c2, info2) = frozenset({k}), r
            for _ in range(8):
                if s2 == "ok" or at2 is None:
                    break
                sites2 = info2.get("sites", {})
                near2 = sorted((a for a in info2["choices"] if a >= 0 and a not in fl),
                               key=lambda a: min(abs(x - at2) for x in sites2.get(a, [a])))[:12]
                step = None
                for a in near2:
                    r2 = attempt(fl | {a})
                    if r2 is not None and (r2[0] == "ok" or (r2[0] == "diff" and r2[2] is not None
                                                            and r2[2] > at2)):
                        if step is None or r2[0] == "ok" or (step[1][0] != "ok" and r2[2] > step[1][2]):
                            step = (fl | {a}, r2)
                        if r2[0] == "ok":
                            break
                if step is None and not os.environ.get("LIFT_NOREGPIN"):
                    pf = pin_search(name, va, c2, d2, at2, fl, attempt)
                    if pf is not None:
                        step = (pf[2], pf[3])
                if step is None:
                    break
                fl, (s2, d2, at2, c2, info2) = step[0], step[1]
            if s2 == "ok":
                flips, status, detail, at, c = fl, s2, d2, at2, c2
                break
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
