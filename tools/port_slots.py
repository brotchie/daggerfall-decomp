#!/usr/bin/env python3
"""Frame slots the code reaches through a wider type than their declaration (docs/port.md).

Under Watcom every local and parameter has a 4-byte frame slot, so the original code wrote 4
bytes into a `short` safely (`*(int *)&x = v`) and read addresses out of one (`*(char **)&x`).
The matching build keeps the declared type (Watcom's frame layout follows the declarations,
tools/w10_frame.py), but natively a short is 2 bytes and the wider access runs over whatever
clang puts next to it. include/ptrint.h has the slot types: the declared type under Watcom,
as wide as the widest access natively (slot16: short / int; pslot16: short / iptr...).

  port_slots.py [FILES]          every access wider than its variable, natively:
                                 file:line: name (type, scope) <how> as T (size)
  port_slots.py --vars [FILES]   per variable: the slot type it needs, its wide accesses, the
                                 direct reads, and what needs a hand (`hand`: a write whose
                                 value is used, its address passed on, a parameter, a global)
  port_slots.py --apply [FILES]  for each local or parameter whose declaration is simple: the
                                 declaration becomes the slot type, and every direct read in a
                                 value context `x` becomes `(short)x` (its Watcom type: a no-op
                                 there, natively the low bits a word load gives) unless an
                                 explicit cast to 16 bits or fewer already narrows it. Writes
                                 stay. Left to a person, and listed:
                                 - a 4-byte access of a pointer-wide slot (one that holds an
                                   address becomes `*(iptr *)&x`; a BIOS address stored as a
                                   number becomes `(iptr)DOS_LOW(0x46C)`);
                                 - a parameter's declarations in other files;
                                 - the --vars `hand` items.

The AST is clang's JSON of the native build (port/include/port.h, DAGGER_PORT); a variable
declared with a slot type is as wide natively as its accesses, so it drops out of the list.
Not seen here: accesses that need neighbouring locals side by side (a 12-byte copy into three
ints: include/ptrint.h's VEC3_LOCALS_*), and pointer types whose pointee size is unknown.
"""
import argparse
import collections
import glob
import os
import re
import sys
from concurrent.futures import ThreadPoolExecutor

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from port_iptr import load_ast, annotate, where, kids, qt, dqt  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

SIZE = {"char": 1, "signed char": 1, "unsigned char": 1, "_Bool": 1, "short": 2,
        "unsigned short": 2, "int": 4, "unsigned int": 4, "float": 4, "long": 8,
        "unsigned long": 8, "double": 8, "long long": 8, "unsigned long long": 8}

# the slot type for a declared type, by the widest access natively (4: int, 8: pointer-wide)
SLOT = {("short", 4): "slot16", ("unsigned short", 4): "uslot16",
        ("short", 8): "pslot16", ("unsigned short", 8): "upslot16",
        ("signed char", 4): "slot8", ("unsigned char", 4): "uslot8"}


def game_files():
    return sorted(glob.glob("src/lifted/*.c") + glob.glob("src/hand/*.c") + glob.glob("src/*.c"))


TYPEDEFS = {}     # name -> its type natively (from the AST's TypedefDecls; every file agrees)


def size_of(t):
    t = t.replace("const ", "").replace("volatile ", "").strip()
    if t.endswith("*") or "(*)" in t:
        return 8
    m = re.match(r"^(.*?)\s*\[(\d+)\]$", t)
    if m:
        s = size_of(m.group(1))
        return s * int(m.group(2)) if s else None
    if t in SIZE:
        return SIZE[t]
    if t in TYPEDEFS and TYPEDEFS[t] != t:
        return size_of(TYPEDEFS[t])
    return None


def pointee(t):
    t = t.strip()
    return t[:-1].strip() if t.endswith("*") else None


def up(n):
    """the parent, past parentheses"""
    p = n["_p"]
    while p is not None and p.get("kind") == "ParenExpr":
        p = p["_p"]
    return p


def is_child0(p, n):
    """n (or the parens around it) is p's first operand"""
    c = kids(p)[0] if kids(p) else None
    while c is not None and c is not n and c.get("kind") == "ParenExpr":
        c = kids(c)[0]
    return c is n


