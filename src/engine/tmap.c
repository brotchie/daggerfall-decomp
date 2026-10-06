/* tmap.c: XnGine's texture mapper and its pool (xtmap.h). */
#include "xtmap.h"
#include "xrender.h"
#include "xdos.h"
#include "xkbd.h"
#include "xjoy.h"
#include "xgfx.h"
#include "xmem.h"

void xn_tmap_pool_alloc(void)
{
    u8 *block = func_000A10A8(XN_TMAP_SLOTS * XN_TMAP_SLOT_BYTES + 0x20);

    if (block != 0) {
        xn_tmap_pool_block = block;
        xn_tmap_pool = (u8 *)(((u32)block + 0x1F) & ~0x1Fu);
        xn_tmap_pool_count = 0;
        return;
    }
    /* out of memory: the engine shut down and the program ended (its exit code: the asm's
       AL there is its caller's, a leftover; 0 here) */
    xn_kbd_remove();
    xn_joy_shutdown();
    xn_render_shutdown();
    xn_gfx_restore_mode();
    xn_mem_shutdown();
    xn_dos_print(xn_tmap_msg_no_memory);
    xn_dos_exit(0);
}

void xn_tmap_pool_free(void)
{
    if (xn_tmap_pool_block != 0)
        func_000A117E(xn_tmap_pool_block);
}

void *xn_tmap_compile(const struct xn_tex_entry *entry, u32 mask)
{
    /* Quirk Q-TMAP-01: the count goes up before the test and is not put back */
    u32 slot = xn_tmap_pool_count++;

    (void)entry;
    (void)mask;
    if (xn_tmap_pool_count >= XN_TMAP_SLOTS) {
        xn_tex_cache_full = 1;
        return 0;
    }
    return xn_tmap_pool + slot * XN_TMAP_SLOT_BYTES;
}

void xn_tmap_rebase(void *handle, u8 *texels)
{
    (void)handle;
    (void)texels;
}

void xn_tmap_pool_reset(void)
{
    xn_tmap_pool_count = 0;
}

void xn_tmap_draw(u8 *pix, s32 n, u32 uv, u32 step, u32 shade, s32 shade_step,
                  const u8 *texels, u32 mask)
{
    u32 row = 0;
    s32 k;

    for (k = 0; k < n; k++) {
        /* u's integer byte is uv's top byte, v's the third */
        u32 t = (uv >> 24 | (uv & 0xFF00u)) & mask;

        if ((k & 1) == 0) {
            row = shade & ~0xFFu;               /* a pixel pair's shade row */
            shade += shade_step;
        }
        pix[k] = *(const u8 *)(row | texels[t]);
        uv += step;
    }
}
