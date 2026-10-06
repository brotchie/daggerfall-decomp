/* structs.h: the game's shared structures that are not records (records.h has those): file
 * headers, the engine's buffers, the UI tables, the sound channels... records.h includes this
 * file at its end, so every file that includes records.h sees these too; a file that needs
 * only these includes structs.h.
 *
 * The conventions are records.h's: packed, offsets in the comments from the start of the
 * struct (hex), fields nobody has named padNN by their offset, each size checked at compile
 * time (RECORD_SIZE, or RECORD_OFFSET for a struct that ends in its variable-length data), and
 * tools/offset_casts.py checks every `+0xNN` comment against the layout. The lifter's bit views
 * (struct bf8_B_N) are in bitfield.h; the runtime's own types come from its headers (struct
 * find_t from <dos.h>, union REGS and struct SREGS from <i86.h>). */
#ifndef STRUCTS_H
#define STRUCTS_H

#include "ptrint.h"

#ifndef RECORD_SIZE
#define RECORD_SIZE(tag, n) typedef char tag##_size_check[(sizeof(struct tag) == (n)) ? 1 : -1]
/* RECORD_OFFSET checks a member's offset (natively with offsetof: the Watcom form casts a
   pointer to an int). The _P forms are for a struct holding pointers: exact where pointers are
   4 bytes (Watcom); in the native build (docs/port.md) pointers are 8 bytes and the struct may
   only grow */
#if defined(DAGGER_PORT)
#define RECORD_OFFSET(tag, m, n) \
    typedef char tag##_##m##_offset_check[(__builtin_offsetof(struct tag, m) == (n)) ? 1 : -1]
#define RECORD_SIZE_P(tag, n) typedef char tag##_size_check[(sizeof(struct tag) >= (n)) ? 1 : -1]
#define RECORD_OFFSET_P(tag, m, n) \
    typedef char tag##_##m##_offset_check[(__builtin_offsetof(struct tag, m) >= (n)) ? 1 : -1]
#else
#define RECORD_OFFSET(tag, m, n) \
    typedef char tag##_##m##_offset_check[((unsigned)&((struct tag *)0)->m == (n)) ? 1 : -1]
#define RECORD_SIZE_P(tag, n) RECORD_SIZE(tag, n)
#define RECORD_OFFSET_P(tag, m, n) RECORD_OFFSET(tag, m, n)
#endif
#endif

#pragma pack(1)

struct record;

/* the BIOS tick count (18.2 a second) in the BIOS data area, 0040:006C */
#define BIOS_TICKS (*(int *)0x46C)

/* ---- the engine -------------------------------------------------------------------------- */

/* a point or a vector in world units (1/40 m): a record's x, y, z, the collision probe's
 * position (D_00196D54) */
struct vec3 {
    int x;                          /* +0x00 */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
};                                  /* +0x0C */
RECORD_SIZE(vec3, 12);

/* a step on the ground by heading quadrant (D_0017B657[4], indexed by yaw >> 9: the pedestrians'
 * look-ahead, pedestrian_can_stand_at) */
struct xz_step {
    short dx;                       /* +0x00 */
    short dz;                       /* +0x02 */
};                                  /* +0x04 */
RECORD_SIZE(xz_step, 4);

/* a collision probe: spheres around a position (the movers' shapes: D_00187B44 the creatures',
 * D_00187B6E, D_00187BB8, D_00187C12, D_00187C3C the player's, D_00179F48); the collision code
 * keeps the current one in D_00196D4C and writes the tested position into it */
struct probe_sphere {
    int x;                          /* +0x00: from the position */
    int y;                          /* +0x04 */
    int z;                          /* +0x08 */
    int radius;                     /* +0x0C */
};                                  /* +0x10 */
RECORD_SIZE(probe_sphere, 16);

struct collide_probe {
    struct vec3 position;           /* +0x00 */
    char pad0C[12];                 /* +0x0C */
    short sphere_count;             /* +0x18 */
    struct probe_sphere spheres[1]; /* +0x1A: sphere_count of them */
};

/* XnGine's collision result (xn_collide_segment_model, xn_collide_spheres_model...): the faces
 * hit; colstuff.c keeps the last in D_00196D48, and the planes the mover slides along in
 * D_00196D50 (collide_move_player, collide_move_object) */
