/* collide.c: XnGine's geometric collision tests (canonical C; the interface and the module's
   documentation are in xcollide.h): planes, spheres, points and faces. Most of them sum 64-bit
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

/* *s = a . b, 64-bit */
static void dot64(xn_s64 *s, const xn_vec3 *a, const xn_vec3 *b)
{
    xn_s64_mul(s, a->x, b->x);
    xn_s64_mac(s, a->y, b->y);
    xn_s64_mac(s, a->z, b->z);
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

/* ---- planes ----------------------------------------------------------------------------- */

static int is_vertical(const xn_vec3 *p0, const xn_vec3 *p1)
{
    return p1->x == p0->x && p1->z == p0->z;
}

/* A segment against a plane; vertical: the y terms of the direction only */
static s32 segment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                         const xn_vec3 *p1, int vertical, xn_vec3 *hit)
{
    xn_vec3 d;
    xn_s64 d0, d1, t64;
    s32 den, t;

    dot_diff(&d0, n, c, p0);                    /* the ends' sides */
    dot_diff(&d1, n, c, p1);
    if ((d0.hi ^ d1.hi) >= 0)
        return -1;                              /* the same side */
    vec_sub(&d, p1, p0);
    if (vertical) {
        den = mul_round16(n->y, d.y);
    } else {
        dot64(&t64, n, &d);
        den = round16(&t64);
    }
    if ((s32)(((u32)den - 8) ^ ((u32)den + 8)) < 0)
        return -1;                              /* nearly parallel: -8 <= den <= 7 (the
                                                   asm's sign test, which wraps) */
    t = xn_s64_div_or0(&d0, den);
    hit->y = mul_round16(d.y, t) + p0->y;
    if (vertical) {
        hit->x = p0->x;
        hit->z = p0->z;
    } else {
        hit->z = mul_round16(d.z, t) + p0->z;
        hit->x = mul_round16(d.x, t) + p0->x;
    }
    return t;
}

s32 xn_collide_segment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                             const xn_vec3 *p1, xn_vec3 *hit)
{
    return segment_plane(n, c, p0, p1, is_vertical(p0, p1), hit);
}

s32 xn_collide_vsegment_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                              const xn_vec3 *p1, xn_vec3 *hit)
{
    return segment_plane(n, c, p0, p1, 1, hit);
}

s32 xn_collide_line_plane_point(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *p0,
                                const xn_vec3 *p1, xn_vec3 *hit)
{
    xn_vec3 d;
    xn_s64 num, den;
    s32 t;

    vec_sub(&d, p1, p0);
    dot64(&den, n, &d);
    dot_diff(&num, n, c, p0);
    t = xn_s64_div_or0(&num, round16(&den));
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

    dot_diff(&num, n, c, p0);
    t = xn_s64_div_or0(&num, mul_round16(n->y, dy));
    hit->x = p0->x;
    hit->y = mul_round16(dy, t) + p0->y;
    hit->z = p0->z;
    return t;
}

s32 xn_collide_plane_distance(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s)
{
    xn_s64 d;

    xn_s64_mul(&d, n->x, s->x - c->x);
    xn_s64_mac(&d, n->y, s->y - c->y);
    xn_s64_mac(&d, n->z, s->z - c->y);  /* Quirk Q-COLL-01: c's y */
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

    dot64(&t, a, b);
    xn_s64_addu(&t, 0x80);
    return xn_s64_shr(&t, 8);
}

/* "small" for the plane-plane test: -1600h <= v < 1600h (the asm's sign test, which wraps) */
static int small_component(s32 v)
{
    return (s32)(((u32)v - 0x1600) ^ ((u32)v + 0x1600)) < 0;
}

/* a component of the 2 x 2 system's solution: (u * d1 + v * d2) / det */
static s32 solve2(s32 u, s32 d1, s32 v, s32 d2, s32 det)
{
    xn_s64 t;

    xn_s64_mul(&t, u, d1);
    xn_s64_mac(&t, v, d2);
    return xn_s64_div_or0(&t, det);
}

