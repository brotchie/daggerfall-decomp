/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00076F3D */
#include "records.h"

struct move { int x, y, z; int f12, f16, f20; char *name; int f28; };
extern char D_00187B44[];
extern int D_00195C74;
extern unsigned char player_on_ground;
extern short collide_flags;
extern int collide_move_object(struct record *, int, struct move *, int);

int func_00076F3D(struct record *a1)
{
    int l_1C;
    int l_20;
    struct move m;

    l_20 = player_on_ground;
    l_1C = D_00195C74;
    m.x = a1->x;
    m.y = a1->y;
    m.z = a1->z;
    m.f12 = a1->angle_x;
    m.f16 = a1->yaw;
    m.f20 = a1->angle_z;
    m.name = D_00187B44;
    collide_move_object(a1, 0, &m, 0);
    player_on_ground = l_20;
    D_00195C74 = l_1C;
    return (collide_flags & 10) == 0 ? 1 : 0;
}
