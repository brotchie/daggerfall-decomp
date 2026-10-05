/* joy.c: XnGine's joystick as readable C (xjoy.h; see xngine.h). */
#include "xjoy.h"
#include "xmem.h"
#include "xtimer.h"

/* Busy-waits for the next BIOS tick */
static void next_tick(void)
{
    u32 t = XN_BIOS_TICKS;

    while (XN_BIOS_TICKS == t)
        ;
}

void xn_joy_init(void)
{
    xn_regs v;
    u8 seen;
    s32 k;

    if (xn_joy.installed == 1)
        return;
    xn_joy.installed = 1;
    xn_joy.axis_mask = 0;
    xn_outb(XN_JOY_PORT, 0xFF);             /* fire the one-shots */
    next_tick();
    xn_cli();
    seen = 0xFF;                            /* the bits that read 0 every time */
    for (k = 0; k < 64; k++)
        seen &= ~xn_inb(XN_JOY_PORT);
    xn_joy.axis_mask = seen;                /* (see xjoy.h) */
    xn_joy.status = XN_JOY_OFF;
    xn_sti();
    v.eax = 0x1C;
    xn_asmcall(func_000A1272, &v);          /* _dos_getvect(1Ch): DX:EAX */
    xn_joy.old_int1c_sel = (u16)v.edx;
    xn_joy.old_int1c_off = v.eax;
    func_000A12A6(0x1C, asm_xn_joy_timer_isr, xn_cs());
    xn_mem_lock_region(asm_xn_joy_timer_isr, xn_code_152F41 - (u8 *)asm_xn_joy_timer_isr);
    xn_mem_lock_region(&xn_joy, sizeof(struct xn_joy_state));
}

void xn_joy_shutdown(void)
{
    if (xn_joy.installed == 0)
        return;
    xn_joy.installed = 0;
    xn_joy.status = XN_JOY_OFF;
    xn_cli();
    func_000A12A6(0x1C, (void *)xn_joy.old_int1c_off, xn_joy.old_int1c_sel);
    xn_sti();
}

void xn_joy_reset_range(void)
{
    xn_joy.min_x = 32000;
    xn_joy.min_y = 32000;
    xn_joy.max_x = 0;
    xn_joy.max_y = 0;
    xn_joy.min_x_b = 32000;
    xn_joy.min_y_b = 32000;
    xn_joy.max_x_b = 0;
    xn_joy.max_y_b = 0;
    *(u16 *)&xn_joy.button1 = 0;            /* buttons 1 and 2 (a word) */
    *(u16 *)&xn_joy.button3 = 0;            /* 3 and 4 */
}

void xn_joy_calibrate(void)
{
    s32 k;

    if (xn_joy.status == XN_JOY_OFF || xn_joy.installed == 0)
        return;
    xn_joy_reset_range();
    xn_joy.center.center_x = 0;
    xn_joy.center.center_y = 0;
    xn_joy.center_b.center_x = 0;
    xn_joy.center_b.center_y = 0;
    for (k = 0; k < 4; k++) {
        next_tick();
        xn_joy.center.center_x += xn_joy.raw_x;
        xn_joy.center.center_y += xn_joy.raw_y;
        xn_joy.center_b.center_x += xn_joy.raw_x_b;
        xn_joy.center_b.center_y += xn_joy.raw_y_b;
    }
    xn_joy.center.center_x = (xn_joy.center.center_x + 2) >> 2;
    xn_joy.center.center_y = (xn_joy.center.center_y + 2) >> 2;
    xn_joy.center_b.center_x = (xn_joy.center_b.center_x + 2) >> 2;
    xn_joy.center_b.center_y = (xn_joy.center_b.center_y + 2) >> 2;
}

/* *lo = min(*lo, v), *hi = max(*hi, v) */
static void widen(s32 v, s32 *lo, s32 *hi)
{
    if (v < *lo)
        *lo = v;
    if (v > *hi)
        *hi = v;
}

/* One axis: the count's distance from the centre, over the range on its side (min - centre
   below, max - centre above), times -4096 (the asm negates the distance above the centre,
   then the quotient), within -4095..4095; 0 inside the dead zone */
static s32 joy_axis(s32 raw, s32 center, s32 lo, s32 hi)
{
    s32 d = raw - center;
    s32 range = lo;
    s32 v;
    xn_s64 n;

    if (d >= 0) {
        d = -d;
        range = hi;
    }
    range -= center;
    if ((d < 0 ? -d : d) <= xn_joy.dead_zone)
        return 0;
    xn_s64_set(&n, d << 12);
    v = -xn_s64_div(&n, range);             /* a range of 0: the divide handler's 0 */
    if (v < -4095)
        v = -4095;
    else if (v > 4095)
        v = 4095;
    return v;
}

void xn_joy_poll(void)
{
    s32 x, y, xb, yb;

    xn_joy.x = 0;
    xn_joy.y = 0;
    xn_joy.x_b = 0;
    xn_joy.y_b = 0;
    if (xn_joy.status == XN_JOY_OFF || xn_joy.installed == 0)
        return;
    x = xn_joy.raw_x;
    y = xn_joy.raw_y;
    xb = xn_joy.raw_x_b;
    yb = xn_joy.raw_y_b;
    widen(x, &xn_joy.min_x, &xn_joy.max_x);
    widen(y, &xn_joy.min_y, &xn_joy.max_y);
    widen(xb, &xn_joy.min_x_b, &xn_joy.max_x_b);
    widen(yb, &xn_joy.min_y_b, &xn_joy.max_y_b);
    xn_joy.x += joy_axis(x, xn_joy.center.center_x, xn_joy.min_x, xn_joy.max_x);
    xn_joy.y += joy_axis(y, xn_joy.center.center_y, xn_joy.min_y, xn_joy.max_y);
}

void xn_joy_timer_isr(void)
{
    s32 x = 0, y = 0, xb = 0, yb = 0;
    s32 k;
    u8 bits;

    *(u16 *)&xn_joy.button1 = 0;
    *(u16 *)&xn_joy.button3 = 0;
    if (xn_joy.status == XN_JOY_OFF)
        return;
    xn_outb(XN_JOY_PORT, 0x0F);             /* fire the one-shots */
    for (k = 0x800; k != 0; k--) {          /* count while each axis bit stays up */
        bits = xn_inb(XN_JOY_PORT);
        if (bits == 0)
            break;
        x += bits & 1;
        y += bits >> 1 & 1;
        xb += bits >> 2 & 1;
        yb += bits >> 3 & 1;
    }
    xn_joy.raw_x = x;
    xn_joy.raw_y = y;
    xn_joy.raw_x_b = xb;
    xn_joy.raw_y_b = yb;
    xn_outb(XN_JOY_PORT, 0xF0);
    bits = xn_inb(XN_JOY_PORT);             /* the buttons: a bit at 0 is pressed */
    xn_joy.button4 = (bits & 0x80) == 0;
    xn_joy.button3 = (bits & 0x40) == 0;
    xn_joy.button2 = (bits & 0x20) == 0;
    xn_joy.button1 = (bits & 0x10) == 0;
}
