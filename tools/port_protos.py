#!/usr/bin/env python3
"""Prototypes for the calls the game makes through declarations without parameters
(docs/port.md, phase 3). `extern int f();` passes and returns ints: natively a pointer that goes
through one loses its top half, and a call to a function whose parameters are pointers leaves
the top halves of the registers undefined. This tool gives every such declaration its
parameters:
  - the C library, MemCheck and SOS: the file's own declarations go, and the file includes
    include/clib.h, which declares them all once;
  - XnGine's functions (no definition in the game's C): the engine's prototype
    (src/engine/x*.h, read with clang), in the game's types: s32 and the narrower integers as
    int (what a call without a prototype passes), byte pointers as char *, other pointers as
    void *, the return as the file declared it unless the engine's is void or a pointer;
  - the game's own functions: the definition's parameters, narrower integers as int.
A file keeps a change only when Watcom 10.0a still compiles every function in it to
FALL.EXE's bytes (tools/structure.py's Verifier); otherwise the changes that break it are
found one at a time and left out.

  port_protos.py apply [FILES] [--in-place] [--report CSV]
  port_protos.py show FILE        the changes, without compiling
  port_protos.py write [FILES]    make every change, without compiling (then tools/port_iptr.py
                                  run --in-place adds the casts the call sites need)
  port_protos.py verify --base DIR [--in-place] [--report CSV]
                                  check each file against its matching text in DIR (a copy of
                                  the tree from before `write`); put back the declarations
                                  that break a file

The call sites still pass ints where a prototype now wants a pointer (and the reverse): the
explicit casts are tools/port_iptr.py's (`fix` rules), run after this.
"""
import argparse
import collections
import csv
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

TOOLS = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, TOOLS)
ROOT = os.path.dirname(TOOLS)

DECL_RE = re.compile(r"^extern\s+([A-Za-z_][\w\s\*]*?)\s*\b(\w+)\s*\((.*)\)\s*;\s*(/\*.*\*/)?\s*$")
CLIB = os.path.join(ROOT, "include", "clib.h")
INCLUDE = '#include "clib.h"'

# engine functions with no prototype in src/engine (asm interfaces), and the object-2
# functions without a name
MANUAL = {
    "xn_kbd_numlock_off": ("void", ()),
    "xn_kbd_read_key": (None, ()),
    "xn_mouse_set_range_320x200": ("void", ()),
    "xn_sys_yield": ("void", ()),
    "func_000C810C": (None, ("void *",)),
    "func_000C2068": (None, ("void *",)),
    "func_000C9EB2": (None, ()),
    # put_pixel and get_pixel (port/shim/xgfx.c): one file passes shorts (a prototype of its
    # own), the others ints
    "func_000A134C": (None, ("int", "int", "int")),
    "func_000A138E": (None, ("int", "int")),
    "func_000CE4E0": (None, ("void *",)),
}


# declarations a system header already makes (with parameters)
DROP = {"int386x": "#include <i86.h>"}


def game_files():
    import glob
    return sorted(glob.glob(os.path.join(ROOT, "src", "lifted", "*.c")) +
                  glob.glob(os.path.join(ROOT, "src", "hand", "*.c")) +
                  glob.glob(os.path.join(ROOT, "src", "*.c")))


def clib_names():
    names = set()
    for line in open(CLIB):
        m = DECL_RE.match(line.strip())
        if m:
            names.add(m.group(2))
    return names


# ---- the engine's prototypes ------------------------------------------------------------------

INT_LIKE = {"s32", "int", "signed int", "s16", "short", "u16", "unsigned short", "u8",
            "unsigned char", "s8", "signed char", "char", "long"}
UNSIGNED_LIKE = {"u32", "unsigned int", "unsigned"}


