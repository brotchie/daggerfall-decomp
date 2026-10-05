# poly: projectors, the edge walker, gradients and the textured setup

These are the polygon functions with patch fields. None has an unrolled body or a planted
`ret`; all of their mechanisms are patched operands, written by the view-window setup or by the
function itself (rules PF and KEEP in index.md).

## Group POLY-PROJECT: `xn_poly_project_flat` (158360), `_face` (158420), `_terrain` (1585C0)

**What.** They clip a polygon (Sutherland-Hodgman, 158678) when its outcodes say so, then
project each vertex in place:
- `1/z = 2^40 / z` (`div` with edx = 100h);
- `sx = (u32)(hi32(x * hw * inv_z) + (cx << 8) + 80h) >> 3` (5 fraction bits, rounded);
- `sy = (u32)(hi32(y * hh * inv_z) + (cy << 8) + 80h) >> 8`.

They also track the top vertex, then call the rasterizer (15B9A0, or 1552A0 for flats).

**Patch fields** (4 per projector, all written by `xn_cam_set_view_window`, rule PF):

| Projector | hw (`imul eax, eax, HW`) | cx (`add edx, CX`) | hh | cy |
|---|---|---|---|---|
| flat 158360 | 1583AF | 1583B7 | 1583C5 | 1583CD |
| face 158420 | 1584AD | 1584B5 | 1584C3 | 1584CB |
| terrain 1585C0 | 158608 | 158610 | 15861E | 158626 |

```c
extern s32 xn_poly_flat_sx, xn_poly_flat_cx, xn_poly_flat_sy, xn_poly_flat_cy;   /* PF */

static void project_vertex(xn_clip_vertex *v, s32 sx, s32 cx, s32 sy, s32 cy)
{
    u32 inv_z = xn_udiv64(0x100, 0, (u32)v->z);           /* 2^40 / z: a real div (DIV) */
    v->z = inv_z;
    v->x = (u32)(xn_mulhi(v->x * sx, (s32)inv_z) + cx) >> 3;
    v->y = (u32)(xn_mulhi(v->y * sy, (s32)inv_z) + cy) >> 8;
}
```

**Groups:** each projector is its own group (records: 158360 28, 158420 28, 1585C0 26). They
share the clipper (plain C) and call the rasterizers.

**Traps.**
- `shr` (unsigned) for the screen coordinates. A vertex left of the screen would come out huge,
  not negative, but clipping prevents it.
- `imul eax, eax, HW` is a 32-bit product (it can wrap); `imul ebx` is 64-bit (take the high
  dword).
- `xn_poly_project_terrain` has a different register interface (ABI: outputs
  ah/eax.u/dh/edx.u/bl/ebx.u). Use the ABI row, not the sketch's signature.

## Group POLY-RASTER: `xn_poly_rasterize` (15B9A0) and `xn_poly_rasterize_row_loop` (15BA16)

**What.** It walks the projected polygon's left edge forward and right edge backward around the
vertex ring (`xn_poly_ring_a/b`), one screen row at a time, and inserts each row's span into
the S-buffer (`xn_span_insert` 158D04).

**Mechanism.** Self-patched loop state (the walker keeps its edge state in its own operands):

| Field | Operand | Role | Written |
|---|---|---|---|
| 15B9C4 | `mov ebp, LEFT` | left edge's vertex-ring pointer | 15B9A0 (entry), then `+4` per left edge (15B9C8) |
| 15BA68 | `mov ebx, RIGHT` | right edge's vertex-ring pointer | 15B9A6 (entry), then `-4` per right edge (15BA6C) |
| 15BA3C | `add ebp, DXL` | left x slope (16.16) | 15BA00, per left edge |
| 15BA48 | `add ecx, DZL` | left 1/z slope | 15BA0D, per left edge |
| 15BA42 | `add ebx, DXR` | right x slope | 15BA91, per right edge |

The slopes come from `xn_recip16_table` / `xn_recip32_table` (0xFFFF/n, 0xFFFFFFFF/n). The
row counters are packed in edi (left rows in di, right rows in edi's top half;
`sub edi, 10001h` per row).

