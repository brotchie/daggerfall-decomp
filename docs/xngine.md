# XnGine (object 2)

Object 2 of `FALL.EXE` is the XnGine engine: hand-written asm, shared with Battlespire and
Redguard. This file is the plan and running log for it, kept apart from `docs/progress.md` (the
game C).

## Plan

Two layers in one source tree:

1. **Matching asm** (`src/xngine/*.asm`): modules that reassemble with Borland Turbo Assembler
   4.0, the assembler XnGine was built with, to the original bytes and fixups, with every address a symbol. This is the reference for the
   byte-identical build.
2. **Equivalent C**: each function rewritten in documented C and proved equal to its asm, by
   differential testing against an emulator first and then, where it pays, by a solver. This is
   what the native port and fuzzing build on.

A function counts at five levels: matching asm, documented, equivalent C (tested), proved,
ported. Phases:

| Phase | Work | Gate |
|---|---|---|
| 0 | Map object 2: code, data, modules, entry points | every byte is code, data or a named table |
| 1 | Matching asm: `tools/xn_disasm.py`, `tools/xn_link.py`, build splice | `FALL.EXE` byte-identical with object 2 from asm |
| 2 | Name the self-modifying patch fields | every patch write targets a named label |
| 3 | Test harness: DOSBox-X scene dumps, Unicorn frame replay | the asm replays itself; no unknown writes to code |
| 4 | Document: names, header comments, shared types with the game C | no `func_`/`D_` names left |
| 5 | Equivalent C, by batch: literal C for every function, then readable C | all functions pass replay and fuzz tests |
| 6 | Proofs: code-integrity frame, patch removal, per-function equivalence | each function proved or its gap listed |
| 7 | Native port hand-off | captured scenes render the same natively |

## What object 2 holds

Measured with `tools/xn_disasm.py` on 1.07.213:

| Fact | Value |
|---|---|
| Range | 0xC0000-0x161568 (660,840 bytes, 79% zero) |
| Functions | 657 (`config/xngine_functions.csv`): 348 in `config/functions.csv`, 58 more reached by calls, 100 through pointers in data (dispatch tables, callbacks), 151 nothing references directly |
| Code | 38,380 instructions, 133,169 bytes |
| Calls from game C into XnGine | 952 sites, 174 entry points |
| Calls from XnGine into game C | 66 sites, 14 targets |
| Self-modifying writes | 303 from 46 functions into 256 instructions of 56 functions (`config/xngine_patches.csv`) |

Error strings name the subsystems: `DOS:` file loading (0xC0B00), `WORLD:` (0xC2300),
`FONT:` (0x12DA00), `SET:` texture sets (0x132F00), `ENGINE:` shaders (0x136500), the 3D object
loader (0x13F700), `GFX:` mode setting (0x142900), `SYSTEM:` memory (0x147900), and a precision
timer whose messages match Michael Abrash's Zen timer (0x15F60C). The first 0x30 bytes of the
object are a table of 12 message pointers; the rest of that page is zero (the "zero page" the
engine checks for wild jumps).

## 2026-10-02: phase 1, matching asm

All of object 2 is now built from source, and `FALL.EXE` is still byte-identical.

- **`tools/xn_disasm.py`** writes 113 modules to `src/xngine/`, cut at 0x100-aligned addresses
  after zero padding (`config/xngine_modules.csv`). Code comes from recursive descent; a call
  into another object in bytes nothing reaches marks a gap as code (5 functions), and an
  instruction that would cut a fixup field in two is data, not code.
- **Every address is a symbol.** Each LE fixup becomes `func_X`, `L_X` (code) or `D_X` (data);
  a target inside an instruction is `label+k` (the patch fields). Each module declares
  `public` the 1,120 labels other modules use, so the set links with a real linker too.
- **No game bytes in the repo.** Plain data is written `B_<address>_<length>`; at build time
  `tools/xn_link.py` defines those macros from your `FALL.EXE` (`build/xngine/*.inc`). Zero runs
  are `db n dup (0)`.
- **`tools/xn_link.py`** assembles each module (with TASM 4.0, see below), writes its fixups as
  the linker and LE loader would, and requires identical bytes and the same target for every LE fixup in the
  range (16-bit selector fixups excepted: the LE table keeps them). `build_fall.py` calls it and
  splices the modules in; the `--blank` self-test and two negative tests (a changed constant, a
  pointer to the wrong table) behave.
- **Encodings.** A probe pass assembles each module with an `org` back to the original offset
  after every instruction, so one mis-encoded instruction cannot shift the rest; those are
  written as `db` (symbol fields stay `dd` expressions). What is left as bytes, and what the
  encodings say about the original assembler, is in the TASM section below.

## 2026-10-02: phase 2, the self-modifying code

Every write into code now targets a named field, and `config/xngine_patches.csv` lists all 303
(field, patched instruction, original value, kind, writer). The census needed a better code
map first: the first one stopped at 20,857 instructions because a lot of the engine is reached
only through dispatch tables, callback variables or nothing at all.

- **Code discovery**, in order: calls; an undecoded call into game C marks its gap as code;
  pointers in data or `offset` immediates whose target decodes as code (a clean path to `ret`
  or `jmp`, no string-like instructions, no fixup cut in two); and code after a `ret` past
  alignment padding (`nop`, `xchg ebx,ebx`). An address instructions read or write as memory is
  never a function entry; discovery restarts without any it took for one. This found 309
  functions beyond the 348 listed, and code went from 69 KB to 133 KB.
- **Patch fields**: an operand the code rewrites is `patch_<address>`, defined next to its
  instruction (`patch_1584AD equ L_1584AB+2   ; rewritten at run time`); a module that writes
  another module's field defines the name from that instruction's public label. Writers read like a setup list:

  ```asm
  func_0012A2D0:
      pushad
      mov dword ptr [D_000CEA28], eax
      ...
      mov dword ptr [patch_1584AD], eax
      mov dword ptr [patch_158608], eax
  ```

