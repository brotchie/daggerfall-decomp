/* xrender.h: XnGine's renderer: start-up and shutdown, the render mode, the start of a frame,
   the pick, the polygons' first-span setups, the model and flat passes, the background and
   the draw lists' sort (src/engine/render.c; the frame loop itself is xrframe.h, rframe.c).
   Canonical C: plain prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     The game starts the renderer once (xn_render_init: the divide handler, the view, the
     squares and size-mask tables, the 1/z table) and, every frame, empties its pools
     (xn_render_begin_frame), queues models and flats (the model and flat modules), then
     draws (xn_render_frame). The render mode picks how polygons are drawn: 0 outlines (each
     span's two ends), 4 solid colours, 8 textured (the game's). A polygon's span routine
     starts as the mode's setup for its texture entry's kind (xn_render_span_setup): on its
     first span the setup lights the polygon (xlight.h), stores the span routine (xspan.h)
     the lighting chooses and runs it.

   Units and formats
     render mode, kinds   byte offsets, as the asm indexes its tables: the mode 0, 4 or 8;
                          a texture entry's kind 0..16 (0 solid, 4 textured, 16 terrain);
                          a lighting kind 0 (none: fully lit or no light), 4 (one shade row)
                          or 8 (lit per pixel by a shader).
     1/z table            xn_render_recip_table[k] = 2^24 / k for k = 1..65536 (entry 0
                          never written); the span routines index it with inv_z >> 13.
     squares table        xn_squares_table[k + 4096] = k*k << 8, k = -4096..4095 (the light
                          shaders' squared distances).
     draw lists           struct xn_sort_pair {key, value}, sorted ascending by key.

   Globals (in object 2, the engine's): xn_render_recip_table, xn_render_mode,
   xn_squares_table, xn_tex_size_mask, the frame's pools and counts (xpipe.h), the sentinel
   span and the rows' list heads, xn_pick_view_x/y and pick_distance (the pick's results,
   game-visible). The game's: text_colour, text_shadow_colour.

   Quirks kept (docs/engine/quirks.md): Q-RENDER-01 (the outline's right end is drawn one
   pixel past the span), Q-RENDER-02 (the draw lists' quicksort resumes its right part where
   the left part's scan stopped), Q-RENDER-03 (the pick's span test counts x_end as covered
   and offsets only x's low word), Q-RENDER-05 (the frame draws the rows symmetric about the
   view centre); D-RENDER-01 (a pick outside the clip window: dropped). */
#ifndef XRENDER_H
#define XRENDER_H

#include "xpipe.h"
#include "xspan.h"

extern s32 *xn_render_recip_table;      /* 2^24 / k, k = 0..65536 (entry 0 unwritten) */
extern s32 xn_render_mode;              /* 0 outline, 4 solid, 8 textured */
extern s32 xn_squares_table[8192];      /* k * k << 8, k = -4096..4095 */
extern u8 xn_tex_size_mask[256];        /* n - 1 for a power of two n, else FFh */

/* ---- start-up, shutdown, the render mode ------------------------------------------------- */

/* Start-up: XnGine's divide-error handler; the view window as the camera globals hold it
   and the focal lengths 200, 180; the squares table; the texture size masks; render mode 8;
   the 1/z table (65537 dwords from the game's allocator, entries 65536 down to 1). One game
   site (the video set-up). */
void xn_render_init(void);

/* Shutdown: frees the lights, the texture cache, the world, the divide-error handler and the
   1/z table. One game site; the fatal paths of the engine's allocators. */
void xn_render_shutdown(void);

/* Sets the render mode (0 outline, 4 solid colours, 8 textured; anything else is outside the
   asm's table). Four game sites. */
void xn_render_set_mode(s32 mode);

/* The first-span setup of a polygon whose texture entry has kind `kind` (0, 4, 8, 12 or 16)
   in the current render mode: the model draw stores it in each face's polygon, the terrain
   (kind 16) in each cell's. */
xn_span_fn xn_render_span_setup(s32 kind);

