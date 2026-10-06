#!/usr/bin/env python3
"""Scripted lockstep scenarios for XnGine: the game from a save, frame by frame, with the asm
and with the readable C, compared at the engine's boundary.

A scenario (SCENARIOS below) is:
  - a snapshot (build/emu/snap/NAME.snap: the 18 UESP saves and the cheat saves);
  - a prelude, run with the asm alone: game state set first (`poke`, `water`, `time`,
    `teleport`), tools/fallplay.py steps (`play`: [["door", "13"]]: stand at a building's
    door), then `ticks` timer ticks of play with a `script` (fallemu's script syntax, ticks
    from the prelude's start), then the game is run on to the sync point;
  - `frames` lockstep frames. A frame runs from the sync point to the next: by default main's
    per-frame update (game_frame, func_0001025B, `sync: safe`), or the entry of an engine
    function (`sync: fn:NAME`, hooked at its asm entry and, in the C machine, at its C entry
    too: internal calls of canonical C never reach the asm entry);
  - per-frame input (`input`): "FRAME ACTION ARG; ..." with ACTION
        down KEY / up KEY   a key pressed or released (scan code through the game's own int 9
                            handler: port 60h and irq 9)
        key KEY             down at FRAME, up two frames later
        mouse X,Y,B         the mouse (int 33h answers it from then on)
        click X,Y           mouse there at FRAME, button 1 down at FRAME+1, up at FRAME+3
        rclick X,Y          the same with the right button
        irqs N              timer interrupts at each frame's start from FRAME on
    Input is delivered to both machines at the same instruction, the sync point, before the
    frame runs: each interrupt is delivered and its handler run to completion (back at the
    sync point, ESP as it was) before the next;
  - `irqs`: timer interrupts per frame (game time moves only with them: walking, animation);
  - `call`: [VA, ARGS]: a direct call of a game function at the first frame, in both machines
    (fallcall's convention, returning to its trap); the scenario ends when it returns. For
    the VID movie, whose player loops inside the engine.

Two lockstep modes:
  - continuous (the default): the C machine is copied from the asm machine once, after the
    prelude, and runs on with its own state: what the C keeps, in private memory too, feeds
    the later frames;
  - resync (`continuous: False`, --resync): the C machine starts each frame from a copy of the
    asm machine (the original `xn_rc.py frames`): each frame is compared on its own. Not for a
    `call` scenario: resynced mid-call, the C machine would run the asm's own loop around the
    C (it stays continuous).

Each frame is compared:
  - the screen: VGA memory, the palette and the video mode;
  - memory: every byte of low memory and program memory but the dead stack (`full`), and
    the game-visible bytes only (`boundary`: engine-private memory, from
    config/xngine_boundary.csv via tools/xn_boundary.py, is masked);
  - the frame's port I/O and interrupts, in order (`boundary` leaves out the CPU exceptions
    XnGine's own divide-error handler takes: they are inside the engine).
A frame is identical when the screen, the game-visible memory and the I/O match.

Used by tools/xn_rc.py (`scenarios`, `frames`), tools/xn_boundary.py (the game-visible
memory, from the asm side) and tools/xn_cover.py (the asm's coverage).

usage: xn_scenarios.py list
       xn_scenarios.py show NAME          a scenario's spec
       xn_scenarios.py shots NAME [-o DIR]  the asm's screens of each frame (to check input)
"""
import argparse
import os
import struct
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402

ROOT = fallemu.ROOT
LOAD = fallemu.LOAD
SAFE = 0x1025B              # game_frame: main's per-frame update (tools/fallcall.py)
TRAP = 0xBBF00              # where direct calls return (fallcall.py's trap: never runs)
OBJ2 = (0xC0000, 0x161568)
OUT = os.path.join(ROOT, "build", "xn_canon", "scenarios")

# ---- the scenarios --------------------------------------------------------------------------
SAVES = ["blades", "dorian", "gash", "keophex", "kral", "kralvamp", "kralwolf", "ming", "mord",
         "morthag1", "orcs", "sentinel", "shadow", "tlalac_s", "uking", "wayrest", "wereboar",
         "worms"]
INDOORS = {"blades", "keophex", "kral", "mord", "orcs", "sentinel", "uking", "wayrest", "worms"}

# modal screens (their own loops, not main's): a frame ends at the game's screen update or at
# a mouse poll (the screens' waits for a button's release poll the mouse: input comes there)
MODAL = "fn:xn_gfx_present_inclusive+fn:xn_mouse_poll"

# walk forward, turn left, walk and turn right, back up
WALK_TURN = ("0 mouse 160,100,0; 0 down up; 9 up up; 10 down left; 15 up left; 16 down up; "
             "17 down right; 22 up right; 23 up up; 24 down down; 29 up down")
WALK = "0 mouse 160,100,0; 0 down up; 39 up up"

