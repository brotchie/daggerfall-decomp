/* render.c: XnGine's renderer set-up, the frame's start, the pick, the first-span setups and
   the passes (xrender.h; the frame loop is rframe.c). */
#include "xrender.h"
#include "xcam.h"
#include "xflat.h"
#include "xmodel.h"
#include "xtex.h"
#include "xlight.h"
#include "xworld.h"
#include "xsys.h"
#include "xnsmc.h"
#include "xmem.h"

/* group D's textured setup (xpoly.h): lights a textured model face, stores its span
   routine (xn_render_tmap_span_fns) and draws the span */
void xn_poly_setup_textured(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                            u8 *pix);

extern u8 text_colour, text_shadow_colour;

/* a * b / d with a 64-bit product, truncated; 0 when the quotient does not fit (the asm's
   idiv faulted: Q-SYS-01) */
static s32 muldiv_or0(s32 a, s32 b, s32 d)
{
    xn_s64 t;

    xn_s64_mul(&t, a, b);
    return xn_s64_div_or0(&t, d);
}

/* ---- the routine tables ------------------------------------------------------------------ */

/* The first-span setups by render mode (0, 4, 8) and texture entry kind (0, 4, 8, 12, 16) */
static xn_span_fn const mode_setups[3][5] = {
    {   /* outlines */
        xn_render_span_mark_ends, xn_render_span_mark_ends, xn_render_span_mark_ends,
        xn_render_span_mark_ends, xn_render_span_mark_ends
    },
    {   /* solid colours */
        xn_render_span_setup_solid, xn_render_span_setup_solid, xn_render_span_setup_solid,
        xn_render_span_setup_solid, xn_render_span_setup_terrain_solid
    },
    {   /* textured */
        xn_render_span_setup_solid, xn_poly_setup_textured, xn_render_span_mark_ends,
        xn_render_span_mark_ends, xn_render_span_setup_terrain
    }
};

/* The solid colour routines by lighting kind (0, 4, 8) */
static xn_span_fn const solid_span_fns[3] = {
    xn_span_solid, xn_span_solid_shaded_setup, xn_span_solid_lit
};

xn_span_fn const xn_render_tmap_span_fns[6] = {
    xn_span_tex_8, xn_span_tex_shaded_8, xn_span_tex_lit_setup,
    xn_span_tex_16_setup, xn_span_tex_shaded_16_setup, xn_span_tex_lit_setup
};

/* The terrain's by lighting kind (its setup gives 0 or 4) */
static xn_span_fn const terrain_span_fns[3] = {
    xn_span_tex64, xn_span_tex64_shaded, xn_span_tex64_shaded
};

/* ---- start-up, shutdown, the render mode ------------------------------------------------- */

void xn_render_init(void)
{
    s32 k;
    s32 *table;

    xn_sys_install_divide_handler();
    xn_cam_set_view_window(xn_cam_half_width, xn_cam_half_height, xn_cam_centre_x,
                           xn_cam_centre_y);
    xn_cam_set_focal(200, 180);
    for (k = -4096; k != 4096; k++)
        xn_squares_table[k + 4096] = (k * k) << 8;
    for (k = 0; k < 256; k++)
        xn_tex_size_mask[k] = (k & (k - 1)) == 0 ? (u8)(k - 1) : 0xFF;
    xn_render_set_mode(8);

    table = (s32 *)func_000A10A8(0x40004);
    xn_render_recip_table = table;
    /* table[k] = 2^24 / k (the asm's 2^37 / (k << 13)), from k = 65536 down to 1; table[0]
       is never written */
    for (k = 0x10000; k >= 1; k--)
        table[k] = 0x1000000 / k;
}

void xn_render_shutdown(void)
{
    xn_light_free();
    xn_tex_cache_free();
    xn_world_shutdown();
    xn_sys_remove_divide_handler();
    func_000A117E(xn_render_recip_table);
}

void xn_render_set_mode(s32 mode)
{
    xn_render_mode = mode;
}

xn_span_fn xn_render_span_setup(s32 kind)
{
    return mode_setups[xn_render_mode >> 2][kind >> 2];
}

/* ---- the frame --------------------------------------------------------------------------- */

