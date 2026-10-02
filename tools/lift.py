#!/usr/bin/env python3
"""Lift a FALL.EXE game function (Watcom 10 -od code) to C that should compile back to the
same bytes.

Watcom's -od output is a near-literal translation of the source: every statement loads its
operands from memory, computes in registers and ends with a store, a call or a branch, and no
register value survives into the next statement. So the lifter executes each instruction
symbolically (register -> C expression) and emits a statement whenever something is stored,
called with its result unused, or branched on. Control flow is emitted as `if (...) goto` and
`goto`, which -od compiles back to the same cmp/jcc/jmp; the peephole optimiser makes the same
choices Watcom 10 did. Structuring into if/while is a later, separately verified pass.

Conventions in the generated C:
  - globals are `char D_XXXXXXXX[]` and accessed through casts (`*(int *)D_X`), so no type
    inference is needed; a reference to D+4 is its own symbol D_(X+4)
  - every function is declared `int func_X();` (no prototype: arguments are passed in
    eax, edx, ebx, ecx exactly as the call site loads them)
  - parameters are a1..a4, locals are named by frame offset (l_1C for [ebp-0x1C])

Anything the lifter does not handle raises Unsupported(reason); the batch driver counts the
reasons so the most common gap is fixed first.

usage: lift.py func_XXXXXXXX     (prints the C)
"""
import csv
import os
import re
import struct
import sys

import capstone
from capstone import x86 as cx

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE, SRC_OFF32, SRC_REL32  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")
PARM_REGS = ["eax", "edx", "ebx", "ecx"]
SAVE_ORDER = ["ebx", "ecx", "edx", "esi", "edi"]
SIZE_TYPE = {1: "char", 2: "short", 4: "int"}
UTYPE = {1: "unsigned char", 2: "unsigned short", 4: "unsigned"}
JCC = {  # mnemonic -> (operator, unsigned compare)
    "je": ("==", False), "jne": ("!=", False),
    "jl": ("<", False), "jle": ("<=", False), "jg": (">", False), "jge": (">=", False),
    "jb": ("<", True), "jbe": ("<=", True), "ja": (">", True), "jae": (">=", True),
}
REG8 = {"al": "eax", "dl": "edx", "bl": "ebx", "cl": "ecx"}
REG16 = {"ax": "eax", "dx": "edx", "bx": "ebx", "cx": "ecx", "si": "esi", "di": "edi"}


class Unsupported(Exception):
    pass


class E:
    """A C expression. `size` is how many low bytes of the register are meaningful (1, 2, 4)."""

    def __init__(self, text, size=4, atom=False):
        self.text, self.size, self.atom = text, size, atom

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


def fixup_at(ins, offset):
    """The off32 fixup on the 4-byte field at `offset` in the instruction, if any."""
    if not offset:
        return None
    f = IMG.fix_at.get(ins.address + offset)
    return f if f is not None and f.kind == SRC_OFF32 else None


def sym(va):
    return ("func_%08X" if va in IMG.funcs else "D_%08X") % va


