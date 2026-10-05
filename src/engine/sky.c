/* sky.c: XnGine's sky and weather functions as readable C (xsky.h; see xngine.h and
   docs/xngine_readable.md). The stars are directions of length 2^20; the snow flakes keep
   their screen position with 6 fraction bits. */
#include "xsky.h"
#include "xmat.h"
#include "xmath.h"

#define SKY_STARS       768
#define SNOW_FLAKES     100
#define SNOW_COLOUR     0x70
#define SCREEN_WIDTH    320

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
        x = xn_muldiv(v.x, xn_cam_focal_x, v.z) + xn_cam_centre_x;
        y = xn_muldiv(v.y, xn_cam_focal_y, v.z) + xn_cam_centre_y;
        if (x < xn_gfx_clip_left || y < xn_gfx_clip_top || x >= xn_gfx_clip_right ||
            y >= xn_gfx_clip_bottom)
            continue;
        pixel = screen_buffer + xn_gfx_row_offset[y] + x;
        /* (the asm also asks for a pixel of at least 10h: DFh always is) */
        if (*pixel == 0xDF)
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
   its x across the view; a second its y in the view's first 8 rows. Returns the clip window's
   width, which the asm leaves in the register it picks the flake's size by. */
static u32 snow_respawn(struct xn_snow_flake *f)
{
    u32 r = xn_rand_next();
    u32 width;

    f->drift = (r & 0x20) ? -1 : 1;
    width = xn_gfx_clip_right - xn_gfx_clip_left;
    f->x = (u16)((xn_umod16((u16)r, (u16)width) + xn_gfx_clip_left) << 6);
    f->y = (u16)(((xn_rand_next() & 7) + xn_gfx_clip_top) << 6);
    return width;
}

void xn_sky_draw_snow(void)
{
    struct xn_snow_flake *f = xn_snow_flakes;
    u8 *screen = screen_buffer;
    u32 left;                           /* flakes left, 100 down to 1 (the asm's `loop`) */
    u16 size;                           /* the size class: the count, or a respawn's width */
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
        f->y = (u16)((xn_umod16((u16)xn_rand_next(),
                                (u16)(xn_gfx_clip_bottom - xn_gfx_clip_top - 20)) +
                      xn_gfx_clip_top) << 6);
        f->x = (u16)((xn_umod16((u16)xn_rand_next(),
                                (u16)(xn_gfx_clip_right - xn_gfx_clip_left)) +
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
        y = (s16)(xn_umod16((u16)xn_rand_next(),
                            (u16)(xn_gfx_clip_bottom - xn_gfx_clip_top + 40)) +
                  xn_gfx_clip_top - 20);
        x = (s16)(xn_umod16((u16)xn_rand_next(), (u16)(xn_gfx_clip_right - xn_gfx_clip_left)) +
                  xn_gfx_clip_left);
        xn_sky_draw_rain_streak(x, y);
    }
}

/* The streak's run (the unrolled body at 0xC9D89): n pixels down from dst, colour k on row
   k. The asm plants a `ret` at step n of its 31 steps: an n outside 0..31 lands outside the
   body (before it: code already run; after it: past the restore), so all 31 run. */
static void rain_run(u8 *dst, s32 n, const u8 *colours)
{
    s32 k;

    if (n < 0 || n > 31)
        n = 31;
    for (k = 0; k < n; k++)
        dst[k * SCREEN_WIDTH] = colours[k];
}

void xn_sky_draw_rain_streak(s32 x, s32 y)
{
    const u8 *colours = xn_rain_streak_colours;
    s32 n = 30;
    s32 y_end;

    if (y < xn_gfx_clip_top) {
        /* the rows above the view are cut off. A streak starting more than 30 rows above it
           has n < 0, and the asm then draws 31 pixels from the top with colours read past the
           gradient (kept: the game picks y at most 20 rows above) */
        colours += xn_gfx_clip_top - y;
        n += y - xn_gfx_clip_top;
        y = xn_gfx_clip_top;
    } else {
        y_end = y + 30;
        if (y_end >= xn_gfx_clip_bottom)
            return;
    }
    rain_run(screen_buffer + xn_gfx_row_offset[y] + x, n, colours);
}

void xn_sky_draw_rain_streak_r(xn_regs *r)
{
    xn_sky_draw_rain_streak(r->eax, r->edx);
}

void xn_sky_draw_rain_streak_bottom_clip(u8 *dst, s32 n, s32 y_end, const u8 *colours)
{
    n -= y_end - xn_gfx_clip_bottom + 1;
    rain_run(dst, n, colours);
    /* n = 31 plants the `ret` on the restoring instruction itself: the asm returns through it
       without restoring, and the `ret` stays */
    if (n == 31)
        xn_sky_rain_restore_op = 0xC3;
}

/* ---- the sky image ---------------------------------------------------------------------- */

/* width bytes from src to dst, as `rep movsd` then `rep movsb` copy them (forwards, a dword at
   a time) */
static void copy_row(u8 *dst, const u8 *src, u32 width)
{
    u32 i;

    for (i = 0; i < width >> 2; i++)
        ((u32 *)dst)[i] = ((const u32 *)src)[i];
    for (i = width & ~3u; i < width; i++)
        dst[i] = src[i];
}

void xn_sky_copy_rows(const u8 *src, u8 *dst, u32 width, u32 rows)
{
    do {                                /* (rows = 0 is 2^32 rows) */
        copy_row(dst, src, width);
        dst += SCREEN_WIDTH;
        src += 512;
    } while (--rows != 0);
}
