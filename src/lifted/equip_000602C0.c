/* equip.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */


extern int rand_range(int, int);

int func_000602C0(int a1, int a2)
{
    int l_18;

    switch (a2) {
    case 0:
        if (a1 == 0) {
            l_18 = 3;
        } else if (a1 == 1) {
            l_18 = 7;
        } else {
            l_18 = rand_range(4, 6);
        }
        break;
    case 1:
        if (a1 == 0) {
            l_18 = 8;
        } else {
            l_18 = 9;
        }
        break;
    case 2:
        if (a1 == 0) {
            l_18 = rand_range(10, 11);
        } else if (a1 == 1) {
            l_18 = 16;
        } else {
            l_18 = rand_range(12, 15);
        }
        break;
    case 3:
        if (a1 == 0) {
            l_18 = 17;
        } else if (a1 == 1) {
            l_18 = 21;
        } else {
            l_18 = rand_range(18, 20);
        }
        break;
    case 4:
        if (a1 == 0) {
            l_18 = 22;
        } else if (a1 == 1) {
            l_18 = 26;
        } else {
            l_18 = rand_range(23, 25);
        }
        break;
    case 6:
        if (a1 == 0) {
            l_18 = 0;
        } else {
            l_18 = 1;
        }
        break;
    default:
        l_18 = -1;
    }
    return l_18;
}
