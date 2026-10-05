/* xjoy.h: XnGine's joystick (src/engine/joy.c; see xngine.h): port 201h timed by an int 1Ch
   (BIOS tick) handler, ranges widened as the stick moves, joystick_x and joystick_y in
   -4095..4095 with a dead zone. The state is struct xn_joy_state (xnstruct.h) at 0x152A00,
   whose globals names.csv names one by one (xn_joy_axis_mask, joystick_status, joystick_x...);
   the C reads it through the struct. Stick B's ranges and counts are kept but its outputs are
   only cleared. */
#ifndef XJOY_H
#define XJOY_H

#include "xngine.h"
#include "xnstruct.h"
#include "xsysutil.h"

extern u8 xn_joy_axis_mask;
#define xn_joy (*(struct xn_joy_state *)&xn_joy_axis_mask)

#define XN_JOY_OFF      2                   /* joystick_status: disabled */
#define XN_JOY_PORT     0x201

/* The tick handler's asm entry (the vector holds it), and the end of its code */
void asm_xn_joy_timer_isr(void);
extern u8 xn_code_152F41[];

/* init_game_data: once, finds which axis bits of port 201h answer (64 reads after a BIOS
   tick), sets the joystick off (joystick_status 2: the options turn it on), and installs the
   int 1Ch handler, locking its code and the state. The mask is every bit that read 0 all
   64 times: the asm's tests on its low nibble (meant to pair the axes) end in an OR of bits
   the mask already has. */
void xn_joy_init(void);
#pragma aux xn_joy_init parm [] modify exact [eax ecx edx ebx];

/* shutdown_free_all: the int 1Ch vector back (when installed); the joystick off. */
void xn_joy_shutdown(void);
#pragma aux xn_joy_shutdown parm [] modify exact [eax];

/* The ranges back to "nothing seen" (minimums 32000, maximums 0) and the buttons released. */
void xn_joy_reset_range(void);

/* The options' CALIBRATE (when the joystick is on): the ranges reset, then each stick's
   centre the average of its counts over 4 BIOS ticks, rounded. */
void xn_joy_calibrate(void);

/* Every frame: joystick_x and joystick_y from the last counts (each axis: its distance from
   the centre over the range on that side, times -4096, within -4095..4095; 0 inside the dead
   zone), widening the ranges first. Stick B's outputs are cleared only. A range of 0 divides
   by zero: XnGine's divide handler makes that axis 0. */
void xn_joy_poll(void);
#pragma aux xn_joy_poll parm [] modify exact [eax edx ebx];

/* The int 1Ch handler: the buttons cleared, and while the joystick is on, the four axes
   timed (port 201h fired, then counted while each bit stays up, at most 800h reads) and the
   four buttons read (set while pressed). The asm's 4 run-time blocks (152EAD..152F40) are
   this function's. */
void xn_joy_timer_isr(void);

#endif
