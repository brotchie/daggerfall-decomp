/* sky.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

extern int xn_cam_pitch;
extern int xn_cam_yaw;
extern int xn_cam_roll;
extern char *xn_pal_current;
extern char xn_cam_rotation[];
extern char xn_cam_view_matrix[];
extern iptr screen_buffer;
extern iptr D_00147954;
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
extern struct image *hud_bar_image;
extern int game_minutes;
extern struct settings *game_settings;
extern char *scratch_buffer;
extern iptr D_00195CF4;
extern int sky_loaded_frame;
extern signed char climate_weathers[];
extern signed char D_00196286;
extern signed char night_sky_loaded;
extern iptr sky_image_a;
extern iptr sky_image_b;
extern struct cfa_header *moon0_image;
extern struct cfa_header *moon1_image;
extern int sun_light;
extern int moon0_phase;
extern int moon1_phase;
extern char moon1_direction[];
extern char moon0_direction[];
extern int sun_direction;
extern int D_001985CC;
extern int D_001985D0;

extern int climate_category(void);
extern iptr disk_read_file(char *, iptr);
extern int disk_open_data(char *);
extern int rand_range(int, int);
extern int xn_math_fixmul28_v2(int, int);
extern void xn_sky_init_stars(void);
extern void xn_sky_copy_rows(char *, char *, unsigned, unsigned);
extern void xn_pal_set_range_8bit(char *, int, int);
extern int xn_light_add(int, int, int, int, int, int);
extern void xn_mat_from_angles(int, int, int, void *);
extern void xn_cam_scale_matrix(void *, void *);
extern void xn_vec_normalize_ptr(void *, iptr, iptr);
void sky_orbit_direction(int *, int, int, int);
void sky_set_time_colour(int);
void sky_load_night(void);
void sky_add_stars(unsigned char *);
#pragma aux mc_set_location parm routine [];

void sky_init(void)
{
    int unused1;
    int unused2;
    int i;
    int unused3;
    int unused4;
    int unused5;

    night_sky_loaded = 0;
    sky_loaded_frame = 10000;
    moon0_image = (struct cfa_header *)disk_read_file(D_00170A6C, 0);
    moon1_image = (struct cfa_header *)disk_read_file(D_00170A79, 0);
    sky_image_a = (iptr)mc_malloc(112640, D_00170A86, 93);
    sky_image_b = (iptr)mc_malloc(112640, D_00170A86, 94);
    D_00196286 = 13;
    for (i = 0; i < 32; i++) {
        D_0017A294[i * 3] <<= 2;
        D_0017A295[i * 3] <<= 2;
        D_0017A296[i * 3] <<= 2;
    }
    xn_sky_init_stars();
}

void sky_apply_sunlight(void)
{
    int x;
    int y;
    int z;

    if (sun_light == 0) return;
    x = player_object->x + sun_direction;
    y = player_object->y + D_001985CC;
    z = player_object->z + D_001985D0;
    xn_vec_normalize_ptr(&x, (iptr)&y, (iptr)&z);
    xn_light_add(-x, y, -z, sun_light, 0, 8);
}

void sky_update_moons(void)
{
    xn_cam_pitch = (camera_object->angle_x + view_look_pitch) & 2047;
    xn_cam_yaw = (camera_object->yaw + view_look_yaw) & 2047;
    xn_cam_roll = 0;
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, xn_cam_rotation);
    xn_cam_scale_matrix(xn_cam_rotation, xn_cam_view_matrix);
    sky_orbit_direction((int *)moon0_direction, 1000, ((unsigned)game_minutes) % 2500, 2500);
    moon0_phase = (((unsigned)game_minutes) / 1440) & 31;
    sky_orbit_direction((int *)moon1_direction, -1000, ((unsigned)game_minutes) % 3500, 3500);
    moon1_phase = (((unsigned)(game_minutes + 5760)) / 1440) & 31;
}

void sky_free(void)
{
    int unused;

    if ((iptr)moon0_image != 0 && (iptr)moon0_image != (-1751672937)) {
        mc_free(moon0_image, D_00170A86, 368);
        moon0_image = (struct cfa_header *)(iptr)-1751672937;
    }
    if ((iptr)moon1_image != 0 && (iptr)moon1_image != (-1751672937)) {
        mc_free(moon1_image, D_00170A86, 369);
        moon1_image = (struct cfa_header *)(iptr)-1751672937;
    }
    if (sky_image_a != 0 && sky_image_a != (-1751672937)) {
        mc_free((void *)sky_image_a, D_00170A86, 370);
        sky_image_a = -1751672937;
    }
    if (sky_image_b == 0 || sky_image_b == (-1751672937)) return;
    mc_free((void *)sky_image_b, D_00170A86, 371);
    sky_image_b = -1751672937;
}

void sky_sun_direction(int *direction, int z, int minutes)
{
    int angle;

    angle = ((1087 - ((minutes * 1087) / 720)) + 2015) & 2047;
    direction[2] = z;
    direction[0] = xn_math_fixmul28_v2(8192, xn_cos_table[angle]);
    direction[1] = -(xn_math_fixmul28_v2(8192, xn_sin_table[angle]));
}

void sky_orbit_direction(int *direction, int z, int t, int period)
{
    int angle;

    angle = (t << 11) / period;
    direction[2] = z;
    direction[0] = xn_math_fixmul28_v2(8192, xn_cos_table[angle]);
    direction[1] = -(xn_math_fixmul28_v2(8192, xn_sin_table[angle]));
}

void weather_roll(void)
{
    int climate;
    int season;
    int roll;
    int weather;
    int unused1;
    int unused2;
    int current_weather;

    unused2 = 0;
    current_weather = (int)(unsigned char)climate_weathers[climate_category()];
    season = (int)(unsigned char)month_seasons[((unsigned)(((unsigned)game_minutes) % 518400)) / 43200];
    for (climate = 0; climate < 6; climate++) {
        roll = rand_range(0, 99);
        weather = 0;
        while (roll > (-1)) {
            roll -= (int)(unsigned char)weather_chances[((season * 42) + (climate * 7)) + weather++];
        }
        weather--;
        if (weather > 6) weather = 6;
        if (weather == 4 && rand_range(0, 100) <= 15) weather |= 128;
        if (weather == 5 && rand_range(0, 100) <= 10) weather |= 128;
        climate_weathers[climate] = *(signed char *)&weather;
    }
}

void sky_load_day(int minutes)
{
    int fd;
    int frame;
    int saved_seed;
    int weather;
    int climate;

    minutes = minutes % 1440;
    if (minutes < 360) {
        frame = 0;
    } else if (minutes > 1080) {
        frame = 0;
    } else if (minutes >= 488 && minutes <= 952) {
        frame = 31;
    } else if (minutes < 700) {
        frame = (minutes - 360) >> 2;
    } else {
        frame = (-(minutes - 1080)) >> 2;
    }
    climate = climate_category();
    weather = (int)(unsigned char)climate_weathers[climate];
    if ((weather & 127) == 3 || (weather & 128) != 0) return;
    saved_seed = rand();
    srand(((unsigned)game_minutes) / 1440);
    frame += ((int)(unsigned char)D_0017A39C[rand_range(0, 2) + ((((int)(unsigned char)D_0017A3D8[(int)(unsigned char)climate_weathers[climate]]) * 3) + (((int)(unsigned char)D_0017A3DF[climate]) * 15))]) << 5;
    srand(saved_seed);
    if (frame == sky_loaded_frame) return;
    sky_loaded_frame = frame;
    mc_set_location(558, D_00170A86);
    mc_sprintf((char *)text_buffer, D_00170A8C, frame >> 5);
    fd = disk_open_data(text_buffer);
    frame &= 31;
    lseek(fd, (int)(iptr)&*(signed char *)((char *)(iptr)(frame * 776) + 11), 0);
    read(fd, (void *)scratch_buffer, 93);
    xn_pal_set_range_8bit(scratch_buffer, 1, 31);
    mc_memcpy((xn_pal_current + 3), (void *)scratch_buffer, 93, D_00170A86, 565, 4);
    sky_set_time_colour(game_minutes);
    lseek(fd, (frame << 14) + 24832, 0);
    read(fd, (void *)D_00195CF4, 16384);
    lseek(fd, (int)(iptr)&*(signed char *)((char *)(iptr)(frame * 112640) + 549120), 0);
    read(fd, (void *)sky_image_a, 112640);
    lseek(fd, (frame * 112640) + 4153600, 0);
    read(fd, (void *)sky_image_b, 112640);
    close(fd);
}

void sky_draw_day(int horizon_y, int horizon_y2, int day, int climate)
{
    int src_offset;
    int column;
    int width;
    int b_section;
    int section;
    int view_bottom;
    int row;
    int unused;

    night_sky_loaded = 0;
    view_bottom = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : hud_bar_image->y);
    if (((int)(unsigned char)(climate_weathers[climate] & 127)) == 3 || ((int)(unsigned char)(climate_weathers[climate] & 128)) != 0) {
        mc_memset((void *)screen_buffer, 119, view_bottom * 320, D_00170A86, 598, 4);
        return;
    }
    view_bottom = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : hud_bar_image->y);
    src_offset = (139 - horizon_y) << 9;
    if (src_offset < 0) src_offset = 0;
    column = ((camera_object->yaw + view_look_yaw) - 705) & 2047;
    section = column / 512;
    column = column % 512;
    if (((unsigned)(((unsigned)game_minutes) % 1440)) < 720) {
        b_section = 0;
    } else {
        b_section = 2;
    }
    horizon_y += 75;
    width = 511 - column;
    if (width >= 320) {
        if (section == b_section) {
            xn_sky_copy_rows((char *)((sky_image_b + column) + src_offset), (char *)D_00147954, 320, horizon_y);
        } else {
            xn_sky_copy_rows((char *)((sky_image_a + column) + src_offset), (char *)D_00147954, 320, horizon_y);
        }
    } else {
        if (section == b_section) {
            xn_sky_copy_rows((char *)((sky_image_b + column) + src_offset), (char *)D_00147954, width, horizon_y);
        } else {
            xn_sky_copy_rows((char *)((sky_image_a + column) + src_offset), (char *)D_00147954, width, horizon_y);
        }
        if ((section + 1) == b_section) {
            xn_sky_copy_rows((char *)(sky_image_b + src_offset), (char *)(D_00147954 + width), 320 - width, horizon_y);
        } else {
            xn_sky_copy_rows((char *)(sky_image_a + src_offset), (char *)(D_00147954 + width), 320 - width, horizon_y);
        }
    }
    for (row = horizon_y; row < view_bottom; row++) {
        mc_memset((*(char **)&D_00147954 + (row * 320)), (int)(unsigned char)*(signed char *)(((char *)sky_image_a) + 109058), 320, D_00170A86, 636, 4);
    }
    mc_memcpy((void *)screen_buffer, (void *)D_00147954, view_bottom * 320, D_00170A86, 638, 4);
}

void sky_set_time_colour(int minutes)
{
    int colour_row;

    minutes = ((unsigned)minutes) % 1440;
    if (((unsigned)minutes) < 360) {
        colour_row = 0;
    } else if (((unsigned)minutes) > 1080) {
        colour_row = 0;
    } else if (((unsigned)minutes) > 488 && ((unsigned)minutes) < 952) {
        colour_row = 31;
    } else if (((unsigned)minutes) < 700) {
        colour_row = ((unsigned)(minutes - 360)) >> 2;
    } else {
        colour_row = ((unsigned)(-(minutes - 1080))) >> 2;
    }
    xn_pal_set_range_8bit((char *)(((iptr)D_0017A294) + (colour_row * 3)), 255, 1);
}

void sky_stub(int unused)
{
}

void sky_load_night(void)
{
    disk_read_file(D_00170A98, (iptr)scratch_buffer);
    xn_pal_set_range_8bit((char *)((iptr)scratch_buffer + 11), 1, 31);
    mc_memcpy((void *)((iptr)xn_pal_current + 3), (scratch_buffer + 11), 93, D_00170A86, 671, 4);
    mc_set_location(673, D_00170A86);
    mc_sprintf((char *)text_buffer, D_00170AA5, (int)(unsigned char)D_0017A3E5[climate_category()]);
    disk_read_file(text_buffer, sky_image_a);
    sky_add_stars((unsigned char *)sky_image_a);
    night_sky_loaded = 1;
}

void sky_draw_night(int horizon_y, int horizon_y2)
{
    int src_offset;
    int column;
    int width;
    int unused1;
    int section;
    int view_bottom;
    int row;
    int unused2;

    if (night_sky_loaded == 0) sky_load_night();
    view_bottom = ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : hud_bar_image->y);
    src_offset = (139 - horizon_y) << 9;
    if (src_offset < 0) src_offset = 0;
    column = ((camera_object->yaw + view_look_yaw) - 705) & 2047;
    section = column / 512;
    column = column % 512;
    horizon_y += 75;
    width = 511 - column;
    if (horizon_y > 0) {
        if (width >= 320) {
            xn_sky_copy_rows((char *)((sky_image_a + column) + src_offset), (char *)D_00147954, 320, horizon_y);
        } else {
            xn_sky_copy_rows((char *)((sky_image_a + column) + src_offset), (char *)D_00147954, width, horizon_y);
            xn_sky_copy_rows((char *)(sky_image_a + src_offset), (char *)(D_00147954 + width), 320 - width, horizon_y);
        }
    }
    if (horizon_y < 0) horizon_y = 0;
    for (row = horizon_y; row < view_bottom; row++) {
        mc_memset((*(char **)&D_00147954 + (row * 320)), 15, 320, D_00170A86, 713, 4);
    }
    mc_memcpy((void *)screen_buffer, (void *)D_00147954, view_bottom * 320, D_00170A86, 715, 4);
}

void sky_add_stars(unsigned char *image)
{
    int i;
    int x;
    int y_offset;

    for (i = 0; i < 300; i++) {
        x = rand_range(0, 511);
        y_offset = rand_range(0, 199) << 9;
        if (image[x + y_offset] < 16) continue;
        image[x + y_offset] = D_0017A3EB[rand() & 15];
    }
}
