/* xcam.h: XnGine's camera functions (readable C, cam.c; see xngine.h): the view window and
   focal lengths and what derives from them, the projection of a point, the view matrix's row
   scales, and the sphere test against the view volume.

   Each declaration keeps the function's asm interface (its row in config/xngine_abi.csv): no
   pragma is Watcom's own convention; a pragma names the registers; NAME_r is the glue for a
   function whose asm callers read several registers or flags. */
#ifndef XCAM_H
#define XCAM_H

#include "xpipe.h"

/* Dead (no caller): the angles of the view direction (row 2 of xn_cam_rotation): the pitch
   to *pitch, then the yaw to *yaw. */
void xn_cam_forward_angles(s32 *pitch, s32 *yaw);

/* Sets the focal lengths (pixels; the game passes 200, 180) and their reciprocals 2^38 / f,
   patches the model-scale constants 2^36 / focal_x and 2^33 / focal_y into
   xn_model_scale_matrix, then recomputes the derived values (xn_cam_update_derived). */
void xn_cam_set_focal(s32 focal_x, s32 focal_y);
#pragma aux xn_cam_set_focal parm [eax] [edx] modify exact [eax];

/* Sets the 3D view's half size and centre on the screen (the world view: 160, 77, 160, 77)
   and writes them into the code that projects and draws: the projectors, the model and
   terrain transforms, the textured setup and the flat gradients get the half size; the
   frame loop the centre x; the four flat span routines centre x - 1; the projectors, model
   and terrain the centre as 24.8 + 0.5. Then xn_cam_update_derived. */
void xn_cam_set_view_window(s32 half_w, s32 half_h, s32 centre_x, s32 centre_y);

/* From the view window and the focal lengths: the view matrix's row scales (focal * 2^14 /
   half size) and their 2^45 reciprocals, the model depth scale 2^48 / (scale_x * focal_x),
   the flats' scales, the side planes' normals, the per-column and per-row direction tables
   (k << 18) / focal, and the lit spans' 16-pixel ray step 2^22 / focal_x. */
void xn_cam_update_derived(void);

/* Dead: in place, rows 0 and 1 of m times the view scales (>> 14); the asm scales m[2][2]
   where m[1][2] belongs (a bug, kept). */
void xn_cam_scale_matrix_in_place(xn_mat3 *m);

/* (*sx, *sy) = the projection of (x, y, z): offsets from the view centre (xn_cam_project). */
void xn_cam_project_ptr(s32 x, s32 y, s32 z, s32 *sx, s32 *sy);

/* The projection of the view-space point (x, y, z): x * focal_x / z and y * focal_y / z,
   offsets from the view centre (64-bit products; the y divide first). */
void xn_cam_project(s32 x, s32 y, s32 z, s32 *sx, s32 *sy);
void xn_cam_project_r(xn_regs *r);

/* Dead: the same for a point in the scaled view space: x * half_w / z and y * half_h / z,
   rounded (+ 0x80 before the divide). */
void xn_cam_project_scaled(s32 x, s32 y, s32 z, s32 *sx, s32 *sy);
void xn_cam_project_scaled_r(xn_regs *r);

/* dst = src with rows 0 and 1 times the view scales (64-bit products >> 14), so that the
   frustum's side planes become x = +-z and y = +-z: the game makes xn_cam_view_matrix from
   xn_cam_rotation with it. */
void xn_cam_scale_matrix(const xn_mat3 *src, xn_mat3 *dst);

/* Dead: in place, rows 0 and 1 of m divided by the view scales (<< 14, 64-bit): the inverse
   of xn_cam_scale_matrix. */
void xn_cam_unscale_matrix(xn_mat3 *m);

/* Is the sphere at (x, y, z) (relative to the eye, 24.8) with the given radius outside the
   view? Moves the centre into view space, keeps it for the pick (xn_pick_view_x/y unscaled,
   pick_distance = z) and its outcode in xn_poly_clip_outcode_or (when not 0), then tests
   the sphere against each plane the centre is outside of. Returns 1 when culled (the asm's
   CF). *residue: what the asm leaves in EAX (the last value it computed), which
   xn_light_add's callers may read. */
int xn_cam_cull_sphere(s32 x, s32 y, s32 z, s32 radius, s32 *residue);
void xn_cam_cull_sphere_r(xn_regs *r);

/* The distance of the view-space centre (xn_pick_view_x, pick_distance) from a left or
   right side plane with the normal (nx, nz) (16.16), made negative: -|nx * x + nz * z| >> 16.
   The sphere is outside when that plus its radius is not positive (the asm's `add edx, edi`
   and the caller's jle). *low: the low dword of nz * pick_distance (the asm's EAX). */
s32 xn_cam_sphere_dist_x(s32 nx, s32 nz, s32 *low);
void xn_cam_sphere_dist_x_r(xn_regs *r);

/* The same for the top and bottom planes (xn_pick_view_y). */
s32 xn_cam_sphere_dist_y(s32 ny, s32 nz, s32 *low);
void xn_cam_sphere_dist_y_r(xn_regs *r);

#endif
