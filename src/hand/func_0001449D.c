/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001449D */
#include "records.h"

#pragma pack(1)
struct P { int x; int y; int z; };
extern struct record *player_object;
extern struct P D_00120288;
extern void object_set_position(struct record *, int, int, int, int, int, int);

void player_move_by_xn_vector(void)
{
    int l_1C;
    int l_18;

    l_1C = player_object->x + (D_00120288.x >> 9);
    l_18 = player_object->z + (D_00120288.z >> 9);
    object_set_position(player_object, l_1C, player_object->y, l_18, player_object->angle_x, player_object->yaw, player_object->angle_z);
}
