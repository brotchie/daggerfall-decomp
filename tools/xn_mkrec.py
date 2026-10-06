#!/usr/bin/env python3
"""The canonical phase's crafted records: direct calls of XnGine's asm functions with made-up
inputs, reaching what the corpus and the scenarios do not (dead functions, error paths, the
edges of clipping and decoding), recorded as tools/xn_record.py records into the record corpus
(build/xngine/records), where every tool (xn_rc.py test, xn_cover.py corpus...) sees them.

Each case loads a safe-point snapshot of build/xngine/bases afresh (its own DOS overlay under
build/xngine/ov), makes the Recorder (so what the case writes is part of the record's input
pages), writes its memory, and calls the function(s) the way tools/fallcall.py does, the
Recorder keeping the calls of the functions it lists. A file per job, in the corpus's format
(NAME.pkl and its summary NAME.json, as xn_record.write_job writes them).

  a     group A (system and input): one job a case, grpa_CASE (one record each): strings,
        flags, spells, the dead timer wait, the ASCR interpreter, the keyboard through the
        BIOS, the mouse's clamp, double click and cursor, DOS paths and palettes, the serial
        ring, the helmet drivers, the joystick, the zero-page check, the divide-error handler
        (a fault in a stub of the case's own). From safe_cheat_tlalac_s.
  b     group B (screen and 2D): one job a case, b_CASE (401 records in all): the overlay,
        darkened and shadowed rectangles, the paper doll's items, lines and rectangles cut by a
        smaller clip window, text, the scaled image at the window's edges, the VID player on
        shipped movies (sampled) and on two synthetic ones written into the overlay, masked
        and RLE images above the view, the dead remaps, the banked VESA present. From
        safe_cheat_tlalac_s.
  c     group C (the rasteriser and the frame): two jobs. group_c_span: the flats'
        translucent span (157E20 and its tail 157FA0), which never ran in play (it needs a
        ghost's or a wraith's flat in view): a crafted flat with 16-row texels and a
        translucency table drawn into a scratch row, counts 1..40, both u directions, two
        depths (From safe_cheat_tlalac_s). group_c_frame: the frame's clamps and full pools:
        the ambient row below 0 and beyond the last row, a screen not 320 wide, the frame
        with the texture cache full, a light's radius beyond 512, more than 32 lights, the
        texture mapper's pool full (from safe_save_tlalac_s).
  d     group D (the 3D objects): four jobs, group_d_tex / _model / _flat / _cam, a record a
        case: the texture cache full, an animated compiled image, a crafted archive marked
        translucent, loads with no backslash and with no room, the heap's first fits and
        merges; "v2.5" models, models with frames, degenerate faces, angles with no angles,
        a model drawn with the cache full and a framed one; a translucent flat's light setup
        and more than 32 lights; the side planes' distance from the inner side. From
        safe_save_tlalac_s.
  e     group E (world and collision): five jobs, group_e_collide / _world / _terrain / _sky
        / _water, a record a case: collision against crafted ARCH3D models, spheres, planes
        and flats; world streaming, the cell reader, heightmap noise, the dead editor's writes,
        path and river walkers; the terrain's light list; the rain streaks and stars; the
        water at levels above and below the eye. From safe_save_tlalac_s (outdoors) and
        safe_save_mord (a dungeon with water).

usage: xn_mkrec.py a|b|c|d|e|all [CASE ...] [--out DIR] [--list]
           regenerate a group's jobs (CASE: a case's or job's name, or a prefix of it)
       xn_mkrec.py check a|b|c|d|e|all [--out DIR] [--against DIR ...]
           replay every record of the group's jobs on the asm (exactly, xn_record.replay); with
           --against, compare them with the records of the same jobs there (functions, counts,
           and each record's entry, exit, writes and I/O)

One machine at a time (about 1 GB). a: 15 s (61 records); b: 2-3 minutes (the movies); c and
d: 15 s (86 records) and 5 s (29); e: 30 s (99 records). Regenerated, they are identical to the groups'
first ones.
"""
import argparse
import collections
import os
import shutil
import struct
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import fallemu  # noqa: E402  (first: it picks the Unicorn build)
import xn_record as R  # noqa: E402
from unicorn import UcError  # noqa: E402

ROOT = fallemu.ROOT
LOAD = L = fallemu.LOAD
OUT = R.OUT                                     # build/xngine/records: the corpus
SAFE = R.SAFE_SNAP                              # build/xngine/bases/safe_cheat_tlalac_s.snap
OUTDOOR = os.path.join("build", "xngine", "bases", "safe_save_tlalac_s.snap")
DUNGEON = os.path.join("build", "xngine", "bases", "safe_save_mord.snap")


def u32(v):
    return struct.pack("<I", v & 0xFFFFFFFF)


def overlay_of(job):
    """a job's own DOS overlay (empty: nothing left from an earlier run)"""
    ov = os.path.join(R.XN, "ov", "mkrec_" + job)
    shutil.rmtree(ov, ignore_errors=True)
    return ov


def load(base, job):
    """(machine, its overlay) from a snapshot (a path relative to the repository)"""
    ov = overlay_of(job)
    return R._load_base(os.path.join(ROOT, base), ov), ov


def done(emu, ov):
    emu.close()
    shutil.rmtree(ov, ignore_errors=True)


# ==== group A: system and input ==========================================================
def u16(v):
    return struct.pack("<H", v & 0xFFFF)


class Ctx:
    """A case's machine: the scratch page and helpers to put data there."""

    def __init__(self, emu):
        self.emu = emu
        self.buf = (emu.r("esp") - 0x8000) & ~0xFFF
        self.at = self.buf

    def put(self, data):
        """data into the scratch page; its linear address."""
        a = self.at
        self.emu.write(a, data)
        self.at += (len(data) + 15) & ~15
        return a

    def poke(self, va, data):
        self.emu.write(LOAD + va, data)

    def peek32(self, va):
        return struct.unpack("<I", self.emu.read(LOAD + va, 4))[0]

    def ticks(self):
        return struct.unpack("<I", self.emu.read(0x46C, 4))[0]


# ---- the cases -----------------------------------------------------------------------------
# name: (va, setup(ctx) -> (args, regs)); args go to EAX EDX EBX ECX, regs is a dict

def anim_state(c, script_off, request=0xFF, wait=0, pos_off=None, last_step=0xFFFF):
    """A struct xn_anim and an ASCR record in the scratch page: the record's state 0 enters at
    0x20, state 1 is 8000h (none); the script at 0x20 runs a random loop of 1..3 rounds of set
    mirror, set events, goto-if-events, then a frame:
      20: 00 01 03 00      loop_start lo 1 hi 3
      24: 09 01            set_mirror 1
      26: 02 01 00         set_events 1
      29: 08 01 00 2E 00   goto_if_events 1 -> 2E
      2E: 01 20 00         loop_end -> 20
      31: 80               frame 0"""
    rec = bytearray(0x40)
    struct.pack_into("<HHHHH", rec, 0, 0x40, 0x20, 2, 0x20, 0x8000)
    rec[0x20:0x32] = bytes.fromhex("0001030009010201000801002e0001200080")
    script = c.put(bytes(rec))
    a = bytearray(24)
    pos = 0 if pos_off is None else script + pos_off
    struct.pack_into("<HHIIHHHBBBBBB", a, 0, 0, 0, script, pos, 1, last_step, 0, wait, 0xFF,
                     request, 0, 0, 0)
    return c.put(bytes(a)), script


def case_anim_update(c):
    a, _s = anim_state(c, 0x20, request=1)
    return [a], {}


def case_anim_tick_stopped(c):
    a, _s = anim_state(c, 0x20)
    return [a], {}


def case_anim_tick_wait(c):
    a, _s = anim_state(c, 0x20, wait=3, pos_off=0x20)
    return [a], {}


def strs(c, *items):
    return [c.put(s) for s in items]


A_CASES = {
    # strings
    "str_copy_line_cr": (0xCE300, lambda c: (
        [c.put(bytes(32)), c.put(b"ab\rcd\0")], {})),
    "str_copy_alnum_caps": (0xCE3FD, lambda c: (
        [c.put(bytes(32)), c.put(b"AbC1 x\0")], {})),
    "str_find_char_n_hit": (0xCE435, lambda c: (
        [c.put(b"abcd\0"), ord("c"), 4], {})),
    "str_find_char_n_end": (0xCE435, lambda c: (
        [c.put(b"abcd\0"), ord("z"), 2], {})),
    "str_count_nonzero": (0xCE77F, lambda c: (
        [c.put(bytes([1, 0, 2, 0, 3])), 5], {})),
    "str_skip_fields_end": (0xCE790, lambda c: (
        [c.put(b"a,b\0"), ord(","), 3], {})),
    "str_find_byte_pair_hit": (0xCE7A7, lambda c: (
        [c.put(b"xyAB"), ord("A") | ord("B") << 8, 4], {})),
    "str_find_byte_pair_none": (0xCE7A7, lambda c: (
        [c.put(b"xyAB"), ord("Q") | ord("R") << 8, 3], {})),
    "str_find_nonzero_hit": (0xCE87B, lambda c: (
        [c.put(bytes([0, 0, 5, 0])), 4], {})),
    "str_char_lower": (0x153700, lambda c: ([ord("Q")], {})),
    "str_char_upper": (0x15370C, lambda c: ([ord("q")], {})),
    "str_length": (0x15374C, lambda c: ([c.put(b"abc\0")], {})),
    "str_find_char_none": (0x15377E, lambda c: ([c.put(b"abc\0"), ord("z")], {})),
    "str_from_int_neg": (0x153900, lambda c: ([-42, c.put(bytes(16)), 3], {})),
    "str_to_int_neg": (0x15392C, lambda c: ([c.put(b"-12\0")], {})),
    "str_to_int_plus": (0x15392C, lambda c: ([c.put(b"+7x\0")], {})),
    # flags, spells
    "bits_u8_set": (0xCE4A9, lambda c: ([c.put(bytes([0x01])), 0x04, 0], {})),
    "bits_u16_clear": (0xCE4B5, lambda c: ([c.put(u16(0xFFFF)), 0x0010, 1], {})),
    "spell_find_slot1": (0xCE4C4, lambda c: ([c.put(bytes([5, 0, 7, 0, 9, 0])), 7], {})),
    "spell_none": (0xCE4E0, lambda c: ([c.put(bytes([0xFF, 0, 0xFF, 0, 0xFF, 0]))], {})),
    "spell_slot2_only": (0xCE4E0, lambda c: ([c.put(bytes([0xFF, 0, 0xFF, 0, 5, 0]))], {})),
    # timer (dead). (xn_rand_seed_from_ticks reads 40006Ch, which faults: Q-RAND-01)
    "timer_wait_change": (0xCD38E, lambda c: ([c.put(u32(1)), c.put(u32(2))], {})),
    # animation
    "anim_update_none_entry": (0xC013B, case_anim_update),
    "anim_tick_stopped": (0xC019C, case_anim_tick_stopped),
    "anim_tick_wait": (0xC019C, case_anim_tick_wait),
}


def setup_kbd_bios(key):
    def f(c):
        c.poke(0x142306, b"\0")             # the handler not installed
        c.emu.kbd = [key] if key else []
        return [], {}
    return f


def setup_wait_key_handler(c):
    c.poke(0x142307, b"\x1e")               # a last key waiting (A)
    return [], {}


def setup_mouse_clamp(lo):
    def f(c):
        c.emu.mouse = [100, 100, 0]
        if lo:                              # the window right and below the mouse
            c.poke(0x12AF34, u16(150) + u16(300) + u16(150) + u16(190))
        else:                               # left and above
            c.poke(0x12AF34, u16(10) + u16(50) + u16(10) + u16(50))
        return [], {}
    return f


def setup_mouse_double(right):
    def f(c):
        now = c.ticks()
        c.emu.mouse = [100, 100, 2 if right else 1]
        c.poke(0x12AC00, b"\0")             # no button at the last poll
        if right:                           # the last right press: just after now, (99, 99)
            c.poke(0x12AF2C, u32(now + 2) + u16(99) + u16(99))
        else:                               # the last left press: just after now, (101, 100)
            c.poke(0x12AF24, u32(now + 2) + u16(101) + u16(100))
        return [], {}
    return f


def setup_cursor(x, y, hot):
    def f(c):
        c.poke(0x147964, u16(0))            # not drawn
        c.poke(0x12AF18, u16(hot) + u16(hot))
        return [x, y], {}
    return f


A_CASES.update({
    "kbd_read_key_bios_none": (0x1427A8, setup_kbd_bios(0)),
    "kbd_read_key_bios_key": (0x1427A8, setup_kbd_bios(0x1E)),
    "kbd_wait_key_bios": (0x142808, setup_kbd_bios(0x1E)),
    "kbd_wait_key_handler": (0x142808, setup_wait_key_handler),
    "mouse_clamp_low": (0x12B136, setup_mouse_clamp(True)),
    "mouse_clamp_high": (0x12B136, setup_mouse_clamp(False)),
    "mouse_double_left": (0x12B196, setup_mouse_double(False)),
    "mouse_double_right": (0x12B196, setup_mouse_double(True)),
    "mouse_cursor_top_left": (0x12B2D3, setup_cursor(5, 5, 20)),
    "mouse_cursor_right": (0x12B2D3, setup_cursor(312, 100, 0)),
})


