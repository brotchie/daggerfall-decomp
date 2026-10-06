/* sos.c: HMI SOS 4.0 for DOS (the June 1995 release: songs are "HMI-MIDISONG061595"), as
   the game calls it: the timer services, the digital driver (sound effects) and the MIDI
   driver (music). FALL.EXE links the library in at 0x9E18C-0xA0AD9 and 0xA1A16-0xA2A2B;
   docs/state.md, "The library region".

   There is no audio yet (docs/port.md, phase 5): each call answers as FALL.EXE's would and
   keeps the state the game looks at (handles, which samples and songs are playing), so the
   game's logic goes on. The timer services are real: SOS's events are the game's clocks
   (xn_timer_tick_callback at 140 Hz counts frame time; the video player's 60 Hz callback),
   and they run on SDL's timer thread, as they ran from the timer interrupt.

   With the install's HMISET.CFG ("No Digital Device", "No MIDI Device": device -1 both)
   the game starts the three systems and the timer event only; the drivers, samples and
   songs are for a configured device.

   Each function returns SOS's error code (0 none). */
#include <stdint.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "port_host.h"

typedef unsigned int W32;

/* SOS's start-sample block (_SOS_START_SAMPLE, 0xF0 bytes under Watcom) and song block (32
   bytes), as the game fills them: these must stay field for field the same as include/
   structs.h's struct sos_sample and struct sos_song (the native layout follows from their
   types: data is a pointer). */
#pragma pack(push, 1)
struct sos_sample {
    char *data;                     /* +0x00: the samples */
    char pad04[8];
    int length;                     /* +0x0C: bytes */
    int pad10;                      /* +0x10: the length again (sound_play_sample) */
    char pad14[24];
    int volume;                     /* +0x2C: left << 16 | right, 0x7FFF each the loudest */
    int loop;                       /* +0x30: -1 loops */
    int rate;                       /* +0x34: samples per second */
    int bits;                       /* +0x38 */
    int channels;                   /* +0x3C */
    int format;                     /* +0x40: 0x8000 8-bit unsigned, 0 16-bit */
    int pan;                        /* +0x44: 0x8000 the centre */
    char pad48[168];
};
struct sos_song {
    char *data;                     /* +0x00: the song (a MIDI.BSA record) */
    char pad04[28];
};
#pragma pack(pop)

/* the error codes as FALL.EXE returns them */
#define SOS_OK 0
#define SOS_ERR_INVALID_HANDLE 0x0A
#define SOS_ERR_NO_HANDLES 0x0B
#define SOS_ERR_INVALID_DATA 0x0E       /* not an HMI-MIDISONG061595 song */
#define SOS_ERR_ALREADY_INIT 0x18       /* sosMIDIInitSystem twice */
#define SOS_ERR_NOT_INIT 0x19           /* no MIDI system, driver, or song not playing */
#define SOS_ERR_NO_TRACKS 0x1A          /* no track of the song has a driver */

/* ---- timer (0x9E1A1-0x9E8FF) ------------------------------------------------------------ */

#define TIMER_EVENTS 16
#define TIMER_DOS_RATE 0xFF00           /* the BIOS's own rate, 1193180 / 65536 Hz */

static struct timer_event {
    int used;
    void (*fn)(void);
    W32 rate;
    Uint64 period_ns, next_ns;
    SDL_TimerID id;
} events[TIMER_EVENTS];
static int timer_installed;

static Uint64 SDLCALL timer_fire(void *userdata, SDL_TimerID id, Uint64 interval)
{
    struct timer_event *e = userdata;
    Uint64 now = SDL_GetTicksNS();
    int n = 0;

    (void)id;
    (void)interval;
    /* as many calls as the rate owes, a few at most when the thread was held up */
    while (e->used && now >= e->next_ns && n++ < 8) {
        void (*fn)(void) = e->fn;
        if (fn != NULL)
            fn();
        e->next_ns += e->period_ns;
    }
    if (!e->used)
        return 0;
    if (now >= e->next_ns)
        e->next_ns = now + e->period_ns;
    return e->next_ns - now;
}

