/* records.h: the game's record structures (docs/state.md, config/names.csv).
 *
 * Every game object (world object, entity, item, spell, effect, quest...) starts with the same
 * 71-byte header, Daggerfall Unity's RecordRoot; the record's own data follows at +0x47. */
#ifndef RECORDS_H
#define RECORDS_H

#pragma pack(1)
struct record {
    unsigned char type;             /* +0x00: 2 item, 3 character, 9 spell, 11 effect, ... */
    short angle_x;                  /* +0x01 */
    short yaw;                      /* +0x03: 2048 to a turn, clockwise from north */
    short angle_z;                  /* +0x05 */
    int x;                          /* +0x07: 1/40 m east */
    int y;                          /* +0x0B: 1/40 m, height */
    int z;                          /* +0x0F: 1/40 m north */
    char pad13[2];
    unsigned short flags;           /* +0x15: 0x20 not owned, 0x02 not listed */
    unsigned short owner;           /* +0x17 */
    char pad19[2];
    unsigned short image;           /* +0x1B: archive<<7 | record in the world */
    unsigned short image2;          /* +0x1D */
    unsigned int id;                /* +0x1F */
    unsigned char link_flag;        /* +0x23 */
    char pad24[2];
    unsigned char quest_id;         /* +0x26 */
    unsigned int parent_id;         /* +0x27: in the save file */
    unsigned int repair_due;        /* +0x2B */
    struct record *caster;          /* +0x2F: spells */
    struct record *twin;            /* +0x33 */
    struct record *next;            /* +0x37 */
    struct record *prev;            /* +0x3B */
    struct record *children;        /* +0x3F */
    struct record *parent;          /* +0x43 */
};                                  /* +0x47: the record's data */
#pragma pack()

#endif