def case_palette(name, buf):
    def f(c):
        nm = c.put(name + b"\0")
        return [nm, c.put(bytes(0x300)) if buf else 0], {}
    return f


def case_path_eax(path, reg):
    def f(c):
        p = c.put(path + b"\0")
        return ([p], {}) if reg == "eax" else ([p, p], {})
    return f


def case_make_path(c):
    c.poke(0x1917E4, b"C:\\ARENA2\0")     # no backslash at its end
    return [0, c.put(b"MAP.PAL\0")], {}


def ring(c, port, data, head=0):
    """port's receive ring holding data from head (count, tail set)"""
    k = port - 1
    c.poke(0x160050 + 512 * k + head, data)
    c.poke(0x160850 + 4 * k, u32(head))
    c.poke(0x160860 + 4 * k, u32(head + len(data)))
    c.poke(0x160870 + 4 * k, u32(len(data)))


def case_rx_get(c):
    ring(c, 1, b"\x41\x42")
    return [1], {}


def case_a_read(n):
    def f(c):
        c.poke(0x160D00, u32(1))            # driver A's port
        ring(c, 1, bytes(range(0x30, 0x30 + n - 2)) + b"\r\n")
        return [], {}
    return f


def b_packet(yaw, pitch, roll):
    body = struct.pack(">hhh", yaw, pitch, roll)
    return b"\xff" + body + bytes([(0xFF + sum(body)) & 0xFF])


def case_b_read(c):
    c.poke(0x153400, u32(1))                # the front end's port
    c.poke(0x160F18, u32(1))                # B's yaw sign
    ring(c, 1, b"\x00\x00" + b_packet(0x0102, 0x0304, 0x0506) + bytes(9))
    return [], {}


def case_b_version(c):
    c.poke(0x153400, u32(1))
    ring(c, 1, b"xxM1.00 F1.003 \0")
    return [], {}


def case_helmet_open_c(c):
    return [1, 8], {}


def case_helmet_poll_a(c):
    c.poke(0x153400, u32(1))                # port 1
    c.poke(0x153404, u32(0))                # driver A
    c.poke(0x153408, b"\x01")               # active
    c.poke(0x160D00, u32(1))
    ring(c, 1, bytes(6))
    # A's read gives no angles: the asm smooths the poll's caller's EDX and EBX (Q-HELMET-02,
    # dropped in the C, which smooths 0): 0 here
    return [], {"edx": 0, "ebx": 0}


def case_helmet_smooth(c):
    c.poke(0x153409, u32(9))                # smoothing: clamped to 7
    c.poke(0x153461, u32(6 * 12))           # the last slot: the position wraps
    # the ring's yaws far from the sample's (negative): taken half a turn nearer
    c.poke(0x153465, b"".join(struct.pack("<iii", 10 * k, 0x6000, k) for k in range(7)))
    return [5, -0x2000, 7], {}


A_CASES.update({
    "dos_load_palette_pal": (0xC0C00, case_palette(b"MAP.PAL", True)),
    "dos_load_palette_col": (0xC0C00, case_palette(b"ART_PAL.COL", False)),
    "dos_open_c": (0xC0C7C, case_path_eax(b"C:\\ARENA2\\MAP.PAL", "eax")),
    "dos_file_size_c": (0xC0DEC, case_path_eax(b"C:\\ARENA2\\ART_PAL.COL", "eax")),
    "dos_file_size": (0xC0DFC, case_path_eax(b"C:\\ARENA2\\MAP.PAL", "edx")),
    "dos_make_path_noslash": (0xC0ED0, case_make_path),
    "serial_rx_get": (0x160A9C, case_rx_get),
    "helmet_a_read": (0x160EAC, case_a_read(6)),
    "helmet_a_read_text": (0x160E6F, case_a_read(18)),
    "helmet_b_read": (0x16116C, case_b_read),
    "helmet_b_parse_version": (0x161114, case_b_version),
    "helmet_open_c_fails": (0x153500, case_helmet_open_c),
    "helmet_poll_a": (0x15356C, case_helmet_poll_a),
    "helmet_smooth_wrap": (0x1535A4, case_helmet_smooth),
})


def joy(c, raw, center, lo, hi, raw_b=(5, 6)):
    """the joystick on, with these counts, centres and ranges (x, y) for stick A"""
    c.poke(0x152A01, b"\x01\x01")          # installed, status 1 (on)
    c.poke(0x152A04, u32(center[0]) + u32(center[1]) + u32(10))     # centre, dead zone 10
    c.poke(0x152A10, u32(lo[0]) + u32(hi[0]) + u32(lo[1]) + u32(hi[1]))
    c.poke(0x152A28, u32(raw[0]) + u32(raw[1]))
    c.poke(0x152A40, u32(32000) + u32(0) + u32(32000) + u32(0))     # stick B's ranges reset
    c.poke(0x152A58, u32(raw_b[0]) + u32(raw_b[1]))


def case_joy_poll(raw, center, lo, hi):
    def f(c):
        joy(c, raw, center, lo, hi)
        return [], {}
    return f


def case_zero_page_vectors(c):
    """the copy of linear 0 differing at vectors 0, 1 and 8 (which may change): no failure"""
    v = c.emu.read(0, 0x24)
    junk = bytes(x ^ 0x5A for x in v)
    c.poke(0xCDE4F, junk[0:8])
    c.poke(0xCDE4F + 0x20, junk[0x20:0x24])
    return [], {}


def case_div_fault(code):
    """a divide by 0 with a memory operand, run from the scratch page: XnGine's handler steps
    over it (by its ModRM: Q-SYS-02)"""
    def f(c):
        zero = c.put(bytes(16))
        stub = c.put(code(zero) + b"\xC3")  # ...; ret
        c.case_va = stub - LOAD
        return [1, 0], {"ebp": zero - 8}
    return f


def case_serial_timeout(c):
    c.poke(0x160004, u32(0x3C))             # COM1's base: its line status (41h) reads 0
    return [1, ord("x")], {}


def case_b_wait(byte):
    def f(c):
        c.poke(0x153400, u32(1))
        ring(c, 1, byte)
        return [], {}
    return f


def case_b_reset(c):
    c.poke(0x153400, u32(1))
    ring(c, 1, b"O" * 4)
    return [], {}


def case_c_close(c):
    ok, = struct.unpack("<I", c.emu.read(LOAD + 0xC5478, 4))
    c.poke(0x161332, u32(ok & 0xFFFF))      # a DOS block's selector (the transfer buffer's)
    return [], {}


A_CASES.update({
    "joy_poll_clamp_a": (0x152D00, case_joy_poll((0x100, 0x700), (0x400, 0x400), (32000, 32000), (0, 0))),
    "joy_poll_clamp_b": (0x152D00, case_joy_poll((0x700, 0x100), (0x400, 0x400), (32000, 32000), (0, 0))),
    "joy_poll_inside": (0x152D00, case_joy_poll((0x500, 0x300), (0x400, 0x400), (0, 0), (0x800, 0x800))),
    "sys_zero_page_vectors": (0xCE8D5, case_zero_page_vectors),
    "mem_init_small": (0x149E00, lambda c: ([0x100], {})),
    "sys_div_fault_disp8": (None, case_div_fault(lambda z: b"\xF7\x7D\x08")),
    "sys_div_fault_disp32": (None, case_div_fault(lambda z: b"\xF7\x3D" + u32(z))),
    "serial_send_timeout": (0x160A54, case_serial_timeout),
    "helmet_b_wait_ok": (0x161250, case_b_wait(b"O")),
    "helmet_b_wait_not_ok": (0x161250, case_b_wait(b"X")),
    "helmet_b_reset": (0x161094, case_b_reset),
    "helmet_c_close": (0x1614D0, case_c_close),
})


def run_a(names, out):
    """Each case from a fresh copy of the safe snapshot: one record of its function (for a
    case that runs code of its own, va None: the divide-error handler it faults into)."""
    for name in names:
        va, setup = A_CASES[name]
        job = "grpa_" + name
        t0 = time.time()
        emu, ov = load(SAFE, job)
        c = Ctx(emu)
        hook = 0x149FC8 if va is None else va
        rec = R.Recorder(emu, per=1, only=[hook], base=SAFE, name=job, state="grpa:" + name)
        notes = []
        try:
            args, regs = setup(c)
            va = getattr(c, "case_va", va)
            for k, v in regs.items():
                emu.w(k, v & 0xFFFFFFFF)
            ok = R.call_job(emu, rec, va, args, False, 60)
        except Exception as e:          # noqa: BLE001
            ok = False
            notes.append(repr(e))
        finally:
            done(emu, ov)
        for r in rec.records:
            r["probe"] = "grpa"
        spec = {"name": job, "kind": "probe", "tool": "tools/xn_mkrec.py a", "load": SAFE,
                "vas": ["%X" % hook], "state": "grpa:" + name}
        R.write_job(out, job, spec, os.path.join(ROOT, SAFE), rec.records, rec.store, set(),
                    rec.count, rec.failed, rec.long, notes, t0, 0, None, 0, 0, 1)
        print("%-34s %06X %s: %d record(s)%s" % (
            job, hook, "returned" if ok else "DID NOT RETURN", len(rec.records),
            " " + "; ".join(notes) if notes else ""), flush=True)


# ==== group B: screen and 2D =============================================================
# globals (preferred addresses)
SCREEN_BUFFER = 0x143550
SCRATCH = 0x195C44
COLOR_REMAP = 0x195B80
TEXT_COLOUR = 0x12B508
CLIP = 0x142940             # left, top, right, bottom (dwords)
FONT_GLYPHS = 0x12DA74

# functions
B_F = {
    "overlay": 0x0CB552, "darken": 0x0CD0F1, "shadow": 0x0CD1C5, "paperdoll_item": 0x0CD291,
    "cast": 0x0CDB7A, "line_tc": 0x0CE4FA, "draw_string": 0x12DBCC, "glyph": 0x12DC44,
    "scaled": 0x0C0700, "scaled_clip": 0x0C08CC, "scaled_row": 0x0C094A, "unpack": 0x0C0A36,
    "put_rect": 0x144ED8, "get_rect": 0x144E84, "clip_rect": 0x144E00, "image": 0x144F68,
    "image_t": 0x144FB4, "image_t_regs": 0x144FC8, "row_t": 0x14501C, "image_regs": 0x144F7C,
    "line": 0x1531F0, "line_nosave": 0x153204, "line_unclipped": 0x153210,
    "line_clip": 0x1532D4, "hline": 0x15310C, "vline": 0x153188, "masked_remap": 0x0CB473,
    "note_open": 0x04D1E6, "note_close": 0x04D904, "note_update": 0x04D402,
    "vid_play": 0x0C1500, "vid_open": 0x0C1654, "vid_update": 0x0C17D6, "vid_finish": 0x0C1844,
    "vid_decode": 0x0C18A3, "vid_close": 0x0C1B1A, "vid_refill": 0x0C1B43, "vid_fill": 0x0C1B9A,
    "vid_schedule": 0x0C1BBE, "vid_audio": 0x0C1C4A, "vid_timer": 0x0C1CDC, "vid_stop": 0x0C1D4D,
    "masked_remap": 0x0CB473, "cast": 0x0CDB7A, "set_mode": 0x143600,
    "remap_rect": 0x147848, "remap_rect_regs": 0x147858, "rle_unpack": 0x0CB4D7,
    "remap_colours": 0x147820, "present_banked": 0x15FEAE, "blit_banked": 0x15FF53,
}
ARENA2_PATH = 0x1917E4
VID = ["vid_open", "vid_update", "vid_finish", "vid_decode", "vid_close", "vid_refill", "vid_fill",
       "vid_schedule", "vid_audio", "vid_timer", "vid_stop"]


