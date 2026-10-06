/* mouse.c: XnGine's mouse (canonical C; the interface and the module's documentation are in
   xmouse.h). */
#include "xmouse.h"
#include "xpc.h"
#include "xdraw.h"

#define CURSOR_SIZE         16
#define DOUBLE_CLICK_TICKS  8
#define DOUBLE_CLICK_PIXELS 4

/* ---- the driver ---------------------------------------------------------------------------- */

void xn_mouse_set_range_320x200(void)
{
    xn_mousedrv_x_range(0, 310);
    xn_mousedrv_y_range(0, 194);
}

void xn_mouse_set_range_640x480(void)
{
    xn_mousedrv_x_range(0, 620);
    xn_mousedrv_y_range(0, 460);
}

void xn_mouse_init(void)
{
    xn_mousedrv_x_range(10, 630);
    xn_mousedrv_y_range(10, 460);
    xn_mousedrv_set_position(10, 10);
}

void xn_mouse_set_sensitivity(s32 horizontal, s32 vertical)
{
    xn_mousedrv_set_speed((u16)horizontal, (u16)vertical, 50);
}

void xn_mouse_get_sensitivity(u8 *horizontal, u8 *vertical)
{
    u16 h, v, threshold;

    xn_mousedrv_speed(&h, &v, &threshold);
    *vertical = (u8)v;
    *horizontal = (u8)h;
}

void xn_mouse_ps2_set_sample_rate(u8 rate)
{
    xn_bios_ps2_sample_rate(rate);
}

void xn_mouse_read_motion(s16 *mx, s16 *my)
{
    s16 dx, dy;

    xn_mousedrv_motion(&dx, &dy);
    if (mx != 0)
        *mx = dx;
    if (my != 0)
        *my = dy;
}

void xn_mouse_set_position(s32 x, s32 y)
{
    xn_mousedrv_set_position((s16)x, (s16)y);
}

/* ---- polling ------------------------------------------------------------------------------- */

/* |a - b| < 4 in 16 bits (Quirk Q-MOUSE-02: negating -32768 leaves it negative: near) */
static int near16(s16 a, s16 b)
{
    s16 d = (s16)(a - b);

    if (d < 0)
        d = (s16)-d;
    return d < DOUBLE_CLICK_PIXELS;
}

/* A new press of the button `bit` since the last poll: a double click when the last press
   was within 8 BIOS ticks and 4 pixels; then this press is the last one */
static void mouse_press(u8 bit, u32 now, u32 *tick, s16 *x, s16 *y)
{
    s32 dt;

    if ((mouse_buttons & bit) == 0 || (xn_mouse_buttons_prev & bit) != 0)
        return;
    dt = now - *tick;
    if (dt < 0)
        dt = -dt;
    if (dt < DOUBLE_CLICK_TICKS && near16(*x, mouse_x) && near16(*y, mouse_y))
        mouse_double_click |= bit;
    *tick = now;
    *x = mouse_x;
    *y = mouse_y;
}

void xn_mouse_poll(void)
{
    u16 buttons;
    u32 now;

    xn_mouse_buttons_prev = mouse_buttons;
    xn_mousedrv_state(&buttons, &mouse_x, &mouse_y);
    mouse_buttons = (u8)buttons;
    mouse_double_click = 0;
    now = XN_BIOS_TICKS;
    mouse_press(XN_MOUSE_LEFT, now, &xn_mouse_left_press_tick, &xn_mouse_left_press_x,
                &xn_mouse_left_press_y);
    mouse_press(XN_MOUSE_RIGHT, now, &xn_mouse_right_press_tick, &xn_mouse_right_press_x,
                &xn_mouse_right_press_y);
}

static s16 clamp16(s16 v, s16 lo, s16 hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}

void xn_mouse_poll_clamped(void)
{
    xn_mouse_poll();
    mouse_x = clamp16(mouse_x, mouse_x_min, mouse_x_max);
    mouse_y = clamp16(mouse_y, mouse_y_min, mouse_y_max);
}

/* ---- the cursor --------------------------------------------------------------------------- */

void xn_mouse_cursor_move(s16 x, s16 y)
{
    mouse_x = x;
    mouse_y = y;
    xn_mouse_cursor_erase();
    xn_mouse_cursor_draw();
}

void xn_mouse_cursor_erase(void)
{
    if ((xn_mouse_cursor_drawn & 1) == 0)
        return;
    xn_mouse_cursor_drawn &= ~1;
    xn_draw_put_rect(xn_mouse_under_x, xn_mouse_under_y, (u16)xn_mouse_cursor_w,
                     (u16)xn_mouse_cursor_h, xn_mouse_cursor_under, 0);
}

void xn_mouse_cursor_clip(void)
{
    s32 skip = 0;                       /* the first visible pixel's offset in the image */
    s32 x, y, w, h;
    const u8 *src;
    u8 *dst;

    xn_mouse_cursor_w = CURSOR_SIZE;
    xn_mouse_cursor_h = CURSOR_SIZE;
    x = mouse_x - xn_mouse_hotspot_x;
    if (x < 0) {
        x = -x;
        skip = x;
        xn_mouse_cursor_w -= (s16)x;
        x = 0;
    }
    xn_mouse_cursor_x = x;
    if ((u32)x >= (u32)(xn_gfx_width - CURSOR_SIZE))
        xn_mouse_cursor_w = (s16)(xn_gfx_width - x);
    y = mouse_y - xn_mouse_hotspot_x;   /* Quirk Q-MOUSE-01: the x hotspot */
    if (y < 0) {
        y = -y;
        xn_mouse_cursor_h -= (s16)y;
        skip += y * CURSOR_SIZE;
        y = 0;
    }
    xn_mouse_cursor_y = y;
    if ((u32)y >= (u32)(xn_gfx_height - CURSOR_SIZE))
        xn_mouse_cursor_h = (s16)(xn_gfx_height - y);
    w = xn_mouse_cursor_w;
    h = xn_mouse_cursor_h;
    if (w <= 0 || h <= 0)
        return;
    dst = xn_mouse_cursor_clipped;
    for (src = xn_mouse_cursor_image + skip; h != 0; h--, src += CURSOR_SIZE) {
        for (x = 0; x < w; x++)
            *dst++ = src[x];
    }
}

void xn_mouse_cursor_draw(void)
{
    if (xn_mouse_cursor_drawn & 1)
        return;
    xn_mouse_cursor_drawn |= 1;
    xn_mouse_cursor_clip();
    xn_mouse_under_x = xn_mouse_cursor_x;
    xn_mouse_under_y = xn_mouse_cursor_y;
    xn_draw_get_rect(xn_mouse_cursor_x, xn_mouse_cursor_y, (u16)xn_mouse_cursor_w,
                     (u16)xn_mouse_cursor_h, xn_mouse_cursor_under, 0);
    xn_draw_image_transparent(xn_mouse_cursor_x, xn_mouse_cursor_y, xn_mouse_cursor_w,
                              xn_mouse_cursor_h, xn_mouse_cursor_clipped);
}

void xn_mouse_set_cursor_image(const u8 *image, s16 hot_x, s16 hot_y)
{
    s32 k;

    for (k = 0; k < 64; k++)
        ((u32 *)xn_mouse_cursor_image)[k] = ((const u32 *)image)[k];
    xn_mouse_hotspot_x = hot_x;
    xn_mouse_hotspot_y = hot_y;
}
