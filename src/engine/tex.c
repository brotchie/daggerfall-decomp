/* tex.c: XnGine's texture cache as readable C (xtex.h; see xngine.h and
   docs/xngine_readable.md). */
#include "xtex.h"
#include "xrender.h"

extern u8 xn_tex_cache_full;
extern s32 xn_tex_cur_archive, xn_tex_cur_record, xn_tex_cur_frame;
extern struct xn_tex_archive *xn_tex_archives[512];
extern u16 xn_tex_archive_use[512];     /* lookups this frame, per archive */
extern s32 xn_tex_record_offsets[512];  /* record * 20 */
extern char xn_tex_filename[];          /* "texture.NNN" */
extern char xn_tex_path[];
extern char cfg_last_path[];
extern s32 xn_tex_file_size;
extern u8 xn_tex_size_mask[256];        /* n - 1 for a power of two n, else 0xFF */
extern u32 xn_anim_ticks;
/* the heap */
extern struct xn_tex_block *xn_tex_heap_base;
extern struct xn_tex_block xn_tex_heap_head;   /* .next: the first block; marked used */
extern s32 xn_tex_heap_size, xn_tex_heap_free_bytes, xn_tex_heap_request;
/* the unpack buffer */
extern u8 *xn_tex_unpack_buffer;
extern struct xn_tex_unpack_strip xn_tex_unpack_strips[256];
extern u32 xn_tex_unpack_strip_count;
extern u32 xn_tex_unpack_used;          /* bytes of xn_tex_unpack_entries */
extern struct xn_tex_unpack_entry xn_tex_unpack_entries[256];
/* 'SET: ...' DOS strings ('$'-ended) */
extern char xn_tex_msg_out_of_memory[], xn_tex_msg_no_room[], xn_tex_msg_unpack_full[];

#define TEX_UNPACK_SIZE 0xC0000         /* the unpack buffer's bytes */
#define TEX_UNPACK_MAX  0x800           /* bytes of decoded-frame entries */

/* other groups' functions, through their asm entries */
extern void asm_xn_tmap_compile(void);
extern void asm_xn_tmap_pool_reset(void);
extern void asm_xn_tmap_pool_alloc(void);
extern void asm_xn_tmap_pool_free(void);
void asm_xn_tmap_rebase(void *copy, u8 *texels);
extern void asm_xn_dos_open(void);
s32 asm_xn_dos_read(s32 handle, void *buf, u32 n);
#pragma aux asm_xn_dos_read parm [ebx] [edx] [ecx] value [eax] modify exact [eax];
void asm_xn_dos_close(s32 handle);
#pragma aux asm_xn_dos_close parm [ebx] modify exact [];
extern void asm_xn_kbd_remove(void);
extern void asm_xn_joy_shutdown(void);
extern void asm_xn_gfx_restore_mode(void);
extern void asm_xn_mem_shutdown(void);
/* the game's C library (object 1) */
void *func_000A10A8(u32 size);          /* malloc */
void func_000A117E(void *p);            /* free */
s32 filelength(s32 handle);

/* A heap block's header, from its data */
static struct xn_tex_block *block_of(void *data)
{
    return (struct xn_tex_block *)((u8 *)data - sizeof(struct xn_tex_block));
}

/* The texture cache's fatal exit: the engine shut down, the message printed, the program ended
   (int 21h 4Ch) */
static void fatal(char *msg)
{
    xn_regs r;

    xn_call_asm(asm_xn_kbd_remove);
    xn_call_asm(asm_xn_joy_shutdown);
    xn_render_shutdown();
    xn_call_asm(asm_xn_gfx_restore_mode);
    xn_call_asm(asm_xn_mem_shutdown);
    r.eax = 0x0900;
    r.edx = (u32)msg;
    r.ecx = r.ebx = r.ebp = r.esi = r.edi = 0;
    xn_int21(&r);
    r.eax = 0x4C00;
    xn_int21(&r);
}

/* An RLE frame into dst (rows of 256 bytes): each row a run of (zero count, copy count, bytes)
   pairs until the width is used */
static void decode_rle(const struct xn_tex_frame *frame, u8 *dst)
{
    const u8 *src = frame->data;
    u32 rows = frame->height;

    do {                                /* dec; jne */
        u8 *p = dst;
        s32 left = frame->width;

        do {
            u32 n = *src++;

            left -= n;
            while (n-- != 0)
                *p++ = 0;
            n = *src++;
            left -= n;
            while (n-- != 0)
                *p++ = *src++;
        } while (left != 0);
        dst += 256;
    } while (--rows != 0);
}

