#!/usr/bin/env python3
"""Record XnGine calls from the running game, and replay them in isolation.

A record is one call's exact entry state and everything it does:
  - its input: the 4 KB pages of memory that differ from a base snapshot (the replay's
    starting point), plus registers, descriptors and the game's exception handlers,
  - its effect: every byte that differs after the call (memory before and after, compared),
    the registers at return, and its port I/O and interrupts, with what each service returned.
Interrupts are held off while a call is recorded, so it runs as one piece. Read hooks also
note which bytes it read before writing them (a footprint of its inputs; Unicorn misses some
reads inside self-modified blocks, so replay does not depend on it), and a block hook which
code it ran: `own` is a bitmask over the function's own instructions (codemap()).

Replay restores the base snapshot and the record's pages, runs from the entry to the return
with services and port reads answered as recorded, and compares. The asm must replay itself
exactly; later the C versions must too: replay(rec, patch).

Keeping records small: records made close together share most of their pages, so a job's
file stores each distinct page once (write_records). Memory is compared up to the game's DPMI
break and a margin (memory_top: the game uses ~12 of the 64 MB). A long run drifts away from
the snapshot it started from: when a call's entry state differs from the base in more than
REBASE_PAGES pages (in practice only while the game boots), the recorder saves the machine as
a new base (build/xngine/bases/) and later records start from that.

The corpus (build/xngine/records/) is made by jobs: a starting state and the input that
drives it (jobs(): snapshots with scripted input, play-session steps, fuzz-corpus entries, a
new game from boot; targets(): direct calls of game functions that reach what is left, and
direct calls of the remaining XnGine functions themselves), one .pkl of records and one .json
summary per job. Which calls a job keeps: up to `per` per function, `gap` ticks apart; past
`floor` records a function only gets records that run instructions of it no record ran yet,
and none past `cap`. A call made while another is being recorded is part of that record, not
one of its own: a function that only ran that way gets a second pass with only it hooked.

Reading records: read_records(PKL), iter_records(DIR) or records_of(VA) (one function's)
give records with their pages inline (rec["pages"] = zlib(pickle({linear address: 4 KB
page}))), whichever of the two file formats the .pkl is in (see write_records);
replay(rec, patch) runs one.

usage: xn_record.py record --load SNAPSHOT [--ticks N] [--script ...] [--per 2] [--out DIR]
       xn_record.py jobs                       write build/xngine/jobs.jsonl (every job below)
       xn_record.py job JSON|@FILE             run one job (batch runs them in subprocesses)
       xn_record.py batch [JOBS.jsonl] [-j 3] [--redo] [--match TEXT]
       xn_record.py targets call|probe         build/xngine/targets.jsonl for what is left
       xn_record.py freeze                     copy the base snapshots into build/xngine/bases
       xn_record.py replay [DIR|FILE.pkl ...] [-j 3] [--prune] [--new]
       xn_record.py coverage                   build/xngine/coverage.md and coverage.csv
"""
import argparse
import bisect
import collections
import csv
import ctypes
import glob
import hashlib
import json
import os
import pickle
import struct
import subprocess
import sys
import time
import zlib


sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402  (first: it picks the Unicorn build)
from unicorn import UC_HOOK_BLOCK, UC_HOOK_CODE, UC_HOOK_MEM_READ, UcError  # noqa: E402

ROOT = fallemu.ROOT
LOAD = fallemu.LOAD


def _setsel_logged(self, sel, base, limit, access, _orig=fallemu.Emu.setsel):
    """Emu.setsel, and while a service runs for a record, its descriptor write too: DPMI
    calls that allocate or change selectors write the descriptor table directly, and replay
    must put those bytes back with the rest of what the service wrote."""
    _orig(self, sel, base, limit, access)
    if getattr(self, "svc_writes", None) is not None:
        a = fallemu.GDT + (sel & ~7)
        self.svc_writes.append((a, self.read(a, 8)))


fallemu.Emu.setsel = _setsel_logged

# BIOS software interrupts. fallemu logs every vector below 20h as a CPU exception, which
# replay runs for real; but int 10h (video), 15h, 16h (keyboard) and 1Ah (ticks) are services
# whose answers depend on the machine's device state (the video mode, the tick count), not
# on memory. A record with io_version 2 logs and replays them like DOS and DPMI calls.
BIOS_SERVICES = {0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x1A}
IO_VERSION = 2


def _on_int_bios(self, uc, intno, user, _orig=fallemu.Emu.on_int):
    if self.io_log is None or intno not in BIOS_SERVICES or not getattr(self, "bios_as_service", False):
        return _orig(self, uc, intno, user)
    self.io_log.append(("int", intno, self.r("eax")))
    if self.int_replay is not None:
        regs, writes = self.int_replay.pop(0)
        for r, v in zip(self.SVC_REGS, regs):
            self.w(r, v)
        for lin, data in writes:
            self.write(lin, data)
    else:
        self.svc_writes = []
        self.service(uc, intno)
        regs, writes = tuple(self.r(r) for r in self.SVC_REGS), self.svc_writes
        self.svc_writes = None
    self.io_log.append(("int-ret", regs, writes))


fallemu.Emu.on_int = _on_int_bios
XN = os.path.join(ROOT, "build", "xngine")
OUT = os.path.join(XN, "records")
BASES = os.path.join(XN, "bases")
REPLAYS = os.path.join(XN, "replay")
REGS = ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp", "eflags",
        "cs", "ds", "es", "fs", "gs", "ss")
EXIT_REGS = ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp")
FLAGS = 0x8D5           # CF PF AF ZF SF OF: what a caller may test after the call
MAX_INSNS = 4_000_000       # the footprint (reads, blocks) covers this much of a call
HARD_MAX = 60_000_000       # longer calls are noted, not recorded
REBASE_PAGES = 4096         # a new base snapshot when the entry state differs in more pages
                            # (16 MB: in practice only while the game boots and loads)
SATURATED = 40              # attempts without a kept record after which a function at its
                            # floor is no longer tried
OBJ2 = (0xC0000, 0x161568)
TRAP = 0xBBF00              # where direct calls return (fallcall.py's trap: never runs)
PAGE = 4096
# where replay() loads base snapshots: their DOS files are restored into this overlay (a
# process replaying side by side with another should have its own: XN_REPLAY_OVERLAY)
REPLAY_OVERLAY = os.environ.get("XN_REPLAY_OVERLAY",
                                os.path.join(ROOT, "build", "emu", "overlay_replay"))


def read_functions():
    """[(va, found_by)] of config/xngine_functions.csv."""
    with open(os.path.join(ROOT, "config", "xngine_functions.csv"), newline="") as f:
        return [(int(r["va"], 16), r["found_by"]) for r in csv.DictReader(f)]


def handlers():
    """Run-time entries that are real entry points (interrupt and exception handlers), not
    blocks inside other code that the static walk missed."""
    p = os.path.join(ROOT, "config", "xngine_seeds.csv")
    with open(p, newline="") as f:
        return {int(r["va"], 16) for r in csv.DictReader(f) if r["source"] != "executed"}


def xngine_functions():
    """Function entries to record: every function, and of the blocks seen only at run time
    (xngine_seeds.csv) the interrupt and exception handlers."""
    h = handlers()
    return [va for va, by in read_functions() if by != "run time" or va in h]


# ---- the code map: instructions and their functions ------------------------------------
_codemap = None


def codemap():
    """{"insns": sorted instruction starts in object 2, "sizes": their sizes, "funcs": sorted
    function entries (all 719), "own": {func: [its instructions]}}: an instruction belongs to
    the nearest entry at or below it. Built by tools/xn_disasm.py (3 s), cached in
    build/xngine/codemap.pkl."""
    global _codemap
    if _codemap is not None:
        return _codemap
    path = os.path.join(XN, "codemap.pkl")
    src = os.path.join(ROOT, "config", "xngine_functions.csv")
    if os.path.exists(path) and os.path.getmtime(path) >= os.path.getmtime(src):
        with open(path, "rb") as f:
            _codemap = pickle.load(f)
        return _codemap
    import xn_disasm
    import xn_link
    an = xn_disasm.Analysis(xn_link.Image())
    insns = sorted(an.insns)
    funcs = sorted(set(an.funcs) | {va for va, _ in read_functions()})
    own = collections.defaultdict(list)
    for a in insns:
        k = bisect.bisect_right(funcs, a) - 1
        if k >= 0:
            own[funcs[k]].append(a)
    _codemap = {"insns": insns, "sizes": [an.insns[a].size for a in insns], "funcs": funcs,
                "own": dict(own)}
    os.makedirs(XN, exist_ok=True)
    with open(path + ".tmp", "wb") as f:
        pickle.dump(_codemap, f)
    os.replace(path + ".tmp", path)
    return _codemap


_mask = None


def code_mask():
    """A bytes mask over object 2: 1 where an instruction is."""
    global _mask
    if _mask is None:
        cm = codemap()
        m = bytearray(OBJ2[1] - OBJ2[0])
        for a, n in zip(cm["insns"], cm["sizes"]):
            m[a - OBJ2[0]:a - OBJ2[0] + n] = b"\x01" * n
        _mask = bytes(m)
    return _mask


def own_mask(f, blocks):
    """The instructions of f (codemap()["own"][f]) that the executed blocks [(va, size)]
    cover, as an int bitmask (bit k: f's k-th instruction)."""
    ins = codemap()["own"].get(f, [])
    if not ins:
        return 0
    lo, hi = ins[0], ins[-1]
    m = 0
    for a, n in blocks:
        if a + n <= lo or a > hi:
            continue
        i = bisect.bisect_left(ins, a)
        while i < len(ins) and ins[i] < a + n:
            m |= 1 << i
            i += 1
    return m


def pristine_obj2():
    """Object 2 as FALL.EXE loads it (relocated to LOAD), to diff the live code against."""
    le = fallemu.LE(fallemu.EXE)
    img = le.load(relocate=True)
    o = le.objs[1]
    buf = img[2]
    for f in le.fixups():
        if o.base <= f.src_va < o.base + o.vsize:
            k = f.src_va - o.base
            if f.kind == fallemu.SRC_OFF32:
                v = struct.unpack_from("<I", buf, k)[0]
                struct.pack_into("<I", buf, k, (v + LOAD) & 0xFFFFFFFF)
            elif f.kind == fallemu.SRC_SEL16:
                struct.pack_into("<H", buf, k, fallemu.SEL_DATA)
    return bytes(buf)


# ---- memory diffs -------------------------------------------------------------------------
def memory(emu, top=None):
    """(low memory, program memory up to LOAD + top) as bytes."""
    return emu.read(0, fallemu.LOW), emu.read(LOAD, top or fallemu.MEM)


MARGIN = 8 << 20


def memory_top(emu):
    """How much program memory to compare: the DPMI break (every block the game was given
    lies below it) and a margin, in MB. Nothing in the game writes above its blocks; a call
    that did would not be seen, in the record or in its replay alike."""
    return min(fallemu.MEM, ((getattr(emu, "brk", fallemu.MEM) + MARGIN + 0xFFFFF) >> 20) << 20)


def regions(mem):
    return ((0, mem[0]), (LOAD, mem[1]))


CHUNK = 1 << 18


