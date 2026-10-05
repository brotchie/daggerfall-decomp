/* text.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern int xn_cam_far_z;
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down[];
extern signed char key_down_esc;
extern signed char key_down_ctrl;
extern signed char key_down_lshift;
extern signed char key_down_x;
extern int screen_buffer;
extern int D_00147954;
extern char D_00170D55[];
extern char D_00170DAE[];
extern char D_00170DB7[];
extern unsigned char player_environment;
extern int guards_timer;
extern signed char anim_mirror_facing[];
extern short D_0017B64F[];
extern struct xz_step D_0017B657[];
extern signed char D_0017B6CD;
extern struct record *creature_list[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern signed char D_001940D6;
extern signed char D_001940DA;
extern char frame_counter[];
extern struct record *player_object;
extern struct record *scratch_current_object;
extern int frame_ticks;
extern struct record *location_object;
extern int creature_count;
extern char *buttons_rci;
extern struct location *current_location;
extern struct character *player_character;
extern int realtime_clock_tick;
extern signed char climate_weathers[];
extern short D_00195F2E;
extern short D_00195F34;
extern unsigned short text_cursor_y;
extern signed char msgbox_button_keys;
extern signed char D_00196034;
extern signed char D_00196035;
extern signed char msgbox_button_ids;
extern signed char D_00196090;
extern signed char D_00196091;
extern signed char current_region;
extern signed char msgbox_kind;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char D_0019628E;
extern int D_00196DA4;
extern int msgbox_spop_tiles[];
extern int msgbox_mpop_tiles;
extern char *msgbox_next_page;
extern char msgbox_image[];
extern int msgbox_saved_screen;
extern short msgbox_w;
extern short msgbox_h;
extern signed char D_0019966C;
extern struct record *people_list[];
extern int people_max;
extern int people_idle_count;
extern int people_spawn_range;
extern int people_count;
extern struct quest *current_quest;
extern int daylight;

extern struct faction *faction_find_type_in_region(int, short);
extern int climate_category(void);
extern int collide_creature_near(int, int);
extern char *text_rsc_load(int, int, int);
extern char *text_qrc_load_for_quest(struct quest *, short, int, short);
extern int msgbox_render_rsc(short, int, int);
extern int msgbox_render_quest_text(struct quest *, short, int, int);
extern int town_map_area_clear(int, int);
extern int ai_angle_diff(int, int, int *);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int spawn_find_point(struct record *, int, int);
extern int key_pressed_once(unsigned char);
extern int rand_range(int, int);
extern int location_contains(int, int);
extern int inpstr_update(void);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern int rand();
extern int abs();
extern int mc_free();
extern int mc_malloc();
extern int mc_memcpy();
extern int xn_math_approx_dist2d();
extern int xn_math_angle_to_point();
extern int xn_gfx_present_inclusive();
extern void xn_math_yaw_offset_xz(int, int, int *, int *);
extern int xn_mouse_poll_clamped();
extern int xn_font_select();
extern int xn_draw_image();
extern int xn_draw_image_transparent();
extern int xn_terrain_height_at();
extern unsigned char ground_tile_at(int, int);
extern void marquee_start(char *);
extern void msgbox_render(char *, char **);
extern void msgbox_set_border_style(int);
extern void guards_summon(int);
extern void pedestrian_pick_sprite(struct record *);
extern void keys_world_actions(void);
extern void game_exit(int);
extern void func_0006987B(void);
extern void monster_init(struct record *, int);
extern void mode_push(void);
extern void mode_pop(void);
extern void cursor_restore_background(void);
extern void player_movement_update(void);
int msgbox_button_at(short, short);
int pedestrian_walk(struct record *, int);
int people_max_count(void);
int pedestrian_clear_of_others(struct record *, int);
int pedestrian_can_stand_at(int, int);
int pedestrian_spawn_spot_ok(struct record *, int, int);
int town_map_line_blocked(int, int);
void msgbox_wait(void);
void msgbox_update(void);
void msgbox_close(void);
void pedestrian_place(struct record *);
void people_set_spawn_range(void);

void str_copy_double_nul(signed char *dest, signed char *src)
{
    signed char *from;
    signed char *to;

    from = src;
    to = dest;
    while (*from != 0 || from[1] != 0) {
        *to = *from;
        from++;
        to++;
    }
    *to = 0;
    to[1] = 0;
}

void msgbox_show_more_pages(char **image)
{
    while (msgbox_next_page != 0) {
        mc_memcpy(D_00147954, screen_buffer, 64000, (int)D_00170D55, 607, 4);
        msgbox_kind = 1;
        mode_push();
        game_mode = 8;
        D_00196272 = 1;
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        mouse_buttons = (mouse_buttons_prev = 0);
        while (((int)(unsigned char)game_mode) == 8) {
            mc_memcpy(screen_buffer, D_00147954, 64000, (int)D_00170D55, 618, 4);
            keys_world_actions();
            msgbox_update();
            player_movement_update();
            xn_gfx_present_inclusive(0);
        }
        if (msgbox_next_page != 0) {
            D_0012B508 = 146;
            msgbox_render(msgbox_next_page, image);
            while (mouse_buttons != 0) xn_mouse_poll_clamped();
            mouse_buttons_prev = 0;
        }
    }
    msgbox_kind = 1;
    mode_push();
    game_mode = 8;
    D_00196272 = 1;
    mouse_buttons = (mouse_buttons_prev = 0);
    while (((int)(unsigned char)game_mode) == 8) {
        mc_memcpy(screen_buffer, D_00147954, 64000, (int)D_00170D55, 641, 4);
        keys_world_actions();
        msgbox_update();
        player_movement_update();
        xn_gfx_present_inclusive(0);
    }
}

void msgbox_show_quest_text(struct quest *quest, short message_id, int kind)
{
    int single_page;

    if (msgbox_kind != 0) return;
    current_quest = quest;
    msgbox_saved_screen = mc_malloc(64000, (int)D_00170D55, 735);
    mc_memcpy(msgbox_saved_screen, screen_buffer, 64000, (int)D_00170D55, 736, 4);
    xn_font_select(4);
    if ((short)kind == 5) {
        D_00196271 = 0;
        single_page = msgbox_render_quest_text(quest, message_id, (int)msgbox_image, 4);
    } else {
        single_page = msgbox_render_quest_text(quest, message_id, (int)msgbox_image, 0);
    }
    if (single_page == 0) return;
    msgbox_kind = kind;
    mode_push();
    game_mode = 8;
    D_00196272 = 1;
    mouse_buttons_prev = mouse_buttons;
    xn_mouse_poll_clamped();
    msgbox_wait();
}

void marquee_show_quest_text(struct quest *quest, short message_id)
{
    char *text;

    current_quest = quest;
    text = text_qrc_load_for_quest(quest, message_id, 0, 280);
    if (text == 0 || *text == 0) return;
    D_001940DA |= 8;
    marquee_start(text);
}

void marquee_show_rsc(short text_id)
{
    {
        char *text;

        text = text_rsc_load(text_id, 0, 280);
        if (text == 0 || *text == 0) return;
        D_001940DA |= 8;
        marquee_start(text);
    }
}

void msgbox_show_rsc(int text_id, int kind)
{
    {
        int single_page;

        if (msgbox_kind != 0) return;
        cursor_restore_background();
        msgbox_saved_screen = mc_malloc(64000, (int)D_00170D55, 824);
        mc_memcpy(msgbox_saved_screen, screen_buffer, 64000, (int)D_00170D55, 825, 4);
        xn_font_select(4);
        if ((short)kind == 5) {
            D_00196271 = 0;
            single_page = msgbox_render_rsc((short)text_id, (int)msgbox_image, 4);
        } else {
            single_page = msgbox_render_rsc((short)text_id, (int)msgbox_image, 0);
        }
        if (single_page == 0) return;
        msgbox_kind = kind;
        mode_push();
        game_mode = 8;
        D_00196272 = 1;
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        msgbox_wait();
    }
}

void msgbox_open_rsc(int text_id, int kind)
{
    {
        int single_page;

        if (msgbox_kind != 0) return;
        cursor_restore_background();
        msgbox_saved_screen = mc_malloc(64000, (int)D_00170D55, 854);
        mc_memcpy(msgbox_saved_screen, screen_buffer, 64000, (int)D_00170D55, 855, 4);
        xn_font_select(4);
        if ((short)kind == 5) {
            D_00196271 = 0;
            single_page = msgbox_render_rsc((short)text_id, (int)msgbox_image, 4);
        } else {
            single_page = msgbox_render_rsc((short)text_id, (int)msgbox_image, 0);
        }
        if (single_page == 0) return;
        msgbox_kind = kind;
        mode_push();
        game_mode = 8;
        D_00196272 = 1;
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
    }
}

void msgbox_wait(void)
{
    int *bios_ticks;

    mouse_buttons = (mouse_buttons_prev = 1);
    while (((int)(unsigned char)game_mode) == 8) {
        mc_memcpy(screen_buffer, msgbox_saved_screen, 64000, (int)D_00170D55, 882, 4);
        if (key_down_ctrl != 0 && key_down_x != 0 && key_down_lshift != 0) {
            game_exit(0);
        }
        msgbox_update();
        player_movement_update();
        xn_gfx_present_inclusive(1);
        if (((struct bf8_0_4 *)&frame_counter)->f == 0) func_0006987B();
    }
    bios_ticks = (int *)1132;
    realtime_clock_tick = *bios_ticks;
    if (msgbox_saved_screen != 0 && msgbox_saved_screen != (-1751672937)) {
        mc_free(msgbox_saved_screen, (int)D_00170D55, 893);
        msgbox_saved_screen = -1751672937;
    }
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
}

void msgbox_update(void)
{
    int button;

    if (((int)(unsigned char)msgbox_kind) != 4 && ((int)(unsigned char)game_mode) != 8) {
        return;
    }
    D_0012B508 = 146;
    if (((struct bf8_6_1 *)&D_001940D5)->f != 0) {
        xn_draw_image_transparent(160 - (((int)(short)msgbox_w) >> 1), ((int)(short)D_00195F34) - ((int)(short)msgbox_h), (int)(short)msgbox_w, (int)(short)msgbox_h, *(int *)msgbox_image);
    } else {
        xn_draw_image_transparent(160 - (((int)(short)msgbox_w) >> 1), 100 - (((int)(short)msgbox_h) >> 1), (int)(short)msgbox_w, (int)(short)msgbox_h, *(int *)msgbox_image);
    }
    if (((int)(unsigned char)msgbox_kind) == 1) {
        if (D_00195F2E != 0) {
            D_00195F2E--;
            if (D_00195F2E == 0) goto L3F47F;
        }
        if (((struct bf8_0_1 *)&D_001940D5)->f != 0 && ((int)(unsigned char)(mouse_buttons & 3)) == 0) {
        } else if (key_pressed_once(28) == 0) {
            if (mouse_buttons == 0) return;
            if (mouse_buttons_prev != 0) return;
        }
L3F47F:;
        mouse_buttons_prev = 0;
        msgbox_close();
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        return;
    }
    if (((int)(unsigned char)msgbox_kind) == 2) {
        if ((D_0019966C = inpstr_update()) != 0) msgbox_close();
        return;
    }
    if (((int)(unsigned char)msgbox_kind) == 4 && ((struct bf8_5_1 *)&D_001940D5)->f != 0) {
        msgbox_close();
        return;
    }
    if (((int)(unsigned char)msgbox_kind) != 5) return;
    if (((struct bf8_0_1 *)&D_001940D4)->f != 0 && key_down_esc != 0) {
        msgbox_close();
        return;
    }
    button = msgbox_button_at((int)(short)mouse_x, (int)(short)mouse_y);
    if ((short)button == 0) return;
    sound_play(203, (int)player_object, 110);
    D_00196271 = button;
    msgbox_close();
}

void msgbox_close(void)
{
    if (*(int *)msgbox_image == 0) return;
    D_001940D5 &= 158;
    if (*(int *)msgbox_image != 0 && *(int *)msgbox_image != (-1751672937)) {
        mc_free(*(int *)msgbox_image, (int)D_00170D55, 950);
        *(int *)msgbox_image = -1751672937;
    }
    *(int *)msgbox_image = 0;
    msgbox_kind = 0;
    mode_pop();
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    mouse_buttons = (mouse_buttons_prev = 0);
}

void msgbox_load_borders(void)
{
    msgbox_spop_tiles[0] = disk_read_file((int)D_00170DAE, 0);
    msgbox_mpop_tiles = disk_read_file((int)D_00170DB7, 0);
    msgbox_set_border_style(0);
}

void msgbox_free_borders(void)
{
    if (msgbox_spop_tiles[0] != 0 && msgbox_spop_tiles[0] != (-1751672937)) {
        mc_free(msgbox_spop_tiles[0], (int)D_00170D55, 977);
        msgbox_spop_tiles[0] = -1751672937;
    }
    if (msgbox_mpop_tiles == 0 || msgbox_mpop_tiles == (-1751672937)) return;
    mc_free(msgbox_mpop_tiles, (int)D_00170D55, 978);
    msgbox_mpop_tiles = -1751672937;
}

void msgbox_draw_buttons(short y)
{
    if (((struct bf8_6_1 *)&D_001940D5)->f != 0) {
        text_cursor_y = y + ((((int)(short)D_00195F34) - ((int)(short)msgbox_h)) - (100 - (((int)(short)msgbox_h) >> 1)));
    } else {
        text_cursor_y = y;
    }
    if (D_00196090 == 0) {
        xn_draw_image(144, y, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)msgbox_button_ids) - 1) << 9)));
        return;
    }
    if (D_00196091 == 0) {
        xn_draw_image(112, y, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)msgbox_button_ids) - 1) << 9)));
        xn_draw_image(176, y, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)D_00196090) - 1) << 9)));
        return;
    }
    xn_draw_image(96, y, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)msgbox_button_ids) - 1) << 9)));
    xn_draw_image(144, y, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)D_00196090) - 1) << 9)));
    xn_draw_image(192, y, 32, 16, (int)(buttons_rci + ((((int)(unsigned char)D_00196091) - 1) << 9)));
}

int msgbox_button_at(short x, short y)
{
    if (D_00196090 == 0) {
        if (msgbox_button_keys != 0 && key_down[(int)(unsigned char)msgbox_button_keys] != 0) {
            return 1;
        }
        if (mouse_buttons == 0) return 0;
        if (mouse_buttons_prev != 0) return 0;
        if (mouse_buttons != 0 && x > 144 && y > text_cursor_y && x < 176 && y < text_cursor_y + 16) {
            return 1;
        }
    } else if (D_00196091 == 0) {
        if (msgbox_button_keys != 0 && key_down[(int)(unsigned char)msgbox_button_keys] != 0) {
            return 1;
        }
        if (D_00196034 != 0 && key_down[(int)(unsigned char)D_00196034] != 0) {
            return 2;
        }
        if (mouse_buttons == 0) return 0;
        if (mouse_buttons_prev != 0) return 0;
        if (mouse_buttons != 0 && x > 112 && y > text_cursor_y && x < 144 && y < text_cursor_y + 16) {
            return 1;
        }
        if (mouse_buttons != 0 && x > 176 && y > text_cursor_y && x < 208 && y < text_cursor_y + 16) {
            return 2;
        }
    } else {
        if (msgbox_button_keys != 0 && key_down[(int)(unsigned char)msgbox_button_keys] != 0) {
            return 1;
        }
        if (D_00196034 != 0 && key_down[(int)(unsigned char)D_00196034] != 0) {
            return 2;
        }
        if (D_00196035 != 0 && key_down[(int)(unsigned char)D_00196035] != 0) {
            return 3;
        }
        if (mouse_buttons == 0) return 0;
        if (mouse_buttons_prev != 0) return 0;
        if (mouse_buttons != 0 && x > 96 && y > text_cursor_y && x < 128 && y < text_cursor_y + 16) {
            return 1;
        }
        if (mouse_buttons != 0 && x > 144 && y > text_cursor_y && x < 176 && y < text_cursor_y + 16) {
            return 2;
        }
        if (mouse_buttons != 0 && x > 192 && y > text_cursor_y && x < 224 && y < text_cursor_y + 16) {
            return 3;
        }
    }
    return 0;
}

void people_spawn_tick(void)
{
    int i;

    people_max = people_max_count();
    if (people_count == 0 && people_max == 0) return;
    people_set_spawn_range();
    for (i = people_count; i < people_max; i++) {
        pedestrian_place(object_create_child(location_object, 0, 3));
    }
}

void people_update(void)
{
    int i;
    int distance;
    struct record *pedestrian;

    if (people_count == 0) return;
    people_idle_count = 0;
    for (i = 0; i < people_count; i++) {
        if (people_list[i] == 0) continue;
        pedestrian = people_list[i];
        distance = xn_math_approx_dist2d(pedestrian->x, pedestrian->z, player_object->x, player_object->z);
        if (distance > people_spawn_range) {
            if (people_count <= people_max) {
                pedestrian_place(pedestrian);
            } else {
                object_delete(pedestrian);
                people_list[i] = 0;
                continue;
            }
        }
        if (pedestrian_walk(pedestrian, distance) == 0) {
            pedestrian->image = (pedestrian->image & -128) + 5;
            if ((pedestrian->image >> 7) == 399) pedestrian->image += 10;
        }
    }
}

int pedestrian_walk(struct record *pedestrian, int player_distance)
{
    int dx;
    int dz;
    int can_stand;
    int turn_count;
    int unused;
    int retry_count;
    int facing;
    int angle_to_player;
    int heading;
    int tile_dx;
    int tile_dz;
    int speed;

    retry_count = 0;
    speed = 75;
    if (creature_count == 0 && player_distance < 192 && ((struct bf8_6_1 *)&D_001940D6)->f == 0 && D_0019628E == 0 && people_idle_count < 3 && ((int)(unsigned short)(pedestrian->npc_flags & 32768)) == 0 && player_character->race < 9) {
        people_idle_count++;
        pedestrian->image = (pedestrian->image & -128) + 5;
        if ((pedestrian->image >> 7) == 399) pedestrian->image += 10;
        return 1;
    }
    pedestrian->image &= ~0x7F;
    for (;;) {
        xn_math_yaw_offset_xz(pedestrian->yaw, ((speed << 8) * frame_ticks) / 1000, &dx, &dz);
        tile_dx = abs((pedestrian->x & 63) - 32);
        tile_dz = abs((pedestrian->z & 63) - 32);
        if (tile_dx < 8 && tile_dz < 8) {
            if ((rand() & 255) != 0) {
                can_stand = pedestrian_can_stand_at((int)(pedestrian->x + (D_0017B657[pedestrian->yaw >> 9].dx)), (D_0017B657[pedestrian->yaw >> 9].dz) + pedestrian->z);
            } else {
                can_stand = 0;
            }
        } else {
            can_stand = 1;
        }
        turn_count = 0;
        facing = pedestrian->yaw;
        if (can_stand == 0) pedestrian->yaw = D_0017B64F[(rand() % 4)];
        while (can_stand == 0) {
            pedestrian->yaw = (pedestrian->yaw + 512) % 2048;
            xn_math_yaw_offset_xz(pedestrian->yaw, (frame_ticks * 19200) / 1000, &dx, &dz);
            can_stand = pedestrian_can_stand_at((D_0017B657[pedestrian->yaw >> 9].dx) + pedestrian->x, (D_0017B657[pedestrian->yaw >> 9].dz) + pedestrian->z);
            if (turn_count++ > 8) {
                pedestrian->yaw = facing;
                return 0;
            }
        }
        dx += pedestrian->move_remainder & 255;
        dz += ((unsigned)(pedestrian->move_remainder & 65280)) >> 8;
        pedestrian->x += dx / 256;
        pedestrian->z += dz / 256;
        pedestrian->move_remainder = dx & 255;
        pedestrian->move_remainder |= (dz & 255) << 8;
        if (pedestrian->yaw == 1024 || pedestrian->yaw == 0) {
            pedestrian->x = (pedestrian->x & -64) + 32;
        } else {
            pedestrian->z = (pedestrian->z & -64) + 32;
        }
        angle_to_player = xn_math_angle_to_point(pedestrian->x, pedestrian->z, player_object->x, player_object->z);
        heading = (pedestrian->yaw + 128) & 2047;
        facing = (heading - angle_to_player) & 2047;
        facing >>= 8;
        if (facing > 4) {
            pedestrian->flags |= 0x4000;
            facing = (int)(unsigned char)anim_mirror_facing[facing];
        } else {
            pedestrian->flags &= ~0x4000;
        }
        pedestrian->y = xn_terrain_height_at(pedestrian->x, pedestrian->z);
        pedestrian->image += facing;
        if (pedestrian_clear_of_others(pedestrian, 64) != 0) break;
        pedestrian->yaw = (pedestrian->yaw + 512) % 2048;
        ++retry_count;
        if (retry_count >= 8) break;
        speed += 100;
        pedestrian->image -= facing;
    }
    return 1;
}

void pedestrian_place(struct record *pedestrian)
{
    int unused1;
    int unused2;
    int unused3;
    int spot_ok;
    int unused4;
    int tries;
    int radius;
    struct person *person;

    tries = 0;
    person = &pedestrian->data.person;
    pedestrian->type = 53;
    pedestrian->flags |= 1;
    pedestrian->home_building = rand() % current_location->building_count;
    pedestrian->id = object_new_id(((unsigned)location_object->id) >> 16);
    pedestrian->image2 = 0;
    pedestrian->npc_flags = 0;
    person->faction_id = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15)->id;
    pedestrian_pick_sprite(pedestrian);
    radius = xn_cam_far_z >> 8;
    if (radius > 1536) {
        radius = 1536;
    } else {
        radius = 1024;
    }
    do {
        xn_math_yaw_offset_xz(rand_range(0, 2047), radius, &pedestrian->x, &pedestrian->z);
        pedestrian->x += player_object->x;
        pedestrian->z += player_object->z;
        pedestrian->y = xn_terrain_height_at(pedestrian->x, pedestrian->z);
        spot_ok = pedestrian_spawn_spot_ok(pedestrian, pedestrian->x - location_object->x, pedestrian->z - location_object->z);
    } while (spot_ok == 0 && tries++ < 10);
    if (tries != 11) return;
    pedestrian->z = 0;
    pedestrian->y = pedestrian->z;
    pedestrian->x = pedestrian->y;
}

int people_max_count(void)
{
    int max_count;

    if (location_object->image == 65535) return 0;
    if (((int)player_environment) != 1) return 0;
    if (current_location->kind == 4 || current_location->kind == 7 || current_location->kind > 8) {
        return 0;
    }
    if (player_character->race > 8) return 0;
    if (creature_count != 0) return 0;
    if (location_contains(player_object->x, player_object->z) == 0) return 0;
    if (daylight == 0) return 0;
    max_count = (current_location->width * current_location->height) * 2;
    if (max_count < 8) {
        max_count = 8;
    } else if (max_count > 30) {
        max_count = 30;
    }
    if (((int)(unsigned char)climate_weathers[climate_category()]) >= 3) {
        max_count >>= 1;
    }
    return max_count;
}

void people_set_spawn_range(void)
{
    if (location_object->image == 65535) {
        people_spawn_range = 1;
        return;
    }
    people_spawn_range = 3100;
}

int player_in_town_area(void)
{
    int x;
    int z;
    int max_x;
    int max_z;

    x = player_object->x - location_object->x;
    z = player_object->z - location_object->z;
    max_x = (current_location->width << 12) + 2048;
    max_z = (current_location->height << 12) + 2048;
    if (x < (-2048) || x > max_x) return 0;
    if (z < (-2048) || z > max_z) return 0;
    return 1;
}

int pedestrian_clear_of_others(struct record *pedestrian, int min_distance)
{
    int i;
    int x;
    int z;
    int distance;

    x = pedestrian->x;
    z = pedestrian->z;
    for (i = 0; i < people_count; i++) {
        if (people_list[i] == 0 || people_list[i] == pedestrian) continue;
        distance = xn_math_approx_dist2d(x, z, people_list[i]->x, people_list[i]->z);
        if (distance < min_distance) return 0;
    }
    return 1;
}

int pedestrian_can_stand_at(int x, int z)
{
    if ((signed char)ground_tile_at(x, z) == 0) return 0;
    x -= location_object->x;
    z -= location_object->z;
    if (x < 0 || z < 0) return 0;
    if ((current_location->width << 12) <= x || (current_location->height << 12) <= z) return 0;
    return town_map_area_clear(x, z);
}

int pedestrian_spawn_spot_ok(struct record *pedestrian, int x, int z)
{
    int angle_to_pedestrian;
    int angle_diff;
    int turn_direction;

    if (x < 0 || z < 0) return 0;
    if ((current_location->width << 12) <= x || (current_location->height << 12) <= z) return 0;
    if ((signed char)ground_tile_at(x + location_object->x, z + location_object->z) == 0) return 0;
    if (town_map_area_clear(x, z) == 0) return 0;
    if (pedestrian_clear_of_others(pedestrian, 64) == 0) return 0;
    if (collide_creature_near((int)pedestrian, (int)&pedestrian->x) != 0) return 0;
    angle_to_pedestrian = xn_math_angle_to_point(player_object->x, player_object->z, pedestrian->x, pedestrian->z);
    angle_diff = ai_angle_diff(player_object->yaw, angle_to_pedestrian, &turn_direction);
    if (angle_diff > 400) return 1;
    return town_map_line_blocked(x, z);
}

int town_map_line_blocked(int x, int y)
{
    int player_x;
    int player_y;
    int dx;
    int dy;
    int step_count;
    int i;
    float step_x;
    float step_y;
    unsigned char pixel;

    x >>= 6;
    y >>= 6;
    y = ((current_location->height << 6) - y) - 1;
    player_x = player_object->x - location_object->x;
    player_y = player_object->z - location_object->z;
    player_x >>= 6;
    player_y >>= 6;
    player_y = ((current_location->height << 6) - player_y) - 1;
    dx = x - player_x;
    dy = y - player_y;
    step_count = ((abs(dx) > abs(dy)) ? abs(dx) : abs(dy));
    step_x = (double)dx / step_count;
    step_y = (double)dy / step_count;
    for (i = 0; (step_count + 1) > i; i++) {
        pixel = *(signed char *)((char *)(int)(*(char **)&D_00196DA4 + (((current_location->width << 6) * player_y) + player_x)));
        if (pixel != 0 && ((int)(unsigned char)pixel) < 100) return 1;
        player_x += step_x;
        player_y += step_y;
    }
    return 0;
}

int guards_are_present(void)
{
    int i;

    for (i = 0; i < creature_count; i++) {
        if ((creature_list[i]->image >> 7) == 399 && creature_list[i]->data.character.team == 1) {
            return 1;
        }
    }
    return 0;
}

int is_guard_sprite(struct record *object)
{
    return (((object->image >> 7) == 399) ? 1 : 0);
}

int creatures_guard_mix(void)
{
    int non_guard_count;
    int i;

    if (creature_count == 0) return 0;
    if (((int)player_environment) != 1) return 2;
    i = 0;
    non_guard_count = i;
    for (; i < creature_count; i++) {
        if (creature_list[i]->data.character.mobile_id != 146) non_guard_count++;
    }
    if (non_guard_count == 0) return 1;
    if (non_guard_count != creature_count) return 3;
    return 2;
}

void guard_spawn(struct record *pedestrian)
{
    struct record *guard;

    guard = object_create_child(player_object->parent, 0, 659);
    if (creature_count > 10) {
        object_delete(guard);
        return;
    }
    guard->type = 18;
    guard->flags |= 1;
    guard->id = object_new_id(((unsigned)location_object->id) >> 16);
    monster_init(guard, 146);
    guard->data.character.team = D_0017B6CD;
    if (pedestrian != 0) {
        guard->x = pedestrian->x;
        guard->y = pedestrian->y;
        guard->z = pedestrian->z;
    } else if (spawn_find_point(guard, 512, 2048) == 0) {
        object_delete(guard);
        guard = 0;
    }
    scratch_current_object = guard;
}

void guards_timer_tick(void)
{
    if (guards_timer < 0) return;
    if (guards_timer > 0) {
        guards_timer--;
        return;
    }
    guards_timer--;
    guards_summon(1);
}
