#!/usr/bin/env python3
"""Name FALL.EXE's library functions by matching them against Watcom C32 10.0a's own libraries.

The game links Watcom's register-convention DOS runtime (clib3r), the math libraries and the
387 emulator. Each library module is an OMF object: cut every public function out of it, mask
the bytes its fixups cover (and the bytes FALL.EXE's LE fixups cover), and compare with the
library region's functions (config/regions.csv). Equal bytes, length to within alignment
padding, is a match: the function is that library routine.

The libraries come off the Watcom 10.0a CD (WATCOM_C10A.ISO, as for the compiler):
third_party/watcom10/lib386/WATCOM/LIB386/{DOS/CLIB3R.LIB, MATH3R.LIB, MATH387R.LIB, ...}.

usage: libmatch.py [--write]
    prints the matches and the library functions left over; --write puts the names into
    build/names/library.csv (merge with tools/names.py)
"""
import argparse
import collections
import csv
import glob
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from match import Target  # noqa: E402
from omf import OMF  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
LIBDIR = os.path.join(ROOT, "third_party", "watcom10", "lib386", "WATCOM", "LIB386")


def lib_modules(path):
    """The OMF modules of a library (each from THEADR to MODEND), or the one object."""
    d = open(path, "rb").read()
    if d[0] != 0xF0:
        yield os.path.basename(path), d
        return
    page = struct.unpack_from("<H", d, 1)[0] + 3
    p = page
    while p < len(d) and d[p] != 0xF1:             # F1: the dictionary
        start = p
        name = d[p + 4:p + 4 + d[p + 3]].decode("latin1") if d[p] == 0x80 else "?"
        while True:
            typ = d[p]
            p += 3 + struct.unpack_from("<H", d, p + 1)[0]
            if typ in (0x8A, 0x8B):
                break
        yield name, d[start:p]
        p = (p + page - 1) // page * page


def omf_of(data):
    o = OMF.__new__(OMF)
    o.path = "<lib>"
    o.lnames, o.segs, o.groups, o.exts = [None], [None], [None], [None]
    o.publics, o.data, o.fixups = {}, {}, []
    o._parse(data)
    return o


def library_functions():
    """[(library, module, public name, code bytes, masked offsets)]."""
    out = []
    for path in sorted(glob.glob(os.path.join(LIBDIR, "**", "*.LIB"), recursive=True) +
                       glob.glob(os.path.join(LIBDIR, "**", "*.OBJ"), recursive=True)):
        lib = os.path.relpath(path, LIBDIR)
        for mod, data in lib_modules(path):
            try:
                o = omf_of(data)
            except Exception:                       # noqa: BLE001  (a record we don't read)
                continue
            for name, si, off, sz in o.functions():
                code = bytes(o.data[si][off:off + sz])
                mask = {f.offset - off + k for f in o.fixups if f.seg == si and off <= f.offset < off + sz
                        for k in range(f.size)}
                out.append((lib, mod, name, code, mask))
    return out


def library_region():
    with open(os.path.join(ROOT, "config", "regions.csv"), newline="") as f:
        reg = {r["region"]: (int(r["start"], 16), int(r["end"], 16)) for r in csv.DictReader(f)}
    lo, hi = reg["library"]
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        return [(int(r["va"], 16), int(r["size"])) for r in csv.DictReader(f) if lo <= int(r["va"], 16) < hi]


def same(theirs, tfix, ours, mask):
    if len(ours) > len(theirs):
        return False
    for k in range(len(ours)):
        if k in mask or k in tfix:
            continue
        if ours[k] != theirs[k]:
            return False
    return True


def c_name(pub):
    """Watcom's register convention appends _ to function names; keep the rest as linked
    (with any character C can't use, like the math library's @, made a _)."""
    return ident(pub[:-1] if pub.endswith("_") and not pub.endswith("__") else pub)


