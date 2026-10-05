#!/usr/bin/env python3
"""XnGine's readable C (src/engine/): skeletons, the build, routing, tests and the report.

The readable C replaces object 2's asm one function at a time. Each C function keeps its asm
interface (config/xngine_abi.csv, tools/xn_abi.py), is compiled by Watcom C32 10.0a and runs
inside the game on the game's own stack (src/engine/xngine.h says how). The asm stays loaded:
an asm caller reaches the C through a code hook at the function's asm entry, and the C reaches
asm code by calling its address.

  skel SUBSYS  src/engine/x<subsys>.h and <subsys>.c to start from (build/xn_readable/skel/
               when they exist): a declaration per function with the pragma its ABI row
               gives, its name, evidence and asm in a comment; bodies under `#if 0`.
  build        compile src/engine/*.c and *.asm (tools/wcc10.py, one DOSBox-X session) and link
               them (tools/xn_cload.py link) at RBASE, a region the emulator maps outside the
               memory records compare. Names the objects do not define resolve to the loaded
               game: functions and globals of config/names.csv (NAME_ and _NAME as Watcom
               decorates them), asm_NAME (always the asm entry), xn_data_XXXXXX and
               xn_code_XXXXXX (any address), func_XXXXXXXX and D_XXXXXXXX. A C function named
               like an object-2 function is converted; NAME_r makes it a glue route.
               -> build/xn_readable/image.pkl
  route        the routing table: each converted function's asm entry goes straight to its C;
               or, when its callers need EAX kept (Watcom's code never keeps it), to a stub
               `push eax; call C; pop eax; ret N`; or (NAME_r defined: several outputs, flags
               out) to a stub `pushfd; pushad; mov eax, esp; call NAME_r; popad; popfd; ret N`.
               Checks each route's declared interface (its #pragma aux, or Watcom's default)
               against the ABI row.
  test         replay records with converted functions routed to C and compare as the
               interface sees them (xn_abi.abi_compare): memory writes exactly except the dead
               stack below the exit ESP; registers except the row's clobbers (preserved ones
               against their entry values); flags only flags_out; port I/O and interrupts
               exactly. FUNC/SUBSYS: their records, one function routed at a time;
               --all: the same records with every converted function routed at once;
               --corpus: every record of the corpus with every converted function routed
               (each compared by its own function's row).
  diff         differential tests (tools/xn_cload.py diff_test): made-up entry states, the asm
               recorded from each, the C replaying it; --untested: converted functions with no
               records.
  play         the game with every converted function in C; --compare runs the asm too from
               the same snapshot and compares the screens pixel for pixel.
  report       build/xngine/rc_report.csv: per function, converted, records passed/total, all
               routed, diff passed/total, the clobber and input tests (tools/xn_abi.py).
  asm FUNC     the function's asm with names; abi FUNC: its ABI row.

An agent converting a subsystem works in its own folder: XN_RC_SRC=DIR adds its C files and
headers to src/engine's; XN_RC_OUT=DIR keeps its image, results, report and proposed names
(names.csv) apart; XN_ABI and XN_ABI_OVERRIDES (tools/xn_abi.py) give it its own copy of the
ABI table. The coordinator merges finished work into src/engine and config/.

usage: xn_rc.py skel SUBSYS [--force] [--out DIR]
       xn_rc.py build
       xn_rc.py route
       xn_rc.py test [FUNC|SUBSYS ...] [--all] [--corpus] [-j N] [--max-per N] [-v]
       xn_rc.py diff [FUNC|SUBSYS ...] [--untested] [--trials N]
       xn_rc.py play SNAP [--ticks N] [--shot PNG] [--asm] [--compare] [--script INPUT]
       xn_rc.py report
       xn_rc.py asm FUNC | abi FUNC
"""
import argparse
import collections
import csv
import glob
import json
import os
import pickle
import re
import shutil
import struct
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src", "engine")
BASE_OUT = os.path.join(ROOT, "build", "xn_readable")
# A conversion agent works in a folder of its own: XN_RC_OUT holds its image, results and
# report (and names.csv, its proposed names); XN_RC_SRC (folders separated by ':') adds its
# C files and headers to src/engine's, replacing any of the same name.
OUT = os.environ.get("XN_RC_OUT") or BASE_OUT
EXTRA_SRC = [d for d in os.environ.get("XN_RC_SRC", "").split(os.pathsep) if d]
WORK = os.path.join(OUT, "work")
IMAGE = os.path.join(OUT, "image.pkl")
REPORT = os.path.join(ROOT, "build", "xngine", "rc_report.csv") if OUT == BASE_OUT else \
    os.path.join(OUT, "rc_report.csv")
RESULTS = os.path.join(OUT, "results.json")
LOAD = 0x01000000
RBASE = 0x11000000          # above the literal C's region (0x10000000, 16 MB)
RSIZE = 0x00400000
FLAGS = "-mf -4r -s -zl -zld -ox -bt=dos -w3 -zq".split()


