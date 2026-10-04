#!/usr/bin/env python3
"""Play FALL.EXE a step at a time, for a model (or a person) at the controls.

The game only runs when told to, so there is no real time: each step loads the session's last
snapshot, does something, saves a new snapshot, and prints the screen (2x PNG with a ruler),
where the player is (position, heading, the nearest buildings) and the code that ran for the
first time. Any step can be rewound. Needs tools/build_unicorn.sh (speed and coverage).

usage (SESSION is a name; steps are kept in build/play/SESSION):
  new SESSION FROM            start from a snapshot in build/emu/snap (e.g. cheat_tlalac_s)
  step SESSION "EVENTS" [--ticks N] [--frames K]   play input (below)
  walk SESSION METRES         walk forward that far (stops early if blocked); back: backwards
  face SESSION DEGREES        turn to a compass heading (0 north, 90 east, ...), with the
                              arrow keys; turn: relative
  tp SESSION EAST NORTH       move the player by metres (east, north; negative for west/south)
  buildings SESSION [TYPE]    the town's buildings: index, type, distance and bearing
  goto SESSION N [SIDE [M]]   stand M metres (default 16; 30 for guild halls, temples and
                              palaces) from building N's centre on SIDE (n/e/s/w, default the
                              side facing you), facing it
  door SESSION N [K]          stand 1.5 m outside building N's door K (default the first),
                              facing it (doors: the door faces of its 3D models in ARCH3D.BSA)
  enter SESSION N [K]         door, then F2 (grab mode), click the door, click away the shop's
                              description and wait until inside
  look4 SESSION N [M]         one picture of building N from each side (no step is taken):
                              top left north side, top right east, bottom left south, bottom
                              right west; a door shows where to `goto`
  where SESSION               position and nearby buildings now
  rewind SESSION N / log SESSION / cov SESSION

EVENTS (';'-separated, ticks counted from the start of the step):
  T press KEY        hold KEY 20 ticks (a shorter press can fall between two frames)
  T press ctrl+f1    a chord: modifiers held around the key
  T hold KEY N       hold KEY for N ticks
  T click X,Y        left click at game pixel X,Y (0-319, 0-199; the screenshot is 2x)
  T rclick X,Y / T dclick X,Y / T mouse X,Y,B  / T down KEY / T up KEY
Keys: a-z 0-9 f1-f12 up down left right esc enter space tab bksp ins del home end pgup pgdn
ctrl alt lshift rshift [ ] ; ' ` \\ , . / - = kp+ kp- kp*. Every key goes up before the step
ends. --ticks: step length (default 60 after the last event); --frames K: K extra screenshots.

Game state comes from memory: the player is the object *D_00195AA4 (x, y, z at +7/+11/+15 in
units of 1/40 m, yaw at +3, 2048 to a turn, clockwise from north); the byte at 0x1789FA is 1
outside, 2 inside a building, 3 in a dungeon (palaces too); loaded town blocks carry
their world origin 0x40 bytes before their name; buildings come from ARENA2/BLOCKS.BSA, their
doors from the door faces of their models in ARCH3D.BSA. Coverage goes to build/play/SESSION
and build/cov/play_SESSION.cov (for fallcov.py report). docs/play.md: the brief for playing.
"""
import argparse
import ctypes
import json
import math
import os
import re
import shutil
import struct
import sys
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402  (first: it picks the Unicorn build)
import fallcov  # noqa: E402

ROOT = fallemu.ROOT
L = fallemu.LOAD
PLAY = os.path.join(ROOT, "build", "play")
LO, HI = fallcov.LO, fallcov.HI
UNITS = 40.0                      # world units per metre (a block is 4096 units, ~102 m)
BTYPES = ["alchemist", "house for sale", "armorer", "bank", "town4", "bookseller",
          "clothing store", "furniture store", "gem store", "general store", "library",
          "guild hall", "pawn shop", "weapon smith", "temple", "tavern", "palace", "house1",
          "house2", "house3", "house4", "house5", "house6", "town23", "ship"]
_FACTIONS = {}


def faction(n):
    """A faction's name, from ARENA2/FACTION.TXT ("#40 ... name: The Mages Guild")."""
    if not _FACTIONS:
        cur = None
        for line in open(os.path.join(ROOT, "build", "game", "ARENA2", "faction.txt"),
                         encoding="latin-1"):
            m = re.match(r"\s*#(\d+)", line)
            if m:
                cur = int(m.group(1))
            m = re.match(r"\s*name:\s*(.*)", line, re.I)
            if m and cur is not None:
                _FACTIONS.setdefault(cur, m.group(1).strip())
    return _FACTIONS.get(n)


# ---- sessions --------------------------------------------------------------------------
def sdir(session):
    return os.path.join(PLAY, session)


