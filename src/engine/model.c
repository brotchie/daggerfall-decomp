/* model.c: XnGine's ARCH3D models as readable C (xmodel.h; see xngine.h and
   docs/xngine_readable.md). */
#include "xmodel.h"
#include "xrender.h"
#include "xpoly.h"
#include "xtex.h"
#include "xcam.h"
#include "xvec.h"
#include "xmat.h"
#include "xmath.h"
#include "xdraw.h"

extern s32 xn_model_angle_or_bits;
extern xn_mat3 xn_model_base_matrix, xn_model_object_matrix, xn_model_combined_matrix;
extern xn_mat3 xn_model_rot_matrix;             /* the model being drawn */
extern u8 *player_object;                       /* x, y, z ints at +7, +0Bh, +0Fh */
extern xn_vec3 xn_scratch_vec_b;
extern s32 xn_model_uv_len2, xn_model_uv_t;
extern s32 xn_model_drawn_count;
extern s32 xn_model_queue_count;
extern struct xn_sort_pair *xn_model_queue_ptr;
extern struct xn_vert_cam xn_vert_cam[1024];   /* indexed by byte offset (index * 12) */
extern struct xn_vert_screen xn_vert_screen[1024];
extern struct xn_vert_flags xn_vert_flags[1024];
extern s32 xn_span_dzdx;
extern xn_routine xn_render_span_setups[5];
extern struct xn_light xn_light_table[33];
extern s32 xn_model_bounds_x0, xn_model_bounds_y0, xn_model_bounds_x1, xn_model_bounds_y1;
extern char xn_model_msg_too_many_verts[], xn_model_msg_corrupted[];

/* patch fields (smc/model.md) */
extern struct xn_model_handle *xn_model_draw_handle;   /* 140497, read as data by 140606 */
extern s32 xn_model_eye_bf_x, xn_model_eye_bf_y, xn_model_eye_bf_z;   /* back-face test */
extern s32 xn_model_eye_x, xn_model_eye_y, xn_model_eye_z;            /* added to vertices */
extern s32 xn_model_depth_x, xn_model_depth_y, xn_model_depth_z;      /* 1/z gradient row */
extern s32 xn_model_depth_scale;                       /* 14030A (12A3AC) */
extern xn_mat3 *xn_model_matrix_slot;                  /* 14053E */
extern u8 *xn_model_points_x, *xn_model_points_y, *xn_model_points_z;  /* 14051C/522/528 */
extern xn_vec3 *xn_model_faces_end;                    /* 1404EB: the normals' end */
extern s32 xn_model_vert_sx, xn_model_vert_cx, xn_model_vert_sy, xn_model_vert_cy;
extern s32 xn_model_light_pos_x, xn_model_light_pos_y, xn_model_light_pos_z; /* 140666.. */
extern s32 xn_model_light_r2;                          /* 14068B */
extern s32 xn_model_scale_inv_x, xn_model_scale_inv_y;     /* 1406DD 14072A (12A3AC) */
extern s32 xn_model_scale_focal_x, xn_model_scale_focal_y; /* 1406E2 14072F (12A274) */

/* other groups' functions, through their asm entries */
extern void asm_xn_kbd_remove(void);
extern void asm_xn_joy_shutdown(void);
void xn_gfx_restore_mode(void);
extern void asm_xn_mem_shutdown(void);
extern void asm_xn_model_clear_vert_flags(void);    /* the body the asm plants its ret in */

/* the shared scratch vector a (struct xn_scratch at 0x120288): xn_pick_view_x, _y and
   pick_distance */
#define SCRATCH_A ((xn_vec3 *)&xn_pick_view_x)

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

/* The models' fatal exit (13FEC3, 13FEEB): the engine shut down, the message printed, the
   program ended; the too-many-vertices exit does not shut the joystick down */
static void fatal(char *msg, int joy)
{
    xn_regs r;

    xn_call_asm(asm_xn_kbd_remove);
    if (joy)
        xn_call_asm(asm_xn_joy_shutdown);
    xn_render_shutdown();
    xn_gfx_restore_mode();
    xn_call_asm(asm_xn_mem_shutdown);
    r.eax = 0x0900;
    r.edx = (u32)msg;
    r.ecx = r.ebx = r.ebp = r.esi = r.edi = 0;
    xn_int21(&r);
    r.eax = 0x4C00;
    xn_int21(&r);
}

