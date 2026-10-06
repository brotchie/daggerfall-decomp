#!/usr/bin/env python3
"""Basic-block coverage of XnGine's asm (object 2) under the test suite.

  cfg       the static CFG: every function of config/xngine_functions.csv (the run-time blocks
            and handlers included; the data items and the code templates, which run only as
            copies, left out), walked as tools/xn_c.py walks it (everything reachable from the
            entry without passing another function's entry); basic blocks start at the entry,
            at jump and branch targets, after a conditional branch or loop, and after a ret or
            jmp when something else reaches the next instruction. A block shared by functions
            counts once. -> build/xn_canon/coverage/cfg.json
  corpus    replay every record of the corpus on the asm with Unicorn's coverage map (the
            patched Unicorn: a byte per code byte, set when a translation block covering it is
            made, i.e. just before it runs). The block at a record's return address is made
            but never runs (the replay stops on it): it is taken out again.
            -> build/xn_canon/coverage/corpus.bin
  scenarios run the scenarios (tools/xn_scenarios.py) on the asm alone with the coverage map.
            -> build/xn_canon/coverage/scen_NAME.bin
  report    blocks executed / reachable, overall, per subsystem and per function; the blocks
            never executed, grouped by a likely reason: a dead function (no caller per the
            ABI), helmet or serial hardware, an error or fatal path, VESA, debug and
            benchmarks, the rest. -> build/xn_canon/coverage/report.md and functions.csv

A block counts as executed when its first byte is in the map; instructions are counted too.

usage: xn_cover.py cfg | corpus [-j N] | scenarios [NAME ...] [-j N] | report [--top N]
"""
import argparse
import base64
import bisect
import collections
import csv
import glob
import json
import os
import struct
import sys
import time
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# XN_COVER_OUT: another folder for the coverage results (an agent's), the CFG staying shared
COVER = os.path.join(ROOT, "build", "xn_canon", "coverage")
OUT = os.environ.get("XN_COVER_OUT") or COVER
CFG = os.path.join(COVER, "cfg.json")
LOAD = 0x01000000
OBJ2 = (0xC0000, 0x161568)
NOT_CODE = {0x157B02, 0x157E02, 0x160F00, 0x160F04, 0x160F11}
FATAL = {0x050069: "fatal_error", 0x09DB11: "exit"}
BRANCHES = ("jmp", "ret", "retf", "iret", "iretd", "call", "int", "hlt", "loop", "loope",
            "loopne", "jecxz", "jcxz")


def is_branch(m):
    b = m.split()[-1]
    return b in BRANCHES or b.startswith("j")


