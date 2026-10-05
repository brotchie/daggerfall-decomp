/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000295BE */
#include "records.h"

extern int game_minutes;
extern void qaction_place_foe(struct qbn_op *, int);
extern int rand(void);

void qaction_op09_spawn_repeat(struct quest *unused, struct qbn_op *o)
{
    struct qbn_foe *s;
    int i;

    if (o->args[4].value == 0)
        return;
    if (game_minutes - o->last_minutes < (unsigned)o->args[2].value)
        return;
    o->last_minutes = game_minutes;
    if (rand() % 100 > o->args[3].section)
        return;
    if (o->args[4].value != -1)
        o->args[4].value--;
    s = (struct qbn_foe *)o->args[1].record;
    for (i = 0; i < (unsigned char)s->count; i++)
        qaction_place_foe(o, 0);
}
