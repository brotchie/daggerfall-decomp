/* xworld.h: XnGine's world (world.c; see xngine.h): WOODS.WLD and the 2 x 2 window of its
   cells around the eye, unpacked into the four 256 x 256 terrain layers, and the ground the
   world generates for each cell (a fractal heightmap, ground tiles, nature flats).

   The window's four slots (0: north-west, 1: north-east, 2: south-west, 3: south-east) each
   hold a cell of 128 x 128 squares. A slot is numbered by its byte offset in the slot tables
   (slot * 4: 0, 4, 8, 12), as xn_world_slot holds it. A cell is numbered row * width + column,
   row 0 at the north edge.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XWORLD_H
#define XWORLD_H

#include "xwshare.h"

/* the slot tables by a slot's byte offset */
#define XN_SLOT(table, off)     (*(s32 *)((u8 *)(table) + (off)))

/* ---- the eye and the layers ----------------------------------------------------------- */
extern s32 xn_cam_x, xn_cam_y, xn_cam_z;        /* the eye, world units (256 to a square) */
extern s32 xn_world_size_z;                     /* the world's z extent: rows count from it */
extern u8 *xn_world_height_layer;               /* 256 x 256 bytes: height (bit 7: the square is
                                                   two triangles) */
extern u8 *xn_world_flat_layer;                 /* nature flat << 2 | the tile's flips */
extern u8 *xn_world_tile_layer;                 /* the ground tile: texture | rotation << 6 */
extern s32 xn_world_height_scale[128];          /* a height byte's height, world units */
extern u16 xn_world_nature_archive;             /* the nature flats' TEXTURE archive */
extern u16 xn_world_ground_archive;             /* the ground's TEXTURE archive */
extern s32 xn_world_scratch_x;                  /* the generator's square; the terrain's cell */
extern s32 xn_world_scratch_y;                  /*   texture origins; xn_terrain_height_at's
                                                   point */

/* ---- WOODS.WLD and the window ---------------------------------------------------------- */
extern u32 xn_world_file;                       /* the DOS handle of WOODS.WLD */
extern struct xn_wld_header xn_world_header;    /* .offsets: the offset table's window */
extern u32 xn_world_offsets_window_bytes;       /* bytes of the table in the window */
extern s32 xn_world_offsets_window_max;         /* the window's capacity (4000h) */
extern s32 xn_world_offsets_window_count;       /* cells in the window */
extern s32 xn_world_offsets_window_first;       /* the window's first cell */
extern struct xn_wld_bands xn_world_height_bands[256];  /* per climate */
extern struct xn_wld_cell_header xn_world_cell_header;  /* the cell read last */
extern s32 xn_world_slot_cells[4];              /* the cell in each slot, -1 none */
extern u32 xn_world_slot;                       /* the slot being filled (byte offset) */
extern s32 xn_world_slot_x0[4];                 /* each slot's origin in the layers */
extern s32 xn_world_slot_y0[4];
extern s32 xn_world_size_x;                     /* the world's extent: width, height << 15 */
extern s32 xn_world_eye_col, xn_world_eye_row;  /* the eye's cell */
extern s32 xn_world_eye_sub_x, xn_world_eye_sub_z;  /* the eye's square in its cell (0..127,
                                                   z from the north edge) */
extern s32 xn_world_edge_near, xn_world_edge_far;   /* load a neighbour at these squares */
extern u8 xn_world_loaded_dirs;                 /* cells loaded this frame: 1 west, 2 east,
                                                   4 north, 8 south */
extern u8 xn_world_update_pass;
extern u8 *xn_world_water_layer;                /* (after the tile layer: the fourth) */
extern u8 *xn_world_layers_alloc;
extern char xn_world_msg_out_of_memory[];       /* '$'-terminated */
extern char xn_world_msg_open_failed[];

/* ---- the generator --------------------------------------------------------------------- */
extern s32 xn_world_noise_amp;                  /* the heightmap's noise: 0..amp */
extern s32 xn_world_noise_bias;                 /*   less amp / 2 */
extern s32 xn_world_gen_step, xn_world_gen_count;   /* the fractal's square size and count */
extern s32 xn_world_scratch_x2, xn_world_scratch_y2;
extern s32 xn_world_grid_x[5], xn_world_grid_y[5];  /* the 5 x 5 control points' places */
extern u32 xn_rand_seed;
extern s32 xn_world_rand_elev_min, xn_world_rand_elev_max;
extern u8 xn_world_tile_class[65];              /* a tile's class 0..3 (FFh: none) */
extern u8 xn_world_lone_tile_table[16];         /* [neighbours' class * 4 + own] */
extern s8 xn_world_dir_dx[8], xn_world_dir_dy[8];   /* the 8 neighbours' steps */
extern s8 xn_world_last_dx, xn_world_last_dy;   /* the dead walkers' last step */
extern s8 xn_world_path_avoid_dx, xn_world_path_avoid_dy;   /* the paths' steps never taken
                                                   (-1, -1: they go right and down) */
