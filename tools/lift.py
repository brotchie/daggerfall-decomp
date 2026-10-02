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
        self.structs = set()     # bit-field struct declarations used
        self.choices = []        # instruction addresses of operand-order choice points
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
        afirst = {}    # address-taken slot -> address of its first access
        sign = {}      # slot -> 's' / 'u' hints
        addr = set()
        body = self.ins[self.body_start:self.body_end]
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
                first.setdefault(off, ins.address)
                if n == 0 and ins.mnemonic == "mov" and op.size == 4 and k and \
                        ins.operands[1].type == cx.X86_OP_REG:
                    # stored whole right after being computed as 16 bits (mov ax,[x];
                    # mov [l],eax): Watcom 10's store of a 2-byte variable
                    pv = body[k - 1]
                    r16 = {"eax": "ax", "edx": "dx", "ebx": "bx", "ecx": "cx"}.get(
                        ins.reg_name(ins.operands[1].reg))
                    if pv.operands and pv.operands[0].type == cx.X86_OP_REG and \
                            pv.reg_name(pv.operands[0].reg) == r16:
                        wide16.add(off)
                if not (n == 0 and ins.mnemonic == "mov"):
                    reads.setdefault(off, set()).add(op.size)
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
            if off in wide16 and reads.get(off, set()) <= {2, 4} and 4 in reads.get(off, set()):
                # a 2-byte variable read whole, or an int: a choice point
                self.choices.append(first[off] + 0.0625)
            if off in wide16 and reads.get(off, set()) <= {2, 4} and \
                    (4 not in reads.get(off, set()) or first[off] + 0.0625 in self.flips):
                sz = 2
                if any(ins.mnemonic == "and" and ins.op_str.endswith("0xffff")
                       for ins in body):
                    sign.setdefault(off, set()).add("u")
            # Watcom 10 stores a short with the whole register: read only as a word, it is one
            if reads.get(off) == {2} and sizes <= {2, 4} and \
                    not os.environ.get("LIFT_NOSHORTREAD"):
                # ... or an int read through (short) casts: a choice point
                if 4 in sizes and off not in [p[1] for p in self.params]:
                    self.choices.append(first[off])
                if first.get(off) not in self.flips or off in [p[1] for p in self.params]:
                    sz = 2
            if off in addr:
                sz = 4
            hint = sign.get(off, set())
            if sz not in (1, 2, 4):
                raise Unsupported("%d-byte stack slot (floating point)" % sz)
            t = (UTYPE if hint == {"u"} or (sz == 1 and hint != {"s"}) else STYPE)[sz]
            self.slot_type[off] = t
        self.size_of = {"signed char": 1, "unsigned char": 1, "short": 2,
                        "unsigned short": 2, "int": 4, "unsigned": 4}
        # stack parameters ([ebp+8], [ebp+12], ...): a narrow one makes callers push through a
        # register (`mov eax,0x9c; push eax`)
        self.stack_type = [self.slot_type.pop(-(8 + 4 * k), "int") for k in range(self.nstack)]
        cts = caller_types().get(self.va) if not os.environ.get("LIFT_NOCALLERTYPES") else None
        if cts and self.conv is None:
            for k in range(self.nstack):
                if 4 + k < len(cts) and cts[4 + k] == "int":
                    self.stack_type[k] = "int"      # callers push it as an immediate
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
                parts.append(self.reg(base, ins).p())
        elif base and not index and disp and self.reg(base, ins).tag and \
                self.reg(base, ins).tag[0] == "padd" and \
                (ins.address + 0.125 in self.flips) != bool(os.environ.get("LIFT_FIELDFIRST")):
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
            if self.side:
                # the stores of an enclosing expression come first: (x = a, x <<= 2, x)
                text = "(%s, %s)" % (", ".join(self.side), t)
                self.side = []
                return E(text, op.size, atom=True)
            ch = getattr(self, "chain", None)
            if ch is not None and ch[0] == t and ch[2] == len(self.out) - 1 and \
                    ch[3] + 1 == getattr(self, "k", -1) and self.pending is None and \
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
            if ins.address in self.cswitches:
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
        if kind == "cmp":
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
                if sz == 1 and cur is not None and cur.tag == ("hiclr",):
                    # xor dh,dh; mov dl,[x]: a byte zero-extended to 16 bits
                    self.set_reg(full, E("(unsigned short)(unsigned char)" + v.p(), 2))
                elif sz < 4 and cur is not None and cur.text == "0":
                    self.set_reg(full, E(ext_text(UTYPE[sz], v), 4, atom=IMPLICIT))
                elif sz < 4:
                    self.set_reg(full, E(v.text, sz, v.atom))
                else:
                    self.set_reg(full, v)
                return
            v = self.src(s, ins)
            lhs = self.mem(d, ins, d.size)
            mm = re.fullmatch(r"\*\(int \*\)&(l_[0-9A-F]+|a\d+)", lhs)
            if mm and d.size == 4 and v.size == 2 and \
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
            if (held or self.pushes) and ebp_slot(ins, d) is not None and \
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
        if m == "cdq":
            v = self.reg("eax", ins)
            self.set_reg("edx", E("%s >> 31" % v.p(), 4, tag=("sign", v)))
            return
        if m == "xor" and ins.op_str in ("ah, ah", "dh, dh", "bh, bh", "ch, ch"):
            full = {"a": "eax", "d": "edx", "b": "ebx", "c": "ecx"}[ins.op_str[0]]
            if full not in self.regs:
                # the high byte cleared before the low byte is loaded
                self.set_reg(full, E("0", 4, atom=True, tag=("hiclr",)))
                return
            v = self.reg(full, ins)
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
                    v = E("%s & %d" % (a.p(), (s.imm & 0xFF) - 256), a.size)
                    self.set_reg(full, v)
                    self.flags = ("val", v, None)
                    return
                if m == "and" and s.type == cx.X86_OP_IMM and sz == 4 and s.imm in (0xFF, 0xFFFF) \
                        and a.size == 4 and a.atom and a.text.startswith("*(int *)") and \
                        not os.environ.get("LIFT_ANDCAST"):
                    # a dword read masked to 16/8 bits: `x & 0xffff` (the cast narrows the load)
                    v = E("%s & %d" % (a.p(), s.imm), 4)
                    self.set_reg(full, v)
                    self.flags = ("val", v, None)
                    return
                if m == "and" and s.type == cx.X86_OP_IMM and sz == 4 and s.imm in (0xFF, 0xFFFF):
                    t = "unsigned char" if s.imm == 0xFF else "unsigned short"
                    self.set_reg(full, E(ext_text(t, a), 4, atom=IMPLICIT))
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
                    (self.loaded_ptr(a) or self.loaded_ptr(b))
                if ptr_hint:
                    # a dword read kept in a register for an add (not folded into it):
                    # perhaps a pointer, a choice point
                    self.choices.append(ins.address + 0.375)
                if m == "add" and s.type == cx.X86_OP_REG and (
                        self.used_as_base(full) or self.global_ptr(b) or self.global_ptr(a) or
                        (ptr_hint and (ins.address + 0.375 in self.flips) ==
                         bool(os.environ.get("LIFT_NOPTRADD")))):
                    # pointer arithmetic: the operand loaded from memory is the base pointer
                    # (as char *, -od keeps it in a register: mov edx,[p]; add eax,edx)
                    pa, pb = (b, a) if self.loaded_ptr(b) and not self.loaded_ptr(a) else (a, b)
                    ptxt = ("*(char **)" + pa.text[len("*(int *)"):]) if self.loaded_ptr(pa) \
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
                if m in ("add", "imul", "and", "or", "xor") and s.type == cx.X86_OP_REG and \
                        not CONST_RE.fullmatch(a.text) and not CONST_RE.fullmatch(b.text) and \
                        "func_" not in a.text + b.text:
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
                    pre_r is not None and self.reg_used_later(pre_r):
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
            if m in ("inc", "dec") and prev is not None and prev.mnemonic == "mov" and \
                    prev.operands[0].type == cx.X86_OP_REG and \
                    prev.op_str.split(", ", 1)[1] == ins.op_str and \
                    prev.operands[0].size == d.size:
                # x++ used as a value: the old value, loaded just before, is used afterwards
                full_r = subreg(prev.reg_name(prev.operands[0].reg))[0]
                if self.reg_used_later(full_r):
                    self.post_expr("%s%s" % (lv, "++" if m == "inc" else "--"), full_r, d.size)
                    return
            dead_load = prev is not None and prev.mnemonic == "mov" and prev.operands and \
                prev.operands[0].type == cx.X86_OP_REG and \
                re.sub(r"^\w+ ptr ", "", prev.op_str.split(", ", 1)[1]) == \
                re.sub(r"^\w+ ptr ", "", ins.op_str)
            if m in ("inc", "dec") and not dead_load and ebp_slot(ins, d) is not None and \
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
            loaded = [r for r in PARM_REGS if r in self.regs and self.regs[r] is not self.pending
                      and r not in self.stale]
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
                    if (r in loaded or (pend and ins.address + 0.5 in self.flips)) and \
                            not any(self.regs[r] is self.regs[q] for q in PARM_REGS[:nreg]):
                        nreg += 1
                    else:
                        break
                self.calls.add(name)
            if nstack > len(self.pushes):
                raise Unsupported("call needs %d stack arguments, %d pushed" % (nstack, len(self.pushes)))
            args = []
            for r in PARM_REGS[:nreg]:
                if r not in self.regs:
                    raise Unsupported("call argument %s not loaded" % r)
                if self.regs[r] is self.pending:
                    self.pending = None              # nested call: f(g(x))
                args.append(self.regs[r].text)
            if nstack:
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

    def used_as_base(self, reg):
        """Is `reg` next read as the base of a memory operand (so it holds a pointer)?"""
        for ins in self.body[self.k + 1:]:
            for op in ins.operands:
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
            sw["store"] = i.address
            found[i.address] = sw
            self.temps.add(t)           # the compiler's own temp: not declared
        return found

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

    def parse_ctree(self, start, t, w, by_addr, nxt, stubs=frozenset()):
        """Walk the compare tree from `start`, tracking the selector's possible values.
        `stubs`: jumps a compare goes to that belong to the tree (an empty sub-range's
        `jmp default`, emitted right after the tree's own jumps)."""
        def node(a, fall=True):
            """('cmp', v, [(jcc, target)], fallthrough) / ('jmp', target) / ('table', jmp)
            / None for a leaf."""
            i = by_addr.get(a)
            if i is None:
                return None
            if i.mnemonic == "cmp" and ebp_slot(i, i.operands[0]) == t and \
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
                (prev.get(a) in nodes or start < a < hi) and
                by_addr[a].mnemonic == "jmp" and by_addr[a].operands[0].type == cx.X86_OP_IMM}
        if more and len(stubs) < 64:
            return self.parse_ctree(start, t, w, by_addr, nxt, stubs | more)
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
        e = self.reg("eax", ins)
        w, signed = sw["width"], sw["signed"]
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

    def reg_used_later(self, reg):
        for ins in self.body[self.k + 1:]:
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

    def indirect_call(self, ins, op):
        """call [mem] / call reg: through a function pointer, arguments in registers."""
        if op.type == cx.X86_OP_MEM:
            target = self.mem(op, ins, 4)
            if target == "@RET":
                raise Unsupported("call through the return slot")
        else:
            target = self.reg(subreg(ins.reg_name(op.reg))[0], ins).p()
        args = []
        for r in PARM_REGS:
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
    def c(self):
        self.lift()
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
        lines = ["%s %s(%s)" % ("void" if self.void else self.ret_type, name, ", ".join(ps) or "void"),
                 "{"]
        # Locals below every parameter were declared in a nested block: Watcom gives a
        # block's locals their slots when the block starts, after the function's own.
        nested = [o for o in rest + two if o in self.nested]
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
        if PIN_SLOTS in self.flips:
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
        # function-level choice points: code generator options (#pragma dagger)
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
            decl.append(d)
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
FUNC_OPTS = ["KKND_CONFREV", "DAGGER_LEFTPREF", "DAGGER_CHARAUTOSMALL", "DAGGER_CLRAFTER",
             "DAGGER_WORDSTORE", "DAGGER_RMW", "DAGGER_PUSHMEM", "DAGGER_DEADDEF", "DAGGER_CDQ",
             "DAGGER_FLUSH", "DAGGER_CHARPARMBIG", "DAGGER_SIGNEDBF", "KKND_CONSTREG",
             "KKND_LINSEL", "KKND_NOROT", "KKND_STRETCH", "DAGGER_FIRSTUSE",
             "DAGGER_NOSAVES", "DAGGER_REGLAST", "DAGGER_NOGIVEN", "DAGGER_CONFLIST",
             "DAGGER_CONFLISTREV", "DAGGER_KEEPSUB", "DAGGER_NODEMOTE",
             "DAGGER_NOCVTDEMOTE", "DAGGER_DEADDEFMEM"]
