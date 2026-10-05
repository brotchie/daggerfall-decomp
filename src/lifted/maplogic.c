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
extern int xn_world_slot_cells[];
extern char *xn_world_flat_layer;
extern char *xn_world_tile_layer;
extern int dungeon_water_level;
extern int xn_light_ambient;
extern char D_00176C94[];
extern char D_00176C9F[];
extern char D_00176CAA[];
extern char D_00176CC0[];
extern char D_00176CDD[];
extern char D_00176CFE[];
extern char D_00176D20[];
extern char D_00176D3E[];
extern unsigned char player_environment;
extern struct spell *selected_spell;
extern int D_0018461C;
extern int D_00185097;
extern char D_00186503[];
extern int D_00187F30[];
extern char D_00187F44[];
extern char D_00187F46[];
extern signed char text_buffer[];
extern signed char D_001903A5;
extern signed char D_001903A6;
extern char scratch_190ee4[];
extern signed char D_001940D5;
extern int D_001950E4;
extern int D_001950E8;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *location_object;
extern char cheat_flags[];
extern struct record *found_object;
extern char clothing_gender_group[];
extern struct location *current_location;
extern struct character *player_character;
extern struct career *player_class;
extern int game_minutes;
extern char scratch_buffer[];
extern char D_00195C88[];
extern struct record *D_00195CB8;
extern int D_00195CF4;
extern int D_00195D84;
extern char D_00195F28[];
extern short D_00195F5E;
extern char saved_player_object[];
extern char D_00196120[];
extern signed char current_region;
extern unsigned char interaction_mode;
extern signed char current_climate;
extern signed char crime_current;
extern signed char is_daytime;
extern signed char world_loading;
extern signed char D_0019628C;
extern signed char D_001962A3;
extern char dungeon_blocks[];
extern signed char D_001967A1;
extern int region_location_count;
extern char rmb_block[];
extern struct map_location *location_here;
extern struct loaded_location loaded_location;
extern struct record *loaded_location_object;
extern struct location *loaded_location_data;
extern struct map_location *region_locations;
extern int blocks_bsa;
extern signed char dungeon_block_count;
extern int D_00196DA4;
extern signed char monsters_woken;
extern int D_001A41DC;
extern int D_001A41E4;
extern struct membership *guild_membership;
extern signed char D_001A4A1D;
extern int stocked_shop_count;
extern int D_001A4C94;
extern int D_001A94C0;
extern int terrain_cell_at_player;
extern int climate_index;
extern int D_001A99F4;

extern int archive_open(int, int, int);
extern int lockpick_action_door(struct building *, int, struct record *);
extern int region_update_from_player(void);
extern int climate_update_at_player(void);
extern int damage_apply(struct record *, int, int);
extern int quest_find_site_for_building(struct building *);
extern int people_check_witnesses(void);
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
extern int xn_world_cell_at();
extern int func_000C810C();
extern int xn_pal_set_range_8bit();
extern int xn_tex_cache_flush();
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
extern void kludge_fix_dungeon_door(void);
extern void building_update_open_state(void);
extern void func_0004C759(void);
extern void quest_mark_givers(void);
extern void fatal_error(int);
extern void item_make(int, int, struct item *);
extern void links_resolve(void);
extern void sound_stop_ambient(void);
extern void position_history_reset(void);
extern void building_grant_access(struct building *, unsigned char, int);
extern void dungeon_grid_build(void);
extern void town_load(int);
extern void location_unload(unsigned short);
extern void terrain_update_cells(void);
extern void climate_set_textures(void);
extern void spell_end(struct record *);
extern void spfx_effect_tick(int, int, int);
extern void object_free_children(struct record *);
extern void object_delete_block(int, int);
extern void inv_store_item(struct record *);
extern void inv_merge_arrows(struct record *, struct record *, int);
extern void shop_quality_message(struct building *);
extern void building_disable_monster_markers(struct record *);
struct map_location *region_find_location(int);
int building_try_enter(struct building *);
void location_free(struct loaded_location *);
void building_load_interior(struct building *);
void location_set_discovered(int, int);
void location_set_hidden(int, int);

