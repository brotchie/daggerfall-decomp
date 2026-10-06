#!/usr/bin/env python3
"""The native build's worklist (docs/port.md): what the host's clang says about the game's C
for a 64-bit target, counted by kind and by file. Nothing is compiled to objects.

  port_census.py [FILES]          the table: each kind of diagnostic, how many, in how many files
  port_census.py --files [FILES]  per file, the 64-bit counts (most first)
  port_census.py --json OUT       also write {file: {kind: n}} to OUT
  port_census.py --list KIND      every diagnostic of one kind (file:line:col: text)

The kinds that break a 64-bit build are marked *: a pointer that passes through a 32-bit
integer (casts, declarations without parameters, int <-> pointer conversions) loses its top
half. The flags are tools/port_build.py's (port/CMakeLists.txt), with the warnings on.
"""
import argparse
import collections
import glob
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
FLAGS = ["-fsyntax-only", "-std=gnu89", "-include", "port/include/port.h",
         "-funsigned-char", "-fwrapv", "-fno-strict-aliasing", "-Iport/include", "-Iinclude",
         "-Wall", "-Wno-unused", "-Wno-parentheses", "-Wno-dangling-else", "-Wno-missing-braces",
         "-Wno-implicit-int", "-Wno-unknown-pragmas", "-Wpointer-to-int-cast",
         "-Wint-to-pointer-cast", "-Wshorten-64-to-32", "-Wint-conversion",
         "-Wno-error=int-conversion", "-Wno-error=incompatible-function-pointer-types",
         "-Wno-error=implicit-function-declaration", "-ferror-limit=0",
         "-fno-caret-diagnostics", "-fno-color-diagnostics"]

# the kinds that lose half a pointer on a 64-bit host
BREAKS = {"pointer-to-int-cast", "int-to-pointer-cast", "void-pointer-to-int-cast",
          "int-to-void-pointer-cast", "shorten-64-to-32", "int-conversion",
          "deprecated-non-prototype", "pointer-integer-compare", "error"}

DIAG = re.compile(r"^(.*?):(\d+):(\d+): (warning|error): (.*?)(?: \[-W([\w-]+)(?:,[^\]]*)?\])?$")


def game_files():
    return sorted(glob.glob("src/lifted/*.c") + glob.glob("src/hand/*.c") + glob.glob("src/*.c"))


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
    ap.add_argument("--json")
    ap.add_argument("--list")
    a = ap.parse_args()
    os.chdir(ROOT)
    files = a.files or game_files()
    with ThreadPoolExecutor(os.cpu_count()) as ex:
        results = list(ex.map(diagnose, files))

    if a.list:
        for _path, diags in results:
            for f, line, col, kind, text in diags:
                if kind == a.list:
                    print("%s:%d:%d: %s" % (f, line, col, text))
        return

    kinds = collections.Counter()
    kind_files = collections.defaultdict(set)
    per_file = {}
    for path, diags in results:
        c = collections.Counter()
        for f, _line, _col, kind, _text in diags:
            kinds[kind] += 1
            kind_files[kind].add(f)
            c[kind] += 1
        per_file[path] = dict(c)
    if a.json:
        with open(a.json, "w") as f:
            json.dump(per_file, f, indent=1, sort_keys=True)
    if a.per_file:
        rows = sorted(((sum(n for k, n in c.items() if k in BREAKS), p) for p, c in per_file.items()),
                      reverse=True)
        for n, p in rows:
            if n:
                print("%6d  %s" % (n, p))
        return
    total = sum(n for k, n in kinds.items() if k in BREAKS)
    print("%d files; %d diagnostics that break a 64-bit build (*)" % (len(files), total))
    for kind, n in kinds.most_common():
        print("%s %-40s %6d in %4d files" % ("*" if kind in BREAKS else " ", kind, n,
                                             len(kind_files[kind])))


if __name__ == "__main__":
    sys.exit(main())