def game_type(t, param=True):
    """An engine type as the game declares it."""
    t = re.sub(r"\bconst\b", "", t)
    t = re.sub(r"\s+", " ", t).strip()
    if t.endswith("*"):
        base = t[:-1].strip()
        if base.endswith("*"):
            return "void *"
        if base in ("u8", "s8", "char", "unsigned char", "signed char"):
            return "char *"
        if base in ("s32", "int"):
            return "int *"
        if base in ("u32", "unsigned int", "unsigned"):
            return "unsigned *"
        if base in ("s16", "short"):
            return "short *"
        if base in ("u16", "unsigned short"):
            return "unsigned short *"
        return "void *"
    if "(*)" in t:
        return "void (*)()"
    if t == "void":
        return "void"
    if t in UNSIGNED_LIKE:
        return "unsigned"
    if t in INT_LIKE:
        return "int"
    return None


def engine_protos():
    """{name: (return type, (param types))} from src/engine's headers, in the engine's types."""
    cache = os.path.join(ROOT, "build", "port", "engine_protos.json")
    hdrs = sorted(f for f in os.listdir(os.path.join(ROOT, "src", "engine")) if f.endswith(".h"))
    src = '#include "xngine.h"\n' + "".join('#include "%s"\n' % h for h in hdrs if h != "xngine.h")
    with tempfile.TemporaryDirectory() as td:
        p = os.path.join(td, "eng.c")
        open(p, "w").write(src)
        r = subprocess.run(["clang", "-fsyntax-only", "-Wno-everything", "-ferror-limit=0",
                            "-I" + os.path.join(ROOT, "src", "engine"), "-Xclang",
                            "-ast-dump=json", p], capture_output=True)
    d = json.loads(r.stdout)
    out = {}
    for n in d["inner"]:
        if n.get("kind") == "FunctionDecl" and "name" in n:
            ft = n["type"]["qualType"]
            ret = ft.split("(")[0].strip()
            ps = tuple(c.get("type", {}).get("qualType", "") for c in n.get("inner", [])
                       if c.get("kind") == "ParmVarDecl")
            out[n["name"]] = (ret, ps)
    os.makedirs(os.path.dirname(cache), exist_ok=True)
    json.dump(out, open(cache, "w"), indent=0)
    return out


# ---- the game's definitions -------------------------------------------------------------------

def definitions():
    import protos
    return protos.definitions(game_files())


def promoted(t):
    """A parameter type as a call without a prototype passes it."""
    import protos
    n = protos.norm(t)
    if n in ("char", "signed char", "unsigned char", "short", "unsigned short"):
        return "int"
    return t


# ---- one file ---------------------------------------------------------------------------------

def used_value(text, name):
    """Whether any call of name in the file uses its result."""
    for m in re.finditer(r"\b%s\s*\(" % re.escape(name), text):
        ls = text.rfind("\n", 0, m.start()) + 1
        before = text[ls:m.start()]
        if before.startswith("extern") or before.startswith("#"):
            continue
        if before.strip() in ("", "else", "{", "}") or re.match(r"^\s*(else\s*)?$", before):
            # a statement of its own: ends with `);`
            depth, i = 0, m.end() - 1
            while i < len(text):
                c = text[i]
                if c == "(":
                    depth += 1
                elif c == ")":
                    depth -= 1
                    if depth == 0:
                        break
                i += 1
            if text[i + 1:i + 2] == ";":
                continue
        return True
    return False


def call_arities(path):
    """{name: {argument counts}} of the direct calls in a file (clang's AST)."""
    import port_iptr
    ast = port_iptr.load_ast(os.path.relpath(path, ROOT))
    out = collections.defaultdict(set)

    def walk(n):
        if n.get("kind") == "CallExpr":
            c = n["inner"][0]
            while c.get("kind") in ("ImplicitCastExpr", "ParenExpr"):
                c = c["inner"][0]
            if c.get("kind") == "DeclRefExpr":
                out[c["referencedDecl"].get("name")].add(len(n["inner"]) - 1)
        for k in n.get("inner", ()):
            walk(k)
    walk(ast)
    return out


