/* mplace.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int dungeon_water_level;
extern char D_00170788[];
extern unsigned char player_environment;
extern signed char D_0017A028[];
extern int encounter_tables[];
extern char monster_table_flags[];
extern char D_00187B6E[];
extern unsigned char D_001940D7;
extern signed char player_motion_flags;
extern signed char dungeon_water_monster_table[];
extern signed char dungeon_monster_table[];
extern signed char D_001951ED[];
extern char frame_counter[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern struct character *player_character;
extern struct settings *game_settings;
extern int D_00195C74;
extern signed char game_mode;
extern signed char player_on_ground;
extern signed char current_climate;
extern signed char D_00196280;
extern signed char D_00196293;
extern signed char D_00196294;
extern signed char D_00196299;
extern signed char D_001962A0;
extern struct map_location *location_here;
extern char collide_flags[];
extern int D_00199808;

extern int climate_category(void);
extern int collide_move_object(struct record *, int, int, int);
extern int spawn_find_point(struct record *, int, int);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int location_here_contains(int, int);
extern int location_contains(int, int);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern int rand();
extern int srand();
extern int func_0009DEAC();
extern int mc_memset();
extern int mc_memcpy();
extern int func_000C7FD9();
extern int func_000CE6E2();
extern int func_0014B45B();
extern void monster_init(struct record *, int);
extern void monster_pacify_check(struct record *);
extern void object_foreach(struct record *, int);
int encounter_pick_monster(int);
int func_00026081(struct record *, int);
void func_0002631D(struct record *);
void func_000266C7(int);
void func_0002675D(int);

int place_spawn_from_marker(struct record *a1)
{
    int l_28;
    int l_24;
    struct character *l_20;
    int l_1C;
    {
        char l_54[12];
        char l_48[28];

        l_28 = (int)(unsigned char)player_on_ground;
        l_24 = D_00195C74;
        if ((a1->flags & 512) != 0) return 0;
        if (func_00026081(a1, 1) == 0) return 0;
        if (*(int *)frame_counter < 5) return 0;
        *(int *)((char *)l_48 + 24) = rand();
        srand((int)(unsigned short)a1->spawn_seed);
        *(int *)((char *)l_48 + 20) = (int)a1;
        l_20 = &(*(struct record **)((char *)l_48 + 20))->data.character;
        if ((((int)(unsigned short)(*(short *)(*(char **)((char *)l_48 + 20) + 27) & 31)) - 2) == 13) {
            l_20->flags |= 64;
        } else {
            D_00196293 = a1->link_flag;
        }
        l_1C = (int)(unsigned short)*(short *)(*(char **)((char *)l_48 + 20) + 19);
        if ((l_1C & 128) == 0 && ((int)(unsigned short)(*(short *)(monster_table_flags + (l_1C * 29)) & 64)) != 0 && (dungeon_water_level == 10000 || (dungeon_water_level - 20) > *(int *)(*(char **)((char *)l_48 + 20) + 11))) {
            return 0;
        }
        if (l_1C == (-1)) {
            srand(*(int *)((char *)l_48 + 24));
            return 0;
        }
        *(signed char *)(*(char **)((char *)l_48 + 20)) = 18;
        *(signed char *)(*(char **)((char *)l_48 + 20) + 21) |= 1;
        monster_init(*(struct record **)((char *)l_48 + 20), l_1C);
        *(int *)(*(char **)((char *)l_48 + 20) + 11) -= 5;
        l_20->team = 1;
        l_20->career_id = *(short *)(*(char **)((char *)l_48 + 20) + 25);
        l_20->target = 0;
        monster_pacify_check(*(struct record **)((char *)l_48 + 20));
        D_00196293 = 0;
        D_001940D7 |= 32;
        D_001940D7 |= 128;
        mc_memcpy((int)l_54, *(int *)((char *)l_48 + 20) + 7, 12, (int)D_00170788, 119, 4);
        mc_memset((int)l_48, 0, 12, (int)D_00170788, 120, 4);
        *(int *)((char *)l_48 + 12) = (int)D_00187B6E;
        player_motion_flags |= 8;
        collide_move_object(*(struct record **)((char *)l_48 + 20), 0, (int)l_54, 0);
        player_motion_flags &= 247;
        player_on_ground = *(signed char *)&l_28;
        D_00195C74 = l_24;
        srand(*(int *)((char *)l_48 + 24));
        if (((int)(short)(*(short *)collide_flags & 1)) == 0) a1->flags |= 16;
        return 1;
    }
}

int encounter_pick_monster(int a1)
{
    struct building *l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_28 = -1;
    l_24 = 0;
L25BFC:;
    while (1) {
        l_24 = 0;
        l_28 = -1;
        if (((int)player_environment) != 3) {
            if ((int)location_here != 0) {
                if (location_here_contains(player_object->x, player_object->z) != 0) {
                    l_28 = ((unsigned)(location_here->x_type_flags << 2)) >> 27;
                    l_24 = 1;
                }
            }
            if (l_24 != 0) {
                l_30 = object_building(player_object->parent);
                if (l_30 != 0) {
                    switch (l_30->type) {
                        goto L25CE0;
                    case 11:
                        l_28 = 40;
                        break;
                    case 14:
                        l_28 = 41;
                        break;
                    case 16:
                    case 17:
                        l_28 = 42;
                        break;
                    case 18:
                        l_28 = 43;
                        break;
                    case 19:
                        l_28 = 44;
                        break;
                    default:
L25CE0:;
                        l_28 = 39;
                    }
                } else {
                    if (D_00199808 != 0) return -1;
                    l_28 = climate_category();
                    if (((int)(unsigned char)current_climate) == 232) l_28 = 232;
                    switch ((unsigned)l_28) {
                        break;
                    case 0:
                        l_28 = 20;
                        break;
                    case 1:
                        l_28 = 23;
                        break;
                    case 2:
                        l_28 = 26;
                        break;
                    case 4:
                        l_28 = 29;
                        break;
                    case 5:
                        l_28 = 32;
                        break;
                    case 232:
                        l_28 = 35;
                    }
                }
            } else {
                l_28 = climate_category();
                if (((int)(unsigned char)current_climate) == 232) l_28 = 232;
                switch ((unsigned)l_28) {
                    break;
                case 0:
                    l_28 = ((D_00199808 != 0) ? 21 : 22);
                    break;
                case 1:
                    l_28 = ((D_00199808 != 0) ? 24 : 25);
                    break;
                case 2:
                    l_28 = ((D_00199808 != 0) ? 27 : 28);
                    break;
                case 4:
                    l_28 = ((D_00199808 != 0) ? 30 : 31);
                    break;
                case 5:
                    l_28 = ((D_00199808 != 0) ? 33 : 34);
                    break;
                case 232:
                    l_28 = ((D_00199808 != 0) ? 36 : 37);
                }
            }
        } else {
            l_28 = current_location->kind;
        }
        if (a1 != 0) l_28 = 19;
        l_2C = encounter_tables[l_28];
        l_28 = rand_range(1, 100);
        if (l_28 <= 80) {
            l_20 = player_character->level - 3;
            l_1C = player_character->level + 3;
        } else if (l_28 <= 95) {
            l_20 = 0;
            l_1C = player_character->level + 1;
        } else if (player_character->level > 5) {
            l_20 = 0;
            l_1C = 19;
        } else {
            l_20 = 0;
            l_1C = player_character->level + 2;
        }
        if (l_20 < 0) {
            l_20 = 0;
            l_1C = 5;
        }
        if (l_1C > 19) {
            l_20 = 14;
            l_1C = 19;
        }
        l_28 = rand_range(l_20, l_1C);
        if ((((int)(unsigned char)*(signed char *)((char *)(l_2C + l_28))) == 29 || ((int)(unsigned char)*(signed char *)((char *)(l_2C + l_28))) == 10 || ((int)(unsigned char)*(signed char *)((char *)(l_2C + l_28))) == 42) && ((int)(unsigned short)(game_settings->view_flags & 4)) != 0) {
            goto L25BFC;
        }
        if (D_00196280 == 0 || ((int)player_environment) != 1 || (((int)(unsigned char)*(signed char *)((char *)(l_2C + l_28))) != 18 && ((int)(unsigned char)*(signed char *)((char *)(l_2C + l_28))) != 23)) {
            break;
        }
    }
    return (int)(unsigned char)*(signed char *)((char *)(l_2C + l_28));
}

int func_00026081(struct record *a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    {
        int l_30;
        int l_2C;

        l_20 = a1->y - player_object->y;
        l_18 = func_0009DEAC(l_20);
        l_1C = func_000C7FD9(a1->x, a1->z, player_object->x, player_object->z);
        if (((int)player_environment) == 1 && l_1C < 4096) return 1;
        if (a2 == 0) {
            if (a1->trigger_range >= 4) {
                if (l_18 <= 768 && l_1C <= 768) {
                    l_2C = 1;
                } else {
                    l_2C = 0;
                }
                return l_2C;
            }
            if (l_18 <= 384 && l_1C <= 1024) {
                l_30 = 1;
            } else {
                l_30 = 0;
            }
            return l_30;
        }
        switch (a1->trigger_range) {
        case 0:
            {
                int l_4C;
                int l_48;
                int l_44;
                int l_40;
                int l_3C;
                int l_38;
                if (l_18 <= 128 && l_1C <= 1024) {
                    l_38 = 1;
                } else {
                    l_38 = 0;
                }
                return l_38;
            case 1:
                if (l_18 <= 128 && l_1C <= 384) {
                    l_3C = 1;
                } else {
                    l_3C = 0;
                }
                return l_3C;
            case 2:
                if (l_18 <= 128 && l_1C <= 640) {
                    l_40 = 1;
                } else {
                    l_40 = 0;
                }
                return l_40;
            case 3:
                if (l_18 <= 384 && l_1C <= 768) {
                    l_44 = 1;
                } else {
                    l_44 = 0;
                }
                return l_44;
            case 4:
                if (l_20 <= 128 && l_20 >= (-768) && l_1C <= 768) {
                    l_48 = 1;
                } else {
                    l_48 = 0;
                }
                return l_48;
            case 5:
                if (l_20 >= (-128) && l_20 <= 768 && l_1C <= 768) {
                    l_4C = 1;
                } else {
                    l_4C = 0;
                }
                return l_4C;
            case 6:
                return (((l_18 <= 256) && (l_1C <= 768)) ? 1 : 0);
            default:
                return 0;
            }
        }
    }
}

void func_0002631D(struct record *a1)
{
    struct record *l_1C;
    short l_18;

    if (a1->type != 34 || ((a1->image & 31) - 2) != 13) return;
    l_1C = a1;
    while (l_1C->type != 47) l_1C = l_1C->parent;
    l_18 = l_1C->water_level;
    if (((int)(short)l_18) < a1->y) {
        a1->mobile_id = (unsigned short)(unsigned char)dungeon_water_monster_table[a1->mobile_id];
        return;
    }
    a1->mobile_id = (unsigned short)(unsigned char)dungeon_monster_table[a1->mobile_id];
}

void dungeon_roll_monster_tables(void)
{
    int l_1C;
    int l_18;

    l_18 = rand();
    srand(((unsigned)D_00195AC4->id) >> 16);
    for (l_1C = 0; l_1C < 256; l_1C++) {
        dungeon_monster_table[l_1C] = encounter_pick_monster(0);
    }
    for (l_1C = 0; l_1C < 256; l_1C++) {
        dungeon_water_monster_table[l_1C] = encounter_pick_monster(1);
    }
    object_foreach(D_00195AC4, (int)func_0002631D);
    srand(l_18);
}

void func_00026508(void)
{
    int l_1C;
    int l_18;

    l_1C = rand_range(2, 5);
    for (l_18 = 0; l_18 < l_1C; l_18++) {
        func_000266C7((int)(unsigned char)D_0017A028[rand() & 3]);
    }
}

void encounter_tick(int a1, int a2)
{
    if (D_001962A0 != 0) return;
    if (a2 == 0) {
        if (D_00196294 != 0) return;
        if ((((unsigned)a1) % 12) != 0) return;
    }
    if (((int)player_environment) == 1) {
        a1 = ((unsigned)a1) % 1440;
        if (location_contains(player_object->x, player_object->z) != 0) {
            if (a2 != 0 || ((unsigned)a1) < 360 || ((unsigned)a1) > 1080) {
                if (a2 == 0 && rand_range(0, 23) != 0) return;
                func_000266C7(encounter_pick_monster(0));
            }
        } else {
            if (a2 == 0) {
                if (((unsigned)a1) < 360 || ((unsigned)a1) > 1080) {
                    if (rand_range(0, 23) != 0) return;
                } else {
                    if (rand_range(0, 35) != 0) return;
                }
            }
            func_0002675D(encounter_pick_monster(0));
        }
        return;
    }
    if (((int)player_environment) != 3) return;
    if (((int)(unsigned char)game_mode) != 16) return;
    if (rand_range(0, 35) != 0) return;
    func_000266C7((int)(unsigned char)D_001951ED[rand() % 6]);
}

void func_000266C7(int a1)
{
    struct record *l_18;

    if (a1 == (-1)) return;
    l_18 = object_create_child(D_00195AC4, 0, 659);
    l_18->type = 18;
    l_18->flags |= 1;
    l_18->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    monster_init(l_18, a1);
    l_18->data.character.team = 1;
    if (spawn_find_point(l_18, 384, 768) == 0) {
        object_delete(l_18);
        return;
    }
    D_00196299 = 1;
}

void func_0002675D(int a1)
{
    struct record *l_18;

    if (a1 == (-1)) return;
    l_18 = object_create_child(D_00195AC4, 0, 659);
    l_18->type = 18;
    l_18->flags |= 1;
    l_18->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    monster_init(l_18, a1);
    l_18->data.character.team = 1;
    func_000CE6E2(player_object->yaw, 1024, &l_18->x, &l_18->z);
    l_18->x += player_object->x;
    l_18->z += player_object->z;
    l_18->y = func_0014B45B(l_18->x, l_18->z);
    D_00196299 = 1;
}
