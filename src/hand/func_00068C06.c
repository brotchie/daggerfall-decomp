/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00068C06 */
#include "records.h"

extern int D_0018DD60;
extern struct record *player_object;
extern struct career *player_class;
extern struct sound_channel sound_channels[];
extern iptr nearest_fire;
extern int D_001A3F2C;
extern int D_001A3F34;
extern int D_001A3F38;
extern char sound_enabled;
extern void sound_volume_pan(int *, int *, int *, int *, struct record *);
extern void sound_stop_channel(int);
extern int func_000A1ED5(int, int, int);
extern int func_000A20BF(int, int, int);
extern short func_000A2460(int, int);

void sound_update_channels(void)
{
    int i;
    int volume;
    int pan;
    int unused;

    if (sound_enabled == 0)
        return;
    for (i = 0; i < 4; i++) {
        D_001A3F34 = i;
        if (sound_channels[i].handle == 0x12345678)
            continue;
        if (func_000A2460(D_0018DD60, sound_channels[i].handle) != 0) {
            sound_channels[i].handle = 0x12345678;
            continue;
        }
        if (sound_channels[i].source == 0)
            continue;
        if (sound_channels[i].source == (struct record *)nearest_fire)
            D_001A3F2C = 320;
        else if ((int)(unsigned short)(player_class->flags & 1) != 0)
            D_001A3F2C = 1024;
        else
            D_001A3F2C = 768;
        sound_volume_pan(sound_channels[i].position, &sound_channels[i].source->x, &volume, &pan, sound_channels[i].source);
        sound_channels[i].volume = volume;
        if (volume == 0 && i == 3) {
            sound_stop_channel(3);
            continue;
        }
        func_000A20BF(D_0018DD60, sound_channels[i].handle, pan);
        func_000A1ED5(D_0018DD60, sound_channels[i].handle, (short)volume << 16 | (short)volume);
    }
    D_001A3F38 = player_object->yaw;
}
