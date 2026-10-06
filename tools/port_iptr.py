#!/usr/bin/env python3
"""The 64-bit pass (docs/port.md, phase 3): give every integer that holds an address a
pointer-wide type, driven by clang's AST of the native build.

The game keeps addresses in ints. Under Watcom an int is as wide as a pointer; natively a
pointer is 64 bits. include/ptrint.h defines `iptr` (`uptr` unsigned): `int` under Watcom, so
FALL.EXE cannot change, and `long` in the native build. This tool:
  - rewrites `(int)ptr` / `(unsigned)ptr` casts as `(iptr)` / `(uptr)`;
  - follows each value that holds an address to where it is kept, and retypes that place
    iptr/uptr: a local, a parameter, a global, a struct field, a function's return. A
    parameter, return or global is retyped in every file that declares it (each file declares
    what it uses);
  - finds the places that hold addresses from the other side as well: an int cast to a
    pointer, compared with one, or passed where a pointer is wanted;
  - writes a pointer difference kept in an int as `(int)(p - q)`: an offset, not an address.
It repeats until nothing changes (a fixed point). Everything it writes is the same code under
Watcom (iptr is int there), so the matching build cannot change; tools/build-and-verify.sh
checks it anyway.

  port_iptr.py run [--in-place] [--max-iter N] [--log FILE] [FILES]
  port_iptr.py show FILE          what one file's AST asks for (no changes)

Places it cannot retype are logged (`residue`): an address kept in a short or a char, an
address stored through a cast to `int *` (a 4-byte slot that natively needs 8: those are
retyped `(iptr *)` and listed, since the slot's layout changes), calls through function
pointers, initialiser lists.
"""
import argparse
import collections
import json
import os
import re
import subprocess
import sys
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CLANG_FLAGS = ["-fsyntax-only", "-std=gnu89", "-include", "port/include/port.h",
               "-funsigned-char", "-fwrapv", "-fno-strict-aliasing", "-Iport/include",
               "-Iinclude", "-Wno-everything", "-ferror-limit=0", "-Xclang", "-ast-dump=json"]

WIDE = {"long", "unsigned long", "iptr", "uptr", "long int", "unsigned long int"}
NARROW_INT = {"int", "unsigned int", "unsigned"}
SHORTISH = {"short", "unsigned short", "char", "signed char", "unsigned char"}
CMP_OPS = {"==", "!=", "<", ">", "<=", ">="}


def game_files():
    import glob
    return sorted(glob.glob("src/lifted/*.c") + glob.glob("src/hand/*.c") + glob.glob("src/*.c"))


def editable(path):
    """Files this pass may change: the game's C and its headers."""
    return path.startswith(("src/lifted/", "src/hand/", "include/")) or \
        (path.startswith("src/") and path.count("/") == 1)


# ---- the AST ----------------------------------------------------------------------------

def load_ast(path):
    r = subprocess.run(["clang"] + CLANG_FLAGS + [path], capture_output=True, cwd=ROOT)
    if not r.stdout:
        raise RuntimeError("clang failed on %s: %s" % (path, r.stderr.decode()[:400]))
    return json.loads(r.stdout)


def annotate(root):
    """Give every location dict its file (the JSON names a file only when it changes, in
    document order), and every node its parent."""
    cur = [None]

    def loc(d):
        if "file" in d:
            cur[0] = d["file"]
        d["_file"] = cur[0]

    def visit(n, parent):
        n["_p"] = parent
        for key in ("loc", "range"):
            v = n.get(key)
            if not v:
                continue
            if key == "range":
                for side in ("begin", "end"):
                    s = v.get(side)
                    if s:
                        visit_loc(s)
            else:
                visit_loc(v)
        for k, v in n.items():
            if k in ("loc", "range", "_p", "inner"):
                continue
            if isinstance(v, dict) and "offset" in v:
                visit_loc(v)
        for c in n.get("inner", ()):
            visit(c, n)

    def visit_loc(d):
        if "spellingLoc" in d or "expansionLoc" in d:
            for k in ("spellingLoc", "expansionLoc"):
                if k in d:
                    loc(d[k])
            return
        if "offset" in d:
            loc(d)

    visit(root, None)


def where(locd):
    """(file, offset, in_macro) of a location dict: the expansion for a macro, the spelling
    when the macro body is the place to edit is decided by the caller."""
    if not locd:
        return None, None, False
    if "expansionLoc" in locd:
        e = locd["expansionLoc"]
        return e.get("_file"), e.get("offset"), True
    return locd.get("_file"), locd.get("offset"), False


def spelling(locd):
    if not locd:
        return None, None
    if "spellingLoc" in locd:
        s = locd["spellingLoc"]
        return s.get("_file"), s.get("offset")
    return locd.get("_file"), locd.get("offset")


def qt(n):
    t = n.get("type") or {}
    return t.get("qualType", "")


def dqt(n):
    t = n.get("type") or {}
    return t.get("desugaredQualType", t.get("qualType", ""))


def is_wide(n):
    t = dqt(n)
    return t in WIDE


