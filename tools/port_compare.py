#!/usr/bin/env python3
"""The native build against FALL.EXE itself (tools/fallemu.py), from the same classic saves.

For each save of orig/saves (one zip each) that has an emulator snapshot of it loaded
(build/emu/snap/save_NAME.snap in the main checkout):
  - the emulator runs the snapshot 10 ticks and saves the screen;
  - the native build (build/port/fall) loads the save from slot 0 of an overlay of its own
    (a few seconds of play) and saves the screen;
  - the two are compared pixel by pixel, colours within 4 of each other (the two screenshots
    turn the VGA's 6-bit levels into 8 bits differently), by bands of 25 rows.
Random effects (rain, the NPCs walking, the clouds) leave a few percent; a lower score is a
difference to look at (build/port/compare/NAME/diff.bmp: the emulator, the native, the
differing pixels in red).

usage: port_compare.py [NAME ...]          (default: every save with a snapshot)
One emulator at a time (docs: emulator memory). Takes a minute or two a save.
"""
import os
import struct
import subprocess
import sys
import zipfile
import zlib

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
MAIN = os.path.realpath(os.path.join(ROOT, "orig", ".."))     # the main checkout (orig is linked)
SNAPS = os.path.join(MAIN, "build", "emu", "snap")
GAME = os.environ.get("DAGGER_GAME") or os.path.join(MAIN, "build", "game")
OUT = os.path.join(ROOT, "build", "port", "compare")
FALL = os.path.join(ROOT, "build", "port", "fall")


