/* engine_structs.h: XnGine's run-time data structures (FALL.EXE object 2) as C structs.

   Groundwork for the readable C of the engine (src/engine/): the layouts were recovered from
   the asm's addressing ([reg+disp] on a known pointer, loop strides), from the recorded calls'
   memory (build/xngine/records) and from the game-side structs (include/structs.h,
   include/records.h). docs/engine/structs.md has, per struct, where its
   instances live, its users, the evidence per field, the confidence and the open questions;
   fields.csv lists every field with its evidence, globals.csv the new global names.

   Conventions (records.h's):
   - packed (#pragma pack(1)); every field has its offset in a `/ * +0xNN * /` comment, every
     struct its size after the closing brace and a compile-time size check (RECORD_SIZE, or
     RECORD_OFFSET on the member a variable-length tail starts at);
   - tags are xn_<subsystem>_<what>; unknown fields are unknown_XX (XX = the hex offset), and
     bytes no instruction touches are pad_XX;
   - fixed point is noted per field: 2.28 (1.0 = 0x10000000: the sine tables, rotation
     matrices), 16.16, 24.8...; angles are 2048 to a turn;
   - a pointer field holds a flat address, as every pointer in the game does.

   The native build (docs/port.md: arm64, 8-byte pointers). Under Watcom every size below is
   exact. Natively:
   - a struct only the engine uses may grow with its pointers (RECORD_SIZE_P /
     RECORD_OFFSET_P, as records.h checks the game's);
   - a struct the game also sees has the layout the game's headers give it natively (the
     model handle, the texture cache's entries and heap blocks, the animation state, the
     polygon's first 16 bytes that the pick reads): XN_NATIVE_SIZE / XN_NATIVE_OFFSET check
     it here, tools/port_sizes.py check against the game's headers;
   - a struct read raw from a data file keeps the file's layout, so a 4-byte slot the engine
     fills with an address after loading cannot hold one: each such slot has a native-only
     layout, documented at the field (the texture image's mapper, a face's texture-axis
     pointer, WOODS.WLD's offset window; the TEXTURE.nnn directory is widened when it is
     loaded, tex.c).

   Use: include it after src/engine/xngine.h when both are used (both define xn_vec3 and
   xn_mat3; this file defines them only when XNGINE_H is not defined). The compiler runs
   under DOS: copy or rename it to an 8.3 name (e.g. xnstruct.h) where it is included. */
#ifndef XN_ENGINE_STRUCTS_H
#define XN_ENGINE_STRUCTS_H

#include "ptrint.h"                   /* iptr, uptr: ints that hold addresses */

#ifndef RECORD_SIZE
#define RECORD_SIZE(tag, n) typedef char tag##_size_check[(sizeof(struct tag) == (n)) ? 1 : -1]
#endif
#ifndef RECORD_OFFSET
#if defined(DAGGER_PORT)
#define RECORD_OFFSET(tag, m, n) \
    typedef char tag##_##m##_offset_check[(__builtin_offsetof(struct tag, m) == (n)) ? 1 : -1]
#else
#define RECORD_OFFSET(tag, m, n) \
    typedef char tag##_##m##_offset_check[((unsigned)&((struct tag *)0)->m == (n)) ? 1 : -1]
#endif
#endif
/* a struct holding pointers: exact under Watcom, natively it may only grow (records.h's) */
#ifndef RECORD_SIZE_P
#if defined(DAGGER_PORT)
#define RECORD_SIZE_P(tag, n) typedef char tag##_size_check[(sizeof(struct tag) >= (n)) ? 1 : -1]
#define RECORD_OFFSET_P(tag, m, n) \
    typedef char tag##_##m##_offset_check[(__builtin_offsetof(struct tag, m) >= (n)) ? 1 : -1]
#else
#define RECORD_SIZE_P(tag, n) RECORD_SIZE(tag, n)
#define RECORD_OFFSET_P(tag, m, n) RECORD_OFFSET(tag, m, n)
#endif
#endif
/* the native size and offsets of a struct the game also sees (nothing under Watcom) */
#if defined(DAGGER_PORT)
#define XN_NATIVE_SIZE(tag, n) \
    typedef char tag##_native_size_check[(sizeof(struct tag) == (n)) ? 1 : -1]
#define XN_NATIVE_OFFSET(tag, m, n) \
    typedef char tag##_##m##_native_offset_check[(__builtin_offsetof(struct tag, m) == (n)) ? 1 : -1]
#else
#define XN_NATIVE_SIZE(tag, n) typedef char tag##_native_size_check[1]
#define XN_NATIVE_OFFSET(tag, m, n) typedef char tag##_##m##_native_offset_check[1]
#endif
/* a member's offset as an int constant, where the asm wrote the number (records.h's
   REC_OFFSETOF): the same constant under Watcom, the native layout's natively */
#if defined(DAGGER_PORT)
#define XN_OFFSETOF(type, m) ((int)__builtin_offsetof(type, m))
#else
#define XN_OFFSETOF(type, m) ((int)&((type *)0)->m)
#endif

#pragma pack(1)

/* xngine.h (src/engine) defines these two too: include it first when both are used */
#ifndef XNGINE_H
typedef struct xn_vec3 {
    int x, y, z;
} xn_vec3;
typedef struct xn_mat3 {
    int m[3][3];                    /* rows, 2.28 fixed point (1.0 = 0x10000000) */
} xn_mat3;
#endif

#pragma pack()

/* every tag, so any fragment may point at any other */
struct xn_poly;
struct xn_span;
struct xn_poly_vertex;
struct xn_poly_clip_state;
struct xn_vert_cam;
struct xn_vert_screen;
struct xn_vert_flags;
struct xn_sort_pair;
struct xn_light;
struct xn_light_ref;
struct xn_light_point_slots;
struct xn_tex_block;
struct xn_tex_entry;
struct xn_tex_archive;
struct xn_tex_image;
struct xn_tex_frame;
struct xn_tex_unpack_strip;
struct xn_tex_unpack_entry;
struct xn_tmap_step;
struct xn_tmap_copy;
struct xn_model;
struct xn_model_frame;
struct xn_model_face_data;
struct xn_model_face_point;
struct xn_model_face;
struct xn_model_sphere_face;
struct xn_model_sphere;
struct xn_model_handle;
struct xn_model_matrix_slot;
struct xn_model_matrix_pool;
struct xn_model_draw_state;
struct xn_collide_probe_sphere;
struct xn_collide_probe;
struct xn_collide_hit;
struct xn_collide_hits;
struct xn_collide_scratch;
struct xn_collide_seg_state;
struct xn_collide_sph_state;
struct xn_scratch;
struct xn_anim;
struct xn_ascr;
struct xn_wld_header;
struct xn_wld_cell_header;
struct xn_wld_cell;
struct xn_wld_bands;
struct xn_world_nature_odds;
struct xn_terrain_vert_coord;
struct xn_flat;
struct xn_light_shader;
struct xn_sky_star;
struct xn_snow_flake;
struct xn_view;
struct xn_mouse_press;
struct xn_mouse_state;
struct xn_kbd_state;
struct xn_joy_axes;
struct xn_joy_state;
struct xn_gfx_state;
struct xn_rm_regs;
struct xn_vesa_mode_info;
struct xn_fnt_glyph;
struct xn_fnt_file;
struct xn_font_state;
struct xn_vid_header;
struct xn_vid_player;

/* ==== Renderer core: polygons, spans, vertex buffers ====================================== */

#pragma pack(1)

struct xn_light_ref;
struct xn_tex_entry;
struct xn_model_handle;
struct xn_model_face;

/* ---- the polygon record ------------------------------------------------------------------ */

/* A drawn polygon (100 bytes), taken from the frame pool xn_render_poly_pool (0xDB320,
 * 1000 records to 0xF39E0) through xn_render_poly_next (0xCEA64, += 0x64). The first record
 * of every frame is the background record (xn_render_begin_frame: span routine
 * xn_render_frame_row_end, the sentinel span's polygon). Model faces (xn_model_draw_faces
 * 1403AF), terrain cells (xn_terrain_draw_cells 13EFD7) and flats (xn_flat_add 154D20, which
 * gives the same 100 bytes its own layout: struct xn_flat) take records; a span node points at
 * its record; [+3Ch] is called for every span of it (xn_render_frame 12A870: eax = the
 * record, esi = the span node, edi = destination - 1, ebp = pixel count, ebx = x - centre x).
 * Fields +18h..+20h and +50h..+58h change meaning when the record's setup routine runs on its
 * first span (unions below). The pick (xn_render_pick 12A608) returns a record: include/
 * structs.h struct xn_pick_hit is its first 8 bytes. */
struct xn_poly {
    union {
        struct xn_model_face *face; /* +0x00: model faces: the face (arch3d_plane) in the
                                       model data: [+1] base shade row (15BC42), [+2] texture,
                                       [+0Ch] packed texture origin, [+14h] plane distance,
                                       [+1Ch] its texture axes (15BB8C); 0 for terrain */
        unsigned int flat_image;    /*       flats (struct xn_flat): archive<<7 | record */
    };
    union {
        struct xn_model_handle *handle; /* +0x04: model faces: the drawn model's handle
                                       (patch_140497; 15BC42 sets handle+39h bit 1) */
        iptr owner;                 /*       1 terrain (13EC17), 0 flats: xn_pick_hit.model
                                       (pointer-wide: the pick reads the whole slot) */
    };
    int shader_falloff[3];          /* +0x08: the asm's light shaders kept their per-light
                                       falloffs here; canonical C keeps them in the shader
                                       record (struct xn_light_shader), and nothing uses these */
    union {
        struct xn_light_ref *light_list; /* +0x14: terrain cells before setup: their
                                       directional light list (13EC14, read by 15BC9C) */
        unsigned char *shade_row;   /*       after setup, light index 4: the shade row (256
                                       bytes of xn_shade_table) for the whole polygon */
        const struct xn_light_shader *shader; /* after setup, light index 8: the polygon's
                                       light shader (xlight.h: light.c's frame pool) */
    };
    union {
        struct {
            int cam_x;              /* +0x18: model faces before setup: the first vertex in
                                       camera space (1404AD), the texture origin's anchor */
            int cam_y;              /* +0x1C */
            int cam_z;              /* +0x20 */
        };
        struct {
            unsigned short tex_u0;  /* +0x18: after setup (15BC37) / terrain (13F4ED,
                                       13F5A5): the packed texture origin: u in the low word, */
            unsigned short tex_v0;  /* +0x1A:   v in the high word (8.8 texels) */
        };
    };
    int u_dx;                       /* +0x24: u/z gradient per screen x (15BAF2: the 8-px step
                                       >> 3); terrain: from the u axis setter 13F4CC.. */
    int u_dy;                       /* +0x28: u/z per screen y (row = xn_render_row_y) */
    int u_c;                        /* +0x2C: u/z constant (at the view centre) */
    int v_dx;                       /* +0x30: v/z per screen x */
    int v_dy;                       /* +0x34: v/z per screen y */
    int v_c;                        /* +0x38: v/z constant */
    void (*span_fn)(struct xn_poly *, const struct xn_span *, int, int, unsigned char *);
                                    /* +0x3C: the span routine (xspan.h xn_span_fn): first a
                                       setup (xn_render_span_setup(kind)), which stores the
                                       polygon's routine here and runs it */
    union {
        struct xn_tex_entry *tex;   /* +0x40: before setup: the texture's cache entry
                                       (xn_tex_cache_lookup) */
        unsigned char *texels;      /*       after textured setup: the texels (stride 256) */
        unsigned int colour4;       /*       after solid setup: the colour byte x 4
                                       (xn_colour_fill_table[entry byte 1]) */
    };
    void (*tmap)(void);             /* +0x44: the texture's compiled mapper copy (image +0Ah;
                                       xn_tmap_pool), called by xn_span_tex_lit */
    int *normal;                    /* +0x48: model faces: the face's normal (12 bytes in the
                                       model's normal list), read by the light dispatchers and
                                       by xn_model_push_player_from_pick C810C */
    unsigned int wrap_mask;         /* +0x4C: the image's packed wrap masks (image +0):
                                       0xFF | u mask << 8 | 0xFF << 16 | v mask << 24, ANDed with
                                       the packed u/v coordinate in the span loops */
    union {
        struct {
            int u_step;             /* +0x50: u/z step for 8 pixels (16 in the 16-px
                                       routines, which double it: 155F60) */
            int v_step;             /* +0x54: v/z step */
            int inv_z_step;         /* +0x58: 1/z step (models: 140466) */
        };
        struct {
            int nx;                 /* +0x50: terrain cells before setup: the face normal in
                                       camera space (13EBD9..), read by 15C08C; setup 12A740 */
            int ny;                 /* +0x54:   replaces it with the 16-px steps */
            int nz;                 /* +0x58 */
        };
    };
    int inv_z_dx;                   /* +0x5C: d(1/z)/dx per pixel (xn_span_dzdx when the
                                       record was built); the S-buffer compares depths with it */
    int dx_per_inv_z;               /* +0x60: 2^32 / [+5Ch] (15BC55, 15BCA8; 0 when +5Ch is 0:
                                       the divide-error handler) */
};                                  /* +0x64 */
RECORD_SIZE_P(xn_poly, 100);
/* natively the pick's struct xn_pick_hit (include/structs.h) is the first 16 bytes */
XN_NATIVE_OFFSET(xn_poly, handle, 8);
XN_NATIVE_OFFSET(xn_poly, shader_falloff, 16);

