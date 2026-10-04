#!/usr/bin/env python3
"""Static evidence from the C and FALL.EXE's data, no emulator: the TEXT.RSC records and
sounds each function asks for by a constant id, and the button and text-macro tables.

Text and sound APIs are found by following the id outward from the readers: text.c
func_0003D412's first argument is a TEXT.RSC record id, objlib.c func_00085A51's a DAGGER.SND
record id, and a function that passes one of its own parameters there (as is, or plus a
constant) is an API too: func_0003E8A8, func_0003F09F, func_0007DDC9, parse.c func_0004A6B5;
sound.c func_00069938, func_0006998C, func_00069A62, ... (the quest-file readers that point
func_0003D412 at a .QRC are left out). Every call to an API with a constant id, or with
`expression + constant` (a base of 100 or more, written 500+), gets the id named: TEXT.RSC's text, the
sound's name in Daggerfall Unity (config/sound_clips.csv).

Button tables are arrays of {short x0, y0, x1, y1; void (*handler)()} in object 3: runs of
function pointers (LE fixups) 12 bytes apart behind plausible screen boxes, split where the
code reads an entry as a whole (a table's start). Text macros (%pcf, %dwr, ...) are
{char name[5]; handler} entries 9 bytes apart.

usage:
  asset_ids.py         build/assets/ids.csv (function, api, asset, id, meaning),
                       buttons.csv (table, name, index, box, handler), macros.csv
  asset_ids.py apis    the APIs found: which argument carries the id, and its offset
"""
import collections
import csv
import glob
import os
import re
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import assets  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "build", "assets")
EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")
READERS = {"TEXT.RSC": 0x3D412, "DAGGER.SND": 0x85A51}
QRC = {0x3E6B1, 0x3E7D7, 0x3EAB4}       # func_0003D412 on a quest's .QRC: not TEXT.RSC ids
BASE_MIN = 100                          # snd + 1 adjusts an id held in a variable: not a base

DEF = re.compile(r"^(?!extern\b|typedef\b)[A-Za-z_][\w \t\*]*?\bfunc_([0-9A-F]{8})\s*\(", re.M)
CALL = re.compile(r"\bfunc_([0-9A-F]{8})\s*\(")
TYPE = re.compile(r"\b(char|short|int|long|signed|unsigned|struct|void|float|double)\b|\*\s*$")
LIT = re.compile(r"-?(0[xX][0-9a-fA-F]+|\d+)[uUlL]*")


def _close(s, i):
    """The index of the parenthesis closing the one at s[i]."""
    d = 0
    for j in range(i, len(s)):
        if s[j] == "(":
            d += 1
        elif s[j] == ")":
            d -= 1
            if d == 0:
                return j
    return len(s) - 1


def _split(s):
    """s split at its top-level commas."""
    out, d, cur = [], 0, []
    for c in s:
        if c in "([":
            d += 1
        elif c in ")]":
            d -= 1
        if c == "," and d == 0:
            out.append("".join(cur))
            cur = []
        else:
            cur.append(c)
    if "".join(cur).strip():
        out.append("".join(cur))
    return out


def _clean(text):
    """C text without comments, and with string and char literals emptied."""
    text = re.sub(r"/\*.*?\*/", " ", text, flags=re.S)
    text = re.sub(r"//[^\n]*", " ", text)
    text = re.sub(r'"(?:\\.|[^"\\\n])*"', '""', text)
    return re.sub(r"'(?:\\.|[^'\\\n])+'", "0", text)


def functions():
    """{va: (parameter names, [(callee va, [argument text])])} for every function in src/."""
    out = {}
    for p in sorted(glob.glob(os.path.join(ROOT, "src", "**", "*.c"), recursive=True)):
        text = _clean(open(p, errors="replace").read())
        defs = []
        for m in DEF.finditer(text):
            o = m.end() - 1
            c = _close(text, o)
            after = text[c + 1:c + 200].lstrip()
            if after.startswith("{"):
                defs.append((int(m.group(1), 16), text[o + 1:c], c + 1))
        for k, (va, plist, start) in enumerate(defs):
            end = defs[k + 1][2] if k + 1 < len(defs) else len(text)
            body = "\n".join(line for line in text[start:end].split("\n")
                             if not line.lstrip().startswith(("extern", "#")))
            params = []
            for piece in _split(plist):
                fp = re.search(r"\(\s*\*\s*(\w+)\s*\)", piece)
                ids = re.findall(r"[A-Za-z_]\w*", re.sub(r"\[.*?\]", "", piece))
                params.append(fp.group(1) if fp else ids[-1] if ids and ids[-1] != "void" else None)
            params = [x for x in params if x]
            calls = []
            for c in CALL.finditer(body):
                o = c.end() - 1
                calls.append((int(c.group(1), 16), _split(body[o + 1:_close(body, o)])))
            out[va] = (params, calls)
    return out


