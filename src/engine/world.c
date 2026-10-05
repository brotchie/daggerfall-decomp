/* world.c: XnGine's world (xworld.h; see xngine.h and docs/xngine_readable.md): WOODS.WLD
   streamed through a 2 x 2 window of cells, and the ground generated for each cell.

   World units: 256 to a square, 128 squares (2^15) to a cell. The layers are 256 x 256 bytes
   (a square's byte at row << 8 | column; a slot's squares start at its origin). */
#include "xworld.h"

#define CELL_RECORD_HEADER      0x16    /* bytes of a cell record's header */
#define FULL_CELL_BYTES         0x10000 /* a full cell's four 128 x 128 layers */
#define LAYER_BYTES             0x10000

/* a slot's first square in the layers */
static u32 slot_origin(u32 slot)
{
    return (XN_SLOT(xn_world_slot_y0, slot) << 8) + XN_SLOT(xn_world_slot_x0, slot);
}

/* the world file to byte offset off (mode 0: from the start) */
static void seek_to(u32 off)
{
    xn_dos_seek(0, xn_world_file, off >> 16, off & 0xFFFF);
}

/* The engine shut down, msg ('$'-terminated) printed and the game ended (int 21h 4C00h). al:
   what the asm has in AL at the print (the I/O log keeps AX). */
static void world_fatal(const char *msg, u8 al)
{
    xn_regs r = { 0 };

    xn_render_shutdown();
    xn_gfx_restore_mode();
    xn_kbd_remove();
    xn_mem_shutdown();
    r.eax = 0x0900 | al;
    r.edx = (u32)msg;
    xn_int21(&r);
    r.eax = 0x4C00;
    xn_int21(&r);
}

/* ---- the layers ------------------------------------------------------------------------- */

void xn_world_init(void)
{
    u8 *block = func_000A10A8(4 * LAYER_BYTES + 0x20);
    u8 *layer;

    if (block == 0) {
        world_fatal(xn_world_msg_out_of_memory, 0);
        return;
    }
    xn_world_layers_alloc = block;
    layer = (u8 *)(((u32)block + 0x1F) & ~0x1Fu);
    xn_world_height_layer = layer;
    xn_world_flat_layer = layer + LAYER_BYTES;
    xn_world_tile_layer = layer + 2 * LAYER_BYTES;
    xn_world_water_layer = layer + 3 * LAYER_BYTES;
}

void xn_world_init_r(xn_regs *r)
{
    (void)r;
    xn_world_init();
}

void xn_world_shutdown(void)
{
    if (xn_world_layers_alloc == 0)
        return;
    func_000A117E(xn_world_layers_alloc);
    xn_dos_close(xn_world_file);
    xn_world_free_offsets();
}

void xn_world_shutdown_r(xn_regs *r)
{
    if (xn_world_layers_alloc != 0)
        r->ebx = xn_world_file;         /* the asm closes the file with the handle in EBX */
    xn_world_shutdown();
}

/* ---- cells and slots -------------------------------------------------------------------- */

s32 xn_world_cell_at(s32 x, s32 z)
{
    return xn_world_cell_index(x, z);
}

u32 xn_world_cell_index(s32 x, s32 z)
{
    xn_world_eye_row = (u32)(xn_world_size_z - z) >> 15;
    xn_world_eye_col = (u32)x >> 15;
    return xn_world_eye_row * xn_world_header.width + xn_world_eye_col;
}

u32 xn_world_cell_slot(s32 x, s32 z)
{
    u32 cx = (u32)x & 0xFFFF;                   /* the position in the cell pair */
    u32 cz = (u32)(xn_world_size_z - z) & 0xFFFF;

    xn_world_eye_sub_x = (cx >> 8) & 0x7F;
    xn_world_eye_sub_z = (cz >> 8) & 0x7F;
    xn_world_slot = (cz >> 15) * 8 + (cx >> 15) * 4;
    return xn_world_slot;
}

/* Reads cell into slot and generates its ground */
static void load_slot(u32 cell, u32 slot)
{
    XN_SLOT(xn_world_slot_cells, slot) = cell;
    xn_world_slot = slot;
    xn_world_read_cell(cell);
    xn_world_unpack_cell();
    xn_world_gen_heightmap();
}

