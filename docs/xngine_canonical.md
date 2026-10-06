# XnGine as canonical C: the guide

Phase 6 made all of XnGine readable C (`docs/xngine_readable.md`): 648 functions in
`src/engine/`, each verified against its recorded calls with its asm register interface.
That C still answers to the asm: `#pragma aux` interfaces, `_r` adapters, scratch globals
written because the asm wrote them, patch fields at code addresses, generated code, leftover
registers. This phase turns it into **canonical C**: plain C modules that keep only what the
game can see, documented, with the original bugs kept and catalogued, and shown equivalent
to the asm at the engine's **boundary** with the game.

This guide is for the agents converting the engine group by group, and for readers. The
pilot (vec, mat and math: 62 functions) is done and is the worked example throughout.

**Status (2026-10-05): part 1 done.** The boundary, the yardsticks and the test mechanism
are in place, and the pilot is canonical:

| | |
|---|---|
| The boundary (`config/xngine_boundary.csv`) | 181 entries (174 called by the game, 3 held as pointers, 3 called through pointers, 5 vectors), 15 game functions and 40 service and 13 port forms as exits; memory classified from 36 surveyed scenarios (1,605 frames) and the corpus's writes |
| Lockstep scenarios (`xn_rc.py scenarios`) | 36 scenarios, **1,605 / 1,605 frames identical** at the boundary, continuous (the C machine keeps its own state; 67 s with four workers) and resynced each frame (`--resync`). A deliberate one-unit error in `xn_math_approx_dist2d` and one in `xn_mat_from_angles` fail every frame of three scenarios and the records |
| The game's calls (`xn_rc.py test --boundary`) | all 176 converted entries with records: **2,501 / 2,501** game-called records; the pilot's 15 game-called entries 288 / 288 |
| Records through shims (`xn_rc.py test vec mat math`) | 62 / 62 functions, **803 / 803** records, alone and all at once; the whole corpus with everything routed: 8,204 / 8,204 |
| Pure helpers (`xn_equiv.py`) | 33 specs, **90.1 million samples** (54.6 million exhaustive), no difference |
| Coverage of the asm (`xn_cover.py`) | **4,495 / 6,550 basic blocks (68.6%)** executed, 74.6% of instructions; 90.8% executed or justified (dead code, unrolled copies of executed blocks, absent hardware, VESA, fatal paths); 605 blocks in 149 live functions not reached yet |

The phase's coverage target (95% of reachable blocks executed, the rest justified) is the
conversion groups' to reach: each group adds the records and scenarios its functions need
(workflow step 8). `build/xn_canon/coverage/report.md` lists the gaps by function.

### The boundary in numbers

