/* render.c: XnGine's renderer set-up and frame helpers as readable C (xrender.h; see xngine.h
   and docs/xngine_readable.md). xn_render_frame (12A870) and its run-time blocks stay asm: they
   belong to the rasteriser group. */
#include "xrender.h"
#include "xcam.h"
#include "xflat.h"
#include "xmodel.h"
#include "xtex.h"

extern s32 xn_squares_table[8192];      /* (k * k) << 8 for k = -4096..4095 */
extern u8 xn_tex_size_mask[256];
extern s32 *xn_render_recip_table;      /* 2^24 / k, k = 0..65536 */
extern xn_routine *xn_render_mode_tables[3];   /* per render mode: 5 setups (unaligned) */
extern xn_routine xn_render_span_setups[5];
extern xn_routine xn_render_solid_span_fns[3];     /* by lighting kind 0/4/8 */
extern xn_routine xn_render_terrain_span_fns[3];
extern u32 xn_colour_fill_table[256];   /* the colour byte in all four bytes */
extern u8 text_colour, text_shadow_colour;
extern xn_vec3 xn_scratch_vec_b;        /* xn_flat_pick leaves the pick point's x, y here */
extern s32 xn_pick_flat_x, xn_pick_flat_y, xn_pick_flat_z;

/* the frame's lists and counts */
extern struct xn_sort_pair xn_model_queue[200];
extern struct xn_sort_pair *xn_model_queue_ptr;
extern s32 xn_model_queue_count, xn_model_drawn_count;
extern struct xn_sort_pair xn_flat_sort_list[512];
extern struct xn_sort_pair *xn_flat_sort_end;
extern s32 xn_flat_count, xn_flat_drawn_count;
extern s32 xn_light_count;
extern s32 xn_render_unused_54;         /* 0xCEA54: zeroed here, never read */
extern u8 *xn_light_code_next;
extern s32 xn_light_ambient;
extern u8 *xn_shade_table;

/* patch fields */
extern s32 xn_light_ambient_row, xn_flat_ambient_row;   /* 15BC61 155636 */
extern u8 xn_poly_subdiv_shift;                         /* 15BBB9: a shift count byte */
/* the 25 span-routine operands that address the 1/z table ([ecx*4 + TABLE]) */
extern s32 *xn_span_tex64_recip_a, *xn_span_tex64_recip_b, *xn_span_tex64_recip_c;
extern s32 *xn_span_tex64sh_recip_a, *xn_span_tex64sh_recip_b, *xn_span_tex64sh_recip_c;
extern s32 *xn_span_tex8_recip_a, *xn_span_tex8_recip_b, *xn_span_tex8_recip_c;
extern s32 *xn_span_tex16_recip_a, *xn_span_tex16_recip_b, *xn_span_tex16_recip_c;
extern s32 *xn_span_texsh8_recip_a, *xn_span_texsh8_recip_b, *xn_span_texsh8_recip_c;
extern s32 *xn_span_texsh16_recip_a, *xn_span_texsh16_recip_b, *xn_span_texsh16_recip_c;
extern s32 *xn_span_texlit_recip_a, *xn_span_texlit_recip_b, *xn_span_texlit_recip_c;
extern s32 *xn_span_flat_transparent_recip, *xn_span_flat_shaded_recip;
extern s32 *xn_span_flat_fogged_recip, *xn_span_flat_translucent_recip;

/* other groups' functions, through their asm entries (routed to their C when converted) */
extern void asm_xn_sys_install_divide_handler(void);
extern void asm_xn_sys_remove_divide_handler(void);
extern void asm_xn_light_free(void);
void xn_world_shutdown(void);              /* the world group's (xworld.h) */
extern void asm_xn_light_to_view(void);
extern void asm_xn_light_setup_poly(void);
extern void asm_xn_light_setup_terrain(void);
extern void asm_xn_render_frame_row_end(void);  /* the sentinel span routine (12A949) */

/* the game's allocator (Watcom C, object 1) */
void *func_000A10A8(u32 size);
void func_000A117E(void *p);