def is_ptr(n):
    """A pointer, or an array (an address)."""
    t = dqt(n)
    return t.endswith("*") or t.endswith("]") or "(*)" in t


def is_narrow_int(n):
    return dqt(n) in NARROW_INT


def kids(n):
    return n.get("inner", [])


def strip(n):
    """Parentheses and the implicit casts that keep a value as it is."""
    while n is not None:
        k = n.get("kind")
        if k == "ParenExpr":
            n = kids(n)[0]
        elif k == "ImplicitCastExpr" and n.get("castKind") in ("LValueToRValue", "NoOp"):
            n = kids(n)[0]
        else:
            return n
    return n


def end_of(rng):
    f, o, _m = where(rng["end"])
    e = rng["end"]
    if "expansionLoc" in e:
        e = e["expansionLoc"]
    return o + e.get("tokLen", 0)


# ---- one file -----------------------------------------------------------------------------

class FileFacts:
    def __init__(self, path):
        self.path = path
        self.sites = []       # (ident, file, begin, loc_off, end, qualtype, group)
        self.wants = set()    # (ident, mode)  mode: 'v' value, 'p' pointee
        self.edits = []       # (file, start, end, text, why)
        self.residue = []     # (file:line, text)
        self.slots = []       # (file:line, text)
        self.why = {}         # want -> the seed that asked for it
        self.puns = []        # (file:line, text): pointer declarations of int globals


def ident_of_decl(d, fn_stack):
    k = d.get("kind")
    p = d.get("_p")
    f, off, _m = where(d.get("loc"))
    if k == "FunctionDecl":
        return ("f", d.get("name"))
    if k == "ParmVarDecl":
        if p is not None and p.get("kind") == "FunctionDecl":
            ps = [c for c in kids(p) if c.get("kind") == "ParmVarDecl"]
            idx = next(i for i, c in enumerate(ps) if c is d)
            return ("p", p.get("name"), idx)
        return None
    if k == "VarDecl":
        if p is not None and p.get("kind") == "TranslationUnitDecl":
            if d.get("storageClass") == "static":
                return ("s", f, d.get("name"))
            return ("g", d.get("name"))
        return ("l", f, off)
    if k == "FieldDecl":
        return ("fd", f, off)
    return None


# library functions whose int result is never an address
INT_LIBC = {"atoi", "strlen", "abs", "rand", "stricmp", "strcmp", "strncmp", "strnicmp",
            "memcmp", "tolower", "toupper", "filelength", "read", "write", "lseek", "open",
            "close", "unlink", "port_read", "port_write", "port_lseek", "port_open",
            "port_close", "port_unlink", "port_filelength", "port_rand", "mc_sprintf",
            "printf", "fprintf", "port_fprintf"}


# fields that hold an id, never an address: a save or a link keeps a record's id where a
# pointer to it goes (`arg->object = (struct record *)arg->object->id`)
NEVER_FIELDS = {"id"}
# ints the lifter reads as pointers for the arithmetic (`*(char **)&hp_cost + k`): numbers
NOT_ADDR = {"hp_cost"}


def int_globals_scan(files):
    """The globals some file declares as a plain int (`extern int g;`, `extern unsigned g[];`)."""
    out = set()
    pat = re.compile(r"^extern\s+(?:unsigned\s+int|unsigned|int|signed\s+int|long|unsigned\s+long)\s+"
                     r"(\w+)\s*(?:\[[^\]]*\])?\s*;", re.M)
    for f in files + ["include/dagger.h", "include/records.h", "include/structs.h"]:
        out |= set(pat.findall(open(os.path.join(ROOT, f), encoding="latin-1").read()))
    return out


