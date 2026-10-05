#!/usr/bin/env python3
"""XnGine (FALL.EXE object 2) as C: translate every function's instructions into literal C on a
register file, build it with Watcom C32 10.0a, and check it against recorded calls.

Literal C (src/xngine_c/xn_<module>.c, generated; the runtime is src/xngine_c/runtime.[ch] and
xn_rt.asm):
  - each function becomes `void xn_<va>(void)` over its instructions, found by walking from its
    entry (code shared with another function is translated into both; reaching another
    function's entry is a jump to it);
  - every instruction is C on R (the registers) and on memory at the game's addresses; the
    stack is R.esp in that memory, so a call pushes the original return address;
  - arithmetic flags are computed only where something reads them (liveness over the
    function); a compare or test and the branch after it become a C comparison, other readers
    use the lazy flags in F; at `ret`, calls and jumps out every flag is live;
  - the self-modifying code: an operand the program rewrites (config/xngine_patches.csv,
    config/xngine_runtime_patches.csv) is read from its code bytes when the instruction runs, and
    in the unrolled loops where a `ret` is planted every instruction first checks for one;
  - calls go through the callee's asm entry (xn_call), where a translated function's entry
    leads to its C; jumps out of the function, and any instruction the translator does not
    handle, continue in the asm at that address (GOTO_ASM), which stays correct.

Build: wcc386 -mf -s -zl for every file in one DOSBox-X session, then a small linker
(tools/xn_cload.py) lays the objects out at 0x10000000 in a memory region the emulator maps for
them. Test: each record of a function is replayed (tools/xn_record.py replay) with that
function's asm entry sent to its C; it passes when every exit register, the flags, every byte
written and the port I/O match.

usage: xn_c.py translate                 write src/xngine_c/xn_*.c
       xn_c.py build [FUNC ...]          compile and link (build/xngine/c_work/image.pkl)
       xn_c.py test [FUNC ...] [--records DIR ...] [--all-c] [--max-per N] [-j 3]
                                         replay records with the C; build/xngine/c_report.csv
       xn_c.py diff [FUNC ...] [--untested] [--trials N]
                                         asm and C from the same made-up entry states
       xn_c.py play SNAP [--ticks N] [--asm] [--shot PNG] [--script INPUT]
                                         run the game with XnGine in C
       xn_c.py show FUNC                 print one function's C
"""
import argparse
import bisect
import collections
import csv
import json
import os
import re
import sys

from capstone import x86 as cx

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
OUT = os.path.join(ROOT, "src", "xngine_c")
WORK = os.path.join(ROOT, "build", "xngine", "c_work")
OVERRIDES = os.path.join(OUT, "override")      # hand-written replacements: xn_<va>.c

CF, PF, AF, ZF, SF, OF = 0x1, 0x4, 0x10, 0x40, 0x80, 0x800
ALL = CF | PF | AF | ZF | SF | OF

CC = ["o", "no", "b", "ae", "e", "ne", "be", "a", "s", "ns", "p", "np", "l", "ge", "le", "g"]
CC_ALIAS = {"z": "e", "nz": "ne", "c": "b", "nae": "b", "nb": "ae", "nc": "ae", "na": "be",
            "nbe": "a", "nge": "l", "nl": "ge", "ng": "le", "nle": "g", "pe": "p", "po": "np"}
CC_READS = [OF, OF, CF, CF, ZF, ZF, CF | ZF, CF | ZF, SF, SF, PF, PF, SF | OF, SF | OF,
            ZF | SF | OF, ZF | SF | OF]

R32 = ["eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"]
REG = {}
for _r in R32:
    REG[_r] = ("R." + _r, 4)
for _r in ("ax", "cx", "dx", "bx", "sp", "bp", "si", "di"):
    REG[_r] = (_r.upper(), 2)
for _r in ("al", "ah", "cl", "ch", "dl", "dh", "bl", "bh"):
    REG[_r] = (_r.upper(), 1)
SEGS = ("es", "cs", "ss", "ds", "fs", "gs")
for _r in SEGS:
    REG[_r] = ("R." + _r, 2)

U = {1: "(u8)", 2: "(u16)", 4: ""}
SG = {1: "(s8)", 2: "(s16)", 4: "(s32)"}
TY = {1: "u8", 2: "u16", 4: "u32"}
MEM = {1: "M8", 2: "M16", 4: "M32"}
SIZE = {1: 0, 2: 1, 4: 2}
SIGN = {1: 0x80, 2: 0x8000, 4: 0x80000000}
MASK = {1: 0xFF, 2: 0xFFFF, 4: 0xFFFFFFFF}

LAZY = {"add": "XF_ADD", "adc": "XF_ADC", "sub": "XF_SUB", "sbb": "XF_SBB", "cmp": "XF_SUB",
        "and": "XF_LOGIC", "or": "XF_LOGIC", "xor": "XF_LOGIC", "test": "XF_LOGIC",
        "inc": "XF_INC", "dec": "XF_DEC", "neg": "XF_SUB", "shl": "XF_SHL", "sal": "XF_SHL",
        "shr": "XF_SAR", "sar": "XF_SAR", "shld": "XF_SHL", "shrd": "XF_SAR"}


def h(v):
    return "0x%X" % v


class Unsupported(Exception):
    pass


def load_analysis():
    import xn_link
    import xn_disasm
    return xn_disasm.Analysis(xn_link.Image())


def functions():
    with open(os.path.join(ROOT, "config", "xngine_functions.csv"), newline="") as f:
        return [(int(r["va"], 16), r["found_by"]) for r in csv.DictReader(f)]


def modules():
    with open(os.path.join(ROOT, "config", "xngine_modules.csv"), newline="") as f:
        return [(r["name"], int(r["start"], 16), int(r["end"], 16)) for r in csv.DictReader(f)]


class Program:
    """Object 2's code map and its self-modifying fields, from tools/xn_disasm.py."""

    def __init__(self, an=None):
        self.an = an or load_analysis()
        an = self.an
        self.funcs = [va for va, _k in functions()]
        self.func_set = set(self.funcs)
        self.sorted_funcs = sorted(self.funcs)
        # operands the code rewrites: field address -> True
        self.patch_fields = set(an.patch_fields)
        self.patch_bytes = set()
        # functions where a ret is planted at a computed instruction (opcode patches)
        self.ret_funcs = set()
        with open(os.path.join(ROOT, "config", "xngine_patches.csv"), newline="") as f:
            for r in csv.DictReader(f):
                field, insn = int(r["field"], 16), int(r["insn"], 16)
                if r["kind"] == "opcode":
                    self.ret_funcs.add(self.func_of(insn))
                else:
                    self.patch_fields.add(field)
        with open(os.path.join(ROOT, "config", "xngine_runtime_patches.csv"), newline="") as f:
            for r in csv.DictReader(f):
                self.patch_fields.add(int(r["field"], 16))

    def func_of(self, va):
        k = bisect.bisect_right(self.sorted_funcs, va) - 1
        return self.sorted_funcs[k] if k >= 0 else None

    def fixup_target(self, field):
        """The LE fixup at this field (an OFF32 into anywhere), or None."""
        fx = self.an.fields.get(field)
        if fx is not None and fx.kind == 7:      # SRC_OFF32
            return fx.target_va
        return None

    def jump_table(self, disp):
        """Targets of a jump table at disp, read through the LE fixups as xn_disasm does."""
        out, k = [], 0
        while True:
            fx = self.an.img.fix_at.get(disp + 4 * k)
            if fx is None or fx.kind != 7 or not self.an.lo <= fx.target_va < self.an.hi:
                break
            out.append(fx.target_va)
            k += 1
        return out


# --------------------------------------------------------------------------------------------
# One function

