/* book.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int font_text_width(int);
extern int func_0012DBCC();

void text_draw_centred(int a1, int a2, short a3)
{
    a2 -= font_text_width(a1) >> 1;
    func_0012DBCC((int)(short)*(short *)&a2, (int)(short)a3, a1);
}