def analyze(path, final=False, int_globals=frozenset()):
    """The facts one translation unit gives: the declarations it holds, the places that must
    hold addresses, and the text edits (casts) it needs. With `final`, also the lifter's
    integer arithmetic written as pointer arithmetic (is_lea) whose base nothing showed to be
    an address: it stays int, with explicit casts."""
    ast = load_ast(path)
    annotate(ast)
    text_cache = {}

    def text(f):
        if f not in text_cache:
            text_cache[f] = open(os.path.join(ROOT, f), encoding="latin-1").read()
        return text_cache[f]

    def lineof(f, off):
        if f is None or off is None:
            return "%s:?" % path
        return "%s:%d" % (f, text(f).count("\n", 0, off) + 1)

    ff = FileFacts(path)
    decls = {}

    # pass 1: declarations
    def collect(n):
        k = n.get("kind")
        if k in ("FunctionDecl", "ParmVarDecl", "VarDecl", "FieldDecl"):
            decls[n["id"]] = n
            f, b, mb = where((n.get("range") or {}).get("begin"))
            lf, lo, ml = where(n.get("loc"))
            if f and editable(f) and not mb and not ml and lo is not None:
                ident = ident_of_decl(n, None)
                if ident is not None:
                    ff.sites.append((ident, f, b, lo, end_of(n["range"]), dqt(n), qt(n)))
        for c in kids(n):
            collect(c)
    collect(ast)

    def decl_of(ref):
        if ref is None:
            return None
        return decls.get(ref.get("id"))

    def note(lst, n, msg):
        f, o, _m = where((n.get("range") or {}).get("begin"))
        lst.append((lineof(f, o), msg))

    seed = [""]
    lea_inner = set()

    def add_want(w):
        ff.wants.add(w)
        ff.why.setdefault(w, seed[0])

    def want(d, mode, n):
        if d is None:
            note(ff.residue, n, "no declaration")
            return
        ident = ident_of_decl(d, None)
        if ident is None:
            note(ff.residue, n, "declaration without identity (%s)" % d.get("kind"))
            return
        add_want((ident, mode))

    def enclosing_function(n):
        while n is not None and n.get("kind") != "FunctionDecl":
            n = n.get("_p")
        return n

    def callee_decl(call):
        c = strip(kids(call)[0])
        while c is not None and c.get("kind") == "ImplicitCastExpr":
            c = kids(c)[0]
        if c is not None and c.get("kind") == "DeclRefExpr":
            d = decl_of(c.get("referencedDecl"))
            if d is not None and d.get("kind") == "FunctionDecl":
                return d
        return None

    def slot_cast(cast, why):
        """`*(int *)x = addr`: the slot's cast becomes `(iptr *)`."""
        f, o, m = where(cast["range"]["begin"])
        if m:
            f, o = spelling(cast["range"]["begin"])
        if not f or not editable(f):
            note(ff.residue, cast, "slot cast outside the game's files")
            return
        t = text(f)
        mm = re.match(r"\(\s*(unsigned int|unsigned|int|signed int|long)\s*\*\s*\)", t[o:o + 40])
        if not mm:
            note(ff.residue, cast, "slot cast not (int *): %r" % t[o:o + 30])
            return
        new = "(uptr *)" if mm.group(1).startswith("unsigned") else "(iptr *)"
        ff.edits.append((f, o, o + mm.end(), new, "slot"))
        ff.slots.append((lineof(f, o), why))

    def lvalue(n, why):
        """The place an lvalue names, retyped to hold an address."""
        n = strip(n)
        if n is None:
            return
        k = n.get("kind")
        if k == "DeclRefExpr":
            d = decl_of(n.get("referencedDecl"))
            if d is not None and d.get("kind") == "FunctionDecl":
                return
            if is_ptr(n):
                return
            want(d, "v", n)
        elif k == "MemberExpr":
            d = decls.get(n.get("referencedMemberDecl"))
            if d is None:
                note(ff.residue, n, "member without declaration")
                return
            if d.get("name") in NEVER_FIELDS:
                note(ff.residue, n, "id kept in a pointer (%s)" % d.get("name"))
                return
            want(d, "v", n)
        elif k == "ArraySubscriptExpr":
            base = kids(n)[0]
            b = base
            while b.get("kind") in ("ImplicitCastExpr", "ParenExpr"):
                b = kids(b)[0]
            bk = b.get("kind")
            if bk == "DeclRefExpr":
                d = decl_of(b.get("referencedDecl"))
                want(d, "v" if dqt(b).endswith("]") else "p", n)
            elif bk == "MemberExpr":
                d = decls.get(b.get("referencedMemberDecl"))
                if d is None:
                    note(ff.residue, n, "member without declaration")
                else:
                    want(d, "v" if dqt(b).endswith("]") else "p", n)
            elif bk == "CStyleCastExpr":
                slot_cast(b, why)
            else:
                note(ff.residue, n, "element of %s" % bk)
        elif k == "UnaryOperator" and n.get("opcode") == "*":
            o = strip(kids(n)[0])
            while o is not None and o.get("kind") == "ImplicitCastExpr":
                o = strip(kids(o)[0])
            ok = o.get("kind") if o else None
            if ok == "CStyleCastExpr":
                slot_cast(o, why)
            elif ok == "DeclRefExpr":
                want(decl_of(o.get("referencedDecl")), "p", n)
            elif ok == "MemberExpr":
                want(decls.get(o.get("referencedMemberDecl")), "p", n)
            elif ok == "BinaryOperator" and o.get("opcode") in ("+", "-"):
                a = strip(kids(o)[0])
                if a is not None and is_ptr(a):
                    lvalue({"kind": "UnaryOperator", "opcode": "*", "inner": [a],
                            "range": n["range"]}, why)
                else:
                    note(ff.residue, n, "store through pointer arithmetic")
            else:
                note(ff.residue, n, "store through %s" % ok)
        elif k in ("UnaryOperator",) and n.get("opcode") in ("++", "--"):
            lvalue(kids(n)[0], why)
        else:
            note(ff.residue, n, "lvalue %s" % k)

    def is_const(n):
        n = strip(n)
        while n is not None and n.get("kind") in ("ImplicitCastExpr", "CStyleCastExpr") and \
                not is_ptr(n):
            n = strip(kids(n)[0])
        return n is not None and n.get("kind") in ("IntegerLiteral", "UnaryExprOrTypeTraitExpr",
                                                    "CharacterLiteral")

    def cands(n):
        """Where an int expression could have its address from: ('ok', []) when it is
        pointer-wide already, ('addr', places) for the places (lvalues, calls) to retype,
        ('none', []) when it cannot be an address (a constant, a product, a shift, a byte or
        a short)."""
        n = strip(n)
        if n is None:
            return "none", []
        if is_wide(n) or is_ptr(n):
            return "ok", []
        if dqt(n) in SHORTISH:
            return "none", []
        k = n.get("kind")
        if k in ("DeclRefExpr", "MemberExpr", "ArraySubscriptExpr"):
            return "addr", [("lv", n)]
        if k == "UnaryOperator":
            op = n.get("opcode")
            if op == "*":
                return "addr", [("lv", n)]
            if op in ("++", "--"):
                return "addr", [("lv", kids(n)[0])]
            return "none", []
        if k == "CallExpr":
            d = callee_decl(n)
            if d is not None and d.get("name") in INT_LIBC:
                return "none", []
            return "addr", [("call", n)]
        if k == "BinaryOperator":
            op = n.get("opcode")
            a, b = kids(n)[0], kids(n)[1]
            if op == "=":
                return "addr", [("lv", a)]
            if op == ",":
                return cands(b)
            if op in ("+", "|", "^"):
                sa, la = cands(a)
                sb, lb = cands(b)
                if "ok" in (sa, sb):
                    return "ok", []          # an address plus an offset
                if sa == "addr" and sb == "addr" and all(x[0] == "call" for x in la) and \
                        not all(x[0] == "call" for x in lb):
                    return "addr", lb
                if sa == "addr":
                    return "addr", la
                if sb == "addr":
                    return "addr", lb
                return "none", []
            if op in ("-", "&"):
                sa, la = cands(a)
                if sa != "addr" and op == "-":
                    sb, _lb = cands(b)
                    if sb == "ok":
                        return "none", []    # an int minus an address: not one
                return sa, la
            return "none", []
        if k == "CompoundAssignOperator":
            return "addr", [("lv", kids(n)[0])]
        if k == "ConditionalOperator":
            s1, l1 = cands(kids(n)[1])
            s2, l2 = cands(kids(n)[2])
            if "addr" in (s1, s2):
                return "addr", l1 + l2
            return "none", []
        if k in ("CStyleCastExpr", "ImplicitCastExpr"):
            c = strip(kids(n)[0])
            if n.get("castKind") == "PointerToIntegral":
                return "ok", []          # (int)ptr: the cast becomes (iptr)
            if c is None or is_ptr(c) or is_wide(c) or dqt(c) in SHORTISH:
                return "none", []
            return cands(c)
        return "none", []

    def follow(lst, why):
        for kind, x in lst:
            if kind == "lv":
                lvalue(x, why)
            else:
                d = callee_decl(x)
                if d is None:
                    note(ff.residue, x, "address returned through a function pointer")
                else:
                    add_want((("f", d.get("name")), "r"))

    def source(n, why):
        """An int expression that must hold an address: retype what it reads."""
        st, lst = cands(n)
        if st == "addr":
            follow(lst, why)
        return st

    def ptr_base(n):
        """The pointer an address expression starts from: through casts, `&*`, `p + k`."""
        while n is not None:
            k = n.get("kind")
            if k in ("ParenExpr", "ImplicitCastExpr"):
                n = kids(n)[0]
            elif k == "CStyleCastExpr" and n.get("castKind") in ("BitCast", "NoOp"):
                n = kids(n)[0]
            elif k == "UnaryOperator" and n.get("opcode") == "&":
                c = strip(kids(n)[0])
                if c is not None and c.get("kind") == "UnaryOperator" and c.get("opcode") == "*":
                    n = kids(c)[0]
                else:
                    return n
            elif k == "BinaryOperator" and n.get("opcode") in ("+", "-"):
                a, b = kids(n)
                if is_ptr(a):
                    n = a
                elif is_ptr(b):
                    n = b
                else:
                    return n
            else:
                return n
        return n

    def lea_base(cast):
        """`(int)&*(char *)((char *)(y0) + 2)` is integer arithmetic the lifter wrote as pointer
        arithmetic (Watcom's lea): an int made a pointer, offset, and made an int again, never
        read through. Returns the inner int-to-pointer cast, or None."""
        b = ptr_base(kids(cast)[0])
        if b is not None and b.get("kind") == "DeclRefExpr":
            # a global this file declares as a pointer for the arithmetic, that other files
            # declare as an int (`extern char *game_minutes;`): a pun
            d = decl_of(b.get("referencedDecl"))
            if d is not None and d.get("kind") == "VarDecl" and \
                    (d.get("_p") or {}).get("kind") == "TranslationUnitDecl" and \
                    d.get("name") in int_globals and dqt(d).endswith("*"):
                note(ff.puns, b, "%s declared %s here, an int elsewhere" % (d.get("name"), dqt(d)))
                return b
        if b is not None and b.get("kind") == "UnaryOperator" and b.get("opcode") == "*":
            pl = punned_load(b)
            if pl is None:
                return None
            x = strip(pl)
            if x is not None and x.get("kind") == "UnaryOperator" and x.get("opcode") == "&":
                y = strip(kids(x)[0])
                if y is not None and place_name(y) in NOT_ADDR:
                    note(ff.puns, b, "%s (a number) read as a pointer" % place_name(y))
                    return b
                return None
            q = ptr_base(pl)
            if q is not None and re.match(r"^struct \w+ \*$", dqt(q)) and \
                    strip(pl).get("kind") in ("BinaryOperator",):
                note(ff.puns, b, "a field of %s read as a pointer at a raw offset" % dqt(q))
                return b
            return None
        if b is None or b.get("kind") != "CStyleCastExpr" or b.get("castKind") != "IntegralToPointer":
            return None
        if is_wide(strip(kids(b)[0])):
            return None
        return b

    def punned_load(n):
        """`*(char **)p`: memory read as a pointer through a cast; returns p."""
        o = kids(n)[0]
        while o is not None and o.get("kind") in ("ParenExpr", "ImplicitCastExpr"):
            o = kids(o)[0]
        if o is not None and o.get("kind") == "CStyleCastExpr" and o.get("castKind") == "BitCast" \
                and re.sub(r"\s+", "", dqt(o)).endswith("**"):
            return kids(o)[0]
        return None

    def place_name(n):
        if n.get("kind") == "DeclRefExpr":
            return (n.get("referencedDecl") or {}).get("name")
        if n.get("kind") == "MemberExpr":
            return n.get("name")
        return None

    def insert_after_cast(cast, word, why):
        """`(T *)(x)` -> `(T *)(iptr)(x)`."""
        f, o, m = where(cast["range"]["begin"])
        if m or not f or not editable(f):
            note(ff.residue, cast, "cast inside a macro")
            return
        t = text(f)
        c = strip(kids(cast)[0])
        cf, co, cm = where(kids(cast)[0]["range"]["begin"])
        if cm or co is None or co <= o:
            note(ff.residue, cast, "cast operand position")
            return
        ff.edits.append((f, co, co, word, why))

    def small_const(n):
        n = strip(n)
        while n is not None and n.get("kind") == "ImplicitCastExpr":
            n = strip(kids(n)[0])
        if n is None or n.get("kind") != "IntegerLiteral":
            return False
        try:
            return abs(int(n.get("value", "0"))) < 0x10000
        except ValueError:
            return False

    def num_op(n):
        """A wide operation whose result is a number, not an address: the distance between
        two addresses, a product, a quotient, a shift, a low-bits mask, a sizeof."""
        k = n.get("kind")
        if k == "UnaryExprOrTypeTraitExpr":
            return True
        if k == "UnaryOperator" and n.get("opcode") in ("-", "~"):
            return True
        if k != "BinaryOperator":
            return False
        op = n.get("opcode")
        a, b = kids(n)
        if op == "-":
            return "addr" in classify(a) and "addr" in classify(b)
        if op in ("*", "/", "%", "<<", ">>"):
            return True
        if op == "&":
            return small_const(a) or small_const(b)
        return False

    def classify(n):
        """What a wide expression is made of: 'addr' (an address) or 'num' (a number made
        from addresses: a distance, a product...)."""
        n = strip(n)
        if n is None:
            return set()
        if is_ptr(n):
            return {"addr"}
        if not is_wide(n):
            return set()
        k = n.get("kind")
        if num_op(n):
            return {"num"}
        if k == "BinaryOperator":
            op = n.get("opcode")
            a, b = kids(n)
            if op in CMP_OPS or op in ("&&", "||"):
                return set()
            if op == ",":
                return classify(b)
            return classify(a) | classify(b)
        if k == "ConditionalOperator":
            return classify(kids(n)[1]) | classify(kids(n)[2])
        if k == "ImplicitCastExpr":
            return classify(kids(n)[0])
        if k == "UnaryOperator" and n.get("opcode") == "+":
            return classify(kids(n)[0])
        return {"addr"}

    def wrap_nums(n):
        """`(int)(p - q)` around each number made from addresses inside a narrowed
        expression."""
        s = strip(n)
        if s is None or not (is_wide(s) or is_ptr(s)):
            return
        if num_op(s):
            tgt = s
            p = tgt.get("_p")
            while p is not None and p.get("kind") == "ParenExpr":
                tgt, p = p, p.get("_p")
            f, b, m = where(tgt["range"]["begin"])
            if m or not f or not editable(f):
                note(ff.residue, s, "number from addresses inside a macro")
                return
            e = end_of(tgt["range"])
            if tgt.get("kind") == "ParenExpr" or s.get("kind") == "UnaryExprOrTypeTraitExpr":
                ff.edits.append((f, b, b, "(int)", "num"))
            else:
                ff.edits.append((f, b, b, "(int)(", "num"))
                ff.edits.append((f, e, e, ")", "num"))
            return
        for c in kids(s):
            if isinstance(c, dict) and c.get("kind") not in ("CStyleCastExpr",):
                wrap_nums(c)

    def narrowed(n, inner, dest):
        """A wide value narrowed to 32 bits at n (`inner` is the wide expression); `dest`
        retypes the place it goes to."""
        cls = classify(inner)
        if "addr" in cls:
            dest()
        elif cls:
            wrap_nums(inner)

    def dest_of_narrowing(n):
        p = n.get("_p")
        while p is not None and p.get("kind") == "ParenExpr":
            n, p = p, p.get("_p")
        if p is None:
            return
        pk = p.get("kind")
        if pk == "BinaryOperator" and p.get("opcode") == "=" and kids(p)[1] is n:
            lvalue(kids(p)[0], "store")
        elif pk == "VarDecl":
            want(p, "v", n)
        elif pk == "CallExpr":
            idx = next(i for i, c in enumerate(kids(p)) if c is n) - 1
            d = callee_decl(p)
            if d is None:
                note(ff.residue, n, "argument to a function pointer")
            else:
                add_want((("p", d.get("name"), idx), "v"))
        elif pk == "ReturnStmt":
            fn = enclosing_function(p)
            add_want((("f", fn.get("name")), "r"))
        elif pk == "ConditionalOperator":
            dest_of_narrowing(p)
        else:
            note(ff.residue, n, "narrowed in %s" % pk)

    def visit(n):
        k = n.get("kind")
        if k and (k.endswith("Expr") or k.endswith("Operator")):
            f, o, _m = where((n.get("range") or {}).get("begin"))
            if f and o is not None:
                seed[0] = "%s %s" % (lineof(f, o), k)
        if k == "CStyleCastExpr" and n.get("castKind") == "PointerToIntegral":
            if dqt(n) in NARROW_INT:
                f, o, m = where(n["range"]["begin"])
                if m:
                    f, o = spelling(n["range"]["begin"])
                if f and editable(f):
                    t = text(f)
                    mm = re.match(r"\(\s*(unsigned int|unsigned|int|signed int|signed)\s*\)", t[o:o + 20])
                    if mm:
                        w = "uptr" if mm.group(1).startswith("unsigned") else "iptr"
                        inner = lea_base(n)
                        if inner is not None:
                            lea_inner.add(id(inner))
                            if final:
                                ff.edits.append((f, o, o + mm.end(), "(%s)(%s)" % (mm.group(1), w), "lea"))
                        else:
                            ff.edits.append((f, o, o + mm.end(), "(%s)" % w, "cast"))
                    else:
                        note(ff.residue, n, "pointer cast not spelled (int): %r" % t[o:o + 20])
        elif k in ("CStyleCastExpr", "ImplicitCastExpr") and n.get("castKind") == "IntegralToPointer":
            c = kids(n)[0]
            if id(n) in lea_inner:
                if final and not is_const(c):
                    insert_after_cast(n, "(iptr)", "lea")
            elif not is_wide(strip(c)) and not is_const(c):
                st = source(c, "to pointer")
                if st == "none":
                    if k == "CStyleCastExpr":
                        if final:
                            insert_after_cast(n, "(iptr)", "int")
                    else:
                        note(ff.residue, n, "int that is no address used as a pointer")
                elif st == "ok" and k == "ImplicitCastExpr":
                    note(ff.residue, n, "pointer cast to int, then used as a pointer")
        elif k == "ImplicitCastExpr" and n.get("castKind") == "IntegralCast" and \
                dqt(n) in NARROW_INT and is_wide(kids(n)[0]):
            inner = kids(n)[0]
            narrowed(n, inner, lambda: dest_of_narrowing(n))
        elif k == "ImplicitCastExpr" and n.get("castKind") == "PointerToIntegral" and \
                dqt(n) in NARROW_INT:
            dest_of_narrowing(n)
        elif k == "UnaryOperator" and n.get("opcode") == "*" and punned_load(n) is not None:
            x = strip(punned_load(n))
            if x is not None and x.get("kind") == "UnaryOperator" and x.get("opcode") == "&":
                y = strip(kids(x)[0])
                if y is not None and dqt(y) in NARROW_INT and place_name(y) not in NOT_ADDR:
                    lvalue(y, "read as a pointer")
        elif k == "CompoundAssignOperator":
            a, b = kids(n)
            if dqt(a) in NARROW_INT and (is_wide(b) or is_ptr(b)):
                narrowed(n, b, lambda: lvalue(a, "update"))
        elif k == "BinaryOperator" and n.get("opcode") in CMP_OPS:
            # an address compared with an int: the int holds an address too
            a, b = kids(n)
            for x, y in ((a, b), (b, a)):
                if y.get("kind") == "ImplicitCastExpr" and y.get("castKind") == "IntegralCast" and \
                        is_wide(y) and dqt(kids(y)[0]) in NARROW_INT and classify(x) == {"addr"}:
                    z = strip(kids(y)[0])
                    if z is not None and z.get("kind") in ("DeclRefExpr", "MemberExpr",
                                                           "ArraySubscriptExpr"):
                        source(z, "compare")
        for c in kids(n):
            visit(c)

    for top in kids(ast):
        visit(top)
    return ff


