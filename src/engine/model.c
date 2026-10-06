/* model.c: XnGine's ARCH3D models (canonical C; the interface and the module's documentation
   are in xmodel.h). */
#include "xmodel.h"
#include "xcam.h"
#include "xpoly.h"
#include "xtex.h"
#include "xvec.h"
#include "xmat.h"
#include "xmath.h"
#include "xdraw.h"
#include "xrender.h"
#include "xdos.h"
#include "xkbd.h"
#include "xjoy.h"
#include "xgfx.h"
#include "xmem.h"

extern u8 *player_object;                       /* x, y, z ints at +7, +0Bh, +0Fh */
extern struct xn_vert_cam xn_vert_cam[1024];   /* indexed by byte offset (index * 12) */
extern struct xn_vert_screen xn_vert_screen[1024];
extern struct xn_vert_flags xn_vert_flags[1024];
extern s32 xn_span_dzdx;                        /* the S-buffer's 1/z step of the polygon */
extern struct xn_light xn_light_table[33];

#define VERT_FLAGS_MAX  1024            /* the vertex arrays' entries */

/* the handle whose angles the game passes (&handle->pad_0c) */
#define HANDLE_OF(angles) ((struct xn_model_handle *)((u8 *)(angles) - 0x0C))

/* a model's lists: by their offsets from the model's start */
#define POINTS(m) ((const xn_vec3 *)((const u8 *)(m) + (m)->point_offset))
#define NORMALS(m) ((xn_vec3 *)((u8 *)(m) + (m)->normal_offset))
#define FACES(m) ((struct xn_model_face *)((u8 *)(m) + (m)->face_offset))
#define FACE_DATA(m) ((struct xn_model_face_data *)((u8 *)(m) + (m)->face_data_offset))
/* a point by its byte offset (a face point's vertex), and the next face */
#define POINT_AT(pts, off) ((const xn_vec3 *)((const u8 *)(pts) + (off)))
#define NEXT_FACE(f) ((struct xn_model_face *)((u8 *)(f) + 8 + 8 * (f)->point_count))

/* The models' fatal exit (13FEC3, 13FEEB): the engine shut down, msg printed, the game ended;
   the too-many-vertices exit does not shut the joystick down */
static void fatal(const char *msg, int joy)
{
    xn_kbd_remove();
    if (joy)
        xn_joy_shutdown();
    xn_render_shutdown();
    xn_gfx_restore_mode();
    xn_mem_shutdown();
    xn_dos_print(msg);
    xn_dos_exit(0);
}

/* ---- a placement's angles ------------------------------------------------------------------ */

void xn_model_set_angles(s32 pitch, s32 yaw, s32 roll, s16 *angles)
{
    angles[1] = (s16)pitch;
    angles[2] = (s16)yaw;
    angles[3] = (s16)roll;
}

void xn_model_compose_angles(s16 *angles, s32 pitch, s32 yaw, s32 roll)
{
    struct xn_model_handle *h = HANDLE_OF(angles);
    s32 p, y, r;

    xn_mat_from_angles((u16)angles[1] | xn_model_angle_or_bits,
                       (u16)angles[2] | xn_model_angle_or_bits,
                       (u16)angles[3] | xn_model_angle_or_bits, &xn_model_base_matrix);
    xn_mat_from_angles(pitch, yaw, roll, &xn_model_object_matrix);
    xn_mat_multiply(&xn_model_base_matrix, &xn_model_object_matrix, &xn_model_combined_matrix);
    if (xn_mat_to_angles(&xn_model_combined_matrix, &p, &y, &r)) {
        h->angle_x = p;
        h->yaw = y;
        h->angle_z = r;
    } else {
        h->angle_x = angles[1];
        h->yaw = angles[2];
        h->angle_z = angles[3];
    }
}

void xn_model_set_angles_yaw_offset(s16 *angles, s32 yaw_offset)
{
    struct xn_model_handle *h = HANDLE_OF(angles);

    h->angle_x = (u16)angles[1];
    h->yaw = (u16)angles[2] + yaw_offset;
    h->angle_z = (u16)angles[3];
}

/* ---- the game's helpers ----------------------------------------------------------------- */

