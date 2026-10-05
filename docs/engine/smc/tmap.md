# tmap: the texture-mapper code generator (0x15C200-0x15C4A1)

XnGine compiles one copy of a 16-pixel texture-mapping loop per texture. The copy has the
texture's wrap mask and texel base baked into its instructions. The engine's own error message
calls these copies "Shaders". The copies live in a heap pool of 768 slots of 418 bytes. Only
`xn_span_tex_lit` (span.md) runs them. Code generation is part of the behaviour the records
see: the template's in-place patches are object-2 writes, and the copy is a heap write.

Rules (PF, RET, GEN ...) are in index.md.

## Group TMAP

| VA | Name | Role |
|---|---|---|
| 15C200 | `xn_tmap_pool_alloc` | allocates 0x4E620 bytes (768 x 418, 32-aligned) |
| 15C260 | `xn_tmap_pool_free` | frees it (no self-modifying code; listed for completeness) |
| 15C274 | `xn_tmap_compile` | patches the template in place with one texture's mask and base, copies it into the next slot |
| 15C2DC | `xn_tmap_rebase` | rewrites the texel base in a copy when the texture moves in the heap |
| 15C2F0 | `xn_tmap_pool_reset` | count = 0 (texture cache flush) |
| 15C300 | `xn_tmap_template` | the template: 8 two-pixel steps of 52 bytes, then `ret` (never run in place) |

The callers:
- `xn_tex_load_archive` (135EAB) calls `xn_tmap_compile` per texture record. The mask comes
  from `xn_tex_size_mask` (129EC0, built by `xn_render_init`). The copy's address goes into the
  image record's +0Ah.
- `xn_tex_cache_lookup` (135D00) calls `xn_tmap_rebase`.
- `xn_tex_cache_flush` (135E39) calls `xn_tmap_pool_reset`.
- `xn_poly_setup_textured` (15BB8C) copies the image's +0Ah into `poly->tmap` (+44h).
- `xn_span_tex_lit` (156A94) calls the copy through `xn_span_tex_lit_tmap` (156A40).

### 1. What the template computes

Entry registers:
- eax = ebx = packed (u << 16 | v) texture coordinate, 8.8 each;
- ecx = packed step;
- esi = shade (a shade-table pointer with an 8-bit fraction);
- ebp = shade step per 2 pixels;
- edi = destination - 1.

It writes `[edi+1]`..`[edi+16]`.

Per pixel k (k = 0..15), with A = the first pixel of a pair and B = the second:

```
t   = bswap(eax); t.ah = ebx.bh        ; t = v_frac<<24 | v_int<<16 | v_int<<8 | u_int
                                        ; (bytes of uv reversed, then ah = v_int)
A:  edx = esi                           ; the pair's shade row
t  &= MASK                              ; MASK is baked in: (h-1)<<8 | (w-1) for a w x h texture
ebx += ecx
dl  = [t + BASE]                        ; BASE is baked in: the texels
A:  esi += ebp
dl  = [edx]                             ; shade
eax = ebx
[edi+1+k] = dl
```

The texel offset is `(v_int & vmask) << 8 | (u_int & umask)`. This is the same 256-wide layout
as the other span routines; the `bswap` gets there from the swapped u/v order this path uses.

### 2. Mechanism

- **Template bytes** (15C300, 418 bytes copied: 416 of code, `C3`, and one `00` after it).
  Per 52-byte pair:

  | Offset in pair | Bytes | Instruction | Patch point |
  |---|---|---|---|
  | +00h | 0F C8 | `bswap eax` | pixel A starts here (a planted `ret` goes here) |
  | +02h | 8A E7 | `mov ah, bh` | |
  | +04h | 8B D6 | `mov edx, esi` | |
  | +06h | 25 imm32 | `and eax, MASK` | **+07h mask** |
  | +0Bh | 03 D9 | `add ebx, ecx` | |
  | +0Dh | 8A 90 disp32 | `mov dl, [eax+BASE]` | **+0Fh base** |
  | +13h | 03 F5 | `add esi, ebp` | |
  | +15h | 8A 12 | `mov dl, [edx]` | |
  | +17h | 8B C3 | `mov eax, ebx` | |
  | +19h | 88 57 0k | `mov [edi+k], dl` | |
  | +1Ch | 0F C8 ... | pixel B | **+21h mask**, **+29h base** |

  The ret offsets `xn_tmap_ret_offsets` (156A00, dwords) are 0, 28, 52, 80, ... = 52*(n/2) +
  28*(n odd).
