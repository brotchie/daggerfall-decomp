/* light_t.c: test shims of src/engine/light.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call.

   The asm's light setup passed its state between its steps in object-2 globals: the point
   slots (xn_light_point_*) the dispatch handlers fill and the builders read, the shade row
   being accumulated (xn_light_shade_row), the slot count in EBP. The shims of the steps read
   and write them where a record of the asm starts with them; canonical C keeps them in the
   setup's locals. */
#include "xctest.h"

/* ---- decoding the asm's compiled shaders ------------------------------------------------- */

/* the three templates (light.c's asm: docs/engine/smc/light.md): per light, the operands'
   offsets in the copy (the squares-table bases of the foot point's x, y, z; the address of
   the polygon's falloff; the intensity), then the base row's */
typedef struct shader_layout {
    u8 nlights;
    u8 sq_x[3], sq_y[3], sq_z[3];
    u8 falloff_p[3], intensity[3];
    u8 row;
} shader_layout;

static const shader_layout layouts[3] = {
    { 1, { 0x15 }, { 0x0B }, { 0x21 }, { 0x27 }, { 0x3D }, 0x1A },
    { 2, { 0x15, 0x31 }, { 0x0E, 0x2A }, { 0x1C, 0x23 }, { 0x37, 0x5F }, { 0x54, 0x75 }, 0x3C },
    { 3, { 0x15, 0x31, 0x46 }, { 0x0E, 0x2A, 0x3F }, { 0x1C, 0x23, 0x38 },
      { 0x4C, 0x56, 0x60 }, { 0x7B, 0x96, 0xB1 }, 0x65 },
};

#define OPERAND(code, off)  (*(const u32 *)((const u8 *)(code) + (off)))
#define SQ_INDEX(a)         ((s32)((a) - (u32)xn_squares_table_mid) >> 2)
#define C_REGION            0x11000000u     /* tools/xn_rc.py RBASE: the image's own memory */

const struct xn_light_shader *xc_adopt_shader(struct xn_poly *poly, struct xn_light_shader *tmp)
{
    const struct xn_light_shader *was = poly->shader;
    const u8 *code = (const u8 *)was;
    const shader_layout *l;
    int i;

    if ((u32)code >= C_REGION)
        return was;                     /* a canonical record already */
    /* which template: the third instruction (mov ebx, edx in the one-light one), and the
       two- and three-light ones by the register of their second sum at +20h */
    if (code[2] == 0x8B && code[3] == 0xDA)
        l = &layouts[0];
    else
        l = &layouts[code[0x21] == 0x0C ? 1 : 2];
    tmp->nlights = l->nlights;
    tmp->row = (u8 *)OPERAND(code, l->row);
    for (i = 0; i < l->nlights; i++) {
        tmp->light[i].x = SQ_INDEX(OPERAND(code, l->sq_x[i]));
        tmp->light[i].y = SQ_INDEX(OPERAND(code, l->sq_y[i]));
        tmp->light[i].z = SQ_INDEX(OPERAND(code, l->sq_z[i]));
        tmp->light[i].intensity = OPERAND(code, l->intensity[i]);
        tmp->light[i].falloff = *(const s32 *)OPERAND(code, l->falloff_p[i]);
    }
    poly->shader = tmp;
    return was;
}

void xc_asm_points(struct xn_light_point *pts, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        pts[i].x = xn_light_point_x[i];
        pts[i].y = xn_light_point_y[i];
        pts[i].z = xn_light_point_z[i];
        pts[i].intensity = xn_light_point_intensity[i];
        pts[i].falloff = xn_light_point_falloff[i];
    }
}

/* ---- the frame's lights -------------------------------------------------------------------- */

void xn_light_init_r(xn_regs *r)
{
    (void)r;
    xn_light_init();
}

void xn_light_free_r(xn_regs *r)
{
    (void)r;
    xn_light_free();
}

void xn_light_reset_r(xn_regs *r)
{
    (void)r;
    xn_light_reset();
}

