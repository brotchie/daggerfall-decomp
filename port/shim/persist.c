/* persist.c: the game's file layouts (port/include/disk.h) to and from the native structs
   (docs/port.md, phase 3; build/port/agents/persist/report.md §4, qbn/notes.md §4).

   Every struct is converted by a field map: stretches copied as they are, and pointer slots
   (u32 on disk, 8 bytes here) zero-extended on read, cut to their low 32 bits on write. The
   maps are checked once (each side contiguous, sizes as declared), so a change in
   include/records.h that moves a field shows up as an anomaly, not as a wrong save.

   include/portio.h hooks the fd-level functions at the end into the game's I/O. They only
   use the DOS file layer (port_read/port_write), so port/test/savetest.c links this file
   without the game. */
#ifndef DAGGER_PORT
#define DAGGER_PORT 1           /* the game's headers in their native form (port/include/port.h) */
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "port_host.h"
#include "disk.h"
#include "portio.h"

int persist_exact;
int persist_anomalies;

static void anomaly(const char *what, int a, int b)
{
    persist_anomalies++;
    if (getenv("PERSIST_DEBUG"))
        fprintf(stderr, "persist: %s (%d, %d)\n", what, a, b);
}

/* ---- slots ------------------------------------------------------------------------------- */

static uint32_t rd32(const void *p) { uint32_t v; memcpy(&v, p, 4); return v; }
static void wr32(void *p, uint32_t v) { memcpy(p, &v, 4); }
static uintptr_t rdptr(const void *p) { uintptr_t v; memcpy(&v, p, sizeof v); return v; }
static void wrptr(void *p, uintptr_t v) { memcpy(p, &v, sizeof v); }

/* ---- field maps -------------------------------------------------------------------------- */

enum { F_COPY, F_SLOT, F_DEAD };    /* bytes; a live slot (id, offset, type); a dead address */

struct field { short d, n, size, kind; };   /* size: of a copy (a slot is 4 / 8) */

struct layout {
    const char *name;
    int dsize, nsize, count;
    struct field f[48];
};

static void add(struct layout *l, int d, int n, int size, int kind)
{
    struct field *f = &l->f[l->count++];
    f->d = d; f->n = n; f->size = size; f->kind = kind;
    l->dsize = d + (kind == F_COPY ? size : 4);
    l->nsize = n + (kind == F_COPY ? size : (int)sizeof(void *));
}

#define COPY(l, d, n, size) add(l, d, n, size, F_COPY)
#define SLOT(l, d, n) add(l, d, n, 0, F_SLOT)
#define DEAD(l, d, n) add(l, d, n, 0, F_DEAD)

/* both sides contiguous from 0, and the sizes the file layout and the native struct say */
static void check(const struct layout *l, int dsize, int nsize)
{
    int d = 0, n = 0, i;
    for (i = 0; i < l->count; i++) {
        const struct field *f = &l->f[i];
        if (f->d != d || f->n != n) {
            fprintf(stderr, "persist: %s: field %d at %d/%d, expected %d/%d\n", l->name, i,
                    f->d, f->n, d, n);
            persist_anomalies++;
        }
        d = f->d + (f->kind == F_COPY ? f->size : 4);
        n = f->n + (f->kind == F_COPY ? f->size : (int)sizeof(void *));
    }
    if (d != dsize || n != nsize) {
        fprintf(stderr, "persist: %s: %d/%d bytes, expected %d/%d\n", l->name, d, n, dsize, nsize);
        persist_anomalies++;
    }
}

static void map_from_disk(const struct layout *l, void *dst, const void *src)
{
    const unsigned char *s = src;
    unsigned char *t = dst;
    int i;
    for (i = 0; i < l->count; i++) {
        const struct field *f = &l->f[i];
        if (f->kind == F_COPY)
            memcpy(t + f->n, s + f->d, f->size);
        else if (f->kind == F_SLOT || persist_exact)
            wrptr(t + f->n, rd32(s + f->d));
        else
            wrptr(t + f->n, 0);
    }
}

static void map_to_disk(const struct layout *l, void *dst, const void *src)
{
    const unsigned char *s = src;
    unsigned char *t = dst;
    int i;
    for (i = 0; i < l->count; i++) {
        const struct field *f = &l->f[i];
        if (f->kind == F_COPY)
            memcpy(t + f->d, s + f->n, f->size);
        else
            wr32(t + f->d, (uint32_t)rdptr(s + f->n));
    }
}

#define OFF(t, m) ((int)__builtin_offsetof(struct t, m))

static struct layout L_header, L_character, L_anim, L_instance, L_block, L_block_model,
    L_location, L_faction, L_link, L_face;
/* QBN: sections 0-9, and 10 the text variables */
static struct layout L_qbn[11];

