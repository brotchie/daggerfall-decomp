#!/usr/bin/env python3
"""XnGine's register interfaces: what each function of FALL.EXE's object 2 takes, returns and
clobbers, for the readable C (src/engine/, tools/xn_rc.py) to declare with `#pragma aux`.

config/xngine_abi.csv, one row per function:
  va, name, subsystem
  convention  watcom (a plain Watcom C prototype fits: arguments in EAX EDX EBX ECX then the
              stack, which the callee pops; result in EAX; no flags out), reg (a register
              interface of its own: a pragma), interrupt (an interrupt or exception handler:
              preserves everything), template (code the engine copies and patches at run time),
              data (bytes in the function list that are not code), unknown (escapes analysis)
  inputs      registers (and flags) live at entry, by parts: al, ah, ax, eax, eax.u (the upper
              half), si, esi...
  stack_args  bytes of stack arguments read above the return address
  outputs     registers some caller reads after the return that the function may change
  flags_out   flags some caller reads after the return (CF as a status, ...)
  clobber     registers no caller reads after the return: the C may change them; it keeps the
              rest
  ret_pop     N of `ret N`
  callers     known (every call site seen), partial (some through pointers resolved by field
              offset, tables or the record corpus), unknown (an address that escapes, or no
              caller: every register it may change is an output)
  notes

How:
  - each instruction's register reads and writes come from capstone, by register part (a write
    to AL defines AL only; `xor r,r` reads nothing; `bsr`/`bsf` read their destination, which a
    zero source leaves; flags from capstone's eflags detail, and a shift by CL or a rep compare
    may leave them);
  - a forward pass per function follows ESP (pushes, pops, calls by the callee's `ret N`) and a
    state per register and stack slot (the 32-bit value last written whole: a register's entry
    value, a stack address, a constant, modified, maybe modified; and the parts written since):
    a register part is preserved when every return finds its own entry value in it again, as a
    push/pop pair or a pushad/popad leaves it;
  - a backward (strong) liveness pass per function over register parts, flags and dword stack
    slots: an instruction whose only effects are dead registers and slots uses nothing; a call
    uses the callee's inputs and stack arguments and kills what it always writes; a tail jump
    (or a call falling into the next function's entry) uses its target's inputs;
  - interprocedural, over objects 1 and 2 (the game's code too: what it reads after a call into
    XnGine reaches back through its own calls): the forward stage bottom-up over the call graph's
    strongly connected components, the liveness stage a worklist. What callers read after the
    return (used_after) is live at a function's returns for its call sites' sake; only its
    outputs (used_after among the registers it may change) are for its inputs' sake;
  - the game's code (Watcom 10.0a) never reads, after a call, the argument registers it passed
    (Watcom does not preserve a callee's argument registers: a 2-argument function never saves
    EDX) nor EAX but as the result; a call through a pointer, or of a routine with no body, takes
    the argument registers loaded just before it; a game function nothing calls or whose address
    is taken has Watcom's convention for its callers;
  - indirect calls: jump and call tables through their LE fixups ([reg*4+table] and Watcom's
    [reg+table]); `call [var]` through the constant stores into var (code and data);
    `call [reg+d]` through stores of function addresses into a field at offset d;
    build/xn_readable/indirect.json (from `xn_abi.py trace`) adds the targets the record
    corpus took (code inside a function is analysed as an entry of its own; generated code is
    anything);
  - `int` services: AH/AX followed as a constant; the DOS, DPMI, mouse and video calls the engine
    makes have their registers listed (SERVICES), anything else is every register.
config/xngine_abi_override.csv (va, field, value, reason) replaces a column where the analysis is
wrong or where there is no caller to decide (dead code); the row's notes say so.
build/xn_readable/abi_raw.csv keeps the analysis' own rows, so `overrides` can apply them alone.

Cross-checks:
  check     static, against every record: a register the row says is preserved comes back
            with its entry value
  clobber   dynamic: after each routed function returns, its clobber registers (and the flags
            but flags_out) are scrambled; every record must still replay (ABI-aware compare at
            the record's own function: its clobbers excused, flags only its flags_out). --all:
            every function scrambled at once; with FUNC: those functions, over the records
            whose code ran them
  inputs    dynamic: registers that are not inputs are scrambled at entry; the records must
            still replay (ABI-aware)

usage: xn_abi.py analyze [-v]              write config/xngine_abi.csv (30 s)
       xn_abi.py overrides                 rewrite it from the last analysis and the overrides
       xn_abi.py show FUNC                 one function's analysis, instruction by instruction
       xn_abi.py check                     preserved registers against the record corpus
       xn_abi.py trace [-j N]              indirect-call targets the corpus takes (indirect.json)
       xn_abi.py clobber [FUNC ...] [--all] [-j N] [--max-per N]
       xn_abi.py inputs [FUNC ...] [-j N] [--max-per N]
"""
import argparse
import bisect
import collections
import csv
import json
import os
import re
import struct
import sys
import time

from capstone import x86 as cx

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
# XN_ABI: another copy of the table to read and write (an agent's own, with its own overrides
# in XN_ABI_OVERRIDES, files separated by ':'), for testing before the coordinator merges them
ABI_CSV = os.environ.get("XN_ABI") or os.path.join(ROOT, "config", "xngine_abi.csv")
OVERRIDE_CSV = os.path.join(ROOT, "config", "xngine_abi_override.csv")
OVERRIDE_FILES = [OVERRIDE_CSV] + [p for p in os.environ.get("XN_ABI_OVERRIDES", "").split(
    os.pathsep) if p]
WORK = os.path.join(ROOT, "build", "xn_readable")
INDIRECT = os.path.join(WORK, "indirect.json")
OBJ2 = (0xC0000, 0x161568)

# ---- locations: register parts and flags as bits --------------------------------------------
# EAX..EBX in three parts (L: bits 0-7, H: 8-15, U: 16-31), EBP ESI EDI in two (W: 0-15, U), the
# segment registers and the flags whole. A write to AL defines eaxL only, so `mov dx, [..]`
# reads nothing of EDX.
REG32 = ["eax", "ecx", "edx", "ebx", "ebp", "esi", "edi"]
ABCD = {"eax", "ecx", "edx", "ebx"}
LOCS = []
for _r in REG32:
    LOCS += [_r + p for p in (("L", "H", "U") if _r in ABCD else ("W", "U"))]
LOCS += ["es", "ds", "fs", "gs", "CF", "PF", "AF", "ZF", "SF", "OF", "DF"]
BIT = {n: 1 << k for k, n in enumerate(LOCS)}
RMASK = {}                      # full register -> its parts
for _r in REG32:
    RMASK[_r] = sum(v for k, v in BIT.items() if k[:3] == _r and len(k) == 4)
for _r in ("es", "ds", "fs", "gs"):
    RMASK[_r] = BIT[_r]
GPR = sum(RMASK[r] for r in REG32)
SEG = sum(BIT[n] for n in ("es", "ds", "fs", "gs"))
REGS = GPR | SEG
ARITH = sum(BIT[n] for n in ("CF", "PF", "AF", "ZF", "SF", "OF"))
FLAGS = ARITH | BIT["DF"]
ALL = REGS | FLAGS
FLAG_NAMES = ["CF", "PF", "AF", "ZF", "SF", "OF", "DF"]
WATCOM_ARGS = ["eax", "edx", "ebx", "ecx"]
EFLAGS_BIT = {"CF": 0x1, "PF": 0x4, "AF": 0x10, "ZF": 0x40, "SF": 0x80, "OF": 0x800,
              "DF": 0x400}
# a capstone register name -> (full register, parts mask, value bits VL/VH/VU it covers)
VL, VH, VU = 1, 2, 4
NAME = {}
for _r in ("a", "c", "d", "b"):
    _f = "e%sx" % _r
    NAME[_r + "l"] = (_f, BIT[_f + "L"], VL)
    NAME[_r + "h"] = (_f, BIT[_f + "H"], VH)
    NAME[_r + "x"] = (_f, BIT[_f + "L"] | BIT[_f + "H"], VL | VH)
    NAME[_f] = (_f, RMASK[_f], VL | VH | VU)
for _r in ("bp", "si", "di"):
    _f = "e" + _r
    NAME[_r] = (_f, BIT[_f + "W"], VL | VH)
    NAME[_f] = (_f, RMASK[_f], VL | VH | VU)
for _r in ("es", "ds", "fs", "gs"):
    NAME[_r] = (_r, BIT[_r], VL | VH | VU)
RENDER_FRAME = 0x12A870
RENDER_FRAME_BLOCKS = (0x12A949, 0x12A94E, 0x12A969, 0x12A975, 0x12A976, 0x12A97C, 0x12A97E)
SEG_NAMES = {"es", "ds", "fs", "gs", "ss", "cs"}
VALUE_BITS = {"L": 0xFF, "H": 0xFF00, "U": 0xFFFF0000, "W": 0xFFFF}


def vparts(reg, v):
    """Value bits (VL VH VU) of a register's value -> that register's parts mask."""
    if reg not in REG32:
        return RMASK.get(reg, 0) if v else 0
    if reg in ABCD:
        return (BIT[reg + "L"] if v & VL else 0) | (BIT[reg + "H"] if v & VH else 0) | \
            (BIT[reg + "U"] if v & VU else 0)
    return (BIT[reg + "W"] if v & (VL | VH) else 0) | (BIT[reg + "U"] if v & VU else 0)


def reg_text(r, m):
    """The parts m of register r as text: eax, ax, al, ah; esi, si; odd sets part by part."""
    m &= RMASK[r]
    if not m:
        return []
    if m == RMASK[r]:
        return [r]
    if r in ABCD:
        lo = {BIT[r + "L"] | BIT[r + "H"]: r[1] + "x", BIT[r + "L"]: r[1] + "l",
              BIT[r + "H"]: r[1] + "h"}
        low = m & (BIT[r + "L"] | BIT[r + "H"])
        out = [lo[low]] if low else []
    else:
        out = [r[1:]] if m & BIT[r + "W"] else []
    if m & BIT[r + "U"]:
        out.append(r + ".u")
    return out


def names_of(mask):
    out = []
    for r in REG32 + ["es", "ds", "fs", "gs"]:
        out += reg_text(r, mask)
    out += [f for f in FLAG_NAMES if mask & BIT[f]]
    return " ".join(out)


def mask_of(text):
    m = 0
    for w in (text or "").split():
        if w.endswith(".u"):
            m |= BIT[w[:-2] + "U"]
        elif w in NAME:
            m |= NAME[w][1]
        elif w in BIT:
            m |= BIT[w]
        else:
            raise ValueError("register %r" % w)
    return m


def value_mask(reg, parts):
    """The bits of register reg's 32-bit value that its parts in `parts` cover."""
    v = 0
    for k, n in enumerate(LOCS):
        if parts & BIT[n] and n[:3] == reg and len(n) == 4:
            v |= VALUE_BITS[n[3]]
    if reg in SEG_NAMES and parts & BIT.get(reg, 0):
        v = 0xFFFF
    return v


def _flag_bits():
    """capstone eflags detail -> (reads, writes) per flag name."""
    rd, wr = {}, {}
    for f in ("CF", "PF", "AF", "ZF", "SF", "OF", "DF"):
        rd[f] = getattr(cx, "X86_EFLAGS_TEST_" + f, 0)
        w = 0
        for k in ("MODIFY", "RESET", "SET", "UNDEFINED"):
            w |= getattr(cx, "X86_EFLAGS_%s_%s" % (k, f), 0)
        wr[f] = w
    return rd, wr


FLAG_RD, FLAG_WR = _flag_bits()

from xn_services import SERVICES  # noqa: E402  (int services: inputs, mustdef, maydef)


