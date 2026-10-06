/* joy_t.c: test shims of src/engine/joy.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). */
#include "xjoy.h"

void xn_joy_init_r(xn_regs *r)
{
    xn_joy_init();
}

void xn_joy_shutdown_r(xn_regs *r)
{
    xn_joy_shutdown();
}

void xn_joy_reset_range_r(xn_regs *r)
{
    xn_joy_reset_range();
}

void xn_joy_calibrate_r(xn_regs *r)
{
    xn_joy_calibrate();
}

void xn_joy_poll_r(xn_regs *r)
{
    xn_joy_poll();
}

/* a vector: its interrupt stub calls the C handler */
void xn_joy_timer_isr_r(xn_regs *r)
{
    xn_joy_timer_isr();
}
