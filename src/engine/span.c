/* span.c: XnGine's span routines as readable C (xspan.h; see xngine.h and
   docs/engine/smc/span.md). */
#include "xspan.h"
#include "xnsmc.h"
#include "xlight.h"
#include "xtmap.h"

/* the asm entries a setup routine installs in the polygon (compared as asm addresses) */
extern void asm_xn_span_solid(void);
extern void asm_xn_span_tex_16(void);
extern void asm_xn_span_tex_shaded_16(void);
extern void asm_xn_span_tex_lit(void);
/* the tails' asm bodies, where the planters put their rets */
extern void asm_xn_span_solid_lit_tail(void);
extern void asm_xn_span_tex_tail(void);
extern void asm_xn_span_tex_shaded_tail(void);
extern void asm_xn_span_tex64_tail(void);
extern void asm_xn_span_tex64_shaded_tail(void);
extern void asm_xn_span_flat_transparent_tail(void);
extern void asm_xn_span_flat_transparent_shaded_tail(void);
extern void asm_xn_span_flat_lit_fogged_tail(void);
extern void asm_xn_span_flat_translucent_tail(void);

/* z at a 1/z value (2^40 / z): the 1/z table at (inv_z >> 13) & FFFFh, << 9 */
#define Z_OF(table, inv_z)  ((s32)((table)[((u32)(inv_z) >> 13) & 0xFFFF] << 9))

/* a packed texture coordinate (v in the high half, u in the low half, 8.8 each): its texel's
   offset in rows of 256, v's integer byte then u's */
#define TEXEL(uv)           ((((uv) >> 16) & 0xFF00) | (((uv) >> 8) & 0xFF))

/* a shade row's colour c: the row's address (a shade value: its low byte is a fraction) with
   the low byte replaced */
#define ROW(row, c)         (*(const u8 *)(((u32)(row) & ~0xFFu) | (c)))

/* the polygon's packed texture origin (+18h: u in the low word, v in the high one) */
#define TEX_ORIGIN(poly)    (*(const u32 *)&(poly)->tex_u0)

/* ---- solid colour ----------------------------------------------------------------------- */

/* the asm's rep stosb of the n & 3 first pixels, then rep stosd of the colour dword */
static void fill(u8 *pix, s32 n, u32 colour4)
{
    u32 k;

    for (k = 0; k < (u32)(n & 3); k++)
        *pix++ = (u8)colour4;
    for (k = 0; k < (u32)n >> 2; k++, pix += 4)
        *(u32 *)pix = colour4;
}

void xn_span_solid(struct xn_poly *poly, s32 n, u8 *pix)
{
    fill(pix, n, poly->colour4);
}

/* asm: eax = poly, ebp = n, edi = the first pixel - 1 (and esi, ebx as for every routine) */
void xn_span_solid_r(xn_regs *r)
{
    xn_span_solid((struct xn_poly *)r->eax, r->ebp, (u8 *)r->edi + 1);
}

void xn_span_solid_shaded_setup(struct xn_poly *poly, s32 n, u8 *pix)
{
    poly->span_fn = XN_ASM(xn_span_solid);
    poly->colour4 = xn_colour_fill_table[ROW(poly->shade_row, (u8)poly->colour4)];
    fill(pix, n, poly->colour4);
}

void xn_span_solid_shaded_setup_r(xn_regs *r)
{
    xn_span_solid_shaded_setup((struct xn_poly *)r->eax, r->ebp, (u8 *)r->edi + 1);
}

/* ---- solid, lit per pixel ------------------------------------------------------------------ */

extern s32 xn_span_solid_lit_ray_a, xn_span_solid_lit_ray_b, xn_span_solid_lit_ray_c;
                                        /* the row's y ray (xn_render_frame, per row) */
extern s32 xn_span_solid_lit_ray_dx16;  /* the x ray over 16 pixels (xn_cam_update_derived) */
extern s32 xn_span_solid_lit_dz16;      /* KEEP: 1/z over 16 pixels */
extern u8 xn_span_solid_lit_colour;     /* KEEP: the colour (a byte operand) */
extern u8 *xn_span_solid_lit_end;       /* KEEP: the blocks' end (the first pixel - 1 + ...) */
extern s32 xn_span_solid_lit_shade;     /* the shade at the last 16-pixel boundary */

/* the polygon's shader at a pixel: the row's y ray, the column's x ray, its z */
#define SHADE_AT(shader, ray_y, ray_x, z) \
    xn_light_shade_eval((const u8 *)(shader), (ray_y), (ray_x), (z))

/* n pixels of the colour through a shade that moves by step every `per` pixels (1, or 2 in
   the 16-pixel blocks). Only the shade's bits 8-15 move the address: its top half stays the
   start shade's (base, whose low byte is the colour), so a carry out of bit 15 is lost. */
static u32 solid_lit_run(u8 *pix, int n, u32 base, u32 shade, s32 step, int per)
{
    int k;

    for (k = 0; k < n; k += per) {
        u8 c = *(const u8 *)((base & 0xFFFF00FFu) | (shade & 0xFF00u));

        shade += step;
        pix[k] = c;
        if (per == 2)
            pix[k + 1] = c;
    }
    return shade;
}