- **Entries.** The game calls 174 engine functions directly (952 call sites); it holds three
  engine functions as pointers (`xn_timer_tick_callback` for the sound library's timer,
  `xn_draw_image` and `xn_draw_image_transparent`) and calls two engine routines through
  pointers at run time (`09E8AC`: the SOS timer service calls `xn_timer_tick_callback` and
  the VID player's `xn_vid_timer_cb`); the engine installs five vectors (int 9
  `xn_kbd_int9_handler`, 1Ch `xn_joy_timer_isr`, the COM1 IRQ `xn_serial_irq_com1`, int 24h
  `xn_sys_crit_error_handler`, and the DPMI divide-error handler
  `xn_sys_divide_error_handler`). After most calls the game reads only EBP, ESI, EDI (and EAX
  for a result); a few entries are read further (`xn_draw_image`: AH and EAX's upper half;
  `xn_math_yaw_offset_xz`: EBX's upper part; `xn_math_isqrt`: ECX).
- **Exits.** malloc (17 sites), free (15), `_dos_getvect`/`_dos_setvect`, `filelength`,
  `fatal_error`, `exit`, `mem_check_heap`, `disk_resolve_path`, `dpmi_lock_region`/`unlock`,
  the SOS timer (`sound_timer_add`/`remove`) and sample calls; DOS file calls (3Ch-4Fh), DPMI
  (descriptors, DOS memory, real-mode interrupts, locks, the exception vector), the mouse
  (33h), the video BIOS (mode 13h, VESA through DPMI 0300h), the keyboard BIOS, int 2Fh 1680h
  (yield); ports 20h/21h (PIC), 40h/43h (PIT), 60h/61h (keyboard), 201h (joystick), 3C7h-3C9h
  (palette), 3DAh (retrace), 3F8h (COM1).
- **Game-visible memory.** In object 2: 405 KB (the game's own tables placed in object 2, the
  camera, mouse, keyboard, font, clip and screen globals, the sine tables, `xn_render_poly_pool`
  where the game's pick reads results); low memory: VGA and the BIOS data area; allocations:
  the work buffer (`big_buffer`), the back buffer, the world layers, the texture heap (whole:
  the game reads images from it) and the current font. **Engine-private:** 494 KB of object 2
  (all code, with the patch fields; the render pools; scratch), the descriptor table and DOS
  memory, and 1.35 MB of allocations (the texture unpack buffer, the texture-mapper pool,
  engine-only parts of the world layers and the work buffer). 46 KB of object-2 data and three
  allocations (the span reciprocal table, a light table, three fonts) were not written in any
  survey: they are compared.
- Limits: the classification is as good as the surveys' reach (36 scenarios); a chunk is 256
  bytes; a game read of a byte nobody wrote in the window counts as a read of engine data
  (conservative). Replayed DOS services answer from the record, so a record test cannot see a
  canonical C that asks DOS for a different buffer; the scenarios, where DOS runs, can.

## The canonical bar

A canonical module is C a person would write for this engine today, under one constraint:
the game sees exactly what it saw before.

- **Plain prototypes in a public header** (`src/engine/x<subsys>.h`), Watcom's own calling
  convention, no `#pragma aux` on engine functions. Results come back as return values and
  through pointers, statuses as `int`. (The one-instruction inline helpers in `xngine.h` for
  64-bit products and divides stay: Watcom C32 10.0a has no 64-bit integer type. They are
  compiler support, like intrinsics.)
- **No register machinery in `src/engine/`.** No `_r` adapters (they move to test shims), no
  `xn_asmcall`, no `asm_NAME` calls. Internal calls are plain C calls. The only code that
  touches registers is the test shims (`src/engine_test/`), the boundary routes the build
  generates, and a boundary adapter where the game's own registers matter (a quirk,
  below).
- **No asm-only state.** Scratch globals the asm used to pass values between its steps
  become locals or parameters; patch fields become variables or parameters; planted-ret
  counts become loop counts; code generators become the loops they generated. What stays in
  memory is state some other code (the game, or another engine module) reads.
- **No hardware exceptions as control flow.** A divide that the asm let fault (XnGine's
  handler made the result 0) checks first and gives 0 (`xn_s64_div_or0` and siblings in
  `arith.c`; quirk Q-SYS-01).
- **asm only at the hardware boundary**: interrupt entry (the build's ISR stubs), DOS, BIOS,
  DPMI and port I/O (the `xn_int21`... and `xn_inb`... helpers).
- **Documented**: a header comment per module (what it does, its fixed-point formats and
  units, the tables it reads) and one per function (what it computes in the game's terms,
  units, preconditions, the quirks it keeps). See "Documentation standard".
- **The original bugs kept**, each in `docs/engine/quirks.md` and marked in the C
  (`Quirk Q-...`). A behaviour the game cannot observe may be dropped; the catalogue says so
  and why.
- **Equivalent at the boundary**, by the four checks below.

Data at fixed addresses: the engine's tables and globals still live where the asm has them
(`extern` declarations resolved to the loaded game). Moving engine-private state into C
storage is allowed once nothing asm reads it (see the boundary map for what the game reads:
those must stay where the game looks).

### Before and after (the pilot)

`xn_math_isqrt`, readable (phase 6):

```c
/* For v = 0 the asm's `bsr` leaves its register as the caller had it: ecx is that value. */
s32 xn_math_isqrt(s32 v, s32 ecx);
#pragma aux xn_math_isqrt parm [eax] [ecx] value [eax] modify exact [eax edx ebx];
```

canonical (`src/engine/xmath.h`, `math.c`):

```c
/* The integer square root of v by bit-by-bit trial ... isqrt(0) = 0 (the asm's answer for 0
   depends on its caller's ECX: xn_math_isqrt_zero, Q-MATH-02). Four game sites (through
   xn_math_isqrt_b). */
s32 xn_math_isqrt(s32 v);

/* The game's calls of xn_math_isqrt (boundary adapter: the game's registers; Q-MATH-02). */
void xn_math_isqrt_b(xn_regs *r)
{
    r->eax = r->eax != 0 ? xn_math_isqrt(r->eax) : xn_math_isqrt_zero(r->ecx);
}
```

The leftover register is gone from the engine's interface; where the game can see it (one
game site reaches isqrt(0) with ECX = 0), the boundary adapter keeps it, documented as a
quirk; inside the engine, `xn_math_angle_to_point` passes the value its own asm had in ECX.

`xn_mat_from_angles` kept three products in code bytes (`xn_mat_from_angles_tmp`, 0x13719F)
because the asm did; canonical C keeps them in locals, and `config/xngine_dropped.csv` excuses
the bytes the asm wrote there. `xn_mat_mul_fixed` passed its row strides and shift to
`xn_mat_dot_fixed` through globals in code bytes and took the shift in EBP (glue);
canonical:

```c
s32 xn_mat_dot_fixed(s32 n, const s32 *a, const s32 *b, s32 b_stride, u32 shift);
void xn_mat_mul_fixed(s32 *c, const s32 *a, const s32 *b, s32 rows, s32 inner, s32 cols,
                      u32 shift);
```

and the test shim of the dot product's asm entry reads the stride and shift from those
globals, where the asm's callers left them (`src/engine_test/mat_t.c`).

## What "equivalent" means: the boundary

The engine is equivalent when everything the game can observe is the same: what comes back
from each call into the engine, what the engine leaves in memory the game reads, and what it
does to the outside world (the screen, the palette, ports, DOS). Everything else is the
engine's own business. `config/xngine_boundary.csv` (written by `tools/xn_boundary.py`) is
that boundary, row by row:

| kind | what | how it is found |
|---|---|---|
| entry | engine code entered from outside: the game's direct calls (LE fixups from object 1), pointers the game holds and calls (fixups to object-2 functions; object-1 indirect calls traced in the scenarios), the vectors the engine installs (keyboard int 9, the joystick's 1Ch, serial IRQ, int 24h, the DPMI divide-error handler), callbacks (`xn_timer_tick_callback`) | static (`static`), surveys (`survey`) |
| entry `game_reads` | per entry, the registers the game reads after its calls: the union of the game's call sites' liveness (tools/xn_abi.py), with Watcom's rule that the game never reads the argument registers it passed | static |
| exit | the game's functions the engine calls (malloc, free, the SOS timer and sample calls, `_dos_getvect`/`setvect`, `fatal_error`, `exit`...), the `int` services by AH/AX, the ports | static |
| memory | per object-2 item (each named global, unnamed data run, code), low-memory region, and engine allocation (by the global that holds its pointer, in 256-byte chunks): `visible` (compared) or `private` (masked), with the evidence | static references, surveys, the corpus |

