/* sky.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int xn_cam_pitch;
extern int xn_cam_yaw;
extern int xn_cam_roll;
extern char *xn_pal_current;
extern char xn_cam_rotation[];
extern char xn_cam_view_matrix[];
extern int screen_buffer;
extern int D_00147954;
extern int xn_sin_table[];
extern int xn_cos_table[];
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
extern int view_look_yaw;
extern struct record *camera_object;
extern struct record *player_object;
extern char hud_bar_image[];
extern int game_minutes;
extern struct settings *game_settings;
extern char scratch_buffer[];
extern int D_00195CF4;
extern int sky_loaded_frame;
extern signed char climate_weathers[];
extern signed char D_00196286;
extern signed char night_sky_loaded;
extern int sky_image_a;
extern int sky_image_b;
extern char moon0_image[];
extern char moon1_image[];
extern int sun_light;
extern int moon0_phase;
extern int moon1_phase;
extern char moon1_direction[];
extern char moon0_direction[];
extern int sun_direction;
extern int D_001985CC;
extern int D_001985D0;

extern int climate_category(void);
extern int disk_read_file(int, int);
extern int disk_open_data(int);
extern int rand_range(int, int);
extern int rand();
extern int srand();
extern int close();
extern int mc_free();
extern int mc_memset();
extern int lseek();
extern int mc_malloc();
extern int read();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int xn_math_fixmul28_v2();
extern int xn_sky_init_stars();
extern int xn_sky_copy_rows();
extern int xn_pal_set_range_8bit();
extern int xn_light_add();
extern int xn_mat_from_angles();
extern int xn_cam_scale_matrix();
extern int xn_vec_normalize_ptr();
void sky_orbit_direction(int, int, int, int);
void sky_set_time_colour(int);
void sky_load_night(void);
void sky_add_stars(int);
#pragma aux mc_set_location parm routine [];

void sky_init(void)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    night_sky_loaded = 0;
    sky_loaded_frame = 10000;
    *(int *)moon0_image = disk_read_file((int)D_00170A6C, 0);
    *(int *)moon1_image = disk_read_file((int)D_00170A79, 0);
    sky_image_a = mc_malloc(112640, (int)D_00170A86, 93);
    sky_image_b = mc_malloc(112640, (int)D_00170A86, 94);
    D_00196286 = 13;
    for (l_24 = 0; l_24 < 32; l_24++) {
        D_0017A294[l_24 * 3] <<= 2;
        D_0017A295[l_24 * 3] <<= 2;
        D_0017A296[l_24 * 3] <<= 2;
    }
    xn_sky_init_stars();
}

void sky_apply_sunlight(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (sun_light == 0) return;
    l_20 = player_object->x + sun_direction;
    l_1C = player_object->y + D_001985CC;
    l_18 = player_object->z + D_001985D0;
    xn_vec_normalize_ptr((int)&l_20, (int)&l_1C, (int)&l_18);
    xn_light_add(-l_20, l_1C, -l_18, sun_light, 0, 8);
}

void sky_update_moons(void)
{
    xn_cam_pitch = (camera_object->angle_x + view_look_pitch) & 2047;
    xn_cam_yaw = (camera_object->yaw + view_look_yaw) & 2047;
    xn_cam_roll = 0;
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (int)xn_cam_rotation);
    xn_cam_scale_matrix((int)xn_cam_rotation, (int)xn_cam_view_matrix);
    sky_orbit_direction((int)moon0_direction, 1000, ((unsigned)game_minutes) % 2500, 2500);
    moon0_phase = (((unsigned)game_minutes) / 1440) & 31;
    sky_orbit_direction((int)moon1_direction, -1000, ((unsigned)game_minutes) % 3500, 3500);
    moon1_phase = (((unsigned)(game_minutes + 5760)) / 1440) & 31;
}

void sky_free(void)
{
    int l_18;

    if (*(int *)moon0_image != 0 && *(int *)moon0_image != (-1751672937)) {
        mc_free(*(int *)moon0_image, (int)D_00170A86, 368);
        *(int *)moon0_image = -1751672937;
    }
    if (*(int *)moon1_image != 0 && *(int *)moon1_image != (-1751672937)) {
        mc_free(*(int *)moon1_image, (int)D_00170A86, 369);
        *(int *)moon1_image = -1751672937;
    }
    if (sky_image_a != 0 && sky_image_a != (-1751672937)) {
        mc_free(sky_image_a, (int)D_00170A86, 370);
        sky_image_a = -1751672937;
    }
    if (sky_image_b == 0 || sky_image_b == (-1751672937)) return;
    mc_free(sky_image_b, (int)D_00170A86, 371);
    sky_image_b = -1751672937;
}

void sky_sun_direction(int a1, int a2, int a3)
{
    int l_10;

    l_10 = ((1087 - ((a3 * 1087) / 720)) + 2015) & 2047;
    *(int *)((char *)a1 + 8) = a2;
    *(int *)((char *)a1) = xn_math_fixmul28_v2(8192, xn_cos_table[l_10]);
    *(int *)((char *)a1 + 4) = -(xn_math_fixmul28_v2(8192, xn_sin_table[l_10]));
}

void sky_orbit_direction(int a1, int a2, int a3, int a4)
{
    int l_C;

    l_C = (a3 << 11) / a4;
    *(int *)((char *)a1 + 8) = a2;
    *(int *)((char *)a1) = xn_math_fixmul28_v2(8192, xn_cos_table[l_C]);
    *(int *)((char *)a1 + 4) = -(xn_math_fixmul28_v2(8192, xn_sin_table[l_C]));
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
    if (l_24 == sky_loaded_frame) return;
    sky_loaded_frame = l_24;
    mc_set_location(558, (int)D_00170A86);
    mc_sprintf((int)text_buffer, (int)D_00170A8C, l_24 >> 5);
    l_28 = disk_open_data((int)text_buffer);
    l_24 &= 31;
    lseek(l_28, (int)&*(signed char *)((char *)(l_24 * 776) + 11), 0);
    read(l_28, *(int *)scratch_buffer, 93);
    xn_pal_set_range_8bit(*(int *)scratch_buffer, 1, 31);
    mc_memcpy((int)(xn_pal_current + 3), *(int *)scratch_buffer, 93, (int)D_00170A86, 565, 4);
    sky_set_time_colour(game_minutes);
    lseek(l_28, (l_24 << 14) + 24832, 0);
    read(l_28, D_00195CF4, 16384);
    lseek(l_28, (int)&*(signed char *)((char *)(l_24 * 112640) + 549120), 0);
    read(l_28, sky_image_a, 112640);
    lseek(l_28, (l_24 * 112640) + 4153600, 0);
    read(l_28, sky_image_b, 112640);
    close(l_28);
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

    night_sky_loaded = 0;
    l_14 = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(*(char **)hud_bar_image + 2));
    if (((int)(unsigned char)(climate_weathers[a4] & 127)) == 3 || ((int)(unsigned char)(climate_weathers[a4] & 128)) != 0) {
        mc_memset(screen_buffer, 119, l_14 * 320, (int)D_00170A86, 598, 4);
        return;
    }
    l_14 = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(*(char **)hud_bar_image + 2));
    l_28 = (139 - a1) << 9;
    if (l_28 < 0) l_28 = 0;
    l_24 = ((camera_object->yaw + view_look_yaw) - 705) & 2047;
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
            xn_sky_copy_rows((sky_image_b + l_24) + l_28, D_00147954, 320, a1);
        } else {
            xn_sky_copy_rows((sky_image_a + l_24) + l_28, D_00147954, 320, a1);
        }
    } else {
        if (l_18 == l_1C) {
            xn_sky_copy_rows((sky_image_b + l_24) + l_28, D_00147954, l_20, a1);
        } else {
            xn_sky_copy_rows((sky_image_a + l_24) + l_28, D_00147954, l_20, a1);
        }
        if ((l_18 + 1) == l_1C) {
            xn_sky_copy_rows(sky_image_b + l_28, D_00147954 + l_20, 320 - l_20, a1);
        } else {
            xn_sky_copy_rows(sky_image_a + l_28, D_00147954 + l_20, 320 - l_20, a1);
        }
    }
    for (l_10 = a1; l_10 < l_14; l_10++) {
        mc_memset((int)(*(char **)&D_00147954 + (l_10 * 320)), (int)(unsigned char)*(signed char *)(((char *)sky_image_a) + 109058), 320, (int)D_00170A86, 636, 4);
    }
    mc_memcpy(screen_buffer, D_00147954, l_14 * 320, (int)D_00170A86, 638, 4);
}

void sky_set_time_colour(int a1)
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
    xn_pal_set_range_8bit(((int)D_0017A294) + (l_18 * 3), 255, 1);
}

void sky_stub(int a1)
{
}

void sky_load_night(void)
{
    disk_read_file((int)D_00170A98, *(int *)scratch_buffer);
    xn_pal_set_range_8bit(*(int *)scratch_buffer + 11, 1, 31);
    mc_memcpy((int)xn_pal_current + 3, (int)&*(signed char *)(*(char **)scratch_buffer + 11), 93, (int)D_00170A86, 671, 4);
    mc_set_location(673, (int)D_00170A86);
    mc_sprintf((int)text_buffer, (int)D_00170AA5, (int)(unsigned char)D_0017A3E5[climate_category()]);
    disk_read_file((int)text_buffer, sky_image_a);
    sky_add_stars(sky_image_a);
    night_sky_loaded = 1;
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

    if (night_sky_loaded == 0) sky_load_night();
    l_1C = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(*(char **)hud_bar_image + 2));
    l_30 = (139 - a1) << 9;
    if (l_30 < 0) l_30 = 0;
    l_2C = ((camera_object->yaw + view_look_yaw) - 705) & 2047;
    l_20 = l_2C / 512;
    l_2C = l_2C % 512;
    a1 += 75;
    l_28 = 511 - l_2C;
    if (a1 > 0) {
        if (l_28 >= 320) {
            xn_sky_copy_rows((sky_image_a + l_2C) + l_30, D_00147954, 320, a1);
        } else {
            xn_sky_copy_rows((sky_image_a + l_2C) + l_30, D_00147954, l_28, a1);
            xn_sky_copy_rows(sky_image_a + l_30, D_00147954 + l_28, 320 - l_28, a1);
        }
    }
    if (a1 < 0) a1 = 0;
    for (l_18 = a1; l_18 < l_1C; l_18++) {
        mc_memset((int)(*(char **)&D_00147954 + (l_18 * 320)), 15, 320, (int)D_00170A86, 713, 4);
    }
    mc_memcpy(screen_buffer, D_00147954, l_1C * 320, (int)D_00170A86, 715, 4);
}

void sky_add_stars(int a1)
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
