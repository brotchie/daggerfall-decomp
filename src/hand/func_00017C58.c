/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00017C58 */
#include "records.h"

extern struct location *current_location;

int town_has_building(short key, int by_type)
{
    struct building *building;
    int i;

    building = current_location->buildings;
    for (i = 0; i < current_location->building_count; i++, building++) {
        if (by_type == 0) {
            if (building->faction_id == key)
                return 1;
        } else {
            if (building->type == key)
                return 1;
        }
    }
    return 0;
}
