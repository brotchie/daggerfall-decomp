# XnGine's self-modifying, unrolled and generated code: readable-C designs

This folder is the groundwork for converting the self-modifying parts of XnGine (FALL.EXE
object 2) to readable C. It covers every function that uses a patched operand, a planted
`ret`, an unrolled body, a computed entry, or a code template, with:
- what it computes;
- the mechanism;
- the readable design and a C sketch;
- the conversion group and the records that test it;
- the exactness traps;
- where readable C cannot match the records.

| File | Groups |
|---|---|
| [span.md](span.md) | SPAN-SOLID-LIT, SPAN-TEX, SPAN-TEX-SHADED, SPAN-TEX-LIT, SPAN-TEX64, SPAN-TEX64-SHADED, SPAN-FLAT-TRANSPARENT/-SHADED/-FOGGED/-TRANSLUCENT |
| [tmap.md](tmap.md) | TMAP: the texture-mapper generator |
| [light.md](light.md) | LIGHT-SHADERS (templates, generators, template writers), LIGHT-SETUP |
| [shade.md](shade.md) | SHADE-FOG-SETUP, SHADE-FOG |
| [water.md](water.md) | WATER-DRAW, WATER-SPAN |
| [draw.md](draw.md) | DRAW-SCALED (generated row code), DRAW-TRANSPARENT, DRAW-SHADED-ROW, DRAW-OVERLAY, DRAW-CHECKER, DRAW-REMAP320, DRAW-PATTERN8 |
| [poly.md](poly.md) | POLY-PROJECT, POLY-RASTER, POLY-GRAD, POLY-SETUP-TEX |
| [render.md](render.md) | RENDER-SETUP (the cross-function field writers), RENDER-FRAME |
| [terrain.md](terrain.md) | TERRAIN-GRID, TERRAIN-CELLS, TERRAIN-AXES |
| [model.md](model.md) | MODEL-DRAW, MODEL-FACES-CLEAR, MODEL-VERTS, MODEL-LIGHTS, MODEL-SCALE |
| [flat.md](flat.md) | FLAT-WRITERS, FLAT-GRAD, FLAT-RASTER, FLAT-EMIT, FLAT-LIGHT |
| [sky.md](sky.md), [font.md](font.md), [gfx.md](gfx.md) | SKY-RAIN, FONT-GLYPH, GFX-COPYCLEAR-UNROLLED, GFX-VESA-BLIT |
| `groups.csv` | per function: group, mechanism, must_convert_with, constraint, records |
| `patch_fields.csv` | all 292 patch fields: reader, writers, width, proposed C name and type |
| `scratch/` | the census (`census.py`), asm dumps (`asm.py --fold VA`), table dumps (`tables.py`), the planted-`ret` check (`retcheck.py`), the CSV writer (`emit.py`) |

## The census

**104 functions in 47 groups.**
- They hold 22,792 instructions, 18,434 of them in unrolled runs.
- 5 functions are not code or only data: the 3 shader templates, the tmap template and
  157E02.
- 13 are group members with no mechanism of their own (wrappers, setups that fall into a
  routine, frame-skip partners).

| Mechanism | Count | Where |
|---|---|---|
| Patch fields | **292 fields, 337 writes**:<br>181 placeholder immediates (204 writes),<br>62 operand addresses (63),<br>32 template fields written through a register,<br>2 operands (a shift count, the screen width),<br>15 opcode bytes (36 writes) | read by 58 functions, written by 47. 110 writes are a function patching itself; 227 patch another function |
| Planted `ret` | **15 unrolled bodies** in object 2:<br>C83F6, the C9D89 body, 12F9E0, 140A28, 14501C, 14D320, 155880, 155C00, 156320, 156D40, 157100, 157760, 157960, 157CA0, 157FA0;<br>plus every **tmap copy in the heap** (planted by 156A94) | 18 planter functions. 13 of the bodies (counting the tmap copies) get a constant opcode back; 3 get their saved bytes back (14501C, C83F6: a word; the rain streak: a byte) |
| Fixed unrolled loops | 6 whole functions (CB552, CD53C, C9F4F, CD39B, 143BB0, 13E744), the 8/16-px blocks of 11 span routines, the tmap template's 8 pairs, the 4-corner copy in 13EFD7 | no code writes |
| Computed entries | **none into an unrolled body.** Every body is entered at its first instruction, and the count is set only by the planted `ret`. Two near-cases: CD53C enters its 2-row loop at the second row when the row count is odd; C9D19 jumps into C9D63's body after pushing a return address the planted `ret` returns to | |
| Code templates and generators | 3 generators:<br>the scaled-image row compiler C094A (into `big_buffer`);<br>the light-shader builders 15BCF6/15BD78/15BE4D (templates 136C30/136C84/136D10, into the `big_buffer` shader pool);<br>the texture-mapper compiler 15C274 and rebase 15C2DC (template 15C300, into the 768-slot heap pool) | the generated code is called by C0700, 155920/156A94, and 156A94 |

