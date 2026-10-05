# shade: distance fog (0x14D200-0x150133)

The fog is a per-span post-pass. After each span routine, `xn_render_frame` calls
`[xn_render_span_hook]` (CEA58) with ebx = poly, esi = span node, ebp = n and
edi = destination - 1. With fog on, the hook is `xn_shade_fog_span` (150040). It finds the part
of the span beyond the fog start, computes the fog level at both ends, and remaps those pixels
through a 64-level fog table. The remap is a 641-step unrolled loop stopped by a planted `ret`.
These functions hold 3,409 instructions, almost all of them in that loop.

Other shade functions (`xn_shade_load` 136B88, the table builders CB300/C9EA7/C9EB2/CDC4B)
have no self-modifying code, except that `xn_shade_load` writes the light templates' clamp
fields (light.md, LIGHT-SHADERS). The full-screen overlay CB552 uses the shade table but is a
2D blit (draw.md).

Rules (PF, KEEP, RET, DIV ...) are in index.md.

## Group SHADE-FOG-SETUP: `xn_shade_set_fog` (14D23C)

**What.** It sets the fog for the frames to come. Called from the game's `update_fog` with
eax = the fog start (in z >> 8 units), or -1 for no fog.

**Mechanism: patch fields written** (all in 150040; one writer, ten fields, four values):

| Value | Fields (operand) | Meaning |
|---|---|---|
| `xn_fog_table_last` (D_0014CC0C) | 1500DF (`add ecx, T`), 15011C (`lea ecx, [eax+T]`) | pointer to the last 256-byte level of the fog table |
| `3F00h / (far>>8 - start)` | 15009B, 1500BC, 150103 (`mov edx, S`) | the fog step (also stored in `xn_fog_step` D_0014CC14) |
| `2^32 / start` | 150056 (`sub ecx, I`), 150071 (`mov ecx, I`), 1500FA (`cmp ecx, I`) | 1/z of the fog start, in the span lists' 2^40/z units |
| `start * step` | 1500D7 (`sub ecx, K`), 15010A (`sub eax, K`) | the level at the fog start |

It also sets `xn_fog_start` (CC10), `xn_fog_inv_far` (CC18 = 2^40/far) and the hook
(150040, or 14D300, a lone `ret`, when start = -1 or start >= far >> 8).

**Readable C** (rule PF: the fields are variables; every copy is written, because the records
see all ten):

```c
/* fog.c */
extern u8 *xn_fog_table_last_a, *xn_fog_table_last_b;          /* 1500DF 15011C */
extern u32 xn_fog_step_a, xn_fog_step_b, xn_fog_step_c;         /* 15009B 1500BC 150103 */
extern u32 xn_fog_inv_start_a, xn_fog_inv_start_b, xn_fog_inv_start_c; /* 150056 150071 1500FA */
extern s32 xn_fog_start_step_a, xn_fog_start_step_b;            /* 1500D7 15010A */

/* asm: eax = start (pushad: preserves everything) */
void xn_shade_set_fog(s32 start)
{
    s32 range;
    u32 step, inv_start;

    if (start != -1) {
        xn_fog_start = start;
        range = (s32)(xn_cam_far_z >> 8) - start;
        if (range > 0) {
            xn_fog_table_last_a = xn_fog_table_last_b = xn_fog_table_last;
            step = xn_udiv64(0, 0x3F00, range);         /* edx:eax = 3F00h / range */
            xn_fog_step_a = xn_fog_step_b = xn_fog_step_c = xn_fog_step = step;
            inv_start = xn_udiv64(1, 0, start);         /* 2^32 / start */
            xn_fog_inv_start_a = xn_fog_inv_start_b = xn_fog_inv_start_c = inv_start;
            xn_fog_start_step_a = xn_fog_start_step_b = start * step;
            xn_fog_inv_far = xn_udiv64(0x100, 0, xn_cam_far_z);   /* 2^40 / far */
            xn_render_span_hook = xn_shade_fog_span;
            return;
        }
    }
    xn_render_span_hook = xn_shade_fog_span_off;        /* 14D300 */
    xn_fog_start = xn_cam_far_z;
}
```

