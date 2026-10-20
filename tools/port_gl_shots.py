#!/usr/bin/env python3
"""The native build's OpenGL renderer (port/include/port_gl.h) against its software renderer,
from the classic saves (orig/saves/*.zip).

For each save, build/port/fall loads it from slot 0 of an overlay of its own twice, once as
it is and once with --gl, runs a script of keys after the load (turning, walking), and saves
the screen at the same moments. The pairs go to build/port/glshots/NAME/: sw_T.png (the
software screen, 320x200 scaled up to the GL shot's size as the window shows it) and
gl_T.png (the OpenGL window). The GL run also saves the software screen of the same frame
(gl_T_sw.bmp, PORT_GL_SW_SHOTS): the score is the share of the 3D window's pixels where the
GL picture, sampled at the 320x200 pixels' centres, has the software pixel's colour (within 4
a component), over the GL run's shots. GL renders at a higher resolution with exact
perspective, so edges and texel boundaries differ by a pixel here and there: 0.85 and up is
the same picture.

--aspect 16:9,21:9 runs the GL side once per shape, in a window of that shape with the view
filling it (PORT_GL_ASPECT=fill: the wide pass, docs/port.md "Widescreen"). The game's screen
is the 4:3 column in the middle of the window, scored as above; the columns beside it are
scored against the wide frame the software renderer drew (gl_T_swwide.bmp), sampled the same
way ("sides"). The shots are compared as the game drew them: smooth motion is off
(PORT_GL_INTERP=off) unless --interp.

usage: port_gl_shots.py [--jobs N] [--keys SCRIPT] [--aspect A,B] [--interp] [NAME ...]
       (default: every save, 4:3)
"""
import argparse
import os
import shutil
import struct
import subprocess
import sys
import zipfile
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GAME = os.environ.get("DAGGER_GAME") or os.path.join(ROOT, "build", "game")
OUT = os.path.join(ROOT, "build", "port", "glshots")
FALL = os.path.join(os.environ.get("PORT_BUILD") or os.path.join(ROOT, "build", "port"), "fall")
PACKED = os.path.join(ROOT, "build", "emu", "packed")

# after the load (28 s): look around, step forward
DEFAULT_SHOTS = [34, 37, 40]
DEFAULT_KEYS = "35 down Left; 35.5 up Left; 38 down Up; 39 up Up"
# the window for each shape (points; 720 tall)
WINDOWS = {"4:3": (960, 720), "16:10": (1152, 720), "16:9": (1280, 720), "21:9": (1680, 720),
           "32:9": (2560, 720)}


def overlay_for(name, out, tag):
    overlay = os.path.join(out, "overlay_" + tag)
    save0 = os.path.join(overlay, "SAVE0")
    if os.path.exists(overlay):
        shutil.rmtree(overlay)
    os.makedirs(save0)
    with zipfile.ZipFile(os.path.join(ROOT, "orig", "saves", name + ".zip")) as z:
        for info in z.infolist():
            if not info.is_dir():
                open(os.path.join(save0, os.path.basename(info.filename)), "wb").write(
                    z.read(info))
    os.makedirs(os.path.join(overlay, "ARENA2"), exist_ok=True)
    for a in ("ARCH3D.BSA", "DAGGER.SND"):
        src = os.path.join(PACKED, a)
        if os.path.exists(src):
            os.link(src, os.path.join(overlay, "ARENA2", a))
    return overlay


def run(name, out, gl, shots, keys, aspect="4:3", interp=False):
    tag = ("gl" if aspect == "4:3" else "gl" + aspect.replace(":", "x")) if gl else "sw"
    overlay = overlay_for(name, out, tag)
    paths = [os.path.join(out, "%s_%d.bmp" % (tag, t)) for t in shots]
    script = "22 key L; 26 click 80 28; 28 click 160 10; " + keys + "; " + \
        "; ".join("%d shot %s" % (t, p) for t, p in zip(shots, paths))
    env = dict(os.environ, DAGGER_GAME=GAME, PORT_EXIT_AFTER=str(max(shots) + 2),
               PORT_SCRIPT=script)
    if gl:
        env["PORT_GL"] = "1"
        env["PORT_GL_SW_SHOTS"] = "1"
        env["PORT_GL_INTERP"] = "on" if interp else "off"
        env["PORT_WINDOW"] = "%dx%d" % WINDOWS[aspect]
        env["PORT_GL_ASPECT"] = "4:3" if aspect == "4:3" else "fill"
    try:
        r = subprocess.run([FALL, "--game", GAME, "--overlay", overlay, "--nosound"], env=env,
                           capture_output=True, text=True, timeout=max(shots) + 60)
        log = r.stdout + r.stderr
    except subprocess.TimeoutExpired as e:
        # a run that never ended: its output so far, and the run counted as no shots
        log = ((e.stdout or b"").decode(errors="replace") + (e.stderr or b"").decode(
            errors="replace") + "\nport_gl_shots: the run did not end in time\n")
        print("%s %s: the run did not end in time" % (name, tag), file=sys.stderr, flush=True)
    with open(os.path.join(out, tag + ".log"), "w") as f:
        f.write(log)
    shutil.rmtree(overlay, ignore_errors=True)
    return [p for p in paths if os.path.exists(p)]


