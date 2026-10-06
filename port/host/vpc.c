/* vpc.c: the virtual PC's core (port/include/port_vpc.h): low memory and the BIOS data area,
   the port map, the main thread's poll, and the BIOS, DOS and DPMI services XnGine calls with
   a register file (the mouse is in vpc_input.c, the screen in vpc_video.c, the interrupts in
   vpc_irq.c). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <SDL3/SDL.h>

#include "port_host.h"
#include "port_vpc.h"

/* real-mode memory: a fixed block, so the game's data can point into it (tools/port_data.py
   turns FALL.EXE's pointers to 0xA0000 and the BIOS data area into addresses in it) */
unsigned char port_low_memory_block[VPC_LOWMEM_SIZE] __attribute__((aligned(4096)));
unsigned char *port_low_memory = port_low_memory_block;
volatile int vpc_quit_requested;
static SDL_ThreadID main_thread;      /* SDL's window and events belong to it */
static int vpc_initialised;

/* the BIOS data area */
#define BDA_VIDEO_MODE 0x449
#define BDA_COLUMNS 0x44A
#define BDA_CURSOR 0x450
#define BDA_CRTC 0x463
#define BDA_TICKS 0x46C

/* where the services keep their real-mode blocks: the PSP (int 21h 51h) and its DTA */
#define LOW_PSP 0x0600
#define LOW_DTA (LOW_PSP + 0x80)

static void put16(unsigned int a, unsigned int v)
{
    port_low_memory[a] = (unsigned char)v;
    port_low_memory[a + 1] = (unsigned char)(v >> 8);
}

static void put32(unsigned int a, unsigned int v)
{
    put16(a, v & 0xFFFF);
    put16(a + 2, v >> 16);
}

int vpc_init(const char *title)
{
    const char *t0 = getenv("PORT_BIOS_TICKS");
    unsigned int ticks;

    main_thread = SDL_GetCurrentThreadID();
    vpc_initialised = 1;
    port_low_memory[BDA_VIDEO_MODE] = 3;
    put16(BDA_COLUMNS, 80);
    put16(BDA_CRTC, 0x3D4);
    /* the tick count is the time since midnight, as DOS starts it; PORT_BIOS_TICKS fixes it
       (the game seeds rand from it, so a run can be repeated) */
    if (t0 != NULL) {
        ticks = (unsigned int)strtoul(t0, NULL, 0);
    } else {
        time_t now = time(NULL);
        struct tm *lt = localtime(&now);
        ticks = (unsigned int)((lt->tm_hour * 3600 + lt->tm_min * 60 + lt->tm_sec) * 18.2065);
    }
    put32(BDA_TICKS, ticks);
    vpc_video_init(title);
    vpc_input_init();
    vpc_irq_init();
    vpc_audio_init();
    vpc_music_init();
    vpc_script_init();
    return 0;
}

void vpc_shutdown(void)
{
    vpc_music_shutdown();
    vpc_audio_shutdown();
    vpc_irq_shutdown();
    vpc_video_shutdown();
}

/* ---- the threads -------------------------------------------------------------------------- *
   The game runs on a thread of its own (vpc_run), as a PC's CPU runs beside its hardware: the
   main thread, which SDL's window and events belong to, takes the keyboard and the mouse,
   shows the screen at the VGA's 70 Hz and runs the scripted driver, whatever loop the game is
   in (a game loop waiting for a key with no port I/O still sees the key come up). A program
   that runs everything on the main thread (vpcdemo) gets the same from vpc_poll. */

static SDL_ThreadID game_thread_id;
static SDL_AtomicInt game_done;
static int game_exit_code;
static int threaded;

int vpc_on_main_thread(void)
{
    return SDL_GetCurrentThreadID() == main_thread;
}

/* the main thread's turn: events, the script, the screen */
static void pump(void)
{
    SDL_Event e;

    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_EVENT_QUIT)
            vpc_quit_requested = 1;
        else if (!vpc_video_handle_event(&e))
            vpc_input_handle_event(&e);
    }
    vpc_script_poll();
    vpc_video_maybe_present();
}

