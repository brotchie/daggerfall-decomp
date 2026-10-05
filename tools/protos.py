#!/usr/bin/env python3
"""Make every file's declaration of a game function agree with its definition.

Each C file declares the functions it calls (`extern int faction_find(short);`), written when
the function was lifted, before its parameters and return value had types. Once the definition
is typed (`struct faction *faction_find(short id)`), the old declarations elsewhere disagree.
Each file compiles on its own, so nothing breaks, but the C says two things.

This tool rewrites each disagreeing declaration as the definition's prototype. A file keeps a
change only when Watcom 10.0a still compiles all of the file's functions to FALL.EXE's bytes and
gives no more warnings than before. More warnings would mean the callers pass another type,
for example an int where the definition takes a pointer.
A declaration can stay different for one of three reasons:
- the code depends on it, as `faction_find(x) + 3` does on an int return;
- the callers need retyping first;
- the definition's types are not visible in the file.
The `report` lists every declaration that stays different.

usage:
  protos.py census [FILES]                 list the declarations that disagree (no compiling)
  protos.py run [FILES] [--in-place] [-j N] [--report CSV]
                                           rewrite them, keeping what matches
  protos.py unify [--names F,G] [--in-place] [--report CSV]
                                           for the functions still disagreeing: search the
                                           types their definition and declarations use for one
                                           prototype that every file (the definition's too)
                                           matches with
Without FILES: src/lifted, src/hand and src/*.c (never src/xngine_c, which is generated).
"""
import argparse
import collections
import csv
import glob
import os
import re
import sys
import tempfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, TOOLS)
ROOT = os.path.dirname(TOOLS)

TYPE_WORDS = {"unsigned", "signed", "char", "short", "int", "long", "void", "struct", "union",
              "const", "volatile", "float", "double", "enum", "far", "near", "_Packed"}
DEF_RE = re.compile(r"^(static\s+)?([A-Za-z_][\w\s\*]*?)\s*\b(\w+)\s*\((.*)\)\s*$")
DECL_RE = re.compile(r"^(extern\s+)?([A-Za-z_][\w\s\*]*?)\s*\b(\w+)\s*\((.*)\)\s*;\s*(/\*.*\*/)?\s*$")


def default_files():
    fs = sorted(glob.glob(os.path.join(ROOT, "src", "lifted", "*.c")))
    fs += sorted(glob.glob(os.path.join(ROOT, "src", "hand", "*.c")))
    fs += sorted(glob.glob(os.path.join(ROOT, "src", "*.c")))
    return fs


def split_top(s):
    """Split a parameter list at its top-level commas."""
    out, d, cur = [], 0, ""
    for ch in s:
        if ch == "(":
            d += 1
        elif ch == ")":
            d -= 1
        if ch == "," and d == 0:
            out.append(cur)
            cur = ""
        else:
            cur += ch
    if cur.strip():
        out.append(cur)
    return [p.strip() for p in out]


def norm(t):
    """A type's canonical spelling."""
    t = re.sub(r"\s+", " ", t.replace("*", " * ")).strip()
    t = re.sub(r"\bconst\b ?", "", t).strip()
    words = t.split(" ")
    stars = "".join(w for w in words if w == "*")
    base = " ".join(w for w in words if w != "*")
    base = {"unsigned": "unsigned int", "signed": "int", "signed int": "int", "long": "int",
            "long int": "int", "unsigned long": "unsigned int", "short int": "short",
            "signed short": "short", "unsigned short int": "unsigned short",
            "signed char": "signed char"}.get(base, base)
    return base + (" " + stars if stars else "")


def param_type(p):
    """A parameter declaration without its name: `struct record *obj` -> `struct record *`."""
    p = p.strip()
    if p in ("void", "...", ""):
        return p
    m = re.match(r"^(.*)\(\s*\*\s*(\w*)\s*\)\s*\((.*)\)$", p)       # function pointer
    if m:
        return "%s (*)(%s)" % (norm(m.group(1)), ", ".join(param_type(q) for q in split_top(m.group(3))))
    arr = p.endswith("]")
    if arr:
        p = re.sub(r"\[[^\]]*\]\s*$", "", p).strip()
    toks = re.findall(r"\w+|\*", p)
    if len(toks) >= 2 and toks[-1] not in TYPE_WORDS and toks[-1] != "*" and \
            not (len(toks) == 2 and toks[0] in ("struct", "union", "enum")):
        p = p[:p.rfind(toks[-1])]
    return norm(p) + (" *" if arr else "")


