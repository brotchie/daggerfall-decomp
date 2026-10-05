/* objlib.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int rand_range(int, int);

short flat_table_pick(short *table)
{
    int record;
    int i;

    record = rand_range(0, table[1]);
    for (;;) {
        for (i = 2; i < 14; i += 2) {
            if (table[i] == 0 && table[i + 1] == 0) {
                break;
            }
            if (table[i] <= record && table[i + 1] >= record) {
                return record + (table[0] << 7);
            }
        }
        record = rand_range(0, table[1]);
    }
}