def service_of(vector, ax):
    """(inputs, mustdef, maydef) masks of an int call. ax: AX when known (< 0x10000), or
    0x10000 | AH << 8 when only AH is, or None."""
    if ax is not None:
        keys = ([(vector, "ax", ax)] if ax < 0x10000 else []) + [(vector, "ah", (ax >> 8) & 0xFF)]
        for key in keys:
            s = SERVICES.get(key)
            if s:
                i, must, may = (mask_of(t) for t in s)
                return i, must, must | may | ARITH
    return GPR & ~RMASK["ebp"] | BIT["es"], 0, GPR & ~RMASK["ebp"] | ARITH


# ---- instruction semantics ------------------------------------------------------------------
class Ins:
    """One instruction's register and flag effects (parts), and its place in the control flow."""
    __slots__ = ("va", "i", "m", "next", "use", "defs", "wv", "fuse", "fkill", "fmay", "kind",
                 "succ", "tails", "callees", "target", "table", "esp", "toks", "live",
                 "service", "note", "summ", "nargs")

    def __init__(self, i, patched_imm=False):
        self.va, self.i, self.m = i.address, i, i.mnemonic
        self.next = i.address + i.size
        self.succ, self.tails, self.callees = [], [], None
        self.target = self.table = None
        self.esp = self.toks = self.live = None
        self.service = None
        self.note = ""
        self.summ = None                # a callee summary of its own (game C's indirect calls)
        self.nargs = None               # game C: the arguments a call passes in registers
        rd, wr = i.regs_access()
        use = defs = 0
        wv = {}                         # full register -> value bits written
        for r in rd:
            e = NAME.get(i.reg_name(r))
            if e:
                use |= e[1]
        for r in wr:
            e = NAME.get(i.reg_name(r))
            if e:
                defs |= e[1]
                wv[e[0]] = wv.get(e[0], 0) | e[2]
        m = self.m
        ops = i.operands
        if m in ("bsf", "bsr") and ops and ops[0].type == cx.X86_OP_REG:
            use |= NAME.get(i.reg_name(ops[0].reg), (0, 0))[1]    # a zero source leaves it
        if m in ("xor", "sub", "sbb") and len(ops) == 2 and ops[0].type == cx.X86_OP_REG and \
                ops[1].type == cx.X86_OP_REG and ops[0].reg == ops[1].reg:
            e = NAME.get(i.reg_name(ops[0].reg))
            if e:
                use &= ~e[1]            # a zero (or -CF) idiom reads nothing
        # flags
        fuse = fw = 0
        ef = i.eflags or 0
        for f in FLAG_NAMES:
            if ef & FLAG_RD[f]:
                fuse |= BIT[f]
            if ef & FLAG_WR[f]:
                fw |= BIT[f]
        names_rd = {i.reg_name(r) for r in rd}
        names_wr = {i.reg_name(r) for r in wr}
        if "eflags" in names_rd and not fuse:
            fuse = FLAGS
        if "eflags" in names_wr and not fw:
            fw = FLAGS
        if m in ("pushfd", "pushf"):
            fuse = FLAGS
        if m in ("popfd", "popf", "iretd", "iret"):
            fw = FLAGS
        fkill, fmay = fw, fw
        base = m.split()[-1]
        if base in ("shl", "sal", "shr", "sar", "rol", "ror", "rcl", "rcr", "shld", "shrd"):
            cnt = ops[-1] if ops else None
            if cnt is not None and (cnt.type != cx.X86_OP_IMM or patched_imm):
                fkill = 0               # a count of 0 leaves the flags as they were
        if m.startswith("rep") and base in ("scasb", "scasw", "scasd", "cmpsb", "cmpsw", "cmpsd"):
            fkill = 0                   # ECX = 0 compares nothing
        self.use, self.defs, self.wv = use, defs, wv
        self.fuse, self.fkill, self.fmay = fuse, fkill, fmay
        self.kind = "seq"


def mem_ops(i):
    """[(op, read, write)] for the explicit memory operands."""
    out = []
    for op in i.operands:
        if op.type == cx.X86_OP_MEM:
            out.append((op, bool(op.access & 1), bool(op.access & 2)))
    if i.mnemonic == "lea":
        return []
    return out


# ---- the code of a function -----------------------------------------------------------------
class Body:
    """A function's instructions (everything reachable from its entry without passing another
    function's entry, as tools/xn_c.py walks it), with control flow and call targets."""

    def __init__(self, entry, insns, obj):
        self.entry, self.insns, self.obj = entry, insns, obj
        self.order = sorted(insns)
        self.exits = []                 # (va, kind)
        self.calls = []                 # va of call instructions
        self.escapes = []               # va: where control leaves for somewhere unknown


class Code:
    """Decoded code of objects 1 and 2 with their function entries and LE fixups."""

    def __init__(self):
        import xn_c
        import fallemu
        from le import SRC_OFF32, SRC_REL32
        self.prog = xn_c.Program()
        self.an = self.prog.an
        self.xn_c = xn_c
        le = self.an.img.le
        self.le = le
        self.fix_at = self.an.img.fix_at
        self.SRC_OFF32, self.SRC_REL32 = SRC_OFF32, SRC_REL32
        self.funcs2 = list(self.prog.funcs)
        self.func2_set = set(self.funcs2)
        with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
            self.funcs1 = sorted(int(r["va"], 16) for r in csv.DictReader(f) if r["obj"] == "1")
        self.func1_set = set(self.funcs1)
        o1 = le.objs[0]
        self.o1 = (o1.base, o1.base + o1.vsize)
        self.rel1 = le.load(relocate=True)[1]
        import capstone
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.md.detail = True
        self._ins1 = {}
        self.fallemu = fallemu

    def decode1(self, va):
        i = self._ins1.get(va)
        if i is None:
            o = va - self.o1[0]
            i = next(self.md.disasm(bytes(self.rel1[o:o + 16]), va), None)
            self._ins1[va] = i
        return i

    def func1_of(self, va):
        k = bisect.bisect_right(self.funcs1, va) - 1
        return self.funcs1[k] if k >= 0 else None

    def table(self, disp):
        """Targets of a jump/call table at disp (consecutive OFF32 fixups into code)."""
        out, k = [], 0
        while True:
            fx = self.fix_at.get(disp + 4 * k)
            if fx is None or fx.kind != self.SRC_OFF32:
                break
            t = fx.target_va
            if not (self.o1[0] <= t < self.o1[1] or OBJ2[0] <= t < OBJ2[1]):
                break
            out.append(t)
            k += 1
        return out

    def body2(self, va):
        fn = self.xn_c.Func(self.prog, va)
        insns = {}
        for a, x in fn.insns.items():
            ins = Ins(x.i, fn.patched_imm(x.i))
            ins.target, ins.table = x.target, x.table
            insns[a] = ins
        return self.link(Body(va, insns, 2), self.func2_set)

    def body1(self, va):
        insns = {}
        work = [va]
        while work:
            a = work.pop()
            while a not in insns:
                if a != va and a in self.func1_set:
                    break
                if not self.o1[0] <= a < self.o1[1]:
                    break
                i = self.decode1(a)
                if i is None:
                    break
                ins = Ins(i)
                insns[a] = ins
                m = i.mnemonic
                if cx.X86_GRP_RET in i.groups or m in ("hlt", "retf", "iretd", "iret"):
                    break
                op = i.operands[0] if i.operands else None
                if m == "jmp":
                    if op.type == cx.X86_OP_IMM:
                        ins.target = op.imm & 0xFFFFFFFF
                        work.append(ins.target)
                    elif op.type == cx.X86_OP_MEM and (op.mem.index or op.mem.base) and \
                            not (op.mem.index and op.mem.base):
                        # Watcom's switch: jmp cs:[eax*4 + table] or, scaled before, [eax + table]
                        ins.table = self.table(op.mem.disp & 0xFFFFFFFF) or None
                        work.extend(ins.table or ())
                    break
                if (cx.X86_GRP_JUMP in i.groups or cx.X86_GRP_BRANCH_RELATIVE in i.groups) and \
                        op is not None and op.type == cx.X86_OP_IMM:
                    ins.target = op.imm & 0xFFFFFFFF
                    work.append(ins.target)
                a += i.size
        return self.link(Body(va, insns, 1), self.func1_set | self.func2_set)

    def is_entry(self, va):
        return va in self.func2_set or va in self.func1_set

    def link(self, b, entries):
        """Successors, exits, tail jumps and call targets of each instruction."""
        for va in b.order:
            x = b.insns[va]
            i, m = x.i, x.m
            g = i.groups
            op = i.operands[0] if i.operands else None

            def edge(t):
                if t in b.insns:
                    x.succ.append(t)
                elif self.is_entry(t):
                    x.tails.append(t)
                else:
                    x.kind = "escape" if x.kind == "seq" else x.kind
                    b.escapes.append(va)
            if cx.X86_GRP_RET in g or m in ("retf", "iretd", "iret", "hlt"):
                x.kind = {"retf": "retf", "iretd": "iret", "iret": "iret", "hlt": "hlt"}.get(m, "ret")
                b.exits.append(va)
                continue
            if m == "jmp":
                if x.target is not None:
                    x.kind = "jmp"
                    edge(x.target)
                elif x.table is not None and x.table:
                    x.kind = "jmp"
                    for t in x.table:
                        edge(t)
                else:
                    x.kind = "ijmp"
                    b.escapes.append(va)
                continue
            if m == "call":
                x.kind = "call"
                b.calls.append(va)
                if op.type == cx.X86_OP_IMM:
                    x.callees = [op.imm & 0xFFFFFFFF]
                elif op.type == cx.X86_OP_MEM and op.mem.index and not op.mem.base and \
                        op.mem.scale == 4:
                    x.callees = self.table(op.mem.disp & 0xFFFFFFFF) or None
                if x.next in b.insns:
                    x.succ.append(x.next)
                elif self.is_entry(x.next):
                    x.tails.append(x.next)
                continue
            if x.target is not None:        # jcc, loop, jecxz
                x.kind = "jcc"
                edge(x.target)
            if m.startswith("int"):
                x.kind = "int"
            if x.next in b.insns:
                x.succ.append(x.next)
            elif self.is_entry(x.next):
                x.tails.append(x.next)
            elif x.kind != "escape":
                x.kind = "escape" if x.kind == "seq" else x.kind
                b.escapes.append(va)
        return b


# ---- callee summaries -----------------------------------------------------------------------
class Summary:
    __slots__ = ("va", "obj", "inputs", "stack_args", "maydef", "mustdef", "ret_pop", "outputs",
                 "preserved", "exits", "notes", "fixed", "stack_known", "ret_pops")

    def __init__(self, va, obj):
        self.va, self.obj = va, obj
        self.inputs = 0
        self.stack_args = 0
        self.maydef = 0
        self.mustdef = ALL
        self.ret_pop = 0
        self.ret_pops = set()
        self.outputs = 0
        self.exits = set()
        self.notes = []
        self.fixed = False          # a summary from outside the analysis (game C, unknown)
        self.stack_known = True


def watcom_summary(va, nparams, returns, varargs=False, stack_only=False):
    """A game C function: arguments in EAX EDX EBX ECX then the stack (callee pops), result in
    EAX, every other register preserved."""
    s = Summary(va, 1)
    s.fixed = True
    n = nparams if nparams is not None else 4
    if varargs or stack_only:
        s.inputs, s.stack_args, s.ret_pop = 0, 4 * n, 0
    else:
        s.inputs = sum(RMASK[r] for r in WATCOM_ARGS[:min(n, 4)])
        s.stack_args = 4 * max(0, n - 4)
        s.ret_pop = s.stack_args
    # Watcom 10.0a code saves every register it uses but EAX and its argument registers
    s.maydef = RMASK["eax"] | s.inputs | ARITH
    s.mustdef = (RMASK["eax"] if returns else 0) | ARITH
    return s


def unknown_summary():
    s = Summary(None, 0)
    s.fixed = True
    s.inputs = GPR
    s.maydef = GPR | ARITH
    s.mustdef = 0
    return s


