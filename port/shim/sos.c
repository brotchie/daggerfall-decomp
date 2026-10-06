/* sos.c: HMI SOS 4.0 for DOS (the June 1995 release: songs are "HMI-MIDISONG061595"), as
   the game calls it: the timer services, the digital driver (sound effects) and the MIDI
   driver (music). FALL.EXE links the library in at 0x9E18C-0xA0AD9 and 0xA1A16-0xA2A2B;
   docs/state.md, "The library region".

   Each call answers as FALL.EXE's would and keeps the state the game looks at (handles,
   which samples and songs are playing). Samples play on the virtual PC's sound card
   (port/host/vpc_audio.c); music on its OPL3, through HMI's own player and OPL driver. The timer services are real: SOS's events are the game's clocks
   (xn_timer_tick_callback at 140 Hz counts frame time; the video player's 60 Hz callback),
   and they run on SDL's timer thread, as they ran from the timer interrupt.

   With the install's HMISET.CFG ("No Digital Device", "No MIDI Device": device -1 both)
   the game starts the three systems and the timer event only; the drivers, samples and
   songs are for a configured device.

   Each function returns SOS's error code (0 none). */
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "port_host.h"
#include "port_vpc.h"
#include "hmi_opl.h"
#include "hmi_seq.h"
#include "opl3.h"

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
        /* under the virtual PC's interrupt lock, as its own interrupts run (port_vpc.h) */
        vpc_irq_lock();
        if (fn != NULL)
            fn();
        vpc_irq_unlock();
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
        struct sos_sample *start;       /* the game's block: its done callback gets it */
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
   game filled it (the callback 0: the event runs nothing); the virtual PC's sound card mixes
   on its own (port/host/vpc_audio.c), a voice for each of the driver's sample slots. */
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
    {
        int i;
        for (i = 0; i < DIGI_SAMPLES; i++)
            vpc_audio_stop((int)h * DIGI_SAMPLES + i);
    }
    memset(&digi[h], 0, sizeof digi[h]);
    return SOS_OK;
}

/* the sound card's voice for a driver's sample slot */
static int digi_voice(const struct digi_sample *s)
{
    int d = 0;

    while (d < DIGI_DRIVERS - 1 && !(s >= digi[d].samples && s < digi[d].samples + DIGI_SAMPLES))
        d++;
    return d * DIGI_SAMPLES + (int)(s - digi[d].samples);
}

static struct digi_sample *digi_sample(W32 h, W32 hs)
{
    if (h >= DIGI_DRIVERS || digi[h].driver == NULL || hs >= DIGI_SAMPLES)
        return NULL;
    return &digi[h].samples[hs];
}

static int digi_playing(struct digi_sample *s)
{
    if (s->playing && !vpc_audio_playing(digi_voice(s)))
        s->playing = 0;
    return s->playing;
}

/* The start block's done callback (+0x5C, natively after the 8-byte data pointer: the video
   player's xn_vid_audio_done_cb): SOS calls it with the block when the sample has played, and
   the callback may put the next buffer in the block (data, length), which plays on at once:
   that is how the movies stream their sound. Without new data the sample has ended. */
struct sos_sample_cb {
    char *data;
    char pad04[8];
    int length;
    int length2;
    char pad14[24];
    int volume, loop, rate, bits, channels, format, pan;
    char pad48[20];
    void (*done)(struct sos_sample *);
};

static void digi_sample_end(int voice, void *arg)
{
    struct digi_sample *s = arg;
    struct sos_sample_cb *b = (struct sos_sample_cb *)s->start;
    char *old = b->data;
    int old_length = b->length;

    if (b->done == NULL) {
        s->playing = 0;
        return;
    }
    b->done(s->start);
    if (b->data != NULL && b->length > 0 && (b->data != old || b->length != old_length)) {
        vpc_audio_continue(voice, b->data, b->length);
        vpc_audio_on_end(voice, digi_sample_end, s);
    } else {
        s->playing = 0;
    }
}

/* sosDIGIStartSample(driver, start block) (0xA2504) -> the sample's handle (its slot), -1
   when every slot plays, or 0x0A for a bad driver (FALL.EXE returns the error code as the
   handle). The block (struct sos_sample): data, length, volume (left << 16 | right), loop
   (-1), rate, bits, channels, format, pan. It plays on the sound card's voice for the slot. */
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
        s->s = *start;
        s->playing = 1;
        port_check_ptr(start->data, "sosDIGIStartSample's data");
        s->start = (struct sos_sample *)start;
        vpc_audio_play(digi_voice(s), start->data, start->length, start->rate,
                       start->bits > 8 ? 16 : 8, start->channels, start->loop != 0,
                       (unsigned int)start->volume, (unsigned int)start->pan);
        if (((const struct sos_sample_cb *)start)->done != NULL)
            vpc_audio_on_end(digi_voice(s), digi_sample_end, s);
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
    s->playing = 0;
    vpc_audio_stop(digi_voice(s));
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
    s->s.volume = (int)volume;
    vpc_audio_set_volume(digi_voice(s), volume);
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
    s->s.pan = (int)pan;
    vpc_audio_set_pan(digi_voice(s), pan);
    return old;
}

