/* input.c: the mouse-steering dispatch as readable C (xinput.h; see xngine.h). */
#include "xinput.h"

void xn_input_steer_dispatch(s32 region)
{
    xn_tail_jump(xn_steer_region_handlers[region]);   /* as the asm: jmp (see xsysutil.h) */
}
