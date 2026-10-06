/* musictest.c: a Daggerfall song rendered offline through the native build's music path: the
   HMI song player (port/host/hmi_seq.c), HMI's OPL3 driver from the install's HMIMDRV.386
   (port/host/hmi_opl.c) and the OPL3 (port/host/opl3.c), as the game plays it with a Sound
   Blaster 16. Writes a WAV and prints what went out.

   usage: musictest --game DIR [--song NAME.HMI] [--seconds N] [--opl2] [--out FILE.wav]
          musictest --game DIR --all      every song for 20 s: notes, level, silence */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hmi_opl.h"
#include "hmi_seq.h"
#include "opl3.h"

static struct opl3 chip;
static struct hmi_opl drv;
static long n_events, n_notes, n_regs;

static void chip_write(void *ctx, unsigned reg, unsigned val)
{
    (void)ctx;
    n_regs++;
    opl3_write(&chip, reg, (uint8_t)val);
}

static void drv_send(void *ctx, unsigned st, unsigned d1, unsigned d2)
{
    (void)ctx;
    n_events++;
    if ((st & 0xF0) == 0x90 && d2)
        n_notes++;
    hmi_opl_send(&drv, st, d1, d2);
}

static unsigned char *load(const char *dir, const char *name, size_t *len)
{
    char path[1024];
    FILE *f;
    unsigned char *b;

    snprintf(path, sizeof path, "%s/%s", dir, name);
    f = fopen(path, "rb");
    if (f == NULL)
        return NULL;
    fseek(f, 0, SEEK_END);
    *len = (size_t)ftell(f);
    fseek(f, 0, SEEK_SET);
    b = malloc(*len);
    if (fread(b, 1, *len, f) != *len) {
        free(b);
        b = NULL;
    }
    fclose(f);
    return b;
}

/* a record of a BSA archive with named records (MIDI.BSA) */
static unsigned char *bsa_find(unsigned char *d, size_t len, const char *name, size_t *rl,
                               int index, char *found)
{
    unsigned count = d[0] | d[1] << 8, i, pos = 4;
    size_t dir = len - count * 18;

    for (i = 0; i < count; i++) {
        unsigned char *e = d + dir + i * 18;
        unsigned sz = e[14] | e[15] << 8 | e[16] << 16 | (unsigned)e[17] << 24;
        if ((name != NULL && strncasecmp((char *)e, name, 14) == 0) || (int)i == index) {
            *rl = sz;
            if (found)
                snprintf(found, 15, "%.14s", (char *)e);
            return d + pos;
        }
        pos += sz;
    }
    return NULL;
}

static void wav(const char *path, const int16_t *s, long frames)
{
    FILE *f = fopen(path, "wb");
    unsigned bytes = (unsigned)frames * 4, rate = OPL3_RATE, brate = OPL3_RATE * 4, riff = 36 + bytes;
    unsigned char h[44] = "RIFF....WAVEfmt \x10\0\0\0\x01\0\x02\0........\x04\0\x10\0data....";

    if (f == NULL)
        return;
    memcpy(h + 4, &riff, 4);
    memcpy(h + 24, &rate, 4);
    memcpy(h + 28, &brate, 4);
    memcpy(h + 40, &bytes, 4);
    fwrite(h, 1, 44, f);
    fwrite(s, 4, (size_t)frames, f);
    fclose(f);
}

/* render a song for secs seconds; returns the samples (stereo) */
static int16_t *render(unsigned char *song, size_t len, double secs, long *frames_out, int opl2,
                       const char *game)
{
    unsigned char *mel, *drum;
    size_t ml, dl;
    char path[1024];
    struct hmi_song *s;
    int ready;
    long frames = (long)(secs * OPL3_RATE), done = 0;
    int16_t *out = calloc((size_t)frames * 2, sizeof(int16_t));
    double due = 0, per = (double)OPL3_RATE / HMI_TICK_HZ;

    opl3_reset(&chip);
    snprintf(path, sizeof path, "%s/HMIMDRV.386", game);
    if (hmi_opl_load(&drv, !opl2, path, chip_write, NULL) != 0) {
        fprintf(stderr, "no OPL driver in %s\n", path);
        exit(1);
    }
    hmi_set_driver(opl2 ? 0xA002 : 0xA009, drv_send, NULL);
    hmi_opl_init(&drv, 0x388);
    mel = load(game, "MELODIC.BNK", &ml);
    drum = load(game, "DRUM.BNK", &dl);
    hmi_opl_set_ins_data(&drv, mel, ml);
    hmi_opl_set_ins_data(&drv, drum, dl);
    hmi_driver_reset();
    hmi_set_master_volume(127);
    s = hmi_song_new(song, len, &ready);
    if (s == NULL || !ready) {
        fprintf(stderr, "not a playable song\n");
        exit(1);
    }
    hmi_song_start(s);
    while (done < frames) {
        long k;
        if (due < 1) {
            hmi_tick();
            due += per;
            continue;
        }
        k = (long)due;
        if (k > frames - done)
            k = frames - done;
        opl3_generate(&chip, out + done * 2, (int)k);
        done += k;
        due -= (double)k;
    }
    hmi_song_free(s);
    hmi_opl_free(&drv);
    free(mel);
    free(drum);
    *frames_out = frames;
    return out;
}