void xn_span_solid_lit(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    u32 inv_z = span->inv_z;
    s32 ray_x = xn_cam_dir_x_mid[xs];
    s32 dz16 = poly->inv_z_step * 2;
    s32 z, start, shade;

    XN_KEEP(xn_span_solid_lit_dz16, dz16);
    z = xn_udiv64(0x4000, 0, inv_z);                /* 2^46 / (2^40 / z): a divide error
                                                       below 4000h gives 0 */
    xn_span_solid_lit_shade = SHADE_AT(poly->shader, xn_span_solid_lit_ray_a, ray_x, z);
    if (n >= 16) {
        u8 *end = pix - 1 + (n & ~15);
        u8 colour = (u8)poly->colour4;

        XN_KEEP(xn_span_solid_lit_colour, colour);
        XN_KEEP(xn_span_solid_lit_end, end);
        do {
            inv_z += dz16;
            z = xn_udiv64(0x4000, 0, inv_z);
            ray_x += xn_span_solid_lit_ray_dx16;
            shade = SHADE_AT(poly->shader, xn_span_solid_lit_ray_b, ray_x, z);
            start = xn_span_solid_lit_shade;
            xn_span_solid_lit_shade = shade;
            solid_lit_run(pix, 16, ((u32)start & ~0xFFu) | colour, start, (shade - start) >> 3, 2);
            pix += 16;
        } while (pix - 1 != end);
        n &= 15;
        if (n == 0)
            return;
    }
    /* the last n pixels: the shade at x + n, the step divided by n */
    ray_x += xn_cam_dir_x_mid[n];
    z = xn_udiv64(0x4000, 0, n * poly->inv_z_dx + inv_z);
    shade = SHADE_AT(poly->shader, xn_span_solid_lit_ray_c, ray_x, z);
    start = xn_span_solid_lit_shade;
    solid_lit_run(pix, n, ((u32)start & ~0xFFu) | (u8)poly->colour4, start,
                  (shade - start) * xn_recip16_table[n] >> 16, 1);
}

/* asm: eax = poly, esi = span, ebx = x - centre x, ebp = n, edi = the first pixel - 1 */
void xn_span_solid_lit_r(xn_regs *r)
{
    xn_span_solid_lit((struct xn_poly *)r->eax, (const struct xn_span *)r->esi, r->ebx, r->ebp,
                      (u8 *)r->edi + 1);
}

/* the tail's own records (the RET adapter): eax = the start shade with the colour in al,
   ecx = the shade, ebp = its step, edi = the first pixel - 1 */
void xn_span_solid_lit_tail_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_span_solid_lit_tail, 9, 16);

    solid_lit_run((u8 *)r->edi + 1, n, r->eax, r->ecx, r->ebp, 1);
}

/* ---- textured -------------------------------------------------------------------------------- */

/* a packed step from the u and v differences over a block of 2^shift pixels: v's in the high
   half (shl 16 - shift: the block difference / 2^shift in 8.8), u's below (sar) */
static u32 uv_step(s32 dv, s32 du, int shift)
{
    return ((u32)dv << (16 - shift) & 0xFFFF0000u) | (u16)(du >> shift);
}

/* n texels from uv, wrapped by the texture's mask after every step (row: 0, or the shade
   row they go through); returns the coordinate after the last */
static u32 tex_run(u8 *pix, int n, u32 uv, u32 step, u32 mask, const u8 *texels, const u8 *row)
{
    int k;

    for (k = 0; k < n; k++) {
        u8 c = texels[TEXEL(uv)];

        uv = (uv + step) & mask;
        pix[k] = row ? ROW(row, c) : c;
    }
    return uv;
}

/* One of the four textured routines: its blocks (2^shift pixels), the operands it patches
   (rule PF: the 1/z table's, from xn_render_init; KEEP: its own) and its variables. */
typedef struct tex_routine {
    int shift;
    const u32 **recip_a, **recip_b, **recip_c;
    s32 *dz, *dv, *du;
    u32 *origin;
    u8 **end;
    s32 *u_num, *v_num, *u_prev, *v_prev;
    u8 **row;                           /* the shaded routines' shade row, else 0 */
} tex_routine;

extern const u32 *xn_span_tex8_recip_a, *xn_span_tex8_recip_b, *xn_span_tex8_recip_c;
extern s32 xn_span_tex8_dz, xn_span_tex8_dv, xn_span_tex8_du;
extern u32 xn_span_tex8_origin;
extern u8 *xn_span_tex8_end;
extern const u32 *xn_span_tex16_recip_a, *xn_span_tex16_recip_b, *xn_span_tex16_recip_c;
extern s32 xn_span_tex16_dz, xn_span_tex16_dv, xn_span_tex16_du;
extern u32 xn_span_tex16_origin;
extern u8 *xn_span_tex16_end;
extern s32 xn_span_tex_u_num, xn_span_tex_v_num, xn_span_tex_u_prev, xn_span_tex_v_prev;

extern const u32 *xn_span_texsh8_recip_a, *xn_span_texsh8_recip_b, *xn_span_texsh8_recip_c;
extern s32 xn_span_texsh8_dz, xn_span_texsh8_dv, xn_span_texsh8_du;
extern u32 xn_span_texsh8_origin;
extern u8 *xn_span_texsh8_end;
extern const u32 *xn_span_texsh16_recip_a, *xn_span_texsh16_recip_b, *xn_span_texsh16_recip_c;
extern s32 xn_span_texsh16_dz, xn_span_texsh16_dv, xn_span_texsh16_du;
extern u32 xn_span_texsh16_origin;
extern u8 *xn_span_texsh16_end;
extern s32 xn_span_tex_shaded_u_num, xn_span_tex_shaded_v_num;
extern s32 xn_span_tex_shaded_u_prev, xn_span_tex_shaded_v_prev;
extern u8 *xn_span_tex_shaded_row;