class Func:
    def __init__(self, va):
        self.va = va
        self.ins = IMG.insns(va)
        self.out = []            # C statement lines
        self.globals = set()     # D_ symbols used
        self.calls = set()       # func_ symbols used
        self.labels = set()
        self.regs = {}
        self.pending = None      # (reg, call expression) not yet emitted
        self.pushes = []
        self.flags = None        # ("cmp", a, b) / ("test", a, b)
        self.ret_slot = None
        self.void = True
        self.slot_size = {}

    # ---- frame -------------------------------------------------------------------------
    def prologue(self):
        ins = self.ins
        if len(ins) < 3 or ins[0].mnemonic != "push" or ins[0].op_str != "ebp" or \
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
        # register parameters are spilled first, in eax, edx, ebx, ecx order
        self.params = []
        nparm_max = len([r for r in PARM_REGS if r not in saved])
        for reg in PARM_REGS:
            if len(self.params) == nparm_max or i >= len(ins):
                break
            m = re.fullmatch(r"(?:dword|word|byte) ptr \[ebp - (0x[0-9a-f]+)\], (\w+)", ins[i].op_str)
            if ins[i].mnemonic == "mov" and m and m.group(2) in (reg, reg[1:], reg[1] + "l"):
                self.params.append((reg, int(m.group(1), 16), {"d": 4, "w": 2, "b": 1}[ins[i].op_str[0]]))
                i += 1
            else:
                break
        if len(self.params) != nparm_max and nparm_max < 4 and saved != SAVE_ORDER[:len(saved)]:
            pass
        self.body_start = i
        self.top = 4 * (1 + len(saved))   # first slot below the saved registers is -(top+4)
        # epilogue: lea esp, [ebp - top]; pops; pop ebp; ret
        if self.ins[-1].mnemonic != "ret" or self.ins[-1].op_str:
            raise Unsupported("epilogue: not a plain ret")
        j = len(ins) - 2
        while ins[j].mnemonic == "pop":
            j -= 1
        if ins[j].mnemonic != "lea" or not ins[j].op_str.startswith("esp, [ebp"):
            raise Unsupported("epilogue: no lea esp")
        self.epi = ins[j].address
        self.body_end = j
        if j > 0 and ins[j - 1].mnemonic == "mov" and \
                re.fullmatch(r"eax, dword ptr \[ebp - 0x[0-9a-f]+\]", ins[j - 1].op_str):
            self.ret_slot = int(ins[j - 1].op_str.split("- ")[1].rstrip("]"), 16)
            self.void = False
            self.ret_ins = ins[j - 1].address
            self.body_end = j - 1
        else:
            self.ret_ins = self.epi

    def var(self, off):
        for k, (reg, o, sz) in enumerate(self.params):
            if o == off:
                return "a%d" % (k + 1)
        if off == self.ret_slot:
            return None
        self.slot_size.setdefault(off, 4)
        return "l_%X" % off

    # ---- operands ----------------------------------------------------------------------
    def mem(self, op, ins, size):
        """C lvalue text for a memory operand."""
        m = op.mem
        disp = m.disp
        base = ins.reg_name(m.base) if m.base else None
        index = ins.reg_name(m.index) if m.index else None
        seg = ins.reg_name(m.segment) if m.segment else None
        if seg not in (None, "ds"):
            raise Unsupported("segment override")
        t = SIZE_TYPE[size]
        # fixup inside the instruction = absolute address of a global
        fx = fixup_at(ins, ins.disp_offset) if ins.disp_size == 4 else None
        if base == "ebp" and index is None:
            if disp < 0:
                name = self.var(-disp)
                if name is None:
                    return "@RET"
                self.slot_size[-disp] = max(size, self.slot_size.get(-disp, 0)) \
                    if not name.startswith("a") else 4
                if name.startswith("l_") and size != 4:
                    return "*(%s *)&%s" % (t, name)
                return name if size == 4 or name.startswith("l_") else "*(%s *)&%s" % (t, name)
            raise Unsupported("stack parameter [ebp+%d]" % disp)
        parts = []
        if fx is not None:
            g = sym(fx.target_va)
            self.globals.add(g)
            parts.append(g)
        elif disp and not base and not index:
            raise Unsupported("absolute address without fixup")
        if base:
            parts.append("(char *)" + self.reg(base, ins).p())
        if index:
            sc = m.scale
            iexp = self.reg(index, ins).p()
            parts.append(iexp if sc == 1 else "%s * %d" % (iexp, sc))
        if fx is None and disp:
            parts.append(str(disp) if disp > 0 else None)
            if disp < 0:
                parts[-1] = None
                parts.append("-%d" % -disp)
        parts = [p for p in parts if p]
        addr = parts[0] if fx is not None and len(parts) == 1 else " + ".join(parts).replace("+ -", "- ")
        return "*(%s *)(%s)" % (t, addr)

    def reg(self, name, ins):
        r = self.regs.get(name)
        if r is None:
            raise Unsupported("read of undefined register %s at %x" % (name, ins.address))
        if self.pending and self.pending[0] == name:
            self.pending = None
        return r

    def src(self, op, ins, size):
        if op.type == cx.X86_OP_REG:
            n = ins.reg_name(op.reg)
            if n in REG8:
                return self.reg(REG8[n], ins)
            if n in REG16:
                return self.reg(REG16[n], ins)
            return self.reg(n, ins)
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
        if self.pending:
            self.out.append("    %s;" % self.pending[1].text)
            self.pending = None

    def emit(self, line):
        self.flush_pending()
        self.out.append("    " + line)

    def set_reg(self, name, expr):
        if self.pending and self.pending[0] == name:
            self.flush_pending()
        self.regs[name] = expr

    def lift(self):
        self.prologue()
        targets = set()
        for ins in self.ins:
            if cx.X86_GRP_JUMP in ins.groups and ins.operands[0].type == cx.X86_OP_IMM:
                targets.add(ins.operands[0].imm)
        i = self.body_start
        body = self.ins[i:self.body_end]
        for ins in body:
            if ins.address in targets:
                self.flush_pending()
                self.labels.add(ins.address)
                self.out.append("L%X:;" % ins.address)
                self.regs = {}
            self.step(ins)
        self.flush_pending()
        if self.ret_ins in targets or self.epi in targets:
            pass

    def step(self, ins):
        m, ops = ins.mnemonic, ins.operands
        if m == "mov":
            d, s = ops
            if d.type == cx.X86_OP_REG:
                n = ins.reg_name(d.reg)
                v = self.src(s, ins, d.size)
                if n in REG8:
                    full = REG8[n]
                    cur = self.regs.get(full)
                    if cur is not None and cur.text == "0":
                        self.set_reg(full, E("(unsigned char)" + v.p(), 4))
                    else:
                        self.set_reg(full, E(v.text, 1, v.atom))
                elif n in REG16:
                    full = REG16[n]
                    cur = self.regs.get(full)
                    if cur is not None and cur.text == "0":
                        self.set_reg(full, E("(unsigned short)" + v.p(), 4))
                    else:
                        self.set_reg(full, E(v.text, 2, v.atom))
                else:
                    self.set_reg(n, v)
                return
            # store
            v = self.src(s, ins, d.size)
            lhs = self.mem(d, ins, d.size)
            if lhs == "@RET":
                self.void = False
                self.emit("return %s;" % v.text)
                self.after_return = True
                return
            self.emit("%s = %s;" % (lhs, v.text))
            return
        if m in ("movsx", "movzx"):
            d, s = ops
            n = ins.reg_name(d.reg)
            v = self.src(s, ins, s.size)
            t = (SIZE_TYPE if m == "movsx" else UTYPE)[s.size]
            if v.size == s.size or s.type == cx.X86_OP_MEM:
                self.set_reg(n, E("(%s)%s" % (t, v.p()), 4))
            else:
                self.set_reg(n, E("(%s)%s" % (t, v.p()), 4))
            return
        if m == "cwde":
            v = self.reg("eax", ins)
            self.set_reg("eax", E("(short)" + v.p(), 4))
            return
        if m == "xor" and ops[0].type == cx.X86_OP_REG and ops[1].type == cx.X86_OP_REG and \
                ops[0].reg == ops[1].reg:
            self.set_reg(ins.reg_name(ops[0].reg), E("0", 4, atom=True))
            return
        if m in ("add", "sub", "and", "or", "xor", "imul", "shl", "sar", "shr"):
            if m == "imul" and len(ops) != 2:
                raise Unsupported("imul with 3 operands")
            d, s = ops
            opch = {"add": "+", "sub": "-", "and": "&", "or": "|", "xor": "^", "imul": "*",
                    "shl": "<<", "sar": ">>", "shr": ">>"}[m]
            if d.type == cx.X86_OP_REG:
                n = ins.reg_name(d.reg)
                if n not in self.regs and n in REG8.values():
                    pass
                full = REG8.get(n) or REG16.get(n) or n
                a = self.reg(full, ins)
                b = self.src(s, ins, d.size)
                if m == "and" and s.type == cx.X86_OP_IMM and s.imm in (0xFF, 0xFFFF) and d.size == 4:
                    t = "unsigned char" if s.imm == 0xFF else "unsigned short"
                    self.set_reg(full, E("(%s)%s" % (t, a.p()), 4))
                    return
                if m == "shr":
                    a = E("(unsigned)" + a.p(), 4)
                self.set_reg(full, E("%s %s %s" % (a.p(), opch, b.p()), d.size))
                self.flags = ("val", self.regs[full])
                return
            # read-modify-write memory
            lhs = self.mem(d, ins, d.size)
            b = self.src(s, ins, d.size)
            if lhs == "@RET":
                raise Unsupported("rmw on the return slot")
            self.emit("%s %s= %s;" % (lhs, opch, b.text))
            return
        if m in ("inc", "dec", "neg", "not"):
            d = ops[0]
            if d.type == cx.X86_OP_REG:
                n = ins.reg_name(d.reg)
                a = self.reg(n, ins)
                t = {"inc": "%s + 1", "dec": "%s - 1", "neg": "-%s", "not": "~%s"}[m] % a.p()
                self.set_reg(n, E(t, 4))
                return
            lhs = self.mem(d, ins, d.size)
            self.emit({"inc": "%s++;", "dec": "%s--;", "neg": "%s = -%s;", "not": "%s = ~%s;"}[m]
                      % ((lhs,) if m in ("inc", "dec") else (lhs, lhs)))
            return
        if m in ("cmp", "test"):
            a = self.src(ops[0], ins, ops[0].size)
            b = self.src(ops[1], ins, ops[1].size)
            self.flags = (m, a, b)
            return
        if m == "lea":
            d, s = ops
            n = ins.reg_name(d.reg)
            if s.mem.base and ins.reg_name(s.mem.base) == "ebp":
                off = -s.mem.disp
                name = self.var(off)
                if name is None:
                    raise Unsupported("address of return slot")
                self.set_reg(n, E("(int)&" + name, 4))
                return
            addr = self.mem(s, ins, 1)            # *(char *)(X)
            self.set_reg(n, E("(int)&" + addr, 4))
            return
        if m == "push":
            self.pushes.append(self.src(ops[0], ins, 4))
            return
        if m == "call":
            op = ops[0]
            if op.type != cx.X86_OP_IMM:
                raise Unsupported("indirect call")
            tgt = op.imm
            name = sym(tgt)
            self.calls.add(name)
            args = []
            for r in PARM_REGS:
                if r in self.regs:
                    args.append(self.regs[r].text)
                else:
                    break
            if len(args) < len([r for r in PARM_REGS if r in self.regs]):
                raise Unsupported("call arguments not in eax, edx, ebx, ecx order")
            stack = list(reversed(self.pushes))
            if stack and len(args) < 4:
                raise Unsupported("stack arguments with free registers (cdecl/varargs callee)")
            args += [s.text for s in stack]
            self.pushes = []
            call = E("%s(%s)" % (name, ", ".join(args)), 4, atom=True)
            self.flush_pending()
            self.regs = {"eax": call}
            self.pending = ("eax", call)
            return
        if m.startswith("j"):
            tgt = ops[0].imm if ops[0].type == cx.X86_OP_IMM else None
            if tgt is None:
                raise Unsupported("indirect jump (switch)")
            if m == "jmp":
                if tgt in (self.ret_ins, self.epi):
                    if getattr(self, "after_return", False):
                        self.after_return = False
                        self.regs = {}
                        return
                    self.emit("return;" if self.void else "goto L%X;" % tgt)
                    if not self.void:
                        self.labels.add(tgt)
                else:
                    self.emit("goto L%X;" % tgt)
                    self.labels.add(tgt)
                self.regs = {}
                return
            if m not in JCC or self.flags is None:
                raise Unsupported("conditional jump %s" % m)
            opr, uns = JCC[m]
            kind = self.flags[0]
            if kind == "cmp":
                a, b = self.flags[1], self.flags[2]
                if uns:
                    a = E("(unsigned)" + a.p(), 4)
                cond = "%s %s %s" % (a.p(), opr, b.p())
            elif kind == "test":
                a, b = self.flags[1], self.flags[2]
                if opr not in ("==", "!="):
                    raise Unsupported("test with ordered jcc")
                if a.text == b.text:
                    cond = "%s %s 0" % (a.p(), opr)
                else:
                    cond = "(%s & %s) %s 0" % (a.p(), b.p(), opr)
            else:
                raise Unsupported("jcc on arithmetic flags")
            self.emit("if (%s) goto L%X;" % (cond, tgt))
            self.labels.add(tgt)
            self.regs = {}
            self.flags = None
            return
        raise Unsupported("instruction %s" % m)

    # ---- output ------------------------------------------------------------------------
    def c(self):
        self.lift()
        name = sym(self.va)
        ps = ["int a%d" % (k + 1) for k in range(len(self.params))]
        # locals: the slot rule gives slots top down: 2-byte ones, return, others last to
        # first, so declare them bottom-up (deepest first)
        locals_ = sorted((o for o in self.slot_size if not any(o == p[1] for p in self.params)),
                         reverse=True)
        lines = ["%s %s(%s)" % ("void" if self.void else "int", name, ", ".join(ps) or "void"),
                 "{"]
        for o in locals_:
            lines.append("    %s l_%X;" % ("int", o))
        if locals_:
            lines.append("")
        body = []
        used = set()
        for l in self.out:
            for t in re.findall(r"goto L([0-9A-F]+);", l):
                used.add(t)
        for l in self.out:
            mm = re.fullmatch(r"L([0-9A-F]+):;", l)
            if mm and mm.group(1) not in used:
                continue
            body.append(l)
        # a return label at the very end (int functions whose paths jump to the return)
        if not self.void and "%X" % self.ret_ins in used:
            body.append("L%X:;" % self.ret_ins)
        lines += body + ["}"]
        decl = ["/* lifted from 0x%08X */" % self.va]
        for g in sorted(self.globals):
            decl.append("extern char %s[];" % g)
        for f in sorted(self.calls - {name}):
            decl.append("extern int %s();" % f)
        return "\n".join(decl + [""] + lines) + "\n"


IMG = None


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
