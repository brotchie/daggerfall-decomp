/* trade.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_000CDDA8[];
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
extern char D_00195C44[];
extern signed char msgbox_button_keys;
extern signed char D_00196034;
extern signed char D_00196035;
extern signed char msgbox_button_ids;
extern signed char D_00196090;
extern signed char D_00196091;
extern unsigned char D_00196271;
extern double D_001A3AAC;
extern double trade_haggle_asking;
extern double D_001A3AC4;
extern double D_001A3ACC;
extern double D_001A3AD4;
extern int D_001A3ADC;
extern struct character *D_001A3AE0;
extern char sound_channels[];
extern char D_001A3BD8[];
extern int D_001A3F3C;
extern int D_001A3F40;
extern int midi_bsa;
extern signed char sound_enabled;

extern int sos_init(int, ...);
extern int sos_shutdown(void);
extern int sos_read_settings(int, ...);
extern int archive_open(int, int, int);
extern int func_00069B0E(int, int);
extern int dpmi_lock_region(int, int);
extern int dpmi_unlock_region(int, int);
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int func_000A1D3C();
extern void archive_close(int);
extern void msgbox_show_string(int, int);
extern void sound_stop_channel(int);
extern void music_stop(void);
extern void func_00069B53(int);
extern void inpstr_begin_number(int);
void func_0006899B(void);
#pragma aux func_000A0ED9 parm routine [];

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
    func_000A0ED9(131, (int)D_00175A30);
    mc_sprintf((int)text_buffer, (int)D_00175A38, (int)trade_haggle_asking);
    msgbox_show_string((int)text_buffer, 5);
}

void trade_counter_offer(void)
{
    int l_18;

    D_0012B508 = 146;
    l_18 = *(int *)D_00195C44 + 55000;
    func_000A0ED9(141, (int)D_00175A30);
    mc_sprintf(l_18, (int)D_00175A6A);
    *(signed char *)((char *)(func_000A0DF4(l_18) + l_18) + 1) = 0;
    msgbox_show_string(l_18, 2);
    inpstr_begin_number((int)trade_haggle_asking);
}

int func_00068845(int a1)
{
    if (a1 == D_001A3ADC) return 0;
    D_001A3ADC = a1;
    D_001A3ACC = trade_haggle_asking - D_001A3AD4;
    D_001A3AC4 = D_001A3AAC + D_001A3AD4;
    if (D_001A3ACC < D_001A3AC4 && a1 > D_001A3AAC) return a1;
    if (a1 > D_001A3ACC) return a1;
    if (a1 < D_001A3AAC) return -1;
    if (a1 <= D_001A3AC4 && a1 >= D_001A3AAC) return 0;
    if (a1 < D_001A3ACC && a1 > D_001A3AC4) {
        trade_haggle_asking -= D_001A3AD4;
        D_001A3AAC = D_001A3AD4 + D_001A3AAC;
    }
    if (((int)trade_haggle_asking) == a1) return a1;
    func_0006899B();
    return 0;
}

void func_0006899B(void)
{
    D_001A3AD4 = ((((D_00175A9E - ((short)(current_building->quality) * D_00175A96)) + (D_001A3AE0->skills[21].value * D_00175AA6)) + (D_001A3AE0->attributes[5] * D_00175AA6)) + (D_001A3AE0->reputation[1] * D_00175AAE)) * (trade_haggle_asking - D_001A3AAC);
}

int func_00068A1D(void)
{
    int l_1C;

    if ((short)sos_read_settings((int)D_00175AB8) == 0) return 0;
    if (sos_init(D_0018DD5C, D_0018DD54) != 0) return 0;
    D_0018DC64 = 2048;
    func_000A1D3C(127);
    midi_bsa = archive_open((int)D_00175AC3, 0, 0);
    for (l_1C = 0; l_1C < 4; l_1C++) {
        *(int *)(D_001A3BD8 + (l_1C * 268)) = 305419896;
    }
    sound_enabled = 1;
    dpmi_lock_region((int)sound_channels, 5168);
    dpmi_lock_region((int)D_000CDDA8, 4096);
    dpmi_lock_region((int)&D_001A3F40, 4096);
    D_001A3F3C = func_00069B0E((int)D_000CDDA8, 140);
    return 1;
}

void func_00068B1B(void)
{
    int l_18;

    if (sound_enabled == 0) return;
    func_00069B53(D_001A3F3C);
    music_stop();
    archive_close(midi_bsa);
    for (l_18 = 0; l_18 < 4; l_18++) {
        sound_stop_channel(l_18);
    }
    sos_shutdown();
    dpmi_unlock_region((int)sound_channels, 5168);
    dpmi_unlock_region((int)D_000CDDA8, 4096);
    dpmi_unlock_region((int)&D_001A3F40, 4096);
}
