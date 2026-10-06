/* xrframe.h: XnGine's frame loop, xn_render_frame (src/engine/rframe.c; the rest of the
   renderer is xrender.h). Canonical C: plain prototypes, Watcom's own calling convention;
   docs/xngine_canonical.md.

   What it does
     Draws the frame the game queued since xn_render_begin_frame: the models (their faces go
     into the S-buffer, xspan.h), the background, then every screen row of the clip window
     from the top, each span of the row's list by its polygon's routine (+3Ch) and then the
     span hook (the fog, xshade.h), and last the flats. The asm called each span routine
     with the row's state patched into the lit routines' code and stopped each row by
     calling the sentinel span's polygon's routine, which unwound the call's stack frame
     (12A949 and the run-time blocks after it); canonical C is two loops, ending a row at the
     sentinel span (the span that is its own next).

   Globals: xn_render_row_y (the row being drawn minus the view centre's; the span routines
   read it), xn_render_frame_flags (bit 1: outdoors, no background fill), the screen (xpipe.h),
   xn_tex_cache_full. Canonical C's own: xn_render_span_hook.

   Quirks kept: none of its own. */
#ifndef XRFRAME_H
#define XRFRAME_H

#include "xngine.h"
#include "xnstruct.h"

/* The span hook: called after each span with the span's polygon, node, pixel count and
   first pixel. */
typedef void (*xn_span_hook_fn)(const struct xn_poly *poly, const struct xn_span *span, s32 n,
                                u8 *pix);

/* The frame's span hook: xn_shade_fog_span or xn_shade_fog_span_off (xn_shade_set_fog). */
extern xn_span_hook_fn xn_render_span_hook;

extern s32 xn_render_frame_flags;       /* bit 1: no background fill (outdoors) */

/* Draws the frame with the given flags (bit 1: outdoors). Returns 1 when the texture cache
   is full or the models or the flats could not be drawn (their textures did not fit), else
   0. Four game sites, once a frame. */
s32 xn_render_frame(s32 flags);

#endif
