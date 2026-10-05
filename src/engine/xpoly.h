/* xpoly.h: XnGine's polygon functions (readable C, poly.c; see xngine.h): outcodes, the
   Sutherland-Hodgman clipper against the view volume (x = +-z, y = +-z, near, far), the three
   projectors (flats, model faces, terrain), the edge walker that turns a projected polygon
   into S-buffer spans, and the textured polygon's first-span setup.

   Each declaration keeps the function's asm interface (its row in config/xngine_abi.csv): no
   pragma is Watcom's own convention; a pragma names the registers; NAME_r is the glue for a
   function whose asm callers read several registers or flags. */
#ifndef XPOLY_H
#define XPOLY_H

#include "xpipe.h"

/* the clipper's state (struct xn_poly_clip_state at 0x158200, as separate globals) */
extern struct xn_poly_vertex *xn_poly_clip_src, *xn_poly_clip_dst, *xn_poly_clip_src_end;
extern xn_routine xn_poly_clip_intersect_fn;   /* the plane's intersect routine (asm entry) */
extern u8 xn_poly_clip_outcode_and, xn_poly_clip_outcode_or;
extern struct xn_poly_vertex xn_poly_vertex_buf_a[32], xn_poly_vertex_buf_b[32];
extern s8 xn_poly_vertex_count;         /* counted down by the edge walkers */

/* The outcode of a camera-space point: 1 x < -z, 2 x > z, 4 y > z, 8 y < -z, 10h z < near,
   20h z > far. (The asm leaves it in ECX; z stays in EBX.) */
u32 xn_poly_outcode(s32 x, s32 y, s32 z);
void xn_poly_outcode_r(xn_regs *r);

/* The same, in EAX (no caller). */
u32 xn_poly_outcode_eax(s32 x, s32 y, s32 z);
void xn_poly_outcode_eax_r(xn_regs *r);

/* The intersection of the edge from `in` (inside the plane) to `out` (outside) with a plane of
   the view volume, written at *dst with its outcode, which also goes into the clipper's OR and
   AND: x = -z, x = z, y = z, y = -z, z = near, z = far. The parameter of the cut is a 32-bit
   fraction from a 64-bit divide; the coordinates move from `in` by the high dword of twice the
   difference times it. The asm interface: ebx = in, esi = out, edi = dst (advanced by 16);
   the new x, y, z and the outcode are left in eax, edx, ebx and ecx (the water's clipper
   12F59C calls them too). */