static const tex_routine tex_8 = {
    3, &xn_span_tex8_recip_a, &xn_span_tex8_recip_b, &xn_span_tex8_recip_c,
    &xn_span_tex8_dz, &xn_span_tex8_dv, &xn_span_tex8_du, &xn_span_tex8_origin,
    &xn_span_tex8_end, &xn_span_tex_u_num, &xn_span_tex_v_num, &xn_span_tex_u_prev,
    &xn_span_tex_v_prev, 0
};
static const tex_routine tex_16 = {
    4, &xn_span_tex16_recip_a, &xn_span_tex16_recip_b, &xn_span_tex16_recip_c,
    &xn_span_tex16_dz, &xn_span_tex16_dv, &xn_span_tex16_du, &xn_span_tex16_origin,
    &xn_span_tex16_end, &xn_span_tex_u_num, &xn_span_tex_v_num, &xn_span_tex_u_prev,
    &xn_span_tex_v_prev, 0
};
static const tex_routine tex_shaded_8 = {
    3, &xn_span_texsh8_recip_a, &xn_span_texsh8_recip_b, &xn_span_texsh8_recip_c,
    &xn_span_texsh8_dz, &xn_span_texsh8_dv, &xn_span_texsh8_du, &xn_span_texsh8_origin,
    &xn_span_texsh8_end, &xn_span_tex_shaded_u_num, &xn_span_tex_shaded_v_num,
    &xn_span_tex_shaded_u_prev, &xn_span_tex_shaded_v_prev, &xn_span_tex_shaded_row
};
static const tex_routine tex_shaded_16 = {
    4, &xn_span_texsh16_recip_a, &xn_span_texsh16_recip_b, &xn_span_texsh16_recip_c,
    &xn_span_texsh16_dz, &xn_span_texsh16_dv, &xn_span_texsh16_du, &xn_span_texsh16_origin,
    &xn_span_texsh16_end, &xn_span_tex_shaded_u_num, &xn_span_tex_shaded_v_num,
    &xn_span_tex_shaded_u_prev, &xn_span_tex_shaded_v_prev, &xn_span_tex_shaded_row
};

static void tex_span(const tex_routine *t, struct xn_poly *poly, const struct xn_span *span,
                     s32 xs, s32 n, u8 *pix)
{
    s32 block = 1 << t->shift;
    u32 inv_z = span->inv_z;
    s32 z = Z_OF(*t->recip_a, inv_z);
    u32 mask = poly->wrap_mask;
    const u8 *row = 0;
    s32 u, v;

    if (t->row)
        row = *t->row = poly->shade_row;
    *t->v_num = xs * poly->v_dx + xn_render_row_y * poly->v_dy + poly->v_c;
    *t->v_prev = xn_mulhi(*t->v_num, z);
    *t->u_num = xs * poly->u_dx + xn_render_row_y * poly->u_dy + poly->u_c;
    *t->u_prev = xn_mulhi(*t->u_num, z);
    if (n >= block) {
        u8 *end = pix - 1 + (n & ~(block - 1));
        s32 dz = poly->inv_z_step, du = poly->u_step, dv = poly->v_step;
        u32 origin = TEX_ORIGIN(poly);

        XN_KEEP(*t->dz, dz);
        XN_KEEP(*t->du, du);
        XN_KEEP(*t->dv, dv);
        XN_KEEP(*t->origin, origin);
        XN_KEEP(*t->end, end);
        do {
            u32 uv;

            inv_z += dz;
            *t->v_num += dv;
            *t->u_num += du;
            z = Z_OF(*t->recip_b, inv_z);
            v = xn_mulhi(z, *t->v_num);
            u = xn_mulhi(z, *t->u_num);
            uv = ((u32)*t->v_prev << 16 | (u16)*t->u_prev) + origin;
            tex_run(pix, block, uv & mask, uv_step(v - *t->v_prev, u - *t->u_prev, t->shift),
                    mask, poly->texels, row);
            *t->v_prev = v;
            *t->u_prev = u;
            pix += block;
        } while (pix - 1 != end);
        n &= block - 1;
        if (n == 0)
            return;
    }
    /* the last n pixels: the step of a whole block from here, n pixels of it */
    z = Z_OF(*t->recip_c, inv_z + poly->inv_z_step);
    v = xn_mulhi(poly->v_step + *t->v_num, z);
    u = xn_mulhi(poly->u_step + *t->u_num, z);
    tex_run(pix, n, (((u32)*t->v_prev << 16 | (u16)*t->u_prev) + TEX_ORIGIN(poly)) & mask,
            uv_step(v - *t->v_prev, u - *t->u_prev, t->shift), mask, poly->texels, row);
}

#define SPAN_ARGS(r)  (struct xn_poly *)(r)->eax, (const struct xn_span *)(r)->esi, (r)->ebx, \
                      (r)->ebp, (u8 *)(r)->edi + 1

void xn_span_tex_8(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    tex_span(&tex_8, poly, span, xs, n, pix);
}

void xn_span_tex_8_r(xn_regs *r)
{
    xn_span_tex_8(SPAN_ARGS(r));
}

/* the 8-pixel steps doubled once: a 16-pixel routine from now on */
static void double_steps(struct xn_poly *poly)
{
    poly->u_step <<= 1;
    poly->v_step <<= 1;
    poly->inv_z_step <<= 1;
}