# ---- the static CFG ----------------------------------------------------------------------------
def cfg():
    import xn_abi
    import xn_c
    from capstone import x86 as cx
    t0 = time.time()
    prog = xn_c.Program()
    abi = xn_abi.read_abi()
    an = prog.an
    funcs = []
    skip = {}
    for va, by in xn_c.functions():
        row = abi.get(va, {})
        if va in NOT_CODE or row.get("convention") in ("data", "template"):
            skip["%06X" % va] = row.get("convention") or "data"
            continue
        funcs.append(va)
    insn_of = {}                # instruction -> size, mnemonic
    owners = collections.defaultdict(set)
    leaders = set()
    for va in funcs:
        fn = xn_c.Func(prog, va)
        if not fn.insns:
            skip["%06X" % va] = "no code"
            continue
        leaders.add(va)
        order = fn.order
        body = set(order)
        preds = collections.Counter()
        for a in order:
            x = fn.insns[a]
            i = x.i
            insn_of[a] = (i.size, i.mnemonic, i.op_str)
            owners[a].add(va)
            m = i.mnemonic
            if x.target is not None and x.target in body:
                leaders.add(x.target)
                preds[x.target] += 1
            for t in x.table or ():
                if t in body:
                    leaders.add(t)
            if (m.startswith("j") and m != "jmp") or m.startswith("loop") or m in ("jecxz", "jcxz"):
                if x.next in body:
                    leaders.add(x.next)
            if m in ("ret", "jmp", "retf", "iretd", "iret", "hlt") and x.next in body:
                leaders.add(x.next)      # reached from elsewhere (the walk found it)
    starts = sorted(insn_of)
    blocks = []
    cur = None
    for a in starts:
        size, m, _ops = insn_of[a]
        if cur is None or a in leaders or a != cur["end"]:
            cur = {"start": a, "end": a + size, "insns": 1, "last": m}
            blocks.append(cur)
        else:
            cur["end"] = a + size
            cur["insns"] += 1
            cur["last"] = m
        if is_branch(m) and m.split()[-1] not in ("call", "int"):
            cur = None                   # a branch ends the block
    out = []
    for b in blocks:
        fs = sorted(owners[b["start"]])
        out.append([b["start"], b["end"], b["insns"], fs])
    # what each block calls (for the reasons): its call targets and int services
    calls = {}
    for b in out:
        a = b[0]
        tg = []
        ah = None
        while a < b[1]:
            size, m, ops = insn_of[a]
            if m == "call" and ops.startswith("0x"):
                tg.append(int(ops, 16))
            if m == "mov" and ops.split(",")[0] in ("ah", "ax", "eax") and "0x" in ops.split(",")[-1]:
                try:
                    v = int(ops.split(",")[-1].strip(), 16)
                    ah = v if ops.startswith("ah") else (v >> 8) & 0xFF
                except ValueError:
                    ah = None
            if m == "int":
                tg.append("int " + ops + ("/%02X" % ah if ah is not None else ""))
            a += size
        if tg:
            calls[b[0]] = tg
    d = {"blocks": out, "skipped": skip, "functions": ["%06X" % f for f in funcs],
         "calls": {"%06X" % k: [("%06X" % t if isinstance(t, int) else t) for t in v]
                   for k, v in calls.items()},
         "insns": {"%06X" % a: insn_of[a][0] for a in starts},
         "text": {"%06X" % a: "%s %s" % (insn_of[a][1], insn_of[a][2]) for a in starts}}
    os.makedirs(OUT, exist_ok=True)
    with open(CFG, "w") as f:
        json.dump(d, f)
    print("%d functions, %d basic blocks, %d instructions (%d items skipped: data, templates) "
          "-> %s in %.0f s" % (len(funcs), len(out), len(starts), len(skip),
                               os.path.relpath(CFG, ROOT), time.time() - t0))
    return d


def load_cfg():
    if not os.path.exists(CFG):
        return cfg()
    with open(CFG) as f:
        return json.load(f)


# ---- executed: the corpus ------------------------------------------------------------------------
def _bor(a, b):
    n = len(a)
    return (int.from_bytes(a, "little") | int.from_bytes(b, "little")).to_bytes(n, "little")


def tb_extent(a, insns, starts):
    """The bytes of the translation block Unicorn makes at a (to the first branch, inclusive)."""
    k = bisect.bisect_left(starts, a)
    end = a
    n = 0
    while k < len(starts) and starts[k] == end and n < 64:
        size, m = insns[starts[k]]
        end += size
        n += 1
        if is_branch(m):
            break
        k += 1
    return a, end


