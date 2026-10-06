#!/usr/bin/env python3
"""Globals that hold addresses but are declared as bytes (docs/port.md, phase 3).

The lifter declared many globals `extern char g[];` and read them through casts. Where g holds
a pointer (`*(char **)g`, `*(int *)g`) or is a table of pointers (`*(int *)(g + (i << 2))`),
the native build needs the declaration to say so: a pointer is 8 bytes there, and a table's
entries are 8 bytes apart. tools/port_data.py reports these: declarations that disagree
across files about where pointers are (`disagree`), and tables whose type has no pointer where
FALL.EXE relocates one (`narrow`).

This tool gives each such global, in every file, the declaration other files (or the data)
show: `extern struct character *player_character;`, `extern char *region_names[];`. Its uses
are rewritten with clang's AST:
  - a pointer:  `*(T *)g` -> `g` or `(T)g`, `*(int *)g` -> `(iptr)g`; the address `g` -> `(char *)&g`
  - a table:    `*(T *)(g + (i << 2))`, `((T *)g)[i]` -> `g[i]` (with a cast when T differs:
                `(iptr)g[i]`); `*(T *)g` -> `g[0]`; the table's address `g` -> `(char *)g`
When Watcom's code for a function changes, the uses fall back to forms that keep the code:
`*(T *)&g` for a pointer (an int read of it becomes `*(iptr *)&g`). A global whose file still
does not match keeps its old declaration there (listed).

  port_globals.py plan [--out JSON]             the decisions (from port_data's report)
  port_globals.py tables [--out JSON]           byte arrays the code reads as pointer tables
                                                (4-byte strides; zero-filled data that
                                                port_data cannot see relocations in)
  port_globals.py apply --plan JSON [--base DIR] [--in-place] [--report CSV] [FILES]
"""
import argparse
import collections
import csv
import json
import os
import re
import shutil
import sys
import tempfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, TOOLS)
ROOT = os.path.dirname(TOOLS)

import port_iptr as pi  # noqa: E402

BYTES = {"char", "signed char", "unsigned char"}
DECL_LINE = r"^extern\s+([^;()]*?)\s*\b%s\b\s*((?:\[[^\]]*\])*)\s*;.*$"


def game_files():
    return pi.game_files()


# ---- decisions ----------------------------------------------------------------------------

def plan(log=print):
    """{name: {"type": element or scalar type, "array": bool}} for the globals to convert."""
    import port_data
    decls = port_data.declarations()
    out = {}
    for name, ds in decls.items():
        kinds = []
        for path, member, _incomplete in ds:
            t = re.sub(r"\s+", " ", member)
            m = re.match(r"^(.*?)\s*__v((?:\[[^\]]*\])*)$", t)
            if not m:
                continue
            base, dims = m.group(1).strip(), m.group(2)
            kinds.append((os.path.relpath(path, ROOT), base, dims))
        byte_files = [k for k in kinds if k[1] in BYTES and k[2]]
        ptrs = [k for k in kinds if k[1].endswith("*") and "(" not in k[1]]
        if not byte_files or not ptrs:
            continue
        # the pointer declaration most files use; a struct pointer over char * or void *
        c = collections.Counter((k[1], bool(k[2])) for k in ptrs)
        if re.match(r"^scratch_[0-9a-f]{6}$", name):
            continue        # scratch slots, reused for several things
        best = sorted(c.items(), key=lambda kv: (-kv[1], -("struct" in kv[0][0]), kv[0]))[0][0]
        out[name] = {"type": best[0], "array": best[1]}
    return out


def narrow_tables(path):
    """Tables of pointers declared as bytes, from narrow_class.txt (built from port_data's
    report): name -> element type."""
    out = {}
    for l in open(path):
        m = re.match(r"(\w+)\s+n=(\d+)\s+first=(\S+)\s+regular=(\w+)\s+(.*?) \|", l)
        if not m:
            continue
        name, n, first, reg, decls = m.groups()
        ds = [d.strip() for d in decls.split(";") if d.strip()]
        if reg != "True" or int(first, 16) % 4 or not ds:
            continue
        if all(re.match(r"^(signed |unsigned )?char %s\[\]$" % name, d) for d in ds):
            out[name] = {"type": "char *", "array": True}
    return out


