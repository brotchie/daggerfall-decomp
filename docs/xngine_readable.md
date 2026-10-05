# XnGine as readable C: the guide for the conversion

This is the working guide for turning XnGine (object 2 of `FALL.EXE`) into readable C, one
subsystem at a time. The pilot (vec, mat and math: 62 functions in `src/engine/`) was done
this way and is the worked example. The log and the numbers are in `docs/xngine.md`, "phase
6: readable C".

**Status (2026-10-05): done.** All of XnGine is readable C in `src/engine/`: 648 of the 719
functions run as C, and the other 71 are blocks inside other functions or data. Every record
of the corpus passes with all of it routed at once, and lockstep frames of the game match the
asm in all of memory. The guide stays as the reference for how the C runs, how its interfaces
are declared and tested, and how to change it.

Read these before starting a subsystem:
- `docs/engine/`: the engine's data structures. `src/engine/xnstruct.h` has
  64 structs with compile-checked offsets; `structs.md` has the evidence. Their field and
  global names are merged into `config/names.csv`.
- `docs/engine/smc/`: a design for every function that patches code, plants `ret`s,
  runs unrolled bodies or generates code. `index.md` gives the rules, there is one `.md`
  per subsystem, and `groups.csv` lists the groups that must convert together and the
  records that test them. `patch_fields.csv` names all 292 patch fields.
- `docs/xngine_map.md`: the engine map. `build/names/scratch/xngine/part_?.md`: the naming
  notes.

## The bar

A readable function is C a person would write:
- named, typed parameters and locals;
- structured control flow (`if`, loops), no `goto` except for a shared failure exit;
- named globals and struct fields instead of addresses and offsets;
- loops where the asm unrolled;
- a comment saying what it computes, in the terms of the game, on its declaration in the
  subsystem's header (the pilot's way) or above its definition.

It is also exact. Every recorded call of the function must replay with:
- the same memory writes, byte for byte, except below the stack pointer at return (that
  stack is dead);
- the same registers, except those the ABI row says no caller reads (its clobbers);
- the same flags, for the flags the row says callers read (`flags_out`);
- the same port I/O and interrupts, in the same order, including divide errors.

So the C keeps whatever the callers can see. That includes scratch globals the asm writes,
even when nothing reads them back, and the quirks: an unmasked table index, a `bsr` of zero
that leaves its register, a stored pointer where a value was meant. When you keep one,
say so in a comment.

Before, the literal C (`tools/xn_c.py show 0xC7FD9`):

```c
void xn_000C7FD9(void)
{
    u32 fa, fb, fr, fc, ea, t, t2;

L_0C7FD9:
    fa = R.eax; fb = R.ebx;                                      /* 0C7FD9: sub eax, ebx */
    fr = fa - fb;
    R.eax = fr;
    if (!((s32)fr < 0)) goto L_0C7FDF;                           /* 0C7FDB: jns 0xc7fdf */
    R.eax = 0 - R.eax;                                           /* 0C7FDD: neg eax */
L_0C7FDF:
    ...
    R.ebx >>= 1;                                                 /* 0C7FED: shr ebx, 1 */
    R.eax += R.edx;                                              /* 0C7FEF: add eax, edx */
    ...
    RET(0);
}
```

After (`src/engine/math.c`, with the comment from `xmath.h`):

```c
/* An approximate ground distance between (x1, z1) and (x2, z2): the larger axis distance plus
   half the smaller (an octagon). 46 game sites. */
s32 xn_math_approx_dist2d(s32 x1, s32 z1, s32 x2, s32 z2)
{
    s32 dx = x1 - x2;
    s32 dz = z1 - z2;
    s32 small;

    if (dx < 0)
        dx = -dx;
    if (dz < 0)
        dz = -dz;
    small = dx < dz ? dx : dz;
    return dx + dz - (s32)((u32)small >> 1);
}
```

The `(u32)` is the asm's `shr`: an unsigned halving of a value that is negative only for the
most negative input. Keep such details; they are what makes it exact.

## How readable C runs

