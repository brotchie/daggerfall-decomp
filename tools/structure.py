#!/usr/bin/env python3
"""Rewrite the lifter's goto control flow as structured C, keeping every function
byte-identical (docs/natural_c.md).

The lifted functions in src/lifted/ write control flow the way the code runs: `if (c) goto
L;`, `goto L;` and `L:;` labels, one label per address, in address order. Watcom C32 10.0a
at -od compiles each structured statement to a fixed jump shape (docs/natural_c.md), so the
shapes can be read back: this tool parses each function's gotos and labels, finds the
regions that are the shape of an if / if-else / && / || / while / do-while / for / for(;;)
with break and continue, and writes them as such. Every function is then compiled with
Watcom 10.0a and compared with FALL.EXE; a function keeps the structured form only where it
matches (a search over its regions when the whole function does not), else it stays as it
was.

usage:
  structure.py show FILE.c [--func NAME]
        print the structured form of FILE's functions (no compile)
  structure.py run FILE.c ... [-o DIR | --in-place] [-j N] [--report CSV]
        structure, verify and write each file (to DIR/<name>, or over the file); prints
        gotos and labels before and after, per file
  structure.py count FILE.c ...
        gotos and labels in the files
  structure.py selftest FILE.c ...
        check that the parser reproduces every function exactly with no region structured

The input is the lifter's form (a statement per line, gotos, labels, switches, blocks of
inner-block locals); a function already written as structured C is left alone ("not
parsed"). STRUCT_TMP=dir puts the compile scratch there; STRUCT_SABOTAGE=while writes every
while condition wrong, to test that the search gives them back.
"""
import argparse
import collections
import os
import re
import sys
import time

TOOLS = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.dirname(TOOLS)
sys.path.insert(0, TOOLS)

COMPARE = ("==", "!=", "<=", ">=", "<", ">")
FLIP = {"==": "!=", "!=": "==", "<": ">=", ">=": "<", ">": "<=", "<=": ">"}
MAXLINE = 100


# ---- reading the functions ----------------------------------------------------------------

def match_paren(s, i):
    """Index of the parenthesis closing the one at s[i]."""
    d = 0
    q = None
    k = i
    while k < len(s):
        c = s[k]
        if q:
            if c == "\\":
                k += 2
                continue
            if c == q:
                q = None
        elif c in "\"'":
            q = c
        elif c == "(":
            d += 1
        elif c == ")":
            d -= 1
            if d == 0:
                return k
        k += 1
    return -1


def brace_delta(line):
    d = 0
    q = None
    k = 0
    s = line.split("//")[0] if "//" in line and '"' not in line else line
    while k < len(s):
        c = s[k]
        if q:
            if c == "\\":
                k += 2
                continue
            if c == q:
                q = None
        elif c in "\"'":
            q = c
        elif c == "{":
            d += 1
        elif c == "}":
            d -= 1
        k += 1
    return d


class Unsupported(Exception):
    pass


class Item:
    __slots__ = ("kind", "line", "text", "cond", "target", "body", "head", "uid", "seq",
                 "orig", "lineno")

    def __init__(self, kind, line, text="", cond=None, target=None, body=None, head=None):
        self.kind, self.line, self.text, self.cond, self.target = kind, line, text, cond, target
        self.body, self.head = body, head
        self.uid = None
        self.seq = None
        self.orig = kind
        self.lineno = None

    def __repr__(self):
        return "<%s %s>" % (self.kind, self.text or self.target)


class Container:
    def __init__(self, items, kind, close_line=None):
        self.items = items
        self.kind = kind            # 'func', 'switch', 'block'
        self.close_line = close_line
        self.addr_at = None
        self.end_addr = None


DECL_RE = re.compile(r"^(?:(?:unsigned|signed|const|volatile|register|static|int|short|char|long|"
                     r"double|float|void|struct\s+\w+|union\s+\w+)\b\s*)+[\s\*]*\w+(?:\[[^\]]*\])*;$")
LABEL_RE = re.compile(r"^([A-Za-z_]\w*):;?$")
CASE_RE = re.compile(r"^(?:case\b.*|default):;?$")


def classify(raw, void):
    s = raw.strip()
    if LABEL_RE.match(s) and not s.startswith("default"):
        return Item("label", raw, target=LABEL_RE.match(s).group(1))
    if CASE_RE.match(s):
        return Item("case", raw, text=s)
    m = re.match(r"^goto (\w+);$", s)
    if m:
        return Item("goto", raw, target=m.group(1))
    if s == "return;":
        return Item("ret", raw, target="END") if void else Item("stmt", raw, text=s)
    if re.match(r"^return\b.*;$", s):
        return Item("retv", raw, text=s)
    if s.startswith("if ("):
        e = match_paren(s, 3)
        if e < 0:
            raise Unsupported("if: " + s)
        cond = s[4:e]
        rest = s[e + 1:].strip()
        m = re.match(r"^goto (\w+);$", rest)
        if m:
            return Item("cj", raw, cond=cond, target=m.group(1))
        if rest == "return;" and void:
            return Item("ifret", raw, cond=cond, target="END")
        if rest.endswith(";") or rest == "{}":
            return Item("stmt", raw, text=s)
        raise Unsupported("if: " + s)
    if re.match(r"^(else|while|for|do)\b", s):
        raise Unsupported("structured statement: " + s)
    if s.endswith(";"):
        return Item("stmt", raw, text=s)
    raise Unsupported("line: " + s)


def parse_body(lines, k, void, kind):
    """Items up to the closing brace; returns (Container, index after the brace)."""
    items = []
    in_decls = True
    while k < len(lines):
        raw = lines[k]
        s = raw.strip()
        if s == "":
            items.append(Item("blank", raw))
            items[-1].lineno = k
            k += 1
            continue
        if s == "}":
            return Container(items, kind, raw), k + 1
        if s.startswith("/*") or s.startswith("//") or s.startswith("#"):
            if s.startswith("/*") and "*/" not in s:
                raise Unsupported("multi-line comment")
            items.append(Item("comment", raw, text=s))
            items[-1].lineno = k
            k += 1
            continue
        if s == "{":
            start = k
            sub, k = parse_body(lines, k + 1, void, "block")
            it = Item("block", raw, body=sub)
            it.lineno = start
            items.append(it)
            in_decls = False
            continue
        if s.startswith("switch (") and s.endswith("{"):
            start = k
            sub, k = parse_body(lines, k + 1, void, "switch")
            it = Item("switch", raw, head=s[:-1].rstrip(), body=sub)
            it.lineno = start
            items.append(it)
            in_decls = False
            continue
        if in_decls and DECL_RE.match(re.sub(r"\s*/\*.*?\*/\s*$", "", s)):
            items.append(Item("decl", raw, text=s))
            items[-1].lineno = k
            k += 1
            continue
        if brace_delta(raw) != 0:
            raise Unsupported("braces: " + s)
        it = classify(raw, void)
        it.lineno = k
        in_decls = False
        items.append(it)
        k += 1
    raise Unsupported("unterminated body")


class Function:
    def __init__(self, name, header, lines, void):
        self.name, self.header, self.lines, self.void = name, header, lines, void
        self.text = "\n".join(lines)


HEADER_RE = re.compile(r"^[A-Za-z_][^;=]*?\b(\w+)\s*\([^;]*\)\s*$")


