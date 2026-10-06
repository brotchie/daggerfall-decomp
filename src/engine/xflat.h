/* xflat.h: XnGine's flats, the billboard sprites (src/engine/flat.c). Canonical C: plain
   prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     The game queues flats each frame (xn_flat_add: a world position, an image, a frame, a
     kind and a scale); the renderer draws them back to front (xn_render_draw_flats, after
     xn_flat_begin_frame). A flat is drawn as a quad facing the eye, centred on its position
     or standing on it: clipped, projected (xn_poly_project_flat) and walked row by row like a
     polygon, each row cut against the S-buffer's spans by depth (xn_flat_span_clip) and the
     visible pieces drawn by the flat's span routine (the rasteriser group's xn_span_flat_*).
     The first piece of every flat goes to its light setup instead, which picks the span
     routine and draws nothing (Q-FLAT-01). The pick finds the nearest flat under a screen
     point.

   Fixed point and units
     a flat        struct xn_flat in the polygon pool: its view-space position (24.8, the view
                   matrix), its image (archive << 7 | record), quad kind (flags & 1Fh; 20h
                   mirrored), scale (100h = 1.0; its high bytes the light) and, once drawn, its
                   texture, gradients and span routine
     corners       offsets of the quad's half size << 4 through xn_flat_matrix (the camera's
                   pitch scaled by the view), high dwords
     gradients     u/z and v/z per screen x and row and at the view centre: the frame's screen
                   axes (2^30 on x or y through xn_flat_proj_matrix) times 2^33 / scale
     rows          the row walk steps the flat's u/v constants by u_dy and v_dy - 8 a row

   Tables and state (object 2): the flat sort list (xn_flat_sort_list, xn_flat_sort_end:
   {key -z, flat} pairs) and counts (xn_flat_count, capped at 512; xn_flat_drawn_count), the
   frame's matrices (xn_flat_matrix, xn_flat_proj_matrix: xn_flat_begin_frame), the pick's
   results (xn_pick_flat_x/_y/_z) and point (xn_scratch_vec_b), the polygon pool and the
   clipper's buffer (xpoly.h), the lights (xn_light_table) and the fog. The asm passed the
   flat being drawn, its row and its per-row steps through operands of its own code (smc
   FLAT-*); here a walk (xn_flat_walk) carries them.

   Quirks kept: Q-FLAT-01 (the first piece of a flat is not drawn), Q-FLAT-02 (a flat not
   queued returns its scale argument), Q-FLAT-03 (a quad-less kind draws the last flat's quad
   when that one was not wholly outside the view), Q-FLAT-04 (a directional light is added
   again and again until the shade row is the last), Q-FLAT-05 (the pick reads a missing
   image's header at address 0). Dropped: Q-FLAT-06 (kinds 17-31 read past the quad table).
   docs/engine/quirks.md. */
#ifndef XFLAT_H
#define XFLAT_H

#include "xpoly.h"

/* A flat being drawn, as its row walk carries it */
typedef struct xn_flat_walk {
    struct xn_flat *flat;       /* the flat */
    u32 tex_w;                  /* its image's width (a mirrored flat's u offset) */
    u8 *row;                    /* the first pixel of the screen row being walked */
    s32 u_step, v_step;         /* the flat's u/v constants' steps a row */
    xn_edge_walk edges;         /* the quad's edges (xpoly.h) */
} xn_flat_walk;

/* ---- queueing ------------------------------------------------------------------------------ */

/* Queues a flat at the world position (x, y, z) for this frame (xn_flat_add_body). Returns
   the flat, or, when it is not queued (behind the eye, beyond the far plane, or the 513th),
   the scale argument (Q-FLAT-02). 14 game sites. The game keeps the result as the object's
   draw handle: an address (uptr). */
uptr xn_flat_add(s32 x, s32 y, s32 z, u32 image, s32 frame, u32 flags, u32 scale);

/* The flat at (x, y, z) into view space ((p - eye) << 8 through the view matrix), then a
   polygon record: image (archive << 7 | record), frame (the texture lookup's: -1 by the
   animation clock), flags (bits 0-4 the quad kind, 20h mirrored), scale (100h = 1.0; its
   third byte the light), and an entry of the flat sort list (key -z). Returns the flat, or
   the scale argument when it is not queued (Q-FLAT-02). */
uptr xn_flat_add_body(s32 x, s32 y, s32 z, u32 image, s32 frame, u32 flags, u32 scale);

/* Queues a flat already in view space: frame 0, standing (kind 4), scale 100h. The terrain's
   nature flats. */
void xn_flat_add_view(s32 x, s32 y, s32 z, u32 image);

/* ---- drawing ------------------------------------------------------------------------------ */

/* Each frame, before the flats: the flat matrix (the camera's pitch, unless
   xn_flat_ignore_pitch is 64h, through the view scales) and its projection version (rows 0
   and 1 times the flats' scales, row 2 halved). */
void xn_flat_begin_frame(void);

