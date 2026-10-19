/* glcap.c: the OpenGL renderer's capture (port/include/port_gl.h), on the game's thread,
   compiled with the engine (its dialect and its headers). The engine calls the hooks below
   (each under DAGGER_PORT in src/engine) as it builds a frame:

   - xn_glcap_begin_frame (xn_render_begin_frame): the polygon pool starts again;
   - xn_glcap_poly / _face / _terrain (poly.c, terrain.c): a polygon's camera-space vertices,
     after clipping, as the pool record that is being built (xn_render_poly_next) has them;
   - xn_glcap_tex (poly.c): a textured face's image, on its first span;
   - xn_glcap_flat (flat.c): a flat's quad and image;
   - xn_glcap_pre / xn_glcap_end_frame (rframe.c): the screen before the 3D pass, and the frame
     drawn: every polygon that got a span routine (one the S-buffer showed somewhere) becomes
     triangles, every flat whose light setup ran a quad;
   - xn_glcap_present (gfx.c): the frame is handed to the main thread.

   The texel coordinates and shades are the span routines' (span.c), evaluated at each vertex:
   GL's perspective-correct interpolation is linear in u/z and 1/z across the screen, as the
   engine's gradients are, so the texels land where the engine puts them. */
#include <stddef.h>
#include <string.h>

#include "xngine.h"
#include "xnstruct.h"
#include "xpoly.h"
#include "xrender.h"
#include "xrframe.h"
#include "xspan.h"
#include "xlight.h"
#include "xshade.h"
#include "xgfx.h"
#include "xcam.h"
#include "xflat.h"
#include "xwshare.h"
#include "port_gl.h"

extern struct xn_vert_cam xn_vert_cam[1024];        /* indexed by byte offset (index * 12) */

#define POOL 1000
#define MAX_CAM 40000

enum { CAP_WORLD = 1, CAP_FLAT };

/* what the hooks saw of each pool record this frame */
static struct cap {
    unsigned int gen;
    int kind;
    int first, n;                       /* camera-space vertices in cam[] */
    const struct xn_tex_image *image;   /* a textured face's or a flat's */
} caps[POOL];
static s32 cam[MAX_CAM][3];
static int ncam;
static unsigned int gen = 1;
static int scene_done;
static int vertex_light = -1;           /* PORT_GL_VERTEX_LIGHT=1: light shaders per vertex */
static unsigned long serial;

static int on(void)
{
    return pgl_enabled && xn_gfx_width == PGL_W && xn_gfx_height == PGL_H;
}

static struct cap *cap_of(const void *record)
{
    ptrdiff_t i = (const struct xn_poly *)record - xn_render_poly_pool;

    if (i < 0 || i >= POOL)
        return 0;
    return &caps[i];
}

static struct cap *begin(const void *record, int kind, int n)
{
    struct cap *c = cap_of(record);

    if (c == 0 || n < 3 || ncam + n > MAX_CAM)
        return 0;
    c->gen = gen;
    c->kind = kind;
    c->first = ncam;
    c->n = n;
    c->image = 0;
    ncam += n;
    return c;
}

/* ---- the hooks --------------------------------------------------------------------------- */

void xn_glcap_begin_frame(void)
{
    gen++;
    ncam = 0;
}

void xn_glcap_poly(const void *record, const struct xn_poly_vertex *v, int n)
{
    struct cap *c;
    int k;

    if (!on() || (c = begin(record, CAP_WORLD, n)) == 0)
        return;
    for (k = 0; k < n; k++) {
        cam[c->first + k][0] = v[k].x;
        cam[c->first + k][1] = v[k].y;
        cam[c->first + k][2] = v[k].z;
    }
}

void xn_glcap_face(const void *record, const struct xn_model_face *face)
{
    struct cap *c;
    int k;

    if (!on() || (c = begin(record, CAP_WORLD, face->point_count)) == 0)
        return;
    for (k = 0; k < c->n; k++) {
        const struct xn_vert_cam *p =
            &XN_AT(struct xn_vert_cam, xn_vert_cam, face->points[k].vertex);

        cam[c->first + k][0] = p->x;
        cam[c->first + k][1] = p->y;
        cam[c->first + k][2] = p->z;
    }
}

