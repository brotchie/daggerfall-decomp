/* generate.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_0018801E[];
extern short D_00188186;
extern short D_0018818A;
extern short D_00188192;
extern short D_00188196;
extern short scratch_190d6a;
extern short chargen_selected_attribute;


void chargen_select_attribute(short attribute)
{
    chargen_selected_attribute = *(int *)&attribute;
    D_00188186 = (scratch_190d6a = *(short *)(D_0018801E + ((((int)(short)attribute) + 20) * 12)) + 1);
    D_0018818A = scratch_190d6a + 6;
    D_00188192 = scratch_190d6a + 13;
    D_00188196 = scratch_190d6a + 19;
}