def changed_pages(mem, base):
    """{linear address: page bytes} where mem differs from base (compared a chunk at a time:
    most of memory is unchanged), over the length of mem."""
    out = {}
    for (origin, cur), (_o, ref) in zip(regions(mem), regions(base)):
        n = len(cur)
        for c in range(0, n, CHUNK):
            if cur[c:c + CHUNK] == ref[c:c + CHUNK]:
                continue
            for k in range(c, min(c + CHUNK, n), PAGE):
                page = cur[k:k + PAGE]
                if page != ref[k:k + PAGE]:
                    out[origin + k] = page
    return out


def changed_bytes(before, after):
    """{linear address: new byte} for every byte that differs."""
    out = {}
    for page, data in changed_pages(after, before).items():
        origin, buf = (0, before[0]) if page < LOAD else (LOAD, before[1])
        old = buf[page - origin:page - origin + PAGE]
        for j in range(0, PAGE, 64):
            if data[j:j + 64] != old[j:j + 64]:
                for i in range(j, j + 64):
                    if data[i] != old[i]:
                        out[page + i] = data[i]
    return out


_runs = None


def code_runs():
    """[(start, end)] offsets in object 2 of the runs of instruction bytes."""
    global _runs
    if _runs is None:
        m, _runs, k = code_mask(), [], 0
        while k < len(m):
            if m[k]:
                j = k
                while j < len(m) and m[j]:
                    j += 1
                _runs.append((k, j))
                k = j
            else:
                k += 1
    return _runs


def smc_state(live, pristine):
    """{va: byte} of the XnGine code bytes that differ from the file: the patched fields and
    planted rets in effect (data in object 2 is left out; it is in the pages)."""
    out = {}
    for a, b in code_runs():
        if live[a:b] != pristine[a:b]:
            for k in range(a, b):
                if live[k] != pristine[k]:
                    out[OBJ2[0] + k] = live[k]
    return out


# ---- one call -----------------------------------------------------------------------------
def call_once(emu, footprint=True, max_insns=MAX_INSNS, before=None, flush_after=True, top=None):
    """Run the call that starts at the current EIP to its return. Returns the record body:
    whether it returned, the bytes it changed, the exit registers, its port I/O and
    interrupts; with footprint, also the bytes it read before writing them and the code
    blocks it ran. before: memory(emu) now, if the caller has it. flush_after: retranslate
    the code after the call (not needed for correctness: a block translated with a hook
    that is gone calls into Unicorn and finds nothing)."""
    uc = emu.uc
    esp0 = emu.r("esp")
    ret = struct.unpack("<I", emu.read(esp0, 4))[0]
    reads, written, blocks = {}, set(), set()
    done = []
    if before is None:
        before = memory(emu, top)
    low, high = before

    # one hook each: Unicorn calls hooks of a kind newest-first, so a pair of read hooks
    # (mark, then fill in) loses bytes read only once. A byte read before the call wrote it
    # still holds its value from the start of the call.
    def on_read(uc_, access, address, size, value, _):
        for a in range(address, address + size):
            if a not in written and a not in reads:
                if a >= LOAD:
                    if a - LOAD < len(high):
                        reads[a] = high[a - LOAD]
                elif a < len(low):
                    reads[a] = low[a]

    def on_write(uc_, access, address, size, value, _):
        for a in range(address, address + size):
            written.add(a)

    def on_block(uc_, address, size, _):
        blocks.add((address, size))

    def on_ret(uc_, address, size, _):
        if uc_.reg_read(fallemu.R["esp"]) > esp0:
            done.append(True)
            uc_.emu_stop()

    from unicorn import UC_HOOK_MEM_WRITE
    hooks = [uc.hook_add(UC_HOOK_CODE, on_ret, None, ret, ret)]
    if footprint:
        hooks += [uc.hook_add(UC_HOOK_MEM_READ, on_read), uc.hook_add(UC_HOOK_MEM_WRITE, on_write),
                  uc.hook_add(UC_HOOK_BLOCK, on_block)]
    fallemu.flush_caches(uc)    # translated code and the TLB would skip new hooks
    emu.io_log = []
    n = 0
    partial = False
    try:
        while not done and n < max(max_insns, HARD_MAX):
            if n >= max_insns and footprint and not partial:
                # a long call: the rest runs without the footprint hooks (which cost a call
                # into Python per access), so that only the effects are compared
                for hk in hooks[1:]:
                    uc.hook_del(hk)
                del hooks[1:]
                fallemu.flush_caches(uc)
                partial = True
            uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=1_000_000)
            if emu.clear_exception_state():     # a fault the game's handler took
                continue
            n += 1_000_000
    finally:
        for h in hooks:
            uc.hook_del(h)
        if flush_after:
            fallemu.flush_caches(uc)
        io, emu.io_log = emu.io_log, None
    writes = changed_bytes(before, memory(emu, len(high)))
    out = {
        "returned": bool(done),
        "reads": reads,
        "writes": writes,
        "exit": {r: emu.r(r) for r in EXIT_REGS + ("eflags",)},
        "io": io,
    }
    if footprint:
        out["blocks"] = sorted((a - LOAD, n) for a, n in blocks)
        if partial:
            out["partial"] = True       # reads and blocks cover the first max_insns only
    return out


def _ff_len(b, s):
    """Length of the FF /r instruction at b[s] (opcode, ModRM, SIB, displacement)."""
    modrm = b[s + 1]
    mod, rm = modrm >> 6, modrm & 7
    if mod == 3:
        return 2
    n = 2
    if rm == 4:
        if s + 2 >= len(b):
            return 0
        n += 1
        if mod == 0 and b[s + 2] & 7 == 5:
            n += 4
    elif mod == 0 and rm == 5:
        n += 4
    return n + (1 if mod == 1 else 4 if mod == 2 else 0)


def entry_kind(emu, f):
    """How the function was entered, from the return address on the stack: "call" (a direct
    call of f), "tail" (a call of something else that jumped here), "indirect" (call through
    a register or memory), "handler" (an exception or interrupt stub frame), "other"."""
    esp = emu.r("esp")
    ret = struct.unpack("<I", emu.read(esp, 4))[0]
    if ret == fallemu.EXC_STUB or fallemu.STUBS <= ret < fallemu.STUBS + 1024:
        return "handler"
    if ret == LOAD + TRAP:
        return "direct"         # called by call_job (a direct call from the safe point)
    if not (LOAD <= ret < LOAD + fallemu.MEM) or ret < LOAD + 8:
        return "other"
    b = emu.read(ret - 7, 7)
    if b[2] == 0xE8:
        tgt = (ret + struct.unpack("<i", b[3:7])[0]) & 0xFFFFFFFF
        return "call" if tgt == LOAD + f else "tail"
    for n in range(2, 8):           # call r/m32 (FF /2) of n bytes ending at ret
        s = 7 - n
        if b[s] == 0xFF and (b[s + 1] >> 3) & 7 == 2 and _ff_len(b, s) == n:
            return "indirect"
    cs = struct.unpack("<I", emu.read(esp + 4, 4))[0]
    if cs == fallemu.SEL_CODE:      # an interrupt frame: EIP, CS, EFLAGS
        return "interrupt"
    return "other"


# ---- the recorder -------------------------------------------------------------------------
class Recorder:
    """Records XnGine calls while the machine runs (emu.run calls after_slice).

    per: records kept per function in this run; gap: ticks between attempts on a function;
    known: {func: (records in the corpus, bitmask of its instructions they ran)};
    floor: keep any record while the corpus has fewer; past it only records that run new
    instructions of the function are kept, and none past cap."""

    HALT = 0x1F00       # a hlt in low memory: entry hooks divert here to stop the CPU

    def __init__(self, emu, per=2, only=None, base=None, gap=0, known=None, floor=None,
                 cap=None, name="rec", state=None, rebase=REBASE_PAGES, max_insns=MAX_INSNS,
                 footprint=True, bases=BASES):
        self.emu, self.per, self.gap = emu, per, gap
        self.base_path = base            # the snapshot emu was loaded from
        self.base = memory(emu)
        self.top = memory_top(emu)
        self.name, self.state = name, state or name
        self.rebase, self.max_insns, self.footprint, self.bases = rebase, max_insns, footprint, bases
        self.known = known or {}
        self.floor, self.cap = floor, cap
        funcs = only or xngine_functions()
        self.full = set()       # enough records, or nothing new for a long while
        if known and floor is not None:
            own = codemap()["own"]
            for f in funcs:
                n, m, misses = (tuple(known.get(f, ())) + (0, 0, 0))[:3]
                allm = (1 << len(own.get(f, []))) - 1
                if (cap is not None and n >= cap) or (
                        n >= floor and ((allm and m & allm == allm) or misses >= SATURATED)):
                    self.full.add(f)
        self.funcs = [f for f in funcs if f not in self.full]
        self.count = collections.Counter()      # attempts
        self.kept = collections.Counter()
        self.last = {}                          # tick of the last attempt
        self.cover = {f: known.get(f, (0, 0))[1] for f in self.funcs} if known else {}
        self.records = []
        self.long = set()
        self.failed = collections.Counter()     # attempts not kept, by reason
        self.recording = False
        self.faulted = False
        self.pending = None
        self.pristine = pristine_obj2()
        self.nbase = 0
        self.store = {}                         # page key -> page: shared by the records
        self.handlers = handlers()
        emu.write(self.HALT, b"\xf4")
        emu.bios_as_service = True
        self.hooks = {}
        for f in self.funcs:
            self.hooks[f] = emu.uc.hook_add(UC_HOOK_CODE, self.on_entry, None, LOAD + f, LOAD + f)
        emu.on_stop = self.after_slice

    def max_attempts(self):
        return self.per + 2

    def on_entry(self, uc, address, size, _):
        f = address - LOAD
        if self.pending is None and not self.recording:
            if self.gap and f in self.last and self.emu.ticks < self.last[f] + self.gap:
                return
            # stop before the function's first instruction runs: a hook that moves EIP
            # skips the instruction it was called for
            self.pending = (f, address)
            uc.reg_write(fallemu.R["eip"], self.HALT)

    def drop(self, f):
        h = self.hooks.pop(f, None)
        if h is not None:
            self.emu.uc.hook_del(h)

    def new_base(self, mem):
        os.makedirs(self.bases, exist_ok=True)
        self.nbase += 1
        p = os.path.join(self.bases, "%s_%d_%d.snap" % (self.name, self.emu.ticks, self.nbase))
        self.emu.save(p + ".tmp")
        os.replace(p + ".tmp", p)
        self.base_path, self.base = p, memory(self.emu)

    def after_slice(self):
        if self.pending is None:
            return False
        (f, address), self.pending = self.pending, None
        emu = self.emu
        emu.w("eip", address)
        self.count[f] += 1
        self.last[f] = emu.ticks
        via = entry_kind(emu, f)
        if self.count[f] >= self.max_attempts():
            self.drop(f)
        if f in self.handlers and via == "other":
            self.failed["handler entered without a frame"] += 1
            return True
        if memory_top(emu) > self.top:      # the game was given more memory
            self.top = memory_top(emu)
        mem = memory(emu, self.top)
        pages = changed_pages(mem, self.base)
        if self.rebase and len(pages) > self.rebase:
            self.new_base(mem)
            pages = {}
        refs = {}
        for a, pg in pages.items():
            k = hashlib.blake2b(pg, digest_size=16).digest()
            self.store.setdefault(k, pg)
            refs[a] = k
        del pages
        live = mem[1][OBJ2[0]:OBJ2[1]]
        rec = {"func": f, "entry": {r: emu.r(r) for r in REGS}, "eip": emu.r("eip"),
               "sels": {s: tuple(v) for s, v in emu.sels.items()},
               "smc": smc_state(live, self.pristine),
               "exc": dict(emu.exc), "base": self.base_path,
               "page_refs": refs,
               "tick": emu.ticks, "via": via, "job": self.name, "state": self.state,
               "top": self.top, "io_version": IO_VERSION}
        t0 = time.time()
        self.recording = True
        try:
            rec.update(call_once(emu, self.footprint, self.max_insns, before=mem, flush_after=False))
        except (fallemu.Stop, UcError):      # the game faulted inside the call
            self.failed["fault"] += 1
            self.drop(f)
            self.faulted = True
            return True
        finally:
            self.recording = False
        rec["seconds"] = time.time() - t0
        if not rec["returned"]:
            self.long.add(f)    # ran past HARD_MAX (a main loop, a wait): not a unit
            self.drop(f)
            return True
        keep = True
        if self.footprint:
            m = own_mask(f, rec["blocks"])
            rec["own"] = m
            n = self.known.get(f, (0, 0))[0] + self.kept[f]
            if self.floor is not None and n >= self.floor and not (m & ~self.cover.get(f, 0)):
                keep = False
            if self.cap is not None and n >= self.cap:
                keep = False
            if keep:
                self.cover[f] = self.cover.get(f, 0) | m
        if keep:
            self.kept[f] += 1
            self.records.append(rec)
        else:
            self.failed["nothing new"] += 1
        if self.kept[f] >= self.per:
            self.drop(f)
        return True