# ---- the analysis ---------------------------------------------------------------------------
TOK_X = "X"         # maybe modified (some paths still hold the register's entry value)
TOK_M = "M"         # modified


def join_tok(loc, a, b):
    if a == b:
        return a
    if a == TOK_X or b == TOK_X:
        return TOK_X
    e = ("E", loc)
    if a == e or b == e:
        return TOK_X
    return TOK_M


# A register's state in the forward pass is V = (token, maybe-dirty, must-dirty): the token says
# what 32-bit value it was last given whole (("E", r) the entry value of r, ("C", n) a constant,
# ("SP", k) the address ESP+k at entry, M modified, X maybe modified), and the dirty masks (value
# bits VL VH VU) which parts were written since. A stack slot holds a V (a dword pushed or
# stored), ("FL", flag tokens) from pushfd, or ("RA",) the return address.
V_M = (TOK_M, 0, 0)


def v_ident(r):
    return (("E", r), 0, 0)


def join_v(r, a, b):
    if a == b:
        return a
    if not (isinstance(a, tuple) and len(a) == 3 and isinstance(b, tuple) and len(b) == 3 and
            a[0] != "FL" and b[0] != "FL"):
        return V_M
    return (join_tok(r, a[0], b[0]), a[1] | b[1], a[2] & b[2])


def join_key(k, a, b):
    if isinstance(k, int):
        if a is None or b is None:
            return V_M
        return join_v(None, a, b)
    if k == "AXC":
        a, b = a or (None, None), b or (None, None)
        return tuple(x if x == y else None for x, y in zip(a, b))
    if k in FLAG_NAMES:
        return join_tok(k, a if a is not None else ("E", k), b if b is not None else ("E", k))
    return join_v(k, a if a is not None else v_ident(k), b if b is not None else v_ident(k))


def pbits(r, mask):
    """A register's parts in mask -> value bits (VL VH VU)."""
    if r not in REG32:
        return VL | VH | VU if mask & RMASK.get(r, 0) else 0
    if r in ABCD:
        return (VL if mask & BIT[r + "L"] else 0) | (VH if mask & BIT[r + "H"] else 0) | \
            (VU if mask & BIT[r + "U"] else 0)
    return (VL | VH if mask & BIT[r + "W"] else 0) | (VU if mask & BIT[r + "U"] else 0)


def is_sp(v):
    return isinstance(v, tuple) and len(v) == 3 and isinstance(v[0], tuple) and \
        v[0][0] == "SP" and not v[1]