SCENARIOS = []
for _s in SAVES:
    SCENARIOS.append({
        "name": "walk_%s" % _s, "snap": "save_%s" % _s, "covers": "%s walking and turning" % (
            "dungeon" if _s in INDOORS else "outdoor"),
        "script": "5 mouse 160,100,0", "ticks": 20, "frames": 30, "irqs": 2,
        "input": WALK_TURN})
SCENARIOS += [
    {"name": "town_crowd", "snap": "save_kralvamp", "covers": "a large town with people",
     "script": "5 mouse 160,100,0", "ticks": 40, "frames": 40, "irqs": 2, "input": WALK},
    {"name": "rain", "snap": "save_tlalac_s", "covers": "rain outdoors (a town)",
     "poke": [["195E2A", "04" * 6]], "script": "5 mouse 160,100,0", "ticks": 120, "frames": 30,
     "irqs": 2, "input": WALK_TURN},
    {"name": "snow", "snap": "save_shadow", "covers": "snow outdoors",
     "poke": [["195E2A", "05" * 6]], "script": "5 mouse 160,100,0", "ticks": 120, "frames": 30,
     "irqs": 2, "input": WALK_TURN},
    {"name": "water_below", "snap": "save_mord", "covers": "dungeon water below the eye",
     "water": 200, "script": "5 mouse 160,100,0", "ticks": 40, "frames": 30, "irqs": 2,
     "input": WALK_TURN},
    {"name": "water_above", "snap": "save_mord", "covers": "under dungeon water",
     "water": -200, "script": "5 mouse 160,100,0", "ticks": 40, "frames": 30, "irqs": 2,
     "input": WALK_TURN},
    {"name": "automap", "sync": MODAL, "snap": "save_mord", "covers": "the automap (indoors)",
     "script": "5 mouse 160,100,0; 10 down m; 30 up m", "ticks": 260, "frames": 36, "irqs": 1,
     "input": "0 down left; 6 up left; 8 down pgup; 11 up pgup; 13 down right; 18 up right; "
              "20 down pgdn; 23 up pgdn; 25 click 110,180; 30 click 160,180"},
    {"name": "inventory", "snap": "save_blades", "covers": "the inventory and the paper doll",
     "script": "5 mouse 160,100,0; 10 down f6; 30 up f6", "ticks": 300, "frames": 40, "irqs": 1,
     "input": "0 mouse 90,4,0; 2 click 90,4; 8 mouse 150,4,0; 10 click 150,4; 16 click 210,4; "
              "22 click 30,4; 28 click 240,64; 34 click 60,60"},
    {"name": "charsheet", "sync": MODAL, "snap": "save_dorian", "covers": "the character sheet",
     "script": "5 mouse 160,100,0; 10 down f5; 30 up f5", "ticks": 300, "frames": 24,
     "irqs": 1, "input": "0 mouse 80,109,0; 2 click 80,109; 10 click 160,100; 16 click 80,119"},
    {"name": "spellbook", "sync": MODAL, "snap": "save_blades", "covers": "the spellbook",
     "script": "5 mouse 160,100,0; 10 down bksp; 30 up bksp", "ticks": 220, "frames": 30,
     "irqs": 1, "input": "0 mouse 60,41,0; 2 click 60,41; 8 click 60,49; 14 click 60,57; "
                         "20 click 132,174; 26 click 60,33"},
    {"name": "spell_cast", "sync": "safe+fn:xn_mouse_poll", "snap": "save_blades",
     "covers": "casting a spell (Shock)",
     "script": "5 mouse 160,100,0; 10 down bksp; 30 up bksp; 200 mouse 80,40,0; "
               "210 mouse 80,33,0; 230 mouse 80,33,1; 236 mouse 80,33,0; 242 mouse 80,33,1; "
               "248 mouse 80,33,0", "ticks": 400, "frames": 40, "irqs": 2,
     "input": "0 mouse 160,100,0; 2 mouse 160,100,1; 5 mouse 160,100,0"},
    {"name": "travelmap", "sync": MODAL, "snap": "save_tlalac_s", "covers": "the travel map",
     "script": "5 mouse 160,100,0; 10 down w; 30 up w", "ticks": 300, "frames": 30, "irqs": 1,
     "input": "0 mouse 90,60,0; 4 mouse 150,80,0; 8 click 90,60; 20 mouse 200,100,0"},
    {"name": "fight", "snap": "save_mord", "covers": "a weapon swing at a monster (Frost Daedra)",
     "teleport": ["0x9022B8", 100, 0, 0], "script": "5 mouse 160,100,0; 10 down a; 30 up a",
     "ticks": 100, "frames": 40, "irqs": 2,
     "input": "0 mouse 160,100,2; 2 mouse 100,100,2; 4 mouse 160,100,2; 6 mouse 230,100,2; "
              "8 mouse 160,130,2; 10 mouse 160,100,0; 16 mouse 160,100,2; 18 mouse 160,60,2; "
              "20 mouse 160,140,2; 22 mouse 160,100,0; 28 mouse 160,100,2; 30 mouse 230,100,2; "
              "32 mouse 160,100,2; 34 mouse 100,100,2; 36 mouse 160,100,0"},
    {"name": "rest", "snap": "save_tlalac_s", "covers": "resting for a while",
     "script": "5 mouse 160,100,0; 10 down r; 30 up r; 100 mouse 110,70,0; 110 mouse 110,70,1; "
               "130 mouse 110,70,0; 200 down 2; 220 up 2; 240 down enter; 260 up enter",
     "ticks": 300, "frames": 40, "irqs": 4, "input": ""},
    {"name": "options", "sync": MODAL, "snap": "save_blades", "covers": "the options menu",
     "script": "5 mouse 160,100,0; 10 down esc; 30 up esc", "ticks": 260, "frames": 30,
     "irqs": 1, "input": "0 mouse 150,73,0; 4 mouse 196,108,0; 8 click 122,108; "
                         "20 mouse 160,100,0; 24 click 160,100"},
    {"name": "night", "snap": "save_morthag1", "covers": "a city at night", "time": 0,
     "script": "5 mouse 160,100,0", "ticks": 60, "frames": 30, "irqs": 2, "input": WALK_TURN},
    {"name": "enter_tavern", "sync": "safe+fn:xn_mouse_poll", "snap": "save_tlalac_s",
     "covers": "entering a building (the door, the load, the interior)",
     "play": [["door", "13"]], "script": "5 down f2; 25 up f2; 40 mouse 165,100,0", "ticks": 80,
     "frames": 60, "irqs": 2, "input": "0 click 165,112; 14 click 160,60; 30 mouse 160,100,0; "
                                        "32 down up; 50 up up"},
    {"name": "mainmenu", "sync": MODAL, "snap": "build/xngine/bases/newgame_1.snap",
     "covers": "the main menu (after boot)", "script": "", "ticks": 20, "frames": 30, "irqs": 2,
     "input": "0 mouse 100,60,0; 6 mouse 160,100,0; 12 mouse 200,140,0; 18 mouse 60,180,0"},
    {"name": "vid", "snap": "save_blades", "covers": "a VID movie (the logo, a direct call)",
     "script": "5 mouse 160,100,0", "ticks": 10, "frames": 600, "irqs": 4,
     "call": [0x3A325, [0, 0, 0, 0]], "sync": "fn:xn_gfx_present", "continuous": True,
     "input": ""},
]
BY_NAME = {s["name"]: s for s in SCENARIOS}


