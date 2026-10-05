/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000410D7 */
#include "records.h"

extern unsigned char D_0017B667[];
extern short D_0017B66D[][2][4];
extern int climate_category(void);
extern int rand(void);

void person_pick_sprite(struct record *a1)
{
    int kind;
    int flip;

    kind = climate_category();
    flip = rand() & 1;
    if ((rand() & 31) == 0) {
        a1->image = 51072;
        a1->flags &= ~0x4000;
        return;
    }
    a1->image = D_0017B66D[D_0017B667[kind]][flip][rand() & 3] << 7;
    a1->flags |= flip != 0 ? 16384 : 0;
    *((unsigned char *)a1 + 73) |= flip != 0 ? 16 : 0;      /* a person's data +2 flags: bit 4 female */
}
