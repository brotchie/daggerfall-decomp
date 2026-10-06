/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006310D */
#include "records.h"
#include "clib.h"

extern struct record *player_object;
extern void monster_play_sound(struct record *, int);
extern int sound_play(int, struct record *, int);
extern int xn_math_approx_dist2d(int, int, int, int);

void monster_ambient_sound(struct record *monster, struct character *monster_char)
{
    int unused[2];
    int dist;

    if (rand() > 195) return;
    dist = xn_math_approx_dist2d(monster->x, monster->z, player_object->x, player_object->z);
    if (dist >= 1024) return;
    if (monster_char->mobile_id == 146) {
        sound_play(11461, monster, 100);
        return;
    }
    if (monster_char->race >= 43) return;
    monster_play_sound(monster, dist);
}
