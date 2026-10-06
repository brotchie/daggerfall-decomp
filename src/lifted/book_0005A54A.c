/* book.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int xn_font_draw_string(int, int, char *);

void text_draw(char *text, short x, short y)
{
    xn_font_draw_string((int)(short)x, (int)(short)y, text);
}
