/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char mouse_y[];
extern char D_0012DA44[];
extern char D_00190CEE[];
extern char D_00190D02[];

extern int func_0012DB50();

int itemmaker_row_slot(short a1)
{
    short l_18;

    func_0012DB50(3);
    l_18 = *(short *)mouse_y - 60;
    if (l_18 >= 0) goto L5793A;
    return -1;
L5793A:;
    *(int *)&l_18 = ((int)(short)l_18) / ((int)(short)*(short *)D_0012DA44);
    if (a1 == 0) goto L57966;
    return (int)(signed char)*(signed char *)(D_00190D02 + ((int)(short)l_18));
L57966:;
    return (int)(signed char)*(signed char *)(D_00190CEE + ((int)(short)l_18));
}
