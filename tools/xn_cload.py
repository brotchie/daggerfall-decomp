#!/usr/bin/env python3
"""Build, load and test XnGine's C translation (see tools/xn_c.py).

  build: compile src/xngine_c/ with Watcom C32 10.0a (tools/wcc10.py, one DOSBox-X session,
         objects in build/xngine/c_work/obj) and link the OMF objects into one image at
         CBASE: code, then data, then BSS (build/xngine/c_work/image.pkl: bytes, symbols).
  load:  map CBASE..CBASE+CSIZE in a machine (the emulator's own memory stops at
         LOAD + MEM, so the C, its data and its stack are never part of a record's
         compared memory), write the image, give it a stack, and write one stub per C function:
         `mov [xn_target], f; jmp xn_thunk`.
  route: send asm entries to C: a code hook at each function's entry moves EIP to its stub
         (the asm bytes stay as they are, so the self-modifying code patches them as usual).
  test:  replay records (tools/xn_record.py replay) with the function routed to its C.
"""
import collections
import csv
import glob
import json
import os
import pickle
import shutil
import struct
import sys
import time
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
SRC = os.path.join(ROOT, "src", "xngine_c")
WORK = os.path.join(ROOT, "build", "xngine", "c_work")
IMAGE = os.path.join(WORK, "image.pkl")
REPORT = os.path.join(ROOT, "build", "xngine", "c_report.csv")
CBASE = 0x10000000
CSIZE = 0x01000000          # 16 MB: image, stubs, C stack
STACK_TOP = CBASE + CSIZE - 0x100
FLAGS = "-mf -4r -s -zl -zld -ox -bt=dos -w1 -zq".split()


# --------------------------------------------------------------------------------------------
# OMF objects