def fit_arity(name, ps, arities):
    """The parameters as the file's calls pass them: a call with fewer arguments than the
    callee's C takes leaves the rest to registers (the engine's asm interface); one with more
    passes ints the callee never reads. None when the calls disagree."""
    a = arities.get(name)
    if not a:
        return ps
    if len(a) > 1:
        return None
    n = a.pop()
    a.add(n)
    if n == len(ps):
        return ps
    return tuple(ps[:n]) + ("int",) * max(0, n - len(ps))


def spell(ret, name, params):
    r = ret if ret.endswith("*") else ret + " "
    ps = ", ".join(params) if params else "void"
    return "extern %s%s(%s);" % (r, name, ps)


def plan_file(path, lib, eng, defs):
    """The changes for one file: [(key, line index, old line, new line or None)]; key 'clib'
    for the library declarations (one change: the include), else the function's name."""
    text = open(path, encoding="latin-1").read()
    lines = text.split("\n")
    out = []
    arities = None
    for k, l in enumerate(lines):
        if not l.startswith("extern"):
            continue
        m = DECL_RE.match(l)
        if not m:
            continue
        ret, name, params = m.group(1).strip(), m.group(2), m.group(3).strip()
        if name in lib:
            out.append(("clib", k, l, None))
            continue
        if name in DROP and DROP[name] in text:
            out.append((name, k, l, None))
            continue
        if params != "":
            continue
        if arities is None:
            arities = call_arities(path)
        if name in MANUAL:
            r, ps = MANUAL[name]
            out.append((name, k, l, spell(r or ret, name, ps)))
        elif name in defs:
            dret, dps, _f = defs[name]
            if dps is None:
                continue
            ps = fit_arity(name, tuple(promoted(p) for p in dps), arities)
            if ps is None:
                continue
            r = dret if ("*" in dret or dret == "void" or "iptr" in dret) else ret
            if r == "void" and used_value(text, name):
                r = ret
            out.append((name, k, l, spell(r, name, ps)))
        elif name in eng:
            eret, eps = eng[name]
            ps = tuple(game_type(p) for p in eps)
            if None in ps:
                continue
            ps = fit_arity(name, ps, arities)
            if ps is None:
                continue
            gr = game_type(eret)
            if gr == "void" and not used_value(text, name):
                r = "void"
            elif gr is not None and gr.endswith("*"):
                r = gr
            else:
                r = ret
            out.append((name, k, l, spell(r, name, ps)))
    return text, out


def render(text, changes, chosen):
    """The file with the chosen changes made."""
    lines = text.split("\n")
    drop = set()
    lib = False
    for key, k, _old, new in changes:
        if key not in chosen:
            continue
        if key == "clib":
            drop.add(k)
            lib = True
        elif new is None:
            drop.add(k)
        else:
            lines[k] = new
    if lib:
        # the include goes where the first library declaration was, or after the includes
        first = min(k for key, k, _o, _n in changes if key == "clib")
        incs = [i for i, l in enumerate(lines) if l.startswith("#include")]
        at = incs[-1] + 1 if incs else first
        lines.insert(at, INCLUDE)
        drop = {d + (1 if d >= at else 0) for d in drop}
    out = [l for i, l in enumerate(lines) if i not in drop]
    return "\n".join(out)


# ---- verification -----------------------------------------------------------------------------

def check_many(ver, items, tmp):
    """items: [(text, base text)]; for each, whether every function that matches in the base
    matches in the text, and the ones that do not."""
    texts = []
    for txt, base in items:
        texts += [txt, base]
    res = ver.run(texts, [None] * len(texts), tmp, None)
    out = []
    for i in range(len(items)):
        r, b = res[2 * i], res[2 * i + 1]
        if b is None:
            out.append((None, ["(base does not compile)"]))
            continue
        want = [n for n, ok in b.items() if ok]
        if r is None:
            out.append((False, ["(does not compile)"]))
            continue
        bad = sorted(n for n in want if not r.get(n))
        out.append((not bad, bad))
    return out


