/* opl3.h: a Yamaha YMF262 (OPL3) FM synthesis chip, written for the native port from the
   chip's documented behaviour (port/host/opl3.c). The register interface is the chip's own:
   the two register sets 0x000-0x0FF and 0x100-0x1FF; OPL2 compatibility until 0x105 bit 0 is
   set. Samples come out at the chip's rate, 49716 Hz (14.31818 MHz / 288), as 16-bit stereo. */
#ifndef OPL3_H
#define OPL3_H

#include <stdint.h>

#define OPL3_RATE 49716

struct opl3_slot {
    /* registers */
    uint8_t am, vib, egt, ksr, mult, ksl, tl, ar, dr, sl, rr, ws;
    /* the phase generator: 20 bits a cycle */
    uint32_t phase;
    /* the envelope generator: 9-bit attenuation in 0.1875 dB steps */
    int eg, eg_state, key;
    uint32_t eg_acc;
    /* the last two outputs (feedback), 13-bit signed */
    int out, prev;
    int ksl_att;
};

struct opl3_chan {
    uint16_t fnum;
    uint8_t block, fb, cnt, out_l, out_r, keyon;
    int pair;                   /* 4-op: 1 the first channel of a pair, 2 the second */
};

struct opl3 {
    struct opl3_slot slot[36];
    struct opl3_chan chan[18];
    uint8_t reg[0x200];
    int opl3_mode;              /* 0x105 bit 0 */
    int nts;                    /* 0x08 bit 6 */
    int dam, dvb, rhythm;       /* 0xBD */
    uint8_t fourop;             /* 0x104 */
    uint32_t noise;
    unsigned tremolo_pos, vibrato_pos;
    unsigned sample;            /* samples since reset, for the LFO clocks */
};

void opl3_reset(struct opl3 *c);
void opl3_write(struct opl3 *c, unsigned reg, uint8_t v);
/* n stereo frames (left, right) at OPL3_RATE */
void opl3_generate(struct opl3 *c, int16_t *out, int n);

#endif
