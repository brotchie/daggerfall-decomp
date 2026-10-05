#!/usr/bin/env python3
"""Give the lifter's untyped globals their types, keeping every function byte-identical
(docs/natural_c.md).

The lifter declares every global as `extern char g[];` and reads it through a cast at each
use: `*(int *)g`, `*(signed char *)g`, `*(char **)g`. A global that every file uses through
one scalar type T (and never through byte offsets) becomes `extern T g;`, its loads and
stores plain `g`, and its address `&g` (`(int)g` becomes `(int)&g`). A global read as an
array of one element type, `*(short *)(g + (i * 2))`, `*(int *)(g + (i << 2))`, becomes
`extern T g[];` with `g[i]` (--arrays). The decision is made over all the files given, so a
global gets the same declaration everywhere; globals used through several types are left
alone and listed. Every changed file is compiled with Watcom 10.0a and compared with
FALL.EXE; a global whose typing breaks a function is put back in that file.

Record globals (tools/offset_casts.py GLOBAL_TYPES) are left to offset_casts.py.

usage:
  type_globals.py census FILE.c ... [--arrays] [--list]
        what would be typed, what is mixed, and why
  type_globals.py run FILE.c ... [--arrays] [-o DIR | --in-place] [-j N]
        type, verify and write
"""
import argparse
import collections
import os
import re
import sys
import time

TOOLS = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, TOOLS)

TYPE = r"(?:(?:unsigned|signed|struct|const)\s+)*[A-Za-z_]\w*(?:\s*\*)*"
PRE_DEREF = re.compile(r"\*\(\s*(" + TYPE + r")\s*\*\s*\)\s*$")
PRE_ELEM = re.compile(r"\*\(\s*(" + TYPE + r")\s*\*\s*\)\s*\(\s*$")
PRE_CAST = re.compile(r"\(\s*(" + TYPE + r")\s*\)\s*$")
DECL = re.compile(r"^extern char (\w+)\[\];[ \t]*$", re.M)

SIZE = {"char": 1, "signed char": 1, "unsigned char": 1, "short": 2, "unsigned short": 2,
        "signed short": 2, "int": 4, "unsigned int": 4, "unsigned": 4, "long": 4,
        "unsigned long": 4, "float": 4, "double": 8}
BASIC_WORDS = {"char", "short", "int", "long", "unsigned", "signed", "float", "double"}


def norm_type(t):
    t = re.sub(r"\s+", " ", t.strip())
    t = re.sub(r"\s*\*", " *", t).replace("* *", "**")
    return t


def type_size(t):
    if "*" in t:
        return 4
    return SIZE.get(t)


def decl_of(t, g, array=False):
    t = norm_type(t)
    if t.endswith("*"):
        return "extern %s%s%s;" % (t, g, "[]" if array else "")
    return "extern %s %s%s;" % (t, g, "[]" if array else "")


def match_paren(s, i):
    d = 0
    for k in range(i, len(s)):
        if s[k] == "(":
            d += 1
        elif s[k] == ")":
            d -= 1
            if d == 0:
                return k
    return -1


class Use:
    __slots__ = ("kind", "t", "start", "end", "index", "g", "istart", "cast", "cstart", "lv",
                 "rec")

    def __init__(self, kind, g, start, end, t=None, index=None, istart=None):
        self.kind, self.g, self.start, self.end, self.t, self.index = \
            kind, g, start, end, t, index
        self.istart = istart        # where index starts in the text
        self.cast = self.cstart = None  # a cast right before a deref: its type, its start
        self.lv = None              # a deref used as an lvalue: assign, inc, compound, op=, addr
        self.rec = False            # the pointer read is the base of `*(T *)(p + N)`

    def __repr__(self):
        return "<%s %s %s>" % (self.kind, self.g, self.t)