def split_file(text):
    """[str | Function]: the file as text segments and function definitions."""
    lines = text.split("\n")
    segs = []
    cur = []
    k = 0
    while k < len(lines):
        l = lines[k]
        m = HEADER_RE.match(l)
        if m and not l.startswith(("extern", "typedef", "#", " ", "\t", "/*")) and \
                k + 1 < len(lines) and lines[k + 1].strip() == "{":
            # the body: until the brace depth returns to 0
            d = 0
            j = k + 1
            while j < len(lines):
                d += brace_delta(lines[j])
                if d == 0:
                    break
                j += 1
            if j >= len(lines):
                cur.append(l)
                k += 1
                continue
            if cur:
                segs.append("\n".join(cur))
                cur = []
            void = bool(re.match(r"^(?:static\s+)?void\s+\w+\s*\(", l))
            segs.append(Function(m.group(1), l, lines[k:j + 1], void))
            k = j + 1
            continue
        cur.append(l)
        k += 1
    segs.append("\n".join(cur))
    return segs


def join_file(segs, texts):
    out = []
    for s in segs:
        out.append(texts.get(s.name, s.text) if isinstance(s, Function) else s)
    return "\n".join(out)


# ---- conditions ---------------------------------------------------------------------------

def top_compare(e):
    """The one top-level comparison operator of e: (index, op), or None."""
    d = 0
    q = None
    found = []
    k = 0
    while k < len(e):
        c = e[k]
        if q:
            if c == "\\":
                k += 2
                continue
            if c == q:
                q = None
        elif c in "\"'":
            q = c
        elif c in "([":
            d += 1
        elif c in ")]":
            d -= 1
        elif d == 0:
            if e.startswith("&&", k) or e.startswith("||", k) or c == "?":
                return None
            for op in COMPARE:
                if e.startswith(op, k):
                    if op in ("<", ">"):
                        nx = e[k + 1:k + 2]
                        pv = e[k - 1:k]
                        if nx in ("<", ">", "=") or pv in ("<", ">", "-"):
                            break
                    if op in ("==", "<=", ">=") or op == "!=" or op in ("<", ">"):
                        found.append((k, op))
                        k += len(op) - 1
                    break
        k += 1
    return found[0] if len(found) == 1 else None


def negate_text(e, style):
    if style == "flip":
        tc = top_compare(e)
        if tc:
            k, op = tc
            return e[:k].rstrip() + " " + FLIP[op] + " " + e[k + len(op):].lstrip()
    return "!(%s)" % e


class Cond:
    __slots__ = ("op", "a", "b", "item", "neg", "pre")

    def __init__(self, op, a=None, b=None, item=None, neg=False, pre=None):
        self.op, self.a, self.b, self.item, self.neg = op, a, b, item, neg
        self.pre = pre      # the cond text with an assignment taken in: `(v = e) != 0`

    def render(self, style, parent=None, right=False):
        if self.op == "leaf":
            t = self.pre or self.item.cond
            t = negate_text(t, style) if self.neg else t
            if parent in ("&&", "||") and top_compare(t) is None and not \
                    re.match(r"^[\w.\->\[\]*()]+$", t):
                t = "(%s)" % t
            return t
        if self.op == "!":
            return "!(%s)" % self.a.render(style)
        s = "%s %s %s" % (self.a.render(style, self.op), self.op,
                          self.b.render(style, self.op, True))
        if parent is not None and (parent != self.op or right):
            s = "(%s)" % s
        return s

    def leaves(self):
        if self.op == "leaf":
            return 1
        return self.a.leaves() + (self.b.leaves() if self.b else 0)


# ---- the structured tree ------------------------------------------------------------------

class Node:
    __slots__ = ("kind", "item", "cond", "body", "els", "init", "incr", "tail", "key",
                 "absorbed", "label", "head", "jump")

    def __init__(self, kind, **kw):
        self.kind = kind
        self.item = self.cond = self.body = self.els = self.init = self.incr = None
        self.tail = self.key = self.label = self.head = self.jump = None
        self.absorbed = ()
        for k, v in kw.items():
            setattr(self, k, v)


