/* colmodel.c: XnGine's collision tests of models, probes and flats as readable C (xcollide.h;
   see xngine.h and docs/xngine_readable.md).

   A model's collision spheres each list the faces near them (descending by face offset). A
   test keeps the spheres it meets and merges their lists, then tests those faces. Hits go to
   big_buffer (struct xn_collide_hits); big_buffer + 1000h..3FFFh are the tests' work areas. */
#include "xcollide.h"
#include "xmat.h"
#include "xmath.h"
#include "xvec.h"

#define HIT_LIST        ((struct xn_collide_hits *)big_buffer)
#define FACE_LIST_A     ((struct xn_model_sphere_face *)(big_buffer + 0x2000))
#define FACE_LIST_B     ((struct xn_model_sphere_face *)(big_buffer + 0x3000))

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

/* the 16.16 normal of an 8-bit model normal */
static void normal16(xn_vec3 *n, const xn_vec3 *normal)
{
    n->x = normal->x << 8;
    n->y = normal->y << 8;
    n->z = normal->z << 8;
}

/* a world point into model space: (p - pos) << 8 through the model's rotation's inverse */
static void to_model_space(xn_vec3 *out, const xn_vec3 *p, const xn_vec3 *pos)
{
    out->x = (p->x - pos->x) << 8;
    out->y = (p->y - pos->y) << 8;
    out->z = (p->z - pos->z) << 8;
    xn_mat_transform_transposed(out, &xn_collide_work.model_matrix);
}

/* (v + 80h) >> 8: model space to world units */
static s32 round8(s32 v)
{
    return (v + 0x80) >> 8;
}

/* ---- merging face lists ------------------------------------------------------------------- */

/* Merges sphere's face list into *list (count entries) at *next, both descending by face
   offset, a face in both once; then *list is the result and *next the other buffer. */