void xn_tex_archive_set_translucent(s32 archive)
{
    struct xn_tex_archive *a = xn_tex_archives[archive];
    struct xn_tex_entry *e;
    u32 n;

    if (a == 0)
        return;
    e = a->entries;
    n = a->record_count;
    do {                                /* loop: a count of 0 would run 2^32 times */
        e->blend_index = 1;
        e++;
    } while (--n != 0);
}

struct xn_tex_entry *xn_tex_cache_lookup(s32 archive, s32 record, s32 *frame)
{
    struct xn_tex_archive *a;
    struct xn_tex_entry *e;
    struct xn_tex_image *image;
    s32 f = *frame;

    if (xn_tex_cache_full)
        return 0;
    xn_tex_cur_archive = archive;
    xn_tex_cur_record = record;
    xn_tex_cur_frame = f;
    a = xn_tex_archives[archive];
    xn_tex_archive_use[archive]++;
    if (a == 0) {
        if (!xn_tex_load_archive(archive))
            return 0;
        a = xn_tex_archives[archive];
    }
    block_of(a)->last_tick = XN_BIOS_TICKS;
    e = &XN_AT(struct xn_tex_entry, a->entries, xn_tex_record_offsets[record]);
    image = e->image;
    if (image != 0 && (s16)image->frame_count > 1) {
        u8 *pixels;

        if (f < 0) {                    /* by the animation clock */
            f = xn_udiv64(0, xn_anim_ticks, image->frame_time);
            /* only the quotient's low word is compared: a larger one stays unwrapped */
            if (image->frame_count <= (u16)f)
                f = xn_umod64(0, f, image->frame_count);
        }
        pixels = xn_tex_decode_frame((struct xn_tex_frame *)((u8 *)image + 0x1C +
                                                              image->frame_offsets[f]));
        if (!(image->flags & 0x1000) && !(image->flags & 0x100) && image->tmap != 0)
            asm_xn_tmap_rebase(image->tmap, pixels);
        image->data_offset = pixels - (u8 *)image;
    }
    e->current = image;
    *frame = f;
    return e;
}

void xn_tex_cache_lookup_r(xn_regs *r)
{
    s32 frame = r->ebx;
    struct xn_tex_entry *e = xn_tex_cache_lookup(r->eax, r->edx, &frame);

    r->eax = (u32)e;
    if (e != 0) {
        r->edx = (u32)e->current;
        r->ebx = frame;
    }
    XN_SETFLAG(r, XN_CF, e == 0);
}

struct xn_tex_image *xn_tex_cache_lookup_image(s32 archive, s32 record, s32 frame)
{
    struct xn_tex_archive *a;

    if (xn_tex_cache_full)
        return 0;
    xn_tex_cur_archive = archive;
    xn_tex_cur_record = record;
    xn_tex_cur_frame = frame;
    a = xn_tex_archives[archive];
    if (a == 0) {
        if (!xn_tex_load_archive(archive))
            return 0;
        a = xn_tex_archives[archive];
    }
    return XN_AT(struct xn_tex_entry, a->entries, xn_tex_record_offsets[record]).image;
}

/* the heap as one free block after its head */
static void heap_reset(struct xn_tex_block *b)
{
    b->size = xn_tex_heap_free_bytes;
    b->next = 0;
    b->prev = &xn_tex_heap_head;
    b->flags = 0;
}

void xn_tex_cache_flush(void)
{
    struct xn_tex_block *b = xn_tex_heap_base;

    xn_tex_heap_head.next = b;
    xn_tex_heap_head.flags |= 1;
    xn_tex_heap_free_bytes = xn_tex_heap_size - sizeof(struct xn_tex_block);
    heap_reset(b);
    xn_fill32(xn_tex_archives, 0, 512);
    xn_tex_cache_full = 0;
    xn_call_asm(asm_xn_tmap_pool_reset);
}

void xn_tex_cache_begin_frame(void)
{
    xn_fill32(xn_tex_archive_use, 0, 256);        /* 512 words */
    xn_tex_unpack_strip_count = 0;
    xn_tex_unpack_used = 0;
}

/* xn_tmap_compile (asm, rasteriser group): the entry's compiled mapper for the wrap mask
   (v mask << 8 | u mask); 0 when the pool is full */
