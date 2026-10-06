/* drawline.c: XnGine's lines and clipped rectangles (canonical C; the interface and the
   module's documentation are in xdraw.h). */
#include "xdraw.h"

static void swap(s32 *a, s32 *b)
{
    s32 t = *a;

    *a = *b;
    *b = t;
}

/* the line's ends exchanged */
static void swap_ends(xn_line *l)
{
    swap(&l->x1, &l->x2);
    swap(&l->y1, &l->y2);
}

void xn_draw_hline(s32 x1, s32 y, s32 x2)
{
    if (y < xn_gfx_clip_top || y >= xn_gfx_clip_bottom)
        return;
    if (x1 > x2)
        swap(&x1, &x2);
    if (x2 <= xn_gfx_clip_left || x1 >= xn_gfx_clip_right)
        return;
    if (x1 < xn_gfx_clip_left)
        x1 = xn_gfx_clip_left;
    if (x2 > xn_gfx_clip_right)
        x2 = xn_gfx_clip_right;
    if (x2 - x1 <= 0)
        return;
    xn_fill_bytes(XN_SCREEN(x1, y), text_colour, x2 - x1);     /* Quirk Q-DRAW-07: [x1, x2) */
}

void xn_draw_vline_keep_regs(s32 x, s32 y1, s32 y2)
{
    xn_draw_vline(x, y1, y2);
}

void xn_draw_vline(s32 x, s32 y1, s32 y2)
{
    u8 *row;
    s32 rows;

    if (x < xn_gfx_clip_left || x >= xn_gfx_clip_right)
        return;
    if (y1 > y2)
        swap(&y1, &y2);
    if (y1 > xn_gfx_clip_bottom || y2 < xn_gfx_clip_top)
        return;
    if (y1 < xn_gfx_clip_top)
        y1 = xn_gfx_clip_top;
    if (y2 > xn_gfx_clip_bottom)
        y2 = xn_gfx_clip_bottom;
    rows = y2 - y1;                     /* Quirk Q-DRAW-07: [y1, y2) */
    if (rows <= 0)
        return;
    row = screen_buffer + xn_gfx_row_offset[y1];
    do {
        row[x] = text_colour;
        row += xn_gfx_width;
    } while (--rows != 0);
}

void xn_draw_line(s32 x1, s32 y1, s32 x2, s32 y2)
{
    xn_line l;

    l.x1 = x1;
    l.y1 = y1;
    l.x2 = x2;
    l.y2 = y2;
    if (xn_draw_line_clip(&l))
        xn_draw_line_unclipped(l.x1, l.y1, l.x2, l.y2);
}

void xn_draw_line_nosave(s32 x1, s32 y1, s32 x2, s32 y2)
{
    xn_draw_line(x1, y1, x2, y2);
}

void xn_draw_line_unclipped(s32 x1, s32 y1, s32 x2, s32 y2)
{
    s32 dx = x2 - x1, dy = y2 - y1;
    s32 adx, ady, n, step;
    u32 pos;                            /* the minor axis in 16.16 */
    u8 *p;

    if (dx == 0) {
        xn_draw_vline(x1, y1, y2);
        return;
    }
    if (dy == 0) {
        xn_draw_hline(x1, y1, x2);
        return;
    }
    adx = dx < 0 ? -dx : dx;
    ady = dy < 0 ? -dy : dy;
    if (adx >= ady) {
        /* x-major: left to right, a row from the 16.16 y per column (Q-DRAW-07: x2 not drawn) */
        if (x1 > x2) {
            swap(&x1, &x2);
            swap(&y1, &y2);
        }
        n = x2 - x1;
        step = (y2 - y1) * (s32)xn_recip16_table[n];
        pos = (u32)y1 << 16;
        p = screen_buffer + x1;
        do {
            p[xn_gfx_row_offset[pos >> 16]] = text_colour;
            pos += step;
            p++;
        } while (--n != 0);
    } else {
        /* y-major: top to bottom, a column from the 16.16 x per row */
        if (y1 > y2) {
            swap(&x1, &x2);
            swap(&y1, &y2);
        }
        n = y2 - y1;
        step = (x2 - x1) * (s32)xn_recip16_table[n];
        pos = (u32)x1 << 16;
        p = screen_buffer + xn_gfx_row_offset[y1];
        do {
            p[pos >> 16] = text_colour;
            pos += step;
            p += xn_gfx_width;
        } while (--n != 0);
    }
}

void xn_draw_line_to(s32 x, s32 y)
{
    xn_draw_line_nosave(pen_x, pen_y, x, y);
}