/* The neighbour cell (the slot beside: slot ^ flip) unless the slot holds it already */
static int load_neighbour(u32 *cell, u32 *slot, u32 next, u32 flip, u8 dir)
{
    u32 s = *slot ^ flip;

    if (XN_SLOT(xn_world_slot_cells, s) == (s32)next)
        return 0;
    *cell = next;
    *slot = s;
    load_slot(next, s);
    xn_world_loaded_dirs |= dir;
    return 1;
}

int xn_world_load_west(u32 *cell, u32 *slot)
{
    if (xn_world_eye_col == 0)
        return 0;
    return load_neighbour(cell, slot, *cell - 1, 4, 1);
}

int xn_world_load_east(u32 *cell, u32 *slot)
{
    if (xn_world_eye_col == xn_world_header.width - 1)
        return 0;
    return load_neighbour(cell, slot, *cell + 1, 4, 2);
}

int xn_world_load_north(u32 *cell, u32 *slot)
{
    if (xn_world_eye_row == 0)
        return 0;
    return load_neighbour(cell, slot, *cell - xn_world_header.width, 8, 4);
}

int xn_world_load_south(u32 *cell, u32 *slot)
{
    if (xn_world_eye_row == xn_world_header.height - 1)
        return 0;
    return load_neighbour(cell, slot, *cell + xn_world_header.width, 8, 8);
}

/* the asm interface of a load: the cell in EDI, the slot in ESI, both moved; EBX the slot
   beside (it computes it there) unless at the world's edge */
static void load_regs(xn_regs *r, int (*load)(u32 *, u32 *), u32 flip, int at_edge)
{
    u32 cell = r->edi, slot = r->esi;

    if (at_edge)
        return;
    r->ebx = slot ^ flip;
    load(&cell, &slot);
    r->edi = cell;
    r->esi = slot;
}

void xn_world_load_west_r(xn_regs *r)
{
    load_regs(r, xn_world_load_west, 4, xn_world_eye_col == 0);
}

void xn_world_load_east_r(xn_regs *r)
{
    load_regs(r, xn_world_load_east, 4, xn_world_eye_col == xn_world_header.width - 1);
}

void xn_world_load_north_r(xn_regs *r)
{
    load_regs(r, xn_world_load_north, 8, xn_world_eye_row == 0);
}

void xn_world_load_south_r(xn_regs *r)
{
    load_regs(r, xn_world_load_south, 8, xn_world_eye_row == xn_world_header.height - 1);
}

void xn_world_update(void)
{
    u32 slot, cell;

    xn_world_loaded_dirs = 0;
    xn_world_update_pass = 0;
    slot = xn_world_cell_slot(xn_cam_x, xn_cam_z);
    cell = xn_world_cell_index(xn_cam_x, xn_cam_z);
    do {
        if (!(xn_world_loaded_dirs & (1 | 2))) {
            if (xn_world_eye_sub_x <= xn_world_edge_near)
                xn_world_load_west(&cell, &slot);
        }
        if (!(xn_world_loaded_dirs & (1 | 2))) {
            if (xn_world_eye_sub_x >= xn_world_edge_far)
                xn_world_load_east(&cell, &slot);
        }
        if (!(xn_world_loaded_dirs & (4 | 8))) {
            if (xn_world_eye_sub_z <= xn_world_edge_near)
                xn_world_load_north(&cell, &slot);
        }
        if (!(xn_world_loaded_dirs & (4 | 8))) {
            if (xn_world_eye_sub_z >= xn_world_edge_far)
                xn_world_load_south(&cell, &slot);
        }
    } while (++xn_world_update_pass != 2);
    if (xn_world_loaded_dirs != 0) {
        xn_world_blend_tiles();
        xn_world_mark_nonplanar_quads();
    }
}

/* the row of cells beside the slot's: down (+1) for a slot in the top row, else up */
static s32 vertical_step(u32 slot)
{
    return (slot & 8) ? 1 : -1;
}

void xn_world_reload(void)
{
    u32 cell = xn_world_cell_index(xn_cam_x, xn_cam_z);
    u32 slot = xn_world_cell_slot(xn_cam_x, xn_cam_z);
    u32 s;

    load_slot(cell, slot);
    load_slot(cell ^ 1, slot ^ 4);
    s = slot ^ 8;
    load_slot(cell + vertical_step(s) * xn_world_header.width, s);
    s = slot ^ 12;
    load_slot((cell + vertical_step(s) * xn_world_header.width) ^ 1, s);
    xn_world_blend_tiles();
    xn_world_mark_nonplanar_quads();
}

