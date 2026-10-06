/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "ptrint.h"

extern short mouse_y;
extern short font_height;
extern signed char scratch_190cee[];
extern signed char D_00190D02[];

extern int xn_font_select(int);

int itemmaker_row_slot(short side)
{
    slot16 row;

    xn_font_select(3);
    row = mouse_y - 60;
    if ((short)row < 0) return -1;
    *(int *)&row = ((int)(short)row) / ((int)(short)font_height);
    if (side != 0) return (int)(signed char)D_00190D02[(int)(short)row];
    return (int)(signed char)scratch_190cee[(int)(short)row];
}
