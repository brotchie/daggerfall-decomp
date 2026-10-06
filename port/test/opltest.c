/* opltest.c: checks the OPL3 model (port/host/opl3.c) against the chip's documented numbers,
   and writes WAV files to listen to.

   usage: opltest [OUTDIR]     PASS/FAIL per check; WAVs in OUTDIR (default .) */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "opl3.h"

static int fails;

static void check(int ok, const char *what)
{
    printf("%s %s\n", ok ? "PASS" : "FAIL", what);
    if (!ok)
        fails++;
}

static void wav(const char *dir, const char *name, const int16_t *s, int frames)
{
    char path[512];
    FILE *f;
    unsigned int bytes = (unsigned int)frames * 4, rate = OPL3_RATE, brate = OPL3_RATE * 4;
    unsigned char h[44] = "RIFF....WAVEfmt \x10\0\0\0\x01\0\x02\0........\x04\0\x10\0data....";

    snprintf(path, sizeof path, "%s/%s", dir, name);
    f = fopen(path, "wb");
    if (f == NULL)
        return;
    memcpy(h + 4, &(unsigned int){36 + bytes}, 4);
    memcpy(h + 24, &rate, 4);
    memcpy(h + 28, &brate, 4);
    memcpy(h + 40, &bytes, 4);
    fwrite(h, 1, 44, f);
    fwrite(s, 4, (size_t)frames, f);
    fclose(f);
}

/* channel 0 of register set 0: modulator offset 0x00, carrier 0x03 */
static void voice(struct opl3 *c, int ch, int mod_tl, int mod_mult, int car_mult, int ar, int dr,
                  int sl, int rr, int egt, int fb, int cnt, int ws)
{
    static const int moff[9] = {0, 1, 2, 8, 9, 10, 16, 17, 18};
    int m = moff[ch], k = m + 3;

    opl3_write(c, 0x20 + m, (uint8_t)(egt << 5 | mod_mult));
    opl3_write(c, 0x20 + k, (uint8_t)(egt << 5 | car_mult));
    opl3_write(c, 0x40 + m, (uint8_t)mod_tl);
    opl3_write(c, 0x40 + k, 0);
    opl3_write(c, 0x60 + m, (uint8_t)(ar << 4 | dr));
    opl3_write(c, 0x60 + k, (uint8_t)(ar << 4 | dr));
    opl3_write(c, 0x80 + m, (uint8_t)(sl << 4 | rr));
    opl3_write(c, 0x80 + k, (uint8_t)(sl << 4 | rr));
    opl3_write(c, 0xE0 + m, (uint8_t)ws);
    opl3_write(c, 0xE0 + k, (uint8_t)ws);
    opl3_write(c, 0xC0 + ch, (uint8_t)(0x30 | fb << 1 | cnt));
}

static void key(struct opl3 *c, int ch, int fnum, int block, int on)
{
    opl3_write(c, 0xA0 + ch, (uint8_t)fnum);
    opl3_write(c, 0xB0 + ch, (uint8_t)((on ? 0x20 : 0) | block << 2 | fnum >> 8));
}

static double crossings_hz(const int16_t *s, int frames)
{
    int i, n = 0;

    for (i = 1; i < frames; i++)
        if (s[(i - 1) * 2] < 0 && s[i * 2] >= 0)
            n++;
    return n * (double)OPL3_RATE / frames;
}

static int peak(const int16_t *s, int from, int to)
{
    int i, p = 0;

    for (i = from; i < to; i++)
        if (abs(s[i * 2]) > p)
            p = abs(s[i * 2]);
    return p;
}

/* the first frame from which the channel stays below level */
static int falls_below(const int16_t *s, int from, int frames, int level)
{
    int i, last = from;

    for (i = from; i < frames; i++)
        if (abs(s[i * 2]) >= level)
            last = i;
    return last;
}