class Structurer:
    """One function: the parse with a given set of allowed / forbidden constructs."""

    def __init__(self, fn, xforms=frozenset()):
        splits = hoists = frozenset(xforms)
        self.fn = fn
        lines = fn.lines
        if lines[1].strip() != "{":
            raise Unsupported("no body")
        self.top, k = parse_body(lines, 2, fn.void, "func")
        if k != len(lines):
            raise Unsupported("trailing lines")
        # a label nothing jumps to is a marker (the lifter's `__dagger_tbl` switch-table
        # marks): it stays where it is, and is not part of any condition
        targets = set()
        self._targets(self.top, targets)
        self._marks(self.top, targets)
        # switches that end before the function does (docs/natural_c.md)
        self.split_cands = []
        self._splits(self.top, splits | hoists)
        # nested blocks opened in the middle of a statement's code
        self.hoist_cands = []
        self._hoists(self.top, splits | hoists)
        self._shrinks(self.top, splits | hoists)
        self.xform_cands = self.split_cands + self.hoist_cands
        self.has_jumps = any(it.kind in ("cj", "goto", "label", "mark")
                             for c in self._all(self.top) for it in c.items)
        # number the items, give the code-bearing ones their addresses
        self.seq = 0
        self.containers = []
        self.lpos = {}
        self._walk(self.top)
        self.END = self.seq
        self._fill(self.top)
        self.laddr = {}
        for name, (c, i) in self.lpos.items():
            self.laddr[name] = c.addr_at[i]
        self.laddr["END"] = self.END
        # who jumps where
        self.refs = collections.defaultdict(list)
        for c in self.containers:
            for i, it in enumerate(c.items):
                if it.kind in ("cj", "goto", "ret", "ifret"):
                    if it.target not in self.laddr:
                        raise Unsupported("goto to unknown label " + it.target)
                    self.refs[it.target].append(it)
        self.ctl = ("cj", "goto", "ret", "ifret", "label")
        self.memo = {}

    def _targets(self, c, acc):
        for it in c.items:
            if it.kind in ("cj", "goto", "ret", "ifret"):
                acc.add(it.target)
            elif it.body is not None:
                self._targets(it.body, acc)

    def _marks(self, c, targets):
        for it in c.items:
            if it.kind == "label" and it.target not in targets:
                it.kind = "mark"
            elif it.body is not None:
                self._marks(it.body, targets)

    def _all(self, c):
        yield c
        for it in c.items:
            if it.body is not None:
                yield from self._all(it.body)

    def _jumps(self, c, acc):
        for it in c.items:
            if it.kind in ("cj", "goto"):
                acc[it.target] += 1
            elif it.body is not None:
                self._jumps(it.body, acc)

    def _splits(self, c, splits):
        """The lifter runs a switch's body to the end of the function: the code after the
        switch sits after its `default:`. Where the jumps out of the cases (the breaks) go to
        a label after `default:`, the switch can end there: the items from it on move after
        the switch, and a `default:` left empty goes."""
        p = 0
        while p < len(c.items):
            it = c.items[p]
            if it.kind == "switch" and it.body.items and it.body.items[-1].kind == "block" \
                    and any(x.kind == "case" for x in it.body.items[-1].body.items):
                # a block of inner-block locals opened in one case and running over the
                # later ones: open it before the switch instead, so the switch can end
                key = ("swlift", it.lineno)
                self.split_cands.append(key)
                if key in splits:
                    blk = it.body.items[-1]
                    inner = blk.body.items
                    d = 0
                    while d < len(inner) and inner[d].kind in ("decl", "blank", "comment"):
                        d += 1
                    it.body.items = it.body.items[:-1] + inner[d:]
                    blk.body.items = inner[:d] + [it]
                    blk.head = "swlift"         # placed: not to be moved again
                    c.items[p] = blk
                    continue
            if it.body is not None:
                self._splits(it.body, splits)
            moved = []
            if it.kind == "switch":
                body = it.body.items
                cases = [q for q, x in enumerate(body) if x.kind == "case"]
                if cases and body[cases[-1]].text.startswith("default"):
                    d = cases[-1]
                    cnt = collections.Counter()
                    self._jumps(it.body, cnt)
                    best = None
                    for q in range(d + 1, len(body)):
                        x = body[q]
                        if x.kind == "label" and cnt[x.target] and \
                                (best is None or cnt[x.target] > cnt[body[best].target]):
                            best = q
                    after = body[d + 1:]
                    if best is None and after and \
                            any(x.kind in ("cj", "goto", "label") for x in after) and \
                            not any(x.kind == "stmt" and x.text in ("break;", "continue;")
                                    for x in after):
                        best = d + 1        # the code after an empty default
                    if best is not None:
                        key = ("swend", it.lineno)
                        self.split_cands.append(key)
                        if key in splits:
                            t = best
                            while t > d + 1 and body[t - 1].kind in ("label", "mark", "blank"):
                                t -= 1
                            moved = body[t:]
                            it.body.items = body[:d] if t == d + 1 else body[:t]
                            c.items[p + 1:p + 1] = moved
            p += 1 + len(moved)

    def _texts(self, it):
        """The source text of an item and everything in it."""
        out = [it.line or ""]
        if it.body is not None:
            for x in it.body.items:
                out.extend(self._texts(x))
        return out

    def _shrinks(self, c, hoists):
        """The lifter runs a block of inner-block locals to the end of its container; the
        items after the last use of its locals can follow the block instead (so a loop
        around the block can end)."""
        for it in list(c.items):
            if it.body is not None:
                self._shrinks(it.body, hoists)
        b = 0
        while b < len(c.items):
            it = c.items[b]
            if it.kind != "block":
                b += 1
                continue
            body = it.body.items
            names = [re.match(r".*?(\w+)(\[[^\]]*\])*;", x.text).group(1)
                     for x in body if x.kind == "decl"]
            last = -1
            if names:
                pat = re.compile(r"\b(%s)\b" % "|".join(map(re.escape, names)))
                for q, x in enumerate(body):
                    if x.kind != "decl" and any(pat.search(t) for t in self._texts(x)):
                        last = q
            tail = body[last + 1:] if last >= 0 else []
            if tail and any(x.kind in ("cj", "goto", "label") for x in tail):
                key = ("shrink", it.lineno)
                self.hoist_cands.append(key)
                if key in hoists:
                    it.body.items = body[:last + 1]
                    c.items[b + 1:b + 1] = tail
            b += 1

    def _hoists(self, c, hoists):
        """The lifter opens a block for inner-block locals at their first use, which can be
        in the middle of an if or a loop. Opening it earlier, at the start of its case (or
        of its container), lets the code around it be structured."""
        for it in list(c.items):
            if it.body is not None:
                self._hoists(it.body, hoists)
        b = 0
        while b < len(c.items):
            it = c.items[b]
            if it.kind != "block" or it.head == "swlift":
                b += 1
                continue
            h = 0
            for q in range(b):
                if c.items[q].kind == "case":
                    h = q + 1
            while h < b and c.items[h].kind in ("decl", "blank", "comment"):
                h += 1
            if h < b and any(c.items[q].kind in ("label", "cj", "goto", "ret", "ifret")
                             for q in range(h, b)):
                key = ("hoist", it.lineno)
                self.hoist_cands.append(key)
                if key in hoists:
                    body = it.body.items
                    d = 0
                    while d < len(body) and body[d].kind in ("decl", "blank", "comment"):
                        d += 1
                    it.body.items = body[:d] + c.items[h:b] + body[d:]
                    c.items[h:b] = []
                    b = h
            b += 1

    def _walk(self, c):
        self.containers.append(c)
        for i, it in enumerate(c.items):
            it.uid = it.lineno
            if it.kind == "label":
                if it.target in self.lpos:
                    raise Unsupported("duplicate label " + it.target)
                self.lpos[it.target] = (c, i)
            elif it.kind == "block":
                self._walk(it.body)
            elif it.kind == "switch":
                it.seq = self.seq
                self.seq += 1
                self._walk(it.body)
            elif it.kind in ("case", "decl", "blank", "mark", "comment"):
                pass
            else:
                it.seq = self.seq
                self.seq += 1
        c.end_addr = self.seq

    def _fill(self, c):
        n = len(c.items)
        c.addr_at = [None] * (n + 1)
        c.addr_at[n] = c.end_addr
        nxt = c.end_addr
        for i in range(n - 1, -1, -1):
            it = c.items[i]
            if it.kind == "block":
                self._fill(it.body)
                nxt = it.body.addr_at[0]
            elif it.kind == "switch":
                self._fill(it.body)
                nxt = it.seq
            elif it.seq is not None:
                nxt = it.seq
            c.addr_at[i] = nxt

    # -- helpers --
    def taddr(self, it):
        return self.laddr[it.target]

    def skip(self, c, i):
        n = len(c.items)
        while i < n and c.items[i].kind in ("label", "blank"):
            i += 1
        return i

    def is_ctl(self, it):
        return it.kind in self.ctl or it.kind == "blank"

    def run_end(self, c, i):
        n = len(c.items)
        while i < n:
            it = c.items[i]
            if self.is_ctl(it):
                i += 1
            elif it.kind == "stmt" and i + 1 < n and self.embed(it, c.items[i + 1]) is not None:
                i += 1
            else:
                break
        return i

    def end_index(self, c, lo, hi, a, name=None):
        """The index in [lo, hi] of c where code reaches address a (a body ends there)."""
        if name and name in self.lpos:
            lc, li = self.lpos[name]
            if lc is c and lo <= li <= hi:
                return li
        if c.addr_at[hi] == a:
            return hi
        return None

    def has_case(self, c, a, b):
        """A case label among items[a:b] (or in a block among them): no construct around
        it."""
        def inside(it):
            return it.kind == "case" or (it.kind == "block" and
                                         any(inside(x) for x in it.body.items))
        return any(inside(c.items[k]) for k in range(a, b))

    # -- the condition grammar (docs/natural_c.md) --
    def pE(self, c, i, F, top, first=True):
        """[(cond, j)]: items[i:j] jump to F when cond is false and fall through to j when
        it is true. first: the condition's first test may not take in an assignment."""
        return self._spans(self.sE, c, i, F, top, first)

    def pJT(self, c, i, T, top, first=True):
        """[(cond, j)]: items[i:j] jump to T when cond is true and fall through to j when it
        is false."""
        return self._spans(self.sJT, c, i, T, top, first)

    JUMPS = ("cj", "goto", "ret", "ifret")

    def _spans(self, fn, c, i, X, top, first):
        i0 = self.skip(c, i)
        e = self.run_end(c, i0)
        out = []
        for k in range(i0 + 1, e + 1):
            if c.items[k - 1].kind not in self.JUMPS:
                continue
            for cond in fn(c, i0, k, X, top, first):
                out.append((cond, k))
        return out

    def _ends(self, c, i0, k):
        """Where a span ending at k may be split: j in (i0, k) after a jump."""
        return [j for j in range(i0 + 1, k) if c.items[j - 1].kind in self.JUMPS]

    def _before(self, c, i0, g):
        """The end of the span before item g (its labels skipped back)."""
        j = g
        while j > i0 and c.items[j - 1].kind in ("label", "blank"):
            j -= 1
        return j

    def embed(self, st, cj):
        """`v = e;` then `if (... v ...)`: the cond with the assignment in it, `(v = e)`, when
        v is the left operand's only variable use; else None."""
        if st.kind != "stmt" or cj.kind not in ("cj", "ifret"):
            return None
        m = re.match(r"^(\w+) = (.+);$", st.text)
        if not m or "=" in m.group(2).replace("==", "").replace("!=", "").replace(
                "<=", "").replace(">=", ""):
            return None
        v, e = m.groups()
        cond = cj.cond
        tc = top_compare(cond)
        if not tc:
            return None
        lhs, rhs = cond[:tc[0]], cond[tc[0]:]
        uses = [u for u in re.finditer(r"(?<![\w.>&])%s\b" % re.escape(v), lhs)]
        if len(uses) != 1 or re.search(r"\b%s\b" % re.escape(v), rhs) or \
                re.search(r"\b%s\b" % re.escape(v), e):
            return None
        u = uses[0]
        return lhs[:u.start()] + "(%s = %s)" % (v, e) + lhs[u.end():] + rhs

    def _lead(self, c, i0, k, X, first):
        """A test at the start of items[i0:k] jumping to X: (leaf, its span length) list."""
        items = c.items
        out = []
        if items[i0].kind in ("cj", "ifret"):
            if k == i0 + 1 and self.taddr(items[i0]) == X:
                out.append(Cond("leaf", item=items[i0]))
        elif not first and k == i0 + 2 and items[i0].kind == "stmt":
            t = self.embed(items[i0], items[i0 + 1])
            if t is not None and self.taddr(items[i0 + 1]) == X:
                out.append(Cond("leaf", item=items[i0 + 1], pre=t))
        return out

    def _starts(self, c, i0, first):
        it = c.items[i0]
        if it.kind in ("cj", "ifret"):
            return True
        return not first and it.kind == "stmt" and i0 + 1 < len(c.items) and \
            self.embed(it, c.items[i0 + 1]) is not None

    def sE(self, c, i, k, F, top, first):
        """The conds for which items[i:k] is E(cond, F): jump to F when false, fall through
        when true (Watcom's code for `if (cond)`)."""
        i0 = self.skip(c, i)
        key = ("E", id(c), i0, k, F, top, first)
        if key in self.memo:
            return self.memo[key]
        res = []
        items = c.items
        if i0 < k and self._starts(c, i0, first):
            for lf in self._lead(c, i0, k, F, first):
                lf.neg = True
                res.append(lf)
            Tp = c.addr_at[k]
            # A || B: J_T(A, T') E(B, F) T':
            if Tp != F:
                for j in self._ends(c, i0, k):
                    for A in self.sJT(c, i0, j, Tp, False, first):
                        for B in self.sE(c, j, k, F, False, False):
                            res.append(Cond("||", A, B))
            # A && B: J_T(A && B, T') goto F; T':
            g = k - 1
            if items[g].kind in ("goto", "ret") and self.taddr(items[g]) == F:
                j = self._before(c, i0, g)
                if j > i0:
                    for C in self.sJT(c, i0, j, Tp, False, first):
                        if C.op == "&&":
                            res.append(C)
            # !(x) around a whole condition
            if top:
                for x in self.sJT(c, i0, k, F, False, first):
                    if x.op in ("&&", "||"):
                        res.append(Cond("!", x))
        self.memo[key] = res
        return res

    def sJT(self, c, i, k, T, top, first):
        """The conds for which items[i:k] is J_T(cond, T): jump to T when true, fall through
        when false."""
        i0 = self.skip(c, i)
        key = ("J", id(c), i0, k, T, top, first)
        if key in self.memo:
            return self.memo[key]
        res = []
        items = c.items
        if i0 < k and self._starts(c, i0, first):
            res.extend(self._lead(c, i0, k, T, first))
            Fp = c.addr_at[k]
            # A && B: E(A, F') J_T(B, T) F':
            if Fp != T:
                for j in self._ends(c, i0, k):
                    for A in self.sE(c, i0, j, Fp, False, first):
                        for B in self.sJT(c, j, k, T, False, False):
                            res.append(Cond("&&", A, B))
            # A || B: E(A || B, F') goto T; F':
            g = k - 1
            if items[g].kind in ("goto", "ret") and self.taddr(items[g]) == T:
                j = self._before(c, i0, g)
                if j > i0:
                    for C in self.sE(c, i0, j, Fp, False, first):
                        if C.op == "||":
                            res.append(C)
            if top:
                for x in self.sE(c, i0, k, T, False, first):
                    if x.op in ("&&", "||"):
                        res.append(Cond("!", x))
        self.memo[key] = res
        return res

    @staticmethod
    def best(cands):
        """The longest parse; among equals, the one without a `!` and with fewer leaves."""
        if not cands:
            return None
        return max(cands, key=lambda cj: (cj[1], cj[0].op != "!", -cj[0].leaves()))

    def labels_in(self, c, a, b):
        return [c.items[k].target for k in range(a, b) if c.items[k].kind == "label"]

    def ok(self, key):
        if key in self.forbidden:
            return False
        return self.allowed is None or key in self.allowed

    # -- statements --
    def parse(self, allowed=None, forbidden=()):
        """The structured tree of the function. Constructs are keyed; `allowed` (None: all)
        and `forbidden` choose which are formed."""
        self.allowed = allowed
        self.forbidden = set(forbidden)
        while True:
            self.keys = []
            tree = self.block(self.top, 0, len(self.top.items), (None, None))
            self.remaining = collections.Counter()
            self._remaining(tree)
            bad = [nd.key for nd in self.keys
                   if any(self.remaining[l] for l in nd.absorbed)]
            # no goto into a construct from outside it
            self._entries(tree, bad)
            if not bad:
                self.tree = tree
                return tree
            self.forbidden.update(bad)

    def _entries(self, nodes, bad):
        """(labels placed in nodes, gotos in nodes); adds the constructs entered from
        outside to bad."""
        defined = set()
        gotos = collections.Counter()
        for nd in nodes:
            if nd.kind == "label":
                if self.remaining[nd.item.target]:
                    defined.add(nd.item.target)
                continue
            if nd.kind == "jump":
                if nd.jump == "goto":
                    gotos[nd.item.target] += 1
                continue
            if nd.label is not None and self.remaining[nd.label.target]:
                defined.add(nd.label.target)       # a loop's label goes before it
            d2 = set()
            g2 = collections.Counter()
            for sub in (nd.body, nd.els):
                if sub:
                    d, g = self._entries(sub, bad)
                    d2 |= d
                    g2.update(g)
            if nd.key is not None and nd.kind not in ("else", "swbrk", "markdrop", "swlead"):
                if any(self.remaining[l] > g2[l] for l in d2):
                    bad.append(nd.key)
            defined |= d2
            gotos.update(g2)
        return defined, gotos

    def _remaining(self, nodes):
        for nd in nodes:
            if nd.kind == "jump" and nd.jump == "goto":
                self.remaining[nd.item.target] += 1
            for sub in (nd.body, nd.els):
                if sub:
                    self._remaining(sub)

    def jump_node(self, it, ctx, cond=None):
        """A goto that is not part of a construct: break, continue, return or goto."""
        a = self.taddr(it)
        brk, cont = ctx
        if it.orig in ("ret", "ifret"):
            kind = "return"
        elif brk is not None and a == brk:
            kind = "break"
        elif cont is not None and a == cont:
            kind = "continue"
        else:
            kind = "goto"
        return Node("jump", item=it, jump=kind, cond=cond)

    def block(self, c, lo, hi, ctx):
        out = []
        i = lo
        items = c.items
        while i < hi:
            it = items[i]
            r = None
            if it.kind == "label":
                r = self.try_for(c, i, hi, ctx, out) or self.try_loop(c, i, hi, ctx)
                if r is None:
                    out.append(Node("label", item=it))
                    i += 1
                    continue
            elif it.kind in ("cj", "ifret"):
                r = self.try_if(c, i, hi, ctx)
                if r is None:
                    out.append(self.jump_node(it, ctx, Cond("leaf", item=it)))
                    i += 1
                    continue
            elif it.kind in ("goto", "ret"):
                r = self.try_for_nocond(c, i, hi, ctx, out)
                if r is None:
                    out.append(self.jump_node(it, ctx))
                    i += 1
                    continue
            elif it.kind == "switch":
                n = len(it.body.items)
                sb = it.body
                lo = 0
                # the lifter's jump before the first case (to the default, or to the end) is
                # the dispatch's own: Watcom emits it for a switch without it
                j0 = self.skip(sb, 0)
                if j0 < n and sb.items[j0].kind == "goto":
                    dft = [q for q, x in enumerate(sb.items) if x.kind == "case" and
                           x.text.startswith("default")]
                    a = self.taddr(sb.items[j0])
                    key = ("swlead", it.lineno)
                    if (a == sb.addr_at[n] or (dft and a == sb.addr_at[dft[0]])) and \
                            self.ok(key) and not any(x.kind == "case" for x in sb.items[:j0]):
                        self.keys.append(Node("swlead", key=key))
                        lo = j0 + 1
                key = ("swbrk", it.lineno)
                if self.ok(key):
                    # a jump to the end of the switch is a break
                    self.keys.append(Node("swbrk", key=key))
                    body = self.block(sb, lo, n, (sb.addr_at[n], ctx[1]))
                else:
                    body = self.block(sb, lo, n, (None, ctx[1]))
                out.append(Node("switch", item=it, body=body))
                i += 1
                continue
            elif it.kind == "block":
                body = self.block(it.body, 0, len(it.body.items), ctx)
                out.append(Node("block", item=it, body=body))
                i += 1
                continue
            elif it.kind == "mark":
                # the lifter's switch-table marks: structured code mostly places the table
                # where the original has it without them
                key = ("mark", it.uid)
                if self.ok(key):
                    self.keys.append(Node("markdrop", key=key))
                else:
                    out.append(Node(it.kind, item=it))
                i += 1
                continue
            elif it.kind == "comment":
                out.append(Node(it.kind, item=it))
                i += 1
                continue
            else:
                out.append(Node("stmt", item=it))
                i += 1
                continue
            nd, i = r
            self.keys.append(nd)
            out.append(nd)
        return out

    def try_if(self, c, i, hi, ctx):
        items = c.items
        e = self.run_end(c, i)
        targets = set()
        for k in range(i, e):
            if items[k].kind in ("cj", "goto", "ifret", "ret"):
                a = self.taddr(items[k])
                if a > c.addr_at[i]:
                    targets.add((a, items[k].target))
        best = None
        for a, name in sorted(targets):
            end = self.end_index(c, i, hi, a, name)
            if end is None or self.has_case(c, i, end):
                continue
            key = ("if", items[i].uid, a)
            if not self.ok(key):
                continue
            for cond, j in self.pE(c, i, a, True):
                if j > end:
                    continue
                # an early `if (c) return;` stays one
                if cond.op == "leaf" and a == self.END and \
                        items[i].orig == "ifret":
                    continue
                # the rest of a void function as an if's body only when nothing else fits:
                # early returns read better
                cand = (a != self.END, j, cond.op != "!", -cond.leaves(), -a)
                if best is None or cand > best[0]:
                    best = (cand, cond, j, a, name, end, key)
        if best is None:
            return None
        _c, cond, j, a, name, end, key = best
        absorbed = self.labels_in(c, i, j)
        # else: the then-part ends in a jump past the else-part
        t = end - 1
        while t >= j and items[t].kind in ("label", "blank"):
            t -= 1
        if t >= j and items[t].kind == "goto":
            g = self.taddr(items[t])
            gend = self.end_index(c, end, hi, g, items[t].target)
            ekey = ("else", items[i].uid, a)
            if g > a and gend is not None and gend > end and g != self.END and \
                    self.ok(ekey) and not self.has_case(c, end, gend):
                then = self.block(c, j, t, ctx)
                els = self.block(c, end, gend, ctx)
                nd = Node("if", cond=cond, body=then, els=els, key=key, absorbed=absorbed,
                          item=items[i])
                ek = Node("else", key=ekey)
                self.keys.append(ek)
                return nd, gend
        body = self.block(c, j, end, ctx)
        return Node("if", cond=cond, body=body, key=key, absorbed=absorbed,
                    item=items[i]), end

    def backrefs(self, c, i, hi, a):
        return [k for k in range(i + 1, hi)
                if c.items[k].kind in ("cj", "goto", "ifret", "ret") and
                self.taddr(c.items[k]) == a]

    def try_loop(self, c, i, hi, ctx):
        items = c.items
        T = c.addr_at[i]
        back = self.backrefs(c, i, hi, T)
        if not back:
            return None
        k = max(back)
        if self.has_case(c, i, k + 1):
            return None
        lab = items[i]
        E = c.addr_at[k + 1]
        # while (c) body: T: <c false -> E> body goto T; E:
        key = ("while", lab.uid)
        if items[k].kind == "goto" and self.ok(key):
            cond = self.best([(cd, j) for cd, j in self.pE(c, i + 1, E, True, first=False)
                              if j <= k])
            if cond:
                cd, j = cond
                body = self.block(c, j, k, (E, c.addr_at[k]))
                return Node("while", cond=cd, body=body, key=key, label=lab,
                            absorbed=self.labels_in(c, i + 1, j)), k + 1
        # do body while (c): T: body <c true -> T>; E:
        key = ("do", lab.uid)
        if self.ok(key):
            s = k
            while s - 1 > i and self.is_ctl(items[s - 1]):
                s -= 1
            for s0 in range(s, k + 1):
                if items[s0].kind in ("label", "blank"):
                    continue
                got = [(cd, j) for cd, j in self.pJT(c, s0, T, True) if j == k + 1]
                if got:
                    cd, j = self.best(got)
                    body = self.block(c, i + 1, s0, (E, c.addr_at[s0]))
                    return Node("do", cond=cd, body=body, key=key, label=lab,
                                absorbed=self.labels_in(c, s0, k + 1)), k + 1
        # for (;;) / while (1): T: body goto T; E:
        key = ("ever", lab.uid)
        if items[k].kind == "goto" and self.ok(key):
            inner_cont = any(self.taddr(items[q]) == c.addr_at[k]
                             for q in range(i + 1, k)
                             if items[q].kind in ("cj", "goto"))
            if inner_cont:
                body = self.block(c, i + 1, k, (E, c.addr_at[k]))
                return Node("ever", body=body, key=key, label=lab, head="while (1)"), k + 1
            body = self.block(c, i + 1, k, (E, T))
            return Node("ever", body=body, key=key, label=lab, head="for (;;)"), k + 1
        return None

    def _for_tail(self, c, k, hi, C, E):
        """The for body's end: the last jump back to the increment, q, with E after it."""
        back = [q for q in range(k + 1, hi)
                if c.items[q].kind in ("cj", "goto") and self.taddr(c.items[q]) == C]
        if not back:
            return None
        q = max(back)
        if c.addr_at[q + 1] != E:
            return None
        return q

    def _init(self, c, i, out, names):
        """The statement before the loop as its init, when it sets a loop variable."""
        if not out or out[-1].kind != "stmt" or i == 0 or c.items[i - 1] is not out[-1].item:
            return None
        m = re.match(r"^(\w+) = ", out[-1].item.text)
        if m and any(re.search(r"\b%s\b" % m.group(1), n) for n in names):
            return out[-1]
        return None

    def try_for(self, c, i, hi, ctx, out):
        """I; T: <c true -> B> goto E; C: N; goto T; B: body goto C; E:"""
        items = c.items
        lab = items[i]
        key = ("for", lab.uid)
        if not self.ok(key):
            return None
        T = c.addr_at[i]
        back = self.backrefs(c, i, hi, T)
        if not back:
            return None
        k = max(back)
        if items[k].kind != "goto" or k + 1 >= hi:
            return None
        B = c.addr_at[k + 1]
        got = [(cd, j) for cd, j in self.pJT(c, i + 1, B, True) if j <= k]
        if not got:
            return None
        cd, j = self.best(got)
        g = self.skip(c, j)
        if g >= k or items[g].kind not in ("goto", "ret"):
            return None
        E = self.taddr(items[g])
        if E <= B:
            return None
        incr = []
        for q in range(g + 1, k):
            if items[q].kind == "stmt":
                incr.append(items[q])
            elif items[q].kind not in ("label", "blank"):
                return None
        C = c.addr_at[g + 1]
        q = self._for_tail(c, k, hi, C, E)
        if q is None or self.has_case(c, i, q + 1):
            return None
        body = self.block(c, k + 1, q, (E, C))
        tail = items[q] if items[q].kind == "cj" else None
        texts = [cd.render("flip")] + [x.text for x in incr]
        init = self._init(c, i, out, texts)
        if init is not None:
            out.pop()
        nd = Node("for", cond=cd, body=body, key=key, label=lab, init=init, incr=incr,
                  tail=tail, absorbed=self.labels_in(c, i, k + 1))
        return nd, q + 1

    def try_for_nocond(self, c, i, hi, ctx, out):
        """I; goto B; C: N; B: body goto C; E:"""
        items = c.items
        it = items[i]
        key = ("fornc", it.uid)
        if it.kind != "goto" or not self.ok(key):
            return None
        B = self.taddr(it)
        bidx = self.end_index(c, i, hi, B, it.target)
        if bidx is None or bidx <= i + 1 or bidx >= hi:
            return None
        incr = []
        for q in range(i + 1, bidx):
            if items[q].kind == "stmt":
                incr.append(items[q])
            elif items[q].kind not in ("label", "blank"):
                return None
        if not incr:
            return None
        C = c.addr_at[i + 1]
        back = [q for q in range(bidx, hi)
                if items[q].kind in ("cj", "goto") and self.taddr(items[q]) == C]
        if not back:
            return None
        q = max(back)
        E = c.addr_at[q + 1]
        if self.has_case(c, i, q + 1):
            return None
        body = self.block(c, bidx, q, (E, C))
        tail = items[q] if items[q].kind == "cj" else None
        init = self._init(c, i, out, [x.text for x in incr])
        if init is not None:
            out.pop()
        nd = Node("for", cond=None, body=body, key=key, label=None, init=init, incr=incr,
                  tail=tail, absorbed=self.labels_in(c, i + 1, bidx))
        return nd, q + 1

    # -- output --
    def render(self, style="flip"):
        lines = [self.fn.lines[0], self.fn.lines[1]]
        lines += self.emit(self.tree, 1, style)
        lines.append(self.top.close_line)
        return "\n".join(lines)

    def cond_text(self, nd, style):
        return nd.cond.render(style)

    def emit(self, nodes, ind, style):
        pad = "    " * ind
        out = []
        for nd in nodes:
            k = nd.kind
            if k == "stmt":
                it = nd.item
                if it.kind == "blank":
                    out.append("")
                elif it.kind == "case":
                    out.append(pad[4:] + it.text)
                else:
                    out.append(pad + it.text)
            elif k == "label":
                if self.remaining[nd.item.target]:
                    out.append(nd.item.target + ":;")
            elif k == "mark":
                out.append(nd.item.line.strip())
            elif k == "comment":
                out.append(pad + nd.item.text)
            elif k == "jump":
                out.append(pad + self.jump_text(nd))
            elif k == "switch":
                out.append(pad + nd.item.head + " {")
                out += self.emit(nd.body, ind + 1, style)
                out.append(pad + "}")
            elif k == "block":
                out.append(pad + "{")
                out += self.emit(nd.body, ind + 1, style)
                out.append(pad + "}")
            elif k == "if":
                out += self.emit_if(nd, ind, style, pad)
            elif k == "while":
                out += self.loop_label(nd)
                ct = self.cond_text(nd, style)
                if os.environ.get("STRUCT_SABOTAGE") == "while":     # tests the search
                    ct = "!(%s)" % ct
                out += self.braced(pad + "while (%s)" % ct, nd.body, ind, style)
            elif k == "do" and nd.cond.op == "leaf" and self.simple(nd.body) is None and \
                    not self.emit(nd.body, ind + 1, style):
                # an empty body: `while (c);` folds to the same jump (docs/natural_c.md)
                out += self.loop_label(nd)
                out.append(pad + "while (%s);" % self.cond_text(nd, style))
            elif k == "do":
                out += self.loop_label(nd)
                out.append(pad + "do {")
                out += self.emit(nd.body, ind + 1, style)
                out.append(pad + "} while (%s);" % self.cond_text(nd, style))
            elif k == "ever":
                out += self.loop_label(nd)
                out += self.braced(pad + nd.head, nd.body, ind, style, force=True)
            elif k == "for":
                init = nd.init.item.text[:-1] if nd.init else ""
                cond = self.cond_text(nd, style) if nd.cond else ""
                incr = ", ".join(x.text[:-1] for x in nd.incr)
                body = list(nd.body)
                if nd.tail is not None:
                    body.append(Node("jump", item=nd.tail, jump="break",
                                     cond=Cond("leaf", item=nd.tail, neg=True)))
                out += self.braced(pad + "for (%s; %s; %s)" % (init, cond, incr), body, ind,
                                   style, force=True)
        return out

    def loop_label(self, nd):
        if nd.label is not None and self.remaining[nd.label.target]:
            return [nd.label.target + ":;"]
        return []

    def jump_text(self, nd):
        it = nd.item
        if nd.jump == "goto":
            j = "goto %s;" % it.target
        elif nd.jump == "return":
            j = "return;"
        else:
            j = nd.jump + ";"
        if nd.cond is not None:
            return "if (%s) %s" % (nd.cond.render("flip"), j)
        return j

    def simple(self, nodes):
        """A body that can go without braces: one plain statement."""
        live = [n for n in nodes if not (n.kind == "label" and not self.remaining[n.item.target])
                and not (n.kind == "stmt" and n.item.kind == "blank")]
        if len(live) != 1:
            return None
        n = live[0]
        if n.kind == "stmt" and n.item.kind in ("stmt", "retv"):
            return n.item.text
        if n.kind == "jump":
            return self.jump_text(n)
        return None

    def braced(self, head, body, ind, style, force=False):
        pad = "    " * ind
        one = self.simple(body)
        if one is not None and len(head) + 1 + len(one) <= MAXLINE and not force:
            return [head + " " + one]
        out = [head + " {"]
        out += self.emit(body, ind + 1, style)
        out.append(pad + "}")
        return out

    def emit_if(self, nd, ind, style, pad, chained=False):
        head = ("" if chained else pad) + "if (%s)" % self.cond_text(nd, style)
        out = []
        if not nd.els:
            if chained:
                lines = self.braced(pad + head, nd.body, ind, style, force=True)
                lines[0] = lines[0][len(pad):]
                return lines
            return self.braced(head, nd.body, ind, style)
        out.append(head + " {")
        out += self.emit(nd.body, ind + 1, style)
        live = [n for n in nd.els if not (n.kind == "label" and not self.remaining[n.item.target])]
        if len(live) == 1 and live[0].kind == "if":
            sub = self.emit_if(live[0], ind, style, pad, chained=True)
            out.append(pad + "} else " + sub[0])
            out += sub[1:]
            return out
        out.append(pad + "} else {")
        out += self.emit(nd.els, ind + 1, style)
        out.append(pad + "}")
        return out

    def raw_render(self):
        """No construct: must reproduce the function exactly."""
        self.parse(allowed=set())
        return self.render()


