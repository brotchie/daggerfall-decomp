/* xflat.h: XnGine's flats, the billboard sprites (readable C, flat.c; see xngine.h): queued per
   frame with their view-space position, then drawn back to front as a quad facing the eye
   (centred, or standing on its base) that is clipped, projected and walked row by row; each row
   is cut against the world's spans by depth and the visible pieces drawn by the flat's span
   routine, which the light setup installs on the flat's first piece (that piece is not drawn:
   an original quirk). The per-flat state is passed through patched operands (smc/flat.md).

   Each declaration keeps the function's asm interface (its row in config/xngine_abi.csv): no
   pragma is Watcom's own convention; a pragma names the registers; NAME_r is the glue for a
   function whose asm callers read several registers or flags. */
#ifndef XFLAT_H
#define XFLAT_H

#include "xpipe.h"

/* Queues a flat at the world position (x, y, z) (xn_flat_add_body). Returns the flat record,
   or, when it is behind the eye, beyond the far plane or the 512th of the frame, the scale
   argument (the asm's EDI). */
u32 xn_flat_add(s32 x, s32 y, s32 z, u32 image, s32 frame, u32 flags, u32 scale);

/* The flat at (x, y, z), into view space (<< 8, the view matrix), then a polygon record:
   image (archive << 7 | record), frame (for the texture lookup), flags (bits 0-4 the quad
   kind, 20h mirrored), scale (100h = 1.0; its high word the light byte), and an entry of the
   flat sort list (key -z). Returns the record, or the scale argument when not queued. */
u32 xn_flat_add_body(s32 x, s32 y, s32 z, u32 image, s32 frame, u32 flags, u32 scale);
void xn_flat_add_body_r(xn_regs *r);

/* Queues a flat already in view space: frame 0, flags 4 (standing), scale 100h. */
void xn_flat_add_view(s32 x, s32 y, s32 z, u32 image);

/* Draws a queued flat: its texture frame (the translucency table of its archive), the scale
   plus the image's own, the quad, and when the quad is not wholly outside a plane, its centre
   for the pick (xn_pick_view_x/y, pick_distance: the first corner >> 3), the flat for the row
   emitter, the light setup as its span routine, and the quad projected and walked. Returns 1
   (the asm's CF) when the texture cache failed. */
int xn_flat_draw(struct xn_flat *flat);
void xn_flat_draw_r(xn_regs *r);

/* The quad builders of xn_flat_quad_table (by flags & 1Fh), with the size in view units
   (w, h: the image size times the scale): four corners in xn_poly_vertex_buf_a from the flat's
   position and xn_flat_matrix, with their outcodes and the clipper's OR and AND. Centred: half
   the size each way; standing: the base on the flat's position, the top h above, the base's
   corners x-only. (The asm leaves the last corner and its outcode in its registers, and EBP the
   last offset it rotated.) */
void xn_flat_quad_centred(u32 w, u32 h, const struct xn_flat *flat);
void xn_flat_quad_centred_r(xn_regs *r);
void xn_flat_quad_standing(u32 w, u32 h, const struct xn_flat *flat);
void xn_flat_quad_standing_r(xn_regs *r);
/* The kinds without a quad: nothing (the clipper's AND stays as the last flat left it). */
void xn_flat_quad_none(void);
#pragma aux xn_flat_quad_none modify exact [eax];
void xn_flat_quad_none_8(void);
#pragma aux xn_flat_quad_none_8 modify exact [eax];
void xn_flat_quad_none_16(void);
#pragma aux xn_flat_quad_none_16 modify exact [eax];

/* The flat's texture gradients from its scale and the frame's axes (xn_flat_axis_u/v, set by
   xn_flat_begin_frame), at its view position (xn_pick_view_x/y, pick_distance); the u/v
   constants moved to the top row (top_y), and the row walker told which fields to step per row
   and by how much (u by u_dy, v by v_dy - 8). A mirrored flat (flags 20h) negates its u terms. */
void xn_flat_setup_gradients(s32 top_y);
#pragma aux xn_flat_setup_gradients parm [eax] modify exact [eax ecx edx ebx esi edi];

/* One row of a flat, x0..x1 at depth inv_z, against the row's world spans (from the row head
   `node`): the pieces where the flat is nearer go to xn_flat_span_emit. */
void xn_flat_span_clip(struct xn_span *node, s32 x0, s32 x1, s32 inv_z);
void xn_flat_span_clip_r(xn_regs *r);

/* Draws the piece x0..x1 of the current row (xn_flat_emit_row) with the current flat's span
   routine. */
void xn_flat_span_emit(s32 x0, s32 x1, s32 inv_z);
void xn_flat_span_emit_r(xn_regs *r);

/* The flat's quad walker: like xn_poly_rasterize, with its own fields; per row the span is cut
   against the world (xn_flat_span_clip) and the flat's u/v constants and row address step. */
void xn_flat_raster(s32 top_y, struct xn_poly_vertex **ring);
void xn_flat_raster_r(xn_regs *r);

/* An entry inside xn_flat_raster's row loop that nothing reaches: the walk from a row. */
void xn_flat_raster_rows_r(xn_regs *r);

/* A corner offset (ox, oy) of the quad through xn_flat_matrix (rows 0 and 1, and m[2][1]: the
   matrix is a pitch), << 4 and the products' high dwords, into *out. */
void xn_flat_rotate_offset(s32 ox, s32 oy, xn_vec3 *out);
void xn_flat_rotate_offset_r(xn_regs *r);

/* Each frame, before the flats: the flat matrix (the camera pitch, unless xn_flat_ignore_pitch
   is 64h) with the view scales, its projection version (rows scaled by the flat scales, the
   third halved), the row stride (the screen width), and the screen axes u, v of a flat
   (xn_flat_axis_u/v_x/y/z). */
void xn_flat_begin_frame(void);
void xn_flat_begin_frame_r(xn_regs *r);

/* The nearest-first queued flat under the screen point (x, y), with its view point in
   xn_pick_flat_x/y/z, or 0. frame: what the first texture lookup keeps as the current frame
   (the asm's EBX at entry: its caller's leftover). EBP, the row's other input, is only saved
   and restored. */
void *xn_flat_pick(s32 x, s32 y, s32 frame);
#pragma aux xn_flat_pick parm [eax] [edx] [ebx] value [eax] modify exact [eax ecx edx ebx esi edi];

/* The flat's first span routine (through [flat+3Ch], esi = the flat): installs the real one
   and returns without drawing. Translucent flats get 157E20. Otherwise the shade row: the
   flat's light byte as rows plus the ambient row, plus each point light in range (intensity *
   2^16 / sqrt(d^2) / d^2) and each directional light's whole intensity (repeated until the
   row is the last: the asm does not step to the next light); then 157800 (shaded) or, fogged,
   157B20 with the fog row; at the last row 157620 (plain), or 157800 with the fog row. (It
   reads only ESI: the row's other inputs are registers its callers' rows pass through.) */
void xn_flat_span_light_setup(struct xn_flat *flat);
#pragma aux xn_flat_span_light_setup parm [esi] modify exact [eax ecx edx ebx edi];

#endif
