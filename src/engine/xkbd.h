/* xkbd.h: XnGine's keyboard (src/engine/kbd.c; see xngine.h): an int 9 handler that keeps a
   down flag per scan code, the last key pressed and the Shift, Ctrl and Alt state, and the
   reads of a key (from that, or from the BIOS when the handler is not installed). The state
   is struct xn_kbd_state (xnstruct.h) at 0x142300; the game's keys.c reads key_down.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose registers go to the BIOS as its caller left them. */
#ifndef XKBD_H
#define XKBD_H

#include "xngine.h"
#include "xsysutil.h"

extern u32 xn_kbd_old_int9_offset;          /* the int 9 vector before ours */
extern u16 xn_kbd_old_int9_selector;
extern u8 xn_kbd_installed;
extern volatile u8 xn_kbd_last_scancode;    /* the last make code; 0 after a break or a read */
extern volatile u8 key_down[128];           /* 1 while the key of that scan code is held */
extern volatile u32 xn_kbd_shift_flags;     /* 1 left Shift, 2 right Shift, 4 Alt, 8 Ctrl */
extern u8 *xn_kbd_keymap;                   /* -> xn_kbd_ascii_table */
extern u8 xn_kbd_ascii_table[2][128];       /* ASCII by scan code - 1: [1] shifted (US) */

#define XN_KBD_LSHIFT   1
#define XN_KBD_RSHIFT   2
#define XN_KBD_ALT      4
#define XN_KBD_CTRL     8

/* The BIOS keyboard flags (0040:0017), a flat address: bit 5 NumLock */
#define XN_BIOS_KBD_FLAGS (*(volatile u8 *)0x417)

/* The handler's asm entry (the vector holds it; its route takes it to the C) and the asm
   entry of the installer, whose code the install locks */
void asm_xn_kbd_int9_handler(void);
void asm_xn_kbd_install(void);

/* init_video: once, saves the int 9 vector and installs xn_kbd_int9_handler; locks the
   handler's code (1EEh bytes from the installer) and the state (397h bytes); the keymap. */
void xn_kbd_install(void);
#pragma aux xn_kbd_install parm [] modify exact [eax];

/* Puts the saved int 9 vector back (when installed). */
void xn_kbd_remove(void);
#pragma aux xn_kbd_remove parm [] modify exact [eax];

/* Forgets every key: key_down cleared, no last key. */
void xn_kbd_flush(void);
#pragma aux xn_kbd_flush parm [] modify exact [eax];

/* The last key pressed and forgets it: AH the scan code, AL its ASCII (shifted while either
   Shift is down), 0 when none. Kept from the asm: the upper half of EAX is the keymap
   pointer's (the asm looks the key up in EAX). Without the handler: the BIOS's next key (int
   16h AH = 1 with the caller's AL and upper half, then AH = 0), 0 when none waits. */
void xn_kbd_read_key_r(xn_regs *r);

/* Dead: waits for a key, then xn_kbd_read_key; without the handler, the BIOS's (int 16h 0). */
void xn_kbd_wait_key_r(xn_regs *r);

/* The int 9 handler: the scan code from port 60h (acknowledged on port 61h); a make code
   sets its key_down flag and becomes the last key, a break code clears its flag and the last
   key; Ctrl, the Shifts and Alt are kept in xn_kbd_shift_flags; then the EOI. The asm's 15
   run-time blocks (14284D..1428EC) are this function's. */
void xn_kbd_int9_handler(void);

/* Waits until no key is down. Kept from the asm: it scans 128 bytes from
   xn_kbd_last_scancode, one before key_down, so a last key of Esc (1) counts as down and
   key 127 is not looked at. */
void xn_kbd_wait_all_released(void);

/* init_video: NumLock off in the BIOS flags, then int 16h AH = 1 (AL and the upper half of
   EAX as the caller left them) so the BIOS sees it; every register kept. */
void xn_kbd_numlock_off_r(xn_regs *r);

#endif