/* ---- angles ---------------------------------------------------------------------------------- */

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

/* the sum of the model's vertices into *sum; their count (dec; jne: 0 runs 2^32 times) */
static void sum_points(const struct xn_model *m, xn_vec3 *sum)
{
    const xn_vec3 *p = POINTS(m);
    u32 n = m->point_count;

    sum->x = sum->y = sum->z = 0;
    do {
        sum->x += p->x;
        sum->y += p->y;
        sum->z += p->z;
        p++;
    } while (--n != 0);
}

/* v / d, the asm's cdq; idiv */
static s32 sdiv(s32 v, s32 d)
{
    xn_s64 t;

    xn_s64_set(&t, v);
    return xn_s64_div(&t, d);
}

void xn_model_centroid_to_pick(const struct xn_model_handle *h)
{
    xn_vec3 sum;

    sum_points(h->model, &sum);
    xn_pick_view_x = sdiv(sum.x, h->model->point_count);
    xn_pick_view_y = sdiv(sum.y, h->model->point_count);
    pick_distance = sdiv(sum.z, h->model->point_count);
}

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

    do {
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
    s32 x0 = -100000, x1 = 100000, z0 = -100000, z1 = 100000;  /* sic: min and max swapped */

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

/* ---- preparing a model ------------------------------------------------------------------------ */

void xn_model_prepare(struct xn_model *m)
{
    struct xn_model_face *f;
    u32 nf;

    if (m->version < 0x362E3276) {      /* before "v2.6": vertex offsets are index * 4 */
        if ((u32)m->point_count >= 0x400) {
            fatal(xn_model_msg_too_many_verts, 0);
            return;
        }
        f = FACES(m);
        nf = m->face_count;
        do {                            /* loop: counts of 0 run 2^32 times */
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
            return;
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
                return;
            }
            p++;
        } while (--np != 0);
        f = (struct xn_model_face *)p;
    } while (--nf != 0);
}