def all_xforms(fn):
    """The switch splits and block moves worth making in fn: all of them (found by making
    them until no new one appears), less those whose leaving out leaves no more gotos (a
    block move that changes nothing is a risk to the frame layout for nothing)."""
    xf = frozenset()
    for _ in range(10):
        st = Structurer(fn, xf)
        new = xf | frozenset(st.xform_cands)
        if new == xf:
            break
        xf = new

    def gotos(x):
        st = Structurer(fn, x)
        st.parse()
        return sum(st.remaining.values())
    if not xf:
        return xf
    n = gotos(xf)
    for k in sorted(xf, key=lambda k: (k[0] == "swend", k)):
        if k not in xf:
            continue
        rest = frozenset(x for x in xf if x != k)
        m = gotos(rest)
        if m < n or (m == n and k[0] != "swend"):
            xf, n = rest, m
    return xf


def norm(text):
    """Text without indentation, for comparing a re-indented rendering with the original."""
    return "\n".join(l.strip() for l in text.split("\n"))


def count_text(text):
    return (len(re.findall(r"\bgoto\s+\w+\s*;", text)),
            len(re.findall(r"^\s*[A-Za-z_]\w*:;?\s*$", text, re.M)) -
            len(re.findall(r"^\s*default:;?\s*$", text, re.M)))


