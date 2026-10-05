/* water.c: XnGine's dungeon water as readable C (xwater.h; see xngine.h and
   docs/xngine_readable.md, and docs/engine/smc/water.md for the self-modifying parts:
   the per-row step xn_water_draw patches into its own code, and the unrolled pixel loop
   xn_water_span stops with a planted `ret`). */
#include "xwater.h"
#include "xmat.h"

extern s32 xn_cam_near_z, xn_cam_far_z;         /* 24.8 */
extern s32 xn_cam_yaw, xn_cam_y;
extern xn_mat3 xn_cam_view_matrix;
extern s32 xn_cam_half_width, xn_cam_half_height, xn_cam_centre_x, xn_cam_centre_y;
extern s32 xn_gfx_clip_top, xn_gfx_clip_bottom, xn_gfx_width;
extern s32 xn_gfx_row_offset[768];
extern u8 *screen_buffer;
extern u8 *big_buffer;
extern struct xn_span xn_render_span_rows[];    /* one list head per screen row */

/* the asm entries of the body and of the poly group's clipping intersections (registers:
   esi, ebx the edge's ends, edi where the new vertex goes) */
extern u8 asm_xn_water_span_unrolled[];
extern void asm_xn_poly_clip_intersect_near(void);
extern void asm_xn_poly_clip_intersect_top(void);
extern void asm_xn_poly_clip_intersect_bottom(void);

s32 xn_rand_next(void);

#define OUT_BOTTOM  0x04
#define OUT_TOP     0x08
#define OUT_NEAR    0x10

#define WATER_STEP  21          /* bytes per pixel of the unrolled body */
#define WATER_STEPS 641

void xn_water_init(void)
{
    s32 v;
    int i;

    /* (each draw masks the number to the range: the asm's halving loops never run) */
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

/* A point of the plane to view space, and its outcode: the view planes it is outside of */
static void water_end(xn_water_end *e, s32 x, s32 y, s32 z)
{
    xn_vec3 v;
    u8 code = 0x3F;

    v.x = x;
    v.y = y;
    v.z = z;
    xn_mat_transform(&v, &xn_cam_view_matrix);
    e->x = v.x;
    e->y = v.y;
    e->z = v.z;
    if (v.z >= xn_cam_near_z)
        code ^= OUT_NEAR;
    if (v.z <= xn_cam_far_z)
        code ^= 0x20;
    if (v.x <= v.z)
        code ^= 0x02;
    if (v.y <= v.z)
        code ^= OUT_BOTTOM;
    if (v.x >= -v.z)
        code ^= 0x01;
    if (v.y >= -v.z)
        code ^= OUT_TOP;
    e->outcode = code;
}

/* Clips the line's second end to a plane: the intersection (with its outcode) goes to
   big_buffer and is copied over the end, 16 bytes */
static void clip_end(void (*intersect)(void))
{
    xn_regs r;

    r.esi = (u32)&xn_water_line_p0;
    r.ebx = (u32)&xn_water_line_p1;
    r.edi = (u32)big_buffer;
    xn_asmcall(intersect, &r);
    xn_water_line_p1 = *(xn_water_end *)big_buffer;
}

int xn_water_clip_extent(void)
{
    s32 saved_near = xn_cam_near_z;
    const xn_water_dirs *d;
    s32 y, t;
    u8 c;
    int pass;

    xn_cam_near_z = 0x401;
    d = &xn_water_quadrant_dirs[((xn_cam_yaw + 0x100) & 0x7FF) >> 9];
    y = (dungeon_water_level - xn_cam_y) << 8;
    water_end(&xn_water_line_p0, d->x0, y, d->z0);
    water_end(&xn_water_line_p1, d->x1, y, d->z1);

    /* each end in turn (the ends are swapped after each pass) */
    for (pass = 0; pass < 2; pass++) {
        if (xn_water_line_p1.outcode & OUT_NEAR) {
            clip_end(asm_xn_poly_clip_intersect_near);
            xn_water_line_p1.outcode &= ~OUT_NEAR;
        }
        if (xn_water_line_p1.outcode & OUT_TOP) {
            clip_end(asm_xn_poly_clip_intersect_top);
            xn_water_line_p1.outcode &= ~OUT_TOP;
        }
        if (xn_water_line_p1.outcode & OUT_BOTTOM)
            clip_end(asm_xn_poly_clip_intersect_bottom);    /* (its bit is not cleared) */
        /* the swap leaves the pad bytes where they are */
        t = xn_water_line_p0.x;
        xn_water_line_p0.x = xn_water_line_p1.x;
        xn_water_line_p1.x = t;
        t = xn_water_line_p0.y;
        xn_water_line_p0.y = xn_water_line_p1.y;
        xn_water_line_p1.y = t;
        t = xn_water_line_p0.z;
        xn_water_line_p0.z = xn_water_line_p1.z;
        xn_water_line_p1.z = t;
        c = xn_water_line_p0.outcode;
        xn_water_line_p0.outcode = xn_water_line_p1.outcode;
        xn_water_line_p1.outcode = c;
    }
    xn_cam_near_z = saved_near;
    return ((xn_water_line_p0.outcode | xn_water_line_p1.outcode) & (OUT_TOP | OUT_BOTTOM)) == 0;
}

void xn_water_clip_extent_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_water_clip_extent());
}