void xn_span_tex_16_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix)
{
    double_steps(poly);
    poly->span_fn = XN_ASM(xn_span_tex_16);
    xn_span_tex_16(poly, span, xs, n, pix);
}

void xn_span_tex_16_setup_r(xn_regs *r)
{
    xn_span_tex_16_setup(SPAN_ARGS(r));
}

void xn_span_tex_16(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    tex_span(&tex_16, poly, span, xs, n, pix);
}

void xn_span_tex_16_r(xn_regs *r)
{
    xn_span_tex_16(SPAN_ARGS(r));
}

/* asm: eax = the mask, ecx = the step, ebx = the coordinate, esi = the texels, edi = the
   first pixel - 1 */
void xn_span_tex_tail_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_span_tex_tail, 17, 16);

    tex_run((u8 *)r->edi + 1, n, r->ebx, r->ecx, r->eax, (const u8 *)r->esi, 0);
}

void xn_span_tex_shaded_8(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix)
{
    tex_span(&tex_shaded_8, poly, span, xs, n, pix);
}

void xn_span_tex_shaded_8_r(xn_regs *r)
{
    xn_span_tex_shaded_8(SPAN_ARGS(r));
}

void xn_span_tex_shaded_16_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                 s32 n, u8 *pix)
{
    double_steps(poly);
    poly->span_fn = XN_ASM(xn_span_tex_shaded_16);
    xn_span_tex_shaded_16(poly, span, xs, n, pix);
}

void xn_span_tex_shaded_16_setup_r(xn_regs *r)
{
    xn_span_tex_shaded_16_setup(SPAN_ARGS(r));
}

void xn_span_tex_shaded_16(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                           u8 *pix)
{
    tex_span(&tex_shaded_16, poly, span, xs, n, pix);
}

void xn_span_tex_shaded_16_r(xn_regs *r)
{
    xn_span_tex_shaded_16(SPAN_ARGS(r));
}

/* asm: eax = the shade row (bits 8-31), ecx = the step, ebx = the coordinate, ebp = the mask,
   esi = the texels, edi = the first pixel - 1 */
void xn_span_tex_shaded_tail_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_span_tex_shaded_tail, 19, 16);

    tex_run((u8 *)r->edi + 1, n, r->ebx, r->ecx, r->ebp, (const u8 *)r->esi,
            (const u8 *)(r->eax | 1));
}

/* ---- textured, lit per pixel ------------------------------------------------------------------ */

extern const u32 *xn_span_texlit_recip_a, *xn_span_texlit_recip_b, *xn_span_texlit_recip_c;
extern s32 xn_span_texlit_ray_a, xn_span_texlit_ray_b, xn_span_texlit_ray_c;
extern s32 xn_span_texlit_ray_dx16;
extern s32 xn_span_texlit_dz, xn_span_texlit_du, xn_span_texlit_dv;
extern u32 xn_span_texlit_origin;
extern u8 *xn_span_texlit_end;
extern struct xn_tmap_copy *xn_span_tex_lit_tmap;
extern void (*xn_span_tex_lit_shader)(void);
extern s32 xn_span_tex_lit_shade, xn_span_tex_lit_shade_end;
extern s32 xn_span_tex_lit_u_num, xn_span_tex_lit_v_num, xn_span_tex_lit_u_prev,
    xn_span_tex_lit_v_prev;
extern s32 xn_span_tex_lit_ray_x;

void xn_span_tex_lit_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                           u8 *pix)
{
    u32 *origin = (u32 *)&poly->tex_u0;

    double_steps(poly);
    *origin = *origin << 16 | *origin >> 16;
    poly->span_fn = XN_ASM(xn_span_tex_lit);
    xn_span_tex_lit(poly, span, xs, n, pix);
}

void xn_span_tex_lit_setup_r(xn_regs *r)
{
    xn_span_tex_lit_setup(SPAN_ARGS(r));
}