/* ---- MIDI driver (music) ---------------------------------------------------------------- */

/* Music plays as FALL.EXE played it on an FM card: HMI's song player (port/host/hmi_seq.c)
   sends MIDI to HMI's own OPL driver, run from the install's HMIMDRV.386 (port/host/
   hmi_opl.c), which writes the registers of the virtual PC's OPL3 (port/host/vpc_music.c).
   The player ticks at 120 Hz inside the music stream; every call here takes its lock. Other
   devices (MPU-401, GUS ...) are accepted and play nothing. */

#define MIDI_DRIVERS 8
#define MIDI_SONGS 32

static int midi_system;
static W32 midi_master_volume = 0x7F;
static struct {
    int used;
    W32 device;                         /* the device id (0xA002 ... 0xA00A) */
    int has_opl;
    struct hmi_opl opl;
} midi_drv[MIDI_DRIVERS];
static struct {
    int used;
    int ready;                          /* a track has a driver (FALL.EXE: +0x20 bit 0x1000) */
    struct hmi_song *song;
} songs[MIDI_SONGS];

static void opl_write(void *ctx, unsigned reg, unsigned val)
{
    (void)ctx;
    opl3_write(vpc_music_chip(), reg, (uint8_t)val);
}

static void opl_send(void *ctx, unsigned st, unsigned d1, unsigned d2)
{
    hmi_opl_send(ctx, st, d1, d2);
}

static void music_tick(void *arg)
{
    (void)arg;
    hmi_tick();
}

