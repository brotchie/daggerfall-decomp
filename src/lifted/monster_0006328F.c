/* monster.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
extern int player_momentum;
extern struct character *player_character;
extern int game_minutes;
extern short player_base_speed;

extern int rand_range(int, int);
extern void skill_add_uses(int, int);

int ai_stealth_check(int monster_type, int detected, int dist, int encountered)
{
    int result;
    int chance;

    if (dist > 1024) return 0;
    if (game_minutes != player_character->last_stealth_check_minutes) {
        if (encountered != 0) {
            if ((((int)(short)player_base_speed) >> 1) < player_momentum) return 1;
        }
        if ((((((int)(short)player_base_speed) >> 1) >= player_momentum) ? 1 : 0) != 0 && ((struct bf8_0_1 *)&game_minutes)->f != 0) {
            return detected;
        }
        skill_add_uses(16, 1);
        player_character->last_stealth_check_minutes = game_minutes;
        chance = player_character->skills[16].value;
        chance = ((chance * dist) / 1024) * 2;
        result = ((rand_range(1, 100) > chance) ? 1 : 0);
        return result;
    }
    return detected;
}
