/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern short mouse_y;
extern short font_height;
extern signed char scratch_190cee[];
extern signed char D_00190D02[];

extern int xn_font_select();

int itemmaker_row_slot(short a1)
{
    short l_18;

    xn_font_select(3);
    l_18 = mouse_y - 60;
    if (l_18 < 0) return -1;
    *(int *)&l_18 = ((int)(short)l_18) / ((int)(short)font_height);
    if (a1 != 0) return (int)(signed char)D_00190D02[(int)(short)l_18];
    return (int)(signed char)scratch_190cee[(int)(short)l_18];
}
