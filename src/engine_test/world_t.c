/* world_t.c: test shims of src/engine/world.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call. The asm numbers a slot by
   its byte offset (slot * 4), and passes some values in globals: the shims read those where
   the asm's callers left them. */
#include "xworld.h"

extern u32 xn_world_slot;                       /* the slot being filled (byte offset) */
extern s32 xn_world_eye_col, xn_world_eye_row;  /* the eye's cell (xn_world_cell_index) */
extern s8 xn_world_last_dx, xn_world_last_dy;   /* the dead walkers' last step */

#define SLOT()  ((s32)(xn_world_slot >> 2))

void xn_world_init_r(xn_regs *r)
{
    (void)r;
    xn_world_init();
}

void xn_world_shutdown_r(xn_regs *r)
{
    (void)r;
    xn_world_shutdown();
}

void xn_world_cell_at_r(xn_regs *r)
{
    r->eax = xn_world_cell_at(r->eax, r->edx);
}

/* x EAX, z EBX -> the cell in EDI */
void xn_world_cell_index_r(xn_regs *r)
{
    s32 col, row;

    r->edi = xn_world_cell_index(r->eax, r->ebx, &col, &row);
}

/* x EAX, z EBX -> the slot (byte offset) in ESI */
void xn_world_cell_slot_r(xn_regs *r)
{
    s32 sub_x, sub_z;

    r->esi = xn_world_cell_slot(r->eax, r->ebx, &sub_x, &sub_z) << 2;
}

void xn_world_update_r(xn_regs *r)
{
    (void)r;
    xn_world_update();
}

/* the cell in EDI and the slot in ESI, both moved */
static void load_regs(xn_regs *r, int (*load)(s32 *, s32 *, s32), s32 eye)
{
    s32 cell = r->edi, slot = r->esi >> 2;

    load(&cell, &slot, eye);
    r->edi = cell;
    r->esi = slot << 2;
}

void xn_world_load_west_r(xn_regs *r)
{
    load_regs(r, xn_world_load_west, xn_world_eye_col);
}

void xn_world_load_east_r(xn_regs *r)
{
    load_regs(r, xn_world_load_east, xn_world_eye_col);
}

void xn_world_load_north_r(xn_regs *r)
{
    load_regs(r, xn_world_load_north, xn_world_eye_row);
}

void xn_world_load_south_r(xn_regs *r)
{
    load_regs(r, xn_world_load_south, xn_world_eye_row);
}

void xn_world_reload_r(xn_regs *r)
{
    (void)r;
    xn_world_reload();
}

/* the name in EDX */
void xn_world_create_file_r(xn_regs *r)
{
    xn_world_create_file((const char *)r->edx);
}

void xn_world_open_r(xn_regs *r)
{
    xn_world_open((const char *)r->eax);
}

/* the seek's mode in AL (the handle in EBX is the world file's) */
void xn_world_read_height_bands_r(xn_regs *r)
{
    xn_world_read_height_bands((u8)r->eax);
}

void xn_world_write_header_r(xn_regs *r)
{
    (void)r;
    xn_world_write_header();
}

void xn_world_read_header_r(xn_regs *r)
{
    (void)r;
    xn_world_read_header();
}

void xn_world_free_offsets_r(xn_regs *r)
{
    (void)r;
    xn_world_free_offsets();
}

/* the cell in EDI */
void xn_world_read_cell_r(xn_regs *r)
{
    xn_world_read_cell(r->edi);
}

/* n ECX, data EDX, the cell EDI */
void xn_world_write_cell_r(xn_regs *r)
{
    xn_world_write_cell(r->edi, (const void *)r->edx, r->ecx);
}

void xn_world_unpack_cell_slot_r(xn_regs *r)
{
    xn_world_unpack_cell_slot(r->eax);
}

void xn_world_unpack_cell_r(xn_regs *r)
{
    (void)r;
    xn_world_unpack_cell(SLOT());
}

/* out EDI -> the end in EDI */
void xn_world_random_elevations_r(xn_regs *r)
{
    r->edi = (u32)xn_world_random_elevations((u8 *)r->edi);
}

void xn_world_gen_heightmap_slot_r(xn_regs *r)
{
    xn_world_gen_heightmap_slot(r->eax);
}

void xn_world_gen_heightmap_r(xn_regs *r)
{
    (void)r;
    xn_world_gen_heightmap(SLOT());
}

void xn_world_place_nature_flats_r(xn_regs *r)
{
    xn_world_place_nature_flats(r->eax);
}

/* pos ECX, fallback EBP, heights EDI; the last step from its globals -> the step in EAX, the
   drop in EBX, CF */
void xn_world_steepest_neighbour_r(xn_regs *r)
{
    u32 step;
    u8 drop;
    int back = xn_world_steepest_neighbour(r->ecx, r->ebp, (u8)xn_world_last_dx |
                                           (u32)(u8)xn_world_last_dy << 8,
                                           (const u8 *)r->edi, &step, &drop);

    r->eax = step;
    r->ebx = drop;
    XN_SETFLAG(r, XN_CF, back);
}

void xn_world_gen_tiles_r(xn_regs *r)
{
    (void)r;
    xn_world_gen_tiles(SLOT());
}

void xn_world_fix_lone_tiles_r(xn_regs *r)
{
    (void)r;
    xn_world_fix_lone_tiles(SLOT());
}

void xn_world_rock_slopes_r(xn_regs *r)
{
    (void)r;
    xn_world_rock_slopes(SLOT());
}

void xn_world_gen_paths_r(xn_regs *r)
{
    (void)r;
    xn_world_gen_paths();
}

/* fallback EBP */
void xn_world_gen_paths_body_r(xn_regs *r)
{
    xn_world_gen_paths_body(SLOT(), r->ebp);
}

void xn_world_gen_river_r(xn_regs *r)
{
    (void)r;
    xn_world_gen_river();
}

void xn_world_gen_river_body_r(xn_regs *r)
{
    xn_world_gen_river_body(SLOT(), r->ebp);
}

void xn_world_blend_tiles_r(xn_regs *r)
{
    (void)r;
    xn_world_blend_tiles();
}

void xn_world_mark_nonplanar_quads_r(xn_regs *r)
{
    (void)r;
    xn_world_mark_nonplanar_quads();
}