/* u in the high half and v in the low one here: the order the mapper copies want */
void xn_span_tex_lit(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    u32 inv_z = span->inv_z;
    s32 z, u, v, shade;

    xn_span_tex_lit_tmap = (struct xn_tmap_copy *)poly->tmap;
    xn_span_tex_lit_shader = poly->shader;
    z = Z_OF(xn_span_texlit_recip_a, inv_z);
    xn_span_tex_lit_ray_x = xn_cam_dir_x_mid[xs];
    xn_span_tex_lit_shade = SHADE_AT(xn_span_tex_lit_shader, xn_span_texlit_ray_a,
                                     xn_span_tex_lit_ray_x, z);
    xn_span_tex_lit_u_num = xs * poly->u_dx + xn_render_row_y * poly->u_dy + poly->u_c;
    xn_span_tex_lit_u_prev = xn_mulhi(xn_span_tex_lit_u_num, z);
    xn_span_tex_lit_v_num = xs * poly->v_dx + xn_render_row_y * poly->v_dy + poly->v_c;
    xn_span_tex_lit_v_prev = xn_mulhi(xn_span_tex_lit_v_num, z);
    if (n >= 16) {
        u8 *end = pix - 1 + (n & ~15);
        s32 dz = poly->inv_z_step, du = poly->u_step, dv = poly->v_step;
        u32 origin = TEX_ORIGIN(poly);
        s32 start = xn_span_tex_lit_shade;

        XN_KEEP(xn_span_texlit_dz, dz);
        XN_KEEP(xn_span_texlit_dv, dv);
        XN_KEEP(xn_span_texlit_du, du);
        XN_KEEP(xn_span_texlit_origin, origin);
        XN_KEEP(xn_span_texlit_end, end);
        do {
            u32 uv, step;

            xn_span_tex_lit_ray_x += xn_span_texlit_ray_dx16;
            xn_span_tex_lit_u_num += du;
            xn_span_tex_lit_v_num += dv;
            inv_z += dz;
            z = Z_OF(xn_span_texlit_recip_b, inv_z);
            shade = SHADE_AT(xn_span_tex_lit_shader, xn_span_texlit_ray_b,
                             xn_span_tex_lit_ray_x, z);
            u = xn_mulhi(z, xn_span_tex_lit_u_num);
            v = xn_mulhi(z, xn_span_tex_lit_v_num);
            uv = ((u32)xn_span_tex_lit_u_prev << 16 | (u16)xn_span_tex_lit_v_prev) + origin;
            step = uv_step(u - xn_span_tex_lit_u_prev, v - xn_span_tex_lit_v_prev, 4);
            xn_span_tex_lit_u_prev = u;
            xn_span_tex_lit_v_prev = v;
            xn_tmap_run(xn_span_tex_lit_tmap, pix, 16, uv, step, start, (shade - start) >> 3);
            start = shade;
            pix += 16;
        } while (pix - 1 != end);
        xn_span_tex_lit_shade = start;
        n &= 15;
        if (n == 0)
            return;
    }
    /* The last n pixels: the shade at x + n but with the 1/z of pixel n - 1 (the asm's
       lea eax, [ebp - 1]), u and v at pixel n, steps divided by n. */
    z = Z_OF(xn_span_texlit_recip_c, inv_z + (n - 1) * poly->inv_z_dx);
    xn_span_tex_lit_shade_end = SHADE_AT(xn_span_tex_lit_shader, xn_span_texlit_ray_c,
                                         xn_span_tex_lit_ray_x + xn_cam_dir_x_mid[n], z);
    u = xn_mulhi(poly->u_dx * n + xn_span_tex_lit_u_num, z);
    v = xn_mulhi(poly->v_dx * n + xn_span_tex_lit_v_num, z);
    xn_tmap_run(xn_span_tex_lit_tmap, pix, n,
                ((u32)xn_span_tex_lit_u_prev << 16 | (u16)xn_span_tex_lit_v_prev) + TEX_ORIGIN(poly),
                ((u32)((u - xn_span_tex_lit_u_prev) * xn_recip16_table[n]) & 0xFFFF0000u) |
                    (u16)((v - xn_span_tex_lit_v_prev) * xn_recip16_table[n] >> 16),
                xn_span_tex_lit_shade, (xn_span_tex_lit_shade_end - xn_span_tex_lit_shade) >> 3);
}

void xn_span_tex_lit_r(xn_regs *r)
{
    xn_span_tex_lit(SPAN_ARGS(r));
}

/* ---- the terrain's 64 x 64 textures ------------------------------------------------------------ */

/* v * z and u * z as low dwords: v's top half and u's bits 16-31, packed */
static u32 pack64(s32 vz, s32 uz)
{
    return ((u32)vz & 0xFFFF0000u) | ((u32)uz >> 16);
}

/* the per-pixel step of a 16-pixel block from uv to end: both halves of the difference >> 4
   at once (v's takes u's borrow), and u's half the low 12 bits of that, sign-extended */
static u32 step64_16(u32 end, u32 uv)
{
    s32 d = (s32)(end - uv) >> 4;

    return ((u32)d & 0xFFFF0000u) | (u16)((s16)(d << 4) >> 4);
}

/* the step over the last n pixels: u's difference (the low half, signed) / n through
   2^32 / n, v's (the high half) through 2^16 / n */
static u32 step64_n(u32 end, u32 uv, s32 n)
{
    u32 d = end - uv;

    return ((u32)(((s32)d >> 16) * xn_recip16_table[n]) & 0xFFFF0000u) |
           (u16)xn_mulhi((s16)d, xn_recip32_table[n]);
}

/* n texels: the offset wrapped by mask (3F3Fh), the coordinate itself never wrapped (its
   carries run on into the other half); row: 0 or the shade row */
static u32 tex64_run(u8 *pix, int n, u32 uv, u32 step, u32 mask, const u8 *texels,
                     const u8 *row)
{
    int k;

    for (k = 0; k < n; k++) {
        u8 c = texels[TEXEL(uv) & mask];

        uv += step;
        pix[k] = row ? ROW(row, c) : c;
    }
    return uv;
}

typedef struct tex64_routine {
    const u32 **recip_a, **recip_b, **recip_c;
    s32 *dz, *du, *dv;
    u32 *origin;
    u8 **end, **row;                    /* the shaded routine's (KEEP), else 0 */
} tex64_routine;

extern const u32 *xn_span_tex64_recip_a, *xn_span_tex64_recip_b, *xn_span_tex64_recip_c;
extern s32 xn_span_tex64_dz, xn_span_tex64_du, xn_span_tex64_dv;
extern u32 xn_span_tex64_origin;
extern const u32 *xn_span_tex64sh_recip_a, *xn_span_tex64sh_recip_b, *xn_span_tex64sh_recip_c;
extern s32 xn_span_tex64sh_dz, xn_span_tex64sh_du, xn_span_tex64sh_dv;
extern u32 xn_span_tex64sh_origin;
extern u8 *xn_span_tex64sh_end, *xn_span_tex64sh_row;