extern u8 xn_terrain_corner_tile[256];          /* the transition tile of 4 corner classes */
extern u8 xn_terrain_corner_flip[256];          /*   and its flips */
extern struct xn_world_nature_odds xn_world_nature_flat_odds;

/* ---- engine shutdown (render, gfx, kbd, mem groups; asm) ------------------------------- */
void xn_render_shutdown(void);
#pragma aux xn_render_shutdown modify exact [eax];
void xn_gfx_restore_mode(void);
#pragma aux xn_gfx_restore_mode modify exact [eax];
void xn_kbd_remove(void);
#pragma aux xn_kbd_remove modify exact [eax];
void xn_mem_shutdown(void);
#pragma aux xn_mem_shutdown modify exact [eax edx];

/* ---- the game's heap (Watcom's C library in object 1) ---------------------------------- */
void *func_000A10A8(u32 size);                  /* malloc */
void func_000A117E(void *p);                    /* free */

/* Allocates the four terrain layers (64 KB each, 32-byte aligned, one block); out of memory
   ends the game. (The glue only because the row lists registers the asm saves as outputs.) */
void xn_world_init(void);
void xn_world_init_r(xn_regs *r);

/* Frees the layers, closes the world file and frees the offset window. (The glue leaves the
   file's handle in EBX, as the asm does, which the row lists as an output.) */
void xn_world_shutdown(void);
void xn_world_shutdown_r(xn_regs *r);

/* The cell under (x, z) (world units). */
s32 xn_world_cell_at(s32 x, s32 z);

/* The cell under (x, z), and the eye's cell column and row. (Keeps EAX: the route's stub.) */
u32 xn_world_cell_index(s32 x, s32 z);
#pragma aux xn_world_cell_index parm [eax] [ebx] value [edi] modify exact [eax edx edi];

/* The window slot of (x, z) (the cells' parity), and the eye's square in its cell. */
u32 xn_world_cell_slot(s32 x, s32 z);
#pragma aux xn_world_cell_slot parm [eax] [ebx] value [esi] modify exact [eax edx esi];

/* Each outdoor frame: loads the neighbour cell the eye nears (within the edge squares) into
   the window, twice over so a diagonal follows; then, if any loaded, blends the tiles and
   marks the squares that are two triangles. (The asm saves every register.) */
void xn_world_update(void);

/* Loads the cell west (east, north, south) of *cell into the slot beside *slot, unless the
   window is at the world's edge or the slot holds it already; then *cell and *slot are the
   loaded cell's. Returns 1 when it loaded. */
int xn_world_load_west(u32 *cell, u32 *slot);
void xn_world_load_west_r(xn_regs *r);
int xn_world_load_east(u32 *cell, u32 *slot);
void xn_world_load_east_r(xn_regs *r);
int xn_world_load_north(u32 *cell, u32 *slot);
void xn_world_load_north_r(xn_regs *r);
int xn_world_load_south(u32 *cell, u32 *slot);
void xn_world_load_south_r(xn_regs *r);

/* Loads the window around the eye: its cell and the three others of its 2 x 2 group (the
   column beside by the cell number's bit 0), then blends and marks. */
void xn_world_reload(void);

/* Dead (an editor's): creates the world file name and an offset table for its cells, the
   first cell after the header, the table and the band table. */
void xn_world_create_file(const char *name);
#pragma aux xn_world_create_file parm [edx] modify exact [eax edx ebx];

/* Opens the world file: its header and offset window, its height bands; empties the window's
   slots and the height layer; the world's extent. No file ends the game. */
void xn_world_open(const char *name);

/* Reads the 400h-byte height band table. The seek's mode is the caller's AL (never set: the
   low byte of the file name's address, an invalid mode) and its handle the caller's EBX (the
   file's): harmless, because the header's reader leaves the file at the table. */
void xn_world_read_height_bands(u32 eax, u32 ebx);
#pragma aux xn_world_read_height_bands parm [eax] [ebx] modify exact [eax ebx ecx edx esi edi];

/* Dead (an editor's): writes the header, the offset table and the band table back. (Keeps
   EAX: the route's stub.) */
void xn_world_write_header(void);
#pragma aux xn_world_write_header parm [] modify exact [eax];

/* Reads the 90h-byte header and the first window of the offset table (at most 4000h bytes);
   out of memory ends the game. Leaves the file at the band table. (Glue: the row lists
   registers the asm saves as outputs.) */
void xn_world_read_header(void);
void xn_world_read_header_r(xn_regs *r);

/* Frees the offset window. (Glue: as init.) */
void xn_world_free_offsets(void);
void xn_world_free_offsets_r(xn_regs *r);

/* Reads cell's record: moves the offset window when the cell is outside it, then the cell's
   22-byte header and its 5 x 5 control heights (or, for a full cell, 64 KB of layers) into
   big_buffer; the cell's noise amplitude. (The row's ECX input is the asm's `shld ecx, ..`,
   whose result it masks off.) */