- **Patch points:** 32 fields, `config/xngine_runtime_patches.csv`: 15C307 + 52j (mask A),
  15C30F + 52j (base A), 15C321 + 52j (mask B), 15C329 + 52j (base B), for j = 0..7. All mask
  fields get the same value, and so do all base fields.
- **Who writes:**
  - `xn_tmap_compile` writes them *in the template in object 2* through ebx (15C2A9-15C2B2),
    then copies the template with `rep movsb`;
  - `xn_tmap_rebase` writes the 16 base fields *in a copy*.
- **What calls the generated code:** `xn_span_tex_lit`, 16 pixels at a time
  (`call [xn_span_tex_lit_tmap]`). For the last run, it plants `C3` at
  `copy + xn_tmap_ret_offsets[n]` (n = 1..15), calls, and restores `0Fh`.

### 3. Readable C

The generator stays a generator (rule GEN): its writes are the behaviour.

```c
/* tmap.c */
#define XN_TMAP_SIZE    418     /* bytes copied per instance (416 code + ret + 1) */
#define XN_TMAP_SLOTS   768
#define XN_TMAP_PAIR    52      /* bytes per two-pixel step */
#define XN_TMAP_MASK_A  0x07    /* patch points within a pair */
#define XN_TMAP_BASE_A  0x0F
#define XN_TMAP_MASK_B  0x21
#define XN_TMAP_BASE_B  0x29

extern u8  xn_tmap_template[XN_TMAP_SIZE];      /* 15C300: code, patched in place */
extern u8 *xn_tmap_pool;                        /* 15C150 */
extern u32 xn_tmap_pool_count;                  /* 15C158 */
extern u8  xn_tex_cache_full;                   /* 132F58 */

static void tmap_put32(u8 *p, u32 v) { *(u32 *)p = v; }

/* Bake one texture's mask and texel base into the 8 pairs of a mapper (in place). */
static void tmap_patch(u8 *code, u32 mask, const u8 *texels, int with_mask)
{
    int j;
    for (j = 0; j < 8; j++, code += XN_TMAP_PAIR) {
        tmap_put32(code + XN_TMAP_BASE_A, (u32)texels);
        if (with_mask)
            tmap_put32(code + XN_TMAP_MASK_A, mask);
        tmap_put32(code + XN_TMAP_BASE_B, (u32)texels);
        if (with_mask)
            tmap_put32(code + XN_TMAP_MASK_B, mask);
    }
}

/* asm: eax = texture record entry, edx = mask; returns the copy in eax (0 and CF when full).
   The count is incremented before the test and never decremented: the 768th compile and
   every later one fail, and slot 767 is never used. */
u8 *xn_tmap_compile(const xn_tex_entry *entry, u32 mask)
{
    u32 slot = xn_tmap_pool_count++;
    const u8 *texels;
    u8 *copy;

    if (xn_tmap_pool_count >= XN_TMAP_SLOTS) {
        xn_tex_cache_full = 1;
        return 0;                                       /* + CF: the glue sets it */
    }
    copy = xn_tmap_pool + slot * XN_TMAP_SIZE;
    texels = entry->record + entry->record->data_offset;   /* [[eax+2]] + [[eax+2]+0Eh] */
    tmap_patch(xn_tmap_template, mask, texels, 1);      /* object-2 writes, as the asm */
    memcpy(copy, xn_tmap_template, XN_TMAP_SIZE);
    return copy;                                        /* + no CF */
}

/* asm: eax = copy, edx = new texel base */
void xn_tmap_rebase(u8 *copy, const u8 *texels)
{
    tmap_patch(copy, 0, texels, 0);
}

void xn_tmap_pool_reset(void)
{
    xn_tmap_pool_count = 0;
}
```

The **runner** has two honest forms. Both are exact against the records.

**Option A: run the generated code** (do this first). `xn_span_tex_lit` in C calls the copy
with the asm's registers through the infra glue, and plants and restores the `ret` as the asm
does:

```c
void xn_tmap_run(u8 *copy, u8 *dst, int n, u32 uv, u32 step, u32 shade, s32 shade_step)
{
    xn_regs r;
    u8 *stop = 0;

    if (n < 16) {
        stop = copy + xn_tmap_ret_offsets[n];
        *stop = 0xC3;                           /* planted ret (heap write, undone below) */
    }
    r.eax = r.ebx = uv;
    r.ecx = step;
    r.esi = shade;
    r.ebp = (u32)shade_step;
    r.edi = (u32)dst;                           /* destination - 1, as the asm passes it */
    xn_asmcall((void (*)(void))copy, &r);
    if (stop)
        *stop = 0x0F;                           /* the bswap's first byte */
}
```

