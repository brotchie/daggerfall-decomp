/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003478D */
#include "records.h"

struct s12 { int a[3]; };
extern char D_000346B8[];
extern int xn_cam_pitch;
extern int xn_cam_yaw;
extern int xn_cam_roll;
extern short xn_cam_centre_x;
extern short xn_cam_centre_y;
extern int dungeon_water_level;
extern int xn_light_ambient;
extern char xn_cam_rotation[];
extern char xn_cam_view_matrix[];
extern int screen_buffer;
extern char D_00170A86[];
extern unsigned char player_environment;
extern signed char D_00187CA8;
extern signed char region_precipitation_override[];
extern int view_look_pitch;
extern int view_look_yaw;
extern struct record *camera_object;
extern struct record *player_object;
extern int frame_ticks;
extern char hud_bar_image[];
extern int game_minutes;
extern struct settings *game_settings;
extern int D_00195CF4;
extern signed char climate_weathers[];
extern signed char current_region;
extern signed char D_001962A1;
extern char moon0_image[];
extern char moon1_image[];
extern int sun_light;
extern char moon1_direction[];
extern char moon0_direction[];
extern int sun_direction;
extern int daylight;
extern int climate_category(void);
extern void sky_sun_direction(int, int, int);
extern void sky_load_day(int);
extern void sky_draw_day(int, int, int, int);
extern void sky_set_time_colour(int);
extern void sky_draw_night(int, int);
extern int object_reparent(struct record *, struct record *);
extern struct record *location_cell_at(int, int);
extern int mc_memset();
extern int mc_memcpy();
extern int xn_mat_from_angles();
extern int xn_mat_transform_ptr();
extern int xn_cam_project_ptr();
extern int xn_cam_scale_matrix();