/* ---- the file --------------------------------------------------------------------------- */

void xn_world_create_file(const char *name)
{
    u32 *offsets;

    xn_world_file = xn_dos_create(name, (u32)name);   /* DOS sees the name in EAX's top half, as the asm left it */
    xn_world_header.offsets_size = (xn_world_header.width * xn_world_header.height + 1) * 4;
    offsets = func_000A10A8(xn_world_header.offsets_size);
    xn_world_header.offsets = offsets;
    offsets[0] = xn_world_header.offsets_size + 0x90 + 0x400;
}

/* xn_dos_open (dos group, asm): opens the file name; its handle, 0 (CF) on failure. (A
   failure ends the game inside it; its EAX is the caller's, kept.) */
static int dos_open(const char *name, u32 *handle)
{
    extern void asm_xn_dos_open(void);
    xn_regs r = { 0 };

    r.eax = (u32)name;
    r.edx = (u32)name;
    xn_asmcall(asm_xn_dos_open, &r);
    *handle = r.ebx;
    return !(r.eflags & XN_CF);
}

void xn_world_open(const char *name)
{
    u32 handle;

    if (!dos_open(name, &handle)) {
        world_fatal(xn_world_msg_open_failed, (u8)(u32)name);
        return;
    }
    xn_world_file = handle;
    xn_world_read_header();
    xn_world_read_height_bands((u32)name, handle);
    xn_world_slot_cells[0] = xn_world_slot_cells[1] = -1;
    xn_fill32(xn_world_height_layer, 0xFFFFFFFF, LAYER_BYTES / 4);
    xn_world_size_x = xn_world_header.width << 15;
    xn_world_size_z = xn_world_header.height << 15;
    xn_fill32(xn_world_slot_cells, (u32)-1, 4);
}

void xn_world_read_height_bands(u32 eax, u32 ebx)
{
    u32 off = xn_world_header.bands_offset;

    xn_dos_seek(eax, ebx, off >> 16, off & 0xFFFF);
    xn_dos_read(xn_world_height_bands, sizeof xn_world_height_bands, xn_world_file);
}

/* Writes n bytes to the world file (xn_dos_write takes the buffer pointer by address: it
   advances it past each whole 32K piece). */
static void write_all(const void *data, u32 n)
{
    const u8 *p = (const u8 *)data;

    xn_dos_write(&p, n, xn_world_file);
}

void xn_world_write_header(void)
{
    xn_world_header.bands_offset = xn_world_header.offsets_size + 0x90;
    xn_dos_seek(0, xn_world_file, 0, 0);
    write_all(&xn_world_header, sizeof xn_world_header);
    write_all(xn_world_header.offsets, xn_world_header.offsets_size);
    write_all(xn_world_height_bands, sizeof xn_world_height_bands);
}

void xn_world_read_header(void)
{
    u32 n;
    u32 *offsets;

    seek_to(0);
    xn_dos_read(&xn_world_header, sizeof xn_world_header, xn_world_file);
    n = xn_world_offsets_window_max;
    if (xn_world_offsets_window_max > (s32)xn_world_header.offsets_size)
        n = xn_world_header.offsets_size;
    xn_world_offsets_window_bytes = n;
    offsets = func_000A10A8(n);
    if (offsets == 0) {
        world_fatal(xn_world_msg_out_of_memory, 0);
        return;
    }
    xn_world_header.offsets = offsets;
    xn_dos_read(offsets, n, xn_world_file);
    seek_to(xn_world_header.offsets_size + 0x90);      /* the band table */
    xn_world_offsets_window_count = n >> 2;
    xn_world_offsets_window_first = 0;
}

void xn_world_read_header_r(xn_regs *r)
{
    (void)r;
    xn_world_read_header();
}

void xn_world_free_offsets(void)
{
    if (xn_world_header.offsets != 0) {
        func_000A117E(xn_world_header.offsets);
        xn_world_header.offsets = 0;
    }
}

void xn_world_free_offsets_r(xn_regs *r)
{
    (void)r;
    xn_world_free_offsets();
}

