/* color.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern char D_00177350[];
extern unsigned char player_environment;
extern struct collide_probe D_00187B6E;
extern signed char D_001886A8[];
extern signed char D_001886A9[];
extern char D_001886D2[];
extern char scratch_190be4[];
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern int game_minutes;
extern iptr D_001AA5FC;
extern unsigned char *color_remap_tables;    /* 32 tables of 256 */
extern struct record *doors_moving[];

extern int sound_play(int, struct record *, int);
extern iptr mc_malloc();
extern int mc_memcpy();
extern int xn_str_fill_ascending();
extern iptr xn_mem_align_up();
extern int xn_collide_spheres_model();
extern void object_foreach_post(struct record *, void (*)());
int door_blocked_by_player(struct record *);
void building_disable_monster_marker_cb(struct record *);

void color_init_remap_tables(void)
{
    int table;
    int colour;

    D_001AA5FC = mc_malloc(8448, (iptr)D_00177350, 59);
    color_remap_tables = (unsigned char *)xn_mem_align_up(D_001AA5FC, 256);
    for (table = 0; table < 32; table++) {
        for (colour = 0; colour < 256; colour++) {
            color_remap_tables[colour + (table << 8)] = *(signed char *)&colour;
        }
    }
    for (table = 1; table < 16; table++) {
        xn_str_fill_ascending((int)(iptr)(color_remap_tables + (table << 8)) + ((int)(unsigned char)D_001886A8[table * 2]), (int)(unsigned char)D_001886A9[table * 2], 16);
    }
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 6689, 161, 15);
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 6721, 193, 15);
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 6945, 97, 15);
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 6977, 129, 15);
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 7201, 161, 15);
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 7220, 84, 2);
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 7233, 193, 15);
    color_remap_tables[7421] = 216;
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 7457, 97, 15);
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 7476, 84, 2);
    xn_str_fill_ascending((int)(iptr)color_remap_tables + 7489, 129, 15);
    color_remap_tables[7677] = 216;
    for (table = 0; table < 10; table++) {
        mc_memcpy((int)(iptr)(color_remap_tables + ((table << 8) + 4096)) + 112, ((iptr)D_001886D2) + (table << 4), 16, (iptr)D_00177350, 87, 4);
    }
}

int string_hash(char *text)
{
    short i;
    short hash;

    *(int *)&hash = 0;
    *(int *)&i = 0;
    while (text[i] != 0) {
        *(int *)&hash <<= 1;
        *(int *)&hash += (unsigned char)text[i];
        (*(int *)&i)++;
    }
    return *(int *)&hash;
}

void doors_update(void)
{
    int i;
    int angle;
    struct record *door;
    int *bios_ticks;
    int *ticks;

    for (i = 0; i < 16; i++) {
        if (doors_moving[i] == 0) continue;
        door = doors_moving[i];
        if ((door->door_swing & 0x80000000) == 0 && door_blocked_by_player(door) != 0) {
            bios_ticks = (int *)1132;
            door->door_swing = *bios_ticks | (-1073741824);
        }
        ticks = (int *)1132;
        angle = ((*ticks - (door->door_swing & 1073741823)) * 22) & 2047;
        if (angle >= 512 || angle < 0) {
            if (angle >= 512) {
                angle = 512;
            } else if (angle < 0) {
                angle = 0;
            }
            doors_moving[i] = 0;
            door->door_swing &= ~0x40000000;
            if ((door->door_swing & 0x80000000) == 0) {
                sound_play(((((int)player_environment) == 2) ? 361 : 26), door, 100);
            }
        }
        if ((door->door_swing & 0x80000000) == 0) angle = 512 - angle;
        door->door_angle = angle;
    }
}

int door_blocked_by_player(struct record *door)
{
    int *model;
    int hit;

    D_00187B6E.position.x = player_object->x;
    D_00187B6E.position.y = player_object->y;
    D_00187B6E.position.z = player_object->z;
    model = (int *)RECORD_DATA(door);
    if (*model != 0) {
        hit = xn_collide_spheres_model(model, (iptr)&D_00187B6E, 0);
        return (((hit != 0) && (hit != (-1))) ? 1 : 0);
    }
    return 0;
}

struct building *object_find_building(struct record *object)
{
    short i;

    object = object->parent;
    while (object != 0 && object != location_object) {
        *(int *)&i = 0;
        for (; i < current_location->building_count; (*(int *)&i)++) {
            if (current_location->buildings[i].id == object->id) {
                return &current_location->buildings[i];
            }
        }
    }
    return 0;
}

void building_disable_monster_marker_cb(struct record *object)
{
    int marker_kind;

    if (object->type != 34) return;
    marker_kind = ((int)(unsigned short)(object->image & 31)) - 2;
    if (marker_kind != 13) if (marker_kind != 14) return;
    object->flags |= 0x200;
}

void building_disable_monster_markers(struct record *building)
{
    int minute_of_day;

    minute_of_day = ((unsigned)game_minutes) % 1440;
    *(int *)scratch_190be4 = (((minute_of_day > 360) && (minute_of_day < 1080)) ? 1 : 0);
    object_foreach_post(building->children, building_disable_monster_marker_cb);
}
