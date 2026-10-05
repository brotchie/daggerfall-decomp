/* intrface.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern int D_001788CF;
extern short player_speed;
extern struct character *player_character;
extern signed char mouse_control_mode;
extern short D_00195F4E;
extern signed char D_0019628E;
extern int D_001A5AE8;
extern char turn_this_frame[];
extern int D_001A5AFC;
extern int move_angle_offset;
extern short steer_weight_down;
extern short steer_weight_right;
extern short steer_region_width;
extern short steer_region_height;
extern short steer_weight_left;
extern short steer_weight_up;
extern short steer_key_region;

extern int func_000CAE00();

void intrface_steer(int a1, int a2, int a3, int a4)
{
    int l_C;
    {
        unsigned char l_20;

        if (((int)(unsigned char)(mouse_buttons & 1)) != 0 || ((int)(short)steer_key_region) != (-1)) {
            if (((int)(short)steer_key_region) == (-1)) {
                D_001A5AFC = D_001788CF;
                steer_weight_right = ((((int)(short)mouse_x) - a3) << 8) / ((int)(short)steer_region_width);
                steer_weight_down = ((((int)(short)mouse_y) - a4) << 8) / ((int)(short)steer_region_height);
                steer_weight_left = ((((int)(short)steer_region_width) - (((int)(short)mouse_x) - a3)) << 8) / ((int)(short)steer_region_width);
                steer_weight_up = ((((int)(short)steer_region_height) - (((int)(short)mouse_y) - a4)) << 8) / ((int)(short)steer_region_height);
            } else {
                l_C = 1132;
                if ((D_001A5AFC = *(int *)((char *)l_C) - D_001A5AE8) > D_001788CF) {
                    D_001A5AFC = D_001788CF;
                }
            }
        }
        move_angle_offset = 0;
        player_speed = D_00195F4E;
        if ((player_character->conditions & 0x1) == 0 && ((((int)(unsigned char)mouse_control_mode) != 1 && ((int)(unsigned char)(mouse_buttons & 1)) != 0) || ((int)(short)steer_key_region) != (-1))) {
            func_000CAE00(a2);
        } else {
            player_speed = 0;
            *(int *)turn_this_frame = 0;
            move_angle_offset = 0;
        }
        if (((int)(short)player_speed) > 2) {
            l_20 = 1;
        } else {
            l_20 = 0;
        }
        D_0019628E = l_20;
    }
}
