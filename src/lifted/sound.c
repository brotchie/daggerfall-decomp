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
extern signed char is_daytime;
extern signed char cfg_stereo;
extern struct sound_channel sound_channels[];
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
extern int xn_math_approx_dist2d();
extern int xn_math_approx_hypot();
extern int xn_math_angle_to_point();
int sound_play_sample_flat(int, int);
int sound_play_ambient_loop(int, struct record *, int);
void sound_stop_channel(int);
void music_stop(void);
void sound_stop_all(void);

void sound_channel_set_source(struct record *object, int channel)
{
    sound_channels[channel].source = object;
    if (object == 0) return;
    mc_memcpy(sound_channels[channel].position, &object->x, 12, (int)D_00175ACC, 95, 4);
}

void sound_volume_pan(int *listener, int *source, int *volume, int *pan, struct record *object)
{
    int range;
    int distance;
    int angle;
    int offset;
    int saved_distance;

    mc_memcpy(listener, source, 12, (int)D_00175ACC, 148, 4);
    listener = &player_object->x;
    distance = xn_math_approx_hypot(listener[1] - source[1], xn_math_approx_dist2d(listener[0], listener[2], source[0], source[2]));
    saved_distance = distance;
    if (distance < 25) {
        *volume = (((int)(short)game_settings->sound_volume) * 32767) / 128;
        *pan = 32768;
        return;
    }
    range = D_001A3F2C;
    if (distance > range) {
        *volume = 0;
    } else {
        *volume = 32767 - ((distance * 32767) / range);
    }
    if (*volume > 32767) *volume = 32767;
    angle = xn_math_angle_to_point(listener[0], listener[2], source[0], source[2]);
    offset = ai_angle_diff(player_object->yaw, angle, (int)&distance);
    if (offset > 512) offset = 512 - (offset - 512);
    offset = (offset << 15) / 512;
    if (cfg_stereo != 0) offset = -offset;
    if (distance > 0) {
        *pan = offset + 32768;
    } else {
        *pan = 32768 - offset;
    }
    *volume = (*volume * ((int)(short)game_settings->sound_volume)) / 128;
}

int func_00069281(int sample, int length)
{
    int channel;
    int volume;
    int pan;
    int unused;
    int loop;

    loop = 1;
    if (sound_enabled == 0) return -1;
    if (D_0018DD5C == (-1)) return -1;
    for (channel = 0; channel < 3; channel++) {
        if (sound_channels[channel].handle == 305419896) break;
        if ((short)func_000A2460(D_0018DD60, sound_channels[channel].handle) != 0) break;
    }
    if (channel == 3) {
        for (channel = 0; channel < 3; channel++) {
            if (sound_channels[channel].priority < 127) {
                func_000A2687(D_0018DD60, sound_channels[channel].handle);
                break;
            }
        }
    }
    if (channel == 3) return -1;
    volume = 32767;
    pan = 32768;
    mc_memset(&sound_channels[channel].sample, 0, 240, (int)D_00175ACC, 291, 4);
    sound_channels[channel].priority = 127;
    sound_channels[channel].sample.data = (char *)sample;
    sound_channels[channel].sample.length = length;
    sound_channels[channel].sample.volume = ((int)(short)*(short *)&volume) | (((int)(short)*(short *)&volume) << 16);
    sound_channels[channel].sample.rate = 11111;
    sound_channels[channel].sample.format = 32768;
    sound_channels[channel].sample.pan = pan;
    sound_channels[channel].sample.loop = ((loop != 0) ? -1 : 0);
    sound_channels[channel].sample.pad10 = length;
    sound_channels[channel].sample.bits = 8;
    sound_channels[channel].sample.channels = 1;
    sound_channels[channel].source = 0;
    sound_channels[channel].handle = func_000A2504(D_0018DD60, &sound_channels[channel].sample);
    return channel;
}

