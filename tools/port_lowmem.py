#!/usr/bin/env python3
"""Real-mode memory the game's C reads by address (docs/port.md, phase 5): the BIOS tick count
at 0x46C (`*(int *)1132`, `(int *)0x46c`) and any other low address cast to a pointer.

Under Watcom these are flat addresses that DOS maps; natively nothing is mapped there (arm64
macOS keeps the low 4 GB empty). This tool rewrites each cast of a constant below 0x110000 to a
pointer as `(T *)DOS_LOW(0x46C)`. include/doslow.h defines DOS_LOW as the same constant under
Watcom, so FALL.EXE does not change (tools/build-and-verify.sh checks it), and as the virtual
PC's low memory natively (port/include/port_vpc.h).

  port_lowmem.py [--apply] [FILES]   list the sites (default), or rewrite them
"""
import argparse
import glob
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CAST = re.compile(r"\(\s*((?:(?:unsigned|signed|volatile|const)\s+)*(?:char|short|int|long|void|"
                  r"u8|u16|u32|s8|s16|s32)\s*\*)\s*\)\s*(0[xX][0-9A-Fa-f]+|\d+)\b")
HEADER = '#include "doslow.h"\n'

DOSLOW_H = '''/* doslow.h: real-mode memory the game reads by address: the BIOS data area (the tick count
   at 0x46C) and VGA memory at 0xA0000. Under Watcom, DOS maps them at those flat addresses;
   in the native build (docs/port.md) they are the virtual PC's low memory
   (port/include/port_vpc.h). Written by tools/port_lowmem.py. */
#ifndef DOSLOW_H
#define DOSLOW_H

#ifdef DAGGER_PORT
extern unsigned char *port_low_memory;
#define DOS_LOW(addr) ((void *)(port_low_memory + (addr)))
#else
#define DOS_LOW(addr) ((void *)(addr))
#endif

#endif
'''


def files(args):
    if args:
        return args
    return sorted(glob.glob(os.path.join(ROOT, "src/lifted/*.c")) +
                  glob.glob(os.path.join(ROOT, "src/hand/*.c")) +
                  glob.glob(os.path.join(ROOT, "src/*.c")))


def sites(text):
    for m in CAST.finditer(text):
        v = int(m.group(2), 0)
        if 0x400 <= v < 0x110000:
            yield m, v


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--apply", action="store_true")
    ap.add_argument("files", nargs="*")
    a = ap.parse_args()
    total, changed = 0, 0
    for path in files(a.files):
        text = open(path).read()
        found = list(sites(text))
        if not found:
            continue
        for m, v in found:
            line = text.count("\n", 0, m.start()) + 1
            print("%s:%d: (%s)%s  [0x%X]" % (os.path.relpath(path, ROOT), line, m.group(1),
                                             m.group(2), v))
        total += len(found)
        if a.apply:
            new = CAST.sub(lambda m: "(%s)DOS_LOW(0x%X)" % (m.group(1), int(m.group(2), 0))
                           if 0x400 <= int(m.group(2), 0) < 0x110000 else m.group(0), text)
            if HEADER not in new:
                # after the last #include, else at the top (after the opening comment)
                incs = list(re.finditer(r'^#include .*\n', new, re.M))
                pos = incs[-1].end() if incs else 0
                new = new[:pos] + HEADER + new[pos:]
            open(path, "w").write(new)
            changed += 1
    if a.apply:
        h = os.path.join(ROOT, "include", "doslow.h")
        if not os.path.exists(h):
            open(h, "w").write(DOSLOW_H)
    print("%d sites%s" % (total, (" in %d files rewritten" % changed) if a.apply else ""))


if __name__ == "__main__":
    sys.exit(main())