static void timer_start(struct timer_event *e)
{
    if (!timer_installed || e->id != 0)
        return;
    e->next_ns = SDL_GetTicksNS() + e->period_ns;
    e->id = SDL_AddTimerNS(e->period_ns, timer_fire, e);
}

static void timer_stop(struct timer_event *e)
{
    if (e->id != 0) {
        SDL_RemoveTimer(e->id);
        e->id = 0;
    }
}

/* sosTIMERInitSystem(rate, debug) (0x9E1A1): rate 0xFF00 keeps the PIT at the BIOS's rate;
   debug bit 0 leaves the timer interrupt alone (then no event runs). The game: (0xFF00, 0). */
W32 func_0009E1A1(W32 rate, W32 debug)
{
    (void)rate;
    if (!(debug & 1)) {
        int i;
        timer_installed = 1;
        for (i = 0; i < TIMER_EVENTS; i++)
            if (events[i].used)
                timer_start(&events[i]);
    }
    return SOS_OK;
}

/* sosTIMERUnInitSystem(arg) (0x9E281): the timer interrupt goes back to the BIOS; the events
   stay registered but no longer run */
W32 func_0009E281(W32 arg)
{
    int i;

    (void)arg;
    for (i = 0; i < TIMER_EVENTS; i++)
        timer_stop(&events[i]);
    timer_installed = 0;
    return SOS_OK;
}

/* sosTIMERRegisterEvent(rate, callback, &handle) (0x9E2BB): calls callback `rate` times a
   second (0xFF00: the BIOS's rate); the handle is its slot, 0-15 */
W32 func_0009E2BB(W32 rate, void (*fn)(void), W32 *handle)
{
    int i;

    port_check_ptr((const void *)fn, "sosTIMERRegisterEvent's callback");
    port_check_ptr(handle, "sosTIMERRegisterEvent's handle");
    for (i = 0; i < TIMER_EVENTS && events[i].used; i++)
        ;
    if (i == TIMER_EVENTS)
        return SOS_ERR_NO_HANDLES;
    events[i].fn = fn;
    events[i].rate = rate;
    events[i].period_ns = rate == TIMER_DOS_RATE ? 65536ull * 1000000000ull / 1193180ull
                          : rate ? 1000000000ull / rate : 1000000000ull;
    events[i].used = 1;
    timer_start(&events[i]);
    *handle = (W32)i;
    return SOS_OK;
}

/* sosTIMERRemoveEvent(handle) (0x9E61A) */
W32 func_0009E61A(W32 handle)
{
    if (handle < TIMER_EVENTS) {
        events[handle].used = 0;
        timer_stop(&events[handle]);
        events[handle].fn = NULL;
    }
    return SOS_OK;
}

/* ---- digital driver (sound effects) ----------------------------------------------------- */

#define DIGI_DRIVERS 5
#define DIGI_SAMPLES 32                 /* the driver's sample table: 0x1E00 / 0xF0 */

static struct digi_driver {
    void *driver;                       /* the game's driver block (0x18DC38, 268 bytes) */
    struct digi_sample {
        int playing;
        struct sos_sample s;            /* SOS copies the start block into the slot */
        Uint64 end_ns;                  /* without a mixer: when it would have ended */
    } samples[DIGI_SAMPLES];
} digi[DIGI_DRIVERS];

/* sosDIGIInitSystem(driver path, debug) (0x9E8FF): the game passes (0, 0) */
W32 func_0009E8FF(const char *path, W32 debug)
{
    (void)path;
    (void)debug;
    return SOS_OK;
}

/* sosDIGIUnInitSystem() (0x9E95B) */
W32 func_0009E95B(void)
{
    return SOS_OK;
}