class Obj:
    """The parts of an OMF-386 object a flat link needs."""

    def __init__(self, path):
        self.path = path
        self.lnames = [None]
        self.segs = [None]          # (name, class, length)
        self.groups = [None]
        self.exts = [None]          # names
        self.pubs = {}              # name -> (seg, offset), global
        self.lpubs = {}             # name -> (seg, offset), local to this object
        self.comdefs = {}           # name -> size (communal data)
        self.data = {}
        self.fixups = []            # (seg, offset, loc, selfrel, tmethod, tindex, disp)
        self.parse(open(path, "rb").read())

    @staticmethod
    def idx(b, p):
        v = b[p]
        if v & 0x80:
            return ((v & 0x7F) << 8) | b[p + 1], p + 2
        return v, p + 1

    def parse(self, d):
        p = 0
        last = (None, 0)
        threads = {}
        while p < len(d):
            typ = d[p]
            ln = struct.unpack_from("<H", d, p + 1)[0]
            body = d[p + 3:p + 3 + ln - 1]
            p += 3 + ln
            big = typ & 1
            t = typ & ~1
            W = 4 if big else 2
            WF = "<I" if big else "<H"
            if t == 0x96:
                q = 0
                while q < len(body):
                    n = body[q]
                    self.lnames.append(body[q + 1:q + 1 + n].decode("latin1"))
                    q += 1 + n
            elif t == 0x98:
                attr = body[0]
                q = 1
                if (attr >> 5) == 0:
                    q += 3
                length = struct.unpack_from(WF, body, q)[0]
                q += W
                ni, q = self.idx(body, q)
                ci, q = self.idx(body, q)
                if attr & 2 and not big:
                    length = 0x10000
                self.segs.append((self.lnames[ni], self.lnames[ci], length))
                self.data[len(self.segs) - 1] = bytearray(length)
            elif t == 0x9A:
                ni, _ = self.idx(body, 0)
                self.groups.append(self.lnames[ni])
            elif t in (0x8C, 0xB4):
                q = 0
                while q < len(body):
                    n = body[q]
                    self.exts.append(body[q + 1:q + 1 + n].decode("latin1"))
                    q += 1 + n
                    _, q = self.idx(body, q)
            elif t in (0xB0, 0xB8):         # COMDEF / LCOMDEF
                q = 0
                while q < len(body):
                    n = body[q]
                    name = body[q + 1:q + 1 + n].decode("latin1")
                    q += 1 + n
                    _, q = self.idx(body, q)
                    dt = body[q]
                    q += 1
                    vals = []
                    for _k in range(2 if dt == 0x61 else 1):
                        lb = body[q]
                        q += 1
                        if lb <= 0x80:
                            v = lb
                        elif lb == 0x81:
                            v = struct.unpack_from("<H", body, q)[0]
                            q += 2
                        elif lb == 0x84:
                            v = body[q] | body[q + 1] << 8 | body[q + 2] << 16
                            q += 3
                        else:
                            v = struct.unpack_from("<I", body, q)[0]
                            q += 4
                        vals.append(v)
                    size = vals[0] * vals[1] if len(vals) == 2 else vals[0]
                    self.exts.append(name)
                    self.comdefs[name] = size
            elif t in (0x90, 0xB6):
                gi, q = self.idx(body, 0)
                si, q = self.idx(body, q)
                if si == 0:
                    q += 2
                while q < len(body):
                    n = body[q]
                    name = body[q + 1:q + 1 + n].decode("latin1")
                    q += 1 + n
                    off = struct.unpack_from(WF, body, q)[0]
                    q += W
                    _, q = self.idx(body, q)
                    (self.pubs if t == 0x90 else self.lpubs)[name] = (si, off)
            elif t == 0xA0:
                si, q = self.idx(body, 0)
                off = struct.unpack_from(WF, body, q)[0]
                q += W
                chunk = body[q:]
                buf = self.data[si]
                if off + len(chunk) > len(buf):
                    buf.extend(b"\0" * (off + len(chunk) - len(buf)))
                buf[off:off + len(chunk)] = chunk
                last = (si, off)
            elif t == 0xA2:
                si, q = self.idx(body, 0)
                off = struct.unpack_from(WF, body, q)[0]
                q += W
                out = bytearray()
                while q < len(body):
                    blk, q = self.lidata(body, q, big)
                    out += blk
                self.data[si][off:off + len(out)] = out
                last = (si, off)
            elif t == 0x9C:
                q = 0
                while q < len(body):
                    b0 = body[q]
                    if not b0 & 0x80:           # THREAD
                        method = (b0 >> 2) & 7
                        idx = None
                        q += 1
                        if b0 & 0x40:
                            if method < 3:
                                idx, q = self.idx(body, q)
                        elif method & 3 < 3:
                            idx, q = self.idx(body, q)
                        threads[("F" if b0 & 0x40 else "T", b0 & 3)] = (method, idx)
                        continue
                    locat = (b0 << 8) | body[q + 1]
                    q += 2
                    loc = (locat >> 10) & 0xF
                    off = locat & 0x3FF
                    selfrel = not (b0 & 0x40)
                    fixdat = body[q]
                    q += 1
                    if not fixdat & 0x80:
                        fm = (fixdat >> 4) & 7
                        if fm < 3:
                            _, q = self.idx(body, q)
                    if fixdat & 0x08:
                        tm, tidx = threads[("T", fixdat & 3)]
                        tm = tm & 3
                    else:
                        tm = fixdat & 3
                        tidx, q = self.idx(body, q)
                    disp = 0
                    if not fixdat & 0x04:
                        disp = struct.unpack_from(WF, body, q)[0]
                        q += W
                    self.fixups.append((last[0], last[1] + off, loc, selfrel, tm, tidx, disp))

    def lidata(self, b, q, big):
        rep = struct.unpack_from("<I" if big else "<H", b, q)[0]
        q += 4 if big else 2
        cnt = struct.unpack_from("<H", b, q)[0]
        q += 2
        if cnt == 0:
            n = b[q]
            content = bytes(b[q + 1:q + 1 + n])
            q += 1 + n
        else:
            content = bytearray()
            for _ in range(cnt):
                blk, q = self.lidata(b, q, big)
                content += blk
        return bytes(content) * rep, q


def kind(cls):
    c = (cls or "").upper()
    if "CODE" in c:
        return 0
    if "BSS" in c or "STACK" in c:
        return 2
    return 1


