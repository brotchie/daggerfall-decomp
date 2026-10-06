/* collide.c: XnGine's geometric collision tests as readable C (xcollide.h; see xngine.h and
   docs/xngine_readable.md): planes, spheres, points and faces. Most of them sum 64-bit
   products and keep 16 fraction bits, rounded (+8000h) unless said otherwise. */
#include "xcollide.h"
#include "xvec.h"

/* ---- arithmetic ------------------------------------------------------------------------- */

/* (*s + 8000h) >> 16: the low dword of a rounded 64-bit sum without its fraction */
static s32 round16(const xn_s64 *s)
{
    xn_s64 t = *s;

    xn_s64_addu(&t, 0x8000);
    return xn_s64_shr(&t, 16);
}

/* a * b, rounded >> 16 */
static s32 mul_round16(s32 a, s32 b)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    return round16(&t);
}

/* *s = n . (a - b), 64-bit */
static void dot_diff(xn_s64 *s, const xn_vec3 *n, const xn_vec3 *a, const xn_vec3 *b)
{
    xn_s64_mul(s, n->x, a->x - b->x);
    xn_s64_mac(s, n->y, a->y - b->y);
    xn_s64_mac(s, n->z, a->z - b->z);
}

/* *s = |v|^2, 64-bit */
static void length2(xn_s64 *s, const xn_vec3 *v)
{
    xn_s64_mul(s, v->x, v->x);
    xn_s64_mac(s, v->y, v->y);
    xn_s64_mac(s, v->z, v->z);
}

/* *d = a - b */
static void vec_sub(xn_vec3 *d, const xn_vec3 *a, const xn_vec3 *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}

/* |v| as `neg` leaves it (the most negative value stays negative) */
static s32 magnitude(s32 v)
{
    return v < 0 ? -v : v;
}

/* a vector from three registers of an asm caller */
static void regs_vec(xn_vec3 *v, u32 x, u32 y, u32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

/* ---- planes ----------------------------------------------------------------------------- */

/* A segment against a plane, as 14C2AD (and its vertical case 14C3EC) computes it, with the
   intermediate values its asm callers see in the registers */
struct seg_plane {
    xn_s64 d0, d1;      /* n . (c - p0), n . (c - p1): the ends' sides */
    s32 den;            /* n . (p1 - p0), rounded >> 16 (vertical: the y term only) */
    u32 den_xy;         /* the low dword of den's x and y products' sum (not vertical) */
    s32 t;              /* d0 / den (16.16), -1 for no crossing */
    int stage;          /* 1 the same side, 2 nearly parallel, 3 crossed */
    xn_vec3 hit;
};

static void segment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                          const xn_vec3 *p1, int vertical, struct seg_plane *s)
{
    xn_vec3 d;
    xn_s64 den;

    xn_collide_normal = *n;
    dot_diff(&s->d0, n, c, p0);
    dot_diff(&s->d1, n, c, p1);
    s->t = -1;
    if ((s->d0.hi ^ s->d1.hi) >= 0) {
        s->stage = 1;
        return;
    }
    vec_sub(&d, p1, p0);
    if (vertical) {
        s->den = mul_round16(n->y, d.y);
    } else {
        s->den_xy = n->x * d.x + n->y * d.y;
        xn_s64_mul(&den, n->x, d.x);
        xn_s64_mac(&den, n->y, d.y);
        xn_s64_mac(&den, n->z, d.z);
        s->den = round16(&den);
    }
    if (((s->den - 8) ^ (s->den + 8)) < 0) {     /* -8 <= den <= 7 */
        s->stage = 2;
        return;
    }
    s->stage = 3;
    s->t = xn_s64_div(&s->d0, s->den);
    s->hit.y = mul_round16(d.y, s->t) + p0->y;
    if (vertical) {
        s->hit.x = p0->x;
        s->hit.z = p0->z;
    } else {
        s->hit.z = mul_round16(d.z, s->t) + p0->z;
        s->hit.x = mul_round16(d.x, s->t) + p0->x;
    }
}

static int is_vertical(const xn_vec3 *p0, const xn_vec3 *p1)
{
    return p1->x == p0->x && p1->z == p0->z;
}

s32 xn_collide_segment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                             const xn_vec3 *p1, xn_vec3 *hit)
{
    struct seg_plane s;

    segment_plane(n, c, p0, p1, is_vertical(p0, p1), &s);
    if (s.stage == 3)
        *hit = s.hit;
    return s.t;
}

s32 xn_collide_vsegment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                              const xn_vec3 *p1, xn_vec3 *hit)
{
    struct seg_plane s;

    segment_plane(n, c, p0, p1, 1, &s);
    if (s.stage == 3)
        *hit = s.hit;
    return s.t;
}

