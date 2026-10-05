/* options.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char key_down[];
extern char key_down_esc[];
extern char screen_buffer[];
extern char D_00152A02[];
extern char D_00152A04[];
extern char D_00152A0C[];
extern char D_00152A10[];
extern char D_00152A14[];
extern char D_00152A18[];
extern char D_00152A1C[];
extern char D_00152A20[];
extern char D_00152A24[];
extern char D_00152A30[];
extern char D_00152A31[];
extern char D_00170EE8[];
extern char D_00170EF2[];
extern char D_00170EFF[];
extern char D_00170F0C[];
extern char D_00170F54[];
extern char D_00170F61[];
extern char D_00170F6E[];
extern char D_001788E4[];
extern char D_0017B41C[];
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
extern char D_0017BA6C[];
extern char D_0017BA6E[];
extern char D_0017BA70[];
extern char D_0017BA72[];
extern char key_names[];
extern char default_key_map[];
extern char D_00187CA8[];
extern char player_object[];
extern char D_00195B5C[];
extern char D_00195B60[];
extern char game_settings[];
extern char mouse_control_mode[];
extern char mouse_turn_rate[];
extern char mouse_sensitivity_x[];
extern char mouse_sensitivity_y[];
extern char joystick_setting[];
extern char joystick_threshold[];
extern char D_00195E82[];
extern char key_map[];
extern char D_00196271[];
extern char D_00196272[];
extern char game_mode[];
extern char mouse_buttons_prev[];
extern char D_00199708[];
extern char options_image[];
extern char options_saved_screen[];

extern int options_open(int);
extern int sound_play(int, int, int);
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
    if (key_pressed_once(1) == 0) goto L43419;
    options_close();
    return;
L43419:;
    options_draw();
    if (*(signed char *)mouse_buttons == 0) goto L4343B;
    if (*(signed char *)mouse_buttons == 0) goto L43439;
    if (*(signed char *)mouse_buttons_prev != 0) goto L4343B;
L43439:;
    goto L43440;
L4343B:;
    return;
L43440:;
    l_1C = 0;
L43447:;
    if (((int)(short)*(short *)&l_1C) < 10) goto L4345D;
    return;
L43455:;
    l_1C++;
    goto L43447;
L4345D:;
    if (*(short *)mouse_x < *(short *)(options_buttons + (((int)(short)*(short *)&l_1C) * 12))) goto L4348B;
    if (*(short *)mouse_x <= *(short *)(D_0017B794 + (((int)(short)*(short *)&l_1C) * 12))) goto L4348D;
L4348B:;
    goto L434A4;
L4348D:;
    if (*(short *)mouse_y >= *(short *)(D_0017B792 + (((int)(short)*(short *)&l_1C) * 12))) goto L434A6;
L434A4:;
    goto L434BD;
L434A6:;
    if (*(short *)mouse_y <= *(short *)(D_0017B796 + (((int)(short)*(short *)&l_1C) * 12))) goto L434BF;
L434BD:;
    goto L434E3;
L434BF:;
    sound_play(203, *(int *)player_object, 100);
    ((int (*)())(*(int *)(D_0017B798 + (((int)(short)*(short *)&l_1C) * 12))))();
    return;
L434E3:;
    goto L43455;
}

int options_close(void)
{
L43500:;
    if (*(signed char *)key_down_esc != 0) goto L43500;
    *(signed char *)D_00187CA8 = 1;
    *(signed char *)game_mode = 0;
    if (*(int *)options_image == 0) goto L4352C;
    if (*(int *)options_image != (-1751672937)) goto L4352E;
L4352C:;
    goto L4354C;
L4352E:;
    mc_free(*(int *)options_image, (int)D_00170EE8, 164);
    *(int *)options_image = -1751672937;
L4354C:;
    *(signed char *)D_00196272 = 0;
    if (*(int *)options_saved_screen == 0) goto L43568;
    if (*(int *)options_saved_screen != (-1751672937)) goto L4356A;
L43568:;
    goto L43588;
L4356A:;
    mc_free(*(int *)options_saved_screen, (int)D_00170EE8, 166);
    *(int *)options_saved_screen = -1751672937;
L43588:;
    return 1;
}

void options_draw(void)
{
    int l_20;
    int l_1C;
    int l_18;

    mc_memcpy(*(int *)screen_buffer, *(int *)options_saved_screen, 64000, (int)D_00170EE8, 176, 4);
    l_20 = *(int *)options_image;
    func_00144F68((int)(unsigned short)*(short *)((char *)l_20), (int)(unsigned short)*(short *)((char *)l_20 + 2), (int)(unsigned short)*(short *)((char *)l_20 + 4), (int)(unsigned short)*(short *)((char *)l_20 + 6), l_20 + 12);
    *(signed char *)D_0012B508 = 246;
    l_18 = 0;
L43613:;
    if (l_18 < 2) goto L43623;
    goto L4367C;
L4361B:;
    l_18++;
    goto L43613;
L43623:;
    if ((*(unsigned short *)(*(char **)game_settings) & (1 << l_18)) == 0) goto L4367A;
    l_1C = ((int)options_buttons) + ((l_18 + 6) * 12);
    func_00144D00((int)(short)(*(short *)((char *)l_1C + 4) - 5), (int)(short)(*(short *)((char *)l_1C + 2) + 3), 3, 3);
L4367A:;
    goto L4361B;
L4367C:;
    if (*(short *)(*(char **)game_settings + 2) == 0) goto L436B9;
    func_00144D00(91, 64, (int)(short)((((int)(short)*(short *)(*(char **)game_settings + 2)) * 108) / 128), 3);
L436B9:;
    if (*(short *)(*(char **)game_settings + 4) == 0) goto L436F6;
    func_00144D00(91, 72, (int)(short)((((int)(short)*(short *)(*(char **)game_settings + 4)) * 108) / 128), 3);
L436F6:;
    if (((int)(unsigned short)(*(short *)(*(char **)game_settings) & -256)) == 0) return;
    func_00144D00(91, 80, (int)(short)(((((int)(unsigned short)*(short *)(*(char **)game_settings)) >> 8) * 108) / 128), 3);
}

void options_save_game(void)
{
    options_close();
    saveload_menu(1);
L43769:;
    if (*(signed char *)mouse_buttons == 0) return;
    func_0012B136();
    goto L43769;
}

void options_load_game(void)
{
    options_close();
    saveload_menu(0);
L4379D:;
    if (*(signed char *)mouse_buttons == 0) return;
    func_0012B136();
    goto L4379D;
}

void options_exit_game(void)
{
    int l_18;

    msgbox_yes_no_rsc(1069);
    if (((int)(unsigned char)*(signed char *)D_00196271) != 1) return;
    options_close();
    game_exit(0);
}

void options_sound_slider(void)
{
    *(short *)(*(char **)game_settings + 2) = options_slider_value();
}

void options_music_slider(void)
{
    *(short *)(*(char **)game_settings + 4) = options_slider_value();
    sound_set_volume((int)(short)*(short *)(*(char **)game_settings + 4));
}

void options_detail_slider(void)
{
    *(short *)(*(char **)game_settings) = (options_slider_value() << 8) | (*(short *)(*(char **)game_settings) & 255);
}

int options_slider_value(void)
{
    int l_1C;

    l_1C = ((((int)(short)*(short *)mouse_x) << 7) - 11648) / 108;
    if (l_1C >= 0) goto L438BF;
    l_1C = 0;
    goto L438CC;
L438BF:;
    if (l_1C <= 127) goto L438CC;
    l_1C = 127;
L438CC:;
    return l_1C;
}

void options_toggle_full_screen(void)
{
    *(signed char *)(*(char **)game_settings) ^= 1;
    if (((int)(unsigned short)(*(short *)(*(char **)game_settings) & 1)) == 0) goto L43926;
    func_0012A2D0(160, 100, 160, 100);
    goto L4393F;
L43926:;
    func_0012A2D0(160, 77, 160, 77);
L4393F:;
    func_0012A274(200, 180);
}

void options_toggle_head_bobbing(void)
{
    *(signed char *)(*(char **)game_settings) ^= 2;
}

int options_controls_rebind(int a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = -1;
    mc_memset((int)key_down, 0, 128, (int)D_00170EE8, 276, 128);
L439CA:;
    if (*(signed char *)mouse_buttons == 0) goto L439DA;
    func_0012B136();
    goto L439CA;
L439DA:;
    if (l_1C != (-1)) goto L43B36;
    l_18 = func_000CE87B((int)key_down, 128);
    if (l_18 == 0) goto L43A33;
    l_1C = l_18 - ((int)key_down);
    if (l_1C != 1) goto L43A1B;
    return 0;
L43A1B:;
    if (l_1C < 2) goto L43A27;
    if (l_1C != 68) goto L43A2E;
L43A27:;
    l_1C = -1;
L43A2E:;
    goto L43ACB;
L43A33:;
    if (((int)(unsigned char)*(signed char *)D_00152A02) != 1) goto L43ACB;
    func_00152D00();
    if (*(signed char *)D_00152A30 == 0) goto L43A5D;
    l_1C = 200;
    goto L43ACB;
L43A5D:;
    if (*(signed char *)D_00152A31 == 0) goto L43A6F;
    l_1C = 201;
    goto L43ACB;
L43A6F:;
    if (a1 >= 6) goto L43ACB;
    if (func_0009DEAC(*(int *)D_00152A20) <= 2048) goto L43AA1;
    if (*(int *)D_00152A20 >= 0) goto L43A98;
    l_1C = 204;
    goto L43A9F;
L43A98:;
    l_1C = 205;
L43A9F:;
    goto L43ACB;
L43AA1:;
    if (func_0009DEAC(*(int *)D_00152A24) <= 2048) goto L43ACB;
    if (*(int *)D_00152A24 >= 0) goto L43AC4;
    l_1C = 206;
    goto L43ACB;
L43AC4:;
    l_1C = 207;
L43ACB:;
    if (l_1C != (-1)) goto L43B1F;
    func_0012B136();
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L43AEF;
    l_1C = 202;
    goto L43B1F;
L43AEF:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L43B08;
    l_1C = 203;
    goto L43B1F;
L43B08:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 4)) == 0) goto L43B1F;
    l_1C = 212;
L43B1F:;
    options_controls_draw(a1, a2);
    func_000CDD81(0);
    goto L439DA;
L43B36:;
    *(signed char *)(key_map + a1) = *(signed char *)&l_1C;
    return 0;
}

void options_controls_draw(int a1, int a2)
{
    int l_18;
    int l_14;

    mc_memcpy(*(int *)screen_buffer, a2, 64000, (int)D_00170EE8, 355, 4);
    if (((int)(unsigned char)*(signed char *)mouse_control_mode) != 1) goto L43BD3;
    func_00144F68((int)(unsigned short)*(short *)(*(char **)D_00199708), (int)(unsigned short)*(short *)(*(char **)D_00199708 + 2), (int)(unsigned short)*(short *)(*(char **)D_00199708 + 4), (int)(unsigned short)*(short *)(*(char **)D_00199708 + 6), (int)(*(char **)D_00199708 + 12));
L43BD3:;
    l_18 = 0;
L43BDA:;
    if (l_18 < 38) goto L43BED;
    return;
L43BE5:;
    l_18++;
    goto L43BDA;
L43BED:;
    if (a1 == l_18) goto L43BE5;
    l_14 = 146;
    if (options_controls_is_duplicate(l_18) == 0) goto L43C0F;
    l_14 = 244;
L43C0F:;
    *(signed char *)D_0012B508 = *(signed char *)&l_14;
    if (((int)(unsigned char)*(signed char *)(key_map + l_18)) >= 200) goto L43C7B;
    text_draw_centred(*(int *)(key_names + (((int)(unsigned char)*(signed char *)(key_map + l_18)) << 2)), (((int)(short)*(short *)(D_0017B80C + (l_18 * 12))) + ((int)(short)*(short *)(controls_buttons + (l_18 * 12)))) / 2, (int)&*(signed char *)((char *)((int)(short)*(short *)(D_0017B80A + (l_18 * 12))) + 2));
    goto L43CC8;
L43C7B:;
    text_draw_centred(*(int *)(D_0017B41C + (((int)(unsigned char)*(signed char *)(key_map + l_18)) << 2)), (((int)(short)*(short *)(D_0017B80C + (l_18 * 12))) + ((int)(short)*(short *)(controls_buttons + (l_18 * 12)))) / 2, (int)&*(signed char *)((char *)((int)(short)*(short *)(D_0017B80A + (l_18 * 12))) + 2));
L43CC8:;
    goto L43BE5;
}

void options_controls_screen(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 0;
    l_18 = disk_read_file((int)D_00170EF2, 0);
    *(int *)D_00199708 = disk_read_file((int)D_00170EFF, 0);
L43D0B:;
    if (l_20 != 0) goto L43E0F;
    if (*(signed char *)key_down_esc == 0) goto L43D27;
    if (options_controls_check() != 0) goto L43D29;
L43D27:;
    goto L43D2E;
L43D29:;
    goto L43E0F;
L43D2E:;
    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    func_0012B136();
    options_controls_draw(-1, l_18);
    cursor_draw_arrow();
    if (*(signed char *)mouse_buttons == 0) goto L43D61;
    if (*(signed char *)mouse_buttons_prev == 0) goto L43D66;
L43D61:;
    goto L43E03;
L43D66:;
    l_24 = 0;
L43D6D:;
    if (l_24 < 42) goto L43D80;
    goto L43E03;
L43D78:;
    l_24++;
    goto L43D6D;
L43D80:;
    if (*(short *)mouse_x <= *(short *)(controls_buttons + (l_24 * 12))) goto L43DA8;
    if (*(short *)mouse_x < *(short *)(D_0017B80C + (l_24 * 12))) goto L43DAA;
L43DA8:;
    goto L43DBE;
L43DAA:;
    if (*(short *)mouse_y > *(short *)(D_0017B80A + (l_24 * 12))) goto L43DC0;
L43DBE:;
    goto L43DD4;
L43DC0:;
    if (*(short *)mouse_y < *(short *)(D_0017B80E + (l_24 * 12))) goto L43DD6;
L43DD4:;
    goto L43DFE;
L43DD6:;
    sound_play(203, *(int *)player_object, 100);
    l_20 = ((int (*)())(*(int *)(D_0017B810 + (l_24 * 12))))(l_24, l_18);
L43DFE:;
    goto L43D78;
L43E03:;
    func_000CDD81(0);
    goto L43D0B;
L43E0F:;
    if (*(signed char *)key_down_esc != 0) goto L43E0F;
    if (l_18 == 0) goto L43E27;
    if (l_18 != (-1751672937)) goto L43E29;
L43E27:;
    goto L43E42;
L43E29:;
    mc_free(l_18, (int)D_00170EE8, 401);
    l_18 = -1751672937;
L43E42:;
    if (*(int *)D_00199708 == 0) goto L43E57;
    if (*(int *)D_00199708 != (-1751672937)) goto L43E59;
L43E57:;
    goto L43E77;
L43E59:;
    mc_free(*(int *)D_00199708, (int)D_00170EE8, 402);
    *(int *)D_00199708 = -1751672937;
L43E77:;
    l_1C = disk_create(*(int *)D_001788E4);
    mc_memcpy((int)D_00195E82, (int)D_00152A04, 46, (int)D_00170EE8, 405, 4);
    write(l_1C, (int)mouse_control_mode, 54);
    write(l_1C, (int)key_map, 38);
    func_0009DEA7(l_1C);
}

int options_controls_check(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
L43EEF:;
    if (l_20 < 38) goto L43EFF;
    goto L43F50;
L43EF7:;
    l_20++;
    goto L43EEF;
L43EFF:;
    l_1C = 0;
L43F06:;
    if (l_1C < 38) goto L43F16;
    goto L43F4E;
L43F0E:;
    l_1C++;
    goto L43F06;
L43F16:;
    if (l_20 == l_1C) goto L43F32;
    if (*(signed char *)(key_map + l_20) == *(signed char *)(key_map + l_1C)) goto L43F34;
L43F32:;
    goto L43F4C;
L43F34:;
    msgbox_show_string((int)D_00170F0C, 1);
    return 0;
L43F4C:;
    goto L43F0E;
L43F4E:;
    goto L43EF7;
L43F50:;
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
    mc_memcpy(*(int *)screen_buffer, *(int *)options_saved_screen, 64000, (int)D_00170EE8, 480, 4);
    func_00144F68((int)(unsigned short)*(short *)((char *)l_18), (int)(unsigned short)*(short *)((char *)l_18 + 2), (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
    *(signed char *)D_0012B508 = 246;
    func_00144D00((int)(short)((*(signed char *)mouse_control_mode == 0) ? 134 : 220), 47, 5, 5);
    l_1C = 0;
L4426E:;
    if (((int)(unsigned char)*(signed char *)mouse_sensitivity_x) > l_1C) goto L44284;
    goto L442BD;
L4427C:;
    l_1C++;
    goto L4426E;
L44284:;
    func_00144F68((l_1C * 7) + 139, 108, (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 4), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 6), (int)(*(char **)D_00195B5C + 12));
    goto L4427C;
L442BD:;
    l_1C = 0;
L442C4:;
    if (((int)(unsigned char)*(signed char *)mouse_sensitivity_y) > l_1C) goto L442DA;
    goto L44313;
L442D2:;
    l_1C++;
    goto L442C4;
L442DA:;
    func_00144F68((l_1C * 7) + 139, 121, (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 4), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 6), (int)(*(char **)D_00195B5C + 12));
    goto L442D2;
L44313:;
    l_1C = 0;
L4431A:;
    if (((int)(unsigned char)(*(signed char *)mouse_turn_rate & 127)) > l_1C) goto L44335;
    goto L4436E;
L4432D:;
    l_1C++;
    goto L4431A;
L44335:;
    func_00144F68((l_1C * 7) + 139, 134, (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 4), (int)(unsigned short)*(short *)(*(char **)D_00195B5C + 6), (int)(*(char **)D_00195B5C + 12));
    goto L4432D;
L4436E:;
    if (((int)(unsigned char)(*(signed char *)mouse_turn_rate & 128)) == 0) return;
    func_00144F68((int)(unsigned short)*(short *)(*(char **)D_00195B60), (int)(unsigned short)*(short *)(*(char **)D_00195B60 + 2), (int)(unsigned short)*(short *)(*(char **)D_00195B60 + 4), (int)(unsigned short)*(short *)(*(char **)D_00195B60 + 6), (int)(*(char **)D_00195B60 + 12));
}

int options_mouse_cursor_mode(void)
{
    *(signed char *)mouse_control_mode = 0;
    return 0;
}

int options_mouse_horizontal(void)
{
    *(signed char *)mouse_sensitivity_x = ((((int)(short)*(short *)mouse_x) - 139) / 6) + 1;
    if (((int)(unsigned char)*(signed char *)mouse_sensitivity_x) <= 15) goto L4445B;
    *(signed char *)mouse_sensitivity_x = 15;
L4445B:;
    if (((int)(unsigned char)*(signed char *)mouse_sensitivity_x) >= 1) goto L4446E;
    *(signed char *)mouse_sensitivity_x = 1;
L4446E:;
    func_000CE88D(((int)(unsigned char)*(signed char *)mouse_sensitivity_x) * 6, ((int)(unsigned char)*(signed char *)mouse_sensitivity_y) * 6);
    return 0;
}

int options_mouse_vertical(void)
{
    *(signed char *)mouse_sensitivity_y = ((((int)(short)*(short *)mouse_x) - 139) / 6) + 1;
    if (((int)(unsigned char)*(signed char *)mouse_sensitivity_y) <= 15) goto L444DC;
    *(signed char *)mouse_sensitivity_y = 15;
L444DC:;
    if (((int)(unsigned char)*(signed char *)mouse_sensitivity_y) >= 1) goto L444EF;
    *(signed char *)mouse_sensitivity_y = 1;
L444EF:;
    func_000CE88D(((int)(unsigned char)*(signed char *)mouse_sensitivity_x) * 6, ((int)(unsigned char)*(signed char *)mouse_sensitivity_y) * 6);
    return 0;
}

int options_mouse_turn_rate(void)
{
    *(signed char *)mouse_turn_rate = ((int)(unsigned char)(*(signed char *)mouse_turn_rate & 128 & 255)) | ((((int)(short)*(short *)mouse_x) - 139) / 7);
    return 0;
}

int options_mouse_reverse_vertical(void)
{
    *(signed char *)mouse_turn_rate ^= 128;
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
    *(int *)D_00195B5C = disk_read_file((int)D_00170F61, 0);
    *(int *)D_00195B60 = disk_read_file((int)D_00170F6E, 0);
L445FB:;
    if (l_20 != 0) goto L446E3;
    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    func_0012B136();
    options_joystick_draw(0, l_1C);
    cursor_draw_arrow();
    if (*(signed char *)mouse_buttons == 0) goto L44635;
    if (*(signed char *)mouse_buttons_prev == 0) goto L4463A;
L44635:;
    goto L446D7;
L4463A:;
    l_24 = 0;
L44641:;
    if (l_24 < 6) goto L44654;
    goto L446D7;
L4464C:;
    l_24++;
    goto L44641;
L44654:;
    if (*(short *)mouse_x <= *(short *)(options_joystick_buttons + (l_24 * 12))) goto L4467C;
    if (*(short *)mouse_x < *(short *)(D_0017BA64 + (l_24 * 12))) goto L4467E;
L4467C:;
    goto L44692;
L4467E:;
    if (*(short *)mouse_y > *(short *)(D_0017BA62 + (l_24 * 12))) goto L44694;
L44692:;
    goto L446A8;
L44694:;
    if (*(short *)mouse_y < *(short *)(D_0017BA66 + (l_24 * 12))) goto L446AA;
L446A8:;
    goto L446D2;
L446AA:;
    sound_play(203, *(int *)player_object, 100);
    l_20 = ((int (*)())(*(int *)(D_0017BA68 + (l_24 * 12))))(l_24, l_1C);
L446D2:;
    goto L4464C;
L446D7:;
    func_000CDD81(0);
    goto L445FB;
L446E3:;
    if (*(int *)D_00195B60 == 0) goto L446F8;
    if (*(int *)D_00195B60 != (-1751672937)) goto L446FA;
L446F8:;
    goto L44718;
L446FA:;
    mc_free(*(int *)D_00195B60, (int)D_00170EE8, 580);
    *(int *)D_00195B60 = -1751672937;
L44718:;
    if (*(int *)D_00195B5C == 0) goto L4472D;
    if (*(int *)D_00195B5C != (-1751672937)) goto L4472F;
L4472D:;
    goto L4474D;
L4472F:;
    mc_free(*(int *)D_00195B5C, (int)D_00170EE8, 581);
    *(int *)D_00195B5C = -1751672937;
L4474D:;
    if (l_1C == 0) goto L4475C;
    if (l_1C != (-1751672937)) goto L4475E;
L4475C:;
    goto L44777;
L4475E:;
    mc_free(l_1C, (int)D_00170EE8, 582);
    l_1C = -1751672937;
L44777:;
    return 0;
}

void options_joystick_draw(int a1, int a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = a2;
    mc_memcpy(*(int *)screen_buffer, *(int *)options_saved_screen, 64000, (int)D_00170EE8, 593, 4);
    func_00144F68((int)(unsigned short)*(short *)((char *)l_1C), (int)(unsigned short)*(short *)((char *)l_1C + 2), (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), l_1C + 12);
    if (((int)(unsigned char)*(signed char *)joystick_setting) != 2) goto L44823;
    *(signed char *)D_0012B508 = 246;
    func_00144D00(138, 42, 5, 5);
L44823:;
    l_18 = (int)(short)*(short *)joystick_threshold;
    if (l_18 != 10) goto L4483C;
    l_18 = 0;
    goto L44852;
L4483C:;
    if (l_18 != 20) goto L4484B;
    l_18 = 1;
    goto L44852;
L4484B:;
    l_18 = 2;
L44852:;
    l_14 = (int)(*(char **)D_00195B60 + 12);
    func_000CB34E((((int)(short)*(short *)(options_joystick_buttons + ((l_18 + 2) * 12))) + l_14) - 113, (int)(*(char **)screen_buffer + ((((int)(short)*(short *)(D_0017BA62 + ((l_18 + 2) * 12))) * 320) + ((int)(short)*(short *)(options_joystick_buttons + ((l_18 + 2) * 12))))), (int)&*(signed char *)((char *)(((int)(short)*(short *)(D_0017BA64 + ((l_18 + 2) * 12))) - ((int)(short)*(short *)(options_joystick_buttons + ((l_18 + 2) * 12)))) + 1), (int)&*(signed char *)((char *)(((int)(short)*(short *)(D_0017BA66 + ((l_18 + 2) * 12))) - ((int)(short)*(short *)(D_0017BA62 + ((l_18 + 2) * 12)))) + 1), (int)(unsigned short)*(short *)(*(char **)D_00195B60 + 4));
    if (a1 == 0) return;
    if (*(int *)D_00152A20 != 0) goto L4491C;
    if (*(int *)D_00152A24 == 0) return;
L4491C:;
    l_1C = *(int *)D_00195B5C;
    l_18 = func_00044B8A();
    if (l_18 == (-1)) return;
L44932:;
    if (l_18 == 0) goto L44955;
    l_1C = (((int)(unsigned short)*(short *)((char *)l_1C + 10)) + l_1C) + 12;
    l_18--;
    goto L44932;
L44955:;
    func_00144FB4((int)(unsigned short)*(short *)((char *)l_1C), (int)(unsigned short)*(short *)((char *)l_1C + 2), (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), l_1C + 12);
}

int options_joystick_disable(void)
{
    if (((int)(unsigned char)*(signed char *)joystick_setting) != 1) goto L449B3;
    *(signed char *)joystick_setting = 2;
    goto L449BA;
L449B3:;
    *(signed char *)joystick_setting = 1;
L449BA:;
    *(signed char *)D_00152A02 = *(signed char *)joystick_setting;
    return 0;
}

int options_joystick_calibrate_button(int a1, int a2)
{
    int l_18;

    l_18 = 0;
    if (((int)(unsigned char)*(signed char *)D_00152A02) != 2) goto L44A0A;
    return 0;
L44A0A:;
    *(int *)D_00152A10 = 32000;
    *(int *)D_00152A18 = 32000;
    *(int *)D_00152A14 = 0;
    *(int *)D_00152A1C = 0;
    func_00152C40();
L44A37:;
    if (l_18 != 0) goto L44AD2;
    func_00152D00();
    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    func_0012B136();
    options_joystick_draw(1, a2);
    cursor_draw_arrow();
    if (*(signed char *)mouse_buttons == 0) goto L44A79;
    if (*(signed char *)mouse_buttons_prev == 0) goto L44A7B;
L44A79:;
    goto L44A8A;
L44A7B:;
    if (*(short *)mouse_x > *(short *)D_0017BA6C) goto L44A8C;
L44A8A:;
    goto L44A9B;
L44A8C:;
    if (*(short *)mouse_x < *(short *)D_0017BA70) goto L44A9D;
L44A9B:;
    goto L44AAC;
L44A9D:;
    if (*(short *)mouse_y > *(short *)D_0017BA6E) goto L44AAE;
L44AAC:;
    goto L44ABD;
L44AAE:;
    if (*(short *)mouse_y < *(short *)D_0017BA72) goto L44ABF;
L44ABD:;
    goto L44AC6;
L44ABF:;
    l_18 = 1;
L44AC6:;
    func_000CDD81(0);
    goto L44A37;
L44AD2:;
    return 0;
}

int options_joystick_threshold_low(void)
{
    *(int *)D_00152A0C = (int)(short)(*(short *)joystick_threshold = 10);
    return 0;
}

int options_joystick_threshold_med(void)
{
    *(int *)D_00152A0C = (int)(short)(*(short *)joystick_threshold = 20);
    return 0;
}

int options_joystick_threshold_high(void)
{
    *(int *)D_00152A0C = (int)(short)(*(short *)joystick_threshold = 40);
    return 0;
}

int func_00044B8A(void)
{
    if (*(int *)D_00152A20 >= 0) goto L44C26;
    if (*(int *)D_00152A24 >= 0) goto L44BEA;
    if (*(int *)D_00152A20 <= (-2048)) goto L44BC6;
    return 0;
L44BC6:;
    if (*(int *)D_00152A24 <= (-2048)) goto L44BDE;
    return 6;
L44BDE:;
    return 7;
L44BEA:;
    if (*(int *)D_00152A20 <= (-2048)) goto L44C02;
    return 4;
L44C02:;
    if (*(int *)D_00152A24 >= 2048) goto L44C1A;
    return 6;
L44C1A:;
    return 5;
L44C26:;
    if (*(int *)D_00152A24 >= 0) goto L44C62;
    if (*(int *)D_00152A20 >= 2048) goto L44C44;
    return 0;
L44C44:;
    if (*(int *)D_00152A24 <= (-2048)) goto L44C59;
    return 2;
L44C59:;
    return 1;
L44C62:;
    if (*(int *)D_00152A20 >= 2048) goto L44C77;
    return 4;
L44C77:;
    if (*(int *)D_00152A24 >= 2048) goto L44C8C;
    return 2;
L44C8C:;
    return 3;
}

int options_controls_is_duplicate(int a1)
{
    int l_1C;

    l_1C = 0;
L44CB8:;
    if (l_1C < 38) goto L44CC8;
    goto L44CEF;
L44CC0:;
    l_1C++;
    goto L44CB8;
L44CC8:;
    if (l_1C == a1) goto L44CC0;
    if (*(signed char *)(key_map + a1) != *(signed char *)(key_map + l_1C)) goto L44CED;
    return 1;
L44CED:;
    goto L44CC0;
L44CEF:;
    return 0;
}
