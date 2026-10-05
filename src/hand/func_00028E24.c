/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00028E24 */
#include "records.h"

extern unsigned char player_environment;
extern struct record *D_00196DA0;

#define SETBIT(map, n) ((map)[(unsigned)(n) >> 3] |= 1 << ((n) & 7))

void automap_mark_seen(struct record *object)
{
    char *seen_bits;

    if (object == 0) return;
    if (player_environment != 3) return;
    if (D_00196DA0 == 0) return;
    if ((object->flags & 128) != 0) return;
    object->flags |= 128;
    seen_bits = RECORD_DATA(D_00196DA0);
    seen_bits += 2048;
    SETBIT(seen_bits, object->id & 65535);
}