- **Kinds** (193 of the 303 writes patch a different function than the writer):

  | Kind | Writes | What |
  |---|---|---|
  | placeholder | 202 | Immediates assembled as round numbers and overwritten: 100000 (193), 10000 (4), 100 (4), 1000 (1). In `add`, `mov`, `cmp`, `imul`, `sub`, `shl` |
  | address | 63 | A memory operand's address rewritten (`mov`, `add`, `mul dword ptr [x]`, `lea`) |
  | opcode | 36 | Opcode bytes of 15 unrolled loops: `0xC3` (`ret`) planted at a computed entry, the original opcode (`8B`, `88`, or a saved word) put back after the call |
  | operand | 2 | A shift count (18) and a step (320, the screen width) |

The busiest writers are setup routines: `0x12A2D0` (29 writes), `0x12A100` (25), `0x13E8C8`
(20). What the static census cannot see (writes through registers, generated code) is phase
3's job: replay flags every write into code.

## 2026-10-02: phase 3, a headless FALL.EXE

Phase 3 needs XnGine's real inputs, so instead of dumping memory from DOSBox-X, the game now
runs headless in `tools/fallemu.py`: Unicorn for the CPU, with the extender, DOS, BIOS and PC
hardware the game touches written in Python. It boots `FALL.EXE z.cfg` to the main menu in
about a billion instructions (50 s, 20 MIPS) and saves the screen as a PNG.

- **Memory as CauseWay sets it up.** The game is zero-based and flat: it reads the BIOS tick
  count at 0x46C and video memory at 0xA0000 directly, so its objects load above 1 MB and are
  relocated through the LE fixups. They load at 0x01000000 + their preferred address, so
  `func_00012345` runs at 0x01012345. The six selector fixups in object 2 get the flat data
  selector.
- **Services:** DOS files over `build/game` (a clone of the patched install) with writes going
  to `build/emu/overlay`; DOS memory, dates and directory search; about 25 DPMI functions;
  CauseWay's `FF25h`/`FF26h`/`FF30h`; BIOS video, keyboard and mouse; VGA palette and retrace
  ports. The Watcom startup takes the DOS/4G path (`int 21h ax=FF00h`).
- **Deterministic:** the timer interrupt fires every 400,000 instructions while interrupts
  are enabled, and the date is fixed.
- **`Z.CFG`:** the game needs exactly one argument, its config file (`main` checks
  `argc == 2`, the "Please run DAGGER.EXE" message). The overlay holds one with `path`,
  `pathcd`, `maps` and `mapfile maps.bsa` (12 characters at most).
- Two things DOS does that mattered: handles are the lowest free number (Watcom's runtime
  only tracks 20), and a seek with an invalid mode fails without moving.

**A latent bug, found on the way:** XnGine's `func_000C31AE` seeks with `mov ah,42h / int 21h`
and never sets AL, so the mode is the low byte of whatever EAX held (a pointer, 0x1C, in this
run). DOS rejects it and does not move, and the 0x400-byte read that follows only works if the
file position is already right. The emulator logs it as a note each run.

## 2026-10-02: phase 3, input, a scripted new game, and call records

- **Input.** Keys go through the game's own int 9 handler (XnGine's, at `0x142840`). The
  mouse driver behaves as the game uses it: XnGine polls absolute position (function 3) with
  its own range, while some screens move the cursor from motion counters (function `0Bh`).
  XnGine's mouse code (`0x12B196`) pairs two presses within 8 BIOS ticks and 4 pixels into a
  double-click, so scripted clicks are 80+ timer ticks apart, and lists select with a
  double-click.
- **Snapshots** hold the whole machine and what the game wrote to its files, so each step of
  a long scenario resumes in seconds. `tools/scenarios/newgame.txt` boots, creates a Breton
  battlemage and skips the intro; it replays identically from boot.
- **The install's missing files.** Bethesda's free release ships the CD's `ARENA2` without
  `ARCH3D.BSA` and `DAGGER.SND`; the installer unpacks them from `PACKED.DAT` (`DAG_HUGE.LST`
  says so). That file is 256 KB blocks of PKWARE DCL "implode" data, each with a 36-byte
  header, and a directory of names and sizes at the end. `tools/blast.py` (after zlib's
  blast.c) unpacks both, 27,143,532 and 7,661,766 bytes, into the run's overlay.
- **Records** (`tools/xn_record.py`). An entry hook stops the CPU before a function's first
  instruction (by moving EIP to a `hlt`), and the call then runs alone with memory hooks: the
  record holds every byte it read before writing, the XnGine code bytes that differ from the
  file (the self-modified state), registers and descriptors, and its writes, exit registers,
  port I/O, and interrupts with what each service returned. Replay loads `FALL.EXE` fresh,
  puts that input back, answers services from the record, and compares. **All 13 XnGine
  functions that run at the main menu replay exactly.**
- **Tracer** (`tools/xn_trace.py`): at the menu, 13 of 657 functions run, the planted-`ret`
  loops make 4,400 opcode writes per 100 ticks, and nothing writes into code outside the known
  patch fields.

**The divide fault entering the world was by design.** The first 3D frame divides by zero in
`func_0015BC42` (`idiv` computing 2^32 / `[edi+5Ch]`, a texture gradient that is 0 for
axis-aligned walls seen straight on), whether or not the intro is skipped. XnGine expects it:
it installs a DPMI divide-error handler (`0x149FC8`) that reads the faulting `idiv`'s ModRM
byte, steps EIP over the instruction (2, 3 or 6 bytes) and returns with EAX = EDX = 0. A frame
takes about 10 of them. Two emulator changes made it work:

- CPU exceptions go to the game's DPMI 0203h handlers with a DPMI 0.9 frame (return CS:EIP,
  error code, faulting CS:EIP, EFLAGS, SS:ESP), and resume from the frame after its `retf`.
- Unicorn keeps the last exception in `env->old_exception` and clears it only when delivering
  through an IDT, so a second handled #DE came back as a double fault and the CPU stalled. The
  emulator finds that field in Unicorn's context blob at start-up (a scratch #DE) and resets it
  inside the hook after each handled exception.

With that, the headless game reaches the 3D world (a snowy exterior after the fast-start
character), at about 4.5 timer ticks a second. The handler was not in the code map (it is
reached only through DPMI and ends in `retf`); `tools/xn_trace.py` now writes code it sees run
outside the map, and handlers the game registers, to `config/xngine_seeds.csv`, which
`tools/xn_disasm.py` starts from.

