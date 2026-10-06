/* hmi_opl.c: HMI SOS 4.0's OPL MIDI drivers, fmmidi3.com (device 0xA009, a Sound Blaster
   16's OPL3) and fmmidi.com (0xA002, OPL2), as the native build's music driver. They write
   the same registers with the same values as the originals in HMIMDRV.386 do. Port of the
   reference model build/port/agents/opl/hmi_opl_ref.py (static analysis of the drivers;
   addresses below are offsets in a driver's image).

   As the game did, the driver comes from the install's HMIMDRV.386 at run time: the driver's
   image is its memory here too, so its tables (F-numbers, the volume curve, the operator
   offsets), its initial state and its quirks are the original's:
   - 9 voices; on 0xA009 each write goes to both register sets, the set 0 voice heard on one
     side (C0h CHB) and the set 1 voice on the other (CHA), CC10 pan making one quieter;
   - voice allocation: a free voice, else the lowest voice of the lowest channel that never
     had a pitch bend, else the channel's own number;
   - TL = ((0x2000 - a * (64 - TL)) >> 7) with a from the volume table at the scaled
     velocity (channel volume * velocity); KSL kept from the instrument;
   - drums on MIDI channel 9: DRUM.BNK[note], at the pitch in byte 2 of its name record;
   - the banks converted in place (125 of 128 instruments: 125-127 stay raw and silent);
   - the 0x80 shadow overlapping the A0 shadow, the OPL3 bend-down range read by voice,
     note 0 looking like a free voice, CC121's velocity reset indexed by channel. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hmi_opl.h"

struct addrs {
    unsigned code, size;
    unsigned program, owner, bend, pbactive, brange, volume, vel, volflag, sustain, pan,
        panflag, suscount, susbuf, drum_loaded, bend_enable, toggle, port, sh40, sh80, shA0,
        shB0, shBD, insloaded, note, inited, opoff, voltab, fbase, ftab12, wrap_down, half;
};

static const struct addrs opl3_addrs = {
    0x3218, 0x3850, 0x2C38, 0x2C78, 0x2C9C, 0x2CDC, 0x2D1C, 0x2D60, 0x2DA0, 0x2DC4, 0x2E04,
    0x2E68, 0x2EA8, 0x2EE8, 0x2F28, 0x2C2C, 0x2C34, 0x352C, 0x3530, 0x3548, 0x3568, 0x3578,
    0x3581, 0x359C, 0x359D, 0x35A8, 0x35B7, 0x35B9, 0x35D0, 0x35E4, 0x3614, 0x37AC, 0x37B0};

static const struct addrs opl2_addrs = {
    0x005C, 0x318C, 0x25E4, 0x2624, 0x2648, 0x2688, 0x26C8, 0x270C, 0x274C, 0x2770, 0x27B0,
    0, 0, 0x280C, 0x284C, 0x25D8, 0x25E0, 0x2E58, 0x2E5C, 0x2E74, 0x2E94, 0x2EA4, 0x2EAD,
    0x2EC8, 0x2EC9, 0x2ED4, 0x2EE3, 0x2EE5, 0x2F00, 0x2F1C, 0x2F4C, 0x30E4, 0x30E8};

#define A (d->a)

/* ---- the driver's memory -------------------------------------------------------------- */

static unsigned r8(const struct hmi_opl *d, unsigned off)
{
    return off < d->size ? d->m[off] : 0;       /* past the image: undefined in the original */
}

static void w8(struct hmi_opl *d, unsigned off, unsigned v)
{
    if (off < d->size)
        d->m[off] = (unsigned char)v;
}

static unsigned r32(const struct hmi_opl *d, unsigned off)
{
    if (off > d->size || d->size - off < 4)
        return 0;
    return d->m[off] | d->m[off + 1] << 8 | d->m[off + 2] << 16 | (unsigned)d->m[off + 3] << 24;
}

static void w32(struct hmi_opl *d, unsigned off, unsigned v)
{
    w8(d, off, v & 0xFF);
    w8(d, off + 1, (v >> 8) & 0xFF);
    w8(d, off + 2, (v >> 16) & 0xFF);
    w8(d, off + 3, v >> 24);
}

static unsigned g(const struct hmi_opl *d, unsigned base, unsigned i)
{
    return r32(d, base + 4 * i);
}

static void s(struct hmi_opl *d, unsigned base, unsigned i, unsigned v)
{
    w32(d, base + 4 * i, v);
}