void region_unload(void)
{
    automap_save();
    location_unload(location_object->image);
    if ((int)region_locations != 0 && (int)region_locations != (-1751672937)) {
        mc_free((int)region_locations, (int)D_00176C94, 64);
        region_locations = (struct map_location *)-1751672937;
    }
    region_locations = 0;
    region_location_count = 0;
    location_here = 0;
}

void location_free(struct loaded_location *a1)
{
    if (a1->data != 0) {
        if (a1->data->buildings != 0) {
            if (a1->data->buildings != 0 && (int)a1->data->buildings != (-1751672937)) {
                mc_free((int)a1->data->buildings, (int)D_00176C94, 83);
                a1->data->buildings = (struct building *)-1751672937;
            }
        }
    }
    if (a1->object != 0) {
        if (a1->object != 0 && (int)a1->object != (-1751672937)) {
            mc_free((int)a1->object, (int)D_00176C94, 86);
            a1->object = (struct record *)-1751672937;
        }
    }
    if (a1->doors != 0) {
        if (a1->doors != 0 && (int)a1->doors != (-1751672937)) {
            mc_free((int)a1->doors, (int)D_00176C94, 89);
            a1->doors = (char *)-1751672937;
        }
    }
    mc_memset(a1, 0, 20, (int)D_00176C94, 91, 4);
}

struct map_location *region_find_location(int a1)
{
    struct map_location *l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_20 = region_location_count;
    l_1C = (l_20 + l_24) >> 1;
    while (l_20 > l_24) {
        l_1C = (l_20 + l_24) >> 1;
        l_28 = (struct map_location *)((char *)region_locations + (l_1C * 17));
        if ((l_28->map_id & 1048575) == a1) return l_28;
        if ((l_28->map_id & 1048575) > a1) {
            l_20 = l_1C - 1;
        } else {
            l_24 = l_1C + 1;
        }
    }
    if ((region_locations[l_24].map_id & 1048575) == a1) {
        return (struct map_location *)((char *)region_locations + (l_24 * 17));
    }
    return 0;
}

void world_update_location(void)
{
    int l_1C;
    int l_18;

    if (((int)player_environment) > 2) return;
    terrain_update_cells();
    l_18 = xn_world_cell_at(player_object->x, player_object->z);
    if (l_18 != terrain_cell_at_player) {
        terrain_cell_at_player = l_18;
        location_unload(location_object->image);
        region_update_from_player();
        climate_update_at_player();
        climate_index = ((int)(unsigned char)current_climate) - 224;
        location_here = (struct map_location *)((int)region_find_location(terrain_cell_at_player));
        climate_set_textures();
    }
    if ((int)location_here == 0) return;
    if (location_here_contains(player_object->x, player_object->z) != 0) {
        town_load(((unsigned)location_here->map_id) >> 20);
        return;
    }
    location_unload(location_object->image);
}

void dungeon_load(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    sound_stop_ambient();
    if (location_object->image == 65535) object_delete_block((int)location_object->children, location_object->id);
    if (world_loading == 0) {
        player_object->parent_id = player_object->parent->id;
        mc_memcpy((int)saved_player_object, (int)player_object, 55, (int)D_00176C94, 235, 4);
    }
    l_20 = location_object->id;
    location_unload(location_object->image);
    if (a1 == (-1)) {
        location_load_dungeon_by_id(&loaded_location, l_20);
    } else {
        location_load_dungeon(&loaded_location, a1);
    }
    mc_memcpy((int)location_object, (int)loaded_location_object, 55, (int)D_00176C94, 247, 4);
    mc_memcpy((int)current_location, (int)loaded_location_data, 48, (int)D_00176C94, 248, 4);
    blocks_bsa = archive_open((int)D_00176C9F, 0, 0);
    if ((((unsigned)location_object->id) >> 16) == 50015) D_001967A1 = 254;
    for (l_1C = 0; ((int)(unsigned char)dungeon_block_count) > l_1C; l_1C++) {
        dungeon_load_rdb_block(((int)dungeon_blocks) + (l_1C << 2));
    }
    kludge_fix_dungeon_door();
    archive_close(blocks_bsa);
    links_resolve();
    dungeon_grid_build();
    dungeon_choose_textures();
    D_0019628C = 0;
    D_00195CB8 = 0;
    player_environment = 3;
    dungeon_water_level = 10000;
    quest_mark_givers();
    player_object->x = location_object->x;
    player_object->y = location_object->y;
    player_object->z = location_object->z;
    l_1C = player_to_nearest_marker(location_object->children, 8);
    if (l_1C == 0) fatal_error((int)D_00176CAA);
    func_00028EAA();
    automap_mark_seen(found_object);
    position_history_reset();
    dungeon_roll_monster_tables();
    monsters_woken = 0;
    disk_read_file((int)D_00176CC0, D_00195CF4);
    mc_memset(*(int *)scratch_buffer, 0, 93, (int)D_00176C94, 290, 4);
    xn_pal_set_range_8bit(*(int *)scratch_buffer, 1, 31);
    if (world_loading == 0) func_0004C759();
    automap_restore_seen();
    D_001950E4 = marker_count(location_object, 9);
    D_001950E8 = marker_count(location_object, 16);
    xn_light_ambient = 0;
    xn_tex_cache_flush();
    text_buffer[0] = (D_001903A5 = (D_001903A6 = 0));
    xn_pal_set_range_8bit((int)text_buffer, 255, 1);
}

