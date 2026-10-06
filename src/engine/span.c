/* span.c: XnGine's span routines and its S-buffer (xspan.h). */
#include "xspan.h"
#include "xlight.h"
#include "xtmap.h"
#include "xnsmc.h"

/* z << 8 from the 1/z table at inv_z (2^40 / z): entry (inv_z >> 13) & FFFFh, << 9 */
#define Z_OF(inv_z)     ((s32)(xn_render_recip_table[((u32)(inv_z) >> 13) & 0xFFFF] << 9))

/* a packed texture coordinate's texel (v in the high half, u in the low half, 8.8 each):
   v's integer byte, then u's */
#define TEXEL(uv)       ((((uv) >> 16) & 0xFF00) | (((uv) >> 8) & 0xFF))

/* colour c through a shade row: the row's address (a shade: its low byte is a fraction) with
   the low byte replaced */
#define ROW(row, c)     (*(const u8 *)(((u32)(row) & ~0xFFu) | (c)))

/* the polygon's packed texture origin (+18h: u in the low word, v in the high one) */
#define TEX_ORIGIN(poly) (*(const u32 *)&(poly)->tex_u0)

/* the camera's y ray of the row being drawn, and its x ray over 16 pixels (2^22 / focal x) */
#define RAY_Y()         (xn_cam_dir_y_mid[xn_render_row_y])
#define RAY_DX16        (xn_cam_dir_x_mid[16])

/* ---- solid colour ----------------------------------------------------------------------- */

/* the first n & 3 pixels one by one, then the colour dword (the asm's rep stosb, rep stosd) */
static void fill(u8 *pix, s32 n, u32 colour4)
{
    u32 k;

    for (k = 0; k < (u32)(n & 3); k++)
        *pix++ = (u8)colour4;
    for (k = 0; k < (u32)n >> 2; k++, pix += 4)
        *(u32 *)pix = colour4;
}

void xn_span_solid(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    (void)span;
    (void)xs;
    fill(pix, n, poly->colour4);
}

void xn_span_solid_shaded_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                s32 n, u8 *pix)
{
    poly->span_fn = xn_span_solid;
    poly->colour4 = xn_colour_fill_table[ROW(poly->shade_row, (u8)poly->colour4)];
    xn_span_solid(poly, span, xs, n, pix);
}

/* ---- solid, lit per pixel ------------------------------------------------------------------ */

/* z << 8 from 1/z by an exact divide: 2^46 / inv_z (0 for inv_z < 4000h, Q-SPAN-04) */
static s32 lit_z(u32 inv_z)
{
    return (s32)xn_udiv64_or0(0x4000, 0, inv_z);
}

void xn_span_solid_lit_tail(u8 *pix, s32 n, u32 base, u32 shade, s32 step)
{
    s32 k;

    for (k = 0; k < n; k++) {
        /* Quirk Q-SPAN-01: only the shade's bits 8-15 move the address; its top half stays
           the start shade's (base, whose low byte is the colour), so a carry out of bit 15
           is lost */
        pix[k] = *(const u8 *)((base & 0xFFFF00FFu) | (shade & 0xFF00u));
        shade += step;
    }
}

