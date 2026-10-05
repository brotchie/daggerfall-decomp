/* init.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_5 { unsigned char f:5; };
extern char disk_last_file_size[];
extern char D_000C23C4[];
extern char D_000C23C8[];
extern char D_000C23CC[];
extern char D_000CDE2F[];
extern char D_000CEA20[];
extern char D_000CEA24[];
extern char D_0012B508[];
extern char dungeon_water_level[];
extern char D_00136911[];
extern char D_0014CC0C[];
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
extern char D_001788D3[];
extern char player_environment[];
extern char mem_check_level[];
extern char frame_checkpoint[];
extern char D_0018DC24[];
extern char saved_positions[];
extern char region_event_values[];
extern char region_price_adjustment[];
extern char text_buffer[];
extern struct record *D_00190504[];
extern char D_00190704[];
extern char hud_compass_image[];
extern char D_00190908[];
extern char D_0019090C[];
extern char D_00190910[];
extern char D_001917E4[];
extern char D_00191834[];
extern char flats_cfg[];
extern char D_001940D7[];
extern char D_001940D8[];
extern char horse_overlay_image[];
extern char cart_overlay_image[];
extern char D_00195998[];
extern struct record *nonworld_root;
extern char logbook_object[];
extern char options_object[];
extern struct record *inventory_containers[];
extern char D_001959DC[];
extern char D_001959E0[];
extern char D_001959E4[];
extern char D_001959EC[];
extern char D_001959F0[];
extern char D_001959F4[];
extern char D_001959F8[];
extern char D_00195A00[];
extern char bank_accounts[];
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern char D_00195AC8[];
extern char D_00195AD0[];
extern char picklist_image[];
extern struct spell *spell_records;
extern char creature_count[];
extern char current_region_data[];
extern char D_00195B64[];
extern char hud_bar_image[];
extern char hud_mode_icons[];
extern char D_00195B74[];
extern char D_00195B78[];
extern char icon_image[];
extern char buttons_rci[];
extern struct character *player_character;
extern char window_image[];
extern struct career *player_class;
extern char game_minutes[];
extern struct settings *game_settings;
extern char D_00195C44[];
extern char cfg_map_file[];
extern char D_00195C7C[];
extern char D_00195C80[];
extern char D_00195C84[];
extern char flats_cfg_count[];
extern char spell_cast_anim_fire[];
extern char spell_cast_anim_frost[];
extern char spell_cast_anim_poison[];
extern char spell_cast_anim_shock[];
extern char spell_cast_anim_magic[];
extern char D_00195CF4[];
extern char D_00195CF8[];
extern char D_00195D04[];
extern char D_00195D18[];
extern char D_00195D20[];
extern char D_00195D24[];
extern char magic_def[];
extern char D_00195D74[];
extern char books_path[];
extern char compass_image[];
extern char compass_box_image[];
extern char qbn_opcode_arg_counts[];
extern char climate_weathers[];
extern char view_cursor_active[];
extern char D_0019621B[];
extern char D_0019625E[];
extern char current_region[];
extern char player_underwater[];
extern char D_00196283[];
extern char exiting[];
extern char maps_bsa[];
extern char D_001997F0[];
extern char D_001997F4[];
extern char D_001997FC[];
extern char D_00199800[];
extern char D_00199804[];
extern char D_00199808[];
extern char D_001A3F60[];
extern char D_001A3F7C[];
extern char D_001A3F90[];
extern char D_001A3F9C[];
extern char cfg_mapsave_file[];
extern char cfg_start_map[];
extern char cfg_region[];
extern char arch3d_bsa[];
extern char dagger_snd[];
extern char D_001AA5FC[];

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
    if (((struct bf8_0_5 *)((char *)l_18))->f != 0) goto L4ECEA;
    dpmi_get_free_memory((int)D_001A3F60);
    *(int *)D_001A3F9C = *(int *)D_001A3F90 - (*(int *)D_001A3F7C << 2);
L4ECEA:;
    func_000A0ED9(47, (int)D_00175040);
    mc_sprintf((int)text_buffer, (int)D_00175047, *(int *)D_001A3F9C);
    *(signed char *)D_0012B508 = 146;
    text_draw((int)text_buffer, 246, 23);
}

void player_refresh_paperdoll(void)
{
    *(signed char *)D_001940D8 |= 8;
    mem_check_crt_heap(0);
    func_000CB552((*(int *)window_image = disk_read_file((int)D_00175207, 0)));
    paperdoll_draw(0, 0);
    func_00143914(0);
    if (*(int *)window_image == 0) goto L4F452;
    if (*(int *)window_image != (-1751672937)) goto L4F454;
L4F452:;
    return;
L4F454:;
    mc_free(*(int *)window_image, (int)D_00175040, 246);
    *(int *)window_image = -1751672937;
}

void init_world_objects(void)
{
    object_heap_init();
    *(int *)D_00195AD0 = (int)D_00195AC4;
    init_player_records(D_00195AC4);
    inv_reset_left_list();
    calendar_update();
}

void newgame_place_player(void)
{
    int l_1C;
    int l_18;

    D_00195AC4->image = 65535;
    if (((int)(unsigned char)*(signed char *)cfg_map_file) != 100) goto L4F4E5;
    *(signed char *)player_environment = 3;
    goto L4F4EC;
L4F4E5:;
    *(signed char *)player_environment = 1;
L4F4EC:;
    *(signed char *)current_region = *(signed char *)cfg_region;
    *(int *)current_region_data = ((int)region_event_values) + (((int)(unsigned char)*(signed char *)current_region) * 80);
    region_enter(0, (int)(unsigned char)*(signed char *)current_region);
    *(int *)dungeon_water_level = 10000;
    if (((int)(unsigned char)*(signed char *)player_environment) != 1) goto L4F54F;
    map_goto_location((int)(unsigned char)*(signed char *)cfg_region, (int)(unsigned char)*(signed char *)player_environment, *(int *)cfg_start_map, 0);
    goto L4F5A8;
L4F54F:;
    map_goto_location((int)(unsigned char)*(signed char *)cfg_region, 1, *(int *)cfg_start_map, 0);
    player_to_nearest_marker(D_00195AC4, 8);
    dungeon_load(-1);
    automap_load();
    if (marker_count(D_00195AC4, 6) == 0) goto L4F5A8;
    player_to_nearest_marker(D_00195AC4, 6);
L4F5A8:;
    *(int *)D_000C23C4 = player_object->x;
    *(int *)D_000C23CC = player_object->z;
    *(int *)D_000C23C8 = player_object->y;
    *(signed char *)view_cursor_active = 0;
    player_character->health = player_character->max_health;
}

void shutdown_free_all(void)
{
    int l_1C;
    int l_18;

    func_00152BA8();
    func_0012A230();
    archive_close(*(int *)arch3d_bsa);
    archive_close(*(int *)maps_bsa);
    archive_close(*(int *)dagger_snd);
    archive_close(*(int *)D_00195AC8);
    l_1C = 0;
L4F63A:;
    if (((int)(short)*(short *)&l_1C) < 127) goto L4F650;
    goto L4F6B4;
L4F648:;
    l_1C++;
    goto L4F63A;
L4F650:;
    if (*(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)) == 0) goto L4F6B2;
    if (*(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)) == 0) goto L4F683;
    if (*(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)) != (-1751672937)) goto L4F685;
L4F683:;
    goto L4F6B2;
L4F685:;
    mc_free(*(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)), (int)D_00175040, 314);
    *(int *)(D_00190704 + (((int)(short)*(short *)&l_1C) << 2)) = -1751672937;
L4F6B2:;
    goto L4F648;
L4F6B4:;
    func_000CE88D(*(int *)D_001997FC, *(int *)D_00199800);
    if (*(int *)qbn_opcode_arg_counts == 0) goto L4F6D9;
    if (*(int *)qbn_opcode_arg_counts != (-1751672937)) goto L4F6DB;
L4F6D9:;
    goto L4F6F9;
L4F6DB:;
    mc_free(*(int *)qbn_opcode_arg_counts, (int)D_00175040, 319);
    *(int *)qbn_opcode_arg_counts = -1751672937;
L4F6F9:;
    region_free_tables();
    region_unload();
    model_heap_free_all();
    object_heap_shutdown();
    intrface_free();
    func_0007EF20();
    if (*(int *)D_00195B78 == 0) goto L4F72C;
    if (*(int *)D_00195B78 != (-1751672937)) goto L4F72E;
L4F72C:;
    goto L4F74C;
L4F72E:;
    mc_free(*(int *)D_00195B78, (int)D_00175040, 326);
    *(int *)D_00195B78 = -1751672937;
L4F74C:;
    if (*(int *)D_001AA5FC == 0) goto L4F761;
    if (*(int *)D_001AA5FC != (-1751672937)) goto L4F763;
L4F761:;
    goto L4F781;
L4F763:;
    mc_free(*(int *)D_001AA5FC, (int)D_00175040, 327);
    *(int *)D_001AA5FC = -1751672937;
L4F781:;
    if ((int)spell_records == 0) goto L4F796;
    if ((int)spell_records != (-1751672937)) goto L4F798;
L4F796:;
    goto L4F7B6;
L4F798:;
    mc_free((int)spell_records, (int)D_00175040, 328);
    spell_records = (struct spell *)-1751672937;
L4F7B6:;
    if (*(int *)buttons_rci == 0) goto L4F7CB;
    if (*(int *)buttons_rci != (-1751672937)) goto L4F7CD;
L4F7CB:;
    goto L4F7EB;
L4F7CD:;
    mc_free(*(int *)buttons_rci, (int)D_00175040, 329);
    *(int *)buttons_rci = -1751672937;
L4F7EB:;
    if (*(int *)icon_image == 0) goto L4F800;
    if (*(int *)icon_image != (-1751672937)) goto L4F802;
L4F800:;
    goto L4F820;
L4F802:;
    mc_free(*(int *)icon_image, (int)D_00175040, 330);
    *(int *)icon_image = -1751672937;
L4F820:;
    if (*(int *)D_00195C7C == 0) goto L4F835;
    if (*(int *)D_00195C7C != (-1751672937)) goto L4F837;
L4F835:;
    goto L4F855;
L4F837:;
    mc_free(*(int *)D_00195C7C, (int)D_00175040, 331);
    *(int *)D_00195C7C = -1751672937;
L4F855:;
    if (*(int *)D_00195C80 == 0) goto L4F86A;
    if (*(int *)D_00195C80 != (-1751672937)) goto L4F86C;
L4F86A:;
    goto L4F88A;
L4F86C:;
    mc_free(*(int *)D_00195C80, (int)D_00175040, 332);
    *(int *)D_00195C80 = -1751672937;
L4F88A:;
    if (*(int *)D_00195C84 == 0) goto L4F89F;
    if (*(int *)D_00195C84 != (-1751672937)) goto L4F8A1;
L4F89F:;
    goto L4F8BF;
L4F8A1:;
    mc_free(*(int *)D_00195C84, (int)D_00175040, 333);
    *(int *)D_00195C84 = -1751672937;
L4F8BF:;
    if (*(int *)hud_compass_image == 0) goto L4F8D4;
    if (*(int *)hud_compass_image != (-1751672937)) goto L4F8D6;
L4F8D4:;
    goto L4F8F4;
L4F8D6:;
    mc_free(*(int *)hud_compass_image, (int)D_00175040, 334);
    *(int *)hud_compass_image = -1751672937;
L4F8F4:;
    if (*(int *)D_00190908 == 0) goto L4F909;
    if (*(int *)D_00190908 != (-1751672937)) goto L4F90B;
L4F909:;
    goto L4F929;
L4F90B:;
    mc_free(*(int *)D_00190908, (int)D_00175040, 335);
    *(int *)D_00190908 = -1751672937;
L4F929:;
    if (*(int *)D_0019090C == 0) goto L4F93E;
    if (*(int *)D_0019090C != (-1751672937)) goto L4F940;
L4F93E:;
    goto L4F95E;
L4F940:;
    mc_free(*(int *)D_0019090C, (int)D_00175040, 336);
    *(int *)D_0019090C = -1751672937;
L4F95E:;
    if (*(int *)D_00190910 == 0) goto L4F973;
    if (*(int *)D_00190910 != (-1751672937)) goto L4F975;
L4F973:;
    goto L4F993;
L4F975:;
    mc_free(*(int *)D_00190910, (int)D_00175040, 337);
    *(int *)D_00190910 = -1751672937;
L4F993:;
    if (*(int *)magic_def == 0) goto L4F9A8;
    if (*(int *)magic_def != (-1751672937)) goto L4F9AA;
L4F9A8:;
    goto L4F9C8;
L4F9AA:;
    mc_free(*(int *)magic_def, (int)D_00175040, 338);
    *(int *)magic_def = -1751672937;
L4F9C8:;
    if (*(int *)D_00199804 == 0) goto L4F9DD;
    if (*(int *)D_00199804 != (-1751672937)) goto L4F9DF;
L4F9DD:;
    goto L4F9FD;
L4F9DF:;
    mc_free(*(int *)D_00199804, (int)D_00175040, 339);
    *(int *)D_00199804 = -1751672937;
L4F9FD:;
    if (*(int *)D_001997F0 == 0) goto L4FA12;
    if (*(int *)D_001997F0 != (-1751672937)) goto L4FA14;
L4FA12:;
    goto L4FA32;
L4FA14:;
    mc_free(*(int *)D_001997F0, (int)D_00175040, 340);
    *(int *)D_001997F0 = -1751672937;
L4FA32:;
    if (*(int *)D_00195D24 == 0) goto L4FA47;
    if (*(int *)D_00195D24 != (-1751672937)) goto L4FA49;
L4FA47:;
    goto L4FA67;
L4FA49:;
    mc_free(*(int *)D_00195D24, (int)D_00175040, 341);
    *(int *)D_00195D24 = -1751672937;
L4FA67:;
    if (*(int *)D_00195CF8 == 0) goto L4FA7C;
    if (*(int *)D_00195CF8 != (-1751672937)) goto L4FA7E;
L4FA7C:;
    goto L4FA9C;
L4FA7E:;
    mc_free(*(int *)D_00195CF8, (int)D_00175040, 342);
    *(int *)D_00195CF8 = -1751672937;
L4FA9C:;
    if (*(int *)D_00195B64 == 0) goto L4FAB1;
    if (*(int *)D_00195B64 != (-1751672937)) goto L4FAB3;
L4FAB1:;
    goto L4FAD1;
L4FAB3:;
    mc_free(*(int *)D_00195B64, (int)D_00175040, 343);
    *(int *)D_00195B64 = -1751672937;
L4FAD1:;
    if (*(int *)D_00195B74 == 0) goto L4FAE6;
    if (*(int *)D_00195B74 != (-1751672937)) goto L4FAE8;
L4FAE6:;
    goto L4FB06;
L4FAE8:;
    mc_free(*(int *)D_00195B74, (int)D_00175040, 344);
    *(int *)D_00195B74 = -1751672937;
L4FB06:;
    if (*(int *)D_00195D74 == 0) goto L4FB1B;
    if (*(int *)D_00195D74 != (-1751672937)) goto L4FB1D;
L4FB1B:;
    goto L4FB3B;
L4FB1D:;
    mc_free(*(int *)D_00195D74, (int)D_00175040, 345);
    *(int *)D_00195D74 = -1751672937;
L4FB3B:;
    if (*(int *)horse_overlay_image == 0) goto L4FB50;
    if (*(int *)horse_overlay_image != (-1751672937)) goto L4FB52;
L4FB50:;
    goto L4FB70;
L4FB52:;
    mc_free(*(int *)horse_overlay_image, (int)D_00175040, 346);
    *(int *)horse_overlay_image = -1751672937;
L4FB70:;
    if (*(int *)cart_overlay_image == 0) goto L4FB85;
    if (*(int *)cart_overlay_image != (-1751672937)) goto L4FB87;
L4FB85:;
    goto L4FBA5;
L4FB87:;
    mc_free(*(int *)cart_overlay_image, (int)D_00175040, 347);
    *(int *)cart_overlay_image = -1751672937;
L4FBA5:;
    if (*(int *)compass_box_image == 0) goto L4FBBA;
    if (*(int *)compass_box_image != (-1751672937)) goto L4FBBC;
L4FBBA:;
    goto L4FBDA;
L4FBBC:;
    mc_free(*(int *)compass_box_image, (int)D_00175040, 348);
    *(int *)compass_box_image = -1751672937;
L4FBDA:;
    if (*(int *)compass_image == 0) goto L4FBEF;
    if (*(int *)compass_image != (-1751672937)) goto L4FBF1;
L4FBEF:;
    goto L4FC0F;
L4FBF1:;
    mc_free(*(int *)compass_image, (int)D_00175040, 349);
    *(int *)compass_image = -1751672937;
L4FC0F:;
    if (*(int *)hud_mode_icons == 0) goto L4FC24;
    if (*(int *)hud_mode_icons != (-1751672937)) goto L4FC26;
L4FC24:;
    goto L4FC44;
L4FC26:;
    mc_free(*(int *)hud_mode_icons, (int)D_00175040, 350);
    *(int *)hud_mode_icons = -1751672937;
L4FC44:;
    if (*(int *)hud_bar_image == 0) goto L4FC59;
    if (*(int *)hud_bar_image != (-1751672937)) goto L4FC5B;
L4FC59:;
    goto L4FC79;
L4FC5B:;
    mc_free(*(int *)hud_bar_image, (int)D_00175040, 351);
    *(int *)hud_bar_image = -1751672937;
L4FC79:;
    if (*(int *)picklist_image == 0) goto L4FC8E;
    if (*(int *)picklist_image != (-1751672937)) goto L4FC90;
L4FC8E:;
    goto L4FCAE;
L4FC90:;
    mc_free(*(int *)picklist_image, (int)D_00175040, 352);
    *(int *)picklist_image = -1751672937;
L4FCAE:;
    if (*(int *)D_00195C44 == 0) goto L4FCC3;
    if (*(int *)D_00195C44 != (-1751672937)) goto L4FCC5;
L4FCC3:;
    goto L4FCE3;
L4FCC5:;
    mc_free(*(int *)D_00195C44, (int)D_00175040, 353);
    *(int *)D_00195C44 = -1751672937;
L4FCE3:;
    if (*(int *)D_001997F4 == 0) goto L4FCF8;
    if (*(int *)D_001997F4 != (-1751672937)) goto L4FCFA;
L4FCF8:;
    goto L4FD18;
L4FCFA:;
    mc_free(*(int *)D_001997F4, (int)D_00175040, 354);
    *(int *)D_001997F4 = -1751672937;
L4FD18:;
    weapon_free_sprites();
    spell_cast_anims_free();
    func_0003F699();
    l_1C = 0;
L4FD2E:;
    if (((int)(short)*(short *)&l_1C) < 5) goto L4FD41;
    goto L4FD95;
L4FD39:;
    l_1C++;
    goto L4FD2E;
L4FD41:;
    if (*(int *)(D_00195D04 + (((int)(short)*(short *)&l_1C) << 2)) == 0) goto L4FD64;
    if (*(int *)(D_00195D04 + (((int)(short)*(short *)&l_1C) << 2)) != (-1751672937)) goto L4FD66;
L4FD64:;
    goto L4FD93;
L4FD66:;
    mc_free(*(int *)(D_00195D04 + (((int)(short)*(short *)&l_1C) << 2)), (int)D_00175040, 360);
    *(int *)(D_00195D04 + (((int)(short)*(short *)&l_1C) << 2)) = -1751672937;
L4FD93:;
    goto L4FD39;
L4FD95:;
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
    func_000CE8A0((int)D_001997FC, (int)D_00199800);
    func_00149E00(102400);
    *(int *)D_00195C44 = mc_malloc(80780, (int)D_00175040, 382);
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
    *(int *)D_00136911 = 2048;
    *(int *)D_000CEA20 = 2560;
    *(int *)D_000CEA24 = 393216;
    func_0012A254(8);
    func_00136B88(0, 0);
    func_000CB300();
    *(signed char *)player_underwater = 0;
    disk_read_file((int)D_00175214, (*(int *)D_001997F4 = mc_malloc(768, (int)D_00175040, 410)));
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
    func_0012D851(*(int *)D_001997F4);
    *(signed char *)D_001940D7 |= 1;
}

void game_exit(int a1)
{
    func_000A0ED9(440, (int)D_00175040);
    func_000A148C((int)D_00175226);
    *(int *)D_0018DC24 = 0;
    *(int *)mem_check_level = 0;
    *(signed char *)exiting = 1;
    game_shutdown();
    if (a1 != 0) goto L4FFCC;
    func_0009DA1C(449, (int)D_00175040);
    printf((int)D_0017523C);
    goto L4FFE8;
L4FFCC:;
    func_0009DA1C(451, (int)D_00175040);
    printf(a1);
L4FFE8:;
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

    *(int *)mem_check_level = 0;
    if (*(signed char *)exiting != 0) return;
    if (*(int *)D_000CDE2F == 0) goto L500C7;
    func_000A0ED9(480, (int)D_00175040);
    mc_sprintf((int)l_418, (int)D_00175266, *(int *)D_000CDE2F);
    goto L500E4;
L500C7:;
    mc_strncpy((int)l_418, a1, 1024, (int)D_00175040, 482);
L500E4:;
    func_000A0ED9(484, (int)D_00175040);
    func_000A148C((int)D_00175281, *(int *)frame_checkpoint);
    func_000A0ED9(485, (int)D_00175040);
    func_000A148C((int)D_001752B6, (int)l_418);
    func_000A0ED9(487, (int)D_00175040);
    func_000A18C3((int)l_418);
    *(signed char *)exiting = 1;
    *(int *)D_0018DC24 = 0;
    *(int *)mem_check_level = 0;
    game_shutdown();
    func_0009DA1C(495, (int)D_00175040);
    printf((int)D_001752D2, *(int *)frame_checkpoint);
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
    l_20 = 1;
L502BC:;
    if (l_1C->level > l_20) goto L502D9;
    goto L502F6;
L502D1:;
    l_20++;
    goto L502BC;
L502D9:;
    l_1C->health += rand_range(1, l_18->hp_per_level);
    goto L502D1;
L502F6:;
    l_1C->max_health_base = (int)(short)(l_1C->max_health = l_1C->health);
    character_reset_magicka(l_1C, l_18);
    l_1C->attack_damage[0][0] = 1;
    l_1C->attack_damage[0][1] = 2;
    weapon_reload_hand_sprites();
}

void character_reset_magicka(struct character *a1, struct career *a2)
{
    a1->magicka = (a1->max_magicka = (a1->attributes[ATTR_INT] * ((int)(short)*(short *)(D_001788D3 + (((a2->flags >> 10) & 7) * 2)))) / 256);
}

int flats_cfg_find(int a1)
{
    int l_1C;

    l_1C = 0;
L504F0:;
    if (l_1C < *(int *)flats_cfg_count) goto L50505;
    goto L5052C;
L504FD:;
    l_1C++;
    goto L504F0;
L50505:;
    if (((int)(unsigned short)*(short *)(flats_cfg + (l_1C * 40))) != a1) goto L5052A;
    return ((int)flats_cfg) + (l_1C * 40);
L5052A:;
    goto L504FD;
L5052C:;
    return 0;
}

void func_00050540(int a1, int a2)
{
    *(int *)((char *)a1) = func_00050607(*(int *)((char *)a1));
L50564:;
    if (((int)(unsigned char)*(signed char *)(*(char **)((char *)a1))) == 13) goto L50591;
    *(signed char *)((char *)a2++) = *(signed char *)(*(char **)((char *)a1));
    (*(int *)((char *)a1))++;
    (*(int *)disk_last_file_size)--;
    goto L50564;
L50591:;
    *(signed char *)((char *)a2++) = 0;
}

int func_000505A3(int a1)
{
    int l_1C;

    *(int *)((char *)a1) = func_00050607(*(int *)((char *)a1));
    if (*(int *)((char *)a1) != 0) goto L505D6;
    return 100000;
L505D6:;
    l_1C = atoi(*(int *)((char *)a1));
    *(int *)((char *)a1) = func_0005065A(*(int *)((char *)a1));
    return l_1C;
}

int func_00050607(int a1)
{
L50618:;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) > 32) goto L50647;
    a1++;
    (*(int *)disk_last_file_size)--;
    if (*(int *)disk_last_file_size >= 1) goto L50645;
    return 0;
L50645:;
    goto L50618;
L50647:;
    return a1;
}

int func_0005065A(int a1)
{
L5066B:;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) <= 32) goto L50688;
    a1++;
    (*(int *)disk_last_file_size)--;
    goto L5066B;
L50688:;
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
    *(signed char *)((char *)(*(int *)D_001959DC = (int)object_create_child(player_entity, 0, 0))) = 52;
    *(short *)(*(char **)D_001959DC + 21) = 3;
    *(short *)(*(char **)D_001959DC + 27) = 1;
    *(signed char *)((char *)(*(int *)D_001959E0 = (int)object_create_child(player_entity, 0, 0))) = 52;
    *(short *)(*(char **)D_001959E0 + 21) = 3;
    *(short *)(*(char **)D_001959E0 + 27) = 2;
    *(signed char *)((char *)(*(int *)D_001959E4 = (int)object_create_child(player_entity, 0, 0))) = 52;
    *(short *)(*(char **)D_001959E4 + 21) = 3;
    *(short *)(*(char **)D_001959E4 + 27) = 3;
    *(signed char *)((char *)(*(int *)D_001959EC = (int)object_create_child(player_entity, 0, 0))) = 52;
    *(short *)(*(char **)D_001959EC + 21) = 3;
    *(short *)(*(char **)D_001959EC + 27) = 5;
    *(signed char *)((char *)(*(int *)D_001959F0 = (int)object_create_child(player_entity, 0, 0))) = 52;
    *(short *)(*(char **)D_001959F0 + 21) = 3;
    *(short *)(*(char **)D_001959F0 + 27) = 6;
    *(signed char *)((char *)(*(int *)D_001959F4 = (int)object_create_child(player_entity, 0, 0))) = 52;
    *(short *)(*(char **)D_001959F4 + 21) = 3;
    *(short *)(*(char **)D_001959F4 + 27) = 7;
    *(signed char *)((char *)(*(int *)D_001959F8 = (int)object_create_child(player_entity, 0, 0))) = 52;
    *(short *)(*(char **)D_001959F8 + 21) = 3;
    *(short *)(*(char **)D_001959F8 + 27) = 8;
    *(signed char *)((char *)(*(int *)D_00195A00 = (int)object_create_child(player_entity, 0, 0))) = 16;
    *(short *)(*(char **)D_00195A00 + 21) = 3;
    *(signed char *)((char *)(*(int *)options_object = (int)object_create_child(player_entity, 0, 6))) = 23;
    *(short *)(*(char **)options_object + 21) = 3;
    *(short *)((char *)(*(int *)&game_settings = *(int *)options_object + 71)) = 32514;
    game_settings->sound_volume = 127;
    game_settings->music_volume = 128;
    *(signed char *)((char *)(*(int *)logbook_object = (int)object_create_child(player_entity, 0, 3008))) = 24;
    *(short *)(*(char **)logbook_object + 21) = 3;
    l_18 = *(int *)logbook_object + 71;
    *(signed char *)((char *)(*(int *)bank_accounts = (int)object_create_child(player_entity, 0, 806))) = 25;
    *(short *)(*(char **)bank_accounts + 21) = 3;
}

void update_underwater(void)
{
    int l_18;

    if (*(int *)dungeon_water_level == 10000) return;
    if ((player_object->y - 76) <= *(int *)dungeon_water_level) goto L50A5B;
    l_18 = 1;
    goto L50A62;
L50A5B:;
    l_18 = 0;
L50A62:;
    if (((int)(unsigned char)*(signed char *)player_underwater) == l_18) return;
    *(signed char *)player_underwater = *(signed char *)&l_18;
    func_00136B88(l_18, l_18);
    func_000CB300();
}

void spell_cast_anims_load(void)
{
    spell_cast_anims_free();
    *(int *)spell_cast_anim_fire = disk_read_file((int)D_00175324, 0);
    *(int *)spell_cast_anim_frost = disk_read_file((int)D_00175331, 0);
    *(int *)spell_cast_anim_magic = disk_read_file((int)D_0017533E, 0);
    *(int *)spell_cast_anim_poison = disk_read_file((int)D_0017534B, 0);
    *(int *)spell_cast_anim_shock = disk_read_file((int)D_00175358, 0);
}

void spell_cast_anims_free(void)
{
    if (*(int *)spell_cast_anim_fire == 0) goto L50B25;
    if (*(int *)spell_cast_anim_fire != (-1751672937)) goto L50B27;
L50B25:;
    goto L50B45;
L50B27:;
    mc_free(*(int *)spell_cast_anim_fire, (int)D_00175040, 754);
    *(int *)spell_cast_anim_fire = -1751672937;
L50B45:;
    if (*(int *)spell_cast_anim_frost == 0) goto L50B5A;
    if (*(int *)spell_cast_anim_frost != (-1751672937)) goto L50B5C;
L50B5A:;
    goto L50B7A;
L50B5C:;
    mc_free(*(int *)spell_cast_anim_frost, (int)D_00175040, 755);
    *(int *)spell_cast_anim_frost = -1751672937;
L50B7A:;
    if (*(int *)spell_cast_anim_magic == 0) goto L50B8F;
    if (*(int *)spell_cast_anim_magic != (-1751672937)) goto L50B91;
L50B8F:;
    goto L50BAF;
L50B91:;
    mc_free(*(int *)spell_cast_anim_magic, (int)D_00175040, 756);
    *(int *)spell_cast_anim_magic = -1751672937;
L50BAF:;
    if (*(int *)spell_cast_anim_poison == 0) goto L50BC4;
    if (*(int *)spell_cast_anim_poison != (-1751672937)) goto L50BC6;
L50BC4:;
    goto L50BE4;
L50BC6:;
    mc_free(*(int *)spell_cast_anim_poison, (int)D_00175040, 757);
    *(int *)spell_cast_anim_poison = -1751672937;
L50BE4:;
    if (*(int *)spell_cast_anim_shock == 0) goto L50BF9;
    if (*(int *)spell_cast_anim_shock != (-1751672937)) goto L50BFB;
L50BF9:;
    return;
L50BFB:;
    mc_free(*(int *)spell_cast_anim_shock, (int)D_00175040, 758);
    *(int *)spell_cast_anim_shock = -1751672937;
}

void update_fog(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_18 = ((unsigned)*(int *)game_minutes) % 1440;
    if (l_18 <= 360) goto L50C54;
    if (l_18 < 1080) goto L50C56;
L50C54:;
    goto L50C5F;
L50C56:;
    l_20 = 1;
    goto L50C66;
L50C5F:;
    l_20 = 0;
L50C66:;
    *(int *)D_00199808 = l_20;
    if (((int)(unsigned char)*(signed char *)player_environment) != 1) goto L50D8F;
    if (*(int *)D_00199808 != 0) goto L50CCF;
    *(int *)D_000CEA24 = ((((int)(unsigned short)*(short *)(*(char **)&game_settings)) >> 8) + 129) * 1536;
    *(int *)D_0014CC0C = *(int *)D_00195D20;
    func_0014D23C((*(int *)D_000CEA24 >> 8) - 512);
    *(signed char *)D_00196283 = 0;
    return;
L50CCF:;
    l_1C = climate_category();
    if (((int)(unsigned char)(*(signed char *)(climate_weathers + l_1C) & 127)) == 3) goto L50D00;
    if (((int)(unsigned char)(*(signed char *)(climate_weathers + l_1C) & 128)) == 0) goto L50D42;
L50D00:;
    *(int *)D_000CEA24 = ((((int)(unsigned short)*(short *)(*(char **)&game_settings)) >> 8) + 129) * 768;
    *(int *)D_0014CC0C = *(int *)D_00195D18 + 16128;
    func_0014D23C(8);
    *(signed char *)D_00196283 = 119;
    goto L50D8A;
L50D42:;
    *(int *)D_000CEA24 = ((((int)(unsigned short)*(short *)(*(char **)&game_settings)) >> 8) + 129) * 3072;
    *(int *)D_0014CC0C = *(int *)D_00195CF4 + 16128;
    func_0014D23C((*(int *)D_000CEA24 >> 8) - 512);
    *(signed char *)D_00196283 = 0;
L50D8A:;
    return;
L50D8F:;
    if (*(int *)dungeon_water_level == 10000) goto L50DAE;
    if ((player_object->y - 76) > *(int *)dungeon_water_level) goto L50DB0;
L50DAE:;
    goto L50DF2;
L50DB0:;
    *(int *)D_000CEA24 = ((((int)(unsigned short)*(short *)(*(char **)&game_settings)) >> 8) + 129) * 768;
    *(int *)D_0014CC0C = *(int *)D_00195CF4 + 16128;
    func_0014D23C(4);
    *(signed char *)D_00196283 = 107;
    return;
L50DF2:;
    *(int *)D_000CEA24 = ((((int)(unsigned short)*(short *)(*(char **)&game_settings)) >> 8) + 129) * 1536;
    *(int *)D_0014CC0C = *(int *)D_00195D20;
    func_0014D23C((*(int *)D_000CEA24 >> 8) - 512);
    *(signed char *)D_00196283 = 223;
}

void game_reset(void)
{
    int l_18;

    if (D_00195AC4->image == 65535) goto L50E75;
    location_unload(D_00195AC4->image);
L50E75:;
    object_free_children(nonworld_root);
    *(int *)frame_checkpoint = 500;
    object_delete(player_object);
    init_player_records(D_00195AC4);
    *(int *)frame_checkpoint = 501;
    newgame_init_player();
    *(int *)frame_checkpoint = 502;
    inv_reset_left_list();
    *(int *)frame_checkpoint = 503;
    *(signed char *)D_001940D8 |= 8;
    *(int *)D_00195998 = (*(int *)game_minutes = 519210);
    *(int *)frame_checkpoint = 504;
    faction_load_file();
    *(int *)frame_checkpoint = 505;
    l_18 = 0;
L50F00:;
    if (l_18 < 62) goto L50F10;
    goto L50F30;
L50F08:;
    l_18++;
    goto L50F00;
L50F10:;
    *(short *)(region_price_adjustment + (l_18 * 80)) = rand_range(0, 500) + 750;
    goto L50F08;
L50F30:;
    mc_memset((int)saved_positions, 0, 48, (int)D_00175040, 851, 48);
    mc_memset((int)((char *)D_00190504), 0, 512, (int)D_00175040, 852, 512);
    *(int *)creature_count = 0;
}

void books_find_path(void)
{
    int l_18;

    func_000A0ED9(860, (int)D_00175040);
    mc_sprintf((int)text_buffer, (int)D_00175365, (int)D_001917E4);
    l_18 = open((int)text_buffer, 512);
    if (l_18 >= 0) goto L50FE1;
    *(int *)books_path = (int)D_00191834;
    return;
L50FE1:;
    *(int *)books_path = (int)D_001917E4;
    func_0009DEA7(l_18);
}

void func_00050FFD(void)
{
    *(signed char *)D_0019621B = 18;
    *(int *)D_0019625E = (int)D_00195AC4;
}
