/* water.c: XnGine's dungeon water (canonical C; the interface and the module's documentation
   are in xwater.h). The asm's self-modifying parts (docs/engine/smc/water.md) are plain C
   here: the per-row 1/z step it patched into its own code is a local, and the 641-step
   unrolled pixel loop it stopped with a planted `ret` is a loop of n steps. */
#include "xwater.h"
#include "xmat.h"
#include "xpoly.h"
#include "xrand.h"

#define OUT_RIGHT   0x02
#define OUT_LEFT    0x01
#define OUT_BOTTOM  0x04
#define OUT_TOP     0x08
#define OUT_NEAR    0x10
#define OUT_FAR     0x20

#define CLIP_NEAR_Z 0x401       /* the near plane while the water line is clipped */

void xn_water_init(void)
{
    s32 v;
    int i;

    /* (each draw is masked to its range: the asm's halving loops never run) */
    for (i = 0; i < 512; i++) {
        v = (xn_rand_next() & 0x3F) - 0x20;
        if (v < -1)
            v = -1;
        else if (v > 1)
            v = 1;
        xn_water_jitter[i] = v;
    }
    for (i = 0; i < 200; i++) {
        xn_water_row_phase[i] = (xn_rand_next() & 7) - 4;
        v = xn_rand_next() & 0x7F;
        xn_water_row_base[i] = v < 8 ? 8 : v;
        xn_water_row_velocity[i] = ((xn_rand_next() & 1) - 1) | 1;
    }
}

/* ---- the water line --------------------------------------------------------------------- */

/* A point of the plane (x, y, z before the view's rotation) in view space, and its outcode:
   the view planes it is outside of */
static void water_end(struct xn_poly_vertex *e, s32 x, s32 y, s32 z)
{
    xn_vec3 v;
    u8 code = 0;

    v.x = x;
    v.y = y;
    v.z = z;
    xn_mat_transform(&v, &xn_cam_view_matrix);
    e->x = v.x;
    e->y = v.y;
    e->z = v.z;
    if (v.z < xn_cam_near_z)
        code |= OUT_NEAR;
    if (v.z > xn_cam_far_z)
        code |= OUT_FAR;
    if (v.x > v.z)
        code |= OUT_RIGHT;
    if (v.y > v.z)
        code |= OUT_BOTTOM;
    if (v.x < -v.z)
        code |= OUT_LEFT;
    if (v.y < -v.z)
        code |= OUT_TOP;
    e->outcode = code;
}

/* Cuts the end p1 of the line p0 -> p1 at a plane: the clipper's intersection, taken from p1
   (its edge's first end), is written with its outcode at big_buffer, the asm's work area,
   and replaces p1 */
static void clip_end(const struct xn_poly_vertex *p0, struct xn_poly_vertex *p1,
                     void (*intersect)(const struct xn_poly_vertex *,
                                       const struct xn_poly_vertex *, struct xn_poly_vertex *))
{
    struct xn_poly_vertex *cut = (struct xn_poly_vertex *)big_buffer;

    intersect(p1, p0, cut);
    *p1 = *cut;
}

static void swap_ends(struct xn_poly_vertex *a, struct xn_poly_vertex *b)
{
    struct xn_poly_vertex t = *a;

    *a = *b;
    *b = t;
}

int xn_water_clip_extent(struct xn_poly_vertex *p0, struct xn_poly_vertex *p1)
{
    s32 saved_near = xn_cam_near_z;
    const xn_water_dirs *d;
    s32 y;
    int pass;

    xn_cam_near_z = CLIP_NEAR_Z;
    d = &xn_water_quadrant_dirs[((xn_cam_yaw + 0x100) & 0x7FF) >> 9];
    y = (dungeon_water_level - xn_cam_y) << 8;
    water_end(p0, d->x0, y, d->z0);
    water_end(p1, d->x1, y, d->z1);
    /* each end in turn (the ends are swapped after each pass, so p0 ends as it began) */
    for (pass = 0; pass < 2; pass++) {
        if (p1->outcode & OUT_NEAR) {
            clip_end(p0, p1, xn_poly_clip_intersect_near);
            p1->outcode &= ~OUT_NEAR;
        }
        if (p1->outcode & OUT_TOP) {
            clip_end(p0, p1, xn_poly_clip_intersect_top);
            p1->outcode &= ~OUT_TOP;
        }
        if (p1->outcode & OUT_BOTTOM)
            clip_end(p0, p1, xn_poly_clip_intersect_bottom);   /* Quirk Q-WATER-02: its bit
                                                                  is not cleared */
        swap_ends(p0, p1);
    }
    xn_cam_near_z = saved_near;
    return ((p0->outcode | p1->outcode) & (OUT_TOP | OUT_BOTTOM)) == 0;
}

/* ---- drawing ---------------------------------------------------------------------------- */