void xn_span_solid_lit(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    const struct xn_light_shader *shader = poly->shader;
    s32 ray_y = RAY_Y();
    s32 ray_x = xn_cam_dir_x_mid[xs];
    u32 inv_z = span->inv_z;
    u8 colour = (u8)poly->colour4;
    s32 start, shade;

    start = xn_light_shade(shader, ray_y, ray_x, lit_z(inv_z));
    if (n >= 16) {
        s32 blocks = n >> 4;
        s32 dz16 = poly->inv_z_step * 2;

        do {
            u32 base = ((u32)start & ~0xFFu) | colour;
            u32 sh = start;
            s32 step, k;

            inv_z += dz16;
            ray_x += RAY_DX16;
            shade = xn_light_shade(shader, ray_y, ray_x, lit_z(inv_z));
            step = (shade - start) >> 3;
            /* one lookup for each pixel pair */
            for (k = 0; k < 16; k += 2) {
                u8 c = *(const u8 *)((base & 0xFFFF00FFu) | (sh & 0xFF00u));    /* Q-SPAN-01 */

                sh += step;
                pix[k] = c;
                pix[k + 1] = c;
            }
            start = shade;
            pix += 16;
        } while (--blocks != 0);
        n &= 15;
        if (n == 0)
            return;
    }
    /* the last n pixels: the shade at x + n, the step divided by n */
    ray_x += xn_cam_dir_x_mid[n];
    shade = xn_light_shade(shader, ray_y, ray_x, lit_z(n * poly->inv_z_dx + inv_z));
    xn_span_solid_lit_tail(pix, n, ((u32)start & ~0xFFu) | colour, start,
                           (s32)((shade - start) * xn_recip16_table[n]) >> 16);
}

/* ---- textured -------------------------------------------------------------------------------- */

/* a packed step from the u and v differences over a block of 2^shift pixels: v's in the high
   half (shl 16 - shift: the difference / 2^shift in 8.8), u's below (sar) */
static u32 uv_step(s32 dv, s32 du, int shift)
{
    return ((u32)dv << (16 - shift) & 0xFFFF0000u) | (u16)(du >> shift);
}

/* n texels from uv, wrapped by mask after every step; row: 0, or the shade row they go
   through */
static u32 tex_run(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels, const u8 *row)
{
    s32 k;

    for (k = 0; k < n; k++) {
        u8 c = texels[TEXEL(uv)];

        uv = (uv + step) & mask;
        pix[k] = row ? ROW(row, c) : c;
    }
    return uv;
}

void xn_span_tex_tail(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels)
{
    tex_run(pix, n, uv, step, mask, texels, 0);
}

void xn_span_tex_shaded_tail(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels,
                             const u8 *row)
{
    tex_run(pix, n, uv, step, mask, texels, row);
}

/* The four textured routines: blocks of 2^shift pixels; row: the polygon's shade row for the
   shaded ones, else 0. */
static void tex_span(int shift, const u8 *row, struct xn_poly *poly, const struct xn_span *span,
                     s32 xs, s32 n, u8 *pix)
{
    s32 block = 1 << shift;
    u32 inv_z = span->inv_z;
    s32 z = Z_OF(inv_z);
    u32 mask = poly->wrap_mask;
    s32 v_num = xs * poly->v_dx + xn_render_row_y * poly->v_dy + poly->v_c;
    s32 u_num = xs * poly->u_dx + xn_render_row_y * poly->u_dy + poly->u_c;
    s32 v_prev = xn_mulhi(v_num, z);
    s32 u_prev = xn_mulhi(u_num, z);
    s32 u, v;

    if (n >= block) {
        s32 blocks = n >> shift;

        do {
            u32 uv;

            inv_z += poly->inv_z_step;
            v_num += poly->v_step;
            u_num += poly->u_step;
            z = Z_OF(inv_z);
            v = xn_mulhi(z, v_num);
            u = xn_mulhi(z, u_num);
            uv = ((u32)v_prev << 16 | (u16)u_prev) + TEX_ORIGIN(poly);
            tex_run(pix, block, uv & mask, uv_step(v - v_prev, u - u_prev, shift), mask,
                    poly->texels, row);
            v_prev = v;
            u_prev = u;
            pix += block;
        } while (--blocks != 0);
        n &= block - 1;
        if (n == 0)
            return;
    }
    /* the last n pixels: a whole block's step from here, n pixels of it */
    z = Z_OF(inv_z + poly->inv_z_step);
    v = xn_mulhi(poly->v_step + v_num, z);
    u = xn_mulhi(poly->u_step + u_num, z);
    tex_run(pix, n, (((u32)v_prev << 16 | (u16)u_prev) + TEX_ORIGIN(poly)) & mask,
            uv_step(v - v_prev, u - u_prev, shift), mask, poly->texels, row);
}

