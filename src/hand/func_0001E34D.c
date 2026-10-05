/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E34D */
#include "records.h"
extern int region_location_count;
extern struct map_location *region_locations;
extern void location_load_dungeon(int, int);

void location_load_nth_dungeon_of_type(int a1, int a2, int a3)
{
    struct map_location *p;
    int i;
    int n;

    p = region_locations;
    for (i = n = 0; i < region_location_count; i++, p++) {
        if (p->dungeon_type == a2) {
            if (a3-- == 0) {
                location_load_dungeon(a1, n);
                return;
            }
            n++;
        }
    }
    location_load_dungeon(a1, 0);
}