# ---- retyping declarations ---------------------------------------------------------------

BASE_RE = re.compile(r"\b(unsigned\s+long\s+int|unsigned\s+long|unsigned\s+int|unsigned|"
                     r"signed\s+int|signed|long\s+int|long|int)\b")
SPEC_WORDS = r"(?:extern|static|register|const|volatile|unsigned|signed|int|long)"


def retype_ok(dq, mode):
    if mode in ("v", "r"):
        base = re.sub(r"\[[^\]]*\]", "", dq).strip()
        if mode == "r":
            m = re.match(r"^(.*?)\s*\(.*\)$", dq)
            base = m.group(1).strip() if m else ""
        return base in NARROW_INT or base in ("long", "unsigned long")
    if mode == "p":
        return re.sub(r"\s+", " ", dq) in ("int *", "unsigned int *", "unsigned *")
    return False


def new_base(spec_text):
    m = BASE_RE.search(spec_text)
    if not m:
        return None
    return m, ("uptr" if m.group(1).startswith("unsigned") else "iptr")


def plan_decl_edits(sites, wants, texts):
    """Edits that retype the declarations named in wants. sites: every declaration seen;
    returns [(file, start, end, text, why)] and the list of retyped declarations."""
    by_ident = collections.defaultdict(list)
    for s in sites:
        by_ident[s[0]].append(s)
    retype = {}                      # (file, begin, loc) -> new site
    done = []
    for ident, mode in wants:
        if ident[0] == "f":
            fsites = by_ident.get(ident, [])
            for s in fsites:
                _i, f, b, lo, e, dq, q = s
                if retype_ok(dq, "r") and "iptr" not in q.split("(")[0] and "uptr" not in q.split("(")[0]:
                    retype[(f, b, lo)] = (s, "r")
            continue
        for s in by_ident.get(ident, []):
            _i, f, b, lo, e, dq, q = s
            if q.startswith(("iptr", "uptr")) or "iptr" in q or "uptr" in q:
                continue
            if retype_ok(dq, mode):
                retype[(f, b, lo)] = (s, mode)
    # group declarators that share their specifiers (`int a, b;`)
    groups = collections.defaultdict(list)
    for s in sites:
        groups[(s[1], s[2])].append(s)
    edits = []
    seen_groups = set()
    for key, (s, mode) in sorted(retype.items()):
        f, b, lo = key
        if s[0][0] in ("f",):
            t = texts[f]
            spec = t[b:lo]
            nb = new_base(spec)
            if not nb:
                continue
            m, word = nb
            edits.append((f, b + m.start(), b + m.end(), word, "ret"))
            done.append((s[0], f, t.count("\n", 0, lo) + 1, word))
            continue
        g = sorted({x for x in groups[(f, b)] if x[0][0] != "f"}, key=lambda x: x[3])
        if len(g) <= 1 or s[0][0] == "p":
            t = texts[f]
            spec = t[b:lo]
            nb = new_base(spec)
            if not nb:
                continue
            m, word = nb
            edits.append((f, b + m.start(), b + m.end(), word, "decl"))
            done.append((s[0], f, t.count("\n", 0, lo) + 1, word))
            continue
        if (f, b) in seen_groups:
            continue
        seen_groups.add((f, b))
        t = texts[f]
        members = [(x, retype.get((f, b, x[3]))) for x in g]
        first_lo = g[0][3]
        head = t[b:first_lo]
        hm = re.match(r"^((?:%s\s+)*%s\b)" % (SPEC_WORDS, SPEC_WORDS), head)
        if not hm:
            continue
        spec = hm.group(1)
        pos = b + len(spec)
        parts = []
        ok = True
        for i, (x, r) in enumerate(members):
            end = x[4]
            d = t[pos:end]
            if "(" in d and "=" not in d:
                ok = False
            parts.append((d.strip(), r))
            # skip the comma to the next declarator
            if i + 1 < len(members):
                c = t.find(",", end)
                if c < 0:
                    ok = False
                    break
                pos = c + 1
        if not ok:
            continue
        tail_end = members[-1][0][4]
        nbm = new_base(spec)
        if not nbm:
            continue
        m, _w = nbm
        out = []
        for d, r in parts:
            if r is not None:
                sp = spec[:m.start()] + ("uptr" if m.group(1).startswith("unsigned") else "iptr") + spec[m.end():]
            else:
                sp = spec
            out.append("%s %s" % (sp, d))
        # merge neighbours with the same specifiers back into one declaration
        merged = []
        for d, r in parts:
            sp = spec if r is None else spec[:m.start()] + ("uptr" if m.group(1).startswith("unsigned") else "iptr") + spec[m.end():]
            if merged and merged[-1][0] == sp:
                merged[-1][1].append(d)
            else:
                merged.append((sp, [d]))
        new = "; ".join("%s %s" % (sp, ", ".join(ds)) for sp, ds in merged)
        edits.append((f, b, tail_end, new, "group"))
        for x, r in members:
            if r is not None:
                done.append((x[0], f, t.count("\n", 0, x[3]) + 1, "iptr"))
    return edits, done


