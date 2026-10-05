/* mouse.c: XnGine's mouse as readable C (xmouse.h; see xngine.h). */
#include "xmouse.h"
#include "xtimer.h"

/* int 33h function fn with CX and DX set: AX, CX and DX as 16-bit loads into a copy of the
   caller's registers r (the asm's pushal ... popal keeps them all) */
static void mouse_call16(const xn_regs *r, u16 fn, u16 cx, u16 dx)
{
    xn_regs d = *r;

    d.eax = (d.eax & 0xFFFF0000) | fn;
    d.ecx = (d.ecx & 0xFFFF0000) | cx;
    d.edx = (d.edx & 0xFFFF0000) | dx;
    xn_int33(&d);
}

void xn_mouse_set_range_320x200_r(xn_regs *r)
{
    xn_regs d = *r;

    d.eax = (d.eax & 0xFFFF0000) | 7;
    d.ecx &= 0xFFFF0000;
    d.edx = (d.edx & 0xFFFF0000) | 310;
    xn_int33(&d);
    mouse_call16(&d, 8, 0, 194);        /* (with the registers as the first call left them) */
}

void xn_mouse_set_range_640x480_r(xn_regs *r)
{
    xn_regs d = *r;

    d.eax = (d.eax & 0xFFFF0000) | 7;
    d.ecx &= 0xFFFF0000;
    d.edx = (d.edx & 0xFFFF0000) | 620;
    xn_int33(&d);
    mouse_call16(&d, 8, 0, 460);
}

void xn_mouse_set_sensitivity(s32 horizontal, s32 vertical)
{
    xn_regs d;

    d.eax = 0x1A;
    d.ebx = horizontal;
    d.ecx = vertical;
    d.edx = 50;                         /* the double-speed threshold */
    xn_int33(&d);
}

void xn_mouse_get_sensitivity(u8 *horizontal, u8 *vertical)
{
    xn_regs d;

    d.eax = 0x1B;
    xn_int33(&d);
    *vertical = (u8)d.ecx;
    *horizontal = (u8)d.ebx;
}

void xn_mouse_ps2_set_sample_rate(u8 rate)
{
    xn_regs d;

    d.eax = 0xC202;
    d.ebx = (u32)rate << 8;
    xn_int15(&d);
}

