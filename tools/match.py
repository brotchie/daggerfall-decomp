#!/usr/bin/env python3
"""Compile one C file with wcc386 and diff each of its functions against FALL.EXE.

Adapted from KKND-Decomp's match.py (CC0):
  1. `bwcc386 <flags> file.c` -> OMF object (tools/omf.py reads it)
  2. every public in a CODE segment named `func_XXXXXXXX` (or listed in config/symbols.txt)
     is cut out and compared to the original bytes at its VA
  3. bytes under a relocation on *either* side are masked; everything else must be identical,
     and the compiled length must equal the original function size.

A match here is necessary, not sufficient: the full rebuild and SHA-1 check is the real test.

usage: match.py src/file.c [--flags "..."] [--func NAME] [-q]
"""
import argparse
import csv
import os
import re
import shutil
import subprocess
import sys
import tempfile

import capstone

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE  # noqa: E402
from omf import OMF  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")


def compiler():
    """$WCC386, else the patched boot compiler from tools/build_ow.sh."""
    cc = os.environ.get("WCC386") or os.path.join(
        ROOT, "third_party", "open-watcom-v2", "build", "binbuild", "bwcc386")
    if not os.path.exists(cc):
        raise SystemExit("no compiler at %s: run tools/build_ow.sh" % cc)
    return cc


def default_flags():
    p = os.path.join(ROOT, "config", "cflags.txt")
    if os.path.exists(p):
        return [w for line in open(p) if not line.startswith("#") for w in line.split()]
    return []


def compile_c(src, flags, obj):
    with tempfile.TemporaryDirectory() as td:
        base = os.path.splitext(os.path.basename(src))[0]
        shutil.copyfile(src, os.path.join(td, base + ".c"))
        cmd = [compiler(), "-q", "-zq"] + flags + [
            "-i=" + os.path.join(ROOT, "include"), "-i=" + os.path.join(ROOT, "src"),
            "-fo=" + base + ".obj", base + ".c"]
        r = subprocess.run(cmd, capture_output=True, text=True, cwd=td)
        out = os.path.join(td, base + ".obj")
        if r.returncode != 0 or not os.path.exists(out):
            sys.stderr.write(r.stdout + r.stderr)
            raise SystemExit("compile failed")
        if r.stdout.strip():
            sys.stderr.write(r.stdout)
        shutil.move(out, obj)


def symbol_map():
    m = {}
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        for r in csv.DictReader(f):
            m[r["name"]] = (int(r["va"], 16), int(r["size"]))
    p = os.path.join(ROOT, "config", "symbols.txt")
    if os.path.exists(p):
        sizes = {va: sz for va, sz in m.values()}
        for line in open(p, encoding="utf-8"):
            mm = re.match(r"\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;", line)
            if mm:
                va = int(mm.group(2), 16)
                if va in sizes:
                    m[mm.group(1)] = (va, sizes[va])
    return m


class Target:
    def __init__(self):
        self.le = LE(EXE)
        self.raw = self.le.load(relocate=False)
        self.fix = set()
        for f in self.le.fixups():
            for k in range(f.size):
                self.fix.add(f.src_va + k)

    def bytes_at(self, va, n):
        o = self.le.obj_of_va(va)
        return bytes(self.raw[o.index][va - o.base: va - o.base + n])


def code_tables():
    with open(os.path.join(ROOT, "config", "code_data.csv"), newline="") as f:
        return [(int(r["va"], 16), int(r["size"])) for r in csv.DictReader(f)]


def compare(tgt, obj, name, va, size, quiet=False):
    """Return (ok, n_diff_bytes). Relocations are masked on both sides."""
    funcs = {n.strip("_"): (si, off, sz) for n, si, off, sz in obj.functions()}
    si, off, csz = funcs[name]
    ours = bytes(obj.data[si][off:off + csz])
    # trailing padding in our segment belongs to alignment, not the function
    if len(ours) > size:
        ours = ours.rstrip(b"\x00")
    mask = set()
    for f in obj.fixups:
        if f.seg == si and off <= f.offset < off + csz:
            for k in range(f.size):
                mask.add(f.offset - off + k)
    theirs = tgt.bytes_at(va, size)
    for k in range(size):
        if va + k in tgt.fix:
            mask.add(k)
    diff = 0
    for k in range(max(len(ours), size)):
        if k in mask:
            continue
        a = ours[k] if k < len(ours) else None
        b = theirs[k] if k < size else None
        if a != b:
            diff += 1
    ok = diff == 0 and len(ours) == size
    if not ok and not quiet:
        md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        # step over switch tables (data in the code) on both sides, and walk the two lists
        # in step so a branch that changed size doesn't shift everything after it
        tables = sorted((t, n) for t, n in code_tables() if va <= t < va + size)

        def decode(code):
            out, pos = [], 0
            for t, n in tables + [(va + len(code), 0)]:
                out += list(md.disasm(code[pos:max(pos, t - va)], va + pos))
                pos = t - va + n
            return out
        Li = decode(ours)
        Ri = decode(theirs)
        print("--- %s  ours %d bytes / target %d bytes, %d differing" % (name, len(ours), size, diff))
        for k in range(max(len(Li), len(Ri))):
            a = Li[k] if k < len(Li) else None
            b = Ri[k] if k < len(Ri) else None
            left = "%-8x %-18s %s %s" % (a.address, a.bytes.hex(), a.mnemonic, a.op_str) if a else ""
            right = "%-8x %-18s %s %s" % (b.address, b.bytes.hex(), b.mnemonic, b.op_str) if b else ""
            # differs = unmasked bytes differ (relocated fields match by construction)
            same = a is not None and b is not None and ((a.size == b.size and all(
                (a.address - va + j) in mask or (b.address - va + j) in mask or
                a.bytes[j] == b.bytes[j] for j in range(a.size)))
                or (a.mnemonic == b.mnemonic and a.mnemonic.startswith("j")
                    and a.op_str.startswith("0x")))   # displacement only: a size difference later
            print("%s%-60s | %s" % ("  " if same else "! ", left, right))
    return ok, diff


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("--flags", default=None)
    ap.add_argument("--func", default=None)
    ap.add_argument("-q", action="store_true")
    a = ap.parse_args()
    flags = a.flags.split() if a.flags is not None else default_flags()
    syms = symbol_map()
    tgt = Target()
    with tempfile.TemporaryDirectory() as td:
        objp = os.path.join(td, "m.obj")
        compile_c(a.src, flags, objp)
        obj = OMF(objp)
    allok = True
    for n, _si, _off, _sz in obj.functions():
        name = n.strip("_")
        if a.func and name != a.func:
            continue
        if name not in syms:
            print("?? %s: not a known function name" % name)
            continue
        va, size = syms[name]
        ok, _diff = compare(tgt, obj, name, va, size, a.q)
        print("%s %s" % ("OK  " if ok else "FAIL", name))
        allok &= ok
    sys.exit(0 if allok else 1)


if __name__ == "__main__":
    main()