void xn_render_init(void)
{
    s32 k;
    s32 *table;

    xn_call_asm(asm_xn_sys_install_divide_handler);
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
    xn_span_tex64_recip_a = xn_span_tex64_recip_b = xn_span_tex64_recip_c = table;
    xn_span_tex64sh_recip_a = xn_span_tex64sh_recip_b = xn_span_tex64sh_recip_c = table;
    xn_span_tex8_recip_a = xn_span_tex8_recip_b = xn_span_tex8_recip_c = table;
    xn_span_tex16_recip_a = xn_span_tex16_recip_b = xn_span_tex16_recip_c = table;
    xn_span_texsh8_recip_a = xn_span_texsh8_recip_b = xn_span_texsh8_recip_c = table;
    xn_span_texsh16_recip_a = xn_span_texsh16_recip_b = xn_span_texsh16_recip_c = table;
    xn_span_texlit_recip_a = xn_span_texlit_recip_b = xn_span_texlit_recip_c = table;
    xn_span_flat_transparent_recip = xn_span_flat_shaded_recip = table;
    xn_span_flat_fogged_recip = xn_span_flat_translucent_recip = table;
    /* table[k] = 2^37 / (k << 13) = 2^24 / k, from k = 65536 down to 1; table[0] unwritten */
    for (k = 0x10000; k >= 1; k--)
        table[k] = xn_udiv64(0x20, 0, (u32)k << 13);
}

void xn_render_shutdown(void)
{
    xn_call_asm(asm_xn_light_free);
    xn_tex_cache_free();
    xn_world_shutdown();
    xn_call_asm(asm_xn_sys_remove_divide_handler);
    func_000A117E(xn_render_recip_table);
}

void xn_render_set_mode(s32 mode)
{
    xn_routine *setups = XN_AT(xn_routine *, xn_render_mode_tables, mode);
    int k;

    xn_render_mode = mode;
    for (k = 0; k < 5; k++)
        xn_render_span_setups[k] = setups[k];
}

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

    /* the background polygon and the sentinel span that ends every row */
    background = xn_render_poly_next++;
    background->span_fn = asm_xn_render_frame_row_end;
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
    xn_render_unused_54 = 0;
    xn_light_code_next = big_buffer;

    ambient = xn_light_ambient & ~0xFF;
    if (ambient < 0)
        ambient = 0;
    else if (ambient > 0x3F00)
        ambient = 0x3F00;
    ambient += (s32)xn_shade_table;
    xn_light_ambient_row = ambient;
    xn_flat_ambient_row = ambient;
    xn_poly_subdiv_shift = xn_gfx_width == 320 ? 18 : 25;
}