class Machine:
    """A group B case's machine: the safe snapshot, its own overlay (synthetic movies are
    written into its ARENA2), and a Recorder of the case's functions."""

    def __init__(self, name, only, per=4, floor=None, gap=0):
        self.name = name
        self.t0 = time.time()
        self.emu, self.overlay = load(SAFE, name)
        self.rec = R.Recorder(self.emu, per=per, only=[B_F[o] if isinstance(o, str) else o
                                                       for o in only],
                              base=SAFE, name=name, state=name, gap=gap,
                              known={} if floor is not None else None, floor=floor)

    def u32(self, va):
        return struct.unpack("<I", self.emu.read(LOAD + va, 4))[0]

    def w(self, lin, data):
        self.emu.write(lin, data)

    def w32(self, va, v):
        self.emu.write(LOAD + va, struct.pack("<I", v & 0xFFFFFFFF))

    def w8(self, va, v):
        self.emu.write(LOAD + va, bytes([v & 0xFF]))

    def scratch(self, off=0):
        return self.u32(SCRATCH) + off

    def clip(self, l, t, r, b):
        self.w32(CLIP, l)
        self.w32(CLIP + 4, t)
        self.w32(CLIP + 8, r)
        self.w32(CLIP + 12, b)

    def call(self, va, args, stack=False, ticks=600):
        f = B_F[va] if isinstance(va, str) else va
        return R.call_job(self.emu, self.rec, f, list(args), stack, ticks)

    def save(self, out, case):
        rec = self.rec
        spec = {"name": self.name, "kind": "call", "tool": "tools/xn_mkrec.py b", "case": case}
        R.write_job(out, self.name, spec, os.path.join(ROOT, SAFE), rec.records, rec.store,
                    set(), rec.count, rec.failed, rec.long, [], self.t0, 0, None, 0, rec.nbase, 1)
        done(self.emu, self.overlay)
        return len(rec.records)


def pattern(n, seed, dark_every=3):
    """n pixel bytes: colours above 0Fh with every dark_every-th one 0..0Fh (and some 0)"""
    out = bytearray()
    x = seed
    for k in range(n):
        x = (x * 1103515245 + 12345) & 0x7FFFFFFF
        out.append((x >> 16) & 0x0F if k % dark_every == 0 else 0x10 + ((x >> 16) % 0xE0))
    return bytes(out)


# ---- the cases --------------------------------------------------------------------------------
def case_overlay_note(m):
    """the notebook (mask02i0.img: 53,256 pixels of colour 0-0Fh) drawn by note_update over the
    view: the overlay's shading path, from the game's own code"""
    m.call("note_open", [100])
    m.call("note_close", [])
    m.call("note_open", [1])
    m.w8(0x12AC00, 0)                  # mouse_buttons: no click
    m.call("note_update", [])


def case_overlay_direct(m):
    """the overlay with a synthetic image: dark and light pixels in every row"""
    src = m.scratch(0x9000)
    m.w(src, pattern(64000, 7))
    m.call("overlay", [src])


def case_darken(m):
    """the info popup's darkened rectangle, and one of a single row"""
    m.call("darken", [20, 30, 100, 40])
    m.call("darken", [0, 0, 320, 1])


def case_shadow(m):
    """the potion maker's drop-shadow blit: rows 256 bytes apart, some 0 (transparent)"""
    src = m.scratch(0x9000)
    img = bytearray()
    for y in range(24):
        img += bytes((0 if (x + y) % 5 == 0 else 0x40 + x % 0xB0) for x in range(256))
    m.w(src, bytes(img))
    m.call("shadow", [40, 50, 30, 24, src], stack=False)


def case_paperdoll_item(m):
    """a row-compressed paperdoll item with colour FFh (the background shows through)"""
    base = m.scratch(0xA000)
    w, h = 20, 6
    table = base + 0x1C
    rows_at = 0x1C + 4 * h
    data = bytearray(rows_at)
    for y in range(h):
        struct.pack_into("<I", data, 0x1C + 4 * y, rows_at + w * y)
    for y in range(h):
        data += bytes((0xFF if x % 4 == 0 else 0 if x % 7 == 0 else 0x30 + y) for x in range(w))
    m.w(base, bytes(data))
    m.call("paperdoll_item", [100, 60, w, h, table])


def case_lines(m):
    """the list frames' lines in every direction (x- and y-major, up and down, leftwards), and
    clipped and unclipped lines through every edge of a smaller clip window"""
    m.w8(TEXT_COLOUR, 0x9C)
    for x1, y1, x2, y2 in [(10, 10, 60, 30), (60, 30, 10, 10), (10, 30, 60, 10), (10, 10, 30, 60),
                           (30, 60, 10, 10), (10, 60, 30, 10), (5, 5, 5, 40), (5, 40, 5, 5),
                           (5, 5, 50, 5), (50, 5, 5, 5)]:
        m.call("line_tc", [x1, y1, x2, y2])
    m.clip(40, 30, 280, 170)
    for x1, y1, x2, y2 in [(0, 0, 319, 199), (319, 0, 0, 199), (10, 100, 300, 120),
                           (300, 120, 10, 100), (150, 0, 170, 199), (170, 199, 150, 0),
                           (0, 100, 30, 120), (290, 10, 300, 20), (100, 30, 200, 30),
                           (100, 100, 100, 100), (100, 50, 101, 160), (100, 50, 220, 51)]:
        m.call("line", [x1, y1, x2, y2])
        m.call("line_nosave", [x1, y1, x2, y2])
    m.clip(0, 0, 320, 200)
    for x1, y1, x2, y2 in [(10, 10, 60, 30), (60, 30, 10, 12), (10, 10, 30, 60), (30, 60, 10, 10),
                           (20, 20, 20, 50), (20, 20, 70, 20)]:
        m.call("line_unclipped", [x1, y1, x2, y2])


def case_rects(m):
    """rectangles cut by every edge of a smaller clip window, and wholly outside it"""
    buf = m.scratch(0x9000)
    m.w(buf, pattern(0x4000, 3))
    m.clip(40, 30, 280, 170)
    for x, y, w, h in [(20, 50, 60, 20), (250, 50, 60, 20), (100, 10, 30, 40), (100, 150, 30, 40),
                       (20, 10, 300, 190), (300, 50, 10, 10), (100, 180, 10, 10), (0, 0, 30, 20),
                       (100, 0, 10, 20), (60, 60, 0, 10), (60, 60, 10, -2)]:
        m.call("put_rect", [x, y, w, h, buf, 3], stack=False)
        m.call("get_rect", [x, y, w, h, buf, 2], stack=False)
        m.call("image", [x, y, w, h, buf], stack=False)
        m.call("image_t", [x, y, w, h, buf], stack=False)
    m.clip(0, 0, 320, 200)


def case_text(m):
    """text with CR and LF, and glyphs cut by the clip window's edges"""
    s = m.scratch(0x9000)
    m.w(s, b"Ab\rCd\nEf gh\n\x80\xa1!~\0")
    m.w8(TEXT_COLOUR, 0x9C)
    m.call("draw_string", [20, 20, s])
    w = m.scratch(0x9100)
    m.w(w, b"MWMW\0")
    m.clip(40, 30, 280, 170)
    for x, y in [(36, 50), (30, 50), (100, 25), (275, 60), (100, 165), (20, 26)]:
        m.call("draw_string", [x, y, s])
    for x in range(30, 42):
        m.call("draw_string", [x, 60 + x, w])
    for x in range(268, 281):
        m.call("draw_string", [x, 40 + x - 268, w])
    m.clip(0, 0, 320, 200)


def case_scaled(m):
    """the scaled image at the window's edges: cut on the left, top, right and bottom, a column
    count cut to nothing, and starting right of and below it"""
    src = m.scratch(0x9000)
    img = bytearray()
    for y in range(32):
        img += bytes((0 if (x * y) % 9 == 0 else 0xFF if x % 11 == 0 else 0x20 + x % 0xD0) for x in range(256))
    m.w(src, bytes(img))
    m.clip(40, 30, 280, 170)
    for x, y, w, h, flags in [(260, 60, 64, 48, 0), (20, 20, 64, 48, 0x8000), (100, 150, 96, 64, 0),
                              (270, 100, 40, 40, 0), (279, 100, 40, 40, 0), (300, 100, 40, 40, 0),
                              (100, 190, 40, 40, 0), (30, 25, 20, 20, 0x8000), (90, 90, 200, 20, 0)]:
        m.call("scaled", [x, y, w, h, 32, 32, flags, src], stack=False)
    m.clip(0, 0, 320, 200)


def movie_path(m, name):
    base = bytes(m.emu.read(LOAD + ARENA2_PATH, 80)).split(b"\0")[0]
    path = m.scratch(0xF000)
    m.w(path, base + name + b"\0")
    return path


def case_vid_missing(m):
    """a movie whose file is missing: nothing played, the buffers freed"""
    m.call("vid_play", [movie_path(m, b"NOSUCH00.VID"), 0, 0, 1])


def vid_case(name, x):
    def case(m):
        m.call("vid_play", [movie_path(m, name), x, 0, 0], ticks=4000)
    case.__doc__ = "the movie %s at x = %d: its decoding, a call at a time" % (name.decode(), x)
    return case


def synth_vid():
    """A small VID movie (64 x 40) made to reach the decoder's edges: raw frames (no shipped
    movie has one) crossing the read buffer's end, deltas from a start row and RLE frames whose
    commands start past the refill mark, an audio header with a delay, audio chunks crossing the
    buffer's end, and header flag bit 0 (keep the file open)."""
    w, h = 64, 40
    out = bytearray(b"VID\0" + bytes([2]))
    hdr_at = len(out)
    out += struct.pack("<HHHHH", 0, w, h, 1, 0x0F)
    frames = 0
    out += bytes([2]) + bytes((k * 7) % 64 for k in range(768))

    def audio(n, seed):
        return struct.pack("<H", n) + bytes((seed + k) & 0xFF for k in range(n))
    out += bytes([0x7C]) + struct.pack("<HB", 3, 0xA6) + audio(900, 1)

    def raw(seed):
        return bytes([0]) + struct.pack("<H", 1) + bytes(((x * 3 + y * 5 + seed) % 200) + 16
                                                       for y in range(h) for x in range(w))

    def delta_from(start, seed):
        b = bytearray([4]) + struct.pack("<HH", 1, start)
        for r in range(h - start):
            b += bytes([0x80 + 10]) + bytes([20]) + bytes((seed + r + k) & 0x7F | 0x10 for k in range(20))
            b += bytes([0x80 + 34])
        return bytes(b) + bytes([0])           # the row-end byte (version 2)

    def rle(seed):
        b = bytearray([3]) + struct.pack("<H", 1)
        for r in range(h):
            b += bytes([0x80 + 30, (seed + r) & 0xFF | 1]) + bytes([34]) + bytes(
                ((seed + k) & 0x7F) | 0x10 for k in range(34))
        return bytes(b)

    def delta(seed):
        b = bytearray([1]) + struct.pack("<H", 1)
        b += bytes([0x80 + 70]) + bytes([100]) + bytes(((seed + k) & 0x7F) | 0x10 for k in range(100))
        return bytes(b) + bytes([0])
    seq = []
    for k in range(60):
        kind = k % 4
        seq.append(raw(k) if kind == 0 else delta_from(5 + k % 7, k) if kind == 1 else
                   rle(k) if kind == 2 else delta(k))
        if k % 3 == 1:
            seq.append(bytes([0x7D]) + audio(700 + 37 * (k % 5), k))
    for c in seq:
        out += c
        if c[0] in (0, 1, 3, 4):
            frames += 1
    out += bytes([0x14])
    struct.pack_into("<H", out, hdr_at, frames + 1)
    return bytes(out)


def synth_vid_big():
    """A 320 x 200 synthetic movie whose chunks are bigger than what is left of the read buffer:
    a delta from row 0 copying every pixel, an RLE frame of literal runs (each crosses the
    buffer's end mid-frame), audio chunks of 16000 bytes read one after another (the last
    crosses it), and a raw frame."""
    w, h = 320, 200
    out = bytearray(b"VID\0" + bytes([2]))
    hdr_at = len(out)
    out += struct.pack("<HHHHH", 0, w, h, 1, 0x0E)
    out += bytes([2]) + bytes((k * 5) % 64 for k in range(768))
    out += bytes([0x7C]) + struct.pack("<HB", 5, 0xA6) + struct.pack("<H", 500) + bytes(500)
    frames = 0

    def px(k, seed):
        return ((k * 7 + seed) % 200) + 16
    # a delta from row 0: copy runs of 64 bytes (5 a row)
    b = bytearray([4]) + struct.pack("<HH", 1, 0)
    for r in range(h):
        for c in range(5):
            b += bytes([64]) + bytes(px(r * 320 + c * 64 + k, 2) for k in range(64))
    out += b + bytes([0])
    frames += 1
    # an RLE frame of literal runs (and a fill a row)
    b = bytearray([3]) + struct.pack("<H", 1)
    for r in range(h):
        b += bytes([0x80 + 64, 0x33])
        for c in range(4):
            b += bytes([64]) + bytes(px(r * 320 + c * 64 + k, 3) for k in range(64))
    out += b
    frames += 1
    # a small delta, then four audio chunks of 16000 bytes read at once after it
    out += bytes([1]) + struct.pack("<H", 1) + bytes([0x80 + 10, 3, 1, 2, 3, 0])
    frames += 1
    for k in range(4):
        out += bytes([0x7D]) + struct.pack("<H", 16000) + bytes((k + j) & 0xFF for j in range(16000))
    out += bytes([1]) + struct.pack("<H", 1) + bytes([0x80 + 10, 3, 4, 5, 6, 0])
    frames += 1
    # last, a raw frame (it crosses the buffer's end: Q-VID-04 garbles what follows)
    out += bytes([0]) + struct.pack("<H", 1) + bytes(px(k, 1) for k in range(w * h))
    frames += 1
    out += bytes([0x14])
    struct.pack_into("<H", out, hdr_at, frames + 1)
    return bytes(out)


