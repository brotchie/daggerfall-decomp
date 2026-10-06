/* book.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int font_text_width(char *);
extern int xn_font_draw_string(int, int, char *);

void text_draw_centred(char *text, int x, short y)
{
    x -= font_text_width(text) >> 1;
    xn_font_draw_string((int)(short)*(short *)&x, (int)(short)y, text);
}
