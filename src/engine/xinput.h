/* xinput.h: the mouse-steering dispatch (src/engine/input.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   intrface_steer splits the view into a 3 x 3 grid of steering regions; this runs the region's
   handler as a tail call (xsysutil.h), as the asm's `jmp [table + region*4]` did. */
#ifndef XINPUT_H
#define XINPUT_H

#include "xngine.h"

/* The game's steering handlers by region (0xCAF00): steer_forward_left, steer_forward,
   steer_forward_right, steer_turn_left, steer_center, steer_turn_right, steer_slide_left,
   steer_backward, steer_slide_right */
extern void (*xn_steer_region_handlers[9])(void);

/* intrface_steer: runs the handler of the steering region 0..8 the mouse is in. */
void xn_input_steer_dispatch(s32 region);

#endif