IMPLICIT = bool(os.environ.get("LIFT_IMPLICIT"))


def ext_text(t, v):
    """A narrow value widened to int: an explicit (int) cast, or (LIFT_IMPLICIT) left to the
    usual promotions."""
    if IMPLICIT:
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
            for j in range(k - 1, max(-1, k - 24), -1):
                i = ins[j]
                if i.mnemonic in ("call", "ret") or i.mnemonic.startswith("j"):
                    break
                if i.mnemonic == "push":
                    if i.operands[0].type == cx.X86_OP_IMM:
                        ev.setdefault(tgt, {}).setdefault(n, set()).add("int")
                    elif i.operands[0].type == cx.X86_OP_REG and j and \
                            ins[j - 1].mnemonic == "mov" and \
                            ins[j - 1].op_str.startswith(i.op_str + ", 0x") and \
                            ins[j - 1].operands[1].imm & 0x80000000:
                        # a negative constant pushed through a register: a signed narrow one
                        ev.setdefault(tgt, {}).setdefault(n, set()).add("signed")
                    n += 1
    out = {}
    for tgt, params in ev.items():
        types = []
        for n in range(max([4] + [m + 1 for m in params])):
            seen = params.get(n, set()) - {"const"}
            if seen == {"signed"}:
                types.append("signed")
                continue
            seen -= {"signed"}
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


def lift(va, flips=frozenset(), info=None):
    """C for the function at `va`. `flips`: choice points (instruction addresses) to take
    the other way; `info`, a dict, receives the list of choice points as info["choices"]."""
    init()
    f = Func(va)
    f.flips = flips
    try:
        return f.c()
    finally:
        if info is not None:
            info["choices"] = list(f.choices)


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