# ---- verification -------------------------------------------------------------------------

class Verifier:
    """Compile file variants with Watcom 10.0a in batches; check the named functions."""

    def __init__(self):
        import build_fall
        import match
        from le import LE
        self.match, self.build_fall = match, build_fall
        self.tgt = match.Target()
        self.syms = match.symbol_map()
        self.le = LE(match.EXE)
        self.fix_at = {f.src_va: f for f in self.le.fixups()}
        self.gsyms = build_fall.load_symbols()
        self.flags = match.default_flags()
        self.compiles = 0
        self.errors = []

    def run(self, texts, funcs, tmp, flags=None):
        """texts: file variants; funcs: for each, the functions to check (None: all).
        Returns for each variant {func: ok} or None (compile error)."""
        import shutil
        import wcc10
        from omf import OMF
        res = []
        for lo in range(0, len(texts), 200):
            chunk = texts[lo:lo + 200]
            paths = []
            for k, t in enumerate(chunk):
                p = os.path.join(tmp, "v%05d.c" % (lo + k))
                with open(p, "w") as f:
                    f.write(t)
                paths.append(p)
            objs, td = wcc10.compile_many(paths, flags or self.flags)
            self.compiles += len(chunk)
            for k, p in enumerate(paths):
                obj = objs[p]
                if obj is None:
                    try:
                        err = open(os.path.join(td, "N%04d.ERR" % k), errors="replace").read()
                    except OSError:
                        err = ""
                    self.errors = [l for l in err.splitlines() if "Error!" in l][:8]
                    res.append(None)
                    continue
                want = funcs[lo + k]
                o = OMF(obj)
                fns = o.functions()
                r = {}
                for pub, si, off, fsize in fns:
                    name = self.build_fall.c_name(pub)
                    if name not in self.syms or (want is not None and name not in want):
                        continue
                    va, size = self.syms[name]
                    ok, _d = self.match.compare(self.tgt, o, name, va, size, quiet=True)
                    if ok:
                        lo2 = max([q + z for _p, s2, q, z in fns if s2 == si and q < off] + [0])
                        bad = self.build_fall.check_relocs(o, si, lo2, off, fsize, va, self.tgt,
                                                           self.fix_at, self.syms, self.gsyms,
                                                           self.le, {})
                        if bad is None and lo2 < off and self.tgt.bytes_at(
                                va - (off - lo2), off - lo2) != bytes(o.data[si][lo2:off]):
                            bad = "table"
                        ok = bad is None
                    r[name] = ok
                res.append(r)
            shutil.rmtree(td, ignore_errors=True)
            for p in paths:
                os.unlink(p)
        return res


