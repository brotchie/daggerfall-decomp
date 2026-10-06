/* maplogic.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00176C94[];
extern int region_location_count;
extern struct map_location *region_locations;

extern int location_has_service(iptr, int, int);
extern int rand();
extern int mc_memset();
extern void location_load_exterior(struct loaded_location *, int);
extern void location_free(struct loaded_location *);

void location_pick_random_with_service(struct loaded_location *location, int kind, int sub_kind)
{
    struct map_location *map_location;
    int i;
    int count;
    int pick;

    map_location = region_locations;
    count = 0;
    mc_memset(location, 0, 20, (iptr)D_00176C94, 952, 4);
    for (i = 0; i < region_location_count; i++, map_location++) {
        count += location_has_service((iptr)&map_location->services, kind, sub_kind);
    }
    if (count == 0) {
        location_free(location);
        return;
    }
    map_location = region_locations;
    pick = (rand() % count) + 1;
    for (i = 0; i < region_location_count; i++, map_location++) {
        pick -= location_has_service((iptr)&map_location->services, kind, sub_kind);
        if (pick == 0) {
            location_load_exterior(location, i);
            return;
        }
    }
}
