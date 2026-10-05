/* xdos.h: XnGine's DOS file I/O (src/engine/dos.c; see xngine.h): opening, reading and
   seeking with int 21h, whole-file loads from the ARENA2 directory, and the 'DOS:' fatal
   error. The asm cores take their arguments in the registers DOS wants (the handle in EBX,
   the buffer in EDX, the count in ECX); a C-callable wrapper (_c) of each, and the find,
   create, write and file size functions, are XnGine's library, linked in whole and never
   called.

   A DOS call is compared by the value of EAX at its int 21h. Where the asm sets only AH (or
   AX), the rest of EAX is what its caller left there, and the C passes it on: those
   functions take the caller's EAX.

   Each declaration keeps the function's asm interface (config/xngine_abi.csv): no pragma is
   Watcom's own convention; a pragma names the registers; NAME_r is the glue for a function
   whose asm callers read several registers or flags. */
#ifndef XDOS_H
#define XDOS_H

#include "xngine.h"
#include "xsysutil.h"

extern char xn_dos_path[81];                /* ARENA2 path + '\' + name (0xC0B53) */
extern char xn_dos_msg_not_found[];         /* 'DOS: File not found: $' */
extern char xn_dos_msg_create_failed[];     /* 'DOS: Error creating file: $' */
extern char xn_dos_msg_out_of_memory[];     /* 'DOS: Out of memory loading file: $' */

/* The game's */
extern char arena2_path[];                  /* the Z.CFG path: 'C:\ARENA2\' */
extern u32 disk_last_file_size;             /* the length of the last file loaded */
extern u32 internal_check_failed;           /* non-zero: fatal_error reports it */
extern char *(*file_resolver)(char *path);  /* the disk index's path lookup, or 0 */
s32 filelength(s32 handle);
void exit(s32 status);
/* (the game's void function: what it leaves in EAX when it returns, which it does only when
   the game is already exiting) */
u32 fatal_error(const char *message);

/* Dead: loads a palette ARENA2\name into buf (300h bytes allocated when buf is 0): a 768-byte
   file as it is; any other is read from offset 8, 768 bytes, each shifted right 2 (8-bit
   colour to the DAC's 6). Returns buf. */
u8 *xn_dos_load_palette(const char *name, u8 *buf);
#pragma aux xn_dos_load_palette parm [eax] [edx] value [eax] modify exact [eax esi];

/* Dead: xn_dos_open for C: the handle of path. */
s32 xn_dos_open_c(const char *path);

/* Opens *path for reading (int 21h 3D00h), first passing it through the game's
   file_resolver when one is set (which may replace *path). Returns the handle. A missing file
   is fatal ('DOS: File not found: '), reported as the failed internal check `check` when that
   is not 0: the asm stores its caller's EAX there. */
s32 xn_dos_open(const char **path, u32 check);
void xn_dos_open_r(xn_regs *r);

/* Dead: xn_dos_close for C. */
void xn_dos_close_c(s32 handle);
#pragma aux xn_dos_close_c parm [eax] modify exact [eax];

/* Closes a file (int 21h 3E00h). */
void xn_dos_close(s32 handle);
#pragma aux xn_dos_close parm [ebx] modify exact [eax];

/* Dead: xn_dos_find_first for C: 1 when nothing matches spec (DOS's carry), else 0. */
s32 xn_dos_find_first_c(const char *spec);

/* Find first (int 21h AH = 4Eh, any attributes) of the spec at EDX; AL and the upper half of
   EAX as the caller left them; DOS's carry says none matched. */
void xn_dos_find_first_r(xn_regs *r);

/* Dead: xn_dos_find_next for C: 1 when there are no more (the carry). eax: the caller's EAX,
   which DOS gets with AH = 4Fh. */
s32 xn_dos_find_next_c(u32 eax);

/* Find next (int 21h AH = 4Fh); the carry says no more. */
void xn_dos_find_next_r(xn_regs *r);

/* Dead: xn_dos_file_exists for C: 0 when path opens, 1 when not. */
s32 xn_dos_file_exists_c(const char *path);
#pragma aux xn_dos_file_exists_c parm [eax] value [eax] modify exact [eax ebx];