def verify(files, lib, eng, defs, in_place, log=print):
    """Every file with all its changes in one batch; then, for the files that fail, each change
    alone, then the good ones together (greedy)."""
    import structure
    ver = structure.Verifier()
    tmp = tempfile.mkdtemp(prefix="pprotos_", dir=os.environ.get("STRUCT_TMP"))
    plans = {}
    for p in files:
        text, changes = plan_file(p, lib, eng, defs)
        if changes:
            plans[p] = (text, changes, sorted({c[0] for c in changes}))
    order = sorted(plans)
    res = check_many(ver, [(render(plans[p][0], plans[p][1], set(plans[p][2])), plans[p][0])
                           for p in order], tmp)
    keep, why = {}, {}
    failing = []
    for p, (ok, bad) in zip(order, res):
        if ok:
            keep[p] = list(plans[p][2])
        else:
            failing.append(p)
            log("%s: fails with all changes (%s)" % (os.path.relpath(p, ROOT), ",".join(bad)))
    # singles
    items, owners = [], []
    for p in failing:
        text, changes, keys = plans[p]
        for k in keys:
            items.append((render(text, changes, {k}), text))
            owners.append((p, k))
    res = check_many(ver, items, tmp) if items else []
    good = collections.defaultdict(list)
    for (p, k), (ok, bad) in zip(owners, res):
        if ok:
            good[p].append(k)
        else:
            why[(p, k)] = ",".join(bad)
    # the good ones together, then greedily
    for p in failing:
        text, changes, keys = plans[p]
        ks = good[p]
        if not ks:
            keep[p] = []
            continue
        ok, bad = check_many(ver, [(render(text, changes, set(ks)), text)], tmp)[0]
        if ok:
            keep[p] = ks
            continue
        got = []
        for k in ks:
            if check_many(ver, [(render(text, changes, set(got + [k])), text)], tmp)[0][0]:
                got.append(k)
            else:
                why[(p, k)] = "with the others"
        keep[p] = got
    rows = []
    for p in order:
        text, changes, keys = plans[p]
        rel = os.path.relpath(p, ROOT)
        for k in keys:
            ok = k in keep[p]
            rows.append(dict(file=rel, change=k, kept="yes" if ok else "no",
                             why="" if ok else why.get((p, k), "")))
            if not ok:
                log("%s: %s left out (%s)" % (rel, k, why.get((p, k), "")))
        if in_place and keep[p]:
            open(p, "w", encoding="latin-1").write(render(text, changes, set(keep[p])))
    shutil.rmtree(tmp, ignore_errors=True)
    return rows


def revert(cur, base_changes, keys):
    """The current text with the declarations of `keys` put back as the base had them."""
    lines = cur.split("\n")
    for key in keys:
        olds = [(k, old) for kk, k, old, _new in base_changes if kk == key]
        if key == "clib":
            if INCLUDE not in lines:
                continue
            at = lines.index(INCLUDE)
            lines[at:at + 1] = [old for _k, old in olds]
            continue
        pat = re.compile(r"^extern\s.*\b%s\s*\(" % re.escape(key))
        hit = [i for i, l in enumerate(lines) if pat.match(l)]
        if hit:
            lines[hit[0]] = olds[0][1]
        else:
            incs = [i for i, l in enumerate(lines) if l.startswith("#include")]
            lines.insert(incs[-1] + 1 if incs else 0, olds[0][1])
    return "\n".join(lines)


