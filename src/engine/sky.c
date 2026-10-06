/* sky.c: XnGine's sky and weather (canonical C; the interface and the module's documentation
   are in xsky.h). */
#include "xsky.h"
#include "xmat.h"
#include "xmath.h"
#include "xrand.h"

#define SKY_STARS       768
#define SNOW_FLAKES     100
#define SNOW_COLOUR     0x70
#define SKY_COLOUR      0xDF            /* the sky's pixels, which the stars may cover */
#define SCREEN_WIDTH    320
#define STREAK_ROWS     30
#define STREAK_MAX      31              /* the asm's unrolled run: 31 steps */

/* lo % d of 16-bit values, as `xor dx, dx; div cx` leaves the remainder: a zero divisor (an
   empty clip window) faults, and the engine's handler makes the remainder 0 (Q-SYS-01) */
static u32 mod16(u32 lo, u32 d)
{
    lo &= 0xFFFF;
    d &= 0xFFFF;
    return d != 0 ? lo % d : 0;
}

/* a * b / d with the 64-bit product, truncated; 0 where the asm's idiv faults (Q-SYS-01) */
static s32 muldiv(s32 a, s32 b, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    return xn_s64_div_or0(&t, d);
}

/* ---- stars ------------------------------------------------------------------------------ */

void xn_sky_init_stars(void)
{
    u32 i;
    s32 polar, azimuth;

    for (i = 0; i < SKY_STARS; i++) {
        /* (the asm halves each masked number while it is above the mask: never) */
        do {
            xn_sky_star_colours[i] = xn_sky_star_palette[xn_rand_next() & 0xF];
            polar = xn_rand_next() & XN_ANGLE_MASK;
            azimuth = xn_rand_next() & XN_ANGLE_MASK;
        } while (azimuth < 0x400 || polar >= 0x400);
        xn_math_angles_to_vector(polar, azimuth, 0x100000, &xn_sky_stars[i]);
    }
}

void xn_sky_draw_stars(void)
{
    u32 i;
    xn_vec3 v;
    s32 x, y;
    u8 *pixel;

    for (i = 0; i < SKY_STARS; i++) {
        v = xn_sky_stars[i];
        xn_mat_transform(&v, &xn_cam_rotation);
        if (v.z <= xn_cam_near_z)
            continue;
        x = muldiv(v.x, xn_cam_focal_x, v.z) + xn_cam_centre_x;
        y = muldiv(v.y, xn_cam_focal_y, v.z) + xn_cam_centre_y;
        if (x < xn_gfx_clip_left || y < xn_gfx_clip_top || x >= xn_gfx_clip_right ||
            y >= xn_gfx_clip_bottom)
            continue;
        pixel = screen_buffer + xn_gfx_row_offset[y] + x;
        /* (the asm also asks for a pixel of at least 10h: DFh always is) */
        if (*pixel == SKY_COLOUR)
            *pixel = xn_sky_star_colours[i];
    }
}

void xn_sky_rotate_stars(void)
{
    u32 i;

    for (i = 0; i < SKY_STARS; i++)
        xn_mat_transform(&xn_sky_stars[i], &xn_cam_rotation);
}

/* ---- snow ------------------------------------------------------------------------------- */

/* A flake that reached the bottom: one random number picks its drift direction (bit 5) and
   its x across the view; a second its y in the view's first 8 rows. Returns the clip
   window's width: Quirk Q-SKY-02, the flake is then drawn by that "size" (the asm reuses the
   register it counts the flakes in). */
static u32 snow_respawn(struct xn_snow_flake *f)
{
    u32 r = xn_rand_next();
    u32 width = xn_gfx_clip_right - xn_gfx_clip_left;

    f->drift = (r & 0x20) ? -1 : 1;
    f->x = (u16)((mod16(r, width) + xn_gfx_clip_left) << 6);
    f->y = (u16)(((xn_rand_next() & 7) + xn_gfx_clip_top) << 6);
    return width;
}