struct collide_hit {
    int x;                          /* +0x00: the point hit */
    int y;                          /* +0x04: (the height a vertical segment found) */
    int z;                          /* +0x08 */
    int face;                       /* +0x0C: the face's offset in the model */
    int nx;                         /* +0x10: the face's normal */
    int ny;                         /* +0x14 */
    int nz;                         /* +0x18 */
    short pad1C;                    /* +0x1C */
};                                  /* +0x1E */
RECORD_SIZE(collide_hit, 30);

struct collide_hits {
    int count;                      /* +0x00 */
    struct collide_hit hits[1];     /* +0x04: count of them */
};

/* a 3D model (ARCH3D.BSA) as model_load keeps it: the file's header (DFU Arch3dFile
 * FileHeader, 64 bytes; the version is "v2.7"), then the data its offsets point at, from the
 * start of the header. A model instance or a block model holds a pointer to it (their `model`),
 * and the pick and collision code pass the address of that pointer (arch3d_plane_at) */
struct arch3d_header {
    char version[4];                /* +0x00 */
    int point_count;                /* +0x04 */
    int plane_count;                /* +0x08 */
    int radius;                     /* +0x0C */
    char null_value1[8];            /* +0x10 */
    int plane_data_offset;          /* +0x18 */
    int object_data_offset;         /* +0x1C */
    int object_data_count;          /* +0x20 */
    int unknown2;                   /* +0x24 */
    char null_value2[8];            /* +0x28 */
    int point_list_offset;          /* +0x30 */
    int normal_list_offset;         /* +0x34 */
    int unknown3;                   /* +0x38 */
    int plane_list_offset;          /* +0x3C: the planes, one after the other */
};                                  /* +0x40 */
RECORD_SIZE(arch3d_header, 64);

/* a point of a model's plane (DFU Arch3dFile PlanePoint) */
struct arch3d_plane_point {
    int point_offset;               /* +0x00 */
    short u;                        /* +0x04 */
    short v;                        /* +0x06 */
};                                  /* +0x08 */
RECORD_SIZE(arch3d_plane_point, 8);

/* a plane (face) of a model (DFU Arch3dFile PlaneHeader, then its points): the face the pick
 * or the collision code hit is click_face_texture */
struct arch3d_plane {
    unsigned char point_count;      /* +0x00: DFU PlanePointCount */
    unsigned char unknown1;         /* +0x01 */
    unsigned short texture;         /* +0x02: archive<<7 | record (arch3d_apply_climate_textures maps
                                       the archive by climate; click_world_face) */
    signed char floor_sound;        /* +0x04: DFU Unknown2's low byte: in a dungeon, the footstep
                                       sound (0-4) of the floor plane (footstep_sounds) */
    char pad05[3];                  /* +0x05 */
    struct arch3d_plane_point points[1]; /* +0x08: point_count of them */
};
RECORD_OFFSET(arch3d_plane, points, 8);

/* what xn_render_pick returns for the point under the cursor (engine_pick_object keeps it in
 * pick_hit): the record of a drawn polygon, or of a flat (whose address is then the flat's
 * draw handle, pick_sprite_cb); the first 8 bytes */
struct xn_pick_hit {
    struct arch3d_plane *plane;     /* +0x00: a polygon's plane (click_face_texture) */
    int model;                      /* +0x04: a polygon's: the address of its model pointer (pick_model_cb
                                       compares &instance.model); 0 a flat, 1 nothing to pick */
};                                  /* +0x08 */
RECORD_SIZE_P(xn_pick_hit, 8);

/* ---- files ------------------------------------------------------------------------------ */

/* a profile: an INI-style text file ([section] lines, then `item = value` lines) loaded whole
 * into memory by profile_open, edited in place, written back by profile_close when changed
 * (the HMI SOS settings, sos_read_settings) */
struct profile {
    unsigned char pad00;            /* +0x00 */
    unsigned char flags;            /* +0x01: 0x80 changed (profile_close writes the file back) */
    char pad02[2];                  /* +0x02 */
    char path[128];                 /* +0x04 */
    char *buffer;                   /* +0x84: the file, with 1024 bytes of room to grow */
    unsigned int length;            /* +0x88 */
    unsigned int capacity;          /* +0x8C */
    char *section;                  /* +0x90: the found section's first line, after its header */
    int section_offset;             /* +0x94: its offset in the buffer */
    char *section_header;           /* +0x98: its `[` */
    char *item;                     /* +0x9C: the found item's line */
    char *value;                    /* +0xA0: the found item's value */
    char *next_value;               /* +0xA4: the next number of a comma list (profile_get_number) */
    char *line;                     /* +0xA8: the raw-line cursor (profile_get_raw_line) */
    int result;                     /* +0xAC: sos_read_settings' last result */
};                                  /* +0xB0 */
RECORD_SIZE_P(profile, 176);

