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

short texture_archive_for_climate(int archive, int record)
{
    int found;
    int base;

    base = archive % 100;
    switch (player_environment) {
    case 1:
        if (xn_str_find_u16((int)D_00179970, (int)(short)*(short *)&base, 15) != 0) {
            if (base == 74 && record > 2) return archive;
            archive = base + (((int)(signed char)scratch_190ce4[0]) * 100);
            if (((int)(unsigned char)climate_weathers[(int)(signed char)scratch_190ce5]) == 5 && base != 74) {
                archive++;
            }
        }
        return archive;
    case 2:
        if (xn_str_find_u16((int)D_0017998E, (int)(short)*(short *)&base, 15) != 0) {
            if (base == 74 && record > 2) return archive;
            archive = (archive % 100) + (((int)(signed char)scratch_190ce4[0]) * 100);
        }
        return archive;
    case 3:
        if (base == 74 && record > 2) return archive;
        if (base == 74) return base + ((short)scratch_190ce4[0] * 100);
        found = xn_str_find_u16((int)D_0017995C, (int)(short)*(short *)&archive, 5);
        if (found != 0) {
            archive = (int)(short)D_00179966[((found - ((int)D_0017995C)) >> 1)];
        } else if (archive == 168) {
            archive = (((int)(signed char)scratch_190ce4[0]) * 100) + 68;
        }
        return archive;
    default:
        return archive;
    }
}
