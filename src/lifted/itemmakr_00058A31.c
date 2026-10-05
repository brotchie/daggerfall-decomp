/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char itemmaker_slots[];
extern char D_001998E2[];

extern void func_00058AF7(void);

int itemmaker_power_excluded(int a1)
{
    int l_1C;

    func_00058AF7();
    l_1C = 0;
L58A4E:;
    if (l_1C < 10) goto L58A61;
    goto L58ADE;
L58A59:;
    l_1C++;
    goto L58A4E;
L58A61:;
    if (a1 != 25) goto L58A79;
    if (((int)(short)*(short *)(itemmaker_slots + (l_1C << 2))) == 25) goto L58A7B;
L58A79:;
    goto L58A8D;
L58A7B:;
    if (((int)(short)*(short *)(D_001998E2 + (l_1C << 2))) == 5) goto L58A8F;
L58A8D:;
    goto L58A9D;
L58A8F:;
    func_00058AF7();
    return 1;
L58A9D:;
    if (a1 != 14) goto L58AB5;
    if (((int)(short)*(short *)(itemmaker_slots + (l_1C << 2))) == 14) goto L58AB7;
L58AB5:;
    goto L58AC9;
L58AB7:;
    if (((int)(short)*(short *)(D_001998E2 + (l_1C << 2))) == 5) goto L58ACB;
L58AC9:;
    goto L58AD9;
L58ACB:;
    func_00058AF7();
    return 1;
L58AD9:;
    goto L58A59;
L58ADE:;
    func_00058AF7();
    return 0;
}