def get(name):
    if name not in BY_NAME:
        raise SystemExit("no scenario %s (xn_scenarios.py list)" % name)
    return dict(BY_NAME[name])


def snap_path(snap):
    return snap if os.path.exists(snap) else os.path.join(fallemu.SNAPS, snap + ".snap")


# ---- input -----------------------------------------------------------------------------------
def parse_input(text):
    """'FRAME ACTION ARG; ...' -> {frame: [(action, arg)]} (click/key expanded)."""
    out = {}

    def add(f, a, g):
        out.setdefault(f, []).append((a, g))
    for item in (text or "").replace("\n", ";").split(";"):
        w = item.split()
        if not w:
            continue
        f, act, arg = int(w[0]), w[1], w[2] if len(w) > 2 else None
        if act in ("click", "rclick"):
            x, y = (int(v) for v in arg.split(","))
            b = 1 if act == "click" else 2
            add(f, "mouse", (x, y, 0))
            add(f + 1, "mouse", (x, y, b))
            add(f + 3, "mouse", (x, y, 0))
        elif act == "key":
            add(f, "down", arg)
            add(f + 2, "up", arg)
        elif act == "mouse":
            add(f, "mouse", tuple(int(v) for v in arg.split(",")))
        elif act == "irqs":
            add(f, "irqs", int(arg))
        elif act in ("down", "up"):
            add(f, act, arg)
        else:
            raise SystemExit("unknown input action %r" % act)
    return out


