/* automap.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int xn_cam_pitch;
extern int xn_cam_yaw;
extern int xn_cam_roll;
extern int xn_cam_x;
extern int xn_cam_y;
extern int xn_cam_z;
extern int xn_cam_far_z;
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char xn_tex_cache_full;
extern int xn_light_ambient;
extern char xn_cam_rotation[];
extern char xn_cam_view_matrix[];
extern signed char key_down_esc;
extern signed char key_down_up;
extern signed char key_down_left;
extern signed char key_down_right;
extern signed char key_down_down;
extern short xn_gfx_clip_bottom;
extern int screen_buffer;
extern char D_00170794[];
extern char D_001707A1[];
extern char D_001707AE[];
extern char D_001707B8[];
extern char D_001707E4[];
extern unsigned char player_environment;
extern char automap_buttons[];
extern char D_0017A02E[];
extern char D_0017A030[];
extern char D_0017A032[];
extern char D_0017A034[];
extern signed char D_0017A11D;
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern char scratch_190be4[];
extern int scratch_190be8;
extern int scratch_190bec;
extern int automap_pitch;
extern int automap_yaw;
extern int D_00190BFC;
extern int D_00190C00;
extern int D_00190C04;
extern signed char scratch_190ce5;
extern short scratch_190d68;
extern short scratch_190d6a;
extern char scratch_190de4[];
extern int scratch_190de8;
extern int D_00190E18;
extern signed char player_motion_flags;
extern char location_grid[];
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern struct record *found_object;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern struct settings *game_settings;
extern char scratch_buffer[];
extern struct record *D_00195CB8;
extern signed char D_00196272;
extern signed char mouse_buttons_prev;
extern signed char D_0019629F;
extern int town_map_view_x;
extern int town_map_view_y;
extern int D_00196D94;
extern struct record *D_00196DA0;
extern int D_00196DA4;
extern int town_map_size;
extern int D_00196DAC;
extern struct record *D_00196DB0;
extern int automap_notes;
extern signed char automap_top_down;
extern struct record *quest_event_object2;
extern struct record *quest_event_object;
extern signed char cfg_show_markers;

extern int engine_pick_object(int, int, int);
extern int town_note_at(short, short);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int disk_write_arena2_file(int, int, int);
extern int disk_open_data(int);
extern int disk_file_exists(int);
extern int guild_find_membership_by_kind(unsigned char);
extern struct building *object_building(struct record *);
extern int automap_draw_object_cb(int);
extern int location_contains(int, int);
extern int building_name(struct building *);
extern int inpstr_edit(int, short, short, short, short, short);
extern int object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern struct record *marker_find_first(struct record *, int);
extern int location_cell_at(int, int);
extern int rand();
extern int close();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int read();
extern int mc_strncpy();
extern int strlen();
extern int mc_memmove();
extern int stricmp();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int xn_model_set_angles();
extern int xn_gfx_present_inclusive();
extern int xn_render_set_mode();
extern int xn_cam_set_view_window();
extern int xn_render_begin_frame();
extern int xn_render_frame();
extern int xn_mouse_poll_clamped();
extern int xn_mouse_cursor_move();
extern int xn_mouse_cursor_erase();
extern int xn_mouse_cursor_draw();
extern int xn_tex_cache_flush();
extern int xn_tex_cache_begin_frame();
extern int xn_light_reset();
extern int xn_light_add();
extern int xn_mat_from_angles();
extern int xn_mat_transform_ptr();
extern int xn_mat_transform_transposed_ptr();
extern int xn_cam_scale_matrix();
extern int xn_draw_image();
extern int xn_shade_set_fog();
extern void screenshot_poll(void);
extern void automap_add_note(struct record *, int);
extern void town_map_open(void);
extern void town_map_scroll(int);
extern void town_note_add(int);
extern void quest_set_state(struct quest *, struct qbn_op *, int);
extern void quest_set_arg_state(struct quest *, struct qbn_op *, int, int);
extern void qaction_place_foe(struct qbn_op *, int);
extern void text_draw(int, int, int);
extern void hud_draw_heading_strip(int);
extern void text_draw_coloured(int, int, int, int, unsigned char);
extern void world_collect_objects(void);
extern void building_grant_access(struct building *, unsigned char, int);
extern void object_foreach(struct record *, int);
int automap_move_forward(int);
int automap_move_back(int);
int automap_move_left(int);
int automap_move_right(int);
int automap_find_note(struct record *);
int town_notes_size(void);
int town_note_get(int);
int town_note_exists(int);
void automap_draw(void);
void automap_render(void);
void automap_find_record(void);
void automap_match_record(struct record *);
void automap_delete_note(void);
void automap_init_view(void);
void func_00028210(int);
void town_note_delete(int);
void func_00028547(void);
void town_map_note_building(struct record *, struct building *);
void automap_draw_block_overview(void);
void func_00028DDB(struct record *);
void func_00028DFD(void);
void func_00028ED1(struct record *);
void automap_restore_seen(void);
void func_00028F8B(struct record *);
void unequip_object(struct record *);
#pragma aux mc_set_location parm routine [];

void automap_open(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    struct record *l_1C;
    struct record *l_18;

    l_2C = 0;
    if (((int)player_environment) == 1 && location_contains(player_object->x, player_object->z) != 0) {
        town_map_open();
        return;
    }
    if (((int)player_environment) == 1) return;
    l_20 = (int)(unsigned char)cfg_show_markers;
    cfg_show_markers = 1;
    mouse_buttons_prev = 0;
    automap_init_view();
    xn_cam_far_z = 1048576;
    xn_render_set_mode(8);
    automap_restore_seen();
    D_0019629F = 1;
    l_24 = (int)(unsigned char)D_00196272;
    D_00196272 = 1;
    *(int *)scratch_190de4 = disk_read_file((int)D_00170794, 0);
    D_00190E18 = disk_read_file((int)D_001707A1, 0);
    *(int *)scratch_190be4 = player_object->x;
    scratch_190be8 = player_object->y;
    scratch_190bec = player_object->z;
    xn_gfx_clip_bottom = 199;
    xn_cam_set_view_window(160, 84, 160, 85);
    automap_find_record();
    scratch_190de8 = 0;
    l_18 = 0;
    if (((int)D_00195CB8 != 0 && ((int)player_environment) == 3) || ((int)player_environment) == 2) {
        if (((int)player_environment) == 3) {
            l_1C = D_00195CB8;
            while (l_1C != 0 && l_1C->type != 47 && l_1C->type != 1) l_1C = l_1C->parent;
        } else {
            l_1C = player_object->parent;
        }
        if (l_1C != 0) {
            l_18 = object_create_child(l_1C, 0, 62);
            l_18->image2 = 999;
            l_18->image = 0;
            mc_memcpy(&l_18->angle_x, &player_object->angle_x, 18, (int)D_001707AE, 130, 4);
            l_18->yaw = 2047 - l_18->yaw;
            xn_model_set_angles(l_18->angle_x, l_18->yaw, l_18->angle_z, (int)RECORD_DATA(l_18) + 12);
            l_18->type = 6;
            l_18->flags = 128;
            l_18->y -= 40;
        }
    }
    while (l_2C == 0) {
        automap_draw();
        if (key_down_left != 0) {
            automap_move_left(2);
        } else if (key_down_right != 0) {
            automap_move_right(3);
        } else if (key_down_up != 0) {
            automap_move_forward(0);
        } else if (key_down_down != 0) {
            automap_move_back(1);
        }
        if (key_down_esc != 0) l_2C = 1;
        if (((int)(unsigned char)(mouse_buttons & 3)) != 0) {
            for (l_28 = 0; l_28 < 12; l_28++) {
                if (mouse_x > *(short *)(automap_buttons + (l_28 * 12)) && mouse_x < *(short *)(D_0017A030 + (l_28 * 12)) && mouse_y > *(short *)(D_0017A02E + (l_28 * 12)) && mouse_y < *(short *)(D_0017A032 + (l_28 * 12))) {
                    if (((int)(unsigned char)(mouse_buttons & 3)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 3)) == 0) {
                        sound_play(203, player_object, 100);
                    }
                    l_2C = ((int (*)())(*(int *)(D_0017A034 + (l_28 * 12))))(l_28);
                }
            }
        }
        screenshot_poll();
        xn_gfx_present_inclusive(1);
    }
    while (key_down_esc != 0);
    if (*(int *)scratch_190de4 != 0 && *(int *)scratch_190de4 != (-1751672937)) {
        mc_free(*(int *)scratch_190de4, (int)D_001707AE, 168);
        *(int *)scratch_190de4 = -1751672937;
    }
    if (D_00190E18 != 0 && D_00190E18 != (-1751672937)) {
        mc_free(D_00190E18, (int)D_001707AE, 169);
        D_00190E18 = -1751672937;
    }
    if (l_18 != 0) object_free_single(l_18);
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        xn_cam_set_view_window(160, 100, 160, 100);
    } else {
        xn_cam_set_view_window(160, 77, 160, 77);
    }
    object_foreach(location_object, (int)func_00028F8B);
    func_00028DFD();
    D_00196272 = *(signed char *)&l_24;
    cfg_show_markers = *(signed char *)&l_20;
    D_0019629F = 0;
}

void automap_draw(void)
{
    int l_18;

    xn_shade_set_fog(-1);
    xn_mouse_cursor_erase();
    mc_memcpy(screen_buffer, *(int *)scratch_190de4, 64000, (int)D_001707AE, 192, 4);
    l_18 = D_00190E18;
    if (automap_top_down == 0) {
        xn_draw_image((int)(unsigned short)*(short *)((char *)l_18), (int)(unsigned short)*(short *)((char *)l_18 + 2), (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
    }
    automap_render();
    if (((int)player_environment) != 2) {
        if (D_00196DAC != 0) text_draw_coloured(D_00196DAC, 2, 191, 145, 156);
        automap_draw_block_overview();
    }
    hud_draw_heading_strip(1);
    xn_mouse_cursor_draw();
    mouse_buttons_prev = mouse_buttons;
    xn_mouse_poll_clamped();
    xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
}

int automap_button_note_line(void)
{
    if ((int)D_00196DB0 == 0) return 0;
    if (D_00196DAC != 0) {
        mc_strncpy((int)text_buffer, D_00196DAC, 160, (int)D_001707AE, 213);
    } else {
        mc_memset((int)text_buffer, 0, 80, (int)D_001707AE, 215, 160);
    }
    inpstr_edit((int)text_buffer, 2, 191, 300, 9, 50);
    if (D_00196DAC != 0 && scratch_190de8 != 0) automap_delete_note();
    if (text_buffer[0] != 0) automap_add_note(D_00196DB0, (int)text_buffer);
    D_00196DAC = automap_find_note(D_00196DB0);
    sound_play(206, player_object, 100);
    return 0;
}

int automap_button_rotate_left(void)
{
    automap_yaw = (automap_yaw + 16) & 2047;
    return 0;
}

int automap_button_rotate_right(void)
{
    automap_yaw = (automap_yaw - 16) & 2047;
    return 0;
}

int automap_button_exit(void)
{
    if (((int)player_environment) == 1) return 1;
    xn_render_set_mode(8);
    xn_cam_far_z = 396288;
    return 1;
}

void automap_render(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = xn_light_ambient;
    xn_mat_from_angles(0, automap_yaw, 0, (int)xn_cam_rotation);
    xn_cam_x = D_00190BFC;
    xn_cam_y = D_00190C00;
    xn_cam_z = D_00190C04;
    xn_mat_transform_ptr((int)&xn_cam_x, (int)&xn_cam_y, (int)&xn_cam_z, (int)xn_cam_rotation);
    xn_cam_x += *(int *)scratch_190be4;
    xn_cam_y += scratch_190be8;
    xn_cam_z += scratch_190bec;
    xn_cam_pitch = automap_pitch;
    xn_cam_yaw = (-automap_yaw) & 2047;
    xn_cam_roll = 0;
    xn_light_ambient = 4096;
    xn_tex_cache_begin_frame();
    xn_render_begin_frame();
    xn_light_reset();
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (int)xn_cam_rotation);
    xn_cam_scale_matrix((int)xn_cam_rotation, (int)xn_cam_view_matrix);
    player_motion_flags |= 1;
    l_28 = 0;
    l_24 = 0;
    l_20 = 65535;
    xn_mat_transform_transposed_ptr((int)&l_28, (int)&l_24, (int)&l_20, (int)xn_cam_rotation);
    xn_light_add(l_28, l_24, l_20, 28, 0, 8);
    if (((int)player_environment) == 3) {
        object_foreach(location_object, (int)automap_draw_object_cb);
    } else {
        object_foreach(player_object->parent->children, (int)automap_draw_object_cb);
    }
    player_motion_flags &= 254;
    l_18 = xn_render_frame(2);
    if (l_18 != 0 || xn_tex_cache_full != 0) {
        xn_tex_cache_full = 0;
        xn_tex_cache_flush();
    }
    xn_light_ambient = l_1C;
}

int automap_move_forward(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (((int)player_environment) == 1) {
        town_map_scroll(a1);
        return 0;
    }
    xn_mat_from_angles(0, automap_yaw, 0, (int)xn_cam_rotation);
    l_1C = -16;
    l_24 = 0;
    l_20 = l_24;
    xn_mat_transform_ptr((int)&l_24, (int)&l_20, (int)&l_1C, (int)xn_cam_rotation);
    *(int *)scratch_190be4 += l_24;
    scratch_190bec += l_1C;
    return 0;
}

int automap_move_back(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (((int)player_environment) == 1) {
        town_map_scroll(a1);
        return 0;
    }
    xn_mat_from_angles(0, automap_yaw, 0, (int)xn_cam_rotation);
    l_1C = 16;
    l_24 = 0;
    l_20 = l_24;
    xn_mat_transform_ptr((int)&l_24, (int)&l_20, (int)&l_1C, (int)xn_cam_rotation);
    *(int *)scratch_190be4 += l_24;
    scratch_190bec += l_1C;
    return 0;
}

int automap_move_left(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (((int)player_environment) == 1) {
        town_map_scroll(a1);
        return 0;
    }
    xn_mat_from_angles(0, automap_yaw, 0, (int)xn_cam_rotation);
    l_24 = 16;
    l_1C = 0;
    l_20 = l_1C;
    xn_mat_transform_ptr((int)&l_24, (int)&l_20, (int)&l_1C, (int)xn_cam_rotation);
    *(int *)scratch_190be4 += l_24;
    scratch_190bec += l_1C;
    return 0;
}

int automap_move_right(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (((int)player_environment) == 1) {
        town_map_scroll(a1);
        return 0;
    }
    xn_mat_from_angles(0, automap_yaw, 0, (int)xn_cam_rotation);
    l_24 = -16;
    l_1C = 0;
    l_20 = l_1C;
    xn_mat_transform_ptr((int)&l_24, (int)&l_20, (int)&l_1C, (int)xn_cam_rotation);
    *(int *)scratch_190be4 += l_24;
    scratch_190bec += l_1C;
    return 0;
}

int automap_button_upstairs(void)
{
    if (scratch_190be8 > (-3072)) scratch_190be8 -= 16;
    return 0;
}

int automap_button_downstairs(void)
{
    if (scratch_190be8 < 3072) scratch_190be8 += 16;
    return 0;
}

int automap_button_map_click(void)
{
    char l_2C[20];

    if (((int)player_environment) == 1) {
        func_00028547();
        return 0;
    }
    engine_pick_object((int)(short)mouse_x, (int)(short)mouse_y, (int)l_2C);
    if ((*(int *)l_2C & 1) != 0) {
        if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
            (*(struct record **)(l_2C + 4))->flags |= 0x400;
        } else {
            D_00196DAC = automap_find_note((struct record *)(*(int *)&D_00196DB0 = *(int *)((char *)l_2C + 4)));
        }
    } else {
        D_00196DB0 = 0;
    }
    return 0;
}

void automap_find_record(void)
{
    struct record *l_18;

    found_object = (struct record *)((*(int *)&D_00196DA0 = 0));
    if (player_entity == 0) return;
    object_foreach(player_entity->children, (int)automap_match_record);
    if (found_object == 0) {
        l_18 = object_create_child(player_entity, 0, 10240);
        l_18->type = 51;
        l_18->id = object_new_id(location_object->id >> 16);
        l_18->created_minutes = game_minutes;
        l_18->seen_count = (current_location->object_counter + 7) / 8;
        automap_notes = (int)RECORD_DATA(l_18);
        D_00196DA0 = l_18;
        return;
    }
    automap_notes = (int)RECORD_DATA(found_object);
    D_00196DA0 = found_object;
}

void automap_match_record(struct record *a1)
{
    if (a1->type != 51) return;
    if ((a1->id >> 16) != (location_object->id >> 16)) return;
    found_object = a1;
}

int automap_find_note(struct record *a1)
{
    int l_1C;

    l_1C = automap_notes;
    while (*(signed char *)((char *)l_1C + 2) != 0) {
        scratch_190de8 = l_1C;
        if (((int)(unsigned short)*(short *)((char *)l_1C)) == ((int)a1->id & 65535)) {
            return l_1C + 2;
        }
        l_1C += strlen(l_1C + 2) + 3;
    }
    return 0;
}

void automap_delete_note(void)
{
    int l_18;

    l_18 = strlen(scratch_190de8 + 2) + 3;
    mc_memmove(scratch_190de8, scratch_190de8 + l_18, ((int)(*(char **)&automap_notes + 2048) - scratch_190de8) - l_18, (int)D_001707AE, 494, 4);
}

void automap_init_view(void)
{
    int l_18;

    if (D_0017A11D != 0) return;
    D_0017A11D = 1;
    D_00190BFC = 0;
    if (((int)player_environment) == 3) {
        l_18 = -1536;
    } else {
        l_18 = -1024;
    }
    D_00190C00 = l_18;
    D_00190C04 = 0;
    automap_pitch = 512;
    automap_yaw = 0;
    automap_top_down = 1;
}

int automap_button_view_mode(void)
{
    int l_20;
    int l_1C;

    if (mouse_buttons_prev != 0) return 0;
    if (automap_top_down != 0) {
        D_00190BFC = 0;
        D_00190C00 = -768;
        if (((int)player_environment) == 3) {
            l_1C = -2048;
        } else {
            l_1C = -1024;
        }
        D_00190C04 = l_1C;
        automap_pitch = 128;
        automap_yaw = 0;
        automap_top_down = 0;
    } else {
        D_00190BFC = 0;
        if (((int)player_environment) == 3) {
            l_20 = -1536;
        } else {
            l_20 = -1024;
        }
        D_00190C00 = l_20;
        D_00190C04 = 0;
        automap_pitch = 512;
        automap_yaw = 0;
        automap_top_down = 1;
    }
    return 0;
}

void automap_expire_records(void)
{
    struct record *l_1C;
    struct record *l_18;

    l_1C = player_entity->children;
    while (l_1C != 0) {
        l_18 = l_1C->next;
        if (l_1C->type == 51) {
            if (((unsigned)(game_minutes - l_1C->created_minutes)) > 43200) {
                object_free_single(l_1C);
            }
        }
        l_1C = l_18;
    }
}

void automap_alloc_town_map(void)
{
    town_map_size = (current_location->width * current_location->height) << 12;
    D_00196DA4 = mc_malloc(town_map_size, (int)D_001707AE, 582);
    mc_memset(D_00196DA4, 0, town_map_size, (int)D_001707AE, 583, 4);
}

void automap_free_town_map(void)
{
    town_map_size = 0;
    if (D_00196DA4 == 0 || D_00196DA4 == (-1751672937)) return;
    mc_free(D_00196DA4, (int)D_001707AE, 598);
    D_00196DA4 = -1751672937;
}

int town_notes_size(void)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_1C = *(int *)scratch_buffer + 4;
    while (*(signed char *)((char *)l_1C) != 0) {
        l_24 += 5;
        l_20 = strlen(l_1C + 4);
        l_24 += l_20;
        l_1C += l_20 + 5;
    }
    return l_24 + 4;
}

void func_00028210(int a1)
{
    D_0012B508 = 146;
    if (a1 != 0) {
        mc_strncpy((int)text_buffer, a1, 160, (int)D_001707AE, 809);
    } else {
        mc_memset((int)text_buffer, 0, 80, (int)D_001707AE, 811, 160);
    }
    inpstr_edit((int)text_buffer, 2, 191, 300, 9, 50);
}

void town_note_delete(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    l_1C = *(int *)scratch_buffer + 4;
    while (*(signed char *)((char *)l_1C) != 0 && a1 != 0) {
        l_20 = strlen(l_1C + 4);
        l_1C += l_20 + 5;
        a1--;
    }
    l_18 = (strlen(l_1C + 4) + l_1C) + 5;
    mc_memcpy(l_1C, l_18, (*(int *)scratch_buffer + 49999) - l_18, (int)D_001707AE, 839, 4);
}

int town_note_get(int a1)
{
    int l_20;
    int l_1C;

    l_1C = *(int *)scratch_buffer + 4;
    while (*(short *)((char *)l_1C) != 0 && a1 != 0) {
        l_20 = strlen(l_1C + 4);
        l_1C += l_20 + 5;
        a1--;
    }
    return l_1C;
}

int town_note_exists(int a1)
{
    int l_20;
    int l_1C;

    l_1C = *(int *)scratch_buffer + 4;
    while (*(short *)((char *)l_1C) != 0) {
        if (stricmp(l_1C + 4, a1) == 0) return 1;
        l_20 = strlen(l_1C + 4);
        l_1C += l_20 + 5;
    }
    return 0;
}

void func_00028547(void)
{
    int l_20;
    int l_1C;
    int l_18;

    scratch_190d68 = ((((int)(short)mouse_x) - 10) / 2) + town_map_view_x;
    scratch_190d6a = ((((int)(short)mouse_y) - 10) / 2) + town_map_view_y;
    l_20 = town_note_at((int)(short)mouse_x, (int)(short)mouse_y);
    if (D_00196D94 != 0 && l_20 == 0 && ((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        scratch_190ce5 = 1;
        l_1C = town_note_get(D_00196D94 - 1);
        l_18 = l_1C;
        *(short *)((char *)l_18) = scratch_190d68;
        *(short *)((char *)l_18 + 2) = scratch_190d6a;
        return;
    }
    if (l_20 != 0 && ((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_00196D94 = l_20;
        return;
    }
    if (l_20 != 0) {
        scratch_190ce5 = 1;
        l_1C = town_note_get(l_20 - 1);
        l_18 = l_1C;
        scratch_190d68 = *(short *)((char *)l_18);
        scratch_190d6a = *(short *)((char *)l_18 + 2);
        mc_strncpy((int)text_buffer, l_1C + 4, 160, (int)D_001707AE, 921);
        town_note_delete(l_20 - 1);
        func_00028210((int)text_buffer);
        if (text_buffer[0] != 0) town_note_add((int)text_buffer);
        return;
    }
    mc_memset((int)text_buffer, 0, 80, (int)D_001707AE, 928, 160);
    func_00028210((int)text_buffer);
    if (text_buffer[0] == 0) return;
    town_note_add((int)text_buffer);
}

void town_map_draw_notes(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    short l_18;

    l_20 = 0;
    l_1C = *(int *)scratch_buffer + 4;
    while (*(signed char *)((char *)l_1C) != 0) {
        *(int *)&l_18 = l_1C;
        l_28 = ((((int)(unsigned short)*(short *)(*(char **)&l_18)) - town_map_view_x) * 2) + 10;
        l_24 = ((((int)(unsigned short)*(short *)(*(char **)&l_18 + 2)) - town_map_view_y) * 2) + 10;
        if ((l_20 + 1) == D_00196D94) {
            D_0012B508 = 244;
        } else {
            D_0012B508 = 146;
        }
        if (l_24 < 173) text_draw(l_1C + 4, l_28, l_24);
        l_2C = strlen(l_1C + 4);
        l_1C += l_2C + 5;
        l_20++;
    }
}

void town_map_note_building(struct record *a1, struct building *a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_14 = building_name(a2);
    mc_memset(*(int *)scratch_buffer, 0, 50000, (int)D_001707AE, 963, 4);
    mc_set_location(964, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, location_object->id >> 16);
    if (disk_file_exists((int)text_buffer) != 0) {
        disk_read_file((int)text_buffer, *(int *)scratch_buffer);
        *(int *)(*(char **)scratch_buffer) = game_minutes;
        mc_memcpy(*(int *)scratch_buffer, *(int *)scratch_buffer, 50000, (int)D_001707AE, 970, 4);
        if (town_note_exists(l_14) != 0) return;
    } else {
        *(int *)(*(char **)scratch_buffer) = game_minutes;
    }
    l_1C = a1->x - location_object->x;
    l_18 = a1->z - location_object->z;
    l_1C >>= 6;
    l_18 >>= 6;
    l_18 = ((current_location->height << 6) - l_18) - 1;
    scratch_190d68 = l_1C;
    scratch_190d6a = l_18;
    town_note_add(l_14);
    mc_set_location(986, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, location_object->id >> 16);
    disk_write_arena2_file((int)text_buffer, *(int *)scratch_buffer, town_notes_size());
}

void automap_draw_block_overview(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = location_cell_at(player_object->x, player_object->z);
    l_18 = (int)marker_find_first(location_object, 8);
    if (l_18 != 0) l_18 = location_cell_at(((struct record *)l_18)->x, ((struct record *)l_18)->z);
    for (l_28 = 0; l_28 < 32; l_28++) {
        for (l_24 = 0; l_24 < 32; l_24++) {
            if (*(int *)(location_grid + (((l_28 << 5) + l_24) << 2)) == 0) continue;
            l_20 = ((((31 - l_28) * 640) + 2560) + (l_24 * 2)) + 8;
            if (*(int *)(location_grid + (((l_28 << 5) + l_24) << 2)) == l_1C) {
                D_0012B508 = 240;
            } else if (*(int *)(location_grid + (((l_28 << 5) + l_24) << 2)) == l_18) {
                D_0012B508 = 132;
            } else {
                D_0012B508 = 145;
            }
            *(signed char *)((char *)(screen_buffer + l_20)) = D_0012B508;
            *(signed char *)((char *)(screen_buffer + l_20) + 1) = D_0012B508;
            *(signed char *)((char *)(screen_buffer + l_20) + 320) = D_0012B508;
            *(signed char *)((char *)(screen_buffer + l_20) + 321) = D_0012B508;
        }
    }
}

void automap_save(void)
{
    if (((int)player_environment) != 3 || (int)D_00196DA0 == 0) {
        return;
    }
    *(int *)(*(char **)scratch_buffer) = game_minutes;
    mc_memcpy((int)(*(char **)scratch_buffer + 4), (int)D_00196DA0, 10240, (int)D_001707AE, 1055, 4);
    mc_set_location(1056, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707E4, location_object->id >> 16);
    disk_write_arena2_file((int)text_buffer, *(int *)scratch_buffer, 10244);
}

void automap_load(void)
{
    int l_18;

    if (((int)player_environment) != 3) return;
    mc_memset(*(int *)scratch_buffer, 0, 50000, (int)D_001707AE, 1066, 4);
    mc_set_location(1067, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707E4, location_object->id >> 16);
    l_18 = disk_open_data((int)text_buffer);
    if (l_18 == (-1)) return;
    read(l_18, *(int *)scratch_buffer, 50000);
    close(l_18);
    automap_restore_seen();
}

void town_map_note_secret_guild_halls(void)
{
    int l_24;
    struct building *l_20;
    short l_1C;
    short l_18;

    *(int *)&l_1C = -5;
    *(int *)&l_18 = -5;
    if (guild_find_membership_by_kind(0) != 0) *(int *)&l_18 = 108;
    if (guild_find_membership_by_kind(3) != 0) *(int *)&l_1C = 42;
    l_20 = current_location->buildings;
    for (l_24 = 0; current_location->building_count > l_24; l_24++, l_20++) {
        if (l_20->faction_id == (short)l_1C || l_20->faction_id == (short)l_18) {
            town_map_note_building(object_find_by_id(location_object, l_20->id), l_20);
        }
    }
}

void func_00028DDB(struct record *a1)
{
    a1->flags &= ~0x400;
}

void func_00028DFD(void)
{
    object_foreach(location_object, (int)func_00028DDB);
}

void func_00028EAA(void)
{
    automap_find_record();
    D_00196DA0 = found_object;
}

void func_00028ED1(struct record *a1)
{
    int l_18;

    if (a1->type != 6 && a1->type != 34) return;
    l_18 = (int)D_00196DA0 + 71;
    l_18 += 2048;
    if ((((int)(unsigned char)*(signed char *)((char *)((((unsigned)(a1->id & 65535)) >> 3) + l_18))) & (1 << ((a1->id & 65535) & 7))) == 0) return;
    a1->flags |= 128;
}

void automap_restore_seen(void)
{
    if ((int)D_00196DA0 == 0) automap_find_record();
    object_foreach(location_object, (int)func_00028ED1);
}

void func_00028F8B(struct record *a1)
{
    a1->flags &= ~0x80;
}

int qcond_op05_item_dropped_at_place(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_1C;
    struct record *l_18;

    l_1C = a2->args[2].object;
    if (l_1C->twin == 0) return 0;
    l_1C = l_1C->twin;
    while (l_1C != 0) {
        if (l_1C->type == 43 && ((int)(unsigned short)(l_1C->flags & 1)) != 0) break;
        l_1C = l_1C->parent;
    }
    if (l_1C->type != 43) return 0;
    l_18 = player_object->parent;
    while (l_18 != 0) {
        if (l_18->type == 43 && ((int)(unsigned short)(l_18->flags & 1)) != 0) break;
        l_18 = l_18->parent;
    }
    if (l_18->type != 43) return 0;
    if ((int)quest_event_object->twin == (int)a2->args[1].object && l_1C == l_18) {
        quest_set_state(a1, a2, 1);
        return 1;
    }
    return 0;
}

int qcond_op43_pc_at_place(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    struct record *l_1C;
    struct record *l_18;

    l_1C = a2->args[2].object;
    if (l_1C == 0) return 0;
    if (l_1C->twin == 0) {
        quest_set_arg_state(a1, a2, 1, 0);
        return 0;
    }
    l_1C = l_1C->twin;
    while (l_1C != 0) {
        if ((l_1C->type == 43 && ((int)(unsigned short)(l_1C->flags & 1)) != 0) || l_1C->type == 47 || l_1C->type == 1) {
            break;
        }
        l_1C = l_1C->parent;
    }
    if (l_1C->type != 43 && l_1C->type != 47 && l_1C->type != 1) return 0;
    l_18 = player_object->parent;
    while (l_18 != 0) {
        if ((l_18->type == 43 && ((int)(unsigned short)(l_18->flags & 1)) != 0) || l_18->type == 1) {
            break;
        }
        l_18 = l_18->parent;
    }
    if (l_18->type != 43 && l_18->type != 1) return 0;
    if (l_1C->type == 47) l_1C = l_1C->parent;
    quest_set_arg_state(a1, a2, 1, ((l_18->id == l_1C->id) ? 1 : 0));
    return ((l_18->id == l_1C->id) ? 1 : 0);
}

void qaction_op17_grant_building_access(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_18;
    struct building *l_14;

    l_18 = a2->args[1].object;
    if (l_18->twin == 0) return;
    l_14 = object_building(l_18->twin);
    if (l_14 == 0) return;
    building_grant_access(l_14, (unsigned char)a2->args[2].value, game_minutes + a2->args[3].value);
}

int qcond_op01_item_given_to_npc(struct quest *a1, struct qbn_op *a2)
{
    int l_24;
    struct record *l_20;
    struct record *l_1C;
    int l_18;

    l_20 = a2->args[1].object;
    l_1C = l_20;
    if (l_1C == 0) return 0;
    if (l_1C->twin == 0) return 0;
    l_1C = l_1C->twin;
    while (l_1C != 0 && l_1C != player_entity) l_1C = l_1C->parent;
    if (l_1C != player_entity) return 0;
    if ((a2->args[2].object->type == 65 && (short)quest_event_object2->data.person.faction_id == a2->args[2].object->faction_id) || (int)quest_event_object2->twin == (int)a2->args[2].object) {
        quest_set_state(a1, a2, 1);
        unequip_object(a2->args[1].object->twin);
        object_delete(a2->args[1].object->twin);
        a2->args[1].object->twin = 0;
        a2->args[1].object = 0;
        return 1;
    }
    return 0;
}

int qcond_op03_item_found(struct quest *a1, struct qbn_op *a2)
{
    if ((int)quest_event_object->twin == (int)a2->args[1].object) {
        quest_set_state(a1, a2, 1);
        return 1;
    }
    return 0;
}

int qcond_op21_foe_hurt(struct quest *a1, struct qbn_op *a2)
{
    if ((short)a2->args[1].object->image2 == (short)quest_event_object->image2) {
        quest_set_state(a1, a2, 1);
        return 1;
    }
    return 0;
}

int qcond_op02_foe_killed(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_foe *l_18;

    l_18 = (struct qbn_foe *)a2->args[1].record;
    if ((short)a2->args[1].object->image2 == (short)quest_event_object->image2) {
        if ((short)(short)(l_18->killed) < (short)a2->args[2].value) {
            l_18->killed++;
            if ((short)(l_18->killed) == (short)a2->args[2].value) {
                quest_set_state(a1, a2, 1);
                return 1;
            }
        }
    }
    return 0;
}

void func_0002956B(struct record *a1)
{
    if (a1->twin == 0) return;
    a1->id = object_new_id(location_object->id >> 16);
    a1->twin->twin = 0;
    a1->twin = 0;
}

void qaction_op87_respawn(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_foe *l_18;
    int l_14;

    if (a2->args[4].value == 0) return;
    if (((unsigned)(game_minutes - a2->last_minutes)) < a2->args[2].value) return;
    a2->last_minutes = game_minutes;
    l_18 = (struct qbn_foe *)a2->args[1].record;
    if (l_18->object->twin != 0) {
        if (l_18->object->twin->type != 34) return;
        object_delete(l_18->object->twin);
        if (D_00187CA8 != 0) world_collect_objects();
    }
    if ((rand() % 100) > a2->args[3].value) return;
    if (a2->args[4].value != (-1)) a2->args[4].value--;
    qaction_place_foe(a2, 0);
}

int qcond_op28_npc_clicked(struct quest *a1, struct qbn_op *a2)
{
    if ((a2->args[1].object->type == 65 && a2->args[1].object->faction_id == (short)quest_event_object->data.person.faction_id) || ((int)quest_event_object->twin == (int)a2->args[1].object && a2->args[0].value != (-1))) {
        quest_set_state(a1, a2, 1);
        return 1;
    }
    return 0;
}

int qcond_op70_player_has_items(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    for (l_20 = 2; l_20 < 5; l_20++) {
        if (a2->args[l_20].value == (-1)) continue;
        l_18 = a2->args[l_20].object;
        if (l_18 == 0 || l_18->twin == 0) {
            quest_set_arg_state(a1, a2, 1, 0);
            return 0;
        }
        if ((int)l_18 == (-1768515946)) {
            quest_set_arg_state(a1, a2, 1, 0);
            return 0;
        }
        l_18 = l_18->twin;
        if ((int)l_18 == (-1768515946)) {
            quest_set_arg_state(a1, a2, 1, 0);
            return 0;
        }
        while (l_18 != 0 && l_18 != player_entity) l_18 = l_18->parent;
        if (l_18 != player_entity) {
            quest_set_arg_state(a1, a2, 1, 0);
            return 0;
        }
    }
    quest_set_arg_state(a1, a2, 1, 1);
    return 1;
}

void unequip_object(struct record *a1)
{
    int l_18;

    for (l_18 = 0; l_18 < 27; l_18++) {
        if (player_character->equipped[l_18] == a1) player_character->equipped[l_18] = 0;
    }
}
