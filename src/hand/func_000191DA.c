/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000191DA */
extern int D_0019671C;
extern int factions;
extern int faction_find_type_in_region_r(int, short, short);

int faction_find_type_in_region(short a1, short a2)
{
    int r;

    D_0019671C = 0;
    r = faction_find_type_in_region_r(factions, a1, a2);
    return r != 0 ? r : D_0019671C;
}
