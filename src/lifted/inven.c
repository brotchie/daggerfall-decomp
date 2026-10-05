/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern struct region regions[];
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern char D_00176FE4[];
extern char D_00176FF1[];
extern char D_00176FFE[];
extern char D_0017700B[];
extern char D_00177018[];
extern char D_00177025[];
extern char D_00177032[];
extern char D_0017703F[];
extern char D_0017704C[];
extern char D_00177054[];
extern char D_00177063[];
extern char D_00177099[];
extern char D_001770A7[];
extern char D_001770C0[];
extern char D_001770CA[];
extern char D_001770EA[];
extern char D_0017710F[];
extern char D_00177129[];
extern char D_00177147[];
extern char D_0017716F[];
extern char D_0017718D[];
extern char D_001771B5[];
extern char D_001771C5[];
extern char D_001771D3[];
extern char D_001771FF[];
extern char D_00177217[];
extern char D_0017722B[];
extern char D_00177255[];
extern char D_0017727F[];
extern char D_00177300[];
extern char D_00177323[];
extern char D_00177346[];
extern char D_0017887F[];
extern int trade_price_scale;
extern unsigned char player_environment;
extern struct item_template item_templates[];
extern char potion_recipes[];
extern int D_001832A4;
extern int D_00184221;
extern int key_names[];
extern short spell_last_cast_id;
extern char item_group_templates[];
extern signed char D_00187CA8;
extern signed char D_00187DAC[];
extern signed char item_group_tab[];
extern struct rect inv_mode_buttons[][7];   /* 5 trade modes */
extern struct rect inv_buttons[];
extern char weapon_proficiency_bits[];
extern signed char text_buffer[];
extern struct record *creature_list[];
extern char D_00190B44[];
extern int D_00190CA8;
extern signed char scratch_190ce4[];
extern int text_macro_map_location;
extern char D_001913E4[];
extern signed char D_001940D4;
extern signed char D_001940D6;
extern signed char D_001940D8;
extern signed char player_motion_flags;
extern int D_0019597C[];
extern int left_hand_ready_delay;
extern struct record *inventory_containers[];
extern struct record *D_001959DC;
extern struct record *wagon_container;
extern struct item *text_macro_item;
extern struct record *camera_object;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *scratch_current_object;
extern struct record *location_object;
extern int D_00195ACC;
extern int text_macro_book;
extern int inventory_close_callback;
extern struct record *found_object;
extern int creature_count;
extern struct record *inv_right_container;
extern struct record *inv_right_container_base;
extern struct record *scratch_object;
extern char D_00195B84[];
extern char inpstr_result[];
extern struct character *player_character;
extern struct career *player_class;
extern int game_minutes;
extern struct settings *game_settings;
extern char scratch_buffer[];
extern int trade_total;
extern int trade_price;
extern int trade_mode;
extern int inventory_action;
extern int free_later_count;
extern int player_death_timer;
extern struct record *D_00195DA8;
extern int cfg_magic_repair;
extern short D_00195F2E;
extern char saved_player_object[];
extern char D_001960D9[];
extern signed char current_region;
extern unsigned char D_0019626F;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern signed char crime_current;
extern signed char inv_right_icon;
extern signed char D_0019628A;
extern signed char D_0019629A;
extern signed char D_0019629D;
extern signed char quests_suspended;
extern signed char D_001962AE;
extern signed char D_001962AF;
extern signed char D_001962B1;
extern struct quest *current_quest;
extern struct record *quest_reward_container;
extern struct record *itemmaker_item_object;
extern struct membership *guild_membership;
extern int inventory_images;
extern int D_001AA420;
extern int D_001AA424;
extern int D_001AA428;
extern int D_001AA42C;
extern int D_001AA430;
extern int D_001AA434;
extern int D_001AA438;
extern int D_001AA43C;
extern int D_001AA440;
extern int D_001AA444;
extern int D_001AA448;
extern int D_001AA44C;
extern int D_001AA450;
extern int D_001AA454;
extern char D_001AA458[];
extern struct record *inv_temp_pile;
extern char D_001AA460[];
extern int D_001AA4CC[];
extern int D_001AA534[];
extern int D_001AA53C;
extern int D_001AA540;
extern int D_001AA544;
extern struct record *inv_selected_item;
extern char inv_left_scroll[];
extern int inv_right_scroll;
extern char inv_left_rows[];
extern struct record *inv_left_container;
extern int D_001AA580;
extern short D_001AA586;
extern signed char trade_offer_pending;
extern signed char D_001AA5F8;

