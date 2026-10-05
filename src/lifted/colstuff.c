/* colstuff.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern int dungeon_water_level;
extern struct monster_template monster_table[];
extern char D_00170710[];
extern char D_0017071B[];
extern unsigned char player_environment;
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
extern struct collide_hits *D_00196D48;
extern struct collide_probe *D_00196D4C;
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
int collide_creature_within(struct record *, int *, int);
void collide_segment_model_cb(struct record *);
void collide_vertical_cb(struct record *);
void func_00022174(struct record *);

int collide_move_missile(struct record *object, int *position, int *angles)
{
    int unused1;
    int unused2;

    unused2 = 0;
    *(signed char *)collide_flags &= 252;
    collide_height = object->y;
    if (((int)player_environment) == 1) {
        if ((collide_height = xn_terrain_height_at(object->x, object->z)) <= object->y) return 2;
    }
    if (collide_creature_within(object, position, 100) != 0) {
        *(signed char *)collide_flags |= 8;
        return 8;
    }
    *(int *)D_00196B10 = object->x;
    D_00196B14 = object->y;
    D_00196B18 = object->z;
    mc_memcpy((int)D_00196B28, position, 12, (int)D_00170710, 61, 4);
    if (angles != 0) mc_memcpy((int)&D_00196B34, angles, 12, (int)D_00170710, 63, 4);
    collide_for_each_nearby(object, (int)collide_segment_model_cb);
    if (((int)(short)(*(short *)collide_flags & 10)) == 0) {
        object_set_position(object, *(int *)D_00196B28, D_00196B2C, D_00196B30, D_00196B34, D_00196B38, D_00196B3C);
    }
    return (int)(short)*(short *)collide_flags;
}

void collide_segment_model_cb(struct record *object)
{
    char **model;
    struct block *block;
    struct block_model *block_model;
    int unused1;
    int unused2;
    int i;
    int unused3;
    int unused4;

    if ((object->flags & 512) != 0) return;
    switch (object->type) {
    case 43:
        block = &object->data.block;
        block_model = block->models;
        for (i = 0; block->model_count > i; i++, block_model++) {
            if (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
                model = &block_model->model;
                if (*model != 0) {
                    if ((*(int *)&D_00196D48 = xn_collide_segment_model(model, (int)D_00196B10, (int)D_00196B28, 0)) != 0 && (int)D_00196D48 != (-1)) {
                        *(int *)click_face_texture = (int)(*model + D_00196D48->hits[0].face);
                        *(signed char *)collide_flags |= 2;
                        D_00195C48 = object;
                    }
                }
            }
        }
        return;
    case 56:
        block_model = (struct block_model *)RECORD_DATA(object);
        for (i = 0; object->model_count > i; i++, block_model++) {
            if (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
                model = &block_model->model;
                if (*model != 0) {
                    if ((*(int *)&D_00196D48 = xn_collide_segment_model(model, (int)D_00196B10, (int)D_00196B28, 0)) != 0 && (int)D_00196D48 != (-1)) {
                        *(int *)click_face_texture = (int)(*model + D_00196D48->hits[0].face);
                        *(signed char *)collide_flags |= 2;
                        D_00195C48 = object;
                    }
                }
            }
        }
        return;
    case 6:
    case 32:
        model = &object->data.instance.model;
        if (*model == 0) return;
        if (((int)(short)(*(short *)collide_flags & 4)) == 0 || ((int)(short)(*(short *)collide_flags & 2)) != 0) {
            return;
        }
        if ((*(int *)&D_00196D48 = xn_collide_segment_model(model, (int)D_00196B10, (int)D_00196B28, 0)) == 0 || (int)D_00196D48 == (-1)) {
            return;
        }
        *(int *)click_face_texture = (int)(*model + D_00196D48->hits[0].face);
        *(signed char *)collide_flags |= 2;
        D_00195C48 = object;
    default:;
    }
}

void collide_vertical_cb(struct record *object)
{
    char **model;
    struct block *block;
    struct block_model *block_model;
    int i;
    int j;
    int unused;

    if ((object->flags & 512) != 0 || ((struct bf8_5_1 *)&cheat_flags)->f != 0) return;
    switch (object->type) {
    case 43:
        block = &object->data.block;
        block_model = block->models;
        for (i = 0; block->model_count > i; i++, block_model++) {
            model = &block_model->model;
            if (*model != 0) {
                if ((*(int *)&D_00196D48 = xn_collide_segment_model(model, (int)D_00196B10, (int)D_00196B1C, 0)) != 0 && (int)D_00196D48 != (-1)) {
                    for (j = 0; j < D_00196D48->count; j++) {
                        if ((*(int *)scratch_190be4 != 0 && D_00196D48->hits[j].y > collide_height) || D_00196D48->hits[j].y < collide_height) {
                            collide_height = D_00196D48->hits[j].y;
                            *(int *)D_00195AB4 = (int)(*model + D_00196D48->hits[j].face);
                            D_00195CD8 = (int)&D_00196D48->hits[j];
                            D_00195CB8 = object;
                        }
                    }
                    *(signed char *)collide_flags |= 1;
                }
            }
        }
        return;
    case 56:
        block_model = (struct block_model *)RECORD_DATA(object);
        for (i = 0; object->model_count > i; i++, block_model++) {
            model = &block_model->model;
            if (*model != 0) {
                if ((*(int *)&D_00196D48 = xn_collide_segment_model(model, (int)D_00196B10, (int)D_00196B28, 0)) != 0 && (int)D_00196D48 != (-1)) {
                    for (j = 0; j < D_00196D48->count; j++) {
                        if ((*(int *)scratch_190be4 != 0 && D_00196D48->hits[j].y > collide_height) || D_00196D48->hits[j].y < collide_height) {
                            collide_height = D_00196D48->hits[j].y;
                            *(int *)D_00195AB4 = (int)(*model + D_00196D48->hits[j].face);
                            D_00195CD8 = (int)&D_00196D48->hits[j];
                            D_00195CB8 = object;
                        }
                    }
                    *(signed char *)collide_flags |= 1;
                }
            }
        }
        return;
    case 6:
    case 32:
        model = &object->data.instance.model;
        if (*model == 0) return;
        if ((*(int *)&D_00196D48 = xn_collide_segment_model(model, (int)D_00196B10, (int)D_00196B1C, 0)) == 0 || (int)D_00196D48 == (-1)) {
            return;
        }
        for (j = 0; j < D_00196D48->count; j++) {
            if ((*(int *)scratch_190be4 != 0 && D_00196D48->hits[j].y > collide_height) || D_00196D48->hits[j].y < collide_height) {
                collide_height = D_00196D48->hits[j].y;
                *(int *)D_00195AB4 = (int)(*model + D_00196D48->hits[j].face);
                D_00195CD8 = (int)&D_00196D48->hits[j];
                D_00195CB8 = object;
            }
        }
        *(signed char *)collide_flags |= 1;
    default:;
    }
}

void func_00022174(struct record *object)
{
    char **model;
    struct block *block;
    struct block_model *block_model;
    int i;
    int unused1;
    struct character *character;
    int unused2;
    int unused3;

    if ((object->flags & 512) != 0 || ((struct bf8_5_1 *)&cheat_flags)->f != 0) return;
    switch (object->type) {
    case 43:
        block = &object->data.block;
        block_model = block->models;
        for (i = 0; block->model_count > i; i++, block_model++) {
            if (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
                model = &block_model->model;
                if (*model != 0) {
                    if ((*(int *)&D_00196D48 = xn_collide_spheres_model(model, (int)D_00196D4C, 0)) != 0 && (int)D_00196D48 != (-1)) {
                        D_00196D50 = (int)D_00196D48;
                        *(int *)click_face_texture = (int)(*model + D_00196D48->hits[0].face);
                        D_00195CD4 = (int)D_00196D48->hits;
                        *(signed char *)collide_flags |= 2;
                        D_00195C48 = object;
                    }
                }
            }
        }
        return;
    case 56:
        block_model = (struct block_model *)RECORD_DATA(object);
        for (i = 0; object->model_count > i; i++, block_model++) {
            if (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0) {
                model = &block_model->model;
                if (*model != 0) {
                    if ((*(int *)&D_00196D48 = xn_collide_spheres_model(model, (int)D_00196D4C, 0)) != 0 && (int)D_00196D48 != (-1)) {
                        D_00196D50 = (int)D_00196D48;
                        *(int *)click_face_texture = (int)(*model + D_00196D48->hits[0].face);
                        D_00195CD4 = (int)D_00196D48->hits;
                        *(signed char *)collide_flags |= 2;
                        D_00195C48 = object;
                    }
                }
            }
        }
        return;
    case 32:
        if ((object->flags & 256) != 0) return;
        if (found_object->type != 18) goto L22468;
        character = &found_object->data.character;
        if ((character->mobile_id & 128) == 0) {
            if (((int)(unsigned short)(monster_table[character->race].flags & 4)) == 0) goto L22468;
        }
        if (object->lock_level != 0) if ((object->flags & 64) == 0) goto L22468;
        if (door_start_swing(object, 0) == 0) goto L22468;
        object->flags |= 0x100;
        return;
    case 6:
L22468:;
        model = &object->data.instance.model;
        if (*model != 0) {
            if (object->move_frame == *(int *)frame_counter || (((int)(short)(*(short *)collide_flags & 4)) != 0 && ((int)(short)(*(short *)collide_flags & 2)) == 0)) {
                if ((*(int *)&D_00196D48 = xn_collide_spheres_model(model, (int)D_00196D4C, 0)) != 0 && (int)D_00196D48 != (-1)) {
                    D_00196D50 = (int)D_00196D48;
                    *(int *)click_face_texture = (int)(*model + D_00196D48->hits[0].face);
                    D_00195CD4 = (int)D_00196D48->hits;
                    *(signed char *)collide_flags |= 2;
                    D_00195C70 = object;
                    D_00195C48 = object;
                }
            }
        }
        return;
    case 18:
        if (found_object == object) return;
        if ((*(int *)&D_00196D48 = xn_collide_segment_flat_stk(&object->x, (int)D_00196B10, (int)D_00196B28, object->image, 4, 0, 0)) == 0 || (int)D_00196D48 == (-1)) {
            return;
        }
        D_00195C48 = object;
        *(signed char *)collide_flags |= 8;
    default:;
    }
}

int collide_step_player(struct record *object, int unused_arg, struct move_request *request)
{
    int unused2;
    int landed;
    int unused3;
    int i;

    landed = 0;
    D_00196297 = 1;
    ceiling_height = object->y - 120;
    *(int *)scratch_190be4 = 0;
    *(int *)((char *)(*(int *)&D_00196D4C = (int)request->probe)) = request->x;
    D_00196D4C->position.y = request->y;
    D_00196D4C->position.z = request->z;
    *(signed char *)collide_flags &= 228;
    found_object = object;
    D_001962A0 = 0;
    if (((int)player_environment) == 1) {
        collide_height = xn_terrain_height_at(object->x, object->z);
        if (object->y >= collide_height && (signed char)ground_tile_at(request->x, request->z) == 0) {
            D_001962A0 = 1;
        }
    } else {
        collide_height = object->y + 1000;
    }
    mc_memcpy((int)D_00196B28, request, 12, (int)D_00170710, 474, 4);
    if (((int)(short)(*(short *)collide_flags & 4)) != 0 && memcmp(request, &object->x, 12) == 0) {
        *(signed char *)collide_flags &= 251;
    }
    if (vertical_velocity > 5120) {
        *(int *)D_00196B10 = *(int *)D_00196D54;
        D_00196B14 = D_00196D58 - 20;
        D_00196B18 = D_00196D5C;
        *(int *)D_00196B1C = request->x;
        D_00196B20 = request->y + 20;
        D_00196B24 = request->z;
        for (i = 0; i < collide_candidate_count; i++) {
            collide_vertical_cb(collide_candidates[i]);
        }
    }
    if (vertical_velocity < 0 || ((object == player_object && ((player_character->conditions & 0x8) != 0 || in_dungeon_water != 0)) || ((struct bf8_5_1 *)&player_motion_flags)->f != 0)) {
        *(int *)scratch_190be4 = 1;
        *(signed char *)collide_flags &= 254;
        *(int *)D_00196B10 = request->x;
        D_00196B14 = request->y - 60;
        D_00196B18 = request->z;
        *(int *)D_00196B1C = request->x;
        D_00196B20 = request->y - 120;
        D_00196B24 = request->z;
        for (i = 0; i < collide_candidate_count; i++) {
            collide_vertical_cb(collide_candidates[i]);
        }
        *(int *)scratch_190be4 = 0;
        if (((int)(short)(*(short *)collide_flags & 1)) != 0) {
            ceiling_height = collide_height;
        } else {
            ceiling_height = object->y - 120;
        }
        *(signed char *)collide_flags &= 254;
        if (((int)player_environment) == 1) {
            collide_height = xn_terrain_height_at(object->x, object->z);
        } else {
            collide_height = object->y + 1000;
        }
    }
    if (((int)(short)(*(short *)collide_flags & 1)) == 0) {
        *(int *)D_00196B10 = request->x;
        D_00196B14 = request->y - 25;
        D_00196B18 = request->z;
        *(int *)D_00196B1C = request->x;
        if (((struct bf8_3_1 *)&player_motion_flags)->f != 0) {
            D_00196B20 = request->y + 140;
        } else {
            D_00196B20 = request->y + 40;
        }
        D_00196B24 = request->z;
        for (i = 0; i < collide_candidate_count; i++) {
            collide_vertical_cb(collide_candidates[i]);
        }
    }
    if (collide_height < request->y) {
        D_00196B14 = (D_00196B2C = collide_height);
        D_00196D4C->position.y = D_00196B14;
    }
    for (i = 0; i < collide_candidate_count; i++) {
        func_00022174(collide_candidates[i]);
    }
    if (collide_creature_near(object, request) != 0) *(signed char *)collide_flags |= 8;
    if (((int)(short)(*(short *)collide_flags & 10)) != 0 && ((int)(short)(*(short *)collide_flags & 4)) != 0) {
        if (object == player_object && ((struct bf8_5_1 *)&player_motion_flags)->f != 0) {
            if (request->y < object->y && (request->y - 90) > ceiling_height) {
                object_move_by(object, 0, -(object->y - request->y), 0, 0, object->yaw - request->yaw, 0);
            } else if (request->y > object->y && collide_height > request->y) {
                object_move_by(object, 0, -(object->y - request->y), 0, 0, object->yaw - request->yaw, 0);
            }
        }
        if ((collide_height < request->y && (((int)(short)(*(short *)collide_flags & 1)) != 0 || ((int)player_environment) == 1)) || ((struct bf8_5_1 *)&D_001940D7)->f != 0) {
            D_001940D7 &= 223;
            object_move_by(object, 0, collide_height - object->y, 0, 0, 0, 0);
            landed = 1;
        } else if (collide_height != object->y) {
            if (((struct bf8_5_1 *)&player_motion_flags)->f == 0 && D_00196296 == 0 && (player_character->conditions & 0x8) == 0 && in_dungeon_water == 0 && (collide_height - object->y) < 30) {
                object_move_by(object, 0, collide_height - object->y, 0, 0, 0, 0);
            } else {
                *(signed char *)collide_flags |= 16;
                if (((int)(short)(*(short *)collide_flags & 1)) == 0) {
                    object_move_by(object, 0, request->y - object->y, 0, 0, 0, 0);
                }
                player_on_ground = 0;
            }
        } else if (((int)(short)(*(short *)collide_flags & 16)) != 0) {
            object_set_position(object, object->x, object->y, object->z, request->angle_x, request->yaw, request->angle_z);
        }
        return (int)(short)*(short *)collide_flags;
    }
    if ((collide_height < request->y && (((int)(short)(*(short *)collide_flags & 1)) != 0 || ((int)player_environment) == 1)) || ((struct bf8_5_1 *)&D_001940D7)->f != 0) {
        D_001940D7 &= 223;
        object_move_by(object, 0, collide_height - object->y, 0, 0, 0, 0);
        landed = 1;
    } else if (collide_height >= object->y && ((int)(short)(*(short *)collide_flags & 8)) == 0) {
        if (((struct bf8_5_1 *)&player_motion_flags)->f == 0 && D_00196296 == 0 && in_dungeon_water == 0 && (player_character->conditions & 0x8) == 0 && (collide_height - object->y) < 30) {
            object_move_by(object, 0, collide_height - object->y, 0, 0, 0, 0);
        } else {
            if ((request->y - 100) >= ceiling_height) {
                object_move_by(object, 0, request->y - object->y, 0, 0, 0, 0);
            } else if ((request->y - 100) < ceiling_height && request->y > object->y) {
                object_move_by(object, 0, request->y - object->y, 0, 0, 0, 0);
            }
            if (collide_height > object->y) {
                *(signed char *)collide_flags |= 16;
                player_on_ground = 0;
            }
        }
    }
    if (((int)(short)(*(short *)collide_flags & 10)) == 0 && ((int)(short)(*(short *)collide_flags & 4)) != 0) {
        if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
            object_set_position(object, *(int *)D_00196B28, object->y, D_00196B30, request->angle_x, request->yaw, request->angle_z);
        }
    } else if (((struct bf8_5_1 *)&player_motion_flags)->f == 0) {
        object_set_position(object, object->x, object->y, object->z, request->angle_x, request->yaw, request->angle_z);
    }
    *(signed char *)collide_flags &= 251;
    if (object == player_object) {
        *(int *)D_00196D54 = object->x;
        D_00196D58 = object->y;
        D_00196D5C = object->z;
    }
    return (int)(short)*(short *)collide_flags;
}

void collide_gather_cb(struct record *object)
{
    char **model;
    struct block *block;
    struct block_model *block_model;
    int i;
    int unused1;
    int unused2;
    int unused3;

    if ((object->flags & 512) != 0) return;
    switch (object->type) {
    case 43:
        block = &object->data.block;
        block_model = block->models;
        for (i = 0; block->model_count > i; i++, block_model++) {
            model = &block_model->model;
            if (*model != 0) {
                if ((*(int *)&D_00196D48 = xn_collide_spheres_model(model, (int)D_00196D4C, 2)) == 0 && (int)D_00196D48 != (-1)) {
                    collide_candidates[collide_candidate_count++] = object;
                    if (collide_candidate_count > 128) fatal_error((int)D_0017071B);
                }
            }
        }
        return;
    case 56:
        block_model = (struct block_model *)RECORD_DATA(object);
        for (i = 0; object->model_count > i; i++, block_model++) {
            model = &block_model->model;
            if (*model != 0) {
                if ((*(int *)&D_00196D48 = xn_collide_spheres_model(model, (int)D_00196D4C, 2)) == 0 && (int)D_00196D48 != (-1)) {
                    collide_candidates[collide_candidate_count++] = object;
                    if (collide_candidate_count > 128) fatal_error((int)D_0017071B);
                }
            }
        }
        return;
    case 32:
        if ((object->flags & 256) != 0) return;
    case 6:
        model = &object->data.instance.model;
        if (*model == 0) return;
        if ((*(int *)&D_00196D48 = xn_collide_spheres_model(model, (int)D_00196D4C, 2)) != 0 || (int)D_00196D48 == (-1)) {
            return;
        }
        collide_candidates[collide_candidate_count++] = object;
        if (collide_candidate_count <= 128) return;
        fatal_error((int)D_0017071B);
    default:;
    }
}

struct record *collide_find_floor_object(struct record *object)
{
    D_00195CB8 = 0;
    collide_for_each_nearby(object, (int)collide_vertical_cb);
    return D_00195CB8;
}

int collide_line_of_sight_cb(struct record *object)
{
    char **model;
    struct block *block;
    struct block_model *block_model;
    int i;
    int unused1;
    short unused2;

    if ((object->flags & 512) != 0) return 0;
    switch (object->type) {
    case 43:
        block = &object->data.block;
        block_model = block->models;
        for (i = 0; block->model_count > i; i++, block_model++) {
            model = &block_model->model;
            if (*model != 0) {
                if ((*(int *)&D_00196D48 = xn_collide_segment_model(model, (int)D_00196B10, (int)D_00196B1C, 0)) != 0 && (int)D_00196D48 != (-1)) {
                    scratch_190ce4[0] = 1;
                    return 1;
                }
            }
        }
        break;
    case 56:
        block_model = (struct block_model *)RECORD_DATA(object);
        for (i = 0; object->model_count > i; i++, block_model++) {
            model = &block_model->model;
            if (*model != 0) {
                if ((*(int *)&D_00196D48 = xn_collide_segment_model(model, (int)D_00196B10, (int)D_00196B1C, 0)) != 0 && (int)D_00196D48 != (-1)) {
                    scratch_190ce4[0] = 1;
                    return 1;
                }
            }
        }
        break;
    case 32:
        if ((object->flags & 256) != 0) break;
    case 6:
        model = &object->data.instance.model;
        if (*model != 0) {
            if ((*(int *)&D_00196D48 = xn_collide_segment_model(model, (int)D_00196B10, (int)D_00196B1C, 0)) != 0 && (int)D_00196D48 != (-1)) {
                scratch_190ce4[0] = 1;
                return 1;
            }
        }
    }
    return 0;
}

int collide_line_of_sight(struct record *from, struct record *to)
{
    scratch_190ce4[0] = 0;
    mc_memcpy((int)D_00196B10, (int)&from->x, 12, (int)D_00170710, 923, 4);
    mc_memcpy((int)D_00196B1C, (int)&to->x, 12, (int)D_00170710, 924, 4);
    D_00196B14 -= 40;
    D_00196B20 -= 40;
    grid_visit_func = (int)object_find_open;
    collide_for_each_nearby(from, (int)collide_line_of_sight_cb);
    grid_visit_func = (int)object_foreach_open;
    return (int)(signed char)(scratch_190ce4[0] ^ 1);
}

void creatures_find_nearest(void)
{
    int best;
    int i;
    int distance;

    best = 10000;
    nearest_creature = 0;
    for (i = 0; i < creature_count; i++) {
        distance = xn_math_approx_hypot(player_object->y - creature_list[i]->y, xn_math_approx_dist2d(player_object->x, player_object->z, creature_list[i]->x, creature_list[i]->z));
        if (distance < best) {
            nearest_creature_distance = distance;
            nearest_creature = (struct record *)((int)creature_list[i]);
            best = distance;
        }
    }
}

int collide_creature_near(struct record *object, struct move_request *request)
{
    int i;
    int dy;

    creature_list[creature_count] = player_object;
    for (i = 0; i <= creature_count; i++) {
        if (creature_list[i] == object) continue;
        dy = creature_list[i]->y - request->y;
        if (dy < 0 || dy > 100) continue;
        if (xn_math_approx_dist2d(creature_list[i]->x, creature_list[i]->z, request->x, request->z) < 50) {
            D_00195C48 = (struct record *)((int)creature_list[i]);
            return 1;
        }
    }
    return 0;
}

int collide_creature_within(struct record *object, int *position, int radius)
{
    int i;
    int dy;

    creature_list[creature_count] = player_object;
    for (i = 0; i <= creature_count; i++) {
        if (creature_list[i] == object) continue;
        dy = creature_list[i]->y - position[1];
        if (dy < 0 || dy > 100) continue;
        if (xn_math_approx_dist2d(creature_list[i]->x, creature_list[i]->z, position[0], position[2]) < radius) {
            D_00195C48 = (struct record *)((int)creature_list[i]);
            return 1;
        }
    }
    return 0;
}

int func_00023FA5(struct record *object, int unused_arg, struct move_request *request)
{
    int unused2;
    int landed;
    int unused3;
    int i;
    struct character *character;

    landed = 0;
    D_00196297 = 0;
    ceiling_height = object->y - 120;
    *(int *)scratch_190be4 = 0;
    character = &object->data.character;
    *(int *)((char *)(*(int *)&D_00196D4C = (int)request->probe)) = request->x;
    D_00196D4C->position.y = request->y;
    D_00196D4C->position.z = request->z;
    *(signed char *)collide_flags &= 228;
    found_object = object;
    if (((int)player_environment) == 1) {
        collide_height = xn_terrain_height_at(object->x, object->z);
    } else {
        collide_height = object->y + 1000;
    }
    mc_memcpy((int)D_00196B28, request, 12, (int)D_00170710, 1021, 4);
    if (((int)(short)(*(short *)collide_flags & 4)) != 0 && memcmp(request, &object->x, 12) == 0) {
        *(signed char *)collide_flags &= 251;
    }
    if (character->fall_velocity > 5120) {
        *(int *)D_00196B10 = *(int *)D_00196D54;
        D_00196B14 = D_00196D58 - 20;
        D_00196B18 = D_00196D5C;
        *(int *)D_00196B1C = request->x;
        D_00196B20 = request->y;
        D_00196B24 = request->z;
        for (i = 0; i < collide_candidate_count; i++) {
            collide_vertical_cb(collide_candidates[i]);
        }
    }
    if (character->fall_velocity < 0 || ((struct bf8_0_1 *)&ai_monster_flags)->f != 0) {
        *(int *)scratch_190be4 = 1;
        *(int *)D_00196B10 = object->x;
        D_00196B14 = object->y - 60;
        D_00196B18 = object->z;
        *(int *)D_00196B1C = object->x;
        D_00196B20 = object->y - 120;
        D_00196B24 = object->z;
        for (i = 0; i < collide_candidate_count; i++) {
            collide_vertical_cb(collide_candidates[i]);
        }
        *(int *)scratch_190be4 = 0;
        if (((int)(short)(*(short *)collide_flags & 1)) != 0) {
            ceiling_height = collide_height;
        } else {
            ceiling_height = object->y - 120;
        }
        if (dungeon_water_level != 10000 && dungeon_water_level > ceiling_height && request->y > dungeon_water_level) {
            ceiling_height = dungeon_water_level;
        }
        *(signed char *)collide_flags &= 254;
        collide_height = object->y + 1000;
    }
    if (((int)(short)(*(short *)collide_flags & 1)) == 0) {
        *(int *)D_00196B10 = object->x;
        D_00196B14 = object->y - 25;
        D_00196B18 = object->z;
        *(int *)D_00196B1C = object->x;
        if (((struct bf8_3_1 *)&player_motion_flags)->f != 0) {
            D_00196B20 = object->y + 140;
        } else {
            D_00196B20 = object->y + 40;
        }
        D_00196B24 = object->z;
        for (i = 0; i < collide_candidate_count; i++) {
            collide_vertical_cb(collide_candidates[i]);
        }
    }
    if (collide_height < request->y) {
        D_00196B14 = (D_00196B2C = collide_height);
        D_00196D4C->position.y = D_00196B14;
    }
    for (i = 0; i < collide_candidate_count; i++) {
        func_00022174(collide_candidates[i]);
    }
    if (((int)player_environment) != 2 && collide_creature_near(object, request) != 0) {
        *(signed char *)collide_flags |= 8;
    }
    if (((int)(unsigned short)(request->flags & 1)) == 0 && ((struct bf8_7_1 *)&D_001940D7)->f != 0 && ((struct bf8_0_1 *)&ai_monster_flags)->f == 0) {
        D_001940D7 &= 127;
        if ((collide_height - request->y) > 60) {
            *(signed char *)collide_flags |= 2;
            return (int)(short)*(short *)collide_flags;
        }
    }
    if (((int)(short)(*(short *)collide_flags & 10)) != 0 && ((int)(short)(*(short *)collide_flags & 4)) != 0) {
        return (int)(short)*(short *)collide_flags;
    }
    if ((collide_height - request->y) < 30 && ((struct bf8_0_1 *)&ai_monster_flags)->f == 0) {
        object_move_by(object, 0, collide_height - object->y, 0, 0, 0, 0);
    }
    if ((collide_height < request->y && (collide_height - 90) > ceiling_height && (((int)(short)(*(short *)collide_flags & 1)) != 0 || ((int)player_environment) == 1)) || ((struct bf8_5_1 *)&D_001940D7)->f != 0) {
        D_001940D7 &= 223;
        object_move_by(object, 0, collide_height - object->y, 0, 0, 0, 0);
        landed = 1;
    } else if (collide_height != object->y) {
        *(signed char *)collide_flags |= 16;
        player_on_ground = 0;
    }
    if (((int)(short)(*(short *)collide_flags & 10)) == 0 && ((int)(short)(*(short *)collide_flags & 4)) != 0) {
        object_set_position(object, *(int *)D_00196B28, object->y, D_00196B30, request->angle_x, request->yaw, request->angle_z);
    } else {
        object_set_position(object, object->x, object->y, object->z, request->angle_x, request->yaw, request->angle_z);
    }
    *(signed char *)collide_flags &= 251;
    return (int)(short)*(short *)collide_flags;
}

int collide_floor_height(struct record *object)
{
    mc_memcpy((int)D_00196B10, (int)&object->x, 12, (int)D_00170710, 1143, 4);
    mc_memcpy((int)D_00196B1C, (int)&object->x, 12, (int)D_00170710, 1144, 4);
    D_00196B14 -= 20;
    D_00196B20 += 40;
    collide_height = 100000;
    object_foreach(location_object->children, (int)collide_vertical_cb);
    return collide_height;
}
