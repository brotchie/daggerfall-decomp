/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002F824 */
#include "records.h"

/* a struct move_request (records.h) as this frame holds it: the request, then slots this function
 * leaves unused, then dz (the shared struct moves dz in the frame: it does not match) */
struct move { int x, y, z; int angle_x, yaw, angle_z; char *probe; short flags; char pad[26]; int dz; };
extern char D_00187B44[];
extern int ceiling_height;
extern unsigned char player_on_ground;
extern short collide_flags;
extern int collide_move_object(struct record *, int, struct move *, int);
extern int damage_apply(struct record *, int, struct record *);
extern void xn_math_yaw_offset_xz(int, int, int *, int *);

void damage_knockback_move(struct record *creature, struct character *creature_char)
{
    struct move m;
    int dx;
    int unused1, unused2, unused3, unused4;     /* unused, but they have slots */
    int saved_on_ground;
    int saved_ceiling;
    int hit_flags;

    saved_on_ground = player_on_ground;
    saved_ceiling = ceiling_height;
    if (creature_char->knockback_speed > 40)
        creature_char->knockback_speed = 40;
    xn_math_yaw_offset_xz(creature_char->knockback_angle, creature_char->knockback_speed > 25 ? 25 : creature_char->knockback_speed, &dx, &m.dz);
    m.x = creature->x + dx;
    m.y = creature->y;
    m.z = creature->z + m.dz;
    m.angle_x = creature->angle_x;
    m.yaw = creature->yaw;
    m.angle_z = creature->angle_z;
    m.probe = D_00187B44;
    m.flags |= 1;
    collide_flags |= 4;
    collide_move_object(creature, 0, &m, 1);
    player_on_ground = saved_on_ground;
    ceiling_height = saved_ceiling;
    hit_flags = collide_flags;
    if (hit_flags & 2) {
        creature_char->flags &= ~0x20;
        creature_char->flags |= 0x800;
        damage_apply(creature, creature_char->knockback_speed >> 1, 0);
    } else {
        creature_char->knockback_speed -= 5;
        if (creature_char->knockback_speed <= 5) {
            creature_char->flags &= ~0x20;
            creature_char->flags |= 0x800;
        }
    }
    if (creature_char->flags & 32)
        creature_char->action = 16;
}
