/* xserial.h: XnGine's 8250 UART driver (src/engine/serial.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     Drives COM1-4 for the head trackers (xhelmet.h): an IRQ handler that fills a 512-byte
     receive ring per port, and byte sends paced by the VGA's vertical retrace. The game opens
     a tracker only when its config asks for one ('helmet'), which none does.

   Ports are numbered 1..4 (index port - 1 in the tables). A ring's head is where the next
   byte is read, its tail where the IRQ writes the next; both wrap at 512.

   Globals (object 2, 880h bytes from 0x160000, locked by the first open): the ports' bases,
   vectors and PIC masks, the vectors replaced, the IRQ handlers' entries (COM1's at 160AE0,
   and three copies for COM2-4 that nothing installs), the rings and their indices.

   Quirks (docs/engine/quirks.md): Q-SERIAL-01 (rx_get reads the ring's first byte),
   Q-SERIAL-02 (close sets the line control's break bit), Q-SERIAL-03 (the IRQ handler reads
   the wrong registers for a modem status interrupt), Q-SERIAL-04 (the IRQ handler returns
   with a 16-bit iret: its entry stub keeps it). */
#ifndef XSERIAL_H
#define XSERIAL_H

#include "xngine.h"

extern u8 xn_serial_port_active[4];         /* the IRQ handler installed */
extern u32 xn_serial_port_base[4];          /* 3F8h 2F8h 3E8h 2E8h */
extern u32 xn_serial_port_vector[4];        /* 0Ch 0Bh 0Ch 0Bh */
extern u8 xn_serial_port_pic_mask[4];       /* EFh F7h EFh F7h: the IRQ's bit clear */
extern u32 xn_serial_old_vector_off[4];     /* the vectors replaced */
extern u16 xn_serial_old_vector_sel[4];
extern void (*xn_serial_irq_handlers[4])(void);  /* the IRQ entries: COM1's, three copies */
extern u8 xn_serial_rx_buffers[4][512];     /* the receive rings */
extern volatile u32 xn_serial_rx_head[4];   /* where the next byte is read */
extern volatile u32 xn_serial_rx_tail[4];   /* where the IRQ writes the next */
extern volatile u32 xn_serial_rx_count[4];  /* bytes waiting */

#define XN_COM1         0x3F8               /* the IRQ handler's port */
#define XN_VGA_STATUS   0x3DA               /* bit 3: vertical retrace */
#define XN_SERIAL_RING  0x200

/* Opens a port: the first time, saves and hooks its IRQ vector and locks the handler and the
   driver's data; the ring emptied; line control lcr (8N1: 3) at divisor (6: 19200 baud),
   OUT2 and the receive interrupt on, the IRQ unmasked, an EOI. */
void xn_serial_open(s32 port, u16 divisor, u8 lcr);

/* Closes a port (when open): the IRQ masked, the vector put back, an EOI. Q-SERIAL-02: it
   writes 40h (the break bit) to the line control register, not 0. */
void xn_serial_close(s32 port);

/* Empties a port's receive ring (interrupts off meanwhile). */
void xn_serial_rx_reset(s32 port);

/* The port's line status register (bit 5: the transmitter takes a byte; bit 6: it is idle). */
u8 xn_serial_read_lsr(s32 port);

/* Sends a byte when the transmitter takes one, looking once per vertical retrace, 35 times.
   Returns 0 when sent, 1 when it gave up. */
s32 xn_serial_send_byte(s32 port, u8 byte);

/* The next received byte to *byte: 1, or 0 when the ring is empty. Q-SERIAL-01: the head
   moves on, but the byte is always the ring's first. */
s32 xn_serial_rx_get(s32 port, u8 *byte);

/* COM1's IRQ handler: a received byte (after the line status) goes into port 1's ring; then
   the EOI. Q-SERIAL-03: a modem status interrupt reads the data and line status registers
   (not the modem status register), and a transmit interrupt nothing. A vector (its entry
   stub's, which returns with the asm's 16-bit iret: Q-SERIAL-04). */
void xn_serial_irq_com1(void);

#endif
