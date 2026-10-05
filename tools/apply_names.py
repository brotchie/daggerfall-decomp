#!/usr/bin/env python3
"""Put the names from config/names.csv into the C sources, or take them back out.

The sources name functions and globals by address (func_0001025B, D_00195BE0) until a name is
certain enough: `apply` rewrites those identifiers in src/ and include/ to the confirmed and
strong names (candidates stay addresses), and records every name it applied in
config/symbols.txt (`name = 0xADDR;`), which the build reads to check a named function at its
address and to resolve a named global. The matching build is the test: names change no code.

A name is left out (and reported) when it would clash in C: a keyword, a name the sources
already use for something else (a local, a member, a macro, a parameter: renaming the global
would change what that code means), `main`, or a library function whose name is internal to
the compiler (a leading `_` or a trailing `_`: Watcom's name decoration would mangle them).

Re-running is safe: a name that changed in names.csv is renamed again, and one that dropped
below strong goes back to its address. Tools that read the sources by address use
names.canonical() to see them that way.

usage: apply_names.py [apply | revert | report]
"""
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import names as namesmod  # noqa: E402

ROOT = namesmod.ROOT
SYMBOLS = os.path.join(ROOT, "config", "symbols.txt")
KEYWORDS = set("""auto break case char const continue default do double else enum extern float for
goto if int long register return short signed sizeof static struct switch typedef union unsigned
void volatile while main far near huge cdecl pascal fortran interrupt _Packed""".split())
IDENT = re.compile(r"\b[A-Za-z_][A-Za-z0-9_]*\b")
ADDRESS = re.compile(r"^(func|D)_([0-9A-F]{8})$")


def sources():
    out = []
    for d, _sub, files in os.walk(os.path.join(ROOT, "src")):
        out += [os.path.join(d, f) for f in files if f.endswith(".c")]
    inc = os.path.join(ROOT, "include")
    out += [os.path.join(inc, f) for f in os.listdir(inc) if f.endswith(".h")]
    return sorted(out)


def wanted(rows, used):
    """{"func_XXXXXXXX"/"D_XXXXXXXX": name} to apply, and the names left out, with why."""
    out, skipped = {}, []
    region_lib = (0x9DA1C, 0xBB27F)
    for r in rows:
        if r["kind"] not in ("func", "global") or r["confidence"] not in ("confirmed", "strong"):
            continue
        a = int(r["address"], 16)
        ident = ("func_%08X" if r["kind"] == "func" else "D_%08X") % a
        n = r["name"]
        if r["kind"] == "func" and region_lib[0] <= a < region_lib[1] and \
                not re.fullmatch(r"[a-z][a-z0-9_]*[a-z0-9]", n):
            skipped.append((n, "a library function with a compiler-internal name"))
        elif n in KEYWORDS:
            skipped.append((n, "a C keyword or main"))
        elif n in used:
            skipped.append((n, "already an identifier in the sources"))
        else:
            out[ident] = n
    return out, skipped


def read_symbols():
    syms = {}
    if os.path.exists(SYMBOLS):
        for line in open(SYMBOLS):
            m = re.match(r"\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+)\s*;\s*(func|global)?", line)
            if m:
                syms[m.group(1)] = (int(m.group(2), 16), m.group(3) or "func")
    return syms


def write_symbols(applied):
    with open(SYMBOLS, "w") as f:
        f.write("# Names applied to the sources by tools/apply_names.py (from config/names.csv).\n"
                "# The build resolves these to their addresses; edit names.csv, not this file.\n")
        for ident, n in sorted(applied.items(), key=lambda kv: kv[0]):
            m = ADDRESS.match(ident)
            f.write("%s = 0x%s; %s\n" % (n, m.group(2), "func" if m.group(1) == "func" else "global"))


def rewrite(mapping, files):
    changed = 0
    for p in files:
        t = open(p, encoding="latin-1").read()
        u = IDENT.sub(lambda m: mapping.get(m.group(0), m.group(0)), t)
        if u != t:
            open(p, "w", encoding="latin-1").write(u)
            changed += 1
    return changed


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else "apply"
    files = sources()
    old = read_symbols()                            # name -> (address, kind), applied before
    back = {n: ("func_%08X" if kind == "func" else "D_%08X") % a for n, (a, kind) in old.items()}
    if cmd == "revert":
        n = rewrite(back, files)
        if os.path.exists(SYMBOLS):
            os.remove(SYMBOLS)
        print("%d files back to addresses" % n)
        return
    # identifiers the sources use for anything else (address tokens and applied names aside)
    used = set()
    for p in files:
        for tok in IDENT.findall(open(p, encoding="latin-1").read()):
            if not ADDRESS.match(tok) and tok not in old:
                used.add(tok)
    want, skipped = wanted(namesmod.load(), used)
    if cmd == "report":
        print("%d names to apply; %d left out" % (len(want), len(skipped)))
        for n, why in skipped:
            print("  %-36s %s" % (n, why))
        return
    mapping = dict(back)                            # first undo, then apply what is wanted now
    for n, ident in back.items():
        if ident in want:
            mapping[n] = want[ident]                # renamed (or the same)
    for ident, n in want.items():
        mapping[ident] = n
    changed = rewrite(mapping, files)
    write_symbols(want)
    print("applied %d names (%d left out) in %d files; config/symbols.txt" % (len(want), len(skipped), changed))


if __name__ == "__main__":
    main()