# ---- record files -------------------------------------------------------------------------
# Two formats. v1 (`record`): a pickled list of records, each with "pages" = zlib(pickle({linear
# address: page})). v2 (the corpus, `job`): MAGIC + zlib(pickle({"format": 2, "pages": {key:
# page}, "records": [...]})), each record with "page_refs" = {address: key}: records made
# close together share most pages, so each is stored once per file. read_records() gives
# v1-shaped records either way.
MAGIC = b"XNR2"


def write_records(path, records, store):
    """Write records (with page_refs into store) as a v2 file, atomically."""
    used = {k for r in records for k in r["page_refs"].values()}
    data = pickle.dumps({"format": 2, "pages": {k: store[k] for k in used}, "records": records},
                        protocol=pickle.HIGHEST_PROTOCOL)
    with open(path + ".tmp", "wb") as f:
        f.write(MAGIC + zlib.compress(data, 6))
    os.replace(path + ".tmp", path)


def v1_record(rec, store):
    """A record with its pages inline (rec["pages"] = zlib(pickle({address: page})))."""
    r = dict(rec)
    refs = r.pop("page_refs")
    r["pages"] = zlib.compress(pickle.dumps({a: store[k] for a, k in refs.items()}), 1)
    return r


def read_raw(path):
    """(records with page_refs, page store) of a v2 file; (records, None) of a v1 file."""
    with open(path, "rb") as f:
        raw = f.read()
    if raw[:4] != MAGIC:
        return pickle.loads(raw), None
    obj = pickle.loads(zlib.decompress(raw[4:]))
    return obj["records"], obj["pages"]


def read_records(path):
    """The records of one .pkl (either format), each with its pages inline."""
    recs, store = read_raw(path)
    return recs if store is None else [v1_record(r, store) for r in recs]


def record_files(d):
    return [d] if d.endswith(".pkl") else sorted(glob.glob(os.path.join(d, "*.pkl")))


def iter_records(d):
    """Every record in a directory of .pkl files (or one .pkl), a file at a time."""
    for p in record_files(d):
        yield from read_records(p)


def records_of(va, out=OUT):
    """The records of one function (preferred address, e.g. 0x12A254) across the corpus,
    pages inline, reading only the files that hold some (the job summaries list each file's
    records in order)."""
    for sp in sorted(glob.glob(os.path.join(out, "*.json"))):
        try:
            sm = json.load(open(sp))
        except (OSError, ValueError):
            continue
        ks = [k for k, r in enumerate(sm.get("records", ())) if int(r["func"], 16) == va]
        if not ks:
            continue
        recs, store = read_raw(sp[:-5] + ".pkl")
        for k in ks:
            if k < len(recs) and recs[k]["func"] == va:
                yield recs[k] if store is None else v1_record(recs[k], store)


def load_records(d):
    """Every record in a directory of .pkl files (or one .pkl). The corpus is several GB of
    pages: iter_records() holds one file at a time."""
    return list(iter_records(d))


# ---- replay -------------------------------------------------------------------------------
_bases = {}


def _load_base(path, overlay=None):
    """Emu.load, tolerating DOS files the snapshot had open that are gone since (replay
    answers every DOS call from the record, so they are never read)."""
    overlay = overlay or REPLAY_OVERLAY
    try:
        return fallemu.Emu.load(path, overlay=overlay)
    except FileNotFoundError:
        with open(path, "rb") as fh:
            st = pickle.load(fh)
        f = list(st["files"])
        f[4] = {h: (name if os.path.exists(name) else os.devnull, "rb", 0)
                for h, (name, mode, pos) in f[4].items()}
        st["files"] = tuple(f)
        tmp = os.path.join(overlay + "_tmp.snap")
        with open(tmp, "wb") as fh:
            pickle.dump(st, fh)
        try:
            return fallemu.Emu.load(tmp, overlay=overlay)
        finally:
            os.remove(tmp)


def base_path(path):
    """A record's base: absolute, or (after freeze) relative to the repository."""
    return path if os.path.isabs(path) else os.path.join(ROOT, path)


def base_machine(path):
    """The snapshot a record was taken from, loaded once: (machine, its memory)."""
    path = base_path(path)
    if path not in _bases:
        for emu, _m in _bases.values():
            emu.close()
        _bases.clear()
        emu = _load_base(path)
        _bases[path] = (emu, memory(emu))
    return _bases[path]


def replay(rec, patch=None):
    """Run one record again from its exact entry state. `patch(emu)` may replace the
    function's code first (a C version). Returns a list of differences (empty: it matched)."""
    emu, (low, mem) = base_machine(rec["base"])
    emu.write(0, low)
    emu.write(LOAD, mem)
    for a, page in pickle.loads(zlib.decompress(rec["pages"])).items():
        emu.write(a, page)
    # the descriptor table itself is in low memory, restored exactly with the pages (with the
    # accessed bits the CPU set); only the emulator's own copy of it is set here
    emu.sels = {s: [b, lim, acc] for s, (b, lim, acc) in rec["sels"].items()}
    emu.exc = dict(rec.get("exc", {}))
    for r, v in rec["entry"].items():
        emu.w(r, v)
    emu.w("eip", rec["eip"])
    if patch:
        patch(emu)
    # services (DOS, BIOS, the mouse) answer as they did in the recording
    emu.bios_as_service = rec.get("io_version", 1) >= 2
    emu.int_replay = [(e[1], e[2]) for e in rec["io"] if e[0] == "int-ret"]
    emu.in_replay = [e[2] for e in rec["io"] if e[0] == "in"]   # and ports read as they were
    try:
        got = call_once(emu, footprint=False, top=rec.get("top"))
    except (IndexError, fallemu.Stop) as e:   # asked for more services than recorded; a fault
        emu.int_replay = emu.in_replay = None
        return ["stopped: %r" % e]
    emu.int_replay = emu.in_replay = None
    diffs = []
    if not got["returned"]:
        diffs.append("did not return")
    for r in EXIT_REGS:
        if got["exit"][r] != rec["exit"][r]:
            diffs.append("%s %08X != %08X" % (r, got["exit"][r], rec["exit"][r]))
    if (got["exit"]["eflags"] ^ rec["exit"]["eflags"]) & FLAGS:
        diffs.append("flags %03X != %03X" % (got["exit"]["eflags"] & FLAGS,
                                              rec["exit"]["eflags"] & FLAGS))
    if got["writes"] != rec["writes"]:
        extra = set(got["writes"]) ^ set(rec["writes"])
        wrong = [a for a in set(got["writes"]) & set(rec["writes"])
                 if got["writes"][a] != rec["writes"][a]]
        diffs.append("writes: %d addresses differ in the set, %d in value%s" % (
            len(extra), len(wrong), " (first %#x)" % min(extra | set(wrong))
            if extra or wrong else ""))
    if got["io"] != rec["io"]:
        diffs.append("port I/O or interrupts differ")
    return diffs


# ---- jobs ---------------------------------------------------------------------------------
# Input that drives a state: walking about, and each screen bound to a key in the world,
# opened, used a little and closed. Ticks from the job's start; keys are held 20 ticks.
def _keys(t, *names, hold=20):
    return "; ".join("%d down %s; %d up %s" % (t + 30 * k, n, t + 30 * k + hold, n)
                     for k, n in enumerate(names))


