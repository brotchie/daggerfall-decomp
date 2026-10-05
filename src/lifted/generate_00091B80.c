/* generate.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_0018801E[];
extern char D_00188186[];
extern char D_0018818A[];
extern char D_00188192[];
extern char D_00188196[];
extern char D_00190D6A[];
extern char chargen_selected_attribute[];


void chargen_select_attribute(short a1)
{
    *(short *)chargen_selected_attribute = *(int *)&a1;
    *(short *)D_00188186 = (*(short *)D_00190D6A = *(short *)(D_0018801E + ((((int)(short)a1) + 20) * 12)) + 1);
    *(short *)D_0018818A = *(short *)D_00190D6A + 6;
    *(short *)D_00188192 = *(short *)D_00190D6A + 13;
    *(short *)D_00188196 = *(short *)D_00190D6A + 19;
}
