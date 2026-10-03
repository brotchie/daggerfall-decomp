#!/usr/bin/env python3
"""Assemble the XnGine modules (src/xngine/*.asm) and check them against FALL.EXE's object 2.

Object 2 is the XnGine engine, hand-written asm. Each module is one MASM source file holding
one segment that covers [start, end) of object 2 (config/xngine_modules.csv). Borland Turbo
Assembler 4.0, the assembler XnGine was built with, assembles it to OMF under DOSBox-X.
A module matches when, with its fixups written as the linker and the LE loader would:
  - every byte equals the original file's byte, and
  - every LE fixup in the range comes from a fixup in our object with the same target, so every
    address in the module is a symbol, not a number. 16-bit selector fixups are the exception:
    the LE table keeps them and the source holds the file's value.

Plain data bytes are not in the repo (game files never are). The source names each run of
them `B_<address>_<length>`, and assemble_many() writes a per-module include that defines those
macros from the original FALL.EXE.

build_fall.py calls splice() to put matching modules into object 2. As a tool:

usage: xn_link.py [module ...]     assemble and check, print one line per module
"""
import csv
import os
import re
import shutil
import struct
import subprocess
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE, SRC_OFF32, SRC_REL32, SRC_SEL16  # noqa: E402
from omf import OMF  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")
MODULES = os.path.join(ROOT, "config", "xngine_modules.csv")
SRC = os.path.join(ROOT, "src", "xngine")
OBJDIR = os.path.join(ROOT, "build", "xngine")
OBJ2 = 2

NAME = re.compile(r"(?:func|D|L)_0*([0-9A-Fa-f]{5,8})$")
BLOB = re.compile(r"\bB_([0-9A-F]{6})_([0-9]+)\b")


TASM = os.path.join(ROOT, "third_party", "tasm", "BIN")
DOSDIR = os.path.join(ROOT, "build", "xngine", "dos")
TASM_MSG = re.compile(r"^\*+(Error|Fatal|Warning)\*+ (\S+?)\((\d+)\) (.*)$", re.M)
TASM_FATAL = re.compile(r"^\*\*Fatal\*\* (?!\S+\(\d+\))(.*)$", re.M)


def tasm_available():
    """Borland Turbo Assembler 4.0 (from your own copy, unpacked into third_party/tasm/BIN) and
    DOSBox-X to run it. TASMX.EXE is TASM as a DPMI program: real-mode TASM.EXE runs out of
    memory on the larger modules, and the two write the same objects."""
    return all(os.path.exists(os.path.join(TASM, f))
               for f in ("TASMX.EXE", "DPMI16BI.OVL", "RTM.EXE")) and \
        shutil.which("dosbox-x") is not None


def modules():
    """[(name, start, end)] in address order."""
    with open(MODULES, newline="") as f:
        return [(r["name"], int(r["start"], 16), int(r["end"], 16)) for r in csv.DictReader(f)]


class Image:
    """Object 2 of the original FALL.EXE: raw file bytes and LE fixups."""

    def __init__(self, le=None):
        self.le = le or LE(EXE)
        self.obj = self.le.objs[OBJ2 - 1]
        self.raw = self.le.load(relocate=False)[OBJ2]
        self.fixups = self.le.fixups()
        self.fix_at = {f.src_va: f for f in self.fixups}

    def bytes_at(self, va, n):
        o = va - self.obj.base
        return bytes(self.raw[o:o + n])


def write_blobs(asm_text, inc_path, img):
    """Define every B_<address>_<length> macro the source uses, from the original bytes
    (DOS line ends)."""
    lines = []
    for a, n in sorted(set((int(a, 16), int(n)) for a, n in BLOB.findall(asm_text))):
        data = img.bytes_at(a, n)
        lines.append("B_%06X_%d macro" % (a, n))
        for k in range(0, n, 16):
            lines.append("    db " + ", ".join("0%02Xh" % b for b in data[k:k + 16]))
        lines.append("endm")
    text = "\n".join(lines) + "\n"
    with open(inc_path, "w", newline="\r\n") as f:
        f.write(text)


def assemble(asm_path, img, objdir=OBJDIR):
    """Assemble one module. Returns (obj path or None, assembler messages)."""
    return assemble_many([asm_path], img, objdir)[asm_path]


