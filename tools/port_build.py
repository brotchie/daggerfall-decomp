#!/usr/bin/env python3
"""The native build (docs/port.md): the game's C compiled by the host's clang with SDL3.

  port_build.py [build]   configure (CMake, Ninja) and build build/port/fall
  port_build.py run ...   prepare a game folder as tools/fallemu.py does, then run the build
                          (arguments after `run` go to fall)
  port_build.py missing   list what the build still lacks, by kind (the stubs it generated)

  PORT_BUILD=DIR sets the build folder (build/port by default); PORT_BUILD_TYPE=Release (or
  RelWithDebInfo) configures a new folder optimised (Debug by default).

How the build fills its gaps: the game's objects name ~4,000 functions and globals. Those the
port defines (src/, port/shim, port/host) link as they are, and so do the C library functions
the game means to take from the host (HOST_LIBC). Everything else gets a generated definition
in build/port/gen/ (never committed), so the build always links:
  - a function: a stub that reports its name and stops (port_unimplemented);
  - a global: FALL.EXE's data, laid out by tools/port_data.py: all of object 3 in address
    order, typed globals in their native layouts with 8-byte pointers from FALL.EXE's fixups
    (its problems, declarations still too narrow for 64 bits, are in gen/data_problems.txt).
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
# PORT_BUILD=DIR: another build folder, so two people (or agents) can build at once
BUILD = os.path.abspath(os.environ.get("PORT_BUILD") or os.path.join(ROOT, "build", "port"))
GEN = os.path.join(BUILD, "gen")
EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")

# the C library functions the game calls that the host's do as Watcom's did (port.h sends the
# others to port/shim); anything else the game names must be defined by the port
HOST_LIBC = {
    "abs", "atoi", "exp", "memchr", "memcmp", "printf", "strchr", "strcmp", "strlen",
    "strncmp", "strstr", "tolower", "toupper",
    # and XnGine's (clang's own calls for struct copies and zeroing, too, and what its
    # optimiser makes of loops on Darwin)
    "memcpy", "memset", "memmove", "bzero", "memset_pattern4", "memset_pattern8",
    "memset_pattern16",
}


def run(cmd, **kw):
    return subprocess.run(cmd, check=False, text=True, **kw)


def configure():
    if not os.path.exists(os.path.join(BUILD, "build.ninja")):
        r = run(["cmake", "-S", PORT, "-B", BUILD, "-G", "Ninja",
                 "-DCMAKE_BUILD_TYPE=" + os.environ.get("PORT_BUILD_TYPE", "Debug"),
                 "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"])
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
    """name -> (address, kind) for every name the sources may use: config/symbols.txt's names
    (the game's), then XnGine's as its harness resolves them (tools/xn_rc.py: config/names.csv
    at any confidence, and config/xngine_aliases.csv's second names for an address), and the
    address names (func_X, D_X, xn_data_X)"""
    table = {}
    for line in open(os.path.join(ROOT, "config", "symbols.txt")):
        m = re.match(r"(\w+) = (0x[0-9A-Fa-f]+); (\w+)", line)
        if m:
            table[m.group(1)] = (int(m.group(2), 16), m.group(3))
    for p in ("names.csv", "xngine_aliases.csv"):
        with open(os.path.join(ROOT, "config", p), newline="") as f:
            for r in csv.DictReader(f):
                if r["kind"] in ("func", "global") and r["name"] not in table:
                    table[r["name"]] = (int(r["address"], 16), r["kind"])
    return table


def address_of(name, table):
    if name in table:
        return table[name]
    m = re.fullmatch(r"(func|D|xn_data|asm)_([0-9A-Fa-f]{6,8})", name)
    if m:
        kind = "func" if m.group(1) in ("func", "asm") else "global"
        return int(m.group(2), 16), kind
    return None, None


def generate(need, table, defined):
    """write gen/stubs.c and gen/data.c for the names in need (defined: what the game's and the
    port's objects define); returns (functions, globals, unknown, data plan)"""
    import port_data
    image = le.LE(EXE)
    xn_funcs = set()
    for r in csv.DictReader(open(os.path.join(ROOT, "config", "xngine_functions.csv"))):
        xn_funcs.add(int(r["va"], 16))

    declared = port_data.declarations()     # extern data declarations: data, whatever names.csv says
    funcs, data, unknown = [], {}, []
    for name in sorted(need):
        addr, kind = address_of(name, table)
        if addr is None:
            unknown.append(name)
            continue
        o = image.obj_of_va(addr)
        # a name the sources declare as data is data: names.csv calls some "func" (the
        # helmet's command strings, which xngine_functions.csv takes for code)
        if name not in declared and ((o is not None and o.index == 2 and addr in xn_funcs) or
                                     (kind == "func" and not (o is not None and o.index == 2))):
            funcs.append((name, addr))
        elif o is None:
            unknown.append(name)
        else:
            data[name] = addr

    # the data: all of object 3 and the object-2 globals the game names, with real pointers
    # (tools/port_data.py); the functions it points at need definitions too
    text, plan = port_data.plan_data(data, table, image=image, defined=defined)
    have = defined | {n for n, _a in funcs}
    for name in sorted(plan.functions - have):
        addr, _kind = address_of(name, table)
        funcs.append((name, addr or 0))

    os.makedirs(GEN, exist_ok=True)
    with open(os.path.join(GEN, "stubs.c"), "w") as f:
        f.write("/* generated by tools/port_build.py: functions the native build lacks */\n")
        f.write('#include "port_host.h"\n\n')
        for name, addr in funcs:
            f.write('void %s(void) { port_unimplemented("%s (0x%08X)"); }\n' % (name, name, addr))
        for name in unknown:
            f.write('void %s(void) { port_unimplemented("%s"); }\n' % (name, name))
    with open(os.path.join(GEN, "data.c"), "w") as f:
        f.write(text)
    with open(os.path.join(GEN, "data_problems.txt"), "w") as f:
        for kind, rows in sorted(plan.problems.items()):
            f.write("%s: %d\n" % (kind, len(rows)))
            for r in rows:
                f.write("  %s\n" % r)
    return funcs, data, unknown, plan


def build():
    configure()
    # compile everything; the link may fail until the generated definitions are current
    r = run(["ninja", "-C", BUILD, "fall"], capture_output=True)
    game_objs = objects("game.dir") + objects("engine.dir")
    port_objs = [o for o in objects("fall.dir") + objects("porthost.dir") if "/gen/" not in o]
    errors = [l for l in (r.stdout + r.stderr).splitlines() if re.search(r"\.[ch]:\d+:\d+: error:", l)]
    if errors or not game_objs:
        print("\n".join(errors[:50]) or r.stdout[-4000:])
        sys.exit("port: compile errors")
    defined, _ = nm_symbols(game_objs + port_objs)
    _, undefined = nm_symbols(game_objs)
    # names with two underscores are the compiler's and the host's (__stack_chk_guard ...)
    need = {n for n in undefined - defined - HOST_LIBC if not n.startswith("__")}
    funcs, data, unknown, plan = generate(need, symbol_table(), defined)
    typed = sum(1 for _a, _e, _n, t in plan.items if t)
    print("port: %d game objects; generated %d function stubs, %d unknown; data: %d globals "
          "(%d with native layouts), problems: %s (gen/data_problems.txt)"
          % (len(game_objs), len(funcs), len(unknown), len(plan.items), typed,
             ", ".join("%s %d" % (k, len(v)) for k, v in sorted(plan.problems.items()))))
    r = run(["ninja", "-C", BUILD, "fall", "vpcdemo", "opltest", "musictest"])
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