void xn_model_prepare_r(xn_regs *r)
{
    xn_model_prepare((struct xn_model *)r->eax);
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

/* the 64-bit sum of squares of v, >> 8 (bits 8..39) */
static s32 len2(const xn_vec3 *v)
{
    xn_s64 t;

    xn_s64_mul(&t, v->x, v->x);
    xn_s64_mac(&t, v->y, v->y);
    xn_s64_mac(&t, v->z, v->z);
    return xn_s64_shr(&t, 8);
}

/* (v << 25) / d, 64-bit */
static s32 scale25(s32 v, s32 d)
{
    xn_s64 t;

    xn_s64_set(&t, v);
    xn_s64_shl(&t, 25);
    return xn_s64_div(&t, d);
}

/* one texture axis: d1 e1 + d2' e2, d2' = d2 - (d1 t >> 16); 32-bit products */
static void uv_axis(s32 d1, s32 d2, const xn_vec3 *e1, const xn_vec3 *e2, xn_vec3 *out)
{
    d2 -= xn_mulshr(d1, xn_model_uv_t, 16);
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
    xn_vec3 *e1 = SCRATCH_A;
    xn_vec3 *e2 = &xn_scratch_vec_b;
    xn_s64 t;
    s32 l2;

    e1->x = p1->x - p0->x;
    e1->y = p1->y - p0->y;
    e1->z = p1->z - p0->z;
    l2 = len2(e1);
    if (l2 == 0)
        return;
    xn_model_uv_len2 = l2;
    e2->x = p2->x - p1->x;
    e2->y = p2->y - p1->y;
    e2->z = p2->z - p1->z;
    /* t = (e1 . e2 << 8) / |e1|^2: e2's part along e1, 16.16 */
    xn_s64_mul(&t, e1->x, e2->x);
    xn_s64_mac(&t, e1->y, e2->y);
    xn_s64_mac(&t, e1->z, e2->z);
    xn_s64_shl(&t, 8);
    xn_model_uv_t = xn_s64_div(&t, xn_model_uv_len2);
    e2->x -= xn_mulshr(e1->x, xn_model_uv_t, 16);
    e2->y -= xn_mulshr(e1->y, xn_model_uv_t, 16);
    e2->z -= xn_mulshr(e1->z, xn_model_uv_t, 16);
    e1->x = scale25(e1->x, xn_model_uv_len2);
    e1->y = scale25(e1->y, xn_model_uv_len2);
    e1->z = scale25(e1->z, xn_model_uv_len2);
    l2 = len2(e2);
    if (l2 == 0)
        return;
    e2->x = scale25(e2->x, l2);
    e2->y = scale25(e2->y, l2);
    e2->z = scale25(e2->z, l2);
    /* the file's u/v deltas of points 1 and 2 */
    uv_axis(face->points[1].uv[0], face->points[2].uv[0], e1, e2, &out->u_axis);
    uv_axis(face->points[1].uv[1], face->points[2].uv[1], e1, e2, &out->v_axis);
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
    xn_vec3 *e1 = SCRATCH_A;
    xn_vec3 *e2 = &xn_scratch_vec_b;
    xn_vec3 n;
    s32 shift;

    e1->x = p1->x - p0->x;
    e1->y = p1->y - p0->y;
    e1->z = p1->z - p0->z;
    e2->x = p2->x - p1->x;
    e2->y = p2->y - p1->y;
    e2->z = p2->z - p1->z;
    xn_vec_cross(e1, e2, &n);
    xn_vec_normalize(&n);
    shift = 16 - bits;
    if (shift > 0) {
        n.x >>= shift & 31;
        n.y >>= shift & 31;
        n.z >>= shift & 31;
    }
    *out = n;
}

void xn_model_calc_centroid(const struct xn_model *m)
{
    xn_vec3 sum;

    sum_points(m, &sum);
    xn_pick_view_x = sdiv(sum.x, m->point_count);
    xn_pick_view_y = sdiv(sum.y, m->point_count);
    pick_distance = sdiv(sum.z, m->point_count);
}

/* ---- the queue ----------------------------------------------------------------------------- */

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
    /* the key: the distance (the culling left the centre in view space) less the radius */
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
    const struct xn_model *m = h->model;
    xn_mat3 *slot;
    xn_vec3 eye;

    if (xn_model_is_occluded(m, h)) {
        h->flags |= 1;
        return 0;
    }
    xn_model_drawn_count++;
    xn_model_draw_handle = h;
    xn_mat_from_angles(h->angle_x, h->yaw, h->angle_z, &xn_model_rot_matrix);
    slot = (xn_mat3 *)xn_render_matrix_next;
    xn_mat_multiply(&xn_cam_view_matrix, &xn_model_rot_matrix, slot);
    /* the eye in object space (the object's origin seen from the eye) */
    eye.x = h->rel_x;
    eye.y = h->rel_y;
    eye.z = h->rel_z;
    xn_mat_transform_transposed(&eye, &xn_model_rot_matrix);
    h->rel_x = eye.x;
    h->rel_y = eye.y;
    h->rel_z = eye.z;
    xn_model_eye_bf_x = xn_model_eye_x = eye.x;
    xn_model_eye_bf_y = xn_model_eye_y = eye.y;
    xn_model_eye_bf_z = xn_model_eye_z = eye.z;
    /* d(1/z)/dx per unit of a face's plane: the matrix's first row times the depth scale */
    xn_model_depth_x = xn_mulhi(slot->m[0][0], xn_model_depth_scale);
    xn_model_depth_y = xn_mulhi(slot->m[0][1], xn_model_depth_scale);
    xn_model_depth_z = xn_mulhi(slot->m[0][2], xn_model_depth_scale);
    h->matrix = (struct xn_model_matrix_slot *)slot;
    xn_model_matrix_slot = slot;
    if (m->frame_count != 0)
        xn_model_set_frame((struct xn_model *)m, h->frame);
    if (xn_model_draw_faces(m))
        return 1;
    xn_model_build_light_list();
    xn_model_scale_matrix();
    xn_render_matrix_next++;
    return 0;
}

void xn_model_draw_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_model_draw((struct xn_model_handle *)r->edi));
}

/* the frame's lists into the model's header (frame clamped to the frames), then the planes */
static void set_frame(struct xn_model *m, s32 frame, s32 count)
{
    const struct xn_model_frame *fr;

    count--;
    if (frame < 0)
        frame = 0;
    else if (frame > count)
        frame = count;
    fr = (const struct xn_model_frame *)((u8 *)m + m->frame_table_offset + frame * 16);
    m->point_offset = fr->point_offset;
    m->normal_offset = fr->normal_offset;
    m->face_data_offset = fr->face_data_offset;
    xn_model_calc_face_planes(m);
}

