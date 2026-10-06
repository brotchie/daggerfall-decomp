/* disk.h: the 32-bit layouts the game's files keep, for the native build (docs/port.md,
   phase 3; build/port/agents/persist/report.md §4).

   FALL.EXE reads and writes some of its structs byte for byte: the SAVETREE.DAT records
   (the 71-byte header, then the data by record type), the SAVEVARS.DAT quest faces, header
   copy and factions, the MAPS.BSA location records, the BLOCKS.BSA RMB blocks and the QBN
   quest files. Natively pointers are 8 bytes, so the in-memory structs grow; the files must
   not (classic saves load, DOS Daggerfall and Daggerfall Unity read what the port writes).
   These are the file layouts: packed, every pointer a u32 slot, written from the offset
   comments of include/records.h. port/shim/persist.c converts them to and from the native
   structs; include/portio.h hooks the converters into the game's I/O functions.

   Native only: the Watcom build reads and writes the structs themselves. */
#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include "records.h"

typedef uint32_t u32slot;           /* a pointer as the file holds it: an id, an offset, the
                                       parent's type, or a DOS address nobody reads */

/* a compile-time check of a file layout */
#define DISK_SIZE(tag, n) _Static_assert(sizeof(struct tag) == (n), #tag " is not " #n " bytes")
#define DISK_OFFSET(tag, m, n) \
    _Static_assert(__builtin_offsetof(struct tag, m) == (n), #tag "." #m " is not at " #n)

#pragma pack(push, 1)

/* ---- the record header (DFU RecordRoot), 71 bytes -------------------------------------- */
struct record_disk {
    uint8_t type;                   /* +0x00 */
    int16_t angle_x;                /* +0x01 */
    int16_t yaw;                    /* +0x03 */
    int16_t angle_z;                /* +0x05 */
    int32_t x;                      /* +0x07 */
    int32_t y;                      /* +0x0B */
    int32_t z;                      /* +0x0F */
    uint16_t pad13;                 /* +0x13 */
    uint16_t flags;                 /* +0x15 */
    uint16_t pad17;                 /* +0x17 */
    int16_t pad19;                  /* +0x19 */
    uint16_t pad1B;                 /* +0x1B: type 56: the model count */
    uint16_t pad1D;                 /* +0x1D */
    uint32_t id;                    /* +0x1F */
    uint8_t link_flag;              /* +0x23 */
    int16_t pad24;                  /* +0x24 */
    uint8_t quest_id;               /* +0x26 */
    uint32_t parent_id;             /* +0x27 */
    uint32_t pad2B;                 /* +0x2B */
    u32slot caster;                 /* +0x2F: spells: the caster's id (savetree_write_record) */
    u32slot twin;                   /* +0x33: the twin's id when quest_id, else 0 */
    u32slot next;                   /* +0x37: garbage; the loader clears it */
    u32slot prev;                   /* +0x3B: garbage; cleared */
    u32slot children;               /* +0x3F: garbage; cleared */
    u32slot parent;                 /* +0x43: the parent's type (savetree_write_record) */
};                                  /* +0x47 */
DISK_SIZE(record_disk, 71);
DISK_OFFSET(record_disk, caster, 0x2F);

/* the bytes before the first pointer slot: the same in both layouts */
#define RECORD_DISK_PLAIN 0x2F
_Static_assert(__builtin_offsetof(struct record, caster) == RECORD_DISK_PLAIN,
               "struct record: the plain header part moved");

/* ---- the character record (types 3, 18, 44, 45, 46; markers 34 of kind 13/14), 634 bytes */
struct character_disk {
    uint8_t pad000[0x70];           /* +0x000: name .. knockback_speed */
    u32slot target;                 /* +0x070: an id in the save (save_unlink_character) */
    uint8_t pad074[0xFB];           /* +0x074: house .. skills[35] */
    u32slot equipped[27];           /* +0x16F: ids in the save */
    uint8_t pad1DB[0x9F];           /* +0x1DB: table_flags .. career (74 bytes at +0x230) */
};                                  /* +0x27A */
DISK_SIZE(character_disk, 634);
DISK_OFFSET(character_disk, equipped, 0x16F);

/* ---- a creature's animation (struct monster_anim; XnGine's xn_anim), 25 bytes ------------ */
struct monster_anim_disk {
    int16_t anim_frame;             /* +0x00 */
    int16_t pad02;                  /* +0x02 */
    u32slot anim_script;            /* +0x04: the DOS address of the ASCR record, raw */
    u32slot anim_script_pos;        /* +0x08: raw too: monster_reload_anim_cb keeps pos - script */
    int16_t frame_count;            /* +0x0C */
    int16_t timer;                  /* +0x0E */
    uint16_t anim_bits;             /* +0x10 */
    uint8_t pad12[2];               /* +0x12 */
    uint8_t anim_request;           /* +0x14 */
    uint8_t anim_facing;            /* +0x15 */
    uint8_t anim_record;            /* +0x16 */
    uint8_t anim_current;           /* +0x17 */
    uint8_t pad18;                  /* +0x18 */
};                                  /* +0x19 */
DISK_SIZE(monster_anim_disk, 25);

struct monster_disk {
    struct character_disk character; /* +0x000 */
    struct monster_anim_disk anim;  /* +0x27A */
};                                  /* +0x293 */
DISK_SIZE(monster_disk, 659);

/* ---- XnGine's model handle as the game keeps it (types 6, 32; inside block_model), 58 -- */
struct model_instance_disk {
    u32slot model;                  /* +0x00: model_get's result: a dead DOS address */
    u32slot lights;                 /* +0x04: the engine's, rewritten every frame */
    u32slot matrix;                 /* +0x08: the engine's */
    uint8_t angles[20];             /* +0x0C */
    int32_t x;                      /* +0x20 */
    int32_t y;                      /* +0x24 */
    int32_t z;                      /* +0x28 */
    int32_t missile_angles[3];      /* +0x2C */
    uint8_t engine38[2];            /* +0x38: xn_model_handle +0x38/+0x39 (the engine's) */
};                                  /* +0x3A */
DISK_SIZE(model_instance_disk, 58);

/* ---- RMB: a 3D object (DFU RmbBlock3dObjectRecord), 66 bytes --------------------------- */
struct block_model_disk {
    uint16_t id;                    /* +0x00 */
    uint8_t variant;                /* +0x02 */
    uint8_t kind;                   /* +0x03 */
    u32slot model;                  /* +0x04: DFU Unknown1; the loaded model (a dead address) */
    u32slot lights;                 /* +0x08: DFU Unknown2; the engine's */
    u32slot matrix;                 /* +0x0C: DFU Unknown3; the engine's */
    uint8_t pad10[20];              /* +0x10 */
    int32_t x;                      /* +0x24 */
    int32_t y;                      /* +0x28 */
    int32_t z;                      /* +0x2C */
    uint8_t pad30[4];               /* +0x30 */
    int32_t yaw;                    /* +0x34 */
    uint8_t pad38[10];              /* +0x38 */
};                                  /* +0x42 */
DISK_SIZE(block_model_disk, 66);

/* RMB flats (17), section-3 records (16), people (17), doors (19) hold no pointers: the
   native structs are the file's */
_Static_assert(sizeof(struct block_flat) == 17, "block_flat");
_Static_assert(sizeof(struct block_section3) == 16, "block_section3");

/* ---- RMB: a subrecord's header (DFU RmbBlockHeader), 17 bytes; a type-43 object's data -- */
struct block_disk {
    uint8_t model_count;            /* +0x00 */
    uint8_t flat_count;             /* +0x01 */
    uint8_t section3_count;         /* +0x02 */
    uint8_t people_count;           /* +0x03 */
    uint8_t door_count;             /* +0x04 */
    u32slot models;                 /* +0x05: the file's unknown shorts; in a save, the DOS
                                       address of the object's own models */
    u32slot flats;                  /* +0x09 */
    u32slot section3;               /* +0x0D */
};                                  /* +0x11 */
DISK_SIZE(block_disk, 17);

/* ---- RMB: the file header (DFU RmbFldHeader); the subrecords start at +0x1A78 ------------ */
struct rmb_file_disk {
    uint8_t block_data_count;       /* +0x0000 */
    uint8_t misc_model_count;       /* +0x0001 */
    uint8_t misc_flat_count;        /* +0x0002 */
    struct rmb_block_position positions[32]; /* +0x0003 */
    struct building buildings[32];  /* +0x0283 */
    u32slot block_data[32];         /* +0x05C3: DFU Section2UnknownData (rmb_index_records'
                                       pointers in FALL.EXE) */
    int32_t block_data_sizes[32];   /* +0x0643 */
    u32slot misc_models;            /* +0x06C3: DFU GroundData's header (pointers in FALL.EXE) */
    u32slot misc_flats;             /* +0x06C7 */
    uint8_t ground_tiles[256];      /* +0x06CB */
    uint8_t ground_scenery[256];    /* +0x07CB */
    uint8_t automap[4096];          /* +0x08CB */
    char name[13];                  /* +0x18CB */
    char other_names[32][13];       /* +0x18D8 */
};                                  /* +0x1A78: the subrecords, then the misc objects */
DISK_SIZE(rmb_file_disk, 0x1A78);
DISK_OFFSET(rmb_file_disk, block_data_sizes, 0x643);
DISK_OFFSET(rmb_file_disk, ground_tiles, 0x6CB);

/* ---- MAPS.BSA: a location's data after its 71-byte header, 48 bytes ---------------------- */
struct location_disk {
    char name[32];                  /* +0x00 */
    uint8_t width;                  /* +0x20 */
    uint8_t height;                 /* +0x21 */
    uint8_t kind;                   /* +0x22 */
    uint8_t pad23[2];               /* +0x23 */
    uint16_t object_counter;        /* +0x25 */
    uint16_t marker_counter;        /* +0x27 */
    uint16_t building_count;        /* +0x29 */
    u32slot buildings;              /* +0x2B: file garbage; location_read_record mallocs them */
    uint8_t pad2F;                  /* +0x2F */
};                                  /* +0x30 */
DISK_SIZE(location_disk, 48);

/* the record location_read_record reads: header and location, 119 bytes */
struct location_record_disk {
    struct record_disk header;      /* +0x00 */
    struct location_disk location;  /* +0x47 */
};                                  /* +0x77 */
DISK_SIZE(location_record_disk, 119);

/* ---- SAVEVARS.DAT: a faction, 92 bytes ----------------------------------------------------- */
struct faction_disk {
    uint8_t type;                   /* +0x00 */
    uint8_t region;                 /* +0x01 */
    uint8_t ruler;                  /* +0x02 */
    char name[26];                  /* +0x03 */
    int16_t reputation;             /* +0x1D */
    int16_t power;                  /* +0x1F */
    uint16_t id;                    /* +0x21 */
    uint16_t vampire_clan;          /* +0x23 */
    uint16_t flags;                 /* +0x25 */
    uint32_t seed;                  /* +0x27 */
    uint32_t politics_factor;       /* +0x2B */
    uint16_t flats[2];              /* +0x2F */
    uint16_t face;                  /* +0x33 */
    uint8_t race;                   /* +0x35 */
    uint8_t social_group;           /* +0x36 */
    uint8_t guild_group;            /* +0x37 */
    u32slot allies[3];              /* +0x38: faction ids (faction_save_r) */
    u32slot enemies[3];             /* +0x44: faction ids */
    u32slot next;                   /* +0x50: garbage, never read back (faction_load copies 80) */
    u32slot child;                  /* +0x54 */
    u32slot parent;                 /* +0x58 */
};                                  /* +0x5C */
DISK_SIZE(faction_disk, 92);
_Static_assert(__builtin_offsetof(struct faction, allies) == 0x38, "faction: plain part moved");

/* ---- SAVETREE.DAT tail: an action link, 39 bytes ------------------------------------------ */
struct link_disk {
    uint8_t plain[0x23];            /* +0x00: object_id .. delta[3] (struct link) */
    u32slot object;                 /* +0x23: the moved object's id */
};                                  /* +0x27 */
DISK_SIZE(link_disk, 39);
_Static_assert(__builtin_offsetof(struct link, object) == 0x23, "link: plain part moved");

/* ---- SAVEVARS.DAT: a quest face, 10 bytes (quest_faces[10]) ------------------------------ */
struct quest_face_disk {
    uint8_t face;                   /* +0x00 */
    uint8_t quest_id;               /* +0x01 */
    int32_t object_id;              /* +0x02 */
    u32slot image;                  /* +0x06: garbage; quest_faces_after_load recomputes it */
};                                  /* +0x0A */
DISK_SIZE(quest_face_disk, 10);
_Static_assert(__builtin_offsetof(struct quest_face, image) == 6, "quest_face: plain part moved");

/* ---- QBN (type-14 data; the quest files): the records with pointers (qbn/notes.md) -------
   struct quest (60), qbn_state (8) and the opaque sections 1, 2, 5 (94, 34, 16) hold none */
struct qbn_arg_disk {
    uint8_t negate;                 /* +0x00 */
    u32slot record;                 /* +0x01: file: section << 8 | index (0x12345678 none);
                                       save: the record's offset from the quest start */
    int16_t section;                /* +0x05 */
    int32_t value;                  /* +0x07 */
    u32slot object;                 /* +0x0B: save: an object id */
};                                  /* +0x0F */
DISK_SIZE(qbn_arg_disk, 15);

struct qbn_op_disk {                /* section 8 */
    int16_t opcode;                 /* +0x00 */
    uint16_t flags;                 /* +0x02 */
    int16_t arg_count;              /* +0x04 */
    struct qbn_arg_disk args[5];    /* +0x06 */
    int16_t message;                /* +0x51 */
    int32_t last_minutes;           /* +0x53 */
};                                  /* +0x57 */
DISK_SIZE(qbn_op_disk, 87);

struct qbn_timer_disk {             /* section 6 */
    int16_t pad00;                  /* +0x00 */
    uint16_t flags;                 /* +0x02 */
    uint8_t type;                   /* +0x04 */
    int32_t minimum;                /* +0x05 */
    int32_t maximum;                /* +0x09 */
    int32_t start;                  /* +0x0D */
    uint32_t delay;                 /* +0x11 */
    u32slot link1;                  /* +0x15: file: a place or person index; save: an id */
    u32slot link2;                  /* +0x19 */
    int32_t state_hash;             /* +0x1D */
};                                  /* +0x21 */
DISK_SIZE(qbn_timer_disk, 33);

struct qbn_item_disk {              /* section 0 */
    uint8_t pad00[2];               /* +0x00 */
    uint8_t flags;                  /* +0x02 */
    int16_t group;                  /* +0x03 */
    int16_t index;                  /* +0x05 */
    int32_t symbol;                 /* +0x07 */
    u32slot object;                 /* +0x0B */
    int16_t messages[2];            /* +0x0F */
};                                  /* +0x13 */
DISK_SIZE(qbn_item_disk, 19);

struct qbn_person_disk {            /* section 3 */
    uint8_t pad00[2];               /* +0x00 */
    uint16_t flags;                 /* +0x02 */
    int16_t kind;                   /* +0x04 */
    int16_t faction_id;             /* +0x06 */
    int32_t symbol;                 /* +0x08 */
    u32slot object;                 /* +0x0C */
    int16_t messages[2];            /* +0x10 */
};                                  /* +0x14 */
DISK_SIZE(qbn_person_disk, 20);

struct qbn_place_disk {             /* section 4 */
    uint8_t pad00[2];               /* +0x00 */
    uint8_t flags;                  /* +0x02 */
    int8_t scope;                   /* +0x03 */
    uint16_t p1;                    /* +0x04 */
    int16_t p2;                     /* +0x06 */
    int16_t p3;                     /* +0x08 */
    uint8_t pad0A[2];               /* +0x0A */
    int32_t symbol;                 /* +0x0C */
    u32slot object;                 /* +0x10 */
    int16_t messages[2];            /* +0x14 */
};                                  /* +0x18 */
DISK_SIZE(qbn_place_disk, 24);

struct qbn_foe_disk {               /* section 7 */
    uint8_t pad00[3];               /* +0x00 */
    uint8_t type;                   /* +0x03 */
    uint8_t count;                  /* +0x04 */
    uint8_t killed;                 /* +0x05 */
    int32_t symbol;                 /* +0x06 */
    u32slot object;                 /* +0x0A */
};                                  /* +0x0E */
DISK_SIZE(qbn_foe_disk, 14);

struct qbn_text_var_disk {          /* section 10 (quest text_offset) */
    char name[20];                  /* +0x00: 0 ends the list */
    uint8_t section;                /* +0x14 */
    int16_t index;                  /* +0x15 */
    u32slot record;                 /* +0x17: a stale address; recomputed on load */
};                                  /* +0x1B */
DISK_SIZE(qbn_text_var_disk, 27);

#pragma pack(pop)

/* ---- the converters (port/shim/persist.c) ------------------------------------------------
   *_from_disk widens: every u32 slot is zero-extended (ids, offsets and types stay as they
   are), except where the slot is dead (below). *_to_disk keeps each slot's low 32 bits.
   persist_exact = 1 widens dead slots too, so that disk -> native -> disk gives back the
   same bytes (port/test/savetest.c); the game leaves it 0, and then:
   - model handles (types 6, 32, 43, 56: model, lights, matrix) are zeroed on read;
   - a type-43 block's models/flats/section3 are pointed at its own data (they held the old
     session's DOS addresses: an original hazard). */
extern int persist_exact;

/* conversion problems seen (offsets off a record start, truncated data ...); the test
   requires 0 */
extern int persist_anomalies;

/* the header: 71 disk bytes <-> RECORD_HEADER_SIZE native */
void persist_header_from_disk(struct record *dst, const void *src);
void persist_header_to_disk(void *dst, const struct record *src);

/* a SAVETREE record (header and data, disk_len bytes as the file's length word says):
   the native length, or the disk length; dst may be 0 to measure only */
int persist_record_from_disk(void *dst, const void *src, int disk_len);
int persist_record_to_disk(void *dst, const void *src, int native_len);

/* a record's data by type: header is the record's native header (types 56 and 34 need its
   counts; data address for type 43); the sizes returned are the other side's */
int persist_data_from_disk(const struct record *header, void *dst, const void *src, int size);
int persist_data_to_disk(const struct record *header, void *dst, const void *src, int size);

/* QBN: form PERSIST_QBN_FILE (a quest file: arg.record is section << 8 | index) or
   PERSIST_QBN_SAVE (a saved quest: arg.record is an offset from the quest start, remapped
   between the layouts); dst may be 0 to measure only. Return the other side's size. */
enum { PERSIST_QBN_FILE, PERSIST_QBN_SAVE };
int persist_qbn_from_disk(void *dst, const void *src, int size, int form);
int persist_qbn_to_disk(void *dst, const void *src, int size, int form);

/* the single structs */
void persist_character_from_disk(struct character *dst, const struct character_disk *src);
void persist_character_to_disk(struct character_disk *dst, const struct character *src);
void persist_monster_anim_from_disk(struct monster_anim *dst, const struct monster_anim_disk *src);
void persist_monster_anim_to_disk(struct monster_anim_disk *dst, const struct monster_anim *src);
void persist_link_from_disk(struct link *dst, const struct link_disk *src);
void persist_link_to_disk(struct link_disk *dst, const struct link *src);
void persist_faction_from_disk(struct faction *dst, const struct faction_disk *src);
void persist_faction_to_disk(struct faction_disk *dst, const struct faction *src);
void persist_quest_face_from_disk(struct quest_face *dst, const struct quest_face_disk *src);
void persist_quest_face_to_disk(struct quest_face_disk *dst, const struct quest_face *src);
void persist_location_from_disk(struct location *dst, const struct location_disk *src);
void persist_location_to_disk(struct location_disk *dst, const struct location *src);
void persist_block_model_from_disk(struct block_model *dst, const struct block_model_disk *src);
void persist_block_model_to_disk(struct block_model_disk *dst, const struct block_model *src);

/* an RMB subrecord (block header, models, flats, section-3 records) as a type-43 object's
   data; returns the native size (dst 0: measure) */
int persist_block_from_disk(struct block *dst, const void *src);
/* the file bytes of an RMB subrecord's header and lists (its people follow) */
int persist_block_disk_size(const void *src);

/* a native type-43 block's pointers, at its own data */
struct block_model *persist_block_relink(struct block *block);

#endif
