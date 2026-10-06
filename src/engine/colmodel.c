/* colmodel.c: XnGine's collision tests of models, probes and flats (canonical C; the interface
   and the module's documentation are in xcollide.h).

   A model's collision spheres each list the faces near them (descending by face offset). A
   test keeps the spheres it meets and merges their lists, then tests those faces. The hits go
   to big_buffer (struct xn_collide_hits); big_buffer + 1000h..3FFFh are the tests' work areas,
   which the game can see: they are written as the asm writes them. The asm kept the tests'
   other state in globals and in its own code; here it is in locals. */
#include "xcollide.h"
#include "xmat.h"
#include "xmath.h"
#include "xvec.h"
#include "xtex.h"

#define HIT_LIST        ((struct xn_collide_hits *)big_buffer)
#define PROBE_MET       ((struct xn_collide_probe_sphere **)(big_buffer + 0x1000))
#define PROBE_LOCAL     ((struct xn_collide_probe_sphere *)(big_buffer + 0x1400))
#define FACE_LIST_A     ((struct xn_model_sphere_face *)(big_buffer + 0x2000))
#define FACE_LIST_B     ((struct xn_model_sphere_face *)(big_buffer + 0x3000))
#define FIXED_SHIFT     0x1C            /* the rotations' products: 2.28 */

/* ---- model data ------------------------------------------------------------------------- */

static void *model_at(const struct xn_model *m, s32 offset)
{
    return (u8 *)m + offset;
}

/* the collision sphere after s */
static const struct xn_model_sphere *next_sphere(const struct xn_model_sphere *s)
{
    return (const struct xn_model_sphere *)((const u8 *)s + 0x12 + 6 * s->face_count);
}

static const xn_vec3 *sphere_centre(const struct xn_model_sphere *s)
{
    return (const xn_vec3 *)&s->x;
}

/* a face list entry's normal: normal4 is the face's index * 4 */
static const xn_vec3 *entry_normal(const xn_vec3 *normals, const struct xn_model_sphere_face *e)
{
    return (const xn_vec3 *)((const u8 *)normals + e->normal4 * 3);
}

/* a point on the face's plane: its first point */
static const xn_vec3 *face_anchor(const struct xn_model_face *face, const xn_vec3 *points)
{
    return (const xn_vec3 *)((const u8 *)points + face->points[0].vertex);
}

static const xn_vec3 *handle_pos(const struct xn_model_handle *h)
{
    return (const xn_vec3 *)&h->x;
}

/* the bounding sphere's radius in world units */
static s32 model_radius(const struct xn_model *m)
{
    return (u32)m->radius >> 8;
}

/* the 16.16 normal of an 8-bit model normal */
static void normal16(xn_vec3 *n, const xn_vec3 *normal)
{
    n->x = normal->x << 8;
    n->y = normal->y << 8;
    n->z = normal->z << 8;
}

/* a world point into model space: (p - pos) << 8 through the inverse of the rotation rot */
static void to_model_space(xn_vec3 *out, const xn_vec3 *p, const xn_vec3 *pos,
                           const xn_mat3 *rot)
{
    out->x = (p->x - pos->x) << 8;
    out->y = (p->y - pos->y) << 8;
    out->z = (p->z - pos->z) << 8;
    xn_mat_transform_transposed(out, rot);
}

/* (v + 80h) >> 8: model space to world units */
static s32 round8(s32 v)
{
    return (v + 0x80) >> 8;
}

/* ---- merging face lists ------------------------------------------------------------------- */

/* The faces of the spheres met so far: the merged list and the other buffer, both in
   big_buffer */
struct face_list {
    struct xn_model_sphere_face *list, *next;
    u32 count;
};

static void face_list_init(struct face_list *fl)
{
    fl->list = FACE_LIST_A;
    fl->next = FACE_LIST_B;
    fl->count = 0;
}

/* Merges sphere's face list into the list (both descending by face offset, a face in both
   once) in the other buffer, which becomes the list */