void xn_model_set_frame(struct xn_model *m, s32 frame)
{
    set_frame(m, frame, m->frame_count);
}

void xn_model_set_frame_regs_r(xn_regs *r)
{
    struct xn_model *m = (struct xn_model *)r->esi;

    set_frame(m, r->eax, r->ebx);
    r->eax = (u32)m;
    r->edx = m->face_data_offset;
}

void xn_model_set_frame_r(xn_regs *r)
{
    struct xn_model *m = (struct xn_model *)r->eax;

    xn_model_set_frame(m, r->edx);
    r->edx = m->face_data_offset;
}

int xn_model_draw_faces(const struct xn_model *m)
{
    const struct xn_model_face *f;
    const xn_vec3 *n;

    xn_model_clear_vert_flags(xn_vert_flags, m->point_count, 0);
    XN_KEEP(xn_model_points_x, (u8 *)m + m->point_offset);
    XN_KEEP(xn_model_points_y, xn_model_points_x + 4);
    XN_KEEP(xn_model_points_z, xn_model_points_x + 8);
    xn_render_poly_count += m->face_count;
    n = NORMALS(m);
    XN_KEEP(xn_model_faces_end, (xn_vec3 *)n + m->face_count);
    f = FACES(m);
    do {
        /* a front face: the eye on the normal's side of the plane */
        s32 d = n->x * xn_model_eye_bf_x + n->y * xn_model_eye_bf_y +
                n->z * xn_model_eye_bf_z + f->points[1].plane_d;

        if (d < 0) {
            u32 codes = xn_model_transform_face_verts(f);

            if ((codes & 0xFF) == 0) {  /* not wholly outside one plane */
                struct xn_poly *poly = xn_render_poly_next;
                const struct xn_vert_cam *c;
                struct xn_tex_entry *e;
                xn_s64 t;
                s32 frame = -1;
                u32 tex;

                /* the face's 1/z gradient: n . depth row << 13 / d, 64-bit */
                xn_s64_set(&t, n->x * xn_model_depth_x + n->z * xn_model_depth_z +
                               n->y * xn_model_depth_y);
                xn_s64_shl(&t, 13);
                poly->inv_z_step = xn_s64_div(&t, d);
                poly->normal = (int *)n;
                poly->inv_z_dx = poly->inv_z_step >> 3;
                xn_span_dzdx = poly->inv_z_dx;
                if (xn_poly_project_face(f, codes)) {
                    xn_render_poly_next++;
                    poly->face = (struct xn_model_face *)f;
                    poly->handle = xn_model_draw_handle;
                    c = &XN_AT(struct xn_vert_cam, xn_vert_cam, f->points[0].vertex);
                    poly->cam_x = c->x;
                    poly->cam_y = c->y;
                    poly->cam_z = c->z;
                    /* (the vertex offset's high word stays above the texture word) */
                    tex = ((u32)f->points[0].vertex & 0xFFFF0000) | f->texture;
                    e = xn_tex_cache_lookup(tex >> 7, tex & 0x7F, &frame);
                    if (e == 0)
                        return 1;
                    poly->tex = e;
                    poly->span_fn = XN_AT(xn_routine, xn_render_span_setups, e->kind);
                }
            }
        }
        f = NEXT_FACE(f);
        n++;
    } while (n != xn_model_faces_end);
    return 0;
}

void xn_model_draw_faces_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_model_draw_faces((const struct xn_model *)r->esi));
}

