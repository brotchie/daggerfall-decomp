/* coll_t.c: test shims of src/engine/collide.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call. The asm takes a vector
   as three registers (x EAX, y EDX, z EBX) and most pointers in ECX, ESI and EDI.

   Two of the asm's outputs are real on some paths and leftovers on the others: the segment
   and line tests' point (EAX, EDX, EBX) when there is no crossing, and the plane-plane
   test's when there is no line. So that the real outputs stay compared, these shims put
   there what the asm leaves, recomputed here (the canonical C does not compute them). The
   outputs that are only ever leftovers are excused instead (config/xngine_dropped.csv). */
#include "xcollide.h"

static void get3(const xn_regs *r, xn_vec3 *v)
{
    v->x = r->eax;
    v->y = r->edx;
    v->z = r->ebx;
}

static void put3(const xn_vec3 *v, xn_regs *r)
{
    r->eax = v->x;
    r->edx = v->y;
    r->ebx = v->z;
}

/* ---- the asm's leftovers (test only) ------------------------------------------------------- */

static s32 round16(const xn_s64 *s)
{
    xn_s64 t = *s;

    xn_s64_addu(&t, 0x8000);
    return xn_s64_shr(&t, 16);
}

static void dot_diff(xn_s64 *s, const xn_vec3 *n, const xn_vec3 *a, const xn_vec3 *b)
{
    xn_s64_mul(s, n->x, a->x - b->x);
    xn_s64_mac(s, n->y, a->y - b->y);
    xn_s64_mac(s, n->z, a->z - b->z);
}

static s32 det2_round16(s32 a, s32 b, s32 c, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, d);
    xn_s64_msub(&t, b, c);
    return round16(&t);
}

/* What 14C2AD / 14C3D9 leave in EAX, EDX, EBX without a crossing: d0's low and high dwords
   and d1's low dword (the same side); nearly parallel: d0's low dword, den's sign test and
   the low dword of the x and y products' sum (vertical: d1's low dword). Returns 0 for a
   crossing (the registers then hold the point). */
static int segment_plane_leftovers(xn_regs *r, const xn_vec3 *n, const xn_vec3 *c,
                                   const xn_vec3 *p0, const xn_vec3 *p1, int vertical)
{
    xn_s64 d0, d1, t;
    s32 dx, dy, dz, den;

    dot_diff(&d0, n, c, p0);
    dot_diff(&d1, n, c, p1);
    if ((d0.hi ^ d1.hi) >= 0) {
        r->eax = d0.lo;
        r->edx = d0.hi;
        r->ebx = d1.lo;
        return 1;
    }
    dx = p1->x - p0->x;
    dy = p1->y - p0->y;
    dz = p1->z - p0->z;
    if (vertical) {
        xn_s64_mul(&t, n->y, dy);
    } else {
        xn_s64_mul(&t, n->x, dx);
        xn_s64_mac(&t, n->y, dy);
        xn_s64_mac(&t, n->z, dz);
    }
    den = round16(&t);
    if (((den - 8) ^ (den + 8)) >= 0)
        return 0;                       /* a crossing: the canonical C's point */
    r->eax = d0.lo;
    r->edx = (den + 8) ^ (den - 8);
    if (!vertical)
        r->ebx = n->x * dx + n->y * dy;
    else
        r->ebx = d1.lo;
    return 1;
}

/* n in EAX EDX EBX, c ECX, p0 ESI, p1 EDI -> t ECX, the point EAX EDX EBX */
static void segment_plane_regs(xn_regs *r, int vertical)
{
    const xn_vec3 *c = (const xn_vec3 *)r->ecx, *p0 = (const xn_vec3 *)r->esi;
    const xn_vec3 *p1 = (const xn_vec3 *)r->edi;
    xn_vec3 n, hit;
    s32 t;

    get3(r, &n);
    t = vertical ? xn_collide_vsegment_plane(&n, c, p0, p1, &hit)
                 : xn_collide_segment_plane(&n, c, p0, p1, &hit);
    r->ecx = t;
    if (!segment_plane_leftovers(r, &n, c, p0, p1, vertical ||
                                 (p1->x == p0->x && p1->z == p0->z)))
        put3(&hit, r);
}

void xn_collide_segment_plane_r(xn_regs *r)
{
    segment_plane_regs(r, 0);
}

void xn_collide_vsegment_plane_r(xn_regs *r)
{
    segment_plane_regs(r, 1);
}

