/* kbd.c: XnGine's keyboard as readable C (xkbd.h; see xngine.h). */
#include "xkbd.h"
#include "xmem.h"

void xn_kbd_install(void)
{
    xn_regs v;

    if (xn_kbd_installed == 1)
        return;
    xn_kbd_installed = 1;
    v.eax = 9;
    xn_asmcall(func_000A1272, &v);          /* _dos_getvect(9): DX:EAX */
    xn_kbd_old_int9_offset = v.eax;
    xn_kbd_old_int9_selector = (u16)v.edx;
    func_000A12A6(9, asm_xn_kbd_int9_handler, xn_cs());
    xn_mem_lock_region(asm_xn_kbd_install, 0x1EE);
    xn_mem_lock_region(&xn_kbd_old_int9_offset, 0x397);
    xn_kbd_keymap = &xn_kbd_ascii_table[0][0];
}

void xn_kbd_remove(void)
{
    if (xn_kbd_installed == 0)
        return;
    xn_kbd_installed = 0;
    func_000A12A6(9, (void *)xn_kbd_old_int9_offset, xn_kbd_old_int9_selector);
}

void xn_kbd_flush(void)
{
    volatile u32 *keys = (volatile u32 *)key_down;
    s32 k;

    for (k = 0; k < 32; k++)
        keys[k] = 0;
    xn_kbd_last_scancode = 0;
}

/* The last key from the handler's state (see xn_kbd_read_key_r) */
static u32 kbd_last_key(void)
{
    u32 scancode = xn_kbd_last_scancode;
    const u8 *key;
    u32 shifted;

    if (scancode == 0) {
        xn_kbd_last_scancode = 0;
        return 0;
    }
    key = xn_kbd_keymap + scancode - 1;
    shifted = (xn_kbd_shift_flags & (XN_KBD_LSHIFT | XN_KBD_RSHIFT)) != 0;
    xn_kbd_last_scancode = 0;
    return ((u32)key & 0xFFFF0000) | scancode << 8 | key[shifted << 7];
}

void xn_kbd_read_key_r(xn_regs *r)
{
    if (xn_kbd_installed) {
        r->eax = kbd_last_key();
        return;
    }
    r->eax = (r->eax & 0xFFFF00FF) | 0x0100;    /* a key waiting? */
    xn_int16(r);
    if (r->eflags & XN_ZF) {
        r->eax = 0;
        return;
    }
    r->eax = 0;                                 /* read it */
    xn_int16(r);
}

void xn_kbd_wait_key_r(xn_regs *r)
{
    if (xn_kbd_installed) {
        while (xn_kbd_last_scancode == 0)
            ;
        xn_kbd_read_key_r(r);
        return;
    }
    r->eax = 0;
    xn_int16(r);
}

void xn_kbd_int9_handler(void)
{
    u8 scancode = xn_inb(0x60);
    u8 control = xn_inb(0x61);
    u32 key = scancode & 0x7F;
    u32 shift;

    xn_outb(0x61, control | 0x80);              /* acknowledge: pulse bit 7 */
    xn_outb(0x61, control & 0x7F);
    switch (key) {
    case 0x1D: shift = XN_KBD_CTRL; break;
    case 0x2A: shift = XN_KBD_LSHIFT; break;
    case 0x36: shift = XN_KBD_RSHIFT; break;
    case 0x38: shift = XN_KBD_ALT; break;
    default: shift = 0; break;
    }
    if (scancode & 0x80) {                      /* released */
        xn_kbd_shift_flags &= ~shift;
        xn_kbd_last_scancode = 0;
        key_down[key] = 0;
    } else {
        xn_kbd_shift_flags |= shift;
        xn_kbd_last_scancode = scancode;
        key_down[key] = 1;
    }
    xn_outb(0x20, 0x20);                        /* end of interrupt */
}

void xn_kbd_wait_all_released(void)
{
    const volatile u8 *keys = &xn_kbd_last_scancode;    /* (sic: one byte early) */
    s32 k, held;

    if (xn_kbd_installed == 0)
        return;
    do {
        held = 0;
        for (k = 0; k < 128 && !held; k++)
            held = keys[k] == 1;
    } while (held);
}

void xn_kbd_numlock_off_r(xn_regs *r)
{
    xn_regs d = *r;

    XN_BIOS_KBD_FLAGS &= ~0x20;
    d.eax = (d.eax & 0xFFFF00FF) | 0x0100;
    xn_int16(&d);
}
