/* color.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char D_00177350[];
extern unsigned char player_environment;
extern char D_00187B6E[];
extern int D_00187B72;
extern int D_00187B76;
extern signed char D_001886A8[];
extern signed char D_001886A9[];
extern char D_001886D2[];
extern char scratch_190be4[];
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern int game_minutes;
extern int D_001AA5FC;
extern char color_remap_tables[];
extern char doors_moving[];

extern int sound_play(int, int, int);
extern int mc_malloc();
extern int mc_memcpy();
extern int xn_str_fill_ascending();
extern int xn_mem_align_up();
extern int xn_collide_spheres_model();
extern void object_foreach_post(struct record *, int);
int door_blocked_by_player(int);
void building_disable_monster_marker_cb(struct record *);

void color_init_remap_tables(void)
{
    int l_1C;
    int l_18;

    D_001AA5FC = mc_malloc(8448, (int)D_00177350, 59);
    *(int *)color_remap_tables = xn_mem_align_up(D_001AA5FC, 256);
    for (l_1C = 0; l_1C < 32; l_1C++) {
        for (l_18 = 0; l_18 < 256; l_18++) {
            *(signed char *)((char *)(int)(*(char **)color_remap_tables + (l_18 + (l_1C << 8)))) = *(signed char *)&l_18;
        }
    }
    for (l_1C = 1; l_1C < 16; l_1C++) {
        xn_str_fill_ascending((int)(*(char **)color_remap_tables + (l_1C << 8)) + ((int)(unsigned char)D_001886A8[l_1C * 2]), (int)(unsigned char)D_001886A9[l_1C * 2], 16);
    }
    xn_str_fill_ascending(*(int *)color_remap_tables + 6689, 161, 15);
    xn_str_fill_ascending(*(int *)color_remap_tables + 6721, 193, 15);
    xn_str_fill_ascending(*(int *)color_remap_tables + 6945, 97, 15);
    xn_str_fill_ascending(*(int *)color_remap_tables + 6977, 129, 15);
    xn_str_fill_ascending(*(int *)color_remap_tables + 7201, 161, 15);
    xn_str_fill_ascending(*(int *)color_remap_tables + 7220, 84, 2);
    xn_str_fill_ascending(*(int *)color_remap_tables + 7233, 193, 15);
    *(signed char *)(*(char **)color_remap_tables + 7421) = 216;
    xn_str_fill_ascending(*(int *)color_remap_tables + 7457, 97, 15);
    xn_str_fill_ascending(*(int *)color_remap_tables + 7476, 84, 2);
    xn_str_fill_ascending(*(int *)color_remap_tables + 7489, 129, 15);
    *(signed char *)(*(char **)color_remap_tables + 7677) = 216;
    for (l_1C = 0; l_1C < 10; l_1C++) {
        mc_memcpy((int)(*(char **)color_remap_tables + ((l_1C << 8) + 4096)) + 112, ((int)D_001886D2) + (l_1C << 4), 16, (int)D_00177350, 87, 4);
    }
}

int string_hash(int a1)
{
    short l_18;
    short l_1C;

    *(int *)&l_1C = 0;
    *(int *)&l_18 = 0;
    while (*(signed char *)((char *)(((int)(short)l_18) + a1)) != 0) {
        *(int *)&l_1C <<= 1;
        *(int *)&l_1C += (int)(unsigned char)*(signed char *)((char *)(((int)(short)l_18) + a1));
        (*(int *)&l_18)++;
    }
    return *(int *)&l_1C;
}

void doors_update(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    for (l_28 = 0; l_28 < 16; l_28++) {
        if (*(int *)(doors_moving + (l_28 << 2)) == 0) continue;
        l_20 = *(int *)(doors_moving + (l_28 << 2));
        if (((struct bf8_7_1 *)((char *)l_20 + 46))->f == 0 && door_blocked_by_player(l_20) != 0) {
            l_1C = 1132;
            *(int *)((char *)l_20 + 43) = *(int *)((char *)l_1C) | (-1073741824);
        }
        l_18 = 1132;
        l_24 = ((*(int *)((char *)l_18) - (*(int *)((char *)l_20 + 43) & 1073741823)) * 22) & 2047;
        if (l_24 >= 512 || l_24 < 0) {
            if (l_24 >= 512) {
                l_24 = 512;
            } else if (l_24 < 0) {
                l_24 = 0;
            }
            *(int *)(doors_moving + (l_28 << 2)) = 0;
            *(signed char *)((char *)l_20 + 46) &= 191;
            if (((struct bf8_7_1 *)((char *)l_20 + 46))->f == 0) {
                sound_play(((((int)player_environment) == 2) ? 361 : 26), l_20, 100);
            }
        }
        if (((struct bf8_7_1 *)((char *)l_20 + 46))->f == 0) l_24 = 512 - l_24;
        *(short *)((char *)l_20 + 36) = l_24;
    }
}

int door_blocked_by_player(int a1)
{
    int l_20;
    int l_1C;

    *(int *)D_00187B6E = player_object->x;
    D_00187B72 = player_object->y;
    D_00187B76 = player_object->z;
    l_20 = a1 + 71;
    if (*(int *)((char *)l_20) != 0) {
        l_1C = xn_collide_spheres_model(l_20, (int)D_00187B6E, 0);
        return (((l_1C != 0) && (l_1C != (-1))) ? 1 : 0);
    }
    return 0;
}

struct building *object_find_building(struct record *a1)
{
    short l_18;

    a1 = a1->parent;
    while (a1 != 0 && a1 != location_object) {
        *(int *)&l_18 = 0;
        for (; (short)l_18 < current_location->building_count; (*(int *)&l_18)++) {
            if (current_location->buildings[(int)(short)l_18].id == a1->id) {
                return &current_location->buildings[(int)(short)l_18];
            }
        }
    }
    return 0;
}

void building_disable_monster_marker_cb(struct record *a1)
{
    int l_18;

    if (a1->type != 34) return;
    l_18 = ((int)(unsigned short)(a1->image & 31)) - 2;
    if (l_18 != 13) if (l_18 != 14) return;
    a1->flags |= 0x200;
}

void building_disable_monster_markers(struct record *a1)
{
    int l_18;

    l_18 = ((unsigned)game_minutes) % 1440;
    *(int *)scratch_190be4 = (((l_18 > 360) && (l_18 < 1080)) ? 1 : 0);
    object_foreach_post(a1->children, (int)building_disable_monster_marker_cb);
}
