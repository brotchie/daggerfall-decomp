/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008DD7E */
#include "records.h"

extern int object_move_new;
extern int D_001A9AF8;
extern int D_001A9AFC;
extern short D_001A9B00;
extern short D_001A9B04;
extern short D_001A9B08;
extern short object_move_old_angles;
extern short D_001A9B20;
extern short D_001A9B24;
extern int object_move_old_z;
extern int object_move_old_x;
extern int object_move_old_y;
extern void rotate_xz(int *, int *, int);

void object_follow_move_cb(struct record *object)
{
    int unused;
    int dx;
    int dz;

    if ((iptr)object->parent == 3 || (iptr)object->parent == 18)
        return;
    if (object->type == 52 || object->parent->type == 52 || object->parent->type == 22)
        return;
    object->angle_x = D_001A9B00 + (object->angle_x - object_move_old_angles) & 2047;
    object->yaw = D_001A9B04 + (object->yaw - D_001A9B20) & 2047;
    object->angle_z = D_001A9B08 + (object->angle_z - D_001A9B24) & 2047;
    dx = object->x - object_move_old_x;
    dz = object->z - object_move_old_z;
    rotate_xz(&dx, &dz, object->yaw);
    object->x = object_move_new + dx;
    object->y = D_001A9AF8 + (object->y - object_move_old_y);
    object->z = D_001A9AFC + dz;
}
