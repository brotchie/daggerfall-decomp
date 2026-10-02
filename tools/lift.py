#!/usr/bin/env python3
"""Lift a FALL.EXE game function (Watcom 10 -od code) to C that should compile back to the
same bytes.

Watcom's -od output is a near-literal translation of the source: every statement loads its
operands from memory, computes in registers and ends with a store, a call or a branch, and no
register value survives into the next statement. So the lifter executes each instruction
symbolically (register -> C expression) and emits a statement whenever something is stored,
a call's result goes unused, or a branch is taken. Control flow comes out as `if (...) goto`
and `goto`, which -od compiles back to the same cmp/jcc/jmp; the peephole optimiser makes the
same choices Watcom 10 did. Structuring into if/while is a later, separately verified pass.

Conventions in the generated C:
  - globals are `char D_XXXXXXXX[]` and accessed through casts (`*(int *)D_X`), so no type
    inference is needed; a reference to D+4 is its own symbol D_(X+4)
  - callees are unprototyped `int func_X();` (arguments are passed in eax, edx, ebx, ecx as the
    call site loads them), except callees taking stack arguments, which are `int f(int, ...);`
  - parameters are a1..a4; locals are named by frame offset (l_1C for [ebp-0x1C]); each stack
    slot's type comes from how the function accesses it (a pre-pass)

Anything the lifter does not handle raises Unsupported(reason); the batch driver counts the
reasons so the most common gap is fixed first.

usage: lift.py func_XXXXXXXX ...     (prints the C)
"""
import csv
import os
import re
import sys

import capstone
from capstone import x86 as cx

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE, SRC_OFF32  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")
PARM_REGS = ["eax", "edx", "ebx", "ecx"]
SAVE_ORDER = ["ebx", "ecx", "edx", "esi", "edi"]
STYPE = {1: "signed char", 2: "short", 4: "int"}
UTYPE = {1: "unsigned char", 2: "unsigned short", 4: "unsigned"}
JCC = {  # mnemonic -> (operator, unsigned compare)
    "je": ("==", False), "jne": ("!=", False),
    "jl": ("<", False), "jle": ("<=", False), "jg": (">", False), "jge": (">=", False),
    "jb": ("<", True), "jbe": ("<=", True), "ja": (">", True), "jae": (">=", True),
}
NEG = {"==": "!=", "!=": "==", "<": ">=", ">=": "<", ">": "<=", "<=": ">"}
SUB = {}
for full, b, w in (("eax", "al", "ax"), ("edx", "dl", "dx"), ("ebx", "bl", "bx"),
                   ("ecx", "cl", "cx"), ("esi", None, "si"), ("edi", None, "di")):
    SUB[full] = (full, 4)
    SUB[w] = (full, 2)
    if b:
        SUB[b] = (full, 1)


class Unsupported(Exception):
    pass


def subreg(name):
    """(full register, size) for a register name; high byte registers are not handled."""
    if name not in SUB:
        raise Unsupported("register %s" % name)
    return SUB[name]


class E:
    """A C expression. `size` = how many low bytes are meaningful; `tag` marks idioms in
    progress (('sign', X) for X >> 31, ('half', X) for X - (X >> 31))."""

    def __init__(self, text, size=4, atom=False, tag=None):
        self.text, self.size, self.atom, self.tag = text, size, atom, tag

    def p(self):
        return self.text if self.atom else "(%s)" % self.text


class Image:
    """The loaded executable plus the indexes the lifter needs (shared across functions)."""

    def __init__(self):
        self.le = LE(EXE)
        self.img = self.le.load(relocate=True)
        self.fix_at = {f.src_va: f for f in self.le.fixups()}
        with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
            self.funcs = {int(r["va"], 16): int(r["size"]) for r in csv.DictReader(f)}
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.md.detail = True

    def code(self, va, n):
        o = self.le.obj_of_va(va)
        return bytes(self.img[o.index][va - o.base: va - o.base + n])

    def insns(self, va):
        return list(self.md.disasm(self.code(va, self.funcs[va]), va))


IMG = None


def fixup_at(ins, offset):
    """The off32 fixup on the 4-byte field at `offset` in the instruction, if any."""
    if not offset:
        return None
    f = IMG.fix_at.get(ins.address + offset)
    return f if f is not None and f.kind == SRC_OFF32 else None


