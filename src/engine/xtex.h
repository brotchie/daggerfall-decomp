/* xtex.h: XnGine's texture cache, 'SET:' (src/engine/tex.c). Canonical C: plain prototypes,
   Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     The TEXTURE.nnn archives a frame uses, loaded whole into a heap of blocks and kept while
     there is room: a lookup loads the archive when it is not there, stamps its block with the
     BIOS tick, and for an animated record decodes this frame's image into the unpack buffer.
     The heap gives the first free block that fits (split when bigger) and merges freed
     blocks with free neighbours; when it is short, the archives not used this frame go,
     least recently looked up first. Loading relocates the records' image offsets, marks the
     images with a transparent pixel, and gives each solid image its wrap masks and a texture
     mapper (xn_tmap_compile, the rasteriser group's). A full heap or mapper pool sets
     xn_tex_cache_full: lookups then fail until the game flushes the cache.

   Formats
     an archive    the file as it is (struct xn_tex_archive): record_count, a name, and
                   20-byte directory entries (struct xn_tex_entry) whose image offsets the
                   load turns into pointers; an entry's kind is 4 when its image has a mapper
                   (the render mode's textured setup), else 0
     an image      struct xn_tex_image: the packed wrap masks (0FFh | u mask << 8 | 0FFh << 16
                   | v mask << 24: n - 1 for a power-of-two size, else 0FFh), width, height,
                   flags (1000h RLE, 100h has a colour 0), the mapper, the pixels' offset (rows
                   of 256 bytes), and for an animated record its frames (RLE: per row, pairs of
                   a zero count and a copy count with the bytes)
     the heap      blocks of a 22-byte header (struct xn_tex_block: next, prev, size, flags,
                   the archive slot, the tick) and the data, from xn_tex_heap_head
     the unpack    the 0xC0000-byte buffer of 256-wide rows, in strips (struct
                   xn_tex_unpack_strip) side by side; this frame's decoded frames by key
                   ((archive << 7 | record) << 16 | frame & 0FFFFh)

   Tables and state (object 2): xn_tex_archives[512] (game-visible), the use counts this frame
   (xn_tex_archive_use, words), the record offsets (record * 20), xn_tex_size_mask, the heap
   (xn_tex_heap_base, _head, _size, _free_bytes), the unpack buffer and its strips and
   entries, xn_tex_cache_full (game-visible), the path built for the last load (xn_tex_path,
   game-visible). The heap's block is game-visible whole (the game reads images from it).

   Quirks kept: Q-TEX-01 (an eviction never reports failure: the heap's fatal exit cannot
   run), Q-TEX-02 (the least recently used search tests the archive's use count at a byte
   offset), Q-TEX-03 (the path is not terminated after the file name), Q-TEX-04 (a pixel 0 in
   an image's last column is missed), Q-TEX-05 (a frame number from the caller is not checked
   against the image's frames), Q-TEX-06 (the frames the animation clock picks share one
   decoded frame a game frame). docs/engine/quirks.md. */
#ifndef XTEX_H
#define XTEX_H

#include "xpipe.h"

/* ---- lookups ----------------------------------------------------------------------------- */

/* A record's cache entry: loads its archive when needed, counts the archive's use this
   frame, stamps its heap block with the BIOS tick. For an animated record (more than one
   frame) the image is decoded for this frame: `frame`, or when it is negative the animation
   clock's (xn_anim_ticks / the image's frame time, wrapped by the frame count; Q-TEX-06:
   decoded under the key of frame -1, once a game frame); the image's
   pixel offset then points at it. The entry's current image is the record's image. Returns 0
   when the cache is full or the archive cannot be had (the cache is then full). 11 game
   sites; the flats, the models' faces, the terrain. */
struct xn_tex_entry *xn_tex_cache_lookup(s32 archive, s32 record, s32 frame);

/* A record's image header (loading its archive when needed), or 0 when the cache is full.
   8 game sites; the flats' pick. */
