/* xsky.h: XnGine's sky and weather functions (sky.c; see xngine.h): the stars, the snow and
   the rain, and the sky image's row copy.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers. */
#ifndef XSKY_H
#define XSKY_H

#include "xwshare.h"

extern xn_vec3 xn_sky_stars[768];       /* struct xn_sky_star: directions of length 2^20 */
extern u8 xn_sky_star_colours[768];
extern u8 xn_sky_star_palette[16];
extern struct xn_snow_flake xn_snow_flakes[100];
extern s32 xn_snow_turn_shift;          /* the view's sideways turn this frame, pixels */
extern u32 xn_snow_speed_small;         /* fall speeds, pixels << 6 per frame */
extern u32 xn_snow_speed_medium;
extern u32 xn_snow_speed_large;
extern u16 xn_snow_drift_speed;         /* sideways drift, pixels << 6 per frame */
extern u8 xn_rain_streak_colours[30];   /* a streak's colours, top to bottom */

/* The rain streak's unrolled body (0xC9D89, 31 steps of 9 bytes) ends in the instruction that
   restores the byte its planted `ret` replaced (0xC9EA0). Only a made-up 31-pixel run of the
   dead bottom-clip entry plants its `ret` on that instruction, and leaves it there. */
extern u8 xn_sky_rain_restore_op;

/* Places the 768 stars on the upper half of the sky: each a random colour of the 16-colour
   palette and random angles (polar below a quarter turn, azimuth in the upper half; a pair
   out of range draws all three again), made a vector of length 2^20. */
void xn_sky_init_stars(void);

/* Dead: projects the 768 stars through the eye's rotation and writes each star's colour on the
   screen pixels still showing the sky's colour DFh. (Keeps every register: the route's stub
   keeps EAX.) */
void xn_sky_draw_stars(void);
#pragma aux xn_sky_draw_stars parm [] modify exact [eax];

/* Dead: rotates the 768 star vectors in place by the eye's rotation. */
void xn_sky_rotate_stars(void);
#pragma aux xn_sky_rotate_stars parm [] modify exact [eax];

/* Moves and draws the 100 snow flakes: each is shifted by the view's turn, drawn in colour
   70h (flakes 0-20 large, 21-50 medium: 2x2; 51-99 small: one pixel), falls by its class's
   speed and drifts sideways, turning at random. A flake that leaves the view's sides wraps to
   the other side; one that reaches the bottom starts again near the top, drawn large. */
void xn_sky_draw_snow(void);

/* Scatters the 100 snow flakes over the view (above its last 20 rows), each drifting left or
   right at random. */
void xn_sky_snow_init(void);

/* The snow's speeds for this frame, in pixels << 6: the large flakes fall frame_ticks * 3200 /
   1000 (50 pixels a second), the small ones half that, the medium ones three quarters; the
   sideways drift is frame_ticks * 1280 / 1000 (20 pixels a second). */
void xn_sky_snow_update_speeds(void);

/* Draws 50 rain streaks at random places: x in the view, the top y from 20 rows above it to 20
   below its bottom. (The row's inputs bh, ebx.u and ebp are the streak's pass-through
   registers: the asm saves every register.) */
void xn_sky_draw_rain(void);

/* One rain streak from (x, y) down: 30 pixels in the streak's colours, one per row, clipped
   at the view's top. A streak that would reach the view's bottom is not drawn. (The row's
   outputs bh, ebx.u and ebp are registers the asm keeps: its analysis loses the stack at the
   planted `ret`'s return. The glue keeps them.) */
void xn_sky_draw_rain_streak(s32 x, s32 y);
void xn_sky_draw_rain_streak_r(xn_regs *r);

/* Dead (nothing reaches it): the rain streak's run shortened at the view's bottom. The
   streak's pixel address dst, its pixel count n, the y below its last row y_end and its
   colours arrive as C9D19 left them. */
void xn_sky_draw_rain_streak_bottom_clip(u8 *dst, s32 n, s32 y_end, const u8 *colours);
#pragma aux xn_sky_draw_rain_streak_bottom_clip parm [eax] [ecx] [edx] [esi] \
    modify exact [eax ecx edx esi edi];

/* Copies rows rows of width bytes from a 512-wide image to a 320-wide one (the sky image into
   big_buffer). */
void xn_sky_copy_rows(const u8 *src, u8 *dst, u32 width, u32 rows);

#endif
