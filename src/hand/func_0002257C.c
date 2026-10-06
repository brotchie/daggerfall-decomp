/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002257C */
#include "records.h"

extern unsigned char player_environment;
extern struct collide_probe D_00179F48;
extern unsigned char D_001940D7;
extern iptr D_00195C70;
extern iptr D_00195CB8;
extern unsigned char player_on_ground;
extern int collide_candidate_count;
extern struct collide_probe *D_00196D4C;
extern struct collide_hits *D_00196D50;
extern int collide_height;
extern short collide_flags;
extern void collide_for_each_nearby(struct record *, void (*)(iptr));
extern void collide_gather_cb(iptr);
extern int func_00023FA5(struct record *, int, struct move_request *);
extern void object_set_position(struct record *, int, int, int, int, int, int);
extern int xn_terrain_height_at(int, int);
extern void xn_vec_normalize_ptr(struct vec3 *);

int collide_move_object(struct record *object, int unused_arg, struct move_request *request, int unused_arg2)
{
    int result;
    int unused40;           /* never used, but it has a stack slot */
    struct vec3 direction;          /* declared here: its place in the list decides the slots */
    int flags;
    int dx;
    int dy;
    int dz;
    int best;
    int i;
    int mindot;
    int dot;
    int unused18;
    iptr saved_cb8;
    iptr saved_c70;

    saved_cb8 = D_00195CB8;
    saved_c70 = D_00195C70;
    D_00196D50 = 0;
    collide_candidate_count = 0;
    D_00196D4C = &D_00179F48;
    D_00196D4C->position.x = request->x;
    D_00196D4C->position.y = request->y;
    D_00196D4C->position.z = request->z;
    collide_for_each_nearby(object, collide_gather_cb);
    if (player_environment != 1 && collide_candidate_count == 0)
        return 0;
    if (collide_candidate_count == 0 && player_environment == 1) {
        collide_height = xn_terrain_height_at(object->x, object->z);
        object_set_position(object, request->x, collide_height, request->z, request->angle_x, request->yaw, request->angle_z);
        return 0;
    }
    flags = collide_flags;
    player_on_ground = 1;
    result = func_00023FA5(object, unused_arg, request);
    D_001940D7 &= 223;
    D_00195CB8 = saved_cb8;
    D_00195C70 = saved_c70;
    if (!(result & 10) || !(flags & 4))
        return 0;
    direction.x = request->x - object->x;
    direction.y = request->y - object->y;
    direction.z = request->z - object->z;
    xn_vec_normalize_ptr(&direction);
    if (D_00196D50 == 0)
        return 1;
    if (D_00196D50->count > 1) {
        best = 0;
        mindot = 1000000;
        for (i = 0; i < D_00196D50->count; i++) {
            dot = direction.x * D_00196D50->hits[i].nx + direction.z * D_00196D50->hits[i].nz;
            if (dot < mindot) {
                mindot = dot;
                best = i;
            }
        }
    } else {
        mindot = direction.x * D_00196D50->hits[0].nx + direction.z * D_00196D50->hits[0].nz;
        best = 0;
    }
    dx = (request->x - object->x) * D_00196D50->hits[best].nx;
    dy = (request->y - object->y) * D_00196D50->hits[best].ny;
    dz = (request->z - object->z) * D_00196D50->hits[best].nz;
    dx = dz + (dx + dy);
    dy = dx * D_00196D50->hits[best].ny;
    dz = dx * D_00196D50->hits[best].nz;
    dx = dx * D_00196D50->hits[best].nx;
    dx >>= 8;
    dy >>= 8;
    dz >>= 8;
    dx = (request->x - object->x) - (dx >> 8);
    dy = (request->y - object->y) - (dy >> 8);
    dz = (request->z - object->z) - (dz >> 8);
    request->x = object->x + dx;
    request->y = object->y + dy;
    request->z = object->z + dz;
    flags = collide_flags;
    player_on_ground = 1;
    result = func_00023FA5(object, unused_arg, request);
    D_00195CB8 = saved_cb8;
    D_00195C70 = saved_c70;
    if (!(result & 10) || !(flags & 4))
        return 0;
    return 1;
}
