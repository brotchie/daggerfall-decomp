/* light.c: XnGine's lights as readable C (xlight.h; see xngine.h and
   docs/engine/smc/light.md). */
#include "xlight.h"
#include "xnsmc.h"
#include "xmat.h"

void *func_000A10A8(u32 size);                  /* the game's malloc and free */
void func_000A117E(void *block);
#define xn_game_malloc  func_000A10A8
#define xn_game_free    func_000A117E

/* other groups' functions, by their asm entries */
u8 *asm_xn_dos_load_file(const char *name, void *buf);
#pragma aux asm_xn_dos_load_file parm [eax] [edx] value [eax] modify exact [eax edx];
void asm_xn_kbd_remove(void);
#pragma aux asm_xn_kbd_remove modify exact [eax];
void asm_xn_render_shutdown(void);
#pragma aux asm_xn_render_shutdown modify exact [eax];
void xn_gfx_restore_mode(void);
void asm_xn_mem_shutdown(void);
#pragma aux asm_xn_mem_shutdown modify exact [eax edx];
extern void asm_xn_cam_cull_sphere(void);       /* eax ecx edx ebx in; eax and CF out */

extern s32 xn_cam_x, xn_cam_y, xn_cam_z;
extern xn_mat3 xn_cam_rotation;
extern char xn_light_msg_no_memory[];           /* "ENGINE: Out of memory for shaders.$" */

/* the light templates' falloff-table and last-row operands (light.md) */
extern u16 *xn_light_t1_tab, *xn_light_t2_tab1, *xn_light_t2_tab2;
extern u16 *xn_light_t3_tab1, *xn_light_t3_tab2, *xn_light_t3_tab3;

/* ---- the frame's lights -------------------------------------------------------------------- */

void xn_light_init(void)
{
    u8 *block;
    xn_regs r;

    xn_light_reset();
    block = xn_game_malloc(0x8000);
    if (block != 0) {
        xn_shade_table_alloc = block;
        xn_shade_table = (u8 *)(((u32)block + 0x3FFF) & ~0x3FFFu);
        block = xn_game_malloc(0x10020);
        if (block != 0) {
            xn_light_falloff_alloc = block;
            xn_light_falloff = (u16 *)(((u32)block + 0x1F) & ~0x1Fu);
            xn_light_t1_tab = xn_light_t2_tab1 = xn_light_t2_tab2 = xn_light_falloff;
            xn_light_t3_tab1 = xn_light_t3_tab2 = xn_light_t3_tab3 = xn_light_falloff;
            asm_xn_dos_load_file(xn_light_filename, xn_light_falloff);
            return;
        }
    }
    /* out of memory: never in the records (the game exits) */
    asm_xn_kbd_remove();
    asm_xn_render_shutdown();
    xn_gfx_restore_mode();
    asm_xn_mem_shutdown();
    r.eax = 0x0900;
    r.edx = (u32)xn_light_msg_no_memory;
    xn_int21(&r);
    r.eax = 0x4C00;
    xn_int21(&r);
}

/* the rows' outputs are FS and GS (unchanged): glue, which keeps every register */
void xn_light_init_r(xn_regs *r)
{
    (void)r;
    xn_light_init();
}

void xn_light_free(void)
{
    if (xn_shade_table_alloc != 0) {
        xn_game_free(xn_shade_table_alloc);
        xn_game_free(xn_light_falloff_alloc);
    }
}

void xn_light_free_r(xn_regs *r)
{
    (void)r;
    xn_light_free();
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
    return xn_light_add_regs(x, y, z, intensity, radius, type);
}

s32 xn_light_add_regs(s32 x, s32 y, s32 z, s32 intensity, s32 radius, s32 type)
{
    struct xn_light *l;
    xn_regs r;

    if (intensity <= 0)
        return x;
    if ((u32)++xn_light_count >= XN_LIGHTS)
        return x;                               /* full (the count stays one up) */
    l = xn_light_next;
    if (intensity > 32)
        intensity = 32;
    l->type = type;
    l->intensity = intensity;
    l->x = x;
    l->y = y;
    l->z = z;
    if (type != 8) {                            /* a point light: culled by its sphere */
        if (radius > 0x200)
            radius = 0x200;
        l->reach = (s32)((u32)intensity >> 1) * radius;
        l->range_sq = radius * radius * (s32)((u32)intensity >> 1);
        r.eax = (x - xn_cam_x) << 8;
        r.edx = (y - xn_cam_y) << 8;
        r.ebx = (z - xn_cam_z) << 8;
        r.ecx = l->reach << 6;
        xn_asmcall(asm_xn_cam_cull_sphere, &r);
        if (r.eflags & XN_CF) {
            xn_light_count--;                   /* not seen: the slot is not used */
            return r.eax;
        }
        x = r.eax;
    }
    xn_light_next = (struct xn_light *)((u8 *)l + sizeof(struct xn_light));
    return x;
}

