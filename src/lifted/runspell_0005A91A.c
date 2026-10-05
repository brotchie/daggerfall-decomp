/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_001757F4[];
extern struct record *player_object;
extern struct record *location_object;
extern struct spell *spell_records;
extern struct character *player_character;

extern int spell_cost(struct spell *, struct character *);
extern int cast_player_spell(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_new_id(int);
extern int mc_memcpy();

int cast_item_used_spell(int spell_id)
{
    int i;
    struct record *spell;

    i = 0;
    spell = object_create_child(player_object->parent, 0, 89);
    while (spell_records[i].name[0] == 0 || spell_records[i].id != spell_id) i++;
    spell->type = 9;
    spell->id = object_new_id(((unsigned)location_object->id) >> 16);
    mc_memcpy(&spell->data.spell, &spell_records[i], 89, (int)D_001757F4, 103, 4);
    i = spell_cost(&spell->data.spell, player_character);
    if (cast_player_spell(spell) != 0) object_delete(spell);
    return i;
}
