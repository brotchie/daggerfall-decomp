/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000234EB */
#include "records.h"
#include "bitfield.h"

extern unsigned char player_environment;
extern struct collide_probe D_00179F48;
extern unsigned char D_001940D7;
extern signed char player_motion_flags;
extern struct record *player_object;
extern struct character *player_character;
extern char D_00195C70[];
extern char D_00195CB8[];
extern iptr D_00195CD4;
extern iptr D_00195CD8;
extern short D_00195F5A;
extern signed char player_on_ground;
extern signed char D_00196296;
extern int collide_candidate_count;
extern struct collide_probe *D_00196D4C;
extern struct collide_hits *D_00196D50;
extern char collide_flags[];
extern int collide_step_player(struct record *, int, struct move_request *);
extern void collide_for_each_nearby(struct record *, iptr);
extern void collide_gather_cb(struct record *);
extern void automap_mark_seen(int);
extern iptr links_object_motion(iptr);
extern int abs();
extern int xn_vec_normalize_ptr();



#define FLAGS (*(short *)collide_flags)


int collide_move_player(struct record *object, int unused_arg, struct move_request *request, int unused_arg2)
{
    int result;
    int unused1;           /* unused1-3 are never read: they only shape the frame */
    int unused2;
    struct vec3 direction;
    int unused3;
    int dx;
    int dy;
    int dz;
    int best;
    int i;
    int mindot;
    int dot;
    int flags;
    short *motion;

    unused3 = 0;
    *(int *)D_00195CB8 = *(int *)D_00195C70 = 0;
    D_00195F5A = 10000;
    D_00195CD8 = D_00195CD4 = 0;
    D_00196D50 = 0;
    collide_candidate_count = 0;
    (D_00196D4C = &D_00179F48)->position.x = request->x;
    D_00196D4C->position.y = request->y;
    D_00196D4C->position.z = request->z;
    collide_for_each_nearby(object, (iptr)collide_gather_cb);
    if (!((player_character->conditions & 0x8) || collide_candidate_count != 0 || player_environment == 1))
        return FLAGS = 16;
    flags = FLAGS;
    player_on_ground = 1;
    result = collide_step_player(object, unused_arg, request);
    D_001940D7 &= 223;
    if ((char)player_on_ground != 0 && (char)D_00196296 != 0)
        D_00196296 = 0;
    if (*(int *)D_00195CB8 != 0 && (motion = (short *)links_object_motion(*(int *)D_00195CB8)) != 0) {
        if (motion[0] != 0 || motion[2] != 0) {
            request->x += motion[0];
            request->y += motion[1];
            request->z += motion[2];
            *(unsigned char *)collide_flags |= 4;
            result = collide_step_player(object, unused_arg, request);
        }
    }
    if (*(int *)D_00195C70 != 0 && (motion = (short *)links_object_motion(*(int *)D_00195C70)) != 0) {
        request->x += motion[0];
        request->y += motion[1];
        request->z += motion[2];
        *(unsigned char *)collide_flags |= 4;
        result = collide_step_player(object, unused_arg, request);
    }
    if (*(int *)D_00195CB8 != 0)
        automap_mark_seen(*(int *)D_00195CB8);
    if (*(int *)D_00195C70 != 0)
        automap_mark_seen(*(int *)D_00195C70);
    if (result & 2)
        D_00195F5A = 0;
    if (object == player_object && ((struct bf8_5_1 *)&player_motion_flags)->f)
        return 0;
    if (!(result & 10) || !(flags & 4))
        return 0;
    direction.x = request->x - object->x;
    direction.y = request->y - object->y;
    direction.z = request->z - object->z;
    xn_vec_normalize_ptr((iptr)&direction);
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
    D_00195F5A = abs(mindot >> 16);
    if (dx != 0 || dy != 0 || dz != 0) {
        D_00195CD8 = D_00195CD4 = 0;
        flags = FLAGS;
        player_on_ground = 1;
        result = collide_step_player(object, unused_arg, request);
        D_001940D7 &= 223;
        if ((char)player_on_ground != 0 && (char)D_00196296 != 0)
            D_00196296 = 0;
        if (*(int *)D_00195CB8 != 0)
            automap_mark_seen(*(int *)D_00195CB8);
        if (*(int *)D_00195C70 != 0)
            automap_mark_seen(*(int *)D_00195C70);
        if (object == player_object && ((struct bf8_4_1 *)&player_motion_flags)->f)
            return 0;
        if (!(result & 10) || !(flags & 4))
            return 0;
        return 1;
    }
    return FLAGS;
}
