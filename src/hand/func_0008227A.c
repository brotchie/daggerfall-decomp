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
#pragma pack(1)
struct place {
    int x, y, z;
    int a, b, c;
    char *name;
};
#pragma pack()
extern char D_00187B6E[];
extern char D_00187BB8[];
extern char D_00187C12[];
extern char D_00187C3C[];
extern struct place D_00187C86;
extern struct flags player_motion_flags;
extern struct record *player_object;
extern struct character *player_character;
extern unsigned char collide_flags;
extern int collide_move_player(struct record *, int, struct place *, int);

int player_try_move_vertical(int a1)
{
    int l_20;
    int l_1C;

    D_00187C86.x = player_object->x;
    D_00187C86.y = player_object->y + a1;
    D_00187C86.z = player_object->z;
    D_00187C86.a = player_object->angle_x;
    D_00187C86.b = player_object->yaw;
    D_00187C86.c = player_object->angle_z;
    D_00187C86.name = player_motion_flags.b2 ? D_00187C12 : D_00187B6E;
    D_00187C86.name = (player_character->flags & 1536) ? D_00187C3C : D_00187C86.name;
    if (player_motion_flags.b5)
        D_00187C86.name = D_00187BB8;
    collide_flags &= 251;
    l_20 = collide_move_player(player_object, 0, &D_00187C86, 1);
    return l_20;
}