void vpc_poll(void)
{
    static Uint64 last;
    Uint64 now;

    if (!vpc_initialised)
        return;
    if (!vpc_on_main_thread()) {
        /* the game thread (or an interrupt): a quit ends the game where it stands */
        if (vpc_quit_requested && threaded && SDL_GetCurrentThreadID() == game_thread_id)
            vpc_game_exit(0);
        return;
    }
    now = SDL_GetTicksNS();
    if (now - last < 1000000)
        return;
    last = now;
    pump();
    if (vpc_quit_requested) {
        fprintf(stderr, "port: quit\n");
        port_exit(0);
    }
}

void vpc_game_exit(int code)
{
    if (!threaded || vpc_on_main_thread())
        return;
    game_exit_code = code;
    SDL_SetAtomicInt(&game_done, 1);
    for (;;)                            /* the main thread ends the process */
        SDL_Delay(1000);
}

struct game_start {
    int (*fn)(void *);
    void *arg;
};

static int SDLCALL game_main(void *p)
{
    struct game_start *g = p;

    game_thread_id = SDL_GetCurrentThreadID();
    vpc_game_exit(g->fn(g->arg));
    return 0;
}

int vpc_run(int (*fn)(void *), void *arg)
{
    static struct game_start g;
    SDL_PropertiesID props = SDL_CreateProperties();
    SDL_Thread *t;
    Uint64 quit_at = 0;

    g.fn = fn;
    g.arg = arg;
    threaded = 1;
    SDL_SetPointerProperty(props, SDL_PROP_THREAD_CREATE_ENTRY_FUNCTION_POINTER, (void *)game_main);
    SDL_SetPointerProperty(props, SDL_PROP_THREAD_CREATE_USERDATA_POINTER, &g);
    SDL_SetStringProperty(props, SDL_PROP_THREAD_CREATE_NAME_STRING, "game");
    SDL_SetNumberProperty(props, SDL_PROP_THREAD_CREATE_STACKSIZE_NUMBER, 64 << 20);
    t = SDL_CreateThreadWithProperties(props);
    SDL_DestroyProperties(props);
    if (t == NULL) {
        fprintf(stderr, "port: no game thread: %s\n", SDL_GetError());
        return 1;
    }
    SDL_DetachThread(t);
    while (!SDL_GetAtomicInt(&game_done)) {
        SDL_WaitEventTimeout(NULL, 2);
        pump();
        if (vpc_quit_requested) {
            /* the game stops at its next port access; one in a loop without any, soon after */
            if (quit_at == 0) {
                fprintf(stderr, "port: quit\n");
                quit_at = SDL_GetTicksNS();
            } else if (SDL_GetTicksNS() - quit_at > 500000000ull) {
                game_exit_code = 0;
                break;
            }
        }
    }
    return game_exit_code;
}

/* ---- ports ------------------------------------------------------------------------------- */

static unsigned char pic_mask[2];
static unsigned char com1[8] = {0, 0, 0, 0, 0, 0x60, 0, 0};    /* LSR: transmitter empty */

unsigned char xn_inb(unsigned int port)
{
    vpc_poll();
    port &= 0xFFFF;
    if (port >= 0x3C0 && port <= 0x3DF)
        return vpc_video_inb(port);
    switch (port) {
    case 0x60:
    case 0x61:
    case 0x64:
        return vpc_kbd_inb(port);
    case 0x20:
    case 0xA0:
        return 0;
    case 0x21:
        return pic_mask[0];
    case 0xA1:
        return pic_mask[1];
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
        return vpc_pit_inb(port);
    case 0x201:
        return 0xFF;            /* no joystick: the one-shots never time out */
    }
    if (port >= 0x3F8 && port <= 0x3FF)
        return com1[port - 0x3F8];
    return 0xFF;
}

void xn_outb(unsigned int port, unsigned char v)
{
    vpc_poll();
    port &= 0xFFFF;
    if (port >= 0x3C0 && port <= 0x3DF) {
        vpc_video_outb(port, v);
        return;
    }
    switch (port) {
    case 0x60:
    case 0x61:
    case 0x64:
        vpc_kbd_outb(port, v);
        return;
    case 0x20:
    case 0xA0:
        return;                 /* end of interrupt: delivery already serialises them */
    case 0x21:
        pic_mask[0] = v;
        return;
    case 0xA1:
        pic_mask[1] = v;
        return;
    case 0x40:
    case 0x41:
    case 0x42:
    case 0x43:
        vpc_pit_outb(port, v);
        return;
    case 0x201:
        return;
    }
    if (port >= 0x3F8 && port <= 0x3FF)
        com1[port - 0x3F8] = (port == 0x3FD) ? com1[5] : v;
}

