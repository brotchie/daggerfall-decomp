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

void region_enter(unsigned char old_region, unsigned char region)
{
    current_region = region;
    *(int *)current_region_data = ((int)region_event_values) + (((int)(unsigned char)current_region) * 80);
    D_00196269 = current_region;
    region_unload();
    maploads_enter_region((int)(unsigned char)region);
}

int region_update_from_player(void)
{
    unsigned char region;

    region = politic_region_at(player_object->x, player_object->z);
    if ((signed char)region != current_region) {
        region_enter((int)(unsigned char)current_region, (int)(unsigned char)region);
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

unsigned char politic_region_at(int x, int z)
{
    int column;
    int row;
    unsigned char value;

    column = (x >> 15) + 2;
    row = 499 - (z >> 15);
    if (row < 1) {
        row = 1;
    } else if (row > 499) {
        row = 499;
    }
    value = pak_lookup(column, row, politic_pak);
    if (((int)(unsigned char)value) == 64) return 31;
    return value & 127;
}

unsigned char climate_lookup(int x, int z)
{
    int column;
    int row;

    column = (x >> 15) + 2;
    row = 499 - (z >> 15);
    if (row < 1) {
        row = 1;
    } else if (row > 499) {
        row = 499;
    }
    current_climate = pak_lookup(column, row, climate_pak);
    if (((int)(unsigned char)current_climate) == 223) {
        current_climate = 228;
        climate_is_ocean = 1;
        return 3;
    }
    climate_is_ocean = 0;
    return climate_categories[(int)(unsigned char)current_climate];
}

unsigned char pak_lookup(int column, int row, int pak)
{
    int base;
    int run;

    base = pak;
    run = (int)(*(char **)((char *)((row << 2) + base)) + pak);
    column -= (int)(short)*(short *)((char *)run);
    while (column > 0) {
        (*(char (**)[3])&run)++;
        column -= (int)(short)*(short *)((char *)run);
    }
    return *(signed char *)((char *)run + 2);
}

unsigned char ground_tile_at(int x, int z)
{
    signed char *tile;
    int cell;
    int unused;
    int slot;

    cell = xn_world_cell_at(x, z);
    for (slot = 0; slot < 4; slot++) {
        if (cell == xn_world_slot_cells[slot]) break;
    }
    if (slot == 4) return 255;
    tile = (signed char *)(xn_world_tile_layer + D_00187F30[slot]);
    tile += (127 - ((z & 32767) >> 8)) << 8;
    tile += (x & 32767) >> 8;
    return *tile & 63;
}