void xn_model_push_player_from_pick(const struct xn_poly *pick)
{
    const struct xn_model_handle *h = pick->handle;
    xn_mat3 *m = (xn_mat3 *)big_buffer;
    xn_vec3 n;

    xn_mat_from_angles(h->angle_x, h->yaw, h->angle_z, m);
    n.x = pick->normal[0];
    n.y = pick->normal[1];
    n.z = pick->normal[2];
    xn_mat_transform(&n, m);
    *(s32 *)(player_object + 7) -= (n.x * 0x80 + 0x80) >> 8;
    *(s32 *)(player_object + 0x0F) -= (n.z * 0x80 + 0x80) >> 8;
}

s32 xn_model_max_y(const struct xn_model *m)
{
    const xn_vec3 *p = POINTS(m);
    u32 n = m->point_count;
    s32 top = -100000;

    do {                                /* Quirk Q-MODEL-02: a count of 0 runs 2^32 times */
        if (top <= p->y)
            top = p->y;
        p++;
    } while (--n != 0);
    return top;
}

void xn_model_xz_extent(const struct xn_model *m, u32 *dx, u32 *dz)
{
    const xn_vec3 *p = POINTS(m);
    u32 n = m->point_count;
    s32 x0 = -100000, x1 = 100000, z0 = -100000, z1 = 100000;  /* Quirk Q-MODEL-01 */

    do {
        if (x0 >= p->x)
            x0 = p->x;
        if (x1 <= p->x)
            x1 = p->x;
        if (z0 >= p->z)
            z0 = p->z;
        if (z1 <= p->z)
            z1 = p->z;
        p++;
    } while (--n != 0);
    *dz = (u32)(z1 - z0) >> 8;
    *dx = (u32)(x1 - x0) >> 8;
}

/* the mean of the model's vertices (cdq; idiv) into the pick's point */
static void centroid(const struct xn_model *m)
{
    const xn_vec3 *p = POINTS(m);
    u32 n = m->point_count;
    xn_vec3 sum;

    sum.x = sum.y = sum.z = 0;
    do {
        sum.x += p->x;
        sum.y += p->y;
        sum.z += p->z;
        p++;
    } while (--n != 0);
    xn_pick_view_x = xn_idiv64_or0(sum.x >> 31, sum.x, m->point_count);
    xn_pick_view_y = xn_idiv64_or0(sum.y >> 31, sum.y, m->point_count);
    pick_distance = xn_idiv64_or0(sum.z >> 31, sum.z, m->point_count);
}

void xn_model_calc_centroid(const struct xn_model *m)
{
    centroid(m);
}

void xn_model_centroid_to_pick(const struct xn_model_handle *h)
{
    centroid(h->model);
}

/* ---- preparing a model ------------------------------------------------------------------------ */

struct xn_model *xn_model_prepare(struct xn_model *m)
{
    struct xn_model_face *f;
    u32 nf;

    if (m->version < 0x362E3276) {      /* before "v2.6": vertex offsets are index * 4 */
        if ((u32)m->point_count >= 0x400) {
            fatal(xn_model_msg_too_many_verts, 0);
            return m;
        }
        f = FACES(m);
        nf = m->face_count;
        do {                            /* Quirk Q-MODEL-02 (`loop`) */
            struct xn_model_face_point *p = f->points;
            u32 np = f->point_count;

            do {
                p->vertex *= 3;
                p++;
            } while (--np != 0);
            f = NEXT_FACE(f);
        } while (--nf != 0);
    }
    xn_model_calc_uv_axes(m);
    xn_model_calc_face_planes(m);
    xn_model_calc_radius(m);
    xn_model_calc_face_normals(m, 8);
    f = FACES(m);
    nf = m->face_count;
    do {
        f->points[0].uv_packed <<= 4;
        if (f->point_count > 0x18) {
            fatal(xn_model_msg_too_many_verts, 0);
            return m;
        }
        f = NEXT_FACE(f);
    } while (--nf != 0);
    /* every vertex offset a multiple of 12 within the vertices */
    f = FACES(m);
    nf = m->face_count;
    do {
        struct xn_model_face_point *p = f->points;
        u32 np = f->point_count;

        do {
            if ((u32)p->vertex % 12 != 0 || (s32)((u32)p->vertex / 12) >= m->point_count) {
                fatal(xn_model_msg_corrupted, 1);
                return m;
            }
            p++;
        } while (--np != 0);
        f = (struct xn_model_face *)p;
    } while (--nf != 0);
    return m;
}

