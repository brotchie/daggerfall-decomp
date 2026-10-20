/* wide.c: the wide 3D pass (docs/port.md, "Widescreen"), on the game's thread, compiled with
   the engine (its dialect and its headers).

   The game draws into a 320x200 screen. For a wider view the port widens the engine around
   the world's drawing instead (port_wide_enter / port_wide_exit, called from game_frame under
   DAGGER_PORT): the sky and the 3D pass draw into a W x 200 frame whose middle 320 columns are
   the game's screen, with the view window as wide as the frame and the focal unchanged, so the
   vertical field of view stays the game's and the horizontal one widens (Hor+). The middle
   columns come out exactly as the 320-column pass draws them: they go back into the game's
   screen, and the HUD, the weapon and the pick run on the frame they always saw.

   The engine's per-frame pools (polygons, spans, light lists) have no bounds and were sized
   for 320 columns: a wide pass takes larger ones of its own. The S-buffer stays the last
   frame's until the next, so the pick (xn_render_pick) runs in the wide frame too
   (port_wide_pick_*). The game's weather is drawn after the pass, over its own columns; the
   sides get theirs below (port_wide_weather).

   The main thread asks for the width (pgl_wide_request, vpc_gl.c); 0 or 320 is the original
   path, untouched. */
#include <string.h>

#include "xngine.h"
#include "xnstruct.h"
#include "xpipe.h"
#include "xgfx.h"
#include "xcam.h"
#include "xsky.h"
#include "port_gl.h"

int port_wide_cols;                     /* the frame's width while a wide pass runs, else 0 */
int port_wide_dx;                       /* the game's column 0 in it */
int port_wide_last_cols;                /* the last frame's (its S-buffer is the pick's) */
int port_wide_last_dx;
unsigned long port_wide_frames;         /* wide passes drawn */

struct xn_poly port_poly_pool[PORT_POLY_POOL];
struct xn_span port_span_pool[PORT_SPAN_POOL];
struct xn_light_ref port_light_pool[PORT_LIGHT_POOL];
u8 port_wide_buffer[PGL_MAX_W * PGL_H];

/* the game's screen and view while the wide pass runs */
static struct {
    u8 *buffer;
    s32 width, size;
    s32 clip_l, clip_t, clip_r, clip_b;
    s32 hw, hh, cx, cy;
} saved;

/* the game's own view: the full width of its screen, the view centred (the large view or the
   one above the HUD bar). The bank's and others' smaller views stay as they are. */
static int game_view_is_wide_able(void)
{
    return xn_gfx_width == PGL_W && xn_gfx_height == PGL_H && screen_buffer != 0 &&
           xn_cam_half_width == PGL_W / 2 && xn_cam_centre_x == PGL_W / 2 &&
           xn_cam_half_height == xn_cam_centre_y && xn_gfx_clip_left == 0 &&
           xn_gfx_clip_right == PGL_W;
}

static void use_frame(u8 *buffer, s32 w)
{
    screen_buffer = buffer;
    xn_gfx_width = w;
    xn_gfx_screen_size = w * PGL_H;
    xn_gfx_build_row_offsets();
}

/* The game's clock for the eye's motion (vpc_gl.c's smooth frame rate): the frame times
   summed. A frame's eye moved by the previous frame's time (player_movement_update runs after
   the frame is drawn, with the frame_ticks of that frame) */
double port_game_ms;

/* For PORT_GL_WIDE_CHECK: each pixel of the game's screen as the S-buffer has it, 1/z of the
   polygon that covers it (0: none, the sky or the background), from the spans as the pick
   reads them. The geometry decides it, not the texels, so the narrow and the wide pass agree
   on it but at polygons' edges. dx: the game's column 0 in the frame */
static u32 check_depth[PGL_W * PGL_H];
static u8 check_sky[PGL_W * PGL_H];     /* the game's screen after the sky, before the 3D pass */
static unsigned long frames_at_enter;  /* port_render_frames when the frame began */

/* after sky_update (game_frame): the sky's picture, for the check */
void port_wide_sky_drawn(void)
{
    s32 y, dx = port_wide_cols ? port_wide_dx : 0;

    if (!pgl_check_pending() && !pgl_check_child())
        return;
    for (y = 0; y < PGL_H; y++)
        memcpy(check_sky + y * PGL_W, screen_buffer + y * xn_gfx_width + dx, PGL_W);
}

static const u32 *screen_depth(s32 dx)
{
    int y;

    memset(check_depth, 0, sizeof check_depth);
    if (port_render_frames == frames_at_enter)
        return check_depth;             /* no 3D pass this frame (a menu): the S-buffer is an
                                           older frame's */
    for (y = 0; y < PGL_H; y++) {
        const struct xn_span *node;

        for (node = xn_render_span_rows[y].next; node != node->next; node = node->next) {
            s32 x, x0 = node->x_start, x1 = node->x_end;

            if (node->poly == 0 || node->poly->span_fn == 0)
                continue;
            for (x = x0 < dx ? dx : x0; x < x1 && x < dx + PGL_W; x++)
                check_depth[y * PGL_W + x - dx] =
                    (u32)node->inv_z + (u32)((x - x0) * node->poly->inv_z_dx);
        }
    }
    return check_depth;
}