int xn_draw_line_clip(xn_line *l)
{
    s32 dx = l->x2 - l->x1, dy = l->y2 - l->y1;

    pen_x = l->x2;
    pen_y = l->y2;
    if (l->x1 > l->x2)
        swap_ends(l);
    if (l->x1 >= xn_gfx_clip_right || l->x2 < xn_gfx_clip_left)
        return 0;
    if (dx != 0) {
        if (l->x1 < xn_gfx_clip_left) {
            l->y1 -= xn_muldiv(l->x1 - xn_gfx_clip_left, dy, dx);
            l->x1 = xn_gfx_clip_left;
        }
        if (l->x2 > xn_gfx_clip_right) {
            l->y2 -= xn_muldiv(l->x2 - xn_gfx_clip_right, dy, dx);
            l->x2 = xn_gfx_clip_right - 1;      /* Quirk Q-DRAW-08 */
        }
    }
    if (l->y1 > l->y2)
        swap_ends(l);
    if (l->y1 >= xn_gfx_clip_bottom || l->y2 <= xn_gfx_clip_top)     /* (Q-DRAW-08: <=) */
        return 0;
    if (dy != 0) {
        if (l->y1 < xn_gfx_clip_top) {
            l->x1 -= xn_muldiv(l->y1 - xn_gfx_clip_top, dx, dy);
            l->y1 = xn_gfx_clip_top;
        }
        if (l->y2 > xn_gfx_clip_bottom) {
            l->x2 -= xn_muldiv(l->y2 - xn_gfx_clip_bottom, dx, dy);
            l->y2 = xn_gfx_clip_bottom;         /* Quirk Q-DRAW-08 */
        }
    }
    return 1;
}

int xn_draw_clip_rect_xyxy(xn_line *r)
{
    if (r->x1 >= xn_gfx_clip_right || r->x2 <= xn_gfx_clip_left ||
        r->y1 >= xn_gfx_clip_bottom || r->y2 <= xn_gfx_clip_top)
        return 0;
    if (r->x1 < xn_gfx_clip_left)
        r->x1 = xn_gfx_clip_left;
    if (r->x2 > xn_gfx_clip_right)
        r->x2 = xn_gfx_clip_right;
    if (r->y1 < xn_gfx_clip_top)
        r->y1 = xn_gfx_clip_top;
    if (r->y2 > xn_gfx_clip_bottom)
        r->y2 = xn_gfx_clip_bottom;
    return 1;
}

/* the corners of (x1, y1)-(x2, y2) in order, clipped; 0 when outside */
static int order_and_clip(xn_line *r, s32 x1, s32 y1, s32 x2, s32 y2)
{
    r->x1 = x1;
    r->y1 = y1;
    r->x2 = x2;
    r->y2 = y2;
    if (r->x1 > r->x2)
        swap(&r->x1, &r->x2);
    if (r->y1 > r->y2)
        swap(&r->y1, &r->y2);
    return xn_draw_clip_rect_xyxy(r);
}

void xn_draw_rect_outline(s32 x1, s32 y1, s32 x2, s32 y2)
{
    xn_line r;

    if (!order_and_clip(&r, x1, y1, x2, y2))
        return;
    xn_draw_hline(r.x1, r.y1, r.x2);
    xn_draw_hline(r.x1, r.y2, r.x2);
    xn_draw_vline(r.x1, r.y1, r.y2);
    xn_draw_vline(r.x2 - 1, r.y1, r.y2);
}

void xn_draw_fill_rect_clipped(s32 x1, s32 y1, s32 x2, s32 y2)
{
    xn_line r;
    u8 *row;
    u32 rows, w, fill;

    if (!order_and_clip(&r, x1, y1, x2, y2))
        return;
    row = XN_SCREEN(r.x1, r.y1);
    w = r.x2 - r.x1;
    rows = r.y2 - r.y1;                 /* Quirk Q-DRAW-06: 0 rows run 2^32 times */
    fill = xn_colour_fill_table[text_colour];
    do {
        xn_fill_dwords(row, fill, w >> 2);
        xn_fill_bytes(row + (w & ~3), (u8)fill, w & 3);
        row += xn_gfx_width;
    } while (--rows != 0);
}

void xn_draw_line_text_colour(s32 x1, s32 y1, s32 x2, s32 y2)
{
    s32 dx, dy, major, minor, err, step_neg, step_pos, row_step = 320;
    u32 n;
    u8 *p;

    dx = x2 - x1;
    if (dx == 0) {
        /* vertical: down from the upper end, both ends drawn */
        dy = y2 - y1;
        if (dy < 0) {
            dy = -dy;
            y1 = y2;
        }
        p = screen_buffer + xn_gfx_row_offset[y1] + x1;
        for (n = dy + 1; n != 0; n--) {
            *p = text_colour;
            p += 320;
        }
        return;
    }
    if (dx < 0) {
        /* left to right */
        dx = -dx;
        swap(&x1, &x2);
        swap(&y1, &y2);
    }
    dy = y2 - y1;
    p = screen_buffer + xn_gfx_row_offset[y1] + x1;
    if (dy == 0) {
        xn_fill_bytes(p, text_colour, dx + 1);
        return;
    }
    if (dy < 0) {
        dy = -dy;
        row_step = -320;
    }
    major = dx;
    minor = dy;
    if (dy > dx) {
        major = dy;
        minor = dx;
    }
    step_neg = 2 * minor;               /* the error's step after a major-axis step */
    step_pos = 2 * minor - 2 * major;   /* after a minor-axis step too */
    err = 2 * minor - major;
    n = major + 1;
    if (dy <= dx) {
        /* x-major: a pixel per column, a row on when the error is >= 0 */
        do {
            *p++ = text_colour;
            if (err < 0) {
                err += step_neg;
            } else {
                err += step_pos;
                p += row_step;
            }
        } while (--n != 0);
    } else {
        /* y-major: a pixel per row, a column right when the error is >= 0 */
        do {
            *p = text_colour;
            p += row_step + 1;
            if (err < 0) {
                err += step_neg;
                p--;
            } else {
                err += step_pos;
            }
        } while (--n != 0);
    }
}
