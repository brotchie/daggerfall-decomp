/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000404D9 */
#include "records.h"

extern struct location *current_location;
extern iptr D_00196DA4;

int town_map_area_clear(int x, int y)
{
    int blocked;
    int stride;
    unsigned char *pixel;

    blocked = 0;
    x >>= 6;
    y >>= 6;
    y = (current_location->height << 6) - y - 1;
    stride = current_location->height << 6;
    pixel = (unsigned char *)(*(char **)&D_00196DA4 + ((current_location->width << 6) * y + x));
    blocked = pixel[0] | pixel[-1] | pixel[1];
    blocked |= pixel[-stride] | pixel[-stride - 1] | pixel[-stride + 1];
    blocked |= pixel[stride] | pixel[stride - 1] | pixel[stride + 1];
    return (blocked == 0) ? 1 : 0;
}
