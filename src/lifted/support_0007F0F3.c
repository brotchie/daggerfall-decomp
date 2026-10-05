/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *location_object;
extern struct location *current_location;

extern struct record *object_find_by_id(struct record *, int);
extern void town_map_note_building(struct record *, struct building *);

void func_0007F0F3(int faction_id)
{
    int building_index;
    struct building *building;
    struct record *building_object;

    if (faction_id == 0) return;
    building = current_location->buildings;
    for (building_index = 0; current_location->building_count > building_index; building_index++, building++) {
        if (building->faction_id == faction_id) {
            building_object = object_find_by_id(location_object, building->id);
            if (building_object != 0) town_map_note_building(building_object, building);
        }
    }
}