SCRIPTS = {
    "walk": ("2 mouse 160,100,0; 10 down up; 220 up up; 230 down left; 290 up left; "
             "300 down up; 300 down p; 480 up up; 480 up p; 490 down right; 600 up right; "
             "610 down j; 630 up j; 640 down rshift; 700 up rshift; 710 down down; 800 up down; "
             "810 down d; 830 up d; 840 down up; 960 up up; 970 down pgup; 1000 up pgup; "
             "1010 down left; 1200 up left; 1210 down up; 1400 up up", 1460),
    "swing": ("2 mouse 160,100,0; 10 down a; 30 up a; 60 mouse 160,100,2; 68 mouse 100,100,2; "
              "76 mouse 160,100,2; 84 mouse 230,100,2; 92 mouse 160,130,2; 100 mouse 160,100,0; "
              "160 mouse 160,100,2; 168 mouse 160,60,2; 176 mouse 160,140,2; 184 mouse 160,100,0; "
              "240 mouse 160,100,2; 248 mouse 230,100,2; 256 mouse 160,100,2; 264 mouse 100,100,2; "
              "272 mouse 160,100,0; 330 down up; 400 up up; 420 mouse 160,100,2; 428 mouse 100,100,2; "
              "436 mouse 160,100,0; 500 down a; 520 up a", 600),
    "automap": ("10 down m; 30 up m; 200 down up; 260 up up; 270 down left; 330 up left; "
                "340 down pgup; 380 up pgup; 390 down pgdn; 430 up pgdn; 440 down right; 500 up right; "
                "520 click 60,180; 620 click 110,180; 720 click 160,180; 820 click 200,180; "
                "920 click 240,180; 1020 mouse 100,100,0; 1030 mouse 100,100,1; 1060 mouse 140,120,1; "
                "1080 mouse 140,120,0; 1120 click 297,180; 1250 down esc; 1270 up esc", 1400),
    "travelmap": ("10 down w; 30 up w; 250 click 90,60; 450 click 200,100; 650 click 150,150; "
                  "850 down esc; 870 up esc; 1000 down esc; 1020 up esc; 1150 down esc; 1170 up esc", 1300),
    "inventory": ("10 down f6; 30 up f6; 200 click 30,4; 300 click 90,4; 400 click 150,4; "
                  "500 click 210,4; 600 click 240,42; 700 click 60,60; 800 click 240,64; "
                  "900 click 60,80; 1000 click 240,20; 1100 click 240,189; 1250 down esc; 1270 up esc", 1400),
    "sheet": ("10 down f5; 30 up f5; 200 click 80,109; 330 click 160,100; 450 click 80,119; "
              "580 click 160,100; 700 click 80,129; 830 click 160,100; 950 click 80,139; "
              "1080 click 160,100; 1200 click 70,87; 1330 click 160,100; 1450 click 70,187; "
              "1600 down esc; 1620 up esc", 1750),
    "spellbook": ("10 down bksp; 30 up bksp; 200 click 60,33; 300 click 60,41; 400 click 60,49; "
                  "500 click 132,174; 600 click 60,57; 700 click 260,174; 850 down esc; 870 up esc", 1000),
    "options": ("10 down esc; 30 up esc; 250 click 122,108; 450 click 160,100; 550 down esc; "
                "570 up esc; 700 click 150,65; 780 click 150,73; 860 click 150,81; 950 click 196,108; "
                "1100 down esc; 1120 up esc; 1300 click 160,52; 1500 down esc; 1520 up esc; "
                "1650 down esc; 1670 up esc", 1800),
    "screens": ("10 down l; 30 up l; 200 click 65,191; 320 click 187,191; 440 click 290,191; "
                "600 down esc; 620 up esc; 700 down i; 720 up i; 850 click 160,100; 950 down esc; "
                "970 up esc; 1050 down f10; 1070 up f10; 1200 down f10; 1220 up f10; "
                "1300 down t; 1320 up t; 1450 down esc; 1470 up esc; 1550 down u; 1570 up u; "
                "1700 down esc; 1720 up esc; 1800 down f4; 1820 up f4; 1830 click 165,112; "
                "1950 down esc; 1970 up esc; 2050 down f2; 2070 up f2; 2080 click 165,112; "
                "2250 click 160,60; 2400 down esc; 2420 up esc", 2500),
    "rest": ("10 down r; 30 up r; 200 click 110,70; 330 down 2; 350 up 2; 380 down enter; "
             "400 up enter; 900 click 160,80; 1000 down esc; 1020 up esc; 1100 click 160,100; "
             "1200 down esc; 1220 up esc", 1300),
}


FIGHTS = [   # save_mord: next to the Frost Daedra (build/asset_trace/exp8_fight.py), three ways
    {"name": "save_mord__fight_swing", "teleport": ["0x9022B8", 100, 0, 0], "script": "swing",
     "ticks": 1200},
    {"name": "save_mord__fight_idle", "teleport": ["0x9022B8", 100, 0, 0], "script": "",
     "ticks": 1500},
    {"name": "save_mord__fight_walk", "teleport": ["0x9022B8", 300, 0, 0], "script": "walk"},
]


OUTDOORS = ["save_tlalac_s", "save_morthag1", "save_dorian", "save_gash", "save_shadow",
            "save_wereboar", "save_kralvamp", "cheat_ming"]
DUNGEONS = ["save_mord", "save_keophex", "save_kral", "save_blades", "save_orcs", "save_uking"]


def situations():
    """Snapshot jobs with game state set first: the weather (climate_weathers 0x195E2A, all six
    climates: 0-2 clear to overcast, 3 fog, 4 rain, 5 snow, 6), dungeon water (dungeon_water_level
    0x12DE00, relative to the eye), and the time of day (game minutes 0x195BF4)."""
    out = []
    for k, nm in enumerate(OUTDOORS):
        for w in ("00", "01", "02", "03", "05", "06"):
            out.append({"name": "%s__weather%s" % (nm, w), "load": nm, "poke": [["195E2A", w * 6]],
                        "script": ("walk", "swing", "automap")[k % 3]})
        for t in (0, 330, 400, 1110, 1200):
            out.append({"name": "%s__time%04d" % (nm, t), "load": nm, "time": t, "script": "walk"})
    for nm in DUNGEONS:
        for dy in (-1000, -200, 200, 1000):
            out.append({"name": "%s__water%+d" % (nm, dy), "load": nm, "water": dy, "script": "walk"})
    for j in out:
        j.update({"kind": "snap", "state": j["load"] + ":" + j["name"].split("__")[1],
                  "load": os.path.relpath(os.path.join(fallemu.SNAPS, j["load"] + ".snap"), ROOT)})
        j["ticks"] = SCRIPTS[j["script"]][1]
    # render modes 0 (span ends: an outline view) and 4 (solid colours), which the game never
    # sets: xn_render_set_mode called at the safe point (fallcall.py's snapshots), then play
    for nm in ("save_tlalac_s", "save_wayrest", "save_mord"):
        for mode in (0, 4):
            out.append({"name": "%s__rendermode%d" % (nm, mode), "kind": "snap",
                        "load": os.path.join("build", "xngine", "bases", "safe_%s.snap" % nm),
                        "precall": ["12A254", [mode]], "script": "walk",
                        "ticks": SCRIPTS["walk"][1], "state": "%s:rendermode%d" % (nm, mode)})
    return out


def jobs():
    """Every job: (1) each snapshot in build/emu/snap with each script above (and the save_mord
    fights); (2) every step of the play sessions in build/play, replayed from the step before;
    (3) the fuzz corpus entries that ran new XnGine code, replayed from their parents; (4) a
    new game from boot (tools/scenarios/newgame.txt, in pieces). Ordered so that the first
    jobs are as different from each other as possible (the floor fills with varied
    situations): the categories take turns, snapshots go round with a different script
    each time, play sessions take turns."""
    snaps = sorted(glob.glob(os.path.join(fallemu.SNAPS, "*.snap")))
    names = list(SCRIPTS)
    snap_jobs = []
    for r in range(len(names)):
        for k, s in enumerate(snaps):
            nm, sc = os.path.basename(s)[:-5], names[(k + r) % len(names)]
            snap_jobs.append({"name": "%s__%s" % (nm, sc), "kind": "snap",
                              "load": os.path.relpath(s, ROOT), "script": sc,
                              "ticks": SCRIPTS[sc][1], "state": nm})
    for k, f in enumerate(FIGHTS):
        f = dict(f)
        load = f.pop("load", "save_mord")
        f.update({"kind": "snap", "load": os.path.relpath(os.path.join(fallemu.SNAPS, load + ".snap"), ROOT),
                  "state": load + ":fight"})
        snap_jobs.insert(3 + 7 * k, f)
    sessions = collections.OrderedDict()
    for d in sorted(glob.glob(os.path.join(ROOT, "build", "play", "*", "steps.json"))):
        sess = os.path.basename(os.path.dirname(d))
        for st in json.load(open(d)):
            n = st["n"]
            if n < 1 or not os.path.exists(os.path.join(os.path.dirname(d), "%03d.snap" % (n - 1))):
                continue
            sessions.setdefault(sess, []).append(
                {"name": "play_%s_%03d" % (sess, n), "kind": "play", "session": sess, "n": n,
                 "state": "play:%s/%03d" % (sess, n - 1)})
    play_jobs = interleave(list(sessions.values()))
    other = [{"name": "newgame_%d" % k, "kind": "boot", "segment": k, "state": "newgame:%d" % k}
             for k in range(5)]
    for p in sorted(glob.glob(os.path.join(ROOT, "build", "fuzz", "corpus_*.jsonl"))):
        for line in open(p):
            e = json.loads(line)
            if any(u.startswith("xn_") for u in e.get("units", ())):
                other.append({"name": "fuzz_%s" % e["id"], "kind": "fuzz", "id": e["id"],
                              "state": "fuzz:%s" % e["parent"]})
    return interleave([snap_jobs, play_jobs, other, situations()])


def interleave(lists):
    """One from each list in turn."""
    out, lists = [], [list(v) for v in lists]
    while any(lists):
        for v in lists:
            if v:
                out.append(v.pop(0))
    return out


def fuzz_entry(eid):
    for p in sorted(glob.glob(os.path.join(ROOT, "build", "fuzz", "corpus_*.jsonl"))):
        for line in open(p):
            e = json.loads(line)
            if e["id"] == eid:
                return e
    raise SystemExit("no fuzz entry %s" % eid)


def fuzz_snap(eid):
    if eid.startswith("seed_"):
        return os.path.join(fallemu.SNAPS, eid[5:] + ".snap")
    return os.path.join(ROOT, "build", "fuzz", "corpus", eid + ".snap")


def newgame_segments():
    """The newgame scenario cut at its `save` lines: [(name, [run lines])]."""
    path = os.path.join(ROOT, "tools", "scenarios", "newgame.txt")
    segs, cur = [], []
    for line in open(path):
        line = line.split("#", 1)[0].strip()
        if not line:
            continue
        word, _, rest = line.partition(" ")
        if word == "run":
            cur.append(rest)
        elif word == "save":
            segs.append((rest, cur))
            cur = []
    return segs


def run_lines(emu, lines):
    for rest in lines:
        n, _, events = rest.partition(":")
        start = emu.ticks
        emu.run(start + int(n), [(start + t, a, g) for t, a, g in fallemu.parse_script(events)])
        if emu.exit_code is not None:
            return


def newgame_snap(k, overlay):
    """The machine at the start of newgame segment k: k=0 a fresh boot, else the scenario's
    k-th save point (made once, without recording, in build/xngine/bases/)."""
    os.makedirs(BASES, exist_ok=True)
    p0 = os.path.join(BASES, "newgame_0.snap")
    if not os.path.exists(p0):
        emu = fallemu.Emu(overlay=overlay)
        emu.save(p0 + ".tmp")
        os.replace(p0 + ".tmp", p0)
        emu.close()
    p = os.path.join(BASES, "newgame_%d.snap" % k)
    if os.path.exists(p):
        return p
    prev = newgame_snap(k - 1, overlay)
    emu = fallemu.Emu.load(prev, overlay=overlay)
    run_lines(emu, newgame_segments()[k - 1][1])
    emu.save(p + ".tmp")
    os.replace(p + ".tmp", p)
    emu.close()
    return p


def coverage_lib():
    from unicorn.unicorn_py3 import unicorn as ucmod
    lib = ucmod.uclib
    if not hasattr(lib, "uc_dagger_coverage"):
        return None
    lib.uc_dagger_coverage.argtypes = [ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64,
                                       ctypes.c_void_p]
    return lib


def call_job(emu, rec, va, args, stack, max_ticks, regs_pattern=None):
    """Call va(args) from where the machine stands (fallcall.py's convention), recording the
    XnGine calls it makes, until it returns. regs_pattern sets the other registers first:
    "zero", "buf" (each a pointer to 16 KB of zeros on the stack below the call), "small"
    or "str" (see below)."""
    ok = _call(emu, rec, va, args, stack, max_ticks, regs_pattern, emu.uc.context_save())
    return ok


def _call(emu, rec, va, args, stack, max_ticks, regs_pattern, ctx):
    try:
        return _call_run(emu, rec, va, args, stack, max_ticks, regs_pattern)
    finally:
        emu.uc.context_restore(ctx)     # the CPU as it was: the game carries on from there


