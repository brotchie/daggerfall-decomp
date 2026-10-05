/* xserial.h: XnGine's 8250 UART driver (src/engine/serial.c; see xngine.h) for COM1-4: an
   IRQ handler filling a 512-byte receive ring per port, and byte sends paced by the vertical
   retrace. Only the head-tracker drivers (xhelmet.h) use it, and the game never runs them.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. Ports are numbered 1..4. */
#ifndef XSERIAL_H
#define XSERIAL_H

#include "xngine.h"
#include "xsysutil.h"

extern u8 xn_serial_port_active[4];         /* the IRQ handler installed */
extern u32 xn_serial_port_base[4];          /* 3F8h 2F8h 3E8h 2E8h */
extern u32 xn_serial_port_vector[4];        /* 0Ch 0Bh 0Ch 0Bh */
extern u8 xn_serial_port_pic_mask[4];       /* EFh F7h EFh F7h: the IRQ's bit clear */
extern u32 xn_serial_old_vector_off[4];     /* the vectors replaced */
extern u16 xn_serial_old_vector_sel[4];
extern void (*xn_serial_irq_handlers[4])(void);  /* 0x160AE0 and three undecoded copies */
extern u8 xn_serial_rx_buffers[4][512];     /* the receive rings */
extern volatile u32 xn_serial_rx_head[4];   /* where the next byte is read */
extern volatile u32 xn_serial_rx_tail[4];   /* where the IRQ writes the next */
extern volatile u32 xn_serial_rx_count[4];  /* bytes waiting */

#define XN_COM1         0x3F8               /* the IRQ handler's port */
#define XN_VGA_STATUS   0x3DA               /* bit 3: vertical retrace */

/* Opens a port: the first time, saves and hooks its IRQ vector and locks the handler and the
   driver's data; the ring emptied; line control lcr (8N1: 3) at divisor (6: 19200 baud),
   OUT2 and the receive interrupt on, the IRQ unmasked. The asm leaves EDX = the port's base
   and CF clear. */
void xn_serial_open(s32 port, u16 divisor, u8 lcr);
void xn_serial_open_r(xn_regs *r);

/* Closes a port: the IRQ masked and the vector put back. Kept from the asm: it writes 40h to
   the line control register (the break bit), not 0. */
void xn_serial_close(s32 port);
#pragma aux xn_serial_close parm [eax] modify exact [eax];

/* Empties a port's receive ring (interrupts off). */
void xn_serial_rx_reset(s32 port);
#pragma aux xn_serial_rx_reset parm [eax] modify exact [eax];

/* The port's line status register. */
u8 xn_serial_read_lsr(s32 port);

/* Sends a byte when the transmitter is empty, looking once per vertical retrace, 35 times.
   Returns 0 when sent, 1 when it gave up (the asm's CF too). */
s32 xn_serial_send_byte(s32 port, u8 byte);
void xn_serial_send_byte_r(xn_regs *r);

/* The next received byte to *byte; 0 when the ring is empty (the asm's CF; EAX 0). Kept from
   the asm: the head moves on, but the byte read is always the ring's first (the asm does not
   add the head to the ring's address). */
s32 xn_serial_rx_get(s32 port, u8 *byte);
void xn_serial_rx_get_r(xn_regs *r);

/* COM1's IRQ handler: a received byte (after the line status) goes into port 1's ring. Kept
   from the asm: a modem status interrupt reads the data and line status registers (not the
   modem status register), and a transmit interrupt nothing. Then the EOI. */
void xn_serial_irq_com1(void);

#endif
