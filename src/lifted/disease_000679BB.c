/* disease.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

#include "records.h"

extern signed char scratch_190ce4[];
extern struct record *player_entity;
extern char D_00195B84[];
extern struct character *player_character;

extern void reaction_mod_item_cb(int);
extern void object_foreach(struct record *, int);

int player_reaction_mod(int a1)
{
    signed char l_18;

    scratch_190ce4[0] = *(signed char *)&a1;
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, (int)reaction_mod_item_cb);
    l_18 = player_character->reputation_mod;
    *(int *)D_00195B84 += (int)(signed char)l_18;
    return *(int *)D_00195B84;
}
