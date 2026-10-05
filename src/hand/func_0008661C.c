/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008661C */
#include "records.h"
extern struct record *D_00195AC4;
extern struct location *current_location;

int location_contains(int a1, int a2)
{
    if (D_00195AC4->image != 0xffff)
        if (a1 > D_00195AC4->x && D_00195AC4->x + (current_location->width << 12) > a1)
            if (a2 > D_00195AC4->z && D_00195AC4->z + (current_location->height << 12) > a2)
                return 1;
    return 0;
}