static void merge_faces(struct face_list *fl, const struct xn_model_sphere *sphere)
{
    const struct xn_model_sphere_face *src = fl->list, *add = sphere->faces;
    struct xn_model_sphere_face *dst = fl->next, *t;
    u32 old = fl->count, left = sphere->face_count;

    fl->count = 0;
    if (old != 0) {
        for (;;) {
            if (src->face > add->face) {
                *dst++ = *src++;
                fl->count++;
                if (--old == 0)
                    goto rest_add;
            } else {
                if (src->face < add->face) {
                    *dst++ = *add;
                    fl->count++;
                }
                add++;
                if (--left == 0)
                    goto rest_src;
            }
        }
    }
rest_add:
    for (; left != 0; left--) {
        *dst++ = *add++;
        fl->count++;
    }
    goto swap;
rest_src:
    for (; old != 0; old--) {
        *dst++ = *src++;
        fl->count++;
    }
swap:
    t = fl->list;
    fl->list = fl->next;
    fl->next = t;
}

/* ---- a segment against a model ------------------------------------------------------------ */

s32 xn_collide_segment_model(struct xn_model_handle *h, const xn_vec3 *start,
                             const xn_vec3 *end, s32 mode)
{
    struct xn_model *m = h->model;
    struct xn_collide_hits *hits = HIT_LIST;
    struct xn_collide_hit *hit;
    const struct xn_model_sphere *sphere;
    const struct xn_model_sphere_face *e;
    const struct xn_model_face *face;
    const xn_vec3 *points, *normals, *normal;
    struct face_list fl;
    xn_mat3 rot;
    xn_vec3 ls, le, n, p, v;
    s32 left, t;

    if (xn_collide_segment_sphere(handle_pos(h), model_radius(m), start, end) < 0)
        return -1;
    if (mode == 2)
        return 0;
    /* the segment in model space */
    xn_mat_from_angles(h->angle_x, h->yaw, h->angle_z, &rot);
    to_model_space(&le, end, handle_pos(h), &rot);
    to_model_space(&ls, start, handle_pos(h), &rot);
    points = model_at(m, m->point_offset);
    normals = model_at(m, m->normal_offset);
    face_list_init(&fl);
    hits->count = 0;
    hit = hits->hits;
    /* the faces of the collision spheres it meets */
    sphere = model_at(m, m->sphere_offset);
    left = m->sphere_count;
    if (left <= 0)
        return 0;                       /* Quirk Q-COLL-03: no spheres, a hit */
    do {
        if (xn_collide_segment_sphere_fx(sphere_centre(sphere), sphere->radius, &ls, &le) >= 0) {
            if (mode == 1)
                return 0;
            merge_faces(&fl, sphere);
        }
        sphere = next_sphere(sphere);
    } while (--left != 0);
    /* the faces it crosses inside their edges */
    left = fl.count;
    if (left == 0)
        return -1;
    e = fl.list;
    do {
        face = model_at(m, e->face);
        normal = entry_normal(normals, e);
        normal16(&n, normal);
        t = xn_collide_segment_plane(&n, face_anchor(face, points), &ls, &le, &p);
        if (t >= 0 && xn_collide_point_in_face(&p, face, points, normal) >= 0) {
            v = p;
            xn_mat_transform(&v, &rot);
            hit->x = round8(v.x) + h->x;
            hit->y = round8(v.y) + h->y;
            hit->z = round8(v.z) + h->z;
            v = *normal;
            xn_mat_transform(&v, &rot);
            hit->nx = v.x;
            hit->ny = v.y;
            hit->nz = v.z;
            hit->face = e->face;
            hit->t_half = (s16)(t >> 1);
            hit = (struct xn_collide_hit *)((u8 *)hit + sizeof *hit);
            hits->count++;
        }
        e++;
    } while (--left != 0);
    return hits->count >= 1 ? (s32)hits : -1;
}

/* ---- two models --------------------------------------------------------------------------- */

s32 xn_collide_model_model(struct xn_model_handle *a, struct xn_model_handle *b, s32 mode)
{
    (void)mode;
    if (xn_collide_sphere_sphere(handle_pos(a), model_radius(a->model), handle_pos(b),
                                 model_radius(b->model)) < 0)
        return -1;
    /* Quirk Q-COLL-04: the asm jumps past its detailed test whatever the mode */
    return 0;
}

/* ---- a probe against a model -------------------------------------------------------------- */

/* the relative rotation of a probe in a model's space: rot_inv * the probe's rotation */
static void relative_rotation(xn_mat3 *rel, const xn_mat3 *rot_inv, s32 angle_x, s32 yaw,
                              s32 angle_z)
{
    xn_mat3 probe;

    xn_mat_from_angles(angle_x, yaw, angle_z, &probe);
    xn_mat_mul_fixed(&rel->m[0][0], &rot_inv->m[0][0], &probe.m[0][0], 3, 3, 3, FIXED_SHIFT);
}