/* The asm interface of both: n in EAX EDX EBX, c in ECX, p0 in ESI, p1 in EDI; t to ECX, the
   point to EAX EDX EBX. Without a crossing those three hold what the asm leaves: the low and
   high dwords of d0 and the low dword of d1; nearly parallel: d0's low dword, den's sign test
   and den's partial sum (vertical: d1's low dword). */
static void segment_plane_regs(xn_regs *r, int vertical)
{
    struct seg_plane s;
    xn_vec3 n;

    regs_vec(&n, r->eax, r->edx, r->ebx);
    segment_plane(&n, (const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi,
                  (const xn_vec3 *)r->edi, vertical, &s);
    r->ecx = s.t;
    switch (s.stage) {
    case 1:
        r->eax = s.d0.lo;
        r->edx = s.d0.hi;
        r->ebx = s.d1.lo;
        break;
    case 2:
        r->eax = s.d0.lo;
        r->edx = (s.den + 8) ^ (s.den - 8);
        r->ebx = vertical ? s.d1.lo : s.den_xy;
        break;
    default:
        r->eax = s.hit.x;
        r->edx = s.hit.y;
        r->ebx = s.hit.z;
    }
}

void xn_collide_segment_plane_r(xn_regs *r)
{
    segment_plane_regs(r, is_vertical((const xn_vec3 *)r->esi, (const xn_vec3 *)r->edi));
}

void xn_collide_vsegment_plane_r(xn_regs *r)
{
    segment_plane_regs(r, 1);
}

s32 xn_collide_line_plane_point(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                                const xn_vec3 *p1, xn_vec3 *hit)
{
    xn_vec3 d;
    xn_s64 num, den;
    s32 t;

    xn_collide_normal = *n;
    vec_sub(&d, p1, p0);
    xn_s64_mul(&den, n->x, d.x);
    xn_s64_mac(&den, n->y, d.y);
    xn_s64_mac(&den, n->z, d.z);
    dot_diff(&num, n, c, p0);
    t = xn_s64_div(&num, round16(&den));
    hit->y = mul_round16(d.y, t) + p0->y;
    hit->z = mul_round16(d.z, t) + p0->z;
    hit->x = mul_round16(d.x, t) + p0->x;
    return t;
}

s32 xn_collide_vline_plane_point(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                                 const xn_vec3 *p1, xn_vec3 *hit)
{
    xn_s64 num;
    s32 dy = p1->y - p0->y, t;

    xn_collide_normal = *n;
    dot_diff(&num, n, c, p0);
    t = xn_s64_div(&num, mul_round16(n->y, dy));
    hit->x = p0->x;
    hit->y = mul_round16(dy, t) + p0->y;
    hit->z = p0->z;
    return t;
}

/* the asm interface of the line tests: as segment_plane_regs, t and the point always */
static void line_regs(xn_regs *r, s32 (*f)(const xn_vec3 *, const xn_vec3 *, const xn_vec3 *,
                                          const xn_vec3 *, xn_vec3 *))
{
    xn_vec3 n, hit;

    regs_vec(&n, r->eax, r->edx, r->ebx);
    r->ecx = f(&n, (const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi, (const xn_vec3 *)r->edi,
               &hit);
    r->eax = hit.x;
    r->edx = hit.y;
    r->ebx = hit.z;
}

void xn_collide_line_plane_point_r(xn_regs *r)
{
    line_regs(r, xn_collide_line_plane_point);
}

void xn_collide_vline_plane_point_r(xn_regs *r)
{
    line_regs(r, xn_collide_vline_plane_point);
}

s32 xn_collide_plane_distance(s32 nx, s32 ny, s32 nz, const xn_vec3 *c, const xn_vec3 *s)
{
    xn_s64 d;

    xn_collide_normal.x = nx;
    xn_collide_normal.y = ny;
    xn_collide_normal.z = nz;
    xn_s64_mul(&d, nx, s->x - c->x);
    xn_s64_mac(&d, ny, s->y - c->y);
    xn_s64_mac(&d, nz, s->z - c->y);    /* (c's y: a bug, kept) */
    return round16(&d);
}

/* (a * d - b * c + 8000h) >> 16 */
static s32 det2_round16(s32 a, s32 b, s32 c, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, d);
    xn_s64_msub(&t, b, c);
    return round16(&t);
}

/* (a . b + 80h) >> 8, 64-bit */
static s32 dot_round8(const xn_vec3 *a, const xn_vec3 *b)
{
    xn_s64 t;

    xn_s64_mul(&t, a->x, b->x);
    xn_s64_mac(&t, a->y, b->y);
    xn_s64_mac(&t, a->z, b->z);
    xn_s64_addu(&t, 0x80);
    return xn_s64_shr(&t, 8);
}

/* whether v is "small" for the plane-plane test: -1600h <= v < 1600h */
static int small_component(s32 v)
{
    return ((v - 0x1600) ^ (v + 0x1600)) < 0;
}

/* the 2 x 2 system's solution's component: (u * d1 + v * d2) / det */
static s32 solve2(s32 u, s32 d1, s32 v, s32 d2, s32 det)
{
    xn_s64 t;

    xn_s64_mul(&t, u, d1);
    xn_s64_mac(&t, v, d2);
    return xn_s64_div(&t, det);
}

/* What 14C4A6 leaves: the point, or on a parallel exit the registers it has then */
struct plane_line {
    int found;
    xn_vec3 point;
    u32 eax, edx, ebx;      /* the parallel exits' leftovers */
};

static void plane_plane_line(const xn_vec3 *n1, const xn_vec3 *p1, const xn_vec3 *n2,
                             const xn_vec3 *p2, struct plane_line *pl)
{
    xn_vec3 *dir = &xn_collide_normal;
    s32 ax, ay, az, a, b, c, e, det, u, v;

    xn_collide_ppl_n1 = n1;
    xn_collide_ppl_p1 = p1;
    xn_collide_ppl_n2 = n2;
    xn_collide_ppl_p2 = p2;
    /* the line's direction n1 x n2 */
    dir->x = det2_round16(n1->y, n1->z, n2->y, n2->z);
    dir->y = det2_round16(n1->z, n1->x, n2->z, n2->x);
    dir->z = det2_round16(n1->x, n1->y, n2->x, n2->y);
    pl->found = 0;
    if (small_component(dir->z) && small_component(dir->x) && small_component(dir->y)) {
        pl->eax = (dir->y - 0x1600) ^ (dir->y + 0x1600);
        pl->edx = dir->y + 0x1600;
        pl->ebx = dir->z;
        return;
    }
    xn_collide_mid.x = dot_round8(n1, p1);      /* the planes' constants d1, d2 */
    xn_collide_mid.y = dot_round8(n2, p2);
    /* solve on the two axes other than the direction's largest */
    ax = magnitude(dir->x);
    ay = magnitude(dir->y);
    az = magnitude(dir->z);
    if (ax >= ay && ax >= az) {
        xn_collide_ppl_axis = 1;
        a = n1->y, b = n1->z, c = n2->y, e = n2->z;
    } else if (ax < ay && ay >= az) {
        xn_collide_ppl_axis = 2;
        a = n1->x, b = n1->z, c = n2->x, e = n2->z;
    } else {
        xn_collide_ppl_axis = 3;
        a = n1->x, b = n1->y, c = n2->x, e = n2->y;
    }
    xn_collide_vec_a.y = a;
    xn_collide_vec_a.z = b;
    xn_collide_vec_b.y = c;
    xn_collide_vec_b.z = e;
    det = det2_round16(a, b, c, e);
    if (det == 0) {
        xn_s64 bc;

        xn_s64_mul(&bc, b, c);
        pl->eax = bc.lo;
        pl->edx = bc.hi;
        pl->ebx = c;
        return;
    }
    u = solve2(-c, xn_collide_mid.x, a, xn_collide_mid.y, det);  /* the second axis' */
    v = solve2(-xn_collide_mid.y, b, xn_collide_mid.x, e, det);  /* the first's */
    pl->found = 1;
    if (xn_collide_ppl_axis == 1) {
        pl->point.x = 0, pl->point.y = v, pl->point.z = u;
    } else if (xn_collide_ppl_axis == 2) {
        pl->point.x = v, pl->point.y = 0, pl->point.z = u;
    } else {
        pl->point.x = v, pl->point.y = u, pl->point.z = 0;
    }
}

int xn_collide_plane_plane_line(const xn_vec3 *n1, const xn_vec3 *p1, const xn_vec3 *n2,
                                const xn_vec3 *p2, xn_vec3 *point)
{
    struct plane_line pl;

    plane_plane_line(n1, p1, n2, p2, &pl);
    if (pl.found)
        *point = pl.point;
    return pl.found;
}

void xn_collide_plane_plane_line_r(xn_regs *r)
{
    struct plane_line pl;

    plane_plane_line((const xn_vec3 *)r->eax, (const xn_vec3 *)r->edx, (const xn_vec3 *)r->ebx,
                     (const xn_vec3 *)r->ecx, &pl);
    if (pl.found) {
        r->eax = pl.point.x;
        r->edx = pl.point.y;
        r->ebx = pl.point.z;
    } else {
        r->eax = pl.eax;
        r->edx = pl.edx;
        r->ebx = pl.ebx;
    }
}

/* ---- spheres against planes -------------------------------------------------------------- */

s32 xn_collide_sphere_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                            s32 *dist)
{
    xn_s64 d;

    xn_collide_normal.y = n->y;
    xn_collide_normal.z = n->z;
    dot_diff(&d, n, s, c);
    *dist = round16(&d);
    return r - magnitude(*dist);
}