void xn_model_calc_uv_axes(struct xn_model *m)
{
    s32 k = 0;

    do {                                /* each frame (once when there are none) */
        struct xn_model_face *f;
        struct xn_model_face_data *d;
        u32 nf;

        if (m->frame_count != 0) {
            const struct xn_model_frame *fr = (const struct xn_model_frame *)
                ((u8 *)m + m->frame_table_offset + k * 16);

            m->point_offset = fr->point_offset;
            m->normal_offset = fr->normal_offset;
            m->face_data_offset = fr->face_data_offset;
        }
        d = FACE_DATA(m);
        f = FACES(m);
        nf = m->face_count;
        do {
            if (f->texture >= 0x100)
                xn_model_calc_face_uv_axes(m, f, d);
            d++;
            f = NEXT_FACE(f);
        } while (--nf != 0);
    } while (++k < m->frame_count);
}

/* the 64-bit sum of squares of v, bits 8..39 */
static s32 len2(const xn_vec3 *v)
{
    xn_s64 t;

    xn_s64_mul(&t, v->x, v->x);
    xn_s64_mac(&t, v->y, v->y);
    xn_s64_mac(&t, v->z, v->z);
    return xn_s64_shr(&t, 8);
}

/* (v << 25) / d, 64-bit, or 0 */
static s32 scale25(s32 v, s32 d)
{
    xn_s64 t;

    xn_s64_set(&t, v);
    xn_s64_shl(&t, 25);
    return xn_s64_div_or0(&t, d);
}

/* one texture axis: d1 e1 + d2' e2, d2' = d2 - (d1 t >> 16); 32-bit products */
static void uv_axis(s32 d1, s32 d2, s32 t, const xn_vec3 *e1, const xn_vec3 *e2, xn_vec3 *out)
{
    d2 -= xn_mulshr(d1, t, 16);
    out->x = d1 * e1->x + d2 * e2->x;
    out->y = d1 * e1->y + d2 * e2->y;
    out->z = d1 * e1->z + d2 * e2->z;
}

void xn_model_calc_face_uv_axes(const struct xn_model *m, const struct xn_model_face *face,
                                struct xn_model_face_data *out)
{
    const xn_vec3 *pts = POINTS(m);
    const xn_vec3 *p0 = POINT_AT(pts, face->points[0].vertex);
    const xn_vec3 *p1 = POINT_AT(pts, face->points[1].vertex);
    const xn_vec3 *p2 = POINT_AT(pts, face->points[2].vertex);
    xn_vec3 *e1 = &xn_scratch_vecs.a;   /* Quirk Q-MODEL-03: the pick's scratch point */
    xn_vec3 e2;
    xn_s64 n;
    s32 l1, l2, t;

    e1->x = p1->x - p0->x;
    e1->y = p1->y - p0->y;
    e1->z = p1->z - p0->z;
    l1 = len2(e1);
    if (l1 == 0)
        return;
    e2.x = p2->x - p1->x;
    e2.y = p2->y - p1->y;
    e2.z = p2->z - p1->z;
    /* t = (e1 . e2 << 8) / |e1|^2: e2's part along e1, 16.16 */
    xn_s64_mul(&n, e1->x, e2.x);
    xn_s64_mac(&n, e1->y, e2.y);
    xn_s64_mac(&n, e1->z, e2.z);
    xn_s64_shl(&n, 8);
    t = xn_s64_div_or0(&n, l1);
    e2.x -= xn_mulshr(e1->x, t, 16);
    e2.y -= xn_mulshr(e1->y, t, 16);
    e2.z -= xn_mulshr(e1->z, t, 16);
    e1->x = scale25(e1->x, l1);
    e1->y = scale25(e1->y, l1);
    e1->z = scale25(e1->z, l1);
    l2 = len2(&e2);
    if (l2 == 0)
        return;
    e2.x = scale25(e2.x, l2);
    e2.y = scale25(e2.y, l2);
    e2.z = scale25(e2.z, l2);
    /* the file's u/v deltas of points 1 and 2 */
    uv_axis(face->points[1].uv[0], face->points[2].uv[0], t, e1, &e2, &out->u_axis);
    uv_axis(face->points[1].uv[1], face->points[2].uv[1], t, e1, &e2, &out->v_axis);
}

