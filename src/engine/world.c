/* world.c: XnGine's world (canonical C; the interface and the module's documentation are in
   xworld.h). The asm passed the eye's cell and square, the slot being filled, the noise and
   the generator's walk between its routines in globals; here they are parameters and
   locals. */
#include "xworld.h"
#include "xdos.h"
#include "xrand.h"
#include "xrender.h"
#include "xgfx.h"
#include "xkbd.h"
#include "xmem.h"

#define CELL_RECORD_HEADER  0x16        /* bytes of a cell record's header */
#define CELL_RECORD_BYTES   0x2F        /* a cell record (header and 5 x 5 heights) */
#define CELL_CONTROLS       25          /* the 5 x 5 control heights */
#define FULL_CELL_BYTES     0x10000     /* a full cell's four 128 x 128 layers */
#define FILE_HEADER_BYTES   0x90
#define SEA_LEVEL           0x80        /* the work grid's heights: below it, water */

#define WEST    1                       /* xn_world_update's directions loaded */
#define EAST    2
#define NORTH   4
#define SOUTH   8

void func_000A117E(void *p);            /* the game's free */

#if defined(DAGGER_PORT)
u32 *xn_world_offsets;                  /* the offset window (XN_WORLD_OFFSETS, xworld.h) */
#endif

/* a slot's first square in the layers */
static u32 slot_origin(s32 slot)
{
    return (xn_world_slot_y0[slot] << 8) + xn_world_slot_x0[slot];
}

/* the world file to byte offset off from its start */
static void seek_to(u32 off)
{
    xn_dos_seek(xn_world_file, off, XN_DOS_SEEK_SET, 0);
}

/* The engine shut down, msg ('$'-terminated) printed and the game ended. */
static void world_fatal(const char *msg)
{
    xn_render_shutdown();
    xn_gfx_restore_mode();
    xn_kbd_remove();
    xn_mem_shutdown();
    xn_dos_print(msg);
    xn_dos_exit(0);
}

/* n bytes of v from p */
static void fill(u8 *p, u8 v, u32 n)
{
    while (n-- != 0)
        *p++ = v;
}

/* is this cell's record a full cell (its four layers stored, not generated)? */
static int full_cell(void)
{
    return (xn_world_cell_header.noise & 0x1F) != 0;
}

/* ---- the layers ------------------------------------------------------------------------- */

void xn_world_init(void)
{
    u8 *block = func_000A10A8(4 * XN_WORLD_LAYER + 0x20);
    u8 *layer;

    if (block == 0) {
        world_fatal(xn_world_msg_out_of_memory);
        return;
    }
    xn_world_layers_alloc = block;
    layer = (u8 *)(((uptr)block + 0x1F) & ~(uptr)0x1F);
    xn_world_height_layer = layer;
    xn_world_flat_layer = layer + XN_WORLD_LAYER;
    xn_world_tile_layer = layer + 2 * XN_WORLD_LAYER;
    xn_world_water_layer = layer + 3 * XN_WORLD_LAYER;
}

void xn_world_shutdown(void)
{
    if (xn_world_layers_alloc == 0)
        return;
    func_000A117E(xn_world_layers_alloc);
    xn_dos_close(xn_world_file);
    xn_world_free_offsets();
}

/* ---- cells and slots -------------------------------------------------------------------- */

s32 xn_world_cell_index(s32 x, s32 z, s32 *col, s32 *row)
{
    *row = (u32)(xn_world_size_z - z) >> 15;
    *col = (u32)x >> 15;
    return *row * xn_world_header.width + *col;
}

s32 xn_world_cell_at(s32 x, s32 z)
{
    s32 col, row;

    return xn_world_cell_index(x, z, &col, &row);
}

s32 xn_world_cell_slot(s32 x, s32 z, s32 *sub_x, s32 *sub_z)
{
    u32 cx = (u32)x & 0xFFFF;                   /* the position in the cells' pair */
    u32 cz = (u32)(xn_world_size_z - z) & 0xFFFF;

    *sub_x = (cx >> 8) & 0x7F;
    *sub_z = (cz >> 8) & 0x7F;
    return (cz >> 15) * 2 + (cx >> 15);
}