class Analysis:
    def __init__(self, verbose=False):
        self.code = Code()
        self.verbose = verbose
        self.phase = "must"
        self.names = load_names()
        self.bodies = {}
        self.summ = {}
        self.extra = set()          # code inside functions that indirect jumps reach
        self.game = self.game_summaries()
        self.unknown = unknown_summary()
        self.indirect = self.resolve_indirect()
        t0 = time.time()
        for va in self.code.funcs2:
            self.bodies[va] = self.code.body2(va)
            self.summ[va] = Summary(va, 2)
        self.resolve_sites()
        for a, (tg, _how) in list(self.site_targets.items()):
            for t in tg or ():
                if OBJ2[0] <= t < OBJ2[1] and t not in self.code.func2_set and t not in self.summ:
                    self.bodies[t] = self.code.body2(t)     # a jump into a function's body
                    self.summ[t] = Summary(t, 2)
                    self.extra.add(t)
        self.game_callers()
        if verbose:
            print("bodies: %d (%d of the game's) in %.1f s" % (
                len(self.bodies), len(self.code.funcs1), time.time() - t0), flush=True)

    # -- game C ----------------------------------------------------------------------------
    def game_summaries(self):
        """{object 1 address: Summary} from the game's C definitions."""
        import protos
        defs = protos.definitions(protos.default_files())
        addr = {}
        with open(os.path.join(ROOT, "config", "symbols.txt")) as f:
            for line in f:
                m = re.match(r"(\w+) = (0x[0-9A-Fa-f]+); func", line.strip())
                if m:
                    addr[m.group(1)] = int(m.group(2), 16)
        pragma = set()
        for p in protos.default_files():
            for line in open(p, encoding="latin-1"):
                m = re.match(r"#pragma aux (?:\(\w+\) )?(\w+)\b.*", line)
                if m and ("parm routine []" in line or "parm caller []" in line or "(sosconv)" in line):
                    pragma.add(m.group(1))
        out = {}
        for name, (ret, params, _path) in defs.items():
            va = addr.get(name)
            if va is None:
                m = re.match(r"func_([0-9A-F]{8})$", name)
                va = int(m.group(1), 16) if m else None
            if va is None:
                continue
            if params is None or params == ("void",):
                n, var = 0, False
            else:
                var = "..." in params
                n = len([p for p in params if p != "..."])
            out[va] = watcom_summary(va, n, ret.strip() != "void", var, name in pragma)
        return out

    def summary_of(self, va):
        # a game function with a C definition is called by its C prototype: its inputs are its
        # parameters, whatever its compiled code saves and restores or calls (mem_check_heap,
        # 0x6A319, calls an engine check whose analysis made every register an input)
        if va in self.game:
            return self.game[va]
        if va in self.summ:
            return self.summ[va]
        if self.code.o1[0] <= va < self.code.o1[1]:
            return watcom_summary(va, None, True)       # no C definition: a library routine
        return self.unknown

    # -- indirect calls --------------------------------------------------------------------
    def resolve_indirect(self):
        """{site va: [targets]} for indirect calls and jumps resolved by stores or the corpus,
        and {function: how its address is taken}."""
        code = self.code
        an = code.an
        stores = collections.defaultdict(set)       # ("var", addr) / ("field", disp) -> funcs
        self.taken = collections.defaultdict(set)   # func -> {"table", "var", "field", "escape"}
        tables = set()
        for va, i in an.insns.items():
            if i.mnemonic in ("call", "jmp") and i.operands and i.operands[0].type == cx.X86_OP_MEM:
                op = i.operands[0]
                if op.mem.index and not op.mem.base:
                    tables.add(op.mem.disp & 0xFFFFFFFF)
        table_slots = {}
        for t in tables:
            for k, f in enumerate(code.table(t)):
                table_slots[t + 4 * k] = f
        for fx in code.le.fixups():
            if fx.kind != code.SRC_OFF32 or fx.target_va not in code.func2_set:
                continue
            f = fx.target_va
            src = fx.src_va
            if src in table_slots:
                self.taken[f].add("table")
                continue
            ins = None
            if OBJ2[0] <= src < OBJ2[1]:
                a = an.covered.get(src)
                ins = an.insns.get(a) if a is not None else None
                if ins is None:             # a pointer in object 2's data: a variable or table
                    stores[("var", src)].add(f)
                    self.taken[f].add("var")
                    continue
                if ins.disp_size and src == ins.address + ins.disp_offset:
                    continue                # an address used as memory (code patched there)
            elif code.o1[0] <= src < code.o1[1]:
                for back in range(1, 12):
                    i = code.decode1(src - back)
                    if i is not None and i.address + i.size > src and \
                            i.address + i.size - 4 >= src - 4 and src - i.address < i.size:
                        ins = i
                        break
            if ins is not None and ins.mnemonic == "mov" and len(ins.operands) == 2 and \
                    ins.operands[0].type == cx.X86_OP_MEM and ins.operands[1].type == cx.X86_OP_IMM:
                mm = ins.operands[0].mem
                if not mm.base and not mm.index:
                    stores[("var", mm.disp & 0xFFFFFFFF)].add(f)
                    self.taken[f].add("var")
                    continue
                if mm.base and not mm.index:
                    stores[("field", mm.disp)].add(f)
                    self.taken[f].add("field")
                    continue
            if src in an.patch_fields or any(src + k in code.prog.patch_fields for k in range(4)):
                self.taken[f].add("patch")
                continue
            self.taken[f].add("escape")
        self.stores = stores
        dyn = {}
        if os.path.exists(INDIRECT):
            with open(INDIRECT) as fh:
                dyn = {int(k, 16): [int(t, 16) for t in v] for k, v in json.load(fh).items()}
        self.dynamic = dyn
        return dyn

    def resolve_sites(self):
        """Targets of each indirect call or jump in object 2: (targets, how)."""
        self.site_targets = {}
        for va, b in self.bodies.items():
            for a in b.order:
                x = b.insns[a]
                if x.kind not in ("call", "ijmp") or x.callees is not None and x.kind == "call":
                    continue
                op = x.i.operands[0]
                tg, how = None, None
                if op.type == cx.X86_OP_MEM and not op.mem.base and not op.mem.index:
                    s = self.stores.get(("var", op.mem.disp & 0xFFFFFFFF))
                    if s:
                        tg, how = sorted(s), "stores"
                elif op.type == cx.X86_OP_MEM and op.mem.base and not op.mem.index:
                    tb = self.code.table(op.mem.disp & 0xFFFFFFFF)
                    s = self.stores.get(("field", op.mem.disp))
                    if tb:
                        tg, how = sorted(set(tb)), "table"
                    elif s:
                        tg, how = sorted(s), "field"
                d = self.dynamic.get(a)
                if d:
                    # the record corpus' targets: entries of object 2 or the game, code inside
                    # a function (analysed as an entry of its own), or code the engine generated
                    tg = sorted(set(tg or ()) | set(d))
                    how = (how + "+corpus") if how else "corpus"
                    if any(not self.code.o1[0] <= t < OBJ2[1] for t in tg):
                        how += "+generated"
                        tg = None       # generated code: anything
                self.site_targets[a] = (tg, how)
                if tg and x.kind == "call":
                    x.callees = tg
                    x.note = how
                elif tg and x.kind == "ijmp":
                    x.kind = "jmp"
                    x.note = how
                    for t in tg:
                        if t in b.insns:
                            x.succ.append(t)
                        else:
                            x.tails.append(t)
                    if a in b.escapes:
                        b.escapes.remove(a)

    def game_callers(self):
        """Object 1, the game's C, analysed like object 2: a body and a summary per function
        (what is live after a call into XnGine reaches back through the game's own calls). A
        call through a pointer, or of a routine with no body (the C library), takes the
        argument registers its call site loads: Watcom 10.0a -od code loads them just before
        the call. A game function whose address is taken, or that nothing calls (main, the
        extender's entries), has Watcom's convention for its callers: EAX out, all but EAX and
        the argument registers preserved."""
        code = self.code
        self.game_sites = collections.defaultdict(list)
        for fx in code.le.fixups():
            if fx.kind == code.SRC_REL32 and code.o1[0] <= fx.src_va < code.o1[1] and \
                    fx.target_va in code.func2_set:
                self.game_sites[code.func1_of(fx.src_va)].append(fx.src_va - 1)
        self.taken1 = {fx.target_va for fx in code.le.fixups()
                       if fx.kind == code.SRC_OFF32 and fx.target_va in code.func1_set}
        self.game_funcs = []
        for f in code.funcs1:
            try:
                b = code.body1(f)
            except Exception:       # noqa: BLE001  (bytes that do not decode)
                continue
            if not b.insns:
                continue
            self.bodies[f] = b
            self.summ[f] = Summary(f, 1)
            self.game_funcs.append(f)
        for f in self.game_funcs:
            b = self.bodies[f]
            preds = collections.defaultdict(list)
            for a in b.order:
                for t in b.insns[a].succ:
                    preds[t].append(a)
            for a in b.order:
                x = b.insns[a]
                if x.kind != "call":
                    continue
                setr = 0
                k = a
                for _n in range(12):
                    ps = preds.get(k, [])
                    if len(ps) != 1 or b.insns[ps[0]].kind != "seq":
                        break
                    k = ps[0]
                    setr |= b.insns[k].defs
                n = max([j + 1 for j, r in enumerate(WATCOM_ARGS)
                         if setr & RMASK[r] == RMASK[r]] or [0])
                for t in x.callees or ():
                    g = self.game.get(t)
                    if g is not None:       # the prototype the caller was compiled against
                        n = max(n, len([r for r in WATCOM_ARGS if g.inputs & RMASK[r]]))
                x.nargs = n
                if not (x.callees and all(t in self.summ for t in x.callees)):
                    x.summ = watcom_summary(None, n, True)

    # -- forward: ESP, preserved registers -------------------------------------------------
    def callee_list(self, x):
        if x.summ is not None:
            return [x.summ]
        if x.callees is None:
            return [self.unknown]
        return [self.summary_of(t) for t in x.callees]

    def forward(self, b, summ, record=True):
        """ESP offsets and register states to a fixpoint; sets each instruction's esp/toks (on
        entry), and the summary's maydef/mustdef/ret_pop/exits."""
        init = {r: v_ident(r) for r in REG32 + ["es", "ds", "fs", "gs"]}
        for f in FLAG_NAMES:
            init[f] = ("E", f)
        init[0] = ("RA",)
        init["AXC"] = (None, None)
        state = {b.entry: (0, init)}
        work = [b.entry]
        exits = []
        seen = collections.Counter()
        while work:
            va = work.pop()
            x = b.insns[va]
            esp, toks = state[va]
            seen[va] += 1
            for t, st in self.step(b, x, esp, toks, exits):
                old = state.get(t)
                if old is None:
                    state[t] = st
                    work.append(t)
                    continue
                e = old[0] if old[0] == st[0] else None
                nt = {}
                for k in set(old[1]) | set(st[1]):
                    nt[k] = join_key(k, old[1].get(k), st[1].get(k))
                if e != old[0] or nt != old[1]:
                    if seen[t] > 50:        # a loop that keeps changing: give up on precision
                        nt = {k: (v if not isinstance(k, int) else V_M) for k, v in nt.items()}
                        e = None if seen[t] > 60 else e
                    state[t] = (e, nt)
                    work.append(t)
        for va, (esp, toks) in state.items():
            b.insns[va].esp, b.insns[va].toks = esp, toks
        if not record:
            return
        may, must = 0, ALL
        pops = set()
        kinds = set()
        for va, kind, esp, toks, rp in exits:
            kinds.add(kind)
            if kind in ("ret", "tail"):
                pops.add(rp)
            m_here = x_here = 0
            for r in REG32 + ["es", "ds", "fs", "gs"]:
                tok, dmay, dmust = toks.get(r, v_ident(r))
                if tok == ("E", r):
                    x_here |= vparts(r, dmay)
                    m_here |= vparts(r, dmust)
                elif tok == TOK_X:
                    x_here |= RMASK[r]
                    m_here |= vparts(r, dmust)
                else:
                    m_here |= RMASK[r]
            for f in FLAG_NAMES:
                tok = toks.get(f, ("E", f))
                if tok == TOK_X:
                    x_here |= BIT[f]
                elif tok != ("E", f):
                    m_here |= BIT[f]
            may |= m_here | x_here
            must &= m_here
            if kind in ("ret", "tail") and esp not in (0, None):
                summ.notes.append("ESP %+d at the return at %X" % (esp, va))
        if not exits:
            must = 0
        if b.escapes:
            may |= GPR | ARITH
            must = 0
        summ.maydef, summ.mustdef = may, must
        summ.exits = kinds
        summ.ret_pops = pops
        summ.ret_pop = max(pops) if pops else 0
        summ.stack_known = all(b.insns[a].esp is not None for a in b.order)

    def step(self, b, x, esp, toks, exits):
        """One instruction forward: [(successor, (esp, toks))]; exits appended."""
        i, m = x.i, x.m
        t = dict(toks)
        ops = i.operands
        axc = list(t.get("AXC", (None, None)))

        def src32(op):
            """The V of a 32-bit source operand."""
            if op.type == cx.X86_OP_REG:
                n = i.reg_name(op.reg)
                if n == "esp":
                    return (("SP", esp), 0, 0) if esp is not None else V_M
                e = NAME.get(n)
                if e and e[2] == VL | VH | VU:
                    v = t.get(e[0], v_ident(e[0]))
                    return v if len(v) == 3 else V_M
                return V_M
            if op.type == cx.X86_OP_IMM:
                return (("C", op.imm & 0xFFFFFFFF), 0, 0)
            if op.type == cx.X86_OP_MEM and op.size == 4:
                sl = self.slot(i, op, esp, toks)
                if sl is not None:
                    v = t.get(sl, V_M)
                    return v if isinstance(v, tuple) and len(v) == 3 and v[0] != "FL" else V_M
            return V_M

        def dirty(r, bits, must=True):
            tok, dm, du = t.get(r, v_ident(r))
            if bits == VL | VH | VU and must:
                t[r] = V_M
            else:
                t[r] = (tok, dm | bits, du | (bits if must else 0))

        def callee_effect(must_mask, may_mask):
            for r in REG32 + ["es", "ds", "fs", "gs"]:
                mu = pbits(r, must_mask) if self.phase == "must" else 0
                ma = pbits(r, may_mask)
                if ma or mu:
                    tok, dm, du = t.get(r, v_ident(r))
                    if mu == VL | VH | VU:
                        t[r] = V_M
                    else:
                        t[r] = (tok, dm | ma | mu, du | mu)
            for f in FLAG_NAMES:
                if self.phase == "must" and must_mask & BIT[f]:
                    t[f] = TOK_M
                elif may_mask & BIT[f]:
                    t[f] = join_tok(f, t.get(f, ("E", f)), TOK_M)
            if may_mask & RMASK["eax"]:
                if may_mask & BIT["eaxL"]:
                    axc[0] = None
                if may_mask & BIT["eaxH"]:
                    axc[1] = None

        def kill_slots_below(e):
            for k in [k for k in t if isinstance(k, int) and (e is None or k < e)]:
                del t[k]
        new_esp = esp
        handled = True
        op0 = ops[0] if ops else None
        if m == "push":
            n = opsize(i, op0)
            if op0.type == cx.X86_OP_REG and i.reg_name(op0.reg) in SEG_NAMES:
                v = t.get(i.reg_name(op0.reg), V_M)
            elif n == 4:
                v = src32(op0)
            else:
                v = V_M
            new_esp = None if esp is None else esp - n
            if new_esp is not None:
                t[new_esp] = v
                if n != 4:
                    t[new_esp & ~3] = V_M
        elif m == "pop":
            n = opsize(i, op0)
            v = t.get(esp, V_M) if esp is not None else V_M
            if not (isinstance(v, tuple) and len(v) == 3 and v[0] != "FL") or n != 4:
                v = V_M
            new_esp = None if esp is None else esp + n
            if op0.type == cx.X86_OP_REG:
                nm = i.reg_name(op0.reg)
                e = NAME.get(nm)
                if nm == "esp":
                    new_esp = None
                elif nm in SEG_NAMES and nm in t:
                    t[nm] = v
                elif e and e[2] == VL | VH | VU:
                    t[e[0]] = v
                    if e[0] == "eax":
                        axc = [None, None]
                elif e:
                    dirty(e[0], e[2])
            else:
                sl = self.slot(i, op0, new_esp, toks)
                if sl is not None:
                    t[sl] = v
        elif m in ("pushal", "pushad"):
            if esp is not None:
                for k, n in enumerate(("eax", "ecx", "edx", "ebx", None, "ebp", "esi", "edi")):
                    t[esp - 4 * (k + 1)] = t.get(n, v_ident(n)) if n else (("SP", esp), 0, 0)
                new_esp = esp - 32
        elif m in ("popal", "popad"):
            for k, n in enumerate(("edi", "esi", "ebp", None, "ebx", "edx", "ecx", "eax")):
                if n:
                    v = t.get(esp + 4 * k, V_M) if esp is not None else V_M
                    t[n] = v if isinstance(v, tuple) and len(v) == 3 and v[0] != "FL" else V_M
            axc = [None, None]
            new_esp = None if esp is None else esp + 32
        elif m in ("pushfd", "pushf"):
            new_esp = None if esp is None else esp - 4
            if new_esp is not None:
                t[new_esp] = ("FL",) + tuple(t.get(f, ("E", f)) for f in FLAG_NAMES)
        elif m in ("popfd", "popf"):
            v = t.get(esp) if esp is not None else None
            for k, f in enumerate(FLAG_NAMES):
                t[f] = v[1 + k] if isinstance(v, tuple) and v and v[0] == "FL" else TOK_M
            new_esp = None if esp is None else esp + 4
        elif m == "leave":
            v = t.get("ebp")
            new_esp = v[0][1] if is_sp(v) else None
            nv = t.get(new_esp, V_M) if new_esp is not None else V_M
            t["ebp"] = nv if isinstance(nv, tuple) and len(nv) == 3 and nv[0] != "FL" else V_M
            new_esp = None if new_esp is None else new_esp + 4
        elif m == "mov" and len(ops) == 2 and op0.type == cx.X86_OP_REG:
            nm = i.reg_name(op0.reg)
            e = NAME.get(nm)
            if nm == "esp":
                v = src32(ops[1])
                new_esp = v[0][1] if is_sp(v) else None
            elif nm in SEG_NAMES:
                if nm in t:
                    t[nm] = V_M
            elif e and e[2] == VL | VH | VU:
                t[e[0]] = src32(ops[1])
                if e[0] == "eax":
                    c = ops[1].imm & 0xFFFFFFFF if ops[1].type == cx.X86_OP_IMM else None
                    axc = [None if c is None else c & 0xFF, None if c is None else (c >> 8) & 0xFF]
            elif e:
                dirty(e[0], e[2])
                if e[0] == "eax":
                    c = ops[1].imm if ops[1].type == cx.X86_OP_IMM else None
                    if nm == "ax":
                        axc = [None, None] if c is None else [c & 0xFF, (c >> 8) & 0xFF]
                    elif nm == "al":
                        axc[0] = None if c is None else c & 0xFF
                    elif nm == "ah":
                        axc[1] = None if c is None else c & 0xFF
        elif m == "mov" and len(ops) == 2 and op0.type == cx.X86_OP_MEM:
            sl = self.slot(i, op0, esp, toks)
            if sl is not None:
                if op0.size == 4 and sl % 4 == 0:
                    t[sl] = src32(ops[1])
                else:
                    for k in (sl & ~3, (sl + op0.size - 1) & ~3):
                        t[k] = V_M
        elif m == "lea" and op0.type == cx.X86_OP_REG:
            mm = ops[1].mem
            nm = i.reg_name(op0.reg)
            v = V_M
            if not mm.index and mm.base:
                bn = i.reg_name(mm.base)
                bv = (("SP", esp), 0, 0) if bn == "esp" and esp is not None else t.get(bn)
                if is_sp(bv):
                    v = (("SP", bv[0][1] + mm.disp), 0, 0)
            e = NAME.get(nm)
            if nm == "esp":
                new_esp = v[0][1] if is_sp(v) else None
            elif e and e[2] == VL | VH | VU:
                t[e[0]] = v
                if e[0] == "eax":
                    axc = [None, None]
            elif e:
                dirty(e[0], e[2])
        elif m in ("add", "sub") and op0.type == cx.X86_OP_REG and i.reg_name(op0.reg) == "esp":
            if ops[1].type == cx.X86_OP_IMM and esp is not None:
                new_esp = esp + (ops[1].imm if m == "add" else -ops[1].imm)
            else:
                new_esp = None
            for f in FLAG_NAMES[:6]:
                t[f] = TOK_M
        elif m == "xchg" and len(ops) == 2 and all(o.type == cx.X86_OP_REG for o in ops) and \
                all(NAME.get(i.reg_name(o.reg), (0, 0, 0))[2] == VL | VH | VU for o in ops):
            a, c = NAME[i.reg_name(ops[0].reg)][0], NAME[i.reg_name(ops[1].reg)][0]
            t[a], t[c] = t.get(c, v_ident(c)), t.get(a, v_ident(a))
            if "eax" in (a, c):
                axc = [None, None]
        elif x.kind == "call":
            e0 = None if esp is None else esp - 4
            cs = self.callee_list(x)
            must = ALL
            may = 0
            for sm in cs:
                must &= sm.mustdef
                may |= sm.maydef
            callee_effect(must, may)
            kill_slots_below(e0)
            pops = {sm.ret_pop for sm in cs}
            rp = pops.pop() if len(pops) == 1 else None
            new_esp = None if esp is None or rp is None else esp + rp
        elif x.kind == "int":
            vec = op0.imm if op0 is not None else 3
            ax = None
            if axc[1] is not None:
                ax = (axc[1] << 8) | (axc[0] if axc[0] is not None else 0x100 * 0)
                if axc[0] is None:
                    ax = (axc[1] << 8) | 0x10000        # AH known, AL not
            x.service = (vec, ax)
            _inp, must, may = service_of(vec, ax)
            callee_effect(must, may)
        else:
            handled = False
        if not handled and x.kind not in ("ret", "retf", "iret", "hlt"):
            for r, bits in x.wv.items():
                if r in SEG_NAMES:
                    t[r] = V_M
                    continue
                dirty(r, bits)
                if r == "eax":
                    if bits & VL:
                        axc[0] = None
                    if bits & VH:
                        axc[1] = None
            for f in FLAG_NAMES:
                if x.fkill & BIT[f]:
                    t[f] = TOK_M
                elif x.fmay & BIT[f]:
                    t[f] = join_tok(f, t.get(f, ("E", f)), TOK_M)
            for op, rd, wr in mem_ops(i):
                if wr:
                    sl = self.slot(i, op, esp, toks)
                    if sl is not None:
                        for k in (sl & ~3, (sl + op.size - 1) & ~3):
                            t[k] = V_M
            if m == "enter":
                new_esp = None
        t["AXC"] = tuple(axc)
        out = []
        if x.kind in ("ret", "retf", "iret", "hlt"):
            rp = ops[0].imm if (m == "ret" and ops) else 0
            if x.kind == "ret" and esp is not None and esp < 0:
                # `push target; ret`: a jump to an address on the stack, not a return
                if x.va not in b.escapes:
                    b.escapes.append(x.va)
                exits.append((x.va, "escape", esp, t, 0))
                return out
            exits.append((x.va, x.kind, esp, t, rp))
            return out
        if x.kind in ("escape", "ijmp"):
            exits.append((x.va, "escape", esp, t, 0))
        for tg in x.tails:
            sm = self.summary_of(tg)
            saved = dict(t)
            callee_effect(sm.mustdef, sm.maydef)
            exits.append((x.va, "tail", esp, t, sm.ret_pop))
            t = saved
        for sc in x.succ:
            out.append((sc, (new_esp, t)))
        return out

    def slot(self, i, op, esp, toks):
        """The stack slot offset (from the entry ESP) a memory operand addresses, or None."""
        mm = op.mem
        if mm.index or not mm.base:
            return None
        bn = i.reg_name(mm.base)
        if bn == "esp":
            if esp is None:
                return None
            return esp + mm.disp
        v = toks.get(bn) if toks else None
        if is_sp(v):
            return v[0][1] + mm.disp
        return None

    def stack_access(self, i, op, esp, toks):
        """'none' (not the stack), an offset, or 'any' (the stack, somewhere)."""
        mm = op.mem
        if not mm.base and not mm.index:
            return "none"
        s = self.slot(i, op, esp, toks)
        if s is not None:
            return s
        for r in (mm.base, mm.index):
            if r:
                n = i.reg_name(r)
                if n == "esp":
                    return "any"
                v = toks.get(n) if toks else None
                if isinstance(v, tuple) and len(v) == 3 and isinstance(v[0], tuple) and \
                        v[0][0] == "SP":
                    return "any"
        return "none"

    # -- backward: liveness ----------------------------------------------------------------
    def backward(self, b, out_live, record_sites=None):
        """Liveness to a fixpoint. out_live: the live set at a return. Returns live-in at the
        entry (mask, slots); each call's live-after goes to record_sites[va]."""
        live_in = {}
        order = b.order
        preds = collections.defaultdict(list)
        for va in order:
            for s in b.insns[va].succ:
                preds[s].append(va)
        work = list(order)
        inq = set(work)
        while work:
            va = work.pop()
            inq.discard(va)
            x = b.insns[va]
            m, slots = 0, set()
            for s in x.succ:
                li = live_in.get(s)
                if li:
                    m |= li[0]
                    slots |= li[1]
            if x.kind == "call":
                # a call whose next instruction is another function's entry: that function
                # runs after it (a fall-through tail)
                for tg in x.tails:
                    ts = self.summary_of(tg)
                    m |= ts.inputs | (out_live & ~ts.mustdef)
            after = (m, frozenset(slots))
            if record_sites is not None and x.kind == "call":
                record_sites[va] = after
            new = self.transfer(b, x, after, out_live)
            if live_in.get(va) != new:
                live_in[va] = new
                for p in preds[va]:
                    if p not in inq:
                        inq.add(p)
                        work.append(p)
        for va in order:
            b.insns[va].live = live_in.get(va)
        return live_in.get(b.entry, (0, frozenset()))

    def transfer(self, b, x, after, out_live):
        i, m = x.i, x.m
        live, slots = after
        slots = set(slots)
        esp, toks = x.esp, x.toks or {}
        # exits
        if x.kind == "ret" and esp is not None and esp < 0:
            live, slots = ALL, {esp}        # push target; ret: a jump through the stack
        elif x.kind == "ret":
            live, slots = out_live, set()
        elif x.kind in ("iret", "retf"):
            live, slots = REGS, set()
            if esp is not None:
                slots |= {esp, esp + 4, esp + 8}
        elif x.kind == "hlt":
            live, slots = 0, set()
        elif x.kind in ("escape", "ijmp"):
            live |= ALL
            slots = {"any"}
        for tg in x.tails:
            s = self.summary_of(tg)
            live |= s.inputs | (out_live & ~s.mustdef)
            if esp is not None:
                slots |= {esp + 4 + k for k in range(0, s.stack_args, 4)}
        if x.kind == "call":
            cs = self.callee_list(x)
            must = ALL
            for s in cs:
                must &= s.mustdef
            live &= ~must
            for s in cs:
                live |= s.inputs
                if esp is not None:
                    slots |= {esp + k for k in range(0, s.stack_args, 4)}
                elif s.stack_args:
                    slots.add("any")
            if x.i.operands[0].type == cx.X86_OP_REG:
                live |= NAME.get(i.reg_name(x.i.operands[0].reg), (0, 0))[1]
            elif x.i.operands[0].type == cx.X86_OP_MEM:
                live |= self.addr_regs(i, x.i.operands[0])
            if esp is not None:
                slots.discard(esp - 4)
            return live, frozenset(slots)
        if x.kind == "int":
            vec, ax = x.service or (3, None)
            inp, must, _may = service_of(vec, ax)
            return (live & ~must) | inp, frozenset(slots)
        # registers, flags and the stack, as strong liveness: an instruction whose only effects
        # are on registers, flags and stack slots nothing reads later uses nothing (so a pop
        # into a dead register does not make its slot live, nor the push before it the register)
        anyafter = "any" in slots
        if m == "push":
            op = i.operands[0]
            w = esp - opsize(i, op) if esp is not None else None
            needed = anyafter or w is None or w in slots or (w & ~3) in slots
            if w is not None and opsize(i, op) == 4:
                slots.discard(w)
            if needed:
                live |= x.use
                if op.type == cx.X86_OP_MEM:
                    sa = self.stack_access(i, op, esp, toks)
                    if sa == "any":
                        slots.add("any")
                    elif sa != "none":
                        slots |= {sa & ~3, (sa + 3) & ~3}
            return live, frozenset(slots)
        if m == "pop":
            op = i.operands[0]
            if op.type == cx.X86_OP_REG:
                nm = i.reg_name(op.reg)
                b = NAME.get(nm, (0, 0))[1]
                needed = bool(live & b) or nm == "esp"
                live &= ~b
            else:
                needed = True
                live |= self.addr_regs(i, op)
            if needed:
                slots.add(esp if esp is not None else "any")
            return live, frozenset(slots)
        if m in ("pushal", "pushad"):
            for k, n in enumerate(("eax", "ecx", "edx", "ebx", None, "ebp", "esi", "edi")):
                w = esp - 4 * (k + 1) if esp is not None else None
                if n and (anyafter or w is None or w in slots):
                    live |= RMASK[n]
                if w is not None:
                    slots.discard(w)
            return live, frozenset(slots)
        if m in ("popal", "popad"):
            for k, n in enumerate(("edi", "esi", "ebp", None, "ebx", "edx", "ecx", "eax")):
                if n and live & RMASK[n]:
                    slots.add(esp + 4 * k if esp is not None else "any")
                live &= ~RMASK.get(n, 0)
            return live, frozenset(slots)
        if m in ("pushfd", "pushf"):
            w = esp - 4 if esp is not None else None
            if anyafter or w is None or w in slots:
                live |= FLAGS
            if w is not None:
                slots.discard(w)
            return live, frozenset(slots)
        if m in ("popfd", "popf"):
            if live & FLAGS:
                slots.add(esp if esp is not None else "any")
            live &= ~FLAGS
            return live, frozenset(slots)
        if m == "leave":
            return live | RMASK["ebp"], frozenset(slots | {"any"})
        impure = x.kind not in ("seq",) or m.split()[-1] in IMPURE
        reads, wfull, wpart = set(), set(), set()
        for op, rd, wr in mem_ops(i):
            sa = self.stack_access(i, op, esp, toks)
            if sa == "none":
                if wr:
                    impure = True
                continue
            if sa == "any":
                if rd:
                    reads.add("any")
                if wr:
                    impure = True
                continue
            ks = {sa & ~3, (sa + op.size - 1) & ~3}
            if rd:
                reads |= ks
            if wr:
                if op.size == 4 and sa % 4 == 0:
                    wfull.add(sa)
                else:
                    wpart |= ks
        defs = x.defs | x.fmay
        needed = impure or bool(live & defs) or bool((wfull | wpart) & slots) or \
            (anyafter and (wfull or wpart))
        live &= ~(x.defs | x.fkill)
        slots -= wfull
        if needed:
            live |= x.use | x.fuse
            slots |= reads | wpart
        return live, frozenset(slots)

    @staticmethod
    def addr_regs(i, op):
        m = 0
        for r in (op.mem.base, op.mem.index):
            if r:
                m |= NAME.get(i.reg_name(r), (0, 0))[1]
        return m

    # -- the whole program -----------------------------------------------------------------
    def run(self):
        """Both stages to a fixpoint, a worklist each: a function is analysed again only when
        something it depends on changed (a callee's summary, its callers' reads)."""
        t0 = time.time()
        callers_of = collections.defaultdict(set)
        for va in self.analysed():
            for x in self.bodies[va].insns.values():
                for t in list(x.callees or ()) + list(x.tails):
                    callers_of[t].add(va)
        # stage 1: what each function may change, and ret N (tail jumps take their target's),
        # from below with every callee write a "may"; then what it always changes, from above
        # (ALL) with the may sets fixed: each half is monotone. Bottom-up over the call graph:
        # callees first, a recursive group (a strongly connected component) to its fixpoint.
        callees_of = collections.defaultdict(set)
        for g, cs in callers_of.items():
            for c in cs:
                if g in self.summ:
                    callees_of[c].add(g)
        order = sccs(self.analysed(), callees_of)
        for phase in ("may", "must"):
            self.phase = phase
            n = 0
            for group in order:
                for _it in range(100):
                    changed = False
                    for va in group:
                        s = self.summ[va]
                        before = (s.maydef, s.ret_pop) if phase == "may" else s.mustdef
                        s.notes = []
                        self.forward(self.bodies[va], s)
                        n += 1
                        if phase == "must":
                            s.mustdef &= before     # never grows: from above
                        after = (s.maydef, s.ret_pop) if phase == "may" else s.mustdef
                        changed |= after != before
                    if not changed or len(group) == 1 and _it:
                        break
            if self.verbose:
                print("forward (%s): %d function passes over %d groups (%.1f s)" % (
                    phase, n, len(order), time.time() - t0), flush=True)
        # stage 2: inputs and outputs, to a fixpoint
        self.after = {}             # call site -> live after (mask, slots)
        self.used_after = {}
        dirty = set(self.analysed())
        for it in range(60):
            changed_inputs = set()
            for va in self.analysed():
                if va not in dirty:
                    continue
                s = self.summ[va]
                sites = {}
                # what is live after each call: everything the callers read after the return,
                # preserved registers too (a callee may only clobber what nothing reads)
                self.backward(self.bodies[va], self.used_after.get(va, 0), sites)
                self.after.update(sites)
                # the inputs: with only the outputs live at the return (a register the
                # function preserves for its callers is not one of its inputs)
                live = self.backward(self.bodies[va], s.outputs)
                ins = live[0] & ~(BIT["es"] | BIT["ds"] | BIT["DF"])    # flat model, DF = 0
                sl = live[1]
                sa = 0
                if "any" in sl:
                    s.stack_known = False
                for k in sl:
                    if k != "any" and k >= 4:
                        sa = max(sa, k)
                new = (ins, sa)
                if (s.inputs, s.stack_args) != new:
                    s.inputs, s.stack_args = new
                    changed_inputs.add(va)
            # outputs from the call sites
            old_used = dict(self.used_after)
            outs = self.compute_outputs()
            dirty = set()
            for va in changed_inputs:
                dirty |= callers_of.get(va, set())
            for va, o in outs.items():
                s = self.summ[va]
                if o != s.outputs or self.used_after.get(va, 0) != old_used.get(va, 0):
                    s.outputs = o
                    dirty.add(va)
            if self.verbose:
                print("liveness pass %d: %d to do again (%.1f s)" % (it + 1, len(dirty),
                                                                     time.time() - t0), flush=True)
            if not dirty:
                break

    def compute_outputs(self):
        """{func: what callers read after it returns} from the live-after sets of its call
        sites (and, through a tail jump, its caller's callers); conservative where callers are
        unknown. Outputs are these among the registers it may change."""
        used = collections.defaultdict(int)
        self.site_kinds = collections.defaultdict(set)   # func -> {"direct", "game", ...}
        self.site_list = collections.defaultdict(list)
        tails = []
        for va, b in self.bodies.items():
            game = b.obj == 1
            for a in b.order:
                x = b.insns[a]
                if x.kind == "call":
                    after = self.after.get(a, (0, frozenset()))[0]
                    if game:
                        # Watcom 10.0a code never reads, after a call, the registers the
                        # convention lets the callee change: EAX (but as the result) and the
                        # argument registers it passed
                        after &= ~sum(RMASK[r] for r in WATCOM_ARGS[1:min(x.nargs or 0, 4)])
                        after &= ~ARITH
                    for t in (x.callees or ()):
                        if t in self.summ:
                            used[t] |= after
                            k = "game" if game else ("direct" if x.i.operands[0].type ==
                                                     cx.X86_OP_IMM else "indirect")
                            self.site_kinds[t].add(k)
                            self.site_list[t].append(a)
                for t in x.tails:
                    if t in self.summ:
                        tails.append((va, t))
                        self.site_kinds[t].add("tail")
                        self.site_list[t].append(a)
        # unresolved indirect sites: any function whose address is taken may be their target
        self.unresolved_after = 0
        for va, b in self.bodies.items():
            for a in b.order:
                x = b.insns[a]
                if x.kind == "call" and x.callees is None:
                    self.unresolved_after |= self.after.get(a, (0, frozenset()))[0]
        for f in self.game_funcs:
            if f in self.taken1 or not self.site_kinds.get(f):
                used[f] |= self.watcom_live_out(f)
        for f in self.code.funcs2:
            how = self.taken.get(f, set())
            if how:
                used[f] |= self.unresolved_after
            if "escape" in how or "patch" in how or "field" in how and not self.site_kinds.get(f):
                used[f] |= GPR | self.status_flags(f)
                self.site_kinds[f].add("escape")
            if not self.site_kinds.get(f) and not self.is_handler(f):
                used[f] |= GPR | self.status_flags(f)   # no caller: every register
            if self.is_handler(f):
                used[f] |= REGS
        # xn_render_frame's run-time blocks are pieces of it (docs: docs/engine/smc/
        # render.md): the span routines return into them, and their `popal; ret` returns from
        # render_frame to the game. What render_frame's callers read is all that is live there.
        for b in RENDER_FRAME_BLOCKS:
            if b in used:
                used[b] = used[RENDER_FRAME]
        for _k in range(20):                # a tail jump returns to its caller's callers
            ch = False
            for g, t in tails:
                n = used[t] | used[g]
                if n != used[t]:
                    used[t] = n
                    ch = True
            if not ch:
                break
        self.used_after = used
        return {f: used[f] & self.summ[f].maydef for f in self.analysed()}

    def analysed(self):
        """Object 2's functions, code inside them that indirect jumps reach, and the game's."""
        return list(self.code.funcs2) + sorted(self.extra) + self.game_funcs

    def status_flags(self, f):
        """CF when the function sets it as a status (clc/stc/cmc), for callers we cannot see."""
        b = self.bodies[f]
        return BIT["CF"] if any(b.insns[a].m in ("clc", "stc", "cmc") for a in b.order) else 0

    def watcom_live_out(self, f):
        """What a game function's callers read after it returns: EAX when it returns a value
        and the registers Watcom 10.0a preserves (all but EAX and the argument registers)."""
        s = self.game.get(f)
        if s is None:
            return GPR
        return (GPR & ~s.maydef) | (RMASK["eax"] if s.mustdef & RMASK["eax"] else 0)

    def is_handler(self, f):
        return f in HANDLERS() or bool(self.summ[f].exits & {"iret", "retf"})


