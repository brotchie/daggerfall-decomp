#!/usr/bin/env python3
"""Disassemble FALL.EXE's object 2 (the XnGine engine) into MASM modules under src/xngine/
that reassemble to the original bytes and fixups (checked by tools/xn_link.py).

  1. Code: recursive descent from the object 2 functions in config/functions.csv and the
     calls they make, through jump tables (`jmp [reg*4 + table]`, read via the LE fixups).
     Everything else is data.
  2. Symbols: every LE fixup in object 2 becomes a symbol expression. Targets in object 2 get
     labels: func_X at function starts, L_X at other code, D_X in data. A target inside an
     instruction (a patch field of the self-modifying code) is written `label + k`. Targets in
     other objects, or in another module, are externs func_X / D_X, which the build resolves by
     address.
  3. Modules: object 2 is cut at 0x100-aligned addresses that follow zero padding
     (config/xngine_modules.csv; --split rewrites it).
  4. Text: each instruction is capstone's, turned into MASM syntax. Plain data bytes become
     B_<address>_<length> blobs that the build fills from the original (no game bytes in the
     repo); zero runs are `db n dup (0)`; pointers are `dd symbol`.
  5. Encodings: a probe pass assembles every module with an `org` back to the original offset
     after each instruction, so a mis-encoded instruction cannot shift the rest. Each one whose
     bytes differ, or that wasm rejects, is written as `db` bytes with its symbol fields as
     `dd` expressions from then on, and the pass repeats until nothing changes. The final files
     have no `org`s and are checked whole.

usage: xn_disasm.py [--split] [module ...]
"""
import argparse
import bisect
import csv
import os
import re
import sys

import capstone
from capstone import x86 as cx

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import xn_link  # noqa: E402
from le import SRC_OFF32, SRC_REL32, SRC_SEL16  # noqa: E402

ROOT = xn_link.ROOT
PROBE_DIR = os.path.join(ROOT, "build", "xngine", "probe")

STRING_OPS = {"movsb", "movsw", "movsd", "stosb", "stosw", "stosd", "lodsb", "lodsw",
              "lodsd", "scasb", "scasw", "scasd", "cmpsb", "cmpsw", "cmpsd", "insb", "insw",
              "insd", "outsb", "outsw", "outsd"}
SEG_PREFIXES = {0x26, 0x2E, 0x36, 0x3E, 0x64, 0x65}
MNEMONIC = {"pushal": "pushad", "popal": "popad"}


def h(v):
    """MASM hex literal."""
    return "%s0%Xh" % ("-" if v < 0 else "", abs(v))


def branch_target(i):
    """Target of a direct jump, call or loop, else None."""
    g = i.groups
    if (cx.X86_GRP_JUMP in g or cx.X86_GRP_CALL in g or cx.X86_GRP_BRANCH_RELATIVE in g) \
            and i.operands and i.operands[0].type == cx.X86_OP_IMM:
        return i.operands[0].imm & 0xFFFFFFFF
    return None


