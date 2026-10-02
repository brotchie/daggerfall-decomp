#!/usr/bin/env python3
"""Compile C files with the real Watcom C32 10.0a compiler (under DOSBox-X) and compare the
functions against FALL.EXE.

The compiler comes from your own Watcom C/C++ 10.0a media: extract WATCOM/BIN, WATCOM/BINB
and WATCOM/H into third_party/watcom10/w10a/WATCOM (they are never committed). BINB/WCC386.EXE
is a Win32-format binary that runs on DOS through BIN/W32RUN.EXE and DOS4GW.EXE.

usage: wcc10.py file.c [file.c ...] [--flags "..."] [--func name] [-q]
       (each file's functions are compared when its name is func_XXXXXXXX.c, or --func)

All files go through one DOSBox-X session (8.3 names N0000.C, N0001.C, ...).
"""
import argparse
import os
import shutil
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import match  # noqa: E402
from omf import OMF  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
W10 = os.path.join(ROOT, "third_party", "watcom10", "w10a")
# FALL.EXE was built with debug info (-d2): it keeps the dead `mov eax,[i]` of `i++` and
# orders the stack frame its own way, so src/w10/ adds it to config/cflags.txt.
FLAGS = ["-d2"]


def available():
    """True when the compiler has been extracted and DOSBox-X is on PATH."""
    return os.path.exists(os.path.join(W10, "WATCOM", "BINB", "WCC386.EXE")) and \
        shutil.which("dosbox-x") is not None


def compile_many(srcs, flags, workdir=None):
    """Compile `srcs` with Watcom 10.0a; returns {src: obj path or None} (and the log)."""
    td = workdir or tempfile.mkdtemp(prefix="wcc10_")
    flags = list(flags) + os.environ.get("DAGGER_W10EXTRA", "").split()
    lines = ["@echo off", "set WATCOM=D:\\WATCOM", "set INCLUDE=D:\\WATCOM\\H",
             "set PATH=D:\\WATCOM\\BINB;D:\\WATCOM\\BIN;Z:\\", "C:"]
    names = {}
    for k, src in enumerate(srcs):
        n = "N%04d" % k
        names[src] = n
        shutil.copyfile(src, os.path.join(td, n + ".C"))
        lines.append("wcc386 %s %s.C > %s.ERR" % (" ".join(flags), n, n))
    lines.append("echo done > DONE.TXT")
    with open(os.path.join(td, "GO.BAT"), "w", newline="\r\n") as f:
        f.write("\n".join(lines) + "\n")
    env = dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy")
    opts = []
    for kv in os.environ.get("DAGGER_DOSBOX", "cpu:cycles=max").split():
        opts += ["-set", kv.replace(":", " ", 1)]
    subprocess.run(["dosbox-x", "-silent", "-nogui", "-nomenu"] + opts +
                   ["-c", "mount c %s" % td, "-c", "mount d %s" % W10,
                    "-c", "c:\\go.bat", "-exit"],
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, env=env,
                   timeout=60 + 10 * len(srcs))
    out = {}
    for src, n in names.items():
        obj = os.path.join(td, n + ".OBJ")
        out[src] = obj if os.path.exists(obj) else None
    return out, td


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("srcs", nargs="+")
    ap.add_argument("--flags", default=None)
    ap.add_argument("--func", default=None)
    ap.add_argument("-q", action="store_true")
    a = ap.parse_args()
    flags = a.flags.split() if a.flags else match.default_flags()
    objs, td = compile_many(a.srcs, flags)
    tgt = match.Target()
    syms = match.symbol_map()
    nok = 0
    for src, obj in objs.items():
        name = a.func or os.path.basename(src)[:-2]
        if obj is None:
            err = open(os.path.join(td, "N%04d.ERR" % list(objs).index(src)), errors="replace").read()
            print("ERROR", name, err.strip().splitlines()[-1:] if err.strip() else "")
            continue
        if name not in syms:
            print("?", name, obj)
            continue
        va, size = syms[name]
        try:
            ok, diff = match.compare(tgt, OMF(obj), name, va, size, quiet=a.q)
        except KeyError:
            print("MISSING", name)
            continue
        nok += ok
        print("%-4s %s %s" % ("OK" if ok else "FAIL", name, "" if ok else "%d bytes" % diff))
    print("%d / %d match" % (nok, len(objs)))
    if not a.srcs or len(a.srcs) == 1:
        print("work dir:", td)


if __name__ == "__main__":
    main()
