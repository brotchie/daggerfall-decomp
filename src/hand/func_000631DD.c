/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000631DD */
#include "records.h"

extern struct character *player_character;
extern int monster_sees_invisible(int);
extern int rand_range(int, int);

int ai_sees_through_illusion(int monster_type)
{
    int chance;

    if ((player_character->conditions & 0x3004) == 0)
        return 1;
    if (player_character->conditions & 4)
        return monster_sees_invisible(monster_type);
    if (player_character->conditions & 0x2000)
        chance = 8;
    else
        chance = 4;
    if (monster_sees_invisible(monster_type))
        return 1;
    return rand_range(1, 100) <= chance;
}
