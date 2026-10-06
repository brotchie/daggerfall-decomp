# The native port: FALL.EXE's C on arm64 macOS with SDL3

The goal is the game built from `src/` by the host's clang as a native program, with SDL3 in
place of DOS, VGA, the keyboard and timer interrupts and HMI SOS. arm64 macOS comes first.
Work happens on the `port` branch, in the worktree `~/dagger_comp-port`, alongside the XnGine
work on `main`.

## Rules

- **One source, two builds.** The matching build (`tools/build-and-verify.sh`) stays
  byte-identical. It checks every change the port makes to `src/` and `include/`. When the
  native build needs a different type than Watcom's bytes allow, the difference goes into a
  header (`#ifdef DAGGER_PORT`), not into a copy of the source.
- **The engine belongs to the XnGine work.** `src/engine/`, `tools/xn_*.py`,
  `config/xngine_*`, `docs/xngine*.md` and `docs/engine/` change only on `main`. For now the
  native build stubs the engine; it links `src/engine/` once the canonical C has landed.
- **Nothing in `build/port/gen/` is committed.** `tools/port_build.py` regenerates it on every
  build.

## Layout

| Path | What |
|---|---|
| `port/CMakeLists.txt` | the native build: the game's C as an object library, the shims, the generated definitions, SDL3 |
| `port/include/port.h` | included ahead of every game file: Watcom's far-pointer keywords become nothing, and the C library calls whose DOS behaviour differs go to `port/shim` (function-like macros, so a parameter named `close` stays) |
| `port/include/dos.h`, `i86.h` | Watcom's `REGS`, `SREGS` and the packed 43-byte `find_t`. Far pointers: a selector stands for a pointer's top 32 bits, so `MK_FP(FP_SEG(p), FP_OFF(p)) == p` for 64-bit pointers; selector 0 and the flat selector `segread` gives are the program's own 4 GB window |
| `port/shim/libc.c`, `watcom.c` | Watcom's `rand` (so a native run can follow an emulated one), `itoa`, `utoa`, `stricmp`, `strnicmp`, `exit`, `pow` (Watcom's square-and-multiply for integral exponents, which libm differs from in the last bits), `fscanf`/`sscanf`, `_msize`, `_nheapwalk` |
| `port/shim/memcheck.c`, `mcheck2.c` | MemCheck's `mc_*` replacements, passed to the C library. MemCheck is never started in FALL.EXE, so its checks return 0 and its debug output goes nowhere, natively as in the original |
| `port/shim/sos.c` | HMI SOS (timer, digital, MIDI, songs) as the game calls it. The timer events really fire, on SDL's timer thread: the 140 Hz event is the game's frame clock. There is no audio yet; samples "end" after their length's time |
| `port/shim/except.c`, `xgfx.c` | MemCheck's exception hooks (no-ops); two pixel routines in the library region (`gfx_put_pixel`, `gfx_get_pixel`) |
| `port/shim/dosfile.c` | DOS files: `C:\` and relative paths are the install folder, names match without regard to case, writes go to an overlay folder (read first), Watcom's `open` flags; `_dos_findfirst`/`_dos_findnext` as DOS matches (`*.*` matches every name, 8.3 upper case, directories only with `_A_SUBDIR`) |
| `port/shim/dos.c` | `int386`/`int386x`: DPMI 0500h (free memory, the emulator's figures), 0600h-0603h, 0100h/0101h/0006h (DOS memory as host blocks), CauseWay FF30h; anything else logs and sets carry |
| `port/include/port_vpc.h`, `port/host/vpc*.c` | the virtual PC on SDL3 (phase 5, below): low memory, VGA mode 13h and its DAC, the 70 Hz retrace, the keyboard (scan codes, int 9, the BIOS buffer), the mouse driver (int 33h), the PIT and BIOS tick, the vector table, DOS/BIOS/DPMI services with a register file, the sound card |
| `port/host/opl3.c` | a YMF262 (OPL3) from the chip's documented behaviour: log-sine and exponent tables, the 9-bit envelope generator and its rates, the eight waveforms, feedback, two- and four-operator channels, the rhythm section and its noise, tremolo, vibrato, OPL3 stereo; 49716 Hz |
| `port/host/hmi_opl.c` | HMI SOS's OPL MIDI drivers (fmmidi3.com 0xA009, fmmidi.com 0xA002), run from the install's HMIMDRV.386 as the game ran them: the driver's image is its memory, so its tables, state and quirks are the original's |
| `port/host/hmi_seq.c` | HMI SOS's song player as FALL.EXE links it: "HMI-MIDISONG061595" songs at 120 ticks a second, its loops and branches, track groups on hardware channels, CC7 by the master volume |
| `port/host/vpc_music.c` | the OPL3 in its own SDL3 stream at 49716 Hz; the player ticks inside the stream, so notes land on their sample |
| `port/test/opltest.c`, `musictest.c` | the OPL3 against the datasheet's numbers; a song rendered offline through the whole music path to a WAV (`--all`: every song) |
| `port/test/vpcdemo.c` | the virtual PC on its own, through XnGine's entry points, with the game's image, palette and sounds; `--selftest` checks it end to end |
| `port/host/host.c` | stopping SDL and the virtual PC, stopping on a stub or fault with the call chain, `port_check_ptr` |
| `port/host/vpc_script.c` | the scripted driver: `PORT_SCRIPT` (a timeline of `shot`, `key`, `type`, `click` ... steps), `PORT_SHOT_EVERY`/`PORT_SHOT_DIR`, `PORT_EXIT_AFTER` |
| `port/host/main.c` | `main`: SDL, the folders, then the game's main (0x10010) with `Z.CFG`, as `FALL.EXE Z.CFG` ran; a backtrace for a stub or a fault; `port_check_ptr` stops on a pointer that lost its top half |
| `tools/port_build.py` | configure, build and generate; `run` prepares a game folder as `tools/fallemu.py` does and starts the build; `missing` lists the stubs |
| `tools/port_census.py` | the 64-bit worklist: clang's diagnostics on the game's C, by kind and by file |
| `tools/port_lowmem.py` | the game's reads of real-mode memory by address (68 of the BIOS tick at 0x46C, 44 of VGA memory at 0xA0000) as `DOS_LOW(addr)` (include/doslow.h): the same constant under Watcom, the virtual PC's low memory natively; lists the sites, `--apply` rewrites them |
| `tools/port_data.py` | the game's data (phase 2): object 3 in address order, globals whose types hold pointers in native layouts with 8-byte relocations from FALL.EXE's fixups; `report` lists declarations still too narrow |

## Building and running

```sh
.venv/bin/python tools/port_build.py                 # build/port/fall
DAGGER_GAME=~/dagger_comp/build/game .venv/bin/python tools/port_build.py run [--nosound]
build/port/vpcdemo --game ~/dagger_comp/build/game        # the virtual PC on its own (with music)
build/port/musictest --game ~/dagger_comp/build/game --song D1.HMI --seconds 60 --out d1.wav
SDL_VIDEO_DRIVER=offscreen SDL_AUDIO_DRIVER=dummy DAGGER_OVERLAY=build/port/run \
    build/port/vpcdemo --game ~/dagger_comp/build/game --selftest
