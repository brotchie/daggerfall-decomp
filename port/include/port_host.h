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

/* stop the host layer (SDL, the virtual PC) before the process ends */
void host_shutdown(void);

/* SDL has started on this thread (the main thread: SDL_Quit only from here) */
void host_started(void);

/* a fault prints the call chain before the process ends */
void host_install_fault_handlers(void);
/* ARCH3D.BSA and DAGGER.SND from ARENA2\PACKED.DAT into the overlay when the install lacks
   them, as the installer did (port/host/packed.c); 0 when both are there */
int port_unpack_packed(void);
/* a fault at a low address: finish the access against the virtual PC's low memory, as DOS's
   flat memory did (port/host/zeropage.c); 1 when handled */
int port_zero_page_fault(void *siginfo, void *ucontext);

/* a symbol the native build does not provide yet: logs its name and stops (port/gen stubs) */
void port_unimplemented(const char *name) __attribute__((noreturn));

/* stops with a message and the call chain (port/host/main.c) */
void port_fatal(const char *fmt, ...) __attribute__((noreturn, format(printf, 1, 2)));

/* stops when p is a pointer that lost its top half on the way (an int in the game's C:
   docs/port.md, phase 3): nothing the host maps lies below 4 GB */
void port_check_ptr(const void *p, const char *what);

/* selectors (port/shim/dos.c): what a selector stands for, a new one for a base */
unsigned long port_sel_base(unsigned short sel);
unsigned short port_sel_new(unsigned long base);

/* the DOS file layer (port/shim/dosfile.c), with Watcom's open flags */
int port_open(const char *dos, int wflags, ...);
int port_read(int fd, void *buf, unsigned int n);
int port_write(int fd, const void *buf, unsigned int n);
int port_lseek(int fd, int offset, int whence);
int port_close(int fd);
void port_exit(int status) __attribute__((noreturn));

#endif
