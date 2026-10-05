/* args.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
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
extern char rmb_record_ptr[];
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
extern int model_get(unsigned short, int, int);
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
extern void func_0007E815(struct record *, int);
extern void town_grid_visit_near(struct record *, int);
extern void player_light_draw(void);
extern void object_foreach_open(struct record *, int);
void rotate_xz(int, int, int);
#pragma aux func_0009DA1C parm routine [];
#pragma aux mc_set_location parm routine [];

void config_read(int a1)
{
    int l_1C;
    unsigned char l_18;
    {
        char l_60[32];
        char l_40[32];

        l_18 = 0;
        func_0009DA1C(62, (int)D_00176A88);
        l_1C = fopen(a1, (int)D_00176A8F);
        if (l_1C != 0) {
            while (fprintf_2(l_1C, (int)D_00176A91, (int)l_60, (int)l_40) != (-1)) {
                if (stricmp((int)l_60, (int)D_00176A97) == 0) {
                    mc_strncpy((int)arena2_path, (int)l_40, 80, (int)D_00176A88, 70);
                    if (((int)(unsigned char)D_001917E3[strlen((int)arena2_path)]) != 92) {
                        func_000A1054((int)arena2_path, (int)D_00176A9C, (int)D_00176A88, 72, 80);
                    }
                    mc_strncpy((int)cfg_last_path, (int)l_40, 80, (int)D_00176A88, 73);
                    cfg_flags |= 1;
                } else if (stricmp((int)l_60, (int)D_00176A9E) == 0) {
                    mc_strncpy((int)arena2_cd_path, (int)l_40, 80, (int)D_00176A88, 78);
                    if (((int)(unsigned char)D_00191833[strlen((int)arena2_cd_path)]) != 92) {
                        func_000A1054((int)arena2_cd_path, (int)D_00176A9C, (int)D_00176A88, 80, 80);
                    }
                    mc_strncpy((int)cfg_last_path, (int)l_40, 80, (int)D_00176A88, 81);
                    cfg_flags |= 1;
                } else if (stricmp((int)l_60, (int)D_00176AA5) == 0) {
                    mc_strncpy(controls_file, (int)l_40, 4, (int)D_00176A88, 86);
                } else if (stricmp((int)l_60, (int)D_00176AAE) == 0) {
                    cfg_fade_colour = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176AB8) == 0) {
                    mc_strncpy((int)&cfg_map_file, (int)l_40, 12, (int)D_00176A88, 94);
                    cfg_flags |= 4;
                } else if (stricmp((int)l_60, (int)D_00176AC0) == 0) {
                    cfg_faction = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176AC8) == 0) {
                    cfg_fpu = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176ACC) == 0) {
                    cfg_start_map = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176AD5) == 0) {
                    cfg_artifact = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176ADE) == 0) {
                    cfg_facloop = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176AE6) == 0) {
                    mc_strncpy((int)classmaker_file, (int)l_40, 20, (int)D_00176A88, 119);
                } else if (stricmp((int)l_60, (int)D_00176AED) == 0) {
                    mc_strncpy((int)cfg_item_file, (int)l_40, 20, (int)D_00176A88, 123);
                } else if (stricmp((int)l_60, (int)D_00176AF2) == 0) {
                    cfg_gender = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176AF9) == 0) {
                    cfg_ps2fix = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B00) == 0) {
                    cfg_user = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B05) == 0) {
                    cfg_region = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B0C) == 0) {
                    cfg_debug = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B12) == 0) {
                    mc_strncpy((int)cfg_block_str, (int)l_40, 80, (int)D_00176A88, 147);
                } else if (stricmp((int)l_60, (int)D_00176B1B) == 0) {
                    cfg_helmet = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B22) == 0) {
                    mc_strncpy((int)cfg_mapsave_file, (int)l_40, 80, (int)D_00176A88, 155);
                } else if (stricmp((int)l_60, (int)D_00176B27) == 0) {
                    cfg_show_markers = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B2E) == 0) {
                    cfg_seed = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B33) == 0) {
                    cfg_stereo = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B3A) == 0) {
                    object_heap_size = atoi((int)l_40) << 10;
                } else if (stricmp((int)l_60, (int)D_00176B45) == 0) {
                    cfg_texture_memory = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B53) == 0) {
                    cfg_magic_repair = atoi((int)l_40);
                } else if (stricmp((int)l_60, (int)D_00176B5F) == 0) {
                    cheat_mode = atoi((int)l_40);
                }
            }
            func_0009DA1C(196, (int)D_00176A88);
            fclose(l_1C);
        }
        if (((int)(unsigned char)(cfg_flags & 1)) == 0) {
            func_0009DA1C(201, (int)D_00176A88);
            printf((int)D_00176B69);
            l_18 = 1;
        }
        if (((int)(unsigned char)(cfg_flags & 4)) == 0) {
            func_0009DA1C(207, (int)D_00176A88);
            printf((int)D_00176B82);
            l_18 = 1;
        } else if (((int)(unsigned char)cfg_map_file) == 100) {
            player_environment = 3;
        }
        if (l_18 != 0) {
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
    int l_30;
    int l_2C;
    int l_28;
    struct record *l_24;
    struct record *l_20;
    struct record *l_1C;
    unsigned short l_18;

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
            object_foreach_open(player_object->parent->children, (int)object_draw_cb);
        } else {
            town_grid_visit_near(player_object, (int)object_draw_cb);
        }
        l_20 = location_object->children;
        while (l_20 != 0) {
            l_24 = l_20->next;
            l_1C = l_20->children;
            l_18 = l_20->flags;
            if (l_20->type != 38) {
                object_draw_cb(l_20);
                if (((int)(unsigned short)(*(int *)&l_18 & 1)) == 0) {
                    object_foreach_open(l_1C, (int)object_draw_cb);
                }
            }
            l_20 = l_24;
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
        func_0007E815(player_object, (int)object_draw_cb);
        l_20 = location_object->children;
        while (l_20 != 0) {
            l_24 = l_20->next;
            l_1C = l_20->children;
            l_18 = l_20->flags;
            if (l_20->type != 47) {
                object_draw_cb(l_20);
                if (((int)(unsigned short)(*(int *)&l_18 & 1)) == 0) {
                    object_foreach_open(l_1C, (int)object_draw_cb);
                }
            }
            l_20 = l_24;
        }
    }
    spell_cast_queued_run();
    object_free_pending();
}

void rotate_xz(int a1, int a2, int a3)
{
    int l_14;
    int l_10;

    l_14 = *(int *)((char *)a1);
    l_10 = *(int *)((char *)a2);
    a3 &= 2047;
    *(int *)((char *)a1) = xn_math_fixmul28(l_14, xn_cos_table[a3]) - xn_math_fixmul28(l_10, xn_sin_table[a3]);
    *(int *)((char *)a2) = xn_math_fixmul28(l_10, xn_cos_table[a3]) + xn_math_fixmul28(l_14, xn_sin_table[a3]);
}

int automap_draw_object_cb(struct record *a1)
{
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    short l_18;

    if (model_cache_flush_count != 0) return 1;
    if ((a1->flags & 512) != 0) return 0;
    if ((a1->flags & 1024) != 0) return 0;
    if (((int)player_environment) != 2) {
        if (a1->type == 34 && ((a1->image & 31) - 2) == 8) {
        } else {
            if ((a1->flags & 128) == 0 && ((struct bf8_4_1 *)&cheat_flags)->f == 0) return 0;
            if (a1->type != 6 && a1->type != 32) return 0;
            *(int *)&l_18 = 1132;
            if ((int)D_00196DB0 == (int)a1 && ((struct bf8_3_1 *)(*(char **)&l_18))->f == 0) {
                return 0;
            }
        }
    }
    if (abs(a1->y - scratch_190be8) > 700) return 0;
    if (xn_math_approx_dist2d(a1->x, a1->z, *(int *)scratch_190be4, scratch_190bec) > 2048) return 0;
    a1->draw_handle = 0;
    D_000C5404 = 0;
    switch (a1->type) {
    case 34:
        if (cfg_show_markers == 0) break;
        if (a1->image == 0 || a1->image == 65535) break;
        a1->draw_handle = xn_flat_add(a1->x, a1->y, a1->z, a1->image, -1, 4, 4129024);
        break;
    case 43:
        l_3C = (int)RECORD_DATA(a1);
        l_34 = *(int *)((char *)l_3C + 5);
        l_38 = *(int *)((char *)l_3C + 9);
        for (l_28 = 0; ((int)(unsigned char)*(signed char *)((char *)l_3C)) > l_28; l_28++, (*(char (**)[66])&l_34)++) {
            if (abs(*(int *)((char *)l_34 + 40) - scratch_190be8) > 100) continue;
            *(int *)((char *)l_34 + 4) = model_get((int)(unsigned short)*(short *)((char *)l_34), (int)(unsigned char)*(signed char *)((char *)l_34 + 2), (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)D_001A949C));
            if (*(int *)((char *)l_34 + 4) != 0) xn_model_submit(l_34 + 4, 0);
        }
    case 6:
    case 32:
        if (a1->image2 == 0) break;
        l_40 = (int)RECORD_DATA(a1);
        *(int *)((char *)l_40) = model_get(a1->image2, a1->image, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)D_001A949C));
        if (*(int *)((char *)l_40) != 0) {
            *(int *)((char *)l_40 + 32) = a1->x;
            *(int *)((char *)l_40 + 36) = a1->y;
            *(int *)((char *)l_40 + 40) = a1->z;
            if (a1->image2 == 998) {
                if (weapon_arrow_update(a1) == 0) break;
                weapon_missile_orient(a1);
                *(int *)((char *)l_40 + 44) = a1->missile_yaw;
                *(int *)((char *)l_40 + 48) = a1->angle_z;
                *(int *)((char *)l_40 + 52) = 0;
            } else if (a1->link_flag != 255) {
                xn_model_set_angles_yaw_offset(l_40 + 12, a1->wait_state);
            } else {
                if (a1->image2 == 610 && a1->image == 32) D_000C5404 = 3;
                xn_model_compose_angles(l_40 + 12, a1->angle_x, a1->yaw + a1->wait_state, a1->angle_z);
            }
            xn_model_submit(l_40, 0);
        }
        break;
    case 56:
        l_34 = (int)RECORD_DATA(a1);
        for (l_28 = 0; a1->image > l_28; l_28++, (*(char (**)[66])&l_34)++) {
            if (abs(*(int *)((char *)l_34 + 40) - scratch_190be8) > 100) continue;
            *(int *)((char *)l_34 + 4) = model_get((int)(unsigned short)*(short *)((char *)l_34), (int)(unsigned char)*(signed char *)((char *)l_34 + 2), (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)D_001A949C));
            if (*(int *)((char *)l_34 + 4) != 0) xn_model_submit(l_34 + 4, 0);
        }
    }
    return 0;
}

struct record *rmb_make_light(struct record *a1, int a2, int a3)
{
    struct record *l_14;

    l_14 = object_create_child(a1, 0, 0);
    l_14->type = 7;
    l_14->image = a2;
    l_14->light_radius = a3;
    l_14->pad13 = 8000;
    l_14->id = location_object->id + ((int)(unsigned short)(current_location->object_counter)++);
    return l_14;
}

void rmb_add_doors(struct record *a1, struct block_door *a2)
{
    struct record *l_18;
    int l_14;

    if (*(signed char *)(*(char **)rmb_record_ptr + 4) == 0) return;
    for (l_14 = 0; ((int)(unsigned char)*(signed char *)(*(char **)rmb_record_ptr + 4)) > l_14; l_14++, a2++) {
        l_18 = rmb_make_door(a1, a2->image2, (int)(short)((unsigned short)a2->image), 1);
        xn_model_set_angles(0, (a2->yaw + *(int *)rmb_origin_yaw) % 2048, 0, (int)RECORD_DATA(l_18) + 12);
        l_18->lock_level = (unsigned short)a2->lock_level;
        rotate_xz((int)&a2->x, (int)&a2->z, *(int *)rmb_origin_yaw);
        a2->x += rmb_origin_x;
        l_18->x = a2->x;
        a2->z += rmb_origin_z;
        l_18->z = a2->z;
        a2->y += rmb_origin_y;
        l_18->y = a2->y;
    }
}

void rmb_add_people(struct record *a1, struct block_flat *a2)
{
    struct record *l_18;
    int l_14;

    if (*(signed char *)(*(char **)rmb_record_ptr + 3) == 0) return;
    for (l_14 = 0; ((int)(unsigned char)*(signed char *)(*(char **)rmb_record_ptr + 3)) > l_14; l_14++, a2++) {
        l_18 = rmb_make_flat(a1, (int)(short)a2->image, (int)(short)a2->faction_id, 0);
        rotate_xz((int)&a2->x, (int)&a2->z, *(int *)rmb_origin_yaw);
        a2->x += rmb_origin_x;
        l_18->x = a2->x;
        a2->z += rmb_origin_z;
        l_18->z = a2->z;
        a2->y += rmb_origin_y;
        l_18->y = a2->y;
        if (((int)(unsigned char)(a2->flags & 4)) != 0) l_18->data.person.flags |= 8;
        if (((int)(unsigned char)(a2->flags & 8)) != 0) l_18->data.person.flags |= 32;
        if (((int)(unsigned char)(a2->flags & 32)) != 0) l_18->data.person.flags |= 16;
    }
}
