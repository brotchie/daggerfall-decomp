/* xwshare.h: what the world group's readable C (collide, world, terrain, sky) uses of other
   groups: their globals and the asm functions it calls, declared with their ABI rows
   (config/xngine_abi.csv). For the coordinator to move into those groups' headers (or
   xngine.h) when they are merged. */
#ifndef XWSHARE_H
#define XWSHARE_H

#include "xngine.h"
#include "xnstruct.h"

/* ---- the 2D screen (gfx group) ---------------------------------------------------------- */
extern s32 xn_gfx_clip_left;            /* the clip window: left, top inclusive */
extern s32 xn_gfx_clip_top;
extern s32 xn_gfx_clip_right;           /* right, bottom exclusive */
extern s32 xn_gfx_clip_bottom;
extern s32 xn_gfx_row_offset[768];      /* y * screen width */
extern u8 *screen_buffer;               /* the 8-bit frame buffer (320 wide) */
extern u8 *big_buffer;                  /* the shared work buffer */

/* ---- the camera (cam and render groups) ------------------------------------------------- */
extern xn_mat3 xn_cam_rotation;         /* the eye's rotation, 2.28 */
extern s32 xn_cam_near_z;               /* the near plane, camera units (24.8) */
extern s32 xn_cam_far_z;
extern s32 xn_cam_centre_x;             /* the screen point of the view axis */
extern s32 xn_cam_centre_y;
extern s32 xn_cam_focal_x;              /* the projection scales, pixels */
extern s32 xn_cam_focal_y;

extern struct xn_scratch xn_scratch_vecs;       /* the shared scratch vectors (0x120288) */

/* ---- the game ---------------------------------------------------------------------------- */
extern u32 frame_ticks;                 /* milliseconds of the last frame */

/* ---- rand (system group; asm) ----------------------------------------------------------- */
/* The next of the engine's pseudo-random numbers: seed = seed * 797 mod 4099 (keeps every
   register but EAX) */
u32 xn_rand_next(void);
#pragma aux xn_rand_next value [eax] modify exact [eax];

/* a / d, truncated (`cdq; idiv`: a divide error when d = 0 or the quotient overflows) */
s32 xn_idiv(s32 a, s32 d);
#pragma aux xn_idiv = "cdq" "idiv ebx" parm [eax] [ebx] value [eax] modify [edx];

/* count dwords of v from dst (`rep stosd`; Watcom's own loop would call the library's
   __STOSD, which is not linked) */
void xn_fill32(void *dst, u32 v, u32 count);
#pragma aux xn_fill32 = "rep stosd" parm [edi] [eax] [ecx] modify [edi ecx];

/* ---- DOS files (dos group; asm) --------------------------------------------------------- */
/* int 21h 42h: moves the file to hi:lo (16-bit halves) from mode (AL; AH is set); returns the
   new position's low half (DX:AX, CF on error) */
u32 xn_dos_seek(u32 mode, u32 handle, u32 hi, u32 lo);
#pragma aux xn_dos_seek parm [eax] [ebx] [ecx] [edx] value [eax] modify exact [eax ecx edx];
/* int 21h 3Fh: reads n bytes into buf; returns the count read */
/* read, write, close and create are the system group's C (xdos.h) */
#include "xdos.h"

/* ---- flats (flat group; asm) ------------------------------------------------------------- */
/* Adds a flat (billboard) at a camera-space position: image = archive << 7 | record */
void xn_flat_add_view(s32 x, s32 y, s32 z, u32 image);
#pragma aux xn_flat_add_view parm [eax] [edx] [ebx] [ecx] modify exact [eax ecx edx ebx esi];

/* dx:ax / d with dx = 0, 16-bit (`xor dx, dx; div cx`): the remainder of lo / d. The asm's own
   instruction, so a divide error (d = 0) goes through the engine's handler as in the asm:
   the remainder is then 0. */
u16 xn_umod16(u16 lo, u16 d);
#pragma aux xn_umod16 = "xor dx, dx" "div cx" parm [eax] [ecx] value [dx] modify [eax edx];

#endif
