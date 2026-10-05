/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008227A */
#include "records.h"

struct flags {
    unsigned char b0:1;
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
};
extern struct collide_probe D_00187B6E;
extern struct collide_probe D_00187BB8;
extern struct collide_probe D_00187C12;
extern struct collide_probe D_00187C3C;
extern struct move_request D_00187C86;
extern struct flags player_motion_flags;
extern struct record *player_object;
extern struct character *player_character;
extern unsigned char collide_flags;
extern int collide_move_player(struct record *, int, struct move_request *, int);

int player_try_move_vertical(int dy)
{
    int result;
    int unused;

    D_00187C86.x = player_object->x;
    D_00187C86.y = player_object->y + dy;
    D_00187C86.z = player_object->z;
    D_00187C86.angle_x = player_object->angle_x;
    D_00187C86.yaw = player_object->yaw;
    D_00187C86.angle_z = player_object->angle_z;
    D_00187C86.probe = player_motion_flags.b2 ? &D_00187C12 : &D_00187B6E;
    D_00187C86.probe = (player_character->flags & 1536) ? &D_00187C3C : D_00187C86.probe;
    if (player_motion_flags.b5)
        D_00187C86.probe = &D_00187BB8;
    collide_flags &= 251;
    result = collide_move_player(player_object, 0, &D_00187C86, 1);
    return result;
}