/* ---- spans: the S-buffer ----------------------------------------------------------------- */

/* A span node (16 bytes). Every screen row keeps a sorted list of non-overlapping spans; the
 * row's head is an xn_span of its own in xn_render_span_rows (0xF39F0 + row * 16, of which
 * only .next is used); every list ends at the sentinel xn_render_span_sentinel (0xF39E0:
 * next = itself, x_start = the clip right edge, x_end = that + 1, poly = the background
 * record). Nodes come from xn_render_span_next (0xCEA60, after the row heads, += 16; a
 * split takes 32). Writers: xn_render_begin_frame 12A4F0, xn_span_insert 158D04. Readers:
 * xn_render_frame 12A870, xn_render_fill_background 12A98B, xn_render_pick 12A608,
 * xn_model_is_occluded 140910, xn_flat_span_clip 155200, xn_water_draw 12F79C. */
struct xn_span {
    struct xn_span *next;           /* +0x00: the next span to the right */
    unsigned short x_end;           /* +0x04: last pixel + 1 */
    unsigned short x_start;         /* +0x06: first pixel */
    int inv_z;                      /* +0x08: 1/z at x_start (2^40 / z, as the projectors
                                       store it); the span routines index the reciprocal
                                       table with (inv_z >> 13) & 0xFFFF */
    struct xn_poly *poly;           /* +0x0C: the polygon drawn there */
};                                  /* +0x10 */
RECORD_SIZE_P(xn_span, 16);

/* ---- the clipper's vertices --------------------------------------------------------------- */

/* A polygon vertex of the clipper (16 bytes): xn_poly_vertex_buf_a (0x158EA0) and _b
 * (0x1590A0), 32 each; the clipper ping-pongs between them (xn_poly_clip_plane copies the
 * first two after the last, so a polygon has at most 30). Camera space while clipping; the
 * projectors (158360, 158420, 1585C0) then replace x, y, z in place. */
struct xn_poly_vertex {
    int x;                          /* +0x00: camera x; projected: screen x << 5
                                       (cx*256+128 + x*sx*256/z, >> 3) */
    int y;                          /* +0x04: camera y; projected: screen row */
    int z;                          /* +0x08: camera z; projected: 1/z = 2^40 / z */
    unsigned char outcode;          /* +0x0C: 1 x<-z, 2 x>z, 4 y>z, 8 y<-z, 10h z<near,
                                       20h z>far (xn_poly_outcode) */
    char pad_0d[3];                 /* +0x0D */
};                                  /* +0x10 */
RECORD_SIZE(xn_poly_vertex, 16);

/* The clipper's state (0x158200, 18 bytes; separate globals in names.csv) */
struct xn_poly_clip_state {
    struct xn_poly_vertex *src;     /* +0x00: xn_poly_clip_src: the polygon */
    struct xn_poly_vertex *dst;     /* +0x04: xn_poly_clip_dst: the other buffer */
    struct xn_poly_vertex *src_end; /* +0x08: xn_poly_clip_src_end */
    void (*intersect)(void);        /* +0x0C: xn_poly_clip_intersect_fn: the plane's
                                       intersection routine (158864 .. 158B1C) */
    unsigned char outcode_and;      /* +0x10: xn_poly_clip_outcode_and */
    unsigned char outcode_or;       /* +0x11: xn_poly_clip_outcode_or (xn_cam_cull_sphere
                                       leaves a sphere's outcode here) */
};                                  /* +0x12 */
RECORD_SIZE_P(xn_poly_clip_state, 18);

/* ---- the transformed-vertex arrays ---------------------------------------------------------- */

/* Three parallel arrays of 1024 12-byte elements, indexed by a vertex's byte offset (index *
 * 12, as the model faces store their points): the terrain fills them as a 32 x 32 grid (row
 * stride 0x180, xn_terrain_transform_grid 13E8C8), each model draw as its vertices
 * (xn_model_transform_face_verts 1404FA, lazily per face). */

/* xn_vert_cam (0xCEAC0): camera space (the view matrix applied: the frustum is x = +-z,
 * y = +-z) */
struct xn_vert_cam {
    int x;                          /* +0x00 */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
};                                  /* +0x0C */
RECORD_SIZE(xn_vert_cam, 12);

/* xn_vert_screen (0xD1AC0): projected, as struct xn_poly_vertex after projection (only when
 * the vertex is inside the frustum: outcode 0) */
struct xn_vert_screen {
    int sx;                         /* +0x00: screen x << 5 */
    int sy;                         /* +0x04: screen row */
    int inv_z;                      /* +0x08: 2^40 / z */
};                                  /* +0x0C */
RECORD_SIZE(xn_vert_screen, 12);

/* xn_vert_flags (0xD4AC0): three bytes per vertex, different for terrain and models */
struct xn_vert_flags {
    union {
        unsigned char done;         /* +0x00: models: 1 once transformed in this draw
                                       (xn_model_clear_vert_flags 140A28 clears nverts) */
        unsigned char terrain_flat; /*       terrain: the vertex's xn_world_flat_layer byte
                                       (bits 0-1 the cell's flips; 13F074) */
    };
    unsigned char terrain_outcode;  /* +0x01: terrain: outcode | 80h (the height byte's bit 7:
                                       the cell is two triangles) */
    union {
        unsigned char outcode;      /* +0x02: models: the vertex's outcode (1404FA), copied to
                                       xn_poly_vertex.outcode by 158452 */
        unsigned char terrain_tile; /*       terrain: the xn_world_tile_layer byte (texture
                                       & 3Fh, rotation bits 6-7; 13F06E) */
    };
    char pad_03[9];                 /* +0x03 */
};                                  /* +0x0C */
RECORD_SIZE(xn_vert_flags, 12);

/* ---- draw lists ---------------------------------------------------------------------------- */

/* A {key, value} pair of a draw list, sorted ascending by key by xn_render_sort_pairs
 * (15810E, a quicksort; eax = the list, edx = 0, ebx = the last pair's byte offset): the
 * model queue (xn_model_queue 0x13F784, 200 pairs: key = |p| - radius, value = the model
 * handle) and the flat sort list (xn_flat_sort_list 0x153C44, 512 pairs: key = -z, value =
 * the struct xn_flat record) */
struct xn_sort_pair {
    int key;                        /* +0x00 */
    void *value;                    /* +0x04 */
};                                  /* +0x08 */
RECORD_SIZE_P(xn_sort_pair, 8);

#pragma pack()

/* ==== Lights, shading, fog, the texture cache and the tmap pool =========================== */

#pragma pack(1)

struct xn_poly;
struct xn_tex_archive;
struct xn_tex_image;

/* ---- lights -------------------------------------------------------------------------------- */

/* A light of the frame (29 bytes): xn_light_table (0x136540), 32 slots and a 33rd at 0x1368E0
 * that nothing writes (its intensity 0 ends the scans when all 32 are used). xn_light_reset
 * (136AB4, every frame) sets every slot's intensity to -1 and xn_light_next to the table;
 * xn_light_add (136AD8 / 136AF0: x, y, z, intensity, radius, type) fills the next slot,
 * at most 31 a frame (xn_light_count is incremented first and must stay below 32), and drops
 * a point light whose sphere xn_cam_cull_sphere rejects. Position: world units when added;
 * xn_light_to_view (136BD8, from xn_render_draw_flats, after the models) rewrites it in view
 * space << 8 in place, for the flats. Readers: xn_model_build_light_list 140606,
 * xn_terrain_build_light_list 13E63C, xn_flat_span_light_setup 155610, and through the light
 * lists 15BC42 / 15BC9C. */
struct xn_light {
    int x;                          /* +0x00: world x (view x << 8 after xn_light_to_view);
                                       directional lights: the direction */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
    int intensity;                  /* +0x0C: 1..32 (clamped; the game passes 16 for torches,
                                       sun_light for the sun); -1 a free slot, 0 the end */
    int reach;                      /* +0x10: intensity / 2 * radius (radius clamped to 512);
                                       the culling sphere's radius is reach << 6 */
    int range_sq;                   /* +0x14: radius^2 * (intensity / 2): the point light's
                                       squared range (models compare |P - L|^2 - r^2 with it) */
    int type;                       /* +0x18: 0 point, 4 ignored by both dispatchers,
                                       8 directional (the sun, the automap; never culled);
                                       a byte offset into the dispatch tables */
    unsigned char pad_1c;           /* +0x1C: never read or written */
};                                  /* +0x1D */
RECORD_SIZE(xn_light, 29);

/* A light of a light list (24 bytes): lists are built per frame in the pool
 * xn_render_light_list_pool (0x116C80, 400 entries to 0x119200) through
 * xn_render_light_list_next (0xCEA68), each ended by a -1 light pointer: one per drawn model
 * (xn_model_build_light_list 140606; the model handle's +4 points at it) and one for the
 * terrain (xn_terrain_build_light_list 13E63C: directional lights only; every terrain
 * polygon's +14h points at it). Read by xn_light_setup_poly 15BC42 (through the polygon's
 * handle) and xn_light_setup_terrain 15BC9C, which call the dispatchers with esi = the entry,
 * edi = the polygon. The end mark is the light pointer alone (an iptr -1; the next list
 * starts PTR_SIZE bytes on, the asm's 4). */
struct xn_light_ref {
    struct xn_light *light;         /* +0x00: -1 ends the list; light->type picks the routine */
    int x;                          /* +0x04: models: the light in object space (point: world
                                       position - model position, << 8, through the transposed
                                       object rotation; directional: the direction rotated);
                                       terrain: the direction in view space */
    int y;                          /* +0x08 */
    int z;                          /* +0x0C */
    int intensity;                  /* +0x10: light->intensity (terrain: negated) */
    int range_sq;                   /* +0x14: models: light->range_sq << 4 (15BF9F compares the
                                       squared distance to the face plane with it); terrain:
                                       not written */
};                                  /* +0x18 */
RECORD_SIZE_P(xn_light_ref, 24);

/* The point lights that reach the polygon being set up (0x158C28, 64 bytes; names.csv names
 * each array): xn_light_add_point (15BF75) fills slot ebp/4 = 0..2, the shader builders
 * 15BCF6/15BD78/15BE4D patch them into the templates. Arrays of three, indexed together. */
struct xn_light_point_slots {
    int intensity[3];               /* +0x00: xn_light_point_intensity: (2^32/d^2 ... ) *
                                       ref->intensity, > 256 */
    int falloff[3];                 /* +0x0C: xn_light_point_falloff: 2^30 / d^2, > 128;
                                       copied into the polygon's shader_falloff */
    int x[3];                       /* +0x18: xn_light_point_x: the light's foot point on the
                                       face plane, view space >> 8 (squares-table index) */
    int y[3];                       /* +0x24: xn_light_point_y */
    int z[3];                       /* +0x30: xn_light_point_z */
    unsigned char *shade_row;       /* +0x3C: xn_light_shade_row: the polygon's base shade
                                       row (base row + ambient + xn_shade_table); directional
                                       lights add whole rows to it */
};                                  /* +0x40 */
RECORD_SIZE_P(xn_light_point_slots, 64);

/* ---- the texture cache ('SET:') -------------------------------------------------------------- */

/* A block of the texture heap (22-byte header, then the data): one TEXTURE.nnn archive per
 * used block. The heap (cfg_texture_memory KB, xn_tex_heap_base) is a doubly linked list in
 * address order; xn_tex_heap_head (0x1343E4) is a header of its own (flags 1, never merged)
 * whose next is the first block. xn_tex_heap_alloc_first_fit 1361B8 splits, xn_tex_heap_free
 * 13622F merges with free neighbours, xn_tex_heap_find_lru 136173 picks the used block with
 * the oldest tick whose archive was not used this frame, xn_tex_heap_evict 136145 frees it
 * (and clears its archive slot). */
struct xn_tex_block {
    struct xn_tex_block *next;      /* +0x00: 0 the last */
    struct xn_tex_block *prev;      /* +0x04: the first block's is &xn_tex_heap_head */
    int size;                       /* +0x08: data bytes after the header */
    unsigned short flags;           /* +0x0C: bit 0 used */
    struct xn_tex_archive **slot;   /* +0x0E: the xn_tex_archives slot that points at the data
                                       (135F2F; cleared on eviction) */
    unsigned int last_tick;         /* +0x12: BIOS tick of the last lookup (135D5A writes
                                       archive - 4) */
};                                  /* +0x16 */
RECORD_SIZE_P(xn_tex_block, 22);
XN_NATIVE_SIZE(xn_tex_block, 34);       /* = include/structs.h struct tex_block natively */

