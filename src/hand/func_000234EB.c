/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000234EB */
#include "records.h"

struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern unsigned char player_environment;
extern char D_00179F48[];
extern unsigned char D_001940D7;
extern signed char player_motion_flags;
extern struct record *player_object;
extern struct character *player_character;
extern char D_00195C70[];
extern char D_00195CB8[];
extern int D_00195CD4;
extern int D_00195CD8;
extern short D_00195F5A;
extern signed char player_on_ground;
extern signed char D_00196296;
extern int collide_candidate_count;
extern char D_00196D4C[];
extern int D_00196D50;
extern char collide_flags[];
extern int collide_step_player(struct record *, int, struct move_request *);
extern void collide_for_each_nearby(struct record *, int);
extern void collide_gather_cb(int);
extern void automap_mark_seen(int);
extern int links_object_motion(int);
extern int abs();
extern int xn_vec_normalize_ptr();

struct plane {
    int f0;
    int f4;
    int f8;
    int f12;
    int nx;
    int ny;
    int nz;
    short f28;
};

struct planes {
    int count;
    struct plane p[1];
};

#define PLANES (((struct planes *)D_00196D50))
#define PL (PLANES->p)
#define FLAGS (*(short *)collide_flags)

struct vec3 {
    int x;
    int y;
    int z;
};

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
    (*(struct vec3 **)D_00196D4C = (struct vec3 *)D_00179F48)->x = request->x;
    (*(struct vec3 **)D_00196D4C)->y = request->y;
    (*(struct vec3 **)D_00196D4C)->z = request->z;
    collide_for_each_nearby(object, (int)collide_gather_cb);
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
    xn_vec_normalize_ptr((int)&direction);
    if (D_00196D50 == 0)
        return 1;
    if (PLANES->count > 1) {
        best = 0;
        mindot = 1000000;
        for (i = 0; i < PLANES->count; i++) {
            dot = direction.x * PL[i].nx + direction.z * PL[i].nz;
            if (dot < mindot) {
                mindot = dot;
                best = i;
            }
        }
    } else {
        mindot = direction.x * PL[0].nx + direction.z * PL[0].nz;
        best = 0;
    }
    dx = (request->x - object->x) * PL[best].nx;
    dy = (request->y - object->y) * PL[best].ny;
    dz = (request->z - object->z) * PL[best].nz;
    dx = dz + (dx + dy);
    dy = dx * PL[best].ny;
    dz = dx * PL[best].nz;
    dx = dx * PL[best].nx;
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