def _corpus_task(files, sp):
    import ctypes
    import fallemu
    import xn_cload
    import xn_record
    xn_cload.longer_calls(xn_record)
    lib = xn_record.coverage_lib()
    if lib is None:
        raise SystemExit("coverage needs the patched Unicorn (tools/build_unicorn.sh)")
    lo, hi = LOAD + OBJ2[0], LOAD + OBJ2[1]
    n = OBJ2[1] - OBJ2[0]
    acc = bytes(n)
    insns = {}
    import xn_c
    an = xn_c.load_analysis()
    for a, i in an.insns.items():
        insns[a] = (i.size, i.mnemonic)
    starts = sorted(insns)
    orig_load = xn_record._load_base
    started = set()

    def load_base(path, overlay=None):
        emu = orig_load(path, overlay)
        lib.uc_dagger_coverage(emu.uc._uch, lo, hi, None)
        started.add(id(emu))
        return emu
    xn_record._load_base = load_base
    nrec = 0
    for path in files:
        for rec in xn_cload.read_records(xn_record, path):
            try:
                ret = None
                try:
                    import xn_rc
                    ret = (xn_rc.record_return(rec) - LOAD) & 0xFFFFFFFF
                except Exception:       # noqa: BLE001
                    pass
                xn_record.replay_run(rec)
                emu = xn_cload.machine_of(xn_record, rec["base"])
                if emu is None:
                    continue
                buf = ctypes.create_string_buffer(n)
                lib.uc_dagger_coverage(emu.uc._uch, lo, hi, buf)
                cov = bytearray(buf.raw)
                # the block at the return address was made, never run (the replay stops there)
                if ret is not None and OBJ2[0] <= ret < OBJ2[1]:
                    s, e = tb_extent(ret, insns, starts)
                    cov[s - OBJ2[0]:e - OBJ2[0]] = bytes(e - s)
                lib.uc_dagger_coverage(emu.uc._uch, lo, hi, None)      # afresh for the next
                acc = _bor(acc, bytes(cov))
                nrec += 1
            except Exception:       # noqa: BLE001
                xn_cload.drop_machine(xn_record, rec["base"])
    return {"cov": base64.b64encode(zlib.compress(acc, 6)).decode(), "records": nrec}


def corpus(jobs=None):
    import xn_cload
    t0 = time.time()
    files = xn_cload.record_files(None)
    res = xn_cload.parallel("xn_cover:_corpus_task", files, {}, jobs)
    acc = bytes(OBJ2[1] - OBJ2[0])
    n = 0
    for r in res:
        acc = _bor(acc, zlib.decompress(base64.b64decode(r["cov"])))
        n += r["records"]
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "corpus.bin"), "wb") as f:
        f.write(zlib.compress(acc, 6))
    print("%d records replayed on the asm with coverage in %.0f s -> %s" % (
        n, time.time() - t0, os.path.relpath(os.path.join(OUT, "corpus.bin"), ROOT)))
    return 0


def scenarios(names_=None, jobs=None):
    import xn_cload
    import xn_scenarios
    t0 = time.time()
    names_ = names_ or [s["name"] for s in xn_scenarios.SCENARIOS]
    res = xn_cload.parallel("xn_scenarios:_task", names_, {"asm_only": True, "coverage": True,
                                                           "masks": False}, jobs)
    os.makedirs(OUT, exist_ok=True)
    n = 0
    for rs in res:
        for r in rs:
            if r.get("coverage"):
                with open(os.path.join(OUT, "scen_%s.bin" % r["name"]), "wb") as f:
                    f.write(zlib.compress(zlib.decompress(base64.b64decode(r["coverage"])), 6))
                n += 1
    print("%d scenarios' asm coverage in %.0f s" % (n, time.time() - t0))
    return 0


def executed():
    """{source: bytes over object 2} of the coverage maps there are."""
    out = {}
    for p in sorted(glob.glob(os.path.join(OUT, "*.bin"))):
        with open(p, "rb") as f:
            out[os.path.basename(p)[:-4]] = zlib.decompress(f.read())
    return out


# ---- the report -------------------------------------------------------------------------------------
import re as _re
_NUM = _re.compile(r"0x[0-9a-f]+|\b\d+\b")


def signature(blk, text, insns):
    """A block's instructions with their numbers taken out: unrolled copies match."""
    out = []
    a = blk[0]
    while a < blk[1]:
        t = text.get("%06X" % a, "?")
        out.append(_NUM.sub("#", t))
        a += insns.get(a, 1)
    return tuple(out)