void xn_world_read_cell(u32 cell)
{
    s32 first = xn_world_offsets_window_first;
    s32 last;

    if ((s32)cell < first || (s32)cell >= first + xn_world_offsets_window_count) {
        /* the window from an eighth of it before the cell, within the table */
        last = (xn_world_header.offsets_size >> 2) - 1 - xn_world_offsets_window_count;
        first = cell - (xn_world_offsets_window_bytes >> 3);
        if (first < 0)
            first = 0;
        else if (first > last)
            first = last;
        xn_world_offsets_window_first = first;
        seek_to(first * 4 + 0x90);
        xn_dos_read(xn_world_header.offsets, xn_world_offsets_window_bytes, xn_world_file);
    }
    seek_to(xn_world_header.offsets[cell - xn_world_offsets_window_first]);
    xn_dos_read(&xn_world_cell_header, CELL_RECORD_HEADER, xn_world_file);
    xn_dos_read(big_buffer, (xn_world_cell_header.noise & 0x1F) ? FULL_CELL_BYTES : 25,
                xn_world_file);
    xn_world_noise_amp = xn_world_cell_header.noise >> 5;
    xn_world_noise_bias = xn_world_noise_amp >> 1;
}

void xn_world_write_cell(u32 n, const void *data, u32 cell)
{
    u32 *offsets = xn_world_header.offsets;

    seek_to(offsets[cell]);
    write_all(data, n);
    offsets[cell + 1] = offsets[cell] + sizeof(struct xn_wld_cell);
}

/* ---- unpacking a cell ------------------------------------------------------------------- */

/* the work grid's point (x, y): big_buffer + 100h, 256 bytes a row, 129 x 129 used */
static u8 *grid_at(u32 x, u32 y)
{
    return big_buffer + 0x100 + (y << 8) + x;
}

u32 xn_world_unpack_cell_slot(u32 index)
{
    xn_world_slot = index * 4;
    xn_world_unpack_cell();
    return xn_world_slot;
}

void xn_world_unpack_cell(void)
{
    const u8 *src = big_buffer;
    u8 *layers[4];
    u32 origin, k, row, col;
    u8 *dst;

    if (!(xn_world_cell_header.noise & 0x1F)) {
        /* the 25 control heights onto the cleared work grid */
        xn_fill32(big_buffer + 0x100, 0, 129 * 256 / 4);
        for (row = 0; row < 5; row++)
            for (col = 0; col < 5; col++)
                *grid_at(xn_world_grid_x[col], xn_world_grid_y[row]) = (u8)(*src++ + 0x80);
        return;
    }
    /* a full cell: the slot's square of each layer (the height square cleared first) */
    origin = slot_origin(xn_world_slot);
    for (row = 0; row < 128; row++)
        xn_fill32(xn_world_height_layer + origin + (row << 8), 0, 128 / 4);
    layers[0] = xn_world_height_layer;
    layers[1] = xn_world_flat_layer;
    layers[2] = xn_world_tile_layer;
    layers[3] = xn_world_water_layer;
    for (k = 0; k < 4; k++) {
        dst = layers[k] + origin;
        for (row = 0; row < 128; row++, dst += 256)
            for (col = 0; col < 128; col++)
                dst[col] = *src++;
    }
}

u8 *xn_world_random_elevations(u8 *out)
{
    s32 k, h;

    for (k = 0; k < 25; k++) {
        h = xn_rand_next() & 0x7F;      /* (the asm halves it while above 7Fh: never) */
        if (h < xn_world_rand_elev_min)
            h = xn_world_rand_elev_min;
        if (h > xn_world_rand_elev_max)
            h = xn_world_rand_elev_max;
        *out++ = (u8)h;
    }
    return out;
}

/* ---- the heightmap ---------------------------------------------------------------------- */

void xn_world_gen_heightmap_slot(u32 index)
{
    xn_world_slot = index * 4;
    xn_world_gen_heightmap();
}

/* v plus the cell's noise: a random 0..amp (a 0..7 halved while above amp) less the bias,
   clamped to a byte */
static s32 displace(s32 v)
{
    s32 r;

    if (xn_world_noise_amp == 0)
        return v;
    r = xn_rand_next() & 7;
    while (r > xn_world_noise_amp)
        r >>= 1;
    v += r - xn_world_noise_bias;
    if (v < 0)
        v = 0;
    if ((u32)v > 0xFF)
        v = 0xFF;
    return v;
}