void town_door_roll_unlock(struct record *a1)
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

    if (world_loading != 0) return 1;
    if (a1->id == player_character->house) return 1;
    if (a1->type == 24) return 1;
    if (a1->type < 17 && building_is_open(a1) != 0) return 1;
    if (a1->type < 17 && (a1->flags & 16) != 0) return 0;
    if ((a1->flags & 16) != 0) return 1;
    if (quest_find_site_for_building(a1) != 0 && D_001962A3 != 0) return 1;
    if ((player_character->conditions & 0x40) != 0) {
        player_character->conditions &= ~0x40;
        return 1;
    }
    if (a1->faction_id == 108 && guild_find_membership_by_kind(0) != 0) return 1;
    if (a1->faction_id == 42 && guild_find_membership_by_kind(3) != 0) return 1;
    if (a1->type >= 17 && a1->type <= 20 && is_daytime != 0 && (a1->id & 65535) % 100 < 50) {
        msgbox_show_rsc(256, 1);
        return 1;
    }
    if (((int)interaction_mode) == 2) {
        l_20 = (a1->name_seed % 10) + 3;
        l_24 = object_find_by_id(location_object, a1->id);
        if (lockpick_action_door(a1, l_20, l_24) != 0) {
            if (people_check_witnesses() != 0 || rand_range(1, 300) < (100 - player_character->skills[16].value)) {
                crime_current = 1;
                guards_summon(1);
            }
            return 1;
        }
    }
    lock_show_difficulty((a1->name_seed % 10) + 3);
    return 0;
}

void building_load_interior(struct building *a1)
{
    struct record *l_18;

    l_18 = object_find_by_id(location_object, a1->id);
    object_free_children((struct record *)D_00196120);
    interior_stock_shelves(l_18->children, a1);
    if (l_18 == 0) return;
    if (player_to_nearest_marker(l_18->children, 6) == 0) {
        hud_message_add((int)D_00176CDD);
        return;
    }
    object_reparent(l_18, player_object);
    building_disable_monster_markers(l_18);
    people_clear();
}

void building_enter(struct building *a1)
{
    if (a1 == 0) {
        hud_message_add((int)D_00176CFE);
        return;
    }
    D_00195F5E = rand();
    if (building_try_enter(a1) == 0) return;
    current_building = a1;
    D_00195D84 = *(int *)(D_00186503 + (a1->type << 2));
    object_free_children((struct record *)D_00196120);
    if (a1->id != player_character->house) {
        building_grant_access(a1, 5, (int)(((char *)game_minutes) + building_minutes_to_close(a1)));
    } else {
        building_grant_access(a1, 255, game_minutes + 10000000);
    }
    if (a1->type < 14 && a1->type != 1 && a1->type != 3) shop_quality_message(a1);
    building_load_interior(a1);
    sound_stop_ambient();
    position_history_reset();
    player_environment = 2;
    D_001940D5 |= 2;
    D_00195CB8 = 0;
    D_001A4A1D = 0;
    if (world_loading == 0) building_update_open_state();
    if (a1->id == D_001A4C94) return;
    stocked_shop_count = 0;
    D_001A4C94 = a1->id;
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
    object_reparent(location_object, player_object);
    if (l_1C->type == 24) player_to_nearest_marker(location_object, 6);
    position_history_reset();
    player_environment = 1;
    D_001940D5 |= 2;
    D_001A4A1D = 0;
    current_building = 0;
    guild_membership = 0;
    sound_stop_ambient();
    if (D_001A41E4 == 0) return;
    town_map_note_building(D_001A41E4, D_001A41DC);
    D_001A41E4 = 0;
}