/* A TEXTURE.nnn record's directory entry (DFU TextureFile RecordDirectoryEntry, 20 bytes) as
 * the cache keeps it: xn_tex_load_archive (135EAB) turns the offset into a pointer and fills
 * the two null dwords. xn_tex_cache_lookup (135D00) returns one (include/structs.h
 * struct tex_cache_entry: its +0x0C image); the polygon's +40h holds it until setup.
 * Natively the two pointers make an entry 28 bytes (the game's tex_cache_entry agrees): the
 * load widens the file's 20-byte entries in place (tex.c, tex_widen_directory). */
struct xn_tex_entry {
    unsigned char type1_lo;         /* +0x00: DFU Type1 (low byte) */
    unsigned char solid_colour;     /* +0x01: DFU Type1's high byte: the colour a polygon is
                                       filled with when the record is not compiled (kind 0:
                                       xn_render_span_setup_solid 12A719) */
    struct xn_tex_image *image;     /* +0x02: DFU Offset, relocated: the record's header */
    unsigned short type2;           /* +0x06: DFU Type2 */
    unsigned short unknown_08;      /* +0x08: DFU Unknown1, low word */
    unsigned short blend_index;     /* +0x0A: DFU Unknown1, high word: the flats' blend table,
                                       xn_shade_blend_tables[it] (xn_flat_draw 154E38); 1 in
                                       the archives xn_tex_archive_set_translucent CDC99 marks
                                       (ghost 273, wraith 278), else 0 */
    struct xn_tex_image *current;   /* +0x0C: DFU NullValue1: the image the last lookup chose
                                       (the frame decoded this frame; tex_cache_entry.image) */
    int kind;                       /* +0x10: DFU NullValue2: 0, or 4 when a mapper copy was
                                       compiled: an index into xn_render_span_setups (0 solid
                                       colour, 4 textured) */
};                                  /* +0x14 */
RECORD_SIZE_P(xn_tex_entry, 20);
XN_NATIVE_SIZE(xn_tex_entry, 28);
XN_NATIVE_OFFSET(xn_tex_entry, current, 16);    /* tex_cache_entry.image natively */
#define XN_TEX_FILE_ENTRY 20            /* a directory entry's bytes in the file */

/* A loaded archive, in a heap block's data: the TEXTURE.nnn file read whole (its 26-byte
 * header, then the directory, then the records). xn_tex_archives[archive] points here;
 * xn_tex_record_offsets[record] = record * 20 indexes the directory. Natively the file is
 * read (record_count * 8) bytes further on and its directory widened into the native entries
 * in front of it (the game reads the name at +2 and the entries); the images' offsets count
 * from the file's start, there. */
struct xn_tex_archive {
    unsigned short record_count;    /* +0x00: DFU RecordCount */
    char name[24];                  /* +0x02 */
    struct xn_tex_entry entries[1]; /* +0x1A: record_count of them */
};
RECORD_OFFSET(xn_tex_archive, entries, 0x1A);

/* A record's image header (DFU TextureFile RecordHeader, 28 bytes; include/structs.h
 * struct texture_header, which agrees) with what xn_tex_load_archive writes over it: the
 * packed wrap masks over x/y and the compiled mapper's address over the record size, and
 * xn_tex_cache_lookup the decoded frame's offset over data_offset. The layout is the file's
 * natively too (the game reads it): the mapper's slot holds the low 32 bits of its address
 * there, which nothing reads (the canonical span routines run no mapper copies); the decoded
 * frame's offset fits because the unpack buffer is the heap block's tail natively (tex.c). */
struct xn_tex_image {
    union {
        struct {
            short x;                /* +0x00: DFU OffsetX */
            short y;                /* +0x02: DFU OffsetY */
        };
        unsigned int wrap_mask;     /* +0x00: compiled records: 0xFF | (size mask of width)
                                       << 8 | 0xFF << 16 | (size mask of height) << 24
                                       (xn_tex_size_mask; 135F98): xn_poly.wrap_mask */
    };
    unsigned short width;           /* +0x04 */
    unsigned short height;          /* +0x06 */
    unsigned short flags;           /* +0x08: DFU CompressionFlags: 0x1000 not compiled or
                                       checked (RLE), 0x100 set by xn_tex_check_transparent
                                       when a pixel is 0 (then not compiled either); flats:
                                       drawn with 0x8000 added */
    int size;                       /* +0x0A: DFU RecordSize; compiled records: the mapper
                                       copy's address (xn_tmap_compile; xn_tmap_rebase gets
                                       it when the frame moves), natively its low 32 bits */
    int data_offset;                /* +0x0E: the pixels, from the start of this header (rows of
                                       256 bytes); animated records: the decoded frame's */
    unsigned short is_normal;       /* +0x12: DFU IsNormal */
    unsigned short frame_count;     /* +0x14: > 1 animated */
    unsigned short frame_time;      /* +0x16: BIOS ticks per frame (xn_anim_ticks / it) */
    short x_scale;                  /* +0x18 */
    short y_scale;                  /* +0x1A */
    int frame_offsets[1];           /* +0x1C: animated: frame_count offsets from +0x1C of RLE
                                       frames (struct xn_tex_frame) */
};
RECORD_OFFSET(xn_tex_image, frame_offsets, 0x1C);

/* An animated record's frame in the file: rows of (zero-run byte, copy-count byte, bytes)
 * pairs until the width is used; xn_tex_decode_frame (136282) unpacks it into the decode
 * buffer at a 256-byte stride */
struct xn_tex_frame {
    unsigned short width;           /* +0x00 */
    unsigned short height;          /* +0x02 */
    unsigned char data[1];          /* +0x04 */
};
RECORD_OFFSET(xn_tex_frame, data, 4);

/* A strip of the decode buffer (xn_tex_unpack_buffer, 0xC0000 bytes of 256-byte rows):
 * frames are packed side by side in strips of rows; xn_tex_unpack_strips (0x1343FA), at most
 * 256, xn_tex_unpack_strip_count of them (cleared every frame) */
struct xn_tex_unpack_strip {
    unsigned short width_left;      /* +0x00: 256 - the width used */
    unsigned char *start;           /* +0x02: the strip's first row in the buffer */
    unsigned short height;          /* +0x06: its rows */
};                                  /* +0x08 */
RECORD_SIZE_P(xn_tex_unpack_strip, 8);

/* A frame decoded this frame (xn_tex_unpack_entries 0x134C02, at most 256; xn_tex_unpack_used
 * is the bytes used, cleared every frame; xn_tex_unpack_find 1363D9 looks the key up) */
struct xn_tex_unpack_entry {
    unsigned int key;               /* +0x00: (archive << 7 | record) << 16 | frame */
    unsigned char *pixels;          /* +0x04: in the decode buffer, stride 256 */
};                                  /* +0x08 */
RECORD_SIZE_P(xn_tex_unpack_entry, 8);

/* ---- the texture-mapper copies ('Shaders') --------------------------------------------------- */

/* Two pixels of the mapper template xn_tmap_template (0x15C300): 52 bytes of code with two
 * texel fetches; xn_tmap_compile (15C274) patches the wrap mask and the texel base of every
 * fetch, xn_tmap_rebase (15C2DC) the texel bases */
struct xn_tmap_step {
    unsigned char code_00[7];       /* +0x00: bswap eax; mov ah,bh; mov edx,esi; and eax, */
    unsigned int wrap_mask_a;       /* +0x07:   the mask: v mask << 8 | u mask (from the
                                       image's wrap_mask) */
    unsigned char code_0b[4];       /* +0x0B: add ebx,ecx; mov dl,[eax+ */
    unsigned int texels_a;          /* +0x0F:   the texel base] (an address as the code's
                                       32-bit displacement) */
    unsigned char code_13[14];      /* +0x13: shade lookup, store the first pixel, next u/v */
    unsigned int wrap_mask_b;       /* +0x21 */
    unsigned char code_25[4];       /* +0x25 */
    unsigned int texels_b;          /* +0x29 */
    unsigned char code_2d[7];       /* +0x2D: store the second pixel */
};                                  /* +0x34 */
RECORD_SIZE(xn_tmap_step, 52);

/* A compiled mapper copy (418 bytes): a slot of xn_tmap_pool (768 slots, 32-aligned in
 * xn_tmap_pool_block; xn_tmap_pool_count used; a full pool sets xn_tex_cache_full). Called
 * by xn_span_tex_lit 16 pixels at a time; for a shorter run a ret is planted at
 * xn_tmap_ret_offsets[n] (int[16]: 0, 28, 52, 80...: pixel n's code) and the 0Fh (bswap) put
 * back after */
struct xn_tmap_copy {
    struct xn_tmap_step steps[8];   /* +0x000: 16 pixels */
    unsigned char tail[2];          /* +0x1A0: ret (C3h), 00h */
};                                  /* +0x1A2 */
RECORD_SIZE(xn_tmap_copy, 418);

#pragma pack()

/* ==== ARCH3D models at run time, model drawing, collision ================================= */

/* ---- model: ARCH3D models at run time, model drawing, collision (group "model") ---------- */

#pragma pack(1)

struct xn_poly;                     /* the 100-byte polygon record (render group) */
struct xn_sort_pair;                /* {int key; void *value} (render group): xn_model_queue's
                                       element, 8 bytes */
struct xn_light_ref;                /* a 24-byte entry of a light list (light group): a model
                                       handle's +0x04 points at its list, -1 ends it */

/* A model's header: the ARCH3D.BSA record (DFU Arch3dFile FileHeader, 64 bytes) as
   xn_model_prepare 0x13FE15 leaves it. Instances: the game's model cache (model_get /
   model_cache_add: one heap copy per model id and variant); a handle's +0x00 points at it.
   Every list is found by an offset from the start of this header, also after prepare (the
   engine adds the model address each time; the only absolute pointer it writes is each
   face's xn_model_face_point.data). Coordinates (points, radius, spheres) are in 1/256 world
   unit (24.8: the handle's world position << 8 is in the same units); normals have an 8-bit
   fraction (1.0 = 256). In memory the lists follow the header: points, faces, normals, face
   data, spheres (10244 of the 10251 records). */
struct xn_model {
    unsigned int version;           /* +0x00: "v2.5" "v2.6" "v2.7" as a dword (0x362E3276 =
                                       "v2.6"); below "v2.6" the vertex offsets are indexes * 4
                                       and prepare multiplies them by 3 */
    int point_count;                /* +0x04: vertices (12 bytes each); 1024 or more is fatal
                                       for a pre-v2.6 file (xn_model_msg_too_many_verts) */
    int face_count;                 /* +0x08: faces = normals = face data entries */
    int radius;                     /* +0x0C: bounding radius about the origin, 24.8: prepare
                                       recomputes it as max |point| (0x1407DE; equal to the
                                       file's value) */
    int frame_count;                /* +0x10: animation frames, 0 none (DFU NullValue1: 0 in all
                                       10251 ARCH3D.BSA records, so never used by Daggerfall) */
    int frame_table_offset;         /* +0x14: frame_count struct xn_model_frame (DFU NullValue1) */
    int face_data_offset;           /* +0x18: face_count struct xn_model_face_data (DFU
                                       PlaneDataOffset); a frame selection rewrites it */
    int sphere_offset;              /* +0x1C: the collision spheres, struct xn_model_sphere one
                                       after the other (DFU ObjectDataOffset) */
    int sphere_count;               /* +0x20: DFU ObjectDataCount */
    int unknown_24;                 /* +0x24: DFU Unknown2; no engine read seen */
    char pad_28[8];                 /* +0x28: DFU NullValue2 */
    int point_offset;               /* +0x30: point_count xn_vec3 (DFU PointListOffset); a frame
                                       selection rewrites it */
    int normal_offset;              /* +0x34: face_count xn_vec3 normals, 8-bit fraction,
                                       recomputed by prepare (DFU NormalListOffset); a frame
                                       selection rewrites it */
    int unknown_38;                 /* +0x38: DFU Unknown3 (0 in 10232 records); no engine read */
    int face_offset;                /* +0x3C: the faces, struct xn_model_face one after the other
                                       (8 + 8 * point_count bytes each; DFU PlaneListOffset) */
};                                  /* +0x40 */
RECORD_SIZE(xn_model, 64);

/* An animation frame of a model: the table at model + frame_table_offset (frame_count entries).
   xn_model_calc_uv_axes 0x13FF0E (every frame at prepare) and xn_model_set_frame_regs 0x14037A
   (the handle's frame at draw time) copy the three offsets into the header. Unused by
   Daggerfall's data (frame_count is always 0). */