STMT_PARENTS = {"CompoundStmt"}


def value_used(e):
    """does anything use the value of expression e (an assignment, ++, a compound assignment)?"""
    p = up(e)
    if p is None:
        return False
    k = p.get("kind")
    if k in STMT_PARENTS:
        return False
    if k in ("ForStmt",):
        # inner: init, condvar, cond, inc, body (null children as {})
        ks = kids(p)
        idx = next((i for i, c in enumerate(ks) if c is e or _under_parens(c, e)), None)
        return idx == 2
    if k in ("IfStmt", "WhileStmt", "DoStmt", "SwitchStmt"):
        # the statement bodies use nothing; the condition does
        ks = kids(p)
        idx = next((i for i, c in enumerate(ks) if c is e or _under_parens(c, e)), None)
        if k == "DoStmt":
            return idx == 1
        return idx == 0
    if k == "BinaryOperator" and p.get("opcode") == ",":
        return not is_child0(p, e) and value_used(p)
    if k == "CStyleCastExpr" and dqt(p) == "void":
        return False
    if k in ("LabelStmt", "CaseStmt", "DefaultStmt"):
        return False
    return True


def _under_parens(c, e):
    while c is not None and c.get("kind") == "ParenExpr":
        c = kids(c)[0]
    return c is e


class Var:
    def __init__(self, decl, scope, fn):
        self.decl = decl
        self.name = decl.get("name")
        self.type = dqt(decl)
        self.spelled = qt(decl)
        self.scope = scope        # local, static, param, global
        self.fn = fn
        self.wide = []            # (node, how, T, size)
        self.reads = []           # DeclRefExpr nodes read in a value context
        self.vwrites = []         # (node, how): writes whose value is used
        self.escapes = []         # (node, how): &x used some other way
        self.other = []           # (node, how)


def classify_addr(ref, v):
    """&x: through a cast to a wider pointee (a wide access), or something else"""
    a = up(ref)                       # the & operator
    c = up(a)
    while c is not None and c.get("kind") == "ImplicitCastExpr":
        c = up(c)
    if c is not None and c.get("kind") == "CStyleCastExpr":
        to = dqt(c)
        pt = pointee(to)
        sz = size_of(pt) if pt else None
        if sz is not None and sz > (size_of(v.type) or 99):
            d = up(c)
            k = d.get("kind") if d else None
            if k == "UnaryOperator" and d.get("opcode") == "*":
                e = up(d)
                ek = e.get("kind") if e else None
                if ek == "BinaryOperator" and e.get("opcode") == "=" and is_child0(e, d):
                    how = "store"
                elif ek == "CompoundAssignOperator" and is_child0(e, d):
                    how = "update"
                elif ek == "UnaryOperator" and e.get("opcode") in ("++", "--"):
                    how = "update"
                elif ek == "UnaryOperator" and e.get("opcode") == "&":
                    how = "address"
                elif ek == "ImplicitCastExpr" and e.get("castKind") == "LValueToRValue":
                    how = "load"
                else:
                    how = "deref"
            elif k == "MemberExpr":
                how = "member"
            elif k == "ArraySubscriptExpr":
                how = "index"
            elif k == "CallExpr":
                how = "argument"
            else:
                how = "cast"
            v.wide.append((c, how, pt, sz))
            return
        if sz is not None and sz <= (size_of(v.type) or 0):
            return                    # a narrower or same-size view: fine natively
    v.escapes.append((a, "&%s as %s" % (v.name, dqt(c) if c else "?")))


