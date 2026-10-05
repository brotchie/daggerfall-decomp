/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008661C */
#include "records.h"
extern struct record *location_object;
extern struct location *current_location;

int location_contains(int a1, int a2)
{
    if (location_object->image != 0xffff)
        if (a1 > location_object->x && location_object->x + (current_location->width << 12) > a1)
            if (a2 > location_object->z && location_object->z + (current_location->height << 12) > a2)
                return 1;
    return 0;
}