def _call_run(emu, rec, va, args, stack, max_ticks, regs_pattern):
    L = LOAD
    esp0 = emu.r("esp")
    esp = (esp0 - 0x400) & ~3
    if regs_pattern == "zero":
        for r in ("eax", "ebx", "ecx", "edx", "esi", "edi"):
            emu.w(r, 0)
    elif regs_pattern == "buf":
        buf = (esp0 - 0x8000) & ~0xFFF
        emu.write(buf, bytes(0x4000))
        for r in ("eax", "ebx", "ecx", "edx", "esi", "edi"):
            emu.w(r, buf)
    elif regs_pattern in ("small", "str"):
        # small counts, pointers into a buffer; "str": a file name there, and four stack
        # arguments (name, 0, 2, name) for the C-convention wrappers
        buf = (esp0 - 0x8000) & ~0xFFF
        emu.write(buf, (b"Z.CFG\0" if regs_pattern == "str" else b"") + bytes(0x4000))
        for r, v in (("eax", 1), ("ecx", 2), ("edx", 1), ("ebx", buf), ("esi", buf + 0x100),
                     ("edi", buf + 0x1000)):
            emu.w(r, buf if regs_pattern == "str" and r in ("eax", "edx") else v)
        if regs_pattern == "str":
            args, stack = [buf, 0, 2, buf], True
    regs = [] if stack else list(args[:4])
    onstack = list(args) if stack else list(args[4:])
    for v in reversed(onstack):
        esp -= 4
        emu.uc.mem_write(esp, struct.pack("<I", v & 0xFFFFFFFF))
    esp -= 4
    emu.uc.mem_write(esp, struct.pack("<I", L + TRAP))
    emu.w("esp", esp)
    for reg, v in zip(("eax", "edx", "ebx", "ecx"), regs):
        emu.w(reg, v & 0xFFFFFFFF)
    emu.w("eip", L + va)
    target = L + TRAP
    end = emu.ticks + max_ticks
    while emu.ticks < end and emu.exit_code is None:
        try:
            emu.uc.emu_start(emu.r("eip"), target, count=fallemu.TICK)
            while emu.r("eip") != target and emu.clear_exception_state():
                emu.uc.emu_start(emu.r("eip"), target, count=fallemu.TICK)
        except (fallemu.Stop, UcError):
            return False
        if emu.r("eip") == target:
            return True
        if rec.after_slice():
            if rec.faulted:
                return False
            continue
        emu.insns += fallemu.TICK
        if not emu.r("eflags") & 0x200:
            continue
        emu.ticks += 1
        emu.pit_reads = 0
        emu.irq(8)
    return False


def job_base(spec, overlay):
    kind = spec.get("kind", "snap")
    if kind in ("snap", "call"):
        return os.path.join(ROOT, spec["load"])
    if kind == "play":
        return os.path.join(ROOT, "build", "play", spec["session"], "%03d.snap" % (spec["n"] - 1))
    if kind == "fuzz":
        return fuzz_snap(fuzz_entry(spec["id"])["parent"])
    if kind == "boot":
        return newgame_snap(spec["segment"], overlay)
    raise SystemExit("unknown job kind %s" % kind)


def drive(emu, rec, spec):
    """Play the job's input from where the machine stands. Returns a note (or "")."""
    kind = spec.get("kind", "snap")
    start = emu.ticks
    for addr, data in spec.get("poke", ()):     # [[address, hex bytes]]: game state to set
        emu.write(LOAD + int(addr, 16), bytes.fromhex(data))
    if spec.get("time") is not None:    # the time of day, in minutes after midnight
        m = struct.unpack("<I", emu.read(LOAD + 0x195BF4, 4))[0]
        emu.write(LOAD + 0x195BF4, struct.pack("<I", m - m % 1440 + spec["time"]))
    if spec.get("water") is not None:   # dungeon water this far below the eye (world units)
        po = struct.unpack("<I", emu.read(LOAD + 0x195AA4, 4))[0]
        y = struct.unpack("<i", emu.read(po + 11, 4))[0]
        emu.write(LOAD + 0x12DE00, struct.pack("<i", y + spec["water"]))
    if spec.get("precall"):             # [function, args]: a direct call first (from a safe point)
        va, args = spec["precall"]
        call_job(emu, rec, int(va, 16), args, False, 100)
    if kind == "snap":
        if spec.get("teleport"):        # [object, dx, dy, dz]: stand next to a world object
            obj, dx, dy, dz = spec["teleport"]
            po = struct.unpack("<I", emu.read(LOAD + 0x195AA4, 4))[0]
            x, y, z = struct.unpack("<iii", emu.read(LOAD + int(obj, 16) + 7, 12))
            emu.write(po + 7, struct.pack("<iii", x + dx, y + dy, z + dz))
        text, ticks = SCRIPTS[spec["script"]] if spec.get("script") in SCRIPTS else (
            spec.get("script", ""), spec.get("ticks", 300))
        ticks = spec.get("ticks", ticks)
        emu.run(start + ticks, [(start + t, a, g) for t, a, g in fallemu.parse_script(text)])
    elif kind == "play":
        import fallplay
        st = next(x for x in fallplay.load_steps(spec["session"]) if x["n"] == spec["n"])
        argv = (fallplay.argv_of(st, spec["session"]) or [None])[0]
        note = ""
        if argv is None:
            note = "step has no command"
        else:
            a = fallplay.build_parser().parse_args([argv[0], spec["session"]] + list(argv[1:]))
            sa = fallplay.step_action(a, None)
            try:
                sa[1](emu)
            except (SystemExit, Exception) as e:    # noqa: BLE001  (a step that fails differently)
                note = "step: %r" % e
        emu.run(emu.ticks + spec.get("extra", 30), [])
        return note
    elif kind == "fuzz":
        e = fuzz_entry(spec["id"])
        if e.get("macro"):
            import fallfuzz
            fallfuzz.run_macro(emu, e["macro"])
        s = emu.ticks
        emu.run(s + e["ticks"], [(s + t, a, tuple(g) if isinstance(g, list) else g)
                                  for t, a, g in e["events"]])
    elif kind == "boot":
        segs = newgame_segments()
        if spec["segment"] < len(segs):
            run_lines(emu, segs[spec["segment"]][1])
        else:
            emu.run(emu.ticks + spec.get("ticks", 1500), [])
    elif kind == "call":
        ok = call_job(emu, rec, int(spec["va"], 16), spec.get("args", []), spec.get("stack", False),
                      spec.get("ticks", 400))
        return "returned" if ok else "did not return"
    return ""


def run_job(spec, known=None, out=OUT, overlay=None):
    """Run one job; write OUT/NAME.pkl (the records) and OUT/NAME.json (a summary).

    A call that starts while another is being recorded is not recorded (it is part of the
    outer record): a function that only ran that way (texture loading in the first frames,
    say) gets a second pass of the same input with only it hooked."""
    name = spec["name"]
    overlay = overlay or os.path.join(XN, "ov", name)
    t0 = time.time()
    base = job_base(spec, overlay)
    known = dict(known or {})
    only = [int(x, 16) for x in spec["only"]] if spec.get("only") else None
    hookable = set(xngine_functions())
    records, store, ran, notes = [], {}, set(), []
    attempts, dropped, long = collections.Counter(), collections.Counter(), set()
    nbase = nfull = ticks = 0
    exit_code, full = None, set()
    lib = coverage_lib()
    lo, hi = LOAD + OBJ2[0], LOAD + OBJ2[1]
    pass_hooks = []
    for npass in range(3 if spec.get("repass", True) else 1):
        if npass:
            only = sorted(f for f in ran & hookable if not attempts[f] and f not in long
                          and f not in full)
            if not only:
                break
        pass_hooks.append(len(only) if only else -1)
        emu = _load_base(base, overlay)
        rec = Recorder(emu, per=spec.get("per", 2), only=only, base=os.path.abspath(base),
                       gap=spec.get("gap", 15), known=known, floor=spec.get("floor", 24),
                       cap=spec.get("cap", 80), name=name + ("_p2" if npass else ""),
                       state=spec.get("state", name), max_insns=spec.get("max_insns", MAX_INSNS))
        if npass == 0:
            full = set(rec.full)
            nfull = len(full)
        if lib:
            lib.uc_dagger_coverage(emu.uc._uch, lo, hi, None)
        start = emu.ticks
        notes.append(drive(emu, rec, spec))
        if lib:
            buf = ctypes.create_string_buffer(hi - lo)
            lib.uc_dagger_coverage(emu.uc._uch, lo, hi, buf)
            ran |= {f for f in codemap()["funcs"] if OBJ2[0] <= f < OBJ2[1] and buf.raw[f - OBJ2[0]]}
        if npass == 0:
            ticks, exit_code = emu.ticks - start, emu.exit_code
        emu.close()
        records += rec.records
        store.update(rec.store)
        attempts.update(rec.count)
        dropped.update(rec.failed)
        long |= rec.long
        nbase += rec.nbase
        for r in rec.records:           # the second pass knows what the first kept
            n, m, x = (tuple(known.get(r["func"], ())) + (0, 0, 0))[:3]
            known[r["func"]] = (n + 1, m | r.get("own", 0), x)
    drop_overlay(overlay)
    os.makedirs(out, exist_ok=True)
    write_records(os.path.join(out, name + ".pkl"), records, store)
    summary = {
        "name": name, "spec": spec, "base": os.path.relpath(base, ROOT), "ticks": ticks,
        "seconds": round(time.time() - t0, 1), "exit": exit_code,
        "note": "; ".join(n for n in notes if n), "passes": len(notes), "pass_hooks": pass_hooks,
        "records": [summary_row(r) for r in records],
        "long": ["%X" % f for f in sorted(long)], "ran": ["%X" % f for f in sorted(ran)],
        "attempts": {"%X" % f: n for f, n in sorted(attempts.items())},
        "dropped": dict(dropped), "full": nfull, "bases": nbase,
        "size": os.path.getsize(os.path.join(out, name + ".pkl")),
    }
    with open(os.path.join(out, name + ".json.tmp"), "w") as f:
        json.dump(summary, f)
    os.replace(os.path.join(out, name + ".json.tmp"), os.path.join(out, name + ".json"))
    return summary


