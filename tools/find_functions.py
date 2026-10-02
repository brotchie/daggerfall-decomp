#!/usr/bin/env python3
"""Find the functions in FALL.EXE's code and write config/functions.csv.

Adapted from KKND-Decomp's find_functions.py (CC0). Differences for FALL.EXE:

- Two objects hold code. Object 1 is the Watcom-compiled game plus runtime. Object 2 is
  writable and mixes assembler-made code with data, so it is only walked from call targets,
  never gap-filled.
- Calls between objects 1 and 2 are rel32 fixups (the loader writes the displacement), so the
  image is loaded with relocations applied and those calls decode normally.
- Watcom 10 packs functions back to back in object 1: no alignment padding.

Method: recursive descent with capstone from these seeds:
  - the LE entry point
  - every call target
  - off32 fixups into object 1 whose source is in a data object (function-pointer tables)
  - off32 fixups into object 1 used as `push imm32` / `mov reg, imm32` (callbacks), when the
    target starts with a plausible prologue
  - switch tables (`jmp cs:[reg*4 + table]` or `shl eax,2; jmp cs:[eax + table]`) are read
    through the fixups at consecutive dwords
then repeated gap filling in object 1: an undecoded byte that starts with a plausible prologue,
is not inside a table and is not between two pieces of one function becomes a new function.

A function's size runs to its last decoded instruction (or the last one it reaches by a jump
before the next function starts).

usage: find_functions.py [FALL.EXE]
"""
import bisect
import csv
import os
import sys

import capstone
from capstone import x86 as cx

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE, SRC_OFF32  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CODE_OBJS = (1, 2)
GAP_FILL_OBJS = (1,)
PROLOGUES = (b"\x55\x89\xe5", b"\x53", b"\x51", b"\x52", b"\x56", b"\x57", b"\x55",
             b"\x83\xec", b"\x81\xec", b"\x60", b"\x1e", b"\x06")