extern int damage_apply(struct record *, int, struct record *);
extern int sheet_open(short);
extern int spellbook_open(short);
extern int key_action_held(int);
extern int macro_kg_weight(void);
extern int object_weight(struct record *);
extern int holiday_today(int, int);
extern struct quest *quest_find_by_id(int);
extern int enchant_item_value(struct item *);
extern int cast_item_used_spell(int);
extern int spell_find_on_entity(int, short, int);
extern int equip_hiding_capacity(int);
extern struct record *monster_summon_near_player(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(char *, int);
extern int hud_message_add(char *);
extern int rand_range(int, int);
extern int gold_can_afford(int);
extern int carry_capacity(void);
extern struct record *object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern int object_find(struct record *, int (*)());
extern int object_new_id(int);
extern int inv_match_arrows(struct record *);
extern int inv_draw_item_cell(struct record *, int, int);
extern int trade_shop_takes_group(int);
extern int player_to_nearest_marker(struct record *, int);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A1054();
extern int xn_draw_image_scaled();
extern int xn_gfx_wait_vretrace_start();
extern int xn_gfx_wait_vretrace_end();
extern int xn_str_find_u32();
extern int xn_mouse_poll_clamped();
extern int xn_tex_cache_lookup();
extern int xn_tex_cache_flush();
extern int xn_tex_cache_begin_frame();
extern int xn_draw_image_transparent();
extern void quest_items_release_on_close(void);
extern void skill_add_uses(int, int);
extern void msgbox_show_string(int, int);
extern void msgbox_show_quest_text(struct quest *, short, int);
extern void msgbox_show_qrc_text(char *, unsigned short, short);
extern void msgbox_show_rsc(int, int);
extern void guards_summon(int);
extern void parse_expand(unsigned char *, char *);
extern void quest_raise_event(short, struct record *, struct record *);
extern void player_refresh_paperdoll(void);
extern void book_open(short);
extern void item_make(int, int, struct item *);
extern void item_next_clothing_style(struct item *);
extern void item_info_painting(struct item *);
extern void item_damage(struct record *, int);
extern void poison_apply(struct record *, int, int);
extern void weapon_reload_hand_sprites(void);
extern void book_read_header(int, unsigned short);
extern void text_draw_coloured(int, int, int, int, unsigned char);
extern void size_fit(short *, short *, short, short);
extern void object_free_pending(void);
extern void msgbox_yes_no_rsc(int);
extern void gold_add(int);
extern void gold_spend(int);
extern void location_free(struct loaded_location *);
extern void map_goto_location(int, int, int, int);
extern void location_pick_random_undiscovered(struct loaded_location *);
extern void location_set_discovered(int, int);
extern void spell_end(int);
extern void inpstr_begin_number(int);
extern void object_free_children(struct record *);
extern void object_foreach_pre(struct record *, void (*)());
extern void object_foreach(struct record *, void (*)());
extern void potion_drink(struct record *);
extern void inv_sum_hidden_weight(struct record *);
extern void trade_add_buy_price(struct record *);
extern void trade_add_repair_cost(struct record *);
extern void inv_return_unpaid_item(struct record *);
extern void inv_store_cb(struct record *);
extern void inv_claim_item(int);
extern void inv_assign_item_id(struct record *);
extern void inventory_draw(void);
extern void inv_select_tab(int);
extern void inv_click_right_item(int);
extern void inv_click_left_item(int);
extern void inv_equip_item(int);
extern void item_apply_equip_effects(struct record *, int);
extern void trade_schedule_shop_repairs(void);
extern void trade_mark_identified(void);
int inventory_open(int, int, int);
int trade_region_price(int);
int trade_adjust_price(int, int);
int trade_settle_offer(void);
int trade_base_price(int);
int trade_pay_spell_points(void);
int func_00098B91(struct record *);
int potion_recipe_text(signed char *);
void inventory_load_images(void);
void inventory_free_images(void);
void inventory_close(void);
void inv_unequip_item(struct record *);
void inv_equip_in_slot(struct record *, int);
void inv_close_return_unpaid(void);
void item_remove_equip_effects(struct record *, int);
void inv_store_item(struct record *);
void inv_count_cart_cb(struct record *);
void inv_drop_wagon_if_no_cart(void);
void inv_merge_arrows(struct record *, struct record *, int);
void trade_make_offer(void);
void inv_wagon_button(void);
void inv_claim_items(void);
void inv_read_map_scrap(struct record *);
void func_00098F1D(struct record *);
void inv_close_assign_ids(void);
void inv_track_hand_weapons(int);
void item_refresh_magic_value_cb(struct record *);
void inv_refresh_magic_values(void);
void trade_mark_in_repair(void);
#pragma aux mc_set_location parm routine [];

void inventory_load_images(void)
{
    inventory_images = disk_read_file(D_00176FE4, 0);
    D_001AA420 = disk_read_file(D_00176FF1, 0);
    D_001AA424 = disk_read_file(D_00176FFE, 0);
    D_001AA428 = disk_read_file(D_0017700B, 0);
    D_001AA42C = disk_read_file(D_00177018, 0);
    D_001AA430 = disk_read_file(D_00177025, 0);
    D_001AA43C = disk_read_file(D_00177032, 0);
    D_001AA440 = disk_read_file(D_0017703F, 0);
    if (trade_mode == 0) return;
    mc_set_location(313, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_00177054, (trade_mode * 2) + 6);
    D_001AA434 = disk_read_file(text_buffer, 0);
    mc_set_location(315, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_00177054, (trade_mode * 2) + 7);
    D_001AA438 = disk_read_file(text_buffer, 0);
}

void inventory_free_images(void)
{
    if (inventory_images != 0 && inventory_images != (-1751672937)) {
        mc_free(inventory_images, (int)D_0017704C, 322);
        inventory_images = -1751672937;
    }
    if (D_001AA420 != 0 && D_001AA420 != (-1751672937)) {
        mc_free(D_001AA420, (int)D_0017704C, 323);
        D_001AA420 = -1751672937;
    }
    if (D_001AA424 != 0 && D_001AA424 != (-1751672937)) {
        mc_free(D_001AA424, (int)D_0017704C, 324);
        D_001AA424 = -1751672937;
    }
    if (D_001AA428 != 0 && D_001AA428 != (-1751672937)) {
        mc_free(D_001AA428, (int)D_0017704C, 325);
        D_001AA428 = -1751672937;
    }
    if (D_001AA42C != 0 && D_001AA42C != (-1751672937)) {
        mc_free(D_001AA42C, (int)D_0017704C, 326);
        D_001AA42C = -1751672937;
    }
    if (D_001AA430 != 0 && D_001AA430 != (-1751672937)) {
        mc_free(D_001AA430, (int)D_0017704C, 327);
        D_001AA430 = -1751672937;
    }
    if (D_001AA43C != 0 && D_001AA43C != (-1751672937)) {
        mc_free(D_001AA43C, (int)D_0017704C, 328);
        D_001AA43C = -1751672937;
    }
    if (D_001AA440 != 0 && D_001AA440 != (-1751672937)) {
        mc_free(D_001AA440, (int)D_0017704C, 329);
        D_001AA440 = -1751672937;
    }
    if (trade_mode == 0) return;
    if (D_001AA434 != 0 && D_001AA434 != (-1751672937)) {
        mc_free(D_001AA434, (int)D_0017704C, 333);
        D_001AA434 = -1751672937;
    }
    if (D_001AA438 == 0 || D_001AA438 == (-1751672937)) return;
    mc_free(D_001AA438, (int)D_0017704C, 334);
    D_001AA438 = -1751672937;
}

int inventory_open(int how, int mode, int icon)
{
    if (player_death_timer > 0) return 0;
    if (((int)D_0019626F) == 4 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (how != 0 || (game_mode == 0 && key_action_held(37) != 0 && player_death_timer == 0)) {
        inv_temp_pile = 0;
        if (player_character->race > 8) {
            msgbox_show_string((int)D_00177063, 1);
            return 0;
        }
        if (how == 0) how = 1;
        if (mode == 0 && how == 1) {
            inv_temp_pile = (inv_right_container_base = (inv_right_container = object_create_child(player_object->parent, 0, 0)));
            inv_right_container->type = 33;
            inv_right_container->image = ((unsigned short)(unsigned char)D_00187DAC[rand() % 20]) + 27648;
            inv_right_container->pad19 = 1;
            inv_right_container->flags |= 5;
            inv_right_container->x = player_object->x;
            inv_right_container->y = player_object->y;
            inv_right_container->z = player_object->z;
            inv_right_container->id = object_new_id(((unsigned)location_object->id) >> 16);
            how = 2;
        }
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        D_00187CA8 = 0;
        game_mode = 4;
        D_00196272 = 1;
        trade_mode = mode;
        D_001AA5F8 = (inv_right_icon = *(signed char *)&icon);
        inventory_action = 2;
        trade_total = (trade_price = 0);
        inv_right_scroll = (*(int *)inv_left_scroll = 0);
        inventory_load_images();
        inv_track_hand_weapons(0);
        inv_refresh_magic_values();
        quests_suspended = 1;
        D_001AA53C = (int)inv_right_container;
    }
    return ((((int)(unsigned char)game_mode) == 4) ? 1 : 0);
}

void inventory_frame(void)
{
    int button;
    int price;
    int holiday;

    if (inventory_open(0, 0, 2) == 0) return;
    xn_gfx_wait_vretrace_start();
    xn_gfx_wait_vretrace_end();
    xn_tex_cache_begin_frame();
    inventory_draw();
    price = trade_settle_offer();
    if (price > 0) {
        if (trade_mode != 2 && gold_can_afford(price) == 0) {
            msgbox_show_rsc(454, 1);
        } else if (trade_mode != 2) {
            holiday = holiday_today(game_minutes, (int)(unsigned char)current_region);
            if (trade_mode == 4) {
                if (holiday != 43 && ((struct bf8_7_1 *)&player_motion_flags)->f == 0) {
                    gold_spend(price);
                }
            } else {
                gold_spend(price);
            }
            if (trade_mode == 4) {
                trade_mark_identified();
            } else if (trade_mode == 1) {
                inv_claim_items();
            } else if (trade_mode == 3) {
                trade_mark_in_repair();
            }
        } else {
            gold_add(price);
            inv_drop_wagon_if_no_cart();
            object_free_children(inv_right_container);
        }
        sound_play(204, player_object, 100);
    }
    if (key_down_esc != 0) inventory_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    if (((int)(unsigned char)game_mode) != 4) return;
    for (button = 0; button < 7; button++) {
        if (mouse_x > inv_mode_buttons[trade_mode][button].x0 && mouse_x < inv_mode_buttons[trade_mode][button].x1 && mouse_y > inv_mode_buttons[trade_mode][button].y0 && mouse_y < inv_mode_buttons[trade_mode][button].y1) {
            sound_play(203, player_object, 100);
            inv_mode_buttons[trade_mode][button].handler(button, 27);
            break;
        }
    }
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (button = 0; button < 45; button++) {
        if (mouse_x > inv_buttons[button].x0 && mouse_x < inv_buttons[button].x1 && mouse_y > inv_buttons[button].y0 && mouse_y < inv_buttons[button].y1) {
            sound_play(203, player_object, 100);
            inv_buttons[button].handler(button, 27);
            return;
        }
    }
}

void inventory_close(void)
{
    struct record *object;
    struct record *next;

    if (inv_left_container == wagon_container && trade_mode == 1) inv_select_tab(41);
    if (inv_right_container == wagon_container) inv_wagon_button();
    while (key_down_esc != 0);
    if (trade_mode == 0) inv_claim_items();
    if (trade_mode == 2 || trade_mode == 4 || trade_mode == 3) {
        object = inv_right_container_base->children;
        while (object != 0 && object->type == 2) {
            next = object->next;
            if (((int)(unsigned short)(object->flags & 32)) == 0) inv_store_item(object);
            object = next;
        }
    }
    D_001962B1 = 0;
    D_00187CA8 = 1;
    inventory_free_images();
    quest_reward_container = 0;
    quest_items_release_on_close();
    inv_close_return_unpaid();
    D_001940D8 |= 8;
    trade_price_scale = 256;
    inv_close_assign_ids();
    if (D_001962AF != 0) {
        D_001962AF = 0;
        object_free_children((struct record *)D_001960D9);
    }
    if ((struct record *)D_001960D9 == inv_right_container_base) {
        *(int *)&inv_right_container = (*(int *)&inv_right_container_base = 0);
    }
    while (inv_right_container != inv_right_container_base) {
        inv_right_container = (struct record *)inv_right_container->parent;
    }
    if (inv_right_container->type == 33 && inv_right_container->children == 0) {
        inv_right_container->flags |= 0x200;
    }
    if (inv_right_container->type == 33) inv_right_container->flags |= 1;
    if (D_00195DA8->type == 33 && D_00195DA8->children == 0) {
        D_00195DA8->flags |= 0x200;
    }
    if (inventory_close_callback != 0) ((int (*)())(inventory_close_callback))();
    if (inv_temp_pile != 0 && inv_temp_pile->children == 0) object_delete(inv_temp_pile);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    game_mode = 0;
    D_00196272 = 0;
    D_001940D6 &= 123;
    player_motion_flags &= 127;
    weapon_reload_hand_sprites();
    inv_track_hand_weapons(1);
    if (D_0019597C[0] > 0 && player_character->equipped[19] != 0) {
        text_macro_item = &player_character->equipped[19]->data.item;
        parse_expand(D_00177099, D_001913E4);
        hud_message_add(D_001913E4);
    }
    if (left_hand_ready_delay > 0 && player_character->equipped[21] != 0) {
        text_macro_item = &player_character->equipped[21]->data.item;
        parse_expand(D_00177099, D_001913E4);
        hud_message_add(D_001913E4);
    }
    if (((struct bf8_5_1 *)&D_001940D8)->f != 0) {
        D_001940D8 &= 223;
        sheet_open(1);
    }
    quests_suspended = 0;
}

void inv_draw_container_icon(int button, int icon)
{
    struct image *image;
    int i;
    int x;
    int y;

    image = (struct image *)D_001AA440;
    i = 0;
    if (icon == 1 && ((int)(unsigned short)(game_settings->view_flags & 4)) != 0) icon = 10;
    while (i < icon) {
        image = (struct image *)((char *)image + image->data_size + 12);
        i++;
    }
    x = inv_buttons[button].x0 + ((((int)&*(signed char *)((char *)(inv_buttons[button].x1 - inv_buttons[button].x0) + 1)) - image->width) >> 1);
    y = inv_buttons[button].y0 + ((((inv_buttons[button].y1 - inv_buttons[button].y0) + 1) - image->height) >> 1);
    xn_draw_image_transparent(x, y, image->width, image->height, image->pixels);
    if (wagon_container == 0 || icon != 3) return;
    D_001962AE = 1;
    scratch_current_object = wagon_container;
    mc_set_location(657, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_001770A7, macro_kg_weight());
    text_draw_coloured((int)text_buffer, (int)(short)(x + 1), (int)(short)(y + 1), 145, 156);
    D_001962AE = 0;
}

void func_00093BD9(int unused, struct rect *buttons, int button)
{
    struct image *image;
    short centre_x;
    short centre_y;
    short width;
    short height;
    short i;

    *(int *)&centre_x = (buttons[button].x0 + buttons[button].x1) >> 1;
    *(int *)&centre_y = (buttons[button].y0 + buttons[button].y1) >> 1;
    image = (struct image *)D_001AA440;
    *(int *)&i = 0;
    while (((int)(short)i) < 3) {
        image = (struct image *)((char *)image + image->data_size + 12);
        (*(int *)&i)++;
    }
    width = image->width;
    height = image->height;
    size_fit(&width, &height, (int)(short)((buttons[button].x1 - buttons[button].x0) - 4), (int)(short)((buttons[button].y1 - buttons[button].y0) - 4));
    for (button = 0; image->height > button; button++) {
        mc_memcpy((int)(*(char **)scratch_buffer + (button << 8)), image->pixels + (image->width * button), image->width, (int)D_0017704C, 787, 4);
    }
    xn_draw_image_scaled(((int)(short)centre_x) - (((int)(short)width) >> 1), ((int)(short)centre_y) - (((int)(short)height) >> 1), (int)(short)width, (int)(short)height, image->width, image->height, 0, *(int *)scratch_buffer);
    mc_set_location(791, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_001770C0, macro_kg_weight());
    text_draw_coloured((int)text_buffer, (int)(short)(buttons[button].x0 + 3), (int)(short)(buttons[button].y0 + 2), 145, 156);
}

void inv_draw_cell_mark(int archive, int record_index, struct rect *buttons, int button)
{
    int texture;
    struct texture_header *image;
    int centre_x;
    int centre_y;
    int width;
    int height;

    texture = xn_tex_cache_lookup(archive, record_index, -1);
    if (texture == 0) {
        xn_tex_cache_flush();
        texture = xn_tex_cache_lookup(archive, record_index, -1);
    }
    image = *(struct texture_header **)((char *)texture + 12);
    centre_x = (buttons[button].x0 + buttons[button].x1) >> 1;
    centre_y = (buttons[button].y0 + buttons[button].y1) >> 1;
    width = image->width;
    height = image->height;
    xn_draw_image_scaled(centre_x - (width >> 1), centre_y - (height >> 1), width, height, image->width, image->height, (int)(unsigned short)(image->flags | 32768), (char *)image + image->data_offset);
}

void inv_scroll_left_up(void)
{
    if (*(int *)inv_left_scroll == 0) return;
    (*(int *)inv_left_scroll)--;
}

void inv_click_list_row(int row, int first_button)
{
    row -= first_button;
    switch ((unsigned)row) {
    case 0:
        row = 4;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        row--;
        break;
    case 5:
        row = 9;
        break;
    case 6:
    case 7:
    case 8:
    case 9:
        row--;
    }
    if (row < 5 && *(int *)(inv_left_rows + (row << 2)) != 0) {
        inv_click_left_item(*(int *)(inv_left_rows + (row << 2)));
        return;
    }
    if (row <= 4 || row >= 10 || D_001AA534[row] == 0) return;
    inv_click_right_item(D_001AA534[row]);
}

int inv_take_item(struct record *object)
{
    struct item *item;
    int unused;
    struct record **slot;
    int weight;
    int carried;
    int capacity;

    item = &object->data.item;
    if (object->parent->parent != player_entity) {
        if (inv_left_container != wagon_container) {
            if (item->group != 23) {
                weight = object_weight(object);
                carried = object_weight(player_entity);
                capacity = carry_capacity() << 2;
                if ((weight + carried) > capacity) {
                    msgbox_show_string((int)D_001770CA, 1);
                    return 0;
                }
            }
        } else {
            weight = object_weight(object);
            D_001962AE = 1;
            carried = object_weight(wagon_container);
            D_001962AE = 0;
            if ((weight + carried) > 3000) {
                msgbox_show_string((int)D_001770EA, 1);
                return 0;
            }
        }
    }
    if (item->group == 23 && item->index == 0) {
        inv_store_item(object);
        return 0;
    }
    if (item->group == 27 && item->index == 8) {
        inv_read_map_scrap(inv_selected_item);
        return 0;
    }
    if (object->type == 54) object->type = 2;
    if (item->group == 3 && item->index == 18) {
        capacity = item->stack_count;
        inv_merge_arrows(player_entity, object, 1);
        found_object->data.item.condition = capacity;
        return 0;
    }
    slot = (struct record **)xn_str_find_u32(player_character->equipped, (int)inv_selected_item, 27);
    if (slot != 0) {
        object = *slot;
        item_remove_equip_effects(object, ((int)slot - (int)player_character->equipped) / 4);
        *slot = 0;
        return 0;
    }
    D_001AA454 = 0;
    object->id = object_new_id(100);
    if (object->twin != 0) object->twin->id = object->id;
    func_00098F1D(object);
    item = &object->data.item;
    if (item->group == 28 && item->index == 0) {
        if (object != inv_right_container) D_001AA454 += item->value;
        object_free_single(object);
    } else {
        object->caster = 0;
        quest_raise_event(3, object, 0);
        object->x = player_object->x;
        object->y = player_object->y;
        object->z = player_object->z;
        if (inv_left_container != wagon_container) {
            inv_store_item(object);
            return 1;
        }
        object_reparent(inv_left_container, object);
        return 0;
    }
    if (D_001AA454 != 0) {
        gold_add(D_001AA454);
        D_0012B508 = 144;
        mc_set_location(1237, (int)D_0017704C);
        mc_sprintf((int)text_buffer, D_001832A4, D_001AA454);
        msgbox_show_string((int)text_buffer, 1);
        sound_play(204, player_object, 100);
        return 0;
    }
    return 0;
}

void inv_update_hidden_load(void)
{
    int capacity;

    capacity = equip_hiding_capacity(1);
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity, inv_sum_hidden_weight);
    player_character->hidden_load_percent = (*(int *)D_00195B84 * 100) / capacity;
}

void inv_unequip_item(struct record *object)
{
    int slot;

    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] == object) {
            item_remove_equip_effects(object, slot);
            player_character->equipped[slot] = 0;
            return;
        }
    }
}