/* sosDIGIInitDriver(driver block, &handle) (0x9F4DE): loads the device's driver from
   HMIDRV.386 (device id at +0x104, port, DMA and IRQ at +0x5C/+0x60/+0x64, rate at +0x04,
   DMA buffer size at +0x44), allocates the sample table and fills in the mixer callback at
   +0x108, which the game then registers as a 90 Hz timer event. Here the block stays as the
   game filled it (the callback 0: the event runs nothing). phase 5: SDL audio */
W32 func_0009F4DE(void *driver, W32 *handle)
{
    int i;

    port_check_ptr(driver, "sosDIGIInitDriver's driver");
    port_check_ptr(handle, "sosDIGIInitDriver's handle");
    for (i = 0; i < DIGI_DRIVERS && digi[i].driver != NULL; i++)
        ;
    if (i == DIGI_DRIVERS)
        return SOS_ERR_NO_HANDLES;
    memset(&digi[i], 0, sizeof digi[i]);
    digi[i].driver = driver;
    *handle = (W32)i;
    return SOS_OK;
}

/* sosDIGIUnInitDriver(handle, shut down, free memory) (0x9F9A7) */
W32 func_0009F9A7(W32 h, W32 shutdown, W32 release)
{
    (void)shutdown;
    (void)release;
    if (h >= DIGI_DRIVERS || digi[h].driver == NULL)
        return SOS_ERR_INVALID_HANDLE;
    memset(&digi[h], 0, sizeof digi[h]);
    return SOS_OK;
}

static struct digi_sample *digi_sample(W32 h, W32 hs)
{
    if (h >= DIGI_DRIVERS || digi[h].driver == NULL || hs >= DIGI_SAMPLES)
        return NULL;
    return &digi[h].samples[hs];
}

static int digi_playing(struct digi_sample *s)
{
    if (s->playing && s->end_ns != 0 && SDL_GetTicksNS() >= s->end_ns)
        s->playing = 0;
    return s->playing;
}

/* sosDIGIStartSample(driver, start block) (0xA2504) -> the sample's handle (its slot), -1
   when every slot plays, or 0x0A for a bad driver (FALL.EXE returns the error code as the
   handle). The block (struct sos_sample): data, length, volume (left << 16 | right), loop
   (-1), rate, bits, channels, format, pan. phase 5: SDL audio */
W32 func_000A2504(W32 h, const struct sos_sample *start)
{
    int i;

    if (h >= DIGI_DRIVERS || digi[h].driver == NULL)
        return SOS_ERR_INVALID_HANDLE;
    port_check_ptr(start, "sosDIGIStartSample's sample");
    for (i = 0; i < DIGI_SAMPLES && digi_playing(&digi[h].samples[i]); i++)
        ;
    if (i == DIGI_SAMPLES)
        return 0xFFFFFFFFu;
    {
        struct digi_sample *s = &digi[h].samples[i];
        Uint64 bytes_per_second = (Uint64)(start->rate > 0 ? start->rate : 0) *
                                  (start->channels > 0 ? start->channels : 1) *
                                  (start->bits > 8 ? 2 : 1);
        s->s = *start;
        s->playing = 1;
        if (start->loop != 0)
            s->end_ns = 0;          /* a loop plays until it is stopped */
        else if (bytes_per_second == 0 || start->length <= 0)
            s->end_ns = SDL_GetTicksNS();
        else
            s->end_ns = SDL_GetTicksNS() +
                        (Uint64)start->length * 1000000000ull / bytes_per_second;
    }
    return (W32)i;
}

/* sosDIGISampleDone(driver, sample) (0xA2460): 1 when it has finished, 0 while it plays,
   0x0A for a bad handle */
W32 func_000A2460(W32 h, W32 hs)
{
    struct digi_sample *s = digi_sample(h, hs);

    if (s == NULL)
        return SOS_ERR_INVALID_HANDLE;
    return digi_playing(s) ? 0 : 1;
}