void xn_glcap_terrain(const void *record, u32 v, const u32 *corner, int n)
{
    struct cap *c;
    int k;

    if (!on() || (c = begin(record, CAP_WORLD, n)) == 0)
        return;
    for (k = 0; k < n; k++) {
        const struct xn_vert_cam *p = &xn_vert_cam[v + corner[k]];

        cam[c->first + k][0] = p->x;
        cam[c->first + k][1] = p->y;
        cam[c->first + k][2] = p->z;
    }
}

void xn_glcap_tex(const void *record, const struct xn_tex_image *image)
{
    struct cap *c = cap_of(record);

    if (c && c->gen == gen)
        c->image = image;
}

void xn_glcap_flat(const struct xn_flat *flat, const struct xn_tex_image *image,
                   const struct xn_poly_vertex *quad)
{
    struct cap *c;
    int k;

    if (!on() || (c = begin(flat, CAP_FLAT, 4)) == 0)
        return;
    for (k = 0; k < 4; k++) {
        cam[c->first + k][0] = quad[k].x;
        cam[c->first + k][1] = quad[k].y;
        cam[c->first + k][2] = quad[k].z;
    }
    c->image = image;
}

void xn_glcap_pre(void)
{
    if (!on())
        return;
    memcpy(pgl_building()->pre, screen_buffer, PGL_W * PGL_H);
}

/* ---- the frame ------------------------------------------------------------------------------ */

static pgl_scene *S;

static pgl_vertex *more_verts(int n)
{
    if (S->nverts + n > S->cap_verts) {
        int cap = S->cap_verts ? S->cap_verts * 2 : 65536;

        while (cap < S->nverts + n)
            cap *= 2;
        S->verts = pgl_realloc(S->verts, (size_t)cap * sizeof *S->verts);
        S->cap_verts = cap;
    }
    S->nverts += n;
    return S->verts + S->nverts - n;
}

static pgl_draw *new_draw(int mode, int tex, int layer)
{
    pgl_draw *d;

    if (S->ndraws == S->cap_draws) {
        S->cap_draws = S->cap_draws ? S->cap_draws * 2 : 4096;
        S->draws = pgl_realloc(S->draws, (size_t)S->cap_draws * sizeof *S->draws);
    }
    d = &S->draws[S->ndraws++];
    d->first = S->nverts;
    d->count = 0;
    d->mode = mode;
    d->tex = tex;
    d->colour = 0;
    d->row2 = -1;
    d->fog = 0;
    d->layer = layer;
    d->nlights = 0;
    return d;
}

/* ---- textures: an image's pixels by a hash of them, remembered per frame by address ------ */

static struct memo {
    unsigned int gen;
    const u8 *texels;
    int w, h, id;
} memo[4096];

static uint64_t hash_pixels(const u8 *p, int w, int h)
{
    uint64_t x = 0xCBF29CE484222325ull ^ ((uint64_t)w << 32 | (uint64_t)h);
    int y, k;

    for (y = 0; y < h; y++, p += 256) {
        for (k = 0; k + 8 <= w; k += 8) {
            uint64_t q;

            memcpy(&q, p + k, 8);
            x = (x ^ q) * 0x100000001B3ull;
            x ^= x >> 29;
        }
        for (; k < w; k++)
            x = (x ^ p[k]) * 0x100000001B3ull;
    }
    return x;
}

static int texture(const u8 *texels, int w, int h)
{
    unsigned int j = (unsigned int)(((uptr)texels >> 4) * 2654435761u + (unsigned)(w * 31 + h)) &
                     4095;
    struct memo *m;
    uint64_t hash;
    int id;

    if (texels == 0 || w <= 0 || h <= 0)
        return -1;
    for (;; j = (j + 1) & 4095) {
        m = &memo[j];
        if (m->gen != gen)
            break;
        if (m->texels == texels && m->w == w && m->h == h)
            return m->id;
    }
    hash = hash_pixels(texels, w, h);
    id = pgl_texture_id(hash);
    if (id < 0)
        id = pgl_texture_add(hash, w, h, texels, 256);
    m->gen = gen;
    m->texels = texels;
    m->w = w;
    m->h = h;
    m->id = id;
    return id;
}

