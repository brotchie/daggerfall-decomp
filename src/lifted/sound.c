/* sound.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00175ACC[];
extern unsigned char player_environment;
extern int D_00186DEC;
extern int D_0018DC34;
extern int D_0018DD54;
extern int D_0018DD5C;
extern int D_0018DD60;
extern struct record *player_object;
extern struct settings *game_settings;
extern int sound_last_size;
extern signed char climate_weathers[];
extern signed char D_00196280;
extern signed char cfg_stereo;
extern char sound_channels[];
extern char D_001A3AF4[];
extern char D_001A3AF8[];
extern char D_001A3B14[];
extern char D_001A3B18[];
extern char D_001A3B1C[];
extern char D_001A3B20[];
extern char D_001A3B24[];
extern char D_001A3B28[];
extern char D_001A3B2C[];
extern char D_001A3BD8[];
extern char D_001A3BDC[];
extern char D_001A3BE4[];
extern int D_001A3EFC;
extern int D_001A3F08;
extern int nearest_fire_distance;
extern struct record *nearest_fire;
extern int ambient_rain_channel;
extern int ambient_crickets_channel;
extern int ambient_fire_channel;
extern int D_001A3F2C;
extern int D_001A3F30;
extern int D_001A3F48;
extern char music_current[];
extern signed char sound_enabled;
extern int D_001A5AD0;

extern int sos_load_song(int, ...);
extern int climate_category(void);
extern int ai_angle_diff(int, int, int);
extern int sound_play_sample(int, int, struct record *, int);
extern int sound_cache_load(int);
extern int dpmi_lock_region(int, int);
extern int dpmi_unlock_region(int, int);
extern int func_0009E2BB();
extern int func_0009E61A();
extern int mc_free();
extern int mc_memset();
extern int func_000A0517();
extern int mc_strncpy();
extern int stricmp();
extern int mc_memcpy();
extern int func_000A1D3C();
extern int func_000A2460();
extern int func_000A2504();
extern int func_000A2687();
extern int func_000A277F();
extern int func_000A27A0();
extern int func_000A2857();
extern int func_000A2941();
extern int func_000C7FD9();
extern int func_000C7FF4();
extern int func_000C808D();
int sound_play_sample_flat(int, int);
int sound_play_ambient_loop(int, struct record *, int);
void sound_stop_channel(int);
void music_stop(void);
void sound_stop_all(void);

void func_00068BA8(struct record *a1, int a2)
{
    *(int *)(D_001A3BE4 + (a2 * 268)) = (int)a1;
    if (a1 == 0) return;
    mc_memcpy((((int)sound_channels) + (a2 * 268)) + 256, (int)&a1->x, 12, (int)D_00175ACC, 95, 4);
}

void sound_volume_pan(int a1, int a2, int a3, int a4, int a5)
{
    int l_1C;
    int l_18;
    int l_14;
    int l_10;
    int l_C;

    mc_memcpy(a1, a2, 12, (int)D_00175ACC, 148, 4);
    a1 = (int)&player_object->x;
    l_18 = func_000C7FF4(*(int *)((char *)a1 + 4) - *(int *)((char *)a2 + 4), func_000C7FD9(*(int *)((char *)a1), *(int *)((char *)a1 + 8), *(int *)((char *)a2), *(int *)((char *)a2 + 8)));
    l_C = l_18;
    if (l_18 < 25) {
        *(int *)((char *)a3) = (((int)(short)game_settings->sound_volume) * 32767) / 128;
        *(int *)((char *)a4) = 32768;
        return;
    }
    l_1C = D_001A3F2C;
    if (l_18 > l_1C) {
        *(int *)((char *)a3) = 0;
    } else {
        *(int *)((char *)a3) = 32767 - ((l_18 * 32767) / l_1C);
    }
    if (*(int *)((char *)a3) > 32767) *(int *)((char *)a3) = 32767;
    l_14 = func_000C808D(*(int *)((char *)a1), *(int *)((char *)a1 + 8), *(int *)((char *)a2), *(int *)((char *)a2 + 8));
    l_10 = ai_angle_diff(player_object->yaw, l_14, (int)&l_18);
    if (l_10 > 512) l_10 = 512 - (l_10 - 512);
    l_10 = (l_10 << 15) / 512;
    if (cfg_stereo != 0) l_10 = -l_10;
    if (l_18 > 0) {
        *(int *)((char *)a4) = l_10 + 32768;
    } else {
        *(int *)((char *)a4) = 32768 - l_10;
    }
    *(int *)((char *)a3) = (*(int *)((char *)a3) * ((int)(short)game_settings->sound_volume)) / 128;
}

int func_00069281(int a1, int a2)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = 1;
    if (sound_enabled == 0) return -1;
    if (D_0018DD5C == (-1)) return -1;
    for (l_28 = 0; l_28 < 3; l_28++) {
        if (*(int *)(D_001A3BD8 + (l_28 * 268)) == 305419896) break;
        if ((short)func_000A2460(D_0018DD60, *(int *)(D_001A3BD8 + (l_28 * 268))) != 0) break;
    }
    if (l_28 == 3) {
        for (l_28 = 0; l_28 < 3; l_28++) {
            if (*(int *)(D_001A3BDC + (l_28 * 268)) < 127) {
                func_000A2687(D_0018DD60, *(int *)(D_001A3BD8 + (l_28 * 268)));
                break;
            }
        }
    }
    if (l_28 == 3) return -1;
    l_24 = 32767;
    l_20 = 32768;
    mc_memset(((int)sound_channels) + (l_28 * 268), 0, 240, (int)D_00175ACC, 291, 4);
    *(int *)(D_001A3BDC + (l_28 * 268)) = 127;
    *(int *)(sound_channels + (l_28 * 268)) = a1;
    *(int *)(D_001A3AF4 + (l_28 * 268)) = a2;
    *(int *)(D_001A3B14 + (l_28 * 268)) = ((int)(short)*(short *)&l_24) | (((int)(short)*(short *)&l_24) << 16);
    *(int *)(D_001A3B1C + (l_28 * 268)) = 11111;
    *(int *)(D_001A3B28 + (l_28 * 268)) = 32768;
    *(int *)(D_001A3B2C + (l_28 * 268)) = l_20;
    *(int *)(D_001A3B18 + (l_28 * 268)) = ((l_18 != 0) ? -1 : 0);
    *(int *)(D_001A3AF8 + (l_28 * 268)) = a2;
    *(int *)(D_001A3B20 + (l_28 * 268)) = 8;
    *(int *)(D_001A3B24 + (l_28 * 268)) = 1;
    *(int *)(D_001A3BE4 + (l_28 * 268)) = 0;
    *(int *)(D_001A3BD8 + (l_28 * 268)) = func_000A2504(D_0018DD60, ((int)sound_channels) + (l_28 * 268));
    return l_28;
}

int sound_play_sample_flat(int a1, int a2)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = 0;
    if (sound_enabled == 0) return -1;
    if (D_0018DD5C == (-1)) return -1;
    for (l_28 = 0; l_28 < 3; l_28++) {
        if (*(int *)(D_001A3BD8 + (l_28 * 268)) == 305419896) break;
        if ((short)func_000A2460(D_0018DD60, *(int *)(D_001A3BD8 + (l_28 * 268))) != 0) break;
    }
    if (l_28 == 3) {
        for (l_28 = 0; l_28 < 3; l_28++) {
            if (*(int *)(D_001A3BDC + (l_28 * 268)) < 90) {
                func_000A2687(D_0018DD60, *(int *)(D_001A3BD8 + (l_28 * 268)));
                break;
            }
        }
    }
    if (l_28 == 3) return -1;
    mc_memset(((int)sound_channels) + (l_28 * 268), 0, 240, (int)D_00175ACC, 336, 4);
    *(int *)(D_001A3BDC + (l_28 * 268)) = 90;
    *(int *)(sound_channels + (l_28 * 268)) = a1;
    *(int *)(D_001A3AF4 + (l_28 * 268)) = a2;
    *(int *)(D_001A3B14 + (l_28 * 268)) = 2147450879;
    *(int *)(D_001A3B1C + (l_28 * 268)) = 11025;
    *(int *)(D_001A3B28 + (l_28 * 268)) = 32768;
    *(int *)(D_001A3B2C + (l_28 * 268)) = 32768;
    *(int *)(D_001A3B20 + (l_28 * 268)) = 8;
    *(int *)(D_001A3B24 + (l_28 * 268)) = 1;
    *(int *)(D_001A3BE4 + (l_28 * 268)) = 0;
    *(int *)(D_001A3BD8 + (l_28 * 268)) = func_000A2504(D_0018DD60, ((int)sound_channels) + (l_28 * 268));
    return l_28;
}

void sound_stop_channel(int a1)
{
    if (*(int *)(D_001A3BD8 + (a1 * 268)) == 305419896) return;
    func_000A2687(D_0018DD60, *(int *)(D_001A3BD8 + (a1 * 268)));
    *(int *)(D_001A3BD8 + (a1 * 268)) = 305419896;
}

int func_000696F9(int a1)
{
    if (*(int *)(D_001A3BD8 + (a1 * 268)) == 305419896) return 1;
    return (int)(short)func_000A2460(D_0018DD60, *(int *)(D_001A3BD8 + (a1 * 268)));
}

void music_play(int a1)
{
    if (sound_enabled == 0) return;
    if (stricmp((int)music_current, a1) == 0) return;
    music_stop();
    mc_strncpy((int)music_current, a1, 13, (int)D_00175ACC, 374);
    D_001A3F30 = sos_load_song((int)music_current);
    if (D_0018DC34 != 0) {
        dpmi_lock_region(D_0018DC34, func_000A277F(D_0018DC34));
    }
    func_000A27A0(D_001A3F30);
}

void music_stop(void)
{
    if (sound_enabled == 0 || D_001A3F48 == 0) return;
    func_000A2857(D_001A3F30);
    func_000A0517(D_001A3F30);
    dpmi_unlock_region(D_0018DC34, func_000A277F(D_0018DC34));
    if (D_001A3F48 != 0 && D_001A3F48 != (-1751672937)) {
        mc_free(D_001A3F48, (int)D_00175ACC, 393);
        D_001A3F48 = -1751672937;
    }
    D_0018DC34 = 0;
}

void func_0006987B(void)
{
    if (sound_enabled == 0) return;
    if (D_0018DD54 == (-1)) return;
    func_000A1D3C(D_00186DEC);
}

void music_update(void)
{
    if (sound_enabled == 0) return;
    if (D_001A3F48 == 0) return;
    if (D_0018DD54 == (-1)) return;
    if ((short)func_000A2941(D_001A3F30) == 0) return;
    func_000A27A0(D_001A3F30);
}

int sound_play(int a1, struct record *a2, int a3)
{
    int l_14;

    if (sound_enabled == 0) return -1;
    l_14 = sound_cache_load(a1);
    return sound_play_sample(l_14, sound_last_size, a2, a3);
}

int sound_play_ui(int a1)
{
    int l_1C;

    if (sound_enabled == 0) return -1;
    l_1C = sound_cache_load(a1);
    return sound_play_sample_flat(l_1C, sound_last_size);
}

int sound_play_ambient_loop(int a1, struct record *a2, int a3)
{
    int l_14;

    if (sound_enabled == 0) return -1;
    l_14 = sound_cache_load(a1);
    return sound_play_sample(l_14, sound_last_size, a2, -1);
}

int func_00069AB8(int a1, struct record *a2, int a3)
{
    int l_14;

    if (sound_enabled == 0) return -1;
    l_14 = sound_cache_load(a1);
    return sound_play_sample(l_14, sound_last_size, a2, -2);
}

int func_00069B0E(int a1, int a2)
{
    int l_18;

    if (sound_enabled == 0) return -1;
    func_0009E2BB(a2, a1, (int)&l_18);
    return l_18;
}

void func_00069B53(int a1)
{
    if (sound_enabled == 0) return;
    func_0009E61A(a1);
}

void sound_update_ambient(void)
{
    if (((int)player_environment) == 1) {
        if (ambient_fire_channel != 0) {
            sound_stop_channel(ambient_fire_channel);
            ambient_fire_channel = 0;
        }
        if (((int)(unsigned char)(climate_weathers[climate_category()] & 127)) == 4) {
            if (ambient_rain_channel == 0) {
                ambient_rain_channel = sound_play_ambient_loop(385, player_object, 100);
            }
        } else if (ambient_rain_channel != 0) {
            sound_stop_channel(ambient_rain_channel);
            ambient_rain_channel = 0;
        }
        if (D_00196280 == 0 && ((int)(unsigned char)climate_weathers[climate_category()]) < 2) {
            if (ambient_crickets_channel == 0) {
                ambient_crickets_channel = sound_play_ambient_loop(375, player_object, 100);
            }
        } else if (ambient_crickets_channel != 0) {
            sound_stop_channel(ambient_crickets_channel);
            ambient_crickets_channel = 0;
        }
        return;
    }
    if (ambient_rain_channel != 0) {
        sound_stop_channel(ambient_rain_channel);
        ambient_rain_channel = 0;
    }
    if (ambient_crickets_channel != 0) {
        sound_stop_channel(ambient_crickets_channel);
        ambient_crickets_channel = 0;
    }
    if (nearest_fire_distance < 300 && ambient_fire_channel == 0 && D_001A3EFC == 305419896) {
        ambient_fire_channel = sound_play_ambient_loop(242, nearest_fire, 100);
        return;
    }
    if (ambient_fire_channel != 0 && nearest_fire_distance >= 300) {
        sound_stop_channel(ambient_fire_channel);
        ambient_fire_channel = 0;
        return;
    }
    D_001A3F08 = (int)nearest_fire;
}

void sound_stop_ambient(void)
{
    if (ambient_fire_channel != 0) {
        sound_stop_channel(ambient_fire_channel);
        ambient_fire_channel = 0;
    }
    if (ambient_crickets_channel != 0) {
        sound_stop_channel(ambient_crickets_channel);
        ambient_crickets_channel = 0;
    }
    if (ambient_rain_channel != 0) {
        sound_stop_channel(ambient_rain_channel);
        ambient_rain_channel = 0;
    }
    sound_stop_all();
}

void sound_stop_all(void)
{
    int l_18;

    for (l_18 = 0; l_18 < 4; l_18++) {
        if (*(int *)(D_001A3BD8 + (l_18 * 268)) == 305419896) continue;
        if ((short)func_000A2460(D_0018DD60, *(int *)(D_001A3BD8 + (l_18 * 268))) == 0) {
            func_000A2687(D_0018DD60, *(int *)(D_001A3BD8 + (l_18 * 268)));
        }
        *(int *)(D_001A3BD8 + (l_18 * 268)) = 305419896;
    }
    D_001A5AD0 = -1;
}
