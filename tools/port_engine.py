#!/usr/bin/env python3
"""XnGine's 64-bit worklist (docs/port.md): what the host's clang says about src/engine/ for
the native build, by kind and by file. Nothing is compiled to objects.

  port_engine.py [FILES]          the table: each kind of diagnostic, how many, in how many files
  port_engine.py --files [FILES]  per file, the counts (most first)
  port_engine.py --list KIND      every diagnostic of one kind (file:line:col: text)
  port_engine.py --all            every diagnostic (file:line:col: [kind] text)

The flags are the native build's for the engine (PORT_ENGINE: port/include/port.h leaves the
engine's file calls alone) with the warnings that lose half a pointer on. The platform layer
(pc.c, dos.c, timer.c ...) is listed with the rest; --skip-platform leaves it out.
"""
import argparse
import collections
import glob
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FLAGS = ["-fsyntax-only", "-std=gnu89", "-include", "port/include/port.h", "-DPORT_ENGINE",
         "-funsigned-char", "-fwrapv", "-fno-strict-aliasing", "-Iport/include", "-Iinclude",
         "-Isrc/engine", "-Wno-unknown-pragmas", "-Wno-unused",
         "-Wpointer-to-int-cast", "-Wint-to-pointer-cast", "-Wshorten-64-to-32",
         "-Wint-conversion", "-Wno-error=int-conversion", "-ferror-limit=0",
         "-fno-caret-diagnostics", "-fno-color-diagnostics"]

PLATFORM = {"pc.c", "dos.c", "timer.c", "kbd.c", "joy.c", "serial.c", "sys.c", "helmet.c",
            "gfx.c", "pal.c", "mouse.c", "vid.c", "input.c"}

DIAG = re.compile(r"^(.*?):(\d+):(\d+): (warning|error): (.*?)(?: \[-W([\w-]+)(?:,[^\]]*)?\])?$")


def engine_files(skip_platform=False):
    fs = sorted(glob.glob("src/engine/*.c"))
    if skip_platform:
        fs = [f for f in fs if os.path.basename(f) not in PLATFORM]
    return fs


def diagnose(path):
    r = subprocess.run(["clang"] + FLAGS + [path], capture_output=True, text=True, cwd=ROOT)
    out = []
    for line in r.stderr.splitlines():
        m = DIAG.match(line)
        if m:
            kind = "error" if m.group(4) == "error" else (m.group(6) or "other")
            out.append((m.group(1), int(m.group(2)), int(m.group(3)), kind, m.group(5)))
    return path, out


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*")
    ap.add_argument("--files", dest="per_file", action="store_true")
    ap.add_argument("--list")
    ap.add_argument("--all", action="store_true")
    ap.add_argument("--skip-platform", action="store_true")
    a = ap.parse_args()
    os.chdir(ROOT)
    files = a.files or engine_files(a.skip_platform)
    with ThreadPoolExecutor(os.cpu_count()) as ex:
        results = list(ex.map(diagnose, files))
    seen = set()
    uniq = []
    for _path, diags in results:
        for d in diags:
            # a header's diagnostic once, not once per file that includes it
            if d[0].endswith(".h"):
                if d in seen:
                    continue
                seen.add(d)
            uniq.append(d)
    if a.list or a.all:
        for f, line, col, kind, text in uniq:
            if a.all or kind == a.list:
                print("%s:%d:%d: [%s] %s" % (f, line, col, kind, text))
        return
    kinds = collections.Counter()
    kind_files = collections.defaultdict(set)
    per_file = collections.defaultdict(collections.Counter)
    for f, line, col, kind, text in uniq:
        kinds[kind] += 1
        kind_files[kind].add(f)
        per_file[f][kind] += 1
    if a.per_file:
        for f, c in sorted(per_file.items(), key=lambda kv: -sum(kv[1].values())):
            print("%5d %s  %s" % (sum(c.values()), f,
                                  ", ".join("%s %d" % (k, n) for k, n in c.most_common())))
        return
    print("%-30s %6s %6s" % ("kind", "count", "files"))
    for k, n in kinds.most_common():
        print("%-30s %6d %6d" % (k, n, len(kind_files[k])))
    print("%-30s %6d" % ("total", sum(kinds.values())))


if __name__ == "__main__":
    sys.exit(main())