/* an IMG file, or one image of a CIF file (a CIF is several, each data_size bytes after the
 * end of its header): the 12-byte header, then the pixels (DFU ImgFile) */
struct image {
    unsigned short x;               /* +0x00: where it is drawn */
    unsigned short y;               /* +0x02 */
    unsigned short width;           /* +0x04 */
    unsigned short height;          /* +0x06 */
    unsigned short compression;     /* +0x08 */
    unsigned short data_size;       /* +0x0A: bytes of pixels */
    char pixels[1];                 /* +0x0C */
};
RECORD_OFFSET(image, pixels, 12);

/* a BSS file (the compass strips, cmpa00i0.bss to cmpa02i0.bss): a 10-byte header, then frames
 * of width x height pixels (hud_draw_compass) */
struct bss_image {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    short width;                    /* +0x04 */
    short height;                   /* +0x06 */
    short pad08;                    /* +0x08 */
    char pixels[1];                 /* +0x0A */
};
RECORD_OFFSET(bss_image, pixels, 10);

/* a CFA file's header (DFU CfaFile): the moons (moon00i0.cfa); sky_update keeps a moon's
 * screen position in the two unknown words */
struct cfa_header {
    unsigned short width;           /* +0x00 */
    unsigned short height;          /* +0x02 */
    unsigned short width_compressed; /* +0x04 */
    short x;                        /* +0x06: DFU Unknown1: sky_update's screen x */
    short y;                        /* +0x08: DFU Unknown2: the screen y */
    unsigned char bits_per_pixel;   /* +0x0A */
    unsigned char frame_count;      /* +0x0B */
    unsigned short header_size;     /* +0x0C */
};                                  /* +0x0E */
RECORD_SIZE(cfa_header, 14);

/* a TEXTURE.nnn record's image header (DFU TextureFile RecordHeader), as XnGine keeps it:
 * xn_tex_cache_lookup_image's result, and the decoded frame at +0x0C of a cache entry */
struct texture_header {
    short x;                        /* +0x00 */
    short y;                        /* +0x02 */
    unsigned short width;           /* +0x04 */
    unsigned short height;          /* +0x06 */
    unsigned short flags;           /* +0x08: DFU Compression; drawn with 0x8000 added */
    int size;                       /* +0x0A: DFU RecordSize; in the cache XnGine keeps a pointer
                                       here (xn_tex_cache_lookup hands it to 0x15C2DC) */
    int data_offset;                /* +0x0E: the pixels, from the start of the header */
    unsigned short pad12;           /* +0x12: DFU IsNormal */
    unsigned short frame_count;     /* +0x14: xn_tex_cache_lookup, flat_anim_finished */
    unsigned short frame_time;      /* +0x16: ticks per frame (xn_tex_cache_lookup divides the clock
                                       by it, flat_anim_step); creatures: monster_init and
                                       object_draw_cb copy it to monster_anim.frame_count */
    short x_scale;                  /* +0x18 */
    short y_scale;                  /* +0x1A */
};                                  /* +0x1C */
RECORD_SIZE(texture_header, 28);

/* a TEXTURE.nnn record's entry in XnGine's texture cache, as xn_tex_cache_lookup returns it
 * (the first 16 bytes) */
struct tex_cache_entry {
    char pad00[12];                 /* +0x00 */
    struct texture_header *image;   /* +0x0C: the decoded frame (its header, then the pixels) */
};                                  /* +0x10 */
RECORD_SIZE_P(tex_cache_entry, 16);

/* a BSA archive's directory entry: type-256 archives name their records, the others number them */
struct bsa_name_entry {
    char name[14];                  /* +0x00 */
    int size;                       /* +0x0E */
};                                  /* +0x12 */
RECORD_SIZE(bsa_name_entry, 18);

struct bsa_id_entry {
    int id;                         /* +0x00 */
    int size;                       /* +0x04 */
};                                  /* +0x08 */
RECORD_SIZE(bsa_id_entry, 8);

