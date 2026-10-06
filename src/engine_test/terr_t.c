/* terr_t.c: test shims of src/engine/terrain.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call. The asm numbers a vertex
   by its byte offset in the vertex arrays (12 bytes a vertex), and its cell axis routines
   read the cells' texture origins from the world's scratch globals, where
   xn_terrain_draw_cells leaves them. */
#include "xterrain.h"

extern s32 xn_world_scratch_x, xn_world_scratch_y;

#define VERTEX(off) ((u32)(off) / 12)

void xn_terrain_draw_r(xn_regs *r)
{
    (void)r;
    xn_terrain_draw();
}

void xn_terrain_build_light_list_r(xn_regs *r)
{
    (void)r;
    xn_terrain_build_light_list();
}

void xn_terrain_add_nature_flats_r(xn_regs *r)
{
    (void)r;
    xn_terrain_add_nature_flats();
}

void xn_terrain_setup_axes_r(xn_regs *r)
{
    (void)r;
    xn_terrain_setup_axes();
}

void xn_terrain_build_height_table_r(xn_regs *r)
{
    (void)r;
    xn_terrain_build_height_table();
}

void xn_terrain_transform_grid_r(xn_regs *r)
{
    (void)r;
    xn_terrain_transform_grid();
}

/* the vertex (byte offset) in ESI, the polygon in EDI; CF when it faces away */
void xn_terrain_face_plane_a_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_terrain_face_plane_a(VERTEX(r->esi), (struct xn_poly *)r->edi));
}

void xn_terrain_face_plane_b_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_terrain_face_plane_b(VERTEX(r->esi), (struct xn_poly *)r->edi));
}

void xn_terrain_draw_tri_a_r(xn_regs *r)
{
    xn_terrain_draw_tri_a(VERTEX(r->esi), (struct xn_poly *)r->edi);
}

void xn_terrain_draw_tri_b_r(xn_regs *r)
{
    xn_terrain_draw_tri_b(VERTEX(r->esi), (struct xn_poly *)r->edi);
}

/* the outcodes in BX */
void xn_terrain_draw_tri_a_clipped_r(xn_regs *r)
{
    xn_terrain_draw_tri_a_clipped(VERTEX(r->esi), (struct xn_poly *)r->edi, r->ebx & 0xFFFF);
}

void xn_terrain_draw_tri_b_clipped_r(xn_regs *r)
{
    xn_terrain_draw_tri_b_clipped(VERTEX(r->esi), (struct xn_poly *)r->edi, r->ebx & 0xFFFF);
}

/* CF when the ground's texture cannot be had */
void xn_terrain_draw_cells_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_terrain_draw_cells());
}

void xn_terrain_setup_tex_gradients_r(xn_regs *r)
{
    s32 u0, v0;

    (void)r;
    xn_terrain_setup_tex_gradients(&u0, &v0);
}

/* the polygon in EDI */
#define AXIS_SHIM(name) \
    void name##_r(xn_regs *r) \
    { \
        name((struct xn_poly *)r->edi, xn_world_scratch_x, xn_world_scratch_y); \
    }

AXIS_SHIM(xn_terrain_u_axis_x)
AXIS_SHIM(xn_terrain_u_axis_z)
AXIS_SHIM(xn_terrain_u_axis_neg_x)
AXIS_SHIM(xn_terrain_u_axis_neg_z)
AXIS_SHIM(xn_terrain_v_axis_z)
AXIS_SHIM(xn_terrain_v_axis_x)
AXIS_SHIM(xn_terrain_v_axis_neg_z)
AXIS_SHIM(xn_terrain_v_axis_neg_x)

/* x EAX, z EDX */
void xn_terrain_height_at_r(xn_regs *r)
{
    r->eax = xn_terrain_height_at(r->eax, r->edx);
}