int sound_play_sample_flat(int sample, int length)
{
    int channel;
    int unused1;
    int unused2;
    int unused3;
    int loop;

    loop = 0;
    if (sound_enabled == 0) return -1;
    if (D_0018DD5C == (-1)) return -1;
    for (channel = 0; channel < 3; channel++) {
        if (sound_channels[channel].handle == 305419896) break;
        if ((short)func_000A2460(D_0018DD60, sound_channels[channel].handle) != 0) break;
    }
    if (channel == 3) {
        for (channel = 0; channel < 3; channel++) {
            if (sound_channels[channel].priority < 90) {
                func_000A2687(D_0018DD60, sound_channels[channel].handle);
                break;
            }
        }
    }
    if (channel == 3) return -1;
    mc_memset(&sound_channels[channel].sample, 0, 240, (int)D_00175ACC, 336, 4);
    sound_channels[channel].priority = 90;
    sound_channels[channel].sample.data = (char *)sample;
    sound_channels[channel].sample.length = length;
    sound_channels[channel].sample.volume = 2147450879;
    sound_channels[channel].sample.rate = 11025;
    sound_channels[channel].sample.format = 32768;
    sound_channels[channel].sample.pan = 32768;
    sound_channels[channel].sample.bits = 8;
    sound_channels[channel].sample.channels = 1;
    sound_channels[channel].source = 0;
    sound_channels[channel].handle = func_000A2504(D_0018DD60, &sound_channels[channel].sample);
    return channel;
}

void sound_stop_channel(int channel)
{
    if (sound_channels[channel].handle == 305419896) return;
    func_000A2687(D_0018DD60, sound_channels[channel].handle);
    sound_channels[channel].handle = 305419896;
}

int sound_channel_done(int channel)
{
    if (sound_channels[channel].handle == 305419896) return 1;
    return (int)(short)func_000A2460(D_0018DD60, sound_channels[channel].handle);
}

void music_play(char *name)
{
    if (sound_enabled == 0) return;
    if (stricmp((int)music_current, name) == 0) return;
    music_stop();
    mc_strncpy((int)music_current, name, 13, (int)D_00175ACC, 374);
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

int sound_play(int id, struct record *object, int priority)
{
    int sample;

    if (sound_enabled == 0) return -1;
    sample = sound_cache_load(id);
    return sound_play_sample(sample, sound_last_size, object, priority);
}

int sound_play_ui(int id)
{
    int sample;

    if (sound_enabled == 0) return -1;
    sample = sound_cache_load(id);
    return sound_play_sample_flat(sample, sound_last_size);
}

int sound_play_ambient_loop(int id, struct record *object, int priority)
{
    int sample;

    if (sound_enabled == 0) return -1;
    sample = sound_cache_load(id);
    return sound_play_sample(sample, sound_last_size, object, -1);
}

int sound_play_loop(int id, struct record *object, int priority)
{
    int sample;

    if (sound_enabled == 0) return -1;
    sample = sound_cache_load(id);
    return sound_play_sample(sample, sound_last_size, object, -2);
}

int sound_timer_add(int callback, int rate)
{
    int handle;

    if (sound_enabled == 0) return -1;
    func_0009E2BB(rate, callback, (int)&handle);
    return handle;
}

void sound_timer_remove(int handle)
{
    if (sound_enabled == 0) return;
    func_0009E61A(handle);
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
        if (is_daytime == 0 && ((int)(unsigned char)climate_weathers[climate_category()]) < 2) {
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
    if (nearest_fire_distance < 300 && ambient_fire_channel == 0 && sound_channels[3].handle == 305419896) {
        ambient_fire_channel = sound_play_ambient_loop(242, nearest_fire, 100);
        return;
    }
    if (ambient_fire_channel != 0 && nearest_fire_distance >= 300) {
        sound_stop_channel(ambient_fire_channel);
        ambient_fire_channel = 0;
        return;
    }
    sound_channels[3].source = nearest_fire;
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
    int channel;

    for (channel = 0; channel < 4; channel++) {
        if (sound_channels[channel].handle == 305419896) continue;
        if ((short)func_000A2460(D_0018DD60, sound_channels[channel].handle) == 0) {
            func_000A2687(D_0018DD60, sound_channels[channel].handle);
        }
        sound_channels[channel].handle = 305419896;
    }
    D_001A5AD0 = -1;
}