def array_index(off, size):
    """The element index of a byte offset expression: (text, start, end) with the index's
    place in off (None for a constant), or None. `(i << 2)` and `(i * 4)` for size 4, a
    constant multiple of size, any expression for size 1."""
    lead = len(off) - len(off.lstrip())
    off = off.strip()
    m = re.match(r"^(\d+)$", off)
    if m:
        n = int(m.group(1))
        return (str(n // size), None, None) if n % size == 0 else None
    wrapped = off.startswith("(") and match_paren(off, 0) == len(off) - 1
    if size == 1:
        if wrapped:
            return off[1:-1], lead + 1, lead + len(off) - 1
        return off, lead, lead + len(off)
    if not wrapped:
        return None
    inner = off[1:-1]
    # the top-level operator must be the scale
    for op, val in (("<<", {2: "1", 4: "2", 8: "3"}.get(size)), ("*", str(size))):
        if val is None:
            continue
        k = top_level_last(inner, op)
        if k is None:
            continue
        left, right = inner[:k].strip(), inner[k + len(op):].strip()
        if right == val and left:
            if top_level_ops(left) - {"*", "/", "%"} and not (
                    left.startswith("(") and match_paren(left, 0) == len(left) - 1):
                return None      # `a + b << 2`: not one term
            st = lead + 1 + len(inner[:k]) - len(inner[:k].lstrip())
            return left, st, st + len(left)
    return None


def top_level_last(e, op):
    d = 0
    found = None
    k = 0
    while k < len(e):
        c = e[k]
        if c in "([":
            d += 1
        elif c in ")]":
            d -= 1
        elif d == 0 and e.startswith(op, k):
            if op == "*" and (k == 0 or e[k - 1] in "(*" or e[k + 1:k + 2] == ")"):
                k += 1
                continue
            if op == "<<" or (op == "*" and e[k - 1:k] != "*"):
                found = k
            k += len(op)
            continue
        k += 1
    return found


def top_level_ops(e):
    d = 0
    ops = set()
    for k, c in enumerate(e):
        if c in "([":
            d += 1
        elif c in ")]":
            d -= 1
        elif d == 0 and c in "+-*/%<>&|^?":
            ops.add(c)
    return ops


def scan(text, names):
    """Every use of the globals `names` in text (not their declarations)."""
    uses = collections.defaultdict(list)
    if not names:
        return uses
    pat = re.compile(r"(?<![\w.>])(%s)\b" % "|".join(sorted(map(re.escape, names), key=len,
                                                                 reverse=True)))
    for m in pat.finditer(text):
        g = m.group(1)
        ls = text.rfind("\n", 0, m.start()) + 1
        if text.startswith("extern", ls) or (text.startswith("#", ls) and
                                              not text.startswith("#define", ls)):
            continue
        pre = text[max(0, m.start() - 80):m.start()]
        post = text[m.end():m.end() + 2]
        md = PRE_DEREF.search(pre)
        if md and not post.startswith("["):
            u = Use("deref", g, m.start() - len(md.group(0)), m.end(), norm_type(md.group(1)))
            before = text[max(0, u.start - 60):u.start]
            mc = PRE_CAST.search(before)
            if mc and mc.group(1).split()[0] in BASIC_WORDS | {"struct", "void"}:
                u.cast = norm_type(mc.group(1))
                u.cstart = u.start - len(mc.group(0))
            after = text[u.end:u.end + 8]
            ma = re.match(r"(\)*)\s*(\+\+|--|<<=|>>=|[-+*/%&|^]=|=(?!=))", after)
            if ma:
                op = ma.group(2)
                u.lv = {"=": "assign", "++": "inc", "--": "inc", "+=": "compound",
                        "-=": "compound", "&=": "compound", "|=": "compound",
                        "^=": "compound"}.get(op, "op=")
            elif re.search(r"(\+\+|--)\s*\(*\s*$", before):
                u.lv = "inc"
            elif re.search(r"(?<![&\w)\]])&\s*\(*\s*$", before):
                u.lv = "addr"
            if re.search(r"\*\(\s*" + TYPE + r"\s*\*\s*\)\s*\(\s*(\(\s*char\s*\*\s*\)\s*)?$",
                         before) and re.match(r"\s*\+\s*\d+\s*\)", after):
                u.rec = True
            uses[g].append(u)
            continue
        me = PRE_ELEM.search(pre)
        if me:
            p = m.start() - 1
            while text[p] != "(":
                p -= 1
            q = match_paren(text, p)
            inner = text[m.end():q]
            mm = re.match(r"^\s*\+\s*(.*)$", inner, re.S)
            if mm:
                uses[g].append(Use("elem", g, m.start() - len(me.group(0)), q + 1,
                                   norm_type(me.group(1)), mm.group(1), m.end() + mm.start(1)))
                continue
        if pre.endswith("&") and not pre.endswith("&&"):
            uses[g].append(Use("amp", g, m.start(), m.end()))
            continue
        mc = PRE_CAST.search(pre)
        if mc and mc.group(1).split()[0] in BASIC_WORDS | {"struct", "void"} \
                and not pre[:len(pre) - len(mc.group(0))].rstrip().endswith(("sizeof",)):
            uses[g].append(Use("cast", g, m.start(), m.end(), norm_type(mc.group(1))))
            continue
        rest = text[m.end():m.end() + 3].lstrip()
        uses[g].append(Use("offs" if rest[:1] in ("+", "-", "[") else "bare", g, m.start(),
                           m.end()))
    return uses


def is_int(t):
    return "*" not in t and t in SIZE and t not in ("float", "double")


def mixed_cost(uses, d):
    """How many uses of a global read through other types need a cast if it is declared d."""
    n = 0
    for u in uses:
        if u.kind != "deref" or u.t == d:
            continue
        if u.lv in ("assign", "inc", "compound") and is_int(u.t) and is_int(d):
            continue
        if u.lv is None and u.cast and type_size(u.cast) == type_size(d) and \
                (is_int(u.cast) or "*" in u.cast):
            continue
        n += 1
    return n


def census(paths, arrays=False, mixed=False):
    """{global: decision}, {global: reason}: the typing of each `extern char g[];` global
    over all the files."""
    try:
        from offset_casts import GLOBAL_TYPES
        skip = {g.rstrip("[]") for g in GLOBAL_TYPES}
    except Exception:  # noqa: BLE001
        skip = set()
    per = collections.defaultdict(list)     # g -> uses over all files
    other_decl = collections.defaultdict(set)
    for p in paths:
        text = open(p, encoding="latin1").read()
        names = set(DECL.findall(text))
        for m in re.finditer(r"^extern ([^;()]*?)\b(\w+)(\[\d*\])?;", text, re.M):
            if m.group(2) not in names:
                other_decl[m.group(2)].add(norm_type(m.group(1)) + (m.group(3) or ""))
        u = scan(text, names)
        for g in names:
            per[g].extend(u.get(g, []))
    decide, why = {}, {}
    for g, uses in per.items():
        if g in skip:
            why[g] = "record global (offset_casts.py)"
            continue
        kinds = collections.Counter(u.kind for u in uses)
        dts = {u.t for u in uses if u.kind in ("deref", "elem")}
        if not uses:
            why[g] = "unused"
            continue
        if not dts:
            why[g] = "address only"
            continue
        if len(dts) > 1:
            why[g] = "mixed types: " + ", ".join(sorted(dts))
            sizes = {type_size(t) for t in dts}
            if not mixed or kinds["elem"] or kinds["offs"] or len(sizes) != 1 or \
                    None in sizes or any(not (is_int(t) or "*" in t) for t in dts):
                continue
            if kinds["cast"] + kinds["bare"] > kinds["deref"]:
                continue
            if any(u.rec for u in uses):
                why[g] = "record pointer (%d offset reads): left for offset_casts.py" % sum(
                    u.rec for u in uses)
                continue
            hand = {norm_type(x) for x in other_decl.get(g, ())}
            order = {"unsigned char": 0, "signed char": 1, "char": 2}
            cnt = collections.Counter(u.t for u in uses if u.kind == "deref")
            d = min(dts, key=lambda t: (mixed_cost(uses, t), t not in hand,
                                        order.get(t, 3), -cnt[t], t))
            decide[g] = ("mixed", d)
            continue
        t = dts.pop()
        if ("*" in t or t == "int") and sum(u.rec for u in uses) >= 1:
            # a pointer to a record read at offsets: for records.h and offset_casts.py
            why[g] = "record pointer (%d offset reads): left for offset_casts.py" % sum(
                u.rec for u in uses)
            continue
        if type_size(t) is None and "*" not in t:
            # a struct by value needs its definition in every file: not here
            why[g] = "type " + t
            continue
        if kinds["elem"]:
            if not arrays:
                why[g] = "array use (--arrays)"
                continue
            size = type_size(t)
            bad = [u for u in uses if u.kind == "elem" and
                   (size is None or array_index(u.index, size) is None)]
            # an index that is itself an elem use of g: leave it
            
            if bad:
                why[g] = "byte offsets not a multiple of %s: %s" % (t, bad[0].index.strip())
                continue
            decide[g] = ("array", t)
            continue
        if kinds["offs"] and not arrays:
            why[g] = "%s with byte offsets from its address" % t
            continue
        if kinds["offs"]:
            why[g] = "%s array with byte offsets from its address" % t
            continue
        if kinds["cast"] + kinds["bare"]:
            why[g] = "scalar %s with %d address uses" % (t, kinds["cast"] + kinds["bare"])
            if kinds["cast"] + kinds["bare"] > kinds["deref"]:
                continue
        decide[g] = ("scalar", t)
        why.pop(g, None)
    for g, d in decide.items():
        if other_decl.get(g):
            why[g] = "also declared " + "/".join(sorted(other_decl[g]))
    return decide, why


def rewrite(text, decide, only=None, exact=()):
    """text with the decided globals typed (only those in `only`, if given). A mixed global
    in `exact` keeps its other-typed uses as `*(T *)&g`."""
    names = set(DECL.findall(text))
    todo = {g: decide[g] for g in names if g in decide and (only is None or g in only)}
    if not todo:
        return text, []
    uses = scan(text, set(todo))
    edits = []          # (start, end, prefix, inner (start, end) or None, suffix)
    for g, (kind, t) in todo.items():
        for u in uses.get(g, []):
            if kind == "mixed" and u.kind == "deref" and u.t != t:
                x = u.t
                if g in exact or u.lv in ("op=", "addr") or (
                        u.lv and not (is_int(x) and is_int(t))):
                    edits.append((u.start, u.end, "*(%s%s*)&%s" % (
                        x, "" if x.endswith("*") else " ", g), None, ""))
                elif u.lv:
                    edits.append((u.start, u.end, g, None, ""))
                elif u.cast and type_size(u.cast) == type_size(t) and \
                        (is_int(u.cast) or "*" in u.cast):
                    edits.append((u.cstart, u.end, g if u.cast == t else "(%s)%s" % (u.cast, g),
                                  None, ""))
                else:
                    edits.append((u.start, u.end, "((%s)%s)" % (x, g) if "*" in x else
                                  "(%s)%s" % (x, g), None, ""))
                continue
            if kind in ("scalar", "mixed"):
                if u.kind == "deref":
                    edits.append((u.start, u.end, g, None, ""))
                elif u.kind == "amp":
                    pass
                elif u.kind == "cast":
                    edits.append((u.start, u.end, "&" + g, None, ""))
                else:
                    edits.append((u.start, u.end, "((char *)&%s)" % g, None, ""))
            else:
                size = type_size(t)
                if u.kind == "deref":
                    edits.append((u.start, u.end, "%s[0]" % g, None, ""))
                elif u.kind == "elem":
                    ix, a, b = array_index(u.index, size)
                    if a is None:
                        edits.append((u.start, u.end, "%s[%s]" % (g, ix), None, ""))
                    else:
                        edits.append((u.start, u.end, g + "[", (u.istart + a, u.istart + b),
                                      "]"))
                elif u.kind == "amp":
                    edits.append((u.start - 1, u.end, "((char *)%s)" % g, None, ""))
                elif u.kind == "cast":
                    pass
                else:
                    edits.append((u.start, u.end, "((char *)%s)" % g, None, ""))
    edits.sort(key=lambda e: (e[0], -e[1]))

    def render(lo, hi, eds):
        out = []
        pos = lo
        k = 0
        while k < len(eds):
            a, b, pre, inner, suf = eds[k]
            if a < pos or b > hi:
                k += 1
                continue
            out.append(text[pos:a])
            nested = [e for e in eds[k + 1:] if e[0] >= a and e[1] <= b]
            if inner is None:
                out.append(pre)
            else:
                out.append(pre + render(inner[0], inner[1], nested) + suf)
            pos = b
            k += 1 + len(nested)
        out.append(text[pos:hi])
        return "".join(out)
    out = render(0, len(text), edits)
    for g, (kind, t) in todo.items():
        if kind in ("scalar", "mixed"):
            # `(*(int *)g)++` became `(g)++`: no parentheses around a name
            def bare(m):
                before = out[:m.start()].rstrip()
                if before and (before[-1].isalnum() or before[-1] in "_)]"):
                    return m.group(0)       # a call, `if (g)`, `(int)(g)`: as it is
                return g
            out = re.sub(r"\(\s*%s\s*\)" % re.escape(g), bare, out)
        out = re.sub(r"^extern char %s\[\];" % re.escape(g),
                     decl_of(t, g, kind == "array").replace("\\", "\\\\"), out, flags=re.M)
    return out, sorted(todo)


def process(path, out, decide, log=print):
    import tempfile
    import structure
    t0 = time.time()
    text = open(path, encoding="latin1").read()
    new, done = rewrite(text, decide)
    if not done:
        with open(out, "w", encoding="latin1") as f:
            f.write(text)
        return dict(file=path, typed=0, reverted=0, seconds=0)
    ver = structure.Verifier()
    tmp = tempfile.mkdtemp(prefix="typeg_", dir=os.environ.get("STRUCT_TMP"))
    flags = structure.file_flags(text)
    r, orig = ver.run([new, text], [None, None], tmp, flags)
    keep = list(done)
    exact = set()
    mixed = [g for g in done if decide[g][0] == "mixed"]
    if orig is None:
        log("%s: the original does not compile" % path)
        keep = []
    else:
        want = [n for n, ok in orig.items() if ok]

        def good(rr):
            return rr is not None and all(rr.get(n) for n in want)
        if not good(r) and mixed:
            # the other-typed uses of the mixed globals as `*(T *)&g`, then back to the
            # natural form one global at a time
            t2, _ = rewrite(text, decide, None, set(mixed))
            rr, = ver.run([t2], [None], tmp, flags)
            if good(rr):
                exact = set(mixed)
                variants = [rewrite(text, decide, None, exact - {g})[0] for g in mixed]
                res = ver.run(variants, [None] * len(variants), tmp, flags)
                for g, rr in zip(mixed, res):
                    if good(rr):
                        exact.discard(g)
                if exact != set(mixed):
                    rr, = ver.run([rewrite(text, decide, None, exact)[0]], [None], tmp, flags)
                    if not good(rr):
                        exact = set(mixed)
                r = None if not exact else rr
                r = rr if good(rr) else r
                if exact:
                    log("%s: other-typed uses kept as casts for %s" % (
                        os.path.basename(path), ", ".join(sorted(exact))))
                r = {n: True for n in want}
        if not good(r):
            # find the globals to put back: each one left out
            variants = []
            for g in done:
                t2, _ = rewrite(text, decide, set(done) - {g}, set(mixed))
                variants.append(t2)
            res = ver.run(variants, [None] * len(variants), tmp, flags)
            culprits = []
            for g, rr in zip(done, res):
                if good(rr):
                    culprits = [g]
                    break
            exact = set(mixed)
            if not culprits:
                # one at a time
                variants = []
                for g in done:
                    t2, _ = rewrite(text, decide, {g}, exact)
                    variants.append(t2)
                res = ver.run(variants, [None] * len(variants), tmp, flags)
                keep = [g for g, rr in zip(done, res) if good(rr)]
                t2, _ = rewrite(text, decide, set(keep), exact)
                rr, = ver.run([t2], [None], tmp, flags)
                if not good(rr):
                    keep = []
            else:
                keep = [g for g in done if g not in culprits]
            log("%s: put back %s" % (os.path.basename(path), ", ".join(
                g for g in done if g not in keep)))
        new, _ = rewrite(text, decide, set(keep), exact)
    import shutil
    shutil.rmtree(tmp, ignore_errors=True)
    with open(out, "w", encoding="latin1") as f:
        f.write(new if keep else text)
    dt = time.time() - t0
    return dict(file=path, typed=len(keep), reverted=len(done) - len(keep),
                reverted_names=" ".join(g for g in done if g not in keep),
                exact=" ".join(sorted(exact & set(keep))),
                seconds=round(dt, 1), compiles=ver.compiles)


def _worker(args):
    path, out, decide = args
    lines = []
    try:
        r = process(path, out, decide, log=lines.append)
    except Exception:  # noqa: BLE001
        import traceback
        lines.append("ERROR %s: %s" % (path, traceback.format_exc()))
        r = dict(file=path, error=1)
    return r, lines


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd")
    cp = sub.add_parser("census")
    cp.add_argument("files", nargs="+")
    cp.add_argument("--arrays", action="store_true")
    cp.add_argument("--mixed", action="store_true")
    cp.add_argument("--list", action="store_true")
    rp = sub.add_parser("run")
    rp.add_argument("files", nargs="+")
    rp.add_argument("--arrays", action="store_true")
    rp.add_argument("--mixed", action="store_true")
    rp.add_argument("-o", dest="outdir")
    rp.add_argument("--in-place", action="store_true")
    rp.add_argument("-j", type=int, default=1)
    rp.add_argument("--report")
    a = ap.parse_args()
    if a.cmd == "census":
        decide, why = census(a.files, a.arrays, a.mixed)
        kinds = collections.Counter(k for k, _t in decide.values())
        reasons = collections.Counter(re.sub(r"\d+", "N", re.sub(r":.*", "", w)) for g, w in why.items()
                                      if g not in decide)
        print("typed:", dict(kinds), "of", len(decide) + len([g for g in why if g not in decide]))
        for r, n in reasons.most_common():
            print("  left: %5d  %s" % (n, r))
        if a.list:
            for g in sorted(decide):
                print("%-28s %s %s  %s" % (g, decide[g][0], decide[g][1], why.get(g, "")))
            for g in sorted(why):
                if g not in decide:
                    print("%-28s -- %s" % (g, why[g]))
        return
    if a.cmd == "run":
        if not a.in_place and not a.outdir:
            raise SystemExit("give -o DIR or --in-place")
        decide, why = census(a.files, a.arrays, a.mixed)
        if a.outdir:
            os.makedirs(a.outdir, exist_ok=True)
        jobs = [(f, f if a.in_place else os.path.join(a.outdir, os.path.basename(f)), decide)
                for f in a.files]
        rows = []
        if a.j > 1:
            import multiprocessing
            with multiprocessing.Pool(a.j) as pool:
                for r, lines in pool.imap_unordered(_worker, jobs):
                    for l in lines:
                        print(l, flush=True)
                    rows.append(r)
        else:
            for job in jobs:
                r, lines = _worker(job)
                for l in lines:
                    print(l, flush=True)
                rows.append(r)
        print("TOTAL: %d global declarations typed in %d files, %d put back; %d globals "
              "decided, %d left" % (sum(r.get("typed", 0) for r in rows), len(rows),
                                    sum(r.get("reverted", 0) for r in rows), len(decide),
                                    len([g for g in why if g not in decide])))
        if a.report:
            import csv
            keys = sorted({k for r in rows for k in r})
            with open(a.report, "w", newline="") as f:
                w = csv.DictWriter(f, fieldnames=keys)
                w.writeheader()
                for r in rows:
                    w.writerow(r)
        return
    ap.print_help()


if __name__ == "__main__":
    main()
