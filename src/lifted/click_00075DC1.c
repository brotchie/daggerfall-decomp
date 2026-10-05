/* click.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct character *player_character;

extern int quest_pick_file(unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void coven_menu_close(void);

void coven_menu_quest(void)
{
    coven_menu_close();
    quest_pick_file(81, 81, 48, 67, player_character->level);
}