class Analysis:
    def __init__(self, img):
        self.img = img
        le = img.le
        self.lo, self.hi = img.obj.base, img.obj.base + img.obj.vsize
        self.rel = le.load(relocate=True)[xn_link.OBJ2]   # decoded text shows real targets
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.md.detail = True
        with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
            rows = list(csv.DictReader(f))
        self.all_funcs = {int(r["va"], 16) for r in rows}
        self.funcs = sorted(int(r["va"], 16) for r in rows if r["obj"] == str(xn_link.OBJ2))
        self.fields = {}            # field va -> LE fixup, for fields in object 2
        for fx in img.fixups:
            if self.lo <= fx.src_va < self.hi:
                self.fields[fx.src_va] = fx
        self.field_bytes = {}
        for fx in self.fields.values():
            for k in range(fx.size):
                self.field_bytes[fx.src_va + k] = fx.src_va
        self.insns = {}
        self.tables = set()          # bytes of jump tables
        self.code_labels = set()     # branch and call targets
        # code seen at run time that no static rule reaches (tools/xn_trace.py)
        p = os.path.join(ROOT, "config", "xngine_seeds.csv")
        if os.path.exists(p):
            with open(p, newline="") as f:
                self.runtime_seeds = [int(r["va"], 16) for r in csv.DictReader(f)]
            self.funcs = sorted(set(self.funcs) | set(self.runtime_seeds))
        else:
            self.runtime_seeds = []
        # Discovery. An address instructions access as memory is data (or a patch field),
        # never a function entry; when a pointer found a "function" there, start over without.
        seeds = list(self.funcs)
        self.not_code = set()
        while True:
            self.funcs = list(seeds)
            self.insns, self.tables, self.code_labels = {}, set(), set()
            self.gap_funcs, self.ptr_funcs = [], []
            work = list(self.funcs)
            while work:
                self.descend(work)
                work = self.gap_fill() or self.pointer_seeds() or self.after_ret()
            accessed = self.memory_targets()
            wrong = {t for t in self.ptr_funcs if t in accessed} - self.not_code
            if not wrong:
                break
            self.not_code |= wrong
        self.funcs = sorted(set(self.funcs))
        self.func_set = set(self.funcs)
        self.starts = sorted(self.insns)
        self.covered = {}
        for a in self.starts:
            for k in range(self.insns[a].size):
                self.covered.setdefault(a + k, a)
        self.find_patches()

    def find_patches(self):
        """The self-modifying code: every instruction that writes through an address fixup
        into code. self.patches = [(writer va, field va, patched insn va)]; a field inside an
        instruction (an operand) is named patch_<field> (self.patch_fields)."""
        self.patches = []
        self.patch_fields = set()
        for a in self.starts:
            i = self.insns[a]
            for op in i.operands:
                if op.type != cx.X86_OP_MEM or not op.access & capstone.CS_AC_WRITE or \
                        not i.disp_offset:
                    continue
                fx = self.fields.get(a + i.disp_offset)
                if fx is None or fx.kind != SRC_OFF32 or fx.target_va not in self.covered:
                    continue
                base = self.covered[fx.target_va]
                self.patches.append((a, fx.target_va, base))
                if fx.target_va != base and fx.target_va not in self.insns:
                    self.patch_fields.add(fx.target_va)

    def func_of(self, va):
        k = bisect.bisect_right(self.funcs, va) - 1
        return self.funcs[k] if k >= 0 else None

    def decode(self, va):
        o = va - self.lo
        return next(self.md.disasm(bytes(self.rel[o:o + 16]), va), None)

    def whole_fields(self, i):
        """An instruction holds a fixup field whole or not at all; one that cuts a field is
        data being decoded as code."""
        for k in range(i.size):
            f = self.field_bytes.get(i.address + k)
            if f is not None and (f < i.address or
                                  f + self.fields[f].size > i.address + i.size):
                return False
        return True

    def gap_fill(self):
        """Code nothing reaches by a direct call: an undecoded call into another object (an
        LE rel32 fixup) is code. Decode from the start of its gap (after zero padding); if
        that lands on the call exactly, the gap start is a function."""
        found = []
        starts = sorted(self.insns)
        ends = sorted(a + self.insns[a].size for a in starts)
        for f in sorted(self.fields):
            fx = self.fields[f]
            call = f - 1
            if fx.kind != SRC_REL32 or call in self.insns or \
                    self.img.raw[call - self.lo] not in (0xE8, 0xE9):
                continue
            k = bisect.bisect_right(ends, call) - 1
            gap = ends[k] if k >= 0 else self.lo
            # the earliest start after a boundary (zero padding, a ret, the end of a fixup
            # field) that decodes cleanly onto the call; the gap may begin with data
            for s in range(gap, call):
                prev = self.img.raw[s - 1 - self.lo] if s > self.lo else 0
                if self.img.raw[s - self.lo] == 0 or s in self.insns or not (
                        s == gap or prev in (0, 0xC3) or s - 4 in self.fields):
                    continue
                va = s
                while va < call:
                    i = self.decode(va)
                    if i is None or not self.whole_fields(i) or i.bytes[:2] == b"\0\0" or \
                            i.mnemonic.split()[-1] in self.UNLIKELY:
                        break
                    va += i.size
                if va == call:
                    found.append(s)
                    self.gap_funcs.append(s)
                    self.funcs.append(s)
                    break
        return found

    # instructions hand-written engine code does not use but text and tables decode to
    UNLIKELY = {"arpl", "bound", "into", "das", "daa", "aaa", "aas", "aam", "aad", "salc",
                "insb", "insw", "insd", "outsb", "outsw", "outsd", "lds", "les", "lfs", "lgs",
                "lss", "retf", "ljmp", "lcall", "hlt", "int3", "icebp", "wait", "fwait",
                "sahf", "lahf", "xlatb", "cmc", "std", "enter", "leave", "pushfd", "popfd",
                "iretd", "int1", "ud2", "ud1", "ud0", "in", "out"}

    def looks_like_code(self, va, limit=3000):
        """Decode from va along fall-through and local branches: every path must end in
        ret or jmp, with no decode errors, cut fixup fields or unlikely instructions."""
        seen, work = set(), [va]
        while work:
            p = work.pop()
            while True:
                if p in seen or p in self.insns:
                    break
                if not self.lo <= p < self.hi or len(seen) > limit:
                    return False
                i = self.decode(p)
                if i is None or not self.whole_fields(i) or i.mnemonic in self.UNLIKELY or \
                        i.mnemonic.split()[-1] in self.UNLIKELY or i.bytes[:2] == b"\0\0":
                    return False
                seen.add(p)
                g = i.groups
                if cx.X86_GRP_RET in g:
                    break
                t = branch_target(i)
                if t is not None and cx.X86_GRP_CALL not in g:
                    work.append(t)
                if i.mnemonic == "jmp":
                    break
                p += i.size
        return True

    def skip_padding(self, s):
        """Past alignment fill: zeros, nop (90) and the two-byte xchg ebx,ebx (87 DB)."""
        raw, lo = self.img.raw, self.lo
        while s < self.hi:
            if raw[s - lo] in (0x00, 0x90):
                s += 1
            elif raw[s - lo] == 0x87 and s + 1 < self.hi and raw[s + 1 - lo] == 0xDB:
                s += 2
            else:
                break
        return s

    def after_ret(self):
        """Code nothing references directly, right after a ret or jmp (past zero padding):
        accepted when it decodes as code (looks_like_code)."""
        found = []
        for a, i in list(self.insns.items()):
            if not (cx.X86_GRP_RET in i.groups or i.mnemonic == "jmp"):
                continue
            s = self.skip_padding(a + i.size)
            if s >= self.hi or s in self.insns or self.covered_by_code(s) or \
                    s in self.not_code or s in found or not self.looks_like_code(s):
                continue
            found.append(s)
        self.gap_funcs += found
        self.funcs += found
        return found

    def memory_targets(self):
        """Addresses decoded instructions read or write through an address fixup."""
        out = set()
        for a, i in self.insns.items():
            if i.disp_offset and any(op.type == cx.X86_OP_MEM for op in i.operands):
                fx = self.fields.get(a + i.disp_offset)
                if fx is not None and fx.kind == SRC_OFF32:
                    out.add(fx.target_va)
        return out

    def pointer_seeds(self):
        """Code reached only through pointers: dispatch tables and callback variables. An
        address fixup in data whose target decodes as code (looks_like_code) is a function."""
        found = []
        for fx in self.img.fixups:
            t, src = fx.target_va, fx.src_va
            if fx.kind != SRC_OFF32 or not self.lo <= t < self.hi or t in self.insns or \
                    self.covered_by_code(t) or not self.lo <= src < self.hi:
                continue
            if self.covered_by_code(src):
                # in code only `offset func` immediates; memory operands read or patch
                a = self._starts[bisect.bisect_right(self._starts, src) - 1]
                i = self.insns[a]
                if not i.imm_offset or src != a + i.imm_offset:
                    continue
            if t in found or t in self.not_code or not self.looks_like_code(t):
                continue
            found.append(t)
        self.ptr_funcs += found
        self.funcs += found
        return found

    def covered_by_code(self, va):
        k = bisect.bisect_right(self._starts_cache(), va) - 1
        if k < 0:
            return False
        a = self._starts[k]
        return a <= va < a + self.insns[a].size

    def _starts_cache(self):
        if getattr(self, "_starts_n", None) != len(self.insns):
            self._starts = sorted(self.insns)
            self._starts_n = len(self.insns)
        return self._starts

    def descend(self, work):
        seen_funcs = set(self.funcs)
        while work:
            va = work.pop()
            while self.lo <= va < self.hi and va not in self.insns and va not in self.tables:
                i = self.decode(va)
                if i is None or not self.whole_fields(i):
                    break
                self.insns[va] = i
                g = i.groups
                if cx.X86_GRP_RET in g or cx.X86_GRP_IRET in g or i.mnemonic == "hlt":
                    break
                if cx.X86_GRP_JUMP in g or cx.X86_GRP_CALL in g or \
                        cx.X86_GRP_BRANCH_RELATIVE in g:
                    op = i.operands[0]
                    if op.type == cx.X86_OP_IMM and self.lo <= op.imm < self.hi:
                        self.code_labels.add(op.imm)
                        work.append(op.imm)
                        if cx.X86_GRP_CALL in g and op.imm not in seen_funcs:
                            seen_funcs.add(op.imm)
                            self.funcs.append(op.imm)
                    elif op.type == cx.X86_OP_MEM and op.mem.index and op.mem.scale == 4 and \
                            cx.X86_GRP_JUMP in g:
                        t = op.mem.disp & 0xFFFFFFFF
                        k = 0
                        while True:
                            fx = self.img.fix_at.get(t + 4 * k)
                            if fx is None or fx.kind != SRC_OFF32 or \
                                    not self.lo <= fx.target_va < self.hi:
                                break
                            for b in range(4):
                                self.tables.add(t + 4 * k + b)
                            self.code_labels.add(fx.target_va)
                            work.append(fx.target_va)
                            k += 1
                    if i.mnemonic == "jmp":
                        break
                va += i.size

    def insn_at(self, va):
        """Start of the instruction holding va, or None."""
        return self.covered.get(va)

    def split(self):
        """Module boundaries: 0x100-aligned addresses after zero padding where something
        starts (code, a referenced label, or non-zero data)."""
        targets = {fx.target_va for fx in self.img.fixups}
        cuts = [self.lo]
        for a in range(self.lo + 0x100, self.hi, 0x100):
            if a in self.covered and self.covered[a] != a or a in self.field_bytes and \
                    self.field_bytes[a] != a:
                continue
            if any(self.img.raw[a - self.lo - 16:a - self.lo]):
                continue
            if a in self.insns or a in targets or self.img.raw[a - self.lo]:
                cuts.append(a)
        cuts.append(self.hi)
        return [("xn_%05X" % s, s, e) for s, e in zip(cuts, cuts[1:])]


