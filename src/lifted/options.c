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
extern signed char D_00152A02;
extern char D_00152A04[];
extern int D_00152A0C;
extern int D_00152A10;
extern int D_00152A14;
extern int D_00152A18;
extern int D_00152A1C;
extern int D_00152A20;
extern int D_00152A24;
extern signed char D_00152A30[];
extern signed char D_00152A31;
extern char D_00170EE8[];
extern char D_00170EF2[];
extern char D_00170EFF[];
extern char D_00170F0C[];
extern char D_00170F54[];
extern char D_00170F61[];
extern char D_00170F6E[];
extern int D_001788E4;
extern int D_0017B41C[];
extern char options_buttons[];
extern char D_0017B792[];
extern char D_0017B794[];
extern char D_0017B796[];
extern char D_0017B798[];
extern char controls_buttons[];
extern char D_0017B80A[];
extern char D_0017B80C[];
extern char D_0017B80E[];
extern char D_0017B810[];
extern char options_joystick_buttons[];
extern char D_0017BA62[];
extern char D_0017BA64[];
extern char D_0017BA66[];
extern char D_0017BA68[];
extern short D_0017BA6C;
extern short D_0017BA6E;
extern short D_0017BA70;
extern short D_0017BA72;
extern int key_names[];
extern char default_key_map[];
extern signed char D_00187CA8;
extern struct record *player_object;
extern int D_00195B5C;
extern int D_00195B60;
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
extern char *D_00199708;
extern int options_image;
extern int options_saved_screen;

extern int options_open(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int disk_create(int);
extern int key_pressed_once(unsigned char);
extern int func_0009DEA7();
extern int func_0009DEAC();
extern int mc_free();
extern int mc_memset();
extern int write();
extern int mc_memcpy();
extern int func_000CB34E();
extern int func_000CDD81();
extern int func_000CE87B();
extern int func_000CE88D();
extern int func_0012A274();
extern int func_0012A2D0();
extern int func_0012B136();
extern int func_0012DB50();
extern int func_00144D00();
extern int func_00144F68();
extern int func_00144FB4();
extern int func_00152C40();
extern int func_00152D00();
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
int func_00044B8A(void);
int options_controls_is_duplicate(int);
void options_draw(void);
void options_controls_draw(int, int);
void options_joystick_draw(int, int);

void options_frame(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (options_open(0) == 0) return;
    func_0012DB50(4);
    if (key_pressed_once(1) != 0) {
        options_close();
        return;
    }
    options_draw();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (l_1C = 0; ((int)(short)*(short *)&l_1C) < 10; l_1C++) {
        if (mouse_x >= *(short *)(options_buttons + (((int)(short)*(short *)&l_1C) * 12)) && mouse_x <= *(short *)(D_0017B794 + (((int)(short)*(short *)&l_1C) * 12)) && mouse_y >= *(short *)(D_0017B792 + (((int)(short)*(short *)&l_1C) * 12)) && mouse_y <= *(short *)(D_0017B796 + (((int)(short)*(short *)&l_1C) * 12))) {
            sound_play(203, player_object, 100);
            ((int (*)())(*(int *)(D_0017B798 + (((int)(short)*(short *)&l_1C) * 12))))();
            return;
        }
    }
}

int options_close(void)
{
    do {
    } while (key_down_esc != 0);
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
    int l_20;
    int l_1C;
    int l_18;

    mc_memcpy(screen_buffer, options_saved_screen, 64000, (int)D_00170EE8, 176, 4);
    l_20 = options_image;
    func_00144F68((int)(unsigned short)*(short *)((char *)l_20), (int)(unsigned short)*(short *)((char *)l_20 + 2), (int)(unsigned short)*(short *)((char *)l_20 + 4), (int)(unsigned short)*(short *)((char *)l_20 + 6), l_20 + 12);
    D_0012B508 = 246;
    for (l_18 = 0; l_18 < 2; l_18++) {
        if ((game_settings->view_flags & (1 << l_18)) != 0) {
            l_1C = ((int)options_buttons) + ((l_18 + 6) * 12);
            func_00144D00((int)(short)(*(short *)((char *)l_1C + 4) - 5), (int)(short)(*(short *)((char *)l_1C + 2) + 3), 3, 3);
        }
    }
    if (game_settings->sound_volume != 0) {
        func_00144D00(91, 64, (int)(short)((((int)(short)game_settings->sound_volume) * 108) / 128), 3);
    }
    if (game_settings->music_volume != 0) {
        func_00144D00(91, 72, (int)(short)((((int)(short)game_settings->music_volume) * 108) / 128), 3);
    }
    if (((int)(unsigned short)(game_settings->view_flags & -256)) == 0) return;
    func_00144D00(91, 80, (int)(short)(((game_settings->view_flags >> 8) * 108) / 128), 3);
}

void options_save_game(void)
{
    options_close();
    saveload_menu(1);
    while (mouse_buttons != 0) func_0012B136();
}

void options_load_game(void)
{
    options_close();
    saveload_menu(0);
    while (mouse_buttons != 0) func_0012B136();
}

void options_exit_game(void)
{
    int l_18;

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
    int l_1C;

    l_1C = ((((int)(short)mouse_x) << 7) - 11648) / 108;
    if (l_1C < 0) {
        l_1C = 0;
    } else if (l_1C > 127) {
        l_1C = 127;
    }
    return l_1C;
}

void options_toggle_full_screen(void)
{
    game_settings->view_flags ^= 1;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        func_0012A2D0(160, 100, 160, 100);
    } else {
        func_0012A2D0(160, 77, 160, 77);
    }
    func_0012A274(200, 180);
}

