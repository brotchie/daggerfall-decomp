#!/usr/bin/env python3
"""Compile a C file with the project compiler and print the disassembly of each function.

usage: cc_dis.py file.c [--flags "..."]
"""
import argparse
import os
import sys
import tempfile

import capstone

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from match import compile_c, default_flags  # noqa: E402
from omf import OMF  # noqa: E402


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("src")
    ap.add_argument("--flags", default=None)
    a = ap.parse_args()
    flags = a.flags.split() if a.flags is not None else default_flags()
    with tempfile.TemporaryDirectory() as td:
        objp = os.path.join(td, "m.obj")
        compile_c(a.src, flags, objp)
        obj = OMF(objp)
    md = capstone.Cs(capstone.CS_ARCH_X86, capstone.CS_MODE_32)
    for name, si, off, size in obj.functions():
        print("%s:" % name)
        for i in md.disasm(bytes(obj.data[si][off:off + size]), 0):
            print("  %04x %-18s %s %s" % (i.address, i.bytes.hex(), i.mnemonic, i.op_str))


if __name__ == "__main__":
    main()