`scratch/retcheck.py` checks that every planted `ret` lands on a step's first byte and that
the restored constant equals the original byte, for every n the planters can produce. It
holds for all 15 bodies and the tmap copies. The only exceptions are unreachable n:
- fog n >= 640 and water n >= 641, which run past their offset tables;
- 140A28 with 1024 vertices, where the restore would overwrite the body's own `ret`.

## Design rules

The subsystem files refer to these by name.

### PF: a patch field is a variable at the field's address

Every operand the code rewrites (`patch_fields.csv`) becomes an `extern` with the operand's
width and type, resolved by the linker to the field's address (the infra's design 3):
- `s32` for immediates;
- a pointer type for address operands (`mov eax, [edi + disp32]`, `mul dword ptr [addr]`);
- **`u8` for the four byte fields:**
  - 1559B9 (`mov al, imm8`);
  - C08BE (`mov bh, imm8`);
  - 12DD02 (`shl eax, imm8`);
  - 15BBB9 (`shr edx, imm8`).

  A dword store there would overwrite the next instruction.

Writers assign the variable and readers read it, whichever side is still asm. So **patched
operands never force two functions to be converted together.** The bytes land where the asm
expects them, and the C reads what the asm wrote. `patch_fields.csv` proposes a name for every
field. The names follow the functions' words (`xn_span_tex8_end`, `xn_tgrid_row_x`, ...).