struct xn_model_frame {
    int point_offset;               /* +0x00: -> xn_model.point_offset */
    int normal_offset;              /* +0x04: -> xn_model.normal_offset */
    int face_data_offset;           /* +0x08: -> xn_model.face_data_offset */
    int pad_0c;                     /* +0x0C: never read */
};                                  /* +0x10 */
RECORD_SIZE(xn_model_frame, 16);

/* A face's texture axes (DFU PlaneData, 24 bytes per face at model + face_data_offset):
   xn_model_calc_face_uv_axes 0x13FF65 computes them for textured faces (texture >= 0x100) from
   the first three points and their u/v (the file holds the same values: record of 0x140284,
   model 634 faces 0-2), xn_poly_tex_gradients 0x15BAC0 turns them into the polygon's u/z and
   v/z gradients through the handle's scaled matrix. */
struct xn_model_face_data {
    xn_vec3 u_axis;                 /* +0x00: gradient of u in object space: 2^33 * (du1 e1/|e1|^2
                                       + du2' e2'/|e2'|^2), e1 = p1-p0, e2' = (p2-p1) minus its
                                       part along e1 */
    xn_vec3 v_axis;                 /* +0x0C: the same for v */
};                                  /* +0x18 */
RECORD_SIZE(xn_model_face_data, 24);

/* A point of a face (DFU PlanePoint, 8 bytes). The second dword holds the file's u/v; prepare
   reuses it in the first three points (every face has at least three). */
struct xn_model_face_point {
    int vertex;                     /* +0x00: the vertex's byte offset in the point list (index *
                                       12; v2.5 files index * 4, made * 3 by prepare): also the
                                       index into xn_vert_cam / xn_vert_screen / xn_vert_flags */
    union {
        short uv[2];                /* +0x04: the file's u, v (texel units with a 4-bit
                                       fraction?); points 1 and 2: deltas from the previous point */
        unsigned int uv_packed;     /* +0x04: point 0 after prepare: (u | v << 16) << 4 as one
                                       dword (0x13FE78); 0x15BC2D subtracts the gradient origin
                                       from it into poly +0x18 */
        int plane_d;                /* +0x04: point 1 after prepare: the plane constant
                                       dot(point0, normal) (24.8 * 8-bit fraction; 0x1407C6) */
#if !defined(DAGGER_PORT)
        struct xn_model_face_data *data; /* +0x04: point 2 after prepare: this face's texture
                                       axes (absolute pointer, 0x1407CA); XN_FACE_DATA */
#else
        int data_offset;            /*       natively the file's 4 bytes cannot hold that
                                       address: the axes' offset from the face (both are in
                                       the model's block); XN_FACE_DATA */
#endif
    };
};                                  /* +0x08 */
RECORD_SIZE(xn_model_face_point, 8);

/* A face (DFU PlaneHeader then its points; the game's struct arch3d_plane): the faces follow
   each other at model + face_offset; the next is at + 8 + 8 * point_count. Face i's normal is
   normals[i] and its texture axes face_data[i]. The game identifies a face by its offset from
   the model's start (collision hits, the spheres' lists, click_face_texture). */
struct xn_model_face {
    unsigned char point_count;      /* +0x00: 3..24 (more is "too many vertices", 0x13FE7F) */
    unsigned char shade;            /* +0x01: darkening in shade rows: 0x15BC5C adds it * 256 to
                                       the polygon's shade row (DFU Unknown1: 0 in 98% of faces;
                                       4, 8, 9, 16, 30, 35...) */
    unsigned short texture;         /* +0x02: archive << 7 | record; below 0x100 (archives 0, 1)
                                       no texture axes are computed (0x13FF42); 0x1404B6 looks it
                                       up in the texture cache */
    int unknown_04;                 /* +0x04: DFU PlaneHeader Unknown2 (the game's floor_sound
                                       byte); no engine read */
    struct xn_model_face_point points[1]; /* +0x08: point_count of them */
};
RECORD_OFFSET(xn_model_face, points, 8);

/* A prepared face's texture axes (point 2's slot), and setting them */
#if !defined(DAGGER_PORT)
#define XN_FACE_DATA(f)         ((f)->points[2].data)
#define XN_SET_FACE_DATA(f, d)  ((f)->points[2].data = (d))
#else
#define XN_FACE_DATA(f) \
    ((struct xn_model_face_data *)((const unsigned char *)(f) + (f)->points[2].data_offset))
#define XN_SET_FACE_DATA(f, d) \
    ((f)->points[2].data_offset = (int)((const unsigned char *)(d) - (const unsigned char *)(f)))
#endif

/* An entry of a collision sphere's face list: 6 bytes, sorted by face offset, descending (the
   merge in 0x14A4C1 / 0x14AD35 relies on it). */
struct xn_model_sphere_face {
    int face;                       /* +0x00: the face's offset from the model's start */
    unsigned short normal4;         /* +0x04: the face's index * 4 (* 3 = byte offset of its
                                       normal) */
};                                  /* +0x06 */
RECORD_SIZE(xn_model_sphere_face, 6);

/* A collision sphere of a model (DFU ObjectData, "N1..N4, SubRecordCount" then 6-byte
   subrecords): at model + sphere_offset, sphere_count of them one after the other; the next is
   at + 0x12 + 6 * face_count. The collision tests keep the spheres a segment or a probe
   sphere meets and merge their face lists. */
struct xn_model_sphere {
    int x;                          /* +0x00: centre in object space, 24.8 */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
    int radius;                     /* +0x0C: 24.8 */
    unsigned short face_count;      /* +0x10 */
    struct xn_model_sphere_face faces[1]; /* +0x12: face_count of them */
};
RECORD_OFFSET(xn_model_sphere, faces, 0x12);


/* A model handle: what the game passes to xn_model_submit 0x1401D4 (eax; edx = the frame) and
   the collision routines (xn_collide_segment_model 0x14A300, xn_collide_spheres_model 0x14AA92,
   eax), and what the queue, the polygons (poly +0x04) and the pick hit (+0x04) point at. 58
   bytes; the engine writes +0x04, +0x08, +0x14..+0x1C, +0x38, +0x39, the game the rest.
   Instances: the data of a type-6 or type-32 object record (record +0x47; object_create_child
   gives them 62 bytes: the game's struct model_instance, which records.h makes only 56 bytes
   long), D_001A945E (the arrow in flight), &block_model.model (block_model +0x04: RMB block
   models of type-43 and type-56 objects, bank_draw_preview's ship; the engine then scribbles
   its +0x04/+0x08/+0x14..+0x1C over the RMB record's DFU Unknown2, Unknown3, XPos1..ZPos1).
   xn_model_set_angles C7F07, xn_model_compose_angles C7F14 and xn_model_set_angles_yaw_offset
   C7F98 take &handle->pad_0c (the game's instance->angles) and address +2/+4/+6 (base angles)
   and +0x20/+0x24/+0x28 (the draw angles) from there. */
struct xn_model_handle {
    struct xn_model *model;         /* +0x00: the prepared ARCH3D record (model_get), 0 none */
    struct xn_light_ref *lights;    /* +0x04: this frame's light list (xn_model_build_light_list
                                       0x140648; from xn_render_light_list_next) */
    struct xn_model_matrix_slot *matrix; /* +0x08: this frame's slot in xn_render_matrix_pool
                                       (xn_model_draw 0x140330) */
    short pad_0c;                   /* +0x0C: never read (the base of the game's "angles") */
    short base_angle_x;             /* +0x0E: the placement's own rotation (pitch), 2048 a turn:
                                       C7F07 writes, C7F14 / C7F98 read (the game: the RDB
                                       model's x_rotation, a door's yaw...) */
    short base_yaw;                 /* +0x10 */
    short base_angle_z;             /* +0x12 */
    int rel_x;                      /* +0x14: (x - xn_cam_x) << 8 (xn_model_cull_and_queue
                                       0x14020C), then the same vector in object axes (xn_model_draw
                                       0x1402D8): the object's origin seen from the eye, 24.8 */
    int rel_y;                      /* +0x18 */
    int rel_z;                      /* +0x1C */
    int x;                          /* +0x20: world position (world units; the game copies the
                                       object's x, y, z) */
    int y;                          /* +0x24 */
    int z;                          /* +0x28 */
    int angle_x;                    /* +0x2C: the draw rotation, xn_mat_from_angles' pitch, yaw,
                                       roll (2048 a turn): C7F14 / C7F98 write it, the game for
                                       arrows (records.h missile_angles) and block models */
    int yaw;                        /* +0x30 */
    int angle_z;                    /* +0x34 */
    unsigned char frame;            /* +0x38: animation frame (submit's edx; clamped to
                                       frame_count - 1 by 0x14037A) */
    unsigned char flags;            /* +0x39: cleared by submit; bit 0: occluded this frame
                                       (0x140363); bit 1: one of its polygons reached the
                                       rasterizer (xn_light_setup_poly 0x15BC48) */
};                                  /* +0x3A */
RECORD_SIZE_P(xn_model_handle, 58);
/* natively the game's struct model_instance (records.h), whose lights and matrix are pointers */
XN_NATIVE_SIZE(xn_model_handle, 70);
XN_NATIVE_OFFSET(xn_model_handle, pad_0c, 24);

/* A model's matrices for one frame: element of xn_render_matrix_pool 0xD7AC0 (struct
   xn_model_matrix_pool below), handed out by xn_render_matrix_next 0xCEA70 (+0x24 per drawn
   model, reset by xn_render_begin_frame 0x12A4F1). xn_model_draw 0x1402C0 stores
   xn_cam_view_matrix * xn_model_rot_matrix (object to camera, 2.28) here and transforms the
   vertices with it (0x14053E); xn_model_scale_matrix 0x1406D6 then rescales it for the
   texture gradients and keeps a second version in light[] of the pool (same index). */
struct xn_model_matrix_slot {
    int m[3][3];                    /* +0x00: rows, 2.28 while the vertices are transformed.
                                       After 0x1406D6, light[i] row r (r = 0, 1) = (m * inv_scale)
                                       >> 31 (xn_cam_inv_scale_x, _y), row 2 = m; view[i] row 0 =
                                       (light row * 2^36 / focal_x) >> 32, row 1 = (light row *
                                       2^33 / focal_y) >> 32, row 2 = m << 1 (record: ratios 12.5
                                       and 90 = focal 200 / 16 and 180 / 2). view[i] is read by
                                       xn_poly_tex_gradients 0x15BAC0 (via handle +0x08), light[i]
                                       by xn_light_add_point 0x15C00F (add ecx, 1C20h) */
};                                  /* +0x24 */
RECORD_SIZE(xn_model_matrix_slot, 36);

/* xn_render_matrix_pool 0xD7AC0..0xDB2FF: 200 slots (the model queue holds at most 199 models,
   no bounds check) and their light-setup copies 0x1C20 bytes on (0xD96E0). */
struct xn_model_matrix_pool {
    struct xn_model_matrix_slot view[200];  /* +0x0000 */
    struct xn_model_matrix_slot light[200]; /* +0x1C20 */
};                                  /* +0x3840 */
RECORD_SIZE(xn_model_matrix_pool, 14400);

/* Not a memory layout: the values xn_model_draw 0x140284 and its helpers patch into their own
   code, gathered as the locals the readable C would use (xn_model_draw_faces 0x1403AF,
   xn_model_transform_face_verts 0x1404FA, xn_model_build_light_list 0x140606). The patch
   addresses are the immediates' / displacements' addresses. */
struct xn_model_draw_state {
    struct xn_model_handle *handle; /* +0x00: 0x140497 (mov [edi+4], imm: poly +0x04); 0x140606
                                       reads it back */
    xn_vec3 rel;                    /* +0x04: handle rel_x..z in object axes: 0x14040B / 0x140412
                                       / 0x140419 (back-face test n.rel + plane_d >= 0 culls) and
                                       0x14052D / 0x140533 / 0x140539 (added to each vertex) */
    struct xn_model_matrix_slot *matrix; /* +0x10: 0x14053E (mov ecx, imm for 0x13749E) */
    xn_vec3 dz_row;                 /* +0x14: row 0 of the slot times 0x14030A (2^48 /
                                       (xn_cam_scale_x * focal_x), from 0x12A41A), high dwords:
                                       0x140446 / 0x14044D / 0x140454; n.dz_row / plane distance
                                       gives poly +0x58 / +0x5C (d(1/z)/dx) */
    xn_vec3 *points;                /* +0x20: the point list: 0x14051C (and +4 at 0x140522, +8
                                       at 0x140528: the x, y, z displacements) */
    xn_vec3 *normals_end;           /* +0x24: normals + 12 * face_count: 0x1404EB (loop end) */
    xn_vec3 world;                  /* +0x28: handle x, y, z: 0x140666 / 0x14066C / 0x140672
                                       (light minus model) */
    int radius_sq;                  /* +0x34: (radius^2 + 0x8000) >> 16 (world units^2):
                                       0x14068B */
};                                  /* +0x38 */
RECORD_SIZE_P(xn_model_draw_state, 56);