Records now also replay port reads, and carry the exception handlers: all 110 records from the
menu, the province map, character creation and the intro video replay exactly.

## 2026-10-02: phase 3 in the 3D world

- **Exact records.** Inside blocks that rewrite their own code, Unicorn drops some memory-hook
  events (a read of `0xC28BC` in `0x13E8C8` reached no hook, though the same bytes alone report
  correctly). Records no longer depend on hooks: a call's input is the 4 KB pages that differ
  from the snapshot the recording started from, and its effect is every byte that differs
  after it. **All 387 calls of 194 functions recorded in 150 ticks of the world replay
  exactly.**
- **Coverage.** 231 of the 719 functions ran in 300 ticks of walking and turning outside.
- **Code generated at run time, found by the tracer.** `func_0015C274` patches a 418-byte
  texture-mapping template (`func_0015C300`: 8 unrolled steps, each with a mask and two table
  addresses) through a register, then copies it into a heap buffer that holds up to 768
  compiled instances; `func_0015C2DC` re-patches an instance in place, and the renderer calls
  the copies. The static census could not see these 32 writes (the address is in EBX);
  `tools/xn_trace.py` reports them in `config/xngine_runtime_patches.csv` and the patch table
  lists them as kind `register`, named like the others (`patch_15C307 equ L_15C306+1`). The
  census is now 337 writes. For the C version this is one texture mapper with parameters; the
  records already hold the generated code, since they keep whole pages.
- **What the generated code is (read from the asm, not yet run).** XnGine draws
  perspective-correct textures the way Quake later did: subdivide each span every 16 pixels,
  do the perspective divide at the ends, and fill the 16 pixels in between with an affine
  inner loop. That inner loop is the template: per pixel, `bswap` and `mov ah,bh` turn packed
  fixed-point u/v (EBX, stepped by ECX) into a texel offset, `and` with the texture's size mask,
  fetch the texel from the texture's base, map it through a shade table (EDX, stepped by
  ESI/EBP every second pixel), store to `[edi+k]`. The generator bakes one texture's mask and
  base into a copy (the per-texture specialisation that code generation buys); the texture
  cache (`0x135D00`) builds the mask from the texture's size via the table at `0x129EC0`,
  keeps the copy's address in the texture record at +0Ah, and re-patches the copy when the
  texture's data moves. The span routine `0x156A80` calls the copy 16 pixels at a time
  (`call [D_00156A40]`); for the last, shorter run it plants a `ret` inside the heap copy, calls
  it, and puts the `0x0F` back: the planted-`ret` trick again, on generated code, which no
  static census of object 2 can see. In 220 ticks outdoors no generated copy ran (74 were
  built); an interior or a wall close up should exercise them.
- **Phase 3 gate.** The asm replays itself exactly in every scene recorded so far (menu,
  province map, character creation, intro video, the world), and every write into XnGine code
  seen at run time is now a named field. What remains is breadth: dungeons, towns, combat,
  the automap and the other screens, to run the other 488 functions.

## 2026-10-02: XnGine was assembled with TASM 4.0

The game's C was compiled by Watcom C32 10.0a, but XnGine's asm was not assembled with Watcom's
WASM: its 5,336 register-to-register ALU ops and `mov`s are in the `reg, r/m` form
(`mov edi, esi` = `8B FE`), and WASM writes the other one (`89 F7`). The bytes point to
Borland's Turbo Assembler in its default single-pass mode with `JUMPS`, and TASM 4.0 (14 Dec 1993, from your own copy, unpacked into `third_party/tasm/BIN`)
reproduces each fingerprint:

| In the original | TASM 4.0 |
|---|---|
| reg,reg ALU and `mov` in the `8B /r` form, `sbb` included (`1B`) | same |
| 24 forward short `jcc` followed by exactly 4 NOPs, at 86-127 bytes, never after a backward jump | an unmarked forward `jcc` is reserved as a 6-byte near jump; when it comes out short, TASM pads with 4 NOPs (`/m` multi-pass would remove them) |
| `loop $+4` / `jmp short $+7` / `jmp near X` (2 places) | an out-of-range `loop X` under `JUMPS` |
| `cmp`/`and`/`add ax,imm` always in the accumulator form (`66 3D iw`), never `66 83 /r ib` | same, even for small values |
| `xchg ebx,ebx` (`87 DB`) and `nop` between functions | `align 4` fill in a code segment |
| `xchg`/`test reg,reg` with the first operand in the reg field | same |
| `shr reg,1` 63 times as `D1` and 23 times as `C1 /r 01` | `D1` for any count it knows is 1; `C1 /r ib` for an `extrn K:abs`, a count the linker fills in |

The build uses TASM: `tools/xn_link.py` runs `TASMX.EXE` (TASM 4.0 as a DPMI program;
real-mode `TASM.EXE` runs out of memory on the larger modules and writes the same objects) in
up to 8 DOSBox-X sessions side by side. Without TASM the build keeps object 2's original
bytes. The generator writes the source the way
the authors evidently did: `jcc short L` for short forward jumps, a plain `jcc L` for the rest
(TASM sizes them, and writes the 4 NOPs where they belong), `loop X` for the stretched
sequence, and externs declared inside the 32-bit segment (outside it TASM addresses them with
16-bit offsets). TASM handles segments past 64K, so the modules are the original 113.

What is left as bytes: 28 instructions.

| Instruction | Count | Why |
|---|---|---|
| `shr reg,1` as `C1 /r 01` | 23 | most likely a shift count from another module, resolved by the linker; 18 are in one routine at 0xC33EA-0xC3993 next to `shr eax,1` written with a literal |
| `add r8,r8` as `00 /r` | 4 | TASM writes `02 /r`; all four are in `xn_160900`, maybe assembled by hand |
| data decoded as code | 1 | `add byte ptr [ebx], dh` |

The three `mov ax, sel` loads are bytes as before: their selector fixups stay in the LE table.

Run: `.venv/bin/python tools/xn_disasm.py` regenerates `src/xngine/` (12 s, most of it in
DOSBox-X); `.venv/bin/python tools/xn_link.py [module]` checks modules (5 s for all);
`tools/build-and-verify.sh` builds.

