/* colstuff.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern int dungeon_water_level;
extern char D_00170710[];
extern char D_0017071B[];
extern unsigned char player_environment;
extern char monster_table_flags[];
extern struct record *creature_list[];
extern char scratch_190be4[];
extern signed char scratch_190ce4[];
extern unsigned char D_001940D7;
extern signed char player_motion_flags;
extern char frame_counter[];
extern struct record *player_object;
extern char D_00195AB4[];
extern int vertical_velocity;
extern struct record *location_object;
extern char cheat_flags[];
extern struct record *found_object;
extern int creature_count;
extern struct character *player_character;
extern struct record *D_00195C48;
extern struct record *D_00195C70;
extern int ceiling_height;
extern struct record *D_00195CB8;
extern int grid_visit_func;
extern int D_00195CD4;
extern int D_00195CD8;
extern int nearest_creature_distance;
extern int ai_monster_flags;
extern struct record *nearest_creature;
extern char click_face_texture[];
extern signed char player_on_ground;
extern signed char in_dungeon_water;
extern signed char D_00196296;
extern signed char D_00196297;
extern signed char D_001962A0;
extern int collide_candidate_count;
extern char D_00196B10[];
extern int D_00196B14;
extern int D_00196B18;
extern char D_00196B1C[];
extern int D_00196B20;
extern int D_00196B24;
extern char D_00196B28[];
extern int D_00196B2C;
extern int D_00196B30;
extern int D_00196B34;
extern int D_00196B38;
extern int D_00196B3C;
extern struct record *collide_candidates[];
extern char D_00196D48[];
extern char D_00196D4C[];
extern int D_00196D50;
extern char D_00196D54[];
extern int D_00196D58;
extern int D_00196D5C;
extern int collide_height;
extern char collide_flags[];

extern int object_find_open(int, int);
extern int door_start_swing(struct record *, int);
extern int mc_memcpy();
extern int memcmp();
extern int xn_math_approx_dist2d();
extern int xn_math_approx_hypot();
extern int xn_collide_segment_model();
extern int xn_collide_spheres_model();
extern int xn_collide_segment_flat_stk();
extern int xn_terrain_height_at();
extern unsigned char ground_tile_at(int, int);
extern void collide_for_each_nearby(struct record *, int);
extern void fatal_error(int);
extern void object_set_position(struct record *, int, int, int, int, int, int);
extern void object_move_by(struct record *, int, int, int, int, int, int);
extern void object_foreach(struct record *, int);
extern void object_foreach_open(struct record *, int);
int collide_line_of_sight_cb(struct record *);
int collide_creature_near(struct record *, struct move_request *);
int collide_creature_within(struct record *, int, int);
void collide_segment_model_cb(struct record *);
void collide_vertical_cb(struct record *);
void func_00022174(struct record *);

int collide_move_missile(struct record *a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_14 = 0;
    *(signed char *)collide_flags &= 252;
    collide_height = a1->y;
    if (((int)player_environment) == 1) {
        if ((collide_height = xn_terrain_height_at(a1->x, a1->z)) <= a1->y) return 2;
    }
    if (collide_creature_within(a1, a2, 100) != 0) {
        *(signed char *)collide_flags |= 8;
        return 8;
    }
    *(int *)D_00196B10 = a1->x;
    D_00196B14 = a1->y;
    D_00196B18 = a1->z;
    mc_memcpy((int)D_00196B28, a2, 12, (int)D_00170710, 61, 4);
    if (a3 != 0) mc_memcpy((int)&D_00196B34, a3, 12, (int)D_00170710, 63, 4);
    collide_for_each_nearby(a1, (int)collide_segment_model_cb);
    if (((int)(short)(*(short *)collide_flags & 10)) == 0) {
        object_set_position(a1, *(int *)D_00196B28, D_00196B2C, D_00196B30, D_00196B34, D_00196B38, D_00196B3C);
    }
    return (int)(short)*(short *)collide_flags;
}

void collide_segment_model_cb(struct record *a1)
{
    int l_34;
    struct block *l_30;
    struct block_model *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    if ((a1->flags & 512) != 0) return;
    switch (a1->type) {
    case 43:
        l_30 = &a1->data.block;
        l_2C = l_30->models;
        for (l_20 = 0; l_30->model_count > l_20; l_20++, l_2C++) {
            if (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
                l_34 = (int)&l_2C->model;
                if (*(int *)((char *)l_34) != 0) {
                    if ((*(int *)D_00196D48 = xn_collide_segment_model(l_34, (int)D_00196B10, (int)D_00196B28, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                        *(int *)click_face_texture = (int)(*(char **)((char *)l_34) + *(int *)(*(char **)D_00196D48 + 16));
                        *(signed char *)collide_flags |= 2;
                        D_00195C48 = a1;
                    }
                }
            }
        }
        return;
    case 56:
        l_2C = (struct block_model *)RECORD_DATA(a1);
        for (l_20 = 0; a1->model_count > l_20; l_20++, l_2C++) {
            if (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
                l_34 = (int)&l_2C->model;
                if (*(int *)((char *)l_34) != 0) {
                    if ((*(int *)D_00196D48 = xn_collide_segment_model(l_34, (int)D_00196B10, (int)D_00196B28, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                        *(int *)click_face_texture = (int)(*(char **)((char *)l_34) + *(int *)(*(char **)D_00196D48 + 16));
                        *(signed char *)collide_flags |= 2;
                        D_00195C48 = a1;
                    }
                }
            }
        }
        return;
    case 6:
    case 32:
        l_34 = (int)RECORD_DATA(a1);
        if (*(int *)((char *)l_34) == 0) return;
        if (((int)(short)(*(short *)collide_flags & 4)) == 0 || ((int)(short)(*(short *)collide_flags & 2)) != 0) {
            return;
        }
        if ((*(int *)D_00196D48 = xn_collide_segment_model(l_34, (int)D_00196B10, (int)D_00196B28, 0)) == 0 || *(int *)D_00196D48 == (-1)) {
            return;
        }
        *(int *)click_face_texture = (int)(*(char **)((char *)l_34) + *(int *)(*(char **)D_00196D48 + 16));
        *(signed char *)collide_flags |= 2;
        D_00195C48 = a1;
    default:;
    }
}

void collide_vertical_cb(struct record *a1)
{
    int l_2C;
    struct block *l_28;
    struct block_model *l_24;
    int l_20;
    int l_1C;
    int l_18;

    if ((a1->flags & 512) != 0 || ((struct bf8_5_1 *)&cheat_flags)->f != 0) return;
    switch (a1->type) {
    case 43:
        l_28 = &a1->data.block;
        l_24 = l_28->models;
        for (l_20 = 0; l_28->model_count > l_20; l_20++, l_24++) {
            l_2C = (int)&l_24->model;
            if (*(int *)((char *)l_2C) != 0) {
                if ((*(int *)D_00196D48 = xn_collide_segment_model(l_2C, (int)D_00196B10, (int)D_00196B1C, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                    for (l_1C = 0; l_1C < *(int *)(*(char **)D_00196D48); l_1C++) {
                        if ((*(int *)scratch_190be4 != 0 && *(int *)(*(char **)D_00196D48 + 8 + (l_1C * 30)) > collide_height) || *(int *)(*(char **)D_00196D48 + 8 + (l_1C * 30)) < collide_height) {
                            collide_height = *(int *)(*(char **)D_00196D48 + 8 + (l_1C * 30));
                            *(int *)D_00195AB4 = (int)(*(char **)((char *)l_2C) + *(int *)(*(char **)D_00196D48 + 16 + (l_1C * 30)));
                            D_00195CD8 = (int)(*(char **)D_00196D48 + 4 + (l_1C * 30));
                            D_00195CB8 = a1;
                        }
                    }
                    *(signed char *)collide_flags |= 1;
                }
            }
        }
        return;
    case 56:
        l_24 = (struct block_model *)RECORD_DATA(a1);
        for (l_20 = 0; a1->model_count > l_20; l_20++, l_24++) {
            l_2C = (int)&l_24->model;
            if (*(int *)((char *)l_2C) != 0) {
                if ((*(int *)D_00196D48 = xn_collide_segment_model(l_2C, (int)D_00196B10, (int)D_00196B28, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                    for (l_1C = 0; l_1C < *(int *)(*(char **)D_00196D48); l_1C++) {
                        if ((*(int *)scratch_190be4 != 0 && *(int *)(*(char **)D_00196D48 + 8 + (l_1C * 30)) > collide_height) || *(int *)(*(char **)D_00196D48 + 8 + (l_1C * 30)) < collide_height) {
                            collide_height = *(int *)(*(char **)D_00196D48 + 8 + (l_1C * 30));
                            *(int *)D_00195AB4 = (int)(*(char **)((char *)l_2C) + *(int *)(*(char **)D_00196D48 + 16 + (l_1C * 30)));
                            D_00195CD8 = (int)(*(char **)D_00196D48 + 4 + (l_1C * 30));
                            D_00195CB8 = a1;
                        }
                    }
                    *(signed char *)collide_flags |= 1;
                }
            }
        }
        return;
    case 6:
    case 32:
        l_2C = (int)RECORD_DATA(a1);
        if (*(int *)((char *)l_2C) == 0) return;
        if ((*(int *)D_00196D48 = xn_collide_segment_model(l_2C, (int)D_00196B10, (int)D_00196B1C, 0)) == 0 || *(int *)D_00196D48 == (-1)) {
            return;
        }
        for (l_1C = 0; l_1C < *(int *)(*(char **)D_00196D48); l_1C++) {
            if ((*(int *)scratch_190be4 != 0 && *(int *)(*(char **)D_00196D48 + 8 + (l_1C * 30)) > collide_height) || *(int *)(*(char **)D_00196D48 + 8 + (l_1C * 30)) < collide_height) {
                collide_height = *(int *)(*(char **)D_00196D48 + 8 + (l_1C * 30));
                *(int *)D_00195AB4 = (int)(*(char **)((char *)l_2C) + *(int *)(*(char **)D_00196D48 + 16 + (l_1C * 30)));
                D_00195CD8 = (int)(*(char **)D_00196D48 + 4 + (l_1C * 30));
                D_00195CB8 = a1;
            }
        }
        *(signed char *)collide_flags |= 1;
    default:;
    }
}

void func_00022174(struct record *a1)
{
    int l_34;
    struct block *l_30;
    struct block_model *l_2C;
    int l_28;
    int l_24;
    struct character *l_20;
    int l_1C;
    int l_18;

    if ((a1->flags & 512) != 0 || ((struct bf8_5_1 *)&cheat_flags)->f != 0) return;
    switch (a1->type) {
    case 43:
        l_30 = &a1->data.block;
        l_2C = l_30->models;
        for (l_28 = 0; l_30->model_count > l_28; l_28++, l_2C++) {
            if (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
                l_34 = (int)&l_2C->model;
                if (*(int *)((char *)l_34) != 0) {
                    if ((*(int *)D_00196D48 = xn_collide_spheres_model(l_34, *(int *)D_00196D4C, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                        D_00196D50 = *(int *)D_00196D48;
                        *(int *)click_face_texture = (int)(*(char **)((char *)l_34) + *(int *)(*(char **)D_00196D48 + 16));
                        D_00195CD4 = *(int *)D_00196D48 + 4;
                        *(signed char *)collide_flags |= 2;
                        D_00195C48 = a1;
                    }
                }
            }
        }
        return;
    case 56:
        l_2C = (struct block_model *)RECORD_DATA(a1);
        for (l_28 = 0; a1->model_count > l_28; l_28++, l_2C++) {
            if (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
                l_34 = (int)&l_2C->model;
                if (*(int *)((char *)l_34) != 0) {
                    if ((*(int *)D_00196D48 = xn_collide_spheres_model(l_34, *(int *)D_00196D4C, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                        D_00196D50 = *(int *)D_00196D48;
                        *(int *)click_face_texture = (int)(*(char **)((char *)l_34) + *(int *)(*(char **)D_00196D48 + 16));
                        D_00195CD4 = *(int *)D_00196D48 + 4;
                        *(signed char *)collide_flags |= 2;
                        D_00195C48 = a1;
                    }
                }
            }
        }
        return;
    case 32:
        if ((a1->flags & 256) != 0) return;
        if (found_object->type != 18) goto L22468;
        l_20 = &found_object->data.character;
        if ((l_20->mobile_id & 128) == 0) {
            if (((int)(unsigned short)(*(short *)(monster_table_flags + (l_20->race * 29)) & 4)) == 0) goto L22468;
        }
        if (a1->lock_level != 0) if ((a1->flags & 64) == 0) goto L22468;
        if (door_start_swing(a1, 0) == 0) goto L22468;
        a1->flags |= 0x100;
        return;
    case 6:
L22468:;
        l_34 = (int)RECORD_DATA(a1);
        if (*(int *)((char *)l_34) != 0) {
            if (a1->move_frame == *(int *)frame_counter || (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0)) {
                if ((*(int *)D_00196D48 = xn_collide_spheres_model(l_34, *(int *)D_00196D4C, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                    D_00196D50 = *(int *)D_00196D48;
                    *(int *)click_face_texture = (int)(*(char **)((char *)l_34) + *(int *)(*(char **)D_00196D48 + 16));
                    D_00195CD4 = *(int *)D_00196D48 + 4;
                    *(signed char *)collide_flags |= 2;
                    D_00195C70 = a1;
                    D_00195C48 = a1;
                }
            }
        }
        return;
    case 18:
        if (found_object == a1) return;
        if ((*(int *)D_00196D48 = xn_collide_segment_flat_stk(&a1->x, (int)D_00196B10, (int)D_00196B28, a1->image, 4, 0, 0)) == 0 || *(int *)D_00196D48 == (-1)) {
            return;
        }
        D_00195C48 = a1;
        *(signed char *)collide_flags |= 8;
    default:;
    }
}

int collide_step_player(struct record *a1, int a2, struct move_request *a3)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_1C = 0;
    D_00196297 = 1;
    ceiling_height = a1->y - 120;
    *(int *)scratch_190be4 = 0;
    *(int *)((char *)(*(int *)D_00196D4C = (int)a3->probe)) = a3->x;
    *(int *)(*(char **)D_00196D4C + 4) = a3->y;
    *(int *)(*(char **)D_00196D4C + 8) = a3->z;
    *(signed char *)collide_flags &= 228;
    found_object = a1;
    D_001962A0 = 0;
    if (((int)player_environment) == 1) {
        collide_height = xn_terrain_height_at(a1->x, a1->z);
        if (a1->y >= collide_height && (signed char)ground_tile_at(a3->x, a3->z) == 0) {
            D_001962A0 = 1;
        }
    } else {
        collide_height = a1->y + 1000;
    }
    mc_memcpy((int)D_00196B28, a3, 12, (int)D_00170710, 474, 4);
    if (((int)(short)(*(short *)collide_flags & 4)) != 0 && memcmp(a3, &a1->x, 12) == 0) {
        *(signed char *)collide_flags &= 251;
    }
    if (vertical_velocity > 5120) {
        *(int *)D_00196B10 = *(int *)D_00196D54;
        D_00196B14 = D_00196D58 - 20;
        D_00196B18 = D_00196D5C;
        *(int *)D_00196B1C = a3->x;
        D_00196B20 = a3->y + 20;
        D_00196B24 = a3->z;
        for (l_14 = 0; l_14 < collide_candidate_count; l_14++) {
            collide_vertical_cb(collide_candidates[l_14]);
        }
    }
    if (vertical_velocity < 0 || ((a1 == player_object && ((player_character->conditions & 0x8) != 0 || in_dungeon_water != 0)) || ((struct bf8_5_1 *)&player_motion_flags)->f != 0)) {
        *(int *)scratch_190be4 = 1;
        *(signed char *)collide_flags &= 254;
        *(int *)D_00196B10 = a3->x;
        D_00196B14 = a3->y - 60;
        D_00196B18 = a3->z;
        *(int *)D_00196B1C = a3->x;
        D_00196B20 = a3->y - 120;
        D_00196B24 = a3->z;
        for (l_14 = 0; l_14 < collide_candidate_count; l_14++) {
            collide_vertical_cb(collide_candidates[l_14]);
        }
        *(int *)scratch_190be4 = 0;
        if (((int)(short)(*(short *)collide_flags & 1)) != 0) {
            ceiling_height = collide_height;
        } else {
            ceiling_height = a1->y - 120;
        }
        *(signed char *)collide_flags &= 254;
        if (((int)player_environment) == 1) {
            collide_height = xn_terrain_height_at(a1->x, a1->z);
        } else {
            collide_height = a1->y + 1000;
        }
    }
    if (((int)(short)(*(short *)collide_flags & 1)) == 0) {
        *(int *)D_00196B10 = a3->x;
        D_00196B14 = a3->y - 25;
        D_00196B18 = a3->z;
        *(int *)D_00196B1C = a3->x;
        if (((struct bf8_3_1 *)&player_motion_flags)->f != 0) {
            D_00196B20 = a3->y + 140;
        } else {
            D_00196B20 = a3->y + 40;
        }
        D_00196B24 = a3->z;
        for (l_14 = 0; l_14 < collide_candidate_count; l_14++) {
            collide_vertical_cb(collide_candidates[l_14]);
        }
    }
    if (collide_height < a3->y) {
        D_00196B14 = (D_00196B2C = collide_height);
        *(int *)(*(char **)D_00196D4C + 4) = D_00196B14;
    }
    for (l_14 = 0; l_14 < collide_candidate_count; l_14++) {
        func_00022174(collide_candidates[l_14]);
    }
    if (collide_creature_near(a1, a3) != 0) *(signed char *)collide_flags |= 8;
    if (((int)(short)(*(short *)collide_flags & 10)) != 0 && ((int)(short)(*(short *)collide_flags & 4)) != 0) {
        if (a1 == player_object && ((struct bf8_5_1 *)&player_motion_flags)->f != 0) {
            if (a3->y < a1->y && (a3->y - 90) > ceiling_height) {
                object_move_by(a1, 0, -(a1->y - a3->y), 0, 0, a1->yaw - a3->yaw, 0);
            } else if (a3->y > a1->y && collide_height > a3->y) {
                object_move_by(a1, 0, -(a1->y - a3->y), 0, 0, a1->yaw - a3->yaw, 0);
            }
        }
        if ((collide_height < a3->y && (((int)(short)(*(short *)collide_flags & 1)) != 0 || ((int)player_environment) == 1)) || ((struct bf8_5_1 *)&D_001940D7)->f != 0) {
            D_001940D7 &= 223;
            object_move_by(a1, 0, collide_height - a1->y, 0, 0, 0, 0);
            l_1C = 1;
        } else if (collide_height != a1->y) {
            if (((struct bf8_5_1 *)&player_motion_flags)->f == 0 && D_00196296 == 0 && (player_character->conditions & 0x8) == 0 && in_dungeon_water == 0 && (collide_height - a1->y) < 30) {
                object_move_by(a1, 0, collide_height - a1->y, 0, 0, 0, 0);
            } else {
                *(signed char *)collide_flags |= 16;
                if (((int)(short)(*(short *)collide_flags & 1)) == 0) {
                    object_move_by(a1, 0, a3->y - a1->y, 0, 0, 0, 0);
                }
                player_on_ground = 0;
            }
        } else if (((int)(short)(*(short *)collide_flags & 16)) != 0) {
            object_set_position(a1, a1->x, a1->y, a1->z, a3->angle_x, a3->yaw, a3->angle_z);
        }
        return (int)(short)*(short *)collide_flags;
    }
    if ((collide_height < a3->y && (((int)(short)(*(short *)collide_flags & 1)) != 0 || ((int)player_environment) == 1)) || ((struct bf8_5_1 *)&D_001940D7)->f != 0) {
        D_001940D7 &= 223;
        object_move_by(a1, 0, collide_height - a1->y, 0, 0, 0, 0);
        l_1C = 1;
    } else if (collide_height >= a1->y && ((int)(short)(*(short *)collide_flags & 8)) == 0) {
        if (((struct bf8_5_1 *)&player_motion_flags)->f == 0 && D_00196296 == 0 && in_dungeon_water == 0 && (player_character->conditions & 0x8) == 0 && (collide_height - a1->y) < 30) {
            object_move_by(a1, 0, collide_height - a1->y, 0, 0, 0, 0);
        } else {
            if ((a3->y - 100) >= ceiling_height) {
                object_move_by(a1, 0, a3->y - a1->y, 0, 0, 0, 0);
            } else if ((a3->y - 100) < ceiling_height && a3->y > a1->y) {
                object_move_by(a1, 0, a3->y - a1->y, 0, 0, 0, 0);
            }
            if (collide_height > a1->y) {
                *(signed char *)collide_flags |= 16;
                player_on_ground = 0;
            }
        }
    }
    if (((int)(short)(*(short *)collide_flags & 10)) == 0 && ((int)(short)(*(short *)collide_flags & 4)) != 0) {
        if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
            object_set_position(a1, *(int *)D_00196B28, a1->y, D_00196B30, a3->angle_x, a3->yaw, a3->angle_z);
        }
    } else if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
        object_set_position(a1, a1->x, a1->y, a1->z, a3->angle_x, a3->yaw, a3->angle_z);
    }
    *(signed char *)collide_flags &= 251;
    if (a1 == player_object) {
        *(int *)D_00196D54 = a1->x;
        D_00196D58 = a1->y;
        D_00196D5C = a1->z;
    }
    return (int)(short)*(short *)collide_flags;
}

void collide_gather_cb(struct record *a1)
{
    int l_30;
    struct block *l_2C;
    struct block_model *l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    if ((a1->flags & 512) != 0) return;
    switch (a1->type) {
    case 43:
        l_2C = &a1->data.block;
        l_28 = l_2C->models;
        for (l_24 = 0; l_2C->model_count > l_24; l_24++, l_28++) {
            l_30 = (int)&l_28->model;
            if (*(int *)((char *)l_30) != 0) {
                if ((*(int *)D_00196D48 = xn_collide_spheres_model(l_30, *(int *)D_00196D4C, 2)) == 0 && *(int *)D_00196D48 != (-1)) {
                    collide_candidates[collide_candidate_count++] = a1;
                    if (collide_candidate_count > 128) fatal_error((int)D_0017071B);
                }
            }
        }
        return;
    case 56:
        l_28 = (struct block_model *)RECORD_DATA(a1);
        for (l_24 = 0; a1->model_count > l_24; l_24++, l_28++) {
            l_30 = (int)&l_28->model;
            if (*(int *)((char *)l_30) != 0) {
                if ((*(int *)D_00196D48 = xn_collide_spheres_model(l_30, *(int *)D_00196D4C, 2)) == 0 && *(int *)D_00196D48 != (-1)) {
                    collide_candidates[collide_candidate_count++] = a1;
                    if (collide_candidate_count > 128) fatal_error((int)D_0017071B);
                }
            }
        }
        return;
    case 32:
        if ((a1->flags & 256) != 0) return;
    case 6:
        l_30 = (int)RECORD_DATA(a1);
        if (*(int *)((char *)l_30) == 0) return;
        if ((*(int *)D_00196D48 = xn_collide_spheres_model(l_30, *(int *)D_00196D4C, 2)) != 0 || *(int *)D_00196D48 == (-1)) {
            return;
        }
        collide_candidates[collide_candidate_count++] = a1;
        if (collide_candidate_count <= 128) return;
        fatal_error((int)D_0017071B);
    default:;
    }
}

int collide_find_floor_object(struct record *a1)
{
    D_00195CB8 = 0;
    collide_for_each_nearby(a1, (int)collide_vertical_cb);
    return (int)D_00195CB8;
}

int collide_line_of_sight_cb(struct record *a1)
{
    int l_30;
    struct block *l_2C;
    struct block_model *l_28;
    int l_24;
    int l_20;
    short l_18;

    if ((a1->flags & 512) != 0) return 0;
    switch (a1->type) {
    case 43:
        l_2C = &a1->data.block;
        l_28 = l_2C->models;
        for (l_24 = 0; l_2C->model_count > l_24; l_24++, l_28++) {
            l_30 = (int)&l_28->model;
            if (*(int *)((char *)l_30) != 0) {
                if ((*(int *)D_00196D48 = xn_collide_segment_model(l_30, (int)D_00196B10, (int)D_00196B1C, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                    scratch_190ce4[0] = 1;
                    return 1;
                }
            }
        }
        break;
    case 56:
        l_28 = (struct block_model *)RECORD_DATA(a1);
        for (l_24 = 0; a1->model_count > l_24; l_24++, l_28++) {
            l_30 = (int)&l_28->model;
            if (*(int *)((char *)l_30) != 0) {
                if ((*(int *)D_00196D48 = xn_collide_segment_model(l_30, (int)D_00196B10, (int)D_00196B1C, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                    scratch_190ce4[0] = 1;
                    return 1;
                }
            }
        }
        break;
    case 32:
        if ((a1->flags & 256) != 0) break;
    case 6:
        l_30 = (int)RECORD_DATA(a1);
        if (*(int *)((char *)l_30) != 0) {
            if ((*(int *)D_00196D48 = xn_collide_segment_model(l_30, (int)D_00196B10, (int)D_00196B1C, 0)) != 0 && *(int *)D_00196D48 != (-1)) {
                scratch_190ce4[0] = 1;
                return 1;
            }
        }
    }
    return 0;
}

int collide_line_of_sight(struct record *a1, struct record *a2)
{
    scratch_190ce4[0] = 0;
    mc_memcpy((int)D_00196B10, (int)&a1->x, 12, (int)D_00170710, 923, 4);
    mc_memcpy((int)D_00196B1C, (int)&a2->x, 12, (int)D_00170710, 924, 4);
    D_00196B14 -= 40;
    D_00196B20 -= 40;
    grid_visit_func = (int)object_find_open;
    collide_for_each_nearby(a1, (int)collide_line_of_sight_cb);
    grid_visit_func = (int)object_foreach_open;
    return (int)(signed char)(scratch_190ce4[0] ^ 1);
}

void creatures_find_nearest(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 10000;
    nearest_creature = 0;
    for (l_1C = 0; l_1C < creature_count; l_1C++) {
        l_18 = xn_math_approx_hypot(player_object->y - creature_list[l_1C]->y, xn_math_approx_dist2d(player_object->x, player_object->z, creature_list[l_1C]->x, creature_list[l_1C]->z));
        if (l_18 < l_20) {
            nearest_creature_distance = l_18;
            nearest_creature = (struct record *)((int)creature_list[l_1C]);
            l_20 = l_18;
        }
    }
}

int collide_creature_near(struct record *a1, struct move_request *a2)
{
    int l_1C;
    int l_18;

    creature_list[creature_count] = player_object;
    for (l_1C = 0; l_1C <= creature_count; l_1C++) {
        if (creature_list[l_1C] == a1) continue;
        l_18 = creature_list[l_1C]->y - a2->y;
        if (l_18 < 0 || l_18 > 100) continue;
        if (xn_math_approx_dist2d(creature_list[l_1C]->x, creature_list[l_1C]->z, a2->x, a2->z) < 50) {
            D_00195C48 = (struct record *)((int)creature_list[l_1C]);
            return 1;
        }
    }
    return 0;
}

int collide_creature_within(struct record *a1, int a2, int a3)
{
    int l_18;
    int l_14;

    creature_list[creature_count] = player_object;
    for (l_18 = 0; l_18 <= creature_count; l_18++) {
        if (creature_list[l_18] == a1) continue;
        l_14 = creature_list[l_18]->y - *(int *)((char *)a2 + 4);
        if (l_14 < 0 || l_14 > 100) continue;
        if (xn_math_approx_dist2d(creature_list[l_18]->x, creature_list[l_18]->z, *(int *)((char *)a2), *(int *)((char *)a2 + 8)) < a3) {
            D_00195C48 = (struct record *)((int)creature_list[l_18]);
            return 1;
        }
    }
    return 0;
}

int func_00023FA5(struct record *a1, int a2, struct move_request *a3)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    struct character *l_14;

    l_20 = 0;
    D_00196297 = 0;
    ceiling_height = a1->y - 120;
    *(int *)scratch_190be4 = 0;
    l_14 = &a1->data.character;
    *(int *)((char *)(*(int *)D_00196D4C = (int)a3->probe)) = a3->x;
    *(int *)(*(char **)D_00196D4C + 4) = a3->y;
    *(int *)(*(char **)D_00196D4C + 8) = a3->z;
    *(signed char *)collide_flags &= 228;
    found_object = a1;
    if (((int)player_environment) == 1) {
        collide_height = xn_terrain_height_at(a1->x, a1->z);
    } else {
        collide_height = a1->y + 1000;
    }
    mc_memcpy((int)D_00196B28, a3, 12, (int)D_00170710, 1021, 4);
    if (((int)(short)(*(short *)collide_flags & 4)) != 0 && memcmp(a3, &a1->x, 12) == 0) {
        *(signed char *)collide_flags &= 251;
    }
    if (l_14->fall_velocity > 5120) {
        *(int *)D_00196B10 = *(int *)D_00196D54;
        D_00196B14 = D_00196D58 - 20;
        D_00196B18 = D_00196D5C;
        *(int *)D_00196B1C = a3->x;
        D_00196B20 = a3->y;
        D_00196B24 = a3->z;
        for (l_18 = 0; l_18 < collide_candidate_count; l_18++) {
            collide_vertical_cb(collide_candidates[l_18]);
        }
    }
    if (l_14->fall_velocity < 0 || ((struct bf8_0_1 *)&ai_monster_flags)->f != 0) {
        *(int *)scratch_190be4 = 1;
        *(int *)D_00196B10 = a1->x;
        D_00196B14 = a1->y - 60;
        D_00196B18 = a1->z;
        *(int *)D_00196B1C = a1->x;
        D_00196B20 = a1->y - 120;
        D_00196B24 = a1->z;
        for (l_18 = 0; l_18 < collide_candidate_count; l_18++) {
            collide_vertical_cb(collide_candidates[l_18]);
        }
        *(int *)scratch_190be4 = 0;
        if (((int)(short)(*(short *)collide_flags & 1)) != 0) {
            ceiling_height = collide_height;
        } else {
            ceiling_height = a1->y - 120;
        }
        if (dungeon_water_level != 10000 && dungeon_water_level > ceiling_height && a3->y > dungeon_water_level) {
            ceiling_height = dungeon_water_level;
        }
        *(signed char *)collide_flags &= 254;
        collide_height = a1->y + 1000;
    }
    if (((int)(short)(*(short *)collide_flags & 1)) == 0) {
        *(int *)D_00196B10 = a1->x;
        D_00196B14 = a1->y - 25;
        D_00196B18 = a1->z;
        *(int *)D_00196B1C = a1->x;
        if (((struct bf8_3_1 *)&player_motion_flags)->f != 0) {
            D_00196B20 = a1->y + 140;
        } else {
            D_00196B20 = a1->y + 40;
        }
        D_00196B24 = a1->z;
        for (l_18 = 0; l_18 < collide_candidate_count; l_18++) {
            collide_vertical_cb(collide_candidates[l_18]);
        }
    }
    if (collide_height < a3->y) {
        D_00196B14 = (D_00196B2C = collide_height);
        *(int *)(*(char **)D_00196D4C + 4) = D_00196B14;
    }
    for (l_18 = 0; l_18 < collide_candidate_count; l_18++) {
        func_00022174(collide_candidates[l_18]);
    }
    if (((int)player_environment) != 2 && collide_creature_near(a1, a3) != 0) {
        *(signed char *)collide_flags |= 8;
    }
    if (((int)(unsigned short)(a3->flags & 1)) == 0 && ((struct bf8_7_1 *)&D_001940D7)->f != 0 && ((struct bf8_0_1 *)&ai_monster_flags)->f == 0) {
        D_001940D7 &= 127;
        if ((collide_height - a3->y) > 60) {
            *(signed char *)collide_flags |= 2;
            return (int)(short)*(short *)collide_flags;
        }
    }
    if (((int)(short)(*(short *)collide_flags & 10)) != 0 && ((int)(short)(*(short *)collide_flags & 4)) != 0) {
        return (int)(short)*(short *)collide_flags;
    }
    if ((collide_height - a3->y) < 30 && ((struct bf8_0_1 *)&ai_monster_flags)->f == 0) {
        object_move_by(a1, 0, collide_height - a1->y, 0, 0, 0, 0);
    }
    if ((collide_height < a3->y && (collide_height - 90) > ceiling_height && (((int)(short)(*(short *)collide_flags & 1)) != 0 || ((int)player_environment) == 1)) || ((struct bf8_5_1 *)&D_001940D7)->f != 0) {
        D_001940D7 &= 223;
        object_move_by(a1, 0, collide_height - a1->y, 0, 0, 0, 0);
        l_20 = 1;
    } else if (collide_height != a1->y) {
        *(signed char *)collide_flags |= 16;
        player_on_ground = 0;
    }
    if (((int)(short)(*(short *)collide_flags & 10)) == 0 && ((int)(short)(*(short *)collide_flags & 4)) != 0) {
        object_set_position(a1, *(int *)D_00196B28, a1->y, D_00196B30, a3->angle_x, a3->yaw, a3->angle_z);
    } else {
        object_set_position(a1, a1->x, a1->y, a1->z, a3->angle_x, a3->yaw, a3->angle_z);
    }
    *(signed char *)collide_flags &= 251;
    return (int)(short)*(short *)collide_flags;
}

int collide_floor_height(struct record *a1)
{
    mc_memcpy((int)D_00196B10, (int)&a1->x, 12, (int)D_00170710, 1143, 4);
    mc_memcpy((int)D_00196B1C, (int)&a1->x, 12, (int)D_00170710, 1144, 4);
    D_00196B14 -= 20;
    D_00196B20 += 40;
    collide_height = 100000;
    object_foreach(location_object->children, (int)collide_vertical_cb);
    return collide_height;
}
