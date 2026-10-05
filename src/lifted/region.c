/* region.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int xn_world_slot_cells[];
extern char *xn_world_tile_layer;
extern char D_001705F8[];
extern signed char climate_categories[];
extern int D_00187F30[];
extern signed char region_event_values[];
extern struct record *player_object;
extern char current_region_data[];
extern signed char current_region;
extern signed char D_00196269;
extern signed char current_climate;
extern signed char climate_is_ocean;
extern int politic_pak;
extern int climate_pak;

extern int mc_free();
extern int xn_world_cell_at();
extern void maploads_enter_region(int);
extern void region_unload(void);
unsigned char politic_region_at(int, int);
unsigned char climate_lookup(int, int);
unsigned char pak_lookup(int, int, int);
void region_enter(unsigned char, unsigned char);

void region_free_tables(void)
{
    if (politic_pak != 0 && politic_pak != (-1751672937)) {
        mc_free(politic_pak, (int)D_001705F8, 33);
        politic_pak = -1751672937;
    }
    if (climate_pak == 0 || climate_pak == (-1751672937)) return;
    mc_free(climate_pak, (int)D_001705F8, 34);
    climate_pak = -1751672937;
}

void region_enter(unsigned char a1, unsigned char a2)
{
    current_region = a2;
    *(int *)current_region_data = ((int)region_event_values) + (((int)(unsigned char)current_region) * 80);
    D_00196269 = current_region;
    region_unload();
    maploads_enter_region((int)(unsigned char)a2);
}

int region_update_from_player(void)
{
    unsigned char l_18;

    l_18 = politic_region_at(player_object->x, player_object->z);
    if ((signed char)l_18 != current_region) {
        region_enter((int)(unsigned char)current_region, (int)(unsigned char)l_18);
        return 1;
    }
    return 0;
}

int climate_category(void)
{
    return (int)(unsigned char)climate_categories[(int)(unsigned char)current_climate];
}

int climate_update_at_player(void)
{
    return (int)(unsigned char)climate_lookup(player_object->x, player_object->z);
}

unsigned char politic_region_at(int a1, int a2)
{
    int l_20;
    int l_1C;
    unsigned char l_18;

    l_20 = (a1 >> 15) + 2;
    l_1C = 499 - (a2 >> 15);
    if (l_1C < 1) {
        l_1C = 1;
    } else if (l_1C > 499) {
        l_1C = 499;
    }
    l_18 = pak_lookup(l_20, l_1C, politic_pak);
    if (((int)(unsigned char)l_18) == 64) return 31;
    return l_18 & 127;
}

unsigned char climate_lookup(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = (a1 >> 15) + 2;
    l_18 = 499 - (a2 >> 15);
    if (l_18 < 1) {
        l_18 = 1;
    } else if (l_18 > 499) {
        l_18 = 499;
    }
    current_climate = pak_lookup(l_1C, l_18, climate_pak);
    if (((int)(unsigned char)current_climate) == 223) {
        current_climate = 228;
        climate_is_ocean = 1;
        return 3;
    }
    climate_is_ocean = 0;
    return climate_categories[(int)(unsigned char)current_climate];
}

unsigned char pak_lookup(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_18 = a3;
    l_14 = (int)(*(char **)((char *)((a2 << 2) + l_18)) + a3);
    a1 -= (int)(short)*(short *)((char *)l_14);
    while (a1 > 0) {
        (*(char (**)[3])&l_14)++;
        a1 -= (int)(short)*(short *)((char *)l_14);
    }
    return *(signed char *)((char *)l_14 + 2);
}

unsigned char ground_tile_at(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_20 = xn_world_cell_at(a1, a2);
    for (l_18 = 0; l_18 < 4; l_18++) {
        if (l_20 == xn_world_slot_cells[l_18]) break;
    }
    if (l_18 == 4) return 255;
    l_24 = (int)(xn_world_tile_layer + D_00187F30[l_18]);
    l_24 += (127 - ((a2 & 32767) >> 8)) << 8;
    l_24 += (a1 & 32767) >> 8;
    return *(signed char *)((char *)l_24) & 63;
}