void location_pick_random_town(struct loaded_location *a1)
{
    struct map_location *l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = region_locations;
    l_18 = 0;
    mc_memset(a1, 0, 20, (int)D_00176C94, 991, 4);
    for (l_20 = 0; l_20 < region_location_count; l_20++, l_24++) {
        switch ((l_24->x_type_flags << 2) >> 27) {
        case 0:
        case 1:
        case 2:
            l_18++;
        }
    }
    if (l_18 == 0) {
        location_free(a1);
        return;
    }
    l_1C = (rand() % l_18) + 1;
    l_24 = region_locations;
    for (l_20 = 0; l_20 < region_location_count; l_20++, l_24++) {
        switch ((l_24->x_type_flags << 2) >> 27) {
        case 0:
        case 1:
        case 2:
            l_1C--;
        }
        if (l_1C == 0) {
            location_load_exterior(a1, l_20);
            return;
        }
    }
}

void location_pick_random_undiscovered(struct loaded_location *a1)
{
    struct map_location *l_20;
    int l_1C;
    int l_18;

    l_20 = region_locations;
    l_18 = 0;
    mc_memset(a1, 0, 20, (int)D_00176C94, 1108, 4);
    for (l_1C = 0; l_1C < region_location_count; l_1C++, l_20++) {
        if ((l_20->x_type_flags & 0x40000000) == 0 && (l_20->x_type_flags & 0x80000000) == 0) {
            l_18++;
        }
    }
    if (l_18 == 0) {
        l_18 = rand() % region_location_count;
        location_load_exterior(a1, l_1C);
        return;
    }
    l_18 = rand() % l_18;
    l_20 = region_locations;
    for (l_1C = 0; l_1C < region_location_count; l_1C++, l_20++) {
        if ((l_20->x_type_flags & 0x40000000) == 0 && (l_20->x_type_flags & 0x80000000) == 0) {
            l_18--;
        }
        if (l_18 == 0) {
            location_load_exterior(a1, l_1C);
            return;
        }
    }
}

void location_set_discovered(int a1, int a2)
{
    ((struct bf32_30_1 *)((char *)(int)((a1 * 17) + (char *)region_locations) + 4))->f = a2;
}

void location_set_hidden(int a1, int a2)
{
    ((struct bf32_31_1 *)((char *)(int)((a1 * 17) + (char *)region_locations) + 4))->f = a2;
}

