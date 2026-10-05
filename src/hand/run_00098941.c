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
extern signed char D_00190D16;
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
extern int func_000A0024(char *, char *, int);
extern int func_000CDD81();
extern int func_0012B136();
extern int func_00144F68();

void func_00098538(void)
{
    unsigned char *l_18;

    l_18 = *(unsigned char **)(inv_right_container + 63);
    while (l_18 != 0) {
        l_18[113] |= 32;
        l_18 = *(unsigned char **)(l_18 + 55);
    }
}

int func_00098573(void)
{
    unsigned char *l_24;
    int l_20;
    int l_1C;

    l_24 = inv_selected_item + 71;
    if (*(short *)(l_24 + 67) != -1) {
        if (*(unsigned short *)(l_24 + 42) & 32) {
            msgbox_show_string(D_001832BC, 1);
            return 0;
        }
        l_20 = holiday_today(game_minutes, current_region);
        l_1C = (unsigned)(*(int *)(l_24 + 36) * 25) >> 8;
        if (l_20 != 43 && gold_can_afford(l_1C) == 0 && ((struct bits8 *)&player_motion_flags)->b7 == 0) {
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
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 0;
    l_18 = 17;
    if (player_environment != 1) {
        hud_message_add(D_001772BC);
        return;
    }
    D_00195B5C = disk_read_file(D_001772E6, 0);
    D_00195B60 = disk_read_file(D_001772F3, 0);
    D_00190D16 = 0;
    travel_find_transport();
    if (func_00098B20() != 0) {
        l_18 = 24;
    } else {
        if ((D_00190D16 & 2) && player_environment == 1)
            l_18 |= 2;
        if ((D_00190D16 & 1) && player_environment == 1)
            l_18 |= 4;
        if (*(int *)(player_character + 120) != 0 && player_environment != 3)
            l_18 |= 8;
    }
    while (l_20 == 0) {
        mouse_buttons_prev = mouse_buttons;
        func_0012B136();
        func_00144F68(*(unsigned short *)D_00195B5C, *(unsigned short *)(D_00195B5C + 2), *(unsigned short *)(D_00195B5C + 4), *(unsigned short *)(D_00195B5C + 6), D_00195B5C + 12);
        for (l_1C = 0; l_1C < 4; l_1C++) {
            if (((1 << l_1C) & l_18) == 0)
                func_00144F68(transport_buttons[l_1C].x0, transport_buttons[l_1C].y0, *(unsigned short *)(D_00195B60 + 4), 9, D_00195B60 + 12 + l_1C * (*(unsigned short *)(D_00195B60 + 4) * 9));
        }
        cursor_draw_arrow();
        if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
            for (l_1C = 0; l_1C < 5; l_1C++) {
                if ((1 << l_1C) & l_18) {
                    if (mouse_x > transport_buttons[l_1C].x0 && mouse_x < transport_buttons[l_1C].x1 && mouse_y > transport_buttons[l_1C].y0 && mouse_y < transport_buttons[l_1C].y1) {
                        if (transport_buttons[l_1C].fn != 0)
                            transport_buttons[l_1C].fn(l_1C);
                        l_20 = 1;
                    }
                }
            }
        }
        func_000CDD81(0);
    }
    if (D_00195B5C != 0 && D_00195B5C != FREED) {
        func_000A0024(D_00195B5C, D_0017704C, 2870);
        D_00195B5C = FREED;
    }
    if (D_00195B60 != 0 && D_00195B60 != FREED) {
        func_000A0024(D_00195B60, D_0017704C, 2871);
        D_00195B60 = FREED;
    }
}

void transport_choose(int a1)
{
    switch (a1) {
    case 0:
        player_character[65] &= 249;
        player_horse_sounds_stop();
        break;
    case 1:
        D_00190D16 = 0;
        travel_find_transport();
        if (D_00190D16 & 2) {
            player_character[65] |= 2;
            player_character[65] &= 251;
            player_motion_flags &= 251;
        }
        break;
    case 2:
        D_00190D16 = 0;
        travel_find_transport();
        if (D_00190D16 & 1) {
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