RENDER_FRAME = 0x12A870
RENDER_FRAME_BLOCKS = (0x12A949, 0x12A94E, 0x12A969, 0x12A975, 0x12A976, 0x12A97C, 0x12A97E)
SEG_NAMES = {"es", "ds", "fs", "gs", "ss", "cs"}
# instructions with effects beyond registers, flags and the stack (a divide may fault)
IMPURE = {"div", "idiv", "in", "out", "insb", "insw", "insd", "outsb", "outsw", "outsd",
          "movsb", "movsw", "movsd", "stosb", "stosw", "stosd", "cli", "sti", "hlt", "int",
          "lodsb", "lodsw", "lodsd", "scasb", "scasw", "scasd", "cmpsb", "cmpsw", "cmpsd",
          "cld", "std", "lgdt", "lidt", "invlpg", "wbinvd", "enter"}


def opsize(i, op):
    """Bytes a push or pop of this operand moves (a segment register: 4 in 32-bit code)."""
    if op.type == cx.X86_OP_REG and i.reg_name(op.reg) in SEG_NAMES:
        return 4
    if op.type == cx.X86_OP_IMM:
        return 2 if 0x66 in i.prefix else 4
    return 2 if op.size == 2 else 4
FLAG_NAMES = ["CF", "PF", "AF", "ZF", "SF", "OF", "DF"]
_handlers = None


