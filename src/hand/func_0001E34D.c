/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E34D */
#include "records.h"
extern int region_location_count;
extern struct map_location *region_locations;
extern void location_load_dungeon(struct loaded_location *, int);

void location_load_nth_dungeon_of_type(struct loaded_location *location, int dungeon_type, int n)
{
    struct map_location *map_location;
    int i;
    int dungeon_index;

    map_location = region_locations;
    for (i = dungeon_index = 0; i < region_location_count; i++, map_location++) {
        if (map_location->dungeon_type == dungeon_type) {
            if (n-- == 0) {
                location_load_dungeon(location, dungeon_index);
                return;
            }
            dungeon_index++;
        }
    }
    location_load_dungeon(location, 0);
}
