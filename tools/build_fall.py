#!/usr/bin/env python3
"""Build FALL.EXE from src/ and verify it is byte-identical to the original.

Adapted from KKND-Decomp's build_kknd.py (CC0). Pipeline:
  1. Load orig/1.07.213/FALL.EXE (tools/le.py) as raw, unrelocated object images.
  2. Compile every src/**/*.c with Watcom C32 10.0a, the game's own compiler, under
     DOSBox-X (tools/wcc10.py; flags: config/cflags.txt, or a first-line
     `/* cflags: ... */` override in the file), and assemble src/**/*.asm with its WASM,
     into OMF objects: one DOSBox-X run for the lot.
  3. Check every function defined in C against the original at its address
     (config/functions.csv + config/symbols.txt):
       - every non-relocated byte must be identical, and the length must equal the original;
       - every relocation must resolve to the SAME target as the original: a call/jmp rel32
         to the same function (inside an object the original holds a raw displacement,
         between objects an LE rel32 fixup), an absolute reference to the same LE fixup
         target (+ addend).
     A function that fails is a build failure. Functions not yet in C keep the original bytes.
  4. Splice the checked bytes into the object images, writing each relocated field as a
     linker would (a rel32 displacement inside an object, 0 for an LE rel32 fixup, the
     object-relative offset for an LE off32 fixup), rewrite the LE pages, and compare the
     file's SHA-1 with config/fall.sha1. The LE fixup tables are kept as they are: step 3
     proved every relocation in the new code is one the original already has.

Success prints `build/FALL.EXE: OK`. Anything else is a failure.

usage: build_fall.py [-v] [--blank]
"""
import argparse
import csv
import glob
import hashlib
import os
import re
import shutil
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE, SRC_OFF32, SRC_REL32  # noqa: E402
from omf import OMF  # noqa: E402
import match  # noqa: E402
import wcc10  # noqa: E402

ROOT = match.ROOT
OUT = os.path.join(ROOT, "build", "FALL.EXE")


def file_flags(path, default):
    with open(path, encoding="latin1") as f:
        first = f.readline()
    m = re.match(r"\s*/\*\s*cflags:\s*(.*?)\s*\*/", first)
    return m.group(1).split() if m else default


def c_name(pub):
    """OMF public -> C name. Watcom's register convention: functions get a trailing `_`,
    data a leading `_`."""
    if pub.endswith("_"):
        return pub[:-1]
    if pub.startswith("_"):
        return pub[1:]
    return pub


def load_symbols():
    syms = {}
    p = os.path.join(ROOT, "config", "symbols.txt")
    if os.path.exists(p):
        for line in open(p, encoding="utf-8"):
            m = re.match(r"\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;", line)
            if m:
                syms[m.group(1)] = int(m.group(2), 16)
    return syms


# C runtime helpers the compiler calls by name, at their FALL.EXE addresses
RUNTIME = {"__CHP": 0xA167C}


def resolve(name, funcs, syms):
    if name in RUNTIME:
        return RUNTIME[name]
    n = c_name(name)
    if n in funcs:
        return funcs[n][0]
    if n in syms:
        return syms[n]
    m = re.match(r"(?:func|D|jtbl|sub)_([0-9A-Fa-f]{8})$", n)
    if m:
        return int(m.group(1), 16)
    return None


