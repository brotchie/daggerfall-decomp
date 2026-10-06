/* xworld.h: XnGine's world (src/engine/world.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     The outdoor world is WOODS.WLD's grid of cells (1000 x 500). The engine keeps a window
     of 2 x 2 cells around the eye, unpacked into four 256 x 256 terrain layers, and streams
     the neighbour cell into the window as the eye nears a cell's edge. A cell record holds a
     seed, a climate, a noise amplitude and 5 x 5 control heights; the ground of a cell is
     generated from them: a fractal heightmap (midpoint displacement), the sea split off as
     water depths, ground tiles by the climate's height bands, rock on slopes, and the tiles'
     transitions; nature flats (trees, rocks) are placed by the region's odds. The terrain
     module (xterrain.h) draws the layers.

   Units
     world units  256 to a square, 128 squares (2^15) to a cell; z counts down from the
                  world's north edge (xn_world_size_z) to give a row
     cells        numbered row * width + column, row 0 at the north edge
     slots        the window's four places, 0 north-west, 1 north-east, 2 south-west, 3 south-
                  east (bit 0 east, bit 1 south); slot s holds its cell's 128 x 128 squares at
                  (xn_world_slot_x0[s], xn_world_slot_y0[s]) of the layers. The asm numbers a
                  slot by its byte offset in its tables (s * 4).
     layers       256 x 256 bytes, a square at row << 8 | column: heights (bits 0-6: an
                  index into xn_world_height_scale; bit 7: the square is two triangles),
                  flats (nature flat << 2 | the tile's flips), tiles (texture | rotation << 6),
                  water depths

   Files and state (object 2)
     xn_world_file, xn_world_header (its .offsets: a window of the cell offset table:
     xn_world_offsets_window_*), xn_world_height_bands (per climate), xn_world_cell_header
     (the cell read last: game-visible), xn_world_slot_cells (the cell in each slot, -1 none:
     game-visible), xn_world_size_x/_z, the layer pointers (game-visible) and their block.
     Read only: xn_world_slot_x0/_y0, xn_world_grid_x/_y (the control points' places on the
     work grid), xn_world_height_scale, xn_world_edge_near/_far (17, 111: the squares where
     a neighbour loads), the tile tables (xn_world_tile_class, xn_world_lone_tile_table,
     xn_terrain_corner_tile/_flip), the walkers' directions. The game's: the eye (xn_cam_*),
     xn_world_nature_flat_odds, the archives. The generator seeds the engine's random numbers
     (xn_rand_seed, xrand.h) with the cell's seed.
   Files are read through the DOS layer (xdos.h); WOODS.WLD's header is 90h bytes, then the
   cell offset table, the 400h-byte band table, a byte map XnGine never reads, and the 47-byte
   cell records (xnstruct.h). The work grid of the generator is big_buffer + 100h (129 x 129
   bytes, rows of 256), and a cell record's control heights arrive at big_buffer.

   Quirks kept: Q-WORLD-01 (the band table's seek mode), Q-WORLD-02 (the water's stray store),
   Q-WORLD-03 (the last row and column read past the layer); dropped: Q-WORLD-04 (the tile
   passes' stale register bytes), Q-WORLD-05 (xn_world_open's unreachable message).
   docs/engine/quirks.md. */
#ifndef XWORLD_H
#define XWORLD_H

#include "xwshare.h"

#define XN_WORLD_LAYER  0x10000         /* bytes of a layer: 256 x 256 */
#define XN_WORLD_CELL   0x8000          /* world units of a cell (128 squares) */

/* ---- the layers and the eye's world ------------------------------------------------------- */
extern s32 xn_world_size_x, xn_world_size_z;    /* the world's extent, world units */
extern u8 *xn_world_height_layer;               /* the four layers (64 KB each, one block) */
extern u8 *xn_world_flat_layer;
extern u8 *xn_world_tile_layer;
extern u8 *xn_world_water_layer;
extern u8 *xn_world_layers_alloc;               /* the block, from the game's allocator */
extern s32 xn_world_height_scale[128];          /* a height byte's height, world units */
extern u16 xn_world_nature_archive;             /* the nature flats' TEXTURE archive */
extern u16 xn_world_ground_archive;             /* the ground's TEXTURE archive */

/* ---- WOODS.WLD and the window ------------------------------------------------------------ */
extern s32 xn_world_file;                       /* the DOS handle of WOODS.WLD */
extern struct xn_wld_header xn_world_header;    /* .offsets: the offset table's window */
/* The offset table's window. Natively the header keeps the file's 4 bytes at +0Ch (it is
   read and written whole), too narrow for the address: the window's pointer is a global of
   the native build's own (world.c). */