static void init(void)
{
    static int done;
    struct layout *l;
    int i, s;
    if (done)
        return;
    done = 1;

    l = &L_header; l->name = "record";
    COPY(l, 0, 0, RECORD_DISK_PLAIN);
    SLOT(l, 0x2F, OFF(record, caster));
    SLOT(l, 0x33, OFF(record, twin));
    SLOT(l, 0x37, OFF(record, next));
    SLOT(l, 0x3B, OFF(record, prev));
    SLOT(l, 0x3F, OFF(record, children));
    SLOT(l, 0x43, OFF(record, parent));
    check(l, sizeof(struct record_disk), OFF(record, data));

    l = &L_character; l->name = "character";
    COPY(l, 0, 0, 0x70);
    SLOT(l, 0x70, OFF(character, target));
    COPY(l, 0x74, OFF(character, house), 0x16F - 0x74);
    for (i = 0; i < 27; i++)
        SLOT(l, 0x16F + 4 * i, OFF(character, equipped) + (int)sizeof(void *) * i);
    COPY(l, 0x1DB, OFF(character, table_flags), 634 - 0x1DB);
    check(l, sizeof(struct character_disk), sizeof(struct character));

    l = &L_anim; l->name = "monster_anim";
    COPY(l, 0, 0, 4);
    SLOT(l, 4, OFF(monster_anim, anim_script));
    SLOT(l, 8, OFF(monster_anim, anim_script_pos));
    COPY(l, 0x0C, OFF(monster_anim, frame_count), 25 - 0x0C);
    check(l, sizeof(struct monster_anim_disk), sizeof(struct monster_anim));

    /* XnGine's handle is 58 bytes; the game's struct model_instance names the first 56 */
    l = &L_instance; l->name = "model_instance";
    DEAD(l, 0, OFF(model_instance, model));
    DEAD(l, 4, OFF(model_instance, lights));
    DEAD(l, 8, OFF(model_instance, matrix));
    COPY(l, 0x0C, OFF(model_instance, angles), 58 - 0x0C);
    check(l, sizeof(struct model_instance_disk), OFF(model_instance, angles) + 58 - 0x0C);

    l = &L_block; l->name = "block";
    COPY(l, 0, 0, 5);
    SLOT(l, 5, OFF(block, models));
    SLOT(l, 9, OFF(block, flats));
    SLOT(l, 0x0D, OFF(block, section3));
    check(l, sizeof(struct block_disk), sizeof(struct block));

    l = &L_block_model; l->name = "block_model";
    COPY(l, 0, 0, 4);
    DEAD(l, 4, OFF(block_model, model));
    DEAD(l, 8, OFF(block_model, lights));
    DEAD(l, 0x0C, OFF(block_model, matrix));
    COPY(l, 0x10, OFF(block_model, pad10), 66 - 0x10);
    check(l, sizeof(struct block_model_disk), sizeof(struct block_model));

    l = &L_location; l->name = "location";
    COPY(l, 0, 0, 0x2B);
    DEAD(l, 0x2B, OFF(location, buildings));
    COPY(l, 0x2F, OFF(location, pad2F), 1);
    check(l, sizeof(struct location_disk), sizeof(struct location));

    l = &L_faction; l->name = "faction";
    COPY(l, 0, 0, 0x38);
    for (i = 0; i < 3; i++)
        SLOT(l, 0x38 + 4 * i, OFF(faction, allies) + (int)sizeof(void *) * i);
    for (i = 0; i < 3; i++)
        SLOT(l, 0x44 + 4 * i, OFF(faction, enemies) + (int)sizeof(void *) * i);
    DEAD(l, 0x50, OFF(faction, next));
    DEAD(l, 0x54, OFF(faction, child));
    DEAD(l, 0x58, OFF(faction, parent));
    check(l, sizeof(struct faction_disk), sizeof(struct faction));

    l = &L_link; l->name = "link";
    COPY(l, 0, 0, 0x23);
    SLOT(l, 0x23, OFF(link, object));
    check(l, sizeof(struct link_disk), sizeof(struct link));

    l = &L_face; l->name = "quest_face";
    COPY(l, 0, 0, 6);
    DEAD(l, 6, OFF(quest_face, image));
    check(l, sizeof(struct quest_face_disk), sizeof(struct quest_face));

    l = &L_qbn[0]; l->name = "qbn_item";
    COPY(l, 0, 0, 0x0B);
    SLOT(l, 0x0B, OFF(qbn_item, object));
    COPY(l, 0x0F, OFF(qbn_item, messages), 4);
    check(l, sizeof(struct qbn_item_disk), sizeof(struct qbn_item));

    l = &L_qbn[1]; l->name = "qbn section 1"; COPY(l, 0, 0, 94);
    l = &L_qbn[2]; l->name = "qbn section 2"; COPY(l, 0, 0, 34);

    l = &L_qbn[3]; l->name = "qbn_person";
    COPY(l, 0, 0, 0x0C);
    SLOT(l, 0x0C, OFF(qbn_person, object));
    COPY(l, 0x10, OFF(qbn_person, messages), 4);
    check(l, sizeof(struct qbn_person_disk), sizeof(struct qbn_person));

    l = &L_qbn[4]; l->name = "qbn_place";
    COPY(l, 0, 0, 0x10);
    SLOT(l, 0x10, OFF(qbn_place, object));
    COPY(l, 0x14, OFF(qbn_place, messages), 4);
    check(l, sizeof(struct qbn_place_disk), sizeof(struct qbn_place));

    l = &L_qbn[5]; l->name = "qbn section 5"; COPY(l, 0, 0, 16);

    l = &L_qbn[6]; l->name = "qbn_timer";
    COPY(l, 0, 0, 0x15);
    SLOT(l, 0x15, OFF(qbn_timer, link1));
    SLOT(l, 0x19, OFF(qbn_timer, link2));
    COPY(l, 0x1D, OFF(qbn_timer, state_hash), 4);
    check(l, sizeof(struct qbn_timer_disk), sizeof(struct qbn_timer));

    l = &L_qbn[7]; l->name = "qbn_foe";
    COPY(l, 0, 0, 0x0A);
    SLOT(l, 0x0A, OFF(qbn_foe, object));
    check(l, sizeof(struct qbn_foe_disk), sizeof(struct qbn_foe));

    l = &L_qbn[8]; l->name = "qbn_op";
    COPY(l, 0, 0, 6);
    for (i = 0; i < 5; i++) {
        int d = 6 + 15 * i, n = OFF(qbn_op, args) + (int)sizeof(struct qbn_arg) * i;
        COPY(l, d, n, 1);
        SLOT(l, d + 1, n + OFF(qbn_arg, record));
        COPY(l, d + 5, n + OFF(qbn_arg, section), 6);
        SLOT(l, d + 0x0B, n + OFF(qbn_arg, object));
    }
    COPY(l, 0x51, OFF(qbn_op, message), 6);
    check(l, sizeof(struct qbn_op_disk), sizeof(struct qbn_op));

    l = &L_qbn[9]; l->name = "qbn_state"; COPY(l, 0, 0, 8);
    check(l, 8, sizeof(struct qbn_state));

    l = &L_qbn[10]; l->name = "qbn_text_var";
    COPY(l, 0, 0, 0x17);
    SLOT(l, 0x17, OFF(qbn_text_var, record));
    check(l, sizeof(struct qbn_text_var_disk), sizeof(struct qbn_text_var));

    for (s = 0; s < 11; s++)
        if (!L_qbn[s].name)
            anomaly("qbn layout missing", s, 0);
}