/* x, y, z, intensity in EAX EDX EBX ECX; radius and type on the stack (ret 8) */
void xn_light_add_r(xn_regs *r)
{
    r->eax = xn_light_add(r->eax, r->edx, r->ebx, r->ecx, XN_STACK_ARG(r, 0),
                          XN_STACK_ARG(r, 1));
}

/* the same with the radius in EBP and the type in ESI */
void xn_light_add_regs_r(xn_regs *r)
{
    r->eax = xn_light_add(r->eax, r->edx, r->ebx, r->ecx, r->ebp, r->esi);
}

void xn_light_to_view_r(xn_regs *r)
{
    (void)r;
    xn_light_to_view();
}

/* ---- the per-polygon lighting -------------------------------------------------------------- */

/* EDI = the polygon -> EAX = the lighting kind */
void xn_light_setup_poly_r(xn_regs *r)
{
    r->eax = xn_light_setup_poly((struct xn_poly *)r->edi);
}

void xn_light_setup_terrain_r(xn_regs *r)
{
    r->eax = xn_light_setup_terrain((struct xn_poly *)r->edi);
}

/* the row being accumulated is the asm's xn_light_shade_row */
void xn_light_shade_constant_r(xn_regs *r)
{
    r->eax = xn_light_shade_constant((struct xn_poly *)r->edi, xn_light_shade_row);
}

/* the builders: the asm's slots and row (they keep the flags: the stub does) */
static void build_shader_r(xn_regs *r, int n)
{
    struct xn_light_point pts[3];

    xc_asm_points(pts, n);
    r->eax = xn_light_build_shader((struct xn_poly *)r->edi, xn_light_shade_row, pts, n);
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

/* xn_light_add_point's asm entry, a handler of the setup's dispatch table: EDI the polygon,
   ESI the light ref, EBP the slots filled * 4. A light that reaches the face fills slot
   EBP / 4 (the asm's slots) and EBP goes up by 4; CF clear. When that was the third, the asm
   drops its own return address and jumps into the three-light builder, which returns to the
   light setup's caller with EAX = 8: xn_light_add_point_r (lightp_t.asm) makes that return
   when this returns 1, with the registers set here. */
int xc_light_add_point(xn_regs *r)
{
    struct xn_light_point pt, pts[3];
    struct xn_poly *poly = (struct xn_poly *)r->edi;
    int slot = r->ebp >> 2;

    XN_SETFLAG(r, XN_CF, 0);
    if (!xn_light_add_point(poly, (const struct xn_light_ref *)r->esi, &pt))
        return 0;
    xn_light_point_intensity[slot] = pt.intensity;
    xn_light_point_falloff[slot] = pt.falloff;
    xn_light_point_x[slot] = pt.x;
    xn_light_point_y[slot] = pt.y;
    xn_light_point_z[slot] = pt.z;
    r->ebp += 4;
    if (r->ebp != 12)
        return 0;
    xc_asm_points(pts, 3);
    r->eax = xn_light_build_shader(poly, xn_light_shade_row, pts, 3);
    return 1;
}

/* the dispatch tables' empty entries: canonical C's switch passes over those types */
void xn_light_add_type4_noop_r(xn_regs *r)
{
    (void)r;
}

void xn_light_terrain_add_point_noop_r(xn_regs *r)
{
    (void)r;
}

void xn_light_terrain_add_type4_noop_r(xn_regs *r)
{
    (void)r;
}

/* ESI the light ref, EDI the polygon; the row is the asm's xn_light_shade_row. CF and EBP 0
   when the row reached the last one. */
void xn_light_add_directional_r(xn_regs *r)
{
    int stop = xn_light_add_directional((const struct xn_poly *)r->edi,
                                        (const struct xn_light_ref *)r->esi,
                                        &xn_light_shade_row);

    if (stop)
        r->ebp = 0;
    XN_SETFLAG(r, XN_CF, stop);
}

void xn_light_terrain_add_directional_r(xn_regs *r)
{
    int stop = xn_light_terrain_add_directional((const struct xn_poly *)r->edi,
                                                (const struct xn_light_ref *)r->esi,
                                                &xn_light_shade_row);

    if (stop)
        r->ebp = 0;
    XN_SETFLAG(r, XN_CF, stop);
}
