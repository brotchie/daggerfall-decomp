/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int D_00195D84;


int trade_shop_takes_group(int group)
{
    int i;

    i = 0;
    while (((int)(unsigned char)*(signed char *)((char *)(D_00195D84 + i))) != 255) {
        if (((int)(unsigned char)*(signed char *)((char *)(D_00195D84 + i))) == group) {
            return 1;
        }
        i += 2;
    }
    return 0;
}
