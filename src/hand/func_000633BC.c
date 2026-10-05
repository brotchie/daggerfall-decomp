/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000633BC */
#include "records.h"

extern void sound_play(int, struct record *, int);
extern int rand(void);

void monster_play_sound(struct record *a1, int a2)
{
    struct character *m;
    int snd;

    m = &a1->data.character;
    snd = m->mobile_id * 10 + 10000;
    if (m->flags & 384) {
        if (a2 < 128)
            sound_play(rand() & 3 ? snd + 2 : snd + 1, a1, 100);
        else if (rand() < 32000)
            sound_play(snd + 1, a1, 100);
        else
            sound_play(snd, a1, 100);
    } else
        sound_play(snd, a1, 100);
}
