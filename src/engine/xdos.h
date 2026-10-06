/* xdos.h: XnGine's DOS files (src/engine/dos.c). Canonical C: plain prototypes, Watcom's own
   calling convention; docs/xngine_canonical.md.

   What it does
     The DOS services the engine uses (int 21h through the DOS extender, CauseWay): open,
     create, read, write, seek and close a file, find files, print a message and end the
     program. Each of those functions is one DOS call, the platform's file layer; the rest of
     the engine reads its files through them. On top of them: whole files loaded from the
     ARENA2 directory (fonts, light.dat, the shade tables), the length of a file, and the
     'DOS:' fatal error that ends the game when a file is missing.

   Units and conventions
     handles    DOS's file handles (16 bits, in an s32).
     offsets    bytes; seek takes a signed 32-bit offset and a mode (0 from the start, 1 from
                the current position, 2 from the end), as DOS does.
     results    what DOS returns (a count, a handle, a position); a failure (DOS's carry) as
                the function says. A count or a position comes back as DOS left it: after a
                failure that is DOS's error code, and no engine caller tells the two apart
                (Q-DOS-02).
     paths      NUL-terminated; xn_dos_print's messages end in '$' (DOS function 09h).

   Globals (in object 2, the engine's)
     xn_dos_path                ARENA2 path + '\' + name, built by xn_dos_make_path;
     xn_dos_msg_not_found, xn_dos_msg_create_failed, xn_dos_msg_out_of_memory
                                the 'DOS:' messages ('$'-terminated).
   The game's
     arena2_path                Z.CFG's path ('C:\ARENA2\');
     disk_last_file_size        the length of the last file loaded (the game reads it);
     file_resolver              the disk index's path lookup (disk_resolve_path), or 0;
     internal_check_failed      not 0: fatal_error reports 'Failed internal check N.' with it
                                instead of its message;
     fatal_error, exit, filelength and the allocator (the boundary's exits).

   Quirks kept (docs/engine/quirks.md): Q-DOS-01 (a missing file is reported as a failed
   internal check), Q-DOS-02 (counts and positions are DOS's AX even after a failure),
   Q-DOS-03 (xn_dos_write checks only its last piece), Q-DOS-04 (xn_dos_make_path's shared
   count of 80 characters), Q-DOS-05 (xn_dos_load_file adds an error code to the length). */
#ifndef XDOS_H
#define XDOS_H

#include "xngine.h"

#define XN_DOS_SEEK_SET 0               /* seek modes */
#define XN_DOS_SEEK_CUR 1
#define XN_DOS_SEEK_END 2

extern char xn_dos_path[81];            /* ARENA2 path + '\' + name (0xC0B53) */
extern char xn_dos_msg_not_found[];     /* 'DOS: File not found: $' */
extern char xn_dos_msg_create_failed[]; /* 'DOS: Error creating file: $' */
extern char xn_dos_msg_out_of_memory[]; /* 'DOS: Out of memory loading file: $' */

/* The game's */
extern char arena2_path[];
extern u32 disk_last_file_size;
extern u32 internal_check_failed;
extern char *(*file_resolver)(char *path);
s32 filelength(s32 handle);
void exit(s32 status);
u32 fatal_error(const char *message);   /* returns only while the game is already exiting */
void *func_000A10A8(u32 size);          /* the game's allocator (malloc) */

/* ---- files ------------------------------------------------------------------------------ */

/* Opens path for reading (DOS 3D00h), first passing it through the game's file_resolver when
   one is set (the disk index: it may give another path). Returns the handle. A file that
   does not open ends the game: the 'DOS:' fatal error (Q-DOS-01). The textures, WOODS.WLD,
   xn_dos_load_file. */
s32 xn_dos_open(const char *path);

/* Opens path as it is (DOS 3Dh with access mode `mode`: 0 read, 1 write, 2 both; no
   file_resolver, never fatal). *handle gets DOS's AX either way: the handle, or the error
   code when DOS fails. Returns 0 when it opened, else that error code. The movie player opens
   its file with it and never looks at the result. */
u32 xn_dos_open_mode(const char *path, u8 mode, s32 *handle);

/* Creates (or truncates) path with no attributes (DOS 3Ch). Returns the handle. A failure
   ends the game ('DOS: Error creating file: ', Q-DOS-01). The world editor's (dead). */