static unsigned mod_op(const struct hmi_opl *d, unsigned v)
{
    return r8(d, A->opoff + 2 * v);
}

static unsigned car_op(const struct hmi_opl *d, unsigned v)
{
    return r8(d, A->opoff + 2 * v + 1);
}

/* ---- port writes: register set 0 at the base, set 1 at base + 2 (OPL3 only) ------------- */

static void wr0(struct hmi_opl *d, unsigned reg, unsigned val)
{
    d->out(d->ctx, (reg & 0xFF), val & 0xFF);
}

static void wr1(struct hmi_opl *d, unsigned reg, unsigned val)
{
    d->out(d->ctx, 0x100 | (reg & 0xFF), val & 0xFF);
}

static void wr_both(struct hmi_opl *d, unsigned reg, unsigned val)
{
    wr0(d, reg, val);
    if (d->opl3)
        wr1(d, reg, val);
}

/* ---- loading ------------------------------------------------------------------------------- */

int hmi_opl_load(struct hmi_opl *d, int opl3, const char *hmimdrv_path,
                 void (*out)(void *ctx, unsigned reg, unsigned val), void *ctx)
{
    FILE *f = fopen(hmimdrv_path, "rb");
    long len;

    memset(d, 0, sizeof *d);
    d->opl3 = opl3;
    d->a = opl3 ? &opl3_addrs : &opl2_addrs;
    d->out = out;
    d->ctx = ctx;
    if (f == NULL)
        return -1;
    fseek(f, 0, SEEK_END);
    len = ftell(f);
    if (len < (long)(A->code + A->size)) {
        fclose(f);
        return -1;
    }
    d->size = A->size;
    d->m = malloc(d->size);
    fseek(f, (long)A->code, SEEK_SET);
    if (d->m == NULL || fread(d->m, 1, d->size, f) != d->size) {
        fclose(f);
        free(d->m);
        d->m = NULL;
        return -1;
    }
    fclose(f);
    /* the driver identifies itself by its device id at +5 */
    if ((d->m[5] | d->m[6] << 8) != (opl3 ? 0xA009 : 0xA002)) {
        free(d->m);
        d->m = NULL;
        return -1;
    }
    return 0;
}

void hmi_opl_free(struct hmi_opl *d)
{
    free(d->m);
    free(d->mel);
    free(d->drum);
    d->m = d->mel = d->drum = NULL;
}

/* ---- entry points ------------------------------------------------------------------------------ */

static void silence(struct hmi_opl *d)
{
    unsigned v;

    for (v = 0; v < 9; v++) {
        w8(d, A->shB0 + v, 0);
        wr_both(d, 0xB0 + v, 0);
    }
    for (v = 0; v < 11; v++)
        w8(d, A->insloaded + v, 0);
    w8(d, A->shBD, (r8(d, A->shBD) & 0xC0) | 0xC0);
    wr0(d, 0xBD, r8(d, A->shBD));
}

/* entry 1: Init(port) */
void hmi_opl_init(struct hmi_opl *d, unsigned port)
{
    if (d->opl3) {
        if (port != 0x388 && port != 0x380)
            return;             /* 0x1CEC: only these; the caller ignores the failure */
        w32(d, A->port, port);
        wr1(d, 0x05, 0x01);     /* OPL3 mode */
        wr1(d, 0x04, 0x00);     /* no four-operator channels */
    } else {
        w32(d, A->port, port);
        wr0(d, 0x01, 0x20);     /* waveform select */
    }
    w8(d, A->shBD, 0);
    silence(d);
    w8(d, A->inited, 1);
}

/* entry 2: UnInit */
void hmi_opl_uninit(struct hmi_opl *d)
{
    unsigned v;

    hmi_opl_init(d, r32(d, A->port));
    if (!r8(d, A->inited))
        return;
    if (d->opl3) {
        w8(d, A->shBD, 0);
        wr0(d, 0xBD, 0);
    } else {
        wr0(d, 0xBD, r8(d, A->shBD));
    }
    for (v = 0; v < 9; v++)
        wr_both(d, 0xB0 + v, r8(d, A->shB0 + v) & 0xDF);
    for (v = 0; v < 9; v++)
        wr_both(d, 0x40 + car_op(d, v), 0xFF);
    for (v = 0; v < 11; v++)
        w8(d, A->insloaded + v, 0);
    if (d->opl3)
        wr1(d, 0x05, 0x00);
    w8(d, A->inited, 0);
}