def proto_of(ret, params):
    ps = split_top(params)
    if not ps:
        return norm(ret), None                       # `()`: unprototyped
    return norm(ret), tuple(param_type(p) for p in ps)


def text_of(ret, params):
    if params is None:
        return "%s(%s)" % ("", "")
    return ", ".join(params)


def spell(ret, name, params, extern):
    r = ret.replace(" *", "*").replace("*", " *") if "*" in ret else ret
    sep = "" if r.endswith("*") else " "
    ps = "void" if params == () else ", ".join(params)
    ps = re.sub(r"\s+\*", " *", ps)
    return "%s%s%s%s(%s);" % ("extern " if extern else "", r, sep, name, ps)


def definitions(files):
    """{name: (ret, params, file)} for every non-static function defined in the files."""
    defs, dup = {}, set()
    for p in files:
        lines = open(p, encoding="latin-1").read().split("\n")
        for k, l in enumerate(lines[:-1]):
            if l.startswith((" ", "\t", "extern", "typedef", "#", "/*", "}")):
                continue
            m = DEF_RE.match(l)
            if not m or lines[k + 1].strip() != "{" or m.group(1):
                continue
            ret, name, params = m.group(2), m.group(3), m.group(4)
            if name in ("if", "while", "for", "switch", "return"):
                continue
            if name in defs:
                dup.add(name)
            defs[name] = proto_of(ret, params) + (p,)
    for n in dup:
        defs.pop(n, None)
    return defs


def declarations(path, defs):
    """[(line index, line, name, (ret, params), wanted line)] for the declarations of a file
    that disagree with the definition."""
    out = []
    lines = open(path, encoding="latin-1").read().split("\n")
    for k, l in enumerate(lines):
        if l.startswith((" ", "\t")) or "(" not in l:
            continue
        m = DECL_RE.match(l)
        if not m or m.group(3) not in defs:
            continue
        name = m.group(3)
        if re.match(r"^\s*(return|else)\b", l):
            continue
        have = proto_of(m.group(2), m.group(4))
        ret, params, _f = defs[name]
        ret, params = merge(have, (ret, params))
        if have == (ret, params):
            continue
        out.append((k, l, name, have, spell(ret, name, params, bool(m.group(1)))))
    return out


def is_ptr(t):
    return "*" in t


def merge(have, defn):
    """The prototype a declaration should have: the definition's, except where the
    declaration has a pointer and the definition an integer (the definition is the one that
    wants typing there; `report` lists those)."""
    def pick(h, d):
        return h if is_ptr(h) and not is_ptr(d) else d
    hr, hp = have
    dr, dp = defn
    r = pick(hr, dr)
    if dp is None or hp is None or len(hp) != len(dp):
        return r, dp
    return r, tuple(pick(h, d) for h, d in zip(hp, dp))


PRIMARY = re.compile(r"^(&?\w+(\[[^\[\]]*\])?|\"(\\.|[^\"\\])*\")$")
INT_CAST = re.compile(r"^\(\s*(int|unsigned|unsigned int|long)\s*\)\s*(.+)$")


def uncast_args(line, ptr_pos):
    """In calls to the functions of ptr_pos, drop an `(int)` cast from the arguments at the
    positions that now take a pointer, where the argument is a single name, element or string."""
    out, i = "", 0
    pat = re.compile(r"\b(%s)\s*\(" % "|".join(map(re.escape, ptr_pos)))
    while True:
        m = pat.search(line, i)
        if not m:
            return out + line[i:]
        j, d = m.end(), 1
        while j < len(line) and d:
            d += {"(": 1, ")": -1}.get(line[j], 0)
            j += 1
        if d:
            return out + line[i:]
        args = split_top(line[m.end():j - 1])
        for a in ptr_pos[m.group(1)]:
            if a < len(args):
                c = INT_CAST.match(args[a])
                if c and PRIMARY.match(c.group(2).strip()):
                    args[a] = c.group(2).strip()
        inner = uncast_args(", ".join(args), ptr_pos)
        out += line[i:m.end()] + inner + ")"
        i = j


