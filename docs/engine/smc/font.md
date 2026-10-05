# font: the glyph blitter (0x12DC44)

Rules (PF, KEEP) are in index.md.

## Group FONT-GLYPH: `xn_font_draw_glyph` (12DC44)

**What.** It draws one glyph: 16-bit row bitmaps (`esi`, one word per row, MSB first), width
ebx, at (eax, edx), in `text_colour`, clipped to the clip rectangle. It is called by
`xn_font_select` (12DB50) and `xn_font_draw_string` (12DBCC).

**Mechanism.** Two self-patched operands (rule KEEP):

| Field | Operand | Written | Value |
|---|---|---|---|
| 12DD02 | `shl eax, SKIP` (byte) | 12DC9C: 10h; 12DCBA after a left clip: `bl + 10h` | bits to shift out before the first pixel |
| 12DD10 | `add edi, STEP` (dword) | 12DCEC | `xn_gfx_width - visible width` |

The row loop: `ax = row bits; eax <<= SKIP; repeat cl = width times: add eax,eax / jnc / store`.

**Readable C.**

```c
extern u8  xn_font_glyph_skip;     /* 12DD02 KEEP (a byte) */
extern u32 xn_font_glyph_step;     /* 12DD10 KEEP */

/* asm: eax = x, edx = y, ebx = width, esi = rows; saves eax ebx ecx edx */
void xn_font_draw_glyph(s32 x, s32 y, s32 w, const u16 *rows)
{
    s32 h = font_height, cut;
    u8 skip = 0x10;
    u8 *dst;

    if (x >= xn_gfx_clip_right || y >= xn_gfx_clip_bottom)
        return;
    if (y < xn_gfx_clip_top) {
        cut = xn_gfx_clip_top - y;
        if ((h -= cut) <= 0)
            return;
        rows += cut;
        y = xn_gfx_clip_top;
    }
    if ((cut = y + h - xn_gfx_clip_bottom) > 0 && (h -= cut) <= 0)
        return;
    XN_KEEP(xn_font_glyph_skip, 0x10);      /* 12DC9C: written before the left clip */
    if (x < xn_gfx_clip_left) {
        w -= xn_gfx_clip_left - x;
        if (w <= 0)
            return;                         /* (the 10h above stays written) */
        w = (w & ~0xFF) | (u8)(w + 0x10);   /* add bl, 10h: the asm adds 16 to the WIDTH */
        skip = (u8)w;                       /*   and uses it as the shift (see below) */
        XN_KEEP(xn_font_glyph_skip, skip);  /* 12DCBA */
        x = xn_gfx_clip_left;
    }
    if ((cut = x + w - xn_gfx_clip_right) > 0 && (w -= cut) <= 0)
        return;
    dst = screen_buffer + xn_gfx_row_offset[y] + x;
    XN_KEEP(xn_font_glyph_step, xn_gfx_width - w);
    do {
        u32 bits = (u32)*rows++ << (skip & 31);
        int k;
        for (k = 0; k < (u8)w; k++, dst++) {
            if (bits & 0x80000000u)
                *dst = text_colour;
            bits <<= 1;
        }
        dst += xn_gfx_width - w;
    } while (--h != 0);
}
```

**Group:** 12DC44 alone. Tests:
- 26 own records (51 in all-C mode);
- `xn_font_draw_string` (25) and `xn_font_select` (25) call it.

**Exactness traps.**
- **An original bug to keep.** After a left clip, the asm does `add bl, 10h` on the *remaining
  width* and stores bl as the shift. So the shift is 16 + visible width, not 16 + clipped
  pixels. The visible width then includes 16 extra pixels (the bits shifted in are 0, so they
  draw nothing, but edi advances over them, and the right clip sees the larger width). Copy it
  as in the sketch: `w` really becomes `w + 16` in its low byte.
- The pixel count per row is `cl = bl` (the low byte of the width), counted with
  `dec cl; jne`. A width whose low byte is 0 would loop 256 times. The C uses `(u8)w`.
- `shl eax, imm8`: the CPU masks the count to 5 bits. The C masks too (`& 31`).
- 12DD02 is a byte field. Writing a dword would overwrite `add eax, eax` after it.
- `font_height` (12DA44) is read every call. There are 4 NOPs at 12DC98 (TASM's padding), no
  behaviour.