def sccs(nodes, succ):
    """The strongly connected components of the graph, each a list, callees (successors)
    before their callers (Tarjan's, iterative)."""
    index, low, on, stack, out = {}, {}, set(), [], []
    counter = [0]
    for root in nodes:
        if root in index:
            continue
        work = [(root, iter(sorted(succ.get(root, ()))))]
        index[root] = low[root] = counter[0]
        counter[0] += 1
        stack.append(root)
        on.add(root)
        while work:
            v, it = work[-1]
            w = next(it, None)
            if w is not None:
                if w not in index:
                    index[w] = low[w] = counter[0]
                    counter[0] += 1
                    stack.append(w)
                    on.add(w)
                    work.append((w, iter(sorted(succ.get(w, ())))))
                elif w in on:
                    low[v] = min(low[v], index[w])
                continue
            work.pop()
            if work:
                low[work[-1][0]] = min(low[work[-1][0]], low[v])
            if low[v] == index[v]:
                comp = []
                while True:
                    w = stack.pop()
                    on.discard(w)
                    comp.append(w)
                    if w == v:
                        break
                out.append(sorted(comp))
    return out


def HANDLERS():
    global _handlers
    if _handlers is None:
        with open(os.path.join(ROOT, "config", "xngine_seeds.csv"), newline="") as f:
            _handlers = {int(r["va"], 16) for r in csv.DictReader(f) if r["source"] != "executed"}
    return _handlers


def load_names():
    out = {}
    with open(os.path.join(ROOT, "config", "names.csv"), newline="") as f:
        for r in csv.DictReader(f):
            if r["kind"] == "func":
                out[int(r["address"], 16)] = r["name"]
    return out


def subsystem(name):
    return name.split("_")[1] if name.startswith("xn_") and name.count("_") >= 1 else ""


TEMPLATES = {0x136C30, 0x136C84, 0x136D10, 0x15C300}


def data_functions():
    """Entries in the function list that are bytes, not code (no instruction decodes there, or
    tools/xn_c.py translated nothing)."""
    p = os.path.join(ROOT, "build", "xngine", "c_work", "translate.json")
    out = set()
    if os.path.exists(p):
        for k, v in json.load(open(p)).items():
            if not v.get("insns"):
                out.add(int(k, 16))
    return out


COLUMNS = ("va", "name", "subsystem", "convention", "inputs", "stack_args", "outputs",
           "flags_out", "clobber", "ret_pop", "callers", "notes")


def rows(an):
    data = data_functions()
    out = []
    for va in an.code.funcs2:
        s = an.summ[va]
        b = an.bodies[va]
        name = an.names.get(va, "func_%08X" % va)
        kinds = an.site_kinds.get(va, set())
        notes = list(dict.fromkeys(s.notes))
        inputs = s.inputs & (GPR | BIT["fs"] | BIT["gs"] | FLAGS)
        used = an.used_after.get(va, 0)
        outputs = s.outputs & (GPR | BIT["fs"] | BIT["gs"])
        flags_out = used & s.maydef & ARITH         # DF: 0 by convention, as Watcom assumes
        kept = used & ARITH & ~s.maydef
        if kept:
            notes.append("callers read flags it leaves alone: %s" % names_of(kept))
        clobber = GPR & ~used
        if "escape" in kinds:
            callers = "unknown"
            notes.append("address escapes (%s): every register it may change is an output" %
                         " ".join(sorted(an.taken.get(va, ()))))
        elif not kinds:
            callers = "unknown"
            notes.append("no caller: every register it may change is an output")
        elif "indirect" in kinds or an.taken.get(va):
            callers = "partial"
        else:
            callers = "known"
        if "game" in kinds:
            notes.append("called from the game's C (%d sites)" % sum(
                1 for a in an.site_list[va] if not OBJ2[0] <= a < OBJ2[1]))
        if b.escapes:
            notes.append("leaves through %s" % " ".join("%X" % a for a in b.escapes[:4]))
        if not s.stack_known:
            notes.append("stack use not followed")
        if len(s.ret_pops) > 1:
            notes.append("ret N differs: %s" % sorted(s.ret_pops))
        if flags_out & ~BIT["DF"] and s.exits:
            pass
        conv = "reg"
        if va in data:
            conv = "data"
        elif va in TEMPLATES:
            conv = "template"
        elif an.is_handler(va):
            conv = "interrupt"
        elif b.escapes or not s.stack_known:
            conv = "unknown"
        elif fits_watcom(inputs, s.stack_args, outputs, flags_out, s.ret_pop, clobber):
            conv = "watcom"
        out.append({"va": "%06X" % va, "name": name, "subsystem": subsystem(name),
                    "convention": conv, "inputs": names_of(inputs),
                    "stack_args": s.stack_args, "outputs": names_of(outputs),
                    "flags_out": names_of(flags_out), "clobber": names_of(clobber),
                    "ret_pop": s.ret_pop, "callers": callers, "notes": "; ".join(notes)})
    return out