static int tmap_compile(struct xn_tex_entry *e, u32 mask, void **copy)
{
    xn_regs r;

    r.eax = (u32)e;
    r.edx = mask;
    r.ecx = r.ebx = r.ebp = r.esi = r.edi = 0;
    xn_asmcall(asm_xn_tmap_compile, &r);
    *copy = (void *)r.eax;
    return (r.eflags & XN_CF) == 0;
}

/* xn_dos_open (asm, system group): the handle of a file opened for reading (a missing file
   ends the program) */
static s32 dos_open(char *path)
{
    xn_regs r;

    r.edx = (u32)path;
    r.eax = r.ecx = r.ebx = r.ebp = r.esi = r.edi = 0;
    xn_asmcall(asm_xn_dos_open, &r);
    return r.ebx;
}

int xn_tex_load_archive(s32 archive)
{
    u16 n = (u16)archive;
    char *d;
    const char *s;
    s32 handle;
    struct xn_tex_archive *a;
    struct xn_tex_entry *e;
    u32 left;

    /* "texture.NNN" */
    xn_tex_filename[10] = '0' + n % 10;
    n /= 10;
    xn_tex_filename[9] = '0' + n % 10;
    n /= 10;
    xn_tex_filename[8] = (u8)('0' + n);
    /* the path: the configured one, a backslash, the name (no 0 written after it: the buffer
       keeps the one it had) */
    d = xn_tex_path;
    for (s = cfg_last_path; *s != 0; s++)
        *d++ = *s;
    if (d[-1] != '\\')
        *d++ = '\\';
    for (s = xn_tex_filename; *s != 0; s++)
        *d++ = *s;

    handle = dos_open(xn_tex_path);
    xn_tex_file_size = filelength(handle);
    a = (struct xn_tex_archive *)xn_tex_heap_alloc(xn_tex_file_size);
    if (a == 0) {
        asm_xn_dos_close(handle);
        xn_tex_cache_full = 1;
        return 0;
    }
    xn_tex_archives[archive] = a;
    block_of(a)->slot = &xn_tex_archives[archive];
    asm_xn_dos_read(handle, a, xn_tex_file_size);
    asm_xn_dos_close(handle);

    e = a->entries;
    left = a->record_count;
    do {                                /* dec; jne */
        e->kind = 0;
        if (e->image != 0) {
            struct xn_tex_image *image;

            e->image = (struct xn_tex_image *)((u8 *)e->image + (u32)a);     /* relocated */
            image = e->image;
            if (!(image->flags & 0x1000)) {
                xn_tex_check_transparent(image);
                if (!(image->flags & 0x100)) {
                    u32 umask = xn_tex_size_mask[image->width];
                    u32 vmask = xn_tex_size_mask[image->height];
                    void *copy;
                    int ok;

                    image->wrap_mask = vmask << 24 | 0xFF0000 | umask << 8 | 0xFF;
                    ok = tmap_compile(e, vmask << 8 | umask, &copy);
                    image->tmap = (xn_routine)copy;
                    e->kind = 4;
                    if (!ok)
                        return 0;
                }
            }
        }
        e++;
    } while (--left != 0);
    return 1;
}

void xn_tex_load_archive_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_tex_load_archive(r->eax));
}

void xn_tex_check_transparent(struct xn_tex_image *image)
{
    const u8 *row = (u8 *)image + image->data_offset;
    s32 width = image->width;
    u32 rows = image->height;

    if ((s16)image->frame_count > 1) {  /* animated: its first frame */
        xn_tex_decode_to_big_buffer((struct xn_tex_frame *)((u8 *)image + 0x1C +
                                                             image->frame_offsets[0]));
        row = big_buffer;
    }
    image->flags &= ~0x100;
    do {                                /* dec; jne */
        s32 k;

        for (k = 0; k < width && row[k] != 0; k++)
            ;
        if (k < width - 1) {
            image->flags |= 0x100;
            return;
        }
        row += width + (u16)(0x100 - width);    /* 256 for a width up to 256 */
    } while (--rows != 0);
}

