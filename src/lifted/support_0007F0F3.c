/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *location_object;
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
    for (l_20 = 0; current_location->building_count > l_20; l_20++, l_1C++) {
        if (l_1C->faction_id == a1) {
            l_18 = object_find_by_id(location_object, l_1C->id);
            if (l_18 != 0) town_map_note_building(l_18, l_1C);
        }
    }
}
