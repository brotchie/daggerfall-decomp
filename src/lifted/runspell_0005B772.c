/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"



struct record *spell_find_on_entity(struct record *a1, int a2, int a3)
{
    struct spell *l_14;

    a1 = a1->children;
    while (a1 != 0) {
        if (a1->type == 9) {
            l_14 = &a1->data.spell;
            if (a3 != 0 && l_14->id == a2 && l_14->icon == a3) return a1;
            if (a3 == 0 && l_14->id == a2) return a1;
        }
        a1 = a1->next;
    }
    return 0;
}