def dead_functions(d, abi):
    """Functions nothing live can call: no caller per the ABI, or every static caller dead
    (callers all known), not a boundary entry, not reached through a table or a pointer."""
    try:
        import xn_boundary
        ents = set(xn_boundary.entries())
    except Exception:       # noqa: BLE001
        ents = set()
    callers = collections.defaultdict(set)
    for b in d["blocks"]:
        for t in d["calls"].get("%06X" % b[0], []):
            if not t.startswith("int"):
                for f in b[3]:
                    callers[int(t, 16)].add(f)
    smc_dead = set()
    p = os.path.join(ROOT, "docs", "engine", "smc", "groups.csv")
    if os.path.exists(p):
        with open(p, newline="") as fh:
            for r in csv.DictReader(fh):
                if (r.get("constraint") or "").startswith("dead") or "no caller" in (r.get("constraint") or ""):
                    smc_dead.add(int(r["va"], 16))
    dead = {va for va, r in abi.items() if "no caller" in r.get("notes", "") and va not in ents}
    dead |= smc_dead - ents
    for _k in range(20):
        new = set()
        for va, r in abi.items():
            if va in dead or va in ents or r.get("callers") != "known":
                continue
            cs = callers.get(va, set())
            if cs and cs <= dead:
                new.add(va)
        if not new:
            break
        dead |= new
    return dead


ERROR_SERVICES = {"int 0x21/09", "int 0x21/4C"}


def reasons_for(fs, blk, d, abi, names, calls, dead=(), copies=None):
    """A likely reason a block never ran."""
    nm = " ".join(names.get(f, "") for f in fs)
    rows = [abi.get(f, {}) for f in fs]
    if all(f in dead for f in fs):
        return "dead function"
    if copies is not None and blk[0] in copies:
        return "unrolled copy of an executed block"
    subs = {r.get("subsystem", "") for r in rows}
    if subs & {"helmet", "serial"} or "helmet" in nm or "serial" in nm:
        return "helmet or serial hardware"
    tg = calls.get("%06X" % blk[0], [])
    if any(t in ("%06X" % k for k in FATAL) for t in tg) or any(t in ERROR_SERVICES for t in tg):
        return "error or fatal path"
    if "vesa" in nm or "banked" in nm:
        return "VESA"
    if any(w in nm for w in ("debug", "bench", "zen", "profile", "editor", "dump", "test")):
        return "debug or benchmark"
    if any(w in nm for w in ("joy", "_ps2", "change_mode", "restore_mode", "shutdown", "free",
                             "remove", "close", "crit_error")):
        return "hardware absent, shutdown or teardown"
    return "not reached"