unsigned short xn_inw(unsigned int port)
{
    return (unsigned short)(xn_inb(port) | xn_inb(port + 1) << 8);
}

void xn_outw(unsigned int port, unsigned short v)
{
    xn_outb(port, (unsigned char)v);
    xn_outb(port + 1, (unsigned char)(v >> 8));
}

/* ---- register helpers ----------------------------------------------------------------------- */

#define AX(r) ((unsigned int)(r)->eax & 0xFFFF)
#define AH(r) (((unsigned int)(r)->eax >> 8) & 0xFF)
#define AL(r) ((unsigned int)(r)->eax & 0xFF)
#define BX(r) ((unsigned int)(r)->ebx & 0xFFFF)
#define BL(r) ((unsigned int)(r)->ebx & 0xFF)
#define CX(r) ((unsigned int)(r)->ecx & 0xFFFF)
#define DX(r) ((unsigned int)(r)->edx & 0xFFFF)
#define SET16(reg, v) ((reg) = ((reg) & ~0xFFFFul) | ((unsigned long)(v) & 0xFFFF))
#define SET8L(reg, v) ((reg) = ((reg) & ~0xFFul) | ((unsigned long)(v) & 0xFF))
#define SET8H(reg, v) ((reg) = ((reg) & ~0xFF00ul) | (((unsigned long)(v) & 0xFF) << 8))

static void carry(struct vpc_regs *r, int on)
{
    r->eflags = on ? (r->eflags | VPC_CF) : (r->eflags & ~(unsigned long)VPC_CF);
}

static void unknown(const char *what, struct vpc_regs *r)
{
    fprintf(stderr, "port: %s ax=%04X bx=%04X not implemented\n", what, AX(r), BX(r));
}

/* ---- int 10h: video ------------------------------------------------------------------------- */

void xn_int10(struct vpc_regs *r)
{
    vpc_poll();
    switch (AH(r)) {
    case 0x00:                  /* set mode AL (bit 7: keep the screen) */
        vpc_video_set_mode(AL(r));
        break;
    case 0x02:                  /* set the cursor: DH row, DL column */
        put16(BDA_CURSOR, DX(r));
        break;
    case 0x03:                  /* get the cursor: DX, and CX the cursor's shape */
        SET16(r->edx, port_low_memory[BDA_CURSOR] | port_low_memory[BDA_CURSOR + 1] << 8);
        SET16(r->ecx, 0x0607);
        break;
    case 0x0F:                  /* get mode: AL mode, AH columns, BH page */
        SET8L(r->eax, vpc_video_mode());
        SET8H(r->eax, vpc_video_mode() == 0x13 ? 40 : 80);
        SET8H(r->ebx, 0);
        break;
    case 0x4F:                  /* VESA: none (XnGine stays in mode 13h) */
        SET16(r->eax, 0x014F);
        break;
    default:
        unknown("int 10h", r);
    }
}

/* ---- int 15h, int 2Fh: system ----------------------------------------------------------------- */

void xn_int15(struct vpc_regs *r)
{
    vpc_poll();
    if (AX(r) == 0xC202 || AH(r) == 0xC2) {     /* PS/2 mouse: accepted */
        SET8H(r->eax, 0);
        carry(r, 0);
        return;
    }
    unknown("int 15h", r);
    carry(r, 1);
}

volatile unsigned long vpc_frame_yields;

void xn_int2f(struct vpc_regs *r)
{
    if (AX(r) == 0x1680) {      /* release the time slice: the game's loop does once a frame */
        /* at most one frame a VGA refresh (70 Hz): what a frame was waiting for on a PC is
           done in a fraction of that here, and the rest would only spin (PORT_FRAME_CAP=0
           turns it off) */
        static Uint64 last;
        static int cap = -1;
        Uint64 now = SDL_GetTicksNS(), period = 14285714ull;
        if (cap < 0) {
            const char *e = getenv("PORT_FRAME_CAP");
            cap = !(e != NULL && strcmp(e, "0") == 0);
        }
        vpc_frame_yields++;
        vpc_poll();
        if (cap && last != 0 && now - last < period)
            SDL_DelayNS(period - (now - last));
        else
            SDL_DelayNS(100000);
        last = SDL_GetTicksNS();
        SET8L(r->eax, 0);
        return;
    }
    unknown("int 2Fh", r);
}

