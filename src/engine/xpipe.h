/* xpipe.h: what XnGine's 3D pipeline shares (src/engine/pipe.c). Canonical C: plain
   prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it is
     The declarations the 3D objects (xcam.h, xpoly.h, xflat.h, xmodel.h, xtex.h) and the
     renderer (xrender.h) have in common: the view (the camera's globals), the frame's pools
     and the S-buffer's row heads, the screen, and a few arithmetic helpers that keep the
     asm's results where its divides would fault.

   Fixed point and units
     camera space   world units << 8 (24.8) rotated by the eye; the view matrix scales x and
                    y so that the frustum's side planes are x = +-z and y = +-z
     screen         a projected vertex: x << 5 (the pixel and 5 fraction bits), the row, and
                    1/z = 2^40 / z
     angles         2048 steps to a turn

   The view (struct xn_view at 0xCEA20, kept as separate globals in object 2): the game reads
   the centre and writes the near and far planes; xn_cam_set_view_window and xn_cam_set_focal
   (xcam.h) write the rest, and every projector reads its constants from here.

   Divisions: XnGine let a divide that overflows fault into its handler, which made the result
   0 (docs/engine/quirks.md Q-SYS-01); the _or0 helpers (here and the rasteriser's xnsmc.h:
   xn_udiv64_or0, xn_idiv64_or0) give that result without the fault. */
#ifndef XPIPE_H
#define XPIPE_H

#include "xngine.h"
#include "xnstruct.h"
#include "xnsmc.h"                    /* xn_udiv64_or0, xn_idiv64_or0 */
#include "doslow.h"                   /* DOS_LOW: real-mode memory */

/* ---- arithmetic (compiler support and the asm's divides; promote to xngine.h) ------------ */

/* Bits 16..47 of a * b: a 16.16 product (imul; shrd 16) */
#ifndef DAGGER_PORT
s32 xn_fixmul16(s32 a, s32 b);
#pragma aux xn_fixmul16 = "imul edx" "shrd eax, edx, 16" parm [eax] [edx] value [eax] \
    modify [edx];
#else
/* natively: the low 32 bits of the shrd */
static __inline__ s32 xn_fixmul16(s32 a, s32 b) { return (s32)(((int64_t)a * b) >> 16); }
#endif

/* The low dword of a * b (what an asm imul leaves in EAX) */
#define XN_LOW32(a, b) ((s32)((u32)(a) * (u32)(b)))

/* (hi:lo) % d, unsigned; 0 where xn_udiv64_or0 (xnsmc.h) gives 0 */
u32 xn_umod64_or0(u32 hi, u32 lo, u32 d);

/* a * b / d with a 64-bit product, truncated; 0 when the quotient does not fit (`imul; idiv`) */
s32 xn_muldiv_or0(s32 a, s32 b, s32 d);

/* ---- routines and tables --------------------------------------------------------------- */

/* A routine the engine keeps as a code address in its data: a polygon's span routine
   (+3Ch), a flat's, an entry of the render-mode tables. The values are the asm entries
   (they are compared with the asm's in the game-visible polygon pool). */
typedef void (*xn_routine)(void);

/* The entry `off` bytes into a table the asm indexes by byte offsets (0, 4, 8...) */
#define XN_AT(type, base, off) (*(type *)((u8 *)(base) + (off)))

/* A pointer the asm reads through without testing it for 0 (a quirk): under DOS a 0 reads the
   real-mode memory at linear 0 (the interrupt vector table); natively the virtual PC's low
   memory there (include/doslow.h), not a fault. The pointer itself under Watcom. */
#if defined(DAGGER_PORT)
static __inline__ void *xn_low_if_null(void *p) { return p != 0 ? p : DOS_LOW(0); }
#define XN_LOW_IF_NULL(p) xn_low_if_null(p)
#else
#define XN_LOW_IF_NULL(p) (p)
#endif

/* ---- the view (struct xn_view at 0xCEA20, as separate globals) ---------------------------- */
extern s32 xn_cam_near_z, xn_cam_far_z;         /* the near and far planes, 24.8 */
extern s32 xn_cam_half_width, xn_cam_half_height;      /* the 3D view's half size, pixels */
extern s32 xn_cam_centre_x, xn_cam_centre_y;    /* the screen point of the view axis */
extern s32 xn_cam_focal_x, xn_cam_inv_focal_x;  /* pixels; 2^38 / focal_x */
extern s32 xn_cam_focal_y, xn_cam_inv_focal_y;
extern s32 xn_cam_scale_x, xn_cam_inv_scale_x;  /* focal * 2^14 / half size; 2^45 / scale */
extern s32 xn_cam_scale_y, xn_cam_inv_scale_y;
extern s32 xn_cam_hfov_cos, xn_cam_hfov_sin;    /* the side planes' normals, 16.16 */
extern s32 xn_cam_vfov_cos, xn_cam_vfov_sin;
extern s32 xn_cam_flat_scale_x, xn_cam_flat_scale_y;       /* unaligned (0xCEA99, 0xCEA9D) */
extern xn_mat3 xn_cam_rotation;         /* the eye's rotation, 2.28 */
extern xn_mat3 xn_cam_view_matrix;      /* the rotation with rows 0 and 1 scaled */
extern s32 xn_cam_x, xn_cam_y, xn_cam_z;        /* the eye, world units */
/* the shared scratch vector a (0x120288): the pick's view-space point; pick_distance (its z)
   is game-visible, and so is xn_pick_view_x */
extern s32 xn_pick_view_x, xn_pick_view_y;
extern s32 pick_distance;

/* ---- the frame's pools and the S-buffer (xn_render_begin_frame resets them) ---------------- */
extern struct xn_poly xn_render_poly_pool[1000];
extern struct xn_poly *xn_render_poly_next;
extern struct xn_model_matrix_pool xn_render_matrix_pool;
extern struct xn_model_matrix_slot *xn_render_matrix_next;
extern struct xn_light_ref xn_render_light_list_pool[400];
extern struct xn_light_ref *xn_render_light_list_next;
extern struct xn_span xn_render_span_sentinel;
extern struct xn_span xn_render_span_rows[];       /* a head per screen row (only .next used) */
extern struct xn_span *xn_render_span_next;
extern s32 xn_render_poly_count;
extern s32 xn_render_mode;              /* 0 outline, 4 solid, 8 textured: a byte offset */
extern s32 xn_render_row_y;
extern s32 xn_render_frame_flags;       /* bit 1: no background fill (outdoors) */

/* ---- the screen (gfx group's globals) ------------------------------------------------------- */
extern s32 xn_gfx_width, xn_gfx_height;
extern s32 xn_gfx_clip_left, xn_gfx_clip_top, xn_gfx_clip_right, xn_gfx_clip_bottom;
extern s32 xn_gfx_row_offset[768];      /* y * width */
extern u8 *screen_buffer;
extern u8 *big_buffer;

#endif