void xn_model_calc_face_planes(struct xn_model *m)
{
    struct xn_model_face *f = FACES(m);
    const xn_vec3 *n = NORMALS(m);
    struct xn_model_face_data *d = FACE_DATA(m);
    u32 nf = m->face_count;

    do {
        const xn_vec3 *p = POINT_AT(POINTS(m), f->points[0].vertex);

        f->points[1].plane_d = p->x * n->x + p->y * n->y + p->z * n->z;
        f->points[2].data = d;
        d++;
        n++;
        f = NEXT_FACE(f);
    } while (--nf != 0);
}

void xn_model_calc_radius(struct xn_model *m)
{
    const xn_vec3 *p = POINTS(m);
    u32 n = m->point_count;
    s32 radius = 0;

    do {
        xn_s64 t;
        s32 d;

        xn_s64_mul(&t, p->x, p->x);
        xn_s64_mac(&t, p->y, p->y);
        xn_s64_mac(&t, p->z, p->z);
        d = xn_math_isqrt64(t.lo, t.hi);
        if (radius < d)
            radius = d;
        p++;
    } while (--n != 0);
    m->radius = radius;
}

void xn_model_calc_face_normals(struct xn_model *m, s32 bits)
{
    struct xn_model_face *f = FACES(m);
    xn_vec3 *n = NORMALS(m);
    u32 nf = m->face_count;

    do {
        xn_model_calc_face_normal(m, f, n, bits);
        n++;
        f = NEXT_FACE(f);
    } while (--nf != 0);
}

void xn_model_calc_face_normal(const struct xn_model *m, const struct xn_model_face *face,
                               xn_vec3 *out, s32 bits)
{
    const xn_vec3 *pts = POINTS(m);
    const xn_vec3 *p0 = POINT_AT(pts, face->points[0].vertex);
    const xn_vec3 *p1 = POINT_AT(pts, face->points[1].vertex);
    const xn_vec3 *p2 = POINT_AT(pts, face->points[2].vertex);
    xn_vec3 *e1 = &xn_scratch_vecs.a;   /* Quirk Q-MODEL-03: the pick's scratch point */
    xn_vec3 e2, n;
    s32 shift;

    e1->x = p1->x - p0->x;
    e1->y = p1->y - p0->y;
    e1->z = p1->z - p0->z;
    e2.x = p2->x - p1->x;
    e2.y = p2->y - p1->y;
    e2.z = p2->z - p1->z;
    xn_vec_cross(e1, &e2, &n);
    xn_vec_normalize(&n);
    shift = 16 - bits;
    if (shift > 0) {
        n.x >>= shift & 31;
        n.y >>= shift & 31;
        n.z >>= shift & 31;
    }
    *out = n;
}

/* ---- the frame's queue ----------------------------------------------------------------------- */

void xn_model_submit(struct xn_model_handle *h, s32 frame)
{
    xn_model_cull_and_queue(h, (u8)frame);
}

void xn_model_cull_and_queue(struct xn_model_handle *h, u8 frame)
{
    struct xn_sort_pair *pair;
    s32 radius, key, residue;

    h->frame = frame;
    h->flags = 0;
    h->rel_x = (h->x - xn_cam_x) << 8;
    h->rel_y = (h->y - xn_cam_y) << 8;
    h->rel_z = (h->z - xn_cam_z) << 8;
    radius = h->model->radius;
    if (xn_cam_cull_sphere(h->rel_x, h->rel_y, h->rel_z, radius, &residue))
        return;
    if ((u32)++xn_model_queue_count >= 200)
        return;
    pair = xn_model_queue_ptr;
    /* the key: the distance (the sphere test left the centre in view space) less the radius */
    key = 0;
    if (pick_distance >= 0) {
        s32 x = xn_pick_view_x >> 8, y = xn_pick_view_y >> 8, z = pick_distance >> 8;

        key = xn_math_isqrt_lookup(x * x + y * y + z * z) - (radius >> 8);
        if (key < 0)
            key = 0;
    }
    xn_model_queue_ptr++;
    pair->value = h;
    pair->key = key;
}