void xn_render_begin_frame(void)
{
    struct xn_poly *background;
    struct xn_span *head;
    u32 rows;
    s32 ambient;

    xn_render_matrix_next = xn_render_matrix_pool.view;
    xn_render_poly_next = xn_render_poly_pool;
    xn_render_light_list_next = xn_render_light_list_pool;
    xn_model_queue_ptr = xn_model_queue;
    xn_flat_sort_end = xn_flat_sort_list;

    /* the background polygon, and the sentinel span that ends every row (it is its own next;
       the frame loop stops there, and no routine draws it) */
    background = xn_render_poly_next++;
    background->span_fn = 0;
    xn_render_span_sentinel.next = &xn_render_span_sentinel;
    xn_render_span_sentinel.x_start = (u16)xn_gfx_clip_right;
    xn_render_span_sentinel.x_end = (u16)(xn_gfx_clip_right + 1);
    xn_render_span_sentinel.poly = background;
    head = xn_render_span_rows;
    rows = xn_gfx_height;
    do {                                /* dec; jne: a height of 0 would run 2^32 rows */
        head->next = &xn_render_span_sentinel;
        head++;
    } while (--rows != 0);
    xn_render_span_next = head;

    xn_light_count = 0;
    xn_model_queue_count = 0;
    xn_model_drawn_count = 0;
    xn_flat_count = 0;
    xn_flat_drawn_count = 0;
    xn_render_poly_count = 0;
    xn_light_begin_frame();
    xn_light_code_next = big_buffer;

    ambient = xn_light_ambient & ~0xFF;
    if (ambient < 0)
        ambient = 0;
    else if (ambient > 0x3F00)
        ambient = 0x3F00;
    xn_light_ambient_row = xn_shade_table + ambient;
}

void *xn_render_pick(s32 x, s32 y)
{
    void *flat;
    struct xn_poly *poly = 0;
    struct xn_span *node;

    if (x < xn_gfx_clip_left || x >= xn_gfx_clip_right ||
        y < xn_gfx_clip_top || y >= xn_gfx_clip_bottom)
        return 0;           /* (the asm pops one register too many here and returns through a
                               stack word: the game never asks outside the clip window) */
    flat = xn_flat_pick(x, y);
    pick_distance = 0x8000;
    /* the point as xn_flat_pick left it */
    for (node = xn_render_span_rows[xn_scratch_vec_b.y].next; node != node->next;
         node = node->next) {
        s32 px = xn_scratch_vec_b.x;
        s32 inv_z;

        /* Quirk Q-RENDER-03: signed 16-bit compares, x_end counted as covered */
        if ((s16)px < (s16)node->x_start || (s16)node->x_end < (s16)px)
            continue;
        poly = node->poly;
        /* (Q-RENDER-03: `sub ax, x_start`, only x's low word offset) */
        px = (px & 0xFFFF0000) | (u16)(px - node->x_start);
        inv_z = px * poly->inv_z_dx + node->inv_z;
        pick_distance = xn_udiv64_or0(1, 0, inv_z);     /* 2^32 / (1/z) */
        xn_pick_view_x = muldiv_or0(xn_scratch_vec_b.x - xn_cam_centre_x, pick_distance,
                                     xn_cam_focal_x);
        xn_pick_view_y = muldiv_or0(xn_scratch_vec_b.y - xn_cam_centre_y, pick_distance,
                                     xn_cam_focal_y);
        break;
    }
    if (flat != 0 && xn_pick_flat_z < pick_distance) {
        xn_pick_view_x = xn_pick_flat_x;
        xn_pick_view_y = xn_pick_flat_y;
        pick_distance = xn_pick_flat_z;
        return flat;
    }
    return poly;
}

/* ---- the first-span setups ------------------------------------------------------------------ */

void xn_render_span_setup_solid(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                s32 n, u8 *pix)
{
    xn_span_fn fn = solid_span_fns[xn_light_setup_poly(poly) >> 2];

    poly->span_fn = fn;
    poly->colour4 = xn_colour_fill_table[poly->tex->solid_colour];
    fn(poly, span, xs, n, pix);
}

void xn_render_span_setup_terrain(struct xn_poly *poly, const struct xn_span *span, s32 xs,
                                  s32 n, u8 *pix)
{
    struct xn_tex_image *image = poly->tex->current;
    xn_span_fn fn;

    poly->texels = (u8 *)image + image->data_offset;
    fn = terrain_span_fns[xn_light_setup_terrain(poly) >> 2];
    poly->span_fn = fn;
    poly->u_step = poly->u_dx << 4;     /* the 16-pixel steps (over the cell's normal, which
                                           the lighting has read) */
    poly->v_step = poly->v_dx << 4;
    poly->inv_z_step = poly->inv_z_dx << 4;
    fn(poly, span, xs, n, pix);
}