class Insn:
    """A decoded instruction with what the translator needs to know about flags."""
    __slots__ = ("i", "va", "next", "m", "reads", "kill", "wkind", "succ", "exit", "label",
                 "fused", "local", "store", "cf_live", "live_out", "rep", "target", "table",
                 "pre_lf", "check_lf")

    def __init__(self, i):
        self.i = i
        self.va = i.address
        self.next = i.address + i.size
        self.m = i.mnemonic
        self.reads = 0          # flags it reads
        self.kill = 0           # flags it always writes
        self.wkind = None       # "lazy" (fusable), "var" (may write), "helper"/"mat" (global)
        self.succ = []
        self.exit = False       # leaves the function (all flags live)
        self.label = False
        self.fused = None       # the lazy writer a reader's flags come from (C locals)
        self.local = False      # a lazy writer some fused reader reads
        self.store = False      # a writer that must store the lazy flags (globally)
        self.cf_live = False
        self.live_out = 0
        self.rep = 0
        self.target = None
        self.table = None
        self.pre_lf = None      # a lazy writer whose flags are stored just before this
        self.check_lf = None    # likewise, on the way out of a planted ret


def cc_of(m):
    """Condition index of jcc/setcc mnemonic m, or None."""
    for p in ("j", "set"):
        if m.startswith(p):
            c = m[len(p):]
            c = CC_ALIAS.get(c, c)
            if c in CC:
                return CC.index(c)
    return None