class WarnCounter:
    """Counts each variant's compiler warnings (wcc10 keeps them in N####.ERR)."""

    def __init__(self):
        import wcc10
        self.wcc10 = wcc10
        self.orig = wcc10.compile_many
        self.counts = []
        wcc10.compile_many = self.compile_many

    def compile_many(self, srcs, flags, workdir=None):
        objs, td = self.orig(srcs, flags, workdir)
        for k, _s in enumerate(srcs):
            try:
                err = open(os.path.join(td, "N%04d.ERR" % k), errors="replace").read()
            except OSError:
                err = ""
            self.counts.append(sum(1 for l in err.splitlines() if "Warning!" in l))
        return objs, td


def process(path, defs, in_place):
    import structure
    text = open(path, encoding="latin-1").read()
    todo = declarations(path, defs)
    rows = []
    if not todo:
        return rows
    lines = text.split("\n")
    has_records = '#include "records.h"' in text

    def variant(chosen):
        ls = list(lines)
        decl_lines = {c[0] for c in chosen}
        for k, _l, _n, _h, want in chosen:
            ls[k] = want
        ptr_pos = {}
        for c in chosen:
            m = DECL_RE.match(c[4])
            ps = proto_of(m.group(2), m.group(4))[1] or ()
            pos = {i for i, (q, h) in enumerate(zip(ps, c[3][1] or ())) if is_ptr(q) and not is_ptr(h)}
            if pos:
                ptr_pos[c[2]] = pos
        if ptr_pos:
            for k, l in enumerate(ls):
                if k not in decl_lines and l.startswith((" ", "\t")):
                    ls[k] = uncast_args(l, ptr_pos)
        t = "\n".join(ls)
        if not has_records and any("struct " in c[4] for c in chosen):
            t = '#include "records.h"\n' + t
        return t

    wc = WarnCounter()
    ver = structure.Verifier()
    tmp = tempfile.mkdtemp(prefix="protos_", dir=os.environ.get("STRUCT_TMP"))
    flags = structure.file_flags(text)

    def check(variants):
        wc.counts = []
        res = ver.run(variants + [text], [None] * (len(variants) + 1), tmp, flags)
        base, base_w = res[-1], wc.counts[-1] if wc.counts else 0
        if base is None:
            return None
        want = [n for n, ok in base.items() if ok]
        return [r is not None and all(r.get(n) for n in want) and w <= base_w
                for r, w in zip(res[:-1], wc.counts[:-1])]

    keep = []
    ok = check([variant(todo)])
    if ok is None:
        return [dict(file=path, name=c[2], old=c[1], new=c[4], kept="no", why="file does not compile")
                for c in todo]
    if ok[0]:
        keep = list(todo)
    else:
        singles = check([variant([c]) for c in todo])
        keep = [c for c, g in zip(todo, singles) if g]
        if keep and not check([variant(keep)])[0]:
            got = []
            for c in keep:                               # greedy: one at a time
                if check([variant(got + [c])])[0]:
                    got.append(c)
            keep = got
    for c in todo:
        rows.append(dict(file=os.path.relpath(path, ROOT), name=c[2], old=c[1].strip(), new=c[4],
                         kept="yes" if c in keep else "no",
                         why="" if c in keep else "code or warnings differ"))
    if in_place and keep:
        open(path, "w", encoding="latin-1").write(variant(keep))
    import shutil
    shutil.rmtree(tmp, ignore_errors=True)
    return rows


def param_name(p):
    m = re.match(r"^.*\(\s*\*\s*(\w+)\s*\)\s*\(.*\)$", p)
    if m:
        return m.group(1)
    p = re.sub(r"\[[^\]]*\]\s*$", "", p).strip()
    toks = re.findall(r"\w+|\*", p)
    return toks[-1] if toks and toks[-1] not in TYPE_WORDS and toks[-1] != "*" else None


