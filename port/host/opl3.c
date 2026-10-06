/* opl3.c: a Yamaha YMF262 (OPL3), written for the native port from the chip's documented
   behaviour (port/include/opl3.h). One sample at a time at 49716 Hz, as the chip makes them:

   - Phase: each operator's 20-bit phase advances by (F-number << block) times the multiple
     (0.5, 1, 2 ... 15); vibrato shifts the F-number by up to a sixteenth of it, in 8 steps
     every 1024 samples (6.07 Hz), half as far without 0xBD's DVB.
   - Wave: the phase indexes the waveform as a log-sine (a quarter wave of 256 entries); the
     attenuation adds to it in the log domain and an exponent table turns the sum back into a
     13-bit signed output. The eight OPL3 waveforms (OPL2's first four unless 0x01 bit 5).
   - Envelope: 9 bits of attenuation in 0.1875 dB steps, through attack (exponential), decay to
     the sustain level, sustain (held, or released at the release rate without EG-TYP) and
     release. The rate is 4 * R plus the key scale (block and the top F-number bit, by NTS),
     shifted down twice without KSR; rate 4n + m steps (4 + m) / 4 * 2^(n - 13) a sample.
   - Level: the envelope plus TL (0.75 dB steps), key scale level (0, 1.5, 3 or 6 dB an octave)
     and tremolo (3.7 Hz, 4.8 dB, or 1 dB without DAM).
   - Channels: the modulator's own feedback (the average of its last two outputs, scaled by FB),
     FM (the modulator's output added to the carrier's phase) or additive; OPL3's four-operator
     pairs (0x104) in their four algorithms; the rhythm section (0xBD) with its noise and its
     phase combinations for the hi-hat, snare and cymbal; OPL3's left and right outputs.
   Accuracy is that of a careful model, not of a die analysis: envelope and LFO timing, the
   tables and the mixing follow the chip; rounding at the last bit may differ. */
#include <math.h>
#include <string.h>

#include "opl3.h"

enum { EG_ATTACK, EG_DECAY, EG_SUSTAIN, EG_RELEASE };

static int logsin[256];         /* -log2(sin) of a quarter wave, in 1/256 octave */
static int exptab[256];         /* 4095 * 2^(-i/256) */
static int tables_ready;

static const int mult_x2[16] = {1, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 20, 24, 24, 30, 30};
static const int ksl_rom[16] = {0, 32, 40, 45, 48, 51, 53, 55, 56, 58, 59, 60, 61, 62, 63, 64};
static const int ksl_shift[4] = {16, 1, 2, 0};