int main(int argc, char **argv)
{
    const char *dir = argc > 1 ? argv[1] : ".";
    struct opl3 c;
    int n = OPL3_RATE * 2;
    int16_t *buf = calloc((size_t)n * 2, sizeof(int16_t));

    /* 1. a pure sine: the modulator silent (TL 63), the carrier at A4. F-number 580 at block 4
          is 580 * 49716 / 2^16 = 440.0 Hz */
    opl3_reset(&c);
    opl3_write(&c, 0x105, 1);
    voice(&c, 0, 63, 1, 1, 15, 0, 0, 15, 1, 0, 0, 0);
    key(&c, 0, 580, 4, 1);
    opl3_generate(&c, buf, OPL3_RATE);
    {
        double hz = crossings_hz(buf + 2000 * 2, OPL3_RATE - 2000);
        int p = peak(buf, 1000, OPL3_RATE);
        printf("  sine: %.2f Hz, peak %d\n", hz, p);
        check(fabs(hz - 440.0) < 1.0, "a carrier at F-number 580, block 4 sounds at 440 Hz");
        check(p > 4000 && p <= 4096, "full level is 13 bits (peak about 4095)");
    }
    wav(dir, "opl_sine440.wav", buf, OPL3_RATE);

    /* 2. multiples: MULT 2 an octave up, MULT 0 an octave down */
    opl3_reset(&c);
    opl3_write(&c, 0x105, 1);
    voice(&c, 0, 63, 1, 2, 15, 0, 0, 15, 1, 0, 0, 0);
    key(&c, 0, 580, 4, 1);
    opl3_generate(&c, buf, OPL3_RATE);
    check(fabs(crossings_hz(buf + 4000, OPL3_RATE - 2000) - 880.0) < 2.0, "MULT 2 doubles it");

    /* 3. the envelope: the datasheet's decay from 0 to 96 dB takes 39.3 s at rate 4 and halves
          every 4 rates. DR 8 at block 4 (key scale 9, shifted down twice without KSR: +2) is
          rate 34: 307 ms * 4 / 6 = 205 ms to 96 dB, about 0.10 s to -48 dB */
    opl3_reset(&c);
    opl3_write(&c, 0x105, 1);
    voice(&c, 0, 63, 1, 1, 15, 8, 15, 15, 1, 0, 0, 0);
    key(&c, 0, 580, 4, 1);
    opl3_generate(&c, buf, n);
    {
        int t = falls_below(buf, 0, n, 4095 / 256);  /* -48 dB */
        double secs = (double)t / OPL3_RATE;
        printf("  decay DR 8 to -48 dB: %.2f s\n", secs);
        check(secs > 0.08 && secs < 0.13, "decay at DR 8 (rate 34) reaches -48 dB in about 0.10 s");
    }

    /* 4. attack: AR 15 is immediate, AR 4 takes tens of milliseconds */
    opl3_reset(&c);
    opl3_write(&c, 0x105, 1);
    voice(&c, 0, 63, 1, 1, 4, 0, 0, 15, 1, 0, 0, 0);
    key(&c, 0, 580, 4, 1);
    opl3_generate(&c, buf, OPL3_RATE);
    {
        int i, t = -1;
        for (i = 0; i < OPL3_RATE && t < 0; i++)
            if (abs(buf[i * 2]) > 3900)
                t = i;
        printf("  attack AR 4: %.1f ms\n", t * 1000.0 / OPL3_RATE);
        check(t > 0 && t * 1000.0 / OPL3_RATE > 190 && t * 1000.0 / OPL3_RATE < 280,
              "attack AR 4 (rate 18) reaches full level in about 235 ms (the datasheet: 353 * 4 / 6)");
    }

    /* 5. sustain and release: SL 4 holds at -12 dB with EG-TYP, then RR 8 after key-off */
    opl3_reset(&c);
    opl3_write(&c, 0x105, 1);
    voice(&c, 0, 63, 1, 1, 15, 10, 4, 8, 1, 0, 0, 0);
    key(&c, 0, 580, 4, 1);
    opl3_generate(&c, buf, OPL3_RATE);
    {
        int p = peak(buf, OPL3_RATE / 2, OPL3_RATE);
        printf("  sustain SL 4: peak %d (-12 dB is %d)\n", p, (int)(4095 * pow(10, -12 / 20.0)));
        check(p > 950 && p < 1150, "SL 4 sustains at -12 dB");
        key(&c, 0, 580, 4, 0);
        opl3_generate(&c, buf, OPL3_RATE);
        check(peak(buf, OPL3_RATE / 2, OPL3_RATE) < 40, "key-off releases (RR 8)");
    }

    /* 6. FM: with the modulator at full level the output is no longer a pure tone, and stays
          within 13 bits; additive (CNT 1) adds the two */
    opl3_reset(&c);
    opl3_write(&c, 0x105, 1);
    voice(&c, 0, 0, 1, 1, 15, 0, 0, 15, 1, 5, 0, 0);
    key(&c, 0, 580, 4, 1);
    opl3_generate(&c, buf, OPL3_RATE);
    check(peak(buf, 1000, OPL3_RATE) <= 4096 && fabs(crossings_hz(buf + 2000, OPL3_RATE - 1000) - 440) > 5,
          "FM with feedback changes the waveform, within 13 bits");

    /* 7. OPL3 stereo: channel 1 left only */
    opl3_reset(&c);
    opl3_write(&c, 0x105, 1);
    voice(&c, 1, 63, 1, 1, 15, 0, 0, 15, 1, 0, 0, 0);
    opl3_write(&c, 0xC1, 0x10);
    key(&c, 1, 580, 4, 1);
    opl3_generate(&c, buf, 4000);
    {
        int i, l = 0, r = 0;
        for (i = 1000; i < 4000; i++) {
            if (abs(buf[i * 2]) > l) l = abs(buf[i * 2]);
            if (abs(buf[i * 2 + 1]) > r) r = abs(buf[i * 2 + 1]);
        }
        check(l > 4000 && r == 0, "OPL3: C0h bit 4 alone sends a channel left only");
    }

    /* 8. tremolo: AM with DAM swings the level by 4.8 dB at 3.7 Hz */
    opl3_reset(&c);
    opl3_write(&c, 0x105, 1);
    opl3_write(&c, 0xBD, 0x80);
    voice(&c, 0, 63, 1, 1, 15, 0, 0, 15, 1, 0, 0, 0);
    opl3_write(&c, 0x23, 0xA1);          /* carrier: AM, EG-TYP, MULT 1 */
    key(&c, 0, 580, 4, 1);
    opl3_generate(&c, buf, OPL3_RATE);
    {
        int i, lo = 99999, hi = 0;
        for (i = 0; i < OPL3_RATE - 600; i += 600) {
            int p = peak(buf, i, i + 600);
            if (p < lo) lo = p;
            if (p > hi) hi = p;
        }
        printf("  tremolo: %d..%d (%.1f dB)\n", lo, hi, 20 * log10((double)hi / lo));
        check(20 * log10((double)hi / lo) > 4.0 && 20 * log10((double)hi / lo) < 5.5,
              "tremolo depth 4.8 dB with DAM");
    }

    /* 9. a C major scale, and an FM bell (mult 1 : 3.5-ish via 7:2, feedback), for the ear */
    {
        static const int fnums[8] = {345, 387, 435, 460, 517, 580, 651, 690};   /* C4..C5, block 4 */
        int i, len = OPL3_RATE / 3;
        int16_t *song = calloc((size_t)len * 9 * 2, sizeof(int16_t));
        opl3_reset(&c);
        opl3_write(&c, 0x105, 1);
        voice(&c, 0, 20, 1, 1, 13, 3, 3, 6, 1, 3, 0, 0);
        for (i = 0; i < 8; i++) {
            key(&c, 0, fnums[i], 4, 1);
            opl3_generate(&c, song + (size_t)i * len * 2, len * 2 / 3);
            key(&c, 0, fnums[i], 4, 0);
            opl3_generate(&c, song + (size_t)i * len * 2 + (size_t)(len * 2 / 3) * 2, len - len * 2 / 3);
        }
        wav(dir, "opl_scale.wav", song, len * 8);
        opl3_reset(&c);
        opl3_write(&c, 0x105, 1);
        voice(&c, 0, 16, 7, 2, 15, 4, 15, 4, 0, 2, 0, 0);
        key(&c, 0, 517, 4, 1);
        opl3_generate(&c, song, OPL3_RATE * 2);
        wav(dir, "opl_bell.wav", song, OPL3_RATE * 2);
        free(song);
    }

    printf("%s: %d failed\n", fails ? "FAIL" : "PASS", fails);
    free(buf);
    return fails ? 1 : 0;
}