void location_flatten_terrain(int a1, int a2)
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

    if (((struct bf8_1_5 *)((char *)location_here + 7))->f == 0) {
        l_20 = 1;
        l_28 = (location_here->x_type_flags & 33554431) + 1536;
        l_28 = (l_28 & 32767) >> 8;
        l_24 = ((location_here->z_size & 16777215) + ((((unsigned)location_here->z_size) >> 28) << 12)) - 1537;
        l_24 = 128 - ((l_24 & 32767) >> 8);
    } else {
        l_20 = 2;
        l_28 = (location_here->x_type_flags & 33554431) + 3584;
        l_28 = (l_28 & 32767) >> 8;
        l_24 = ((location_here->z_size & 16777215) + (((((unsigned)location_here->z_size) >> 28) - 1) << 12)) + 511;
        l_24 = 128 - ((l_24 & 32767) >> 8);
    }
    l_34 = a1;
    l_34 += l_24 << 8;
    l_34 += l_28;
    a1 = l_34;
    a2 += l_24 << 8;
    a2 += l_28;
    l_1C = (((((unsigned)(location_here->z_size << 4)) >> 28) - l_20) << 4) + 8;
    l_18 = (((((unsigned)location_here->z_size) >> 28) - l_20) << 4) + 8;
    if (((128 - l_28) - 4) < l_1C) l_1C = (128 - l_28) - 4;
    if (((128 - l_24) - 4) < l_18) l_18 = (128 - l_24) - 4;
    l_30 = l_34 + (l_18 << 8);
    l_14 = 0;
    for (l_2C = 0; l_2C < l_1C; l_2C++, l_34++) {
        l_14 += (int)(unsigned char)(*(signed char *)((char *)l_34) & 127);
    }
    l_34 = a1 + 256;
    for (l_2C = 0; (l_18 - 2) > l_2C; l_2C++) {
        l_14 += (int)(unsigned char)(*(signed char *)((char *)l_34) & 127);
        l_14 += (int)(unsigned char)(*(signed char *)((char *)(l_34 + l_1C) - 1) & 127);
        l_34 += 256;
    }
    for (l_2C = 0; l_2C < l_1C; l_2C++, l_34++) {
        l_14 += (int)(unsigned char)(*(signed char *)((char *)l_34) & 127);
    }
    l_14 = ((unsigned)l_14) / ((int)&*(signed char *)((char *)((l_1C * 2) + (l_18 * 2)) - 4));
    ++l_14;
    if (((unsigned)l_14) > 127) l_14 = 127;
    l_34 = a1;
    while (((unsigned)l_34) < l_30) {
        mc_memset(l_34, (int)(unsigned char)*(signed char *)&l_14, l_1C, (int)D_00176C94, 1250, 4);
        mc_memset(a2, 0, l_1C, (int)D_00176C94, 1251, 4);
        l_34 += 256;
        a2 += 256;
    }
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

    D_001A94C0 = 4;
    for (l_18 = 0; l_18 < 4; l_18++) {
        if (terrain_cell_at_player == xn_world_slot_cells[l_18]) D_001A94C0 = l_18;
    }
    if (D_001A94C0 == 4) return;
    l_28 = (int)(xn_world_flat_layer + D_00187F30[D_001A94C0]);
    l_2C = (int)(xn_world_tile_layer + D_00187F30[D_001A94C0]);
    if ((a2 & 32767) == 1) {
        a2 += -2;
        l_14 = 240;
    } else {
        l_14 = 256;
    }
    l_28 += (128 - ((a2 & 32767) >> 8)) << 8;
    l_28 += (a1 & 32767) >> 8;
    l_2C += (128 - ((a2 & 32767) >> 8)) << 8;
    l_2C += (a1 & 32767) >> 8;
    l_24 = *(int *)rmb_block + 1739;
    l_20 = *(int *)rmb_block + 1995;
    for (l_18 = 0; l_18 < l_14; l_18++, l_2C++, l_28++, l_24++, l_20++) {
        if (((int)(unsigned char)*(signed char *)((char *)l_24)) != 255) {
            if (((int)(unsigned char)(*(signed char *)((char *)l_24) & 63)) < 56) {
                *(signed char *)((char *)l_2C) = *(signed char *)((char *)l_24);
            }
        }
        if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 255) {
            if ((((int)(unsigned char)*(signed char *)((char *)l_20)) >> 2) < 33) {
                *(signed char *)((char *)l_28) = *(signed char *)((char *)l_20);
            }
        } else {
            *(signed char *)((char *)l_28) &= 3;
        }
        if (((l_18 + 1) % 16) == 0) {
            l_2C += 240;
            l_28 += 240;
        }
    }
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
    for (l_24 = a2; l_24 < l_1C; l_24++) {
        for (l_28 = a1; l_28 < l_20; l_28++) {
            *(signed char *)((char *)(int)(*(char **)&D_00196DA4 + ((l_24 * l_14) + l_28))) = *(signed char *)(*(char **)rmb_block + 2251 + l_18++);
        }
    }
}

struct building *location_find_building(int a1)
{
    struct building *l_20;
    int l_1C;

    l_20 = current_location->buildings;
    for (l_1C = 0; current_location->building_count > l_1C; l_1C++, l_20++) {
        if (l_20->id == a1) return l_20;
    }
    return 0;
}

