/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00086B0B */
#include "records.h"
extern int dungeon_water_level;
extern char D_00176C94[];       /* __FILE__ */
extern char D_00176CC9[];
extern int D_00187F2C;
extern signed char text_buffer[];
extern signed char text_rsc_buffer[];
extern unsigned loan_collectors_next;
extern struct record *location_object;
extern struct location *current_location;
extern int game_minutes;
extern int D_00195CB8;
extern int sky_loaded_frame;
extern char world_loading;
extern char night_sky_loaded;
extern struct map_location *location_here;
extern char loaded_location[];
extern char *loaded_location_object;
extern char *loaded_location_data;
extern void location_load_exterior(char *, int);
extern void town_load_blocks(void);
extern void automap_alloc_town_map(void);
extern void town_map_note_secret_guild_halls(void);
extern void people_spawn_tick(void);
extern void people_clear(void);
extern void parse_rsc_text(int, int, int);
extern void func_0004C759(void);
extern void quest_mark_givers(void);
extern void sound_stop_ambient(void);
extern void position_history_reset(void);
extern iptr hud_message_add(char *);
extern int rand_range(int, int);
extern void town_grid_build(void);
extern void location_restore_stored(void);
extern void town_door_roll_unlock(struct record *);
extern void location_set_discovered(int, int);
extern void object_foreach(struct record *, void (*)());
extern void object_delete_block(iptr, int);
extern void func_0008EB52(void);
extern int player_to_nearest_marker(struct record *, int);
extern void mc_memcpy(void *, void *, int, char *, int, int);
extern int xn_terrain_height_at(int, int);
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);

void town_load(int location_index)
{
    if (location_object->image == location_index)
        return;
    sound_stop_ambient();
    func_0008EB52();
    if (location_object->image == 65535)
        object_delete_block((iptr)location_object->children, location_object->id);
    location_load_exterior(loaded_location, location_index);
    mc_memcpy(location_object, loaded_location_object, 55, D_00176C94, 365, 4);
    mc_memcpy(current_location, loaded_location_data, 48, D_00176C94, 366, 4);
    location_object->y = xn_terrain_height_at(location_object->x, location_object->z);
    automap_alloc_town_map();
    town_load_blocks();
    location_set_discovered(location_index, 1);
    town_grid_build();
    position_history_reset();
    people_clear();
    quest_mark_givers();
    object_foreach(location_object, town_door_roll_unlock);
    if (D_00187F2C != 0) {
        player_to_nearest_marker(location_object, 8);
        D_00187F2C = 0;
    }
    if (world_loading == 0) {
        people_spawn_tick();
        func_0004C759();
        location_restore_stored();
        town_map_note_secret_guild_halls();
        switch ((location_here->x_type_flags << 2) >> 27) {
        case 4:
        case 7:
        case 10:
        case 12:
            parse_rsc_text(location_here->dungeon_type + 520, 0, 0);
            hud_message_add(((char *)text_rsc_buffer));
            break;
        default:
            mc_set_location(401, D_00176C94);
            mc_sprintf(((char *)text_buffer), D_00176CC9, current_location->name);
            hud_message_add(((char *)text_buffer));
            break;
        }
    }
    D_00195CB8 = 0;
    night_sky_loaded = 0;
    sky_loaded_frame = 10000;
    dungeon_water_level = 10000;
    loan_collectors_next = game_minutes + rand_range(1400, 1700);
    func_0008EB52();
}
