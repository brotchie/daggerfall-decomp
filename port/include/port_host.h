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
/* copies as Watcom's library made them (forward: rep movsd then rep movsb), so a copy into
   an overlapping buffer after its source repeats as it did under DOS (port/shim/memcheck.c) */
void *port_copy_forward(void *dst, const void *src, unsigned int n);
/* the overlay folder (given, $DAGGER_OVERLAY or ~/Library/Application Support/Daggerfall) and
   the game folder (given, $DAGGER_GAME, the remembered one, or the player picks it): launch.c */
const char *launch_overlay_dir(const char *given);
const char *launch_game_dir(const char *given);
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
/* a save slot's folder (its DOS name, e.g. "SAVE5") sent to a host folder, or (NULL, NULL) to
   stop: the feedback report's save (port/host/report.c) */
void dos_redirect_dir(const char *name, const char *root);

/* ---- feedback and crash reports (port/host/report.c, docs/port.md) ------------------------- */

/* the spool under the overlay (reports/), the session's log (stderr, teed) and input record,
   and the uploader; argv0 for the crash path's re-exec as the uploader */
void report_init(const char *overlay, const char *argv0);
/* the main thread, on F8: the screen, the memory, the log and input now; the game's save from
   the game's thread when it is in the world; then a box for the player's notes, and the upload */
void report_feedback(void);
/* a crash (a fault, a stub, a fatal error): what is known, written with plain system calls
   (a signal handler's), then the process becomes the uploader. what: a line of text */
void report_crash(const char *what, int sig, const void *addr, const void *pc);
/* `fall --upload-reports SPOOL`: sends what is pending and returns the exit code */
int report_upload_main(const char *spool);
/* the same as a crashed process becomes it (re-exec from report_crash): exits 70 */
int report_upload_after_crash(const char *spool);
/* a region of the game's memory for the snapshot (main.c: FALL.EXE's data block, low memory) */
void report_add_region(const char *name, const void *p, size_t size);
/* the game's side (main.c): fill in the game's facts as JSON members ("k": v, ...), and the
   game thread's per-frame hook asks whether a save is wanted (its folder) and says it is done */
void report_set_game_meta(void (*meta)(char *buf, size_t size));
/* whether the game can save now (main()'s frame loop runs: not the title or character creation) */
void report_set_can_save(int (*can_save)(void));
const char *report_save_wanted(void);
void report_save_done(int ok);
/* the player's input as a PORT_SCRIPT step ("down W", "mrel 1.5 0", "mdown 0" ...), at the
   script clock's time (port/host/vpc_input.c) */
void report_input(const char *step);
/* a line in the session's notes (the srand seed ...) */
void report_note(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

#endif
