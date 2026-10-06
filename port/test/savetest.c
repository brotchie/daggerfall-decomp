/* savetest.c: the 32-bit file layouts through the native converters (port/shim/persist.c,
   port/include/disk.h; docs/port.md, phase 3).

   savetest [--saves DIR] [--arena2 DIR]
     --saves   a folder of save folders, each with SAVETREE.DAT and SAVEVARS.DAT (default
               build/port/io/saves: the 18 classic saves of orig/saves, unzipped)
     --arena2  the game's ARENA2 (default ~/dagger_comp/build/game/ARENA2)

   What it checks:
   - SAVETREE.DAT (header, buildings, the location and nonworld records until a 0 length,
     the links, the active links): every record disk -> native -> disk gives the same bytes;
     the native fields read as the file's; a saved quest's arg records land on the records of
     their sections; then the whole file read and written again through the game's hooks
     (persist_savetree_read/write, persist_read_links ...) is the same file;
   - SAVEVARS.DAT: the quest faces, the header copy and the factions, the same way;
   - MAPS.BSA: every location record (header and location) to native, fields against the file;
   - BLOCKS.BSA: every RMB block into the native rmb_file, every subrecord into a native
     type-43 block and the misc models into type-56 models, fields against the file;
   - every QBN quest file to native and back.
   Prints PASS or FAIL with the counts; exits 1 on a failure. */
#ifndef DAGGER_PORT
#define DAGGER_PORT 1           /* the game's headers in their native form */
#endif
#include <dirent.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <unistd.h>
#include "disk.h"
#include "portio.h"

static int failures, checks;

static void fail(const char *fmt, ...)
{
    va_list ap;
    failures++;
    if (failures > 40)
        return;
    va_start(ap, fmt);
    fputs("FAIL: ", stdout);
    vprintf(fmt, ap);
    fputc('\n', stdout);
    va_end(ap);
}

#define CHECK(cond, ...) do { checks++; if (!(cond)) fail(__VA_ARGS__); } while (0)

static uint32_t rd32(const void *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static uint16_t rd16(const void *p) { uint16_t v; memcpy(&v, p, 2); return v; }

static unsigned char *load(const char *path, long *size)
{
    FILE *f = fopen(path, "rb");
    unsigned char *b;
    if (!f)
        return 0;
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);
    b = malloc(*size + 16);
    if (fread(b, 1, *size, f) != (size_t)*size) {
        fclose(f);
        free(b);
        return 0;
    }
    fclose(f);
    return b;
}

/* a file in dir whatever its case */
static int find(const char *dir, const char *name, char *out, size_t n)
{
    DIR *d = opendir(dir);
    struct dirent *e;
    if (!d)
        return 0;
    while ((e = readdir(d))) {
        if (strcasecmp(e->d_name, name) == 0) {
            snprintf(out, n, "%s/%s", dir, e->d_name);
            closedir(d);
            return 1;
        }
    }
    closedir(d);
    return 0;
}

/* ---- SAVETREE.DAT ---------------------------------------------------------------------------- */

static const int qbn_disk_sizes[10] = {19, 94, 34, 20, 24, 16, 33, 14, 87, 8};
/* the in-memory sizes (tamriel.c's qbn_record_sizes natively) */
static const short native_qbn_sizes[10] = {
    sizeof(struct qbn_item), 94, 34, sizeof(struct qbn_person), sizeof(struct qbn_place), 16,
    sizeof(struct qbn_timer), sizeof(struct qbn_foe), sizeof(struct qbn_op), sizeof(struct qbn_state)
};
static int n_records, n_types[256], n_quests, n_quest_args, n_links, n_active, n_saves;
static unsigned char native[1 << 17], back[1 << 17];

/* a saved quest: each arg record (an offset) is a record start of its arg's section in the
   native layout, as quest_relink_after_load will make it a pointer */