struct xn_tex_image *xn_tex_cache_lookup_image(s32 archive, s32 record);

/* Marks every record of a loaded archive for the flats' blend table 1 (translucent: the
   ghost and wraith archives). 3 game sites. */
void xn_tex_archive_set_translucent(s32 archive);

/* ---- the cache --------------------------------------------------------------------------- */

/* Start-up: a heap of `size` bytes from the game's allocator as one free block, the unpack
   buffer, no archive loaded, the mapper pool, the frame's counts cleared. Without the memory
   the game ends ('SET: Out of memory.'). One game site. */
void xn_tex_cache_init(u32 size);

/* Frees the heap, the unpack buffer and the mapper pool. xn_render_shutdown. */
void xn_tex_cache_free(void);

/* Empties the cache: the heap one free block, no archive loaded, not full, the mapper pool
   reset. 12 game sites. */
void xn_tex_cache_flush(void);

/* Each frame: the archives' use counts and the unpack buffer cleared. 8 game sites. */
void xn_tex_cache_begin_frame(void);

/* Loads TEXTURE.nnn (the archive's number in three digits) from the configured path
   (xn_tex_path, Q-TEX-03) into a heap block, relocates its records' image offsets, and gives
   each image that is neither RLE nor transparent its wrap masks and a mapper (kind 4).
   Returns 0 when there is no room (the cache is then full) or the mapper pool is full. A
   missing file ends the game (xn_dos_open). */
int xn_tex_load_archive(s32 archive);

/* Sets the image's flag 100h when a row of it (its first frame, decoded into big_buffer, when
   animated) has a colour 0 before its last column (Q-TEX-04). */
void xn_tex_check_transparent(struct xn_tex_image *image);

/* ---- the heap ------------------------------------------------------------------------------ */

/* A block's data of `size` bytes: the free total recounted, archives evicted when it is
   short, then the first block that fits. 0 when none fits: the cache is then full. */
void *xn_tex_heap_alloc(s32 size);

/* Frees the least recently used archives until `size` bytes are free, or none is left to
   free. (The asm's carry for "none left" is cleared again: no caller learns of it,
   Q-TEX-01.) */
void xn_tex_heap_evict(s32 size);

/* The used block with the oldest tick (older than now) whose archive was not used this frame
   (Q-TEX-02), or 0. */
struct xn_tex_block *xn_tex_heap_find_lru(void);

/* The first free block of at least `size` bytes, marked used; one bigger than size + a header
   is split (it keeps size + a header's bytes, the rest a free block after it). Its data, or
   0. */
void *xn_tex_heap_alloc_first_fit(s32 size);

/* Frees a block (by its data) and merges it with a free next block, then with a free previous
   one. (The last block's next is 0: its header is read at address 0.) */
void xn_tex_heap_free(void *data);

/* xn_tex_heap_free_bytes = the sizes of the free blocks. */
void xn_tex_heap_sum_free(void);

/* ---- animated frames ------------------------------------------------------------------------ */

/* The RLE frame `frame` decoded into this frame's unpack buffer under `key`, unless it already
   is: its pixels (rows of 256 bytes). */
u8 *xn_tex_decode_frame(const struct xn_tex_frame *frame, u32 key);

/* A place for a w x h frame with this key in the unpack buffer: 1 and *pixels when this frame
   already has it; else 0 and a new place: in the first strip with room, or a new strip under
   the last one. A full buffer (256 frames, or past its end) ends the game ('SET: Out of
   unpackmemory.'). */
int xn_tex_unpack_alloc(s32 w, s32 h, u32 key, u8 **pixels);

/* The pixels of the frame `key` decoded this frame: 1 and *pixels when found (entry 0 is
   looked at first even when none is used). */
int xn_tex_unpack_find(u32 key, u8 **pixels);

/* An RLE frame decoded into big_buffer (rows of 256 bytes). */
void xn_tex_decode_to_big_buffer(const struct xn_tex_frame *frame);

#endif
