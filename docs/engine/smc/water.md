# water: the dungeon water post-process (0x12F500-0x132E75)

When the eye is not at `dungeon_water_level`, `xn_water_draw` (12F79C) re-draws the pixels
behind the water plane. Each such pixel is taken from a horizontally jittered position on the
same row (a per-row ripple) and mapped through the water tint table. The pixel loop is a
641-step unrolled body stopped by a planted `ret`. These functions hold 2,894 instructions.

Rules (PF, KEEP, RET, DIV ...) are in index.md.

## Group WATER-DRAW: `xn_water_draw` (12F79C)

**What.**
1. It projects the water line (`xn_water_clip_extent` 12F59C) and finds the screen rows it
   covers.
2. It steps 1/z of the water plane down those rows.
3. Per row, it advances the ripple and walks the row's span list. For each span (or part of
   one) farther than the water plane at that row, it calls `[xn_water_span_fn]` (12DE04, =
   12F9A0) with:
   - ebx = x start, ebp = x end;
   - edi = row - 100h;
   - edx = the row's water 1/z;
   - esi = the span node, eax = poly `+60h`.

**Mechanism.** One self-patched field:

| Field | Operand | Written at | Value |
|---|---|---|---|
| 12F976 | `add edx, STEP` | 12F87A, once per call | `(inv_bottom - inv_top) / rows` (`idiv ecx`), the per-row step of the plane's 1/z |

The row loop reads it once per row. No other function reads it.

**Readable C** (rule KEEP: a local, plus one store for the records):

```c
extern s32 xn_water_row_inv_step;              /* 12F976 KEEP */

void xn_water_draw(void)
{
    s32 top, bottom, rows, inv_z, step, y;
    ...                                         /* clip extent, the two 2^8 divides, swap */
    step = (xn_water_inv_b - xn_water_inv_a) / rows;    /* idiv: cdq; idiv ecx */
    XN_KEEP(xn_water_row_inv_step, step);
    inv_z = xn_water_inv_a;
    xn_water_row = top;
    for (y = top; rows-- != 0; y++, inv_z += step) {
        u8 *row = screen_buffer + xn_gfx_row_offset[y] - 0x100;
        int r = xn_water_row++;
        s32 ph = xn_water_row_phase[r] + xn_water_row_velocity[r];

        if (ph > 4)  xn_water_row_velocity[r] = -xn_water_row_velocity[r];
        if (ph < -4) xn_water_row_velocity[r] = -xn_water_row_velocity[r];
        xn_water_row_phase[r] = ph;
        xn_water_row_jitter = &xn_water_jitter[ph + xn_water_row_base[r]];   /* lea [eax*4+12DA18h] */
        water_row_spans(&xn_render_span_rows[y], row, inv_z);   /* the 12F8FA-12F96D walk */
    }
}
```

The row loop is `dec ecx; jne` with ecx = rows (> 2, checked before), so the C's
`rows-- != 0` matches. The span walk calls the span routine through `xn_water_span_fn`, a data
pointer. In C call `xn_water_span` directly only if nothing writes 12DE04; the census found no
writer, but read it the asm's way (through the pointer) to stay safe.

**Tests:**
- 24 own records;
- the game's `world_render` calls it in dungeons with water;
- 12F9A0 has 24 records and 12F9E0 has 12.

**Traps.**
- The two `div [12F482]` / `div [12F492]` (`2^8 * 2^32 / z`) overwrite the clip extent's z
  values with 1/z in place. These are memory writes.
- `xchg [12F482], eax` swaps the stored pair when the line is upside down.
- `lea eax, [eax*4 + D_0012DA18]` addresses the jitter table at 12DE18 (the code reads
  `[esi+400h]`). Name the base `xn_water_jitter - 0x100` in C, not "the FONT string" the asm
  label suggests.
- The ripple update writes `xn_water_row_velocity`/`phase` per row, even when no span of that
  row is drawn.

## Group WATER-SPAN: `xn_water_span` (12F9A0) and `xn_water_span_unrolled` (12F9E0)

**What.** It re-draws pixels x_start..x_end-1 of one row:
`dst[k] = tint_row[dst[k + jitter[x+k]]]`.

**Mechanism.**
- Unrolled body 12F9E0: 641 steps of 4 instructions, 21 bytes a step. Pixel k reads the
  jitter dword at `[esi+400h+4k]` and the source pixel at `[edi+ebx+100h+k]`, maps it through
  eax (the tint table, al = pixel), and stores to `[edi+100h+k]`.
- The planter 12F9A0 does:
  1. n = ebp - ebx (returns if n <= 0);
  2. `edi += ebx`;
  3. `bp = xn_water_unroll_offsets[n]` (12EF78, 641 words, = 21n; only bp is written, and
     ebp's top half is 0 because n < 65536);
  4. eax = `xn_water_tint_table`;
  5. esi = `xn_water_row_jitter + 4 * x_start`;
  6. plants `C3`, calls, restores `8Bh`.
- The same offsets table serves `xn_span_flat_transparent_shaded`'s tail (span.md), which
  also has 21-byte steps.

**Readable C.**

```c
/* 12F9E0: unrolled x641, stopped by a planted ret */
static void water_run(u8 *pix, int n, const s32 *jit, u32 tint)
{
    int k;
    for (k = 0; k < n; k++)
        pix[k] = *(const u8 *)((tint & ~0xFFu) | pix[k + jit[k]]);
}

/* asm: ebx = x start, ebp = x end, edi = row - 100h; returns ebp (the planted offset, an
   artefact: no caller reads it; check the ABI row) */
void xn_water_span(s32 x0, s32 x1, u8 *row)
{
    s32 n = x1 - x0;

    if (n <= 0)
        return;
    water_run(row + 0x100 + x0, n,
              (const s32 *)((u8 *)xn_water_row_jitter + 0x400) + x0, xn_water_tint_table);
}
```

**Conversion group:** 12F9A0 and 12F9E0 (rule RET).
- Adapter for 12F9E0's own 12 records: `xn_planted_count(0x12F9E0, 21, 641)`, with
  esi = jitter - 400h, edi = pixel - 100h, eax = tint.
- WATER-DRAW calls through the pointer, so it can be converted before or after this pair.

**Exactness traps.**
- **Order matters.** `water_run` reads `pix[k + jit[k]]`, which may be a pixel this run has
  already rewritten (a jitter of -1 reads the pixel just tinted, so it is tinted twice). The C
  must go left to right, one pixel at a time, exactly as the unrolled code does.
- eax's top bits are the tint table address. `al` is replaced per pixel, so only bits 8-31 of
  `xn_water_tint_table` matter (it is 256-aligned in practice). Copy the `& ~0xFF` form.
- The table has 641 entries, so n = 641 would read past it (an offset of 0: nothing drawn).
  This cannot happen, because a row is at most 320 px.
- The ABI row lists outputs eax and ebp: the planter leaves the tint in eax and the planted
  offset in ebp. `xn_water_draw` overwrites both before reading them (12F930, 12F941). The
  clobber test should confirm that, and then the C returns nothing.
