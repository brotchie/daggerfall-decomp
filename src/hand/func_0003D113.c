/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D113 */
struct skill { short v; short f2; short f4; };
struct player {
    char pad0[157];
    struct skill skills[1];     /* 0x9d */
};
struct pc { char pad0[16]; unsigned char sk[12]; };
extern struct player *player_character;
extern struct pc *player_class;

int level_skill_sum(void)
{
    int i;
    int sum;
    int max;
    int min;

    for (i = sum = 0; i < 6; i++)
        sum += player_character->skills[player_class->sk[i]].v;
    min = player_character->skills[player_class->sk[3]].v;
    if (player_character->skills[player_class->sk[4]].v < min)
        min = player_character->skills[player_class->sk[4]].v;
    if (player_character->skills[player_class->sk[5]].v < min)
        min = player_character->skills[player_class->sk[5]].v;
    sum -= min;
    max = player_character->skills[player_class->sk[6]].v;
    for (i = 7; i < 12; i++)
        if (player_character->skills[player_class->sk[i]].v > max)
            max = player_character->skills[player_class->sk[i]].v;
    sum += max;
    return sum;
}
