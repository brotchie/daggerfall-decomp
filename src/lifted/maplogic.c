/* maplogic.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf32_30_1 { unsigned _:30; unsigned f:1; };
struct bf32_31_1 { unsigned _:31; unsigned f:1; };
struct bf8_0_1 { unsigned char f:1; };
struct bf8_1_5 { unsigned char _:1; unsigned char f:5; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char D_000C2893[];
extern char D_000C28C0[];
extern char D_000C28C4[];
extern char dungeon_water_level[];
extern char D_00136911[];
extern char D_00176C94[];
extern char D_00176C9F[];
extern char D_00176CAA[];
extern char D_00176CC0[];
extern char D_00176CDD[];
extern char D_00176CFE[];
extern char D_00176D20[];
extern char D_00176D3E[];
extern char player_environment[];
extern struct spell *selected_spell;
extern char D_0018461C[];
extern char D_00185097[];
extern char D_00186503[];
extern char D_00187F30[];
extern char D_00187F44[];
extern char D_00187F46[];
extern char text_buffer[];
extern char D_001903A5[];
extern char D_001903A6[];
extern char D_00190EE4[];
extern char D_001940D5[];
extern char D_001950E4[];
extern char D_001950E8[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern char cheat_flags[];
extern struct record *D_00195AF4;
extern char clothing_gender_group[];
extern struct location *current_location;
extern struct character *player_character;
extern struct career *player_class;
extern char game_minutes[];
extern char D_00195C44[];
extern char D_00195C88[];
extern struct record *D_00195CB8;
extern char D_00195CF4[];
extern char D_00195D84[];
extern char D_00195F28[];
extern char D_00195F5E[];
extern char D_00195FB1[];
extern char D_00196120[];
extern char current_region[];
extern char interaction_mode[];
extern char current_climate[];
extern char crime_current[];
extern char D_00196280[];
extern char D_00196289[];
extern char D_0019628C[];
extern char D_001962A3[];
extern char dungeon_blocks[];
extern char D_001967A1[];
extern char D_00196A28[];
extern char rmb_block[];
extern struct map_location *location_here;
extern struct loaded_location loaded_location;
extern struct record *loaded_location_object;
extern struct location *loaded_location_data;
extern struct map_location *D_00196A9C;
extern char blocks_bsa[];
extern char dungeon_block_count[];
extern char D_00196DA4[];
extern char D_001970C4[];
extern char D_001A41DC[];
extern char D_001A41E4[];
extern struct membership *guild_membership;
extern char D_001A4A1D[];
extern char stocked_shop_count[];
extern char D_001A4C94[];
extern char D_001A94C0[];
extern char D_001A94C4[];
extern char climate_index[];
extern char D_001A99F4[];

extern int archive_open(int, int, int);
extern int lockpick_action_door(struct building *, int, struct record *);
extern int region_update_from_player(void);
extern int climate_update_at_player(void);
extern int damage_apply(struct record *, int, int);
extern int quest_find_site_for_building(struct building *);
extern int func_00041347(void);
extern int building_is_open(struct building *);
extern int building_minutes_to_close(struct building *);
extern int disk_read_file(int, int);
extern int guild_find_membership_by_kind(unsigned char);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int location_here_contains(int, int);
extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern struct record *object_find_by_id(struct record *, int);
extern int marker_count(struct record *, int);
extern int player_to_nearest_marker(struct record *, int);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int mc_memcpy();
extern int func_000C2D81();
extern int func_000C810C();
extern int func_000CD33A();
extern int func_00135E39();
extern void archive_close(int);
extern void lock_show_difficulty(int);
extern void dungeon_choose_textures(void);
extern void interior_stock_shelves(struct record *, struct building *);
extern void maploads_load_region(int);
extern void location_load_dungeon(struct loaded_location *, int);
extern void location_load_dungeon_by_id(struct loaded_location *, int);
extern void location_load_exterior(struct loaded_location *, int);
extern void dungeon_roll_monster_tables(void);
extern void town_map_note_building(int, int);
extern void automap_save(void);
extern void automap_mark_seen(struct record *);
extern void func_00028EAA(void);
extern void automap_restore_seen(void);
extern void damage_creature_death(struct record *);
extern void dungeon_load_rdb_block(int);
extern void msgbox_show_rsc(int, int);
extern void guards_summon(int);
extern void people_clear(void);
extern void func_000461E9(void);
extern void func_0004B5CF(void);
extern void func_0004C759(void);
extern void func_0004CA9D(void);
extern void fatal_error(int);
extern void item_make(int, int, struct item *);
extern void links_resolve(void);
extern void sound_stop_ambient(void);
extern void position_history_reset(void);
extern void building_grant_access(struct building *, unsigned char, int);
extern void func_0007E5C2(void);
extern void town_load(int);
extern void location_unload(unsigned short);
extern void terrain_update_cells(void);
extern void climate_set_textures(void);
extern void spell_end(struct record *);
extern void spfx_effect_tick(int, int, int);
extern void object_free_children(struct record *);
extern void func_0008EAF1(int, int);
extern void inv_store_item(struct record *);
extern void inv_merge_arrows(struct record *, struct record *, int);
extern void shop_quality_message(struct building *);
extern void func_00099D0D(struct record *);
struct map_location *region_find_location(int);
int building_try_enter(struct building *);
void location_free(struct loaded_location *);
void func_000871FA(struct building *);
void location_set_discovered(int, int);
void func_0008824A(int, int);

void region_unload(void)
{
    automap_save();
    location_unload(D_00195AC4->image);
    if ((int)D_00196A9C == 0) goto L8634F;
    if ((int)D_00196A9C != (-1751672937)) goto L86351;
L8634F:;
    goto L8636F;
L86351:;
    mc_free((int)D_00196A9C, (int)D_00176C94, 64);
    D_00196A9C = (struct map_location *)-1751672937;
L8636F:;
    D_00196A9C = 0;
    *(int *)D_00196A28 = 0;
    location_here = 0;
}

void location_free(struct loaded_location *a1)
{
    if (a1->data == 0) goto L863FF;
    if (a1->data->buildings == 0) goto L863FF;
    if (a1->data->buildings == 0) goto L863D8;
    if ((int)a1->data->buildings != (-1751672937)) goto L863DA;
L863D8:;
    goto L863FF;
L863DA:;
    mc_free((int)a1->data->buildings, (int)D_00176C94, 83);
    a1->data->buildings = (struct building *)-1751672937;
L863FF:;
    if (a1->object == 0) goto L8643E;
    if (a1->object == 0) goto L8641D;
    if ((int)a1->object != (-1751672937)) goto L8641F;
L8641D:;
    goto L8643E;
L8641F:;
    mc_free((int)a1->object, (int)D_00176C94, 86);
    a1->object = (struct record *)-1751672937;
L8643E:;
    if (a1->doors == 0) goto L8647D;
    if (a1->doors == 0) goto L8645C;
    if ((int)a1->doors != (-1751672937)) goto L8645E;
L8645C:;
    goto L8647D;
L8645E:;
    mc_free((int)a1->doors, (int)D_00176C94, 89);
    a1->doors = (char *)-1751672937;
L8647D:;
    mc_memset(a1, 0, 20, (int)D_00176C94, 91, 4);
}

struct map_location *region_find_location(int a1)
{
    struct map_location *l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_20 = *(int *)D_00196A28;
    l_1C = (l_20 + l_24) >> 1;
L864CA:;
    if (l_20 <= l_24) goto L86524;
    l_1C = (l_20 + l_24) >> 1;
    l_28 = (struct map_location *)((char *)D_00196A9C + (l_1C * 17));
    if ((l_28->map_id & 1048575) != a1) goto L86503;
    return l_28;
L86503:;
    if ((l_28->map_id & 1048575) <= a1) goto L8651B;
    l_20 = l_1C - 1;
    goto L86522;
L8651B:;
    l_24 = l_1C + 1;
L86522:;
    goto L864CA;
L86524:;
    if ((D_00196A9C[l_24].map_id & 1048575) != a1) goto L8654C;
    return (struct map_location *)((char *)D_00196A9C + (l_24 * 17));
L8654C:;
    return 0;
}

void world_update_location(void)
{
    int l_1C;
    int l_18;

    if (((int)(unsigned char)*(signed char *)player_environment) > 2) return;
    terrain_update_cells();
    l_18 = func_000C2D81(player_object->x, player_object->z);
    if (l_18 == *(int *)D_001A94C4) goto L86744;
    *(int *)D_001A94C4 = l_18;
    location_unload(D_00195AC4->image);
    region_update_from_player();
    climate_update_at_player();
    *(int *)climate_index = ((int)(unsigned char)*(signed char *)current_climate) - 224;
    location_here = (struct map_location *)((int)region_find_location(*(int *)D_001A94C4));
    climate_set_textures();
L86744:;
    if ((int)location_here == 0) return;
    if (location_here_contains(player_object->x, player_object->z) == 0) goto L86777;
    town_load(((unsigned)location_here->map_id) >> 20);
    return;
L86777:;
    location_unload(D_00195AC4->image);
}

void dungeon_load(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    sound_stop_ambient();
    if (D_00195AC4->image != 65535) goto L867D4;
    func_0008EAF1((int)D_00195AC4->children, D_00195AC4->id);
L867D4:;
    if (*(signed char *)D_00196289 != 0) goto L86811;
    player_object->parent_id = player_object->parent->id;
    mc_memcpy((int)D_00195FB1, (int)player_object, 55, (int)D_00176C94, 235, 4);
L86811:;
    l_20 = D_00195AC4->id;
    location_unload(D_00195AC4->image);
    if (a1 != (-1)) goto L86844;
    location_load_dungeon_by_id(&loaded_location, l_20);
    goto L86851;
L86844:;
    location_load_dungeon(&loaded_location, a1);
L86851:;
    mc_memcpy((int)D_00195AC4, (int)loaded_location_object, 55, (int)D_00176C94, 247, 4);
    mc_memcpy((int)current_location, (int)loaded_location_data, 48, (int)D_00176C94, 248, 4);
    *(int *)blocks_bsa = archive_open((int)D_00176C9F, 0, 0);
    if ((((unsigned)D_00195AC4->id) >> 16) != 50015) goto L868BF;
    *(signed char *)D_001967A1 = 254;
L868BF:;
    l_1C = 0;
L868C6:;
    if (((int)(unsigned char)*(signed char *)dungeon_block_count) > l_1C) goto L868DC;
    goto L868F0;
L868D4:;
    l_1C++;
    goto L868C6;
L868DC:;
    dungeon_load_rdb_block(((int)dungeon_blocks) + (l_1C << 2));
    goto L868D4;
L868F0:;
    func_000461E9();
    archive_close(*(int *)blocks_bsa);
    links_resolve();
    func_0007E5C2();
    dungeon_choose_textures();
    *(signed char *)D_0019628C = 0;
    D_00195CB8 = 0;
    *(signed char *)player_environment = 3;
    *(int *)dungeon_water_level = 10000;
    func_0004CA9D();
    player_object->x = D_00195AC4->x;
    player_object->y = D_00195AC4->y;
    player_object->z = D_00195AC4->z;
    l_1C = player_to_nearest_marker(D_00195AC4->children, 8);
    if (l_1C != 0) goto L8698D;
    fatal_error((int)D_00176CAA);
L8698D:;
    func_00028EAA();
    automap_mark_seen(D_00195AF4);
    position_history_reset();
    dungeon_roll_monster_tables();
    *(signed char *)D_001970C4 = 0;
    disk_read_file((int)D_00176CC0, *(int *)D_00195CF4);
    mc_memset(*(int *)D_00195C44, 0, 93, (int)D_00176C94, 290, 4);
    func_000CD33A(*(int *)D_00195C44, 1, 31);
    if (*(signed char *)D_00196289 != 0) goto L869FC;
    func_0004C759();
L869FC:;
    automap_restore_seen();
    *(int *)D_001950E4 = marker_count(D_00195AC4, 9);
    *(int *)D_001950E8 = marker_count(D_00195AC4, 16);
    *(int *)D_00136911 = 0;
    func_00135E39();
    *(signed char *)text_buffer = (*(signed char *)D_001903A5 = (*(signed char *)D_001903A6 = 0));
    func_000CD33A((int)text_buffer, 255, 1);
}

void func_00086A71(struct record *a1)
{
    struct building *l_18;

    if (a1->type != 32) return;
    l_18 = object_building(a1);
    switch (l_18->type) {
case 11:
case 14:
case 15:
    a1->lock_level = 0;
    a1->flags |= 64;
    return;
default:
    if ((rand() % 100) >= 90) return;
    a1->lock_level = 0;
    a1->flags |= 64;
}
}

int building_try_enter(struct building *a1)
{
    struct record *l_24;
    int l_20;
    int l_1C;

    if (*(signed char *)D_00196289 == 0) goto L86F76;
    return 1;
L86F76:;
    if (a1->id != player_character->house) goto L86F93;
    return 1;
L86F93:;
    if (a1->type != 24) goto L86FAF;
    return 1;
L86FAF:;
    if (a1->type >= 17) goto L86FCB;
    if (building_is_open(a1) != 0) goto L86FCD;
L86FCB:;
    goto L86FD9;
L86FCD:;
    return 1;
L86FD9:;
    if (a1->type >= 17) goto L86FFA;
    if ((a1->flags & 16) != 0) goto L86FFC;
L86FFA:;
    goto L87008;
L86FFC:;
    return 0;
L87008:;
    if ((a1->flags & 16) == 0) goto L87025;
    return 1;
L87025:;
    if (quest_find_site_for_building(a1) == 0) goto L8703A;
    if (*(signed char *)D_001962A3 != 0) goto L8703C;
L8703A:;
    goto L87048;
L8703C:;
    return 1;
L87048:;
    if ((player_character->conditions & 0x40) == 0) goto L8706E;
    player_character->conditions &= ~0x40;
    return 1;
L8706E:;
    if (a1->faction_id != 108) goto L8708A;
    if (guild_find_membership_by_kind(0) != 0) goto L8708C;
L8708A:;
    goto L87098;
L8708C:;
    return 1;
L87098:;
    if (a1->faction_id != 42) goto L870B7;
    if (guild_find_membership_by_kind(3) != 0) goto L870B9;
L870B7:;
    goto L870C5;
L870B9:;
    return 1;
L870C5:;
    if (a1->type < 17) goto L870E5;
    if (a1->type <= 20) goto L870E7;
L870E5:;
    goto L870F0;
L870E7:;
    if (*(signed char *)D_00196280 != 0) goto L870F2;
L870F0:;
    goto L8710B;
L870F2:;
    if ((a1->id & 65535) % 100 < 50) goto L8710D;
L8710B:;
    goto L87128;
L8710D:;
    msgbox_show_rsc(256, 1);
    return 1;
L87128:;
    if (((int)(unsigned char)*(signed char *)interaction_mode) != 2) goto L871C6;
    l_20 = (a1->name_seed % 10) + 3;
    l_24 = object_find_by_id(D_00195AC4, a1->id);
    if (lockpick_action_door(a1, l_20, l_24) == 0) goto L871C6;
    if (func_00041347() != 0) goto L871AC;
    if (rand_range(1, 300) >= (100 - player_character->skills[16].value)) goto L871BD;
L871AC:;
    *(signed char *)crime_current = 1;
    guards_summon(1);
L871BD:;
    return 1;
L871C6:;
    lock_show_difficulty((a1->name_seed % 10) + 3);
    return 0;
}

void func_000871FA(struct building *a1)
{
    struct record *l_18;

    l_18 = object_find_by_id(D_00195AC4, a1->id);
    object_free_children((struct record *)D_00196120);
    interior_stock_shelves(l_18->children, a1);
    if (l_18 == 0) return;
    if (player_to_nearest_marker(l_18->children, 6) != 0) goto L8725C;
    hud_message_add((int)D_00176CDD);
    return;
L8725C:;
    object_reparent(l_18, player_object);
    func_00099D0D(l_18);
    people_clear();
}

void building_enter(struct building *a1)
{
    if (a1 != 0) goto L872A7;
    hud_message_add((int)D_00176CFE);
    return;
L872A7:;
    *(short *)D_00195F5E = rand();
    if (building_try_enter(a1) == 0) return;
    current_building = a1;
    *(int *)D_00195D84 = *(int *)(D_00186503 + (a1->type << 2));
    object_free_children((struct record *)D_00196120);
    if (a1->id == player_character->house) goto L8731D;
    building_grant_access(a1, 5, (int)(*(char **)game_minutes + building_minutes_to_close(a1)));
    goto L87336;
L8731D:;
    building_grant_access(a1, 255, *(int *)game_minutes + 10000000);
L87336:;
    if (a1->type >= 14) goto L87356;
    if (a1->type != 1) goto L87358;
L87356:;
    goto L87368;
L87358:;
    if (a1->type != 3) goto L8736A;
L87368:;
    goto L87372;
L8736A:;
    shop_quality_message(a1);
L87372:;
    func_000871FA(a1);
    sound_stop_ambient();
    position_history_reset();
    *(signed char *)player_environment = 2;
    *(signed char *)D_001940D5 |= 2;
    D_00195CB8 = 0;
    *(signed char *)D_001A4A1D = 0;
    if (*(signed char *)D_00196289 != 0) goto L873B1;
    func_0004B5CF();
L873B1:;
    if (a1->id == *(int *)D_001A4C94) return;
    *(int *)stocked_shop_count = 0;
    *(int *)D_001A4C94 = a1->id;
}

void building_exit(void)
{
    struct building *l_1C;
    int l_18;

    l_1C = object_building(player_object->parent);
    object_free_children((struct record *)D_00196120);
    building_grant_access(l_1C, 0, 0);
    player_to_nearest_marker(player_object->parent->children, 6);
    func_000C810C(*(int *)D_00195C88);
    object_reparent(D_00195AC4, player_object);
    if (l_1C->type != 24) goto L87460;
    player_to_nearest_marker(D_00195AC4, 6);
L87460:;
    position_history_reset();
    *(signed char *)player_environment = 1;
    *(signed char *)D_001940D5 |= 2;
    *(signed char *)D_001A4A1D = 0;
    current_building = 0;
    guild_membership = 0;
    sound_stop_ambient();
    if (*(int *)D_001A41E4 == 0) return;
    town_map_note_building(*(int *)D_001A41E4, *(int *)D_001A41DC);
    *(int *)D_001A41E4 = 0;
}

void location_pick_random_town(struct loaded_location *a1)
{
    struct map_location *l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = D_00196A9C;
    l_18 = 0;
    mc_memset(a1, 0, 20, (int)D_00176C94, 991, 4);
    l_20 = 0;
L87EAA:;
    if (l_20 < *(int *)D_00196A28) goto L87EC6;
    goto L87EE7;
L87EB7:;
    l_20++;
    l_24++;
    goto L87EAA;
L87EC6:;
    switch ((l_24->x_type_flags << 2) >> 27) {
    goto L87EE5;
case 0:
case 1:
case 2:
    l_18++;
default:
L87EE5:;
    goto L87EB7;
L87EE7:;
    if (l_18 != 0) goto L87EFA;
    location_free(a1);
    return;
L87EFA:;
    l_1C = (rand() % l_18) + 1;
    l_24 = D_00196A9C;
    l_20 = 0;
L87F1C:;
    if (l_20 < *(int *)D_00196A28) goto L87F38;
    return;
L87F29:;
    l_20++;
    l_24++;
    goto L87F1C;
L87F38:;
}
    switch ((l_24->x_type_flags << 2) >> 27) {
    goto L87F57;
case 0:
case 1:
case 2:
    l_1C--;
default:
L87F57:;
    if (l_1C != 0) goto L87F6A;
    location_load_exterior(a1, l_20);
    return;
L87F6A:;
    goto L87F29;
}
}

void location_pick_random_undiscovered(struct loaded_location *a1)
{
    struct map_location *l_20;
    int l_1C;
    int l_18;

    l_20 = D_00196A9C;
    l_18 = 0;
    mc_memset(a1, 0, 20, (int)D_00176C94, 1108, 4);
    l_1C = 0;
L8813A:;
    if (l_1C < *(int *)D_00196A28) goto L88156;
    goto L88172;
L88147:;
    l_1C++;
    l_20++;
    goto L8813A;
L88156:;
    if ((l_20->x_type_flags & 0x40000000) != 0) goto L88168;
    if ((l_20->x_type_flags & 0x80000000) == 0) goto L8816A;
L88168:;
    goto L88170;
L8816A:;
    l_18++;
L88170:;
    goto L88147;
L88172:;
    if (l_18 != 0) goto L8819D;
    l_18 = rand() % *(int *)D_00196A28;
    location_load_exterior(a1, l_1C);
    return;
L8819D:;
    l_18 = rand() % l_18;
    l_20 = D_00196A9C;
    l_1C = 0;
L881BE:;
    if (l_1C < *(int *)D_00196A28) goto L881DA;
    return;
L881CB:;
    l_1C++;
    l_20++;
    goto L881BE;
L881DA:;
    if ((l_20->x_type_flags & 0x40000000) != 0) goto L881EC;
    if ((l_20->x_type_flags & 0x80000000) == 0) goto L881EE;
L881EC:;
    goto L881F4;
L881EE:;
    l_18--;
L881F4:;
    if (l_18 != 0) goto L88207;
    location_load_exterior(a1, l_1C);
    return;
L88207:;
    goto L881CB;
}

void location_set_discovered(int a1, int a2)
{
    ((struct bf32_30_1 *)((char *)(int)((a1 * 17) + (char *)D_00196A9C) + 4))->f = a2;
}

void func_0008824A(int a1, int a2)
{
    ((struct bf32_31_1 *)((char *)(int)((a1 * 17) + (char *)D_00196A9C) + 4))->f = a2;
}

void func_00088281(int a1, int a2)
{
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    if (((struct bf8_1_5 *)((char *)location_here + 7))->f != 0) goto L88307;
    l_20 = 1;
    l_28 = (location_here->x_type_flags & 33554431) + 1536;
    l_28 = (l_28 & 32767) >> 8;
    l_24 = ((location_here->y_size & 16777215) + ((((unsigned)location_here->y_size) >> 28) << 12)) - 1537;
    l_24 = 128 - ((l_24 & 32767) >> 8);
    goto L8836E;
L88307:;
    l_20 = 2;
    l_28 = (location_here->x_type_flags & 33554431) + 3584;
    l_28 = (l_28 & 32767) >> 8;
    l_24 = ((location_here->y_size & 16777215) + (((((unsigned)location_here->y_size) >> 28) - 1) << 12)) + 511;
    l_24 = 128 - ((l_24 & 32767) >> 8);
L8836E:;
    l_34 = a1;
    l_34 += l_24 << 8;
    l_34 += l_28;
    a1 = l_34;
    a2 += l_24 << 8;
    a2 += l_28;
    l_1C = (((((unsigned)(location_here->y_size << 4)) >> 28) - l_20) << 4) + 8;
    l_18 = (((((unsigned)location_here->y_size) >> 28) - l_20) << 4) + 8;
    if (((128 - l_28) - 4) >= l_1C) goto L883E7;
    l_1C = (128 - l_28) - 4;
L883E7:;
    if (((128 - l_24) - 4) >= l_18) goto L88405;
    l_18 = (128 - l_24) - 4;
L88405:;
    l_30 = l_34 + (l_18 << 8);
    l_14 = 0;
    l_2C = 0;
L88421:;
    if (l_2C < l_1C) goto L88439;
    goto L8844A;
L8842B:;
    l_2C++;
    l_34++;
    goto L88421;
L88439:;
    l_14 += (int)(unsigned char)(*(signed char *)((char *)l_34) & 127);
    goto L8842B;
L8844A:;
    l_34 = a1 + 256;
    l_2C = 0;
L8845C:;
    if ((l_18 - 2) > l_2C) goto L88471;
    goto L8849C;
L88469:;
    l_2C++;
    goto L8845C;
L88471:;
    l_14 += (int)(unsigned char)(*(signed char *)((char *)l_34) & 127);
    l_14 += (int)(unsigned char)(*(signed char *)((char *)(l_34 + l_1C) - 1) & 127);
    l_34 += 256;
    goto L88469;
L8849C:;
    l_2C = 0;
L884A3:;
    if (l_2C < l_1C) goto L884BB;
    goto L884CC;
L884AD:;
    l_2C++;
    l_34++;
    goto L884A3;
L884BB:;
    l_14 += (int)(unsigned char)(*(signed char *)((char *)l_34) & 127);
    goto L884AD;
L884CC:;
    l_14 = ((unsigned)l_14) / ((int)&*(signed char *)((char *)((l_1C * 2) + (l_18 * 2)) - 4));
    ++l_14;
    if (((unsigned)l_14) <= 127) goto L884F5;
    l_14 = 127;
L884F5:;
    l_34 = a1;
L884FB:;
    if (((unsigned)l_34) >= l_30) return;
    mc_memset(l_34, (int)(unsigned char)*(signed char *)&l_14, l_1C, (int)D_00176C94, 1250, 4);
    mc_memset(a2, 0, l_1C, (int)D_00176C94, 1251, 4);
    l_34 += 256;
    a2 += 256;
    goto L884FB;
}

void town_block_apply_ground(int a1, int a2)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    *(int *)D_001A94C0 = 4;
    l_18 = 0;
L88575:;
    if (l_18 < 4) goto L88585;
    goto L885A3;
L8857D:;
    l_18++;
    goto L88575;
L88585:;
    if (*(int *)D_001A94C4 != *(int *)(D_000C2893 + (l_18 << 2))) goto L885A1;
    *(int *)D_001A94C0 = l_18;
L885A1:;
    goto L8857D;
L885A3:;
    if (*(int *)D_001A94C0 == 4) return;
    l_28 = (int)(*(char **)D_000C28C0 + *(int *)(D_00187F30 + (*(int *)D_001A94C0 << 2)));
    l_2C = (int)(*(char **)D_000C28C4 + *(int *)(D_00187F30 + (*(int *)D_001A94C0 << 2)));
    if ((a2 & 32767) != 1) goto L885FC;
    a2 += -2;
    l_14 = 240;
    goto L88603;
L885FC:;
    l_14 = 256;
L88603:;
    l_28 += (128 - ((a2 & 32767) >> 8)) << 8;
    l_28 += (a1 & 32767) >> 8;
    l_2C += (128 - ((a2 & 32767) >> 8)) << 8;
    l_2C += (a1 & 32767) >> 8;
    l_24 = *(int *)rmb_block + 1739;
    l_20 = *(int *)rmb_block + 1995;
    l_18 = 0;
L88674:;
    if (l_18 < l_14) goto L886A1;
    return;
L88681:;
    l_18++;
    l_2C++;
    l_28++;
    l_24++;
    l_20++;
    goto L88674;
L886A1:;
    if (((int)(unsigned char)*(signed char *)((char *)l_24)) == 255) goto L886CD;
    if (((int)(unsigned char)(*(signed char *)((char *)l_24) & 63)) >= 56) goto L886CD;
    *(signed char *)((char *)l_2C) = *(signed char *)((char *)l_24);
L886CD:;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) == 255) goto L886FC;
    if ((((int)(unsigned char)*(signed char *)((char *)l_20)) >> 2) >= 33) goto L886FA;
    *(signed char *)((char *)l_28) = *(signed char *)((char *)l_20);
L886FA:;
    goto L88702;
L886FC:;
    *(signed char *)((char *)l_28) &= 3;
L88702:;
    if (((l_18 + 1) % 16) != 0) goto L88724;
    l_2C += 240;
    l_28 += 240;
L88724:;
    goto L88681;
}

void town_map_add_block(int a1, int a2)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_14 = current_location->width << 6;
    a1 <<= 6;
    a2 <<= 6;
    l_20 = a1 + 64;
    l_1C = a2 + 64;
    l_18 = 0;
    l_24 = a2;
L8890F:;
    if (l_24 < l_1C) goto L88921;
    return;
L88919:;
    l_24++;
    goto L8890F;
L88921:;
    l_28 = a1;
L88927:;
    if (l_28 < l_20) goto L88939;
    goto L88962;
L88931:;
    l_28++;
    goto L88927;
L88939:;
    *(signed char *)((char *)(int)(*(char **)D_00196DA4 + ((l_24 * l_14) + l_28))) = *(signed char *)(*(char **)rmb_block + 2251 + l_18++);
    goto L88931;
L88962:;
    goto L88919;
}

struct building *location_find_building(int a1)
{
    struct building *l_20;
    int l_1C;

    l_20 = current_location->buildings;
    l_1C = 0;
L88990:;
    if (current_location->building_count > l_1C) goto L889B4;
    goto L889C9;
L889A5:;
    l_1C++;
    l_20++;
    goto L88990;
L889B4:;
    if (l_20->id != a1) goto L889C7;
    return l_20;
L889C7:;
    goto L889A5;
L889C9:;
    return 0;
}

void location_reveal(int a1, int a2)
{
    struct map_location *l_1C;
    int l_18;
    int l_14;

    l_14 = (int)(unsigned char)*(signed char *)current_region;
    maploads_load_region(a1);
    l_1C = D_00196A9C;
    l_18 = 0;
L88A11:;
    if (l_18 < *(int *)D_00196A28) goto L88A2D;
    goto L88A57;
L88A1E:;
    l_18++;
    l_1C++;
    goto L88A11;
L88A2D:;
    if ((l_1C->map_id & 1048575) != a2) goto L88A55;
    location_set_discovered(l_18, 1);
    func_0008824A(l_18, 0);
    goto L88A57;
L88A55:;
    goto L88A1E;
L88A57:;
    maploads_load_region(l_14);
}

int region_nth_dungeon(int a1)
{
    struct map_location *l_20;
    int l_1C;

    l_20 = D_00196A9C;
    l_1C = 0;
L88BB8:;
    if (l_1C < *(int *)D_00196A28) goto L88BD4;
    goto L88BFA;
L88BC5:;
    l_1C++;
    l_20++;
    goto L88BB8;
L88BD4:;
    if (l_20->dungeon_type == 255) goto L88BF8;
    a1 += -1;
    if (a1 >= 0) goto L88BF8;
    return l_1C;
L88BF8:;
    goto L88BC5;
L88BFA:;
    return -1;
}

void spfx_create_item_cb(int a1)
{
    struct record *l_1C;
    struct item *l_18;

    l_1C = object_create_child(D_00195AC4, 0, 107);
    l_1C->type = 2;
    l_1C->repair_due = *(int *)D_001A99F4;
    l_18 = &l_1C->data.item;
    if (((int)(short)*(short *)(D_00187F44 + (a1 << 2))) != (-1)) goto L88C7F;
    item_make((int)(unsigned short)*(short *)clothing_gender_group, (int)(short)*(short *)(D_00187F46 + (a1 << 2)), l_18);
    goto L88CA6;
L88C7F:;
    item_make((int)(unsigned short)*(short *)(D_00187F44 + (a1 << 2)), (int)(short)*(short *)(D_00187F46 + (a1 << 2)), l_18);
L88CA6:;
    l_1C->data.item.item_flags |= 0x1000;
    inv_store_item(l_1C);
    if (l_18->group != 3) goto L88CD7;
    if (l_18->index == 18) goto L88CD9;
L88CD7:;
    return;
L88CD9:;
    inv_merge_arrows(player_entity, l_1C, 1);
}

int spfx_paralyze(struct record *a1, int a2, struct record *a3)
{
    struct character *l_18;
    struct spell *l_14;

    l_14 = &a1->data.spell;
    l_18 = &a3->data.character;
    if (l_18->race != 4) goto L88D38;
    return 0;
L88D38:;
    if (l_18 != player_character) goto L88D56;
    if (((int)(unsigned char)(player_class->immunity_flags & 1)) != 0) goto L88D58;
L88D56:;
    goto L88D64;
L88D58:;
    return 0;
L88D64:;
    if ((l_18->conditions & 0x1) == 0) goto L88D79;
    return 0;
L88D79:;
    if ((l_18->conditions & 0x8000) == 0) goto L88D8E;
    return 0;
L88D8E:;
    if (rand_range(1, 100) <= l_14->cast_chances[a2]) goto L88DB5;
    return 0;
L88DB5:;
    l_18->conditions |= 1;
    if (l_18 != player_character) goto L88DD4;
    hud_message_add(*(int *)D_0018461C);
L88DD4:;
    return 1;
}

void spfx_continuous_damage(int a1, int a2, int a3)
{
    spfx_effect_tick(a1, a3, a2);
}

int spfx_damage(struct record *a1, int a2, struct record *a3)
{
    struct spell *l_1C;
    struct character *l_18;
    int l_14;

    l_1C = &a1->data.spell;
    l_18 = &a3->data.character;
    switch (l_1C->effects[a2].subtype) {
case 0:
    if (l_18 != player_character) goto L890A3;
    if (((struct bf8_6_1 *)&cheat_flags)->f != 0) goto L890A5;
L890A3:;
    goto L890AA;
L890A5:;
    goto L8916B;
L890AA:;
    *(short *)D_00195F28 = l_1C->cast_magnitudes[a2];
    damage_apply(a3, l_1C->cast_magnitudes[a2], 0);
    goto L8916B;
case 1:
    *(short *)D_00195F28 = l_1C->cast_magnitudes[a2];
    l_14 = l_18->fatigue;
    l_14 -= l_1C->cast_magnitudes[a2];
    if (l_14 >= 0) goto L8911B;
    l_14 = 0;
L8911B:;
    l_18->fatigue = l_14;
    goto L8916B;
case 2:
    *(short *)D_00195F28 = l_1C->cast_magnitudes[a2];
    l_18->magicka -= l_1C->cast_magnitudes[a2];
    if (l_18->magicka >= 0) goto L8916B;
    l_18->magicka = 0;
default:
L8916B:;
    return 0;
}
}

int spfx_disintegrate(struct record *a1, int a2, struct record *a3)
{
    struct spell *l_14;

    l_14 = &a1->data.spell;
    if (rand_range(1, 100) <= l_14->cast_chances[a2]) goto L891CC;
    hud_message_add(*(int *)D_00185097);
    return 0;
L891CC:;
    damage_creature_death(a3);
    return 0;
}

void spfx_dispel_magic_cb(int a1)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    l_18 = *(struct record **)(D_00190EE4 + (a1 << 2));
    if (l_18->caster == player_entity) goto L89271;
    l_20 = (player_character->level - l_18->caster->data.character.level) * 5;
    l_1C = l_20 + selected_spell->cast_chances[*(int *)D_001A99F4];
    if (l_1C >= 5) goto L89264;
    l_1C = 5;
    goto L89271;
L89264:;
    if (l_1C <= 95) goto L89271;
    l_1C = 95;
L89271:;
    if (l_18->caster == player_entity) goto L89293;
    if (rand_range(1, 100) > l_1C) goto L892A7;
L89293:;
    spell_end(l_18);
    hud_message_add((int)D_00176D20);
    return;
L892A7:;
    hud_message_add((int)D_00176D3E);
}