void options_toggle_head_bobbing(void)
{
    game_settings->view_flags ^= 2;
}

int options_controls_rebind(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = -1;
    mc_memset((int)key_down, 0, 128, (int)D_00170EE8, 276, 128);
    while (mouse_buttons != 0) func_0012B136();
    while (l_1C == (-1)) {
        l_18 = func_000CE87B((int)key_down, 128);
        if (l_18 != 0) {
            l_1C = l_18 - ((int)key_down);
            if (l_1C == 1) return 0;
            if (l_1C < 2 || l_1C == 68) l_1C = -1;
        } else if (((int)(unsigned char)D_00152A02) == 1) {
            func_00152D00();
            if (D_00152A30[0] != 0) {
                l_1C = 200;
            } else if (D_00152A31 != 0) {
                l_1C = 201;
            } else if (a1 < 6) {
                if (func_0009DEAC(D_00152A20) > 2048) {
                    if (D_00152A20 < 0) {
                        l_1C = 204;
                    } else {
                        l_1C = 205;
                    }
                } else if (func_0009DEAC(D_00152A24) > 2048) {
                    if (D_00152A24 < 0) {
                        l_1C = 206;
                    } else {
                        l_1C = 207;
                    }
                }
            }
        }
        if (l_1C == (-1)) {
            func_0012B136();
            if (((int)(unsigned char)(mouse_buttons & 1)) != 0) {
                l_1C = 202;
            } else if (((int)(unsigned char)(mouse_buttons & 2)) != 0) {
                l_1C = 203;
            } else if (((int)(unsigned char)(mouse_buttons & 4)) != 0) {
                l_1C = 212;
            }
        }
        options_controls_draw(a1, a2);
        func_000CDD81(0);
    }
    *(signed char *)(key_map + a1) = *(signed char *)&l_1C;
    return 0;
}

