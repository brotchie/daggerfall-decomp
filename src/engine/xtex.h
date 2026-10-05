/* xtex.h: XnGine's texture cache, 'SET:' (readable C, tex.c; see xngine.h): TEXTURE.nnn archives
   loaded whole into a heap of blocks (first fit, merged when freed, the least recently used
   archive evicted when the heap is short), their records relocated and given compiled mappers,
   and the animated records' frames decoded per frame into 256-wide strips of an unpack buffer.

   Each declaration keeps the function's asm interface (its row in config/xngine_abi.csv): no
   pragma is Watcom's own convention; a pragma names the registers; NAME_r is the glue for a
   function whose asm callers read several registers or flags. */
#ifndef XTEX_H
#define XTEX_H

#include "xpipe.h"

/* Marks every record of a loaded archive for the flats' blend table 1 (translucent: the ghost
   and wraith archives). */
void xn_tex_archive_set_translucent(s32 archive);

/* A record's cache entry: loads its archive when needed, counts the archive's use this frame,
   stamps its heap block with the BIOS tick; for an animated record, picks the frame (*frame,
   or when it is negative the animation clock's frame) and decodes it for this frame (moving
   the record's compiled mapper to it). The entry's current image is the record's image.
   Returns 0 when the cache is full or the archive cannot be loaded (the asm's CF). */
struct xn_tex_entry *xn_tex_cache_lookup(s32 archive, s32 record, s32 *frame);
void xn_tex_cache_lookup_r(xn_regs *r);

/* A record's image header (loading its archive when needed), or 0. frame: only kept for the
   unpack key. (EBP EDI, the row's other inputs, are only saved around the archive load.) */
struct xn_tex_image *xn_tex_cache_lookup_image(s32 archive, s32 record, s32 frame);
#pragma aux xn_tex_cache_lookup_image parm [eax] [edx] [ebx] value [eax] modify exact [eax ecx edx];

/* Empties the cache: the heap one free block, no archive loaded, the mapper pool reset. */
void xn_tex_cache_flush(void);

/* Each frame: the archives' use counts and the unpack buffer cleared. (Keeps every register
   but AL: the route's stub keeps EAX.) */
void xn_tex_cache_begin_frame(void);
#pragma aux xn_tex_cache_begin_frame modify exact [eax];

/* Loads TEXTURE.nnn (the archive's number in three digits) from the configured path into a
   heap block, relocates its records' image offsets, and gives each solid (not RLE, no colour
   0) record its wrap masks and a compiled mapper. Returns 0 when there is no room (the cache
   is then full) or the mapper pool is full (the asm's CF). */
int xn_tex_load_archive(s32 archive);
void xn_tex_load_archive_r(xn_regs *r);

/* Sets the image's flag 100h when a row of it (its first frame, when animated) has a colour 0
   before its last column (a pixel 0 in the last column is missed: the asm's repne scasb leaves
   ECX 0 there too). */
void xn_tex_check_transparent(struct xn_tex_image *image);
#pragma aux xn_tex_check_transparent parm [ebx] modify exact [eax esi edi];

/* Start-up: a heap of `size` bytes as one free block, the 0xC0000-byte unpack buffer, no
   archive loaded, the mapper pool; out of memory ends the program ('SET: Out of memory.'). */
void xn_tex_cache_init(u32 size);
#pragma aux xn_tex_cache_init parm [eax] modify exact [eax edx ebx];

/* Frees the heap, the unpack buffer and the mapper pool. */
void xn_tex_cache_free(void);
#pragma aux xn_tex_cache_free modify exact [eax];

/* A heap block's data of `size` bytes: the free total recounted, archives evicted when it is
   short, then the first block that fits. 0 (the asm's CF) when none fits: the cache is full. */
void *xn_tex_heap_alloc(s32 size);
void xn_tex_heap_alloc_r(xn_regs *r);

/* Frees the least recently used archives until `size` bytes are free (or none is left to
   free). Returns 1: the asm's stc is undone by the clc after it, so it never reports a
   failure (an original bug, kept: xn_tex_heap_alloc's fatal exit is unreachable). */
int xn_tex_heap_evict(s32 size);
void xn_tex_heap_evict_r(xn_regs *r);

/* The used heap block with the oldest tick (older than now) whose archive was not used this
   frame, or 0. (The asm leaves EDX 0, the end of its walk.) */
struct xn_tex_block *xn_tex_heap_find_lru(void);
void xn_tex_heap_find_lru_r(xn_regs *r);

/* The first free block of at least `size` bytes, marked used (split when bigger than size +
   a header): its data, or 0. */
void *xn_tex_heap_alloc_first_fit(s32 size);

/* Frees a block (by its data) and merges it with a free next block, then with a free previous
   one. */
void xn_tex_heap_free(void *data);
#pragma aux xn_tex_heap_free parm [eax] modify exact [eax ecx ebx];

/* An animated record's RLE frame decoded into this frame's unpack buffer, unless it already
   is: returns its pixels (rows of 256 bytes). (The row's EBX input reaches only the unpack
   allocator's fatal exit, whose callees' rows read every register.) */
u8 *xn_tex_decode_frame(const struct xn_tex_frame *frame);
#pragma aux xn_tex_decode_frame parm [esi] value [eax] modify exact [eax esi];

/* A place for a w x h frame in the unpack buffer, keyed by the current lookup (archive,
   record, frame): returns 1 (the asm's CF) when this frame already has it (*pixels), else 0
   and a new place in a strip with room, or in a new strip under the last one. A full buffer
   ends the program ('SET: Out of unpackmemory.'). */
int xn_tex_unpack_alloc(s32 w, s32 h, u8 **pixels);
void xn_tex_unpack_alloc_r(xn_regs *r);

/* The pixels of the frame `key` decoded this frame: 1 (CF) and *pixels when found. */
int xn_tex_unpack_find(u32 key, u8 **pixels);
void xn_tex_unpack_find_r(xn_regs *r);

/* xn_tex_heap_free_bytes = the sizes of the free blocks. (Keeps every register.) */
void xn_tex_heap_sum_free(void);
#pragma aux xn_tex_heap_sum_free modify exact [eax];

/* An RLE frame decoded into big_buffer (rows of 256 bytes). */
void xn_tex_decode_to_big_buffer(const struct xn_tex_frame *frame);
#pragma aux xn_tex_decode_to_big_buffer parm [esi] modify exact [eax ecx edx ebx esi edi];

#endif