/* A sphere of a collision probe (the game's struct probe_sphere): offsets from the probe's
   position in the probe's own axes, world units. */
struct xn_collide_probe_sphere {
    int x;                          /* +0x00 */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
    int radius;                     /* +0x0C: world units (<< 8 in model space, 0x14AC33) */
};                                  /* +0x10 */
RECORD_SIZE(xn_collide_probe_sphere, 16);

/* A collision probe ("sphere set"): the mover's shape that the game passes to
   xn_collide_spheres_model 0x14AA92 (edx) and the dead 0x14AF30 / 0x14B017 (the game's struct
   collide_probe: D_00187B44.., D_00179F48; D_00196D4C points at the one under test). */
struct xn_collide_probe {
    xn_vec3 position;               /* +0x00: world units (the game writes the tested position) */
    int angle_x;                    /* +0x0C: the probe's rotation, pitch / yaw / roll for
                                       xn_mat_from_angles (0x14ABAB; the game leaves them 0:
                                       structs.h pad0C) */
    int yaw;                        /* +0x10 */
    int angle_z;                    /* +0x14 */
    unsigned short sphere_count;    /* +0x18: movzx 0x14AAA7 */
    struct xn_collide_probe_sphere spheres[1]; /* +0x1A: sphere_count of them */
};
RECORD_OFFSET(xn_collide_probe, spheres, 0x1A);

/* A face hit by a collision test (the game's struct collide_hit, 30 bytes). */
struct xn_collide_hit {
    int x;                          /* +0x00: the crossing point, world units (segment test
                                       0x14A62B; the sphere test 0x14AA92 leaves +0x00..+0x0B
                                       as they were) */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
    int face;                       /* +0x0C: the face's offset from the model's start */
    int nx;                         /* +0x10: the face normal in world axes, 8-bit fraction (the
                                       model normal through the model's rotation) */
    int ny;                         /* +0x14 */
    int nz;                         /* +0x18 */
    short t_half;                   /* +0x1C: segment test: where along start..end the plane is
                                       crossed, 16.16 t >> 1 (0x8000 = the end); sphere test:
                                       -1 (0xFFFF) */
};                                  /* +0x1E */
RECORD_SIZE(xn_collide_hit, 30);

/* The hit list the collision tests return (in eax) at big_buffer +0x0000: count then the hits
   (no bound: 136 fit below big_buffer +0x1000, the tests' next work area). The tests return 0
   for a hit in modes 1/2 and -1 for none. */
struct xn_collide_hits {
    int count;                      /* +0x00 */
    struct xn_collide_hit hits[1];  /* +0x04: count of them */
};
RECORD_OFFSET(xn_collide_hits, hits, 4);

/* The collision code's globals, 0x14A100..0x14A247 (data module xn_14A100). */
struct xn_collide_scratch {
    union {
        xn_mat3 probe_matrix;       /* +0x000: 0x14A100 xn_collide_matrix_b: the probe's
                                       rotation (0x14ABB9) */
        xn_vec3 ground_tri[3];      /* +0x000: xn_terrain_height_at 0x14B45B: the ground
                                       triangle for 0x14C021 */
    };
    char pad_024[24];               /* +0x024: 0x14A124, never addressed */
    xn_vec3 flat_normal;            /* +0x03C: 0x14A13C (xn_collide_segment_flat 0x14B1E0) */
    xn_mat3 model_matrix;           /* +0x048: 0x14A148: the handle's rotation (0x14A367) */
    xn_mat3 model_matrix_inv;       /* +0x06C: 0x14A16C: its transpose (0x14AB5F) */
    struct xn_model_handle *model;  /* +0x090: 0x14A190: the model under test (eax) */
    struct xn_collide_probe *probe; /* +0x094: 0x14A194: 0x14AA92's edx */
    xn_vec3 *seg_end;               /* +0x098: 0x14A198: 0x14A300's ebx (world units) */
    xn_vec3 *seg_start;             /* +0x09C: 0x14A19C: 0x14A300's edx */
    xn_vec3 local_end;              /* +0x0A0: 0x14A1A0: the segment end (0x14AA92: the probe
                                       position) in object space, 24.8 */
    xn_vec3 local_start;            /* +0x0AC: 0x14A1AC: the segment start in object space */
    int hit_t;                      /* +0x0B8: 0x14A1B8: 0x14C2AD's t (16.16), -1 no crossing */
    int mode;                       /* +0x0BC: 0x14A1BC: 0 hit list, 1 any sphere hit, 2 the
                                       bounding spheres only */
    xn_vec3 local_pos;              /* +0x0C0: 0x14A1C0: 0x14AA92 the probe position (world);
                                       0x14B1E0 the flat's centre offset */
    xn_mat3 rel_matrix;             /* +0x0CC: 0x14A1CC: model_matrix_inv * probe_matrix
                                       (0x15D170) */
    int build_cell;                 /* +0x0F0: dead 0x14B58E from here: cell size, r * sqrt 2 */
    int build_radius;               /* +0x0F4: its edx, then * 1.2: the spheres' radius */
    int build_cells[3];             /* +0x0F8: cells along x, y, z */
    xn_vec3 build_first;            /* +0x104: first cell's centre */
    xn_vec3 build_centre;           /* +0x110: current cell's centre */
    int *build_cell_list;           /* +0x11C: big_buffer: per-cell face lists */
    int build_left[3];              /* +0x120: loop counters x, y, z */
    char *build_out;                /* +0x12C: its ebx: the sphere output */
    xn_vec3 build_min;              /* +0x130: the points' bounding box */
    xn_vec3 build_max;              /* +0x13C */
};                                  /* +0x148 */
RECORD_SIZE_P(xn_collide_scratch, 0x148);

/* xn_collide_segment_model 0x14A300's own variables, stored in its code segment right after
   its ret (0x14A68F..0x14A6B2). */
struct xn_collide_seg_state {
    xn_vec3 *points;                /* +0x00: 0x14A68F: model + point_offset */
    xn_vec3 *normals;               /* +0x04: 0x14A693: model + normal_offset */
    struct xn_model *model;         /* +0x08: 0x14A697: face offsets are added to it */
    xn_vec3 *normal;                /* +0x0C: 0x14A69B: the normal of the face under test */
    struct xn_model_sphere_face *list; /* +0x10: 0x14A69F: the merged face list (big_buffer
                                       +0x2000 / +0x3000, swapped after each merge) */
    struct xn_model_sphere_face *list_next; /* +0x14: 0x14A6A3: the merge's output */
    int list_count;                 /* +0x18: 0x14A6A7: entries in list */
    struct xn_collide_hits *hits;   /* +0x1C: 0x14A6AB: big_buffer, the result */
    struct xn_collide_hit *hit_next; /* +0x20: 0x14A6AF: where the next hit goes */
};                                  /* +0x24 */
RECORD_SIZE_P(xn_collide_seg_state, 36);

/* xn_collide_spheres_model 0x14AA92's variables, after its ret (0x14AF0C..0x14AF2F). Its other
   work areas: big_buffer +0x1000 the probe spheres that meet the bounding sphere (pointers),
   +0x1400 those spheres in object space (16 bytes: x, y, z, radius, 24.8). */
struct xn_collide_sph_state {
    xn_vec3 *points;                /* +0x00: 0x14AF0C */
    xn_vec3 *normals;               /* +0x04: 0x14AF10 */
    struct xn_model *model;         /* +0x08: 0x14AF14 */
    struct xn_collide_hits *hits;   /* +0x0C: 0x14AF18: big_buffer, the result */
    struct xn_collide_hit *hit_next; /* +0x10: 0x14AF1C */
    struct xn_model_sphere_face *list; /* +0x14: 0x14AF20 */
    struct xn_model_sphere_face *list_next; /* +0x18: 0x14AF24 */
    int list_count;                 /* +0x1C: 0x14AF28 */
    int probe_count;                /* +0x20: 0x14AF2C: probe spheres that met the bounding
                                       sphere */
};                                  /* +0x24 */
RECORD_SIZE_P(xn_collide_sph_state, 36);

/* The shared scratch vectors at 0x120288..0x1202B3 (data of xn_120200): free for any routine
   between frames' uses; the names.csv names come from xn_render_pick. Users: xn_cam_cull_sphere
   0x15CF18 (a = the centre in view space, x and y * 2 * inv_scale), the model queue 0x14023B,
   xn_model_calc_face_uv_axes 0x13FF65 (a, b, len2, t), xn_model_calc_face_normal 0x140845 (a,
   b: the cross product's inputs), the centroids 0x1408D3 / 0xC80CC (a), xn_render_pick
   0x12A608 (a = the picked point, b.x/b.y the screen point, flat), the flats 0x1535DC.. /
   0x154EAB / 0x155508, the terrain 0x13EB4E.., xn_collide_point_in_face_v2 0x15CC37 (flat). */
struct xn_scratch {
    xn_vec3 a;                      /* +0x00: 0x120288 xn_pick_view_x, 0x12028C xn_pick_view_y,
                                       0x120290 pick_distance */
    xn_vec3 b;                      /* +0x0C: 0x120294 */
    xn_vec3 flat;                   /* +0x18: 0x1202A0 xn_pick_flat_x / _y / _z */
    int len2;                       /* +0x24: 0x1202AC: 13FF65 |e1|^2 >> 8 */
    int t;                          /* +0x28: 0x1202B0: 13FF65 (e1.e2 / |e1|^2) 16.16 */
};                                  /* +0x2C */
RECORD_SIZE(xn_scratch, 44);

#pragma pack()

/* ==== World (WOODS.WLD), terrain, flats, animation, sky =================================== */

/* world.h: XnGine's world group: WOODS.WLD and the streaming window, the terrain, flats
   (billboards), the ASCR animation state and the sky/weather data. */

#pragma pack(1)

struct xn_sort_pair;                /* the render group's {int key; void *value} draw-list pair */

/* ---- the ASCR animation interpreter (xn_C0100) ------------------------------------------- */

/* A creature's animation state, run by the ASCR interpreter: xn_anim_reset C010F,
   xn_anim_update C013B, xn_anim_tick C019C and the opcodes C020A..C02DF (esi = the state).
   Instances: the game's struct monster_anim (include/records.h) at monster+0x2C1 (the
   character record + 0x27A) of every type-18 creature; the game passes it in eax. 0x18 bytes
   are the engine's; the game's struct has one more byte (+0x18) that no engine code touches. */
struct xn_anim {
    unsigned short frame;           /* +0x00: the image frame to draw (a frame byte b >= 80h in
                                       the script gives -b-1; opcode 3 gives its operand);
                                       object_draw_cb passes it to xn_flat_add */
    unsigned short frame_copy;      /* +0x02: written with +0x00 every time; nothing reads it */
    unsigned char *script;          /* +0x04: the ASCR record (struct xn_ascr) */
    unsigned char *pos;             /* +0x08: the next opcode in the script; 0 = stopped (idle) */
    unsigned short tick_divisor;    /* +0x0C: xn_anim_ticks per step (the animation speed); the
                                       game copies the image's frame_time (texture header +0x16)
                                       here and calls it frame_count */
    unsigned short last_step;       /* +0x0E: xn_anim_ticks / tick_divisor at the last step:
                                       at most one step per change of the quotient */
    unsigned short events;          /* +0x10: opcode 2 ORs its word in: bit 0 strike, bit 1
                                       missile (the game clears them); bit 15 mirrored (opcode
                                       9, and the game's monster_set_action) */
    unsigned char wait;             /* +0x12: steps left before the next opcode; FFh = hold
                                       (a frame+wait opcode with wait 0) */
    unsigned char opcode10_byte;    /* +0x13: opcode 10's operand (FFh after reset); no reader */
    unsigned char request;          /* +0x14: the state to start (an index into the ASCR state
                                       table: 0, 8, 16 ... 60); FFh = none */
    unsigned char facing;           /* +0x15: 0..4, the view of the sprite (game-written; the
                                       reset clears it) */
    unsigned char record_group;     /* +0x16: the texture record group of the state (opcode
                                       11: 0 move, 5 attack, 15 idle, 20 spell...); the game
                                       draws record record_group + facing */
    unsigned char state;            /* +0x17: the state last started (copied from +0x14) */
};                                  /* +0x18 */
RECORD_SIZE_P(xn_anim, 24);
XN_NATIVE_SIZE(xn_anim, 32);            /* natively the game's struct monster_anim's first 32 */

