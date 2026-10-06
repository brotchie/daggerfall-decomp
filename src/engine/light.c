/* light.c: XnGine's lights (xlight.h). */
#include "xlight.h"
#include "xnsmc.h"
#include "xmat.h"
#include "xcam.h"
#include "xrender.h"
#include "xdos.h"
#include "xkbd.h"
#include "xgfx.h"
#include "xmem.h"

/* the light types: byte offsets into the asm's dispatch tables */
#define LIGHT_POINT         0
#define LIGHT_IGNORED       4
#define LIGHT_DIRECTIONAL   8

/* a light list ends at a -1 light */
#define LIST_END(ref)       ((s32)(ref)->light == -1)

/* a model face's plane distance (its arch3d_plane header's +14h) */
#define FACE_PLANE_D(face)  (*(const s32 *)((const u8 *)(face) + 0x14))

#define XN_RENDER_POLYS     1000        /* polygons a frame (xn_render_poly_pool) */

u8 *xn_light_ambient_row;

/* the frame's shaders: one at most for each polygon of the frame */
static struct xn_light_shader shader_pool[XN_RENDER_POLYS];
static s32 shader_count;

/* ---- the frame's lights -------------------------------------------------------------------- */

void xn_light_init(void)
{
    u8 *block;

    xn_light_reset();
    block = func_000A10A8(0x8000);
    if (block != 0) {
        xn_shade_table_alloc = block;
        xn_shade_table = (u8 *)(((u32)block + 0x3FFF) & ~0x3FFFu);
        block = func_000A10A8(0x10020);
        if (block != 0) {
            xn_light_falloff_alloc = block;
            xn_light_falloff = (u16 *)(((u32)block + 0x1F) & ~0x1Fu);
            xn_dos_load_file(xn_light_filename, xn_light_falloff);
            return;
        }
    }
    /* out of memory: the engine shut down and the program ended (the asm's exit code is its
       caller's AL, a leftover; 0 here) */
    xn_kbd_remove();
    xn_render_shutdown();
    xn_gfx_restore_mode();
    xn_mem_shutdown();
    xn_dos_print(xn_light_msg_no_memory);
    xn_dos_exit(0);
}

void xn_light_free(void)
{
    if (xn_shade_table_alloc != 0) {
        func_000A117E(xn_shade_table_alloc);
        func_000A117E(xn_light_falloff_alloc);
    }
}

void xn_light_reset(void)
{
    struct xn_light *l;

    for (l = xn_light_table; l < xn_light_table + XN_LIGHTS; l++)
        l->intensity = -1;
    xn_light_next = xn_light_table;
}

s32 xn_light_add(s32 x, s32 y, s32 z, s32 intensity, s32 radius, s32 type)
{
    struct xn_light *l;

    if (intensity <= 0)
        return x;
    /* Quirk Q-LIGHT-02: the count goes up before the full test and stays up */
    if ((u32)++xn_light_count >= XN_LIGHTS)
        return x;
    l = xn_light_next;
    if (intensity > 32)
        intensity = 32;
    l->type = type;
    l->intensity = intensity;
    l->x = x;
    l->y = y;
    l->z = z;
    if (type != LIGHT_DIRECTIONAL) {            /* a point light: culled by its sphere */
        s32 residue;

        if (radius > 0x200)
            radius = 0x200;
        l->reach = (s32)((u32)intensity >> 1) * radius;
        l->range_sq = radius * radius * (s32)((u32)intensity >> 1);
        /* Quirk Q-LIGHT-01: what the cull leaves is the result (the game keeps it) */
        if (xn_cam_cull_sphere((x - xn_cam_x) << 8, (y - xn_cam_y) << 8, (z - xn_cam_z) << 8,
                               l->reach << 6, &residue)) {
            xn_light_count--;                   /* not seen: the slot is not used */
            return residue;
        }
        x = residue;
    }
    xn_light_next = l + 1;
    return x;
}