class Func:
    def __init__(self, prog, entry):
        self.prog, self.entry = prog, entry
        self.insns = {}
        self.unsupported = []
        self.walk()

    # -- the body ------------------------------------------------------------------------
    def walk(self):
        prog, an = self.prog, self.prog.an
        work = [self.entry]
        while work:
            va = work.pop()
            while va not in self.insns:
                if va != self.entry and va in prog.func_set:
                    break               # another function: reached by a jump (GOTO_ASM)
                i = an.insns.get(va)
                if i is None:
                    break               # not decoded code: the asm decides
                x = Insn(i)
                self.insns[va] = x
                g = i.groups
                m = i.mnemonic
                if cx.X86_GRP_RET in g or cx.X86_GRP_IRET in g or m in ("hlt", "retf", "iret",
                                                                         "iretd"):
                    break
                if m == "jmp":
                    op = i.operands[0]
                    if op.type == cx.X86_OP_IMM:
                        x.target = op.imm & 0xFFFFFFFF
                        work.append(x.target)
                    elif op.type == cx.X86_OP_MEM and op.mem.index and op.mem.scale == 4 and \
                            not op.mem.base:
                        x.table = prog.jump_table(op.mem.disp & 0xFFFFFFFF)
                        work.extend(x.table)
                    break
                if (cx.X86_GRP_JUMP in g or cx.X86_GRP_BRANCH_RELATIVE in g) and \
                        i.operands and i.operands[0].type == cx.X86_OP_IMM:
                    x.target = i.operands[0].imm & 0xFFFFFFFF
                    work.append(x.target)
                va += i.size
        self.order = sorted(self.insns)

    def inside(self, va):
        return va in self.insns

    # -- flags ---------------------------------------------------------------------------
    def classify(self, x):
        """reads/kill/wkind/succ/exit for one instruction."""
        i, m = x.i, x.m
        words = m.split()
        base = words[-1]
        if len(words) > 1:
            x.rep = 2 if words[0] in ("repne", "repnz") else 1
        c = cc_of(m)
        nxt = [x.next]
        x.succ = nxt
        if c is not None:
            x.reads = CC_READS[c]
            if m.startswith("j"):
                t = x.target
                x.succ = nxt + ([t] if t in self.insns else [])
                if t not in self.insns:
                    x.exit = True
            return
        if m == "jmp":
            if x.target is not None and x.target in self.insns:
                x.succ = [x.target]
            elif x.table is not None:
                x.succ = [t for t in x.table if t in self.insns]
                x.exit = True
            else:
                x.succ, x.exit = [], True
            return
        if m in ("ret", "retf", "hlt"):
            x.succ, x.reads = [], ALL       # the caller gets every flag
            return
        if m in ("iret", "iretd"):
            x.succ, x.kill, x.wkind = [], ALL, "helper"     # flags come off the stack
            return
        if m in ("loop", "jecxz", "jcxz", "loope", "loopne"):
            t = x.target
            x.succ = nxt + ([t] if t in self.insns else [])
            x.exit = t not in self.insns
            if m in ("loope", "loopne"):
                x.reads = ZF
            return
        if m in ("call", "int", "pushfd", "pushf", "div", "idiv") or m.startswith("int"):
            # a call: the callee may read flags (CF) and sets them; pushfd reads them all; a
            # divide may fault, and the fault frame holds EFLAGS
            x.reads = ALL
            if m in ("call", "int"):
                x.kill = ALL
                x.wkind = "helper"
            return
        if m in ("popfd", "popf"):
            x.kill, x.wkind = ALL, "helper"
            return
        if m in ("add", "sub", "and", "or", "xor", "cmp", "test", "neg"):
            x.kill, x.wkind = ALL, "lazy"
            return
        if m in ("adc", "sbb"):
            x.reads, x.kill, x.wkind = CF, ALL, "lazy"
            return
        if m in ("inc", "dec"):
            x.kill, x.wkind = ALL & ~CF, "lazy"
            return
        if m in ("shl", "sal", "shr", "sar", "shld", "shrd"):
            cnt = i.operands[-1]
            if cnt.type == cx.X86_OP_IMM and not self.patched_imm(i):
                n = (cnt.imm if i.imm_size else 1) & 31
                if n:
                    x.kill, x.wkind = ALL, "lazy"
                return
            x.wkind = "var"         # a count of 0 leaves the flags alone
            return
        if m in ("rol", "ror", "rcl", "rcr", "clc", "stc", "cmc"):
            x.reads, x.kill, x.wkind = ALL, (CF if m in ("clc", "stc", "cmc") else CF | OF), "mat"
            return
        if m in ("mul", "imul", "bsf", "bsr"):
            x.kill, x.wkind = ALL, "helper"
            return
        if base in ("scasb", "scasw", "scasd", "cmpsb", "cmpsw", "cmpsd") and \
                len(i.operands) == 2 and i.operands[0].type in (cx.X86_OP_MEM, cx.X86_OP_REG):
            if x.rep:
                x.wkind = "helper-var"
            else:
                x.kill, x.wkind = ALL, "helper"
            return

    def patched_imm(self, i):
        if not i.imm_size:
            return False
        a = i.address + i.imm_offset
        return any(a + k in self.prog.patch_fields for k in range(i.imm_size))

    def flags(self):
        for va in self.order:
            self.classify(self.insns[va])
        # instructions the translator does not handle continue in the asm: all flags live
        self.unsupported = []
        for va in self.order:
            x = self.insns[va]
            try:
                self.insn(x)
            except Unsupported:
                x.exit, x.reads, x.succ, x.kill, x.wkind = True, ALL, [], 0, None
        labels = {self.entry}
        for x in self.insns.values():
            if x.target is not None and x.target in self.insns:
                labels.add(x.target)
            if x.table:
                labels.update(t for t in x.table if t in self.insns)
        # code not entered by falling through from the instruction before it is a label, and
        # so is a fall-through target that is not the next instruction emitted
        prev = None
        for k, va in enumerate(self.order):
            x = self.insns[va]
            if prev is None or not (self.falls(prev) and prev.next == va):
                labels.add(va)
            nxt = self.order[k + 1] if k + 1 < len(self.order) else None
            if self.falls(x) and x.next in self.insns and x.next != nxt:
                labels.add(x.next)
            prev = x
        for va in labels:
            if va in self.insns:
                self.insns[va].label = True
        # Which flag writer's values reach each instruction (forward): a lazy writer W when
        # the last writer on every path to it is W, so the C locals still hold W's operands.
        state = {va: None for va in self.insns}
        state[self.entry] = "E"
        work = [self.entry]
        while work:
            va = work.pop()
            x = self.insns[va]
            st = state[va]
            out = x if x.wkind == "lazy" else "X" if x.wkind else st
            for t in x.succ:
                if t not in self.insns:
                    continue
                old = state[t]
                new = out if old is None or old is out else "X"
                if new is not old:
                    state[t] = new
                    work.append(t)
        uses = {}
        for va in self.order:
            x = self.insns[va]
            w = state[va]
            lazy = w if isinstance(w, Insn) else None
            u = x.reads
            if x.reads and lazy is not None:
                if x.reads == ALL:
                    # everything is read (a return, a call, pushfd...): store the writer's
                    # flags just before
                    if lazy.kill == ALL:
                        x.pre_lf = lazy
                        lazy.local = True
                        u = 0
                elif x.reads & ~lazy.kill == 0 and x.wkind != "mat":
                    x.fused = lazy
                    lazy.local = True
                    u = 0
            if self.prog.func_of(va) in self.prog.ret_funcs:
                # a ret may be planted here: it returns with the flags so far
                if lazy is not None and lazy.kill == ALL:
                    x.check_lf = lazy
                    lazy.local = True
                elif w != "E":
                    u = ALL
            uses[va] = u
        # global liveness (backward): which lazy writers must store their flags in F
        live_in = {va: 0 for va in self.insns}
        changed = True
        order = list(reversed(self.order))
        while changed:
            changed = False
            for va in order:
                x = self.insns[va]
                out = ALL if x.exit else 0
                for s in x.succ:
                    if s in self.insns:
                        out |= live_in[s]
                    else:
                        out = ALL       # falls out of the walk: into asm
                x.live_out = out
                li = uses[va] | (out & ~x.kill)
                if li != live_in[va]:
                    live_in[va] = li
                    changed = True
        for x in self.insns.values():
            if x.wkind == "lazy":
                x.store = bool(x.kill & x.live_out)
                x.cf_live = bool(x.live_out & CF)
            elif x.wkind == "var":
                x.store = bool(x.live_out & ALL)

    def falls(self, x):
        """Does execution continue at x.next?"""
        m = x.m
        if m in ("jmp", "ret", "retf", "iret", "iretd", "hlt"):
            return False
        return True

    # -- C ---------------------------------------------------------------------------------
    SPLIT = 600         # functions longer than this become parts (Watcom's limits)
    PART_LINES = 800

    def c(self):
        self.flags()
        self.unsupported = []
        blocks = []         # (va, lines) per instruction
        for k, va in enumerate(self.order):
            x = self.insns[va]
            out = []
            if x.label:
                out.append("L_%06X:" % va)
            if self.prog.func_of(va) in self.prog.ret_funcs:
                lf = self.store_lf(x.check_lf)
                out.append("    if (M8(XN(%s)) == 0xC3) { %sRET(0); }   /* a planted ret */" % (
                    h(va), lf + " " if lf else ""))
            try:
                body = self.insn(x)
                if x.pre_lf is not None:
                    body = [self.store_lf(x.pre_lf)] + body
            except Unsupported as e:
                self.unsupported.append((va, x.m, str(e)))
                body = ["GOTO_ASM(XN(%s));   /* %s %s: %s */" % (h(va), x.m, x.i.op_str, e)]
            text = "%s %s" % (x.m, x.i.op_str)
            if body:
                out.append("    %-60s /* %06X: %s */" % (body[0], va, text))
                for b in body[1:]:
                    out.append("    " + b)
            else:
                out.append("    /* %06X: %s */" % (va, text))
            nxt = self.order[k + 1] if k + 1 < len(self.order) else None
            if self.falls(x) and x.next != nxt:
                if x.next in self.insns:
                    out.append("    goto L_%06X;" % x.next)
                else:
                    out.append("    GOTO_ASM(XN(%s));" % h(x.next))
            blocks.append((va, out))
        name = "xn_%08X" % self.entry
        if len(blocks) > self.SPLIT or sum(len(b) for _v, b in blocks) > 2 * self.PART_LINES:
            return self.parts(name, blocks)
        out = [line for _va, lines in blocks for line in lines]
        if out and out[-1].endswith(":"):
            out.append("    ;")
        hdr = ["void %s(void)" % name, "{",
               "    u32 fa, fb, fr, fc, ea, t, t2;", ""]
        if self.order[0] != self.entry:
            hdr.append("    goto L_%06X;" % self.entry)
        return "\n".join(hdr + out + ["}", ""])

    def parts(self, name, blocks):
        """A long function as several C functions: each runs from a label to where control
        leaves it and returns the label to go on at (-1: the function is done). The flag
        locals are file-scope statics they share."""
        # cut every PART_LINES lines, preferably at a label; a cut is a label
        cuts = [0]
        n = 0
        for k, (va, lines) in enumerate(blocks):
            if n >= self.PART_LINES and (lines[0].startswith("L_") or
                                         n >= self.PART_LINES + 200):
                cuts.append(k)
                if not lines[0].startswith("L_"):
                    lines.insert(0, "L_%06X:" % va)
                n = 0
            n += len(lines)
        cuts.append(len(blocks))
        labels = [va for va, lines in blocks if lines and lines[0].startswith("L_")]
        lid = {va: k for k, va in enumerate(labels)}
        defs = ["extern u32 %s_f[4];   /* the flag locals its parts share */" % name,
                "#define fa %s_f[0]" % name, "#define fb %s_f[1]" % name,
                "#define fr %s_f[2]" % name, "#define fc %s_f[3]" % name,
                "#undef XN_DONE", "#define XN_DONE -1"]
        undefs = ["#undef XN_DONE", "#define XN_DONE", "#undef fa", "#undef fb", "#undef fr",
                  "#undef fc"]
        pieces = []
        ranges = []
        goto = re.compile(r"goto L_([0-9A-F]{6});")
        for p in range(len(cuts) - 1):
            part = blocks[cuts[p]:cuts[p + 1]]
            mine = {va for va, _l in part}
            own = [va for va in labels if va in mine]
            ranges.append((lid[own[0]], lid[own[-1]]))
            out = ["/* %s, part %d of %d */" % (name, p + 1, len(cuts) - 1)] + defs
            out.append("int %s_%d(int at)" % (name, p))
            out.append("{")
            out.append("    u32 ea, t, t2;")
            out.append("    switch (at) {")
            for va in own:
                out.append("    case %d: goto L_%06X;" % (lid[va], va))
            out.append("    }")
            for va, lines in part:
                for line in lines:
                    out.append(goto.sub(lambda mm: mm.group(0) if int(mm.group(1), 16) in mine
                                        else "return %d;" % lid[int(mm.group(1), 16)], line))
            last_va = part[-1][0]
            x = self.insns[last_va]
            k = cuts[p + 1]
            if self.falls(x) and k < len(blocks) and blocks[k][0] == x.next:
                out.append("    return %d;" % lid[x.next])
            out.append("    return -1;")
            out.append("}")
            out += undefs + [""]
            pieces.append("\n".join(out))
        out = ["/* %s is long: it runs as %d parts, each from a label to where control leaves"
               " it (returning the label to go on at, or -1) */" % (name, len(cuts) - 1),
               "u32 %s_f[4];" % name]
        for p in range(len(ranges)):
            out.append("int %s_%d(int at);" % (name, p))
        out.append("")
        out.append("void %s(void)" % name)
        out.append("{")
        out.append("    int at = %d;" % lid[self.entry])
        out.append("    while (at >= 0) {")
        for p, (lo, hi) in enumerate(ranges):
            out.append("        %sif (at <= %d) at = %s_%d(at);" % ("else " if p else "", hi,
                                                                    name, p))
        out.append("    }")
        out.append("}")
        out.append("")
        return pieces + ["\n".join(out)]

    # operands -------------------------------------------------------------------------------
    def imm(self, x, op, size=None):
        """An immediate operand: a constant, a relocated address, or a field read from the
        code when the program rewrites it."""
        i = x.i
        size = size or op.size
        if i.imm_size:
            a = i.address + i.imm_offset
            if any(a + k in self.prog.patch_fields for k in range(i.imm_size)):
                raw = "%s(XN(%s))" % (MEM[i.imm_size], h(a))
                if i.imm_size < size:
                    return "(%s)(%s)(s8)%s" % (TY[size], SG[size], raw) if i.imm_size == 1 \
                        else "(%s)(%s)%s" % (TY[size], SG[size], raw)
                return raw
            t = self.prog.fixup_target(a)
            if t is not None:
                return "XN(%s)" % h(t)
            fx = self.prog.an.fields.get(a)
            if fx is not None and fx.kind == 2:     # a selector: the loader writes it
                return "M16(XN(%s))" % h(a)
        v = op.imm & MASK[size]
        return h(v)

    def addr(self, x, op):
        i = x.i
        mem = op.mem
        parts = []
        if mem.segment and i.reg_name(mem.segment) not in ("ds", "ss", "es"):
            raise Unsupported("segment " + i.reg_name(mem.segment))
        if mem.base:
            b = i.reg_name(mem.base)
            if b not in R32:
                raise Unsupported("16-bit address")
            parts.append("R." + b)
        if mem.index:
            n = i.reg_name(mem.index)
            if n not in R32:
                raise Unsupported("16-bit address")
            parts.append("R.%s * %d" % (n, mem.scale) if mem.scale > 1 else "R." + n)
        disp = mem.disp & 0xFFFFFFFF
        if i.disp_size:
            a = i.address + i.disp_offset
            if any(a + k in self.prog.patch_fields for k in range(i.disp_size)):
                d = "%s(XN(%s))" % (MEM[i.disp_size], h(a))
                if i.disp_size == 1:
                    d = "(u32)(s32)(s8)" + d
                parts.append(d)
            else:
                t = self.prog.fixup_target(a)
                if t is not None:
                    parts.append("XN(%s)" % h(t))
                elif disp:
                    parts.append(h(disp) if disp < 0x80000000 or not parts else
                                 "-" + h((-disp) & 0xFFFFFFFF))
        elif disp:
            parts.append(h(disp))
        if not parts:
            return "0"
        if i.prefix[3] == 0x67:
            raise Unsupported("16-bit address")
        s = " + ".join(parts).replace("+ -", "- ")
        return s

    def reg(self, x, r):
        n = x.i.reg_name(r)
        if n not in REG:
            raise Unsupported("register " + n)
        return REG[n]

    def rd(self, x, op, ea=None):
        """C expression for the operand's value."""
        if op.type == cx.X86_OP_REG:
            e, size = self.reg(x, op.reg)
            if size == 2 and e.startswith("R."):
                return "(u16)" + e
            return e
        if op.type == cx.X86_OP_IMM:
            return self.imm(x, op)
        if op.type == cx.X86_OP_MEM:
            return "%s(%s)" % (MEM[op.size], ea or self.addr(x, op))
        raise Unsupported("operand")

    def lv(self, x, op, ea=None):
        """C lvalue for the operand."""
        if op.type == cx.X86_OP_REG:
            return self.reg(x, op.reg)[0]
        if op.type == cx.X86_OP_MEM:
            return "%s(%s)" % (MEM[op.size], ea or self.addr(x, op))
        raise Unsupported("operand")

    def ea_setup(self, x, op, out):
        """For an operand both read and written: compute a memory address once (into ea)."""
        if op.type == cx.X86_OP_MEM:
            out.append("ea = %s;" % self.addr(x, op))
            return "ea"
        return None

    # flags --------------------------------------------------------------------------------
    def cf_expr(self, x):
        """C expression for CF as instruction x reads it."""
        w = x.fused
        if w is None:
            return "xn_cf()"
        return self.flag_expr(w, CF)

    def flag_expr(self, w, flag):
        k = LAZY[w.m]
        size = self.wsize(w)
        u, s = U[size], SG[size]
        bits = size * 8
        if flag == ZF:
            return "(%sfr == 0)" % u
        if flag == SF:
            return "(%sfr < 0)" % s
        if flag == PF:
            return "xn_parity[(u8)fr]"
        if flag == CF:
            return {"XF_ADD": "(%sfr < %sfa)" % (u, u),
                    "XF_ADC": "(fc ? %sfr <= %sfa : %sfr < %sfa)" % (u, u, u, u),
                    "XF_SUB": "(%sfa < %sfb)" % (u, u),
                    "XF_SBB": "(fc ? %sfa <= %sfb : %sfa < %sfb)" % (u, u, u, u),
                    "XF_LOGIC": "0", "XF_SHL": "((fa >> %d) & 1)" % (bits - 1),
                    "XF_SAR": "(fa & 1)"}[k]
        if flag == OF:
            return {"XF_ADD": "((~(fa ^ fb) & (fa ^ fr)) >> %d & 1)" % (bits - 1),
                    "XF_ADC": "((~(fa ^ fb) & (fa ^ fr)) >> %d & 1)" % (bits - 1),
                    "XF_SUB": "(((fa ^ fb) & (fa ^ fr)) >> %d & 1)" % (bits - 1),
                    "XF_SBB": "(((fa ^ fb) & (fa ^ fr)) >> %d & 1)" % (bits - 1),
                    "XF_LOGIC": "0", "XF_INC": "(%sfr == %s)" % (u, h(SIGN[size])),
                    "XF_DEC": "(%sfr == %s)" % (u, h(SIGN[size] - 1)),
                    "XF_SHL": "((fa ^ fr) >> %d & 1)" % (bits - 1),
                    "XF_SAR": "((fa ^ fr) >> %d & 1)" % (bits - 1)}[k]
        raise Unsupported("flag")

    def wsize(self, w):
        return w.i.operands[0].size

    def cond(self, x, c):
        """C expression for condition c (0-15) as x reads it."""
        w = x.fused
        if w is None:
            return "xn_cond(%d)" % c
        k = LAZY[w.m]
        size = self.wsize(w)
        u, s = U[size], SG[size]
        neg = c & 1
        name = CC[c & ~1]
        if k == "XF_SUB" and w.m != "neg":
            e = {"e": "%sfa == %sfb", "b": "%sfa < %sfb", "be": "%sfa <= %sfb",
                 "l": "%sfa < %sfb", "le": "%sfa <= %sfb"}
            if name in ("e", "b", "be"):
                r = e[name] % (u, u)
                return "(%s)" % (r if not neg else "!(%s)" % r)
            if name in ("l", "le"):
                r = e[name] % (s, s)
                return "(%s)" % (r if not neg else "!(%s)" % r)
        if k == "XF_LOGIC":
            r = {"e": "%sfr == 0" % u, "b": "0", "be": "%sfr == 0" % u, "s": "%sfr < 0" % s,
                 "l": "%sfr < 0" % s, "le": "%sfr <= 0" % s, "o": "0"}.get(name)
            if r is not None:
                return "(%s)" % (r if not neg else "!(%s)" % r)
        f = lambda fl: self.flag_expr(w, fl)
        if name == "be":
            r = "(%s | %s)" % (f(CF), f(ZF))
        elif name == "l":
            r = "(%s != %s)" % (f(SF), f(OF))
        elif name == "le":
            r = "(%s | (%s != %s))" % (f(ZF), f(SF), f(OF))
        else:
            r = f({"o": OF, "b": CF, "e": ZF, "s": SF, "p": PF}[name])
        return "!%s" % r if neg else r

    def store_lf(self, w):
        """The C that stores lazy writer w's flags in F from the locals (they still hold its
        operands), or "" when none is needed."""
        if w is None:
            return ""
        size = SIZE[self.wsize(w)]
        k = LAZY[w.m]
        b = "0" if k in ("XF_SHL", "XF_SAR") else "fb"
        pre = "F.x = fc; " if k in ("XF_ADC", "XF_SBB") else ""
        return "%sLF(%s | %d, fa, %s, fr);" % (pre, k, size, b)

    def lazy(self, x, out, a="fa", b="fb", r="fr"):
        """Store the lazy flags of writer x if they are live past the straight run."""
        if not x.store:
            return
        size = SIZE[self.wsize(x)]
        k = LAZY[x.m]
        if k in ("XF_INC", "XF_DEC"):
            if x.cf_live:
                out.insert(0, "F.x = xn_cf();")     # the CF it keeps, before F changes
            out.append("F.op = %s | %d; F.r = %s;" % (k, size, r))
            return
        if k in ("XF_ADC", "XF_SBB"):
            out.append("F.x = fc;")
        out.append("LF(%s | %d, %s, %s, %s);" % (k, size, a, b, r))

    # instructions -------------------------------------------------------------------------
    def insn(self, x):
        i, m = x.i, x.m
        ops = i.operands
        words = m.split()
        base = words[-1]
        out = []
        c = cc_of(m)
        if m.startswith("j") and c is not None:
            return [self.branch(x, self.cond(x, c))]
        if m.startswith("set") and c is not None:
            return ["%s = %s;" % (self.lv(x, ops[0]), self.cond(x, c))]
        f = getattr(self, "i_" + base, None)
        if f is None:
            raise Unsupported("instruction")
        r = f(x, ops, out)
        return out if r is None else r

    def branch(self, x, cond):
        t = x.target
        go = "goto L_%06X;" % t if t in self.insns else "GOTO_ASM(XN(%s));" % h(t)
        if cond is None:
            return go
        return "if (%s) %s" % (cond, go)

    def i_jmp(self, x, ops, out):
        if x.target is not None:
            return [self.branch(x, None)]
        op = ops[0]
        if x.table is not None:
            a = self.addr(x, op)
            out.append("t = M32(%s);" % a)
            out.append("switch (t) {")
            for t in sorted(set(x.table)):
                if t in self.insns:
                    out.append("case XN(%s): goto L_%06X;" % (h(t), t))
            out.append("}")
            out.append("GOTO_ASM(t);")
            return out
        return ["GOTO_ASM(%s);" % self.rd(x, op)]

    def i_call(self, x, ops, out):
        op = ops[0]
        if op.type == cx.X86_OP_IMM:
            t = op.imm & 0xFFFFFFFF
            fx = self.prog.an.fields.get(x.va + x.i.size - 4)
            if fx is not None and fx.kind == 8:      # SRC_REL32 into another object
                t = fx.target_va
            out.append("PUSH32(XN(%s)); xn_call(XN(%s));" % (h(x.next), h(t)))
            return out
        out.append("t = %s;" % self.rd(x, op))
        out.append("PUSH32(XN(%s)); xn_call(t);" % h(x.next))
        return out

    def i_ret(self, x, ops, out):
        n = ops[0].imm if ops else 0
        return ["RET(%d);" % n]

    def i_retf(self, x, ops, out):
        n = ops[0].imm if ops else 0
        return ["xn_ret = M32(R.esp); xn_retcs = M16(R.esp + 4); R.esp += 8 + %d;" % n,
                "xn_far = 1; return XN_DONE;"]

    def i_iretd(self, x, ops, out):
        return ["xn_ret = M32(R.esp); xn_retcs = M16(R.esp + 4); xn_popf(M32(R.esp + 8));",
                "R.esp += 12; xn_far = 1; return XN_DONE;"]

    def i_loop(self, x, ops, out):
        if x.i.prefix[3] == 0x67:
            return [self.branch(x, "--CX != 0")]
        return [self.branch(x, "--R.ecx != 0")]

    def i_jecxz(self, x, ops, out):
        return [self.branch(x, "R.ecx == 0")]

    def i_jcxz(self, x, ops, out):
        return [self.branch(x, "CX == 0")]

    def i_nop(self, x, ops, out):
        return []

    def i_mov(self, x, ops, out):
        d, s = ops
        if d.type == cx.X86_OP_REG and x.i.reg_name(d.reg) in SEGS:
            if x.i.reg_name(d.reg) in ("cs", "ss"):
                raise Unsupported("load cs/ss")
            return ["%s = (u16)%s;" % (self.lv(x, d), self.rd(x, s))]
        if s.type == cx.X86_OP_REG and x.i.reg_name(s.reg) in SEGS:
            return ["%s = (u16)%s;" % (self.lv(x, d), self.lv(x, s))]
        return ["%s = %s;" % (self.lv(x, d), self.rd(x, s))]

    def i_movzx(self, x, ops, out):
        d, s = ops
        return ["%s = %s;" % (self.lv(x, d), self.rd(x, s))]

    def i_movsx(self, x, ops, out):
        d, s = ops
        return ["%s = (%s)%s%s;" % (self.lv(x, d), TY[d.size], SG[s.size], self.rd(x, s))]

    def i_lea(self, x, ops, out):
        d, s = ops
        return ["%s = %s;" % (self.lv(x, d), self.addr(x, s))]

    def i_xchg(self, x, ops, out):
        a, b = ops
        if a.type == b.type == cx.X86_OP_REG and a.reg == b.reg:
            return []
        ea = None
        for op in (a, b):
            if op.type == cx.X86_OP_MEM:
                ea = self.ea_setup(x, op, out)
        out.append("t = %s; %s = %s; %s = t;" % (self.rd(x, a, ea), self.lv(x, a, ea),
                                                 self.rd(x, b, ea), self.lv(x, b, ea)))
        return out

    def i_bswap(self, x, ops, out):
        r = self.lv(x, ops[0])
        out.append("t = %s; %s = t >> 24 | (t >> 8 & 0xFF00) | (t << 8 & 0xFF0000) | t << 24;"
                   % (r, r))
        return out

    def i_cdq(self, x, ops, out):
        return ["R.edx = (u32)((s32)R.eax >> 31);"]

    def i_cwd(self, x, ops, out):
        return ["DX = (u16)((s16)AX >> 15);"]

    def i_cwde(self, x, ops, out):
        return ["R.eax = (u32)(s32)(s16)AX;"]

    def i_cbw(self, x, ops, out):
        return ["AX = (u16)(s16)(s8)AL;"]

    def i_push(self, x, ops, out):
        op = ops[0]
        if op.type == cx.X86_OP_REG and x.i.reg_name(op.reg) in SEGS:
            if x.i.prefix[2] == 0x66:
                return ["PUSH16((u16)%s);" % self.lv(x, op)]
            return ["t = (u16)%s; PUSH32(t);" % self.lv(x, op)]
        if op.size == 2:
            return ["t = %s; PUSH16(t);" % self.rd(x, op)]
        return ["t = %s; PUSH32(t);" % self.rd(x, op)]

    def i_pop(self, x, ops, out):
        op = ops[0]
        if op.type == cx.X86_OP_REG and x.i.reg_name(op.reg) in SEGS:
            if x.i.reg_name(op.reg) in ("ss", "cs"):
                raise Unsupported("pop ss")
            if x.i.prefix[2] == 0x66:
                return ["%s = M16(R.esp); R.esp += 2;" % self.lv(x, op)]
            return ["%s = M16(R.esp); R.esp += 4;" % self.lv(x, op)]
        if op.type == cx.X86_OP_REG and x.i.reg_name(op.reg) == "esp":
            return ["R.esp = M32(R.esp);"]
        if op.size == 2:
            return ["t = M16(R.esp); R.esp += 2; %s = t;" % self.lv(x, op)]
        return ["t = M32(R.esp); R.esp += 4; %s = t;" % self.lv(x, op)]

    def i_pushal(self, x, ops, out):
        return ["t = R.esp; PUSH32(R.eax); PUSH32(R.ecx); PUSH32(R.edx); PUSH32(R.ebx);",
                "PUSH32(t); PUSH32(R.ebp); PUSH32(R.esi); PUSH32(R.edi);"]

    def i_popal(self, x, ops, out):
        return ["R.edi = M32(R.esp); R.esi = M32(R.esp + 4); R.ebp = M32(R.esp + 8);",
                "R.ebx = M32(R.esp + 16); R.edx = M32(R.esp + 20); R.ecx = M32(R.esp + 24);",
                "R.eax = M32(R.esp + 28); R.esp += 32;"]

    def i_pushfd(self, x, ops, out):
        return ["t = xn_eflags(); PUSH32(t);"]

    def i_popfd(self, x, ops, out):
        return ["xn_popf(M32(R.esp)); R.esp += 4;"]

    # arithmetic
    def dead(self, x):
        """No one reads this writer's flags: plain C will do."""
        return not x.store and not x.local

    PLAIN = {"add": "+=", "sub": "-=", "and": "&=", "or": "|=", "xor": "^="}

    def alu(self, x, ops, out, expr, store=True):
        d, s = ops[0], ops[1]
        if self.dead(x) and x.m not in ("adc", "sbb"):
            if not store:
                return out          # cmp/test whose flags no one reads
            if x.m in ("sub", "xor") and d.type == s.type == cx.X86_OP_REG and d.reg == s.reg:
                return ["%s = 0;" % self.lv(x, d)]
            return ["%s %s %s;" % (self.lv(x, d), self.PLAIN[x.m], self.rd(x, s))]
        ea = self.ea_setup(x, d, out)
        size = d.size
        if x.m in ("adc", "sbb"):
            out.append("fc = %s;" % self.cf_expr(x))
        out.append("fa = %s; fb = %s;" % (self.rd(x, d, ea), self.rd(x, s)))
        out.append("fr = %s;" % expr)
        if store:
            out.append("%s = %s;" % (self.lv(x, d, ea), "fr" if size == 4 else
                                     "(%s)fr" % TY[size]))
        self.lazy(x, out)
        return out

    def i_add(self, x, ops, out):
        return self.alu(x, ops, out, "fa + fb")

    def i_adc(self, x, ops, out):
        return self.alu(x, ops, out, "fa + fb + fc")

    def i_sub(self, x, ops, out):
        if ops[0].type == ops[1].type == cx.X86_OP_REG and ops[0].reg == ops[1].reg:
            pass
        return self.alu(x, ops, out, "fa - fb")

    def i_sbb(self, x, ops, out):
        return self.alu(x, ops, out, "fa - fb - fc")

    def i_cmp(self, x, ops, out):
        return self.alu(x, ops, out, "fa - fb", store=False)

    def i_and(self, x, ops, out):
        return self.alu(x, ops, out, "fa & fb")

    def i_or(self, x, ops, out):
        return self.alu(x, ops, out, "fa | fb")

    def i_xor(self, x, ops, out):
        return self.alu(x, ops, out, "fa ^ fb")

    def i_test(self, x, ops, out):
        return self.alu(x, ops, out, "fa & fb", store=False)

    def i_inc(self, x, ops, out):
        return self.incdec(x, ops, out, "+")

    def i_dec(self, x, ops, out):
        return self.incdec(x, ops, out, "-")

    def incdec(self, x, ops, out, sign):
        d = ops[0]
        if self.dead(x):
            return ["%s %s= 1;" % (self.lv(x, d), sign)]
        ea = self.ea_setup(x, d, out)
        out.append("fa = %s; fr = fa %s 1; %s = %s;" % (
            self.rd(x, d, ea), sign, self.lv(x, d, ea),
            "fr" if d.size == 4 else "(%s)fr" % TY[d.size]))
        self.lazy(x, out)
        return out

    def i_neg(self, x, ops, out):
        d = ops[0]
        if self.dead(x):
            return ["%s = 0 - %s;" % (self.lv(x, d), self.rd(x, d))]
        ea = self.ea_setup(x, d, out)
        out.append("fa = 0; fb = %s; fr = 0 - fb; %s = %s;" % (
            self.rd(x, d, ea), self.lv(x, d, ea),
            "fr" if d.size == 4 else "(%s)fr" % TY[d.size]))
        self.lazy(x, out)
        return out

    def i_not(self, x, ops, out):
        d = ops[0]
        ea = self.ea_setup(x, d, out)
        return out + ["%s = ~%s;" % (self.lv(x, d, ea), self.rd(x, d, ea))]

    # shifts
    def count(self, x, op):
        if op.type == cx.X86_OP_IMM:
            if not x.i.imm_size:
                return "1", 1
            if self.patched_imm(x.i):
                return "(%s & 31)" % self.imm(x, op, 1), None
            return str(op.imm & 31), op.imm & 31
        if op.type == cx.X86_OP_REG and x.i.reg_name(op.reg) == "cl":
            return "(CL & 31)", None
        raise Unsupported("shift count")

    def shift(self, x, ops, out):
        d = ops[0]
        size = d.size
        u, s = U[size], SG[size]
        cnt, const = self.count(x, ops[-1])
        if const == 0:
            return out
        m = x.m
        if self.dead(x):
            if const is None:
                out.append("t = %s;" % cnt)
                cnt = "t"
            if m in ("shl", "sal"):
                out.append("%s <<= %s;" % (self.lv(x, d), cnt))
            elif m == "shr":
                out.append("%s >>= %s;" % (self.lv(x, d), cnt))
            else:
                out.append("%s = (%s)(%s%s >> %s);" % (self.lv(x, d), TY[size], s,
                                                       self.rd(x, d), cnt))
            return out
        ea = self.ea_setup(x, d, out)
        body = []
        if const is None:
            out.append("t = %s;" % cnt)
            cnt = "t"
        less = "%s - 1" % cnt if const is None else str(const - 1)
        v = "%s%s" % (u, self.rd(x, d, ea)) if size < 4 else self.rd(x, d, ea)
        if m in ("shl", "sal"):
            body.append("fa = %s << %s; fr = fa << 1;" % (v, less if const is not None
                                                         else "(%s)" % less))
        elif m == "shr":
            body.append("fa = %s >> %s; fr = fa >> 1;" % (v, less if const is not None
                                                         else "(%s)" % less))
        else:
            body.append("fa = (u32)(%s%s >> %s); fr = (u32)((s32)fa >> 1);" % (
                s, self.rd(x, d, ea), less if const is not None else "(%s)" % less))
        body.append("%s = %s;" % (self.lv(x, d, ea), "fr" if size == 4 else
                                  "(%s)fr" % TY[size]))
        self.lazy(x, body, b="0")
        if const is None:
            out.append("if (t) {")
            out.extend("    " + b for b in body)
            out.append("}")
        else:
            out.extend(body)
        return out

    i_shl = i_sal = i_shr = i_sar = shift

    def i_shld(self, x, ops, out):
        return self.dshift(x, ops, out, left=True)

    def i_shrd(self, x, ops, out):
        return self.dshift(x, ops, out, left=False)

    def dshift(self, x, ops, out, left):
        d, s = ops[0], ops[1]
        if d.size != 4:
            raise Unsupported("16-bit shld/shrd")
        cnt, const = self.count(x, ops[2])
        if const == 0:
            return out
        ea = self.ea_setup(x, d, out)
        out.append("t = %s;" % cnt)
        body = ["t2 = %s; fb = %s;" % (self.rd(x, d, ea), self.rd(x, s))]
        if left:
            body.append("fa = t == 1 ? t2 : t2 << (t - 1) | fb >> (33 - t);")
            body.append("fr = t2 << t | fb >> (32 - t);")
        else:
            body.append("fa = t == 1 ? t2 : t2 >> (t - 1) | fb << (33 - t);")
            body.append("fr = t2 >> t | fb << (32 - t);")
        body.append("%s = fr;" % self.lv(x, d, ea))
        self.lazy(x, body, b="0")
        if const is None:
            out.append("if (t) {")
            out.extend("    " + b for b in body)
            out.append("}")
        else:
            out.extend(body)
        return out

    def rotate(self, x, ops, out, kind):
        d = ops[0]
        cnt, const = self.count(x, ops[1])
        if d.type == cx.X86_OP_MEM:
            target = "(void *)(%s)" % self.addr(x, d)
        else:
            target = "(void *)&%s" % self.lv(x, d)
        return ["xn_rotate(%d, %d, %s, %s);" % (kind, SIZE[d.size], target, cnt)]

    def i_rol(self, x, ops, out):
        return self.rotate(x, ops, out, 0)

    def i_ror(self, x, ops, out):
        return self.rotate(x, ops, out, 1)

    def i_rcl(self, x, ops, out):
        return self.rotate(x, ops, out, 2)

    def i_rcr(self, x, ops, out):
        return self.rotate(x, ops, out, 3)

    # multiply, divide
    def i_imul(self, x, ops, out):
        if len(ops) == 1:
            return self.mul1(x, ops, out, signed=True)
        d = ops[0]
        a, b = (ops[0], ops[1]) if len(ops) == 2 else (ops[1], ops[2])
        size = d.size
        live = x.live_out & ALL
        if not live:
            if size == 4:
                return ["%s = %s * %s;" % (self.lv(x, d), self.rd(x, a), self.rd(x, b))]
            return ["%s = (u16)((s32)(s16)%s * (s32)(s16)%s);" % (self.lv(x, d), self.rd(x, a),
                                                               self.rd(x, b))]
        return ["%s = xn_imul2(%s, %s, %d);" % (self.lv(x, d), self.rd(x, a), self.rd(x, b),
                                                SIZE[size])]

    def i_mul(self, x, ops, out):
        return self.mul1(x, ops, out, signed=False)

    def mul1(self, x, ops, out, signed):
        s = ops[0]
        size = s.size
        if size == 4 and not x.live_out & ALL:
            return ["t = %s; R.eax = %s(R.eax, t, &R.edx);" % (
                self.rd(x, s), "xn_muls" if signed else "xn_mulu")]
        return ["%s(%s, %d);" % ("xn_imul1" if signed else "xn_mul1", self.rd(x, s),
                                 SIZE[size])]

    def div(self, x, ops, out, fn):
        s = ops[0]
        size = s.size
        i = x.i
        # the program's handler resumes after a divide error by the ModRM byte
        mod = i.bytes[1]
        skip = 6 if mod in (0x3D, 0x35) or (0xB0 <= mod <= 0xBF and mod not in (0xB4, 0xBC)) \
            else 3 if 0x70 <= mod <= 0x7F and mod not in (0x74, 0x7C) else 2
        if i.bytes[0] in (0x66, 0x67) or skip != i.size:
            out.append("xn_faulted = 0;")
            out.append("%s(%s, %d, %s);" % (fn, self.rd(x, s), SIZE[size], h(x.va)))
            out.append("if (xn_faulted) GOTO_ASM(XN(%s));" % h(x.va + skip))
            return out
        return ["%s(%s, %d, %s);" % (fn, self.rd(x, s), SIZE[size], h(x.va))]

    def i_div(self, x, ops, out):
        return self.div(x, ops, out, "xn_div")

    def i_idiv(self, x, ops, out):
        return self.div(x, ops, out, "xn_idiv")

    def i_bsf(self, x, ops, out):
        d, s = ops
        return ["%s = xn_bsf(%s, %s);" % (self.lv(x, d), self.rd(x, s), self.rd(x, d))]

    def i_bsr(self, x, ops, out):
        d, s = ops
        return ["%s = xn_bsr(%s, %s);" % (self.lv(x, d), self.rd(x, s), self.rd(x, d))]

    # flags
    def i_clc(self, x, ops, out):
        return ["xn_setcf(0);"]

    def i_stc(self, x, ops, out):
        return ["xn_setcf(1);"]

    def i_cmc(self, x, ops, out):
        return ["xn_setcf(xn_cf() ^ 1);"]

    def i_cld(self, x, ops, out):
        return ["R.eflags &= ~0x400u;"]

    def i_std(self, x, ops, out):
        return ["R.eflags |= 0x400u;"]

    def i_cli(self, x, ops, out):
        return ["R.eflags &= ~0x200u;"]

    def i_sti(self, x, ops, out):
        return ["R.eflags |= 0x200u;"]

    # system
    def i_int(self, x, ops, out):
        return ["xn_int(%s);" % h(ops[0].imm & 0xFF)]

    def i_in(self, x, ops, out):
        d, p = ops
        port = self.rd(x, p) if p.type == cx.X86_OP_IMM else "DX"
        fn = {1: "xn_in8", 2: "xn_in16", 4: "xn_in32"}[d.size]
        return ["%s = %s(%s);" % (self.lv(x, d), fn, port)]

    def i_out(self, x, ops, out):
        p, v = ops
        port = self.rd(x, p) if p.type == cx.X86_OP_IMM else "DX"
        fn = {1: "xn_out8", 2: "xn_out16", 4: "xn_out32"}[v.size]
        return ["%s(%s, %s);" % (fn, port, self.rd(x, v))]

    # strings
    def string(self, x, fn, suffix):
        size = {"b": 0, "w": 1, "d": 2}[suffix]
        return ["%s(%d, %d);" % (fn, size, x.rep)]

    def i_movsb(self, x, ops, out):
        return self.string(x, "xn_movs", "b")

    def i_movsw(self, x, ops, out):
        return self.string(x, "xn_movs", "w")

    def i_movsd(self, x, ops, out):
        return self.string(x, "xn_movs", "d")

    def i_stosb(self, x, ops, out):
        return self.string(x, "xn_stos", "b")

    def i_stosw(self, x, ops, out):
        return self.string(x, "xn_stos", "w")

    def i_stosd(self, x, ops, out):
        return self.string(x, "xn_stos", "d")

    def i_lodsb(self, x, ops, out):
        return self.string(x, "xn_lods", "b")

    def i_lodsw(self, x, ops, out):
        return self.string(x, "xn_lods", "w")

    def i_lodsd(self, x, ops, out):
        return self.string(x, "xn_lods", "d")

    def i_scasb(self, x, ops, out):
        return self.string(x, "xn_scas", "b")

    def i_scasw(self, x, ops, out):
        return self.string(x, "xn_scas", "w")

    def i_scasd(self, x, ops, out):
        return self.string(x, "xn_scas", "d")

    def i_cmpsb(self, x, ops, out):
        return self.string(x, "xn_cmps", "b")

    def i_cmpsw(self, x, ops, out):
        return self.string(x, "xn_cmps", "w")

    def i_cmpsd(self, x, ops, out):
        return self.string(x, "xn_cmps", "d")

    def i_insb(self, x, ops, out):
        return self.string(x, "xn_ins", "b")

    def i_outsb(self, x, ops, out):
        return self.string(x, "xn_outs", "b")

    def i_outsw(self, x, ops, out):
        return self.string(x, "xn_outs", "w")


