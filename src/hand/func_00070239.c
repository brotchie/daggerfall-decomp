/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00070239 */
#include "records.h"

extern unsigned char D_00190D20;
extern short D_00190DDC;
extern struct record *player_entity;
extern struct record *D_00195AF4;
extern void func_00070191(void);
extern void object_foreach(struct record *, void (*)(void));

struct membership *guild_find_membership_by_faction(short a1)
{
    D_00195AF4 = 0;
    D_00190DDC = a1;
    D_00190D20 = 255;
    object_foreach(player_entity->children, func_00070191);
    if (D_00195AF4 == 0)
        return 0;
    return &D_00195AF4->data.membership;
}
