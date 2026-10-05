/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00068C06 */
#pragma pack(1)
struct Chan {
    char pad[0xf0];
    int handle;                 /* 0xf0 */
    int pad2;                   /* 0xf4 */
    int len;                    /* 0xf8 */
    char *sample;               /* 0xfc */
    char name[12];              /* 0x100 */
};
extern int D_0018DD60;
extern char *player_object;
extern char *player_class;
extern char sound_channels[][268];
extern int nearest_fire;
extern int D_001A3F2C;
extern int D_001A3F34;
extern int D_001A3F38;
extern char sound_enabled;
extern void sound_volume_pan(char *, char *, int *, int *, char *);
extern void sound_stop_channel(int);
extern int func_000A1ED5(int, int, int);
extern int func_000A20BF(int, int, int);
extern short func_000A2460(int, int);
#define CH ((struct Chan *)sound_channels)

void sound_update_channels(void)
{
    int i;
    int len;
    int x;
    int unused;

    if (sound_enabled == 0)
        return;
    for (i = 0; i < 4; i++) {
        D_001A3F34 = i;
        if (CH[i].handle == 0x12345678)
            continue;
        if (func_000A2460(D_0018DD60, CH[i].handle) != 0) {
            CH[i].handle = 0x12345678;
            continue;
        }
        if (CH[i].sample == 0)
            continue;
        if (CH[i].sample == (char *)nearest_fire)
            D_001A3F2C = 320;
        else if ((int)(unsigned short)(*(unsigned short *)(player_class + 4) & 1) != 0)
            D_001A3F2C = 1024;
        else
            D_001A3F2C = 768;
        sound_volume_pan(sound_channels[i] + 256, CH[i].sample + 7, &len, &x, CH[i].sample);
        CH[i].len = len;
        if (len == 0 && i == 3) {
            sound_stop_channel(3);
            continue;
        }
        func_000A20BF(D_0018DD60, CH[i].handle, x);
        func_000A1ED5(D_0018DD60, CH[i].handle, (short)len << 16 | (short)len);
    }
    D_001A3F38 = *(short *)(player_object + 3);
}
