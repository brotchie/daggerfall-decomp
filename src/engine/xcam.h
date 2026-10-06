/* xcam.h: XnGine's camera (src/engine/cam.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     The view: the 3D window's half size and centre on the screen, the focal lengths, and what
     derives from them (the view matrix's row scales, the side planes' normals, the per-column
     and per-row ray tables); the projection of a camera-space point; the view matrix made
     from the eye's rotation; the sphere test against the view volume (models and lights).

   Fixed point and units
     camera space  world units << 8 (24.8) through xn_cam_rotation (2.28); the view space is
                   the same with rows 0 and 1 scaled by xn_cam_scale_x/_y (2.14: focal *
                   2^14 / half size), so that the frustum's side planes are x = +-z, y = +-z
     focal         pixels: a screen offset is x * focal / z (the game sets 200, 180)
     screen        a projected vertex: x << 5 (5 fraction bits), the row, 1/z = 2^40 / z
     normals       16.16: (focal_x, 0, half_w) and (focal_y, half_h, 0) normalised
     ray tables    (k << 18) / focal for k = -512..511 (columns) and -384..383 (rows)

   Tables and state (object 2): the view globals of xpipe.h (struct xn_view at 0xCEA20; the
   game reads the centre, sets the near and far planes, and nothing else writes them), the
   ray tables xn_cam_dir_x_table/_y_table (the lit span routines read them), the eye's
   rotation and view matrix, and the shared scratch vector a (the pick's point; pick_distance
   is game-visible).

   Every projector of the engine (the polygons', the models', the terrain's, the water's)
   reads its constants from the view globals: the half size, and the centre << 8 + 80h (the
   rounding half). The asm patched copies of them into each projector's code; the view is
   the one copy here. The asm divides that can overflow give 0 (Q-SYS-01).

   Quirks kept: Q-CAM-01 (the dead in-place scale scales m[2][2] for m[1][2]), Q-CAM-02 (the
   sphere test's leftover reaches the game through xn_light_add). docs/engine/quirks.md. */
#ifndef XCAM_H
#define XCAM_H

#include "xpipe.h"

extern s32 xn_cam_dir_x_table[1024];    /* (k << 18) / focal_x for k = -512..511 */
extern s32 xn_cam_dir_y_table[768];     /* (k << 18) / focal_y for k = -384..383 */
/* the view's forward direction: xn_cam_rotation's row 2 (one storage) */
#define xn_cam_forward_x (xn_cam_rotation.m[2][0])
#define xn_cam_forward_y (xn_cam_rotation.m[2][1])
#define xn_cam_forward_z (xn_cam_rotation.m[2][2])

/* ---- the view ----------------------------------------------------------------------------- */

/* Sets the focal lengths (pixels; the game passes 200, 180) and their reciprocals 2^38 / f,
   then the derived values (xn_cam_update_derived). Three game sites, and xn_render_init. */
void xn_cam_set_focal(s32 focal_x, s32 focal_y);

/* Sets the 3D view's half size and centre on the screen (the world view: 160, 77, 160, 77;
   full screen 160, 100, 160, 100; the bank's preview 51, 45, 216, 72; the automap 160, 84,
   160, 85), then the derived values (xn_cam_update_derived). 13 game sites. */
void xn_cam_set_view_window(s32 half_w, s32 half_h, s32 centre_x, s32 centre_y);

/* From the view window and the focal lengths: the view matrix's row scales (focal * 2^14 /
   half size) and their 2^45 reciprocals, the flats' scales ((half size << 32) / focal^2 / 2),
   the side planes' normals, and the ray tables. */
void xn_cam_update_derived(void);

/* ---- projection ---------------------------------------------------------------------------- */

/* The screen position of the camera-space point (x, y, z) (view space: x and y scaled), as
   every projector computes it: *inv_z = 2^40 / z (0 when z <= 100h, Q-SYS-01); *sx =
   (hi(x * half_w * inv_z) + centre_x << 8 + 80h) >> 3, the column << 5 (rounded); *sy = the
   same for y >> 8, the row. The products x * half_w are 32-bit, the shifts unsigned (a point
   outside the view would come out huge, not negative: callers clip first). */
void xn_cam_screen_point(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, u32 *inv_z);

/* (*sx, *sy) = x * focal_x / z and y * focal_y / z (64-bit products; 0 where the divide would
   overflow, Q-SYS-01): the offsets of the camera-space point from the view centre. Three game
   sites (the moons, the pick); the model's screen bounds. */
void xn_cam_project_ptr(s32 x, s32 y, s32 z, s32 *sx, s32 *sy);
void xn_cam_project(s32 x, s32 y, s32 z, s32 *sx, s32 *sy);

/* Dead: the same for a view-space point: x * half_w / z and y * half_h / z, rounded (+ 80h
   before the divide). */
void xn_cam_project_scaled(s32 x, s32 y, s32 z, s32 *sx, s32 *sy);

/* ---- the view matrix ------------------------------------------------------------------------ */

/* dst = src with rows 0 and 1 times the view scales (64-bit products >> 14), row 2 copied:
   the game makes xn_cam_view_matrix from xn_cam_rotation with it (7 game sites); the flats'
   matrix. src and dst may be the same. */
void xn_cam_scale_matrix(const xn_mat3 *src, xn_mat3 *dst);

/* Dead: in place, rows 0 and 1 of m times the view scales (>> 14); m[2][2] is scaled where
   m[1][2] belongs (Q-CAM-01). */
void xn_cam_scale_matrix_in_place(xn_mat3 *m);

/* Dead: in place, rows 0 and 1 of m divided by the view scales (<< 14, 64-bit; 0 where the
   divide would overflow): the inverse of xn_cam_scale_matrix. */
void xn_cam_unscale_matrix(xn_mat3 *m);

/* Dead: the angles of the view direction (xn_cam_rotation's row 2): the pitch to *pitch, the
   yaw to *yaw (xn_vec_dir_to_angles_regs). */
void xn_cam_forward_angles(s32 *pitch, s32 *yaw);

/* ---- the sphere test ---------------------------------------------------------------------- */

/* Is the sphere at (x, y, z) (relative to the eye, 24.8) with the given radius outside the
   view? Moves the centre into view space and leaves it for the pick (xn_pick_view_x/_y
   unscaled, pick_distance = z: game-visible), then tests the sphere against each plane its
   centre is outside of (near, far, left, right, bottom, top). Returns 1 when it is culled.
   *residue: the last value the test computed (the asm's EAX): the low dword of y * 2 * the
   inverse y scale when the centre is inside, else the last plane's term (Q-CAM-02: the
   lights' xn_light_add returns it to the game). Models (xn_model_cull_and_queue) and
   lights. */
int xn_cam_cull_sphere(s32 x, s32 y, s32 z, s32 radius, s32 *residue);

/* The distance of the sphere's centre (as xn_cam_cull_sphere left it: xn_pick_view_x and
   pick_distance) from a left or right side plane with the normal (nx, nz) (16.16), made
   negative: -|nx * x + nz * z| >> 16 (each product's bits 16..47, the sum 32-bit). The sphere
   is outside when that plus its radius is not positive. *low: the low dword of
   nz * pick_distance (the test's residue). */
s32 xn_cam_sphere_dist_x(s32 nx, s32 nz, s32 *low);

/* The same for the top and bottom planes (xn_pick_view_y). */
s32 xn_cam_sphere_dist_y(s32 ny, s32 nz, s32 *low);

#endif
