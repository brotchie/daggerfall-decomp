/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006FF7E */
#include "records.h"

extern int guild_skill_lists[];
extern struct faction *D_0019671C;
extern int guild_best_skill(int *, int, int);

int guild_join_check(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    if (D_0019671C->reputation < 0)
        return 1;
    l_20 = guild_best_skill(&l_28, guild_skill_lists[a1], -1);
    if (l_20 < 22)
        return 2;
    l_1C = guild_best_skill(&l_28, guild_skill_lists[a1], l_28);
    if (l_1C < 4)
        return 2;
    return 0;
}