def _strip(e):
    """An expression without its outer parentheses, casts and *(T *)& reinterpretations."""
    e = e.strip()
    while e:
        if e[0] == "(" and _close(e, 0) == len(e) - 1:
            e = e[1:-1].strip()
            continue
        if e[0] == "(":
            c = _close(e, 0)
            if c < len(e) - 1 and TYPE.search(e[1:c]):
                e = e[c + 1:].strip()            # a cast
                continue
        m = re.fullmatch(r"\*\s*\([\w\s]+\*\s*\)\s*&\s*(\w+)", e)
        if m:
            return m.group(1)
        return e
    return e


def value(e, params):
    """What an argument is: [("const", v)], [("param", index, offset)], [("base", v)] for
    expression + constant; both branches of a ?:; [] for anything else."""
    e = _strip(e)
    d, q = 0, None
    for i, c in enumerate(e):
        d += c == "("
        d -= c == ")"
        if c == "?" and d == 0 and q is None:
            q = i
        elif c == ":" and d == 0 and q is not None:
            return value(e[q + 1:i], params) + value(e[i + 1:], params)
    if LIT.fullmatch(e):
        return [("const", int(LIT.fullmatch(e).group(0).rstrip("uUlL"), 0))]
    if e in params:
        return [("param", params.index(e), 0)]
    d = 0
    for i in range(len(e) - 1, 0, -1):          # the last top-level binary + or -
        c = e[i]
        d += c == ")"
        d -= c == "("
        if d == 0 and c in "+-" and e[i - 1] not in "+-*/(<>=&|^!?:" and e[i + 1:i + 2] not in "+-=":
            right = _strip(e[i + 1:])
            if LIT.fullmatch(right):
                off = int(right.rstrip("uUlL"), 0) * (1 if c == "+" else -1)
                left = value(e[:i], params)
                if len(left) == 1 and left[0][0] == "param":
                    return [("param", left[0][1], left[0][2] + off)]
                if len(left) == 1:
                    return [(left[0][0], left[0][-1] + off)]
                return [("base", off)] if off >= BASE_MIN else []
            break
    return []


def apis(fns=None):
    """{asset: {va: {(argument index, offset)}}}: the functions whose argument (plus offset)
    is a record id of the asset."""
    fns = fns or functions()
    out = {a: {va: {(0, 0)}} for a, va in READERS.items()}
    changed = True
    while changed:
        changed = False
        for va, (params, calls) in fns.items():
            if va in QRC:
                continue
            for callee, args in calls:
                for a in out.values():
                    for p, off in list(a.get(callee, ())):
                        for v in (value(args[p], params) if p < len(args) else []):
                            if v[0] == "param" and (v[1], off + v[2]) not in a.get(va, ()):
                                a.setdefault(va, set()).add((v[1], off + v[2]))
                                changed = True
    return out


def meaning(asset, rid, base=False):
    """TEXT.RSC's text or the sound's name; for a base, the first record from rid on that has
    one ("from N: ...")."""
    for k in range(64 if base else 1):
        m = assets.rsc_text(rid + k, 80) if asset == "TEXT.RSC" else assets.sound_name(rid + k)
        if m:
            return ("from %d: " % (rid + k) if base else "") + m
    return "(no such %s record)" % asset


def ids(fns=None):
    """[(function va, api va, asset, id, meaning)], one per distinct id a call passes; a base
    (expression + constant) has id 'N+'."""
    fns = fns or functions()
    api = apis(fns)
    rows = set()
    for va, (params, calls) in fns.items():
        for callee, args in calls:
            for asset, a in api.items():
                for p, off in a.get(callee, ()):
                    for v in (value(args[p], params) if p < len(args) else []):
                        if v[0] in ("const", "base"):
                            b = v[0] == "base"
                            rows.add((va, callee, asset, "%d%s" % (v[1] + off, "+" * b),
                                      meaning(asset, v[1] + off, b)))
    return sorted(rows, key=lambda r: (r[0], r[2], int(r[3].rstrip("+"))))


# ---- tables in object 3 --------------------------------------------------------------------
def _image():
    from le import LE, SRC_OFF32
    le = LE(EXE)
    img = le.load(relocate=True)
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        starts = {int(r["va"], 16) for r in csv.DictReader(f)}
    o1, o3 = le.objs[0], le.objs[2]
    fx = [f for f in le.fixups() if f.kind == SRC_OFF32]
    ptrs = {f.src_va: f.target_va for f in fx if o3.base <= f.src_va < o3.base + o3.vsize}
    refs = collections.defaultdict(set)         # object 3 address -> how code reads it
    for f in fx:
        if o1.base <= f.src_va < o1.base + o1.vsize and o3.base <= f.target_va < o3.base + o3.vsize:
            refs[f.target_va].add(f.src_va)
    data = img[o3.index]
    return (lambda va, n: bytes(data[va - o3.base: va - o3.base + n])), ptrs, starts, refs


