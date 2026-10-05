/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009C3E7 */
#include "records.h"

extern unsigned char mouse_buttons;
extern int screen_buffer;
extern char D_0017743D[];
extern short travel_options;
extern unsigned char D_00187CA8;
extern unsigned char D_001889BC;
extern int travel_selected_location;
extern unsigned char scratch_190ce5;
extern unsigned last_skill_check_minutes;
extern struct record *player_object;
extern struct character *player_character;
extern struct career *player_class;
extern unsigned game_minutes;
extern int sky_loaded_frame;
extern unsigned char D_00196271;
extern unsigned char mouse_buttons_prev;
extern unsigned char is_daytime;
extern unsigned char D_00196294;
extern unsigned char night_sky_loaded;
extern unsigned char D_001962A2;
extern unsigned char D_001962A8;
extern unsigned char quests_suspended;
extern struct map_location *region_locations;
extern int D_001AA678;
extern int D_001AA67C;
extern int D_001AA698;
extern int health_status_text(void);
extern void raise_skills(void);
extern void time_pass(int);
extern int sound_play(int, struct record *, int);
extern void msgbox_yes_no_rsc(int);
extern void gold_spend(int);
extern int gold_can_afford(int);
extern void location_place_player_at_edge(unsigned int);
extern void map_goto_location(int, int, int, int);
extern void travel_button_exit(int);
extern void func_0009BE38(void);
extern int travel_route(int, int, int, int, int);
extern int travel_trip_cost(void);
extern void mc_memset(int, int, int, char *, int, int);
extern int xn_math_angle_to_point(int, int, int, int);

void travel_begin_trip(void)
{
    int r;
    int t;
    unsigned saved;
    int dir;

    dir = (((xn_math_angle_to_point(player_object->x, player_object->z, region_locations[travel_selected_location].x_type_flags & 33554431, region_locations[travel_selected_location].z_size & 16777215) >> 2) + 32) & 511) >> 6;
    if (((unsigned char)mouse_buttons & 1) == 0 || ((unsigned char)mouse_buttons_prev & 1) != 0) return;
    r = health_status_text();
    if (r != 0 || D_001962A2 != 0) {
        msgbox_yes_no_rsc(1010);
        if (D_00196271 != 1) return;
    }
    if (gold_can_afford(travel_trip_cost()) != 0)
        gold_spend(travel_trip_cost());
    else
        player_character->gold = 0;
    sound_play(203, player_object, 110);
    scratch_190ce5 = 0;
    func_0009BE38();
    quests_suspended = 1;
    D_00196294 = 1;
    D_00187CA8 = 1;
    D_001962A8 = 1;
    saved = player_character->fatigue;
    D_001AA698 = 1;
    t = travel_route(player_object->x, player_object->z, D_001AA678, D_001AA67C, 1);
    D_001AA698 = 0;
    func_0009BE38();
    travel_button_exit(100);
    if (t != -1)
        map_goto_location(D_001889BC, 1, travel_selected_location, 0);
    if (t != -1 && (unsigned short)(travel_options & 3) == 1 && is_daytime == 0) {
        if (player_character->race != 8) {
            t = game_minutes % 1440;
            if (t >= 1080)
                time_pass(1440 - t + 370);
            else
                time_pass(360 - t + 10);
        }
    }
    if (is_daytime != 0 && (player_character->race == 8 || (player_class->flags & 16) != 0)) {
        t = game_minutes % 1440;
        if (t < 1080)
            time_pass(1080 - t + 10);
    }
    if ((unsigned short)(travel_options & 3) == 1)
        player_character->fatigue = (player_character->attributes[ATTR_STR] + player_character->attributes[ATTR_END]) << 6;
    else
        player_character->fatigue = saved;
    if (t != -1)
        location_place_player_at_edge(dir);
    if (game_minutes - last_skill_check_minutes > 360) {
        last_skill_check_minutes = game_minutes;
        raise_skills();
    }
    mc_memset(655360, 0, 64000, D_0017743D, 782, 4);
    mc_memset(screen_buffer, 0, 64000, D_0017743D, 783, 4);
    D_00196294 = 0;
    D_001962A8 = 0;
    quests_suspended = 0;
    sky_loaded_frame = 10000;
    night_sky_loaded = 0;
}