def load_steps(session):
    p = os.path.join(sdir(session), "steps.json")
    return json.load(open(p)) if os.path.exists(p) else []


def save_steps(session, steps):
    json.dump(steps, open(os.path.join(sdir(session), "steps.json"), "w"), indent=1)


def latest(session):
    steps = load_steps(session)
    if not steps:
        raise SystemExit("no session %s: fallplay.py new %s FROM" % (session, session))
    return steps, steps[-1]["n"]


def load_emu(session, n):
    d = sdir(session)
    return fallemu.Emu.load(os.path.join(d, "%03d.snap" % n), overlay=os.path.join(d, "overlay"))


# ---- game state ------------------------------------------------------------------------
def rd(emu, a, f):
    return struct.unpack(f, bytes(emu.uc.mem_read(L + a, struct.calcsize(f))))[0]


def wr(emu, a, v, f):
    emu.uc.mem_write(L + a, struct.pack(f, v))


def player_obj(emu):
    return rd(emu, 0x195AA4, "<I") - L


def player(emu):
    o = player_obj(emu)
    return {"x": rd(emu, o + 7, "<i"), "y": rd(emu, o + 11, "<i"), "z": rd(emu, o + 15, "<i"),
            "yaw": rd(emu, o + 3, "<h") & 2047}


ENVS = {1: "outside", 2: "inside a building", 3: "in a dungeon"}   # palaces are dungeons


def env(emu):
    """Where the player is: the byte at 0x1789FA (1 outside, 2 a building, 3 a dungeon)."""
    return rd(emu, 0x1789FA, "<B")


def set_player(emu, x=None, z=None, yaw=None):
    o = player_obj(emu)
    if x is not None:
        wr(emu, o + 7, int(x), "<i")
    if z is not None:
        wr(emu, o + 15, int(z), "<i")
    if yaw is not None:
        old = rd(emu, o + 3, "<h")
        wr(emu, o + 3, (old & ~2047) | (int(yaw) & 2047), "<h")