def verify_against(files, base_dir, lib, eng, defs, in_place, log=print):
    """Each file as it is now against its text in base_dir (which matches): every function
    must still match. For a file that does not, the declaration changes are put back, the
    fewest that make it match."""
    import structure
    ver = structure.Verifier()
    tmp = tempfile.mkdtemp(prefix="pprotos_", dir=os.environ.get("STRUCT_TMP"))
    todo = []
    for p in files:
        rel = os.path.relpath(p, ROOT)
        bp = os.path.join(base_dir, rel)
        if not os.path.exists(bp):
            continue
        cur, base = open(p, encoding="latin-1").read(), open(bp, encoding="latin-1").read()
        if cur != base:
            todo.append((p, rel, cur, base, plan_file(bp, lib, eng, defs)[1]))
    res = check_many(ver, [(cur, base) for _p, _r, cur, base, _c in todo], tmp)
    rows = []
    for (p, rel, cur, base, changes), (ok, bad) in zip(todo, res):
        keys = sorted({c[0] for c in changes})
        if ok:
            rows.append(dict(file=rel, result="ok", reverted=""))
            continue
        log("%s: %s fail" % (rel, ",".join(bad)))
        singles = check_many(ver, [(revert(cur, changes, [k]), base) for k in keys], tmp) if keys else []
        fixed = None
        for k, (ok1, _b) in zip(keys, singles):
            if ok1:
                fixed = [k]
                break
        if fixed is None and keys:
            ok_all, bad_all = check_many(ver, [(revert(cur, changes, keys), base)], tmp)[0]
            if ok_all:
                back = list(keys)          # put declarations back in while it still matches
                for k in keys:
                    trial = [x for x in back if x != k]
                    if check_many(ver, [(revert(cur, changes, trial), base)], tmp)[0][0]:
                        back = trial
                fixed = back
        if fixed is None:
            rows.append(dict(file=rel, result="FAIL " + ",".join(bad), reverted=""))
            log("%s: still fails with every declaration put back" % rel)
            continue
        rows.append(dict(file=rel, result="reverted", reverted=" ".join(fixed)))
        log("%s: put back %s" % (rel, " ".join(fixed)))
        if in_place:
            open(p, "w", encoding="latin-1").write(revert(cur, changes, fixed))
    shutil.rmtree(tmp, ignore_errors=True)
    return rows


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("cmd", choices=["apply", "show", "write", "verify"])
    ap.add_argument("--base", help="verify: the folder holding the matching texts (src/...)")
    ap.add_argument("files", nargs="*")
    ap.add_argument("--in-place", action="store_true")
    ap.add_argument("--report")
    a = ap.parse_args()
    files = [os.path.abspath(f) for f in a.files] or game_files()
    lib, eng, defs = clib_names(), engine_protos(), definitions()
    if a.cmd == "show":
        for p in files:
            text, changes = plan_file(p, lib, eng, defs)
            for c in changes:
                print("%s:%d: %s  ->  %s" % (os.path.relpath(p, ROOT), c[1] + 1, c[2], c[3]))
        return
    if a.cmd == "write":
        n = 0
        for p in files:
            text, changes = plan_file(p, lib, eng, defs)
            if changes:
                open(p, "w", encoding="latin-1").write(render(text, changes, {c[0] for c in changes}))
                n += 1
        print("%d files rewritten" % n)
        return
    if a.cmd == "verify":
        rows = verify_against(files, a.base, lib, eng, defs, a.in_place)
        print("%d files checked: %d ok, %d with declarations put back, %d failing" % (
            len(rows), sum(r["result"] == "ok" for r in rows),
            sum(r["result"] == "reverted" for r in rows),
            sum(r["result"].startswith("FAIL") for r in rows)))
        if a.report:
            with open(a.report, "w", newline="") as f:
                w = csv.DictWriter(f, fieldnames=["file", "result", "reverted"])
                w.writeheader()
                w.writerows(rows)
        return
    rows = verify(files, lib, eng, defs, a.in_place)
    kept = sum(1 for r in rows if r["kept"] == "yes")
    print("%d changes; %d kept, %d left out" % (len(rows), kept, len(rows) - kept))
    if a.report:
        with open(a.report, "w", newline="") as f:
            w = csv.DictWriter(f, fieldnames=["file", "change", "kept", "why"])
            w.writeheader()
            w.writerows(sorted(rows, key=lambda r: (r["file"], r["change"])))


if __name__ == "__main__":
    main()