def deliver(emu, vector, port60=None, cap=50_000_000):
    """Deliver hardware interrupt `vector` at the current instruction and run its handler to
    completion: until EIP is back where it was with ESP as it was. False when interrupts are
    off (nothing delivered)."""
    from unicorn import UC_HOOK_CODE
    if not emu.r("eflags") & 0x200:
        return False
    eip0, esp0 = emu.r("eip"), emu.r("esp")
    if port60 is not None:
        emu.port60 = port60
    if vector == 8:
        emu.ticks += 1
        emu.pit_reads = 0          # the count restarts with each interrupt
    emu.irq(vector)
    if emu.r("eip") == eip0 and emu.r("esp") == esp0:
        return True                 # the default handler (in Python) took it
    done = []
    esp_id = fallemu.R["esp"]

    def back(uc, address, size, _):
        if uc.reg_read(esp_id) == esp0:
            done.append(1)
            uc.emu_stop()
    hk = emu.uc.hook_add(UC_HOOK_CODE, back, None, eip0, eip0)
    try:
        n = 0
        while not done and n < cap:
            emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=200000)
            emu.clear_exception_state()
            n += 200000
    finally:
        emu.uc.hook_del(hk)
    if not done:
        raise RuntimeError("interrupt %02Xh's handler did not return" % vector)
    return True


# ---- one machine ------------------------------------------------------------------------------
class Side:
    """A machine in a lockstep run: its sync hooks, the frame's I/O log, its coverage."""

    def __init__(self, emu, sync_addrs, img=None, alias=None):
        self.emu, self.img = emu, img
        self.sync = sorted(set(sync_addrs))
        self.alias = alias or {}        # an asm entry -> its C entry (a sync function's)
        self.start_eip = emu.r("eip") if emu is not None else None
        self.cov = None

    def start_coverage(self):
        import xn_record
        lib = xn_record.coverage_lib()
        if lib is None:
            return False
        self.cov = lib
        lib.uc_dagger_coverage(self.emu.uc._uch, LOAD + OBJ2[0], LOAD + OBJ2[1], None)
        return True

    def coverage(self):
        """bytes over object 2: 1 where code was translated (ran)."""
        import ctypes
        if self.cov is None:
            return None
        buf = ctypes.create_string_buffer(OBJ2[1] - OBJ2[0])
        self.cov.uc_dagger_coverage(self.emu.uc._uch, LOAD + OBJ2[0], LOAD + OBJ2[1], buf)
        return buf.raw

    def inputs(self, events, irqs):
        """Deliver this frame's input at the sync point, every interrupt run to completion."""
        emu = self.emu
        n = 0
        for act, arg in events:
            if act == "mouse":
                emu.mouse = list(arg)
            elif act in ("down", "up"):
                code = fallemu.SCAN[arg] | (0x80 if act == "up" else 0)
                n += deliver(emu, 9, port60=code)
        for _ in range(irqs):
            n += deliver(emu, 8)
        return n

    def run_frame(self, cap=1_500_000_000, trap=None):
        """Step off the sync point, then run to the next one. Returns (instructions, how):
        how is "sync", "trap" (a direct call returned) or None (cap reached)."""
        from unicorn import UC_HOOK_CODE, UcError
        emu = self.emu
        hit = []
        armed = [False]
        # a machine resumed at a sync function's asm entry (a resync from the asm machine)
        # reaches that call's C entry next: the same call, not the next frame (the input
        # delivered there may already have moved EIP along the route: where the frame began
        # is what counts)
        skip = set()
        if self.start_eip in self.alias and emu.r("eip") != self.alias[self.start_eip]:
            skip.add(self.alias[self.start_eip])     # (at the C entry itself: stepped over)

        def stop(uc, address, size, _):
            if armed[0]:
                if address in skip:
                    skip.discard(address)
                    return
                hit.append(address)
                uc.emu_stop()
        hooks = [emu.uc.hook_add(UC_HOOK_CODE, stop, None, a, a) for a in self.sync]
        if trap is not None:
            hooks.append(emu.uc.hook_add(UC_HOOK_CODE, stop, None, trap, trap))
        fallemu.flush_caches(emu.uc)
        done = 0
        try:
            emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=1)   # off the sync point
            while emu.clear_exception_state() and not hit:
                emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=1)
            armed[0] = True
            while done < cap and not hit:
                emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=fallemu.TICK)
                while not hit and emu.clear_exception_state():
                    emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=fallemu.TICK)
                done += fallemu.TICK
                if emu.exit_code is not None:
                    break
                if emu.io_log is not None and len(emu.io_log) > 1_000_000:
                    break               # a wait loop polling a port: not a frame
        except (UcError, fallemu.Stop) as e:
            return done, "fault: %r at %08X" % (e, emu.r("eip"))
        finally:
            for h in hooks:
                emu.uc.hook_del(h)
        if not hit:
            return done, None
        return done, "trap" if trap is not None and hit[-1] == trap else "sync"


def sync_alias(spec, img):
    """{asm entry: C entry} of the scenario's fn: sync functions that have C."""
    out = {}
    for part in spec.get("sync", "safe").split("+"):
        if part.startswith("fn:") and img is not None:
            a, c = sync_addrs(dict(spec, sync=part), img)
            if c and c != a:
                out[a[0]] = c[0]
    return out


