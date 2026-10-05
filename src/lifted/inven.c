/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
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
extern char item_templates[];
extern char potion_recipes[];
extern int D_001832A4;
extern int D_00184221;
extern int key_names[];
extern short spell_last_cast_id;
extern char item_group_templates[];
extern signed char D_00187CA8;
extern signed char D_00187DAC[];
extern signed char item_group_tab[];
extern char inv_mode_buttons[];
extern char D_00188283[];
extern char D_00188285[];
extern char D_00188287[];
extern char D_00188289[];
extern char inv_buttons[];
extern char D_00188427[];
extern char D_00188429[];
extern char D_0018842B[];
extern char D_0018842D[];
extern char weapon_proficiency_bits[];
extern char region_price_adjustment[];
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
extern struct record *D_00195AA8;
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
extern char D_00195DA8[];
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

extern int damage_apply(struct record *, int, int);
extern int sheet_open(int);
extern int spellbook_open(int);
extern int key_action_held(int);
extern int macro_kg_weight(void);
extern int object_weight(struct record *);
extern int holiday_today(int, int);
extern int quest_find_by_id(int);
extern int enchant_item_value(int);
extern int cast_item_used_spell(short);
extern int spell_find_on_entity(int, short, int);
extern int equip_hiding_capacity(int);
extern struct record *monster_summon_near_player(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int gold_can_afford(int);
extern int carry_capacity(void);
extern int object_free_single(struct record *);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern int object_find(struct record *, int);
extern int object_new_id(int);
extern int inv_match_arrows(int);
extern int inv_draw_item_cell(struct record *, int, int);
extern int trade_shop_takes_group(unsigned short);
extern int player_to_nearest_marker(int, int);
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
extern void msgbox_show_qrc_text(char *, unsigned short, int);
extern void msgbox_show_rsc(int, int);
extern void guards_summon(int);
extern void parse_expand(int, int);
extern void quest_raise_event(int, int, int);
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
extern void size_fit(int, int, short, short);
extern void object_free_pending(void);
extern void msgbox_yes_no_rsc(int);
extern void gold_add(int);
extern void gold_spend(int);
extern void location_free(int);
extern void map_goto_location(int, int, int, int);
extern void location_pick_random_undiscovered(int);
extern void location_set_discovered(int, int);
extern void spell_end(int);
extern void inpstr_begin_number(int);
extern void object_free_children(struct record *);
extern void object_foreach_pre(struct record *, int);
extern void object_foreach(struct record *, int);
extern void potion_drink(struct record *);
extern void inv_sum_hidden_weight(int);
extern void trade_add_buy_price(int);
extern void trade_add_repair_cost(int);
extern void inv_return_unpaid_item(int);
extern void inv_store_cb(int);
extern void inv_claim_item(int);
extern void inv_assign_item_id(int);
extern void inventory_draw(void);
extern void inv_select_tab(unsigned char);
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
int potion_recipe_text(int);
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
void item_refresh_magic_value_cb(int);
void inv_refresh_magic_values(void);
void trade_mark_in_repair(void);
#pragma aux mc_set_location parm routine [];

void inventory_load_images(void)
{
    inventory_images = disk_read_file((int)D_00176FE4, 0);
    D_001AA420 = disk_read_file((int)D_00176FF1, 0);
    D_001AA424 = disk_read_file((int)D_00176FFE, 0);
    D_001AA428 = disk_read_file((int)D_0017700B, 0);
    D_001AA42C = disk_read_file((int)D_00177018, 0);
    D_001AA430 = disk_read_file((int)D_00177025, 0);
    D_001AA43C = disk_read_file((int)D_00177032, 0);
    D_001AA440 = disk_read_file((int)D_0017703F, 0);
    if (trade_mode == 0) return;
    mc_set_location(313, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_00177054, (trade_mode * 2) + 6);
    D_001AA434 = disk_read_file((int)text_buffer, 0);
    mc_set_location(315, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_00177054, (trade_mode * 2) + 7);
    D_001AA438 = disk_read_file((int)text_buffer, 0);
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

int inventory_open(int a1, int a2, int a3)
{
    if (player_death_timer > 0) return 0;
    if (((int)D_0019626F) == 4 && ((int)(unsigned char)game_mode) == 8) {
        return 1;
    }
    if (a1 != 0 || (game_mode == 0 && key_action_held(37) != 0 && player_death_timer == 0)) {
        inv_temp_pile = 0;
        if (player_character->race > 8) {
            msgbox_show_string((int)D_00177063, 1);
            return 0;
        }
        if (a1 == 0) a1 = 1;
        if (a2 == 0 && a1 == 1) {
            inv_temp_pile = (inv_right_container_base = (inv_right_container = object_create_child(player_object->parent, 0, 0)));
            inv_right_container->type = 33;
            inv_right_container->image = ((unsigned short)(unsigned char)D_00187DAC[rand() % 20]) + 27648;
            inv_right_container->pad19 = 1;
            inv_right_container->flags |= 5;
            inv_right_container->x = player_object->x;
            inv_right_container->y = player_object->y;
            inv_right_container->z = player_object->z;
            inv_right_container->id = object_new_id(((unsigned)location_object->id) >> 16);
            a1 = 2;
        }
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        D_00187CA8 = 0;
        game_mode = 4;
        D_00196272 = 1;
        trade_mode = a2;
        D_001AA5F8 = (inv_right_icon = *(signed char *)&a3);
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
    int l_20;
    int l_1C;
    int l_18;

    if (inventory_open(0, 0, 2) == 0) return;
    xn_gfx_wait_vretrace_start();
    xn_gfx_wait_vretrace_end();
    xn_tex_cache_begin_frame();
    inventory_draw();
    l_1C = trade_settle_offer();
    if (l_1C > 0) {
        if (trade_mode != 2 && gold_can_afford(l_1C) == 0) {
            msgbox_show_rsc(454, 1);
        } else if (trade_mode != 2) {
            l_18 = holiday_today(game_minutes, (int)(unsigned char)current_region);
            if (trade_mode == 4) {
                if (l_18 != 43 && ((struct bf8_7_1 *)&player_motion_flags)->f == 0) {
                    gold_spend(l_1C);
                }
            } else {
                gold_spend(l_1C);
            }
            if (trade_mode == 4) {
                trade_mark_identified();
            } else if (trade_mode == 1) {
                inv_claim_items();
            } else if (trade_mode == 3) {
                trade_mark_in_repair();
            }
        } else {
            gold_add(l_1C);
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
    for (l_20 = 0; l_20 < 7; l_20++) {
        if (mouse_x > *(short *)(inv_mode_buttons + ((trade_mode * 84) + (l_20 * 12))) && mouse_x < *(short *)(D_00188285 + ((trade_mode * 84) + (l_20 * 12))) && mouse_y > *(short *)(D_00188283 + ((trade_mode * 84) + (l_20 * 12))) && mouse_y < *(short *)(D_00188287 + ((trade_mode * 84) + (l_20 * 12)))) {
            sound_play(203, player_object, 100);
            ((int (*)())(*(int *)(D_00188289 + ((trade_mode * 84) + (l_20 * 12)))))(l_20, 27);
            break;
        }
    }
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (l_20 = 0; l_20 < 45; l_20++) {
        if (mouse_x > *(short *)(inv_buttons + (l_20 * 12)) && mouse_x < *(short *)(D_00188429 + (l_20 * 12)) && mouse_y > *(short *)(D_00188427 + (l_20 * 12)) && mouse_y < *(short *)(D_0018842B + (l_20 * 12))) {
            sound_play(203, player_object, 100);
            ((int (*)())(*(int *)(D_0018842D + (l_20 * 12))))(l_20, 27);
            return;
        }
    }
}

void inventory_close(void)
{
    struct record *l_1C;
    struct record *l_18;

    if (inv_left_container == wagon_container && trade_mode == 1) inv_select_tab(41);
    if (inv_right_container == wagon_container) inv_wagon_button();
    while (key_down_esc != 0);
    if (trade_mode == 0) inv_claim_items();
    if (trade_mode == 2 || trade_mode == 4 || trade_mode == 3) {
        l_1C = inv_right_container_base->children;
        while (l_1C != 0 && l_1C->type == 2) {
            l_18 = l_1C->next;
            if (((int)(unsigned short)(l_1C->flags & 32)) == 0) inv_store_item(l_1C);
            l_1C = l_18;
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
    if (((int)(unsigned char)*(signed char *)(*(char **)D_00195DA8)) == 33 && *(int *)(*(char **)D_00195DA8 + 63) == 0) {
        *(signed char *)(*(char **)D_00195DA8 + 22) |= 2;
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
        parse_expand((int)D_00177099, (int)D_001913E4);
        hud_message_add((int)D_001913E4);
    }
    if (left_hand_ready_delay > 0 && player_character->equipped[21] != 0) {
        text_macro_item = &player_character->equipped[21]->data.item;
        parse_expand((int)D_00177099, (int)D_001913E4);
        hud_message_add((int)D_001913E4);
    }
    if (((struct bf8_5_1 *)&D_001940D8)->f != 0) {
        D_001940D8 &= 223;
        sheet_open(1);
    }
    quests_suspended = 0;
}

void inv_draw_container_icon(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_20 = D_001AA440;
    l_1C = 0;
    if (a2 == 1 && ((int)(unsigned short)(game_settings->view_flags & 4)) != 0) a2 = 10;
    while (l_1C < a2) {
        l_20 = (((int)(unsigned short)*(short *)((char *)l_20 + 10)) + l_20) + 12;
        l_1C++;
    }
    l_18 = ((int)(short)*(short *)(inv_buttons + (a1 * 12))) + ((((int)&*(signed char *)((char *)(((int)(short)*(short *)(D_00188429 + (a1 * 12))) - ((int)(short)*(short *)(inv_buttons + (a1 * 12)))) + 1)) - ((int)(unsigned short)*(short *)((char *)l_20 + 4))) >> 1);
    l_14 = ((int)(short)*(short *)(D_00188427 + (a1 * 12))) + ((((((int)(short)*(short *)(D_0018842B + (a1 * 12))) - ((int)(short)*(short *)(D_00188427 + (a1 * 12)))) + 1) - ((int)(unsigned short)*(short *)((char *)l_20 + 6))) >> 1);
    xn_draw_image_transparent(l_18, l_14, (int)(unsigned short)*(short *)((char *)l_20 + 4), (int)(unsigned short)*(short *)((char *)l_20 + 6), l_20 + 12);
    if (wagon_container == 0 || a2 != 3) return;
    D_001962AE = 1;
    D_00195AA8 = wagon_container;
    mc_set_location(657, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_001770A7, macro_kg_weight());
    text_draw_coloured((int)text_buffer, (int)(short)(l_18 + 1), (int)(short)(l_14 + 1), 145, 156);
    D_001962AE = 0;
}

void func_00093BD9(int a1, int a2, int a3)
{
    int l_24;
    short l_20;
    short l_1C;
    short l_18;
    short l_14;
    short l_10;

    *(int *)&l_20 = (((int)(short)*(short *)((char *)((a3 * 12) + a2))) + ((int)(short)*(short *)((char *)((a3 * 12) + a2) + 4))) >> 1;
    *(int *)&l_1C = (((int)(short)*(short *)((char *)((a3 * 12) + a2) + 2)) + ((int)(short)*(short *)((char *)((a3 * 12) + a2) + 6))) >> 1;
    l_24 = D_001AA440;
    *(int *)&l_10 = 0;
    while (((int)(short)l_10) < 3) {
        l_24 = (((int)(unsigned short)*(short *)((char *)l_24 + 10)) + l_24) + 12;
        (*(int *)&l_10)++;
    }
    l_18 = *(short *)((char *)l_24 + 4);
    l_14 = *(short *)((char *)l_24 + 6);
    size_fit((int)&l_18, (int)&l_14, (int)(short)((*(short *)((char *)((a3 * 12) + a2) + 4) - *(short *)((char *)((a3 * 12) + a2))) - 4), (int)(short)((*(short *)((char *)((a3 * 12) + a2) + 6) - *(short *)((char *)((a3 * 12) + a2) + 2)) - 4));
    for (a3 = 0; ((int)(unsigned short)*(short *)((char *)l_24 + 6)) > a3; a3++) {
        mc_memcpy((int)(*(char **)scratch_buffer + (a3 << 8)), (l_24 + 12) + (((int)(unsigned short)*(short *)((char *)l_24 + 4)) * a3), (int)(unsigned short)*(short *)((char *)l_24 + 4), (int)D_0017704C, 787, 4);
    }
    xn_draw_image_scaled(((int)(short)l_20) - (((int)(short)l_18) >> 1), ((int)(short)l_1C) - (((int)(short)l_14) >> 1), (int)(short)l_18, (int)(short)l_14, (int)(unsigned short)*(short *)((char *)l_24 + 4), (int)(unsigned short)*(short *)((char *)l_24 + 6), 0, *(int *)scratch_buffer);
    mc_set_location(791, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_001770C0, macro_kg_weight());
    text_draw_coloured((int)text_buffer, (int)(short)(*(short *)((char *)((a3 * 12) + a2)) + 3), (int)(short)(*(short *)((char *)((a3 * 12) + a2) + 2) + 2), 145, 156);
}

void inv_draw_cell_mark(int a1, int a2, int a3, int a4)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;
    int l_C;

    l_20 = xn_tex_cache_lookup(a1, a2, -1);
    if (l_20 == 0) {
        xn_tex_cache_flush();
        l_20 = xn_tex_cache_lookup(a1, a2, -1);
    }
    l_1C = *(int *)((char *)l_20 + 12);
    l_18 = (((int)(short)*(short *)((char *)((a4 * 12) + a3))) + ((int)(short)*(short *)((char *)((a4 * 12) + a3) + 4))) >> 1;
    l_14 = (((int)(short)*(short *)((char *)((a4 * 12) + a3) + 2)) + ((int)(short)*(short *)((char *)((a4 * 12) + a3) + 6))) >> 1;
    l_10 = (int)(unsigned short)*(short *)((char *)l_1C + 4);
    l_C = (int)(unsigned short)*(short *)((char *)l_1C + 6);
    xn_draw_image_scaled(l_18 - (l_10 >> 1), l_14 - (l_C >> 1), l_10, l_C, (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), (int)(unsigned short)(*(short *)((char *)l_1C + 8) | 32768), l_1C + *(int *)((char *)l_1C + 14));
}

void inv_scroll_left_up(void)
{
    if (*(int *)inv_left_scroll == 0) return;
    (*(int *)inv_left_scroll)--;
}

void inv_click_list_row(int a1, int a2)
{
    a1 -= a2;
    switch ((unsigned)a1) {
    case 0:
        a1 = 4;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        a1--;
        break;
    case 5:
        a1 = 9;
        break;
    case 6:
    case 7:
    case 8:
    case 9:
        a1--;
    }
    if (a1 < 5 && *(int *)(inv_left_rows + (a1 << 2)) != 0) {
        inv_click_left_item(*(int *)(inv_left_rows + (a1 << 2)));
        return;
    }
    if (a1 <= 4 || a1 >= 10 || D_001AA534[a1] == 0) return;
    inv_click_right_item(D_001AA534[a1]);
}

int inv_take_item(struct record *a1)
{
    struct item *l_30;
    int l_2C;
    struct record **l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_30 = &a1->data.item;
    if (a1->parent->parent != player_entity) {
        if (inv_left_container != wagon_container) {
            if (l_30->group != 23) {
                l_24 = object_weight(a1);
                l_20 = object_weight(player_entity);
                l_1C = carry_capacity() << 2;
                if ((l_24 + l_20) > l_1C) {
                    msgbox_show_string((int)D_001770CA, 1);
                    return 0;
                }
            }
        } else {
            l_24 = object_weight(a1);
            D_001962AE = 1;
            l_20 = object_weight(wagon_container);
            D_001962AE = 0;
            if ((l_24 + l_20) > 3000) {
                msgbox_show_string((int)D_001770EA, 1);
                return 0;
            }
        }
    }
    if (l_30->group == 23 && l_30->index == 0) {
        inv_store_item(a1);
        return 0;
    }
    if (l_30->group == 27 && l_30->index == 8) {
        inv_read_map_scrap(inv_selected_item);
        return 0;
    }
    if (a1->type == 54) a1->type = 2;
    if (l_30->group == 3 && l_30->index == 18) {
        l_1C = l_30->stack_count;
        inv_merge_arrows(player_entity, a1, 1);
        found_object->data.item.condition = l_1C;
        return 0;
    }
    l_28 = (struct record **)xn_str_find_u32(player_character->equipped, (int)inv_selected_item, 27);
    if (l_28 != 0) {
        a1 = *l_28;
        item_remove_equip_effects(a1, ((int)l_28 - (int)player_character->equipped) / 4);
        *l_28 = 0;
        return 0;
    }
    D_001AA454 = 0;
    a1->id = object_new_id(100);
    if (a1->twin != 0) a1->twin->id = a1->id;
    func_00098F1D(a1);
    l_30 = &a1->data.item;
    if (l_30->group == 28 && l_30->index == 0) {
        if (a1 != inv_right_container) D_001AA454 += l_30->value;
        object_free_single(a1);
    } else {
        a1->caster = 0;
        quest_raise_event(3, (int)a1, 0);
        a1->x = player_object->x;
        a1->y = player_object->y;
        a1->z = player_object->z;
        if (inv_left_container != wagon_container) {
            inv_store_item(a1);
            return 1;
        }
        object_reparent(inv_left_container, a1);
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
    int l_18;

    l_18 = equip_hiding_capacity(1);
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity, (int)inv_sum_hidden_weight);
    player_character->hidden_load_percent = (*(int *)D_00195B84 * 100) / l_18;
}

void inv_unequip_item(struct record *a1)
{
    int l_18;

    for (l_18 = 0; l_18 < 27; l_18++) {
        if (player_character->equipped[l_18] == a1) {
            item_remove_equip_effects(a1, l_18);
            player_character->equipped[l_18] = 0;
            return;
        }
    }
}

void inv_unequip_all_saved(void)
{
    struct item *l_1C;
    int l_18;

    mc_memset((int)D_001AA4CC, 0, 108, (int)D_0017704C, 1274, 108);
    mc_memset((int)D_001AA460, 0, 108, (int)D_0017704C, 1275, 108);
    for (l_18 = 0; l_18 < 27; l_18++) {
        if (player_character->equipped[l_18] != 0) {
            l_1C = &player_character->equipped[l_18]->data.item;
            if (l_1C->enchantments[0].type != (-1)) {
                D_001AA4CC[l_18] = (int)player_character->equipped[l_18];
                *(int *)(D_001AA460 + (l_18 << 2)) = l_1C->condition;
                l_1C->condition = l_1C->max_condition;
                inv_unequip_item(player_character->equipped[l_18]);
            }
        }
    }
}

void inv_reequip_saved(void)
{
    struct item *l_1C;
    int l_18;

    for (l_18 = 0; l_18 < 27; l_18++) {
        if (D_001AA4CC[l_18] != 0) {
            inv_equip_item(D_001AA4CC[l_18]);
            l_1C = (struct item *)(D_001AA4CC[l_18] + 71);
            l_1C->condition = *(short *)(D_001AA460 + (l_18 << 2));
        }
    }
}

void inv_use_item(void)
{
    struct item *l_30;
    int l_2C;
    struct record *l_28;
    struct record *l_24;
    int l_20;
    int l_1C;
    short l_18;

    l_1C = 0;
    l_28 = inv_selected_item;
    l_30 = &inv_selected_item->data.item;
    if (((int)(unsigned short)(l_28->flags & 32)) != 0) {
        msgbox_show_string((int)D_0017710F, 1);
        return;
    }
    l_30->item_flags |= 0x200;
    if (l_28->twin != 0) l_28->twin->data.item.item_flags |= 0x200;
    D_001940D8 |= 8;
    if (l_30->enchantments[0].type == 26 && l_30->enchantments[0].param == 3) {
        cast_item_used_spell(92);
        item_damage(inv_selected_item, 50);
        return;
    }
    if (l_30->enchantments[0].type == 26 && l_30->enchantments[0].param == 4) {
        if (creature_count == 0) {
            hud_message_add((int)D_00177129);
            return;
        }
        D_0019629D = 1;
        l_24 = monster_summon_near_player(27);
        D_0019629D = 0;
        if (l_24 == 0) {
            hud_message_add((int)D_00177147);
            return;
        }
        l_24->data.character.flags |= 2;
        item_damage(inv_selected_item, 100);
        return;
    }
    if (l_30->enchantments[0].type == 26 && l_30->enchantments[0].param == 8) {
        if (creature_count == 0) {
            hud_message_add((int)D_0017716F);
            return;
        }
        D_0019629D = 1;
        l_24 = monster_summon_near_player(creature_list[0]->data.character.mobile_id);
        D_0019629D = 0;
        if (l_24 == 0) {
            hud_message_add((int)D_0017718D);
            return;
        }
        l_24->data.character.flags |= 2;
        item_damage(inv_selected_item, 100);
        return;
    }
    if (l_30->enchantments[0].type == 26 && l_30->enchantments[0].param == 5) {
        sheet_open(50);
        object_delete(inv_selected_item);
        return;
    }
    if (l_30->enchantments[0].type == 26 && l_30->enchantments[0].param == 9) {
        if (inv_selected_item->children != 0) {
            object_delete(inv_selected_item->children);
            msgbox_show_rsc(32, 1);
        } else {
            msgbox_show_rsc(20, 1);
        }
        return;
    }
    if (l_30->enchantments[0].type != (-1)) {
        l_20 = 0;
        while (l_20 < 10 && l_30->enchantments[l_20].type != (-1)) {
            if (l_30->enchantments[l_20].type == 0) {
                *(int *)&l_18 = (int)(short)spell_last_cast_id;
                D_0019629A = 1;
                cast_item_used_spell(l_30->enchantments[l_20].param);
                D_0019629A = 1;
                l_1C = 1;
                spell_last_cast_id = *(int *)&l_18;
                item_damage(inv_selected_item, 10);
            }
            if (l_30->enchantments[l_20].type == 21 && l_30->enchantments[l_20].param == 0) {
                damage_apply(player_entity, (int)&*(signed char *)((char *)(player_character->level >> 1) + 1), 0);
                item_damage(inv_selected_item, 2);
            }
            l_20++;
        }
    }
    if (l_30->group == 9 && l_30->index == 5 && (short)l_30->message != 0) {
        if (func_00098B91(inv_selected_item) != 0) {
            msgbox_show_quest_text(current_quest, (int)(short)(short)l_30->message, 1);
        } else {
            msgbox_show_qrc_text(l_30->name + 10, (int)(unsigned short)(short)l_30->message, 1);
        }
        return;
    }
    if (l_30->group == 0) {
        poison_apply(player_entity, l_30->index + 136, 1);
        object_delete(l_28);
        return;
    }
    if (l_30->group == 27 && l_30->index == 0) {
        if (l_28->children == 0) {
            msgbox_show_rsc(12, 1);
        } else {
            while (mouse_buttons != 0) xn_mouse_poll_clamped();
            inventory_close();
            spellbook_open(1);
            D_001940D8 |= 128;
        }
        return;
    }
    if (l_30->group == 7) {
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        inventory_close();
        book_open((int)(short)(short)l_30->message);
        D_001940D8 |= 128;
        return;
    }
    if (l_30->group == 6 || l_30->group == 12) {
        item_next_clothing_style(l_30);
        return;
    }
    if (l_30->group == 1 && l_30->index == 1 && l_28->children != 0 && l_28->children->type == 31) {
        potion_drink(l_28->children);
        object_delete(l_28);
        return;
    }
    if (l_1C != 0 || l_28->quest_id != 0) return;
    D_00195F2E = 30;
    D_0012B508 = 146;
    msgbox_show_string(D_00184221, 1);
}

void inv_item_info(struct record *a1, struct item *a2)
{
    int l_14;

    D_0012B508 = 146;
    D_00195AA8 = a1;
    text_macro_item = a2;
    if (a2->enchantments[0].type == 26 && a2->enchantments[0].param == 9) {
        msgbox_show_rsc(1004, 1);
    } else if (a2->group == 27 && a2->index == 4) {
        D_00195ACC = ((int)potion_recipes) + (a2->stack_count * 109);
        msgbox_show_string((int)D_001771B5, 1);
        msgbox_show_string(potion_recipe_text(D_00195ACC), 1);
    } else if (a2->group == 27 && a2->index == 6) {
        msgbox_show_rsc(1073, 1);
    } else if (a2->group == 13) {
        item_info_painting(a2);
    } else if (a2->group == 7) {
        if (a2->enchantments[0].type == 26) {
            msgbox_show_rsc(1015, 1);
        } else {
            l_14 = *(int *)scratch_buffer + 63000;
            book_read_header(l_14, (int)(unsigned short)(short)a2->message);
            text_macro_book = l_14;
            msgbox_show_rsc(1009, 1);
        }
    } else if (a2->group == 2) {
        if (((int)(unsigned short)(text_macro_item->item_flags & 2048)) != 0) {
            msgbox_show_rsc(1014, 1);
        } else {
            msgbox_show_rsc(1000, 1);
        }
    } else if (a2->group == 3) {
        if (a1->children != 0) {
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
    } else if (a2->group == 1 && a1->children != 0 && a1->children->type == 11) {
        msgbox_show_rsc(1006, 1);
    } else if (a2->group == 1 && a2->index == 1 && a1->children != 0 && a1->children->type == 31) {
        D_00195ACC = (int)a1->children + 71;
        msgbox_show_rsc(1008, 1);
    } else if (a2->group == 27 && a2->index == 1) {
        msgbox_show_rsc(1004, 1);
    } else if (a2->group == 27 && a2->index == 2) {
        msgbox_show_rsc(1007, 1);
    } else {
        msgbox_show_rsc(1003, 1);
    }
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    mouse_buttons_prev = 0;
    if (a2->enchantments[0].type != (-1)) msgbox_show_rsc(1016, 1);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    mouse_buttons_prev = 0;
}

void inv_list_left_item(struct record *a1, int a2)
{
    struct item *l_14;

    if ((a1->type != 50 && a1->type != 2) || ((int)(unsigned short)(a1->flags & 2)) != 0) return;
    l_14 = &a1->data.item;
    if (guild_membership != 0 && guild_membership->kind == 3) {
        if (trade_mode == 2 && l_14->enchantments[0].type == (-1)) return;
        if (((struct bf8_2_1 *)&D_001940D8)->f == 0 && xn_str_find_u32((int)player_character + 367, a1, 27) != 0 && l_14->group != 1) {
            return;
        }
    } else {
        if (trade_mode == 2 && trade_shop_takes_group(l_14->group) == 0) return;
        if (((struct bf8_1_1 *)&D_001940D4)->f != 0 && l_14->condition == l_14->max_condition) {
            return;
        }
        if (((struct bf8_1_1 *)&D_001940D4)->f != 0 && l_14->enchantments[0].type != (-1)) return;
        if ((((int)(unsigned char)game_mode) == 10 || (((int)D_0019626F) == 10 && ((int)(unsigned char)game_mode) == 8)) && (l_14->enchantments[0].type != (-1) || l_14->enchant_points == 0 || (int)itemmaker_item_object == a1 || (l_14->group == 3 && l_14->index == 18) || (l_14->group == 27 && l_14->index == 1) || l_14->group == 23)) {
            return;
        }
        if (((struct bf8_2_1 *)&D_001940D8)->f == 0 && xn_str_find_u32((int)player_character + 367, a1, 27) != 0 && l_14->group != 1) {
            return;
        }
    }
    if (((int)(short)D_001AA586) >= *(int *)inv_left_scroll && ((int)(short)D_001AA586) < (*(int *)inv_left_scroll + 4)) {
        *(int *)(inv_left_rows + ((((int)(short)D_001AA586) - *(int *)inv_left_scroll) << 2)) = (int)a1;
        if (a1->type != 50) {
            inv_draw_item_cell(a1, (int)(short)(D_001AA586 - *(short *)inv_left_scroll), a2);
        }
    }
    if (((int)(unsigned char)game_mode) == 10) {
        if (l_14->enchantments[0].type != (-1)) return;
    }
    D_001AA586++;
}

void inv_equip_in_slot_pair(struct record *a1, int a2, int a3)
{
    if (player_character->equipped[a2] != 0) {
        if (player_character->equipped[a2 + a3] != 0) {
            inv_equip_in_slot(a1, a2);
        } else {
            inv_equip_in_slot(a1, a2 + a3);
        }
        return;
    }
    inv_equip_in_slot(a1, a2);
}

void inv_unequip_slot(int a1)
{
    item_remove_equip_effects(player_character->equipped[a1], a1);
    player_character->equipped[a1] = 0;
}

void inv_equip_in_slot(struct record *a1, int a2)
{
    if (player_character->equipped[a2] != 0) {
        item_remove_equip_effects(player_character->equipped[a2], a2);
        player_character->equipped[a2] = a1;
        quest_raise_event(3, (int)a1, 0);
        item_apply_equip_effects(a1, a2);
        return;
    }
    player_character->equipped[a2] = a1;
    quest_raise_event(3, (int)a1, 0);
    item_apply_equip_effects(a1, a2);
}

int item_is_two_handed(struct record *a1)
{
    struct item *l_1C;

    if (a1 == 0) return 0;
    l_1C = &a1->data.item;
    return ((((int)(unsigned short)(l_1C->item_flags & 4)) == 0) ? 1 : 0);
}

void trade_total_buy(void)
{
    int l_18;

    D_00190CA8 = 0;
    trade_total = 0;
    object_foreach(player_entity->children, (int)trade_add_buy_price);
    trade_total = trade_base_price(trade_total);
    trade_price = ((trade_price = trade_adjust_price(trade_total, 0)) * trade_price_scale) / 256;
}

int trade_total_repair(void)
{
    int l_20;
    int l_1C;

    trade_total = 0;
    object_foreach(inv_right_container->children, (int)trade_add_repair_cost);
    if (trade_total > 0) {
        l_20 = trade_total;
    } else {
        l_20 = 1;
    }
    trade_total = trade_base_price((trade_total = l_20));
    trade_price = ((trade_price = trade_adjust_price(trade_total, 0)) * trade_price_scale) / 256;
    return trade_total;
}

void inv_close_return_unpaid(void)
{
    int l_18;

    free_later_count = 0;
    object_foreach_pre(player_entity->children, (int)inv_return_unpaid_item);
    if (D_0019628A != 0) {
        object_foreach_pre(player_entity->children, (int)inv_store_cb);
    }
    for (l_18 = 0; l_18 < 27; l_18++) {
        if (player_character->equipped[l_18] != 0 && ((int)(unsigned short)(player_character->equipped[l_18]->flags & 32)) != 0) {
            player_character->equipped[l_18] = 0;
        }
    }
    object_free_pending();
}

void item_remove_equip_effects(struct record *a1, int a2)
{
    int l_1C;
    struct item *l_18;
    int l_14;

    l_1C = 0;
    l_18 = &a1->data.item;
    while (l_1C < 10 && l_18->enchantments[l_1C].type != (-1)) {
        switch (l_18->enchantments[l_1C].type) {
        case 1:
            l_14 = spell_find_on_entity((int)player_entity, l_18->enchantments[l_1C].param, a2 + 200);
            if (l_14 != 0) spell_end(l_14);
            break;
        case 3:
            if (l_18->magicka_bonus != 0) {
                player_character->magicka -= (unsigned short)l_18->magicka_bonus;
                player_character->max_magicka -= (unsigned short)l_18->magicka_bonus;
                if (player_character->magicka < 0) player_character->magicka = 0;
            }
            break;
        case 9:
            player_character->conditions &= ~0x200;
            break;
        case 10:
            player_character->skills[l_18->enchantments[l_1C].param].value -= 15;
        }
        l_1C++;
    }
}

void item_repair_cb(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    if (*(int *)D_00195B84 == 0) return;
    l_18 = &a1->data.item;
    if (l_18->group == 3 && l_18->index == 18) return;
    if (l_18->enchantments[0].type != (-1) && cfg_magic_repair == 0) return;
    if (l_18->condition == l_18->max_condition) return;
    l_18->condition += *(short *)D_00195B84;
    if (l_18->condition > l_18->max_condition) l_18->condition = l_18->max_condition;
    *(int *)D_00195B84 = 0;
}

void item_break(struct record *a1)
{
    struct item *l_20;
    int l_1C;
    int l_18;

    l_20 = &a1->data.item;
    l_18 = -1;
    text_macro_item = l_20;
    l_20->condition = 0;
    inv_store_item(a1);
    mc_set_location(2157, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_001771C5, l_20->name);
    parse_expand((int)text_buffer, (int)D_00190B44);
    hud_message_add((int)D_00190B44);
    for (l_1C = 0; l_1C < 27; l_1C++) {
        if (player_character->equipped[l_1C] == a1) l_18 = l_1C;
    }
    if (l_20->enchantments[0].type != (-1)) {
        if (l_18 != (-1)) item_remove_equip_effects(a1, l_18);
        l_1C = 0;
        while (l_1C < 10 && l_20->enchantments[l_1C].type != (-1)) {
            if (l_20->enchantments[l_1C].type == 15) {
                hud_message_add((int)D_001771D3);
                monster_summon_near_player(l_20->enchantments[l_1C].param)->data.character.team = 1;
            }
            l_1C++;
        }
        object_delete(a1);
    }
    if (l_18 != (-1)) player_character->equipped[l_18] = 0;
    weapon_reload_hand_sprites();
    player_refresh_paperdoll();
}

void inv_store_item(struct record *a1)
{
    struct item *l_1C;
    int l_18;

    a1->id = object_new_id(100);
    if (a1->twin != 0) a1->twin->id = a1->id;
    func_00098F1D(a1);
    a1->caster = 0;
    l_1C = &a1->data.item;
    if (l_1C->enchantments[0].type != (-1) || (l_1C->group == 27 && l_1C->index == 0)) {
        object_reparent(D_001959DC, a1);
        return;
    }
    object_reparent(inventory_containers[(int)(unsigned char)item_group_tab[l_1C->group]], a1);
}

void inv_count_cart_cb(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (l_18->group != 23 || l_18->index != 0) return;
    (scratch_190ce4[0])++;
    scratch_object = a1;
}

void inv_create_wagon(void)
{
    if (wagon_container != 0) return;
    (wagon_container = object_create_child(player_entity, 0, 0))->type = 52;
    wagon_container->flags = 3;
    wagon_container->image = 4;
}

void inv_drop_wagon_if_no_cart(void)
{
    scratch_190ce4[0] = 0;
    object_foreach(player_entity, (int)inv_count_cart_cb);
    if (scratch_190ce4[0] != 0 || wagon_container == 0) return;
    object_delete(wagon_container);
    wagon_container = 0;
}

void inv_merge_arrows(struct record *a1, struct record *a2, int a3)
{
    struct record *l_14;
    int l_10;

    found_object = 0;
    object_find(a1->children, (int)inv_match_arrows);
    if (found_object == 0) {
        l_14 = object_create_child(a1, 0, 107);
        found_object = l_14;
        l_14->type = 2;
        l_14->image2 = 998;
        l_14->image = 0;
        item_make(3, 18, &l_14->data.item);
        l_14->data.item.stack_count = a2->data.item.stack_count;
        if (a1 == player_entity) inv_store_item(l_14);
    } else {
        l_10 = found_object->data.item.stack_count + a2->data.item.stack_count;
        if (l_10 >= 200) l_10 = 199;
        found_object->data.item.stack_count = *(signed char *)&l_10;
    }
    if (a3 == 0) return;
    object_delete(a2);
}

int trade_total_sell(void)
{
    int l_24;
    int l_20;
    int l_1C;

    l_1C = 0;
    l_24 = (int)inv_right_container_base->children;
    while (l_24 != 0) {
        if (((int)(unsigned short)(*(short *)((char *)l_24 + 21) & 32)) == 0) {
            l_20 = l_24 + 71;
            l_1C += *(int *)((char *)l_20 + 36);
        }
        l_24 = *(int *)((char *)l_24 + 55);
    }
    trade_total = trade_base_price(l_1C);
    trade_price = trade_adjust_price(trade_total, 1);
    return trade_total;
}

int trade_region_price(int a1)
{
    a1 = (((int)(unsigned short)*(short *)(region_price_adjustment + (((int)(unsigned char)current_region) * 80))) * a1) / 1000;
    if (a1 < 0) a1 = 1;
    return a1;
}

int trade_adjust_price(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = ((current_building->quality - 10) * 5) + 50;
    l_20 = ((current_building->quality - 10) * 5) + 50;
    if (a2 == 0) {
        l_1C = ((((l_24 << 8) / 200) + 128) * ((int)&*(signed char *)((char *)(((100 - player_character->skills[14].value) << 8) / 200) + 128))) / 256;
        l_18 = ((((l_20 << 8) / 200) + 128) * ((int)&*(signed char *)((char *)(((100 - player_character->attributes[5]) << 8) / 200) + 128))) / 256;
        a1 = (a1 * (((l_1C * 192) / 256) + ((l_18 << 6) / 256))) / 256;
    } else {
        l_1C = (((((100 - l_24) << 8) / 200) + 128) * ((int)&*(signed char *)((char *)((player_character->skills[14].value << 8) / 200) + 128))) / 256;
        l_18 = (((((100 - l_20) << 8) / 200) + 128) * ((int)&*(signed char *)((char *)((player_character->attributes[5] << 8) / 200) + 128))) / 256;
        a1 = (a1 * (((l_1C * 179) / 256) + ((l_18 * 51) / 256))) / 256;
    }
    trade_price = a1;
    return a1;
}

void trade_make_offer(void)
{
    int l_1C;
    int l_18;

    trade_offer_pending = 1;
    l_18 = holiday_today(game_minutes, (int)(unsigned char)current_region);
    if ((trade_mode == 4 && l_18 == 43) || ((struct bf8_7_1 *)&player_motion_flags)->f != 0) {
        return;
    }
    if ((trade_total >> 1) > trade_price) {
        l_1C = 260;
    } else if ((trade_total - (trade_total >> 2)) > trade_price) {
        l_1C = 261;
    } else {
        l_1C = 262;
    }
    if (trade_mode == 2) l_1C += 3;
    msgbox_yes_no_rsc(l_1C);
}

int trade_settle_offer(void)
{
    int l_1C;

    if (trade_offer_pending != 0) {
        l_1C = holiday_today(game_minutes, (int)(unsigned char)current_region);
        if ((trade_mode == 4 && l_1C == 43) || ((struct bf8_7_1 *)&player_motion_flags)->f != 0) {
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

int trade_base_price(int a1)
{
    a1 = trade_region_price(a1);
    a1 += ((current_building->quality - 10) * a1) / 100;
    a1 += a1;
    return a1;
}

void shop_quality_message(struct building *a1)
{
    short l_18;

    D_0012B508 = 146;
    if (a1->quality <= 3) {
        *(int *)&l_18 = 270;
    } else if (a1->quality <= 7) {
        *(int *)&l_18 = 269;
    } else if (a1->quality <= 13) {
        *(int *)&l_18 = 268;
    } else if (a1->quality <= 17) {
        *(int *)&l_18 = 267;
    } else {
        *(int *)&l_18 = 266;
    }
    msgbox_show_rsc((int)(short)l_18, 1);
}

void inv_wagon_button(void)
{
    int l_1C;
    int l_18;

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
    int l_18;

    l_18 = (player_character->skills[15].value - (trade_total / 32)) - (D_00190CA8 / 4);
    if ((rand() % 101) > l_18) {
        skill_add_uses(16, 1);
        inventory_close();
        crime_current = 13;
        guards_summon(1);
        hud_message_add((int)D_001771FF);
        return;
    }
    hud_message_add((int)D_00177217);
    object_foreach(player_entity->children, (int)inv_claim_item);
    for (l_18 = 0; l_18 < 27; l_18++) {
        if (player_character->equipped[l_18] != 0) {
            inv_claim_item((int)player_character->equipped[l_18]);
            inv_store_item(player_character->equipped[l_18]);
        }
    }
    inventory_close();
}

void inv_claim_items(void)
{
    int l_18;

    object_foreach(player_entity->children, (int)inv_claim_item);
    for (l_18 = 0; l_18 < 27; l_18++) {
        if (player_character->equipped[l_18] != 0) {
            inv_claim_item((int)player_character->equipped[l_18]);
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
    struct record *l_1C;
    struct record *l_18;

    switch (trade_mode) {
        return;
    case 1:
        object_foreach(player_entity->children, (int)inv_return_unpaid_item);
        return;
    case 2:
    case 3:
    case 4:
        l_1C = inv_right_container->children;
        while (l_1C != 0) {
            l_18 = l_1C->next;
            if (l_1C->type == 54) l_1C->type = 2;
            inv_store_item(l_1C);
            l_1C = l_18;
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
    struct record *l_24;
    struct record *l_20;
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = 0;
    if (current_building->type != 11 || current_building->faction_id != 40) {
        if (trade_pay_spell_points() == 0) {
            msgbox_show_string((int)D_0017722B, 1);
            return;
        }
        l_24 = inv_right_container->children;
        while (l_24 != 0) {
            if (trade_pay_spell_points() != 0) {
                if (player_character->lock_open_chance >= rand_range(1, 100)) {
                    l_1C++;
                    l_24->data.item.item_flags |= 32;
                }
            }
            l_18++;
            l_24 = l_24->next;
        }
        mc_set_location(2642, (int)D_0017704C);
        mc_sprintf((int)text_buffer, key_names[0], l_1C, l_18);
        msgbox_show_string((int)text_buffer, 1);
        l_24 = inv_right_container->children;
        while (l_24 != 0) {
            l_20 = l_24->next;
            if (((int)(unsigned short)(l_24->data.item.item_flags & 32)) != 0) inv_store_item(l_24);
            l_24 = l_20;
        }
        return;
    }
    trade_make_offer();
}

void inv_toggle_hidden(void)
{
    int l_20;
    struct record *l_1C;
    struct item *l_18;

    if (inv_selected_item == 0) return;
    l_1C = inv_selected_item;
    l_18 = &inv_selected_item->data.item;
    if (((int)(unsigned short)(l_18->item_flags & 64)) != 0) {
        l_18->item_flags &= ~0x40;
        return;
    }
    if (xn_str_find_u32(player_character->equipped, (int)l_1C, 27) != 0) {
        msgbox_show_string((int)D_00177255, 1);
        return;
    }
    l_20 = equip_hiding_capacity(1);
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity, (int)inv_sum_hidden_weight);
    if ((object_weight(l_1C) + *(int *)D_00195B84) > l_20) {
        msgbox_show_string((int)D_0017727F, 1);
        return;
    }
    sound_play(235, player_object, 100);
    l_18->item_flags |= 64;
}

int trade_pay_spell_points(void)
{
    if (player_character->magicka < *(int *)D_001AA458) return 0;
    player_character->magicka -= *(short *)D_001AA458;
    return 1;
}

void trade_total_identify(void)
{
    int l_1C;
    int l_18;

    trade_total = 0;
    l_1C = (int)inv_right_container->children;
    while (l_1C != 0) {
        trade_total += ((unsigned)(*(int *)((char *)l_1C + 107) * 25)) >> 8;
        l_1C = *(int *)((char *)l_1C + 55);
    }
    trade_price = trade_total;
}

void func_00098A15(void)
{
    int l_18;

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
        l_18 = 1;
    } else {
        l_18 = 2;
    }
    map_goto_location(31, 1, l_18, 0);
    player_to_nearest_marker((int)location_object, 8);
}

int func_00098B20(void)
{
    int l_24;
    int l_20;
    int l_1C;

    if ((((unsigned)location_object->id) >> 16) == 992) {
        l_24 = 1;
    } else {
        l_24 = 0;
    }
    l_1C = l_24;
    if ((((unsigned)location_object->id) >> 16) == 993) {
        l_20 = 1;
    } else {
        l_20 = 0;
    }
    l_1C += l_20;
    return l_1C;
}

int func_00098B91(struct record *a1)
{
    if (a1->twin != 0 && a1->quest_id != 0) {
        current_quest = (struct quest *)quest_find_by_id((int)(short)((unsigned short)a1->quest_id));
    } else {
        return 0;
    }
    return 1;
}

void inv_read_map_scrap(struct record *a1)
{
    {
        char l_2C[20];

        if (a1 != 0) object_delete(a1);
        text_macro_map_location = (int)l_2C;
        location_pick_random_undiscovered((int)l_2C);
        msgbox_show_rsc(499, 1);
        location_set_discovered((int)(unsigned short)*(short *)(*(char **)((char *)l_2C + 12) + 27), 1);
        location_free((int)l_2C);
    }
}

int item_forbidden_for_class(struct item *a1)
{
    if ((player_class->forbidden_materials != 0 || player_class->forbidden_equipment != 0) && (a1->group == 3 || a1->group == 2)) {
        if (a1->group == 2 && a1->index >= 7 && a1->index <= 10) {
            if ((player_class->forbidden_equipment & ((1 << (a1->index - 7)) << 9)) != 0) {
                msgbox_show_rsc(1068, 1);
                return 1;
            }
        }
        if (a1->group == 2 && a1->index < 7 && (player_class->forbidden_equipment & ((1 << a1->armor_type) << 6)) != 0) {
            msgbox_show_rsc(1068, 1);
            return 1;
        }
        if (a1->group == 3 && (player_class->forbidden_equipment & ((int)(short)*(short *)(weapon_proficiency_bits + (a1->index * 2)))) != 0) {
            msgbox_show_rsc(1068, 1);
            return 1;
        }
        if (a1->group == 3 && (player_class->forbidden_materials & (1 << a1->material)) != 0) {
            msgbox_show_rsc(1068, 1);
            return 1;
        }
        if (a1->armor_type == 2 && (player_class->forbidden_materials & (1 << a1->material)) != 0) {
            msgbox_show_rsc(1068, 1);
            return 1;
        }
    }
    return 0;
}

void func_00098F1D(struct record *a1)
{
    int l_18;

    if (a1->twin == 0) return;
    a1->twin->id = a1->id;
}

void inv_close_assign_ids(void)
{
    object_foreach(player_entity->children, (int)inv_assign_item_id);
}

void inv_track_hand_weapons(int a1)
{
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = 0;
    if (a1 == 0) {
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
            if (D_001AA44C != 0) l_1C += *(int *)(D_0017887F + (D_001AA448 << 2));
            if (player_character->equipped[19] != 0) {
                l_1C += *(int *)(D_0017887F + (player_character->equipped[19]->data.item.index << 2));
            }
        }
        if (D_001AA450 != (int)player_character->equipped[21]) {
            if (D_001AA450 != 0) l_18 += *(int *)(D_0017887F + (D_001AA444 << 2));
            if (player_character->equipped[21] != 0) {
                l_18 += *(int *)(D_0017887F + (player_character->equipped[21]->data.item.index << 2));
            }
        }
    }
    D_0019597C[0] += l_1C;
    left_hand_ready_delay += l_18;
}

void item_refresh_magic_value_cb(int a1)
{
    int l_1C;
    int l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) return;
    l_1C = *(int *)((char *)a1 + 67);
    while (((int)(unsigned char)*(signed char *)((char *)l_1C)) != 52) {
        if (((int)(unsigned char)*(signed char *)((char *)l_1C)) == 1) return;
        l_1C = *(int *)((char *)l_1C + 67);
    }
    if (((int)(unsigned short)*(short *)((char *)l_1C + 27)) > 4) return;
    l_18 = a1 + 71;
    if (((int)(short)*(short *)((char *)l_18 + 67)) == (-1)) return;
    *(int *)((char *)l_18 + 36) = enchant_item_value(l_18);
}

void inv_refresh_magic_values(void)
{
    object_foreach(player_entity->children, (int)item_refresh_magic_value_cb);
}

int trade_can_repair_item(struct item *a1)
{
    if (inv_right_container->children != 0 && current_building->type == 11) {
        msgbox_show_string((int)D_00177300, 1);
        return 0;
    }
    if (inv_selected_item->children != 0) {
        msgbox_show_string((int)D_00177323, 1);
        return 0;
    }
    if (a1->group == 3 && a1->index == 18) {
        msgbox_show_rsc(24, 1);
        return 0;
    }
    if (a1->condition == a1->max_condition) {
        msgbox_show_rsc(24, 1);
        return 0;
    }
    return 1;
}

void trade_schedule_repair(void)
{
    struct item *l_18;

    if (trade_mode != 3) return;
    l_18 = &inv_selected_item->data.item;
    if (current_building->type == 11) {
        inv_selected_item->repair_due = (((((l_18->max_condition - l_18->condition) / ((int)&*(signed char *)((char *)(guild_membership->rank) + 1))) * 1440) / 144000) + 1440) + game_minutes;
        return;
    }
    trade_schedule_shop_repairs();
}

void trade_mark_in_repair(void)
{
    int l_18;

    l_18 = (int)inv_right_container->children;
    while (l_18 != 0) {
        *(signed char *)((char *)l_18) = 54;
        l_18 = *(int *)((char *)l_18 + 55);
    }
}

int potion_recipe_text(int a1)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = *(int *)scratch_buffer + 55000;
    *(signed char *)((char *)l_1C) = 0;
    while (((int)(signed char)*(signed char *)((char *)(a1 + l_20))) != (-2) && l_20 < 8) {
        func_000A1054(l_1C, ((int)item_templates) + (((int)(short)*(short *)((char *)(int)(*(char **)(item_group_templates + (((int)(signed char)*(signed char *)((char *)(a1 + l_20) + 10)) << 2)) + (((int)(signed char)*(signed char *)((char *)(a1 + l_20))) * 2)))) * 48), (int)D_0017704C, 3200, 4);
        func_000A1054(l_1C, (int)D_00177346, (int)D_0017704C, 3201, 4);
        l_20++;
    }
    *(signed char *)((char *)(strlen(l_1C) + l_1C) + 1) = 0;
    return l_1C;
}