void xn_span_tex_8(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    tex_span(3, 0, poly, span, xs, n, pix);
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
    poly->span_fn = xn_span_tex_16;
    xn_span_tex_16(poly, span, xs, n, pix);
}

void xn_span_tex_16(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    tex_span(4, 0, poly, span, xs, n, pix);
}

void xn_span_tex_shaded_8(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix)
{
    tex_span(3, poly->shade_row, poly, span, xs, n, pix);
}

void xn_span_tex_shaded_16_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                 s32 n, u8 *pix)
{
    double_steps(poly);
    poly->span_fn = xn_span_tex_shaded_16;
    xn_span_tex_shaded_16(poly, span, xs, n, pix);
}

void xn_span_tex_shaded_16(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                           u8 *pix)
{
    tex_span(4, poly->shade_row, poly, span, xs, n, pix);
}

/* ---- textured, lit per pixel ------------------------------------------------------------------ */

void xn_span_tex_lit_setup(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                           u8 *pix)
{
    u32 *origin = (u32 *)&poly->tex_u0;

    double_steps(poly);
    *origin = *origin << 16 | *origin >> 16;    /* u high, as the mapper takes it */
    poly->span_fn = xn_span_tex_lit;
    xn_span_tex_lit(poly, span, xs, n, pix);
}

/* the polygon's size masks as the mapper takes them (v mask << 8 | u mask) from its packed
   wrap masks (+4Ch: v mask << 24 | FF0000h | u mask << 8 | FFh) */
#define MAPPER_MASK(poly) ((((poly)->wrap_mask >> 16) & 0xFF00) | (((poly)->wrap_mask >> 8) & 0xFF))