Where one writer stores the same value into several fields (the 25 1/z-table fields, the
view-window fields, the fog's ten fields), the writer must store all of them, because the
records see every one. A reader may read any copy: they are equal in every reachable state,
since their only writer writes them together. Reading the copy the asm reads is the safe
default.

### KEEP: self-patched constants

When a function patches its own operand only to use it later in the same call (loop ends,
steps, saved registers), compute it in a local. Then store the field once, at the point the
asm stores it:

```c
#define XN_KEEP(field, v)   ((field) = (v))   /* only for the records: the asm keeps v in its code */
```

The records compare every byte that differs after a call, and the field is in a code page, so
the store is needed for exactness. One store is enough even where the asm stores three times
(the span routines' `mov [end], ebp` / `nop` / `mov [end], ebp`, a prefetch-queue flush on a
real 486). Only the final value is compared.
- When the field is the loop's real state (the terrain walker, the rasterizers' ring
  pointers, the flat walker's row address), use the field itself as the variable: no epilogue
  to get wrong.
- When another function reads it (140497 is read as data by 140606; 15526F by 1550E0; 155275 by
  15526C), it is shared state: always the field.

### RET: an unrolled body stopped by a planted `ret` is a loop with a count

The body becomes a `static` C function (`tex_run`, `fog_run`, ...) that takes n, and the
planter calls it with n. The C writes no code bytes: the asm's plant and restore leave the
code as it was (checked), so the records see no difference.
- **While a body is not converted** and a C planter calls it, the C must plant and restore
  exactly as the asm does: write `C3` at `body + offset[n]`, call the asm body through the
  register-file glue, then write back the asm's restore byte or saved word.
- **To test a converted body against its own records** (the bodies have 12-29 records each,
  recorded at entry with the `ret` already planted), route the body's asm entry through an
  adapter that recovers n from the planted byte:

  ```c
  /* the count a planted ret gives a body of `max` steps of `stride` bytes (or `max` if none) */
  int xn_planted_count(const u8 *body, int stride, int max)
  {
      int n;
      for (n = 0; n < max; n++)
          if (body[n * stride] == 0xC3)
              return n;
      return max;
  }
  ```

  Then it calls the C body with the asm's registers. This is honest: the planted `ret` *is*
  the count. With the adapter, a body and its planter can be converted in either order. Without
  it, convert them as a pair and leave the asm body unrouted (its records then test nothing new).
- The tmap copies and the rain streak need their own forms (tmap.md, sky.md).

### UNR: fixed unrolled code

A `for` loop. Keep the pixel order (the water reads pixels it has just written). Keep the
access width only where it is visible: it never is for these byte writes, because the records
compare bytes. Keep the loop-control quirks (`dec ecx; jne` with a u32 counter, the parity
entry of CD53C).

### GEN: generated code

Code generation is behaviour: the template's in-place patches are object-2 writes, and the
copies are heap or `big_buffer` writes. So the generators stay byte-exact generators (copy
the template bytes, patch the fields by offset, or emit the bytes).
- **Option A** (first): the code that calls the generated code calls it as machine code,
  through the infra's register-file glue (`xn_asmcall(copy, &regs)`). This is exact.
- **Option B** (the readable and native-port form): interpret it in C. Read the copy's
  parameters from its patch points (the copy *is* the per-texture or per-polygon parameter
  record), or, for the scaled-image row, from a plan built in the same call. This is also
  exact, as long as the generator still writes the bytes.

### DIV: divides that can fault

XnGine relies on its divide-error handler (149FC8: EAX = EDX = 0, step over the instruction).
`xn_light_setup_poly` divides by zero about ten times a frame. A readable divide that can fault
must be a one-instruction pragma with a **register** divisor (2-byte `div`/`idiv`, which the
handler steps over correctly), so the engine's own handler runs as in the records:
- xngine.h's `xn_muldiv`, `xn_s64_div` and `xn_u64_div` qualify;
- add `xn_udiv64(hi, lo, d)` (`div ecx` on edx:eax).

Never let the compiler emit a memory-operand or SIB-form divide where a fault is possible: the
handler mis-steps SIB forms.

### HOLD: no C-side state

Every record starts from a snapshot of object-2 memory. So the C must not cache anything
derived from object-2 state in its own data (the C region is not compared and not restored).
The patch fields, the generated copies and the pools are the only state. That rules out
"remember the shader's parameters in a C table". A plan built and used within one call (the
scaled row) is fine.

### Helpers to add to xngine.h

| Helper | Instruction | Used by |
|---|---|---|
| `xn_umulhi(a, b)` | `mul edx` (high dword, unsigned) | light shaders (option B) |
| `xn_udiv64(hi, lo, d)` | `div ecx` | fog, projectors, render init |
| `xn_bswap(x)` | `bswap eax` | tmap option B |
| `XN_KEEP`, `XN_PUT32`, `xn_planted_count` | - | above |
| `XN_ASM(name)` | the asm entry address of a function (for stored routine pointers and the sentinel test) | span setups, flat light, render frame |

### Provisional structs used by the sketches

Align them with `src/engine/xnstruct.h` when it lands.

```c
typedef struct xn_poly {        /* 100 bytes from xn_render_poly_pool; flats reuse it */
    void *face;                 /* +00 */
    void *owner;                /* +04 model handle (+04 its light list) */
    s32 falloff_1;              /* +08 shader falloffs (1-3 lights); flats: u step per x */
    s32 falloff_2;              /* +0C flats: v step */
    s32 falloff_3;              /* +10 flats: the shade row */
    u32 shade;                  /* +14 shade row (light index 4) or shader code (8) */
    u32 tex_origin;             /* +18 packed texture origin (rotated by 16 for the lit path) */
    s32 unknown_1c, unknown_20; /* +1C +20 (flats: +1C the v origin) */
    s32 u_dx, u_dy, u_0;        /* +24 +28 +2C  u/z gradients */
    s32 v_dx, v_dy, v_0;        /* +30 +34 +38  v/z gradients */
    void *span_fn;              /* +3C span routine (a setup routine until the first span) */
    const u8 *texels;           /* +40 texels, or the colour */
    u8 *tmap;                   /* +44 compiled mapper copy (flats: scale | light << 16) */
    s32 normal;                 /* +48 */
    u32 wrap_mask;              /* +4C (flats: the fog row or translucency table) */
    s32 du8, dv8, dz8;          /* +50 +54 +58  8-px steps, doubled for 16-px routines */
    s32 dzdx;                   /* +5C d(1/z)/dx */
    s32 inv_dzdx;               /* +60 2^32 / dzdx */
} xn_poly;

typedef struct xn_span {        /* 16 bytes */
    struct xn_span *next;       /* +0 */
    u16 x_end, x_start;         /* +4 +6 */
    u32 inv_z;                  /* +8 1/z at x_start (2^40/z) */
    xn_poly *poly;              /* +C */
} xn_span;
```

## Exactness traps that apply everywhere

1. **The records compare final bytes, not write sequences.** Repeated stores, store order and
   the planted-`ret` plant/restore pairs are invisible. Missing a store of a patch field, or a
   store with the wrong width, is not.
2. **Patched bytes stay observable for as long as the records include code pages.** Even after
   every reader of a field is C, the writer must keep writing it (KEEP). `patch_fields.csv`'s
   `self_patch` column marks the fields that become dead once their one function is C. Those
   are the candidates for the record-mask proposal below.
3. **Register outputs follow the ABI rows, not the sketches.** Several of these functions are
   reached through pointers (span routines, shaders, the fog hook), so their rows say
   "partial" or "unknown". Run the infra's clobber test before trusting "no outputs".
   - 144FC8 returns ebx = 16 * w if any caller reads it.
   - 12F9A0 returns the planted offset in ebp.
4. **Flags.**
   - CF outputs: `xn_tmap_compile`, `xn_terrain_draw_cells`, `xn_model_draw` /
     `_draw_faces`, `xn_render_frame` (eax as well).
   - The shader builders *preserve* the caller's CF. Use the flags-adapter stubs.
5. **Signedness.** `sar` vs `shr`, `imul` vs `mul`, `jl` vs `jb`. The sketches mark them. The
   packed 8.8 u/v values mix both in one register.
6. **Port I/O and interrupts.** Only GFX-VESA-BLIT (bank switches through `int 10h` between the
   copies) has any, and it is dead.
7. **Asm addresses as data.** The sentinel routine 12A949, the span routines the setups store
   in `poly->span_fn`, and the fog hook are compared as asm addresses. Store `XN_ASM(...)`,
   never a C function's address.

## Where readable C cannot match the records, and what to do

| Case | Why | Proposal |
|---|---|---|
| **Dropping code generation:** a C texture mapper or light shader that stores parameters instead of x86 bytes | the 418-byte tmap slots, the shader copies in `big_buffer`, the scaled row in `big_buffer` and the 77 template fields (32 tmap + 45 light) are recorded writes | Keep the byte-exact generators for the whole replay phase; option B already makes the *runners* readable. For the native port, add a record-compare rule to `tools/xn_rc.py`: writes into the tmap pool, the shader pool range and the template fields are "generated code", compared through a decoder (a few lines of Python per template: read mask/base, or the light fields, from the written bytes) against the C's parameter record. |
| **Dropping KEEP stores** | a self-patched field's final value is in the record | The same mechanism: a mask of fields whose readers are all C (`self_patch = yes`, plus the cross fields once both sides are C). Until then, keep the stores. They cost one line each. |
| **A body's own records** with the C body | recorded with the `ret` planted | Not a mismatch: the RET adapter recovers n. |
| **Unreachable planted-`ret` cases:**<br>fog n >= 640 and water n >= 641 (past the offset tables);<br>140A28 with 1024 vertices (the restore overwrites the body's `ret`);<br>14501C wider than 640 (plants into 147820) | the asm would read past a table or write into code; the C loop does neither | Document, don't reproduce. The game cannot produce these (spans are at most 320 px, and `xn_model_prepare` rejects 1024 vertices). |
| **DRAW-SCALED limits** | at 34x scale or more a `je rel8` wraps; the pair loop's `loop` counts with ecx's top half (the caller's h argument) | The emitter reproduces the bytes exactly; the interpreted runner does not follow a wrapped jump. Assert scale < 34 and h < 65536 in the glue. |
| **Planted-`ret` bodies that are reached only with the `ret` already planted** (the tails) | they have records but no natural C entry | The RET adapter (above). |

Two original bugs the C must *keep*, because the records include them:
- the glyph left-clip shift (font.md);
- the rain streak's n < 0 path (sky.md).

The odd-row checkerboard count (draw.md) and the first piece of every flat not being drawn
(flat.md) are also original behaviour to keep.

## Conversion order

1. **Infrastructure:** the helpers above, the RET adapter in `tools/xn_rc.py`'s router, and the
   register-file call for routine pointers (span routines, shaders, the fog hook, tmap copies).
2. **The pure writers**, to prove the field-variable linkage, field types and widths:
   - RENDER-SETUP;
   - SHADE-FOG-SETUP;
   - FLAT-WRITERS;
   - `xn_light_init` and `xn_shade_load`.
3. **Leaf functions with patched operands only:**
   - FONT-GLYPH, POLY-PROJECT, POLY-GRAD, POLY-SETUP-TEX;
   - MODEL-SCALE, MODEL-VERTS, MODEL-LIGHTS, MODEL-DRAW;
   - TERRAIN-AXES, TERRAIN-CELLS, TERRAIN-GRID;
   - WATER-DRAW, FLAT-GRAD, FLAT-EMIT, FLAT-LIGHT.
4. **Fixed unrolled:** DRAW-OVERLAY, DRAW-CHECKER.
5. **Planted-`ret` pairs.** Pilot the adapter on SHADE-FOG (the simplest body, 29 + 29
   records). Then:
   - DRAW-TRANSPARENT, WATER-SPAN, MODEL-FACES-CLEAR, SKY-RAIN;
   - the span family: SPAN-TEX, SPAN-TEX-SHADED, SPAN-TEX64, SPAN-TEX64-SHADED, then
     SPAN-FLAT-*.
6. **The walkers:** POLY-RASTER and FLAT-RASTER.
7. **Generators:**
   - DRAW-SCALED (C094A first: C0700 still runs its bytes);
   - TMAP;
   - LIGHT-SHADERS;
   - LIGHT-SETUP (15BC42 together with 15BF75).
8. **Runners of generated code**, with option A then option B: SPAN-SOLID-LIT, SPAN-TEX-LIT.
9. **RENDER-FRAME** last: it calls everything through pointers, and its C must recognise the
   sentinel instead of calling it.
10. **The dead or never-run groups**, by differential tests only:
    - DRAW-SHADED-ROW, DRAW-REMAP320, DRAW-PATTERN8;
    - GFX-COPYCLEAR-UNROLLED, GFX-VESA-BLIT;
    - SPAN-FLAT-TRANSLUCENT.

## Least certain

- **SPAN-FLAT-*:** the uv/step/remainder packing was transcribed from the asm, and it is the
  easiest place to slip. FLAT-RASTER: which of the two patched `add [mem], imm` advances u.
  The answer is in 1550E0's stores, but nothing has run it.
- **LIGHT-SHADERS option B:** identifying the template of a copy by its bytes, and the offsets
  of templates 2 and 3. They come from the writer pairs in `patch_fields.csv`; check them there.
- **POLY-RASTER and FLAT-RASTER:** the sketches give the state and the order, not the full
  jump structure.
- **SPAN-TEX-LIT's last run** (the shader at pixel n-1, the reciprocal-table divide). Read
  156C2B-156CE5 again when converting.
- **The ABI-dependent claims:** "no register outputs" for the span family and water, and
  "CF preserved" for the builders. They depend on the clobber test.

## Every affected function

`groups.csv` has the same rows, plus the record counts (own, all-C runs, and the recorded
callers that exercise the group). Status: every group has a design and a sketch; no function is
converted yet.

| VA | Name | Group | Mechanism | Must convert with | Design |
|---|---|---|---|---|---|
| 0C0700 | `xn_draw_image_scaled` | DRAW-SCALED | self-patched operands (3); runs generated code | - | [draw.md](draw.md) |
| 0C08CC | `xn_draw_image_scaled_clip` | DRAW-SCALED | none (group member: clip) | - | [draw.md](draw.md) |
| 0C094A | `xn_draw_image_scaled_compile_row` | DRAW-SCALED | code generator (scaled row into big_buffer) | - | [draw.md](draw.md) |
| 0C0A36 | `xn_img_unpack_rows` | DRAW-SCALED | none (group member: unpack) | - | [draw.md](draw.md) |
| 0C8381 | `xn_draw_image_shaded` | DRAW-SHADED-ROW | none (stack wrapper) | - | [draw.md](draw.md) |
| 0C8395 | `xn_draw_image_shaded_regs` | DRAW-SHADED-ROW | planted ret planter (word saved and restored) | 0C83F6 (dead; soft) | [draw.md](draw.md) |
| 0C83F6 | `xn_draw_shaded_row_unrolled` | DRAW-SHADED-ROW | unrolled body x321, planted ret | 0C8395 (dead; soft) | [draw.md](draw.md) |
| 0C9D19 | `xn_sky_draw_rain_streak` | SKY-RAIN | planted ret with a pushed return address; body x31 | 0C9D63 (soft) | [sky.md](sky.md) |
| 0C9D63 | `xn_sky_draw_rain_streak_bottom_clip` | SKY-RAIN | shared body entry (planted ret) | 0C9D19 (no caller; soft) | [sky.md](sky.md) |
| 0C9F4F | `xn_draw_remap_rows_320` | DRAW-REMAP320 | fixed unrolled (320 per row) | - | [draw.md](draw.md) |
| 0CB552 | `xn_draw_fullscreen_overlay_shaded` | DRAW-OVERLAY | fixed unrolled (320 x 200) | - | [draw.md](draw.md) |
| 0CD39B | `xn_draw_fill_rows_pattern8` | DRAW-PATTERN8 | fixed unrolled (40 per row) | - | [draw.md](draw.md) |
| 0CD53C | `xn_draw_view_checkerboard` | DRAW-CHECKER | fixed unrolled (2 x 160), entered mid-loop by parity | - | [draw.md](draw.md) |
| 12A100 | `xn_render_init` | RENDER-SETUP | patch writer (25 fields) | - | [render.md](render.md) |
| 12A274 | `xn_cam_set_focal` | RENDER-SETUP | patch writer (2 fields) | - | [render.md](render.md) |
| 12A2D0 | `xn_cam_set_view_window` | RENDER-SETUP | patch writer (29 fields) | - | [render.md](render.md) |
| 12A3AC | `xn_cam_update_derived` | RENDER-SETUP | patch writer (5 fields) | - | [render.md](render.md) |
| 12A4F0 | `xn_render_begin_frame` | RENDER-SETUP | patch writer (3 fields) | - | [render.md](render.md) |
| 12A870 | `xn_render_frame` | RENDER-FRAME | self-patched operands (2); patch writer (6 ray fields); stack unwinding row end | 12A949 12A94E 12A969 12A975 12A976 12A97C 12A97E (hard) | [render.md](render.md) |
| 12A949 | `xn_render_frame_row_end` | RENDER-FRAME | run-time block of xn_render_frame | 12A870 12A94E 12A969 12A975 12A976 12A97C 12A97E (hard) | [render.md](render.md) |
| 12A94E | `xn_render_frame_next_row` | RENDER-FRAME | run-time block of xn_render_frame | 12A870 12A949 12A969 12A975 12A976 12A97C 12A97E (hard) | [render.md](render.md) |
| 12A969 | `xn_render_frame_rows_done` | RENDER-FRAME | run-time block of xn_render_frame | 12A870 12A949 12A94E 12A975 12A976 12A97C 12A97E (hard) | [render.md](render.md) |
| 12A975 | `xn_render_frame_rows_done_popfd` | RENDER-FRAME | run-time block of xn_render_frame | 12A870 12A949 12A94E 12A969 12A976 12A97C 12A97E (hard) | [render.md](render.md) |
| 12A976 | `xn_render_frame_flats` | RENDER-FRAME | run-time block of xn_render_frame | 12A870 12A949 12A94E 12A969 12A975 12A97C 12A97E (hard) | [render.md](render.md) |
| 12A97C | `xn_render_frame_flats_check` | RENDER-FRAME | run-time block of xn_render_frame | 12A870 12A949 12A94E 12A969 12A975 12A976 12A97E (hard) | [render.md](render.md) |
| 12A97E | `xn_render_frame_exit` | RENDER-FRAME | run-time block of xn_render_frame | 12A870 12A949 12A94E 12A969 12A975 12A976 12A97C (hard) | [render.md](render.md) |
| 12DC44 | `xn_font_draw_glyph` | FONT-GLYPH | self-patched operands (2) | - | [font.md](font.md) |
| 12F79C | `xn_water_draw` | WATER-DRAW | self-patched operand (1) | - | [water.md](water.md) |
| 12F9A0 | `xn_water_span` | WATER-SPAN | planted ret planter | 12F9E0 (soft) | [water.md](water.md) |
| 12F9E0 | `xn_water_span_unrolled` | WATER-SPAN | unrolled body x641, planted ret | 12F9A0 (soft) | [water.md](water.md) |
| 136A00 | `xn_light_init` | LIGHT-SHADERS | patch writer (6 template fields) | - | [light.md](light.md) |
| 136B88 | `xn_shade_load` | LIGHT-SHADERS | patch writer (6 template fields) | - | [light.md](light.md) |
| 136C30 | `xn_light_tmpl_1` | LIGHT-SHADERS | code template (1 light), never run in place | - | [light.md](light.md) |
| 136C84 | `xn_light_tmpl_2` | LIGHT-SHADERS | code template (2 lights) | - | [light.md](light.md) |
| 136D10 | `xn_light_tmpl_3` | LIGHT-SHADERS | code template (3 lights) | - | [light.md](light.md) |
| 13E744 | `xn_terrain_setup_axes` | TERRAIN-AXES | fixed unrolled (6 + 9) | - | [terrain.md](terrain.md) |
| 13E8C8 | `xn_terrain_transform_grid` | TERRAIN-GRID | self-patched walker state (20) + view fields (4) | - | [terrain.md](terrain.md) |
| 13EFD7 | `xn_terrain_draw_cells` | TERRAIN-CELLS | self-patched constants (6); small fixed unroll (4 corners) | - | [terrain.md](terrain.md) |
| 140284 | `xn_model_draw` | MODEL-DRAW | patch writer (13 fields); patched operand (1) | - | [model.md](model.md) |
| 1403AF | `xn_model_draw_faces` | MODEL-FACES-CLEAR | planted ret planter; patch writer (4); patched operands (8) | 140A28 (soft) | [model.md](model.md) |
| 1404FA | `xn_model_transform_face_verts` | MODEL-VERTS | patched operands (11) | - | [model.md](model.md) |
| 140606 | `xn_model_build_light_list` | MODEL-LIGHTS | self-patched operands (4); reads 140497 as data | - | [model.md](model.md) |
| 1406D6 | `xn_model_scale_matrix` | MODEL-SCALE | patched operands (4) | - | [model.md](model.md) |
| 140A28 | `xn_model_clear_vert_flags` | MODEL-FACES-CLEAR | unrolled body x1024, planted ret | 1403AF (soft) | [model.md](model.md) |
| 143BB0 | `xn_gfx_copy_and_clear_unrolled` | GFX-COPYCLEAR-UNROLLED | fixed unrolled (128 per 512 bytes) | - | [gfx.md](gfx.md) |
| 144F58 | `xn_draw_img_record_transparent_regs` | DRAW-TRANSPARENT | none (IMG record wrapper, jmp 144FC8) | - | [draw.md](draw.md) |
| 144FB4 | `xn_draw_image_transparent` | DRAW-TRANSPARENT | none (stack wrapper) | - | [draw.md](draw.md) |
| 144FC8 | `xn_draw_image_transparent_regs` | DRAW-TRANSPARENT | planted ret planter (word saved and restored) | 14501C (soft) | [draw.md](draw.md) |
| 14501C | `xn_draw_transparent_row` | DRAW-TRANSPARENT | unrolled body x640, planted ret | 144FC8 (soft) | [draw.md](draw.md) |
| 14D23C | `xn_shade_set_fog` | SHADE-FOG-SETUP | patch writer (10 fields) | - | [shade.md](shade.md) |
| 14D300 | `xn_shade_fog_span_off` | SHADE-FOG | none (the empty hook) | - | [shade.md](shade.md) |
| 14D320 | `xn_shade_fog_pixels` | SHADE-FOG | unrolled body x641, planted ret | 150040 (soft) | [shade.md](shade.md) |
| 150040 | `xn_shade_fog_span` | SHADE-FOG | planted ret planter; patched operands (10) | 14D320 (soft) | [shade.md](shade.md) |
| 154E20 | `xn_flat_draw` | FLAT-WRITERS | patch writer (1 field) | - | [flat.md](flat.md) |
| 1550E0 | `xn_flat_setup_gradients` | FLAT-GRAD | patched operands (8); patch writer (4, incl. 2 address fields) | - | [flat.md](flat.md) |
| 15526C | `xn_flat_span_emit` | FLAT-EMIT | patched operands (2) | - | [flat.md](flat.md) |
| 1552A0 | `xn_flat_raster` | FLAT-RASTER | self-patched walker state; patch writer | 155332 (hard) | [flat.md](flat.md) |
| 155332 | `xn_flat_raster_rows` | FLAT-RASTER | entry inside 1552A0 (self-patched walker) | 1552A0 (hard; no caller) | [flat.md](flat.md) |
| 155415 | `xn_flat_begin_frame` | FLAT-WRITERS | patch writer (7 fields) | - | [flat.md](flat.md) |
| 155610 | `xn_flat_span_light_setup` | FLAT-LIGHT | patched operand (1) | - | [flat.md](flat.md) |
| 155880 | `xn_span_solid_lit_tail` | SPAN-SOLID-LIT | unrolled body x16, planted ret | 155920 (soft) | [span.md](span.md) |
| 155920 | `xn_span_solid_lit` | SPAN-SOLID-LIT | unrolled 16-px block; planted ret planter; self-patched (4) and patched (4) operands; runs a generated shader | 155880 (soft) | [span.md](span.md) |
| 155C00 | `xn_span_tex_tail` | SPAN-TEX | unrolled body x16, planted ret (shared tail) | 155D20 155F60 155F70 (soft) | [span.md](span.md) |
| 155D20 | `xn_span_tex_8` | SPAN-TEX | unrolled 8-px block; planted ret planter; self-patched (5), patched (3) | 155F60 155F70 155C00 (soft) | [span.md](span.md) |
| 155F60 | `xn_span_tex_16_setup` | SPAN-TEX | none (setup, falls into 155F70) | 155D20 155F70 155C00 | [span.md](span.md) |
| 155F70 | `xn_span_tex_16` | SPAN-TEX | unrolled 16-px block; planted ret planter; self-patched (5), patched (3) | 155D20 155F60 155C00 (soft) | [span.md](span.md) |
| 156320 | `xn_span_tex_shaded_tail` | SPAN-TEX-SHADED | unrolled body x16, planted ret (shared tail) | 156460 1566B4 1566C4 (soft) | [span.md](span.md) |
| 156460 | `xn_span_tex_shaded_8` | SPAN-TEX-SHADED | unrolled 8-px block; planted ret planter; self-patched (5), patched (3) | 1566B4 1566C4 156320 (soft) | [span.md](span.md) |
| 1566B4 | `xn_span_tex_shaded_16_setup` | SPAN-TEX-SHADED | none (setup, falls into 1566C4) | 156460 1566C4 156320 | [span.md](span.md) |
| 1566C4 | `xn_span_tex_shaded_16` | SPAN-TEX-SHADED | unrolled 16-px block; planted ret planter; self-patched (5), patched (3) | 156460 1566B4 156320 (soft) | [span.md](span.md) |
| 156A80 | `xn_span_tex_lit_setup` | SPAN-TEX-LIT | none (setup, falls into 156A94) | 156A94 | [span.md](span.md) |
| 156A94 | `xn_span_tex_lit` | SPAN-TEX-LIT | self-patched (5), patched (7); runs generated tmap copies and shaders; plants a ret in the heap copy | 156A80 | [span.md](span.md) |
| 156D40 | `xn_span_tex64_tail` | SPAN-TEX64 | unrolled body x16, planted ret | 156E60 (soft) | [span.md](span.md) |
| 156E60 | `xn_span_tex64` | SPAN-TEX64 | unrolled 16-px block; planted ret planter; self-patched (4), patched (3) | 156D40 (soft) | [span.md](span.md) |
| 157100 | `xn_span_tex64_shaded_tail` | SPAN-TEX64-SHADED | unrolled body x16, planted ret | 157240 (soft) | [span.md](span.md) |
| 157240 | `xn_span_tex64_shaded` | SPAN-TEX64-SHADED | unrolled 16-px block; planted ret planter; self-patched (6), patched (3) | 157100 (soft) | [span.md](span.md) |
| 157620 | `xn_span_flat_transparent` | SPAN-FLAT-TRANSPARENT | unrolled 8-px block; planted ret planter; patched (2) | 157760 (soft) | [span.md](span.md) |
| 157760 | `xn_span_flat_transparent_tail` | SPAN-FLAT-TRANSPARENT | unrolled body x8, planted ret | 157620 (soft) | [span.md](span.md) |
| 157800 | `xn_span_flat_transparent_shaded` | SPAN-FLAT-SHADED | unrolled 8-px block; planted ret planter; self-patched (1), patched (2) | 157960 (soft) | [span.md](span.md) |
| 157960 | `xn_span_flat_transparent_shaded_tail` | SPAN-FLAT-SHADED | unrolled body x8, planted ret | 157800 (soft) | [span.md](span.md) |
| 157B20 | `xn_span_flat_lit_fogged` | SPAN-FLAT-FOGGED | unrolled 8-px block; planted ret planter; self-patched (2), patched (2) | 157CA0 (soft) | [span.md](span.md) |
| 157CA0 | `xn_span_flat_lit_fogged_tail` | SPAN-FLAT-FOGGED | unrolled body x8, planted ret | 157B20 (soft) | [span.md](span.md) |
| 157E02 | `xn_span_tail_offsets_25_entry1` | SPAN-FLAT-TRANSLUCENT | not code (an offsets-table entry decoded as code) | 157E20 157FA0 (no C) | [span.md](span.md) |
| 157E20 | `xn_span_flat_translucent` | SPAN-FLAT-TRANSLUCENT | unrolled 8-px block; planted ret planter; self-patched (2), patched (2) | 157FA0 157E02 (soft; never ran) | [span.md](span.md) |
| 157FA0 | `xn_span_flat_translucent_tail` | SPAN-FLAT-TRANSLUCENT | unrolled body x8, planted ret | 157E20 157E02 (soft) | [span.md](span.md) |
| 158360 | `xn_poly_project_flat` | POLY-PROJECT | patched operands (4) | - | [poly.md](poly.md) |
| 158420 | `xn_poly_project_face` | POLY-PROJECT | patched operands (4) | - | [poly.md](poly.md) |
| 1585C0 | `xn_poly_project_terrain` | POLY-PROJECT | patched operands (4) | - | [poly.md](poly.md) |
| 15B9A0 | `xn_poly_rasterize` | POLY-RASTER | self-patched walker state (5) | 15BA16 (hard) | [poly.md](poly.md) |
| 15BA16 | `xn_poly_rasterize_row_loop` | POLY-RASTER | entry inside 15B9A0 | 15B9A0 (hard; unreachable) | [poly.md](poly.md) |
| 15BAC0 | `xn_poly_tex_gradients` | POLY-GRAD | self-patched operands as saved registers (4) | - | [poly.md](poly.md) |
| 15BB8C | `xn_poly_setup_textured` | POLY-SETUP-TEX | patched operands (3, one a byte shift count); tail-jumps to the span routine | - | [poly.md](poly.md) |
| 15BC42 | `xn_light_setup_poly` | LIGHT-SETUP | patched operand (ambient row); tail-jumps into the generators | 15BF75 (hard) | [light.md](light.md) |
| 15BC9C | `xn_light_setup_terrain` | LIGHT-SETUP | none; tail-jumps into the generators | - | [light.md](light.md) |
| 15BCF6 | `xn_light_build_shader_1` | LIGHT-SHADERS | code generator (shader 1) | - | [light.md](light.md) |
| 15BD78 | `xn_light_build_shader_2` | LIGHT-SHADERS | code generator (shader 2) | - | [light.md](light.md) |
| 15BE4D | `xn_light_build_shader_3` | LIGHT-SHADERS | code generator (shader 3) | - | [light.md](light.md) |
| 15BF75 | `xn_light_add_point` | LIGHT-SETUP | none; frame-skipping jmp into the generator | 15BC42 (hard) | [light.md](light.md) |
| 15C200 | `xn_tmap_pool_alloc` | TMAP | none (allocates the pool) | - | [tmap.md](tmap.md) |
| 15C274 | `xn_tmap_compile` | TMAP | code generator (patches the template in place, copies 418 bytes) | - | [tmap.md](tmap.md) |
| 15C2DC | `xn_tmap_rebase` | TMAP | code patcher (rebases a copy) | - | [tmap.md](tmap.md) |
| 15C2F0 | `xn_tmap_pool_reset` | TMAP | none (pool reset) | - | [tmap.md](tmap.md) |
| 15C300 | `xn_tmap_template` | TMAP | code template (8 two-pixel steps), never run in place; copies get planted rets | - | [tmap.md](tmap.md) |
| 15FF53 | `xn_gfx_vesa_blit_rect_banked` | GFX-VESA-BLIT | self-patched operands (3) | - | [gfx.md](gfx.md) |
