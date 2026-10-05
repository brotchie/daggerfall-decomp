/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00086560 */
#include "records.h"

extern struct map_location *location_here;

int location_here_contains(int x, int z)
{
    if (location_here->x <= x && location_here->x + (location_here->width << 12) > x)
        if (location_here->z <= z && location_here->z + (location_here->height << 12) > z)
            return location_here->hidden == 0 ? 1 : 0;
    return 0;
}