/* ---- drawing --------------------------------------------------------------------------------- */

int xn_model_draw(struct xn_model_handle *h)
{
    struct xn_model *m = h->model;
    struct xn_model_draw_state s;
    struct xn_model_matrix_slot *slot;
    s32 depth_scale;

    if (xn_model_is_occluded(m, h)) {
        h->flags |= 1;
        return 0;
    }
    xn_model_drawn_count++;
    s.handle = h;
    xn_mat_from_angles(h->angle_x, h->yaw, h->angle_z, &xn_model_rot_matrix);
    slot = xn_render_matrix_next;
    xn_mat_multiply(&xn_cam_view_matrix, &xn_model_rot_matrix, (xn_mat3 *)slot);
    /* the eye in object space (the object's origin seen from the eye) */
    s.rel.x = h->rel_x;
    s.rel.y = h->rel_y;
    s.rel.z = h->rel_z;
    xn_mat_transform_transposed(&s.rel, &xn_model_rot_matrix);
    h->rel_x = s.rel.x;
    h->rel_y = s.rel.y;
    h->rel_z = s.rel.z;
    /* d(1/z) per unit of a face's plane: the matrix's first row times
       2^48 / (scale_x * focal_x) (the product 32-bit) */
    depth_scale = xn_udiv64_or0(0x10000, 0, xn_cam_scale_x * xn_cam_focal_x);
    s.dz_row.x = xn_mulhi(slot->m[0][0], depth_scale);
    s.dz_row.y = xn_mulhi(slot->m[0][1], depth_scale);
    s.dz_row.z = xn_mulhi(slot->m[0][2], depth_scale);
    h->matrix = slot;
    s.matrix = slot;
    if (m->frame_count != 0)
        xn_model_set_frame(m, h->frame);
    if (xn_model_draw_faces(m, &s))
        return 1;
    xn_model_build_light_list(h);
    xn_model_scale_matrix(slot);
    xn_render_matrix_next++;
    return 0;
}

void xn_model_set_frame(struct xn_model *m, s32 frame)
{
    const struct xn_model_frame *fr;
    s32 last = m->frame_count - 1;

    if (frame < 0)
        frame = 0;
    else if (frame > last)
        frame = last;
    fr = (const struct xn_model_frame *)((u8 *)m + m->frame_table_offset + frame * 16);
    m->point_offset = fr->point_offset;
    m->normal_offset = fr->normal_offset;
    m->face_data_offset = fr->face_data_offset;
    xn_model_calc_face_planes(m);
}

void xn_model_clear_vert_flags(struct xn_vert_flags *flags, int n, u8 value)
{
    int k;

    for (k = 0; k < n && k < VERT_FLAGS_MAX; k++)     /* Q-MODEL-06 */
        flags[k].done = value;
}

/* a + b < 0 for the true sum (the asm's last `add; jge`: SF != OF) */
static int sum_negative(s32 a, s32 b)
{
    s32 s = (s32)((u32)a + (u32)b);

    if (((a ^ s) & (b ^ s)) < 0)        /* the add overflowed: the true sum has a's sign */
        return a < 0;
    return s < 0;
}

