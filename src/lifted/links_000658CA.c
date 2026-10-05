/* links.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00186A34[];
extern char D_00186A4C[];


int func_000658CA(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = a2 + (a1 * 100);
    l_18 = 0;
L658F0:;
    if (l_18 < 6) goto L65900;
    goto L65924;
L658F8:;
    l_18++;
    goto L658F0;
L65900:;
    if (*(int *)(D_00186A34 + (l_18 << 2)) != l_1C) goto L65922;
    return *(int *)(D_00186A4C + (l_18 << 2));
L65922:;
    goto L658F8;
L65924:;
    return 0;
}