/* ---- rows of the shade and haze tables ------------------------------------------------------- */

static const u8 *haze_base(void)
{
    return xn_fog_table_last ? xn_fog_table_last - 63 * 256 : 0;
}

/* a row pointer (a shade, with its fraction in the low byte) as a row of the scene's tables */
static float row_of(const u8 *row)
{
    const u8 *haze = haze_base();

    if (row >= xn_shade_table && row < xn_shade_table + 64 * 256)
        return (float)(row - xn_shade_table) / 256.0f;
    if (haze && row >= haze && row < haze + 64 * 256)
        return 64.0f + (float)(row - haze) / 256.0f;
    return -1.0f;
}

/* ---- the world's polygons ------------------------------------------------------------------- */

enum { K_MODEL, K_MODEL_SWAPPED, K_TERRAIN, K_SOLID };

typedef struct how {
    int kind;                   /* how its texels are found (K_*) */
    int lit;                    /* a light shader per pixel */
    float shade;                /* the polygon's one row, or -1 */
    const struct xn_poly *poly;
} how;

static void vertex(const how *h, double x, double y, double z, pgl_vertex *out)
{
    const struct xn_poly *p = h->poly;
    double xs = x * xn_cam_half_width / z - 0.5;    /* the span routines' x and row */
    double ys = y * xn_cam_half_height / z - 0.5;
    double un = xs * p->u_dx + ys * p->u_dy + p->u_c;
    double vn = xs * p->v_dx + ys * p->v_dy + p->v_c;
    double u0 = (double)p->tex_u0 / 256.0, v0 = (double)p->tex_v0 / 256.0;

    out->x = (float)x;
    out->y = (float)y;
    out->z = (float)z;
    switch (h->kind) {
    case K_MODEL:           /* (u/z gradient) * (z << 6) >> 32: 8.8 texels */
        out->u = (float)(un * z / 17179869184.0 + u0);
        out->v = (float)(vn * z / 17179869184.0 + v0);
        break;
    case K_MODEL_SWAPPED:   /* the lit routine's origin: u in the high word */
        out->u = (float)(un * z / 17179869184.0 + v0);
        out->v = (float)(vn * z / 17179869184.0 + u0);
        break;
    case K_TERRAIN:         /* (u/z gradient) * (z >> 3), low dwords: bits 16-31 8.8 */
        out->u = (float)(un * z / 134217728.0 + u0);
        out->v = (float)(vn * z / 134217728.0 + v0);
        break;
    default:
        out->u = out->v = 0;
        break;
    }
    out->shade = h->shade;
    if (h->lit) {
        /* the light shader at this pixel: its rays (k << 18) / focal, z << 6 */
        s32 ray_x = (s32)(xs * 262144.0 / xn_cam_focal_x);
        s32 ray_y = (s32)(ys * 262144.0 / xn_cam_focal_y);
        double zz = z * 64.0;

        if (zz > 2147483647.0)
            zz = 2147483647.0;
        out->shade = row_of((const u8 *)xn_light_shade(p->shader, ray_y, ray_x, (s32)zz));
    }
}

static void screen_of(const pgl_vertex *v, float *sx, float *sy)
{
    *sx = v->x * (float)xn_cam_half_width / v->z;
    *sy = v->y * (float)xn_cam_half_height / v->z;
}

static void mid(const how *h, const pgl_vertex *a, const pgl_vertex *b, pgl_vertex *m)
{
    vertex(h, ((double)a->x + b->x) / 2, ((double)a->y + b->y) / 2, ((double)a->z + b->z) / 2, m);
}

/* a triangle; a lit one is split until its sides are short on the screen, so the light is
   sampled about as often as the span routines sample it (every 16 pixels) */
