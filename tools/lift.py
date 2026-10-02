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
import json
import os
import re
import sys

import capstone
from capstone import x86 as cx

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE, SRC_OFF32  # noqa: E402
import slot_plan  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME_END = 0x9DA1C
EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")
PARM_REGS = ["eax", "edx", "ebx", "ecx"]
SAVE_ORDER = ["ebx", "ecx", "edx", "esi", "edi"]
STYPE = {1: "signed char", 2: "short", 4: "int", 8: "double"}
UTYPE = {1: "unsigned char", 2: "unsigned short", 4: "unsigned", 8: "double"}
CHP = 0xA167C               # __CHP: truncates ST0 toward zero (float to int conversions)
FP_OPS = {"fadd": "+", "fsub": "-", "fmul": "*", "fdiv": "/"}
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
if not os.environ.get("LIFT_NOHIGHBYTE"):
    # a high byte register as a byte value of its own (`mov ah,[x]; shl ah,7; add al,ah`)
    for full, h in (("eax", "ah"), ("edx", "dh"), ("ebx", "bh"), ("ecx", "ch")):
        SUB[h] = ("h" + full, 1)


class Unsupported(Exception):
    pass


def subreg(name):
    """(full register, size) for a register name; high byte registers are not handled."""
    if name not in SUB:
        raise Unsupported("register %s" % name)
    return SUB[name]


# an address or number: the compiler loads constants late, whatever their source position
CONST_RE = re.compile(r"\(int\)[DF][_a-z]*\w+|-?\d+")


class E:
    """A C expression. `size` = how many low bytes are meaningful; `tag` marks idioms in
    progress (('sign', X) for X >> 31, ('half', X) for X - (X >> 31))."""

    def __init__(self, text, size=4, atom=False, tag=None):
        self.text, self.size, self.atom, self.tag = text, size, atom, tag

    def p(self):
        return self.text if self.atom else "(%s)" % self.text


def kids(text):
    """Rough size of an expression's code-generator tree: operands, operators, casts."""
    t = re.sub(r"\((?:signed |unsigned )?(?:char|short|int)(?: \*)*\)", " C ", text)
    return len(re.findall(r"[A-Za-z_]\w*|\d+|<<|>>|[-+*/%&|^~!]", t))


class Image:
    """The loaded executable plus the indexes the lifter needs (shared across functions)."""

    def __init__(self):
        self.le = LE(EXE)
        self.img = self.le.load(relocate=True)
        self.fix_at = {f.src_va: f for f in self.le.fixups()}
        with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
            self.funcs = {int(r["va"], 16): int(r["size"]) for r in csv.DictReader(f)}
        with open(os.path.join(ROOT, "config", "code_data.csv"), newline="") as f:
            self.tables = [(int(r["va"], 16), int(r["size"])) for r in csv.DictReader(f)]
        self.md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
        self.md.detail = True

    def code(self, va, n):
        o = self.le.obj_of_va(va)
        return bytes(self.img[o.index][va - o.base: va - o.base + n])

    def insns(self, va):
        """Decode a function, stepping over switch tables inside it (config/code_data.csv)."""
        size = self.funcs[va]
        skip = sorted((t, n) for t, n in self.tables if va <= t < va + size)
        if not skip:
            return list(self.md.disasm(self.code(va, size), va))
        out, pc, end = [], va, va + size
        while pc < end:
            hit = [(t, n) for t, n in skip if t <= pc < t + n]
            if hit:
                pc = hit[0][0] + hit[0][1]
                continue
            nxt = min([t for t, _n in skip if t > pc] + [end])
            got = False
            for ins in self.md.disasm(self.code(pc, nxt - pc), pc):
                out.append(ins)
                pc = ins.address + ins.size
                got = True
            if not got or pc < nxt:
                pc = nxt if got else pc + 1
        return out


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


