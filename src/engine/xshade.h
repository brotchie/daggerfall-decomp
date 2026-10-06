/* xshade.h: XnGine's shade table and distance fog (src/engine/shade.c). Canonical C: plain
   prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     The shade table (SHADE.nnn) is 64 rows of 256 colours: row k darkens or tints every
     colour by k/64 of the way. The game loads it, fixes its reserved colours and builds the
     flats' translucency table from it. The fog darkens every span beyond the fog start:
     after each span xn_render_frame calls the span hook (xn_render_span_hook, xrframe.h),
     which is xn_shade_fog_span while the fog is on and xn_shade_fog_span_off otherwise; the
     fog remaps the span's pixels beyond the start through the fog table's level at their
     depth.

   Units and formats
     shade table  xn_shade_table: 64 rows of 256, 16K-aligned; row 63 at +3F00h.
     fog table    64 levels of 256 colours (the game's haze tables); xn_fog_table_last is
                  its last level, and level L of the fog lies 3F00h - L bytes before it.
     depth        the fog start in z >> 8 units; a pixel's level from its 1/z (2^40 / z):
                  level = step * 2^32 / inv_z - start * step, where step = 3F00h / (far >> 8
                  - start) table bytes per unit (xn_fog_step); 1/z is clamped to
                  xn_fog_min_inv_z = 2^40 / far.
     fog row      a level's address with an 8-bit fraction in its low byte, which the pixel
                  replaces; it moves by a fixed step a pixel along a span.

   Globals (in object 2, the engine's): xn_shade_table (+ _last_row), xn_shade_filename,
   xn_haze_filename, xn_shade_translucent_table (game-visible), xn_fog_start, xn_fog_step,
   xn_fog_min_inv_z. The game's: xn_fog_table_last (the game writes it before each
   xn_shade_set_fog), haze_table_1, xn_cam_far_z (the view distance).

   Quirks kept (docs/engine/quirks.md): Q-SHADE-01 (with the fog off the fog start becomes the
   view distance without the >> 8), Q-SHADE-02 (a fog start of 0 or 1 gives an inverse start
   of 0: Q-SYS-01), Q-SHADE-03 (a span starting nearer than the fog is cut where its slope
   says, in an add whose overflow is not wrapped). */
#ifndef XSHADE_H
#define XSHADE_H

#include "xngine.h"
#include "xnstruct.h"

extern u8 *xn_shade_table;              /* 64 rows of 256 colours (SHADE.nnn), 16K-aligned */
extern u8 *xn_shade_table_last_row;     /* xn_shade_table + 3F00h */
extern char xn_shade_filename[];        /* "SHADE.nnn" */
extern char xn_haze_filename[];         /* "HAZE.nnn" */
extern u8 *haze_table_1;                /* the game's haze.001 table (64 rows of 256) */

extern u8 *xn_fog_table_last;           /* the fog table's last level (the game's) */
extern s32 xn_fog_start;                /* z >> 8 units (the view distance when off) */
extern u32 xn_fog_step;                 /* 3F00h / (far >> 8 - start): table bytes a unit */
extern u32 xn_fog_min_inv_z;            /* 2^40 / far: 1/z is clamped to it */
extern s32 xn_cam_far_z;                /* the view distance (z units) */

/* ---- the shade table -------------------------------------------------------------------- */

/* The last of the 64 shade rows (the game keeps it as a fog table that changes nothing). One
   game site. */
u8 *xn_shade_table_63(void);

/* At start-up, after the haze tables are loaded: in every shade row, colours 0-31 map to FFh
   except colour 0, which stays 0, and colour 255 maps to FFh; in every row of the haze.001
   table, colour 255 maps to 77h. One game site. */
void xn_shade_init_reserved(void);

/* After a shade table is loaded: colour 0 maps to 0 and colour 255 to FFh in every row. Two
   game sites. */
void xn_shade_keep_colours_0_255(void);

/* The flats' translucency table at `table`: 16 rows of 256; row 0 the identity, rows 1-13 the
   shade rows 50, 48, ... 26, rows 14 and 15 all F5h. One game site. */
void xn_shade_build_translucent_table(u8 *table);

/* Loads SHADE.nnn (n as three digits, xn_str_from_int) into the shade table (xn_dos_load_file:
   a missing file ends the game) and sets xn_shade_table_last_row. Two game sites. */
void xn_shade_load(s32 n);

/* Dead (no caller): loads HAZE.nnn into a new 256-aligned block from the game's allocator,
   and makes its last level the fog table's. */
void xn_shade_load_haze(s32 n);

/* ---- the fog ------------------------------------------------------------------------------ */

/* Sets the fog for the frames to come: start in z >> 8 units, or -1 for none. With a start
   nearer than the view distance (far >> 8), the fog is on: its start, step and 1/z clamp
   are kept, and the span hook becomes xn_shade_fog_span. Otherwise the hook is
   xn_shade_fog_span_off and the fog start becomes the view distance (Q-SHADE-01). Seven
   game sites (the game sets xn_fog_table_last first). Q-SHADE-02. */
void xn_shade_set_fog(s32 start);

/* The span hook with the fog off: nothing. */
void xn_shade_fog_span_off(const struct xn_poly *poly, const struct xn_span *span, s32 n,
                           u8 *pix);

/* The span hook with the fog on: the span's n pixels from pix (the polygon's, from its
   x_start) that lie beyond the fog start are remapped through the fog level at their depth,
   interpolated in 1/z from both ends of the fogged part (the polygon's +5Ch and +60h give
   1/z along the row). Q-SHADE-03. */
void xn_shade_fog_span(const struct xn_poly *poly, const struct xn_span *span, s32 n, u8 *pix);

/* The remap: pixel k of pix[0..n-1] becomes the colour at the fog row (row's high bytes, the
   pixel as the low byte); row += step after each pixel. n at most 641. */
void xn_shade_fog_pixels(u8 *pix, s32 n, uptr row, s32 step);

#endif
