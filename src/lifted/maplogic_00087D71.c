/* maplogic.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char D_00176C94[];
extern int region_location_count;
extern char region_locations[];

extern int location_has_service(int, int, int);
extern int rand();
extern int mc_memset();
extern void location_load_exterior(int, int);
extern void location_free(int);

void location_pick_random_with_service(int a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_1C = *(int *)region_locations;
    l_14 = 0;
    mc_memset(a1, 0, 20, (int)D_00176C94, 952, 4);
    for (l_18 = 0; l_18 < region_location_count; l_18++, (*(char (**)[17])&l_1C)++) {
        l_14 += location_has_service(l_1C + 13, a2, a3);
    }
    if (l_14 == 0) {
        location_free(a1);
        return;
    }
    l_1C = *(int *)region_locations;
    l_10 = (rand() % l_14) + 1;
    for (l_18 = 0; l_18 < region_location_count; l_18++, (*(char (**)[17])&l_1C)++) {
        l_10 -= location_has_service(l_1C + 13, a2, a3);
        if (l_10 == 0) {
            location_load_exterior(a1, l_18);
            return;
        }
    }
}