def sync_addrs(spec, img=None):
    """(asm machine's sync addresses, C machine's)."""
    s = spec.get("sync", "safe")
    if s == "safe":
        a = [LOAD + SAFE]
        return a, a
    if "+" in s:                        # several: a frame ends at the first of them
        a, c = [], []
        for part in s.split("+"):
            pa, pc = sync_addrs(dict(spec, sync=part), img)
            a += pa
            c += pc
        return a, c
    if s.startswith("fn:"):
        # the asm machine stops at the function's asm entry; the C machine at its C entry when
        # it has one (every call reaches it: the game's through the asm entry's route, the
        # C's directly; hooking both would count a routed call twice), else the asm entry
        name = s[3:]
        import xn_rc
        va = xn_rc.func_va(name)
        a = [LOAD + va]
        c = list(a)
        if img is not None:
            for sym in (name + "_", "_" + name, name):
                if sym in img.syms:
                    c = [img.syms[sym]]
                    break
        return a, c
    if s.startswith("addr:"):
        a = [LOAD + int(s[5:], 16)]
        return a, a
    raise SystemExit("unknown sync %r" % s)


# ---- the prelude -------------------------------------------------------------------------------
def prelude(emu, spec):
    """Game state, then the scripted ticks, then on to the sync point (asm only)."""
    for addr, data in spec.get("poke", ()):     # [[address, hex bytes]]
        emu.write(LOAD + int(addr, 16), bytes.fromhex(data))
    if spec.get("time") is not None:            # minutes after midnight
        m = struct.unpack("<I", emu.read(LOAD + 0x195BF4, 4))[0]
        emu.write(LOAD + 0x195BF4, struct.pack("<I", m - m % 1440 + spec["time"]))
    if spec.get("water") is not None:           # dungeon water this far below the eye
        po = struct.unpack("<I", emu.read(LOAD + 0x195AA4, 4))[0]
        y = struct.unpack("<i", emu.read(po + 11, 4))[0]
        emu.write(LOAD + 0x12DE00, struct.pack("<i", y + spec["water"]))
    if spec.get("teleport"):                    # [object, dx, dy, dz]: next to a world object
        obj, dx, dy, dz = spec["teleport"]
        po = struct.unpack("<I", emu.read(LOAD + 0x195AA4, 4))[0]
        x, y, z = struct.unpack("<iii", emu.read(LOAD + int(obj, 16) + 7, 12))
        emu.write(po + 7, struct.pack("<iii", x + dx, y + dy, z + dz))
    for argv in spec.get("play", ()):           # tools/fallplay.py steps: ["door", "13"]...
        import fallplay
        a = fallplay.build_parser().parse_args([argv[0], "scenario"] + list(argv[1:]))
        fallplay.step_action(a, None)[1](emu)
    start = emu.ticks
    script = fallemu.parse_script(spec.get("script", ""))
    emu.run(start + spec.get("ticks", 0), [(start + t, a, g) for t, a, g in script])
    # on to the sync point (a direct call starts from main's loop)
    target = [LOAD + SAFE] if spec.get("call") else sync_addrs(spec)[0]
    if not run_until(emu, target, 3000):
        raise RuntimeError("the game does not reach its sync point (%s)" % spec.get("sync", "safe"))


def run_until(emu, addrs, max_ticks):
    """Run (timer interrupts as usual) until EIP reaches one of addrs: a code hook stops it
    (emu_start's `until` is compiled into translated blocks). True if it got there."""
    from unicorn import UC_HOOK_CODE, UcError
    hit = []

    def stop(uc, address, size, _):
        hit.append(address)
        uc.emu_stop()
    hooks = [emu.uc.hook_add(UC_HOOK_CODE, stop, None, a, a) for a in addrs]
    fallemu.flush_caches(emu.uc)
    end = emu.ticks + max_ticks
    try:
        while emu.ticks < end and not hit:
            if emu.exit_code is not None:
                return False
            emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=fallemu.TICK)
            while not hit and emu.clear_exception_state():
                emu.uc.emu_start(emu.r("eip"), 0xFFFFFFFF, count=fallemu.TICK)
            if hit:
                break
            emu.insns += fallemu.TICK
            if emu.r("eflags") & 0x200:
                emu.ticks += 1
                emu.pit_reads = 0
                emu.irq(8)
    except (UcError, fallemu.Stop):
        return False
    finally:
        for h in hooks:
            emu.uc.hook_del(h)
    return bool(hit)


def start_call(emu, va, args):
    """Set up a direct call of va(args) (fallcall's convention: registers, a return to TRAP)."""
    esp = (emu.r("esp") - 0x400) & ~3
    regs, onstack = list(args[:4]), list(args[4:])
    for v in reversed(onstack):
        esp -= 4
        emu.uc.mem_write(esp, struct.pack("<I", v & 0xFFFFFFFF))
    esp -= 4
    emu.uc.mem_write(esp, struct.pack("<I", LOAD + TRAP))
    emu.w("esp", esp)
    for reg, v in zip(("eax", "edx", "ebx", "ecx"), regs):
        emu.w(reg, v & 0xFFFFFFFF)
    emu.w("eip", LOAD + va)


