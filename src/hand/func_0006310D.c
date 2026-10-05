/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006310D */
#include "records.h"

extern struct record *player_object;
extern void monster_play_sound(struct record *, int);
extern int sound_play(int, struct record *, int);
extern int rand(void);
extern int xn_math_approx_dist2d(int, int, int, int);

void monster_ambient_sound(struct record *a1, struct character *a2)
{
    int l_24[2];
    int l_14;

    if (rand() > 195) return;
    l_14 = xn_math_approx_dist2d(a1->x, a1->z, player_object->x, player_object->z);
    if (l_14 >= 1024) return;
    if (a2->mobile_id == 146) {
        sound_play(11461, a1, 100);
        return;
    }
    if (a2->race >= 43) return;
    monster_play_sound(a1, l_14);
}
