/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000820AF */
#include "records.h"

#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct pos {
    int x;
    int y;
    int z;
    int a;
    int b;
    int c;
    char *name;
};
#pragma pack()
extern char D_00187B6E[];
extern char D_00187BB8[];
extern char D_00187C12[];
extern char D_00187C3C[];
extern struct pos D_00187C86;
extern struct bits8 player_motion_flags;
extern struct record *player_object;
extern struct character *player_character;
extern short collide_flags;
extern int D_001A5A60;
extern int D_001A5A64;
extern int move_angle_offset;
extern int collide_move_player(struct record *, int, struct pos *, int);
extern int func_000CE6E2();

int player_try_move(int a1)
{
    int dx;
    int dz;
    int r;

    func_000CE6E2((player_object->yaw + move_angle_offset) & 2047, a1 << 5, &dx, &dz);
    dx += player_object->x << 5;
    dz += player_object->z << 5;
    dx += D_001A5A64;
    dz += D_001A5A60;
    D_001A5A64 = dx & 31;
    D_001A5A60 = dz & 31;
    D_00187C86.x = dx / 32;
    D_00187C86.y = player_object->y;
    D_00187C86.z = dz / 32;
    D_00187C86.a = player_object->angle_x;
    D_00187C86.b = player_object->yaw;
    D_00187C86.c = player_object->angle_z;
    if (D_00187C86.x < 16384 || D_00187C86.x > 32751616 || D_00187C86.z < 16384 || D_00187C86.z > 16367616) {
        collide_flags |= 8;
        return collide_flags;
    }
    D_00187C86.name = player_motion_flags.b2 ? D_00187C12 : D_00187B6E;
    D_00187C86.name = ((unsigned short)player_character->flags & 1536) != 0 ? D_00187C3C : D_00187C86.name;
    if (player_motion_flags.b5)
        D_00187C86.name = D_00187BB8;
    if (a1 != 0)
        collide_flags |= 4;
    else
        collide_flags &= ~4;
    r = collide_move_player(player_object, 0, &D_00187C86, 1);
    return r;
}
