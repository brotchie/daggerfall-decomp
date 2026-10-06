/* vpc_irq.c: the virtual PC's interrupts (port/include/port_vpc.h). They run on SDL's timer
   thread, one at a time, as one CPU takes them, each under the interrupt lock that port_cli
   holds off:
   - IRQ 0, the PIT at 1193182 / 65536 Hz (18.2 Hz): int 8, by default the BIOS's, which
     counts the tick at 0x46C (and the midnight flag at 0x470) and calls int 1Ch;
   - IRQ 1, the keyboard: int 9 once for each byte the main thread queued (vpc_input.c);
   - HMI SOS's timer events (port/shim/sos.c) take the same lock on the same thread.
   The vector table is what _dos_getvect and _dos_setvect read and write. */
#include <stdio.h>

#include <SDL3/SDL.h>

#include "port_vpc.h"

#define PIT_HZ 1193182.0
#define TICK_NS 54925401ull             /* 65536 / 1193182 s */

static vpc_handler vectors[256];
static SDL_Mutex *irq_mutex;
static SDL_TimerID tick_timer, kbd_timer;
static Uint64 tick_next;
static Uint64 pit_epoch;

void vpc_irq_lock(void)
{
    if (irq_mutex)
        SDL_LockMutex(irq_mutex);
}

void vpc_irq_unlock(void)
{
    if (irq_mutex)
        SDL_UnlockMutex(irq_mutex);
}

/* cli and sti nest; SDL's mutexes are recursive */
static int cli_depth;

void port_cli(void)
{
    vpc_irq_lock();
    cli_depth++;
}

void port_sti(void)
{
    if (cli_depth > 0) {
        cli_depth--;
        vpc_irq_unlock();
    }
}

int port_cli_depth(void)
{
    return cli_depth;
}

/* ---- vectors --------------------------------------------------------------------------- */

static void bios_int1c(void)
{
}

static void bios_int8(void)
{
    unsigned char *t = port_low_memory + 0x46C;
    unsigned int ticks = (unsigned int)(t[0] | t[1] << 8 | t[2] << 16 | (unsigned int)t[3] << 24);
    vpc_handler h;

    if (++ticks >= 0x1800B0) {          /* midnight */
        ticks = 0;
        port_low_memory[0x470] = 1;
    }
    t[0] = (unsigned char)ticks;
    t[1] = (unsigned char)(ticks >> 8);
    t[2] = (unsigned char)(ticks >> 16);
    t[3] = (unsigned char)(ticks >> 24);
    h = vectors[0x1C];
    if (h != NULL)
        h();
}

vpc_handler vpc_getvect(unsigned int intno)
{
    return vectors[intno & 0xFF];
}

void vpc_setvect(unsigned int intno, vpc_handler h)
{
    vpc_irq_lock();
    vectors[intno & 0xFF] = h;
    vpc_irq_unlock();
}

/* Watcom's _dos_getvect and _dos_setvect (FALL.EXE 0xA1272, 0xA12A6): XnGine installs its
   keyboard, joystick, serial and critical-error handlers with them */
vpc_handler _dos_getvect(unsigned int intno)
{
    return vpc_getvect(intno);
}

void _dos_setvect(unsigned int intno, vpc_handler h)
{
    vpc_setvect(intno, h);
}

/* ---- the interrupt thread ---------------------------------------------------------------- */

static Uint64 SDLCALL tick_fire(void *userdata, SDL_TimerID id, Uint64 interval)
{
    Uint64 now = SDL_GetTicksNS();
    int n = 0;

    (void)userdata;
    (void)id;
    (void)interval;
    /* the ticks owed, a few at most after a stall (as the PIC keeps one pending) */
    while (now >= tick_next && n++ < 4) {
        vpc_irq_lock();
        if (vectors[8] != NULL)
            vectors[8]();
        vpc_irq_unlock();
        tick_next += TICK_NS;
    }
    if (now >= tick_next)
        tick_next = now + TICK_NS;
    return tick_next - now;
}

static Uint64 SDLCALL kbd_fire(void *userdata, SDL_TimerID id, Uint64 interval)
{
    (void)userdata;
    (void)id;
    vpc_irq_lock();
    vpc_kbd_irq();
    vpc_irq_unlock();
    return interval;
}

