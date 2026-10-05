/* sound.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00175ACC[];
extern char player_environment[];
extern char D_00186DEC[];
extern char D_0018DC34[];
extern char D_0018DD54[];
extern char D_0018DD5C[];
extern char D_0018DD60[];
extern struct record *player_object;
extern struct settings *game_settings;
extern char sound_last_size[];
extern char climate_weathers[];
extern char D_00196280[];
extern char cfg_stereo[];
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
extern char D_001A3EFC[];
extern char D_001A3F08[];
extern char nearest_fire_distance[];
extern char nearest_fire[];
extern char ambient_rain_channel[];
extern char ambient_crickets_channel[];
extern char ambient_fire_channel[];
extern char D_001A3F2C[];
extern char D_001A3F30[];
extern char D_001A3F48[];
extern char music_current[];
extern char sound_enabled[];
extern char D_001A5AD0[];

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
    if (l_18 >= 25) goto L68E53;
    *(int *)((char *)a3) = (((int)(short)game_settings->sound_volume) * 32767) / 128;
    *(int *)((char *)a4) = 32768;
    return;
L68E53:;
    l_1C = *(int *)D_001A3F2C;
    if (l_18 <= l_1C) goto L68E6E;
    *(int *)((char *)a3) = 0;
    goto L68E89;
L68E6E:;
    *(int *)((char *)a3) = 32767 - ((l_18 * 32767) / l_1C);
L68E89:;
    if (*(int *)((char *)a3) <= 32767) goto L68E9D;
    *(int *)((char *)a3) = 32767;
L68E9D:;
    l_14 = func_000C808D(*(int *)((char *)a1), *(int *)((char *)a1 + 8), *(int *)((char *)a2), *(int *)((char *)a2 + 8));
    l_10 = ai_angle_diff(player_object->yaw, l_14, (int)&l_18);
    if (l_10 <= 512) goto L68EED;
    l_10 = 512 - (l_10 - 512);
L68EED:;
    l_10 = (l_10 << 15) / 512;
    if (*(signed char *)cfg_stereo == 0) goto L68F0F;
    l_10 = -l_10;
L68F0F:;
    if (l_18 <= 0) goto L68F24;
    *(int *)((char *)a4) = l_10 + 32768;
    goto L68F31;
L68F24:;
    *(int *)((char *)a4) = 32768 - l_10;
L68F31:;
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
    if (*(signed char *)sound_enabled != 0) goto L692B0;
    return -1;
L692B0:;
    if (*(int *)D_0018DD5C != (-1)) goto L692C5;
    return -1;
L692C5:;
    l_28 = 0;
L692CC:;
    if (l_28 < 3) goto L692DC;
    goto L6930B;
L692D4:;
    l_28++;
    goto L692CC;
L692DC:;
    if (*(int *)(D_001A3BD8 + (l_28 * 268)) == 305419896) goto L6930B;
    if ((short)func_000A2460(*(int *)D_0018DD60, *(int *)(D_001A3BD8 + (l_28 * 268))) == 0) goto L692D4;
L6930B:;
    if (l_28 != 3) goto L69353;
    l_28 = 0;
L69318:;
    if (l_28 < 3) goto L69328;
    goto L69353;
L69320:;
    l_28++;
    goto L69318;
L69328:;
    if (*(int *)(D_001A3BDC + (l_28 * 268)) >= 127) goto L69351;
    func_000A2687(*(int *)D_0018DD60, *(int *)(D_001A3BD8 + (l_28 * 268)));
    goto L69353;
L69351:;
    goto L69320;
L69353:;
    if (l_28 != 3) goto L69365;
    return -1;
L69365:;
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
    *(int *)(D_001A3BD8 + (l_28 * 268)) = func_000A2504(*(int *)D_0018DD60, ((int)sound_channels) + (l_28 * 268));
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
    if (*(signed char *)sound_enabled != 0) goto L694E7;
    return -1;
L694E7:;
    if (*(int *)D_0018DD5C != (-1)) goto L694FC;
    return -1;
L694FC:;
    l_28 = 0;
L69503:;
    if (l_28 < 3) goto L69513;
    goto L69542;
L6950B:;
    l_28++;
    goto L69503;
L69513:;
    if (*(int *)(D_001A3BD8 + (l_28 * 268)) == 305419896) goto L69542;
    if ((short)func_000A2460(*(int *)D_0018DD60, *(int *)(D_001A3BD8 + (l_28 * 268))) == 0) goto L6950B;
L69542:;
    if (l_28 != 3) goto L6958A;
    l_28 = 0;
L6954F:;
    if (l_28 < 3) goto L6955F;
    goto L6958A;
L69557:;
    l_28++;
    goto L6954F;
L6955F:;
    if (*(int *)(D_001A3BDC + (l_28 * 268)) >= 90) goto L69588;
    func_000A2687(*(int *)D_0018DD60, *(int *)(D_001A3BD8 + (l_28 * 268)));
    goto L6958A;
L69588:;
    goto L69557;
L6958A:;
    if (l_28 != 3) goto L6959C;
    return -1;
L6959C:;
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
    *(int *)(D_001A3BD8 + (l_28 * 268)) = func_000A2504(*(int *)D_0018DD60, ((int)sound_channels) + (l_28 * 268));
    return l_28;
}

void sound_stop_channel(int a1)
{
    if (*(int *)(D_001A3BD8 + (a1 * 268)) == 305419896) return;
    func_000A2687(*(int *)D_0018DD60, *(int *)(D_001A3BD8 + (a1 * 268)));
    *(int *)(D_001A3BD8 + (a1 * 268)) = 305419896;
}

int func_000696F9(int a1)
{
    if (*(int *)(D_001A3BD8 + (a1 * 268)) != 305419896) goto L69726;
    return 1;
L69726:;
    return (int)(short)func_000A2460(*(int *)D_0018DD60, *(int *)(D_001A3BD8 + (a1 * 268)));
}

void music_play(int a1)
{
    if (*(signed char *)sound_enabled == 0) return;
    if (stricmp((int)music_current, a1) == 0) return;
    music_stop();
    mc_strncpy((int)music_current, a1, 13, (int)D_00175ACC, 374);
    *(int *)D_001A3F30 = sos_load_song((int)music_current);
    if (*(int *)D_0018DC34 == 0) goto L697CF;
    dpmi_lock_region(*(int *)D_0018DC34, func_000A277F(*(int *)D_0018DC34));
L697CF:;
    func_000A27A0(*(int *)D_001A3F30);
}

void music_stop(void)
{
    if (*(signed char *)sound_enabled == 0) goto L69803;
    if (*(int *)D_001A3F48 != 0) goto L69805;
L69803:;
    return;
L69805:;
    func_000A2857(*(int *)D_001A3F30);
    func_000A0517(*(int *)D_001A3F30);
    dpmi_unlock_region(*(int *)D_0018DC34, func_000A277F(*(int *)D_0018DC34));
    if (*(int *)D_001A3F48 == 0) goto L69847;
    if (*(int *)D_001A3F48 != (-1751672937)) goto L69849;
L69847:;
    goto L69867;
L69849:;
    mc_free(*(int *)D_001A3F48, (int)D_00175ACC, 393);
    *(int *)D_001A3F48 = -1751672937;
L69867:;
    *(int *)D_0018DC34 = 0;
}

void func_0006987B(void)
{
    if (*(signed char *)sound_enabled == 0) return;
    if (*(int *)D_0018DD54 == (-1)) return;
    func_000A1D3C(*(int *)D_00186DEC);
}

void music_update(void)
{
    if (*(signed char *)sound_enabled == 0) return;
    if (*(int *)D_001A3F48 == 0) return;
    if (*(int *)D_0018DD54 == (-1)) return;
    if ((short)func_000A2941(*(int *)D_001A3F30) == 0) return;
    func_000A27A0(*(int *)D_001A3F30);
}

int sound_play(int a1, struct record *a2, int a3)
{
    int l_14;

    if (*(signed char *)sound_enabled != 0) goto L6995F;
    return -1;
L6995F:;
    l_14 = sound_cache_load(a1);
    return sound_play_sample(l_14, *(int *)sound_last_size, a2, a3);
}

int sound_play_ui(int a1)
{
    int l_1C;

    if (*(signed char *)sound_enabled != 0) goto L699AF;
    return -1;
L699AF:;
    l_1C = sound_cache_load(a1);
    return sound_play_sample_flat(l_1C, *(int *)sound_last_size);
}

int sound_play_ambient_loop(int a1, struct record *a2, int a3)
{
    int l_14;

    if (*(signed char *)sound_enabled != 0) goto L69A89;
    return -1;
L69A89:;
    l_14 = sound_cache_load(a1);
    return sound_play_sample(l_14, *(int *)sound_last_size, a2, -1);
}

int func_00069AB8(int a1, struct record *a2, int a3)
{
    int l_14;

    if (*(signed char *)sound_enabled != 0) goto L69ADF;
    return -1;
L69ADF:;
    l_14 = sound_cache_load(a1);
    return sound_play_sample(l_14, *(int *)sound_last_size, a2, -2);
}

int func_00069B0E(int a1, int a2)
{
    int l_18;

    if (*(signed char *)sound_enabled != 0) goto L69B33;
    return -1;
L69B33:;
    func_0009E2BB(a2, a1, (int)&l_18);
    return l_18;
}

void func_00069B53(int a1)
{
    if (*(signed char *)sound_enabled == 0) return;
    func_0009E61A(a1);
}

void sound_update_ambient(void)
{
    if (((int)(unsigned char)*(signed char *)player_environment) != 1) goto L69C7A;
    if (*(int *)ambient_fire_channel == 0) goto L69BBA;
    sound_stop_channel(*(int *)ambient_fire_channel);
    *(int *)ambient_fire_channel = 0;
L69BBA:;
    if (((int)(unsigned char)(*(signed char *)(climate_weathers + climate_category()) & 127)) != 4) goto L69BF6;
    if (*(int *)ambient_rain_channel != 0) goto L69BF4;
    *(int *)ambient_rain_channel = sound_play_ambient_loop(385, player_object, 100);
L69BF4:;
    goto L69C13;
L69BF6:;
    if (*(int *)ambient_rain_channel == 0) goto L69C13;
    sound_stop_channel(*(int *)ambient_rain_channel);
    *(int *)ambient_rain_channel = 0;
L69C13:;
    if (*(signed char *)D_00196280 != 0) goto L69C31;
    if (((int)(unsigned char)*(signed char *)(climate_weathers + climate_category())) < 2) goto L69C33;
L69C31:;
    goto L69C58;
L69C33:;
    if (*(int *)ambient_crickets_channel != 0) goto L69C56;
    *(int *)ambient_crickets_channel = sound_play_ambient_loop(375, player_object, 100);
L69C56:;
    goto L69C75;
L69C58:;
    if (*(int *)ambient_crickets_channel == 0) goto L69C75;
    sound_stop_channel(*(int *)ambient_crickets_channel);
    *(int *)ambient_crickets_channel = 0;
L69C75:;
    return;
L69C7A:;
    if (*(int *)ambient_rain_channel == 0) goto L69C97;
    sound_stop_channel(*(int *)ambient_rain_channel);
    *(int *)ambient_rain_channel = 0;
L69C97:;
    if (*(int *)ambient_crickets_channel == 0) goto L69CB4;
    sound_stop_channel(*(int *)ambient_crickets_channel);
    *(int *)ambient_crickets_channel = 0;
L69CB4:;
    if (*(int *)nearest_fire_distance >= 300) goto L69CC9;
    if (*(int *)ambient_fire_channel == 0) goto L69CCB;
L69CC9:;
    goto L69CD7;
L69CCB:;
    if (*(int *)D_001A3EFC == 305419896) goto L69CD9;
L69CD7:;
    goto L69CF5;
L69CD9:;
    *(int *)ambient_fire_channel = sound_play_ambient_loop(242, *(struct record **)nearest_fire, 100);
    return;
L69CF5:;
    if (*(int *)ambient_fire_channel == 0) goto L69D0A;
    if (*(int *)nearest_fire_distance >= 300) goto L69D0C;
L69D0A:;
    goto L69D22;
L69D0C:;
    sound_stop_channel(*(int *)ambient_fire_channel);
    *(int *)ambient_fire_channel = 0;
    return;
L69D22:;
    *(int *)D_001A3F08 = *(int *)nearest_fire;
}

void sound_stop_ambient(void)
{
    if (*(int *)ambient_fire_channel == 0) goto L69D61;
    sound_stop_channel(*(int *)ambient_fire_channel);
    *(int *)ambient_fire_channel = 0;
L69D61:;
    if (*(int *)ambient_crickets_channel == 0) goto L69D7E;
    sound_stop_channel(*(int *)ambient_crickets_channel);
    *(int *)ambient_crickets_channel = 0;
L69D7E:;
    if (*(int *)ambient_rain_channel == 0) goto L69D9B;
    sound_stop_channel(*(int *)ambient_rain_channel);
    *(int *)ambient_rain_channel = 0;
L69D9B:;
    sound_stop_all();
}

void sound_stop_all(void)
{
    int l_18;

    l_18 = 0;
L69DBF:;
    if (l_18 < 4) goto L69DCF;
    goto L69E28;
L69DC7:;
    l_18++;
    goto L69DBF;
L69DCF:;
    if (*(int *)(D_001A3BD8 + (l_18 * 268)) == 305419896) goto L69DC7;
    if ((short)func_000A2460(*(int *)D_0018DD60, *(int *)(D_001A3BD8 + (l_18 * 268))) != 0) goto L69E15;
    func_000A2687(*(int *)D_0018DD60, *(int *)(D_001A3BD8 + (l_18 * 268)));
L69E15:;
    *(int *)(D_001A3BD8 + (l_18 * 268)) = 305419896;
    goto L69DC7;
L69E28:;
    *(int *)D_001A5AD0 = -1;
}