def apply_edits(edits, texts):
    """Apply (file, start, end, text) edits; overlapping replacements keep the first."""
    by_file = collections.defaultdict(list)
    for e in edits:
        by_file[e[0]].append(e)
    changed = {}
    for f, es in by_file.items():
        es = sorted(set((s, e, t) for _f, s, e, t, _w in es), key=lambda x: (x[0], x[1]))
        t = texts[f]
        out = []
        pos = 0
        last_end = -1
        for s, e, new in es:
            if s < pos:            # overlaps an edit already made
                if s == e and s >= last_end:
                    pass
                else:
                    continue
            out.append(t[pos:s])
            out.append(new)
            pos = e
            last_end = e
        out.append(t[pos:])
        nt = "".join(out)
        if nt != t:
            changed[f] = nt
    return changed


INCLUDE_LINE = '#include "ptrint.h"\n'


def ensure_include(text):
    """ptrint.h for a file that now uses iptr and does not see it through records.h."""
    if not re.search(r"\b[iu]ptr\b", text):
        return text
    if re.search(r'#include "(ptrint|records|structs|dagger)\.h"', text):
        return text
    lines = text.split("\n")
    k = 0
    # after the leading comment
    if lines and lines[0].lstrip().startswith("/*"):
        while k < len(lines) and "*/" not in lines[k]:
            k += 1
        k += 1
    for i in range(k, len(lines)):
        if lines[i].startswith("#include"):
            k = i
            break
        if lines[i].strip() and not lines[i].startswith("#"):
            break
    lines.insert(k, INCLUDE_LINE.rstrip("\n"))
    return "\n".join(lines)