int xn_model_draw_faces(const struct xn_model *m, struct xn_model_draw_state *s)
{
    const struct xn_model_face *f;
    const xn_vec3 *n, *end;

    xn_model_clear_vert_flags(xn_vert_flags, m->point_count, 0);
    s->points = (xn_vec3 *)POINTS(m);
    xn_render_poly_count += m->face_count;
    n = NORMALS(m);
    end = n + m->face_count;
    f = FACES(m);
    do {                                /* Quirk Q-MODEL-02: 0 faces runs on */
        /* a front face: the eye on the normal's side of the plane (Quirk Q-MODEL-05: the
           last add is compared without its wrap) */
        s32 a = n->x * s->rel.x + f->points[1].plane_d;
        s32 b = n->z * s->rel.z + n->y * s->rel.y;

        if (sum_negative(a, b)) {
            u32 codes = xn_model_transform_face_verts(f, s);

            if ((codes & 0xFF) == 0) {  /* not wholly outside one plane */
                struct xn_poly *poly = xn_render_poly_next;
                const struct xn_vert_cam *c;
                struct xn_tex_entry *e;
                xn_s64 t;
                u32 tex;

                /* the face's 1/z gradient: n . depth row << 13 / d, 64-bit */
                xn_s64_set(&t, n->x * s->dz_row.x + n->z * s->dz_row.z + n->y * s->dz_row.y);
                xn_s64_shl(&t, 13);
                poly->inv_z_step = xn_s64_div_or0(&t, (s32)((u32)a + (u32)b));
                poly->normal = (int *)n;
                poly->inv_z_dx = poly->inv_z_step >> 3;
                xn_span_dzdx = poly->inv_z_dx;
                if (xn_poly_project_face(f, codes)) {
                    xn_render_poly_next++;
                    poly->face = (struct xn_model_face *)f;
                    poly->handle = s->handle;
                    c = &XN_AT(struct xn_vert_cam, xn_vert_cam, f->points[0].vertex);
                    poly->cam_x = c->x;
                    poly->cam_y = c->y;
                    poly->cam_z = c->z;
                    /* (the vertex offset's high word stays above the texture word) */
                    tex = ((u32)f->points[0].vertex & 0xFFFF0000) | f->texture;
                    e = xn_tex_cache_lookup(tex >> 7, tex & 0x7F, -1);
                    if (e == 0)
                        return 1;
                    poly->tex = e;
                    poly->span_fn = xn_render_span_setup(e->kind);
                }
            }
        }
        f = NEXT_FACE(f);
        n++;
    } while (n != end);
    return 0;
}

u32 xn_model_transform_face_verts(const struct xn_model_face *face,
                                  const struct xn_model_draw_state *s)
{
    const struct xn_model_face_point *p = face->points;
    u32 n = face->point_count;
    u8 code_and = 0xFF, code_or = 0;

    do {                                /* Quirk Q-MODEL-02 */
        s32 vi = p->vertex;
        struct xn_vert_flags *fl = &XN_AT(struct xn_vert_flags, xn_vert_flags, vi);
        u8 code;

        if (fl->done & 1) {
            code = fl->outcode;
        } else {
            struct xn_vert_cam *c = &XN_AT(struct xn_vert_cam, xn_vert_cam, vi);
            const xn_vec3 *pt = POINT_AT(s->points, vi);
            xn_vec3 v;

            fl->done = 1;
            v.x = pt->x + s->rel.x;
            v.y = pt->y + s->rel.y;
            v.z = pt->z + s->rel.z;
            xn_mat_transform(&v, (const xn_mat3 *)s->matrix);
            c->x = v.x;
            c->y = v.y;
            c->z = v.z;
            code = (u8)xn_poly_outcode(v.x, v.y, v.z);
            fl->outcode = code;
            if (code == 0) {            /* inside: projected now */
                struct xn_vert_screen *sc = &XN_AT(struct xn_vert_screen, xn_vert_screen, vi);
                u32 inv_z;

                xn_cam_screen_point(v.x, v.y, v.z, &sc->sx, &sc->sy, &inv_z);
                sc->inv_z = inv_z;
            }
        }
        code_or |= code;
        code_and &= code;
        p++;
    } while (--n != 0);
    return code_and | (u32)code_or << 8;
}

void xn_model_build_light_list(struct xn_model_handle *h)
{
    struct xn_light_ref *ref;
    const struct xn_light *light;
    s32 r2;
    xn_s64 t;

    xn_s64_mul(&t, h->model->radius, h->model->radius);     /* radius^2 >> 16, rounded */
    xn_s64_addu(&t, 0x8000);
    r2 = xn_s64_shr(&t, 16);
    ref = xn_render_light_list_next;
    h->lights = ref;
    for (light = xn_light_table; light->intensity > 0;
         light = (const struct xn_light *)((const u8 *)light + 29)) {
        xn_vec3 v;

        v.x = light->x;
        v.y = light->y;
        v.z = light->z;
        if (light->type != 8) {         /* a point light: in range of the model's sphere? */
            s32 dx = v.x - h->x;
            s32 dy = v.y - h->y;
            s32 dz = v.z - h->z;

            if (dx * dx + dy * dy + dz * dz - r2 - light->range_sq >= 0)
                continue;
            v.x = dx << 8;
            v.y = dy << 8;
            v.z = dz << 8;
        }
        xn_mat_transform_transposed(&v, &xn_model_rot_matrix);
        ref->light = (struct xn_light *)light;
        ref->x = v.x;
        ref->y = v.y;
        ref->z = v.z;
        ref->intensity = light->intensity;
        ref->range_sq = light->range_sq << 4;
        ref++;
    }
    /* the end mark; the next list starts after it */
    *(s32 *)ref = -1;
    xn_render_light_list_next = (struct xn_light_ref *)((u8 *)ref + 4);
}

