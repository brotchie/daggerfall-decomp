/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003478D */
#include "records.h"

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
extern iptr screen_buffer;
extern char D_00170A86[];
extern unsigned char player_environment;
extern signed char D_00187CA8;
extern struct region regions[];
extern int view_look_pitch;
extern int view_look_yaw;
extern struct record *camera_object;
extern struct record *player_object;
extern int frame_ticks;
extern struct image *hud_bar_image;
extern int game_minutes;
extern struct settings *game_settings;
extern iptr D_00195CF4;
extern signed char climate_weathers[];
extern signed char current_region;
extern signed char D_001962A1;
extern struct cfa_header *moon0_image;
extern struct cfa_header *moon1_image;
extern int sun_light;
extern char moon1_direction[];
extern char moon0_direction[];
extern int sun_direction;
extern int daylight;
extern int climate_category(void);
extern void sky_sun_direction(int *, int, int);
extern void sky_load_day(int);
extern void sky_draw_day(int, int, int, int);
extern void sky_set_time_colour(int);
extern void sky_draw_night(int, int);
extern struct record *object_reparent(struct record *, struct record *);
extern struct record *location_cell_at(int, int);
extern int mc_memset();
extern int mc_memcpy();
extern int xn_mat_from_angles();
extern int xn_mat_transform_ptr();
extern int xn_cam_project_ptr();
extern int xn_cam_scale_matrix();