static void triangle(pgl_draw *d, const how *h, const pgl_vertex *a, const pgl_vertex *b,
                     const pgl_vertex *c, int depth)
{
    if (h->lit && depth < 5) {
        float ax, ay, bx, by, cx, cy, l;

        screen_of(a, &ax, &ay);
        screen_of(b, &bx, &by);
        screen_of(c, &cx, &cy);
        l = (ax - bx) * (ax - bx) + (ay - by) * (ay - by);
        if ((bx - cx) * (bx - cx) + (by - cy) * (by - cy) > l)
            l = (bx - cx) * (bx - cx) + (by - cy) * (by - cy);
        if ((cx - ax) * (cx - ax) + (cy - ay) * (cy - ay) > l)
            l = (cx - ax) * (cx - ax) + (cy - ay) * (cy - ay);
        if (l > 12.0f * 12.0f) {
            pgl_vertex ab, bc, ca;

            mid(h, a, b, &ab);
            mid(h, b, c, &bc);
            mid(h, c, a, &ca);
            triangle(d, h, a, &ab, &ca, depth + 1);
            triangle(d, h, &ab, b, &bc, depth + 1);
            triangle(d, h, &ca, &bc, c, depth + 1);
            triangle(d, h, &ab, &bc, &ca, depth + 1);
            return;
        }
    }
    {
        pgl_vertex *v = more_verts(3);

        v[0] = *a;
        v[1] = *b;
        v[2] = *c;
        d->count += 3;
    }
}

/* A ground cell the S-buffer never showed (no span: at 320x200 it is thinner than a pixel, or
   behind something) still had its first span's setup to come. At the window's resolution a
   distant one can be seen, and leaving it out shows the sky through the ground, so it is set
   up here as xn_render_span_setup_terrain would (the texels, and the lighting: its shade row,
   from the cell's own light list), without drawing. Returns the span routine the setup would
   have chosen, or 0. */
static xn_span_fn finish_ground(struct xn_poly *p, xn_span_fn fn)
{
    struct xn_tex_image *image;
    s32 kind;

    if (p->tex == 0 || (image = p->tex->current) == 0)
        return 0;
    if (fn == xn_render_span_setup_terrain) {
        p->texels = (u8 *)image + image->data_offset;
        kind = xn_light_setup_terrain(p);
        return kind == 4 ? xn_span_tex64_shaded : xn_span_tex64;
    }
    /* solid colours (the low detail): the fill colour, through the shade row */
    {
        u8 colour = p->tex->solid_colour;

        kind = xn_light_setup_terrain(p);
        p->colour4 = colour;
        if (kind == 4)
            p->colour4 = p->shade_row[colour];
        return xn_span_solid;
    }
}

