/* options.c */

#include "dagger.h"

void options_nop(void) { }

int options_mouse_view_mode(void)
{
    mouse_control_mode = 1;
    return 0;
}