class Module:
    def __init__(self, an, name, start, end, publics=()):
        self.an, self.name, self.start, self.end = an, name, start, end
        self.publics = set(publics)  # labels other modules use
        self.externs = {}         # name -> kind
        self.labels = {}          # va -> name, positions in this module that need a label
        self.patch_equ = {}       # patch_X -> expression, for fields in other modules

    def local_name(self, va):
        an = self.an
        if va in an.func_set:
            return "func_%08X" % va
        if va in an.insns:
            return "L_%06X" % va
        return "D_%08X" % va

    def sym(self, t):
        """Symbol expression for linear address t."""
        an = self.an
        if t in an.patch_fields:
            # an operand the self-modifying code rewrites: named, defined next to its
            # instruction (or here from that instruction's public label)
            expr = self.sym_plain(t)
            if not self.start <= t < self.end:
                self.patch_equ["patch_%06X" % t] = expr
            return "patch_%06X" % t
        return self.sym_plain(t)

    def sym_plain(self, t):
        an = self.an
        if self.start <= t < self.end or t == self.end == an.hi:
            if t in an.insns or t not in an.covered and t not in an.field_bytes:
                n = self.local_name(t)
                self.labels[t] = n
                return n
            base = an.covered.get(t, an.field_bytes.get(t))
            n = self.local_name(base)
            self.labels[base] = n
            return "%s+%d" % (n, t - base)
        if an.lo <= t < an.hi:
            # another module: its label for the item holding t, which it makes public
            base = t if t in an.insns or t not in an.covered and t not in an.field_bytes \
                else an.covered.get(t, an.field_bytes.get(t))
            n = self.local_name(base)
            code = base in an.insns
        else:
            base, n = t, "func_%08X" % t if t in an.all_funcs else "D_%08X" % t
            code = t in an.all_funcs
        if code or self.externs.get(n) == "near":
            self.externs[n] = "near"
        else:
            self.externs[n] = "byte"
        return n if base == t else "%s+%d" % (n, t - base)

    # -- instructions -----------------------------------------------------------------
    def text(self, i):
        """MASM text for capstone instruction i, or None when it must be written as bytes."""
        an = self.an
        fields = [f for f in range(i.address, i.address + i.size) if f in an.fields]
        m, ops = i.mnemonic, i.op_str
        m = MNEMONIC.get(m, m)
        prefix_bytes = []
        for b in i.bytes:
            if b in (0xF0, 0xF2, 0xF3, 0x66, 0x67) or b in SEG_PREFIXES:
                prefix_bytes.append(b)
            else:
                break
        words = m.split()
        if words[-1] in STRING_OPS:
            if any(b in SEG_PREFIXES for b in prefix_bytes) or fields:
                return None
            return m
        g = i.groups
        if branch_target(i) is not None:
            t = branch_target(i)
            fx = an.fields.get(i.address + i.size - 4)
            if fx is not None:
                if fx.kind != SRC_REL32:
                    return None
                t = fx.target_va
            s = self.sym(t)
            if cx.X86_GRP_CALL in g:
                return "call " + s
            if m in ("loop", "loope", "loopne", "jecxz", "jcxz"):
                return "%s %s" % (m, s)
            return "%s %s %s" % (m, "short" if i.size == 2 else "near ptr", s)
        if any(an.fields[f].kind != SRC_OFF32 for f in fields):
            return None
        operands = ops.split(", ") if ops else []
        if len(operands) != len(i.operands):
            return None
        out = []
        used = set()
        for k, (txt, op) in enumerate(zip(operands, i.operands)):
            if op.type == cx.X86_OP_MEM:
                f = i.address + i.disp_offset if i.disp_offset else None
                if f in an.fields:
                    used.add(f)
                    t = an.fields[f].target_va
                    num = re.search(r"\[0x[0-9a-f]+\]", txt) or re.search(r"([+-] )0x[0-9a-f]+\]", txt)
                    if not num:
                        return None
                    if num.group(0).startswith("["):
                        txt = txt[:num.start()] + "[" + self.sym(t) + "]" + txt[num.end():]
                    else:
                        txt = txt[:num.start()] + "+ " + self.sym(t) + "]" + txt[num.end():]
                elif not op.mem.base and not op.mem.index and ":" not in txt:
                    txt = txt.replace("[", "ds:[")
            elif op.type == cx.X86_OP_IMM:
                f = i.address + i.imm_offset if i.imm_offset else None
                if f in an.fields:
                    used.add(f)
                    txt = "offset " + self.sym(an.fields[f].target_va)
            out.append(txt)
        if set(fields) != used:
            return None
        s = (m + " " + ", ".join(out)).strip()
        s = s.replace("xword ptr", "tbyte ptr")
        s = re.sub(r"(-?)0x([0-9a-f]+)", lambda mm: h(int(mm.group(1) + mm.group(2), 16)), s)
        if re.fullmatch(r"f\w*p st\(\d\)", s):
            s += ", st"
        return s

    def as_bytes(self, i):
        """The instruction as db/dd lines, keeping symbol fields symbolic."""
        an = self.an
        a, n, raw = i.address, i.size, an.img.bytes_at(i.address, i.size)
        lines, run = [], []
        rel = None   # (offset, size) of an intra-object branch displacement
        if branch_target(i) is not None and (a + n - 4) not in an.fields:
            t = branch_target(i)
            size = 1 if n == 2 or i.bytes[0] in (0xE0, 0xE1, 0xE2, 0xE3) or \
                0x70 <= i.bytes[0] <= 0x7F or i.bytes[0] == 0xEB else 4
            if self.start <= t < self.end:
                rel = (n - size, size, self.sym(t))
        k = 0

        def flush():
            if run:
                lines.append("db " + ", ".join(h(b) for b in run))
                del run[:]
        while k < n:
            f = a + k
            if rel and k == rel[0]:
                flush()
                lines.append("%s %s - ($ + %d)" % ("db" if rel[1] == 1 else "dd", rel[2], rel[1]))
                k += rel[1]
            elif f in an.fields and an.fields[f].kind == SRC_OFF32:
                flush()
                lines.append("dd " + self.sym(an.fields[f].target_va))
                k += 4
            elif f in an.fields and an.fields[f].kind == SRC_REL32:
                return None   # a call into object 1: only `call` can say that
            else:
                run.append(raw[k])
                k += 1
        flush()
        return lines

    # -- data --------------------------------------------------------------------------
    def data(self, a, b, out):
        """Lines for the data bytes [a, b), split at labels and fixup fields."""
        an = self.an
        raw = an.img.bytes_at(a, b - a)
        p = a
        while p < b:
            if p in self.labels and p != a:
                return p     # caller emits the label and continues
            call = an.fields.get(p + 1)
            if call is not None and call.kind == SRC_REL32 and raw[p - a] in (0xE8, 0xE9) and \
                    p + 5 <= b:
                # a call into another object in bytes nothing reaches: only `call` can say it
                out.append("    %s %s" % ("call" if raw[p - a] == 0xE8 else "jmp near ptr",
                                          self.sym(call.target_va)))
                p += 5
                continue
            fx = an.fields.get(p)
            if fx is not None and fx.kind == SRC_OFF32:
                out.append("    dd " + self.sym(fx.target_va))
                p += 4
                continue
            if fx is not None:
                out.append("    db " + ", ".join(h(x) for x in raw[p - a:p - a + fx.size]) +
                           "   ; selector fixup, kept by the LE table")
                p += fx.size
                continue
            q = p
            while q < b and q not in an.fields and not (q in self.labels and q != p) and \
                    not (q + 1 in an.fields and an.fields[q + 1].kind == SRC_REL32):
                q += 1
            if q == p:      # the byte before a rel32 field that is not a call
                q += 1
            chunk = raw[p - a:q - a]
            k = 0
            while k < len(chunk):
                z = k
                while z < len(chunk) and chunk[z] == 0:
                    z += 1
                if z - k >= 8 or z == len(chunk) and z > k:
                    out.append("    db %d dup (0)" % (z - k))
                    k = z
                    continue
                e = k
                while e < len(chunk):
                    z = e
                    while z < len(chunk) and chunk[z] == 0:
                        z += 1
                    if z - e >= 8:
                        break
                    e = max(z, e + 1)
                out.append("    B_%06X_%d" % (p + k, e - k))
                k = e
            p = q
        return b

    # -- whole module ------------------------------------------------------------------
    def emit(self, fallback, probe=False):
        """(source text, {line number: instruction va})."""
        an = self.an
        # labels first: function starts, branch targets and fixup targets in this module
        for t in an.funcs + sorted(an.code_labels):
            if self.start <= t < self.end:
                self.labels[t] = self.local_name(t)
        for fx in an.img.fixups:
            if self.start <= fx.target_va < self.end or fx.target_va == self.end == an.hi:
                self.sym(fx.target_va)
        sorted_labels = sorted(self.labels)
        body, linemap = [], {}
        a = self.start
        while a < self.end:
            if a in self.labels:
                body.append("%s:" % self.labels[a])
            if a in an.insns and an.insns[a].address + an.insns[a].size <= self.end:
                i = an.insns[a]
                j = bisect.bisect_right(sorted_labels, a)
                inner = j < len(sorted_labels) and sorted_labels[j] < a + i.size
                s = None if a in fallback or inner else self.text(i)
                if inner:
                    # a label inside the instruction (code that jumps into it): bytes with
                    # the labels between them
                    for k in range(i.size):
                        if k and a + k in self.labels:
                            body.append("%s:" % self.labels[a + k])
                        linemap[len(body) + 1] = a
                        body.append("    db %s" % h(an.img.bytes_at(a + k, 1)[0]))
                    if probe:
                        body.append("    org %s" % h(a + i.size - self.start))
                    a += i.size
                    continue
                if s is not None:
                    linemap[len(body) + 1] = a
                    body.append("    " + s)
                else:
                    lines = self.as_bytes(i)
                    if lines is None:
                        lines = ["db " + ", ".join(h(b) for b in an.img.bytes_at(a, i.size))]
                    note = "   ; %s %s" % (i.mnemonic, i.op_str)
                    for k, ln in enumerate(lines):
                        linemap[len(body) + 1] = a
                        body.append("    " + ln + (note if k == 0 else ""))
                for f in range(a + 1, a + i.size):
                    if f in an.patch_fields:
                        body.append("patch_%06X equ %s+%d   ; rewritten at run time" % (
                            f, self.labels[a], f - a))
                if probe:
                    body.append("    org %s" % h(a + i.size - self.start))
                a += i.size
                continue
            b = a + 1
            while b < self.end and b not in an.insns:
                b += 1
            a = self.data(a, b, body)
        if self.end in self.labels:
            body.append("%s:" % self.labels[self.end])
        seg = "XN_%05X" % self.start
        head = [
            "; XnGine module %s: FALL.EXE object 2, %#x-%#x." % (self.name, self.start, self.end),
            "; Generated by tools/xn_disasm.py; reassembles to the original (tools/xn_link.py).",
            "; B_<address>_<length> lines are data bytes the build takes from your FALL.EXE.",
            ".486p",
            ".387",
            "include %s.inc" % self.name,
        ]
        for n in sorted(self.publics):
            head.append("public %s" % n)
        for n in sorted(self.externs):
            head.append("extrn %s:%s" % (n, self.externs[n]))
        for n in sorted(self.patch_equ):
            head.append("%s equ %s" % (n, self.patch_equ[n]))
        head += ["%s segment byte public use32 'CODE'" % seg,
                 "    assume cs:%s, ds:%s, es:%s, ss:%s" % ((seg,) * 4)]
        tail = ["%s ends" % seg, "end"]
        off = len(head)
        return "\n".join(head + body + tail) + "\n", {k + off: v for k, v in linemap.items()}


