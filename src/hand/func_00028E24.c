/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00028E24 */
#include "records.h"

extern unsigned char player_environment;
extern struct record *D_00196DA0;

#define SETBIT(map, n) ((map)[(unsigned)(n) >> 3] |= 1 << ((n) & 7))

void automap_mark_seen(struct record *a1)
{
    char *l_18;

    if (a1 == 0) return;
    if (player_environment != 3) return;
    if (D_00196DA0 == 0) return;
    if ((a1->flags & 128) != 0) return;
    a1->flags |= 128;
    l_18 = RECORD_DATA(D_00196DA0);
    l_18 += 2048;
    SETBIT(l_18, a1->id & 65535);
}
