/* kbd_t.c: test shims of src/engine/kbd.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). */
#include "xkbd.h"

void xn_kbd_install_r(xn_regs *r)
{
    xn_kbd_install();
}

void xn_kbd_remove_r(xn_regs *r)
{
    xn_kbd_remove();
}

void xn_kbd_flush_r(xn_regs *r)
{
    xn_kbd_flush();
}

/* -> EAX as the asm leaves it (the boundary adapter has it) */
void xn_kbd_read_key_r(xn_regs *r)
{
    xn_kbd_read_key_b(r);
}

/* -> EAX: the read's (with the handler: after the wait, as xn_kbd_read_key_r) */
void xn_kbd_wait_key_r(xn_regs *r)
{
    if (!xn_kbd_installed) {
        r->eax = xn_kbd_wait_key();
        return;
    }
    while (xn_kbd_last_scancode == 0)
        ;
    xn_kbd_read_key_b(r);
}

/* a vector: its interrupt stub calls the C handler */
void xn_kbd_int9_handler_r(xn_regs *r)
{
    xn_kbd_int9_handler();
}

void xn_kbd_wait_all_released_r(xn_regs *r)
{
    xn_kbd_wait_all_released();
}

void xn_kbd_numlock_off_r(xn_regs *r)
{
    xn_kbd_numlock_off();
}