void inv_unequip_all_saved(void)
{
    struct item *item;
    int slot;

    mc_memset((int)D_001AA4CC, 0, 108, (int)D_0017704C, 1274, 108);
    mc_memset((int)D_001AA460, 0, 108, (int)D_0017704C, 1275, 108);
    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] != 0) {
            item = &player_character->equipped[slot]->data.item;
            if (item->enchantments[0].type != (-1)) {
                D_001AA4CC[slot] = (int)player_character->equipped[slot];
                *(int *)(D_001AA460 + (slot << 2)) = item->condition;
                item->condition = item->max_condition;
                inv_unequip_item(player_character->equipped[slot]);
            }
        }
    }
}

void inv_reequip_saved(void)
{
    struct item *item;
    int slot;

    for (slot = 0; slot < 27; slot++) {
        if (D_001AA4CC[slot] != 0) {
            inv_equip_item(D_001AA4CC[slot]);
            item = (struct item *)(D_001AA4CC[slot] + 71);
            item->condition = *(short *)(D_001AA460 + (slot << 2));
        }
    }
}

void inv_use_item(void)
{
    struct item *item;
    int unused;
    struct record *object;
    struct record *monster;
    int i;
    int used;
    short saved_spell;

    used = 0;
    object = inv_selected_item;
    item = &inv_selected_item->data.item;
    if (((int)(unsigned short)(object->flags & 32)) != 0) {
        msgbox_show_string((int)D_0017710F, 1);
        return;
    }
    item->item_flags |= 0x200;
    if (object->twin != 0) object->twin->data.item.item_flags |= 0x200;
    D_001940D8 |= 8;
    if (item->enchantments[0].type == 26 && item->enchantments[0].param == 3) {
        cast_item_used_spell(92);
        item_damage(inv_selected_item, 50);
        return;
    }
    if (item->enchantments[0].type == 26 && item->enchantments[0].param == 4) {
        if (creature_count == 0) {
            hud_message_add(D_00177129);
            return;
        }
        D_0019629D = 1;
        monster = monster_summon_near_player(27);
        D_0019629D = 0;
        if (monster == 0) {
            hud_message_add(D_00177147);
            return;
        }
        monster->data.character.flags |= 2;
        item_damage(inv_selected_item, 100);
        return;
    }
    if (item->enchantments[0].type == 26 && item->enchantments[0].param == 8) {
        if (creature_count == 0) {
            hud_message_add(D_0017716F);
            return;
        }
        D_0019629D = 1;
        monster = monster_summon_near_player(creature_list[0]->data.character.mobile_id);
        D_0019629D = 0;
        if (monster == 0) {
            hud_message_add(D_0017718D);
            return;
        }
        monster->data.character.flags |= 2;
        item_damage(inv_selected_item, 100);
        return;
    }
    if (item->enchantments[0].type == 26 && item->enchantments[0].param == 5) {
        sheet_open(50);
        object_delete(inv_selected_item);
        return;
    }
    if (item->enchantments[0].type == 26 && item->enchantments[0].param == 9) {
        if (inv_selected_item->children != 0) {
            object_delete(inv_selected_item->children);
            msgbox_show_rsc(32, 1);
        } else {
            msgbox_show_rsc(20, 1);
        }
        return;
    }
    if (item->enchantments[0].type != (-1)) {
        i = 0;
        while (i < 10 && item->enchantments[i].type != (-1)) {
            if (item->enchantments[i].type == 0) {
                *(int *)&saved_spell = (int)(short)spell_last_cast_id;
                D_0019629A = 1;
                cast_item_used_spell(item->enchantments[i].param);
                D_0019629A = 1;
                used = 1;
                spell_last_cast_id = *(int *)&saved_spell;
                item_damage(inv_selected_item, 10);
            }
            if (item->enchantments[i].type == 21 && item->enchantments[i].param == 0) {
                damage_apply(player_entity, (int)&*(signed char *)((char *)(player_character->level >> 1) + 1), 0);
                item_damage(inv_selected_item, 2);
            }
            i++;
        }
    }
    if (item->group == 9 && item->index == 5 && (short)item->message != 0) {
        if (func_00098B91(inv_selected_item) != 0) {
            msgbox_show_quest_text(current_quest, (int)(short)(short)item->message, 1);
        } else {
            msgbox_show_qrc_text(item->name + 10, (int)(unsigned short)(short)item->message, 1);
        }
        return;
    }
    if (item->group == 0) {
        poison_apply(player_entity, item->index + 136, 1);
        object_delete(object);
        return;
    }
    if (item->group == 27 && item->index == 0) {
        if (object->children == 0) {
            msgbox_show_rsc(12, 1);
        } else {
            while (mouse_buttons != 0) xn_mouse_poll_clamped();
            inventory_close();
            spellbook_open(1);
            D_001940D8 |= 128;
        }
        return;
    }
    if (item->group == 7) {
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        inventory_close();
        book_open((int)(short)(short)item->message);
        D_001940D8 |= 128;
        return;
    }
    if (item->group == 6 || item->group == 12) {
        item_next_clothing_style(item);
        return;
    }
    if (item->group == 1 && item->index == 1 && object->children != 0 && object->children->type == 31) {
        potion_drink(object->children);
        object_delete(object);
        return;
    }
    if (used != 0 || object->quest_id != 0) return;
    D_00195F2E = 30;
    D_0012B508 = 146;
    msgbox_show_string(D_00184221, 1);
}