/* The textured span routines by lighting kind / 4, plus 3 for the 16-pixel ones (when 1/z
   changes slowly across the screen): xn_poly_setup_textured picks one. */
extern xn_span_fn const xn_render_tmap_span_fns[6];

/* ---- the frame --------------------------------------------------------------------------- */

/* The start of a frame: empties the matrix, polygon and light-list pools, the model queue,
   the flat list and the light shaders; makes the background polygon and the sentinel span
   that ends every row's list (from the clip right edge, its own next); zeroes the frame's
   counts; and sets the frame's ambient shade row (xn_light_ambient_row: the ambient level's
   whole rows, 0..3F00h, into xn_shade_table). Three game sites, once a frame before the
   models and flats are queued. */
void xn_render_begin_frame(void);

/* The polygon or flat under the screen point (x, y), or 0 outside the clip window (where
   the asm unbalances its stack: D-RENDER-01): the flat pick (xn_flat_pick), then the span of
   row y that covers x and its depth from the polygon's 1/z gradient (pick_distance = 2^32 /
   (1/z)) and view-space point (xn_pick_view_x, _y); a flat wins when it is nearer. One game
   site (engine_pick_object). Q-RENDER-03. */
void *xn_render_pick(s32 x, s32 y);

/* ---- the first-span setups (xn_span_fn: the polygon's span, as xn_render_frame calls it) -- */

/* Solid colour (modes 4 and 8, kind 0): lights the polygon (xn_light_setup_poly), installs
   xn_span_solid, _solid_shaded_setup or _solid_lit by the lighting kind, makes the polygon's
   colour the texture entry's colour byte four times (+40h) and draws the span. */
void xn_render_span_setup_solid(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                s32 n, u8 *pix);

/* A terrain cell, textured (mode 8): its texels from the entry's current image, lit
   (xn_light_setup_terrain: directional lights only), xn_span_tex64 or _tex64_shaded
   installed, the 16-pixel steps (the per-pixel gradients << 4), and the span drawn. */
void xn_render_span_setup_terrain(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                  s32 n, u8 *pix);

/* A terrain cell in solid colours (mode 4): lit, a solid routine installed, the entry's
   colour, the span drawn. */
void xn_render_span_setup_terrain_solid(struct xn_poly *poly, const struct xn_span *span,
                                        s32 xs, s32 n, u8 *pix);

/* The outline mode's routine (and mode 8's for kinds 8 and 12): the span's first pixel and
   the pixel after its last in text_colour. Q-RENDER-01. */
void xn_render_span_mark_ends(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                              u8 *pix);

/* ---- the passes ----------------------------------------------------------------------------- */

/* The frame's models: the queue sorted by distance (nearest first), each drawn
   (xn_model_draw: its faces into the S-buffer). Returns 1 when a draw fails (the texture
   cache is full), else 0. */
int xn_render_draw_models(void);

/* The frame's flats: their frame constants (xn_flat_begin_frame), the lights into view
   space, the flat list sorted by depth (farthest first), each drawn (xn_flat_draw). Returns
   1 when a draw fails, else 0. */
int xn_render_draw_flats(void);

/* Unless the frame's flags have bit 1 (outdoors: the sky covers it), every pixel of the clip
   rows that no span covers, from x = 0 to the sentinel, in text_shadow_colour. */
void xn_render_fill_background(void);

/* Sorts the pairs at byte offsets lo..hi of `pairs` (the first and last pair's offsets) by
   key, ascending, when hi > lo: a quicksort. Q-RENDER-02. */
void xn_render_sort_pairs(struct xn_sort_pair *pairs, s32 lo, s32 hi);

/* The quicksort's step on pairs lo..hi (byte offsets, lo < hi): partitions about the middle
   pair's key, then sorts the left part and the right part. Returns the byte offset where its
   last scan stopped: its caller starts its own right part from there (Q-RENDER-02). */
s32 xn_render_sort_pairs_range(struct xn_sort_pair *pairs, s32 lo, s32 hi);

#endif