- `tools/xn_rc.py build` compiles `src/engine/*.c` and `*.asm` with the real Watcom C32
  10.0a (`-mf -4r -s -zl -zld -ox`) and links them at 0x11000000, a region the emulator maps
  outside the memory records compare.
- The C runs inside the game, on the game's own stack. There is no register file `R` and
  no thunk. The asm stays loaded: a code hook at a converted function's asm entry sends its
  asm callers to the C.
- The C calls other C functions directly, by name. It calls asm by its address: the
  linker resolves every name the C does not define to the loaded game.
  - Functions and globals resolve by their `config/names.csv` names, then by
    `build/xn_readable/names.csv` (merged proposals), `$XN_RC_OUT/names.csv` (yours) and
    the patch fields' proposed names in `docs/engine/smc/patch_fields.csv`.
  - `xn_data_15A3C0`, `xn_code_...`, `D_...` and `func_...` resolve to any address.
  - `asm_NAME` is always the asm entry, even once NAME is C.
- Data stays where the asm has it, in object 2 and the game's memory. Declare it `extern`
  under its name. Never keep engine state in C statics: the C region is not compared,
  and the asm cannot see it.

## Declaring an interface

The interface comes from the function's row in `config/xngine_abi.csv`
(`tools/xn_rc.py abi FUNC` prints it). Its columns:
- `inputs`: the registers live at entry. `stack_args`: argument bytes above the return
  address.
- `outputs`: the registers some caller reads after the return that the function may
  change.
- `flags_out`: flags a caller reads after the return (CF as a status in 85 functions).
- `clobber`: the registers no caller reads after the return. The C may change these; it
  must keep every other register.
- `ret_pop`: the N of `ret N`.
- `convention` and `callers` (known, partial or unknown); `notes` say why.

Registers are tracked in parts: `al`, `ah` and `ax` are parts of `eax`, and `eax.u` is the
upper half. A row can say a caller reads `al` only.

Pick the declaration from the row:

1. **`watcom`: a plain prototype, no pragma.** Watcom's convention passes arguments in EAX,
   EDX, EBX and ECX, then on the stack (the callee pops it), and returns in EAX. Watcom
   10.0a does not keep EAX or the argument registers; it keeps the rest. (The brief said
   "every register except EAX". The game's own code shows otherwise: a two-argument
   function never saves EDX.)

   ```c
   s32 xn_math_angle_to_point(s32 x1, s32 z1, s32 x2, s32 z2);
   ```

2. **`reg` with at most one output: a pragma.** Name the registers, and write
   `modify exact` with the clobbers and the value register:

   ```c
   s32 xn_math_isqrt(s32 v, s32 ecx);
   #pragma aux xn_math_isqrt parm [eax] [ecx] value [eax] modify exact [eax edx ebx];
   ```

   - Without `exact`, Watcom changes the parameter registers freely.
   - Watcom's code never keeps EAX, with or without `exact`. When the row says callers
     need EAX kept (it is not an output or a clobber), list `eax` in `modify exact` anyway.
     The build then routes the asm entry through a stub that keeps it:
     `push eax; call C; pop eax; ret N` (route kind `keep-eax`). Such a function cannot also
     take stack arguments; use glue (3).
   - `parm routine [...]` when the function pops its stack arguments (`ret_pop` =
     `stack_args`), `parm caller [...]` when it does not.
   - `value [al]` with a `u8` result when the output is `al`.
   - EBP cannot be a parameter or in `modify` (Watcom rejects it): use glue.

3. **Several outputs, flags out, or EBP in: a natural function and glue.** The C function
   takes and returns what is natural: pointers to results, an `int` for success. A glue
   function `NAME_r(xn_regs *r)` unpacks the asm caller's registers into a call of it and
   packs the results back. The build routes the asm entry through a stub,
   `pushfd; pushad; mov eax, esp; call NAME_r; popad; popfd; ret N`. Registers the glue
   does not set come back as they were, and so do the flags it leaves alone.

   ```c
   int xn_vec_scale_unit14(xn_vec3 *v, s32 dist);          /* natural */
   void xn_vec_scale_unit14_r(xn_regs *r);                 /* asm interface */

   void xn_vec_scale_unit14_r(xn_regs *r)
   {
       xn_vec3 v;
       int ok;

       regs_to_vec(r, &v);
       ok = xn_vec_scale_unit14(&v, r->ecx);
       vec_to_regs(&v, r);
       XN_SETFLAG(r, XN_CF, !ok);
   }
   ```

   `XN_STACK_ARG(r, k)` reads the k-th stack argument. The glue is temporary: once all
   the callers are C, they call the natural function and the glue goes away.