# --------------------------------------------------------------------------------------------
# Names

_names = None
# config/names.csv, then the names the conversion proposes (merged into it by the coordinator)
NAME_FILES = [os.path.join(ROOT, "config", "names.csv"), os.path.join(BASE_OUT, "names.csv")] + \
    ([os.path.join(OUT, "names.csv")] if OUT != BASE_OUT else [])


def names():
    """{name: (kind, address)} of config/names.csv and build/xn_readable/names.csv (proposed
    names, for the coordinator to merge), and {address: (name, confidence, evidence)} for
    functions."""
    global _names
    if _names is None:
        by_name, funcs = {}, {}
        for p in NAME_FILES:
            if not os.path.exists(p):
                continue
            with open(p, newline="") as f:
                for r in csv.DictReader(f):
                    if r["kind"] not in ("func", "global"):
                        continue                # struct fields: "record+0xNN"
                    a = int(r["address"], 16)
                    if r["name"] in by_name:
                        continue                # config/names.csv first
                    by_name[r["name"]] = (r["kind"], a)
                    if r["kind"] == "func":
                        funcs.setdefault(a, (r["name"], r["confidence"], r["evidence"]))
        # the patch fields' names the self-modifying-code designs propose
        pf = os.path.join(BASE_OUT, "smc", "patch_fields.csv")
        if os.path.exists(pf):
            with open(pf, newline="") as f:
                for r in csv.DictReader(f):
                    if r.get("proposed_name") and r["proposed_name"] not in by_name:
                        by_name[r["proposed_name"]] = ("global", int(r["field"], 16))
        _names = (by_name, funcs)
    return _names


def func_va(text):
    """A function given as a name, VA (hex) or func_XXXXXXXX."""
    by_name, _f = names()
    if text in by_name and by_name[text][0] == "func":
        return by_name[text][1]
    return int(text.replace("func_", ""), 16)


def subsystem_funcs(sub):
    import xn_abi
    abi = xn_abi.read_abi()
    return sorted(va for va, r in abi.items() if r["subsystem"] == sub)


def expand(args):
    """FUNC and SUBSYS arguments -> function VAs."""
    import xn_abi
    abi = xn_abi.read_abi()
    subs = {r["subsystem"] for r in abi.values()}
    out = []
    for a in args:
        if a in subs:
            out += subsystem_funcs(a)
        else:
            out.append(func_va(a))
    return out


def resolver():
    """name -> linear address in the loaded game, for symbols the C does not define."""
    by_name, _f = names()

    def resolve(sym):
        base = sym
        if sym.startswith("_"):
            base = sym[1:]
        elif sym.endswith("_"):
            base = sym[:-1]
        asm = base.startswith("asm_")
        if asm:
            base = base[4:]
        m = re.match(r"(?:xn_data_|xn_code_|D_|func_)([0-9A-Fa-f]{6,8})$", base)
        if m:
            return LOAD + int(m.group(1), 16)
        e = by_name.get(base)
        if e is not None:
            return LOAD + e[1]
        return None
    return resolve


# --------------------------------------------------------------------------------------------
# Build

def _files(pattern):
    """src/engine's files matching pattern, those of the XN_RC_SRC folders replacing any of
    the same name."""
    by = {}
    for d in [SRC] + EXTRA_SRC:
        for p in glob.glob(os.path.join(d, pattern)):
            by[os.path.basename(p).lower()] = p
    return [by[k] for k in sorted(by)]


def sources():
    return _files("*.c") + _files("*.asm")


def headers():
    hs = _files("*.h")
    bad = [h for h in hs if not re.match(r"^[a-z0-9_]{1,8}\.h$", os.path.basename(h))]
    if bad:
        raise SystemExit("header names must be 8.3 (DOSBox): %s" % " ".join(bad))
    return hs


def keeps_eax(row):
    """True when the row's callers read EAX after the call without it being an output: a C
    function cannot keep it (Watcom's code uses EAX freely, `modify exact` or not)."""
    import xn_abi
    return (row["clob"] | row["out"]) & xn_abi.RMASK["eax"] != xn_abi.RMASK["eax"]


