/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004A0C0 */
#include "records.h"

extern iptr text_blank;
extern struct location *current_location;
extern int rand_range(int, int);
extern iptr building_name(struct building *);

iptr parse_town_building_name(short building_type)
{
    struct building *building;
    short i;
    short count;

    building = current_location->buildings;
    for (count = i = 0; i < current_location->building_count; i++, building++)
        if (building->type == building_type) count++;
    if (count == 0) return text_blank;
    if (count == 1)
        count = 0;
    else
        count = rand_range(0, count - 1) + 1;
    building = current_location->buildings;
    while (count != 0) {
        while (building->type != building_type) building++;
        count--;
    }
    return building_name(building);
}
