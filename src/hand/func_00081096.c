/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00081096 */
#include "records.h"

extern int D_000C5400;
extern int D_001940D4;
extern int D_001940DA;
extern int player_motion_flags;
extern int view_look_pitch;
extern int D_001959BC;
extern struct record *camera_object;
extern struct record *player_object;
extern struct character *player_character;
extern unsigned char mouse_control_mode;
extern char mouse_turn_rate;
extern char view_cursor_active;
extern short mouse_motion_x;
extern short mouse_motion_y;
extern char game_mode;
extern char player_on_ground;
extern char in_dungeon_water;
extern int key_action_held(int);
extern void func_0007EED8(void);

void player_mouse_look(void)
{
    short dy;
    short dx;

    if (game_mode || (D_001940D4 & 0x24))
        return;
    if (mouse_control_mode == 1) {
        if (!view_cursor_active && !key_action_held(33)) {
            func_0007EED8();
            if (!game_mode && (player_on_ground || in_dungeon_water || (player_motion_flags & 0x20) || (player_character->conditions & 8) ? 1 : 0)) {
                dy = mouse_motion_x;
                dx = mouse_motion_y;
                if ((unsigned char)mouse_turn_rate & 0x80)
                    dx = -dx;
                player_object->angle_x += dx;
                player_object->yaw += dy;
                if (dy > 2)
                    D_000C5400 = -8;
                else if (dy < -2)
                    D_000C5400 = 8;
                if (player_object->angle_x < -256)
                    player_object->angle_x = -256;
                else if (player_object->angle_x > 256)
                    player_object->angle_x = 256;
                camera_object->angle_x = player_object->angle_x;
                camera_object->yaw = player_object->yaw;
            }
        }
    } else if (D_001940DA & 0x40) {
        dy = mouse_motion_x;
        dx = mouse_motion_y;
        view_look_pitch += dx;
        D_001959BC += dy;
        if (view_look_pitch < -256)
            view_look_pitch = -256;
        else if (view_look_pitch > 256)
            view_look_pitch = 256;
        if (D_001959BC < -512)
            D_001959BC = -512;
        else if (D_001959BC > 512)
            D_001959BC = 512;
    }
}