static const tex64_routine tex64 = {
    &xn_span_tex64_recip_a, &xn_span_tex64_recip_b, &xn_span_tex64_recip_c,
    &xn_span_tex64_dz, &xn_span_tex64_du, &xn_span_tex64_dv, &xn_span_tex64_origin, 0, 0
};
static const tex64_routine tex64_shaded = {
    &xn_span_tex64sh_recip_a, &xn_span_tex64sh_recip_b, &xn_span_tex64sh_recip_c,
    &xn_span_tex64sh_dz, &xn_span_tex64sh_du, &xn_span_tex64sh_dv, &xn_span_tex64sh_origin,
    &xn_span_tex64sh_end, &xn_span_tex64sh_row
};

static void tex64_span(const tex64_routine *t, struct xn_poly *poly, const struct xn_span *span,
                       s32 xs, s32 n, u8 *pix)
{
    u32 inv_z = span->inv_z;
    s32 v_num = xs * poly->v_dx + poly->v_c + xn_render_row_y * poly->v_dy;
    s32 u_num = xs * poly->u_dx + xn_render_row_y * poly->u_dy + poly->u_c;
    s32 z = (*t->recip_a)[(inv_z >> 13) & 0xFFFF];     /* not << 9: low-dword products */
    u32 origin = TEX_ORIGIN(poly);
    const u8 *row = t->row ? poly->shade_row : 0;
    u32 uv;

    XN_KEEP(*t->origin, origin);
    uv = pack64(v_num * z, u_num * z) + origin;
    if (n >= 16) {
        u32 blocks = (u32)n >> 4;
        s32 dz = poly->inv_z_step, du = poly->u_step, dv = poly->v_step;

        if (t->end)
            XN_KEEP(*t->end, pix - 1 + (n & ~15));
        XN_KEEP(*t->du, du);
        XN_KEEP(*t->dv, dv);
        XN_KEEP(*t->dz, dz);
        if (t->row)
            XN_KEEP(*t->row, poly->shade_row);
        do {
            inv_z += dz;
            u_num += du;
            v_num += dv;
            z = (*t->recip_b)[(inv_z >> 13) & 0xFFFF];
            uv = tex64_run(pix, 16, uv, step64_16(pack64(v_num * z, u_num * z) + origin, uv),
                           0x3F3F, poly->texels, row);
            pix += 16;
        } while (--blocks != 0);
        n &= 15;
        if (n == 0)
            return;
    }
    /* the last n pixels: divided exactly at the span's end */
    inv_z += n * poly->inv_z_dx;
    z = (*t->recip_c)[(inv_z >> 13) & 0xFFFF];
    tex64_run(pix, n, uv,
              step64_n(pack64((poly->v_dx * n + v_num) * z, (poly->u_dx * n + u_num) * z) +
                       TEX_ORIGIN(poly), uv, n),
              0x3F3F, poly->texels, row);
}

void xn_span_tex64(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    tex64_span(&tex64, poly, span, xs, n, pix);
}

void xn_span_tex64_r(xn_regs *r)
{
    xn_span_tex64(SPAN_ARGS(r));
}

/* asm: eax = the mask (3F3Fh), ecx = the step, ebx = the coordinate, esi = the texels */
void xn_span_tex64_tail_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_span_tex64_tail, 17, 16);

    tex64_run((u8 *)r->edi + 1, n, r->ebx, r->ecx, r->eax, (const u8 *)r->esi, 0);
}

void xn_span_tex64_shaded(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix)
{
    tex64_span(&tex64_shaded, poly, span, xs, n, pix);
}

void xn_span_tex64_shaded_r(xn_regs *r)
{
    xn_span_tex64_shaded(SPAN_ARGS(r));
}

/* asm: eax = the shade row, ebp = the mask (3F3Fh), ecx, ebx, esi as above */
void xn_span_tex64_shaded_tail_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_span_tex64_shaded_tail, 19, 16);

    tex64_run((u8 *)r->edi + 1, n, r->ebx, r->ecx, r->ebp, (const u8 *)r->esi,
              (const u8 *)(r->eax | 1));
}

/* ---- flats ------------------------------------------------------------------------------------ */

extern s32 xn_span_flat_transparent_cx, xn_span_flat_shaded_cx, xn_span_flat_fogged_cx,
    xn_span_flat_translucent_cx;        /* the view centre's x - 1 (xn_cam_set_view_window) */
extern const s32 *xn_span_flat_transparent_recip, *xn_span_flat_shaded_recip,
    *xn_span_flat_fogged_recip, *xn_span_flat_translucent_recip;
extern u32 xn_span_flat_shaded_rem;     /* KEEP: the DDA's remainder */
extern u32 xn_span_flat_fogged_step, xn_span_flat_fogged_rem;          /* KEEP */
extern u32 xn_span_flat_translucent_step, xn_span_flat_translucent_rem;

/* The flat's coordinate at x, its per-pixel step and the remainder of an 8-pixel DDA: one
   divide (the 1/z table at inv_z >> 13, unmasked; low-dword products). The step is the
   per-pixel difference d (v's half from dv * z, u's from (du * z) >> 16) >> 3 per half,
   and d & 70007h is added once after every 8 pixels: an exact DDA over 8. */