/* a run of a row of a PAK file (CLIMATE.PAK, POLITIC.PAK; DFU PakFile): `count` map cells of
 * `value`; the file starts with each row's offset (pak_lookup) */
struct pak_run {
    short count;                    /* +0x00 */
    unsigned char value;            /* +0x02 */
};                                  /* +0x03 */
RECORD_SIZE(pak_run, 3);

/* an entry of TEXT.RSC's index (text_rsc_load: a u16 index size, then these) */
struct text_rsc_entry {
    short id;                       /* +0x00 */
    int offset;                     /* +0x02: the text's file offset; the next entry's ends it */
};                                  /* +0x06 */
RECORD_SIZE(text_rsc_entry, 6);

/* an entry of the MAPDITEM record's dungeon table (location_load_dungeon_by_id) */
struct dungeon_entry {
    int offset;                     /* +0x00: from the end of the table */
    int id;                         /* +0x04: the location id */
};                                  /* +0x08 */
RECORD_SIZE(dungeon_entry, 8);

/* a .WAV file's header (the canonical 44 bytes; sos_load_sample) */
struct wav_header {
    char riff[4];                   /* +0x00: "RIFF" */
    int riff_size;                  /* +0x04 */
    char wave[4];                   /* +0x08: "WAVE" */
    char fmt[4];                    /* +0x0C: "fmt " */
    int fmt_size;                   /* +0x10 */
    short format;                   /* +0x14 */
    short channels;                 /* +0x16 */
    int rate;                       /* +0x18: samples per second */
    int byte_rate;                  /* +0x1C */
    short block_align;              /* +0x20 */
    short bits;                     /* +0x22: per sample */
    char data[4];                   /* +0x24: "data" */
    int data_size;                  /* +0x28 */
};                                  /* +0x2C */
RECORD_SIZE(wav_header, 44);

/* an FLC animation being played (flc_open fills it, flc_next_frame decodes a frame into its
 * buffers, flc_close frees them) */
struct flc_player {
    unsigned short flags;           /* +0x00: 1, 2, 64 own the chunk, palette and image buffers;
                                       4 ended; 8 the palette changed; 16 keep the palette;
                                       128 decode into the image instead of the screen */
    unsigned short handle;          /* +0x02 */
    short frame_count;              /* +0x04 */
    short frames_left;              /* +0x06 */
    unsigned short ticks_per_frame; /* +0x08 */
    int loop_offset;                /* +0x0A: the second frame's file offset */
    short x;                        /* +0x0E */
    short y;                        /* +0x10 */
    short width;                    /* +0x12 */
    short height;                   /* +0x14 */
    char *chunk;                    /* +0x16 */
    char *palette;                  /* +0x1A: 768 bytes, then the file's */
    char *image;                    /* +0x1E */
    char pad22[9];                  /* +0x22 */
    unsigned char loops;            /* +0x2B: 255 forever */
};                                  /* +0x2C */
RECORD_SIZE_P(flc_player, 44);

/* ---- the user interface ----------------------------------------------------------------- */

/* a screen rectangle, corners included, and for a mouse button its handler: the menus' button
 * tables (talk_buttons, inv_buttons, hud_buttons...; the handler gets the button's index, or
 * nothing) and the boxes the code builds (note_text_box, rect_overlap); always 12 bytes, as the
 * boxes' frame slots show */
struct rect {
    short x0;                       /* +0x00 */
    short y0;                       /* +0x02 */
    short x1;                       /* +0x04 */
    short y1;                       /* +0x06 */
    int (*handler)();               /* +0x08: buttons */
};                                  /* +0x0C */
RECORD_SIZE_P(rect, 12);

/* a where-is topic of the talk window (talk_place_topics, in scratch_buffer; talk_place_topic_count
 * of them): talk_build_place_topics lists the town's buildings by distance, the quest code adds
 * its places, persons and items */
struct talk_place_topic {
    signed char category;           /* +0x00: the building's type's index in talk_category_building_types,
                                       13 other buildings, 14 the region */
    unsigned char kind;             /* +0x01: 0 a building, 1 a quest place, 2 a quest person, 3 a quest
                                       item */
    unsigned char quest;            /* +0x02: the quest's id (kinds 1-3) */
    struct building *building;      /* +0x03: 0 none */
    char pad07[4];                  /* +0x07 */
    int distance;                   /* +0x0B: building_distance */
    short messages[2];              /* +0x0F: the quest resource's messages (kinds 1-3) */
};                                  /* +0x13 */
RECORD_SIZE_P(talk_place_topic, 19);