# --------------------------------------------------------------------------------------------
# Translation of everything

FILE_LINES = 2500

HEADER = """/* %s.c: XnGine module %s (FALL.EXE object 2, 0x%X-0x%X) as literal C.
   Generated by tools/xn_c.py from the code in src/xngine/%s.asm; do not edit (a hand-written
   replacement for a function goes in src/xngine_c/override/xn_<va>.c). Each function is the
   asm one instruction at a time, on the register file R (see runtime.h). */
#include "runtime.h"

"""


def load_names():
    """{va: (name, confidence, evidence)} for XnGine functions (config/names.csv)."""
    out = {}
    p = os.path.join(ROOT, "config", "names.csv")
    if os.path.exists(p):
        with open(p, newline="") as f:
            for r in csv.DictReader(f):
                if r["kind"] == "func" and r["name"].startswith("xn_"):
                    out[int(r["address"], 16)] = (r["name"], r["confidence"], r["evidence"])
    return out


def wrap(text, width=92):
    text = text.replace("*/", "* /")
    words, lines, cur = text.split(), [], ""
    for w in words:
        if cur and len(cur) + 1 + len(w) > width:
            lines.append(cur)
            cur = w
        else:
            cur = (cur + " " + w).strip()
    lines.append(cur)
    return "\n   ".join(lines)