static u32 flat_setup(const struct xn_flat *f, s32 xs, const s32 *recip, u32 inv_z, u32 *step,
                      u32 *rem)
{
    s32 z = recip[inv_z >> 13];
    s32 u = (((xs * f->u_dx + f->u_c) >> 1) * z - f->u_offset) >> 15;
    s32 v = (xs * f->v_dx + f->v_c) * z - f->v_offset;
    s32 du = f->du_dx * z >> 16;
    u32 d = ((u32)(z * f->dv_dx) & 0xFFFF0000u) | (u16)du;

    *step = ((u32)((s32)d >> 3) & 0xFFFF0000u) | (u16)(du >> 3);
    *rem = d & 0x70007;
    return ((u32)v & 0xFFFF0000u) | (u16)u;
}

/* The pixel maps: the texel itself, through a shade row, through the light row then the fog
   row, or blended with the screen's pixel through a translucency table. Texel 0 is
   transparent in all of them. */
enum { FLAT_PLAIN, FLAT_SHADED, FLAT_FOGGED, FLAT_TRANSLUCENT };

static u32 flat_run(int how, u8 *pix, int n, u32 uv, u32 step, const u8 *texels,
                    const u8 *row, const u8 *table)
{
    int k;

    for (k = 0; k < n; k++) {
        u8 c = texels[TEXEL(uv)];

        uv += step;
        if (c == 0)
            continue;
        switch (how) {
        case FLAT_SHADED:
            c = ROW(row, c);
            break;
        case FLAT_FOGGED:
            c = ROW(table, ROW(row, c));
            break;
        case FLAT_TRANSLUCENT:
            c = table[c << 8 | pix[k]];
            break;
        }
        pix[k] = c;
    }
    return uv;
}

/* n pixels of a flat: 8-pixel blocks each ending with the remainder, then the rest */
static void flat_span(int how, const struct xn_flat *f, u32 uv, u32 step, u32 rem, s32 n,
                      u8 *pix, const u8 *row, const u8 *table)
{
    if (n >= 8) {
        u32 blocks = (u32)n >> 3;

        do {
            uv = flat_run(how, pix, 8, uv, step, f->texels, row, table) + rem;
            pix += 8;
        } while (--blocks != 0);
        n &= 7;
    }
    flat_run(how, pix, n, uv, step, f->texels, row, table);
}

void xn_span_flat_transparent(const struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix)
{
    u32 step, rem;
    u32 uv = flat_setup(flat, x - xn_span_flat_transparent_cx, xn_span_flat_transparent_recip,
                        inv_z, &step, &rem);

    flat_span(FLAT_PLAIN, flat, uv, step, rem, n, pix, 0, 0);
}

/* asm: esi = the flat, ecx = 1/z, ebx = x, ebp = n, edi = the first pixel - 1 */
#define FLAT_ARGS(r)  (const struct xn_flat *)(r)->esi, (r)->ecx, (r)->ebx, (r)->ebp, \
                      (u8 *)(r)->edi + 1

void xn_span_flat_transparent_r(xn_regs *r)
{
    xn_span_flat_transparent(FLAT_ARGS(r));
}

/* the tails: ebx = the coordinate, ecx (or ebp) = the step, esi = the texels */
void xn_span_flat_transparent_tail_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_span_flat_transparent_tail, 19, 8);

    flat_run(FLAT_PLAIN, (u8 *)r->edi + 1, n, r->ebx, r->ecx, (const u8 *)r->esi, 0, 0);
}

void xn_span_flat_transparent_shaded(const struct xn_flat *flat, u32 inv_z, s32 x, s32 n,
                                     u8 *pix)
{
    u32 step, rem;
    u32 uv = flat_setup(flat, x - xn_span_flat_shaded_cx, xn_span_flat_shaded_recip, inv_z,
                        &step, &rem);

    XN_KEEP(xn_span_flat_shaded_rem, rem);
    flat_span(FLAT_SHADED, flat, uv, step, rem, n, pix, flat->shade_row, 0);
}

void xn_span_flat_transparent_shaded_r(xn_regs *r)
{
    xn_span_flat_transparent_shaded(FLAT_ARGS(r));
}

/* eax = the shade row */
void xn_span_flat_transparent_shaded_tail_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_span_flat_transparent_shaded_tail, 21, 8);

    flat_run(FLAT_SHADED, (u8 *)r->edi + 1, n, r->ebx, r->ecx, (const u8 *)r->esi,
             (const u8 *)(r->eax | 1), 0);
}

void xn_span_flat_lit_fogged(const struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix)
{
    u32 step, rem;
    u32 uv = flat_setup(flat, x - xn_span_flat_fogged_cx, xn_span_flat_fogged_recip, inv_z,
                        &step, &rem);

    XN_KEEP(xn_span_flat_fogged_step, step);
    XN_KEEP(xn_span_flat_fogged_rem, rem);
    flat_span(FLAT_FOGGED, flat, uv, step, rem, n, pix, flat->shade_row, flat->table);
}

void xn_span_flat_lit_fogged_r(xn_regs *r)
{
    xn_span_flat_lit_fogged(FLAT_ARGS(r));
}

/* eax = the light row, ecx = the fog row, ebp = the step */
void xn_span_flat_lit_fogged_tail_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_span_flat_lit_fogged_tail, 23, 8);

    flat_run(FLAT_FOGGED, (u8 *)r->edi + 1, n, r->ebx, r->ebp, (const u8 *)r->esi,
             (const u8 *)(r->eax | 1), (const u8 *)(r->ecx | 1));
}