s32 xn_collide_spheres_model(struct xn_model_handle *h, struct xn_collide_probe *p, s32 mode)
{
    struct xn_model *m = h->model;
    struct xn_collide_probe_sphere *ps, **met = PROBE_MET, *local = PROBE_LOCAL;
    struct xn_collide_hits *hits;
    struct xn_collide_hit *hit;
    const struct xn_model_sphere *sphere;
    const struct xn_model_sphere_face *e;
    const struct xn_model_face *face;
    const xn_vec3 *points, *normals, *normal;
    struct face_list fl;
    xn_mat3 rot, rot_inv, rel;
    xn_vec3 c, n, v, origin;
    s32 left, k, count, dist;

    left = p->sphere_count;
    if (left == 0)
        return -1;
    /* the probe's spheres that meet the bounding sphere (pointers at big_buffer + 1000h) */
    ps = p->spheres;
    count = 0;
    do {
        c.x = ps->x + p->position.x;
        c.y = ps->y + p->position.y;
        c.z = ps->z + p->position.z;
        if (xn_collide_sphere_sphere(&c, ps->radius, handle_pos(h), model_radius(m)) >= 0) {
            if (mode == 2)
                return 0;
            met[count++] = ps;
        }
        ps++;
    } while (--left != 0);
    if (count <= 0)
        return -1;
    /* those spheres in model space (big_buffer + 1400h) */
    xn_mat_from_angles(h->angle_x, h->yaw, h->angle_z, &rot);
    xn_mat_transpose_copy3(&rot, &rot_inv);
    origin.x = (p->position.x - h->x) << 8;
    origin.y = (p->position.y - h->y) << 8;
    origin.z = (p->position.z - h->z) << 8;
    xn_mat_transform(&origin, &rot_inv);
    relative_rotation(&rel, &rot_inv, p->angle_x, p->yaw, p->angle_z);
    for (k = 0; k < count; k++) {
        ps = met[k];
        v.x = ps->x << 8;
        v.y = ps->y << 8;
        v.z = ps->z << 8;
        xn_mat_transform(&v, &rel);
        local[k].x = v.x + origin.x;
        local[k].y = v.y + origin.y;
        local[k].z = v.z + origin.z;
        local[k].radius = ps->radius << 8;
    }
    points = model_at(m, m->point_offset);
    normals = model_at(m, m->normal_offset);
    face_list_init(&fl);
    hits = HIT_LIST;
    hits->count = 0;
    hit = hits->hits;
    /* the faces of the collision spheres they meet */
    sphere = model_at(m, m->sphere_offset);
    left = m->sphere_count;
    if (left <= 0)
        return 0;                       /* Quirk Q-COLL-03: no spheres, a hit */
    do {
        for (k = 0; k != count; k++) {
            if (xn_collide_sphere_sphere((const xn_vec3 *)&local[k], local[k].radius,
                                         sphere_centre(sphere), sphere->radius) >= 0) {
                /* Quirk Q-COLL-06 (dropped): in mode 1 ("stop at the first") the asm pops two
                   dwords too many here and returns through its caller's frame */
                if (mode == 1)
                    return 0;
                merge_faces(&fl, sphere);
                break;
            }
        }
        sphere = next_sphere(sphere);
    } while (--left != 0);
    /* the faces a probe sphere touches inside their edges, or on one */
    left = fl.count;
    if (left == 0)
        return -1;
    e = fl.list;
    do {
        face = model_at(m, e->face);
        normal = entry_normal(normals, e);
        normal16(&n, normal);
        for (k = 0; k < count; k++) {
            c = *(const xn_vec3 *)&local[k];
            if (xn_collide_sphere_plane(&n, face_anchor(face, points), &c, local[k].radius,
                                        &dist) < 0)
                continue;
            if (xn_collide_point_in_face(&c, face, points, normal) < 0 &&
                xn_collide_face_test_edges(&c, face, points, local[k].radius) < 0)
                continue;
            hit->face = e->face;
            hit->t_half = -1;
            v = *normal;
            xn_mat_transform(&v, &rot);
            hit->nx = v.x;
            hit->ny = v.y;
            hit->nz = v.z;
            hit = (struct xn_collide_hit *)((u8 *)hit + sizeof *hit);
            hits->count++;
            break;
        }
        e++;
    } while (--left != 0);
    return hits->count >= 1 ? (s32)hits : -1;
}