void inv_item_info(struct record *object, struct item *item)
{
    int header;

    D_0012B508 = 146;
    scratch_current_object = object;
    text_macro_item = item;
    if (item->enchantments[0].type == 26 && item->enchantments[0].param == 9) {
        msgbox_show_rsc(1004, 1);
    } else if (item->group == 27 && item->index == 4) {
        D_00195ACC = ((int)potion_recipes) + (item->stack_count * 109);
        msgbox_show_string((int)D_001771B5, 1);
        msgbox_show_string(potion_recipe_text((signed char *)D_00195ACC), 1);
    } else if (item->group == 27 && item->index == 6) {
        msgbox_show_rsc(1073, 1);
    } else if (item->group == 13) {
        item_info_painting(item);
    } else if (item->group == 7) {
        if (item->enchantments[0].type == 26) {
            msgbox_show_rsc(1015, 1);
        } else {
            header = *(int *)scratch_buffer + 63000;
            book_read_header(header, (int)(unsigned short)(short)item->message);
            text_macro_book = header;
            msgbox_show_rsc(1009, 1);
        }
    } else if (item->group == 2) {
        if (((int)(unsigned short)(text_macro_item->item_flags & 2048)) != 0) {
            msgbox_show_rsc(1014, 1);
        } else {
            msgbox_show_rsc(1000, 1);
        }
    } else if (item->group == 3) {
        if (object->children != 0) {
            msgbox_show_rsc(1005, 1);
        } else if (text_macro_item->index == 18) {
            msgbox_show_rsc(1011, 1);
        } else if (((int)(unsigned short)(text_macro_item->item_flags & 2048)) != 0) {
            if (text_macro_item->group == 3) {
                msgbox_show_rsc(1012, 1);
            } else {
                msgbox_show_rsc(1013, 1);
            }
        } else {
            msgbox_show_rsc(1001, 1);
        }
    } else if (item->group == 1 && object->children != 0 && object->children->type == 11) {
        msgbox_show_rsc(1006, 1);
    } else if (item->group == 1 && item->index == 1 && object->children != 0 && object->children->type == 31) {
        D_00195ACC = (int)object->children + 71;
        msgbox_show_rsc(1008, 1);
    } else if (item->group == 27 && item->index == 1) {
        msgbox_show_rsc(1004, 1);
    } else if (item->group == 27 && item->index == 2) {
        msgbox_show_rsc(1007, 1);
    } else {
        msgbox_show_rsc(1003, 1);
    }
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    mouse_buttons_prev = 0;
    if (item->enchantments[0].type != (-1)) msgbox_show_rsc(1016, 1);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    mouse_buttons_prev = 0;
}

