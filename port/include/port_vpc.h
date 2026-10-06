/* port_vpc.h: the native build's virtual PC (docs/port.md, phase 5). XnGine talks to the
   hardware through a handful of helpers (src/engine/xngine.h on main): port I/O (xn_inb,
   xn_outb ...), BIOS, DOS, DPMI and mouse calls with a register file (xn_int10 ... xn_int33),
   interrupt vectors (_dos_getvect, _dos_setvect), and real-mode memory (VGA at 0xA0000, the
   BIOS data area at 0x400). The native build gives it the same helpers, backed by SDL3:

   - low memory: a 1 MB + 64 KB block standing for real-mode memory. VGA's mode 13h screen is
     its 0xA0000 window; the BIOS tick count at 0x46C runs at 18.2 Hz; the shift flags at
     0x417 follow the keyboard;
   - ports: the VGA palette (3C7h-3C9h) and status (3DAh, a 70 Hz retrace), the keyboard
     (60h/61h), the PIC (20h/21h), the PIT (40h/43h), the joystick (201h: none) and COM1;
   - interrupts: the PIT's 18.2 Hz tick (int 8 and int 1Ch), the keyboard (int 9: a scancode at
     port 60h), on SDL's timer thread, one at a time, as one CPU takes them. HMI SOS's timer
     events (port/shim/sos.c) run on the same thread. port_cli/port_sti hold them off;
   - services: video (int 10h: mode 13h), keyboard BIOS (int 16h), DOS files and messages
     (int 21h), DPMI (int 31h), the mouse driver (int 33h), int 15h and int 2Fh;
   - the screen: mode 13h's 64,000 bytes through the 256-colour palette, scaled to the window
     at 4:3, shown at each retrace the engine waits for (or every 14 ms otherwise).

   The register file. XnGine's xn_regs holds eight 32-bit registers; a DOS call passes a
   buffer's address in one (DS:EDX for a read). Natively an address is 64 bits, so the native
   build's register file is pointer-wide: `struct vpc_regs` has the same fields in the same
   order as xn_regs, each an unsigned long. The engine's xngine.h must declare xn_regs that
   way under DAGGER_PORT (docs/port.md, "Linking the engine"); the services read an address
   register as a host pointer and ignore segment registers (the flat model). */
#ifndef PORT_VPC_H
#define PORT_VPC_H

#include <stddef.h>
#include <stdint.h>

/* ---- low memory ---------------------------------------------------------------------- */

#define VPC_LOWMEM_SIZE 0x110000u
extern unsigned char port_low_memory_block[VPC_LOWMEM_SIZE];
extern unsigned char *port_low_memory;          /* = port_low_memory_block */

/* a real-mode linear address (0xA0000, 0x46C) as a host pointer */
#define VPC_LOW(addr) ((void *)(port_low_memory + (uintptr_t)(addr)))

/* ---- the machine --------------------------------------------------------------------- */

/* low memory, the BIOS data area, the window, the interrupts: once, before the game runs
   (after SDL_Init). 0 on success. */
int vpc_init(const char *title);
void vpc_shutdown(void);

/* the main thread's turn: SDL's events (keys, mouse, quit), and the screen when one is due.
   Every port and service call comes through here at most every millisecond, so the game's
   own polling loops keep the window alive. */
void vpc_poll(void);

/* show the screen now (mode 13h through the palette) */
void vpc_present(void);

/* save the screen as it would be shown (320x200, palette applied) as a .bmp; 0 on success */
int vpc_screenshot(const char *path);

/* the window was closed or the game asked to quit: the next poll exits */
extern volatile int vpc_quit_requested;

/* ---- ports ----------------------------------------------------------------------------- */

unsigned char xn_inb(unsigned int port);
void xn_outb(unsigned int port, unsigned char v);
unsigned short xn_inw(unsigned int port);
void xn_outw(unsigned int port, unsigned short v);

/* ---- interrupts ------------------------------------------------------------------------- */

typedef void (*vpc_handler)(void);

/* the vector table: what _dos_setvect installs, what the PIT and keyboard interrupts call.
   The defaults are the BIOS's (int 9 fills the BIOS keyboard buffer; int 8 counts the
   tick and calls int 1Ch). */
vpc_handler vpc_getvect(unsigned int intno);
void vpc_setvect(unsigned int intno, vpc_handler h);

/* Watcom's names for them (FALL.EXE 0xA1272, 0xA12A6), as XnGine calls them */
vpc_handler _dos_getvect(unsigned int intno);
void _dos_setvect(unsigned int intno, vpc_handler h);

/* hold off interrupts (cli) and let them run again (sti); they nest */
void port_cli(void);
void port_sti(void);

/* run fn on the interrupt thread every period_ns until vpc_stop_timer; for HMI SOS's timer
   events. Returns a handle (0 on failure). */
unsigned int vpc_start_timer(uint64_t period_ns, void (*fn)(void *), void *arg);
void vpc_stop_timer(unsigned int handle);

/* ---- services with a register file -------------------------------------------------------- */

struct vpc_regs {
    unsigned long edi, esi, ebp, esp, ebx, edx, ecx, eax;
    unsigned long eflags;
};

#define VPC_CF 0x0001
#define VPC_ZF 0x0040

void xn_int10(struct vpc_regs *r);
void xn_int15(struct vpc_regs *r);
void xn_int16(struct vpc_regs *r);
void xn_int21(struct vpc_regs *r);
void xn_int2f(struct vpc_regs *r);
void xn_int31(struct vpc_regs *r);
void xn_int33(struct vpc_regs *r);

/* ---- sound (vpc_audio.c): voices playing the game's samples where they lie ------------------- */

int vpc_audio_init(void);
void vpc_audio_shutdown(void);
/* voice 0..159; bits 8 (unsigned) or 16 (signed); volume left << 16 | right (0x7FFF the
   loudest); pan 0..0xFFFF (0x8000 the centre) */
int vpc_audio_play(int voice, const void *data, int bytes, int rate, int bits, int channels,
                   int loop, unsigned int volume, unsigned int pan);
void vpc_audio_stop(int voice);
int vpc_audio_playing(int voice);
void vpc_audio_set_volume(int voice, unsigned int volume);
void vpc_audio_set_pan(int voice, unsigned int pan);

/* ---- the parts' own entry points (port/host/vpc_*.c) -------------------------------------- */

void vpc_video_init(const char *title);
void vpc_video_shutdown(void);
unsigned char vpc_video_inb(unsigned int port);
void vpc_video_outb(unsigned int port, unsigned char v);
void vpc_video_set_mode(unsigned int mode);
unsigned int vpc_video_mode(void);
void vpc_video_maybe_present(void);
int vpc_video_handle_event(const void *sdl_event);

void vpc_input_init(void);
int vpc_input_handle_event(const void *sdl_event);
unsigned char vpc_kbd_inb(unsigned int port);
void vpc_kbd_outb(unsigned int port, unsigned char v);
void vpc_kbd_irq(void);                 /* the interrupt thread: deliver queued scancodes */
int vpc_bios_key(int remove);           /* int 16h: scancode << 8 | ASCII, -1 when none */
void vpc_bios_int9(void);               /* the BIOS's own keyboard handler */
void vpc_mouse_service(struct vpc_regs *r);

void vpc_irq_init(void);
void vpc_irq_shutdown(void);
unsigned char vpc_pit_inb(unsigned int port);
void vpc_pit_outb(unsigned int port, unsigned char v);
void vpc_irq_lock(void);                /* taken by every interrupt delivery */
void vpc_irq_unlock(void);

#endif
