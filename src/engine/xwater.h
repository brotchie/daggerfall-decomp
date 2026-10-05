/* xwater.h: XnGine's dungeon water (src/engine/water.c; see xngine.h). After a dungeon frame
   is drawn, the pixels behind the water plane (y = dungeon_water_level) are drawn again from a
   position jittered by -1, 0 or +1 along the row (a ripple that moves every frame), mapped
   through the water tint table (water.tbl).

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XWATER_H
#define XWATER_H

#include "xngine.h"
#include "xnstruct.h"
#include "xscreen.h"

/* An end of the water line: two points far ahead and behind on the plane, in view space */
typedef struct xn_water_end {
    s32 x, y, z;            /* view space (24.8); z becomes 2^40 / z in xn_water_draw */
    u8 outcode;             /* outside: 1 left, 2 right, 4 bottom, 8 top, 10h near, 20h far */
    u8 pad[3];
} xn_water_end;
extern xn_water_end xn_water_line_p0, xn_water_line_p1;

/* Per yaw quadrant ((yaw + 256) >> 9): the plane points 2^20 ahead and behind on the axis */
typedef struct xn_water_dirs {
    s32 x0, z0, x1, z1;
} xn_water_dirs;
extern xn_water_dirs xn_water_quadrant_dirs[4];

extern s32 dungeon_water_level;         /* 10000: no water */
extern s32 xn_water_jitter[512];        /* -1, 0 or +1 per pixel */
extern s32 xn_water_row_phase[200];     /* per screen row: the ripple's phase, -4..4 */
extern s32 xn_water_row_velocity[200];  /* +-1, turned at +-4 */
extern s32 xn_water_row_base[200];      /* 8..127: the row's offset into the jitter table */
extern s32 xn_water_row;                /* the row being drawn */
/* the row's jitter table position: jitter for pixel x is xn_water_row_jitter[0x100 + x] (the
   asm addresses it from 0x12DA18, 400h bytes before the table, and reads it at +400h) */
extern s32 *xn_water_row_jitter;
extern u16 xn_water_unroll_offsets[641];    /* k * 21: pixel k's step in the unrolled body */
extern u8 *xn_water_tint_table;         /* 256-aligned: the tint of each colour */
/* the span routine xn_water_draw calls (xn_water_span's asm entry), with registers */
extern void (*xn_water_span_fn)(void);
/* the per-row step of the water plane's 1/z: self-patched (KEEP) */
extern s32 xn_water_row_inv_step;

/* Fills the ripple tables from the game's random numbers: 512 jitters of -1/0/+1 (nearly all
   +-1: (r & 63) - 32 clamped to -1..1), and per row a phase -4..3, a base 8..127 and a velocity
   of +-1. init_game_data. */
void xn_water_init(void);

/* The water line on screen: the two plane points along the view's axis quadrant, to view
   space, with their outcodes; each end clipped against the near (forced to 401h meanwhile),
   top and bottom planes. 1 when the line can be seen (neither end above or below the view). */
int xn_water_clip_extent(void);
void xn_water_clip_extent_r(xn_regs *r);       /* asm: CF when it cannot be seen */

/* Draws the water: the rows between the projected ends of the water line (inside the clip
   window, more than 2), each with the plane's 1/z stepped down them; per row the ripple
   advances, and every run of the row behind the plane (by the S-buffer's spans: a span in
   front of the water hides it up to where its depth crosses the plane's) is drawn again
   through xn_water_span_fn. world_render, when the eye is not at the water level. */
void xn_water_draw(void);

/* Draws pixels x0..x1 - 1 of a row (row is the screen row less 100h) again, each from its
   jittered neighbour through the tint table. (ABI override: EAX and EBP, the tint table and
   the planted offset, reach no caller.) */
void xn_water_span(s32 x0, s32 x1, u8 *row);
void xn_water_span_r(xn_regs *r);              /* asm: ebx x0, ebp x1, edi row - 100h */

/* The unrolled body of xn_water_span (641 steps of 21 bytes), entered with a `ret` planted
   after the n-th step: the glue recovers n from it. */
void xn_water_span_unrolled_r(xn_regs *r);

/* Dead: a lone ret after the unrolled body. */
void xn_water_stub_ret(void);
#pragma aux xn_water_stub_ret parm [] modify exact [eax];

#endif
