/* timer_t.c: test shims of src/engine/timer.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). */
#include "xtimer.h"

void xn_timer_bios_ticks_r(xn_regs *r)
{
    r->eax = xn_timer_bios_ticks();
}

/* last EAX, counter EDX */
void xn_timer_wait_change_r(xn_regs *r)
{
    xn_timer_wait_change((s32 *)r->eax, (volatile s32 *)r->edx);
}

void xn_timer_tick_callback_r(xn_regs *r)
{
    xn_timer_tick_callback();
}

void xn_timer_add_25_r(xn_regs *r)
{
    xn_timer_add_25();
}

void xn_timer_fps_update_r(xn_regs *r)
{
    r->eax = xn_timer_fps_update();
}

void xn_timer_read_pit_r(xn_regs *r)
{
    r->eax = xn_timer_read_pit();
}

void xn_timer_wait_ticks_r(xn_regs *r)
{
    xn_timer_wait_ticks(r->eax);
}