/* asm: eax, edx, ebx = x, y, z; ecx = intensity; ebp = radius; esi = type */
void xn_light_add_regs_r(xn_regs *r)
{
    r->eax = xn_light_add_regs(r->eax, r->edx, r->ebx, r->ecx, r->ebp, r->esi);
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

/* ---- the shader templates ------------------------------------------------------------------ */

/* Where a template's operands are (offsets from its start; docs/engine/smc/
   patch_fields.csv, group LIGHT-SHADERS). Per light i: the three squares-table bases
   (&SQ[light x], y, z: the shader reads base[pixel x]), the falloff operand (the address of
   the polygon's shader_falloff[i]), the falloff table, and the intensity. Then the base row
   and the clamp's two operands (compared, then stored). */
typedef struct xn_shader_layout {
    u8 size;                            /* bytes the builder copies */
    u8 nlights;
    u8 sq_x[3], sq_y[3], sq_z[3];
    u8 falloff_p[3], tab[3], intensity[3];
    u8 row, max_a, max_b;
} xn_shader_layout;

static const xn_shader_layout shader_layout[3] = {
    { 0x54, 1, { 0x15 }, { 0x0B }, { 0x21 }, { 0x27 }, { 0x37 }, { 0x3D }, 0x1A, 0x48, 0x4F },
    { 0x8C, 2, { 0x15, 0x31 }, { 0x0E, 0x2A }, { 0x1C, 0x23 }, { 0x37, 0x5F }, { 0x4E, 0x6F },
      { 0x54, 0x75 }, 0x3C, 0x80, 0x87 },
    { 0xCC, 3, { 0x15, 0x31, 0x46 }, { 0x0E, 0x2A, 0x3F }, { 0x1C, 0x23, 0x38 },
      { 0x4C, 0x56, 0x60 }, { 0x75, 0x90, 0xAB }, { 0x7B, 0x96, 0xB1 }, 0x65, 0xBC, 0xC3 },
};

static u8 *shader_template(int nlights)
{
    return nlights == 1 ? xn_light_tmpl_1 : nlights == 2 ? xn_light_tmpl_2 : xn_light_tmpl_3;
}

/* a template's dword operand at offset off */
#define OPERAND(code, off)  (*(u32 *)((code) + (off)))

s32 xn_light_build_shader(struct xn_poly *poly, int nlights)
{
    const xn_shader_layout *l = &shader_layout[nlights - 1];
    u8 *t = shader_template(nlights);
    u8 *copy = xn_light_code_next;
    int i;

    for (i = 0; i < nlights; i++) {
        OPERAND(t, l->sq_x[i]) = (u32)&xn_squares_table_mid[xn_light_point_x[i]];
        OPERAND(t, l->sq_y[i]) = (u32)&xn_squares_table_mid[xn_light_point_y[i]];
        OPERAND(t, l->sq_z[i]) = (u32)&xn_squares_table_mid[xn_light_point_z[i]];
        OPERAND(t, l->intensity[i]) = xn_light_point_intensity[i];
        poly->shader_falloff[i] = xn_light_point_falloff[i];
        OPERAND(t, l->falloff_p[i]) = (u32)&poly->shader_falloff[i];
    }
    OPERAND(t, l->row) = (u32)xn_light_shade_row;
    poly->shader = (void (*)(void))copy;
    for (i = 0; i < l->size; i++)
        copy[i] = t[i];
    xn_light_code_next = copy + l->size;
    return 8;
}

s32 xn_light_build_shader_1(struct xn_poly *poly)
{
    return xn_light_build_shader(poly, 1);
}

s32 xn_light_build_shader_2(struct xn_poly *poly)
{
    return xn_light_build_shader(poly, 2);
}

s32 xn_light_build_shader_3(struct xn_poly *poly)
{
    return xn_light_build_shader(poly, 3);
}

/* asm: edi = poly; the flags come back as they were (the stub keeps them). The other
   registers come back as the asm leaves them: ECX 0 and ESI the template's end (rep movsd),
   EDX the copy, EBX the last falloff operand's value. */
static void build_shader_r(xn_regs *r, int nlights)
{
    struct xn_poly *poly = (struct xn_poly *)r->edi;
    u8 *copy = xn_light_code_next;

    r->eax = xn_light_build_shader(poly, nlights);
    r->ecx = 0;
    r->edx = (u32)copy;
    r->ebx = (u32)&poly->shader_falloff[nlights - 1];
    r->esi = (u32)shader_template(nlights) + shader_layout[nlights - 1].size;
}

void xn_light_build_shader_1_r(xn_regs *r)
{
    build_shader_r(r, 1);
}

void xn_light_build_shader_2_r(xn_regs *r)
{
    build_shader_r(r, 2);
}

void xn_light_build_shader_3_r(xn_regs *r)
{
    build_shader_r(r, 3);
}

s32 xn_light_shade_eval(const u8 *shader, s32 ray_y, s32 ray_x, s32 z)
{
    const xn_shader_layout *l;
    s32 y = xn_mulhi(ray_y, z), x = xn_mulhi(ray_x, z);
    u32 zz = (u32)z >> 14;
    u32 h[3];
    s32 shade, added = 0;
    int i;

    /* which template the copy is: its third instruction (mov ebx, edx in the one-light one),
       and the 2- and 3-light ones by the register of their second sum at +20h */
    if (shader[2] == 0x8B && shader[3] == 0xDA)
        l = &shader_layout[0];
    else
        l = &shader_layout[shader[0x21] == 0x0C ? 1 : 2];
    for (i = 0; i < l->nlights; i++) {
        u32 d2 = ((const u32 *)OPERAND(shader, l->sq_y[i]))[y] +
                 ((const u32 *)OPERAND(shader, l->sq_x[i]))[x] +
                 ((const u32 *)OPERAND(shader, l->sq_z[i]))[zz];
        h[i] = xn_umulhi(d2, *(const u32 *)OPERAND(shader, l->falloff_p[i]));
    }
    shade = OPERAND(shader, l->row);
    for (i = 0; i < l->nlights; i++) {
        if ((h[i] & 0xFFFF8000u) == 0) {
            u16 f = ((const u16 *)OPERAND(shader, l->tab[i]))[h[i]];
            shade += (s32)((f * OPERAND(shader, l->intensity[i])) >> 12);
            added = 1;
        }
    }
    if ((l->nlights > 1 || added) && shade > (s32)OPERAND(shader, l->max_a))
        shade = OPERAND(shader, l->max_b);
    return shade;
}

/* ---- the per-polygon setup ----------------------------------------------------------------- */

/* the light types: a byte offset into the dispatch tables */
#define LIGHT_POINT         0
#define LIGHT_IGNORED       4
#define LIGHT_DIRECTIONAL   8

/* a model face's plane distance (its arch3d_plane header's +14h) */
#define FACE_PLANE_D(face)  (*(const s32 *)((const u8 *)(face) + 0x14))

s32 xn_light_shade_constant(struct xn_poly *poly)
{
    poly->shade_row = xn_light_shade_row;
    return 4;
}

void xn_light_shade_constant_r(xn_regs *r)
{
    r->eax = xn_light_shade_constant((struct xn_poly *)r->edi);
}

int xn_light_point_reaches(struct xn_poly *poly, const struct xn_light_ref *ref, int slot)
{
    const s32 *n = poly->normal;
    s32 d, hi, intensity, falloff, s;
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
    intensity = (s32)xn_udiv64(0, ref->range_sq, hi) * ref->intensity;
    if (intensity <= 0x100)
        return 0;
    xn_light_point_intensity[slot] = intensity;
    falloff = (s32)xn_udiv64(0, 0x40000000, hi);
    if (falloff <= 0x80)
        return 0;
    xn_light_point_falloff[slot] = falloff;
    /* the foot point: the light moved back along the normal by d, into view space */
    s = -d << 16;
    h = poly->handle;
    foot.x = xn_mulhi(n[0], s) - ref->x - h->rel_x;
    foot.y = xn_mulhi(n[1], s) - ref->y - h->rel_y;
    foot.z = xn_mulhi(n[2], s) - ref->z - h->rel_z;
    xn_mat_transform(&foot, (const xn_mat3 *)((const struct xn_model_matrix_slot *)h->matrix +
                                              200));    /* the pool's light[] copy */
    xn_light_point_x[slot] = foot.x >> 8;
    xn_light_point_y[slot] = foot.y >> 8;
    xn_light_point_z[slot] = foot.z >> 8;
    return 1;
}

int xn_light_add_directional(const struct xn_poly *poly, const struct xn_light_ref *ref)
{
    const s32 *n = poly->normal;
    s32 xy = ref->x * n[0] + ref->y * n[1], z = ref->z * n[2];

    if (!xn_add_lt0(xy, z))
        return 0;                               /* facing away (the add's sign, unwrapped) */
    xn_light_shade_row += ((u32)(-(xy + z) * ref->intensity) >> 15) & ~0xFFu;
    return (s32)xn_shade_table_last_row <= (s32)xn_light_shade_row;
}

int xn_light_terrain_add_directional(const struct xn_poly *poly, const struct xn_light_ref *ref)
{
    s32 xy = ref->x * poly->nx + ref->y * poly->ny, z = ref->z * poly->nz;

    if (!xn_add_lt0(xy, z))
        return 0;
    xn_light_shade_row += ((xy + z) * ref->intensity >> 15) & ~0xFF;
    return (s32)xn_shade_table_last_row <= (s32)xn_light_shade_row;
}

/* asm: esi = the light ref, edi = the polygon, ebp = the point slots used * 4; CF and EBP 0
   to stop */
void xn_light_add_directional_r(xn_regs *r)
{
    int stop = xn_light_add_directional((const struct xn_poly *)r->edi,
                                        (const struct xn_light_ref *)r->esi);

    if (stop)
        r->ebp = 0;
    XN_SETFLAG(r, XN_CF, stop);
}

void xn_light_terrain_add_directional_r(xn_regs *r)
{
    int stop = xn_light_terrain_add_directional((const struct xn_poly *)r->edi,
                                                (const struct xn_light_ref *)r->esi);

    if (stop)
        r->ebp = 0;
    XN_SETFLAG(r, XN_CF, stop);
}

void xn_light_add_type4_noop(void)
{
}

void xn_light_terrain_add_point_noop(void)
{
}

void xn_light_terrain_add_type4_noop(void)
{
}

/* the end of a light list: a -1 light */
#define LIST_END(ref)   ((s32)(ref)->light == -1)

s32 xn_light_setup_poly(struct xn_poly *poly)
{
    const struct xn_light_ref *ref;
    u8 *row;
    int npoints = 0;

    poly->handle->flags |= 2;                   /* the model reached the rasterizer */
    poly->dx_per_inv_z = xn_idiv64(1, 0, poly->inv_z_dx);      /* 0 for 0: a divide error */
    row = (u8 *)((u32)poly->face->shade << 8) + (u32)xn_light_ambient_row;
    if ((u32)row >= (u32)xn_shade_table_last_row)
        return 0;
    xn_light_shade_row = row;
    for (ref = poly->handle->lights; !LIST_END(ref); ref++) {
        switch (ref->light->type) {
        case LIGHT_POINT:
            if (xn_light_point_reaches(poly, ref, npoints) && ++npoints == 3)
                return xn_light_build_shader(poly, 3);
            break;
        case LIGHT_DIRECTIONAL:
            if (xn_light_add_directional(poly, ref))
                return 0;
            break;
        case LIGHT_IGNORED:
            break;                              /* (the asm's dispatch table has only these
                                                   three entries; the game adds no other type) */
        }
    }
    return npoints ? xn_light_build_shader(poly, npoints) : xn_light_shade_constant(poly);
}

s32 xn_light_setup_terrain(struct xn_poly *poly)
{
    const struct xn_light_ref *ref;

    poly->dx_per_inv_z = xn_idiv64(1, 0, poly->inv_z_dx);
    if ((u32)xn_light_ambient >= 0x3F00)
        return 0;
    xn_light_shade_row = xn_shade_table + xn_light_ambient;
    /* the terrain's dispatch table ignores point lights and type 4 */
    for (ref = poly->light_list; !LIST_END(ref); ref++)
        if (ref->light->type == LIGHT_DIRECTIONAL && xn_light_terrain_add_directional(poly, ref))
            return 0;
    return xn_light_shade_constant(poly);
}