/* the asm interface: n in EAX EDX EBX, c in ECX, s in ESI, r in EDI; the result in EAX, the
   distance in EDX, and in EBX the low dword of the x and y products' sum */
void xn_collide_sphere_plane_r(xn_regs *r)
{
    xn_vec3 n;
    const xn_vec3 *c = (const xn_vec3 *)r->ecx, *s = (const xn_vec3 *)r->esi;
    s32 dist;

    regs_vec(&n, r->eax, r->edx, r->ebx);
    r->eax = xn_collide_sphere_plane(&n, c, s, r->edi, &dist);
    r->edx = dist;
    r->ebx = n.x * (s->x - c->x) + n.y * (s->y - c->y);
}

s32 xn_collide_sphere_plane_xz(s32 nx, s32 nz, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                               s32 *dist)
{
    xn_s64 d;

    xn_collide_normal.z = nz;
    xn_s64_mul(&d, nx, s->x - c->x);
    xn_s64_mac(&d, nz, s->z - c->z);
    *dist = round16(&d);
    return r - magnitude(*dist);
}

void xn_collide_sphere_plane_xz_r(xn_regs *r)
{
    s32 dist;

    r->eax = xn_collide_sphere_plane_xz(r->eax, r->ebx, (const xn_vec3 *)r->ecx,
                                        (const xn_vec3 *)r->esi, r->edi, &dist);
    r->edx = dist;
}

