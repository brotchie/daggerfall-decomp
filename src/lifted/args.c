/* args.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern int D_000C5404;
extern int xn_sin_table[];
extern int xn_cos_table[];
extern int cfg_helmet;
extern char D_00176A88[];
extern char D_00176A8F[];
extern char D_00176A91[];
extern char D_00176A97[];
extern char D_00176A9C[];
extern char D_00176A9E[];
extern char D_00176AA5[];
extern char D_00176AAE[];
extern char D_00176AB8[];
extern char D_00176AC0[];
extern char D_00176AC8[];
extern char D_00176ACC[];
extern char D_00176AD5[];
extern char D_00176ADE[];
extern char D_00176AE6[];
extern char D_00176AED[];
extern char D_00176AF2[];
extern char D_00176AF9[];
extern char D_00176B00[];
extern char D_00176B05[];
extern char D_00176B0C[];
extern char D_00176B12[];
extern char D_00176B1B[];
extern char D_00176B22[];
extern char D_00176B27[];
extern char D_00176B2E[];
extern char D_00176B33[];
extern char D_00176B3A[];
extern char D_00176B45[];
extern char D_00176B53[];
extern char D_00176B5F[];
extern char D_00176B69[];
extern char D_00176B82[];
extern char D_00176B9F[];
extern char D_00176BC1[];
extern char D_00176BED[];
extern int controls_file;
extern unsigned char player_environment;
extern int cfg_texture_memory;
extern int mem_check_level;
extern char scratch_190be4[];
extern int scratch_190be8;
extern int scratch_190bec;
extern signed char D_001917E3[];
extern char arena2_path[];
extern signed char D_00191833[];
extern char arena2_cd_path[];
extern char cfg_last_path[];
extern signed char player_motion_flags;
extern struct record *player_object;
extern struct record *location_object;
extern char cheat_flags[];
extern struct location *current_location;
extern struct character *player_character;
extern signed char cfg_map_file;
extern char cfg_item_file[];
extern char classmaker_file[];
extern int spell_cast_queue_count;
extern int free_later_count;
extern int cfg_magic_repair;
extern signed char climate_weathers[];
extern signed char D_001961AE;
extern signed char cfg_user;
extern signed char cfg_fade_colour;
extern signed char current_climate;
extern signed char cfg_artifact;
extern signed char cfg_fpu;
extern signed char cfg_ps2fix;
extern signed char cfg_stereo;
extern int rmb_origin_z;
extern int rmb_origin_y;
extern int rmb_origin_x;
extern char rmb_origin_yaw[];
extern struct block *rmb_record_ptr;
extern struct record *D_00196DB0;
extern char cfg_block_str[];
extern char cfg_mapsave_file[];
extern int cfg_seed;
extern int cfg_start_map;
extern short cfg_facloop;
extern short cfg_faction;
extern signed char cfg_show_markers;
extern signed char cfg_debug;
extern signed char cheat_mode;
extern signed char cfg_gender;
extern signed char cfg_flags;
extern signed char cfg_region;
extern signed char D_001A949C;
extern signed char model_cache_flush_count;
extern int object_heap_size;

extern int climate_category(void);
extern int weapon_arrow_update(struct record *);
extern int object_draw_cb(struct record *);
extern struct record *rmb_make_door(struct record *, short, short, int);
extern struct record *rmb_make_flat(struct record *, short, short, int);
extern int model_get(int, int, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int func_0009DA1C(int, int);
extern int printf(int, ...);
extern int exit();
extern int abs();
extern int mc_strncpy();
extern int atoi();
extern int strlen();
extern int stricmp();
extern int mc_set_location(int, int);
extern int func_000A1054();
extern int func_000A148C(int, ...);
extern int fopen();
extern int fprintf_2(int, ...);
extern int fclose();
extern int xn_model_set_angles();
extern int xn_model_compose_angles();
extern int xn_model_set_angles_yaw_offset();
extern int xn_math_approx_dist2d();
extern int xn_math_fixmul28();
extern int xn_light_add();
extern int xn_model_submit();
extern int xn_flat_add();
extern void sky_apply_sunlight(void);
extern void weapon_missile_orient(struct record *);
extern void object_free_pending(void);
extern void spell_cast_queued_run(void);
extern void func_0007E815(struct record *, void (*)());
extern void town_grid_visit_near(struct record *, void (*)());
extern void player_light_draw(void);
extern void object_foreach_open(struct record *, void (*)());
void rotate_xz(int *, int *, int);
#pragma aux func_0009DA1C parm routine [];
#pragma aux mc_set_location parm routine [];

void config_read(char *path)
{
    int file;
    unsigned char missing;
    {
        char key[32];
        char value[32];

        missing = 0;
        func_0009DA1C(62, (int)D_00176A88);
        file = fopen(path, (int)D_00176A8F);
        if (file != 0) {
            while (fprintf_2(file, (int)D_00176A91, (int)key, (int)value) != (-1)) {
                if (stricmp((int)key, (int)D_00176A97) == 0) {
                    mc_strncpy((int)arena2_path, (int)value, 80, (int)D_00176A88, 70);
                    if (((int)(unsigned char)D_001917E3[strlen((int)arena2_path)]) != 92) {
                        func_000A1054((int)arena2_path, (int)D_00176A9C, (int)D_00176A88, 72, 80);
                    }
                    mc_strncpy((int)cfg_last_path, (int)value, 80, (int)D_00176A88, 73);
                    cfg_flags |= 1;
                } else if (stricmp((int)key, (int)D_00176A9E) == 0) {
                    mc_strncpy((int)arena2_cd_path, (int)value, 80, (int)D_00176A88, 78);
                    if (((int)(unsigned char)D_00191833[strlen((int)arena2_cd_path)]) != 92) {
                        func_000A1054((int)arena2_cd_path, (int)D_00176A9C, (int)D_00176A88, 80, 80);
                    }
                    mc_strncpy((int)cfg_last_path, (int)value, 80, (int)D_00176A88, 81);
                    cfg_flags |= 1;
                } else if (stricmp((int)key, (int)D_00176AA5) == 0) {
                    mc_strncpy(controls_file, (int)value, 4, (int)D_00176A88, 86);
                } else if (stricmp((int)key, (int)D_00176AAE) == 0) {
                    cfg_fade_colour = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176AB8) == 0) {
                    mc_strncpy((int)&cfg_map_file, (int)value, 12, (int)D_00176A88, 94);
                    cfg_flags |= 4;
                } else if (stricmp((int)key, (int)D_00176AC0) == 0) {
                    cfg_faction = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176AC8) == 0) {
                    cfg_fpu = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176ACC) == 0) {
                    cfg_start_map = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176AD5) == 0) {
                    cfg_artifact = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176ADE) == 0) {
                    cfg_facloop = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176AE6) == 0) {
                    mc_strncpy((int)classmaker_file, (int)value, 20, (int)D_00176A88, 119);
                } else if (stricmp((int)key, (int)D_00176AED) == 0) {
                    mc_strncpy((int)cfg_item_file, (int)value, 20, (int)D_00176A88, 123);
                } else if (stricmp((int)key, (int)D_00176AF2) == 0) {
                    cfg_gender = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176AF9) == 0) {
                    cfg_ps2fix = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B00) == 0) {
                    cfg_user = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B05) == 0) {
                    cfg_region = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B0C) == 0) {
                    cfg_debug = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B12) == 0) {
                    mc_strncpy((int)cfg_block_str, (int)value, 80, (int)D_00176A88, 147);
                } else if (stricmp((int)key, (int)D_00176B1B) == 0) {
                    cfg_helmet = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B22) == 0) {
                    mc_strncpy((int)cfg_mapsave_file, (int)value, 80, (int)D_00176A88, 155);
                } else if (stricmp((int)key, (int)D_00176B27) == 0) {
                    cfg_show_markers = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B2E) == 0) {
                    cfg_seed = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B33) == 0) {
                    cfg_stereo = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B3A) == 0) {
                    object_heap_size = atoi((int)value) << 10;
                } else if (stricmp((int)key, (int)D_00176B45) == 0) {
                    cfg_texture_memory = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B53) == 0) {
                    cfg_magic_repair = atoi((int)value);
                } else if (stricmp((int)key, (int)D_00176B5F) == 0) {
                    cheat_mode = atoi((int)value);
                }
            }
            func_0009DA1C(196, (int)D_00176A88);
            fclose(file);
        }
        if (((int)(unsigned char)(cfg_flags & 1)) == 0) {
            func_0009DA1C(201, (int)D_00176A88);
            printf((int)D_00176B69);
            missing = 1;
        }
        if (((int)(unsigned char)(cfg_flags & 4)) == 0) {
            func_0009DA1C(207, (int)D_00176A88);
            printf((int)D_00176B82);
            missing = 1;
        } else if (((int)(unsigned char)cfg_map_file) == 100) {
            player_environment = 3;
        }
        if (missing != 0) {
            func_0009DA1C(216, (int)D_00176A88);
            printf((int)D_00176B9F);
            exit(5);
        }
        if (mem_check_level != 0) {
            mc_set_location(221, (int)D_00176A88);
            func_000A148C((int)D_00176BC1);
            return;
        }
        mc_set_location(223, (int)D_00176A88);
        func_000A148C((int)D_00176BED);
    }
}

void world_draw_objects(void)
{
    int unused;
    int unused2;
    int unused3;
    struct record *next;
    struct record *block;
    struct record *children;
    unsigned short flags;

    D_001A949C = climate_weathers[climate_category()];
    model_cache_flush_count = 0;
    free_later_count = 0;
    spell_cast_queue_count = 0;
    if (D_001961AE != 0) object_draw_cb((struct record *)&D_001961AE);
    if (((int)player_environment) < 3) {
        if (((struct bf8_0_1 *)&player_motion_flags)->f == 0) {
            if ((player_character->conditions & 0x10) != 0) {
                player_light_draw();
            } else {
                xn_light_add(player_object->x, player_object->y - 60, player_object->z, 16, 128, 0);
            }
        }
        if (player_object->parent->type != 1) {
            object_foreach_open(player_object->parent->children, object_draw_cb);
        } else {
            town_grid_visit_near(player_object, object_draw_cb);
        }
        block = location_object->children;
        while (block != 0) {
            next = block->next;
            children = block->children;
            flags = block->flags;
            if (block->type != 38) {
                object_draw_cb(block);
                if (((int)(unsigned short)(*(int *)&flags & 1)) == 0) {
                    object_foreach_open(children, object_draw_cb);
                }
            }
            block = next;
        }
        if (((int)player_environment) == 1) sky_apply_sunlight();
    } else {
        if (((struct bf8_0_1 *)&player_motion_flags)->f == 0) {
            if ((player_character->conditions & 0x10) != 0) {
                player_light_draw();
            } else {
                xn_light_add(player_object->x, player_object->y - 60, player_object->z, 16, 128, 0);
            }
        }
        func_0007E815(player_object, object_draw_cb);
        block = location_object->children;
        while (block != 0) {
            next = block->next;
            children = block->children;
            flags = block->flags;
            if (block->type != 47) {
                object_draw_cb(block);
                if (((int)(unsigned short)(*(int *)&flags & 1)) == 0) {
                    object_foreach_open(children, object_draw_cb);
                }
            }
            block = next;
        }
    }
    spell_cast_queued_run();
    object_free_pending();
}

void rotate_xz(int *px, int *pz, int yaw)
{
    int x;
    int z;

    x = *px;
    z = *pz;
    yaw &= 2047;
    *px = xn_math_fixmul28(x, xn_cos_table[yaw]) - xn_math_fixmul28(z, xn_sin_table[yaw]);
    *pz = xn_math_fixmul28(z, xn_cos_table[yaw]) + xn_math_fixmul28(x, xn_sin_table[yaw]);
}

int automap_draw_object_cb(struct record *object)
{
    struct model_instance *instance;
    struct block *block;
    struct block_flat *flats;
    struct block_model *model;
    int unused;
    int unused2;
    int i;
    int unused3;
    int unused4;
    short ticks_addr;

    if (model_cache_flush_count != 0) return 1;
    if ((object->flags & 512) != 0) return 0;
    if ((object->flags & 1024) != 0) return 0;
    if (((int)player_environment) != 2) {
        if (object->type == 34 && ((object->image & 31) - 2) == 8) {
        } else {
            if ((object->flags & 128) == 0 && ((struct bf8_4_1 *)&cheat_flags)->f == 0) return 0;
            if (object->type != 6 && object->type != 32) return 0;
            *(int *)&ticks_addr = 1132;
            if ((int)D_00196DB0 == (int)object && ((struct bf8_3_1 *)(*(char **)&ticks_addr))->f == 0) {
                return 0;
            }
        }
    }
    if (abs(object->y - scratch_190be8) > 700) return 0;
    if (xn_math_approx_dist2d(object->x, object->z, *(int *)scratch_190be4, scratch_190bec) > 2048) return 0;
    object->draw_handle = 0;
    D_000C5404 = 0;
    switch (object->type) {
    case 34:
        if (cfg_show_markers == 0) break;
        if (object->image == 0 || object->image == 65535) break;
        object->draw_handle = xn_flat_add(object->x, object->y, object->z, object->image, -1, 4, 4129024);
        break;
    case 43:
        block = &object->data.block;
        model = block->models;
        flats = block->flats;
        for (i = 0; block->model_count > i; i++, model++) {
            if (abs(model->y - scratch_190be8) > 100) continue;
            model->model = (char *)model_get(model->id, model->variant, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)D_001A949C));
            if (model->model != 0) xn_model_submit(&model->model, 0);
        }
    case 6:
    case 32:
        if (object->image2 == 0) break;
        instance = &object->data.instance;
        instance->model = (char *)model_get(object->image2, object->image, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)D_001A949C));
        if (instance->model != 0) {
            instance->x = object->x;
            instance->y = object->y;
            instance->z = object->z;
            if (object->image2 == 998) {
                if (weapon_arrow_update(object) == 0) break;
                weapon_missile_orient(object);
                instance->missile_angles[0] = object->missile_yaw;
                instance->missile_angles[1] = object->angle_z;
                instance->missile_angles[2] = 0;
            } else if (object->link_flag != 255) {
                xn_model_set_angles_yaw_offset(instance->angles, object->wait_state);
            } else {
                if (object->image2 == 610 && object->image == 32) D_000C5404 = 3;
                xn_model_compose_angles(instance->angles, object->angle_x, object->yaw + object->wait_state, object->angle_z);
            }
            xn_model_submit(instance, 0);
        }
        break;
    case 56:
        model = (struct block_model *)RECORD_DATA(object);
        for (i = 0; object->model_count > i; i++, model++) {
            if (abs(model->y - scratch_190be8) > 100) continue;
            model->model = (char *)model_get(model->id, model->variant, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)D_001A949C));
            if (model->model != 0) xn_model_submit(&model->model, 0);
        }
    }
    return 0;
}

struct record *rmb_make_light(struct record *parent, int image, int radius)
{
    struct record *light;

    light = object_create_child(parent, 0, 0);
    light->type = 7;
    light->image = image;
    light->light_radius = radius;
    light->pad13 = 8000;
    light->id = location_object->id + ((int)(unsigned short)(current_location->object_counter)++);
    return light;
}

void rmb_add_doors(struct record *parent, struct block_door *door)
{
    struct record *object;
    int i;

    if (rmb_record_ptr->door_count == 0) return;
    for (i = 0; rmb_record_ptr->door_count > i; i++, door++) {
        object = rmb_make_door(parent, door->image2, (int)(short)((unsigned short)door->image), 1);
        xn_model_set_angles(0, (door->yaw + *(int *)rmb_origin_yaw) % 2048, 0, (int)object->data.instance.angles);
        object->lock_level = (unsigned short)door->lock_level;
        rotate_xz(&door->x, &door->z, *(int *)rmb_origin_yaw);
        door->x += rmb_origin_x;
        object->x = door->x;
        door->z += rmb_origin_z;
        object->z = door->z;
        door->y += rmb_origin_y;
        object->y = door->y;
    }
}

void rmb_add_people(struct record *parent, struct block_flat *person)
{
    struct record *object;
    int i;

    if (rmb_record_ptr->people_count == 0) return;
    for (i = 0; rmb_record_ptr->people_count > i; i++, person++) {
        object = rmb_make_flat(parent, (int)(short)person->image, (int)(short)person->faction_id, 0);
        rotate_xz(&person->x, &person->z, *(int *)rmb_origin_yaw);
        person->x += rmb_origin_x;
        object->x = person->x;
        person->z += rmb_origin_z;
        object->z = person->z;
        person->y += rmb_origin_y;
        object->y = person->y;
        if (((int)(unsigned char)(person->flags & 4)) != 0) object->data.person.flags |= 8;
        if (((int)(unsigned char)(person->flags & 8)) != 0) object->data.person.flags |= 32;
        if (((int)(unsigned char)(person->flags & 32)) != 0) object->data.person.flags |= 16;
    }
}
