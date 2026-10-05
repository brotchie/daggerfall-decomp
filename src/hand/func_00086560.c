/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00086560 */
#include "records.h"

extern struct map_location *location_here;

int location_here_contains(int a1, int a2)
{
    if (location_here->x <= a1 && location_here->x + (location_here->width << 12) > a1)
        if (location_here->z <= a2 && location_here->z + (location_here->height << 12) > a2)
            return location_here->hidden == 0 ? 1 : 0;
    return 0;
}
