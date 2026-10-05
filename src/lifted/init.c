/* init.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_5 { unsigned char f:5; };
extern char disk_last_file_size[];
extern int D_000C23C4;
extern int D_000C23C8;
extern int D_000C23CC;
extern int D_000CDE2F;
extern int D_000CEA20;
extern int D_000CEA24;
extern signed char D_0012B508;
extern int dungeon_water_level;
extern int D_00136911;
extern int D_0014CC0C;
extern char D_00175040[];
extern char D_00175047[];
extern char D_00175207[];
extern char D_00175214[];
extern char D_0017521C[];
extern char D_00175226[];
extern char D_0017523C[];
extern char D_00175253[];
extern char D_00175260[];
extern char D_00175266[];
extern char D_00175281[];
extern char D_001752B6[];
extern char D_001752D2[];
extern char D_00175307[];
extern char D_00175313[];
extern char D_00175324[];
extern char D_00175331[];
extern char D_0017533E[];
extern char D_0017534B[];
extern char D_00175358[];
extern char D_00175365[];
extern short D_001788D3[];
extern unsigned char player_environment;
extern int mem_check_level;
extern int frame_checkpoint;
extern int D_0018DC24;
extern char saved_positions[];
extern signed char region_event_values[];
extern char region_price_adjustment[];
extern signed char text_buffer[];
extern struct record *D_00190504[];
extern char D_00190704[];
extern int hud_compass_image;
extern int D_00190908;
extern int D_0019090C;
extern int D_00190910;
extern char D_001917E4[];
extern char D_00191834[];
extern char flats_cfg[];
extern unsigned char D_001940D7;
extern signed char D_001940D8;
extern int horse_overlay_image;
extern int cart_overlay_image;
extern int D_00195998;
extern struct record *nonworld_root;
extern struct record *logbook_object;
extern struct record *options_object;
extern struct record *inventory_containers[];
extern struct record *D_001959DC;
extern struct record *D_001959E0;
extern struct record *D_001959E4;
extern struct record *D_001959EC;
extern struct record *D_001959F0;
extern struct record *D_001959F4;
extern struct record *D_001959F8;
extern struct record *D_00195A00;
extern struct record *bank_accounts;
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern int D_00195AC8;
extern int D_00195AD0;
extern int picklist_image;
extern struct spell *spell_records;
extern int creature_count;
extern int current_region_data;
extern int D_00195B64;
extern char *hud_bar_image;
extern int hud_mode_icons;
extern int D_00195B74;
extern char D_00195B78[];
extern int icon_image;
extern char *buttons_rci;
extern struct character *player_character;
extern int window_image;
extern struct career *player_class;
extern int game_minutes;
extern struct settings *game_settings;
extern int D_00195C44;
extern signed char cfg_map_file;
extern int D_00195C7C;
extern int D_00195C80;
extern int D_00195C84;
extern int flats_cfg_count;
extern int spell_cast_anim_fire[];
extern int spell_cast_anim_frost;
extern int spell_cast_anim_poison;
extern int spell_cast_anim_shock;
extern int spell_cast_anim_magic;
extern int D_00195CF4;
extern int D_00195CF8;
extern int D_00195D04[];
extern int D_00195D18;
extern int D_00195D20;
extern int D_00195D24;
extern int magic_def;
extern int D_00195D74;
extern int books_path;
extern int compass_image;
extern int compass_box_image;
extern int qbn_opcode_arg_counts;
extern signed char climate_weathers[];
extern signed char view_cursor_active;
extern signed char D_0019621B;
extern int D_0019625E;
extern signed char current_region;
extern signed char player_underwater;
extern signed char D_00196283;
extern signed char exiting;
extern int maps_bsa;
extern int D_001997F0;
extern int D_001997F4;
extern int D_001997FC;
extern int D_00199800;
extern int D_00199804;
extern int D_00199808;
extern char D_001A3F60[];
extern int D_001A3F7C;
extern int D_001A3F90;
extern int D_001A3F9C;
extern char cfg_mapsave_file[];
extern int cfg_start_map;
extern signed char cfg_region;
extern int arch3d_bsa;
extern int dagger_snd;
extern int D_001AA5FC;

extern int climate_category(void);
extern int disk_read_file(int, int);
extern int rand_range(int, int);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int marker_count(struct record *, int);
extern int player_to_nearest_marker(struct record *, int);
extern int func_0009DA1C(int, int);
extern int printf(int, ...);
extern int exit();
extern int open(int, ...);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int mc_strncpy();
extern int atoi();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int unlink();
extern int func_000A148C(int, ...);
extern int func_000A18C3(int);
extern int func_000C0100();
extern int func_000C0580();
extern int func_000C2D00();
extern int func_000C310F();
extern int func_000C9F29();
extern int func_000CB300();
extern int func_000CB552();
extern int func_000CE69A();
extern int func_000CE88D();
extern int func_000CE8A0();
extern int func_000CE918();
extern int func_000CE957();
extern int func_0012A100();
extern int func_0012A230();
extern int func_0012A254();
extern int func_0012A274();
extern int func_0012A2D0();
extern int func_0012B100();
extern int func_0012D851();
extern int func_0012DB00();
extern int func_00136A00();
extern int func_00136B88();
extern int func_00142700();
extern int func_00142764();
extern int func_00143600();
extern int func_00143700();
extern int func_00143914();
extern int func_00149E00();
extern int func_00149EF8();
extern int func_0014D23C();
extern int func_00152BA8();
extern void archive_close(int);
extern void faction_load_file(void);
extern void faction_free(void);
extern void region_free_tables(void);
extern void region_enter(unsigned char, unsigned char);
extern void automap_load(void);
extern void sky_free(void);
extern void func_0003D3F0(void);
extern void func_0003F699(void);
extern void kludge_print_build(int);
extern void calendar_update(void);
extern void paperdoll_draw(int, int);
extern void text_draw(int, int, int);
extern void func_00068B1B(void);
extern void mem_check_crt_heap(int);
extern void disk_copy_file(int, int, int);
extern void file_index_build(void);
extern void file_index_free(void);
extern void weapon_reload_hand_sprites(void);
extern void weapon_free_sprites(void);
extern void func_0007EF20(void);
extern void intrface_free(void);
extern void player_compute_jump_velocity(void);
extern void model_heap_free_all(void);
extern void sound_cache_free_all(void);
extern void region_unload(void);
extern void dungeon_load(int);
extern void location_unload(unsigned short);
extern void map_goto_location(int, int, int, int);
extern void object_heap_init(void);
extern void object_heap_shutdown(void);
extern void object_free_children(struct record *);
extern void inv_reset_left_list(void);
extern void dpmi_get_free_memory(int);
int func_00050607(int);
int func_0005065A(int);
void player_refresh_paperdoll(void);
void shutdown_free_all(void);
void func_0004FEEA(void);
void palette_restore(void);
void game_shutdown(void);
void newgame_init_player(void);
void character_reset_magicka(struct character *, struct career *);
void init_player_records(struct record *);
void spell_cast_anims_free(void);
#pragma aux func_0009DA1C parm routine [];
#pragma aux func_000A0ED9 parm routine [];
#pragma aux func_000A18C3 parm routine [];

void func_0004ECAE(void)
{
    int l_18;

    l_18 = 1132;
    if (((struct bf8_0_5 *)((char *)l_18))->f == 0) {
        dpmi_get_free_memory((int)D_001A3F60);
        D_001A3F9C = D_001A3F90 - (D_001A3F7C << 2);
    }
    func_000A0ED9(47, (int)D_00175040);
    mc_sprintf((int)text_buffer, (int)D_00175047, D_001A3F9C);
    D_0012B508 = 146;
    text_draw((int)text_buffer, 246, 23);
}

void player_refresh_paperdoll(void)
{
    D_001940D8 |= 8;
    mem_check_crt_heap(0);
    func_000CB552((window_image = disk_read_file((int)D_00175207, 0)));
    paperdoll_draw(0, 0);
    func_00143914(0);
    if (window_image == 0 || window_image == (-1751672937)) return;
    mc_free(window_image, (int)D_00175040, 246);
    window_image = -1751672937;
}

void init_world_objects(void)
{
    object_heap_init();
    D_00195AD0 = (int)D_00195AC4;
    init_player_records(D_00195AC4);
    inv_reset_left_list();
    calendar_update();
}

void newgame_place_player(void)
{
    int l_1C;
    int l_18;

    D_00195AC4->image = 65535;
    if (((int)(unsigned char)cfg_map_file) == 100) {
        player_environment = 3;
    } else {
        player_environment = 1;
    }
    current_region = cfg_region;
    current_region_data = ((int)region_event_values) + (((int)(unsigned char)current_region) * 80);
    region_enter(0, (int)(unsigned char)current_region);
    dungeon_water_level = 10000;
    if (((int)player_environment) == 1) {
        map_goto_location((int)(unsigned char)cfg_region, (int)player_environment, cfg_start_map, 0);
    } else {
        map_goto_location((int)(unsigned char)cfg_region, 1, cfg_start_map, 0);
        player_to_nearest_marker(D_00195AC4, 8);
        dungeon_load(-1);
        automap_load();
        if (marker_count(D_00195AC4, 6) != 0) player_to_nearest_marker(D_00195AC4, 6);
    }
    D_000C23C4 = player_object->x;
    D_000C23CC = player_object->z;
    D_000C23C8 = player_object->y;
    view_cursor_active = 0;
    player_character->health = player_character->max_health;
}

void shutdown_free_all(void)
{
    int l_1C;
    int l_18;

    func_00152BA8();
    func_0012A230();
    archive_close(arch3d_bsa);
    archive_close(maps_bsa);
    archive_close(dagger_snd);
    archive_close(D_00195AC8);
    for (l_1C = 0; ((int)(short)*(short *)&l_1C) < 127; l_1C++) {
        if (*(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)) != 0) {
            if (*(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)) != 0 && *(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)) != (-1751672937)) {
                mc_free(*(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)), (int)D_00175040, 314);
                *(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)) = -1751672937;
            }
        }
    }
    func_000CE88D(D_001997FC, D_00199800);
    if (qbn_opcode_arg_counts != 0 && qbn_opcode_arg_counts != (-1751672937)) {
        mc_free(qbn_opcode_arg_counts, (int)D_00175040, 319);
        qbn_opcode_arg_counts = -1751672937;
    }
    region_free_tables();
    region_unload();
    model_heap_free_all();
    object_heap_shutdown();
    intrface_free();
    func_0007EF20();
    if (*(int *)D_00195B78 != 0 && *(int *)D_00195B78 != (-1751672937)) {
        mc_free(*(int *)D_00195B78, (int)D_00175040, 326);
        *(int *)D_00195B78 = -1751672937;
    }
    if (D_001AA5FC != 0 && D_001AA5FC != (-1751672937)) {
        mc_free(D_001AA5FC, (int)D_00175040, 327);
        D_001AA5FC = -1751672937;
    }
    if ((int)spell_records != 0 && (int)spell_records != (-1751672937)) {
        mc_free((int)spell_records, (int)D_00175040, 328);
        spell_records = (struct spell *)-1751672937;
    }
    if ((int)buttons_rci != 0 && (int)buttons_rci != (-1751672937)) {
        mc_free((int)buttons_rci, (int)D_00175040, 329);
        *(int *)&buttons_rci = -1751672937;
    }
    if (icon_image != 0 && icon_image != (-1751672937)) {
        mc_free(icon_image, (int)D_00175040, 330);
        icon_image = -1751672937;
    }
    if (D_00195C7C != 0 && D_00195C7C != (-1751672937)) {
        mc_free(D_00195C7C, (int)D_00175040, 331);
        D_00195C7C = -1751672937;
    }
    if (D_00195C80 != 0 && D_00195C80 != (-1751672937)) {
        mc_free(D_00195C80, (int)D_00175040, 332);
        D_00195C80 = -1751672937;
    }
    if (D_00195C84 != 0 && D_00195C84 != (-1751672937)) {
        mc_free(D_00195C84, (int)D_00175040, 333);
        D_00195C84 = -1751672937;
    }
    if (hud_compass_image != 0 && hud_compass_image != (-1751672937)) {
        mc_free(hud_compass_image, (int)D_00175040, 334);
        hud_compass_image = -1751672937;
    }
    if (D_00190908 != 0 && D_00190908 != (-1751672937)) {
        mc_free(D_00190908, (int)D_00175040, 335);
        D_00190908 = -1751672937;
    }
    if (D_0019090C != 0 && D_0019090C != (-1751672937)) {
        mc_free(D_0019090C, (int)D_00175040, 336);
        D_0019090C = -1751672937;
    }
    if (D_00190910 != 0 && D_00190910 != (-1751672937)) {
        mc_free(D_00190910, (int)D_00175040, 337);
        D_00190910 = -1751672937;
    }
    if (magic_def != 0 && magic_def != (-1751672937)) {
        mc_free(magic_def, (int)D_00175040, 338);
        magic_def = -1751672937;
    }
    if (D_00199804 != 0 && D_00199804 != (-1751672937)) {
        mc_free(D_00199804, (int)D_00175040, 339);
        D_00199804 = -1751672937;
    }
    if (D_001997F0 != 0 && D_001997F0 != (-1751672937)) {
        mc_free(D_001997F0, (int)D_00175040, 340);
        D_001997F0 = -1751672937;
    }
    if (D_00195D24 != 0 && D_00195D24 != (-1751672937)) {
        mc_free(D_00195D24, (int)D_00175040, 341);
        D_00195D24 = -1751672937;
    }
    if (D_00195CF8 != 0 && D_00195CF8 != (-1751672937)) {
        mc_free(D_00195CF8, (int)D_00175040, 342);
        D_00195CF8 = -1751672937;
    }
    if (D_00195B64 != 0 && D_00195B64 != (-1751672937)) {
        mc_free(D_00195B64, (int)D_00175040, 343);
        D_00195B64 = -1751672937;
    }
    if (D_00195B74 != 0 && D_00195B74 != (-1751672937)) {
        mc_free(D_00195B74, (int)D_00175040, 344);
        D_00195B74 = -1751672937;
    }
    if (D_00195D74 != 0 && D_00195D74 != (-1751672937)) {
        mc_free(D_00195D74, (int)D_00175040, 345);
        D_00195D74 = -1751672937;
    }
    if (horse_overlay_image != 0 && horse_overlay_image != (-1751672937)) {
        mc_free(horse_overlay_image, (int)D_00175040, 346);
        horse_overlay_image = -1751672937;
    }
    if (cart_overlay_image != 0 && cart_overlay_image != (-1751672937)) {
        mc_free(cart_overlay_image, (int)D_00175040, 347);
        cart_overlay_image = -1751672937;
    }
    if (compass_box_image != 0 && compass_box_image != (-1751672937)) {
        mc_free(compass_box_image, (int)D_00175040, 348);
        compass_box_image = -1751672937;
    }
    if (compass_image != 0 && compass_image != (-1751672937)) {
        mc_free(compass_image, (int)D_00175040, 349);
        compass_image = -1751672937;
    }
    if (hud_mode_icons != 0 && hud_mode_icons != (-1751672937)) {
        mc_free(hud_mode_icons, (int)D_00175040, 350);
        hud_mode_icons = -1751672937;
    }
    if ((int)hud_bar_image != 0 && (int)hud_bar_image != (-1751672937)) {
        mc_free((int)hud_bar_image, (int)D_00175040, 351);
        *(int *)&hud_bar_image = -1751672937;
    }
    if (picklist_image != 0 && picklist_image != (-1751672937)) {
        mc_free(picklist_image, (int)D_00175040, 352);
        picklist_image = -1751672937;
    }
    if (D_00195C44 != 0 && D_00195C44 != (-1751672937)) {
        mc_free(D_00195C44, (int)D_00175040, 353);
        D_00195C44 = -1751672937;
    }
    if (D_001997F4 != 0 && D_001997F4 != (-1751672937)) {
        mc_free(D_001997F4, (int)D_00175040, 354);
        D_001997F4 = -1751672937;
    }
    weapon_free_sprites();
    spell_cast_anims_free();
    func_0003F699();
    for (l_1C = 0; ((int)(short)*(short *)&l_1C) < 5; l_1C++) {
        if (D_00195D04[((int)(short)*(short *)&l_1C)] != 0 && D_00195D04[((int)(short)*(short *)&l_1C)] != (-1751672937)) {
            mc_free(D_00195D04[((int)(short)*(short *)&l_1C)], (int)D_00175040, 360);
            D_00195D04[((int)(short)*(short *)&l_1C)] = -1751672937;
        }
    }
    sky_free();
    faction_free();
    func_00068B1B();
    sound_cache_free_all();
    file_index_free();
}

void init_video(void)
{
    int l_1C;
    int l_18;

    func_000CE957();
    func_000CE918(18);
    func_000C9F29();
    func_000CE8A0((int)&D_001997FC, (int)&D_00199800);
    func_00149E00(102400);
    D_00195C44 = mc_malloc(80780, (int)D_00175040, 382);
    file_index_build();
    func_00143600(19, 1);
    func_00142700();
    func_0012B100();
    func_000CE69A();
    func_0012DB00();
    func_0012A100();
    func_0012A2D0(160, 77, 160, 77);
    func_0012A274(200, 180);
    func_00136A00();
    func_000C0100();
    D_00136911 = 2048;
    D_000CEA20 = 2560;
    D_000CEA24 = 393216;
    func_0012A254(8);
    func_00136B88(0, 0);
    func_000CB300();
    player_underwater = 0;
    disk_read_file((int)D_00175214, (D_001997F4 = mc_malloc(768, (int)D_00175040, 410)));
    func_000C2D00();
    func_000C310F((int)D_0017521C);
}

void func_0004FEEA(void)
{
    func_00143700();
    func_00142764();
    func_000C0580();
}

void func_0004FF11(void)
{
    palette_restore();
}

void palette_restore(void)
{
    func_0012D851(D_001997F4);
    D_001940D7 |= 1;
}

void game_exit(int a1)
{
    func_000A0ED9(440, (int)D_00175040);
    func_000A148C((int)D_00175226);
    D_0018DC24 = 0;
    mem_check_level = 0;
    exiting = 1;
    game_shutdown();
    if (a1 == 0) {
        func_0009DA1C(449, (int)D_00175040);
        printf((int)D_0017523C);
    } else {
        func_0009DA1C(451, (int)D_00175040);
        printf(a1);
    }
    kludge_print_build(1);
    func_000A0ED9(455, (int)D_00175040);
    mc_sprintf((int)text_buffer, (int)D_00175260, (int)D_001917E4, (int)D_00175253);
    unlink((int)text_buffer);
    exit(0);
}

void game_shutdown(void)
{
    func_0003D3F0();
    shutdown_free_all();
    func_0004FEEA();
    func_00149EF8();
}

void fatal_error(int a1)
{
    {
        char l_418[1024];

        mem_check_level = 0;
        if (exiting != 0) return;
        if (D_000CDE2F != 0) {
            func_000A0ED9(480, (int)D_00175040);
            mc_sprintf((int)l_418, (int)D_00175266, D_000CDE2F);
        } else {
            mc_strncpy((int)l_418, a1, 1024, (int)D_00175040, 482);
        }
        func_000A0ED9(484, (int)D_00175040);
        func_000A148C((int)D_00175281, frame_checkpoint);
        func_000A0ED9(485, (int)D_00175040);
        func_000A148C((int)D_001752B6, (int)l_418);
        func_000A0ED9(487, (int)D_00175040);
        func_000A18C3((int)l_418);
        exiting = 1;
        D_0018DC24 = 0;
        mem_check_level = 0;
        game_shutdown();
        func_0009DA1C(495, (int)D_00175040);
        printf((int)D_001752D2, frame_checkpoint);
        func_0009DA1C(496, (int)D_00175040);
        printf((int)D_001752B6, (int)l_418);
        func_0009DA1C(497, (int)D_00175040);
        printf((int)D_00175307);
        func_000A0ED9(499, (int)D_00175040);
        mc_sprintf((int)l_418, (int)D_00175260, (int)D_001917E4, (int)D_00175253);
        unlink((int)l_418);
        exit(10);
    }
}

void newgame_init_player(void)
{
    int l_20;
    struct character *l_1C;
    struct career *l_18;

    player_refresh_paperdoll();
    player_compute_jump_velocity();
    func_000A0ED9(514, (int)D_00175040);
    mc_sprintf((int)text_buffer, (int)D_00175313, (int)D_001917E4);
    disk_copy_file((int)cfg_mapsave_file, (int)text_buffer, (int)D_001917E4);
    l_1C = player_character;
    l_18 = player_class;
    l_1C->fatigue = (l_1C->attributes[ATTR_STR] + l_1C->attributes[ATTR_END]) << 6;
    l_1C->health = l_18->hp_per_level + 25;
    for (l_20 = 1; l_1C->level > l_20; l_20++) {
        l_1C->health += rand_range(1, l_18->hp_per_level);
    }
    l_1C->max_health_base = (int)(short)(l_1C->max_health = l_1C->health);
    character_reset_magicka(l_1C, l_18);
    l_1C->attack_damage[0][0] = 1;
    l_1C->attack_damage[0][1] = 2;
    weapon_reload_hand_sprites();
}

void character_reset_magicka(struct character *a1, struct career *a2)
{
    a1->magicka = (a1->max_magicka = (a1->attributes[ATTR_INT] * ((int)(short)D_001788D3[((a2->flags >> 10) & 7)])) / 256);
}

int flats_cfg_find(int a1)
{
    int l_1C;

    for (l_1C = 0; l_1C < flats_cfg_count; l_1C++) {
        if (((int)(unsigned short)*(short *)(flats_cfg + (l_1C * 40))) == a1) {
            return ((int)flats_cfg) + (l_1C * 40);
        }
    }
    return 0;
}

void func_00050540(int a1, int a2)
{
    *(int *)((char *)a1) = func_00050607(*(int *)((char *)a1));
    while (((int)(unsigned char)*(signed char *)(*(char **)((char *)a1))) != 13) {
        *(signed char *)((char *)a2++) = *(signed char *)(*(char **)((char *)a1));
        (*(int *)((char *)a1))++;
        (*(int *)disk_last_file_size)--;
    }
    *(signed char *)((char *)a2++) = 0;
}

int func_000505A3(int a1)
{
    int l_1C;

    *(int *)((char *)a1) = func_00050607(*(int *)((char *)a1));
    if (*(int *)((char *)a1) == 0) return 100000;
    l_1C = atoi(*(int *)((char *)a1));
    *(int *)((char *)a1) = func_0005065A(*(int *)((char *)a1));
    return l_1C;
}

int func_00050607(int a1)
{
    while (((int)(unsigned char)*(signed char *)((char *)a1)) <= 32) {
        a1++;
        (*(int *)disk_last_file_size)--;
        if (*(int *)disk_last_file_size < 1) return 0;
    }
    return a1;
}

int func_0005065A(int a1)
{
    while (((int)(unsigned char)*(signed char *)((char *)a1)) > 32) {
        a1++;
        (*(int *)disk_last_file_size)--;
    }
    return a1;
}

void init_player_records(struct record *a1)
{
    int l_1C;
    int l_18;

    (player_object = object_create_child(a1, 0, 0))->type = 4;
    player_object->flags = 3;
    player_object->x = 0;
    player_object->y = 0;
    player_object->z = 0;
    (camera_object = object_create_child(player_object, 0, 0))->type = 5;
    camera_object->flags = 2;
    camera_object->x = player_object->x;
    camera_object->y = player_object->y - 75;
    camera_object->z = player_object->y;
    (player_entity = object_create_child(player_object, 0, 634))->type = 3;
    player_entity->flags = 3;
    player_class = &(player_character = &player_entity->data.character)->career;
    player_character->pad83 = 1;
    player_character->reflexes = 2;
    player_character->mobile_id = 200;
    (inventory_containers[0] = object_create_child(player_entity, 0, 0))->type = 52;
    inventory_containers[0]->flags = 3;
    inventory_containers[0]->image = 0;
    *(signed char *)((char *)(*(int *)&D_001959DC = (int)object_create_child(player_entity, 0, 0))) = 52;
    D_001959DC->flags = 3;
    D_001959DC->image = 1;
    *(signed char *)((char *)(*(int *)&D_001959E0 = (int)object_create_child(player_entity, 0, 0))) = 52;
    D_001959E0->flags = 3;
    D_001959E0->image = 2;
    *(signed char *)((char *)(*(int *)&D_001959E4 = (int)object_create_child(player_entity, 0, 0))) = 52;
    D_001959E4->flags = 3;
    D_001959E4->image = 3;
    *(signed char *)((char *)(*(int *)&D_001959EC = (int)object_create_child(player_entity, 0, 0))) = 52;
    D_001959EC->flags = 3;
    D_001959EC->image = 5;
    *(signed char *)((char *)(*(int *)&D_001959F0 = (int)object_create_child(player_entity, 0, 0))) = 52;
    D_001959F0->flags = 3;
    D_001959F0->image = 6;
    *(signed char *)((char *)(*(int *)&D_001959F4 = (int)object_create_child(player_entity, 0, 0))) = 52;
    D_001959F4->flags = 3;
    D_001959F4->image = 7;
    *(signed char *)((char *)(*(int *)&D_001959F8 = (int)object_create_child(player_entity, 0, 0))) = 52;
    D_001959F8->flags = 3;
    D_001959F8->image = 8;
    *(signed char *)((char *)(*(int *)&D_00195A00 = (int)object_create_child(player_entity, 0, 0))) = 16;
    D_00195A00->flags = 3;
    *(signed char *)((char *)(*(int *)&options_object = (int)object_create_child(player_entity, 0, 6))) = 23;
    options_object->flags = 3;
    *(short *)((char *)(*(int *)&game_settings = (int)options_object + 71)) = 32514;
    game_settings->sound_volume = 127;
    game_settings->music_volume = 128;
    *(signed char *)((char *)(*(int *)&logbook_object = (int)object_create_child(player_entity, 0, 3008))) = 24;
    logbook_object->flags = 3;
    l_18 = (int)logbook_object + 71;
    *(signed char *)((char *)(*(int *)&bank_accounts = (int)object_create_child(player_entity, 0, 806))) = 25;
    bank_accounts->flags = 3;
}

void update_underwater(void)
{
    int l_18;

    if (dungeon_water_level == 10000) return;
    if ((player_object->y - 76) > dungeon_water_level) {
        l_18 = 1;
    } else {
        l_18 = 0;
    }
    if (((int)(unsigned char)player_underwater) == l_18) return;
    player_underwater = *(signed char *)&l_18;
    func_00136B88(l_18, l_18);
    func_000CB300();
}

void spell_cast_anims_load(void)
{
    spell_cast_anims_free();
    spell_cast_anim_fire[0] = disk_read_file((int)D_00175324, 0);
    spell_cast_anim_frost = disk_read_file((int)D_00175331, 0);
    spell_cast_anim_magic = disk_read_file((int)D_0017533E, 0);
    spell_cast_anim_poison = disk_read_file((int)D_0017534B, 0);
    spell_cast_anim_shock = disk_read_file((int)D_00175358, 0);
}

void spell_cast_anims_free(void)
{
    if (spell_cast_anim_fire[0] != 0 && spell_cast_anim_fire[0] != (-1751672937)) {
        mc_free(spell_cast_anim_fire[0], (int)D_00175040, 754);
        spell_cast_anim_fire[0] = -1751672937;
    }
    if (spell_cast_anim_frost != 0 && spell_cast_anim_frost != (-1751672937)) {
        mc_free(spell_cast_anim_frost, (int)D_00175040, 755);
        spell_cast_anim_frost = -1751672937;
    }
    if (spell_cast_anim_magic != 0 && spell_cast_anim_magic != (-1751672937)) {
        mc_free(spell_cast_anim_magic, (int)D_00175040, 756);
        spell_cast_anim_magic = -1751672937;
    }
    if (spell_cast_anim_poison != 0 && spell_cast_anim_poison != (-1751672937)) {
        mc_free(spell_cast_anim_poison, (int)D_00175040, 757);
        spell_cast_anim_poison = -1751672937;
    }
    if (spell_cast_anim_shock == 0 || spell_cast_anim_shock == (-1751672937)) {
        return;
    }
    mc_free(spell_cast_anim_shock, (int)D_00175040, 758);
    spell_cast_anim_shock = -1751672937;
}

void update_fog(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_18 = ((unsigned)game_minutes) % 1440;
    if (l_18 > 360 && l_18 < 1080) {
        l_20 = 1;
    } else {
        l_20 = 0;
    }
    D_00199808 = l_20;
    if (((int)player_environment) == 1) {
        if (D_00199808 == 0) {
            D_000CEA24 = ((game_settings->view_flags >> 8) + 129) * 1536;
            D_0014CC0C = D_00195D20;
            func_0014D23C((D_000CEA24 >> 8) - 512);
            D_00196283 = 0;
            return;
        }
        l_1C = climate_category();
        if (((int)(unsigned char)(climate_weathers[l_1C] & 127)) == 3 || ((int)(unsigned char)(climate_weathers[l_1C] & 128)) != 0) {
            D_000CEA24 = ((game_settings->view_flags >> 8) + 129) * 768;
            D_0014CC0C = D_00195D18 + 16128;
            func_0014D23C(8);
            D_00196283 = 119;
        } else {
            D_000CEA24 = ((game_settings->view_flags >> 8) + 129) * 3072;
            D_0014CC0C = D_00195CF4 + 16128;
            func_0014D23C((D_000CEA24 >> 8) - 512);
            D_00196283 = 0;
        }
        return;
    }
    if (dungeon_water_level != 10000 && (player_object->y - 76) > dungeon_water_level) {
        D_000CEA24 = ((game_settings->view_flags >> 8) + 129) * 768;
        D_0014CC0C = D_00195CF4 + 16128;
        func_0014D23C(4);
        D_00196283 = 107;
        return;
    }
    D_000CEA24 = ((game_settings->view_flags >> 8) + 129) * 1536;
    D_0014CC0C = D_00195D20;
    func_0014D23C((D_000CEA24 >> 8) - 512);
    D_00196283 = 223;
}

void game_reset(void)
{
    int l_18;

    if (D_00195AC4->image != 65535) location_unload(D_00195AC4->image);
    object_free_children(nonworld_root);
    frame_checkpoint = 500;
    object_delete(player_object);
    init_player_records(D_00195AC4);
    frame_checkpoint = 501;
    newgame_init_player();
    frame_checkpoint = 502;
    inv_reset_left_list();
    frame_checkpoint = 503;
    D_001940D8 |= 8;
    D_00195998 = (game_minutes = 519210);
    frame_checkpoint = 504;
    faction_load_file();
    frame_checkpoint = 505;
    for (l_18 = 0; l_18 < 62; l_18++) {
        *(short *)(region_price_adjustment + (l_18 * 80)) = rand_range(0, 500) + 750;
    }
    mc_memset((int)saved_positions, 0, 48, (int)D_00175040, 851, 48);
    mc_memset((int)((char *)D_00190504), 0, 512, (int)D_00175040, 852, 512);
    creature_count = 0;
}

void books_find_path(void)
{
    int l_18;

    func_000A0ED9(860, (int)D_00175040);
    mc_sprintf((int)text_buffer, (int)D_00175365, (int)D_001917E4);
    l_18 = open((int)text_buffer, 512);
    if (l_18 < 0) {
        books_path = (int)D_00191834;
        return;
    }
    books_path = (int)D_001917E4;
    func_0009DEA7(l_18);
}

void func_00050FFD(void)
{
    D_0019621B = 18;
    D_0019625E = (int)D_00195AC4;
}