/* The ASCR record (MONSTER.BSA ASCRnnnn.ANC, kept in memory as read): the header, a word
   state table, a byte table the engine never reads, then the script bytes. 60 records; 59
   have 98 states (scripts start at 0x12C), ASCR0029 (the seducer) 120 (at 0x16E). The game
   loads it (monster_init, monster_reload_anim_cb) into monster_anim_records D_00190704 and
   xn_anim.script. Script: a byte >= 80h is a frame (-b-1) and yields; 0..11 are opcodes
   (handlers in xn_anim_opcodes 0xC0000), each followed by its operands. */
struct xn_ascr {
    unsigned short size;            /* +0x00: the record's size in bytes (= the BSA length) */
    unsigned short restart;         /* +0x02: offset of state 0's entry; opcode 5 (restart)
                                       reads it */
    unsigned short state_count;     /* +0x04: entries in the state table (98; scripts start at
                                       6 + 3 * state_count); no engine reader */
    unsigned short state_entry[1];  /* +0x06: [state_count] offset of each state's script from
                                       the record start; 8000h = none (use state_entry[0]);
                                       followed by state_count unread bytes, then the scripts */
};
RECORD_OFFSET(xn_ascr, state_entry, 6);

/* ---- WOODS.WLD (xn_C2D00) ---------------------------------------------------------------- */

/* The WOODS.WLD header (0x90 bytes at file offset 0), read by xn_world_read_header C322D into
   xn_world_header 0xC27E9; +0x0C is then overwritten with the offset-window pointer. DFU
   WoodsFile.cs FileHeader: OffsetSize, Width, Height, NullValue1, DataSection1Offset,
   Unknown1, Unknown2, HeightMapOffset, NullValue2[28]. Then in the file: the cell offset
   table, the 0x400-byte band table, a 1000x500 byte map, the 47-byte cell records. */
struct xn_wld_header {
    unsigned int offsets_size;      /* +0x00: bytes of the cell offset table (2000000 = 4 *
                                       width * height; the dead editor C30D6 makes it 4 more) */
    unsigned int width;             /* +0x04: cells per row (1000) = xn_world_width */
    unsigned int height;            /* +0x08: rows of cells (500) = xn_world_height */
#if !defined(DAGGER_PORT)
    unsigned int *offsets;          /* +0x0C: in memory: the window of the offset table
                                       (xn_world_offsets); 0 in the file (DFU NullValue1).
                                       XN_WORLD_OFFSETS (xworld.h) */
#else
    unsigned int offsets_slot;      /*       natively the file's 4 bytes, as read: the window's
                                       pointer is a global of its own (XN_WORLD_OFFSETS) */
#endif
    unsigned int bands_offset;      /* +0x10: file offset of the band table (0x90 +
                                       offsets_size = 0x1E8510) = xn_world_bands_offset */
    unsigned int unknown_14;        /* +0x14: 1 (DFU Unknown1); not read */
    unsigned int unknown_18;        /* +0x18: 22 = the cell header size (DFU Unknown2); not
                                       read */
    unsigned int height_map_offset; /* +0x1C: file offset of the 1000x500 byte map (0x1E8910,
                                       DFU HeightMapOffset); XnGine never reads the map */
    unsigned int pad_20[28];        /* +0x20: zero (DFU NullValue2) */
};                                  /* +0x90 */
RECORD_SIZE(xn_wld_header, 144);

/* The first 22 bytes of a cell record: xn_world_read_cell C3301 reads them into
   xn_world_cell_header 0xC2879 (one instance: the cell read last); the elevations go to
   big_buffer. */
struct xn_wld_cell_header {
    unsigned int seed;              /* +0x00: the generator's seed (copied to xn_rand_seed
                                       D_00153800 by C3570: the cell is the same each visit) */
    unsigned short nature_archive;  /* +0x04: 0 in WOODS.WLD; world_render stores
                                       nature_texture_archive in the copy each frame */
    unsigned short ground_archive;  /* +0x06: TEXTURE archive of the ground (2, 102, 302,
                                       402: DFU FileIndex); world_render overwrites the copy */
    unsigned char climate;          /* +0x08: index into the band table (0..2 in WOODS.WLD) */
    unsigned char noise;            /* +0x09: bits 5-7 the midpoint noise amplitude (3..6
                                       seen); bits 0-4 != 0: a full 4x128x128 layer block
                                       follows instead of the 5x5 grid (no record has it) */
    unsigned short path_starts[6];  /* +0x0A: 0-terminated packed start points, read only by
                                       the dead path pass C3DA9 (which first overwrites [0]);
                                       zero in WOODS.WLD (DFU NullValue2[3]) */
};                                  /* +0x16 */
RECORD_SIZE(xn_wld_cell_header, 22);

/* A WOODS.WLD cell record (47 bytes: the dead editor's xn_world_write_cell C33F4 steps the
   offsets by 2Fh; the file's offsets differ by 47 throughout). File only: the engine reads
   the header to 0xC2879 and the grid (25 bytes) to big_buffer. */
struct xn_wld_cell {
    struct xn_wld_cell_header header;   /* +0x00 */
    unsigned char elevation[5][5];  /* +0x16: control heights, row 0 north; xn_world_unpack_cell
                                       C343E adds 80h (sea level) and plants them every 32
                                       squares of the 129x129 grid (big_buffer+0x100, stride
                                       256); 0..109 seen */
};                                  /* +0x2F */
RECORD_SIZE(xn_wld_cell, 47);

/* One climate's height bands: the band table (DFU DataSection1) is 256 of them, 0x400 bytes,
   read by C31AE into xn_world_height_bands 0xC23E9; WOODS.WLD fills climates 0-2:
   (3,15,75,110), (3,20,90,110), (3,10,20,60). */
struct xn_wld_bands {
    unsigned char threshold[4];     /* +0x00: tile class of a height h (0..127) = the first i
                                       with h <= threshold[i], else 3, dithered down near the
                                       edge (C3B87); threshold[0] is the water level (C3C8E);
                                       nature flats only between [0] and [3] (C3ACB/C3AD4) */
};                                  /* +0x04 */
RECORD_SIZE(xn_wld_bands, 4);

/* The nature flat odds of the current region: one at xn_world_nature_flat_odds 0xC2BB8,
   copied from region_flats[climate].f2 by climate_set_textures; read by
   xn_world_place_nature_flats C3A60. */
struct xn_world_nature_odds {
    unsigned char density;          /* +0x00: percent of squares that get a flat (the game
                                       caps it at 50) */
    unsigned char odds[4][32];      /* +0x01: [height >> 5][n-1]: cumulative percent for flat
                                       n = 1..32, ascending; FFh = unused */
};                                  /* +0x81 */
RECORD_SIZE(xn_world_nature_odds, 129);

/* ---- the terrain (xn_13E600) ------------------------------------------------------------- */

/* One entry of xn_terrain_vert_x 0x138578 or xn_terrain_vert_y 0x13B578 (1024 each, the
   32x32 grid): xn_terrain_transform_grid 13E8C8 indexes them with the same 12-byte vertex
   offset as xn_vert_cam, so only the first dword of each entry is used. */
struct xn_terrain_vert_coord {
    int value;                      /* +0x00: the vertex's camera-space x (or y) before the
                                       view scale (13E9EB/13E9F1); with xn_vert_cam.z the face
                                       planes 13EB21/13EC22 build the normal from it */
    char pad_04[8];                 /* +0x04 */
};                                  /* +0x0C */
RECORD_SIZE(xn_terrain_vert_coord, 12);

/* ---- flats (billboards, xn_154D00) ------------------------------------------------------- */

/* A flat (billboard): a 0x64-byte record taken from the render polygon pool
   (xn_render_poly_next D_000CEA64) by xn_flat_add 154D00/154D20 or xn_flat_add_view 154DA5,
   up to 512 a frame, listed in xn_flat_sort_list; drawn after the world spans by
   xn_render_draw_flats 12A814 -> xn_flat_draw 154E20 -> 158360 -> xn_flat_raster 1552A0 ->
   xn_flat_span_clip 155200 -> xn_flat_span_emit 15526C -> [+0x3C]. Shares the pool with the
   polygon record (struct xn_poly) but has its own layout; the game keeps its address as the
   object's draw handle (pick_sprite_cb). */
struct xn_flat {
    union {
        unsigned int image;         /* +0x00: archive << 7 | record (154D72, 154DC5; 154E20) */
        void *pad_00;               /*       (the slot is as wide as the polygon's face pointer:
                                       natively too the pick reads object where a polygon has
                                       its handle) */
    };
    iptr object;                    /* +0x04: always 0 (154D83, 154DDE): xn_pick_hit.model 0 =
                                       a flat (pointer-wide: the pick reads the whole slot) */
    int du_dx;                      /* +0x08: u step per pixel times 1/z (155116; negated when
                                       mirrored 1551D4); spans 157654 */
    int dv_dx;                      /* +0x0C: v step per pixel times 1/z (155156); spans
                                       15765D */
    unsigned char *shade_row;       /* +0x10: the 256-byte light (or fog) row the shaded spans
                                       map texels through (1556A9, 15571A; 157855, 157B7B) */
    unsigned int flags;             /* +0x14: bits 0-4 the quad kind (xn_flat_quad_table: 0-1
                                       centred, 4-7 standing on its base, others none); 20h
                                       mirrored (1551CB); 4 from the terrain and creatures */
    int u_offset;                   /* +0x18: u at the view origin (15515C, mirrored 1551E0..E6);
                                       spans subtract it (15763E) */
    int v_offset;                   /* +0x1C: v at the view origin (15518D); spans 15764E */
    char pad_20[4];                 /* +0x20 */
    int u_dx;                       /* +0x24: u/z gradient per screen x (du_dx >> 3, 15511C) */
    int u_dy;                       /* +0x28: per screen y (155126; patched into the row loop
                                       155195) */
    int u_c;                        /* +0x2C: constant term, moved to the top row (1551A4) and
                                       stepped per row by the patched 15534D */
    int v_dx;                       /* +0x30: v/z gradient per screen x (15515F) */
    int v_dy;                       /* +0x34: per screen y (155169) */
    int v_c;                        /* +0x38: constant term (155173, 1551BF; per row 155357) */
    void (*span)(struct xn_flat *, unsigned int, int, int, unsigned char *);
                                    /* +0x3C: the span routine (xspan.h xn_flat_span_fn) 15526C
                                       calls: first xn_flat_span_light_setup 155610 (154ECD),
                                       which installs one of the four flat spans */
    unsigned char *texels;          /* +0x40: the frame's pixels, 256 bytes a row (154E5B) */
    unsigned short scale;           /* +0x44: size, 100h = 1.0; xn_flat_draw adds the image's
                                       x_scale (texture header +0x18) (154E4F) */
    unsigned char light;            /* +0x46: the shade row the flat starts from (155625:
                                       >= the last row = unshaded); 154D9E stores the 7th
                                       argument's high word here */
    unsigned char unknown_47;       /* +0x47: the argument's top byte; ignored */
    char pad_48[4];                 /* +0x48 */
    union {
        int frame;                  /* +0x4C: at add: the frame for xn_tex_cache_lookup
                                       (-1 = by the animation clock) (154D74; 154E2A) */
        unsigned char *table;       /* +0x4C: from xn_flat_draw on: the translucency table of
                                       the archive (D_00136921[record word +0x0A], 0 = none;
                                       154E43); non-zero selects 157E20 (155610); then the fog
                                       row when lit and fogged (1556D7, read 157B84) */
    };
    int view_x;                     /* +0x50: camera-space position, 24.8 (154D7A, 154DD5) */
    int view_y;                     /* +0x54 */
    int view_z;                     /* +0x58: depth; the sort key is -view_z */
    char pad_5c[8];                 /* +0x5C */
};                                  /* +0x64 */
RECORD_SIZE_P(xn_flat, 100);
/* natively too a flat is a record of the polygon pool, read by the pick as one */
XN_NATIVE_OFFSET(xn_flat, object, 8);
typedef char xn_flat_in_poly_check[(sizeof(struct xn_flat) <= sizeof(struct xn_poly)) ? 1 : -1];

/* ---- sky and weather data (xn_C5400, used by xn_C7F00) ----------------------------------- */

/* A star: xn_sky_stars 0xC547A, 768 of them, placed on the upper hemisphere by
   xn_sky_init_stars C816E (sky_init); the code that would rotate and draw them (C8292,
   C81F2) is dead. Colours in xn_sky_star_colours 0xC787A (768 bytes). */
struct xn_sky_star {
    int x;                          /* +0x00: direction, length 2^20 (C81DC) */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
};                                  /* +0x0C */
RECORD_SIZE(xn_sky_star, 12);

/* A snow flake: xn_snow_flakes 0xC7B8B, 100 of them (C9BF7 initialises, C9A89 moves and
   draws them in colour 70h: flakes 0-19 2x2 at full speed, 20-49 2x2 at 3/4 speed, 50-99
   single pixels at half speed). */