def assemble_many(asm_paths, img, objdir=OBJDIR):
    """Assemble modules with TASM 4.0 in a few DOSBox-X sessions run side by side (8.3 names
    M000.ASM, M001.ASM, ... with their blob includes). Returns {path: (obj path or None,
    messages)}; TASM's messages are rewritten as `path(N): Error! ...`, naming the original
    path, so error lines point into it."""
    os.makedirs(objdir, exist_ok=True)
    out = {}
    if not tasm_available():
        raise SystemExit("no assembler: XnGine needs Borland Turbo Assembler 4.0 "
                         "(third_party/tasm/BIN/TASMX.EXE) and DOSBox-X")
    shutil.rmtree(DOSDIR, ignore_errors=True)
    # up to 8 DOSBox-X sessions side by side, the files dealt out largest first
    jobs = max(1, min(8, os.cpu_count() or 1, len(asm_paths) // 4 or 1))
    load = [0] * jobs
    share = [[] for _ in range(jobs)]
    for path in sorted(asm_paths, key=os.path.getsize, reverse=True):
        j = load.index(min(load))
        load[j] += os.path.getsize(path)
        share[j].append(path)
    names, runs = {}, []
    env = dict(os.environ, SDL_VIDEODRIVER="dummy", SDL_AUDIODRIVER="dummy")
    opts = []
    for kv in os.environ.get("DAGGER_DOSBOX", "cpu:cycles=max").split():
        opts += ["-set", kv.replace(":", " ", 1)]
    for j, paths in enumerate(share):
        if not paths:
            continue
        d = os.path.join(DOSDIR, "J%02d" % j)
        os.makedirs(d)
        lines = ["@echo off", "set PATH=D:\\;Z:\\", "C:"]
        for k, path in enumerate(paths):
            n = "M%03d" % k
            names[path] = d, n
            base = os.path.splitext(os.path.basename(path))[0]
            text = open(path).read().replace("include %s.inc" % base, "include %s.INC" % n)
            with open(os.path.join(d, n + ".ASM"), "w", newline="\r\n") as f:
                f.write(text)
            write_blobs(text, os.path.join(d, n + ".INC"), img)
            lines.append("TASMX /ml %s.ASM > %s.OUT" % (n, n))
        with open(os.path.join(d, "GO.BAT"), "w", newline="\r\n") as f:
            f.write("\n".join(lines) + "\n")
        runs.append(subprocess.Popen(
            ["dosbox-x", "-silent", "-nogui", "-nomenu"] + opts +
            ["-c", "mount c %s" % d, "-c", "mount d %s" % TASM, "-c", "c:\\go.bat", "-exit"],
            stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, env=env))
    for r in runs:
        try:
            r.wait(timeout=600)
        except subprocess.TimeoutExpired:
            r.kill()
            r.wait()
    for path, (d, n) in names.items():
        base = os.path.splitext(os.path.basename(path))[0]
        src_obj, err = os.path.join(d, n + ".OBJ"), os.path.join(d, n + ".OUT")
        msgs = open(err, errors="replace").read() if os.path.exists(err) else "no output"
        msgs = TASM_MSG.sub(lambda m: "%s(%s): %s! %s" % (
            path if m.group(2).upper() == n + ".ASM" else m.group(2), m.group(3),
            "Warning" if m.group(1) == "Warning" else "Error", m.group(4)), msgs)
        msgs = TASM_FATAL.sub(lambda m: "%s: Error! %s" % (path, m.group(1)), msgs)
        obj = os.path.join(objdir, base + ".obj")
        if os.path.exists(obj):
            os.remove(obj)
        if os.path.exists(src_obj) and "Error!" not in msgs:
            shutil.copyfile(src_obj, obj)
            out[path] = obj, msgs
        else:
            out[path] = None, msgs
    return out


def link(obj_path, start, end, img):
    """Write the module's fixups as the file holds them. Returns (bytes, problems) where
    problems is [(va, reason)]: fixups whose target differs from the original's, LE fixups with
    no symbol, and bytes that differ."""
    obj = OMF(obj_path)
    segs = [si for si, d in obj.data.items() if d]
    problems = []
    if len(segs) != 1:
        return b"", [(start, "expected one segment with data, found %d" % len(segs))]
    si = segs[0]
    data = bytearray(obj.data[si])
    if len(data) != end - start:
        problems.append((start, "length %#x != %#x" % (len(data), end - start)))
        data = data[:end - start] + bytearray(max(0, end - start - len(data)))
    covered = set()
    for fx in obj.fixups:
        if fx.seg != si:
            continue
        field = start + fx.offset
        if fx.size != 4:
            problems.append((field, "%d-byte fixup" % fx.size))
            continue
        if fx.offset + 4 > len(data):
            problems.append((field, "fixup past the end"))
            continue
        addend = struct.unpack_from("<i", data, fx.offset)[0]
        if fx.target_kind == "seg" and fx.target_index == si:
            want = start + addend
        elif fx.target_kind == "ext" and NAME.match(fx.target):
            want = int(NAME.match(fx.target).group(1), 16) + addend
        else:
            problems.append((field, "unknown target %s %s" % (fx.target_kind, fx.target)))
            continue
        want &= 0xFFFFFFFF
        of = img.fix_at.get(field)
        if fx.selfrel:
            if of is not None:
                if of.kind != SRC_REL32 or of.target_va != want:
                    problems.append((field, "call target %#x != original %#x" % (
                        want, of.target_va)))
                value = 0       # between objects the loader writes the displacement
            else:
                value = want - (field + 4)
        else:
            if of is None or of.kind != SRC_OFF32 or of.target_va != want:
                problems.append((field, "reference %#x != original %s" % (
                    want, "%#x" % of.target_va if of else "number (no fixup)")))
                value = 0
            else:
                value = want - img.le.objs[of.target_obj - 1].base
        struct.pack_into("<I", data, fx.offset, value & 0xFFFFFFFF)
        covered.add(field)
    for f in img.fixups:
        if start <= f.src_va < end and f.kind != SRC_SEL16 and f.src_va not in covered:
            problems.append((f.src_va, "LE fixup to %#x has no symbol" % f.target_va))
    want_bytes = img.bytes_at(start, end - start)
    k = 0
    while k < len(want_bytes):
        if data[k] != want_bytes[k]:
            j = k
            while j < len(want_bytes) and data[j] != want_bytes[j]:
                j += 1
            problems.append((start + k, "%d bytes differ" % (j - k)))
            k = j
        else:
            k += 1
    return bytes(data), problems


def check(name, start, end, img, src=SRC, objdir=OBJDIR):
    """Assemble and link one module: (bytes or None, problems, assembler messages)."""
    return check_many([(name, start, end)], img, src, objdir)[name]


def check_many(mods, img, src=SRC, objdir=OBJDIR):
    """Assemble [(name, start, end)] in one batch and link each:
    {name: (bytes or None, problems, assembler messages)}."""
    paths = {name: os.path.join(src, name + ".asm") for name, _s, _e in mods}
    res = assemble_many(list(paths.values()), img, objdir)
    out = {}
    for name, start, end in mods:
        obj, msgs = res[paths[name]]
        if obj is None:
            out[name] = None, [(start, "does not assemble")], msgs
        else:
            data, problems = link(obj, start, end, img)
            out[name] = data, problems, msgs
    return out


def splice(raw, img, blank=False):
    """Assemble every module, put the matching ones into raw[2] (the unrelocated object 2
    image). Returns ([(va, name, source)] of the functions they hold, [error lines])."""
    matched, errors, done = [], [], []
    if not os.path.exists(MODULES):
        return matched, errors
    funcs = []
    with open(os.path.join(ROOT, "config", "functions.csv"), newline="") as f:
        for r in csv.DictReader(f):
            if r["obj"] == str(OBJ2):
                funcs.append((int(r["va"], 16), r["name"]))
    if not tasm_available():
        print("xngine   no assembler (TASM 4.0 under DOSBox-X): object 2 keeps the original bytes")
        return matched, errors
    buf = raw[OBJ2]
    base = img.obj.base
    have = [m for m in modules() if os.path.exists(os.path.join(SRC, m[0] + ".asm"))]
    results = check_many(have, img)
    for name, start, end in have:
        rel = os.path.relpath(os.path.join(SRC, name + ".asm"), ROOT)
        data, problems, msgs = results[name]
        if problems:
            va, why = problems[0]
            errors.append("%s: %s at %#x (%d problems)%s" % (
                rel, why, va, len(problems), "\n" + msgs.strip() if data is None else ""))
            continue
        if blank:
            buf[start - base:end - base] = b"\xcc" * (end - start)
        buf[start - base:end - base] = data
        matched += [(va, fn, rel) for va, fn in funcs if start <= va < end]
        done.append((start, end))
    # object 2 has more functions than config/functions.csv lists (xngine_functions.csv)
    p = os.path.join(ROOT, "config", "xngine_functions.csv")
    if os.path.exists(p):
        with open(p, newline="") as f:
            rows = [(int(r["va"], 16), int(r["code_bytes"])) for r in csv.DictReader(f)]
        ok = [(va, n) for va, n in rows if any(s <= va < e for s, e in done)]
        print("xngine   %d / %d modules from asm: %d / %d functions, %d / %d code bytes" % (
            len(done), len(modules()), len(ok), len(rows), sum(n for _v, n in ok),
            sum(n for _v, n in rows)))
    return matched, errors


def main():
    img = Image()
    want = set(sys.argv[1:])
    bad = 0
    mods = [m for m in modules() if not want or m[0] in want]
    results = check_many(mods, img)
    for name, start, end in mods:
        data, problems, msgs = results[name]
        if problems:
            bad += 1
            print("%-14s %#x-%#x  %d problems: %s" % (
                name, start, end, len(problems),
                "; ".join("%#x %s" % p for p in problems[:3])))
            if data is None:
                print(msgs.strip())
        else:
            print("%-14s %#x-%#x  OK" % (name, start, end))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