#if defined(DAGGER_PORT)
extern u32 *xn_world_offsets;
#define XN_WORLD_OFFSETS xn_world_offsets
#else
#define XN_WORLD_OFFSETS xn_world_header.offsets
#endif
extern u32 xn_world_offsets_window_bytes;       /* bytes of the table in the window */
extern s32 xn_world_offsets_window_max;         /* the window's capacity (4000h bytes) */
extern s32 xn_world_offsets_window_count;       /* cells in the window */
extern s32 xn_world_offsets_window_first;       /* the window's first cell */
extern struct xn_wld_bands xn_world_height_bands[256];  /* per climate */
extern struct xn_wld_cell_header xn_world_cell_header;  /* the cell read last */
extern s32 xn_world_slot_cells[4];              /* the cell in each slot, -1 none */
extern s32 xn_world_slot_x0[4];                 /* each slot's origin in the layers */
extern s32 xn_world_slot_y0[4];
extern s32 xn_world_edge_near, xn_world_edge_far;   /* load a neighbour at these squares */
extern char xn_world_msg_out_of_memory[];       /* '$'-terminated */
extern char xn_world_msg_open_failed[];

/* ---- the generator's tables --------------------------------------------------------------- */
extern s32 xn_world_grid_x[5], xn_world_grid_y[5];  /* the 5 x 5 control points' places */
extern u32 xn_rand_seed;                        /* the engine's random numbers (xrand.h) */
extern s32 xn_world_rand_elev_min, xn_world_rand_elev_max;
extern u8 xn_world_tile_class[65];              /* a tile's class 0..3 (FFh: none) */
extern u8 xn_world_lone_tile_table[16];         /* [neighbours' class * 4 + own] */
extern s8 xn_world_dir_dx[8], xn_world_dir_dy[8];   /* the 8 neighbours' steps, from east
                                                   clockwise (y grows southward) */
extern u8 xn_terrain_corner_tile[256];          /* the transition tile of 4 corner classes */
extern u8 xn_terrain_corner_flip[256];          /*   and its flips */
extern struct xn_world_nature_odds xn_world_nature_flat_odds;

/* ---- the layers --------------------------------------------------------------------------- */

/* Allocates the four terrain layers: one block of 4 x 64 KB from the game's allocator, the
   layers 32-byte aligned in it. Out of memory ends the game (the engine shut down, a message
   printed). One game site. */
void xn_world_init(void);

/* Frees the layers, closes the world file and frees the offset window (nothing when the
   layers were never allocated). The render group's shutdown. */
void xn_world_shutdown(void);

/* ---- cells and slots ---------------------------------------------------------------------- */

/* The cell under (x, z) (world units). Four game sites. */
s32 xn_world_cell_at(s32 x, s32 z);

/* The cell under (x, z), and its column and row (to *col, *row). */
s32 xn_world_cell_index(s32 x, s32 z, s32 *col, s32 *row);

/* The window slot of (x, z) (0..3, by the parity of its cell's column and row), and its
   square in its cell (to *sub_x, *sub_z: 0..127, z from the cell's north edge). */
s32 xn_world_cell_slot(s32 x, s32 z, s32 *sub_x, s32 *sub_z);

/* Each outdoor frame: loads into the window the neighbour cell the eye nears (its square
   within xn_world_edge_near of the cell's west or north edge, or from xn_world_edge_far of
   its east or south edge), at most one east-west and one north-south, in two passes so that
   a diagonal neighbour follows; when any loaded, blends the tiles and marks the squares that
   are two triangles. One game site. */
void xn_world_update(void);

/* Loads the cell west of *cell into the slot west of *slot, unless the eye's cell is at the
   world's west edge (eye_col 0) or that slot holds it already; then *cell and *slot are the
   loaded cell's. Returns 1 when it loaded. */
int xn_world_load_west(s32 *cell, s32 *slot, s32 eye_col);

/* The same to the east (the edge: eye_col = width - 1). */
int xn_world_load_east(s32 *cell, s32 *slot, s32 eye_col);

/* The same to the north (the edge: eye_row 0). */
int xn_world_load_north(s32 *cell, s32 *slot, s32 eye_row);

/* The same to the south (the edge: eye_row = height - 1). */
int xn_world_load_south(s32 *cell, s32 *slot, s32 eye_row);

/* Loads the whole window around the eye: its cell, the one beside it in its pair of columns
   (cell ^ 1), and the two in the row of cells above or below it (for a slot in the top row,
   the row below), then blends the tiles and marks. Four game sites (arriving outdoors). */
void xn_world_reload(void);

/* ---- the file ----------------------------------------------------------------------------- */

/* Dead (the world editor's): creates the world file name and an offset table of width *
   height + 1 cells, the first cell after the header, the table and the band table. */
void xn_world_create_file(const char *name);

/* Opens the world file name: its header and the first window of its offset table, its height
   bands; empties the window's slots and the height layer; the world's extent. One game site
   (the outdoors' setup). */
void xn_world_open(const char *name);

/* Reads the 400h-byte height band table, after a seek to it with mode `mode`. Quirk
   Q-WORLD-01: the asm never sets the mode: it is the low byte of the file name's address
   xn_world_open received (1Ch: DOS refuses the seek), harmless because
   xn_world_read_header leaves the file at the table. */
