/* custom.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_001854F6[];
extern short D_00185646;
extern short D_0018564A;
extern short D_00185652;
extern short D_00185656;
extern short scratch_190d6a;
extern short chargen_selected_attribute;


void classmaker_select_attribute(short attribute)
{
    chargen_selected_attribute = *(int *)&attribute;
    D_00185646 = (scratch_190d6a = *(short *)(D_001854F6 + ((((int)(short)attribute) + 18) * 12)) + 1);
    D_0018564A = scratch_190d6a + 6;
    D_00185652 = scratch_190d6a + 13;
    D_00185656 = scratch_190d6a + 19;
}