/* ---- single structs ------------------------------------------------------------------------ */

void persist_header_from_disk(struct record *dst, const void *src)
{
    init();
    map_from_disk(&L_header, dst, src);
}

void persist_header_to_disk(void *dst, const struct record *src)
{
    init();
    map_to_disk(&L_header, dst, src);
}

void persist_character_from_disk(struct character *dst, const struct character_disk *src)
{
    init();
    map_from_disk(&L_character, dst, src);
}

void persist_character_to_disk(struct character_disk *dst, const struct character *src)
{
    init();
    map_to_disk(&L_character, dst, src);
}

/* the two script pointers matter only as a difference (monster_reload_anim_cb): pos is
   rebuilt from script, so that pos - script survives whatever the high halves were */
void persist_monster_anim_from_disk(struct monster_anim *dst, const struct monster_anim_disk *src)
{
    uint32_t script, pos;
    init();
    map_from_disk(&L_anim, dst, src);
    script = src->anim_script;
    pos = src->anim_script_pos;
    dst->anim_script = (char *)(uintptr_t)script;
    dst->anim_script_pos = pos ? dst->anim_script + (uint32_t)(pos - script) : 0;
}

void persist_monster_anim_to_disk(struct monster_anim_disk *dst, const struct monster_anim *src)
{
    init();
    map_to_disk(&L_anim, dst, src);
}

void persist_link_from_disk(struct link *dst, const struct link_disk *src)
{
    init();
    map_from_disk(&L_link, dst, src);
}

void persist_link_to_disk(struct link_disk *dst, const struct link *src)
{
    init();
    map_to_disk(&L_link, dst, src);
}

void persist_faction_from_disk(struct faction *dst, const struct faction_disk *src)
{
    init();
    map_from_disk(&L_faction, dst, src);
}