static void check_saved_quest(const char *save, const unsigned char *disk, int dsize,
                              const unsigned char *nat, int nsize)
{
    const struct quest *dq = (const void *)disk, *nq = (const void *)nat;
    int s, i, j;
    n_quests++;
    for (s = 0; s < 10; s++) {
        CHECK(dq->section_counts[s] == nq->section_counts[s], "%s: quest section %d count", save, s);
        if (nq->section_counts[s] > 0)
            CHECK(nq->section_offsets[s] + nq->section_counts[s] * native_qbn_sizes[s] <= nsize,
                  "%s: quest section %d past the end", save, s);
    }
    if (nq->section_counts[8] <= 0)
        return;
    for (i = 0; i < nq->section_counts[8]; i++) {
        const struct qbn_op *op = (const void *)(nat + nq->section_offsets[8] + i * sizeof(struct qbn_op));
        const struct qbn_op_disk *dop = (const void *)(disk + dq->section_offsets[8] + i * sizeof(struct qbn_op_disk));
        CHECK(op->opcode == dop->opcode && op->arg_count == dop->arg_count, "%s: quest op %d", save, i);
        for (j = 0; j < op->arg_count && j < 5; j++) {
            const struct qbn_arg *a = &op->args[j];
            uintptr_t r = (uintptr_t)a->record;
            int sec = a->section, k;
            CHECK(a->value == dop->args[j].value && a->section == dop->args[j].section,
                  "%s: quest op %d arg %d fields", save, i, j);
            CHECK((uint32_t)(uintptr_t)a->object == dop->args[j].object, "%s: quest arg object id", save);
            if (r == 0)
                continue;
            n_quest_args++;
            if (sec < 0 || sec > 9) {
                fail("%s: quest arg section %d", save, sec);
                continue;
            }
            k = ((int)r - nq->section_offsets[sec]) / native_qbn_sizes[sec];
            CHECK(r >= (uintptr_t)nq->section_offsets[sec] && k < nq->section_counts[sec] &&
                  (int)r == nq->section_offsets[sec] + k * native_qbn_sizes[sec],
                  "%s: quest op %d arg %d record %lu not a record of section %d", save, i, j,
                  (unsigned long)r, sec);
            /* and the same record in the file */
            CHECK((int)dop->args[j].record == dq->section_offsets[sec] + k * qbn_disk_sizes[sec],
                  "%s: quest arg record index differs", save);
        }
    }
}

/* the native record's fields against the file's */
static void check_record(const char *save, const unsigned char *disk, int dlen, const unsigned char *nat, int nlen)
{
    const struct record_disk *dh = (const void *)disk;
    const struct record *nh = (const void *)nat;
    int type = dh->type, i;
    const unsigned char *dd = disk + 71, *nd = nat + RECORD_HEADER_SIZE;
    int dsize = dlen - 71, nsize = nlen - RECORD_HEADER_SIZE;

    CHECK(nh->type == dh->type && nh->x == dh->x && nh->y == dh->y && nh->z == dh->z &&
          nh->id == dh->id && nh->parent_id == dh->parent_id && nh->flags == dh->flags &&
          nh->image == dh->pad1B, "%s: record header fields (type %d)", save, type);
    CHECK((uint32_t)(uintptr_t)nh->twin == dh->twin && (uint32_t)(uintptr_t)nh->caster == dh->caster &&
          (uintptr_t)nh->parent == dh->parent, "%s: header slots (type %d)", save, type);
    if ((type == 3 || type == 18 || type == 44 || type == 45 || type == 46 || type == 34) && dsize >= 634) {
        const struct character_disk *dc = (const void *)dd;
        const struct character *nc = (const void *)nd;
        CHECK(memcmp(nc->name, dd, 32) == 0 && nc->level == dd[0x81] && nc->gold == (int)rd32(dd + 0x85) &&
              nc->health == (short)rd16(dd + 0x7C) && nc->faction_id == (short)rd16(dd + 0x227),
              "%s: character fields (type %d)", save, type);
        CHECK(memcmp(&nc->career, dd + 0x230, 74) == 0, "%s: character career", save);
        CHECK((uint32_t)(uintptr_t)nc->target == dc->target, "%s: character target", save);
        for (i = 0; i < 27; i++)
            CHECK((uintptr_t)nc->equipped[i] == dc->equipped[i], "%s: equipped[%d]", save, i);
        if ((type == 18 || type == 44 || type == 34) && dsize >= 659) {
            const struct monster_anim_disk *da = (const void *)(dd + 634);
            const struct monster_anim *na = (const void *)(nd + sizeof(struct character));
            CHECK(na->frame_count == da->frame_count && na->anim_record == da->anim_record &&
                  na->anim_current == da->anim_current, "%s: monster_anim fields", save);
            CHECK(da->anim_script_pos == 0 ? na->anim_script_pos == 0 :
                  (uint32_t)(na->anim_script_pos - na->anim_script) == da->anim_script_pos - da->anim_script,
                  "%s: monster_anim script difference", save);
        }
    }
    if ((type == 6 || type == 32) && dsize >= 58) {
        const struct model_instance_disk *dm = (const void *)dd;
        const struct model_instance *nm = (const void *)nd;
        CHECK(nm->x == dm->x && nm->y == dm->y && nm->z == dm->z && memcmp(nm->angles, dm->angles, 20) == 0,
              "%s: model_instance fields", save);
    }
    if (type == 14)
        check_saved_quest(save, dd, dsize, nd, nsize);
}