# ---- rewriting ----------------------------------------------------------------------------

def scan_tables(path):
    """The byte-array globals a file reads as 4-byte entries (`*(T *)(g + (i << 2))`):
    {name: [(T, offset kind)]}; offset kind 'idx' (a multiple of 4), 'odd' otherwise."""
    rel = os.path.relpath(path, ROOT)
    ast = pi.load_ast(rel)
    pi.annotate(ast)
    found = collections.defaultdict(list)
    decls = {}
    for n in ast.get("inner", []):
        if n.get("kind") == "VarDecl" and re.match(r"^(signed |unsigned )?char ?\[", pi.dqt(n)):
            decls[n["id"]] = n["name"]

    def strip(n):
        while n is not None and n.get("kind") in ("ParenExpr", "ImplicitCastExpr"):
            n = pi.kids(n)[0]
        return n

    def mult4(e):
        e = strip(e)
        k = e.get("kind")
        if k == "IntegerLiteral":
            return int(e.get("value", "0")) % 4 == 0
        if k == "BinaryOperator":
            a, b = pi.kids(e)
            op = e.get("opcode")
            if op == "<<":
                b = strip(b)
                return b.get("kind") == "IntegerLiteral" and int(b.get("value", "0")) >= 2
            if op == "*":
                for y in (strip(a), strip(b)):
                    if y.get("kind") == "IntegerLiteral" and int(y.get("value", "0")) % 4 == 0:
                        return True
                return False
            if op == "+":
                return mult4(a) and mult4(b)
        return False

    def visit(n, parent):
        if n.get("kind") == "UnaryOperator" and n.get("opcode") == "*":
            c = strip(pi.kids(n)[0])
            if c is not None and c.get("kind") == "CStyleCastExpr" and pi.dqt(c).endswith("*"):
                e = strip(pi.kids(c)[0])
                X = pi.qt(c)[:-1].strip()
                if e is not None and e.get("kind") == "BinaryOperator" and e.get("opcode") == "+":
                    g = strip(pi.kids(e)[0])
                    if g is not None and g.get("kind") == "DeclRefExpr" and \
                            (g.get("referencedDecl") or {}).get("id") in decls:
                        found[decls[g["referencedDecl"]["id"]]].append(
                            (X, "idx" if mult4(pi.kids(e)[1]) else "odd"))
        for k in pi.kids(n):
            visit(k, n)
    visit(ast, None)
    return found


def plan_tables(files):
    """Byte-array globals that some file reads as a table of pointers at a 4-byte stride."""
    from concurrent.futures import ThreadPoolExecutor
    with ThreadPoolExecutor(os.cpu_count()) as ex:
        res = list(ex.map(scan_tables, files))
    uses = collections.defaultdict(list)
    for r in res:
        for k, v in r.items():
            uses[k] += v
    out, left = {}, {}
    for name, us in sorted(uses.items()):
        ptr = [x for x, k in us if x.endswith("*") or norm(x) in ("iptr", "uptr")]
        if not ptr:
            continue
        sizes = {x for x, _k in us if not (x.endswith("*") or is_int_type(x))}
        if re.match(r"^scratch_[0-9a-f]{6}$", name):
            left[name] = "scratch slot reused as a table"
            continue
        if any(k == "odd" for _x, k in us) or sizes:
            left[name] = "read at other offsets or widths: %s" % sorted({"%s/%s" % u for u in us})
            continue
        types = collections.Counter(x for x in ptr if x.endswith("*"))
        T = types.most_common(1)[0][0] if types and len(types) == 1 and \
            all(x.endswith("*") for x, _k in us) else "iptr"
        out[name] = {"type": T, "array": True}
    return out, left


def norm(t):
    return re.sub(r"\s+", "", t)


def is_int_type(t):
    return norm(t) in ("int", "unsignedint", "unsigned", "iptr", "uptr", "long", "unsignedlong")


class Uses:
    """The edits that convert one global in one file: {'decl': edit, 'uses': [(clean, neutral)]}."""


