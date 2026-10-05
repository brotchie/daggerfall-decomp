/* guilds.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct character *player_character;
extern char game_minutes[];

extern int disease_is_lycanthrope(void);

void guild_count_crime(int a1, int a2)
{
    if (a1 != 5) goto L703F6;
    if (player_character->thieves_invite_time != 0) return;
    if (player_character->thieves_invite_count == 100) return;
    player_character->thieves_invite_count += *(signed char *)&a2;
    if (player_character->thieves_invite_count < 6) goto L703F1;
    player_character->thieves_invite_time = *(int *)game_minutes + 4320;
L703F1:;
    return;
L703F6:;
    if (player_character->max_health_base == 0) goto L7040A;
    if (disease_is_lycanthrope() != 0) goto L7040C;
L7040A:;
    goto L7042D;
L7040C:;
    player_character->lycanthrope_kill_time = *(int *)game_minutes;
    player_character->max_health = (short)player_character->max_health_base;
L7042D:;
    if (player_character->brotherhood_invite_time != 0) return;
    if (player_character->brotherhood_invite_count == 100) return;
    player_character->brotherhood_invite_count += *(signed char *)&a2;
    if (player_character->brotherhood_invite_count < 15) return;
    player_character->brotherhood_invite_time = *(int *)game_minutes + 4320;
}