/* the mean of the grid's points (scratch_x, scratch_y) and (scratch_x2, scratch_y2) */
static s32 pair_mean(void)
{
    return (*grid_at(xn_world_scratch_x2, xn_world_scratch_y2) +
            *grid_at(xn_world_scratch_x, xn_world_scratch_y)) >> 1;
}

/* the grid's point midway between them */
static void set_midpoint(s32 v)
{
    *grid_at((u32)(xn_world_scratch_x2 + xn_world_scratch_x) >> 1,
             (u32)(xn_world_scratch_y2 + xn_world_scratch_y) >> 1) = (u8)v;
}

/* The square of the current step at (scratch_x, scratch_y): the midpoints of its top and
   bottom edges, its centre (the mean of its diagonals' means), its left and right edges. The
   asm walks the pair (scratch_x, _y), (scratch_x2, _y2) around the square; they are globals,
   left as it leaves them. */
static void displace_square(void)
{
    s32 step = xn_world_gen_step;
    s32 diag;

    xn_world_scratch_x2 = xn_world_scratch_x + step;            /* top */
    xn_world_scratch_y2 = xn_world_scratch_y;
    set_midpoint(displace(pair_mean()));
    xn_world_scratch_y += step;                                 /* bottom */
    xn_world_scratch_y2 += step;
    set_midpoint(displace(pair_mean()));
    xn_world_scratch_y -= step;                                 /* centre */
    diag = pair_mean();
    xn_world_scratch_y2 -= step;
    xn_world_scratch_y += step;
    set_midpoint(displace((pair_mean() + diag) >> 1));
    xn_world_scratch_x2 = xn_world_scratch_x;                   /* left */
    xn_world_scratch_y2 = xn_world_scratch_y;
    xn_world_scratch_y -= step;
    set_midpoint(displace(pair_mean()));
    xn_world_scratch_x += step;                                 /* right */
    xn_world_scratch_x2 += step;
    set_midpoint(displace(pair_mean()));
}

void xn_world_gen_heightmap(void)
{
    s32 x, y, rows, cols;
    u32 origin, row, col;
    u8 *grid, *height, *water;

    if (xn_world_cell_header.noise & 0x1F)
        return;                         /* a full cell has its layers */
    xn_rand_seed = xn_world_cell_header.seed;
    xn_world_gen_step = 32;
    xn_world_gen_count = 4;
    do {
        xn_world_scratch_y = xn_world_slot_y0[0];       /* (slot 0's origin: 0, 0) */
        for (rows = xn_world_gen_count; rows != 0; rows--) {
            xn_world_scratch_x = xn_world_slot_x0[0];
            cols = xn_world_gen_count;
            do {
                x = xn_world_scratch_x;
                y = xn_world_scratch_y;
                displace_square();
                xn_world_scratch_y = y;
                xn_world_scratch_x = x + xn_world_gen_step;
            } while (--cols != 0);
            xn_world_scratch_y += xn_world_gen_step;
        }
        xn_world_gen_count <<= 1;
        xn_world_gen_step = (u32)xn_world_gen_step >> 1;
    } while (xn_world_gen_step != 1);
    /* the grid at sea level (80h) into heights above it and water depths below */
    origin = slot_origin(xn_world_slot);
    grid = big_buffer + 0x100;
    height = xn_world_height_layer + origin;
    water = xn_world_water_layer + origin;
    for (row = 0; row < 128; row++, grid += 256, height += 256, water += 256) {
        for (col = 0; col < 128; col++) {
            if (grid[col] >= 0x80) {
                water[col] = 0;
                height[col] = (u8)(grid[col] - 0x80);
            } else {
                water[col] = grid[col];
                height[col] = 0;
            }
        }
    }
    xn_world_gen_tiles();
}

/* ---- nature flats ----------------------------------------------------------------------- */

/* a random 0..100: 0..127 halved while above 100 */
static s32 percent(void)
{
    s32 r = xn_rand_next() & 0x7F;

    while (r > 100)
        r >>= 1;
    return r;
}

