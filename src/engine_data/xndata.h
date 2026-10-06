/* xndata.h: XnGine's data as C, for a build of the engine outside FALL.EXE (the port).
   docs/engine/data.md.

   What it is
     The canonical engine (src/engine/) declares its globals and tables as externs. In the
     test harness they stay where FALL.EXE has them (object 2), because the game reads them
     there. This module defines them in C instead (ddefs.c: every engine-owned symbol, with
     the type the engine's headers declare), so that src/engine and src/engine_data link
     without FALL.EXE's addresses. config/xngine_data.csv is the inventory
     (tools/xn_datagen.py), with each symbol's address in FALL.EXE, kind and evidence.

   How the data gets its values
     zero       plain storage (all zero in FALL.EXE's load image too);
     defaults   small scalars are C initializers, and the tables of code addresses name their
                functions (the game's spell, steering and debug-menu handlers; the IRQ
                entries): ddefs.c;
     initial    strings, tables and defaults that are FALL.EXE's own bytes are not in the
                repository: xn_data_load copies them from the user's FALL.EXE at start-up
                (object 2's load image; the table of symbols, addresses and sizes is dtable.c);
     computed   the tables code generates: xn_data_compute (dcompute.c), or the engine's own
                init functions at run time (xn_mem_init, xn_render_init...).

   A port's start-up: read FALL.EXE (1.07.213), xn_data_object2 into a buffer of
   XN_DATA_OBJECT2_SIZE bytes, xn_data_load(buffer), xn_data_compute(), then the game. */
#ifndef XNDATA_H
#define XNDATA_H

#include "xngine.h"

#define XN_DATA_OBJECT2_BASE 0xC0000    /* object 2's address in FALL.EXE */
#define XN_DATA_OBJECT2_SIZE 0xA1568    /* and its size (1.07.213) */

/* An initial symbol: its storage, its address in FALL.EXE, its bytes */
typedef struct xn_data_item {
    void *at;
    u32 address;
    u32 size;
} xn_data_item;

extern const xn_data_item xn_data_items[];      /* dtable.c (generated) */
extern const u32 xn_data_item_count;

/* Object 2's load image from the bytes of FALL.EXE (exe, exe_size: the whole file) into
   image (XN_DATA_OBJECT2_SIZE bytes): its pages, as the LE executable maps them at
   XN_DATA_OBJECT2_BASE. Returns 1, or 0 when exe is not FALL.EXE 1.07.213's LE layout. */
int xn_data_object2(const u8 *exe, u32 exe_size, u8 *image);

/* Copies each initial symbol's bytes from object 2's load image (image[a - 0xC0000] is the
   byte at address a: the pages as the file holds them, or relocated; no initial symbol holds
   an address). */
void xn_data_load(const u8 *object2_image);

/* Generates the tables code can generate (dcompute.c): the sine (and so the cosine), the
   square roots, the texture records' offsets, the polygon rings and the face edge tables. */
void xn_data_compute(void);

/* Object-2 data the engine reaches only through its pointer tables (xn_poly_ring_a/_b point
   into the rings, xn_face_edge_tables into the slots) */
struct xn_poly_vertex;
extern struct xn_poly_vertex *xn_poly_ring_tables[2432];
extern s32 xn_face_edge_slots[319];

/* Object-2 data only the game references (the canonical engine does not read it) */
extern s32 xn_cam_roll;                 /* the eye's roll, which the game sets (0xC23C0) */

#endif