def analyze(path):
    ast = load_ast(path)
    annotate(ast)
    base = os.path.basename(path)
    vars_ = {}

    def mine(d):
        f, _o, _m = where(d.get("loc") or {})
        return f is not None and os.path.basename(f) == base

    def collect(n, fn):
        k = n.get("kind")
        if k == "FunctionDecl":
            fn = n
        if k in ("VarDecl", "ParmVarDecl") and "id" in n and mine(n):
            if k == "ParmVarDecl":
                scope = "param"
            elif fn is None:
                scope = "global"
            elif n.get("storageClass") in ("static", "extern"):
                scope = n.get("storageClass")
            else:
                scope = "local"
            vars_[n["id"]] = Var(n, scope, fn)
        for c in kids(n):
            collect(c, fn)

    for d in ast.get("inner", []):
        if d.get("kind") == "TypedefDecl" and d.get("name"):
            TYPEDEFS[d["name"]] = dqt(d)
        collect(d, None)

    refs = []

    def walk(n):
        if n.get("kind") == "DeclRefExpr":
            rd = n.get("referencedDecl") or {}
            if rd.get("id") in vars_:
                refs.append((n, vars_[rd["id"]]))
        for c in kids(n):
            walk(c)

    for d in ast.get("inner", []):
        if d.get("kind") == "FunctionDecl" and mine(d):
            walk(d)

    for ref, v in refs:
        p = up(ref)
        k = p.get("kind") if p else None
        if k == "UnaryOperator" and p.get("opcode") == "&":
            classify_addr(ref, v)
        elif k == "ImplicitCastExpr" and p.get("castKind") == "LValueToRValue":
            v.reads.append(ref)
        elif k == "BinaryOperator" and p.get("opcode") == "=" and is_child0(p, ref):
            if value_used(p):
                v.vwrites.append((p, "assignment used as a value"))
        elif k == "CompoundAssignOperator" and is_child0(p, ref):
            if value_used(p):
                v.vwrites.append((p, "compound assignment used as a value"))
        elif k == "UnaryOperator" and p.get("opcode") in ("++", "--"):
            if value_used(p):
                v.vwrites.append((p, "%s used as a value" % p.get("opcode")))
        elif k == "ImplicitCastExpr" and p.get("castKind") in ("ArrayToPointerDecay",
                                                               "FunctionToPointerDecay"):
            pass
        else:
            v.other.append((ref, k or "?"))
    return ast, vars_


def text_of(path):
    with open(os.path.join(ROOT, path), "rb") as f:
        return f.read().decode("latin-1")


def line_of(src, off):
    return src.count("\n", 0, off) + 1


def node_off(n):
    f, o, m = where((n.get("range") or {}).get("begin") or {})
    return f, o, m


def wide_vars(vars_):
    return [v for v in vars_.values() if v.wide]


def slot_for(v):
    widest = max(s for _c, _h, _t, s in v.wide)
    t = v.type
    return SLOT.get((t, 8 if widest > 4 else 4))


def cmd_list(files):
    def one(path):
        out = []
        try:
            _ast, vars_ = analyze(path)
        except Exception as e:  # noqa: BLE001
            return ["%s: error %s" % (path, e)]
        src = text_of(path)
        for v in wide_vars(vars_):
            for c, how, pt, sz in v.wide:
                f, o, m = node_off(c)
                ln = line_of(src, o) if f and os.path.basename(f) == os.path.basename(path) else "?"
                out.append("%s:%s: %s (%s, %s) %s as %s (%d)" % (path, ln, v.name, v.spelled,
                                                                v.scope, how, dqt(c), sz))
        return out
    with ThreadPoolExecutor(os.cpu_count()) as ex:
        for lines in ex.map(one, files):
            for line in lines:
                print(line)


def cmd_vars(files):
    def one(path):
        out = []
        _ast, vars_ = analyze(path)
        src = text_of(path)

        def L(n):
            f, o, _m = node_off(n)
            return line_of(src, o) if o is not None else "?"
        for v in wide_vars(vars_):
            slot = slot_for(v)
            fn = (v.fn or {}).get("name", "-")
            out.append("%s:%s: %s %s %s in %s -> %s" % (
                path, L(v.decl), v.scope, v.spelled, v.name, fn, slot or "?"))
            for c, how, pt, sz in v.wide:
                out.append("    wide  %s: %s as %s (%d)" % (L(c), how, dqt(c), sz))
            out.append("    reads %d" % len(v.reads))
            for n, how in v.vwrites:
                out.append("    hand  %s: %s" % (L(n), how))
            for n, how in v.escapes:
                out.append("    hand  %s: %s" % (L(n), how))
            for n, how in v.other:
                out.append("    hand  %s: used under %s" % (L(n), how))
            if v.scope != "local":
                out.append("    hand  a %s, not a local" % v.scope)
        return out
    with ThreadPoolExecutor(os.cpu_count()) as ex:
        for lines in ex.map(one, files):
            for line in lines:
                print(line)


