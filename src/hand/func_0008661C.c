/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008661C */
#include "records.h"
extern struct record *location_object;
extern struct location *current_location;

int location_contains(int x, int z)
{
    if (location_object->image != 0xffff)
        if (x > location_object->x && location_object->x + (current_location->width << 12) > x)
            if (z > location_object->z && location_object->z + (current_location->height << 12) > z)
                return 1;
    return 0;
}
