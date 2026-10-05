/* maplogic.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

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

extern int archive_open(char *, int, int);
extern int lockpick_action_door(struct building *, int, struct record *);
extern int region_update_from_player(void);
extern int climate_update_at_player(void);
extern int damage_apply(struct record *, int, struct record *);
extern struct record *quest_find_site_for_building(struct building *);
extern int people_check_witnesses(void);
extern int building_is_open(struct building *);
extern int building_minutes_to_close(struct building *);
extern int disk_read_file(char *, int);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int location_here_contains(int, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
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
extern void fatal_error(char *);
extern void item_make(int, int, struct item *);
extern void links_resolve(void);
extern void sound_stop_ambient(void);
extern void position_history_reset(void);
extern void building_grant_access(struct building *, unsigned char, int);
extern void dungeon_grid_build(void);
extern void town_load(int);
extern void location_unload(int);
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

void location_free(struct loaded_location *location)
{
    if (location->data != 0) {
        if (location->data->buildings != 0) {
            if (location->data->buildings != 0 && (int)location->data->buildings != (-1751672937)) {
                mc_free((int)location->data->buildings, (int)D_00176C94, 83);
                location->data->buildings = (struct building *)-1751672937;
            }
        }
    }
    if (location->object != 0) {
        if (location->object != 0 && (int)location->object != (-1751672937)) {
            mc_free((int)location->object, (int)D_00176C94, 86);
            location->object = (struct record *)-1751672937;
        }
    }
    if (location->doors != 0) {
        if (location->doors != 0 && (int)location->doors != (-1751672937)) {
            mc_free((int)location->doors, (int)D_00176C94, 89);
            location->doors = (struct location_door *)-1751672937;
        }
    }
    mc_memset(location, 0, 20, (int)D_00176C94, 91, 4);
}

struct map_location *region_find_location(int map_id)
{
    struct map_location *location;
    int low;
    int high;
    int mid;

    low = 0;
    high = region_location_count;
    mid = (high + low) >> 1;
    while (high > low) {
        mid = (high + low) >> 1;
        location = (struct map_location *)((char *)region_locations + (mid * 17));
        if ((location->map_id & 1048575) == map_id) return location;
        if ((location->map_id & 1048575) > map_id) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    if ((region_locations[low].map_id & 1048575) == map_id) {
        return (struct map_location *)((char *)region_locations + (low * 17));
    }
    return 0;
}

void world_update_location(void)
{
    int unused;
    int cell;

    if (((int)player_environment) > 2) return;
    terrain_update_cells();
    cell = xn_world_cell_at(player_object->x, player_object->z);
    if (cell != terrain_cell_at_player) {
        terrain_cell_at_player = cell;
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

void dungeon_load(int dungeon_index)
{
    int unused1;
    int location_id;
    int i;
    int unused2;

    sound_stop_ambient();
    if (location_object->image == 65535) object_delete_block((int)location_object->children, location_object->id);
    if (world_loading == 0) {
        player_object->parent_id = player_object->parent->id;
        mc_memcpy((int)saved_player_object, (int)player_object, 55, (int)D_00176C94, 235, 4);
    }
    location_id = location_object->id;
    location_unload(location_object->image);
    if (dungeon_index == (-1)) {
        location_load_dungeon_by_id(&loaded_location, location_id);
    } else {
        location_load_dungeon(&loaded_location, dungeon_index);
    }
    mc_memcpy((int)location_object, (int)loaded_location_object, 55, (int)D_00176C94, 247, 4);
    mc_memcpy((int)current_location, (int)loaded_location_data, 48, (int)D_00176C94, 248, 4);
    blocks_bsa = archive_open(D_00176C9F, 0, 0);
    if ((((unsigned)location_object->id) >> 16) == 50015) D_001967A1 = 254;
    for (i = 0; ((int)(unsigned char)dungeon_block_count) > i; i++) {
        dungeon_load_rdb_block(((int)dungeon_blocks) + (i << 2));
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
    i = player_to_nearest_marker(location_object->children, 8);
    if (i == 0) fatal_error(D_00176CAA);
    func_00028EAA();
    automap_mark_seen(found_object);
    position_history_reset();
    dungeon_roll_monster_tables();
    monsters_woken = 0;
    disk_read_file(D_00176CC0, D_00195CF4);
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

void town_door_roll_unlock(struct record *door)
{
    struct building *building;

    if (door->type != 32) return;
    building = object_building(door);
    switch (building->type) {
case 11:
case 14:
case 15:
    door->lock_level = 0;
    door->flags |= 64;
    return;
default:
    if ((rand() % 100) >= 90) return;
    door->lock_level = 0;
    door->flags |= 64;
}
}

int building_try_enter(struct building *building)
{
    struct record *door;
    int lock_level;
    int unused;

    if (world_loading != 0) return 1;
    if (building->id == player_character->house) return 1;
    if (building->type == 24) return 1;
    if (building->type < 17 && building_is_open(building) != 0) return 1;
    if (building->type < 17 && (building->flags & 16) != 0) return 0;
    if ((building->flags & 16) != 0) return 1;
    if (quest_find_site_for_building(building) != 0 && D_001962A3 != 0) return 1;
    if ((player_character->conditions & 0x40) != 0) {
        player_character->conditions &= ~0x40;
        return 1;
    }
    if (building->faction_id == 108 && guild_find_membership_by_kind(0) != 0) return 1;
    if (building->faction_id == 42 && guild_find_membership_by_kind(3) != 0) return 1;
    if (building->type >= 17 && building->type <= 20 && is_daytime != 0 && (building->id & 65535) % 100 < 50) {
        msgbox_show_rsc(256, 1);
        return 1;
    }
    if (((int)interaction_mode) == 2) {
        lock_level = (building->name_seed % 10) + 3;
        door = object_find_by_id(location_object, building->id);
        if (lockpick_action_door(building, lock_level, door) != 0) {
            if (people_check_witnesses() != 0 || rand_range(1, 300) < (100 - player_character->skills[16].value)) {
                crime_current = 1;
                guards_summon(1);
            }
            return 1;
        }
    }
    lock_show_difficulty((building->name_seed % 10) + 3);
    return 0;
}

void building_load_interior(struct building *building)
{
    struct record *interior;

    interior = object_find_by_id(location_object, building->id);
    object_free_children((struct record *)D_00196120);
    interior_stock_shelves(interior->children, building);
    if (interior == 0) return;
    if (player_to_nearest_marker(interior->children, 6) == 0) {
        hud_message_add((int)D_00176CDD);
        return;
    }
    object_reparent(interior, player_object);
    building_disable_monster_markers(interior);
    people_clear();
}

void building_enter(struct building *building)
{
    if (building == 0) {
        hud_message_add((int)D_00176CFE);
        return;
    }
    D_00195F5E = rand();
    if (building_try_enter(building) == 0) return;
    current_building = building;
    D_00195D84 = *(int *)(D_00186503 + (building->type << 2));
    object_free_children((struct record *)D_00196120);
    if (building->id != player_character->house) {
        building_grant_access(building, 5, (int)(((char *)game_minutes) + building_minutes_to_close(building)));
    } else {
        building_grant_access(building, 255, game_minutes + 10000000);
    }
    if (building->type < 14 && building->type != 1 && building->type != 3) shop_quality_message(building);
    building_load_interior(building);
    sound_stop_ambient();
    position_history_reset();
    player_environment = 2;
    D_001940D5 |= 2;
    D_00195CB8 = 0;
    D_001A4A1D = 0;
    if (world_loading == 0) building_update_open_state();
    if (building->id == D_001A4C94) return;
    stocked_shop_count = 0;
    D_001A4C94 = building->id;
}

void building_exit(void)
{
    struct building *building;
    int unused;

    building = object_building(player_object->parent);
    object_free_children((struct record *)D_00196120);
    building_grant_access(building, 0, 0);
    player_to_nearest_marker(player_object->parent->children, 6);
    func_000C810C(*(int *)D_00195C88);
    object_reparent(location_object, player_object);
    if (building->type == 24) player_to_nearest_marker(location_object, 6);
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

void location_pick_random_town(struct loaded_location *location)
{
    struct map_location *map_location;
    int i;
    int pick;
    int town_count;

    map_location = region_locations;
    town_count = 0;
    mc_memset(location, 0, 20, (int)D_00176C94, 991, 4);
    for (i = 0; i < region_location_count; i++, map_location++) {
        switch ((map_location->x_type_flags << 2) >> 27) {
        case 0:
        case 1:
        case 2:
            town_count++;
        }
    }
    if (town_count == 0) {
        location_free(location);
        return;
    }
    pick = (rand() % town_count) + 1;
    map_location = region_locations;
    for (i = 0; i < region_location_count; i++, map_location++) {
        switch ((map_location->x_type_flags << 2) >> 27) {
        case 0:
        case 1:
        case 2:
            pick--;
        }
        if (pick == 0) {
            location_load_exterior(location, i);
            return;
        }
    }
}

void location_pick_random_undiscovered(struct loaded_location *location)
{
    struct map_location *map_location;
    int i;
    int count;

    map_location = region_locations;
    count = 0;
    mc_memset(location, 0, 20, (int)D_00176C94, 1108, 4);
    for (i = 0; i < region_location_count; i++, map_location++) {
        if ((map_location->x_type_flags & 0x40000000) == 0 && (map_location->x_type_flags & 0x80000000) == 0) {
            count++;
        }
    }
    if (count == 0) {
        count = rand() % region_location_count;
        location_load_exterior(location, i);
        return;
    }
    count = rand() % count;
    map_location = region_locations;
    for (i = 0; i < region_location_count; i++, map_location++) {
        if ((map_location->x_type_flags & 0x40000000) == 0 && (map_location->x_type_flags & 0x80000000) == 0) {
            count--;
        }
        if (count == 0) {
            location_load_exterior(location, i);
            return;
        }
    }
}

void location_set_discovered(int location_index, int discovered)
{
    ((struct bf32_30_1 *)((char *)(int)((location_index * 17) + (char *)region_locations) + 4))->f = discovered;
}

void location_set_hidden(int location_index, int hidden)
{
    ((struct bf32_31_1 *)((char *)(int)((location_index * 17) + (char *)region_locations) + 4))->f = hidden;
}

void location_flatten_terrain(signed char *heights, signed char *flats)
{
    signed char *cursor;
    signed char *end;
    int i;
    int column;
    int row;
    int inset;
    int width;
    int height;
    int level;

    if (((struct bf8_1_5 *)((char *)location_here + 7))->f == 0) {
        inset = 1;
        column = (location_here->x_type_flags & 33554431) + 1536;
        column = (column & 32767) >> 8;
        row = ((location_here->z_size & 16777215) + ((((unsigned)location_here->z_size) >> 28) << 12)) - 1537;
        row = 128 - ((row & 32767) >> 8);
    } else {
        inset = 2;
        column = (location_here->x_type_flags & 33554431) + 3584;
        column = (column & 32767) >> 8;
        row = ((location_here->z_size & 16777215) + (((((unsigned)location_here->z_size) >> 28) - 1) << 12)) + 511;
        row = 128 - ((row & 32767) >> 8);
    }
    cursor = heights;
    cursor += row << 8;
    cursor += column;
    heights = cursor;
    flats += row << 8;
    flats += column;
    width = (((((unsigned)(location_here->z_size << 4)) >> 28) - inset) << 4) + 8;
    height = (((((unsigned)location_here->z_size) >> 28) - inset) << 4) + 8;
    if (((128 - column) - 4) < width) width = (128 - column) - 4;
    if (((128 - row) - 4) < height) height = (128 - row) - 4;
    end = cursor + (height << 8);
    level = 0;
    for (i = 0; i < width; i++, cursor++) {
        level += (unsigned char)(*cursor & 127);
    }
    cursor = heights + 256;
    for (i = 0; (height - 2) > i; i++) {
        level += (unsigned char)(*cursor & 127);
        level += (unsigned char)(*(cursor + width - 1) & 127);
        cursor += 256;
    }
    for (i = 0; i < width; i++, cursor++) {
        level += (unsigned char)(*cursor & 127);
    }
    level = ((unsigned)level) / ((width * 2) + (height * 2) - 4);
    ++level;
    if (((unsigned)level) > 127) level = 127;
    cursor = heights;
    while (cursor < end) {
        mc_memset(cursor, (int)(unsigned char)*(signed char *)&level, width, (int)D_00176C94, 1250, 4);
        mc_memset(flats, 0, width, (int)D_00176C94, 1251, 4);
        cursor += 256;
        flats += 256;
    }
}

void town_block_apply_ground(int x, int z)
{
    unsigned char *tile;
    unsigned char *flat;
    unsigned char *src_tile;
    unsigned char *src_flat;
    int unused;
    int i;
    int count;

    D_001A94C0 = 4;
    for (i = 0; i < 4; i++) {
        if (terrain_cell_at_player == xn_world_slot_cells[i]) D_001A94C0 = i;
    }
    if (D_001A94C0 == 4) return;
    flat = (unsigned char *)(xn_world_flat_layer + D_00187F30[D_001A94C0]);
    tile = (unsigned char *)(xn_world_tile_layer + D_00187F30[D_001A94C0]);
    if ((z & 32767) == 1) {
        z += -2;
        count = 240;
    } else {
        count = 256;
    }
    flat += (128 - ((z & 32767) >> 8)) << 8;
    flat += (x & 32767) >> 8;
    tile += (128 - ((z & 32767) >> 8)) << 8;
    tile += (x & 32767) >> 8;
    src_tile = (unsigned char *)(*(char **)rmb_block + 1739);
    src_flat = (unsigned char *)(*(char **)rmb_block + 1995);
    for (i = 0; i < count; i++, tile++, flat++, src_tile++, src_flat++) {
        if (*src_tile != 255) {
            if ((*src_tile & 63) < 56) {
                *tile = *src_tile;
            }
        }
        if (*src_flat != 255) {
            if ((*src_flat >> 2) < 33) {
                *flat = *src_flat;
            }
        } else {
            *flat &= 3;
        }
        if (((i + 1) % 16) == 0) {
            tile += 240;
            flat += 240;
        }
    }
}

void town_map_add_block(int block_x, int block_y)
{
    int x;
    int y;
    int x_end;
    int y_end;
    int src;
    int map_width;

    map_width = current_location->width << 6;
    block_x <<= 6;
    block_y <<= 6;
    x_end = block_x + 64;
    y_end = block_y + 64;
    src = 0;
    for (y = block_y; y < y_end; y++) {
        for (x = block_x; x < x_end; x++) {
            *(signed char *)((char *)(int)(*(char **)&D_00196DA4 + ((y * map_width) + x))) = *(signed char *)(*(char **)rmb_block + 2251 + src++);
        }
    }
}

struct building *location_find_building(int id)
{
    struct building *building;
    int i;

    building = current_location->buildings;
    for (i = 0; current_location->building_count > i; i++, building++) {
        if (building->id == id) return building;
    }
    return 0;
}

void location_reveal(int region, int map_id)
{
    struct map_location *map_location;
    int i;
    int old_region;

    old_region = (int)(unsigned char)current_region;
    maploads_load_region(region);
    map_location = region_locations;
    for (i = 0; i < region_location_count; i++, map_location++) {
        if ((map_location->map_id & 1048575) == map_id) {
            location_set_discovered(i, 1);
            location_set_hidden(i, 0);
            break;
        }
    }
    maploads_load_region(old_region);
}

int region_nth_dungeon(int n)
{
    struct map_location *map_location;
    int i;

    map_location = region_locations;
    for (i = 0; i < region_location_count; i++, map_location++) {
        if (map_location->dungeon_type != 255) {
            n += -1;
            if (n < 0) return i;
        }
    }
    return -1;
}

void spfx_create_item_cb(int row)
{
    struct record *object;
    struct item *item;

    object = object_create_child(location_object, 0, 107);
    object->type = 2;
    object->repair_due = D_001A99F4;
    item = &object->data.item;
    if (((int)(short)*(short *)(D_00187F44 + (row << 2))) == (-1)) {
        item_make((int)(unsigned short)*(short *)clothing_gender_group, (int)(short)*(short *)(D_00187F46 + (row << 2)), item);
    } else {
        item_make((int)(unsigned short)*(short *)(D_00187F44 + (row << 2)), (int)(short)*(short *)(D_00187F46 + (row << 2)), item);
    }
    object->data.item.item_flags |= 0x1000;
    inv_store_item(object);
    if (item->group != 3 || item->index != 18) return;
    inv_merge_arrows(player_entity, object, 1);
}

int spfx_paralyze(struct record *spell_object, int effect, struct record *target)
{
    struct character *character;
    struct spell *spell;

    spell = &spell_object->data.spell;
    character = &target->data.character;
    if (character->race == 4) return 0;
    if (character == player_character && ((int)(unsigned char)(player_class->immunity_flags & 1)) != 0) {
        return 0;
    }
    if ((character->conditions & 0x1) != 0) return 0;
    if ((character->conditions & 0x8000) != 0) return 0;
    if (rand_range(1, 100) > spell->cast_chances[effect]) return 0;
    character->conditions |= 1;
    if (character == player_character) hud_message_add(D_0018461C);
    return 1;
}

void spfx_continuous_damage(int spell_object, int effect, int target)
{
    spfx_effect_tick(spell_object, target, effect);
}

int spfx_damage(struct record *spell_object, int effect, struct record *target)
{
    struct spell *spell;
    struct character *character;
    int fatigue;

    spell = &spell_object->data.spell;
    character = &target->data.character;
    switch (spell->effects[effect].subtype) {
    case 0:
        if (character == player_character && ((struct bf8_6_1 *)&cheat_flags)->f != 0) break;
        *(short *)D_00195F28 = spell->cast_magnitudes[effect];
        damage_apply(target, spell->cast_magnitudes[effect], 0);
        break;
    case 1:
        *(short *)D_00195F28 = spell->cast_magnitudes[effect];
        fatigue = character->fatigue;
        fatigue -= spell->cast_magnitudes[effect];
        if (fatigue < 0) fatigue = 0;
        character->fatigue = fatigue;
        break;
    case 2:
        *(short *)D_00195F28 = spell->cast_magnitudes[effect];
        character->magicka -= spell->cast_magnitudes[effect];
        if (character->magicka < 0) character->magicka = 0;
    }
    return 0;
}

int spfx_disintegrate(struct record *spell_object, int effect, struct record *target)
{
    struct spell *spell;

    spell = &spell_object->data.spell;
    if (rand_range(1, 100) > spell->cast_chances[effect]) {
        hud_message_add(D_00185097);
        return 0;
    }
    damage_creature_death(target);
    return 0;
}

void spfx_dispel_magic_cb(int row)
{
    int level_bonus;
    int chance;
    struct record *spell;

    spell = *(struct record **)(scratch_190ee4 + (row << 2));
    if (spell->caster != player_entity) {
        level_bonus = (player_character->level - spell->caster->data.character.level) * 5;
        chance = level_bonus + selected_spell->cast_chances[D_001A99F4];
        if (chance < 5) {
            chance = 5;
        } else if (chance > 95) {
            chance = 95;
        }
    }
    if (spell->caster == player_entity || rand_range(1, 100) <= chance) {
        spell_end(spell);
        hud_message_add((int)D_00176D20);
        return;
    }
    hud_message_add((int)D_00176D3E);
}