def file_flags(text):
    m = re.match(r"\s*/\*\s*cflags:\s*(.*?)\s*\*/", text.split("\n", 1)[0])
    return m.group(1).split() if m else None


class FilePlan:
    """The functions of one file and their structuring state."""

    def __init__(self, path, log):
        self.path = path
        self.log = log
        self.text = open(path, encoding="latin1").read()
        self.segs = split_file(self.text)
        self.funcs = {}
        self.cache = {}
        self.xcands = {}
        self.unsupported = {}
        for s in self.segs:
            if not isinstance(s, Function):
                continue
            try:
                st = Structurer(s)
                if norm(st.raw_render()) != norm(s.text):
                    raise Unsupported("raw render differs")
                self.funcs[s.name] = st
                self.cache[s.name, frozenset()] = st
            except Unsupported as e:
                self.unsupported[s.name] = str(e)
            except RecursionError:
                self.unsupported[s.name] = "recursion"

    def get(self, name, xforms):
        key = (name, xforms)
        if key not in self.cache:
            self.cache[key] = Structurer(self.funcs[name].fn, xforms)
        return self.cache[key]

    def xforms(self, name):
        """The function's switch splits and block moves: those of the function as lifted,
        and those that appear once they are made."""
        if name not in self.xcands:
            self.xcands[name] = all_xforms(self.funcs[name].fn)
        return self.xcands[name]

    def render(self, name, allowed=None, forbidden=()):
        """The function's text with the constructs chosen by allowed (None: all) and
        forbidden; and the keys of the constructs in it."""
        xf = frozenset(k for k in self.xforms(name)
                       if (allowed is None or k in allowed) and k not in forbidden)
        st = self.get(name, xf)
        st.parse(allowed, forbidden)
        return st.render(), sorted(xf) + [nd.key for nd in st.keys]

    def text_with(self, texts):
        return join_file(self.segs, texts)