def _box(rd, va):
    return struct.unpack("<hhhh", rd(va, 8))


def _onscreen(b):
    """A box on the 320x200 screen, give or take a few pixels (one runs to y 202)."""
    return 0 <= b[0] <= b[2] < 330 and 0 <= b[1] <= b[3] < 210


def buttons():
    """[(table va, index, (x0, y0, x1, y1), handler va or 0)] for the button tables."""
    rd, ptrs, starts, refs = _image()
    ents = sorted(p - 8 for p, t in ptrs.items() if t in starts and _onscreen(_box(rd, p - 8)))
    runs = []
    for e in ents:
        if runs:
            gap = (e - runs[-1][-1]) // 12
            if (e - runs[-1][-1]) % 12 == 0 and 1 <= gap <= 4 and all(
                    rd(runs[-1][-1] + 12 * k + 8, 4) == b"\0\0\0\0" and
                    runs[-1][-1] + 12 * k + 8 not in ptrs for k in range(1, gap)):
                runs[-1] += [runs[-1][-1] + 12 * k for k in range(1, gap)] + [e]
                continue
        runs.append([e])
    out = []
    for run in runs:
        if sum(e + 8 in ptrs for e in run) < 2:
            continue
        table, i = run[0], 0
        for e in run:
            whole = {e + k for k in (0, 2, 4, 6)} <= set(refs) or e + 8 in refs
            if e != run[0] and whole:
                table, i = e, 0
            t = ptrs.get(e + 8, 0)
            out.append((table, i, _box(rd, e), t if t in starts else 0))
            i += 1
    return out


def macros():
    """[(table va, index, macro name, handler va or 0)] for the text-macro tables."""
    rd, ptrs, starts, _refs = _image()

    def name(e):
        b = rd(e, 5)
        n = b.split(b"\0")[0]
        return n.decode() if 1 <= len(n) <= 4 and re.fullmatch(rb"[a-z0-9]+", n) and \
            not b[len(n):].strip(b"\0") else None
    out = []
    seen = set()
    for p in sorted(ptrs):
        e = p - 5
        if e in seen or ptrs[p] not in starts or name(e) is None:
            continue
        while name(e - 9) is not None and e - 9 + 5 in ptrs:
            e -= 9
        run = []
        while name(e) is not None and e + 5 in ptrs:
            run.append(e)
            seen.add(e)
            e += 9
        if len(run) >= 3:
            for i, x in enumerate(run):
                t = ptrs[x + 5]
                out.append((run[0], i, name(x), t if t in starts else 0))
    return out


def write():
    import names as namesmod
    known = namesmod.by_address()
    os.makedirs(OUT, exist_ok=True)
    rows = ids()
    with open(os.path.join(OUT, "ids.csv"), "w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["function", "api", "asset", "id", "meaning"])
        for va, api, asset, rid, mean in rows:
            w.writerow(["func_%08X" % va, "func_%08X" % api, asset, rid, mean])
    bt = buttons()
    with open(os.path.join(OUT, "buttons.csv"), "w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["table", "name", "index", "box", "handler"])
        for t, i, b, h in bt:
            w.writerow(["D_%08X" % t, known.get("D_%08X" % t, ""), i, "(%d,%d)-(%d,%d)" % b,
                        "func_%08X" % h if h else ""])
    mc = macros()
    with open(os.path.join(OUT, "macros.csv"), "w", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        w.writerow(["table", "index", "macro", "handler"])
        for t, i, n, h in mc:
            w.writerow(["D_%08X" % t, i, "%" + n, "func_%08X" % h if h else ""])
    by = collections.Counter(r[2] for r in rows)
    print("ids: %d (%s) in %d functions; %d buttons in %d tables (%d handlers); %d macros: "
          "build/assets/" % (len(rows), ", ".join("%s %d" % kv for kv in sorted(by.items())),
                             len({r[0] for r in rows}), len(bt), len({b[0] for b in bt}),
                             len({b[3] for b in bt if b[3]}), len(mc)))


def main():
    if sys.argv[1:] == ["apis"]:
        for asset, a in apis().items():
            print(asset)
            for va, args in sorted(a.items()):
                print("  func_%08X  %s" % (va, ", ".join(
                    "argument %d%s" % (p + 1, " %+d" % o if o else "") for p, o in sorted(args))))
    else:
        write()


if __name__ == "__main__":
    main()
