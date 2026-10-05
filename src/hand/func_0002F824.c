/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002F824 */
#include "records.h"

struct move { int x, y, z; int f12, f16, f20; char *name; short flags; char pad[26]; int dz; };
extern char D_00187B44[];
extern int D_00195C74;
extern unsigned char player_on_ground;
extern short collide_flags;
extern int collide_move_object(struct record *, int, struct move *, int);
extern void damage_apply(struct record *, int, int);
extern void func_000CE6E2(int, int, int *, int *);

void damage_knockback_move(struct record *a1, struct character *a2)
{
    struct move m;
    int l_30;
    int l_2C, l_28, l_24, l_20;     /* unused, but they have slots */
    int l_1C;
    int l_18;
    int l_14;

    l_1C = player_on_ground;
    l_18 = D_00195C74;
    if (a2->knockback_speed > 40)
        a2->knockback_speed = 40;
    func_000CE6E2(a2->knockback_angle, a2->knockback_speed > 25 ? 25 : a2->knockback_speed, &l_30, &m.dz);
    m.x = a1->x + l_30;
    m.y = a1->y;
    m.z = a1->z + m.dz;
    m.f12 = a1->angle_x;
    m.f16 = a1->yaw;
    m.f20 = a1->angle_z;
    m.name = D_00187B44;
    m.flags |= 1;
    collide_flags |= 4;
    collide_move_object(a1, 0, &m, 1);
    player_on_ground = l_1C;
    D_00195C74 = l_18;
    l_14 = collide_flags;
    if (l_14 & 2) {
        a2->flags &= ~0x20;
        a2->flags |= 0x800;
        damage_apply(a1, a2->knockback_speed >> 1, 0);
    } else {
        a2->knockback_speed -= 5;
        if (a2->knockback_speed <= 5) {
            a2->flags &= ~0x20;
            a2->flags |= 0x800;
        }
    }
    if (a2->flags & 32)
        a2->action = 16;
}
