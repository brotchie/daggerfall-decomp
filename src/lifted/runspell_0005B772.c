/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"



struct record *spell_find_on_entity(struct record *object, int spell_id, int icon)
{
    struct spell *spell;

    object = object->children;
    while (object != 0) {
        if (object->type == 9) {
            spell = &object->data.spell;
            if (icon != 0 && spell->id == spell_id && spell->icon == icon) return object;
            if (icon == 0 && spell->id == spell_id) return object;
        }
        object = object->next;
    }
    return 0;
}