u32 xn_model_transform_face_verts(const struct xn_model_face *face)
{
    const struct xn_model_face_point *p = face->points;
    u32 n = face->point_count;
    u8 code_and = 0xFF, code_or = 0;

    do {
        s32 vi = p->vertex;
        struct xn_vert_flags *fl = &XN_AT(struct xn_vert_flags, xn_vert_flags, vi);
        u8 code;

        if (fl->done & 1) {
            code = fl->outcode;
        } else {
            struct xn_vert_cam *c = &XN_AT(struct xn_vert_cam, xn_vert_cam, vi);
            xn_vec3 v;

            fl->done = 1;
            v.x = *(s32 *)(xn_model_points_x + vi) + xn_model_eye_x;
            v.y = *(s32 *)(xn_model_points_y + vi) + xn_model_eye_y;
            v.z = *(s32 *)(xn_model_points_z + vi) + xn_model_eye_z;
            xn_mat_transform(&v, xn_model_matrix_slot);
            c->x = v.x;
            c->y = v.y;
            c->z = v.z;
            code = (u8)xn_poly_outcode(v.x, v.y, v.z);
            fl->outcode = code;
            if (code == 0) {            /* inside: projected now */
                struct xn_vert_screen *s = &XN_AT(struct xn_vert_screen, xn_vert_screen, vi);
                u32 inv_z = xn_udiv64(0x100, 0, v.z);

                s->inv_z = inv_z;
                s->sx = (u32)(xn_mulhi(v.x * xn_model_vert_sx, inv_z) + xn_model_vert_cx) >> 3;
                s->sy = (u32)(xn_mulhi(v.y * xn_model_vert_sy, inv_z) + xn_model_vert_cy) >> 8;
            }
        }
        code_or |= code;
        code_and &= code;
        p++;
    } while (--n != 0);
    return code_and | (u32)code_or << 8;
}

void xn_model_transform_face_verts_r(xn_regs *r)
{
    r->ebx = xn_model_transform_face_verts((const struct xn_model_face *)r->esi);
}

void xn_model_build_light_list(void)
{
    struct xn_model_handle *h = xn_model_draw_handle;  /* the patch field, read as data */
    struct xn_light_ref *ref;
    const struct xn_light *light;
    xn_s64 t;

    XN_KEEP(xn_model_light_pos_x, h->x);
    XN_KEEP(xn_model_light_pos_y, h->y);
    XN_KEEP(xn_model_light_pos_z, h->z);
    xn_s64_mul(&t, h->model->radius, h->model->radius);     /* radius^2 >> 16, rounded */
    xn_s64_addu(&t, 0x8000);
    XN_KEEP(xn_model_light_r2, xn_s64_shr(&t, 16));
    ref = xn_render_light_list_next;
    h->lights = ref;
    for (light = xn_light_table; light->intensity > 0;
         light = (const struct xn_light *)((const u8 *)light + 29)) {
        xn_vec3 v;

        v.x = light->x;
        v.y = light->y;
        v.z = light->z;
        if (light->type != 8) {         /* a point light: in range of the model's sphere? */
            s32 dx = v.x - xn_model_light_pos_x;
            s32 dy = v.y - xn_model_light_pos_y;
            s32 dz = v.z - xn_model_light_pos_z;

            if (dx * dx + dy * dy + dz * dz - xn_model_light_r2 - light->range_sq >= 0)
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
    /* the end mark; the next list starts 4 bytes on (the asm's stosd) */
    *(s32 *)ref = -1;
    xn_render_light_list_next = (struct xn_light_ref *)((u8 *)ref + 4);
}

void xn_model_scale_matrix(void)
{
    s32 *m = &xn_render_matrix_next->m[0][0];
    s32 *light = (s32 *)((u8 *)m + 0x1C20);     /* the slot in the pool's light half */
    int k;

    for (k = 0; k < 3; k++) {
        light[k] = xn_mulhi(m[k], xn_model_scale_inv_x) * 2;
        m[k] = xn_mulhi(light[k], xn_model_scale_focal_x);
    }
    for (k = 3; k < 6; k++) {
        light[k] = xn_mulhi(m[k], xn_model_scale_inv_y) * 2;
        m[k] = xn_mulhi(light[k], xn_model_scale_focal_y);
    }
    for (k = 6; k < 9; k++) {
        light[k] = m[k];
        m[k] <<= 1;
    }
}

void xn_model_scale_matrix_r(xn_regs *r)
{
    xn_model_scale_matrix();
    r->edx = xn_render_matrix_next->m[1][2];
}

/* xn_draw_clip_rect_xyxy (draw group, xdraw.h): the rectangle clipped to the view; 0 when empty */
static int clip_rect(s32 *x0, s32 *y0, s32 *x1, s32 *y1)
{
    xn_line r;
    int ok;

    r.x1 = *x0;
    r.y1 = *y0;
    r.x2 = *x1;
    r.y2 = *y1;
    ok = xn_draw_clip_rect_xyxy(&r);
    *x0 = r.x1;
    *y0 = r.y1;
    *x1 = r.x2;
    *y1 = r.y2;
    return ok;
}

int xn_model_is_occluded(const struct xn_model *m, const struct xn_model_handle *h)
{
    s32 x0, y0, x1, y1, inv_z;
    xn_vec3 c;
    struct xn_span *row;
    u32 rows;

    if (m->face_count <= 4)
        return 0;
    if (!xn_model_project_bounds(m, h, &x0, &y0, &x1, &y1, &inv_z, &c))
        return 0;
    if (!clip_rect(&x0, &y0, &x1, &y1))
        return 0;
    rows = y1 - y0;
    if (rows >= (u32)xn_gfx_height || (u32)(x1 - x0) >= (u32)xn_gfx_width)
        return 0;
    row = &xn_render_span_rows[y0];
    do {                                /* every row of the bounds covered by nearer spans */
        const struct xn_span *node = row;
        s32 x = x0;

        for (;;) {
            s32 z;

            node = node->next;
            if ((s16)node->x_end <= (s16)x)
                continue;
            /* `sub ax, x_start`: only the low word of x */
            z = (x & 0xFFFF0000) | (u16)(x - node->x_start);
            if ((u16)z != 0) {
                if ((s16)z < 0)
                    return 0;           /* a gap before the span */
                z *= node->poly->inv_z_dx;
            }
            if (z + node->inv_z < inv_z)
                return 0;               /* the span is farther than the model's centre */
            x = node->x_end;
            if (x >= x1)
                break;
        }
        row++;
    } while (--rows != 0);
    return 1;
}

void xn_model_is_occluded_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_model_is_occluded((const struct xn_model *)r->esi,
                                              (const struct xn_model_handle *)r->edi));
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
    xn_cam_project(c->x - r, c->y - r, c->z, &xn_model_bounds_x0, &xn_model_bounds_y0);
    xn_model_bounds_x0 += xn_cam_centre_x;
    xn_model_bounds_y0 += xn_cam_centre_y;
    xn_cam_project(c->x + r, c->y + r, c->z, &xn_model_bounds_x1, &xn_model_bounds_y1);
    xn_model_bounds_x1 += xn_cam_centre_x;
    xn_model_bounds_y1 += xn_cam_centre_y;
    *inv_z = xn_udiv64(0x100, 0, c->z);
    *x0 = xn_model_bounds_x0;
    *y0 = xn_model_bounds_y0;
    *x1 = xn_model_bounds_x1;
    *y1 = xn_model_bounds_y1;
    return 1;
}