void xn_span_tex_lit(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    const struct xn_light_shader *shader = poly->shader;
    const u8 *texels = poly->texels;
    u32 mask = MAPPER_MASK(poly);
    s32 ray_y = RAY_Y();
    s32 ray_x = xn_cam_dir_x_mid[xs];
    u32 inv_z = span->inv_z;
    s32 z = Z_OF(inv_z);
    s32 start = xn_light_shade(shader, ray_y, ray_x, z);
    s32 u_num = xs * poly->u_dx + xn_render_row_y * poly->u_dy + poly->u_c;
    s32 u_prev = xn_mulhi(u_num, z);
    s32 v_num = xs * poly->v_dx + xn_render_row_y * poly->v_dy + poly->v_c;
    s32 v_prev = xn_mulhi(v_num, z);
    s32 u, v, shade;

    if (n >= 16) {
        s32 blocks = n >> 4;

        do {
            ray_x += RAY_DX16;
            u_num += poly->u_step;
            v_num += poly->v_step;
            inv_z += poly->inv_z_step;
            z = Z_OF(inv_z);
            shade = xn_light_shade(shader, ray_y, ray_x, z);
            u = xn_mulhi(z, u_num);
            v = xn_mulhi(z, v_num);
            xn_tmap_draw(pix, 16, ((u32)u_prev << 16 | (u16)v_prev) + TEX_ORIGIN(poly),
                         uv_step(u - u_prev, v - v_prev, 4), start, (shade - start) >> 3,
                         texels, mask);
            u_prev = u;
            v_prev = v;
            start = shade;
            pix += 16;
        } while (--blocks != 0);
        n &= 15;
        if (n == 0)
            return;
    }
    /* Quirk Q-SPAN-02: the last n pixels' shade at x + n but with the 1/z of pixel n - 1 (the
       asm's lea eax, [ebp - 1]); u and v at pixel n; the steps divided by n */
    z = Z_OF(inv_z + (n - 1) * poly->inv_z_dx);
    shade = xn_light_shade(shader, ray_y, ray_x + xn_cam_dir_x_mid[n], z);
    u = xn_mulhi(poly->u_dx * n + u_num, z);
    v = xn_mulhi(poly->v_dx * n + v_num, z);
    xn_tmap_draw(pix, n, ((u32)u_prev << 16 | (u16)v_prev) + TEX_ORIGIN(poly),
                 ((u32)((u - u_prev) * xn_recip16_table[n]) & 0xFFFF0000u) |
                     (u16)((s32)((v - v_prev) * xn_recip16_table[n]) >> 16),
                 start, (shade - start) >> 3, texels, mask);
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

/* the step over the last n pixels: u's difference (the low half, signed) times FFFFFFFFh / n
   (the high dword), v's (the high half) times FFFFh / n */
static u32 step64_n(u32 end, u32 uv, s32 n)
{
    u32 d = end - uv;

    return ((u32)(((s32)d >> 16) * xn_recip16_table[n]) & 0xFFFF0000u) |
           (u16)xn_mulhi((s16)d, xn_recip32_table[n]);
}

/* n texels: the offset wrapped by mask (3F3Fh), the coordinate itself never wrapped (its
   carries run on into the other half); row: 0 or the shade row */
static u32 tex64_run(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels,
                     const u8 *row)
{
    s32 k;

    for (k = 0; k < n; k++) {
        u8 c = texels[TEXEL(uv) & mask];

        uv += step;
        pix[k] = row ? ROW(row, c) : c;
    }
    return uv;
}

void xn_span_tex64_tail(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels)
{
    tex64_run(pix, n, uv, step, mask, texels, 0);
}

void xn_span_tex64_shaded_tail(u8 *pix, s32 n, u32 uv, u32 step, u32 mask, const u8 *texels,
                               const u8 *row)
{
    tex64_run(pix, n, uv, step, mask, texels, row);
}

/* the 1/z table's entry for inv_z (not << 9: the products' low dwords) */
#define Z64_OF(inv_z)   (xn_render_recip_table[((u32)(inv_z) >> 13) & 0xFFFF])

static void tex64_span(const u8 *row, struct xn_poly *poly, const struct xn_span *span, s32 xs,
                       s32 n, u8 *pix)
{
    u32 inv_z = span->inv_z;
    s32 v_num = xs * poly->v_dx + poly->v_c + xn_render_row_y * poly->v_dy;
    s32 u_num = xs * poly->u_dx + xn_render_row_y * poly->u_dy + poly->u_c;
    s32 z = Z64_OF(inv_z);
    u32 uv = pack64(v_num * z, u_num * z) + TEX_ORIGIN(poly);

    if (n >= 16) {
        s32 blocks = n >> 4;

        do {
            inv_z += poly->inv_z_step;
            u_num += poly->u_step;
            v_num += poly->v_step;
            z = Z64_OF(inv_z);
            uv = tex64_run(pix, 16, uv,
                           step64_16(pack64(v_num * z, u_num * z) + TEX_ORIGIN(poly), uv),
                           0x3F3F, poly->texels, row);
            pix += 16;
        } while (--blocks != 0);
        n &= 15;
        if (n == 0)
            return;
    }
    /* the last n pixels: divided exactly at the span's end */
    inv_z += n * poly->inv_z_dx;
    z = Z64_OF(inv_z);
    tex64_run(pix, n, uv,
              step64_n(pack64((poly->v_dx * n + v_num) * z, (poly->u_dx * n + u_num) * z) +
                       TEX_ORIGIN(poly), uv, n),
              0x3F3F, poly->texels, row);
}

void xn_span_tex64(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n, u8 *pix)
{
    tex64_span(0, poly, span, xs, n, pix);
}

void xn_span_tex64_shaded(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                          u8 *pix)
{
    tex64_span(poly->shade_row, poly, span, xs, n, pix);
}

/* ---- flats ------------------------------------------------------------------------------------ */

/* The flat's coordinate at screen column x, its per-pixel step and the remainder of an
   8-pixel DDA: one divide (the 1/z table at inv_z >> 13, unmasked; low-dword products). The
   step is the per-pixel difference d (v's half from dv * z, u's from (du * z) >> 16) >> 3 a
   half, and d & 70007h is added once after every 8 pixels: an exact DDA over 8. */
static u32 flat_setup(const struct xn_flat *f, s32 x, u32 inv_z, u32 *step, u32 *rem)
{
    s32 xs = x - (xn_cam_centre_x - 1);
    s32 z = xn_render_recip_table[inv_z >> 13];
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

static u32 flat_run(int how, u8 *pix, s32 n, u32 uv, u32 step, const u8 *texels,
                    const u8 *row, const u8 *table)
{
    s32 k;

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
static void flat_span(int how, const struct xn_flat *f, s32 x, u32 inv_z, s32 n, u8 *pix,
                      const u8 *row, const u8 *table)
{
    u32 step, rem;
    u32 uv = flat_setup(f, x, inv_z, &step, &rem);

    if (n >= 8) {
        s32 blocks = n >> 3;

        do {
            uv = flat_run(how, pix, 8, uv, step, f->texels, row, table) + rem;
            pix += 8;
        } while (--blocks != 0);
        n &= 7;
    }
    flat_run(how, pix, n, uv, step, f->texels, row, table);
}

void xn_span_flat_transparent(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix)
{
    flat_span(FLAT_PLAIN, flat, x, inv_z, n, pix, 0, 0);
}

void xn_span_flat_transparent_shaded(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix)
{
    flat_span(FLAT_SHADED, flat, x, inv_z, n, pix, flat->shade_row, 0);
}

void xn_span_flat_lit_fogged(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix)
{
    flat_span(FLAT_FOGGED, flat, x, inv_z, n, pix, flat->shade_row, flat->table);
}

void xn_span_flat_translucent(struct xn_flat *flat, u32 inv_z, s32 x, s32 n, u8 *pix)
{
    flat_span(FLAT_TRANSLUCENT, flat, x, inv_z, n, pix, 0, flat->table);
}

void xn_span_flat_transparent_tail(u8 *pix, s32 n, u32 uv, u32 step, const u8 *texels)
{
    flat_run(FLAT_PLAIN, pix, n, uv, step, texels, 0, 0);
}

void xn_span_flat_transparent_shaded_tail(u8 *pix, s32 n, u32 uv, u32 step, const u8 *texels,
                                          const u8 *row)
{
    flat_run(FLAT_SHADED, pix, n, uv, step, texels, row, 0);
}

void xn_span_flat_lit_fogged_tail(u8 *pix, s32 n, u32 uv, u32 step, const u8 *texels,
                                  const u8 *row, const u8 *fog)
{
    flat_run(FLAT_FOGGED, pix, n, uv, step, texels, row, fog);
}

void xn_span_flat_translucent_tail(u8 *pix, s32 n, u32 uv, u32 step, const u8 *texels,
                                   const u8 *table)
{
    flat_run(FLAT_TRANSLUCENT, pix, n, uv, step, texels, 0, table);
}

/* ---- the S-buffer ------------------------------------------------------------------------------ */

/* a new node [x0, x1) of the polygon being built, 1/z inv_z at x0, between prev and next. The
   ends are stored as one dword x0 << 16 | x1 (all of x1's bits: the asm's). */
static void insert_node(struct xn_span *prev, struct xn_span *next, s32 x0, s32 x1, s32 inv_z)
{
    struct xn_span *s = xn_render_span_next++;

    prev->next = s;
    s->next = next;
    *(u32 *)&s->x_end = (u32)x0 << 16 | (u32)x1;
    s->inv_z = inv_z;
    s->poly = xn_render_poly_next;
}

void xn_span_insert(struct xn_span *head, s32 x0, s32 x1, s32 inv_z)
{
    struct xn_span *prev = head;
    struct xn_span *node = prev->next;

    for (;;) {
        s32 ns, d, slope;

        /* Quirk Q-SPAN-03: the ends are compared as 16-bit values, as the asm's */
        if ((s16)node->x_end <= (s16)x0) {      /* ends before x0: on to the next */
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
                /* Q-SPAN-03: a 16-bit difference in the low half of x1 (the asm's sub bx) */
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