/* Reads cell into slot and generates its ground */
static void load_slot(s32 cell, s32 slot)
{
    xn_world_slot_cells[slot] = cell;
    xn_world_read_cell(cell);
    xn_world_unpack_cell(slot);
    xn_world_gen_heightmap(slot);
}

/* The neighbour cell next into the slot beside (slot ^ flip) unless it holds it already */
static int load_neighbour(s32 *cell, s32 *slot, s32 next, s32 flip)
{
    s32 s = *slot ^ flip;

    if (xn_world_slot_cells[s] == next)
        return 0;
    *cell = next;
    *slot = s;
    load_slot(next, s);
    return 1;
}

int xn_world_load_west(s32 *cell, s32 *slot, s32 eye_col)
{
    if (eye_col == 0)
        return 0;
    return load_neighbour(cell, slot, *cell - 1, 1);
}

int xn_world_load_east(s32 *cell, s32 *slot, s32 eye_col)
{
    if (eye_col == (s32)xn_world_header.width - 1)
        return 0;
    return load_neighbour(cell, slot, *cell + 1, 1);
}

int xn_world_load_north(s32 *cell, s32 *slot, s32 eye_row)
{
    if (eye_row == 0)
        return 0;
    return load_neighbour(cell, slot, *cell - xn_world_header.width, 2);
}

int xn_world_load_south(s32 *cell, s32 *slot, s32 eye_row)
{
    if (eye_row == (s32)xn_world_header.height - 1)
        return 0;
    return load_neighbour(cell, slot, *cell + xn_world_header.width, 2);
}

void xn_world_update(void)
{
    s32 slot, cell, sub_x, sub_z, col, row, pass;
    u32 loaded = 0;

    slot = xn_world_cell_slot(xn_cam_x, xn_cam_z, &sub_x, &sub_z);
    cell = xn_world_cell_index(xn_cam_x, xn_cam_z, &col, &row);
    for (pass = 0; pass < 2; pass++) {
        if (!(loaded & (WEST | EAST)) && sub_x <= xn_world_edge_near &&
            xn_world_load_west(&cell, &slot, col))
            loaded |= WEST;
        if (!(loaded & (WEST | EAST)) && sub_x >= xn_world_edge_far &&
            xn_world_load_east(&cell, &slot, col))
            loaded |= EAST;
        if (!(loaded & (NORTH | SOUTH)) && sub_z <= xn_world_edge_near &&
            xn_world_load_north(&cell, &slot, row))
            loaded |= NORTH;
        if (!(loaded & (NORTH | SOUTH)) && sub_z >= xn_world_edge_far &&
            xn_world_load_south(&cell, &slot, row))
            loaded |= SOUTH;
    }
    if (loaded != 0) {
        xn_world_blend_tiles();
        xn_world_mark_nonplanar_quads();
    }
}

/* the row of slot s's cell from the eye's (s in the other row of the window): the row below
   (+1) for a slot in the window's bottom row, the row above (-1) for one in its top row */
static s32 row_step(s32 slot)
{
    return (slot & 2) ? 1 : -1;
}

void xn_world_reload(void)
{
    s32 col, row, sub_x, sub_z, s;
    s32 cell = xn_world_cell_index(xn_cam_x, xn_cam_z, &col, &row);
    s32 slot = xn_world_cell_slot(xn_cam_x, xn_cam_z, &sub_x, &sub_z);

    load_slot(cell, slot);
    load_slot(cell ^ 1, slot ^ 1);
    s = slot ^ 2;
    load_slot(cell + row_step(s) * xn_world_header.width, s);
    s = slot ^ 3;
    load_slot((cell + row_step(s) * xn_world_header.width) ^ 1, s);
    xn_world_blend_tiles();
    xn_world_mark_nonplanar_quads();
}

/* ---- the file --------------------------------------------------------------------------- */