void xn_world_place_nature_flats(u32 index)
{
    struct xn_world_nature_odds *odds = &xn_world_nature_flat_odds;
    const u8 *band;
    u32 at = slot_origin(index * 4);
    u32 row, col;
    s32 r, h, n;

    for (row = 0; row < 128; row++, at += 128) {
        for (col = 0; col < 128; col++, at++) {
            xn_world_flat_layer[at] &= 3;               /* keep the flips */
            if ((s8)percent() >= (s8)odds->density)
                continue;
            r = percent();
            h = xn_world_height_layer[at] & 0x7F;
            band = xn_world_height_bands[xn_world_cell_header.climate].threshold;
            if ((s8)h <= (s8)band[0] || (s8)h >= (s8)band[3])
                continue;                               /* water or the last band */
            for (n = 1; n <= 32; n++) {
                if (odds->odds[h >> 5][n - 1] == 0xFF)
                    continue;
                if ((s8)r < (s8)odds->odds[h >> 5][n - 1]) {
                    xn_world_flat_layer[at] |= (u8)(n << 2);
                    break;
                }
            }
        }
    }
}

/* ---- tiles ------------------------------------------------------------------------------ */

int xn_world_steepest_neighbour(u32 pos, u32 fallback, const u8 *heights, u32 *step, u8 *drop)
{
    u8 h = heights[pos] & 0x7F;
    u8 best = 0, d;
    u32 dir = fallback, at;
    int k;

    for (k = 0; k < 8; k++) {
        at = (u8)(xn_world_dir_dx[k] + (u8)pos) |
             (u32)(u8)(xn_world_dir_dy[k] + (u8)(pos >> 8)) << 8;
        d = (heights[at] & 0x7F) - h;
        if ((s8)(u8)(d - best) < 0) {   /* the asm's `cmp; jns`: the byte difference's sign */
            best = d;
            dir = k;
        }
    }
    *step = (u8)xn_world_dir_dx[dir] | (u32)(u8)xn_world_dir_dy[dir] << 8;
    *drop = best;
    if ((s8)best >= 1)
        return 1;
    return (u8)-xn_world_last_dx == (u8)*step && (u8)-xn_world_last_dy == (u8)(*step >> 8);
}

void xn_world_steepest_neighbour_r(xn_regs *r)
{
    u32 step;
    u8 drop;
    int back = xn_world_steepest_neighbour(r->ecx, r->ebp, (const u8 *)r->edi, &step, &drop);

    r->eax = step;
    r->ebx = drop;
    XN_SETFLAG(r, XN_CF, back);
}

void xn_world_gen_tiles(void)
{
    u32 origin = slot_origin(xn_world_slot);
    u8 *tile = xn_world_tile_layer + origin;
    const u8 *height = xn_world_height_layer + origin;
    const u8 *band;
    u32 row, col;
    s32 cls = 0, r;
    u8 h, dist, width;

    for (row = 0; row < 128; row++, tile += 128, height += 128) {
        for (col = 0; col < 128; col++, tile++, height++) {
            h = *height & 0x7F;
            band = xn_world_height_bands[xn_world_cell_header.climate].threshold;
            for (cls = 0; cls != 3 && (s8)h > (s8)band[cls]; cls++)
                ;
            dist = band[cls] - h;
            if (cls != 0) {
                /* toward the band's bottom, more likely the class below */
                width = band[cls] - band[cls - 1];
                r = xn_rand_next() & 0x7F;
                while (r > width)
                    r >>= 1;
                if ((s8)r <= (s8)dist)
                    cls--;
            }
            *tile = (u8)cls;
        }
    }
    xn_world_fix_lone_tiles(cls);       /* (EAX: the last class) */
    xn_world_gen_river();
    xn_world_gen_paths();
    xn_world_rock_slopes(0);            /* (EBX's upper half: 0 after fix_lone_tiles) */
}

/* the square's neighbour in the 256 x 256 layers (row and column bytes wrap) */
static u32 beside(u32 at, s32 dx, s32 dy)
{
    return (u32)(u8)((at >> 8) + dy) << 8 | (u8)(at + dx);
}