static void level(const int16_t *s, long frames, int *peak, double *rms, double *silent)
{
    long i, quiet = 0, block = OPL3_RATE / 10;
    double sum = 0;

    *peak = 0;
    for (i = 0; i < frames * 2; i++) {
        int v = abs(s[i]);
        if (v > *peak)
            *peak = v;
        sum += (double)s[i] * s[i];
    }
    for (i = 0; i + block <= frames; i += block) {
        long j;
        int p = 0;
        for (j = i; j < i + block; j++)
            if (abs(s[j * 2]) > p || abs(s[j * 2 + 1]) > p)
                p = abs(s[j * 2]) > abs(s[j * 2 + 1]) ? abs(s[j * 2]) : abs(s[j * 2 + 1]);
        if (p < 64)
            quiet++;
    }
    *rms = sqrt(sum / (double)(frames * 2));
    *silent = (double)quiet / (double)(frames / block);
}

int main(int argc, char **argv)
{
    const char *game = NULL, *song = "D1.HMI", *outp = NULL;
    double secs = 30;
    int opl2 = 0, all = 0, i, peak;
    unsigned char *bsa;
    size_t bl, sl;
    char path[1024], name[16];
    long frames;
    double rms, silent;

    for (i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "--game") && i + 1 < argc) game = argv[++i];
        else if (!strcmp(argv[i], "--song") && i + 1 < argc) song = argv[++i];
        else if (!strcmp(argv[i], "--seconds") && i + 1 < argc) secs = atof(argv[++i]);
        else if (!strcmp(argv[i], "--out") && i + 1 < argc) outp = argv[++i];
        else if (!strcmp(argv[i], "--opl2")) opl2 = 1;
        else if (!strcmp(argv[i], "--all")) all = 1;
    }
    if (game == NULL) {
        fprintf(stderr, "usage: musictest --game DIR [--song NAME.HMI] [--seconds N] [--opl2]"
                        " [--out FILE.wav] | --all\n");
        return 2;
    }
    snprintf(path, sizeof path, "%s/ARENA2", game);
    bsa = load(path, "MIDI.BSA", &bl);
    if (bsa == NULL) {
        fprintf(stderr, "no MIDI.BSA\n");
        return 1;
    }
    if (getenv("SIZES")) {          /* hmi_song_size against MIDI.BSA's record sizes */
        int n = bsa[0] | bsa[1] << 8, bad = 0;
        for (i = 0; i < n; i++) {
            unsigned char *d = bsa_find(bsa, bl, NULL, &sl, i, name);
            if (hmi_song_size(d) != sl) {
                printf("%s: size %zu, record %zu\n", name, hmi_song_size(d), sl);
                bad++;
            }
        }
        printf("song sizes: %d of %d differ\n", bad, n);
        return bad != 0;
    }
    if (all) {
        int n = bsa[0] | bsa[1] << 8, bad = 0;
        for (i = 0; i < n; i++) {
            unsigned char *d = bsa_find(bsa, bl, NULL, &sl, i, name);
            int16_t *o;
            n_events = n_notes = n_regs = 0;
            o = render(d, sl, 20, &frames, opl2, game);
            level(o, frames, &peak, &rms, &silent);
            printf("%-14s notes %5ld events %6ld regs %7ld peak %5d rms %6.0f silent %3.0f%%\n",
                   name, n_notes, n_events, n_regs, peak, rms, silent * 100);
            if (n_notes == 0 || peak < 500)
                bad++;
            free(o);
        }
        printf("%d songs, %d with no notes or no level\n", n, bad);
        return bad ? 1 : 0;
    }
    {
        unsigned char *d = bsa_find(bsa, bl, song, &sl, -1, name);
        int16_t *o;
        if (d == NULL) {
            fprintf(stderr, "no %s in MIDI.BSA\n", song);
            return 1;
        }
        o = render(d, sl, secs, &frames, opl2, game);
        level(o, frames, &peak, &rms, &silent);
        printf("%s: %.0f s, %ld notes, %ld MIDI events, %ld register writes; peak %d, rms %.0f, "
               "silent %.0f%% of 0.1 s blocks\n", name, secs, n_notes, n_events, n_regs, peak, rms,
               silent * 100);
        if (outp)
            wav(outp, o, frames);
        free(o);
    }
    return 0;
}
