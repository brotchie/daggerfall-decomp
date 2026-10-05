/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00195D84[];


int func_000990F0(int a1)
{
    int l_1C;

    l_1C = 0;
L99108:;
    if (((int)(unsigned char)*(signed char *)((char *)(*(int *)D_00195D84 + l_1C))) == 255) goto L99141;
    if (((int)(unsigned char)*(signed char *)((char *)(*(int *)D_00195D84 + l_1C))) != a1) goto L9913B;
    return 1;
L9913B:;
    l_1C += 2;
    goto L99108;
L99141:;
    return 0;
}