void persist_faction_to_disk(struct faction_disk *dst, const struct faction *src)
{
    init();
    map_to_disk(&L_faction, dst, src);
}

void persist_quest_face_from_disk(struct quest_face *dst, const struct quest_face_disk *src)
{
    init();
    map_from_disk(&L_face, dst, src);
}

void persist_quest_face_to_disk(struct quest_face_disk *dst, const struct quest_face *src)
{
    init();
    map_to_disk(&L_face, dst, src);
}

void persist_location_from_disk(struct location *dst, const struct location_disk *src)
{
    init();
    map_from_disk(&L_location, dst, src);
}

void persist_location_to_disk(struct location_disk *dst, const struct location *src)
{
    init();
    map_to_disk(&L_location, dst, src);
}

void persist_block_model_from_disk(struct block_model *dst, const struct block_model_disk *src)
{
    init();
    map_from_disk(&L_block_model, dst, src);
}

void persist_block_model_to_disk(struct block_model_disk *dst, const struct block_model *src)
{
    init();
    map_to_disk(&L_block_model, dst, src);
}

/* ---- QBN ----------------------------------------------------------------------------------
   The quest is a 60-byte header, then its sections in their file order (usually 0, 4, 3,
   (1, 5), 6, 9, 7, 8), then the text variables; empty sections share their neighbour's
   offset. The converted quest keeps that order and every byte between and after the
   sections (allocator slack), so a field read past a section lands in the same neighbour
   in both layouts. */

#define QBN_HEADER 60
enum { TO_NATIVE, TO_DISK };

struct qbn_region {
    int s, count;                   /* section (10 the text variables), records */
    int src, src_end, dst, dst_end;
};

struct qbn_map {
    int dir, nregions;
    struct qbn_region r[11];
};

static int qbn_src_size(int dir, int s) { return dir == TO_NATIVE ? L_qbn[s].dsize : L_qbn[s].nsize; }
static int qbn_dst_size(int dir, int s) { return dir == TO_NATIVE ? L_qbn[s].nsize : L_qbn[s].dsize; }

/* an offset inside a record of section s: its place in the other layout */
static int qbn_field(int dir, int s, int delta)
{
    const struct layout *l = &L_qbn[s];
    int i;
    for (i = 0; i < l->count; i++) {
        const struct field *f = &l->f[i];
        int from = dir == TO_NATIVE ? f->d : f->n, to = dir == TO_NATIVE ? f->n : f->d;
        int len = f->kind == F_COPY ? f->size : (dir == TO_NATIVE ? 4 : (int)sizeof(void *));
        if (delta >= from && delta < from + len)
            return to + (f->kind == F_COPY ? delta - from : 0);
    }
    return delta;
}

/* a source offset (a section's start, the text variables', an arg's record) in the
   converted quest */
static int qbn_pos(const struct qbn_map *m, int x, int *exact)
{
    int i, prev_src = QBN_HEADER, prev_dst = QBN_HEADER;
    if (exact)
        *exact = 0;
    if (x < QBN_HEADER)
        return x;
    for (i = 0; i < m->nregions; i++) {
        const struct qbn_region *r = &m->r[i];
        if (x < r->src)
            break;
        if (x < r->src_end) {
            int ss = qbn_src_size(m->dir, r->s), ds = qbn_dst_size(m->dir, r->s);
            int k = (x - r->src) / ss, delta = (x - r->src) % ss;
            if (exact)
                *exact = delta == 0;
            return r->dst + k * ds + qbn_field(m->dir, r->s, delta);
        }
        prev_src = r->src_end;
        prev_dst = r->dst_end;
    }
    return prev_dst + (x - prev_src);
}

