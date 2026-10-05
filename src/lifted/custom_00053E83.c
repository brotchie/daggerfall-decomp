/* custom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001854F6[];
extern char D_00185646[];
extern char D_0018564A[];
extern char D_00185652[];
extern char D_00185656[];
extern char D_00190D6A[];
extern char chargen_selected_attribute[];


void classmaker_select_attribute(short a1)
{
    *(short *)chargen_selected_attribute = *(int *)&a1;
    *(short *)D_00185646 = (*(short *)D_00190D6A = *(short *)(D_001854F6 + ((((int)(short)a1) + 18) * 12)) + 1);
    *(short *)D_0018564A = *(short *)D_00190D6A + 6;
    *(short *)D_00185652 = *(short *)D_00190D6A + 13;
    *(short *)D_00185656 = *(short *)D_00190D6A + 19;
}
