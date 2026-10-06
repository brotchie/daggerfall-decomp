/* joy.c: XnGine's joystick (canonical C; the interface and the module's documentation are in
   xjoy.h). */
#include "xjoy.h"
#include "xsysutil.h"
#include "xpc.h"
#include "xmem.h"

#define RANGE_NONE      32000           /* a minimum before anything is seen */
#define AXIS_MAX        4095
#define COUNT_MAX       0x800           /* port reads per tick, at most */

/* Busy-waits for the next BIOS tick */
static void next_tick(void)
{
    u32 t = XN_BIOS_TICKS;

    while (XN_BIOS_TICKS == t)
        ;
}

void xn_joy_init(void)
{
    u8 zero;
    s32 k;

    if (xn_joy.installed == 1)
        return;
    xn_joy.installed = 1;
    xn_joy.axis_mask = 0;
    xn_outb(XN_JOY_PORT, 0xFF);             /* fire the one-shots */
    next_tick();
    xn_cli();
    zero = 0xFF;                            /* the bits that read 0 every time */
    for (k = 0; k < 64; k++)
        zero &= ~xn_inb(XN_JOY_PORT);
    /* Quirk Q-JOY-01: the asm's tests meant to pair the axes OR in bits the mask has */
    xn_joy.axis_mask = zero;
    xn_joy.status = XN_JOY_OFF;
    xn_sti();
    xn_pc_get_vector(0x1C, &xn_joy.old_int1c_off, &xn_joy.old_int1c_sel);
    xn_pc_install_vector(0x1C, xn_joy_timer_entry);
    xn_mem_lock_region((void *)xn_joy_timer_entry, xn_joy_timer_end - (u8 *)xn_joy_timer_entry);
    xn_mem_lock_region(&xn_joy, sizeof xn_joy);
}

void xn_joy_shutdown(void)
{
    if (xn_joy.installed == 0)
        return;
    xn_joy.installed = 0;
    xn_joy.status = XN_JOY_OFF;
    xn_cli();
    xn_pc_set_vector(0x1C, xn_joy.old_int1c_off, xn_joy.old_int1c_sel);
    xn_sti();
}

/* The four buttons released */
static void release_buttons(void)
{
    xn_joy.button1 = xn_joy.button2 = 0;
    xn_joy.button3 = xn_joy.button4 = 0;
}

void xn_joy_reset_range(void)
{
    xn_joy.min_x = xn_joy.min_y = RANGE_NONE;
    xn_joy.max_x = xn_joy.max_y = 0;
    xn_joy.min_x_b = xn_joy.min_y_b = RANGE_NONE;
    xn_joy.max_x_b = xn_joy.max_y_b = 0;
    release_buttons();
}

void xn_joy_calibrate(void)
{
    s32 k;

    if (xn_joy.status == XN_JOY_OFF || xn_joy.installed == 0)
        return;
    xn_joy_reset_range();
    xn_joy.center.center_x = xn_joy.center.center_y = 0;
    xn_joy.center_b.center_x = xn_joy.center_b.center_y = 0;
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

/* One axis: the count's distance from the centre over the range on its side (min - centre
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
    v = -xn_s64_div_or0(&n, range);         /* Quirk Q-SYS-01: a range of 0 gives 0 */
    if (v < -AXIS_MAX)
        v = -AXIS_MAX;
    else if (v > AXIS_MAX)
        v = AXIS_MAX;
    return v;
}

void xn_joy_poll(void)
{
    s32 x, y;

    xn_joy.x = xn_joy.y = 0;
    xn_joy.x_b = xn_joy.y_b = 0;
    if (xn_joy.status == XN_JOY_OFF || xn_joy.installed == 0)
        return;
    x = xn_joy.raw_x;
    y = xn_joy.raw_y;
    widen(x, &xn_joy.min_x, &xn_joy.max_x);
    widen(y, &xn_joy.min_y, &xn_joy.max_y);
    widen(xn_joy.raw_x_b, &xn_joy.min_x_b, &xn_joy.max_x_b);
    widen(xn_joy.raw_y_b, &xn_joy.min_y_b, &xn_joy.max_y_b);
    xn_joy.x += joy_axis(x, xn_joy.center.center_x, xn_joy.min_x, xn_joy.max_x);
    xn_joy.y += joy_axis(y, xn_joy.center.center_y, xn_joy.min_y, xn_joy.max_y);
}

void xn_joy_timer_isr(void)
{
    s32 x = 0, y = 0, xb = 0, yb = 0;
    s32 k;
    u8 bits;

    release_buttons();
    if (xn_joy.status == XN_JOY_OFF)
        return;
    xn_outb(XN_JOY_PORT, 0x0F);             /* fire the one-shots */
    for (k = COUNT_MAX; k != 0; k--) {      /* count while any axis bit stays up */
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
