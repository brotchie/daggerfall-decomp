/* monster.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
extern char D_00187CA4[];
extern struct character *player_character;
extern char game_minutes[];
extern char D_00195F4E[];

extern int rand_range(int, int);
extern void skill_add_uses(int, int);

int ai_stealth_check(int a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;

    if (a3 <= 1024) goto L632BB;
    return 0;
L632BB:;
    if (*(int *)game_minutes == player_character->last_stealth_check_minutes) goto L633AC;
    if (a4 == 0) goto L632F5;
    if ((((int)(short)*(short *)D_00195F4E) >> 1) >= *(int *)D_00187CA4) goto L632F5;
    return 1;
L632F5:;
    if ((((((int)(short)*(short *)D_00195F4E) >> 1) >= *(int *)D_00187CA4) ? 1 : 0) == 0) goto L63325;
    if (((struct bf8_0_1 *)&game_minutes)->f != 0) goto L63327;
L63325:;
    goto L63332;
L63327:;
    return a2;
L63332:;
    skill_add_uses(16, 1);
    player_character->last_stealth_check_minutes = *(int *)game_minutes;
    l_10 = player_character->skills[16].value;
    l_10 = ((l_10 * a3) / 1024) * 2;
    l_14 = ((rand_range(1, 100) > l_10) ? 1 : 0);
    return l_14;
L633AC:;
    return a2;
}