def run_probe_job(spec, known=None, out=OUT, overlay=None):
    """Direct calls of XnGine functions that play never reaches: each of spec["vas"] called
    from a fresh copy of the snapshot (a safe point: the entry of the per-frame update) with
    each register pattern in spec["patterns"] ("safe": as they are there, "zero", "buf"),
    recording the call itself. A record of such a call is still an exact test case of the
    function, if not one the game makes; they are marked via "direct"."""
    name = spec["name"]
    overlay = overlay or os.path.join(XN, "ov", name)
    t0 = time.time()
    base = os.path.join(ROOT, spec["load"])
    records, store, ran, notes = [], {}, set(), []
    attempts, dropped, long = collections.Counter(), collections.Counter(), set()
    lib = coverage_lib()
    lo, hi = LOAD + OBJ2[0], LOAD + OBJ2[1]
    for vx in spec["vas"]:
        va = int(vx, 16)
        got = 0
        for pat in spec.get("patterns", ["safe", "zero", "buf"]):
            emu = _load_base(base, overlay)
            rec = Recorder(emu, per=1, only=[va], base=os.path.abspath(base), name=name,
                           state="%s:%s" % (spec.get("state", name), pat),
                           max_insns=spec.get("max_insns", MAX_INSNS))
            if lib:
                lib.uc_dagger_coverage(emu.uc._uch, lo, hi, None)
            try:
                ok = call_job(emu, rec, va, [], False, spec.get("ticks", 30), regs_pattern=pat)
            except Exception as e:      # noqa: BLE001  (a call that wrecks the machine)
                ok, rec.faulted = False, True
                notes.append("%s/%s: %r" % (vx, pat, e))
            if lib:
                buf = ctypes.create_string_buffer(hi - lo)
                lib.uc_dagger_coverage(emu.uc._uch, lo, hi, buf)
                ran |= {f for f in codemap()["funcs"] if OBJ2[0] <= f < OBJ2[1] and buf.raw[f - OBJ2[0]]}
            emu.close()
            for r in rec.records:
                r["probe"] = pat
            records += rec.records
            store.update(rec.store)
            attempts.update(rec.count)
            dropped.update(rec.failed)
            long |= rec.long
            got += len(rec.records)
            if not ok and not rec.records:
                dropped["%s did not return" % pat] += 1
            if got >= spec.get("per", 2):
                break
    drop_overlay(overlay)
    return write_job(out, name, spec, base, records, store, ran, attempts, dropped, long, notes,
              t0, 0, None, 0, 0, 1)


def write_job(out, name, spec, base, records, store, ran, attempts, dropped, long, notes, t0,
              ticks, exit_code, nfull, nbase, passes):
    os.makedirs(out, exist_ok=True)
    write_records(os.path.join(out, name + ".pkl"), records, store)
    summary = {
        "name": name, "spec": spec, "base": os.path.relpath(base, ROOT), "ticks": ticks,
        "seconds": round(time.time() - t0, 1), "exit": exit_code,
        "note": "; ".join(n for n in notes if n)[:2000], "passes": passes,
        "records": [summary_row(r) for r in records],
        "long": ["%X" % f for f in sorted(long)], "ran": ["%X" % f for f in sorted(ran)],
        "attempts": {"%X" % f: n for f, n in sorted(attempts.items())},
        "dropped": dict(dropped), "full": nfull, "bases": nbase,
        "size": os.path.getsize(os.path.join(out, name + ".pkl")),
    }
    with open(os.path.join(out, name + ".json.tmp"), "w") as f:
        json.dump(summary, f)
    os.replace(os.path.join(out, name + ".json.tmp"), os.path.join(out, name + ".json"))
    return summary


def drop_overlay(overlay):
    """A job's DOS overlay is only needed while it runs (replay answers DOS from the record,
    and _load_base copes with a base whose open files are gone)."""
    import shutil
    if os.path.dirname(overlay) == os.path.join(XN, "ov"):
        shutil.rmtree(overlay, ignore_errors=True)


def summary_row(r):
    return {"func": "%X" % r["func"], "tick": r["tick"], "via": r["via"],
            "base": os.path.relpath(base_path(r["base"]), ROOT), "own": "%x" % r.get("own", 0),
            "pages": len(r["page_refs"]), "writes": len(r["writes"]),
            "seconds": round(r["seconds"], 3)}


# ---- freezing: the corpus keeps its own copies of the snapshots it starts from --------------
def frozen_name(path):
    """build/xngine/bases/NAME.snap for a base snapshot elsewhere (relative to ROOT)."""
    rel = os.path.relpath(base_path(path), ROOT)
    if rel.startswith(os.path.join("build", "xngine", "bases") + os.sep):
        return rel
    parts = rel.split(os.sep)
    if parts[:2] == ["build", "play"]:
        nm = "play_%s_%s" % (parts[2], parts[-1])
    elif parts[:3] == ["build", "fuzz", "corpus"]:
        nm = "fuzz_" + parts[-1]
    elif parts[:3] == ["build", "emu", "snap"]:
        nm = "snap_" + parts[-1]
    else:
        nm = "_".join(parts[1:])
    return os.path.join("build", "xngine", "bases", nm)


def freeze(out=OUT):
    """Copy every base snapshot the records use into build/xngine/bases (an APFS clone: no
    space until the original changes) and point the records there, relative to the
    repository: play sessions are rewound and the fuzz corpus pruned by their owners."""
    os.makedirs(BASES, exist_ok=True)
    nfiles = ncopied = 0
    for sp in sorted(glob.glob(os.path.join(out, "*.json"))):
        sm = json.load(open(sp))
        pkl = sp[:-5] + ".pkl"
        recs, store = read_raw(pkl)
        changed = False
        for r in recs:
            nb = frozen_name(r["base"])
            if r["base"] != nb:
                dst = os.path.join(ROOT, nb)
                if not os.path.exists(dst):
                    src = base_path(r["base"])
                    if subprocess.call(["cp", "-c", src, dst + ".tmp"],
                                       stderr=subprocess.DEVNULL) != 0:
                        import shutil
                        shutil.copyfile(src, dst + ".tmp")
                    os.replace(dst + ".tmp", dst)
                    ncopied += 1
                r["base"] = nb
                changed = True
        if not changed:
            continue
        if store is None:
            with open(pkl + ".tmp", "wb") as f:
                pickle.dump(recs, f)
            os.replace(pkl + ".tmp", pkl)
        else:
            write_records(pkl, recs, store)
        for row, r in zip(sm["records"], recs):
            row["base"] = r["base"]
        with open(sp + ".tmp", "w") as f:
            json.dump(sm, f)
        os.replace(sp + ".tmp", sp)
        nfiles += 1
    print("%d files point at their own bases now; %d snapshots copied into %s" % (
        nfiles, ncopied, BASES))


# ---- the corpus ---------------------------------------------------------------------------
def summaries(out=OUT):
    res = []
    for p in sorted(glob.glob(os.path.join(out, "*.json"))):
        try:
            res.append(json.load(open(p)))
        except (OSError, ValueError):
            pass
    return res


def known_state(out=OUT):
    """{func: (records, bitmask of its instructions they ran, attempts in jobs that kept
    none)} over the corpus."""
    k = {}
    for s in summaries(out):
        kept = collections.Counter()
        for r in s["records"]:
            f = int(r["func"], 16)
            kept[f] += 1
            n, m, x = k.get(f, (0, 0, 0))
            k[f] = (n + 1, m | int(r["own"], 16), x)
        for fx, a in s["attempts"].items():
            f = int(fx, 16)
            if not kept[f]:
                n, m, x = k.get(f, (0, 0, 0))
                k[f] = (n, m, x + a)
    return k


def batch(jobfile, j, redo=False, match=None, out=OUT):
    """Run the jobs, j at a time, each in its own process (a machine is 0.5-1.5 GB; a process
    that ends gives it all back). The corpus state (records per function, instructions run)
    is read afresh for each job."""
    import memwatch
    memwatch.start()
    specs = [json.loads(line) for line in open(jobfile) if line.strip()]
    if match:
        specs = [s for s in specs if match in s["name"]]
    if not redo:
        specs = [s for s in specs if not os.path.exists(os.path.join(out, s["name"] + ".json"))]
    print("%d jobs to run, %d at a time" % (len(specs), j), flush=True)
    os.makedirs(os.path.join(XN, "logs"), exist_ok=True)
    running = {}
    t0 = time.time()
    done = 0
    while specs or running:
        while specs and len(running) < j:
            s = specs.pop(0)
            st = os.path.join(XN, "logs", "known_%s.pkl" % s["name"])
            with open(st, "wb") as f:
                pickle.dump(known_state(out), f)
            log = open(os.path.join(XN, "logs", s["name"] + ".log"), "w")
            p = subprocess.Popen([sys.executable, os.path.abspath(__file__), "job", json.dumps(s),
                                  "--known", st, "--out", out], stdout=log, stderr=subprocess.STDOUT)
            running[p] = (s, time.time(), log, st)
        time.sleep(1)
        for p in list(running):
            s, ts, log, st = running[p]
            rc = p.poll()
            if rc is None:
                if time.time() - ts > s.get("timeout", 2400):
                    p.kill()
                    print("%s: killed after %.0f s" % (s["name"], time.time() - ts), flush=True)
                continue
            del running[p]
            log.close()
            os.remove(st)
            done += 1
            sm = os.path.join(out, s["name"] + ".json")
            if os.path.exists(sm):
                d = json.load(open(sm))
                print("[%d, %.0f min] %s: %d records of %d functions, %d ran, %.0f s%s" % (
                    done, (time.time() - t0) / 60, s["name"], len(d["records"]),
                    len({r["func"] for r in d["records"]}), len(d["ran"]), d["seconds"],
                    " (%s)" % d["note"] if d["note"] else ""), flush=True)
            else:
                print("[%d] %s: failed (exit %s), see build/xngine/logs/%s.log" % (
                    done, s["name"], rc, s["name"]), flush=True)


def replay_file(path, prune=False):
    """Replay every record of one .pkl; with prune, drop those that differ (and from the
    summary). Writes build/xngine/replay/NAME.json."""
    recs, store = read_raw(path)
    res = []
    t0 = time.time()
    for k, rec in enumerate(recs):
        try:
            diffs = replay(rec if store is None else v1_record(rec, store))
        except Exception as e:      # noqa: BLE001  (a base that does not load, ...)
            diffs = ["error: %r" % e]
        res.append(diffs)
    bad = [k for k, d in enumerate(res) if d]
    name = os.path.basename(path)[:-4]
    os.makedirs(REPLAYS, exist_ok=True)
    out = {"name": name, "records": len(recs), "ok": len(recs) - len(bad),
           "seconds": round(time.time() - t0, 1),
           "bad": [{"k": k, "func": "%X" % recs[k]["func"], "tick": recs[k]["tick"],
                    "diffs": res[k][:4]} for k in bad]}
    if prune and bad:
        keep = [r for k, r in enumerate(recs) if not res[k]]
        if store is None:
            with open(path + ".tmp", "wb") as f:
                pickle.dump(keep, f)
            os.replace(path + ".tmp", path)
        else:
            write_records(path, keep, store)
        sp = path[:-4] + ".json"
        if os.path.exists(sp):
            sm = json.load(open(sp))
            sm["records"] = [r for k, r in enumerate(sm["records"]) if not res[k]]
            sm["pruned"] = sm.get("pruned", 0) + len(bad)
            with open(sp + ".tmp", "w") as f:
                json.dump(sm, f)
            os.replace(sp + ".tmp", sp)
        out["pruned"] = len(bad)
    with open(os.path.join(REPLAYS, name + ".json"), "w") as f:
        json.dump(out, f)
    return out