def read_bmp(path):
    d = open(path, "rb").read()
    off = struct.unpack_from("<I", d, 10)[0]
    w, h = struct.unpack_from("<ii", d, 18)
    bpp = struct.unpack_from("<H", d, 28)[0] // 8
    stride = (w * bpp + 3) & ~3
    return d, off, w, h, bpp, stride


def pixel(img, x, y):
    d, off, w, h, bpp, stride = img
    row = (h - 1 - y) if h > 0 else y
    p = off + row * stride + x * bpp
    return d[p + 2], d[p + 1], d[p]


def same(a, b):
    return abs(a[0] - b[0]) <= 4 and abs(a[1] - b[1]) <= 4 and abs(a[2] - b[2]) <= 4


def score(gl_path, sw_path, box=(0, 0, 320, 145)):
    """the share of pixels in box (the world view above the HUD) where GL, sampled at the
    pixel's centre, matches software. The game's screen is the 4:3 column in the middle of a
    wider shot, the shot's full height."""
    g, s = read_bmp(gl_path), read_bmp(sw_path)
    gw, gh = g[2], abs(g[3])
    cw = min(gw, round(gh * 4 / 3))
    cx = (gw - cw) // 2
    ok = n = 0
    x0, y0, x1, y1 = box
    for y in range(y0, y1):
        gy = min(gh - 1, int((y + 0.5) * gh / 200))
        for x in range(x0, x1):
            gx = cx + min(cw - 1, int((x + 0.5) * cw / 320))
            n += 1
            ok += same(pixel(g, gx, gy), pixel(s, x, y))
    return ok / n if n else 0.0


def score_sides(gl_path, wide_path, rows=145):
    """the same for the columns beside the game's screen: the GL shot against the wide frame
    the software renderer drew (its middle columns shown, 240 * aspect of them)"""
    g, s = read_bmp(gl_path), read_bmp(wide_path)
    gw, gh, sw = g[2], abs(g[3]), s[2]
    cols = min(sw, int(240.0 * gw / gh + 0.5) & ~1)
    c0 = (sw - cols) // 2
    dx = (sw - 320) // 2
    ok = n = 0
    for y in range(rows):
        gy = min(gh - 1, int((y + 0.5) * gh / 200))
        for x in range(c0, c0 + cols):
            if dx <= x < dx + 320:
                continue
            gx = min(gw - 1, int((x - c0 + 0.5) * gw / cols))
            n += 1
            ok += same(pixel(g, gx, gy), pixel(s, x, y))
    return ok / n if n else 0.0


def to_png(bmp, png, width=None):
    cmd = ["sips", "-s", "format", "png", bmp, "--out", png]
    if width:
        cmd[1:1] = ["--resampleWidth", str(width)]
    subprocess.run(cmd, capture_output=True)


def one(name, shots, keys, aspects, interp):
    out = os.path.join(OUT, name)
    os.makedirs(out, exist_ok=True)
    sw = run(name, out, False, shots, keys)
    for t in shots:
        s = os.path.join(out, "sw_%d.bmp" % t)
        if os.path.exists(s):
            to_png(s, s[:-4] + ".png")
    results = []
    for aspect in aspects:
        gl = run(name, out, True, shots, keys, aspect, interp)
        tag = "gl" if aspect == "4:3" else "gl" + aspect.replace(":", "x")
        scores, sides = [], []
        for t in shots:
            g = os.path.join(out, "%s_%d.bmp" % (tag, t))
            if os.path.exists(g):
                to_png(g, g[:-4] + ".png", 960 if aspect == "4:3" else 1280)
                if os.path.exists(g[:-4] + "_sw.bmp"):
                    scores.append(score(g, g[:-4] + "_sw.bmp"))
                if aspect != "4:3" and os.path.exists(g[:-4] + "_swwide.bmp"):
                    sides.append(score_sides(g, g[:-4] + "_swwide.bmp"))
        results.append((aspect, len(gl), scores, sides))
    return name, len(sw), results


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--jobs", type=int, default=3)
    ap.add_argument("--keys", default=DEFAULT_KEYS)
    ap.add_argument("--shots", default=",".join(map(str, DEFAULT_SHOTS)))
    ap.add_argument("--aspect", default="4:3")
    ap.add_argument("--interp", action="store_true")
    ap.add_argument("names", nargs="*")
    a = ap.parse_args()
    aspects = a.aspect.split(",")
    for asp in aspects:
        if asp not in WINDOWS:
            ap.error("--aspect: one of " + ", ".join(WINDOWS))
    names = a.names or sorted(f[:-4] for f in os.listdir(os.path.join(ROOT, "orig", "saves"))
                              if f.endswith(".zip"))
    shots = [int(x) for x in a.shots.split(",")]
    with ThreadPoolExecutor(a.jobs) as ex:
        for name, ns, results in ex.map(lambda n: one(n, shots, a.keys, aspects, a.interp), names):
            for aspect, ng, sc, sides in results:
                print("%-10s %-5s software %d shots, GL %d shots, GL = software %s%s" %
                      (name, aspect, ns, ng, " ".join("%.3f" % x for x in sc),
                       ("; sides " + " ".join("%.3f" % x for x in sides)) if sides else ""),
                      flush=True)


if __name__ == "__main__":
    main()