void sky_update(void)
{
    int day;
    int point_x;
    int point_y;
    int point_z;
    int moon_x;
    int moon_y;
    int moon_z;
    int screen_y;
    int minutes;
    int screen_x;
    int light;
    int horizon_y;
    int unused1;
    int horizon_row;
    int climate;
    int target_light;
    int light_step;
    int unused2;
    int unused3;
    int unused4;
    unsigned char weather;
    struct record *cell;
    unsigned char moons_visible;
    int unused5;
    unsigned char sun_placed;

    *(struct vec3 *)&point_x = *(struct vec3 *)D_000346B8;
    sun_placed = 0;
    if (D_00187CA8 == 0) return;
    if (((int)player_environment) == 3) {
        cell = location_cell_at(player_object->x, player_object->z);
        if (player_object->parent != cell) object_reparent(cell, player_object);
        target_light = cell->light_level << 8;
        light_step = xn_light_ambient;
        if ((target_light >> 8) != (light_step >> 8)) {
            light_step = ((target_light - light_step) * frame_ticks) / 1024;
            xn_light_ambient += light_step;
        } else {
            xn_light_ambient = cell->light_level << 8;
        }
        dungeon_water_level = cell->water_level;
        D_001962A1 = cell->block_special;
        return;
    }
    if (((int)player_environment) == 1) {
        sky_set_time_colour(game_minutes);
    }
    minutes = ((unsigned)game_minutes) % 1440;
    if (minutes > 360 && minutes < 1080) {
        day = 1;
    } else {
        day = 0;
    }
    daylight = day;
    if (((int)player_environment) == 2) {
        if (daylight != 0) {
            minutes += -360;
            if (minutes < 45) {
                light = (minutes * 63) / 45;
            } else if (minutes > 675) {
                light = 63 - (((minutes - 675) * 63) / 45);
            } else {
                light = 63;
            }
        } else {
            light = 24;
        }
        light >>= 1;
        if ((xn_light_ambient = light << 8) < 2560) xn_light_ambient = 2560;
        return;
    }
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        point_y = 450;
    } else {
        point_y = 140;
    }
    xn_cam_pitch = (camera_object->angle_x + view_look_pitch) & 2047;
    xn_cam_yaw = 0;
    xn_cam_roll = 0;
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (iptr)xn_cam_rotation);
    xn_cam_scale_matrix((iptr)xn_cam_rotation, (iptr)xn_cam_view_matrix);
    xn_mat_transform_ptr((iptr)&point_x, (iptr)&point_y, (iptr)&point_z, (iptr)xn_cam_rotation);
    xn_cam_project_ptr(point_x, point_y, 3000, (iptr)&screen_x, (iptr)&screen_y);
    horizon_row = screen_y + 75;
    if (horizon_row < 0) horizon_row = 0;
    if (horizon_row > 199) horizon_row = 199;
    if (daylight == 0) xn_light_ambient = 4096;
    horizon_y = screen_y;
    sun_light = 0;
    climate = climate_category();
    weather = climate_weathers[climate];
    if (regions[(unsigned char)current_region].precipitation_override != 0) {
        weather = regions[(unsigned char)current_region].precipitation_override - 1;
    }
    minutes += -360;
    if (minutes < 45) {
        light = (minutes * 63) / 45;
    } else if (minutes > 675) {
        light = 63 - (((minutes - 675) * 63) / 45);
    } else {
        light = 63;
    }
    if (daylight == 0) light = 8;
    xn_cam_pitch = (camera_object->angle_x + view_look_pitch) & 2047;
    xn_cam_yaw = (camera_object->yaw + view_look_yaw) & 2047;
    xn_cam_roll = 0;
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (iptr)xn_cam_rotation);
    xn_cam_scale_matrix((iptr)xn_cam_rotation, (iptr)xn_cam_view_matrix);
    sun_placed = 1;
    sky_sun_direction(&sun_direction, 0, minutes);
    sun_light = light;
    if (((int)(unsigned char)(climate_weathers[climate] & 127)) == 5) light <<= 1;
    if ((xn_light_ambient = (light >> 1) << 8) < 2048) xn_light_ambient = 2048;
    if (daylight != 0) {
        if (screen_y > (-75)) {
            sky_load_day(game_minutes);
            sky_draw_day(screen_y, horizon_y, daylight, climate);
            return;
        }
        mc_memset(screen_buffer, (int)(unsigned char)*(signed char *)(((char *)D_00195CF4)), ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 199 : hud_bar_image->y) * 320, (iptr)D_00170A86, 251, 4);
    } else {
        sky_draw_night(screen_y, horizon_y);
    }
    if (horizon_row == 0) return;
    if (daylight == 0 && ((int)(unsigned char)weather) > 1) {
        mc_memset(screen_buffer, 223, 64000, (iptr)D_00170A86, 262, 4);
        sun_light = 0;
        return;
    }
    moons_visible = 0;
    mc_memcpy((iptr)&moon_x, (iptr)moon0_direction, 12, (iptr)D_00170A86, 271, 4);
    xn_mat_transform_ptr((iptr)&moon_x, (iptr)&moon_y, (iptr)&moon_z, (iptr)xn_cam_rotation);
    if (moon_z > 100) {
        xn_cam_project_ptr(moon_x, moon_y, moon_z, (iptr)&screen_x, (iptr)&screen_y);
        moon0_image->x = (screen_x + xn_cam_centre_x) - (moon0_image->width >> 1);
        moon0_image->y = (((int)(short)xn_cam_centre_y) + screen_y) - (moon0_image->height >> 1);
        moons_visible |= 1;
    }
    mc_memcpy((iptr)&moon_x, (iptr)moon1_direction, 12, (iptr)D_00170A86, 281, 4);
    xn_mat_transform_ptr((iptr)&moon_x, (iptr)&moon_y, (iptr)&moon_z, (iptr)xn_cam_rotation);
    if (moon_z > 100) {
        xn_cam_project_ptr(moon_x, moon_y, moon_z, (iptr)&screen_x, (iptr)&screen_y);
        moon1_image->x = (screen_x + xn_cam_centre_x) - (moon1_image->width >> 1);
        moon1_image->y = (((int)(short)xn_cam_centre_y) + screen_y) - (moon1_image->height >> 1);
        moons_visible |= 2;
    }
    xn_cam_pitch = (camera_object->angle_x + view_look_pitch) & 2047;
    xn_cam_yaw = (((((unsigned)((((unsigned)game_minutes) % 518400) * 2047)) / 518400) + (((unsigned)((((unsigned)game_minutes) % 1440) * 2047)) / 1440)) + (camera_object->yaw + view_look_yaw)) & 2047;
    xn_cam_roll = 0;
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (iptr)xn_cam_rotation);
    xn_cam_scale_matrix((iptr)xn_cam_rotation, (iptr)xn_cam_view_matrix);
    if (daylight != 0) if (light == 63) {}
    if (daylight != 0) return;
    sun_light = 0;
}