def replay_many(paths, j, prune=False):
    import memwatch
    memwatch.start()
    todo = list(paths)
    running = {}
    ok = tot = 0
    while todo or running:
        while todo and len(running) < j:
            p = todo.pop(0)
            ov = os.path.join(XN, "ov", "replay_%d" % len(running))
            used = {o for (_p, o) in running.values()}
            k = 0
            while os.path.join(XN, "ov", "replay_%d" % k) in used:
                k += 1
            ov = os.path.join(XN, "ov", "replay_%d" % k)
            env = dict(os.environ, XN_REPLAY_OVERLAY=ov)
            args = [sys.executable, os.path.abspath(__file__), "replay-one", p] + (["--prune"] if prune else [])
            running[subprocess.Popen(args, env=env, stdout=subprocess.DEVNULL)] = (p, ov)
        time.sleep(0.5)
        for pr in list(running):
            if pr.poll() is None:
                continue
            p, _ov = running.pop(pr)
            rp = os.path.join(REPLAYS, os.path.basename(p)[:-4] + ".json")
            if pr.returncode != 0 or not os.path.exists(rp):
                print("%s: replay crashed (exit %s)" % (p, pr.returncode), flush=True)
                continue
            r = json.load(open(rp))
            ok += r["ok"]
            tot += r["records"]
            print("%s: %d / %d replay exactly%s" % (r["name"], r["ok"], r["records"],
                  "" if not r["bad"] else "; " + "; ".join(
                      "%s@%d %s" % (b["func"], b["tick"], ",".join(b["diffs"][:2])) for b in r["bad"][:3])),
                  flush=True)
    print("%d / %d records replay exactly" % (ok, tot))
    return ok == tot


# ---- targeted jobs for what is left -------------------------------------------------------
SAFE_SNAP = os.path.join("build", "xngine", "bases", "safe_cheat_tlalac_s.snap")


def unrecorded(out=OUT):
    """Entry points with no record yet (not the blocks inside handlers, not data)."""
    have = {int(r["func"], 16) for s in summaries(out) for r in s["records"]}
    return sorted(f for f in xngine_functions() if f not in have and f not in NOT_CODE)


def sweep_calls():
    """[(ticks, game function, args, stack, XnGine functions it ran)] of the direct calls in
    fallcall.py's sweeps (build/call*/sweep_*.jsonl, made from cheat_tlalac_s) that returned."""
    out = []
    for p in sorted(glob.glob(os.path.join(ROOT, "build", "call*", "sweep_*.jsonl"))):
        for line in open(p):
            r = json.loads(line)
            if r.get("returned"):
                ran = {int(f, 16) for f in r.get("ran", ()) if OBJ2[0] <= int(f, 16) < OBJ2[1]}
                out.append((r.get("ticks", 0), r["va"], r["args"], r.get("stack", False), ran))
    return out


def targets(kind, out=OUT):
    """build/xngine/targets.jsonl: jobs for the functions still unrecorded. kind "call": one
    job per game function whose direct call (in fallcall.py's sweeps) ran some of them,
    hooking only those; kind "probe": direct calls of each remaining function from the safe
    point, 16 functions to a job."""
    left = set(unrecorded(out))
    specs = []
    if not os.path.exists(os.path.join(ROOT, SAFE_SNAP)):
        import shutil
        os.makedirs(BASES, exist_ok=True)
        shutil.copyfile(os.path.join(ROOT, "build", "call", "safe_cheat_tlalac_s.snap"),
                        os.path.join(ROOT, SAFE_SNAP))
    if kind == "call":
        calls = sorted(sweep_calls(), key=lambda c: c[0])
        chosen = {}             # game function -> (args, stack, targets)
        need = set(left)
        # greedy: the call that reaches the most still-needed functions, cheapest first
        while need:
            best = max(calls, key=lambda c: (len(c[4] & need), -c[0]), default=None)
            if best is None or not best[4] & need:
                break
            key = (best[1], json.dumps(best[2]))
            chosen[key] = (best, sorted(best[4] & left))
            need -= best[4]
        for (va, _a), (c, tg) in chosen.items():
            specs.append({"name": "call_%s_%s" % (va, hashlib.md5(json.dumps(c[2]).encode()).hexdigest()[:6]),
                          "kind": "call", "load": SAFE_SNAP, "va": va, "args": c[2], "stack": c[3],
                          "ticks": max(60, c[0] + 60), "only": ["%X" % f for f in tg],
                          "per": 3, "floor": None, "state": "call:%s" % va})
        print("%d functions left; %d game calls reach %d of them" % (
            len(left), len(specs), len(left) - len(need)))
    else:
        vas = sorted(left)
        for k in range(0, len(vas), 16):
            specs.append({"name": "probe_%X" % vas[k], "kind": "probe", "load": SAFE_SNAP,
                          "vas": ["%X" % v for v in vas[k:k + 16]], "per": 2,
                          "state": "probe:cheat_tlalac_s"})
        print("%d functions left: %d probe jobs" % (len(left), len(specs)))
    with open(os.path.join(XN, "targets.jsonl"), "w") as f:
        for s in specs:
            f.write(json.dumps(s) + "\n")
    return specs


# ---- the coverage report ------------------------------------------------------------------
NOT_CODE = {0x157B02, 0x157E02, 0x160F00, 0x160F04, 0x160F11}   # docs/xngine_map.md: data


def xn_names():
    """{va: name} of XnGine functions from config/names.csv."""
    out = {}
    p = os.path.join(ROOT, "config", "names.csv")
    if os.path.exists(p):
        for r in csv.DictReader(open(p, newline="")):
            if r["kind"] == "func":
                va = int(r["address"], 16)
                if OBJ2[0] <= va < OBJ2[1]:
                    out[va] = r["name"]
    return out


def dead_code():
    """Functions with no caller of any kind (docs/xngine_map.md's list, by address)."""
    import re
    p = os.path.join(ROOT, "build", "names", "scratch", "xngine", "dead.md")
    if not os.path.exists(p):
        return set()
    return {int(x, 16) for x in re.findall(r"`([0-9A-F]{5,6})`", open(p).read())}


