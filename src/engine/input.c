/* input.c: the mouse-steering dispatch (canonical C; documented in xinput.h). */
#include "xinput.h"
#include "xsysutil.h"

void xn_input_steer_dispatch(s32 region)
{
    xn_tail_jump(xn_steer_region_handlers[region]);     /* (a tail call: xsysutil.h) */
}