s32 xn_collide_sphere_plane_yz(s32 ny, s32 nz, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                               s32 *dist)
{
    xn_s64 d;

    xn_collide_normal.z = nz;
    xn_s64_mul(&d, ny, s->y - c->y);
    xn_s64_mac(&d, nz, s->z - c->z);
    *dist = round16(&d);
    return r - magnitude(*dist);
}

void xn_collide_sphere_plane_yz_r(xn_regs *r)
{
    s32 dist;

    r->eax = xn_collide_sphere_plane_yz(r->edx, r->ebx, (const xn_vec3 *)r->ecx,
                                        (const xn_vec3 *)r->esi, r->edi, &dist);
    r->edx = dist;
}

s32 xn_collide_sphere_plane_z(const xn_vec3 *c, const xn_vec3 *s, s32 r)
{
    return r - (c->z - s->z);
}

/* ---- segments and points against spheres -------------------------------------------------- */

s32 xn_collide_point_in_sphere(s32 cx, s32 cy, s32 cz, s32 r, const xn_vec3 *p)
{
    xn_s64 t;

    xn_collide_normal.y = cy;
    xn_collide_normal.z = cz;
    xn_s64_mul(&t, r, r);
    xn_s64_msub(&t, p->x - cx, p->x - cx);
    xn_s64_msub(&t, p->y - cy, p->y - cy);
    xn_s64_msub(&t, p->z - cz, p->z - cz);
    return t.hi;
}

static s32 point_in(const xn_vec3 *c, s32 r, const xn_vec3 *p)
{
    return xn_collide_point_in_sphere(c->x, c->y, c->z, r, p);
}

s32 xn_collide_segment_sphere_fx(const xn_vec3 *c, s32 r, const xn_vec3 *p0, const xn_vec3 *p1)
{
    xn_s64 t;
    s32 a2, len2, proj;

    if (point_in(c, r, p0) >= 0 || point_in(c, r, p1) >= 0)
        return 1;
    /* the nearest point of the segment, from p1 along b = p0 - p1 */
    vec_sub(&xn_collide_vec_a, c, p1);
    length2(&t, &xn_collide_vec_a);
    a2 = round16(&t);
    vec_sub(&xn_collide_vec_b, p0, p1);
    length2(&t, &xn_collide_vec_b);
    len2 = round16(&t);
    if (len2 == 0)
        return -1;
    xn_s64_mul(&t, xn_collide_vec_a.x, xn_collide_vec_b.x);
    xn_s64_mac(&t, xn_collide_vec_a.y, xn_collide_vec_b.y);
    xn_s64_mac(&t, xn_collide_vec_a.z, xn_collide_vec_b.z);
    proj = round16(&t);
    if (proj < 0 || proj > len2)
        return -1;
    xn_s64_mul(&t, proj, proj);
    return mul_round16(r, r) - (a2 - xn_s64_div(&t, len2));
}

