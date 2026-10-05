/* sky.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int D_000C23B8;
extern int D_000C23BC;
extern int D_000C23C0;
extern char *D_0012B500;
extern char D_00136E00[];
extern char D_00136E24[];
extern int screen_buffer;
extern int D_00147954;
extern int D_00150200[];
extern int D_00150A00[];
extern char D_00170A6C[];
extern char D_00170A79[];
extern char D_00170A86[];
extern char D_00170A8C[];
extern char D_00170A98[];
extern char D_00170AA5[];
extern signed char month_seasons[];
extern signed char D_0017A294[];
extern signed char D_0017A295[];
extern signed char D_0017A296[];
extern signed char weather_chances[];
extern signed char D_0017A39C[];
extern signed char D_0017A3D8[];
extern signed char D_0017A3DF[];
extern signed char D_0017A3E5[];
extern signed char D_0017A3EB[];
extern signed char text_buffer[];
extern int view_look_pitch;
extern int D_001959BC;
extern struct record *camera_object;
extern struct record *player_object;
extern char *hud_bar_image;
extern int game_minutes;
extern struct settings *game_settings;
extern int D_00195C44;
extern int D_00195CF4;
extern int D_00195D48;
extern signed char climate_weathers[];
extern signed char D_00196286;
extern signed char D_0019629B;
extern int D_001970E0;
extern int D_001970E4;
extern int moon0_image;
extern int moon1_image;
extern int D_0019857C;
extern int moon0_phase;
extern int moon1_phase;
extern char D_001985A8[];
extern char D_001985B4[];
extern int D_001985C8;
extern int D_001985CC;
extern int D_001985D0;

extern int climate_category(void);
extern int disk_read_file(int, int);
extern int disk_open_data(int);
extern int rand_range(int, int);
extern int rand();
extern int srand();
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int mc_malloc();
extern int func_000A00CB();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000C8167();
extern int func_000C816E();
extern int func_000CB31E();
extern int func_000CD33A();
extern int func_00136AD8();
extern int func_00137000();
extern int func_00137725();
extern int func_0014BDDD();
void func_000351B6(int, int, int, int);
void func_000359B4(int);
void sky_load_night(void);
void func_00035CE5(int);
#pragma aux func_000A0ED9 parm routine [];

void sky_init(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    D_0019629B = 0;
    D_00195D48 = 10000;
    moon0_image = disk_read_file((int)D_00170A6C, 0);
    moon1_image = disk_read_file((int)D_00170A79, 0);
    D_001970E0 = mc_malloc(112640, (int)D_00170A86, 93);
    D_001970E4 = mc_malloc(112640, (int)D_00170A86, 94);
    D_00196286 = 13;
    for (l_24 = 0; l_24 < 32; l_24++) {
        D_0017A294[l_24 * 3] <<= 2;
        D_0017A295[l_24 * 3] <<= 2;
        D_0017A296[l_24 * 3] <<= 2;
    }
    func_000C816E();
}

void func_00034EC1(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (D_0019857C == 0) return;
    l_20 = player_object->x + D_001985C8;
    l_1C = player_object->y + D_001985CC;
    l_18 = player_object->z + D_001985D0;
    func_0014BDDD((int)&l_20, (int)&l_1C, (int)&l_18);
    func_00136AD8(-l_20, l_1C, -l_18, D_0019857C, 0, 8);
}

void sky_update_moons(void)
{
    D_000C23B8 = (camera_object->angle_x + view_look_pitch) & 2047;
    D_000C23BC = (camera_object->yaw + D_001959BC) & 2047;
    D_000C23C0 = 0;
    func_00137000(D_000C23B8, D_000C23BC, D_000C23C0, (int)D_00136E00);
    func_00137725((int)D_00136E00, (int)D_00136E24);
    func_000351B6((int)D_001985B4, 1000, ((unsigned)game_minutes) % 2500, 2500);
    moon0_phase = (((unsigned)game_minutes) / 1440) & 31;
    func_000351B6((int)D_001985A8, -1000, ((unsigned)game_minutes) % 3500, 3500);
    moon1_phase = (((unsigned)(game_minutes + 5760)) / 1440) & 31;
}

void sky_free(void)
{
    int l_18;

    if (moon0_image != 0 && moon0_image != (-1751672937)) {
        mc_free(moon0_image, (int)D_00170A86, 368);
        moon0_image = -1751672937;
    }
    if (moon1_image != 0 && moon1_image != (-1751672937)) {
        mc_free(moon1_image, (int)D_00170A86, 369);
        moon1_image = -1751672937;
    }
    if (D_001970E0 != 0 && D_001970E0 != (-1751672937)) {
        mc_free(D_001970E0, (int)D_00170A86, 370);
        D_001970E0 = -1751672937;
    }
    if (D_001970E4 == 0 || D_001970E4 == (-1751672937)) return;
    mc_free(D_001970E4, (int)D_00170A86, 371);
    D_001970E4 = -1751672937;
}

void func_00035129(int a1, int a2, int a3)
{
    int l_10;

    l_10 = ((1087 - ((a3 * 1087) / 720)) + 2015) & 2047;
    *(int *)((char *)a1 + 8) = a2;
    *(int *)((char *)a1) = func_000C8167(8192, D_00150A00[l_10]);
    *(int *)((char *)a1 + 4) = -(func_000C8167(8192, D_00150200[l_10]));
}

void func_000351B6(int a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = (a3 << 11) / a4;
    *(int *)((char *)a1 + 8) = a2;
    *(int *)((char *)a1) = func_000C8167(8192, D_00150A00[l_C]);
    *(int *)((char *)a1 + 4) = -(func_000C8167(8192, D_00150200[l_C]));
}

void weather_roll(void)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = (int)(unsigned char)climate_weathers[climate_category()];
    l_2C = (int)(unsigned char)month_seasons[((unsigned)(((unsigned)game_minutes) % 518400)) / 43200];
    for (l_30 = 0; l_30 < 6; l_30++) {
        l_28 = rand_range(0, 99);
        l_24 = 0;
        while (l_28 > (-1)) {
            l_28 -= (int)(unsigned char)weather_chances[((l_2C * 42) + (l_30 * 7)) + l_24++];
        }
        l_24--;
        if (l_24 > 6) l_24 = 6;
        if (l_24 == 4 && rand_range(0, 100) <= 15) l_24 |= 128;
        if (l_24 == 5 && rand_range(0, 100) <= 10) l_24 |= 128;
        climate_weathers[l_30] = *(signed char *)&l_24;
    }
}

void sky_load_day(int a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    a1 = a1 % 1440;
    if (a1 < 360) {
        l_24 = 0;
    } else if (a1 > 1080) {
        l_24 = 0;
    } else if (a1 >= 488 && a1 <= 952) {
        l_24 = 31;
    } else if (a1 < 700) {
        l_24 = (a1 - 360) >> 2;
    } else {
        l_24 = (-(a1 - 1080)) >> 2;
    }
    l_18 = climate_category();
    l_1C = (int)(unsigned char)climate_weathers[l_18];
    if ((l_1C & 127) == 3 || (l_1C & 128) != 0) return;
    l_20 = rand();
    srand(((unsigned)game_minutes) / 1440);
    l_24 += ((int)(unsigned char)D_0017A39C[rand_range(0, 2) + ((((int)(unsigned char)D_0017A3D8[(int)(unsigned char)climate_weathers[l_18]]) * 3) + (((int)(unsigned char)D_0017A3DF[l_18]) * 15))]) << 5;
    srand(l_20);
    if (l_24 == D_00195D48) return;
    D_00195D48 = l_24;
    func_000A0ED9(558, (int)D_00170A86);
    mc_sprintf((int)text_buffer, (int)D_00170A8C, l_24 >> 5);
    l_28 = disk_open_data((int)text_buffer);
    l_24 &= 31;
    lseek(l_28, (int)&*(signed char *)((char *)(l_24 * 776) + 11), 0);
    func_000A00CB(l_28, D_00195C44, 93);
    func_000CD33A(D_00195C44, 1, 31);
    mc_memcpy((int)(D_0012B500 + 3), D_00195C44, 93, (int)D_00170A86, 565, 4);
    func_000359B4(game_minutes);
    lseek(l_28, (l_24 << 14) + 24832, 0);
    func_000A00CB(l_28, D_00195CF4, 16384);
    lseek(l_28, (int)&*(signed char *)((char *)(l_24 * 112640) + 549120), 0);
    func_000A00CB(l_28, D_001970E0, 112640);
    lseek(l_28, (l_24 * 112640) + 4153600, 0);
    func_000A00CB(l_28, D_001970E4, 112640);
    func_0009DEA7(l_28);
}

void sky_draw_day(int a1, int a2, int a3, int a4)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;
    int l_C;

    D_0019629B = 0;
    l_14 = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(hud_bar_image + 2));
    if (((int)(unsigned char)(climate_weathers[a4] & 127)) == 3 || ((int)(unsigned char)(climate_weathers[a4] & 128)) != 0) {
        mc_memset(screen_buffer, 119, l_14 * 320, (int)D_00170A86, 598, 4);
        return;
    }
    l_14 = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(hud_bar_image + 2));
    l_28 = (139 - a1) << 9;
    if (l_28 < 0) l_28 = 0;
    l_24 = ((camera_object->yaw + D_001959BC) - 705) & 2047;
    l_18 = l_24 / 512;
    l_24 = l_24 % 512;
    if (((unsigned)(((unsigned)game_minutes) % 1440)) < 720) {
        l_1C = 0;
    } else {
        l_1C = 2;
    }
    a1 += 75;
    l_20 = 511 - l_24;
    if (l_20 >= 320) {
        if (l_18 == l_1C) {
            func_000CB31E((D_001970E4 + l_24) + l_28, D_00147954, 320, a1);
        } else {
            func_000CB31E((D_001970E0 + l_24) + l_28, D_00147954, 320, a1);
        }
    } else {
        if (l_18 == l_1C) {
            func_000CB31E((D_001970E4 + l_24) + l_28, D_00147954, l_20, a1);
        } else {
            func_000CB31E((D_001970E0 + l_24) + l_28, D_00147954, l_20, a1);
        }
        if ((l_18 + 1) == l_1C) {
            func_000CB31E(D_001970E4 + l_28, D_00147954 + l_20, 320 - l_20, a1);
        } else {
            func_000CB31E(D_001970E0 + l_28, D_00147954 + l_20, 320 - l_20, a1);
        }
    }
    for (l_10 = a1; l_10 < l_14; l_10++) {
        mc_memset((int)(*(char **)&D_00147954 + (l_10 * 320)), (int)(unsigned char)*(signed char *)(((char *)D_001970E0) + 109058), 320, (int)D_00170A86, 636, 4);
    }
    mc_memcpy(screen_buffer, D_00147954, l_14 * 320, (int)D_00170A86, 638, 4);
}

void func_000359B4(int a1)
{
    int l_18;

    a1 = ((unsigned)a1) % 1440;
    if (((unsigned)a1) < 360) {
        l_18 = 0;
    } else if (((unsigned)a1) > 1080) {
        l_18 = 0;
    } else if (((unsigned)a1) > 488 && ((unsigned)a1) < 952) {
        l_18 = 31;
    } else if (((unsigned)a1) < 700) {
        l_18 = ((unsigned)(a1 - 360)) >> 2;
    } else {
        l_18 = ((unsigned)(-(a1 - 1080))) >> 2;
    }
    func_000CD33A(((int)D_0017A294) + (l_18 * 3), 255, 1);
}

void func_00035A64(int a1)
{
}

void sky_load_night(void)
{
    disk_read_file((int)D_00170A98, D_00195C44);
    func_000CD33A(D_00195C44 + 11, 1, 31);
    mc_memcpy((int)D_0012B500 + 3, (int)&*(signed char *)(*(char **)&D_00195C44 + 11), 93, (int)D_00170A86, 671, 4);
    func_000A0ED9(673, (int)D_00170A86);
    mc_sprintf((int)text_buffer, (int)D_00170AA5, (int)(unsigned char)D_0017A3E5[climate_category()]);
    disk_read_file((int)text_buffer, D_001970E0);
    func_00035CE5(D_001970E0);
    D_0019629B = 1;
}

void sky_draw_night(int a1, int a2)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    if (D_0019629B == 0) sky_load_night();
    l_1C = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(hud_bar_image + 2));
    l_30 = (139 - a1) << 9;
    if (l_30 < 0) l_30 = 0;
    l_2C = ((camera_object->yaw + D_001959BC) - 705) & 2047;
    l_20 = l_2C / 512;
    l_2C = l_2C % 512;
    a1 += 75;
    l_28 = 511 - l_2C;
    if (a1 > 0) {
        if (l_28 >= 320) {
            func_000CB31E((D_001970E0 + l_2C) + l_30, D_00147954, 320, a1);
        } else {
            func_000CB31E((D_001970E0 + l_2C) + l_30, D_00147954, l_28, a1);
            func_000CB31E(D_001970E0 + l_30, D_00147954 + l_28, 320 - l_28, a1);
        }
    }
    if (a1 < 0) a1 = 0;
    for (l_18 = a1; l_18 < l_1C; l_18++) {
        mc_memset((int)(*(char **)&D_00147954 + (l_18 * 320)), 15, 320, (int)D_00170A86, 713, 4);
    }
    mc_memcpy(screen_buffer, D_00147954, l_1C * 320, (int)D_00170A86, 715, 4);
}

void func_00035CE5(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    for (l_20 = 0; l_20 < 300; l_20++) {
        l_1C = rand_range(0, 511);
        l_18 = rand_range(0, 199) << 9;
        if (((int)(unsigned char)*(signed char *)((char *)((l_1C + l_18) + a1))) < 16) continue;
        *(signed char *)((char *)((l_1C + l_18) + a1)) = D_0017A3EB[rand() & 15];
    }
}
