/* mplace.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern int dungeon_water_level;
extern struct monster_template monster_table[];
extern char D_00170788[];
extern unsigned char player_environment;
extern signed char loan_collector_monsters[];
extern iptr encounter_tables[];
extern struct collide_probe D_00187B6E;
extern unsigned char D_001940D7;
extern signed char player_motion_flags;
extern signed char dungeon_water_monster_table[];
extern signed char dungeon_monster_table[];
extern signed char D_001951ED[];
extern char frame_counter[];
extern struct record *player_object;
extern struct record *location_object;
extern struct location *current_location;
extern struct character *player_character;
extern struct settings *game_settings;
extern int ceiling_height;
extern signed char game_mode;
extern signed char player_on_ground;
extern signed char current_climate;
extern signed char is_daytime;
extern signed char D_00196293;
extern signed char D_00196294;
extern signed char D_00196299;
extern signed char D_001962A0;
extern struct map_location *location_here;
extern char collide_flags[];
extern int daylight;

extern int climate_category(void);
extern int collide_move_object(struct record *, int, struct move_request *, int);
extern int spawn_find_point(struct record *, int, int);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int location_here_contains(int, int);
extern int location_contains(int, int);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_new_id(int);
extern int xn_math_approx_dist2d(int, int, int, int);
extern void xn_math_yaw_offset_xz(int, int, int *, int *);
extern int xn_terrain_height_at(int, int);
extern void monster_init(struct record *, int);
extern void monster_pacify_check(struct record *);
extern void object_foreach(struct record *, void (*)());
int encounter_pick_monster(int);
int place_marker_in_range(struct record *, int);
void place_assign_marker_monster(struct record *);
void encounter_spawn_hidden(int);
void encounter_spawn_ahead(int);

int place_spawn_from_marker(struct record *marker)
{
    int saved_on_ground;
    int saved_ceiling;
    struct character *character;
    int monster_id;
    {
        struct move_request request;
        struct record *creature;
        int saved_seed;

        saved_on_ground = (int)(unsigned char)player_on_ground;
        saved_ceiling = ceiling_height;
        if ((marker->flags & 512) != 0) return 0;
        if (place_marker_in_range(marker, 1) == 0) return 0;
        if (*(int *)frame_counter < 5) return 0;
        saved_seed = rand();
        srand((int)(unsigned short)marker->spawn_seed);
        creature = marker;
        character = &creature->data.character;
        if ((((int)(unsigned short)(creature->image & 31)) - 2) == 13) {
            character->flags |= 64;
        } else {
            D_00196293 = marker->link_flag;
        }
        monster_id = creature->mobile_id;
        if ((monster_id & 128) == 0 && ((int)(unsigned short)(monster_table[monster_id].flags & 64)) != 0 && (dungeon_water_level == 10000 || (dungeon_water_level - 20) > creature->y)) {
            return 0;
        }
        if (monster_id == (-1)) {
            srand(saved_seed);
            return 0;
        }
        creature->type = 18;
        creature->flags |= 1;
        monster_init(creature, monster_id);
        creature->y -= 5;
        character->team = 1;
        character->career_id = creature->spawn_seed;
        character->target = 0;
        monster_pacify_check(creature);
        D_00196293 = 0;
        D_001940D7 |= 32;
        D_001940D7 |= 128;
        mc_memcpy(&request, &creature->x, 12, D_00170788, 119, 4);
        mc_memset(&request.angle_x, 0, 12, D_00170788, 120, 4);
        request.probe = &D_00187B6E;
        player_motion_flags |= 8;
        collide_move_object(creature, 0, &request, 0);
        player_motion_flags &= 247;
        player_on_ground = *(signed char *)&saved_on_ground;
        ceiling_height = saved_ceiling;
        srand(saved_seed);
        if (((int)(short)(*(short *)collide_flags & 1)) == 0) marker->flags |= 16;
        return 1;
    }
}

int encounter_pick_monster(int underwater)
{
    struct building *building;
    iptr table;
    int n;
    int in_town;
    int low;
    int high;

    n = -1;
    in_town = 0;
L25BFC:;
    while (1) {
        in_town = 0;
        n = -1;
        if (((int)player_environment) != 3) {
            if ((iptr)location_here != 0) {
                if (location_here_contains(player_object->x, player_object->z) != 0) {
                    n = ((unsigned)(location_here->x_type_flags << 2)) >> 27;
                    in_town = 1;
                }
            }
            if (in_town != 0) {
                building = object_building(player_object->parent);
                if (building != 0) {
                    switch (building->type) {
                    case 11:
                        n = 40;
                        break;
                    case 14:
                        n = 41;
                        break;
                    case 16:
                    case 17:
                        n = 42;
                        break;
                    case 18:
                        n = 43;
                        break;
                    case 19:
                        n = 44;
                        break;
                    default:
                        n = 39;
                    }
                } else {
                    if (daylight != 0) return -1;
                    n = climate_category();
                    if (((int)(unsigned char)current_climate) == 232) n = 232;
                    switch ((unsigned)n) {
                    case 0:
                        n = 20;
                        break;
                    case 1:
                        n = 23;
                        break;
                    case 2:
                        n = 26;
                        break;
                    case 4:
                        n = 29;
                        break;
                    case 5:
                        n = 32;
                        break;
                    case 232:
                        n = 35;
                    }
                }
            } else {
                n = climate_category();
                if (((int)(unsigned char)current_climate) == 232) n = 232;
                switch ((unsigned)n) {
                case 0:
                    n = ((daylight != 0) ? 21 : 22);
                    break;
                case 1:
                    n = ((daylight != 0) ? 24 : 25);
                    break;
                case 2:
                    n = ((daylight != 0) ? 27 : 28);
                    break;
                case 4:
                    n = ((daylight != 0) ? 30 : 31);
                    break;
                case 5:
                    n = ((daylight != 0) ? 33 : 34);
                    break;
                case 232:
                    n = ((daylight != 0) ? 36 : 37);
                }
            }
        } else {
            n = current_location->kind;
        }
        if (underwater != 0) n = 19;
        table = encounter_tables[n];
        n = rand_range(1, 100);
        if (n <= 80) {
            low = player_character->level - 3;
            high = player_character->level + 3;
        } else if (n <= 95) {
            low = 0;
            high = player_character->level + 1;
        } else if (player_character->level > 5) {
            low = 0;
            high = 19;
        } else {
            low = 0;
            high = player_character->level + 2;
        }
        if (low < 0) {
            low = 0;
            high = 5;
        }
        if (high > 19) {
            low = 14;
            high = 19;
        }
        n = rand_range(low, high);
        if ((((int)(unsigned char)*(signed char *)((char *)(table + n))) == 29 || ((int)(unsigned char)*(signed char *)((char *)(table + n))) == 10 || ((int)(unsigned char)*(signed char *)((char *)(table + n))) == 42) && ((int)(unsigned short)(game_settings->view_flags & 4)) != 0) {
            goto L25BFC;
        }
        if (is_daytime == 0 || ((int)player_environment) != 1 || (((int)(unsigned char)*(signed char *)((char *)(table + n))) != 18 && ((int)(unsigned char)*(signed char *)((char *)(table + n))) != 23)) {
            break;
        }
    }
    return (int)(unsigned char)*(signed char *)((char *)(table + n));
}

int place_marker_in_range(struct record *marker, int mode)
{
    int dy;
    int distance;
    int abs_dy;
    {
        int in_range_near;
        int in_range_wide;

        dy = marker->y - player_object->y;
        abs_dy = abs(dy);
        distance = xn_math_approx_dist2d(marker->x, marker->z, player_object->x, player_object->z);
        if (((int)player_environment) == 1 && distance < 4096) return 1;
        if (mode == 0) {
            if (marker->trigger_range >= 4) {
                if (abs_dy <= 768 && distance <= 768) {
                    in_range_wide = 1;
                } else {
                    in_range_wide = 0;
                }
                return in_range_wide;
            }
            if (abs_dy <= 384 && distance <= 1024) {
                in_range_near = 1;
            } else {
                in_range_near = 0;
            }
            return in_range_near;
        }
        {
            int in_range5;
            int in_range4;
            int in_range3;
            int in_range2;
            int in_range1;
            int in_range0;
            switch (marker->trigger_range) {
            case 0:
                if (abs_dy <= 128 && distance <= 1024) {
                    in_range0 = 1;
                } else {
                    in_range0 = 0;
                }
                return in_range0;
            case 1:
                if (abs_dy <= 128 && distance <= 384) {
                    in_range1 = 1;
                } else {
                    in_range1 = 0;
                }
                return in_range1;
            case 2:
                if (abs_dy <= 128 && distance <= 640) {
                    in_range2 = 1;
                } else {
                    in_range2 = 0;
                }
                return in_range2;
            case 3:
                if (abs_dy <= 384 && distance <= 768) {
                    in_range3 = 1;
                } else {
                    in_range3 = 0;
                }
                return in_range3;
            case 4:
                if (dy <= 128 && dy >= (-768) && distance <= 768) {
                    in_range4 = 1;
                } else {
                    in_range4 = 0;
                }
                return in_range4;
            case 5:
                if (dy >= (-128) && dy <= 768 && distance <= 768) {
                    in_range5 = 1;
                } else {
                    in_range5 = 0;
                }
                return in_range5;
            case 6:
                return (((abs_dy <= 256) && (distance <= 768)) ? 1 : 0);
            default:
                return 0;
            }
        }
    }
}

void place_assign_marker_monster(struct record *marker)
{
    struct record *block;
    short water_level;

    if (marker->type != 34 || ((marker->image & 31) - 2) != 13) return;
    block = marker;
    while (block->type != 47) block = block->parent;
    water_level = block->water_level;
    if (((int)(short)water_level) < marker->y) {
        marker->mobile_id = (unsigned short)(unsigned char)dungeon_water_monster_table[marker->mobile_id];
        return;
    }
    marker->mobile_id = (unsigned short)(unsigned char)dungeon_monster_table[marker->mobile_id];
}

void dungeon_roll_monster_tables(void)
{
    int i;
    int saved_seed;

    saved_seed = rand();
    srand(((unsigned)location_object->id) >> 16);
    for (i = 0; i < 256; i++) {
        dungeon_monster_table[i] = encounter_pick_monster(0);
    }
    for (i = 0; i < 256; i++) {
        dungeon_water_monster_table[i] = encounter_pick_monster(1);
    }
    object_foreach(location_object, place_assign_marker_monster);
    srand(saved_seed);
}

void loan_spawn_collectors(void)
{
    int count;
    int i;

    count = rand_range(2, 5);
    for (i = 0; i < count; i++) {
        encounter_spawn_hidden((int)(unsigned char)loan_collector_monsters[rand() & 3]);
    }
}

void encounter_tick(int minutes, int forced)
{
    if (D_001962A0 != 0) return;
    if (forced == 0) {
        if (D_00196294 != 0) return;
        if ((((unsigned)minutes) % 12) != 0) return;
    }
    if (((int)player_environment) == 1) {
        minutes = ((unsigned)minutes) % 1440;
        if (location_contains(player_object->x, player_object->z) != 0) {
            if (forced != 0 || ((unsigned)minutes) < 360 || ((unsigned)minutes) > 1080) {
                if (forced == 0 && rand_range(0, 23) != 0) return;
                encounter_spawn_hidden(encounter_pick_monster(0));
            }
        } else {
            if (forced == 0) {
                if (((unsigned)minutes) < 360 || ((unsigned)minutes) > 1080) {
                    if (rand_range(0, 23) != 0) return;
                } else {
                    if (rand_range(0, 35) != 0) return;
                }
            }
            encounter_spawn_ahead(encounter_pick_monster(0));
        }
        return;
    }
    if (((int)player_environment) != 3) return;
    if (((int)(unsigned char)game_mode) != 16) return;
    if (rand_range(0, 35) != 0) return;
    encounter_spawn_hidden((int)(unsigned char)D_001951ED[rand() % 6]);
}

void encounter_spawn_hidden(int monster_id)
{
    struct record *creature;

    if (monster_id == (-1)) return;
    creature = object_create_child(location_object, 0, 659);
    creature->type = 18;
    creature->flags |= 1;
    creature->id = object_new_id(((unsigned)location_object->id) >> 16);
    monster_init(creature, monster_id);
    creature->data.character.team = 1;
    if (spawn_find_point(creature, 384, 768) == 0) {
        object_delete(creature);
        return;
    }
    D_00196299 = 1;
}

void encounter_spawn_ahead(int monster_id)
{
    struct record *creature;

    if (monster_id == (-1)) return;
    creature = object_create_child(location_object, 0, 659);
    creature->type = 18;
    creature->flags |= 1;
    creature->id = object_new_id(((unsigned)location_object->id) >> 16);
    monster_init(creature, monster_id);
    creature->data.character.team = 1;
    xn_math_yaw_offset_xz(player_object->yaw, 1024, &creature->x, &creature->z);
    creature->x += player_object->x;
    creature->z += player_object->z;
    creature->y = xn_terrain_height_at(creature->x, creature->z);
    D_00196299 = 1;
}
