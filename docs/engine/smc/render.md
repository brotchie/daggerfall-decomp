# render: the setup writers and the frame loop (0x12A100-0x12A98A)

This module is where most cross-function patch fields come from:
- `xn_render_init` writes the 1/z table address into 25 span routines;
- the camera functions write the view window and focal constants into the projectors, the
  models, the terrain, the flats and the span routines;
- `xn_render_begin_frame` writes the ambient row and the subdivision shift;
- `xn_render_frame` writes the per-row view ray into the lit span routines, and patches its own
  row loop.

Every one of these is a write of a named variable (rule PF in index.md). None of them forces
another function to be converted at the same time.

## Group RENDER-SETUP: the field writers

| VA | Name | Fields written (count) | Values | Records |
|---|---|---|---|---|
| 12A100 | `xn_render_init` | 25 (all `[ecx*4 + TABLE]` / `[ebp*4 + TABLE]` in span routines) | `xn_render_recip_table` (the malloc'd 1/z table) | 2 |
| 12A274 | `xn_cam_set_focal` | 2: 1406E2, 14072F (model scale) | `2^36 / focal_x`, `2^33 / focal_y` | 6 |
| 12A2D0 | `xn_cam_set_view_window` | 29 | see below | 25 |
| 12A3AC | `xn_cam_update_derived` | 5: 1406DD, 14072A (model scale), 14030A (model depth), 156B74 / 155996 (16-px x ray step) | `2^45 / scale_x`, `2^45 / scale_y`, `2^48 / (scale_x * focal_x)`, `2^22 / focal_x` | 27 |
| 12A4F0 | `xn_render_begin_frame` | 3: 15BC61, 155636 (dword), 15BBB9 (**byte**) | the ambient shade row; the subdivision shift 18 (320 wide) or 25 | 28 |

`xn_cam_set_view_window(hw, hh, cx, cy)` writes:

| Value | Fields | Readers |
|---|---|---|
| hw (half width) | 1583AF, 1584AD, 158608, 1405B4, 13EA75, 15BBDE, 1550ED | the 3 projectors (poly.md), model verts, terrain grid, textured setup, flat gradients |
| hh (half height) | 1583C5, 1584C3, 15861E, 1405CC, 13EA8D, 15BBE5, 1550F7 | the same |
| cx | 12A931 | `xn_render_frame` (`sub ebx, cx`) |
| cx - 1 | 157622, 157802, 157B22, 157E22 | the four flat spans |
| (cx << 8) + 80h | 1583B7, 1584B5, 1405BC, 13EA7D, 158610 | projectors, model verts, terrain |
| (cy << 8) + 80h | 1583CD, 1584CB, 1405D4, 13EA95, 158626 | the same |

**Readable C** (one assignment per field; the field names are in `patch_fields.csv`):

```c
/* camera.c */
void xn_cam_set_view_window(s32 hw, s32 hh, s32 cx, s32 cy)   /* pushad: preserves all */
{
    s32 sx = (cx << 8) + 0x80, sy = (cy << 8) + 0x80;

    xn_cam_half_width = hw;  xn_cam_half_height = hh;
    xn_cam_centre_x = cx;    xn_cam_centre_y = cy;
    xn_poly_flat_sx = xn_poly_face_sx = xn_poly_terrain_sx = hw;
    xn_model_vert_sx = xn_tgrid_sx = xn_poly_setup_sx = xn_flat_grad_sx = hw;
    xn_poly_flat_sy = xn_poly_face_sy = xn_poly_terrain_sy = hh;
    xn_model_vert_sy = xn_tgrid_sy = xn_poly_setup_sy = xn_flat_grad_sy = hh;
    xn_render_frame_centre_x = cx;
    xn_span_flat_transparent_cx = xn_span_flat_shaded_cx = cx - 1;
    xn_span_flat_fogged_cx = xn_span_flat_translucent_cx = cx - 1;
    xn_poly_flat_cx = xn_poly_face_cx = xn_poly_terrain_cx = sx;
    xn_model_vert_cx = xn_tgrid_cx = sx;
    xn_poly_flat_cy = xn_poly_face_cy = xn_poly_terrain_cy = sy;
    xn_model_vert_cy = xn_tgrid_cy = sy;
    xn_cam_update_derived();
}

void xn_render_init(void)
{
    u32 *t;
    s32 k;
    ...                                   /* divide handler, view window, focal, squares,
                                             size masks, set_mode(8): plain C */
    t = xn_malloc(0x40004);
    xn_render_recip_table = t;
    xn_span_recip_fields_set(t);          /* the 25 fields, each a named extern */
    for (k = 0x10000; k >= 1; k--)        /* t[k] = 2^37 / (k << 13); t[0] stays unwritten */
        t[k] = xn_udiv64(0x20, 0, (u32)k << 13);
}
```

`xn_span_recip_fields_set` is 25 named assignments. A table of 25 pointers to the fields, in
C data, is just as exact, because the C region is not compared.

**Tests:** the writers' own records. Every record of a reader that runs after a writer in the
same capture tests the pair, so the frame functions' records (28 each) test them all.

**Traps.**
- **Field widths.** All of these are dwords except 15BBB9 (`shr edx, imm8`), a byte. A dword
  store there would overwrite `jne` after it.
- `xn_render_init`'s 1/z table loop runs k from 65536 down to 1 (`loop`, ecx = 10000h). The
  entry for k = 0 is never written, and the C must not write it. The divides are 64/32 `div`
  (rule DIV) and do not fault for k >= 1.
- `xn_cam_update_derived` also fills `xn_cam_dir_x_table` (1024 dwords) and
  `xn_cam_dir_y_table` (768) with `(k << 18) / focal` (`shld`/`shl`/`idiv`). These are memory
  writes, but not code.
- The begin-frame ambient row: `(xn_light_ambient & ~0xFF)` clamped to 0..3F00h (signed), plus
  `xn_shade_table`.

## Group RENDER-FRAME: `xn_render_frame` (12A870) and its run-time blocks

The blocks are 12A949 `_row_end`, 12A94E `_next_row`, 12A969, 12A975, 12A976, 12A97C and
12A97E. They are one function: the tracer saw them entered by `call` and fall-through.

**What.**
1. It draws the queued models (12A7D0).
2. It fills the background (12A98B).
3. It walks every screen row's span list from the clip top: for each span node, it calls
   `poly->span_fn`, then `xn_render_span_hook` (the fog).
4. It draws the flats (12A814).

It returns 0, or 1 with CF.

**Mechanism.**
- Self-patched:

  | Field | Operand | Value |
  |---|---|---|
  | 12A950 | `add edi, WIDTH` | `xn_gfx_width` |
  | 12A963 | `cmp [xn_render_row_y], END` | `-(clip_top - centre_y)` (the rows are symmetric about the centre) |

- Read: 12A931, `sub ebx, CENTRE_X` (12A2D0).
- Written per row, for the lit spans: 155944, 15599B, 155A4B, 156AC9, 156BAE and 156C59,
  all = `xn_cam_dir_y_mid[xn_render_row_y]`.
- **Stack unwinding instead of a loop exit.** Each row's list ends with the sentinel node
  (made by `xn_render_begin_frame`). Its polygon's routine is 12A949 itself: `add esp, 18h`
  drops the span call's return address and the 5 dwords pushed for it, pops the row's
  edi/esi, and falls into the next row.

**Readable C.**

```c
/* frame.c */
extern s32 xn_render_frame_centre_x;        /* 12A931 (PF, 12A2D0) */
extern u32 xn_render_frame_width;           /* 12A950 KEEP */
extern s32 xn_render_frame_end_row;         /* 12A963 KEEP */

s32 xn_render_frame(s32 flags)              /* returns 0, or 1 (+ CF) */
{
    xn_span **row;
    u8 *dst;
    s32 ray;

    xn_render_frame_flags = flags;
    if (xn_tex_cache_full || xn_render_draw_models())
        return 1;
    mem_check_heap(0x206);
    xn_render_fill_background();
    mem_check_heap(0x207);
    xn_render_row_y = xn_gfx_clip_top - xn_cam_centre_y;
    XN_KEEP(xn_render_frame_end_row, -xn_render_row_y);
    dst = screen_buffer + xn_gfx_row_offset[xn_gfx_clip_top] - 1;
    XN_KEEP(xn_render_frame_width, xn_gfx_width);
    row = &xn_render_span_rows[xn_gfx_clip_top].head;
    do {
        xn_span *node, *next;

        ray = xn_cam_dir_y_mid[xn_render_row_y];
        xn_span_solid_lit_ray_a = xn_span_solid_lit_ray_b = xn_span_solid_lit_ray_c = ray;
        xn_span_texlit_ray_a = xn_span_texlit_ray_b = xn_span_texlit_ray_c = ray;
        for (node = *row; ; node = next) {
            xn_poly *poly = node->poly;
            s32 x0 = node->x_start;

            next = node->next;
            if (poly->span_fn == XN_ASM(xn_render_frame_row_end))
                break;                      /* the sentinel: the asm unwinds the stack here */
            xn_span_call(poly->span_fn, poly, node, x0 - xn_render_frame_centre_x,
                         node->x_end - x0, dst + x0);
            xn_span_hook_call(xn_render_span_hook, poly, node, node->x_end - x0, dst + x0);
        }
        dst += xn_render_frame_width;
        row += 4;                           /* 16-byte row heads */
    } while (++xn_render_row_y != xn_render_frame_end_row);
    mem_check_heap(0x208);
    return xn_render_draw_flats() ? 1 : 0;
}
```

`xn_span_call` / `xn_span_hook_call` are the infra's register-file calls (`xn_asmcall`) to a
routine that may be asm or C. They reach a C span routine through its routed asm entry.

**Conversion group:** 12A870 with all its run-time blocks (12A949, 12A94E, 12A969, 12A975,
12A976, 12A97C, 12A97E). The C must not call 12A949: it would unwind the C frame.
- The ray writes make the C `xn_render_frame` independent of whether the lit spans are asm or C.
- Tests:
  - 12A870: 28 records;
  - every span routine runs inside them, and so do 12A7D0 / 12A814 and the fog hook.

**Exactness traps.**
- The sentinel test must compare the routine pointer with the asm address 12A949 (the
  begin-frame stores the literal 12A949 in the background polygon), not with a C function's
  address.
- `next` is read before the span routine is called (`push dword ptr [esi]`). Keep that order;
  a setup routine rewrites `poly->span_fn`, not the node.
- The span routine gets `edi = dst + x0` where dst is the row start - 1. The hook gets the same
  edi, ebx = poly (popped from the pushed eax), ebp = n, esi = node.
- `mem_check_heap` is called with `pushad; pushfd ... popfd; popad` around it, so the C call
  must not let it change anything the asm keeps. In C, there is nothing live across it but
  locals.
- The CF result: `xn_render_frame` returns CF as well as eax. Its Watcom callers read eax only
  (the ABI row shows no flags_out), so a plain `return` is enough.