/* the wide pass begins: the frame takes the game's screen in its middle columns */
void port_wide_enter(void)
{
    static int prev_ticks;
    s32 w = pgl_wide_request, y;

    port_game_ms += prev_ticks;
    prev_ticks = frame_ticks;
    port_wide_cols = 0;
    frames_at_enter = port_render_frames;
    if (!pgl_enabled || w <= PGL_W || !game_view_is_wide_able())
        return;
    if (w > PGL_MAX_W)
        w = PGL_MAX_W;
    w &= ~1;
    /* PORT_GL_WIDE_CHECK: a copy of the process draws this frame the original way, to compare
       its screen with the wide pass's middle (pgl_share.c); in the copy, nothing more here */
    if (pgl_check_fork_begin())
        return;
    saved.buffer = screen_buffer;
    saved.width = xn_gfx_width;
    saved.size = xn_gfx_screen_size;
    saved.clip_l = xn_gfx_clip_left;
    saved.clip_t = xn_gfx_clip_top;
    saved.clip_r = xn_gfx_clip_right;
    saved.clip_b = xn_gfx_clip_bottom;
    saved.hw = xn_cam_half_width;
    saved.hh = xn_cam_half_height;
    saved.cx = xn_cam_centre_x;
    saved.cy = xn_cam_centre_y;
    port_wide_dx = (w - PGL_W) / 2;
    for (y = 0; y < PGL_H; y++)
        memcpy(port_wide_buffer + y * w + port_wide_dx, saved.buffer + y * PGL_W, PGL_W);
    use_frame(port_wide_buffer, w);
    xn_gfx_set_clip(0, saved.clip_t, w, saved.clip_b);
    xn_cam_set_view_window(w / 2, saved.hh, w / 2, saved.cy);
    port_wide_cols = w;
}

/* the wide pass ends: its middle columns are the game's screen again */
void port_wide_exit(void)
{
    s32 w = port_wide_cols, y;

    if (w == 0) {
        if (pgl_check_child())          /* the check's copy ends here */
            pgl_check_child_done(screen_buffer, screen_depth(0), check_sky);
        return;
    }
    if (pgl_check_pending())
        screen_depth(port_wide_dx);
    for (y = 0; y < PGL_H; y++)
        memcpy(saved.buffer + y * PGL_W, port_wide_buffer + y * w + port_wide_dx, PGL_W);
    use_frame(saved.buffer, saved.width);
    xn_gfx_screen_size = saved.size;
    xn_gfx_set_clip(saved.clip_l, saved.clip_t, saved.clip_r, saved.clip_b);
    xn_cam_set_view_window(saved.hw, saved.hh, saved.cx, saved.cy);
    port_wide_last_cols = w;
    port_wide_last_dx = port_wide_dx;
    port_wide_cols = 0;
    port_wide_frames++;
    pgl_check_fork_end(saved.buffer, check_depth, check_sky);
}

/* ---- the pick: a point of the game's screen in the last wide frame ------------------------- */

static int picking;

/* x: the game's column, made the frame's; 1 if the pick is in the wide frame (then
   port_wide_pick_end after it) */
int port_wide_pick_begin(s32 *x)
{
    if (port_wide_last_cols == 0 || port_wide_cols != 0 || !game_view_is_wide_able())
        return 0;
    saved.clip_l = xn_gfx_clip_left;
    saved.clip_t = xn_gfx_clip_top;
    saved.clip_r = xn_gfx_clip_right;
    saved.clip_b = xn_gfx_clip_bottom;
    saved.hw = xn_cam_half_width;
    saved.hh = xn_cam_half_height;
    saved.cx = xn_cam_centre_x;
    saved.cy = xn_cam_centre_y;
    xn_gfx_set_clip(0, saved.clip_t, port_wide_last_cols, saved.clip_b);
    xn_cam_set_view_window(port_wide_last_cols / 2, saved.hh, port_wide_last_cols / 2, saved.cy);
    *x += port_wide_last_dx;
    picking = 1;
    return 1;
}

void port_wide_pick_end(void)
{
    if (!picking)
        return;
    picking = 0;
    xn_gfx_set_clip(saved.clip_l, saved.clip_t, saved.clip_r, saved.clip_b);
    xn_cam_set_view_window(saved.hw, saved.hh, saved.cx, saved.cy);
}