15BA16 ("row loop") is an entry inside 15B9A0's body: an `or eax, eax` after an unconditional
jump, which nothing reaches. Give it no C of its own; the group is one function.

**Readable C** (loop-state fields used directly as variables, rule PF: other functions do not
read them, but they are this function's state):

```c
extern xn_clip_vertex **xn_raster_left;      /* 15B9C4 */
extern xn_clip_vertex **xn_raster_right;     /* 15BA68 */
extern s32 xn_raster_dxl, xn_raster_dzl, xn_raster_dxr;   /* 15BA3C 15BA48 15BA42 */

void xn_poly_rasterize(s32 top_y, xn_clip_vertex **ring)
{
    s32 xl, zl, xr;
    u32 rows;                                   /* left rows | right rows << 16 */
    xn_span_row *row = &xn_render_span_rows[top_y];

    xn_raster_left = xn_raster_right = ring;
    rows = 0;
    for (;;) {
        if ((rows & 0xFFFF) == 0) {             /* next left edge */
            if (--xn_poly_vertex_count < 0) return;   /* dec byte; js */
            ... xn_raster_left++; xn_raster_dxl = ...; xn_raster_dzl = ...; continue if dy <= 0
        }
        if ((rows >> 16) == 0) {                /* next right edge */
            if (--xn_poly_vertex_count < 0) return;
            ... xn_raster_right--; xn_raster_dxr = ...;
        }
        if ((xl >> 21) < (xr >> 21))  ... xn_span_insert(row, ...)
        row++; rows -= 0x10001;
        xl += xn_raster_dxl; xr += xn_raster_dxr; zl += xn_raster_dzl;
    }
}
```

The sketch shows the shape. The control flow of 15B9A0 interleaves the two "next edge" blocks
with the row loop through jumps. Transcribe it with the two edge-advance blocks as small
`static` functions, and keep the packed `rows` counter (`sub edi, 10001h`, `or di, di`,
`test edi, 0FFFF0000h`): it decides which edge is advanced first when both run out on the
same row.

**Group:** 15B9A0 and 15BA16. Records:
- 15B9A0: 28 (236 in all-C mode);
- 15BA16: 1, synthetic. Its only record calls the dead entry directly: let it fail, or test it
  through an adapter that enters the C at the row loop.

Callers: 13ED23/13EDCD, 13EFD7, 158420 and 1585C0.

**Traps.**
- All five fields are memory writes. The final values are what the asm leaves: the ring
  pointers advanced past the last edge, and the last slopes. Using the fields as the variables
  gives exactly that.
- `xn_poly_vertex_count` (15B984) is a byte decremented with `js`, signed. The C uses `s8`.
- `shr ebx, 15h` / `shr ebp, 15h` (unsigned) turn 11.21 x into pixels; `jle` compares them
  signed.
- The flat rasterizer 1552A0 (flat.md) is a copy of this walker with its own fields.

## Group POLY-GRAD: `xn_poly_tex_gradients` (15BAC0)

**What.** It multiplies the face's two texture axes (object space) by the object matrix, giving
the u/z and v/z gradients per x, per y and constant (poly +24h..+38h). It also writes the 8-px
steps +50h/+54h.

**Mechanism.** Four self-patched operands that serve as saved registers: the axes' x components
are stored into `mov eax, IMM` operands, to be reloaded after `imul` clobbered eax.

| Field | Value | Read at |
|---|---|---|
| 15BAE8, 15BB0C | `u_axis[0]` (`[edx]`) | the 2nd and 3rd rows of the u product |
| 15BB4D, 15BB6E | `v_axis[0]` (`[edx+0Ch]`) | the same for v |

There are 2 NOPs at 15BAD3 (alignment).

**Readable C** (rule KEEP: a local, plus the four stores):

```c
void xn_poly_tex_gradients(const s32 *m, const s32 *axes, xn_poly *poly)  /* ecx, edx, edi */
{
    XN_KEEP(xn_grad_u0_a, axes[0]);  XN_KEEP(xn_grad_u0_b, axes[0]);
    poly->du8 = row_dot(m + 0, axes);           /* hi32 sums: imul, add edx */
    poly->u_dx = poly->du8 >> 3;
    poly->u_dy = row_dot(m + 3, axes);
    poly->u_0  = row_dot(m + 6, axes);
    XN_KEEP(xn_grad_v0_a, axes[3]);  XN_KEEP(xn_grad_v0_b, axes[3]);
    poly->dv8 = row_dot(m + 0, axes + 3);
    poly->v_dx = poly->dv8 >> 3;
    poly->v_dy = row_dot(m + 3, axes + 3);
    poly->v_0  = row_dot(m + 6, axes + 3);
}

/* hi32(a0*m0) + hi32(a1*m1) + hi32(a2*m2): three high dwords added, not a 64-bit sum */
static s32 row_dot(const s32 *m, const s32 *a)
{
    return xn_mulhi(a[0], m[0]) + xn_mulhi(a[1], m[1]) + xn_mulhi(a[2], m[2]);
}
```

**Group:** alone. Records: 28. It is called by 15BB8C (28).

**Traps.**
- The sum is of high dwords (`mov ebp, edx; ... add ebp, edx`), not the high dword of a 64-bit
  sum. The C `row_dot` must not use `xn_s64_mac`.
- `sar ebp, 3` for +24h/+30h (signed).

## Group POLY-SETUP-TEX: `xn_poly_setup_textured` (15BB8C)

**What.** It is a polygon's first-span setup for textured faces:
1. texel base, mask and compiled mapper from the texture record;
2. lighting (`xn_light_setup_poly`, which returns 0/4/8);
3. the 8- or 16-px choice;
4. the gradients;
5. the packed texture origin (+18h).

Then it stores the chosen span routine in `poly->span_fn` and **tail-jumps** to it with the
original registers (`jmp [eax+3Ch]`).

**Patch fields** (all cross, rule PF):

| Field | Operand | Writer | Value |
|---|---|---|---|
| 15BBB9 | `shr edx, SHIFT` (**byte**) | 12A4F0 | 18 when the screen is 320 wide, else 25 |
| 15BBDE | `imul esi, [edi+18h], HW` | 12A2D0 | half width |
| 15BBE5 | `imul ecx, [edi+1Ch], HH` | 12A2D0 | half height |

```c
extern u8  xn_poly_subdiv_shift;           /* 15BBB9 */
extern s32 xn_poly_setup_sx, xn_poly_setup_sy;

/* asm: the span routine's registers (eax = poly, esi, ebx, ebp, edi); it ends by running
   poly->span_fn on the same span */
void xn_poly_setup_textured(xn_poly *poly, const xn_span *span, s32 x, s32 n, u8 *dst)
{
    s32 kind;
    u32 d;
    ...
    kind = xn_light_setup_poly(poly);                 /* 0, 4, 8 */
    d = poly->dzdx < 0 ? -poly->dzdx : poly->dzdx;
    if ((d >> (xn_poly_subdiv_shift & 31)) == 0)
        kind += 12;                                   /* the 16-px routines */
    poly->span_fn = xn_render_tmap_span_fns[kind / 4];
    ...                                               /* gradients, origin */
    xn_span_call(poly->span_fn, poly, span, x, n, dst);   /* the asm's jmp [eax+3Ch] */
}
```

**Group:** alone (28 records, 56 in all-C mode). It is reached through the render-mode table
`xn_render_span_setups` (12A009) as a polygon's first routine.

**Traps.**
- **The tail jump.** The asm restores ebx, ebp, esi and edi (pushed at entry) and jumps, so the
  span routine sees the setup's own entry registers. In C, call the routine with the same
  arguments. The span routine's register outputs then become the setup's, and the ABI row
  ("partial") should treat them as such.
- `or edx, edx; jns; neg edx` is `abs` with `abs(INT_MIN) = INT_MIN`. Then `shr` (unsigned).
  The C must use u32 for the shift.
- The 64-bit sums for the origin (`imul; add; adc; shrd 1Ah` and `shrd 0Ah`) need
  `xn_s64_mac` and `xn_s64_shr`: these are true 64-bit sums, unlike POLY-GRAD.
