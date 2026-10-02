#!/usr/bin/env python3
"""Run FALL.EXE headless: the CPU is Unicorn, and the DOS extender, DOS, BIOS and the PC
hardware the game touches are emulated here in Python.

The game was built for CauseWay, which runs it zero-based and flat (it reads the BIOS tick
count at 0x46C and video memory at 0xA0000 directly) and loads the LE objects above 1 MB,
relocated through their fixups. Here they load at LOAD + their preferred address, so a function
the sources call func_00012345 runs at 0x01012345. Real-mode memory (PSP, environment, DOS
blocks, the BIOS data area, VGA) is linear 0-1 MB.

Time is counted in instructions, so a run is deterministic: the timer interrupt fires every
TICK instructions while the game has interrupts enabled.

Files: drive C: is the game directory (build/game), read-only; writes go to an overlay
directory (build/emu/overlay) that reads also check first.

usage: fallemu.py [--frames N] [--keys "..."] [--shots DIR] [--trace] [args for FALL.EXE]
"""
import argparse
import os
import struct
import sys
import time
import zlib

from unicorn import Uc, UcError, UC_ARCH_X86, UC_MODE_32, UC_HOOK_INTR, UC_HOOK_INSN, \
    UC_HOOK_MEM_UNMAPPED, UC_HOOK_CODE, UC_HOOK_MEM_WRITE, UC_PROT_ALL
from unicorn.x86_const import *  # noqa: F401,F403

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from le import LE, SRC_OFF32, SRC_SEL16  # noqa: E402

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
EXE = os.path.join(ROOT, "orig", "1.07.213", "FALL.EXE")
GAME = os.path.join(ROOT, "build", "game")
OVERLAY = os.path.join(ROOT, "build", "emu", "overlay")

LOAD = 0x01000000          # the LE objects load at LOAD + their preferred address
MEM = 64 << 20             # program memory from LOAD: objects, then the heap
HEAP = 0x00200000          # where memory blocks start, relative to LOAD
LOW = 0x110000             # real-mode memory, with the HMA
GDT = 0x0800               # descriptor table (256 entries)
PSP = 0x1000               # program segment prefix
ENV = 0x1100               # environment block
STUBS = 0x1800             # default interrupt handlers: int 0FEh; iretd (4 bytes each)
DOSMEM = 0x2000            # DOS memory pool for DPMI 0100h, up to 0x9F000
TICK = 400000              # instructions per timer interrupt

SEL_CODE, SEL_DATA, SEL_ZERO, SEL_PSP, SEL_ENV, SEL_STUB = 0x08, 0x10, 0x18, 0x20, 0x28, 0x30
FIRST_FREE = 0x38

R = {n: globals()["UC_X86_REG_" + n.upper()] for n in (
    "eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp", "eip", "eflags",
    "cs", "ds", "es", "fs", "gs", "ss", "ax", "bx", "cx", "dx", "si", "di",
    "al", "ah", "bl", "bh", "cl", "ch", "dl", "dh")}


class Stop(Exception):
    pass


def descriptor(base, limit, access):
    """An 8-byte GDT descriptor; 32-bit, page granular when the limit needs it."""
    flags = 0x4
    if limit > 0xFFFFF:
        limit >>= 12
        flags |= 0x8
    return struct.pack("<HHBBBB", limit & 0xFFFF, base & 0xFFFF, (base >> 16) & 0xFF, access,
                       (flags << 4) | ((limit >> 16) & 0xF), (base >> 24) & 0xFF)


def png(path, w, h, pixels, palette):
    """Write an 8-bit paletted image as an RGB PNG."""
    rows = []
    for y in range(h):
        row = bytearray(b"\0")
        for p in pixels[y * w:(y + 1) * w]:
            row += bytes(palette[p])
        rows.append(bytes(row))
    raw = b"".join(rows)

    def chunk(t, d):
        c = struct.pack(">I", len(d)) + t + d
        return c + struct.pack(">I", zlib.crc32(t + d) & 0xFFFFFFFF)
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
                + chunk(b"IDAT", zlib.compress(raw, 6)) + chunk(b"IEND", b""))