void xn_world_read_height_bands(u8 mode);

/* Dead (the world editor's): writes the header, the offset table and the band table back. */
void xn_world_write_header(void);

/* Reads the 90h-byte header and the first window of the offset table (at most
   xn_world_offsets_window_max bytes, from the game's allocator: out of memory ends the
   game), and leaves the file at the band table. */
void xn_world_read_header(void);

/* Frees the offset window (when there is one). */
void xn_world_free_offsets(void);

/* Reads cell's record: moves the offset window when the cell is outside it (from an eighth
   of the window before the cell, within the table), then the record's 22-byte header into
   xn_world_cell_header and its 5 x 5 control heights (or, for a full cell, its 64 KB of
   layers) to big_buffer. */
void xn_world_read_cell(s32 cell);

/* Dead (the world editor's): writes n bytes of data as cell's record; the next cell's offset
   47 bytes on. */
void xn_world_write_cell(s32 cell, const void *data, u32 n);

/* ---- a cell's ground ----------------------------------------------------------------------- */

/* Dead: xn_world_unpack_cell for slot. */
void xn_world_unpack_cell_slot(s32 slot);

/* Unpacks the cell read last for slot: its 25 control heights (+80h: sea level) onto the
   cleared 129 x 129 work grid at big_buffer + 100h, 32 squares apart; or a full cell's four
   layers into the slot (its heights cleared first). */
void xn_world_unpack_cell(s32 slot);

/* Dead (the world editor's): 25 random control heights within [xn_world_rand_elev_min,
   xn_world_rand_elev_max] from out; returns the end. */
u8 *xn_world_random_elevations(u8 *out);

/* Dead: xn_world_gen_heightmap for slot. */
void xn_world_gen_heightmap_slot(s32 slot);

/* The ground of the cell read last, in slot (nothing for a full cell, which has its layers):
   the work grid filled by midpoint displacement (squares of 32 squares down to 2, each
   midpoint the mean of its two or four corners plus noise: a random 0..amplitude less half
   the amplitude, the cell's noise bits 5-7; seeded by the cell's seed), split at sea level
   (80h) into the slot's heights above it and water depths below; then its tiles. */
void xn_world_gen_heightmap(s32 slot);

/* Nature flats for slot: on each square, by the region's density, a flat whose odds depend
   on the square's height (a quarter of the height range each), on land between the
   climate's first and last bands. One game site. */
void xn_world_place_nature_flats(s32 slot);

/* The dead walkers' step: the most downhill of the 8 neighbours of square pos (x | y << 8,
   each wrapping in a byte) in heights: its step (*step: dx | dy << 8, bytes) and its height
   difference (*drop, a byte). With no lower neighbour the step is that of direction
   `fallback`. Returns 1 when the step is not downhill or goes straight back against the
   walker's last step (last: dx | dy << 8). */
int xn_world_steepest_neighbour(u32 pos, u32 fallback, u32 last, const u8 *heights, u32 *step,
                                u8 *drop);

/* Slot's tiles: each square's class is the climate's height band it is in (0 water .. 3),
   dithered at random toward the band below near a band's bottom; then the water and lone
   tiles, the disabled river and paths, and the rock slopes. */
void xn_world_gen_tiles(s32 slot);

/* Slot's water and lone tiles: water levels the 3 x 3 heights around it to the water line
   (Q-WORLD-02); a tile whose four neighbours are alike becomes the table's tile for them and
   it. */
void xn_world_fix_lone_tiles(s32 slot);

/* Rock on slot's slopes: a square whose height steps by 2 or more to its right or below it
   becomes rock (tile 3), with odds that grow with the step, unless it is water or path. */
void xn_world_rock_slopes(s32 slot);

/* A disabled pass: returns at once (its body, xn_world_gen_paths_body, follows it). */
void xn_world_gen_paths(void);

/* Dead: paths in slot from the cell's start points (the first forced to square (64, 64)),
   each walking downhill (right and down only) to the slot's edge, painting tile 2Eh on land.
   fallback: the walker's direction where nothing is lower (the asm's EBP: whatever it held). */
void xn_world_gen_paths_body(s32 slot, u32 fallback);

/* A disabled pass: returns at once (its body follows it). */
void xn_world_gen_river(void);

/* Dead: a river in slot from square (120, 120) walking downhill, painting water in L-shaped
   steps and filling pits, until it meets water or the slot's edge. fallback: as above. */
void xn_world_gen_river_body(s32 slot, u32 fallback);

/* Over the window's 255 x 255 squares: each tile with its right, lower and diagonal
   neighbours' classes becomes the transition tile and flips of the four (FFh: none; FEh:
   the diagonal takes the right one's tile and the square is tried again). */
void xn_world_blend_tiles(void);

/* Over the 256 x 256 height layer: marks (bit 7) the squares whose four corners are not in
   a plane: two triangles (Q-WORLD-03 at the last row and column). One game site. */
void xn_world_mark_nonplanar_quads(void);

#endif