`tools/xn_rc.py route` (also run by `build`) checks every route against its row. It
reports a direct route whose row needs glue, and a declaration that may change a register
callers read. Its `note` lines report a row input the C does not take. That is fine when
the C provably does not need it; say why in a comment. For example, `xn_math_isqrt64`'s ECX
input is only the asm's `bsr` of a zero value, which does not change the result.

`tools/xn_rc.py skel SUBSYS` writes the declarations from the rows. Check them; they are a
start.

## Calling asm from the C

- One output or none: declare the asm function with the pragma of its row, under its
  name. List its outputs and clobbers in `modify exact`: everything it may change. The call
  lands on its asm entry, or on its C once that is routed.
- Several outputs, or flags out: fill an `xn_regs` and call `xn_asmcall(asm_NAME, &r)`
  (declare `extern void asm_NAME(void);`). It loads EAX to EBP from `r`, calls, and stores
  them back with the flags (`r.eflags & XN_CF`).
- A converted function: call its natural C function by name.

## Workflow, per subsystem

1. `tools/xn_rc.py skel SUBSYS`: a header `src/engine/x<subsys>.h` and `<subsys>.c`. Each
   function body sits under `#if 0`, with its asm (names applied), its evidence and its row
   in comments. If the files exist, the skeleton goes to `build/xn_readable/skel/SUBSYS/`.
2. Read the material: `tools/xn_rc.py asm FUNC` (the asm with names), `tools/xn_c.py show
   VA` (the literal C), names.csv, the structs and smc designs, the map.
3. Write the functions. Take each out of `#if 0` as it is done.
4. `tools/xn_rc.py build`, then `tools/xn_rc.py test FUNC|SUBSYS ...`: each function's
   records, with that function alone routed to C (seconds).
5. `tools/xn_rc.py test SUBSYS --all`: the same records, with every converted function
   routed at once.
6. `tools/xn_rc.py diff SUBSYS`: made-up entry states. For functions with no records it is
   the only test; it is worth running for all.
7. `tools/xn_rc.py test --corpus`: every one of the 8,204 records, with every converted
   function routed, each compared by its own function's row (10 minutes with 6 workers).
   This tests your C inside its callers, asm and C.
8. `tools/xn_rc.py play SNAP --ticks 300 --compare`: the game from a snapshot, asm and
   then C, screens compared pixel for pixel.
9. `tools/xn_rc.py report`: `build/xngine/rc_report.csv`, per function: converted (and
   how), records passed, all routed, the whole corpus, diff, and the clobber and input
   tests.

The test's differences name the record and what differed:
- `ebx 00000294 != entry 011B541C (preserved)`: the C changed a register its callers read.
  Check the declaration (`modify exact`; EAX).
- `writes: N addresses differ`: the memory differs.
- `port I/O or interrupts differ`: the service calls or divide errors differ in number or
  order.

`-v` prints every failing record. A failing differential state is saved in
`build/xn_readable/fail/`.

Workers: `-j N` (default: what `fallemu.workers()` allows; take 6 when other agents run
emulators). The parallel commands start `tools/memwatch.py` themselves.

## Hard cases

**Self-modifying code.** Follow the design in `docs/engine/smc/`.
- A patch field becomes an extern at the field's address, under its name in
  `patch_fields.csv`. The setup routine writes it as a variable, and the reader reads it.
  While any asm still executes the patched instruction, the C must write the same bytes to
  the same address.
- The groups in `groups.csv` must be routed together: test them with
  `test --all` on the group's functions.

**Planted `ret`s and unrolled bodies.** The readable form is a loop with the count the
planter computes (see the smc designs and their `xn_planted_count` adapter). The planted
byte and its restore are memory writes: keep them while asm can run the body.