/* ---- probes ------------------------------------------------------------------------------- */

/* a point into a probe's space: through the inverse of its rotation (no << 8) */
static void to_probe_space(xn_vec3 *out, const xn_vec3 *p, const struct xn_collide_probe *probe,
                           const xn_mat3 *rot_inv)
{
    out->x = p->x - probe->position.x;
    out->y = p->y - probe->position.y;
    out->z = p->z - probe->position.z;
    xn_mat_transform(out, rot_inv);
}

s32 xn_collide_segment_spheres(struct xn_collide_probe *p, const xn_vec3 *start,
                               const xn_vec3 *end)
{
    struct xn_collide_probe_sphere *s;
    xn_mat3 rot, rot_inv;
    xn_vec3 ls, le;
    s32 left;

    xn_mat_from_angles(p->angle_x, p->yaw, p->angle_z, &rot);
    xn_mat_transpose_copy3(&rot, &rot_inv);
    to_probe_space(&le, end, p, &rot_inv);
    to_probe_space(&ls, start, p, &rot_inv);
    left = p->sphere_count;
    if (left <= 0)
        return -1;
    s = p->spheres;
    do {
        if (xn_collide_segment_sphere((const xn_vec3 *)s, s->radius, &ls, &le) >= 0)
            return 0;
        s++;
    } while (--left != 0);
    return -1;
}

s32 xn_collide_spheres_spheres(struct xn_collide_probe *a, struct xn_collide_probe *b)
{
    struct xn_collide_probe_sphere *sa, *sb;
    struct xn_collide_hits *hits = HIT_LIST;
    struct xn_collide_hit *hit;
    xn_mat3 rot, rot_inv, rel;
    xn_vec3 origin, v;
    u32 la, lb;

    xn_mat_from_angles(a->angle_x, a->yaw, a->angle_z, &rot);
    xn_mat_transpose_copy3(&rot, &rot_inv);
    origin.x = (b->position.x - a->position.x) << 8;
    origin.y = (b->position.y - a->position.y) << 8;
    origin.z = (b->position.z - a->position.z) << 8;
    xn_mat_transform(&origin, &rot_inv);
    relative_rotation(&rel, &rot_inv, b->angle_x, b->yaw, b->angle_z);
    /* Quirk Q-COLL-08: the probes' offset is in 24.8 (<< 8), their spheres in world units:
       only probes at the same place can meet */
    hits->count = 0;
    hit = hits->hits;
    sa = a->spheres;
    la = a->sphere_count;
    do {                                /* (a sphere count of 0 runs 2^32 times) */
        sb = b->spheres;
        lb = b->sphere_count;
        do {
            v.x = sb->x;
            v.y = sb->y;
            v.z = sb->z;
            xn_mat_transform(&v, &rel);
            v.x += origin.x;
            v.y += origin.y;
            v.z += origin.z;
            if (xn_collide_sphere_sphere(&v, sb->radius, (const xn_vec3 *)sa, sa->radius) >= 0) {
                /* the pair's sphere numbers in the hit's face field: a's, then b's */
                ((u16 *)&hit->face)[0] = (u16)(a->sphere_count - la);
                ((u16 *)&hit->face)[1] = (u16)(b->sphere_count - lb);
                hit = (struct xn_collide_hit *)((u8 *)hit + sizeof *hit);
                hits->count++;
            }
            sb++;
        } while (--lb != 0);
        sa++;
    } while (--la != 0);
    return hits->count > 0 ? (s32)hits : -1;
}

s32 xn_collide_miss_stk(s32 a, s32 b)
{
    (void)a;
    (void)b;
    return xn_collide_miss();
}

s32 xn_collide_miss(void)
{
    return -1;
}

/* ---- a segment against a flat ------------------------------------------------------------- */

