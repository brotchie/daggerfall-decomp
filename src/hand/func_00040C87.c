/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00040C87 */
#include "records.h"

extern int xn_timer_fps;
extern unsigned char player_environment;
extern int guards_timer;
extern struct record *player_object;
extern int creature_count;
extern unsigned char people_witness_flags;
extern struct record *people_list[];
extern int people_count;
extern void pedestrian_place(struct record *);
extern int is_guard_sprite(struct record *);
extern void guard_spawn(struct record *);
extern int ai_angle_diff(int, int, int *);
extern int rand_range(int, int);
extern int rand(void);
extern int xn_math_angle_to_point();

void guards_summon(int a1)
{
    int i;
    int d;
    int a;
    int cnt;
    int tmp;

    if (player_environment == 3)
        return;
    if (creature_count > 10)
        return;
    if (a1 != 0) {
        for (cnt = i = 0; i < people_count; i++) {
            if (people_list[i] == 0)
                continue;
            if (is_guard_sprite(people_list[i]) == 0) {
                d = xn_math_angle_to_point(player_object->x, player_object->z, people_list[i]->x, people_list[i]->z);
                a = ai_angle_diff(player_object->yaw, d, &tmp);
                if (a < 600)
                    continue;
            }
            if ((unsigned char)(rand() & 3) == 0 || is_guard_sprite(people_list[i]) != 0) {
                cnt++;
                guard_spawn(people_list[i]);
                pedestrian_place(people_list[i]);
            }
        }
        if (cnt == 0) {
            cnt = rand_range(2, 5);
            for (i = 0; i < cnt; i++)
                guard_spawn(0);
        }
        return;
    }
    if ((int)(unsigned char)(people_witness_flags & 2) != 0) {
        for (i = 0; i < people_count; i++) {
            if (people_list[i] == 0)
                continue;
            if (is_guard_sprite(people_list[i]) != 0) {
                guard_spawn(people_list[i]);
                pedestrian_place(people_list[i]);
            }
        }
        return;
    }
    if ((int)(unsigned char)(people_witness_flags & 1) != 0)
        guards_timer = rand_range(5, 10) * xn_timer_fps;
}
