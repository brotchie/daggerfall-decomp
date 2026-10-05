# draw: 2D blits with generated, unrolled or self-patched code

These are the 2D routines on `screen_buffer` that use the mechanisms. The draw subsystem has
10,665 instructions, about 8,600 of them in four unrolled bodies:
- C83F6: 1,605 instructions;
- C9F4F: 960;
- CB552: 3,520;
- 14501C: 2,560.

The rain streak is in sky.md and the font glyph in font.md.

Rules (PF, KEEP, RET, UNR, GEN, DIV) are in index.md.

| Group | Functions | Mechanism | Live? | Records |
|---|---|---|---|---|
| DRAW-SCALED | C0700, C094A (+ C08CC, C0A36 plain) | **generated row code** in `big_buffer`; 3 self-patched operands | yes (inventory, paperdoll) | C0700 24, C094A 24 |
| DRAW-TRANSPARENT | 144FC8, 14501C (+ wrappers 144FB4, 144F58; caller 12B3ED) | planted `ret` (word saved and restored), 640 steps | yes (34 game sites) | 144FC8 28, 14501C 25 (129 in all-C mode) |
| DRAW-SHADED-ROW | C8395, C83F6 (+ wrapper C8381) | planted `ret` (word saved), 321 steps | dead | 2 + 2 (synthetic) |
| DRAW-OVERLAY | CB552 | fixed unrolled: 320 px x 200 rows | yes (9 game sites) | 24 |
| DRAW-CHECKER | CD53C | fixed unrolled, entered mid-loop by parity | yes (hurt flash) | 16 |
| DRAW-REMAP320 | C9F4F | fixed unrolled: 320 px per row | dead | 1 (synthetic) |
| DRAW-PATTERN8 | CD39B | fixed unrolled: 40 x 8 bytes per row | dead | 1 (synthetic) |