/* Draws a queued flat: its texture frame (and the translucency table of its archive), the
   scale plus the image's own, the quad; unless the quad is wholly outside one plane, its
   first corner for the pick (xn_pick_view_x/_y, pick_distance: the corner >> 3), the light
   setup as its span routine, and the quad projected and walked. Returns 1 when the texture
   cache failed (it is full). xn_render_draw_flats. */
int xn_flat_draw(struct xn_flat *flat);

/* The quad builders, by the flat's kind (flags & 1Fh: 0-1 centred, 4-7 standing, the others
   none: Q-FLAT-03), with the size in view units (w, h: the image size times the scale): four
   corners in xn_poly_vertex_buf_a from the flat's position and xn_flat_matrix, with their
   outcodes, and the clipper's AND and OR over them. Centred: half the size each way;
   standing: the base on the flat's position (its corners moved along x only), the top h
   above it. */
void xn_flat_quad_centred(u32 w, u32 h, const struct xn_flat *flat);
void xn_flat_quad_standing(u32 w, u32 h, const struct xn_flat *flat);
/* The kinds without a quad: nothing (the asm's three `ret`s in the table) */
void xn_flat_quad_none(void);
void xn_flat_quad_none_8(void);
void xn_flat_quad_none_16(void);

/* A corner offset (ox, oy) of the quad through xn_flat_matrix (rows 0 and 1, and m[2][1]: a
   pitch has no other z term): the offsets << 4 times the entries, high dwords, into *out. */
void xn_flat_rotate_offset(s32 ox, s32 oy, xn_vec3 *out);

/* The flat's texture gradients, from its scale (2^33 / scale; 0 for a scale up to 2,
   Q-SYS-01), the frame's screen axes (xn_flat_proj_matrix) and its position (xn_pick_view_x/
   _y, pick_distance, as xn_flat_draw left them): u/z and v/z per x, per row and at the view
   centre, the 8-pixel steps, the offsets; the constants moved to the top row top_y and the
   walk's row steps (u by u_dy, v by v_dy - 8). A mirrored flat (flags 20h) negates its u
   terms and offsets u by the image width << 23. */
void xn_flat_setup_gradients(xn_flat_walk *w, s32 top_y);

/* Walks the flat's projected quad of n vertices from its top vertex (row top_y, its place
   `ring` in the clipper's ring): the gradients, then edge by edge as xn_poly_rasterize does;
   each row's piece, from the backward edge's pixel to the forward edge's less one, is cut
   against the world (xn_flat_span_clip); then the flat's u/v constants and the row step. */
void xn_flat_raster(xn_flat_walk *w, s32 top_y, struct xn_poly_vertex **ring, int n);

/* The walk's row loop from `row` (the S-buffer's head of the current row) with the state w:
   what xn_flat_raster runs once it has its first two edges. (The asm's entry 155332 inside
   xn_flat_raster's loop, which nothing calls.) */
void xn_flat_raster_rows(xn_flat_walk *w, struct xn_span *row);

/* One row's piece of the flat, x0..x1 at depth inv_z, against the row's spans from its head
   `node`: where the flat is nearer than the world (1/z larger), the piece goes to
   xn_flat_span_emit; spans are compared by 16-bit x and their 1/z at x0. */
void xn_flat_span_clip(xn_flat_walk *w, struct xn_span *node, s32 x0, s32 x1, s32 inv_z);

/* Draws the piece x0..x1 of the walk's row with the flat's span routine (inv_z, x0, the
   count x1 - x0, the first pixel). */
void xn_flat_span_emit(xn_flat_walk *w, s32 x0, s32 x1, s32 inv_z);

/* The flat's first span routine (an xn_flat_span_fn: only the flat is used): installs the real
   one and draws nothing (Q-FLAT-01). A translucent flat (a table) gets
   xn_span_flat_translucent. Otherwise the shade row: the flat's light byte as rows plus the
   ambient row (xn_light_ambient_row), plus each of the first 32 lights: a point light in
   range adds intensity * 2^16 * isqrt(d^2) / d^2 (0 at d = 0, Q-SYS-01), a directional one
   its whole intensity again and again (Q-FLAT-04), until the row reaches the last one. Below
   the last row: xn_span_flat_transparent_shaded through that row, or beyond the fog's start
   xn_span_flat_lit_fogged with the fog row; at the last row: xn_span_flat_transparent, or
   beyond the fog's start xn_span_flat_transparent_shaded through the fog row. */
void xn_flat_span_light_setup(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix);

/* ---- the pick ------------------------------------------------------------------------------ */

/* The nearest queued flat whose quad covers the screen point (x, y) (strictly inside its
   first corner's x .. second's, and its first corner's y .. third's, at its first corner's
   depth), with that point in view space in xn_pick_flat_x/_y/_z (z rounded >> 8), or 0. The
   point stays in xn_scratch_vec_b for xn_render_pick. Q-FLAT-05. */
struct xn_flat *xn_flat_pick(s32 x, s32 y);

#endif