def translate(prog=None, only=None):
    prog = prog or Program()
    names = load_names()
    mods = modules()
    stats = {}
    by_mod = collections.defaultdict(list)
    for va, kind in functions():
        if only and va not in only:
            continue
        mod = next(n for n, s, e in mods if s <= va < e)
        by_mod[mod].append(va)
    os.makedirs(OUT, exist_ok=True)
    written = set()
    overrides = set()
    if os.path.isdir(OVERRIDES):
        for n in os.listdir(OVERRIDES):
            mm = re.match(r"xn_([0-9A-F]{8})\.c$", n)
            if mm:
                overrides.add(int(mm.group(1), 16))
    for name, s, e in mods:
        vas = by_mod.get(name)
        if not vas:
            continue
        parts = [HEADER % ("xn_" + name[3:], name, s, e, name)]
        for va in sorted(vas):
            fn = Func(prog, va)
            text = fn.c()
            pieces = text if isinstance(text, list) else [text]
            if va in names:
                n, conf, ev = names[va]
                pieces[-1] = "/* %s (%s): %s */\n%s" % (n, conf, wrap(ev), pieces[-1])
            if va in overrides:
                parts.append("/* xn_%08X: see override/xn_%08X.c */\n" % (va, va))
            else:
                parts.extend(pieces)
            stats[va] = {"module": name, "insns": len(fn.insns),
                         "unsupported": ["%06X %s (%s)" % u for u in fn.unsupported],
                         "override": va in overrides}
        # Watcom 10.0a runs out of room on long files: several files for a big module
        files, cur = [], []
        for text in parts[1:]:
            if cur and sum(t.count("\n") for t in cur) + text.count("\n") > FILE_LINES:
                files.append(cur)
                cur = []
            cur.append(text)
        files.append(cur)
        for k, chunk in enumerate(files):
            fn = "xn_%s%s.c" % (name[3:], "_%d" % k if k else "")
            with open(os.path.join(OUT, fn), "w", newline="\n") as f:
                f.write("\n".join([parts[0]] + chunk))
            written.add(fn)
    for fn in os.listdir(OUT):
        if re.match(r"xn_[0-9A-F]+(_\d+)?\.c$", fn) and fn not in written:
            os.remove(os.path.join(OUT, fn))
    os.makedirs(WORK, exist_ok=True)
    with open(os.path.join(WORK, "translate.json"), "w") as f:
        json.dump({"%08X" % k: v for k, v in sorted(stats.items())}, f, indent=0)
    return stats


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("translate")
    s = sub.add_parser("show")
    s.add_argument("func")
    b = sub.add_parser("build")
    b.add_argument("funcs", nargs="*", help="only the modules holding these functions")
    b.add_argument("--flags", default=None)
    t = sub.add_parser("test")
    t.add_argument("funcs", nargs="*")
    t.add_argument("--records", action="append", default=None)
    t.add_argument("--all-c", action="store_true", help="send every function to its C")
    t.add_argument("--max-per", type=int, default=0)
    t.add_argument("-j", "--jobs", type=int, default=3, help="worker processes (each runs an "
                   "emulator: at most 3)")
    t.add_argument("-v", action="store_true")
    d = sub.add_parser("diff")
    d.add_argument("funcs", nargs="*")
    d.add_argument("--records", action="append", default=None)
    d.add_argument("--trials", type=int, default=8)
    d.add_argument("--untested", action="store_true", help="only functions with no records")
    d.add_argument("-v", action="store_true")
    pl = sub.add_parser("play")
    pl.add_argument("snap")
    pl.add_argument("--ticks", type=int, default=100)
    pl.add_argument("--asm", action="store_true", help="the asm, for comparison")
    pl.add_argument("--shot", default=None)
    pl.add_argument("--script", default="", help="input, as fallemu.py takes it")
    a = ap.parse_args()
    if a.cmd == "translate":
        stats = translate()
        n = len(stats)
        clean = sum(1 for v in stats.values() if not v["unsupported"])
        print("%d functions translated (%d instructions), %d with no fallback to asm" % (
            n, sum(v["insns"] for v in stats.values()), clean))
        miss = collections.Counter()
        for v in stats.values():
            for u in v["unsupported"]:
                miss[u.split(" ", 1)[1]] += 1
        for k, c in miss.most_common(30):
            print("  %4d  %s" % (c, k))
        return 0
    if a.cmd == "show":
        va = int(a.func.replace("func_", ""), 16)
        print(Func(Program(), va).c())
        return 0
    import xn_cload
    if a.cmd == "build":
        srcs = None
        if a.funcs:
            want = {int(f.replace("func_", ""), 16) for f in a.funcs}
            mods = {n for n, s0, e in modules() if any(s0 <= v < e for v in want)}
            srcs = [p for p in xn_cload.sources()
                    if not os.path.basename(p).startswith("xn_") or
                    os.path.basename(p) == "xn_rt.asm" or
                    "xn_" + re.sub(r"_\d+$", "", os.path.basename(p)[3:-2]) in mods or
                    os.path.basename(p)[3:11] in {"%08X" % v for v in want}]
        return xn_cload.build(flags=a.flags, srcs=srcs)
    if a.cmd == "play":
        return xn_cload.play(a.snap, a.ticks, all_c=not a.asm, shot=a.shot, script=a.script)
    if a.cmd == "diff":
        funcs = [int(f.replace("func_", ""), 16) for f in a.funcs] or None
        if a.untested:
            have = set()
            with open(xn_cload.REPORT, newline="") as f:
                for r in csv.DictReader(f):
                    if r["records_total"]:
                        have.add(int(r["function"], 16))
            funcs = [va for va, _k in functions() if va not in have]
        return xn_cload.diff_test(funcs, a.records, trials=a.trials, verbose=a.v)
    if a.cmd == "test":
        funcs = [int(f.replace("func_", ""), 16) for f in a.funcs] or None
        return xn_cload.test(funcs, a.records, all_c=a.all_c, max_per=a.max_per, jobs=a.jobs,
                             verbose=a.v)


if __name__ == "__main__":
    sys.exit(main())