def coverage(out=OUT):
    """build/xngine/coverage.csv (va, records, states, ...) and coverage.md: per function the
    records and the states they come from, and the functions never recorded, by module and
    by why."""
    sms = summaries(out)
    cm = codemap()
    funcs = read_functions()
    names = xn_names()
    dead = dead_code()
    with open(os.path.join(ROOT, "config", "xngine_modules.csv"), newline="") as f:
        mods = [(int(r["start"], 16), int(r["end"], 16), r["name"]) for r in csv.DictReader(f)]

    def module(va):
        return next((n for s, e, n in mods if s <= va < e), "?")
    ev = {}
    evp = os.path.join(ROOT, "build", "evidence", "functions.csv")
    if os.path.exists(evp):
        for r in csv.DictReader(open(evp, newline="")):
            if r["kind"] == "xngine":
                ev[int(r["va"], 16)] = r
    recs = collections.defaultdict(list)
    ran = collections.defaultdict(set)
    long = collections.defaultdict(set)
    probed = set()
    jobs_of = collections.defaultdict(set)
    kinds = collections.Counter()
    for s in sms:
        kind = s["spec"].get("kind", "snap")
        kinds[kind] += 1
        st = s["spec"].get("state", s["name"])
        for r in s["records"]:
            recs[int(r["func"], 16)].append((st, kind, int(r["own"], 16), r["via"]))
            jobs_of[int(r["func"], 16)].add(s["name"])
        for f in s["ran"]:
            ran[int(f, 16)].add(kind)
        for f in s["long"]:
            long[int(f, 16)].add(s["name"])
        if kind == "probe":
            probed |= {int(v, 16) for v in s["spec"]["vas"]}
    rp = {}
    for p in glob.glob(os.path.join(REPLAYS, "*.json")):
        r = json.load(open(p))
        rp[r["name"]] = r
    rep_ok = sum(r["ok"] for r in rp.values())
    rep_tot = sum(r["records"] for r in rp.values())
    rep_pruned = sum(r.get("pruned", 0) for r in rp.values())
    h = handlers()
    rows = []
    for va, by in funcs:
        n_ins = len(cm["own"].get(va, []))
        rs = recs.get(va, [])
        m = 0
        for _s, _k, o, _v in rs:
            m |= o
        states = sorted({s for s, _k, _o, _v in rs})
        game = [x for x in rs if x[1] not in ("probe", "call")]
        called = [x for x in rs if x[1] == "call"]
        if game:
            why = "recorded in play"
        elif called:
            why = "recorded under a direct call of a game function"
        elif rs:
            why = "recorded by a direct call of the function only (synthetic registers)"
        elif va in NOT_CODE:
            why = "not code (data the function map took for an entry)"
        elif by == "run time" and va not in h:
            why = "a block inside a handler (found at run time); its handler's records run it"
        elif va in long:
            why = "too long: did not return within %dM instructions (a wait on the timer, a key or a port, or a loop given a huge count)" % (HARD_MAX // 1_000_000)
        elif ran.get(va) and ran[va] - {"probe"}:
            why = "ran, but never entered as a call (reached by a jump or only inside another recorded call)"
        elif va in probed:
            why = "never reached; a direct call faulted or did not return"
        else:
            e = ev.get(va)
            if e and e["episodes"] != "0":
                why = "never reached here (ran in other play episodes)"
            else:
                why = "never reached"
        if va in dead and not rs:
            why += " [dead code: no caller]"
        rows.append({"va": "0x%08X" % va, "name": names.get(va, ""), "module": module(va),
                     "found_by": by, "records": len(rs), "game_records": len(game),
                     "call_records": len(called),
                     "direct_records": len(rs) - len(game) - len(called), "jobs": len(jobs_of.get(va, ())),
                     "states": len(states),
                     "state_list": " ".join(states[:12]) + (" +%d" % (len(states) - 12) if len(states) > 12 else ""),
                     "kinds": " ".join(sorted({k for _s, k, _o, _v in rs})),
                     "via": " ".join(sorted({v for _s, _k, _o, v in rs})),
                     "insns": n_ins, "insns_run": bin(m).count("1"),
                     "dead": int(va in dead), "status": why})
    with open(os.path.join(XN, "coverage.csv"), "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=list(rows[0]), lineterminator="\n")
        w.writeheader()
        w.writerows(rows)
    # the markdown report
    nrec = sum(r["records"] for r in rows)
    recd = [r for r in rows if r["records"]]
    gamed = [r for r in rows if r["game_records"]]
    calld = [r for r in rows if r["call_records"] and not r["game_records"]]
    size = sum(os.path.getsize(p) for p in glob.glob(os.path.join(out, "*.pkl")))
    bases = sorted({r["base"] for s in sms for r in s["records"]})
    bsize = sum(os.path.getsize(os.path.join(ROOT, b)) for b in bases if os.path.exists(os.path.join(ROOT, b)))
    ins_tot = sum(r["insns"] for r in rows)
    ins_run = sum(r["insns_run"] for r in rows)
    inside = sum(1 for r in rows if r["status"].startswith("a block inside"))
    notcode = sum(1 for r in rows if r["status"].startswith("not code"))
    nstates = len({s for v in recs.values() for s, _k, _o, _v in v})
    L = ["# XnGine record corpus: coverage", "",
         "Made by `tools/xn_record.py` (`jobs`, `batch`, `replay`, `coverage`). Each record is one "
         "call of an XnGine function with its exact entry state and effects; `replay(rec, patch)` "
         "runs it again (`iter_records(DIR)` reads them, a file at a time).", "",
         "| | |", "|---|---|",
         "| jobs | %d (%s) |" % (len(sms), ", ".join("%s %d" % kv for kv in sorted(kinds.items()))),
         "| records | %d in %d files, %.2f GB; from %d distinct states; %d base snapshots (%.2f GB) |" % (
             nrec, len(sms), size / 1e9, nstates, len(bases), bsize / 1e9),
         "| functions recorded | **%d / %d**: %d in play, %d more under direct calls of game "
         "functions, %d more by direct calls of the function itself (synthetic registers) |" % (
             len(recd), len(rows), len(gamed), len(calld), len(recd) - len(gamed) - len(calld)),
         "| not recordable as calls | %d blocks inside handlers (found at run time), %d not code |" % (
             inside, notcode),
         "| instructions run by records of their own function | %d / %d (%.0f%%) |" % (
             ins_run, ins_tot, 100.0 * ins_run / max(1, ins_tot)),
         "| replay | %d / %d records replay exactly%s |" % (
             rep_ok, rep_tot, " (%d that did not were pruned)" % rep_pruned if rep_pruned else ""),
         ""]
    L += ["## Using the corpus", "",
          "- `build/xngine/records/JOB.pkl` holds a job's records (v2 format: pages stored once per "
          "file), `JOB.json` its summary (spec, records in file order, functions that ran, attempts).",
          "- `xn_record.records_of(0x12A254)` yields one function's records; `iter_records(DIR)` all, "
          "a file at a time; each has `pages` inline as in a v1 record. `replay(rec, patch)` returns "
          "the differences (empty: exact).",
          "- A record's `base` is a snapshot in build/xngine/bases (after `freeze`: the corpus "
          "keeps its own copies), relative to the repository; `via` says how the function was "
          "entered (call, indirect, tail, interrupt, handler, direct); `own` is a bitmask over "
          "the function's instructions (`codemap()[\"own\"][va]`) that the call ran; `smc` the "
          "code bytes patched at entry; `reads` the bytes read before written; `io_version` 2 "
          "means BIOS interrupts (10h, 15h, 16h, 1Ah) are answered from the record too; `probe` "
          "marks a direct call made with synthetic registers (`safe`: as at the frame-loop safe point; "
          "`zero`; `buf`, `small`, `str`: pointers into a buffer, small counts, a file name).",
          "- Records made by a direct call (`via` = direct) test the function on a state the game "
          "never gives it; the rest come from play.", ""]
    L += ["## By module", "",
          "| module | functions | recorded (play / game call / probe) | records | instructions run |",
          "|---|---|---|---|---|"]
    bym = collections.OrderedDict()
    for r in rows:
        bym.setdefault(r["module"], []).append(r)
    for mname, rs in bym.items():
        g = sum(1 for r in rs if r["game_records"])
        c = sum(1 for r in rs if r["call_records"] and not r["game_records"])
        d = sum(1 for r in rs if r["records"] and not r["game_records"] and not r["call_records"])
        L.append("| %s | %d | %d (%d / %d / %d) | %d | %d / %d |" % (
            mname, len(rs), g + c + d, g, c, d, sum(r["records"] for r in rs),
            sum(r["insns_run"] for r in rs), sum(r["insns"] for r in rs)))
    L += ["", "## Not recorded, by why", ""]
    byw = collections.OrderedDict()
    for r in rows:
        if not r["records"]:
            byw.setdefault(r["status"], []).append(r)
    for wname, rs in sorted(byw.items(), key=lambda kv: -len(kv[1])):
        L += ["### %s (%d)" % (wname, len(rs)), ""]
        bm = collections.OrderedDict()
        for r in rs:
            bm.setdefault(r["module"], []).append("%s %s" % (r["va"][4:], r["name"]) if r["name"] else r["va"][4:])
        for mname, vs in bm.items():
            L.append("- %s: %s" % (mname, ", ".join(vs)))
        L.append("")
    L += ["## Per function", "",
          "Records in play / by direct call, the jobs (situations: a state and the input played "
          "from it) and the starting states they come from, the share of the "
          "function's own instructions they run, and how it was entered (call, indirect, tail: a "
          "jump from another function, interrupt, handler, direct).", "",
          "| va | name | module | records | jobs | states | instructions run | via | starting states |",
          "|---|---|---|---|---|---|---|---|---|"]
    for r in rows:
        if r["records"]:
            L.append("| %s | %s | %s | %d / %d | %d | %d | %d / %d | %s | %s |" % (
                r["va"][4:], r["name"], r["module"], r["game_records"],
                r["call_records"] + r["direct_records"],
                r["jobs"], r["states"], r["insns_run"], r["insns"], r["via"], r["state_list"]))
    L += ["", "## Regenerating", "",
          "    .venv/bin/python tools/xn_record.py jobs          # build/xngine/jobs.jsonl",
          "    .venv/bin/python tools/xn_record.py batch -j 3    # records/, one .pkl + .json per job",
          "    .venv/bin/python tools/xn_record.py targets call  # game calls that reach what is left",
          "    .venv/bin/python tools/xn_record.py batch build/xngine/targets.jsonl -j 3",
          "    .venv/bin/python tools/xn_record.py targets probe # direct calls of the rest",
          "    .venv/bin/python tools/xn_record.py batch build/xngine/targets.jsonl -j 3",
          "    .venv/bin/python tools/xn_record.py freeze        # bases into build/xngine/bases",
          "    .venv/bin/python tools/xn_record.py replay -j 3 --prune",
          "    .venv/bin/python tools/xn_record.py coverage", "",
          "build/xngine/targets_probe2.jsonl was a second probe round with the `small` and `str` "
          "register patterns for what the first left. The new-game jobs start from snapshots in "
          "build/xngine/bases/newgame_*.snap (made once from tools/scenarios/newgame.txt; they "
          "hold files open in build/xngine/ov/newgame_prep, which must stay).", ""]
    with open(os.path.join(XN, "coverage.md"), "w") as f:
        f.write("\n".join(L))
    print("%d records of %d / %d functions (%d in play, %d under game calls); %d / %d replayed "
          "exactly; %s" % (nrec, len(recd), len(rows), len(gamed), len(calld), rep_ok, rep_tot,
                           os.path.join(XN, "coverage.md")))


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    r = sub.add_parser("record")
    r.add_argument("--load", required=True)
    r.add_argument("--ticks", type=int, default=300)
    r.add_argument("--script", default="")
    r.add_argument("--per", type=int, default=2, help="calls to record per function")
    r.add_argument("--gap", type=int, default=0)
    r.add_argument("--only", default="", help="functions to record: ADDR,ADDR,...")
    r.add_argument("--out", default=OUT)
    sub.add_parser("jobs")
    jb = sub.add_parser("job")
    jb.add_argument("spec")
    jb.add_argument("--known")
    jb.add_argument("--out", default=OUT)
    b = sub.add_parser("batch")
    b.add_argument("jobs", nargs="?", default=os.path.join(XN, "jobs.jsonl"))
    b.add_argument("-j", type=int, default=3)
    b.add_argument("--redo", action="store_true")
    b.add_argument("--match")
    b.add_argument("--out", default=OUT)
    p = sub.add_parser("replay")
    p.add_argument("paths", nargs="*", default=[OUT])
    p.add_argument("-j", type=int, default=0, help="worker processes (0: replay here, in order)")
    p.add_argument("--prune", action="store_true")
    p.add_argument("--new", action="store_true",
                   help="only files with no replay result newer than them")
    p1 = sub.add_parser("replay-one")
    p1.add_argument("path")
    p1.add_argument("--prune", action="store_true")
    fz = sub.add_parser("freeze")
    fz.add_argument("--out", default=OUT)
    t = sub.add_parser("targets")
    t.add_argument("kind", choices=("call", "probe"))
    t.add_argument("--out", default=OUT)
    c = sub.add_parser("coverage")
    c.add_argument("--out", default=OUT)
    a = ap.parse_args()
    if a.cmd == "record":
        emu = fallemu.Emu.load(a.load, overlay=os.path.join(ROOT, "build", "emu", "overlay_record"))
        only = [int(x, 16) for x in a.only.split(",")] if a.only else None
        name = os.path.splitext(os.path.basename(a.load))[0]
        rec = Recorder(emu, a.per, only=only, base=os.path.abspath(a.load), gap=a.gap, name=name)
        start = emu.ticks
        t0 = time.time()
        emu.run(start + a.ticks,
                [(start + t, x, g) for t, x, g in fallemu.parse_script(a.script)])
        os.makedirs(a.out, exist_ok=True)
        with open(os.path.join(a.out, name + ".pkl"), "wb") as f:     # v1: pages inline
            pickle.dump([v1_record(r, rec.store) for r in rec.records], f)
        funcs = {x["func"] for x in rec.records}
        print("%d calls of %d functions recorded in %.0f s -> %s; %d ran too long: %s" % (
            len(rec.records), len(funcs), time.time() - t0, a.out, len(rec.long),
            " ".join("%06X" % f for f in sorted(rec.long))))
        return 0
    if a.cmd == "jobs":
        js = jobs()
        os.makedirs(XN, exist_ok=True)
        with open(os.path.join(XN, "jobs.jsonl"), "w") as f:
            for s in js:
                f.write(json.dumps(s) + "\n")
        print("%d jobs -> %s: %s" % (len(js), os.path.join(XN, "jobs.jsonl"), ", ".join(
            "%s %d" % kv for kv in sorted(collections.Counter(s["kind"] for s in js).items()))))
        return 0
    if a.cmd == "job":
        spec = json.loads(open(a.spec[1:]).read() if a.spec.startswith("@") else a.spec)
        known = None
        if a.known:
            with open(a.known, "rb") as f:
                known = pickle.load(f)
        s = (run_probe_job if spec.get("kind") == "probe" else run_job)(spec, known, a.out)
        print("%s: %d records of %d functions, %d ran, %d too long, %.0f s, %d new bases, %s" % (
            s["name"], len(s["records"]), len({r["func"] for r in s["records"]}), len(s["ran"]),
            len(s["long"]), s["seconds"], s["bases"], s["dropped"]))
        return 0
    if a.cmd == "batch":
        batch(a.jobs, a.j, a.redo, a.match, a.out)
        return 0
    if a.cmd == "replay-one":
        r = replay_file(a.path, a.prune)
        print("%s: %d / %d replay exactly" % (r["name"], r["ok"], r["records"]))
        return 0
    if a.cmd == "freeze":
        freeze(a.out)
        return 0
    if a.cmd == "targets":
        targets(a.kind, a.out)
        return 0
    if a.cmd == "coverage":
        coverage(a.out)
        return 0
    paths = []
    for p in a.paths:
        paths += [p] if p.endswith(".pkl") else sorted(glob.glob(os.path.join(p, "*.pkl")))
    if a.new:
        def fresh(p):
            rp = os.path.join(REPLAYS, os.path.basename(p)[:-4] + ".json")
            return os.path.exists(rp) and os.path.getmtime(rp) >= os.path.getmtime(p)
        paths = [p for p in paths if not fresh(p)]
    if a.j:
        return 0 if replay_many(paths, a.j, a.prune) else 1
    ok = bad = 0
    for p in paths:
        for rec in read_records(p):
            diffs = replay(rec)
            if diffs:
                bad += 1
                print("func_%08X (tick %d): %s" % (rec["func"], rec["tick"], "; ".join(diffs[:3])))
            else:
                ok += 1
    print("%d / %d records replay exactly" % (ok, ok + bad))
    return 1 if bad else 0


if __name__ == "__main__":
    sys.exit(main())