void location_reveal(int a1, int a2)
{
    struct map_location *l_1C;
    int l_18;
    int l_14;

    l_14 = (int)(unsigned char)current_region;
    maploads_load_region(a1);
    l_1C = region_locations;
    for (l_18 = 0; l_18 < region_location_count; l_18++, l_1C++) {
        if ((l_1C->map_id & 1048575) == a2) {
            location_set_discovered(l_18, 1);
            location_set_hidden(l_18, 0);
            break;
        }
    }
    maploads_load_region(l_14);
}

int region_nth_dungeon(int a1)
{
    struct map_location *l_20;
    int l_1C;

    l_20 = region_locations;
    for (l_1C = 0; l_1C < region_location_count; l_1C++, l_20++) {
        if (l_20->dungeon_type != 255) {
            a1 += -1;
            if (a1 < 0) return l_1C;
        }
    }
    return -1;
}

void spfx_create_item_cb(int a1)
{
    struct record *l_1C;
    struct item *l_18;

    l_1C = object_create_child(location_object, 0, 107);
    l_1C->type = 2;
    l_1C->repair_due = D_001A99F4;
    l_18 = &l_1C->data.item;
    if (((int)(short)*(short *)(D_00187F44 + (a1 << 2))) == (-1)) {
        item_make((int)(unsigned short)*(short *)clothing_gender_group, (int)(short)*(short *)(D_00187F46 + (a1 << 2)), l_18);
    } else {
        item_make((int)(unsigned short)*(short *)(D_00187F44 + (a1 << 2)), (int)(short)*(short *)(D_00187F46 + (a1 << 2)), l_18);
    }
    l_1C->data.item.item_flags |= 0x1000;
    inv_store_item(l_1C);
    if (l_18->group != 3 || l_18->index != 18) return;
    inv_merge_arrows(player_entity, l_1C, 1);
}

int spfx_paralyze(struct record *a1, int a2, struct record *a3)
{
    struct character *l_18;
    struct spell *l_14;

    l_14 = &a1->data.spell;
    l_18 = &a3->data.character;
    if (l_18->race == 4) return 0;
    if (l_18 == player_character && ((int)(unsigned char)(player_class->immunity_flags & 1)) != 0) {
        return 0;
    }
    if ((l_18->conditions & 0x1) != 0) return 0;
    if ((l_18->conditions & 0x8000) != 0) return 0;
    if (rand_range(1, 100) > l_14->cast_chances[a2]) return 0;
    l_18->conditions |= 1;
    if (l_18 == player_character) hud_message_add(D_0018461C);
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
        if (l_18 == player_character && ((struct bf8_6_1 *)&cheat_flags)->f != 0) break;
        *(short *)D_00195F28 = l_1C->cast_magnitudes[a2];
        damage_apply(a3, l_1C->cast_magnitudes[a2], 0);
        break;
    case 1:
        *(short *)D_00195F28 = l_1C->cast_magnitudes[a2];
        l_14 = l_18->fatigue;
        l_14 -= l_1C->cast_magnitudes[a2];
        if (l_14 < 0) l_14 = 0;
        l_18->fatigue = l_14;
        break;
    case 2:
        *(short *)D_00195F28 = l_1C->cast_magnitudes[a2];
        l_18->magicka -= l_1C->cast_magnitudes[a2];
        if (l_18->magicka < 0) l_18->magicka = 0;
    }
    return 0;
}

int spfx_disintegrate(struct record *a1, int a2, struct record *a3)
{
    struct spell *l_14;

    l_14 = &a1->data.spell;
    if (rand_range(1, 100) > l_14->cast_chances[a2]) {
        hud_message_add(D_00185097);
        return 0;
    }
    damage_creature_death(a3);
    return 0;
}

void spfx_dispel_magic_cb(int a1)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    l_18 = *(struct record **)(scratch_190ee4 + (a1 << 2));
    if (l_18->caster != player_entity) {
        l_20 = (player_character->level - l_18->caster->data.character.level) * 5;
        l_1C = l_20 + selected_spell->cast_chances[D_001A99F4];
        if (l_1C < 5) {
            l_1C = 5;
        } else if (l_1C > 95) {
            l_1C = 95;
        }
    }
    if (l_18->caster == player_entity || rand_range(1, 100) <= l_1C) {
        spell_end(l_18);
        hud_message_add((int)D_00176D20);
        return;
    }
    hud_message_add((int)D_00176D3E);
}
