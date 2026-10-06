/* trade.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern void xn_timer_tick_callback(void);
extern signed char D_0012B508;
extern char D_00175A30[];
extern char D_00175A38[];
extern char D_00175A6A[];
extern double D_00175A96;
extern double D_00175A9E;
extern double D_00175AA6;
extern double D_00175AAE;
extern char D_00175AB8[];
extern char D_00175AC3[];
extern int D_0018DC64;
extern int D_0018DD54;
extern int D_0018DD5C;
extern signed char text_buffer[];
extern struct building *current_building;
extern char *scratch_buffer;
extern signed char msgbox_button_keys;
extern signed char D_00196034;
extern signed char D_00196035;
extern signed char msgbox_button_ids;
extern signed char D_00196090;
extern signed char D_00196091;
extern unsigned char D_00196271;
extern double trade_haggle_minimum;
extern double trade_haggle_asking;
extern double D_001A3AC4;
extern double D_001A3ACC;
extern double trade_haggle_step_size;
extern int D_001A3ADC;
extern struct character *D_001A3AE0;
extern struct sound_channel sound_channels[];
extern int D_001A3F3C;
extern int timer_tick_count;
extern int midi_bsa;
extern signed char sound_enabled;

#pragma aux sosconv "*" parm caller [] value [eax] modify [eax ebx ecx edx];
extern int sos_init(int, int);
#pragma aux (sosconv) sos_init;
extern int sos_shutdown(void);
extern int sos_read_settings(char *);
#pragma aux (sosconv) sos_read_settings;
extern int archive_open(char *, iptr, int);
extern int sound_timer_add(iptr, int);
extern int dpmi_lock_region(iptr, int);
extern int dpmi_unlock_region(iptr, int);
extern void archive_close(int);
extern void msgbox_show_string(char *, short);
extern void sound_stop_channel(int);
extern void music_stop(void);
extern void sound_timer_remove(int);
extern void inpstr_begin_number(int);
void trade_haggle_step(void);
#pragma aux mc_set_location parm routine [];

void trade_haggle_show_offer(void)
{
    D_00196271 = 0;
    D_0012B508 = 146;
    msgbox_button_ids = 1;
    D_00196090 = 2;
    D_00196091 = 12;
    msgbox_button_keys = 30;
    D_00196034 = 19;
    D_00196035 = 46;
    mc_set_location(131, D_00175A30);
    mc_sprintf((char *)text_buffer, D_00175A38, (int)trade_haggle_asking);
    msgbox_show_string(text_buffer, 5);
}

void trade_counter_offer(void)
{
    char *text;

    D_0012B508 = 146;
    text = scratch_buffer + 55000;
    mc_set_location(141, D_00175A30);
    mc_sprintf(text, D_00175A6A);
    *(strlen(text) + text + 1) = 0;
    msgbox_show_string(text, 2);
    inpstr_begin_number((int)trade_haggle_asking);
}

int trade_haggle_counter(int offer)
{
    if (offer == D_001A3ADC) return 0;
    D_001A3ADC = offer;
    D_001A3ACC = trade_haggle_asking - trade_haggle_step_size;
    D_001A3AC4 = trade_haggle_minimum + trade_haggle_step_size;
    if (D_001A3ACC < D_001A3AC4 && offer > trade_haggle_minimum) return offer;
    if (offer > D_001A3ACC) return offer;
    if (offer < trade_haggle_minimum) return -1;
    if (offer <= D_001A3AC4 && offer >= trade_haggle_minimum) return 0;
    if (offer < D_001A3ACC && offer > D_001A3AC4) {
        trade_haggle_asking -= trade_haggle_step_size;
        trade_haggle_minimum = trade_haggle_step_size + trade_haggle_minimum;
    }
    if (((int)trade_haggle_asking) == offer) return offer;
    trade_haggle_step();
    return 0;
}

void trade_haggle_step(void)
{
    trade_haggle_step_size = ((((D_00175A9E - ((short)(current_building->quality) * D_00175A96)) + (D_001A3AE0->skills[21].value * D_00175AA6)) + (D_001A3AE0->attributes[5] * D_00175AA6)) + (D_001A3AE0->reputation[1] * D_00175AAE)) * (trade_haggle_asking - trade_haggle_minimum);
}

int sound_init_music(void)
{
    int i;

    if ((short)sos_read_settings(D_00175AB8) == 0) return 0;
    if (sos_init(D_0018DD5C, D_0018DD54) != 0) return 0;
    D_0018DC64 = 2048;
    func_000A1D3C(127);
    midi_bsa = archive_open(D_00175AC3, 0, 0);
    for (i = 0; i < 4; i++) {
        sound_channels[i].handle = 305419896;
    }
    sound_enabled = 1;
    dpmi_lock_region((iptr)sound_channels, 4 * REC_SIZEOF(struct sound_channel) + 4096);
    dpmi_lock_region((iptr)xn_timer_tick_callback, 4096);
    dpmi_lock_region((iptr)&timer_tick_count, 4096);
    D_001A3F3C = sound_timer_add((iptr)xn_timer_tick_callback, 140);
    return 1;
}

void sound_shutdown_music(void)
{
    int channel;

    if (sound_enabled == 0) return;
    sound_timer_remove(D_001A3F3C);
    music_stop();
    archive_close(midi_bsa);
    for (channel = 0; channel < 4; channel++) {
        sound_stop_channel(channel);
    }
    sos_shutdown();
    dpmi_unlock_region((iptr)sound_channels, 4 * REC_SIZEOF(struct sound_channel) + 4096);
    dpmi_unlock_region((iptr)xn_timer_tick_callback, 4096);
    dpmi_unlock_region((iptr)&timer_tick_count, 4096);
}