def case_vid_synth_big(m):
    """the 320 x 200 synthetic movie (synth_vid_big) played from the overlay"""
    d = os.path.join(m.overlay, "ARENA2")
    os.makedirs(d, exist_ok=True)
    with open(os.path.join(d, "BSYNTH2.VID"), "wb") as f:
        f.write(synth_vid_big())
    m.call("vid_play", [movie_path(m, b"BSYNTH2.VID"), 0, 0, 0], ticks=4000)


def case_vid_synth(m):
    """the synthetic movie (synth_vid) played from the overlay"""
    d = os.path.join(m.overlay, "ARENA2")
    os.makedirs(d, exist_ok=True)
    with open(os.path.join(d, "BSYNTH.VID"), "wb") as f:
        f.write(synth_vid())
    m.call("vid_play", [movie_path(m, b"BSYNTH.VID"), 100, 50, 0], ticks=4000)


def case_masked_remap(m):
    """a remapped masked IMG starting above the view's top (Q-DRAW-10's row)"""
    base = m.scratch(0x9000)
    w, h = 16, 12
    img = struct.pack("<HHHHHH", 50, 10, w, h, 0, w * h) + bytes(
        (0 if (x + y) % 3 == 0 else 0x50 + x) for y in range(h) for x in range(w))
    m.w(base, img)
    m.clip(0, 20, 320, 200)
    m.call("masked_remap", [base, 4])
    m.clip(0, 0, 320, 200)


def case_cast(m):
    """the casting hands' RLE image starting above the screen (row_start's negative rows)"""
    base = m.scratch(0x9000)
    w, h = 24, 10
    rows = bytearray()
    for y in range(h):
        rows += bytes([11]) + bytes((0 if x % 3 == 0 else 0x60 + y) for x in range(12))
        rows += bytes([0x80 + 11, 0x70 + y])
    img = struct.pack("<HHHHHH", 40, 2, w, h, 0, len(rows)) + bytes(rows)
    m.w(base, img)
    m.call("cast", [base, 0, -6])


def case_dead(m):
    """the dead routines with no record: a block remapped in place (both entries), an RLE group's
    frame unpacked, an IMG's EGA colours remapped"""
    buf = m.scratch(0x9000)
    m.w(buf, pattern(0x2000, 5))
    table = (m.scratch(0xC000) + 0xFF) & ~0xFF
    m.w(table, bytes((255 - k) for k in range(256)))
    m.call("remap_rect", [buf, 7, 33, 9, table], stack=False)
    g = m.scratch(0xD000)
    w, h = 20, 5
    runs = bytearray()
    for y in range(h):
        runs += bytes([9]) + bytes(0x40 + y + k for k in range(10)) + bytes([0x80 + 9, 0x77])
    hdr = struct.pack("<6H", w, h, 0, 3, 4, 0) + struct.pack("<31H", *([0x4C] + [0] * 30)) + \
        struct.pack("<H", 0x4C + len(runs))
    m.w(g, hdr + bytes(runs))
    m.call("rle_unpack", [g, 0, m.scratch(0xE000)])
    img = m.scratch(0xE800)
    m.w(img, struct.pack("<6H", 0, 0, 8, 4, 0, 32) + bytes((k * 3) % 40 for k in range(32)))
    m.call("remap_colours", [img])


def case_vesa(m):
    """the banked VESA present and the dead banked blit, called directly in mode 13h: the bank
    switches go through DPMI 0300h as they would"""
    src = m.scratch(0x9000)
    m.w(src, pattern(0x2000, 9))
    m.call("present_banked", [1])
    m.call("blit_banked", [10, 20, 30, 8, src])


def case_set_mode(m):
    """xn_gfx_set_mode for a mode that is not 13h without VESA: refused (1)"""
    m.call("set_mode", [0x101, 1])


B_CASES = {
    "overlay_note": (case_overlay_note, ["overlay"]),
    "overlay_direct": (case_overlay_direct, ["overlay"]),
    "darken": (case_darken, ["darken"]),
    "shadow": (case_shadow, ["shadow"]),
    "paperdoll_item": (case_paperdoll_item, ["paperdoll_item", "unpack"]),
    "lines": (case_lines, ["line_tc", "line", "line_nosave", "line_unclipped", "line_clip", "hline",
                           "vline"]),
    "rects": (case_rects, ["put_rect", "get_rect", "clip_rect", "image", "image_t",
                           "image_t_regs", "image_regs", "row_t"]),
    "text": (case_text, ["draw_string", "glyph"]),
    "scaled": (case_scaled, ["scaled", "scaled_clip", "scaled_row"]),
    "vid_missing": (case_vid_missing, ["vid_play"]),
    "vid_0007": (vid_case(b"ANIM0007.VID", 0), VID),
    "vid_0011": (vid_case(b"ANIM0011.VID", 0), VID),
    "vid_dag2": (vid_case(b"DAG2.VID", 32), VID),
    "vid_synth": (case_vid_synth, [f for f in VID if f != "vid_update"]),
    "vid_synth_big": (case_vid_synth_big, [f for f in VID if f != "vid_update"]),
    "masked_remap": (case_masked_remap, ["masked_remap"]),
    "cast": (case_cast, ["cast"]),
    "set_mode": (case_set_mode, ["set_mode"]),
    "dead": (case_dead, ["remap_rect", "rle_unpack", "remap_colours"]),
    "vesa": (case_vesa, ["present_banked", "blit_banked"]),
}
SAMPLED = {"vid_0007", "vid_0011", "vid_dag2"}


# ==== group E: world and collision =======================================================
# object-2 globals (config/names.csv)
EG = {
    "cam_x": 0xC23C4, "cam_y": 0xC23C8, "cam_z": 0xC23CC,
    "world_slot": 0xC28F4, "world_cell_header": 0xC2879, "world_noise": 0xC2882,
    "world_offsets_window_max": 0xC2346, "world_elev_min": 0xC28B4, "world_elev_max": 0xC28B8,
    "world_height_layer": 0xC28BC, "world_tile_layer": 0xC28C4, "world_eye_col": 0xC23E0,
    "world_eye_row": 0xC23E4, "world_nature_archive": 0xC287D, "world_width": 0xC27ED,
    "world_height": 0xC27F1, "light_count": None, "water_level": 0x12DE00,
    "screen_buffer": None, "big_buffer": None,
}


def vec(x, y, z):
    return struct.pack("<iii", x, y, z)


def rd32(emu, va):
    return struct.unpack("<I", emu.read(L + va, 4))[0]


def rd16(emu, va):
    return struct.unpack("<H", emu.read(L + va, 2))[0]


def sym(name):
    """an object-2 or game global's address by name (config/names.csv)"""
    import csv
    for r in csv.DictReader(open(os.path.join(ROOT, "config", "names.csv"), newline="")):
        if r["name"] == name:
            return int(r["address"], 16)
    raise KeyError(name)


def run_call(emu, rec, va, regs, stack, max_ticks=200, result=None):
    """Call va with the registers regs (a dict) and the stack arguments, recording; as
    xn_record._call_run does. Returns whether it returned; result (a list): EAX at the return
    is appended to it."""
    ctx = emu.uc.context_save()
    try:
        esp = (emu.r("esp") - 0x400) & ~3
        for v in reversed(stack):
            esp -= 4
            emu.uc.mem_write(esp, u32(v))
        esp -= 4
        emu.uc.mem_write(esp, u32(L + R.TRAP))
        emu.w("esp", esp)
        for reg, v in regs.items():
            emu.w(reg, v & 0xFFFFFFFF)
        emu.w("eip", L + va)
        target = L + R.TRAP
        end = emu.ticks + max_ticks
        while emu.ticks < end and emu.exit_code is None:
            try:
                emu.uc.emu_start(emu.r("eip"), target, count=fallemu.TICK)
                while emu.r("eip") != target and emu.clear_exception_state():
                    emu.uc.emu_start(emu.r("eip"), target, count=fallemu.TICK)
            except (fallemu.Stop, UcError):
                return False
            if emu.r("eip") == target:
                if result is not None:
                    result.append(emu.r("eax"))
                return True
            if rec is not None and rec.after_slice():
                if rec.faulted:  # noqa
                    return False
                continue
            emu.insns += fallemu.TICK
            if not emu.r("eflags") & 0x200:
                continue
            emu.ticks += 1
            emu.pit_reads = 0
            emu.irq(8)
        return False
    finally:
        emu.uc.context_restore(ctx)


class Buf:
    """A scratch area below the stack (the call's own stack stays above it)."""

    def __init__(self, emu):
        self.emu = emu
        self.base = (emu.r("esp") - 0x9000) & ~0xFFF
        emu.write(self.base, bytes(0x4000))
        self.at = self.base

    def put(self, data, align=4):
        self.at = (self.at + align - 1) & ~(align - 1)
        a = self.at
        self.emu.write(a, data)
        self.at += len(data)
        assert self.at <= self.base + 0x4000
        return a


# ---- crafted collision data -------------------------------------------------------------------

def model(b, spheres=1, half=0x4000, two=False):
    """An ARCH3D model: a square face of side 2 * half (24.8) in the plane z = 0, normal +z, one
    collision sphere at the origin listing it (or none); two: a second square beside it (+x,
    at z = 0x1000). Returns (model, face, points, normal)."""
    pts = vec(-half, -half, 0) + vec(half, -half, 0) + vec(half, half, 0) + vec(-half, half, 0)
    face = bytes([4, 0]) + struct.pack("<H", 0) + u32(0)
    for k in range(4):
        face += u32(k * 12) + u32(0)
    if two:
        x0 = 3 * half
        pts += vec(x0 - half, -half, 0x1000) + vec(x0 + half, -half, 0x1000)
        pts += vec(x0 + half, half, 0x1000) + vec(x0 - half, half, 0x1000)
        face += bytes([4, 0]) + struct.pack("<H", 0) + u32(0)
        for k in range(4, 8):
            face += u32(k * 12) + u32(0)
    sph = vec(0, 0, 0) + u32(0x10000) + struct.pack("<H", 1)
    sph_off = 0x40
    pts_off = sph_off + len(sph) + 6 + 2
    nrm_off = pts_off + len(pts)
    face_off = nrm_off + 12 * (2 if two else 1)
    sph += struct.pack("<iH", face_off, 0)
    nf = 2 if two else 1
    hdr = u32(0x372E3276) + u32(4 * nf) + u32(nf) + u32(half * 2 * nf) + u32(0) + u32(0) + u32(0)
    hdr += u32(sph_off) + u32(spheres) + u32(0) + bytes(8) + u32(pts_off) + u32(nrm_off)
    hdr += u32(0) + u32(face_off)
    data = hdr + sph + bytes(pts_off - sph_off - len(sph)) + pts + vec(0, 0, 256)
    if two:
        data += vec(0, 0, 256)
    data += face
    m = b.put(data)
    return m, m + face_off, m + pts_off, m + nrm_off


def handle(b, m, pos, angles=(0, 0, 0)):
    h = u32(m) + bytes(0x1C) + vec(*pos) + vec(*angles) + bytes(2)
    return b.put(h)


def probe(b, pos, spheres, angles=(0, 0, 0)):
    data = vec(*pos) + vec(*angles) + struct.pack("<H", len(spheres))
    for s in spheres:
        data += struct.pack("<iiii", *s)
    return b.put(data)


# ---- the cases --------------------------------------------------------------------------------