/* what the player asked an NPC where to find (D_001965FC; talk_where_target points at it):
 * talk_prepare_where_answer fills it, talk_hint_text_id and the %loc, %reg macros read it */
struct talk_where {
    char pad00[4];                  /* +0x00 */
    short pad04;                    /* +0x04: talk_prepare_where_answer clears it; talk_hint_text_id
                                       answers with the building only when it is 0 */
    char pad06;                     /* +0x06 */
    unsigned char region;           /* +0x07: the region asked about (%reg) */
    char pad08[8];                  /* +0x08 */
    unsigned char kind;             /* +0x10: 5 for a place in the region (talk_prepare_where_answer) */
    char pad11;                     /* +0x11 */
    struct building *building;      /* +0x12: the building asked about (a where-is topic's), 0 none
                                       (%loc) */
};
RECORD_OFFSET(talk_where, building, 0x12);

/* what engine_pick_object finds under a screen point (the global pick_result points to the
 * caller's; the click handlers get it) */
struct pick_result {
    int flags;                      /* +0x00: 1 hit, 2 a sprite, 4 a model, 8 a block model */
    struct record *object;          /* +0x04 */
    int plane;                      /* +0x08: the model's plane index (block models: + index << 8) */
    short block_model_index;        /* +0x0C: in the block's models */
    short model_id;                 /* +0x0E: type-56 objects: the model's id */
    short variant;                  /* +0x10: and variant */
};                                  /* +0x12 */
RECORD_SIZE_P(pick_result, 18);

/* the notebook's page (note_page, 3640 bytes): its entries one after the other, text (91 bytes)
 * or a line (11 bytes), up to a zero kind (note_page_walk) */
struct note_text {
    unsigned char kind;             /* +0x00: 1 */
    short x;                        /* +0x01 */
    short y;                        /* +0x03 */
    unsigned char font;             /* +0x05: note_font, 0-3 */
    unsigned char flags;            /* +0x06: 1 centred, 2 shadow, 64 selected */
    unsigned char colour;           /* +0x07 */
    char pad08[3];                  /* +0x08 */
    char text[80];                  /* +0x0B */
};                                  /* +0x5B */
RECORD_SIZE(note_text, 91);

struct note_line {
    unsigned char kind;             /* +0x00: 2 */
    short x0;                       /* +0x01 */
    short y0;                       /* +0x03 */
    short x1;                       /* +0x05 */
    short y1;                       /* +0x07 */
    unsigned char colour;           /* +0x09 */
    unsigned char flags;            /* +0x0A: 64 selected */
};                                  /* +0x0B */
RECORD_SIZE(note_line, 11);

union note_entry {
    unsigned char kind;             /* +0x00: 0 the end of the page, 1 text, 2 a line */
    struct note_text text;
    struct note_line line;
};

/* ---- the world ---------------------------------------------------------------------------- */

/* a FLATS.CFG entry (flats_cfg[], flats_cfg_load; flats_cfg_find by picture) */
struct flat_cfg {
    unsigned short image;           /* +0x00: archive<<7 | record */
    short pad02;                    /* +0x02 */
    unsigned short face;            /* +0x04: the talk portrait in TFAC00I0.RCI, 0 a random one
                                       (npc_load_face) */
    unsigned char flags;            /* +0x06: 1 female ('2'), 2 hidden by the content filter ('?') */
    unsigned char pad07;            /* +0x07: the entry's first number */
    unsigned char pad08;            /* +0x08: its second */
    char name[31];                  /* +0x09 */
};                                  /* +0x28 */
RECORD_SIZE(flat_cfg, 40);

/* a region's state (regions[62], saved in SAVEVARS.DAT; current_region_data points at the current
 * region's, region_enter; DFU PlayerEntity.RegionDataRecord) */