Not code, but related: 0xCE674 (inside xn_CE300's data blob, unreferenced) is a leftover
template of the scaled-row instructions:
- `mov al,[esi+186A0h]; or al,al; je; xlat; mov al,[eax]; mov [edi+186A0h],al;`
- `mov ah,al; mov [edi+186A0h],ax; ret`.

It shows the instruction forms C094A emits. Give it no C; it can stay a data blob.

## DRAW-SCALED: `xn_draw_image_scaled` (C0700) and its row compiler (C094A)

**What.** It draws an image scaled to w x h at (x, y), clipped, with 0 and FFh transparent and
an optional remap through `[195B80h]` (flag 8000h). Flag 8 first unpacks a row-compressed
image into `scratch_buffer` (C0A36).

**Mechanism.**
1. C0700 computes 8.8 DDA steps:
   - x: `xn_draw_scaled_x_int` C0612 (byte) and `_x_frac` C0614 (word);
   - y: `_y_int` C0613 and `_y_frac` C0616.

   It clips (C08CC) and computes the first source offset C061C.
2. **The generator** C094A (called through `xn_draw_scaled_compile_fn` C061E) writes one row
   of machine code at `big_buffer`. For each source column whose DDA count c is non-zero:

   ```
   8A 86 s32      mov al, [esi + src_ofs]     ; src_ofs = C061C, +1 per source column
   0A C0          or al, al
   74 d1          je end                       ; d1, d2 patched after the column is emitted
   3C FF          cmp al, 0FFh
   74 d2          je end
   D7             xlat                         ; only with flag 8000h
   88 87 d32      mov [edi + x], al            ; if c is odd
   8A E0          mov ah, al                   ; if c >= 2
   66 89 87 d32   mov [edi + x], ax            ; c/2 times, x += 2
   end:
   ```

   then a final `C3`. The right clip shortens the last column. A column clipped to nothing still
   emits its load/compare part with both `je`s to its end.
3. **The runner** is C0700's row loop:
   - `call [big_buffer]` with esi = source row, edi = destination row, ebx = `[195B80h]`;
   - c_y times per source row, edi += 140h (a constant 320);
   - it stops when edi reaches `xn_draw_scaled_bottom`.
4. **Self-patched operands** (all written by C0700 itself):

   | Field | Operand | Value | Kind |
   |---|---|---|---|
   | C08AD | `cmp edi, BOTTOM` | `screen_buffer + xn_gfx_row_offset[clip_bottom]` | dword, KEEP |
   | C08B9 | `add esi, STRIDE` | always 100h (the unpacked image's stride) | dword, KEEP |
   | C08BE | `mov bh, YINT` | `xn_draw_scaled_y_int` | **byte**, KEEP |

**Readable C.** Code generation is behaviour here (the `big_buffer` bytes are recorded writes),
so the emitter stays. The runner does not have to run x86: the emitter builds the same row
plan it encodes, and C draws from the plan (rule GEN, option B, with no decoding needed,
because the plan is built in the same call).

```c
/* scaled.c */
typedef struct scaled_col { u16 src; u16 x; u16 count; } scaled_col;   /* count may be 0 */

extern u8  *xn_draw_scaled_bottom;      /* C08AD KEEP */
extern u32  xn_draw_scaled_stride;      /* C08B9 KEEP (always 100h) */
extern u8   xn_draw_scaled_yint_op;     /* C08BE KEEP, a byte */

static u8 *emit_row(scaled_col *plan, int *ncols)      /* C094A: the bytes are the behaviour */
{
    u8 *code = big_buffer;
    u16 cols = xn_draw_scaled_src_w;
    u16 acc = (u16)(xn_draw_scaled_x_int << 8) | (u8)xn_draw_scaled_x_frac;    /* bh:bl */
    u32 x = (u16)xn_draw_scaled_x;                                             /* esi */
    int n = 0;

    do {
        u32 c = acc >> 8;                                                      /* cl = bh */
        if (c != 0) {
            u8 *je1, *je2;
            *code++ = 0x8A; *code++ = 0x86; XN_PUT32(code, (u16)xn_draw_scaled_src_ofs); code += 4;
            *code++ = 0x0A; *code++ = 0xC0; *code++ = 0x74; je1 = ++code;
            *code++ = 0x3C; *code++ = 0xFF; *code++ = 0x74; je2 = ++code;
            plan[n].src = xn_draw_scaled_src_ofs;
            plan[n].x = (u16)x;
            if ((s16)(x + c) > (s16)xn_gfx_clip_right) {
                cols = 1;                                       /* the last column */
                c -= (s16)(x + c) - (s16)xn_gfx_clip_right;
            }
            plan[n].count = (s16)c > 0 ? c : 0;
            if ((s16)c > 0) {
                if (xn_draw_scaled_flags & 0x8000)
                    *code++ = 0xD7;                             /* xlat */
                if (c & 1) {
                    *code++ = 0x88; *code++ = 0x87; XN_PUT32(code, x); code += 4; x++;
                }
                if (c >>= 1) {
                    *code++ = 0x8A; *code++ = 0xE0;
                    do {
                        *code++ = 0x66; *code++ = 0x89; *code++ = 0x87;
                        XN_PUT32(code, x); code += 4; x += 2;
                    } while (--c);                              /* loop: counts ecx, see traps */
                }
            }
            je2[-1] = (u8)(code - je2);
            je1[-1] = (u8)(code - je1);
            n++;
        }
        xn_draw_scaled_src_ofs++;
        acc = (u16)(((u16)xn_draw_scaled_x_int << 8 | (acc & 0xFF)) + (u16)xn_draw_scaled_x_frac);
    } while (--cols != 0);
    *code = 0xC3;
    *ncols = n;
    return code;
}

static void run_row(const scaled_col *plan, int n, const u8 *src, u8 *row)
{
    int i, k;
    for (i = 0; i < n; i++) {
        u8 c = src[plan[i].src];
        if (c == 0 || c == 0xFF)
            continue;
        if (xn_draw_scaled_flags & 0x8000)
            c = xn_dye_remap[c];                /* xlat with ebx = [195B80h] */
        for (k = 0; k < plan[i].count; k++)
            row[plan[i].x + k] = c;
    }
}
```

`xn_draw_image_scaled` is then the C0700 setup (word-sized globals exactly as the asm writes
them: C0602-C061C are all written), `emit_row`, the three KEEP stores, and the asm's row loop
with `run_row` in place of `call [big_buffer]`. The plan needs at most `src_w` entries (one per
source column, less than 256 for the game's images), so a local array of 320 is enough.

**Conversion group:** C0700 and C094A.
- C094A has 24 records of its own, and its "function" is the emitter, so route it alone first
  (the emitter writes the bytes and returns, as C094A does). Then convert C0700.
- C08CC (clip) and C0A36 (unpack) are plain C.
- While C0700 is asm and C094A is C, C0700 executes the C-emitted code, which is the
  byte-identical test of the emitter.

Tests:
- C0700: 24 records;
- C094A: 24 (48 in all-C mode);
- C08CC: 24;
- C0A36: 24;
- the game's inventory and paperdoll screens.

**Exactness traps.**
- **The `loop` uses ecx.** The pair loop (C0A04 `loop`) decrements all of ecx, but only cx was
  cleared (C096F `xor cx, cx`). ecx's top half is the top half of C0700's h argument (the ABI
  row: input `ecx.u`). The game always passes h < 65536, so the top half is 0. The C assumes
  it; assert it in the glue rather than reproducing 65536-fold loops.
- **rel8 overflow.** A column with c of about 34 or more makes its `je` displacement wrap.
  This is a scale of 34x or more. The emitter above writes the same wrapped byte, but
  `run_row` would not reproduce the asm jumping into the middle of the code. No game image is
  scaled that far; document it as a limit.
- The source stride is always 100h (C08B9), right only for unpacked (flag 8) images. The
  top-clip skips `rows * src_w` bytes, not `rows * 100h` (part_A.md). Both are original
  behaviour: keep them.
- Destination rows advance by a constant 140h, not `xn_gfx_width`.
- The word-sized statics C0600-C061C are memory writes; write them all, with their widths.
- The DDA accumulators are 16-bit (`add bx, dx`), and the column count is `bh` after the carry.

## DRAW-TRANSPARENT: `xn_draw_image_transparent_regs` (144FC8) and the row body (14501C)

**What.** It draws a w x h image at (x, y), clipped (144E00), with colour 0 transparent.

**Mechanism.**
- Unrolled body 14501C: 640 steps of 4 instructions, 16 bytes a step:
  `mov al,[esi+100h+k]; test al,al; je; mov [edi+100h+k],al`.
- The planter does:
  1. `shl ebx,4` (w * 16);
  2. **saves the word** at `14501C + 16w` (`push word ptr`);
  3. plants `C3`;
  4. calls once per row (esi += w + skip, edi += `xn_gfx_width`);
  5. restores the word (`pop word ptr`) after the last row.

  The restore is exact by construction.

**Readable C.**

```c
static void transparent_run(u8 *dst, const u8 *src, int w)     /* 14501C, x640 */
{
    int k;
    for (k = 0; k < w; k++)
        if (src[k])
            dst[k] = src[k];
}

/* asm: eax = x, edx = y, ebx = w, ecx = h, esi = pixels */
void xn_draw_image_transparent_regs(s32 x, s32 y, s32 w, s32 h, const u8 *src)
{
    s32 skip = 0;
    u8 *dst;

    if (xn_draw_clip_rect(&x, &y, &w, &h, &src, &skip))        /* CF: nothing visible */
        return;
    dst = screen_buffer + xn_gfx_row_offset[y] + x;
    do {
        transparent_run(dst, src, w);
        src += w + skip;
        dst += xn_gfx_width;
    } while (--h != 0);
}
```

**Conversion group:** 144FC8 and 14501C (rule RET).
- The wrappers 144FB4 (stack argument) and 144F58 (IMG record, `jmp 144FC8`) are trivial, and
  so is the cursor caller 12B3ED (mouse).
- Adapter for 14501C's own 25 records: `xn_planted_count(0x14501C, 16, 640)`, with
  esi/edi - 100h. The saved word does not matter to the adapter: only the `C3` is looked for.
- Tests:
  - 144FC8: 28 records (108 in all-C mode);
  - 14501C: 25 (129 in all-C mode);
  - 144FB4: 28;
  - the HUD and every UI screen.

**Traps.**
- **Register outputs.** The ABI row of 144FC8 lists eax, ecx, edx and ebx as outputs, because
  144FB4's address escapes. At the return they hold:
  - eax = the clipped x;
  - ecx = 0;
  - edx = `xn_gfx_width`;
  - ebx = **16 * the clipped w**.

  If the clobber test cannot prove them dead, the glue must return exactly these.
- The body is only as wide as 640 pixels. A wider w would plant past the body, into 147820, and
  the word restore would still be exact. Clipped widths are at most 320.
- The clip routine 144E00 returns its results in 5 registers plus ebp (skip). In C it is
  `xn_draw_clip_rect(&x, &y, &w, &h, &src, &skip)`; mind that it is shared with 144E9C, 144EF0
  and 144F7C.

## DRAW-SHADED-ROW (dead): `xn_draw_image_shaded_regs` (C8395) and C83F6

This is DRAW-TRANSPARENT with a shade lookup:
- 321 steps of 5 instructions, 18 bytes a step;
- `mov al,[esi+100h+k]; or al,al; je; mov al,[eax]; mov [edi+100h+k],al`;
- eax = `xn_shade_table + xn_draw_shade_row * 256` (the row byte C7B8A is never written: 0);
- the planter saves and restores the word at `C83F6 + 18w`.

The C is `if (src[k]) dst[k] = row[src[k]]` per pixel. There is no caller of C8381 and no play
record. Convert it last, with differential tests, or leave it as asm. Group: C8381, C8395 and
C83F6.

## DRAW-OVERLAY: `xn_draw_fullscreen_overlay_shaded` (CB552)

**What.** It draws a full-screen 320x200 overlay over `screen_buffer`. Overlay colours 0-15
darken the screen pixel through shade row 20; other colours replace it.

**Mechanism.** Fixed unrolled code: 320 steps of 11 instructions (22 bytes) per row, then
`dec cx; jne` for 200 rows (`mov cx, 0C8h`). It has no patch fields.

```c
/* asm: eax = overlay; pushad/popad */
void xn_draw_fullscreen_overlay_shaded(const u8 *overlay)
{
    u8 *p = screen_buffer;
    int y, x;

    for (y = 0; y < 200; y++)
        for (x = 0; x < 320; x++, p++, overlay++)
            *p = *overlay > 0x0F ? *overlay : xn_shade_table[0x1400 + *p];
}
```

**Group:** CB552 alone. 24 records (the game's 9 call sites: windows over the 3D view).

**Traps.**
- `cmp bh, 0Fh; jbe` is unsigned.
- The screen is assumed 320 wide and contiguous (`inc edi`), not through `xn_gfx_row_offset`.
- `pushad`/`popad`: no register outputs, flags changed. A Watcom caller does not care.

## DRAW-CHECKER: `xn_draw_view_checkerboard` (CD53C)

**What.** The hurt flash: colour F6h on every other pixel of the view, alternating per row.

**Mechanism.** Fixed unrolled code for two rows: 160 `mov [edi+2k], al` and then 160
`mov [edi+2k+1], al`, with `add edi, 140h` after each. The pair count is
`(clip_bottom - clip_top) >> 1`. `shr ecx,1; jb` enters the loop **at the second row** when
the row count is odd: a computed entry, the only one in the engine besides the planted `ret`s.

```c
void xn_draw_view_checkerboard(void)
{
    u32 rows = xn_gfx_clip_bottom - xn_gfx_clip_top;
    u32 pairs = rows >> 1;
    u8 *p = screen_buffer + xn_gfx_row_offset[xn_gfx_clip_top];
    int k;

    if (rows & 1)
        goto odd_row;           /* the asm's `jb` into the loop body */
    do {
        for (k = 0; k < 320; k += 2)
            p[k] = 0xF6;
        p += 320;
odd_row:
        for (k = 1; k < 320; k += 2)
            p[k] = 0xF6;
        p += 320;
    } while (--pairs != 0);
}
```

A `goto` into the loop is the faithful form. The alternative (one odd row before the loop,
then `if (--pairs == 0) return;`) is equally exact; pick whichever the conversion guide
prefers.

**Group:** CD53C alone. 16 records.

**Traps.**
- `pairs` is a u32 counted down with `dec ecx; jne`. For an odd row count, the odd row uses up
  a pair, so rows - 2 rows are drawn: 3 rows draw 1, and 1 row loops 2^32 times. Both are
  original behaviour. The view is never 1 row high, but the C must not "fix" the count.
- Rows are 160 writes at a fixed 320 stride, whatever the clip width.

## DRAW-REMAP320 (C9F4F) and DRAW-PATTERN8 (CD39B), both dead

- **C9F4F:** ecx rows of 320 pixels, `dst[k] = table[src[k]]` (eax = table, al = pixel), src
  and dst advance 320. With `pushad`/`popad` and no caller, it is a plain double loop.
- **CD39B:** ebx rows; each row takes the next 8 bytes of the pattern at eax and repeats them
  40 times across 320 bytes (two dword stores per step). It saves esi, edi and ecx; eax, edx
  and ebx are outputs with no caller ("unknown").

Convert them with differential tests (1 synthetic record each), or leave them as asm. They are
never called.