void xn_poly_clip_intersect_left(const struct xn_poly_vertex *in,
                                 const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_left_r(xn_regs *r);
void xn_poly_clip_intersect_right(const struct xn_poly_vertex *in,
                                  const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_right_r(xn_regs *r);
void xn_poly_clip_intersect_bottom(const struct xn_poly_vertex *in,
                                   const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_bottom_r(xn_regs *r);
void xn_poly_clip_intersect_top(const struct xn_poly_vertex *in,
                                const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_top_r(xn_regs *r);
void xn_poly_clip_intersect_near(const struct xn_poly_vertex *in,
                                 const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_near_r(xn_regs *r);
void xn_poly_clip_intersect_far(const struct xn_poly_vertex *in,
                                const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_far_r(xn_regs *r);

/* Clips the polygon xn_poly_clip_src..src_end against each plane its outcode OR names (near,
   far, left, bottom, right, top), one pass each into the other buffer. Returns 1 (the asm's CF)
   when the polygon is left inside every plane, 0 when it is outside one (rejected). */
int xn_poly_clip_frustum(void);
void xn_poly_clip_frustum_r(xn_regs *r);

/* One Sutherland-Hodgman pass against the plane bit `plane` with the routine in
   xn_poly_clip_intersect_fn: the vertices inside are copied to xn_poly_clip_dst, each vertex
   outside is replaced by the cuts of its edges to inside neighbours; the outcodes' OR and AND
   are recomputed; then the buffers swap. (The asm tests the plane against the outcode byte
   with EAX's upper bits above it: for a plane above FFh, which no caller passes, it differs.) */
void xn_poly_clip_plane(u32 plane);
void xn_poly_clip_plane_r(xn_regs *r);

/* A flat's quad (4 vertices at `quad`, the flat in xn_flat_emit_flat): clipped when its
   outcode OR says so, projected in place, then walked by xn_flat_raster from its top vertex.
   (The row's EBP input is the asm's top index when no vertex is above row 7FFFh, which a
   projected vertex always is: the C starts at 0; its other inputs only pass through.) */
void xn_poly_project_flat(struct xn_poly_vertex *quad);
#pragma aux xn_poly_project_flat parm [esi] modify exact [eax ecx edx ebx esi edi];

/* A model face: its vertices from the model's transformed arrays (xn_vert_cam, or, when no
   vertex is outside (codes' high byte, the outcodes' OR, is 0), the already projected
   xn_vert_screen), clipped, projected and walked into spans. codes: the outcodes' AND in the
   low byte, OR in the high byte. Returns 1 (the asm's CF) when spans were added. A clipped face
   of more than 27 vertices is dropped. */
int xn_poly_project_face(const struct xn_model_face *face, u32 codes);
void xn_poly_project_face_r(xn_regs *r);

/* A stub that returns CF clear (no caller). */
void xn_poly_clc_stub_r(xn_regs *r);

/* A terrain polygon of `bytes` / 16 vertices built in xn_poly_vertex_buf_a (outcodes set by the
   terrain): clipped, projected and walked into spans. (The row's EBP input is the asm's top
   index when no vertex is at or above row 7FFFFFFFh, which cannot happen; the others only pass
   through.) */
void xn_poly_project_terrain(u32 bytes);
#pragma aux xn_poly_project_terrain parm [ecx] modify exact [eax ecx edx ebx esi edi];

/* An edge walker's state, kept in its own code: the ring pointers of its left and right edges
   and their slopes (xn_poly_rasterize's and xn_flat_raster's own fields) */
typedef struct xn_walker {
    struct xn_poly_vertex ***left, ***right;
    s32 *dxl, *dzl, *dxr;
} xn_walker;

/* The next left edge with rows (forward round the ring, counting xn_poly_vertex_count
   down): its rows into the low word of *rows (a 16-bit difference, kept even for an edge
   with none), its x (the 5-bit screen x << 16: 11.21) and 1/z, its slopes (x by
   xn_recip16_table, 1/z by xn_recip32_table). 0 when the vertices run out. */
int xn_walk_left_edge(const xn_walker *w, u32 *rows, s32 *xl, s32 *zl);

/* The next right edge with rows (backward round the ring): its rows into the high word of
   *rows, its x and slope. 0 when the vertices run out. */
int xn_walk_right_edge(const xn_walker *w, u32 *rows, s32 *xr);

/* The edge walker: from the top vertex (screen row top_y, its place `ring` in a ring of
   vertex pointers that repeats the polygon), walks the left edge forward and the right edge
   backward, row by row, and inserts each row's span (x from the left edge to the right, 1/z
   from the left) into the S-buffer. Its edge state is in its own code (xn_raster_left/right,
   the slopes xn_raster_dxl/dzl/dxr). */
void xn_poly_rasterize(s32 top_y, struct xn_poly_vertex **ring);
void xn_poly_rasterize_r(xn_regs *r);

/* An entry inside xn_poly_rasterize's row loop that nothing reaches: the walk from a row with
   the asm's registers (esi = the row, edi = the row counts, ebp/ebx = left/right x, ecx = 1/z). */
void xn_poly_rasterize_row_loop_r(xn_regs *r);

/* The texture gradients of a model face: its texture axes (object space) through the model's
   scaled matrix give u/z and v/z per screen x (+24h, +30h; their 8-pixel steps +50h, +54h),
   per row (+28h, +34h) and at the centre (+2Ch, +38h). Each is a sum of three products' high
   dwords (not a 64-bit sum). */
void xn_poly_tex_gradients(const xn_mat3 *m, const struct xn_model_face_data *axes,
                           struct xn_poly *poly);
#pragma aux xn_poly_tex_gradients parm [ecx] [edx] [edi] modify exact [eax ecx edx ebx esi];

/* The first span of a textured model face (through its span routine pointer, with the span's
   registers): the texels, the wrap masks and the compiled mapper from its image; the lighting;
   the 8- or 16-pixel span routine (16 when |d(1/z)/dx| >> xn_poly_subdiv_shift is 0); the
   gradients; the packed texture origin (+18h). Then the span routine runs on the same span. */
void xn_poly_setup_textured_r(xn_regs *r);

/* A lone `ret` (no caller). */
void xn_poly_ret_stub(void);
#pragma aux xn_poly_ret_stub modify exact [eax];

#endif