def build(flags=None):
    import wcc10
    import xn_cload
    t0 = time.time()
    srcs = sources()
    if not srcs:
        print("nothing in src/engine")
        return 1
    flags = flags.split() if isinstance(flags, str) else (flags or FLAGS)
    shutil.rmtree(WORK, ignore_errors=True)
    os.makedirs(WORK)
    for h in headers():
        shutil.copyfile(h, os.path.join(WORK, os.path.basename(h).upper()))
    objs, td = wcc10.compile_many(srcs, flags, workdir=WORK)
    failed = 0
    warnings = []
    for k, s in enumerate(srcs):
        e = os.path.join(td, "N%04d.ERR" % k)
        msg = open(e, errors="replace").read().strip() if os.path.exists(e) else ""
        name = os.path.relpath(s, ROOT)
        msg = msg.replace("N%04d.C" % k, name).replace("N%04d.ASM" % k, name)
        if objs[s] is None or "Error!" in msg:
            failed += 1
            print("FAILED", name)
            print("\n".join(msg.splitlines()[-15:]))
        elif msg:
            warnings.append(msg)
    for w in warnings:
        print(w)
    if failed:
        return 1
    try:
        img, syms = xn_cload.link([objs[s] for s in srcs], RBASE, resolver())
    except SystemExit as e:
        print("link:", e)
        return 1
    by_name, funcs = names()
    obj2 = {n: a for n, (k, a) in by_name.items() if k == "func" and 0xC0000 <= a < 0x161568}
    conv = {}
    for sym, addr in syms.items():
        if not sym.endswith("_"):
            continue
        n = sym[:-1]
        if n.endswith("_r") and n[:-2] in obj2:
            conv.setdefault(obj2[n[:-2]], {})["glue"] = addr
        elif n in obj2:
            conv.setdefault(obj2[n], {})["c"] = addr
    import xn_abi
    abi = xn_abi.read_abi()
    stubs = bytearray()
    pos = RBASE + ((len(img) + 0xFF) & ~0xFF)
    routes = {}
    for va in sorted(conv):
        c = conv[va]
        if "glue" in c:
            a = pos + len(stubs)
            rp = abi[va]["ret_pop"] if va in abi else 0
            b = b"\x9C\x60\x89\xE0\xE8" + struct.pack("<i", c["glue"] - (a + 9)) + b"\x61\x9D"
            b += b"\xC2" + struct.pack("<H", rp) if rp else b"\xC3"
            stubs.extend(b + b"\x90" * (-len(b) % 16))
            routes[va] = {"to": a, "kind": "glue", "c": c.get("c"), "glue": c["glue"]}
        elif va in abi and keeps_eax(abi[va]) and not abi[va]["stack_args"]:
            # the callers need EAX kept, and Watcom's code never keeps it: a stub does
            a = pos + len(stubs)
            rp = abi[va]["ret_pop"]
            b = b"\x50\xE8" + struct.pack("<i", c["c"] - (a + 6)) + b"\x58"
            b += b"\xC2" + struct.pack("<H", rp) if rp else b"\xC3"
            stubs.extend(b + b"\x90" * (-len(b) % 16))
            routes[va] = {"to": a, "kind": "keep-eax", "c": c["c"]}
        else:
            routes[va] = {"to": c["c"], "kind": "direct", "c": c["c"]}
    data = bytes(img) + b"\0" * (pos - RBASE - len(img)) + bytes(stubs)
    if len(data) > RSIZE:
        raise SystemExit("the image is bigger than the region (%d bytes)" % len(data))
    os.makedirs(OUT, exist_ok=True)
    with open(IMAGE, "wb") as f:
        pickle.dump({"base": RBASE, "bytes": data, "syms": syms, "routes": routes,
                     "flags": flags, "sources": [os.path.relpath(s, ROOT) for s in srcs]}, f)
    kinds = collections.Counter(r["kind"] for r in routes.values())
    print("compiled %d files in %.0f s; image %d bytes; %d functions converted (%s) -> %s" % (
        len(srcs), time.time() - t0, len(data), len(routes),
        ", ".join("%d %s" % (n, k) for k, n in sorted(kinds.items())), os.path.relpath(IMAGE, ROOT)))
    problems = check_routes(routes)
    for p in problems:
        print("ROUTE", p)
    return 1 if problems else 0


# --------------------------------------------------------------------------------------------
# Routing

class RImage:
    """The readable C in a machine: the image at RBASE, and code hooks that send asm entries
    to their C (the interface xn_cload.replay_c / diff_test / play expect of an image)."""

    def __init__(self, path=IMAGE):
        with open(path, "rb") as f:
            d = pickle.load(f)
        self.bytes, self.syms = d["bytes"], d["syms"]
        self.routes = d["routes"]
        self.funcs = {va: r["to"] for va, r in self.routes.items()}
        self.hits = 0
        self.entered = set()

    def install(self, emu):
        uc = emu.uc
        if not getattr(emu, "_xn_rc_mapped", False):
            uc.mem_map(RBASE, RSIZE)
            emu._xn_rc_mapped = True
        uc.mem_write(RBASE, self.bytes)         # the data and BSS as built, every time

    def route(self, emu, funcs):
        from unicorn import UC_HOOK_CODE
        import fallemu
        hooks = []
        eip = fallemu.R["eip"]
        for va in funcs:
            to = self.funcs.get(va)
            if to is None:
                continue

            def hit(uc, address, size, to, va=va):
                self.hits += 1
                self.entered.add(va)
                uc.reg_write(eip, to)
            a = LOAD + va
            hooks.append(emu.uc.hook_add(UC_HOOK_CODE, hit, to, a, a))
        return hooks

    @staticmethod
    def unroute(emu, hooks):
        for hk in hooks:
            emu.uc.hook_del(hk)