def watcom_params(inputs, stack_args):
    """How many arguments a Watcom prototype needs for these inputs (EAX EDX EBX ECX in that
    order, then 4 bytes each on the stack)."""
    n = 0
    for k, r in enumerate(WATCOM_ARGS):
        if inputs & RMASK[r]:
            n = k + 1
    if stack_args:
        n = 4 + (stack_args + 3) // 4
    return n


def fits_watcom(inputs, stack_args, outputs, flags_out, ret_pop, clobber):
    """A plain Watcom prototype fits: arguments in EAX EDX EBX ECX (in that order), then the
    stack (the callee pops it); at most EAX out; no flags out; and the registers such a C
    function may change (EAX and its argument registers: Watcom 10.0a preserves the rest) are
    outputs or clobbers."""
    if inputs & ~(RMASK["eax"] | RMASK["edx"] | RMASK["ebx"] | RMASK["ecx"]):
        return False
    if outputs & ~RMASK["eax"] or flags_out:
        return False
    if stack_args and ret_pop != stack_args:
        return False
    if not stack_args and ret_pop:
        return False
    n = watcom_params(inputs, stack_args)
    may = RMASK["eax"] | sum(RMASK[r] for r in WATCOM_ARGS[:min(n, 4)])
    return not (may & ~(clobber | outputs))


def apply_overrides(out):
    by = {r["va"]: r for r in out}
    for path in OVERRIDE_FILES:
        if not os.path.exists(path):
            continue
        with open(path, newline="") as f:
            overrides = list(csv.DictReader(f))
        for o in overrides:
            va = "%06X" % int(o["va"], 16)
            r = by.get(va)
            if r is None or o["field"] not in COLUMNS:
                continue
            r[o["field"]] = o["value"]
            r["notes"] = (r["notes"] + "; " if r["notes"] else "") + "%s overridden: %s" % (
                o["field"], o["reason"])
            if o["field"] in ("outputs", "inputs", "flags_out", "clobber") and \
                    r["convention"] in ("reg", "watcom"):
                r["convention"] = "watcom" if fits_watcom(
                    mask_of(r["inputs"]), int(r["stack_args"] or 0), mask_of(r["outputs"]),
                    mask_of(r["flags_out"]), int(r["ret_pop"] or 0),
                    mask_of(r["clobber"])) else "reg"
    return out


def write_csv(out, path=ABI_CSV):
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=COLUMNS)
        w.writeheader()
        w.writerows(out)


def read_abi(path=ABI_CSV):
    """{va: row} with masks: row["in"], row["out"], row["fout"], row["clob"]."""
    out = {}
    with open(path, newline="") as f:
        for r in csv.DictReader(f):
            va = int(r["va"], 16)
            r["in"] = mask_of(r["inputs"])
            r["out"] = mask_of(r["outputs"])
            r["fout"] = mask_of(r["flags_out"])
            r["clob"] = mask_of(r["clobber"])
            r["stack_args"] = int(r["stack_args"] or 0)
            r["ret_pop"] = int(r["ret_pop"] or 0)
            out[va] = r
    return out


# ---- ABI-aware comparison -------------------------------------------------------------------
EXIT_REGS = ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp")
STACK_DEAD = 0x10000        # bytes below the exit ESP that count as dead stack


def reg_masks(row):
    """{exit register: (bits compared with the recorded exit, bits compared with the entry)}
    for a row: outputs against the record, preserved parts against their entry values,
    clobbers not at all."""
    out = {}
    for r in EXIT_REGS:
        if r == "esp":
            out[r] = (0xFFFFFFFF, 0)
            continue
        if row is None:
            out[r] = (0xFFFFFFFF, 0)
            continue
        o = value_mask(r, row["out"])
        c = value_mask(r, row["clob"])
        out[r] = (o, 0xFFFFFFFF & ~o & ~c)
    return out


def flag_mask(row):
    if row is None:
        return sum(EFLAGS_BIT[f] for f in FLAG_NAMES if f != "DF")
    return sum(EFLAGS_BIT[f] for f in FLAG_NAMES if row["fout"] & BIT[f])


def abi_compare(rec, got, row, entry=None):
    """The differences between a replay (got: xn_record.call_once's result) and the record,
    as the function's interface sees them: the output registers against the record, the
    preserved ones against their entry values (entry: the registers this replay started with,
    default the record's), clobbers not at all; the flags in flags_out only; every byte
    written except the dead stack below the exit ESP; port I/O and interrupts exactly."""
    diffs = []
    if not got["returned"]:
        diffs.append("did not return")
    entry = dict(rec["entry"], **(entry or {}))
    for r, (mo, mp) in reg_masks(row).items():
        g = got["exit"][r]
        if (g ^ rec["exit"][r]) & mo:
            diffs.append("%s %08X != %08X" % (r, g, rec["exit"][r]))
        elif (g ^ entry[r]) & mp:
            diffs.append("%s %08X != entry %08X (preserved)" % (r, g, entry[r]))
    fm = flag_mask(row)
    if (got["exit"]["eflags"] ^ rec["exit"]["eflags"]) & fm:
        diffs.append("flags %03X != %03X" % (got["exit"]["eflags"] & fm,
                                              rec["exit"]["eflags"] & fm))
    lo, hi = rec["exit"]["esp"] - STACK_DEAD, rec["exit"]["esp"]
    gw = {a: v for a, v in got["writes"].items() if not lo <= a < hi}
    rw = {a: v for a, v in rec["writes"].items() if not lo <= a < hi}
    if gw != rw:
        extra = set(gw) ^ set(rw)
        wrong = [a for a in set(gw) & set(rw) if gw[a] != rw[a]]
        diffs.append("writes: %d addresses differ in the set, %d in value%s" % (
            len(extra), len(wrong), " (first %#x)" % min(extra | set(wrong))
            if extra or wrong else ""))
    if got["io"] != rec["io"]:
        diffs.append("port I/O or interrupts differ")
    return diffs


# ---- commands -------------------------------------------------------------------------------
RAW_CSV = os.path.join(WORK, "abi_raw.csv")


def overrides():
    """config/xngine_abi.csv again from the analysis' own rows (build/xn_readable/abi_raw.csv)
    and config/xngine_abi_override.csv: seconds, not the analysis' minutes."""
    with open(RAW_CSV, newline="") as f:
        out = list(csv.DictReader(f))
    out = apply_overrides(out)
    write_csv(out)
    n = sum(1 for r in out if "overridden" in r["notes"])
    print("%d rows, %d overridden -> %s" % (len(out), n, os.path.relpath(ABI_CSV, ROOT)))
    return out


def analyze(verbose=False):
    t0 = time.time()
    an = Analysis(verbose)
    an.run()
    raw = rows(an)
    os.makedirs(WORK, exist_ok=True)
    write_csv(raw, RAW_CSV)
    out = apply_overrides([dict(r) for r in raw])
    write_csv(out)
    c = collections.Counter(r["convention"] for r in out)
    k = collections.Counter(r["callers"] for r in out)
    print("%d functions -> %s in %.0f s" % (len(out), os.path.relpath(ABI_CSV, ROOT),
                                            time.time() - t0))
    print("  conventions: " + ", ".join("%s %d" % kv for kv in c.most_common()))
    print("  callers: " + ", ".join("%s %d" % kv for kv in k.most_common()))
    print("  flags_out: %d functions; more than one output register: %d" % (
        sum(1 for r in out if r["flags_out"]),
        sum(1 for r in out if len(r["outputs"].split()) > 1)))
    return an


def show(va):
    an = Analysis()
    an.run()
    s = an.summ[va]
    b = an.bodies[va]
    r = next(r for r in rows(an) if int(r["va"], 16) == va)
    for k in COLUMNS:
        print("%-11s %s" % (k, r[k]))
    print("maydef %s | mustdef %s" % (names_of(s.maydef), names_of(s.mustdef)))
    sites = an.site_list.get(va, [])
    for a in sites[:12]:
        print("  site %X live after: %s" % (a, names_of(an.after.get(a, (0,))[0])))
    for a in b.order:
        x = b.insns[a]
        lv = x.live or (0, frozenset())
        sl = sorted(k for k in lv[1] if k != "any")
        print("%06X %-34s esp=%-5s live=%s%s%s" % (
            a, (x.m + " " + x.i.op_str)[:34], x.esp, names_of(lv[0]),
            " slots=%s" % sl if sl else "", " [any]" if "any" in lv[1] else ""))


def check(max_per=0):
    """Every record: the registers a row says are preserved hold their entry values at exit,
    and the outputs are among the registers the function may change."""
    import xn_record
    abi = read_abi()
    bad = collections.defaultdict(collections.Counter)
    seen = collections.Counter()
    t0 = time.time()
    for path in xn_record.record_files(xn_record.OUT):
        try:
            recs, _store = xn_record.read_raw(path)
        except Exception:       # noqa: BLE001
            continue
        for rec in recs:
            va = rec["func"]
            row = abi.get(va)
            if row is None or rec.get("via") in ("handler", "interrupt"):
                continue
            if max_per and seen[va] >= max_per:
                continue
            seen[va] += 1
            for r, (mo, mp) in reg_masks(row).items():
                if r != "esp" and (rec["exit"][r] ^ rec["entry"][r]) & mp:
                    bad[va][r] += 1
            if row["convention"] != "interrupt" and \
                    rec["exit"]["esp"] != rec["entry"]["esp"] + 4 + row["ret_pop"]:
                bad[va]["esp"] += 1
    n = sum(seen.values())
    print("%d records of %d functions in %.0f s" % (n, len(seen), time.time() - t0))
    if not bad:
        print("every preserved register comes back with its entry value; ESP by ret N")
    for va, c in sorted(bad.items()):
        print("  %06X %-34s %s (of %d)" % (va, abi[va]["name"], dict(c), seen[va]))
    return 1 if bad else 0


# ---- dynamic cross-checks on the record corpus -----------------------------------------------
SCRAMBLE = 0x5AC3A55C


def scrambled(v, bits, k=0):
    """v with the bits in `bits` changed (a pattern that differs per register)."""
    r = (k * 5) % 32
    pat = ((SCRAMBLE << r) | (SCRAMBLE >> (32 - r))) & 0xFFFFFFFF if r else SCRAMBLE
    return ((v & ~bits) | ((v ^ pat) & bits)) & 0xFFFFFFFF


def ret_sites():
    """Every ret/retf/iret instruction in object 2."""
    import xn_record
    out = []
    cm = xn_record.codemap()
    import xn_c
    an = xn_c.load_analysis()
    for a, i in an.insns.items():
        if i.mnemonic in ("ret", "retf", "iretd", "iret"):
            out.append(a)
    del cm
    return sorted(out)


