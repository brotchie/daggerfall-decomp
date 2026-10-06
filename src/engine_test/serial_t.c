/* serial_t.c: test shims of src/engine/serial.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). */
#include "xserial.h"

/* port EAX, divisor BX, lcr DL -> EDX the port's base, CF clear (the asm's last flags) */
void xn_serial_open_r(xn_regs *r)
{
    xn_serial_open(r->eax, (u16)r->ebx, (u8)r->edx);
    r->edx = xn_serial_port_base[r->eax - 1];
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_serial_close_r(xn_regs *r)
{
    xn_serial_close(r->eax);
}

void xn_serial_rx_reset_r(xn_regs *r)
{
    xn_serial_rx_reset(r->eax);
}

/* -> AL */
void xn_serial_read_lsr_r(xn_regs *r)
{
    r->eax = (r->eax & ~0xFFu) | xn_serial_read_lsr(r->eax);
}

/* port EAX, byte DL -> EAX and CF: 0 sent, 1 not */
void xn_serial_send_byte_r(xn_regs *r)
{
    r->eax = xn_serial_send_byte(r->eax, (u8)r->edx);
    XN_SETFLAG(r, XN_CF, r->eax);
}

/* port EAX -> AL the byte, CF clear; or EAX 0, CF set */
void xn_serial_rx_get_r(xn_regs *r)
{
    u8 byte;
    s32 got = xn_serial_rx_get(r->eax, &byte);

    r->eax = got ? byte : 0;
    XN_SETFLAG(r, XN_CF, !got);
}

/* a vector: its interrupt stub calls the C handler */
void xn_serial_irq_com1_r(xn_regs *r)
{
    xn_serial_irq_com1();
}
