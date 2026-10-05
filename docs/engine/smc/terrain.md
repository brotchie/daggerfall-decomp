# terrain: the outdoor ground grid (0x13E600-0x13F608)

The terrain's self-modifying code is all patched operands: an incremental walker that keeps
its camera-space position in its own `mov reg, imm` operands, and per-call constants. There is
no planted `ret` and no generated code. The small unrolled pieces are cosmetic (rule UNR).

Rules (PF, KEEP, UNR) are in index.md.

## Group TERRAIN-GRID: `xn_terrain_transform_grid` (13E8C8)

**What.** It transforms the 32x32 vertex grid around the eye into camera space. For each
vertex, it computes:
- the height offset (from three 128-entry tables of the vertex's height byte);
- the outcode (with the height byte's bit 7);
- for unclipped vertices, the projection (`256/z`, screen x with 5 fraction bits, screen y).

It writes `xn_vert_cam`, `xn_vert_screen`, `xn_vert_flags` and the unscaled x/y arrays.

**Mechanism.** 24 patch fields: 20 written by the walker itself, 4 by the view window.

| Field(s) | Operand | Role | Written |
|---|---|---|---|
| 13E98A, 13E994, 13E99E | `mov dword ptr [13E9B7/9BE/9C3], IMM` | the row start (camera xyz) | 13E8FF.. (corner transform), then `+= row step` at 13EAF1.. |
| 13E9B7, 13E9BE, 13E9C3 | `mov eax/ebp/ebx, IMM` | the current vertex (camera xyz) | copied from the row start at each row (13E984..), then `+= column step` at 13EAC2.. |
| 13EAC8, 13EAD2, 13EADC | `add [vertex], IMM` | column step | 13E921..: `xn_terrain_step_x` (136E48..50) |
| 13EAF7, 13EB01, 13EB0B | `add [row], IMM` | row step | 13E943..: `xn_terrain_step_z` (136E60..68) |
| 13EAE7 | `cmp edi, IMM` | row end (vertex byte offset) | 13E9A8: `edi + 180h`, per row |
| 13EB16 | `cmp edi, IMM` | grid end | 13E97A: the constant 3000h, every call |
| 13EA75, 13EA8D | `imul eax, eax, IMM` | half width, half height | 12A2D0 |
| 13EA7D, 13EA95 | `add edx, IMM` | (cx << 8) + 80h, (cy << 8) + 80h | 12A2D0 |

Here the patch fields *are* the walker's state: the operands of one instruction are written by
another instruction of the same loop. Use them directly as the C variables (rule PF). The C
then leaves exactly the final values the asm leaves, with no epilogue to get wrong.

```c
/* tgrid.c */
extern s32 xn_tgrid_row_x, xn_tgrid_row_y, xn_tgrid_row_z;        /* 13E98A 13E994 13E99E */
extern s32 xn_tgrid_x, xn_tgrid_y, xn_tgrid_z;                    /* 13E9B7 13E9BE 13E9C3 */
extern s32 xn_tgrid_col_dx, xn_tgrid_col_dy, xn_tgrid_col_dz;     /* 13EAC8 13EAD2 13EADC */
extern s32 xn_tgrid_row_dx, xn_tgrid_row_dy, xn_tgrid_row_dz;     /* 13EAF7 13EB01 13EB0B */
extern u32 xn_tgrid_row_end, xn_tgrid_end;                        /* 13EAE7 13EB16 */
extern s32 xn_tgrid_sx, xn_tgrid_cx, xn_tgrid_sy, xn_tgrid_cy;    /* 13EA75 13EA7D 13EA8D 13EA95 */

void xn_terrain_transform_grid(void)
{
    s32 cx, cz;
    u32 i = 0;                                  /* edi: 12 bytes per vertex */
    u8 col, row;                                /* bl, bh: the 256x256 layer position */

    cx = -(xn_cam_x & 0xFF) - 0x1000;
    cz = (-xn_cam_z & 0xFF) + 0x1000;
    xn_mat_transform_wide(cx << 8, 0, cz << 8, xn_cam_rotation,
                          &xn_tgrid_row_x, &xn_tgrid_row_y, &xn_tgrid_row_z);
    xn_tgrid_col_dx = xn_terrain_step_x[0]; xn_tgrid_col_dy = xn_terrain_step_x[1];
    xn_tgrid_col_dz = xn_terrain_step_x[2];
    xn_tgrid_row_dx = xn_terrain_step_z[0]; xn_tgrid_row_dy = xn_terrain_step_z[1];
    xn_tgrid_row_dz = xn_terrain_step_z[2];
    row = (u8)(((xn_world_size_z - xn_cam_z) >> 8) - 0x10);   /* sub ah, 10h */
    col = (u8)((xn_cam_x >> 8) - 0x10);
    xn_tgrid_end = 0x3000;
    do {
        xn_tgrid_x = xn_tgrid_row_x;
        xn_tgrid_y = xn_tgrid_row_y;
        xn_tgrid_z = xn_tgrid_row_z;
        xn_tgrid_row_end = i + 0x180;
        do {
            grid_vertex(i, (u16)(row << 8 | col));     /* 13E9AD-13EABC, below */
            xn_tgrid_x += xn_tgrid_col_dx;
            xn_tgrid_y += xn_tgrid_col_dy;
            xn_tgrid_z += xn_tgrid_col_dz;
            i += 12;
            col++;
        } while (i != xn_tgrid_row_end);
        xn_tgrid_row_x += xn_tgrid_row_dx;
        xn_tgrid_row_y += xn_tgrid_row_dy;
        xn_tgrid_row_z += xn_tgrid_row_dz;
        row++;
        col -= 0x20;
    } while (i != xn_tgrid_end);
}
```

`grid_vertex` is the per-vertex body:
- the height offsets: `x = xn_tgrid_x - height_cam_x[h]` ...;
- `or ecx, 10h` (the near flag) for z <= -12C00h;
- otherwise the scaled camera xyz, the outcodes, and for a vertex inside the frustum, the
  projection with `xn_tgrid_sx/cx/sy/cy`;
- the flag and texture bytes.

**Group:** 13E8C8 alone. Records: 26 own (52 in all-C mode). `xn_terrain_draw` (13E600, 26)
calls it every outdoor frame.

**Exactness traps.**
- **Byte arithmetic of the layer position.** `mov bh, ah` takes bits 8-15 of
  `(size_z - cam_z) - 1000h` (`sub ah, 10h` on the high byte of the low word); `sub bl, 10h`
  and `and ebx, 0FFh` for x. Per vertex `inc bl`; per row `inc bh; sub bl, 20h`. All wrap
  modulo 256. Use `u8` for both.
- `xn_tgrid_end = 3000h` is stored on every call (13E97A) even though it never changes. The
  C must store it too: rule KEEP, and here a plain assignment.
- The row-end field is written per row, with `edi + 180h` (`lea`). Its final value is
  `3000h`.
- The projection uses `shrd eax, edx, 0Eh` / `shld edx, eax, 12h` for the scaled camera x/y:
  64-bit, through the infra helpers (`xn_mulshr`).
- `div ebx` (`256 * 2^32 / z`) is only reached for z beyond the near test, so it does not
  fault.

## Group TERRAIN-CELLS: `xn_terrain_draw_cells` (13EFD7)

**What.** It emits the 31x31 cells as polygons: texture, orientation, one quad or two
triangles, clipped or not.

**Mechanism.** Six self-patched constants, all written once at the top of each call and read
per cell (rule KEEP):

| Fields | Operand | Value |
|---|---|---|
| 13F0A2, 13F11B, 13F1BA | `mov eax, ARCHIVE` | `xn_world_ground_archive` (zero-extended word) |
| 13F06A, 13F0E3, 13F182 | `mov [edi+3Ch], SETUP` | `xn_render_span_setup_terrain_ptr` (12A019) |

There is also one small fixed unroll (13F1E8-13F29B): the quad's 4 corners are copied into
`xn_poly_vertex_buf_a` from vertex offsets 0, +180h, +18Ch and +0Ch (cam xyz and the flag
byte). In C this is a loop over a 4-entry corner-offset table (rule UNR).

```c
extern u32 xn_tcells_archive_a, xn_tcells_archive_b, xn_tcells_archive_c;   /* KEEP */
extern void *xn_tcells_setup_a, *xn_tcells_setup_b, *xn_tcells_setup_c;     /* KEEP */
...
archive = xn_world_ground_archive;
XN_KEEP(xn_tcells_archive_a, archive); XN_KEEP(xn_tcells_archive_b, archive);
XN_KEEP(xn_tcells_archive_c, archive);
...                                         /* tex_cache_lookup may fail: CF out */
setup = xn_render_span_setup_terrain_ptr;
XN_KEEP(xn_tcells_setup_a, setup); XN_KEEP(xn_tcells_setup_b, setup);
XN_KEEP(xn_tcells_setup_c, setup);
```

**Group:** 13EFD7 alone. Records: 26 (52 in all-C mode).

**Traps.**
- The archive fields are written *before* the texture lookup (13EFE3), and the setup fields
  *after* it succeeds (13F009). On a lookup failure (CF), only the archive fields have been
  written. The C must keep that order.
- CF out (the ABI row: flags_out CF), so the function needs the infra's flags adapter.
- The orientation dispatch `call [eax + ebx*4 + 138538h]` / `[eax + ebx*2 + 138558h]` is a
  table of function pointers (data), with no self-modifying code. In C it is a call through a
  table of register-interface functions (0x13F4CC..0x13F608).

## Group TERRAIN-AXES: `xn_terrain_setup_axes` (13E744)

Fixed unrolled code, with no patch fields:
1. six `imul [tex_scale]; idiv 1000000h`, turning the two camera-space axes into texture axes;
2. nine `add 80h; sar 8`, rounding the three step vectors to 8 fraction bits less.

```c
for (k = 0; k < 3; k++) {
    xn_terrain_u_axis[k] = xn_muldiv(xn_terrain_step_x[k], xn_terrain_tex_scale, 0x1000000);
    xn_terrain_v_axis[k] = xn_muldiv(xn_terrain_step_z[k], xn_terrain_tex_scale, 0x1000000);
}
for (k = 0; k < 9; k++)                     /* step_x, step_y, step_z: 136E48..136E68 */
    xn_terrain_steps[k] = (xn_terrain_steps[k] + 0x80) >> 8;
```

**Group:** alone. 26 records.

**Trap:** the asm computes all six products first, u then v. The order does not matter (no
shared state), but `idiv` can fault. The C keeps `xn_muldiv` (a real `idiv`, rule DIV).
