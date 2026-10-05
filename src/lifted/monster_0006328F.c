/* monster.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
extern int D_00187CA4;
extern struct character *player_character;
extern int game_minutes;
extern short D_00195F4E;

extern int rand_range(int, int);
extern void skill_add_uses(int, int);

int ai_stealth_check(int a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;

    if (a3 > 1024) return 0;
    if (game_minutes != player_character->last_stealth_check_minutes) {
        if (a4 != 0) {
            if ((((int)(short)D_00195F4E) >> 1) < D_00187CA4) return 1;
        }
        if ((((((int)(short)D_00195F4E) >> 1) >= D_00187CA4) ? 1 : 0) != 0 && ((struct bf8_0_1 *)&game_minutes)->f != 0) {
            return a2;
        }
        skill_add_uses(16, 1);
        player_character->last_stealth_check_minutes = game_minutes;
        l_10 = player_character->skills[16].value;
        l_10 = ((l_10 * a3) / 1024) * 2;
        l_14 = ((rand_range(1, 100) > l_10) ? 1 : 0);
        return l_14;
    }
    return a2;
}
