/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00017C58 */
#include "records.h"

extern struct location *current_location;

int town_has_building(short a1, int a2)
{
    struct building *p;
    int i;

    p = current_location->buildings;
    for (i = 0; i < current_location->building_count; i++, p++) {
        if (a2 == 0) {
            if (p->faction_id == a1)
                return 1;
        } else {
            if (p->type == a1)
                return 1;
        }
    }
    return 0;
}