static int qbn_convert(void *dst, const void *srcp, int size, int form, int dir)
{
    const unsigned char *src = srcp;
    unsigned char *out = dst;
    struct qbn_map m;
    const struct quest *q = srcp;   /* the header has no pointers: one layout */
    int i, j, s, pos_src, pos_dst, n;

    init();
    if (size < QBN_HEADER) {
        anomaly("qbn: shorter than its header", size, 0);
        if (out)
            memcpy(out, src, size > 0 ? size : 0);
        return size;
    }
    memset(&m, 0, sizeof m);
    m.dir = dir;
    for (s = 0; s < 10; s++) {
        int count = q->section_counts[s], off = q->section_offsets[s];
        if (count <= 0)
            continue;
        if (off < QBN_HEADER || off >= size) {
            anomaly("qbn: section outside the quest", s, off);
            continue;
        }
        if (off + count * qbn_src_size(dir, s) > size) {
            anomaly("qbn: section runs past the quest", s, count);
            count = (size - off) / qbn_src_size(dir, s);
        }
        m.r[m.nregions].s = s;
        m.r[m.nregions].count = count;
        m.r[m.nregions].src = off;
        m.r[m.nregions].src_end = off + count * qbn_src_size(dir, s);
        m.nregions++;
    }
    if (q->text_offset != 0) {
        int off = q->text_offset, ss = qbn_src_size(dir, 10), count = 0;
        if (off < QBN_HEADER || off >= size) {
            anomaly("qbn: text variables outside the quest", off, size);
        } else {
            /* through the entry whose name is empty */
            while (off + (count + 1) * ss <= size) {
                count++;
                if (src[off + (count - 1) * ss] == 0)
                    break;
            }
            if (count) {
                m.r[m.nregions].s = 10;
                m.r[m.nregions].count = count;
                m.r[m.nregions].src = off;
                m.r[m.nregions].src_end = off + count * ss;
                m.nregions++;
            }
        }
    }
    /* in file order */
    for (i = 1; i < m.nregions; i++)
        for (j = i; j > 0 && m.r[j].src < m.r[j - 1].src; j--) {
            struct qbn_region t = m.r[j];
            m.r[j] = m.r[j - 1];
            m.r[j - 1] = t;
        }
    for (i = 1; i < m.nregions; i++)
        if (m.r[i].src < m.r[i - 1].src_end) {
            anomaly("qbn: sections overlap", m.r[i - 1].s, m.r[i].s);
            memmove(&m.r[i], &m.r[i + 1], (m.nregions - i - 1) * sizeof m.r[0]);
            m.nregions--;
            i--;
        }

    /* lay out: the header, then each section after the bytes before it */
    if (out)
        memcpy(out, src, QBN_HEADER);
    pos_src = pos_dst = QBN_HEADER;
    for (i = 0; i < m.nregions; i++) {
        struct qbn_region *r = &m.r[i];
        int gap = r->src - pos_src, ss = qbn_src_size(dir, r->s), ds = qbn_dst_size(dir, r->s);
        if (out)
            memcpy(out + pos_dst, src + pos_src, gap);
        pos_dst += gap;
        r->dst = pos_dst;
        r->dst_end = pos_dst + r->count * ds;
        if (out)
            for (j = 0; j < r->count; j++) {
                if (dir == TO_NATIVE)
                    map_from_disk(&L_qbn[r->s], out + pos_dst + j * ds, src + r->src + j * ss);
                else
                    map_to_disk(&L_qbn[r->s], out + pos_dst + j * ds, src + r->src + j * ss);
            }
        pos_dst = r->dst_end;
        pos_src = r->src_end;
    }
    n = size - pos_src;
    if (out)
        memcpy(out + pos_dst, src + pos_src, n);
    pos_dst += n;
    if (!out)
        return pos_dst;

    /* the header's offsets in the new layout */
    {
        struct quest *h = (struct quest *)out;
        for (s = 0; s < 10; s++) {
            int off = q->section_offsets[s];
            h->section_offsets[s] = (short)(off > 0 ? qbn_pos(&m, off, 0) : off);
        }
        if (q->text_offset != 0)
            h->text_offset = qbn_pos(&m, q->text_offset, 0);
    }

    /* a saved quest's arg.record: an offset from the quest start into this layout
       (quest_unlink_for_save); the file's (section << 8 | index) stays as it is */
    if (form == PERSIST_QBN_SAVE)
        for (i = 0; i < m.nregions; i++) {
            const struct qbn_region *r = &m.r[i];
            if (r->s != 8)
                continue;
            for (j = 0; j < r->count; j++) {
                const unsigned char *sop = src + r->src + j * qbn_src_size(dir, 8);
                unsigned char *dop = out + r->dst + j * qbn_dst_size(dir, 8);
                int a, argc = *(const short *)(sop + 4);
                if (argc > 5)
                    argc = 5;
                for (a = 0; a < argc; a++) {
                    int sslot = dir == TO_NATIVE ? 6 + 15 * a + 1
                                                 : OFF(qbn_op, args) + (int)sizeof(struct qbn_arg) * a + 1;
                    int dslot = dir == TO_NATIVE ? OFF(qbn_op, args) + (int)sizeof(struct qbn_arg) * a + 1
                                                 : 6 + 15 * a + 1;
                    uint32_t v = dir == TO_NATIVE ? rd32(sop + sslot) : (uint32_t)rdptr(sop + sslot);
                    int exact, to;
                    if (v == 0)
                        continue;
                    if (v >= (uint32_t)size) {
                        anomaly("qbn: arg record outside the quest", (int)v, size);
                        continue;
                    }
                    to = qbn_pos(&m, (int)v, &exact);
                    if (!exact)
                        anomaly("qbn: arg record not at a record start", (int)v, j);
                    if (dir == TO_NATIVE)
                        wrptr(dop + dslot, (uint32_t)to);
                    else
                        wr32(dop + dslot, (uint32_t)to);
                }
            }
        }
    return pos_dst;
}