PRAGMA_RE = re.compile(r"#pragma\s+aux\s+(\w+)\s+(?!=)(.*?);", re.S)


def declared_interfaces():
    """{function name: (parm registers, value register, modify registers, caller/routine)}
    from the #pragma aux lines of src/engine/*.h and *.c; prototypes without one: Watcom's."""
    out = {}
    protos = {}
    for p in sorted(glob.glob(os.path.join(SRC, "*.h"))) + sorted(glob.glob(os.path.join(SRC, "*.c"))):
        text = re.sub(r"/\*.*?\*/", " ", open(p, errors="replace").read(), flags=re.S)
        text = text.replace("\\\n", " ")
        for m in PRAGMA_RE.finditer(text):
            name, body = m.group(1), " ".join(m.group(2).split())
            if body.startswith("="):
                continue
            parm = re.search(r"parm\s+(?:(routine|caller)\s*)?((?:\[[^\]]*\]\s*)*)", body)
            value = re.search(r"value\s+\[([^\]]*)\]", body)
            modify = re.search(r"modify\s+(?:exact\s+)?\[([^\]]*)\]", body)
            regs = re.findall(r"\[([^\]]*)\]", parm.group(2)) if parm else []
            out[name] = (regs, value.group(1).strip() if value else None,
                         modify.group(1).split() if modify else [],
                         parm.group(1) if parm and parm.group(1) else "routine")
        for m in re.finditer(r"^[\w\s\*]*?\b(\w+)\s*\(([^;{)]*)\)\s*;", text, re.M):
            protos[m.group(1)] = m.group(2)
    return out, protos


def check_routes(routes):
    """Problems with the routing: a direct route whose ABI row needs glue, or whose declared
    interface may change a register the row says callers read."""
    import xn_abi
    abi = xn_abi.read_abi()
    decl, protos = declared_interfaces()
    _b, funcs = names()
    out = []
    for va, r in sorted(routes.items()):
        row = abi.get(va)
        name = funcs.get(va, ("?",))[0]
        if row is None:
            continue
        if r["kind"] == "glue":
            continue
        outs = row["out"]
        if row["fout"] or bin(outs).count("1") and len(xn_abi.names_of(outs).split()) > 1:
            out.append("%s (%06X): outputs %s%s need a glue function %s_r" % (
                name, va, row["outputs"], " flags " + row["flags_out"] if row["flags_out"] else "",
                name))
            continue
        if keeps_eax(row) and row["stack_args"]:
            out.append("%s (%06X): its callers need EAX kept and it takes stack arguments: "
                       "write the glue %s_r" % (name, va, name))
            continue
        may = 0
        if name in decl:
            parm, value, modify, _how = decl[name]
            for reg in modify + ([value] if value else []):
                may |= xn_abi.mask_of(reg) if reg in xn_abi.NAME else 0
            pregs = set()
            for g in parm:
                pregs |= set(g.split())
            need = set(xn_abi.names_of(row["in"]).split())
            missing = [g for g in need if g not in pregs and not g.endswith(".u")]
            if missing:
                print("note: %s (%06X): the row's inputs %s are not all parameters (%s): fine "
                      "when the C provably does not need them (say why in its comment)" % (
                          name, va, row["inputs"], " ".join(sorted(pregs))))
        else:
            params = protos.get(name, "")
            n = 0 if params.strip() in ("", "void") else params.count(",") + 1
            may = xn_abi.RMASK["eax"] | sum(xn_abi.RMASK[g] for g in xn_abi.WATCOM_ARGS[:min(n, 4)])
        if r["kind"] == "keep-eax":
            may &= ~xn_abi.RMASK["eax"]         # the stub keeps it
        bad = may & ~(row["clob"] | row["out"])
        if bad:
            out.append("%s (%06X): may change %s, which callers read (clobber: %s)" % (
                name, va, xn_abi.names_of(bad), row["clobber"] or "none"))
    return out


def route_table():
    img = RImage()
    _b, funcs = names()
    for va, r in sorted(img.routes.items()):
        print("%06X %-36s %-6s -> %08X%s" % (va, funcs.get(va, ("?",))[0], r["kind"], r["to"],
                                             "  (C %08X)" % r["c"] if r.get("c") and
                                             r["kind"] == "glue" else ""))
    problems = check_routes(img.routes)
    for p in problems:
        print("ROUTE", p)
    print("%d functions routed, %d problems" % (len(img.routes), len(problems)))
    return 1 if problems else 0


# --------------------------------------------------------------------------------------------
# Tests

def comparer(abi):
    import xn_abi

    def for_func(va):
        row = abi.get(va)

        def cmp(rec, got, row=row):
            return xn_abi.abi_compare(rec, got, abi.get(rec["func"], row))
        return cmp
    return for_func