def collide_cases():
    c = []

    def seg_model_mode1(emu, b):
        m, _f, _p, _n = model(b)
        h = handle(b, m, (1000, 2000, 3000))
        s = b.put(vec(1000, 2000, 2900))
        e = b.put(vec(1000, 2000, 3100))
        return {"eax": h, "edx": s, "ebx": e, "ecx": 1}, []
    c.append(("segment_model_mode1", 0x14A300, OUTDOOR, seg_model_mode1))

    def seg_model_hit(emu, b):
        m, _f, _p, _n = model(b)
        h = handle(b, m, (1000, 2000, 3000), (0, 0x100, 0))
        s = b.put(vec(1010, 2000, 2900))
        e = b.put(vec(990, 2000, 3100))
        return {"eax": h, "edx": s, "ebx": e, "ecx": 0}, []
    c.append(("segment_model_hit", 0x14A300, OUTDOOR, seg_model_hit))

    def model_model(far, mode):
        def f(emu, b):
            m, _f, _p, _n = model(b)
            a = handle(b, m, (1000, 2000, 3000))
            o = handle(b, m, (1000 + far, 2000, 3000))
            return {"eax": a, "edx": o, "ebx": mode}, []
        return f
    c.append(("model_model_miss", 0x14A6C0, OUTDOOR, model_model(5000, 0)))
    c.append(("model_model_hit", 0x14A6C0, OUTDOOR, model_model(100, 0)))
    c.append(("model_model_mode2", 0x14A6C0, OUTDOOR, model_model(100, 2)))

    def spheres_model(mode, off):
        def f(emu, b):
            m, _f, _p, _n = model(b)
            h = handle(b, m, (1000, 2000, 3000))
            p = probe(b, (1000 + off, 2000, 3000 - 10), [(0, 0, 0, 20), (5, 5, 5, 8)])
            return {"eax": h, "edx": p, "ebx": mode}, []
        return f
    c.append(("spheres_model_hit", 0x14AA92, OUTDOOR, spheres_model(0, 0)))
    c.append(("spheres_model_mode2", 0x14AA92, OUTDOOR, spheres_model(2, 0)))
    c.append(("spheres_model_miss", 0x14AA92, OUTDOOR, spheres_model(0, 4000)))

    def seg_spheres(hit):
        def f(emu, b):
            p = probe(b, (100, 200, 300), [(0, 0, 0, 10), (50, 0, 0, 10), (0, 60, 0, 12)],
                      (0, 0, 0) if hit else (0x80, 0x100, 0))
            s = b.put(vec(100 - 500, 260 if hit else 900, 300))
            e = b.put(vec(100 + 500, 260 if hit else 900, 300))
            return {"eax": p, "edx": s, "ebx": e}, []
        return f
    c.append(("segment_spheres_hit", 0x14AF30, OUTDOOR, seg_spheres(True)))
    c.append(("segment_spheres_miss", 0x14AF30, OUTDOOR, seg_spheres(False)))

    def spheres_spheres(off):
        def f(emu, b):
            a = probe(b, (100, 200, 300), [(0, 0, 0, 10), (30, 0, 0, 10)])
            o = probe(b, (100 + off, 200, 300), [(0, 0, 0, 15), (0, 20, 0, 5), (-20, 0, 0, 6)],
                      (0, 0x200, 0))
            return {"eax": a, "edx": o}, []
        return f
    c.append(("spheres_spheres_hit", 0x14B017, OUTDOOR, spheres_spheres(0)))
    c.append(("spheres_spheres_near", 0x14B017, OUTDOOR, spheres_spheres(15)))
    c.append(("spheres_spheres_miss", 0x14B017, OUTDOOR, spheres_spheres(900)))

    def build(r, two=False):
        def f(emu, b):
            m, _f, _p, _n = model(b, two=two)
            out = b.put(bytes(0x800))
            return {"eax": m, "edx": r, "ebx": out}, []
        return f
    c.append(("build_spheres", 0x14B58E, OUTDOOR, build(0x2000)))
    c.append(("build_spheres_small", 0x14B58E, OUTDOOR, build(0x1000)))
    c.append(("build_spheres_toomany", 0x14B58E, OUTDOOR, build(0x10)))
    c.append(("build_spheres_nodes", 0x14B58E, OUTDOOR, build(0xEA)))
    c.append(("build_spheres_two", 0x14B58E, OUTDOOR, build(0x1800, True)))

    def face_regs(fn_regs):
        def f(emu, b):
            _m, face, pts, nrm = model(b)
            return fn_regs(b, face, pts, nrm), []
        return f
    for name, p in (("in", (0x100, -0x200, 0)), ("out", (0x5000, 0, 0)), ("edge", (0x4000, 0, 0))):
        c.append(("point_in_face_v2_" + name, 0x15CC36, OUTDOOR, face_regs(
            lambda b, face, pts, nrm, p=p: {"eax": p[0], "edx": p[1], "ebx": p[2], "ecx": face,
                                             "esi": pts, "edi": nrm})))
    for name, (p, r) in (("near", ((0x4000, 0x4000, 0x100), 0x400)), ("far", ((0, 0, 0x9000), 0x100))):
        c.append(("face_test_vertices_" + name, 0x15CEDE, OUTDOOR, face_regs(
            lambda b, face, pts, nrm, p=p, r=r: {"eax": p[0], "edx": p[1], "ebx": p[2],
                                                  "ecx": face, "esi": pts, "edi": r})))
    c.append(("face_area", 0x15CD33, OUTDOOR, face_regs(
        lambda b, face, pts, nrm: {"eax": face, "edx": pts})))

    def ppl(n1, n2):
        def f(emu, b):
            a = b.put(vec(*n1))
            p1 = b.put(vec(0x100, 0x200, 0x300))
            o = b.put(vec(*n2))
            p2 = b.put(vec(-0x400, 0x500, 0x80))
            return {"eax": a, "edx": p1, "ebx": o, "ecx": p2, "ebp": 0}, []
        return f
    c.append(("plane_plane_x", 0x14C4A6, OUTDOOR, ppl((0, 0x10000, 0), (0, 0, 0x10000))))
    c.append(("plane_plane_y", 0x14C4A6, OUTDOOR, ppl((0x10000, 0, 0), (0, 0, 0x10000))))
    c.append(("plane_plane_z", 0x14C4A6, OUTDOOR, ppl((0x10000, 0, 0), (0, 0x10000, 0))))
    c.append(("plane_plane_skew", 0x14C4A6, OUTDOOR, ppl((0xB505, 0xB505, 0), (0, 0x9000, -0xD000))))
    c.append(("plane_plane_negz", 0x14C4A6, OUTDOOR, ppl((0, 0x10000, 0), (0x10000, 0, 0))))
    c.append(("plane_plane_negx", 0x14C4A6, OUTDOOR, ppl((0, 0, 0x10000), (0, 0x10000, 0))))
    c.append(("plane_plane_negy", 0x14C4A6, OUTDOOR, ppl((0, 0, 0x10000), (0x10000, 0, 0))))

    def segplane(p0, p1, n=(0, 0x10000, 0), va=0x14C2AD):
        def f(emu, b):
            c_ = b.put(vec(0, 0, 0))
            a = b.put(vec(*p0))
            e = b.put(vec(*p1))
            return {"eax": n[0], "edx": n[1], "ebx": n[2], "ecx": c_, "esi": a, "edi": e}, []
        return f
    c.append(("segment_plane_parallel", 0x14C2AD, OUTDOOR, segplane((0, -1, 0), (1000, 1, 0))))
    c.append(("segment_plane_vparallel", 0x14C2AD, OUTDOOR, segplane((0, -1, 0), (0, 1, 0))))
    c.append(("segment_plane_vcross", 0x14C2AD, OUTDOOR, segplane((5, -100, 7), (5, 300, 7))))
    c.append(("vsegment_plane_cross", 0x14C3D9, OUTDOOR, segplane((5, -100, 7), (5, 300, 7))))
    c.append(("vsegment_plane_parallel", 0x14C3D9, OUTDOOR, segplane((0, -1, 0), (0, 1, 0))))
    c.append(("line_plane", 0x14C111, OUTDOOR, segplane((5, -100, 7), (50, 300, 70))))
    c.append(("vline_plane", 0x14C1DD, OUTDOOR, segplane((5, -100, 7), (5, 300, 7))))

    def cyl(p):
        def f(emu, b):
            c_ = b.put(vec(0, 0, 0))
            return {"eax": p[0], "edx": p[1], "ebx": p[2], "ecx": c_, "esi": 100, "edi": 100}, []
        return f
    c.append(("cylinder_in", 0x14CAEA, OUTDOOR, cyl((5, 10, 5))))
    c.append(("cylinder_out", 0x14CAEA, OUTDOOR, cyl((500, 10, 5))))

    def sph_plane(va, n, s):
        def f(emu, b):
            c_ = b.put(vec(0, 0, 0))
            s_ = b.put(vec(*s))
            return {"eax": n[0], "edx": n[1], "ebx": n[2], "ecx": c_, "esi": s_, "edi": 50}, []
        return f
    c.append(("sphere_plane_xz_neg", 0x14C762, OUTDOOR, sph_plane(0x14C762, (0x10000, 0, 0), (-30, 0, 0))))
    c.append(("sphere_plane_xz_pos", 0x14C762, OUTDOOR, sph_plane(0x14C762, (0, 0, 0x10000), (0, 0, 20))))
    c.append(("sphere_plane_yz_neg", 0x14C79D, OUTDOOR, sph_plane(0x14C79D, (0, 0x10000, 0), (0, -70, 0))))
    c.append(("sphere_plane_neg", 0x14C711, OUTDOOR, sph_plane(0x14C711, (0, 0, 0x10000), (0, 0, -20))))
    c.append(("plane_distance", 0x14C25F, OUTDOOR, sph_plane(0x14C25F, (0x10000, 0x8000, 0x4000), (30, 40, 50))))

    def boxes(va, e):
        def f(emu, b):
            bb = b.put(vec(10, 20, 30))
            ee = b.put(vec(5, 6, 7)) if e is None else e
            return {"eax": 12, "edx": 23, "ebx": 33, "ecx": ee, "esi": bb}, []
        return f
    c.append(("point_in_box", 0x14CB24, OUTDOOR, boxes(0x14CB24, None)))
    c.append(("point_in_cube", 0x14CB4F, OUTDOOR, boxes(0x14CB4F, 8)))

    def approx(va):
        def f(emu, b):
            p0 = b.put(vec(0, 0, 0))
            p1 = b.put(vec(400, 300, 0))
            return {"eax": 200, "edx": 150, "ebx": 0, "ecx": 30, "esi": p0, "edi": p1}, []
        return f
    c.append(("segment_sphere_approx_mid", 0x14C9D2, OUTDOOR, approx(0x14C9D2)))
    c.append(("sphere_sphere_approx", 0x14CA83, OUTDOOR, approx(0x14CA83)))

    def ref(emu, b):
        _m, face, pts, nrm = model(b)
        b.put(vec(0, 0, 0x10000))         # a normal no register points at (kept: the input pages)
        return {"eax": 0, "edx": 0, "ebx": 0x10000, "ecx": face, "esi": pts, "edi": nrm}, []
    c.append(("ref_helpers", 0x14B1B7, OUTDOOR, ref))

    def flat(mode, end, start=None):
        def f(emu, b):
            arch = rd16(emu, EG["world_nature_archive"])
            pos = b.put(vec(1000, 2000, 3000))
            s = b.put(vec(*(start or (1000, 2000 - 10, 3000 - 100))))
            e = b.put(vec(*end))
            return {"eax": pos, "edx": s, "ebx": e, "ecx": arch << 7 | 3, "esi": 0, "edi": 0x100,
                    "ebp": mode}, []
        return f
    c.append(("segment_flat_hit", 0x14B1E0, OUTDOOR, flat(0, (1000, 2000 - 10, 3000 + 100))))
    c.append(("segment_flat_mode2", 0x14B1E0, OUTDOOR, flat(2, (1000, 2000 - 10, 3000 + 100))))
    c.append(("segment_flat_above", 0x14B1E0, OUTDOOR, flat(0, (1000, 2000 - 200, 3000 + 60),
                                                         (1000, 2000 - 200, 3000 - 60))))

    def flat_stk(emu, b):
        arch = rd16(emu, EG["world_nature_archive"])
        pos = b.put(vec(1000, 2000, 3000))
        s = b.put(vec(1000, 2000 - 10, 3000 - 100))
        e = b.put(vec(1000, 2000 - 10, 3000 + 100))
        return {"eax": pos, "edx": s, "ebx": e, "ecx": arch << 7 | 3}, [0, 0x100, 0]
    c.append(("segment_flat_stk_hit", 0x14B1C7, OUTDOOR, flat_stk))
    return c