void inv_list_left_item(struct record *object, int rects)
{
    struct item *item;

    if ((object->type != 50 && object->type != 2) || ((int)(unsigned short)(object->flags & 2)) != 0) return;
    item = &object->data.item;
    if (guild_membership != 0 && guild_membership->kind == 3) {
        if (trade_mode == 2 && item->enchantments[0].type == (-1)) return;
        if (((struct bf8_2_1 *)&D_001940D8)->f == 0 && xn_str_find_u32((int)player_character + 367, object, 27) != 0 && item->group != 1) {
            return;
        }
    } else {
        if (trade_mode == 2 && trade_shop_takes_group(item->group) == 0) return;
        if (((struct bf8_1_1 *)&D_001940D4)->f != 0 && item->condition == item->max_condition) {
            return;
        }
        if (((struct bf8_1_1 *)&D_001940D4)->f != 0 && item->enchantments[0].type != (-1)) return;
        if ((((int)(unsigned char)game_mode) == 10 || (((int)D_0019626F) == 10 && ((int)(unsigned char)game_mode) == 8)) && (item->enchantments[0].type != (-1) || item->enchant_points == 0 || (int)itemmaker_item_object == object || (item->group == 3 && item->index == 18) || (item->group == 27 && item->index == 1) || item->group == 23)) {
            return;
        }
        if (((struct bf8_2_1 *)&D_001940D8)->f == 0 && xn_str_find_u32((int)player_character + 367, object, 27) != 0 && item->group != 1) {
            return;
        }
    }
    if (((int)(short)D_001AA586) >= *(int *)inv_left_scroll && ((int)(short)D_001AA586) < (*(int *)inv_left_scroll + 4)) {
        *(int *)(inv_left_rows + ((((int)(short)D_001AA586) - *(int *)inv_left_scroll) << 2)) = (int)object;
        if (object->type != 50) {
            inv_draw_item_cell(object, (int)(short)(D_001AA586 - *(short *)inv_left_scroll), rects);
        }
    }
    if (((int)(unsigned char)game_mode) == 10) {
        if (item->enchantments[0].type != (-1)) return;
    }
    D_001AA586++;
}

void inv_equip_in_slot_pair(struct record *object, int slot, int step)
{
    if (player_character->equipped[slot] != 0) {
        if (player_character->equipped[slot + step] != 0) {
            inv_equip_in_slot(object, slot);
        } else {
            inv_equip_in_slot(object, slot + step);
        }
        return;
    }
    inv_equip_in_slot(object, slot);
}

void inv_unequip_slot(int slot)
{
    item_remove_equip_effects(player_character->equipped[slot], slot);
    player_character->equipped[slot] = 0;
}

void inv_equip_in_slot(struct record *object, int slot)
{
    if (player_character->equipped[slot] != 0) {
        item_remove_equip_effects(player_character->equipped[slot], slot);
        player_character->equipped[slot] = object;
        quest_raise_event(3, object, 0);
        item_apply_equip_effects(object, slot);
        return;
    }
    player_character->equipped[slot] = object;
    quest_raise_event(3, object, 0);
    item_apply_equip_effects(object, slot);
}