# ---- the driver -----------------------------------------------------------------------------

def run(files, in_place, max_iter, log, extra_wants=()):
    """Iterate to a fixed point; then settle the lifter's integer arithmetic (final) and
    iterate again, until a final round changes nothing."""
    os.chdir(ROOT)
    total_done, whys = [], {}
    all_res, all_slots = [], []
    final = False
    for it in range(1, max_iter + 1):
        with ThreadPoolExecutor(os.cpu_count()) as ex:
            ig = frozenset(int_globals_scan(files))
            facts = list(ex.map(lambda p: analyze(p, final, ig), files))
        sites, wants, edits, res, slots = [], set(extra_wants), [], [], []
        puns = []
        for ff in facts:
            puns += ff.puns
            sites += ff.sites
            wants |= ff.wants
            edits += ff.edits
            res += ff.residue
            slots += ff.slots
            for w, y in ff.why.items():
                whys.setdefault(w, y)
        sites = list(set(sites))
        texts = {}
        for f in {s[1] for s in sites} | {e[0] for e in edits}:
            texts[f] = open(os.path.join(ROOT, f), encoding="latin-1").read()
        dedits, done = plan_decl_edits(sites, wants, texts)
        changed = apply_edits(edits + dedits, texts)
        for f in list(changed):
            if f.endswith(".c"):
                changed[f] = ensure_include(changed[f])
        count = collections.Counter(e[4] for e in set(edits))
        log("iteration %d%s: %s; %d declarations retyped; %d files change"
            % (it, " (final)" if final else "",
               ", ".join("%d %s" % (n, k) for k, n in sorted(count.items())) or "no edits",
               len(done), len(changed)))
        total_done += [d + (whys.get(_want_key(d[0], wants)),) for d in done]
        all_res = res + [(a, "pun: " + b) for a, b in puns]
        all_slots += slots
        if not in_place:
            break
        for f, txt in changed.items():
            with open(os.path.join(ROOT, f), "w", encoding="latin-1") as fh:
                fh.write(txt)
        if not changed:
            if final:
                break
            final = True
        else:
            final = False
    return total_done, sorted(set(all_res)), sorted(set(all_slots))


