/* xkbd.h: XnGine's keyboard (src/engine/kbd.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     An int 9 handler that keeps a down flag per scan code (key_down: the game's keys.c reads
     it), the last key pressed and the Shift, Ctrl and Alt state; the reads of a key (from
     that state, or from the BIOS when the handler is not installed); and the waits for keys.

   Keys
     scan codes  the PC's set 1: a make code 01h..7Fh, its break code with bit 7 set.
     a key       AH the scan code, AL its ASCII from the keymap (US; the shifted half while
                 either Shift is down); 0 when there is none.

   The state (object 2, struct xn_kbd_state in xnstruct.h at 0x142300, locked by the
   install): xn_kbd_old_int9_offset / _selector (the vector replaced), xn_kbd_installed,
   xn_kbd_last_scancode (the last make code; 0 after any break or a read), key_down[128] (the
   game's), xn_kbd_shift_flags, xn_kbd_keymap (-> xn_kbd_ascii_table: [0] plain, [1] shifted,
   by scan code - 1).

   The handler's vector holds its interrupt entry (xn_kbd_int9_entry: the build's stub at the
   asm entry, which loads DS and ES and calls xn_kbd_int9_handler).

   Quirks (docs/engine/quirks.md): Q-KBD-01 (the key's EAX carries the keymap address's upper
   half, for the game: the boundary adapter), Q-KBD-02 (the release wait looks one byte early
   and misses key 127). */
#ifndef XKBD_H
#define XKBD_H

#include "xngine.h"

extern u32 xn_kbd_old_int9_offset;          /* the int 9 vector before ours */
extern u16 xn_kbd_old_int9_selector;
extern u8 xn_kbd_installed;
extern volatile u8 xn_kbd_last_scancode;    /* the last make code; 0 after a break or a read */
extern volatile u8 key_down[128];           /* 1 while the key of that scan code is held */
extern volatile u32 xn_kbd_shift_flags;     /* XN_KBD_LSHIFT ... */
extern u8 *xn_kbd_keymap;                   /* -> xn_kbd_ascii_table */
extern u8 xn_kbd_ascii_table[2][128];       /* ASCII by scan code - 1: [1] shifted (US) */

#define XN_KBD_LSHIFT   1
#define XN_KBD_RSHIFT   2
#define XN_KBD_ALT      4
#define XN_KBD_CTRL     8

/* The handler's interrupt entry (the build's stub at 142840), and the keyboard module's asm
   (142700, 1EEh bytes: the entry's) that the install locks */
extern void xn_kbd_int9_entry(void);
extern u8 xn_kbd_code_start[];

/* init_video: once, saves the int 9 vector and installs xn_kbd_int9_entry; locks the
   handler's code and its state (397h bytes); points the keymap at the table. */
void xn_kbd_install(void);

/* Puts the saved int 9 vector back (when installed). The fatal paths of every module. */
void xn_kbd_remove(void);

/* Forgets every key: key_down cleared, no last key. 11 game sites. */
void xn_kbd_flush(void);

/* The last key pressed (AH scan code, AL ASCII; 0 when none), forgotten as it is read.
   Without the handler: the BIOS's next key if one waits (int 16h 01h, then 00h), else 0.
   Three game sites (through xn_kbd_read_key_b), and xn_vid_play. */
u16 xn_kbd_read_key(void);

/* The game's calls of xn_kbd_read_key (boundary adapter: EAX as the asm left it, Q-KBD-01:
   with a key from the handler, its upper half is the keymap entry's address's). */
void xn_kbd_read_key_b(xn_regs *r);

/* Dead: waits for a key, then reads it (xn_kbd_read_key); without the handler, the BIOS's
   next key (int 16h 00h, which waits). */
u16 xn_kbd_wait_key(void);

/* The int 9 handler: the scan code from port 60h, acknowledged on port 61h (bit 7 pulsed); a
   make code sets its key_down flag and becomes the last key, a break code clears its flag
   and the last key; Ctrl, the Shifts and Alt are kept in xn_kbd_shift_flags; then the EOI.
   A vector (its entry stub's). The asm's 15 run-time blocks (14284D..1428EC) are this
   function's. */
void xn_kbd_int9_handler(void);

/* Waits until no key is down (when the handler is installed). Q-KBD-02: it scans the 128
   bytes from xn_kbd_last_scancode, one before key_down, so a last key of Esc (1) counts as
   held and key 127 is not looked at. Three game sites. */
void xn_kbd_wait_all_released(void);

/* init_video: NumLock off in the BIOS keyboard flags, then a BIOS keyboard call (int 16h
   01h) so the BIOS sees it. */
void xn_kbd_numlock_off(void);

#endif
