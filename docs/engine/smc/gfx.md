# gfx: the unrolled copy-and-clear and the banked VESA blit (both dead)

Rules (PF, KEEP, UNR) are in index.md. Neither function runs in the game: it uses mode 13h and
the `rep movsd` copy-and-clear 143B80.

## Group GFX-COPYCLEAR-UNROLLED: `xn_gfx_copy_and_clear_unrolled` (143BB0)

**What.** It copies ecx bytes from esi (the back buffer) to edi and clears the source to
`text_shadow_colour` (12B504) replicated: `xn_colour_fill_table[colour]`.

**Mechanism.**
- Fixed unrolled code: 128 steps of `mov eax,[esi+4k]; mov [esi+4k],edx; mov [edi+4k],eax`
  (512 bytes per iteration), `ecx >> 9` iterations.
- The remainder `ecx & 1FFh` goes to 143B80.
- Only the dead benchmark at 143A5E (not in the function map) would select it.

```c
void xn_gfx_copy_and_clear_unrolled(u32 *src, u32 *dst, u32 n)
{
    u32 fill = xn_colour_fill_table[text_shadow_colour];
    u32 blocks = n >> 9, k;

    for (; blocks != 0; blocks--, src += 128, dst += 128)
        for (k = 0; k < 128; k++) {
            u32 v = src[k];
            src[k] = fill;
            dst[k] = v;
        }
    if (n & 0x1FF)
        xn_gfx_copy_and_clear(src, dst, n & 0x1FF);
}
```

**Group:** alone. 2 synthetic records and 7 differential tests. Its ABI is "unknown": eax, ecx,
edx, esi and edi are all outputs. The glue must return them: esi and edi advanced by the
blocks, eax = the last dword, edx = the fill, ecx = 0 (or whatever 143B80 leaves).

## Group GFX-VESA-BLIT: `xn_gfx_vesa_blit_rect_banked` (15FF53)

**What.** It copies ecx rows of ebx bytes from esi to the banked VESA window at (eax, edx),
switching banks across 64K boundaries.

**Mechanism.** Three self-patched operands (rule KEEP):

| Field | Operand | Value |
|---|---|---|
| 15FFCC | `mov ecx, DW` | `w >> 2` (dwords per row) |
| 15FFD3 | `mov ecx, B` | `w & 3` |
| 15FFDB | `add edi, SKIP` | `xn_gfx_width - w` |

```c
extern u32 xn_vesa_blit_dwords, xn_vesa_blit_bytes, xn_vesa_blit_skip;   /* KEEP */
...
XN_KEEP(xn_vesa_blit_skip, xn_gfx_width - w);
XN_KEEP(xn_vesa_blit_dwords, w >> 2);
XN_KEEP(xn_vesa_blit_bytes, w & 3);
```

The row loop is plain C around `memcpy`. A row that crosses the 0xB0000 window end is split,
with a bank switch through `xn_gfx_vesa_set_bank` (15FE05, a DPMI/BIOS call: port and
interrupt order must stay the same).

**Group:** alone. It has no records ("too long": it waits on the timer in the differential
tests). Convert it last, or leave it as asm. The `int 10h` bank switches inside must keep their
order relative to the copies.
