/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000820AF */
#include "records.h"

#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
#pragma pack()
extern struct collide_probe D_00187B6E;
extern struct collide_probe D_00187BB8;
extern struct collide_probe D_00187C12;
extern struct collide_probe D_00187C3C;
extern struct move_request D_00187C86;
extern struct bits8 player_motion_flags;
extern struct record *player_object;
extern struct character *player_character;
extern short collide_flags;
extern int D_001A5A60;
extern int D_001A5A64;
extern int move_angle_offset;
extern int collide_move_player(struct record *, int, struct move_request *, int);
extern void xn_math_yaw_offset_xz(int, int, int *, int *);

int player_try_move(int distance)
{
    int dx;
    int dz;
    int result;

    xn_math_yaw_offset_xz((player_object->yaw + move_angle_offset) & 2047, distance << 5, &dx, &dz);
    dx += player_object->x << 5;
    dz += player_object->z << 5;
    dx += D_001A5A64;
    dz += D_001A5A60;
    D_001A5A64 = dx & 31;
    D_001A5A60 = dz & 31;
    D_00187C86.x = dx / 32;
    D_00187C86.y = player_object->y;
    D_00187C86.z = dz / 32;
    D_00187C86.angle_x = player_object->angle_x;
    D_00187C86.yaw = player_object->yaw;
    D_00187C86.angle_z = player_object->angle_z;
    if (D_00187C86.x < 16384 || D_00187C86.x > 32751616 || D_00187C86.z < 16384 || D_00187C86.z > 16367616) {
        collide_flags |= 8;
        return collide_flags;
    }
    D_00187C86.probe = player_motion_flags.b2 ? &D_00187C12 : &D_00187B6E;
    D_00187C86.probe = ((unsigned short)player_character->flags & 1536) != 0 ? &D_00187C3C : D_00187C86.probe;
    if (player_motion_flags.b5)
        D_00187C86.probe = &D_00187BB8;
    if (distance != 0)
        collide_flags |= 4;
    else
        collide_flags &= ~4;
    result = collide_move_player(player_object, 0, &D_00187C86, 1);
    return result;
}