/* ---- drawing ---------------------------------------------------------------------------- */

/* 2^40 / z, unsigned (div: the asm divides the end's z in place) */
static s32 inverse_z(u32 z)
{
    xn_s64 n;

    n.lo = 0;
    n.hi = 0x100;
    return xn_u64_div(&n, z);
}

/* The screen row of a water line end: y * half height * (2^40 / z) >> 40, rounded, from
   the view centre */
static s32 end_row(const xn_water_end *e)
{
    return ((xn_mulhi(e->y * xn_cam_half_height, e->z) + 0x80) >> 8) + xn_cam_centre_y;
}

/* Calls the water span routine for pixels x..x_end - 1 of the row, with the registers the
   asm gives it (line is the row less 100h) */
static void call_span(u32 x, u32 x_end, u8 *line, s32 inv, struct xn_span *node, u32 rows)
{
    xn_regs r;

    r.ebx = x;
    r.ebp = x_end;
    r.edi = (u32)line;
    r.edx = inv;
    r.esi = (u32)node;
    r.ecx = rows;
    r.eax = 0;          /* (the routine sets EAX before reading it) */
    xn_asmcall(xn_water_span_fn, &r);
}

/* The runs of one row behind the water plane, whose 1/z there is inv: between spans, and
   wherever a span's depth goes behind the plane's (the span's x_start plus the 1/z
   difference over its d(1/z)/dx: (inv - span 1/z) * (2^32 / d(1/z)/dx) >> 32). The asm
   keeps x in EBX and sets only its low word (BX) from a span's x_end or x_start: kept. */
static void water_row(struct xn_span *head, u8 *line, s32 inv, u32 rows)
{
    struct xn_span *node = head;
    u32 x = xn_cam_centre_x - xn_cam_half_width;
    s32 k, cross;
    xn_s64 p;

    for (;;) {
        node = node->next;
        if (node == node->next)
            break;                          /* the sentinel */
        k = node->poly->dx_per_inv_z;
        if (inv > node->inv_z) {
            /* the water is in front of the span's start: hidden behind it from where the
               span comes nearer than the water */
            if (k <= 0)
                continue;
            xn_s64_mul(&p, k, inv - node->inv_z);
            cross = node->x_start + p.hi;
            if ((s32)node->x_end <= cross)
                continue;
            call_span(x, cross, line, inv, node, rows);
            x = (x & 0xFFFF0000) | node->x_end;
        } else {
            /* the span is in front: draw up to it, then go on from its end, or from where it
               goes behind the water */
            call_span(x, node->x_start, line, inv, node, rows);
            if (k >= 0) {
                x = node->x_end;
                continue;
            }
            xn_s64_mul(&p, k, inv - node->inv_z);
            x = ((x & 0xFFFF0000) | node->x_start) + p.hi;
            if ((s32)x > (s32)node->x_end)
                x = node->x_end;
        }
    }
    call_span(x, xn_cam_centre_x + xn_cam_half_width, line, inv, node, rows);
}