int item_is_two_handed(struct record *object)
{
    struct item *item;

    if (object == 0) return 0;
    item = &object->data.item;
    return ((((int)(unsigned short)(item->item_flags & 4)) == 0) ? 1 : 0);
}

void trade_total_buy(void)
{
    int unused;

    D_00190CA8 = 0;
    trade_total = 0;
    object_foreach(player_entity->children, trade_add_buy_price);
    trade_total = trade_base_price(trade_total);
    trade_price = ((trade_price = trade_adjust_price(trade_total, 0)) * trade_price_scale) / 256;
}

int trade_total_repair(void)
{
    int total;
    int unused;

    trade_total = 0;
    object_foreach(inv_right_container->children, trade_add_repair_cost);
    if (trade_total > 0) {
        total = trade_total;
    } else {
        total = 1;
    }
    trade_total = trade_base_price((trade_total = total));
    trade_price = ((trade_price = trade_adjust_price(trade_total, 0)) * trade_price_scale) / 256;
    return trade_total;
}

void inv_close_return_unpaid(void)
{
    int slot;

    free_later_count = 0;
    object_foreach_pre(player_entity->children, inv_return_unpaid_item);
    if (D_0019628A != 0) {
        object_foreach_pre(player_entity->children, inv_store_cb);
    }
    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] != 0 && ((int)(unsigned short)(player_character->equipped[slot]->flags & 32)) != 0) {
            player_character->equipped[slot] = 0;
        }
    }
    object_free_pending();
}

void item_remove_equip_effects(struct record *object, int slot)
{
    int i;
    struct item *item;
    int spell;

    i = 0;
    item = &object->data.item;
    while (i < 10 && item->enchantments[i].type != (-1)) {
        switch (item->enchantments[i].type) {
        case 1:
            spell = spell_find_on_entity((int)player_entity, item->enchantments[i].param, slot + 200);
            if (spell != 0) spell_end(spell);
            break;
        case 3:
            if (item->magicka_bonus != 0) {
                player_character->magicka -= (unsigned short)item->magicka_bonus;
                player_character->max_magicka -= (unsigned short)item->magicka_bonus;
                if (player_character->magicka < 0) player_character->magicka = 0;
            }
            break;
        case 9:
            player_character->conditions &= ~0x200;
            break;
        case 10:
            player_character->skills[item->enchantments[i].param].value -= 15;
        }
        i++;
    }
}

void item_repair_cb(struct record *object)
{
    struct item *item;

    if (object->type != 2) return;
    if (*(int *)D_00195B84 == 0) return;
    item = &object->data.item;
    if (item->group == 3 && item->index == 18) return;
    if (item->enchantments[0].type != (-1) && cfg_magic_repair == 0) return;
    if (item->condition == item->max_condition) return;
    item->condition += *(short *)D_00195B84;
    if (item->condition > item->max_condition) item->condition = item->max_condition;
    *(int *)D_00195B84 = 0;
}

void item_break(struct record *object)
{
    struct item *item;
    int i;
    int slot;

    item = &object->data.item;
    slot = -1;
    text_macro_item = item;
    item->condition = 0;
    inv_store_item(object);
    mc_set_location(2157, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_001771C5, item->name);
    parse_expand(text_buffer, D_00190B44);
    hud_message_add(D_00190B44);
    for (i = 0; i < 27; i++) {
        if (player_character->equipped[i] == object) slot = i;
    }
    if (item->enchantments[0].type != (-1)) {
        if (slot != (-1)) item_remove_equip_effects(object, slot);
        i = 0;
        while (i < 10 && item->enchantments[i].type != (-1)) {
            if (item->enchantments[i].type == 15) {
                hud_message_add(D_001771D3);
                monster_summon_near_player(item->enchantments[i].param)->data.character.team = 1;
            }
            i++;
        }
        object_delete(object);
    }
    if (slot != (-1)) player_character->equipped[slot] = 0;
    weapon_reload_hand_sprites();
    player_refresh_paperdoll();
}

void inv_store_item(struct record *object)
{
    struct item *item;
    int unused;

    object->id = object_new_id(100);
    if (object->twin != 0) object->twin->id = object->id;
    func_00098F1D(object);
    object->caster = 0;
    item = &object->data.item;
    if (item->enchantments[0].type != (-1) || (item->group == 27 && item->index == 0)) {
        object_reparent(D_001959DC, object);
        return;
    }
    object_reparent(inventory_containers[(int)(unsigned char)item_group_tab[item->group]], object);
}

void inv_count_cart_cb(struct record *object)
{
    struct item *item;

    if (object->type != 2) return;
    item = &object->data.item;
    if (item->group != 23 || item->index != 0) return;
    (scratch_190ce4[0])++;
    scratch_object = object;
}

void inv_create_wagon(void)
{
    if (wagon_container != 0) return;
    (wagon_container = object_create_child(player_entity, 0, 0))->type = 52;
    wagon_container->flags = 3;
    wagon_container->container_index = 4;
}

void inv_drop_wagon_if_no_cart(void)
{
    scratch_190ce4[0] = 0;
    object_foreach(player_entity, inv_count_cart_cb);
    if (scratch_190ce4[0] != 0 || wagon_container == 0) return;
    object_delete(wagon_container);
    wagon_container = 0;
}

void inv_merge_arrows(struct record *owner, struct record *arrows, int delete_source)
{
    struct record *stack;
    int count;

    found_object = 0;
    object_find(owner->children, inv_match_arrows);
    if (found_object == 0) {
        stack = object_create_child(owner, 0, 107);
        found_object = stack;
        stack->type = 2;
        stack->image2 = 998;
        stack->image = 0;
        item_make(3, 18, &stack->data.item);
        stack->data.item.stack_count = arrows->data.item.stack_count;
        if (owner == player_entity) inv_store_item(stack);
    } else {
        count = found_object->data.item.stack_count + arrows->data.item.stack_count;
        if (count >= 200) count = 199;
        found_object->data.item.stack_count = *(signed char *)&count;
    }
    if (delete_source == 0) return;
    object_delete(arrows);
}

int trade_total_sell(void)
{
    struct record *object;
    struct item *item;
    int total;

    total = 0;
    object = inv_right_container_base->children;
    while (object != 0) {
        if (((int)(unsigned short)(object->flags & 32)) == 0) {
            item = &object->data.item;
            total += item->value;
        }
        object = object->next;
    }
    trade_total = trade_base_price(total);
    trade_price = trade_adjust_price(trade_total, 1);
    return trade_total;
}

int trade_region_price(int price)
{
    price = (regions[(unsigned char)current_region].price_adjustment * price) / 1000;
    if (price < 0) price = 1;
    return price;
}

int trade_adjust_price(int price, int selling)
{
    int skill_quality;
    int attribute_quality;
    int skill_factor;
    int attribute_factor;

    skill_quality = ((current_building->quality - 10) * 5) + 50;
    attribute_quality = ((current_building->quality - 10) * 5) + 50;
    if (selling == 0) {
        skill_factor = ((((skill_quality << 8) / 200) + 128) * ((int)&*(signed char *)((char *)(((100 - player_character->skills[14].value) << 8) / 200) + 128))) / 256;
        attribute_factor = ((((attribute_quality << 8) / 200) + 128) * ((int)&*(signed char *)((char *)(((100 - player_character->attributes[5]) << 8) / 200) + 128))) / 256;
        price = (price * (((skill_factor * 192) / 256) + ((attribute_factor << 6) / 256))) / 256;
    } else {
        skill_factor = (((((100 - skill_quality) << 8) / 200) + 128) * ((int)&*(signed char *)((char *)((player_character->skills[14].value << 8) / 200) + 128))) / 256;
        attribute_factor = (((((100 - attribute_quality) << 8) / 200) + 128) * ((int)&*(signed char *)((char *)((player_character->attributes[5] << 8) / 200) + 128))) / 256;
        price = (price * (((skill_factor * 179) / 256) + ((attribute_factor * 51) / 256))) / 256;
    }
    trade_price = price;
    return price;
}

