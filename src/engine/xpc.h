/* xpc.h: the PC under XnGine (src/engine/pc.c): the platform layer. Canonical C: plain
   prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it is
     Named C functions over the services the engine calls and the memory it reads from the
     BIOS: the DPMI host (int 31h, CauseWay's), the keyboard and PS/2 BIOS (int 16h, 15h),
     the multiplex interrupt's time-slice release (int 2Fh), the mouse driver (int 33h), and
     the protected-mode interrupt vectors (through the game's C library). Each function is
     one service call: it puts the service's inputs in their registers, calls it (glue.asm's
     xn_intNN, the boundary's only register code) and gives back what the service answers.
     DOS's own services (int 21h) are xdos.h's; the ports are xngine.h's xn_inb / xn_outb;
     the interrupt flag and tail calls are xsysutil.h's intrinsics.

   Conventions
     A function that can fail returns 1 when the service worked (its carry clear), else 0.
     Selectors and real-mode segments are 16 bits; linear addresses and sizes 32 bits.
     The mouse driver works in 16-bit signed pixels and its own sensitivity units.

   The BIOS data area is at its linear address (CauseWay maps the first megabyte at 0): the
   tick count XN_BIOS_TICKS and the keyboard flags XN_BIOS_KBD_FLAGS. */
#ifndef XPC_H
#define XPC_H

#include "xngine.h"

/* ---- the BIOS data area ------------------------------------------------------------------ */

#ifdef DAGGER_PORT
/* the native build: real-mode memory is the virtual PC's (port/include/port_vpc.h) */
extern unsigned char *port_low_memory;
#define XN_BIOS_TICKS       (*(volatile u32 *)(port_low_memory + 0x46C))
#define XN_BIOS_KBD_FLAGS   (*(volatile u8 *)(port_low_memory + 0x417))
#else
/* The BIOS tick count (0040:006C), 18.2 a second */
#define XN_BIOS_TICKS       (*(volatile u32 *)0x46C)
/* The BIOS keyboard flags (0040:0017): bit 5 NumLock */
#define XN_BIOS_KBD_FLAGS   (*(volatile u8 *)0x417)
#endif
#define XN_BIOS_NUMLOCK     0x20

/* ---- interrupt vectors (the game's C library: _dos_getvect, and its setvect at 0xA12A6) --- */

/* Interrupt n's protected-mode handler: *offset and *selector. */
void xn_pc_get_vector(u8 n, u32 *offset, u16 *selector);

/* Makes selector:offset interrupt n's handler. */
void xn_pc_set_vector(u8 n, u32 offset, u16 selector);

/* Makes entry, in this program's code segment, interrupt n's handler. */
void xn_pc_install_vector(u8 n, void (*entry)(void));

/* ---- DPMI (int 31h) ----------------------------------------------------------------------- */

/* A DPMI real-mode register block (for DPMI 0300h) */
typedef struct xn_dpmi_rm_regs {
    u32 edi, esi, ebp, reserved, ebx, edx, ecx, eax;
    u16 flags, es, ds, fs, gs, ip, cs, sp, ss;
} xn_dpmi_rm_regs;

/* A selector for the real-mode segment `segment` (0002h) to *selector. */
int xn_dpmi_segment_selector(u16 segment, u16 *selector);

/* The linear base address of selector (0006h) to *base. */
int xn_dpmi_selector_base(u16 selector, u32 *base);

/* Allocates `paragraphs` 16-byte paragraphs of DOS memory (0100h): its real-mode segment to
   *segment, its selector to *selector. */
int xn_dpmi_dos_alloc(u16 paragraphs, u16 *segment, u16 *selector);

/* Frees the DOS memory of selector (0101h). */
int xn_dpmi_dos_free(u16 selector);

/* Exception n's handler (0202h): *offset and *selector. */
void xn_dpmi_get_exception(u8 n, u32 *offset, u16 *selector);

/* Makes selector:offset exception n's handler (0203h). */
void xn_dpmi_set_exception(u8 n, u32 offset, u16 selector);

/* Makes entry, in this program's code segment, exception n's handler (0203h). */
void xn_dpmi_install_exception(u8 n, void (*entry)(void));

/* Runs real-mode interrupt n with the registers in *rm (0300h, nothing copied from the
   stack); the real-mode registers come back in *rm. */
int xn_dpmi_real_int(u8 n, xn_dpmi_rm_regs *rm);

/* Locks size bytes at addr in memory (0600h): interrupt handlers and their data. */
int xn_dpmi_lock(const void *addr, u32 size);

/* CauseWay's DOS transfer buffer (FF26h): size bytes of DOS memory at segment:0, selector. */
void xn_cw_set_transfer_buffer(u16 segment, u16 selector, u32 size);

/* ---- the BIOS ------------------------------------------------------------------------------ */

/* 1 when a key waits in the BIOS's buffer (int 16h 01h), with it to *key (AH scan code, AL
   ASCII; key may be 0); it stays there. */
int xn_bios_key_waiting(u16 *key);

/* The BIOS's next key (int 16h 00h; waits for one): AH the scan code, AL the ASCII. */
u16 xn_bios_read_key(void);

/* The PS/2 pointing device's sample rate (int 15h C202h, BH = rate). */
void xn_bios_ps2_sample_rate(u8 rate);

/* ---- the multiplex interrupt (int 2Fh) ------------------------------------------------------ */

/* Releases the rest of this time slice to the host (1680h: Windows, OS/2, DPMI hosts). */
void xn_pc_yield(void);

/* ---- the mouse driver (int 33h) ---------------------------------------------------------------- */

/* The buttons (bit 0 left, bit 1 right) and the pointer's position (03h). */
void xn_mousedrv_state(u16 *buttons, s16 *x, s16 *y);

/* Moves the pointer to (x, y) (04h). */
void xn_mousedrv_set_position(s16 x, s16 y);

/* The pointer's horizontal range lo..hi (07h). */
void xn_mousedrv_x_range(s16 lo, s16 hi);

/* The pointer's vertical range lo..hi (08h). */
void xn_mousedrv_y_range(s16 lo, s16 hi);

/* The motion since the last call, in mickeys (0Bh). */
void xn_mousedrv_motion(s16 *dx, s16 *dy);

/* The sensitivity: horizontal and vertical, and the double-speed threshold (1Ah). */
void xn_mousedrv_set_speed(u16 horizontal, u16 vertical, u16 threshold);

/* The sensitivity (1Bh). */
void xn_mousedrv_speed(u16 *horizontal, u16 *vertical, u16 *threshold);

#endif