def sym(va):
    return ("func_%08X" if va in IMG.funcs else "D_%08X") % va


def ebp_slot(ins, op):
    """Frame offset (positive) of an [ebp - x] operand, else None."""
    if op.type != cx.X86_OP_MEM or not op.mem.base or ins.reg_name(op.mem.base) != "ebp" \
            or op.mem.index or op.mem.disp >= 0:
        return None
    return -op.mem.disp


class Func:
    def __init__(self, va):
        self.va = va
        self.ins = IMG.insns(va)
        self.out = []            # C lines
        self.globals = set()     # D_ symbols used
        self.calls = set()       # func_ symbols called with registers
        self.vcalls = set()      # func_ symbols called with stack arguments (caller pops)
        self.scalls = {}         # func_ symbol -> arg count, stack arguments (callee pops)
        self.regs = {}
        self.pending = None      # call expression in eax not yet emitted
        self.pushes = []
        self.flags = None
        self.ret_slot = None
        self.void = True
        self.after_return = False

    # ---- frame -------------------------------------------------------------------------
    def prologue(self):
        ins = self.ins
        if len(ins) < 4 or ins[0].mnemonic != "push" or ins[0].op_str != "ebp" or \
                ins[1].mnemonic != "mov" or ins[1].op_str != "ebp, esp":
            raise Unsupported("prologue: no ebp frame")
        i, saved = 2, []
        while ins[i].mnemonic == "push" and ins[i].op_str in SAVE_ORDER:
            saved.append(ins[i].op_str)
            i += 1
        if ins[i].mnemonic != "sub" or not ins[i].op_str.startswith("esp, "):
            raise Unsupported("prologue: no sub esp")
        self.frame = int(ins[i].op_str.split(", ")[1], 16)
        self.saved = saved
        i += 1
        self.params = []         # (reg, slot)
        for reg in PARM_REGS:
            if reg in saved or i >= len(ins):
                break
            d = ins[i].operands[0] if ins[i].operands else None
            s = ins[i].operands[1] if len(ins[i].operands) > 1 else None
            if ins[i].mnemonic == "mov" and d is not None and ebp_slot(ins[i], d) and \
                    s.type == cx.X86_OP_REG and SUB.get(ins[i].reg_name(s.reg), (None,))[0] == reg:
                self.params.append((reg, ebp_slot(ins[i], d), d.size))
                i += 1
            else:
                break
        # eax is never saved (it returns the result), so a free eax alone proves nothing; any
        # other free register carries a parameter
        free = [r for r in PARM_REGS if r not in saved]
        if len(free) >= 2 and len(self.params) != len(free):
            raise Unsupported("prologue: %d free registers but %d spills" % (len(free), len(self.params)))
        self.body_start = i
        if ins[-1].mnemonic != "ret" or ins[-1].op_str:
            raise Unsupported("epilogue: not a plain ret")
        j = len(ins) - 2
        while ins[j].mnemonic == "pop":
            j -= 1
        if ins[j].mnemonic != "lea" or not ins[j].op_str.startswith("esp, [ebp"):
            raise Unsupported("epilogue: no lea esp")
        self.epi = ins[j].address
        self.body_end = j
        prev = ins[j - 1]
        if prev.mnemonic == "mov" and prev.op_str.startswith("eax, dword ptr [ebp - "):
            self.ret_slot = ebp_slot(prev, prev.operands[1])
            self.void = False
            self.ret_ins = prev.address
            self.body_end = j - 1
        else:
            self.ret_ins = self.epi

    def type_slots(self):
        """Pre-pass: give every stack slot a C type from its accesses."""
        acc = {}       # slot -> set of sizes
        sign = {}      # slot -> 's' / 'u' hints
        addr = set()
        body = self.ins[self.body_start:self.body_end]
        for k, ins in enumerate(body):
            for n, op in enumerate(ins.operands):
                off = ebp_slot(ins, op)
                if off is None:
                    continue
                if ins.mnemonic == "lea":
                    addr.add(off)
                    continue
                acc.setdefault(off, set()).add(op.size)
                if ins.mnemonic == "movsx":
                    sign.setdefault(off, set()).add("s")
                elif ins.mnemonic == "movzx":
                    sign.setdefault(off, set()).add("u")
                elif ins.mnemonic == "mov" and n == 1 and op.size < 4:
                    nxt = body[k + 1] if k + 1 < len(body) else None
                    prv = body[k - 1] if k else None
                    if nxt is not None and (nxt.mnemonic == "cwde" or
                                            (nxt.mnemonic == "movsx" and op.size == 1)):
                        sign.setdefault(off, set()).add("s")
                    elif (nxt is not None and nxt.mnemonic == "and" and
                          nxt.op_str in ("eax, 0xff", "eax, 0xffff")) or \
                            (prv is not None and prv.mnemonic == "xor"):
                        sign.setdefault(off, set()).add("u")
        self.slot_type = {}
        for reg, off, _sz in self.params:
            acc.setdefault(off, set())
        for off, sizes in acc.items():
            if off == self.ret_slot:
                continue
            body_sizes = sizes - {4} if off in [p[1] for p in self.params] else sizes
            sz = min(body_sizes) if body_sizes and len(body_sizes) == 1 else 4
            if off in addr:
                sz = 4
            hint = sign.get(off, set())
            if sz not in (1, 2, 4):
                raise Unsupported("%d-byte stack slot (floating point)" % sz)
            t = (UTYPE if hint == {"u"} or (sz == 1 and hint != {"s"}) else STYPE)[sz]
            self.slot_type[off] = t
        self.size_of = {"signed char": 1, "unsigned char": 1, "short": 2,
                        "unsigned short": 2, "int": 4, "unsigned": 4}
        # Every 4-byte slot between the saved registers and the frame bottom belongs to a
        # declared variable (-od gives slots at declaration, used or not). An address-taken
        # slot is the base of an array reaching up to the next known variable.
        top = 4 * len(self.saved) + 4
        bottom = 4 * len(self.saved) + self.frame
        known = set(self.slot_type) | {o for _r, o, _s in self.params} | addr
        if self.ret_slot:
            known.add(self.ret_slot)
        self.arrays = {}         # base offset -> size in bytes
        for a in sorted(addr):
            above = [o for o in known if o < a and o not in range(a - 3, a)]
            nxt = max([o for o in above if o not in self.slot_type or o in addr
                       or o in [p[1] for p in self.params] or o == self.ret_slot]
                      + [top - 4])
            size = a - nxt
            if size > 4:
                self.arrays[a] = size
        inside = {}
        for a, size in self.arrays.items():
            for o in range(a - size + 1, a):
                inside[o] = a
        for o in list(self.slot_type):
            if o in inside:
                del self.slot_type[o]
        self.inside = inside
        for a in addr:
            if a not in self.arrays:
                self.slot_type.setdefault(a, "int")
        for o in range(top, bottom + 1, 4):
            if o in known or o in inside or o in self.arrays:
                continue
            if any(o - 3 <= k <= o for k in known):
                continue
            self.slot_type[o] = "int"         # declared but never used

    def var(self, off):
        for k, (reg, o, sz) in enumerate(self.params):
            if o == off:
                return "a%d" % (k + 1)
        if off == self.ret_slot:
            return None
        return "l_%X" % off

    # ---- operands ----------------------------------------------------------------------
    def mem(self, op, ins, size):
        """C lvalue text for a memory operand ('@RET' for the return slot)."""
        m = op.mem
        disp = m.disp
        base = ins.reg_name(m.base) if m.base else None
        index = ins.reg_name(m.index) if m.index else None
        if m.segment and ins.reg_name(m.segment) not in ("ds", "cs"):
            raise Unsupported("segment override")
        off = ebp_slot(ins, op)
        if off is not None and off in getattr(self, "inside", {}):
            base = self.inside[off]
            return "*(%s *)((char *)l_%X + %d)" % (STYPE[size], base, base - off)
        if off is not None and off in getattr(self, "arrays", {}):
            return "*(%s *)l_%X" % (STYPE[size], off)
        if off is not None:
            name = self.var(off)
            if name is None:
                return "@RET"
            t = self.slot_type.get(off, "int")
            if self.size_of[t] == size:
                return name
            return "*(%s *)&%s" % (STYPE[size], name)
        if base == "ebp":
            raise Unsupported("stack parameter [ebp+%d]" % disp)
        fx = fixup_at(ins, ins.disp_offset) if ins.disp_size == 4 else None
        parts = []
        if fx is not None:
            g = sym(fx.target_va)
            self.globals.add(g)
            parts.append(g)
            if base:
                parts.append(self.reg(base, ins).p())
        elif base:
            parts.append("(char *)" + self.reg(base, ins).p())
        elif not index:
            raise Unsupported("absolute address without fixup")
        if index:
            iexp = self.reg(index, ins).p()
            parts.append(iexp if m.scale == 1 else "%s * %d" % (iexp, m.scale))
            if not fx and not base:
                parts[-1] = "(char *)0 + " + parts[-1]
        if fx is None and disp:
            parts.append(("+ %d" if disp > 0 else "- %d") % abs(disp))
        addr = parts[0]
        for p in parts[1:]:
            addr += (" " + p) if p.startswith(("+ ", "- ")) else " + " + p
        if len(parts) == 1 and fx is not None:
            return "*(%s *)%s" % (STYPE[size], addr)
        return "*(%s *)(%s)" % (STYPE[size], addr)

    def reg(self, name, ins):
        r = self.regs.get(name)
        if r is None:
            raise Unsupported("read of undefined register %s" % name)
        if self.pending is not None and r is self.pending:
            self.pending = None
        return r

    def src(self, op, ins):
        if op.type == cx.X86_OP_REG:
            full, _sz = subreg(ins.reg_name(op.reg))
            return self.reg(full, ins)
        if op.type == cx.X86_OP_IMM:
            fx = fixup_at(ins, ins.imm_offset) if ins.imm_size == 4 else None
            if fx is not None:
                s = sym(fx.target_va)
                (self.calls if s.startswith("func_") else self.globals).add(s)
                return E("(int)" + s, 4)
            v = op.imm & 0xFFFFFFFF
            if v >= 0x80000000:
                v -= 1 << 32
            return E(str(v), 4, atom=v >= 0)
        if op.type == cx.X86_OP_MEM:
            t = self.mem(op, ins, op.size)
            if t == "@RET":
                raise Unsupported("read of the return slot")
            return E(t, op.size, atom=True)
        raise Unsupported("operand type")

    # ---- statements --------------------------------------------------------------------
    def flush_pending(self):
        if self.pending is not None:
            self.out.append("    %s;" % self.pending.text)
            self.pending = None

    def emit(self, line):
        self.flush_pending()
        self.out.append("    " + line)

    def set_reg(self, name, expr):
        cur = self.regs.get(name)
        if self.pending is not None and cur is self.pending and expr is not cur:
            self.flush_pending()
        self.regs[name] = expr

    def lift(self):
        self.prologue()
        self.type_slots()
        targets = set()
        for ins in self.ins:
            if cx.X86_GRP_JUMP in ins.groups and ins.operands and \
                    ins.operands[0].type == cx.X86_OP_IMM:
                targets.add(ins.operands[0].imm)
        self.targets = targets
        body = self.ins[self.body_start:self.body_end]
        self.body = body
        for k, ins in enumerate(body):
            if ins.address in targets:
                self.flush_pending()
                self.out.append("L%X:;" % ins.address)
                self.regs = {}
                self.after_return = False
            self.k = k
            self.step(ins)
        self.flush_pending()

    def cond_text(self, m):
        opr, uns = JCC[m]
        kind = self.flags[0]
        a, b = self.flags[1], self.flags[2]
        if kind == "cmp":
            if uns:
                a = E("(unsigned)" + a.p(), 4)
            return "%s %s %s" % (a.p(), opr, b.p())
        if kind == "test":
            if opr not in ("==", "!="):
                raise Unsupported("test with ordered jcc")
            if a.text == b.text:
                return "%s %s 0" % (a.p(), opr)
            return "(%s & %s) %s 0" % (a.p(), b.p(), opr)
        raise Unsupported("jcc on arithmetic flags")

    def step(self, ins):
        m, ops = ins.mnemonic, ins.operands
        if m == "nop":
            return
        if m == "add" and getattr(self, "skip_add", False):
            self.skip_add = False   # caller's cleanup after a cdecl call
            return
        if m == "mov":
            d, s = ops
            if d.type == cx.X86_OP_REG:
                full, sz = subreg(ins.reg_name(d.reg))
                v = self.src(s, ins)
                cur = self.regs.get(full)
                if sz < 4 and cur is not None and cur.text == "0":
                    self.set_reg(full, E("(%s)%s" % (UTYPE[sz], v.p()), 4))
                elif sz < 4:
                    self.set_reg(full, E(v.text, sz, v.atom))
                else:
                    self.set_reg(full, v)
                return
            v = self.src(s, ins)
            lhs = self.mem(d, ins, d.size)
            if lhs == "@RET":
                self.void = False
                self.emit("return %s;" % v.text)
                self.after_return = True
                return
            self.emit("%s = %s;" % (lhs, v.text))
            self.regs = {}          # -od: nothing survives into the next statement
            return
        if m in ("movsx", "movzx"):
            d, s = ops
            full, _ = subreg(ins.reg_name(d.reg))
            v = self.src(s, ins)
            t = (STYPE if m == "movsx" else UTYPE)[s.size]
            self.set_reg(full, E("(%s)%s" % (t, v.p()), 4))
            return
        if m == "cwde":
            v = self.reg("eax", ins)
            self.set_reg("eax", E("(short)" + v.p(), 4))
            return
        if m == "cdq":
            v = self.reg("eax", ins)
            self.set_reg("edx", E("%s >> 31" % v.p(), 4, tag=("sign", v)))
            return
        if m == "xor" and ins.op_str == "ah, ah":
            v = self.reg("eax", ins)
            self.set_reg("eax", E("(unsigned char)" + v.p(), 2))
            return
        if m == "test" and ops[0].type == cx.X86_OP_REG and ins.reg_name(ops[0].reg) == "ah" \
                and ops[1].type == cx.X86_OP_IMM:
            v = self.reg("eax", ins)
            self.flags = ("test", v, E(str(ops[1].imm << 8), 4, atom=True))
            return
        if m in ("xor", "sub") and ops[0].type == cx.X86_OP_REG and \
                ops[1].type == cx.X86_OP_REG and ops[0].reg == ops[1].reg:
            self.set_reg(subreg(ins.reg_name(ops[0].reg))[0], E("0", 4, atom=True))
            return
        if m == "imul" and len(ops) == 3:
            full, _ = subreg(ins.reg_name(ops[0].reg))
            a = self.src(ops[1], ins)
            b = self.src(ops[2], ins)
            self.set_reg(full, E("%s * %s" % (a.p(), b.p()), 4))
            return
        if m in ("idiv", "div"):
            a = self.reg("eax", ins)
            d = self.regs.get("edx")
            b = self.src(ops[0], ins)
            if m == "idiv" and not (d is not None and d.tag and d.tag[0] == "sign"):
                raise Unsupported("idiv without sign extension")
            if m == "div" and not (d is not None and d.text == "0"):
                raise Unsupported("div without zeroed edx")
            if m == "div":
                a = E("(unsigned)" + a.p(), 4)
            self.set_reg("eax", E("%s / %s" % (a.p(), b.p()), 4))
            self.set_reg("edx", E("%s %% %s" % (a.p(), b.p()), 4))
            return
        if m == "sbb" and ops[0].type == cx.X86_OP_REG and ops[1].type == cx.X86_OP_REG:
            a = self.reg(subreg(ins.reg_name(ops[0].reg))[0], ins)
            b = self.reg(subreg(ins.reg_name(ops[1].reg))[0], ins)
            if b.tag and b.tag[0] == "signshl" and b.tag[1].text == a.text:
                self.set_reg(subreg(ins.reg_name(ops[0].reg))[0],
                             E("%s - %s" % (a.p(), b.p()), 4, tag=("sbbprep", a, b.tag[2])))
                return
            raise Unsupported("sbb outside the division idiom")
        if m in ("add", "sub", "and", "or", "xor", "imul", "shl", "sar", "shr"):
            d, s = ops
            opch = {"add": "+", "sub": "-", "and": "&", "or": "|", "xor": "^", "imul": "*",
                    "shl": "<<", "sar": ">>", "shr": ">>"}[m]
            if d.type == cx.X86_OP_REG:
                full, sz = subreg(ins.reg_name(d.reg))
                a = self.reg(full, ins)
                b = self.src(s, ins)
                if m == "and" and s.type == cx.X86_OP_IMM and sz == 4 and s.imm in (0xFF, 0xFFFF):
                    t = "unsigned char" if s.imm == 0xFF else "unsigned short"
                    self.set_reg(full, E("(%s)%s" % (t, a.p()), 4))
                    return
                # signed division by 2: X - (X >> 31), then >> 1
                if m == "sar" and s.type == cx.X86_OP_IMM and s.imm == 31:
                    self.set_reg(full, E("%s >> 31" % a.p(), 4, tag=("sign", a)))
                    return
                if m == "sub" and b.tag and b.tag[0] == "sign" and b.tag[1].text == a.text:
                    self.set_reg(full, E("%s - %s" % (a.p(), b.p()), 4, tag=("half", a)))
                    return
                if m == "sar" and a.tag and a.tag[0] == "half" and s.type == cx.X86_OP_IMM \
                        and s.imm == 1:
                    self.set_reg(full, E("%s / 2" % a.tag[1].p(), 4))
                    return
                # signed division by 2^k: sar edx,31; shl edx,k; sbb eax,edx; sar eax,k
                if m == "shl" and a.tag and a.tag[0] == "sign" and s.type == cx.X86_OP_IMM:
                    self.set_reg(full, E("%s << %d" % (a.p(), s.imm), 4,
                                         tag=("signshl", a.tag[1], s.imm)))
                    return
                if m == "sar" and a.tag and a.tag[0] == "sbbprep" and s.type == cx.X86_OP_IMM \
                        and s.imm == a.tag[2]:
                    self.set_reg(full, E("%s / %d" % (a.tag[1].p(), 1 << s.imm), 4))
                    return
                if m == "shr":
                    a = E("(unsigned)" + a.p(), 4)
                v = E("%s %s %s" % (a.p(), opch, b.p()), max(sz, 1))
                self.set_reg(full, v)
                self.flags = ("val", v, None)
                return
            lhs = self.mem(d, ins, d.size)
            b = self.src(s, ins)
            if lhs == "@RET":
                raise Unsupported("read-modify-write of the return slot")
            self.emit("%s %s= %s;" % (lhs, opch, b.text))
            return
        if m in ("inc", "dec", "neg", "not"):
            d = ops[0]
            if d.type == cx.X86_OP_REG:
                full, _ = subreg(ins.reg_name(d.reg))
                a = self.reg(full, ins)
                t = {"inc": "%s + 1", "dec": "%s - 1", "neg": "-%s", "not": "~%s"}[m] % a.p()
                self.set_reg(full, E(t, 4))
                return
            lhs = self.mem(d, ins, d.size)
            if lhs == "@RET":
                raise Unsupported("read-modify-write of the return slot")
            lv = lhs if re.fullmatch(r"\w+", lhs) else "(%s)" % lhs
            self.emit({"inc": "%s++;", "dec": "%s--;", "neg": "%s = -%s;", "not": "%s = ~%s;"}[m]
                      % ((lv,) if m in ("inc", "dec") else (lhs, lv)))
            return
        if m in ("cmp", "test"):
            a = self.src(ops[0], ins)
            b = self.src(ops[1], ins)
            self.flags = (m, a, b)
            return
        if m == "lea":
            d, s = ops
            full, _ = subreg(ins.reg_name(d.reg))
            off = ebp_slot(ins, s)
            if off is not None and off in self.arrays:
                self.set_reg(full, E("(int)l_%X" % off, 4))
                return
            if off is not None and off in self.inside:
                base = self.inside[off]
                self.set_reg(full, E("(int)((char *)l_%X + %d)" % (base, base - off), 4))
                return
            if off is not None:
                name = self.var(off)
                if name is None:
                    raise Unsupported("address of the return slot")
                self.set_reg(full, E("(int)&" + name, 4))
                return
            mm = s.mem
            if mm.base and mm.index and mm.base == mm.index and not mm.disp and mm.scale in (2, 4, 8):
                a = self.reg(subreg(ins.reg_name(mm.base))[0], ins)
                self.set_reg(full, E("%s * %d" % (a.p(), mm.scale + 1), 4))
                return
            addr = self.mem(s, ins, 1)
            self.set_reg(full, E("(int)&" + addr, 4))
            return
        if m == "push":
            self.pushes.append(self.src(ops[0], ins))
            if ops[0].type == cx.X86_OP_REG:
                # a pushed register has been consumed as a stack argument
                self.regs.pop(subreg(ins.reg_name(ops[0].reg))[0], None)
            return
        if m == "call":
            op = ops[0]
            if op.type != cx.X86_OP_IMM:
                return self.indirect_call(ins, op)
            name = sym(op.imm)
            nxt = self.body[self.k + 1] if self.k + 1 < len(self.body) else None
            cleanup = nxt is not None and nxt.mnemonic == "add" and nxt.op_str.startswith("esp, ")
            pops = callee_pops(op.imm)
            loaded = [r for r in PARM_REGS if r in self.regs and self.regs[r] is not self.pending]
            if pops and self.pushes and not loaded:
                # stack convention, callee pops (`#pragma aux ... parm routine []`)
                if pops // 4 != len(self.pushes):
                    raise Unsupported("pushes do not match callee's ret %d" % pops)
                args = [s.text for s in reversed(self.pushes)]
                self.scalls[name] = pops // 4
            elif self.pushes and cleanup:
                # cdecl / varargs callee: every argument is on the stack, caller pops
                n = int(nxt.op_str.split(", ")[1], 16) // 4
                if n != len(self.pushes):
                    raise Unsupported("stack cleanup does not match pushes")
                args = [s.text for s in reversed(self.pushes)]
                self.vcalls.add(name)
                self.skip_add = True
            else:
                args = []
                sig = signature(op.imm) if op.imm in IMG.funcs and \
                    IMG.le.obj_of_va(op.imm).index == 1 else None
                if sig is not None:
                    # the callee's own prologue says how many register arguments it takes
                    for r in PARM_REGS[:len(sig[1])]:
                        if r not in self.regs or self.regs[r] is self.pending:
                            raise Unsupported("call argument %s not loaded" % r)
                        args.append(self.regs[r].text)
                else:
                    for r in PARM_REGS:
                        if r in self.regs and self.regs[r] is not self.pending:
                            args.append(self.regs[r].text)
                        else:
                            break
                    if len(args) < len([r for r in PARM_REGS if r in self.regs
                                        and self.regs[r] is not self.pending]):
                        raise Unsupported("call arguments not in eax, edx, ebx, ecx order")
                if self.pushes:
                    if len(args) < 4:
                        raise Unsupported("stack arguments without cleanup (callee pops)")
                    args += [s.text for s in reversed(self.pushes)]
                self.calls.add(name)
            self.pushes = []
            self.finish_call(E("%s(%s)" % (name, ", ".join(args)), 4, atom=True), len(args))
            return
        if m.startswith("j"):
            tgt = ops[0].imm if ops[0].type == cx.X86_OP_IMM else None
            if tgt is None:
                raise Unsupported("indirect jump (switch)")
            to_end = tgt in (self.ret_ins, self.epi)
            if m == "jmp":
                if to_end:
                    if self.after_return:
                        self.after_return = False
                    elif self.void:
                        self.emit("return;")
                    else:
                        self.emit("goto L%X;" % tgt)
                else:
                    self.emit("goto L%X;" % tgt)
                self.regs = {}
                return
            if m not in JCC or self.flags is None:
                raise Unsupported("conditional jump %s" % m)
            cond = self.cond_text(m)
            if to_end and self.void:
                self.emit("if (%s) return;" % cond)
            else:
                self.emit("if (%s) goto L%X;" % (cond, tgt))
            self.regs = {}
            self.flags = None
            return
        raise Unsupported("instruction %s" % m)

    def finish_call(self, call, nargs):
        """After a call: eax holds the result; callee-saved registers that were not
        arguments keep their values (Watcom callees preserve everything but eax)."""
        self.flush_pending()
        keep = {r: v for r, v in self.regs.items()
                if r != "eax" and r not in PARM_REGS[:nargs] and v is not self.pending}
        keep["eax"] = call
        self.regs = keep
        self.pending = call

    def indirect_call(self, ins, op):
        """call [mem] / call reg: through a function pointer, arguments in registers."""
        if op.type == cx.X86_OP_MEM:
            target = self.mem(op, ins, 4)
            if target == "@RET":
                raise Unsupported("call through the return slot")
        else:
            target = self.reg(subreg(ins.reg_name(op.reg))[0], ins).p()
        if self.pushes:
            raise Unsupported("indirect call with stack arguments")
        args = []
        for r in PARM_REGS:
            if r in self.regs and self.regs[r] is not self.pending:
                args.append(self.regs[r].text)
            else:
                break
        call = E("((int (*)())%s)(%s)" % (target if target.startswith("(") else
                                           "(%s)" % target, ", ".join(args)), 4, atom=True)
        self.finish_call(call, len(args))

    # ---- output ------------------------------------------------------------------------
    def c(self):
        self.lift()
        name = sym(self.va)
        ps = []
        for k, (reg, off, _sz) in enumerate(self.params):
            ps.append("%s a%d" % (self.slot_type.get(off, "int"), k + 1))
        # The slot rule (docs/progress.md) gives slots top down: 2-byte locals, the return
        # variable, other locals last to first; so declare locals deepest first.
        locals_ = sorted((o for o in set(self.slot_type) | set(self.arrays)
                          if o not in [p[1] for p in self.params]), reverse=True)
        two = [o for o in locals_ if o in self.slot_type and self.size_of[self.slot_type[o]] == 2]
        rest = [o for o in locals_ if o not in two]
        lines = ["%s %s(%s)" % ("void" if self.void else "int", name, ", ".join(ps) or "void"),
                 "{"]
        for o in rest + two:
            reg = ""
            if o in self.arrays:
                lines.append("    char l_%X[%d];" % (o, self.arrays[o]))
            else:
                lines.append("    %s%s l_%X;" % (reg, self.slot_type[o], o))
        if locals_:
            lines.append("")
        used = set(re.findall(r"goto L([0-9A-F]+);", "\n".join(self.out)))
        for line in self.out:
            mm = re.fullmatch(r"L([0-9A-F]+):;", line)
            if mm and mm.group(1) not in used:
                continue
            lines.append(line)
        if not self.void and "%X" % self.ret_ins in used:
            lines.append("L%X:;" % self.ret_ins)
        lines.append("}")
        decl = ["/* lifted from 0x%08X */" % self.va]
        for g in sorted(self.globals):
            decl.append("extern char %s[];" % g)
        for f in sorted(self.calls - self.vcalls - set(self.scalls) - {name}):
            decl.append(globals()["decl"](f))
        for f, n in sorted(self.scalls.items()):
            decl.append("#pragma aux %s parm routine [];" % f)
            decl.append("extern int %s(%s);" % (f, ", ".join(["int"] * n)))
        for f in sorted(self.vcalls):
            decl.append("extern int %s(int, ...);" % f)
        return "\n".join(decl + [""] + lines) + "\n"


