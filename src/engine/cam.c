/* cam.c: XnGine's camera functions as readable C (xcam.h; see xngine.h and
   docs/xngine_readable.md). */
#include "xcam.h"
#include "xvec.h"
#include "xmat.h"

extern s32 xn_cam_forward_x, xn_cam_forward_y, xn_cam_forward_z;
extern s32 xn_cam_dir_x_table[1024];    /* (k << 18) / focal_x for k = -512..511 */
extern s32 xn_cam_dir_y_table[768];     /* (k << 18) / focal_y for k = -384..383 */
extern u8 xn_poly_clip_outcode_or;

/* patch fields (smc/render.md): the model scale's constants */
extern s32 xn_model_scale_focal_x, xn_model_scale_focal_y;     /* 1406E2 14072F */
extern s32 xn_model_scale_inv_x, xn_model_scale_inv_y;         /* 1406DD 14072A */
extern s32 xn_model_depth_scale;                               /* 14030A */
extern s32 xn_span_texlit_ray_dx16, xn_span_solid_lit_ray_dx16; /* 156B74 155996 */

/* the view window's fields: half width and height */
extern s32 xn_poly_flat_sx, xn_poly_face_sx, xn_poly_terrain_sx;
extern s32 xn_poly_flat_sy, xn_poly_face_sy, xn_poly_terrain_sy;
extern s32 xn_model_vert_sx, xn_model_vert_sy, xn_tgrid_sx, xn_tgrid_sy;
extern s32 xn_poly_setup_sx, xn_poly_setup_sy, xn_flat_grad_sx, xn_flat_grad_sy;
/* the centre */
extern s32 xn_render_frame_centre_x;
extern s32 xn_span_flat_transparent_cx, xn_span_flat_shaded_cx;
extern s32 xn_span_flat_fogged_cx, xn_span_flat_translucent_cx;
extern s32 xn_poly_flat_cx, xn_poly_flat_cy, xn_poly_face_cx, xn_poly_face_cy;
extern s32 xn_model_vert_cx, xn_model_vert_cy, xn_tgrid_cx, xn_tgrid_cy;
extern s32 xn_poly_terrain_cx, xn_poly_terrain_cy;

void xn_cam_forward_angles(s32 *pitch, s32 *yaw)
{
    s32 y;

    *pitch = xn_vec_dir_to_angles_regs(xn_cam_forward_x, xn_cam_forward_y, xn_cam_forward_z, &y);
    *yaw = y;
}

void xn_cam_set_focal(s32 focal_x, s32 focal_y)
{
    xn_cam_focal_x = focal_x;
    xn_cam_focal_y = focal_y;
    xn_cam_inv_focal_x = xn_udiv64(0x40, 0, focal_x);          /* 2^38 / f */
    xn_cam_inv_focal_y = xn_udiv64(0x40, 0, focal_y);
    xn_model_scale_focal_x = xn_udiv64(0x10, 0, focal_x);      /* 2^36 / focal_x */
    xn_model_scale_focal_y = xn_udiv64(0x2, 0, focal_y);       /* 2^33 / focal_y */
    xn_cam_update_derived();
}

void xn_cam_set_view_window(s32 half_w, s32 half_h, s32 centre_x, s32 centre_y)
{
    s32 cx = (centre_x << 8) + 0x80;    /* the centre in 24.8, plus a half for rounding */
    s32 cy = (centre_y << 8) + 0x80;

    xn_cam_half_width = half_w;
    xn_cam_half_height = half_h;
    xn_cam_centre_x = centre_x;
    xn_cam_centre_y = centre_y;
    xn_poly_flat_sx = xn_poly_face_sx = xn_poly_terrain_sx = half_w;
    xn_poly_flat_sy = xn_poly_face_sy = xn_poly_terrain_sy = half_h;
    xn_model_vert_sx = half_w;
    xn_model_vert_sy = half_h;
    xn_tgrid_sx = half_w;
    xn_tgrid_sy = half_h;
    xn_poly_setup_sx = half_w;
    xn_poly_setup_sy = half_h;
    xn_flat_grad_sx = half_w;
    xn_flat_grad_sy = half_h;
    xn_render_frame_centre_x = centre_x;
    xn_span_flat_transparent_cx = centre_x - 1;
    xn_span_flat_shaded_cx = centre_x - 1;
    xn_span_flat_fogged_cx = centre_x - 1;
    xn_span_flat_translucent_cx = centre_x - 1;
    xn_poly_flat_cx = cx;
    xn_poly_flat_cy = cy;
    xn_poly_face_cx = cx;
    xn_poly_face_cy = cy;
    xn_model_vert_cx = cx;
    xn_model_vert_cy = cy;
    xn_tgrid_cx = cx;
    xn_tgrid_cy = cy;
    xn_poly_terrain_cx = cx;
    xn_poly_terrain_cy = cy;
    xn_cam_update_derived();
}