int persist_qbn_from_disk(void *dst, const void *src, int size, int form)
{
    return qbn_convert(dst, src, size, form, TO_NATIVE);
}

int persist_qbn_to_disk(void *dst, const void *src, int size, int form)
{
    return qbn_convert(dst, src, size, form, TO_DISK);
}

int persist_qbn_file_size(void *file, int n)
{
    return qbn_convert(0, file, n, PERSIST_QBN_FILE, TO_NATIVE);
}

void persist_qbn_file_copy(void *dst, void *file, int n)
{
    qbn_convert(dst, file, n, PERSIST_QBN_FILE, TO_NATIVE);
}

/* ---- RMB blocks ---------------------------------------------------------------------------- */

struct block *persist_rmb_block_data[32];
struct block_model *persist_rmb_misc_models;
struct block_flat *persist_rmb_misc_flats;

/* the native struct rmb_file is the file with its pointer slots widened: the same stretches
   in the same order, each moved down by the growth before it */
_Static_assert(OFF(rmb_file, block_data) == 0x5C3, "rmb_file: the header moved");
_Static_assert(OFF(rmb_file, ground_tiles) - 0x6CB == OFF(rmb_file, data) - 0x1A78,
               "rmb_file: the stretch after the misc slots is not one piece");

int persist_rmb_growth(void)
{
    return OFF(rmb_file, data) - 0x1A78;
}

/* the file was read growth bytes into the buffer (PORT_READ_RMB): move its stretches to
   their native places, lowest first (each moves down), and clear the pointer slots */
struct rmb_file *persist_rmb_from_disk(struct rmb_file *rmb, iptr loaded)
{
    unsigned char *base = (unsigned char *)rmb, *file = (unsigned char *)loaded;
    if (file != base + persist_rmb_growth())
        anomaly("rmb: not read where PORT_READ_RMB puts it", (int)(file - base), 0);
    /* from ground_tiles on, the file already lies where the native struct has it */
    memmove(base, file, 0x5C3);
    memmove(base + OFF(rmb_file, block_data_sizes), file + 0x643, 0x80);
    memset(rmb->block_data, 0, sizeof rmb->block_data);
    memset(&rmb->misc_models, 0, sizeof rmb->misc_models);
    memset(&rmb->misc_flats, 0, sizeof rmb->misc_flats);
    memset(persist_rmb_block_data, 0, sizeof persist_rmb_block_data);
    persist_rmb_misc_models = 0;
    persist_rmb_misc_flats = 0;
    return rmb;
}

void persist_block_models_from_disk(struct block_model *dst, void *src, int n)
{
    const unsigned char *s = src;
    int i;
    init();
    for (i = 0; i < n; i++)
        map_from_disk(&L_block_model, (char *)dst + i * L_block_model.nsize, s + i * 66);
}

int persist_block_disk_size(const void *record)
{
    const struct block_disk *b = record;
    return 17 + b->model_count * 66 + b->flat_count * 17 + b->section3_count * 16;
}

struct block_model *persist_block_relink(struct block *block)
{
    block->models = (struct block_model *)((char *)block + sizeof(struct block));
    block->flats = (struct block_flat *)(block->models + block->model_count);
    block->section3 = (struct block_section3 *)(block->flats + block->flat_count);
    return block->models;
}

/* a block's data both ways: the header, its models, then flats and section-3 records as
   they are; size bytes on the source side (the rest copied too) */
static int block_convert(void *dst, const void *srcp, int size, int dir)
{
    const unsigned char *src = srcp;
    unsigned char *out = dst;
    const struct layout *lh = &L_block, *lm = &L_block_model;
    int hs = dir == TO_NATIVE ? lh->dsize : lh->nsize, hd = dir == TO_NATIVE ? lh->nsize : lh->dsize;
    int ms = dir == TO_NATIVE ? lm->dsize : lm->nsize, md = dir == TO_NATIVE ? lm->nsize : lm->dsize;
    int n, i, rest;

    if (size < hs) {
        anomaly("block: shorter than its header", size, 0);
        if (out)
            memcpy(out, src, size);
        return size;
    }
    n = src[0];
    if (hs + n * ms > size) {
        anomaly("block: models run past the data", n, size);
        n = (size - hs) / ms;
    }
    rest = size - hs - n * ms;
    if (out) {
        if (dir == TO_NATIVE)
            map_from_disk(lh, out, src);
        else
            map_to_disk(lh, out, src);
        for (i = 0; i < n; i++) {
            if (dir == TO_NATIVE)
                map_from_disk(lm, out + hd + i * md, src + hs + i * ms);
            else
                map_to_disk(lm, out + hd + i * md, src + hs + i * ms);
        }
        memcpy(out + hd + n * md, src + hs + n * ms, rest);
        if (dir == TO_NATIVE && !persist_exact)
            persist_block_relink((struct block *)out);
    }
    return hd + n * md + rest;
}