void options_controls_draw(int a1, int a2)
{
    int l_18;
    int l_14;

    mc_memcpy(screen_buffer, a2, 64000, (int)D_00170EE8, 355, 4);
    if (((int)(unsigned char)mouse_control_mode) == 1) {
        func_00144F68((int)(unsigned short)*(short *)(D_00199708), (int)(unsigned short)*(short *)(D_00199708 + 2), (int)(unsigned short)*(short *)(D_00199708 + 4), (int)(unsigned short)*(short *)(D_00199708 + 6), (int)(D_00199708 + 12));
    }
    for (l_18 = 0; l_18 < 38; l_18++) {
        if (a1 == l_18) continue;
        l_14 = 146;
        if (options_controls_is_duplicate(l_18) != 0) l_14 = 244;
        D_0012B508 = *(signed char *)&l_14;
        if (((int)(unsigned char)*(signed char *)(key_map + l_18)) < 200) {
            text_draw_centred(key_names[((int)(unsigned char)*(signed char *)(key_map + l_18))], (((int)(short)*(short *)(D_0017B80C + (l_18 * 12))) + ((int)(short)*(short *)(controls_buttons + (l_18 * 12)))) / 2, (int)&*(signed char *)((char *)((int)(short)*(short *)(D_0017B80A + (l_18 * 12))) + 2));
        } else {
            text_draw_centred(D_0017B41C[((int)(unsigned char)*(signed char *)(key_map + l_18))], (((int)(short)*(short *)(D_0017B80C + (l_18 * 12))) + ((int)(short)*(short *)(controls_buttons + (l_18 * 12)))) / 2, (int)&*(signed char *)((char *)((int)(short)*(short *)(D_0017B80A + (l_18 * 12))) + 2));
        }
    }
}

void options_controls_screen(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 0;
    l_18 = disk_read_file((int)D_00170EF2, 0);
    *(int *)&D_00199708 = disk_read_file((int)D_00170EFF, 0);
    while (l_20 == 0) {
        if (key_down_esc != 0 && options_controls_check() != 0) break;
        mouse_buttons_prev = mouse_buttons;
        func_0012B136();
        options_controls_draw(-1, l_18);
        cursor_draw_arrow();
        if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
            for (l_24 = 0; l_24 < 42; l_24++) {
                if (mouse_x > *(short *)(controls_buttons + (l_24 * 12)) && mouse_x < *(short *)(D_0017B80C + (l_24 * 12)) && mouse_y > *(short *)(D_0017B80A + (l_24 * 12)) && mouse_y < *(short *)(D_0017B80E + (l_24 * 12))) {
                    sound_play(203, player_object, 100);
                    l_20 = ((int (*)())(*(int *)(D_0017B810 + (l_24 * 12))))(l_24, l_18);
                }
            }
        }
        func_000CDD81(0);
    }
    do {
    } while (key_down_esc != 0);
    if (l_18 != 0 && l_18 != (-1751672937)) {
        mc_free(l_18, (int)D_00170EE8, 401);
        l_18 = -1751672937;
    }
    if ((int)D_00199708 != 0 && (int)D_00199708 != (-1751672937)) {
        mc_free((int)D_00199708, (int)D_00170EE8, 402);
        *(int *)&D_00199708 = -1751672937;
    }
    l_1C = disk_create(D_001788E4);
    mc_memcpy((int)D_00195E82, (int)D_00152A04, 46, (int)D_00170EE8, 405, 4);
    write(l_1C, (int)&mouse_control_mode, 54);
    write(l_1C, (int)key_map, 38);
    func_0009DEA7(l_1C);
}