def compass(deg):
    return ["N", "NE", "E", "SE", "S", "SW", "W", "NW"][int((deg % 360 + 22.5) // 45) % 8]


_BSA = {}


def bsa():
    """BLOCKS.BSA: {name: (offset, size)} and its bytes."""
    if not _BSA:
        p = os.path.join(ROOT, "build", "game", "ARENA2", "BLOCKS.BSA")
        d = open(p, "rb").read()
        cnt = struct.unpack_from("<h", d, 0)[0]
        dirs, off = len(d) - cnt * 18, 4
        for k in range(cnt):
            name = d[dirs + 18 * k:dirs + 18 * k + 14].split(b"\0")[0].decode()
            size = struct.unpack_from("<I", d, dirs + 18 * k + 14)[0]
            _BSA[name] = (off, size)
            off += size
        _BSA["\0data"] = d
    return _BSA


def block_buildings(name):
    """An RMB block's buildings: [(local x, local z (north up), rotation, type, quality,
    faction)]; header: 3 counts, 32 positions (20 bytes), 32 building records (26 bytes)."""
    b = bsa()
    if name not in b:
        return []
    o, _sz = b[name]
    d = b["\0data"]
    out = []
    for i in range(d[o]):
        _u1, _u2, x, z, rot = struct.unpack_from("<IIiii", d, o + 3 + 20 * i)
        rec = d[o + 3 + 640 + 26 * i:o + 3 + 640 + 26 * (i + 1)]
        fac, btype, qual = struct.unpack_from("<H", rec, 18)[0], rec[24], rec[25]
        out.append((x, 4096 - z, rot, btype, qual, fac))
    return out


_ARCH = {}


def arch3d():
    """ARCH3D.BSA (the 3D models; fallemu unpacks it from PACKED.DAT): {id: (offset, size)}."""
    if not _ARCH:
        for p in (os.path.join(ROOT, "build", "game", "ARENA2", "ARCH3D.BSA"),
                  os.path.join(ROOT, "build", "emu", "packed", "ARCH3D.BSA"),
                  os.path.join(ROOT, "build", "emu", "overlay", "ARENA2", "ARCH3D.BSA")):
            if os.path.exists(p):
                d = open(p, "rb").read()
                break
        else:
            return _ARCH
        cnt = struct.unpack_from("<H", d, 0)[0]
        idx, off = len(d) - 8 * cnt, 4
        for i in range(cnt):
            mid, size = struct.unpack_from("<II", d, idx + 8 * i)
            _ARCH[mid] = (off, size)
            off += size
        _ARCH["\0data"] = d
    return _ARCH


def _ray_hits(faces, o, d):
    """Does the ray o + t d (t > 0) cross any of the polygons?"""
    for vs in faces:
        a = vs[0]
        nx = ny = nz = 0.0                      # Newell's normal
        for i in range(len(vs)):
            p, q = vs[i], vs[(i + 1) % len(vs)]
            nx += (p[1] - q[1]) * (p[2] + q[2])
            ny += (p[2] - q[2]) * (p[0] + q[0])
            nz += (p[0] - q[0]) * (p[1] + q[1])
        den = nx * d[0] + ny * d[1] + nz * d[2]
        if abs(den) < 1e-9:
            continue
        tt = (nx * (a[0] - o[0]) + ny * (a[1] - o[1]) + nz * (a[2] - o[2])) / den
        if tt <= 0:
            continue
        h = [o[i] + tt * d[i] for i in range(3)]
        drop = max(range(3), key=lambda i: abs((nx, ny, nz)[i]))
        u, v = [i for i in range(3) if i != drop]
        inside = False
        for i in range(len(vs)):                # even-odd rule
            p, q = vs[i], vs[(i + 1) % len(vs)]
            if (p[v] > h[v]) != (q[v] > h[v]) and \
                    h[u] < p[u] + (h[v] - p[v]) * (q[u] - p[u]) / (q[v] - p[v]):
                inside = not inside
        if inside:
            return True
    return False


_DOORS = {}


def model_doors(mid):
    """A model's door faces (texture file 74): [(centre x, z, normal x, z, outer)] in world
    units, the normal horizontal, of unit length and pointing outside. A ray from the door's
    middle is cast both ways across the model's other faces: outside is where it gets away
    (outer 1). If it is blocked both ways (an inner door, between rooms or onto a courtyard;
    outer 0) or neither, the normal points to the nearer edge of the model's footprint.
    Outer doors first."""
    if mid in _DOORS:
        return _DOORS[mid]
    a = arch3d()
    if mid not in a:
        return []
    o, sz = a[mid]
    r = a["\0data"][o:o + sz]
    npts, npl = struct.unpack_from("<ii", r, 4)
    ptlo, pllo = struct.unpack_from("<i", r, 48)[0], struct.unpack_from("<i", r, 60)[0]
    pts = [tuple(c / 256 for c in struct.unpack_from("<iii", r, ptlo + 12 * i)) for i in range(npts)]
    faces, doors, p = [], [], pllo
    for _k in range(npl):
        n, tex = r[p], struct.unpack_from("<H", r, p + 2)[0]
        vs = [pts[struct.unpack_from("<i", r, p + 8 + 8 * j)[0] // 12] for j in range(n)]
        if len(vs) >= 3:
            (doors if tex >> 7 == 74 else faces).append(vs)
        p += 8 + 8 * n
    x0, x1 = min(v[0] for v in pts), max(v[0] for v in pts)
    z0, z1 = min(v[2] for v in pts), max(v[2] for v in pts)

    def edge(x, z, nx, nz):
        return min([(x1 - x) / nx if nx > 1e-6 else (x0 - x) / nx if nx < -1e-6 else 1e9,
                    (z1 - z) / nz if nz > 1e-6 else (z0 - z) / nz if nz < -1e-6 else 1e9])
    out = []
    for vs in doors:
        # the face is upright: its horizontal normal is across its widest horizontal edge
        ex, ez = max(((b[0] - a[0], b[2] - a[2]) for a in vs for b in vs),
                     key=lambda e: e[0] * e[0] + e[1] * e[1])
        el = math.hypot(ex, ez) or 1
        nx, nz = ez / el, -ex / el
        c = [sum(v[i] for v in vs) / len(vs) for i in range(3)]
        free = [not _ray_hits(faces, (c[0] + s * 2 * nx, c[1], c[2] + s * 2 * nz), (s * nx, 0, s * nz))
                for s in (1, -1)]
        if free[0] != free[1]:
            s = 1 if free[0] else -1
        else:
            s = 1 if edge(c[0], c[2], nx, nz) <= edge(c[0], c[2], -nx, -nz) else -1
        out.append((c[0], c[2], s * nx, s * nz, int(free[0] != free[1])))
    _DOORS[mid] = sorted(out, key=lambda d: -d[4])
    return _DOORS[mid]


def _turn(x, z, rot):
    th = math.radians(rot * 360 / 2048)
    return x * math.cos(th) - z * math.sin(th), x * math.sin(th) + z * math.cos(th)


def block_doors(name):
    """{building index in the block: [(door centre x, z relative to the building, normal x, z,
    outer)]} (see model_doors):
    the door faces of each building's exterior models (block data: 17-byte header, models of
    66 bytes: model id = u16 at 0 * 100 + byte at 2, position in the building x, z at +36, +44,
    yaw at +54), placed and turned by the model's then the building's rotation (counter-
    clockwise, 2048 to a turn)."""
    b = bsa()
    if name not in b:
        return {}
    o, _sz = b[name]
    d = b["\0data"]
    sizes = struct.unpack_from("<32I", d, o + 1603)
    cur, out = o + 6776, {}
    for i in range(d[o]):
        rot = struct.unpack_from("<i", d, o + 3 + 20 * i + 16)[0]
        doors = []
        for k in range(d[cur]):
            m = cur + 17 + 66 * k
            mid = struct.unpack_from("<H", d, m)[0] * 100 + d[m + 2]
            mx, mz = struct.unpack_from("<i", d, m + 36)[0], struct.unpack_from("<i", d, m + 44)[0]
            myaw = struct.unpack_from("<h", d, m + 54)[0]
            for cx, cz, nx, nz, outer in model_doors(mid):
                x, z = _turn(cx, cz, myaw)
                doors.append(_turn(mx + x, mz + z, rot) + _turn(*_turn(nx, nz, myaw), rot) + (outer,))
        out[i] = sorted(doors, key=lambda d: -d[4])
        cur += sizes[i]
    return out


def loaded_blocks(emu, near=None):
    """Town blocks in memory: [(name, origin x, origin z)] (within ~8 blocks of `near`)."""
    mem = bytes(emu.uc.mem_read(L + 0x200000, 48 << 20))
    out, seen = [], set()
    for m in re.finditer(rb"[A-Z0-9]{8}\.RMB\0", mem):
        h = m.start() - 0x40
        if h < 0:
            continue
        x, _y, z, s = struct.unpack_from("<iiii", mem, h)
        if s != 32768:
            continue
        key = (m.group()[:-1].decode(), x, z)
        if key in seen:
            continue
        if near and (abs(x - near[0]) > 8 * 4096 or abs(z - near[1]) > 8 * 4096):
            continue
        seen.add(key)
        out.append(key)
    return out


def buildings(emu):
    p = player(emu)
    out = []
    for name, ox, oz in loaded_blocks(emu, (p["x"], p["z"])):
        doors = block_doors(name)
        for k, (lx, lz, rot, btype, qual, fac) in enumerate(block_buildings(name)):
            wx, wz = ox + lx, oz + lz
            dx, dz = wx - p["x"], wz - p["z"]
            out.append({"block": name, "x": wx, "z": wz, "type": btype, "quality": qual,
                        "faction": fac, "dist": math.hypot(dx, dz) / UNITS,
                        "bearing": math.degrees(math.atan2(dx, dz)) % 360,
                        "doors": [(wx + dd[0], wz + dd[1]) + dd[2:] for dd in doors.get(k, [])]})
    out.sort(key=lambda b: (b["block"], b["x"], b["z"]))
    for i, b in enumerate(out):
        b["i"] = i
    return out


def bname(b):
    t = BTYPES[b["type"]] if b["type"] < len(BTYPES) else "type %d" % b["type"]
    if b["faction"] and b["faction"] != 510:          # 510: The Merchants, every shop
        t += " (%s)" % (faction(b["faction"]) or "faction %d" % b["faction"])
    return t


def describe(emu, top=6):
    p = player(emu)
    deg = p["yaw"] * 360 / 2048
    lines = ["you (%s): x %.1f m east, z %.1f m north (world units %d, %d), heading %d deg %s" % (
        ENVS.get(env(emu), "?"), p["x"] / UNITS, p["z"] / UNITS, p["x"], p["z"], round(deg) % 360,
        compass(deg))]
    bs = [b for b in buildings(emu) if b["type"] not in range(17, 23)] if env(emu) == 1 else []
    if env(emu) != 1:
        pass
    elif not bs:
        lines.append("no town around (the wilderness)")
    else:
        bs.sort(key=lambda b: b["dist"])
        lines.append("nearest shops and halls: " + "; ".join(
            "#%d %s %dm %s" % (b["i"], bname(b), b["dist"], compass(b["bearing"])) for b in bs[:top]))
    return "\n".join(lines)


# ---- input -----------------------------------------------------------------------------
def expand(events):
    """The step's events -> fallemu script text, plus the tick by which every key is up."""
    out, held, last = [], {}, 0
    for item in events.replace("\n", ";").split(";"):
        w = item.split()
        if not w:
            continue
        t, act = int(w[0]), w[1]
        if act == "press":
            *mods, key = w[2].split("+")
            out += ["%d down %s" % (t, m) for m in mods]
            out += ["%d down %s" % (t + 1, key), "%d up %s" % (t + 21, key)]
            out += ["%d up %s" % (t + 22, m) for m in mods]
            last = max(last, t + 22)
        elif act == "hold":
            out += ["%d down %s" % (t, w[2]), "%d up %s" % (t + int(w[3]), w[2])]
            last = max(last, t + int(w[3]))
        elif act == "rclick":
            x, y = w[2].split(",")
            out += ["%d mouse %s,%s,0" % (t, x, y), "%d mouse %s,%s,2" % (t + 20, x, y),
                    "%d mouse %s,%s,0" % (t + 50, x, y)]
            last = max(last, t + 50)
        else:
            if act == "down":
                held[w[2]] = t
            elif act == "up":
                held.pop(w[2], None)
            out.append(item.strip())
            last = max(last, t + (50 if act in ("click", "dclick") else 3))
    for k in held:                     # nothing stays down after the step
        last += 1
        out.append("%d up %s" % (last, k))
    return "; ".join(out), last


# ---- screens ---------------------------------------------------------------------------
def rgb_png(path, w, h, rgb):
    raw = b"".join(b"\0" + rgb[y * w * 3:(y + 1) * w * 3] for y in range(h))

    def chunk(t, dd):
        c = struct.pack(">I", len(dd)) + t + dd
        return c + struct.pack(">I", zlib.crc32(t + dd) & 0xFFFFFFFF)
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
                + chunk(b"IDAT", zlib.compress(raw, 6)) + chunk(b"IEND", b""))


def frame_rgb(emu):
    src = emu.read(0xA0000, 64000)
    pal = [bytes(c) for c in emu.palette]
    return b"".join(pal[c] for c in src)


def shot(emu, path):
    """The screen at 2x with a ruler every 20 game pixels (longer every 40)."""
    if emu.mode != 0x13:
        return False
    rgb = frame_rgb(emu)
    W, H = 640, 400
    out = bytearray(W * H * 3)
    for y in range(H):
        row = rgb[(y // 2) * 960:(y // 2) * 960 + 960]
        out[y * W * 3:(y + 1) * W * 3] = b"".join(row[3 * x:3 * x + 3] * 2 for x in range(320))
    for g in range(0, 320, 20):
        for k in range(6 if g % 40 == 0 else 3):
            for (px, py) in ((2 * g, k), (k, 2 * g)):
                if py < H:
                    out[(py * W + px) * 3:(py * W + px) * 3 + 3] = b"\xff\xff\x00"
    rgb_png(path, W, H, bytes(out))
    return True


# ---- coverage --------------------------------------------------------------------------
def cov_lib():
    from unicorn.unicorn_py3 import unicorn as ucmod
    lib = ucmod.uclib
    if not hasattr(lib, "uc_dagger_coverage"):
        raise SystemExit("needs the patched Unicorn: tools/build_unicorn.sh")
    lib.uc_dagger_coverage.argtypes = [ctypes.c_void_p, ctypes.c_uint64, ctypes.c_uint64,
                                       ctypes.c_void_p]
    return lib


def session_cov(session):
    p = os.path.join(sdir(session), "cov.bin")
    return bytearray(zlib.decompress(open(p, "rb").read())) if os.path.exists(p) \
        else bytearray(HI - LO)


# ---- steps -----------------------------------------------------------------------------
def run_step(session, label, action):
    """Load the last snapshot, run action(emu) with coverage on, save the next step."""
    steps, n = latest(session)
    d = sdir(session)
    emu = load_emu(session, n)
    lib = cov_lib()
    lo, hi = L + LO, L + HI
    lib.uc_dagger_coverage(emu.uc._uch, lo, hi, None)
    start = emu.ticks
    extra = action(emu) or []
    emu.save(os.path.join(d, "%03d.snap" % (n + 1)))
    final = os.path.join(d, "%03d.png" % (n + 1))
    shot(emu, final)
    buf = ctypes.create_string_buffer(hi - lo)
    lib.uc_dagger_coverage(emu.uc._uch, lo, hi, buf)
    old = session_cov(session)
    cov = (int.from_bytes(old, "little") | int.from_bytes(buf.raw, "little")).to_bytes(HI - LO, "little")
    open(os.path.join(d, "cov.bin"), "wb").write(zlib.compress(cov))
    os.makedirs(fallcov.COV, exist_ok=True)          # for fallcov.py report, with the batch runs
    with open(os.path.join(fallcov.COV, "play_%s.cov" % session), "wb") as f:
        f.write(zlib.compress(json.dumps({"play": session, "steps": n + 1}).encode() + b"\n" + cov))
    fns = fallcov.functions()
    newf = [(g, k) for va, _nm, g, k in fns if LO <= va < HI and cov[va - LO] and not old[va - LO]]
    tot = {}
    for va, _nm, g, k in fns:
        if LO <= va < HI and cov[va - LO]:
            tot[k] = tot.get(k, 0) + 1
    # argv: the command, which replays the step exactly from the previous snapshot (the
    # emulator and every command are deterministic)
    steps.append({"n": n + 1, "events": label, "tick": emu.ticks, "new_functions": len(newf),
                  "argv": sys.argv[1:2] + sys.argv[3:]})
    save_steps(session, steps)
    by = {}
    for g, _k in newf:
        by[g] = by.get(g, 0) + 1
    print("step %d: %s (%d ticks)%s" % (n + 1, label, emu.ticks - start, "" if emu.exit_code is None
                                       else ", the game exited (%s)" % emu.exit_code))
    for line in extra:
        print(line)
    print("screen: %s" % final)
    try:
        print(describe(emu))
    except Exception as e:      # (state readout is best effort, e.g. during a menu)
        print("state: %s" % e)
    print("new code: %d functions%s" % (len(newf), "" if not by else ": " + ", ".join(
        "%s %d" % kv for kv in sorted(by.items(), key=lambda kv: -kv[1])[:10])))
    print("session total: game %d, xngine %d, library %d" % (
        tot.get("game", 0), tot.get("xngine", 0), tot.get("library", 0)))


def run_events(emu, events, extra=60):
    """Run step-syntax events (see step) from now, then extra ticks after the last one."""
    text, last = expand(events)
    s = emu.ticks
    emu.run(s + last + extra, [(s + t, a, g) for t, a, g in fallemu.parse_script(text)], None)


def act_events(events, ticks, frames, session):
    def action(emu):
        text, last = expand(events)
        script = fallemu.parse_script(text)
        length = max(ticks or 0, last + 60 if ticks is None else last + 1)
        s = emu.ticks
        pending = [(s + t, a, g) for t, a, g in script]
        extra = []
        n = latest(session)[1] + 1
        for k in range(frames):
            m = s + length * (k + 1) // (frames + 1)
            emu.run(m, [e for e in pending if e[0] <= m], None)
            pending = [e for e in pending if e[0] > m]
            p = os.path.join(sdir(session), "%03d_%d.png" % (n, k + 1))
            if shot(emu, p):
                extra.append("frame: %s" % p)
        emu.run(s + length, pending, None)
        return extra
    return action


def act_walk(metres, key):
    def action(emu):
        p0 = player(emu)
        s = emu.ticks
        emu.run(s + 1, [(s + 1, "down", key)], None)
        best, best_t, moved = 0.0, emu.ticks, 0.0
        limit = s + int(metres * UNITS / 0.8) + 200
        while moved < metres and emu.ticks < limit:
            emu.run(emu.ticks + 5, [], None)
            p = player(emu)
            moved = math.hypot(p["x"] - p0["x"], p["z"] - p0["z"]) / UNITS
            if moved > best + 0.2:
                best, best_t = moved, emu.ticks
            if emu.ticks - best_t > 60:
                break
        t = emu.ticks
        emu.run(t + 30, [(t + 1, "up", key)], None)
        p = player(emu)
        moved = math.hypot(p["x"] - p0["x"], p["z"] - p0["z"]) / UNITS
        return ["walked %.1f m%s" % (moved, " (blocked)" if moved < metres - 0.5 else "")]
    return action


def turn_to(emu, target):
    """Turn with the arrow keys until the heading is within ~1 degree of target (yaw units):
    the 3D view only follows the game's own turning, not a written heading."""
    for _attempt in range(8):
        delta = (target - player(emu)["yaw"] + 1024) % 2048 - 1024
        if abs(delta) <= 6:
            return
        key = "right" if delta > 0 else "left"
        s = emu.ticks
        emu.run(s + 1, [(s + 1, "down", key)], None)
        while emu.ticks < s + 1500:
            d = (target - player(emu)["yaw"] + 1024) % 2048 - 1024
            if (d > 0) != (delta > 0) or abs(d) <= 6 + min(30, (emu.ticks - s) // 40):
                break
            emu.run(emu.ticks + 1, [], None)
        t = emu.ticks
        emu.run(t + 15, [(t + 1, "up", key)], None)


def act_set(yaw=None, dx=None, dz=None, dyaw=None):
    def action(emu):
        p = player(emu)
        if dx is not None or dz is not None:
            set_player(emu, p["x"] + (dx or 0) * UNITS, p["z"] + (dz or 0) * UNITS)
            emu.run(emu.ticks + 30, [], None)
        target = yaw if yaw is not None else ((p["yaw"] + dyaw) % 2048 if dyaw is not None else None)
        if target is not None:
            turn_to(emu, target % 2048)
        emu.run(emu.ticks + 10, [], None)
    return action


def place(b, side, p, dist=None):
    """Where to stand for building b on side (n/e/s/w), and the yaw that faces it: `dist`
    metres from its centre (default 16, 30 for guild halls, temples and palaces)."""
    if side in (None, "near"):
        dx, dz = p["x"] - b["x"], p["z"] - b["z"]
        side = ("e" if dx > 0 else "w") if abs(dx) > abs(dz) else ("n" if dz > 0 else "s")
    vx, vz = {"n": (0, 1), "s": (0, -1), "e": (1, 0), "w": (-1, 0)}[side]
    d = (dist or (30 if b["type"] in (11, 14, 16) else 16)) * UNITS
    x, z = b["x"] + vx * d, b["z"] + vz * d
    yaw = round(math.degrees(math.atan2(-vx, -vz)) % 360 * 2048 / 360) % 2048
    return x, z, yaw, side


def door_spot(b, k=0):
    """Where to stand for building b's door k (1.5 m outside it, along its normal; the doors
    come outside ones first, see model_doors), and the yaw facing it."""
    if not b["doors"]:
        return None
    dx_, dz_, vx, vz, _outer = b["doors"][min(k, len(b["doors"]) - 1)]
    x, z = dx_ + vx * 1.5 * UNITS, dz_ + vz * 1.5 * UNITS
    yaw = round(math.degrees(math.atan2(-vx, -vz)) % 360 * 2048 / 360) % 2048
    return x, z, yaw


def find_building(emu, n):
    if env(emu) != 1:
        raise SystemExit("you are %s: the town outside is not loaded (leave by a door, or "
                         "rewind)" % ENVS.get(env(emu), "not outside"))
    bs = buildings(emu)
    if not 0 <= n < len(bs):
        raise SystemExit("no building #%d (fallplay.py buildings SESSION)" % n)
    return bs[n]


# ---- commands --------------------------------------------------------------------------
def new(session, frm):
    d = sdir(session)
    if os.path.exists(d):
        shutil.rmtree(d)
    os.makedirs(d)
    src = frm if frm.endswith(".snap") else os.path.join(ROOT, "build", "emu", "snap", frm + ".snap")
    shutil.copyfile(src, os.path.join(d, "000.snap"))
    emu = fallemu.Emu.load(os.path.join(d, "000.snap"), overlay=os.path.join(d, "overlay"))
    shot(emu, os.path.join(d, "000.png"))
    save_steps(session, [{"n": 0, "from": frm, "tick": emu.ticks}])
    print("session %s from %s; screen: %s" % (session, frm, os.path.join(d, "000.png")))
    print(describe(emu))


def cmd_buildings(session, kind):
    emu = load_emu(session, latest(session)[1])
    for b in sorted(buildings(emu), key=lambda b: b["dist"]):
        if kind and kind.lower() not in bname(b).lower():
            continue
        print("#%-3d %-28s quality %2d  %4dm %-2s  (block %s)" % (
            b["i"], bname(b), b["quality"], b["dist"], compass(b["bearing"]), b["block"]))


def look4(session, n, dist=None):
    _steps, last = latest(session)
    d = sdir(session)
    frames = []
    for side in ("n", "e", "s", "w"):
        emu = load_emu(session, last)
        b = find_building(emu, n)
        x, z, yaw, _s = place(b, side, player(emu), dist)
        set_player(emu, x, z)
        emu.run(emu.ticks + 30, [], None)
        turn_to(emu, yaw)
        emu.run(emu.ticks + 10, [], None)
        frames.append(frame_rgb(emu))
    W, H = 640, 400
    out = bytearray(W * H * 3)
    for k, f in enumerate(frames):
        ox, oy = (k % 2) * 320, (k // 2) * 200
        for y in range(200):
            out[((oy + y) * W + ox) * 3:((oy + y) * W + ox + 320) * 3] = f[y * 960:(y + 1) * 960]
    p = os.path.join(d, "look4_%d.png" % n)
    rgb_png(p, W, H, bytes(out))
    print("building #%d from each side (facing it): %s" % (n, p))
    print("top left: north side, top right: east, bottom left: south, bottom right: west")


def rewind(session, n):
    d = sdir(session)
    steps = [s for s in load_steps(session) if s["n"] <= n]
    for f in os.listdir(d):
        m = re.match(r"(\d{3})(_\d+)?\.(snap|png)$", f)
        if m and int(m.group(1)) > n:
            os.remove(os.path.join(d, f))
    save_steps(session, steps)
    print("session %s back at step %d (coverage keeps what the dropped steps ran)" % (session, n))


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    for c, args in (("new", ["frm"]), ("rewind", ["n"]), ("log", []), ("cov", []), ("where", []),
                    ("walk", ["metres"]), ("back", ["metres"]), ("face", ["degrees"]),
                    ("turn", ["degrees"]), ("tp", ["east", "north"])):
        a = sub.add_parser(c)
        a.add_argument("session")
        for x in args:
            a.add_argument(x)
    a = sub.add_parser("step")
    a.add_argument("session")
    a.add_argument("events")
    a.add_argument("--ticks", type=int, default=None)
    a.add_argument("--frames", type=int, default=0)
    a = sub.add_parser("buildings")
    a.add_argument("session")
    a.add_argument("kind", nargs="?")
    a = sub.add_parser("goto")
    a.add_argument("session")
    a.add_argument("n", type=int)
    a.add_argument("side", nargs="?")
    a.add_argument("dist", nargs="?", type=float)
    for c in ("door", "enter"):
        a = sub.add_parser(c)
        a.add_argument("session")
        a.add_argument("n", type=int)
        a.add_argument("k", nargs="?", type=int, default=0)
    a = sub.add_parser("look4")
    a.add_argument("session")
    a.add_argument("n", type=int)
    a.add_argument("dist", nargs="?", type=float)
    a = ap.parse_args()
    s = a.session
    if a.cmd == "new":
        new(s, a.frm)
    elif a.cmd == "step":
        run_step(s, a.events, act_events(a.events, a.ticks, a.frames, s))
    elif a.cmd in ("walk", "back"):
        run_step(s, "%s %s m" % (a.cmd, a.metres), act_walk(float(a.metres),
                                                            "up" if a.cmd == "walk" else "down"))
    elif a.cmd == "face":
        run_step(s, "face %s" % a.degrees, act_set(yaw=round(float(a.degrees) * 2048 / 360)))
    elif a.cmd == "turn":
        run_step(s, "turn %s" % a.degrees, act_set(dyaw=round(float(a.degrees) * 2048 / 360)))
    elif a.cmd == "tp":
        run_step(s, "tp %s %s" % (a.east, a.north), act_set(dx=float(a.east), dz=float(a.north)))
    elif a.cmd == "goto":
        def action(emu):
            b = find_building(emu, a.n)
            x, z, yaw, side = place(b, a.side, player(emu), a.dist)
            set_player(emu, x, z)
            emu.run(emu.ticks + 30, [], None)
            turn_to(emu, yaw)
            emu.run(emu.ticks + 10, [], None)
            return ["at building #%d (%s), %s side, facing it" % (a.n, bname(b), side)]
        run_step(s, "goto %d %s" % (a.n, a.side or ""), action)
    elif a.cmd in ("door", "enter"):
        def action(emu):
            b = find_building(emu, a.n)
            spot = door_spot(b, a.k)
            if spot is None:
                return ["building #%d has no door in its models (try look4 / goto)" % a.n]
            x, z, yaw = spot
            set_player(emu, x, z)
            emu.run(emu.ticks + 30, [], None)
            turn_to(emu, yaw)
            emu.run(emu.ticks + 10, [], None)
            if a.cmd == "door":
                return ["at the door of building #%d (%s), 1.5 m out, facing it (it has %d door%s); "
                        "F2 then click the screen centre (165,112) to enter" % (
                            a.n, bname(b), len(b["doors"]), "" if len(b["doors"]) == 1 else "s")]
            # grab mode, click the door; a shop shows a description first: click it away
            run_events(emu, "1 press f2; 40 click 165,112", 250)
            if math.hypot(player(emu)["x"] - x, player(emu)["z"] - z) < UNITS / 2:
                run_events(emu, "1 click 160,60", 250)
            p = player(emu)
            if math.hypot(p["x"] - x, p["z"] - z) < UNITS / 2:
                return ["tried to enter building #%d (%s) but did not get in (locked? closed at "
                        "night? see the screen)" % (a.n, bname(b))]
            return ["inside building #%d (%s)" % (a.n, bname(b))]
        run_step(s, "%s %d" % (a.cmd, a.n), action)
    elif a.cmd == "buildings":
        cmd_buildings(s, a.kind)
    elif a.cmd == "look4":
        look4(s, a.n, a.dist)
    elif a.cmd == "where":
        print(describe(load_emu(s, latest(s)[1]), top=10))
    elif a.cmd == "rewind":
        rewind(s, int(a.n))
    elif a.cmd == "log":
        for st in load_steps(s):
            print("%3d  tick %-6s %s" % (st["n"], st.get("tick"), st.get("events", "from " + st.get("from", ""))))
    else:
        tmp = os.path.join(sdir(s), "session.cov")
        open(tmp, "wb").write(zlib.compress(b'{"snap": "session"}\n' + bytes(session_cov(s))))
        fallcov.report([tmp])


if __name__ == "__main__":
    main()
