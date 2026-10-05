/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00068F5E */
#include "records.h"

struct voice {
    int sample;                 /* 0x00 */
    char pad04[8];
    int len;                    /* 0x0c */
    int len2;                   /* 0x10 */
    char pad14[24];
    int volume;                 /* 0x2c */
    int loop;                   /* 0x30 */
    int rate;                   /* 0x34 */
    int bits;                   /* 0x38 */
    int chans;                  /* 0x3c */
    int pan;                    /* 0x40 */
    int x;                      /* 0x44 */
    char pad48[168];
    int handle;                 /* 0xf0 */
    int prio;                   /* 0xf4 */
    char padf8[4];
    struct record *source;      /* 0xfc: the object the sound comes from */
    char buf[12];               /* 0x100: its x, y, z */
};
extern char D_00175ACC[];        /* __FILE__ */
extern int D_0018DD5C;
extern int D_0018DD60;
extern struct voice sound_channels[3];
extern int D_001A3EFC;
extern int D_001A3F34;
extern unsigned char sound_enabled;
extern void sound_channel_set_source(struct record *, int);
extern void sound_volume_pan(char *, char *, int *, int *, struct record *);
extern void mc_memset(void *, int, int, char *, int, int);
extern short func_000A2460(int, int);
extern int func_000A2504(int, struct voice *);
extern void func_000A2687(int, int);

int sound_play_sample(int sample, int length, struct record *object, int priority)
{
    int i;
    int volume;
    int pan;
    int unused;
    int loop;

    loop = 0;
    if (sound_enabled == 0)
        return -1;
    if (D_0018DD5C == -1)
        return -1;
    if (priority == -1) {
        priority = 127;
        loop = 1;
        i = 3;
        if (D_001A3EFC != 0x12345678)
            return -1;
    } else {
        for (i = 0; i < 3; i++) {
            if (sound_channels[i].handle == 0x12345678)
                break;
            if (func_000A2460(D_0018DD60, sound_channels[i].handle) != 0) {
                sound_channels[i].handle = 0x12345678;
                break;
            }
        }
        if (priority == -2) {
            priority = 127;
            loop = 1;
        }
        if (i == 3) {
            for (i = 0; i < 3; i++) {
                if (sound_channels[i].prio < priority) {
                    func_000A2687(D_0018DD60, sound_channels[i].handle);
                    break;
                }
            }
        }
        if (loop == 0 && i == 3)
            return -1;
    }
    D_001A3F34 = i;
    sound_channel_set_source(object, i);
    if (sound_channels[i].source != 0)
        sound_volume_pan(sound_channels[i].buf, (char *)&sound_channels[i].source->x, &volume, &pan, sound_channels[i].source);
    else
        sound_volume_pan(sound_channels[i].buf, sound_channels[i].buf, &volume, &pan, sound_channels[i].source);
    mc_memset(&sound_channels[i], 0, 240, D_00175ACC, 246, 4);
    sound_channels[i].prio = priority;
    sound_channels[i].sample = sample;
    sound_channels[i].len = length;
    sound_channels[i].volume = (short)volume | ((short)volume << 16);
    sound_channels[i].rate = 11025;
    sound_channels[i].pan = 32768;
    sound_channels[i].x = pan;
    sound_channels[i].loop = loop != 0 ? -1 : 0;
    sound_channels[i].len2 = length;
    sound_channels[i].bits = 8;
    sound_channels[i].chans = 1;
    sound_channels[i].handle = func_000A2504(D_0018DD60, &sound_channels[i]);
    return i;
}
