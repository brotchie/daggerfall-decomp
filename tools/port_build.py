#!/usr/bin/env python3
"""The native build (docs/port.md): the game's C compiled by the host's clang with SDL3.

  port_build.py [build]   configure (CMake, Ninja) and build build/port/fall
  port_build.py run ...   prepare a game folder as tools/fallemu.py does, then run the build
                          (arguments after `run` go to fall)
  port_build.py missing   list what the build still lacks, by kind (the stubs it generated)

How the build fills its gaps: the game's objects name ~4,000 functions and globals. Those the
port defines (src/, port/shim, port/host) link as they are, and so do the C library functions
the game means to take from the host (HOST_LIBC). Everything else gets a generated definition
in build/port/gen/ (never committed), so the build always links:
  - a function: a stub that reports its name and stops (port_unimplemented);
  - a global: its bytes from FALL.EXE, from its address up to the next known symbol, as a
    byte array. Pointers in them are still FALL.EXE's 32-bit addresses: the typed data
    definitions are phase 2 of docs/port.md.
A game global that shares a name with something in the host's C library (index, time ...)
gets its own definition too, which the linker prefers to the library's.
"""
import argparse
import csv
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, os.path.join(ROOT, "tools"))
import le  # noqa: E402

PORT = os.path.join(ROOT, "port")
BUILD = os.path.join(ROOT, "build", "port")
GEN = os.path.join(BUILD, "gen")
EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")

# the C library functions the game calls that the host's do as Watcom's did (port.h sends the
# others to port/shim); anything else the game names must be defined by the port
HOST_LIBC = {
    "abs", "atoi", "exp", "memchr", "memcmp", "printf", "strchr", "strcmp", "strlen",
    "strncmp", "strstr", "tolower", "toupper",
}


def run(cmd, **kw):
    return subprocess.run(cmd, check=False, text=True, **kw)


def configure():
    if not os.path.exists(os.path.join(BUILD, "build.ninja")):
        r = run(["cmake", "-S", PORT, "-B", BUILD, "-G", "Ninja",
                 "-DCMAKE_BUILD_TYPE=Debug", "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"])
        if r.returncode:
            sys.exit("cmake configure failed")


def objects(target_dir):
    out = []
    for d, _, files in os.walk(os.path.join(BUILD, "CMakeFiles", target_dir)):
        out += [os.path.join(d, f) for f in files if f.endswith(".o")]
    return out


def nm_symbols(objs):
    """(defined, undefined) external symbol names over the objects, without the leading _"""
    defined, undefined = set(), set()
    for i in range(0, len(objs), 200):
        r = run(["nm", "-g"] + objs[i:i + 200], capture_output=True)
        for line in r.stdout.splitlines():
            parts = line.split()
            if len(parts) >= 2 and parts[-1].startswith("_"):
                name = parts[-1][1:]
                if parts[-2] == "U":
                    undefined.add(name)
                elif parts[-2] in "TDBSC":
                    defined.add(name)
    return defined, undefined


def symbol_table():
    """name -> (address, kind) for every name the sources may use: config/symbols.txt's names,
    and the address names (func_X, D_X, xn_data_X)"""
    table = {}
    for line in open(os.path.join(ROOT, "config", "symbols.txt")):
        m = re.match(r"(\w+) = (0x[0-9A-Fa-f]+); (\w+)", line)
        if m:
            table[m.group(1)] = (int(m.group(2), 16), m.group(3))
    return table


def address_of(name, table):
    if name in table:
        return table[name]
    m = re.fullmatch(r"(func|D|xn_data|asm)_([0-9A-Fa-f]{6,8})", name)
    if m:
        kind = "func" if m.group(1) in ("func", "asm") else "global"
        return int(m.group(2), 16), kind
    return None, None


