/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00014334 */
#include "records.h"

struct sel { int flags; struct record *obj; };
extern char key_down_alt;
extern int frame_counter;
extern int D_00196478;
extern struct sel *pick_result;

int pick_sprite_cb(struct record *a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (pick_result->flags & 1)
        return 0;
    switch (a1->type) {
    case 2:
    case 8:
    case 18:
    case 33:
    case 34:
    case 44:
    case 53:
        if ((int)a1->caster == D_00196478 && (frame_counter & 0xffff) == (a1->angle_x & 0xffff)) {
            if (key_down_alt && a1->type != 34)
                return 0;
            pick_result->flags |= 3;
            pick_result->obj = a1;
            return 1;
        }
    default:
        return 0;
    }
}