void xn_tex_cache_init(u32 size)
{
    struct xn_tex_block *b;

    xn_tex_heap_free_bytes = size;
    xn_tex_heap_size = size;
    xn_tex_heap_free_bytes -= sizeof(struct xn_tex_block);
    b = (struct xn_tex_block *)func_000A10A8(size);
    if (b == 0) {
        fatal(xn_tex_msg_out_of_memory);
        return;
    }
    xn_tex_heap_base = b;
    xn_tex_heap_head.next = b;
    xn_tex_heap_head.flags |= 1;
    heap_reset(b);
    xn_tex_unpack_buffer = (u8 *)func_000A10A8(TEX_UNPACK_SIZE);
    if (xn_tex_unpack_buffer == 0) {
        fatal(xn_tex_msg_out_of_memory);
        return;
    }
    ((u32 *)xn_tex_unpack_buffer)[1] = 0;
    xn_fill32(xn_tex_archives, 0, 512);
    xn_call_asm(asm_xn_tmap_pool_alloc);
    xn_tex_cache_begin_frame();
}

void xn_tex_cache_free(void)
{
    void *p;

    p = xn_tex_heap_base;
    xn_tex_heap_base = 0;
    func_000A117E(p);
    p = xn_tex_unpack_buffer;
    xn_tex_unpack_buffer = 0;
    func_000A117E(p);
    xn_call_asm(asm_xn_tmap_pool_free);
}

void *xn_tex_heap_alloc(s32 size)
{
    void *data;

    xn_tex_heap_sum_free();
    xn_tex_heap_request = size;
    if (size > xn_tex_heap_free_bytes && !xn_tex_heap_evict(size)) {
        fatal(xn_tex_msg_no_room);      /* unreachable: the eviction never fails */
        return 0;
    }
    data = xn_tex_heap_alloc_first_fit(size);
    if (data == 0)
        xn_tex_cache_full = 1;
    return data;
}

void xn_tex_heap_alloc_r(xn_regs *r)
{
    void *data = xn_tex_heap_alloc(r->eax);

    r->eax = (u32)data;
    XN_SETFLAG(r, XN_CF, data == 0);
}

int xn_tex_heap_evict(s32 size)
{
    while (xn_tex_heap_free_bytes < size) {
        struct xn_tex_block *b = xn_tex_heap_find_lru();

        if (b == 0)
            break;                      /* the asm's stc; clc: no failure reported */
        *b->slot = 0;                   /* the archive is no longer loaded */
        xn_tex_heap_free(b + 1);
    }
    return 1;
}

void xn_tex_heap_evict_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_tex_heap_evict(r->eax));
}

struct xn_tex_block *xn_tex_heap_find_lru(void)
{
    u32 oldest = XN_BIOS_TICKS;
    struct xn_tex_block *b = xn_tex_heap_head.next;
    struct xn_tex_block *found = 0;

    do {
        if ((b->flags & 1) && b->last_tick < oldest) {
            u32 archive = (u32)((u8 *)b->slot - (u8 *)xn_tex_archives) >> 2;

            /* the archive number used as a byte offset into the word counts (an original
               bug, kept: it tests the bytes of two neighbouring counts) */
            if (XN_AT(u16, xn_tex_archive_use, archive) == 0) {
                oldest = b->last_tick;
                found = b;
            }
        }
        b = b->next;
    } while (b != 0);
    return found;
}

void xn_tex_heap_find_lru_r(xn_regs *r)
{
    r->eax = (u32)xn_tex_heap_find_lru();
    r->edx = 0;                         /* the end of the walk */
}

void *xn_tex_heap_alloc_first_fit(s32 size)
{
    struct xn_tex_block *b;

    for (b = xn_tex_heap_head.next; b != 0; b = b->next) {
        struct xn_tex_block *rest;
        s32 used;

        if (b->flags & 1)
            continue;
        if (b->size == size) {          /* an exact fit */
            xn_tex_heap_free_bytes -= size;
            b->flags |= 1;
            return b + 1;
        }
        if (b->size <= size + (s32)sizeof(struct xn_tex_block))
            continue;
        /* split: the block keeps size + a header's bytes (22 more than asked), the rest
           becomes a free block after them */
        used = size + 2 * sizeof(struct xn_tex_block);
        rest = (struct xn_tex_block *)((u8 *)b + used);
        xn_tex_heap_free_bytes -= used;
        rest->next = b->next;
        b->next = rest;
        rest->prev = b;
        if (rest->next != 0)
            rest->next->prev = rest;
        rest->size = b->size - used;
        b->flags |= 1;
        rest->flags &= ~1;
        b->size = size + sizeof(struct xn_tex_block);
        return b + 1;
    }
    return 0;
}