/* ---- int 16h: the keyboard BIOS ------------------------------------------------------------------ */

void xn_int16(struct vpc_regs *r)
{
    int k;

    switch (AH(r)) {
    case 0x00:                  /* wait for a key */
    case 0x10:
        while ((k = vpc_bios_key(1)) < 0) {
            vpc_poll();
            SDL_DelayNS(1000000);
        }
        SET16(r->eax, k);
        break;
    case 0x01:                  /* is a key waiting? ZF set when not */
    case 0x11:
        vpc_poll();
        k = vpc_bios_key(0);
        if (k < 0) {
            r->eflags |= VPC_ZF;
        } else {
            r->eflags &= ~(unsigned long)VPC_ZF;
            SET16(r->eax, k);
        }
        break;
    case 0x02:                  /* the shift flags */
    case 0x12:
        vpc_poll();
        SET8L(r->eax, port_low_memory[0x417]);
        break;
    default:
        unknown("int 16h", r);
    }
}

/* ---- int 21h: DOS -------------------------------------------------------------------------------- */

#define DOS_ERR_NOT_FOUND 2
#define DOS_ERR_NO_MORE_FILES 18

/* a DOS file name passed in a register: DS is the flat model's, so the register holds a host
   address (struct vpc_regs is pointer-wide) */
static const char *dos_name(unsigned long reg)
{
    return (const char *)(uintptr_t)reg;
}

void port_dos_find(int first, const char *pattern, unsigned int attr, unsigned char *dta);

void xn_int21(struct vpc_regs *r)
{
    int fd, n;

    vpc_poll();
    carry(r, 0);
    switch (AH(r)) {
    case 0x09: {                /* print DS:EDX up to '$' */
        const char *s = dos_name(r->edx);
        while (*s != '$')
            fputc(*s++, stderr);
        break;
    }
    case 0x3C:                  /* create DS:EDX (CX attributes) */
        fd = port_open(dos_name(r->edx), 0x0002 | 0x0020 | 0x0040 | 0x0200, 0666);
        if (fd < 0) {
            SET16(r->eax, 3);   /* path not found */
            carry(r, 1);
        } else {
            SET16(r->eax, fd);
        }
        break;
    case 0x3D:                  /* open DS:EDX, AL access */
        fd = port_open(dos_name(r->edx), (AL(r) & 3) | 0x0200);
        if (fd < 0) {
            SET16(r->eax, DOS_ERR_NOT_FOUND);
            carry(r, 1);
        } else {
            SET16(r->eax, fd);
        }
        break;
    case 0x3E:                  /* close BX */
        if (port_close((int)BX(r)) != 0) {
            SET16(r->eax, 6);
            carry(r, 1);
        }
        break;
    case 0x3F:                  /* read ECX bytes from BX to DS:EDX: EAX read */
        n = port_read((int)BX(r), (void *)(uintptr_t)r->edx, (unsigned int)r->ecx);
        if (n < 0) {
            SET16(r->eax, 5);
            carry(r, 1);
        } else {
            r->eax = (unsigned int)n;
        }
        break;
    case 0x40:                  /* write ECX bytes from DS:EDX to BX */
        n = port_write((int)BX(r), (const void *)(uintptr_t)r->edx, (unsigned int)r->ecx);
        if (n < 0) {
            SET16(r->eax, 5);
            carry(r, 1);
        } else {
            r->eax = (unsigned int)n;
        }
        break;
    case 0x42: {                /* seek BX to CX:DX from AL: DX:AX the position */
        int pos = port_lseek((int)BX(r), (int)(CX(r) << 16 | DX(r)), (int)AL(r));
        if (pos < 0) {
            SET16(r->eax, 25);
            carry(r, 1);
        } else {
            SET16(r->eax, pos & 0xFFFF);
            SET16(r->edx, (unsigned int)pos >> 16);
        }
        break;
    }
    case 0x4C:                  /* exit with AL */
        port_exit((int)AL(r));
    case 0x4E:                  /* find first DS:EDX with CX attributes, into the DTA */
    case 0x4F:                  /* find next */
        port_dos_find(AH(r) == 0x4E, AH(r) == 0x4E ? dos_name(r->edx) : NULL, CX(r),
                      port_low_memory + LOW_DTA);
        if (port_low_memory[LOW_DTA + 0x1E] == 0) {
            SET16(r->eax, DOS_ERR_NO_MORE_FILES);
            carry(r, 1);
        }
        break;
    case 0x2F:                  /* the DTA: ES:EBX (a host address in EBX) */
        r->ebx = (unsigned long)(uintptr_t)(port_low_memory + LOW_DTA);
        break;
    case 0x51:                  /* the PSP: BX a selector for it */
    case 0x62:
        SET16(r->ebx, port_sel_new((unsigned long)(uintptr_t)(port_low_memory + LOW_PSP)));
        break;
    default:
        unknown("int 21h", r);
        carry(r, 1);
    }
}

