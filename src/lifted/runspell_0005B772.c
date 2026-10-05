/* runspell.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"



struct record *spell_find_on_entity(struct record *a1, int a2, int a3)
{
    struct spell *l_14;

    a1 = a1->children;
L5B790:;
    if (a1 == 0) goto L5B816;
    if (a1->type != 9) goto L5B808;
    l_14 = &a1->data.spell;
    if (a3 == 0) goto L5B7CC;
    if (l_14->id == a2) goto L5B7CE;
L5B7CC:;
    goto L5B7DE;
L5B7CE:;
    if (l_14->icon == a3) goto L5B7E0;
L5B7DE:;
    goto L5B7E8;
L5B7E0:;
    return a1;
L5B7E8:;
    if (a3 != 0) goto L5B7FE;
    if (l_14->id == a2) goto L5B800;
L5B7FE:;
    goto L5B808;
L5B800:;
    return a1;
L5B808:;
    a1 = a1->next;
    goto L5B790;
L5B816:;
    return 0;
}