**Option B: interpret the copy** (the native-port form; also exact while the generator still
writes the bytes). The copy is the texture's mapper record: its mask is at +07h and its base
at +0Fh. A C loop with the template's semantics draws the same pixels, and no code runs:

```c
#define XN_TMAP_MASK(copy)   (*(const u32 *)((copy) + XN_TMAP_MASK_A))
#define XN_TMAP_TEXELS(copy) (*(const u8 * const *)((copy) + XN_TMAP_BASE_A))

void xn_tmap_run(u8 *copy, u8 *dst, int n, u32 uv, u32 step, u32 shade, s32 shade_step)
{
    u32 mask = XN_TMAP_MASK(copy);
    const u8 *texels = XN_TMAP_TEXELS(copy);
    u32 row = 0;
    int k;

    for (k = 0; k < n; k++) {
        u32 t = xn_bswap(uv);
        t = (t & 0xFFFF00FFu) | (uv & 0xFF00u);     /* mov ah, bh */
        if ((k & 1) == 0) {
            row = shade;                            /* mov edx, esi */
            shade += shade_step;                    /* add esi, ebp */
        }
        uv += step;
        dst[1 + k] = *(const u8 *)((row & ~0xFFu) | texels[t & mask]);
    }
}
```

`xn_bswap` is one `bswap` pragma (add it to xngine.h). Both options leave the pool's bytes as
they were: option A plants and restores, and option B never touches them. The pool bytes are
still written by `xn_tmap_compile`, so the records match either way.

### 4. Conversion group and tests

- **Generator side:** 15C274, 15C2DC and 15C2F0 can be converted alone (rule PF/GEN: the
  same bytes go to the same addresses). 15C200 and 15C260 are plain C.
- **Runner side:** `xn_tmap_run` belongs to `xn_span_tex_lit` (span.md, SPAN-TEX-LIT).
  Option A needs nothing else; option B needs the generator to keep writing the template's
  fields.
- 15C300 is never routed. It is the template's bytes, `extern u8 xn_tmap_template[418]`. Its
  "records" are synthetic direct calls that faulted; ignore them.
- **Tests:**
  - 15C274: 26 records (each writes the template fields and one 418-byte slot);
  - 15C2DC: 2 records (synthetic);
  - 15C2F0: 2 records;
  - 135EAB `xn_tex_load_archive` (26) and 135D00 `xn_tex_cache_lookup` (29, 221 in all-C mode)
    run the generator in their records;
  - 156A94 (26) runs the copies. Its records carry the heap pages, so the copies are in the
    replayed memory.

### 5. Exactness traps

- The template fields in object 2 are written on every compile, so they hold the last
  texture's mask and base. A C generator that patched only the copy (cleaner) would miss 32
  object-2 writes per compile, and the records see them. Keep `tmap_patch(xn_tmap_template, ...)`.
- `rep movsb` copies 418 bytes including the `00` after the `ret`; copy all 418.
- The count overflow: `inc` before `cmp 300h`, with no undo, as in the sketch. CF and eax = 0 on
  failure; `xn_tex_cache_full` = 1 (a byte).
- `xn_tmap_rebase` writes only the base fields (16 dwords) of a copy, not the masks.
- Option A's planted `ret` is a write into the heap copy. Restore it with `0Fh`, not with a
  saved byte (the asm writes a constant). Every offset in `xn_tmap_ret_offsets[1..15]` is a
  pixel's first byte, `0Fh` (checked: `scratch/retcheck.py`).
- Flags: `xn_tmap_compile` returns CF. The ABI row says flags_out = CF, so it needs the infra's
  flags adapter stub.

### 6. Where readable C cannot match the records

Option B is readable and exact only because the generator still writes the pool. **Dropping
code generation** (storing `{mask, texels}` in the slot instead of 418 bytes of x86) is what a
native port wants, but it is not record-exact. Every compile's 418-byte slot write and the 32
template writes change. Proposed honest form:

1. Keep the byte-exact generator for the replay phase, with option A, then option B.
2. For the port, add a record-compare rule in `tools/xn_rc.py`: writes into the tmap pool
   (`[xn_tmap_pool, +0x4E620)`) and into `xn_tmap_template`'s 32 fields are "generated code".
   They compare equal when a decoder (a few lines of Python: read the mask at +07h and the base
   at +0Fh of each written slot) gives the same `{mask, texels}` as the C's slot record.
3. With that rule, `xn_tmap_compile` can become `slot->mask = mask; slot->texels = texels;`,
   and option B reads the struct.
