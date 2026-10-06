/* port_host.h: what the native build's host side shares (port/shim, port/host; docs/port.md).
   Not included by the game's sources. */
#ifndef PORT_HOST_H
#define PORT_HOST_H

#include <stddef.h>

/* the game's folders: `game` holds the install (only read); `overlay` takes every file the
   game writes (saves, the config it rewrites), and is read first, as tools/fallemu.py does */
void dos_set_dirs(const char *game, const char *overlay);

/* a DOS path ("C:\ARENA2\TEXT.RSC", "arena2\\text.rsc", "Z.CFG") as a host path: the overlay's
   copy when there is one (or when writing), else the install's; 0 when nothing exists and
   `writing` is 0 */
int dos_host_path(const char *dos, int writing, char *out, size_t n);

/* stop the host layer (SDL) before the process ends */
void host_shutdown(void);

/* a symbol the native build does not provide yet: logs its name and stops (port/gen stubs) */
void port_unimplemented(const char *name) __attribute__((noreturn));

/* stops with a message and the call chain (port/host/main.c) */
void port_fatal(const char *fmt, ...) __attribute__((noreturn, format(printf, 1, 2)));

/* stops when p is a pointer that lost its top half on the way (an int in the game's C:
   docs/port.md, phase 3): nothing the host maps lies below 4 GB */
void port_check_ptr(const void *p, const char *what);

#endif
