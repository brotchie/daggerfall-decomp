/* hmi_opl.h: HMI SOS 4.0's OPL MIDI drivers (0xA009 OPL3, 0xA002 OPL2) for the native build,
   run from the install's HMIMDRV.386 as the game ran them (port/host/hmi_opl.c). */
#ifndef HMI_OPL_H
#define HMI_OPL_H

#include <stddef.h>

struct addrs;

struct hmi_opl {
    int opl3;
    const struct addrs *a;
    unsigned char *m;               /* the driver's image: its tables and state */
    size_t size;
    unsigned char *mel, *drum;      /* the banks, converted */
    size_t mel_len, drum_len;
    void (*out)(void *ctx, unsigned reg, unsigned val);    /* reg 0x000-0x1FF */
    void *ctx;
};

/* the driver from HMIMDRV.386 (0 on success); out receives each register write */
int hmi_opl_load(struct hmi_opl *d, int opl3, const char *hmimdrv_path,
                 void (*out)(void *ctx, unsigned reg, unsigned val), void *ctx);
void hmi_opl_free(struct hmi_opl *d);

void hmi_opl_init(struct hmi_opl *d, unsigned port);
void hmi_opl_uninit(struct hmi_opl *d);
/* the first call gives the melodic bank, the next the drums (MELODIC.BNK, DRUM.BNK) */
void hmi_opl_set_ins_data(struct hmi_opl *d, const unsigned char *bank, size_t len);
/* one MIDI event: status with its channel, and its data bytes */
void hmi_opl_send(struct hmi_opl *d, unsigned status, unsigned d1, unsigned d2);

#endif