ERR = re.compile(r"\((\d+)\): Error")


def bad_insns(mod, data, problems, msgs, linemap):
    """Instruction addresses to write as bytes: rejected lines and differing bytes."""
    an = mod.an
    bad = set()
    for m in ERR.finditer(msgs):
        va = linemap.get(int(m.group(1)))
        if va is not None:
            bad.add(va)
    for va, _why in problems:
        s = an.insn_at(va)
        if s is not None:
            bad.add(s)
    if data:
        want = an.img.bytes_at(mod.start, mod.end - mod.start)
        for k in range(min(len(data), len(want))):
            if data[k] != want[k]:
                s = an.insn_at(mod.start + k)
                if s is not None:
                    bad.add(s)
    return bad


def write_functions(an):
    """config/xngine_functions.csv: every function in object 2, its code bytes (up to the next
    function) and how it was found."""
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        listed = {int(r["va"], 16) for r in csv.DictReader(f) if r["obj"] == str(xn_link.OBJ2)}
    gap, ptr = set(an.gap_funcs), set(an.ptr_funcs)
    funcs = an.funcs
    with open(os.path.join(ROOT, "config", "xngine_functions.csv"), "w", newline="") as fh:
        w = csv.writer(fh, lineterminator="\n")
        w.writerow(["va", "code_bytes", "found_by"])
        for k, f in enumerate(funcs):
            nxt = funcs[k + 1] if k + 1 < len(funcs) else an.hi
            j = bisect.bisect_left(an.starts, f)
            size = 0
            while j < len(an.starts) and an.starts[j] < nxt:
                size += an.insns[an.starts[j]].size
                j += 1
            how = "functions.csv" if f in listed else "pointer" if f in ptr else \
                "unreferenced" if f in gap else "call"
            w.writerow(["0x%08X" % f, size, how])


