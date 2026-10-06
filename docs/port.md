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
| `port/host/main.c` | `main`: SDL, the folders, then the game's main (0x10010) with `Z.CFG`, as `FALL.EXE Z.CFG` ran; a backtrace for a stub or a fault; `port_check_ptr` stops on a pointer that lost its top half |
| `tools/port_build.py` | configure, build and generate; `run` prepares a game folder as `tools/fallemu.py` does and starts the build; `missing` lists the stubs |
| `tools/port_census.py` | the 64-bit worklist: clang's diagnostics on the game's C, by kind and by file |
| `tools/port_data.py` | the game's data (phase 2): object 3 in address order, globals whose types hold pointers in native layouts with 8-byte relocations from FALL.EXE's fixups; `report` lists declarations still too narrow |

## Building and running

```sh
.venv/bin/python tools/port_build.py                 # build/port/fall
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

The 64-bit worklist (`port_census.py`) has 7,243 diagnostics that lose half a pointer:

| Kind | Count | Files |
|---|---|---|
| pointer-to-int casts | 4,638 | 148 |
| calls through a declaration without parameters | 2,128 | 173 |
| int-to-pointer casts | 448 | 96 |
| other narrowing (shorten, int-conversion, compares) | 29 | 15 |

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