void vpc_irq_init(void)
{
    irq_mutex = SDL_CreateMutex();
    vectors[8] = bios_int8;
    vectors[9] = vpc_bios_int9;
    vectors[0x1C] = bios_int1c;
    pit_epoch = SDL_GetTicksNS();
    tick_next = pit_epoch + TICK_NS;
    tick_timer = SDL_AddTimerNS(TICK_NS, tick_fire, NULL);
    kbd_timer = SDL_AddTimerNS(1000000, kbd_fire, NULL);
}

void vpc_irq_shutdown(void)
{
    if (tick_timer)
        SDL_RemoveTimer(tick_timer);
    if (kbd_timer)
        SDL_RemoveTimer(kbd_timer);
    tick_timer = kbd_timer = 0;
}

/* run fn(arg) every period_ns on the interrupt thread, under the lock */
struct vpc_timer {
    void (*fn)(void *);
    void *arg;
    Uint64 period, next;
    SDL_TimerID id;
};

#define MAX_TIMERS 32
static struct vpc_timer timers[MAX_TIMERS];

static Uint64 SDLCALL timer_fire(void *userdata, SDL_TimerID id, Uint64 interval)
{
    struct vpc_timer *t = userdata;
    Uint64 now = SDL_GetTicksNS();
    int n = 0;

    (void)id;
    (void)interval;
    while (t->fn != NULL && now >= t->next && n++ < 8) {
        vpc_irq_lock();
        t->fn(t->arg);
        vpc_irq_unlock();
        t->next += t->period;
    }
    if (t->fn == NULL)
        return 0;
    if (now >= t->next)
        t->next = now + t->period;
    return t->next - now;
}

unsigned int vpc_start_timer(uint64_t period_ns, void (*fn)(void *), void *arg)
{
    int i;

    for (i = 0; i < MAX_TIMERS; i++) {
        if (timers[i].fn == NULL) {
            timers[i].fn = fn;
            timers[i].arg = arg;
            timers[i].period = period_ns;
            timers[i].next = SDL_GetTicksNS() + period_ns;
            timers[i].id = SDL_AddTimerNS(period_ns, timer_fire, &timers[i]);
            return (unsigned int)i + 1;
        }
    }
    return 0;
}

void vpc_stop_timer(unsigned int handle)
{
    struct vpc_timer *t;

    if (handle == 0 || handle > MAX_TIMERS)
        return;
    t = &timers[handle - 1];
    vpc_irq_lock();
    t->fn = NULL;
    vpc_irq_unlock();
    if (t->id)
        SDL_RemoveTimer(t->id);
    t->id = 0;
}

/* ---- the PIT's ports: counter 0 counts down from 65536 at 1193182 Hz ---------------------- */

static unsigned int pit_latch;
static int pit_latched, pit_flip;
static unsigned int pit_reload = 0x10000;

static unsigned int pit_count(void)
{
    Uint64 ns = SDL_GetTicksNS() - pit_epoch;
    Uint64 counts = (Uint64)(ns * (PIT_HZ / 1e9));
    return (unsigned int)(pit_reload - counts % pit_reload) & 0xFFFF;
}

unsigned char vpc_pit_inb(unsigned int port)
{
    unsigned int v;

    if (port != 0x40)
        return 0xFF;
    v = pit_latched ? pit_latch : pit_count();
    if (!pit_flip) {
        pit_flip = 1;
        return (unsigned char)v;
    }
    pit_flip = 0;
    pit_latched = 0;
    return (unsigned char)(v >> 8);
}

void vpc_pit_outb(unsigned int port, unsigned char v)
{
    static int reload_flip;

    if (port == 0x43) {
        if ((v & 0xC0) == 0 && (v & 0x30) == 0) {       /* latch counter 0 */
            pit_latch = pit_count();
            pit_latched = 1;
            pit_flip = 0;
        }
        reload_flip = 0;
        return;
    }
    if (port == 0x40) {                 /* a new reload, low byte then high */
        if (!reload_flip) {
            pit_reload = (pit_reload & 0xFF00) | v;
            reload_flip = 1;
        } else {
            pit_reload = (pit_reload & 0x00FF) | (unsigned int)v << 8;
            if (pit_reload == 0)
                pit_reload = 0x10000;
            reload_flip = 0;
        }
    }
}
