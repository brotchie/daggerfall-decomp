/* automap.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern int D_00195C44;
extern int D_00196D88;
extern int D_00196D8C;

extern int font_text_width(int);
extern int func_000A0DF4();

int func_000283FD(int a1, int a2)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_2C = 0;
    l_1C = D_00195C44 + 4;
    while (*(short *)((char *)l_1C) != 0) {
        l_18 = l_1C;
        l_24 = ((((int)(unsigned short)*(short *)((char *)l_18)) - D_00196D88) * 2) + 10;
        l_20 = ((((int)(unsigned short)*(short *)((char *)l_18 + 2)) - D_00196D8C) * 2) + 10;
        if (a1 >= l_24 && a2 >= l_20 && (font_text_width(l_1C + 4) + l_24) >= a1 && (l_20 + 6) >= a2) {
            return l_2C + 1;
        }
        l_2C++;
        l_28 = func_000A0DF4(l_1C + 4);
        l_1C += l_28 + 5;
    }
    return 0;
}
