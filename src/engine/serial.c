/* serial.c: XnGine's 8250 UART driver as readable C (xserial.h; see xngine.h). */
#include "xserial.h"
#include "xmem.h"

void xn_serial_open(s32 port, u16 divisor, u8 lcr)
{
    s32 k = port - 1;
    u32 base;
    xn_regs v;

    if (xn_serial_port_active[k] != 1) {
        xn_serial_port_active[k] = 1;
        v.eax = xn_serial_port_vector[k];
        xn_asmcall(func_000A1272, &v);      /* _dos_getvect: DX:EAX */
        xn_serial_old_vector_sel[k] = (u16)v.edx;
        xn_serial_old_vector_off[k] = v.eax;
        func_000A12A6(xn_serial_port_vector[k], xn_serial_irq_handlers[k], xn_cs());
        xn_mem_lock_region(xn_serial_irq_handlers[k], 0x400);
        xn_mem_lock_region(xn_serial_port_active, 0x880);
    }
    xn_serial_rx_reset(port);
    xn_cli();
    base = xn_serial_port_base[k];
    xn_outb(base + 3, lcr | 0x80);          /* the divisor latch */
    xn_outw(base, divisor);
    xn_outb(base + 3, lcr & 0x7F);
    xn_outb(base + 4, 8);                   /* OUT2: interrupts reach the PIC */
    xn_outb(base + 1, 1);                   /* on received data */
    xn_inb(base);                           /* what the receive register holds: dropped */
    xn_outb(0x21, xn_inb(0x21) & xn_serial_port_pic_mask[k]);
    xn_sti();
    xn_outb(0x20, 0x20);
}

void xn_serial_open_r(xn_regs *r)
{
    xn_serial_open(r->eax, (u16)r->ebx, (u8)r->edx);
    r->edx = xn_serial_port_base[r->eax - 1];
    XN_SETFLAG(r, XN_CF, 0);
}

void xn_serial_close(s32 port)
{
    s32 k = port - 1;

    if (xn_serial_port_active[k] == 0)
        return;
    xn_cli();
    xn_serial_port_active[k] = 0;
    xn_outb(xn_serial_port_base[k] + 3, 0x40);
    xn_outb(0x21, xn_inb(0x21) | (u8)~xn_serial_port_pic_mask[k]);
    func_000A12A6(xn_serial_port_vector[k], (void *)xn_serial_old_vector_off[k],
                  xn_serial_old_vector_sel[k]);
    xn_outb(0x20, 0x20);
    xn_sti();
}

void xn_serial_rx_reset(s32 port)
{
    xn_cli();
    xn_serial_rx_head[port - 1] = 0;
    xn_serial_rx_tail[port - 1] = 0;
    xn_serial_rx_count[port - 1] = 0;
    xn_sti();
}

u8 xn_serial_read_lsr(s32 port)
{
    return xn_inb(xn_serial_port_base[port - 1] + 5);
}

s32 xn_serial_send_byte(s32 port, u8 byte)
{
    s32 tries;

    for (tries = 35; tries != 0; tries--) {
        while (xn_inb(XN_VGA_STATUS) & 8)   /* the end of a retrace */
            ;
        while (!(xn_inb(XN_VGA_STATUS) & 8))    /* the start of the next */
            ;
        if (xn_serial_read_lsr(port) & 0x20) {  /* the transmit register is empty */
            xn_outb(xn_serial_port_base[port - 1], byte);
            return 0;
        }
    }
    return 1;
}

void xn_serial_send_byte_r(xn_regs *r)
{
    r->eax = xn_serial_send_byte(r->eax, (u8)r->edx);
    XN_SETFLAG(r, XN_CF, r->eax);
}

s32 xn_serial_rx_get(s32 port, u8 *byte)
{
    s32 k = port - 1;
    u32 head = xn_serial_rx_head[k];

    if (head == xn_serial_rx_tail[k])
        return 0;
    xn_serial_rx_head[k] = (head + 1) & 0x1FF;
    xn_serial_rx_count[k]--;
    *byte = xn_serial_rx_buffers[k][0];     /* (sic: see xserial.h) */
    return 1;
}

void xn_serial_rx_get_r(xn_regs *r)
{
    u8 byte;
    s32 got = xn_serial_rx_get(r->eax, &byte);

    r->eax = got ? byte : 0;
    XN_SETFLAG(r, XN_CF, !got);
}

void xn_serial_irq_com1(void)
{
    u8 iir = xn_inb(XN_COM1 + 2);
    u32 tail;

    if ((iir & 1) == 0) {                   /* one is pending */
        if ((iir & 6) == 0) {               /* modem status */
            xn_inb(XN_COM1);
            xn_inb(XN_COM1 + 5);
        } else if (iir & 4) {               /* received data, or line status */
            xn_inb(XN_COM1 + 5);
            tail = xn_serial_rx_tail[0];
            xn_serial_rx_tail[0] = tail + 1;
            xn_serial_rx_buffers[0][tail] = xn_inb(XN_COM1);
            xn_serial_rx_tail[0] &= 0x1FF;
            xn_serial_rx_count[0]++;
        }
    }
    xn_outb(0x20, 0x20);                    /* end of interrupt */
}