void *xn_render_pick(s32 x, s32 y, s32 frame)
{
    void *flat;
    struct xn_poly *poly = 0;
    struct xn_span *node;

    if (x < xn_gfx_clip_left || x >= xn_gfx_clip_right ||
        y < xn_gfx_clip_top || y >= xn_gfx_clip_bottom)
        return 0;           /* the asm pops one register too many here and returns through a
                               stack word: the game never asks outside the clip window */
    flat = xn_flat_pick(x, y, frame);
    pick_distance = 0x8000;
    /* the point as xn_flat_pick left it */
    for (node = xn_render_span_rows[xn_scratch_vec_b.y].next; node != node->next;
         node = node->next) {
        s32 px = xn_scratch_vec_b.x;
        s32 inv_z;

        /* signed 16-bit compares; x_end counts as covered (one pixel more than drawn) */
        if ((s16)px < (s16)node->x_start || (s16)node->x_end < (s16)px)
            continue;
        poly = node->poly;
        /* `sub ax, x_start`: only the low word of x is offset */
        px = (px & 0xFFFF0000) | (u16)(px - node->x_start);
        inv_z = px * poly->inv_z_dx + node->inv_z;
        pick_distance = xn_udiv64(1, 0, inv_z);         /* 2^32 / (1/z) */
        xn_pick_view_x = xn_muldiv(xn_scratch_vec_b.x - xn_cam_centre_x, pick_distance,
                                   xn_cam_focal_x);
        xn_pick_view_y = xn_muldiv(xn_scratch_vec_b.y - xn_cam_centre_y, pick_distance,
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

void xn_render_span_setup_solid_r(xn_regs *r)
{
    struct xn_poly *poly = (struct xn_poly *)r->eax;
    xn_routine fn;

    fn = XN_AT(xn_routine, xn_render_solid_span_fns, xn_call_light_setup(asm_xn_light_setup_poly, poly, r));
    poly->span_fn = fn;
    poly->colour4 = xn_colour_fill_table[poly->tex->solid_colour];
    xn_run_span(fn, r);
}

void xn_render_span_setup_terrain_r(xn_regs *r)
{
    struct xn_poly *poly = (struct xn_poly *)r->eax;
    struct xn_tex_image *image = poly->tex->current;
    xn_routine fn;

    poly->texels = (u8 *)image + image->data_offset;
    r->ecx = (u32)poly->texels;         /* the light setup gets it in ECX */
    fn = XN_AT(xn_routine, xn_render_terrain_span_fns,
               xn_call_light_setup(asm_xn_light_setup_terrain, poly, r));
    poly->span_fn = fn;
    poly->u_step = poly->u_dx << 4;     /* the 16-pixel steps */
    poly->v_step = poly->v_dx << 4;
    poly->inv_z_step = poly->inv_z_dx << 4;
    xn_run_span(fn, r);
}

void xn_render_span_setup_terrain_solid_r(xn_regs *r)
{
    struct xn_poly *poly = (struct xn_poly *)r->eax;
    xn_routine fn;

    fn = XN_AT(xn_routine, xn_render_solid_span_fns,
               xn_call_light_setup(asm_xn_light_setup_terrain, poly, r));
    poly->span_fn = fn;
    poly->colour4 = xn_colour_fill_table[poly->tex->solid_colour];
    xn_run_span(fn, r);
}

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

void xn_render_draw_models_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_render_draw_models());
}

int xn_render_draw_flats(void)
{
    struct xn_sort_pair *pair;
    s32 last, left;

    if (xn_flat_count == 0)
        return 0;
    xn_flat_begin_frame();
    xn_call_asm(asm_xn_light_to_view);
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

void xn_render_draw_flats_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, xn_render_draw_flats());
}

u8 xn_render_span_mark_ends(u8 *dst, s32 n)
{
    dst[1] = text_colour;
    dst[n + 1] = text_colour;
    return text_colour;
}

void xn_render_span_mark_ends_r(xn_regs *r)
{
    r->eax = (r->eax & ~0xFFu) | xn_render_span_mark_ends((u8 *)r->edi, r->ebp);
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

void xn_render_sort_pairs_saveregs(struct xn_sort_pair *pairs, s32 lo, s32 hi)
{
    xn_render_sort_pairs(pairs, lo, hi);
}

void xn_render_sort_pairs(struct xn_sort_pair *pairs, s32 lo, s32 hi)
{
    s32 pivot;

    if (hi > lo)
        xn_render_sort_pairs_range(pairs, lo, hi, &pivot);
}

void xn_render_sort_pairs_r(xn_regs *r)
{
    s32 pivot;

    if ((s32)r->ebx > (s32)r->edx) {
        r->esi = xn_render_sort_pairs_range((struct xn_sort_pair *)r->eax, r->edx, r->ebx, &pivot);
        r->ebp = pivot;
    }
}

/* the pair `off` bytes into the list */
#define PAIR(off) XN_AT(struct xn_sort_pair, pairs, off)

s32 xn_render_sort_pairs_range(struct xn_sort_pair *pairs, s32 lo, s32 hi, s32 *pivot)
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
    *pivot = key;
    /* the asm's recursive calls leave their own ESI (and EBP): the right part starts where the
       left part's sort stopped scanning */
    if (lo < j)
        i = xn_render_sort_pairs_range(pairs, lo, j, pivot);
    if (i < hi)
        i = xn_render_sort_pairs_range(pairs, i, hi, pivot);
    return i;
}

void xn_render_sort_pairs_range_r(xn_regs *r)
{
    s32 pivot;

    r->esi = xn_render_sort_pairs_range((struct xn_sort_pair *)r->eax, r->edx, r->ebx, &pivot);
    r->ebp = pivot;
}