void xn_world_read_cell(u32 cell);
#pragma aux xn_world_read_cell parm [edi] modify exact [eax edx];

/* Dead (an editor's): writes n bytes of data as cell's record; the next cell's offset 47
   bytes on. (Keeps EAX.) */
void xn_world_write_cell(u32 n, const void *data, u32 cell);
#pragma aux xn_world_write_cell parm [ecx] [edx] [edi] modify exact [eax];

/* Dead: unpacks the cell into slot index; returns the slot's byte offset (as the asm
   leaves it). */
u32 xn_world_unpack_cell_slot(u32 index);

/* Unpacks the cell read last: its 25 control heights (+80h, sea level) onto the 129 x 129 work
   grid at big_buffer + 100h, 32 squares apart; or a full cell's four layers into its slot.
   (Keeps EAX.) */
void xn_world_unpack_cell(void);
#pragma aux xn_world_unpack_cell parm [] modify exact [eax];

/* Dead (an editor's): 25 random control heights within [rand_elev_min, rand_elev_max] from
   out; returns the end (the asm's EDI). (Keeps EAX.) */
u8 *xn_world_random_elevations(u8 *out);
#pragma aux xn_world_random_elevations parm [edi] value [edi] modify exact [eax edi];

/* Dead: generates the heights of slot index. (Keeps EAX.) */
void xn_world_gen_heightmap_slot(u32 index);
#pragma aux xn_world_gen_heightmap_slot parm [eax] modify exact [eax];

/* The cell's ground (unless it is a full cell): the work grid filled by midpoint
   displacement (squares of 32 down to 2, each midpoint the mean of its two or four corners
   plus noise, seeded by the cell), then split at sea level (80h) into the slot's heights and
   water depths; then its tiles. */
void xn_world_gen_heightmap(void);

/* Nature flats for slot index: on each square, by the region's density, a flat whose odds
   depend on the square's height band, on land between the climate's first and last band. */
void xn_world_place_nature_flats(u32 index);

/* Dead walkers' step: the most downhill of the 8 neighbours of square pos (x | y << 8) in
   heights: its step (*step: dx | dy << 8) and height difference (*drop). Returns 1 (the asm's
   CF) when it is not downhill or goes straight back (against xn_world_last_dx/dy). With no
   lower neighbour the step is that of direction fallback (the caller's EBP). */
int xn_world_steepest_neighbour(u32 pos, u32 fallback, const u8 *heights, u32 *step, u8 *drop);
void xn_world_steepest_neighbour_r(xn_regs *r);

/* The slot's tiles: each square's class is the climate's height band it is in, dithered at
   random toward the band below; then the lone tiles and the rock slopes. */
void xn_world_gen_tiles(void);

/* The slot's water and lone tiles: water levels the 3 x 3 heights around it to the water
   line; a tile whose four neighbours share a class takes the table's tile. eax: the caller's
   EAX, whose upper bytes the asm's class lookups include until it first clears them; returns
   the EAX it ends with. */
u32 xn_world_fix_lone_tiles(u32 eax);
void xn_world_fix_lone_tiles_r(xn_regs *r);

/* Rock on the slot's slopes: a square whose height steps by 2 or more to its right or below
   becomes rock (tile 3) with odds growing with the step, unless water or path. ebx: the
   asm's square offsets are EBX with only its low half cleared (0 from its caller). */
void xn_world_rock_slopes(u32 ebx);
#pragma aux xn_world_rock_slopes parm [ebx] modify exact [eax ebx ecx edx esi edi];

/* A disabled pass: returns at once (its body follows). (Keeps EAX.) */
void xn_world_gen_paths(void);
#pragma aux xn_world_gen_paths parm [] modify exact [eax];

/* Dead: paths from the cell's start points (the first forced to square (64, 64)), each
   walking downhill (right and down only) to the slot's edge, painting tile 2Eh on land.
   fallback: the walker's direction where nothing is lower (the asm's EBP). */
void xn_world_gen_paths_body(u32 fallback);
void xn_world_gen_paths_body_r(xn_regs *r);

/* A disabled pass: returns at once (its body follows). (Keeps EAX.) */
void xn_world_gen_river(void);
#pragma aux xn_world_gen_river parm [] modify exact [eax];

/* Dead: a river from square (120, 120) walking downhill, painting water in L-shaped steps
   and filling pits, until it meets water or the slot's edge. */
void xn_world_gen_river_body(u32 fallback);
void xn_world_gen_river_body_r(xn_regs *r);

/* Over the 255 x 255 window: each tile with its right, lower and diagonal neighbours' classes
   becomes the transition tile and flips of the four (FFh: none; FEh: the diagonal takes the
   right one's tile and the square is tried again). */
void xn_world_blend_tiles(void);

/* Over the 256 x 256 height layer: marks (bit 7) the squares whose four corners are not in a
   plane: two triangles. */
void xn_world_mark_nonplanar_quads(void);

#endif
