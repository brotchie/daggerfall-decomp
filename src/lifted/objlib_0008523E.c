/* objlib.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char D_00196280;

extern int model_cache_find(int);

int model_get(int a1, int a2, int a3)
{
    if (a1 == 4 && (a2 == 46 || a2 == 47)) {
        if (D_00196280 != 0) {
            a2 = 46;
        } else {
            a2 = 47;
        }
    }
    return model_cache_find((a2 + (a1 * 100)) + (a3 << 17));
}