## 2026-10-03: the 3D world at full emulator speed

In the 3D world `fallemu.py` ran at 0.5 MIPS, against ~30 MIPS in the menus: 500 timer ticks of
gameplay took four minutes. Profiling put nearly all of it inside Unicorn, translating code
again and again (`tcg_gen_code`, `tcg_optimize`, liveness), for three reasons:

- **XnGine's divide errors.** The renderer divides by zero about 240 times a tick and lets its
  own handler zero the quotient. Stock Unicorn never clears `env->old_exception` for an
  exception a hook handles, so `fallemu` reset it with a context save and restore per fault.
- **Instruction counting.** `uc_emu_start(count=N)` is a hook on every instruction: each
  translated block grows several times over, and every instruction calls it.
- **Self-modifying code.** XnGine rewrites operands and plants `ret`s all the time (4,000 code
  writes a tick in a town); each write throws away the translation of the block around it,
  and the next run translates the whole block again. On a page whose code keeps being
  retranslated, QEMU also discards the page's code bitmap whenever a block is added, so every
  write scanned all of the page's blocks.

A self-modifying loop in bare Unicorn: 1,000 MIPS unpatched, 2.4 MIPS patching itself, 0.4 MIPS
patching itself while counting. `tools/build_unicorn.sh` builds Unicorn 2.1.4 with
`tools/unicorn/dagger-unicorn.patch` (11 files, ~100 lines), and `fallemu.py` uses it whenever
it is built:

- the count is kept a block at a time, in the exit check every block already makes (the
  block's instruction count is a parameter filled in when its translation is done); a slice
  ends on a block boundary, so runs stay deterministic (`UC_EXACT_COUNT=1` for the old way);
- an interrupt a hook handled clears `old_exception`, as delivering it would;
- code on a page the guest has patched is translated in blocks of at most 8 instructions and
  without the optimiser, so each patch retranslates little (`UC_SMC_TB_INSNS`, `UC_SMC_NOOPT`);
- a page's code bitmap is built at the first write and kept up to date as blocks are added
  (bits cleared only when the page has no code left), so a write next to code costs a bit test;
- a store that leaves memory unchanged does not invalidate anything.

| | stock Unicorn | patched |
|---|---|---|
| 3D world (four saves: town, dungeon, ship, castle) | 0.5 MIPS | 30-40 MIPS |
| boot to the main menu (2,600 ticks) | 30.5 s (34 MIPS) | 15.2 s (68 MIPS) |
| 300 ticks in the world | ~3.5 min | 3-4 s |

Two runs of each save end with identical memory, and every recorded XnGine call still replays
exactly (387 of 387 in the world, 110 of 110 in the menu set). Snapshots carry over between the
two builds; ticks now land on block boundaries, so a run continues differently from one made
with stock Unicorn, deterministically. What remains is mostly retranslation of patched code,
Apple Silicon's W^X switching for the JIT, and the per-block exit check.

## 2026-10-04: phase 4, names and the engine map

Every one of the 719 functions has a name in config/names.csv: 112 confirmed, 410 strong
and 193 candidate. Most of the candidates are dead code or never ran in play. There are
also 481 globals.

**How they were named.** One dossier per function gathered:
- the game C callers, which are named now, and their call sites;
- XnGine callers and callees;
- the data tables, the globals each function reads and writes, strings and ports;
- the patch fields, play evidence, and the asm itself.

Direct calls confirmed some names. `xn_render_set_mode` 0 draws only span ends (an outline
view), and 4 draws solid colours without textures.

docs/xngine_map.md is the engine map:
- **Subsystems:** draw, sys, collide, the VR helmet drivers, world, gfx, span, model, terrain,
  tex, poly, light, anim, and the rest.
- **Startup.** `init_video` starts the subsystems in order.
- **The frame**, in order:
  1. Texture cache, render pool and lights.
  2. Camera matrices.
  3. The game's object loop queues models, flats and lights.
  4. Outdoors only: the world update and the terrain.
  5. `xn_render_frame` draws the sorted models, fills the background, runs the span pass,
     then draws the flats.
  6. A redraw if the texture cache overflowed, then the water and the present.
- **The pipeline.**
  - Projection, outcodes and Sutherland-Hodgman clipping.
  - A per-row S-buffer of spans, where the nearer span wins by 1/z.
  - Span routines (solid, textured 8 and 16 px, 64×64 terrain, sprites), each ending its
    last partial run with a planted `ret`.
  - Lit polygons get per-pixel shaders compiled from three templates.
  - The 418-byte texture-mapper template is compiled per texture. Its copies run in play
    (334 traced episodes).
- **Corrections.** 0x153400 and 0x160E00 are not video drivers: they are three serial
  head-tracker (VR helmet) drivers on an 8250 UART driver.
- **Dead code.** 139 functions have no caller at all and never ran: XnGine's C-wrapper DOS
  library, a world editor, most collision primitives, star drawing, and the disabled river and
  path generators.
- **Bugs found:**
  - texture eviction can never fail;
  - the pick routine's stack imbalance;
  - the cursor clip uses the x hotspot for y;
  - flat collision has scale 0;
  - the divide handler doesn't decode SIB forms;
  - every flat skips its first visible piece.


## 2026-10-05: phase 5, literal C

Every XnGine function now has a C version. It is generated from the function's
instructions, compiled by the real Watcom C32 10.0a, and checked against the record corpus.
**All 620 recorded functions replay all 8,204 of their records exactly.** That holds with
one function at a time in C, and with all 719 in C at once. "Exactly" means the exit
registers, the six arithmetic flags, every byte written, and the port I/O and interrupts.

The game also runs with all of XnGine in C:
- 2,000 ticks of walking and turning in each of eight saves, with no fault (57M C calls);
- a still scene stays pixel-identical to the asm after 300 ticks.

| | |
|---|---|
| Translated | 719 of 719 functions (39,257 instructions). Two instructions are left to the asm: a `pop ss` in data, and a 16-bit `iret` |
| Compiled | 719 (73 files, 75,000 lines; 30 s in four DOSBox-X sessions) |
| Records, one function in C | 620 / 620 functions, 8,204 / 8,204 records |
| Records, every function in C | 620 / 620 functions, 8,204 / 8,204 records. Inside them the C of 678 functions runs, including the 57 handler blocks that have no records of their own |
| Differential tests (made-up entry states) | 556 functions, 4,033 tests, all agree. Ten of them are among the 41 functions no record runs at all |
| Not tested | 31 of the 99 functions without records: 16 that fault when called and 10 that wait on a timer, key or port (no made-up state returned), and 5 that are data |

**The C.** `tools/xn_c.py translate` writes `src/xngine_c/xn_<module>.c` (a module per file,
big modules in several), and each function becomes `void xn_<va>(void)`, its name and
evidence from config/names.csv in a comment above it:

```c
/* xn_span_flat_transparent (strong): Flat sprite span ... */
void xn_00157620(void)
{
    ...
    R.ebx -= M32(XN(0x157622));                                  /* 157620: sub ebx, 0x2710 */
    R.ecx >>= 13;                                                /* 157626: shr ecx, 0xd */
    ...
    fa = R.ebp; fb = 0x8;                                        /* 157678: cmp ebp, 8 */
    fr = fa - fb;
    LF(XF_SUB | 2, fa, fb, fr);
    if (((s32)fa < (s32)fb)) goto L_15772F;                      /* 15767B: jl 0x15772f */
```

- **Registers and memory.** The registers are a struct `R` in memory (sub-registers are
  macros: `AL`, `BX`...). Memory is the game's: `XN(va)` is where address va is loaded, and
  `M8/M16/M32` read and write it. The stack is `R.esp` in that memory, so pushes, calls (the
  original return address) and returns write the same bytes as the asm.
- **Flags** are computed only where something reads them. A forward pass finds, for each
  reader, the flag writer whose values reach it on every path; when there is one, the reader
  is a C expression on that writer's operands, held in locals (`cmp` + `jl` becomes a signed
  compare, across labels too). Other readers use lazy flags: the writer stores its operation,
  operands and result in `F` (`LF(...)`), which `xn_cond`/`xn_eflags` turn into flags when
  read. Which writers store is a backward liveness pass. Returns, calls, `pushfd`, a divide
  (its fault frame holds EFLAGS) and jumps out of the function read every flag; a writer
  right before one of them stores just there. A writer whose flags no one reads is plain C
  (`R.ecx >>= 13`). The formulas are QEMU's, undefined flags included (logic ops, shifts and
  multiplies clear AF; a shift's OF compares the top bits before and after its last step;
  `bsf`/`bsr` set the logic flags of their source; divides leave the flags alone), checked
  against Unicorn instruction by instruction first.