def local_indexed(ins, op):
    """Frame offset of a [reg + ebp - x] operand (a local array indexed by reg), else None."""
    if op.type != cx.X86_OP_MEM or not op.mem.base or not op.mem.index:
        return None
    names = (ins.reg_name(op.mem.base), ins.reg_name(op.mem.index))
    if "ebp" not in names or op.mem.disp >= 0:
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
        self.born_hint = None
        self.temps = set()       # slots that are the compiler's own temps (not declared)
        self.side = []           # stores inside an expression, waiting for the next read
        self.used_results = set()   # callees whose return value is used
        self.first_call = {}     # callee -> address of its first call
        self.noproto = set()     # callees called without a prototype in scope
        self.structs = set()     # bit-field struct declarations used
        self.skip_set = set()    # instructions already lifted with an earlier one
        self.fpst = []           # the x87 stack of double expressions
        self.fpcalls = {}        # functions returning doubles: name -> double arguments
        self.farcalls = set()    # functions given a far pointer: declared without a prototype
        self.fild_val = {}       # temp slot -> the int an fild converts
        self.slot_val = {}       # temp slot -> the value its one later read gets
        self.choices = []        # instruction addresses of operand-order choice points
        self.choice_sites = {}   # choice -> addresses where it shows (slot accesses)
        self.flips = frozenset()
        self.pending = None      # call expression in eax not yet emitted
        self.pushes = []
        self.flags = None
        self.ret_slot = None
        self.ret_type = "int"
        self.void = True
        self.after_return = False
        self.stale = set()       # registers readable but not implicit call arguments

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
        self.conv = None
        self.stack_base = 5      # stack parameters follow the four register ones
        if saved == ["esi", "edi"] and not self.params:
            # sosez.c/profile.c: arguments on the stack, caller cleanup, only esi/edi saved:
            # #pragma aux sosconv parm caller [] modify [eax ebx ecx edx]
            self.conv = "sosconv"
            self.stack_base = 1
            free = []
        if len(free) >= 2 and len(self.params) != len(free):
            raise Unsupported("prologue: %d free registers but %d spills" % (len(free), len(self.params)))
        self.body_start = i
        if ins[-1].mnemonic == "jmp" and not os.environ.get("LIFT_NONORET"):
            # a function that never returns (an endless loop): no epilogue at all
            self.nstack = 0
            self.epi = ins[-1].address + ins[-1].size
            self.body_end = len(ins)
            self.ret_ins = self.epi
            self.void = True
            return
        if ins[-1].mnemonic != "ret":
            raise Unsupported("epilogue: no ret")
        # `ret N`: N/4 more parameters on the stack ([ebp+8], [ebp+12], ...), which Watcom
        # only uses once all four argument registers are taken
        self.nstack = int(ins[-1].op_str, 16) // 4 if ins[-1].op_str else 0
        if self.conv == "sosconv":
            disps = [op.mem.disp for x in ins for op in x.operands
                     if op.type == cx.X86_OP_MEM and op.mem.base and
                     x.reg_name(op.mem.base) == "ebp" and op.mem.disp >= 8]
            self.nstack = (max(disps) - 8) // 4 + 1 if disps else 0
        elif self.nstack and len(self.params) != 4:
            raise Unsupported("stack parameters with free argument registers")
        j = len(ins) - 2
        while ins[j].mnemonic == "pop":
            j -= 1
        if ins[j].mnemonic != "lea" or not ins[j].op_str.startswith("esp, [ebp"):
            raise Unsupported("epilogue: no lea esp")
        self.epi = ins[j].address
        self.body_end = j
        prev = ins[j - 1]
        if prev.mnemonic == "mov" and prev.op_str.startswith(
                ("eax, dword ptr [ebp - ", "al, byte ptr [ebp - ", "ax, word ptr [ebp - ")):
            # the return variable, loaded into al/ax for a char/short function
            self.ret_slot = ebp_slot(prev, prev.operands[1])
            self.ret_type = {1: "unsigned char", 2: "short", 4: "int"}[prev.operands[1].size]
            self.void = False
            self.ret_ins = prev.address
            self.body_end = j - 1
        else:
            self.ret_ins = self.epi

    def type_slots(self):
        """Pre-pass: give every stack slot a C type from its accesses."""
        acc = {}       # slot -> set of sizes
        reads = {}     # slot -> set of sizes read
        first = {}     # slot -> address of its first access
        wide16 = set() # slots stored whole from a 16-bit value
        sites = {}     # slot -> addresses of its accesses
        self.w16_stores = set()
        afirst = {}    # address-taken slot -> address of its first access
        sign = {}      # slot -> 's' / 'u' hints
        fpu4 = set()   # slots the FPU reads or writes as floats
        addr = set()
        body = self.ins[self.body_start:self.body_end]
        R16 = {"eax": "ax", "edx": "dx", "ebx": "bx", "ecx": "cx"}
        FAM = {r + s_: "e%sx" % r for r in "abcd" for s_ in ("l", "h", "x")}
        FAM.update({"e%sx" % r: "e%sx" % r for r in "abcd"})

        def is16(r32, j, depth):
            """Whether r32 at body[j] (its last write at or before j) holds a value computed
            as 16 bits: a word load, a high byte cleared, or whole-register arithmetic on
            one (the high half garbage, so only the low half counts)."""
            r16 = R16.get(r32)
            while j >= 0 and k - j < 8 and depth < 3 and r16:
                i = body[j]
                ops_ = i.operands
                if i.mnemonic in ("call",) or i.mnemonic.startswith("j"):
                    return False
                if not ops_ or ops_[0].type != cx.X86_OP_REG or \
                        FAM.get(i.reg_name(ops_[0].reg)) != r32:
                    j -= 1
                    continue
                dst = i.reg_name(ops_[0].reg)
                if dst == r16 and i.mnemonic in ("mov", "movsx", "movzx", "add", "sub", "inc",
                                                 "dec", "and", "or", "xor"):
                    return True
                if i.mnemonic == "xor" and dst == r16[0] + "h" and \
                        i.op_str == "%sh, %sh" % (r16[0], r16[0]):
                    return True
                if dst != r32:
                    return False
                if i.mnemonic in ("add", "sub", "inc", "dec", "shl", "neg", "imul", "and", "or",
                                  "xor"):
                    srcs = ops_[1:]
                    if not all(o.type in (cx.X86_OP_IMM, cx.X86_OP_REG) for o in srcs):
                        return False
                    if i.mnemonic in ("add", "sub", "imul"):
                        for o in srcs:
                            if o.type == cx.X86_OP_REG and o.size == 4 and \
                                    is16(i.reg_name(o.reg), j - 1, depth + 1):
                                return True
                    j -= 1
                    continue
                return False
            return False

        for k, ins in enumerate(body):
            for n, op in enumerate(ins.operands):
                lo = local_indexed(ins, op)
                if lo is not None:
                    addr.add(lo)          # indexed local array: [reg + ebp - x]
                    afirst.setdefault(lo, ins.address)
                    continue
                off = ebp_slot(ins, op)
                if off is None and op.type == cx.X86_OP_MEM and op.mem.base and \
                        ins.reg_name(op.mem.base) == "ebp" and not op.mem.index and \
                        op.mem.disp >= 8:
                    off = -op.mem.disp    # stack parameter: typed like a slot, key < 0
                if off is None:
                    continue
                if ins.mnemonic == "lea":
                    addr.add(off)
                    afirst.setdefault(off, ins.address)
                    continue
                acc.setdefault(off, set()).add(op.size)
                if op.size == 4 and ins.mnemonic in ("fld", "fst", "fstp", "fadd", "fsub",
                                                     "fsubr", "fmul", "fdiv", "fdivr",
                                                     "fcom", "fcomp"):
                    fpu4.add(off)
                first.setdefault(off, ins.address)
                sites.setdefault(off, []).append(ins.address)
                if n == 0 and ins.mnemonic in ("mov", "add", "sub", "and", "or", "xor") and \
                        op.size == 4 and k and ins.operands[1].type == cx.X86_OP_REG:
                    # stored whole right after being computed as 16 bits (mov ax,[x];
                    # mov [l],eax): Watcom 10's store of a 2-byte variable
                    r32 = ins.reg_name(ins.operands[1].reg)
                    if is16(r32, k - 1, 0):
                        wide16.add(off)
                        if not (body[k - 1].operands and
                                body[k - 1].operands[0].type == cx.X86_OP_REG and
                                body[k - 1].operands[0].size == 2):
                            self.w16_stores.add(ins.address)
                rmw_imm = n == 0 and ins.mnemonic in ("add", "sub") and op.size == 4 and \
                    ins.operands[1].type == cx.X86_OP_IMM and \
                    not os.environ.get("LIFT_NORMWIMM")
                if not (n == 0 and ins.mnemonic == "mov") and not rmw_imm:
                    # (`add dword [x],10` is Watcom 10's x += 10 on a short too)
                    reads.setdefault(off, set()).add(op.size)
                if ins.mnemonic == "movsx":
                    sign.setdefault(off, set()).add(("s", op.size))
                elif ins.mnemonic == "movzx":
                    sign.setdefault(off, set()).add(("u", op.size))
                elif ins.mnemonic == "mov" and n == 1 and op.size < 4:
                    nxt = body[k + 1] if k + 1 < len(body) else None
                    prv = body[k - 1] if k else None
                    if nxt is not None and (nxt.mnemonic == "cwde" or
                                            (nxt.mnemonic == "movsx" and op.size == 1)):
                        sign.setdefault(off, set()).add(("s", op.size))
                    elif (nxt is not None and nxt.mnemonic == "and" and
                          nxt.op_str in ("eax, 0xff", "eax, 0xffff")) or \
                            (prv is not None and prv.mnemonic == "xor"):
                        sign.setdefault(off, set()).add(("u", op.size))
        if self.ret_slot is not None and self.ret_slot in wide16 and self.ret_type == "int" \
                and not os.environ.get("LIFT_NOSHORTRET"):
            # a 16-bit value stored whole into the return variable: a short function (its
            # return variable is read back whole either way); a choice point
            self.choices.append(first[self.ret_slot] + 0.0625)
            uses = ret_uses().get(self.va, set())
            if (first[self.ret_slot] + 0.0625 not in self.flips) == (uses != {"int"}):
                self.ret_type = "short"
        self.slot_type = {}
        self.addr_taken = addr
        self.slot_reads = reads
        self.first_acc = first
        for reg, off, _sz in self.params:
            acc.setdefault(off, set())
        for off, sizes in acc.items():
            if off == self.ret_slot:
                continue
            body_sizes = sizes - {4} if off in [p[1] for p in self.params] else sizes
            sz = min(body_sizes) if body_sizes and len(body_sizes) == 1 else 4
            spill = next((p[2] for p in self.params if p[1] == off), None)
            if spill == 1 and not sizes:
                sz = 1          # an unused char parameter: the spill says so
            if off in wide16 and reads.get(off, set()) <= {2, 4} and 4 in reads.get(off, set()):
                # a 2-byte variable read whole, or an int: a choice point
                self.choices.append(first[off] + 0.0625)
                self.choice_sites[first[off] + 0.0625] = sites[off]
            if off in wide16 and reads.get(off, set()) <= {2, 4} and \
                    (4 not in reads.get(off, set()) or first[off] + 0.0625 in self.flips):
                sz = 2
                if any(ins.mnemonic == "and" and ins.op_str.endswith("0xffff")
                       for ins in body):
                    sign.setdefault(off, set()).add(("u", 2))
            # Watcom 10 stores a short with the whole register: read only as a word, it is one
            if reads.get(off) == {2} and sizes <= {2, 4} and \
                    not os.environ.get("LIFT_NOSHORTREAD"):
                # ... or an int read through (short) casts: a choice point
                if 4 in sizes and off not in [p[1] for p in self.params]:
                    self.choices.append(first[off])
                    self.choice_sites[first[off]] = sites[off]
                if first.get(off) not in self.flips or off in [p[1] for p in self.params]:
                    sz = 2
            if off in addr:
                sz = 4
                if 2 in reads.get(off, set()) and 1 not in reads.get(off, set()) and \
                        off in afirst and not os.environ.get("LIFT_NOADDRSHORT"):
                    # an address-taken variable read as a word: a short (unless it is an
                    # array; a choice point)
                    self.choices.append(afirst[off] + 0.15625)
                    if afirst[off] + 0.15625 not in self.flips:
                        sz = 2
            # (a narrower read's extension says nothing about a wider variable's sign)
            hint = {h for h, hs in sign.get(off, set())
                    if hs == sz or os.environ.get("LIFT_ANYSIGN")}
            if sz not in (1, 2, 4, 8):
                raise Unsupported("%d-byte stack slot" % sz)
            t = (UTYPE if hint == {"u"} or (sz == 1 and hint != {"s"}) else STYPE)[sz]
            if sz == 4 and off in fpu4 and not os.environ.get("LIFT_NOFLOAT"):
                t = "float"     # stored and loaded by the FPU as 4 bytes
            self.slot_type[off] = t
        self.size_of = {"signed char": 1, "unsigned char": 1, "short": 2,
                        "unsigned short": 2, "int": 4, "unsigned": 4, "double": 8,
                        "float": 4}
        # stack parameters ([ebp+8], [ebp+12], ...): a narrow one makes callers push through a
        # register (`mov eax,0x9c; push eax`)
        self.stack_type = [self.slot_type.pop(-(8 + 4 * k), "int") for k in range(self.nstack)]
        cts = caller_types().get(self.va) if not os.environ.get("LIFT_NOCALLERTYPES") else None
        if cts and self.conv is None:
            for k in range(self.nstack):
                if 4 + k < len(cts) and cts[4 + k] == "short16":
                    self.stack_type[k] = "short"    # callers widen it from 16 bits
                elif 4 + k < len(cts) and cts[4 + k] == "int":
                    self.stack_type[k] = "int"      # callers push it as an immediate
                elif 4 + k < len(cts) and cts[4 + k] == "narrow" and \
                        self.stack_type[k] == "int":
                    self.stack_type[k] = "short"    # callers push it through a register
                elif 4 + k < len(cts) and cts[4 + k] == "signed":
                    self.stack_type[k] = {"unsigned char": "signed char",
                                          "unsigned short": "short"}.get(self.stack_type[k],
                                                                         self.stack_type[k])
        for o in [o for o in self.slot_type if o < 0]:
            del self.slot_type[o]
        # Every 4-byte slot between the saved registers and the frame bottom belongs to a
        # declared variable (-od gives slots at declaration, used or not). An address-taken
        # slot is the base of an array reaching up to the next known variable.
        top = 4 * len(self.saved) + 4
        bottom = 4 * len(self.saved) + self.frame
        known = set(self.slot_type) | {o for _r, o, _s in self.params} | addr
        if self.ret_slot:
            known.add(self.ret_slot)
        self.arrays = {}         # base offset -> size in bytes
        for a in sorted(x for x in addr if x > 0):
            above = [o for o in known if o < a and o not in range(a - 3, a)]
            nxt = max([o for o in above if o not in self.slot_type or o in addr
                       or o in [p[1] for p in self.params] or o == self.ret_slot]
                      + [top - 4])
            # a slot accessed directly between the array and the next variable is one of its
            # elements or a variable of its own: a choice point (at the array's first access)
            own = [o for o in above if o in self.slot_type and o > nxt]
            if own and a in afirst:
                self.choices.append(afirst[a] + 0.5)
                self.choice_sites[afirst[a] + 0.5] = [afirst[a]] + \
                    [x for o in own for x in sites.get(o, [])]
                if afirst[a] + 0.5 in self.flips:
                    nxt = max(own)
            size = a - nxt
            if size > 4:
                self.arrays[a] = size
            elif any(a - 3 <= o < a for o in known) and not os.environ.get("LIFT_NOSMALLARR"):
                # an address-taken slot with bytes of it read separately: a 4-byte array
                self.arrays[a] = 4
        inside = {}
        for a, size in self.arrays.items():
            for o in range(a - size + 1, a):
                inside[o] = a
        for o in list(self.slot_type):
            if o in inside:
                del self.slot_type[o]
        self.inside = inside
        for a in addr:
            if a not in self.arrays and a > 0:      # (< 0: an address-taken stack parameter)
                self.slot_type.setdefault(a, "int")
        for o in range(top, bottom + 1, 4):
            if o in known or o in inside or o in self.arrays:
                continue
            if any(o - 3 <= k <= o for k in known):
                continue
            if any(self.slot_type.get(k) == "double" and k - 7 <= o <= k for k in known):
                continue                      # the upper half of a double
            self.slot_type[o] = "int"         # declared but never used
        # callers see the access-based parameter types; the layout plan only changes how the
        # function itself declares them
        # a parameter's declared type is what the callers convert their arguments to
        ct = caller_types().get(self.va) if not os.environ.get("LIFT_NOCALLERTYPES") else None
        self.sig_types = [self.slot_type.get(off, "int") for _r, off, _s in self.params]
        # A parameter spilled as a dword is not a char (OW spills those as bytes): an int
        # read through (char) casts
        for _r, off, spill in self.params:
            if spill == 4 and self.slot_type.get(off) in ("signed char", "unsigned char") and \
                    not os.environ.get("LIFT_CHARPARM4"):
                self.slot_type[off] = "int"
        # A parameter spilled as a dword and only read as a word is a short, or an int read
        # through (short) casts; they differ in where the frame puts it: a choice point at
        # the spill.
        for n, (_r, off, spill) in enumerate(self.params):
            if spill == 4 and self.slot_type.get(off) in ("short", "unsigned short"):
                at = self.ins[self.body_start - len(self.params) + n].address
                self.choices.append(at)
                if at in self.flips:
                    self.slot_type[off] = "int"
        if ct:
            for n, (_r, off, spill) in enumerate(self.params):
                # a narrow load can also be a narrow value passed to an int: the spill width
                # decides (a char parameter is spilled as a byte, a short one as a dword).
                # Callers get this type; the definition keeps the one its frame shows.
                if ct[n] is not None and \
                        (spill == 1) == (ct[n] in ("signed char", "unsigned char")):
                    if os.environ.get("LIFT_CTDEF"):
                        self.slot_type[off] = ct[n]
                    self.sig_types[n] = ct[n]
        self.plan_slots()

    def plan_slots(self):
        """Read 2-byte types and nested locals off the frame layout (tools/slot_plan.py)."""
        params = {o: i + 1 for i, (_r, o, _s) in enumerate(self.params)}
        toks = []
        for o in sorted(set(self.slot_type) | set(params) | set(self.arrays)
                        | ({self.ret_slot} if self.ret_slot else set())):
            if o in params:
                toks.append(("P", params[o]))
            elif o == self.ret_slot:
                toks.append(("R",))
            else:
                toks.append(("L", o))
        res = slot_plan.plan(toks, len(params))
        self.nested = set()
        if res is None:
            return
        small_p, small_l, nested = res
        self.nested = nested
        for o, i in params.items():
            if i in small_p and self.size_of[self.slot_type.get(o, "int")] == 4:
                if (o in getattr(self, "addr_taken", set()) or
                        getattr(self, "slot_reads", {}).get(o) == {4}) and \
                        not os.environ.get("LIFT_ADDRSMALL"):
                    # a parameter sitting where a 2-byte one would but read only whole (an
                    # address-taken one gets its slot early): an int, pinned to its slot
                    self.force_pin = True
                    continue
                self.slot_type[o] = "short"
        for o in small_l:
            if o in self.slot_type and self.size_of[self.slot_type[o]] == 4:
                self.slot_type[o] = "short"

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
        lo = local_indexed(ins, op)
        if lo is not None:
            other = [r for r in (base, index) if r != "ebp"][0]
            sc = m.scale if index != "ebp" else 1
            iexp = self.reg(subreg(other)[0], ins).p()
            if sc != 1:
                iexp = "%s * %d" % (iexp, sc)
            arr = "l_%X" % lo
            return "*(%s *)((char *)%s + %s)" % (STYPE[size], arr, iexp)
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
        if base == "ebp" and not index and disp >= 8 and (disp - 8) % 4 == 0 \
                and (disp - 8) // 4 < self.nstack:
            k = (disp - 8) // 4
            name = "a%d" % (self.stack_base + k)
            return name if self.size_of[self.stack_type[k]] == size else \
                "*(%s *)&%s" % (STYPE[size], name)
        if base == "ebp":
            raise Unsupported("stack parameter [ebp+%d]" % disp)
        fx = fixup_at(ins, ins.disp_offset) if ins.disp_size == 4 else None
        parts = []
        if fx is not None:
            g = sym(fx.target_va)
            self.globals.add(g)
            parts.append(g)
            if base:
                bv = self.reg(base, ins)
                sp = split_sum(bv.text) if not bv.atom else None
                if sp is not None and not os.environ.get("LIFT_NOFLATIDX"):
                    # g + (x*48 + y*4): or g[x][y], (g + x*48) + y*4 (a choice point)
                    self.choices.append(ins.address + 0.65625)
                if sp is not None and (ins.address + 0.65625 in self.flips) != \
                        bool(os.environ.get("LIFT_FLATIDX")):
                    parts += [sp[0], sp[1]]
                else:
                    parts.append(bv.p())
        elif base and not index and self.reg(base, ins).tag and \
                self.reg(base, ins).tag[0] == "padd" and size in (2, 4) and disp % size == 0 \
                and re.fullmatch(r"\(\((.*) \* (2|3)\) \* 2\)", self.reg(base, ins).tag[2]) and \
                self.reg(base, ins).tag[1].startswith("*(char **)") and \
                not (size == 4 and re.fullmatch(r"\(\((.*) \* 3\) \* 2\)",
                                                self.reg(base, ins).tag[2])) and \
                not os.environ.get("LIFT_NOARRIDX"):
            # (i * 2) * 2 doubled twice, or (i * 3) * 2 (lea eax,[eax+eax*2]; add eax,eax:
            # OW folds a written `* 4` / `* 6` into a shift / imul): an index into an array
            # of shorts, scaled by the compiler
            _, ptxt, ptxtb = self.reg(base, ins).tag
            mm = re.fullmatch(r"\(\((.*) \* (2|3)\) \* 2\)", ptxtb)
            arr = "(*(%s **)%s)" % (STYPE[size], ptxt[len("*(char **)"):])
            idx = "%s * %d" % (mm.group(1), int(mm.group(2)) * 2 // size) if size == 2 \
                else mm.group(1)
            k = disp // size
            return "%s[%s%s]" % (arr, idx, " + %d" % k if k > 0 else " - %d" % -k if k else "")
        elif base and not index and disp and self.reg(base, ins).tag and \
                self.reg(base, ins).tag[0] == "padd" and \
                (ins.address + 0.125 in self.flips) != (FIELD_LAST not in self.flips):
            # p->arr[i]: the field offset belongs with the pointer (`p + 367 + i*4`), which
            # makes the pointer side the bigger tree and so evaluated first
            _, ptxt, ptxtb = self.reg(base, ins).tag
            self.choices.append(ins.address + 0.125)
            return "*(%s *)(%s %s %d + %s)" % (STYPE[size], ptxt, "+" if disp > 0 else "-",
                                               abs(disp), ptxtb)
        elif base:
            bv = self.reg(base, ins)
            if bv.tag and bv.tag[0] == "padd" and disp and not index:
                self.choices.append(ins.address + 0.125)
            pick = bv.atom and bv.text.startswith("*(int *)")
            if pick:
                self.choices.append(ins.address + 0.75)     # or the int load and cast
            if pick and ins.address + 0.75 not in self.flips and \
                    not os.environ.get("LIFT_INTPTRBASE"):
                # a pointer read from memory: *(char **)x, not (char *)*(int *)x (the int
                # load and cast keep Watcom from folding the offset into an lea)
                parts.append("*(char **)" + bv.text[len("*(int *)"):])
            else:
                parts.append("(char *)" + bv.p())
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
            if size == 4 and fx.target_va in unsigned_globals() and \
                    os.environ.get("LIFT_UGLOBALS"):
                return "*(unsigned *)%s" % addr
            return "*(%s *)%s" % (STYPE[size], addr)
        return "*(%s *)(%s)" % (STYPE[size], addr)

    def reg(self, name, ins):
        r = self.regs.get(name)
        if r is None:
            raise Unsupported("read of undefined register %s" % name)
        if r.tag and r.tag[0] == "call":
            self.used_results.add(r.tag[1])
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
            off_ = ebp_slot(ins, op)
            if off_ is not None and off_ in self.slot_val:
                return self.slot_val.pop(off_)
            pre = getattr(self, "pre_ops", {})
            if t in pre:
                # an in-place ++/-- done while the expression was being evaluated: --x
                lv = t if re.fullmatch(r"\w+", t) else "(%s)" % t
                return E("%s%s" % (pre.pop(t), lv), op.size, atom=False)
            if self.side:
                # the stores of an enclosing expression come first: (x = a, x <<= 2, x)
                text = "(%s, %s)" % (", ".join(self.side), t)
                self.side = []
                return E(text, op.size, atom=True)
            ch = getattr(self, "chain", None)
            k_ = getattr(self, "k", -1)
            # (or right after reloading the pointer: x = (p->f = v) with a short field,
            # re-read as a word and widened with cwde)
            reload = ch is not None and ch[3] + 2 == k_ and ins.mnemonic == "mov" and \
                op.size == 2 and self.body[k_ - 1].mnemonic == "mov" and \
                k_ + 1 < len(self.body) and self.body[k_ + 1].mnemonic in ("cwde", "mov") and \
                ebp_slot(self.body[k_ - 1], self.body[k_ - 1].operands[1]) is not None and \
                not os.environ.get("LIFT_NORELOADCHAIN")
            if ch is not None and ch[0] == t and ch[2] == len(self.out) - 1 and \
                    (ch[3] + 1 == k_ or reload) and self.pending is None and \
                    self.out[-1] == "    %s = %s;" % (ch[0], ch[1].text):
                # re-reading what the previous statement stored: x = (lhs = v)
                self.out.pop()
                self.chain = None
                return E("(%s = %s)" % (t, ch[1].text), op.size, atom=True)
            return E(t, op.size, atom=True)
        raise Unsupported("operand type")

    # ---- statements --------------------------------------------------------------------
    def flush_pending(self):
        if self.pending is not None:
            self.out.append("    %s;" % self.pending.text)
            self.pending = None

    def emit(self, line):
        for t, op in list(getattr(self, "pre_ops", {}).items()):
            # never read back: a statement of its own
            del self.pre_ops[t]
            self.out.append("    %s%s;" % (op, t if re.fullmatch(r"\w+", t) else "(%s)" % t))
        self.flush_pending()
        self.out.append("    " + line)

    def set_reg(self, name, expr):
        self.stale.discard(name)
        cur = self.regs.get(name)
        if self.pending is not None and cur is self.pending and expr is not cur:
            self.flush_pending()
        if not hasattr(expr, "born"):
            # where the evaluation of this value started: the operand of a commutative op
            # computed first was the left one in the source
            expr.born = self.born_hint if self.born_hint is not None else getattr(self, "k", 0)
        self.born_hint = None
        self.regs[name] = expr

    def bitfield8(self, load, start, length):
        """A read of an unsigned char bit-field: `load` is the 8-bit memory read."""
        mm = re.fullmatch(r"\*\((?:signed |unsigned )?char \*\)(.*)", load.text)
        if not mm or os.environ.get("LIFT_NOBITFIELD"):
            return None
        tag = "bf8_%d_%d" % (start, length)
        pad = "unsigned char _:%d; " % start if start else ""
        self.structs.add("struct %s { %sunsigned char f:%d; };" % (tag, pad, length))
        addr = mm.group(1)
        return E("((struct %s *)%s)->f" % (tag, addr if addr.startswith("(") else "&" + addr),
                 1, atom=True)

    def bitfield(self, load, start, length, signed=False):
        """A read of a 16-bit bit-field: `load` is the 16-bit memory read. Watcom's bit-fields
        are unsigned either way; a `short` one widens with cwde, `unsigned short` with a
        zero extension."""
        mm = re.fullmatch(r"\*\((?:unsigned )?short \*\)(.*)", load.text)
        if not mm or os.environ.get("LIFT_NOBITFIELD"):
            return None
        t = "short" if signed else "unsigned short"
        tag = "bf%s16_%d_%d" % ("s" if signed else "", start, length)
        pad = "%s _:%d; " % (t, start) if start else ""
        self.structs.add("struct %s { %s%s f:%d; };" % (tag, pad, t, length))
        addr = mm.group(1)
        return E("((struct %s *)%s)->f" % (tag, addr if addr.startswith("(") else "&" + addr),
                 2, atom=True, tag=("bf", load, start, length))

    def operand_order(self, ins, a, b, k0):
        """Source order of a commutative op's operands; `a` is the destination register's.

        The code generator (TNBinary) evaluates the left operand first when it has at least
        as many tree nodes as the right one, else the right one first. So the operand the
        code computed first is the left one, unless it is the bigger one: then either order
        gives the same evaluation and this is a choice point, defaulting to the destination
        register on the left. The batch driver (tools/lift_all.py) flips choice points near
        a mismatch and keeps what helps."""
        first, second = (b, a) if getattr(b, "born", k0) < getattr(a, "born", k0) else (a, b)
        if kids(first.text) <= kids(second.text) and not os.environ.get("LIFT_ONLYAMBIG"):
            # the tree-size estimate can be off: the other order is a choice point too
            self.choices.append(ins.address)
            if ins.address in self.flips:
                return second, first
            return first, second
        if kids(first.text) > kids(second.text):
            self.choices.append(ins.address)
            left, right = (first, second) if os.environ.get("LIFT_AMBIG") == "born" else (a, b)
            if ins.address in self.flips:
                left, right = right, left
            return left, right
        return first, second

    def lift(self):
        self.prologue()
        self.type_slots()
        targets = set()
        for ins in self.ins:
            if cx.X86_GRP_JUMP in ins.groups and ins.operands and \
                    ins.operands[0].type == cx.X86_OP_IMM:
                targets.add(ins.operands[0].imm)
        self.switches = self.find_switches()
        self.cswitches = self.find_cswitches()
        for sw in list(self.switches.values()) + list(self.cswitches.values()):
            targets |= set(sw["entries"])
        self.targets = targets
        self.sw_stack = []       # open switches, innermost last
        body = self.ins[self.body_start:self.body_end]
        self.body = body
        # where each switch table sat: the compiler is told to put the table there
        table_end = {}
        for t, n in IMG.tables:
            for sw in self.switches.values():
                if sw["table"] == t:
                    table_end[t + n] = ("scn", sw["scan_values"]) if "scan_values" in sw \
                        else ("tbl", t)
        dead = False
        for k, ins in enumerate(body):
            if ins.address in table_end:
                self.flush_pending()
                # the original's jmp over the table: the compiler inserts its own
                if self.out and self.out[-1] == "    goto L%X;" % ins.address:
                    self.out.pop()
                self.out.append("__dagger_%s%X:;" % table_end[ins.address])
            if ins.address in self.cskip:
                continue        # a switch's compare tree: the compiler builds it again
            if ins.address in self.cswitches and self.cswitches[ins.address].get("var") is None:
                self.k = k
                self.cswitch_start(ins, self.cswitches[ins.address])
                continue
            if ins.address in targets:
                self.flush_pending()
                sw = self.switch_owning(ins.address)
                if sw is not None:
                    if ins.address != sw.get("default"):
                        for v in sw["cases"].get(ins.address, []):
                            self.out.append("case %d:" % v)
                    if sw.get("default") == ins.address:
                        self.out.append("default:")
                self.out.append("L%X:;" % ins.address)
                self.regs = {}
                self.after_return = False
                dead = False
            if ins.address in self.cswitches and not dead:
                self.k = k
                self.cswitch_start(ins, self.cswitches[ins.address])     # switch (v)
                continue
            if dead:
                continue        # alignment padding after a jump (before a switch table)
            self.k = k
            self.step(ins)
            if ins.mnemonic == "jmp":
                dead = True
        self.flush_pending()

    def cond_text(self, m):
        opr, uns = JCC[m]
        kind = self.flags[0]
        a, b = self.flags[1], self.flags[2]
        if kind == "fcmp":
            return "%s %s %s" % (a.p(), opr, b.p())
        if kind == "cmp":
            if a.size == 2 and a.atom and re.fullmatch(r"\(?-\d+\)?", b.text) and \
                    not os.environ.get("LIFT_NOSHORTNEG"):
                # cmp word [x], -k: compared at 16 bits, so the constant was a short (OW
                # widens a compare with a negative int)
                b = E("(short)%s" % b.p(), 2, True)
            # two narrow values extended differently ((int)(unsigned short)x vs (int)(short)y):
            # written with explicit (int) casts OW compares them narrow; promoted implicitly,
            # as ints, like Watcom 10
            ext = re.compile(r"\(int\)\(((?:unsigned |signed )?(?:char|short))\)(.+)")
            ma_, mb_ = ext.fullmatch(a.text), ext.fullmatch(b.text)
            if ma_ and mb_ and ("unsigned" in ma_.group(1)) != ("unsigned" in mb_.group(1)) and \
                    not os.environ.get("LIFT_NOMIXEDCMP"):
                def narrow(m_):
                    t_, inner = m_.group(1), m_.group(2)
                    mm_ = re.fullmatch(r"\*\((?:unsigned |signed )?(?:char|short) \*\)(.+)", inner)
                    if mm_:
                        return "*(%s *)%s" % (t_, mm_.group(1))
                    return "(%s)%s" % (t_, inner)
                return "%s %s %s" % (narrow(ma_), opr, narrow(mb_))
            if a.size == 1 and b.size == 1 and not os.environ.get("LIFT_NOBYTECMP"):
                # a byte compare: both operands the same char type (mixing signed and
                # unsigned char promotes both to int)
                def ctype(e):
                    if re.fullmatch(r"[al]_?\w+", e.text):
                        t = self.var_type(e.text)
                        if t in ("signed char", "unsigned char"):
                            return t
                    m_ = re.match(r"\*\((signed char|unsigned char) \*\)", e.text)
                    return m_.group(1) if m_ else None
                want = ("unsigned char" if uns else "signed char") if opr not in ("==", "!=") \
                    else (ctype(b) or ctype(a) or "unsigned char")

                def conv(e):
                    t = ctype(e)
                    if t == want or e.text.lstrip("-").isdigit():
                        return e
                    if e.atom and re.match(r"\*\((signed|unsigned) char \*\)", e.text):
                        return E(re.sub(r"^\*\((signed|unsigned) char \*\)", "*(%s *)" % want,
                                        e.text), 1, True)
                    return E("(%s)%s" % (want, e.p()), 1, True)
                return "%s %s %s" % (conv(a).p(), opr, conv(b).p())
            if a.size == 2 and b.size == 2 and a.atom and b.atom and opr not in ("==", "!=") \
                    and not os.environ.get("LIFT_NOWORDCMP"):
                # an ordered word compare: both operands of the jump's signedness (mixing
                # short and unsigned short promotes both to int)
                want = "unsigned short" if uns else "short"

                def convw(e):
                    if e.text.lstrip("-").isdigit():
                        return e
                    if re.match(r"\*\((?:unsigned )?short \*\)", e.text):
                        return E(re.sub(r"^\*\((?:unsigned )?short \*\)", "*(%s *)" % want,
                                        e.text), 2, True)
                    if re.fullmatch(r"[al]_?\w+", e.text) and self.var_type(e.text) == want:
                        return e
                    return E("(%s)%s" % (want, e.p()), 2, True)
                return "%s %s %s" % (convw(a).p(), opr, convw(b).p())
            if a.size == 2 and b.size == 2 and not a.atom:
                # a 16-bit register value against a 16-bit operand: cmp ax, word [x], with the
                # jump's signedness
                a = E("(%s)%s" % ("unsigned short" if uns else "short", a.p()), 2, True)
                return "%s %s %s" % (a.p(), opr, b.p())
            if uns and a.size < 4 and a.atom:
                # a narrow operand: read it unsigned and let the compare stay narrow
                # (an explicit (unsigned) cast would widen it)
                t = a.text.replace("*(signed char *)", "*(unsigned char *)", 1) \
                          .replace("*(short *)", "*(unsigned short *)", 1)
                if re.fullmatch(r"[al]_?\w+", t):
                    var_t = self.var_type(t)
                    if var_t in ("signed char", "short"):
                        t = "(unsigned %s)%s" % ("char" if var_t == "signed char" else "short", t)
                a = E(t, a.size, True)
            elif uns:
                a = E("(unsigned)" + a.p(), 4)
            return "%s %s %s" % (a.p(), opr, b.p())
        if kind == "test":
            if opr not in ("==", "!=") and a.text == b.text and not uns:
                # test x,x; jle: a signed compare with zero
                return "%s %s 0" % (a.p(), opr)
            if opr not in ("==", "!="):
                raise Unsupported("test with ordered jcc")
            if a.text == b.text:
                return "%s %s 0" % (a.p(), opr)
            return "(%s & %s) %s 0" % (a.p(), b.p(), opr)
        raise Unsupported("jcc on arithmetic flags")

    def params_offs(self):
        return {p[1] for p in self.params}

    def fpu_step(self, ins):
        """x87 code: an expression stack of doubles. Returns True when handled."""
        m, ops = ins.mnemonic, ins.operands
        st = self.fpst
        nxt = self.body[self.k + 1] if self.k + 1 < len(self.body) else None
        if m == "sub" and ins.op_str == "esp, 8" and nxt is not None and \
                nxt.mnemonic == "fstp" and nxt.op_str == "qword ptr [esp]":
            return True                     # room for a double argument
        if m == "fstp" and ins.op_str == "qword ptr [esp]":
            self.pushes.append(E("", 0))    # (a double takes two stack slots)
            self.pushes.append(st.pop())
            return True
        if m == "call" and ops[0].type == cx.X86_OP_IMM and ops[0].imm == CHP and st:
            st[-1] = E(st[-1].text, 8, st[-1].atom, tag=("chp", st[-1].tag))
            return True
        if m == "call" and ops[0].type == cx.X86_OP_IMM and nxt is not None and \
                nxt.mnemonic.startswith("f") and nxt.mnemonic not in ("fld", "fild", "fld1",
                                                                       "fldz"):
            # a function returning a double (in ST0); its arguments are doubles on the stack
            name = sym(ops[0].imm)
            n = callee_pops(ops[0].imm) // 8
            args = []
            for _ in range(n):
                v = self.pushes.pop()
                w = self.pushes.pop()       # the double's other half
                if v.size != 8 and re.fullmatch(r"-?\d+", v.text) and \
                        re.fullmatch(r"-?\d+", w.text):
                    # a double constant pushed as two dwords (low half last)
                    import struct
                    bits = (int(w.text) & 0xFFFFFFFF) << 32 | (int(v.text) & 0xFFFFFFFF)
                    v = E(repr(struct.unpack("<d", struct.pack("<Q", bits))[0]), 8, True)
                args.append(v.text)
            self.fpcalls[name] = n
            st.append(E("%s(%s)" % (name, ", ".join(args)), 8, atom=True))
            return True
        if not m.startswith("f") and m != "sahf":
            return False
        if m == "fnstsw":
            self.stale.add("eax")
            return True
        if m == "sahf":
            return True

        def operand(op):
            if op.size == 8:
                return E(self.mem(op, ins, 8), 8, atom=True)
            if op.size == 4 and m.startswith("fi"):
                return E(self.mem(op, ins, 4), 4, atom=True)
            if op.size == 2 and m.startswith("fi"):
                return E(self.mem(op, ins, 2), 2, atom=True)
            if op.size == 4:
                t = self.mem(op, ins, 4)
                return E(t.replace("*(int *)", "*(float *)", 1), 4, atom=True)
            raise Unsupported("x87 operand")

        if m == "fld1":
            st.append(E("1.0", 8, atom=True))
            return True
        if m == "fldz":
            st.append(E("0.0", 8, atom=True))
            return True
        if m in ("fld", "fild"):
            if ops[0].type != cx.X86_OP_MEM:
                raise Unsupported("x87 register load")
            off = ebp_slot(ins, ops[0])
            if m == "fild" and off is not None and off in self.fild_val:
                st.append(self.fild_val.pop(off))   # an int converted through a temp
            else:
                st.append(operand(ops[0]))
            return True
        base = m[:-1] if m.endswith("p") else m
        rev = False
        if base.endswith("r") and base[:-1] in FP_OPS:
            base, rev = base[:-1], True
        if base in FP_OPS:
            opr = FP_OPS[base]
            if m.endswith("p"):
                # fXXXp st(1): st1 = st1 op st0 (reversed: st0 op st1), pop
                b, a = st.pop(), st.pop()
                if rev:
                    a, b = b, a
            elif ops and ops[0].type == cx.X86_OP_MEM:
                a, b = st.pop(), operand(ops[0])
                other = a
                if rev:
                    a, b = b, a
                # (the memory operand is the left one: maybe a compound assignment to it)
                tag = ("fopm", a.text, opr, other) if rev or opr in ("+", "*") else None
                e = E("%s %s %s" % (a.p(), opr, b.p()), 8, tag=tag)
                e.fparts = (a, opr, b)
                st.append(e)
                return True
            else:
                raise Unsupported("x87 register operation")
            if a.size != 8 and b.size != 8 and not os.environ.get("LIFT_NOFPCAST"):
                # two ints loaded with fild: a floating-point operation on them
                a = E("(double)%s" % a.p(), 8, atom=True)
            st.append(E("%s %s %s" % (a.p(), opr, b.p()), 8))
            return True
        if m == "fcomp" and ops and ops[0].type == cx.X86_OP_MEM:
            a = st.pop()
            self.flags = ("fcmp", a, operand(ops[0]))
            return True
        if m == "fstp" and ops[0].size == 4 and nxt is not None and nxt.mnemonic == "fld" and \
                nxt.op_str == ins.op_str and not os.environ.get("LIFT_NOF32TEMP"):
            off = ebp_slot(ins, ops[0])
            uses = [x for x in self.body for o in x.operands
                    if o.type == cx.X86_OP_MEM and ebp_slot(x, o) == off]
            pairs = all(x.mnemonic in ("fstp", "fld") for x in uses)
            if off is not None and pairs and off not in self.params_offs():
                # rounded to a float through a temp: the expression's type is float
                v = st.pop()
                self.temps.add(off)
                self.skip_addr = nxt.address
                st.append(E("(float)%s" % v.p(), 8, atom=True, tag=("f32", v)))
                return True
        if m in ("fstp", "fistp"):
            v = st.pop()
            inner = v.tag[1] if v.tag and v.tag[0] == "chp" else v.tag
            if m == "fistp" and inner and inner[0] == "f32" and \
                    getattr(inner[1], "fparts", None) and not os.environ.get("LIFT_NOFCOMPOUND"):
                # (int)(float)((float)x op y) stored back to x: x op= y
                lhs = self.mem(ops[0], ins, ops[0].size)
                fa, opr, fb = inner[1].fparts
                if fa.tag and fa.tag[0] == "f32" and fa.tag[1].text == lhs:
                    self.emit("%s %s= %s;" % (lhs, opr, fb.text))
                    return True
            if m == "fistp":
                # (a 64-bit store whose low half is read: the conversion to unsigned)
                v = E("(%s)%s" % ("unsigned" if ops[0].size == 8 else "int", v.p()), 4)
            op = ops[0]
            off = ebp_slot(ins, op)
            if m == "fistp" and off is not None and nxt is not None and nxt.mnemonic == "mov" \
                    and nxt.operands[0].type == cx.X86_OP_REG and \
                    nxt.op_str.split(", ", 1)[1] == ins.op_str:
                # converted through a temp slot, read straight back
                self.temps.add(off)
                self.skip_addr = nxt.address
                self.set_reg(subreg(nxt.reg_name(nxt.operands[0].reg))[0], v)
                return True
            if m == "fistp" and off is not None and \
                    sum(1 for x in self.body for o in x.operands if ebp_slot(x, o) == off) == 2 \
                    and not os.environ.get("LIFT_NOFISTPTEMP"):
                # converted through a temp read once later: the read gets the conversion
                self.temps.add(off)
                if op.size == 8:
                    self.temps.add(off - 4)     # (a 64-bit temp: both halves)
                self.slot_val[off] = v
                return True
            if m == "fistp" and off is not None and nxt is not None and nxt.mnemonic == "push" \
                    and nxt.op_str == ins.op_str:
                # converted through a temp slot and pushed as an argument
                self.temps.add(off)
                self.skip_addr = nxt.address
                self.pushes.append(v)
                return True
            lhs = self.mem(op, ins, 8 if op.size == 8 else op.size)
            if v.tag and v.tag[0] == "fopm" and v.tag[1] == lhs and m == "fstp":
                # fld y; fsubr [x]; fstp [x]: x -= y
                self.emit("%s %s= %s;" % (lhs, v.tag[2], v.tag[3].text))
                return True
            if op.size == 4 and m == "fstp":
                lhs = lhs.replace("*(int *)", "*(float *)", 1)
            self.emit("%s = %s;" % (lhs, v.text))
            return True
        raise Unsupported("instruction %s" % m)

    def step(self, ins):
        m, ops = ins.mnemonic, ins.operands
        if ins.address == getattr(self, "skip_addr", None) or \
                ins.address in getattr(self, "skip_set", ()):
            return
        if m in ("movsd", "movsw", "movsb") and not os.environ.get("LIFT_NOMOVS"):
            # lea edi,[dst]; mov esi,src; movsd...: a struct assignment
            n, j = 0, self.k
            sizes = {"movsd": 4, "movsw": 2, "movsb": 1}
            while j < len(self.body) and self.body[j].mnemonic in sizes and \
                    not self.body[j].op_str.startswith("rep"):
                n += sizes[self.body[j].mnemonic]
                if j > self.k:
                    self.skip_set.add(self.body[j].address)
                j += 1
            dst, src_ = self.reg("edi", ins), self.reg("esi", ins)
            tag = "s%d" % n
            self.structs.add("struct %s { %s; };" % (
                tag, "int a[%d]" % (n // 4) if n % 4 == 0 else "char a[%d]" % n))

            def addr(e):
                mm = re.fullmatch(r"\(int\)(&?)(.*)", e.text)
                if mm:
                    return mm.group(2) if mm.group(1) == "" else "&" + mm.group(2)
                return "(char *)%s" % e.p()
            self.emit("*(struct %s *)%s = *(struct %s *)%s;" % (tag, addr(dst), tag, addr(src_)))
            self.regs.pop("edi", None)
            self.regs.pop("esi", None)
            return
        if m == "nop":
            return
        if (m.startswith("f") or m in ("sahf", "call") or self.fpst or
                (m == "sub" and ins.op_str == "esp, 8")) and self.fpu_step(ins):
            return
        if m == "add" and getattr(self, "skip_add", False):
            self.skip_add = False   # caller's cleanup after a cdecl call
            return
        if m == "mov" and ops[1].type == cx.X86_OP_REG and ins.reg_name(ops[1].reg) == "ds" \
                and ops[0].type == cx.X86_OP_REG and not os.environ.get("LIFT_NOFAR"):
            # the data segment: the selector half of a far pointer being built
            self.set_reg(subreg(ins.reg_name(ops[0].reg))[0], E("__DS__", 4, True, tag=("ds",)))
            return
        if m == "mov":
            d, s = ops
            if d.type == cx.X86_OP_REG:
                full, sz = subreg(ins.reg_name(d.reg))
                v = self.src(s, ins)
                cur = self.regs.get(full)
                if sz == 1 and cur is not None and cur.tag == ("hiclr",):
                    # xor dh,dh; mov dl,[x]: a byte zero-extended to 16 bits
                    self.set_reg(full, E("(unsigned short)(unsigned char)" + v.p(), 2))
                elif sz < 4 and cur is not None and cur.text == "0":
                    self.set_reg(full, E(ext_text(UTYPE[sz], byteval(v) if sz == 1 else v), 4,
                                         atom=IMPLICIT))
                elif sz < 4:
                    self.set_reg(full, E(v.text, sz, v.atom))
                else:
                    self.set_reg(full, v)
                return
            v = self.src(s, ins)
            if v.tag and v.tag[0] == "ds" and "eax" in self.regs and \
                    not os.environ.get("LIFT_NOFAR"):
                # the selector of a far pointer to what eax points at: FP_SEG()
                v = E("_FP_SEG((void *)%s)" % self.regs["eax"].p(), 2, atom=True)
                self.uses_fpseg = True
            nx = self.body[self.k + 1] if self.k + 1 < len(self.body) else None
            off_ = ebp_slot(ins, d)
            if nx is not None and nx.mnemonic == "fild" and off_ is not None and \
                    re.sub(r"^\w+ ptr ", "", nx.op_str) == \
                    re.sub(r"^\w+ ptr ", "", ins.op_str.split(", ")[0]) and d.size == 4 and \
                    sum(1 for x in self.body for o in x.operands
                        if ebp_slot(x, o) == off_) == 2:
                # an int stored to a temp only to be converted to a double (a short one
                # when read back as a word)
                self.fild_val[off_] = v if nx.operands[0].size == 4 else \
                    E("(short)%s" % v.p(), 2, atom=True)
                self.temps.add(off_)
                return
            lhs = self.mem(d, ins, d.size)
            mm = re.fullmatch(r"\*\(int \*\)&(l_[0-9A-F]+|a\d+)", lhs)
            pv = self.body[self.k - 1] if getattr(self, "k", 0) > 0 else None
            conv = pv is not None and pv.mnemonic == "mov" and s.type == cx.X86_OP_REG and \
                len(pv.operands) == 2 and pv.operands[0].type == cx.X86_OP_REG and \
                pv.operands[1].type == cx.X86_OP_REG and pv.operands[0].reg == s.reg and \
                pv.operands[0].size == 4 and not os.environ.get("LIFT_NOCONVSTORE")
            plain = False
            if mm and off_ is not None and off_ not in self.addr_taken and d.size == 4 and \
                    off_ in self.first_acc and not os.environ.get("LIFT_NOPLAINSHORT"):
                # an int stored to a short variable whose address isn't taken: through
                # its address (keeps the store whole), or plainly (the compiler widens
                # the store); a choice point for the variable
                key = self.first_acc[off_] + 0.59375
                self.choices.append(key)
                plain = key in self.flips
            if mm and d.size == 4 and (v.size == 2 or ins.address in self.w16_stores or conv or
                                       plain) and \
                    self.var_type(mm.group(1)) in ("short", "unsigned short") and \
                    not os.environ.get("LIFT_NOSHORTSTORE"):
                # Watcom 10 stores a short variable with the whole register
                lhs = mm.group(1)
            if lhs == "@RET":
                self.void = False
                self.emit("return %s;" % v.text)
                self.after_return = True
                return
            src0 = subreg(ins.reg_name(s.reg))[0] if s.type == cx.X86_OP_REG else None
            held = [r for r in self.regs if r != src0 and self.regs[r] is not self.pending
                    and r not in self.stale and self.live_later(r)]
            nx2 = self.body[self.k + 1] if self.k + 1 < len(self.body) else None
            arm_end = nx2 is not None and (nx2.mnemonic == "jmp" or nx2.address in self.targets) \
                and not os.environ.get("LIFT_NOARMEND")
            if (held or self.pushes) and ebp_slot(ins, d) is not None and not arm_end and \
                    not os.environ.get("LIFT_NOCOMMA"):
                # a store in the middle of an expression (arguments of a call are being
                # held in registers or pushed): a comma expression, attached to the next
                # value read
                self.side.append("%s = %s" % (lhs, v.text))
                if src0:
                    self.regs[src0] = v
                return
            self.emit("%s = %s;" % (lhs, v.text))
            # -od: nothing survives into the next statement, except the value just stored,
            # which an enclosing assignment may reuse (a = b = x), and the registers of the
            # address, through which an enclosing assignment re-reads it (x = (p->f = 0))
            src = subreg(ins.reg_name(s.reg))[0] if s.type == cx.X86_OP_REG else None
            keep = {}
            for r in (d.mem.base, d.mem.index):
                if r and ins.reg_name(r) != "ebp":
                    full_r = subreg(ins.reg_name(r))[0]
                    if full_r in self.regs:
                        keep[full_r] = self.regs[full_r]
            if src:
                keep[src] = v
            self.regs = keep
            self.stale = set(keep)
            # (a local's reload is the dead load of a statement like x++, not a chain)
            self.chain = None if ebp_slot(ins, d) is not None else \
                (lhs, v, len(self.out) - 1, getattr(self, "k", -1))
            return
        if m in ("movsx", "movzx"):
            d, s = ops
            full, _ = subreg(ins.reg_name(d.reg))
            v = self.src(s, ins)
            if m == "movsx" and v.tag and v.tag[0] == "bf":
                _, load, start, length = v.tag
                self.set_reg(full, E(self.bitfield(load, start, length, True).text, 4, atom=True))
                return
            t = (STYPE if m == "movsx" else UTYPE)[s.size]
            if d.size == 2 and s.size == 1 and not os.environ.get("LIFT_NOMOVSX16"):
                # extended to 16 bits only: a short (char) operand of short arithmetic
                tt = "short" if m == "movsx" else "unsigned short"
                vv = v if v.text.startswith("*(%s *)" % t) else E("(%s)%s" % (t, v.p()), 1, True)
                self.set_reg(full, E("(%s)%s" % (tt, vv.p()), 2, atom=True))
                return
            self.set_reg(full, E(ext_text(t, v), 4, atom=IMPLICIT))
            return
        if m == "cwde" and self.regs.get("eax") is not None and \
                (self.regs["eax"].tag or ("",))[0] == "bf":
            _, load, start, length = self.regs["eax"].tag
            self.set_reg("eax", E(self.bitfield(load, start, length, True).text, 4, atom=True))
            return
        if m == "cwde":
            v = self.reg("eax", ins)
            self.set_reg("eax", E("(int)(short)" + v.p(), 4))
            return
        if m == "cwd":
            # sign-extend ax into dx: the start of a 16-bit division
            v = self.reg("eax", ins)
            self.set_reg("edx", E("%s >> 15" % v.p(), 2, tag=("sign16", v)))
            return
        if m == "cdq":
            v = self.reg("eax", ins)
            self.set_reg("edx", E("%s >> 31" % v.p(), 4, tag=("sign", v)))
            return
        if m == "xor" and ins.op_str in ("ah, ah", "dh, dh", "bh, bh", "ch, ch"):
            full = {"a": "eax", "d": "edx", "b": "ebx", "c": "ecx"}[ins.op_str[0]]
            nx = self.body[self.k + 1] if self.k + 1 < len(self.body) else None
            if full not in self.regs or (
                    nx is not None and nx.mnemonic == "mov" and
                    nx.op_str.startswith(ins.op_str[0] + "l, ") and
                    not os.environ.get("LIFT_NOHICLRNEXT")):
                # the high byte cleared before the low byte is loaded
                self.set_reg(full, E("0", 4, atom=True, tag=("hiclr",)))
                return
            v = self.reg(full, ins)
            if v.size == 2 and v.atom and not os.environ.get("LIFT_NOAHMASK"):
                # a word read with its high byte cleared: x & 0xff (mov ax,[x]; xor ah,ah)
                self.set_reg(full, E("%s & 255" % v.p(), 2))
                return
            # byte zero-extended to 16 bits: mov al,[x]; xor ah,ah
            self.set_reg(full, E("(unsigned short)(unsigned char)" + v.p(), 2))
            return
        if m in ("and", "or", "xor", "add", "sub") and ops[0].type == cx.X86_OP_REG and \
                ins.reg_name(ops[0].reg) in ("ah", "bh", "ch", "dh") and \
                ops[1].type == cx.X86_OP_IMM:
            # a 16-bit op whose constant leaves the low byte alone: x & 0x3ff is `and ah,3`
            full = {"a": "eax", "d": "edx", "b": "ebx", "c": "ecx"}[ins.reg_name(ops[0].reg)[0]]
            v = self.reg(full, ins)
            k = (ops[1].imm & 0xFF) << 8 | (0xFF if m == "and" else 0)
            opch = {"and": "&", "or": "|", "xor": "^", "add": "+", "sub": "-"}[m]
            if m == "and" and v.size == 2 and ((ops[1].imm & 0xFF) + 1) & (ops[1].imm & 0xFF) == 0:
                bf = self.bitfield(v, 0, 8 + (ops[1].imm & 0xFF).bit_length())
                if bf is not None:
                    self.set_reg(full, bf)
                    return
            t = v.p() if v.size == 2 else "(short)%s" % v.p()
            self.set_reg(full, E("%s %s %d" % (t, opch, k), 2))
            return
        if m == "test" and ops[0].type == cx.X86_OP_REG and ins.reg_name(ops[0].reg) == "ah" \
                and ops[1].type == cx.X86_OP_IMM:
            v = self.reg("eax", ins)
            self.flags = ("test", v, E(str(ops[1].imm << 8), 4, atom=True))
            return
        if m in ("xor", "sub") and ops[0].type == cx.X86_OP_REG and \
                ops[1].type == cx.X86_OP_REG and ops[0].reg == ops[1].reg:
            full_r, sz_r = subreg(ins.reg_name(ops[0].reg))
            cur_r = self.regs.get(full_r)
            if sz_r < 4 and cur_r is not None and cur_r.size == 4 and \
                    not os.environ.get("LIFT_NOXORLOW"):
                # xor ax,ax clears only the low word
                self.set_reg(full_r, E("%s & %d" % (cur_r.p(), -(1 << (8 * sz_r))), 4))
                return
            if sz_r == 1 and cur_r is not None and cur_r.size == 2 and \
                    ins.reg_name(ops[0].reg).endswith("l") and \
                    not os.environ.get("LIFT_NOXORLOW16"):
                # xor bl,bl on a word: its high byte kept (w & 0xff00)
                self.set_reg(full_r, E("%s & %d" % (cur_r.p(), -256), 2))
                return
            self.set_reg(full_r, E("0", 4, atom=True))
            return
        if m == "imul" and len(ops) == 3:
            full, _ = subreg(ins.reg_name(ops[0].reg))
            a = self.src(ops[1], ins)
            b = self.src(ops[2], ins)
            self.set_reg(full, E("%s * %s" % (a.p(), b.p()), 4))
            return
        if m == "idiv" and ops[0].size == 2:
            # cwd; idiv bx: short division, quotient in ax, remainder in dx
            a = self.reg("eax", ins)
            d = self.regs.get("edx")
            if not (d is not None and d.tag and d.tag[0] == "sign16"):
                raise Unsupported("16-bit idiv without cwd")
            b = self.src(ops[0], ins)
            sa = a if a.size == 2 else \
                E(a.text.replace("*(int *)", "*(short *)", 1), 2, True) \
                if a.atom and a.text.startswith("*(int *)") else E("(short)%s" % a.p(), 2, True)
            sb = b if b.size == 2 or re.fullmatch(r"-?\d+", b.text) else \
                E("(short)%s" % b.p(), 2, True)
            self.set_reg("eax", E("%s / %s" % (sa.p(), sb.p()), 2))
            self.set_reg("edx", E("%s %% %s" % (sa.p(), sb.p()), 2))
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
                k0 = getattr(self, "k", 0)
                self.born_hint = min(getattr(a, "born", k0), getattr(b, "born", k0))
                if m in ("shl", "sar", "shr") and s.type == cx.X86_OP_REG:
                    # shift by cl: the compiler loads only the low byte of an int count
                    mb = re.fullmatch(r"\*\((?:signed|unsigned) char \*\)&(\w+)", b.text)
                    if mb:
                        b = E(mb.group(1), 4, atom=True)
                    self.regs.pop("ecx", None)      # the count is used up, not an argument
                if m == "and" and s.type == cx.X86_OP_IMM and sz == 1 and a.size > 1 and \
                        not os.environ.get("LIFT_NOAND8"):
                    # `and dl,0x80` on a wider value keeps its high bits: x & ~0x7f
                    v = E("%s & %d" % (a.p(), (s.imm & 0xFF) - 256), a.size,
                          tag=("and8", a, s.imm & 0xFF))
                    self.set_reg(full, v)
                    self.flags = ("val", v, None)
                    return
                if m == "and" and s.type == cx.X86_OP_IMM and sz == 4 and s.imm in (0xFF, 0xFFFF) \
                        and a.size == 4 and a.atom and (a.text.startswith("*(int *)") or (
                            re.fullmatch(r"[al]_?[0-9A-F]+|a\d+", a.text) and
                            self.var_type(a.text) == "int")) and \
                        not os.environ.get("LIFT_ANDCAST"):
                    # a dword read masked to 16/8 bits: `x & 0xffff` (the cast narrows the load)
                    v = E("%s & %d" % (a.p(), s.imm), 4)
                    self.set_reg(full, v)
                    self.flags = ("val", v, None)
                    return
                if m == "and" and s.type == cx.X86_OP_IMM and sz == 4 and s.imm in (0xFF, 0xFFFF):
                    t = "unsigned char" if s.imm == 0xFF else "unsigned short"
                    av = byteval(a) if s.imm == 0xFF else a
                    imp = IMPLICIT
                    if not IMPLICIT and av.atom and re.fullmatch(
                            r"\*\((?:unsigned |signed )?(?:char|short) \*\)(.+)", av.text) and \
                            not os.environ.get("LIFT_NOIMPSITE"):
                        # a zero-extended read: `(int)(unsigned short)*(short *)p` or
                        # `*(unsigned short *)p` (fewer tree nodes: evaluated later); a
                        # choice point
                        self.choices.append(ins.address + 0.5625)
                        imp = ins.address + 0.5625 in self.flips
                    self.set_reg(full, E(ext_text(t, av, imp), 4, atom=imp))
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
                if m == "add" and s.type == cx.X86_OP_REG and d.reg == s.reg and sz == 1 and \
                        a.size == 1 and a.atom:
                    # add al,al on a byte read: the shift left of a bit-field read
                    self.set_reg(full, E("%s << 1" % a.p(), 1, tag=("bfshl8", a, 1)))
                    return
                if m == "add" and s.type == cx.X86_OP_REG and d.reg == s.reg:
                    # add r,r: doubling (2-byte array indexing), not x + x
                    v = E("%s * 2" % a.p(), 4)
                    self.set_reg(full, v)
                    self.flags = ("val", v, None)
                    return
                if m == "shr" and sz == 1 and s.type == cx.X86_OP_IMM:
                    # shr al,r on a byte read (shifted left first: shl al,s / add al,al)
                    base_, sft = (a.tag[1], a.tag[2]) if a.tag and a.tag[0] == "bfshl8" else \
                        (a, 0)
                    if base_.size == 1 and base_.atom:
                        bf = self.bitfield8(base_, s.imm - sft, 8 - s.imm)
                        if bf is not None:
                            self.set_reg(full, bf)
                            return
                if m == "shl" and sz == 1 and s.type == cx.X86_OP_IMM and a.size == 1 and a.atom:
                    self.set_reg(full, E("%s << %d" % (a.p(), s.imm), 1, tag=("bfshl8", a, s.imm)))
                    return
                if m == "shr" and sz == 2 and s.type == cx.X86_OP_IMM and a.tag and \
                        a.tag[0] in ("shl2", "bfshl"):
                    # shl eax,s; shr ax,r on a 16-bit read: a bit-field
                    bf = self.bitfield(a.tag[1], s.imm - a.tag[2], 16 - s.imm)
                    if bf is not None:
                        self.set_reg(full, bf)
                        return
                if m == "shl" and s.type == cx.X86_OP_IMM and s.imm == 2:
                    v = E("%s << 2" % a.p(), 4, tag=("shl2", a, 2))
                    self.set_reg(full, v)
                    return
                if m == "shl" and s.type == cx.X86_OP_IMM and a.size == 2:
                    v = E("%s << %d" % (a.p(), s.imm), 4, tag=("bfshl", a, s.imm))
                    self.set_reg(full, v)
                    return
                if m == "shr":
                    a = E("(unsigned)" + a.p(), 4)
                ptr_hint = m == "add" and s.type == cx.X86_OP_REG and \
                    (self.loaded_ptr(a) or self.loaded_ptr(b) or bool(loaded_ptr_k(a)) or
                     bool(loaded_ptr_k(b)))
                if ptr_hint:
                    # a dword read kept in a register for an add (not folded into it):
                    # perhaps a pointer, a choice point
                    self.choices.append(ins.address + 0.375)
                base_only = m == "add" and s.type == cx.X86_OP_REG and not ptr_hint and \
                    self.used_as_base(full) and not self.global_ptr(b) and \
                    not self.global_ptr(a)
                gidx = False
                if base_only:
                    # the sum is used as an address: pointer arithmetic, or an int sum (a
                    # choice point; an int sum by default when a global is the displacement:
                    # g[x][y])
                    self.choices.append(ins.address + 0.375)
                    gidx = self.base_of_global(full) and not os.environ.get("LIFT_NOGIDX")
                if m == "add" and s.type == cx.X86_OP_REG and (
                        (self.used_as_base(full) and not (
                            base_only and (ins.address + 0.375 in self.flips) != gidx)) or
                        self.global_ptr(b) or self.global_ptr(a) or
                        (ptr_hint and (ins.address + 0.375 in self.flips) ==
                         bool(os.environ.get("LIFT_NOPTRADD")))):
                    # pointer arithmetic: the operand loaded from memory is the base pointer
                    # (as char *, -od keeps it in a register: mov edx,[p]; add eax,edx)
                    pa, pb = (b, a) if self.loaded_ptr(b) and not self.loaded_ptr(a) else (a, b)
                    if not self.loaded_ptr(pa) and not loaded_ptr_k(pa) and loaded_ptr_k(pb):
                        pa, pb = pb, pa
                    if self.loaded_ptr(a) and self.loaded_ptr(b):
                        # both read from memory: which one is the pointer (a choice point)
                        self.choices.append(ins.address + 0.8125)
                        if ins.address + 0.8125 in self.flips:
                            pa, pb = pb, pa
                    ptxt = ("*(char **)" + pa.text[len("*(int *)"):]) if self.loaded_ptr(pa) \
                        else "*(char **)%s + %s" % loaded_ptr_k(pa) if loaded_ptr_k(pa) \
                        else "(char *)" + pa.p()
                    self.choices.append(ins.address)
                    if ins.address in self.flips:     # the offset written first
                        v = E("(int)(%s + %s)" % (pb.p(), ptxt), 4, atom=True)
                    else:
                        v = E("(int)(%s + %s)" % (ptxt, pb.p()), 4, atom=True,
                              tag=("padd", ptxt, pb.p()))
                    self.set_reg(full, v)
                    self.flags = ("val", v, None)
                    return
                if m in ("add", "sub") and s.type == cx.X86_OP_IMM and sz == 4 and \
                        self.loaded_ptr(a) and not os.environ.get("LIFT_NOPTRK"):
                    # a pointer read plus an offset, converted back to an int (the
                    # conversion costs a register move later): a choice point
                    self.choices.append(ins.address + 0.4375)
                    if ins.address + 0.4375 in self.flips:
                        v = E("(int)(*(char **)%s %s %d)" % (a.text[len("*(int *)"):],
                                                             "+" if m == "add" else "-",
                                                             s.imm), 4, atom=True)
                        self.set_reg(full, v)
                        self.flags = ("val", v, None)
                        return
                if m in ("add", "imul", "and", "or", "xor") and s.type == cx.X86_OP_REG and \
                        not CONST_RE.fullmatch(a.text) and not CONST_RE.fullmatch(b.text) and \
                        ("func_" not in a.text + b.text or
                         ("func_" in a.text and "func_" in b.text and
                          not os.environ.get("LIFT_NOCALLORDER"))):
                    a, b = self.operand_order(ins, a, b, k0)
                v = E("%s %s %s" % (a.p(), opch, b.p()), max(sz, 1))
                self.set_reg(full, v)
                self.flags = ("val", v, None)
                return
            lhs = self.mem(d, ins, d.size)
            b = self.src(s, ins)
            if lhs == "@RET":
                raise Unsupported("read-modify-write of the return slot")
            prev = self.body[self.k - 1] if self.k else None
            pre_r = None
            if prev is not None and prev.mnemonic == "mov" and prev.operands and \
                    prev.operands[0].type == cx.X86_OP_REG and prev.operands[0].size == 4 and \
                    prev.op_str.split(", ", 1)[1] == ins.op_str.split(", ")[0]:
                pre_r = subreg(prev.reg_name(prev.operands[0].reg))[0]
            if m in ("add", "sub") and s.type == cx.X86_OP_IMM and d.size == 4 and \
                    pre_r is not None and self.reg_used_later(
                        pre_r, calls=not os.environ.get("LIFT_NOPOSTARG")):
                # *p++-style: the old value, loaded just before, is used afterwards
                lv = lhs if re.fullmatch(r"\w+", lhs) else "(%s)" % lhs
                self.post_expr("(int)(*(char (**)[%d])&%s)%s" % (s.imm, lv, "++" if m == "add" else "--"),
                               pre_r)
                return
            if m in ("add", "sub") and s.type == cx.X86_OP_IMM and d.size == 4 and \
                    prev is not None and prev.mnemonic == "mov" and \
                    prev.op_str == "eax, " + ins.op_str.split(", ")[0] and 1 < s.imm < 0x10000:
                # mov eax,[p]; add [p],K: post-increment of a pointer to a K-byte object
                lv = lhs if re.fullmatch(r"\w+", lhs) else "(%s)" % lhs
                self.emit("(*(char (**)[%d])&%s)%s;" % (s.imm, lv, "++" if m == "add" else "--"))
                return
            mm = re.fullmatch(r"\*\(int \*\)&(l_[0-9A-F]+|a\d+)", lhs)
            if mm and m in ("add", "sub", "and", "or", "xor") and s.type == cx.X86_OP_REG and \
                    b.size == 2 and self.var_type(mm.group(1)) in ("short", "unsigned short"):
                # Watcom 10 updates a short variable with the whole register too
                lhs = mm.group(1)
            if m == "shr":
                # a logical shift in place: the variable is unsigned
                lhs = lhs.replace("*(short *)", "*(unsigned short *)", 1) \
                    .replace("*(signed char *)", "*(unsigned char *)", 1) \
                    .replace("*(int *)", "*(unsigned *)", 1)
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
                last = self.out[-1] if self.out else ""
                mm = re.fullmatch(r"    return (.*);", last)
                if mm and m in ("neg", "not", "inc", "dec"):
                    # the return variable changed in place right after being set
                    self.out[-1] = "    return %s;" % ({"neg": "-(%s)", "not": "~(%s)",
                                                         "inc": "(%s) + 1", "dec": "(%s) - 1"}[m]
                                                        % mm.group(1))
                    return
                raise Unsupported("read-modify-write of the return slot")
            lv = lhs if re.fullmatch(r"\w+", lhs) else "(%s)" % lhs
            prev = self.body[self.k - 1] if self.k else None
            if m in ("neg", "not") and prev is not None and prev.mnemonic == "mov" and \
                    prev.operands[0].type == cx.X86_OP_MEM and \
                    prev.op_str.split(", ")[0] == ins.op_str and self.out and \
                    self.out[-1].startswith("    %s = " % lhs) and \
                    not os.environ.get("LIFT_NOSTORENEG"):
                # mov [x],edx; neg [x] (the address not reloaded): x = -y, negated in place
                rhs = self.out[-1][len("    %s = " % lhs):-1]
                self.out[-1] = "    %s = %s(%s);" % (lhs, "-" if m == "neg" else "~", rhs)
                return
            if m in ("inc", "dec") and prev is not None and prev.mnemonic == "mov" and \
                    prev.operands[0].type == cx.X86_OP_REG and \
                    prev.op_str.split(", ", 1)[1] == ins.op_str and \
                    prev.operands[0].size == d.size:
                # x++ used as a value: the old value, loaded just before, is used afterwards
                full_r = subreg(prev.reg_name(prev.operands[0].reg))[0]
                if self.reg_used_later(full_r):
                    self.post_expr("%s%s" % (lv, "++" if m == "inc" else "--"), full_r, d.size)
                    return
            if m in ("inc", "dec") and self.k >= 2 and not os.environ.get("LIFT_NOPOSTFAR"):
                # mov dx,[eax+6]; mov eax,[p]; dec word [eax+6]: the old value loaded two
                # instructions back, the pointer reloaded in between
                p2 = self.body[self.k - 2]
                if p2.mnemonic == "mov" and p2.operands[0].type == cx.X86_OP_REG and \
                        p2.operands[1].type == cx.X86_OP_MEM and \
                        p2.operands[0].size == d.size and prev.mnemonic == "mov" and \
                        prev.operands[0].type == cx.X86_OP_REG and \
                        p2.op_str.split(", ", 1)[1] == ins.op_str:
                    full_r = subreg(p2.reg_name(p2.operands[0].reg))[0]
                    v_ = self.regs.get(full_r)
                    if v_ is not None and re.sub(r"^\(int\)\((?:unsigned )?short\)|^\(int\)\((?:signed |unsigned )?char\)", "", v_.text) \
                            .strip("()") in (lhs, lhs.strip("()")) and \
                            self.reg_used_later(full_r) and \
                            full_r != subreg(prev.reg_name(prev.operands[0].reg))[0]:
                        self.post_expr("%s%s" % (lv, "++" if m == "inc" else "--"), full_r, d.size)
                        return
            if m in ("inc", "dec") and not os.environ.get("LIFT_NOEXPRPRE") and \
                    any(r in self.regs and self.regs[r] is not self.pending and
                        r not in self.stale and self.live_later(r) for r in PARM_REGS) and \
                    self.k + 1 < len(self.body) and any(
                        o.type == cx.X86_OP_MEM and
                        self.body[self.k + 1].op_str.split(", ")[-1] == ins.op_str
                        for o in self.body[self.k + 1].operands[1:]):
                # in the middle of an expression (a value waits in a register) and read
                # right after: ++x / --x inside it
                if not hasattr(self, "pre_ops"):
                    self.pre_ops = {}
                self.pre_ops[lhs] = "++" if m == "inc" else "--"
                return
            dead_load = prev is not None and prev.mnemonic == "mov" and prev.operands and \
                prev.operands[0].type == cx.X86_OP_REG and \
                re.sub(r"^\w+ ptr ", "", prev.op_str.split(", ", 1)[1]) == \
                re.sub(r"^\w+ ptr ", "", ins.op_str)
            stack_param = d.type == cx.X86_OP_MEM and d.mem.base and \
                ins.reg_name(d.mem.base) == "ebp" and not d.mem.index and d.mem.disp >= 8
            if m in ("inc", "dec") and not dead_load and \
                    (ebp_slot(ins, d) is not None or stack_param) and \
                    not os.environ.get("LIFT_NOPREINC"):
                # no load of the old value first: Watcom 10 compiled a pre-increment (a
                # post-increment statement reads the old value: mov eax,[x]; inc [x])
                self.emit("%s%s;" % ("++" if m == "inc" else "--", lv))
                return
            self.emit({"inc": "%s++;", "dec": "%s--;", "neg": "%s = -%s;", "not": "%s = ~%s;"}[m]
                      % ((lv,) if m in ("inc", "dec") else (lhs, lv)))
            return
        if m in ("cmp", "test"):
            a = self.src(ops[0], ins)
            b = self.src(ops[1], ins)
            # a 16/8-bit register holding a wider value (a call's int result tested as
            # `test ax,ax`): the source narrowed it
            for n_, op_ in enumerate(ops):
                v_ = (a, b)[n_]
                if m == "test" and n_ == 0 and op_.type == cx.X86_OP_REG and op_.size == 1 and \
                        ops[1].type == cx.X86_OP_IMM and ops[1].imm == 0xFF and \
                        not os.environ.get("LIFT_NOTESTFF"):
                    continue    # test al,0xff on a wider value: (x & 255)
                if op_.type == cx.X86_OP_REG and op_.size < 4 and v_.size > op_.size and \
                        not os.environ.get("LIFT_NONARROW"):
                    v_ = E("(%s)%s" % (STYPE[op_.size], v_.p()), op_.size, atom=True)
                    if n_ == 0:
                        a = v_
                    else:
                        b = v_
            if m == "test" and ops[0].type == cx.X86_OP_MEM and ops[0].size < 4:
                # a bit test doesn't care about sign; unsigned keeps it `test byte ptr [x], K`
                a = E(a.text.replace("*(signed char *)", "*(unsigned char *)", 1)
                      .replace("*(short *)", "*(unsigned short *)", 1), a.size, a.atom)
            self.flags = (m, a, b)
            nx = self.body[self.k + 1] if self.k + 1 < len(self.body) else None
            if nx is not None and not nx.mnemonic.startswith(("j", "set", "adc", "sbb", "cmov")) \
                    and not os.environ.get("LIFT_NODEADCMP"):
                # a compare nothing branches on: an empty if (the jump to the next
                # instruction was dropped)
                self.emit("if (%s) {}" % self.cond_text("je"))
                self.flags = None
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
            pe = self.src(ops[0], ins)
            if ops[0].type == cx.X86_OP_IMM:
                pe = E(pe.text, pe.size, pe.atom, pe.tag)
                pe.imm_push = True
            self.pushes.append(pe)
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
            loaded = [r for r in PARM_REGS if r in self.regs and self.regs[r] is not self.pending
                      and r not in self.stale and not (
                          self.regs[r].tag and self.regs[r].tag[0] == "call" and
                          self.void_call(self.regs[r]))]
            sig = signature(op.imm) if op.imm in IMG.funcs and \
                IMG.le.obj_of_va(op.imm).index == 1 and op.imm < GAME_END else None
            pops = callee_pops(op.imm)
            # The callee's convention says how many register and stack arguments it takes; the
            # most recent pushes are its stack arguments, and other pushes and registers belong
            # to an enclosing call (f(g(x), 1, 2) evaluates f's later arguments first).
            if self.pushes and cleanup:
                nreg, nstack = 0, int(nxt.op_str.split(", ")[1], 16) // 4   # cdecl / varargs
                self.vcalls.add(name)
                self.skip_add = True
            elif sig is not None:
                nreg, nstack = min(4, len(sig[1])), max(0, len(sig[1]) - 4)
                self.calls.add(name)
                if any(t != "int" for t in sig[1]) and not os.environ.get("LIFT_NONOPROTO"):
                    # no prototype in scope at the call (arguments passed as ints): a
                    # choice point for the function's calls of it
                    key = self.first_call.setdefault(name, ins.address) + 0.03125
                    self.choices.append(key)
                    if key in self.flips:
                        self.noproto.add(name)
            elif pops and "eax" not in loaded:
                nreg, nstack = 0, pops // 4          # stack only, callee pops: #pragma aux
                self.scalls[name] = nstack
            elif pops:
                nreg, nstack = 4, pops // 4          # four registers, then the stack
                self.calls.add(name)
            else:
                nreg, nstack = 0, 0
                # at most as many register arguments as the callee leaves unsaved; without
                # that information, registers read after the call were kept across it
                limit = max_reg_args(op.imm)
                live = self.args_before_live(self.k)
                if live < limit:
                    # or fewer: registers read after the call may have been kept across it;
                    # a choice point at the call
                    self.choices.append(ins.address)
                    if ins.address in self.flips:
                        limit = live
                last = max((PARM_REGS.index(r) for r in loaded if r in PARM_REGS[:limit]),
                           default=-1)
                if last > 0 and self.reg_consumed(self.k, PARM_REGS[last]) and \
                        not os.environ.get("LIFT_NOCONSUMED"):
                    # the last argument register's value was already used (an index added
                    # into another register): not an argument (a choice point)
                    self.choices.append(ins.address + 0.6875)
                    if ins.address + 0.6875 not in self.flips:
                        limit = last
                if limit and "eax" in loaded and self.eax_consumed(self.k):
                    # eax's value was already used (copied into a byte register to pass on):
                    # perhaps no argument at all; a choice point
                    self.choices.append(ins.address + 0.875)
                    if ins.address + 0.875 in self.flips:
                        limit = 0
                if limit < 4:
                    # or more: a callee may save a register it takes as an argument
                    self.choices.append(ins.address + 0.25)
                    if ins.address + 0.25 in self.flips:
                        limit = 4
                for r in PARM_REGS[:limit]:
                    # a register still holding a copy of an earlier argument (mov edx,ebx)
                    # is left over, not another argument; a call's result still in eax is
                    # one (f(g(), x, y))
                    pend = r == "eax" and self.pending is not None and \
                        self.regs.get("eax") is self.pending and not self.void_call(self.pending)
                    if pend:
                        # an earlier call's result still in eax: an argument (f(g(), x)) or
                        # not (a call with no arguments after one returning a value)
                        self.choices.append(ins.address + 0.5)
                    # (by default an argument when later argument registers are loaded too:
                    # f(g(), x, y) evaluates x and y first)
                    pend_dflt = pend and any(q in loaded for q in PARM_REGS[1:limit]) and \
                        not os.environ.get("LIFT_NOPENDDFLT")
                    if (r in loaded or (pend and (ins.address + 0.5 in self.flips) != pend_dflt)) and \
                            not any(self.regs[r] is self.regs[q] for q in PARM_REGS[:nreg]):
                        nreg += 1
                    else:
                        break
                self.calls.add(name)
            if nstack > len(self.pushes):
                raise Unsupported("call needs %d stack arguments, %d pushed" % (nstack, len(self.pushes)))
            args = []
            far = [k for k in range(1, 4) if (self.regs.get(PARM_REGS[k]) is not None and
                                             (self.regs[PARM_REGS[k]].tag or ("",))[0] == "ds")]
            if far:
                # a far pointer argument: offset and selector in a pair of registers
                nreg = max(nreg, far[-1] + 1)
            k = 0
            while k < nreg:
                r = PARM_REGS[k]
                if r not in self.regs:
                    if k >= 1 and sig is not None and not self.pushes and \
                            not os.environ.get("LIFT_NOSHORTCALL"):
                        # fewer arguments than the callee's parameters: called without a
                        # prototype in scope
                        self.noproto.add(name)
                        nreg = k
                        break
                    raise Unsupported("call argument %s not loaded" % r)
                if self.regs[r] is self.pending:
                    self.pending = None              # nested call: f(g(x))
                if k + 1 in far:
                    args.append("(void __far *)(void *)%s" % self.regs[r].p())
                    self.farcalls.add(name)
                    k += 2
                    continue
                args.append(self.regs[r].text)
                k += 1
            if nstack:
                if sig is not None and not os.environ.get("LIFT_NOIMMNOPROTO") and any(
                        getattr(x, "imm_push", False) and 4 + k < len(sig[1]) and
                        sig[1][4 + k] in ("short", "unsigned short", "signed char",
                                          "unsigned char")
                        for k, x in enumerate(reversed(self.pushes[-nstack:]))):
                    # a narrow stack parameter pushed as an immediate: called without the
                    # prototype in scope
                    self.noproto.add(name)
                args += [x.text for x in reversed(self.pushes[-nstack:])]
                del self.pushes[-nstack:]
            self.finish_call(E("%s(%s)" % (name, ", ".join(args)), 4, atom=True,
                               tag=("call", name)), nreg)
            return
        if m.startswith("j"):
            tgt = ops[0].imm if ops[0].type == cx.X86_OP_IMM else None
            if tgt is None:
                return self.switch_dispatch(ins, ops[0])
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

    def base_of_global(self, reg):
        """Is `reg` next read as the base of a memory operand whose displacement is a
        global's address ([eax + g])?"""
        for ins in self.body[self.k + 1:]:
            for op in sorted(ins.operands, key=lambda o: o.type != cx.X86_OP_MEM):
                if op.type == cx.X86_OP_MEM and op.mem.base and \
                        SUB.get(ins.reg_name(op.mem.base), (None,))[0] == reg:
                    return ins.disp_size == 4 and \
                        fixup_at(ins, ins.disp_offset) is not None
                if op.type == cx.X86_OP_REG and \
                        SUB.get(ins.reg_name(op.reg), (None,))[0] == reg:
                    return False
            if ins.mnemonic in ("call", "ret") or ins.mnemonic.startswith("j"):
                return False
        return False

    def used_as_base(self, reg):
        """Is `reg` next read as the base of a memory operand (so it holds a pointer)?"""
        for ins in self.body[self.k + 1:]:
            # (a memory operand is read before the destination register is written:
            # mov ax,[eax+k])
            ops = sorted(ins.operands, key=lambda o: o.type != cx.X86_OP_MEM) \
                if not os.environ.get("LIFT_NOBASEFIRST") else ins.operands
            for op in ops:
                if op.type == cx.X86_OP_MEM and op.mem.base and \
                        SUB.get(ins.reg_name(op.mem.base), (None,))[0] == reg:
                    return True
                if op.type == cx.X86_OP_REG and \
                        SUB.get(ins.reg_name(op.reg), (None,))[0] == reg:
                    return False
            if ins.mnemonic in ("call", "ret") or ins.mnemonic.startswith("j"):
                return False
        return False

    @staticmethod
    def global_ptr(e):
        """A dword global read straight from memory: Watcom 10 never folds it into an add
        (`mov edx,[p]; add eax,edx`), which marks it as a pointer."""
        return e.atom and re.fullmatch(r"\*\(int \*\)D_[0-9A-F]{8}", e.text) is not None

    @staticmethod
    def loaded_ptr(e):
        """A dword read straight from memory: the likely pointer operand of an add."""
        return e.atom and e.text.startswith("*(int *)")

    def find_switches(self):
        """jmp cs:[idx*4 + table] / jmp cs:[reg + table]: the table's entries (from the fixups
        at consecutive dwords) and the bias (the displacement points 4k bytes before the
        table when the lowest case is k)."""
        found = {}
        self.default_at = {}
        self.default_targets = set()
        for ins in self.ins:
            if ins.mnemonic != "jmp" or not ins.operands or ins.operands[0].type != cx.X86_OP_MEM:
                continue
            disp = ins.operands[0].mem.disp & 0xFFFFFFFF
            tabs = [(t, n) for t, n in IMG.tables if disp <= t < disp + 4 * 64]
            if not tabs:
                continue
            t, n = min(tabs)
            entries = [IMG.fix_at[t + 4 * i].target_va for i in range(n // 4)
                       if t + 4 * i in IMG.fix_at]
            bias = (t - disp) // 4
            cases = {}
            for k, e in enumerate(entries):
                cases.setdefault(e, []).append(k + bias)
            found[ins.address] = {"table": t, "entries": entries, "bias": bias, "cases": cases}
        return found

    def find_cswitches(self):
        """Switches compiled as a compare tree: Watcom copies the selector into a temp (the
        whole register stored, then compared at the selector's width) and binary-searches the
        case values, possibly ending in jump tables. Returns {store address: switch}."""
        found = {}
        self.cskip = set()
        if os.environ.get("LIFT_NOCSWITCH"):
            return found
        by_addr = {i.address: i for i in self.ins}
        nxt = {self.ins[k].address: self.ins[k + 1].address for k in range(len(self.ins) - 1)}
        for k, i in enumerate(self.ins[:-1]):
            if i.mnemonic != "mov" or len(i.operands) != 2 or \
                    ebp_slot(i, i.operands[0]) is None or \
                    i.op_str.split(", ")[-1] not in ("eax", "ax", "al"):
                continue
            t = ebp_slot(i, i.operands[0])
            if t in [p[1] for p in self.params] or k < self.body_start:
                continue                # a parameter's spill, not a temp
            j = self.ins[k + 1]
            if j.mnemonic == "cmp" and ebp_slot(j, j.operands[0]) == t and \
                    j.operands[0].size <= i.operands[0].size and \
                    j.operands[1].type == cx.X86_OP_IMM:
                w = j.operands[0].size
            elif self.scan_at(j.address, t, by_addr, nxt) is not None:
                w = self.scan_at(j.address, t, by_addr, nxt)["width"]
            else:
                continue
            saved = set(self.cskip)
            try:
                sw = self.parse_ctree(j.address, t, w, by_addr, nxt)
            except Unsupported:
                # not a compare tree after all (a local compared, then used again)
                if any(x.address != i.address and x.address != j.address and
                       x.address not in self.cskip
                       for x in self.ins for op in x.operands if ebp_slot(x, op) == t) \
                        or w == 4:
                    self.cskip = saved
                    continue
                raise
            # an int selector looks like `l = f(); if (l < 0) ...` on a local used once: take
            # it for a switch only when it has a table or several cases
            if w == 4 and not sw["has_table"] and \
                    sum(len(v) for v in sw["cases"].values()) < 3:
                self.cskip = saved
                continue
            # a temp: nothing but the store and the tree touches the slot
            if any(x.address != i.address and x.address not in sw["nodes"]
                   for x in self.ins for op in x.operands if ebp_slot(x, op) == t):
                self.cskip = saved
                continue
            # a case before the dispatch (a jump back to a loop's continue): a chain of ifs
            if any(a < i.address for a in sw["cases"]) and not os.environ.get("LIFT_BACKCASE"):
                self.cskip = saved
                continue
            sw["store"] = i.address
            found[i.address] = sw
            self.temps.add(t)           # the compiler's own temp: not declared
        if not os.environ.get("LIFT_NOVARSWITCH"):
            self.find_var_switches(found, by_addr, nxt)
        return found

    def find_var_switches(self, found, by_addr, nxt):
        """`switch (v)` on an int variable: Watcom compares the variable itself, no temp. A
        compare tree on a variable is told from a chain of ifs by the binary search's
        repeated compare (`cmp v,3; jb; cmp v,3; jbe`)."""
        body = self.ins[self.body_start:self.body_end]
        inside = set()          # nodes of trees seen (a subtree is no switch of its own)
        for k, j in enumerate(body[:-3]):
            if j.mnemonic != "cmp" or j.address in self.cskip or j.address in inside or \
                    j.operands[1].type != cx.X86_OP_IMM or j.operands[0].size != 4:
                continue
            t = ebp_slot(j, j.operands[0])
            if t is None:
                op = j.operands[0]
                if not (op.type == cx.X86_OP_MEM and op.mem.base and
                        j.reg_name(op.mem.base) == "ebp" and not op.mem.index and
                        op.mem.disp >= 8):
                    continue
                t = -op.mem.disp
            pv = body[k - 1] if k else None
            if pv is not None and pv.mnemonic == "cmp" and pv.operands[0].type == cx.X86_OP_MEM \
                    and pv.op_str.split(",")[0] == j.op_str.split(",")[0]:
                continue
            # the repeated compare right at the root
            j2 = by_addr.get(nxt.get(j.address))
            j3 = by_addr.get(nxt.get(j2.address)) if j2 is not None else None
            if j2 is None or j2.mnemonic not in JCC or j3 is None or j3.mnemonic != "cmp" or \
                    j3.op_str.split(",")[0] != j.op_str.split(",")[0]:
                continue
            if j3.op_str != j.op_str:
                # (case ranges compare different constants: then the tree's dead `jmp
                # default` stubs say switch)
                run, x = [], j
                while x is not None and ((x.mnemonic == "cmp" and x.op_str.split(",")[0] ==
                                          j.op_str.split(",")[0]) or x.mnemonic in JCC or
                                         x.mnemonic == "jmp"):
                    run.append(x.mnemonic)
                    x = by_addr.get(nxt.get(x.address))
                if not any(a == b == "jmp" for a, b in zip(run, run[1:])) or \
                        os.environ.get("LIFT_NORANGESWITCH"):
                    continue
            # the tree: compares of the variable and jumps, up to the first other code
            end, x = j.address, j
            while x is not None and ((x.mnemonic == "cmp" and x.op_str.split(",")[0] ==
                                      j.op_str.split(",")[0] and
                                      x.operands[1].type == cx.X86_OP_IMM) or
                                     x.mnemonic in JCC or x.mnemonic == "jmp"):
                end = nxt.get(x.address)
                x = by_addr.get(end)
            saved = set(self.cskip)
            try:
                sw = self.parse_ctree(j.address, t, 4, by_addr, nxt, limit=end)
            except Unsupported:
                self.cskip = saved
                continue
            stubs = any(a == b == "jmp" for a, b in zip(
                [x.mnemonic for x in body[k:k + 40]], [x.mnemonic for x in body[k + 1:k + 41]]))
            if sum(len(v) for v in sw["cases"].values()) < (2 if stubs else 3) or \
                    sw["default"] is None:
                self.cskip = saved
                continue
            inside |= sw["nodes"]
            # or a chain of ifs after all (OW's tree differs): a choice point
            self.choices.append(j.address + 0.9375)
            if j.address + 0.9375 in self.flips:
                self.cskip = saved
                continue
            sw["store"] = None
            sw["var"] = j
            self.cskip.discard(j.address)
            found[j.address] = sw

    def scan_at(self, a, t, by_addr, nxt):
        """A scan switch's dispatch at `a`: `mov al, [t]; mov ecx, n+1; mov edi, offset
        values; repne scasb; jmp cs:[ecx*4 + labels]` (scasw/scasd for wider values). The
        values are stored largest first; a value found at index i leaves ecx = n - i, which
        picks its label; not found leaves 0, the default."""
        seq, b = [], a
        for _ in range(5):
            if b not in by_addr:
                return None
            seq.append(by_addr[b])
            b = nxt.get(b)
        ld, mc, me, sc, jp = seq
        if ld.mnemonic != "mov" or ebp_slot(ld, ld.operands[1]) != t or \
                ld.op_str.split(",")[0] not in ("al", "ax", "eax") or \
                mc.mnemonic != "mov" or not mc.op_str.startswith("ecx, ") or \
                me.mnemonic != "mov" or not me.op_str.startswith("edi, ") or \
                not sc.mnemonic.startswith("repne scas") or jp.mnemonic != "jmp" or \
                jp.address not in self.switches:
            return None
        width = {"b": 1, "w": 2, "d": 4}[sc.mnemonic[-1]]
        n = mc.operands[1].imm - 1
        vals_va = me.operands[1].imm
        labels = self.switches[jp.address]["entries"]
        if len(labels) != n + 1:
            raise Unsupported("scan switch label table size")
        o = IMG.le.obj_of_va(vals_va)
        raw = bytes(IMG.img[o.index][vals_va - o.base: vals_va - o.base + n * width])
        cases = {}
        for i in range(n):
            v = int.from_bytes(raw[i * width:(i + 1) * width], "little")
            cases[v] = labels[n - i]
        return {"width": width, "cases": cases, "other": labels[0], "jmp": jp.address,
                "values": vals_va, "seq": [x.address for x in seq]}

    def parse_ctree(self, start, t, w, by_addr, nxt, stubs=frozenset(), limit=None):
        """Walk the compare tree from `start`, tracking the selector's possible values.
        `stubs`: jumps a compare goes to that belong to the tree (an empty sub-range's
        `jmp default`, emitted right after the tree's own jumps)."""
        def node(a, fall=True):
            """('cmp', v, [(jcc, target)], fallthrough) / ('jmp', target) / ('table', jmp)
            / None for a leaf."""
            i = by_addr.get(a)
            if i is None:
                return None
            if limit is not None and not start <= a < limit:
                return None             # a variable's tree ends where other code starts
            if i.mnemonic == "cmp" and frame_slot(i, i.operands[0]) == t and \
                    i.operands[1].type == cx.X86_OP_IMM:
                js, b = [], nxt.get(a)
                while b in by_addr and by_addr[b].mnemonic in JCC:
                    js.append((by_addr[b].mnemonic, by_addr[b].operands[0].imm))
                    b = nxt.get(b)
                if not js:
                    raise Unsupported("switch compare without a jump")
                return ("cmp", i.operands[1].imm, js, b, [a] + [x for x in self.span(a, b, nxt)])
            # the tree's own jumps follow its compares; a jump a compare goes to is a leaf
            # (a case body's `break`)
            if i.mnemonic == "jmp" and i.operands[0].type == cx.X86_OP_IMM and a != start and \
                    (fall or a in stubs):
                return ("jmp", i.operands[0].imm, [a])
            sc = self.scan_at(a, t, by_addr, nxt)
            if sc is not None:
                return ("scan", sc, sc["seq"])
            if i.mnemonic == "xor" and i.op_str == "eax, eax":
                b = nxt.get(a)
                seq = [a]
                while b in by_addr and len(seq) < 5:
                    seq.append(b)
                    if by_addr[b].mnemonic == "jmp":
                        if b in self.switches:
                            ld = by_addr[seq[1]]
                            if ld.mnemonic != "mov" or ebp_slot(ld, ld.operands[1]) != t:
                                raise Unsupported("switch table on another value")
                            return ("table", b, seq)
                        break
                    b = nxt.get(b)
            return None

        bits = 8 * w
        mask = (1 << bits) - 1
        # signedness from the jumps used anywhere in the tree
        signed, todo, seen = False, [(start, True)], set()
        while todo:
            a, fall = todo.pop()
            if (a, fall) in seen or len(seen) > 4000:
                continue
            seen.add((a, fall))
            n = node(a, fall)
            if n is None:
                continue
            if n[0] == "cmp":
                signed |= any(m in ("jl", "jle", "jg", "jge") for m, _ in n[2])
                todo += [(x, False) for _, x in n[2]] + [(n[3], True)]
            elif n[0] == "jmp":
                todo.append((n[1], False))
        lo, hi = (-(1 << (bits - 1)), (1 << (bits - 1)) - 1) if signed else (0, mask)

        def val(imm):
            v = imm & mask
            return v - (1 << bits) if signed and v >> (bits - 1) else v

        def cut(ivs, op, v):
            yes, no = [], []
            for a, b in ivs:
                for (x, y), into in (((a, min(b, v - 1)), "lt"), ((max(a, v), min(b, v)), "eq"),
                                     ((max(a, v + 1), b), "gt")):
                    if x > y:
                        continue
                    ok = {"==": into == "eq", "!=": into != "eq", "<": into == "lt",
                          "<=": into != "gt", ">": into == "gt", ">=": into != "lt"}[op]
                    (yes if ok else no).append((x, y))
            return yes, no

        leaves, nodes = [], set()
        todo = [(start, [(lo, hi)], True)]
        while todo:
            a, ivs, fall = todo.pop()
            if not ivs:
                continue
            if len(nodes) > 4000:
                raise Unsupported("switch compare tree too large")
            n = node(a, fall)
            if n is None:
                leaves.append((a, ivs))
                continue
            nodes.update(n[-1])
            if n[0] == "cmp":
                v = val(n[1])
                for m, x in n[2]:
                    yes, ivs = cut(ivs, JCC[m][0], v)
                    todo.append((x, yes, False))
                todo.append((n[3], ivs, True))
            elif n[0] == "jmp":
                todo.append((n[1], ivs, False))
            elif n[0] == "scan":
                sc = n[1]
                tsw = self.switches[sc["jmp"]]
                tsw["merged"] = True
                tsw["scan_values"] = sc["values"]
                for v in sorted(sc["cases"]):
                    if any(x <= v <= y for x, y in ivs):
                        leaves.append((sc["cases"][v], [(v, v)]))
                        _yes, ivs = cut(ivs, "==", v)
                if ivs:
                    leaves.append((sc["other"], ivs))
            else:
                tsw = self.switches[n[1]]
                tsw["merged"] = True
                for x, y in ivs:
                    for v in range(x, y + 1):
                        k = v - tsw["bias"]
                        if not 0 <= k < len(tsw["entries"]):
                            raise Unsupported("switch table index out of range")
                        leaves.append((tsw["entries"][k], [(v, v)]))
        # the default takes every value that isn't a case: the target with the most values
        count = {}
        for a, ivs in leaves:
            count[a] = count.get(a, 0) + sum(y - x + 1 for x, y in ivs)
        dflt = max(count, key=count.get)
        cases = {}
        for a, ivs in leaves:
            if a == dflt:
                continue
            for x, y in ivs:
                if y - x > 1024:
                    raise Unsupported("switch case range too large")
                cases.setdefault(a, []).extend(range(x, y + 1))
        for a in cases:
            cases[a].sort()
        prev = {b: a for a, b in nxt.items()}
        # jumps a compare goes to inside the tree's own stretch of code are the tree's stubs
        # (`jb L; ... L: jmp case`), not case bodies
        hi = max(nodes) if nodes else start
        more = {a for a, _ in leaves if a not in stubs and
                (prev.get(a) in nodes or start < a < max(hi, limit or 0)) and
                by_addr[a].mnemonic == "jmp" and by_addr[a].operands[0].type == cx.X86_OP_IMM}
        if more and len(stubs) < 64:
            return self.parse_ctree(start, t, w, by_addr, nxt, stubs | more, limit)
        self.cskip |= nodes
        return {"cases": cases, "default": dflt, "width": w, "signed": signed,
                "has_table": any(n is not None and n[0] in ("table", "scan")
                                 for n in (node(a) for a in nodes)),
                "entries": sorted(cases), "nodes": nodes}

    def span(self, a, b, nxt):
        """Addresses after `a` up to (not including) `b`."""
        out, x = [], nxt.get(a)
        while x is not None and x != b:
            out.append(x)
            x = nxt.get(x)
        return out

    @staticmethod
    def switch_targets(sw):
        return [x for x in list(sw["cases"]) + [sw.get("default")] if isinstance(x, int)]

    def close_switch(self, at):
        """End the innermost switch's body before `at`."""
        sw = self.sw_stack.pop()
        if at is not None and (sw.get("default") == "END" or
                               any(x > at for x in self.switch_targets(sw))):
            raise Unsupported("switch targets interleave")
        self.flush_pending()
        self.out.append("}")

    def open_switch(self, sw, text, at):
        """Start a switch: inside the current one when that has case targets still ahead
        (a switch in a case body), else after it."""
        if any(x < at for x in sw["cases"]):
            raise Unsupported("switch case before the dispatch")
        while self.sw_stack and self.sw_stack[-1].get("default") != "END" and \
                not any(x > at for x in self.switch_targets(self.sw_stack[-1])):
            self.close_switch(at)
        self.emit("switch (%s) {" % text)
        self.sw_stack.append(sw)
        self.regs = {}
        self.born_hint = None

    def switch_owning(self, a):
        """The open switch with a case (or its default) at `a`, closing those inside it."""
        own = [k for k, sw in enumerate(self.sw_stack) if a in sw["cases"] or sw.get("default") == a]
        if not own:
            return None
        k = own[0]
        # an inner switch whose default is here ends here: falling out of it is the same
        if any(a in self.sw_stack[j]["cases"] for j in own[1:]):
            raise Unsupported("case label shared by nested switches")
        while len(self.sw_stack) > k + 1:
            if self.sw_stack[-1].get("default") == a:
                self.sw_stack[-1]["default"] = None
            self.close_switch(a)
        return self.sw_stack[k]

    def cswitch_start(self, ins, sw):
        """The store into the selector temp: `switch (expr) {`."""
        w, signed = sw["width"], sw["signed"]
        if sw.get("var") is not None:
            v = self.mem(ins.operands[0], ins, 4)
            e = E(v if signed else "(unsigned)" + v, 4, atom=signed)
        else:
            e = self.reg("eax", ins)
        tname = {1: "char", 2: "short", 4: "int"}[w]
        mm = re.fullmatch(r"\*\((?:signed |unsigned )?(?:char|short) \*\)(.*)", e.text)
        if w == 4:
            text = e.text if e.size == 4 else "(int)%s" % e.p()
        elif mm and e.size == w and e.atom:
            text = "*(%s%s *)%s" % ("" if signed else "unsigned ",
                                    "signed char" if signed and w == 1 else tname, mm.group(1))
        else:
            text = "(%s%s)%s" % ("" if signed else "unsigned ",
                                 "signed char" if signed and w == 1 else tname, e.p())
        d = sw["default"]
        if d in (self.ret_ins, self.epi):
            sw["default"] = "END"
            self.default_targets.add("END")
        elif not ins.address < d:
            raise Unsupported("switch default before the dispatch")
        self.flush_pending()
        self.open_switch(sw, text, ins.address)

    def switch_dispatch(self, ins, op):
        sw = self.switches.get(ins.address)
        if sw is None:
            raise Unsupported("indirect jump without a table")
        m = op.mem
        if m.index and m.scale == 4:
            sel = self.reg(subreg(ins.reg_name(m.index))[0], ins)
        elif m.base and not m.index:
            v = self.reg(subreg(ins.reg_name(m.base))[0], ins)
            if not (v.tag and v.tag[0] == "shl2"):
                raise Unsupported("switch index not scaled")
            sel = v.tag[1]
        else:
            raise Unsupported("switch addressing")
        # the range check just before is the switch's own: `if (sel > max) goto Ldefault;`
        dflt = None
        for i in range(len(self.out) - 1, -1, -1):
            mm = re.fullmatch(r"    if \((.*)\) goto L([0-9A-F]+);", self.out[i])
            if mm:
                dflt = int(mm.group(2), 16)
                del self.out[i]
                break
            if re.fullmatch(r"    if \((.*)\) return;", self.out[i]):
                dflt = "END"            # out of range: straight to the end of the function
                del self.out[i]
                break
            if not self.out[i].startswith("    "):
                break
        if dflt is None:
            raise Unsupported("switch without a range check")
        sw["default"] = dflt
        if dflt == "END":
            self.default_targets.add(dflt)
        elif not (ins.address < dflt):
            raise Unsupported("switch default before the dispatch")
        text = re.sub(r"^\(int\)\((?:unsigned|signed) (?:char|short)\)", "", sel.text) \
            if sel.atom or sel.text.startswith("(int)(") else sel.text
        text = self.switch_temp(text)
        self.open_switch(sw, text, ins.address)
        return

    def switch_temp(self, text):
        """`l = expr; switch (l)` where l is used for nothing else: that slot is the compiler's
        own selector temp (a switch on an expression), so switch on the expression."""
        mm = re.fullmatch(r"(?:\*\([\w ]+ \*\)&)?l_([0-9A-F]+)", text)
        if not mm or os.environ.get("LIFT_NOSWTEMP"):
            return text
        off = int(mm.group(1), 16)
        last = self.out[-1] if self.out else ""
        ma = re.fullmatch(r"    l_%X = (.*);" % off, last)
        if not ma:
            return text
        uses = sum(1 for i in self.ins for op in i.operands
                   if ebp_slot(i, op) == off or local_indexed(i, op) == off)
        if uses != 3:                   # the store, the range check and the dispatch load
            return text
        # the temp is a whole register stored, compared at the selector's narrower width
        sizes = [op.size for i in self.ins for op in i.operands if ebp_slot(i, op) == off]
        if not (sizes and sizes[0] == 4 and min(sizes) < 4):
            return text
        self.out.pop()
        self.temps.add(off)
        return ma.group(1)

    def var_type(self, name):
        """Declared type of a parameter or local by name."""
        if name.startswith("l_"):
            return self.slot_type.get(int(name[2:], 16), "int")
        k = int(name[1:]) - 1
        if k < len(self.params):
            return self.slot_type.get(self.params[k][1], "int")
        k -= self.stack_base - 1
        return self.stack_type[k] if 0 <= k < len(self.stack_type) else "int"

    def args_before_live(self, k):
        """How many of edx/ebx/ecx (in argument order) can be arguments of the call at
        body[k]: one read after the call before being written was kept across it (Watcom
        callees preserve non-argument registers), so it and the later ones are not."""
        for n, r in enumerate(PARM_REGS[1:]):
            if self.read_after(k, r):
                return n + 1
        return 4

    def read_after(self, k, reg):
        for ins in self.body[k + 1:]:
            if ins.address in self.targets:
                return False
            reads, writes = ins.regs_access()
            if any(SUB.get(ins.reg_name(x), (None,))[0] == reg for x in reads):
                return True
            if any(SUB.get(ins.reg_name(x), (None,))[0] == reg for x in writes):
                return False
            if ins.mnemonic in ("call", "ret", "jmp") or ins.mnemonic.startswith("j"):
                return False
        return False

    @staticmethod
    def void_call(e):
        """Is `e` a call of a void game function?"""
        if not (e.tag and e.tag[0] == "call"):
            return False
        va = int(e.tag[1][5:], 16) if e.tag[1].startswith("func_") else None
        sig = signature(va) if va is not None and va in IMG.funcs and va < GAME_END else None
        return sig is not None and sig[0] == "void"

    def live_later(self, reg):
        """Is `reg` read later before being written, across calls (callees keep it)?"""
        for ins in self.body[self.k + 1:]:
            if ins.address in self.targets or ins.mnemonic == "ret" or \
                    ins.mnemonic.startswith("j"):
                return False
            reads, writes = ins.regs_access()
            if any(SUB.get(ins.reg_name(x), (None,))[0] == reg for x in reads):
                return True
            if any(SUB.get(ins.reg_name(x), (None,))[0] == reg for x in writes):
                return False
            if ins.mnemonic == "call" and reg in ("eax",):
                return False
        return False

    def eax_used_later(self):
        """Is eax read by a later instruction before being written (statement-local)?"""
        return self.reg_used_later("eax")

    def reg_used_later(self, reg, calls=False):
        for ins in self.body[self.k + 1:]:
            if calls and ins.mnemonic == "call" and reg in PARM_REGS:
                return True         # (an argument of the call)
            if ins.address in self.targets or ins.mnemonic in ("call", "ret") or \
                    ins.mnemonic.startswith("j"):
                return False
            reads, writes = ins.regs_access()
            if any(SUB.get(ins.reg_name(r), (None,))[0] == reg for r in reads):
                return True
            if any(SUB.get(ins.reg_name(r), (None,))[0] == reg for r in writes):
                return False
        return False

    def post_expr(self, text, reg="eax", size=4):
        """A post-increment whose old value `reg` carries into the next instructions."""
        e = E(text, size, atom=True)
        e.post = True
        pv = self.pending
        held = [r for r, v in self.regs.items() if v is pv and r != reg]
        if not (pv is not None and getattr(pv, "post", False) and held and
                self.reg_used_later(held[0]) and not os.environ.get("LIFT_NOPOSTPAIR")):
            # (an earlier post-increment's old value still waiting in a register for a
            # later use stays there: *q++ = *p++)
            self.flush_pending()
        self.regs[reg] = e
        self.stale.discard(reg)
        self.pending = e

    def finish_call(self, call, nargs):
        """After a call: eax holds the result; callee-saved registers that were not
        arguments keep their values (Watcom callees preserve everything but eax)."""
        self.flush_pending()
        keep = {r: v for r, v in self.regs.items()
                if r != "eax" and r not in PARM_REGS[:nargs] and v is not self.pending}
        keep["eax"] = call
        self.regs = keep
        self.pending = call

    def reg_consumed(self, k, reg):
        """Whether the value in a 32-bit register at instruction k was read since it was
        computed (as an operand, not an address)."""
        fam = {reg, reg[1:], reg[1] + "l", reg[1] + "h"}
        for j in range(k - 1, -1, -1):
            i = self.body[j]
            if i.mnemonic == "call" or i.mnemonic.startswith("j"):
                return False
            ops = i.operands
            if i.mnemonic in ("xor", "sub") and len(ops) == 2 and \
                    ops[0].type == cx.X86_OP_REG and ops[1].type == cx.X86_OP_REG and \
                    ops[0].reg == ops[1].reg and i.reg_name(ops[0].reg) in fam:
                return False        # zeroed: a fresh value
            for n, o in enumerate(ops):
                if n > 0 and o.type == cx.X86_OP_REG and i.reg_name(o.reg) in fam:
                    return True
                if o.type == cx.X86_OP_MEM and any(
                        x and i.reg_name(x) == reg for x in (o.mem.base, o.mem.index)):
                    return True     # (used as an address)
            if ops and ops[0].type == cx.X86_OP_REG and i.reg_name(ops[0].reg) in fam and \
                    i.mnemonic in ("mov", "movsx", "movzx", "lea", "xor"):
                return False
        return False

    def eax_consumed(self, k):
        """Whether the value in eax at instruction k was read since it was computed."""
        fam = {"eax", "ax", "al", "ah"}
        for j in range(k - 1, -1, -1):
            i = self.body[j]
            if i.mnemonic in ("call",) or i.mnemonic.startswith("j"):
                return False
            ops = i.operands
            srcs = ops[1:] if i.mnemonic in ("mov", "movsx", "movzx", "lea") else ops
            for n, o in enumerate(ops):
                if o.type == cx.X86_OP_REG and i.reg_name(o.reg) in fam and \
                        o in srcs and n > 0:
                    return True
                if o.type == cx.X86_OP_MEM and any(
                        x and i.reg_name(x) == "eax" for x in (o.mem.base, o.mem.index)):
                    return True
            if ops and ops[0].type == cx.X86_OP_REG and i.reg_name(ops[0].reg) in fam and \
                    i.mnemonic in ("mov", "movsx", "movzx", "lea", "xor") and \
                    not (i.mnemonic == "xor" and i.reg_name(ops[0].reg) == "ah"):
                return False
        return False

    def indirect_call(self, ins, op):
        """call [mem] / call reg: through a function pointer, arguments in registers."""
        if op.type == cx.X86_OP_MEM:
            target = self.mem(op, ins, 4)
            if target == "@RET":
                raise Unsupported("call through the return slot")
        else:
            target = self.reg(subreg(ins.reg_name(op.reg))[0], ins).p()
        args = []
        if op.type == cx.X86_OP_MEM:
            used = {ins.reg_name(x) for x in (op.mem.base, op.mem.index) if x}
        else:
            used = {ins.reg_name(op.reg)}
        for r in PARM_REGS:
            if r in used and not self.pushes:
                # the register holds (part of) the function's address: no more arguments,
                # or it is one too (a choice point)
                self.choices.append(ins.address + 0.25)
                if ins.address + 0.25 not in self.flips:
                    break
            if r in self.regs and self.regs[r] is not self.pending:
                args.append(self.regs[r].text)
            else:
                break
        proto = ""
        if self.pushes:
            # four register arguments, then the stack (the callee pops it)
            if len(args) != 4:
                raise Unsupported("indirect call with stack arguments")
            args += [x.text for x in reversed(self.pushes)]
            self.pushes = []
            proto = ", ".join(["int"] * len(args))
        call = E("((int (*)(%s))%s)(%s)" % (proto, target if target.startswith("(") else
                                             "(%s)" % target, ", ".join(args)), 4, atom=True)
        self.finish_call(call, len(args))

    def decl_line(self, o, two):
        if o in self.arrays:
            return "    char l_%X[%d];" % (o, self.arrays[o])
        return "    %s l_%X;" % (self.slot_type[o], o)

    # ---- output ------------------------------------------------------------------------
    def ternaries(self):
        """`if (c) goto A; t = x; goto B; A: t = y; B: ... t ...` with t a slot used only
        there is `c ? y : x`, t the compiler's temp. Arguments of a call with a ternary
        argument are evaluated into temps first (right to left), so single-use slots
        stored just before go back into the call too."""
        out = self.out
        body = "\n".join(out)

        def uses(v):
            return len(re.findall(r"\b%s\b" % v, body))

        def gotos(lab):
            return len(re.findall(r"goto %s;" % lab, body))

        # a compound condition (a chain of `if (c) goto` to the two arms) first becomes
        # one line carrying the then-condition (put back if no ternary comes of it)
        chains = {}
        i = 0
        while not os.environ.get("LIFT_NOCHAINTERN") and i < len(out) - 5:
            i += 1
            if True:
                mt = re.fullmatch(r"(L[0-9A-F]+):;", out[i])
                m1 = re.fullmatch(r"    (l_[0-9A-F]+) = (.*);", out[i + 1])
                m2 = re.fullmatch(r"    goto (L[0-9A-F]+);", out[i + 2])
                me = re.fullmatch(r"(L[0-9A-F]+):;", out[i + 3])
                if not (mt and m1 and m2 and me) or \
                        not re.fullmatch(r"    %s = (.*);" % m1.group(1), out[i + 4]) or \
                        out[i + 5] != m2.group(1) + ":;":
                    continue
                lt, le = mt.group(1), me.group(1)
                k = i
                # `Lx:; goto Le;` just above the then-arm: the chain's fall-through goes to
                # the else-arm (`if (a) goto Lx; if (b) goto Lt; Lx: goto Le;`)
                lx = None
                if k > 2 and out[k - 1] == "    goto %s;" % le and \
                        re.fullmatch(r"L[0-9A-F]+:;", out[k - 2]) and \
                        not os.environ.get("LIFT_NOTRAMPTERN"):
                    lx = out[k - 2][:-2]
                    k -= 2
                raw = []
                while k > 0:
                    mc = re.fullmatch(r"    if \((.*)\) goto (L[0-9A-F]+);", out[k - 1])
                    if not mc or mc.group(2) not in (lt, le, lx):
                        break
                    raw.insert(0, (mc.group(1), mc.group(2)))
                    k -= 1
                if len(raw) < 2 or gotos(lt) != sum(lab == lt for _, lab in raw) or \
                        gotos(le) != sum(lab == le for _, lab in raw) + (lx is not None) or \
                        (lx is not None and (raw[-1][1] != lt or
                                             gotos(lx) != sum(lab == lx for _, lab in raw))):
                    continue
                terms = [(c, le if lab == lx else lab) for c, lab in raw]
                if lx is not None:
                    terms[-1] = ("@NEG:" + terms[-1][0], le)
                if not any(lab == le for _, lab in terms):
                    continue

                def then(j):
                    if j == len(terms):
                        return None
                    c, lab = terms[j]
                    rest = then(j + 1)
                    if lab == lt:
                        return None if rest is None else "(%s) || (%s)" % (c, rest)
                    nc = c[5:] if c.startswith("@NEG:") else negate(c)
                    return nc if rest is None else "%s && (%s)" % (nc, rest)
                cond = then(0)
                if cond is None:
                    continue
                line = "    if (@THEN:%s) goto %s;" % (cond, le)
                chains[line] = out[k:i + 1]
                out[k:i + 1] = [line]
                body = "\n".join(out)

        # comma stores folded into a diamond's condition (an argument spilled while the
        # condition is evaluated) come out as statements before it
        for i in range(len(out)):
            if not out[i].startswith("    if (") or ", " not in out[i]:
                continue
            line, pre = out[i], []
            while True:
                mm = re.search(r"\((l_[0-9A-F]+) = ", line)
                if not mm:
                    break
                k, depth = mm.end(), 0
                while k < len(line) and not (depth == 0 and line[k] == ","):
                    depth += line[k] == "("
                    depth -= line[k] == ")"
                    if depth < 0:
                        break
                    k += 1
                if k >= len(line) or line[k] != ",":
                    break
                # the closing paren of the comma expression
                e, depth = k + 1, 0
                while e < len(line) and not (depth == 0 and line[e] == ")"):
                    depth += line[e] == "("
                    depth -= line[e] == ")"
                    e += 1
                pre.append("    %s = %s;" % (mm.group(1), line[mm.end():k]))
                line = line[:mm.start()] + "(" + line[k + 2:e] + ")" + line[e + 1:]
            if pre and all(uses(re.match(r"    (l_[0-9A-F]+)", p_).group(1)) == 2 for p_ in pre):
                out[i:i + 1] = pre + [line]
                body = "\n".join(out)

        # a compound assignment through the address temp the code spilled
        # (`*(T *)t = (*(T *)(t = p + k, t)) | x`) is `*(T *)(p + k) |= x`
        rmw = set()
        for i in range(len(out)):
            mm = re.fullmatch(r"    \*\((short|signed char) \*\)\(\(char \*\)(l_[0-9A-F]+)\) = "
                              r"\(\(int\)\((unsigned short|short|unsigned char)\)\*\(\1 \*\)"
                              r"\(\(char \*\)\(\2 = (l_[0-9A-F]+|a\d+) \+ (\d+), \2\)\)\) "
                              r"([|&^+-]) (.*);", out[i])
            if not mm or uses(mm.group(2)) != 3 or os.environ.get("LIFT_NORMWTEMP"):
                continue
            ty = {"unsigned short": "unsigned short", "short": "short",
                  "unsigned char": "unsigned char"}[mm.group(3)]
            out[i] = "    *(%s *)((char *)%s + %s) %s= %s;" % (
                ty, mm.group(4), mm.group(5), mm.group(6), mm.group(7))
            rmw.add(out[i])
            self.temps.add(int(mm.group(2)[2:], 16))
            body = "\n".join(out)

        changed = True
        while changed:
            changed = False
            body = "\n".join(out)
            for i in range(len(out) - 6):
                m0 = re.fullmatch(r"    if \((.*)\) goto (L[0-9A-F]+);", out[i])
                m1 = re.fullmatch(r"    (l_[0-9A-F]+) = (.*);", out[i + 1])
                m2 = re.fullmatch(r"    goto (L[0-9A-F]+);", out[i + 2])
                if not (m0 and m1 and m2) or out[i + 3] != m0.group(2) + ":;":
                    continue
                t = m1.group(1)
                m4 = re.fullmatch(r"    %s = (.*);" % t, out[i + 4])
                if not m4 or out[i + 5] != m2.group(1) + ":;" or \
                        gotos(m0.group(2)) != 1 or gotos(m2.group(1)) != 1 or \
                        uses(t) != 3 or len(re.findall(r"\b%s\b" % t, out[i + 6])) != 1 or \
                        "&" + t in out[i + 6] or \
                        re.match(r"\w+:;|case |default:", out[i + 6]):
                    continue
                off = int(t[2:], 16)
                if off in self.arrays or self.slot_type.get(off) != "int":
                    continue
                cnd = m0.group(1)
                cnd = "(%s)" % cnd[len("@THEN:"):] if cnd.startswith("@THEN:") else negate(cnd)
                tern = "%s ? %s : %s" % (cnd, m1.group(2), m4.group(1))
                use = re.sub(r"\b%s\b" % t, lambda _m: "(%s)" % tern, out[i + 6])
                self.temps.add(off)
                # the call's later arguments, stored into temps just before
                j = i
                # (only for a call statement whose argument the ternary is)
                call = re.match(r"    (?:[\w*()& ]+ = )?func_[0-9A-F]{8}\(", out[i + 6])
                if call and not re.search(r"\b%s\b" % t, out[i + 6][call.end():]):
                    call = None
                while j > 0 and call:
                    mm = re.fullmatch(r"    (l_[0-9A-F]+) = (.*);", out[j - 1])
                    if not mm or uses(mm.group(1)) != 2 or "&" + mm.group(1) in use or \
                            not re.search(r"\b%s\b" % mm.group(1), use[call.end():]) or \
                            len(re.findall(r"\b%s\b" % mm.group(1), use)) != 1 or \
                            self.slot_type.get(int(mm.group(1)[2:], 16)) != "int" or \
                            int(mm.group(1)[2:], 16) in self.arrays:
                        break
                    use = re.sub(r"\b%s\b" % mm.group(1), lambda _m: mm.group(2), use)
                    self.temps.add(int(mm.group(1)[2:], 16))
                    j -= 1
                deepest = max([o for o in self.slot_type if o > 0 and o not in self.temps
                               and o != off] + [0])
                if j == i and out[i + 6] not in rmw and \
                        not (off in self.nested and off > deepest and
                             not os.environ.get("LIFT_NOTEMPTERN")):
                    # no argument spilled: an if/else on a variable of its own, as likely
                    # (unless the slot is in the compiler's temp region, below every
                    # declared variable)
                    self.temps.discard(off)
                    continue
                out[j:i + 7] = [use]
                changed = True
                break
        self._restore_chains(chains)

    def _restore_chains(self, chains):
        """Put back the compound conditions no ternary came of (end of the last pass)."""
        self._chains_pending = chains

    def bitfield_stores(self):
        """`p->f = v` on a bit-field: Watcom clears the field (narrowed to the byte holding
        it: `and byte [p+7],0xbf`), then ors in `(v & m) << s` on the whole unit. The two
        compound assignments the lift gives become the one bit-field store."""
        if os.environ.get("LIFT_NOBFSTORE"):
            return
        size = {"signed char": 1, "unsigned char": 1, "short": 2, "unsigned short": 2,
                "int": 4}

        def addr(t):
            mm = re.fullmatch(r"\(\(char \*\)(.+) \+ (\d+)\)", t)
            if mm:
                return mm.group(1), int(mm.group(2))
            mm = re.fullmatch(r"\(\(char \*\)(.+)\)", t)
            return (mm.group(1), 0) if mm else (None, None)
        out = self.out
        i = 0
        while i < len(out) - 1:
            ma = re.fullmatch(r"    \*\((signed char|unsigned char|short|unsigned short|int) "
                              r"\*\)(.+) &= (\d+);", out[i])
            mb = re.fullmatch(r"    \*\((signed char|unsigned char|short|unsigned short|int) "
                              r"\*\)(.+) \|= (.+);", out[i + 1])
            i += 1
            if not (ma and mb):
                continue
            ba, oa = addr(ma.group(2))
            bb, ob = addr(mb.group(2))
            if ba is None or ba != bb:
                continue
            wa, wb = size[ma.group(1)], size[mb.group(1)]
            if not (ob <= oa and oa + wa <= ob + wb):
                continue
            cleared = (~int(ma.group(3)) & ((1 << 8 * wa) - 1)) << 8 * (oa - ob)
            mv = re.fullmatch(r"\((.+) & (\d+)\) << (\d+)", mb.group(3))
            if mv:
                v, m, sh = mv.group(1), int(mv.group(2)), int(mv.group(3))
            else:
                mv = re.fullmatch(r"(.+) & (\d+)", mb.group(3))
                if not mv:
                    continue
                v, m, sh = mv.group(1), int(mv.group(2)), 0
            if m & (m + 1) or cleared != m << sh:
                continue
            length = m.bit_length()
            t = {1: "unsigned char", 2: "unsigned short", 4: "unsigned"}[wb]
            tag = "bf%d_%d_%d" % (8 * wb, sh, length)
            pad = "%s _:%d; " % (t, sh) if sh else ""
            self.structs.add("struct %s { %s%s f:%d; };" % (tag, pad, t, length))
            if v.startswith("(") and v.endswith(")") and v.count("(") == 1:
                v = v[1:-1]
            out[i - 1:i + 1] = ["    ((struct %s *)%s)->f = %s;" % (tag, mb.group(2), v)]

    def c(self):
        self.lift()
        self.bitfield_stores()
        if not os.environ.get("LIFT_NOTERNARY"):
            self.ternaries()
            chains = getattr(self, "_chains_pending", {})
            for n in range(len(self.out) - 1, -1, -1):
                if self.out[n] in chains:
                    self.out[n:n + 1] = chains[self.out[n]]
        name = sym(self.va)
        ps = []
        for k, (reg, off, _sz) in enumerate(self.params):
            ps.append("%s a%d" % (self.slot_type.get(off, "int"), k + 1))
        for k in range(self.nstack):
            ps.append("%s a%d" % (self.stack_type[k], self.stack_base + k))
        # The slot rule (docs/progress.md) gives slots top down: 2-byte locals, the return
        # variable, other locals last to first; so declare locals deepest first.
        locals_ = sorted((o for o in set(self.slot_type) | set(self.arrays)
                          if o not in [p[1] for p in self.params] and o not in self.temps),
                         reverse=True)
        two = [o for o in locals_ if o in self.slot_type and self.size_of[self.slot_type[o]] == 2]
        rest = [o for o in locals_ if o not in two]
        if two and rest and max(two) > min(rest) and \
                not os.environ.get("LIFT_NOSHORTPIN"):
            # a 2-byte local below a 4-byte one: the slot rule puts 2-byte locals on top, so
            # only pinned slots give this layout
            self.must_pin = True
        lines = ["%s %s(%s)" % ("void" if self.void else self.ret_type, name, ", ".join(ps) or "void"),
                 "{"]
        # Locals below every parameter were declared in a nested block: Watcom gives a
        # block's locals their slots when the block starts, after the function's own.
        nested = [o for o in rest + two if o in self.nested]
        if not nested and self.cswitches and not os.environ.get("LIFT_NOTEMPNEST"):
            # locals deeper than a switch's selector temp got their slots after the switch
            # was reached: a block opened inside it (a choice point)
            sw_temps = []
            for sw in self.cswitches.values():
                st = sw.get("store")
                if st is not None:
                    si = next((x for x in self.ins if x.address == st), None)
                    if si is not None and ebp_slot(si, si.operands[0]) is not None:
                        sw_temps.append(ebp_slot(si, si.operands[0]))
            if sw_temps:
                deeper = [o for o in rest + two if o > min(sw_temps)]
                if deeper:
                    self.choices.append(-2000)
                    if -2000 not in self.flips:
                        nested = deeper
        outer = [o for o in rest + two if o not in nested]
        used = set(re.findall(r"goto L([0-9A-F]+);", "\n".join(self.out)))
        body = []
        for line in self.out:
            mm = re.fullmatch(r"L([0-9A-F]+):;", line)
            if mm and mm.group(1) not in used:
                continue
            body.append(line)
        if not self.void and "%X" % self.ret_ins in used:
            body.append("L%X:;" % self.ret_ins)
        # A switch temp among the nested locals splits them: those above it were in a block
        # opened before the switch, those below it in one opened inside it.
        early = []
        if nested and self.temps and not os.environ.get("LIFT_NOSPLIT"):
            inside = [t for t in self.temps if min(nested) < t < max(nested)]
            if inside:
                cut = min(inside)
                early = [o for o in nested if o < cut]
                nested = [o for o in nested if o > cut]
        nested_decls = [self.decl_line(o, two) for o in nested]
        # Where the nested block starts: at the top, unless a switch's selector temp (which
        # gets its slot when the switch is reached) sits above the block's locals: then the
        # block began after that switch, at the first statement using its locals.
        at, depth = None, 0
        if nested and self.cswitches and not os.environ.get("LIFT_NOLATEBLOCK"):
            names = re.compile(r"\b(?:%s)\b" % "|".join("l_%X" % o for o in nested))
            d, seen_switch = 0, False
            for k, line in enumerate(body):
                if names.search(line):
                    if seen_switch:
                        at, depth = k, d
                    break
                if line.startswith("    switch (") and line.endswith("{"):
                    d += 1
                    seen_switch = True
                elif line == "}":
                    d -= 1
        closers = []
        for sw in reversed(self.sw_stack):
            closers.append((["default:;"] if sw.get("default") == "END" else []) + ["}"])
        decl_lines = [self.decl_line(o, two) for o in outer]
        if early:
            decl_lines += ["{"] + [self.decl_line(o, two) for o in early]
        if nested and at is None:
            decl_lines += ["{"] + nested_decls
        lines += decl_lines
        if locals_:
            lines.append("")
        if at is None:
            lines += body
            for c in closers:
                lines += c
            if nested:
                lines.append("}")
        else:
            # open before the statement (after its labels), close with the enclosing switch
            k = at
            d, end = depth, None
            for j in range(at, len(body)):
                if body[j].startswith("    switch (") and body[j].endswith("{"):
                    d += 1
                elif body[j] == "}":
                    d -= 1
                    if d < depth:
                        end = j
                        break
            stop = end if end is not None else len(body)
            if any(names.search(x) for x in body[stop:]):
                raise Unsupported("nested-block locals used after their block")
            lines += body[:k] + ["{"] + nested_decls + body[k:stop]
            if end is not None:
                lines += ["}"] + body[stop:]
                for c in closers:
                    lines += c
            else:
                # the enclosing switch is still open at the end: close the block inside it
                inner = len(closers) - depth       # closers run innermost first
                for n, c in enumerate(closers):
                    if n == inner:
                        lines.append("}")
                    lines += c
                if inner >= len(closers):
                    lines.append("}")
        if early:
            lines.append("}")
        lines.append("}")
        decl = ["/* lifted from 0x%08X */" % self.va] + sorted(self.structs)
        # last resort, a function-level choice: pin every variable at its frame depth
        self.choices.append(PIN_SLOTS)
        if (PIN_SLOTS in self.flips) != getattr(self, "force_pin", False) or \
                getattr(self, "must_pin", False):
            base = 4 * len(self.saved)
            pins = []
            for k, (_r, off, _sz) in enumerate(self.params):
                pins.append(("a%d" % (k + 1), off - base))
            for o in locals_:
                pins.append(("l_%X" % o, o - base))
            if self.ret_slot:
                pins.append(("ret", self.ret_slot - base))
            if pins:
                decl.append("#pragma dagger slots %s %s" % (
                    name, " ".join("%s %d" % (n, d) for n, d in pins)))
        # register pins (#pragma dagger reg), set by the search at register-only
        # differences: flips -(3000 + 16 k + r)
        wins = sorted(win_decode(f) for f in self.flips if f <= -200000)
        if wins:
            decl.append("#pragma dagger confwin %s %s" % (
                name, " ".join("%d %d" % (a, b) for a, b in wins)))
        pins = sorted(pin_decode(f) for f in self.flips if -200000 < f <= -3000)
        if pins:
            decl.append("#pragma dagger reg %s %s" % (
                name, " ".join("%d %s" % (k, r) for k, r in pins)))
        # function-level choice points: code generator options (#pragma dagger), and the
        # default spelling of p->arr[i] (field offset with the pointer, or after the sum)
        self.choices.append(FIELD_LAST)
        self.choices.extend(INVERT_KINDS)
        self.choices.extend(-1 - k for k in range(len(FUNC_OPTS)))
        for k, opt in enumerate(FUNC_OPTS):
            if -1 - k in self.flips:
                decl.append("#pragma dagger %s %s" % (opt, name))
        if self.conv == "sosconv":
            decl.append(SOSCONV)
            decl.append("#pragma aux (sosconv) %s;" % name)
        for g in sorted(self.globals):
            decl.append("extern char %s[];" % g)
        for f in sorted(self.calls - self.vcalls - set(self.scalls) - {name}):
            d = globals()["decl"](f)
            if f in self.used_results and d.startswith("extern void "):
                # a void function whose eax the caller uses: no prototype was in scope
                # there (implicit int)
                d = "extern int %s();" % f
            elif f in self.noproto or f in self.farcalls:
                d = re.sub(r"\(.*\);$", "();", d)
            decl.append(d)
        if getattr(self, "uses_fpseg", False):
            decl.append("extern unsigned short _FP_SEG( const volatile void __far * );")
            decl.append("#pragma aux _FP_SEG = parm caller [eax dx] value [dx] modify exact [];")
        for f, n in sorted(self.fpcalls.items()):
            decl.append("#pragma aux %s parm routine [] value [8087];" % f)
            decl.append("extern double %s(%s);" % (f, ", ".join(["double"] * n) or "void"))
        for f, n in sorted(self.scalls.items()):
            decl.append("#pragma aux %s parm routine [];" % f)
            decl.append("extern int %s(%s);" % (f, ", ".join(["int"] * n)))
        for f in sorted(self.vcalls):
            decl.append("extern int %s(int, ...);" % f)
        return "\n".join(decl + [""] + lines) + "\n"


SIGS = {}
POPS = {}


# Code generator switches the batch search may turn on for one function
# (`#pragma dagger <SWITCH> <function>`, DaggerEnv() in the compiler)
PIN_SLOTS = -1000         # the choice point for `#pragma dagger slots`
FIELD_LAST = -9999        # p->arr[i] spelt (char *)(p + i*k) + off by default
PIN_REGS = ["eax", "ebx", "ecx", "edx", "esi", "edi", "ax", "bx", "cx", "dx", "si", "di"]


def pin_encode(k, reg):
    """The flip for `#pragma dagger reg <f> k reg`."""
    return -(3000 + 16 * k + PIN_REGS.index(reg))


def win_encode(k1, k2):
    """The flip for `#pragma dagger confwin <f> k1 k2`."""
    return -(200000 + 64 * k1 + (k2 - k1))


def win_decode(f):
    n = -f - 200000
    return n // 64, n // 64 + n % 64


def pin_decode(f):
    n = -f - 3000
    return n // 16, PIN_REGS[n % 16]
FUNC_OPTS = ["KKND_CONFREV", "DAGGER_LEFTPREF", "DAGGER_CHARAUTOSMALL", "DAGGER_CLRAFTER",
             "DAGGER_WORDSTORE", "DAGGER_RMW", "DAGGER_PUSHMEM", "DAGGER_DEADDEF", "DAGGER_CDQ",
             "DAGGER_FLUSH", "DAGGER_CHARPARMBIG", "DAGGER_SIGNEDBF", "KKND_CONSTREG",
             "KKND_LINSEL", "KKND_NOROT", "KKND_STRETCH", "DAGGER_FIRSTUSE",
             "DAGGER_NOSAVES", "DAGGER_REGLAST", "DAGGER_NOGIVEN", "DAGGER_CONFLIST",
             "DAGGER_CONFLISTREV", "DAGGER_KEEPSUB", "DAGGER_NODEMOTE",
             "DAGGER_NOCVTDEMOTE", "DAGGER_DEADDEFMEM", "DAGGER_CONFPOS", "DAGGER_CONFPOSREV", "DAGGER_RIGHTPREF",
             "DAGGER_IDXKEEP", "DAGGER_SEXTCONST"]
IMPLICIT = bool(os.environ.get("LIFT_IMPLICIT"))


def loaded_ptr_k(e):
    """`*(int *)p + k`: a dword read plus a constant, perhaps a pointer and an offset."""
    mm = re.fullmatch(r"\*\(int \*\)(\w+|\((?:[^()]|\([^()]*\))*\)) \+ (\d+)", e.text)
    return (mm.group(1), mm.group(2)) if mm else None


def split_sum(text):
    """`A + B` at the top level of an expression: (A, B), else None."""
    depth = 0
    for k, ch in enumerate(text):
        depth += ch == "("
        depth -= ch == ")"
        if depth == 0 and text.startswith(" + ", k):
            return text[:k], text[k + 3:]
    return None


def negate(c):
    """The opposite of a condition: its top-level comparison flipped, else !(c)."""
    depth, ops = 0, []
    for k, ch in enumerate(c):
        depth += ch == "("
        depth -= ch == ")"
        if depth == 0:
            for op in ("==", "!=", "<=", ">=", "<", ">"):
                if c.startswith(op, k) and not (op in ("<", ">") and c[k + 1:k + 2] in "<>=") \
                        and not (op in ("<", ">") and k and c[k - 1] in "<>"):
                    ops.append((k, op))
                    break
    if len(ops) != 1 or "&&" in c or "||" in c:
        return "!(%s)" % c
    k, op = ops[0]
    flip = {"==": "!=", "!=": "==", "<": ">=", ">=": "<", ">": "<=", "<=": ">"}[op]
    return "(%s%s%s)" % (c[:k], flip, c[k + len(op):])


def frame_slot(ins, op):
    """A local's slot (ebp_slot) or a stack parameter's (-disp)."""
    off = ebp_slot(ins, op)
    if off is None and op.type == cx.X86_OP_MEM and op.mem.base and \
            ins.reg_name(op.mem.base) == "ebp" and not op.mem.index and op.mem.disp >= 8:
        off = -op.mem.disp
    return off


def byteval(v):
    """A value about to be narrowed to a byte: `and al,k` on a wider one is just `x & k`."""
    if v.tag and v.tag[0] == "and8":
        return E("%s & %d" % (v.tag[1].p(), v.tag[2]), 4)
    return v


def ext_text(t, v, implicit=None):
    """A narrow value widened to int: an explicit (int) cast, or (LIFT_IMPLICIT) left to the
    usual promotions."""
    if IMPLICIT if implicit is None else implicit:
        mm = re.fullmatch(r"\*\((?:unsigned |signed )?(?:char|short) \*\)(.+)", v.text)
        if mm and v.atom:
            return "*(%s *)%s" % (t, mm.group(1))
        return "(%s)%s" % (t, v.p())
    return "(int)(%s)%s" % (t, v.p())


SOSCONV = '#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];'
SAVES = {}


def max_reg_args(va):
    """At most how many register arguments a callee takes: Watcom callees save every
    register they use that isn't a parameter, so a pushed edx means at most one."""
    if va not in SAVES:
        n = 4
        if va in IMG.funcs and IMG.le.obj_of_va(va).index == 1:   # Watcom code, not asm
            pushed = set()
            for i in IMG.insns(va)[:8]:
                if i.mnemonic == "push" and i.operands[0].type == cx.X86_OP_REG:
                    pushed.add(i.reg_name(i.operands[0].reg))
                elif i.mnemonic == "mov" and i.op_str == "ebp, esp":
                    continue
                else:
                    break
            for k, r in enumerate(PARM_REGS[1:]):
                if r in pushed:
                    n = k + 1
                    break
        try:
            ins_ = IMG.insns(va)
        except Exception:
            ins_ = []
        if True:
            library = va >= GAME_END or not (va in IMG.funcs and
                                             IMG.le.obj_of_va(va).index == 1)
            if ins_ and (ins_[0].mnemonic != "push" or library) and len(ins_) < 40 and \
                    not any(i.mnemonic == "call" for i in ins_) and \
                    not os.environ.get("LIFT_NOASMARGS"):
                # a hand-written leaf: its arguments are the registers it reads before
                # writing them
                read, written = set(), set()
                for i in ins_:
                    if i.mnemonic == "ret":
                        break
                    if i.mnemonic in ("push", "pop"):
                        continue        # (saving a register is not reading it)
                    rd, wr = i.regs_access()
                    if i.mnemonic in ("xor", "sub") and len(i.operands) == 2 and \
                            i.operands[0].type == cx.X86_OP_REG and \
                            i.operands[1].type == cx.X86_OP_REG and \
                            i.operands[0].reg == i.operands[1].reg:
                        rd = []             # (zeroing)
                    for r_ in rd:
                        full = SUB.get(i.reg_name(r_), (None,))[0]
                        if full in PARM_REGS and full not in written:
                            read.add(full)
                    for r_ in wr:
                        full = SUB.get(i.reg_name(r_), (None,))[0]
                        if full:
                            written.add(full)
                n = max([PARM_REGS.index(r_) + 1 for r_ in read] + [0])
        SAVES[va] = n
    return SAVES[va]


def callee_pops(va):
    """Bytes of stack arguments a callee pops (its `ret N`), else 0. Whether it also takes
    register arguments is decided at the call site."""
    if va not in POPS:
        n = 0
        if va in IMG.funcs:
            rets = [i for i in IMG.insns(va) if i.mnemonic == "ret" and i.op_str]
            if rets and all(i.op_str == rets[0].op_str for i in rets):
                n = int(rets[0].op_str, 16)
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
            ret = "void" if f.void else f.ret_type
            # a return statement decides void-ness during lifting; the epilogue tells us now
            SIGS[va] = (ret, f.sig_types + f.stack_type)
        except Unsupported:
            SIGS[va] = None
    return SIGS[va]


def decl(name):
    va = int(name[5:], 16)
    sig = signature(va) if va in IMG.funcs else None
    if sig is None:
        return "extern int %s();" % name
    return "extern %s %s(%s);" % (sig[0], name, ", ".join(sig[1]) or "void")


CALLER_TYPES = None


def caller_types():
    """{callee va: [type or None per register parameter]} from how every call site loads its
    arguments: a caller converts each argument to the parameter's declared type, so
    `movsx edx, word ptr [x]` before the call means a short second parameter."""
    global CALLER_TYPES
    if CALLER_TYPES is not None:
        return CALLER_TYPES
    path = os.path.join(ROOT, "build", "lift", "caller_types.json")
    if os.path.exists(path) and os.path.getmtime(path) >= os.path.getmtime(__file__):
        with open(path) as f:
            CALLER_TYPES = {int(k): v for k, v in json.load(f).items()}
        return CALLER_TYPES
    ev = {}
    for va in IMG.funcs:
        if not (0x10000 <= va < GAME_END) or IMG.le.obj_of_va(va).index != 1:
            continue
        try:
            ins = IMG.insns(va)
        except Exception:
            continue
        for k, c in enumerate(ins):
            if c.mnemonic != "call" or c.operands[0].type != cx.X86_OP_IMM:
                continue
            tgt = c.operands[0].imm
            for n, r in enumerate(PARM_REGS):
                t = arg_type(ins, k, r)
                if t is not None:
                    ev.setdefault(tgt, {}).setdefault(n, set()).add(t)
            # stack arguments, nearest push first: `push imm` is an int (a narrow one is
            # pushed through a register)
            n = 4
            for j in range(k - 1, max(-1, k - 96), -1):
                i = ins[j]
                if i.mnemonic in ("call", "ret") or i.mnemonic.startswith("j"):
                    break
                if i.mnemonic == "push":
                    if i.operands[0].type == cx.X86_OP_REG and j and \
                            ins[j - 1].mnemonic == "cwde" and i.op_str == "eax" and \
                            not os.environ.get("LIFT_NOSHORT16"):
                        # widened from 16 bits just before the push: a short parameter
                        ev.setdefault(tgt, {}).setdefault(n, set()).add("short16")
                    if i.operands[0].type == cx.X86_OP_IMM:
                        ev.setdefault(tgt, {}).setdefault(n, set()).add("int")
                    elif i.operands[0].type == cx.X86_OP_REG and j and \
                            ins[j - 1].mnemonic == "mov" and \
                            ins[j - 1].op_str.startswith(i.op_str + ", 0x") and \
                            ins[j - 1].operands[1].imm & 0x80000000:
                        # a negative constant pushed through a register: a signed narrow one
                        ev.setdefault(tgt, {}).setdefault(n, set()).add("signed")
                    elif i.operands[0].type == cx.X86_OP_REG and j and \
                            ins[j - 1].mnemonic == "mov" and \
                            ins[j - 1].op_str.startswith(i.op_str + ", ") and \
                            ins[j - 1].operands[1].type == cx.X86_OP_IMM and \
                            ins[j - 1].operands[1].imm < 0x8000 and \
                            not os.environ.get("LIFT_NONARROWPUSH"):
                        # a constant pushed through a register: a narrow one
                        ev.setdefault(tgt, {}).setdefault(n, set()).add("narrow")
                    n += 1
    out = {}
    for tgt, params in ev.items():
        types = []
        for n in range(max([4] + [m + 1 for m in params])):
            seen = params.get(n, set()) - {"const"}
            if "short16" in seen and n >= 4 and not seen & {"int"}:
                types.append("short16")
                continue
            seen -= {"short16"}
            if seen == {"signed"} or seen == {"signed", "narrow"}:
                types.append("signed")
                continue
            if seen == {"narrow"}:
                types.append("narrow")
                continue
            if "narrow" in seen and seen - {"signed", "narrow"} == {"int"} and \
                    not os.environ.get("LIFT_NOMIXNARROW"):
                # some callers push it through a register, others as an immediate (called
                # without a prototype): narrow
                types.append("signed" if "signed" in seen else "narrow")
                continue
            seen -= {"signed", "narrow"}
            if not seen:
                types.append(None)
            elif "int" in seen or len(seen) > 1:
                types.append("int")
            else:
                types.append(seen.pop())
        out[tgt] = types
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        json.dump({str(k): v for k, v in out.items()}, f)
    CALLER_TYPES = out
    return out


GLOBAL_SIGN = None


RET_USES = None


def ret_uses():
    """{callee va: set of 'short' / 'int'}: how callers use the returned eax. `cwde` (or
    movsx from ax) right after the call: a short function; eax used whole: an int one."""
    global RET_USES
    if RET_USES is not None:
        return RET_USES
    path = os.path.join(ROOT, "build", "lift", "ret_uses.json")
    if os.path.exists(path) and os.path.getmtime(path) >= os.path.getmtime(__file__):
        with open(path) as f:
            RET_USES = {int(k): set(v) for k, v in json.load(f).items()}
        return RET_USES
    out = {}
    for va in IMG.funcs:
        if not (0x10000 <= va < GAME_END) or IMG.le.obj_of_va(va).index != 1:
            continue
        try:
            ins = IMG.insns(va)
        except Exception:
            continue
        for k, c in enumerate(ins[:-1]):
            if c.mnemonic != "call" or c.operands[0].type != cx.X86_OP_IMM:
                continue
            n = ins[k + 1]
            if n.mnemonic == "cwde" or (n.mnemonic == "movsx" and n.op_str.endswith(", ax")):
                out.setdefault(c.operands[0].imm, set()).add("short")
            elif re.search(r"\beax\b", n.op_str) and not (
                    n.mnemonic in ("mov", "movsx", "movzx", "lea") and n.op_str.startswith("eax,")):
                out.setdefault(c.operands[0].imm, set()).add("int")
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        json.dump({str(k): sorted(v) for k, v in out.items()}, f)
    RET_USES = out
    return out


def unsigned_globals():
    """Dword globals compared unsigned (ja/jb...) and never signed anywhere in the game: they
    were declared unsigned, which changes code beyond the compares."""
    global GLOBAL_SIGN
    if GLOBAL_SIGN is not None:
        return GLOBAL_SIGN
    path = os.path.join(ROOT, "build", "lift", "global_sign.json")
    if os.path.exists(path) and os.path.getmtime(path) >= os.path.getmtime(__file__):
        with open(path) as f:
            GLOBAL_SIGN = set(json.load(f))
        return GLOBAL_SIGN
    ev = {}
    for va in IMG.funcs:
        if not (0x10000 <= va < GAME_END) or IMG.le.obj_of_va(va).index != 1:
            continue
        try:
            ins = IMG.insns(va)
        except Exception:
            continue
        for k in range(1, len(ins) - 1):
            c, j = ins[k], ins[k + 1]
            if c.mnemonic != "cmp" or j.mnemonic not in JCC or j.mnemonic in ("je", "jne"):
                continue
            kind = "u" if JCC[j.mnemonic][1] else "s"
            gs = []
            for n, op in enumerate(c.operands):
                if op.type == cx.X86_OP_MEM and op.size == 4 and not op.mem.base and \
                        not op.mem.index:
                    fx = fixup_at(c, c.disp_offset)
                    if fx is not None:
                        gs.append(fx.target_va)
                elif op.type == cx.X86_OP_REG and op.size == 4:
                    p_ = ins[k - 1]
                    if p_.mnemonic == "mov" and p_.operands[0].type == cx.X86_OP_REG and \
                            p_.operands[0].reg == op.reg and \
                            p_.operands[1].type == cx.X86_OP_MEM and \
                            not p_.operands[1].mem.base and not p_.operands[1].mem.index:
                        fx = fixup_at(p_, p_.disp_offset)
                        if fx is not None:
                            gs.append(fx.target_va)
            for g in gs:
                ev.setdefault(g, set()).add(kind)
    out = sorted(g for g, kinds in ev.items() if kinds == {"u"})
    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, "w") as f:
        json.dump(out, f)
    GLOBAL_SIGN = set(out)
    return GLOBAL_SIGN


def arg_type(ins, k, reg):
    """How the instructions before the call at ins[k] last set `reg` (None: not seen)."""
    for j in range(k - 1, max(-1, k - 14), -1):
        i = ins[j]
        if i.mnemonic in ("call", "ret") or i.mnemonic.startswith("j"):
            return None
        if not i.operands or i.operands[0].type != cx.X86_OP_REG:
            continue
        name = i.reg_name(i.operands[0].reg)
        full, sz = SUB.get(name, (None, 0))
        if full != reg or i.mnemonic in ("cmp", "test", "push"):
            continue
        m = i.mnemonic
        if m == "movsx":
            return {1: "signed char", 2: "short"}[i.operands[1].size]
        if m == "movzx":
            return {1: "unsigned char", 2: "unsigned short"}[i.operands[1].size]
        if m == "cwde":
            return "short"
        if m == "and" and i.operands[1].type == cx.X86_OP_IMM and sz == 4:
            return {0xFF: "unsigned char", 0xFFFF: "unsigned short"}.get(i.operands[1].imm, "int")
        if m == "mov" and sz < 4:
            prev = ins[j - 1] if j else None
            if prev is not None and prev.mnemonic == "xor" and \
                    prev.op_str == "%s, %s" % (reg, reg):
                return {1: "unsigned char", 2: "unsigned short"}[sz]
            return None
        if m == "mov" and i.operands[1].type == cx.X86_OP_IMM:
            return "const"
        if sz == 4:
            return "int"
        return None
    return None


def init():
    global IMG
    if IMG is None:
        IMG = Image()
    return IMG


class InvertedFlips(frozenset):
    """Flips with one kind of choice point (by its fractional part) taken the other way by
    default: for trying a different default (LIFT_INVERT=0.375)."""
    def __new__(cls, base, fracs):
        o = super().__new__(cls, base)
        o.fracs = fracs
        return o

    def __contains__(self, x):
        hit = frozenset.__contains__(self, x)
        if isinstance(x, float) and x >= 0 and \
                any(abs((x - int(x)) - f) < 1e-9 for f in self.fracs):
            return not hit
        return hit

    def __or__(self, other):
        return InvertedFlips(frozenset(self) | frozenset(other), self.fracs)


# function-level choices that take every choice point of one kind the other way by default
INVERT_KINDS = {-9100: 0.4375, -9200: 0.8125, -9300: 0.75}


def lift(va, flips=frozenset(), info=None):
    """C for the function at `va`. `flips`: choice points (instruction addresses) to take
    the other way; `info`, a dict, receives the list of choice points as info["choices"]."""
    init()
    f = Func(va)
    inv = os.environ.get("LIFT_INVERT")
    fracs = [float(inv)] if inv else []
    fracs += [fr for k, fr in INVERT_KINDS.items() if k in flips]
    f.flips = InvertedFlips(flips, fracs) if fracs else flips
    try:
        return f.c()
    finally:
        if info is not None:
            info["choices"] = list(f.choices)
            info["sites"] = dict(f.choice_sites)
            try:
                info["where"] = f.body[f.k].address
            except Exception:
                pass


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
