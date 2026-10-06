/* portio.h: where the game reads or writes a struct in its 32-bit layout (docs/port.md,
   phase 3; build/port/agents/persist/report.md §4).

   SAVETREE.DAT, SAVEVARS.DAT, MAPS.BSA, the RMB blocks of BLOCKS.BSA and the QBN files hold
   structs as FALL.EXE lays them out: 4-byte pointers, a 71-byte record header. Natively the
   structs grow (8-byte pointers), and the files must not. Each hook below is the original
   call or expression under Watcom, token for token, so FALL.EXE does not change; natively it
   goes through a converter of port/shim/persist.c (the file layouts are port/include/disk.h).

   The macros that name a struct's size or layout need records.h. */
#ifndef PORTIO_H
#define PORTIO_H

#include "ptrint.h"

#ifdef DAGGER_PORT

struct record;
struct faction;
struct link;
struct block;
struct block_model;
struct block_flat;
struct rmb_file;

/* ---- SAVETREE.DAT records (savetree_write_record, savetree_read_chunk) ------------------ */

/* the heap block's size of a record (its header and data): the SAVETREE record length */
#define RECORD_BLOCK_SIZE(p) \
    ((int)((struct mem_block *)((char *)(p) - sizeof(struct mem_block)))->size)
/* the length word goes out with the converted record (its disk length) */
#define PORT_SAVETREE_LEN(fd, p) ((void)(fd), (void)(p), 4)
/* writes the length word and the record converted to its disk layout; len on success */
#define PORT_SAVETREE_WRITE(fd, buf, len) persist_savetree_write(fd, buf, len)
/* reads size disk bytes (the length word read) and converts them into buf; size becomes the
   native length */
#define PORT_SAVETREE_READ(fd, buf, size) persist_savetree_read(fd, buf, &(size))
int persist_savetree_write(int fd, struct record *buf, int len);
int persist_savetree_read(int fd, struct record *buf, int *size);

/* a type-43 block's models (and flats, section3) at its own data, after a load */
#define PORT_BLOCK_RELINK(block) persist_block_relink(block)
struct block_model *persist_block_relink(struct block *block);

/* ---- SAVEVARS.DAT ------------------------------------------------------------------------ */

/* quest_faces: n disk bytes (10 per face) */
#define PORT_READ_QUEST_FACES(fd, p, n) persist_read_quest_faces(fd, p, n)
#define PORT_WRITE_QUEST_FACES(fd, p, n) persist_write_quest_faces(fd, p, n)
/* a record header copy (saved_player_object): n = 71 disk bytes */
#define PORT_READ_HEADER(fd, p, n) persist_read_header(fd, p, n)
#define PORT_WRITE_HEADER(fd, p, n) persist_write_header(fd, p, n)
/* one faction (92 disk bytes) */
#define PORT_READ_FACTION(fd, p) persist_read_faction(fd, p)
#define PORT_WRITE_FACTION(fd, p) persist_write_faction(fd, p)
int persist_read_quest_faces(int fd, void *faces, int n);
int persist_write_quest_faces(int fd, void *faces, int n);
int persist_read_header(int fd, void *header, int n);
int persist_write_header(int fd, void *header, int n);
int persist_read_faction(int fd, struct faction *faction);
int persist_write_faction(int fd, struct faction *faction);

/* ---- SAVETREE.DAT tail: the action links ------------------------------------------------- */

/* n links (39 disk bytes each) */
#define PORT_READ_LINKS(fd, p, n) persist_read_links(fd, p, n)
#define PORT_WRITE_LINKS(fd, p, n) persist_write_links(fd, p, n)
/* n active links: u32 indexes on disk, pointer-wide slots in memory */
#define PORT_READ_INDEXES(fd, p, n) persist_read_indexes(fd, p, n)
int persist_read_links(int fd, struct link *links, int n);
int persist_write_links(int fd, struct link *links, int n);
int persist_read_indexes(int fd, iptr *slots, int n);

/* ---- MAPS.BSA: a location's header and data (location_read_record) ---------------------- */

/* the native buffer for them; the read takes n = 119 disk bytes */
#define LOCATION_RECORD_SIZE (RECORD_HEADER_SIZE + REC_SIZEOF(struct location))
#define PORT_READ_LOCATION(fd, p, n) persist_read_location(fd, p, n)
int persist_read_location(int fd, struct record *object, int n);

/* ---- BLOCKS.BSA: RMB blocks -------------------------------------------------------------- */

/* the RMB file read into the rmb buffer, laid out as struct rmb_file natively */
#define PORT_READ_RMB(fd, record, rmb) \
    persist_rmb_from_disk(rmb, archive_read_record(fd, record, (iptr)(rmb) + persist_rmb_growth()))
