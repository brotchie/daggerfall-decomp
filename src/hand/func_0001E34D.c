/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001E34D */
#include "records.h"
extern int D_00196A28;
extern struct map_location *D_00196A9C;
extern void location_load_dungeon(int, int);

void func_0001E34D(int a1, int a2, int a3)
{
    struct map_location *p;
    int i;
    int n;

    p = D_00196A9C;
    for (i = n = 0; i < D_00196A28; i++, p++) {
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
