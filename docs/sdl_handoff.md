# Handoff: an SDL port of the canonical XnGine C

**Start from commit `3c12880`** (or later) on `main` of github.com/brotchie/daggerfall-decomp. Work on
a branch or a separate worktree: the decompilation continues on `main`.

## What is ready

- **The engine as canonical C**: `src/engine/`, 648 functions.
  - Plain prototypes in documented headers (`x<module>.h`); structs in `xnstruct.h`.
  - No asm interfaces, no self-modifying or generated code.
  - Overview: docs/engine/architecture.md. The guide and the proof: docs/xngine_canonical.md.
  - The original bugs are kept and catalogued in docs/engine/quirks.md. A port should keep them
    unless it decides otherwise on purpose.
- **The engine's data as C**: `src/engine_data/` (docs/engine/data.md).
  - At startup, call `xn_data_object2` (object 2 from the user's FALL.EXE 1.07.213), then
    `xn_data_load`, then `xn_data_compute`.
  - No game bytes are in the repository: the initial data comes from the user's file.
  - Proven equal to FALL.EXE's image, and the engine links with no FALL.EXE address.
- **The game's own C**: `src/lifted/`, `src/hand/` and `src/*.c`. It is byte-matching Watcom C32 10.0a,
  compiled to exactly FALL.EXE's object 1 (`tools/build-and-verify.sh`). It is DOS code: MemCheck
  wrappers, the Watcom runtime, int/pointer casts everywhere, and `#pragma aux` in places.

## What the port has to provide

docs/engine/data.md, "What a port must provide", has the full list. In short:
1. **The platform layer.** The engine reaches the machine only through:
   - `dos.c`/`xdos.h` (files, the 'DOS:' messages, exit);
   - `pc.c`/`xpc.h` (DPMI, the BIOS keyboard, the mouse driver, the vectors, the BIOS data area);
   - the port helpers `xn_inb`/`xn_outb` (PIC, PIT, keyboard, joystick, palette DAC, retrace);
   - `glue.asm`'s `xn_int10`...`xn_int33`;
   - `gfx.c`'s VGA/VESA. Its int 10h calls go direct rather than through `pc.c`: tidy that first.

   Replace these with SDL:
   - the screen is `screen_buffer`, 320x200 with 8-bit indices, plus the palette from `pal.c`;
   - input comes from the keyboard state (`key_down[]`) and the mouse;
   - time comes from the BIOS tick count, at 0x46C in the original.
2. **The interrupt handlers** become event-driven calls:
   - `xn_kbd_int9_handler`, run per key event;
   - `xn_joy_timer_isr`, run per timer tick;
   - the critical-error and divide-error handlers, which can go: canonical C never faults on a
     divide (Q-SYS-01).
3. **The game's symbols.** The engine calls 86 game functions and reads 14 game data symbols. The
   game names 39 places inside engine symbols (`key_down[k]`, `xn_joy` fields, `pick_distance`...).
4. **Runtime pieces:** an allocator, `exit`, `filelength`, and the sound library's timer and sample
   calls (the VID player). The original uses HMI SOS.
5. **A modern compiler:**
   - `xngine.h`'s `#pragma aux` inline helpers (64-bit multiply and divide, `bsr`/`bsf`, port I/O)
     become `int64_t` arithmetic and intrinsics;
   - several structs keep pointers in 32-bit fields, mirroring the original memory layout: build
     32-bit (`-m32`) first, or widen them deliberately;
   - Watcom 10.0a is the reference compiler (`tools/wcc10.py`).

## How to check it still matches the original

The harness compares the C engine against the original asm inside the headless game (`tools/fallemu.py`,
patched Unicorn). Before changing semantics for the port, keep these green:
- `tools/xn_rc.py build`, then `tools/xn_rc.py test --corpus -j 4`: 8,880 recorded calls, each through its
  shim.
- `tools/xn_rc.py test --boundary`: the game's calls.
- `tools/xn_rc.py scenarios`: 39 lockstep scenarios, 1,675 frames. The screen and game memory must
  stay identical.
- `tools/xn_equiv.py`: 140 pure-helper specs.

They need the user's game files (`orig/`), Watcom 10.0a (`third_party/watcom10`) and the patched Unicorn
(`tools/build_unicorn.sh`). See README.md.

Once the port runs natively, the comparison moves to screenshots or traces of the same input against
the original running in `tools/fallemu.py`. The scenarios' scripts (`tools/xn_scenarios.py`) are a
ready set of inputs.
