/* parse.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char D_001841E3[];
extern signed char D_00190D1F;

extern int name_generate_seeded(unsigned char, unsigned char, int);

int parse_regional_name(int seed, int gender)
{
    return name_generate_seeded((int)(unsigned char)D_001841E3[(int)(signed char)D_00190D1F], (int)(unsigned char)*(signed char *)&gender, seed);
}