static unsigned bnk32(const unsigned char *b, unsigned off)
{
    return b[off] | b[off + 1] << 8 | b[off + 2] << 16 | (unsigned)b[off + 3] << 24;
}

/* 0xCD6 / 0xD37: an AdLib BNK's instruments as register bytes, in place; byte 2 'H' marks it */
static void convert_bnk(unsigned char *b, size_t len)
{
    int count = (short)(b[8] | b[9] << 8), i;
    unsigned p = bnk32(b, 0x10);

    b[2] = 'H';
    for (i = 0; i < count - 2 && p + 30 <= len; i++, p += 30) {
        b[p + 0x0B] = (unsigned char)(b[p + 0x0B] << 7 | b[p + 0x0C] << 6 | b[p + 0x07] << 5 |
                                      b[p + 0x0D] << 4 | b[p + 0x03]);
        b[p + 0x02] = (unsigned char)(b[p + 0x02] << 6 | b[p + 0x0A]);
        b[p + 0x05] = (unsigned char)(b[p + 0x05] << 4 | b[p + 0x08]);
        b[p + 0x06] = (unsigned char)(b[p + 0x06] << 4 | b[p + 0x09]);
        b[p + 0x0E] = (unsigned char)(b[p + 0x04] * 2 | b[p + 0x0E]);
        b[p + 0x18] = (unsigned char)(b[p + 0x18] << 7 | b[p + 0x19] << 6 | b[p + 0x14] << 5 |
                                      b[p + 0x1A] << 4 | b[p + 0x10]);
        b[p + 0x0F] = (unsigned char)(b[p + 0x0F] << 6 | b[p + 0x17]);
        b[p + 0x12] = (unsigned char)(b[p + 0x12] << 4 | b[p + 0x15]);
        b[p + 0x13] = (unsigned char)(b[p + 0x13] << 4 | b[p + 0x16]);
    }
}

/* entry 4: SetInsData(bank): the first call the melodic bank, the next the drums, alternating.
   The driver keeps the caller's buffer; here a copy */
void hmi_opl_set_ins_data(struct hmi_opl *d, const unsigned char *bank, size_t len)
{
    unsigned char *b = malloc(len);
    unsigned ch;

    if (b == NULL || len < 0x20)
        return;
    memcpy(b, bank, len);
    if (b[2] != 'H')
        convert_bnk(b, len);
    if (r32(d, A->toggle) == 0) {
        w32(d, A->toggle, 1);
        free(d->mel);
        d->mel = b;
        d->mel_len = len;
        for (ch = 0; ch < 9; ch++)
            s(d, A->program, ch, 0);
    } else {
        w32(d, A->toggle, 0);
        free(d->drum);
        d->drum = b;
        d->drum_len = len;
        w32(d, A->drum_loaded, 1);
    }
}

/* ---- instruments ----------------------------------------------------------------------------- */

static const unsigned char *ins_of(const unsigned char *b, size_t len, unsigned i)
{
    static const unsigned char blank[30];
    unsigned off;

    if (b == NULL)
        return blank;
    off = bnk32(b, 0x10) + 30 * i;
    return off + 30 <= len ? b + off : blank;
}

static const unsigned char *mel_ins(const struct hmi_opl *d, unsigned prog)
{
    return ins_of(d->mel, d->mel_len, prog);
}

static const unsigned char *drum_ins(const struct hmi_opl *d, unsigned note)
{
    return ins_of(d->drum, d->drum_len, note);
}

static unsigned drum_pitch(const struct hmi_opl *d, unsigned note)
{
    unsigned off;

    if (d->drum == NULL)
        return 60;
    off = bnk32(d->drum, 0x0C) + 12 * note + 2;
    return off < d->drum_len ? d->drum[off] : 60;
}

/* the CON bit of channel ch's melodic instrument (channel 9's too) */
static unsigned additive(const struct hmi_opl *d, unsigned ch)
{
    return mel_ins(d, g(d, A->program, ch))[0x0E] & 1;
}

/* ---- voices -------------------------------------------------------------------------------- */

static unsigned alloc_voice(const struct hmi_opl *d, unsigned ch)
{
    unsigned v, c;

    for (v = 0; v < 9; v++)
        if (r8(d, A->note + v) == 0)
            return v;
    for (c = 0; c < 16; c++)
        if (g(d, A->pbactive, c) == 0)
            for (v = 0; v < 9; v++)
                if (g(d, A->owner, v) == c)
                    return v;
    return ch >= 9 ? ch - 9 : ch;
}

