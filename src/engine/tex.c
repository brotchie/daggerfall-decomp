/* tex.c: XnGine's texture cache (canonical C; the interface and the module's documentation
   are in xtex.h). */
#include "xtex.h"
#include "xtmap.h"
#include "xrender.h"
#include "xdos.h"
#include "xpc.h"
#include "xkbd.h"
#include "xjoy.h"
#include "xgfx.h"
#include "xmem.h"

extern u8 xn_tex_cache_full;
extern char cfg_last_path[];            /* the game's configured texture path */
extern u8 xn_tex_size_mask[256];        /* n - 1 for a power of two n, else 0FFh */
extern u32 xn_anim_ticks;               /* the animation clock (BIOS ticks) */

#define TEX_UNPACK_SIZE 0xC0000         /* the unpack buffer's bytes */
#define TEX_UNPACK_MAX  0x800           /* bytes of decoded-frame entries */

/* A heap block's header, from its data */
static struct xn_tex_block *block_of(void *data)
{
    return (struct xn_tex_block *)((u8 *)data - sizeof(struct xn_tex_block));
}

/* The texture cache's fatal exit: the engine shut down, msg printed, the game ended */
static void fatal(const char *msg)
{
    xn_kbd_remove();
    xn_joy_shutdown();
    xn_render_shutdown();
    xn_gfx_restore_mode();
    xn_mem_shutdown();
    xn_dos_print(msg);
    xn_dos_exit(0);
}

/* n dwords of 0 at p */
static void clear32(void *p, u32 n)
{
    u32 *d = (u32 *)p;

    while (n-- != 0)
        *d++ = 0;
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

/* ---- lookups ----------------------------------------------------------------------------- */

/* The archive's directory entry of a record */
static struct xn_tex_entry *entry_of(struct xn_tex_archive *a, s32 record)
{
    return &XN_AT(struct xn_tex_entry, a->entries, xn_tex_record_offsets[record]);
}

struct xn_tex_entry *xn_tex_cache_lookup(s32 archive, s32 record, s32 frame)
{
    struct xn_tex_archive *a;
    struct xn_tex_entry *e;
    struct xn_tex_image *image;

    if (xn_tex_cache_full)
        return 0;
    a = xn_tex_archives[archive];
    xn_tex_archive_use[archive]++;
    if (a == 0) {
        if (!xn_tex_load_archive(archive))
            return 0;
        a = xn_tex_archives[archive];
    }
    block_of(a)->last_tick = XN_BIOS_TICKS;
    e = entry_of(a, record);
    image = e->image;
    if (image != 0 && (s16)image->frame_count > 1) {
        /* the decoded frame's key: the frame as the caller gave it (Quirk Q-TEX-06: the
           clock's frames all go under -1, 0FFFFh) */
        u32 key = ((u32)archive << 7 | record) << 16 | (frame & 0xFFFF);
        u8 *pixels;

        if (frame < 0) {                /* by the animation clock */
            frame = xn_udiv64_or0(0, xn_anim_ticks, image->frame_time);
            /* only the quotient's low word is compared: a larger one stays unwrapped */
            if (image->frame_count <= (u16)frame)
                frame = xn_umod64_or0(0, frame, image->frame_count);
        }
        /* Quirk Q-TEX-05: a caller's frame is not checked against the image's frames */
        pixels = xn_tex_decode_frame((struct xn_tex_frame *)((u8 *)image + 0x1C +
                                                              image->frame_offsets[frame]), key);
        image->data_offset = pixels - (u8 *)image;
    }
    e->current = image;
    return e;
}

struct xn_tex_image *xn_tex_cache_lookup_image(s32 archive, s32 record)
{
    struct xn_tex_archive *a;

    if (xn_tex_cache_full)
        return 0;
    a = xn_tex_archives[archive];
    if (a == 0) {
        if (!xn_tex_load_archive(archive))
            return 0;
        a = xn_tex_archives[archive];
    }
    return entry_of(a, record)->image;
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
    do {                                /* a count of 0 would run 2^32 times */
        e->blend_index = 1;
        e++;
    } while (--n != 0);
}

/* ---- the cache --------------------------------------------------------------------------- */

/* the heap as one free block after its head */
static void heap_reset(struct xn_tex_block *b)
{
    xn_tex_heap_head.next = b;
    xn_tex_heap_head.flags |= 1;
    b->size = xn_tex_heap_free_bytes;
    b->next = 0;
    b->prev = &xn_tex_heap_head;
    b->flags = 0;
}

void xn_tex_cache_init(u32 size)
{
    struct xn_tex_block *b;

    xn_tex_heap_size = size;
    xn_tex_heap_free_bytes = size - sizeof(struct xn_tex_block);
    b = (struct xn_tex_block *)func_000A10A8(size);
    if (b == 0) {
        fatal(xn_tex_msg_out_of_memory);
        return;
    }
    xn_tex_heap_base = b;
    heap_reset(b);
    xn_tex_unpack_buffer = (u8 *)func_000A10A8(TEX_UNPACK_SIZE);
    if (xn_tex_unpack_buffer == 0) {
        fatal(xn_tex_msg_out_of_memory);
        return;
    }
    ((u32 *)xn_tex_unpack_buffer)[1] = 0;
    clear32(xn_tex_archives, 512);
    xn_tmap_pool_alloc();
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
    xn_tmap_pool_free();
}

void xn_tex_cache_flush(void)
{
    xn_tex_heap_free_bytes = xn_tex_heap_size - sizeof(struct xn_tex_block);
    heap_reset(xn_tex_heap_base);
    clear32(xn_tex_archives, 512);
    xn_tex_cache_full = 0;
    xn_tmap_pool_reset();
}

void xn_tex_cache_begin_frame(void)
{
    clear32(xn_tex_archive_use, 256);   /* 512 words */
    xn_tex_unpack_strip_count = 0;
    xn_tex_unpack_used = 0;
}

int xn_tex_load_archive(s32 archive)
{
    u16 n = (u16)archive;
    char name[12];
    char *d;
    const char *s;
    s32 handle, size;
    struct xn_tex_archive *a;
    struct xn_tex_entry *e;
    u32 left;

    /* "texture.NNN": the number's three digits (16-bit; a hundreds digit past 9 is not one) */
    for (s = "texture.", d = name; *s != 0; s++)
        *d++ = *s;
    name[10] = (char)('0' + n % 10);
    n /= 10;
    name[9] = (char)('0' + n % 10);
    n /= 10;
    name[8] = (char)('0' + (u8)n);
    name[11] = 0;
    /* the path: the configured one, a backslash (unless it ends in one), the name.
       Quirk Q-TEX-03: no terminator is written after the name */
    d = xn_tex_path;
    for (s = cfg_last_path; *s != 0; s++)
        *d++ = *s;
    if (d[-1] != '\\')
        *d++ = '\\';
    for (s = name; *s != 0; s++)
        *d++ = *s;

    handle = xn_dos_open(xn_tex_path);
    size = filelength(handle);
    a = (struct xn_tex_archive *)xn_tex_heap_alloc(size);
    if (a == 0) {
        xn_dos_close(handle);
        xn_tex_cache_full = 1;
        return 0;
    }
    xn_tex_archives[archive] = a;
    block_of(a)->slot = &xn_tex_archives[archive];
    xn_dos_read(handle, a, size);
    xn_dos_close(handle);

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
                    void *mapper;

                    image->wrap_mask = vmask << 24 | 0xFF0000 | umask << 8 | 0xFF;
                    mapper = xn_tmap_compile(e, vmask << 8 | umask);
                    image->tmap = (xn_routine)mapper;
                    e->kind = 4;
                    if (mapper == 0)
                        return 0;       /* the mapper pool is full (xn_tex_cache_full) */
                }
            }
        }
        e++;
    } while (--left != 0);
    return 1;
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
        if (k < width - 1) {            /* Quirk Q-TEX-04: the last column's 0 is missed */
            image->flags |= 0x100;
            return;
        }
        row += width + (u16)(0x100 - width);    /* 256 for a width up to 256 */
    } while (--rows != 0);
}