void xn_light_to_view(void)
{
    struct xn_light *l = xn_light_table;
    s32 n = xn_light_count;
    xn_vec3 v;

    if (n > XN_LIGHTS)
        n = XN_LIGHTS;
    if (n == 0)
        return;
    do {
        v.x = (l->x - xn_cam_x) << 8;
        v.y = (l->y - xn_cam_y) << 8;
        v.z = (l->z - xn_cam_z) << 8;
        xn_mat_transform(&v, &xn_cam_rotation);
        l->x = v.x;
        l->y = v.y;
        l->z = v.z;
        l++;
    } while (--n != 0);
}

/* ---- the shaders ------------------------------------------------------------------------------ */

void xn_light_begin_frame(void)
{
    shader_count = 0;
}

/* Quirk Q-LIGHT-07: the asm compiled each shader into big_buffer (at xn_light_code_next, which
   the frame's start resets to big_buffer), and the game reads big_buffer back through a stale
   pointer (player_movement_update copies a collision hit that the frame's shaders have
   overwritten since), so the shaders' bytes are game-visible. They are written as the asm
   wrote them: its template for 1, 2 or 3 lights (the bytes below, with its operands 0) and
   the shader's operands. Nothing runs them. */
static const u8 tmpl_1[0x54] = {
    0xF7, 0xE9, 0x8B, 0xDA, 0x8B, 0xC5, 0xF7, 0xE9, 0x8B, 0x04, 0x9D, 0x00, 0x00, 0x00, 0x00, 0xC1,
    0xE9, 0x0E, 0x03, 0x04, 0x95, 0x00, 0x00, 0x00, 0x00, 0xBD, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04,
    0x8D, 0x00, 0x00, 0x00, 0x00, 0xF7, 0x25, 0x00, 0x00, 0x00, 0x00, 0xF7, 0xC2, 0x00, 0x80, 0xFF,
    0xFF, 0x75, 0x20, 0x66, 0x8B, 0x14, 0x55, 0x00, 0x00, 0x00, 0x00, 0x69, 0xD2, 0x00, 0x00, 0x00,
    0x00, 0xC1, 0xEA, 0x0C, 0x03, 0xEA, 0x81, 0xFD, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x05, 0xBD, 0x00,
    0x00, 0x00, 0x00, 0xC3,
};
static const u8 tmpl_2[0x8C] = {
    0xF7, 0xE9, 0x8B, 0xC5, 0x8B, 0xDA, 0xF7, 0xE9, 0xC1, 0xE9, 0x0E, 0x8B, 0x04, 0x9D, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x04, 0x95, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x8D, 0x00, 0x00, 0x00, 0x00,
    0x8B, 0x0C, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0C, 0x9D, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0C,
    0x95, 0x00, 0x00, 0x00, 0x00, 0xF7, 0x25, 0x00, 0x00, 0x00, 0x00, 0xBD, 0x00, 0x00, 0x00, 0x00,
    0x8B, 0xC1, 0xF7, 0xC2, 0x00, 0x80, 0xFF, 0xFF, 0x75, 0x13, 0x66, 0x8B, 0x14, 0x55, 0x00, 0x00,
    0x00, 0x00, 0x69, 0xD2, 0x00, 0x00, 0x00, 0x00, 0xC1, 0xEA, 0x0C, 0x03, 0xEA, 0xF7, 0x25, 0x00,
    0x00, 0x00, 0x00, 0xF7, 0xC2, 0x00, 0x80, 0xFF, 0xFF, 0x75, 0x13, 0x66, 0x8B, 0x14, 0x55, 0x00,
    0x00, 0x00, 0x00, 0x69, 0xD2, 0x00, 0x00, 0x00, 0x00, 0xC1, 0xEA, 0x0C, 0x03, 0xEA, 0x81, 0xFD,
    0x00, 0x00, 0x00, 0x00, 0x7E, 0x05, 0xBD, 0x00, 0x00, 0x00, 0x00, 0xC3,
};
static const u8 tmpl_3[0xCC] = {
    0xF7, 0xE9, 0x8B, 0xC5, 0x8B, 0xDA, 0xF7, 0xE9, 0xC1, 0xE9, 0x0E, 0x8B, 0x04, 0x9D, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x04, 0x95, 0x00, 0x00, 0x00, 0x00, 0x03, 0x04, 0x8D, 0x00, 0x00, 0x00, 0x00,
    0x8B, 0x2C, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x03, 0x2C, 0x9D, 0x00, 0x00, 0x00, 0x00, 0x03, 0x2C,
    0x95, 0x00, 0x00, 0x00, 0x00, 0x8B, 0x0C, 0x8D, 0x00, 0x00, 0x00, 0x00, 0x03, 0x0C, 0x9D, 0x00,
    0x00, 0x00, 0x00, 0x03, 0x0C, 0x95, 0x00, 0x00, 0x00, 0x00, 0xF7, 0x25, 0x00, 0x00, 0x00, 0x00,
    0x8B, 0xC5, 0x8B, 0xDA, 0xF7, 0x25, 0x00, 0x00, 0x00, 0x00, 0x8B, 0xC1, 0x8B, 0xCA, 0xF7, 0x25,
    0x00, 0x00, 0x00, 0x00, 0xBD, 0x00, 0x00, 0x00, 0x00, 0xF7, 0xC3, 0x00, 0x80, 0xFF, 0xFF, 0x75,
    0x13, 0x66, 0x8B, 0x1C, 0x5D, 0x00, 0x00, 0x00, 0x00, 0x69, 0xDB, 0x00, 0x00, 0x00, 0x00, 0xC1,
    0xEB, 0x0C, 0x03, 0xEB, 0xF7, 0xC1, 0x00, 0x80, 0xFF, 0xFF, 0x75, 0x13, 0x66, 0x8B, 0x0C, 0x4D,
    0x00, 0x00, 0x00, 0x00, 0x69, 0xC9, 0x00, 0x00, 0x00, 0x00, 0xC1, 0xE9, 0x0C, 0x03, 0xE9, 0xF7,
    0xC2, 0x00, 0x80, 0xFF, 0xFF, 0x75, 0x13, 0x66, 0x8B, 0x14, 0x55, 0x00, 0x00, 0x00, 0x00, 0x69,
    0xD2, 0x00, 0x00, 0x00, 0x00, 0xC1, 0xEA, 0x0C, 0x03, 0xEA, 0x81, 0xFD, 0x00, 0x00, 0x00, 0x00,
    0x7E, 0x05, 0xBD, 0x00, 0x00, 0x00, 0x00, 0xC3, 0x00, 0x00, 0x00, 0x00,
};