int persist_rmb_growth(void);
struct rmb_file *persist_rmb_from_disk(struct rmb_file *rmb, iptr loaded);
/* rmb_index_records' pointers: the file keeps 4-byte slots, so natively they live here */
extern struct block *persist_rmb_block_data[32];
extern struct block_model *persist_rmb_misc_models;
extern struct block_flat *persist_rmb_misc_flats;
#define RMB_BLOCK_DATA(rmb, i) persist_rmb_block_data[i]
#define RMB_MISC_MODELS(rmb) persist_rmb_misc_models
#define RMB_MISC_FLATS(rmb) persist_rmb_misc_flats
/* the flats after n file models (66 bytes each) */
#define RMB_FLATS_AFTER_MODELS(models, n) \
    ((struct block_flat *)((char *)(models) + (n) * PERSIST_BLOCK_MODEL_DISK))
#define PERSIST_BLOCK_MODEL_DISK 66
/* n file models into native block_models (town_block_create_misc_objects) */
#define PORT_COPY_BLOCK_MODELS(dst, src, n, file, line) persist_block_models_from_disk(dst, src, n)
void persist_block_models_from_disk(struct block_model *dst, void *src, int n);
/* an RMB subrecord into a type-43 object's data (rmb_add_subrecord) */
#define PORT_COPY_BLOCK(dst, src, size, file, line) persist_block_from_disk(dst, src)
int persist_block_from_disk(struct block *dst, const void *src);
/* the file bytes after a subrecord's header and lists: its people */
#define RMB_PEOPLE(record, size) ((iptr)((char *)(record) + persist_block_disk_size(record)))
int persist_block_disk_size(const void *record);

/* ---- QBN quest files (quest_start) -------------------------------------------------------- */

/* the native size of a quest file of n bytes, and the file into the type-14 data */
#define PORT_QBN_SIZE(file, n) persist_qbn_file_size((void *)(file), n)
#define PORT_QBN_COPY(dst, file, n, srcfile, line) persist_qbn_file_copy(dst, (void *)(file), n)
int persist_qbn_file_size(void *file, int n);
void persist_qbn_file_copy(void *dst, void *file, int n);


#else /* Watcom: the original expressions */

#define RECORD_BLOCK_SIZE(p) (*(int *)((char *)(p) - 6))
#define PORT_SAVETREE_LEN(fd, p) write(fd, p, 4)
#define PORT_SAVETREE_WRITE(fd, buf, len) write(fd, buf, len)
#define PORT_SAVETREE_READ(fd, buf, size) read(fd, buf, size)
#define PORT_BLOCK_RELINK(block) (block)->models

#define PORT_READ_QUEST_FACES(fd, p, n) read(fd, p, n)
#define PORT_WRITE_QUEST_FACES(fd, p, n) write(fd, p, n)
#define PORT_READ_HEADER(fd, p, n) read(fd, p, n)
#define PORT_WRITE_HEADER(fd, p, n) write(fd, p, n)
#define PORT_READ_FACTION(fd, p) read(fd, p, 92)
#define PORT_WRITE_FACTION(fd, p) write(fd, p, 92)

#define PORT_READ_LINKS(fd, p, n) read(fd, p, n * 39)
#define PORT_WRITE_LINKS(fd, p, n) write(fd, p, n * 39)
#define PORT_READ_INDEXES(fd, p, n) read(fd, p, n << 2)

#define LOCATION_RECORD_SIZE 119
#define PORT_READ_LOCATION(fd, p, n) read(fd, p, n)

#define PORT_READ_RMB(fd, record, rmb) archive_read_record(fd, record, (iptr)rmb)
#define RMB_BLOCK_DATA(rmb, i) rmb->block_data[i]
#define RMB_MISC_MODELS(rmb) rmb->misc_models
#define RMB_MISC_FLATS(rmb) rmb->misc_flats
#define RMB_FLATS_AFTER_MODELS(models, n) (struct block_flat *)(models + n)
#define PORT_COPY_BLOCK_MODELS(dst, src, n, file, line) mc_memcpy(dst, src, n * 66, file, line, 4)
#define PORT_COPY_BLOCK(dst, src, size, file, line) mc_memcpy(dst, src, size, file, line, 4)
#define RMB_PEOPLE(record, size) (iptr)((char *)record + size)

#define PORT_QBN_SIZE(file, n) n
#define PORT_QBN_COPY(dst, file, n, srcfile, line) mc_memcpy(dst, (void *)file, n, srcfile, line, 4)


#endif

#endif