struct region {
    unsigned char values[29];       /* +0x00: DFU Values: a set event flag's countdown (region_flag_set:
                                       random(min, max) from region_event_durations) */
    unsigned char flags[29];        /* +0x1D: DFU Flags (RegionDataFlags): the events; 0 WarBeginning,
                                       1 WarOngoing, 2 WarWon, 3 WarLost (region_reset_war) */
    unsigned char groups[14];       /* +0x3A: DFU Flags2: a group is set while one of its flags is
                                       (region_event_flag_groups) */
    unsigned char precipitation_override; /* +0x48: the weather + 1 forced for the region, 0 none
                                       (sky_update, weather_draw_precipitation) */
    unsigned char punishment_flags; /* +0x49: DFU SeverePunishmentFlags: 1 banished, 2 death sentence */
    short legal_reputation;         /* +0x4A: DFU LegalRep */
    short persecuted_temple;        /* +0x4C: DFU IDOfPersecutedTemple: a god's faction id (%prg, %ptm) */
    unsigned short price_adjustment; /* +0x4E: DFU PriceAdjustment: 1000 normal, 250-4000 */
};                                  /* +0x50 */
RECORD_SIZE(region, 80);

/* a block of the dungeon being loaded (dungeon_blocks[32], from MAPS; DFU DungeonBlock):
 * dungeon_load_rdb_block loads its RDB file (rdb_dungeon_block points at it meanwhile) */
struct dungeon_block {
    signed char x;                  /* +0x00: in blocks */
    signed char z;                  /* +0x01 */
    unsigned short number:10;       /* +0x02: DFU BlockNumber: the RDB file's number */
    unsigned short is_starting:1;   /*        DFU IsStartingBlock */
    unsigned short index:3;         /*        DFU BlockIndex: the file's letter ('NWLSBM', D_0017A844) */
};                                  /* +0x04 */
RECORD_SIZE(dungeon_block, 4);

/* the objects made for an RDB block's objects (rdb_object_ids[512], rdb_object_id_count of them;
 * rdb_create_objects): rdb_object_id_by_offset gives the links their objects' ids */
struct rdb_object_id {
    int offset;                     /* +0x00: the RDB object's, from the start of the file */
    int id;                         /* +0x04: the object's record id */
};                                  /* +0x08 */
RECORD_SIZE(rdb_object_id, 8);

/* a door of the location being loaded (loaded_location.doors, from MAPS) */
struct location_door {
    unsigned short building_index;  /* +0x00: into the location's buildings, 0xFFFF none */
    unsigned short flags;           /* +0x02: 0x3FF a faction id; 0x1000, 0x2000, 0x4000, 0x8000 the
                                       quest site tests; the high nibble cleared when the door's
                                       quest object is deleted */
    unsigned short id;              /* +0x04: the door object's id, its low word */
};                                  /* +0x06 */
RECORD_SIZE(location_door, 6);

/* ---- items -------------------------------------------------------------------------------- */

/* an item template (item_templates[288]; item_init_from_template; DFU ItemTemplate) */
struct item_template {
    char name[24];                  /* +0x00 */
    int weight;                     /* +0x18 */
    short condition;                /* +0x1C: hit points */
    int capacity;                   /* +0x1E: DFU capacityOrTarget */
    int value;                      /* +0x22: the base price */
    short enchant_points;           /* +0x26 */
    unsigned char rarity;           /* +0x28 */
    unsigned char variants;         /* +0x29 */
    unsigned char draw_order;       /* +0x2A */
    unsigned char flags;            /* +0x2B: item_flags; bit 0 (DFU isBluntWeapon...) */
    short dropped_image;            /* +0x2C */
    unsigned short inventory_image; /* +0x2E: 32512 none */
};                                  /* +0x30 */
RECORD_SIZE(item_template, 48);

/* a MAGIC.DEF record: a magic item or an artifact (magic_def, item_make_magic); its enchantments
 * are (type, param) byte pairs, as are the item maker's ruled-out pairs (itemmaker_slot_exclusions,
 * 10 slots x 5) */
struct magic_enchantment {
    unsigned char type;             /* +0x00: 255 ends the list */
    unsigned char param;            /* +0x01: 255 none */
};

struct magic_template {
    char name[32];                  /* +0x00 */
    unsigned char artifact;         /* +0x20 */
    unsigned char group;            /* +0x21 */
    unsigned char group_index;      /* +0x22 */
    struct magic_enchantment enchantments[10]; /* +0x23 */
    short condition;                /* +0x37 */
    int value;                      /* +0x39 */
    unsigned char material;         /* +0x3D */
};                                  /* +0x3E */
RECORD_SIZE(magic_template, 62);