u32 xn_world_fix_lone_tiles(u32 eax)
{
    u32 origin = slot_origin(xn_world_slot);
    u8 *tiles = xn_world_tile_layer + origin;
    u8 *heights = xn_world_height_layer + origin;
    u32 row, col, at;
    u8 water;
    s32 dx, dy;

    for (row = 0; row < 128; row++) {
        for (col = 0; col < 128; col++) {
            at = row << 8 | col;
            eax = (eax & ~0xFFu) | (tiles[at] & 0x3F);
            eax = (eax & ~0xFFu) | xn_world_tile_class[eax];
            if ((s8)eax < 0)
                continue;                               /* not a base tile */
            if ((u8)eax == 0) {
                /* water: the heights around it to the water line (the asm's first store goes
                   to the slot's square (climate * 4, 0) instead: kept) */
                water = xn_world_height_bands[xn_world_cell_header.climate].threshold[0];
                heights[xn_world_cell_header.climate * 4] = water;
                for (dy = -1; dy <= 1; dy++)
                    for (dx = -1; dx <= 1; dx++)
                        heights[beside(at, dx, dy)] = water;
            }
            eax = tiles[beside(at, -1, 0)];
            if ((u8)eax != tiles[beside(at, 1, 0)] || (u8)eax != tiles[beside(at, 0, -1)] ||
                (u8)eax != tiles[beside(at, 0, 1)])
                continue;
            /* four neighbours alike: the table's tile for (theirs, its own) */
            eax <<= 2;
            eax = (eax & ~0xFFu) | (u8)(eax + tiles[at]);
            eax = (eax & ~0xFFu) | xn_world_lone_tile_table[eax];
            tiles[at] = (u8)eax;
        }
    }
    return eax;
}

void xn_world_fix_lone_tiles_r(xn_regs *r)
{
    r->eax = xn_world_fix_lone_tiles(r->eax);
    r->ebx = 0x8080;                    /* its row and column bytes, past the slot's end */
}

/* |a| of a byte difference, as `or; jns; neg` leave it (-128 stays) */
static u8 byte_magnitude(u8 a)
{
    return (s8)a < 0 ? (u8)-a : a;
}

void xn_world_rock_slopes(u32 ebx)
{
    u32 origin = slot_origin(xn_world_slot) + (ebx & 0xFFFF0000);
    u8 *tiles = xn_world_tile_layer + origin;
    u8 *heights = xn_world_height_layer + origin;
    u32 row, col, at;
    u8 across, down, slope;

    for (row = 0; row < 128; row++) {
        for (col = 0; col < 128; col++) {
            at = row << 8 | col;
            down = byte_magnitude(heights[at + 0x100] - heights[at]);
            across = byte_magnitude(heights[at] - heights[at + 1]);
            if ((s8)across >= 2)
                slope = across;
            else if ((s8)down >= 2)
                slope = down;
            else
                continue;
            /* (the asm halves the random 0..15 while above 15: never) */
            if ((s32)(xn_rand_next() & 0xF) - slope * 4 >= 0)
                continue;
            if (tiles[at] == 0 || tiles[at] == 0x2E)
                continue;                               /* water, path */
            tiles[at] = 3;                              /* rock */
        }
    }
}

void xn_world_gen_paths(void)
{
}

/* a square's position byte (x or y) plus a step's, as `add cl, al` does */
static u32 step_x(u32 pos, s8 d)
{
    return (pos & ~0xFFu) | (u8)(pos + d);
}

static u32 step_y(u32 pos, s8 d)
{
    return (pos & ~0xFF00u) | (u32)(u8)((pos >> 8) + d) << 8;
}

void xn_world_gen_paths_body(u32 fallback)
{
    u32 origin = slot_origin(xn_world_slot);
    u8 *tiles = xn_world_tile_layer + origin;
    u8 *heights = xn_world_height_layer + origin;
    u16 *starts = xn_world_cell_header.path_starts;
    u32 k, start, pos, step;
    u8 drop;
    s8 dx, dy;

    starts[0] = 0x410;                                  /* square (64, 64) */
    for (k = 0; k < 4; k++) {
        start = starts[k];
        if (start == 0)
            continue;
        /* x = (start << 2) & FFh, y = bits 4-11 of start, even (and the bits above) */
        pos = ((start << 4) & ~0x1FFu) | (u8)(start << 2);
        xn_world_last_dx = xn_world_last_dy = 0;
        xn_world_path_avoid_dx = xn_world_path_avoid_dy = -1;
        for (;;) {
            xn_world_steepest_neighbour(pos, fallback, heights, &step, &drop);
            dx = (s8)step;
            dy = (s8)(step >> 8);
            if (dx == xn_world_path_avoid_dx || dy == xn_world_path_avoid_dy) {
                dx = -xn_world_path_avoid_dx;
                dy = -xn_world_path_avoid_dy;
            }
            xn_world_last_dx = dx;
            xn_world_last_dy = dy;
            pos = step_x(pos, dx);
            if ((u8)pos >= 0x80)
                break;
            pos = step_y(pos, dy);
            if ((u8)(pos >> 8) >= 0x80)
                break;
            if (tiles[pos] != 0)
                tiles[pos] = 0x2E;                      /* path */
        }
    }
}