/* What the asm leaves in ESI (an output of its row; the face test passes it on): p0 when p0 is
   inside, p1 when p1 is or the segment has no length, else |b|^2 */
static u32 fx_esi(const xn_vec3 *c, s32 r, const xn_vec3 *p0, const xn_vec3 *p1)
{
    xn_s64 t;
    s32 len2;

    if (point_in(c, r, p0) >= 0)
        return (u32)p0;
    if (point_in(c, r, p1) >= 0)
        return (u32)p1;
    length2(&t, &xn_collide_vec_b);
    len2 = round16(&t);
    return len2 == 0 ? (u32)p1 : (u32)len2;
}

void xn_collide_segment_sphere_fx_r(xn_regs *r)
{
    xn_vec3 c;
    const xn_vec3 *p0 = (const xn_vec3 *)r->esi, *p1 = (const xn_vec3 *)r->edi;

    regs_vec(&c, r->eax, r->edx, r->ebx);
    r->eax = xn_collide_segment_sphere_fx(&c, r->ecx, p0, p1);
    r->esi = fx_esi(&c, r->ecx, p0, p1);
}

/* |v|^2 with 32-bit products */
static s32 length2_32(const xn_vec3 *v)
{
    return v->x * v->x + v->y * v->y + v->z * v->z;
}

