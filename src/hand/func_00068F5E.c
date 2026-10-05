/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00068F5E */
#include "records.h"

extern char D_00175ACC[];        /* __FILE__ */
extern int D_0018DD5C;
extern int D_0018DD60;
extern struct sound_channel sound_channels[];
extern int D_001A3F34;
extern unsigned char sound_enabled;
extern void sound_channel_set_source(struct record *, int);
extern void sound_volume_pan(int *, int *, int *, int *, struct record *);
extern void mc_memset(void *, int, int, char *, int, int);
extern short func_000A2460(int, int);
extern int func_000A2504(int, struct sos_sample *);
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
        if (sound_channels[3].handle != 0x12345678)
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
                if (sound_channels[i].priority < priority) {
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
        sound_volume_pan(sound_channels[i].position, &sound_channels[i].source->x, &volume, &pan, sound_channels[i].source);
    else
        sound_volume_pan(sound_channels[i].position, sound_channels[i].position, &volume, &pan, sound_channels[i].source);
    mc_memset(&sound_channels[i].sample, 0, 240, D_00175ACC, 246, 4);
    sound_channels[i].priority = priority;
    sound_channels[i].sample.data = (char *)sample;
    sound_channels[i].sample.length = length;
    sound_channels[i].sample.volume = (short)volume | ((short)volume << 16);
    sound_channels[i].sample.rate = 11025;
    sound_channels[i].sample.format = 32768;
    sound_channels[i].sample.pan = pan;
    sound_channels[i].sample.loop = loop != 0 ? -1 : 0;
    sound_channels[i].sample.pad10 = length;
    sound_channels[i].sample.bits = 8;
    sound_channels[i].sample.channels = 1;
    sound_channels[i].handle = func_000A2504(D_0018DD60, &sound_channels[i].sample);
    return i;
}
