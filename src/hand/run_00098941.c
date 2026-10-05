/* matched by the real Watcom C32 10.0a (-d2): a run of inven from 0x98538 to 0x98941, kept together for its switch table's alignment */
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct button { short x0, y0, x1, y1; void (*fn)(int); };
#define FREED ((char *)0x97979797)
extern char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern char D_0017704C[];
extern char D_0017729E[];
extern char D_001772BC[];
extern char D_001772E6[];
extern char D_001772F3[];
extern unsigned char player_environment;
extern char *D_001832B4;
extern char *D_001832BC;
extern struct button transport_buttons[];
extern signed char scratch_190d16;
extern unsigned char player_motion_flags;
extern unsigned char *inv_right_container;
extern char *D_00195B5C;
extern char *D_00195B60;
extern char *player_character;
extern int game_minutes;
extern unsigned char current_region;
extern char mouse_buttons_prev;
extern unsigned char *inv_selected_item;
extern void msgbox_show_string(char *, int);
extern int holiday_today(int, int);
extern char *disk_read_file(char *, int);
extern int hud_message_add(char *);
extern int gold_can_afford(int);
extern void cursor_draw_arrow(void);
extern void player_horse_sounds_stop(void);
extern int func_00098B20(void);
extern void func_00098A15(void);
extern void travel_find_transport(void);
extern int mc_free(char *, char *, int);
extern int xn_gfx_present_inclusive();
extern int xn_mouse_poll_clamped();
extern int xn_draw_image();

void trade_mark_identified(void)
{
    unsigned char *object;

    object = *(unsigned char **)(inv_right_container + 63);
    while (object != 0) {
        object[113] |= 32;
        object = *(unsigned char **)(object + 55);
    }
}

int trade_can_identify_selected(void)
{
    unsigned char *item;
    int holiday;
    int price;

    item = inv_selected_item + 71;
    if (*(short *)(item + 67) != -1) {
        if (*(unsigned short *)(item + 42) & 32) {
            msgbox_show_string(D_001832BC, 1);
            return 0;
        }
        holiday = holiday_today(game_minutes, current_region);
        price = (unsigned)(*(int *)(item + 36) * 25) >> 8;
        if (holiday != 43 && gold_can_afford(price) == 0 && ((struct bits8 *)&player_motion_flags)->b7 == 0) {
            msgbox_show_string(D_0017729E, 1);
            return 0;
        }
        return 1;
    }
    msgbox_show_string(D_001832B4, 1);
    return 0;
}

void transport_menu(void)
{
    int done;
    int button;
    int enabled;

    done = 0;
    enabled = 17;
    if (player_environment != 1) {
        hud_message_add(D_001772BC);
        return;
    }
    D_00195B5C = disk_read_file(D_001772E6, 0);
    D_00195B60 = disk_read_file(D_001772F3, 0);
    scratch_190d16 = 0;
    travel_find_transport();
    if (func_00098B20() != 0) {
        enabled = 24;
    } else {
        if ((scratch_190d16 & 2) && player_environment == 1)
            enabled |= 2;
        if ((scratch_190d16 & 1) && player_environment == 1)
            enabled |= 4;
        if (*(int *)(player_character + 120) != 0 && player_environment != 3)
            enabled |= 8;
    }
    while (done == 0) {
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        xn_draw_image(*(unsigned short *)D_00195B5C, *(unsigned short *)(D_00195B5C + 2), *(unsigned short *)(D_00195B5C + 4), *(unsigned short *)(D_00195B5C + 6), D_00195B5C + 12);
        for (button = 0; button < 4; button++) {
            if (((1 << button) & enabled) == 0)
                xn_draw_image(transport_buttons[button].x0, transport_buttons[button].y0, *(unsigned short *)(D_00195B60 + 4), 9, D_00195B60 + 12 + button * (*(unsigned short *)(D_00195B60 + 4) * 9));
        }
        cursor_draw_arrow();
        if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
            for (button = 0; button < 5; button++) {
                if ((1 << button) & enabled) {
                    if (mouse_x > transport_buttons[button].x0 && mouse_x < transport_buttons[button].x1 && mouse_y > transport_buttons[button].y0 && mouse_y < transport_buttons[button].y1) {
                        if (transport_buttons[button].fn != 0)
                            transport_buttons[button].fn(button);
                        done = 1;
                    }
                }
            }
        }
        xn_gfx_present_inclusive(0);
    }
    if (D_00195B5C != 0 && D_00195B5C != FREED) {
        mc_free(D_00195B5C, D_0017704C, 2870);
        D_00195B5C = FREED;
    }
    if (D_00195B60 != 0 && D_00195B60 != FREED) {
        mc_free(D_00195B60, D_0017704C, 2871);
        D_00195B60 = FREED;
    }
}

void transport_choose(int button)
{
    switch (button) {
    case 0:
        player_character[65] &= 249;
        player_horse_sounds_stop();
        break;
    case 1:
        scratch_190d16 = 0;
        travel_find_transport();
        if (scratch_190d16 & 2) {
            player_character[65] |= 2;
            player_character[65] &= 251;
            player_motion_flags &= 251;
        }
        break;
    case 2:
        scratch_190d16 = 0;
        travel_find_transport();
        if (scratch_190d16 & 1) {
            player_character[65] |= 4;
            player_character[65] &= 253;
            player_motion_flags &= 251;
        }
        break;
    case 3:
        player_character[65] &= 249;
        func_00098A15();
        player_horse_sounds_stop();
        break;
    }
}