def settle(plan, ver, tmp, log):
    """Choose each function's structured form: the whole function if it matches, else the
    largest set of its constructs found to match; the original text otherwise."""
    flags = file_flags(plan.text)
    base = {}                          # the settled text of each function
    names = [n for n in plan.funcs]
    full = {}
    keys = {}
    for n in names:
        t, ks = plan.render(n)
        full[n] = t
        keys[n] = ks
    todo = [n for n in names if full[n] != plan.funcs[n].fn.text and plan.funcs[n].has_jumps]
    if not todo:
        return {}, {}
    # every function structured, one compile; and the original, for reference
    r0, rorig = ver.run([plan.text_with(full), plan.text], [None, None], tmp, flags)
    if rorig is None:
        log("the original does not compile: %s" % "; ".join(ver.errors))
        return {}, {}
    if r0 is None:
        log("compile error with every function structured: %s" % "; ".join(ver.errors))
        r0 = {}
    status = {}
    for n in todo:
        if not rorig.get(n, False):
            status[n] = "orig-fails"
            continue
        if r0.get(n):
            base[n] = full[n]
            status[n] = "full"
    fail = [n for n in todo if n not in status]
    if r0 is not None and fail:
        # a compile with only the passing ones changed, in case a failing one disturbed them
        pass
    if not fail:
        return base, status
    log("%d of %d functions match fully structured; searching %d" %
        (len(todo) - len(fail), len(todo), len(fail)))
    # 1. each construct left out
    variants, meta = [], []
    for n in fail:
        for k in keys[n]:
            t, _ = plan.render(n, forbidden={k})
            variants.append(plan.text_with(dict(base, **{n: t})))
            meta.append((n, k, t))
    res = ver.run(variants, [{m[0]} for m in meta], tmp, flags)
    without = {}
    for (n, k, t), r in zip(meta, res):
        if r and r.get(n) and n not in without:
            without[n] = (k, t)
    for n, (k, t) in without.items():
        base[n] = t
        status[n] = "all-but-1"
        log("  %s: all but %s" % (n, k))
    # 2. each construct alone
    rest = [n for n in fail if n not in without]
    variants, meta = [], []
    for n in rest:
        for k in keys[n]:
            t, _ = plan.render(n, allowed={k})
            variants.append(plan.text_with(dict(base, **{n: t})))
            meta.append((n, k, t))
    res = ver.run(variants, [{m[0]} for m in meta], tmp, flags) if variants else []
    alone = collections.defaultdict(list)
    for (n, k, t), r in zip(meta, res):
        if r and r.get(n):
            alone[n].append(k)
    grow = []
    for n in rest:
        if alone[n]:
            grow.append(n)
        else:
            status[n] = "raw"
    # 2. grow a set from the constructs that match alone: the longest passing prefix, drop
    #    the construct after it, go on
    acc = {n: [] for n in grow}
    rest = {n: list(alone[n]) for n in grow}
    while any(rest[n] for n in grow):
        variants, meta = [], []
        for n in grow:
            for m in range(1, len(rest[n]) + 1):
                al = set(acc[n] + rest[n][:m])
                t, _ = plan.render(n, allowed=al)
                variants.append(plan.text_with(dict(base, **{n: t})))
                meta.append((n, m, t))
        res = ver.run(variants, [{m[0]} for m in meta], tmp, flags)
        ok = collections.defaultdict(int)
        okt = {}
        for (n, m, t), r in zip(meta, res):
            if r and r.get(n) and ok[n] == m - 1:
                ok[n] = m
                okt[n] = t
        for n in grow:
            m = ok[n]
            acc[n] += rest[n][:m]
            rest[n] = rest[n][m + 1:]
            if m:
                base[n] = okt[n]
    for n in grow:
        if acc[n]:
            t, _ = plan.render(n, allowed=set(acc[n]))
            base[n] = t
            status[n] = "partial"
            log("  %s: %d of %d constructs" % (n, len(acc[n]), len(keys[n])))
        else:
            status[n] = "raw"
    return base, status