def png(path):
    d = open(path, "rb").read()
    pos, idat, w, h, ct, pal = 8, b"", 0, 0, 0, None
    while pos < len(d):
        n, t = struct.unpack(">I4s", d[pos:pos + 8])
        body = d[pos + 8:pos + 8 + n]
        pos += 12 + n
        if t == b"IHDR":
            w, h, _bd, ct = struct.unpack(">IIBB", body[:10])
        elif t == b"PLTE":
            pal = [tuple(body[i:i + 3]) for i in range(0, len(body), 3)]
        elif t == b"IDAT":
            idat += body
    raw = zlib.decompress(idat)
    bpp = {2: 3, 6: 4, 3: 1}[ct]
    stride = w * bpp
    rows, prev, p = [], bytearray(stride), 0
    for _y in range(h):
        f = raw[p]
        line = bytearray(raw[p + 1:p + 1 + stride])
        p += 1 + stride
        for i in range(stride):
            a = line[i - bpp] if i >= bpp else 0
            b = prev[i]
            c = prev[i - bpp] if i >= bpp else 0
            if f == 1:
                line[i] = (line[i] + a) & 255
            elif f == 2:
                line[i] = (line[i] + b) & 255
            elif f == 3:
                line[i] = (line[i] + (a + b) // 2) & 255
            elif f == 4:
                pa, pb, pc = abs(b - c), abs(a - c), abs(a + b - 2 * c)
                line[i] = (line[i] + (a if pa <= pb and pa <= pc else b if pb <= pc else c)) & 255
        rows.append(bytes(line))
        prev = line
    px = []
    for r in rows:
        for x in range(w):
            px.append(pal[r[x]] if ct == 3 else tuple(r[x * bpp:x * bpp + 3]))
    return w, h, px


def bmp(path):
    d = open(path, "rb").read()
    off = struct.unpack_from("<I", d, 10)[0]
    w, h = struct.unpack_from("<ii", d, 18)
    bpp = struct.unpack_from("<H", d, 28)[0] // 8
    stride = (w * bpp + 3) & ~3
    px = []
    for y in (range(h - 1, -1, -1) if h > 0 else range(-h)):
        for x in range(w):
            b, g, r = d[off + y * stride + x * bpp:off + y * stride + x * bpp + 3]
            px.append((r, g, b))
    return w, abs(h), px


def write_diff(path, w, h, a, b, close):
    out = bytearray()
    for y in range(h - 1, -1, -1):
        row = bytearray()
        for src in (a, b):
            for x in range(w):
                r, g, bb = src[y * w + x]
                row += bytes((bb, g, r))
        for x in range(w):
            if close[y * w + x]:
                r, g, bb = a[y * w + x]
                row += bytes((bb // 3, g // 3, r // 3))
            else:
                row += bytes((0, 0, 255))
        out += row + bytes((-len(row)) % 4)
    hdr = struct.pack("<2sIHHI", b"BM", 54 + len(out), 0, 0, 54) + \
        struct.pack("<IiiHHIIiiII", 40, w * 3, h, 1, 24, 0, len(out), 2835, 2835, 0, 0)
    open(path, "wb").write(hdr + out)


def emulator_shot(name, out):
    shots = os.path.join(out, "emu")
    os.makedirs(shots, exist_ok=True)
    r = subprocess.run([sys.executable, os.path.join(ROOT, "tools", "fallemu.py"), "--load",
                        os.path.join(SNAPS, "save_%s.snap" % name), "--ticks", "10", "--shots",
                        shots], capture_output=True, text=True, timeout=900)
    p = os.path.join(shots, "screen.png")
    return p if r.returncode == 0 and os.path.exists(p) else None


def native_shot(name, out):
    overlay = os.path.join(out, "overlay")
    save0 = os.path.join(overlay, "SAVE0")
    os.makedirs(save0, exist_ok=True)
    with zipfile.ZipFile(os.path.join(ROOT, "orig", "saves", name + ".zip")) as z:
        for info in z.infolist():
            if not info.is_dir():
                open(os.path.join(save0, os.path.basename(info.filename)), "wb").write(z.read(info))
    # the archives unpacked from PACKED.DAT, linked from an overlay that has them, so the
    # script's timings hold (unpacking takes a few seconds)
    os.makedirs(os.path.join(overlay, "ARENA2"), exist_ok=True)
    for a in ("ARCH3D.BSA", "DAGGER.SND"):
        src = os.path.join(ROOT, "build", "port", "run_fresh", "ARENA2", a)
        dst = os.path.join(overlay, "ARENA2", a)
        if os.path.exists(src) and not os.path.exists(dst):
            os.link(src, dst)
    shot = os.path.join(out, "native.bmp")
    env = dict(os.environ, DAGGER_GAME=GAME, PORT_EXIT_AFTER="36",
               PORT_SCRIPT="22 key L; 26 click 80 28; 28 click 160 10; 33 shot %s" % shot)
    subprocess.run(["timeout", "-k", "5", "80", FALL, "--game", GAME, "--overlay", overlay],
                   env=env, capture_output=True, timeout=120)
    return shot if os.path.exists(shot) else None


def compare(name):
    out = os.path.join(OUT, name)
    os.makedirs(out, exist_ok=True)
    e = emulator_shot(name, out)
    n = native_shot(name, out)
    if not e or not n:
        return name, None, "no %s screenshot" % ("emulator" if not e else "native")
    w, h, a = png(e)
    _w, _h, b = bmp(n)
    close = [all(abs(x - y) <= 4 for x, y in zip(p, q)) for p, q in zip(a, b)]
    bands = [sum(close[y * w:(y + 25) * w]) / (25 * w) for y in range(0, h, 25)]
    write_diff(os.path.join(out, "diff.bmp"), w, h, a, b, close)
    return name, sum(close) / len(close), " ".join("%.2f" % x for x in bands)


def main():
    names = sys.argv[1:] or sorted(
        f[5:-5] for f in os.listdir(SNAPS) if f.startswith("save_") and f.endswith(".snap")
        and os.path.exists(os.path.join(ROOT, "orig", "saves", f[5:-5] + ".zip")))
    os.makedirs(OUT, exist_ok=True)
    with open(os.path.join(OUT, "report.txt"), "w") as rep:
        for name in names:
            name, score, detail = compare(name)
            line = "%-10s %s  %s" % (name, "%.3f" % score if score is not None else "  -  ", detail)
            print(line, flush=True)
            rep.write(line + "\n")
            rep.flush()


if __name__ == "__main__":
    main()