def typed(t, name):
    if "(*)" in t:
        return t.replace("(*)", "(*%s)" % name)
    return t + name if t.endswith("*") else t + " " + name


def sites(text, name):
    """The definition (k, match) and the declarations [(k, match)] of `name` in a file."""
    lines = text.split("\n")
    d, decls = None, []
    pat = re.compile(r"\b%s\s*\(" % re.escape(name))
    for k, l in enumerate(lines):
        if l.startswith((" ", "\t", "#", "/*")) or not pat.search(l):
            continue
        m = DECL_RE.match(l)
        if m and m.group(3) == name:
            decls.append((k, m))
            continue
        m = DEF_RE.match(l)
        if m and m.group(3) == name and not m.group(1) and k + 1 < len(lines) and \
                lines[k + 1].strip() == "{":
            d = (k, m)
    return d, decls


def unify_variant(text, name, cand):
    """The file with `name`'s definition and declarations retyped to cand = (ret, params)."""
    ret, params = cand
    lines = text.split("\n")
    d, decls = sites(text, name)
    ptr = set()
    for k, m in decls:
        have = proto_of(m.group(2), m.group(4))
        lines[k] = spell(ret, name, params, bool(m.group(1)))
        ptr |= {i for i, (q, h) in enumerate(zip(params, have[1] or ())) if is_ptr(q) and not is_ptr(h)}
    if d:
        k, m = d
        have = proto_of(m.group(2), m.group(4))
        ps = split_top(m.group(4))
        names = [param_name(q) for q in ps]
        if None in names or len(names) != len(params):
            return None
        ptr |= {i for i, (q, h) in enumerate(zip(params, have[1] or ())) if is_ptr(q) and not is_ptr(h)}
        r = ret if ret.endswith("*") else ret + " "
        lines[k] = "%s%s(%s)" % (r, name, ", ".join(typed(q, n) for q, n in zip(params, names)))
    if ptr:
        skip = {k for k, _m in decls} | ({d[0]} if d else set())
        for k, l in enumerate(lines):
            if k not in skip and l.startswith((" ", "\t")):
                lines[k] = uncast_args(l, {name: ptr})
    return "\n".join(lines)


def candidates(name, texts, limit=24):
    """Prototypes to try for `name`: per position, the types its definition and declarations use."""
    import itertools
    seen = []
    defn = None
    for path, text in texts.items():
        d, decls = sites(text, name)
        for k, m in decls + ([d] if d else []):
            pr = proto_of(m.group(2), m.group(4))
            if d and k == d[0]:
                defn = pr
            if pr[1] is not None:
                seen.append((pr, 1.5 if d and k == d[0] else 1.0))
    if not seen:
        return []
    n = collections.Counter(len(pr[1]) for pr, _w in seen).most_common(1)[0][0]
    pos = [collections.Counter() for _ in range(n + 1)]
    for (r, ps), w in seen:
        if len(ps) != n:
            continue
        pos[0][r] += w
        for i, q in enumerate(ps):
            pos[i + 1][q] += w
    ranked = [[t for t, _c in c.most_common()] for c in pos]
    # the most typed first (pointers over integers), then the types most files use
    combos = sorted(itertools.product(*[range(len(r)) for r in ranked]),
                    key=lambda ix: (-sum(is_ptr(ranked[i][j]) for i, j in enumerate(ix)), sum(ix), ix))
    # never give a pointer of the definition back to an integer
    keep = [] if defn is None or defn[1] is None or len(defn[1]) != n else \
        [i for i, q in enumerate((defn[0],) + defn[1]) if is_ptr(q)]
    out = []
    for ix in combos:
        types = [ranked[i][j] for i, j in enumerate(ix)]
        if all(is_ptr(types[i]) for i in keep):
            out.append((types[0], tuple(types[1:])))
        if len(out) == limit:
            break
    return out