void trade_make_offer(void)
{
    int text_id;
    int holiday;

    trade_offer_pending = 1;
    holiday = holiday_today(game_minutes, (int)(unsigned char)current_region);
    if ((trade_mode == 4 && holiday == 43) || ((struct bf8_7_1 *)&player_motion_flags)->f != 0) {
        return;
    }
    if ((trade_total >> 1) > trade_price) {
        text_id = 260;
    } else if ((trade_total - (trade_total >> 2)) > trade_price) {
        text_id = 261;
    } else {
        text_id = 262;
    }
    if (trade_mode == 2) text_id += 3;
    msgbox_yes_no_rsc(text_id);
}

int trade_settle_offer(void)
{
    int holiday;

    if (trade_offer_pending != 0) {
        holiday = holiday_today(game_minutes, (int)(unsigned char)current_region);
        if ((trade_mode == 4 && holiday == 43) || ((struct bf8_7_1 *)&player_motion_flags)->f != 0) {
            return 1;
        }
        if (((int)(unsigned char)game_mode) == 8) return -1;
        trade_offer_pending = 0;
        if (((int)D_00196271) == 2) return -1;
        skill_add_uses(14, 1);
        return trade_price;
    }
    return 0;
}

int trade_base_price(int price)
{
    price = trade_region_price(price);
    price += ((current_building->quality - 10) * price) / 100;
    price += price;
    return price;
}

void shop_quality_message(struct building *building)
{
    short text_id;

    D_0012B508 = 146;
    if (building->quality <= 3) {
        *(int *)&text_id = 270;
    } else if (building->quality <= 7) {
        *(int *)&text_id = 269;
    } else if (building->quality <= 13) {
        *(int *)&text_id = 268;
    } else if (building->quality <= 17) {
        *(int *)&text_id = 267;
    } else {
        *(int *)&text_id = 266;
    }
    msgbox_show_rsc((int)(short)text_id, 1);
}

void inv_wagon_button(void)
{
    int unused1;
    int unused2;

    if (wagon_container == 0) return;
    if (D_001962B1 == 0 && ((int)player_environment) == 3) {
        return;
    }
    inv_right_icon = 3;
    if (trade_mode == 0) {
        if (wagon_container == inv_right_container) {
            *(int *)&inv_right_container_base = (*(int *)&inv_right_container = D_001AA53C);
        } else {
            *(int *)&inv_right_container_base = (*(int *)&inv_right_container = (int)wagon_container);
        }
        return;
    }
    inv_left_container = wagon_container;
}

void inv_info_button(void)
{
    inventory_action = 1;
}

void inv_equip_button(void)
{
    inventory_action = 2;
}

void inv_remove_button(void)
{
    inventory_action = 3;
}

void inv_use_button(void)
{
    inventory_action = 4;
}

void inv_gold_button(void)
{
    if (player_character->gold == 0) return;
    inpstr_begin_number(0);
    msgbox_show_rsc(25, 2);
    if (*(int *)inpstr_result < 1) return;
    if (((unsigned)*(int *)inpstr_result) > player_character->gold) return;
    player_character->gold -= *(int *)inpstr_result;
}

void trade_steal_button(void)
{
    int chance;

    chance = (player_character->skills[15].value - (trade_total / 32)) - (D_00190CA8 / 4);
    if ((rand() % 101) > chance) {
        skill_add_uses(16, 1);
        inventory_close();
        crime_current = 13;
        guards_summon(1);
        hud_message_add(D_001771FF);
        return;
    }
    hud_message_add(D_00177217);
    object_foreach(player_entity->children, inv_claim_item);
    for (chance = 0; chance < 27; chance++) {
        if (player_character->equipped[chance] != 0) {
            inv_claim_item((int)player_character->equipped[chance]);
            inv_store_item(player_character->equipped[chance]);
        }
    }
    inventory_close();
}

void inv_claim_items(void)
{
    int slot;

    object_foreach(player_entity->children, inv_claim_item);
    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] != 0) {
            inv_claim_item((int)player_character->equipped[slot]);
        }
    }
}

void trade_buy_button(void)
{
    if (trade_total == 0) return;
    trade_make_offer();
}

void trade_clear_button(void)
{
    struct record *object;
    struct record *next;

    switch (trade_mode) {
        return;
    case 1:
        object_foreach(player_entity->children, inv_return_unpaid_item);
        return;
    case 2:
    case 3:
    case 4:
        object = inv_right_container->children;
        while (object != 0) {
            next = object->next;
            if (object->type == 54) object->type = 2;
            inv_store_item(object);
            object = next;
        }
    default:;
    }
}

void trade_sell_button(void)
{
    if (trade_total == 0) return;
    trade_make_offer();
}

void trade_repair_button(void)
{
    if (trade_total == 0) return;
    trade_make_offer();
}

void trade_identify_button(void)
{
    struct record *object;
    struct record *next;
    int identified;
    int count;

    identified = 0;
    count = 0;
    if (current_building->type != 11 || current_building->faction_id != 40) {
        if (trade_pay_spell_points() == 0) {
            msgbox_show_string((int)D_0017722B, 1);
            return;
        }
        object = inv_right_container->children;
        while (object != 0) {
            if (trade_pay_spell_points() != 0) {
                if (player_character->lock_open_chance >= rand_range(1, 100)) {
                    identified++;
                    object->data.item.item_flags |= 32;
                }
            }
            count++;
            object = object->next;
        }
        mc_set_location(2642, (int)D_0017704C);
        mc_sprintf((int)text_buffer, key_names[0], identified, count);
        msgbox_show_string((int)text_buffer, 1);
        object = inv_right_container->children;
        while (object != 0) {
            next = object->next;
            if (((int)(unsigned short)(object->data.item.item_flags & 32)) != 0) inv_store_item(object);
            object = next;
        }
        return;
    }
    trade_make_offer();
}

void inv_toggle_hidden(void)
{
    int capacity;
    struct record *object;
    struct item *item;

    if (inv_selected_item == 0) return;
    object = inv_selected_item;
    item = &inv_selected_item->data.item;
    if (((int)(unsigned short)(item->item_flags & 64)) != 0) {
        item->item_flags &= ~0x40;
        return;
    }
    if (xn_str_find_u32(player_character->equipped, (int)object, 27) != 0) {
        msgbox_show_string((int)D_00177255, 1);
        return;
    }
    capacity = equip_hiding_capacity(1);
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity, inv_sum_hidden_weight);
    if ((object_weight(object) + *(int *)D_00195B84) > capacity) {
        msgbox_show_string((int)D_0017727F, 1);
        return;
    }
    sound_play(235, player_object, 100);
    item->item_flags |= 64;
}

int trade_pay_spell_points(void)
{
    if (player_character->magicka < *(int *)D_001AA458) return 0;
    player_character->magicka -= *(short *)D_001AA458;
    return 1;
}

