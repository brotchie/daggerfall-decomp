/* xjoy.h: XnGine's joystick (src/engine/joy.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     Reads a PC game-port joystick (port 201h). An int 1Ch handler (the BIOS tick, 18.2 Hz)
     fires the port's one-shots and counts how long each axis bit stays up, and reads the
     four buttons; every frame xn_joy_poll widens the ranges seen and turns stick A's counts
     into joystick_x and joystick_y in -4095..4095 around the calibrated centre, with a dead
     zone. Stick B's ranges and counts are kept, its outputs only cleared.

   Units
     counts      port reads while an axis bit stays up (at most 800h).
     outputs     -4095..4095: the distance from the centre over the range seen on that side,
                 times -4096 (right and down are negative).

   The state (object 2, 6Eh bytes at 0x152A00, locked by the install): struct xn_joy_state
   (xnstruct.h), xn_joy; the game reads joystick_status (2: off; the options turn it on),
   joystick_x / _y and the buttons.

   The handler's vector holds its interrupt entry (xn_joy_timer_entry: the build's stub at the
   asm entry, which loads DS and ES and calls xn_joy_timer_isr).

   Quirks (docs/engine/quirks.md): Q-JOY-01 (the axis mask's pairing tests do nothing),
   Q-SYS-01 (a range of 0 divides by zero: that axis is 0). */
#ifndef XJOY_H
#define XJOY_H

#include "xngine.h"
#include "xnstruct.h"

extern struct xn_joy_state xn_joy;          /* 0x152A00 */

#define XN_JOY_OFF      2                   /* joystick_status: disabled */
#define XN_JOY_PORT     0x201

/* The handler's interrupt entry (the build's stub at 152EA0), and the end of its asm
   (152F41), which the install locks */
extern void xn_joy_timer_entry(void);
extern u8 xn_joy_timer_end[];

/* init_game_data: once, finds which axis bits of port 201h answer (64 reads after a BIOS
   tick: the bits that read 0 every time, Q-JOY-01), sets the joystick off, installs the int
   1Ch handler and locks its code and the state. */
void xn_joy_init(void);

/* shutdown_free_all and the fatal paths: the int 1Ch vector back (when installed); the
   joystick off. */
void xn_joy_shutdown(void);

/* The ranges back to "nothing seen" (minimums 32000, maximums 0) and the buttons released. */
void xn_joy_reset_range(void);

/* The options' CALIBRATE (when the joystick is on): the ranges reset, then each stick's
   centre the average of its counts over 4 BIOS ticks, rounded. */
void xn_joy_calibrate(void);

/* Every frame (3 game sites): joystick_x and joystick_y from the last counts, the ranges
   widened first; stick B's outputs cleared. Nothing but the clearing while the joystick is
   off. */
void xn_joy_poll(void);

/* The int 1Ch handler: the buttons cleared; while the joystick is on, the four axes timed
   (port 201h fired, then read while any axis bit is up, at most 800h times, each read adding
   its bits to the counts) and the four buttons read (set while pressed: their bit 0). A
   vector (its entry stub's). The asm's 4 run-time blocks (152EAD..152F40) are this
   function's. */
void xn_joy_timer_isr(void);

#endif
