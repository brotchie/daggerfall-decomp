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

#ifndef RECORD_SIZE
#define RECORD_SIZE(tag, n) typedef char tag##_size_check[(sizeof(struct tag) == (n)) ? 1 : -1]
#define RECORD_OFFSET(tag, m, n) \
    typedef char tag##_##m##_offset_check[((unsigned)&((struct tag *)0)->m == (n)) ? 1 : -1]
#endif

#pragma pack(1)

struct record;

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
RECORD_SIZE(profile, 176);

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
RECORD_SIZE(flc_player, 44);

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
RECORD_SIZE(rect, 12);

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
RECORD_SIZE(pick_result, 18);

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
RECORD_SIZE(quest_face, 10);

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
RECORD_SIZE(sos_sample, 240);

/* HMI SOS's song descriptor (sos_load_song builds one in front of the MIDI data it loads) */
struct sos_song {
    char *data;                     /* +0x00: the song (a MIDI.BSA record) */
    char pad04[28];                 /* +0x04 */
};                                  /* +0x20 */
RECORD_SIZE(sos_song, 32);

/* a sound in the sound cache (sound_cache[256]: sound_cache_load fills it, sound_cache_trim frees
 * the oldest) */
struct sound_cache_entry {
    int last_frame;                 /* +0x00: frame_counter when last loaded or used */
    int id;                         /* +0x04: the DAGGER.SND record id, -1 none */
    int size;                       /* +0x08 */
    char *data;                     /* +0x0C: 0 a free entry */
};                                  /* +0x10 */
RECORD_SIZE(sound_cache_entry, 16);

/* a sound channel (sound_channels[4]: 0-2 the sounds, 3 the ambient loop) */
struct sound_channel {
    struct sos_sample sample;       /* +0x00 */
    int handle;                     /* +0xF0: the driver's sample handle, 0x12345678 none */
    int priority;                   /* +0xF4: a new sound takes the channel of a lower one */
    int volume;                     /* +0xF8: the computed volume */
    struct record *source;          /* +0xFC: the object the sound comes from, 0 none */
    int position[3];                /* +0x100: its x, y, z (sound_channel_set_source) */
};                                  /* +0x10C */
RECORD_SIZE(sound_channel, 268);

#pragma pack()

#endif
