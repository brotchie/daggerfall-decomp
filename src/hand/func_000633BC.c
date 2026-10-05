/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000633BC */
#include "records.h"

extern int sound_play(int, struct record *, int);
extern int rand(void);

void monster_play_sound(struct record *monster, int dist)
{
    struct character *monster_char;
    int sound;

    monster_char = &monster->data.character;
    sound = monster_char->mobile_id * 10 + 10000;
    if (monster_char->flags & 384) {
        if (dist < 128)
            sound_play(rand() & 3 ? sound + 2 : sound + 1, monster, 100);
        else if (rand() < 32000)
            sound_play(sound + 1, monster, 100);
        else
            sound_play(sound, monster, 100);
    } else
        sound_play(sound, monster, 100);
}
