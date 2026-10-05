/* links.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int D_00186A34[];
extern int D_00186A4C[];


int func_000658CA(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = a2 + (a1 * 100);
    for (l_18 = 0; l_18 < 6; l_18++) {
        if (D_00186A34[l_18] == l_1C) return D_00186A4C[l_18];
    }
    return 0;
}
