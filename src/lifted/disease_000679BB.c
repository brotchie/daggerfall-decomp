/* disease.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

#include "records.h"

extern char itemmaker_slot_kinds[];
extern struct record *player_entity;
extern char D_00195B84[];
extern struct character *player_character;

extern void func_00067875(int);
extern void object_foreach(struct record *, int);

int func_000679BB(int a1)
{
    signed char l_18;

    *(signed char *)itemmaker_slot_kinds = *(signed char *)&a1;
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, (int)func_00067875);
    l_18 = player_character->pad224;
    *(int *)D_00195B84 += (int)(signed char)l_18;
    return *(int *)D_00195B84;
}
