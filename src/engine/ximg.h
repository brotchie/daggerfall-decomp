/* ximg.h: XnGine's image records and decoders (src/engine/img.c; see xngine.h): IMG and CIF
   records, the row-compressed images of the inventory, the RLE rows of CEL files and weapon
   CIFs.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XIMG_H
#define XIMG_H

#include "xngine.h"

/* An IMG file, or one image of a CIF file (the game's struct image, include/structs.h): a
   12-byte header, then width x height pixels; a CIF's next image follows data_size bytes of
   pixels. */
typedef struct xn_img {
    u16 x;                  /* +00 where it is drawn */
    u16 y;                  /* +02 */
    u16 width;              /* +04 */
    u16 height;             /* +06 */
    u16 compression;        /* +08 */
    u16 data_size;          /* +0A bytes of pixels */
    u8 pixels[1];           /* +0C */
} xn_img;

/* A group of RLE frames in a weapon CIF (after its first, plain image): the frames' rows are
   runs (a byte < 80h: n+1 literal pixels follow; else (n & 7Fh)+1 copies of the next byte) */
typedef struct xn_rle_group {
    u16 width;              /* +00 */
    u16 height;             /* +02 */
    u16 unknown_04;         /* +04 */
    u16 x;                  /* +06 where it is drawn */
    u16 y;                  /* +08 */
    u16 unknown_0a;         /* +0A */
    u16 frame_offset[31];   /* +0C each frame's runs, from the group's start */
    u16 size;               /* +4A bytes from this group to the next */
} xn_rle_group;

/* A CEL animation (MAGE.CEL, ROGUE.CEL): a 14-byte header, then per frame a dword per row:
   the row's pixels from the file's start, bit 31 set for an RLE row (xn_img_rle_decode) */
#pragma pack(1)
typedef struct xn_cel {
    u16 frames;             /* +00 */
    u16 width;              /* +02 */
    u16 height;             /* +04 */
    u8 unknown_06[8];       /* +06 */
    u32 row_offset[1];      /* +0E frames x height */
} xn_cel;
#pragma pack()

/* The image xn_draw_image_scaled and xn_draw_paperdoll_item unpack (word-sized statics, as the
   asm keeps them) */
extern u16 xn_draw_scaled_src_w;        /* columns (and the unpacked row's bytes) */
extern u16 xn_draw_scaled_src_h;        /* rows */
extern u8 *xn_draw_scaled_src;          /* the image: dword row offsets (bit 31: RLE) */

/* The row-compressed image xn_draw_scaled_src (src_h rows; per row a dword offset from 1Ch
   bytes before the image, bit 31 set for an RLE row) unpacked into scratch_buffer, rows 256
   bytes apart (src_w bytes each). Keeps every register. */
void xn_img_unpack_rows(void);

/* Decodes one RLE row from src to dst: a word, the row's length; then words n: n > 0, n
   literal bytes follow; n < 0, -n copies of the next byte; until the length is used up.
   Returns 0 in ECX, the asm's spent counts (xn_draw_cel_frame's row loop, which keeps only CX
   across the call, goes on with its top half). */
u32 xn_img_rle_decode(const u8 *src, u8 *dst);
#pragma aux xn_img_rle_decode parm [eax] [edx] value [ecx] modify exact [eax ecx edx];

/* The group-th RLE group of a weapon CIF: past its first image, then group sizes. */
xn_rle_group *xn_img_cif_group(const xn_img *cif, s32 group);

/* Dead: frame `frame` of an RLE group unpacked into dst (rows width bytes apart). Returns 1.
   (Its failure exit 0xCB54C, `xor eax, eax` and CB4D7's epilogue, is never reached and stays
   asm: it pops a frame it did not push.) */
s32 xn_img_rle_frame_unpack(const xn_rle_group *g, s32 frame, u8 *dst);

/* Dead: the k-th image of a CIF (k images skipped by their data sizes). */
const xn_img *xn_img_skip_records(const xn_img *img, s32 k);

/* Dead: an IMG's opaque pixels remapped in place through xn_pal_ega16_map (an identity map of
   the 16 EGA colours: indices above 15 read past it). Keeps every register. */
void xn_img_remap_colours(xn_img *img);
#pragma aux xn_img_remap_colours parm [eax] modify exact [eax];

#endif