void xn_world_gen_paths_body_r(xn_regs *r)
{
    xn_world_gen_paths_body(r->ebp);
}

void xn_world_gen_river(void)
{
}

void xn_world_gen_river_body(u32 fallback)
{
    u32 origin = slot_origin(xn_world_slot);
    u8 *tiles = xn_world_tile_layer + origin;
    u8 *heights = xn_world_height_layer + origin;
    u32 pos = 0x7878, step;                             /* square (120, 120) */
    u8 drop, level;
    s8 dx, dy;

    xn_world_last_dx = xn_world_last_dy = 0;
    for (;;) {
        if (xn_world_steepest_neighbour(pos, fallback, heights, &step, &drop)) {
            /* not downhill: step on, filling the pit to this square's height */
            dx = (s8)step;
            dy = (s8)(step >> 8);
            level = heights[pos];
            xn_world_last_dx = dx;
            xn_world_last_dy = dy;
            pos = step_y(step_x(pos, dx), dy);
            if ((u8)pos >= 0x80 || (u8)(pos >> 8) >= 0x80)
                return;
            heights[pos] = level;
            tiles[pos] = 0;
            continue;
        }
        /* downhill: water along x, then along y, then the corner */
        dx = (s8)step;
        dy = (s8)(step >> 8);
        xn_world_last_dx = dx;
        xn_world_last_dy = dy;
        pos = step_x(pos, dx);
        if ((u8)pos >= 0x80)
            return;
        tiles[pos] = 0;
        pos = step_y(step_x(pos, -dx), dy);
        if ((u8)(pos >> 8) >= 0x80)
            return;
        tiles[pos] = 0;
        pos = step_x(pos, dx);
        if (tiles[pos] == 0)
            return;                                     /* met water */
        tiles[pos] = 0;
    }
}

void xn_world_gen_river_body_r(xn_regs *r)
{
    xn_world_gen_river_body(r->ebp);
}

/* ---- the window's tiles ----------------------------------------------------------------- */

void xn_world_blend_tiles(void)
{
    u8 *tiles = xn_world_tile_layer, *flats = xn_world_flat_layer;
    u32 row, col, at, idx, cls, t;

    for (row = 0; row != 0xFF; row++) {
        col = 0;
        while (col != 0xFF) {
            at = row << 8 | col;
            flats[at] &= 0xFC;
            col++;                                      /* (the next square, unless retried) */
            cls = xn_world_tile_class[tiles[at] & 0x3F];
            if ((s8)cls < 0)
                continue;
            idx = cls << 4;                             /* this square */
            cls = xn_world_tile_class[tiles[at + 0x100] & 0x3F];
            if (cls == 0xFF)
                continue;
            idx += (u8)(cls << 2);                      /* below */
            cls = xn_world_tile_class[tiles[at + 1] & 0x3F];
            if (cls == 0xFF)
                continue;
            idx += cls;                                 /* right */
            cls = xn_world_tile_class[tiles[at + 0x101] & 0x3F];
            if (cls == 0xFF)
                continue;
            idx += cls << 6;                            /* diagonal */
            t = xn_terrain_corner_tile[idx];
            if (t == 0xFE) {
                tiles[at + 0x101] = tiles[at + 1];      /* and try the square again */
                col--;
                continue;
            }
            if (t == 0xFF)
                continue;
            tiles[at] = (u8)t;
            flats[at] = xn_terrain_corner_flip[idx];
        }
    }
}

/* a height byte's height */
static s32 height_of(u8 b)
{
    return xn_world_height_scale[b & 0x7F];
}

void xn_world_mark_nonplanar_quads(void)
{
    u8 *p = xn_world_height_layer;
    u32 row, col;

    for (row = 0; row < 256; row++) {
        for (col = 0; col < 256; col++, p++) {
            /* (the last row and column read past the layer: the next one, as the asm) */
            if (height_of(p[0]) - height_of(p[1]) != height_of(p[0x100]) - height_of(p[0x101]) ||
                height_of(p[0]) - height_of(p[0x100]) != height_of(p[1]) - height_of(p[0x101]))
                p[0] |= 0x80;
        }
    }
}
