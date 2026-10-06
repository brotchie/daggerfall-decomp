/* xpoly.h: XnGine's polygons (src/engine/poly.c). Canonical C: plain prototypes, Watcom's
   own calling convention; docs/xngine_canonical.md.

   What it does
     The path of a polygon from camera space to the S-buffer: outcodes against the view
     volume, the Sutherland-Hodgman clipper (near, far, left, bottom, right, top), the three
     projectors (a flat's quad, a model face, a terrain cell), the edge walker that turns a
     projected polygon into one span per screen row (xn_span_insert, the rasteriser group's),
     and a textured model face's first-span setup: its texture, lighting, span routine and
     texture gradients.

   Fixed point and units
     camera space  view space (xcam.h): x and y scaled so the side planes are x = +-z, y = +-z
     vertex        struct xn_poly_vertex: x, y, z and the outcode; projected in place to x << 5
                   (11.5 pixels), the row and 1/z = 2^40 / z (xn_cam_screen_point)
     edges         x in 11.21 (the pixel << 21) with slopes x by xn_recip16_table (0FFFFh / n),
                   1/z by xn_recip32_table (0FFFFFFFFh / n): one step a row
     clip cut      the edge's parameter as a 32-bit fraction from a 64-bit divide; the new
                   coordinates move from the inside end by the high dword of twice the
                   difference times it
     gradients     u/z and v/z per screen x, per row and at the view centre: the face's texture
                   axes through the model's gradient matrix (high dwords)

   Tables and state (object 2)
     the clipper: xn_poly_clip_src/_dst/_src_end (the polygon being clipped and the buffer it
     goes to), xn_poly_clip_outcode_and/_or (its vertices' outcodes; the flat quads and the
     terrain set them), the two 32-vertex buffers xn_poly_vertex_buf_a/_b, and the rings
     xn_poly_ring_a/_b: by vertex count n, the n vertices' addresses repeated, which the walker
     steps through both ways from the top vertex. The renderer's vertex arrays (xn_vert_cam,
     xn_vert_screen, xn_vert_flags; models' faces index them by byte offset, 12 a vertex) and
     the S-buffer's row heads (xpipe.h).

   The asm kept the walker's edges, the gradients' saved axis components and the projectors'
   view constants in operands of its own code (smc POLY-*); here they are locals and the view
   globals. A divide that would overflow gives 0 (Q-SYS-01).

   Quirks kept: Q-POLY-01 (a model face with every vertex inside is walked with byte-sized
   counts: 1 point walks 256, the ring of n mod 64), Q-POLY-02 (the edge walker counts the
   vertices in a signed byte), Q-POLY-03 (a clipped face of 28 or more vertices is dropped).
   docs/engine/quirks.md. */
#ifndef XPOLY_H
#define XPOLY_H

#include "xpipe.h"

/* ---- the clipper's state ---------------------------------------------------------------- */
extern struct xn_poly_vertex *xn_poly_clip_src;         /* the polygon (struct at 0x158200) */
extern struct xn_poly_vertex *xn_poly_clip_dst;         /* the other buffer */
extern struct xn_poly_vertex *xn_poly_clip_src_end;     /* past its last vertex */
extern u8 xn_poly_clip_outcode_and, xn_poly_clip_outcode_or;   /* over its vertices */
extern struct xn_poly_vertex xn_poly_vertex_buf_a[32], xn_poly_vertex_buf_b[32];
extern struct xn_poly_vertex **xn_poly_ring_a[], **xn_poly_ring_b[];   /* rings by count */

/* An edge's cut with one plane of the view volume (the xn_poly_clip_intersect_* functions):
   the point of the edge from `in` (inside the plane) to `out` (outside) on the plane, at *dst
   with its outcode */
typedef void (*xn_clip_cut_fn)(const struct xn_poly_vertex *in,
                               const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);

/* ---- outcodes --------------------------------------------------------------------------- */

/* The outcode of a camera-space point: 1 x < -z, 2 x > z, 4 y > z, 8 y < -z, 10h z < near,
   20h z > far. The models, the flats' corners, the clipper. */
u32 xn_poly_outcode(s32 x, s32 y, s32 z);

/* Dead: the same (the asm's other entry, which answers in EAX). */
u32 xn_poly_outcode_eax(s32 x, s32 y, s32 z);

/* ---- the clipper ------------------------------------------------------------------------ */

/* The cuts with the planes x = -z (left), x = z (right), y = z (bottom), y = -z (top),
   z = near and z = far: the new vertex's coordinates on the plane are set exactly (z = -x,
   z = x, z = y, z = -y, near, far) and the other two interpolated; its outcode is stored and
   also merged into xn_poly_clip_outcode_or and _and. The clipper; the water's surface (near,
   top, bottom). */
