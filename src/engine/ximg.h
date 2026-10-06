/* ximg.h: XnGine's image records and decoders (src/engine/img.c). Canonical C: plain
   prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     Walks IMG and CIF records, finds a weapon CIF's RLE frame groups, and decodes the row
     formats of the game's images: the row-compressed images of the inventory (a dword offset
     per row, each row raw or RLE), the RLE rows of CEL files and the RLE frames of weapon
     CIFs.

   Formats
     IMG/CIF      a 12-byte header (xn_img), then width x height pixels; a CIF's next image
                  follows data_size bytes of pixels
     RLE row      a word: the row's length; then words n: n > 0, n literal bytes follow;
                  n < 0, -n copies of the next byte; until the length is used up
     RLE frame    per row, bytes c: c < 80h, c + 1 literal pixels follow; else (c & 7Fh) + 1
                  copies of the next byte; until the group's width is used up
     row-compressed  a table of dwords (one per row) counted from 1Ch bytes before the table:
                  bit 31 set, an RLE row at that offset; else the raw row there

   Pixels are palette indices; 0 is transparent where the drawing routines say so (xdraw.h).
   Quirk kept: Q-IMG-01 (the EGA remap reads past its 16-entry map). */
#ifndef XIMG_H
#define XIMG_H

#include "xngine.h"

/* An IMG file, or one image of a CIF file (the game's struct image, include/structs.h) */
typedef struct xn_img {
    u16 x;                  /* +00 where it is drawn */
    u16 y;                  /* +02 */
    u16 width;              /* +04 */
    u16 height;             /* +06 */
    u16 compression;        /* +08 */
    u16 data_size;          /* +0A bytes of pixels */
    u8 pixels[1];           /* +0C */
} xn_img;

/* A group of RLE frames in a weapon CIF (after its first, plain image) */
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

/* A CEL animation (MAGE.CEL, ROGUE.CEL): a 14-byte header, then per frame a dword per row: the
   row's pixels from the file's start, bit 31 set for an RLE row (xn_img_rle_decode) */
#pragma pack(1)
typedef struct xn_cel {
    u16 frames;             /* +00 */
    u16 width;              /* +02 */
    u16 height;             /* +04 */
    u8 unknown_06[8];       /* +06 */
    u32 row_offset[1];      /* +0E frames x height */
} xn_cel;
#pragma pack()

/* The row-compressed image whose row table is `rows_table` (rows entries) unpacked into dst,
   rows 256 bytes apart, w bytes each. xn_draw_image_scaled and xn_draw_paperdoll_item. rows of
   0 runs 2^32 times (the asm's dword count; never passed). */
void xn_img_unpack_rows(const u8 *rows_table, u32 w, u32 rows, u8 *dst);

/* Decodes one RLE row from src to dst. */
void xn_img_rle_decode(const u8 *src, u8 *dst);

/* The group-th RLE group of a weapon CIF: past its first image, then group sizes. Two game
   sites. */
xn_rle_group *xn_img_cif_group(const xn_img *cif, s32 group);

/* Dead: frame `frame` of an RLE group unpacked into dst (rows width bytes apart). Returns 1.
   (Its failure exit 0xCB54C, `xor eax, eax` and CB4D7's epilogue, is no function: nothing jumps
   to it.) */
s32 xn_img_rle_frame_unpack(const xn_rle_group *g, s32 frame, u8 *dst);

/* Dead: the k-th image of a CIF (k images skipped by their data sizes; k = 0: img). */
const xn_img *xn_img_skip_records(const xn_img *img, s32 k);

/* Dead: an IMG's opaque pixels remapped in place through xn_pal_ega16_map. Quirk Q-IMG-01: an
   identity map of the 16 EGA colours, read past its end for pixels above 15. */
void xn_img_remap_colours(xn_img *img);

#endif
