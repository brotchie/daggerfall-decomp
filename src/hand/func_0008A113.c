/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008A113 */
#include "records.h"

extern int game_minutes;

int spfx_shield(struct record *a1, int a2, struct record *a3)
{
    struct character *e;
    struct spell *t;

    t = &a1->data.spell;
    e = &a3->data.character;
    e->conditions |= 0x400000;
    e->shield_points = t->cast_magnitudes[a2];
    e->shield_end_time = t->cast_durations[a2] + game_minutes;
    return 1;
}
