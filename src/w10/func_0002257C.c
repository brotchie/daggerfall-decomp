/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002257C */
struct vec3 { int x, y, z; };
struct move { int x, y, z; int f12, f16, f20; };
struct obj { char pad[7]; struct vec3 pos; };   /* 10.0a packs structs (-zp1) */
struct plane { char pad[16]; int nx, ny, nz; char pad2[2]; };
struct planes { int count; struct plane p[1]; };
extern unsigned char D_001789FA;
extern struct vec3 D_00179F48;
extern unsigned char D_001940D7;
extern int D_00195C70;
extern int D_00195CB8;
extern unsigned char D_00196277;
extern int D_00196B0C;
extern struct vec3 *D_00196D4C;
extern struct planes *D_00196D50;
extern int D_00196D60;
extern short D_00196D64;
extern void func_000231F5(struct obj *, void (*)(int));
extern void func_0002325A(int);
extern int func_00023FA5(struct obj *, int, struct move *);
extern void func_0008DEB4(struct obj *, int, int, int, int, int, int);
extern int func_0014B45B(int, int);
extern void func_0014BDDD(struct vec3 *);

int func_0002257C(struct obj *o, int a2, struct move *m, int a4)
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
    D_00196B0C = 0;
    D_00196D4C = &D_00179F48;
    D_00196D4C->x = m->x;
    D_00196D4C->y = m->y;
    D_00196D4C->z = m->z;
    func_000231F5(o, func_0002325A);
    if (D_001789FA != 1 && D_00196B0C == 0)
        return 0;
    if (D_00196B0C == 0 && D_001789FA == 1) {
        D_00196D60 = func_0014B45B(o->pos.x, o->pos.z);
        func_0008DEB4(o, m->x, D_00196D60, m->z, m->f12, m->f16, m->f20);
        return 0;
    }
    flags = D_00196D64;
    D_00196277 = 1;
    result = func_00023FA5(o, a2, m);
    D_001940D7 &= 223;
    D_00195CB8 = saved_cb8;
    D_00195C70 = saved_c70;
    if (!(result & 10) || !(flags & 4))
        return 0;
    d.x = m->x - o->pos.x;
    d.y = m->y - o->pos.y;
    d.z = m->z - o->pos.z;
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
    dx = (m->x - o->pos.x) * D_00196D50->p[best].nx;
    dy = (m->y - o->pos.y) * D_00196D50->p[best].ny;
    dz = (m->z - o->pos.z) * D_00196D50->p[best].nz;
    dx = dz + (dx + dy);
    dy = dx * D_00196D50->p[best].ny;
    dz = dx * D_00196D50->p[best].nz;
    dx = dx * D_00196D50->p[best].nx;
    dx >>= 8;
    dy >>= 8;
    dz >>= 8;
    dx = (m->x - o->pos.x) - (dx >> 8);
    dy = (m->y - o->pos.y) - (dy >> 8);
    dz = (m->z - o->pos.z) - (dz >> 8);
    m->x = o->pos.x + dx;
    m->y = o->pos.y + dy;
    m->z = o->pos.z + dz;
    flags = D_00196D64;
    D_00196277 = 1;
    result = func_00023FA5(o, a2, m);
    D_00195CB8 = saved_cb8;
    D_00195C70 = saved_c70;
    if (!(result & 10) || !(flags & 4))
        return 0;
    return 1;
}