void sky_update(void)
{
    int l_78;
    int l_74;
    int l_70;
    int l_6C;
    int l_68;
    int l_64;
    int l_60;
    int l_5C;
    int l_58;
    int l_54;
    int l_50;
    int l_4C;
    int l_48;
    int l_44;
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    unsigned char l_1C;
    struct record *l_24;
    unsigned char l_20;
    int l_28;
    unsigned char l_18;

    *(struct s12 *)&l_74 = *(struct s12 *)D_000346B8;
    l_18 = 0;
    if (D_00187CA8 == 0) return;
    if (((int)player_environment) == 3) {
        l_24 = location_cell_at(player_object->x, player_object->z);
        if (player_object->parent != l_24) object_reparent(l_24, player_object);
        l_3C = l_24->light_level << 8;
        l_38 = xn_light_ambient;
        if ((l_3C >> 8) != (l_38 >> 8)) {
            l_38 = ((l_3C - l_38) * frame_ticks) / 1024;
            xn_light_ambient += l_38;
        } else {
            xn_light_ambient = l_24->light_level << 8;
        }
        dungeon_water_level = l_24->water_level;
        D_001962A1 = l_24->block_special;
        return;
    }
    if (((int)player_environment) == 1) {
        sky_set_time_colour(game_minutes);
    }
    l_58 = ((unsigned)game_minutes) % 1440;
    if (l_58 > 360 && l_58 < 1080) {
        l_78 = 1;
    } else {
        l_78 = 0;
    }
    daylight = l_78;
    if (((int)player_environment) == 2) {
        if (daylight != 0) {
            l_58 += -360;
            if (l_58 < 45) {
                l_50 = (l_58 * 63) / 45;
            } else if (l_58 > 675) {
                l_50 = 63 - (((l_58 - 675) * 63) / 45);
            } else {
                l_50 = 63;
            }
        } else {
            l_50 = 24;
        }
        l_50 >>= 1;
        if ((xn_light_ambient = l_50 << 8) < 2560) xn_light_ambient = 2560;
        return;
    }
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        l_70 = 450;
    } else {
        l_70 = 140;
    }
    xn_cam_pitch = (camera_object->angle_x + view_look_pitch) & 2047;
    xn_cam_yaw = 0;
    xn_cam_roll = 0;
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (int)xn_cam_rotation);
    xn_cam_scale_matrix((int)xn_cam_rotation, (int)xn_cam_view_matrix);
    xn_mat_transform_ptr((int)&l_74, (int)&l_70, (int)&l_6C, (int)xn_cam_rotation);
    xn_cam_project_ptr(l_74, l_70, 3000, (int)&l_54, (int)&l_5C);
    l_44 = l_5C + 75;
    if (l_44 < 0) l_44 = 0;
    if (l_44 > 199) l_44 = 199;
    if (daylight == 0) xn_light_ambient = 4096;
    l_4C = l_5C;
    sun_light = 0;
    l_40 = climate_category();
    l_1C = climate_weathers[l_40];
    if (region_precipitation_override[((int)(unsigned char)current_region) * 80] != 0) {
        l_1C = region_precipitation_override[((int)(unsigned char)current_region) * 80] - 1;
    }
    l_58 += -360;
    if (l_58 < 45) {
        l_50 = (l_58 * 63) / 45;
    } else if (l_58 > 675) {
        l_50 = 63 - (((l_58 - 675) * 63) / 45);
    } else {
        l_50 = 63;
    }
    if (daylight == 0) l_50 = 8;
    xn_cam_pitch = (camera_object->angle_x + view_look_pitch) & 2047;
    xn_cam_yaw = (camera_object->yaw + view_look_yaw) & 2047;
    xn_cam_roll = 0;
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (int)xn_cam_rotation);
    xn_cam_scale_matrix((int)xn_cam_rotation, (int)xn_cam_view_matrix);
    l_18 = 1;
    sky_sun_direction((int)&sun_direction, 0, l_58);
    sun_light = l_50;
    if (((int)(unsigned char)(climate_weathers[l_40] & 127)) == 5) l_50 <<= 1;
    if ((xn_light_ambient = (l_50 >> 1) << 8) < 2048) xn_light_ambient = 2048;
    if (daylight != 0) {
        if (l_5C > (-75)) {
            sky_load_day(game_minutes);
            sky_draw_day(l_5C, l_4C, daylight, l_40);
            return;
        }
        mc_memset(screen_buffer, (int)(unsigned char)*(signed char *)(((char *)D_00195CF4)), ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : (int)(unsigned short)*(short *)(*(char **)hud_bar_image + 2)) * 320, (int)D_00170A86, 251, 4);
    } else {
        sky_draw_night(l_5C, l_4C);
    }
    if (l_44 == 0) return;
    if (daylight == 0 && ((int)(unsigned char)l_1C) > 1) {
        mc_memset(screen_buffer, 223, 64000, (int)D_00170A86, 262, 4);
        sun_light = 0;
        return;
    }
    l_20 = 0;
    mc_memcpy((int)&l_68, (int)moon0_direction, 12, (int)D_00170A86, 271, 4);
    xn_mat_transform_ptr((int)&l_68, (int)&l_64, (int)&l_60, (int)xn_cam_rotation);
    if (l_60 > 100) {
        xn_cam_project_ptr(l_68, l_64, l_60, (int)&l_54, (int)&l_5C);
        *(short *)(*(char **)moon0_image + 6) = (l_54 + xn_cam_centre_x) - (**(unsigned short **)moon0_image >> 1);
        *(short *)(*(char **)moon0_image + 8) = (((int)(short)xn_cam_centre_y) + l_5C) - (((int)(unsigned short)*(short *)(*(char **)moon0_image + 2)) >> 1);
        l_20 |= 1;
    }
    mc_memcpy((int)&l_68, (int)moon1_direction, 12, (int)D_00170A86, 281, 4);
    xn_mat_transform_ptr((int)&l_68, (int)&l_64, (int)&l_60, (int)xn_cam_rotation);
    if (l_60 > 100) {
        xn_cam_project_ptr(l_68, l_64, l_60, (int)&l_54, (int)&l_5C);
        *(short *)(*(char **)moon1_image + 6) = (l_54 + xn_cam_centre_x) - (**(unsigned short **)moon1_image >> 1);
        *(short *)(*(char **)moon1_image + 8) = (((int)(short)xn_cam_centre_y) + l_5C) - (((int)(unsigned short)*(short *)(*(char **)moon1_image + 2)) >> 1);
        l_20 |= 2;
    }
    xn_cam_pitch = (camera_object->angle_x + view_look_pitch) & 2047;
    xn_cam_yaw = (((((unsigned)((((unsigned)game_minutes) % 518400) * 2047)) / 518400) + (((unsigned)((((unsigned)game_minutes) % 1440) * 2047)) / 1440)) + (camera_object->yaw + view_look_yaw)) & 2047;
    xn_cam_roll = 0;
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (int)xn_cam_rotation);
    xn_cam_scale_matrix((int)xn_cam_rotation, (int)xn_cam_view_matrix);
    if (daylight != 0) if (l_50 == 63) {}
    if (daylight != 0) return;
    sun_light = 0;
}