class Scrambler:
    """Unicorn hooks that, when a function in `funcs` returns, change its clobber registers
    (and the flags it does not output) before the return runs: a register no caller reads
    after the call can hold anything."""

    def __init__(self, abi, funcs, rets):
        self.abi, self.funcs, self.rets = abi, set(funcs), rets
        self.pending = []
        self.fired = collections.Counter()
        self.masks = {}
        for va in self.funcs:
            row = abi.get(va)
            if row is None:
                continue
            regs = {r: value_mask(r, row["clob"]) for r in EXIT_REGS[:-1]}
            fl = sum(EFLAGS_BIT[f] for f in FLAG_NAMES[:6] if not row["fout"] & BIT[f])
            self.masks[va] = (regs, fl)

    def install(self, emu):
        import fallemu
        from unicorn import UC_HOOK_CODE
        self.pending = []
        hooks = []
        esp_id = fallemu.R["esp"]

        def on_entry(uc, address, size, va):
            self.pending.append((va, uc.reg_read(esp_id)))

        def on_ret(uc, address, size, _):
            esp = uc.reg_read(esp_id)
            while self.pending and self.pending[-1][1] < esp:
                self.pending.pop()          # a frame unwound without returning
            while self.pending and self.pending[-1][1] == esp:
                va, _e = self.pending.pop()
                regs, fl = self.masks[va]
                for k, (r, bits) in enumerate(regs.items()):
                    if bits:
                        rid = fallemu.R[r]
                        uc.reg_write(rid, scrambled(uc.reg_read(rid), bits, k + 1))
                if fl:
                    rid = fallemu.R["eflags"]
                    uc.reg_write(rid, uc.reg_read(rid) ^ fl)
                self.fired[va] += 1
        for va in self.masks:
            a = fallemu.LOAD + va
            hooks.append(emu.uc.hook_add(UC_HOOK_CODE, on_entry, va, a, a))
        for va in self.rets:
            a = fallemu.LOAD + va
            hooks.append(emu.uc.hook_add(UC_HOOK_CODE, on_ret, None, a, a))
        return hooks


def _records(files, funcs=None, max_per=0, ran_in=None):
    """Records of these files (of `funcs`, or whose code ran one of `ran_in`)."""
    import xn_record
    seen = collections.Counter()
    for path in files:
        try:
            recs, store = xn_record.read_raw(path)
        except Exception:       # noqa: BLE001
            continue
        for rec in recs:
            va = rec["func"]
            if funcs is not None and va not in funcs:
                continue
            if ran_in is not None:
                hit = [f for f in ran_in if any(a <= f < a + n for a, n in rec.get("blocks", ()))]
                if not hit:
                    continue
                key = hit[0]
            else:
                key = va
            if max_per and seen[key] >= max_per:
                continue
            seen[key] += 1
            yield rec if store is None else xn_record.v1_record(rec, store)


def _clobber_task(files, sp):
    """Replay records with the scramblers of sp["funcs"] (None: every function) installed;
    ABI-aware compare at each record's own function."""
    import xn_record
    import xn_cload
    xn_cload.longer_calls(xn_record)
    abi = read_abi()
    rets = ret_sites()
    funcs = sp["funcs"] or sorted(abi)
    sc = Scrambler(abi, funcs, rets)
    ran_in = set(sp["funcs"]) if sp["funcs"] else None
    stats = {}
    fired = collections.Counter()
    for rec in _records(files, None, sp["max_per"], ran_in):
        hooks = []

        def patch(emu):
            hooks.extend(sc.install(emu))
        sc.fired = collections.Counter()
        try:
            got = xn_record.replay_run(rec, patch=patch)
            row = abi.get(rec["func"])
            diffs = [got["stopped"]] if "stopped" in got else abi_compare(rec, got, row)
        except Exception as e:      # noqa: BLE001
            diffs = ["error: %r" % e]
            xn_cload.drop_machine(xn_record, rec["base"])
        finally:
            emu = xn_cload.machine_of(xn_record, rec["base"])
            if emu is not None:
                for h in hooks:
                    emu.uc.hook_del(h)
        fired.update(sc.fired)
        st = stats.setdefault(rec["func"], [0, 0, []])
        st[1] += 1
        if not diffs:
            st[0] += 1
        elif len(st[2]) < 3:
            st[2].append(["%X@%d" % (rec["func"], rec.get("tick", 0))] + diffs[:3] +
                         ["scrambled: " + " ".join("%X" % f for f in sorted(sc.fired))[:300]])
    return {"stats": stats, "fired": dict(fired)}


def _inputs_task(files, sp):
    """Replay each record with the registers that are not inputs scrambled at entry."""
    import xn_record
    import xn_cload
    xn_cload.longer_calls(xn_record)
    abi = read_abi()
    funcs = set(sp["funcs"]) if sp["funcs"] else None
    stats = {}
    for rec in _records(files, funcs, sp["max_per"]):
        row = abi.get(rec["func"])
        if row is None or row["convention"] in ("interrupt", "data", "template"):
            continue
        entry = {}
        for k, r in enumerate(EXIT_REGS[:-1]):
            bits = 0xFFFFFFFF & ~value_mask(r, row["in"])
            entry[r] = scrambled(rec["entry"][r], bits, k + 1)
        fl = sum(EFLAGS_BIT[f] for f in FLAG_NAMES[:6] if not row["in"] & BIT[f])
        entry["eflags"] = rec["entry"]["eflags"] ^ fl
        cmp_row = row
        if any(e[0] == "int" for e in rec["io"]):
            # a service call replays its recorded registers, scrambled or not: compare the
            # outputs, memory and I/O, not the registers the function preserves
            cmp_row = dict(row, clob=GPR & ~row["out"])
        try:
            got = xn_record.replay_run(rec, entry=entry)
            diffs = [got["stopped"]] if "stopped" in got else abi_compare(rec, got, cmp_row,
                                                                          entry)
        except Exception as e:      # noqa: BLE001
            diffs = ["error: %r" % e]
            xn_cload.drop_machine(xn_record, rec["base"])
        st = stats.setdefault(rec["func"], [0, 0, []])
        st[1] += 1
        if not diffs:
            st[0] += 1
        elif len(st[2]) < 3:
            st[2].append(diffs[:3])
    return {"stats": stats}


def indirect_sites():
    """{site va: capstone instruction} for every indirect call and jump in object 2."""
    import xn_c
    an = xn_c.load_analysis()
    return {a: i for a, i in an.insns.items() if i.mnemonic in ("call", "jmp") and i.operands
            and i.operands[0].type != cx.X86_OP_IMM}


def _trace_task(files, sp):
    """Replay records (asm) with a hook at each indirect call and jump: where they went."""
    import xn_record
    import xn_cload
    import fallemu
    from unicorn import UC_HOOK_CODE
    import capstone
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True
    sites = sorted(indirect_sites())
    seen = collections.defaultdict(set)

    def on_site(uc, address, size, _):
        i = next(md.disasm(bytes(uc.mem_read(address, 16)), address), None)
        if i is None:
            return
        op = i.operands[0]
        try:
            if op.type == cx.X86_OP_REG:
                t = uc.reg_read(fallemu.R[i.reg_name(op.reg)])
            else:
                ea = op.mem.disp
                if op.mem.base:
                    ea += uc.reg_read(fallemu.R[i.reg_name(op.mem.base)])
                if op.mem.index:
                    ea += uc.reg_read(fallemu.R[i.reg_name(op.mem.index)]) * op.mem.scale
                t = struct.unpack("<I", bytes(uc.mem_read(ea & 0xFFFFFFFF, 4)))[0]
        except Exception:       # noqa: BLE001
            return
        seen[address - fallemu.LOAD].add((t - fallemu.LOAD) & 0xFFFFFFFF)
    for rec in _records(files, None, sp["max_per"]):
        hooks = []

        def patch(emu):
            for a in sites:
                hooks.append(emu.uc.hook_add(UC_HOOK_CODE, on_site, None, fallemu.LOAD + a,
                                             fallemu.LOAD + a))
        try:
            xn_record.replay_run(rec, patch=patch)
        except Exception:       # noqa: BLE001
            xn_cload.drop_machine(xn_record, rec["base"])
        finally:
            emu = xn_cload.machine_of(xn_record, rec["base"])
            if emu is not None:
                for h in hooks:
                    emu.uc.hook_del(h)
    return {"%X" % k: sorted("%X" % t for t in v) for k, v in seen.items()}


def corpus_files():
    import xn_cload
    return xn_cload.record_files(None)


def run_dynamic(kind, funcs=None, all_at_once=False, jobs=None, max_per=0, verbose=False):
    """clobber / inputs / trace over the record corpus, in worker processes."""
    import xn_cload
    t0 = time.time()
    files = corpus_files()
    if kind == "trace":
        res = xn_cload.parallel("xn_abi:_trace_task", files, {"max_per": max_per}, jobs)
        merged = collections.defaultdict(set)
        for r in res:
            for k, v in r.items():
                merged[k] |= set(v)
        os.makedirs(WORK, exist_ok=True)
        with open(INDIRECT, "w") as f:
            json.dump({k: sorted(v) for k, v in sorted(merged.items())}, f, indent=0)
        print("%d indirect sites ran, %d targets -> %s in %.0f s" % (
            len(merged), sum(len(v) for v in merged.values()), os.path.relpath(INDIRECT, ROOT),
            time.time() - t0))
        return 0
    if kind == "clobber" and not funcs and not all_at_once:
        print("clobber: give FUNC ... or --all")
        return 2
    spec = {"funcs": funcs, "max_per": max_per}
    res = xn_cload.parallel("xn_abi:_%s_task" % kind, files, spec, jobs)
    stats, fired = {}, collections.Counter()
    for r in res:
        fired.update({int(k, 16) if isinstance(k, str) and not k.isdigit() else int(k): v
                      for k, v in r.get("fired", {}).items()})
        for k, v in r["stats"].items():
            st = stats.setdefault(int(k), [0, 0, []])
            st[0] += v[0]
            st[1] += v[1]
            st[2] += v[2]
    abi = read_abi()
    n_ok = sum(v[0] for v in stats.values())
    n = sum(v[1] for v in stats.values())
    bad = {k: v for k, v in stats.items() if v[0] != v[1]}
    print("%s: %d / %d records pass (%d functions' records, %d with a difference) in %.0f s" % (
        kind, n_ok, n, len(stats), len(bad), time.time() - t0))
    if kind == "clobber":
        print("  scramblers fired in %d functions (%d returns)" % (len(fired), sum(fired.values())))
    for va, v in sorted(bad.items()):
        print("  %06X %-36s %d/%d  %s" % (va, abi.get(va, {}).get("name", "?"), v[0], v[1],
                                         " | ".join("; ".join(d) for d in v[2][:2])[:400]))
    # an agent's runs (XN_RC_OUT set) keep their results apart from the shared ones
    out = os.path.join(os.environ.get("XN_RC_OUT") or WORK, "%s.json" % kind)
    os.makedirs(WORK, exist_ok=True)
    with open(out, "w") as f:
        json.dump({"stats": {"%06X" % k: v for k, v in stats.items()},
                   "fired": {"%06X" % k: v for k, v in fired.items()},
                   "all": all_at_once or not funcs}, f, indent=0)
    return 0 if not bad else 1


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    a1 = sub.add_parser("analyze")
    a1.add_argument("-v", action="store_true")
    sub.add_parser("overrides")
    s = sub.add_parser("show")
    s.add_argument("func")
    c = sub.add_parser("check")
    c.add_argument("--max-per", type=int, default=0)
    for name in ("clobber", "inputs", "trace"):
        p = sub.add_parser(name)
        p.add_argument("funcs", nargs="*")
        p.add_argument("--all", action="store_true")
        p.add_argument("-j", "--jobs", type=int, default=None)
        p.add_argument("--max-per", type=int, default=0)
        p.add_argument("-v", action="store_true")
    a = ap.parse_args()
    if a.cmd == "analyze":
        analyze(a.v)
        return 0
    if a.cmd == "overrides":
        overrides()
        return 0
    if a.cmd == "show":
        show(int(a.func.replace("func_", ""), 16))
        return 0
    if a.cmd == "check":
        return check(a.max_per)
    funcs = [int(f.replace("func_", ""), 16) for f in a.funcs] or None
    return run_dynamic(a.cmd, funcs, all_at_once=a.all, jobs=a.jobs, max_per=a.max_per,
                       verbose=a.v)


if __name__ == "__main__":
    sys.exit(main())