/* sosMIDIInitSystem(driver path, debug) (0x9E9C2): the game passes (0, 0) */
W32 func_0009E9C2(const char *path, W32 debug)
{
    (void)path;
    (void)debug;
    if (midi_system)
        return SOS_ERR_ALREADY_INIT;
    vpc_music_lock();
    memset(midi_drv, 0, sizeof midi_drv);
    memset(songs, 0, sizeof songs);
    midi_master_volume = 0x7F;
    hmi_set_master_volume(0x7F);
    midi_system = 1;
    vpc_music_unlock();
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

/* sosMIDIInitDriver(hardware block, &handle) (0x9EC82): the block (0x18DD64, 46 bytes) starts
   with the device id and holds the port at +0x22 (HMI's OPL3 driver sets the chip up only for
   0x388 or 0x380). SOS then sends CC123, CC7 0 and CC121 on every channel but 9. */
W32 func_0009EC82(const W32 *hardware, W32 *handle)
{
    int i;
    char path[1024];

    port_check_ptr(hardware, "sosMIDIInitDriver's hardware");
    port_check_ptr(handle, "sosMIDIInitDriver's handle");
    for (i = 0; i < MIDI_DRIVERS && midi_drv[i].used; i++)
        ;
    if (i == MIDI_DRIVERS)
        return SOS_ERR_NO_HANDLES;
    vpc_music_lock();
    midi_drv[i].used = 1;
    midi_drv[i].device = hardware[0];
    if ((hardware[0] == 0xA009 || hardware[0] == 0xA002) &&
        dos_host_path("HMIMDRV.386", 0, path, sizeof path) &&
        hmi_opl_load(&midi_drv[i].opl, hardware[0] == 0xA009, path, opl_write, NULL) == 0) {
        midi_drv[i].has_opl = 1;
        /* the driver's Init gets the port from block + 0x22 (0x9F0AB), which sos_read_settings
           filled from HMISET.CFG's [MIDI] DevicePort: 0x388 for an SB16 */
        hmi_opl_init(&midi_drv[i].opl, hardware[0x22 / 4] & 0xFFFF);
        hmi_set_driver(hardware[0], opl_send, &midi_drv[i].opl);
        hmi_driver_reset();
        vpc_music_set_tick(HMI_TICK_HZ, music_tick, NULL);
    } else {
        /* another device (MPU-401, GUS ...), or no OPL driver to be had: songs still match
           their tracks to it, and play nothing */
        if (hardware[0] == 0xA009 || hardware[0] == 0xA002)
            fprintf(stderr, "port: no OPL driver in HMIMDRV.386: music is silent\n");
        hmi_set_driver(hardware[0], NULL, NULL);
    }
    vpc_music_unlock();
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
    vpc_music_lock();
    if (midi_drv[h].has_opl) {
        vpc_music_set_tick(0, NULL, NULL);
        hmi_opl_uninit(&midi_drv[h].opl);
        hmi_set_driver(0, NULL, NULL);
        hmi_opl_free(&midi_drv[h].opl);
    }
    memset(&midi_drv[h], 0, sizeof midi_drv[h]);
    vpc_music_unlock();
    return SOS_OK;
}

/* sosMIDISetInsData(handle, instrument bank, flag) (0x9FEE5): MELODIC.BNK then DRUM.BNK for an
   OPL device, handed to the driver. A BNK is its header, 128 names and 128 instruments. */
W32 func_0009FEE5(W32 h, const void *bank, W32 flag)
{
    const unsigned char *b = bank;

    (void)flag;
    if (h >= MIDI_DRIVERS || !midi_drv[h].used)
        return SOS_ERR_INVALID_HANDLE;
    port_check_ptr(bank, "sosMIDISetInsData's bank");
    if (midi_drv[h].has_opl && b != NULL) {
        size_t len = (size_t)(b[0x10] | b[0x11] << 8 | b[0x12] << 16 | (unsigned)b[0x13] << 24) + 128 * 30;
        vpc_music_lock();
        hmi_opl_set_ins_data(&midi_drv[h].opl, b, len);
        vpc_music_unlock();
    }
    return SOS_OK;
}

/* sosMIDIInitSong(song block, &handle) (0xA021C): checks the song's signature, takes a slot
   and matches the tracks to the drivers by their designations. With no track on a driver it
   fails with 0x1A and keeps the slot, as FALL.EXE does (after 32 such songs: 0x0B). */
W32 func_000A021C(const struct sos_song *init, W32 *handle)
{
    int i, ready = 0;

    port_check_ptr(init, "sosMIDIInitSong's song");
    port_check_ptr(handle, "sosMIDIInitSong's handle");
    port_check_ptr(init->data, "sosMIDIInitSong's song data");
    if (init->data == NULL || strcmp(init->data, "HMI-MIDISONG061595") != 0)
        return SOS_ERR_INVALID_DATA;
    for (i = 0; i < MIDI_SONGS && songs[i].used; i++)
        ;
    if (i == MIDI_SONGS)
        return SOS_ERR_NO_HANDLES;
    vpc_music_lock();
    songs[i].used = 1;
    songs[i].song = hmi_song_new((const unsigned char *)init->data,
                                 hmi_song_size((const unsigned char *)init->data), &ready);
    songs[i].ready = midi_any_driver() && songs[i].song != NULL && ready;
    vpc_music_unlock();
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
    vpc_music_lock();
    hmi_song_free(songs[h].song);
    memset(&songs[h], 0, sizeof songs[h]);
    vpc_music_unlock();
    return SOS_OK;
}

/* sosMIDIStartSong(handle) (0xA27A0): FALL.EXE registers the song's sequencer as a 120 Hz
   timer event; here the music stream's tick plays every started song */
W32 func_000A27A0(W32 h)
{
    if (h >= MIDI_SONGS || !songs[h].used)
        return SOS_ERR_INVALID_HANDLE;
    if (!songs[h].ready)
        return SOS_ERR_NOT_INIT;
    vpc_music_lock();
    hmi_song_start(songs[h].song);
    vpc_music_unlock();
    return SOS_OK;
}

/* sosMIDIStopSong(handle) (0xA2857): its notes off, its channels freed, the song rewound */
W32 func_000A2857(W32 h)
{
    if (h >= MIDI_SONGS || !songs[h].used)
        return SOS_ERR_INVALID_HANDLE;
    if (!songs[h].ready || !hmi_song_playing(songs[h].song))
        return SOS_ERR_NOT_INIT;
    vpc_music_lock();
    hmi_song_stop(songs[h].song);
    vpc_music_unlock();
    return SOS_OK;
}

/* sosMIDISongDone(handle) (0xA2941): 0 while it plays, 1 when it has stopped (music_update
   then starts it again), 0x0A for a bad handle. Every game song but FOLK3.HMI loops for ever
   inside the player. */
W32 func_000A2941(W32 h)
{
    int playing;

    if (h >= MIDI_SONGS || !songs[h].used)
        return SOS_ERR_INVALID_HANDLE;
    vpc_music_lock();
    playing = hmi_song_playing(songs[h].song);
    vpc_music_unlock();
    return playing ? 0 : 1;
}

/* sosMIDISetMasterVolume(volume 0-127) (0xA1D3C): CC7 again on every playing channel */
W32 func_000A1D3C(W32 volume)
{
    vpc_music_lock();
    midi_master_volume = volume;
    hmi_set_master_volume(volume);
    vpc_music_unlock();
    return SOS_OK;
}