void xn_tex_heap_free(void *data)
{
    struct xn_tex_block *b = block_of(data);
    struct xn_tex_block *next, *prev;

    b->flags &= ~1;
    xn_tex_heap_free_bytes += b->size + sizeof(struct xn_tex_block);
    next = b->next;                     /* (the last block reads a header at address 0) */
    if (!(next->flags & 1)) {           /* the next block joins this one */
        b->size += next->size + sizeof(struct xn_tex_block);
        b->next = next->next;
        if (b->next != 0)
            b->next->prev = b;
    }
    prev = b->prev;
    if (!(prev->flags & 1)) {           /* this block joins the previous one */
        prev->size += b->size + sizeof(struct xn_tex_block);
        prev->next = b->next;
        if (b->next != 0)
            b->next->prev = prev;
    }
}

u8 *xn_tex_decode_frame(const struct xn_tex_frame *frame)
{
    u8 *pixels;

    if (!xn_tex_unpack_alloc(frame->width, frame->height, &pixels))
        decode_rle(frame, pixels);
    return pixels;
}

int xn_tex_unpack_alloc(s32 w, s32 h, u8 **pixels)
{
    struct xn_tex_unpack_strip *s = xn_tex_unpack_strips;
    struct xn_tex_unpack_entry *entry;
    u32 key, n, off;
    u8 *dst;

    xn_tex_cur_frame &= 0xFFFF;
    key = ((u32)xn_tex_cur_archive << 7 | xn_tex_cur_record) << 16 | xn_tex_cur_frame;
    n = xn_tex_unpack_strip_count;
    if (n == 0) {                       /* the first strip, at the buffer's start */
        s->height = (u16)h;
        s->start = xn_tex_unpack_buffer;
        s->width_left = (u16)(0x100 - w);
        dst = xn_tex_unpack_buffer;
        xn_tex_unpack_strip_count = 1;
    } else {
        if (xn_tex_unpack_find(key, pixels))
            return 1;                   /* decoded already this frame */
        do {                            /* a strip with room */
            if (s->width_left >= (u16)w && s->height >= (u16)h)
                break;
            s++;
        } while (--n != 0);
        if (n != 0) {
            dst = s->start + (u16)(0x100 - s->width_left);
            s->width_left -= (u16)w;
        } else {                        /* a new strip under the last one */
            xn_tex_unpack_strip_count++;
            s->start = s[-1].start + (s[-1].height << 8);
            s->height = (u16)h;
            s->width_left = (u16)(0x100 - w);
            dst = s->start;
        }
    }
    off = xn_tex_unpack_used;
    xn_tex_unpack_used += 8;
    if (xn_tex_unpack_used >= TEX_UNPACK_MAX) {
        fatal(xn_tex_msg_unpack_full);
        return 0;
    }
    entry = &XN_AT(struct xn_tex_unpack_entry, xn_tex_unpack_entries, off);
    entry->key = key;
    entry->pixels = dst;
    if (dst >= xn_tex_unpack_buffer + TEX_UNPACK_SIZE) {
        fatal(xn_tex_msg_unpack_full);
        return 0;
    }
    *pixels = dst;
    return 0;
}

void xn_tex_unpack_alloc_r(xn_regs *r)
{
    u8 *pixels;
    int cached = xn_tex_unpack_alloc(r->eax, r->edx, &pixels);

    r->edi = (u32)pixels;
    XN_SETFLAG(r, XN_CF, cached);
}

int xn_tex_unpack_find(u32 key, u8 **pixels)
{
    u32 off = 0;

    do {                                /* entry 0 is looked at even when none is used */
        struct xn_tex_unpack_entry *e = &XN_AT(struct xn_tex_unpack_entry,
                                               xn_tex_unpack_entries, off);

        if (e->key == key) {
            *pixels = e->pixels;
            return 1;
        }
        off += 8;
    } while (off != xn_tex_unpack_used);
    return 0;
}

void xn_tex_unpack_find_r(xn_regs *r)
{
    u8 *pixels;
    int found = xn_tex_unpack_find(r->ebp, &pixels);

    if (found)
        r->edi = (u32)pixels;
    XN_SETFLAG(r, XN_CF, found);
}

void xn_tex_heap_sum_free(void)
{
    struct xn_tex_block *b;
    s32 bytes = 0;

    for (b = xn_tex_heap_head.next; b != 0; b = b->next)
        if (!(b->flags & 1))
            bytes += b->size;
    xn_tex_heap_free_bytes = bytes;
}

void xn_tex_decode_to_big_buffer(const struct xn_tex_frame *frame)
{
    decode_rle(frame, big_buffer);
}
