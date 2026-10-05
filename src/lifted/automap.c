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
extern char *scratch_190de8;
extern int D_00190E18;
extern signed char player_motion_flags;
extern struct record *location_grid[];
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern struct record *found_object;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern struct settings *game_settings;
extern char *scratch_buffer;
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
extern char *D_00196DAC;
extern struct record *D_00196DB0;
extern char *automap_notes;
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
extern char *building_name(struct building *);
extern int inpstr_edit(int, short, short, short, short, short);
extern int object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern struct record *marker_find_first(struct record *, int);
extern struct record *location_cell_at(int, int);
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
extern void automap_add_note(struct record *, char *);
extern void town_map_open(void);
extern void town_map_scroll(int);
extern void town_note_add(char *);
extern void quest_set_state(struct quest *, struct qbn_op *, int);
extern void quest_set_arg_state(struct quest *, struct qbn_op *, int, int);
extern void qaction_place_foe(struct qbn_op *, int);
extern void text_draw(char *, int, int);
extern void hud_draw_heading_strip(int);
extern void text_draw_coloured(char *, int, int, int, unsigned char);
extern void world_collect_objects(void);
extern void building_grant_access(struct building *, unsigned char, int);
extern void object_foreach(struct record *, int);
int automap_move_forward(int);
int automap_move_back(int);
int automap_move_left(int);
int automap_move_right(int);
char *automap_find_note(struct record *);
int town_notes_size(void);
char *town_note_get(int);
int town_note_exists(char *);
void automap_draw(void);
void automap_render(void);
void automap_find_record(void);
void automap_match_record(struct record *);
void automap_delete_note(void);
void automap_init_view(void);
void func_00028210(char *);
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
    int done;
    int i;
    int saved_screen_active;
    int saved_show_markers;
    struct record *parent;
    struct record *player_marker;

    done = 0;
    if (((int)player_environment) == 1 && location_contains(player_object->x, player_object->z) != 0) {
        town_map_open();
        return;
    }
    if (((int)player_environment) == 1) return;
    saved_show_markers = (int)(unsigned char)cfg_show_markers;
    cfg_show_markers = 1;
    mouse_buttons_prev = 0;
    automap_init_view();
    xn_cam_far_z = 1048576;
    xn_render_set_mode(8);
    automap_restore_seen();
    D_0019629F = 1;
    saved_screen_active = (int)(unsigned char)D_00196272;
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
    player_marker = 0;
    if (((int)D_00195CB8 != 0 && ((int)player_environment) == 3) || ((int)player_environment) == 2) {
        if (((int)player_environment) == 3) {
            parent = D_00195CB8;
            while (parent != 0 && parent->type != 47 && parent->type != 1) parent = parent->parent;
        } else {
            parent = player_object->parent;
        }
        if (parent != 0) {
            player_marker = object_create_child(parent, 0, 62);
            player_marker->image2 = 999;
            player_marker->image = 0;
            mc_memcpy(&player_marker->angle_x, &player_object->angle_x, 18, (int)D_001707AE, 130, 4);
            player_marker->yaw = 2047 - player_marker->yaw;
            xn_model_set_angles(player_marker->angle_x, player_marker->yaw, player_marker->angle_z, (int)RECORD_DATA(player_marker) + 12);
            player_marker->type = 6;
            player_marker->flags = 128;
            player_marker->y -= 40;
        }
    }
    while (done == 0) {
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
        if (key_down_esc != 0) done = 1;
        if (((int)(unsigned char)(mouse_buttons & 3)) != 0) {
            for (i = 0; i < 12; i++) {
                if (mouse_x > *(short *)(automap_buttons + (i * 12)) && mouse_x < *(short *)(D_0017A030 + (i * 12)) && mouse_y > *(short *)(D_0017A02E + (i * 12)) && mouse_y < *(short *)(D_0017A032 + (i * 12))) {
                    if (((int)(unsigned char)(mouse_buttons & 3)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 3)) == 0) {
                        sound_play(203, player_object, 100);
                    }
                    done = ((int (*)())(*(int *)(D_0017A034 + (i * 12))))(i);
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
    if (player_marker != 0) object_free_single(player_marker);
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        xn_cam_set_view_window(160, 100, 160, 100);
    } else {
        xn_cam_set_view_window(160, 77, 160, 77);
    }
    object_foreach(location_object, (int)func_00028F8B);
    func_00028DFD();
    D_00196272 = *(signed char *)&saved_screen_active;
    cfg_show_markers = *(signed char *)&saved_show_markers;
    D_0019629F = 0;
}

void automap_draw(void)
{
    char *image;

    xn_shade_set_fog(-1);
    xn_mouse_cursor_erase();
    mc_memcpy(screen_buffer, *(int *)scratch_190de4, 64000, (int)D_001707AE, 192, 4);
    image = (char *)D_00190E18;
    if (automap_top_down == 0) {
        xn_draw_image((int)(unsigned short)*(short *)image, (int)(unsigned short)*(short *)(image + 2), (int)(unsigned short)*(short *)(image + 4), (int)(unsigned short)*(short *)(image + 6), image + 12);
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
    if (text_buffer[0] != 0) automap_add_note(D_00196DB0, (char *)text_buffer);
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
    int light_x;
    int light_y;
    int light_z;
    int saved_ambient;
    int overflow;

    saved_ambient = xn_light_ambient;
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
    light_x = 0;
    light_y = 0;
    light_z = 65535;
    xn_mat_transform_transposed_ptr((int)&light_x, (int)&light_y, (int)&light_z, (int)xn_cam_rotation);
    xn_light_add(light_x, light_y, light_z, 28, 0, 8);
    if (((int)player_environment) == 3) {
        object_foreach(location_object, (int)automap_draw_object_cb);
    } else {
        object_foreach(player_object->parent->children, (int)automap_draw_object_cb);
    }
    player_motion_flags &= 254;
    overflow = xn_render_frame(2);
    if (overflow != 0 || xn_tex_cache_full != 0) {
        xn_tex_cache_full = 0;
        xn_tex_cache_flush();
    }
    xn_light_ambient = saved_ambient;
}

int automap_move_forward(int direction)
{
    int step_x;
    int step_y;
    int step_z;

    if (((int)player_environment) == 1) {
        town_map_scroll(direction);
        return 0;
    }
    xn_mat_from_angles(0, automap_yaw, 0, (int)xn_cam_rotation);
    step_z = -16;
    step_x = 0;
    step_y = step_x;
    xn_mat_transform_ptr((int)&step_x, (int)&step_y, (int)&step_z, (int)xn_cam_rotation);
    *(int *)scratch_190be4 += step_x;
    scratch_190bec += step_z;
    return 0;
}

int automap_move_back(int direction)
{
    int step_x;
    int step_y;
    int step_z;

    if (((int)player_environment) == 1) {
        town_map_scroll(direction);
        return 0;
    }
    xn_mat_from_angles(0, automap_yaw, 0, (int)xn_cam_rotation);
    step_z = 16;
    step_x = 0;
    step_y = step_x;
    xn_mat_transform_ptr((int)&step_x, (int)&step_y, (int)&step_z, (int)xn_cam_rotation);
    *(int *)scratch_190be4 += step_x;
    scratch_190bec += step_z;
    return 0;
}

int automap_move_left(int direction)
{
    int step_x;
    int step_y;
    int step_z;

    if (((int)player_environment) == 1) {
        town_map_scroll(direction);
        return 0;
    }
    xn_mat_from_angles(0, automap_yaw, 0, (int)xn_cam_rotation);
    step_x = 16;
    step_z = 0;
    step_y = step_z;
    xn_mat_transform_ptr((int)&step_x, (int)&step_y, (int)&step_z, (int)xn_cam_rotation);
    *(int *)scratch_190be4 += step_x;
    scratch_190bec += step_z;
    return 0;
}

int automap_move_right(int direction)
{
    int step_x;
    int step_y;
    int step_z;

    if (((int)player_environment) == 1) {
        town_map_scroll(direction);
        return 0;
    }
    xn_mat_from_angles(0, automap_yaw, 0, (int)xn_cam_rotation);
    step_x = -16;
    step_z = 0;
    step_y = step_z;
    xn_mat_transform_ptr((int)&step_x, (int)&step_y, (int)&step_z, (int)xn_cam_rotation);
    *(int *)scratch_190be4 += step_x;
    scratch_190bec += step_z;
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
    char pick[20];

    if (((int)player_environment) == 1) {
        func_00028547();
        return 0;
    }
    engine_pick_object((int)(short)mouse_x, (int)(short)mouse_y, (int)pick);
    if ((*(int *)pick & 1) != 0) {
        if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
            (*(struct record **)(pick + 4))->flags |= 0x400;
        } else {
            D_00196DAC = automap_find_note((struct record *)(*(int *)&D_00196DB0 = *(int *)((char *)pick + 4)));
        }
    } else {
        D_00196DB0 = 0;
    }
    return 0;
}

void automap_find_record(void)
{
    struct record *object;

    found_object = (struct record *)((*(int *)&D_00196DA0 = 0));
    if (player_entity == 0) return;
    object_foreach(player_entity->children, (int)automap_match_record);
    if (found_object == 0) {
        object = object_create_child(player_entity, 0, 10240);
        object->type = 51;
        object->id = object_new_id(location_object->id >> 16);
        object->created_minutes = game_minutes;
        object->seen_count = (current_location->object_counter + 7) / 8;
        automap_notes = RECORD_DATA(object);
        D_00196DA0 = object;
        return;
    }
    automap_notes = RECORD_DATA(found_object);
    D_00196DA0 = found_object;
}

void automap_match_record(struct record *object)
{
    if (object->type != 51) return;
    if ((object->id >> 16) != (location_object->id >> 16)) return;
    found_object = object;
}

char *automap_find_note(struct record *object)
{
    char *note;

    note = automap_notes;
    while (*(signed char *)(note + 2) != 0) {
        scratch_190de8 = note;
        if (((int)(unsigned short)*(short *)note) == ((int)object->id & 65535)) {
            return note + 2;
        }
        note += strlen(note + 2) + 3;
    }
    return 0;
}

void automap_delete_note(void)
{
    int length;

    length = strlen(scratch_190de8 + 2) + 3;
    mc_memmove(scratch_190de8, scratch_190de8 + length, ((automap_notes + 2048) - scratch_190de8) - length, (int)D_001707AE, 494, 4);
}

void automap_init_view(void)
{
    int camera_y;

    if (D_0017A11D != 0) return;
    D_0017A11D = 1;
    D_00190BFC = 0;
    if (((int)player_environment) == 3) {
        camera_y = -1536;
    } else {
        camera_y = -1024;
    }
    D_00190C00 = camera_y;
    D_00190C04 = 0;
    automap_pitch = 512;
    automap_yaw = 0;
    automap_top_down = 1;
}

int automap_button_view_mode(void)
{
    int camera_y;
    int camera_z;

    if (mouse_buttons_prev != 0) return 0;
    if (automap_top_down != 0) {
        D_00190BFC = 0;
        D_00190C00 = -768;
        if (((int)player_environment) == 3) {
            camera_z = -2048;
        } else {
            camera_z = -1024;
        }
        D_00190C04 = camera_z;
        automap_pitch = 128;
        automap_yaw = 0;
        automap_top_down = 0;
    } else {
        D_00190BFC = 0;
        if (((int)player_environment) == 3) {
            camera_y = -1536;
        } else {
            camera_y = -1024;
        }
        D_00190C00 = camera_y;
        D_00190C04 = 0;
        automap_pitch = 512;
        automap_yaw = 0;
        automap_top_down = 1;
    }
    return 0;
}

void automap_expire_records(void)
{
    struct record *object;
    struct record *next;

    object = player_entity->children;
    while (object != 0) {
        next = object->next;
        if (object->type == 51) {
            if (((unsigned)(game_minutes - object->created_minutes)) > 43200) {
                object_free_single(object);
            }
        }
        object = next;
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
    int size;
    int length;
    char *note;

    size = 0;
    note = scratch_buffer + 4;
    while (*(signed char *)note != 0) {
        size += 5;
        length = strlen(note + 4);
        size += length;
        note += length + 5;
    }
    return size + 4;
}

void func_00028210(char *text)
{
    D_0012B508 = 146;
    if (text != 0) {
        mc_strncpy((int)text_buffer, text, 160, (int)D_001707AE, 809);
    } else {
        mc_memset((int)text_buffer, 0, 80, (int)D_001707AE, 811, 160);
    }
    inpstr_edit((int)text_buffer, 2, 191, 300, 9, 50);
}

void town_note_delete(int index)
{
    int length;
    char *note;
    char *next;

    note = scratch_buffer + 4;
    while (*(signed char *)note != 0 && index != 0) {
        length = strlen(note + 4);
        note += length + 5;
        index--;
    }
    next = (strlen(note + 4) + note) + 5;
    mc_memcpy(note, next, (scratch_buffer + 49999) - next, (int)D_001707AE, 839, 4);
}

char *town_note_get(int index)
{
    int length;
    char *note;

    note = scratch_buffer + 4;
    while (*(short *)note != 0 && index != 0) {
        length = strlen(note + 4);
        note += length + 5;
        index--;
    }
    return note;
}

int town_note_exists(char *text)
{
    int length;
    char *note;

    note = scratch_buffer + 4;
    while (*(short *)note != 0) {
        if (stricmp(note + 4, text) == 0) return 1;
        length = strlen(note + 4);
        note += length + 5;
    }
    return 0;
}

void func_00028547(void)
{
    int note_index;
    char *note;
    char *entry;

    scratch_190d68 = ((((int)(short)mouse_x) - 10) / 2) + town_map_view_x;
    scratch_190d6a = ((((int)(short)mouse_y) - 10) / 2) + town_map_view_y;
    note_index = town_note_at((int)(short)mouse_x, (int)(short)mouse_y);
    if (D_00196D94 != 0 && note_index == 0 && ((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        scratch_190ce5 = 1;
        note = town_note_get(D_00196D94 - 1);
        entry = note;
        *(short *)entry = scratch_190d68;
        *(short *)(entry + 2) = scratch_190d6a;
        return;
    }
    if (note_index != 0 && ((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        D_00196D94 = note_index;
        return;
    }
    if (note_index != 0) {
        scratch_190ce5 = 1;
        note = town_note_get(note_index - 1);
        entry = note;
        scratch_190d68 = *(short *)entry;
        scratch_190d6a = *(short *)(entry + 2);
        mc_strncpy((int)text_buffer, note + 4, 160, (int)D_001707AE, 921);
        town_note_delete(note_index - 1);
        func_00028210((char *)text_buffer);
        if (text_buffer[0] != 0) town_note_add((char *)text_buffer);
        return;
    }
    mc_memset((int)text_buffer, 0, 80, (int)D_001707AE, 928, 160);
    func_00028210((char *)text_buffer);
    if (text_buffer[0] == 0) return;
    town_note_add((char *)text_buffer);
}

void town_map_draw_notes(void)
{
    int length;
    int x;
    int y;
    int index;
    char *note;
    char *entry;

    index = 0;
    note = scratch_buffer + 4;
    while (*(signed char *)note != 0) {
        entry = note;
        x = ((((int)(unsigned short)*(short *)entry) - town_map_view_x) * 2) + 10;
        y = ((((int)(unsigned short)*(short *)(entry + 2)) - town_map_view_y) * 2) + 10;
        if ((index + 1) == D_00196D94) {
            D_0012B508 = 244;
        } else {
            D_0012B508 = 146;
        }
        if (y < 173) text_draw(note + 4, x, y);
        length = strlen(note + 4);
        note += length + 5;
        index++;
    }
}

void town_map_note_building(struct record *object, struct building *building)
{
    int x;
    int y;
    char *name;

    name = building_name(building);
    mc_memset((int)scratch_buffer, 0, 50000, (int)D_001707AE, 963, 4);
    mc_set_location(964, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, location_object->id >> 16);
    if (disk_file_exists((int)text_buffer) != 0) {
        disk_read_file((int)text_buffer, (int)scratch_buffer);
        *(int *)scratch_buffer = game_minutes;
        mc_memcpy((int)scratch_buffer, (int)scratch_buffer, 50000, (int)D_001707AE, 970, 4);
        if (town_note_exists(name) != 0) return;
    } else {
        *(int *)scratch_buffer = game_minutes;
    }
    x = object->x - location_object->x;
    y = object->z - location_object->z;
    x >>= 6;
    y >>= 6;
    y = ((current_location->height << 6) - y) - 1;
    scratch_190d68 = x;
    scratch_190d6a = y;
    town_note_add(name);
    mc_set_location(986, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707B8, location_object->id >> 16);
    disk_write_arena2_file((int)text_buffer, (int)scratch_buffer, town_notes_size());
}

void automap_draw_block_overview(void)
{
    int row;
    int column;
    int offset;
    struct record *player_cell;
    struct record *start_cell;

    player_cell = location_cell_at(player_object->x, player_object->z);
    start_cell = marker_find_first(location_object, 8);
    if (start_cell != 0) start_cell = location_cell_at(start_cell->x, start_cell->z);
    for (row = 0; row < 32; row++) {
        for (column = 0; column < 32; column++) {
            if (location_grid[(row << 5) + column] == 0) continue;
            offset = ((((31 - row) * 640) + 2560) + (column * 2)) + 8;
            if (location_grid[(row << 5) + column] == player_cell) {
                D_0012B508 = 240;
            } else if (location_grid[(row << 5) + column] == start_cell) {
                D_0012B508 = 132;
            } else {
                D_0012B508 = 145;
            }
            *(signed char *)((char *)(screen_buffer + offset)) = D_0012B508;
            *(signed char *)((char *)(screen_buffer + offset) + 1) = D_0012B508;
            *(signed char *)((char *)(screen_buffer + offset) + 320) = D_0012B508;
            *(signed char *)((char *)(screen_buffer + offset) + 321) = D_0012B508;
        }
    }
}

void automap_save(void)
{
    if (((int)player_environment) != 3 || (int)D_00196DA0 == 0) {
        return;
    }
    *(int *)scratch_buffer = game_minutes;
    mc_memcpy((int)(scratch_buffer + 4), (int)D_00196DA0, 10240, (int)D_001707AE, 1055, 4);
    mc_set_location(1056, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707E4, location_object->id >> 16);
    disk_write_arena2_file((int)text_buffer, (int)scratch_buffer, 10244);
}

void automap_load(void)
{
    int handle;

    if (((int)player_environment) != 3) return;
    mc_memset((int)scratch_buffer, 0, 50000, (int)D_001707AE, 1066, 4);
    mc_set_location(1067, (int)D_001707AE);
    mc_sprintf((int)text_buffer, (int)D_001707E4, location_object->id >> 16);
    handle = disk_open_data((int)text_buffer);
    if (handle == (-1)) return;
    read(handle, (int)scratch_buffer, 50000);
    close(handle);
    automap_restore_seen();
}

void town_map_note_secret_guild_halls(void)
{
    int i;
    struct building *building;
    int thieves_guild_faction;
    int dark_brotherhood_faction;

    thieves_guild_faction = -5;
    dark_brotherhood_faction = -5;
    if (guild_find_membership_by_kind(0) != 0) dark_brotherhood_faction = 108;
    if (guild_find_membership_by_kind(3) != 0) thieves_guild_faction = 42;
    building = current_location->buildings;
    for (i = 0; current_location->building_count > i; i++, building++) {
        if (building->faction_id == (short)thieves_guild_faction || building->faction_id == (short)dark_brotherhood_faction) {
            town_map_note_building(object_find_by_id(location_object, building->id), building);
        }
    }
}

void func_00028DDB(struct record *object)
{
    object->flags &= ~0x400;
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

void func_00028ED1(struct record *object)
{
    char *seen_bits;

    if (object->type != 6 && object->type != 34) return;
    seen_bits = RECORD_DATA(D_00196DA0);
    seen_bits += 2048;
    if ((((int)(unsigned char)*(signed char *)((((unsigned)(object->id & 65535)) >> 3) + seen_bits)) & (1 << ((object->id & 65535) & 7))) == 0) return;
    object->flags |= 128;
}

void automap_restore_seen(void)
{
    if ((int)D_00196DA0 == 0) automap_find_record();
    object_foreach(location_object, (int)func_00028ED1);
}

void func_00028F8B(struct record *object)
{
    object->flags &= ~0x80;
}

int qcond_op05_item_dropped_at_place(struct quest *quest, struct qbn_op *op)
{
    struct record *item_place;
    struct record *player_place;

    item_place = op->args[2].object;
    if (item_place->twin == 0) return 0;
    item_place = item_place->twin;
    while (item_place != 0) {
        if (item_place->type == 43 && ((int)(unsigned short)(item_place->flags & 1)) != 0) break;
        item_place = item_place->parent;
    }
    if (item_place->type != 43) return 0;
    player_place = player_object->parent;
    while (player_place != 0) {
        if (player_place->type == 43 && ((int)(unsigned short)(player_place->flags & 1)) != 0) break;
        player_place = player_place->parent;
    }
    if (player_place->type != 43) return 0;
    if ((int)quest_event_object->twin == (int)op->args[1].object && item_place == player_place) {
        quest_set_state(quest, op, 1);
        return 1;
    }
    return 0;
}

int qcond_op43_pc_at_place(struct quest *quest, struct qbn_op *op)
{
    int unused;
    struct record *place;
    struct record *player_place;

    place = op->args[2].object;
    if (place == 0) return 0;
    if (place->twin == 0) {
        quest_set_arg_state(quest, op, 1, 0);
        return 0;
    }
    place = place->twin;
    while (place != 0) {
        if ((place->type == 43 && ((int)(unsigned short)(place->flags & 1)) != 0) || place->type == 47 || place->type == 1) {
            break;
        }
        place = place->parent;
    }
    if (place->type != 43 && place->type != 47 && place->type != 1) return 0;
    player_place = player_object->parent;
    while (player_place != 0) {
        if ((player_place->type == 43 && ((int)(unsigned short)(player_place->flags & 1)) != 0) || player_place->type == 1) {
            break;
        }
        player_place = player_place->parent;
    }
    if (player_place->type != 43 && player_place->type != 1) return 0;
    if (place->type == 47) place = place->parent;
    quest_set_arg_state(quest, op, 1, ((player_place->id == place->id) ? 1 : 0));
    return ((player_place->id == place->id) ? 1 : 0);
}

void qaction_op17_grant_building_access(struct quest *quest, struct qbn_op *op)
{
    struct record *place;
    struct building *building;

    place = op->args[1].object;
    if (place->twin == 0) return;
    building = object_building(place->twin);
    if (building == 0) return;
    building_grant_access(building, (unsigned char)op->args[2].value, game_minutes + op->args[3].value);
}

int qcond_op01_item_given_to_npc(struct quest *quest, struct qbn_op *op)
{
    int unused;
    struct record *resource;
    struct record *object;
    int unused2;

    resource = op->args[1].object;
    object = resource;
    if (object == 0) return 0;
    if (object->twin == 0) return 0;
    object = object->twin;
    while (object != 0 && object != player_entity) object = object->parent;
    if (object != player_entity) return 0;
    if ((op->args[2].object->type == 65 && (short)quest_event_object2->data.person.faction_id == op->args[2].object->faction_id) || (int)quest_event_object2->twin == (int)op->args[2].object) {
        quest_set_state(quest, op, 1);
        unequip_object(op->args[1].object->twin);
        object_delete(op->args[1].object->twin);
        op->args[1].object->twin = 0;
        op->args[1].object = 0;
        return 1;
    }
    return 0;
}

int qcond_op03_item_found(struct quest *quest, struct qbn_op *op)
{
    if ((int)quest_event_object->twin == (int)op->args[1].object) {
        quest_set_state(quest, op, 1);
        return 1;
    }
    return 0;
}

int qcond_op21_foe_hurt(struct quest *quest, struct qbn_op *op)
{
    if ((short)op->args[1].object->image2 == (short)quest_event_object->image2) {
        quest_set_state(quest, op, 1);
        return 1;
    }
    return 0;
}

int qcond_op02_foe_killed(struct quest *quest, struct qbn_op *op)
{
    struct qbn_foe *foe;

    foe = (struct qbn_foe *)op->args[1].record;
    if ((short)op->args[1].object->image2 == (short)quest_event_object->image2) {
        if ((short)(short)(foe->killed) < (short)op->args[2].value) {
            foe->killed++;
            if ((short)(foe->killed) == (short)op->args[2].value) {
                quest_set_state(quest, op, 1);
                return 1;
            }
        }
    }
    return 0;
}

void func_0002956B(struct record *object)
{
    if (object->twin == 0) return;
    object->id = object_new_id(location_object->id >> 16);
    object->twin->twin = 0;
    object->twin = 0;
}

void qaction_op87_respawn(struct quest *quest, struct qbn_op *op)
{
    struct qbn_foe *foe;
    int unused;

    if (op->args[4].value == 0) return;
    if (((unsigned)(game_minutes - op->last_minutes)) < op->args[2].value) return;
    op->last_minutes = game_minutes;
    foe = (struct qbn_foe *)op->args[1].record;
    if (foe->object->twin != 0) {
        if (foe->object->twin->type != 34) return;
        object_delete(foe->object->twin);
        if (D_00187CA8 != 0) world_collect_objects();
    }
    if ((rand() % 100) > op->args[3].value) return;
    if (op->args[4].value != (-1)) op->args[4].value--;
    qaction_place_foe(op, 0);
}

int qcond_op28_npc_clicked(struct quest *quest, struct qbn_op *op)
{
    if ((op->args[1].object->type == 65 && op->args[1].object->faction_id == (short)quest_event_object->data.person.faction_id) || ((int)quest_event_object->twin == (int)op->args[1].object && op->args[0].value != (-1))) {
        quest_set_state(quest, op, 1);
        return 1;
    }
    return 0;
}

int qcond_op70_player_has_items(struct quest *quest, struct qbn_op *op)
{
    int i;
    int unused;
    struct record *object;

    for (i = 2; i < 5; i++) {
        if (op->args[i].value == (-1)) continue;
        object = op->args[i].object;
        if (object == 0 || object->twin == 0) {
            quest_set_arg_state(quest, op, 1, 0);
            return 0;
        }
        if ((int)object == (-1768515946)) {
            quest_set_arg_state(quest, op, 1, 0);
            return 0;
        }
        object = object->twin;
        if ((int)object == (-1768515946)) {
            quest_set_arg_state(quest, op, 1, 0);
            return 0;
        }
        while (object != 0 && object != player_entity) object = object->parent;
        if (object != player_entity) {
            quest_set_arg_state(quest, op, 1, 0);
            return 0;
        }
    }
    quest_set_arg_state(quest, op, 1, 1);
    return 1;
}

void unequip_object(struct record *object)
{
    int slot;

    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] == object) player_character->equipped[slot] = 0;
    }
}