def link(paths, base=CBASE):
    """Lay out the objects (code, data, BSS) from base and resolve fixups. Returns (image
    bytes, {global symbol: address})."""
    objs = [Obj(p) for p in paths]
    addr = {}       # (obj index, seg index) -> address
    pos = base
    for k in (0, 1, 2):
        for oi, o in enumerate(objs):
            for si in range(1, len(o.segs)):
                name, cls, length = o.segs[si]
                if kind(cls) != k or not length:
                    if kind(cls) == k:
                        addr[(oi, si)] = pos
                    continue
                pos = (pos + 15) & ~15
                addr[(oi, si)] = pos
                pos += length
    # communal data, in BSS
    commons = {}
    for o in objs:
        for name, size in o.comdefs.items():
            if name not in commons and not any(name in x.pubs for x in objs):
                pos = (pos + 15) & ~15
                commons[name] = pos
                pos += size
    end = (pos + 15) & ~15
    syms = {}
    for oi, o in enumerate(objs):
        for name, (si, off) in o.pubs.items():
            if name in syms:
                raise SystemExit("duplicate symbol %s (%s)" % (name, o.path))
            syms[name] = addr[(oi, si)] + off
    syms.update(commons)
    img = bytearray(end - base)
    for oi, o in enumerate(objs):
        for si in range(1, len(o.segs)):
            if kind(o.segs[si][1]) == 2:
                continue
            a = addr[(oi, si)] - base
            data = o.data[si]
            img[a:a + len(data)] = data
    missing = set()
    for oi, o in enumerate(objs):
        for si, off, loc, selfrel, tm, tidx, disp in o.fixups:
            if tm == 0:
                tgt = addr[(oi, tidx)]
            elif tm == 1:
                g = o.groups[tidx]
                if g != "FLAT":
                    raise SystemExit("fixup to group %s in %s" % (g, o.path))
                tgt = 0
            elif tm == 2:
                name = o.exts[tidx]
                if name in o.lpubs:
                    lsi, loff = o.lpubs[name]
                    tgt = addr[(oi, lsi)] + loff
                elif name in syms:
                    tgt = syms[name]
                else:
                    missing.add(name)
                    continue
            else:
                raise SystemExit("fixup target method %d in %s" % (tm, o.path))
            at = addr[(oi, si)] + off - base
            if loc in (9, 13):          # 32-bit offset
                v = struct.unpack_from("<I", img, at)[0]
                v += tgt + disp
                if selfrel:
                    v -= base + at + 4
                struct.pack_into("<I", img, at, v & 0xFFFFFFFF)
            elif loc == 2 and not selfrel:  # a selector: the flat data selector
                struct.pack_into("<H", img, at, 0x10)
            else:
                raise SystemExit("fixup location %d in %s" % (loc, o.path))
    if missing:
        raise SystemExit("undefined: " + " ".join(sorted(missing)))
    return bytes(img), syms


# --------------------------------------------------------------------------------------------
# Build

def sources():
    gen = sorted(glob.glob(os.path.join(SRC, "xn_*.c")))
    over = sorted(glob.glob(os.path.join(SRC, "override", "xn_*.c")))
    return gen + over + [os.path.join(SRC, "runtime.c"), os.path.join(SRC, "xn_rt.asm")]


def build(flags=None, srcs=None, image=IMAGE, jobs=4):
    import wcc10
    srcs = srcs or sources()
    flags = flags.split() if isinstance(flags, str) else (flags or FLAGS)
    from concurrent.futures import ThreadPoolExecutor
    t0 = time.time()
    # several DOSBox-X sessions side by side, the biggest files spread across them
    n = max(1, min(jobs, len(srcs)))
    groups = [[] for _ in range(n)]
    sizes = [0] * n
    for src in sorted(srcs, key=lambda p: -os.path.getsize(p)):
        k = sizes.index(min(sizes))
        groups[k].append(src)
        sizes[k] += os.path.getsize(src)

    def one(k):
        objdir = os.path.join(WORK, "obj%d" % k)
        shutil.rmtree(objdir, ignore_errors=True)
        os.makedirs(objdir)
        shutil.copyfile(os.path.join(SRC, "runtime.h"), os.path.join(objdir, "runtime.h"))
        objs, td = wcc10.compile_many(groups[k], flags, workdir=objdir)
        errs = {}
        for j, src in enumerate(groups[k]):
            if objs[src] is None:
                e = os.path.join(td, "N%04d.ERR" % j)
                errs[src] = open(e, errors="replace").read() if os.path.exists(e) else ""
        return objs, errs
    objs, errs = {}, {}
    with ThreadPoolExecutor(n) as ex:
        for o, e in ex.map(one, range(n)):
            objs.update(o)
            errs.update(e)
    t1 = time.time()
    failed = [(src, errs[src]) for src in srcs if objs[src] is None]
    for s, msg in failed:
        print("FAILED", os.path.relpath(s, ROOT))
        print("\n".join(msg.strip().splitlines()[-12:]))
    good = [objs[s] for s in srcs if objs[s]]
    img, syms = link(good)
    funcs = {}
    for name, a in syms.items():
        if name.startswith("xn_") and name.endswith("_") and len(name) == 12:
            try:
                funcs[int(name[3:11], 16)] = a
            except ValueError:
                pass
    compiled = {}
    for k, s in enumerate(srcs):
        compiled[os.path.basename(s)] = objs[s] is not None
    with open(image, "wb") as f:
        pickle.dump({"base": CBASE, "bytes": img, "syms": syms, "funcs": funcs,
                     "compiled": compiled, "flags": flags}, f)
    print("compiled %d of %d files in %.0f s; image %d bytes, %d functions -> %s" % (
        len(good), len(srcs), t1 - t0, len(img), len(funcs), os.path.relpath(image, ROOT)))
    return 1 if failed else 0


# --------------------------------------------------------------------------------------------
# Load into a machine

