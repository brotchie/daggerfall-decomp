# span: the span routines (0x155800-0x158068)

The span routines draw one run of pixels of one polygon on one screen row. `xn_render_frame`
(12A870) calls `[poly+3Ch]` for every span node; flats call `[flat+3Ch]` from
`xn_flat_span_emit` (15526C). Every routine here uses at least one of the mechanisms, and
together they hold 3,157 instructions, most of them unrolled.

The rules this file refers to (PF, KEEP, RET, UNR, GEN, DIV) are defined in `index.md`, under
"Design rules". The sketches use the provisional structs `xn_poly` and `xn_span`, also defined
in `index.md`. Rename the fields to match `src/engine/xnstruct.h` when that
header lands.

## The family in one picture

| Group | Entry (setup -> routine) | Px per block | Block loop | Tail (planted `ret`) | Step bytes | Offsets table |
|---|---|---|---|---|---|---|
| SPAN-SOLID-LIT | 155920 | 16 (8 words) | `cmp edi,end` | 155880 | 9 | `xn_span_solid_lit_tail_offsets` 155860 |
| SPAN-TEX | 155D20 (8) / 155F60 -> 155F70 (16) | 8 / 16 | `cmp edi,end` | 155C00 (shared) | 17 | `xn_span_tail_offsets_17` 156D20 |
| SPAN-TEX-SHADED | 156460 (8) / 1566B4 -> 1566C4 (16) | 8 / 16 | `cmp edi,end` | 156320 (shared) | 19 | `xn_span_tail_offsets_19` 157600 |
| SPAN-TEX-LIT | 156A80 -> 156A94 | 16 (a tmap copy) | `cmp edi,end` | the tmap copy in the heap | 28/24 | `xn_tmap_ret_offsets` 156A00 (dwords) |
| SPAN-TEX64 | 156E60 | 16 | `dec ebp` | 156D40 | 17 | `xn_span_tail_offsets_17` 156D20 (shared with SPAN-TEX) |
| SPAN-TEX64-SHADED | 157240 | 16 | `cmp edi,end` | 157100 | 19 | `xn_span_tail_offsets_19` 157600 |
| SPAN-FLAT-TRANSPARENT | 157620 | 8 | `dec ebp` | 157760 | 19 | 157600 |
| SPAN-FLAT-SHADED | 157800 | 8 | `dec ebp` | 157960 | 21 | `xn_water_unroll_offsets` 12EF78 (the water's table) |
| SPAN-FLAT-FOGGED | 157B20 | 8 | `dec ebp` | 157CA0 | 23 | `xn_span_tail_offsets_23` 157B00 |
| SPAN-FLAT-TRANSLUCENT | 157E20 | 8 | `dec ebp` | 157FA0 | 25 | `xn_span_tail_offsets_25` 157E00 |

Every routine has the same shape:

1. A perspective divide at the start of the span. The 1/z at the first pixel is
   `span->inv_z`; z is `recip[(inv_z >> 13) & 0xFFFF] << 9`, read from the 1/z table, whose
   address `xn_render_init` patches into 25 `[ecx*4 + table]` operands.
2. Blocks of 8 or 16 pixels. Each block divides again at its end and fills the pixels between
   affinely, with fully unrolled code (rule UNR).
3. A last partial run of 1-15 pixels: an unrolled 16-step (or 8-step) body. It is entered at its
   first instruction and stopped by a `ret` planted at `body + offsets[n]` (rule RET). The
   routine then restores the opcode byte (`8Bh`, or `8Ah` for 155880, or `0Fh` for the tmap
   copy).

The span routines patch their own operands per call: the block-loop end
(`cmp edi, end`, written three times with a `nop` between them, a prefetch-queue flush on a real
486), the 8/16-px steps of 1/z, u/z and v/z, and the packed texture origin. Other routines
patch some operands:
- `xn_render_init` (12A100): the 1/z table address, 25 fields;
- `xn_cam_set_view_window` (12A2D0): centre x - 1 in the four flat routines;
- `xn_cam_update_derived` (12A3AC): the 16-px x-ray step in the lit routines;
- `xn_render_frame` (12A870): the per-row y ray in the lit routines, six fields.

### Facts that hold for the whole family (checked)

- **Every planted `ret` lands on a step's first byte, and the restore puts back that byte**
  for every n the routines can produce (1..15, or 1..7). `scratch/retcheck.py` checks it
  against the tables and the code bytes. So a call leaves the code bytes as they were, and the
  C never has to write them (rule RET).
- No routine enters a body at a computed offset. Every body starts at its first instruction;
  the planted `ret` alone sets the count.
- `xn_render_frame` saves and restores every register around `call [poly+3Ch]`, so on the
  render path the routines have no register outputs. On the flat path,
  `xn_flat_span_emit` restores ecx and edi. The ABI rows of the family say "partial" callers
  (they are reached through a pointer): settle the outputs with the infra's clobber test, not
  by eye. The CF flags_out on 157E20/157FA0 is that conservatism, not a real output.
- The routines take ebp (the count) as an input. The infra's pilot already passes an ebp input
  through glue (src/engine/xmat.h), so each routine gets a `NAME_r(xn_regs *)` glue that calls
  the natural C function below.

## SPAN-TEX: `xn_span_tex_8` (155D20), `xn_span_tex_16` (155F60/155F70), tail 155C00

**What it computes.** It draws n pixels of a textured polygon: perspective-correct texel
coordinates every 8 (or 16) pixels, affine in between, with no shading.

**Mechanism.**
- Unrolled block: 8 pixels (4 word stores, 13 instructions per pixel pair, 32 bytes per pair)
  in 155D20, and 16 pixels (8 pairs) in 155F70.
- Tail 155C00: 16 steps of 7 instructions, 17 bytes a step, storing `[edi+1]`..`[edi+16]`.
  `xn_span_tex_8` uses n = 1..7 of it, and `xn_span_tex_16` uses n = 1..15. Both plant at
  `155C00 + xn_span_tail_offsets_17[n]` (= 17n) and restore `8Bh` (`mov edx, ebx`).
- Self-patched fields:

  | Field (8 / 16 px) | Operand | Written with | Read by |
  |---|---|---|---|
  | 155DC7 / 156017 | `add ecx, dz` | `poly->dz8` (+58h) | the block loop |
  | 155DD1 / 156021 | `add [v_num], dv` | `poly->dv8` (+54h) | the block loop |
  | 155DDB / 15602B | `add [u_num], du` | `poly->du8` (+50h) | the block loop |
  | 155E2B / 15607B | `add ebx, origin` | `poly->tex_origin` (+18h) | the block loop |
  | 155EBD / 15618D | `cmp edi, end` | `dst + (n & ~7)` / `dst + (n & ~15)` | the block loop |

- Fields written by others: 155D34, 155DEC, 155EDD / 155F84, 15603C, 1561AD, the 1/z table
  (`xn_render_init`).
- Data globals in the code page: `xn_span_tex_u_num` / `v_num` / `u_prev` / `v_prev`
  (155B00-155B0C). They are real variables and memory writes, and the C keeps them as globals.
- `xn_span_tex_16_setup` (155F60) doubles `poly->du8`/`dv8`/`dz8` once and stores 155F70 into
  `poly->span_fn`, then falls into 155F70. The "16" routine is the "8" routine with doubled
  steps, `>> 4` instead of `>> 3`, and a 16-pixel block.

**Readable C.** One function per routine. The unrolled pixels become `tex_run`, shared by the
blocks and the tail.

```c
/* span.c */
#include "xspan.h"

/* the 1/z table, patched into each routine by xn_render_init (rule PF: read the field) */
extern const u32 *xn_span_tex8_recip_a, *xn_span_tex8_recip_b, *xn_span_tex8_recip_c;
/* the routine's own operands (rule KEEP: stored once, for the records) */
extern s32 xn_span_tex8_dz, xn_span_tex8_dv, xn_span_tex8_du;
extern u32 xn_span_tex8_origin;
extern u8 *xn_span_tex8_end;
/* real variables in the code page */
extern s32 xn_span_tex_u_num, xn_span_tex_v_num, xn_span_tex_u_prev, xn_span_tex_v_prev;

/* texel offset of a packed coordinate: v integer in bits 24-31, u integer in bits 8-15 */
#define XN_TEXEL(uv)    ((((uv) >> 16) & 0xFF00) | (((uv) >> 8) & 0xFF))

/* n affinely stepped texels: the body the asm unrolls (blocks) or stops with a ret (tail) */
static void tex_run(u8 *dst, int n, u32 uv, u32 step, u32 mask, const u8 *texels)
{
    int k;

    for (k = 0; k < n; k++) {
        u32 ofs = XN_TEXEL(uv);
        uv = (uv + step) & mask;
        dst[k] = texels[ofs];
    }
}

/* z at a 1/z value, through the 1/z table */
#define XN_Z(tab, inv_z)    ((s32)((tab)[((inv_z) >> 13) & 0xFFFF] << 9))

/* packed step: d(v) in the top half (8.8, >> 3 for 8 px), d(u) >> 3 in the bottom half */
static u32 tex_step8(s32 dv, s32 du)
{
    return ((u32)dv << 13 & 0xFFFF0000) | ((u32)(du >> 3) & 0xFFFF);
}

/* asm: eax = poly, esi = span, ebx = x - centre x, ebp = n, edi = destination - 1 */
void xn_span_tex_8(xn_poly *poly, const xn_span *span, s32 x, s32 n, u8 *dst)
{
    u32 inv_z = span->inv_z;
    s32 z = XN_Z(xn_span_tex8_recip_a, inv_z);
    u32 mask = poly->wrap_mask;
    s32 u, v;
    u32 uv;

    xn_span_tex_v_num = x * poly->v_dx + xn_render_row_y * poly->v_dy + poly->v_0;
    xn_span_tex_v_prev = xn_mulhi(xn_span_tex_v_num, z);
    xn_span_tex_u_num = x * poly->u_dx + xn_render_row_y * poly->u_dy + poly->u_0;
    xn_span_tex_u_prev = xn_mulhi(xn_span_tex_u_num, z);
    if (n >= 8) {
        u8 *end = dst + (n & ~7);

        XN_KEEP(xn_span_tex8_dz, poly->dz8);
        XN_KEEP(xn_span_tex8_dv, poly->dv8);
        XN_KEEP(xn_span_tex8_du, poly->du8);
        XN_KEEP(xn_span_tex8_origin, poly->tex_origin);
        XN_KEEP(xn_span_tex8_end, end);
        do {
            inv_z += poly->dz8;
            xn_span_tex_v_num += poly->dv8;
            xn_span_tex_u_num += poly->du8;
            z = XN_Z(xn_span_tex8_recip_b, inv_z);
            v = xn_mulhi(z, xn_span_tex_v_num);
            u = xn_mulhi(z, xn_span_tex_u_num);
            uv = ((u32)xn_span_tex_v_prev << 16 | ((u32)xn_span_tex_u_prev & 0xFFFF))
                 + poly->tex_origin;
            tex_run(dst + 1, 8, uv & mask, tex_step8(v - xn_span_tex_v_prev,
                    u - xn_span_tex_u_prev), mask, poly->texels);
            xn_span_tex_v_prev = v;
            xn_span_tex_u_prev = u;
            dst += 8;
        } while (dst != end);
        n &= 7;
        if (n == 0)
            return;
    }
    /* the last run: the end of an 8-px block, n pixels of it (the asm: planted ret) */
    z = XN_Z(xn_span_tex8_recip_c, inv_z + poly->dz8);
    v = xn_mulhi(z, xn_span_tex_v_num + poly->dv8);
    u = xn_mulhi(z, xn_span_tex_u_num + poly->du8);
    uv = ((u32)xn_span_tex_v_prev << 16 | ((u32)xn_span_tex_u_prev & 0xFFFF)) + poly->tex_origin;
    tex_run(dst + 1, n, uv & mask, tex_step8(v - xn_span_tex_v_prev, u - xn_span_tex_u_prev),
            mask, poly->texels);
}
```

`xn_span_tex_16` is the same with 16-pixel blocks, `(u32)dv << 12 & 0xFFFF0000` and
`du >> 4`, in the blocks and in the last run (1561DD: `shl ecx,0Ch; sar edx,4`). Its last run
also ends at the 16-px boundary (`inv_z + poly->dz8`, with dz8 already doubled), so n pixels of
a 16-px interpolation are drawn. Give `tex_step` the shift as a parameter (3 or 4) rather than
writing two helpers.

**Conversion group.** 155D20, 155F60, 155F70 and 155C00 are one group. The body 155C00
becomes `tex_run` and is never routed. To test the C body against 155C00's own 26 records,
route 155C00 through the RET adapter (`xn_planted_count(0x155C00, 17, 16)`, then `tex_run`
with eax = mask, ecx = step, ebx = uv, esi = texels, edi = dst - 1). If the adapter is not
built, 155C00 stays asm and its records test nothing new.

Records:
- own records: 155D20 24, 155F70 26, 155C00 26;
- callers: `xn_render_frame` 12A870 (28), which runs them through `[poly+3Ch]`.
- In all-C mode, 155C00 ran in 115 records.

**Exactness traps.**
- `xn_span_tex_u_num`..`v_prev` are memory writes. In the tail they are read, never written,
  and the u/v numerators are advanced only in registers there.
- The KEEP stores: five fields per call. The three stores of `end` are one store in C.
- `sar edx, 3` is a signed shift. Watcom's `>>` on `s32` is arithmetic; keep the operands
  signed.
- `xn_mulhi` is `imul` (signed 64-bit, high dword). The infra's xngine.h has it.
- No flags out, no port I/O.

## SPAN-TEX-SHADED: `xn_span_tex_shaded_8` (156460), `_16` (1566B4/1566C4), tail 156320

As SPAN-TEX, but every texel goes through the polygon's shade row: `poly->shade` (+14h), a
pointer into the shade table, kept in `xn_span_tex_shaded_row` (156310). The pixel is
`row[texel]`, where row is the shade value with its low byte replaced by the texel
(`mov al, [esi+edx]; mov al, [eax]`).
- Tail 156320: 16 steps of 8 instructions, 19 bytes a step; restore `8Bh`;
  `xn_span_tail_offsets_19`.
- Its own fields are the five of SPAN-TEX at 156510/15651A/156524/156577/156625 (8 px) and
  156774/15677E/156788/1567DB/156921 (16 px).
- The 1/z table fields are 15647D/156535/156645 and 1566E1/156799/156941.

```c
static void tex_run_shaded(u8 *dst, int n, u32 uv, u32 step, u32 mask, const u8 *texels,
                           u32 row)
{
    int k;

    for (k = 0; k < n; k++) {
        u32 ofs = XN_TEXEL(uv);
        uv = (uv + step) & mask;
        dst[k] = *(const u8 *)((row & ~0xFFu) | texels[ofs]);
    }
}
```

**Group:** 156460, 1566B4, 1566C4 and 156320. Adapter: `xn_planted_count(0x156320, 19, 16)`.

Records:
- own records: 156460 24, 1566C4 28, 156320 28;
- 156320 ran in 139 records in all-C mode.

**Traps.**
- The 8-px tail plants with `eax` as the offset register. eax is saved across the call with
  `push eax`, while the shade row is passed in eax (156692-1566AA).
- Only bits 8-31 of `row` matter: `mov al, texel` replaces the low byte.

## SPAN-TEX64: `xn_span_tex64` (156E60) and tail 156D40; SPAN-TEX64-SHADED: 157240 and tail 157100

These are the terrain's 64x64 textures, with a fixed wrap mask `3F3Fh`.

**Mechanism.**
- Block: 16 pixels (8 pairs, 13 instructions per pair, 39 bytes per pair). The count is in
  ebp (`ebp = n >> 4`) and the loop ends on `dec ebp`.
- Self-patched steps: 156EE9/156EEE/156EF4, with `poly->dz8`/`du8`/`dv8`, all written from
  ebp. 156F19 is the packed origin (`poly->tex_origin`).
- 157240 also self-patches the shade row 15730B (`poly->shade`) and the loop end 157486
  (written once, not three times).
- Tails:
  - 156D40: 7 instructions per step, 17 bytes, sharing `xn_span_tail_offsets_17` with SPAN-TEX.
  - 157100: 8 instructions per step, 19 bytes, sharing `xn_span_tail_offsets_19`.
- The tail divides through the reciprocal tables:
  - `imul [n*4 + xn_recip32_table]` for u;
  - `imul [n*4 + xn_recip16_table]` for v.

  The step over the last n pixels is the exact difference over n (not over 16).

The u step is sign-extended from 12 bits, in the 16-px block (`sar ecx,4; shl cx,4; sar cx,4`).
That C must be:

```c
/* the 16-px packed step of the 64x64 routines: v in the top half, u (12-bit signed) below */
static u32 tex64_step16(u32 end_uv, u32 uv)
{
    u32 d = (u32)((s32)(end_uv - uv) >> 4);
    return (d & 0xFFFF0000) | (u16)((s16)(d << 4) >> 4);
}

/* 156D40, unrolled x16; the 16-px block (156F2A-156F4D) is the same loop, two pixels per
   word store. The tail gets the mask in eax (3F3Fh), the block has it as an immediate. */
static void tex64_run(u8 *dst, int n, u32 uv, u32 step, const u8 *texels)
{
    int k;
    for (k = 0; k < n; k++) {
        dst[k] = texels[XN_TEXEL(uv) & 0x3F3F];
        uv += step;
    }
}
```

The mask is applied to the texel offset, not to uv (unlike SPAN-TEX): uv is never wrapped,
and its carries run on into the other half. Copy this exactly. The 16-px block's products are
low dwords (`imul eax, ecx`), not high dwords as in SPAN-TEX: `(u_num * z) >> 16` for u and
`v_num * z` for v, packed with `mov cx, ax`.

**Groups:**
- SPAN-TEX64: 156E60 and 156D40 (adapter stride 17);
- SPAN-TEX64-SHADED: 157240 and 157100 (adapter stride 19).

Records: 156E60 12, 156D40 12, 157240 26, 157100 27. The terrain caller `xn_render_frame` runs
them on outdoor frames. 156D40 and 156320 share no body, but both groups share the offsets
tables (data, read-only).

**Traps.**
- `mov bp, [n*2 + table]` writes only bp. It is correct only because n < 16 leaves ebp's high
  half 0, so C needs nothing for it.
- `movsx eax, cx; sar ecx, 10h` splits the packed delta into signed halves before the
  reciprocal multiplies.

## SPAN-SOLID-LIT: `xn_span_solid_lit` (155920) and tail 155880

**What.** It draws a solid-colour polygon lit by a per-pixel shader. The shade comes from the
polygon's compiled light shader (`call [poly+14h]`, see light.md) at each 16-px boundary, and
is interpolated in between. The pixel is `shade_table[shade_row_int : colour]`.

**Mechanism.**
- Block: 16 pixels as 8 word stores (`mov ah,ch; add ecx,ebp; mov dl,[eax]; mov dh,dl`): each
  shade lookup is written to two pixels, 12 bytes per pair.
- Tail 155880: 16 single-pixel steps of 4 instructions, 9 bytes a step, restore `8Ah`
  (`mov ah, ch`), offsets `xn_span_solid_lit_tail_offsets` 155860 (= 9n).
- Fields:

  | Field | Operand | Writer | Value |
  |---|---|---|---|
  | 155944, 15599B, 155A4B | `mov eax, ray_y` | 12A870, per row | `xn_cam_dir_y_mid[row]` |
  | 155996 | `add ebx, ray_dx16` | 12A3AC | 2^22 / focal_x (the x ray over 16 px) |
  | 155984 | `add ecx, dz16` | itself | `2 * poly->dz8` |
  | 1559B9 | `mov al, colour` (byte) | itself | the low byte of `poly->texels` (the colour) |
  | 155A21 | `cmp edi, end` | itself (x3) | `dst + (n & ~15)` |

- `xn_span_solid_lit_shade` (155911) is a dword of data right after the tail's `ret`. It holds
  the shade at the last boundary.
- The divides: `0x4000_00000000 / inv_z` (`div ecx`) at each boundary. A zero inv_z faults, and
  the engine's divide handler returns 0 (rule DIV).

**Readable C.**

```c
extern s32 xn_span_solid_lit_ray_a, xn_span_solid_lit_ray_b, xn_span_solid_lit_ray_c;
extern s32 xn_span_solid_lit_ray_dx16;          /* 155996, from xn_cam_update_derived */
extern s32 xn_span_solid_lit_dz16;              /* 155984 KEEP */
extern u8  xn_span_solid_lit_colour;            /* 1559B9 KEEP (a byte field!) */
extern u8 *xn_span_solid_lit_end;               /* 155A21 KEEP */
extern s32 xn_span_solid_lit_shade;             /* 155911: data */

/* the polygon's shader: see light.md (generated code; called through the register thunk) */
s32 xn_light_shade_at(const xn_poly *poly, s32 ray_y, s32 ray_x, u32 z);

/* 155880 (the tail: one pixel a step), and the 16-px block (one lookup per pixel pair).
   eax = the run's start shade with al = colour; only ah (= bits 8-15 of the running
   shade) changes per pixel, so the top half stays the start shade's. */
static void solid_lit_run(u8 *dst, int n, u32 shade, s32 step, u8 colour)
{
    u32 hi = shade & 0xFFFF0000u;
    int k;

    for (k = 0; k < n; k++) {
        dst[k] = *(const u8 *)(hi | (shade & 0xFF00u) | colour);
        shade += step;
    }
}
```

The block uses the same lookup once per pixel pair, with `step = (end - start) >> 3`. The tail
uses `step = (end - start) * xn_recip16_table[n] >> 16` (155A5A-155A72). Do not simplify the
address to `shade | colour`: a carry out of bit 15 of the running shade must not reach the
address's top half, which stays the start's.

**Group:** 155920 and 155880 (adapter stride 9).
- Runs together with the light shaders (light.md), which it calls.
- Records: 155920 24, 155880 25 (75 in all-C mode).

**Traps.**
- The colour field is a byte. Writing it as a dword would destroy `mov ah, ch` after it.
- The shade dword 155911 is a memory write each span.
- The ray fields are written by the frame (rule PF): read them, never cache them.
- `div` by a zero inv_z: use the DIV rule's register-form helper, so that the engine's own
  divide handler runs, as the records expect. It steps over a 2-byte instruction and returns
  0 and 0.

## SPAN-TEX-LIT: `xn_span_tex_lit_setup` (156A80) and `xn_span_tex_lit` (156A94)

**What.** It draws a textured polygon lit per pixel. Every 16 px it divides for u and v, asks
the polygon's light shader for the shade at the boundary, and runs the texture's compiled
mapper copy (tmap.md) for 16 pixels with the shade interpolated.

**Mechanism.**
- 156A80 doubles the 8-px steps, rotates the packed origin by 16
  (`rol [poly+18h], 10h`: u goes to the high half, the order the mapper expects), stores 156A94
  in `poly->span_fn`, and falls in.
- Globals in the code page:
  - `xn_span_tex_lit_tmap` (156A40) = `poly->tmap`;
  - `xn_span_tex_lit_shader` (156A44) = `poly->shade`;
  - `_shade`, `_shade_end`, `_u_num`, `_v_num`, `_u_prev`, `_v_prev`, `_ray_x` (156A48-156A60).
- Fields:
  - the 1/z table: 156AB7, 156B9F, 156C40;
  - the y ray: 156AC9, 156BAE, 156C59 (12A870, per row);
  - the x-ray step: 156B74 (12A3AC);
  - self: 156B7E, 156B88, 156B8E, 156BF4 (steps, origin) and 156C10 (end, written three times).
- The 16-px body is not in this function: `call [xn_span_tex_lit_tmap]` runs the texture's
  copy in the tmap pool, with:
  - eax = ebx = packed uv;
  - ecx = step;
  - esi = shade;
  - ebp = shade step per 2 px ((end - start) >> 3);
  - edi = dst - 1.
- The last run plants a `ret` in the heap copy at `xn_tmap_ret_offsets[n]` (0, 28, 52, 80 ...),
  calls the copy, and restores `0Fh` (the `bswap`). No static census sees this write: the
  address is in eax.

**Readable C.** The copy's semantics are the tmap template's (tmap.md, `xn_tmap_run`). This
routine calls `xn_tmap_run(poly->tmap, dst, n, uv, step, shade, shade_step)`. That function
either runs the generated code or interprets it (tmap.md: options A and B).

```c
void xn_span_tex_lit(xn_poly *poly, const xn_span *span, s32 x, s32 n, u8 *dst)
{
    u32 inv_z = span->inv_z;
    s32 z = XN_Z(xn_span_texlit_recip_a, inv_z);
    s32 u, v, shade;

    xn_span_tex_lit_tmap = poly->tmap;
    xn_span_tex_lit_shader = poly->shade;
    xn_span_tex_lit_ray_x = xn_cam_dir_x_mid[x];
    xn_span_tex_lit_shade = xn_light_shade_at(poly, xn_span_texlit_ray_a, xn_span_tex_lit_ray_x, z);
    xn_span_tex_lit_u_num = x * poly->u_dx + xn_render_row_y * poly->u_dy + poly->u_0;
    xn_span_tex_lit_u_prev = xn_mulhi(xn_span_tex_lit_u_num, z);
    xn_span_tex_lit_v_num = x * poly->v_dx + xn_render_row_y * poly->v_dy + poly->v_0;
    xn_span_tex_lit_v_prev = xn_mulhi(xn_span_tex_lit_v_num, z);
    if (n >= 16) {
        u8 *end = dst + (n & ~15);
        s32 shade0 = xn_span_tex_lit_shade;

        XN_KEEP(xn_span_texlit_dz, poly->dz8);  /* 156B8E, ... (5 KEEP stores) */
        ...
        do {
            xn_span_tex_lit_ray_x += xn_span_texlit_ray_dx16;
            xn_span_tex_lit_u_num += poly->du8;
            xn_span_tex_lit_v_num += poly->dv8;
            inv_z += poly->dz8;
            z = XN_Z(xn_span_texlit_recip_b, inv_z);
            shade = xn_light_shade_at(poly, xn_span_texlit_ray_b, xn_span_tex_lit_ray_x, z);
            u = xn_mulhi(z, xn_span_tex_lit_u_num);
            v = xn_mulhi(z, xn_span_tex_lit_v_num);
            xn_tmap_run(poly->tmap, dst, 16,
                        ((u32)xn_span_tex_lit_u_prev << 16 | ((u32)xn_span_tex_lit_v_prev & 0xFFFF))
                            + poly->tex_origin,
                        ((u32)(u - xn_span_tex_lit_u_prev) << 12 & 0xFFFF0000)
                            | ((u32)((v - xn_span_tex_lit_v_prev) >> 4) & 0xFFFF),
                        shade0, (shade - shade0) >> 3);
            xn_span_tex_lit_u_prev = u;
            xn_span_tex_lit_v_prev = v;
            shade0 = shade;
            dst += 16;
        } while (dst != end);
        xn_span_tex_lit_shade = shade0;
        n &= 15;
        if (n == 0)
            return;
    }
    /* the last run: shader at pixel n-1, u/v divided by n through xn_recip16_table, then
       xn_tmap_run(..., n, ...) where the asm plants a ret at xn_tmap_ret_offsets[n] */
    ...
}
```

The block's packed uv is not masked here: the copy masks a `bswap`ped copy of it. The uv and
step halves are swapped against SPAN-TEX (u high, v low), and the origin was rotated to match.

**Group:** 156A80 and 156A94.
- They also depend on two groups, without needing them converted:
  - LIGHT-SHADERS (the `xn_light_shade_at` calls);
  - TMAP (the copies).
- Records: 156A94 26, with 85 runs in all-C mode. The records keep whole pages, so the heap
  pages with the generated copies and the shader pool come with them.

**Traps.**
- The planted `ret` goes into the heap copy, not object 2. A C caller that runs the generated
  code (tmap option A) must plant and restore exactly as the asm does: C3, then 0Fh. Option B
  writes nothing there. Both are exact, because the asm's restore makes the net write zero.
- The shade step is `(end - start) >> 3` (signed), applied every 2 pixels inside the copy.
- The last run calls the shader at `ray_x + xn_cam_dir_x_mid[n]` with `(n-1) * dzdx` added to
  inv_z (156C2B-156C5D). Copy the `lea eax,[ebp-1]`.

## SPAN-FLAT-*: the billboard spans 157620, 157800, 157B20, 157E20 and their tails

**What.** They draw one span of a flat (sprite). Texel 0 is transparent:
- 157620 is unshaded;
- 157800 maps through one shade row (`flat+10h`);
- 157B20 maps through the light row, then the fog row (`flat+4Ch`);
- 157E20 blends through a translucency table (`table[texel<<8 | pixel]`).

They are called from `xn_flat_span_emit` (flat.md) with ecx = 1/z, ebx = x, ebp = n,
edi = dst - 1, and esi = the flat record.

**Mechanism.**
- One perspective divide per span. The steps are exact: per pixel, `step = d >> 3` (packed),
  plus `d & 7` added once after every 8-px block (`add ebx, edx` / the patched `add ebx, rem`).
  That is an exact DDA over 8.
- Blocks of 8 with `dec ebp` (n >> 3 blocks).
- Tails of 8 steps:

  | Tail | Bytes a step | Offsets table |
  |---|---|---|
  | 157760 | 19 | 157600 |
  | 157960 | 21 | 12EF78 (shared with the water) |
  | 157CA0 | 23 | 157B00 |
  | 157FA0 | 25 | 157E00 |

  Each restores `8Bh`. Two of the tails (157CA0, 157FA0) take their step in ebp: the routine
  reloads it from its own field 157B99/157E98 before calling.
- Fields:
  - 157622/157802/157B22/157E22: `sub ebx, centre_x - 1`, written by 12A2D0;
  - 15762E..157E2E: the 1/z table, written by 12A100;
  - self: 157918/157C58/157F67 (the 8-px remainder) and 157B99/157E98 (the step, read as an
    operand and as data).
- `xn_span_tail_offsets_25_entry1` (157E02) is a table entry decoded as code. It is not a
  function: give it no C.

**Readable C** (157620; the others add their pixel map):

```c
extern s32 xn_span_flat_transparent_cx;          /* 157622: centre x - 1 (PF, 12A2D0) */
extern const u32 *xn_span_flat_transparent_recip; /* 15762E: the 1/z table (PF, 12A100) */

static void flat_run(u8 *dst, int n, u32 *uv, u32 step, const u8 *texels)   /* 157760, x8 */
{
    int k;
    for (k = 0; k < n; k++) {
        u8 c = texels[XN_TEXEL(*uv)];
        *uv += step;
        if (c)
            dst[k] = c;
    }
}

void xn_span_flat_transparent(xn_poly *flat, u32 inv_z, s32 x, s32 n, u8 *dst)
{
    s32 z = xn_span_flat_transparent_recip[inv_z >> 13];
    s32 xs = x - xn_span_flat_transparent_cx;
    u32 uv, step, rem;
    s32 du, dv;

    uv = (u32)((xs * flat->v_dx + flat->v_0) * z - flat->unknown_1c) & 0xFFFF0000
         | (u16)(((xs * flat->u_dx + flat->u_0) >> 1) * z - flat->tex_origin >> 15);
    du = (s32)(flat->unknown_08 * z) >> 16;              /* per-pixel u step, 16.16 */
    dv = flat->unknown_0c * z;
    step = ((u32)(dv >> 3) & 0xFFFF0000) | (u16)(du >> 3);
    rem = ((u32)dv & 0x70000) | (du & 7);                /* and edx, 70007h */
    while (n >= 8) {
        flat_run(dst + 1, 8, &uv, step, flat->texels);
        uv += rem;
        dst += 8;
        n -= 8;
    }
    flat_run(dst + 1, n, &uv, step, flat->texels);
}
```

The uv, step and remainder in the sketch follow 157620-157672 term by term:
- `sar eax,1` then `sar eax,0Fh` for u;
- `mov bx, ax` (v's top half, u's bottom half);
- the packed per-pixel delta `(z * f0C) & 0xFFFF0000 | (u16)((f08 * z) >> 16)`;
- then `sar 3` on both halves for the step;
- and `& 70007h` for the remainder added after every 8 pixels.

The 1/z index here is `inv_z >> 13` with no `& 0xFFFF` (unlike the polygon spans). The packing
of ecx and edx is where a readable rewrite most easily goes wrong. Check the helper alone
against the asm with the differential tests before converting the loop around it.

**Groups:** each routine with its tail:
- 157620 + 157760 (stride 19);
- 157800 + 157960 (21);
- 157B20 + 157CA0 (23);
- 157E20 + 157FA0 (25).

`xn_flat_span_light_setup` (155610) installs them. Records:

| Routine | Records | Tail | Tail records |
|---|---|---|---|
| 157620 | 26 | 157760 | 26 |
| 157800 | 24 | 157960 | 27 |
| 157B20 | 25 | 157CA0 | 24 |
| 157E20 | none: it never ran in play | 157FA0 | 2, synthetic |

For 157E20, use the infra's differential tests.

**Traps.**
- The flat path's callers do not save ebx/ebp/esi (`xn_flat_span_emit` clobbers them), so the
  readable C may leave them changed, as the ABI row says.
- 157B20 and 157E20 keep the step in ebp and the remainder in a field; their tails read the
  step in ebp. `flat_run_*` takes it as a parameter.
- 157E20's `xor eax, eax` before the loop matters: `al` holds the screen pixel and `ah` the
  texel; the table index is the whole eax.

## The solid spans without self-modifying code

`xn_span_solid` (155800) and `xn_span_solid_shaded_setup` (155820) use `rep stos`, with no
patch fields. They are listed here only because 155820 rewrites `poly->span_fn`, which is data
and not code. They need no special design.

## Order within span

1. SPAN-TEX (the most records, the simplest pixel).
2. SPAN-TEX-SHADED.
3. SPAN-TEX64 and SPAN-TEX64-SHADED.
4. SPAN-FLAT-*.
5. SPAN-SOLID-LIT and SPAN-TEX-LIT, which need the light-shader and tmap decisions first.

Build the RET adapter before step 1, so that each tail's own records test `*_run` alone.
