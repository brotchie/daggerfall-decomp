/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00086B0B */
#include "records.h"
extern int dungeon_water_level;
extern char D_00176C94[];       /* __FILE__ */
extern char D_00176CC9[];
extern int D_00187F2C;
extern signed char text_buffer[];
extern signed char text_rsc_buffer[];
extern unsigned D_0019599C;
extern struct record *D_00195AC4;
extern struct location *current_location;
extern int game_minutes;
extern int D_00195CB8;
extern int D_00195D48;
extern char D_00196289;
extern char D_0019629B;
extern struct map_location *location_here;
extern char loaded_location[];
extern char *loaded_location_object;
extern char *loaded_location_data;
extern void location_load_exterior(char *, int);
extern void town_load_blocks(void);
extern void func_00027947(void);
extern void func_00028D1A(void);
extern void people_spawn_tick(void);
extern void people_clear(void);
extern void parse_rsc_text(int, int, int);
extern void func_0004C759(void);
extern void func_0004CA9D(void);
extern void sound_stop_ambient(void);
extern void position_history_reset(void);
extern int hud_message_add(char *);
extern int rand_range(int, int);
extern void func_0007E74E(void);
extern void location_restore_stored(void);
extern void func_00086A71(struct record *);
extern void location_set_discovered(int, int);
extern void object_foreach(struct record *, void (*)(struct record *));
extern void func_0008EAF1(int, int);
extern void func_0008EB52(void);
extern int player_to_nearest_marker(struct record *, int);
extern void mc_memcpy(void *, void *, int, char *, int, int);
extern int func_0014B45B(int, int);
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int mc_sprintf(char *, char *, ...);

void town_load(int id)
{
    if (D_00195AC4->image == id)
        return;
    sound_stop_ambient();
    func_0008EB52();
    if (D_00195AC4->image == 65535)
        func_0008EAF1((int)D_00195AC4->children, D_00195AC4->id);
    location_load_exterior(loaded_location, id);
    mc_memcpy(D_00195AC4, loaded_location_object, 55, D_00176C94, 365, 4);
    mc_memcpy(current_location, loaded_location_data, 48, D_00176C94, 366, 4);
    D_00195AC4->y = func_0014B45B(D_00195AC4->x, D_00195AC4->z);
    func_00027947();
    town_load_blocks();
    location_set_discovered(id, 1);
    func_0007E74E();
    position_history_reset();
    people_clear();
    func_0004CA9D();
    object_foreach(D_00195AC4, func_00086A71);
    if (D_00187F2C != 0) {
        player_to_nearest_marker(D_00195AC4, 8);
        D_00187F2C = 0;
    }
    if (D_00196289 == 0) {
        people_spawn_tick();
        func_0004C759();
        location_restore_stored();
        func_00028D1A();
        switch ((location_here->x_type_flags << 2) >> 27) {
        case 4:
        case 7:
        case 10:
        case 12:
            parse_rsc_text(location_here->dungeon_type + 520, 0, 0);
            hud_message_add(((char *)text_rsc_buffer));
            break;
        default:
            func_000A0ED9(401, D_00176C94);
            mc_sprintf(((char *)text_buffer), D_00176CC9, current_location->name);
            hud_message_add(((char *)text_buffer));
            break;
        }
    }
    D_00195CB8 = 0;
    D_0019629B = 0;
    D_00195D48 = 10000;
    dungeon_water_level = 10000;
    D_0019599C = game_minutes + rand_range(1400, 1700);
    func_0008EB52();
}
