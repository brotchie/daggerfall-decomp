/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D113 */
#include "records.h"

extern struct character *player_character;
extern struct career *player_class;

int level_skill_sum(void)
{
    int i;
    int sum;
    int max;
    int min;

    for (i = sum = 0; i < 6; i++)
        sum += player_character->skills[player_class->skills[i]].value;
    min = player_character->skills[player_class->skills[3]].value;
    if (player_character->skills[player_class->skills[4]].value < min)
        min = player_character->skills[player_class->skills[4]].value;
    if (player_character->skills[player_class->skills[5]].value < min)
        min = player_character->skills[player_class->skills[5]].value;
    sum -= min;
    max = player_character->skills[player_class->skills[6]].value;
    for (i = 7; i < 12; i++)
        if (player_character->skills[player_class->skills[i]].value > max)
            max = player_character->skills[player_class->skills[i]].value;
    sum += max;
    return sum;
}