class Files:
    """DOS file handles over the game directory, with a writable overlay."""

    def __init__(self, root, overlay):
        self.root, self.overlay = root, overlay
        self.handles = {}
        self.next = 5
        self.cwd = ""           # relative to the drive root, no leading backslash
        self.log = []

    def host(self, dos, write=False):
        """Host path for a DOS path, case-insensitive; writes go to the overlay."""
        p = dos.replace("/", "\\")
        if len(p) > 1 and p[1] == ":":
            p = p[2:]
        if not p.startswith("\\"):
            p = (self.cwd + "\\" if self.cwd else "") + p
        parts = [x for x in p.split("\\") if x and x != "."]
        out = []
        for x in parts:
            if x == "..":
                if out:
                    out.pop()
            else:
                out.append(x)
        rel = os.path.join(*out) if out else ""
        ov = os.path.join(self.overlay, rel.upper())
        if write or os.path.exists(ov):
            return ov
        cur = self.root
        for x in out:
            try:
                names = os.listdir(cur)
            except OSError:
                return os.path.join(cur, x)
            m = [n for n in names if n.upper() == x.upper()]
            cur = os.path.join(cur, m[0] if m else x)
        return cur

    def open(self, dos, mode, create=False):
        path = self.host(dos, write=create or (mode & 3) != 0)
        if (mode & 3) != 0 and not create and not os.path.exists(path):
            src = self.host(dos)
            if os.path.exists(src):
                os.makedirs(os.path.dirname(path), exist_ok=True)
                with open(src, "rb") as a, open(path, "wb") as b:
                    b.write(a.read())
        try:
            if create:
                os.makedirs(os.path.dirname(path), exist_ok=True)
                f = open(path, "w+b")
            else:
                f = open(path, "r+b" if (mode & 3) else "rb")
        except OSError:
            self.log.append(("open-fail", dos))
            return None
        h = 5                    # DOS hands out the lowest free handle
        while h in self.handles:
            h += 1
        self.handles[h] = f
        self.log.append(("open", dos, h))
        return h