static void voice_off(struct hmi_opl *d, unsigned v)
{
    unsigned b0;

    if (r8(d, A->note + v) == 0)
        return;
    b0 = r8(d, A->shB0 + v) & 0xDF;
    w8(d, A->shB0 + v, b0);
    wr_both(d, 0xB0 + v, b0);
    w8(d, A->note + v, 0);
}

static void load_ins(struct hmi_opl *d, const unsigned char *ins, unsigned v)
{
    unsigned mo = mod_op(d, v), co = car_op(d, v);

    w8(d, A->sh80 + co, ins[0x13]);
    w8(d, A->sh80 + mo, ins[0x06]);
    wr_both(d, 0x20 + mo, ins[0x0B]);
    wr_both(d, 0x40 + mo, ins[0x02]);
    w8(d, A->sh40 + mo, ins[0x02]);
    wr_both(d, 0x60 + mo, ins[0x05]);
    wr_both(d, 0x80 + mo, ins[0x06]);
    if (d->opl3) {
        unsigned c0 = (ins[0x0E] & 0x1F) | 0x20;
        wr0(d, 0xC0 + v, c0);                   /* set 0: CHB only */
        wr1(d, 0xC0 + v, (c0 & 0xDF) | 0x10);   /* set 1: CHA only */
    } else {
        wr0(d, 0xC0 + v, ins[0x0E]);
    }
    wr_both(d, 0xE0 + mo, ins[0x1C]);
    wr_both(d, 0x20 + co, ins[0x18]);
    w8(d, A->sh40 + co, ins[0x0F]);             /* the carrier's 0x40 is not written here */
    wr_both(d, 0x60 + co, ins[0x12]);
    wr_both(d, 0x80 + co, ins[0x13]);
    wr_both(d, 0xE0 + co, ins[0x1D]);
    w8(d, A->insloaded + v, 1);
}

static unsigned tl(const struct hmi_opl *d, unsigned sh40, unsigned x)
{
    unsigned t = sh40 & 0x3F;
    unsigned a = (0x40 - r8(d, A->voltab + (x >> 1))) * 2;
    unsigned c = (0x2000u - a * (0x40 - t)) >> 7;

    return (c & 0xFF) | (sh40 & 0xC0);
}

static unsigned fnum_of(const struct hmi_opl *d, unsigned note)
{
    return r32(d, A->fbase + 4 * note);
}

static void write_freq_keyon(struct hmi_opl *d, unsigned v, unsigned f)
{
    w8(d, A->shA0 + v, f & 0xFF);
    w8(d, A->shB0 + v, ((f >> 8) | 0x20) & 0xFF);
    wr_both(d, 0xA0 + v, r8(d, A->shA0 + v));
    wr_both(d, 0xB0 + v, r8(d, A->shB0 + v) & 0xDF);
    wr_both(d, 0xB0 + v, r8(d, A->shB0 + v));
    if (!d->opl3)
        w8(d, A->note + v, 1);          /* overwritten by the caller right after */
}

static void write_freq_bend(struct hmi_opl *d, unsigned v, unsigned f)
{
    w8(d, A->shA0 + v, f & 0xFF);
    w8(d, A->shB0 + v, ((f >> 8) | 0x20) & 0xFF);
    wr_both(d, 0xA0 + v, r8(d, A->shA0 + v));
    wr_both(d, 0xB0 + v, r8(d, A->shB0 + v));
}

static unsigned ft(const struct hmi_opl *d, unsigned i)
{
    return r32(d, A->ftab12 + 4 * i);
}

/* 0x1B5F / 0x1A7D: the bent F-number (unsigned 32-bit arithmetic throughout) */
static unsigned bend_calc(const struct hmi_opl *d, unsigned bend, unsigned note, unsigned v)
{
    unsigned n = note - 12, sidx = n % 12;
    unsigned f = ft(d, n), block = f & 0x1C00, fn = f & 0x3FF;
    unsigned owner = g(d, A->owner, v), dd, scale, r;

    if (bend < 0x40) {
        scale = ((0x3F - bend) * 1000) >> 6;
        r = d->opl3 ? g(d, A->brange, v) : g(d, A->brange, owner);  /* OPL3: by voice */
        dd = f - ft(d, n - r);
        if (dd > 0x2CF)
            dd = (fn - r32(d, A->wrap_down + 4 * g(d, A->brange, owner))) & 0x3FF;
        dd = dd * scale / 1000;
        f -= dd;
    } else {
        scale = ((bend - 0x40) * 1000) >> 6;
        r = g(d, A->brange, owner);
        dd = ft(d, n + r) - f;
        if (dd > 0x2CF) {
            block += 0x400;
            fn = r32(d, A->half + 4 * (11 - sidx));
            f = block | fn;
            dd = ft(d, n + r) - f;
        }
        dd = dd * scale / 1000;
        f += dd;
    }
    return f;
}

