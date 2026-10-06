/* custom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"

extern iptr screen_buffer;
extern char D_00175420[];
extern short text_cursor_x;
extern unsigned short text_cursor_y;

extern int inpstr_update(void);
extern int mc_memcpy();
extern int xn_mouse_cursor_erase();
extern int xn_mouse_cursor_draw();
extern void inpstr_begin_text(iptr, short);

void classmaker_input_text(iptr text, int max_length, iptr cancel_cb)
{
    short done;

    xn_mouse_cursor_erase();
    text_cursor_x = 100;
    text_cursor_y = 5;
    inpstr_begin_text(text, (int)(short)*(short *)&max_length);
    *(int *)&done = 0;
    while (done == 0) {
        if (cancel_cb != 0 && ((int (*)())(cancel_cb))(0) != 0) return;
        if (inpstr_update() != 0) *(int *)&done = 1;
        mc_memcpy(655360, screen_buffer, 64000, (iptr)D_00175420, 364, 4);
    }
    xn_mouse_cursor_draw();
}