/* sosDIGIStopSample(driver, sample) (0xA2687) */
W32 func_000A2687(W32 h, W32 hs)
{
    struct digi_sample *s = digi_sample(h, hs);

    if (s == NULL)
        return SOS_ERR_INVALID_HANDLE;
    s->playing = 0;                 /* phase 5: SDL audio */
    return SOS_OK;
}

/* sosDIGISetSampleVolume(driver, sample, left << 16 | right) (0xA1ED5) -> the previous
   volume while it plays, else 0x0A */
W32 func_000A1ED5(W32 h, W32 hs, W32 volume)
{
    struct digi_sample *s = digi_sample(h, hs);
    W32 old;

    if (s == NULL || !digi_playing(s))
        return SOS_ERR_INVALID_HANDLE;
    old = (W32)s->s.volume;
    s->s.volume = (int)volume;      /* phase 5: SDL audio */
    return old;
}

/* sosDIGISetPanLocation(driver, sample, pan: 0x8000 the centre) (0xA20BF) -> the previous
   pan while it plays, else 0x0A */
W32 func_000A20BF(W32 h, W32 hs, W32 pan)
{
    struct digi_sample *s = digi_sample(h, hs);
    W32 old;

    if (s == NULL || !digi_playing(s))
        return SOS_ERR_INVALID_HANDLE;
    old = (W32)s->s.pan;
    s->s.pan = (int)pan;            /* phase 5: SDL audio */
    return old;
}

/* ---- MIDI driver (music) ---------------------------------------------------------------- */

#define MIDI_DRIVERS 8
#define MIDI_SONGS 32

static int midi_system;
static W32 midi_master_volume = 0x7F;
static struct {
    int used;
    W32 device;                         /* the device id (0xA002 ... 0xA00A) */
} midi_drv[MIDI_DRIVERS];
static struct {
    int used;
    int ready;                          /* a track has a driver (FALL.EXE: +0x20 bit 0x1000) */
    int playing;                        /* +0x20 bit 0x8000 */
    struct sos_song init;               /* the game's song block: the data at +0 */
} songs[MIDI_SONGS];

/* sosMIDIInitSystem(driver path, debug) (0x9E9C2): the game passes (0, 0) */
W32 func_0009E9C2(const char *path, W32 debug)
{
    (void)path;
    (void)debug;
    if (midi_system)
        return SOS_ERR_ALREADY_INIT;
    memset(midi_drv, 0, sizeof midi_drv);
    memset(songs, 0, sizeof songs);
    midi_master_volume = 0x7F;
    midi_system = 1;
    return SOS_OK;
}

/* sosMIDIUnInitSystem() (0x9EC0A) */
W32 func_0009EC0A(void)
{
    if (!midi_system)
        return SOS_ERR_NOT_INIT;
    midi_system = 0;
    return SOS_OK;
}

static int midi_any_driver(void)
{
    int i;

    for (i = 0; i < MIDI_DRIVERS; i++)
        if (midi_drv[i].used)
            return 1;
    return 0;
}

/* sosMIDIInitDriver(hardware block, &handle) (0x9EC82): the block (0x18DD64, 46 bytes)
   starts with the device id; FALL.EXE loads its driver from HMIMDRV.386 (OPL devices 0xA002
   and 0xA009 also need sosMIDISetInsData's banks). phase 5: SDL audio */
W32 func_0009EC82(const W32 *hardware, W32 *handle)
{
    int i;

    port_check_ptr(hardware, "sosMIDIInitDriver's hardware");
    port_check_ptr(handle, "sosMIDIInitDriver's handle");
    for (i = 0; i < MIDI_DRIVERS && midi_drv[i].used; i++)
        ;
    if (i == MIDI_DRIVERS)
        return SOS_ERR_NO_HANDLES;
    midi_drv[i].used = 1;
    midi_drv[i].device = hardware[0];
    *handle = (W32)i;
    return SOS_OK;
}