def _rc_test_task(files, sp):
    """Replay the records of these files: those of sp["funcs"] (each with its own function
    routed, or all of sp["route"] with sp["together"]), or every record (sp["corpus"])."""
    import xn_abi
    import xn_cload
    import xn_record
    xn_cload.longer_calls(xn_record)
    abi = xn_abi.read_abi()
    img = RImage()
    want = set(sp["funcs"]) if sp.get("funcs") else None
    every = sorted(img.funcs)
    stats, seen = {}, collections.Counter()
    cmp = comparer(abi)
    for path in files:
        for rec in xn_cload.read_records(xn_record, path):
            va = rec["func"]
            if not sp.get("corpus") and (want is None or va not in want or va not in img.funcs):
                continue
            if sp.get("max_per") and seen[va] >= sp["max_per"]:
                continue
            seen[va] += 1
            img.entered = set()
            route = every if sp.get("together") or sp.get("corpus") else [va]
            diffs = xn_cload.replay_c(xn_record, img, rec, route, cmp(va))
            st = stats.setdefault(va, [0, 0, [], 0])
            st[1] += 1
            st[3] += bool(img.entered)
            if not diffs:
                st[0] += 1
            elif len(st[2]) < 3:
                st[2].append(["%s@%d" % (rec.get("job", ""), rec.get("tick", 0))] + diffs[:4])
            if diffs and sp.get("verbose"):
                print("FAIL %06X: %s" % (va, "; ".join(diffs[:4])), flush=True)
    return {"stats": stats, "hits": img.hits}


def files_with(funcs):
    """The record files holding records of these functions (the job summaries list them)."""
    want = {"%X" % f for f in funcs}
    out = []
    for sp in sorted(glob.glob(os.path.join(ROOT, "build", "xngine", "records", "*.json"))):
        try:
            sm = json.load(open(sp))
        except (OSError, ValueError):
            continue
        if any(r["func"] in want for r in sm.get("records", ())):
            out.append(sp[:-5] + ".pkl")
    return out


def test(funcs, together=False, corpus=False, jobs=None, max_per=0, verbose=False):
    import xn_cload
    t0 = time.time()
    img = RImage()
    if corpus:
        files = xn_cload.record_files(None)
        funcs = None
    else:
        funcs = [f for f in (funcs or sorted(img.funcs)) if f in img.funcs]
        if not funcs:
            print("no converted function to test")
            return 1
        files = files_with(funcs)
    spec = {"funcs": funcs, "together": together, "corpus": corpus, "max_per": max_per,
            "verbose": verbose}
    res = xn_cload.parallel("xn_rc:_rc_test_task", files, spec, jobs)
    stats, hits = {}, 0
    for r in res:
        hits += r["hits"]
        for k, v in r["stats"].items():
            st = stats.setdefault(int(k), [0, 0, [], 0])
            for j in (0, 1, 3):
                st[j] += v[j]
            st[2] += v[2]
    mode = "corpus" if corpus else ("together" if together else "alone")
    save_results(mode, stats)
    _b, names_ = names()
    npass = sum(1 for s in stats.values() if s[0] == s[1])
    print("%s: %d / %d functions pass all their records (%d / %d records) in %.0f s; C entered "
          "%d times" % (mode, npass, len(stats), sum(s[0] for s in stats.values()),
                        sum(s[1] for s in stats.values()), time.time() - t0, hits))
    if corpus:
        ran = sum(s[3] for s in stats.values())
        print("  %d records ran some C" % ran)
    for va, s in sorted(stats.items()):
        if s[0] != s[1]:
            print("  %06X %-34s %d/%d  %s" % (va, names_.get(va, ("?",))[0], s[0], s[1],
                                             " | ".join("; ".join(d) for d in s[2][:2])[:300]))
    if not corpus:
        missing = [f for f in funcs if f not in stats]
        if missing:
            print("  no records: %s (run diff)" % " ".join("%06X" % f for f in missing))
    write_report()
    return 0 if npass == len(stats) else 1


def save_results(mode, stats):
    res = load_results()
    d = res.setdefault(mode, {})
    for va, s in stats.items():
        d["%06X" % va] = [s[0], s[1], "; ".join(s[2][0][1:3]) if s[2] else "", s[3]]
    res.setdefault("image", {})["built"] = os.path.getmtime(IMAGE)
    os.makedirs(OUT, exist_ok=True)
    with open(RESULTS, "w") as f:
        json.dump(res, f, indent=0)


def load_results():
    if os.path.exists(RESULTS):
        with open(RESULTS) as f:
            return json.load(f)
    return {}


def diff(funcs, untested=False, trials=8, verbose=False):
    import xn_abi
    import xn_cload
    img = RImage()
    funcs = funcs or sorted(img.funcs)
    if untested:
        res = load_results().get("alone", {})
        funcs = [f for f in funcs if not res.get("%06X" % f, [0, 0])[1]]
    funcs = [f for f in funcs if f in img.funcs]
    if not funcs:
        print("nothing to test")
        return 0
    abi = xn_abi.read_abi()
    out = {}

    def report(res):
        out.update(res)
        r = load_results()
        d = r.setdefault("diff", {})
        for va, v in res.items():
            d["%06X" % va] = list(v)
        with open(RESULTS, "w") as f:
            json.dump(r, f, indent=0)
    rc = xn_cload.diff_test(funcs, trials=trials, verbose=verbose, img=img,
                            compare_for=comparer(abi), report=report,
                            fail_dir=os.path.join(OUT, "fail"))
    write_report()
    return rc