int xn_collide_plane_plane_line(const xn_vec3 *n1, const xn_vec3 *p1, const xn_vec3 *n2,
                                const xn_vec3 *p2, xn_vec3 *point)
{
    xn_vec3 dir;
    s32 ax, ay, az, a, b, c, e, det, d1, d2, u, v;
    int axis;

    /* the line's direction n1 x n2 */
    dir.x = det2_round16(n1->y, n1->z, n2->y, n2->z);
    dir.y = det2_round16(n1->z, n1->x, n2->z, n2->x);
    dir.z = det2_round16(n1->x, n1->y, n2->x, n2->y);
    if (small_component(dir.z) && small_component(dir.x) && small_component(dir.y))
        return 0;
    d1 = dot_round8(n1, p1);                    /* the planes' constants */
    d2 = dot_round8(n2, p2);
    /* solve on the two axes other than the direction's largest */
    ax = magnitude(dir.x);
    ay = magnitude(dir.y);
    az = magnitude(dir.z);
    if (ax >= ay && ax >= az) {
        axis = 0;
        a = n1->y, b = n1->z, c = n2->y, e = n2->z;
    } else if (ax < ay && ay >= az) {
        axis = 1;
        a = n1->x, b = n1->z, c = n2->x, e = n2->z;
    } else {
        axis = 2;
        a = n1->x, b = n1->y, c = n2->x, e = n2->y;
    }
    det = det2_round16(a, b, c, e);
    if (det == 0)
        return 0;
    u = solve2(-c, d1, a, d2, det);             /* the second axis' */
    v = solve2(-d2, b, d1, e, det);             /* the first's */
    if (axis == 0) {
        point->x = 0, point->y = v, point->z = u;
    } else if (axis == 1) {
        point->x = v, point->y = 0, point->z = u;
    } else {
        point->x = v, point->y = u, point->z = 0;
    }
    return 1;
}

/* ---- spheres against planes -------------------------------------------------------------- */

s32 xn_collide_sphere_plane(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                            s32 *dist)
{
    xn_s64 d;

    dot_diff(&d, n, s, c);
    *dist = round16(&d);
    return r - magnitude(*dist);
}

s32 xn_collide_sphere_plane_xz(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                               s32 *dist)
{
    xn_s64 d;

    xn_s64_mul(&d, n->x, s->x - c->x);
    xn_s64_mac(&d, n->z, s->z - c->z);
    *dist = round16(&d);
    return r - magnitude(*dist);
}

s32 xn_collide_sphere_plane_yz(const xn_vec3 *n, const xn_vec3 *c, const xn_vec3 *s, s32 r,
                               s32 *dist)
{
    xn_s64 d;

    xn_s64_mul(&d, n->y, s->y - c->y);
    xn_s64_mac(&d, n->z, s->z - c->z);
    *dist = round16(&d);
    return r - magnitude(*dist);
}

s32 xn_collide_sphere_plane_z(const xn_vec3 *c, const xn_vec3 *s, s32 r)
{
    return r - (c->z - s->z);
}

/* ---- segments and points against spheres -------------------------------------------------- */

s32 xn_collide_point_in_sphere(const xn_vec3 *c, s32 r, const xn_vec3 *p)
{
    xn_s64 t;

    xn_s64_mul(&t, r, r);
    xn_s64_msub(&t, p->x - c->x, p->x - c->x);
    xn_s64_msub(&t, p->y - c->y, p->y - c->y);
    xn_s64_msub(&t, p->z - c->z, p->z - c->z);
    return t.hi;
}

s32 xn_collide_segment_sphere_fx(const xn_vec3 *c, s32 r, const xn_vec3 *p0, const xn_vec3 *p1)
{
    xn_vec3 a, b;
    xn_s64 t;
    s32 a2, len2, proj;

    if (xn_collide_point_in_sphere(c, r, p0) >= 0 || xn_collide_point_in_sphere(c, r, p1) >= 0)
        return 1;
    /* the nearest point of the segment, from p1 along b = p0 - p1 */
    vec_sub(&a, c, p1);
    dot64(&t, &a, &a);
    a2 = round16(&t);
    vec_sub(&b, p0, p1);
    dot64(&t, &b, &b);
    len2 = round16(&t);
    if (len2 == 0)
        return -1;
    dot64(&t, &a, &b);
    proj = round16(&t);
    if (proj < 0 || proj > len2)
        return -1;
    xn_s64_mul(&t, proj, proj);
    return mul_round16(r, r) - (a2 - xn_s64_div_or0(&t, len2));
}