# a run without a person: screenshots every 2 s, keys and clicks on a timeline, quit at 60 s
PORT_SHOT_EVERY=2 PORT_SHOT_DIR=/tmp/shots PORT_EXIT_AFTER=60 \
    PORT_SCRIPT="8 key Escape; 12 click 160 100; 20 shot /tmp/a.bmp" \
    DAGGER_GAME=~/dagger_comp/build/game .venv/bin/python tools/port_build.py run
.venv/bin/python tools/port_build.py missing         # what the stubs stand in for
.venv/bin/python tools/port_census.py                # the 64-bit worklist
```

The game's C is compiled as Watcom 10.0a compiled it:
- C89 with extensions (`-std=gnu89`);
- plain `char` unsigned (`-funsigned-char`);
- signed overflow wraps (`-fwrapv`);
- no strict aliasing (decompiled code reads one type through another).

`int` and `long` are 32 bits on both, but pointers are 64 bits in the native build.

How the link is completed: the game's objects name about 4,000 functions and globals. What
the port defines links as it is, and so does the short list of C library functions the game
takes from the host (`HOST_LIBC`). Everything else gets a generated definition in
`build/port/gen/`:
- a function becomes a stub that names itself and stops;
- a global becomes its bytes from FALL.EXE, from its address to the next known symbol.

A game global that shares a name with the host's C library gets its own definition, which the
linker prefers.

## Why the 64-bit work cannot be skipped

The game keeps pointers in `int`s everywhere: casts, globals typed `int`, functions declared
without parameters that return `int`. On a 64-bit host each of those loses the pointer's top
half. A 32-bit address space would hide that, but arm64 macOS has none:
- there is no ILP32 target (`arm64_32` is watchOS only);
- the kernel keeps the low 4 GB unmapped: `mmap(0x10000000, MAP_FIXED)` fails;
- a binary linked with a smaller `__PAGEZERO` (`-pagezero_size 0x4000`) is killed at exec
  (SIGKILL, tested 2026-10-05).

So every value that holds an address must have a type as wide as a pointer.

## Status

**2026-10-06, the game runs natively.** With XnGine linked (see "Linking the engine"), `fall`:
- plays the intro movie with its sound;
- loads the classic saves (SAVE0-SAVE5 of the install, from the load screen);
- lets the player walk and turn in town, with sound effects and the OPL3 music;
- shows the character sheet, the options panel and the other screens.

What it took, beyond the engine:
- **Threads.** The game runs on a thread of its own (`vpc_run`). The main thread takes SDL's
  events, shows the screen at 70 Hz and runs the scripted driver, whatever loop the game is
  in. A wait for a key with no port I/O (`options_open`'s `while (key_down_esc)`) used to
  hang.
- **The zero page.** Under the DOS extender a null pointer reads linear 0, the real-mode
  interrupt table. The game does that: enemy slots of factions, flats FLATS.CFG does not list,
  the settings before a save sets them.
  - `port/host/zeropage.c` finishes such faults against the virtual PC's low memory: it
    decodes arm64 LDR/STR/LDP/STP and resumes, and reports each place once.
  - `DOS_NULL(p)` (doslow.h) marks the known places in the source.
- **Packing.** Watcom 10.0a packs structs to 1 byte by default; a wcc386 probe puts
  `{char; int}`'s int at 1. The game and the engine are compiled with `-fpack-struct=1`, and
  port_data lays out i386 and native data the same way.
  - The link uses `-no_fixup_chains`: chained fixups cannot relocate pointers at odd offsets.
- **Data.** Every game table holding pointers is now declared so: port_data's narrow rows went
  from 494 to 0. MemCheck's and Watcom's own data (`library_data`) stays as FALL.EXE's 32-bit
  values, since only the library code the native build replaces reads it.
- **The freed-pointer sentinel** 0x97979797 is written one way, `(T *)(iptr)-1751672937`. A
  zero-extended spelling never compared equal to a sign-extended one.
- **Boundary adapters.** `xn_draw_paperdoll_mask`'s game call passes the engine's argument
  order natively. The other adapters only reproduce registers the asm left behind
  (Q-DRAW-02/03/05, Q-FONT-01, Q-MATH-02, Q-KBD-01); natively the game sites see their C
  values.
- **DOS handles.** File handles are DOS's (the lowest free from 5), each standing for a host
  descriptor, so the game's 20-handle archive tables hold.
- **SOS's sample callback.** The mixer calls it at a voice's end, under the interrupt lock,
  and plays on any data it gives (the movies stream their sound this way).
- **The installer's files.** ARCH3D.BSA and DAGGER.SND are unpacked from PACKED.DAT
  (`port/host/packed.c`, PKWARE DCL) into the overlay on the first run.
- **Checks without a person.**
  - `PORT_SCRIPT` (keys, clicks and screenshots on a timeline), `PORT_SHOT_EVERY`,
    `PORT_EXIT_AFTER`;
  - `PORT_AUDIO_RAW` (the final mix), `PORT_FPS`.

**2026-10-05, phase 1 done.**
- `build/port/fall` is a Mach-O arm64 executable built from all 427 game files.
- The build compiles with no errors. The 39 game structs that hold pointers check their size
  with `RECORD_SIZE_P`, which is exact under Watcom and only lets the struct grow in the
  native build.
- The link needs:
  - 220 function stubs: 175 engine functions, 45 unnamed library functions;
  - 2,624 globals defined from FALL.EXE's bytes.
- A run reaches the game's main. It asks DPMI for free memory (int 31h 0500h, not done yet)
  and stops at `func_0009DB3F`, MemCheck's handler registration.

**2026-10-05, the library.** All 45 unnamed library functions the game calls have native
implementations.
- Each has a name in `config/names.csv`:
  - HMI SOS: timer, digital, MIDI and songs;
  - MemCheck's API, found through its own table of names at 0x188F01;
  - Watcom's `pow`, `_msize`, `fscanf`, `_dos_findfirst`/`_dos_findnext`.
- Three of the existing names were wrong:
  - 0xA0AD9 is MemCheck's `strcpy`, not `strncpy`;
  - 0xA16F8 "fprintf" is `sscanf`;
  - 0xA31A6 "fprintf_2" is `fscanf`.
- The only stubs left are the 175 engine functions. A run gets through the DPMI and MemCheck
  setup and stops at the first engine call, `xn_sys_install_crit_error_handler`.
- The data generator is wired in. Of 2,346 globals, 391 have native layouts.
- Next in `main`:
  - `srand(*(int *)0x46c)` reads the BIOS tick at a fixed address, which the SDL layer has to
    provide;
  - `config_read` opens Z.CFG through a declaration that returns an int.

**2026-10-05, the virtual PC (phase 5, platform side).** XnGine reaches the hardware through
a few helpers, which the canonical engine keeps (docs/xngine_canonical.md on main):
- port I/O: `xn_inb`, `xn_outb`, `xn_inw`, `xn_outw`;
- services with a register file: `xn_int10`, `xn_int15`, `xn_int16`, `xn_int21`,
  `xn_int2f`, `xn_int31`, `xn_int33`;
- `_dos_getvect`/`_dos_setvect` for its five handlers;
- VGA memory and the BIOS data area by address.

`port/host/vpc*.c` gives the native build those same helpers on SDL3:
- **Video:** mode 13h at 0xA0000 of a 1 MB + 64 KB low-memory block, through the 256-colour
  DAC (3C7h-3C9h), shown at 4:3 in a resizable window. Alt+Enter toggles full screen; F12
  saves a screenshot.
- **Retrace:** 3DAh gives a 70 Hz vertical retrace. The screen is shown when the engine
  sees a retrace start, otherwise every 14 ms.
- **Interrupts:** they run on SDL's timer thread one at a time, under a lock that
  `port_cli`/`port_sti` hold off, as one CPU takes them:
  - the PIT's 18.2 Hz tick (int 8, the BIOS's: 0x46C and int 1Ch);
  - the keyboard (int 9 for each scan code byte at port 60h);
  - HMI SOS's timer events.
- **Keyboard:** SDL keys become scan code set 1. The default int 9 is the BIOS's: shift
  flags at 0x417, a keyboard buffer for int 16h.
- **Mouse:** int 33h with mickeys, ranges and press counts. A click captures the mouse;
  Ctrl+G releases it.
- **DOS and DPMI:** the int 21h file calls go to the DOS file layer; DPMI covers
  selectors, DOS memory and exception vectors. There is no VESA, so XnGine stays in
  mode 13h.
- **Sound:** SOS's samples play on a mixer of 160 voices into one SDL3 stream, with SOS's
  volume and pan. The overlay gets an HMISET.CFG with a Sound Blaster 16 (its digital
  device and its FM synthesiser at 0x388), so the game turns its sound effects and music
  on (`--nosound` keeps the install's). Music: below.
- **Data:** `screen_buffer`, whose initial value in FALL.EXE is 0xA0000, now points into the
  native low memory (tools/port_data.py).

`vpcdemo --selftest` checks it end to end, and every check passes:
- mode 13h and the DAC read-back;
- an ARENA2 image read through the DOS layer;
- scan codes 1E 9E 2A 1E 9E AA E0 48 through an installed int 9 at port 60h, and the BIOS
  buffer's a, A, Up when the handler chains;
- int 33h motion;
- frames paced at 70 Hz;
- the BIOS tick at 18.2 Hz;
- a DAGGER.SND sample that plays and finishes.

With a window the demo shows CHGN00I0.IMG with its palette and a mouse cursor, and clicks
play sounds.

`tools/port_lowmem.py` finds the game's own reads of real-mode memory by address: 68 of the
tick at 0x46C, 44 of VGA memory. It rewrites them as `DOS_LOW()`. A trial on three files kept
all 49 functions byte-identical. It is applied once the 64-bit pass has finished with `src/`.

**2026-10-05, music.** The game's music plays as it did on a Sound Blaster 16. The HMI
format and SOS's player were worked out statically from FALL.EXE, and HMI's OPL drivers from
HMIMDRV.386. The two analyses are in `build/port/agents/hmi/` and `build/port/agents/opl/`,
each with a Python reference model.
- **Songs** (`hmi_seq.c`):
  - The format: tracks with MIDI channels and device designations.
  - Timing: one tick is 1/120 s. There are no tempo events, and note-ons carry their
    durations.
  - HMI's 0xFE events: branch points, local and global loop ends (a count of 0xFF loops
    for ever), loop-counter resets.
  - Channels: tracks with the same MIDI channel share one hardware channel, the first free
    of 0-8 and 10-15; drums stay on 9.
  - Volume: CC7 is scaled by the master volume, and drum velocities by the drum channel's
    volume.
  - Every game song but FOLK3.HMI loops for ever inside the player, as in DOS.
- **The driver** (`hmi_opl.c`): HMI's own fmmidi3.com, from HMIMDRV.386.
  - 9 voices, each written to both OPL3 register sets: set 0 heard on one side, set 1 on the
    other, with CC10 pan making one quieter.
  - Instruments from MELODIC.BNK. Drums from DRUM.BNK by note, at the pitch in byte 2 of
    the drum's name record.
  - Volume: TL from a 64-entry volume table at the scaled velocity.
  - Its quirks are kept: programs 125-127 silent, voice stealing that spares channels which
    had a pitch bend, the overlapping register shadows.
  - The analysis found that pan comes out mirrored on an SB16 (MIDI left on the right
    speaker); the port keeps that.
- **Checks:**
  - The note counts after 60 s match the reference simulator exactly: D1.HMI 107, 04.HMI 589,
    and TAVERN.HMI 1,089 with its local loop followed.
  - All 131 songs play: notes and level, no silence beyond the songs' own rests, no clipping.
  - The song length the player works out matches all 131 MIDI.BSA records (sosMIDIInitSong
    is given no length).
  - `vpcdemo --selftest` drives the game's own SOS MIDI calls (sos_init's order: the driver
    for 0xA009 at port 0x388, the two banks, a MIDI.BSA song) and finds the OPL3 playing
    from the live audio stream.
  - Rendering runs at about 15 times real time.

**2026-10-05, phase 3 done (the 64-bit pass, cb305f2).**
- The census of diagnostics that lose half a pointer is down from 7,243 to 19. All 19 are
  calls through `struct rect`'s button handler, which its tables call with 0 to 4 arguments.
- How:
  - `iptr`/`uptr` for ints that hold addresses (`tools/port_iptr.py`, a dataflow over
    clang's AST);
  - `include/clib.h` for the library prototypes;
  - prototypes for every declaration without parameters (`tools/port_protos.py`);
  - 165 pointer tables retyped (`tools/port_globals.py`).
  FALL.EXE stays byte-identical throughout.
- The data generator gives 521 globals native layouts. Two lists are left, both in
  `build/port/agents/x64/` with the reasons:
  - 510 relocated pointers in 18 globals whose neighbours have no symbols;
  - 76 declaration disagreements, 54 of them only in pointee type.
- What the pass left alone: the record and struct size literals and the save/load paths
  (the next pass, with the disk converters); 35 four-byte strides over pointer data.
- `tools/port_lowmem.py` has been applied: the game's 112 reads of real-mode memory (the
  BIOS tick, VGA memory) go through `DOS_LOW()` (include/doslow.h), with FALL.EXE
  byte-identical.

**2026-10-05, saves and sizes (step 1 of the plan after phase 3): done.**
- **Save and data files** (f71d9cc): every struct the game reads or writes in its 32-bit
  layout goes through a converter natively (port/include/disk.h, port/shim/persist.c), hooked
  by macros that are the original code under Watcom (include/portio.h). The DOS formats are
  kept bit for bit: classic saves load, and the game's own saves stay DOS Daggerfall's.
  `port/test/savetest` runs 868,399 checks:
  - 16,465 records of the 18 classic saves round-trip, and their SAVETREE.DAT and SAVEVARS.DAT
    come back identical through the game's hooks;
  - MAPS.BSA, BLOCKS.BSA's RMB blocks and the QBN files convert, with fields checked.
- **Sizes** (c92e896): about 180 struct sizes, strides and offsets written as numbers are now
  `REC_SIZEOF`/`REC_OFFSETOF`/fields (records.h). `tools/port_sizes.py check` confirms each is
  the old number under i386.
- **Narrow frame slots** (911d2cd and the cleanup after it): locals the code reaches through
  a wider type (`short x; *(int *)&x = v;`) use include/ptrint.h's `slot16`/`pslot16`, the
  declared type under Watcom and room enough natively. More hidden BIOS-tick and VGA addresses
  now go through `DOS_LOW`. `tools/port_slots.py` finds none left.
- **Pointer tables at a 4-byte stride** use `PTR_SHIFT`/`PTR_SIZE` (ptrint.h): the list
  popups' scratch tables (scratch_190be4/de4/ee4, typed for the data generator), the talk
  lines, the spell and item-maker lists.
- **Left:**
  - 510 relocated pointers in 18 globals whose neighbours have no symbols, and 70
    declaration disagreements: lists in `build/port/agents/x64/`;
  - natively written saves have not been loaded by DOS or Daggerfall Unity yet;
  - none of this has run in the game yet. The engine is the remaining blocker.

The 64-bit worklist (`port_census.py`) has 7,243 diagnostics that lose half a pointer:

| Kind | Count | Files |
|---|---|---|
| pointer-to-int casts | 4,638 | 148 |
| calls through a declaration without parameters | 2,128 | 173 |
| int-to-pointer casts | 448 | 96 |
| other narrowing (shorten, int-conversion, compares) | 29 | 15 |

## Linking the engine

XnGine's canonical C (`src/engine/`, from `main`) is compiled natively in place of the 175
stubs: CMake's `engine` object library, the game's dialect plus `-DPORT_ENGINE` (port.h leaves
the engine's own `open`/`read` members alone). `glue.asm` is the matching build's; natively the
virtual PC is the glue. What the engine has under `DAGGER_PORT`:
- **Registers and services.** `xn_regs` is pointer-wide (`unsigned long`, the layout of
  `struct vpc_regs`): a DOS read passes its buffer's address in EDX. `xn_int10`..`xn_int33` and
  `xn_inb`/`xn_outb`/`xn_inw`/`xn_outw` are the virtual PC's (port/host/vpc.c); xngine.h's
  arithmetic pragmas are inline C there.
- **Real-mode memory** through the virtual PC's low memory: xpc.h's BIOS tick and keyboard
  flags, gfx.c's `VGA_MEMORY` and VESA window, DOS buffers through `DOS_LOW()`.
- **Interrupts.** A vector is a C function in the virtual PC's table. pc.c's get/set/install
  hand out an index for a saved vector, since a 32-bit offset cannot hold a native pointer. The
  entries the Watcom build makes as stubs (`xn_kbd_int9_entry`, `xn_joy_timer_entry`,
  `xn_sys_crit_error_entry`, `xn_sys_divide_error_entry`, `xn_serial_irq_handlers`) are C
  functions at the end of their modules, and the lock regions' bounds are dummies. `cli`/`sti`
  and `pushfd`/`popfd` (xsysutil.h) are the interrupt lock (`port_cli`/`port_sti`,
  `port_cli_depth`).
- **No hardware exceptions**: an arm64 divide does not trap, and its 0 for a zero divisor is
  what Q-SYS-01's handler gave.
- **Names.** The game calls five engine functions by address in the sources. Four of them
  (`xn_vec_dir_to_angles`, `xn_model_push_player_from_pick`, `xn_shade_init_reserved`,
  `spell_has_no_effects`) were promoted to strong in names.csv. `xn_timer_tick_callback` was
  named `D_000CDDA8` as data. The matching build is unchanged.
- **The engine's data** comes from tools/port_data.py, the way the game's data does:
  - all of object 2's data, in address order, between its functions;
  - types from the engine's headers and .c files (several declarators on a line are parsed);
  - names from names.csv at any confidence and from xngine_aliases.csv (a struct and its first
    field get one layout, the other name as a label);
  - clang's canonical layouts, so typedefs (`u32`, function-pointer arrays) lay out.
  `xn_poly_ring_tables`, which the C reaches only through the rings, is typed in port_data's
  TYPES as pointers. A pointer at -1 stays all ones.
- Watcom's `malloc`/`free` (0xA10A8, 0xA117E), which the engine calls, are in port/shim/watcom.c.

The engine's own 64-bit pass (the struct checks in xnstruct.h, pointers through `u32`,
pointer-table strides, data-file pointer slots) is the engine agent's, merged separately.
The platform files' changes compile to identical Watcom objects (all nine checked with
tools/wcc10.py).

## Phases

1. **A native build that links.** Done.
2. **Data as C.** Typed definitions of the game's globals:
   - pointers become relocations (`&symbol`, function pointers);
   - pointer-holding globals become 8 bytes;
   - unnamed spans stay byte arrays;
   - AddressSanitizer finds code that relies on two globals being adjacent.
   - XnGine's data waits for the canonical engine, which decides what the game can see.
3. **64-bit clean.** Every value that holds an address gets a pointer-wide type:
   - `iptr`/`uptr` where the code does integer arithmetic on addresses. They are `int` under
     Watcom, so FALL.EXE cannot change, and `long` in the native build;
   - real pointer types where Watcom's bytes allow.

   It is driven by clang's diagnostics and its AST, with the matching build as the check.

   **Saves and data files keep their 32-bit formats.** The save-data audit is in
   `build/port/agents/persist/report.md`, regenerable from its `sites.csv` patterns.
   - What is on disk:
     - SAVETREE.DAT writes records byte for byte: the 71-byte header, then the character,
       monster and QBN quest data, model instances and links.
     - SAVEVARS.DAT holds the quest faces, a copy of the player's header, and the factions.
     - MAPS, the RMB blocks and the QBN files are read raw into growing structs.
   - Pointer fields on disk are ids (an unlink pass before saving, a relink after loading)
     or garbage the loader ignores.
   - Classic saves and Daggerfall Unity both use these formats, so the native build keeps
     them bit for bit:
     - packed `*_disk` structs with 32-bit slots;
     - a converter at each function that touches the disk;
     - under Watcom the original call.
   - Everywhere else, about 75 header literals (`+71`, `55`, `0x47`) and the struct sizes
     and strides become `sizeof`/`offsetof`, the same constants under Watcom.
   - `model_instance` and `block_model` hide the engine model handle's `lights` and
     `matrix` pointers in padding. They must be declared as pointers, or the game and the
     engine disagree about where the model's angles and position are.
4. **The engine as plain C.** That is the XnGine work on `main` (the canonical C).
5. **SDL3 platform layer:**
   - video: an 8-bit framebuffer and palette;
   - input: SDL keys into the keyboard handler; the mouse;
   - time: the BIOS tick from SDL's clock;
   - files: done;
   - sound: the SOS API on an SDL mixer, HMI music;
   - DPMI: memory as `malloc`.
6. **Bring-up against the original.** A headless native run with `fallplay`'s input
   scripts, compared frame by frame with `fallemu` in lockstep. Milestones: the title, a
   loaded classic save, walking, sound, all 18 saves.