def play(snap, ticks, shot=None, asm=False, compare=False, script=""):
    import xn_cload
    if compare:
        import fallemu
        screens = []
        for use_c in (False, True):
            img = RImage() if use_c else None
            path = snap if os.path.exists(snap) else os.path.join(fallemu.SNAPS, snap + ".snap")
            emu = fallemu.Emu.load(path, overlay=os.path.join(OUT, "overlay_play"))
            try:
                if img:
                    img.install(emu)
                    img.route(emu, sorted(img.funcs))
                start = emu.ticks
                ok = emu.run(start + ticks, [(start + t, a, g) for t, a, g in
                                             fallemu.parse_script(script)])
                screens.append((ok, emu.mode, bytes(emu.read(0xA0000, 64000)),
                                bytes(bytearray(sum((list(c) for c in emu.palette), [])))
                                if hasattr(emu, "palette") else b"", img.hits if img else 0))
                if shot:
                    p = shot if not use_c else shot.replace(".png", "") + "_c.png"
                    if not use_c:
                        p = shot.replace(".png", "") + "_asm.png"
                    emu.screenshot(p)
            finally:
                emu.close()
        (ok1, m1, s1, p1, _h), (ok2, m2, s2, p2, hits) = screens
        same = s1 == s2 and p1 == p2
        diff_px = sum(1 for a, b in zip(s1, s2) if a != b)
        print("%s: %d ticks; asm %s, readable C %s (C entered %d times); screens %s" % (
            os.path.basename(snap), ticks, "ok" if ok1 else "stopped", "ok" if ok2 else "stopped",
            hits, "identical" if same else "differ in %d pixels%s" % (
                diff_px, "" if p1 == p2 else " and the palette")))
        return 0 if same and ok1 and ok2 else 1
    return xn_cload.play(snap, ticks, all_c=not asm, shot=shot, script=script,
                         img=None if asm else RImage(), what="the readable C")


# --------------------------------------------------------------------------------------------
# Report

COLUMNS = ("function", "name", "subsystem", "convention", "converted", "records_passed",
           "records_total", "first_difference", "together_passed", "together_total",
           "corpus_passed", "corpus_total", "diff_passed", "diff_total", "clobber_test",
           "inputs_test")


def write_report():
    import xn_abi
    abi = xn_abi.read_abi()
    res = load_results()
    routes = {}
    if os.path.exists(IMAGE):
        routes = RImage().routes
    clob = {}
    inp = {}
    for kind, d in (("clobber", clob), ("inputs", inp)):
        p = os.path.join(OUT, kind + ".json")
        if os.path.exists(p):
            j = json.load(open(p))
            for k, v in j.get("stats", {}).items():
                d[k] = v
            if kind == "clobber":
                d["_fired"] = j.get("fired", {})
    rows = []
    for va, r in sorted(abi.items()):
        k = "%06X" % va
        row = {c: "" for c in COLUMNS}
        row.update({"function": k, "name": r["name"], "subsystem": r["subsystem"],
                    "convention": r["convention"],
                    "converted": routes[va]["kind"] if va in routes else "no"})
        a = res.get("alone", {}).get(k)
        if a:
            row["records_passed"], row["records_total"], row["first_difference"] = a[0], a[1], a[2]
        t = res.get("together", {}).get(k)
        if t:
            row["together_passed"], row["together_total"] = t[0], t[1]
        c = res.get("corpus", {}).get(k)
        if c:
            row["corpus_passed"], row["corpus_total"] = c[0], c[1]
        d = res.get("diff", {}).get(k)
        if d:
            row["diff_passed"], row["diff_total"] = d[0], d[1]
        if k in clob or k in clob.get("_fired", {}):
            fired = clob.get("_fired", {}).get(k, 0)
            st = clob.get(k)
            okc = st is None or st[0] == st[1]
            row["clobber_test"] = "%s (scrambled at %d returns)" % ("pass" if okc else "FAIL",
                                                                  fired)
        if k in inp:
            st = inp[k]
            row["inputs_test"] = "%s %d/%d" % ("pass" if st[0] == st[1] else "FAIL", st[0], st[1])
        rows.append(row)
    os.makedirs(os.path.dirname(REPORT), exist_ok=True)
    with open(REPORT, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=COLUMNS)
        w.writeheader()
        w.writerows(rows)
    return rows