/* the same registers; t and the point always */
void xn_collide_line_plane_point_r(xn_regs *r)
{
    xn_vec3 n, hit;

    get3(r, &n);
    r->ecx = xn_collide_line_plane_point(&n, (const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi,
                                         (const xn_vec3 *)r->edi, &hit);
    put3(&hit, r);
}

void xn_collide_vline_plane_point_r(xn_regs *r)
{
    xn_vec3 n, hit;

    get3(r, &n);
    r->ecx = xn_collide_vline_plane_point(&n, (const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi,
                                          (const xn_vec3 *)r->edi, &hit);
    put3(&hit, r);
}

/* n in EAX EDX EBX, c ECX, s ESI */
void xn_collide_plane_distance_r(xn_regs *r)
{
    xn_vec3 n;

    get3(r, &n);
    r->eax = xn_collide_plane_distance(&n, (const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi);
}

/* What 14C4A6 leaves in EAX, EDX, EBX when it finds no line: the direction's sign tests
   (nearly parallel), or b * c and c (no solution) */
static void plane_plane_leftovers(xn_regs *r, const xn_vec3 *n1, const xn_vec3 *n2)
{
    s32 dx, dy, dz, ax, ay, az, b, c;
    xn_s64 bc;

    dx = det2_round16(n1->y, n1->z, n2->y, n2->z);
    dy = det2_round16(n1->z, n1->x, n2->z, n2->x);
    dz = det2_round16(n1->x, n1->y, n2->x, n2->y);
    if (((dz - 0x1600) ^ (dz + 0x1600)) < 0 && ((dx - 0x1600) ^ (dx + 0x1600)) < 0 &&
        ((dy - 0x1600) ^ (dy + 0x1600)) < 0) {
        r->eax = (dy - 0x1600) ^ (dy + 0x1600);
        r->edx = dy + 0x1600;
        r->ebx = dz;
        return;
    }
    ax = dx < 0 ? -dx : dx;
    ay = dy < 0 ? -dy : dy;
    az = dz < 0 ? -dz : dz;
    if (ax >= ay && ax >= az)
        b = n1->z, c = n2->y;
    else if (ax < ay && ay >= az)
        b = n1->z, c = n2->x;
    else
        b = n1->y, c = n2->x;
    xn_s64_mul(&bc, b, c);
    r->eax = bc.lo;
    r->edx = bc.hi;
    r->ebx = c;
}

/* n1 EAX, p1 EDX, n2 EBX, p2 ECX -> the point in EAX EDX EBX */
void xn_collide_plane_plane_line_r(xn_regs *r)
{
    const xn_vec3 *n1 = (const xn_vec3 *)r->eax, *n2 = (const xn_vec3 *)r->ebx;
    xn_vec3 point;

    if (xn_collide_plane_plane_line(n1, (const xn_vec3 *)r->edx, n2, (const xn_vec3 *)r->ecx,
                                    &point))
        put3(&point, r);
    else
        plane_plane_leftovers(r, n1, n2);
}

/* ---- spheres against planes ---------------------------------------------------------------- */

/* n in EAX EDX EBX, c ECX, s ESI, r EDI -> EAX, the distance EDX */
void xn_collide_sphere_plane_r(xn_regs *r)
{
    xn_vec3 n;
    s32 dist;

    get3(r, &n);
    r->eax = xn_collide_sphere_plane(&n, (const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi,
                                     r->edi, &dist);
    r->edx = dist;
}

void xn_collide_sphere_plane_xz_r(xn_regs *r)
{
    xn_vec3 n;
    s32 dist;

    get3(r, &n);
    r->eax = xn_collide_sphere_plane_xz(&n, (const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi,
                                        r->edi, &dist);
    r->edx = dist;
}

void xn_collide_sphere_plane_yz_r(xn_regs *r)
{
    xn_vec3 n;
    s32 dist;

    get3(r, &n);
    r->eax = xn_collide_sphere_plane_yz(&n, (const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi,
                                        r->edi, &dist);
    r->edx = dist;
}

/* c ECX, s ESI, r EDI */
void xn_collide_sphere_plane_z_r(xn_regs *r)
{
    r->eax = xn_collide_sphere_plane_z((const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi, r->edi);
}

/* ---- segments and points against spheres ---------------------------------------------------- */

/* c in EAX EDX EBX, r ECX, p0 ESI, p1 EDI */
void xn_collide_segment_sphere_fx_r(xn_regs *r)
{
    xn_vec3 c;

    get3(r, &c);
    r->eax = xn_collide_segment_sphere_fx(&c, r->ecx, (const xn_vec3 *)r->esi,
                                          (const xn_vec3 *)r->edi);
}

void xn_collide_segment_sphere_r(xn_regs *r)
{
    xn_vec3 c;

    get3(r, &c);
    r->eax = xn_collide_segment_sphere(&c, r->ecx, (const xn_vec3 *)r->esi,
                                       (const xn_vec3 *)r->edi);
}

void xn_collide_segment_sphere_approx_r(xn_regs *r)
{
    xn_vec3 c;

    get3(r, &c);
    r->eax = xn_collide_segment_sphere_approx(&c, r->ecx, (const xn_vec3 *)r->esi,
                                              (const xn_vec3 *)r->edi);
}

/* c1 in EAX EDX EBX, r1 ECX, c2 ESI, r2 EDI */
void xn_collide_sphere_sphere_r(xn_regs *r)
{
    xn_vec3 c;

    get3(r, &c);
    r->eax = xn_collide_sphere_sphere(&c, r->ecx, (const xn_vec3 *)r->esi, r->edi);
}

void xn_collide_sphere_sphere_approx_r(xn_regs *r)
{
    xn_vec3 c;

    get3(r, &c);
    r->eax = xn_collide_sphere_sphere_approx(&c, r->ecx, (const xn_vec3 *)r->esi, r->edi);
}

/* c in EAX EDX EBX, r ECX, p ESI */
void xn_collide_point_in_sphere_r(xn_regs *r)
{
    xn_vec3 c;

    get3(r, &c);
    r->eax = xn_collide_point_in_sphere(&c, r->ecx, (const xn_vec3 *)r->esi);
}

void xn_collide_point_in_sphere_approx_r(xn_regs *r)
{
    xn_vec3 c;

    get3(r, &c);
    r->eax = xn_collide_point_in_sphere_approx(&c, r->ecx, (const xn_vec3 *)r->esi);
}

/* p in EAX EDX EBX, c ECX, d ESI, h EDI */
void xn_collide_point_in_cylinder_r(xn_regs *r)
{
    xn_vec3 p;

    get3(r, &p);
    r->eax = xn_collide_point_in_cylinder(&p, (const xn_vec3 *)r->ecx, r->esi, r->edi);
}

/* p in EAX EDX EBX, e ECX (a vector; the cube: a value), b ESI */
void xn_collide_point_in_box_r(xn_regs *r)
{
    xn_vec3 p;

    get3(r, &p);
    r->eax = xn_collide_point_in_box(&p, (const xn_vec3 *)r->ecx, (const xn_vec3 *)r->esi);
}

void xn_collide_point_in_cube_r(xn_regs *r)
{
    xn_vec3 p;

    get3(r, &p);
    r->eax = xn_collide_point_in_cube(&p, r->ecx, (const xn_vec3 *)r->esi);
}

/* ---- faces ---------------------------------------------------------------------------------- */

/* p in EAX EDX EBX, the face ECX, the points ESI, the normal EDI */
void xn_collide_point_in_face_r(xn_regs *r)
{
    xn_vec3 p;

    get3(r, &p);
    r->eax = xn_collide_point_in_face(&p, (const struct xn_model_face *)r->ecx,
                                      (const xn_vec3 *)r->esi, (const xn_vec3 *)r->edi);
}

/* -> CF outside */
void xn_collide_point_in_face_v2_r(xn_regs *r)
{
    xn_vec3 p;

    get3(r, &p);
    XN_SETFLAG(r, XN_CF, !xn_collide_point_in_face_v2(&p, (const struct xn_model_face *)r->ecx,
                                                       (const xn_vec3 *)r->esi,
                                                       (const xn_vec3 *)r->edi));
}

/* the face EAX, the points EDX */
void xn_collide_face_area_r(xn_regs *r)
{
    r->eax = xn_collide_face_area((const struct xn_model_face *)r->eax, (const xn_vec3 *)r->edx);
}

/* c in EAX EDX EBX, the face ECX, the points ESI, r EDI */
void xn_collide_face_test_edges_r(xn_regs *r)
{
    xn_vec3 c;

    get3(r, &c);
    r->eax = xn_collide_face_test_edges(&c, (const struct xn_model_face *)r->ecx,
                                        (const xn_vec3 *)r->esi, r->edi);
}

void xn_collide_face_test_vertices_r(xn_regs *r)
{
    xn_vec3 c;

    get3(r, &c);
    r->eax = xn_collide_face_test_vertices(&c, (const struct xn_model_face *)r->ecx,
                                           (const xn_vec3 *)r->esi, r->edi);
}

void xn_collide_ref_helpers_r(xn_regs *r)
{
    (void)r;
    xn_collide_ref_helpers();
}