def check_relocs(obj, si, lo, off, size, va, tgt, fix_at, funcs, syms, le, fields):
    """Return None if every relocation in [lo, off+size) of segment si resolves to the
    original's target, else a reason. Fills `fields` with {field va: (size, file value)}: what
    a linker would write there."""
    for fx in obj.fixups:
        if fx.seg != si or not (lo <= fx.offset < off + size):
            continue
        field = va + fx.offset - off
        addend = struct.unpack_from("<i", obj.data[si], fx.offset)[0]
        if fx.target_kind == "seg" and fx.target_index == si:
            want, addend = va - off + addend, 0  # own segment: switch table or case label
        elif fx.target_kind != "ext":
            return "local segment reference at +%#x (move data to an extern D_ symbol)" % (
                fx.offset - off)
        else:
            want = resolve(fx.target, funcs, syms)
        if want is None:
            return "unknown symbol %s" % fx.target
        of = fix_at.get(field)
        if fx.selfrel:
            if of is not None and of.kind == SRC_REL32:
                have = of.target_va          # call into the other code object
            else:
                rel32 = struct.unpack_from("<i", tgt.bytes_at(field, 4))[0]
                have = field + 4 + rel32
            if have != want + addend:
                fields["bad_at"] = field
                return "call/jmp target %s (%#x) != original %#x" % (fx.target, want, have)
            # between objects the LE loader fills the field in and the file holds 0
            value = 0 if of is not None else want + addend - (field + 4)
            fields[field] = (fx.size, value)
        else:
            if of is None or of.kind != SRC_OFF32 or of.target_va != want + addend:
                fields["bad_at"] = field
                return "reference %s+%d (%#x) != original %s" % (
                    fx.target, addend, want + addend,
                    "%#x" % of.target_va if of else "no fixup")
            # the file holds the offset into the target object, the loader adds its base
            fields[field] = (fx.size, want + addend - le.objs[of.target_obj - 1].base)
    return None


