/* options.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down[];
extern signed char key_down_esc;
extern int screen_buffer;
extern signed char joystick_status;
extern char xn_joy_calibration[];
extern int xn_joy_dead_zone;
extern int xn_joy_min_x;
extern int D_00152A14;
extern int D_00152A18;
extern int D_00152A1C;
extern int joystick_x;
extern int joystick_y;
extern signed char joystick_button1[];
extern signed char joystick_button2;
extern char D_00170EE8[];
extern char D_00170EF2[];
extern char D_00170EFF[];
extern char D_00170F0C[];
extern char D_00170F54[];
extern char D_00170F61[];
extern char D_00170F6E[];
extern int controls_file;
extern int binding_names_base[];
extern struct rect options_buttons[];
extern struct rect controls_buttons[];
extern struct rect options_joystick_buttons[];
extern int key_names[];
extern char default_key_map[];
extern signed char D_00187CA8;
extern struct record *player_object;
extern struct image *D_00195B5C;
extern struct image *D_00195B60;
extern struct settings *game_settings;
extern signed char mouse_control_mode;
extern signed char mouse_turn_rate;
extern signed char mouse_sensitivity_x;
extern signed char mouse_sensitivity_y;
extern signed char joystick_setting;
extern short joystick_threshold;
extern char D_00195E82[];
extern char key_map[];
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern struct image *controls_view_image;
extern int options_image;
extern int options_saved_screen;

extern int options_open(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int disk_create(int);
extern int key_pressed_once(unsigned char);
extern int close();
extern int abs();
extern int mc_free();
extern int mc_memset();
extern int write();
extern int mc_memcpy();
extern int xn_draw_copy_rect_stride();
extern int xn_gfx_present_inclusive();
extern int xn_str_find_nonzero();
extern int xn_mouse_set_sensitivity();
extern int xn_cam_set_focal();
extern int xn_cam_set_view_window();
extern int xn_mouse_poll_clamped();
extern int xn_font_select();
extern int xn_draw_fill_rect();
extern int xn_draw_image();
extern int xn_draw_image_transparent();
extern int xn_joy_calibrate();
extern int xn_joy_poll();
extern void msgbox_show_string(int, int);
extern void game_exit(int);
extern void text_draw_centred(int, int, int);
extern void sound_set_volume(short);
extern void saveload_menu(int);
extern void msgbox_yes_no_rsc(int);
extern void cursor_draw_arrow(void);
int options_close(void);
int options_slider_value(void);
int options_controls_check(void);
int options_joystick_direction(void);
int options_controls_is_duplicate(int);
void options_draw(void);
void options_controls_draw(int, int);
void options_joystick_draw(int, int);

void options_frame(void)
{
    int unused;
    int button;
    int unused2;

    if (options_open(0) == 0) return;
    xn_font_select(4);
    if (key_pressed_once(1) != 0) {
        options_close();
        return;
    }
    options_draw();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (button = 0; ((int)(short)*(short *)&button) < 10; button++) {
        if (mouse_x >= options_buttons[(int)(short)*(short *)&button].x0 && mouse_x <= options_buttons[(int)(short)*(short *)&button].x1 && mouse_y >= options_buttons[(int)(short)*(short *)&button].y0 && mouse_y <= options_buttons[(int)(short)*(short *)&button].y1) {
            sound_play(203, player_object, 100);
            options_buttons[(int)(short)*(short *)&button].handler();
            return;
        }
    }
}

int options_close(void)
{
    while (key_down_esc != 0);
    D_00187CA8 = 1;
    game_mode = 0;
    if (options_image != 0 && options_image != (-1751672937)) {
        mc_free(options_image, (int)D_00170EE8, 164);
        options_image = -1751672937;
    }
    D_00196272 = 0;
    if (options_saved_screen != 0 && options_saved_screen != (-1751672937)) {
        mc_free(options_saved_screen, (int)D_00170EE8, 166);
        options_saved_screen = -1751672937;
    }
    return 1;
}

void options_draw(void)
{
    struct image *image;
    struct rect *box;
    int i;

    mc_memcpy(screen_buffer, options_saved_screen, 64000, (int)D_00170EE8, 176, 4);
    image = (struct image *)options_image;
    xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
    D_0012B508 = 246;
    for (i = 0; i < 2; i++) {
        if ((game_settings->view_flags & (1 << i)) != 0) {
            box = &options_buttons[i + 6];
            xn_draw_fill_rect((short)(box->x1 - 5), (short)(box->y0 + 3), 3, 3);
        }
    }
    if (game_settings->sound_volume != 0) {
        xn_draw_fill_rect(91, 64, (int)(short)((((int)(short)game_settings->sound_volume) * 108) / 128), 3);
    }
    if (game_settings->music_volume != 0) {
        xn_draw_fill_rect(91, 72, (int)(short)((((int)(short)game_settings->music_volume) * 108) / 128), 3);
    }
    if (((int)(unsigned short)(game_settings->view_flags & -256)) == 0) return;
    xn_draw_fill_rect(91, 80, (int)(short)(((game_settings->view_flags >> 8) * 108) / 128), 3);
}

void options_save_game(void)
{
    options_close();
    saveload_menu(1);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
}

void options_load_game(void)
{
    options_close();
    saveload_menu(0);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
}

void options_exit_game(void)
{
    int unused;

    msgbox_yes_no_rsc(1069);
    if (((int)D_00196271) != 1) return;
    options_close();
    game_exit(0);
}

void options_sound_slider(void)
{
    game_settings->sound_volume = options_slider_value();
}

void options_music_slider(void)
{
    game_settings->music_volume = options_slider_value();
    sound_set_volume((int)(short)game_settings->music_volume);
}

void options_detail_slider(void)
{
    game_settings->view_flags = (options_slider_value() << 8) | (game_settings->view_flags & 255);
}

int options_slider_value(void)
{
    int value;

    value = ((((int)(short)mouse_x) << 7) - 11648) / 108;
    if (value < 0) {
        value = 0;
    } else if (value > 127) {
        value = 127;
    }
    return value;
}

void options_toggle_full_screen(void)
{
    game_settings->view_flags ^= 1;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        xn_cam_set_view_window(160, 100, 160, 100);
    } else {
        xn_cam_set_view_window(160, 77, 160, 77);
    }
    xn_cam_set_focal(200, 180);
}

void options_toggle_head_bobbing(void)
{
    game_settings->view_flags ^= 2;
}

int options_controls_rebind(int action, int background)
{
    int code;
    signed char *pressed_key;

    code = -1;
    mc_memset((int)key_down, 0, 128, (int)D_00170EE8, 276, 128);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    while (code == (-1)) {
        pressed_key = (signed char *)xn_str_find_nonzero(key_down, 128);
        if (pressed_key != 0) {
            code = pressed_key - key_down;
            if (code == 1) return 0;
            if (code < 2 || code == 68) code = -1;
        } else if (((int)(unsigned char)joystick_status) == 1) {
            xn_joy_poll();
            if (joystick_button1[0] != 0) {
                code = 200;
            } else if (joystick_button2 != 0) {
                code = 201;
            } else if (action < 6) {
                if (abs(joystick_x) > 2048) {
                    if (joystick_x < 0) {
                        code = 204;
                    } else {
                        code = 205;
                    }
                } else if (abs(joystick_y) > 2048) {
                    if (joystick_y < 0) {
                        code = 206;
                    } else {
                        code = 207;
                    }
                }
            }
        }
        if (code == (-1)) {
            xn_mouse_poll_clamped();
            if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
                code = 202;
            } else if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
                code = 203;
            } else if (((int)(unsigned char)(mouse_buttons & 4)) != 0) {
                code = 212;
            }
        }
        options_controls_draw(action, background);
        xn_gfx_present_inclusive(0);
    }
    *(signed char *)(key_map + action) = *(signed char *)&code;
    return 0;
}

void options_controls_draw(int editing, int background)
{
    int action;
    int colour;

    mc_memcpy(screen_buffer, background, 64000, (int)D_00170EE8, 355, 4);
    if (((int)(unsigned char)mouse_control_mode) == 1) {
        xn_draw_image(controls_view_image->x, controls_view_image->y, controls_view_image->width, controls_view_image->height, (int)controls_view_image->pixels);
    }
    for (action = 0; action < 38; action++) {
        if (editing == action) continue;
        colour = 146;
        if (options_controls_is_duplicate(action) != 0) colour = 244;
        D_0012B508 = *(signed char *)&colour;
        if (((int)(unsigned char)*(signed char *)(key_map + action)) < 200) {
            text_draw_centred(key_names[((int)(unsigned char)*(signed char *)(key_map + action))], (controls_buttons[action].x1 + controls_buttons[action].x0) / 2, (int)&*(signed char *)((char *)(controls_buttons[action].y0) + 2));
        } else {
            text_draw_centred(binding_names_base[((int)(unsigned char)*(signed char *)(key_map + action))], (controls_buttons[action].x1 + controls_buttons[action].x0) / 2, (int)&*(signed char *)((char *)(controls_buttons[action].y0) + 2));
        }
    }
}

void options_controls_screen(void)
{
    int button;
    int done;
    int file;
    int background;

    done = 0;
    background = disk_read_file((int)D_00170EF2, 0);
    controls_view_image = (struct image *)disk_read_file((int)D_00170EFF, 0);
    while (done == 0) {
        if (key_down_esc != 0 && options_controls_check() != 0) break;
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        options_controls_draw(-1, background);
        cursor_draw_arrow();
        if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
            for (button = 0; button < 42; button++) {
                if (mouse_x > controls_buttons[button].x0 && mouse_x < controls_buttons[button].x1 && mouse_y > controls_buttons[button].y0 && mouse_y < controls_buttons[button].y1) {
                    sound_play(203, player_object, 100);
                    done = controls_buttons[button].handler(button, background);
                }
            }
        }
        xn_gfx_present_inclusive(0);
    }
    while (key_down_esc != 0);
    if (background != 0 && background != (-1751672937)) {
        mc_free(background, (int)D_00170EE8, 401);
        background = -1751672937;
    }
    if ((int)controls_view_image != 0 && (int)controls_view_image != (-1751672937)) {
        mc_free((int)controls_view_image, (int)D_00170EE8, 402);
        controls_view_image = (struct image *)-1751672937;
    }
    file = disk_create(controls_file);
    mc_memcpy((int)D_00195E82, (int)xn_joy_calibration, 46, (int)D_00170EE8, 405, 4);
    write(file, (int)&mouse_control_mode, 54);
    write(file, (int)key_map, 38);
    close(file);
}

int options_controls_check(void)
{
    int i;
    int j;

    for (i = 0; i < 38; i++) {
        for (j = 0; j < 38; j++) {
            if (i != j && *(signed char *)(key_map + i) == *(signed char *)(key_map + j)) {
                msgbox_show_string((int)D_00170F0C, 1);
                return 0;
            }
        }
    }
    return 1;
}

void options_controls_defaults(void)
{
    mc_memcpy((int)key_map, (int)default_key_map, 38, (int)D_00170EE8, 432, 38);
}

void options_mouse_draw(int background)
{
    int i;
    struct image *image;

    image = (struct image *)background;
    mc_memcpy(screen_buffer, options_saved_screen, 64000, (int)D_00170EE8, 480, 4);
    xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
    D_0012B508 = 246;
    xn_draw_fill_rect((int)(short)((mouse_control_mode == 0) ? 134 : 220), 47, 5, 5);
    for (i = 0; ((int)(unsigned char)mouse_sensitivity_x) > i; i++) {
        xn_draw_image((i * 7) + 139, 108, D_00195B5C->width, D_00195B5C->height, (int)D_00195B5C->pixels);
    }
    for (i = 0; ((int)(unsigned char)mouse_sensitivity_y) > i; i++) {
        xn_draw_image((i * 7) + 139, 121, D_00195B5C->width, D_00195B5C->height, (int)D_00195B5C->pixels);
    }
    for (i = 0; ((int)(unsigned char)(mouse_turn_rate & 127)) > i; i++) {
        xn_draw_image((i * 7) + 139, 134, D_00195B5C->width, D_00195B5C->height, (int)D_00195B5C->pixels);
    }
    if (((int)(unsigned char)(mouse_turn_rate & 128)) == 0) return;
    xn_draw_image(D_00195B60->x, D_00195B60->y, D_00195B60->width, D_00195B60->height, (int)D_00195B60->pixels);
}

int options_mouse_cursor_mode(void)
{
    mouse_control_mode = 0;
    return 0;
}

int options_mouse_horizontal(void)
{
    mouse_sensitivity_x = ((((int)(short)mouse_x) - 139) / 6) + 1;
    if (((int)(unsigned char)mouse_sensitivity_x) > 15) {
        mouse_sensitivity_x = 15;
    }
    if (((int)(unsigned char)mouse_sensitivity_x) < 1) {
        mouse_sensitivity_x = 1;
    }
    xn_mouse_set_sensitivity(((int)(unsigned char)mouse_sensitivity_x) * 6, ((int)(unsigned char)mouse_sensitivity_y) * 6);
    return 0;
}

int options_mouse_vertical(void)
{
    mouse_sensitivity_y = ((((int)(short)mouse_x) - 139) / 6) + 1;
    if (((int)(unsigned char)mouse_sensitivity_y) > 15) {
        mouse_sensitivity_y = 15;
    }
    if (((int)(unsigned char)mouse_sensitivity_y) < 1) {
        mouse_sensitivity_y = 1;
    }
    xn_mouse_set_sensitivity(((int)(unsigned char)mouse_sensitivity_x) * 6, ((int)(unsigned char)mouse_sensitivity_y) * 6);
    return 0;
}

int options_mouse_turn_rate(void)
{
    mouse_turn_rate = ((int)(unsigned char)(mouse_turn_rate & 128 & 255)) | ((((int)(short)mouse_x) - 139) / 7);
    return 0;
}

int options_mouse_reverse_vertical(void)
{
    mouse_turn_rate ^= 128;
    return 0;
}

int options_screen_continue(void)
{
    return 1;
}

int options_joystick_screen(void)
{
    int button;
    int done;
    int background;

    done = 0;
    background = disk_read_file((int)D_00170F54, 0);
    D_00195B5C = (struct image *)disk_read_file((int)D_00170F61, 0);
    D_00195B60 = (struct image *)disk_read_file((int)D_00170F6E, 0);
    while (done == 0) {
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        options_joystick_draw(0, background);
        cursor_draw_arrow();
        if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
            for (button = 0; button < 6; button++) {
                if (mouse_x > options_joystick_buttons[button].x0 && mouse_x < options_joystick_buttons[button].x1 && mouse_y > options_joystick_buttons[button].y0 && mouse_y < options_joystick_buttons[button].y1) {
                    sound_play(203, player_object, 100);
                    done = options_joystick_buttons[button].handler(button, background);
                }
            }
        }
        xn_gfx_present_inclusive(0);
    }
    if ((int)D_00195B60 != 0 && (int)D_00195B60 != (-1751672937)) {
        mc_free((int)D_00195B60, (int)D_00170EE8, 580);
        D_00195B60 = (struct image *)-1751672937;
    }
    if ((int)D_00195B5C != 0 && (int)D_00195B5C != (-1751672937)) {
        mc_free((int)D_00195B5C, (int)D_00170EE8, 581);
        D_00195B5C = (struct image *)-1751672937;
    }
    if (background != 0 && background != (-1751672937)) {
        mc_free(background, (int)D_00170EE8, 582);
        background = -1751672937;
    }
    return 0;
}

void options_joystick_draw(int calibrating, int background)
{
    struct image *image;
    int index;
    int pixels;

    image = (struct image *)background;
    mc_memcpy(screen_buffer, options_saved_screen, 64000, (int)D_00170EE8, 593, 4);
    xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
    if (((int)(unsigned char)joystick_setting) == 2) {
        D_0012B508 = 246;
        xn_draw_fill_rect(138, 42, 5, 5);
    }
    index = (int)(short)joystick_threshold;
    if (index == 10) {
        index = 0;
    } else if (index == 20) {
        index = 1;
    } else {
        index = 2;
    }
    pixels = (int)D_00195B60->pixels;
    xn_draw_copy_rect_stride((options_joystick_buttons[index + 2].x0 + pixels) - 113, (int)(*(char **)&screen_buffer + ((options_joystick_buttons[index + 2].y0 * 320) + options_joystick_buttons[index + 2].x0)), (int)&*(signed char *)((char *)(options_joystick_buttons[index + 2].x1 - options_joystick_buttons[index + 2].x0) + 1), (int)&*(signed char *)((char *)(options_joystick_buttons[index + 2].y1 - options_joystick_buttons[index + 2].y0) + 1), D_00195B60->width);
    if (calibrating == 0) return;
    if (joystick_x == 0) if (joystick_y == 0) return;
    image = D_00195B5C;
    index = options_joystick_direction();
    if (index == (-1)) return;
    while (index != 0) {
        image = (struct image *)((char *)image + image->data_size + 12);
        index--;
    }
    xn_draw_image_transparent(image->x, image->y, image->width, image->height, image->pixels);
}

int options_joystick_disable(void)
{
    if (((int)(unsigned char)joystick_setting) == 1) {
        joystick_setting = 2;
    } else {
        joystick_setting = 1;
    }
    joystick_status = joystick_setting;
    return 0;
}

int options_joystick_calibrate_button(int button, int background)
{
    int done;

    done = 0;
    if (((int)(unsigned char)joystick_status) == 2) return 0;
    xn_joy_min_x = 32000;
    D_00152A18 = 32000;
    D_00152A14 = 0;
    D_00152A1C = 0;
    xn_joy_calibrate();
    while (done == 0) {
        xn_joy_poll();
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        options_joystick_draw(1, background);
        cursor_draw_arrow();
        if (mouse_buttons != 0 && mouse_buttons_prev == 0 && mouse_x > options_joystick_buttons[1].x0 && mouse_x < options_joystick_buttons[1].x1 && mouse_y > options_joystick_buttons[1].y0 && mouse_y < options_joystick_buttons[1].y1) {
            done = 1;
        }
        xn_gfx_present_inclusive(0);
    }
    return 0;
}

int options_joystick_threshold_low(void)
{
    xn_joy_dead_zone = (int)(short)(joystick_threshold = 10);
    return 0;
}

int options_joystick_threshold_med(void)
{
    xn_joy_dead_zone = (int)(short)(joystick_threshold = 20);
    return 0;
}

int options_joystick_threshold_high(void)
{
    xn_joy_dead_zone = (int)(short)(joystick_threshold = 40);
    return 0;
}

int options_joystick_direction(void)
{
    if (joystick_x < 0) {
        if (joystick_y < 0) {
            if (joystick_x > (-2048)) return 0;
            if (joystick_y > (-2048)) return 6;
            return 7;
        }
        if (joystick_x > (-2048)) return 4;
        if (joystick_y < 2048) return 6;
        return 5;
    }
    if (joystick_y < 0) {
        if (joystick_x < 2048) return 0;
        if (joystick_y > (-2048)) return 2;
        return 1;
    }
    if (joystick_x < 2048) return 4;
    if (joystick_y < 2048) return 2;
    return 3;
}

int options_controls_is_duplicate(int action)
{
    int i;

    for (i = 0; i < 38; i++) {
        if (i == action) continue;
        if (*(signed char *)(key_map + action) == *(signed char *)(key_map + i)) return 1;
    }
    return 0;
}