void xn_cam_update_derived(void)
{
    xn_vec3 n;
    s32 k;
    xn_s64 t;

    xn_cam_scale_x = xn_udiv64(0, (u32)xn_cam_focal_x << 14, xn_cam_half_width);
    xn_cam_inv_scale_x = xn_udiv64(0x2000, 0, xn_cam_scale_x);          /* 2^45 / scale */
    xn_model_scale_inv_x = xn_cam_inv_scale_x;
    xn_cam_scale_y = xn_udiv64(0, (u32)xn_cam_focal_y << 14, xn_cam_half_height);
    xn_cam_inv_scale_y = xn_udiv64(0x2000, 0, xn_cam_scale_y);
    xn_model_scale_inv_y = xn_cam_inv_scale_y;
    /* 2^48 / (scale_x * focal_x), the product in 32 bits */
    xn_model_depth_scale = xn_udiv64(0x10000, 0, xn_cam_scale_x * xn_cam_focal_x);
    xn_cam_flat_scale_x = xn_udiv64(xn_cam_half_width, 0, xn_cam_focal_x * xn_cam_focal_x) >> 1;
    xn_cam_flat_scale_y = xn_udiv64(xn_cam_half_height, 0, xn_cam_focal_y * xn_cam_focal_y) >> 1;

    /* the side planes' normals: (focal_x, 0, half_w) and (focal_y, half_h, 0), 16.16 units */
    n.x = xn_cam_focal_x;
    n.y = 0;
    n.z = xn_cam_half_width;
    xn_vec_normalize(&n);
    xn_cam_hfov_cos = n.x;
    xn_cam_hfov_sin = n.z;
    n.x = xn_cam_focal_y;
    n.y = xn_cam_half_height;
    n.z = 0;
    xn_vec_normalize(&n);
    xn_cam_vfov_cos = n.x;
    xn_cam_vfov_sin = n.y;

    /* the direction of each screen column and row: (k << 18) / focal, 64-bit signed */
    for (k = -512; k != 512; k++) {
        xn_s64_set(&t, k);
        xn_s64_shl(&t, 18);
        xn_cam_dir_x_table[k + 512] = xn_s64_div(&t, xn_cam_focal_x);
    }
    for (k = -384; k != 384; k++) {
        xn_s64_set(&t, k);
        xn_s64_shl(&t, 18);
        xn_cam_dir_y_table[k + 384] = xn_s64_div(&t, xn_cam_focal_y);
    }
    xn_span_texlit_ray_dx16 = xn_udiv64(0, 0x400000, xn_cam_focal_x);   /* 2^22 / focal_x */
    xn_span_solid_lit_ray_dx16 = xn_span_texlit_ray_dx16;
}

void xn_cam_scale_matrix_in_place(xn_mat3 *m)
{
    m->m[0][0] = xn_mulshr(m->m[0][0], xn_cam_scale_x, 14);
    m->m[0][1] = xn_mulshr(m->m[0][1], xn_cam_scale_x, 14);
    m->m[0][2] = xn_mulshr(m->m[0][2], xn_cam_scale_x, 14);
    m->m[1][0] = xn_mulshr(m->m[1][0], xn_cam_scale_y, 14);
    m->m[1][1] = xn_mulshr(m->m[1][1], xn_cam_scale_y, 14);
    m->m[2][2] = xn_mulshr(m->m[2][2], xn_cam_scale_y, 14);    /* sic: m[1][2] stays */
}

void xn_cam_project_ptr(s32 x, s32 y, s32 z, s32 *sx, s32 *sy)
{
    s32 px, py;

    xn_cam_project(x, y, z, &px, &py);
    *sx = px;
    *sy = py;
}

void xn_cam_project(s32 x, s32 y, s32 z, s32 *sx, s32 *sy)
{
    *sy = xn_muldiv(y, xn_cam_focal_y, z);
    *sx = xn_muldiv(x, xn_cam_focal_x, z);
}

void xn_cam_project_r(xn_regs *r)
{
    s32 sx, sy;

    xn_cam_project(r->eax, r->edx, r->ebx, &sx, &sy);
    r->eax = sx;
    r->edx = sy;
}

/* (a * b + 0x80) / d with a 64-bit product */
static s32 muldiv_rounded(s32 a, s32 b, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    xn_s64_addu(&t, 0x80);
    return xn_s64_div(&t, d);
}

void xn_cam_project_scaled(s32 x, s32 y, s32 z, s32 *sx, s32 *sy)
{
    *sy = muldiv_rounded(y, xn_cam_half_height, z);
    *sx = muldiv_rounded(x, xn_cam_half_width, z);
}

void xn_cam_project_scaled_r(xn_regs *r)
{
    s32 sx, sy;

    xn_cam_project_scaled(r->eax, r->edx, r->ebx, &sx, &sy);
    r->eax = sx;
    r->edx = sy;
}

void xn_cam_scale_matrix(const xn_mat3 *src, xn_mat3 *dst)
{
    int c;

    for (c = 0; c < 3; c++)
        dst->m[0][c] = xn_mulshr(src->m[0][c], xn_cam_scale_x, 14);
    for (c = 0; c < 3; c++)
        dst->m[1][c] = xn_mulshr(src->m[1][c], xn_cam_scale_y, 14);
    for (c = 0; c < 3; c++)
        dst->m[2][c] = src->m[2][c];
}