# ---- comparing two machines ---------------------------------------------------------------------
SCREEN = (0xA0000, 0xA0000 + 64000)


def mem_regions(emu):
    top = min(fallemu.MEM, ((getattr(emu, "brk", fallemu.MEM) + (8 << 20) + 0xFFFFF) >> 20) << 20)
    return ((0, fallemu.LOW), (LOAD, top))


def diff_bytes(a, b, esp, limit=1 << 20):
    """Addresses of the bytes that differ between machines a and b in low and program memory,
    leaving out the dead stack (the MB below ESP). At most `limit`."""
    out = []
    for base, size in mem_regions(a):
        ma, mb = a.read(base, size), b.read(base, size)
        if ma == mb:
            continue
        for off in range(0, size, 0x10000):
            if ma[off:off + 0x10000] == mb[off:off + 0x10000]:
                continue
            for p in range(off, min(off + 0x10000, size), 4096):
                if ma[p:p + 4096] == mb[p:p + 4096]:
                    continue
                if esp - (1 << 20) <= base + p < esp:
                    continue
                for j in range(p, p + 4096):
                    if ma[j] != mb[j]:
                        out.append(base + j)
                        if len(out) >= limit:
                            return out
    return out


def io_view(log, boundary):
    """The I/O log as compared: ports and services in order. boundary leaves out the CPU
    exceptions (XnGine's own divide-error handler takes them), and of what a service returns
    keeps the registers it defines (tools/xn_services.py) and the memory it writes: the other
    registers are the caller's own (C keeps other values in them than the asm)."""
    if not boundary:
        return log
    import xn_services
    out = []
    last = None
    for e in log:
        if e[0] == "exc":
            continue
        if e[0] == "int":
            last = (e[1], e[2])
            # by AX (AH alone when AL is no input): EAX's upper half is the caller's (xn_rc.service_ax)
            e = (e[0], e[1], e[2] & xn_services.eax_mask(e[1], e[2]))
        elif e[0] == "int-ret" and last is not None:
            keep = xn_services.replay_regs(*last)
            # the BIOS and the mouse driver answer in 16-bit registers: the upper halves are
            # the caller's
            wide = 0xFFFFFFFF if last[0] in (0x21, 0x31) else 0xFFFF
            regs = tuple((r, v if r in ("eflags", "es") else v & wide)
                         for r, v in zip(fallemu.Emu.SVC_REGS, e[1])
                         if keep is None or r in keep or r == "eflags" and keep & {"CF", "ZF"})
            if keep is not None and "eflags" not in keep:
                m = (1 if "CF" in keep else 0) | (0x40 if "ZF" in keep else 0)
                regs = tuple((r, v & m if r == "eflags" else v) for r, v in regs)
            e = ("int-ret", regs, e[2])
        out.append(e)
    return out


class Compare:
    """A frame's comparison of the asm machine and the C machine."""

    def __init__(self, masks=None):
        self.masks = masks

    def frame(self, asm, c, io_a, io_c):
        esp = asm.r("esp")
        diffs = diff_bytes(asm, c, esp)
        screen = not any(SCREEN[0] <= x < SCREEN[1] for x in diffs) and \
            asm.palette == c.palette and asm.mode == c.mode
        visible = diffs
        if self.masks is not None and diffs:
            priv = self.masks.resolve(asm)
            visible = [x for x in diffs if not priv.contains(x)]
        io_full = io_a == io_c
        io_b = io_view(io_a, True) == io_view(io_c, True)
        return {"screen": screen, "full": not diffs and screen, "io_full": io_full,
                "boundary": not visible and screen and io_b, "io": io_b,
                "diff_bytes": len(diffs), "visible_bytes": len(visible),
                "first": ["%08X" % x for x in visible[:4]],
                "first_any": ["%08X" % x for x in diffs[:4]]}