void xn_water_draw(void)
{
    s32 top, bottom, step, inv, t, r, phase;
    u32 rows;
    struct xn_span *head;
    u8 *line;

    if (!xn_water_clip_extent())
        return;
    xn_water_line_p0.z = inverse_z(xn_water_line_p0.z);
    xn_water_line_p1.z = inverse_z(xn_water_line_p1.z);
    bottom = end_row(&xn_water_line_p1);
    top = end_row(&xn_water_line_p0);
    if (bottom == top)
        return;
    if (bottom < top) {
        t = top;
        top = bottom;
        bottom = t;
        t = xn_water_line_p0.z;
        xn_water_line_p0.z = xn_water_line_p1.z;
        xn_water_line_p1.z = t;
    }
    if (top >= xn_gfx_clip_bottom || bottom <= xn_gfx_clip_top)
        return;
    if (top < xn_gfx_clip_top)
        top = xn_gfx_clip_top;
    if (bottom > xn_gfx_clip_bottom)
        bottom = xn_gfx_clip_bottom;
    rows = bottom - top;
    if ((s32)rows <= 2)
        return;
    step = (xn_water_line_p1.z - xn_water_line_p0.z) / (s32)rows;     /* cdq; idiv */
    XN_KEEP(xn_water_row_inv_step, step);
    inv = xn_water_line_p0.z;
    xn_water_row = top;
    line = screen_buffer + xn_gfx_row_offset[top] - 0x100;
    head = &xn_render_span_rows[top];
    do {
        /* the row's ripple moves on, whether any of it is drawn or not */
        r = xn_water_row++;
        phase = xn_water_row_phase[r] + xn_water_row_velocity[r];
        if (phase > 4)
            xn_water_row_velocity[r] = -xn_water_row_velocity[r];
        if (phase < -4)
            xn_water_row_velocity[r] = -xn_water_row_velocity[r];
        xn_water_row_phase[r] = phase;
        xn_water_row_jitter = xn_water_jitter + phase + xn_water_row_base[r] - 0x100;
        water_row(head, line, inv, rows);
        inv += step;
        line += xn_gfx_width;
        head++;
    } while (--rows != 0);
}

/* ---- the span ---------------------------------------------------------------------------- */

/* The unrolled body: pixel k = tint[pixel (k + jitter[k])], left to right (a pixel may be
   read after this run has rewritten it: a jitter of -1 tints it twice). The tint table is
   256-aligned; the asm puts the pixel in AL of the table's address, so only its bits 8-31
   count. Returns the last value written (AL), or tint's low byte when n is 0. */
static u8 water_run(u8 *pix, int n, const s32 *jitter, u32 tint)
{
    const u8 *table = (const u8 *)(tint & ~0xFFu);
    u8 v = (u8)tint;
    int k;

    for (k = 0; k < n; k++) {
        v = table[pix[k + jitter[k]]];
        pix[k] = v;
    }
    return v;
}

void xn_water_span(s32 x0, s32 x1, u8 *row)
{
    s32 n = x1 - x0;

    if (n <= 0)
        return;
    /* The body runs to the step the offset table gives for n: n itself (21 n) up to 640 (a
       row is at most 320 pixels). For a longer run the asm reads its offset past the table;
       the C takes that word as a step count too, which matches while it falls on a step. */
    n = xn_water_unroll_offsets[n] / WATER_STEP;
    water_run(row + 0x100 + x0, n, xn_water_row_jitter + 0x100 + x0, (u32)xn_water_tint_table);
}

void xn_water_span_r(xn_regs *r)
{
    xn_water_span(r->ebx, r->ebp, (u8 *)r->edi);
}

/* The count a planted ret gives an unrolled body of `max` steps of `stride` bytes: the step
   it stands on (max: none planted) */
static int planted_count(const u8 *body, int stride, int max)
{
    int n;

    for (n = 0; n < max; n++)
        if (body[n * stride] == 0xC3)
            return n;
    return max;
}

void xn_water_span_unrolled_r(xn_regs *r)
{
    int n = planted_count(asm_xn_water_span_unrolled, WATER_STEP, WATER_STEPS);
    u8 v = water_run((u8 *)r->edi + 0x100, n, (const s32 *)(r->esi + 0x400), r->eax);

    r->eax = (r->eax & ~0xFFu) | v;
}

void xn_water_stub_ret(void)
{
}