def write_patches(an):
    """config/xngine_patches.csv: every write into code, with the instruction it changes."""
    rows = []
    for w, f, base in sorted(an.patches, key=lambda p: (p[1], p[0])):
        i, wi = an.insns[base], an.insns[w]
        k = f - base
        size = next(op.size for op in wi.operands if op.type == cx.X86_OP_MEM)
        orig = int.from_bytes(an.img.bytes_at(f, size), "little")
        # placeholders are round numbers: 100, 1000, 10000, 100000
        if k == 0:
            kind = "opcode"         # a ret planted in an unrolled loop, or restored
        elif k == i.disp_offset and k != i.imm_offset:
            kind = "address"        # a memory operand pointed elsewhere
        elif orig in (100, 1000, 10000, 100000):
            kind = "placeholder"
        else:
            kind = "operand"
        rows.append(["0x%08X" % f, "0x%08X" % base, "0x%08X" % an.func_of(base),
                     "%s %s" % (i.mnemonic, i.op_str), "0x%0*X" % (2 * size, orig), kind,
                     "0x%08X" % w, "0x%08X" % an.func_of(w), "%s %s" % (wi.mnemonic, wi.op_str)])
    with open(os.path.join(ROOT, "config", "xngine_patches.csv"), "w", newline="") as fh:
        wr = csv.writer(fh, lineterminator="\n")
        wr.writerow(["field", "insn", "func", "insn_text", "original", "kind", "writer",
                     "writer_func", "writer_text"])
        wr.writerows(rows)
    kinds = {}
    for r in rows:
        kinds[r[5]] = kinds.get(r[5], 0) + 1
    print("self-modifying code: %d writes from %d functions into %d instructions of %d "
          "functions (%s)" % (len(rows), len({r[7] for r in rows}), len({r[1] for r in rows}),
                              len({r[2] for r in rows}),
                              ", ".join("%s %d" % kv for kv in sorted(kinds.items()))))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--split", action="store_true", help="rewrite config/xngine_modules.csv")
    ap.add_argument("only", nargs="*")
    a = ap.parse_args()
    img = xn_link.Image()
    an = Analysis(img)
    print("object 2: %d functions (%d found by gap filling, %d through pointers in data), "
          "%d instructions, %d code bytes" % (
              len(an.funcs), len(an.gap_funcs), len(an.ptr_funcs), len(an.insns),
              sum(i.size for i in an.insns.values())))
    if a.split or not os.path.exists(xn_link.MODULES):
        with open(xn_link.MODULES, "w", newline="\n") as f:
            w = csv.writer(f, lineterminator="\n")
            w.writerow(["name", "start", "end"])
            for n, s, e in an.split():
                w.writerow([n, "0x%08X" % s, "0x%08X" % e])
    write_functions(an)
    write_patches(an)
    mods = xn_link.modules()
    starts = [s for _n, s, _e in mods]
    # labels each module must make public: what the others reference
    exports = {}
    for name, start, end in mods:
        mod = Module(an, name, start, end)
        mod.emit(set())
        for n in mod.externs:
            va = int(xn_link.NAME.match(n).group(1), 16)
            if an.lo <= va < an.hi:
                owner = mods[bisect.bisect_right(starts, va) - 1][0]
                exports.setdefault(owner, set()).add(n)
    os.makedirs(xn_link.SRC, exist_ok=True)
    total_db = 0
    failed = []
    for name, start, end in mods:
        if a.only and name not in a.only:
            continue
        fallback = set()
        for _round in range(20):
            mod = Module(an, name, start, end, exports.get(name, ()))
            text, linemap = mod.emit(fallback, probe=True)
            path = os.path.join(PROBE_DIR, name + ".asm")
            os.makedirs(PROBE_DIR, exist_ok=True)
            open(path, "w", newline="\n").write(text)
            obj, msgs = xn_link.assemble(path, img, PROBE_DIR)
            data, problems = xn_link.link(obj, start, end, img) if obj else (None, [])
            if obj is None and not ERR.search(msgs):
                break
            bad = bad_insns(mod, data, problems, msgs, linemap) - fallback
            if not bad:
                break
            fallback |= bad
        mod = Module(an, name, start, end, exports.get(name, ()))
        text, _ = mod.emit(fallback)
        open(os.path.join(xn_link.SRC, name + ".asm"), "w", newline="\n").write(text)
        data, problems, msgs = xn_link.check(name, start, end, img)
        total_db += len(fallback)
        status = "OK" if not problems else "%d problems: %s" % (
            len(problems), "; ".join("%#x %s" % p for p in problems[:3]))
        if problems:
            failed.append(name)
            if data is None:
                status += "\n" + msgs.strip()[:2000]
        print("%-10s %#x-%#x  %5d bytes as db  %s" % (
            name, start, end, len(fallback), status))
    print("%d modules, %d instructions written as bytes, %d not matching" % (
        len(mods), total_db, len(failed)))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
