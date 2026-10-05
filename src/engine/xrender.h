/* xrender.h: XnGine's renderer set-up and frame helpers (readable C, render.c; see xngine.h):
   start-up and shutdown, the render mode, the frame's reset, the pick, the first-span setups
   of solid and terrain polygons, the model and flat passes, the background fill and the
   draw-list sort. xn_render_frame and its run-time blocks (0x12A870-0x12A98A) belong to the
   rasteriser group and stay asm here.

   Each declaration keeps the function's asm interface (its row in config/xngine_abi.csv): no
   pragma is Watcom's own convention; a pragma names the registers; NAME_r is the glue for a
   function whose asm callers read several registers or flags. */
#ifndef XRENDER_H
#define XRENDER_H

#include "xpipe.h"

/* Start-up: the divide-error handler, the default view window (the current globals) and
   focal lengths (200, 180), the squares table ((k*k) << 8 for k = -4096..4095), the
   texture size masks (n - 1 for a power of two n, else 0xFF), render mode 8 (textured), and
   the 1/z table: 65537 dwords 2^24 / k (entry 0 unwritten), allocated and patched into the
   25 span routines that read it. (The row's inputs EBP and ESI only pass through to the asm
   routines it calls, which the C runs on register files of their own.) */
void xn_render_init(void);
#pragma aux xn_render_init modify exact [eax ecx edx ebx];

/* Shutdown: frees the lights, the texture cache, the world, the divide handler and the 1/z
   table. (The row's inputs only pass through to the callees' pushad-saved registers.) */
void xn_render_shutdown(void);
#pragma aux xn_render_shutdown modify exact [eax];

/* The render mode (a byte offset: 0 outline, 4 solid colours, 8 textured): copies that mode's
   five first-span setups into xn_render_span_setups. */
void xn_render_set_mode(s32 mode);

/* The start of a frame: empties the matrix, polygon and light-list pools, the model queue and
   the flat list; makes the background polygon (its span routine is the frame loop's row end)
   and the sentinel span that ends every row's list from the clip right edge; zeroes the
   frame's counts; points the light shaders at big_buffer; sets the ambient shade row (the
   ambient level's whole rows, 0..0x3F00, into xn_shade_table) for the polygons and the flats
   and the textured setup's subdivision shift (18 at 320 pixels wide, else 25). */
void xn_render_begin_frame(void);

/* The polygon or flat under the screen point (x, y) (inside the clip window), or 0: asks
   xn_flat_pick, then finds the span of row y that covers x and its depth from the polygon's
   1/z gradient (pick_distance = 2^32 / (1/z)); the view-space point goes to xn_pick_view_x/y.
   A flat wins when it is nearer. (ECX EBP ESI, the row's other inputs, are only saved and
   restored.) frame: passed on to xn_flat_pick (the asm's EBX, a
   leftover its texture lookup keeps). */
void *xn_render_pick(s32 x, s32 y, s32 frame);
#pragma aux xn_render_pick parm [eax] [edx] [ebx] value [eax] modify exact [eax ecx edx ebx esi edi];

/* First-span setups, called through a polygon's span routine pointer with the span's
   registers (eax = the polygon, ebx = x - centre x, ebp = the pixel count, esi = the span
   node, edi = the destination - 1). Each lights the polygon, installs the span routine the
   lighting kind (0/4/8) chooses and runs it on the same span (the asm's `jmp ecx`). */

/* Solid colour: the texture entry's colour byte four times in the polygon's +40h. */
void xn_render_span_setup_solid_r(xn_regs *r);
/* Terrain, textured: the texels from the entry's current image; the 16-pixel steps (the
   per-pixel gradients << 4). */
void xn_render_span_setup_terrain_r(xn_regs *r);
/* Terrain, solid colour. */
void xn_render_span_setup_terrain_solid_r(xn_regs *r);

/* The models of the frame: the queue sorted by distance, nearest first, each drawn. Returns
   1 (the asm's CF) when a model draw fails (the texture cache is full). */
int xn_render_draw_models(void);
void xn_render_draw_models_r(xn_regs *r);

/* The flats of the frame: their frame constants, the lights into view space, the flat list
   sorted by depth, each drawn. Returns 1 (CF) when a flat draw fails. */
int xn_render_draw_flats(void);
void xn_render_draw_flats_r(xn_regs *r);

/* The outline mode's span routine: the span's first and last pixels in text_colour. Returns
   the colour (the asm's AL). */
u8 xn_render_span_mark_ends(u8 *dst, s32 n);
void xn_render_span_mark_ends_r(xn_regs *r);

/* Unless the frame's flags have bit 1 (outdoors: the sky covers it), fills every pixel of the
   clip rows that no span covers, from x = 0, with text_shadow_colour. */
void xn_render_fill_background(void);
#pragma aux xn_render_fill_background modify exact [eax ecx ebx esi edi];

/* Sorts pairs[lo..hi] (byte offsets of the first and last pair) by key, ascending: a
   quicksort (xn_render_sort_pairs_range) when hi > lo. The first keeps every register (the
   row's EBP ESI inputs only pass through it). */
void xn_render_sort_pairs_saveregs(struct xn_sort_pair *pairs, s32 lo, s32 hi);
#pragma aux xn_render_sort_pairs_saveregs parm [eax] [edx] [ebx] modify exact [eax];
void xn_render_sort_pairs(struct xn_sort_pair *pairs, s32 lo, s32 hi);
void xn_render_sort_pairs_r(xn_regs *r);

/* The quicksort's step on pairs[lo..hi]: partitions about the middle pair's key, then sorts
   the left part and the right part. Returns where its forward scan stopped (the asm's ESI),
   and the pivot to *pivot (EBP): its caller starts its own right part from that returned
   scan position, not its own (an original quirk, kept: the sort stays a sort, the
   recursion differs). */
s32 xn_render_sort_pairs_range(struct xn_sort_pair *pairs, s32 lo, s32 hi, s32 *pivot);
void xn_render_sort_pairs_range_r(xn_regs *r);

#endif
