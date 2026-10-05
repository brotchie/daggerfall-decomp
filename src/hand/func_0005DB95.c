/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005DB95 */
#include "records.h"

#pragma pack(1)
struct Img { unsigned short x; unsigned short y; unsigned short w; unsigned short h; char pad[4]; char data[1]; };
extern int xn_cam_yaw;
extern char *screen_buffer;
extern char D_00175898[];
extern struct record *detect_target;
extern struct record *player_object;
extern struct settings *game_settings;
extern char *compass_image;
extern struct Img *compass_box_image;
extern char game_mode;
extern int ai_angle_diff(int, int, int *);
extern void mc_memcpy(char *, char *, int, char *, int, int);
extern int xn_math_angle_to_point();
extern int xn_draw_image();

void hud_draw_heading_strip(int a1)
{
    int i;
    int off;
    int dir;
    int dist;
    int pos;

    if (a1 == 0) {
        if ((int)(unsigned short)(game_settings->view_flags & 1) == 0)
            return;
        if (game_mode != 0)
            return;
        xn_draw_image(compass_box_image->x, compass_box_image->y, compass_box_image->w, compass_box_image->h, compass_box_image->data);
        off = ((short)(player_object->yaw & 0x7ff) << 5) / 256;
        for (i = 185; i <= 197; i++)
            mc_memcpy(screen_buffer + (i * 320 + 253), (i - 185) * 322 + (compass_image + off), 65, D_00175898, 440, 4);
        if (detect_target != 0) {
            dir = ai_angle_diff(player_object->yaw, xn_math_angle_to_point(player_object->x, player_object->z, detect_target->x, detect_target->z), &dist);
            dir = (dir << 5) / 256;
            if (dir > 32)
                dir = 32;
            pos = dir * dist + 57885;
            screen_buffer[pos - 2] = screen_buffer[pos - 1] = screen_buffer[pos] = screen_buffer[pos + 1] = screen_buffer[pos + 2] = 245;
            pos += 320;
            screen_buffer[pos - 1] = screen_buffer[pos] = screen_buffer[pos + 1] = 245;
            pos += 320;
            screen_buffer[pos] = 245;
        }
        return;
    }
    xn_draw_image(compass_box_image->x - 250, compass_box_image->y - 11, compass_box_image->w, compass_box_image->h, compass_box_image->data);
    off = ((xn_cam_yaw & 2047) << 5) / 256;
    for (i = 174; i <= 186; i++)
        mc_memcpy(screen_buffer + (i * 320 + 3), (i - 174) * 322 + (compass_image + off), 65, D_00175898, 460, 4);
}