void xn_render_span_setup_terrain_solid(struct xn_poly *poly, const struct xn_span *span,
                                        s32 xs, s32 n, u8 *pix)
{
    xn_span_fn fn = solid_span_fns[xn_light_setup_terrain(poly) >> 2];

    poly->span_fn = fn;
    poly->colour4 = xn_colour_fill_table[poly->tex->solid_colour];
    fn(poly, span, xs, n, pix);
}

void xn_render_span_mark_ends(struct xn_poly *poly, const struct xn_span *span, s32 xs, s32 n,
                              u8 *pix)
{
    (void)poly;
    (void)span;
    (void)xs;
    pix[0] = text_colour;
    pix[n] = text_colour;               /* Quirk Q-RENDER-01: one past the span */
}

/* ---- the passes ----------------------------------------------------------------------------- */

int xn_render_draw_models(void)
{
    struct xn_sort_pair *pair;
    s32 last, left;

    if (xn_model_queue_count == 0)
        return 0;
    last = (u8 *)xn_model_queue_ptr - (u8 *)xn_model_queue - 8;
    xn_render_sort_pairs(xn_model_queue, 0, last);
    pair = xn_model_queue;
    left = (u32)last >> 3;
    do {                                /* dec; jns: last / 8 + 1 models */
        if (xn_model_draw((struct xn_model_handle *)pair->value))
            return 1;
        pair++;
    } while (--left >= 0);
    return 0;
}

int xn_render_draw_flats(void)
{
    struct xn_sort_pair *pair;
    s32 last, left;

    if (xn_flat_count == 0)
        return 0;
    xn_flat_begin_frame();
    xn_light_to_view();
    last = (u8 *)xn_flat_sort_end - (u8 *)xn_flat_sort_list - 8;
    xn_render_sort_pairs(xn_flat_sort_list, 0, last);
    pair = xn_flat_sort_list;
    left = (u32)last >> 3;
    do {
        if (xn_flat_draw((struct xn_flat *)pair->value))
            return 1;
        pair++;
    } while (--left >= 0);
    return 0;
}

void xn_render_fill_background(void)
{
    struct xn_span *head;
    u8 *line;
    u32 rows;

    if (xn_render_frame_flags & 2)
        return;
    head = &xn_render_span_rows[xn_gfx_clip_top];
    line = screen_buffer + xn_gfx_row_offset[xn_gfx_clip_top];
    rows = xn_gfx_clip_bottom - xn_gfx_clip_top;
    do {                                /* dec; jne */
        struct xn_span *node = head->next;
        u32 x = 0;

        for (;;) {
            u32 k;

            for (k = x; k < node->x_start; k++)     /* the gap before the span */
                line[k] = text_shadow_colour;
            if (node == node->next)                 /* the sentinel */
                break;
            x = node->x_end;
            node = node->next;
        }
        head++;
        line += xn_gfx_width;
    } while (--rows != 0);
}

void xn_render_sort_pairs(struct xn_sort_pair *pairs, s32 lo, s32 hi)
{
    if (hi > lo)
        xn_render_sort_pairs_range(pairs, lo, hi);
}

/* the pair `off` bytes into the list */
#define PAIR(off) XN_AT(struct xn_sort_pair, pairs, off)

s32 xn_render_sort_pairs_range(struct xn_sort_pair *pairs, s32 lo, s32 hi)
{
    s32 i = lo;
    s32 j = hi;
    s32 key = PAIR(((u32)(lo + hi) >> 4) << 3).key;     /* the middle pair */

    do {
        while (PAIR(i).key < key)
            i += 8;
        while (key < PAIR(j).key)
            j -= 8;
        if (i <= j) {
            struct xn_sort_pair t = PAIR(i);

            PAIR(i) = PAIR(j);
            PAIR(j) = t;
            i += 8;
            j -= 8;
        }
    } while (i <= j);
    /* Quirk Q-RENDER-02: the right part starts where the left part's sort stopped scanning */
    if (lo < j)
        i = xn_render_sort_pairs_range(pairs, lo, j);
    if (i < hi)
        i = xn_render_sort_pairs_range(pairs, i, hi);
    return i;
}
