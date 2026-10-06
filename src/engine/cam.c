/* cam.c: XnGine's camera (canonical C; the interface and the module's documentation are in
   xcam.h). */
#include "xcam.h"
#include "xvec.h"
#include "xmat.h"

void xn_cam_set_focal(s32 focal_x, s32 focal_y)
{
    xn_cam_focal_x = focal_x;
    xn_cam_focal_y = focal_y;
    xn_cam_inv_focal_x = xn_udiv64_or0(0x40, 0, focal_x);      /* 2^38 / f */
    xn_cam_inv_focal_y = xn_udiv64_or0(0x40, 0, focal_y);
    xn_cam_update_derived();
}

void xn_cam_set_view_window(s32 half_w, s32 half_h, s32 centre_x, s32 centre_y)
{
    xn_cam_half_width = half_w;
    xn_cam_half_height = half_h;
    xn_cam_centre_x = centre_x;
    xn_cam_centre_y = centre_y;
    xn_cam_update_derived();
}

/* (k << 18) / focal for k = first .. first + n - 1, 64-bit signed (0 for a focal of 0) */
static void ray_table(s32 *table, s32 first, s32 n, s32 focal)
{
    s32 k;

    for (k = first; k != first + n; k++) {
        xn_s64 t;

        xn_s64_set(&t, k);
        xn_s64_shl(&t, 18);
        *table++ = xn_s64_div_or0(&t, focal);
    }
}

void xn_cam_update_derived(void)
{
    xn_vec3 n;

    xn_cam_scale_x = xn_udiv64_or0(0, (u32)xn_cam_focal_x << 14, xn_cam_half_width);
    xn_cam_inv_scale_x = xn_udiv64_or0(0x2000, 0, xn_cam_scale_x);     /* 2^45 / scale */
    xn_cam_scale_y = xn_udiv64_or0(0, (u32)xn_cam_focal_y << 14, xn_cam_half_height);
    xn_cam_inv_scale_y = xn_udiv64_or0(0x2000, 0, xn_cam_scale_y);
    /* (half size << 32) / focal^2, halved; the squares are 32-bit */
    xn_cam_flat_scale_x = xn_udiv64_or0(xn_cam_half_width, 0,
                                        xn_cam_focal_x * xn_cam_focal_x) >> 1;
    xn_cam_flat_scale_y = xn_udiv64_or0(xn_cam_half_height, 0,
                                        xn_cam_focal_y * xn_cam_focal_y) >> 1;

    /* the side planes' normals: (focal_x, 0, half_w) and (focal_y, half_h, 0), 16.16 */
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

    /* the ray of each screen column and row */
    ray_table(xn_cam_dir_x_table, -512, 1024, xn_cam_focal_x);
    ray_table(xn_cam_dir_y_table, -384, 768, xn_cam_focal_y);
}

/* ---- projection ---------------------------------------------------------------------------- */

void xn_cam_screen_point(s32 x, s32 y, s32 z, s32 *sx, s32 *sy, u32 *inv_z)
{
    u32 iz = xn_udiv64_or0(0x100, 0, z);

    *inv_z = iz;
    *sx = (u32)(xn_mulhi(x * xn_cam_half_width, iz) + (xn_cam_centre_x << 8) + 0x80) >> 3;
    *sy = (u32)(xn_mulhi(y * xn_cam_half_height, iz) + (xn_cam_centre_y << 8) + 0x80) >> 8;
}

void xn_cam_project_ptr(s32 x, s32 y, s32 z, s32 *sx, s32 *sy)
{
    xn_cam_project(x, y, z, sx, sy);
}

void xn_cam_project(s32 x, s32 y, s32 z, s32 *sx, s32 *sy)
{
    *sy = xn_muldiv_or0(y, xn_cam_focal_y, z);
    *sx = xn_muldiv_or0(x, xn_cam_focal_x, z);
}

/* (a * b + 80h) / d with a 64-bit product, or 0 */
static s32 muldiv_rounded(s32 a, s32 b, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    xn_s64_addu(&t, 0x80);
    return xn_s64_div_or0(&t, d);
}

void xn_cam_project_scaled(s32 x, s32 y, s32 z, s32 *sx, s32 *sy)
{
    *sy = muldiv_rounded(y, xn_cam_half_height, z);
    *sx = muldiv_rounded(x, xn_cam_half_width, z);
}

/* ---- the view matrix ------------------------------------------------------------------------ */

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

void xn_cam_scale_matrix_in_place(xn_mat3 *m)
{
    m->m[0][0] = xn_mulshr(m->m[0][0], xn_cam_scale_x, 14);
    m->m[0][1] = xn_mulshr(m->m[0][1], xn_cam_scale_x, 14);
    m->m[0][2] = xn_mulshr(m->m[0][2], xn_cam_scale_x, 14);
    m->m[1][0] = xn_mulshr(m->m[1][0], xn_cam_scale_y, 14);
    m->m[1][1] = xn_mulshr(m->m[1][1], xn_cam_scale_y, 14);
    m->m[2][2] = xn_mulshr(m->m[2][2], xn_cam_scale_y, 14);    /* Quirk Q-CAM-01: m[1][2] */
}

/* (v << 14) / d, 64-bit signed, or 0 */
static s32 unscale(s32 v, s32 d)
{
    xn_s64 t;

    xn_s64_set(&t, v);
    xn_s64_shl(&t, 14);
    return xn_s64_div_or0(&t, d);
}

void xn_cam_unscale_matrix(xn_mat3 *m)
{
    int c;

    for (c = 0; c < 3; c++)
        m->m[0][c] = unscale(m->m[0][c], xn_cam_scale_x);
    for (c = 0; c < 3; c++)
        m->m[1][c] = unscale(m->m[1][c], xn_cam_scale_y);
}

void xn_cam_forward_angles(s32 *pitch, s32 *yaw)
{
    s32 p, y;

    p = xn_vec_dir_to_angles_regs(xn_cam_forward_x, xn_cam_forward_y, xn_cam_forward_z, &y);
    *pitch = p;                         /* the pitch first: the yaw wins where they alias */
    *yaw = y;
}

/* ---- the sphere test ---------------------------------------------------------------------- */

/* a + b <= 0 for the true sum (the asm's `add; jle`: SF != OF, or ZF) */
static int sum_not_positive(s32 a, s32 b)
{
    s32 s = (s32)((u32)a + (u32)b);

    if (((a ^ s) & (b ^ s)) < 0)        /* the add overflowed: the true sum has a's sign */
        return a < 0;
    return s <= 0;
}

/* -|n_view * view + nz * pick_distance| >> 16, as the side-plane tests compute it */
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

s32 xn_cam_sphere_dist_y(s32 ny, s32 nz, s32 *low)
{
    return plane_dist(ny, xn_pick_view_y, nz, low);
}

int xn_cam_cull_sphere(s32 x, s32 y, s32 z, s32 radius, s32 *residue)
{
    xn_vec3 v;
    u8 code = 0;

    v.x = x;
    v.y = y;
    v.z = z;
    xn_mat_transform(&v, &xn_cam_view_matrix);
    /* the centre's outcode: the planes it is outside of */
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
    *residue = XN_LOW32(v.y * 2, xn_cam_inv_scale_y);  /* Quirk Q-CAM-02: the leftovers */
    pick_distance = v.z;
    if (code == 0)
        return 0;
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