**Game-visible memory, found dynamically.** `xn_boundary.py survey` runs every scenario on
the asm with memory hooks. Code is attributed by EIP (object 1 is the game; object 2 and code
run from the heap, the generated code, are the engine). For every byte it keeps: the engine
wrote it (E), the game read it (G), the game read it while its last writer was the engine (a
**flow**, F; object 2's initial data counts as the engine's, and so does what DOS writes for
an engine call), the game read it with no writer in the window (U). The record corpus adds
the bytes each engine call writes. Then:

- an object-2 item is **visible** when the game references it statically (an object-1 fixup
  into it) or a flow was seen, **private** when the engine writes it and no flow was seen
  (code, scratch, pools), **unobserved** otherwise (compared);
- an engine allocation (found by `xn_boundary.py allocs`: a boot through character creation
  with hooks on the engine's malloc and free sites) is classified by 256-byte chunks: a
  chunk the engine writes whose engine-written bytes the game never reads is private; a
  block whose layout follows its contents (the texture cache's heap) is classified whole;
- VGA memory is the screen (visible); the palette goes through ports (compared in the I/O
  log); the descriptor table is the CPU's (private).

Anything not listed is compared. A test resolves the private rows in the machine it compares
(allocation rules read their pointer global there): `xn_boundary.load_masks().resolve(emu)`.

**The argument.** Four checks together:

1. **The game's calls** (`xn_rc.py test --boundary`): every recorded call of a boundary entry
   whose caller is the game (the return address in object 1, a direct call from the safe
   point, or an interrupt) replays with the entry routed as the game reaches it, and matches
   on the registers the game reads, ESP, the game-visible memory written, and the port and
   DOS I/O in order (with the bytes of DOS writes; CPU exceptions left out).
2. **Lockstep scenarios** (`xn_rc.py scenarios`): the game from 18 saves and the screens,
   a frame at a time, asm and C side by side with the same per-frame input: every frame
   matches on the screen, the game-visible memory and the frame's I/O. Continuous by default:
   the C machine keeps its own state from frame to frame. The 36 scenarios (frames each, all
   identical): walking and turning in each of the 18 saves (30 each: nine dungeons and
   palaces, nine towns, cities and a ship), a large town with people (40), rain (30), snow
   (30), a city at night (30), dungeon water below the eye (30) and above it (30), the automap
   (36), the inventory and paper doll (40), the character sheet (24), the spellbook (30),
   casting Shock (40), the travel map (30), weapon swings at a Frost Daedra (40), resting (40),
   the options menu (30), entering a tavern with its load (60), the main menu after boot (30),
   and the Bethesda logo VID played through a direct call of the game's `intro_play_logo`
   (475 frames, to its end: no save starts a movie by itself).
3. **Coverage** (`xn_cover.py`): the record corpus and the scenarios execute nearly all of the
   asm's reachable basic blocks; the rest is listed with the reason it cannot run (dead
   code, absent hardware, fatal paths...). Equivalence shown on inputs that reach the code.
4. **Pure helpers** (`xn_equiv.py`): fixed-point products, square roots, distances, angles:
   exhaustive over small domains, millions of random samples otherwise, asm against C.

Checks 1 and 2 compare only what the game sees; 3 says how much of the engine those
comparisons exercised; 4 covers arithmetic corners no game input reaches. The per-function
record tests (through shims) remain, as the conversion's unit tests.

## Test shims and dropped memory

The mechanism that keeps per-function equivalence while interfaces become plain C:

- **A canonical function has a plain prototype and no pragma.** A function is canonical when
  `src/engine_test/` defines its shim `NAME_r(xn_regs *r)`.
- **The shim** (`src/engine_test/<subsys>_t.c`, 8.3 names) maps the registers of the asm entry
  (its row in `config/xngine_abi.csv`) to the C call and back: inputs from `r->eax`...,
  results into the output registers, CF with `XN_SETFLAG`. A value the asm took from a global
  (a stride the product left for its dot product) the shim reads from that global. Shims are
  compiled into the image by `tools/xn_rc.py` only; nothing in `src/engine/` calls them.
- **The record test routes the asm entry to the shim** (route kind `shim`: the build's
  `pushfd; pushad; call NAME_r; popad; popfd; ret N` stub), so every record of the function
  still replays, compared by the ABI row: outputs against the record, preserved registers
  against their entry values, flags in `flags_out`, all memory written, the I/O.
- **Dropped memory** (`config/xngine_dropped.csv`: `function,kind,start,end,name,reason`):
  what canonical C no longer does on purpose. Kinds:
  - `memory` (preferred addresses `start`-`end`): bytes the asm wrote and the C does not:
    scratch globals, patch fields, leftover-only state. Excused in every record (a function's
    scratch is nobody's input; the boundary map must agree it is private).
  - `register` / `flags` (`name`: the registers or flags): an output the asm left that no
    remaining caller reads; excused in that function's records only.
  - `exceptions` (`name`: the vectors, hex): CPU exceptions the asm raised and the C does
    not (the divide error and the return from XnGine's handler: `00 FD`).
  Everything else stays exact.
- **The game reaches a canonical function only at the boundary.** In the game runs (frames,
  scenarios, play) the build routes a canonical function's asm entry only if it is a
  boundary entry: straight to its C (kind `boundary`) when its plain prototype gives the game
  everything it reads; through a stub that keeps the registers the game reads and the
  prototype may change (`boundary-keep`: `push ebx; call; pop ebx`; it keeps EAX too, for a
  void prototype whose asm keeps the parts of EAX the game reads after the call, as
  `xn_mouse_poll_clamped` and `xn_kbd_flush` do); or to a boundary
  adapter `NAME_b(xn_regs *)` in the module (`boundary-adapter`) when the game's own
  registers change the result (a quirk, like Q-MATH-02). Internal calls are plain C; an asm
  entry no game code reaches has no route outside the tests.

## Workflow per group

Work in your own folder (`XN_RC_SRC`, `XN_RC_OUT`, as in `docs/xngine_readable.md`); your
shims go in `$XN_RC_SRC/test/`.

1. **Read**: the group's rows in `config/xngine_abi.csv`; its boundary rows
   (`tools/xn_boundary.py show NAME`; `grep ',NAME,' config/xngine_boundary.csv`): which of
   its functions the game enters and what the game reads after, which of its memory the game
   sees; `docs/engine/structs.md`, `docs/engine/smc/` (the self-modifying code designs and
   `groups.csv`: groups that must convert together), `docs/engine/quirks.md`; the coverage
   report's lines for the group (`build/xn_canon/coverage/functions.csv`).
2. **Write the canonical module**: plain prototypes and documentation in the header; the C
   without register machinery. For each asm-only thing, decide with the boundary map:
   - scratch, patch fields, pools only the engine reads: locals, parameters, C state;
   - memory the game reads (visible): keep writing it, exactly;
   - a leftover register: if a game site reads it (the entry's `game_reads`) keep it in a
     boundary adapter (and catalogue it); otherwise drop it;
   - a divide that can fault: `xn_s64_div_or0` and siblings;
   - a quirk: keep it, mark it `Quirk Q-<GROUP>-<nn>`, add it to `docs/engine/quirks.md`.
3. **Shims**: `$XN_RC_SRC/test/<subsys>_t.c`, one `NAME_r` per function of the group. Rows in
   your copy of `config/xngine_dropped.csv` for what you dropped, each with its reason.
4. **Callers in other groups**: update their call sites and declarations to your prototypes
   (only those lines). Say which in your report.
5. **Build and test**: `tools/xn_rc.py build` (the route check must stay clean: no pragma on
   a canonical function, a prototype for each entry); `xn_rc.py test GROUP` (each function's
   records through its shim), `test GROUP --all`, `test --corpus` (everything at once);
   `xn_rc.py test --boundary GROUP` (the game's calls).
6. **Scenarios**: `xn_rc.py scenarios` must stay all identical. A difference prints the first
   game-visible bytes; `tools/xn_boundary.py show ADDR` names them.
7. **Pure helpers**: add specs to `tools/xn_equiv.py` and run them.
8. **Coverage**: `tools/xn_cover.py report`; blocks of your group that nothing runs need a
   reason (dead, hardware, fatal), a record (`tools/xn_record.py targets`) or a scenario.
9. **Report**: functions converted; records, `--all`, `--corpus`, `--boundary` and scenario
   results; equivalence samples; dropped rows and quirks added; other groups' files touched.

## Documentation standard

- **Module header** (top of `x<subsys>.h`): what the module does in the game's terms; its
  fixed-point formats and units (2.28, 16.16, 2.14, world units, 2048ths of a turn...); the
  tables and globals it reads and where they live; the quirk family it keeps. The `.c` file
  starts with one line pointing at the header and any implementation notes.
- **Per function** (on its declaration): what it computes, in the game's terms; parameters
  and result with units; preconditions (a count of at least 1...); who calls it when that
  explains it (game sites, "dead: no caller"); the quirks it keeps (`Q-...`).
- **In the code**: comments for what is not obvious from the C: an unsigned shift that is the
  asm's, a quirk (`Quirk Q-...`), a value kept for another module.
- Names come from `config/names.csv`; propose better ones there (a C function's name is its
  link to its asm entry).

## The quirk catalogue

`docs/engine/quirks.md`: one entry per original bug or oddity the canonical C keeps (or
drops), with where it is, the asm that shows it (addresses), what callers get, who can see it
(with evidence), the C that keeps it and the test that checks it, and its status. The file
gives the exact format.

Comparing services: a service call's EAX is compared by AX (`xn_rc.service_ax`). Every
DOS, DPMI, BIOS and mouse service takes its function in AH or AX, so EAX's upper half is
whatever the caller left. What a service returns is compared in the registers it defines
(`tools/xn_services.py`). An agent's own dropped rows can be added with `XN_DROPPED=path[:path]`.

## The tools

| tool | what |
|---|---|
| `tools/xn_boundary.py static / allocs / survey / write / masks / show` | the boundary map (above) |
| `tools/xn_scenarios.py list / show / shots` | the scenario definitions and the lockstep runner; `shots NAME` writes the asm's screens and a montage (to check the input does what it says) |
| `tools/xn_rc.py scenarios [NAME...] [--resync]` | lockstep scenarios with the C, compared at the boundary |
| `tools/xn_rc.py frames SNAP --boundary` | the old frames command through the scenario runner, compared at the boundary |
| `tools/xn_rc.py test --boundary [FUNC...]` | the game's calls of the boundary's entries |
| `tools/xn_rc.py test FUNC/SUBSYS` | records through shims (canonical) or the old routes |
| `tools/xn_cover.py cfg / corpus / scenarios / report` | asm basic-block coverage; a block no input can reach is justified by its reason in the tool's `JUSTIFIED` table (a block range, the reason, why: from a group's analysis or a quirk), listed in `report.md` |
| `tools/xn_equiv.py [FUNC...] [-j 2]` | pure helpers, asm against C, at scale: the pilot's, group A's, B's and E's specs (`--list`); a function's dropped register and flag outputs (`config/xngine_dropped.csv`, `XN_DROPPED`) are not compared, as in the record tests |
| `tools/xn_mkrec.py a / b / e / all [CASE...]` | the groups' crafted records (direct calls with made-up inputs, for coverage), regenerated into the corpus (`build/xngine/records`: `grpa_*`, `b_*`, `group_e_*`); `check [a b e]` replays them on the asm, `--against DIR` compares them with another copy |

The crafted records are part of the corpus (561: A 61, B 401, E 99): regenerate them with
`.venv/bin/python tools/xn_mkrec.py all` (or `a`, `b`, `e`; a case or job name or prefix picks
some; one machine at a time, about three minutes in all), then `tools/xn_mkrec.py check a b e`
(every record replays exactly on the asm). Regenerated, they are identical to the groups' own
(each record's entry state, pages, exit, writes and I/O). A new group's generator adds its
cases there, beside A's, B's and E's, rather than in a folder of its own and `XN_RECORDS`.

Scenario syntax (`tools/xn_scenarios.py`): a snapshot; a prelude, asm only (`poke`, `water`,
`time`, `teleport`, `play`: tools/fallplay.py steps such as `["door", "13"]`, then `script`
and `ticks`); `frames` lockstep frames; per-frame `input` (`FRAME down KEY`, `up KEY`,
`key KEY`, `mouse X,Y,B`, `click X,Y`, `rclick X,Y`, `irqs N`); `irqs` timer interrupts per
frame; `sync` (`safe`: main's per-frame update; `fn:NAME`: an engine function's entry, at its
asm entry in the asm machine and its C entry in the C machine; several joined by `+`);
`call` (a direct call started in both machines at the first frame; the scenario ends when
it returns; always continuous). Modal screens run their own loops: they sync on the game's
screen update or a mouse poll (`MODAL`), where their waits for a button's release also poll.
Input reaches both machines at the sync point, each interrupt run to completion before the
next. A frame that polls a port a million times without reaching the sync point (a wait for
input that only comes at the next frame) is reported as not finishing: give that wait a
sync point.

## The rest: a proposed split and order

The other 657 functions (586 of them C; the rest are run-time blocks and data inside them),
in 38 subsystems, in five groups (sizes: the asm's instructions / functions; game entries:
the boundary's). A group owns its modules' `.c`, `.h` and shims; calls into another group go
to that group's C by its current prototype (legacy or canonical: Watcom honours either
declaration), so groups can work side by side; when a group changes a prototype it updates
its callers' call sites (as the pilot did in `collide.c` and `colmodel.c`). What couples
groups is self-modifying code: a patch field written in one module and read in another must
change on both sides at once (`docs/engine/smc/groups.csv`), so those modules share a group.

| Group | Subsystems | Size | Game entries | What makes it hard |
|---|---|---|---|---|
| A. System and input | sys, dos, str, mem, rand, timer, bits, spell, input, kbd, mouse, joy, serial, helmet, anim, and the four unnamed | 2,750 / 228 | 68 | the boundary's hardware edge: DOS, DPMI, BIOS and port calls stay as the `xn_int*`/`xn_in*` helpers; five interrupt and exception handlers (int 9, the joystick's 1Ch, the serial IRQ, int 24h, the divide error) keep build-made entry stubs; the helmet drivers' timeout loops; the divide-error handler becomes unnecessary once nothing divides into it (Q-SYS-01) |
| B. Screen and 2D | gfx, pal, vid, draw, img, font | 13,100 / 133 | 44 | draw's unrolled and self-patching blits (smc DRAW-*: 9,400 instructions become short loops), the scaled-image row compiler (code generated into `big_buffer`: game-visible chunks, see the boundary rows), VESA (no VESA in the emulator: its blocks are unexecuted), the VID player's timing (scenario `vid`) |
| C. Rasteriser and the frame | span, shade, tmap, light, render | 7,400 / 89 | 15 | almost all self-modifying: render's setups patch span and shade fields (smc RENDER-SETUP), `xn_render_frame`'s run-time blocks and stack unwinding (RENDER-FRAME), the light-shader and texture-mapper generators and their pools (`xn_tmap_pool_block`: private), planted rets in spans |
| D. 3D objects | poly, flat, model, cam, tex | 4,800 / 96 | 18 | the projection and clipping with flag outputs (glue today), the texture cache's heap (game-visible: classified whole), calls into C's spans (poly) and tmap (tex) |
| E. World and collision | collide, world, terrain, sky, water | 8,400 / 111 | 20 | collision's plane and sphere tests (the shared `xn_collide_*` vectors: engine state other tests read), world streaming through DOS, terrain cells with patched fields, water's unrolled spans (WATER-*), the rain streaks' planted ret (SKY-RAIN) |

Order: **A, B and E first, side by side** (A and B are mostly leaves the game calls; E needs
only the pilot, done, and A's DOS and random calls, which it can call in their current form).
**Then C and D together**, two agents coordinating the patch fields between render, poly,
span and tex (one owner per field; `test --all` over both groups' functions before either
reports). Each group runs its tests with two workers (five agents at once fit the memory
budget; never two runs of six workers). After all five: the boundary routes are the only
routes in the game runs; `src/engine/` has no `#pragma aux` but the compiler-support helpers;
the asm can go.

Cross-group calls today (asm call sites; most of the rest are fatal paths that restore the
keyboard, joystick and video): collide -> mat 29, vec 5; model -> mat 12, cam 3; world -> dos
20, rand 10; terrain -> poly 6, mat 4, tex 4; flat -> poly 9, mat 4; render -> light 5, flat
3; sky -> rand 11; water -> rand 4, poly 3; tex -> tmap 5, dos 4; helmet -> serial 12; vec ->
math 6; mat -> math 3.