def report():
    rows = write_report()
    conv = [r for r in rows if r["converted"] != "no"]
    by = collections.defaultdict(lambda: [0, 0, 0, 0, 0])
    for r in rows:
        b = by[r["subsystem"]]
        b[0] += 1
        if r["converted"] != "no":
            b[1] += 1
            if r["records_total"] and r["records_passed"] == r["records_total"]:
                b[2] += 1
            if r["diff_total"] and r["diff_passed"] == r["diff_total"]:
                b[3] += 1
            if not r["records_total"] and not r["diff_total"]:
                b[4] += 1
    print("%d of %d functions converted -> %s" % (len(conv), len(rows), os.path.relpath(REPORT, ROOT)))
    print("%-10s %5s %9s %13s %10s %9s" % ("subsystem", "funcs", "converted", "records pass",
                                          "diff pass", "untested"))
    for s, b in sorted(by.items(), key=lambda kv: -kv[1][1]):
        if b[1]:
            print("%-10s %5d %9d %13d %10d %9d" % (s, *b))
    return 0


# --------------------------------------------------------------------------------------------
# The asm, with names; skeletons

_prog = None


def asm_listing(va):
    """The asm of a function (as tools/xn_c.py walks it), with names for its targets and the
    globals it addresses."""
    import xn_c
    by_name, funcs = names()
    glob_at = {a: n for n, (k, a) in by_name.items()}
    global _prog
    if _prog is None:
        _prog = xn_c.Program()
    fn = xn_c.Func(_prog, va)
    lines = []
    for a in fn.order:
        x = fn.insns[a]
        i = x.i
        text = "%s %s" % (i.mnemonic, i.op_str)

        def sub(m):
            v = int(m.group(0), 16)
            n = glob_at.get(v)
            if n and (v >= 0xC0000 or v >= 0x10000 and by_name[n][0] == "func"):
                return n
            return m.group(0)
        text = re.sub(r"0x[0-9a-f]{5,8}", sub, text)
        lab = "%06X" % a
        if a in funcs and a != va:
            lab += " <%s>" % funcs[a][0]
        lines.append("%s  %s" % (lab, text))
    return lines


def skel(sub, force=False, out=None):
    """A header and a stub .c for a subsystem, from config/xngine_abi.csv and names.csv."""
    import xn_abi
    abi = xn_abi.read_abi()
    _b, funcs = names()
    vas = subsystem_funcs(sub)
    if not vas:
        print("no subsystem %s" % sub)
        return 1
    hname = ("x" + sub)[:8] + ".h"
    cname = sub[:8] + ".c"
    dest = out or SRC
    if not out and not force and (os.path.exists(os.path.join(SRC, hname)) or
                                  os.path.exists(os.path.join(SRC, cname))):
        dest = os.path.join(OUT, "skel", sub)
    os.makedirs(dest, exist_ok=True)
    guard = "X%s_H" % sub.upper()
    h = ["/* %s: XnGine's %s functions (readable C, src/engine/%s; see xngine.h). */" % (
        hname, sub, cname), "#ifndef %s" % guard, "#define %s" % guard, "",
        '#include "xngine.h"', ""]
    c = ["/* %s: XnGine's %s functions as readable C (see xngine.h, docs/xngine_readable.md)."
         % (cname, sub), "   Functions under #if 0 are still asm; take one out when it passes "
         "`tools/xn_rc.py test`. */", '#include "%s"' % hname, ""]
    for va in vas:
        row = abi[va]
        name, conf, ev = funcs.get(va, ("func_%08X" % va, "", ""))
        decl, pragma, glue = declaration(row, name)
        note = "%s (%06X, %s): %s" % (name, va, conf, ev)
        abi_line = "ABI %s: in %s%s; out %s%s; clobber %s; callers %s%s" % (
            row["convention"], row["inputs"] or "-",
            " +%d stack bytes" % row["stack_args"] if row["stack_args"] else "",
            row["outputs"] or "-", " flags " + row["flags_out"] if row["flags_out"] else "",
            row["clobber"] or "-", row["callers"], "; " + row["notes"] if row["notes"] else "")
        h.append("/* %s\n   %s */" % (xn_c_wrap(note), xn_c_wrap(abi_line)))
        h.append(decl)
        if pragma:
            h.append(pragma)
        if glue:
            h.append("void %s_r(xn_regs *r);      /* the asm interface: see xngine.h */" % name)
        h.append("")
        c.append("#if 0    /* %s %06X */" % (name, va))
        c.append("/* asm:")
        c += ["   " + ln for ln in asm_listing(va)]
        c.append("*/")
        c.append(decl[:-1])
        c.append("{")
        c.append("}")
        if glue:
            c.append("")
            c.append("void %s_r(xn_regs *r)" % name)
            c.append("{")
            c.append("    /* in: %s; out: %s%s */" % (row["inputs"] or "-", row["outputs"] or "-",
                                                    " flags " + row["flags_out"]
                                                    if row["flags_out"] else ""))
            c.append("}")
        c.append("#endif")
        c.append("")
    h.append("#endif")
    with open(os.path.join(dest, hname), "w") as f:
        f.write("\n".join(h) + "\n")
    with open(os.path.join(dest, cname), "w") as f:
        f.write("\n".join(c) + "\n")
    print("%d functions -> %s, %s" % (len(vas), os.path.relpath(os.path.join(dest, hname), ROOT),
                                     os.path.relpath(os.path.join(dest, cname), ROOT)))
    return 0


