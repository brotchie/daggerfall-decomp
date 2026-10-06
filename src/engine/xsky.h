/* xsky.h: XnGine's sky and weather (src/engine/sky.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     The night sky's stars (placed once; drawing them is dead code), the snow (100 flakes that
     fall and drift across the view), the rain (50 streaks drawn at random places each rainy
     frame) and the sky image's row copy (the game's sky backdrop, 512 wide, into a 320-wide
     buffer).

   Fixed point and units
     stars        directions in world axes of length 2^20 (xn_math_angles_to_vector)
     snow         a flake's x and y are screen pixels << 6 (u16, wrapping); speeds are pixels
                  << 6 a frame, from the frame's length in milliseconds
     rain         screen pixels; a streak is 30 rows of the gradient xn_rain_streak_colours
     screen       screen_buffer rows of 320 bytes (xn_gfx_row_offset), clipped to the gfx
                  group's clip window (left, top inclusive; right, bottom exclusive)

   Tables and state (object 2)
     xn_sky_stars[768], xn_sky_star_colours[768]        the stars (engine-private)
     xn_sky_star_palette[16]                            the stars' colours (read only)
     xn_snow_flakes[100]                                the flakes (engine-private)
     xn_snow_speed_small / _medium / _large, xn_snow_drift_speed: this frame's speeds
     xn_snow_turn_shift                                 the game's: the view's turn, pixels
     xn_rain_streak_colours[30]                         the streak's gradient (read only)
   Random numbers come from xn_rand_next (xrand.h).

   Quirks kept: Q-SKY-01 (a streak far above the view), Q-SKY-02 (a respawned flake's size);
   Q-SKY-03 is dropped (the dead bottom-clip entry's stuck ret). docs/engine/quirks.md. */
#ifndef XSKY_H
#define XSKY_H

#include "xwshare.h"

extern xn_vec3 xn_sky_stars[768];       /* struct xn_sky_star: directions of length 2^20 */
extern u8 xn_sky_star_colours[768];
extern u8 xn_sky_star_palette[16];
extern struct xn_snow_flake xn_snow_flakes[100];
extern s32 xn_snow_turn_shift;          /* the view's sideways turn this frame, pixels (the
                                           game sets it; the snow clears it) */
extern u32 xn_snow_speed_small;         /* fall speeds, pixels << 6 a frame */
extern u32 xn_snow_speed_medium;
extern u32 xn_snow_speed_large;
extern u16 xn_snow_drift_speed;         /* sideways drift, pixels << 6 a frame */
extern u8 xn_rain_streak_colours[30];   /* a streak's colours, top to bottom */

/* ---- stars -------------------------------------------------------------------------------- */

/* Places the 768 stars on the upper half of the sky: each a random colour of the 16-colour
   palette and random angles (polar below a quarter turn, azimuth in the upper half; a pair
   out of range draws all three numbers again), made a vector of length 2^20. One game site
   (the sky's setup). */
void xn_sky_init_stars(void);

/* Dead: projects the 768 stars through the eye's rotation and writes each star's colour on
   the screen pixels inside the clip window that still show the sky's colour DFh. */
void xn_sky_draw_stars(void);

/* Dead: rotates the 768 star vectors in place by the eye's rotation. */
void xn_sky_rotate_stars(void);

/* ---- snow -------------------------------------------------------------------------------- */

/* Moves and draws the 100 snow flakes in colour 70h: each is shifted by the view's turn,
   drawn (flakes 0-50 2 x 2: 0-20 large, 21-50 medium; 51-99 one pixel, small), falls by its
   class's speed and drifts sideways, turning one time in four. A flake that leaves the
   view's sides wraps to 8 pixels inside the other side; one that reaches the last 4 rows
   starts again near the top, and is then drawn by the size the clip window's width gives it
   (Q-SKY-02). Clears xn_snow_turn_shift. One game site, each snowy frame. */
void xn_sky_draw_snow(void);

/* Scatters the 100 snow flakes over the view (above its last 20 rows), each drifting left or
   right at random. One game site. */
void xn_sky_snow_init(void);

/* The snow's speeds for this frame, in pixels << 6: the large flakes fall frame_ticks * 3200
   / 1000 (50 pixels a second), the small ones half that, the medium ones three quarters; the
   sideways drift is frame_ticks * 1280 / 1000 (20 pixels a second). The products are 32-bit
   (a frame of more than 22 minutes wraps). One game site. */
void xn_sky_snow_update_speeds(void);

/* ---- rain -------------------------------------------------------------------------------- */

/* Draws 50 rain streaks at random places: x in the view, the top y from 20 rows above it to
   20 below its bottom. One game site, each rainy frame. */
void xn_sky_draw_rain(void);

/* One rain streak from (x, y) down: 30 pixels in the streak's colours, one per row, the rows
   above the view cut off. A streak that would reach the view's bottom row or below is not
   drawn. Q-SKY-01: a streak starting more than 30 rows above the view draws 31 pixels from
   the view's top with colours from before the gradient. */
void xn_sky_draw_rain_streak(s32 x, s32 y);

/* Dead (nothing reaches this entry of the streak routine): the streak's run shortened at the
   view's bottom. dst: the run's first pixel; n: its pixel count before the cut; y_end: the
   row below its last; colours: its first colour. Q-SKY-01 for n outside 0..31; Q-SKY-03
   (dropped): the asm's 31-pixel run leaves its planted `ret` in the code. */
void xn_sky_draw_rain_streak_bottom_clip(u8 *dst, s32 n, s32 y_end, const u8 *colours);

/* ---- the sky image ------------------------------------------------------------------------- */

/* Copies rows rows of width bytes from a 512-wide image (src) to a 320-wide buffer (dst): the
   sky backdrop into big_buffer. rows at least 1 (0 copies 2^32 rows). Nine game sites. */
void xn_sky_copy_rows(const u8 *src, u8 *dst, u32 width, u32 rows);

#endif