s32 xn_collide_segment_flat(const xn_vec3 *pos, const xn_vec3 *start, const xn_vec3 *end,
                            u32 image, u32 flags, s32 scale, s32 mode)
{
    struct xn_collide_hits *hits = HIT_LIST;
    struct xn_collide_hit *hit = hits->hits;
    const u8 *img;
    xn_vec3 anchor, c, ls, le, normal, p;
    s32 height, width, half_diag, radius, t;

    /* the flat's size, and a sphere around it at its anchor */
    img = (const u8 *)xn_tex_cache_lookup_image(image >> 7, image & 0x7F, 0);
    height = (*(const u16 *)(img + 6) * scale) >> 8;
    width = (*(const u16 *)(img + 4) * scale) >> 8;
    half_diag = xn_vec_length_approx(height >> 1, width >> 1, 0);
    radius = half_diag + (half_diag >> 3);
    anchor.x = 0;
    anchor.y = (height >> xn_collide_flat_anchor_shift[((flags >> 1) & 0xF) * 2]) - (height >> 1);
    anchor.z = 0;
    c.x = pos->x + anchor.x;
    c.y = pos->y + anchor.y;
    c.z = pos->z + anchor.z;
    if (xn_collide_segment_sphere(&c, radius, start, end) < 0)
        return -1;
    if (mode == 2)
        return 0;
    /* the segment from the anchor, << 8; the upright plane facing it */
    ls.x = (start->x - pos->x - anchor.x) << 8;
    ls.y = (start->y - pos->y - anchor.y) << 8;
    ls.z = (start->z - pos->z - anchor.z) << 8;
    le.x = (end->x - pos->x - anchor.x) << 8;
    le.y = (end->y - pos->y - anchor.y) << 8;
    le.z = (end->z - pos->z - anchor.z) << 8;
    normal.x = le.x - ls.x;
    normal.y = 0;
    normal.z = le.z - ls.z;
    xn_vec_normalize(&normal);
    t = xn_collide_segment_plane(&normal, &anchor, &ls, &le, &p);
    if (t < 0)
        return -1;
    if (xn_collide_point_in_cylinder(&p, &anchor, width << 8, height << 8) < 0)
        return -1;
    /* one hit */
    hits->count = 1;
    hit->x = round8(p.x) + pos->x + anchor.x;
    hit->y = round8(p.y) + pos->y + anchor.y;
    hit->z = round8(p.z) + pos->z + anchor.z;
    hit->nx = round8(normal.x) + pos->x;        /* Quirk Q-COLL-05: + the flat's position */
    hit->ny = round8(normal.y) + pos->y;
    hit->nz = round8(normal.z) + pos->z;
    hit->face = -1;
    hit->t_half = (s16)(t >> 1);
    return (s32)hits;
}

s32 xn_collide_segment_flat_stk(const xn_vec3 *pos, const xn_vec3 *start, const xn_vec3 *end,
                                u32 image, u32 flags, s32 scale, s32 mode)
{
    return xn_collide_segment_flat(pos, start, end, image, flags, scale, mode);
}

/* ---- the sphere builder -------------------------------------------------------------------- */

/* extent / cells, the cells' step (an unsigned 64-bit dividend: the asm's `xor edx, edx`) */
static s32 cell_step(s32 extent, s32 cells)
{
    xn_s64 t;

    t.lo = extent;
    t.hi = 0;
    return xn_s64_div_or0(&t, cells);
}