/* ---- creatures ----------------------------------------------------------------------------- */

/* a creature type's entry in monster_table (29 bytes, by character.race; monster_init) */
struct monster_template {
    unsigned char hp_bonus;         /* +0x00: added to the d8 rolls */
    unsigned char armor;            /* +0x01: x5 into armor_values */
    unsigned char loot_table;       /* +0x02: 1-based, 0 none */
    unsigned char min_metal_to_hit; /* +0x03 */
    unsigned short flags;           /* +0x04: character.table_flags: 1 flying, 2 spellcaster, 4 ...,
                                       64 aquatic (names.csv monster_table_flags) */
    unsigned short attack_damage[5]; /* +0x06: copied to character.attack_damage (10 bytes) */
    char pad10[10];                 /* +0x10 */
    signed char range_min;          /* +0x1A: character.pad22A is a random value in the range */
    signed char range_max;          /* +0x1B */
    unsigned char level;            /* +0x1C */
};                                  /* +0x1D */
RECORD_SIZE(monster_template, 29);

/* ---- quests -------------------------------------------------------------------------------- */

/* an escort's face at the top left of the screen (quest_faces[10], saved in SAVEVARS.DAT;
 * quest_face_add, quest_faces_draw) */
struct quest_face {
    unsigned char face;             /* +0x00: gender << 7 | name bank << 6 | 16 a set face | index */
    unsigned char quest_id;         /* +0x01 */
    int object_id;                  /* +0x02: the person or foe, 0 a free slot */
    struct image *image;            /* +0x06: the face's image (from quest_face_images) */
};                                  /* +0x0A */
RECORD_SIZE_P(quest_face, 10);

/* ---- sound -------------------------------------------------------------------------------- */

/* HMI SOS's sample descriptor (sos_load_sample builds one in front of the loaded sample;
 * each sound channel starts with one, given to the driver to start the sample) */
struct sos_sample {
    char *data;                     /* +0x00: the samples */
    char pad04[8];                  /* +0x04 */
    int length;                     /* +0x0C: bytes */
    int pad10;                      /* +0x10: the length again (sound_play_sample) */
    char pad14[24];                 /* +0x14 */
    int volume;                     /* +0x2C: left << 16 | right, 0x7FFF each the loudest */
    int loop;                       /* +0x30: -1 loops */
    int rate;                       /* +0x34: samples per second */
    int bits;                       /* +0x38: per sample */
    int channels;                   /* +0x3C */
    int format;                     /* +0x40: 0x8000 8-bit unsigned samples, 0 16-bit */
    int pan;                        /* +0x44: 0x8000 the centre (sound_volume_pan) */
    char pad48[168];                /* +0x48 */
};                                  /* +0xF0 */
RECORD_SIZE_P(sos_sample, 240);

/* HMI SOS's song descriptor (sos_load_song builds one in front of the MIDI data it loads) */
struct sos_song {
    char *data;                     /* +0x00: the song (a MIDI.BSA record) */
    char pad04[28];                 /* +0x04 */
};                                  /* +0x20 */
RECORD_SIZE_P(sos_song, 32);

/* a sound in the sound cache (sound_cache[256]: sound_cache_load fills it, sound_cache_trim frees
 * the oldest) */
struct sound_cache_entry {
    int last_frame;                 /* +0x00: frame_counter when last loaded or used */
    int id;                         /* +0x04: the DAGGER.SND record id, -1 none */
    int size;                       /* +0x08 */
    char *data;                     /* +0x0C: 0 a free entry */
};                                  /* +0x10 */
RECORD_SIZE_P(sound_cache_entry, 16);

/* a sound channel (sound_channels[4]: 0-2 the sounds, 3 the ambient loop) */
struct sound_channel {
    struct sos_sample sample;       /* +0x00 */
    int handle;                     /* +0xF0: the driver's sample handle, 0x12345678 none */
    int priority;                   /* +0xF4: a new sound takes the channel of a lower one */
    int volume;                     /* +0xF8: the computed volume */
    struct record *source;          /* +0xFC: the object the sound comes from, 0 none */
    int position[3];                /* +0x100: its x, y, z (sound_channel_set_source) */
};                                  /* +0x10C */
RECORD_SIZE_P(sound_channel, 268);

#pragma pack()

#endif