**Templates and generated code.** The template bytes stay machine code, as an extern byte
array at the template's address. The generator becomes C that copies and patches them,
byte for byte (the records compare the pool). Calls into generated code go through a
function pointer with a pragma.

**Flags out.** Glue: the natural function returns a status, and the glue sets the flag with
`XN_SETFLAG(r, XN_CF, ...)`. Six functions return ZF, and two return ZF, SF and OF from a
comparison; the glue sets each.

**Interrupt and exception handlers** (`convention` interrupt) end in `iret` or `retf`. The
route stubs end in `ret`, so leave handlers in asm until the end. Then they need a stub of
their own. The divide-error handler's 30 blocks belong to it.

**64-bit arithmetic.** Watcom C32 10.0a has no 64-bit integers. `xngine.h` has
one-instruction inline helpers:
- `xn_fixmul28` and `xn_fixmul28r` (rounded): 2.28 products;
- `xn_mulhi`: the high dword of a product;
- `xn_mulshr`, `xn_muldiv`;
- `xn_s64`, with `_set`, `_mul`, `_mac`, `_msub`, `_addu`, `_shl`, `_shr`, `_div` and
  `_divrem`;
- `xn_u64_mul` and `xn_u64_div`.

Write the asm's rounding, such as `add eax, 08000000h; adc edx, 0` before a `shrd`, as
it is.

**Divides.** A divide that overflows raises a divide error. XnGine's handler steps over the
instruction, makes EAX and EDX 0, and the emulator logs the exception. Divide where the asm
divides, with the helpers: they use a register operand, which the handler steps over. Never
use C's `/` where a divide can fail. A dead result can matter: `xn_math_exp_series` keeps a
division only for its divide error.

**Port I/O and int calls.** `xn_inb`/`xn_outb` (and `w`), and `xn_int21(&r)` and its
siblings (10h, 15h, 16h, 21h, 2Fh, 31h, 33h) with an `xn_regs`. The I/O log is compared in
order, with the value of AX at each `int`.

**Bit scans.** `bsr`/`bsf` of zero leave the destination as it was, and code depends on it
(`xn_math_isqrt(0)`). Use `xn_bsr(v, old)`. The ABI counts the destination as an input.

**Segment registers and DF.** The C assumes ES = DS and DF = 0, as Watcom does. A
function that changes ES must keep it in asm, or put it back before calling C.

**Scratch globals.** The asm keeps intermediate values in globals, often in code bytes next
to the function (`xn_mat_from_angles_tmp`, `xn_math_tri_edges`). Write them as the asm
does. Declare them with a struct type when that makes the C clearer, and propose names in
`$XN_RC_OUT/names.csv`.

**Made-up states.** A differential state can give a loop count of millions and pointers into
the dead stack. The C's own stack frame lives there, so a function that reads it can
differ for that reason alone (`xn_mat_dot_fixed` has one such state of 7). Look at the
saved state before chasing it.

## When the ABI looks wrong

`config/xngine_abi.csv` comes from `tools/xn_abi.py analyze` (30 s). It is
interprocedural liveness over objects 1 and 2, and it is cross-checked three ways:
- `xn_abi.py check`: every record's preserved registers come back with their entry values
  (static).
- `xn_abi.py clobber --all`: every function's clobbers, and the flags it does not output,
  are scrambled at its return. Every record must still replay, compared by its own row.
- `xn_abi.py inputs`: every register that is not an input is scrambled at entry. The
  records must still replay.

Signs it is wrong for your function:
- outputs no caller could want;
- an input the function never reads;
- a test failure in another function's record after you routed yours.

What to do:
1. `tools/xn_abi.py show FUNC` prints the row, each call site with what is live after it,
   and the liveness at every instruction. Follow a register back to the site that keeps
   it live.
2. Conservative rows are expected:
   - a function with no callers (143, mostly dead code): every register it changes is an
     output;
   - an address that escapes (19).
   For dead code, choose the natural interface and record it as an override
   (va, field, value, reason): in your `$XN_ABI_OVERRIDES` file, which the coordinator
   merges into `config/xngine_abi_override.csv`. Pair each `outputs` override with the
   `clobber` one. Then run `tools/xn_abi.py overrides` (seconds). The pilot has 7 such
   overrides.
