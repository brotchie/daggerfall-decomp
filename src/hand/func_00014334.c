/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00014334 */
#include "records.h"

struct sel { int flags; struct record *obj; };
extern char key_down_alt;
extern int frame_counter;
extern int D_00196478;
extern struct sel *pick_result;

int pick_sprite_cb(struct record *object)
{
    int unused1;
    int unused2;
    int unused3;

    if (pick_result->flags & 1)
        return 0;
    switch (object->type) {
    case 2:
    case 8:
    case 18:
    case 33:
    case 34:
    case 44:
    case 53:
        if ((int)object->caster == D_00196478 && (frame_counter & 0xffff) == (object->angle_x & 0xffff)) {
            if (key_down_alt && object->type != 34)
                return 0;
            pick_result->flags |= 3;
            pick_result->obj = object;
            return 1;
        }
    default:
        return 0;
    }
}