def compile_all(srcs, default, objdir):
    """Compile every source with Watcom 10.0a, grouped by flags: {src: obj path or None}."""
    groups = {}
    for s in srcs:
        flags = default if s.endswith(".asm") else file_flags(s, default)
        groups.setdefault(tuple(flags), []).append(s)
    out = {}
    for flags, group in groups.items():
        objs, td = wcc10.compile_many(group, list(flags))
        for s, o in objs.items():
            rel = os.path.relpath(s, ROOT)
            dst = os.path.join(objdir, re.sub(r"\.(c|asm)$", "", rel.replace(os.sep, "_")) + ".obj")
            out[s] = None
            if o is not None:
                shutil.move(o, dst)
                out[s] = dst
        shutil.rmtree(td, ignore_errors=True)
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("-v", action="store_true", help="print diffs for non-matching functions")
    ap.add_argument("--blank", action="store_true",
                    help="fill each matched function with int3 before splicing (self-test)")
    a = ap.parse_args()

    le = LE(match.EXE)
    raw = le.load(relocate=False)
    fix_at = {f.src_va: f for f in le.fixups()}
    tgt = match.Target()
    funcs = match.symbol_map()          # name -> (va, size)
    syms = load_symbols()
    default = match.default_flags()

    srcs = sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True) +
                  glob.glob(os.path.join(ROOT, "src", "**", "*.asm"), recursive=True))
    matched, errors = [], []
    objdir = os.path.join(ROOT, "build", "obj")
    os.makedirs(objdir, exist_ok=True)
    if not wcc10.available():
        raise SystemExit("Watcom C32 10.0a is not installed: see tools/wcc10.py")
    objs = compile_all(srcs, default, objdir)
    for src in srcs:
        rel = os.path.relpath(src, ROOT)
        obj_path = objs[src]
        if obj_path is None:
            errors.append("%s: Watcom 10.0a could not compile it" % rel)
            continue
        obj = OMF(obj_path)
        fns = obj.functions()
        for pub, si, off, size in fns:
            name = c_name(pub)
            if name not in funcs:
                errors.append("%s: %s is not a known function (add it to config/symbols.txt)"
                              % (rel, name))
                continue
            va, osize = funcs[name]
            ok, diff = match.compare(tgt, obj, name, va, osize, quiet=not a.v)
            if not ok:
                errors.append("%s: %s does not match (%d bytes differ)" % (rel, name, diff))
                continue
            # A switch table emitted ahead of the function's code would sit between the
            # previous public and this one, so check from the end of the previous function.
            lo = max([o + z for _p, s2, o, z in fns if s2 == si and o < off] + [0])
            fields = {}
            bad = check_relocs(obj, si, lo, off, size, va, tgt, fix_at, funcs, syms, le, fields)
            if bad:
                errors.append("%s: %s: %s" % (rel, name, bad))
                continue
            # Splice: our bytes, with every relocated field written as a linker would.
            ours = bytearray(obj.data[si][lo:off + osize])
            base = va - (off - lo)
            for field, (n, value) in fields.items():
                k = field - base
                ours[k:k + n] = (value & (2 ** (8 * n) - 1)).to_bytes(n, "little")
            if lo < off and tgt.bytes_at(base, off - lo) != bytes(ours[:off - lo]):
                errors.append("%s: %s: leading table bytes differ" % (rel, name))
                continue
            o = le.obj_of_va(va)
            buf = raw[o.index]
            if a.blank:
                # prove the output comes from our code: wipe the original function first
                buf[base - o.base:base - o.base + len(ours)] = b"\xcc" * len(ours)
            buf[base - o.base:base - o.base + len(ours)] = ours
            matched.append((va, name, rel))

    for e in errors:
        print("ERROR " + e)

    # rewrite the LE pages from the object images
    out = bytearray(le.data)
    for o in le.objs:
        buf = raw[o.index]
        for k in range(o.page_count):
            fo, ln, _ = le.pages[o.page_index - 1 + k]
            chunk = bytes(buf[k * le.page_size: k * le.page_size + ln])
            out[fo:fo + len(chunk)] = chunk
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    open(OUT, "wb").write(out)
    with open(os.path.join(ROOT, "build", "matched.txt"), "w", newline="\n") as f:
        for va, name, rel in sorted(matched):
            f.write("0x%08X %s %s\n" % (va, name, rel.replace(os.sep, "/")))

    sha = hashlib.sha1(out).hexdigest()
    want = open(os.path.join(ROOT, "config", "fall.sha1")).read().split()[0]
    # progress per region (config/regions.csv): the game's own C is what counts
    with open(os.path.join(ROOT, "config", "regions.csv"), newline="") as f:
        regions = list(csv.DictReader(f))
    by_va = {v: sz for v, sz in funcs.values()}
    done_va = {va for va, _n, _r in matched}
    for g in regions:
        lo, hi = int(g["start"], 16), int(g["end"], 16)
        vas = [v for v in by_va if lo <= v < hi]
        dv = [v for v in vas if v in done_va]
        if g["region"] == "game" or dv:
            print("%-8s %4d / %4d functions, %6d / %6d bytes (%.2f%%)" % (
                g["region"], len(dv), len(vas), sum(by_va[v] for v in dv),
                sum(by_va[v] for v in vas),
                100.0 * sum(by_va[v] for v in dv) / max(1, sum(by_va[v] for v in vas))))
    # numbers for the README badges (tools/update_readme_progress.py)
    import json
    game = next(g for g in regions if g["region"] == "game")
    lo, hi = int(game["start"], 16), int(game["end"], 16)
    gvas = [v for v in by_va if lo <= v < hi]
    gdone = [v for v in gvas if v in done_va]
    with open(os.path.join(ROOT, "build", "progress.json"), "w") as f:
        json.dump({"functions_done": len(gdone), "functions_total": len(gvas),
                   "bytes_done": sum(by_va[v] for v in gdone),
                   "bytes_total": sum(by_va[v] for v in gvas),
                   "matching": sha == want and not errors}, f)
    rel_out = os.path.relpath(OUT, ROOT)
    if sha == want and not errors:
        print("%s: OK" % rel_out)
        return 0
    print("%s: FAILED (sha1 %s, %d errors)" % (rel_out, sha, len(errors)))
    return 1


if __name__ == "__main__":
    sys.exit(main())