void xn_poly_clip_intersect_left(const struct xn_poly_vertex *in,
                                 const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_right(const struct xn_poly_vertex *in,
                                  const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_bottom(const struct xn_poly_vertex *in,
                                   const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_top(const struct xn_poly_vertex *in,
                                const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_near(const struct xn_poly_vertex *in,
                                 const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);
void xn_poly_clip_intersect_far(const struct xn_poly_vertex *in,
                                const struct xn_poly_vertex *out, struct xn_poly_vertex *dst);

/* One Sutherland-Hodgman pass of the polygon xn_poly_clip_src..src_end against the plane
   whose outcode bit is `plane`, with `cut` for its edges: each vertex inside is copied to
   xn_poly_clip_dst, each vertex outside is replaced by the cuts of its edges to inside
   neighbours; the outcodes' OR and AND are recounted over the result; then the buffers swap
   (the result is the new xn_poly_clip_src). The polygon is closed by copying its first two
   vertices after its last (its buffer needs room for them). */
void xn_poly_clip_plane(u32 plane, xn_clip_cut_fn cut);

/* Clips the polygon xn_poly_clip_src..src_end against each plane its outcodes' OR names, in
   the order near, far, left, bottom, right, top. Returns 1 when what is left is inside every
   plane (or there was nothing to clip), 0 when it is wholly outside one (rejected). */
int xn_poly_clip_frustum(void);

/* ---- the edge walker -------------------------------------------------------------------- */

/* A projected polygon's walk down the screen: its edges' state and the current row's ends */
typedef struct xn_edge_walk {
    struct xn_poly_vertex **left;       /* the ring place of the left edge's upper vertex */
    struct xn_poly_vertex **right;      /* the right edge's */
    s32 dxl, dzl, dxr;                  /* the left edge's x and 1/z steps, the right's x */
    s8 count;                           /* vertices left to take an edge from (Q-POLY-02) */
    u32 rows;                           /* the left edge's rows left (low word), the right's
                                           (high word) */
    s32 xl, xr;                         /* the row's left and right x, 11.21 */
    s32 zl;                             /* 1/z at its left end */
} xn_edge_walk;

/* The next left edge that has rows (forward round the ring, one vertex of the count each):
   its rows into the low word of w->rows (a 16-bit difference; the rows of edges without any
   are left there), its x (the vertex's x << 16) and 1/z, its slopes. 0 when the vertices run
   out. */
int xn_walk_left_edge(xn_edge_walk *w);

/* The next right edge that has rows (backward round the ring): its rows into the high word of
   w->rows, its x and slope. 0 when the vertices run out. */
int xn_walk_right_edge(xn_edge_walk *w);

/* Walks a projected polygon of n vertices into the S-buffer: from its top vertex (screen row
   top_y, its place `ring` in a ring of the polygon's vertex addresses), the left edge forward
   and the right edge backward, one row at a time; each row's span (from the left edge's
   pixel to the right edge's, when that is right of it; 1/z from the left) goes to
   xn_span_insert. The projectors and the terrain. */
void xn_poly_rasterize(s32 top_y, struct xn_poly_vertex **ring, int n);

/* The walk's row loop from `row` (the S-buffer's head of the walk's current row) with the
   state w: what xn_poly_rasterize runs once it has its first two edges. (The asm's entry
   15BA16 inside xn_poly_rasterize's loop, which nothing calls.) */
void xn_poly_rasterize_rows(struct xn_span *row, xn_edge_walk *w);

/* ---- the projectors ----------------------------------------------------------------------- */

/* A flat's quad (4 vertices at `quad`, set up by the quad builders with the clipper's
   outcodes): clipped when its outcodes' OR says so, projected in place, and walked by
   xn_flat_raster (with the flat's walk w, xflat.h) from its top vertex (the first of the
   least rows). The flats' draw. */
void xn_poly_project_flat(struct xn_flat_walk *w, struct xn_poly_vertex *quad);

/* A model face (the outcodes of its vertices: their AND in the low byte of `codes`, OR in
   the high byte). When no vertex is outside a plane (the OR is 0), from the vertices'
   projections (xn_vert_screen: Q-POLY-01); else from their camera-space points, clipped
   (Q-POLY-03) and projected; walked from the top vertex (the last of the least rows). Returns
   1 when spans were added. The model's faces. */
int xn_poly_project_face(const struct xn_model_face *face, u32 codes);

/* A terrain polygon of n vertices in xn_poly_vertex_buf_a (with their outcodes, and the
   clipper's AND and OR set by the terrain): clipped, projected and walked. The terrain's
   cells partly outside the view. */
void xn_poly_project_terrain(int n);

/* ---- the textured model face --------------------------------------------------------------- */

/* A textured model face's texture gradients: its texture axes (object space) through the
   model's gradient matrix m give u/z and v/z per screen x (+24h, +30h; their 8-pixel steps
   +50h, +54h), per row (+28h, +34h) and at the centre (+2Ch, +38h). Each is a sum of three
   products' high dwords (not a 64-bit sum). */
void xn_poly_tex_gradients(const xn_mat3 *m, const struct xn_model_face_data *axes,
                           struct xn_poly *poly);

/* The first span of a textured model face, as a span routine (xn_span_fn: the polygon, the
   span node, x - the view centre, the pixel count and the first pixel): the texels and the
   wrap masks from its image; its lighting (xn_light_setup_poly: 0, 4 or 8); the 8-pixel span
   routine, or the 16-pixel one when |d(1/z)/dx| is small (below 2^18 at 320 pixels wide,
   else 2^25); the gradients; the packed texture origin at the face's first vertex (+18h).
   Then the span routine draws the same span. The render mode's setup for textured faces. */
void xn_poly_setup_textured(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                            u8 *pix);

/* ---- stubs ------------------------------------------------------------------------------- */

/* Dead: returns 0 (the asm's `clc; ret`). */
int xn_poly_clc_stub(void);

/* Dead: a lone `ret`. */
void xn_poly_ret_stub(void);

#endif
