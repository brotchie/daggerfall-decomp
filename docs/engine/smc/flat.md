# flat: billboard sprites (0x154D00-0x155725)

The flat pipeline passes its per-flat state through patched operands:
- `xn_flat_draw` writes the flat record into the emitter's `mov esi, FLAT`;
- `xn_flat_begin_frame` writes six projection constants and the screen width;
- `xn_flat_setup_gradients` writes *which* polygon fields the row walker advances, and by how
  much;
- the row walker keeps its edge state in its own operands.

There is no planted `ret` here (the flat span routines are in span.md) and no generated code.
Rules (PF, KEEP) are in index.md.

## Writers: `xn_flat_begin_frame` (155415) and `xn_flat_draw` (154E20)

| Writer | Field | Operand (reader) | Value |
|---|---|---|---|
| 155415 | 155367 | `add [row], STRIDE` (155332) | `xn_gfx_width` (an "operand" patch: assembled as 140h) |
| 155415 | 155110, 155120, 15512A | `mov eax, IMM` (1550E0) | `xn_mat_transform_wide(2^30, 0, 0, xn_flat_proj_matrix)`, with y and z `(v + 4) >> 3` |
| 155415 | 15514C, 155163, 15516D | `mov eax, IMM` (1550E0) | the same for (0, 2^30, 0) |
| 154E20 | 15526F | `mov esi, FLAT` (15526C), also read as data by 1550E0 | the flat's polygon record |

These are plain C writing named fields: per frame for 155415, per flat for 154E20.

## Group FLAT-GRAD: `xn_flat_setup_gradients` (1550E0)

**What.** It computes the flat's texture gradients from its scale and the frame's projection
constants. It also tells the row walker what to advance per row:
- *the address* of the polygon's u/z constant (`+2Ch`) and v/z constant (`+38h`);
- the per-row steps.

For a mirrored flat (flags & 20h), it negates and offsets the u terms.

**Patch fields read:**
- 1550ED/1550F7 (half width/height, 12A2D0);
- the six constants of 155415;
- 15526F as data (`mov edi, [15526Fh]`).

**Fields written** (all read by 155332):

| Field | Operand at 15534D/155357 | Value |
|---|---|---|
| 15534F | `add dword ptr [U_ROW_VAR], ...`: the **address** | `&poly->u_0` (`lea edx, [edi+2Ch]`) |
| 155353 | the same instruction's immediate | `poly->u_dy` (`[edi+28h]`) |
| 155359 | `add dword ptr [V_ROW_VAR], ...`: the **address** | `&poly->v_0` (`[edi+38h]`) |
| 15535D | the same instruction's immediate | `poly->v_dy - 8` |

```c
extern s32 *xn_flat_row_u_var;     /* 15534F: which s32 the walker advances per row */
extern s32  xn_flat_row_u_step;    /* 155353 */
extern s32 *xn_flat_row_v_var;     /* 155359 */
extern s32  xn_flat_row_v_step;    /* 15535D */

/* at the end of xn_flat_setup_gradients (edi = the flat record, ebx = top row - centre y): */
xn_flat_row_u_step = flat->u_dy;
flat->u_0 += flat->u_dy * dy;
xn_flat_row_u_var = &flat->u_0;
xn_flat_row_v_step = flat->v_dy - 8;
flat->v_0 += xn_flat_row_v_step * dy;
xn_flat_row_v_var = &flat->v_0;
```

**Group:** alone. 29 records (142 in all-C mode). It is called from `xn_flat_raster` (1552A0).

**Traps.**
- The address fields are pointers into the polygon pool: two 4-byte operand addresses inside
  two `add dword ptr [mem], imm32` instructions. Declare them `s32 *`.
- The order is as in the asm: the step is stored, then the start is advanced by `step * dy`,
  then the address is stored.
- `div ecx` with ecx = the scale (`[edi+44h] & 0FFFFh`): a scale of 0 faults (rule DIV).

## Group FLAT-RASTER: `xn_flat_raster` (1552A0) and `xn_flat_raster_rows` (155332)

**What.** It walks the flat's projected quad edge by edge, like `xn_poly_rasterize`. Per row,
it clips the flat's span against the row's world spans by depth (`xn_flat_span_clip` 155200)
and then advances the row state.

**Mechanism.** These are the self-patched edge-walker fields, used as the walker's variables
(rule PF):

| Field | Operand | Role | Written |
|---|---|---|---|
| 1552E0 | `mov ebp, LEFT` | left ring pointer | 1552A0 (entry), `+= 4` (1552E4) |
| 15539D | `mov ebx, RIGHT` | right ring pointer | 1552A6 (entry), `-= 4` (1553A1) |
| 155373 | `add ebp, DXL` | left x slope | 15531C |
| 15537F | `add ecx, DZL` | left 1/z slope | 155329 |
| 155379 | `add ebx, DXR` | right x slope | 1553C6 |
| 155275 | `lea edi, [ebx + ROW]` in 15526C | the current row's address - 1 | 1552C3 (start), `+= STRIDE` per row (155361) |
| 155367 | the STRIDE of that `add` | screen width | 155415 |