class Emu:
    def __init__(self, args=(), exe=EXE, game=GAME, overlay=OVERLAY, trace=False):
        self.trace = trace
        self.uc = uc = Uc(UC_ARCH_X86, UC_MODE_32)
        uc.mem_map(0, LOW)
        uc.mem_map(LOAD, MEM)
        self.le = le = LE(exe)
        img = le.load(relocate=True)
        for f in le.fixups():           # relocate to LOAD, as the extender's loader does
            o = le.obj_of_va(f.src_va)
            k = f.src_va - o.base
            if f.kind == SRC_OFF32:
                v = struct.unpack_from("<I", img[o.index], k)[0]
                struct.pack_into("<I", img[o.index], k, (v + LOAD) & 0xFFFFFFFF)
            elif f.kind == SRC_SEL16:
                struct.pack_into("<H", img[o.index], k, SEL_DATA)
        for o in le.objs:
            uc.mem_write(LOAD + o.base, bytes(img[o.index]))
        self.brk = HEAP                    # next free program offset for DPMI blocks
        self.blocks = {}                   # handle -> (offset, size)
        self.dos_next = DOSMEM >> 4        # next free real-mode paragraph
        self.files = Files(game, overlay)
        # descriptors: selector -> [base, limit, access]
        self.sels = {}
        self.setsel(SEL_CODE, 0, 0xFFFFFFFF, 0x9A)
        self.setsel(SEL_DATA, 0, 0xFFFFFFFF, 0x92)
        self.setsel(SEL_ZERO, 0, 0xFFFFFFFF, 0x92)
        self.setsel(SEL_PSP, PSP, 0xFF, 0x92)
        self.setsel(SEL_ENV, ENV, 0x6FF, 0x92)
        self.setsel(SEL_STUB, 0, 0xFFFFFFFF, 0x9A)
        uc.reg_write(UC_X86_REG_GDTR, (0, GDT, 256 * 8 - 1, 0))
        # default interrupt handlers: int 0FEh (we see which vector by EIP), iretd
        for n in range(256):
            uc.mem_write(STUBS + 4 * n, b"\xcd\xfe\xcf\x90")
        self.pm_vec = {n: (SEL_STUB, STUBS + 4 * n) for n in range(256)}
        self.rm_vec = {n: (0xF000, 0xFF53) for n in range(256)}
        self.exc = {}
        # PSP and environment
        tail = (" " + " ".join(args)).encode() if args else b""
        psp = bytearray(256)
        psp[0:2] = b"\xcd\x20"
        struct.pack_into("<H", psp, 0x2C, SEL_ENV)
        psp[0x80] = len(tail)
        psp[0x81:0x81 + len(tail)] = tail
        psp[0x81 + len(tail)] = 0x0D
        uc.mem_write(PSP, bytes(psp))
        env = b"PATH=C:\\\0COMSPEC=C:\\COMMAND.COM\0\0\x01\0C:\\FALL.EXE\0"
        uc.mem_write(ENV, env)
        # BIOS data: 80x25 colour text, tick count 0
        uc.mem_write(0x449, b"\x03")
        # CPU state at entry, as DOS/4G-style extenders leave it
        uc.reg_write(UC_X86_REG_CS, SEL_CODE)
        for s in ("ds", "ss"):
            uc.reg_write(R[s], SEL_DATA)
        uc.reg_write(UC_X86_REG_ES, SEL_PSP)
        uc.reg_write(UC_X86_REG_FS, 0)
        uc.reg_write(UC_X86_REG_GS, 0)
        uc.reg_write(UC_X86_REG_ESP, LOAD + le.objs[le.esp_obj - 1].base + le.esp)
        uc.reg_write(UC_X86_REG_EIP, LOAD + le.entry_va)
        uc.reg_write(UC_X86_REG_EFLAGS, 0x202)
        # hardware state
        self.palette = [(0, 0, 0)] * 256
        self.pal_w = self.pal_r = 0
        self.pal_sub = 0
        self.pal_rgb = []
        self.retrace = 0
        self.mode = 3
        self.kbd = []           # pending scancodes
        self.port60 = 0
        self.mouse = [160, 100, 0]
        self.ticks = 0
        self.insns = 0
        self.dta = 0
        self.find = None
        self.exit_code = None
        self.unknown = []
        self.notes = set()      # odd things the program did that DOS tolerates
        uc.hook_add(UC_HOOK_INTR, self.on_int)
        uc.hook_add(UC_HOOK_INSN, self.on_in, None, 1, 0, UC_X86_INS_IN)
        uc.hook_add(UC_HOOK_INSN, self.on_out, None, 1, 0, UC_X86_INS_OUT)
        uc.hook_add(UC_HOOK_MEM_UNMAPPED, self.on_unmapped)

    # -- registers and memory --------------------------------------------------------
    def r(self, n):
        return self.uc.reg_read(R[n])

    def w(self, n, v):
        self.uc.reg_write(R[n], v)

    def setsel(self, sel, base, limit, access):
        self.sels[sel] = [base, limit, access]
        self.uc.mem_write(GDT + (sel & ~7), descriptor(base, limit, access))

    def lin(self, sel, off):
        return (self.sels.get(sel & ~7, [0])[0] + off) & 0xFFFFFFFF

    def read(self, lin, n):
        return bytes(self.uc.mem_read(lin, n))

    def write(self, lin, data):
        self.uc.mem_write(lin, bytes(data))

    def asciiz(self, lin, limit=260):
        b = self.read(lin, limit)
        return b[:b.index(0)].decode("latin1") if 0 in b else b.decode("latin1")

    def ds_edx(self):
        return self.lin(self.r("ds"), self.r("edx"))

    def ret_addr(self):
        """The caller of the routine that made this DOS call: its return address."""
        return struct.unpack("<I", self.read(self.lin(self.r("ss"), self.r("esp")), 4))[0]

    def carry(self, on):
        f = self.r("eflags")
        self.w("eflags", f | 1 if on else f & ~1)

    def zero(self, on):
        f = self.r("eflags")
        self.w("eflags", f | 0x40 if on else f & ~0x40)

    def fail(self, code):
        self.w("ax", code)
        self.carry(True)

    # -- interrupts --------------------------------------------------------------------
    def on_int(self, uc, intno, _):
        try:
            if intno == 0xFE:
                n = (self.r("eip") - 2 - STUBS) // 4
                self.default_handler(n)
            elif intno == 0x21:
                self.int21()
            elif intno == 0x31:
                self.int31()
            elif intno == 0x10:
                self.int10()
            elif intno == 0x33:
                self.int33()
            elif intno == 0x16:
                self.int16()
            elif intno == 0x2F:
                if self.r("ax") == 0x1600:
                    self.w("al", 0)          # no Windows
                # other multiplex functions: not installed
            elif intno == 0x4B:
                self.carry(True)             # no virtual DMA services
            elif intno == 0x15:
                self.carry(True)
            elif intno == 0x1A:
                t = self.ticks
                self.w("cx", (t >> 16) & 0xFFFF)
                self.w("dx", t & 0xFFFF)
                self.w("al", 0)
            elif intno in (8, 9) or intno >= 0x70 or 0x0A <= intno <= 0x0F:
                self.default_handler(intno)
            else:
                self.unsupported("int %02Xh" % intno)
        except Stop:
            uc.emu_stop()
            raise

    def unsupported(self, what):
        msg = "%s ax=%04X at %#x" % (what, self.r("ax"), self.r("eip"))
        self.unknown.append(msg)
        if self.trace:
            print("UNSUPPORTED " + msg)
        self.carry(True)

    def default_handler(self, n):
        """What the BIOS would do for an interrupt nobody else handles."""
        if n == 8:          # timer: tick count, end of interrupt
            t = struct.unpack("<I", self.read(0x46C, 4))[0] + 1
            self.write(0x46C, struct.pack("<I", t))
        elif n == 9:        # keyboard: read the scan code
            pass

    def int21(self):
        ah, al = self.r("ah"), self.r("al")
        f = self.files
        self.carry(False)
        if self.trace and ah in (0x3F, 0x42, 0x3E, 0x44):
            print("int 21h ah=%02X al=%02X bx=%04X ecx=%08X edx=%08X from %#x" % (
                ah, al, self.r("bx"), self.r("ecx"), self.r("edx"), self.ret_addr()))
        if ah == 0x30:
            self.w("eax", 0x0005)        # DOS 5.0, no Phar Lap
            self.w("ebx", 0)
        elif self.r("ax") == 0xFF00:
            self.w("al", 1)              # DOS/4G-compatible extender API present
        elif ah == 0x25:                 # set vector (protected mode)
            self.pm_vec[al] = (self.r("ds"), self.r("edx"))
        elif ah == 0x35:                 # get vector
            sel, off = self.pm_vec[al]
            self.w("es", sel)
            self.w("ebx", off)
        elif ah == 0x2A:
            t = time.gmtime(820454400)   # fixed date: 1 Jan 1996, for determinism
            self.w("cx", t.tm_year)
            self.w("dh", t.tm_mon)
            self.w("dl", t.tm_mday)
            self.w("al", (t.tm_wday + 1) % 7)
        elif ah == 0x2C:
            cs = self.ticks * 100 // 18
            self.w("ch", (cs // 360000) % 24)
            self.w("cl", (cs // 6000) % 60)
            self.w("dh", (cs // 100) % 60)
            self.w("dl", cs % 100)
        elif ah == 0x19:
            self.w("al", 2)              # drive C:
        elif ah in (0x51, 0x62):
            self.w("bx", SEL_PSP)
        elif ah == 0x0E:
            self.w("al", 26)
        elif ah == 0x47:                 # current directory
            self.write(self.lin(self.r("ds"), self.r("esi")), f.cwd.encode() + b"\0")
        elif ah == 0x3B:                 # change directory
            d = self.asciiz(self.ds_edx()).replace("/", "\\")
            if len(d) > 1 and d[1] == ":":
                d = d[2:]
            f.cwd = d.strip("\\")
        elif ah in (0x3C, 0x5B):         # create
            h = f.open(self.asciiz(self.ds_edx()), 2, create=True)
            self.w("eax", h) if h else self.fail(3)
        elif ah == 0x3D:                 # open
            h = f.open(self.asciiz(self.ds_edx()), al)
            self.w("eax", h) if h else self.fail(2)
        elif ah == 0x3E:
            fh = f.handles.pop(self.r("bx"), None)
            if fh:
                fh.close()
        elif ah == 0x3F:                 # read
            h, n = self.r("bx"), self.r("ecx")
            fh = f.handles.get(h)
            if h == 0:
                self.w("eax", 0)
            elif fh is None:
                self.fail(6)
            else:
                data = fh.read(n)
                self.write(self.ds_edx(), data)
                self.w("eax", len(data))
        elif ah == 0x40:                 # write
            h, n = self.r("bx"), self.r("ecx")
            data = self.read(self.ds_edx(), n)
            if h in (1, 2):
                if self.trace:
                    sys.stdout.write(data.decode("latin1"))
                self.w("eax", n)
            elif h in f.handles:
                f.handles[h].write(data)
                self.w("eax", n)
            else:
                self.fail(6)
        elif ah == 0x41:                 # delete
            p = f.host(self.asciiz(self.ds_edx()), write=True)
            if os.path.exists(p):
                os.remove(p)
            else:
                self.fail(2)
        elif ah == 0x42:                 # seek
            fh = f.handles.get(self.r("bx"))
            if fh is None:
                self.fail(6)
                return
            off = ((self.r("cx") << 16) | self.r("dx"))
            if off & 0x80000000:
                off -= 1 << 32
            if al > 2:
                # DOS rejects the mode and does not move. XnGine's func_000C31AE never sets
                # AL, so this happens in play (a latent bug: the read after it relies on the
                # file already being at the right place).
                self.notes.add("seek with AL=%d (invalid mode) from %#x" % (
                    al, self.ret_addr()))
                self.fail(1)
                return
            fh.seek(off, al)
            pos = fh.tell()
            self.w("dx", pos >> 16)
            self.w("ax", pos & 0xFFFF)
            self.w("eax", pos)
            self.w("edx", pos >> 16)
        elif ah == 0x43:                 # attributes
            p = f.host(self.asciiz(self.ds_edx()))
            if os.path.exists(p):
                self.w("cx", 0x10 if os.path.isdir(p) else 0x20)
            else:
                self.fail(2)
        elif ah == 0x44:                 # ioctl
            h = self.r("bx")
            if al == 0:
                self.w("dx", 0x80D3 if h < 5 else 0x0002)
            elif al == 8:
                self.w("ax", 1)
            else:
                self.w("ax", 0)
        elif ah == 0x1A:
            self.dta = self.ds_edx()
        elif ah == 0x2F:
            self.w("es", SEL_ZERO)
            self.w("ebx", self.dta)
        elif ah == 0x4E:
            self.find_first(self.asciiz(self.ds_edx()))
        elif ah == 0x4F:
            self.find_next()
        elif ah == 0x48:                 # allocate memory: EBX paragraphs -> EAX selector
            n = self.r("ebx") << 4
            off = self.alloc(n)
            sel = self.new_sel(off, max(n, 1) - 1, 0x92)
            self.w("eax", sel)
        elif ah == 0x49:
            pass
        elif ah == 0x4A:
            pass
        elif ah == 0x4C:
            self.exit_code = al
            raise Stop()
        elif ah == 0x09:
            if self.trace:
                s = self.read(self.ds_edx(), 512)
                print(s[:s.index(b"$")].decode("latin1") if b"$" in s else s)
        elif ah in (0x01, 0x06, 0x07, 0x08, 0x0B, 0x0C):
            self.w("al", 0)
        else:
            self.unsupported("int 21h ah=%02Xh" % ah)

    def find_first(self, pattern):
        import fnmatch
        p = pattern.replace("/", "\\")
        d, _, mask = p.rpartition("\\")
        hd = self.files.host(d) if d else self.files.host("")
        names = []
        for base in (hd, self.files.host(d, write=True) if d else self.files.overlay):
            if os.path.isdir(base):
                names += [n for n in os.listdir(base) if fnmatch.fnmatch(n.upper(), mask.upper())]
        self.find = [(n, hd) for n in sorted(set(names))]
        self.find_next()

    def find_next(self):
        if not self.find:
            self.fail(18)
            return
        name, d = self.find.pop(0)
        p = os.path.join(d, name)
        size = os.path.getsize(p) if os.path.exists(p) else 0
        rec = bytearray(43)
        rec[21] = 0x10 if os.path.isdir(p) else 0x20
        struct.pack_into("<I", rec, 26, size)
        nm = name.upper().encode()[:12]
        rec[30:30 + len(nm)] = nm
        self.write(self.dta, rec)
        self.carry(False)

    # -- DPMI --------------------------------------------------------------------------
    def new_sel(self, base, limit, access):
        s = FIRST_FREE
        while s in self.sels:
            s += 8
        self.setsel(s, base, limit, access)
        return s

    def alloc(self, n):
        """A block of program memory: its linear address."""
        if self.trace:
            print("alloc %#x bytes at %#x (from %#x)" % (n, LOAD + self.brk, self.ret_addr()))
        off = self.brk
        self.brk = (self.brk + n + 0xFFF) & ~0xFFF
        if self.brk > MEM:
            raise Stop("out of memory")
        return LOAD + off

    def int31(self):
        ax = self.r("ax")
        self.carry(False)
        if ax == 0x0000:                 # allocate descriptors
            n = self.r("cx")
            first = self.new_sel(0, 0, 0x92)
            for k in range(1, n):
                self.setsel(first + 8 * k, 0, 0, 0x92)
            self.w("ax", first)
        elif ax == 0x0001:
            self.sels.pop(self.r("bx") & ~7, None)
        elif ax == 0x0002:               # segment to descriptor
            seg = self.r("bx")
            self.w("ax", self.new_sel(seg << 4, 0xFFFF, 0x92))
        elif ax == 0x0003:
            self.w("ax", 8)
        elif ax == 0x0006:               # get base
            base = self.sels.get(self.r("bx") & ~7, [0])[0]
            self.w("cx", base >> 16)
            self.w("dx", base & 0xFFFF)
        elif ax == 0x0007:
            s = self.sels.setdefault(self.r("bx") & ~7, [0, 0, 0x92])
            self.setsel(self.r("bx") & ~7, (self.r("cx") << 16) | self.r("dx"), s[1], s[2])
        elif ax == 0x0008:
            s = self.sels.setdefault(self.r("bx") & ~7, [0, 0, 0x92])
            self.setsel(self.r("bx") & ~7, s[0], (self.r("cx") << 16) | self.r("dx"), s[2])
        elif ax == 0x0009:
            s = self.sels.setdefault(self.r("bx") & ~7, [0, 0, 0x92])
            self.setsel(self.r("bx") & ~7, s[0], s[1], self.r("cl"))
        elif ax == 0x000A:               # alias descriptor
            s = self.sels.get(self.r("bx") & ~7, [0, 0, 0x92])
            self.w("ax", self.new_sel(s[0], s[1], 0x92))
        elif ax == 0x000B:
            s = self.sels.get(self.r("bx") & ~7, [0, 0, 0x92])
            self.write(self.lin(self.r("es"), self.r("edi")), descriptor(*s))
        elif ax == 0x0100:               # allocate DOS memory: BX paragraphs
            n = self.r("bx")
            seg = self.dos_next
            if (seg + n) << 4 > 0x9F000:
                self.fail(8)
                self.w("bx", 0)
                return
            self.dos_next += n
            self.write(seg << 4, b"\0" * (n << 4))
            self.w("ax", seg)
            self.w("dx", self.new_sel(seg << 4, (n << 4) - 1, 0x92))
        elif ax in (0x0101, 0x0102):
            pass
        elif ax == 0x0200:
            seg, off = self.rm_vec[self.r("bl")]
            self.w("cx", seg)
            self.w("dx", off)
        elif ax == 0x0201:
            self.rm_vec[self.r("bl")] = (self.r("cx"), self.r("dx"))
        elif ax == 0x0202:
            sel, off = self.exc.get(self.r("bl"), (SEL_STUB, STUBS + 4 * self.r("bl")))
            self.w("cx", sel)
            self.w("edx", off)
        elif ax == 0x0203:
            self.exc[self.r("bl")] = (self.r("cx"), self.r("edx"))
        elif ax == 0x0204:
            sel, off = self.pm_vec[self.r("bl")]
            self.w("cx", sel)
            self.w("edx", off)
        elif ax == 0x0205:
            self.pm_vec[self.r("bl")] = (self.r("cx"), self.r("edx"))
            if self.trace:
                print("vector %02Xh -> %04X:%08X" % (self.r("bl"), self.r("cx"), self.r("edx")))
        elif ax in (0x0300, 0x0301, 0x0302):
            self.realmode_call()
        elif ax == 0x0400:
            self.w("ax", 0x005A)
            self.w("bx", 0x0005)
            self.w("cl", 5)
            self.w("dx", 0x0870)
        elif ax == 0x0500:               # free memory information
            info = struct.pack("<12I", MEM - self.brk, (MEM - self.brk) >> 12,
                               (MEM - self.brk) >> 12, MEM >> 12, (MEM - self.brk) >> 12,
                               (MEM - self.brk) >> 12, MEM >> 12, (MEM - self.brk) >> 12,
                               0, 0, 0, 0)
            self.write(self.lin(self.r("es"), self.r("edi")), info + b"\xff" * 0)
        elif ax == 0x0501:               # allocate memory block: BX:CX bytes
            n = (self.r("bx") << 16) | self.r("cx")
            off = self.alloc(n)
            lin = off
            h = len(self.blocks) + 1
            self.blocks[h] = (off, n)
            self.w("bx", lin >> 16)
            self.w("cx", lin & 0xFFFF)
            self.w("si", h >> 16)
            self.w("di", h & 0xFFFF)
        elif ax == 0x0502:
            pass
        elif ax == 0x0503:               # resize: allocate anew and copy
            h = (self.r("si") << 16) | self.r("di")
            n = (self.r("bx") << 16) | self.r("cx")
            old = self.blocks.get(h)
            off = self.alloc(n)
            if old:
                self.write(off, self.read(old[0], min(old[1], n)))
            self.blocks[h] = (off, n)
            lin = off
            self.w("bx", lin >> 16)
            self.w("cx", lin & 0xFFFF)
        elif ax in (0x0600, 0x0601, 0x0602, 0x0603, 0x0702, 0x0703):
            pass                         # lock / unlock: nothing pages out here
        elif ax == 0x0604:
            self.w("bx", 0)
            self.w("cx", 0x1000)
        elif ax == 0x0800:               # physical mapping: identity below 4 GB
            pass
        elif ax == 0x0900:
            f = self.r("eflags")
            self.w("al", (f >> 9) & 1)
            self.w("eflags", f & ~0x200)
        elif ax == 0x0901:
            f = self.r("eflags")
            self.w("al", (f >> 9) & 1)
            self.w("eflags", f | 0x200)
        elif ax == 0x0902:
            self.w("al", (self.r("eflags") >> 9) & 1)
        elif ax == 0x0A00:
            self.carry(True)             # no vendor API
        elif ax in (0xFF26, 0xFF2F, 0xFF30, 0xFF31, 0xFF32):
            pass                         # CauseWay: transfer buffer, error-dump settings
        elif ax == 0xFF25:               # CauseWay GetDOSTrans
            self.w("bx", 0x9F00)
            self.w("dx", self.new_sel(0x9F000, 0xFFFF, 0x92))
            self.w("ecx", 0x1000)
        else:
            self.unsupported("int 31h ax=%04Xh" % ax)

    def realmode_call(self):
        """DPMI 0300h: simulate a real-mode interrupt with the register block at ES:EDI."""
        blk = self.lin(self.r("es"), self.r("edi"))
        regs = list(struct.unpack("<8IHHHHHHHHH", self.read(blk, 0x32)))
        n = self.r("bl")
        eax = regs[7]
        if n == 0x10:
            ah = (eax >> 8) & 0xFF
            if ah == 0x00:
                self.mode = eax & 0x7F
            elif ah == 0x4F:            # VESA: not present
                regs[7] = (eax & ~0xFFFF) | 0x014F
        elif n == 0x33:
            pass
        elif n == 0x2F:
            regs[7] = eax & ~0xFF
        else:
            self.unsupported("real-mode int %02Xh" % n)
        self.write(blk, struct.pack("<8IHHHHHHHHH", *regs))

    # -- BIOS --------------------------------------------------------------------------
    def int10(self):
        ah = self.r("ah")
        if ah == 0x00:
            self.mode = self.r("al") & 0x7F
            if self.mode == 0x13:
                self.write(0xA0000, b"\0" * 64000)
        elif ah == 0x0F:
            self.w("al", self.mode)
            self.w("ah", 40 if self.mode == 0x13 else 80)
            self.w("bh", 0)
        elif ah == 0x10 and self.r("al") == 0x12:
            first, n = self.r("bx"), self.r("cx")
            src = self.lin(self.r("es"), self.r("edx"))
            rgb = self.read(src, 3 * n)
            for k in range(n):
                self.palette[(first + k) & 0xFF] = tuple(min(255, v * 255 // 63) for v in rgb[3 * k:3 * k + 3])
        elif ah == 0x4F:
            self.w("ax", 0x014F)         # no VESA
        elif ah in (0x01, 0x02, 0x03, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0B, 0x0E, 0x11, 0x12, 0x1A):
            if ah == 0x1A:
                self.w("al", 0x1A)
                self.w("bl", 0x08)       # VGA colour
            if ah == 0x12:
                self.w("bl", 0x03)
        else:
            self.unsupported("int 10h ah=%02Xh" % ah)

    def int16(self):
        ah = self.r("ah") & 0xEF
        if ah == 0x00:
            sc = self.kbd.pop(0) if self.kbd else 0
            self.w("ax", sc << 8)
        elif ah == 0x01:
            self.zero(not self.kbd)
            if self.kbd:
                self.w("ax", self.kbd[0] << 8)
        elif ah == 0x02:
            self.w("al", 0)

    def int33(self):
        ax = self.r("ax")
        if ax == 0x0000:
            self.w("ax", 0xFFFF)
            self.w("bx", 2)
        elif ax == 0x0003:
            self.w("bx", self.mouse[2])
            self.w("cx", self.mouse[0] * 2)
            self.w("dx", self.mouse[1])
        elif ax == 0x000B:
            self.w("cx", 0)
            self.w("dx", 0)
        elif ax == 0x001B:
            self.w("bx", 50)
            self.w("cx", 50)
            self.w("dx", 50)
        elif ax in (0x0001, 0x0002, 0x0004, 0x0007, 0x0008, 0x000F, 0x001A, 0x001D, 0x000C,
                    0x0014, 0x0021):
            pass
        else:
            self.unsupported("int 33h ax=%04Xh" % ax)

    # -- ports -------------------------------------------------------------------------
    def on_in(self, uc, port, size, _):
        if port == 0x3DA:                # VGA status: alternate retrace
            self.retrace ^= 0x09
            return self.retrace
        if port == 0x3C9:
            c = self.palette[self.pal_r]
            v = c[self.pal_sub] * 63 // 255
            self.pal_sub += 1
            if self.pal_sub == 3:
                self.pal_sub = 0
                self.pal_r = (self.pal_r + 1) & 0xFF
            return v
        if port == 0x60:
            return self.port60
        if port == 0x64:
            return 0x14
        if port == 0x21 or port == 0xA1:
            return 0
        if port == 0x61:
            return 0x20
        if port in (0x40, 0x41, 0x42):
            return (self.insns >> 4) & 0xFF
        return 0xFF

    def on_out(self, uc, port, size, value, _):
        if port == 0x3C8:
            self.pal_w, self.pal_sub, self.pal_rgb = value & 0xFF, 0, []
        elif port == 0x3C7:
            self.pal_r, self.pal_sub = value & 0xFF, 0
        elif port == 0x3C9:
            self.pal_rgb.append(value & 0x3F)
            if len(self.pal_rgb) == 3:
                self.palette[self.pal_w] = tuple(v * 255 // 63 for v in self.pal_rgb)
                self.pal_w = (self.pal_w + 1) & 0xFF
                self.pal_rgb = []

    def on_unmapped(self, uc, access, address, size, value, _):
        print("unmapped access %#x (size %d, kind %d) at eip %#x" % (
            address, size, access, self.r("eip")))
        return False

    # -- running -----------------------------------------------------------------------
    def irq(self, vector):
        """Deliver a hardware interrupt to the game's handler, as the extender would."""
        sel, off = self.pm_vec[vector]
        if sel == SEL_STUB:
            self.default_handler(vector)
            return
        esp = self.r("esp") - 12
        frame = struct.pack("<III", self.r("eip"), self.r("cs"), self.r("eflags"))
        self.write(self.lin(self.r("ss"), esp), frame)
        self.w("esp", esp)
        self.w("eflags", self.r("eflags") & ~0x200)
        self.w("cs", sel)
        self.w("eip", off)

    def run(self, insns, keys=None):
        """Run for about `insns` instructions, firing the timer every TICK."""
        keys = list(keys or [])
        while self.insns < insns and self.exit_code is None:
            try:
                self.uc.emu_start(self.r("eip"), 0xFFFFFFFF, count=TICK)
            except UcError as e:
                print("CPU error: %s at %04X:%08X" % (e, self.r("cs"), self.r("eip")))
                return False
            except Stop:
                break
            self.insns += TICK
            if self.r("eflags") & 0x200:
                self.ticks += 1
                self.irq(8)
                if keys and self.ticks % 10 == 0:
                    self.port60 = keys.pop(0)
                    self.irq(9)
        return True

    def screenshot(self, path):
        if self.mode == 0x13:
            png(path, 320, 200, self.read(0xA0000, 64000), self.palette)
            return True
        return False


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--insns", type=float, default=2e8, help="instructions to run")
    ap.add_argument("--shot", default=os.path.join(ROOT, "build", "emu", "screen.png"))
    ap.add_argument("--trace", action="store_true")
    ap.add_argument("args", nargs="*")
    a = ap.parse_args()
    emu = Emu(a.args, trace=a.trace)
    t0 = time.time()
    emu.run(int(a.insns))
    dt = time.time() - t0
    print("%.0fM instructions in %.1f s (%.1f MIPS), exit %s, mode %02Xh, eip %04X:%08X" % (
        emu.insns / 1e6, dt, emu.insns / 1e6 / max(dt, 1e-9), emu.exit_code, emu.mode,
        emu.r("cs"), emu.r("eip")))
    for e in emu.files.log[:40]:
        print("  file", *e)
    for u in emu.unknown[:20]:
        print("  unsupported", u)
    for n in sorted(emu.notes):
        print("  note", n)
    os.makedirs(os.path.dirname(a.shot), exist_ok=True)
    if emu.screenshot(a.shot):
        print("screen:", a.shot)


if __name__ == "__main__":
    main()
