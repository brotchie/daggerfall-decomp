/* dcompute.c: the tables of XnGine's data that code generates (xndata.h; docs/engine/data.md).
   Each comes out equal to FALL.EXE's byte for byte (tools/xn_datagen.py check); where the
   original's table is not what its formula gives, the difference is kept and said here. The
   engine's own init functions fill the others at run time (xn_mem_init: the reciprocals and
   the fill table; xn_render_init: the squares, the texture size masks, the depth
   reciprocals...). */
#include "xndata.h"
#include "xmath.h"
#include "xpoly.h"
#include "xcollide.h"
#include "xtex.h"

/* ---- the sine ------------------------------------------------------------------------------- */

#define PI 3.14159265358979323846

/* sin(a) for 0 <= a <= pi/2, by its series (the 20th term is below 1e-40) */
static double sine(double a)
{
    double term = a, sum = a, a2 = a * a;
    int n;

    for (n = 1; n < 20; n++) {
        term = -term * a2 / (double)((2 * n) * (2 * n + 1));
        sum += term;
    }
    return sum;
}

/* v rounded to the nearest integer, without a float-to-integer conversion (Watcom's needs its
   run-time library): v + 1.5 * 2^52 has the integer in its low dword; the 80x87's rounding
   to 64 bits first may leave it one off, which the test of v - r corrects (no value here is
   within 10^-4 of a half) */
static s32 nearest(double v)
{
    union {
        double d;
        u32 w[2];
    } u;
    s32 r;

    u.d = v + 6755399441055744.0;
    r = (s32)u.w[0];
    if (v - r > 0.5)
        r++;
    else if (v - r < -0.5)
        r--;
    return r;
}

/* round(sin(pi j / 1024) * 2^28) for 0 <= j <= 512: the first quadrant in 2.28 */
static s32 quadrant(s32 j)
{
    return nearest(sine(j * (PI / 1024.0)) * 268435456.0);
}

/* xn_sin_table[k] = sin(2 pi k / 2048) in 2.28 for k = 0..2559 (xn_cos_table is the same
   table from k = 512), from the first quadrant by the symmetries of the sine. Entry 1718
   (sin of -330/2048 of a turn, exactly -227665571.50015) is -227665571 in the original, one
   more than its mirror -xn_sin_table[330]: the original's table, kept (Q-DATA-01). */
static void sine_table(void)
{
    s32 k, j;

    for (k = 0; k < XN_ANGLES + XN_ANGLES / 4; k++) {
        j = k & XN_ANGLE_MASK;
        if (j <= 512)
            xn_sin_table[k] = quadrant(j);
        else if (j <= 1024)
            xn_sin_table[k] = quadrant(1024 - j);
        else if (j <= 1536)
            xn_sin_table[k] = -quadrant(j - 1024);
        else
            xn_sin_table[k] = -quadrant(2048 - j);
    }
    xn_sin_table[1718] += 1;            /* Quirk Q-DATA-01 */
}

/* ---- square roots and offsets --------------------------------------------------------------- */

/* xn_math_sqrt_table[i] = round(4 sqrt(i)) for i = 0..4095, at most 255 (i >= 4081 would
   round to 256) */
static void sqrt_table(void)
{
    s32 i, r = 0, v;

    for (i = 0; i < 4096; i++) {
        while ((r + 1) * (r + 1) <= 16 * i)     /* r = floor(4 sqrt(i)) */
            r++;
        v = 4 * r * r + 4 * r + 1 <= 64 * i ? r + 1 : r;   /* 4 sqrt(i) >= r + 1/2 */
        xn_math_sqrt_table[i] = (u8)(v > 255 ? 255 : v);
    }
}

/* ---- the polygon rings ---------------------------------------------------------------------- */

#define RING_LAST   28                  /* the largest polygon with a ring */

/* One clipper buffer's rings from t: three lead entries (v0 v1 v2), then ring n for
   n = 3..28, in full 3n + 1 vertex addresses: v0..v(n-1) three times, then vn. The
   rasteriser's walker reads ring n at -n..2n-1 (from the top vertex, forward for the left
   edge and back for the right): the entries before a ring are the last of the one before (or
   the lead). The last ring holds its first 2n = 56 entries, all the walker reads. Returns
   the end. */
static struct xn_poly_vertex **ring_block(struct xn_poly_vertex **t, struct xn_poly_vertex *v,
                                          struct xn_poly_vertex ***ring)
{
    s32 n, k, size;

    for (k = 0; k < 3; k++)
        *t++ = &v[k];
    for (n = 3; n <= RING_LAST; n++) {
        size = n < RING_LAST ? 3 * n + 1 : 2 * n;
        ring[n] = t;
        for (k = 0; k < size; k++)
            t[k] = &v[k < 3 * n ? k % n : n];
        t += size;
    }
    return t;
}

/* xn_poly_ring_a and _b (the rings of buffers A and B by vertex count, 0 below 3) and the
   rings themselves, as the asm lays them out: two blocks of 1216 entries, each the buffer's
   rings and 7 unused entries (the asm's padding). The asm's ring_a is 26 entries long and its
   [26..28] are ring_b's [0..2], which ring b never uses: ring_b keeps them. */
static void polygon_rings(void)
{
    struct xn_poly_vertex **t;
    s32 k;

    t = ring_block(xn_poly_ring_tables, xn_poly_vertex_buf_a, xn_poly_ring_a);
    ring_block(t + 7, xn_poly_vertex_buf_b, xn_poly_ring_b);
    for (k = 0; k < 3; k++)
        xn_poly_ring_b[k] = xn_poly_ring_a[26 + k];
}

/* xn_face_edge_tables[n] for n = 3..24 (0 below): an n-gon's point slot offsets 0, 8, ..,
   8(n - 1) and 0 again (the edge from the last point back to the first), in
   xn_face_edge_slots: each table's closing 0 is the next one's first slot */
static void face_edges(void)
{
    s32 *s = xn_face_edge_slots;
    s32 n, k;

    for (n = 3; n <= 24; n++) {
        xn_face_edge_tables[n] = s;
        for (k = 0; k < n; k++)
            *s++ = 8 * k;
    }
    *s = 0;                             /* the last table's closing slot */
}

void xn_data_compute(void)
{
    s32 k;

    sine_table();
    sqrt_table();
    for (k = 0; k < 512; k++)
        xn_tex_record_offsets[k] = 20 * k;      /* a record's 20-byte directory entry */
    polygon_rings();
    face_edges();
}
