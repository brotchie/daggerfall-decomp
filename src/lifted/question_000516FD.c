/* question.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"

extern char scratch_190d64[];
extern char *scratch_buffer;

extern void xn_mouse_cursor_erase(void);
extern void xn_mouse_cursor_draw(void);
extern void xn_draw_image_transparent(int, int, int, int, char *);
extern void class_question_scroll_step(iptr *, short);

void class_question_scroll(int *scroll_cels, short delta)
{
    xn_mouse_cursor_erase();
    class_question_scroll_step(scroll_cels, delta);
    xn_draw_image_transparent(0, 135, 320, 48, (scratch_buffer + (((int)(short)*(short *)scratch_190d64) * 320)));
    xn_mouse_cursor_draw();
}
