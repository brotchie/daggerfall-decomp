/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006FF7E */
#include "records.h"

extern int guild_skill_lists[];
extern struct faction *D_0019671C;
extern int guild_best_skill(int *, int, int);

int guild_join_check(int guild)
{
    int best_id;
    int unused;
    int primary;
    int secondary;

    if (D_0019671C->reputation < 0)
        return 1;
    primary = guild_best_skill(&best_id, guild_skill_lists[guild], -1);
    if (primary < 22)
        return 2;
    secondary = guild_best_skill(&best_id, guild_skill_lists[guild], best_id);
    if (secondary < 4)
        return 2;
    return 0;
}