def file_edits(path, decisions):
    """For each global of decisions declared differently in path: (decl edit, [(clean edit,
    neutral edit or None, function)], notes). Edits are (start, end, text)."""
    rel = os.path.relpath(path, ROOT)
    text = open(path, encoding="latin-1").read()
    ast = pi.load_ast(rel)
    pi.annotate(ast)
    out = {}
    notes = collections.defaultdict(list)

    decl_nodes = {}
    for n in ast.get("inner", []):
        if n.get("kind") == "VarDecl" and n.get("name") in decisions:
            f, b, m = pi.where(n["range"]["begin"])
            if f == rel and not m:
                decl_nodes.setdefault(n["name"], []).append(n)
    if not decl_nodes:
        return text, out, notes

    def src(n):
        f, b, m = pi.where(n["range"]["begin"])
        if m or f != rel:
            return None
        return text[b:pi.end_of(n["range"])]

    def rng(n):
        f, b, m = pi.where(n["range"]["begin"])
        if m or f != rel:
            return None
        return b, pi.end_of(n["range"])

    def up(n):
        """The parent, through parentheses and the implicit casts that keep a value."""
        p = n.get("_p")
        while p is not None and (p.get("kind") == "ParenExpr" or
                                 (p.get("kind") == "ImplicitCastExpr" and
                                  p.get("castKind") in ("ArrayToPointerDecay", "LValueToRValue",
                                                        "NoOp", "FunctionToPointerDecay"))):
            p = p.get("_p")
        return p

    def enclosing_function(n):
        while n is not None and n.get("kind") != "FunctionDecl":
            n = n.get("_p")
        return n.get("name") if n else None

    def is_lvalue_use(n):
        p = up(n)
        if p is None:
            return False
        k = p.get("kind")
        if k in ("BinaryOperator", "CompoundAssignOperator") and p.get("opcode", "").endswith("=") \
                and p.get("opcode") not in ("==", "!=", "<=", ">=") and pi.kids(p)[0] is not None:
            first = pi.kids(p)[0]
            while first.get("kind") in ("ParenExpr",):
                first = pi.kids(first)[0]
            return first is n
        if k == "UnaryOperator" and p.get("opcode") in ("++", "--", "&"):
            return True
        return False

    def index_of(e):
        """The element index of a byte offset that is a multiple of 4, as text; None if not."""
        e0 = e
        while e.get("kind") in ("ParenExpr", "ImplicitCastExpr"):
            e = pi.kids(e)[0]
        k = e.get("kind")
        if k == "IntegerLiteral":
            v = int(e.get("value", "0"))
            return str(v // 4) if v % 4 == 0 else None
        if k == "BinaryOperator":
            op = e.get("opcode")
            a, b = pi.kids(e)
            if op == "<<":
                bb = b
                while bb.get("kind") in ("ParenExpr", "ImplicitCastExpr"):
                    bb = pi.kids(bb)[0]
                if bb.get("kind") == "IntegerLiteral" and bb.get("value") == "2":
                    return src(a)
                return None
            if op == "*":
                for x, y in ((a, b), (b, a)):
                    yy = y
                    while yy.get("kind") in ("ParenExpr", "ImplicitCastExpr"):
                        yy = pi.kids(yy)[0]
                    if yy.get("kind") == "IntegerLiteral":
                        v = int(yy.get("value", "0"))
                        if v == 4:
                            return src(x)
                        if v % 4 == 0:
                            sx = src(x)
                            return None if sx is None else "%s * %d" % (sx, v // 4)
                return None
            if op == "+":
                ia, ib = index_of(a), index_of(b)
                if ia is None or ib is None:
                    return None
                return "%s + %s" % (ia, ib)
        return None

    def row_index(e, size):
        """X of `X * size` (a row's byte size)."""
        while e.get("kind") in ("ParenExpr", "ImplicitCastExpr"):
            e = pi.kids(e)[0]
        if e.get("kind") == "BinaryOperator" and e.get("opcode") == "*":
            for x, y in (pi.kids(e), pi.kids(e)[::-1]):
                while y.get("kind") in ("ParenExpr", "ImplicitCastExpr"):
                    y = pi.kids(y)[0]
                if y.get("kind") == "IntegerLiteral" and int(y.get("value", "0")) == size:
                    return src(x)
        return None

    def paren(s):
        return s if re.match(r"^[\w\.\->\[\]]+$", s) else "(%s)" % s

    for name, nodes in decl_nodes.items():
        d = decisions[name]
        T = d["type"]
        old = nodes[0]
        old_t = pi.dqt(old)
        if d["array"]:
            if not re.match(r"^(signed |unsigned )?char ?\[", old_t):
                continue
            newdecl = "extern %s%s%s[]%s;" % (T, "" if T.endswith("*") else " ", name,
                                              "[%d]" % d["row"] if d.get("row") else "")
        else:
            if not re.match(r"^(signed |unsigned )?char ?\[", old_t):
                continue
            newdecl = "extern %s%s%s;" % (T, "" if T.endswith("*") else " ", name)
        lines = text.split("\n")
        decl_edits = []
        for dn in nodes:
            f, b, m = pi.where(dn["range"]["begin"])
            ls = text.rfind("\n", 0, b) + 1
            le = text.find("\n", b)
            line = text[ls:le]
            mm = re.match(DECL_LINE % re.escape(name), line)
            if not mm:
                notes[name].append("declaration not on a line of its own")
                break
            # keep a trailing comment
            tail = line[line.index(";") + 1:]
            decl_edits.append((ls, le, newdecl + tail))
        if len(decl_edits) != len(nodes):
            continue
        uses = []
        ok = True
        ids = {dn["id"] for dn in nodes}

        def visit(n):
            nonlocal ok
            if n.get("kind") == "DeclRefExpr" and (n.get("referencedDecl") or {}).get("id") in ids:
                uses.append(n)
            for c in pi.kids(n):
                visit(c)
        visit(ast)
        conv = []
        for r in uses:
            fn = enclosing_function(r)
            if rng(r) is None:
                notes[name].append("a use in a macro (%s)" % fn)
                ok = False
                break
            p = up(r)
            pk = p.get("kind") if p else None
            done = False
            # *(X *)g
            if pk == "CStyleCastExpr" and pi.dqt(p).endswith("*"):
                X = pi.qt(p)[:-1].strip()
                q = up(p)
                if q is not None and q.get("kind") == "UnaryOperator" and q.get("opcode") == "*" and rng(q):
                    lv = is_lvalue_use(q)
                    if d["array"]:
                        el = "%s[0]" % name
                        clean = conv_elem(el, X, T, lv)
                        Xw = ("uptr" if norm(X).startswith("u") else "iptr") if is_int_type(X) else X
                        conv.append(((rng(q), clean), (rng(q), "*(%s *)&%s" % (Xw, el))
                                     if (is_int_type(X) or X.endswith("*")) else None, fn))
                    else:
                        clean = conv_scalar(name, X, T, lv)
                        neutral = "*(%s *)&%s" % ("iptr" if is_int_type(X) and not norm(X).startswith("u")
                                                  else "uptr" if is_int_type(X) else X, name)
                        conv.append(((rng(q), clean), (rng(q), neutral), fn))
                    done = True
                elif q is not None and q.get("kind") == "ArraySubscriptExpr" and d["array"] and rng(q):
                    # ((X *)g)[i]
                    idx = src(pi.kids(q)[1])
                    if idx is not None and (is_int_type(X) or X.endswith("*")):
                        conv.append(((rng(q), conv_elem("%s[%s]" % (name, idx), X, T, is_lvalue_use(q))),
                                     None, fn))
                        done = True
            # *(X *)(g + E)
            if not done and pk == "BinaryOperator" and p.get("opcode") == "+" and \
                    pi.kids(p)[0] is not None:
                lhs = pi.kids(p)[0]
                while lhs.get("kind") in ("ImplicitCastExpr", "ParenExpr"):
                    lhs = pi.kids(lhs)[0]
                # g + a + b ...: the offsets, left to right
                offs = [pi.kids(p)[1]]
                top = p
                while up(top) is not None and up(top).get("kind") == "BinaryOperator" and \
                        up(top).get("opcode") == "+":
                    nxt = up(top)
                    l2 = pi.kids(nxt)[0]
                    while l2.get("kind") in ("ImplicitCastExpr", "ParenExpr"):
                        l2 = pi.kids(l2)[0]
                    if l2 is not top:
                        break
                    offs.append(pi.kids(nxt)[1])
                    top = nxt
                c = up(top)
                q = up(c) if c is not None else None
                if lhs is r and c is not None and c.get("kind") == "CStyleCastExpr" and \
                        pi.dqt(c).endswith("*") and q is not None and q.get("kind") == "UnaryOperator" \
                        and q.get("opcode") == "*" and rng(q):
                    X = pi.qt(c)[:-1].strip()
                    E = pi.kids(p)[1]
                    if d["array"]:
                        row = d.get("row")
                        el = None
                        if row:
                            # a table of rows (g[][row]): the first offset picks the row
                            ri = row_index(offs[0], row * 4)
                            rest = [index_of(o) for o in offs[1:]]
                            if ri is not None and None not in rest:
                                el = "%s[%s][%s]" % (name, ri, " + ".join(rest) or "0")
                        else:
                            parts = [index_of(o) for o in offs]
                            if None not in parts:
                                el = "%s[%s]" % (name, " + ".join(parts))
                        if el is not None and (is_int_type(X) or X.endswith("*")):
                            Xw = ("uptr" if norm(X).startswith("u") else "iptr") if is_int_type(X) else X
                            conv.append(((rng(q), conv_elem(el, X, T, is_lvalue_use(q))),
                                         (rng(q), "*(%s *)&%s" % (Xw, el)), fn))
                            done = True
                        elif el is not None and norm(X) in ("short", "unsignedshort", "char",
                                                             "signedchar", "unsignedchar"):
                            # part of an entry: its low bytes (the entries are widened)
                            conv.append(((rng(q), "*(%s *)&%s" % (X, el)), None, fn))
                            done = True
                    elif len(offs) == 1:
                        es = src(E)
                        if es is not None:
                            neutral = "*(%s *)((char *)&%s + %s)" % (X, name, es)
                            conv.append(((rng(q), neutral), (rng(q), neutral), fn))
                            notes[name].append("reads inside the pointer at an offset (%s)" % fn)
                            done = True
            if not done:
                # the address
                if pk == "CStyleCastExpr" and pi.is_wide(p) or (pk == "CStyleCastExpr" and
                                                                 is_int_type(pi.dqt(p))):
                    rep = ("(char *)%s" % name) if d["array"] else "(char *)&%s" % name
                else:
                    rep = ("(char *)%s" % name) if d["array"] else "((char *)&%s)" % name
                conv.append(((rng(r), rep), None, fn))
                if d["array"] and pk == "BinaryOperator" and p.get("opcode") in ("+", "-"):
                    notes[name].append("the table's address with a byte offset (%s): a 4-byte "
                                       "stride" % fn)
                if not d["array"]:
                    notes[name].append("the pointer's own address used (%s)" % fn)
        if ok:
            out[name] = (decl_edits, conv)
    return text, out, notes


def conv_scalar(name, X, T, lvalue):
    """`*(X *)g` for a global now declared T (a pointer)."""
    if norm(X) == norm(T):
        return name
    if lvalue:
        return "*(%s *)&%s" % ("iptr" if is_int_type(X) and not norm(X).startswith("u") else
                               "uptr" if is_int_type(X) else X, name)
    if is_int_type(X):
        return "(%s)%s" % ("uptr" if norm(X).startswith("u") else "iptr", name)
    return "(%s)%s" % (X, name)


def conv_elem(el, X, T, lvalue):
    """an element `g[i]` read as X (the table's elements are T)."""
    if norm(X) == norm(T) or (is_int_type(X) and norm(T) in ("iptr", "uptr")):
        return el
    if lvalue:
        return "*(%s *)&%s" % ("iptr" if is_int_type(X) and not norm(X).startswith("u") else
                               "uptr" if is_int_type(X) else X, el)
    if is_int_type(X):
        return "(%s)%s" % ("uptr" if norm(X).startswith("u") else "iptr", el)
    return "(%s)%s" % (X, el)


def render(text, conv_by_global, chosen):
    """chosen: {name: 'clean' | 'neutral'}"""
    edits = []
    for name, mode in chosen.items():
        decl_edits, conv = conv_by_global[name]
        edits += decl_edits
        for clean, neutral, _fn in conv:
            e = neutral if (mode == "neutral" and neutral is not None) else clean
            edits.append((e[0][0], e[0][1], e[1]))
    edits.sort()
    out, pos = [], 0
    for s, e, t in edits:
        if s < pos:
            continue
        out.append(text[pos:s])
        out.append(t)
        pos = e
    out.append(text[pos:])
    return "".join(out)


def apply(files, decisions, base_dir, in_place, log=print):
    import structure
    import port_protos
    ver = structure.Verifier()
    tmp = tempfile.mkdtemp(prefix="pglob_", dir=os.environ.get("STRUCT_TMP"))
    work = {}
    for p in files:
        text, conv, notes = file_edits(p, decisions)
        if conv:
            work[p] = (text, conv, notes)
    order = sorted(work)
    rows = []

    def base_of(p):
        if base_dir:
            return open(os.path.join(base_dir, os.path.relpath(p, ROOT)), encoding="latin-1").read()
        return work[p][0]

    res = port_protos.check_many(ver, [(render(work[p][0], work[p][1], {g: "clean" for g in work[p][1]}),
                                         base_of(p)) for p in order], tmp)
    final = {}
    failing = []
    for p, (ok, bad) in zip(order, res):
        if ok:
            final[p] = {g: "clean" for g in work[p][1]}
        else:
            failing.append(p)
            log("%s: %s fail" % (os.path.relpath(p, ROOT), ",".join(bad)))
    for p in failing:
        text, conv, _n = work[p]
        names = sorted(conv)
        chosen = {}
        for g in names:
            for mode in ("clean", "neutral"):
                trial = dict(chosen)
                trial[g] = mode
                if port_protos.check_many(ver, [(render(text, conv, trial), base_of(p))], tmp)[0][0]:
                    chosen = trial
                    break
            else:
                log("%s: %s keeps its declaration" % (os.path.relpath(p, ROOT), g))
        final[p] = chosen
    for p in order:
        text, conv, notes = work[p]
        rel = os.path.relpath(p, ROOT)
        for g in sorted(conv):
            rows.append(dict(file=rel, glob=g, result=final[p].get(g, "kept old"),
                             notes="; ".join(sorted(set(notes.get(g, []))))))
        if in_place and final[p]:
            open(p, "w", encoding="latin-1").write(render(text, conv, final[p]))
    shutil.rmtree(tmp, ignore_errors=True)
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("cmd", choices=["plan", "tables", "apply"])
    ap.add_argument("files", nargs="*")
    ap.add_argument("--out")
    ap.add_argument("--plan")
    ap.add_argument("--narrow", help="plan: also the byte tables of this narrow_class list")
    ap.add_argument("--base")
    ap.add_argument("--in-place", action="store_true")
    ap.add_argument("--report")
    a = ap.parse_args()
    os.chdir(ROOT)
    if a.cmd == "plan":
        d = plan()
        if a.narrow:
            for k, v in narrow_tables(a.narrow).items():
                d.setdefault(k, v)
        s = json.dumps(d, indent=1, sort_keys=True)
        if a.out:
            open(a.out, "w").write(s)
        else:
            print(s)
        return
    if a.cmd == "tables":
        files = [os.path.join(ROOT, f) for f in game_files()]
        d, left = plan_tables(files)
        for k, v in left.items():
            print("left %-24s %s" % (k, v))
        s = json.dumps(d, indent=1, sort_keys=True)
        if a.out:
            open(a.out, "w").write(s)
        else:
            print(s)
        return
    decisions = json.load(open(a.plan))
    files = [os.path.abspath(f) for f in a.files] or [os.path.join(ROOT, f) for f in game_files()]
    rows = apply(files, decisions, a.base, a.in_place)
    print("%d conversions: %s" % (len(rows), dict(collections.Counter(r["result"] for r in rows))))
    if a.report:
        with open(a.report, "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=["file", "glob", "result", "notes"])
            w.writeheader()
            w.writerows(rows)


if __name__ == "__main__":
    main()
