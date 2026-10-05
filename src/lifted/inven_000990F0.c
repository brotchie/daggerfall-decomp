/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int D_00195D84;


int trade_shop_takes_group(int a1)
{
    int l_1C;

    l_1C = 0;
    while (((int)(unsigned char)*(signed char *)((char *)(D_00195D84 + l_1C))) != 255) {
        if (((int)(unsigned char)*(signed char *)((char *)(D_00195D84 + l_1C))) == a1) {
            return 1;
        }
        l_1C += 2;
    }
    return 0;
}
