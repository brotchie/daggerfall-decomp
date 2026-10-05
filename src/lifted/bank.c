/* bank.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int D_000C23B8;
extern int D_000C23BC;
extern int D_000C23C0;
extern int D_000C23C4;
extern int D_000C23C8;
extern int D_000C23CC;
extern int D_000CEA24;
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char D_00132F58;
extern int D_00136911;
extern char D_00136E00[];
extern char D_00136E24[];
extern signed char key_down_esc;
extern signed char D_00142314;
extern signed char D_00142315;
extern int screen_buffer;
extern char D_00175C90[];
extern char D_00175C9D[];
extern char D_00175CAA[];
extern char D_00175CB7[];
extern char D_00175CC4[];
extern char D_00175CD0[];
extern char D_00175CEF[];
extern char region_names[];
extern char bank_buttons[];
extern char D_00186E26[];
extern char D_00186E28[];
extern char D_00186E2A[];
extern char D_00186E2C[];
extern signed char D_00187CA8;
extern char saved_location_name[];
extern char saved_region_name[];
extern signed char text_buffer[];
extern signed char text_rsc_buffer[];
extern char D_00191020[];
extern int bank_ship_price;
extern int bank_house_price;
extern struct record *D_001959E0;
extern struct record *bank_accounts;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern int D_00195B5C;
extern struct location *current_location;
extern struct character *player_character;
extern int window_image;
extern int game_minutes;
extern struct settings *game_settings;
extern int D_00195C44;
extern int D_00195D30;
extern signed char climate_weathers[];
extern short D_00195F36;
extern unsigned short D_00195F38;
extern signed char current_region;
extern unsigned char D_0019626F;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char current_climate;
extern signed char D_001968BB;
extern char bank_houses_for_sale[];
extern char D_001A3FB0[];
extern char D_001A3FB4[];
extern char D_001A3FB8[];
extern char D_001A3FBC[];
extern char bank_list_top[];
extern int D_001A4140;
extern int bank_saved_screen;
extern short bank_ships_for_sale;
extern signed char D_001A414A;
extern int D_001A414C;
extern char D_001A418A[];
extern char D_001A418E[];
extern short D_001A4192;
extern signed char D_001A4194;
extern int D_001A4196;
extern int D_001A41D4;
extern int D_001A41D8;
extern int D_001A41DC;
extern int D_001A41E0;
extern int D_001A41E4;
extern int D_001A41E8;
extern struct bank_account *bank_account;
extern signed char bank_ship_count;
extern unsigned char bank_screen;
extern signed char bank_selected;
extern signed char bank_house_count;

extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int gold_can_carry(int);
extern int model_get(unsigned short, int, int);
extern int inpstr_update(void);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int mc_free();
extern int mc_memset();
extern int mc_malloc();
extern int mc_strncpy();
extern int atoi();
extern int func_000A0DD9();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000CDD81();
extern int func_0012A2D0();
extern int func_0012A4F0();
extern int func_0012A870();
extern int func_0012B136();
extern int func_00135E39();
extern int func_00135E90();
extern int func_00136AB4();
extern int func_00137000();
extern int func_00137725();
extern int func_001401D4();
extern int func_00142790();
extern int func_00144FB4();
extern int func_0014D23C();
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void msgbox_update(void);
extern void item_make(int, int, struct item *);
extern void bank_draw(void);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void msgbox_yes_no_rsc(int);
extern void cursor_draw_arrow(void);
extern void inpstr_begin_text(int, short);
extern void object_foreach(struct record *, int);
int bank_open(int);
int bank_confirm(int);
int bank_input_amount(void);
void bank_close(void);
void bank_add_house_for_sale(struct record *);
void bank_init_ships(void);
void bank_deposit_letter(struct record *);
void func_0006CB02(void);
#pragma aux func_000A0ED9 parm routine [];

int bank_open(int a1)
{
    if (((int)(unsigned char)game_mode) == 8 && ((int)D_0019626F) == 15) {
        return 1;
    }
    if (a1 != 0) {
        while (mouse_buttons != 0) func_0012B136();
        D_001A41E4 = 0;
        window_image = disk_read_file((int)D_00175C90, 0);
        D_00195B5C = disk_read_file((int)D_00175C9D, 0);
        D_001A41E8 = disk_read_file((int)D_00175CAA, 0);
        D_001A4140 = disk_read_file((int)D_00175CB7, 0);
        func_0012A2D0(51, 45, 216, 72);
        D_00196272 = 1;
        game_mode = 15;
        D_00187CA8 = 0;
        bank_house_count = (bank_ship_count = (bank_screen = (bank_selected = 0)));
        *(int *)bank_list_top = 0;
        D_001A41E0 = 1500;
        bank_saved_screen = mc_malloc(64000, (int)D_00175CC4, 89);
        mc_memcpy(bank_saved_screen, screen_buffer, 64000, (int)D_00175CC4, 90, 4);
        bank_account = &bank_accounts->data.bank_accounts[(int)(unsigned char)current_region];
        object_foreach(D_00195AC4, (int)bank_add_house_for_sale);
        bank_init_ships();
    }
    return ((((int)(unsigned char)game_mode) == 15) ? 1 : 0);
}

void bank_close(void)
{
    do {
    } while (key_down_esc != 0);
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        func_0012A2D0(160, 100, 160, 100);
    } else {
        func_0012A2D0(160, 77, 160, 77);
    }
    if (bank_saved_screen != 0 && bank_saved_screen != (-1751672937)) {
        mc_free(bank_saved_screen, (int)D_00175CC4, 110);
        bank_saved_screen = -1751672937;
    }
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00175CC4, 111);
        window_image = -1751672937;
    }
    if (D_00195B5C != 0 && D_00195B5C != (-1751672937)) {
        mc_free(D_00195B5C, (int)D_00175CC4, 112);
        D_00195B5C = -1751672937;
    }
    if (D_001A41E8 != 0 && D_001A41E8 != (-1751672937)) {
        mc_free(D_001A41E8, (int)D_00175CC4, 113);
        D_001A41E8 = -1751672937;
    }
    if (D_001A4140 != 0 && D_001A4140 != (-1751672937)) {
        mc_free(D_001A4140, (int)D_00175CC4, 114);
        D_001A4140 = -1751672937;
    }
    mc_memset((int)text_rsc_buffer, 0, 2048, (int)D_00175CC4, 116, 2048);
    func_0006CB02();
    game_mode = 0;
    D_00196272 = 0;
    D_00187CA8 = 1;
}

void bank_frame(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (bank_open(0) == 0) return;
    bank_draw();
    if (key_down_esc != 0) bank_close();
    if (mouse_buttons == 0 || mouse_buttons_prev != 0) return;
    switch (bank_screen) {
    case 0:
        l_1C = 0;
        l_18 = 11;
        break;
    case 1:
        l_1C = 11;
        l_18 = 16;
        break;
    case 2:
        l_1C = 16;
        l_18 = 19;
    }
    for (l_20 = l_1C; l_20 < l_18; l_20++) {
        if (mouse_x > *(short *)(bank_buttons + (l_20 * 12)) && mouse_x < *(short *)(D_00186E28 + (l_20 * 12)) && mouse_y > *(short *)(D_00186E26 + (l_20 * 12)) && mouse_y < *(short *)(D_00186E2A + (l_20 * 12))) {
            sound_play(203, (int)player_object, 100);
            ((int (*)())(*(int *)(D_00186E2C + (l_20 * 12))))();
        }
    }
}

void bank_add_house_for_sale(struct record *a1)
{
    int l_28;
    int l_24;
    struct building *l_20;
    struct block *l_1C;
    struct block_model *l_18;

    if (a1->type != 43) return;
    if (a1->children == 0) return;
    if (((int)(unsigned char)bank_house_count) == 20) return;
    l_20 = &current_location->buildings[a1->image];
    if (a1->id != l_20->id || l_20->type != 1) return;
    l_1C = (struct block *)(*(int *)(bank_houses_for_sale + (((int)(unsigned char)bank_house_count) * 20)) = (int)RECORD_DATA(a1));
    l_18 = l_1C->models;
    *(int *)(D_001A3FB0 + (((int)(unsigned char)bank_house_count) * 20)) = (int)l_20;
    *(int *)(D_001A3FB8 + (((int)(unsigned char)bank_house_count) * 20)) = a1->id;
    *(int *)(D_001A3FBC + (((int)(unsigned char)bank_house_count) * 20)) = l_18->yaw;
    l_24 = 0;
    for (l_28 = l_24; l_1C->model_count > l_28; l_28++, l_18++) {
        l_18->model = (char *)model_get(l_18->id, l_18->variant, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)climate_weathers[(int)(unsigned char)current_region]));
        if (*(int *)(l_18->model + 12) > l_24) l_24 = *(int *)(l_18->model + 12);
    }
    *(int *)(D_001A3FB4 + (((int)(unsigned char)bank_house_count) * 20)) = l_24 * 5;
    (bank_house_count)++;
}

void bank_init_ships(void)
{
    bank_ships_for_sale = 415;
    D_001A414A = 6;
    D_001A414C = model_get(415, 6, ((int)(unsigned char)climate_weathers[(int)(unsigned char)current_region]) + (((int)(unsigned char)current_climate) << 2));
    *(int *)D_001A418A = 65011713;
    *(int *)D_001A418E = 100000;
    D_001A4192 = 415;
    D_001A4194 = 11;
    D_001A4196 = model_get(415, 8, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)climate_weathers[(int)(unsigned char)current_region]));
    D_001A41D4 = 65077249;
    D_001A41D8 = 200000;
    bank_ship_count = 2;
}

void bank_deposit_gold(void)
{
    int l_18;

    l_18 = bank_input_amount();
    if (l_18 < 1) return;
    if (((unsigned)player_character->gold) < l_18) {
        msgbox_show_rsc(454, 1);
        return;
    }
    bank_account->balance += l_18;
    player_character->gold -= l_18;
}

void bank_withdraw_gold(void)
{
    int l_18;

    if (bank_account->balance <= 0) return;
    l_18 = bank_input_amount();
    if (l_18 < 1) return;
    if (bank_account->balance < l_18) {
        if (bank_confirm(290) == 0) return;
        l_18 = bank_account->balance;
    }
    if (gold_can_carry(l_18) != 0) {
        bank_account->balance -= l_18;
        player_character->gold += l_18;
        return;
    }
    msgbox_show_string((int)D_00175CD0, 1);
}

void bank_deposit_letter(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (l_18->group != 27 || l_18->index != 2) return;
    bank_account->balance += l_18->value;
    object_delete(a1);
}

void bank_deposit_letters_of_credit(void)
{
    if (bank_confirm(291) == 0) return;
    object_foreach(player_entity->children, (int)bank_deposit_letter);
}

void bank_withdraw_letter_of_credit(void)
{
    int l_20;
    int l_1C;
    struct record *l_18;

    if (bank_account->balance <= 0) return;
    l_1C = bank_input_amount();
    if (l_1C < 1) return;
    if (l_1C < 100) {
        msgbox_show_rsc(293, 1);
        return;
    }
    l_20 = (l_1C / 100) + 1;
    if ((l_20 + l_1C) > bank_account->balance) {
        msgbox_show_rsc(292, 1);
        return;
    }
    bank_account->balance -= l_20 + l_1C;
    l_18 = object_create_child(D_001959E0, 0, 107);
    l_18->type = 2;
    l_18->flags = 1;
    item_make(27, 2, &l_18->data.item);
    l_18->data.item.value = l_1C;
}

void bank_borrow(void)
{
    int l_18;

    if (((int)(unsigned char)(bank_account->flags & 1)) != 0) {
        msgbox_show_rsc(288, 1);
        return;
    }
    if (bank_account->loan_due != 0) {
        msgbox_show_rsc(289, 1);
        return;
    }
    l_18 = bank_input_amount();
    if (l_18 < 1) return;
    if (l_18 < 100) {
        msgbox_show_rsc(296, 1);
        return;
    }
    if ((player_character->level * 50000) < l_18) {
        msgbox_show_rsc(295, 1);
        return;
    }
    bank_account->loan_due = game_minutes + 518400;
    bank_account->loan_owed = l_18 + ((l_18 * 10) / 100);
    bank_account->balance += l_18;
}

void bank_buy_house(void)
{
    if (bank_house_count == 0) {
        msgbox_show_rsc(287, 1);
        return;
    }
    if (player_character->house != 0) {
        msgbox_show_rsc(286, 1);
        return;
    }
    bank_screen = 1;
    bank_selected = 0;
}

void bank_sell_house(void)
{
    if (player_character->house == 0) return;
    D_00195D30 = bank_house_price - ((bank_house_price * 15) / 100);
    if (bank_confirm(298) == 0) return;
    bank_account->balance += D_00195D30;
    bank_house_price = 0;
    player_character->house = 0;
}

void bank_buy_ship(void)
{
    if (player_character->ship_owned != 0) {
        msgbox_show_rsc(284, 1);
        return;
    }
    if (D_001968BB == 0) {
        msgbox_show_rsc(285, 1);
        return;
    }
    bank_screen = 2;
    bank_selected = 0;
}

void bank_sell_ship(void)
{
    if (player_character->ship_owned == 0) return;
    D_00195D30 = bank_ship_price - ((bank_ship_price * 15) / 100);
    if (bank_confirm(299) == 0) return;
    bank_account->balance += D_00195D30;
    bank_ship_price = 0;
    player_character->ship_owned = 0;
}

void bank_draw_preview(int a1, int a2)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct block_model *l_18;
    int l_14;

    l_18 = (struct block_model *)a2;
    D_000CEA24 = 1048576;
    func_0014D23C(-1);
    D_00136911 = 16128;
    D_000C23C4 = 0;
    D_000C23C8 = -100;
    if (D_00142314 != 0) {
        D_001A41E0 -= 8;
    } else if (D_00142315 != 0) {
        D_001A41E0 += 8;
    }
    if (D_001A41E0 < 100) {
        D_001A41E0 = 100;
    } else if (D_001A41E0 > 2000) {
        D_001A41E0 = 2000;
    }
    D_000C23CC = -(D_001A41E0);
    D_000C23B8 = 0;
    D_000C23BC = 0;
    D_000C23C0 = 0;
    func_00135E90();
    func_0012A4F0();
    func_00136AB4();
    func_00137000(D_000C23B8, D_000C23BC, D_000C23C0, (int)D_00136E00);
    func_00137725((int)D_00136E00, (int)D_00136E24);
    for (l_2C = 0; l_2C < a1; l_2C++, l_18++) {
        if (l_18->id > 10 && l_18->id != 415) continue;
        l_28 = l_18->x;
        l_24 = l_18->y;
        l_20 = l_18->z;
    }
    l_18 = (struct block_model *)a2;
    for (l_2C = 0; l_2C < a1; l_2C++, l_18++) {
        l_18->x -= l_28;
        l_18->y -= l_24;
        l_18->z -= l_20;
    }
    l_18 = (struct block_model *)a2;
    for (l_2C = 0; l_2C < a1; l_2C++, l_18++) {
        l_18->model = (char *)model_get(l_18->id, l_18->variant, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)climate_weathers[(int)(unsigned char)current_region]));
        if (l_18->model != 0) {
            l_14 = 1132;
            l_18->yaw = ((*(int *)((char *)l_14) & 2047) << 4) & 2047;
            func_001401D4((int)&l_18->model, 0);
            break;
        }
    }
    l_1C = func_0012A870(2);
    if (l_1C != 0 || D_00132F58 != 0) {
        D_00132F58 = 0;
        func_00135E39();
    }
    l_18 = (struct block_model *)a2;
    for (l_2C = 0; l_2C < a1; l_2C++, l_18++) {
        l_18->x += l_28;
        l_18->y += l_24;
        l_18->z += l_20;
    }
}

void bank_draw_house_list(void)
{
    int l_20;
    int l_1C;
    int l_18;

    for (l_20 = *(int *)bank_list_top; (*(int *)bank_list_top + 11) > l_20; l_20++) {
        if (((int)(unsigned char)bank_selected) == l_20) {
            l_1C = 246;
        } else {
            l_1C = 146;
        }
        func_000A0ED9(573, (int)D_00175CC4);
        mc_sprintf((int)text_buffer, (int)D_00175CEF, *(int *)(D_001A3FB4 + (l_20 * 20)));
        text_draw_colored((int)text_buffer, 52, (int)(short)(((l_20 - *(short *)bank_list_top) * 7) + 38), (int)(short)*(short *)&l_1C, 156);
    }
    if (*(int *)bank_list_top != 0) {
        l_18 = D_001A41E8;
    } else {
        l_18 = D_001A4140;
    }
    func_00144FB4(153, 38, 9, 20, l_18);
    if ((*(int *)bank_list_top + 11) < ((int)(unsigned char)bank_house_count)) {
        l_18 = D_001A41E8;
    } else {
        l_18 = D_001A4140;
    }
    func_00144FB4(153, 102, 9, 16, l_18 + 576);
}

void bank_draw_ship_list(void)
{
    int l_1C;
    short l_18;

    for (l_1C = 0; ((int)(unsigned char)bank_ship_count) > l_1C; l_1C++) {
        if (((int)(unsigned char)bank_selected) == l_1C) {
            *(int *)&l_18 = 246;
        } else {
            *(int *)&l_18 = 146;
        }
        text_draw_colored(func_000A0DD9(*(int *)(D_001A418E + (l_1C * 74)), (int)text_rsc_buffer, 10), 52, (int)(short)((l_1C * 7) + 38), (int)(short)l_18, 156);
    }
}

void bank_house_list_click(void)
{
    int l_18;

    l_18 = (int)(*(char **)bank_list_top + ((((int)(short)mouse_y) - 38) / 7));
    if (((int)(unsigned char)bank_house_count) <= l_18) return;
    bank_selected = *(signed char *)&l_18;
}

void bank_house_list_up(void)
{
    if (*(int *)bank_list_top == 0) return;
    (*(int *)bank_list_top)--;
}

void bank_house_list_down(void)
{
    if (((int)(unsigned char)bank_house_count) <= 11) return;
    if ((((int)(unsigned char)bank_house_count) - 11) <= *(int *)bank_list_top) return;
    (*(int *)bank_list_top)++;
}

void bank_ship_list_click(void)
{
    int l_18;

    l_18 = (((int)(short)mouse_y) - 38) / 7;
    if (((int)(unsigned char)bank_ship_count) <= l_18) return;
    bank_selected = *(signed char *)&l_18;
}

void bank_house_bought(void)
{
    if (bank_account->balance < *(int *)(D_001A3FB4 + (((int)(unsigned char)bank_selected) * 20))) {
        msgbox_show_rsc(454, 1);
        return;
    }
    bank_house_price = *(int *)(D_001A3FB4 + (((int)(unsigned char)bank_selected) * 20));
    bank_account->balance -= bank_house_price;
    player_character->house = *(int *)(D_001A3FB8 + (((int)(unsigned char)bank_selected) * 20));
    D_001A41E4 = (int)(*(char **)(bank_houses_for_sale + (((int)(unsigned char)bank_selected) * 20)) - 71);
    D_001A41DC = *(int *)(D_001A3FB0 + (((int)(unsigned char)bank_selected) * 20));
    msgbox_show_rsc(282, 1);
    mc_strncpy((int)saved_region_name, *(int *)(region_names + (((int)(unsigned char)current_region) << 2)), 32, (int)D_00175CC4, 647);
    mc_strncpy((int)saved_location_name, (int)current_location, 32, (int)D_00175CC4, 648);
    bank_screen = 0;
}

void bank_ship_bought(void)
{
    if (bank_account->balance < *(int *)(D_001A418E + (((int)(unsigned char)bank_selected) * 74))) {
        msgbox_show_rsc(454, 1);
        return;
    }
    bank_account->balance -= *(int *)(D_001A418E + (((int)(unsigned char)bank_selected) * 74));
    bank_ship_price = *(int *)(D_001A418E + (((int)(unsigned char)bank_selected) * 74));
    player_character->ship_owned = *(int *)(D_001A418A + (((int)(unsigned char)bank_selected) * 74));
    msgbox_show_rsc(283, 1);
    bank_screen = 0;
}

void bank_list_exit(void)
{
    bank_screen = 0;
}

int bank_confirm(int a1)
{
    msgbox_yes_no_rsc(a1);
    while (1) {
        mouse_buttons_prev = mouse_buttons;
        func_0012B136();
        bank_draw();
        msgbox_update();
        cursor_draw_arrow();
        func_000CDD81(1);
        if (((int)D_00196271) == 1) return 1;
        if (((int)D_00196271) == 2) return 0;
    }
}

int bank_input_amount(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    D_0012B508 = 146;
    l_1C = D_00195C44 + 55000;
    *(signed char *)((char *)l_1C) = 0;
    *(signed char *)D_00191020 = 0;
    D_00195F36 = 157;
    D_00195F38 = 152;
    inpstr_begin_text((int)D_00191020, 10);
    func_00142790();
    mouse_buttons = (mouse_buttons_prev = 0);
    while (l_20 == 0) {
        bank_draw();
        l_20 = inpstr_update();
        func_000CDD81(1);
    }
    return atoi((int)D_00191020);
}

void func_0006CB02(void)
{
    int l_18;

    for (l_18 = 0; ((int)(unsigned char)bank_house_count) > l_18; l_18++) {
        *(int *)(*(char **)(*(char **)(bank_houses_for_sale + (l_18 * 20)) + 5) + 52) = *(int *)(D_001A3FBC + (l_18 * 20));
    }
}