# ---- a lockstep run -----------------------------------------------------------------------------
def run(spec, img=None, masks=None, continuous=None, asm_only=False, on_asm=None,
        coverage=False, verbose=True, overlay_tag="", frames=None, shots=None):
    """Run one scenario. img: tools/xn_rc.py's RImage (the C), routed by img.game_routes() (the
    boundary routes and the functions still on asm interfaces) when it has them, else every
    route. asm_only: no C machine (the boundary's memory survey, coverage). on_asm(emu, phase,
    data): called with phase "frames" before the lockstep frames, "frame" after each (data: the
    asm's I/O log of the frame) and "end" after them (memory hooks).
    Returns {"name", "frames": [per-frame results], "identical", "total", "coverage"...}."""
    t0 = time.time()
    name = spec["name"]
    cont = spec.get("continuous", True) if continuous is None else continuous
    if spec.get("call"):
        cont = True     # a resync mid-call would run the asm's own loop around the C
    nframes = frames if frames is not None else spec.get("frames", 10)
    tag = "%s_%d%s" % (name, os.getpid(), overlay_tag)
    ov = os.path.join(ROOT, "build", "emu", "ov_scen", tag)
    asm = fallemu.Emu.load(snap_path(spec["snap"]), overlay=ov + "_asm")
    c = None
    state = os.path.join(ROOT, "build", "emu", "ov_scen", tag + ".snap")
    res = {"name": name, "frames": [], "covers": spec.get("covers", ""),
           "mode": "asm only" if asm_only else ("continuous" if cont else "resync")}
    try:
        prelude(asm, spec)
        sa, sc = sync_addrs(spec, img)
        alias = sync_alias(spec, img)
        A = Side(asm, sa)
        if coverage:
            A.start_coverage()
        if spec.get("call"):
            va, args = spec["call"]
            start_call(asm, va, args)
        if on_asm:
            on_asm(asm, "frames")
        cmp = Compare(masks)
        events = parse_input(spec.get("input", ""))
        irqs = spec.get("irqs", 1)
        trap = LOAD + TRAP if spec.get("call") else None
        def route(m):
            img.install(m)
            if hasattr(img, "route_game"):
                img.route_game(m)           # canonical functions only at the boundary
            else:
                img.route(m, sorted(img.funcs))
        if not asm_only and cont:
            asm.save(state)
            c = fallemu.Emu.load(state, overlay=ov + "_c")
            route(c)
        for k in range(nframes):
            ev = events.get(k, [])
            for act, arg in ev:
                if act == "irqs":
                    irqs = arg
            ev = [e for e in ev if e[0] != "irqs"]
            if not asm_only and not cont:
                asm.save(state)
                c = fallemu.Emu.load(state, overlay=ov + "_c")
                route(c)
            C = Side(c, sc, img, alias) if c is not None else None
            hits0 = img.hits if img is not None else 0
            asm.io_log = []
            if C:
                c.io_log = []
            A.inputs(ev, irqs)
            if C:
                C.inputs(ev, irqs)
            na, how_a = A.run_frame(trap=trap)
            nc, how_c = (C.run_frame(trap=trap) if C else (0, how_a))
            io_a, asm.io_log = asm.io_log, None
            io_c = None
            if C:
                io_c, c.io_log = c.io_log, None
            if on_asm:
                on_asm(asm, "frame", io_a)
            fr = {"frame": k, "asm_insns": na, "c_insns": nc, "how": how_a}
            if how_a not in ("sync", "trap") or how_c not in ("sync", "trap") or how_a != how_c:
                fr.update(ok=False, note="asm %s, C %s" % (how_a, how_c))
                res["frames"].append(fr)
                if verbose:
                    print("  frame %d: %s" % (k, fr["note"]), flush=True)
                break
            if shots:
                os.makedirs(shots, exist_ok=True)
                asm.screenshot(os.path.join(shots, "%s_%03d.png" % (name, k)))
            if C:
                fr.update(cmp.frame(asm, c, io_a, io_c))
                fr["c_entered"] = img.hits - hits0
                fr["ok"] = fr["boundary"]
                if verbose and (not fr["boundary"] or not fr["full"]):
                    print("  frame %d: %s%s%s (visible %d / %d bytes differ%s)" % (
                        k, "identical" if fr["boundary"] else "DIFFERS",
                        "" if fr["screen"] else ", screen", "" if fr["io"] else ", I/O",
                        fr["visible_bytes"], fr["diff_bytes"],
                        ": " + " ".join(fr["first"] or fr["first_any"])), flush=True)
                if not cont:
                    c.close()
                    c = None
            res["frames"].append(fr)
            if how_a == "trap":
                break
        if on_asm:
            on_asm(asm, "end")
        if coverage:
            res["coverage"] = A.coverage()
    finally:
        asm.close()
        if c is not None:
            c.close()
        import shutil
        for p in (ov + "_asm", ov + "_c"):
            shutil.rmtree(p, ignore_errors=True)
        if os.path.exists(state):
            os.remove(state)
    fr = res["frames"]
    res["total"] = len(fr)
    res["identical"] = sum(1 for f in fr if f.get("boundary"))
    res["full_identical"] = sum(1 for f in fr if f.get("full"))
    res["seconds"] = round(time.time() - t0, 1)
    res["complete"] = bool(fr) and (len(fr) == nframes or fr[-1].get("how") == "trap") and \
        all(f.get("how") in ("sync", "trap") for f in fr)
    return res


