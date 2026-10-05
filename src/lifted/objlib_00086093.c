/* objlib.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int rand_range(int, int);

short flat_table_pick(int a1)
{
    int l_20;
    int l_1C;

    l_20 = rand_range(0, (int)(short)*(short *)((char *)a1 + 2));
    for (;;) {
        for (l_1C = 2; l_1C < 14; l_1C += 2) {
            if (*(short *)((char *)((l_1C * 2) + a1)) == 0 && *(short *)((char *)((l_1C * 2) + a1) + 2) == 0) {
                break;
            }
            if (((int)(short)*(short *)((char *)((l_1C * 2) + a1))) <= l_20 && ((int)(short)*(short *)((char *)((l_1C * 2) + a1) + 2)) >= l_20) {
                return l_20 + (*(short *)((char *)a1) << 7);
            }
        }
        l_20 = rand_range(0, (int)(short)*(short *)((char *)a1 + 2));
    }
}
