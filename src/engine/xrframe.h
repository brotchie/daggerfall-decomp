/* xrframe.h: XnGine's frame loop, xn_render_frame (rframe.c; see xngine.h and
   docs/engine/smc/render.md, RENDER-FRAME). The rest of the render subsystem is the
   pipeline group's (render.c). */
#ifndef XRFRAME_H
#define XRFRAME_H

#include "xngine.h"
#include "xnstruct.h"

extern s32 xn_render_frame_flags;
extern s32 xn_render_frame_centre_x;    /* 12A931: sub ebx, CENTRE_X (xn_cam_set_view_window) */
extern s32 xn_render_frame_width;       /* 12A950: KEEP, add edi, WIDTH (the screen's width) */
extern s32 xn_render_frame_end_row;     /* 12A963: KEEP, the row_y after the last row */

/* Draws the frame: the queued models (their polygons go into the span lists), the
   background, then every screen row's span list from the clip top, each span by its
   polygon's span routine (+3Ch) and then the span hook (the fog), and then the flats.
   Returns 1 (the asm's CF too) when the texture cache is full or the models or flats could
   not be drawn, else 0. Every row's list ends at the sentinel node, whose polygon's routine
   is xn_render_frame_row_end (12A949): the asm calls it, and it unwinds the call's frame to
   go on to the next row; the C stops at it instead. Its other run-time blocks (12A94E,
   12A969, 12A975, 12A976, 12A97C, 12A97E) are the loop's next-row step and the ending, here
   inside the function. Keeps every register (pushad). */
s32 xn_render_frame(s32 flags);
void xn_render_frame_r(xn_regs *r);

#endif
