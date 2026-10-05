/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000404D9 */
#include "records.h"

extern struct location *current_location;
extern int D_00196DA4;

int town_map_area_clear(int x, int y)
{
    int r;
    int w;
    unsigned char *p;

    r = 0;
    x >>= 6;
    y >>= 6;
    y = (current_location->height << 6) - y - 1;
    w = current_location->height << 6;
    p = (unsigned char *)(*(char **)&D_00196DA4 + ((current_location->width << 6) * y + x));
    r = p[0] | p[-1] | p[1];
    r |= p[-w] | p[-w - 1] | p[-w + 1];
    r |= p[w] | p[w - 1] | p[w + 1];
    return (r == 0) ? 1 : 0;
}
