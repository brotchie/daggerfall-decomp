/* tmap.c: XnGine's texture-mapper code generator as readable C (xtmap.h; see xngine.h and
   docs/engine/smc/tmap.md). */
#include "xtmap.h"
#include "xnsmc.h"

void *func_000A10A8(u32 size);                  /* the game's malloc and free */
void func_000A117E(void *block);
#define xn_game_malloc  func_000A10A8
#define xn_game_free    func_000A117E

/* the engine's shutdown, then DOS's exit (other groups' functions, by their asm entries) */
void asm_xn_kbd_remove(void);
#pragma aux asm_xn_kbd_remove modify exact [eax];
void asm_xn_joy_shutdown(void);
#pragma aux asm_xn_joy_shutdown modify exact [eax];
void asm_xn_render_shutdown(void);
#pragma aux asm_xn_render_shutdown modify exact [eax];
void asm_xn_gfx_restore_mode(void);
#pragma aux asm_xn_gfx_restore_mode modify exact [eax];
void asm_xn_mem_shutdown(void);
#pragma aux asm_xn_mem_shutdown modify exact [eax edx];

void xn_tmap_pool_alloc(void)
{
    u8 *block = xn_game_malloc(XN_TMAP_SLOTS * sizeof(struct xn_tmap_copy) + 0x20);
    xn_regs r;

    if (block != 0) {
        xn_tmap_pool_block = block;
        xn_tmap_pool = (u8 *)(((u32)block + 0x1F) & ~0x1Fu);
        xn_tmap_pool_end = xn_tmap_pool + 0x10000;
        xn_tmap_pool_count = 0;
        return;
    }
    /* out of memory: never in the records (the game exits). The asm runs this with the
       caller's registers back (popad), so AL at the two int 21h is the caller's. */
    asm_xn_kbd_remove();
    asm_xn_joy_shutdown();
    asm_xn_render_shutdown();
    asm_xn_gfx_restore_mode();
    asm_xn_mem_shutdown();
    r.eax = 0x0900;
    r.edx = (u32)xn_tmap_msg_no_memory;
    xn_int21(&r);                               /* print the message */
    r.eax = 0x4C00;
    xn_int21(&r);                               /* exit */
}

/* the row's outputs are FS and GS (unchanged): glue, which keeps every register */
void xn_tmap_pool_alloc_r(xn_regs *r)
{
    (void)r;
    xn_tmap_pool_alloc();
}

void xn_tmap_pool_free(void)
{
    if (xn_tmap_pool_block != 0)
        xn_game_free(xn_tmap_pool_block);
}

void xn_tmap_pool_free_r(xn_regs *r)
{
    (void)r;
    xn_tmap_pool_free();
}

/* every fetch of a mapper gets the texel base, and with mask != 0 the wrap mask too */
static void tmap_patch(struct xn_tmap_copy *code, u8 *texels, const u32 *mask)
{
    struct xn_tmap_step *s;

    for (s = code->steps; s < code->steps + 8; s++) {
        s->texels_a = texels;
        s->texels_b = texels;
        if (mask) {
            s->wrap_mask_a = *mask;
            s->wrap_mask_b = *mask;
        }
    }
}

struct xn_tmap_copy *xn_tmap_compile(const struct xn_tex_entry *entry, u32 mask)
{
    u32 slot = xn_tmap_pool_count++;
    struct xn_tex_image *image;
    struct xn_tmap_copy *copy;

    if (xn_tmap_pool_count >= XN_TMAP_SLOTS) {
        xn_tex_cache_full = 1;
        return 0;
    }
    copy = (struct xn_tmap_copy *)(xn_tmap_pool + slot * sizeof(struct xn_tmap_copy));
    image = entry->image;
    /* the template in object 2 is patched in place (the records see it), then copied whole:
       416 bytes of code, its ret and the 00h after it */
    tmap_patch(&xn_tmap_template, (u8 *)image + image->data_offset, &mask);
    *copy = xn_tmap_template;
    return copy;
}

/* asm: eax = the entry, edx = the mask; returns eax and CF */
void xn_tmap_compile_r(xn_regs *r)
{
    struct xn_tmap_copy *copy = xn_tmap_compile((const struct xn_tex_entry *)r->eax, r->edx);

    r->eax = (u32)copy;
    XN_SETFLAG(r, XN_CF, copy == 0);
}

void xn_tmap_rebase(struct xn_tmap_copy *copy, u8 *texels)
{
    tmap_patch(copy, texels, 0);
}

void xn_tmap_pool_reset(void)
{
    xn_tmap_pool_count = 0;
}

void xn_tmap_run(const struct xn_tmap_copy *copy, u8 *pix, int n, u32 uv, u32 step, u32 shade,
                 s32 shade_step)
{
    const struct xn_tmap_step *s;
    u32 row = 0, t;
    int k;

    for (k = 0; k < n; k++) {
        s = &copy->steps[k >> 1];
        t = xn_bswap(uv);                       /* v_frac v_int u_frac u_int */
        t = (t & ~0xFF00u) | (uv & 0xFF00u);    /* mov ah, bh: v_int over u_frac */
        if ((k & 1) == 0) {
            row = shade & ~0xFFu;               /* a pixel pair's shade row */
            shade += shade_step;
            t &= s->wrap_mask_a;
            pix[k] = *(const u8 *)(row | s->texels_a[t]);
        } else {
            t &= s->wrap_mask_b;
            pix[k] = *(const u8 *)(row | s->texels_b[t]);
        }
        uv += step;
    }
}
