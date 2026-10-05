/* people.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00170DC0[];
extern char D_00170DD6[];
extern unsigned char player_environment;
extern signed char text_buffer[];
extern struct record *creature_list[];
extern struct record *player_object;
extern struct record *location_object;
extern int creature_count;
extern struct location *current_location;
extern signed char game_mode;
extern signed char crime_current;
extern char people_witness_flags[];
extern int D_00196DA4;
extern struct record *people_list[];
extern int people_count;

extern int collide_line_of_sight(struct record *, struct record *);
extern int is_guard_sprite(struct record *);
extern int object_delete(struct record *);
extern int mc_memset();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int func_000A134C();
extern int xn_draw_image_masked_at_origin();
extern void pedestrian_place(struct record *);
extern void guards_summon(int);
extern void text_draw(int, int, int);
extern void guild_count_crime(int, unsigned char);
int people_check_witnesses(void);
#pragma aux mc_set_location parm routine [];

void people_clear(void)
{
    int i;

    for (i = 0; i < people_count; i++) {
        if (people_list[i] != 0) object_delete(people_list[i]);
    }
    people_count = 0;
    mc_memset((int)((char *)people_list), 0, 120, (int)D_00170DC0, 554, 120);
}

int people_check_witnesses(void)
{
    int i;
    int witness_flags;

    witness_flags = 0;
    if (((int)player_environment) == 3) return 0;
    for (i = 0; i < people_count; i++) {
        if (people_list[i] == 0) continue;
        if (is_guard_sprite(people_list[i]) != 0) {
            witness_flags |= collide_line_of_sight(people_list[i], player_object) * 2;
        } else {
            witness_flags |= collide_line_of_sight(people_list[i], player_object);
        }
    }
    people_witness_flags[0] = witness_flags;
    return witness_flags;
}

void pedestrian_killed(struct record *pedestrian)
{
    if (people_check_witnesses() != 0) {
        crime_current = 5;
        guards_summon(1);
    }
    pedestrian_place(pedestrian);
    guild_count_crime(6, 5);
}

void people_debug_map(void)
{
    int i;
    int x;
    int y;

    if (game_mode != 0) return;
    if (((int)player_environment) != 1) return;
    xn_draw_image_masked_at_origin(current_location->height << 6, current_location->width << 6, D_00196DA4);
    for (i = 0; i < people_count; i++) {
        if (people_list[i] == 0) continue;
        x = people_list[i]->x - location_object->x;
        y = people_list[i]->z - location_object->z;
        x >>= 6;
        y >>= 6;
        y = ((current_location->height << 6) - y) - 1;
        func_000A134C((short)x, (short)y, 145);
    }
    for (i = 0; i < creature_count; i++) {
        x = creature_list[i]->x - location_object->x;
        y = creature_list[i]->z - location_object->z;
        x >>= 6;
        y >>= 6;
        y = ((current_location->height << 6) - y) - 1;
        func_000A134C((short)x, (short)y, 161);
    }
    x = player_object->x - location_object->x;
    y = player_object->z - location_object->z;
    x >>= 6;
    y >>= 6;
    y = ((current_location->height << 6) - y) - 1;
    func_000A134C((short)x, (short)y, 244);
    mc_set_location(677, (int)D_00170DC0);
    mc_sprintf((int)text_buffer, (int)D_00170DD6, x, y);
    text_draw((int)text_buffer, 0, (int)&*(signed char *)((char *)(current_location->height << 6) + 2));
}
