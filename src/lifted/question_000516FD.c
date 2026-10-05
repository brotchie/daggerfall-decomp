/* question.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char scratch_190d64[];
extern char scratch_buffer[];

extern int xn_mouse_cursor_erase();
extern int xn_mouse_cursor_draw();
extern int xn_draw_image_transparent();
extern void class_question_scroll_step(int, short);

void class_question_scroll(int a1, short a2)
{
    xn_mouse_cursor_erase();
    class_question_scroll_step(a1, (int)(short)a2);
    xn_draw_image_transparent(0, 135, 320, 48, (int)(*(char **)scratch_buffer + (((int)(short)*(short *)scratch_190d64) * 320)));
    xn_mouse_cursor_draw();
}