def world_cases():
    c = []

    def update(dx, dz):
        def f(emu, b):
            x, z = rd32(emu, EG["cam_x"]), rd32(emu, EG["cam_z"])
            size_z = rd32(emu, sym("xn_world_size_z"))
            sx = (x & ~0x7FFF) + (0x100 if dx < 0 else 0x7F00 if dx > 0 else 0x4000)
            zz = size_z - z
            sz = (zz & ~0x7FFF) + (0x100 if dz < 0 else 0x7F00 if dz > 0 else 0x4000)
            emu.write(L + EG["cam_x"], u32(sx))
            emu.write(L + EG["cam_z"], u32(size_z - sz))
            return {}, []
        return f
    for name, d in (("west", (-1, 0)), ("east", (1, 0)), ("north", (0, -1)), ("south", (0, 1)),
                    ("nw", (-1, -1)), ("se", (1, 1))):
        c.append(("world_update_" + name, 0x0C2E05, OUTDOOR, update(*d)))

    def read_header(emu, b):
        emu.write(L + EG["world_offsets_window_max"], u32(0x7FFFFFFF))
        return {}, []
    c.append(("world_read_header_whole", 0x0C322D, OUTDOOR, read_header))

    def read_cell(which):
        def f(emu, b):
            w, h = rd32(emu, EG["world_width"]), rd32(emu, EG["world_height"])
            return {"edi": w * h - 1 if which == "last" else 0}, []
        return f
    c.append(("world_read_cell_last", 0x0C3301, OUTDOOR, read_cell("last")))
    c.append(("world_read_cell_first", 0x0C3301, OUTDOOR, read_cell("first")))

    def heightmap(level, amp):
        def f(emu, b):
            bb = rd32(emu, sym("big_buffer"))
            grid = bytearray(0x100 + 129 * 256)
            for y in range(0, 129, 32):
                for x in range(0, 129, 32):
                    grid[0x100 + y * 256 + x] = level
            emu.write(bb, bytes(grid))
            hdr = bytearray(emu.read(L + EG["world_cell_header"], 22))
            hdr[9] = amp << 5
            hdr[0:4] = u32(0x1234567)
            emu.write(L + EG["world_cell_header"], bytes(hdr))
            # the noise as xn_world_read_cell leaves it for the cell (the game always reads
            # the cell just before)
            emu.write(L + sym("xn_world_noise_amp"), u32(amp))
            emu.write(L + sym("xn_world_noise_bias"), u32(amp >> 1))
            emu.write(L + EG["world_slot"], u32(4))
            return {}, []
        return f
    c.append(("world_heightmap_high", 0x0C355D, OUTDOOR, heightmap(0xFF, 7)))
    c.append(("world_heightmap_low", 0x0C355D, OUTDOOR, heightmap(0x00, 7)))

    def elev(emu, b):
        emu.write(L + EG["world_elev_min"], u32(40))
        emu.write(L + EG["world_elev_max"], u32(60))
        out = b.put(bytes(32))
        return {"edi": out}, []
    c.append(("world_random_elevations", 0x0C3515, OUTDOOR, elev))

    def new_file(emu, b):
        """the world file replaced by a new one (xn_dos_create_c, not recorded): the editor's
        writes need a file open for writing"""
        name = b.put(b"GROUPE.WLD\0")
        got = []
        run_call(emu, None, 0x0C0D44, {"eax": name}, [], result=got)
        emu.write(L + sym("xn_world_file"), u32(got[0]))

    def write_header(emu, b):
        new_file(emu, b)
        return {}, []
    c.append(("world_write_header", 0x0C31DB, OUTDOOR, write_header))

    def write_cell(emu, b):
        new_file(emu, b)
        data = b.put(bytes(range(47)))
        return {"ecx": 47, "edx": data, "edi": 0}, []
    c.append(("world_write_cell", 0x0C33F4, OUTDOOR, write_cell))

    def walker(va, seed, starts=True):
        def f(emu, b):
            import random
            rnd = random.Random(seed)
            hl = rd32(emu, EG["world_height_layer"])
            tl = rd32(emu, EG["world_tile_layer"])
            emu.write(L + EG["world_slot"], u32(0))
            for row in range(130):
                emu.write(hl + row * 256, bytes(rnd.randrange(0, 40) for _ in range(130)))
                emu.write(tl + row * 256, bytes(rnd.randrange(0, 4) for _ in range(130)))
            if starts:
                hdr = bytearray(emu.read(L + EG["world_cell_header"], 22))
                hdr[10:18] = struct.pack("<HHHH", 0, 0x0208, 0x0C10, 0)
                emu.write(L + EG["world_cell_header"], bytes(hdr))
            return {"ebp": seed % 8}, []
        return f
    for k in range(3):
        c.append(("world_paths_body_%d" % k, 0x0C3DA9, OUTDOOR, walker(0x0C3DA9, k + 1)))
        c.append(("world_river_body_%d" % k, 0x0C3E6A, OUTDOOR, walker(0x0C3E6A, k + 11)))

    def river_pit(emu, b):
        """a pit at (121, 121) below (120, 120): the river steps into it, finds nothing lower,
        and its fallback direction (EBP 5: up and left) goes straight back: the pit is filled"""
        hl = rd32(emu, EG["world_height_layer"])
        tl = rd32(emu, EG["world_tile_layer"])
        emu.write(L + EG["world_slot"], u32(0))
        for row in range(130):
            emu.write(hl + row * 256, bytes([50] * 130))
            emu.write(tl + row * 256, bytes([1] * 130))
        emu.write(hl + 120 * 256 + 120, bytes([30]))
        emu.write(hl + 121 * 256 + 121, bytes([10]))
        return {"ebp": 5}, []
    c.append(("world_river_body_pit", 0x0C3E6A, OUTDOOR, river_pit))
    return c


def terrain_cases():
    c = []

    def lights(n):
        def f(emu, b):
            emu.write(L + sym("xn_light_count"), u32(n))
            return {}, []
        return f
    c.append(("terrain_light_list_many", 0x13E63C, OUTDOOR, lights(40)))
    return c


def sky_cases():
    c = []

    def stars(emu, b):
        sb = rd32(emu, sym("screen_buffer"))
        emu.write(sb, b"\xDF" * 64000)
        return {}, []
    c.append(("sky_draw_stars_sky", 0x0C81F2, OUTDOOR, stars))

    def streak(dy):
        """a streak dy rows from the view's top (Q-SKY-01 for dy < -30)"""
        def f(emu, b):
            top = rd32(emu, sym("xn_gfx_clip_top"))
            return {"eax": 100, "edx": (top + dy) & 0xFFFFFFFF}, []
        return f
    for dy in (-40, -31, -30, -5):
        c.append(("sky_rain_streak_%d" % dy, 0x0C9D19, OUTDOOR, streak(dy)))

    def bottom_clip(n):
        """the dead bottom-clip entry with a run of n pixels after the cut (Q-SKY-03: 31)"""
        def f(emu, b):
            bottom = rd32(emu, sym("xn_gfx_clip_bottom"))
            sb = rd32(emu, sym("screen_buffer"))
            return {"eax": sb + 50 * 320 + 60, "ecx": n, "edx": bottom - 1,
                    "esi": L + sym("xn_rain_streak_colours")}, []
        return f
    for n in (10, 31, 40, -3):
        c.append(("sky_rain_bottom_clip_%d" % n, 0x0C9D63, OUTDOOR, bottom_clip(n)))
    return c


def water_cases():
    c = []

    def level(dy):
        def f(emu, b):
            po = struct.unpack("<I", emu.read(L + 0x195AA4, 4))[0]
            y = struct.unpack("<i", emu.read(po + 11, 4))[0]
            emu.write(L + EG["water_level"], struct.pack("<i", y + dy))
            return {}, []
        return f
    for dy in (2, 40, -40, -2, 600, -600):
        c.append(("water_draw_%d" % dy, 0x12F79C, DUNGEON, level(dy)))
        c.append(("water_clip_%d" % dy, 0x12F59C, DUNGEON, level(dy)))

    def narrow(dy):
        """the clip window smaller than the view: the water's rows are cut to it"""
        def f(emu, b):
            level(dy)(emu, b)
            emu.write(L + sym("xn_gfx_clip_top"), u32(20))
            emu.write(L + sym("xn_gfx_clip_bottom"), u32(180))
            return {}, []
        return f
    for dy in (200, -200):
        c.append(("water_draw_narrow_%d" % dy, 0x12F79C, DUNGEON, narrow(dy)))
    return c


E_JOBS = {
    "group_e_collide": collide_cases,
    "group_e_world": world_cases,
    "group_e_terrain": terrain_cases,
    "group_e_sky": sky_cases,
    "group_e_water": water_cases,
}


def run_b(names, out):
    for n in names:
        fn, only = B_CASES[n]
        t0 = time.time()
        m = Machine("b_" + n, only, per=150) if n.startswith("vid_synth") else \
            Machine("b_" + n, only, per=60, floor=3, gap=8) if n in SAMPLED else \
            Machine("b_" + n, only, per=60)
        try:
            fn(m)
        finally:
            k = m.save(out, n)
        print("%-34s %3d records in %.0f s" % ("b_" + n, k, time.time() - t0), flush=True)


def run_e(names, out):
    run_jobs(E_JOBS, names, out, "group_e", "tools/xn_mkrec.py e")


# ==== group C: the rasteriser and the frame ==============================================
# The flat spans' translucent routine, which never ran in play (it needs a ghost's or a
# wraith's flat, archives 273 and 278, in view), and the frame's clamps and full pools.
C_CENTRE_X = 0xCEA30


def c_flat_record(texels, table, u_offset, v_offset, du_dx, dv_dx, u_dx=0, u_c=0, v_dx=0,
                  v_c=0):
    """a struct xn_flat (xnstruct.h) as the spans read it"""
    f = bytearray(100)
    struct.pack_into("<iiI", f, 0x08, du_dx, dv_dx, 0)          # +08 du_dx, +0C dv_dx, +10 row
    struct.pack_into("<ii", f, 0x18, u_offset, v_offset)
    struct.pack_into("<iiiiii", f, 0x24, u_dx, 0, u_c, v_dx, 0, v_c)
    struct.pack_into("<I", f, 0x40, texels)
    struct.pack_into("<I", f, 0x4C, table)
    return bytes(f)


def c_flat_translucent(params):
    """A crafted flat (100 bytes) with 16-row texels (colours 0..15, a fifth of them 0:
    transparent) and a 16 x 256 translucency table, drawn into a scratch row of 64 pixels"""
    n, du8, mirror, z_index, x, seed = params

    def setup(emu, b):
        import random
        rng = random.Random(seed)
        texels = bytes((rng.randrange(16) if rng.random() > 0.2 else 0) for _ in range(16 * 256))
        table = bytes(rng.randrange(256) for _ in range(16 * 256))
        pix = bytes(rng.randrange(256) for _ in range(64))
        a_tex = b.put(texels, 256)
        a_tab = b.put(table, 256)
        a_pix = b.put(pix, 16)
        z = 0x1000000 // z_index                    # the 1/z table's entry at inv_z >> 13
        du_dx = (du8 << 16) // z * (-1 if mirror else 1)
        dv_dx = 0x100000 // z
        m = rng.randrange(256)
        row = rng.randrange(8)
        f = c_flat_record(a_tex, a_tab, -(m << 23), -(row << 24), du_dx, dv_dx,
                          u_dx=rng.randrange(-64, 64), v_dx=rng.randrange(-8, 8))
        a_flat = b.put(f, 4)
        return {"esi": a_flat, "ecx": z_index << 13, "ebx": x, "ebp": n, "edi": a_pix - 1,
                "eax": rng.randrange(1 << 32), "edx": rng.randrange(1 << 32)}, []
    return setup


def c_span_cases():
    """xn_span_flat_translucent (157E20, and its tail 157FA0): counts 1..40 (every tail
    length, 0..5 blocks), mirrored and plain u steps, two depths, several columns"""
    import random
    out = []
    rng = random.Random(0x157E20)
    k = 0
    for n in list(range(1, 41)) + [7, 8, 9, 15, 16, 17]:
        for du8, mirror in ((0x800, False), (0x600, True)):
            if k % 3 == 2 and n > 20:
                k += 1
                continue
            z_index = 0x400 if k % 2 == 0 else 0x700
            x = 20 + (k * 37) % 250
            seed = rng.randrange(1 << 30)
            out.append(("flat_translucent_n%d_%s_z%X_x%d" % (n, "m" if mirror else "p", z_index, x),
                        0x157E20, SAFE,
                        c_flat_translucent((n, du8, mirror, z_index, x, seed)),
                        [0x157E20, 0x157FA0]))
            k += 1
    return out


def c_poke(*pairs):
    """a setup that writes dwords (preferred address, value) and passes the given registers"""
    def setup(emu, b, regs=None, stack=()):
        for va, v in pairs:
            emu.write(L + va, u32(v))
        return {}, []
    return setup


def c_frame_cases():
    """The frame's clamps and full pools (object-2 globals, config/names.csv):
      begin_frame   xn_light_ambient below 0 and beyond the last row (both clamped), the
                    screen not 320 wide (a VESA mode's span-routine choice)
      frame         xn_render_frame with the texture cache full: it returns 1 at once
      light_add     a radius beyond 512 (clamped)
      light_to_view more than 32 lights counted (clamped)
      tmap_compile  the texture mapper's pool full (768 copies): the cache is marked full"""
    width, ambient, cache_full = 0x142930, 0x136911, 0x132F58
    light_count, tmap_count = 0x13690D, 0x15C158

    def light_add(emu, b):
        x, y, z = (rd32(emu, a) for a in (EG["cam_x"], EG["cam_y"], EG["cam_z"]))
        return {"eax": x + 0x800, "edx": y, "ebx": z + 0x800, "ecx": 16}, [600, 0]

    def full_cache(emu, b):
        emu.write(L + cache_full, b"\x01")
        return {"eax": 2}, []

    def tmap_full(emu, b):
        emu.write(L + tmap_count, u32(0x2FF))
        return {"eax": b.put(bytes(64)), "edx": 0}, []
    return [
        ("begin_frame_ambient_negative", 0x12A4F0, OUTDOOR, c_poke((ambient, -0x200))),
        ("begin_frame_ambient_beyond", 0x12A4F0, OUTDOOR, c_poke((ambient, 0x4100))),
        ("begin_frame_wide", 0x12A4F0, OUTDOOR, c_poke((width, 640))),
        ("frame_cache_full", 0x12A870, OUTDOOR, full_cache),
        ("light_add_radius_600", 0x136AD8, OUTDOOR, light_add),
        ("light_to_view_40", 0x136BD8, OUTDOOR, c_poke((light_count, 40))),
        ("tmap_compile_pool_full", 0x15C274, OUTDOOR, tmap_full),
    ]