static s32 dot_32(const xn_vec3 *a, const xn_vec3 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

s32 xn_collide_segment_sphere(const xn_vec3 *c, s32 r, const xn_vec3 *p0, const xn_vec3 *p1)
{
    xn_s64 t;
    s32 len2, proj;

    if (point_in(c, r, p0) >= 0 || point_in(c, r, p1) >= 0)
        return 1;
    vec_sub(&xn_collide_vec_a, c, p1);
    vec_sub(&xn_collide_vec_b, p0, p1);
    len2 = length2_32(&xn_collide_vec_b);
    if (len2 == 0)
        return -1;
    proj = dot_32(&xn_collide_vec_a, &xn_collide_vec_b);
    if (proj < 0 || proj > len2)
        return -1;
    xn_s64_mul(&t, proj, proj);
    return r * r - (length2_32(&xn_collide_vec_a) - xn_s64_div(&t, len2));
}

/* the asm interface: c in EAX EDX EBX, r in ECX, p0 in ESI, p1 in EDI; the result in EAX, and
   in EDX and EBX what the asm leaves there (its row's outputs): c's y and z when an end is
   inside; else |a|^2 in EBX and in EDX the last product (b.z^2 for a segment without length,
   a.z * b.z when the nearest point is off the segment) or the divide's remainder */
void xn_collide_segment_sphere_r(xn_regs *r)
{
    xn_vec3 c;
    const xn_vec3 *a = &xn_collide_vec_a, *b = &xn_collide_vec_b;
    s32 len2, proj, rem;
    xn_s64 t;

    regs_vec(&c, r->eax, r->edx, r->ebx);
    r->eax = xn_collide_segment_sphere(&c, r->ecx, (const xn_vec3 *)r->esi,
                                       (const xn_vec3 *)r->edi);
    if (point_in(&c, r->ecx, (const xn_vec3 *)r->esi) >= 0 ||
        point_in(&c, r->ecx, (const xn_vec3 *)r->edi) >= 0)
        return;                         /* an end inside: EDX, EBX as they came */
    r->ebx = length2_32(a);
    len2 = length2_32(b);
    if (len2 == 0) {
        r->edx = b->z * b->z;
        return;
    }
    proj = dot_32(a, b);
    if (proj < 0 || proj > len2) {
        r->edx = a->z * b->z;
        return;
    }
    xn_s64_mul(&t, proj, proj);
    r->ebx -= xn_s64_divrem(&t, len2, &rem);
    r->edx = rem;
}

s32 xn_collide_segment_sphere_approx(s32 cx, s32 cy, s32 cz, s32 r, const xn_vec3 *p0,
                                     const xn_vec3 *p1)
{
    xn_vec3 *mid = &xn_collide_mid;
    s32 v;

    xn_collide_normal.x = cx;           /* (the centre, kept there) */
    xn_collide_normal.y = cy;
    xn_collide_normal.z = cz;
    v = xn_collide_point_in_sphere_approx(&xn_collide_normal, r, p0);
    if (v >= 0)
        return v;
    v = xn_collide_point_in_sphere_approx(&xn_collide_normal, r, p1);
    if (v >= 0)
        return v;
    mid->x = (p0->x + p1->x) >> 1;
    mid->y = (p0->y + p1->y) >> 1;
    mid->z = (p0->z + p1->z) >> 1;
    return xn_collide_point_in_sphere_approx(&xn_collide_normal, r, mid);
}

s32 xn_collide_sphere_sphere(const xn_vec3 *c1, s32 r1, const xn_vec3 *c2, s32 r2)
{
    xn_s64 t;

    xn_collide_normal.y = c1->y;
    xn_collide_normal.z = c1->z;
    xn_s64_mul(&t, r1 + r2, r1 + r2);
    xn_s64_msub(&t, c1->x - c2->x, c1->x - c2->x);
    xn_s64_msub(&t, c1->y - c2->y, c1->y - c2->y);
    xn_s64_msub(&t, c1->z - c2->z, c1->z - c2->z);
    return t.hi;
}

/* the asm interface: c1 in EAX EDX EBX, r1 in ECX, c2 in ESI, r2 in EDI; the result in EAX
   and EDX, and in EBX the low dword of d^2 */
void xn_collide_sphere_sphere_r(xn_regs *r)
{
    xn_vec3 c1, d;

    regs_vec(&c1, r->eax, r->edx, r->ebx);
    r->eax = r->edx = xn_collide_sphere_sphere(&c1, r->ecx, (const xn_vec3 *)r->esi, r->edi);
    vec_sub(&d, &c1, (const xn_vec3 *)r->esi);
    r->ebx = length2_32(&d);
}

s32 xn_collide_sphere_sphere_approx(s32 cx, s32 cy, s32 cz, s32 r1, const xn_vec3 *c2, s32 r2)
{
    return r1 + r2 - xn_vec_length_approx(cx - c2->x, cy - c2->y, cz - c2->z);
}

s32 xn_collide_point_in_sphere_approx(const xn_vec3 *c, s32 r, const xn_vec3 *p)
{
    return r - xn_vec_length_approx(c->x - p->x, c->y - p->y, c->z - p->z);
}

/* the asm interface: EDX and EBX are the approximate length's y and z terms */
void xn_collide_point_in_sphere_approx_r(xn_regs *r)
{
    xn_vec3 c, terms;
    const xn_vec3 *p = (const xn_vec3 *)r->esi;

    regs_vec(&c, r->eax, r->edx, r->ebx);
    xn_vec_length_approx_terms(c.x - p->x, c.y - p->y, c.z - p->z, &terms);
    r->eax = r->ecx - (terms.x + terms.y + terms.z);
    r->edx = terms.y;
    r->ebx = terms.z;
}

s32 xn_collide_point_in_cylinder(s32 x, s32 y, s32 z, const xn_vec3 *c, s32 d, s32 h)
{
    s32 half = h >> 1, below, rad;
    xn_s64 t;

    below = y - c->y - half;
    if ((below ^ (below + 2 * half)) >= 0)
        return -1;                      /* not between the ends */
    rad = d >> 1;
    xn_s64_mul(&t, rad, rad);
    xn_s64_msub(&t, x - c->x, x - c->x);
    xn_s64_msub(&t, z - c->z, z - c->z);
    return t.hi;
}

/* the sign-trick inside test of one axis: d and d + e of different signs */
static s32 box_axis(s32 d, s32 e)
{
    return d ^ (d + e);
}

static s32 box_test(s32 sx, s32 sy, s32 sz)
{
    return -(-((sx ^ sy) | (sy ^ sz)) & sz);
}

s32 xn_collide_point_in_box(s32 x, s32 y, s32 z, const xn_vec3 *e, const xn_vec3 *b)
{
    return box_test(box_axis(x - b->x, e->x), box_axis(y - b->y, e->y), box_axis(z - b->z, e->z));
}

s32 xn_collide_point_in_cube(s32 x, s32 y, s32 z, s32 e, const xn_vec3 *b)
{
    return box_test(box_axis(x - b->x, e), box_axis(y - b->y, e), box_axis(z - b->z, e));
}

/* ---- faces -------------------------------------------------------------------------------- */

/* the model point of a face's point slot (a byte offset into its points, 8 bytes each) */
static const xn_vec3 *slot_point(const struct xn_model_face *face, s32 slot,
                                 const xn_vec3 *points)
{
    return (const xn_vec3 *)((const u8 *)points + *(const s32 *)((const u8 *)face + slot + 8));
}

/* the face's k-th point */
static const xn_vec3 *face_point(const struct xn_model_face *face, u32 k, const xn_vec3 *points)
{
    return slot_point(face, k * 8, points);
}

/* (a * b - c * d) >> 16 of the 64-bit difference (no rounding) */
static s32 det2_shr16(s32 a, s32 b, s32 c, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    xn_s64_msub(&t, c, d);
    return xn_s64_shr(&t, 16);
}

/* the face test's last edge, for the asm interface */
struct face_test {
    u32 edge;           /* the edge it stopped at (the count when inside) */
    s32 cy, cz;         /* that edge's cross product's y and z */
};

static s32 point_in_face(const xn_vec3 *p, const struct xn_model_face *face,
                         const xn_vec3 *points, const xn_vec3 *normal, struct face_test *ft)
{
    const s32 *slots = xn_face_edge_tables[face->point_count];
    xn_vec3 *v0 = &xn_collide_edge_v0, *v1 = &xn_collide_edge_v1;
    const xn_vec3 *a, *b;
    s32 cx;
    xn_s64 dot;
    u32 k = 0;

    xn_collide_test_point = *p;
    xn_collide_face_normal_ptr = normal;
    xn_collide_face_points = points;
    do {
        a = slot_point(face, slots[k], points);
        b = slot_point(face, slots[k + 1], points);
        vec_sub(v0, a, p);
        vec_sub(v1, b, p);
        cx = det2_shr16(v0->y, v1->z, v0->z, v1->y);
        ft->cy = det2_shr16(v1->x, v0->z, v0->x, v1->z);
        ft->cz = det2_shr16(v0->x, v1->y, v0->y, v1->x);
        if ((cx | ft->cy | ft->cz) != 0) {
            xn_s64_mul(&dot, cx, normal->x);
            xn_s64_mac(&dot, ft->cy, normal->y);
            xn_s64_mac(&dot, ft->cz, normal->z);
            if (dot.hi < 0) {
                ft->edge = k;
                return dot.hi;          /* outside this edge */
            }
        }
        k++;
    } while ((s8)k < (s8)face->point_count);
    ft->edge = k;
    return 1;
}

s32 xn_collide_point_in_face(const xn_vec3 *p, const struct xn_model_face *face,
                             const xn_vec3 *points, const xn_vec3 *normal)
{
    struct face_test ft;

    return point_in_face(p, face, points, normal, &ft);
}

/* the asm interface: p in EAX EDX EBX, the face in ECX, the points in ESI, the normal in EDI;
   the asm leaves the edge it stopped at in EDX, the face's edge table in EBX and the edge's
   cross product's y and z in ESI and EDI */
void xn_collide_point_in_face_r(xn_regs *r)
{
    const struct xn_model_face *face = (const struct xn_model_face *)r->ecx;
    struct face_test ft;
    xn_vec3 p;

    regs_vec(&p, r->eax, r->edx, r->ebx);
    r->eax = point_in_face(&p, face, (const xn_vec3 *)r->esi, (const xn_vec3 *)r->edi, &ft);
    r->edx = ft.edge;
    r->ebx = (u32)xn_face_edge_tables[face->point_count];
    r->esi = ft.cy;
    r->edi = ft.cz;
}

int xn_collide_point_in_face_v2(const xn_vec3 *p, const struct xn_model_face *face,
                                const xn_vec3 *points, const xn_vec3 *normal)
{
    const s32 *slots = xn_face_edge_tables[face->point_count];
    xn_vec3 *v0 = &xn_collide_edge_v0, *v1 = &xn_collide_edge_v1;
    const xn_vec3 *a, *b;
    s32 cx, cy, cz;
    u32 left = face->point_count;

    xn_scratch_vecs.flat = *p;
    do {
        a = slot_point(face, slots[0], points);
        b = slot_point(face, slots[1], points);
        v0->y = (a->y - p->y) << 8;
        v1->z = (b->z - p->z) << 8;
        cx = xn_mulhi(v0->y, v1->z);
        v0->z = (a->z - p->z) << 8;
        v1->y = (b->y - p->y) << 8;
        cx -= xn_mulhi(v0->z, v1->y);
        v1->x = (b->x - p->x) << 8;
        cy = xn_mulhi(v1->x, v0->z);
        v0->x = (a->x - p->x) << 8;
        cy -= xn_mulhi(v0->x, v1->z);
        cz = xn_mulhi(v0->x, v1->y) - xn_mulhi(v0->y, v1->x);
        if ((cx | cy | cz) != 0 &&
            (cx * normal->x + cz * normal->z + cy * normal->y) >> 8 <= 0)
            return 0;                   /* outside this edge */
        slots++;
    } while (--left != 0);
    return 1;
}

void xn_collide_point_in_face_v2_r(xn_regs *r)
{
    xn_vec3 p;

    regs_vec(&p, r->eax, r->edx, r->ebx);
    XN_SETFLAG(r, XN_CF, !xn_collide_point_in_face_v2(&p, (const struct xn_model_face *)r->ecx,
                                                       (const xn_vec3 *)r->esi,
                                                       (const xn_vec3 *)r->edi));
}

s32 xn_collide_face_area(const struct xn_model_face *face, const xn_vec3 *points)
{
    const s32 *slots = xn_face_edge_tables[face->point_count];
    xn_vec3 *v0 = &xn_collide_edge_v0, *v1 = &xn_collide_edge_v1;
    const xn_vec3 *a, *b;
    s32 k, sum = 0;

    xn_collide_area_points = points;
    xn_collide_test_point = *face_point(face, 0, points);
    xn_collide_area_last = face->point_count - 2;
    k = 1;
    do {
        /* the fan's triangle (point 0, point k, point k + 1) */
        a = slot_point(face, slots[k], points);
        b = slot_point(face, slots[k + 1], points);
        vec_sub(v0, a, &xn_collide_test_point);
        vec_sub(v1, b, a);
        sum += xn_vec_length(det2_round16(v0->y, v0->z, v1->y, v1->z),
                             det2_round16(v1->x, v0->x, v1->z, v0->z),
                             det2_round16(v0->x, v0->y, v1->x, v1->y));
    } while (++k <= xn_collide_area_last);
    return sum >> 1;
}

/* A face's edge k as the edge test walks them: its point and the next (the first after the
   last) */
static void face_edge(const struct xn_model_face *face, u32 k, u32 left, const xn_vec3 *points,
                      const xn_vec3 **p0, const xn_vec3 **p1)
{
    *p0 = face_point(face, k, points);
    *p1 = face_point(face, left == 1 ? 0 : k + 1, points);
}

/* returns the result and the edge it stopped at */
static s32 face_test_edges(const xn_vec3 *c, const struct xn_model_face *face,
                           const xn_vec3 *points, s32 r, u32 *edge)
{
    const xn_vec3 *p0, *p1;
    u32 left = face->point_count, k = 0;

    xn_collide_edge_points = points;
    xn_collide_edge_first = (struct xn_model_face_point *)face->points;
    do {
        face_edge(face, k, left, points, &p0, &p1);
        if (xn_collide_segment_sphere_fx(c, r, p0, p1) >= 0) {
            *edge = k;
            return 1;
        }
        k++;
    } while (--left != 0);
    *edge = k;
    return -1;
}

s32 xn_collide_face_test_edges(const xn_vec3 *c, const struct xn_model_face *face,
                               const xn_vec3 *points, s32 r)
{
    u32 edge;

    return face_test_edges(c, face, points, r, &edge);
}

/* the asm interface: the sphere's centre in EAX EDX EBX, the face in ECX, the points in ESI,
   the radius in EDI; it leaves the radius in ECX, the edge test's ESI and the slot it stopped
   at in EDI */
void xn_collide_face_test_edges_r(xn_regs *r)
{
    const struct xn_model_face *face = (const struct xn_model_face *)r->ecx;
    const xn_vec3 *points = (const xn_vec3 *)r->esi, *p0, *p1;
    s32 radius = r->edi;
    u32 edge, last;
    xn_vec3 c;

    regs_vec(&c, r->eax, r->edx, r->ebx);
    r->eax = face_test_edges(&c, face, points, radius, &edge);
    last = (s32)r->eax > 0 ? edge : edge - 1;   /* the last edge tested */
    face_edge(face, last, face->point_count - last, points, &p0, &p1);
    r->ecx = radius;
    r->esi = fx_esi(&c, radius, p0, p1);
    r->edi = (u32)&face->points[edge];
}

s32 xn_collide_face_test_vertices(s32 cx, s32 cy, s32 cz, const struct xn_model_face *face,
                                  const xn_vec3 *points, s32 r)
{
    u32 left = face->point_count, k = 0;
    s32 v;

    xn_collide_vertex_points = points;
    do {
        v = xn_collide_point_in_sphere(cx, cy, cz, r, face_point(face, k, points));
        if (v >= 0)
            return cx;      /* the asm restores the centre's x over the result: a bug, kept */
        k++;
    } while (--left != 0);
    return -1;
}

/* ---- the reference stub ----------------------------------------------------------------- */

void xn_collide_ref_helpers_r(xn_regs *r)
{
    xn_collide_sphere_plane_r(r);
    xn_collide_point_in_face_r(r);
    xn_collide_face_test_edges_r(r);
}
