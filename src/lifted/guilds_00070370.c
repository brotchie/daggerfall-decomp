/* guilds.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct character *player_character;
extern int game_minutes;

extern int disease_is_lycanthrope(void);

void guild_count_crime(int a1, int a2)
{
    if (a1 == 5) {
        if (player_character->thieves_invite_time != 0) return;
        if (player_character->thieves_invite_count == 100) return;
        player_character->thieves_invite_count += *(signed char *)&a2;
        if (player_character->thieves_invite_count >= 6) {
            player_character->thieves_invite_time = game_minutes + 4320;
        }
        return;
    }
    if (player_character->max_health_base != 0 && disease_is_lycanthrope() != 0) {
        player_character->lycanthrope_kill_time = game_minutes;
        player_character->max_health = (short)player_character->max_health_base;
    }
    if (player_character->brotherhood_invite_time != 0) return;
    if (player_character->brotherhood_invite_count == 100) return;
    player_character->brotherhood_invite_count += *(signed char *)&a2;
    if (player_character->brotherhood_invite_count < 15) return;
    player_character->brotherhood_invite_time = game_minutes + 4320;
}