/* a . b with 32-bit products (Quirk Q-COLL-09: they wrap for vectors longer than about
   32,000 units) */
static s32 dot32(const xn_vec3 *a, const xn_vec3 *b)
{
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

s32 xn_collide_segment_sphere(const xn_vec3 *c, s32 r, const xn_vec3 *p0, const xn_vec3 *p1)
{
    xn_vec3 a, b;
    xn_s64 t;
    s32 len2, proj;

    if (xn_collide_point_in_sphere(c, r, p0) >= 0 || xn_collide_point_in_sphere(c, r, p1) >= 0)
        return 1;
    vec_sub(&a, c, p1);
    vec_sub(&b, p0, p1);
    len2 = dot32(&b, &b);
    if (len2 == 0)
        return -1;
    proj = dot32(&a, &b);
    if (proj < 0 || proj > len2)
        return -1;
    xn_s64_mul(&t, proj, proj);
    return r * r - (dot32(&a, &a) - xn_s64_div_or0(&t, len2));
}

s32 xn_collide_segment_sphere_approx(const xn_vec3 *c, s32 r, const xn_vec3 *p0,
                                     const xn_vec3 *p1)
{
    xn_vec3 mid;
    s32 v;

    v = xn_collide_point_in_sphere_approx(c, r, p0);
    if (v >= 0)
        return v;
    v = xn_collide_point_in_sphere_approx(c, r, p1);
    if (v >= 0)
        return v;
    mid.x = (p0->x + p1->x) >> 1;
    mid.y = (p0->y + p1->y) >> 1;
    mid.z = (p0->z + p1->z) >> 1;
    return xn_collide_point_in_sphere_approx(c, r, &mid);
}

s32 xn_collide_sphere_sphere(const xn_vec3 *c1, s32 r1, const xn_vec3 *c2, s32 r2)
{
    return xn_collide_point_in_sphere(c1, r1 + r2, c2);
}

s32 xn_collide_sphere_sphere_approx(const xn_vec3 *c1, s32 r1, const xn_vec3 *c2, s32 r2)
{
    return r1 + r2 - xn_vec_length_approx(c1->x - c2->x, c1->y - c2->y, c1->z - c2->z);
}

s32 xn_collide_point_in_sphere_approx(const xn_vec3 *c, s32 r, const xn_vec3 *p)
{
    return r - xn_vec_length_approx(c->x - p->x, c->y - p->y, c->z - p->z);
}

s32 xn_collide_point_in_cylinder(const xn_vec3 *p, const xn_vec3 *c, s32 d, s32 h)
{
    s32 half = h >> 1, below, rad;
    xn_s64 t;

    below = p->y - c->y - half;
    if ((below ^ (below + 2 * half)) >= 0)
        return -1;                      /* not between the ends */
    rad = d >> 1;
    xn_s64_mul(&t, rad, rad);
    xn_s64_msub(&t, p->x - c->x, p->x - c->x);
    xn_s64_msub(&t, p->z - c->z, p->z - c->z);
    return t.hi;
}

/* the inside test of one axis by signs: d and d + e have different signs */
static s32 box_axis(s32 d, s32 e)
{
    return d ^ (d + e);
}

/* the three axes' tests together: negative when all three are */
static s32 box_test(s32 sx, s32 sy, s32 sz)
{
    return -(-((sx ^ sy) | (sy ^ sz)) & sz);
}

s32 xn_collide_point_in_box(const xn_vec3 *p, const xn_vec3 *e, const xn_vec3 *b)
{
    return box_test(box_axis(p->x - b->x, e->x), box_axis(p->y - b->y, e->y),
                    box_axis(p->z - b->z, e->z));
}

s32 xn_collide_point_in_cube(const xn_vec3 *p, s32 e, const xn_vec3 *b)
{
    return box_test(box_axis(p->x - b->x, e), box_axis(p->y - b->y, e), box_axis(p->z - b->z, e));
}

/* ---- faces -------------------------------------------------------------------------------- */

/* the model point of a face's point slot (a byte offset into its points, 8 bytes a slot) */
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

s32 xn_collide_point_in_face(const xn_vec3 *p, const struct xn_model_face *face,
                             const xn_vec3 *points, const xn_vec3 *normal)
{
    const s32 *slots = xn_face_edge_tables[face->point_count];
    xn_vec3 v0, v1, cr;
    xn_s64 dot;
    u32 k = 0;

    do {
        vec_sub(&v0, slot_point(face, slots[k], points), p);
        vec_sub(&v1, slot_point(face, slots[k + 1], points), p);
        cr.x = det2_shr16(v0.y, v1.z, v0.z, v1.y);
        cr.y = det2_shr16(v1.x, v0.z, v0.x, v1.z);
        cr.z = det2_shr16(v0.x, v1.y, v0.y, v1.x);
        if ((cr.x | cr.y | cr.z) != 0) {
            dot64(&dot, &cr, normal);
            if (dot.hi < 0)
                return dot.hi;          /* outside this edge */
        }
        k++;
    } while ((s8)k < (s8)face->point_count);
    return 1;
}

int xn_collide_point_in_face_v2(const xn_vec3 *p, const struct xn_model_face *face,
                                const xn_vec3 *points, const xn_vec3 *normal)
{
    const s32 *slots = xn_face_edge_tables[face->point_count];
    const xn_vec3 *a, *b;
    xn_vec3 v0, v1;
    s32 cx, cy, cz;
    u32 left = face->point_count;

    do {
        a = slot_point(face, slots[0], points);
        b = slot_point(face, slots[1], points);
        v0.x = (a->x - p->x) << 8;
        v0.y = (a->y - p->y) << 8;
        v0.z = (a->z - p->z) << 8;
        v1.x = (b->x - p->x) << 8;
        v1.y = (b->y - p->y) << 8;
        v1.z = (b->z - p->z) << 8;
        cx = xn_mulhi(v0.y, v1.z) - xn_mulhi(v0.z, v1.y);
        cy = xn_mulhi(v1.x, v0.z) - xn_mulhi(v0.x, v1.z);
        cz = xn_mulhi(v0.x, v1.y) - xn_mulhi(v0.y, v1.x);
        if ((cx | cy | cz) != 0 &&
            (cx * normal->x + cz * normal->z + cy * normal->y) >> 8 <= 0)
            return 0;                   /* outside this edge */
        slots++;
    } while (--left != 0);
    return 1;
}

s32 xn_collide_face_area(const struct xn_model_face *face, const xn_vec3 *points)
{
    const s32 *slots = xn_face_edge_tables[face->point_count];
    const xn_vec3 *first = face_point(face, 0, points), *a, *b;
    xn_vec3 v0, v1;
    s32 k, sum = 0, last = face->point_count - 2;

    k = 1;
    do {
        /* the fan's triangle (point 0, point k, point k + 1) */
        a = slot_point(face, slots[k], points);
        b = slot_point(face, slots[k + 1], points);
        vec_sub(&v0, a, first);
        vec_sub(&v1, b, a);
        sum += xn_vec_length(det2_round16(v0.y, v0.z, v1.y, v1.z),
                             det2_round16(v1.x, v0.x, v1.z, v0.z),
                             det2_round16(v0.x, v0.y, v1.x, v1.y));
    } while (++k <= last);
    return sum >> 1;
}

s32 xn_collide_face_test_edges(const xn_vec3 *c, const struct xn_model_face *face,
                               const xn_vec3 *points, s32 r)
{
    u32 left = face->point_count, k = 0;

    do {
        /* edge k: its point and the next (the first after the last) */
        if (xn_collide_segment_sphere_fx(c, r, face_point(face, k, points),
                                         face_point(face, left == 1 ? 0 : k + 1, points)) >= 0)
            return 1;
        k++;
    } while (--left != 0);
    return -1;
}

s32 xn_collide_face_test_vertices(const xn_vec3 *c, const struct xn_model_face *face,
                                  const xn_vec3 *points, s32 r)
{
    u32 left = face->point_count, k = 0;

    do {
        if (xn_collide_point_in_sphere(c, r, face_point(face, k, points)) >= 0)
            return c->x;                /* Quirk Q-COLL-02: the asm restores the centre's x
                                           over the result */
        k++;
    } while (--left != 0);
    return -1;
}

void xn_collide_ref_helpers(void)
{
    /* Quirk Q-COLL-07 (dropped): the asm chains three helpers' registers to no purpose */
}