def unify(names, files, in_place, log=print):
    """For each function, the first candidate prototype under which its definition and every
    declaring file still match (no more warnings) is written to all of them."""
    import shutil
    import structure
    wc = WarnCounter()
    ver = structure.Verifier()
    tmp = tempfile.mkdtemp(prefix="unify_", dir=os.environ.get("STRUCT_TMP"))
    texts = {p: open(p, encoding="latin-1").read() for p in files}
    rows = []
    for name in names:
        pat = re.compile(r"\b%s\b" % re.escape(name))
        mine = {p: t for p, t in texts.items() if pat.search(t) and any(sites(t, name))}
        cands = candidates(name, mine)
        ok = [True] * len(cands)
        variants = {}
        for p, t in mine.items():
            vs = [unify_variant(t, name, c) for c in cands]
            live = [i for i, v in enumerate(vs) if v is not None and ok[i]]
            if not live:
                break
            wc.counts = []
            res = ver.run([vs[i] for i in live] + [t], [None] * (len(live) + 1), tmp,
                          structure.file_flags(t))
            base, bw = res[-1], wc.counts[-1]
            want = [f for f, g in (base or {}).items() if g]
            for j, i in enumerate(live):
                r = res[j]
                good = base is not None and r is not None and all(r.get(f) for f in want) and \
                    wc.counts[j] <= bw
                ok[i] = ok[i] and good
            for i, v in enumerate(vs):
                if v is None:
                    ok[i] = False
            variants[p] = vs
        win = next((i for i, g in enumerate(ok) if g and len(variants) == len(mine)), None)
        if win is None:
            log("%-34s no prototype fits all %d files (%d tried)" % (name, len(mine), len(cands)))
            rows.append(dict(name=name, files=len(mine), prototype=""))
            continue
        proto = spell(cands[win][0], name, cands[win][1], False)
        log("%-34s %s  (%d files)" % (name, proto, len(mine)))
        rows.append(dict(name=name, files=len(mine), prototype=proto))
        for p in mine:
            texts[p] = variants[p][win]
            if in_place:
                open(p, "w", encoding="latin-1").write(texts[p])
    shutil.rmtree(tmp, ignore_errors=True)
    return rows


def _worker(args):
    path, defs, in_place = args
    try:
        return process(path, defs, in_place)
    except Exception:  # noqa: BLE001
        import traceback
        return [dict(file=path, name="", old="", new="", kept="no", why="ERROR " + traceback.format_exc())]


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("cmd", choices=["census", "run", "unify"])
    ap.add_argument("--names", help="unify: these functions (comma-separated); default: every "
                    "function whose declarations still disagree")
    ap.add_argument("files", nargs="*")
    ap.add_argument("--in-place", action="store_true")
    ap.add_argument("-j", type=int, default=4)
    ap.add_argument("--report")
    a = ap.parse_args()
    allf = default_files()
    files = [os.path.abspath(f) for f in a.files] or allf
    defs = definitions(allf)
    if a.cmd == "census":
        n = 0
        for p in files:
            for _k, l, name, have, want in declarations(p, defs):
                n += 1
                print("%s: %s\n    -> %s" % (os.path.relpath(p, ROOT), l.strip(), want))
        print("%d declarations disagree with their definition (%d functions defined)" % (n, len(defs)))
        return
    if a.cmd == "unify":
        names = a.names.split(",") if a.names else sorted({d[2] for p in allf for d in declarations(p, defs)})
        rows = unify(names, allf, a.in_place)
        print("%d of %d functions have one prototype now" % (sum(1 for r in rows if r["prototype"]), len(rows)))
        if a.report:
            with open(a.report, "w", newline="") as f:
                w = csv.DictWriter(f, fieldnames=["name", "files", "prototype"])
                w.writeheader()
                w.writerows(rows)
        return
    from multiprocessing import Pool
    rows = []
    with Pool(a.j) as pool:
        for rs in pool.imap_unordered(_worker, [(p, defs, a.in_place) for p in files]):
            rows += rs
            for r in rs:
                if r["why"].startswith("ERROR"):
                    print(r["file"], r["why"])
    kept = sum(1 for r in rows if r["kept"] == "yes")
    print("%d declarations disagreed; %d now agree, %d left" % (len(rows), kept, len(rows) - kept))
    if a.report:
        with open(a.report, "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=["file", "name", "kept", "why", "old", "new"])
            w.writeheader()
            w.writerows(sorted(rows, key=lambda r: (r["file"], r["name"])))


if __name__ == "__main__":
    main()