struct xn_snow_flake {
    unsigned short x;               /* +0x00: screen x << 6 (C9C51; C9AA4 adds the turn shift) */
    unsigned short y;               /* +0x02: screen y << 6 (C9C2A) */
    short drift;                    /* +0x04: sideways direction +1/-1 (C9C64, C9B51) */
};                                  /* +0x06 */
RECORD_SIZE(xn_snow_flake, 6);

#pragma pack()

/* ==== Camera and view, input, graphics, fonts, VID, big_buffer, system ==================== */

#pragma pack(1)

struct xn_poly;
struct xn_span;
struct xn_light_ref;
struct xn_model_matrix_slot;

/* ---- the camera and view constants --------------------------------------------------------- */

/* The view block (data module xn_CEA00, 0xCEA20..0xCEAA8): the projection constants and the
 * frame pools' cursors. names.csv names every used field as its own global (xn_cam_*,
 * xn_render_*); this overlay gives their types and fixed point. Camera space: world units
 * << 8, rotated by xn_cam_view_matrix, so that the frustum's sides are x = +-z, y = +-z.
 * Writers: xn_cam_set_view_window 12A2D0 (160, 77, 160, 77 from init_video),
 * xn_cam_set_focal 12A274 (200, 180), xn_cam_update_derived 12A3AC, init_video (near, far),
 * xn_render_set_mode 12A254, xn_render_begin_frame 12A4F0 and the pool users. */
struct xn_view {
    int near_z;                     /* +0x00: 0xCEA20 xn_cam_near_z: 2560 (camera units: 10
                                       world units << 8); outcode 10h below it */
    int far_z;                      /* +0x04: 0xCEA24 xn_cam_far_z: 393216 (1536 << 8) */
    int half_width;                 /* +0x08: 0xCEA28 xn_cam_half_width: pixels (160) */
    int half_height;                /* +0x0C: 0xCEA2C xn_cam_half_height (77) */
    int centre_x;                   /* +0x10: 0xCEA30 xn_cam_centre_x: screen x of the axis */
    int centre_y;                   /* +0x14: 0xCEA34 xn_cam_centre_y */
    int focal_x;                    /* +0x18: 0xCEA38 xn_cam_focal_x: pixels (200) */
    int inv_focal_x;                /* +0x1C: 0xCEA3C xn_cam_inv_focal_x: 2^38 / focal_x */
    int focal_y;                    /* +0x20: 0xCEA40 xn_cam_focal_y (180) */
    int inv_focal_y;                /* +0x24: 0xCEA44 xn_cam_inv_focal_y: 2^38 / focal_y */
    int render_mode;                /* +0x28: 0xCEA48 xn_render_mode: 0 outline, 4 solid,
                                       8 textured (byte offset into xn_render_mode_tables) */
    int poly_count;                 /* +0x2C: 0xCEA4C xn_render_poly_count: faces submitted
                                       this frame (models add their face count) */
    int unknown_30;                 /* +0x30: 0xCEA50: no reader or writer */
    int unknown_34;                 /* +0x34: 0xCEA54: zeroed by xn_render_begin_frame only */
    void (*span_hook)(void);        /* +0x38: 0xCEA58 xn_render_span_hook: called after every
                                       span (xn_shade_fog_span 150040, or the ret 14D300) */
    int frame_flags;                /* +0x3C: 0xCEA5C xn_render_frame_flags: xn_render_frame's
                                       argument; 2 = no background fill (outdoors) */
    struct xn_span *span_next;      /* +0x40: 0xCEA60 xn_render_span_next */
    struct xn_poly *poly_next;      /* +0x44: 0xCEA64 xn_render_poly_next */
    struct xn_light_ref *light_list_next; /* +0x48: 0xCEA68 xn_render_light_list_next */
    int row_y;                      /* +0x4C: 0xCEA6C xn_render_row_y: the row being drawn,
                                       relative to centre_y */
    struct xn_model_matrix_slot *matrix_next; /* +0x50: 0xCEA70 xn_render_matrix_next */
    int unknown_54;                 /* +0x54: 0xCEA74: no reader or writer */
    int scale_x;                    /* +0x58: 0xCEA78 xn_cam_scale_x: focal_x * 2^14 /
                                       half_width (2.14; 1.25 = 20480) */
    int inv_scale_x;                /* +0x5C: 0xCEA7C xn_cam_inv_scale_x: 2^45 / scale_x */
    int scale_y;                    /* +0x60: 0xCEA80 xn_cam_scale_y */
    int inv_scale_y;                /* +0x64: 0xCEA84 xn_cam_inv_scale_y */
    int hfov_cos;                   /* +0x68: 0xCEA88 xn_cam_hfov_cos: (focal_x, half_width)
                                       normalised, 16.16 (51200) */
    int hfov_sin;                   /* +0x6C: 0xCEA8C xn_cam_hfov_sin (40960) */
    int vfov_cos;                   /* +0x70: 0xCEA90 xn_cam_vfov_cos */
    int vfov_sin;                   /* +0x74: 0xCEA94 xn_cam_vfov_sin */
    unsigned char pick_skip_flats;  /* +0x78: 0xCEA98 xn_pick_skip_flats: xn_flat_pick
                                       returns nothing when set */
    int flat_scale_x;               /* +0x79: 0xCEA99 xn_cam_flat_scale_x: (half_width << 32)
                                       / focal_x^2 / 2 (unaligned) */
    int flat_scale_y;               /* +0x7D: 0xCEA9D xn_cam_flat_scale_y */
    int unknown_81;                 /* +0x81: 0xCEAA1: no reader or writer */
    int *recip_table;               /* +0x85: 0xCEAA5 xn_render_recip_table: 65537 ints,
                                       [n] = 2^24 / n (allocated by xn_render_init; its address
                                       is also patched into 25 span routines) */
};                                  /* +0x89 */
RECORD_SIZE_P(xn_view, 137);

/* ---- the mouse -------------------------------------------------------------------------- */

/* The mouse state (data module xn_12AC00: 0x12AC00..0x12AF3B; the names in names.csv are the
 * game's: mouse_buttons, mouse_x... and xn_mouse_*). Writers: xn_mouse_poll 12B196 (int 33h
 * 3, double clicks), xn_mouse_poll_clamped 12B136, the 16x16 software cursor 12B2D3..12B45B. */
struct xn_mouse_press {
    unsigned int tick;              /* +0x00: BIOS tick of the last press */
    short x;                        /* +0x04 */
    short y;                        /* +0x06 */
};                                  /* +0x08 */
RECORD_SIZE(xn_mouse_press, 8);

struct xn_mouse_state {
    unsigned char buttons;          /* +0x000: 0x12AC00 mouse_buttons: 1 left, 2 right */
    unsigned char buttons_prev;     /* +0x001: xn_mouse_buttons_prev */
    unsigned short double_click;    /* +0x002: mouse_double_click: 1 left, 2 right (a second
                                       press within 8 ticks and 4 pixels) */
    short x;                        /* +0x004: mouse_x */
    short y;                        /* +0x006: mouse_y */
    int cursor_x;                   /* +0x008: xn_mouse_cursor_x: the cursor's clipped left */
    int cursor_y;                   /* +0x00C: xn_mouse_cursor_y */
    unsigned char under[256];       /* +0x010: xn_mouse_cursor_under: the pixels under it */
    unsigned char clipped[256];     /* +0x110: xn_mouse_cursor_clipped: the visible part */
    unsigned char image[256];       /* +0x210: xn_mouse_cursor_image: 16x16, colour 0 clear */
    int pad_310;                    /* +0x310 */
    short cursor_w;                 /* +0x314: xn_mouse_cursor_w: visible width (<= 16) */
    short cursor_h;                 /* +0x316: xn_mouse_cursor_h */
    short hotspot_x;                /* +0x318: xn_mouse_hotspot_x */
    short hotspot_y;                /* +0x31A: xn_mouse_hotspot_y (12B326 reads hotspot_x for
                                       y: a bug; the game passes 0, 0) */
    int under_x;                    /* +0x31C: xn_mouse_under_x: where `under` was saved */
    int under_y;                    /* +0x320: xn_mouse_under_y */
    struct xn_mouse_press press[2]; /* +0x324: xn_mouse_left_press_tick.. (left, right) */
    short x_min;                    /* +0x334: mouse_x_min */
    short x_max;                    /* +0x336 */
    short y_min;                    /* +0x338 */
    short y_max;                    /* +0x33A */
};                                  /* +0x33C */
RECORD_SIZE(xn_mouse_state, 828);

/* ---- the keyboard ------------------------------------------------------------------------- */

/* The keyboard state (0x142300..0x142496; xn_kbd_install locks 0x397 bytes of it): the int 9
 * handler (xn_kbd_int9_handler 142840.., 15 run-time blocks) writes it, xn_kbd_read_key
 * 1427A8 and the game's keys.c read it */
struct xn_kbd_state {
    unsigned int old_int9_offset;   /* +0x000: xn_kbd_old_int9_offset */
    unsigned short old_int9_selector; /* +0x004 */
    unsigned char installed;        /* +0x006: xn_kbd_installed */
    unsigned char last_scancode;    /* +0x007: xn_kbd_last_scancode: the last make code,
                                       0 after any break and after a read */
    unsigned char key_down[128];    /* +0x008: key_down (game name): 1 while held */
    unsigned int shift_flags;       /* +0x088: xn_kbd_shift_flags: 1 LShift, 2 RShift, 4 Alt,
                                       8 Ctrl */
    char pad_8c[7];                 /* +0x08C */
    unsigned char *keymap;          /* +0x093: xn_kbd_keymap: -> ascii (set by install) */
    unsigned char ascii[2][128];    /* +0x097: xn_kbd_ascii_table: by scan code - 1,
                                       [1] while a Shift is down */
};                                  /* +0x197 */
RECORD_SIZE_P(xn_kbd_state, 407);

/* ---- the joystick ------------------------------------------------------------------------- */

/* The joystick state (0x152A00, 0x6E bytes locked by xn_joy_init). The int 1Ch handler
 * (xn_joy_timer_isr 152EA0) times the four axes of port 201h and reads the buttons;
 * xn_joy_poll 152D00 widens the ranges and gives joystick_x/y in -4095..4095. The 46 bytes
 * from +0x04 (center .. buttons) are what the options screen saves (options.c: mc_memcpy
 * of xn_joy_calibration, 46). Stick B's outputs are cleared but never computed. */
struct xn_joy_axes {
    int center_x;                   /* +0x00: the average of 4 BIOS ticks' raw counts
                                       (xn_joy_calibrate 152C40) */
    int center_y;                   /* +0x04 */
};                                  /* +0x08 */
RECORD_SIZE(xn_joy_axes, 8);

struct xn_joy_state {
    unsigned char axis_mask;        /* +0x00: xn_joy_axis_mask: port 201h bits seen */
    unsigned char installed;        /* +0x01: xn_joy_installed */
    unsigned char status;           /* +0x02: joystick_status: 2 off */
    unsigned char pad_03;           /* +0x03 */
    struct xn_joy_axes center;      /* +0x04: xn_joy_calibration */
    int dead_zone;                  /* +0x0C: xn_joy_dead_zone: 10/20/40 (options) */
    int min_x;                      /* +0x10: xn_joy_min_x (32000 after a reset) */
    int max_x;                      /* +0x14 */
    int min_y;                      /* +0x18 */
    int max_y;                      /* +0x1C */
    int x;                          /* +0x20: joystick_x: -4095..4095, 0 in the dead zone */
    int y;                          /* +0x24: joystick_y */
    int raw_x;                      /* +0x28: xn_joy_raw_x: port loops until the bit drops */
    int raw_y;                      /* +0x2C */
    unsigned char button1;          /* +0x30: joystick_button1 */
    unsigned char button2;          /* +0x31: joystick_button2 */
    short pad_32;                   /* +0x32 */
    struct xn_joy_axes center_b;    /* +0x34: xn_joy2_center_x: stick B */
    int pad_3c;                     /* +0x3C */
    int min_x_b;                    /* +0x40: xn_joy2_min_x */
    int max_x_b;                    /* +0x44 */
    int min_y_b;                    /* +0x48 */
    int max_y_b;                    /* +0x4C */
    int x_b;                        /* +0x50: xn_joy2_x: cleared, never computed */
    int y_b;                        /* +0x54 */
    int raw_x_b;                    /* +0x58: xn_joy2_raw_x */
    int raw_y_b;                    /* +0x5C */
    unsigned char button3;          /* +0x60: xn_joy_button3 */
    unsigned char button4;          /* +0x61 */
    unsigned short old_int1c_sel;   /* +0x62: xn_joy_old_int1c_sel */
    unsigned int old_int1c_off;     /* +0x64: xn_joy_old_int1c_off */
    char pad_68[6];                 /* +0x68 */
};                                  /* +0x6E */
RECORD_SIZE(xn_joy_state, 110);

