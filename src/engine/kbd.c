/* kbd.c: XnGine's keyboard (canonical C; the interface and the module's documentation are in
   xkbd.h). */
#include "xkbd.h"
#include "xpc.h"
#include "xmem.h"

#define KBD_DATA        0x60            /* the keyboard controller's data port */
#define KBD_CONTROL     0x61            /* port B: bit 7 acknowledges the key */
#define PIC_COMMAND     0x20
#define PIC_EOI         0x20
#define KBD_CODE_BYTES  0x1EE           /* the keyboard module's asm, from 142700 */
#define KBD_STATE_BYTES 0x397           /* the state, from xn_kbd_old_int9_offset */

void xn_kbd_install(void)
{
    if (xn_kbd_installed == 1)
        return;
    xn_kbd_installed = 1;
    xn_pc_get_vector(9, &xn_kbd_old_int9_offset, &xn_kbd_old_int9_selector);
    xn_pc_install_vector(9, xn_kbd_int9_entry);
    xn_mem_lock_region(xn_kbd_code_start, KBD_CODE_BYTES);
    xn_mem_lock_region(&xn_kbd_old_int9_offset, KBD_STATE_BYTES);
    xn_kbd_keymap = &xn_kbd_ascii_table[0][0];
}

void xn_kbd_remove(void)
{
    if (xn_kbd_installed == 0)
        return;
    xn_kbd_installed = 0;
    xn_pc_set_vector(9, xn_kbd_old_int9_offset, xn_kbd_old_int9_selector);
}

void xn_kbd_flush(void)
{
    volatile u32 *keys = (volatile u32 *)key_down;      /* (dwords: Watcom would call a
                                                           library __STOSB for bytes) */
    s32 k;

    for (k = 0; k < 32; k++)
        keys[k] = 0;
    xn_kbd_last_scancode = 0;
}

/* The last key from the handler's state, forgotten */
static u16 handler_key(void)
{
    u32 scancode = xn_kbd_last_scancode;
    u32 shifted;
    u16 key = 0;

    if (scancode != 0) {
        shifted = (xn_kbd_shift_flags & (XN_KBD_LSHIFT | XN_KBD_RSHIFT)) != 0;
        key = (u16)(scancode << 8 | xn_kbd_keymap[(shifted << 7) + scancode - 1]);
    }
    xn_kbd_last_scancode = 0;
    return key;
}

u16 xn_kbd_read_key(void)
{
    if (xn_kbd_installed)
        return handler_key();
    if (!xn_bios_key_waiting(0))
        return 0;
    return xn_bios_read_key();
}

void xn_kbd_read_key_b(xn_regs *r)
{
    int from_handler = xn_kbd_installed != 0;
    u16 key = xn_kbd_read_key();

    r->eax = key;
    /* Quirk Q-KBD-01: the asm looks the key up with EAX pointing into the keymap and leaves
       that pointer's upper half */
    if (from_handler && key != 0)
        r->eax |= ((u32)xn_kbd_keymap + (key >> 8) - 1) & 0xFFFF0000;
}

u16 xn_kbd_wait_key(void)
{
    if (!xn_kbd_installed)
        return xn_bios_read_key();
    while (xn_kbd_last_scancode == 0)
        ;
    return xn_kbd_read_key();
}

void xn_kbd_int9_handler(void)
{
    u8 scancode = xn_inb(KBD_DATA);
    u8 control = xn_inb(KBD_CONTROL);
    u32 key = scancode & 0x7F;
    u32 shift;

    xn_outb(KBD_CONTROL, control | 0x80);       /* acknowledge: pulse bit 7 */
    xn_outb(KBD_CONTROL, control & 0x7F);
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
    xn_outb(PIC_COMMAND, PIC_EOI);
}

void xn_kbd_wait_all_released(void)
{
    const volatile u8 *keys = &xn_kbd_last_scancode;    /* Quirk Q-KBD-02: one byte early */
    s32 k, held;

    if (xn_kbd_installed == 0)
        return;
    do {
        held = 0;
        for (k = 0; k < 128 && !held; k++)
            held = keys[k] == 1;
    } while (held);
}

void xn_kbd_numlock_off(void)
{
    XN_BIOS_KBD_FLAGS &= ~XN_BIOS_NUMLOCK;
    xn_bios_key_waiting(0);
}
