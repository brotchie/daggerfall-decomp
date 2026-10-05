/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002257C */
#include "records.h"

struct vec3 { int x, y, z; };
struct move { int x, y, z; int f12, f16, f20; };
struct plane { char pad[16]; int nx, ny, nz; char pad2[2]; };
struct planes { int count; struct plane p[1]; };
extern unsigned char player_environment;
extern struct vec3 D_00179F48;
extern unsigned char D_001940D7;
extern int D_00195C70;
extern int D_00195CB8;
extern unsigned char player_on_ground;
extern int collide_candidate_count;
extern struct vec3 *D_00196D4C;
extern struct planes *D_00196D50;
extern int D_00196D60;
extern short collide_flags;
extern void collide_for_each_nearby(struct record *, void (*)(int));
extern void func_0002325A(int);
extern int func_00023FA5(struct record *, int, struct move *);
extern void object_set_position(struct record *, int, int, int, int, int, int);
extern int func_0014B45B(int, int);
extern void func_0014BDDD(struct vec3 *);

int collide_move_object(struct record *o, int a2, struct move *m, int a4)
{
    int result;
    int unused40;           /* never used, but it has a stack slot */
    struct vec3 d;          /* declared here: its place in the list decides the slots */
    int flags;
    int dx;
    int dy;
    int dz;
    int best;
    int i;
    int mindot;
    int dot;
    int unused18;
    int saved_cb8;
    int saved_c70;

    saved_cb8 = D_00195CB8;
    saved_c70 = D_00195C70;
    D_00196D50 = 0;
    collide_candidate_count = 0;
    D_00196D4C = &D_00179F48;
    D_00196D4C->x = m->x;
    D_00196D4C->y = m->y;
    D_00196D4C->z = m->z;
    collide_for_each_nearby(o, func_0002325A);
    if (player_environment != 1 && collide_candidate_count == 0)
        return 0;
    if (collide_candidate_count == 0 && player_environment == 1) {
        D_00196D60 = func_0014B45B(o->x, o->z);
        object_set_position(o, m->x, D_00196D60, m->z, m->f12, m->f16, m->f20);
        return 0;
    }
    flags = collide_flags;
    player_on_ground = 1;
    result = func_00023FA5(o, a2, m);
    D_001940D7 &= 223;
    D_00195CB8 = saved_cb8;
    D_00195C70 = saved_c70;
    if (!(result & 10) || !(flags & 4))
        return 0;
    d.x = m->x - o->x;
    d.y = m->y - o->y;
    d.z = m->z - o->z;
    func_0014BDDD(&d);
    if (D_00196D50 == 0)
        return 1;
    if (D_00196D50->count > 1) {
        best = 0;
        mindot = 1000000;
        for (i = 0; i < D_00196D50->count; i++) {
            dot = d.x * D_00196D50->p[i].nx + d.z * D_00196D50->p[i].nz;
            if (dot < mindot) {
                mindot = dot;
                best = i;
            }
        }
    } else {
        mindot = d.x * D_00196D50->p[0].nx + d.z * D_00196D50->p[0].nz;
        best = 0;
    }
    dx = (m->x - o->x) * D_00196D50->p[best].nx;
    dy = (m->y - o->y) * D_00196D50->p[best].ny;
    dz = (m->z - o->z) * D_00196D50->p[best].nz;
    dx = dz + (dx + dy);
    dy = dx * D_00196D50->p[best].ny;
    dz = dx * D_00196D50->p[best].nz;
    dx = dx * D_00196D50->p[best].nx;
    dx >>= 8;
    dy >>= 8;
    dz >>= 8;
    dx = (m->x - o->x) - (dx >> 8);
    dy = (m->y - o->y) - (dy >> 8);
    dz = (m->z - o->z) - (dz >> 8);
    m->x = o->x + dx;
    m->y = o->y + dy;
    m->z = o->z + dz;
    flags = collide_flags;
    player_on_ground = 1;
    result = func_00023FA5(o, a2, m);
    D_00195CB8 = saved_cb8;
    D_00195C70 = saved_c70;
    if (!(result & 10) || !(flags & 4))
        return 0;
    return 1;
}