/* the slots of a register offset 0x00-0x15 (within a register set); -1 for the gaps */
static const int offset_slot[32] = {
    0, 1, 2, 3, 4, 5, -1, -1, 6, 7, 8, 9, 10, 11, -1, -1,
    12, 13, 14, 15, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

static void init_tables(void)
{
    int i;

    for (i = 0; i < 256; i++) {
        double s = sin((i + 0.5) * M_PI / 512.0);
        logsin[i] = (int)lround(-log2(s) * 256.0);
        exptab[i] = (int)lround(4095.0 * pow(2.0, -i / 256.0));
    }
    tables_ready = 1;
}

/* the slot numbers (0-35) of channel ch (0-17): its modulator and its carrier */
static int mod_slot(int ch)
{
    int b = ch / 9, c = ch % 9;
    return b * 18 + (c % 3) + (c / 3) * 6;
}

static int car_slot(int ch)
{
    return mod_slot(ch) + 3;
}

/* the channel a slot plays in */
static int slot_chan(int s)
{
    int b = s / 18, i = s % 18;
    return b * 9 + (i / 6) * 3 + (i % 3);
}

/* ---- registers --------------------------------------------------------------------------- */

static void update_ksl(struct opl3 *c, int ch)
{
    struct opl3_chan *k = &c->chan[ch];
    int v = (ksl_rom[k->fnum >> 6] << 2) - ((8 - k->block) << 5);
    int i;

    if (v < 0)
        v = 0;
    for (i = 0; i < 2; i++) {
        struct opl3_slot *s = &c->slot[i ? car_slot(ch) : mod_slot(ch)];
        s->ksl_att = s->ksl ? v >> ksl_shift[s->ksl] : 0;
    }
}

static void slot_key(struct opl3_slot *s, int on)
{
    if (on && !s->key) {
        s->eg_state = EG_ATTACK;
        s->phase = 0;
    } else if (!on && s->key) {
        s->eg_state = EG_RELEASE;
    }
    s->key = on;
}

/* the key of a channel's slots, with the rhythm section's own keys and four-operator pairs */
static void update_keys(struct opl3 *c)
{
    int ch;

    for (ch = 0; ch < 18; ch++) {
        struct opl3_chan *k = &c->chan[ch];
        int on = k->keyon;
        if (k->pair == 2)
            on = c->chan[ch - 3].keyon;     /* the pair's first channel keys all four */
        if (c->rhythm && ch >= 6 && ch <= 8) {
            int r = c->reg[0xBD];
            if (ch == 6) {
                slot_key(&c->slot[12], on || (r & 0x10));
                slot_key(&c->slot[15], on || (r & 0x10));
            } else if (ch == 7) {
                slot_key(&c->slot[13], on || (r & 0x01));     /* hi-hat */
                slot_key(&c->slot[16], on || (r & 0x08));     /* snare */
            } else {
                slot_key(&c->slot[14], on || (r & 0x04));     /* tom-tom */
                slot_key(&c->slot[17], on || (r & 0x02));     /* cymbal */
            }
            continue;
        }
        slot_key(&c->slot[mod_slot(ch)], on);
        slot_key(&c->slot[car_slot(ch)], on);
    }
}

static void update_pairs(struct opl3 *c)
{
    static const int first[6] = {0, 1, 2, 9, 10, 11};
    int i;

    for (i = 0; i < 18; i++)
        c->chan[i].pair = 0;
    if (!c->opl3_mode)
        return;
    for (i = 0; i < 6; i++) {
        if (c->fourop & (1 << i)) {
            c->chan[first[i]].pair = 1;
            c->chan[first[i] + 3].pair = 2;
        }
    }
}

void opl3_reset(struct opl3 *c)
{
    int i;

    if (!tables_ready)
        init_tables();
    memset(c, 0, sizeof *c);
    for (i = 0; i < 36; i++) {
        c->slot[i].eg = 511;
        c->slot[i].eg_state = EG_RELEASE;
    }
    for (i = 0; i < 18; i++)
        c->chan[i].out_l = c->chan[i].out_r = 1;
    c->noise = 1;
}

void opl3_write(struct opl3 *c, unsigned reg, uint8_t v)
{
    unsigned bank = (reg >> 8) & 1, r = reg & 0xFF;
    int s, ch;

    c->reg[bank << 8 | r] = v;
    if (bank == 1 && r == 0x05) {
        c->opl3_mode = v & 1;
        update_pairs(c);
        return;
    }
    if (bank == 1 && r == 0x04) {
        c->fourop = v & 0x3F;
        update_pairs(c);
        update_keys(c);
        return;
    }
    if (bank == 0 && r == 0x08) {
        c->nts = (v >> 6) & 1;
        return;
    }
    if (bank == 0 && r == 0xBD) {
        c->dam = (v >> 7) & 1;
        c->dvb = (v >> 6) & 1;
        c->rhythm = (v >> 5) & 1;
        update_keys(c);
        return;
    }
    if (r >= 0x20 && r < 0xA0 && (s = offset_slot[r & 0x1F]) >= 0) {
        struct opl3_slot *o = &c->slot[bank * 18 + s];
        switch (r & 0xE0) {
        case 0x20:
            o->am = (v >> 7) & 1;
            o->vib = (v >> 6) & 1;
            o->egt = (v >> 5) & 1;
            o->ksr = (v >> 4) & 1;
            o->mult = v & 15;
            break;
        case 0x40:
            o->ksl = (v >> 6) & 3;
            o->tl = v & 63;
            update_ksl(c, slot_chan(bank * 18 + s));
            break;
        case 0x60:
            o->ar = v >> 4;
            o->dr = v & 15;
            break;
        case 0x80:
            o->sl = v >> 4;
            o->rr = v & 15;
            break;
        }
        return;
    }
    if (r >= 0xE0 && r <= 0xF5 && (s = offset_slot[r & 0x1F]) >= 0) {
        c->slot[bank * 18 + s].ws = v & 7;
        return;
    }
    if (r >= 0xA0 && r <= 0xA8) {
        ch = bank * 9 + (r - 0xA0);
        c->chan[ch].fnum = (uint16_t)((c->chan[ch].fnum & 0x300) | v);
        update_ksl(c, ch);
        return;
    }
    if (r >= 0xB0 && r <= 0xB8) {
        ch = bank * 9 + (r - 0xB0);
        c->chan[ch].fnum = (uint16_t)((c->chan[ch].fnum & 0xFF) | (v & 3) << 8);
        c->chan[ch].block = (v >> 2) & 7;
        c->chan[ch].keyon = (v >> 5) & 1;
        update_ksl(c, ch);
        update_keys(c);
        return;
    }
    if (r >= 0xC0 && r <= 0xC8) {
        ch = bank * 9 + (r - 0xC0);
        c->chan[ch].fb = (v >> 1) & 7;
        c->chan[ch].cnt = v & 1;
        c->chan[ch].out_l = (v >> 4) & 1;
        c->chan[ch].out_r = (v >> 5) & 1;
        return;
    }
}

/* ---- the generators ------------------------------------------------------------------------- */

/* the F-number and block a slot plays at: a four-operator pair's from its first channel */
static void slot_freq(const struct opl3 *c, int s, int *fnum, int *block)
{
    int ch = slot_chan(s);

    if (c->chan[ch].pair == 2)
        ch -= 3;
    *fnum = c->chan[ch].fnum;
    *block = c->chan[ch].block;
}

static unsigned eg_rate(const struct opl3 *c, const struct opl3_slot *o, int r, int fnum, int block)
{
    int ks, rate;

    if (r == 0)
        return 0;
    ks = block << 1 | ((fnum >> (9 - c->nts)) & 1);
    rate = r * 4 + (o->ksr ? ks : ks >> 2);
    return (unsigned)(rate > 63 ? 63 : rate);
}

/* envelope steps owed this sample at rate r (16.16 fixed point) */
static int eg_steps(struct opl3_slot *o, unsigned rate)
{
    uint32_t inc;

    if (rate == 0)
        return 0;
    inc = (uint32_t)(4 + (rate & 3)) << ((rate >> 2) + 1);
    o->eg_acc += inc;
    {
        int n = (int)(o->eg_acc >> 16);
        o->eg_acc &= 0xFFFF;
        return n;
    }
}

static void envelope(struct opl3 *c, struct opl3_slot *o, int fnum, int block)
{
    int n, sl = o->sl == 15 ? 31 << 4 : o->sl << 4;

    switch (o->eg_state) {
    case EG_ATTACK: {
        unsigned rate = eg_rate(c, o, o->ar, fnum, block);
        if (rate >= 60) {
            o->eg = 0;
        } else {
            n = eg_steps(o, rate);
            while (n-- > 0 && o->eg > 0)
                o->eg -= (o->eg + 8) >> 3;
            if (o->eg < 0)
                o->eg = 0;
        }
        if (o->eg == 0)
            o->eg_state = EG_DECAY;
        break;
    }
    case EG_DECAY:
        o->eg += eg_steps(o, eg_rate(c, o, o->dr, fnum, block));
        if (o->eg >= sl)
            o->eg_state = EG_SUSTAIN;
        break;
    case EG_SUSTAIN:
        if (!o->egt)            /* percussive: on down at the release rate */
            o->eg += eg_steps(o, eg_rate(c, o, o->rr, fnum, block));
        break;
    case EG_RELEASE:
        o->eg += eg_steps(o, eg_rate(c, o, o->rr, fnum, block));
        break;
    }
    if (o->eg > 511)
        o->eg = 511;
}

static void phase_advance(struct opl3 *c, struct opl3_slot *o, int fnum, int block)
{
    int f = fnum;

    if (o->vib) {
        int range = (fnum >> 7) & 7, vp = (int)c->vibrato_pos, d = 0;
        if ((vp & 3) == 2)
            d = range;
        else if (vp & 1)
            d = range >> 1;
        if (!c->dvb)
            d >>= 1;
        f += (vp & 4) ? -d : d;
    }
    o->phase = (o->phase + ((((uint32_t)f << block) >> 1) * (uint32_t)mult_x2[o->mult])) & 0xFFFFF;
}

/* the 13-bit output of waveform ws at phase index i (10 bits) and attenuation level (9 bits) */
static int wave(int ws, int i, int level, int opl3)
{
    int ls, neg = 0, total;

    if (opl3 == 0)
        ws &= 3;
    else if (opl3 < 0)
        ws = 0;                 /* OPL2 without waveform select (0x01 bit 5) */
    i &= 1023;
    switch (ws) {
    case 0:                     /* sine */
        neg = i & 512;
        ls = logsin[(i & 256) ? 255 - (i & 255) : i & 255];
        break;
    case 1:                     /* half sine */
        if (i & 512)
            return 0;
        ls = logsin[(i & 256) ? 255 - (i & 255) : i & 255];
        break;
    case 2:                     /* absolute sine */
        ls = logsin[(i & 256) ? 255 - (i & 255) : i & 255];
        break;
    case 3:                     /* pulse sine: the rising quarters */
        if (i & 256)
            return 0;
        ls = logsin[i & 255];
        break;
    case 4:                     /* alternating sine: a double-rate sine, then silence */
        if (i & 512)
            return 0;
        neg = i & 256;
        ls = logsin[(i & 128) ? 255 - ((i << 1) & 255) : (i << 1) & 255];
        break;
    case 5:                     /* camel: its absolute value */
        if (i & 512)
            return 0;
        ls = logsin[(i & 128) ? 255 - ((i << 1) & 255) : (i << 1) & 255];
        break;
    case 6:                     /* square */
        neg = i & 512;
        ls = 0;
        break;
    default:                    /* derived square: an exponential ramp each half */
        neg = i & 512;
        ls = ((neg ? (1023 - i) : i) & 511) << 3;
        break;
    }
    total = ls + (level << 3);
    if (total >= 0x1FFF)
        return 0;
    {
        int amp = exptab[total & 255] >> (total >> 8);
        return neg ? -amp - 1 : amp;
    }
}

static int slot_level(const struct opl3 *c, const struct opl3_slot *o)
{
    int tremolo = (int)(c->tremolo_pos < 105 ? c->tremolo_pos : 210 - c->tremolo_pos) >> (c->dam ? 2 : 4);
    int level = o->eg + (o->tl << 2) + o->ksl_att + (o->am ? tremolo : 0);

    return level > 511 ? 511 : level;
}

/* a slot's output with phase modulation mod (in phase index units) */
/* wave()'s mode: 1 OPL3, 0 OPL2 with waveform select, -1 OPL2 sine only */
static int wave_mode(const struct opl3 *c)
{
    return c->opl3_mode ? 1 : (c->reg[0x01] & 0x20) ? 0 : -1;
}

static int slot_out(struct opl3 *c, struct opl3_slot *o, int mod)
{
    int out = wave(o->ws, (int)(o->phase >> 10) + mod, slot_level(c, o), wave_mode(c));

    o->prev = o->out;
    o->out = out;
    return out;
}

/* a modulator with its channel's feedback */
static int mod_out(struct opl3 *c, struct opl3_slot *o, int fb)
{
    int fbmod = fb ? (o->out + o->prev) >> (9 - fb) : 0;
    return slot_out(c, o, fbmod);
}

static int channel_out(struct opl3 *c, int ch)
{
    struct opl3_chan *k = &c->chan[ch];
    struct opl3_slot *m = &c->slot[mod_slot(ch)], *s = &c->slot[car_slot(ch)];
    int a;

    if (k->pair == 1) {
        /* four operators: this channel's pair, then the next pair's (CNT of each picks) */
        struct opl3_chan *k2 = &c->chan[ch + 3];
        struct opl3_slot *m2 = &c->slot[mod_slot(ch + 3)], *s2 = &c->slot[car_slot(ch + 3)];
        int o1 = mod_out(c, m, k->fb), o2, o3, o4;
        switch (k->cnt << 1 | k2->cnt) {
        case 0:             /* 1 > 2 > 3 > 4 */
            o2 = slot_out(c, s, o1);
            o3 = slot_out(c, m2, o2);
            return slot_out(c, s2, o3);
        case 2:             /* 1, and 2 > 3 > 4 */
            o2 = slot_out(c, s, 0);
            o3 = slot_out(c, m2, o2);
            return o1 + slot_out(c, s2, o3);
        case 1:             /* 1 > 2, and 3 > 4 */
            o2 = slot_out(c, s, o1);
            o3 = slot_out(c, m2, 0);
            return o2 + slot_out(c, s2, o3);
        default:            /* 1, 2 > 3, and 4 */
            o2 = slot_out(c, s, 0);
            o3 = slot_out(c, m2, o2);
            o4 = slot_out(c, s2, 0);
            return o1 + o3 + o4;
        }
    }
    if (k->pair == 2)
        return 0;           /* played by its first channel */
    a = mod_out(c, m, k->fb);
    if (k->cnt)
        return a + slot_out(c, s, 0);
    return slot_out(c, s, a);
}

/* the rhythm section's channels 6-8 */
static int rhythm_out(struct opl3 *c, int ch)
{
    if (ch == 6)            /* bass drum: channel 6 as a melodic voice, twice as loud */
        return 2 * channel_out(c, 6);
    {
        uint32_t hh = c->slot[13].phase >> 10, tc = c->slot[17].phase >> 10;
        int noise = (int)(c->noise & 1);
        int x = (((hh >> 2) ^ (hh >> 7)) & 1) | (((hh >> 3) ^ (tc >> 5)) & 1) |
                (((tc >> 3) ^ (tc >> 5)) & 1);
        if (ch == 7) {      /* hi-hat and snare */
            int hp = x << 9 | ((x ^ noise) ? 0xD0 : 0x34);
            int b8 = (int)(hh >> 8) & 1;
            int sp = b8 << 9 | (b8 ^ noise) << 8;
            int h = wave(c->slot[13].ws, hp, slot_level(c, &c->slot[13]), wave_mode(c));
            int sd = wave(c->slot[16].ws, sp, slot_level(c, &c->slot[16]), wave_mode(c));
            return 2 * (h + sd);
        } else {            /* tom-tom and cymbal */
            int tom = slot_out(c, &c->slot[14], 0);
            int cp = x << 9 | 0x80;
            int cy = wave(c->slot[17].ws, cp, slot_level(c, &c->slot[17]), wave_mode(c));
            return 2 * (tom + cy);
        }
    }
}

void opl3_generate(struct opl3 *c, int16_t *out, int n)
{
    int f, i;

    for (f = 0; f < n; f++) {
        int l = 0, r = 0;

        /* the LFOs and the noise */
        if ((c->sample & 63) == 0)
            c->tremolo_pos = (c->tremolo_pos + 1) % 210;
        if ((c->sample & 1023) == 0)
            c->vibrato_pos = (c->vibrato_pos + 1) & 7;
        c->sample++;
        {
            uint32_t bit = ((c->noise >> 14) ^ c->noise) & 1;
            c->noise = (c->noise >> 1) | bit << 22;
        }
        /* envelopes and phases: both register sets play in either mode (a YMF262 with
           NEW = 0 still has its 18 channels; only C0h's output bits are ignored) */
        for (i = 0; i < 36; i++) {
            int fnum, block;
            slot_freq(c, i, &fnum, &block);
            envelope(c, &c->slot[i], fnum, block);
            phase_advance(c, &c->slot[i], fnum, block);
        }
        /* the channels */
        for (i = 0; i < 18; i++) {
            struct opl3_chan *k = &c->chan[i];
            int v = (c->rhythm && i >= 6 && i <= 8) ? rhythm_out(c, i) : channel_out(c, i);
            const struct opl3_chan *outk = k->pair == 2 ? &c->chan[i - 3] : k;
            if (!c->opl3_mode || outk->out_l)
                l += v;
            if (!c->opl3_mode || outk->out_r)
                r += v;
        }
        out[f * 2] = (int16_t)(l > 32767 ? 32767 : l < -32768 ? -32768 : l);
        out[f * 2 + 1] = (int16_t)(r > 32767 ? 32767 : r < -32768 ? -32768 : r);
    }
}