s32 xn_dos_create(const char *path);

/* Closes a file (DOS 3Eh). */
void xn_dos_close(s32 handle);

/* Reads count bytes into buf (DOS 3Fh). Returns the bytes read: count, fewer at the end of
   the file, or DOS's error code after a failure (Q-DOS-02). The textures, WOODS.WLD, whole
   files. */
u32 xn_dos_read(s32 handle, void *buf, u32 count);

/* Writes count bytes from buf (DOS 40h) in pieces of 32K while more than 32K are left (a
   signed compare), then the rest. Returns 1 when the last piece failed, else 0: the others'
   failures are not looked at (Q-DOS-03). The world editor's (dead). */
int xn_dos_write(s32 handle, const void *buf, s32 count);

/* Moves the file pointer to offset from mode (DOS 42h: XN_DOS_SEEK_SET, _CUR or _END; any
   other mode DOS refuses and does not move). Returns 0, or DOS's error code when it fails;
   the new position to *pos (pos may be 0). */
u32 xn_dos_seek(s32 handle, s32 offset, u8 mode, u32 *pos);

/* Finds the first file matching spec, any attributes (DOS 4Eh): 1 when one matched (its
   entry in the disk transfer area), 0 when none. Dead (only the C form calls it). */
int xn_dos_find_first(const char *spec);

/* The next match of the last xn_dos_find_first (DOS 4Fh): 1, or 0 when there are no more.
   Dead. */
int xn_dos_find_next(void);

/* 1 when path opens for reading (DOS 3D00h; closed again at once), else 0. xn_vid_play checks
   the movie file with it. */
int xn_dos_file_exists(const char *path);

/* The length of path in bytes: opened (xn_dos_open: a missing file is fatal), the pointer
   moved to the end, closed. */
u32 xn_dos_file_size(const char *path);

/* Loads ARENA2\name whole into buf, or into a block of its length from the game's allocator
   when buf is 0 (no memory is fatal: 'DOS: Out of memory loading file: '), reading FFFFh
   bytes at a time while whole pieces come; disk_last_file_size counts what was read (an
   error code too, Q-DOS-05). Returns buf. Fonts, light.dat, the shade tables. */
void *xn_dos_load_file(const char *name, void *buf);

/* Builds ARENA2\name in xn_dos_path: arena2_path, a backslash when it does not end in one,
   then name, terminated. The path and the name share one count of 80 characters, the
   backslash not counted (Q-DOS-04). Returns xn_dos_path. */
char *xn_dos_make_path(const char *name);

/* Dead: loads the palette ARENA2\name into buf (300h bytes from the game's allocator when buf
   is 0): a file of 768 bytes as it is; any other is read from offset 8, 768 bytes, each
   shifted right 2 (8-bit colour to the DAC's 6). Returns buf. */
u8 *xn_dos_load_palette(const char *name, u8 *buf);

/* ---- the C forms (dead: XnGine's library, linked whole and never called) ---------------- */

s32 xn_dos_open_c(const char *path);                    /* xn_dos_open */
s32 xn_dos_create_c(const char *path);                  /* xn_dos_create */
void xn_dos_close_c(s32 handle);                        /* xn_dos_close */
u32 xn_dos_read_c(void *buf, u32 count, s32 handle);    /* xn_dos_read */
/* xn_dos_write: 1 when the last piece failed */
int xn_dos_write_c(const void *buf, s32 count, s32 handle);
/* xn_dos_seek: 1 when DOS failed */
int xn_dos_seek_c(u8 mode, s32 offset, s32 handle);
int xn_dos_find_first_c(const char *spec);              /* 1 when nothing matched */
int xn_dos_find_next_c(void);                           /* 1 when there are no more */
int xn_dos_file_exists_c(const char *path);             /* 1 when path does NOT open */
u32 xn_dos_file_size_c(const char *path);               /* xn_dos_file_size */

/* ---- messages and the end ---------------------------------------------------------------- */

/* Prints msg, which ends in '$' (DOS 09h). */
void xn_dos_print(const char *msg);

/* Ends the program with exit code `code` (DOS 4Ch). Does not return. */
void xn_dos_exit(u8 code);

/* The selector of the program's PSP (DOS 51h). */
u16 xn_dos_psp(void);

#endif