void xn_world_create_file(const char *name)
{
    u32 *offsets;

    xn_world_file = xn_dos_create(name);
    xn_world_header.offsets_size = (xn_world_header.width * xn_world_header.height + 1) * 4;
    offsets = func_000A10A8(xn_world_header.offsets_size);
    XN_WORLD_OFFSETS = offsets;
    offsets[0] = xn_world_header.offsets_size + FILE_HEADER_BYTES + sizeof xn_world_height_bands;
}

void xn_world_open(const char *name)
{
    /* Quirk Q-WORLD-05 (dropped): a file that does not open ends the game inside
       xn_dos_open, so the asm's own message after it is never reached */
    xn_world_file = xn_dos_open(name);
    xn_world_read_header();
#ifdef DAGGER_PORT
    /* Quirk Q-WORLD-01 as FALL.EXE has it: the game's one call passes the name at 0x17521C,
       so the mode is 1Ch, which DOS refuses (a native address's low byte could be 0-2) */
    xn_world_read_height_bands(0x1C);
#else
    xn_world_read_height_bands((u8)(uptr)name);    /* Quirk Q-WORLD-01 */
#endif
    fill(xn_world_height_layer, 0xFF, XN_WORLD_LAYER);
    xn_world_size_x = xn_world_header.width << 15;
    xn_world_size_z = xn_world_header.height << 15;
    xn_world_slot_cells[0] = xn_world_slot_cells[1] = -1;
    xn_world_slot_cells[2] = xn_world_slot_cells[3] = -1;
}

void xn_world_read_height_bands(u8 mode)
{
    xn_dos_seek(xn_world_file, xn_world_header.bands_offset, mode, 0);
    xn_dos_read(xn_world_file, xn_world_height_bands, sizeof xn_world_height_bands);
}

void xn_world_write_header(void)
{
    xn_world_header.bands_offset = xn_world_header.offsets_size + FILE_HEADER_BYTES;
    seek_to(0);
    xn_dos_write(xn_world_file, &xn_world_header, sizeof xn_world_header);
    xn_dos_write(xn_world_file, XN_WORLD_OFFSETS, xn_world_header.offsets_size);
    xn_dos_write(xn_world_file, xn_world_height_bands, sizeof xn_world_height_bands);
}

void xn_world_read_header(void)
{
    u32 n;
    u32 *offsets;

    seek_to(0);
    xn_dos_read(xn_world_file, &xn_world_header, sizeof xn_world_header);
    n = xn_world_offsets_window_max;
    if (xn_world_offsets_window_max > (s32)xn_world_header.offsets_size)
        n = xn_world_header.offsets_size;
    xn_world_offsets_window_bytes = n;
    offsets = func_000A10A8(n);
    if (offsets == 0) {
        world_fatal(xn_world_msg_out_of_memory);
        return;
    }
    XN_WORLD_OFFSETS = offsets;
    xn_dos_read(xn_world_file, offsets, n);
    seek_to(xn_world_header.offsets_size + FILE_HEADER_BYTES);     /* the band table */
    xn_world_offsets_window_count = n >> 2;
    xn_world_offsets_window_first = 0;
}

void xn_world_free_offsets(void)
{
    if (XN_WORLD_OFFSETS != 0) {
        func_000A117E(XN_WORLD_OFFSETS);
        XN_WORLD_OFFSETS = 0;
    }
}

void xn_world_read_cell(s32 cell)
{
    s32 first = xn_world_offsets_window_first;
    s32 last;

    if (cell < first || cell >= first + xn_world_offsets_window_count) {
        /* the window from an eighth of it before the cell, within the table */
        last = (xn_world_header.offsets_size >> 2) - 1 - xn_world_offsets_window_count;
        first = cell - (xn_world_offsets_window_bytes >> 3);
        if (first < 0)
            first = 0;
        else if (first > last)
            first = last;
        xn_world_offsets_window_first = first;
        seek_to(first * 4 + FILE_HEADER_BYTES);
        xn_dos_read(xn_world_file, XN_WORLD_OFFSETS, xn_world_offsets_window_bytes);
    }
    seek_to(XN_WORLD_OFFSETS[cell - xn_world_offsets_window_first]);
    xn_dos_read(xn_world_file, &xn_world_cell_header, CELL_RECORD_HEADER);
    xn_dos_read(xn_world_file, big_buffer, full_cell() ? FULL_CELL_BYTES : CELL_CONTROLS);
}

