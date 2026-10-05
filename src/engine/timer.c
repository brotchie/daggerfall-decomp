/* timer.c: XnGine's timing as readable C (xtimer.h; see xngine.h). */
#include "xtimer.h"

u32 xn_timer_bios_ticks(void)
{
    return XN_BIOS_TICKS;
}

void xn_timer_wait_change(s32 *last, volatile s32 *counter)
{
    s32 seen = *last;

    while (*counter == seen)
        ;
    *last = *counter;
}

void xn_timer_tick_callback(void)
{
    timer_tick_count++;
}

void xn_timer_add_25(void)
{
    frame_timer_ms += 25;
}

u32 xn_timer_fps_update(void)
{
    xn_fps_frame_count++;
    if (XN_BIOS_TICKS - xn_fps_last_tick > 18) {
        xn_fps_last_tick = XN_BIOS_TICKS;
        xn_timer_fps = xn_fps_frame_count;
        xn_fps_average = (xn_timer_fps + xn_fps_average) >> 1;
        xn_fps_frame_count = 0;
    }
    return (u16)xn_timer_fps;
}

u32 xn_timer_read_pit(void)
{
    u32 lo, hi;

    xn_outb(0x43, 0xB0);
    lo = xn_inb(0x40);
    hi = xn_inb(0x40);
    return hi << 8 | lo;
}

void xn_timer_wait_ticks(u32 n)
{
    u32 start = XN_BIOS_TICKS;

    while (XN_BIOS_TICKS - start < n)
        ;
}