def _want_key(ident, wants):
    for w in wants:
        if w[0] == ident:
            return w
    return None


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("cmd", choices=["run", "show"])
    ap.add_argument("files", nargs="*")
    ap.add_argument("--in-place", action="store_true")
    ap.add_argument("--max-iter", type=int, default=40)
    ap.add_argument("--log", help="write the retyped declarations, residue and slots here")
    a = ap.parse_args()
    os.chdir(ROOT)
    files = a.files or game_files()
    if a.cmd == "show":
        ig = frozenset(int_globals_scan(game_files()))
        for p in files:
            ff = analyze(p, False, ig)
            for w in sorted(ff.wants, key=str):
                print("want", w, ff.why.get(w))
            for e in ff.edits:
                print("edit", e)
            for r in ff.residue:
                print("residue", r)
            for s in ff.slots:
                print("slot", s)
        return
    done, res, slots = run(files, a.in_place, a.max_iter, print)
    print("%d declarations retyped; %d residue notes; %d slots" % (len(done), len(res), len(slots)))
    if a.log:
        with open(a.log, "w") as f:
            for d in done:
                f.write("retyped %s %s:%d %s  (from %s)\n" % (d[0], d[1], d[2], d[3], d[4]))
            for r in res:
                f.write("residue %s %s\n" % r)
            for s in slots:
                f.write("slot %s %s\n" % s)


if __name__ == "__main__":
    sys.exit(main())