def xn_c_wrap(text, width=92):
    import xn_c
    return xn_c.wrap(text, width)


REG_TYPE = {"eax": "s32", "ax": "s16", "al": "u8", "ah": "u8"}


def declaration(row, name):
    """(prototype, #pragma aux or "", needs glue) for a function's ABI row."""
    import xn_abi
    ins = xn_abi.names_of(row["in"] & xn_abi.REGS).split()
    outs = xn_abi.names_of(row["out"]).split()
    glue = len(outs) > 1 or bool(row["fout"]) or any(o.endswith(".u") for o in outs) or \
        keeps_eax(row) and row["stack_args"]
    params = []
    for k, r in enumerate(ins):
        if r.endswith(".u"):
            continue
        t = "u8" if r[-1] in "lh" and len(r) == 2 else ("u16" if len(r) == 2 else "s32")
        params.append(("%s a%d" % (t, k + 1), r))
    for k in range(row["stack_args"] // 4):
        params.append(("s32 s%d" % (k + 1), None))
    if glue:
        ret = "void"
        proto = "%s %s(%s);" % (ret, name, ", ".join(p for p, _r in params) or "void")
        return proto, "", True
    ret = "void"
    if outs:
        o = outs[0]
        ret = "u8" if o in ("al", "ah", "bl", "bh", "cl", "ch", "dl", "dh") else (
            "u16" if len(o) == 2 else "s32")
    proto = "%s %s(%s);" % (ret, name, ", ".join(p for p, _r in params) or "void")
    if row["convention"] == "watcom":
        return proto, "", False
    regs = " ".join("[%s]" % r for _p, r in params if r)
    mod = [r for r in ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp")
           if xn_abi.RMASK[r] & ~row["clob"] == 0 and (not outs or r != outs[0])]
    if keeps_eax(row) and "eax" not in mod and (not outs or outs[0] != "eax"):
        mod = ["eax"] + mod             # the route's stub keeps EAX for the callers
    how = "routine" if row["ret_pop"] == row["stack_args"] else "caller"
    if outs:
        mod = [outs[0]] + mod
    pragma = "#pragma aux %s parm %s%s%s modify exact [%s];" % (
        name, "%s " % how if row["stack_args"] else "", regs or "[]",
        " value [%s]" % outs[0] if outs else "", " ".join(mod))
    return proto, pragma, False


# --------------------------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    s = sub.add_parser("skel")
    s.add_argument("subsys")
    s.add_argument("--force", action="store_true")
    s.add_argument("--out", default=None, help="another folder than src/engine")
    b = sub.add_parser("build")
    b.add_argument("--flags", default=None)
    sub.add_parser("route")
    t = sub.add_parser("test")
    t.add_argument("funcs", nargs="*")
    t.add_argument("--all", action="store_true", help="every converted function routed at once")
    t.add_argument("--corpus", action="store_true", help="every record, all converted routed")
    t.add_argument("-j", "--jobs", type=int, default=None)
    t.add_argument("--max-per", type=int, default=0)
    t.add_argument("-v", action="store_true")
    d = sub.add_parser("diff")
    d.add_argument("funcs", nargs="*")
    d.add_argument("--untested", action="store_true")
    d.add_argument("--trials", type=int, default=8)
    d.add_argument("-v", action="store_true")
    p = sub.add_parser("play")
    p.add_argument("snap")
    p.add_argument("--ticks", type=int, default=100)
    p.add_argument("--shot", default=None)
    p.add_argument("--asm", action="store_true")
    p.add_argument("--compare", action="store_true")
    p.add_argument("--script", default="")
    sub.add_parser("report")
    for n in ("asm", "abi"):
        q = sub.add_parser(n)
        q.add_argument("func")
    a = ap.parse_args()
    if a.cmd == "skel":
        return skel(a.subsys, a.force, a.out)
    if a.cmd == "build":
        return build(a.flags)
    if a.cmd == "route":
        return route_table()
    if a.cmd == "test":
        return test(expand(a.funcs), together=a.all, corpus=a.corpus, jobs=a.jobs,
                    max_per=a.max_per, verbose=a.v)
    if a.cmd == "diff":
        return diff(expand(a.funcs), untested=a.untested, trials=a.trials, verbose=a.v)
    if a.cmd == "play":
        return play(a.snap, a.ticks, a.shot, a.asm, a.compare, a.script)
    if a.cmd == "report":
        return report()
    if a.cmd == "asm":
        print("\n".join(asm_listing(func_va(a.func))))
        return 0
    if a.cmd == "abi":
        import xn_abi
        row = xn_abi.read_abi().get(func_va(a.func))
        for k in xn_abi.COLUMNS:
            print("%-11s %s" % (k, row[k]))
        return 0


if __name__ == "__main__":
    sys.exit(main())