C_JOBS = {
    "group_c_span": c_span_cases,
    "group_c_frame": c_frame_cases,
}


# ==== group D: the 3D objects ==============================================================
# addresses (preferred)
TEX_FULL = 0x132F58
TEX_ARCHIVES = 0x132F6C
TEX_HEAP_FREE = 0x1343E0
TEX_HEAP_HEAD = 0x1343E4
TMAP_POOL = 0x15C150
CFG_LAST_PATH = 0x191884
LIGHT_COUNT = 0x13690D
MODEL_QUEUE = 0x13F784
MODEL_QUEUE_COUNT = 0x13F76C
PICK_VIEW_X = 0x120288
PICK_VIEW_Y = 0x12028C
PICK_DISTANCE = 0x120290
CAM_XYZ = 0x0C23C4

D_F = {
    "set_translucent": 0x0CDC99, "lookup": 0x135D00, "lookup_image": 0x135DE4,
    "load_archive": 0x135EAB, "first_fit": 0x1361B8, "heap_alloc": 0x1360EE,
    "compose_angles": 0x0C7F14, "prepare": 0x13FE15, "set_frame": 0x140369,
    "set_frame_regs": 0x14037A, "model_draw": 0x140284, "light_setup": 0x155610,
    "dist_x": 0x15D03C, "dist_y": 0x15D05C, "heap_free": 0x13622F, "centroid_to_pick": 0x0C80CC,
    "xz_extent": 0x0CE828,
}


def r32(emu, va):
    return struct.unpack("<I", emu.read(L + va, 4))[0]


def w32(emu, va, v):
    emu.write(L + va, u32(v))


def w8(emu, va, v):
    emu.write(L + va, bytes([v & 0xFF]))


def lin_r32(emu, a):
    return struct.unpack("<I", emu.read(a, 4))[0]


# ---- the texture cache ---------------------------------------------------------------------

def unloaded_archive(emu):
    """an archive number the cache does not hold"""
    for a in range(511, 0, -1):
        if r32(emu, TEX_ARCHIVES + 4 * a) == 0:
            return a
    raise RuntimeError("every archive is loaded")


def fake_archive(emu, b, image=None):
    """a one-record archive (a heap block's header in front of it) holding `image`"""
    hdr = b.put(bytes(0x16) + bytes(0x40), 4)
    a = hdr + 0x16
    rec = bytearray(0x1A + 20)
    struct.pack_into("<H", rec, 0, 1)
    struct.pack_into("<HI", rec, 0x1A, 0x0101, image or 0)
    emu.write(a, bytes(rec))
    return a


def animated_image(emu, b):
    """an animated (2 frames), compiled image: 2 x 2 RLE frames, its mapper in the mapper
    pool's last slot (767: never handed out)"""
    frames = []
    for k in range(2):
        frames.append(struct.pack("<HH", 2, 2) + bytes([0, 2, 0x11 + k, 0x22 + k, 0, 2, 0x33 + k,
                                                        0x44 + k]))
    off0 = 8
    off1 = off0 + len(frames[0])
    hdr = bytearray(0x1C)
    struct.pack_into("<I", hdr, 0, 0x01FF01FF)      # wrap masks
    struct.pack_into("<HHH", hdr, 4, 2, 2, 0)       # width, height, flags (compiled)
    copy = lin_r32(emu, L + TMAP_POOL) + 767 * 418
    struct.pack_into("<I", hdr, 0x0A, copy)         # the mapper
    struct.pack_into("<I", hdr, 0x0E, 0x1C + 8)     # pixels (overwritten by the decode)
    struct.pack_into("<HHHhh", hdr, 0x12, 0, 2, 1, 0, 0)    # frames 2, 1 tick each
    tab = struct.pack("<II", off0, off1)
    return b.put(bytes(hdr) + tab + frames[0] + frames[1], 4)


def case_lookup_full(emu, b):
    w8(emu, TEX_FULL, 1)
    return {"eax": 2, "edx": 0, "ebx": 0xFFFFFFFF}, []


def case_lookup_image_full(emu, b):
    w8(emu, TEX_FULL, 1)
    return {"eax": 2, "edx": 0, "ebx": 0}, []


def lookup_rebase(frame):
    def setup(emu, b):
        img = animated_image(emu, b)
        a = fake_archive(emu, b, img)
        n = unloaded_archive(emu)
        w32(emu, TEX_ARCHIVES + 4 * n, a)
        w8(emu, TEX_FULL, 0)
        return {"eax": n, "edx": 0, "ebx": frame}, []
    return setup


def case_translucent(emu, b):
    img = animated_image(emu, b)
    a = fake_archive(emu, b, img)
    n = unloaded_archive(emu)
    w32(emu, TEX_ARCHIVES + 4 * n, a)
    return {"eax": n}, []


def path_backslash(emu, b):
    """the configured path without the backslash at its end (the load adds one)"""
    p = bytearray(emu.read(L + CFG_LAST_PATH, 80))
    s = p[:p.index(0)]
    while s.endswith(b"\\"):
        s = s[:-1]
    emu.write(L + CFG_LAST_PATH, bytes(s) + b"\0")


def shipped_archives():
    """the TEXTURE.nnn the game ships (build/game/ARENA2)"""
    return sorted(int(n[8:]) for n in os.listdir(os.path.join(ROOT, "build", "game", "ARENA2"))
                  if n.upper().startswith("TEXTURE.") and n[8:].isdigit())


def first_free_archive_file(emu):
    """a TEXTURE.nnn the game ships that the cache does not hold"""
    for n in shipped_archives():
        if r32(emu, TEX_ARCHIVES + 4 * n) == 0:
            return n
    raise RuntimeError("no free archive")


def case_load_backslash(emu, b):
    path_backslash(emu, b)
    w8(emu, TEX_FULL, 0)
    return {"eax": first_free_archive_file(emu)}, []


def case_load_no_room(emu, b):
    """every block used and stamped in the future (none older than now): nothing to evict, no
    fit"""
    blk = lin_r32(emu, L + TEX_HEAP_HEAD)
    k = 0
    while blk and k < 10000:
        flags = struct.unpack("<H", emu.read(blk + 0x0C, 2))[0]
        emu.write(blk + 0x0C, struct.pack("<H", flags | 1))
        emu.write(blk + 0x12, u32(0xFFFFFFFF))
        blk = lin_r32(emu, blk)
        k += 1
    w8(emu, TEX_FULL, 0)
    return {"eax": first_free_archive_file(emu)}, []


def heap(emu, b, blocks):
    """a crafted heap after xn_tex_heap_head: blocks of (size, used); their addresses"""
    addrs = []
    for size, used in blocks:
        addrs.append(b.put(bytes(0x16 + size + 0x20), 4))
    prev = L + TEX_HEAP_HEAD
    for k, (size, used) in enumerate(blocks):
        a = addrs[k]
        nxt = addrs[k + 1] if k + 1 < len(addrs) else 0
        emu.write(a, u32(nxt) + u32(prev) + u32(size) + struct.pack("<H", 1 if used else 0) +
                  u32(L + TEX_ARCHIVES + 8) + u32(0))
        prev = a
    w32(emu, TEX_HEAP_HEAD, addrs[0])
    w32(emu, TEX_HEAP_FREE, sum(s for s, u in blocks if not u))
    return addrs


def case_fit_split_next(emu, b):
    heap(emu, b, [(0x400, False), (0x100, True)])
    return {"eax": 0x100}, []


def case_fit_exact(emu, b):
    heap(emu, b, [(0x80, True), (0x100, False)])
    return {"eax": 0x100}, []


def case_fit_none(emu, b):
    heap(emu, b, [(0x80, False), (0x40, True)])
    return {"eax": 0x100}, []


def case_free_merge_next(emu, b):
    """the freed block's next one is free and has a next: the two merge"""
    addrs = heap(emu, b, [(0x80, True), (0x100, False), (0x40, True)])
    return {"eax": addrs[0] + 0x16}, []


def case_free_merge_both(emu, b):
    """free on both sides: the block takes its next, then joins its previous"""
    addrs = heap(emu, b, [(0x80, False), (0x100, True), (0x40, False), (0x20, True)])
    return {"eax": addrs[1] + 0x16}, []


def case_xz_extent(emu, b):
    """points beyond the swapped starting extents (Q-MODEL-01): every comparison taken"""
    pts = vec(-200000, 0, -200000) + vec(200000, 0, 200000) + vec(0, 0, 0)
    m = bytearray(0x40 + len(pts))
    struct.pack_into("<4sii", m, 0, b"v2.7", 3, 0)
    struct.pack_into("<i", m, 0x30, 0x40)
    m[0x40:] = pts
    return {"eax": b.put(bytes(m), 4), "edx": b.put(bytes(4)), "ebx": b.put(bytes(4))}, []


def case_centroid(emu, b):
    m = arch3d(b)
    h = b.put(u32(m) + bytes(0x3C), 4)
    return {"eax": h}, []


# ---- models --------------------------------------------------------------------------------

def arch3d(b, version=b"v2.7", frames=0, degenerate=False, tex=0x0102):
    """A model: 4 points of a square (side 2 * 4000h, z = 0), one textured face of 4 points (a
    second face of 3 points too: degenerate, two of them equal, when asked), normals, face data
    and, with frames, a frame table (each frame the same lists). Vertex offsets are index * 12
    ("v2.5": index * 4). Returns the model's address."""
    pts = vec(-0x4000, -0x4000, 0) + vec(0x4000, -0x4000, 0) + vec(0x4000, 0x4000, 0) + \
        vec(-0x4000, 0x4000, 0)
    unit = 4 if version < b"v2.6" else 12
    faces = bytes([4, 0]) + struct.pack("<H", tex) + u32(0)
    uv = [(0, 0), (0x40, 0), (0, 0x40), (0, 0)]
    for k in range(4):
        faces += u32(k * unit) + struct.pack("<hh", *uv[k])
    nfaces = 1
    if degenerate:
        for pts3 in ((0, 0, 1), (0, 1, 2)):          # p0 = p1; then (below) e2 along e1
            faces += bytes([3, 0]) + struct.pack("<H", tex) + u32(0)
            for i, k in enumerate(pts3):
                faces += u32(k * unit) + struct.pack("<hh", 0x10 * i, 0x10 * i)
            nfaces += 1
    hdr_size = 0x40
    point_off = hdr_size
    normal_off = point_off + len(pts)
    face_off = normal_off + 12 * nfaces
    data_off = face_off + len(faces)
    frame_off = data_off + 24 * nfaces
    total = frame_off + 16 * frames
    m = bytearray(total)
    struct.pack_into("<4siiiiii", m, 0, version, 4, nfaces, 0x5A82, frames, frame_off, data_off)
    struct.pack_into("<ii", m, 0x30, point_off, normal_off)
    struct.pack_into("<i", m, 0x3C, face_off)
    m[point_off:point_off + len(pts)] = pts
    m[face_off:face_off + len(faces)] = faces
    for k in range(frames):
        struct.pack_into("<iiii", m, frame_off + 16 * k, point_off, normal_off, data_off, 0)
    return b.put(bytes(m), 4)


def arch3d_degenerate_e2(b):
    """a model whose second face has e2 along e1 (p0, p1, p2 on a line)"""
    pts = vec(0, 0, 0) + vec(0x1000, 0, 0) + vec(0x2000, 0, 0) + vec(0, 0x1000, 0)
    face = bytes([3, 0]) + struct.pack("<H", 0x0102) + u32(0)
    for i, k in enumerate((0, 1, 2)):
        face += u32(k * 12) + struct.pack("<hh", 0x10 * i, 0)
    m = bytearray(0x40 + len(pts) + 12 + len(face) + 24)
    struct.pack_into("<4siiiiii", m, 0, b"v2.7", 4, 1, 0x2000, 0, 0, 0x40 + len(pts) + 12 +
                     len(face))
    struct.pack_into("<ii", m, 0x30, 0x40, 0x40 + len(pts))
    struct.pack_into("<i", m, 0x3C, 0x40 + len(pts) + 12)
    m[0x40:0x40 + len(pts)] = pts
    m[0x40 + len(pts) + 12:0x40 + len(pts) + 12 + len(face)] = face
    return b.put(bytes(m), 4)


def case_prepare_v25(emu, b):
    return {"eax": arch3d(b, version=b"v2.5")}, []


def case_prepare_frames(emu, b):
    return {"eax": arch3d(b, frames=2)}, []


def case_prepare_degenerate(emu, b):
    return {"eax": arch3d(b, degenerate=True)}, []


def case_prepare_line(emu, b):
    return {"eax": arch3d_degenerate_e2(b)}, []


def case_set_frame(frame):
    def setup(emu, b):
        return {"eax": arch3d(b, frames=2), "edx": frame}, []
    return setup