static void world_poly(int index, int fog)
{
    struct xn_poly *p = &xn_render_poly_pool[index];
    const struct cap *c = &caps[index];
    xn_span_fn fn = p->span_fn;
    how h;

    if ((fn == xn_render_span_setup_terrain || fn == xn_render_span_setup_terrain_solid) &&
        (fn = finish_ground(p, fn)) == 0)
        return;
    pgl_draw *d;
    pgl_vertex v[64];
    int k, n, tex = -1, mode = PGL_TEX_WRAP;

    h.poly = p;
    h.lit = 0;
    h.shade = -1.0f;
    if (fn == xn_span_tex_8 || fn == xn_span_tex_16 || fn == xn_span_tex_shaded_8 ||
        fn == xn_span_tex_shaded_16 || fn == xn_span_tex_lit) {
        u32 um = (p->wrap_mask >> 8) & 0xFF, vm = p->wrap_mask >> 24;
        int w = (int)um + 1, ht = (int)vm + 1;

        if (vm == 0xFF && c->image && c->image->height < 256)
            ht = c->image->height;
        h.kind = fn == xn_span_tex_lit ? K_MODEL_SWAPPED : K_MODEL;
        if (fn == xn_span_tex_shaded_8 || fn == xn_span_tex_shaded_16)
            h.shade = row_of(p->shade_row);
        h.lit = fn == xn_span_tex_lit;
        tex = texture(p->texels, w, ht);
    } else if (fn == xn_span_tex64 || fn == xn_span_tex64_shaded) {
        h.kind = K_TERRAIN;
        if (fn == xn_span_tex64_shaded)
            h.shade = row_of(p->shade_row);
        tex = texture(p->texels, 64, 64);
    } else if (fn == xn_span_solid || fn == xn_span_solid_lit) {
        h.kind = K_SOLID;
        h.lit = fn == xn_span_solid_lit;
        mode = PGL_SOLID;
    } else {
        return;                         /* never shown (still its setup), or an outline */
    }
    if (mode == PGL_TEX_WRAP && tex < 0)
        return;
    d = new_draw(mode, tex, 0);
    d->colour = (u8)p->colour4;
    d->fog = fog;
    if (h.lit && !vertex_light) {
        /* the shader goes to the fragment shader whole: no per-vertex samples */
        const struct xn_light_shader *sh = p->shader;

        d->nlights = sh->nlights < 3 ? sh->nlights : 3;
        d->lbase = (float)(sh->row - xn_shade_table);
        for (k = 0; k < d->nlights; k++) {
            d->lights[k][0] = (float)sh->light[k].x;
            d->lights[k][1] = (float)sh->light[k].y;
            d->lights[k][2] = (float)sh->light[k].z;
            d->lights[k][3] = (float)(u32)sh->light[k].intensity;
            d->lights[k][4] = (float)(u32)sh->light[k].falloff;
        }
        h.lit = 0;
    }
    n = c->n < 64 ? c->n : 64;
    for (k = 0; k < n; k++)
        vertex(&h, cam[c->first + k][0], cam[c->first + k][1], cam[c->first + k][2], &v[k]);
    for (k = 1; k + 1 < n; k++)
        triangle(d, &h, &v[0], &v[k], &v[k + 1], 0);
}

/* ---- the flats ---------------------------------------------------------------------------- */

static void flat_quad(const struct xn_flat *f)
{
    const struct cap *c = cap_of(f);
    xn_flat_span_fn fn = (xn_flat_span_fn)f->span;
    const struct xn_tex_image *im;
    pgl_draw *d;
    pgl_vertex q[4], *v;
    float shade = -1.0f;
    int mode = PGL_FLAT, k, w, ht, tex;
    static const int tri[6] = { 0, 1, 2, 0, 2, 3 };

    if (c == 0 || c->gen != gen || c->kind != CAP_FLAT || (im = c->image) == 0)
        return;
    if (fn == xn_span_flat_transparent_shaded)
        shade = row_of(f->shade_row);
    else if (fn == xn_span_flat_lit_fogged)
        shade = row_of(f->shade_row);
    else if (fn == xn_span_flat_translucent)
        mode = PGL_FLAT_TRANSLUCENT;
    else if (fn != xn_span_flat_transparent)
        return;                         /* its light setup never ran: not drawn (Q-FLAT-01) */
    w = im->width;
    ht = im->height;
    if (w <= 0 || ht <= 0 || w > 256 || ht > 256)
        return;
    tex = texture(f->texels, w, ht);
    if (tex < 0)
        return;
    d = new_draw(mode, tex, 1);
    if (fn == xn_span_flat_lit_fogged)
        d->row2 = (int)row_of(f->table);
    for (k = 0; k < 4; k++) {
        q[k].x = (float)cam[c->first + k][0];
        q[k].y = (float)cam[c->first + k][1];
        q[k].z = (float)cam[c->first + k][2];
        q[k].shade = shade;
    }
    /* the corners: top left, top right, bottom right, bottom left (xn_flat_quad_*) */
    q[0].u = 0, q[0].v = 0;
    q[1].u = (float)w, q[1].v = 0;
    q[2].u = (float)w, q[2].v = (float)ht;
    q[3].u = 0, q[3].v = (float)ht;
    if (f->flags & 0x20)
        for (k = 0; k < 4; k++)
            q[k].u = (float)w - q[k].u;
    v = more_verts(6);
    for (k = 0; k < 6; k++)
        v[k] = q[tri[k]];
    d->count = 6;
}

