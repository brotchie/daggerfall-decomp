/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000306FE */
#include "records.h"

extern int region_location_count;
extern struct map_location *region_locations;

void qaction_op19_reveal_location(struct quest *quest, struct qbn_op *op, int unused)
{
    struct record *place;
    struct map_location *map_entry;
    int skip_count;
    int i;

    place = op->args[1].object;
    skip_count = place->image;
    map_entry = region_locations;
    for (i = 0; i < region_location_count; i++, map_entry++) {
        if (map_entry->dungeon_type != 255)
            if (skip_count-- == 0) break;
    }
    map_entry->x_type_flags |= 0x40000000;
}
