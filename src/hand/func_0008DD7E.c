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

void object_follow_move_cb(struct record *a1)
{
    int l_20;
    int dx;
    int dz;

    if ((int)a1->parent == 3 || (int)a1->parent == 18)
        return;
    if (a1->type == 52 || a1->parent->type == 52 || a1->parent->type == 22)
        return;
    a1->angle_x = D_001A9B00 + (a1->angle_x - object_move_old_angles) & 2047;
    a1->yaw = D_001A9B04 + (a1->yaw - D_001A9B20) & 2047;
    a1->angle_z = D_001A9B08 + (a1->angle_z - D_001A9B24) & 2047;
    dx = a1->x - object_move_old_x;
    dz = a1->z - object_move_old_z;
    rotate_xz(&dx, &dz, a1->yaw);
    a1->x = object_move_new + dx;
    a1->y = D_001A9AF8 + (a1->y - object_move_old_y);
    a1->z = D_001A9AFC + dz;
}
