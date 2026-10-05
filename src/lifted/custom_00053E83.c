/* custom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001854F6[];
extern short D_00185646;
extern short D_0018564A;
extern short D_00185652;
extern short D_00185656;
extern short D_00190D6A;
extern short chargen_selected_attribute;


void classmaker_select_attribute(short a1)
{
    chargen_selected_attribute = *(int *)&a1;
    D_00185646 = (D_00190D6A = *(short *)(D_001854F6 + ((((int)(short)a1) + 18) * 12)) + 1);
    D_0018564A = D_00190D6A + 6;
    D_00185652 = D_00190D6A + 13;
    D_00185656 = D_00190D6A + 19;
}
