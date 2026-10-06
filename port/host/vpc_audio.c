/* vpc_audio.c: the virtual PC's sound card, as HMI SOS's digital driver uses it
   (port/shim/sos.c): voices that play the game's samples where they lie in memory, mixed
   into one SDL3 output stream at 44.1 kHz.

   A voice plays 8-bit unsigned or 16-bit signed PCM, mono or stereo, at its own rate (the
   game's sounds are 8-bit mono at 11025 Hz), with SOS's volume (left << 16 | right, 0x7FFF
   the loudest) and pan (0x8000 the centre), once or looping. Volume and pan change while it
   plays, as SOS's calls change them. The mixer runs on SDL's audio thread; the voice table is
   under its own lock. Without an audio device every call still answers, and a voice ends when
   its length's time has passed. */
#include <math.h>
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL.h>

#include "port_vpc.h"

#define OUT_RATE 44100
#define VOICES 160

static struct voice {
    int active;
    const unsigned char *data;
    int frames;                 /* sample frames in the data */
    int bits, channels;
    double pos, step;           /* in frames */
    int loop;
    float gain_l, gain_r;
    unsigned int volume, pan;
    Uint64 end_ns;              /* without a device: when it ends */
} voices[VOICES];

static SDL_AudioStream *out;
static SDL_Mutex *mix_mutex;
static float *mixbuf;
static int mixbuf_frames;

static void gains(struct voice *v)
{
    double l = ((v->volume >> 16) & 0xFFFF) / 32767.0, r = (v->volume & 0xFFFF) / 32767.0;
    double p = v->pan / 65535.0;                /* 0 left, 0.5 centre, 1 right */
    double pl = p <= 0.5 ? 1.0 : (1.0 - p) * 2.0, pr = p >= 0.5 ? 1.0 : p * 2.0;

    if (l > 1.0) l = 1.0;
    if (r > 1.0) r = 1.0;
    v->gain_l = (float)(l * pl);
    v->gain_r = (float)(r * pr);
}

static float frame_value(const struct voice *v, int frame, int ch)
{
    int c = v->channels == 2 ? ch : 0;

    if (v->bits == 16) {
        const unsigned char *p = v->data + ((size_t)frame * v->channels + c) * 2;
        return (short)(p[0] | p[1] << 8) / 32768.0f;
    }
    return (v->data[(size_t)frame * v->channels + c] - 128) / 128.0f;
}

static void mix(float *buf, int frames)
{
    int i, f;

    memset(buf, 0, (size_t)frames * 2 * sizeof(float));
    SDL_LockMutex(mix_mutex);
    for (i = 0; i < VOICES; i++) {
        struct voice *v = &voices[i];
        if (!v->active)
            continue;
        for (f = 0; f < frames; f++) {
            int a = (int)v->pos, b = a + 1;
            double t = v->pos - a;
            float l, r;
            if (b >= v->frames)
                b = v->loop ? 0 : a;
            l = (float)(frame_value(v, a, 0) * (1 - t) + frame_value(v, b, 0) * t);
            r = (float)(frame_value(v, a, 1) * (1 - t) + frame_value(v, b, 1) * t);
            buf[f * 2] += l * v->gain_l;
            buf[f * 2 + 1] += r * v->gain_r;
            v->pos += v->step;
            if (v->pos >= v->frames) {
                if (v->loop) {
                    v->pos -= v->frames;
                } else {
                    v->active = 0;
                    break;
                }
            }
        }
    }
    SDL_UnlockMutex(mix_mutex);
    for (f = 0; f < frames * 2; f++) {      /* soft clip: several voices can add up */
        float x = buf[f];
        buf[f] = x > 1.0f ? 1.0f : x < -1.0f ? -1.0f : x;
    }
}

static void SDLCALL feed(void *userdata, SDL_AudioStream *stream, int additional, int total)
{
    int frames = additional / (int)(2 * sizeof(float));

    (void)userdata;
    (void)total;
    if (frames <= 0)
        return;
    if (frames > mixbuf_frames) {
        SDL_free(mixbuf);
        mixbuf = SDL_malloc((size_t)frames * 2 * sizeof(float));
        mixbuf_frames = mixbuf ? frames : 0;
        if (mixbuf == NULL)
            return;
    }
    mix(mixbuf, frames);
    SDL_PutAudioStreamData(stream, mixbuf, frames * 2 * (int)sizeof(float));
}

int vpc_audio_init(void)
{
    SDL_AudioSpec spec = {SDL_AUDIO_F32, 2, OUT_RATE};

    mix_mutex = SDL_CreateMutex();
    out = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, feed, NULL);
    if (out == NULL) {
        fprintf(stderr, "port: no audio device (%s): sounds play silently\n", SDL_GetError());
        return -1;
    }
    SDL_ResumeAudioStreamDevice(out);
    return 0;
}

void vpc_audio_shutdown(void)
{
    if (out != NULL)
        SDL_DestroyAudioStream(out);
    out = NULL;
}

int vpc_audio_play(int voice, const void *data, int bytes, int rate, int bits, int channels,
                   int loop, unsigned int volume, unsigned int pan)
{
    struct voice *v;
    int frame_bytes;

    if (voice < 0 || voice >= VOICES || data == NULL)
        return -1;
    bits = bits == 16 ? 16 : 8;
    channels = channels == 2 ? 2 : 1;
    frame_bytes = bits / 8 * channels;
    if (rate <= 0)
        rate = 11025;
    SDL_LockMutex(mix_mutex);
    v = &voices[voice];
    memset(v, 0, sizeof *v);
    v->data = data;
    v->frames = bytes / frame_bytes;
    v->bits = bits;
    v->channels = channels;
    v->step = (double)rate / OUT_RATE;
    v->loop = loop;
    v->volume = volume;
    v->pan = pan;
    gains(v);
    v->active = v->frames > 0;
    v->end_ns = loop ? 0 : SDL_GetTicksNS() + (Uint64)v->frames * 1000000000ull / (Uint64)rate;
    SDL_UnlockMutex(mix_mutex);
    return 0;
}

void vpc_audio_stop(int voice)
{
    if (voice < 0 || voice >= VOICES)
        return;
    SDL_LockMutex(mix_mutex);
    voices[voice].active = 0;
    SDL_UnlockMutex(mix_mutex);
}

int vpc_audio_playing(int voice)
{
    int on;

    if (voice < 0 || voice >= VOICES)
        return 0;
    SDL_LockMutex(mix_mutex);
    on = voices[voice].active;
    /* no device: the voice ends when its time is up */
    if (on && out == NULL && voices[voice].end_ns != 0 && SDL_GetTicksNS() >= voices[voice].end_ns)
        on = voices[voice].active = 0;
    SDL_UnlockMutex(mix_mutex);
    return on;
}

void vpc_audio_set_volume(int voice, unsigned int volume)
{
    if (voice < 0 || voice >= VOICES)
        return;
    SDL_LockMutex(mix_mutex);
    voices[voice].volume = volume;
    gains(&voices[voice]);
    SDL_UnlockMutex(mix_mutex);
}

void vpc_audio_set_pan(int voice, unsigned int pan)
{
    if (voice < 0 || voice >= VOICES)
        return;
    SDL_LockMutex(mix_mutex);
    voices[voice].pan = pan;
    gains(&voices[voice]);
    SDL_UnlockMutex(mix_mutex);
}