/* ---- graphics -------------------------------------------------------------------------------- */

/* The graphics state (0x14291B..0x143570; separate globals in names.csv). The clip rectangle
 * is exclusive on the right and bottom (xn_gfx_set_clip 1438FC(0, 0, width, height)); every
 * 2D blit and the 3D view clip to it. */
struct xn_gfx_state {
    int mode;                       /* +0x000: 0x14291B xn_gfx_mode: 13h or a VESA mode */
    int saved_mode;                 /* +0x004: xn_gfx_saved_mode: the BIOS mode to restore */
    int driver;                     /* +0x008: xn_gfx_driver: byte offset into the driver
                                       tables (always 0) */
    unsigned char pad_00c;          /* +0x00C */
    int pen_x;                      /* +0x00D: 0x142928 pen_x: xn_draw_line_to's start */
    int pen_y;                      /* +0x011: pen_y */
    int width;                      /* +0x015: 0x142930 xn_gfx_width (320) */
    int height;                     /* +0x019: xn_gfx_height (200) */
    int screen_size;                /* +0x01D: xn_gfx_screen_size (64000) */
    int page_count;                 /* +0x021: xn_gfx_page_count */
    int clip_left;                  /* +0x025: 0x142940 xn_gfx_clip_left */
    int clip_top;                   /* +0x029: xn_gfx_clip_top */
    int clip_right;                 /* +0x02D: xn_gfx_clip_right (exclusive) */
    int clip_bottom;                /* +0x031: xn_gfx_clip_bottom (exclusive) */
    int row_offset[768];            /* +0x035: 0x142950 xn_gfx_row_offset: y * width */
    unsigned char *screen_buffer;   /* +0xC35: 0x143550 screen_buffer: the back buffer (or
                                       the VESA linear frame buffer page) */
    unsigned char *buffer_base;     /* +0xC39: xn_gfx_buffer_base */
    void *buffer_alloc;             /* +0xC3D: xn_gfx_buffer_alloc: as allocated (32-aligned
                                       into screen_buffer); 0 for a linear frame buffer */
    unsigned char present_mode;     /* +0xC41: xn_gfx_present_mode: 1 copy, 3 page flip */
    int fill_scratch;               /* +0xC42: xn_gfx_fill_scratch */
    void (*copy_clear_fn)(void);    /* +0xC46: xn_gfx_copy_clear_fn (143B80) */
    void (*drv_shutdown[1])(void);  /* +0xC4A: xn_gfx_drv_shutdown (144C0D, a ret) */
    void (*drv_present[1])(void);   /* +0xC4E: xn_gfx_drv_present (14396C) */
    void (*drv_clear[1])(void);     /* +0xC52: xn_gfx_drv_clear (143924) */
};                                  /* +0xC56 */
RECORD_SIZE_P(xn_gfx_state, 3158);

/* The DPMI real-mode call structure (int 31h AX=0300h, 50 bytes): xn_gfx_vesa_rm_regs
 * (0x15FA08) and xn_helmet_c_rm_regs (0x161300) */
struct xn_rm_regs {
    unsigned int edi;               /* +0x00 */
    unsigned int esi;               /* +0x04 */
    unsigned int ebp;               /* +0x08 */
    unsigned int reserved;          /* +0x0C */
    unsigned int ebx;               /* +0x10 */
    unsigned int edx;               /* +0x14 */
    unsigned int ecx;               /* +0x18 */
    unsigned int eax;               /* +0x1C */
    unsigned short flags;           /* +0x20 */
    unsigned short es;              /* +0x22: the DOS buffer's segment (15FC1A) */
    unsigned short ds;              /* +0x24 */
    unsigned short fs;              /* +0x26 */
    unsigned short gs;              /* +0x28 */
    unsigned short ip;              /* +0x2A */
    unsigned short cs;              /* +0x2C */
    unsigned short sp;              /* +0x2E */
    unsigned short ss;              /* +0x30 */
};                                  /* +0x32 */
RECORD_SIZE(xn_rm_regs, 50);

/* VBE ModeInfoBlock (256 bytes; the VESA standard layout): xn_gfx_vesa_mode_info (0x15FA6D),
 * copied from the DOS buffer by xn_gfx_vesa_set_mode 15FCA7, which reads the resolution,
 * the image pages and the linear frame buffer */
struct xn_vesa_mode_info {
    unsigned short mode_attributes; /* +0x00 */
    unsigned char win_a_attributes; /* +0x02 */
    unsigned char win_b_attributes; /* +0x03 */
    unsigned short win_granularity; /* +0x04 */
    unsigned short win_size;        /* +0x06 */
    unsigned short win_a_segment;   /* +0x08 */
    unsigned short win_b_segment;   /* +0x0A */
    unsigned int win_func;          /* +0x0C */
    unsigned short bytes_per_line;  /* +0x10 */
    unsigned short x_resolution;    /* +0x12: read 15FD16 */
    unsigned short y_resolution;    /* +0x14: read 15FD22 */
    unsigned char x_char_size;      /* +0x16 */
    unsigned char y_char_size;      /* +0x17 */
    unsigned char planes;           /* +0x18 */
    unsigned char bits_per_pixel;   /* +0x19 */
    unsigned char banks;            /* +0x1A */
    unsigned char memory_model;     /* +0x1B */
    unsigned char bank_size;        /* +0x1C */
    unsigned char image_pages;      /* +0x1D: incremented to a page count (15FD5C), read by
                                       xn_gfx_set_mode */
    unsigned char reserved_1e;      /* +0x1E */
    unsigned char colour_masks[9];  /* +0x1F: red/green/blue/reserved mask size and position,
                                       direct colour mode info */
    unsigned int phys_base;         /* +0x28: the linear frame buffer (VBE 2.0); 0 none */
    char pad_2c[212];               /* +0x2C */
};                                  /* +0x100 */
RECORD_SIZE(xn_vesa_mode_info, 256);

/* ---- fonts ----------------------------------------------------------------------------------- */

/* A FONT000n.FNT file as xn_font_load (12DB28) loads it whole into a slot of xn_font_table
 * (0x12DA54, 8 slots; init_game_data loads FONT0000-0003 into 1-4); xn_font_select (12DB50)
 * copies its header into font_space_width / font_height and its address into font_glyphs.
 * Glyph rows are 16-bit words, most significant bit leftmost, one per row */
struct xn_fnt_glyph {
    unsigned short offset;          /* +0x00: the glyph's rows, from the start of the file */
    unsigned short width;           /* +0x02: pixels */
};                                  /* +0x04 */
RECORD_SIZE(xn_fnt_glyph, 4);

struct xn_fnt_file {
    unsigned short space_width;     /* +0x000: the width of ' ' (and of characters below '!') */
    unsigned short height;          /* +0x002: rows per glyph */
    struct xn_fnt_glyph glyphs[240]; /* +0x004: '!' (33) onwards; the engine indexes up to
                                       255 (223 entries); FONT0000.FNT's glyph data starts at
                                       964 = 4 + 240 * 4, 32 bytes per glyph */
};                                  /* +0x3C4 */
RECORD_SIZE(xn_fnt_file, 964);

/* The text state (0x12DA38..0x12DA77; separate globals) */
struct xn_font_state {
    int text_x0;                    /* +0x00: 0x12DA38 xn_font_text_x0: CR returns here */
    int text_y0;                    /* +0x04: 0x12DA3C */
    int space_width;                /* +0x08: font_space_width */
    int height;                     /* +0x0C: font_height */
    int char_spacing;               /* +0x10: font_char_spacing (1) */
    int line_gap;                   /* +0x14: xn_font_line_gap (1) */
    int current;                    /* +0x18: xn_font_current: the slot selected */
    struct xn_fnt_file *table[8];   /* +0x1C: xn_font_table */
    struct xn_fnt_file *glyphs;     /* +0x3C: font_glyphs: the selected font */
};                                  /* +0x40 */
RECORD_SIZE_P(xn_font_state, 64);

/* ---- the VID player ---------------------------------------------------------------------------- */

/* A VID file's 15-byte header (xn_vid_header 0xC1000, read by xn_vid_open C1654) */
struct xn_vid_header {
    char magic[4];                  /* +0x00: "VID\0" */
    unsigned char version;          /* +0x04: 2 (a row-end byte follows rows when >= 2) */
    unsigned short frames;          /* +0x05: xn_vid_header_frames */
    unsigned short width;           /* +0x07: 320 */
    unsigned short height;          /* +0x09: 200 */
    unsigned short frame_delay;     /* +0x0B: added to every frame's own delay (60 Hz ticks) */
    unsigned short flags;           /* +0x0D: 0Eh in every ANIM*.VID; bit 0: leave the file
                                       open at the end (xn_vid_finish) */
};                                  /* +0x0F */
RECORD_SIZE(xn_vid_header, 15);

/* The player's state (0xC10FF..0xC14F0; xn_vid_play C1500 locks 0xC1000..0xC14F0 since the
 * 60 Hz timer callback xn_vid_timer_cb and the sample-done callback run under interrupts).
 * The SOS sample descriptor before it, xn_vid_sample (0xC100F, 240 bytes), is
 * include/structs.h struct sos_sample (+0x5C there: the done callback, xn_vid_audio_done_cb). */
struct xn_vid_player {
    unsigned char audio_state;      /* +0x000: 0xC10FF xn_vid_audio_state: 0 none, 1 an audio
                                       header read, 2 waiting for its delay, 3 streaming */
    unsigned char audio_buffer;     /* +0x001: which buffer the next 7Dh chunk fills (0 a, 1 b) */
    int sample_handle;              /* +0x002: xn_vid_sample_handle (SOS start-sample) */
    unsigned short sample_rate;     /* +0x006: xn_vid_sample_rate: Hz = 1000000 / (256 - b) */
    unsigned short audio_delay;     /* +0x008: the 7Ch chunk's delay (60 Hz ticks) */
    int audio_left;                 /* +0x00A: bytes of the 7Dh chunk still to copy */
    unsigned char rate_byte;        /* +0x00E: the 7Ch chunk's rate byte */
    int audio_size;                 /* +0x00F: the 7Dh chunk's size */
    unsigned char *audio_buf_a;     /* +0x013: xn_vid_audio_buf_a (0x4000, locked) */
    int audio_len_a;                /* +0x017 */
    unsigned char *audio_buf_b;     /* +0x01B: xn_vid_audio_buf_b */
    int audio_len_b;                /* +0x01F */
    unsigned char frame_waits;      /* +0x023: a frame was decoded while streaming: the next
                                       update stops the sample first */
    unsigned char sample_done;      /* +0x024: set by the done callback */
    unsigned char sample_restart;   /* +0x025: start the next buffer at the next update */
    unsigned char palette[768];     /* +0x026: xn_vid_palette: the last 2-chunk's (set at the
                                       end) */
    int dest_offset;                /* +0x326: 0xC1425: y * width + x in screen_buffer */
    unsigned short frames_left;     /* +0x32A: xn_vid_frames_left */
    unsigned short file;            /* +0x32C: xn_vid_file: the DOS handle */
    int file_pos;                   /* +0x32E: bytes consumed before the buffer */
    unsigned char *buf;             /* +0x332: xn_vid_buf: big_buffer */
    unsigned char *read_ptr;        /* +0x336: the next chunk */
    unsigned char *buf_end;         /* +0x33A: buf + 64000 */
    unsigned char *refill_mark;     /* +0x33E: buf_end - 0x81: refill past it */
    unsigned short frames_done;     /* +0x342 */
    int x;                          /* +0x344: where the movie is drawn */
    int y;                          /* +0x348 */
    char *path_arg;                 /* +0x34C: the path xn_vid_open opens */
    unsigned short width;           /* +0x350 */
    unsigned short height;          /* +0x352 */
    unsigned char done;             /* +0x354: xn_vid_done */
    int dest_skip;                  /* +0x355: screen width - movie width */
    int row_width;                  /* +0x359: the movie width */
    unsigned short rows_left;       /* +0x35D: rows left in the frame being decoded */
    unsigned short rows;            /* +0x35F: the movie height */
    unsigned short flags;           /* +0x361: the header's flags */
    int timer;                      /* +0x363: xn_vid_timer: the sound_timer_add handle, -1
                                       when it failed */
    short frame_ticks;              /* +0x367: xn_vid_frame_ticks: counted down at 60 Hz */
    unsigned short audio_ticks;     /* +0x369: the audio delay being counted down */
    unsigned short audio_waiting;   /* +0x36B: 1 while audio_ticks counts */
    char path[129];                 /* +0x36D: xn_vid_path */
    int skippable;                  /* +0x3EE: xn_vid_skippable: a key or button stops it */
};                                  /* +0x3F2 */
RECORD_SIZE_P(xn_vid_player, 1010);

#pragma pack()

#endif
