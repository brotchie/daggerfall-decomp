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
extern void pedestrian_place(int);
extern void guards_summon(int);
extern void text_draw(int, int, int);
extern void guild_count_crime(int, unsigned char);
int people_check_witnesses(void);
#pragma aux mc_set_location parm routine [];

void people_clear(void)
{
    int l_18;

    for (l_18 = 0; l_18 < people_count; l_18++) {
        if (people_list[l_18] != 0) object_delete(people_list[l_18]);
    }
    people_count = 0;
    mc_memset((int)((char *)people_list), 0, 120, (int)D_00170DC0, 554, 120);
}

int people_check_witnesses(void)
{
    int l_20;
    int l_1C;

    l_1C = 0;
    if (((int)player_environment) == 3) return 0;
    for (l_20 = 0; l_20 < people_count; l_20++) {
        if (people_list[l_20] == 0) continue;
        if (is_guard_sprite(people_list[l_20]) != 0) {
            l_1C |= collide_line_of_sight(people_list[l_20], player_object) * 2;
        } else {
            l_1C |= collide_line_of_sight(people_list[l_20], player_object);
        }
    }
    *(signed char *)people_witness_flags = *(signed char *)&l_1C;
    return l_1C;
}

void pedestrian_killed(int a1)
{
    if (people_check_witnesses() != 0) {
        crime_current = 5;
        guards_summon(1);
    }
    pedestrian_place(a1);
    guild_count_crime(6, 5);
}

void people_debug_map(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (game_mode != 0) return;
    if (((int)player_environment) != 1) return;
    xn_draw_image_masked_at_origin(current_location->height << 6, current_location->width << 6, D_00196DA4);
    for (l_20 = 0; l_20 < people_count; l_20++) {
        if (people_list[l_20] == 0) continue;
        l_1C = people_list[l_20]->x - location_object->x;
        l_18 = people_list[l_20]->z - location_object->z;
        l_1C >>= 6;
        l_18 >>= 6;
        l_18 = ((current_location->height << 6) - l_18) - 1;
        func_000A134C((int)(short)*(short *)&l_1C, (int)(short)*(short *)&l_18, 145);
    }
    for (l_20 = 0; l_20 < creature_count; l_20++) {
        l_1C = creature_list[l_20]->x - location_object->x;
        l_18 = creature_list[l_20]->z - location_object->z;
        l_1C >>= 6;
        l_18 >>= 6;
        l_18 = ((current_location->height << 6) - l_18) - 1;
        func_000A134C((int)(short)*(short *)&l_1C, (int)(short)*(short *)&l_18, 161);
    }
    l_1C = player_object->x - location_object->x;
    l_18 = player_object->z - location_object->z;
    l_1C >>= 6;
    l_18 >>= 6;
    l_18 = ((current_location->height << 6) - l_18) - 1;
    func_000A134C((int)(short)*(short *)&l_1C, (int)(short)*(short *)&l_18, 244);
    mc_set_location(677, (int)D_00170DC0);
    mc_sprintf((int)text_buffer, (int)D_00170DD6, l_1C, l_18);
    text_draw((int)text_buffer, 0, (int)&*(signed char *)((char *)(current_location->height << 6) + 2));
}