static void merge_faces(struct xn_model_sphere_face **list, struct xn_model_sphere_face **next,
                        s32 *count, const struct xn_model_sphere *sphere)
{
    const struct xn_model_sphere_face *src = *list, *add = sphere->faces;
    struct xn_model_sphere_face *dst = *next, *t;
    u32 old = *count, left = sphere->face_count;

    *count = 0;
    if (old != 0) {
        for (;;) {
            if (src->face > add->face) {
                *dst++ = *src++;
                (*count)++;
                if (--old == 0)
                    goto rest_add;
            } else {
                if (src->face < add->face) {
                    *dst++ = *add;
                    (*count)++;
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
        (*count)++;
    }
    goto swap;
rest_src:
    for (; old != 0; old--) {
        *dst++ = *src++;
        (*count)++;
    }
swap:
    t = *list;
    *list = *next;
    *next = t;
}

/* ---- a segment against a model ------------------------------------------------------------ */

s32 xn_collide_segment_model(struct xn_model_handle *h, const xn_vec3 *start,
                             const xn_vec3 *end, s32 mode)
{
    struct xn_collide_scratch *w = &xn_collide_work;
    struct xn_collide_seg_state *st = &xn_collide_seg;
    struct xn_model *m = h->model;
    const struct xn_model_sphere *sphere;
    struct xn_model_sphere_face *e;
    struct xn_model_face *face;
    struct xn_collide_hit *hit;
    xn_vec3 n, p, v;
    s32 left;

    w->model = h;
    w->seg_start = (xn_vec3 *)start;
    w->seg_end = (xn_vec3 *)end;
    w->mode = mode;
    if (xn_collide_segment_sphere(handle_pos(h), (u32)m->radius >> 8, start, end) < 0)
        return -1;
    if (mode == 2)
        return 0;
    /* the segment in model space */
    xn_mat_from_angles(h->angle_x, h->yaw, h->angle_z, &w->model_matrix);
    to_model_space(&w->local_end, end, handle_pos(h));
    to_model_space(&w->local_start, start, handle_pos(h));
    st->points = model_at(m, m->point_offset);
    st->normals = model_at(m, m->normal_offset);
    st->model = m;
    st->list = FACE_LIST_A;
    st->list_next = FACE_LIST_B;
    st->list_count = 0;
    st->hits = HIT_LIST;
    st->hits->count = 0;
    st->hit_next = st->hits->hits;
    /* the faces of the collision spheres it meets */
    sphere = model_at(m, m->sphere_offset);
    left = m->sphere_count;
    if (left <= 0)
        return 0;                       /* (no spheres: a hit) */
    do {
        if (xn_collide_segment_sphere_fx(sphere_centre(sphere), sphere->radius, &w->local_start,
                                         &w->local_end) >= 0) {
            if (mode == 1)
                return 0;
            merge_faces(&st->list, &st->list_next, &st->list_count, sphere);
        }
        sphere = next_sphere(sphere);
    } while (--left != 0);
    /* the faces it crosses inside their edges */
    left = st->list_count;
    if (left == 0)
        return -1;
    e = st->list;
    do {
        face = model_at(m, e->face);
        st->normal = (xn_vec3 *)entry_normal(st->normals, e);
        normal16(&n, st->normal);
        w->hit_t = xn_collide_segment_plane(&n, face_anchor(face, st->points), &w->local_start,
                                            &w->local_end, &p);
        if (w->hit_t >= 0 && xn_collide_point_in_face(&p, face, st->points, st->normal) >= 0) {
            hit = st->hit_next;
            v = p;
            xn_mat_transform(&v, &w->model_matrix);
            hit->x = round8(v.x) + h->x;
            hit->y = round8(v.y) + h->y;
            hit->z = round8(v.z) + h->z;
            v = *st->normal;
            xn_mat_transform(&v, &w->model_matrix);
            hit->nx = v.x;
            hit->ny = v.y;
            hit->nz = v.z;
            hit->face = e->face;
            hit->t_half = (s16)(w->hit_t >> 1);
            st->hit_next = (struct xn_collide_hit *)((u8 *)hit + sizeof *hit);
            st->hits->count++;
        }
        e++;
    } while (--left != 0);
    return st->hits->count >= 1 ? (s32)st->hits : -1;
}

/* ---- two models --------------------------------------------------------------------------- */

/* The detailed model-model test the asm has after 14A6C0's exit (from 14A710), which nothing
   reaches: B's position in A's space, A's spheres that meet B's bounding sphere and B's that
   meet A's, the pairs of them that meet, then each pair's B sphere against the A sphere's faces
   (whose plane point is taken from the face's point record, not the point: a bug). 0 on the
   first touch (or the first pair in mode 1), else -1. Kept as C, not routed: entered on its
   own, its exit pops 14A6C0's saved registers. */
static s32 model_model_detail(void)
{
    struct xn_collide_scratch *w = &xn_collide_work;
    struct xn_collide_detail_state *d = &xn_collide_detail;
    struct xn_model_handle *a = w->model, *b = (struct xn_model_handle *)w->probe;
    struct xn_model *ma = a->model, *mb = b->model;
    const struct xn_model_sphere *s, *sa, *sb;
    const struct xn_model_sphere_face *e;
    struct xn_model_sphere **pa, **pb, **pair;
    xn_vec3 v, n;
    s32 left, la, lb, dist;

    xn_mat_from_angles(a->angle_x, a->yaw, a->angle_z, &w->model_matrix);
    xn_mat_transpose_copy3(&w->model_matrix, &w->model_matrix_inv);
    w->local_end.x = (b->x - a->x) << 8;
    w->local_end.y = (b->y - a->y) << 8;
    w->local_end.z = (b->z - a->z) << 8;
    xn_mat_transform(&w->local_end, &w->model_matrix_inv);
    w->local_start.x = -w->local_end.x;
    w->local_start.y = -w->local_end.y;
    w->local_start.z = -w->local_end.z;
    /* A's spheres that meet B's bounding sphere */
    d->a_count = 0;
    d->a_list = d->a_next = (struct xn_model_sphere **)big_buffer;
    s = model_at(ma, ma->sphere_offset);
    left = ma->sphere_count;
    if (left <= 0)
        return 0;
    do {
        if (xn_collide_sphere_sphere(sphere_centre(s), s->radius, &w->local_end, mb->radius) >= 0) {
            *d->a_next++ = (struct xn_model_sphere *)s;
            d->a_count++;
        }
        s = next_sphere(s);
    } while (--left != 0);
    if (d->a_count == 0)
        return -1;
    /* B's that meet A's */
    d->b_count = 0;
    d->b_list = d->b_next = (struct xn_model_sphere **)(big_buffer + 0x1000);
    s = model_at(mb, mb->sphere_offset);
    left = mb->sphere_count;
    if (left <= 0)
        return 0;
    do {
        if (xn_collide_sphere_sphere(sphere_centre(s), s->radius, &w->local_start, ma->radius) >= 0) {
            *d->b_next++ = (struct xn_model_sphere *)s;
            d->b_count++;
        }
        s = next_sphere(s);
    } while (--left != 0);
    if (d->b_count == 0)
        return -1;
    /* the pairs that meet, B's spheres in A's space */
    xn_mat_from_angles(b->angle_x, b->yaw, b->angle_z, &w->probe_matrix);
    xn_mat_mul_fixed(&w->rel_matrix.m[0][0], &w->model_matrix_inv.m[0][0],
                     &w->probe_matrix.m[0][0], 3, 3, 3, 0x1C);
    d->pair_count = 0;
    d->pairs = d->pairs_next = (struct xn_model_sphere **)(big_buffer + 0x2000);
    pa = d->a_list;
    la = d->a_count;
    do {
        sa = *pa;
        pb = d->b_list;
        lb = d->b_count;
        do {
            sb = *pb;
            v = *sphere_centre(sb);
            xn_mat_transform(&v, &w->rel_matrix);
            v.x += w->local_end.x;
            v.y += w->local_end.y;
            v.z += w->local_end.z;
            if (xn_collide_sphere_sphere(&v, sb->radius, sphere_centre(sa), sa->radius) >= 0) {
                if (w->mode == 1)
                    return 0;
                d->pairs_next[0] = (struct xn_model_sphere *)sa;
                d->pairs_next[1] = (struct xn_model_sphere *)sb;
                d->pairs_next += 2;
                d->pair_count++;
            }
            pb++;
        } while (--lb != 0);
        pa++;
    } while (--la != 0);
    if (d->pair_count == 0)
        return -1;
    /* each pair's B sphere against its A sphere's faces */
    d->model = ma;
    d->normals = model_at(ma, ma->normal_offset);
    pair = d->pairs;
    left = d->pair_count;
    do {
        sb = pair[1];
        v = *sphere_centre(sb);
        xn_mat_transform(&v, &w->rel_matrix);
        w->local_pos.x = v.x + w->local_end.x;
        w->local_pos.y = v.y + w->local_end.y;
        w->local_pos.z = v.z + w->local_end.z;
        sa = pair[0];
        e = sa->faces;
        lb = sa->face_count;
        do {
            normal16(&n, entry_normal(d->normals, e));
            if (xn_collide_sphere_plane(&n, (const xn_vec3 *)((u8 *)model_at(ma, e->face) + 8),
                                        &w->local_pos, sb->radius, &dist) >= 0)
                return 0;
            e++;
        } while (--lb != 0);
        pair += 2;
    } while (--left != 0);
    return -1;
}

/* the asm jumps past the detailed test both ways */
#define XN_MODEL_DETAIL     0

s32 xn_collide_model_model(struct xn_model_handle *a, struct xn_model_handle *b, s32 mode)
{
    struct xn_collide_scratch *w = &xn_collide_work;

    w->model = a;
    w->probe = (struct xn_collide_probe *)b;
    w->mode = mode;
    if (xn_collide_sphere_sphere(handle_pos(a), (u32)a->model->radius >> 8, handle_pos(b),
                                 (u32)b->model->radius >> 8) < 0)
        return -1;
    if (mode != 2 && XN_MODEL_DETAIL)
        return model_model_detail();
    return 0;
}

/* ---- a probe against a model -------------------------------------------------------------- */

s32 xn_collide_spheres_model(struct xn_model_handle *h, struct xn_collide_probe *p, s32 mode)
{
    struct xn_collide_scratch *w = &xn_collide_work;
    struct xn_collide_sph_state *st = &xn_collide_sph;
    struct xn_model *m = h->model;
    struct xn_collide_probe_sphere *ps, **met;
    struct xn_collide_probe_sphere *local = (struct xn_collide_probe_sphere *)(big_buffer + 0x1400);
    const struct xn_model_sphere *sphere;
    struct xn_model_sphere_face *e;
    struct xn_model_face *face;
    struct xn_collide_hit *hit;
    const xn_vec3 *normal;
    xn_vec3 c, n, v;
    s32 left, k, count, dist;

    w->model = h;
    w->probe = p;
    w->mode = mode;
    left = p->sphere_count;
    if (left == 0)
        return -1;
    /* the probe's spheres that meet the bounding sphere */
    w->local_pos = p->position;
    ps = p->spheres;
    met = (struct xn_collide_probe_sphere **)(big_buffer + 0x1000);
    count = 0;
    do {
        c.x = ps->x + w->local_pos.x;
        c.y = ps->y + w->local_pos.y;
        c.z = ps->z + w->local_pos.z;
        if (xn_collide_sphere_sphere(&c, ps->radius, handle_pos(h), (u32)m->radius >> 8) >= 0) {
            if (mode == 2)
                return 0;
            *met++ = ps;
            count++;
        }
        ps++;
    } while (--left != 0);
    if (count <= 0)
        return -1;
    st->probe_count = count;
    /* those spheres in model space (big_buffer + 1400h) */
    xn_mat_from_angles(h->angle_x, h->yaw, h->angle_z, &w->model_matrix);
    xn_mat_transpose_copy3(&w->model_matrix, &w->model_matrix_inv);
    w->local_end.x = (p->position.x - h->x) << 8;
    w->local_end.y = (p->position.y - h->y) << 8;
    w->local_end.z = (p->position.z - h->z) << 8;
    xn_mat_transform(&w->local_end, &w->model_matrix_inv);
    xn_mat_from_angles(p->angle_x, p->yaw, p->angle_z, &w->probe_matrix);
    xn_mat_mul_fixed(&w->rel_matrix.m[0][0], &w->model_matrix_inv.m[0][0],
                     &w->probe_matrix.m[0][0], 3, 3, 3, 0x1C);
    met = (struct xn_collide_probe_sphere **)(big_buffer + 0x1000);
    for (k = 0; k < count; k++) {       /* (the asm: a count down from probe_count) */
        ps = met[k];
        v.x = ps->x << 8;
        v.y = ps->y << 8;
        v.z = ps->z << 8;
        xn_mat_transform(&v, &w->rel_matrix);
        local[k].x = v.x + w->local_end.x;
        local[k].y = v.y + w->local_end.y;
        local[k].z = v.z + w->local_end.z;
        local[k].radius = ps->radius << 8;
    }
    st->points = model_at(m, m->point_offset);
    st->normals = model_at(m, m->normal_offset);
    st->model = m;
    st->list = FACE_LIST_A;
    st->list_next = FACE_LIST_B;
    st->list_count = 0;
    st->hits = HIT_LIST;
    st->hits->count = 0;
    st->hit_next = st->hits->hits;
    /* the faces of the collision spheres they meet */
    sphere = model_at(m, m->sphere_offset);
    left = m->sphere_count;
    if (left <= 0)
        return 0;                       /* (no spheres: a hit) */
    do {
        k = 0;
        do {
            if (xn_collide_sphere_sphere((const xn_vec3 *)&local[k], local[k].radius,
                                         sphere_centre(sphere), sphere->radius) >= 0) {
                /* Mode 1 means "stop at the first": here the asm pops two dwords too many
                   and returns through its caller's frame (an original bug no game call
                   reaches); the C returns 0 */
                if (mode == 1)
                    return 0;
                merge_faces(&st->list, &st->list_next, &st->list_count, sphere);
                break;
            }
        } while (++k != st->probe_count);
        sphere = next_sphere(sphere);
    } while (--left != 0);
    /* the faces a probe sphere touches inside their edges, or on one */
    left = st->list_count;
    if (left == 0)
        return -1;
    e = st->list;
    do {
        face = model_at(m, e->face);
        normal = entry_normal(st->normals, e);
        normal16(&n, normal);
        for (k = 0; k < st->probe_count; k++) {
            if (xn_collide_sphere_plane(&n, face_anchor(face, st->points), (const xn_vec3 *)&local[k],
                                        local[k].radius, &dist) < 0)
                continue;
            if (xn_collide_point_in_face((const xn_vec3 *)&local[k], face, st->points, normal) < 0 &&
                xn_collide_face_test_edges((const xn_vec3 *)&local[k], face, st->points,
                                           local[k].radius) < 0)
                continue;
            hit = st->hit_next;
            hit->face = e->face;
            hit->t_half = -1;
            v = *normal;
            xn_mat_transform(&v, &w->model_matrix);
            hit->nx = v.x;
            hit->ny = v.y;
            hit->nz = v.z;
            st->hit_next = (struct xn_collide_hit *)((u8 *)hit + sizeof *hit);
            st->hits->count++;
            break;
        }
        e++;
    } while (--left != 0);
    return st->hits->count >= 1 ? (s32)st->hits : -1;
}

/* the asm interface: the handle in EAX, the probe in EDX, the mode in EBX; it keeps ECX ESI EDI
   EBP (its row lists them as outputs: its mode-1 exit pops them wrongly) */
void xn_collide_spheres_model_r(xn_regs *r)
{
    r->eax = xn_collide_spheres_model((struct xn_model_handle *)r->eax,
                                      (struct xn_collide_probe *)r->edx, r->ebx);
}

/* ---- probes ------------------------------------------------------------------------------- */

/* a point into the probe's space: through the inverse of its rotation (no << 8) */
static void to_probe_space(xn_vec3 *out, const xn_vec3 *p, const struct xn_collide_probe *probe)
{
    out->x = p->x - probe->position.x;
    out->y = p->y - probe->position.y;
    out->z = p->z - probe->position.z;
    xn_mat_transform(out, &xn_collide_work.model_matrix_inv);
}

s32 xn_collide_segment_spheres(struct xn_collide_probe *p, const xn_vec3 *start,
                               const xn_vec3 *end)
{
    struct xn_collide_scratch *w = &xn_collide_work;
    struct xn_collide_probe_sphere *s;
    s32 left;

    w->model = (struct xn_model_handle *)p;
    w->seg_start = (xn_vec3 *)start;
    w->seg_end = (xn_vec3 *)end;
    xn_mat_from_angles(p->angle_x, p->yaw, p->angle_z, &w->model_matrix);
    xn_mat_transpose_copy3(&w->model_matrix, &w->model_matrix_inv);
    to_probe_space(&w->local_end, end, p);
    to_probe_space(&w->local_start, start, p);
    left = p->sphere_count;
    if (left <= 0)
        return -1;
    s = p->spheres;
    do {
        if (xn_collide_segment_sphere((const xn_vec3 *)s, s->radius, &w->local_start,
                                      &w->local_end) >= 0)
            return 0;
        s++;
    } while (--left != 0);
    return -1;
}

s32 xn_collide_spheres_spheres(struct xn_collide_probe *a, struct xn_collide_probe *b)
{
    struct xn_collide_scratch *w = &xn_collide_work;
    struct xn_collide_probe_sphere *sa, *sb;
    struct xn_collide_hit *hit;
    u32 la, lb;
    xn_vec3 v;

    w->model = (struct xn_model_handle *)a;
    w->probe = b;
    xn_mat_from_angles(a->angle_x, a->yaw, a->angle_z, &w->model_matrix);
    xn_mat_transpose_copy3(&w->model_matrix, &w->model_matrix_inv);
    w->local_end.x = (b->position.x - a->position.x) << 8;
    w->local_end.y = (b->position.y - a->position.y) << 8;
    w->local_end.z = (b->position.z - a->position.z) << 8;
    xn_mat_transform(&w->local_end, &w->model_matrix_inv);
    xn_mat_from_angles(b->angle_x, b->yaw, b->angle_z, &w->probe_matrix);
    xn_mat_mul_fixed(&w->rel_matrix.m[0][0], &w->model_matrix_inv.m[0][0],
                     &w->probe_matrix.m[0][0], 3, 3, 3, 0x1C);
    xn_collide_ss_hits = HIT_LIST;
    xn_collide_ss_hits->count = 0;
    xn_collide_ss_hit_next = xn_collide_ss_hits->hits;
    sa = a->spheres;
    la = a->sphere_count;
    do {
        sb = b->spheres;
        lb = b->sphere_count;
        do {
            v.x = sb->x;
            v.y = sb->y;
            v.z = sb->z;
            xn_mat_transform(&v, &w->rel_matrix);
            v.x += w->local_end.x;
            v.y += w->local_end.y;
            v.z += w->local_end.z;
            if (xn_collide_sphere_sphere(&v, sb->radius, (const xn_vec3 *)sa, sa->radius) >= 0) {
                /* the pair's sphere numbers in the hit's face field: A's, then B's */
                hit = xn_collide_ss_hit_next;
                xn_collide_ss_hit_next = (struct xn_collide_hit *)((u8 *)hit + sizeof *hit);
                ((u16 *)&hit->face)[0] = (u16)(a->sphere_count - la);
                ((u16 *)&hit->face)[1] = (u16)(b->sphere_count - lb);
                xn_collide_ss_hits->count++;
            }
            sb++;
        } while (--lb != 0);
        sa++;
    } while (--la != 0);
    return xn_collide_ss_hits->count > 0 ? (s32)xn_collide_ss_hits : -1;
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
    struct xn_collide_scratch *w = &xn_collide_work;
    struct xn_collide_hits *hits = HIT_LIST;
    struct xn_collide_hit *hit = hits->hits;
    const u8 *img;
    xn_vec3 c, d, p;
    s32 half_diag, radius, t;

    w->model = (struct xn_model_handle *)pos;
    w->seg_start = (xn_vec3 *)start;
    w->seg_end = (xn_vec3 *)end;
    w->mode = mode;
    /* the flat's size, and a sphere around it at its anchor */
    img = xn_tex_cache_lookup_image(image >> 7, image & 0x7F, 0);
    xn_collide_flat_height = (*(const u16 *)(img + 6) * scale) >> 8;
    xn_collide_flat_width = (*(const u16 *)(img + 4) * scale) >> 8;
    half_diag = xn_vec_length_approx(xn_collide_flat_height >> 1, xn_collide_flat_width >> 1, 0);
    radius = half_diag + (half_diag >> 3);
    w->local_pos.x = 0;
    w->local_pos.y = (xn_collide_flat_height >> xn_collide_flat_anchor_shift[((flags >> 1) & 0xF) * 2])
                     - (xn_collide_flat_height >> 1);
    w->local_pos.z = 0;
    c.x = pos->x + w->local_pos.x;
    c.y = pos->y + w->local_pos.y;
    c.z = pos->z + w->local_pos.z;
    if (xn_collide_segment_sphere(&c, radius, start, end) < 0)
        return -1;
    if (mode == 2)
        return 0;
    /* the segment from the anchor, << 8; the upright plane facing it */
    w->local_start.x = (start->x - pos->x - w->local_pos.x) << 8;
    w->local_start.y = (start->y - pos->y - w->local_pos.y) << 8;
    w->local_start.z = (start->z - pos->z - w->local_pos.z) << 8;
    w->local_end.x = (end->x - pos->x - w->local_pos.x) << 8;
    w->local_end.y = (end->y - pos->y - w->local_pos.y) << 8;
    w->local_end.z = (end->z - pos->z - w->local_pos.z) << 8;
    d.x = w->local_end.x - w->local_start.x;
    d.y = 0;
    d.z = w->local_end.z - w->local_start.z;
    xn_vec_normalize(&d);
    w->flat_normal = d;
    t = xn_collide_segment_plane(&w->flat_normal, &w->local_pos, &w->local_start, &w->local_end,
                                 &p);
    if (t < 0)
        return -1;
    w->hit_t = t;
    if (xn_collide_point_in_cylinder(p.x, p.y, p.z, &w->local_pos, xn_collide_flat_width << 8,
                                     xn_collide_flat_height << 8) < 0)
        return -1;
    /* one hit; its normal gets the flat's position added (a bug, kept) */
    hits->count = 1;
    hit->x = round8(p.x) + pos->x + w->local_pos.x;
    hit->y = round8(p.y) + pos->y + w->local_pos.y;
    hit->z = round8(p.z) + pos->z + w->local_pos.z;
    hit->nx = round8(w->flat_normal.x) + pos->x;
    hit->ny = round8(w->flat_normal.y) + pos->y;
    hit->nz = round8(w->flat_normal.z) + pos->z;
    hit->face = -1;
    hit->t_half = (s16)(w->hit_t >> 1);
    return (s32)hits;
}

/* the asm interface: pos in EAX, start in EDX, end in EBX, the image in ECX, the flags in ESI,
   the scale in EDI, the mode in EBP */
void xn_collide_segment_flat_r(xn_regs *r)
{
    r->eax = xn_collide_segment_flat((const xn_vec3 *)r->eax, (const xn_vec3 *)r->edx,
                                     (const xn_vec3 *)r->ebx, r->ecx, r->esi, r->edi, r->ebp);
}

s32 xn_collide_segment_flat_stk(const xn_vec3 *pos, const xn_vec3 *start, const xn_vec3 *end,
                                u32 image, u32 flags, s32 scale, s32 mode)
{
    return xn_collide_segment_flat(pos, start, end, image, flags, scale, mode);
}

/* ---- the sphere builder -------------------------------------------------------------------- */

/* (max - min) / cells, the extent's step (xor edx: an unsigned high dword) */
static s32 cell_step(s32 extent, s32 cells)
{
    xn_s64 t;

    t.lo = extent;
    t.hi = 0;
    return xn_s64_div(&t, cells);
}

s32 xn_collide_build_model_spheres(struct xn_model *m, s32 r, u8 *out, s32 *count)
{
    struct xn_collide_scratch *w = &xn_collide_work;
    struct xn_collide_build_state *b = &xn_collide_build;
    s32 *min = &w->build_min.x, *max = &w->build_max.x;
    s32 *first = &w->build_first.x, *centre = &w->build_centre.x;
    s32 *cells = w->build_cells, *left = w->build_left;
    const xn_vec3 *pt, *anchor;
    xn_vec3 n, foot;
    xn_s64 t;
    s32 k, axis, ncells, dist, rr, rem;
    s32 *node, *head, *cell;
    u8 *o;
    u16 entries;

    w->model = (struct xn_model_handle *)m;
    w->build_radius = r;
    w->build_out = (char *)out;
    b->points = model_at(m, m->point_offset);
    b->normals = model_at(m, m->normal_offset);
    b->faces_left = m->face_count;
    b->face = model_at(m, m->face_offset);
    b->face_index = 0;
    /* the points' bounding box */
    for (axis = 0; axis < 3; axis++) {
        min[axis] = 0x800000;
        max[axis] = (s32)0xFF800000;
    }
    pt = b->points;
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
    w->build_cell = xn_s64_shr(&t, 16);
    xn_s64_mul(&t, r, 0x133);
    xn_s64_addu(&t, 0x80);
    w->build_radius = xn_s64_shr(&t, 8);
    for (axis = 0; axis < 3; axis++) {
        xn_s64_set(&t, max[axis] - min[axis]);
        cells[axis] = xn_s64_divrem(&t, w->build_cell, &rem) + 1;
    }
    ncells = cells[2] * cells[0] * cells[1];
    if (ncells * 4 >= 0x10000) {
        *count = rem;                   /* (the asm's EDX: the last remainder) */
        return -1;
    }
    /* the cells' face lists' heads in big_buffer, the nodes after them */
    cell = (s32 *)big_buffer;
    w->build_cell_list = cell;
    do
        *cell++ = 0;
    while (--ncells != 0);
    b->next_node = cell;
    for (axis = 0; axis < 3; axis++)
        first[axis] = centre[axis] = (cell_step(max[axis] - min[axis], cells[axis]) >> 1) +
                                     min[axis];
    /* each face: the cells whose sphere touches it */
    do {
        anchor = face_anchor(b->face, b->points);
        normal16(&n, b->normals);
        left[2] = cells[2];
        do {
            left[1] = cells[1];
            do {
                left[0] = cells[0];
                do {
                    if (xn_collide_sphere_plane(&n, anchor, &w->build_centre, w->build_radius,
                                                &dist) >= 0) {
                        /* the circle the sphere cuts from the plane: its centre and radius */
                        xn_s64_mul(&t, w->build_radius, w->build_radius);
                        xn_s64_msub(&t, dist, dist);
                        rr = xn_math_isqrt64(t.lo, t.hi);
                        foot.x = ((b->normals->x * -dist + 0x80) >> 8) + centre[0];
                        foot.y = ((b->normals->y * -dist + 0x80) >> 8) + centre[1];
                        foot.z = ((b->normals->z * -dist + 0x80) >> 8) + centre[2];
                        if (xn_collide_point_in_face(&foot, b->face, b->points, b->normals) >= 0 ||
                            xn_collide_face_test_edges(&foot, b->face, b->points, rr) >= 0) {
                            node = b->next_node;
                            head = w->build_cell_list;
                            node[0] = *head;
                            *head = (s32)node;
                            node[1] = (u8 *)b->face - (u8 *)m;
                            node[2] = b->face_index << 2;
                            b->next_node = node + 3;
                            if ((u8 *)b->next_node + 12 - big_buffer >= 0x10000) {
                                *count = (s32)node;     /* (the asm's EDX) */
                                return -1;
                            }
                        }
                    }
                    w->build_cell_list++;
                    centre[0] += w->build_cell;
                } while (--left[0] != 0);
                centre[0] = first[0];
                centre[1] += w->build_cell;
            } while (--left[1] != 0);
            centre[1] = first[1];
            centre[2] += w->build_cell;
        } while (--left[2] != 0);
        centre[2] = first[2];
        w->build_cell_list = (s32 *)big_buffer;
        b->normals++;
        b->face = (struct xn_model_face *)((u8 *)b->face + 8 + 8 * b->face->point_count);
        b->face_index++;
    } while (--b->faces_left != 0);
    /* a sphere for each cell with faces: centre, radius, its list */
    cell = w->build_cell_list;
    o = out;
    *count = 0;
    left[2] = cells[2];
    do {
        left[1] = cells[1];
        do {
            left[0] = cells[0];
            do {
                node = (s32 *)*cell;
                if (node != 0) {
                    struct xn_model_sphere *s = (struct xn_model_sphere *)o;

                    (*count)++;
                    s->x = centre[0];
                    s->y = centre[1];
                    s->z = centre[2];
                    s->radius = w->build_radius;
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
                cell++;
                centre[0] += w->build_cell;
            } while (--left[0] != 0);
            centre[0] = first[0];
            centre[1] += w->build_cell;
        } while (--left[1] != 0);
        centre[1] = first[1];
        centre[2] += w->build_cell;
    } while (--left[2] != 0);
    return o - out;
}

/* the asm interface: the model in EAX, r in EDX, out in EBX; the bytes in EAX, the count in
   EDX */
void xn_collide_build_model_spheres_r(xn_regs *r)
{
    s32 count;

    r->eax = xn_collide_build_model_spheres((struct xn_model *)r->eax, r->edx, (u8 *)r->ebx,
                                            &count);
    r->edx = count;
}