/* ---- the weather beside the game's screen ----------------------------------------------------

   The game draws its rain and snow over its 320 columns after the 3D pass (the engine's
   xn_sky_draw_rain / _snow, with the clip window's bottom at the view's). The wide frame's
   columns beside them get the same weather here, as dense: their own streaks and flakes, from
   a generator of their own, so the game's draws and the engine's random numbers are as they
   were. weather_draw_precipitation (src/hand/func_0003533F.c) calls port_wide_weather_turn
   before the game's snow (which clears the view's turn) and port_wide_weather after it. */

#define SIDE_FLAKES 256

static u32 side_seed = 0x2545F491;
static struct {
    s32 x, y;                           /* side columns, pixels << 6 */
    int drift;
} side_flakes[SIDE_FLAKES];
static int side_flake_count, side_flakes_w;
static s32 side_turn;

static u32 side_rand(void)
{
    side_seed = side_seed * 1103515245u + 12345u;
    return side_seed >> 16;
}

void port_wide_weather_turn(void)
{
    side_turn = xn_snow_turn_shift;
}

/* the frame's column of side column sx (the columns left of the game's screen, then right) */
static int side_x(int sx)
{
    return sx < port_wide_last_dx ? sx : sx + PGL_W;
}

static void side_rain(int side, s32 top, s32 bottom)
{
    s32 w = port_wide_last_cols;
    int i, n = 50 * side / PGL_W, k;

    for (i = 0; i < n; i++) {
        s32 y = (s32)(side_rand() % (u32)(bottom - top + 40)) + top - 20;
        s32 x = side_x((int)(side_rand() % (u32)side));
        const u8 *colours = xn_rain_streak_colours;
        int rows = 30;

        if (y < top) {                  /* cut at the top, as xn_sky_draw_rain_streak */
            colours += top - y;
            rows += y - top;
            y = top;
        } else if (y + 30 >= bottom) {
            continue;
        }
        for (k = 0; k < rows; k++)
            port_wide_buffer[(y + k) * w + x] = colours[k];
    }
}

static void side_snow(int side, s32 top, s32 bottom)
{
    s32 w = port_wide_last_cols;
    int i, n = 100 * side / PGL_W;

    if (n > SIDE_FLAKES)
        n = SIDE_FLAKES;
    if (n != side_flake_count || side != side_flakes_w) {
        for (i = 0; i < n; i++) {
            side_flakes[i].x = (s32)(side_rand() % (u32)side) << 6;
            side_flakes[i].y = ((s32)(side_rand() % (u32)(bottom - top - 20)) + top) << 6;
            side_flakes[i].drift = side_rand() & 0x20 ? -1 : 1;
        }
        side_flake_count = n;
        side_flakes_w = side;
    }
    for (i = 0; i < n; i++) {
        s32 x, y, speed, drift;
        int size = 100 - i * 100 / n;   /* the size classes as the game's 100 flakes have them */
        u8 *p;

        side_flakes[i].x += side_turn << 6;
        x = side_flakes[i].x >> 6;
        y = side_flakes[i].y >> 6;
        if (x < 1 || x >= side - 1) {
            side_flakes[i].x = (x < 1 ? side - 8 : 8) << 6;
            x = side_flakes[i].x >> 6;
        }
        if (y >= bottom - 4) {
            side_flakes[i].x = (s32)(side_rand() % (u32)side) << 6;
            side_flakes[i].y = ((s32)(side_rand() & 7) + top) << 6;
            side_flakes[i].drift = side_rand() & 0x20 ? -1 : 1;
            x = side_flakes[i].x >> 6;
            y = side_flakes[i].y >> 6;
        }
        p = port_wide_buffer + y * w + side_x(x);
        if (size >= 50) {
            p[0] = p[w] = 0x70;
            if (side_x(x + 1) == side_x(x) + 1)
                p[1] = p[w + 1] = 0x70;
            speed = size >= 80 ? (s32)xn_snow_speed_large : (s32)xn_snow_speed_medium;
        } else {
            p[0] = 0x70;
            speed = (s32)xn_snow_speed_small;
        }
        drift = xn_snow_drift_speed;
        if (side_flakes[i].drift < 0)
            drift = -drift;
        if ((side_rand() & 0xFFFF) < 0x400) {
            side_flakes[i].drift = -side_flakes[i].drift;
            drift = -drift;
        }
        side_flakes[i].y += speed;
        side_flakes[i].x += drift;
    }
}

/* kind: the weather (4 rain, 5 snow); the clip window as the game set it for its draw */
void port_wide_weather(int kind)
{
    int side = port_wide_last_cols - PGL_W;
    s32 top = xn_gfx_clip_top, bottom = xn_gfx_clip_bottom;

    if (port_wide_last_cols == 0 || side <= 0 || port_wide_cols != 0 || bottom - top < 48)
        return;
    if (kind == 4)
        side_rain(side, top, bottom);
    else if (kind == 5)
        side_snow(side, top, bottom);
}