/* ---- int 31h: DPMI -------------------------------------------------------------------------------- */

static vpc_handler exception_vec[32];

void xn_int31(struct vpc_regs *r)
{
    vpc_poll();
    carry(r, 0);
    switch (AX(r)) {
    case 0x0002:                /* a selector for real-mode segment BX */
        SET16(r->eax, port_sel_new((unsigned long)(uintptr_t)(port_low_memory + (BX(r) << 4))));
        break;
    case 0x0006: {              /* the base of selector BX: CX:DX (its low 32 bits) */
        unsigned long b = port_sel_base((unsigned short)BX(r));
        SET16(r->ecx, (unsigned int)(b >> 16));
        SET16(r->edx, (unsigned int)b);
        break;
    }
    case 0x0100: {              /* BX paragraphs of DOS memory: AX segment, DX selector */
        static unsigned int next_seg = 0x2000;
        unsigned int seg = next_seg;
        if (seg + BX(r) > 0x9F00) {
            SET16(r->eax, 8);
            SET16(r->ebx, 0x9F00 - seg);
            carry(r, 1);
            break;
        }
        next_seg += BX(r);
        SET16(r->eax, seg);
        SET16(r->edx, port_sel_new((unsigned long)(uintptr_t)(port_low_memory + (seg << 4))));
        break;
    }
    case 0x0101:                /* free DOS memory (it stays: low memory is one block) */
        break;
    case 0x0202:                /* exception BL's handler: CX:EDX (EDX the handler) */
        SET16(r->ecx, 0x0008);
        r->edx = (unsigned long)(uintptr_t)exception_vec[BL(r) & 31];
        break;
    case 0x0203:                /* set it. Natively a divide gives no exception (XnGine's
                                   canonical C checks its divides: quirk Q-SYS-01) */
        exception_vec[BL(r) & 31] = (vpc_handler)(uintptr_t)r->edx;
        break;
    case 0x0300: {              /* a real-mode interrupt BL with the call block at ES:EDI:
                                   VESA (int 10h 4Fxxh) and the helmet's int 33h, neither here */
        unsigned char *rm = (unsigned char *)(uintptr_t)r->edi;
        if (BL(r) == 0x10) {
            rm[28] = 0x4F;      /* EAX = 014Fh: the VBE call failed */
            rm[29] = 0x01;
        }
        break;
    }
    case 0x0600:                /* lock / unlock: nothing pages out */
    case 0x0601:
    case 0x0602:
    case 0x0603:
        break;
    case 0x0800:                /* map physical memory (VESA's frame buffer): none */
        SET16(r->eax, 0x8021);
        carry(r, 1);
        break;
    case 0xFF26:                /* CauseWay's DOS transfer buffer */
        break;
    default:
        unknown("int 31h", r);
        carry(r, 1);
    }
}

/* ---- int 33h: the mouse driver (vpc_input.c) ------------------------------------------------------- */

void xn_int33(struct vpc_regs *r)
{
    vpc_poll();
    vpc_mouse_service(r);
}
