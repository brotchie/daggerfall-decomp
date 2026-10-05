/* main.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_0_4 { unsigned char f:4; };
struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
extern int D_000C23B8;
extern int D_000C23BC;
extern int D_000C23C0;
extern int D_000C23C4;
extern int D_000C23C8;
extern int D_000C23CC;
extern short D_000C287D;
extern short D_000C287F;
extern signed char D_0012B508;
extern int dungeon_water_level;
extern signed char D_00132F58;
extern char D_00136E00[];
extern char D_00136E24[];
extern int D_0013F76C;
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
extern int D_0018DC14;
extern int D_0018DC18;
extern int D_0018DC28;
extern int D_0018DC2C;
extern int D_0018DC30;
extern signed char player_motion_flags;
extern char frame_counter[];
extern int view_look_pitch;
extern int D_001959BC;
extern struct record *D_00195A88;
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern int D_00195AB0;
extern struct record *inv_right_container;
extern struct record *D_00195B34;
extern int hud_message_expiry[];
extern struct character *player_character;
extern int hud_message_ptrs[];
extern int D_00195C74;
extern int D_00195CEC;
extern int head_bob_offset;
extern int player_death_timer;
extern char D_001960D9[];
extern unsigned char D_0019626F;
extern signed char game_mode;
extern signed char player_on_ground;
extern signed char in_dungeon_water;
extern signed char D_001962A0;
extern signed char D_001A949D;
extern short ground_texture_archive;
extern short nature_texture_archive;

extern struct record *object_reparent(struct record *, struct record *);
extern int rand();
extern int func_0009DEAC();
extern int func_000C2E05();
extern int func_000C9C70();
extern int func_000CE6D4();
extern int func_0012A4F0();
extern int func_0012A870();
extern int func_0012DB50();
extern int func_0012F79C();
extern int func_00135E39();
extern int func_00135E90();
extern int func_00136AB4();
extern int func_00137000();
extern int func_00137725();
extern int func_0013E600();
extern void player_frame_update(void);
extern void func_00013C56(void);
extern void talk_update(void);
extern void tavern_frame(void);
extern void court_frame(void);
extern void creatures_find_nearest(void);
extern void loan_collectors_update(void);
extern void func_0002F992(void);
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
extern void func_000685EC(void);
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
void screen_shake_offset(int, int);

void game_frame(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    mem_check_crt_heap(98);
    music_choose_song();
    if (((struct bf8_0_4 *)&frame_counter)->f == 0) music_update();
    frame_checkpoint = 99;
    frame_ticks_update();
    func_000C9C70();
    frame_checkpoint = 100;
    update_fog();
    head_bob_update();
    player_entity->x = player_object->x;
    player_entity->y = player_object->y;
    player_entity->z = player_object->z;
    if ((int)D_00195A88 != 0 && player_on_ground != 0 && (int)player_object->parent != (int)D_00195A88) {
        object_reparent(D_00195A88, player_object);
    }
    l_1C = 1132;
    l_20 = *(int *)((char *)l_1C) - D_0018DC30;
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
    func_0012DB50(4);
    if (((int)D_0019626F) != 4 || ((int)(unsigned char)game_mode) != 8) {
        l_2C = 1;
    } else {
        l_2C = 0;
    }
    if (l_2C != 0 && ((int)(unsigned char)game_mode) != 4) {
        inv_right_container = (struct record *)D_001960D9;
        D_00195B34 = (struct record *)D_001960D9;
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
    func_000685EC();
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
    func_0002F992();
    talk_update();
    training_update();
    info_popup_update();
    func_00013C56();
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
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = 0;
    if (D_00187CA8 == 0) return;
    D_001A949D = 0;
    doors_update();
    for (;;) {
        D_000C23C4 = player_object->x;
        D_000C23CC = player_object->z;
        if (((struct bf8_2_1 *)&player_motion_flags)->f != 0) {
            l_34 = 35;
        } else {
            l_34 = 72;
        }
        D_000C23C8 = (player_object->y - l_34) - head_bob_offset;
        if ((player_character->flags & 1536) != 0) D_000C23C8 -= 50;
        if (player_death_timer > 0) {
            D_000C23C8 = ((((D_000C23C8 - (player_object->y - 20)) * player_death_timer) / 1000) + player_object->y) - 20;
        }
        if (D_001962A0 != 0) {
            if (player_death_timer <= 0) {
                l_38 = 1;
            } else {
                l_38 = 0;
            }
            if (l_38 != 0) goto L10C14;
        }
        goto L10C1B;
L10C14:;
        D_000C23C8 += 50;
L10C1B:;
        D_000C23B8 = (camera_object->angle_x + view_look_pitch) & 2047;
        D_000C23BC = (camera_object->yaw + D_001959BC) & 2047;
        D_000C23C0 = camera_object->angle_z;
        if (in_dungeon_water != 0 || D_001962A0 != 0) {
            D_00195CEC += (D_00195AB0 << 11) / 1000;
            D_00195CEC &= 8191;
            if (in_dungeon_water != 0 && (D_00195C74 + 5) < D_000C23C8) {
                D_000C23C8 += (D_001788E8[(D_00195CEC >> 8)] * 2) - 3;
            }
            D_000C23C0 += D_00178968[(D_00195CEC >> 8)];
        }
        if (dungeon_water_level != 10000 && func_0009DEAC(D_000C23C8 - dungeon_water_level) < 5) {
            D_000C23C8 -= 10;
        }
        screen_shake_offset((int)&l_24, (int)&l_20);
        D_000C23B8 += l_24;
        D_000C23B8 &= 2047;
        D_000C23BC += l_20;
        D_000C23BC &= 2047;
        frame_checkpoint = 200;
        func_00135E90();
        func_0012A4F0();
        func_00136AB4();
        func_00137000(D_000C23B8, D_000C23BC, D_000C23C0, (int)D_00136E00);
        func_00137725((int)D_00136E00, (int)D_00136E24);
        frame_checkpoint = 201;
        world_draw_objects();
        frame_checkpoint = 202;
        if (D_001A949D != 0) continue;
        if (((int)player_environment) == 1) {
            func_000C2E05();
            world_update_location();
            if (D_001A949D != 0) continue;
            D_000C287F = ground_texture_archive;
            D_000C287D = nature_texture_archive;
            frame_checkpoint = 203;
            func_0013E600();
            frame_checkpoint = 204;
        }
        D_0018DC10 = D_0013F76C;
        frame_checkpoint = 205;
        if (((int)player_environment) == 1) {
            l_1C = func_0012A870(2);
        } else {
            l_1C = func_0012A870(0);
        }
        frame_checkpoint = 206;
        D_0018DC08 = D_0013F76C;
        if (l_1C == 0) if (D_00132F58 == 0) break;
        ++l_18;
        if (l_18 != 1) fatal_error((int)D_001700A0);
        D_00132F58 = 0;
        func_00135E39();
    }
    if (dungeon_water_level == 10000 || D_000C23C8 == dungeon_water_level) {
        return;
    }
    func_0012F79C();
}

void screen_shake_offset(int a1, int a2)
{
    *(int *)((char *)a1) = 0;
    *(int *)((char *)a2) = 0;
    if (screen_shake == 0) {
        *(int *)((char *)a2) = 0;
        *(int *)((char *)a1) = *(int *)((char *)a2);
        return;
    }
    D_0018DC18 -= D_00195AB0;
    if (D_0018DC18 > 0) {
        *(int *)((char *)a1) = D_0018DC2C;
        *(int *)((char *)a2) = D_0018DBF8;
        return;
    }
    D_0018DC18 = 60;
    screen_shake -= 2;
    D_0018DC14 += 128;
    D_0018DC14 &= 2047;
    *(int *)((char *)a1) = func_000CE6D4(screen_shake, D_0018DC14);
    *(int *)((char *)a2) = *(int *)((char *)a1);
    if (((struct bf8_0_1 *)&D_0018DC28)->f != 0) *(int *)((char *)a1) = -(*(int *)((char *)a1));
    if (((struct bf8_1_1 *)&D_0018DC28)->f != 0) *(int *)((char *)a2) = -(*(int *)((char *)a2));
    D_0018DC2C = *(int *)((char *)a1);
    D_0018DBF8 = *(int *)((char *)a2);
}

void screen_shake_start(int a1)
{
    screen_shake = a1 / 2;
    if (((struct bf8_0_1 *)&screen_shake)->f != 0) (screen_shake)++;
    D_0018DC28 = rand();
    D_0018DC14 = 0;
    D_0018DC18 = 60;
}

void screen_shake_stop(void)
{
    screen_shake = 0;
}