def process(path, out, log=print, keep_tmp=False):
    import tempfile
    t0 = time.time()
    plan = FilePlan(path, log)
    for n, why in plan.unsupported.items():
        log("  %s: not parsed (%s)" % (n, why))
    ver = Verifier()
    tmp = tempfile.mkdtemp(prefix="struct_", dir=os.environ.get("STRUCT_TMP"))
    base, status = settle(plan, ver, tmp, log)
    text = plan.text_with(base)
    # the whole file once more: every function that matched before must match now
    flags = file_flags(plan.text)
    if base:
        final, orig = ver.run([text, plan.text], [None, None], tmp, flags)
        bad = [n for n, ok in (orig or {}).items() if ok and not (final or {}).get(n)]
        if final is None or bad:
            log("FINAL CHECK FAILED (%s); reverting %s" % (
                "compile error" if final is None else ", ".join(bad),
                "the file" if final is None else "those"))
            if final is None:
                base = {}
            else:
                for n in bad:
                    base.pop(n, None)
                    status[n] = "reverted"
            text = plan.text_with(base)
            if base:
                final, = ver.run([text], [None], tmp, flags)
                bad = [n for n, ok in (orig or {}).items() if ok and not (final or {}).get(n)]
                if final is None or bad:
                    log("still failing: reverting the file")
                    text = plan.text
                    base = {}
    if not keep_tmp:
        import shutil
        shutil.rmtree(tmp, ignore_errors=True)
    with open(out, "w", encoding="latin1") as f:
        f.write(text)
    g0, l0 = count_text(plan.text)
    g1, l1 = count_text(text)
    st = collections.Counter(status.values())
    dt = time.time() - t0
    log("%s: gotos %d -> %d, labels %d -> %d; functions %s; %d compiles, %.0fs" % (
        os.path.basename(path), g0, g1, l0, l1, dict(st), ver.compiles, dt))
    return dict(file=path, gotos0=g0, gotos1=g1, labels0=l0, labels1=l1, compiles=ver.compiles,
                seconds=round(dt, 1), unsupported=len(plan.unsupported), **{
                    "fn_" + k: v for k, v in st.items()})


def _worker(args):
    path, out = args
    lines = []
    try:
        r = process(path, out, log=lambda m: lines.append(m))
    except Exception as e:  # noqa: BLE001
        import traceback
        lines.append("ERROR %s: %s" % (path, traceback.format_exc()))
        r = dict(file=path, error=str(e))
    return r, lines


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    sub = ap.add_subparsers(dest="cmd")
    sp = sub.add_parser("show")
    sp.add_argument("file")
    sp.add_argument("--func")
    sp.add_argument("--style", default="flip")
    rp = sub.add_parser("run")
    rp.add_argument("files", nargs="+")
    rp.add_argument("-o", dest="outdir")
    rp.add_argument("--in-place", action="store_true")
    rp.add_argument("-j", type=int, default=1)
    rp.add_argument("--report")
    cp = sub.add_parser("count")
    cp.add_argument("files", nargs="+")
    tp = sub.add_parser("selftest")
    tp.add_argument("files", nargs="+")
    a = ap.parse_args()
    if a.cmd == "show":
        text = open(a.file, encoding="latin1").read()
        for s in split_file(text):
            if not isinstance(s, Function) or (a.func and s.name != a.func):
                continue
            try:
                st = Structurer(s)
                st = Structurer(s, all_xforms(s))
                st.parse()
                print(st.render(a.style))
            except Unsupported as e:
                print("/* %s: not parsed: %s */" % (s.name, e))
                print(s.text)
            print()
        return
    if a.cmd == "count":
        tg = tl = 0
        for f in a.files:
            g, l = count_text(open(f, encoding="latin1").read())
            tg += g
            tl += l
            print("%5d %5d  %s" % (g, l, f))
        print("%5d %5d  total (gotos, labels)" % (tg, tl))
        return
    if a.cmd == "selftest":
        bad = 0
        for f in a.files:
            text = open(f, encoding="latin1").read()
            for s in split_file(text):
                if not isinstance(s, Function):
                    continue
                try:
                    st = Structurer(s)
                    if norm(st.raw_render()) != norm(s.text):
                        bad += 1
                        print("DIFFERS", f, s.name)
                    st = Structurer(s, all_xforms(s))
                    st.parse()
                    st.render()
                except Unsupported as e:
                    print("unsupported", f, s.name, e)
        print("bad:", bad)
        return
    if a.cmd == "run":
        if not a.in_place and not a.outdir:
            raise SystemExit("give -o DIR or --in-place")
        jobs = []
        for f in a.files:
            out = f if a.in_place else os.path.join(a.outdir, os.path.basename(f))
            jobs.append((f, out))
        if a.outdir:
            os.makedirs(a.outdir, exist_ok=True)
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
        g0 = sum(r.get("gotos0", 0) for r in rows)
        g1 = sum(r.get("gotos1", 0) for r in rows)
        l0 = sum(r.get("labels0", 0) for r in rows)
        l1 = sum(r.get("labels1", 0) for r in rows)
        print("TOTAL: gotos %d -> %d, labels %d -> %d, %d files" % (g0, g1, l0, l1, len(rows)))
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