3. If the analysis is wrong for a live function, give the coordinator the function, the
   call site and the evidence. Do not edit the CSV by hand, and do not override a live
   function without a dynamic test that backs it (`xn_abi.py clobber FUNC`,
   `xn_abi.py inputs FUNC`).

## The split of the rest

Six groups. Sizes are instructions (`translate.json`) and functions. Subsystems that patch
one another's code are in the same group (see `smc/groups.csv`).

| Group | Subsystems | Size | What makes it hard |
|---|---|---|---|
| 1. 2D drawing | draw, img, font | 11,047 / 75 | 9,362 of draw's instructions are in 14 unrolled or self-modifying functions (smc DRAW-*), which become short loops; the scaled-image row compiler generates code into `big_buffer` |
| 2. Rasteriser | span, shade, tmap, light | 7,325 / 66 | almost all self-modifying: span routines with planted `ret`s and patched operands, fog spans, the light-shader and texture-mapper generators and their pools (byte-exact) |
| 3. 3D pipeline | poly, render, flat, model, cam, tex | 5,500 / 119 | render's span setups patch group 2's fields (convert or test them together); projection and clipping with flag outputs; the texture cache's heap |
| 4. World and collision | collide, world, terrain, sky | 5,696 / 105 | collision's matrix and plane code (the pilot's mat functions help); terrain cells with a few patched fields; world streaming with DOS reads |
| 5. Water and screen | water, gfx, vid, pal | 4,946 / 64 | water's unrolled spans; VESA bank switching through a pointer from the BIOS; the video player's decoder and timing |
| 6. System and input | dos, sys, str, mem, rand, timer, serial, helmet, anim, mouse, kbd, joy, the rest | 2,916 / 228 | many small functions over DOS, DPMI and BIOS calls and ports; the interrupt handlers (keyboard, joystick timer, serial IRQ, critical error, divide error) need `iret` stubs; the helmet drivers' long timeout loops |

Coordinate across groups where a call or a patch crosses them. Functions of another group
that you call stay asm until that group is done: call them through their asm entries.

## Rules for the conversion agents

- Work in your own folder (say `build/xn_readable/agent_draw/`). Point the tools at it:

  ```sh
  export XN_RC_SRC=build/xn_readable/agent_draw/src   # your .c and .h, added to src/engine's
  export XN_RC_OUT=build/xn_readable/agent_draw/out   # your image, results, report, names.csv
  export XN_ABI=build/xn_readable/agent_draw/abi.csv  # your copy of the ABI table
  export XN_ABI_OVERRIDES=build/xn_readable/agent_draw/override.csv
  tools/xn_abi.py overrides                           # your table: the analysis + overrides
  tools/xn_rc.py skel draw --out $XN_RC_SRC
  ```

  `src/engine/` holds only merged, passing code. The coordinator copies your files in,
  merges your overrides into `config/xngine_abi_override.csv` and your names into
  `build/xn_readable/names.csv` (later `config/names.csv`).
- Your files are your subsystems' `<subsys>.c` and `x<subsys>.h`. Everything else in
  `src/engine/` is read-only to you. `xngine.h` and `glue.asm` belong to the coordinator:
  ask for new helpers.
- Propose names (`kind,address,name,confidence,evidence`) in `$XN_RC_OUT/names.csv`. The
  build resolves them at once.
- Header names are 8.3, lower case: `x<subsys>.h`, at most 8 characters before the dot.
  Include `engine_structs.h` as an 8.3 copy (for example `xnstruct.h`), after
  `xngine.h`.
- Never run git, `tools/build_fall.py` or the full corpus tests while another agent's
  workers run, unless the coordinator says so. Keep scratch work in your own folder.
- Keep `.venv/bin/python tools/xn_c.py test 0x157620` passing if you touch the shared
  tools.
- Report, per subsystem:
  - functions converted, and by which route (direct, keep-eax, glue);
  - records passed, `--all`, `--corpus` and diff results;
  - the overrides you added and why;
  - what is left in asm and why.
