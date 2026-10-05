/* maplogic.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00176C94[];
extern int D_00196A28;
extern char D_00196A9C[];

extern int func_0008795D(int, int, int);
extern int rand();
extern int mc_memset();
extern void location_load_exterior(int, int);
extern void location_free(int);

void func_00087D71(int a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_1C = *(int *)D_00196A9C;
    l_14 = 0;
    mc_memset(a1, 0, 20, (int)D_00176C94, 952, 4);
    for (l_18 = 0; l_18 < D_00196A28; l_18++, (*(char (**)[17])&l_1C)++) {
        l_14 += func_0008795D(l_1C + 13, a2, a3);
    }
    if (l_14 == 0) {
        location_free(a1);
        return;
    }
    l_1C = *(int *)D_00196A9C;
    l_10 = (rand() % l_14) + 1;
    for (l_18 = 0; l_18 < D_00196A28; l_18++, (*(char (**)[17])&l_1C)++) {
        l_10 -= func_0008795D(l_1C + 13, a2, a3);
        if (l_10 == 0) {
            location_load_exterior(a1, l_18);
            return;
        }
    }
}
