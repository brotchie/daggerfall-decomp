/* xwshare.h: what the world group's modules (collide, world, terrain, sky, water: canonical
   C, docs/xngine_canonical.md) read of other groups' data: their globals, declared here with
   the types their owners give them, until the coordinator moves each into its owner's header
   (or xngine.h). The functions of other groups come from their owners' headers (xrand.h,
   xdos.h, xflat.h, xtex.h, xpoly.h, xmath.h...), never from here: a call into another group
   uses that group's current prototype. */
#ifndef XWSHARE_H
#define XWSHARE_H

#include "xngine.h"
#include "xnstruct.h"

/* ---- the 2D screen (gfx group) ---------------------------------------------------------- */
extern s32 xn_gfx_clip_left;            /* the clip window: left, top inclusive */
extern s32 xn_gfx_clip_top;
extern s32 xn_gfx_clip_right;           /* right, bottom exclusive */
extern s32 xn_gfx_clip_bottom;
extern s32 xn_gfx_width;                /* bytes of a screen row */
extern s32 xn_gfx_row_offset[768];      /* y * screen width */
extern u8 *screen_buffer;               /* the 8-bit frame buffer (320 wide) */
extern u8 *big_buffer;                  /* the shared work buffer (game-visible: the collision
                                           hit lists, the sky image...) */

/* ---- the view (cam group; struct xn_view at 0xCEA20 as separate globals) ------------------ */
extern s32 xn_cam_x, xn_cam_y, xn_cam_z;        /* the eye, world units */
extern s32 xn_cam_yaw;                  /* 2048ths of a turn */
extern xn_mat3 xn_cam_rotation;         /* the eye's rotation, 2.28 */
extern xn_mat3 xn_cam_view_matrix;      /* the rotation with rows 0 and 1 scaled by the view */
extern s32 xn_cam_near_z, xn_cam_far_z; /* the near and far planes, camera units (24.8) */
extern s32 xn_cam_centre_x, xn_cam_centre_y;    /* the screen point of the view axis */
extern s32 xn_cam_half_width, xn_cam_half_height;
extern s32 xn_cam_focal_x, xn_cam_focal_y;      /* the projection scales, pixels */
extern s32 xn_cam_inv_focal_x;          /* 2^38 / focal_x */
extern s32 xn_cam_scale_x, xn_cam_scale_y;      /* the view's scales, 2.14 */

/* The shared scratch vectors (0x120288; xnstruct.h): pick_distance (a.z) is game-visible, and
   the terrain's face planes leave a value there (xterrain.h) */
extern struct xn_scratch xn_scratch_vecs;

/* ---- the game's ------------------------------------------------------------------------- */
extern u32 frame_ticks;                 /* milliseconds of the last frame */

#endif