class CImage:
    def __init__(self, path=IMAGE):
        with open(path, "rb") as f:
            d = pickle.load(f)
        self.__dict__.update(d)
        self.stubs = {}
        self.hits = 0
        self.entered = set()        # functions whose C ran (since the caller last cleared it)
        pos = CBASE + ((len(self.bytes) + 0xFFF) & ~0xFFF)
        s = self.syms
        stub_bytes = bytearray()
        self.stub_base = pos
        R = s["_R"]

        def prologue():
            """SAVE_CPU and ENTER_FLAGS of xn_rt.asm, as bytes: the registers to R, then EFLAGS
            to R with interrupts off (pushed on the asm stack, the dead word put back)."""
            b = bytearray(b"\xA3" + struct.pack("<I", R))
            for k, modrm in enumerate((0x0D, 0x15, 0x1D, 0x25, 0x2D, 0x35, 0x3D)):
                b += bytes((0x89, modrm)) + struct.pack("<I", R + 4 + 4 * k)
            for sreg, off in ((0, 36), (1, 40), (2, 44), (3, 48), (4, 52), (5, 56)):
                b += bytes((0x66, 0x8C, 0x05 | sreg << 3)) + struct.pack("<I", R + off)
            b += b"\x8B\x44\x24\xFC\x9C\xFA\x8F\x05" + struct.pack("<I", R + 32)
            b += b"\x89\x44\x24\xFC"
            return b

        def stub(var, value, to):
            """The prologue, then (interrupts off) var = value, then jmp to."""
            a = pos + len(stub_bytes)
            b = prologue() + b"\xC7\x05" + struct.pack("<II", s[var], value)
            b += b"\xE9" + struct.pack("<i", s[to] - (a + len(b) + 5))
            stub_bytes.extend(b + b"\x90" * (-len(b) % 16))
            return a
        for va in sorted(self.funcs):
            self.stubs[va] = stub("_xn_target", self.funcs[va], "xn_thunk_c")
        # return trampolines, one per xn_call depth: xn_back_k = k, then xn_back_c
        self.tramps = [stub("_xn_back_k", k, "xn_back_c") for k in range(256)]
        self.stub_bytes = bytes(stub_bytes)
        self.backtab = struct.pack("<256I", *self.tramps)

    def install(self, emu):
        """Map the C region (once per machine) and write the image, stubs and stack."""
        uc = emu.uc
        if not getattr(emu, "_xn_c_mapped", False):
            uc.mem_map(CBASE, CSIZE)
            emu._xn_c_mapped = True
        if getattr(emu, "_xn_c_image", None) is not self:
            uc.mem_write(CBASE, self.bytes)
            uc.mem_write(self.stub_base, self.stub_bytes)
            emu._xn_c_image = self
        else:
            # the data and BSS may hold anything from the last run: put the image back
            uc.mem_write(CBASE, self.bytes)
        uc.mem_write(self.syms["_xn_cesp"], struct.pack("<I", STACK_TOP))
        uc.mem_write(self.syms["_xn_depth"], struct.pack("<I", 0))
        uc.mem_write(self.syms["_xn_backtab"], self.backtab)

    def route(self, emu, funcs):
        """Send each function's asm entry to its C. Returns the hooks (remove with unroute)."""
        from unicorn import UC_HOOK_CODE
        import fallemu
        hooks = []
        eip = fallemu.R["eip"]
        for va in funcs:
            stub = self.stubs.get(va)
            if stub is None:
                continue

            def hit(uc, address, size, stub, va=va):
                self.hits += 1
                self.entered.add(va)
                uc.reg_write(eip, stub)
            a = fallemu.LOAD + va
            hooks.append(emu.uc.hook_add(UC_HOOK_CODE, hit, stub, a, a))
        return hooks

    @staticmethod
    def unroute(emu, hooks):
        for hk in hooks:
            emu.uc.hook_del(hk)


# --------------------------------------------------------------------------------------------
# Test

def record_files(dirs=None):
    dirs = dirs or [os.path.join(ROOT, "build", "xngine", "records")]
    out = []
    for d in dirs:
        if d.endswith(".pkl"):
            out.append(d)
        else:
            out += sorted(glob.glob(os.path.join(d, "*.pkl")))
    return out


def longer_calls(xn_record, n=200_000_000):
    """A translated call runs more instructions than the asm one: raise replay's limit (the
    default of call_once's max_insns, which replay() leaves at its default)."""
    import inspect
    f = xn_record.call_once
    names = [p.name for p in inspect.signature(f).parameters.values()
             if p.default is not inspect.Parameter.empty]
    d = list(f.__defaults__)
    d[names.index("max_insns")] = n
    f.__defaults__ = tuple(d)


