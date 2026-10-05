/* people.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00170DC0[];
extern char D_00170DD6[];
extern unsigned char player_environment;
extern signed char text_buffer[];
extern struct record *D_00190504[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern int creature_count;
extern struct location *current_location;
extern signed char game_mode;
extern signed char crime_current;
extern char D_0019627F[];
extern int D_00196DA4;
extern struct record *people_list[];
extern int people_count;

extern int collide_line_of_sight(struct record *, struct record *);
extern int is_guard_sprite(struct record *);
extern int object_delete(struct record *);
extern int mc_memset();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int func_000A134C();
extern int func_000CDCB8();
extern void person_place(int);
extern void guards_summon(int);
extern void text_draw(int, int, int);
extern void guild_count_crime(int, unsigned char);
int func_00041347(void);
#pragma aux func_000A0ED9 parm routine [];

void people_clear(void)
{
    int l_18;

    for (l_18 = 0; l_18 < people_count; l_18++) {
        if (people_list[l_18] != 0) object_delete(people_list[l_18]);
    }
    people_count = 0;
    mc_memset((int)((char *)people_list), 0, 120, (int)D_00170DC0, 554, 120);
}

int func_00041347(void)
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
    *(signed char *)D_0019627F = *(signed char *)&l_1C;
    return l_1C;
}

void person_killed(int a1)
{
    if (func_00041347() != 0) {
        crime_current = 5;
        guards_summon(1);
    }
    person_place(a1);
    guild_count_crime(6, 5);
}

void func_00041455(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (game_mode != 0) return;
    if (((int)player_environment) != 1) return;
    func_000CDCB8(current_location->height << 6, current_location->width << 6, D_00196DA4);
    for (l_20 = 0; l_20 < people_count; l_20++) {
        if (people_list[l_20] == 0) continue;
        l_1C = people_list[l_20]->x - D_00195AC4->x;
        l_18 = people_list[l_20]->z - D_00195AC4->z;
        l_1C >>= 6;
        l_18 >>= 6;
        l_18 = ((current_location->height << 6) - l_18) - 1;
        func_000A134C((int)(short)*(short *)&l_1C, (int)(short)*(short *)&l_18, 145);
    }
    for (l_20 = 0; l_20 < creature_count; l_20++) {
        l_1C = D_00190504[l_20]->x - D_00195AC4->x;
        l_18 = D_00190504[l_20]->z - D_00195AC4->z;
        l_1C >>= 6;
        l_18 >>= 6;
        l_18 = ((current_location->height << 6) - l_18) - 1;
        func_000A134C((int)(short)*(short *)&l_1C, (int)(short)*(short *)&l_18, 161);
    }
    l_1C = player_object->x - D_00195AC4->x;
    l_18 = player_object->z - D_00195AC4->z;
    l_1C >>= 6;
    l_18 >>= 6;
    l_18 = ((current_location->height << 6) - l_18) - 1;
    func_000A134C((int)(short)*(short *)&l_1C, (int)(short)*(short *)&l_18, 244);
    func_000A0ED9(677, (int)D_00170DC0);
    mc_sprintf((int)text_buffer, (int)D_00170DD6, l_1C, l_18);
    text_draw((int)text_buffer, 0, (int)&*(signed char *)((char *)(current_location->height << 6) + 2));
}