/* sosMIDIUnInitDriver(handle, release) (0x9F253) */
W32 func_0009F253(W32 h, W32 release)
{
    (void)release;
    if (h >= MIDI_DRIVERS)
        return SOS_ERR_INVALID_HANDLE;
    if (!midi_drv[h].used)
        return SOS_ERR_NOT_INIT;
    midi_drv[h].used = 0;
    return SOS_OK;
}

/* sosMIDISetInsData(handle, instrument bank, flag) (0x9FEE5): MELODIC.BNK and DRUM.BNK for an
   OPL device; FALL.EXE passes them to the driver and returns its result. phase 5: SDL audio */
W32 func_0009FEE5(W32 h, const void *bank, W32 flag)
{
    (void)h;
    (void)bank;
    (void)flag;
    return SOS_OK;
}

/* sosMIDIInitSong(song block, &handle) (0xA021C): checks the song's signature, takes a slot
   and prepares the tracks for their drivers. With no track on a driver (no MIDI driver) it
   fails with 0x1A and keeps the slot, as FALL.EXE does (after 32 such songs: 0x0B). */
W32 func_000A021C(const struct sos_song *init, W32 *handle)
{
    int i;

    port_check_ptr(init, "sosMIDIInitSong's song");
    port_check_ptr(handle, "sosMIDIInitSong's handle");
    port_check_ptr(init->data, "sosMIDIInitSong's song data");
    if (init->data == NULL || strcmp(init->data, "HMI-MIDISONG061595") != 0)
        return SOS_ERR_INVALID_DATA;
    for (i = 0; i < MIDI_SONGS && songs[i].used; i++)
        ;
    if (i == MIDI_SONGS)
        return SOS_ERR_NO_HANDLES;
    songs[i].used = 1;
    songs[i].init = *init;
    songs[i].playing = 0;
    songs[i].ready = midi_any_driver();
    if (!songs[i].ready)
        return SOS_ERR_NO_TRACKS;
    *handle = (W32)i;
    return SOS_OK;
}

/* sosMIDIUnInitSong(handle) (0xA0517) */
W32 func_000A0517(W32 h)
{
    if (h >= MIDI_SONGS || !songs[h].used)
        return SOS_ERR_INVALID_HANDLE;
    memset(&songs[h], 0, sizeof songs[h]);
    return SOS_OK;
}

/* sosMIDIStartSong(handle) (0xA27A0): FALL.EXE registers the song's sequencer as a timer
   event at the song's rate. phase 5: SDL audio */
W32 func_000A27A0(W32 h)
{
    if (h >= MIDI_SONGS || !songs[h].used)
        return SOS_ERR_INVALID_HANDLE;
    if (!songs[h].ready)
        return SOS_ERR_NOT_INIT;
    songs[h].playing = 1;
    return SOS_OK;
}

/* sosMIDIStopSong(handle) (0xA2857) */
W32 func_000A2857(W32 h)
{
    if (h >= MIDI_SONGS || !songs[h].used)
        return SOS_ERR_INVALID_HANDLE;
    if (!songs[h].ready || !songs[h].playing)
        return SOS_ERR_NOT_INIT;
    songs[h].playing = 0;           /* phase 5: SDL audio */
    return SOS_OK;
}

/* sosMIDISongDone(handle) (0xA2941): 0 while it plays, 1 when it has stopped (music_update
   then starts it again), 0x0A for a bad handle. Without a sequencer a song never ends. */
W32 func_000A2941(W32 h)
{
    if (h >= MIDI_SONGS || !songs[h].used)
        return SOS_ERR_INVALID_HANDLE;
    return songs[h].playing ? 0 : 1;
}

/* sosMIDISetMasterVolume(volume 0-127) (0xA1D3C): FALL.EXE applies it to every song */
W32 func_000A1D3C(W32 volume)
{
    midi_master_volume = volume;    /* phase 5: SDL audio */
    return SOS_OK;
}
