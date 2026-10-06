/* xwater.h: XnGine's dungeon water (src/engine/water.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     After a dungeon frame is drawn, the pixels that lie behind the water plane (y =
     dungeon_water_level: the eye above or below it) are drawn again, each from a neighbour
     on its row jittered by -1, 0 or +1 (a ripple that moves every frame), through the water
     tint table (WATER.TBL). The water line (where the plane meets the view) bounds the rows;
     on each row the S-buffer's spans say where the scene is behind the plane.

   Fixed point and units
     view space   world units << 8 rotated by the view (xn_cam_view_matrix: x and y scaled so
                  the frustum is x = +-z, y = +-z); near and far planes in the same units
     1/z          2^40 / z (the projectors' and the S-buffer's)
     screen       rows of xn_gfx_width bytes; a span's x is a pixel column

   Tables and state (object 2)
     dungeon_water_level                the game's: the plane's y, world units (10000: none)
     xn_water_jitter[512]               -1, 0 or +1 per entry (xn_water_init)
     xn_water_row_phase/_velocity/_base[200]: each screen row's ripple: a phase -4..4 moving by
                                        +-1 a frame (turning at +-4), and its base 8..127 into
                                        the jitter table
     xn_water_quadrant_dirs[4]          per yaw quadrant: the plane's two far points
     xn_water_tint_table                the game's: the tint of each colour (256-aligned)
   The view comes from the cam group (xwshare.h), the S-buffer (xn_render_span_rows) and the
   clipper's intersections (xn_poly_clip_intersect_near/top/bottom) from the 3D pipeline.

   Quirks kept: Q-WATER-01 (a row's x kept to 16 bits), Q-WATER-02 (the bottom clip's bit).
   docs/engine/quirks.md. */
#ifndef XWATER_H
#define XWATER_H

#include "xwshare.h"

/* Per yaw quadrant ((yaw + 256) >> 9 of 2048): the water plane's points 2^20 ahead and behind
   on the quadrant's axis (x0, z0) and (x1, z1), view-space units before the rotation */
typedef struct xn_water_dirs {
    s32 x0, z0, x1, z1;
} xn_water_dirs;
extern xn_water_dirs xn_water_quadrant_dirs[4];

extern s32 dungeon_water_level;         /* the game's: 10000 is no water */
extern s32 xn_water_jitter[512];        /* -1, 0 or +1 */
extern s32 xn_water_row_phase[200];     /* per screen row: the ripple's phase, -4..4 */
extern s32 xn_water_row_velocity[200];  /* +-1, turned beyond +-4 */
extern s32 xn_water_row_base[200];      /* 8..127: the row's offset into the jitter table */
extern u8 *xn_water_tint_table;         /* the game's: 256-aligned, the tint of each colour */

/* Fills the ripple tables from the engine's random numbers: 512 jitters of -1, 0 or +1
   ((r & 63) - 32 clamped: nearly all +-1), and per row a phase -4..3, a base 8..127 and a
   velocity of +-1. One game site (the game's data setup). */
void xn_water_init(void);

/* The water line on screen: the plane's two points along the view's yaw quadrant, at the
   water's height from the eye, in view space with their outcodes (1 left, 2 right, 4 below,
   8 above, 10h before the near plane, 20h beyond the far one), each end cut where it is
   before the near plane (forced to 401h meanwhile), above or below the view (the clipper's
   intersections, written at big_buffer and copied). Returns 1 when the line can be seen: no
   end left above or below the view. Q-WATER-02: the bottom cut does not clear its end's
   "below" bit as the near and top cuts clear theirs (no effect: the cut point is on the
   bottom plane, and its own outcode has no such bit). */
int xn_water_clip_extent(struct xn_poly_vertex *p0, struct xn_poly_vertex *p1);

/* Draws the water: the screen rows between the water line's ends (within the clip window,
   more than 2 of them), the plane's 1/z stepped down them; on each row the ripple moves on,
   and every run of the row behind the plane (between the S-buffer's spans, and wherever a
   span's depth goes past the plane's) is drawn again (xn_water_span). One game site
   (world_render, in a dungeon with water when the eye is not at its level). */
void xn_water_draw(void);

/* Draws pixels x0..x1 - 1 of a screen row again (nothing when x1 <= x0): pixel x becomes the
   tint of pixel x + jitter[x] (xn_water_span_unrolled). row: the row's first pixel; jitter:
   the row's jitter, indexed by x. x1 - x0 at most 640 (the asm reads its step past its table
   beyond that; a row is at most 320 pixels). */
void xn_water_span(u8 *row, const s32 *jitter, s32 x0, s32 x1);

/* The run of a span: for k = 0..n-1, left to right, pix[k] = tint[pix[k + jitter[k]]] (a
   pixel may be read after this run has rewritten it: a jitter of -1 tints it twice). tint:
   256-aligned. */
void xn_water_span_unrolled(u8 *pix, const s32 *jitter, s32 n, const u8 *tint);

/* Dead: a lone ret after the run's code (it does nothing). */
void xn_water_stub_ret(void);

#endif