/* (v << 14) / d, 64-bit signed */
static s32 unscale(s32 v, s32 d)
{
    xn_s64 t;

    xn_s64_set(&t, v);
    xn_s64_shl(&t, 14);
    return xn_s64_div(&t, d);
}

void xn_cam_unscale_matrix(xn_mat3 *m)
{
    int c;

    for (c = 0; c < 3; c++)
        m->m[0][c] = unscale(m->m[0][c], xn_cam_scale_x);
    for (c = 0; c < 3; c++)
        m->m[1][c] = unscale(m->m[1][c], xn_cam_scale_y);
}

/* a + b <= 0 as `jle` sees it after `add` (the true sum, without wrapping) */
static int sum_not_positive(s32 a, s32 b)
{
    u32 f = xn_add_flags(a, b);

    return (f & XN_ZF) != 0 || ((f & XN_SF) != 0) != ((f & XN_OF) != 0);
}

int xn_cam_cull_sphere(s32 x, s32 y, s32 z, s32 radius, s32 *residue)
{
    xn_vec3 v;
    u8 code;

    v.x = x;
    v.y = y;
    v.z = z;
    xn_mat_transform(&v, &xn_cam_view_matrix);
    /* the centre's outcode: the planes it is outside of */
    code = 0;
    if (v.z < xn_cam_near_z)
        code |= 0x10;
    if (v.z > xn_cam_far_z)
        code |= 0x20;
    if (v.x > v.z)
        code |= 2;
    if (v.y > v.z)
        code |= 4;
    if (v.x < -v.z)
        code |= 1;
    if (v.y < -v.z)
        code |= 8;
    /* the centre unscaled, for the pick */
    xn_pick_view_x = xn_mulhi(v.x * 2, xn_cam_inv_scale_x);
    xn_pick_view_y = xn_mulhi(v.y * 2, xn_cam_inv_scale_y);
    *residue = XN_LOW32(v.y * 2, xn_cam_inv_scale_y);
    pick_distance = v.z;
    if (code == 0)
        return 0;
    xn_poly_clip_outcode_or = code;
    if (code & 0x10) {                          /* before the near plane */
        *residue = xn_cam_near_z - v.z - radius;
        if (*residue >= 0)
            return 1;
    }
    if (code & 0x20) {                          /* beyond the far plane */
        s32 nearest = v.z - radius;

        *residue = nearest - xn_cam_far_z;
        if (nearest >= xn_cam_far_z)
            return 1;
    }
    if ((code & 1) &&
        sum_not_positive(xn_cam_sphere_dist_x(xn_cam_hfov_cos, xn_cam_hfov_sin, residue), radius))
        return 1;
    if ((code & 2) &&
        sum_not_positive(xn_cam_sphere_dist_x(-xn_cam_hfov_cos, xn_cam_hfov_sin, residue), radius))
        return 1;
    if ((code & 4) &&
        sum_not_positive(xn_cam_sphere_dist_y(-xn_cam_vfov_cos, xn_cam_vfov_sin, residue), radius))
        return 1;
    if ((code & 8) &&
        sum_not_positive(xn_cam_sphere_dist_y(xn_cam_vfov_cos, xn_cam_vfov_sin, residue), radius))
        return 1;
    return 0;
}

void xn_cam_cull_sphere_r(xn_regs *r)
{
    s32 residue;
    int culled;

    culled = xn_cam_cull_sphere(r->eax, r->edx, r->ebx, r->ecx, &residue);
    r->eax = residue;
    XN_SETFLAG(r, XN_CF, culled);
}

/* -|n . (view, distance)| >> 16, as the two side-plane tests compute it */
static s32 plane_dist(s32 n_view, s32 view, s32 nz, s32 *low)
{
    s32 d;

    *low = XN_LOW32(nz, pick_distance);
    d = xn_fixmul16(n_view, view) + xn_fixmul16(nz, pick_distance);
    if (d >= 0)
        d = -d;
    return d;
}

s32 xn_cam_sphere_dist_x(s32 nx, s32 nz, s32 *low)
{
    return plane_dist(nx, xn_pick_view_x, nz, low);
}

void xn_cam_sphere_dist_x_r(xn_regs *r)
{
    s32 low, d;

    d = xn_cam_sphere_dist_x(r->eax, r->ebx, &low);
    r->eax = low;
    r->eflags = (r->eflags & ~(u32)(XN_ZF | XN_SF | XN_OF)) | xn_add_flags(d, r->edi);
}

s32 xn_cam_sphere_dist_y(s32 ny, s32 nz, s32 *low)
{
    return plane_dist(ny, xn_pick_view_y, nz, low);
}

void xn_cam_sphere_dist_y_r(xn_regs *r)
{
    s32 low, d;

    d = xn_cam_sphere_dist_y(r->eax, r->ebx, &low);
    r->eax = low;
    r->eflags = (r->eflags & ~(u32)(XN_ZF | XN_SF | XN_OF)) | xn_add_flags(d, r->edi);
}
