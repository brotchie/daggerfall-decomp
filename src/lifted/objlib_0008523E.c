/* objlib.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char is_daytime;

extern int model_cache_find(int);

int model_get(int id_hundreds, int id_rest, int texture_set)
{
    if (id_hundreds == 4 && (id_rest == 46 || id_rest == 47)) {
        if (is_daytime != 0) {
            id_rest = 46;
        } else {
            id_rest = 47;
        }
    }
    return model_cache_find((id_rest + (id_hundreds * 100)) + (texture_set << 17));
}