# ---- worker task (tools/xn_cload.py parallel) --------------------------------------------------
def _task(names, sp):
    """Run these scenarios (one worker): sp = {"image": path or None, "continuous", "masks",
    "coverage", "asm_only"}."""
    img = None
    if not sp.get("asm_only"):
        import xn_rc
        img = xn_rc.RImage(sp["image"]) if sp.get("image") else xn_rc.RImage()
    masks = None
    if sp.get("masks", True):
        import xn_boundary
        masks = xn_boundary.load_masks()
    out = []
    for n in names:
        spec = get(n)
        try:
            r = run(spec, img=img, masks=masks, continuous=sp.get("continuous"),
                    asm_only=sp.get("asm_only", False), coverage=sp.get("coverage", False),
                    verbose=sp.get("verbose", False), frames=sp.get("frames"))
        except Exception as e:      # noqa: BLE001  (a scenario that cannot start)
            r = {"name": n, "error": repr(e), "frames": [], "total": 0, "identical": 0,
                 "full_identical": 0, "complete": False}
        if r.get("coverage") is not None:
            import zlib
            import base64
            r["coverage"] = base64.b64encode(zlib.compress(r["coverage"], 6)).decode()
        if sp.get("asm_only"):
            print("%-14s %3d frames (asm only)%s %.0f s" % (
                n, r["total"], " ERROR " + r["error"] if r.get("error") else "",
                r.get("seconds", 0)), flush=True)
        else:
            print("%-14s %3d / %3d frames identical (%d in all of memory)%s %.0f s" % (
                n, r["identical"], r["total"], r["full_identical"],
                " ERROR " + r["error"] if r.get("error") else "", r.get("seconds", 0)), flush=True)
        out.append(r)
    return out


def montage(paths, out, cols=4):
    """The screens (fallemu's PNGs, 320x200) at half size in a grid, one PNG."""
    import zlib

    def read(p):
        d = open(p, "rb").read()
        pos, idat, w, h = 8, b"", 0, 0
        while pos < len(d):
            n = struct.unpack(">I", d[pos:pos + 4])[0]
            t, body = d[pos + 4:pos + 8], d[pos + 8:pos + 8 + n]
            if t == b"IHDR":
                w, h = struct.unpack(">II", body[:8])
            elif t == b"IDAT":
                idat += body
            pos += 12 + n
        raw = zlib.decompress(idat)
        return [raw[y * (w * 3 + 1) + 1:(y + 1) * (w * 3 + 1)] for y in range(h)]
    imgs = [read(p) for p in paths]
    rows_n = (len(imgs) + cols - 1) // cols
    lines = []
    for r in range(rows_n):
        for y in range(0, 200, 2):
            line = b""
            for c in range(cols):
                k = r * cols + c
                src = imgs[k][y] if k < len(imgs) else bytes(960)
                line += b"".join(src[x * 3:x * 3 + 3] for x in range(0, 320, 2))
            lines.append(line)
    w, h = 160 * cols, 100 * rows_n

    def chunk(t, d):
        c = struct.pack(">I", len(d)) + t + d
        return c + struct.pack(">I", zlib.crc32(t + d) & 0xFFFFFFFF)
    raw = b"".join(b"\0" + ln for ln in lines)
    with open(out, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", w, h, 8, 2, 0, 0, 0))
                + chunk(b"IDAT", zlib.compress(raw, 6)) + chunk(b"IEND", b""))


def main():
    ap = argparse.ArgumentParser()
    sub = ap.add_subparsers(dest="cmd", required=True)
    sub.add_parser("list")
    s = sub.add_parser("show")
    s.add_argument("name")
    sh = sub.add_parser("shots")
    sh.add_argument("name")
    sh.add_argument("-o", default=os.path.join(OUT, "shots"))
    sh.add_argument("--frames", type=int, default=None)
    a = ap.parse_args()
    if a.cmd == "list":
        for s in SCENARIOS:
            print("%-14s %-15s %4d frames  %s" % (s["name"], s["snap"], s.get("frames", 10),
                                                 s.get("covers", "")))
        return 0
    if a.cmd == "show":
        for k, v in get(a.name).items():
            print("%-10s %s" % (k, v))
        return 0
    if a.cmd == "shots":
        r = run(get(a.name), asm_only=True, shots=a.o, frames=a.frames)
        n = r["total"]
        pick = sorted({0, n // 4, n // 2, 3 * n // 4, max(0, n - 1)} | set(range(0, n, max(1, n // 8))))[:12]
        out = os.path.join(a.o, "%s_montage.png" % a.name)
        montage([os.path.join(a.o, "%s_%03d.png" % (a.name, k)) for k in pick], out)
        print("%d frames -> %s (frames %s in %s)" % (n, a.o, " ".join(map(str, pick)), out))
        return 0


if __name__ == "__main__":
    sys.exit(main())