def read_records(xn_record, path):
    """The records of one file, each with its pages inline, one at a time."""
    if hasattr(xn_record, "read_raw"):
        try:
            recs, store = xn_record.read_raw(path)
        except (OSError, EOFError, ValueError, zlib.error) as e:   # the corpus is being rewritten
            print("skipped %s: %r" % (os.path.basename(path), e))
            return
        for r in recs:
            yield r if store is None else xn_record.v1_record(r, store)
        return
    with open(path, "rb") as f:
        yield from pickle.load(f)


def machine_of(xn_record, base):
    """The cached replay machine of a record's base, or None."""
    key = xn_record.base_path(base) if hasattr(xn_record, "base_path") else base
    m = xn_record._bases.get(key)
    return m[0] if m else None


def replay_c(xn_record, img, rec, route):
    """Replay a record with the functions in `route` sent to their C; the differences."""
    hooks = []

    def patch(emu):
        img.install(emu)
        hooks.extend(img.route(emu, route))
    try:
        diffs = xn_record.replay(rec, patch=patch)
    except Exception as e:      # noqa: BLE001  (an emulator error: a fault in the C)
        emu = machine_of(xn_record, rec["base"])
        diffs = ["error: %r at eip %08X" % (e, emu.r("eip") if emu else 0)]
    finally:
        emu = machine_of(xn_record, rec["base"])
        if emu is not None:
            CImage.unroute(emu, hooks)
    if diffs and diffs[0].startswith("error"):
        drop_machine(xn_record, rec["base"])
    if diffs and diffs[0] == "did not return" and not getattr(xn_record, "_xn_c_long", False):
        # a long asm call (a timeout loop of 16M steps) takes the C several times as many
        # instructions: once more, with a much higher limit
        longer_calls(xn_record, 3_000_000_000)
        xn_record._xn_c_long = True
        try:
            diffs = replay_c(xn_record, img, rec, route)
        finally:
            longer_calls(xn_record)
            xn_record._xn_c_long = False
    return diffs


def test_files(files, funcs=None, all_c=False, max_per=0, verbose=False):
    """Replay the records in these files; {va: [passed, total, [diffs...]]} and C entries."""
    import xn_record
    longer_calls(xn_record)
    img = CImage()
    stats = {}
    ran = {}
    every = sorted(img.funcs)
    seen = collections.Counter()

    def source():
        if funcs and files is None and hasattr(xn_record, "records_of"):
            for f in sorted(funcs):     # only the files that hold these functions' records
                yield from xn_record.records_of(f)
            return
        for path in files or record_files(None):
            yield from read_records(xn_record, path)
    for rec in source():
        va = rec["func"]
        if funcs and va not in funcs or va not in img.funcs:
            continue
        if max_per and seen[va] >= max_per:
            continue
        seen[va] += 1
        img.entered = set()
        diffs = replay_c(xn_record, img, rec, every if all_c else [va])
        if all_c:
            # every function whose C ran inside this record: passed or failed with it
            for f in img.entered:
                e = ran.setdefault(f, [0, 0])
                e[0 if not diffs else 1] += 1
        st = stats.setdefault(va, [0, 0, []])
        st[1] += 1
        if not diffs:
            st[0] += 1
        else:
            if len(st[2]) < 3:
                st[2].append(diffs)
            if verbose:
                print("FAIL %06X: %s" % (va, "; ".join(diffs[:4])), flush=True)
    return stats, img.hits, ran