void xn_model_scale_matrix(struct xn_model_matrix_slot *slot)
{
    s32 *m = &slot->m[0][0];
    s32 *light = (s32 *)((u8 *)m + sizeof(((struct xn_model_matrix_pool *)0)->view));
    s32 focal_x = xn_udiv64_or0(0x10, 0, xn_cam_focal_x);      /* 2^36 / focal_x */
    s32 focal_y = xn_udiv64_or0(0x2, 0, xn_cam_focal_y);       /* 2^33 / focal_y */
    int k;

    for (k = 0; k < 3; k++) {
        light[k] = xn_mulhi(m[k], xn_cam_inv_scale_x) * 2;
        m[k] = xn_mulhi(light[k], focal_x);
    }
    for (k = 3; k < 6; k++) {
        light[k] = xn_mulhi(m[k], xn_cam_inv_scale_y) * 2;
        m[k] = xn_mulhi(light[k], focal_y);
    }
    for (k = 6; k < 9; k++) {
        light[k] = m[k];
        m[k] <<= 1;
    }
}

int xn_model_is_occluded(const struct xn_model *m, const struct xn_model_handle *h)
{
    s32 x0, y0, x1, y1, inv_z;
    xn_vec3 c;
    xn_line r;
    struct xn_span *row;
    u32 rows;

    if (m->face_count <= 4)
        return 0;
    if (!xn_model_project_bounds(m, h, &x0, &y0, &x1, &y1, &inv_z, &c))
        return 0;
    r.x1 = x0;
    r.y1 = y0;
    r.x2 = x1;
    r.y2 = y1;
    if (!xn_draw_clip_rect_xyxy(&r))
        return 0;
    rows = r.y2 - r.y1;
    if (rows >= (u32)xn_gfx_height || (u32)(r.x2 - r.x1) >= (u32)xn_gfx_width)
        return 0;
    row = &xn_render_span_rows[r.y1];
    do {                                /* every row of the bounds covered by nearer spans */
        const struct xn_span *node = row;
        s32 x = r.x1;

        for (;;) {
            s32 z;

            node = node->next;
            if ((s16)node->x_end <= (s16)x)
                continue;
            /* Quirk Q-MODEL-04: `sub ax, x_start` offsets only the low word of x */
            z = (x & 0xFFFF0000) | (u16)(x - node->x_start);
            if ((u16)z != 0) {
                if ((s16)z < 0)
                    return 0;           /* a gap before the span */
                z *= node->poly->inv_z_dx;
            }
            if (z + node->inv_z < inv_z)
                return 0;               /* the span is farther than the model's centre */
            x = node->x_end;
            if (x >= r.x2)
                break;
        }
        row++;
    } while (--rows != 0);              /* Quirk Q-MODEL-02 */
    return 1;
}

int xn_model_project_bounds(const struct xn_model *m, const struct xn_model_handle *h,
                            s32 *x0, s32 *y0, s32 *x1, s32 *y1, s32 *inv_z, xn_vec3 *c)
{
    s32 r;

    c->x = (h->x - xn_cam_x) << 8;
    c->y = (h->y - xn_cam_y) << 8;
    c->z = (h->z - xn_cam_z) << 8;
    xn_mat_transform(c, &xn_cam_rotation);
    if (c->z <= xn_cam_near_z)
        return 0;
    r = m->radius;
    xn_cam_project(c->x - r, c->y - r, c->z, x0, y0);
    *x0 += xn_cam_centre_x;
    *y0 += xn_cam_centre_y;
    xn_cam_project(c->x + r, c->y + r, c->z, x1, y1);
    *x1 += xn_cam_centre_x;
    *y1 += xn_cam_centre_y;
    *inv_z = xn_udiv64_or0(0x100, 0, c->z);
    return 1;
}
