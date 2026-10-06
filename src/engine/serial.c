/* serial.c: XnGine's 8250 UART driver (canonical C; the interface and the module's
   documentation are in xserial.h). */
#include "xserial.h"
#include "xsysutil.h"
#include "xpc.h"
#include "xmem.h"

/* The UART's registers, from its base */
#define UART_DATA       0                   /* receive / transmit; the divisor's low byte */
#define UART_IER        1                   /* interrupt enable; the divisor's high byte */
#define UART_IIR        2                   /* interrupt identification */
#define UART_LCR        3                   /* line control: bit 7 the divisor latch */
#define UART_MCR        4                   /* modem control: bit 3 OUT2 */
#define UART_LSR        5                   /* line status */
#define LSR_THR_EMPTY   0x20
#define PIC_COMMAND     0x20
#define PIC_MASK        0x21
#define PIC_EOI         0x20
#define DRIVER_DATA_BYTES 0x880             /* the driver's data, from xn_serial_port_active */
#define HANDLER_BYTES   0x400

void xn_serial_open(s32 port, u16 divisor, u8 lcr)
{
    s32 k = port - 1;
    u32 base;

    if (xn_serial_port_active[k] != 1) {
        xn_serial_port_active[k] = 1;
        xn_pc_get_vector((u8)xn_serial_port_vector[k], &xn_serial_old_vector_off[k],
                         &xn_serial_old_vector_sel[k]);
        xn_pc_install_vector((u8)xn_serial_port_vector[k], xn_serial_irq_handlers[k]);
        xn_mem_lock_region((void *)xn_serial_irq_handlers[k], HANDLER_BYTES);
        xn_mem_lock_region(xn_serial_port_active, DRIVER_DATA_BYTES);
    }
    xn_serial_rx_reset(port);
    xn_cli();
    base = xn_serial_port_base[k];
    xn_outb(base + UART_LCR, lcr | 0x80);   /* the divisor latch */
    xn_outw(base + UART_DATA, divisor);
    xn_outb(base + UART_LCR, lcr & 0x7F);
    xn_outb(base + UART_MCR, 8);            /* OUT2: interrupts reach the PIC */
    xn_outb(base + UART_IER, 1);            /* on received data */
    xn_inb(base + UART_DATA);               /* what the receive register holds: dropped */
    xn_outb(PIC_MASK, xn_inb(PIC_MASK) & xn_serial_port_pic_mask[k]);
    xn_sti();
    xn_outb(PIC_COMMAND, PIC_EOI);
}

void xn_serial_close(s32 port)
{
    s32 k = port - 1;

    if (xn_serial_port_active[k] == 0)
        return;
    xn_cli();
    xn_serial_port_active[k] = 0;
    xn_outb(xn_serial_port_base[k] + UART_LCR, 0x40);  /* Quirk Q-SERIAL-02: the break bit */
    xn_outb(PIC_MASK, xn_inb(PIC_MASK) | (u8)~xn_serial_port_pic_mask[k]);
    xn_pc_set_vector((u8)xn_serial_port_vector[k], xn_serial_old_vector_off[k],
                     xn_serial_old_vector_sel[k]);
    xn_outb(PIC_COMMAND, PIC_EOI);
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
    return xn_inb(xn_serial_port_base[port - 1] + UART_LSR);
}

s32 xn_serial_send_byte(s32 port, u8 byte)
{
    s32 tries;

    for (tries = 35; tries != 0; tries--) {
        while (xn_inb(XN_VGA_STATUS) & 8)       /* the end of a retrace */
            ;
        while (!(xn_inb(XN_VGA_STATUS) & 8))    /* the start of the next */
            ;
        if (xn_serial_read_lsr(port) & LSR_THR_EMPTY) {
            xn_outb(xn_serial_port_base[port - 1] + UART_DATA, byte);
            return 0;
        }
    }
    return 1;
}

s32 xn_serial_rx_get(s32 port, u8 *byte)
{
    s32 k = port - 1;
    u32 head = xn_serial_rx_head[k];

    if (head == xn_serial_rx_tail[k])
        return 0;
    xn_serial_rx_head[k] = (head + 1) & (XN_SERIAL_RING - 1);
    xn_serial_rx_count[k]--;
    *byte = xn_serial_rx_buffers[k][0];     /* Quirk Q-SERIAL-01: not [head] */
    return 1;
}

void xn_serial_irq_com1(void)
{
    u8 iir = xn_inb(XN_COM1 + UART_IIR);
    u32 tail;

    if ((iir & 1) == 0) {                   /* an interrupt is pending */
        if ((iir & 6) == 0) {               /* modem status (Quirk Q-SERIAL-03) */
            xn_inb(XN_COM1 + UART_DATA);
            xn_inb(XN_COM1 + UART_LSR);
        } else if (iir & 4) {               /* received data, or line status */
            xn_inb(XN_COM1 + UART_LSR);
            tail = xn_serial_rx_tail[0];
            xn_serial_rx_tail[0] = tail + 1;
            xn_serial_rx_buffers[0][tail] = xn_inb(XN_COM1 + UART_DATA);
            xn_serial_rx_tail[0] &= XN_SERIAL_RING - 1;
            xn_serial_rx_count[0]++;
        }
    }
    xn_outb(PIC_COMMAND, PIC_EOI);
}

#ifdef DAGGER_PORT
/* DAGGER_PORT: the IRQ entries (object 2's table of COM1's handler and its copies). The
   virtual PC calls a vector as C, and has no UART: no config opens a tracker. */
void (*xn_serial_irq_handlers[4])(void) = {
    xn_serial_irq_com1, xn_serial_irq_com1, xn_serial_irq_com1, xn_serial_irq_com1
};
#endif
