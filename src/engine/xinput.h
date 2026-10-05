/* xinput.h: the mouse-steering dispatch (src/engine/input.c; see xngine.h). */
#ifndef XINPUT_H
#define XINPUT_H

#include "xngine.h"
#include "xsysutil.h"

/* The game's steering handlers by mouse region (0xCAF00): steer_forward_left, steer_forward,
   steer_forward_right, steer_turn_left, steer_center, steer_turn_right, steer_slide_left,
   steer_backward, steer_slide_right */
extern void (*xn_steer_region_handlers[9])(void);

/* intrface_steer: runs the handler of the steering region the mouse is in. */
void xn_input_steer_dispatch(s32 region);
#pragma aux xn_input_steer_dispatch parm [eax] modify exact [eax ecx edx ebx esi edi];

#endif