/* each template's operands: per light the squares-table bases of the foot point (x, y, z),
   the address of the polygon's falloff field, the falloff table and the intensity; then the
   base row and the clamp (compared, then stored) */
typedef struct asm_image {
    const u8 *code;
    u8 size;                            /* the bytes the asm copied */
    u8 sq_x[3], sq_y[3], sq_z[3];
    u8 falloff_p[3], tab[3], intensity[3];
    u8 row, max_a, max_b;
} asm_image;

static const asm_image images[3] = {
    { tmpl_1, 0x54, { 0x15 }, { 0x0B }, { 0x21 }, { 0x27 }, { 0x37 }, { 0x3D }, 0x1A, 0x48, 0x4F },
    { tmpl_2, 0x8C, { 0x15, 0x31 }, { 0x0E, 0x2A }, { 0x1C, 0x23 }, { 0x37, 0x5F }, { 0x4E, 0x6F },
      { 0x54, 0x75 }, 0x3C, 0x80, 0x87 },
    { tmpl_3, 0xCC, { 0x15, 0x31, 0x46 }, { 0x0E, 0x2A, 0x3F }, { 0x1C, 0x23, 0x38 },
      { 0x4C, 0x56, 0x60 }, { 0x75, 0x90, 0xAB }, { 0x7B, 0x96, 0xB1 }, 0x65, 0xBC, 0xC3 },
};

#define PUT32(p, v)     (*(u32 *)(p) = (u32)(v))