- **Self-modifying code.** An operand the program rewrites (the 337 patch fields) is read
  from its code bytes when the instruction runs (`M32(XN(0x157622))`), so the C follows
  every setup routine whether that routine is asm or C. In the 15 functions where a `ret` is
  planted at a computed instruction, each instruction first checks its own first byte
  (`if (M8(XN(0x157765)) == 0xC3) { LF(...); RET(0); }`). The three selector loads read the
  selector the loader wrote.
- **Control.** A function's body is everything reachable from its entry without passing
  another function's entry (shared code is translated into each function that reaches it).
  Jump tables become `switch`es on the table's entries. A call is `PUSH32(return address);
  xn_call(target)`: through the callee's asm entry, which leads to its C when that function
  is routed to C. A jump out of the function, or an instruction the translator does not
  handle, is `GOTO_ASM(address)`: the C returns and the asm continues there, which is always
  correct. Functions longer than 600 instructions run as parts that pass the next label
  between them, and modules are cut into files of at most 2,500 lines: Watcom 10.0a's code
  generator runs out of room ("internal compiler limit") on big functions and big files.
- **What C cannot say.** Watcom 10.0a has no 64-bit integers, so the 32x32 multiplies and
  64/32 divides (`imul r/m`, `div`, the overflow test of `idiv`) are one-instruction inline
  pragmas. `int`, `in`/`out` and `ins`/`outs` are helpers that do the real instruction, so
  DOS, DPMI and the ports see the same calls.

**The runtime** (`src/xngine_c/runtime.c`, `runtime.h`, `xn_rt.asm`):
- **Entering C.** The loader writes a stub per function. It saves the CPU in `R`, sets
  `xn_target`, and enters `xn_thunk`, which switches to a separate C stack and calls the
  function. When it returns, the thunk loads `R` back and jumps to `xn_ret`: the address its
  `ret` popped, or wherever it jumps.
- **Calling asm.** `xn_call` does the reverse. The caller has pushed the original return
  address; `xn_call` replaces it with a trampoline for its nesting depth, runs the target, and
  puts the original back when control returns. Two things the engine does needed more than
  that:
  - **Returns that skip a frame.** A light handler jumps to the shader setup, which returns
    to the handler's caller's caller. The depth-tagged trampolines tell `xn_back` which
    pending call is returning, and the frames in between are dropped, their slots put back.
  - **Calls that never return.** The span routines leave the row loop by unwinding the
    stack (0x12A949: `add esp, 18h`) into the loop's next iteration. A C entry finding the
    asm stack (within 64 KB) above a pending call's return slot drops that call's frame.
- **Interrupts.** The C runs with interrupts off, and `R.eflags` keeps the program's IF.
  - On the way in, the flags are pushed on the asm stack and the dead word put back.
  - On the way out, interrupts come on with an `sti` whose one-instruction shadow covers the
    final jump.

  An interrupt never finds ESP on the C stack. The stubs set `xn_target` (and the
  trampolines `xn_back_k`) only after `cli`. Segment registers are loaded
  only when the C changed them: loading a selector sets its descriptor's accessed bit, a
  write the asm never made.
- **The divide error.** A divide that would fault takes a real fault, with the asm's
  registers and stack (`xn_divfault`: a `div` by a zero in memory). So the program's own
  handler, asm or C, runs on the same frame, and the emulator logs the same exception. The
  frame's return address is then made the original instruction's.

**The harness** (`tools/xn_cload.py`):
- **Link.** A small OMF linker lays the objects out at 0x10000000, in a region it maps in
  the machine above the emulator's memory, so the C, its data and its stack are never part
  of a record's compared memory.
- **Routing.** A function is sent to C by a code hook at its asm entry that moves EIP to its
  stub. The asm bytes stay as they were, so the patch fields and planted `ret`s land where
  the C reads them.
- **Records.** `test` replays each record with only that function in C (`-j 3`: three
  worker processes). `--all-c` routes every function, so each record also tests everything
  its call runs, including the 57 handler blocks that have no records of their own.
- **Differential tests.** `diff` makes up entry states (another record's machine, registers
  taken from other records, small numbers or random values), records the asm's call from
  each one that returns without a service call, and replays that with the C. This is for
  functions the corpus never ran.

**Bring-up.** The first run, on 300 records of 100 functions, passed 97. The fixes on the
way were in the translator, the flag analysis and the runtime, never by hand in the
generated C:
- a function whose code starts before its entry;
- flags at a planted `ret`;
- Watcom's limits;
- selector immediates;
- descriptor accessed bits;
- a far-return flag left set after a nested exception handler returned;
- the two stack tricks above;
- the interrupt windows (below).

A made-up state can make a function overwrite its own code: 0x147820, given a bogus image,
remaps 88 KB from inside object 2 through its colour table, and the asm then runs what it
wrote. Such states are not tests, and `diff` skips them.

The long timeout loops in the serial head-tracker drivers (16M iterations of `cmp`/`loop`)
take the C several times as many instructions as the asm, so a call that does not return
within 200M instructions is retried with a 3G limit.

**Run:**
- `.venv/bin/python tools/xn_c.py translate` (3 s), then `build` (30 s), then
  `test -j 3` (17 min for the corpus, 5 of them on the head-tracker timeout loops) and
  `test --all-c -j 3` (14 min). A few functions:
  `test 0x157620 0x15BC42`.
- `diff --untested` runs the differential tests. `show 0x157620` prints one function.
- `play SNAP --ticks N [--script INPUT] --shot out.png` runs the game itself with XnGine
  in C (`--asm` for the comparison run).
- Results land in build/xngine/c_report.csv: per function, translated, asm fallbacks,
  compiled, records passed (alone and all-C), the records its C ran in, the differential
  tests, and the corpus's coverage note.

**The game in C** (`play`). The emulator delivers interrupts between slices, wherever the
CPU stopped. A few instructions of the runtime run with interrupts on:
- the stubs save the registers before their `cli`;
- the way out is `sti; jmp [xn_leave_to]`, which a CPU never interrupts (the `sti` shadow).

An interrupt there, whose handler calls into C (the sound library's timer calls
`xn_timer_tick_callback`), would overwrite `R` or the jump target. So `play` steps the CPU out
of the runtime before delivering one. With that, all eight saves run 2,000 ticks with
XnGine in C. Timing differs from the asm (the C takes more instructions per frame), so a moving
scene ends a little differently from the asm's, but renders the same way.

**Left:**
- **Readable C.** The literal C is the asm's register-level dataflow. The next step is
  readable C: locals instead of `R`, structs from config/names.csv and docs/structs.md,
  loops for the unrolled spans, and the generated texture mapper as one routine with
  parameters.
- **The runtime is for mixed asm and C**, inside the emulator. Once everything is C, the
  thunks, trampolines, frame unwinding and interrupt windows go away with the asm.

## 2026-10-05: phase 6, readable C

The literal C (phase 5) is the asm's register-level dataflow. Phase 6 rewrites the engine as
readable C: named parameters and locals, structured control flow, named globals and
structs, loops where the asm unrolled. Each function is still checked against the recorded
calls. The infrastructure is done, and the pilot proves it: the vec, mat and math
subsystems (62 functions, 1,827 instructions) are readable C in `src/engine/`.
`docs/xngine_readable.md` is the guide for converting the rest.

**The interfaces** (`tools/xn_abi.py analyze`, 30 s, writes `config/xngine_abi.csv`). Each
row gives a function's register interface:
- its inputs and stack arguments;
- the outputs callers read and the flags they read (`flags_out`);
- the clobbers no caller reads;
- `ret N`, the convention, and whether all its callers are known.

How the analysis works:
- **Registers in parts.** AL, AH and the upper half are tracked apart, so `mov dx, [..]`
  reads nothing of EDX. A `bsr` or `bsf` counts its destination as an input: a zero
  source leaves it.
- **Per function.** A forward pass follows ESP and a value token per register and stack
  slot (push/pop pairs, pushad/popad), to find what each function preserves. Strong
  liveness then runs over registers, flags and stack slots: a pop into a dead register
  makes nothing live.
- **Interprocedural**, over objects 1 and 2 (4,034 functions). The forward stage goes
  bottom-up over the call graph's strongly connected components; the liveness stage is a
  worklist.
- **The game's calls.** The game's Watcom 10.0a code never reads, after a call, EAX
  (unless it is the result) or the argument registers it passed. Watcom 10.0a does not
  preserve a callee's argument registers: a two-argument function never saves EDX. The
  brief assumed "every register but EAX"; the game's code shows otherwise.
- **Indirect calls** resolve through jump and call tables, constant stores into variables
  and struct fields, and `xn_abi.py trace`: the targets the record corpus reaches
  (`build/xn_readable/indirect.json`, 45 sites, 503 targets). Jumps into the middle of a
  function are analysed as entries of their own; calls into generated code are anything,
  and so is `push target; ret`.
- **DOS, DPMI, mouse and BIOS calls** have their registers listed, by the constant in AH or
  AX.
- **Overrides.** `config/xngine_abi_override.csv` replaces a column where there is no
  caller to decide (7 overrides, the pilot's dead functions). `xn_abi.py overrides`
  applies them in seconds.

| | |
|---|---|
| Conventions | reg 499, watcom 166, interrupt 37, unknown 13, template 4 |
| Callers | known 468, partial 89, unknown 162 (143 with no caller: every register they change is an output; 19 whose address escapes) |
| flags_out | 94 functions: CF in 85, ZF in 6, ZF SF OF in 2, all six in 1 |
| Outputs | none 237, one 122, several 360 |

Cross-checks on the record corpus:
- `check` (static): in every one of 8,116 records, the registers a row says are preserved
  hold their entry values at exit.
- `clobber --all`: at every return of every function, its clobbers, and the flags it does
  not output, are scrambled. 8,204 / 8,204 records pass, compared by each record's own
  row; the scramblers fired at 9.0M returns of 636 functions. The test found two analysis
  bugs on the way: a call that falls into the next function's entry has to count that
  function's needs as live after it (`xn_render_frame` reads CF that `xn_render_draw_flats`
  leaves), and what is live at a function's returns, for its own calls' sake, includes the
  registers its callers read that it preserves, not only its outputs.
- `inputs`: every register that is not an input, and every flag, is scrambled at entry.
  8,107 / 8,107 records pass (the 97 of handlers, data and templates are not tried). In a
  record that calls DOS or the BIOS, the replay answers with the recorded registers, so
  there the test compares outputs, memory and I/O. The test found that `push target; ret`
  dispatchers are jumps, not returns.

**How readable C runs** (`tools/xn_rc.py`):
- `build` compiles `src/engine/*.c` and `glue.asm` with Watcom 10.0a and links them at
  0x11000000 (`tools/xn_cload.py`'s linker, given a resolver for names to the loaded game).
- The C runs natively on the game's stack: no `R`, no thunk. A code hook at a converted
  function's asm entry sends its asm callers there, in one of three ways:
  - **direct**: a pragma (`parm`, `value`, `modify exact`) or Watcom's own convention
    matches the row;
  - **keep-eax**: `push eax; call; pop eax` for callers that need EAX kept, which
    Watcom's code never keeps;
  - **glue**: `pushfd; pushad; call NAME_r; popad; popfd` for several outputs or flags
    out. `NAME_r(xn_regs *r)` unpacks the registers into a natural call.
- `test` compares as the interface sees it: memory exactly except the dead stack below
  the exit ESP; registers except the clobbers, preserved ones against their entry values;
  flags only for `flags_out`; I/O and interrupts exactly.
- Also `diff` (made-up states, `tools/xn_cload.py`'s diff_test), `play --compare` (screens
  against the asm), `report` (`build/xngine/rc_report.csv`), `skel` (a header and stub
  C from the rows) and `asm` (a listing with names).
- Shared pieces:
  - `tools/xn_record.py`'s replay is split into `replay_run` and `compare`;
  - `tools/xn_cload.py`'s `replay_c`, `test_files`, `diff_test` and `play` take any image;
  - its `parallel` runs a task over the record files in `fallemu.workers()` processes,
    with memwatch.

The literal C still passes (`xn_c.py test 0x157620`).

**The pilot.** `src/engine/xngine.h` (types, the 64-bit helpers, `xn_regs`), `glue.asm`
(`xn_asmcall`, `int` calls), and `xvec.h`/`vec.c`, `xmat.h`/`mat.c`, `xmath.h`/`math.c`.
The 62 functions route 33 direct, 7 keep-eax and 22 through glue.

| | |
|---|---|
| Records, each function alone in C | 62 / 62 functions, 803 / 803 records |
| Records, all 62 in C at once | 62 / 62, 803 / 803 |
| The whole corpus with all 62 in C | 8,204 / 8,204 records pass, each compared by its own function's row; 1,814 of them run pilot C (43,562 entries) |
| Differential tests | 59 / 60 functions agree on every made-up state. `xn_mat_dot_fixed` differs on 1 of 7: a count of 3.6M walks its pointers through the dead stack, where the C's frame lives |
| The game | four dungeon saves render pixel-identical to the asm after 300 ticks; two saves walk 1,500 ticks (750,000 C calls each) without a fault. Outdoors, rain and walking people make the screens differ (timing) |

What the pilot taught, now in the guide:
- `modify exact`: without it, Watcom changes a pragma function's parameter registers.
- Watcom's code never keeps EAX, hence the keep-eax stubs.
- Watcom will not take EBP in a pragma.
- Scratch globals the asm writes are kept, with names: `xn_mat_from_angles_tmp`,
  `xn_math_tri_edges`, `xn_vec_angles_in`, the matrix strides. They are proposed in
  `build/xn_readable/names.csv`.
- Divide errors, with XnGine's own handler, are reproduced by dividing where the asm does
  with the register-operand helpers.
- `bsr` of zero is kept with `xn_bsr(v, old)`.

**The conversion (2026-10-05).** Six agents converted the rest, a group of subsystems each,
from the designs in docs/engine/ (the data structures with their evidence, `structs.md`; the
self-modifying, unrolled and generated code, `smc/`). Each group worked in its own folder and
its own copy of the interface table. The coordinator merged the groups into `src/engine/`
(97 files), the agents' interface overrides into `config/xngine_abi_override.csv` (148 rows,
each with its reason and, for a live function, a passing clobber test), and their names into
`config/names.csv` (336 more, 277 of them the patch fields).

| Group | Subsystems | In C | Records (alone and all routed) |
|---|---|---|---|
| pilot | vec, mat, math | 62 / 62 | 803 |
| 2D drawing | draw, img, font | 72 / 75 | 983 |
| rasteriser | span, shade, tmap, light, `xn_render_frame` | all but data | 1,165 |
| 3D pipeline | poly, render, flat, model, cam, tex | 111 / 111 | 2,197 |
| world | collide, world, terrain, sky | 104 / 105 | 1,375 |
| water and screen | water, gfx, vid, pal | 64 / 64 | 489 |
| system and input | dos, sys, str, mem, rand, timer, serial, helmet, anim, mouse, kbd, joy, bits, spell, input | 174 / 228 | 1,186 |

**648 of the 719 functions run as C** (281 direct, 258 through glue, 104 keep-eax, 5 interrupt
handlers). The other 71 are not missing:
- 62 are blocks inside another function's code, which that function's C covers: the run-time
  blocks of `xn_render_frame` and of the keyboard, joystick, divide-error and serial handlers,
  two jump targets, an unreachable failure tail and an unreachable continuation;
- 9 are data the disassembly listed as code: the four code templates (the texture mapper's
  and the three light shaders'), two offset tables and three strings.

The code generators stay byte-exact (the records compare the pools), but the C never runs
the generated code: it evaluates the light shaders and the texture-mapper copies from the
copies' own operands. Patched operands are named variables at their fields' addresses.
Unrolled bodies with a planted `ret` are loops whose count is read from where the `ret` was
planted.

**Verification, all of it in C at once:**
- `xn_rc.py test ... --all`: 613 / 613 functions with records pass all of them (8,157 records).
- `xn_rc.py test --corpus`: 8,204 / 8,204 records pass, each compared by its own function's
  interface; 8,198 of them run some of the C (209,970 entries).
- Differential tests (made-up entry states, and the agents' targeted and service-call states)
  for the functions without records.
- `xn_rc.py frames SNAP` (new): the game a frame at a time in lockstep. From the same machine,
  one pass of main's loop runs with the asm and with the C, the timer held, and then all of
  memory and the screen are compared. On all 18 saves: 20 frames
  standing and 20 after walking with the timer held, then 30 frames walking and 30 walking and
  turning with the same timer interrupts given to both machines each frame (`--irqs`; every
  one of the 30 screens differs from the one before). All 1,800 frames are identical in all of
  memory and on the screen.
- `play --compare` runs by timer ticks instead. The timer fires every so many instructions, the
  C takes a different number, and a moving scene ends differently (lights flicker on other
  ticks); 300 ticks from six saves run without a fault.

**What the merge needed:**
- A game function with a C definition is called by its C prototype in the analysis.
  `mem_check_heap` had made every register an input, and spurious outputs cascaded through
  `xn_render_frame`'s callees.
- A replayed DOS/DPMI/BIOS call now writes back only the registers the service defines
  (tools/xn_services.py). The others keep what the replayed code holds, which differs when C
  keeps its own stack addresses across an `int`.
- Declarations across groups. A group that called another group's function declared it with
  the asm's register convention; once both were C, the call went to the C definition. The
  world group's DOS calls were fixed to the system group's prototypes. A check of every
  `#pragma aux` against the definition's (tools/xn_declcheck.py) leaves only
  compatible ones.
- Two names for one address, where the C views a struct and its first field
  (config/xngine_aliases.csv).

**Left:** the C is readable, but it still answers to the asm. Interfaces follow the asm's
registers (`#pragma aux`, the `_r` adapters). Values that callers or the records see are kept:
leftover registers, scratch globals, patched code bytes, the generated code, 16-bit wraps and
the original bugs. Data stays at its addresses in the game. A next pass can drop each of those
once nothing asm calls the engine, testing against the game's calls and the frames instead of
each function's records.

## 2026-10-05: canonical C, part 1: the boundary, the yardsticks, the pilot

The next pass makes the engine **canonical C**: plain C modules, documented, with the
original bugs kept and catalogued, shown equivalent to the asm at the engine's boundary with
the game rather than function by function at the register level. `docs/xngine_canonical.md`
is the guide; this part built the mechanism and proved it on vec, mat and math.

- **The boundary** (`tools/xn_boundary.py`, `config/xngine_boundary.csv`): 181 entries (952
  game call sites into 174 functions, 3 held pointers, run-time pointer calls traced in the
  scenarios, 5 vectors), 15 game functions, 40 service forms and 13 port forms as exits, and
  the memory the game can see. That is found dynamically: every scenario runs on the asm with
  memory hooks, code attributed by EIP; a byte the game reads after the engine wrote it is a
  flow. Object 2's items, low memory and the engine's allocations (found by a boot with
  hooks on its malloc and free sites) are classified visible (compared), private (masked) or
  unobserved (compared), with the evidence in each row.
- **Boundary comparisons**: `xn_rc.py test --boundary` (the records of entries whose caller
  is the game, compared on what the game reads, the game-visible memory and the I/O),
  `frames --boundary` and the scenarios.
- **Scenarios** (`tools/xn_scenarios.py`, `xn_rc.py scenarios`): 36: walking and turning in
  all 18 saves, a large town, rain, snow, night, dungeon water above and below, the automap,
  the inventory and paper doll, the character sheet, the spellbook, a cast, the travel map, a
  fight, resting, the options, entering a tavern (the load), the main menu, and the logo VID
  (started by a direct call: no save starts one). Input per frame goes through the game's own
  int 9 handler and the mouse driver, at the same instruction in both machines; modal
  screens sync on the game's screen update or a mouse poll. **1,605 / 1,605 frames
  identical**, continuous lockstep.
- **Coverage** (`tools/xn_cover.py`): the asm's static CFG (6,550 basic blocks), the corpus
  replayed and the scenarios run with the patched Unicorn's coverage map: **68.6%** of blocks
  executed, 90.8% executed or justified (dead, unrolled copies of executed blocks, hardware,
  VESA, fatal paths); 605 blocks in 149 live functions not reached yet.
- **Test shims** (`src/engine_test/`, `config/xngine_dropped.csv`): a canonical function's
  asm interface lives in a shim the tools build in; record tests route the asm entry there
  and excuse only the dropped memory (scratch, patch fields), registers and the CPU
  exceptions; the game reaches canonical functions only through generated boundary routes.
- **The pilot**: vec, mat and math canonical (no pragma, no `_r`, scratch in locals, the
  general products' strides as parameters, checked divisions), with module and function
  documentation and `docs/engine/quirks.md` (Q-SYS-01, Q-MATH-01..06, Q-MAT-01). 803 / 803
  records through shims; 288 / 288 game-called records at the boundary; the whole corpus
  (8,204) with everything routed; `tools/xn_equiv.py`: 33 helpers, 90 million samples (55
  million exhaustive), no difference. One quirk needed the game's own registers:
  `xn_math_isqrt(0)` depends on the caller's ECX (`bsr` of 0), and the game's site 080FFC
  reaches it, so its boundary route is an adapter (`xn_math_isqrt_b`).

## 2026-10-06: phase 7, canonical C

The engine is canonical C, shown equivalent to the asm at its boundary with the game:
docs/xngine_canonical.md has the bar, the boundary, the tools and the proof;
docs/engine/architecture.md has the overview.

- 648 functions, all canonical.
- 8,880 recorded calls pass through test shims; the game's calls 2,819 / 2,819.
- 39 lockstep scenarios, 1,675 / 1,675 frames.
- 140 equivalence specs, 217 million samples.
- 98.1% of reachable blocks exercised.

The engine's data is C too (src/engine_data/, docs/engine/data.md), and the engine links without
FALL.EXE. An SDL port starts from docs/sdl_handoff.md.