static void check_record_game_mode(const char *save, const unsigned char *disk, int dlen)
{
    int type = disk[0], nlen;
    persist_exact = 0;
    nlen = persist_record_from_disk(native, disk, dlen);
    persist_exact = 1;
    if ((type == 6 || type == 32) && dlen - 71 >= 58) {
        const struct model_instance *nm = (const void *)(native + RECORD_HEADER_SIZE);
        CHECK(nm->model == 0 && nm->lights == 0 && nm->matrix == 0, "%s: model handle not cleared", save);
    }
    if (type == 43 && dlen - 71 >= 17) {
        struct block *b = (void *)(native + RECORD_HEADER_SIZE);
        CHECK((char *)b->models == (char *)b + sizeof(struct block), "%s: block models not self-relative", save);
    }
    (void)nlen;
}

static void test_savetree(const char *save, const char *path, const char *outpath)
{
    long size;
    unsigned char *b = load(path, &size);
    long p;
    int tree, n, dlen, nlen, blen;
    if (!b) {
        fail("%s: no SAVETREE.DAT", save);
        return;
    }
    persist_exact = 1;
    p = 19;
    p += 4 + rd32(b + p);
    for (tree = 0; tree < 2; tree++) {
        for (;;) {
            if (p + 4 > size) {
                fail("%s: SAVETREE ends inside the records", save);
                free(b);
                return;
            }
            dlen = (int)rd32(b + p);
            p += 4;
            if (dlen == 0)
                break;
            nlen = persist_record_from_disk(native, b + p, dlen);
            CHECK(nlen == persist_record_from_disk(0, b + p, dlen), "%s: measured size differs", save);
            blen = persist_record_to_disk(back, native, nlen);
            CHECK(blen == dlen && memcmp(back, b + p, dlen) == 0,
                  "%s: record (type %d, %d bytes) does not round-trip (%d bytes back)", save, b[p], dlen, blen);
            check_record(save, b + p, dlen, native, nlen);
            check_record_game_mode(save, b + p, dlen);
            n_types[b[p]]++;
            n_records++;
            p += dlen;
        }
    }
    n = (int)rd32(b + p);
    p += 4;
    {
        int i;
        for (i = 0; i < n; i++) {
            struct link l;
            struct link_disk d;
            persist_link_from_disk(&l, (const void *)(b + p + 39 * i));
            persist_link_to_disk(&d, &l);
            CHECK(memcmp(&d, b + p + 39 * i, 39) == 0, "%s: link %d", save, i);
            CHECK((uint32_t)(uintptr_t)l.object == rd32(b + p + 39 * i + 0x23) &&
                  l.object_id == rd16(b + p + 39 * i), "%s: link fields", save);
            n_links++;
        }
    }
    p += 39 * n;
    n = (int)rd32(b + p);
    p += 4 + 4 * n;
    n_active += n;
    CHECK(p == size, "%s: SAVETREE has %ld bytes after the active links", save, size - p);

    /* through the game's hooks: read as load_game does, write as save_game does */
    {
        int in = open(path, O_RDONLY), out = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        unsigned char head[19];
        static unsigned char bld[1 << 16];
        static struct link links[1024];
        static iptr active[64];
        uint32_t w;
        int count, i, len;
        long osize;
        unsigned char *o;
        read(in, head, 19);
        write(out, head, 19);
        read(in, &w, 4);
        write(out, &w, 4);
        read(in, bld, w);
        write(out, bld, w);
        for (tree = 0; tree < 2; tree++) {
            for (;;) {
                read(in, &len, 4);              /* savetree_read_chunk */
                persist_savetree_read(in, (struct record *)native, &len);
                if (len == 0)
                    break;
                CHECK(persist_savetree_write(out, (struct record *)native, len) == len,
                      "%s: persist_savetree_write", save);
            }
            w = 0;
            write(out, &w, 4);
        }
        read(in, &count, 4);                    /* links_load */
        persist_read_links(in, links, count);
        write(out, &count, 4);                  /* links_save */
        persist_write_links(out, links, count);
        read(in, &count, 4);
        persist_read_indexes(in, active, count);
        write(out, &count, 4);
        for (i = 0; i < count; i++) {
            w = (uint32_t)active[i];
            write(out, &w, 4);
        }
        close(in);
        close(out);
        o = load(outpath, &osize);
        CHECK(o && osize == size && memcmp(o, b, size) == 0, "%s: SAVETREE through the hooks differs", save);
        free(o);
    }
    free(b);
}

/* ---- SAVEVARS.DAT ------------------------------------------------------------------------------ */

static const int savevars_items[] = {
    48, 64, 12, 1, -100, 20, 2, 512, 1, -71, 4, 4, 4, 4, 64, 2, 1, 13, 4, 1, 1, 1, 1, 4, 4, 4,
    4, 6, 8, 4, 4, 4, 4, 4, 1, 4960, 1, 4, 4, 4, 4, 2, 4, 4, 32, 32, 1, 4, 4, 4, 6, 4, 4, 4, 4,
    4, 4, 4, 4, 4
};
static int n_faces, n_factions;

