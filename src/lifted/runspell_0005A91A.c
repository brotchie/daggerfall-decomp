/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_001757F4[];
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct spell *spell_records;
extern struct character *player_character;

extern int spell_cost(struct spell *, struct character *);
extern int cast_player_spell(struct record *);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern int mc_memcpy();

int cast_item_used_spell(int a1)
{
    int l_20;
    struct record *l_1C;

    l_20 = 0;
    l_1C = object_create_child(player_object->parent, 0, 89);
    while (spell_records[l_20].name[0] == 0 || spell_records[l_20].id != a1) l_20++;
    l_1C->type = 9;
    l_1C->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    mc_memcpy(&l_1C->data.spell, &spell_records[l_20], 89, (int)D_001757F4, 103, 4);
    l_20 = spell_cost(&l_1C->data.spell, player_character);
    if (cast_player_spell(l_1C) != 0) object_delete(l_1C);
    return l_20;
}
