/* icons.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char key_down_esc[];
extern char screen_buffer[];
extern char D_00175898[];
extern char D_001758A0[];
extern char D_001758A4[];
extern char D_001758A8[];
extern char hud_buttons[];
extern char D_00185C5A[];
extern char D_00185C5C[];
extern char D_00185C5E[];
extern char D_00185C60[];
extern char D_00187CA8[];
extern char D_00188208[];
extern char D_00190B44[];
extern char D_00190D64[];
extern char text_macro_fpc[];
extern char D_001940D4[];
extern char D_001940D6[];
extern char D_001940D8[];
extern char wagon_container[];
extern char D_001959EC[];
extern char D_00195A80[];
extern char player_entity[];
extern char player_object[];
extern char D_00195ACC[];
extern char player_character[];
extern char game_settings[];
extern char magic_items_image[];
extern char mouse_control_mode[];
extern char view_cursor_active[];
extern char D_00196272[];
extern char mouse_buttons_prev[];
extern char player_ailment_flags[];
extern char magic_items_saved_screen[];
extern char hud_pressed_button[];
extern char D_00199D71[];
extern char steer_key_region[];
extern char D_001A9AB8[];
extern char inv_selected_item[];

extern int sheet_open(int);
extern int spellbook_open(int);
extern int options_open(int);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int picklist_poll(int);
extern int inventory_open(int, int, int);
extern int travel_map_open(int);
extern int mc_free();
extern int mc_malloc();
extern int mc_memcpy();
extern int func_000CE44C();
extern int func_0012B136();
extern int func_00144F68();
extern void automap_open(void);
extern void status_show(int);
extern void interaction_mode_cycle(int);
extern void parse_expand(int, int);
extern void rest_open(void);
extern void cursor_draw_arrow(void);
extern void picklist_init(int, short, short, int, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void picklist_add(int, int, int);
extern void picklist_free(int);
extern void picklist_draw(int, int);
extern void object_foreach(int, int);
extern void inv_use_item(void);
extern void transport_menu(void);
void hud_buttons_click(int);
void magic_items_add_cb(int);
void magic_items_close(void);

void hud_buttons_click(int a1)
{
    int l_18;

    if (((int)(unsigned char)*(signed char *)mouse_control_mode) != 1) goto L5D304;
    if (*(signed char *)view_cursor_active == 0) goto L5D306;
L5D304:;
    goto L5D30B;
L5D306:;
    return;
L5D30B:;
    l_18 = 0;
L5D312:;
    if (l_18 < 11) goto L5D325;
    return;
L5D31D:;
    l_18++;
    goto L5D312;
L5D325:;
    if (*(short *)mouse_x <= *(short *)(hud_buttons + (l_18 * 12))) goto L5D34D;
    if (*(short *)mouse_x < *(short *)(D_00185C5C + (l_18 * 12))) goto L5D34F;
L5D34D:;
    goto L5D363;
L5D34F:;
    if (*(short *)mouse_y > *(short *)(D_00185C5A + (l_18 * 12))) goto L5D365;
L5D363:;
    goto L5D379;
L5D365:;
    if (*(short *)mouse_y < *(short *)(D_00185C5E + (l_18 * 12))) goto L5D37B;
L5D379:;
    goto L5D3CC;
L5D37B:;
    if (a1 == 0) goto L5D38D;
    if (((int)(unsigned char)*(signed char *)hud_pressed_button) == l_18) goto L5D38F;
L5D38D:;
    goto L5D3BE;
L5D38F:;
    sound_play(203, *(int *)player_object, 100);
    ((int (*)())(*(int *)(D_00185C60 + (l_18 * 12))))((int)(unsigned char)*(signed char *)mouse_buttons_prev);
    *(signed char *)hud_pressed_button = 255;
    return;
L5D3BE:;
    if (a1 != 0) goto L5D3CC;
    *(signed char *)hud_pressed_button = *(signed char *)&l_18;
L5D3CC:;
    goto L5D31D;
}

void hud_button_sheet(void)
{
    sheet_open(1);
}

void hud_button_spellbook(void)
{
    spellbook_open(1);
}

void hud_button_interaction_mode(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 1)) == 0) goto L5D449;
    interaction_mode_cycle(1);
    return;
L5D449:;
    interaction_mode_cycle(-1);
}

void hud_button_inventory(void)
{
    inventory_open(1, 0, 2);
}

void hud_toggle_weapon(void)
{
    short l_18;

    *(signed char *)D_001940D6 ^= 64;
    if (((struct bf8_6_1 *)&D_001940D6)->f == 0) return;
    if (*(int *)(*(char **)player_character + 443) == 0) return;
    *(int *)&l_18 = *(int *)(*(char **)player_character + 443) + 71;
    sound_play((int)(short)*(short *)(D_00188208 + (((int)(unsigned short)*(short *)(*(char **)&l_18 + 34)) * 2)), *(int *)player_object, 100);
}

void hud_button_status(void)
{
    status_show(1);
}

void hud_button_transport(void)
{
    transport_menu();
}

void hud_button_map(int a1)
{
    if ((a1 & 2) == 0) goto L5D557;
    travel_map_open(1);
    return;
L5D557:;
    automap_open();
}

void hud_button_rest(void)
{
    rest_open();
}

void hud_button_options(void)
{
    options_open(1);
}

void magic_items_add_cb(int a1)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) return;
    l_18 = *(int *)((char *)a1 + 67);
L5D5D2:;
    if (l_18 == 0) goto L5D5EE;
    if (func_000CE44C((int)D_001959EC, l_18, 4) == 0) goto L5D5F0;
L5D5EE:;
    goto L5D5FB;
L5D5F0:;
    l_18 = *(int *)((char *)l_18 + 67);
    goto L5D5D2;
L5D5FB:;
    if (l_18 != 0) return;
    if (*(int *)((char *)a1 + 67) == *(int *)wagon_container) return;
    l_24 = a1 + 71;
    l_1C = 0;
    if (((int)(unsigned short)*(short *)((char *)l_24 + 32)) != 1) goto L5D649;
    if (((int)(unsigned short)*(short *)((char *)l_24 + 34)) == 1) goto L5D64B;
L5D649:;
    goto L5D654;
L5D64B:;
    if (*(int *)((char *)a1 + 63) != 0) goto L5D656;
L5D654:;
    goto L5D668;
L5D656:;
    if (((int)(unsigned char)*(signed char *)(*(char **)((char *)a1 + 63))) == 31) goto L5D66A;
L5D668:;
    goto L5D673;
L5D66A:;
    l_1C = 2;
    goto L5D683;
L5D673:;
    if (((int)(short)*(short *)((char *)l_24 + 67)) == (-1)) return;
L5D683:;
    if (l_1C != 0) goto L5D6D1;
    l_20 = 0;
    l_1C = l_20;
L5D696:;
    if (l_20 < 10) goto L5D6A6;
    goto L5D6D1;
L5D69E:;
    l_20++;
    goto L5D696;
L5D6A6:;
    if (((int)(short)*(short *)((char *)((l_20 << 2) + l_24) + 67)) == (-1)) goto L5D6D1;
    if (*(short *)((char *)((l_20 << 2) + l_24) + 67) != 0) goto L5D6CF;
    l_1C = 1;
L5D6CF:;
    goto L5D69E;
L5D6D1:;
    if (l_1C == 0) return;
    *(int *)D_00195A80 = l_24;
    if (l_1C != 2) goto L5D704;
    *(int *)D_00195ACC = *(int *)((char *)a1 + 63) + 71;
    parse_expand((int)D_001758A0, (int)D_00190B44);
    goto L5D713;
L5D704:;
    parse_expand((int)D_001758A4, (int)D_00190B44);
L5D713:;
    picklist_add((int)D_001A9AB8, (int)D_00190B44, 0);
    *(int *)(text_macro_fpc + (((int)(short)(*(short *)D_00190D64)++) << 2)) = a1;
}

void magic_items_open(void)
{
    int l_18;

    *(short *)D_00190D64 = 0;
    picklist_init((int)D_001A9AB8, 100, 159, 166, 34, 88, 159, 8, 15, 88, 179, 8, 59, 0, 0, 1, 1, 146, 146, 244, 114, 0);
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)magic_items_add_cb);
    if (*(short *)D_00190D64 != 0) goto L5D805;
    picklist_free((int)D_001A9AB8);
    return;
L5D805:;
    *(signed char *)D_00187CA8 = 0;
    *(signed char *)D_001940D8 &= 254;
    *(signed char *)D_001940D4 |= 32;
    *(int *)magic_items_image = disk_read_file((int)D_001758A8, 0);
    *(signed char *)D_00196272 = 1;
    *(int *)magic_items_saved_screen = mc_malloc(64000, (int)D_00175898, 358);
    mc_memcpy(*(int *)magic_items_saved_screen, *(int *)screen_buffer, 64000, (int)D_00175898, 359, 4);
}

void magic_items_frame(void)
{
    short l_18;

    if (((struct bf8_5_1 *)&D_001940D4)->f == 0) return;
    mc_memcpy(*(int *)screen_buffer, *(int *)magic_items_saved_screen, 64000, (int)D_00175898, 368, 4);
    func_00144F68((int)(unsigned short)*(short *)(*(char **)magic_items_image), (int)(unsigned short)*(short *)(*(char **)magic_items_image + 2), (int)(unsigned short)*(short *)(*(char **)magic_items_image + 4), (int)(unsigned short)*(short *)(*(char **)magic_items_image + 6), (int)(*(char **)magic_items_image + 12));
    if (*(signed char *)key_down_esc != 0) goto L5D911;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L5D931;
L5D911:;
    if (*(signed char *)key_down_esc != 0) goto L5D911;
L5D91A:;
    if (*(signed char *)mouse_buttons == 0) goto L5D92A;
    func_0012B136();
    goto L5D91A;
L5D92A:;
    magic_items_close();
    return;
L5D931:;
    *(int *)&l_18 = picklist_poll((int)D_001A9AB8) - 1;
    if (((int)(short)l_18) <= (-1)) goto L5D966;
    magic_items_close();
    *(int *)inv_selected_item = *(int *)(text_macro_fpc + (((int)(short)l_18) << 2));
    inv_use_item();
    return;
L5D966:;
    picklist_draw((int)D_001A9AB8, 0);
}

void magic_items_close(void)
{
    *(signed char *)D_001940D4 &= 223;
    if (*(int *)magic_items_image == 0) goto L5D9A6;
    if (*(int *)magic_items_image != (-1751672937)) goto L5D9A8;
L5D9A6:;
    goto L5D9C6;
L5D9A8:;
    mc_free(*(int *)magic_items_image, (int)D_00175898, 396);
    *(int *)magic_items_image = -1751672937;
L5D9C6:;
    picklist_free((int)D_001A9AB8);
    *(signed char *)D_00196272 = 0;
    *(signed char *)D_00187CA8 = 1;
    if (*(int *)magic_items_saved_screen == 0) goto L5D9F3;
    if (*(int *)magic_items_saved_screen != (-1751672937)) goto L5D9F5;
L5D9F3:;
    return;
L5D9F5:;
    mc_free(*(int *)magic_items_saved_screen, (int)D_00175898, 400);
    *(int *)magic_items_saved_screen = -1751672937;
}

int hud_update(void)
{
    if (((int)(short)*(short *)mouse_y) < 154) goto L5DA42;
    if (*(signed char *)D_00196272 == 0) goto L5DA44;
L5DA42:;
    goto L5DA5A;
L5DA44:;
    if (((int)(unsigned short)(*(short *)(*(char **)game_settings) & 1)) == 0) goto L5DA5F;
L5DA5A:;
    goto L5DAEE;
L5DA5F:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 3)) == 0) goto L5DA7F;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 3)) == 0) goto L5DA81;
L5DA7F:;
    goto L5DA88;
L5DA81:;
    hud_buttons_click(0);
L5DA88:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 3)) != 0) goto L5DAA8;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons_prev & 3)) != 0) goto L5DAAA;
L5DAA8:;
    goto L5DAB4;
L5DAAA:;
    hud_buttons_click(1);
L5DAB4:;
    if (*(signed char *)mouse_control_mode == 0) goto L5DAD4;
    if (((int)(unsigned char)*(signed char *)mouse_control_mode) != 1) goto L5DAD2;
    if (*(signed char *)view_cursor_active != 0) goto L5DAD4;
L5DAD2:;
    goto L5DAEE;
L5DAD4:;
    cursor_draw_arrow();
    if (((int)(short)*(short *)steer_key_region) != (-1)) goto L5DAEE;
    return 1;
L5DAEE:;
    return 0;
}

int hud_portrait_overlay_index(void)
{
    if (((int)(short)*(short *)(*(char **)player_character + 124)) >= (((int)(short)*(short *)(*(char **)player_character + 126)) / 10)) goto L5DB3D;
    return 2;
L5DB3D:;
    if (*(signed char *)D_00199D71 == 0) goto L5DB4F;
    return 0;
L5DB4F:;
    if (((int)(unsigned char)(*(signed char *)player_ailment_flags & 1)) == 0) goto L5DB68;
    return 3;
L5DB68:;
    if (((int)(unsigned char)(*(signed char *)player_ailment_flags & 2)) == 0) goto L5DB81;
    return 1;
L5DB81:;
    return -1;
}