def generate(need, table):
    """write gen/stubs.c and gen/data.c for the names in need; returns the kinds for a report"""
    image = le.LE(EXE)
    objs = image.load(relocate=True)
    xn_funcs = set()
    for r in csv.DictReader(open(os.path.join(ROOT, "config", "xngine_functions.csv"))):
        xn_funcs.add(int(r["va"], 16))

    # every known address, to bound each global's extent
    starts = sorted({a for a, _k in table.values()} |
                    {address_of(n, table)[0] for n in need if address_of(n, table)[0]})
    import bisect

    funcs, data, unknown = [], [], []
    for name in sorted(need):
        addr, kind = address_of(name, table)
        if addr is None:
            unknown.append(name)
            continue
        o = image.obj_of_va(addr)
        if kind == "func" or (o is not None and o.index == 2 and addr in xn_funcs):
            funcs.append((name, addr))
            continue
        if o is None:
            unknown.append(name)
            continue
        i = bisect.bisect_right(starts, addr)
        end = starts[i] if i < len(starts) else o.base + o.vsize
        end = min(end, o.base + o.vsize)
        data.append((name, addr, bytes(objs[o.index][addr - o.base:end - o.base])))

    os.makedirs(GEN, exist_ok=True)
    with open(os.path.join(GEN, "stubs.c"), "w") as f:
        f.write("/* generated by tools/port_build.py: functions the native build lacks */\n")
        f.write('#include "port_host.h"\n\n')
        for name, addr in funcs:
            f.write('void %s(void) { port_unimplemented("%s (0x%08X)"); }\n' % (name, name, addr))
        for name in unknown:
            f.write('void %s(void) { port_unimplemented("%s"); }\n' % (name, name))
    with open(os.path.join(GEN, "data.c"), "w") as f:
        f.write("/* generated by tools/port_build.py: the game's globals as FALL.EXE's bytes, each\n"
                "   up to the next known symbol. Pointers inside are still 32-bit FALL.EXE\n"
                "   addresses (docs/port.md, phase 2). */\n\n")
        for name, addr, b in data:
            size = max(8, (len(b) + 7) & ~7)
            if any(b):
                body = ",".join(str(x) for x in b)
                f.write("unsigned char %s[%d] __attribute__((aligned(16))) = {%s}; /* 0x%08X */\n"
                        % (name, size, body, addr))
            else:
                f.write("unsigned char %s[%d] __attribute__((aligned(16))); /* 0x%08X */\n"
                        % (name, size, addr))
    return funcs, data, unknown


def build():
    configure()
    # compile everything; the link may fail until the generated definitions are current
    r = run(["ninja", "-C", BUILD, "fall"], capture_output=True)
    game_objs = objects("game.dir")
    port_objs = [o for o in objects("fall.dir") if "/gen/" not in o]
    errors = [l for l in (r.stdout + r.stderr).splitlines() if re.search(r"\.[ch]:\d+:\d+: error:", l)]
    if errors or not game_objs:
        print("\n".join(errors[:50]) or r.stdout[-4000:])
        sys.exit("port: compile errors")
    defined, _ = nm_symbols(game_objs + port_objs)
    _, undefined = nm_symbols(game_objs)
    # names with two underscores are the compiler's and the host's (__stack_chk_guard ...)
    need = {n for n in undefined - defined - HOST_LIBC if not n.startswith("__")}
    funcs, data, unknown = generate(need, symbol_table())
    print("port: %d game objects; generated %d function stubs, %d globals, %d unknown"
          % (len(game_objs), len(funcs), len(data), len(unknown)))
    r = run(["ninja", "-C", BUILD, "fall"])
    if r.returncode:
        sys.exit("port: link failed")
    print("port: built", os.path.relpath(os.path.join(BUILD, "fall"), ROOT))


def missing():
    stubs = os.path.join(GEN, "stubs.c")
    if not os.path.exists(stubs):
        sys.exit("build first")
    table = symbol_table()
    regions = [("game", 0x10010, 0x9DA1C), ("library", 0x9DA1C, 0xBB27F),
               ("xngine", 0xC0000, 0x161568)]
    by = {}
    for line in open(stubs):
        m = re.match(r"void (\w+)\(void\)", line)
        if not m:
            continue
        addr, _ = address_of(m.group(1), table)
        region = next((r for r, a, b in regions if addr is not None and a <= addr < b), "?")
        by.setdefault(region, []).append(m.group(1))
    for region, names in sorted(by.items()):
        print("%-8s %4d  %s" % (region, len(names), " ".join(names)))


def run_game(args):
    import fallemu
    game = os.environ.get("DAGGER_GAME") or os.path.join(ROOT, "build", "game")
    overlay = os.environ.get("DAGGER_OVERLAY") or os.path.join(BUILD, "run")
    fallemu.prepare_overlay(game, overlay)
    os.execv(os.path.join(BUILD, "fall"),
             [os.path.join(BUILD, "fall"), "--game", game, "--overlay", overlay] + args)


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("cmd", nargs="?", default="build", choices=["build", "run", "missing"])
    ap.add_argument("rest", nargs=argparse.REMAINDER)
    a = ap.parse_args()
    if a.cmd == "build":
        build()
    elif a.cmd == "missing":
        missing()
    else:
        run_game(a.rest)


if __name__ == "__main__":
    main()