def test(funcs=None, dirs=None, all_c=False, max_per=0, verbose=False, jobs=1):
    """Replay every record (of `funcs`) with the function in C (all_c: every function in C),
    in `jobs` worker processes; write the report."""
    files = record_files(dirs)
    if not files:
        print("no records")
        return 1
    t0 = time.time()
    if funcs and not dirs and not all_c:
        jobs, files = 1, None       # a few functions: their own records (records_of)
    if jobs <= 1:
        stats, hits, ran = test_files(files, funcs, all_c, max_per, verbose)
    else:
        import subprocess
        import tempfile
        n = len(files)      # contiguous runs of files: a job's records share their bases
        groups = [files[k * n // jobs:(k + 1) * n // jobs] for k in range(jobs)]
        procs = []
        for k, g in enumerate(groups):
            spec = {"files": g, "funcs": funcs, "all_c": all_c, "max_per": max_per,
                    "verbose": verbose}
            fd, out = tempfile.mkstemp(prefix="xn_c_test_", suffix=".json", dir=WORK)
            os.close(fd)
            env = dict(os.environ, XN_REPLAY_OVERLAY=os.path.join(
                ROOT, "build", "emu", "overlay_replay_c%d" % k))
            procs.append((subprocess.Popen(
                [sys.executable, os.path.abspath(__file__), "worker", json.dumps(spec), out],
                env=env), out))
        stats, hits, ran = {}, 0, {}
        for pr, out in procs:
            pr.wait()
            with open(out) as f:
                d = json.load(f)
            os.remove(out)
            hits += d["hits"]
            for k, v in d["ran"].items():
                e = ran.setdefault(int(k), [0, 0])
                e[0] += v[0]
                e[1] += v[1]
            for k, v in d["stats"].items():
                st = stats.setdefault(int(k), [0, 0, []])
                st[0] += v[0]
                st[1] += v[1]
                st[2] += v[2]
    dt = time.time() - t0
    print("C entered %d times" % hits)
    write_report(stats, mode="allc" if all_c else "records", ran=ran)
    npass = sum(1 for s in stats.values() if s[0] == s[1])
    print("%d / %d functions pass all their records (%d / %d records) in %.0f s" % (
        npass, len(stats), sum(s[0] for s in stats.values()),
        sum(s[1] for s in stats.values()), dt))
    for va, s in sorted(stats.items()):
        if s[0] != s[1]:
            print("  %06X %d/%d  %s" % (va, s[0], s[1], "; ".join(s[2][0][:3])))
    return 0


def worker_main(spec, out):
    sp = json.loads(spec)
    funcs = set(sp["funcs"]) if sp["funcs"] else None
    stats, hits, ran = test_files(sp["files"], funcs, sp["all_c"], sp["max_per"], sp["verbose"])
    with open(out, "w") as f:
        json.dump({"stats": stats, "hits": hits, "ran": ran}, f)


COLUMNS = ("function", "name", "module", "instructions", "translated", "asm_fallbacks",
           "compiled", "records_passed", "records_total", "first_difference", "allc_passed",
           "allc_total", "allc_ran_in_passing", "allc_ran_in_failing", "diff_passed",
           "diff_total", "coverage_status")


def write_report(stats, path=REPORT, mode="records", ran=None):
    """build/xngine/c_report.csv, one row per function: translated, compiled, its records
    passed (the function alone in C; with --all-c, every function in C: allc_*), and the
    differential tests (made-up states)."""
    import xn_c
    tr = {}
    p = os.path.join(WORK, "translate.json")
    if os.path.exists(p):
        tr = {int(k, 16): v for k, v in json.load(open(p)).items()}
    compiled = {}
    if os.path.exists(IMAGE):
        with open(IMAGE, "rb") as f:
            compiled = pickle.load(f)["funcs"]
    diff = json.load(open(DIFF)) if os.path.exists(DIFF) else {}
    names = xn_c.load_names()
    cover = {}      # the record corpus' own account of each function (tools/xn_record.py)
    cp = os.path.join(ROOT, "build", "xngine", "coverage.csv")
    if os.path.exists(cp):
        with open(cp, newline="") as f:
            for row in csv.DictReader(f):
                cover[int(row["va"], 16)] = row["status"]
    old = {}
    if os.path.exists(path):
        with open(path, newline="") as f:
            for r in csv.DictReader(f):
                old[int(r["function"], 16)] = r
    rows = []
    for va, _k in xn_c.functions():
        t = tr.get(va, {})
        r = {k: old.get(va, {}).get(k, "") for k in COLUMNS}
        r.update({"function": "%06X" % va, "name": names.get(va, ("",))[0],
                  "module": t.get("module", ""), "instructions": t.get("insns", ""),
                  "translated": "yes" if t else "no",
                  "asm_fallbacks": len(t.get("unsupported", [])),
                  "compiled": "yes" if va in compiled else "no"})
        if "%06X" % va in diff:
            d = diff["%06X" % va]
            r["diff_passed"], r["diff_total"] = d[0], d[1]
        if va in cover:
            r["coverage_status"] = cover[va]
        if ran is not None and mode == "allc":
            e = ran.get(va, [0, 0])
            r["allc_ran_in_passing"], r["allc_ran_in_failing"] = e[0], e[1]
        if va in stats:
            st = stats[va]
            if mode == "records":
                r["records_passed"], r["records_total"] = st[0], st[1]
                r["first_difference"] = "; ".join(st[2][0][:2]) if st[2] else ""
            else:
                r["allc_passed"], r["allc_total"] = st[0], st[1]
        rows.append(r)
    with open(path, "w", newline="") as f:
        w = csv.DictWriter(f, fieldnames=COLUMNS)
        w.writeheader()
        w.writerows(rows)


# --------------------------------------------------------------------------------------------
# Differential tests: asm and C from the same made-up entry state

TRAP = 0xBBF00          # page padding after object 1's code: a return address nothing runs


def drop_machine(xn_record, base):
    """Forget a cached replay machine (it is loaded again when next needed)."""
    key = xn_record.base_path(base) if hasattr(xn_record, "base_path") else base
    m = xn_record._bases.pop(key, None)
    if m is not None:
        m[0].close()


def synthetic(xn_record, src, func, regs):
    """Run func in asm from src's machine state with the given registers and a return to TRAP
    (no services: a call that makes one is not used). Returns a record of it, or None."""
    import fallemu
    pages = pickle.loads(zlib.decompress(src["pages"]))
    esp = (src["entry"]["esp"] - 0x200) & ~3
    page = esp & ~0xFFF
    if page not in pages:
        emu, (low, mem) = xn_record.base_machine(src["base"])
        buf = low if page < fallemu.LOAD else mem
        o = page if page < fallemu.LOAD else page - fallemu.LOAD
        pages[page] = bytes(buf[o:o + 0x1000])
    pg = bytearray(pages[page])
    struct.pack_into("<I", pg, esp - page, fallemu.LOAD + TRAP)
    pages[page] = bytes(pg)
    entry = dict(src["entry"])
    entry.update(regs)
    entry["esp"] = esp
    rec = {"func": func, "base": src["base"], "sels": src["sels"], "exc": src.get("exc", {}),
           "entry": entry, "eip": fallemu.LOAD + func, "tick": src.get("tick", 0),
           "pages": zlib.compress(pickle.dumps(pages), 1)}
    for k in ("io_version", "top"):
        if k in src:
            rec[k] = src[k]
    emu, (low, mem) = xn_record.base_machine(rec["base"])
    try:
        emu.write(0, low)
        emu.write(fallemu.LOAD, mem)
        for a, pgb in pages.items():
            emu.write(a, pgb)
        emu.sels = {s: [b, lim, acc] for s, (b, lim, acc) in rec["sels"].items()}
        emu.exc = dict(rec["exc"])
        for r, v in entry.items():
            emu.w(r, v)
        emu.w("eip", rec["eip"])
    except Exception:           # noqa: BLE001  (a machine a fault left unusable)
        drop_machine(xn_record, rec["base"])
        return None
    emu.bios_as_service = rec.get("io_version", 1) >= 2
    emu.int_replay = []         # a service call stops it
    emu.in_replay = None
    try:
        got = xn_record.call_once(emu, footprint=False, max_insns=2_000_000)
    except Exception:           # noqa: BLE001  (a service, a fault, unmapped memory)
        return None
    finally:
        emu.int_replay = emu.in_replay = None
    if not got["returned"]:
        return None
    rec.update(exit=got["exit"], writes=got["writes"], io=got["io"])
    return rec


def diff_test(funcs=None, dirs=None, trials=8, verbose=False, seed=1, nbases=2, per_base=6):
    """For each function: up to `trials` made-up entry states (registers from records of other
    functions, small numbers, random values) on the machine state of a record; each one whose
    asm call returns becomes a record, which the C must then replay exactly. The states come
    from `nbases` base snapshots (`per_base` records each), a base at a time: loading a base
    is the slow part."""
    import random
    import fallemu
    import xn_c
    import xn_record
    longer_calls(xn_record)
    img = CImage()
    prog = xn_c.Program()
    rnd = random.Random(seed)
    by_base = collections.OrderedDict()
    files = record_files(dirs)
    rnd.shuffle(files)
    for path in files:
        for rec in read_records(xn_record, path):
            b = rec["base"]
            if b not in by_base and len(by_base) >= nbases:
                break
            lst = by_base.setdefault(b, [])
            if len(lst) < per_base:
                lst.append(rec)
        if len(by_base) >= nbases and all(len(v) >= per_base for v in by_base.values()):
            break
    if not by_base:
        print("no records")
        return 1
    regs_pool = [r["entry"] for v in by_base.values() for r in v]
    funcs = [va for va in (funcs or sorted(img.funcs)) if va in img.funcs]
    res = {va: [0, 0, ""] for va in funcs}
    t0 = time.time()
    each = max(1, trials // len(by_base))
    for bi, (base, srcs) in enumerate(by_base.items()):
        print("base %d/%d: %s (%d functions)" % (bi + 1, len(by_base), os.path.basename(base),
                                                len(funcs)), flush=True)
        for n, va in enumerate(funcs):
            r = res[va]
            tried = 0
            fn = xn_c.Func(prog, va)
            body = {fallemu.LOAD + x.va + k for x in fn.insns.values() for k in range(x.i.size)}
            for t in range(each * 3):
                if tried >= each:
                    break
                src = srcs[(t + va) % len(srcs)]
                other = regs_pool[rnd.randrange(len(regs_pool))]
                regs = {}
                for reg in ("eax", "ebx", "ecx", "edx", "esi", "edi", "ebp"):
                    c = rnd.random() if t else 0.0
                    regs[reg] = other[reg] if c < 0.6 else rnd.randrange(64) if c < 0.8 else \
                        rnd.getrandbits(32)
                try:
                    rec = synthetic(xn_record, src, va, regs)
                    if rec is None or xn_record.replay(rec):
                        continue    # no record, or the asm does not replay itself: no test
                    if any(a in body for a in rec["writes"]):
                        continue    # it overwrote its own code: the asm ran something else
                except Exception:   # noqa: BLE001
                    drop_machine(xn_record, src["base"])
                    continue
                tried += 1
                diffs = replay_c(xn_record, img, rec, [va])
                r[1] += 1
                if not diffs:
                    r[0] += 1
                elif not r[2]:
                    r[2] = "; ".join(diffs[:2])
                    os.makedirs(os.path.join(WORK, "fail"), exist_ok=True)
                    with open(os.path.join(WORK, "fail", "diff_%06X.pkl" % va), "wb") as f:
                        pickle.dump([rec], f)
                    if verbose:
                        print("DIFF %06X: %s" % (va, r[2]), flush=True)
            if n % 50 == 49:
                print("  %d functions, %.0f s" % (n + 1, time.time() - t0), flush=True)
                write_diff_report({k: tuple(v) for k, v in res.items() if v[1]})
    write_diff_report({k: tuple(v) for k, v in res.items()})
    n = sum(1 for v in res.values() if v[1])
    good = sum(1 for v in res.values() if v[1] and v[0] == v[1])
    print("%d / %d functions agree on every made-up state (%d of %d functions had a state "
          "that returned) in %.0f s" % (good, n, n, len(res), time.time() - t0))
    for va, (o, t, f) in sorted(res.items()):
        if t and o != t:
            print("  %06X %d/%d  %s" % (va, o, t, f))
    return 0


DIFF = os.path.join(WORK, "diff.json")


def write_diff_report(res):
    old = {}
    if os.path.exists(DIFF):
        old = json.load(open(DIFF))
    for va, v in res.items():
        old["%06X" % va] = v
    with open(DIFF, "w") as f:
        json.dump(old, f, indent=0)
    write_report({})


# --------------------------------------------------------------------------------------------
# The game itself with XnGine in C

def play(snap, ticks, all_c=True, shot=None, funcs=None, script=""):
    """Run the game from a snapshot for `ticks` timer ticks with every translated function (or
    `funcs`) in C, and save the screen. The C runs with interrupts off and takes more
    instructions than the asm, so the run is not tick-for-tick the asm's: compare screens."""
    import fallemu
    path = snap if os.path.exists(snap) else os.path.join(fallemu.SNAPS, snap + ".snap")
    emu = fallemu.Emu.load(path, overlay=os.path.join(WORK, "overlay_play"))
    try:
        img = None
        if all_c or funcs:
            img = CImage()
            img.install(emu)
            img.route(emu, funcs or sorted(img.funcs))
            # The emulator delivers interrupts between slices, wherever the CPU stopped. The
            # runtime's ways in and out of C run a few instructions with interrupts on (saving
            # the registers before `cli`; `sti; jmp [xn_leave_to]`), and an interrupt there
            # whose handler calls into C would overwrite R or the jump target: step the CPU out
            # of the runtime first, and if that turns interrupts off, deliver later.
            irq, pending = emu.irq, []

            def irq_outside_runtime(vector):
                pending.append(vector)
                for _ in range(64):
                    if not CBASE <= emu.r("eip") < CBASE + CSIZE:
                        break
                    if not emu.r("eflags") & 0x200:
                        return      # into C: the next tick delivers it
                    emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=1)
                if not emu.r("eflags") & 0x200:
                    return
                for v in pending:
                    irq(v)
                del pending[:]
            emu.irq = irq_outside_runtime
        start, t0 = emu.ticks, time.time()
        ok = emu.run(start + ticks, [(start + t, a, g) for t, a, g in fallemu.parse_script(script)])
        print("%s: %d ticks %s in %.0f s%s; C entered %d times" % (
            os.path.basename(path), emu.ticks - start, "with XnGine in C" if img else "(asm)",
            time.time() - t0, "" if ok else " (stopped: CPU error)", img.hits if img else 0))
        if shot:
            emu.screenshot(shot)
            print("screen ->", shot)
        return 0 if ok else 1
    finally:
        emu.close()


if __name__ == "__main__":
    if len(sys.argv) == 4 and sys.argv[1] == "worker":
        worker_main(sys.argv[2], sys.argv[3])
