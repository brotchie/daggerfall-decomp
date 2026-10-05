/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001449D */
#include "records.h"

extern struct record *player_object;
extern struct vec3 D_00120288;
extern void object_set_position(struct record *, int, int, int, int, int, int);

void player_move_by_xn_vector(void)
{
    int x;
    int z;

    x = player_object->x + (D_00120288.x >> 9);
    z = player_object->z + (D_00120288.z >> 9);
    object_set_position(player_object, x, player_object->y, z, player_object->angle_x, player_object->yaw, player_object->angle_z);
}