/* 2^40 / z, unsigned; 0 where the asm's div faults (z at most 100h; Q-SYS-01) */
static s32 inverse_z(u32 z)
{
    xn_s64 n;

    n.lo = 0;
    n.hi = 0x100;
    return xn_u64_div_or0(&n, z);
}

/* The screen row of a water line end: y * half height * (2^40 / z) >> 40, rounded, from the
   view's centre (the first product 32-bit) */
static s32 end_row(s32 y, s32 inv_z)
{
    return ((xn_mulhi(y * xn_cam_half_height, inv_z) + 0x80) >> 8) + xn_cam_centre_y;
}

/* The runs of one row behind the water plane, whose 1/z on this row is inv: between the
   S-buffer's spans, and wherever a span's depth goes past the plane's (the span's x_start
   plus the 1/z difference times the span's dx per 1/z, its high dword). Quirk Q-WATER-01:
   the asm keeps x in EBX and sets only its low word (BX) from a span's x_end or x_start. */
static void water_row(const struct xn_span *head, u8 *line, const s32 *jitter, s32 inv)
{
    const struct xn_span *node = head;
    s32 x = xn_cam_centre_x - xn_cam_half_width;
    s32 k, cross;
    xn_s64 p;

    for (;;) {
        node = node->next;
        if (node == node->next)
            break;                          /* the sentinel */
        k = node->poly->dx_per_inv_z;
        if (inv > node->inv_z) {
            /* the water is in front at the span's start: hidden behind the span from where
               the span comes nearer than the water */
            if (k <= 0)
                continue;
            xn_s64_mul(&p, k, inv - node->inv_z);
            cross = node->x_start + p.hi;
            if ((s32)node->x_end <= cross)
                continue;
            xn_water_span(line, jitter, x, cross);
            x = (x & 0xFFFF0000) | node->x_end;
        } else {
            /* the span is in front: draw up to it, then go on from its end, or from where it
               goes behind the water */
            xn_water_span(line, jitter, x, node->x_start);
            if (k >= 0) {
                x = node->x_end;
                continue;
            }
            xn_s64_mul(&p, k, inv - node->inv_z);
            x = ((x & 0xFFFF0000) | node->x_start) + p.hi;
            if (x > (s32)node->x_end)
                x = node->x_end;
        }
    }
    xn_water_span(line, jitter, x, xn_cam_centre_x + xn_cam_half_width);
}

void xn_water_draw(void)
{
    struct xn_poly_vertex p0, p1;
    s32 top, bottom, inv_top, inv_bottom, step, inv, phase, rows, t, r;
    const struct xn_span *head;
    u8 *line;

    if (!xn_water_clip_extent(&p0, &p1))
        return;
    inv_top = inverse_z(p0.z);
    inv_bottom = inverse_z(p1.z);
    bottom = end_row(p1.y, inv_bottom);
    top = end_row(p0.y, inv_top);
    if (bottom == top)
        return;
    if (bottom < top) {
        t = top, top = bottom, bottom = t;
        t = inv_top, inv_top = inv_bottom, inv_bottom = t;
    }
    if (top >= xn_gfx_clip_bottom || bottom <= xn_gfx_clip_top)
        return;
    if (top < xn_gfx_clip_top)
        top = xn_gfx_clip_top;
    if (bottom > xn_gfx_clip_bottom)
        bottom = xn_gfx_clip_bottom;
    rows = bottom - top;
    if (rows <= 2)
        return;
    step = (inv_bottom - inv_top) / rows;
    inv = inv_top;
    line = screen_buffer + xn_gfx_row_offset[top];
    head = &xn_render_span_rows[top];
    for (r = top; rows != 0; rows--, r++, inv += step, line += xn_gfx_width, head++) {
        /* the row's ripple moves on, whether any of the row is drawn or not */
        phase = xn_water_row_phase[r] + xn_water_row_velocity[r];
        if (phase > 4 || phase < -4)
            xn_water_row_velocity[r] = -xn_water_row_velocity[r];
        xn_water_row_phase[r] = phase;
        water_row(head, line, xn_water_jitter + phase + xn_water_row_base[r], inv);
    }
}

/* ---- the span ---------------------------------------------------------------------------- */

void xn_water_span(u8 *row, const s32 *jitter, s32 x0, s32 x1)
{
    s32 n = x1 - x0;

    if (n <= 0)
        return;
    /* the asm's tint pointer has the pixel put in its low byte: only bits 8-31 count */
    xn_water_span_unrolled(row + x0, jitter + x0, n,
                           (const u8 *)((u32)xn_water_tint_table & ~0xFFu));
}

void xn_water_span_unrolled(u8 *pix, const s32 *jitter, s32 n, const u8 *tint)
{
    s32 k;

    for (k = 0; k < n; k++)
        pix[k] = tint[pix[k + jitter[k]]];
}

void xn_water_stub_ret(void)
{
}