Per row, 155332 also does `*xn_flat_row_u_var += xn_flat_row_u_step` and the same for v
(15534D, 155357): the polygon's u/v constants move down one row.

There are 3 NOPs at 1552D0 (padding). 155332 is an entry inside 1552A0's loop that nothing
calls (1 synthetic record). As with `xn_poly_rasterize`, it is one function.

```c
extern xn_clip_vertex **xn_flat_raster_left, **xn_flat_raster_right;   /* 1552E0 15539D */
extern s32 xn_flat_raster_dxl, xn_flat_raster_dzl, xn_flat_raster_dxr;  /* 155373 15537F 155379 */
extern u8 *xn_flat_emit_row;                                            /* 155275 */
extern u32 xn_flat_row_stride;                                          /* 155367 */

/* per row, in the walker (after xn_flat_span_clip for the row): */
*xn_flat_row_u_var += xn_flat_row_u_step;     /* 15534D */
*xn_flat_row_v_var += xn_flat_row_v_step;     /* 155357 */
xn_flat_emit_row += xn_flat_row_stride;
rows -= 0x10001;
xl += xn_flat_raster_dxl;
xr += xn_flat_raster_dxr;
zl += xn_flat_raster_dzl;
```

The first `add` (15534D) advances u: its address field 15534F gets `&poly->u_0` from 1550E0.
The second (155357) advances v. Name the fields after what 1550E0 stores, not after the
placeholder symbols `xn_flat_half_h/w` (154C58/154C50) the asm was assembled with.

**Group:** 1552A0 and 155332. Records:
- 1552A0: 28 (113 in all-C mode);
- 155332: 1, synthetic.

It is called from `xn_poly_project_flat` (158360).

**Traps.**
- `dec byte ptr [xn_poly_vertex_count]; js`, as in POLY-RASTER.
- The walker writes the polygon record's u/v constants once per row (through the patched
  addresses). Those are memory writes in the render pool.
- 155275 is an address used by another function's `lea` (15526C). It is shared state, not a
  local.

## Group FLAT-EMIT: `xn_flat_span_emit` (15526C)

**What.** It draws one visible piece of a flat's row: `edi = ROW + x_start`, `ebp -= ebx`, and
`call [FLAT + 3Ch]` (the flat span routine, span.md). ecx and edi are preserved.

**Fields read:** 15526F (`mov esi, FLAT`, written by 154E20) and 155275 (`lea edi, [ebx+ROW]`).

```c
extern xn_poly *xn_flat_emit_flat;    /* 15526F (154E20) */

void xn_flat_span_emit(u32 inv_z, s32 x0, s32 x1, ...)   /* the register interface: see ABI */
{
    xn_span_call(xn_flat_emit_flat->span_fn, xn_flat_emit_flat, inv_z, x0, x1 - x0,
                 xn_flat_emit_row + x0);
}
```

**Group:** alone. 26 records (120 in all-C mode). It is reached by fall-through and jumps from
`xn_flat_span_clip` (155200): five `jle/jg/jl 15526C` that are tail calls. The C 155200 calls
it and returns.

**Trap.** The routine in `[flat+3Ch]` starts as `xn_flat_span_light_setup` (155610). That
setup installs the real routine and *returns without drawing*, so the first visible piece of
every flat is not drawn: an original quirk (docs/xngine_map.md, "Unsure" 3). The C must call
through the pointer exactly once per piece, as the asm does, and not "retry" after the setup.

## Group FLAT-LIGHT: `xn_flat_span_light_setup` (155610)

**What.** It lights the flat: the base shade row, plus up to 32 point lights from
`xn_light_table` by distance, plus directional lights. It also applies the fog, and installs
one of 157620 / 157800 / 157B20 / 157E20 in `[flat+3Ch]`.

**Patch field read:** 155636, `add ebp, AMBIENT_ROW`, written by `xn_render_begin_frame`
(rule PF).

```c
extern s32 xn_flat_ambient_row;   /* 155636 (12A4F0) */
...
row = ((flat->light >> 8) & 0xFF00) + xn_flat_ambient_row;
```

**Group:** alone. 26 records (125 in all-C mode).

**Traps.**
- `xn_math_isqrt_lookup` and `idiv ebx` per light (rule DIV).
- The light walk uses `add edi, 1Dh` (29-byte light records) and caps the count at 32.
- The span routine pointers it stores are asm addresses (`offset func_00157620`). In the C,
  store the asm entry addresses (`XN_ASM(...)`), not C function addresses, because asm
  callers and the records compare them.