CAST_KEEP = {"short", "unsigned short", "char", "signed char", "unsigned char"}


def apply_file(path):
    """the declaration and read edits for one file; returns (n_vars, n_reads, notes)"""
    _ast, vars_ = analyze(path)
    src = text_of(path)
    edits = []          # (start, end, text)
    notes = []
    nv = nr = 0
    decl_starts = collections.Counter()
    for v in vars_.values():
        f, o, m = node_off(v.decl)
        decl_starts[o] += 1
    for v in wide_vars(vars_):
        slot = slot_for(v)
        if v.scope not in ("local", "param") or slot is None:
            notes.append("%s:%d: %s: %s %s, left" % (path, line_of(src, node_off(v.decl)[1]),
                                                      v.name, v.scope, v.spelled))
            continue
        f, o, m = node_off(v.decl)
        nf, no, nm = where(v.decl.get("loc") or {})
        if m or nm or decl_starts[o] > 1:
            notes.append("%s:%d: %s: declaration not simple, left" % (path, line_of(src, o), v.name))
            continue
        spelled = src[o:no]
        if re.sub(r"\s+", " ", spelled.strip()) != v.type:
            notes.append("%s:%d: %s: declared as %r, left" % (path, line_of(src, o), v.name, spelled))
            continue
        edits.append((o, no, slot + " "))
        nv += 1
        for r in v.reads:
            rf, ro, rm = node_off(r)
            if rm or rf is None or os.path.basename(rf) != os.path.basename(path):
                notes.append("%s:%d: %s: read in a macro, left" % (path, line_of(src, ro or 0), v.name))
                continue
            # the read's value already narrowed by an explicit cast: nothing to do
            p = up(r["_p"])
            if p is not None and p.get("kind") == "CStyleCastExpr" and dqt(p) in CAST_KEEP:
                continue
            edits.append((ro, ro, "(%s)" % v.type))
            nr += 1
        for n, how in v.vwrites:
            notes.append("%s:%d: %s: %s" % (path, line_of(src, node_off(n)[1]), v.name, how))
        for n, how in v.escapes:
            notes.append("%s:%d: %s: %s" % (path, line_of(src, node_off(n)[1]), v.name, how))
        for n, how in v.other:
            notes.append("%s:%d: %s: used under %s" % (path, line_of(src, node_off(n)[1]), v.name, how))
        if slot.startswith(("pslot", "upslot")):
            for c, how, pt, sz in v.wide:
                if sz == 4:
                    notes.append("%s:%d: %s: pointer-wide slot, 4-byte %s" % (
                        path, line_of(src, node_off(c)[1]), v.name, how))
    if edits:
        edits.sort(key=lambda e: (e[0], e[1]), reverse=True)
        for s, e, t in edits:
            src = src[:s] + t + src[e:]
        with open(os.path.join(ROOT, path), "wb") as f:
            f.write(src.encode("latin-1"))
    return nv, nr, notes


def cmd_apply(files):
    tv = tr = 0
    for path in files:
        nv, nr, notes = apply_file(path)
        tv += nv
        tr += nr
        if nv:
            print("%s: %d variables, %d reads" % (path, nv, nr))
        for n in notes:
            print("  " + n)
    print("total: %d variables, %d reads" % (tv, tr))


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("files", nargs="*")
    ap.add_argument("--vars", action="store_true")
    ap.add_argument("--apply", action="store_true")
    a = ap.parse_args()
    os.chdir(ROOT)
    files = a.files or game_files()
    if a.apply:
        cmd_apply(files)
    elif a.vars:
        cmd_vars(files)
    else:
        cmd_list(files)


if __name__ == "__main__":
    main()