void trade_total_identify(void)
{
    struct record *object;
    int unused;

    trade_total = 0;
    object = inv_right_container->children;
    while (object != 0) {
        trade_total += (object->data.item.value * 25) >> 8;
        object = object->next;
    }
    trade_price = trade_total;
}

void func_00098A15(void)
{
    int location;

    if (((unsigned)(((unsigned)location_object->id) >> 16)) < 1000) {
        map_goto_location(D_001AA540, D_001AA544, D_001AA580, 0);
        mc_memcpy((int)player_object, (int)saved_player_object, 55, (int)D_0017704C, 2917, 4);
        camera_object->yaw = player_object->yaw;
        return;
    }
    mc_memcpy((int)saved_player_object, (int)player_object, 55, (int)D_0017704C, 2922, 4);
    D_001AA540 = (int)(unsigned char)current_region;
    D_001AA544 = (int)player_environment;
    D_001AA580 = location_object->image;
    if ((((unsigned)player_character->ship_owned) >> 16) == 992) {
        location = 1;
    } else {
        location = 2;
    }
    map_goto_location(31, 1, location, 0);
    player_to_nearest_marker(location_object, 8);
}

int func_00098B20(void)
{
    int on_992;
    int on_993;
    int result;

    if ((((unsigned)location_object->id) >> 16) == 992) {
        on_992 = 1;
    } else {
        on_992 = 0;
    }
    result = on_992;
    if ((((unsigned)location_object->id) >> 16) == 993) {
        on_993 = 1;
    } else {
        on_993 = 0;
    }
    result += on_993;
    return result;
}

int func_00098B91(struct record *object)
{
    if (object->twin != 0 && object->quest_id != 0) {
        current_quest = (struct quest *)quest_find_by_id((int)(short)((unsigned short)object->quest_id));
    } else {
        return 0;
    }
    return 1;
}

void inv_read_map_scrap(struct record *scrap)
{
    {
        struct loaded_location found;

        if (scrap != 0) object_delete(scrap);
        text_macro_map_location = (int)&found;
        location_pick_random_undiscovered(&found);
        msgbox_show_rsc(499, 1);
        location_set_discovered(found.object->location_index, 1);
        location_free(&found);
    }
}

int item_forbidden_for_class(struct item *item)
{
    if ((player_class->forbidden_materials != 0 || player_class->forbidden_equipment != 0) && (item->group == 3 || item->group == 2)) {
        if (item->group == 2 && item->index >= 7 && item->index <= 10) {
            if ((player_class->forbidden_equipment & ((1 << (item->index - 7)) << 9)) != 0) {
                msgbox_show_rsc(1068, 1);
                return 1;
            }
        }
        if (item->group == 2 && item->index < 7 && (player_class->forbidden_equipment & ((1 << item->armor_type) << 6)) != 0) {
            msgbox_show_rsc(1068, 1);
            return 1;
        }
        if (item->group == 3 && (player_class->forbidden_equipment & ((int)(short)*(short *)(weapon_proficiency_bits + (item->index * 2)))) != 0) {
            msgbox_show_rsc(1068, 1);
            return 1;
        }
        if (item->group == 3 && (player_class->forbidden_materials & (1 << item->material)) != 0) {
            msgbox_show_rsc(1068, 1);
            return 1;
        }
        if (item->armor_type == 2 && (player_class->forbidden_materials & (1 << item->material)) != 0) {
            msgbox_show_rsc(1068, 1);
            return 1;
        }
    }
    return 0;
}

void func_00098F1D(struct record *object)
{
    int unused;

    if (object->twin == 0) return;
    object->twin->id = object->id;
}

void inv_close_assign_ids(void)
{
    object_foreach(player_entity->children, inv_assign_item_id);
}

void inv_track_hand_weapons(int closing)
{
    int right_delay;
    int left_delay;

    right_delay = 0;
    left_delay = 0;
    if (closing == 0) {
        D_001AA44C = 0;
        D_001AA450 = 0;
        if (player_character->equipped[19] != 0) {
            D_001AA448 = (int)(unsigned short)*(short *)((char *)(D_001AA44C = (int)player_character->equipped[19]) + 105);
        }
        if (player_character->equipped[21] != 0) {
            D_001AA444 = (int)(unsigned short)*(short *)((char *)(D_001AA450 = (int)player_character->equipped[21]) + 105);
        }
    } else {
        if (D_001AA44C != (int)player_character->equipped[19]) {
            if (D_001AA44C != 0) right_delay += *(int *)(D_0017887F + (D_001AA448 << 2));
            if (player_character->equipped[19] != 0) {
                right_delay += *(int *)(D_0017887F + (player_character->equipped[19]->data.item.index << 2));
            }
        }
        if (D_001AA450 != (int)player_character->equipped[21]) {
            if (D_001AA450 != 0) left_delay += *(int *)(D_0017887F + (D_001AA444 << 2));
            if (player_character->equipped[21] != 0) {
                left_delay += *(int *)(D_0017887F + (player_character->equipped[21]->data.item.index << 2));
            }
        }
    }
    D_0019597C[0] += right_delay;
    left_hand_ready_delay += left_delay;
}

void item_refresh_magic_value_cb(struct record *object)
{
    struct record *parent;
    struct item *item;

    if (object->type != 2) return;
    parent = object->parent;
    while (parent->type != 52) {
        if (parent->type == 1) return;
        parent = parent->parent;
    }
    if (parent->container_index > 4) return;
    item = &object->data.item;
    if (item->enchantments[0].type == (-1)) return;
    item->value = enchant_item_value(item);
}

void inv_refresh_magic_values(void)
{
    object_foreach(player_entity->children, item_refresh_magic_value_cb);
}

int trade_can_repair_item(struct item *item)
{
    if (inv_right_container->children != 0 && current_building->type == 11) {
        msgbox_show_string((int)D_00177300, 1);
        return 0;
    }
    if (inv_selected_item->children != 0) {
        msgbox_show_string((int)D_00177323, 1);
        return 0;
    }
    if (item->group == 3 && item->index == 18) {
        msgbox_show_rsc(24, 1);
        return 0;
    }
    if (item->condition == item->max_condition) {
        msgbox_show_rsc(24, 1);
        return 0;
    }
    return 1;
}

void trade_schedule_repair(void)
{
    struct item *item;

    if (trade_mode != 3) return;
    item = &inv_selected_item->data.item;
    if (current_building->type == 11) {
        inv_selected_item->repair_due = (((((item->max_condition - item->condition) / ((int)&*(signed char *)((char *)(guild_membership->rank) + 1))) * 1440) / 144000) + 1440) + game_minutes;
        return;
    }
    trade_schedule_shop_repairs();
}

void trade_mark_in_repair(void)
{
    struct record *object;

    object = inv_right_container->children;
    while (object != 0) {
        object->type = 54;
        object = object->next;
    }
}

int potion_recipe_text(signed char *recipe)
{
    int i;
    char *text;

    i = 0;
    text = *(char **)scratch_buffer + 55000;
    *text = 0;
    while (recipe[i] != (-2) && i < 8) {
        func_000A1054((int)text, (int)item_templates[((int)(short)*(short *)((char *)(int)(*(char **)(item_group_templates + (recipe[i + 10] << 2)) + (recipe[i] * 2))))].name, (int)D_0017704C, 3200, 4);
        func_000A1054((int)text, (int)D_00177346, (int)D_0017704C, 3201, 4);
        i++;
    }
    *(strlen(text) + text + 1) = 0;
    return (int)text;
}
