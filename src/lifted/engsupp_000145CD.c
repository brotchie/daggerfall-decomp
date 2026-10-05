/* engsupp.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern unsigned char player_environment;
extern char D_0017995C[];
extern short D_00179966[];
extern char D_00179970[];
extern char D_0017998E[];
extern signed char scratch_190ce4[];
extern signed char scratch_190ce5;
extern signed char climate_weathers[];

extern int xn_str_find_u16();

short texture_archive_for_climate(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_18 = a1 % 100;
    switch (player_environment) {
    case 1:
        if (xn_str_find_u16((int)D_00179970, (int)(short)*(short *)&l_18, 15) != 0) {
            if (l_18 == 74 && a2 > 2) return a1;
            a1 = l_18 + (((int)(signed char)scratch_190ce4[0]) * 100);
            if (((int)(unsigned char)climate_weathers[(int)(signed char)scratch_190ce5]) == 5 && l_18 != 74) {
                a1++;
            }
        }
        return a1;
    case 2:
        if (xn_str_find_u16((int)D_0017998E, (int)(short)*(short *)&l_18, 15) != 0) {
            if (l_18 == 74 && a2 > 2) return a1;
            a1 = (a1 % 100) + (((int)(signed char)scratch_190ce4[0]) * 100);
        }
        return a1;
    case 3:
        if (l_18 == 74 && a2 > 2) return a1;
        if (l_18 == 74) return l_18 + ((short)scratch_190ce4[0] * 100);
        l_1C = xn_str_find_u16((int)D_0017995C, (int)(short)*(short *)&a1, 5);
        if (l_1C != 0) {
            a1 = (int)(short)D_00179966[((l_1C - ((int)D_0017995C)) >> 1)];
        } else if (a1 == 168) {
            a1 = (((int)(signed char)scratch_190ce4[0]) * 100) + 68;
        }
        return a1;
    default:
        return a1;
    }
}