int options_controls_check(void)
{
    int l_20;
    int l_1C;

    for (l_20 = 0; l_20 < 38; l_20++) {
        for (l_1C = 0; l_1C < 38; l_1C++) {
            if (l_20 != l_1C && *(signed char *)(key_map + l_20) == *(signed char *)(key_map + l_1C)) {
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

void options_mouse_draw(int a1)
{
    int l_1C;
    int l_18;

    l_18 = a1;
    mc_memcpy(screen_buffer, options_saved_screen, 64000, (int)D_00170EE8, 480, 4);
    func_00144F68((int)(unsigned short)*(short *)((char *)l_18), (int)(unsigned short)*(short *)((char *)l_18 + 2), (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
    D_0012B508 = 246;
    func_00144D00((int)(short)((mouse_control_mode == 0) ? 134 : 220), 47, 5, 5);
    for (l_1C = 0; ((int)(unsigned char)mouse_sensitivity_x) > l_1C; l_1C++) {
        func_00144F68((l_1C * 7) + 139, 108, (int)(unsigned short)*(short *)(*(char **)&D_00195B5C + 4), (int)(unsigned short)*(short *)(*(char **)&D_00195B5C + 6), (int)(*(char **)&D_00195B5C + 12));
    }
    for (l_1C = 0; ((int)(unsigned char)mouse_sensitivity_y) > l_1C; l_1C++) {
        func_00144F68((l_1C * 7) + 139, 121, (int)(unsigned short)*(short *)(*(char **)&D_00195B5C + 4), (int)(unsigned short)*(short *)(*(char **)&D_00195B5C + 6), (int)(*(char **)&D_00195B5C + 12));
    }
    for (l_1C = 0; ((int)(unsigned char)(mouse_turn_rate & 127)) > l_1C; l_1C++) {
        func_00144F68((l_1C * 7) + 139, 134, (int)(unsigned short)*(short *)(*(char **)&D_00195B5C + 4), (int)(unsigned short)*(short *)(*(char **)&D_00195B5C + 6), (int)(*(char **)&D_00195B5C + 12));
    }
    if (((int)(unsigned char)(mouse_turn_rate & 128)) == 0) return;
    func_00144F68((int)(unsigned short)*(short *)(*(char **)&D_00195B60), (int)(unsigned short)*(short *)(*(char **)&D_00195B60 + 2), (int)(unsigned short)*(short *)(*(char **)&D_00195B60 + 4), (int)(unsigned short)*(short *)(*(char **)&D_00195B60 + 6), (int)(*(char **)&D_00195B60 + 12));
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
    func_000CE88D(((int)(unsigned char)mouse_sensitivity_x) * 6, ((int)(unsigned char)mouse_sensitivity_y) * 6);
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
    func_000CE88D(((int)(unsigned char)mouse_sensitivity_x) * 6, ((int)(unsigned char)mouse_sensitivity_y) * 6);
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
    int l_24;
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = disk_read_file((int)D_00170F54, 0);
    D_00195B5C = disk_read_file((int)D_00170F61, 0);
    D_00195B60 = disk_read_file((int)D_00170F6E, 0);
    while (l_20 == 0) {
        mouse_buttons_prev = mouse_buttons;
        func_0012B136();
        options_joystick_draw(0, l_1C);
        cursor_draw_arrow();
        if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
            for (l_24 = 0; l_24 < 6; l_24++) {
                if (mouse_x > *(short *)(options_joystick_buttons + (l_24 * 12)) && mouse_x < *(short *)(D_0017BA64 + (l_24 * 12)) && mouse_y > *(short *)(D_0017BA62 + (l_24 * 12)) && mouse_y < *(short *)(D_0017BA66 + (l_24 * 12))) {
                    sound_play(203, player_object, 100);
                    l_20 = ((int (*)())(*(int *)(D_0017BA68 + (l_24 * 12))))(l_24, l_1C);
                }
            }
        }
        func_000CDD81(0);
    }
    if (D_00195B60 != 0 && D_00195B60 != (-1751672937)) {
        mc_free(D_00195B60, (int)D_00170EE8, 580);
        D_00195B60 = -1751672937;
    }
    if (D_00195B5C != 0 && D_00195B5C != (-1751672937)) {
        mc_free(D_00195B5C, (int)D_00170EE8, 581);
        D_00195B5C = -1751672937;
    }
    if (l_1C != 0 && l_1C != (-1751672937)) {
        mc_free(l_1C, (int)D_00170EE8, 582);
        l_1C = -1751672937;
    }
    return 0;
}

void options_joystick_draw(int a1, int a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = a2;
    mc_memcpy(screen_buffer, options_saved_screen, 64000, (int)D_00170EE8, 593, 4);
    func_00144F68((int)(unsigned short)*(short *)((char *)l_1C), (int)(unsigned short)*(short *)((char *)l_1C + 2), (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), l_1C + 12);
    if (((int)(unsigned char)joystick_setting) == 2) {
        D_0012B508 = 246;
        func_00144D00(138, 42, 5, 5);
    }
    l_18 = (int)(short)joystick_threshold;
    if (l_18 == 10) {
        l_18 = 0;
    } else if (l_18 == 20) {
        l_18 = 1;
    } else {
        l_18 = 2;
    }
    l_14 = (int)(*(char **)&D_00195B60 + 12);
    func_000CB34E((((int)(short)*(short *)(options_joystick_buttons + ((l_18 + 2) * 12))) + l_14) - 113, (int)(*(char **)&screen_buffer + ((((int)(short)*(short *)(D_0017BA62 + ((l_18 + 2) * 12))) * 320) + ((int)(short)*(short *)(options_joystick_buttons + ((l_18 + 2) * 12))))), (int)&*(signed char *)((char *)(((int)(short)*(short *)(D_0017BA64 + ((l_18 + 2) * 12))) - ((int)(short)*(short *)(options_joystick_buttons + ((l_18 + 2) * 12)))) + 1), (int)&*(signed char *)((char *)(((int)(short)*(short *)(D_0017BA66 + ((l_18 + 2) * 12))) - ((int)(short)*(short *)(D_0017BA62 + ((l_18 + 2) * 12)))) + 1), (int)(unsigned short)*(short *)(*(char **)&D_00195B60 + 4));
    if (a1 == 0) return;
    if (D_00152A20 == 0) if (D_00152A24 == 0) return;
    l_1C = D_00195B5C;
    l_18 = func_00044B8A();
    if (l_18 == (-1)) return;
    while (l_18 != 0) {
        l_1C = (((int)(unsigned short)*(short *)((char *)l_1C + 10)) + l_1C) + 12;
        l_18--;
    }
    func_00144FB4((int)(unsigned short)*(short *)((char *)l_1C), (int)(unsigned short)*(short *)((char *)l_1C + 2), (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), l_1C + 12);
}

int options_joystick_disable(void)
{
    if (((int)(unsigned char)joystick_setting) == 1) {
        joystick_setting = 2;
    } else {
        joystick_setting = 1;
    }
    D_00152A02 = joystick_setting;
    return 0;
}

int options_joystick_calibrate_button(int a1, int a2)
{
    int l_18;

    l_18 = 0;
    if (((int)(unsigned char)D_00152A02) == 2) return 0;
    D_00152A10 = 32000;
    D_00152A18 = 32000;
    D_00152A14 = 0;
    D_00152A1C = 0;
    func_00152C40();
    while (l_18 == 0) {
        func_00152D00();
        mouse_buttons_prev = mouse_buttons;
        func_0012B136();
        options_joystick_draw(1, a2);
        cursor_draw_arrow();
        if (mouse_buttons != 0 && mouse_buttons_prev == 0 && mouse_x > D_0017BA6C && mouse_x < D_0017BA70 && mouse_y > D_0017BA6E && mouse_y < D_0017BA72) {
            l_18 = 1;
        }
        func_000CDD81(0);
    }
    return 0;
}

int options_joystick_threshold_low(void)
{
    D_00152A0C = (int)(short)(joystick_threshold = 10);
    return 0;
}

int options_joystick_threshold_med(void)
{
    D_00152A0C = (int)(short)(joystick_threshold = 20);
    return 0;
}

int options_joystick_threshold_high(void)
{
    D_00152A0C = (int)(short)(joystick_threshold = 40);
    return 0;
}

int func_00044B8A(void)
{
    if (D_00152A20 < 0) {
        if (D_00152A24 < 0) {
            if (D_00152A20 > (-2048)) return 0;
            if (D_00152A24 > (-2048)) return 6;
            return 7;
        }
        if (D_00152A20 > (-2048)) return 4;
        if (D_00152A24 < 2048) return 6;
        return 5;
    }
    if (D_00152A24 < 0) {
        if (D_00152A20 < 2048) return 0;
        if (D_00152A24 > (-2048)) return 2;
        return 1;
    }
    if (D_00152A20 < 2048) return 4;
    if (D_00152A24 < 2048) return 2;
    return 3;
}

int options_controls_is_duplicate(int a1)
{
    int l_1C;

    for (l_1C = 0; l_1C < 38; l_1C++) {
        if (l_1C == a1) continue;
        if (*(signed char *)(key_map + a1) == *(signed char *)(key_map + l_1C)) return 1;
    }
    return 0;
}
