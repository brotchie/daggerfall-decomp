/* tex_t.c: test shims of src/engine/tex.c (built only by tools/xn_rc.py; see
   src/engine_test/vec_t.c and docs/xngine_canonical.md). Each NAME_r maps the registers of
   NAME's asm entry (config/xngine_abi.csv) to the canonical C call and back. The asm passed
   the decoded frame's key from the lookup to the unpack allocator in globals: the shims of
   the decoder and the allocator read it there. */
#include "xtex.h"

extern s32 xn_tex_cur_archive, xn_tex_cur_record, xn_tex_cur_frame;

/* the key the asm's unpack allocator makes from the lookup's globals */
static u32 cur_key(void)
{
    return ((u32)xn_tex_cur_archive << 7 | xn_tex_cur_record) << 16 |
           (xn_tex_cur_frame & 0xFFFF);
}

void xn_tex_archive_set_translucent_r(xn_regs *r)
{
    xn_tex_archive_set_translucent(r->eax);
}

/* archive EAX, record EDX, frame EBX -> the entry in EAX (0 and CF when the cache failed),
   its current image in EDX */
void xn_tex_cache_lookup_r(xn_regs *r)
{
    struct xn_tex_entry *e = xn_tex_cache_lookup(r->eax, r->edx, r->ebx);

    r->eax = (u32)e;
    if (e != 0)
        r->edx = (u32)e->current;
    XN_SETFLAG(r, XN_CF, e == 0);
}

/* archive EAX, record EDX -> the image in EAX */
void xn_tex_cache_lookup_image_r(xn_regs *r)
{
    r->eax = (u32)xn_tex_cache_lookup_image(r->eax, r->edx);
}

void xn_tex_cache_flush_r(xn_regs *r)
{
    (void)r;
    xn_tex_cache_flush();
}

void xn_tex_cache_begin_frame_r(xn_regs *r)
{
    (void)r;
    xn_tex_cache_begin_frame();
}

void xn_tex_cache_init_r(xn_regs *r)
{
    xn_tex_cache_init(r->eax);
}

void xn_tex_cache_free_r(xn_regs *r)
{
    (void)r;
    xn_tex_cache_free();
}

/* the archive in EAX -> CF when it could not be loaded */
void xn_tex_load_archive_r(xn_regs *r)
{
    XN_SETFLAG(r, XN_CF, !xn_tex_load_archive(r->eax));
}

void xn_tex_check_transparent_r(xn_regs *r)
{
    xn_tex_check_transparent((struct xn_tex_image *)r->ebx);
}

/* the size in EAX -> the data in EAX, CF when none fits */
void xn_tex_heap_alloc_r(xn_regs *r)
{
    void *data = xn_tex_heap_alloc(r->eax);

    r->eax = (u32)data;
    XN_SETFLAG(r, XN_CF, data == 0);
}

/* the size in EAX -> CF clear (Q-TEX-01) */
void xn_tex_heap_evict_r(xn_regs *r)
{
    xn_tex_heap_evict(r->eax);
    XN_SETFLAG(r, XN_CF, 0);
}

/* -> the block in EAX, 0 in EDX (the end of the walk) */
void xn_tex_heap_find_lru_r(xn_regs *r)
{
    r->eax = (u32)xn_tex_heap_find_lru();
    r->edx = 0;
}

void xn_tex_heap_alloc_first_fit_r(xn_regs *r)
{
    r->eax = (u32)xn_tex_heap_alloc_first_fit(r->eax);
}

void xn_tex_heap_free_r(xn_regs *r)
{
    xn_tex_heap_free((void *)r->eax);
}

void xn_tex_heap_sum_free_r(xn_regs *r)
{
    (void)r;
    xn_tex_heap_sum_free();
}

/* the frame in ESI -> its pixels in EAX */
void xn_tex_decode_frame_r(xn_regs *r)
{
    r->eax = (u32)xn_tex_decode_frame((const struct xn_tex_frame *)r->esi, cur_key());
}

/* w EAX, h EDX -> the pixels in EDI, CF when this frame already has them */
void xn_tex_unpack_alloc_r(xn_regs *r)
{
    u8 *pixels;
    int cached = xn_tex_unpack_alloc(r->eax, r->edx, cur_key(), &pixels);

    r->edi = (u32)pixels;
    XN_SETFLAG(r, XN_CF, cached);
}

/* the key in EBP -> the pixels in EDI and CF when found */
void xn_tex_unpack_find_r(xn_regs *r)
{
    u8 *pixels;
    int found = xn_tex_unpack_find(r->ebp, &pixels);

    if (found)
        r->edi = (u32)pixels;
    XN_SETFLAG(r, XN_CF, found);
}

void xn_tex_decode_to_big_buffer_r(xn_regs *r)
{
    xn_tex_decode_to_big_buffer((const struct xn_tex_frame *)r->esi);
}