`xn_udiv64(hi, lo, d)` is a one-instruction `div` pragma (rule DIV: a real `div`, so a zero
divisor faults into the engine's handler exactly as the asm does). Note that
`range = (far >> 8) - start` uses `shr` (unsigned) for far.

**Tests:**
- 29 own records;
- the game's `update_fog` calls it per frame;
- 150040's 29 records read the fields the setup wrote.

**Traps.**
- `start = 0` divides by zero in `2^32 / start` (#DE, the handler gives 0). Keep the real
  `div`.
- When fog is off, `xn_fog_start` is set to `xn_cam_far_z` (not `>> 8`). Copy it.
- The hook is data (CEA58), not code: a plain function-pointer write.

## Group SHADE-FOG: `xn_shade_fog_span` (150040) and `xn_shade_fog_pixels` (14D320)

**What.** It fogs one span: the pixels beyond the fog start get
`fog_row(level)[pixel]`, with the level interpolated in 1/z along the span.

**Mechanism.**
- **Unrolled body** 14D320: 641 steps of 5 instructions, 18 bytes a step. Pixel k is at
  `[edi+100h+k]` (the hook first does `edi -= 0FFh`, so k = 0 is the span's first pixel):

  ```
  mov eax, ecx ; mov al, [edi+100h+k] ; add ecx, edx ; mov al, [eax] ; mov [edi+100h+k], al
  ```

  ecx is the fog-table pointer: a row address with an 8-bit fraction in its low byte, which
  the pixel replaces. edx is the per-pixel step.
- **Planted ret.** 150040 plants `C3` at `14D320 + xn_fog_ret_offsets[n]` (D_0014CC1C, 640
  words, = 18n) and calls 14D320 at its start. It restores `8Bh` from two sites (the sloped
  path 1500E3/1500EF, the constant-depth path 150120/15012C).
- **Patch fields read:** the ten fields above (rule PF).
- **The sloped path:**
  - if the span starts nearer than the fog start, it skips the first
    `-hi32(inv_slope * (inv_z - inv_start))` pixels;
  - if it ends nearer, it clips n;
  - it clamps 1/z to `xn_fog_inv_far` at both ends;
  - it computes `level = (step << 32) / inv_z` at both ends (two `div`);
  - the step is `-hi32((level1 - level0) * xn_recip32_table[n])`.

  The constant-depth path (poly `+60h` = 0) uses one level for the whole span.

**Readable C.**

```c
/* 14D320: the remap the asm unrolls 641 times and stops with a planted ret */
static void fog_run(u8 *pix, int n, u32 row, s32 step)
{
    int k;
    for (k = 0; k < n; k++) {
        pix[k] = *(const u8 *)((row & ~0xFFu) | pix[k]);
        row += step;
    }
}

/* asm: ebx = poly, esi = span node, ebp = n, edi = destination - 1 */
void xn_shade_fog_span(const xn_poly *poly, const xn_span *span, s32 n, u8 *dst)
{
    u32 inv_z = span->inv_z;
    s32 inv_slope = poly->inv_dzdx;             /* 2^32 / d(1/z)/dx */
    u8 *pix = dst + 1;
    s32 d, h;
    u32 level0, level1, inv_end;

    if (inv_slope == 0) {                       /* the same depth along the span */
        if ((s32)inv_z >= (s32)xn_fog_inv_start_c)
            return;
        level0 = xn_udiv64(xn_fog_step_c, 0, inv_z);
        fog_run(pix, n, (u32)xn_fog_table_last_b + xn_fog_start_step_b - level0, 0);
        return;
    }
    d = (s32)(inv_z - xn_fog_inv_start_a);
    if (d > 0) {                                /* starts nearer than the fog */
        if (inv_slope >= 0)
            return;
        h = xn_mulhi(inv_slope, d);             /* -(pixels before the fog starts) */
        n += h;
        if (n <= 0)
            return;
        pix -= h;
        inv_z = xn_fog_inv_start_b;
    } else if (inv_slope > 0) {                 /* leaves the fog before its end */
        h = -xn_mulhi(inv_slope, d);
        if (n > h)
            n = h;
    }
    if ((s32)inv_z < (s32)xn_fog_inv_far)
        inv_z = xn_fog_inv_far;
    level0 = xn_udiv64(xn_fog_step_a, 0, inv_z);
    inv_end = poly->dzdx * n + inv_z;
    if ((s32)inv_end < (s32)xn_fog_inv_far)
        inv_end = xn_fog_inv_far;
    level1 = xn_udiv64(xn_fog_step_b, 0, inv_end);
    fog_run(pix, n, (u32)xn_fog_table_last_a + xn_fog_start_step_a - level0,
            -xn_mulhi((s32)(level1 - level0), xn_recip32_table[n]));
}
```

Each read uses the field the asm reads at that point (a/b/c). They always hold the same value,
because `xn_shade_set_fog` is their only writer and writes all copies together. So a converter
may collapse them into one name per value. The records cannot tell the difference.

**Conversion group:** 150040 and 14D320 (rule RET).
- 14D320 never becomes C on its own. To test `fog_run` against 14D320's own 29 records, use
  the adapter `xn_planted_count(0x14D320, 18, 641)`, with ecx = row, edx = step,
  edi = pixel - 100h.
- 14D300 (the empty hook) is trivial C.
- Tests:
  - 150040: 29 records (57 in all-C mode);
  - 14D320: 29 records (62 in all-C mode);
  - `xn_render_frame` (28) runs the hook after every span.

**Exactness traps.**
- `(step << 32) / inv_z` is a 64/32 `div`. Overflow (step >= inv_z) or zero faults, and the
  handler returns 0 and 0. Use the real `div` (rule DIV).
- Signed compares throughout (`jle`, `jge` after `sub`/`cmp`), including the clamp against
  `xn_fog_inv_far`.
- `n` is clipped and `pix` moved before the planted offset is read: the offset is for the
  clipped n.
- `xn_fog_ret_offsets` has 640 entries. An n of 640 or more would read past the table (an
  offset of 0: the `ret` at the entry, nothing drawn). Spans are at most 320 px, so this cannot
  happen, but the readable C should document it rather than "fix" it.
- No register outputs on the render path (`xn_render_frame` reloads everything after the
  hook).

## Order

SHADE-FOG-SETUP first (one writer, 29 records), then SHADE-FOG. The fog pair is the easiest
planted-ret pair in the engine to test (a 1-line body, 29 + 29 records, and frequent in play).
That makes it the right pilot for the RET adapter.