def case_set_frame_regs(frame):
    def setup(emu, b):
        m = arch3d(b, frames=3)
        return {"eax": frame, "ebx": 3, "esi": m}, []
    return setup


def case_compose_fail(emu, b):
    h = b.put(bytes(0x40), 4)
    return {"eax": h + 0x0C, "edx": 512, "ebx": 0, "ecx": 0}, []


def queued_handle(emu):
    n = r32(emu, MODEL_QUEUE_COUNT)
    if n == 0:
        raise RuntimeError("no model queued")
    best = None
    for k in range(min(n, 199)):
        key, h = struct.unpack("<iI", emu.read(L + MODEL_QUEUE + 8 * k, 8))
        if best is None or key < best[0]:
            best = (key, h)
    return best[1]


def case_draw_cache_full(emu, b):
    w8(emu, TEX_FULL, 1)
    return {"edi": queued_handle(emu)}, []


def case_draw_frames(emu, b):
    """a crafted handle of the framed model 40 units in front of the eye (on the view axis: the
    camera's rotation's row 2)"""
    m = arch3d(b, frames=2, tex=0x0000)
    # the model prepared first by the asm itself would be a second call: the face plane and
    # normal are set here as prepare leaves them (normal +z with 8 fraction bits)
    mm = bytearray(emu.read(m, 0x40))
    normal_off = struct.unpack_from("<i", mm, 0x34)[0]
    emu.write(m + normal_off, vec(0, 0, 0))               # a back face: n . eye + d = 1
    face_off = struct.unpack_from("<i", mm, 0x3C)[0]
    data_off = struct.unpack_from("<i", mm, 0x18)[0]
    emu.write(m + face_off + 0x14, u32(1))                  # plane d
    emu.write(m + face_off + 0x1C, u32(m + data_off))       # face data
    cx, cy, cz = struct.unpack("<iii", emu.read(L + CAM_XYZ, 12))
    rot = struct.unpack("<9i", emu.read(L + 0x136E00, 36))
    fx, fy, fz = rot[6], rot[7], rot[8]
    d = 400
    px = cx + ((fx >> 14) * d >> 14)
    py = cy + ((fy >> 14) * d >> 14)
    pz = cz + ((fz >> 14) * d >> 14)
    h = bytearray(0x40)
    struct.pack_into("<I", h, 0, m)
    struct.pack_into("<iii", h, 0x14, (px - cx) << 8, (py - cy) << 8, (pz - cz) << 8)
    struct.pack_into("<iii", h, 0x20, px, py, pz)
    struct.pack_into("<B", h, 0x38, 1)
    w8(emu, TEX_FULL, 0)
    return {"edi": b.put(bytes(h), 4)}, []


# ---- flats ---------------------------------------------------------------------------------

def d_flat(b, table=0, light=0, z=0x10000):
    f = bytearray(0x64)
    struct.pack_into("<I", f, 0x44, 0x100 | light << 16)
    struct.pack_into("<I", f, 0x4C, table)
    struct.pack_into("<iii", f, 0x50, 0, 0, z)
    return b.put(bytes(f), 4)


def case_flat_translucent(emu, b):
    return {"esi": d_flat(b, table=0x12345678)}, []


def case_flat_many_lights(emu, b):
    w32(emu, LIGHT_COUNT, 40)
    return {"esi": d_flat(b)}, []


# ---- the camera ----------------------------------------------------------------------------

def case_dist(view_va):
    def setup(emu, b):
        w32(emu, view_va, 0x1000)
        w32(emu, PICK_DISTANCE, 0x1000)
        return {"eax": 0x10000, "ebx": 0x10000, "edi": 0x100}, []
    return setup


def d_cases(cases):
    return lambda: [(label, va, OUTDOOR, setup) for label, va, setup in cases]


D_JOBS = {
    "group_d_tex": d_cases([
        ("lookup_full", D_F["lookup"], case_lookup_full),
        ("lookup_image_full", D_F["lookup_image"], case_lookup_image_full),
        ("lookup_rebase_0", D_F["lookup"], lookup_rebase(0)),
        ("lookup_rebase_clock", D_F["lookup"], lookup_rebase(0xFFFFFFFF)),
        ("translucent_crafted", D_F["set_translucent"], case_translucent),
        ("load_no_backslash", D_F["load_archive"], case_load_backslash),
        ("load_no_room", D_F["load_archive"], case_load_no_room),
        ("fit_split_next", D_F["first_fit"], case_fit_split_next),
        ("fit_exact", D_F["first_fit"], case_fit_exact),
        ("fit_none", D_F["first_fit"], case_fit_none),
        ("free_merge_next", D_F["heap_free"], case_free_merge_next),
        ("free_merge_both", D_F["heap_free"], case_free_merge_both),
    ]),
    "group_d_model": d_cases([
        ("prepare_v25", D_F["prepare"], case_prepare_v25),
        ("prepare_frames", D_F["prepare"], case_prepare_frames),
        ("prepare_degenerate", D_F["prepare"], case_prepare_degenerate),
        ("prepare_line", D_F["prepare"], case_prepare_line),
        ("set_frame_high", D_F["set_frame"], case_set_frame(5)),
        ("set_frame_low", D_F["set_frame"], case_set_frame(0xFFFFFFFF)),
        ("set_frame_regs_mid", D_F["set_frame_regs"], case_set_frame_regs(1)),
        ("set_frame_regs_high", D_F["set_frame_regs"], case_set_frame_regs(9)),
        ("compose_no_angles", D_F["compose_angles"], case_compose_fail),
        ("draw_cache_full", D_F["model_draw"], case_draw_cache_full),
        ("draw_frames", D_F["model_draw"], case_draw_frames),
        ("centroid_to_pick", D_F["centroid_to_pick"], case_centroid),
        ("xz_extent_wide", D_F["xz_extent"], case_xz_extent),
    ]),
    "group_d_flat": d_cases([
        ("light_translucent", D_F["light_setup"], case_flat_translucent),
        ("light_many", D_F["light_setup"], case_flat_many_lights),
    ]),
    "group_d_cam": d_cases([
        ("dist_x_inner", D_F["dist_x"], case_dist(PICK_VIEW_X)),
        ("dist_y_inner", D_F["dist_y"], case_dist(PICK_VIEW_Y)),
    ]),
}


def run_jobs(jobs, names, out, kind, tool):
    """Groups C, D and E: each job a list of cases (label, va, base, setup[, only]), each case
    from a fresh copy of its base: setup(emu, buf) -> (registers, stack arguments), then the
    call, recording the case's function (or the functions `only` lists)."""
    for name in names:
        cases = jobs[name]()
        t0 = time.time()
        records, store, notes = [], {}, []
        attempts, dropped = collections.Counter(), collections.Counter()
        base_used = None
        for case in cases:
            label, va, base, setup = case[:4]
            only = case[4] if len(case) > 4 else [va]
            base_used = base_used or base
            emu, ov = load(base, name)
            try:
                rec = R.Recorder(emu, per=1, only=only, base=base, name=name,
                                 state="%s:%s" % (name, label))
                b = Buf(emu)
                try:
                    regs, stack = setup(emu, b)
                except Exception as e:          # noqa: BLE001
                    notes.append("%s: setup %r" % (label, e))
                    print("  %-32s setup failed: %r" % (label, e), flush=True)
                    continue
                ok = run_call(emu, rec, va, regs, stack)
                got = len(rec.records)
                for r in rec.records:
                    r["probe"] = label
                records += rec.records
                store.update(rec.store)
                attempts.update(rec.count)
                dropped.update(rec.failed)
                print("  %-32s %06X %s, %d record%s" % (
                    label, va, "returned" if ok else "DID NOT RETURN", got,
                    "" if got == 1 else "s"), flush=True)
                if not got:
                    notes.append("%s: no record" % label)
            finally:
                done(emu, ov)
        spec = {"name": name, "kind": kind, "tool": tool, "cases": [c[0] for c in cases]}
        R.write_job(out, name, spec, os.path.join(ROOT, base_used), records, store, set(),
                    attempts, dropped, set(), notes, t0, 0, None, 0, 0, 1)
        print("%s: %d records in %.0f s" % (name, len(records), time.time() - t0), flush=True)


def run_c(names, out):
    run_jobs(C_JOBS, names, out, "group_c", "tools/xn_mkrec.py c")


def run_d(names, out):
    run_jobs(D_JOBS, names, out, "group_d", "tools/xn_mkrec.py d")


# ---- the groups -------------------------------------------------------------------------------
GROUPS = {
    "a": (lambda: ["grpa_" + n for n in A_CASES], run_a, "grpa_"),
    "b": (lambda: ["b_" + n for n in sorted(B_CASES)], run_b, "b_"),
    "c": (lambda: list(C_JOBS), run_c, ""),
    "d": (lambda: list(D_JOBS), run_d, ""),
    "e": (lambda: list(E_JOBS), run_e, ""),
}


def selected(group, want):
    """the group's jobs (full names) that match want (names or prefixes, with or without the
    group's prefix); all of them for none"""
    jobs, _run, pre = GROUPS[group]
    out = []
    for j in jobs():
        short = j[len(pre):]
        if not want or any(j.startswith(w) or short.startswith(w) for w in want):
            out.append(j)
    return out


def generate(group, want, out):
    jobs = selected(group, want)
    _jobs, run, pre = GROUPS[group]
    t0 = time.time()
    run([j[len(pre):] for j in jobs], out)
    print("group %s: %d jobs -> %s in %.0f s" % (group, len(jobs), os.path.relpath(out, ROOT),
                                                time.time() - t0))


def _fields(r, store):
    """what makes a record: its function, entry state (registers and input pages), and what
    the call did (exit registers, writes, I/O)"""
    pages = {a: store[k] for a, k in r["page_refs"].items()} if store is not None else r["pages"]
    return (r["func"], r["eip"], r["entry"], pages, r["exit"], r["writes"], r["io"],
            r["returned"])


def check(group, want, out, against):
    """Replay the jobs' records on the asm (xn_record.replay: exact); with against, compare
    each job's records with that folder's copy of the job."""
    jobs = selected(group, want)
    ok = tot = 0
    funcs = collections.Counter()
    same = differ = 0
    missing = []
    for j in jobs:
        p = os.path.join(out, j + ".pkl")
        if not os.path.exists(p):
            missing.append(j)
            continue
        recs = R.read_records(p)
        bad = []
        for r in recs:
            funcs[r["func"]] += 1
            try:
                d = R.replay(r)
            except Exception as e:      # noqa: BLE001
                d = ["error: %r" % e]
            if d:
                bad.append("%X: %s" % (r["func"], "; ".join(d[:2])))
        ok += len(recs) - len(bad)
        tot += len(recs)
        line = "%-34s %3d / %3d replay exactly" % (j, len(recs) - len(bad), len(recs))
        for d in against or ():
            q = os.path.join(d, j + ".pkl")
            if not os.path.exists(q):
                continue
            theirs, ts = R.read_raw(q)
            mine, ms = R.read_raw(p)
            fm = collections.Counter(r["func"] for r in mine)
            ft = collections.Counter(r["func"] for r in theirs)
            eq = len(mine) == len(theirs) and all(_fields(x, ms) == _fields(y, ts)
                                                  for x, y in zip(mine, theirs))
            same += eq
            differ += not eq
            line += "; %s %s" % (os.path.relpath(d, ROOT), "identical" if eq else
                                 "functions and counts %s, records differ" % (
                                     "agree" if fm == ft else "DIFFER"))
            break
        print(line + ("; " + " | ".join(bad[:3]) if bad else ""), flush=True)
    for emu, _m in R._bases.values():      # the replay machines
        emu.close()
    R._bases.clear()
    print("group %s: %d jobs, %d records of %d functions, %d / %d replay exactly%s%s" % (
        group, len(jobs) - len(missing), tot, len(funcs), ok, tot,
        "; against: %d jobs identical, %d differ" % (same, differ) if against else "",
        "; missing: " + " ".join(missing) if missing else ""))
    return ok == tot and not missing and not differ


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("what", choices=("a", "b", "c", "d", "e", "all", "check"))
    ap.add_argument("names", nargs="*", help="cases or jobs (prefixes); for check: the groups")
    ap.add_argument("--out", default=OUT, help="the records' folder (the corpus)")
    ap.add_argument("--against", action="append", help="check: another copy of the jobs")
    ap.add_argument("--list", action="store_true")
    a = ap.parse_args()
    if a.what == "check":
        groups = [g for g in a.names if g in GROUPS or g == "all"] or ["all"]
        want = [n for n in a.names if n not in GROUPS and n != "all"]
        groups = list(GROUPS) if "all" in groups else groups
        res = [check(g, want, a.out, a.against) for g in groups]
        return 0 if all(res) else 1
    groups = list(GROUPS) if a.what == "all" else [a.what]
    for g in groups:
        if a.list:
            for j in selected(g, a.names):
                print(j)
            continue
        generate(g, a.names, a.out)
    return 0


if __name__ == "__main__":
    sys.exit(main())
