/* spells.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */



void spellmaker_adjust_value(unsigned char *value, int delta, int limit, short pair_offset)
{
    *value += *(signed char *)&delta;
    if (*value < 1) *value = 1;
    if ((short)*value > *(short *)&limit) {
        *value = *(signed char *)&limit;
    }
    if (pair_offset < 0 && value[pair_offset] > *value) {
        value[pair_offset] = *value;
    }
    if (pair_offset <= 0 || value[pair_offset] >= *value) {
        return;
    }
    value[pair_offset] = *value;
}