/* Never ran in play: the differential tests only. */
void xn_span_flat_translucent(const struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix)
{
    u32 step, rem;
    u32 uv = flat_setup(flat, x - xn_span_flat_translucent_cx, xn_span_flat_translucent_recip,
                        inv_z, &step, &rem);

    XN_KEEP(xn_span_flat_translucent_step, step);
    XN_KEEP(xn_span_flat_translucent_rem, rem);
    flat_span(FLAT_TRANSLUCENT, flat, uv, step, rem, n, pix, 0, flat->table);
}

void xn_span_flat_translucent_r(xn_regs *r)
{
    xn_span_flat_translucent(FLAT_ARGS(r));
}

/* ecx = the table, ebp = the step (eax: 0 but for al/ah, which the body loads) */
void xn_span_flat_translucent_tail_r(xn_regs *r)
{
    int n = xn_planted_count(asm_xn_span_flat_translucent_tail, 25, 8);

    flat_run(FLAT_TRANSLUCENT, (u8 *)r->edi + 1, n, r->ebx, r->ebp, (const u8 *)r->esi, 0,
             (const u8 *)(r->ecx + (r->eax & 0xFFFF0000u)));
}

/* ---- the S-buffer ------------------------------------------------------------------------------ */

/* a new node [x0, x1) of the polygon being built, 1/z inv_z at x0, between prev and next. The
   ends are stored as the asm's one dword x0 << 16 | x1 (all of x1's bits). */
static void insert_node(struct xn_span *prev, struct xn_span *next, s32 x0, s32 x1, s32 inv_z)
{
    struct xn_span *s = xn_render_span_next++;

    prev->next = s;
    s->next = next;
    *(u32 *)&s->x_end = (u32)x0 << 16 | (u32)x1;
    s->inv_z = inv_z;
    s->poly = xn_render_poly_next;
}

void xn_span_insert(struct xn_span *prev, s32 x0, s32 x1, s32 inv_z)
{
    struct xn_span *node = prev->next;

    for (;;) {
        s32 ns, d, slope;

        /* spans that end before x0: on to the next (16-bit compares, as the asm's) */
        if ((s16)node->x_end <= (s16)x0) {
            prev = node;
            node = node->next;
            continue;
        }
        ns = node->x_start;
        if (x1 <= ns) {                         /* all of it before this span */
            insert_node(prev, node, x0, x1, inv_z);
            return;
        }
        slope = node->poly->inv_z_dx;
        d = ns - x0;
        if ((s16)x1 < (s16)node->x_end) {
            /* the new span ends inside this one */
            if (ns < x0) {                      /* ... and starts inside it */
                struct xn_span *a, *b;

                if (-d * slope + node->inv_z >= inv_z)
                    return;                     /* behind it */
                /* in front: the span splits in three, the new one in the middle */
                a = xn_render_span_next;
                b = a + 1;
                xn_render_span_next += 2;
                b->inv_z = inv_z;
                *a = *node;
                b->x_start = (u16)x0;
                b->x_end = (u16)x1;
                b->poly = xn_render_poly_next;
                node->x_end = (u16)x0;
                node->next = b;
                a->x_start = (u16)x1;
                b->next = a;
                /* (a 16-bit difference in the low half of x1, as the asm's sub bx) */
                a->inv_z += (s32)(((u32)x1 & 0xFFFF0000u) | (u16)(x1 - ns)) * slope;
                return;
            }
            if (ns == x0 ? inv_z < node->inv_z ||
                           inv_z == node->inv_z && xn_span_dzdx <= slope
                         : d * xn_span_dzdx + inv_z < node->inv_z) {
                /* behind it where it starts: the new span stops there (or is hidden) */
                if (ns == x0)
                    return;
                x1 = (x1 & 0xFFFF0000) | (u16)ns;
                insert_node(prev, node, x0, x1, inv_z);
                return;
            }
            /* in front where it starts: the span now starts at x1 */
            node->x_start = (u16)x1;
            node->inv_z -= (ns - x1) * slope;
            insert_node(prev, node, x0, x1, inv_z);
            return;
        }
        /* the new span reaches past this one's end */
        if (ns < x0) {                          /* this one starts first */
            if (-d * slope + node->inv_z < inv_z) {
                node->x_end = (u16)x0;          /* behind at x0: it ends there */
                prev = node;
                node = node->next;
                continue;
            }
        } else if (ns == x0 ? node->inv_z < inv_z ||
                              node->inv_z == inv_z && slope <= xn_span_dzdx
                            : d * xn_span_dzdx + inv_z >= node->inv_z) {
            node = node->next;                  /* hidden under the new span: dropped (the
                                                   next insert links past it) */
            continue;
        } else if (ns > x0) {                   /* in front where it starts: the new span's
                                                   part before it */
            s32 ne;

            insert_node(prev, node, x0, (s32)node->x_start, inv_z);
            ne = node->x_end;
            if (x1 <= ne)
                return;
            inv_z += (ne - x0) * xn_span_dzdx;
            x0 = ne;
            prev = node;
            node = node->next;
            continue;
        }
        /* this one is in front at x0: the new span goes on from its end, if it gets there */
        if ((s16)x1 <= (s16)node->x_end)
            return;
        inv_z += (node->x_end - x0) * xn_span_dzdx;
        x0 = node->x_end;
        prev = node;
        node = node->next;
    }
}

/* asm: esi = the row's head, ebp = x0, ebx = x1, ecx = 1/z at x0 */
void xn_span_insert_r(xn_regs *r)
{
    xn_span_insert((struct xn_span *)r->esi, r->ebp, r->ebx, r->ecx);
}