def main():
    exe = sys.argv[1] if len(sys.argv) > 1 else os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")
    le = LE(exe)
    img = le.load(relocate=True)
    fix = le.fixups()
    fix_at = {f.src_va: f for f in fix}
    fix_bytes = set()
    for f in fix:
        for k in range(f.size):
            fix_bytes.add(f.src_va + k)

    def obj_range(i):
        o = le.objs[i - 1]
        return o.base, o.base + o.vsize

    def in_code(va):
        o = le.obj_of_va(va)
        return o is not None and o.index in CODE_OBJS

    def rd(va, n):
        o = le.obj_of_va(va)
        return bytes(img[o.index][va - o.base: va - o.base + n])

    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    md.detail = True

    funcs = {le.entry_va}
    for f in fix:
        if f.kind == SRC_OFF32 and f.target_obj == 1:
            src = le.obj_of_va(f.src_va)
            if src is not None and src.index not in CODE_OBJS:
                funcs.add(f.target_va)

    covered = {}         # insn va -> length
    owner = {}           # insn va -> function va
    table_bytes = set()  # jump-table dwords inside code
    scan_values = {}     # scan switch value tables inside code: start -> end
    done = set()

    def walk(fva):
        lo, hi = obj_range(le.obj_of_va(fva).index)
        work, seen = [fva], set()
        edi_val = None
        while work:
            va = work.pop()
            while lo <= va < hi and va not in seen:
                if va in covered and owner.get(va) != fva:
                    break  # ran into another function's code
                if va in table_bytes or any(a <= va < b for a, b in scan_values.items()):
                    break
                seen.add(va)
                ins = next(md.disasm(rd(va, 16), va), None)
                if ins is None:
                    break
                covered[va] = ins.size
                owner.setdefault(va, fva)
                g, m = ins.groups, ins.mnemonic
                if m == "mov" and ins.op_str.startswith("edi, ") and ins.size == 5:
                    ff = fix_at.get(va + 1)
                    edi_val = ff.target_va if ff and ff.target_obj == 1 else None
                if m in ("push", "mov") and ins.size >= 5:
                    ff = fix_at.get(va + ins.size - 4)
                    # `mov edi, offset values` also points into code (the byte table of a
                    # sparse switch), so a callback must start like a function.
                    if ff and ff.kind == SRC_OFF32 and ff.target_obj == 1 and \
                            ins.operands[-1].type == cx.X86_OP_IMM and \
                            any(rd(ff.target_va, len(p)) == p for p in PROLOGUES):
                        funcs.add(ff.target_va)
                if cx.X86_GRP_CALL in g:
                    op = ins.operands[0]
                    if op.type == cx.X86_OP_IMM and in_code(op.imm):
                        funcs.add(op.imm)
                elif cx.X86_GRP_JUMP in g:
                    op = ins.operands[0]
                    if op.type == cx.X86_OP_IMM:
                        if lo <= op.imm < hi:
                            work.append(op.imm)
                        elif in_code(op.imm):
                            funcs.add(op.imm)  # tail call into the other object
                        if m == "jmp":
                            break
                    elif op.type == cx.X86_OP_MEM:
                        t = op.mem.disp & 0xFFFFFFFF
                        # Watcom 10: `jmp cs:[reg*4 + table]`, or with the index pre-scaled,
                        # `shl eax,2; jmp cs:[eax + table]`; the table is in the code object.
                        # When the lowest case is k > 0 the displacement is table - 4k, so look
                        # a little way ahead for the first entry.
                        p = t
                        if op.mem.scale == 4:
                            while p not in fix_at and p < t + 4 * 64:
                                p += 4
                        # a scan switch (`mov edi, offset values; repne scasb; jmp
                        # cs:[ecx*4 + labels]`): its values sit just before the labels
                        if edi_val is not None and edi_val < p <= edi_val + 4 * 256:
                            scan_values[edi_val] = p
                            edi_val = None
                        if lo <= p < hi and p in fix_at and op.mem.index + op.mem.base != 0:
                            while p in fix_at and fix_at[p].kind == SRC_OFF32 and \
                                    lo <= fix_at[p].target_va < hi:
                                for k in range(4):
                                    table_bytes.add(p + k)
                                work.append(fix_at[p].target_va)
                                p += 4
                        break
                    else:
                        break
                elif cx.X86_GRP_RET in g or cx.X86_GRP_IRET in g or m in ("hlt", "ud2"):
                    break
                va += ins.size

    while True:
        pending = sorted(funcs - done)
        if pending:
            for fva in pending:
                done.add(fva)
                if in_code(fva):
                    walk(fva)
            continue
        new = set()
        starts = sorted(covered)
        for oi in GAP_FILL_OBJS:
            lo, hi = obj_range(oi)
            va = lo
            while va < hi:
                if va not in covered:
                    # Data between two pieces of the same function (switch tables and the
                    # byte tables of sparse switches sit mid-body) is not a new function.
                    i = bisect.bisect_left(starts, va)
                    if 0 < i < len(starts) and owner[starts[i - 1]] == owner[starts[i]] \
                            and le.obj_of_va(starts[i]).index == oi:
                        va = starts[i]
                        continue
                if va in covered:
                    va += covered[va]
                    continue
                b = rd(va, 1)[0]
                if b in (0x00, 0x90, 0xCC) or va in fix_bytes or va in table_bytes:
                    va += 1
                    continue
                if any(rd(va, len(p)) == p for p in PROLOGUES):
                    new.add(va)
                    while va < hi and va not in covered:
                        va += 1
                    continue
                va += 1
        new -= funcs
        if not new:
            break
        funcs |= new

    rows = []
    for oi in CODE_OBJS:
        lo, hi = obj_range(oi)
        fs = sorted(f for f in funcs if lo <= f < hi)
        for i, f in enumerate(fs):
            end = fs[i + 1] if i + 1 < len(fs) else hi
            last, va = f, f
            while va < end and va in covered:
                last = va + covered[va]
                va = last
            for a in range(va, end):
                if a in covered and owner.get(a) == f:
                    last = max(last, a + covered[a])
            rows.append((f, oi, max(last, f + 1) - f, end - f))

    os.makedirs(os.path.join(ROOT, "config"), exist_ok=True)
    with open(os.path.join(ROOT, "config", "functions.csv"), "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["va", "obj", "size", "span", "name"])
        for f, oi, size, span in rows:
            w.writerow(["0x%08X" % f, oi, size, span, "func_%08X" % f])
    blocks = []
    for a in sorted(table_bytes):
        if blocks and a == blocks[-1][1]:
            blocks[-1][1] = a + 1
        else:
            blocks.append([a, a + 1])
    with open(os.path.join(ROOT, "config", "code_data.csv"), "w", newline="") as fh:
        w = csv.writer(fh)
        w.writerow(["va", "size", "kind"])
        data = [(a, b - a, "jumptable") for a, b in blocks] + \
               [(a, b - a, "scanvalues") for a, b in scan_values.items()]
        for a, n, kind in sorted(data):
            w.writerow(["0x%08X" % a, n, kind])
    for oi in CODE_OBJS:
        lo, hi = obj_range(oi)
        n = sum(1 for r in rows if r[1] == oi)
        cov = sum(sz for va, sz in covered.items() if lo <= va < hi)
        print("obj %d: functions %d  decoded %d / %d bytes (%.1f%%)" %
              (oi, n, cov, hi - lo, 100.0 * cov / (hi - lo)))
    print("jump tables: %d" % len(blocks))


if __name__ == "__main__":
    main()