def report(top=60):
    import xn_abi
    d = load_cfg()
    abi = xn_abi.read_abi()
    names = {va: r["name"] for va, r in abi.items()}
    ex = executed()
    if not ex:
        raise SystemExit("no coverage yet (xn_cover.py corpus / scenarios)")
    cov = bytes(OBJ2[1] - OBJ2[0])
    srcs = {}
    for k, v in ex.items():
        cov = _bor(cov, v)
        srcs[k] = v
    blocks = d["blocks"]
    insns = {int(k, 16): v for k, v in d["insns"].items()}
    calls = d["calls"]

    def hit(c, a):
        return c[a - OBJ2[0]] != 0
    tot = len(blocks)
    done = sum(1 for b in blocks if hit(cov, b[0]))
    ni = len(insns)
    di = sum(1 for a in insns if hit(cov, a))
    by_src = {k: sum(1 for b in blocks if hit(v, b[0])) for k, v in srcs.items()}
    corpus_only = srcs.get("corpus")
    scen = bytes(len(cov))
    for k, v in srcs.items():
        if k.startswith("scen_"):
            scen = _bor(scen, v)
    per = collections.defaultdict(lambda: [0, 0])
    per_sub = collections.defaultdict(lambda: [0, 0])
    missing = collections.defaultdict(list)
    dead = dead_functions(d, abi)
    text = d.get("text", {})
    # unrolled bodies: an unexecuted block whose instructions, numbers aside, are those of an
    # executed block of the same function is a copy of code that ran
    sigs = collections.defaultdict(set)
    for b in blocks:
        if hit(cov, b[0]):
            sigs[b[3][0]].add(signature(b, text, insns))
    copies = {b[0] for b in blocks if not hit(cov, b[0]) and len(sigs[b[3][0]]) and
              signature(b, text, insns) in sigs[b[3][0]]}
    for b in blocks:
        h = hit(cov, b[0])
        for f in b[3]:
            per[f][1] += 1
            per[f][0] += h
        f0 = b[3][0]
        sub = abi.get(f0, {}).get("subsystem", "") or "?"
        per_sub[sub][1] += 1
        per_sub[sub][0] += h
        if not h:
            missing[reasons_for(b[3], b, d, abi, names, calls, dead, copies)].append(b)
    lines = ["# XnGine asm coverage (tools/xn_cover.py report)", ""]
    lines.append("**%d / %d basic blocks executed (%.1f%%)**, %d / %d instructions (%.1f%%)." % (
        done, tot, 100.0 * done / tot, di, ni, 100.0 * di / ni))
    lines.append("")
    lines.append("Sources: " + ", ".join("%s %d blocks" % kv for kv in sorted(by_src.items())))
    if corpus_only is not None:
        sc = sum(1 for b in blocks if hit(scen, b[0]))
        both = sum(1 for b in blocks if hit(scen, b[0]) and not hit(corpus_only, b[0]))
        lines.append("Scenarios: %d blocks, %d of them not in the corpus." % (sc, both))
    lines.append("")
    lines.append("## Not executed, by likely reason")
    lines.append("")
    lines.append("| reason | blocks | instructions | functions |")
    lines.append("|---|---|---|---|")
    justified = 0
    for why, bs in sorted(missing.items(), key=lambda kv: -len(kv[1])):
        fs = sorted({f for b in bs for f in b[3]})
        lines.append("| %s | %d | %d | %d |" % (why, len(bs), sum(b[2] for b in bs), len(fs)))
        if why != "not reached":
            justified += len(bs)
    lines.append("")
    lines.append("Executed or justified: %d / %d blocks (%.1f%%)." % (
        done + justified, tot, 100.0 * (done + justified) / tot))
    lines.append("")
    lines.append("## Per subsystem")
    lines.append("")
    lines.append("| subsystem | blocks executed | of | % |")
    lines.append("|---|---|---|---|")
    for sub, (h, t) in sorted(per_sub.items(), key=lambda kv: -kv[1][1]):
        lines.append("| %s | %d | %d | %.0f |" % (sub, h, t, 100.0 * h / t))
    lines.append("")
    lines.append("## Functions with the most blocks never executed (not dead, not hardware)")
    lines.append("")
    nr = collections.Counter()
    for b in missing.get("not reached", []):
        nr[b[3][0]] += 1
    for f, n in nr.most_common(top):
        h, t = per[f]
        lines.append("- %06X %s: %d of %d blocks not executed (%s)" % (
            f, names.get(f, "?"), n, t, ", ".join("%06X" % b[0] for b in missing["not reached"]
                                                   if b[3][0] == f)[:120]))
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "report.md"), "w") as fh:
        fh.write("\n".join(lines) + "\n")
    with open(os.path.join(OUT, "functions.csv"), "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["va", "name", "subsystem", "blocks", "executed", "percent", "callers", "notes"])
        for f, (h, t) in sorted(per.items()):
            r = abi.get(f, {})
            w.writerow(["%06X" % f, names.get(f, ""), r.get("subsystem", ""), t, h,
                        "%.0f" % (100.0 * h / t), r.get("callers", ""), r.get("notes", "")[:80]])
    print("\n".join(lines[:20]))
    print("-> %s" % os.path.relpath(os.path.join(OUT, "report.md"), ROOT))
    return {"blocks": tot, "executed": done, "justified": justified, "insns": ni, "insns_done": di}


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("cfg")
    c = sub.add_parser("corpus")
    c.add_argument("-j", "--jobs", type=int, default=None)
    s = sub.add_parser("scenarios")
    s.add_argument("names", nargs="*")
    s.add_argument("-j", "--jobs", type=int, default=None)
    r = sub.add_parser("report")
    r.add_argument("--top", type=int, default=60)
    a = ap.parse_args()
    if a.cmd == "cfg":
        cfg()
    elif a.cmd == "corpus":
        return corpus(a.jobs)
    elif a.cmd == "scenarios":
        return scenarios(a.names, a.jobs)
    elif a.cmd == "report":
        report(a.top)
    return 0


if __name__ == "__main__":
    sys.exit(main())
