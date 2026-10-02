#!/usr/bin/env python3
"""Predict Watcom C32 10.0a's stack frame (-od -d2) from a function's declarations.

The model (found by probing the real compiler, it fit every probe and the matched functions):
take the register parameters in order, then the locals in declaration order, then the
return-value slot (non-void functions); shell-sort that list by size, largest first, strict
comparisons, gaps n/2 then alternately g/2+1 and g/2 down to 1 (Watcom's unstable ShellSort);
allocate from the bottom of the frame upward, so the last element sits nearest ebp. Each slot
takes at least 4 bytes. Compiler temps and locals of inner blocks go below all of these.

usage: w10_frame.py name:size [name:size ...]     (e.g. a1:4 l_24:2 l_18:2 l_1C:2 ret:4)
prints the slots from the top of the frame down, with their ebp offsets
"""
import sys


def gaps(n):
    s = [n // 2] if n // 2 >= 1 else []
    alt = True
    while s and s[-1] > 1:
        s.append(s[-1] // 2 + 1 if alt else s[-1] // 2)
        alt = not alt
    return s


def shell(items, key):
    a = list(items)
    for gap in gaps(len(a)):
        for i in range(gap, len(a)):
            t, j = a[i], i
            while j >= gap and key(a[j - gap]) < key(t):
                a[j] = a[j - gap]
                j -= gap
            a[j] = t
    return a


def layout(decls, top=0x14):
    """decls: [(name, size)] in the order above -> [(name, ebp offset)] from the top down.
    `top` is the bytes below ebp already used (the five saved registers: 0x14)."""
    order = shell(decls, key=lambda d: d[1])
    out, off = [], -top
    for name, size in reversed(order):
        off -= max(4, (size + 3) & ~3)
        out.append((name, off))
    return out


def main():
    decls = []
    for a in sys.argv[1:]:
        name, size = a.rsplit(":", 1)
        decls.append((name, int(size, 0)))
    for name, off in layout(decls):
        print("%-12s [ebp-%#x]" % (name, -off))


if __name__ == "__main__":
    main()
