# sky: the rain streak (0xC9D19-0xC9EA6)

Rules (RET ...) are in index.md.

## Group SKY-RAIN: `xn_sky_draw_rain_streak` (C9D19) and `xn_sky_draw_rain_streak_bottom_clip` (C9D63)

**What.** It draws one rain streak: a vertical run of up to 30 pixels in the gradient colours
`xn_rain_streak_colours` (C7DF1), clipped to the clip rectangle's top and bottom.
`xn_sky_draw_rain` (C9CB9) calls it 50 times a frame with eax = x and edx = y.

**Mechanism.** This is the only planted `ret` in the engine that is not a return.
- The unrolled body (C9D89-C9E9A) has 31 steps of 2 instructions, 9 bytes a step:
  `mov al,[esi+1+k]; mov [edi+100h+140h*k], al`. One pixel per screen row.
- The planter (C9D6C):
  1. pushes the address C9EA0;
  2. saves the byte at `C9D89 + 9n` in bl;
  3. plants `C3` there;
  4. falls into the body.

  The planted `ret` therefore "returns" to C9EA0, which restores bl and returns for real.
  Without a planted `ret` (n = 31), the body runs on into C9EA0 itself.
- Two entries share the body:
  - C9D19 (the live one) clips at the top, or rejects when y + 30 is at or past the clip
    bottom;
  - C9D63, which has no caller of its own, shortens n by the overhang (`sub edx, clip_bottom;
    inc edx; sub ecx, edx`) and is reached only by falling into C9D6C.

  C9D19's `jl C9D6C` jumps into C9D63's code, past C9D63's own three instructions. C9D19
  itself returns at C9D62 when the streak would reach the clip bottom. So C9D63's
  bottom-clip entry is reachable only from outside, and nothing calls it.

**Readable C.**

```c
static void rain_run(u8 *dst, int n, const u8 *colours)     /* C9D89, unrolled x31 */
{
    int k;
    for (k = 0; k < n; k++, dst += 320)
        *dst = colours[k];
}

/* asm: eax = x, edx = y */
void xn_sky_draw_rain_streak(s32 x, s32 y)
{
    s32 n = 30;
    const u8 *colours = xn_rain_streak_colours;
    u8 *dst;

    if (y < xn_gfx_clip_top) {
        colours -= y - xn_gfx_clip_top;           /* skip the clipped-off top */
        n += y - xn_gfx_clip_top;
        y = xn_gfx_clip_top;
    } else if (y + 30 >= xn_gfx_clip_bottom)
        return;
    dst = screen_buffer + xn_gfx_row_offset[y] + x;
    rain_run(dst, rain_count(n), colours);
}
```

`rain_count(n)` reproduces the planted position, n in the asm's sense:
- 0 <= n <= 31 draws n pixels;
- **n < 0** plants the `ret` before the body (inside C9D19's own code, already executed), so
  the whole body runs: 31 pixels;
- n > 31 plants it after C9EA0, also 31 pixels.

```c
static int rain_count(s32 n) { return (n < 0 || n > 31) ? 31 : n; }
```

That n < 0 case happens when a streak starts more than 30 rows above the clip top. Then 31
pixels are drawn from the clip top with the colour pointer moved back past the gradient: a
latent original bug. `xn_sky_draw_rain` picks y in the view, so it may never happen. The C
must keep it, and a differential test with y = clip_top - 40 checks it.

**Conversion group:** C9D19, C9D63 and the body (rule RET; the body is not a function).
- C9D63 gets its own small C function for its (synthetic) records:
  `n -= y - clip_bottom + 1`, then the same run.
- Tests:
  - C9D19: 25 records (50 in all-C mode);
  - C9D63: 2 (synthetic);
  - `xn_sky_draw_rain` (C9CB9) on rainy frames.

**Exactness traps.**
- The saved and restored byte is exact (bl), so there is no net code write. The C writes no
  code.
- The pushed address C9EA0 and the call frame are below the stack pointer at the return:
  dead, and not compared.
- ABI: C9D19 is Watcom (eax, edx in; everything clobbered), but it lowers ESP by 4 at the
  return through C9EA6. The asm "returns" with the pushed C9EA0 popped by the planted `ret`
  and then pops its real return address, so ESP is balanced. The ABI note "ESP -4 at the
  return at C9EA6" is about C9EA6 seen alone. Check that the clobber test agrees before
  trusting either.
