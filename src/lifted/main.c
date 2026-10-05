/* main.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_0_4 { unsigned char f:4; };
struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
extern int xn_cam_pitch;
extern int xn_cam_yaw;
extern int xn_cam_roll;
extern int xn_cam_x;
extern int xn_cam_y;
extern int xn_cam_z;
extern short xn_world_nature_archive;
extern short xn_world_ground_archive;
extern signed char D_0012B508;
extern int dungeon_water_level;
extern signed char xn_tex_cache_full;
extern char xn_cam_rotation[];
extern char xn_cam_view_matrix[];
extern int xn_model_queue_count;
extern char D_001700A0[];
extern int D_001788E8[];
extern int D_00178968[];
extern unsigned char player_environment;
extern int D_001845C8;
extern signed char D_00187CA8;
extern int D_0018DBF8;
extern int frame_checkpoint;
extern int screen_shake;
extern int D_0018DC08;
extern int D_0018DC10;
extern int screen_shake_angle;
extern int screen_shake_timer;
extern int screen_shake_signs;
extern int D_0018DC2C;
extern int D_0018DC30;
extern signed char player_motion_flags;
extern char frame_counter[];
extern int view_look_pitch;
extern int view_look_yaw;
extern struct record *D_00195A88;
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern int frame_ticks;
extern struct record *inv_right_container;
extern struct record *inv_right_container_base;
extern int hud_message_expiry[];
extern struct character *player_character;
extern int hud_message_ptrs[];
extern int ceiling_height;
extern int D_00195CEC;
extern int head_bob_offset;
extern int player_death_timer;
extern char D_001960D9[];
extern unsigned char D_0019626F;
extern signed char game_mode;
extern signed char player_on_ground;
extern signed char in_dungeon_water;
extern signed char D_001962A0;
extern signed char model_cache_flush_count;
extern short ground_texture_archive;
extern short nature_texture_archive;

extern struct record *object_reparent(struct record *, struct record *);
extern int rand();
extern int abs();
extern int xn_world_update();
extern int xn_sky_snow_update_speeds();
extern int xn_math_mul_sin();
extern int xn_render_begin_frame();
extern int xn_render_frame();
extern int xn_font_select();
extern int xn_water_draw();
extern int xn_tex_cache_flush();
extern int xn_tex_cache_begin_frame();
extern int xn_light_reset();
extern int xn_mat_from_angles();
extern int xn_cam_scale_matrix();
extern int xn_terrain_draw();
extern void player_frame_update(void);
extern void marquee_update(void);
extern void talk_update(void);
extern void tavern_frame(void);
extern void court_frame(void);
extern void creatures_find_nearest(void);
extern void loan_collectors_update(void);
extern void damage_expire_drain_bonuses(void);
extern void quest_prompt_answer(void);
extern void quest_faces_draw(void);
extern void sky_update(void);
extern void weather_draw_precipitation(void);
extern void spellmaker_update(void);
extern void sheet_update(void);
extern void training_update(void);
extern void levelup_check(void);
extern void people_spawn_tick(void);
extern void people_update(void);
extern void guards_timer_tick(void);
extern void spellbook_frame(void);
extern void status_show(int);
extern void options_frame(void);
extern void kludge_menu_update(void);
extern void time_update_realtime(void);
extern void quests_run_all(void);
extern void quest_start_pending(void);
extern void fatal_error(int);
extern void update_fog(void);
extern void itemmaker_update(void);
extern void book_update(void);
extern void cast_anim_update(void);
extern void spell_hud_draw_icons(void);
extern void hud_draw(void);
extern void magic_items_frame(void);
extern void ai_update_creatures(void);
extern void links_update(void);
extern void effects_tick(void);
extern void trade_haggle_frame(void);
extern void sound_update_channels(void);
extern void music_update(void);
extern void mem_check_crt_heap(int);
extern void logbook_update(void);
extern void bank_frame(void);
extern void guild_check_invitations(void);
extern void rest_update(void);
extern void breath_update(void);
extern void weapon_player_update(void);
extern void shelf_book_list_update(void);
extern void repair_menu_frame(void);
extern void coven_menu_frame(void);
extern void service_menu_frame(void);
extern void ambient_dungeon_sounds(void);
extern void footstep_sounds(void);
extern void head_bob_update(void);
extern void music_choose_song(void);
extern void ambient_outdoor_sounds(void);
extern void hud_messages_draw(void);
extern void info_popup_update(void);
extern void world_collect_objects(void);
extern void frame_ticks_update(void);
extern void arrival_room_messages(void);
extern void world_draw_objects(void);
extern void world_update_location(void);
extern void spfx_popup_update(void);
extern void potionmaker_update(void);
extern void inventory_frame(void);
extern void doors_update(void);
extern void travel_map_update(void);
void world_render(void);
void screen_shake_offset(int *, int *);

void game_frame(void)
{
    int reset_container;
    int unused;
    int unused2;
    int elapsed;
    int *ticks_addr;
    int unused3;

    mem_check_crt_heap(98);
    music_choose_song();
    if (((struct bf8_0_4 *)&frame_counter)->f == 0) music_update();
    frame_checkpoint = 99;
    frame_ticks_update();
    xn_sky_snow_update_speeds();
    frame_checkpoint = 100;
    update_fog();
    head_bob_update();
    player_entity->x = player_object->x;
    player_entity->y = player_object->y;
    player_entity->z = player_object->z;
    if ((int)D_00195A88 != 0 && player_on_ground != 0 && (int)player_object->parent != (int)D_00195A88) {
        object_reparent(D_00195A88, player_object);
    }
    ticks_addr = (int *)1132;
    elapsed = *ticks_addr - D_0018DC30;
    frame_checkpoint = 101;
    links_update();
    frame_checkpoint = 102;
    sky_update();
    frame_checkpoint = 103;
    world_render();
    frame_checkpoint = 104;
    if (D_00187CA8 != 0) {
        world_collect_objects();
        people_spawn_tick();
    }
    frame_checkpoint = 107;
    D_0012B508 = 146;
    xn_font_select(4);
    if (((int)D_0019626F) != 4 || ((int)(unsigned char)game_mode) != 8) {
        reset_container = 1;
    } else {
        reset_container = 0;
    }
    if (reset_container != 0 && ((int)(unsigned char)game_mode) != 4) {
        inv_right_container = (struct record *)D_001960D9;
        inv_right_container_base = (struct record *)D_001960D9;
    }
    loan_collectors_update();
    weather_draw_precipitation();
    cast_anim_update();
    ambient_outdoor_sounds();
    guards_timer_tick();
    footstep_sounds();
    time_update_realtime();
    frame_checkpoint = 109;
    spell_hud_draw_icons();
    hud_draw();
    frame_checkpoint = 110;
    if (D_00187CA8 != 0) {
        ai_update_creatures();
        frame_checkpoint = 111;
        people_update();
        frame_checkpoint = 112;
    }
    quests_run_all();
    frame_checkpoint = 113;
    kludge_menu_update();
    potionmaker_update();
    spellmaker_update();
    sheet_update();
    inventory_frame();
    status_show(0);
    spellbook_frame();
    itemmaker_update();
    book_update();
    frame_checkpoint = 114;
    logbook_update();
    bank_frame();
    trade_haggle_frame();
    court_frame();
    levelup_check();
    travel_map_update();
    spfx_popup_update();
    tavern_frame();
    repair_menu_frame();
    coven_menu_frame();
    service_menu_frame();
    shelf_book_list_update();
    magic_items_frame();
    options_frame();
    frame_checkpoint = 115;
    damage_expire_drain_bonuses();
    talk_update();
    training_update();
    info_popup_update();
    marquee_update();
    frame_checkpoint = 116;
    ambient_dungeon_sounds();
    quest_faces_draw();
    quest_start_pending();
    quest_prompt_answer();
    sound_update_channels();
    guild_check_invitations();
    frame_checkpoint = 117;
    if (((struct bf8_4_1 *)&player_motion_flags)->f != 0) {
        hud_message_ptrs[0] = D_001845C8;
        hud_message_expiry[0] = 2;
    }
    creatures_find_nearest();
    effects_tick();
    player_frame_update();
    rest_update();
    weapon_player_update();
    breath_update();
    hud_messages_draw();
    arrival_room_messages();
    frame_checkpoint = 118;
}

void world_render(void)
{
    int alive;
    int eye_height;
    int unused;
    int unused2;
    int unused3;
    int shake_pitch;
    int shake_yaw;
    int incomplete;
    int retries;

    retries = 0;
    if (D_00187CA8 == 0) return;
    model_cache_flush_count = 0;
    doors_update();
    for (;;) {
        xn_cam_x = player_object->x;
        xn_cam_z = player_object->z;
        if (((struct bf8_2_1 *)&player_motion_flags)->f != 0) {
            eye_height = 35;
        } else {
            eye_height = 72;
        }
        xn_cam_y = (player_object->y - eye_height) - head_bob_offset;
        if ((player_character->flags & 1536) != 0) xn_cam_y -= 50;
        if (player_death_timer > 0) {
            xn_cam_y = ((((xn_cam_y - (player_object->y - 20)) * player_death_timer) / 1000) + player_object->y) - 20;
        }
        if (D_001962A0 != 0) {
            if (player_death_timer <= 0) {
                alive = 1;
            } else {
                alive = 0;
            }
            if (alive != 0) goto L10C14;
        }
        goto L10C1B;
L10C14:;
        xn_cam_y += 50;
L10C1B:;
        xn_cam_pitch = (camera_object->angle_x + view_look_pitch) & 2047;
        xn_cam_yaw = (camera_object->yaw + view_look_yaw) & 2047;
        xn_cam_roll = camera_object->angle_z;
        if (in_dungeon_water != 0 || D_001962A0 != 0) {
            D_00195CEC += (frame_ticks << 11) / 1000;
            D_00195CEC &= 8191;
            if (in_dungeon_water != 0 && (ceiling_height + 5) < xn_cam_y) {
                xn_cam_y += (D_001788E8[(D_00195CEC >> 8)] * 2) - 3;
            }
            xn_cam_roll += D_00178968[(D_00195CEC >> 8)];
        }
        if (dungeon_water_level != 10000 && abs(xn_cam_y - dungeon_water_level) < 5) {
            xn_cam_y -= 10;
        }
        screen_shake_offset(&shake_pitch, &shake_yaw);
        xn_cam_pitch += shake_pitch;
        xn_cam_pitch &= 2047;
        xn_cam_yaw += shake_yaw;
        xn_cam_yaw &= 2047;
        frame_checkpoint = 200;
        xn_tex_cache_begin_frame();
        xn_render_begin_frame();
        xn_light_reset();
        xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (int)xn_cam_rotation);
        xn_cam_scale_matrix((int)xn_cam_rotation, (int)xn_cam_view_matrix);
        frame_checkpoint = 201;
        world_draw_objects();
        frame_checkpoint = 202;
        if (model_cache_flush_count != 0) continue;
        if (((int)player_environment) == 1) {
            xn_world_update();
            world_update_location();
            if (model_cache_flush_count != 0) continue;
            xn_world_ground_archive = ground_texture_archive;
            xn_world_nature_archive = nature_texture_archive;
            frame_checkpoint = 203;
            xn_terrain_draw();
            frame_checkpoint = 204;
        }
        D_0018DC10 = xn_model_queue_count;
        frame_checkpoint = 205;
        if (((int)player_environment) == 1) {
            incomplete = xn_render_frame(2);
        } else {
            incomplete = xn_render_frame(0);
        }
        frame_checkpoint = 206;
        D_0018DC08 = xn_model_queue_count;
        if (incomplete == 0) if (xn_tex_cache_full == 0) break;
        ++retries;
        if (retries != 1) fatal_error((int)D_001700A0);
        xn_tex_cache_full = 0;
        xn_tex_cache_flush();
    }
    if (dungeon_water_level == 10000 || xn_cam_y == dungeon_water_level) {
        return;
    }
    xn_water_draw();
}

void screen_shake_offset(int *pitch, int *yaw)
{
    *pitch = 0;
    *yaw = 0;
    if (screen_shake == 0) {
        *yaw = 0;
        *pitch = *yaw;
        return;
    }
    screen_shake_timer -= frame_ticks;
    if (screen_shake_timer > 0) {
        *pitch = D_0018DC2C;
        *yaw = D_0018DBF8;
        return;
    }
    screen_shake_timer = 60;
    screen_shake -= 2;
    screen_shake_angle += 128;
    screen_shake_angle &= 2047;
    *pitch = xn_math_mul_sin(screen_shake, screen_shake_angle);
    *yaw = *pitch;
    if (((struct bf8_0_1 *)&screen_shake_signs)->f != 0) *pitch = -(*pitch);
    if (((struct bf8_1_1 *)&screen_shake_signs)->f != 0) *yaw = -(*yaw);
    D_0018DC2C = *pitch;
    D_0018DBF8 = *yaw;
}

void screen_shake_start(int strength)
{
    screen_shake = strength / 2;
    if (((struct bf8_0_1 *)&screen_shake)->f != 0) (screen_shake)++;
    screen_shake_signs = rand();
    screen_shake_angle = 0;
    screen_shake_timer = 60;
}

void screen_shake_stop(void)
{
    screen_shake = 0;
}
