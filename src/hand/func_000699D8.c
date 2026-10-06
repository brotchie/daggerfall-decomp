/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000699D8 */
#include "structs.h"
struct msg {
    unsigned char type;
    char pad1[6];
    int a;          /* 7 */
    int b;          /* 11 */
    int c;          /* 15 */
    char pad13[53];
};
extern int sound_last_size;
extern struct sound_channel sound_channels[];
extern char sound_enabled;
extern int sound_play_sample(iptr, int, struct msg *, int);
extern iptr sound_cache_load(int);

int sound_play_at_point(int id, int x, int y, int z, int priority)
{
    iptr sample;
    struct msg point;
    int channel;

    if (sound_enabled == 0)
        return -1;
    point.type = 0;
    point.a = x;
    point.b = y;
    point.c = z;
    sample = sound_cache_load(id);
    channel = sound_play_sample(sample, sound_last_size, &point, priority);
    if (channel > -1)
        sound_channels[channel].source = 0;
    return channel;
}