static void test_savevars(const char *save, const char *path, const char *outpath)
{
    long size, p = 0;
    unsigned char *b = load(path, &size);
    size_t i;
    int n, k;
    if (!b) {
        fail("%s: no SAVEVARS.DAT", save);
        return;
    }
    persist_exact = 1;
    for (i = 0; i < sizeof savevars_items / sizeof savevars_items[0]; i++) {
        int item = savevars_items[i];
        if (item == -100) {
            for (k = 0; k < 10; k++) {
                struct quest_face f;
                struct quest_face_disk d;
                persist_quest_face_from_disk(&f, (const void *)(b + p + 10 * k));
                persist_quest_face_to_disk(&d, &f);
                CHECK(memcmp(&d, b + p + 10 * k, 10) == 0 && f.object_id == (int)rd32(b + p + 10 * k + 2),
                      "%s: quest face %d", save, k);
                n_faces++;
            }
            p += 100;
        } else if (item == -71) {
            static unsigned char h[256], d[71];
            const struct record *r = (const void *)h;
            persist_header_from_disk((struct record *)h, b + p);
            persist_header_to_disk(d, r);
            CHECK(memcmp(d, b + p, 71) == 0 && r->id == rd32(b + p + 0x1F) && r->x == (int)rd32(b + p + 7),
                  "%s: saved player header", save);
            p += 71;
        } else {
            p += item;
        }
    }
    n = (int)rd32(b + p);
    p += 4;
    for (k = 0; k < n; k++) {
        struct faction f;
        struct faction_disk d;
        persist_faction_from_disk(&f, (const void *)(b + p));
        persist_faction_to_disk(&d, &f);
        CHECK(memcmp(&d, b + p, 92) == 0 && f.id == rd16(b + p + 0x21) && memcmp(f.name, b + p + 3, 26) == 0,
              "%s: faction %d", save, k);
        /* faction_link_relations reads the low short of each slot */
        CHECK(*(short *)&f.allies[0] == (short)rd16(b + p + 0x38) &&
              *(short *)&f.enemies[2] == (short)rd16(b + p + 0x4C), "%s: faction %d relations", save, k);
        n_factions++;
        p += 92;
    }
    CHECK(p == size, "%s: SAVEVARS has %ld bytes after the factions", save, size - p);

    /* through the hooks, as savevars_read and savevars_write */
    {
        int in = open(path, O_RDONLY), out = open(outpath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        static unsigned char buf[8192], faces[10 * sizeof(struct quest_face)], header[256];
        struct faction f;
        int count;
        long osize;
        unsigned char *o;
        for (i = 0; i < sizeof savevars_items / sizeof savevars_items[0]; i++) {
            int item = savevars_items[i];
            if (item == -100) {
                persist_read_quest_faces(in, faces, 100);
                persist_write_quest_faces(out, faces, 100);
            } else if (item == -71) {
                persist_read_header(in, header, 71);
                persist_write_header(out, header, 71);
            } else {
                read(in, buf, item);
                write(out, buf, item);
            }
        }
        read(in, &count, 4);
        write(out, &count, 4);
        for (k = 0; k < count; k++) {
            persist_read_faction(in, &f);
            persist_write_faction(out, &f);
        }
        close(in);
        close(out);
        o = load(outpath, &osize);
        CHECK(o && osize == size && memcmp(o, b, size) == 0, "%s: SAVEVARS through the hooks differs", save);
        free(o);
    }
    free(b);
}

/* ---- BSA archives ------------------------------------------------------------------------------ */

struct bsa_rec { char name[16]; uint32_t id; long offset, size; };

static struct bsa_rec *bsa_dir(const unsigned char *b, long size, int *count)
{
    int n = (short)rd16(b), type = (short)rd16(b + 2), i;
    int es = type == 256 ? 18 : 8;
    long dir = size - (long)n * es, off = 4;
    struct bsa_rec *r = calloc(n, sizeof *r);
    for (i = 0; i < n; i++) {
        const unsigned char *e = b + dir + (long)i * es;
        if (type == 256) {
            memcpy(r[i].name, e, 14);
            r[i].size = rd32(e + 14);
        } else {
            r[i].id = rd32(e);
            r[i].size = rd32(e + 4);
        }
        r[i].offset = off;
        off += r[i].size;
    }
    *count = n;
    return r;
}

static const struct bsa_rec *bsa_find(const struct bsa_rec *r, int n, const char *name)
{
    int i;
    for (i = 0; i < n; i++)
        if (strcasecmp(r[i].name, name) == 0)
            return &r[i];
    return 0;
}

/* ---- MAPS.BSA ---------------------------------------------------------------------------------- */

static int n_locations;

static void check_location(const char *what, const unsigned char *rec)
{
    unsigned char nat[RECORD_HEADER_SIZE + sizeof(struct location)], d[119];
    const struct location_record_disk *dl = (const void *)rec;
    struct record *r = (struct record *)nat;
    struct location *l;
    persist_exact = 1;
    persist_header_from_disk(r, &dl->header);
    l = &r->data.location;
    persist_location_from_disk(l, &dl->location);
    CHECK(r->type == dl->header.type && r->x == dl->header.x && r->y == dl->header.y &&
          r->z == dl->header.z && r->id == dl->header.id && r->image == dl->header.pad1B,
          "%s: location header", what);
    CHECK(memcmp(l->name, dl->location.name, 32) == 0 && l->width == dl->location.width &&
          l->height == dl->location.height && l->kind == dl->location.kind &&
          l->building_count == dl->location.building_count &&
          l->object_counter == dl->location.object_counter, "%s: location fields", what);
    persist_header_to_disk(d, r);
    persist_location_to_disk((struct location_disk *)(d + 71), l);
    CHECK(memcmp(d, rec, 119) == 0, "%s: location does not round-trip", what);
    persist_exact = 0;
    persist_location_from_disk(l, &dl->location);
    CHECK(l->buildings == 0, "%s: location buildings not cleared", what);
    persist_exact = 1;
    n_locations++;
}

static void test_maps(const char *arena2)
{
    char path[1024], name[16], what[64];
    long size;
    unsigned char *b;
    struct bsa_rec *dir;
    int n, region;
    snprintf(path, sizeof path, "%s/MAPS.BSA", arena2);
    b = load(path, &size);
    if (!b) {
        fail("no %s", path);
        return;
    }
    dir = bsa_dir(b, size, &n);
    for (region = 0; region < 62; region++) {
        const struct bsa_rec *table, *items, *dungeons;
        int count, i;
        snprintf(name, sizeof name, "MAPTABLE.%03d", region);
        table = bsa_find(dir, n, name);
        snprintf(name, sizeof name, "MAPPITEM.%03d", region);
        items = bsa_find(dir, n, name);
        snprintf(name, sizeof name, "MAPDITEM.%03d", region);
        dungeons = bsa_find(dir, n, name);
        if (!table || !items)
            continue;
        count = (int)(table->size / 17);
        for (i = 0; i < count; i++) {
            const unsigned char *base = b + items->offset;
            long off = (long)count * 4 + rd32(base + 4 * i);
            int doors = (int)rd32(base + off);
            snprintf(what, sizeof what, "MAPPITEM.%03d #%d", region, i);
            check_location(what, base + off + 4 + 6 * doors);
        }
        if (dungeons) {
            const unsigned char *base = b + dungeons->offset;
            int dcount = (int)rd32(base);
            for (i = 0; i < dcount; i++) {
                long off = 4 + (long)dcount * 8 + rd32(base + 4 + 8 * i);
                int doors = (int)rd32(base + off);
                snprintf(what, sizeof what, "MAPDITEM.%03d #%d", region, i);
                check_location(what, base + off + 4 + 6 * doors);
            }
        }
    }
    free(dir);
    free(b);
}

/* ---- BLOCKS.BSA: RMB --------------------------------------------------------------------------- */

static int n_rmb, n_subrecords, n_block_models, n_misc_models;
static unsigned char rmb_buf[1 << 18], block_buf[1 << 17];

static void check_block_models(const char *what, const struct block_model *nm, const unsigned char *dm, int n)
{
    int i;
    for (i = 0; i < n; i++, dm += 66) {
        const struct block_model_disk *d = (const void *)dm;
        CHECK(nm[i].id == d->id && nm[i].variant == d->variant && nm[i].kind == d->kind &&
              nm[i].x == d->x && nm[i].y == d->y && nm[i].z == d->z && nm[i].yaw == d->yaw &&
              memcmp(nm[i].pad10, d->pad10, 20) == 0 && memcmp(nm[i].pad38, d->pad38, 10) == 0,
              "%s: block model %d", what, i);
        CHECK(nm[i].model == 0 && nm[i].lights == 0 && nm[i].matrix == 0, "%s: model handle not cleared", what);
    }
}

static void test_rmb(const char *what, const unsigned char *file, long size)
{
    struct rmb_file *rmb = (struct rmb_file *)rmb_buf;
    const struct rmb_file_disk *d = (const void *)file;
    int g = persist_rmb_growth(), i, k;
    iptr cursor;
    persist_exact = 0;
    memset(rmb_buf, 0xAA, size + g + 64);
    memcpy(rmb_buf + g, file, size);            /* archive_read_record's place */
    persist_rmb_from_disk(rmb, (iptr)(rmb_buf + g));
    CHECK(rmb->block_data_count == d->block_data_count && rmb->misc_model_count == d->misc_model_count &&
          rmb->misc_flat_count == d->misc_flat_count, "%s: rmb counts", what);
    CHECK(memcmp(rmb->positions, d->positions, sizeof d->positions) == 0 &&
          memcmp(rmb->buildings, d->buildings, sizeof d->buildings) == 0, "%s: rmb positions/buildings", what);
    CHECK(memcmp(rmb->block_data_sizes, d->block_data_sizes, sizeof d->block_data_sizes) == 0,
          "%s: rmb block_data_sizes", what);
    CHECK(memcmp(rmb->ground_tiles, d->ground_tiles, 256) == 0 && memcmp(rmb->ground_scenery, d->ground_scenery, 256) == 0 &&
          memcmp(rmb->automap, d->automap, 4096) == 0 && memcmp(rmb->name, d->name, 13 + 32 * 13) == 0,
          "%s: rmb ground/automap/names", what);
    CHECK(memcmp(rmb->data, file + 0x1A78, size - 0x1A78) == 0, "%s: rmb data", what);
    /* rmb_index_records */
    cursor = (iptr)rmb->data;
    for (i = 0; rmb->block_data_count > i; i++) {
        RMB_BLOCK_DATA(rmb, i) = (struct block *)cursor;
        cursor += rmb->block_data_sizes[i];
    }
    RMB_MISC_MODELS(rmb) = (struct block_model *)cursor;
    RMB_MISC_FLATS(rmb) = RMB_FLATS_AFTER_MODELS(RMB_MISC_MODELS(rmb), rmb->misc_model_count);
    CHECK((char *)RMB_MISC_FLATS(rmb) + 17 * rmb->misc_flat_count == (char *)rmb_buf + g + size,
          "%s: rmb misc objects do not end the file", what);
    /* each building's two subrecords (rmb_add_subrecord), as type-43 data */
    for (i = 0; i < rmb->block_data_count; i++) {
        const unsigned char *rec = (const void *)RMB_BLOCK_DATA(rmb, i);
        const unsigned char *end = rec + rmb->block_data_sizes[i];
        for (k = 0; k < 2 && rec < end; k++) {
            const struct block_disk *bd = (const void *)rec;
            struct block *nb = (struct block *)block_buf;
            int nsize = persist_block_from_disk(0, rec), nsize2;
            int dsize = persist_block_disk_size(rec);
            memset(block_buf, 0x55, nsize + 16);
            nsize2 = persist_block_from_disk(nb, rec);
            CHECK(nsize == nsize2 && nsize == (int)(sizeof(struct block) + bd->model_count * sizeof(struct block_model) +
                                                    bd->flat_count * 17 + bd->section3_count * 16),
                  "%s: block %d size", what, i);
            CHECK(nb->model_count == bd->model_count && nb->flat_count == bd->flat_count &&
                  nb->section3_count == bd->section3_count && nb->people_count == bd->people_count &&
                  nb->door_count == bd->door_count, "%s: block %d counts", what, i);
            CHECK((char *)nb->models == (char *)nb + sizeof(struct block) &&
                  (char *)nb->flats == (char *)(nb->models + nb->model_count) &&
                  (char *)nb->section3 == (char *)(nb->flats + nb->flat_count), "%s: block %d pointers", what, i);
            check_block_models(what, nb->models, rec + 17, bd->model_count);
            CHECK(memcmp(nb->flats, rec + 17 + 66 * bd->model_count, 17 * bd->flat_count + 16 * bd->section3_count) == 0,
                  "%s: block %d flats/section3", what, i);
            n_block_models += bd->model_count;
            n_subrecords++;
            rec += dsize + 17 * bd->people_count + 19 * bd->door_count;
            CHECK((iptr)rec == RMB_PEOPLE(bd, dsize) + 17 * bd->people_count + 19 * bd->door_count,
                  "%s: RMB_PEOPLE", what);
        }
        /* the two subrecords fill the building's bytes, or all but one (5549 of 9005 in
           BLOCKS.BSA end with a pad byte) */
        CHECK(end - rec == 0 || end - rec == 1, "%s: building %d subrecords leave %d of its %d bytes",
              what, i, (int)(end - rec), rmb->block_data_sizes[i]);
    }
    /* the misc models into type-56 data (town_block_create_misc_objects) */
    {
        struct block_model *m = (struct block_model *)block_buf;
        PORT_COPY_BLOCK_MODELS(m, RMB_MISC_MODELS(rmb), rmb->misc_model_count, 0, 0);
        check_block_models(what, m, (const unsigned char *)RMB_MISC_MODELS(rmb), rmb->misc_model_count);
        n_misc_models += rmb->misc_model_count;
    }
    n_rmb++;
}

static void test_blocks(const char *arena2)
{
    char path[1024];
    long size;
    unsigned char *b;
    struct bsa_rec *dir;
    int n, i;
    snprintf(path, sizeof path, "%s/BLOCKS.BSA", arena2);
    b = load(path, &size);
    if (!b) {
        fail("no %s", path);
        return;
    }
    dir = bsa_dir(b, size, &n);
    for (i = 0; i < n; i++) {
        size_t l = strlen(dir[i].name);
        if (l > 4 && strcasecmp(dir[i].name + l - 4, ".RMB") == 0) {
            if (dir[i].size + persist_rmb_growth() + 64 > (long)sizeof rmb_buf) {
                fail("%s: %ld bytes", dir[i].name, dir[i].size);
                continue;
            }
            test_rmb(dir[i].name, b + dir[i].offset, dir[i].size);
        }
    }
    free(dir);
    free(b);
}

/* ---- QBN files ------------------------------------------------------------------------------- */

static int n_qbn, n_qbn_records;

static void test_qbn(const char *name, const unsigned char *file, int size)
{
    const struct quest *dq = (const void *)file;
    const struct quest *nq = (const void *)native;
    int nsize, dsize, s, i, j;
    static const int dsz[10] = {19, 94, 34, 20, 24, 16, 33, 14, 87, 8};
    persist_exact = 0;
    nsize = persist_qbn_file_size((void *)file, size);
    if (nsize > (int)sizeof native) {
        fail("%s: %d bytes natively", name, nsize);
        return;
    }
    persist_qbn_file_copy(native, (void *)file, size);
    CHECK(memcmp(native, file, 0x24) == 0, "%s: quest header", name);
    for (s = 0; s < 10; s++) {
        const unsigned char *d = file + dq->section_offsets[s];
        const unsigned char *n = native + nq->section_offsets[s];
        if (dq->section_counts[s] <= 0)
            continue;
        for (i = 0; i < dq->section_counts[s]; i++, d += dsz[s], n += native_qbn_sizes[s]) {
            n_qbn_records++;
            switch (s) {
            case 0: {
                const struct qbn_item *x = (const void *)n;
                const struct qbn_item_disk *y = (const void *)d;
                CHECK(x->group == y->group && x->index == y->index && x->symbol == y->symbol &&
                      (uintptr_t)x->object == y->object && x->messages[1] == y->messages[1], "%s: item %d", name, i);
                break;
            }
            case 3: {
                const struct qbn_person *x = (const void *)n;
                const struct qbn_person_disk *y = (const void *)d;
                CHECK(x->flags == y->flags && x->faction_id == y->faction_id && x->symbol == y->symbol &&
                      x->messages[0] == y->messages[0], "%s: person %d", name, i);
                break;
            }
            case 4: {
                const struct qbn_place *x = (const void *)n;
                const struct qbn_place_disk *y = (const void *)d;
                CHECK(x->p1 == y->p1 && x->p2 == y->p2 && x->p3 == y->p3 && x->symbol == y->symbol &&
                      x->messages[1] == y->messages[1], "%s: place %d", name, i);
                break;
            }
            case 6: {
                const struct qbn_timer *x = (const void *)n;
                const struct qbn_timer_disk *y = (const void *)d;
                CHECK(x->flags == y->flags && x->minimum == y->minimum && x->maximum == y->maximum &&
                      (uintptr_t)x->link1 == y->link1 && (uintptr_t)x->link2 == y->link2 &&
                      x->state_hash == y->state_hash, "%s: timer %d", name, i);
                break;
            }
            case 7: {
                const struct qbn_foe *x = (const void *)n;
                const struct qbn_foe_disk *y = (const void *)d;
                CHECK(x->type == y->type && x->count == y->count && x->symbol == y->symbol, "%s: foe %d", name, i);
                break;
            }
            case 8: {
                const struct qbn_op *x = (const void *)n;
                const struct qbn_op_disk *y = (const void *)d;
                CHECK(x->opcode == y->opcode && x->message == y->message && x->last_minutes == y->last_minutes,
                      "%s: op %d", name, i);
                for (j = 0; j < 5; j++)
                    CHECK(x->args[j].negate == y->args[j].negate && (uintptr_t)x->args[j].record == y->args[j].record &&
                          x->args[j].section == y->args[j].section && x->args[j].value == y->args[j].value,
                          "%s: op %d arg %d", name, i, j);
                break;
            }
            default:
                CHECK(memcmp(n, d, dsz[s]) == 0, "%s: section %d record %d", name, s, i);
            }
        }
    }
    if (dq->text_offset) {
        const struct qbn_text_var_disk *y = (const void *)(file + dq->text_offset);
        const struct qbn_text_var *x = (const void *)(native + nq->text_offset);
        for (;;) {
            CHECK(memcmp(x->name, y->name, 20) == 0 && x->section == y->section && x->index == y->index,
                  "%s: text variable", name);
            if (!y->name[0])
                break;
            x++;
            y++;
            n_qbn_records++;
        }
    }
    dsize = persist_qbn_to_disk(back, native, nsize, PERSIST_QBN_FILE);
    CHECK(dsize == size && memcmp(back, file, size) == 0, "%s: QBN does not round-trip", name);
    CHECK(persist_qbn_to_disk(0, native, nsize, PERSIST_QBN_FILE) == size, "%s: QBN measure", name);
    n_qbn++;
}

static void test_qbns(const char *arena2)
{
    DIR *d = opendir(arena2);
    struct dirent *e;
    char path[1024];
    if (!d) {
        fail("no %s", arena2);
        return;
    }
    while ((e = readdir(d))) {
        size_t l = strlen(e->d_name);
        long size;
        unsigned char *b;
        if (l < 4 || strcasecmp(e->d_name + l - 4, ".QBN") != 0)
            continue;
        snprintf(path, sizeof path, "%s/%s", arena2, e->d_name);
        b = load(path, &size);
        if (!b) {
            fail("%s: unreadable", path);
            continue;
        }
        test_qbn(e->d_name, b, (int)size);
        free(b);
    }
    closedir(d);
}

/* ---- main ---------------------------------------------------------------------------------- */

int main(int argc, char **argv)
{
    const char *saves = "build/port/io/saves";
    char arena2[1024], tmp[1024];
    const char *home = getenv("HOME");
    int i, before;
    DIR *d;
    struct dirent *e;

    snprintf(arena2, sizeof arena2, "%s/dagger_comp/build/game/ARENA2", home ? home : ".");
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--saves") == 0 && i + 1 < argc)
            saves = argv[++i];
        else if (strcmp(argv[i], "--arena2") == 0 && i + 1 < argc)
            snprintf(arena2, sizeof arena2, "%s", argv[++i]);
        else {
            fprintf(stderr, "usage: savetest [--saves DIR] [--arena2 DIR]\n");
            return 2;
        }
    }
    snprintf(tmp, sizeof tmp, "%s/../savetest.out", saves);

    persist_anomalies = 0;
    d = opendir(saves);
    if (!d) {
        fail("no saves in %s", saves);
    } else {
        while ((e = readdir(d))) {
            char dir[1024], tree[1024], vars[1024], out[1100];
            struct stat st;
            if (e->d_name[0] == '.')
                continue;
            snprintf(dir, sizeof dir, "%s/%s", saves, e->d_name);
            if (stat(dir, &st) != 0 || !S_ISDIR(st.st_mode))
                continue;
            before = failures;
            if (!find(dir, "SAVETREE.DAT", tree, sizeof tree) || !find(dir, "SAVEVARS.DAT", vars, sizeof vars)) {
                fail("%s: no SAVETREE.DAT/SAVEVARS.DAT", e->d_name);
                continue;
            }
            snprintf(out, sizeof out, "%s.tree", tmp);
            test_savetree(e->d_name, tree, out);
            snprintf(out, sizeof out, "%s.vars", tmp);
            test_savevars(e->d_name, vars, out);
            printf("%-10s %s\n", e->d_name, failures == before ? "ok" : "FAILED");
            n_saves++;
        }
        closedir(d);
    }
    test_maps(arena2);
    test_blocks(arena2);
    test_qbns(arena2);

    printf("saves %d: records %d (characters %d, creatures %d, corpses %d, markers %d, quests %d "
           "with %d arg records, models/doors %d), links %d, active links %d; quest faces %d, factions %d\n",
           n_saves, n_records, n_types[3] + n_types[45] + n_types[46], n_types[18], n_types[44],
           n_types[34], n_quests, n_quest_args, n_types[6] + n_types[32], n_links, n_active, n_faces,
           n_factions);
    printf("MAPS.BSA: %d location records; BLOCKS.BSA: %d RMB blocks, %d subrecords, %d models, "
           "%d misc models; QBN: %d files, %d records\n",
           n_locations, n_rmb, n_subrecords, n_block_models, n_misc_models, n_qbn, n_qbn_records);
    CHECK(persist_anomalies == 0, "converter anomalies: %d (PERSIST_DEBUG=1 lists them)", persist_anomalies);
    printf("%s: %d checks, %d failed\n", failures ? "FAIL" : "PASS", checks, failures);
    return failures ? 1 : 0;
}