/* Whether path opens (int 21h 3D00h; closed again at once). *handle: the handle it had.
   xn_vid_play checks the movie file with it. */
s32 xn_dos_file_exists(const char *path, s32 *handle);
void xn_dos_file_exists_r(xn_regs *r);

/* Dead: xn_dos_create for C: the handle of a new file at path. */
s32 xn_dos_create_c(const char *path);

/* Creates path (int 21h 3Ch, attributes 0); returns the handle. Failure is fatal ('DOS:
   Error creating file: '). eax: the caller's EAX: DOS gets it with AX = 3C00h, and it is the
   failed internal check reported. The world editor's (dead). */
s32 xn_dos_create(const char *path, u32 eax);
void xn_dos_create_r(xn_regs *r);

/* Dead: xn_dos_write for C: 1 when the last write failed (the carry), else 0. */
s32 xn_dos_write_c(const u8 *buf, u32 count, s32 handle);
#pragma aux xn_dos_write_c parm [eax] [edx] [ebx] value [eax] modify exact [eax edx];

/* Writes count bytes from *buf to a file (int 21h 40h) in pieces of 32K (while more than 32K
   are left: a signed compare); *buf advances past each whole piece, not past the last.
   Returns DOS's carry from the last piece (the others' are not looked at). The world
   editor's (dead). */
s32 xn_dos_write(const u8 **buf, s32 count, s32 handle);
void xn_dos_write_r(xn_regs *r);

/* Dead: xn_dos_read for C. */
s32 xn_dos_read_c(void *buf, u32 count, s32 handle);
#pragma aux xn_dos_read_c parm [eax] [edx] [ebx] value [eax] modify exact [eax edx];

/* Reads count bytes into buf (int 21h 3F00h); returns DOS's EAX: the bytes read (or the error
   code, with the carry, which no caller looks at). Textures, WOODS.WLD, whole files. */
s32 xn_dos_read(void *buf, u32 count, s32 handle);
#pragma aux xn_dos_read parm [edx] [ecx] [ebx] value [eax] modify exact [eax];

/* Dead: xn_dos_seek for C: the offset as one dword, split into CX:DX; 1 when DOS failed.
   eax: the caller's EAX, whose AL is the mode. */
s32 xn_dos_seek_c(u32 eax, u32 offset, s32 handle);
#pragma aux xn_dos_seek_c parm [eax] [edx] [ebx] value [eax] modify exact [eax edx];

/* Seek (int 21h AH = 42h): AL the mode (0 start, 1 current, 2 end), CX:DX the offset, EBX the
   handle; DX:AX the new position, the carry on error. The rest of EAX goes to DOS as the
   caller left it: xn_world_read_cell's reader calls it without setting AL (docs/xngine.md). */
void xn_dos_seek_r(xn_regs *r);

/* Dead: xn_dos_file_size for C. */
u32 xn_dos_file_size_c(const char *path);

/* The length of path: opened, the pointer moved to the end, closed. eax: the caller's EAX:
   the failed check of a missing file, and the upper half of the seek's EAX. */
u32 xn_dos_file_size(const char *path, u32 eax);
void xn_dos_file_size_r(xn_regs *r);

/* Loads ARENA2\name whole into buf, or into a block of its length allocated with the game's
   malloc when buf is 0 (failure is fatal: 'DOS: Out of memory loading file: '), reading
   FFFFh bytes at a time; disk_last_file_size counts what was read. Returns buf. Fonts,
   light.dat, the shade tables. */
u8 *xn_dos_load_file(const char *name, u8 *buf);
void xn_dos_load_file_r(xn_regs *r);

/* xn_dos_path = arena2_path, a backslash when it does not end in one, then name, terminated.
   The path and the name share one count of 80 characters (the backslash not counted); a path
   of 80 or more leaves the count at 0, which the name's loop then takes as 2^32. Returns
   xn_dos_path. */
char *xn_dos_make_path(const char *name);
void xn_dos_make_path_r(xn_regs *r);

#endif