/* ---- events ---------------------------------------------------------------------------------- */

static void set_pan(struct hmi_opl *d, unsigned ch, unsigned pan);

static void note_on(struct hmi_opl *d, unsigned note, unsigned vel, unsigned ch)
{
    int drum = ch == 9, i;
    unsigned v, mo, co, x, pitch;
    const unsigned char *ins;

    if (ch >= 16)
        return;
    v = alloc_voice(d, ch);
    voice_off(d, v);
    s(d, A->owner, v, ch);
    mo = mod_op(d, v);
    co = car_op(d, v);
    for (i = 0; i < 5; i++) {           /* release rate 15 on both operators */
        unsigned c80 = r8(d, A->sh80 + co) | 0x0F, m80 = r8(d, A->sh80 + mo) | 0x0F;
        if (d->opl3) {
            wr0(d, 0x80 + co, c80);
            wr1(d, 0x80 + co, c80);
            wr0(d, 0x80 + mo, m80);
            wr1(d, 0x80 + mo, m80);
        } else {
            wr0(d, 0x80 + co, c80);
            wr0(d, 0x80 + mo, m80);
        }
    }
    ins = drum ? drum_ins(d, note) : mel_ins(d, g(d, A->program, ch));
    load_ins(d, ins, v);
    s(d, A->vel, v, vel);
    x = (((g(d, A->volume, ch) << 7) / 127 * g(d, A->vel, v)) >> 7) & 0xFF;
    for (i = additive(d, ch) ? 0 : 1; i < 2; i++) {
        unsigned op = i == 0 ? mo : co, val = tl(d, r8(d, A->sh40 + op), x);
        if (d->opl3) {
            wr1(d, 0x40 + op, val);
            wr0(d, 0x40 + op, val);
        } else {
            wr0(d, 0x40 + op, val);
        }
    }
    pitch = drum ? drum_pitch(d, note) : note;
    if (d->opl3) {
        w8(d, A->note + v, note);
        set_pan(d, ch, g(d, A->pan, ch));
        write_freq_keyon(d, v, fnum_of(d, pitch));
    } else {
        write_freq_keyon(d, v, fnum_of(d, pitch));
        w8(d, A->note + v, note);
    }
    if (!drum && r32(d, A->bend_enable) && g(d, A->pbactive, ch))
        write_freq_bend(d, v, bend_calc(d, g(d, A->bend, ch), note, v));
}

static void note_off(struct hmi_opl *d, unsigned note, unsigned vel, unsigned ch)
{
    unsigned v;

    if (ch >= 16)
        return;
    if (g(d, A->sustain, ch) != 0 && g(d, A->suscount, ch) < 0x20) {
        unsigned k = g(d, A->suscount, ch), o = A->susbuf + ch * 0x60 + k * 3;
        w8(d, o, note);
        w8(d, o + 1, vel);
        w8(d, o + 2, ch);
        s(d, A->suscount, ch, k + 1);
        return;
    }
    for (v = 0; v < 9; v++) {
        if (r8(d, A->note + v) == note && g(d, A->owner, v) == ch) {
            voice_off(d, v);
            w8(d, A->note + v, 0);
        }
    }
}

static void set_pan(struct hmi_opl *d, unsigned ch, unsigned pan)
{
    unsigned p, v;

    if (!d->opl3)
        return;
    s(d, A->pan, ch, pan);
    s(d, A->panflag, ch, 1);
    p = pan & 0xFF;
    if (p >= 0x40)
        p = (0x7F - p) & 0xFF;
    p = (p << 1) & 0xFF;
    for (v = 0; v < 9; v++) {
        unsigned x, co;
        int far1;
        if (r8(d, A->note + v) == 0 || g(d, A->owner, v) != ch)
            continue;
        x = ((g(d, A->volume, ch) * g(d, A->vel, v)) >> 7) & 0xFF;
        x = ((x * p) >> 7) & 0xFF;
        far1 = g(d, A->pan, ch) < 0x40;       /* the far side: set 1 when panned to set 0 */
        co = car_op(d, v);
        (far1 ? wr1 : wr0)(d, 0x40 + co, tl(d, r8(d, A->sh40 + co), x));
        if (additive(d, g(d, A->owner, v))) {
            unsigned mo = mod_op(d, v);
            (far1 ? wr1 : wr0)(d, 0x40 + mo, tl(d, r8(d, A->sh40 + mo), x));
        }
    }
}

