/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00076F3D */
#include "records.h"

extern struct collide_probe D_00187B44;
extern int ceiling_height;
extern unsigned char player_on_ground;
extern short collide_flags;
extern int collide_move_object(struct record *, int, struct move_request *, int);

int spawn_point_fits(struct record *object)
{
    int saved_ceiling;
    int saved_on_ground;
    struct move_request m;

    saved_on_ground = player_on_ground;
    saved_ceiling = ceiling_height;
    m.x = object->x;
    m.y = object->y;
    m.z = object->z;
    m.angle_x = object->angle_x;
    m.yaw = object->yaw;
    m.angle_z = object->angle_z;
    m.probe = &D_00187B44;
    collide_move_object(object, 0, &m, 0);
    player_on_ground = saved_on_ground;
    ceiling_height = saved_ceiling;
    return (collide_flags & 10) == 0 ? 1 : 0;
}