void xn_world_write_cell(s32 cell, const void *data, u32 n)
{
    u32 *offsets = XN_WORLD_OFFSETS;

    seek_to(offsets[cell]);
    xn_dos_write(xn_world_file, data, n);
    offsets[cell + 1] = offsets[cell] + CELL_RECORD_BYTES;
}

/* ---- unpacking a cell ------------------------------------------------------------------- */

/* the work grid's point (x, y): big_buffer + 100h, 256 bytes a row, 129 x 129 used */
static u8 *grid_at(u32 x, u32 y)
{
    return big_buffer + 0x100 + (y << 8) + x;
}

void xn_world_unpack_cell_slot(s32 slot)
{
    xn_world_unpack_cell(slot);
}

void xn_world_unpack_cell(s32 slot)
{
    const u8 *src = big_buffer;
    u8 *layers[4];
    u32 origin, k, row, col;
    u8 *dst;

    if (!full_cell()) {
        /* the 25 control heights onto the cleared work grid */
        fill(big_buffer + 0x100, 0, 129 * 256);
        for (row = 0; row < 5; row++)
            for (col = 0; col < 5; col++)
                *grid_at(xn_world_grid_x[col], xn_world_grid_y[row]) = (u8)(*src++ + SEA_LEVEL);
        return;
    }
    /* a full cell: the slot's square of each layer (the height square cleared first) */
    origin = slot_origin(slot);
    for (row = 0; row < 128; row++)
        fill(xn_world_height_layer + origin + (row << 8), 0, 128);
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

    for (k = 0; k < CELL_CONTROLS; k++) {
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

void xn_world_gen_heightmap_slot(s32 slot)
{
    xn_world_gen_heightmap(slot);
}

/* v plus the cell's noise (amplitude amp, 0..7): a random 0..7 halved while above amp, less
   amp / 2; clamped to a byte */
static s32 displace(s32 v, s32 amp)
{
    s32 r;

    if (amp == 0)
        return v;
    r = xn_rand_next() & 7;
    while (r > amp)
        r >>= 1;
    v += r - (amp >> 1);
    if (v < 0)
        v = 0;
    if (v > 0xFF)
        v = 0xFF;
    return v;
}

/* the mean of the grid's points (x0, y0) and (x1, y1) */
static s32 mean(u32 x0, u32 y0, u32 x1, u32 y1)
{
    return (*grid_at(x1, y1) + *grid_at(x0, y0)) >> 1;
}

/* The square of side s at (x, y): the midpoints of its top and bottom edges, its centre (the
   mean of its two diagonals' means), its left and right edges, each displaced by the noise
   (in that order: the random numbers are drawn in it) */
static void displace_square(u32 x, u32 y, u32 s, s32 amp)
{
    u32 mx = (2 * x + s) >> 1, my = (2 * y + s) >> 1;
    s32 diag;

    *grid_at(mx, y) = (u8)displace(mean(x, y, x + s, y), amp);
    *grid_at(mx, y + s) = (u8)displace(mean(x, y + s, x + s, y + s), amp);
    diag = mean(x, y, x + s, y + s);
    *grid_at(mx, my) = (u8)displace((mean(x, y + s, x + s, y) + diag) >> 1, amp);
    *grid_at(x, my) = (u8)displace(mean(x, y, x, y + s), amp);
    *grid_at(x + s, my) = (u8)displace(mean(x + s, y, x + s, y + s), amp);
}

void xn_world_gen_heightmap(s32 slot)
{
    s32 amp = xn_world_cell_header.noise >> 5;
    u32 step, count, x, y, i, j, origin, row, col;
    u8 *grid, *height, *water;

    if (full_cell())
        return;                         /* a full cell has its layers */
    xn_rand_seed = xn_world_cell_header.seed;
    for (step = 32, count = 4; step != 1; step >>= 1, count <<= 1)
        for (i = 0, y = 0; i < count; i++, y += step)
            for (j = 0, x = 0; j < count; j++, x += step)
                displace_square(x, y, step, amp);
    /* the grid at sea level into heights above it and water depths below */
    origin = slot_origin(slot);
    grid = big_buffer + 0x100;
    height = xn_world_height_layer + origin;
    water = xn_world_water_layer + origin;
    for (row = 0; row < 128; row++, grid += 256, height += 256, water += 256) {
        for (col = 0; col < 128; col++) {
            if (grid[col] >= SEA_LEVEL) {
                water[col] = 0;
                height[col] = (u8)(grid[col] - SEA_LEVEL);
            } else {
                water[col] = grid[col];
                height[col] = 0;
            }
        }
    }
    xn_world_gen_tiles(slot);
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

void xn_world_place_nature_flats(s32 slot)
{
    const struct xn_world_nature_odds *odds = &xn_world_nature_flat_odds;
    const u8 *band;
    u32 at = slot_origin(slot);
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

int xn_world_steepest_neighbour(u32 pos, u32 fallback, u32 last, const u8 *heights, u32 *step,
                                u8 *drop)
{
    u8 h = heights[pos] & 0x7F;
    u8 best = 0, d;
    u32 dir = fallback, at;
    int k;

    for (k = 0; k < 8; k++) {
        at = (u8)(xn_world_dir_dx[k] + (u8)pos) |
             (u32)(u8)(xn_world_dir_dy[k] + (u8)(pos >> 8)) << 8;
        d = (heights[at] & 0x7F) - h;
        if ((s8)(u8)(d - best) < 0) {   /* the sign of the byte difference (`cmp; jns`) */
            best = d;
            dir = k;
        }
    }
    *step = (u8)xn_world_dir_dx[dir] | (u32)(u8)xn_world_dir_dy[dir] << 8;
    *drop = best;
    if ((s8)best >= 1)
        return 1;
    /* straight back: the step is the last one negated */
    return (u8)-(s8)last == (u8)*step && (u8)-(s8)(last >> 8) == (u8)(*step >> 8);
}

void xn_world_gen_tiles(s32 slot)
{
    u32 origin = slot_origin(slot);
    u8 *tile = xn_world_tile_layer + origin;
    const u8 *height = xn_world_height_layer + origin;
    const u8 *band;
    u32 row, col;
    s32 cls, r;
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
    xn_world_fix_lone_tiles(slot);
    xn_world_gen_river();
    xn_world_gen_paths();
    xn_world_rock_slopes(slot);
}

/* the square's neighbour in the 256 x 256 layers (row and column bytes wrap: the asm steps
   DH and DL) */
static u32 beside(u32 at, s32 dx, s32 dy)
{
    return (u32)(u8)((at >> 8) + dy) << 8 | (u8)(at + dx);
}

void xn_world_fix_lone_tiles(s32 slot)
{
    u32 origin = slot_origin(slot);
    u8 *tiles = xn_world_tile_layer + origin;
    u8 *heights = xn_world_height_layer + origin;
    u32 row, col, at;
    u8 cls, water, t;
    s32 dx, dy;

    /* (Quirk Q-WORLD-04, dropped: the asm's first class lookups include its caller's stale
       upper EAX, always 0 from xn_world_gen_tiles) */
    for (row = 0; row < 128; row++) {
        for (col = 0; col < 128; col++) {
            at = row << 8 | col;
            cls = xn_world_tile_class[tiles[at] & 0x3F];
            if ((s8)cls < 0)
                continue;                               /* not a base tile */
            if (cls == 0) {
                /* water: the heights around it to the water line. Quirk Q-WORLD-02: the
                   asm's first store goes to the slot's square (climate * 4, 0) instead */
                water = xn_world_height_bands[xn_world_cell_header.climate].threshold[0];
                heights[xn_world_cell_header.climate * 4] = water;
                for (dy = -1; dy <= 1; dy++)
                    for (dx = -1; dx <= 1; dx++)
                        heights[beside(at, dx, dy)] = water;
            }
            t = tiles[beside(at, -1, 0)];
            if (t != tiles[beside(at, 1, 0)] || t != tiles[beside(at, 0, -1)] ||
                t != tiles[beside(at, 0, 1)])
                continue;
            /* four neighbours alike: the table's tile for (theirs, its own) */
            tiles[at] = xn_world_lone_tile_table[(u8)((t << 2) + tiles[at])];
        }
    }
}

/* |a| of a byte difference, as `or; jns; neg` leave it (-128 stays) */
static u8 byte_magnitude(u8 a)
{
    return (s8)a < 0 ? (u8)-a : a;
}

void xn_world_rock_slopes(s32 slot)
{
    /* (Quirk Q-WORLD-04, dropped: the asm's square offsets keep its caller's upper EBX,
       always 0 from xn_world_gen_tiles) */
    u32 origin = slot_origin(slot);
    u8 *tiles = xn_world_tile_layer + origin;
    const u8 *heights = xn_world_height_layer + origin;
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

/* a step's two bytes (dx | dy << 8) */
static u32 pack_step(s8 dx, s8 dy)
{
    return (u8)dx | (u32)(u8)dy << 8;
}

void xn_world_gen_paths_body(s32 slot, u32 fallback)
{
    u32 origin = slot_origin(slot);
    u8 *tiles = xn_world_tile_layer + origin;
    const u8 *heights = xn_world_height_layer + origin;
    u16 *starts = xn_world_cell_header.path_starts;
    u32 k, start, pos, step;
    u8 drop;
    s8 dx, dy, last_dx, last_dy;

    starts[0] = 0x410;                                  /* square (64, 64) */
    for (k = 0; k < 4; k++) {
        start = starts[k];
        if (start == 0)
            continue;
        /* x = (start << 2) & FFh, y = bits 4-11 of start, even (and the bits above) */
        pos = ((start << 4) & ~0x1FFu) | (u8)(start << 2);
        last_dx = last_dy = 0;
        for (;;) {
            xn_world_steepest_neighbour(pos, fallback, pack_step(last_dx, last_dy), heights,
                                        &step, &drop);
            dx = (s8)step;
            dy = (s8)(step >> 8);
            /* never left or up: those steps become right and down */
            if (dx == -1 || dy == -1) {
                dx = 1;
                dy = 1;
            }
            last_dx = dx;
            last_dy = dy;
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

void xn_world_gen_river(void)
{
}

void xn_world_gen_river_body(s32 slot, u32 fallback)
{
    u32 origin = slot_origin(slot);
    u8 *tiles = xn_world_tile_layer + origin;
    u8 *heights = xn_world_height_layer + origin;
    u32 pos = 0x7878, step;                             /* square (120, 120) */
    u8 drop, level;
    s8 dx, dy, last_dx = 0, last_dy = 0;

    for (;;) {
        if (xn_world_steepest_neighbour(pos, fallback, pack_step(last_dx, last_dy), heights,
                                        &step, &drop)) {
            /* not downhill: step on, filling the pit to this square's height */
            dx = (s8)step;
            dy = (s8)(step >> 8);
            level = heights[pos];
            last_dx = dx;
            last_dy = dy;
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
        last_dx = dx;
        last_dy = dy;
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
            /* Quirk Q-WORLD-03: the last row and column read past it, the next row (and the
               flat layer after the last) */
            if (height_of(p[0]) - height_of(p[1]) != height_of(p[0x100]) - height_of(p[0x101]) ||
                height_of(p[0]) - height_of(p[0x100]) != height_of(p[1]) - height_of(p[0x101]))
                p[0] |= 0x80;
        }
    }
}