/* the shader's bytes as the asm compiled them, at xn_light_code_next (Q-LIGHT-07) */
static void write_asm_image(const struct xn_poly *poly, const struct xn_light_shader *s)
{
    const asm_image *im = &images[s->nlights - 1];
    u8 *out = xn_light_code_next;
    int i;

    for (i = 0; i < im->size; i++)
        out[i] = im->code[i];
    for (i = 0; i < s->nlights; i++) {
        PUT32(out + im->sq_x[i], &xn_squares_table_mid[s->light[i].x]);
        PUT32(out + im->sq_y[i], &xn_squares_table_mid[s->light[i].y]);
        PUT32(out + im->sq_z[i], &xn_squares_table_mid[s->light[i].z]);
        PUT32(out + im->falloff_p[i], &poly->shader_falloff[i]);
        PUT32(out + im->tab[i], xn_light_falloff);
        PUT32(out + im->intensity[i], s->light[i].intensity);
    }
    PUT32(out + im->row, s->row);
    PUT32(out + im->max_a, xn_shade_table_last_row);
    PUT32(out + im->max_b, xn_shade_table_last_row);
    xn_light_code_next = out + im->size;
}

s32 xn_light_shade(const struct xn_light_shader *shader, s32 ray_y, s32 ray_x, s32 z)
{
    s32 y = xn_mulhi(ray_y, z), x = xn_mulhi(ray_x, z);
    u32 zz = (u32)z >> 14;
    u32 h[XN_LIGHT_POINTS];
    s32 shade = (s32)shader->row;
    int i;

    /* d^2 from the squares table at the foot point less the pixel's point (Quirk Q-LIGHT-04:
       not bounded) */
    for (i = 0; i < shader->nlights; i++) {
        const struct xn_light_point *p = &shader->light[i];
        u32 d2 = (u32)xn_squares_table_mid[p->y + y] + (u32)xn_squares_table_mid[p->x + x] +
                 (u32)xn_squares_table_mid[p->z + (s32)zz];

        h[i] = xn_umulhi(d2, (u32)p->falloff);
    }
    for (i = 0; i < shader->nlights; i++)
        if ((h[i] & 0xFFFF8000u) == 0)
            shade += (s32)((xn_light_falloff[h[i]] * (u32)shader->light[i].intensity) >> 12);
    /* clamped to the last row (the asm's one-light shader clamps only after its light adds
       something: the base row is below the last row, so that is the same) */
    if (shade > (s32)xn_shade_table_last_row)
        shade = (s32)xn_shade_table_last_row;
    return shade;
}

s32 xn_light_build_shader(struct xn_poly *poly, u8 *row, const struct xn_light_point *pts,
                          int n)
{
    struct xn_light_shader *s = &shader_pool[shader_count++];
    int i;

    s->nlights = n;
    s->row = row;
    for (i = 0; i < n; i++)
        s->light[i] = pts[i];
    poly->shader = s;
    write_asm_image(poly, s);
    return 8;
}

/* ---- the per-polygon lighting -------------------------------------------------------------- */

s32 xn_light_shade_constant(struct xn_poly *poly, u8 *row)
{
    poly->shade_row = row;
    return 4;
}

int xn_light_add_point(const struct xn_poly *poly, const struct xn_light_ref *ref,
                       struct xn_light_point *pt)
{
    const s32 *n = poly->normal;
    s32 d, hi, s;
    xn_s64 d2;
    xn_vec3 foot;
    const struct xn_model_handle *h;

    d = n[0] * ref->x + n[2] * ref->z + n[1] * ref->y;
    if (d <= FACE_PLANE_D(poly->face))
        return 0;                               /* behind the face */
    d -= FACE_PLANE_D(poly->face);
    xn_s64_mul(&d2, d, d);
    hi = d2.hi;                                 /* d^2 >> 32 */
    if (hi >= ref->range_sq)
        return 0;                               /* out of range */
    pt->intensity = (s32)xn_udiv64_or0(0, ref->range_sq, hi) * ref->intensity;
    if (pt->intensity <= 0x100)
        return 0;
    pt->falloff = (s32)xn_udiv64_or0(0, 0x40000000, hi);
    if (pt->falloff <= 0x80)
        return 0;
    /* the foot point: the light moved back along the normal by d, into view space */
    s = -d << 16;
    h = poly->handle;
    foot.x = xn_mulhi(n[0], s) - ref->x - h->rel_x;
    foot.y = xn_mulhi(n[1], s) - ref->y - h->rel_y;
    foot.z = xn_mulhi(n[2], s) - ref->z - h->rel_z;
    xn_mat_transform(&foot, (const xn_mat3 *)((const struct xn_model_matrix_slot *)h->matrix +
                                              200));    /* the pool's light[] copy */
    pt->x = foot.x >> 8;
    pt->y = foot.y >> 8;
    pt->z = foot.z >> 8;
    return 1;
}