void xn_sky_draw_snow(void)
{
    struct xn_snow_flake *f = xn_snow_flakes;
    u8 *screen = screen_buffer;
    u32 left;                           /* flakes left, 100 down to 1 */
    u16 size;                           /* the size class: flakes left, or Q-SKY-02 */
    s32 x, speed, drift;
    u32 y;
    u8 *p;

    for (left = SNOW_FLAKES; left != 0; left--, f++) {
        size = (u16)left;
        for (;;) {
            f->x += (u16)(xn_snow_turn_shift << 6);
            x = f->x >> 6;
            y = f->y >> 6;
            if (x <= xn_gfx_clip_left || x >= xn_gfx_clip_right) {
                /* off a side: to the other side, 8 pixels in (the turn is added again) */
                if (x > xn_gfx_clip_left)
                    f->x = (u16)((xn_gfx_clip_left + 8) << 6);
                else
                    f->x = (u16)((xn_gfx_clip_right - 8) << 6);
                continue;
            }
            if (y >= (u32)(xn_gfx_clip_bottom - 4)) {
                size = (u16)snow_respawn(f);
                continue;
            }
            break;
        }
        p = screen + xn_gfx_row_offset[y] + x;
        if (size >= 50) {
            p[0] = p[1] = SNOW_COLOUR;
            p[SCREEN_WIDTH] = p[SCREEN_WIDTH + 1] = SNOW_COLOUR;
            speed = size >= 80 ? xn_snow_speed_large : xn_snow_speed_medium;
        } else {
            p[0] = SNOW_COLOUR;
            speed = xn_snow_speed_small;
        }
        drift = xn_snow_drift_speed;
        if (f->drift < 0)
            drift = -drift;
        if ((u16)xn_rand_next() < 0x400) {      /* one in four: turn */
            f->drift = -f->drift;
            drift = -drift;
        }
        f->y = (u16)(f->y + speed);
        f->x = (u16)(f->x + drift);
    }
    xn_snow_turn_shift = 0;
}

void xn_sky_snow_init(void)
{
    struct xn_snow_flake *f = xn_snow_flakes;
    u32 i;

    for (i = 0; i < SNOW_FLAKES; i++, f++) {
        f->y = (u16)((mod16(xn_rand_next(), xn_gfx_clip_bottom - xn_gfx_clip_top - 20) +
                      xn_gfx_clip_top) << 6);
        f->x = (u16)((mod16(xn_rand_next(), xn_gfx_clip_right - xn_gfx_clip_left) +
                      xn_gfx_clip_left) << 6);
        f->drift = (xn_rand_next() & 0x20) ? -1 : 1;
    }
}

void xn_sky_snow_update_speeds(void)
{
    u32 large = (frame_ticks * 3200) / 1000;

    xn_snow_speed_large = large;
    xn_snow_speed_small = large >> 1;
    xn_snow_speed_medium = large - (large >> 2);
    xn_snow_drift_speed = (u16)((frame_ticks * 1280) / 1000);
}

/* ---- rain ------------------------------------------------------------------------------- */

void xn_sky_draw_rain(void)
{
    u32 i;
    s16 x, y;

    for (i = 0; i < 50; i++) {
        /* 16-bit arithmetic, as the asm's: the remainders, then the window's edges */
        y = (s16)(mod16(xn_rand_next(), xn_gfx_clip_bottom - xn_gfx_clip_top + 40) +
                  xn_gfx_clip_top - 20);
        x = (s16)(mod16(xn_rand_next(), xn_gfx_clip_right - xn_gfx_clip_left) +
                  xn_gfx_clip_left);
        xn_sky_draw_rain_streak(x, y);
    }
}

/* A streak's run: n pixels down from dst, colour k on row k. Quirk Q-SKY-01: the asm stops
   its 31-step unrolled run with a `ret` planted at step n; an n outside 0..31 plants it
   outside the run (and restores it after), so all 31 pixels are drawn. */
static void rain_run(u8 *dst, s32 n, const u8 *colours)
{
    s32 k;

    if (n < 0 || n > STREAK_MAX)
        n = STREAK_MAX;
    for (k = 0; k < n; k++)
        dst[k * SCREEN_WIDTH] = colours[k];
}

void xn_sky_draw_rain_streak(s32 x, s32 y)
{
    const u8 *colours = xn_rain_streak_colours;
    s32 n = STREAK_ROWS;

    if (y < xn_gfx_clip_top) {
        /* the rows above the view are cut off: more than 30 of them leave n < 0 (Q-SKY-01;
           the game picks y at most 20 rows above) */
        colours += xn_gfx_clip_top - y;
        n += y - xn_gfx_clip_top;
        y = xn_gfx_clip_top;
    } else if (y + STREAK_ROWS >= xn_gfx_clip_bottom)
        return;
    rain_run(screen_buffer + xn_gfx_row_offset[y] + x, n, colours);
}

void xn_sky_draw_rain_streak_bottom_clip(u8 *dst, s32 n, s32 y_end, const u8 *colours)
{
    /* Quirk Q-SKY-03 (dropped): for a run of 31 here the asm's planted `ret` lands on its own
       restoring instruction and stays in the code */
    rain_run(dst, n - (y_end - xn_gfx_clip_bottom + 1), colours);
}

/* ---- the sky image ---------------------------------------------------------------------- */

void xn_sky_copy_rows(const u8 *src, u8 *dst, u32 width, u32 rows)
{
    u32 i;

    do {
        for (i = 0; i < width; i++)
            dst[i] = src[i];
        dst += SCREEN_WIDTH;
        src += 512;
    } while (--rows != 0);
}
