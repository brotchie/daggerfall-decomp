/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *D_00195AC4;
extern struct location *current_location;

extern struct record *object_find_by_id(struct record *, int);
extern void town_map_note_building(struct record *, struct building *);

void func_0007F0F3(int a1)
{
    int l_20;
    struct building *l_1C;
    struct record *l_18;

    if (a1 == 0) return;
    l_1C = current_location->buildings;
    l_20 = 0;
L7F120:;
    if (current_location->building_count > l_20) goto L7F144;
    return;
L7F135:;
    l_20++;
    l_1C++;
    goto L7F120;
L7F144:;
    if (l_1C->faction_id != a1) goto L7F179;
    l_18 = object_find_by_id(D_00195AC4, l_1C->id);
    if (l_18 == 0) goto L7F179;
    town_map_note_building(l_18, l_1C);
L7F179:;
    goto L7F135;
}