int persist_block_from_disk(struct block *dst, const void *src)
{
    init();
    return block_convert(dst, src, persist_block_disk_size(src), TO_NATIVE);
}

/* ---- records ------------------------------------------------------------------------------- */

static int is_character(int type)
{
    return type == 3 || type == 18 || type == 44 || type == 45 || type == 46 || type == 34;
}

static int has_anim(int type)
{
    return type == 18 || type == 44 || type == 34;
}

/* a record's data, by type; size bytes on the source side; returns the other side's size */
static int data_convert(const struct record *header, void *dst, const void *srcp, int size, int dir)
{
    const unsigned char *src = srcp;
    unsigned char *out = dst;
    const struct layout *done[2];
    int ndone = 0, s = 0, d = 0, i, type = header->type;

    init();
    if (size < 0) {
        anomaly("record: negative data size", size, type);
        return size;
    }
    switch (type) {
    case 14:
        return qbn_convert(dst, src, size, PERSIST_QBN_SAVE, dir);
    case 43:
        return block_convert(dst, src, size, dir);
    case 56: {
        /* models (header +0x1B), then the flats as they are */
        const struct layout *lm = &L_block_model;
        int ms = dir == TO_NATIVE ? lm->dsize : lm->nsize, md = dir == TO_NATIVE ? lm->nsize : lm->dsize;
        int n = header->model_count;
        if (n * ms > size) {
            anomaly("type 56: models run past the data", n, size);
            n = size / ms;
        }
        if (out) {
            for (i = 0; i < n; i++) {
                if (dir == TO_NATIVE)
                    map_from_disk(lm, out + i * md, src + i * ms);
                else
                    map_to_disk(lm, out + i * md, src + i * ms);
            }
            memcpy(out + n * md, src + n * ms, size - n * ms);
        }
        return n * md + size - n * ms;
    }
    case 6:
    case 32:
        done[ndone++] = &L_instance;
        break;
    default:
        if (is_character(type)) {
            done[ndone++] = &L_character;
            if (has_anim(type))
                done[ndone++] = &L_anim;
        }
    }
    /* the structs while they fit, then the rest as it is */
    for (i = 0; i < ndone; i++) {
        const struct layout *l = done[i];
        int ls = dir == TO_NATIVE ? l->dsize : l->nsize, ld = dir == TO_NATIVE ? l->nsize : l->dsize;
        if (s + ls > size)
            break;
        if (out) {
            if (l == &L_anim) {
                if (dir == TO_NATIVE)
                    persist_monster_anim_from_disk((struct monster_anim *)(out + d),
                                                   (const struct monster_anim_disk *)(src + s));
                else
                    persist_monster_anim_to_disk((struct monster_anim_disk *)(out + d),
                                                 (const struct monster_anim *)(src + s));
            } else if (dir == TO_NATIVE) {
                map_from_disk(l, out + d, src + s);
            } else {
                map_to_disk(l, out + d, src + s);
            }
        }
        s += ls;
        d += ld;
    }
    if (out)
        memcpy(out + d, src + s, size - s);
    return d + size - s;
}

int persist_data_from_disk(const struct record *header, void *dst, const void *src, int size)
{
    return data_convert(header, dst, src, size, TO_NATIVE);
}

int persist_data_to_disk(const struct record *header, void *dst, const void *src, int size)
{
    return data_convert(header, dst, src, size, TO_DISK);
}

int persist_record_from_disk(void *dst, const void *src, int disk_len)
{
    union { struct record r; unsigned char b[256]; } *h, local;
    int n;
    init();
    if (disk_len < L_header.dsize) {
        anomaly("record: shorter than its header", disk_len, 0);
        if (dst)
            memcpy(dst, src, disk_len);
        return disk_len;
    }
    h = dst ? (void *)dst : (void *)&local;
    map_from_disk(&L_header, h, src);
    n = data_convert(&h->r, dst ? (char *)dst + L_header.nsize : 0,
                     (const char *)src + L_header.dsize, disk_len - L_header.dsize, TO_NATIVE);
    return L_header.nsize + n;
}

int persist_record_to_disk(void *dst, const void *src, int native_len)
{
    int n;
    init();
    if (native_len < L_header.nsize) {
        anomaly("record: shorter than its native header", native_len, 0);
        if (dst)
            memcpy(dst, src, native_len);
        return native_len;
    }
    if (dst)
        map_to_disk(&L_header, dst, src);
    n = data_convert((const struct record *)src, dst ? (char *)dst + L_header.dsize : 0,
                     (const char *)src + L_header.nsize, native_len - L_header.nsize, TO_DISK);
    return L_header.dsize + n;
}

