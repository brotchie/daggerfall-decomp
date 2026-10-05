/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000843E0 */
#include "records.h"

extern struct record *D_00195AC4;
extern struct location *current_location;
extern struct record *object_create_child(struct record *, struct record *, int);

struct record *rmb_make_door(struct record *a1, short a2, short a3, int a4)
{
    struct record *l_1C;

    l_1C = object_create_child(a1, 0, 62);
    l_1C->type = a4 ? 32 : 6;
    l_1C->lock_level = 0;
    l_1C->image2 = a2;
    l_1C->image = a3;
    *(short *)((char *)l_1C + 19) = 8000;
    l_1C->id = D_00195AC4->id + current_location->object_counter++;
    return l_1C;
}