/* the row has reached the last one: lit no further (a signed compare, as the asm's jg) */
#define FULLY_LIT(row)  ((s32)xn_shade_table_last_row <= (s32)(row))

int xn_light_add_directional(const struct xn_poly *poly, const struct xn_light_ref *ref,
                             u8 **row)
{
    const s32 *n = poly->normal;
    s32 xy = ref->x * n[0] + ref->y * n[1], z = ref->z * n[2];

    if (!xn_add_lt0(xy, z))
        return 0;                               /* facing away (the add's sign, unwrapped) */
    *row += ((u32)(-(xy + z) * ref->intensity) >> 15) & ~0xFFu;
    return FULLY_LIT(*row);
}

int xn_light_terrain_add_directional(const struct xn_poly *poly, const struct xn_light_ref *ref,
                                     u8 **row)
{
    s32 xy = ref->x * poly->nx + ref->y * poly->ny, z = ref->z * poly->nz;

    if (!xn_add_lt0(xy, z))
        return 0;
    *row += ((xy + z) * ref->intensity >> 15) & ~0xFF;
    return FULLY_LIT(*row);
}

/* 2^32 / the polygon's 1/z slope (+60h), 0 for a slope of 0 (Quirk Q-LIGHT-06: the asm's idiv
   faulted, Q-SYS-01); the fog interpolates with it */
static void set_inverse_slope(struct xn_poly *poly)
{
    poly->dx_per_inv_z = xn_idiv64_or0(1, 0, poly->inv_z_dx);
}

s32 xn_light_setup_poly(struct xn_poly *poly)
{
    const struct xn_light_ref *ref;
    struct xn_light_point pts[XN_LIGHT_POINTS];
    u8 *row;
    int npoints = 0;

    poly->handle->flags |= 2;                   /* the model reached the rasterizer */
    set_inverse_slope(poly);
    row = (u8 *)((u32)poly->face->shade << 8) + (u32)xn_light_ambient_row;
    if ((u32)row >= (u32)xn_shade_table_last_row)
        return 0;
    for (ref = poly->handle->lights; !LIST_END(ref); ref++) {
        switch (ref->light->type) {
        case LIGHT_POINT:
            /* Quirk Q-LIGHT-05: the third point light ends the list */
            if (xn_light_add_point(poly, ref, &pts[npoints]) && ++npoints == XN_LIGHT_POINTS)
                return xn_light_build_shader(poly, row, pts, npoints);
            break;
        case LIGHT_DIRECTIONAL:
            if (xn_light_add_directional(poly, ref, &row))
                return 0;
            break;
        case LIGHT_IGNORED:
            break;                              /* (the game adds no other type) */
        }
    }
    return npoints ? xn_light_build_shader(poly, row, pts, npoints)
                   : xn_light_shade_constant(poly, row);
}

s32 xn_light_setup_terrain(struct xn_poly *poly)
{
    const struct xn_light_ref *ref;
    u8 *row;

    set_inverse_slope(poly);
    /* the ambient level as it is (its fraction byte too; negative: unsigned, too high) */
    if ((u32)xn_light_ambient >= 0x3F00)
        return 0;
    row = xn_shade_table + xn_light_ambient;
    /* the terrain's dispatch table ignores point lights and type 4 */
    for (ref = poly->light_list; !LIST_END(ref); ref++)
        if (ref->light->type == LIGHT_DIRECTIONAL &&
            xn_light_terrain_add_directional(poly, ref, &row))
            return 0;
    return xn_light_shade_constant(poly, row);
}