void xn_mouse_init(void)
{
    xn_regs d;

    d.eax = 7;                          /* x range */
    d.ecx = 10;
    d.edx = 630;
    xn_int33(&d);
    d.eax = 8;                          /* y range */
    d.ecx = 10;
    d.edx = 460;
    xn_int33(&d);
    d.eax = 4;                          /* position */
    d.ecx = 10;
    d.edx = 10;
    xn_int33(&d);
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

/* |a - b| < 4 in 16 bits (the asm's neg: -32768 stays negative, so it counts as near) */
static int near16(s16 a, s16 b)
{
    s16 d = (s16)(a - b);

    if (d < 0)
        d = (s16)-d;
    return d < 4;
}

/* A new press of the button `bit` (1 left, 2 right) since the last poll: a double click when
   the last press was within 8 BIOS ticks and 4 pixels; then this press is the last one */
static void mouse_press(u8 bit, u32 now, u32 *tick, s16 *x, s16 *y)
{
    s32 dt;

    if ((mouse_buttons & bit) == 0 || (xn_mouse_buttons_prev & bit) != 0)
        return;
    dt = now - *tick;
    if (dt < 0)
        dt = -dt;
    if (dt < 8 && near16(*x, mouse_x) && near16(*y, mouse_y))
        mouse_double_click |= bit;
    *tick = now;
    *x = mouse_x;
    *y = mouse_y;
}

void xn_mouse_poll(void)
{
    xn_regs d;
    u32 now;

    xn_fs_from_ds();
    xn_mouse_buttons_prev = mouse_buttons;
    d.eax = 3;
    xn_int33(&d);
    mouse_buttons = (u8)d.ebx;
    mouse_x = (s16)d.ecx;
    mouse_y = (s16)d.edx;
    mouse_double_click = 0;
    now = XN_BIOS_TICKS;
    mouse_press(1, now, &xn_mouse_left_press_tick, &xn_mouse_left_press_x,
                &xn_mouse_left_press_y);
    mouse_press(2, now, &xn_mouse_right_press_tick, &xn_mouse_right_press_x,
                &xn_mouse_right_press_y);
}

void xn_mouse_cursor_move(s16 x, s16 y)
{
    mouse_x = x;
    mouse_y = y;
    xn_mouse_cursor_erase();
    xn_mouse_cursor_draw();
}

void xn_mouse_cursor_erase(void)
{
    xn_regs r;

    if ((xn_mouse_cursor_drawn & 1) == 0)
        return;
    xn_mouse_cursor_drawn &= ~1;
    r.eax = xn_mouse_under_x;
    r.edx = xn_mouse_under_y;
    r.ebx = (u16)xn_mouse_cursor_w;
    r.ecx = (u16)xn_mouse_cursor_h;
    r.esi = (u32)xn_mouse_cursor_under;
    r.ebp = 0;                          /* no skip between rows */
    xn_asmcall(asm_xn_draw_put_rect_regs, &r);
}

void xn_mouse_cursor_clip(void)
{
    s32 skip = 0;                       /* the first visible pixel's offset in the image */
    s32 x, y, w, h;
    const u8 *src;
    u8 *dst;

    xn_mouse_cursor_w = 16;
    xn_mouse_cursor_h = 16;
    x = mouse_x - xn_mouse_hotspot_x;
    if (x < 0) {
        x = -x;
        skip = x;
        xn_mouse_cursor_w -= (s16)x;
        x = 0;
    }
    xn_mouse_cursor_x = x;
    if ((u32)x >= xn_gfx_width - 16)
        xn_mouse_cursor_w = (s16)(xn_gfx_width - x);
    y = mouse_y - xn_mouse_hotspot_x;   /* (sic: the x hotspot) */
    if (y < 0) {
        y = -y;
        xn_mouse_cursor_h -= (s16)y;
        skip += y << 4;
        y = 0;
    }
    xn_mouse_cursor_y = y;
    if ((u32)y >= xn_gfx_height - 16)
        xn_mouse_cursor_h = (s16)(xn_gfx_height - y);
    w = xn_mouse_cursor_w;
    h = xn_mouse_cursor_h;
    if (w <= 0 || h <= 0)
        return;
    dst = xn_mouse_cursor_clipped;
    for (src = xn_mouse_cursor_image + skip; h != 0; h--, src += 16) {
        for (x = 0; x < w; x++)
            *dst++ = src[x];
    }
}

void xn_mouse_cursor_draw(void)
{
    xn_regs r;

    if (xn_mouse_cursor_drawn & 1)
        return;
    xn_mouse_cursor_drawn |= 1;
    xn_mouse_cursor_clip();
    r.eax = xn_mouse_under_x = xn_mouse_cursor_x;
    r.edx = xn_mouse_under_y = xn_mouse_cursor_y;
    r.ebx = (u16)xn_mouse_cursor_w;
    r.ecx = (u16)xn_mouse_cursor_h;
    r.edi = (u32)xn_mouse_cursor_under;
    r.ebp = 0;
    xn_asmcall(asm_xn_draw_get_rect_regs, &r);     /* save what is under it */
    r.eax = xn_mouse_cursor_x;
    r.edx = xn_mouse_cursor_y;
    r.ebx = xn_mouse_cursor_w;          /* (sign-extended here) */
    r.ecx = xn_mouse_cursor_h;
    r.esi = (u32)xn_mouse_cursor_clipped;
    xn_asmcall(asm_xn_draw_image_transparent_regs, &r);
}

void xn_mouse_set_cursor_image(const u8 *image, s16 hot_x, s16 hot_y)
{
    s32 k;

    for (k = 0; k < 64; k++)
        ((u32 *)xn_mouse_cursor_image)[k] = ((const u32 *)image)[k];
    xn_mouse_hotspot_x = hot_x;
    xn_mouse_hotspot_y = hot_y;
}

void xn_mouse_read_motion(s16 *mx, s16 *my)
{
    xn_regs d;

    xn_fs_from_ds();
    d.eax = ((u32)mx & 0xFFFF0000) | 0x0B;
    xn_int33(&d);
    if (mx != 0)
        *mx = (s16)d.ecx;
    if (my != 0)
        *my = (s16)d.edx;
}

void xn_mouse_set_position(s32 x, s32 y)
{
    xn_regs d;

    d.eax = 4;
    d.ecx = x;
    d.edx = y;
    xn_fs_from_ds();
    xn_int33(&d);
}