/* ---- the game's I/O (include/portio.h) ---------------------------------------------------- */

static void *scratch(int n)
{
    static void *p;
    static int size;
    if (n > size) {
        free(p);
        size = n + 4096;
        p = malloc(size);
        if (!p)
            port_fatal("persist: out of memory (%d bytes)", size);
    }
    return p;
}

static int read_all(int fd, void *p, int n)
{
    return n > 0 ? port_read(fd, p, (unsigned)n) : 0;
}

int persist_savetree_write(int fd, struct record *buf, int len)
{
    unsigned char *disk = scratch(len + 4);
    int dlen = persist_record_to_disk(disk + 4, buf, len);
    wr32(disk, (uint32_t)dlen);
    return port_write(fd, disk, (unsigned)(dlen + 4)) == dlen + 4 ? len : -1;
}

int persist_savetree_read(int fd, struct record *buf, int *size)
{
    int dlen = *size, got;
    unsigned char *disk;
    if (dlen <= 0)
        return 0;
    disk = scratch(dlen);
    got = read_all(fd, disk, dlen);
    if (got != dlen) {
        anomaly("savetree: short record", got, dlen);
        if (got < 0)
            got = 0;
        memset(disk + got, 0, dlen - got);
    }
    *size = persist_record_from_disk(buf, disk, dlen);
    return got;
}

int persist_read_quest_faces(int fd, void *faces, int n)
{
    unsigned char *disk = scratch(n);
    int got = read_all(fd, disk, n), i;
    for (i = 0; i < n / 10; i++)
        persist_quest_face_from_disk((struct quest_face *)faces + i,
                                     (const struct quest_face_disk *)(disk + 10 * i));
    return got;
}

int persist_write_quest_faces(int fd, void *faces, int n)
{
    unsigned char *disk = scratch(n);
    int i;
    for (i = 0; i < n / 10; i++)
        persist_quest_face_to_disk((struct quest_face_disk *)(disk + 10 * i),
                                   (const struct quest_face *)faces + i);
    return port_write(fd, disk, (unsigned)n);
}

int persist_read_header(int fd, void *header, int n)
{
    unsigned char *disk = scratch(n);
    int got = read_all(fd, disk, n);
    if (n != (int)sizeof(struct record_disk))
        anomaly("header: not 71 bytes", n, 0);
    persist_header_from_disk(header, disk);
    return got;
}

int persist_write_header(int fd, void *header, int n)
{
    unsigned char *disk = scratch(n);
    persist_header_to_disk(disk, header);
    return port_write(fd, disk, (unsigned)n);
}

int persist_read_faction(int fd, struct faction *faction)
{
    struct faction_disk disk;
    int got = read_all(fd, &disk, sizeof disk);
    persist_faction_from_disk(faction, &disk);
    return got;
}

int persist_write_faction(int fd, struct faction *faction)
{
    struct faction_disk disk;
    persist_faction_to_disk(&disk, faction);
    return port_write(fd, &disk, sizeof disk);
}

int persist_read_links(int fd, struct link *links, int n)
{
    unsigned char *disk;
    int got, i;
    if (n <= 0)
        return 0;
    disk = scratch(n * 39);
    got = read_all(fd, disk, n * 39);
    for (i = 0; i < n; i++)
        persist_link_from_disk(links + i, (const struct link_disk *)(disk + 39 * i));
    return got;
}

int persist_write_links(int fd, struct link *links, int n)
{
    unsigned char *disk;
    int i;
    if (n <= 0)
        return 0;
    disk = scratch(n * 39);
    for (i = 0; i < n; i++)
        persist_link_to_disk((struct link_disk *)(disk + 39 * i), links + i);
    return port_write(fd, disk, (unsigned)(n * 39));
}

int persist_read_indexes(int fd, iptr *slots, int n)
{
    unsigned char *disk;
    int got, i;
    if (n <= 0)
        return 0;
    disk = scratch(n * 4);
    got = read_all(fd, disk, n * 4);
    for (i = 0; i < n; i++)
        slots[i] = (iptr)rd32(disk + 4 * i);
    return got;
}

int persist_read_location(int fd, struct record *object, int n)
{
    struct location_record_disk disk;
    int got;
    if (n != (int)sizeof disk)
        anomaly("location: not 119 bytes", n, 0);
    memset(&disk, 0, sizeof disk);
    got = read_all(fd, &disk, n < (int)sizeof disk ? n : (int)sizeof disk);
    persist_header_from_disk(object, &disk.header);
    persist_location_from_disk(&object->data.location, &disk.location);
    return got;
}