void xn_glcap_end_frame(void)
{
    const u8 *haze;
    int i, fog;

    if (!on())
        return;
    if (vertex_light < 0)
        vertex_light = pgl_vertex_light();
    S = pgl_building();
    S->nverts = 0;
    S->ndraws = 0;
    S->valid = 1;
    S->serial = ++serial;
    S->cx = (float)xn_cam_centre_x;
    S->cy = (float)xn_cam_centre_y;
    S->hw = (float)xn_cam_half_width;
    S->hh = (float)xn_cam_half_height;
    S->near_z = (float)xn_cam_near_z;
    S->far_z = (float)xn_cam_far_z;
    S->focal_x = (float)xn_cam_focal_x;
    S->focal_y = (float)xn_cam_focal_y;
    S->cam_yaw = xn_cam_yaw;
    S->cam_pitch = xn_cam_pitch;
    pgl_mouse_remainder(&S->mouse_rem_x, &S->mouse_rem_y);
    /* the 3D window: the rows the frame loop draws (from the clip top to as far below the
       view centre as that is above it, Q-RENDER-05) within the clip window, and the columns
       of the view within it. Rows below are not the 3D pass's even inside the clip window
       (the HUD stays there from frame to frame) */
    S->clip_l = xn_gfx_clip_left > xn_cam_centre_x - xn_cam_half_width ?
                    xn_gfx_clip_left : xn_cam_centre_x - xn_cam_half_width;
    S->clip_r = xn_gfx_clip_right < xn_cam_centre_x + xn_cam_half_width ?
                    xn_gfx_clip_right : xn_cam_centre_x + xn_cam_half_width;
    S->clip_t = xn_gfx_clip_top;
    S->clip_b = 2 * xn_cam_centre_y - xn_gfx_clip_top;
    if (S->clip_b > xn_gfx_clip_bottom)
        S->clip_b = xn_gfx_clip_bottom;
    if (S->clip_l < 0)
        S->clip_l = 0;
    if (S->clip_r > PGL_W)
        S->clip_r = PGL_W;
    if (S->clip_b > PGL_H)
        S->clip_b = PGL_H;
    S->bg_fill = !(xn_render_frame_flags & 2);
    S->bg_colour = text_shadow_colour;
    fog = xn_render_span_hook == xn_shade_fog_span && xn_fog_table_last != 0;
    S->fog_on = fog;
    S->fog_start = (float)xn_fog_start;
    S->fog_step = (float)xn_fog_step;
    S->fog_far = (float)((u32)xn_cam_far_z >> 8);
    memcpy(S->tables, xn_shade_table, 64 * 256);
    haze = haze_base();
    memcpy(S->tables + 64 * 256, haze ? haze : xn_shade_table, 64 * 256);
    if (xn_light_falloff)
        memcpy(S->falloff, xn_light_falloff, sizeof S->falloff);

    for (i = 1; i < (int)(xn_render_poly_next - xn_render_poly_pool) && i < POOL; i++)
        if (caps[i].gen == gen && caps[i].kind == CAP_WORLD)
            world_poly(i, fog);
    for (i = 0; i < xn_flat_count && i < 512; i++)
        flat_quad((const struct xn_flat *)xn_flat_sort_list[i].value);

    memcpy(S->post, screen_buffer, PGL_W * PGL_H);
    scene_done = 1;
}

/* A frame drawn since the last present is handed over. When the game shows pictures without
   drawing the world (a menu over the frozen view), the last frame stays: the renderer shows
   it only where the screen still has the 3D pass's pixels, and not at all once most of them
   are gone (vpc_gl.c). */
void xn_glcap_present(void)
{
    if (!pgl_enabled)
        return;
    if (scene_done) {
        pgl_publish();
        scene_done = 0;
    }
}
