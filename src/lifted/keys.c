/* keys.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern signed char mouse_buttons;
extern signed char key_down[];
extern signed char joystick_button1[];
extern signed char key_was_down[];
extern char key_map[];
extern signed char mouse_buttons_prev;
extern signed char joystick_button_latch[];


int key_action_pressed(int action)
{
    int button;

    if (((int)(unsigned char)*(signed char *)(key_map + action)) >= 200) {
        switch (*(unsigned char *)(key_map + action)) {
        case 200:
        case 201:
            button = ((((int)(unsigned char)*(signed char *)(key_map + action)) == 200) ? 0 : 1);
            if (joystick_button1[button] != 0 && joystick_button_latch[button] == 0) {
                joystick_button_latch[button] = 1;
                return 1;
            }
            if (joystick_button1[button] == 0) joystick_button_latch[button] = 0;
            return 0;
        case 202:
            return (((((int)(unsigned char)(mouse_buttons & 1)) != 0) && (((int)(unsigned char)(mouse_buttons_prev & 1)) == 0)) ? 1 : 0);
        case 203:
            return (((((int)(unsigned char)(mouse_buttons & 2)) != 0) && (((int)(unsigned char)(mouse_buttons_prev & 2)) == 0)) ? 1 : 0);
        case 212:
            return (((((int)(unsigned char)(mouse_buttons & 4)) != 0) && (((int)(unsigned char)(mouse_buttons_prev & 4)) == 0)) ? 1 : 0);
        }
        return 0;
    }
    if (key_down[(int)(unsigned char)*(signed char *)(key_map + action)] != 0 && key_was_down[(int)(unsigned char)*(signed char *)(key_map + action)] == 0) {
        key_was_down[(int)(unsigned char)*(signed char *)(key_map + action)] = 1;
        return 1;
    }
    if (key_down[(int)(unsigned char)*(signed char *)(key_map + action)] == 0) {
        key_was_down[(int)(unsigned char)*(signed char *)(key_map + action)] = 0;
    }
    return 0;
}