s32 xn_collide_build_model_spheres(struct xn_model *m, s32 r, u8 *out, s32 *count)
{
    const xn_vec3 *points = model_at(m, m->point_offset), *pt, *anchor;
    const xn_vec3 *normal = model_at(m, m->normal_offset);
    const struct xn_model_face *face = model_at(m, m->face_offset);
    s32 min[3], max[3], first[3], centre[3], cells[3], left[3];
    s32 cell, radius, k, axis, ncells, dist, rr, rem, face_index, faces_left;
    s32 *node, *list, *next_node;
    xn_vec3 n, foot, at;
    xn_s64 t;
    u8 *o;
    u16 entries;

    /* the points' bounding box */
    for (axis = 0; axis < 3; axis++) {
        min[axis] = 0x800000;
        max[axis] = (s32)0xFF800000;
    }
    pt = points;
    k = m->point_count;
    do {
        for (axis = 0; axis < 3; axis++) {
            if ((&pt->x)[axis] < min[axis])
                min[axis] = (&pt->x)[axis];
            if ((&pt->x)[axis] > max[axis])
                max[axis] = (&pt->x)[axis];
        }
        pt++;
    } while (--k != 0);
    /* cells of r * sqrt 2, spheres of 1.2 r */
    xn_s64_mul(&t, 0x16A0A, r);
    xn_s64_addu(&t, 0x8000);
    cell = xn_s64_shr(&t, 16);
    xn_s64_mul(&t, r, 0x133);
    xn_s64_addu(&t, 0x80);
    radius = xn_s64_shr(&t, 8);
    for (axis = 0; axis < 3; axis++) {
        xn_s64_set(&t, max[axis] - min[axis]);
        cells[axis] = xn_s64_divrem_or0(&t, cell, &rem) + 1;
    }
    ncells = cells[2] * cells[0] * cells[1];
    if (ncells * 4 >= 0x10000)
        return -1;
    /* the cells' face lists' heads in big_buffer, the nodes {next, face, normal4} after them */
    list = (s32 *)big_buffer;
    k = ncells;
    do
        *list++ = 0;
    while (--k != 0);
    next_node = list;
    for (axis = 0; axis < 3; axis++)
        first[axis] = centre[axis] = (cell_step(max[axis] - min[axis], cells[axis]) >> 1) +
                                     min[axis];
    /* each face: the cells whose sphere touches it */
    face_index = 0;
    faces_left = m->face_count;
    do {
        anchor = face_anchor(face, points);
        normal16(&n, normal);
        list = (s32 *)big_buffer;
        for (left[2] = cells[2]; left[2] != 0; left[2]--) {
            for (left[1] = cells[1]; left[1] != 0; left[1]--) {
                for (left[0] = cells[0]; left[0] != 0; left[0]--) {
                    at.x = centre[0];
                    at.y = centre[1];
                    at.z = centre[2];
                    if (xn_collide_sphere_plane(&n, anchor, &at, radius, &dist) >= 0) {
                        /* the circle the sphere cuts from the plane: its centre and radius */
                        xn_s64_mul(&t, radius, radius);
                        xn_s64_msub(&t, dist, dist);
                        rr = xn_math_isqrt64(t.lo, t.hi);
                        foot.x = ((normal->x * -dist + 0x80) >> 8) + centre[0];
                        foot.y = ((normal->y * -dist + 0x80) >> 8) + centre[1];
                        foot.z = ((normal->z * -dist + 0x80) >> 8) + centre[2];
                        if (xn_collide_point_in_face(&foot, face, points, normal) >= 0 ||
                            xn_collide_face_test_edges(&foot, face, points, rr) >= 0) {
                            node = next_node;
                            node[0] = *list;
                            *list = (s32)node;
                            node[1] = (u8 *)face - (u8 *)m;
                            node[2] = face_index << 2;
                            next_node = node + 3;
                            if ((u8 *)next_node + 12 - big_buffer >= 0x10000)
                                return -1;
                        }
                    }
                    list++;
                    centre[0] += cell;
                }
                centre[0] = first[0];
                centre[1] += cell;
            }
            centre[1] = first[1];
            centre[2] += cell;
        }
        centre[2] = first[2];
        normal++;
        face = (const struct xn_model_face *)((const u8 *)face + 8 + 8 * face->point_count);
        face_index++;
    } while (--faces_left != 0);
    /* a sphere for each cell with faces: centre, radius, its list */
    list = (s32 *)big_buffer;
    o = out;
    *count = 0;
    for (left[2] = cells[2]; left[2] != 0; left[2]--) {
        for (left[1] = cells[1]; left[1] != 0; left[1]--) {
            for (left[0] = cells[0]; left[0] != 0; left[0]--) {
                node = (s32 *)*list;
                if (node != 0) {
                    struct xn_model_sphere *s = (struct xn_model_sphere *)o;

                    (*count)++;
                    s->x = centre[0];
                    s->y = centre[1];
                    s->z = centre[2];
                    s->radius = radius;
                    o += 0x12;
                    entries = 0;
                    do {
                        ((struct xn_model_sphere_face *)o)->face = node[1];
                        ((struct xn_model_sphere_face *)o)->normal4 = (u16)node[2];
                        o += 6;
                        entries++;
                        node = (s32 *)node[0];
                    } while (node != 0);
                    s->face_count = entries;
                }
                list++;
                centre[0] += cell;
            }
            centre[0] = first[0];
            centre[1] += cell;
        }
        centre[1] = first[1];
        centre[2] += cell;
    }
    return o - out;
}