/* ---- the heap ------------------------------------------------------------------------------ */

void *xn_tex_heap_alloc(s32 size)
{
    void *data;

    xn_tex_heap_sum_free();
    if (size > xn_tex_heap_free_bytes)
        xn_tex_heap_evict(size);        /* Quirk Q-TEX-01: never fails, no fatal exit */
    data = xn_tex_heap_alloc_first_fit(size);
    if (data == 0)
        xn_tex_cache_full = 1;
    return data;
}

void xn_tex_heap_evict(s32 size)
{
    while (xn_tex_heap_free_bytes < size) {
        struct xn_tex_block *b = xn_tex_heap_find_lru();

        if (b == 0)
            return;
        *b->slot = 0;                   /* the archive is no longer loaded */
        xn_tex_heap_free(b + 1);
    }
}

struct xn_tex_block *xn_tex_heap_find_lru(void)
{
    u32 oldest = XN_BIOS_TICKS;
    struct xn_tex_block *b = xn_tex_heap_head.next;
    struct xn_tex_block *found = 0;

    do {
        if ((b->flags & 1) && b->last_tick < oldest) {
            u32 archive = (u32)((u8 *)b->slot - (u8 *)xn_tex_archives) >> 2;

            /* Quirk Q-TEX-02: the archive number used as a byte offset into the word counts */
            if (XN_AT(u16, xn_tex_archive_use, archive) == 0) {
                oldest = b->last_tick;
                found = b;
            }
        }
        b = b->next;
    } while (b != 0);
    return found;
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

void xn_tex_heap_sum_free(void)
{
    struct xn_tex_block *b;
    s32 bytes = 0;

    for (b = xn_tex_heap_head.next; b != 0; b = b->next)
        if (!(b->flags & 1))
            bytes += b->size;
    xn_tex_heap_free_bytes = bytes;
}

/* ---- animated frames ------------------------------------------------------------------------ */

u8 *xn_tex_decode_frame(const struct xn_tex_frame *frame, u32 key)
{
    u8 *pixels;

    if (!xn_tex_unpack_alloc(frame->width, frame->height, key, &pixels))
        decode_rle(frame, pixels);
    return pixels;
}

int xn_tex_unpack_alloc(s32 w, s32 h, u32 key, u8 **pixels)
{
    struct xn_tex_unpack_strip *s = xn_tex_unpack_strips;
    struct xn_tex_unpack_entry *entry;
    u32 n, off;
    u8 *dst;

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

void xn_tex_decode_to_big_buffer(const struct xn_tex_frame *frame)
{
    decode_rle(frame, big_buffer);
}
