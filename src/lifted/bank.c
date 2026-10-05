/* bank.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int xn_cam_pitch;
extern int xn_cam_yaw;
extern int xn_cam_roll;
extern int xn_cam_x;
extern int xn_cam_y;
extern int xn_cam_z;
extern int xn_cam_far_z;
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char xn_tex_cache_full;
extern int xn_light_ambient;
extern char xn_cam_rotation[];
extern char xn_cam_view_matrix[];
extern signed char key_down_esc;
extern signed char key_down_minus;
extern signed char key_down_equals;
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
extern struct record *location_object;
extern char D_00195B5C[];
extern struct location *current_location;
extern struct character *player_character;
extern int window_image;
extern int game_minutes;
extern struct settings *game_settings;
extern char scratch_buffer[];
extern int trade_price;
extern signed char climate_weathers[];
extern short text_cursor_x;
extern unsigned short text_cursor_y;
extern signed char current_region;
extern unsigned char D_0019626F;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char current_climate;
extern signed char location_is_port;
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
extern int itoa();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int xn_gfx_present_inclusive();
extern int xn_cam_set_view_window();
extern int xn_render_begin_frame();
extern int xn_render_frame();
extern int xn_mouse_poll_clamped();
extern int xn_tex_cache_flush();
extern int xn_tex_cache_begin_frame();
extern int xn_light_reset();
extern int xn_mat_from_angles();
extern int xn_cam_scale_matrix();
extern int xn_model_submit();
extern int xn_kbd_flush();
extern int xn_draw_image_transparent();
extern int xn_shade_set_fog();
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void msgbox_update(void);
extern void item_make(int, int, struct item *);
extern void bank_draw(void);
extern void text_draw_coloured(int, int, int, int, unsigned char);
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
#pragma aux mc_set_location parm routine [];

int bank_open(int opening)
{
    if (((int)(unsigned char)game_mode) == 8 && ((int)D_0019626F) == 15) {
        return 1;
    }
    if (opening != 0) {
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        D_001A41E4 = 0;
        window_image = disk_read_file((int)D_00175C90, 0);
        *(int *)D_00195B5C = disk_read_file((int)D_00175C9D, 0);
        D_001A41E8 = disk_read_file((int)D_00175CAA, 0);
        D_001A4140 = disk_read_file((int)D_00175CB7, 0);
        xn_cam_set_view_window(51, 45, 216, 72);
        D_00196272 = 1;
        game_mode = 15;
        D_00187CA8 = 0;
        bank_house_count = (bank_ship_count = (bank_screen = (bank_selected = 0)));
        *(int *)bank_list_top = 0;
        D_001A41E0 = 1500;
        bank_saved_screen = mc_malloc(64000, (int)D_00175CC4, 89);
        mc_memcpy(bank_saved_screen, screen_buffer, 64000, (int)D_00175CC4, 90, 4);
        bank_account = &bank_accounts->data.bank_accounts[(int)(unsigned char)current_region];
        object_foreach(location_object, (int)bank_add_house_for_sale);
        bank_init_ships();
    }
    return ((((int)(unsigned char)game_mode) == 15) ? 1 : 0);
}

void bank_close(void)
{
    while (key_down_esc != 0);
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        xn_cam_set_view_window(160, 100, 160, 100);
    } else {
        xn_cam_set_view_window(160, 77, 160, 77);
    }
    if (bank_saved_screen != 0 && bank_saved_screen != (-1751672937)) {
        mc_free(bank_saved_screen, (int)D_00175CC4, 110);
        bank_saved_screen = -1751672937;
    }
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00175CC4, 111);
        window_image = -1751672937;
    }
    if (*(int *)D_00195B5C != 0 && *(int *)D_00195B5C != (-1751672937)) {
        mc_free(*(int *)D_00195B5C, (int)D_00175CC4, 112);
        *(int *)D_00195B5C = -1751672937;
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
    int i;
    int first;
    int last;

    if (bank_open(0) == 0) return;
    bank_draw();
    if (key_down_esc != 0) bank_close();
    if (mouse_buttons == 0 || mouse_buttons_prev != 0) return;
    switch (bank_screen) {
    case 0:
        first = 0;
        last = 11;
        break;
    case 1:
        first = 11;
        last = 16;
        break;
    case 2:
        first = 16;
        last = 19;
    }
    for (i = first; i < last; i++) {
        if (mouse_x > *(short *)(bank_buttons + (i * 12)) && mouse_x < *(short *)(D_00186E28 + (i * 12)) && mouse_y > *(short *)(D_00186E26 + (i * 12)) && mouse_y < *(short *)(D_00186E2A + (i * 12))) {
            sound_play(203, (int)player_object, 100);
            ((int (*)())(*(int *)(D_00186E2C + (i * 12))))();
        }
    }
}

void bank_add_house_for_sale(struct record *object)
{
    int i;
    int max_radius;
    struct building *building;
    struct block *block;
    struct block_model *model;

    if (object->type != 43) return;
    if (object->children == 0) return;
    if (((int)(unsigned char)bank_house_count) == 20) return;
    building = &current_location->buildings[object->image];
    if (object->id != building->id || building->type != 1) return;
    block = (struct block *)(*(int *)(bank_houses_for_sale + (((int)(unsigned char)bank_house_count) * 20)) = (int)RECORD_DATA(object));
    model = block->models;
    *(int *)(D_001A3FB0 + (((int)(unsigned char)bank_house_count) * 20)) = (int)building;
    *(int *)(D_001A3FB8 + (((int)(unsigned char)bank_house_count) * 20)) = object->id;
    *(int *)(D_001A3FBC + (((int)(unsigned char)bank_house_count) * 20)) = model->yaw;
    max_radius = 0;
    for (i = max_radius; block->model_count > i; i++, model++) {
        model->model = (char *)model_get(model->id, model->variant, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)climate_weathers[(int)(unsigned char)current_region]));
        if (*(int *)(model->model + 12) > max_radius) max_radius = *(int *)(model->model + 12);
    }
    *(int *)(D_001A3FB4 + (((int)(unsigned char)bank_house_count) * 20)) = max_radius * 5;
    bank_house_count++;
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
    int amount;

    amount = bank_input_amount();
    if (amount < 1) return;
    if (((unsigned)player_character->gold) < amount) {
        msgbox_show_rsc(454, 1);
        return;
    }
    bank_account->balance += amount;
    player_character->gold -= amount;
}

void bank_withdraw_gold(void)
{
    int amount;

    if (bank_account->balance <= 0) return;
    amount = bank_input_amount();
    if (amount < 1) return;
    if (bank_account->balance < amount) {
        if (bank_confirm(290) == 0) return;
        amount = bank_account->balance;
    }
    if (gold_can_carry(amount) != 0) {
        bank_account->balance -= amount;
        player_character->gold += amount;
        return;
    }
    msgbox_show_string((int)D_00175CD0, 1);
}

void bank_deposit_letter(struct record *object)
{
    struct item *item;

    if (object->type != 2) return;
    item = &object->data.item;
    if (item->group != 27 || item->index != 2) return;
    bank_account->balance += item->value;
    object_delete(object);
}

void bank_deposit_letters_of_credit(void)
{
    if (bank_confirm(291) == 0) return;
    object_foreach(player_entity->children, (int)bank_deposit_letter);
}

void bank_withdraw_letter_of_credit(void)
{
    int fee;
    int amount;
    struct record *letter;

    if (bank_account->balance <= 0) return;
    amount = bank_input_amount();
    if (amount < 1) return;
    if (amount < 100) {
        msgbox_show_rsc(293, 1);
        return;
    }
    fee = (amount / 100) + 1;
    if ((fee + amount) > bank_account->balance) {
        msgbox_show_rsc(292, 1);
        return;
    }
    bank_account->balance -= fee + amount;
    letter = object_create_child(D_001959E0, 0, 107);
    letter->type = 2;
    letter->flags = 1;
    item_make(27, 2, &letter->data.item);
    letter->data.item.value = amount;
}

void bank_borrow(void)
{
    int amount;

    if (((int)(unsigned char)(bank_account->flags & 1)) != 0) {
        msgbox_show_rsc(288, 1);
        return;
    }
    if (bank_account->loan_due != 0) {
        msgbox_show_rsc(289, 1);
        return;
    }
    amount = bank_input_amount();
    if (amount < 1) return;
    if (amount < 100) {
        msgbox_show_rsc(296, 1);
        return;
    }
    if ((player_character->level * 50000) < amount) {
        msgbox_show_rsc(295, 1);
        return;
    }
    bank_account->loan_due = game_minutes + 518400;
    bank_account->loan_owed = amount + ((amount * 10) / 100);
    bank_account->balance += amount;
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
    trade_price = bank_house_price - ((bank_house_price * 15) / 100);
    if (bank_confirm(298) == 0) return;
    bank_account->balance += trade_price;
    bank_house_price = 0;
    player_character->house = 0;
}

void bank_buy_ship(void)
{
    if (player_character->ship_owned != 0) {
        msgbox_show_rsc(284, 1);
        return;
    }
    if (location_is_port == 0) {
        msgbox_show_rsc(285, 1);
        return;
    }
    bank_screen = 2;
    bank_selected = 0;
}

void bank_sell_ship(void)
{
    if (player_character->ship_owned == 0) return;
    trade_price = bank_ship_price - ((bank_ship_price * 15) / 100);
    if (bank_confirm(299) == 0) return;
    bank_account->balance += trade_price;
    bank_ship_price = 0;
    player_character->ship_owned = 0;
}

void bank_draw_preview(int model_count, struct block_model *models)
{
    int i;
    int origin_x;
    int origin_y;
    int origin_z;
    int render_result;
    struct block_model *model;
    int *bios_ticks;

    model = models;
    xn_cam_far_z = 1048576;
    xn_shade_set_fog(-1);
    xn_light_ambient = 16128;
    xn_cam_x = 0;
    xn_cam_y = -100;
    if (key_down_minus != 0) {
        D_001A41E0 -= 8;
    } else if (key_down_equals != 0) {
        D_001A41E0 += 8;
    }
    if (D_001A41E0 < 100) {
        D_001A41E0 = 100;
    } else if (D_001A41E0 > 2000) {
        D_001A41E0 = 2000;
    }
    xn_cam_z = -D_001A41E0;
    xn_cam_pitch = 0;
    xn_cam_yaw = 0;
    xn_cam_roll = 0;
    xn_tex_cache_begin_frame();
    xn_render_begin_frame();
    xn_light_reset();
    xn_mat_from_angles(xn_cam_pitch, xn_cam_yaw, xn_cam_roll, (int)xn_cam_rotation);
    xn_cam_scale_matrix((int)xn_cam_rotation, (int)xn_cam_view_matrix);
    for (i = 0; i < model_count; i++, model++) {
        if (model->id > 10 && model->id != 415) continue;
        origin_x = model->x;
        origin_y = model->y;
        origin_z = model->z;
    }
    model = models;
    for (i = 0; i < model_count; i++, model++) {
        model->x -= origin_x;
        model->y -= origin_y;
        model->z -= origin_z;
    }
    model = models;
    for (i = 0; i < model_count; i++, model++) {
        model->model = (char *)model_get(model->id, model->variant, (((int)(unsigned char)current_climate) << 2) + ((int)(unsigned char)climate_weathers[(int)(unsigned char)current_region]));
        if (model->model != 0) {
            bios_ticks = (int *)1132;
            model->yaw = ((*bios_ticks & 2047) << 4) & 2047;
            xn_model_submit((int)&model->model, 0);
            break;
        }
    }
    render_result = xn_render_frame(2);
    if (render_result != 0 || xn_tex_cache_full != 0) {
        xn_tex_cache_full = 0;
        xn_tex_cache_flush();
    }
    model = models;
    for (i = 0; i < model_count; i++, model++) {
        model->x += origin_x;
        model->y += origin_y;
        model->z += origin_z;
    }
}

void bank_draw_house_list(void)
{
    int row;
    int colour;
    int arrow_image;

    for (row = *(int *)bank_list_top; (*(int *)bank_list_top + 11) > row; row++) {
        if (((int)(unsigned char)bank_selected) == row) {
            colour = 246;
        } else {
            colour = 146;
        }
        mc_set_location(573, (int)D_00175CC4);
        mc_sprintf((int)text_buffer, (int)D_00175CEF, *(int *)(D_001A3FB4 + (row * 20)));
        text_draw_coloured((int)text_buffer, 52, (int)(short)(((row - *(short *)bank_list_top) * 7) + 38), (int)(short)*(short *)&colour, 156);
    }
    if (*(int *)bank_list_top != 0) {
        arrow_image = D_001A41E8;
    } else {
        arrow_image = D_001A4140;
    }
    xn_draw_image_transparent(153, 38, 9, 20, arrow_image);
    if ((*(int *)bank_list_top + 11) < ((int)(unsigned char)bank_house_count)) {
        arrow_image = D_001A41E8;
    } else {
        arrow_image = D_001A4140;
    }
    xn_draw_image_transparent(153, 102, 9, 16, arrow_image + 576);
}

void bank_draw_ship_list(void)
{
    int row;
    short colour;

    for (row = 0; ((int)(unsigned char)bank_ship_count) > row; row++) {
        if (((int)(unsigned char)bank_selected) == row) {
            *(int *)&colour = 246;
        } else {
            *(int *)&colour = 146;
        }
        text_draw_coloured(itoa(*(int *)(D_001A418E + (row * 74)), (int)text_rsc_buffer, 10), 52, (int)(short)((row * 7) + 38), (int)(short)colour, 156);
    }
}

void bank_house_list_click(void)
{
    int row;

    row = (int)(*(char **)bank_list_top + ((((int)(short)mouse_y) - 38) / 7));
    if (((int)(unsigned char)bank_house_count) <= row) return;
    bank_selected = *(signed char *)&row;
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
    int row;

    row = (((int)(short)mouse_y) - 38) / 7;
    if (((int)(unsigned char)bank_ship_count) <= row) return;
    bank_selected = *(signed char *)&row;
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

int bank_confirm(int text_id)
{
    msgbox_yes_no_rsc(text_id);
    while (1) {
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        bank_draw();
        msgbox_update();
        cursor_draw_arrow();
        xn_gfx_present_inclusive(1);
        if (((int)D_00196271) == 1) return 1;
        if (((int)D_00196271) == 2) return 0;
    }
}

int bank_input_amount(void)
{
    int done;
    char *input;

    done = 0;
    D_0012B508 = 146;
    input = *(char **)scratch_buffer + 55000;
    *input = 0;
    *(signed char *)D_00191020 = 0;
    text_cursor_x = 157;
    text_cursor_y = 152;
    inpstr_begin_text((int)D_00191020, 10);
    xn_kbd_flush();
    mouse_buttons = (mouse_buttons_prev = 0);
    while (done == 0) {
        bank_draw();
        done = inpstr_update();
        xn_gfx_present_inclusive(1);
    }
    return atoi((int)D_00191020);
}

void func_0006CB02(void)
{
    int i;

    for (i = 0; ((int)(unsigned char)bank_house_count) > i; i++) {
        *(int *)(*(char **)(*(char **)(bank_houses_for_sale + (i * 20)) + 5) + 52) = *(int *)(D_001A3FBC + (i * 20));
    }
}