void xn_model_project_bounds_r(xn_regs *r)
{
    s32 x0, y0, x1, y1, inv_z;
    xn_vec3 c;

    if (xn_model_project_bounds((const struct xn_model *)r->esi,
                                (const struct xn_model_handle *)r->edi,
                                &x0, &y0, &x1, &y1, &inv_z, &c)) {
        r->eax = x0;
        r->edx = y0;
        r->ebx = x1;
        r->ecx = y1;
        r->edi = inv_z;
        XN_SETFLAG(r, XN_CF, 0);
    } else {                            /* the asm leaves the centre and the matrix */
        r->eax = c.x;
        r->edx = c.y;
        r->ebx = c.z;
        r->ecx = (u32)&xn_cam_rotation;
        XN_SETFLAG(r, XN_CF, 1);
    }
}

void xn_model_clear_vert_flags(struct xn_vert_flags *flags, int n, u8 value)
{
    int k;

    for (k = 0; k < n; k++)
        flags[k].done = value;
}

/* the count a planted ret gives a body of `max` steps of `stride` bytes (or `max` if none) */
static int planted_count(const u8 *body, int stride, int max)
{
    int n;

    for (n = 0; n < max; n++)
        if (body[n * stride] == 0xC3)
            return n;
    return max;
}

void xn_model_clear_vert_flags_r(xn_regs *r)
{
    /* the asm body's own entry: the planted ret is the count; edi + 100h the flags */
    xn_model_clear_vert_flags((struct xn_vert_flags *)(r->edi + 0x100),
                              planted_count((const u8 *)asm_xn_model_clear_vert_flags, 6, 1024),
                              (u8)r->eax);
}
