/* port.h: included ahead of every game source file in the native build (port/CMakeLists.txt,
   docs/port.md). The game's sources stay as the matching Watcom build needs them; this header
   only adapts them to the host compiler:
   - Watcom's keywords for far pointers become nothing (a flat address space has no segments);
   - the C library calls whose DOS behaviour the host's lack go to port/shim: Watcom's open
     flags (O_BINARY 0x200 ...) and DOS paths, Watcom's rand sequence, and exit (which shuts
     the host layer down). The macros are function-like, so a parameter or field with one of
     these names stays as it is. */
#ifndef PORT_H
#define PORT_H

#define DAGGER_PORT 1

#define __far
#define _far
#define __near
#define __interrupt
#define __loadds
#define __cdecl

/* the game's file calls; not in XnGine's files (PORT_ENGINE: its file I/O goes through int 21h,
   and its head-tracker drivers have members called open, read and close) */
#ifndef PORT_ENGINE
#define open(...) port_open(__VA_ARGS__)
#define read(...) port_read(__VA_ARGS__)
#define write(...) port_write(__VA_ARGS__)
#define lseek(...) port_lseek(__VA_ARGS__)
#define close(...) port_close(__VA_ARGS__)
#define unlink(...) port_unlink(__VA_ARGS__)
#endif
#define filelength(...) port_filelength(__VA_ARGS__)
#define fopen(...) port_fopen(__VA_ARGS__)
#define fclose(...) port_fclose(__VA_ARGS__)
/* FALL.EXE's 0xA16F8, named fprintf in config/names.csv, is sscanf (the sscanf module:
   cget_string 0xA16A4, vsscanf 0xA16D1; fprintf, fscanf and sscanf are the same bytes once
   relocations are masked). The game's one call, in kludge.c, is sscanf(version, "%d", &build). */
#define fprintf(...) port_sscanf(__VA_ARGS__)
#define rand(...) port_rand(__VA_ARGS__)
#define srand(...) port_srand(__VA_ARGS__)
#define exit(...) port_exit(__VA_ARGS__)
#define int386(...) port_int386(__VA_ARGS__)
#define int386x(...) port_int386x(__VA_ARGS__)
#define segread(...) port_segread(__VA_ARGS__)

#endif