SIGS = {}
POPS = {}


def callee_pops(va):
    """Bytes a stack-convention callee pops (`ret N`, no register parameters), else 0."""
    if va not in POPS:
        n = 0
        if va in IMG.funcs:
            ins = IMG.insns(va)
            rets = [i for i in ins if i.mnemonic == "ret" and i.op_str]
            if rets and all(i.op_str == rets[0].op_str for i in rets):
                n = int(rets[0].op_str, 16)
                # register parameters would be spilled from eax/edx/ebx/ecx after the frame
                head = " ".join("%s %s" % (i.mnemonic, i.op_str) for i in ins[:12])
                if re.search(r"mov (dword|word|byte) ptr \[ebp - 0x[0-9a-f]+\], (eax|edx|ebx|ecx)", head):
                    n = 0
        POPS[va] = n
    return POPS[va]


def signature(va):
    """(return type, [param types]) of a game function from its prologue and slot types, or
    None if the lifter can't read its frame. Every caller declares a callee the same way, so
    lifted functions can share a file."""
    if va not in SIGS:
        try:
            f = Func(va)
            f.prologue()
            f.type_slots()
            ret = "void" if f.void else "int"
            # a return statement decides void-ness during lifting; the epilogue tells us now
            SIGS[va] = (ret, [f.slot_type.get(off, "int") for _r, off, _s in f.params])
        except Unsupported:
            SIGS[va] = None
    return SIGS[va]


def decl(name):
    va = int(name[5:], 16)
    sig = signature(va) if va in IMG.funcs else None
    if sig is None:
        return "extern int %s();" % name
    return "extern %s %s(%s);" % (sig[0], name, ", ".join(sig[1]) or "void")


def init():
    global IMG
    if IMG is None:
        IMG = Image()
    return IMG


def lift(va):
    init()
    return Func(va).c()


def main():
    init()
    for a in sys.argv[1:]:
        va = int(a.replace("func_", ""), 16)
        try:
            print(lift(va))
        except Unsupported as e:
            print("/* %s: unsupported: %s */" % (a, e))


if __name__ == "__main__":
    main()
