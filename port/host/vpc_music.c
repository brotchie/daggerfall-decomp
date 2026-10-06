/* vpc_music.c: the virtual PC's FM synthesiser: an OPL3 (port/host/opl3.c) playing into its
   own SDL3 stream at the chip's rate (49716 Hz) on the sound card's device, beside the sound
   effects. The music driver's timer (the song's sequencer) runs inside the stream's callback,
   between samples, so notes land on the sample they are due and only the audio thread writes
   the chip's registers; the game's thread reaches the song state through vpc_music_lock(). */
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "opl3.h"
#include "port_vpc.h"

SDL_AudioDeviceID vpc_audio_device(void);

static struct opl3 chip;
static SDL_AudioStream *mus;
static SDL_Mutex *mus_mutex;
static void (*tick_fn)(void *);
static void *tick_arg;
static double tick_period, tick_due;    /* in samples */
static int16_t buf[2048 * 2];

void vpc_music_lock(void)
{
    if (mus_mutex)
        SDL_LockMutex(mus_mutex);
}

void vpc_music_unlock(void)
{
    if (mus_mutex)
        SDL_UnlockMutex(mus_mutex);
}

/* the chip, for the music driver: write registers only from the tick (or under the lock) */
struct opl3 *vpc_music_chip(void)
{
    return &chip;
}

/* fn(arg) every 1/hz s of the music's own time (0 hz stops it) */
void vpc_music_set_tick(double hz, void (*fn)(void *), void *arg)
{
    vpc_music_lock();
    tick_fn = hz > 0 ? fn : NULL;
    tick_arg = arg;
    tick_period = hz > 0 ? OPL3_RATE / hz : 0;
    tick_due = tick_period;
    vpc_music_unlock();
}

/* render n frames, running the tick where it falls due */
void vpc_music_render(int16_t *out, int n)
{
    while (n > 0) {
        int k = n;
        if (tick_fn != NULL) {
            if (tick_due < 1) {
                tick_fn(tick_arg);
                tick_due += tick_period;
                continue;
            }
            if (k > (int)tick_due)
                k = (int)tick_due;
        }
        opl3_generate(&chip, out, k);
        out += k * 2;
        n -= k;
        tick_due -= k;
    }
}

static void SDLCALL feed(void *userdata, SDL_AudioStream *stream, int additional, int total)
{
    int frames = additional / 4;

    (void)userdata;
    (void)total;
    while (frames > 0) {
        int n = frames > 2048 ? 2048 : frames;
        vpc_music_lock();
        vpc_music_render(buf, n);
        vpc_music_unlock();
        SDL_PutAudioStreamData(stream, buf, n * 4);
        frames -= n;
    }
}

int vpc_music_init(void)
{
    SDL_AudioSpec src = {SDL_AUDIO_S16, 2, OPL3_RATE}, dst;
    SDL_AudioDeviceID dev = vpc_audio_device();

    mus_mutex = SDL_CreateMutex();
    opl3_reset(&chip);
    if (dev == 0 || !SDL_GetAudioDeviceFormat(dev, &dst, NULL))
        return -1;
    mus = SDL_CreateAudioStream(&src, &dst);
    if (mus == NULL)
        return -1;
    SDL_SetAudioStreamGetCallback(mus, feed, NULL);
    if (!SDL_BindAudioStream(dev, mus)) {
        fprintf(stderr, "port: music stream: %s\n", SDL_GetError());
        SDL_DestroyAudioStream(mus);
        mus = NULL;
        return -1;
    }
    return 0;
}

void vpc_music_shutdown(void)
{
    if (mus != NULL)
        SDL_DestroyAudioStream(mus);
    mus = NULL;
}