static void set_volume(struct hmi_opl *d, unsigned ch, unsigned val)
{
    unsigned v;

    if (ch >= 16)
        return;
    s(d, A->volume, ch, val);
    s(d, A->volflag, ch, 1);
    for (v = 0; v < 9; v++) {
        unsigned x, owner;
        int i;
        if (r8(d, A->note + v) == 0 || g(d, A->owner, v) != ch)
            continue;
        x = (((val << 7) / 127 * g(d, A->vel, v)) >> 7) & 0xFF;
        owner = g(d, A->owner, v);
        for (i = additive(d, owner) ? 0 : 1; i < 2; i++) {
            unsigned op = i == 0 ? mod_op(d, v) : car_op(d, v);
            unsigned out = tl(d, r8(d, A->sh40 + op), x);
            if (d->opl3)
                (g(d, A->pan, ch) < 0x40 ? wr0 : wr1)(d, 0x40 + op, out);   /* the near side */
            else
                wr0(d, 0x40 + op, out);
        }
    }
    if (d->opl3)
        set_pan(d, ch, g(d, A->pan, ch));
}

static void controller(struct hmi_opl *d, unsigned ch, unsigned cc, unsigned val)
{
    unsigned v;

    switch (cc) {
    case 7:
        set_volume(d, ch, val);
        break;
    case 10:
        if (d->opl3)
            set_pan(d, ch, val);
        break;
    case 0x40:                          /* sustain: replays the held note-offs, last first */
        s(d, A->sustain, ch, val);
        if (val == 0) {
            while (g(d, A->suscount, ch) != 0) {
                unsigned k = g(d, A->suscount, ch) - 1, o = A->susbuf + ch * 0x60 + k * 3;
                s(d, A->suscount, ch, k);
                note_off(d, r8(d, o), r8(d, o + 1), r8(d, o + 2));
            }
        }
        break;
    case 0x66:                          /* HMI: the pitch-bend range in semitones */
        s(d, A->brange, ch, val);
        break;
    case 0x79:                          /* reset all controllers */
        if (ch < 16) {
            s(d, A->volume, ch, 0x7F);
            s(d, A->vel, ch, 0x7F);     /* the velocity array indexed by channel: as FALL */
            s(d, A->volflag, ch, 0);
            s(d, A->sustain, ch, 0);
            s(d, A->bend, ch, 0x40);
            s(d, A->brange, ch, 2);
        }
        break;
    case 0x7B:                          /* all notes off (sustain ignored) */
        if (ch < 16)
            for (v = 0; v < 9; v++)
                if (g(d, A->owner, v) == ch) {
                    voice_off(d, v);
                    w8(d, A->note + v, 0);
                }
        break;
    }
}

static void pitch_bend(struct hmi_opl *d, unsigned ch, unsigned msb)
{
    unsigned v;

    if (ch >= 16 || ch == 9)
        return;
    s(d, A->bend, ch, msb);
    s(d, A->pbactive, ch, 1);
    for (v = 0; v < 9; v++)
        if (r8(d, A->note + v) != 0 && g(d, A->owner, v) == ch)
            write_freq_bend(d, v, bend_calc(d, msb, r8(d, A->note + v), v));
}

/* entry 0: SendData, one event (no running status) */
void hmi_opl_send(struct hmi_opl *d, unsigned status, unsigned d1, unsigned d2)
{
    unsigned ch = status & 0x0F;

    if (d->m == NULL)
        return;
    switch (status & 0xF0) {
    case 0x90:
        if (d2 != 0)
            note_on(d, d1, d2, ch);
        else
            note_off(d, d1, d2, ch);
        break;
    case 0x80:
        note_off(d, d1, d2, ch);
        break;
    case 0xB0:
        controller(d, ch, d1, d2);
        break;
    case 0xC0:
        s(d, A->program, ch, d1);
        break;
    case 0xE0:
        pitch_bend(d, ch, d2);
        break;
    }
}