def ident(s):
    return re.sub(r"[^A-Za-z0-9_]", "_", s)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--write", action="store_true")
    a = ap.parse_args()
    if not os.path.isdir(LIBDIR):
        raise SystemExit("no Watcom libraries in %s: extract WATCOM/LIB386 from WATCOM_C10A.ISO" % LIBDIR)
    libs = library_functions()
    by_len = collections.defaultdict(list)
    for lib, mod, name, code, mask in libs:
        code = code.rstrip(b"\x00") or code              # segment padding after the last one
        by_len[len(code)].append((lib, mod, name, code, mask))
    tgt = Target()
    region = library_region()
    found, rows = {}, []
    for va, size in region:
        theirs = tgt.bytes_at(va, size)
        tfix = {k for k in range(size) if va + k in tgt.fix}
        hits = []
        for n in range(max(1, size - 15), size + 1):    # the rest of the original: padding
            if any(b not in (0x00, 0x90, 0xCC) for b in theirs[n:]):
                continue
            for lib, mod, name, code, mask in by_len.get(n, ()):
                if same(theirs, tfix, code, mask):
                    hits.append((lib, mod, name))
        if hits and (len({c_name(h[2]) for h in hits}) == 1 or n > 8):
            found[va] = hits                        # a tiny stub matching many: no name
    # a module's public and the static functions after it are one span in the library but
    # several functions in FALL.EXE: match spans from any function start too, and name the
    # functions they cover after their module and offset
    nexts = {va: region[k + 1][0] if k + 1 < len(region) else va + size for k, (va, size) in enumerate(region)}
    statics = {}
    for va, size in region:
        if va in found:
            continue
        for lib, mod, name, code, mask in libs:
            code = code.rstrip(b"\x00") or code
            if len(code) <= size + 15 or len(code) < 16:
                continue
            theirs = tgt.bytes_at(va, len(code))
            tfix = {k for k in range(len(code)) if va + k in tgt.fix}
            if theirs[:1] != code[:1] or not same(theirs, tfix, code, mask):
                continue
            found[va] = [(lib, mod, name)]
            v = nexts[va]
            while v < va + len(code) and v in nexts:
                statics[v] = (lib, mod, name, v - va)
                v = nexts[v]
            break
    near = {}                                       # close, not equal: another version
    for va, size in region:
        if va in found or va in statics or size < 16:
            continue
        theirs = tgt.bytes_at(va, size)
        tfix = {k for k in range(size) if va + k in tgt.fix}
        best = None
        for n in range(int(size * 0.9), int(size * 1.1) + 1):
            for lib, mod, name, code, mask in by_len.get(n, ()):
                m = min(len(code), size)
                eq = sum(1 for k in range(m) if k in mask or k in tfix or code[k] == theirs[k])
                score = eq / max(len(code), size)
                if score >= 0.9 and (best is None or score > best[0]):
                    best = (score, lib, mod, name)
        if best:
            near[va] = best
    names = collections.Counter()
    for va in sorted(found):
        hits = found[va]
        pubs = sorted({c_name(h[2]) for h in hits})
        mods = sorted({"%s:%s" % (h[0], h[1]) for h in hits})
        print("%08X %-28s %s" % (va, "/".join(pubs)[:28], " ".join(mods)[:90]))
        names[pubs[0]] += 1
        rows.append((va, pubs, mods))
    for va in sorted(near):
        score, lib, mod, name = near[va]
        print("%08X ~%-27s %s:%s (%.0f%% of the bytes)" % (va, c_name(name)[:27], lib, mod, 100 * score))
    left = [(va, size) for va, size in region if va not in found and va not in near and va not in statics]
    print("%d of %d library functions matched, %d static functions inside matched modules, %d nearly; %d left" % (
        len(found), len(region), len(statics), len(near), len(left)))
    if a.write:
        os.makedirs(os.path.join(ROOT, "build", "names"), exist_ok=True)
        with open(os.path.join(ROOT, "build", "names", "library.csv"), "w", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            w.writerow(["kind", "address", "name", "confidence", "evidence"])
            used = collections.Counter()
            for va, pubs, mods in rows:
                name = pubs[0]
                used[name] += 1
                if used[name] > 1:                  # the same routine linked twice
                    name = "%s_%d" % (name, used[name])
                w.writerow(["func", "0x%08X" % va, name, "confirmed",
                            "byte-identical (relocations masked) to Watcom C32 10.0a %s%s" % (
                                " ".join(mods)[:150], "; also " + ", ".join(pubs[1:])[:80] if len(pubs) > 1 else "")])
            for va in sorted(statics):
                lib, mod, name, off = statics[va]
                w.writerow(["func", "0x%08X" % va, ident("%s_static_%x" % (mod, off)), "strong",
                            "byte-identical to the code at +0x%X of Watcom C32 10.0a %s:%s, after its public "
                            "%s: a static function (no symbol of its own)" % (off, lib, mod, c_name(name))])
            for va in sorted(near):
                score, lib, mod, name = near[va]
                name = c_name(name)
                used[name] += 1
                if used[name] > 1:
                    name = "%s_%d" % (name, used[name])
                w.writerow(["func", "0x%08X" % va, name, "candidate",
                            "%.0f%% of the bytes equal (relocations masked) to Watcom C32 10.0a %s:%s: "
                            "probably another version of it" % (100 * score, lib, mod)])
        print("wrote build/names/library.csv")


if __name__ == "__main__":
    main()
