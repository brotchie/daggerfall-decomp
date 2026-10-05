/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char key_down_esc[];
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
extern char trade_price_scale[];
extern char player_environment[];
extern char item_templates[];
extern char potion_recipes[];
extern char D_001832A4[];
extern char D_00184221[];
extern char key_names[];
extern char spell_last_cast_id[];
extern char D_00185F88[];
extern char D_00187CA8[];
extern char D_00187DAC[];
extern char item_group_tab[];
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
extern char text_buffer[];
extern char D_00190504[];
extern char D_00190B44[];
extern char D_00190CA8[];
extern char itemmaker_slot_kinds[];
extern char D_00190EAC[];
extern char D_001913E4[];
extern char D_001940D4[];
extern char D_001940D6[];
extern char D_001940D8[];
extern char player_motion_flags[];
extern char D_0019597C[];
extern char D_00195980[];
extern char inventory_containers[];
extern char D_001959DC[];
extern char wagon_container[];
extern char D_00195A80[];
extern char camera_object[];
extern char current_building[];
extern char player_entity[];
extern char player_object[];
extern char D_00195AA8[];
extern char D_00195AC4[];
extern char D_00195ACC[];
extern char text_macro_book[];
extern char D_00195ADC[];
extern char D_00195AF4[];
extern char creature_count[];
extern char inv_right_container[];
extern char D_00195B34[];
extern char guild_npc_object[];
extern char D_00195B84[];
extern char inpstr_result[];
extern char player_character[];
extern char player_class[];
extern char game_minutes[];
extern char game_settings[];
extern char D_00195C44[];
extern char D_00195D2C[];
extern char D_00195D30[];
extern char trade_mode[];
extern char inventory_action[];
extern char free_later_count[];
extern char player_death_timer[];
extern char D_00195DA8[];
extern char cfg_magic_repair[];
extern char D_00195F2E[];
extern char D_00195FB1[];
extern char D_001960D9[];
extern char current_region[];
extern char D_0019626F[];
extern char D_00196271[];
extern char D_00196272[];
extern char game_mode[];
extern char mouse_buttons_prev[];
extern char crime_current[];
extern char inv_right_icon[];
extern char D_0019628A[];
extern char D_0019629A[];
extern char D_0019629D[];
extern char D_001962A9[];
extern char D_001962AE[];
extern char D_001962AF[];
extern char D_001962B1[];
extern char current_quest[];
extern char D_00199768[];
extern char itemmaker_item_object[];
extern char guild_membership[];
extern char inventory_images[];
extern char D_001AA420[];
extern char D_001AA424[];
extern char D_001AA428[];
extern char D_001AA42C[];
extern char D_001AA430[];
extern char D_001AA434[];
extern char D_001AA438[];
extern char D_001AA43C[];
extern char D_001AA440[];
extern char D_001AA444[];
extern char D_001AA448[];
extern char D_001AA44C[];
extern char D_001AA450[];
extern char D_001AA454[];
extern char D_001AA458[];
extern char inv_temp_pile[];
extern char D_001AA460[];
extern char D_001AA4CC[];
extern char D_001AA534[];
extern char D_001AA53C[];
extern char D_001AA540[];
extern char D_001AA544[];
extern char inv_selected_item[];
extern char inv_left_scroll[];
extern char inv_right_scroll[];
extern char inv_left_rows[];
extern char inv_left_container[];
extern char D_001AA580[];
extern char D_001AA586[];
extern char D_001AA5F7[];
extern char D_001AA5F8[];

extern int damage_apply(int, int, int);
extern int sheet_open(int);
extern int spellbook_open(int);
extern int key_action_held(int);
extern int macro_kg_weight(void);
extern int object_weight(int);
extern int holiday_today(int, int);
extern int quest_find_by_id(int);
extern int enchant_item_value(int);
extern int cast_item_used_spell(short);
extern int spell_find_on_entity(int, short, int);
extern int func_0005FD36(int);
extern int monster_summon_near_player(int);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int gold_can_afford(int);
extern int carry_capacity(void);
extern int object_free_single(int);
extern int object_delete(int);
extern int object_create_child(int, int, int);
extern int object_reparent(int, int);
extern int object_find(int, int);
extern int object_new_id(int);
extern int inv_match_arrows(int);
extern int inv_draw_item_cell(int, int, int);
extern int func_000990F0(unsigned short);
extern int player_to_nearest_marker(int, int);
extern int rand();
extern int mc_free();
extern int mc_memset();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A1054();
extern int func_000C0700();
extern int func_000CD308();
extern int func_000CD31A();
extern int func_000CE44C();
extern int func_0012B136();
extern int func_00135D00();
extern int func_00135E39();
extern int func_00135E90();
extern int func_00144FB4();
extern void func_00030F39(void);
extern void skill_add_uses(int, int);
extern void msgbox_show_string(int, int);
extern void msgbox_show_quest_text(int, short, int);
extern void msgbox_show_qrc_text(int, unsigned short, int);
extern void msgbox_show_rsc(int, int);
extern void guards_summon(int);
extern void parse_expand(int, int);
extern void quest_raise_event(int, int, int);
extern void player_refresh_paperdoll(void);
extern void book_open(short);
extern void item_make(int, int, int);
extern void func_0005E722(int);
extern void item_info_painting(int);
extern void item_damage(int, int);
extern void func_00065A8C(int, int, int);
extern void weapon_reload_hand_sprites(void);
extern void book_read_header(int, unsigned short);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void func_0007D19B(int, int, short, short);
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
extern void object_free_children(int);
extern void object_foreach_pre(int, int);
extern void object_foreach(int, int);
extern void potion_drink(int);
extern void inv_sum_hidden_weight(int);
extern void trade_add_buy_price(int);
extern void trade_add_repair_cost(int);
extern void inv_return_unpaid_item(int);
extern void inv_store_callback(int);
extern void inv_claim_item(int);
extern void inv_assign_item_id(int);
extern void inventory_draw(void);
extern void inv_select_tab(unsigned char);
extern void inv_click_right_item(int);
extern void inv_click_left_item(int);
extern void inv_equip_item(int);
extern void item_apply_equip_effects(int, int);
extern void func_0009830F(void);
extern void func_00098538(void);
int inventory_open(int, int, int);
int func_000977F2(int);
int trade_adjust_price(int, int);
int func_00097B2A(void);
int func_00097BD9(int);
int func_0009848E(void);
int func_00098B91(int);
int func_000993CB(int);
void inventory_load_images(void);
void inventory_free_images(void);
void inventory_close(void);
void inv_unequip_item(int);
void inv_equip_in_slot(int, int);
void inv_close_return_unpaid(void);
void item_remove_equip_effects(int, int);
void inv_store_item(int);
void func_000971C1(int);
void func_00097271(void);
void inv_merge_arrows(int, int, int);
void func_00097A85(void);
void inv_wagon_button(void);
void func_00097F6F(void);
void inv_read_map_scrap(int);
void func_00098F1D(int);
void func_00098F50(void);
void inv_track_hand_weapons(int);
void func_00099155(int);
void func_000991E7(void);
void func_00099391(void);
#pragma aux func_000A0ED9 parm routine [];

void inventory_load_images(void)
{
    *(int *)inventory_images = disk_read_file((int)D_00176FE4, 0);
    *(int *)D_001AA420 = disk_read_file((int)D_00176FF1, 0);
    *(int *)D_001AA424 = disk_read_file((int)D_00176FFE, 0);
    *(int *)D_001AA428 = disk_read_file((int)D_0017700B, 0);
    *(int *)D_001AA42C = disk_read_file((int)D_00177018, 0);
    *(int *)D_001AA430 = disk_read_file((int)D_00177025, 0);
    *(int *)D_001AA43C = disk_read_file((int)D_00177032, 0);
    *(int *)D_001AA440 = disk_read_file((int)D_0017703F, 0);
    if (*(int *)trade_mode == 0) return;
    func_000A0ED9(313, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_00177054, (*(int *)trade_mode * 2) + 6);
    *(int *)D_001AA434 = disk_read_file((int)text_buffer, 0);
    func_000A0ED9(315, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_00177054, (*(int *)trade_mode * 2) + 7);
    *(int *)D_001AA438 = disk_read_file((int)text_buffer, 0);
}

void inventory_free_images(void)
{
    if (*(int *)inventory_images == 0) goto L92499;
    if (*(int *)inventory_images != (-1751672937)) goto L9249B;
L92499:;
    goto L924B9;
L9249B:;
    mc_free(*(int *)inventory_images, (int)D_0017704C, 322);
    *(int *)inventory_images = -1751672937;
L924B9:;
    if (*(int *)D_001AA420 == 0) goto L924CE;
    if (*(int *)D_001AA420 != (-1751672937)) goto L924D0;
L924CE:;
    goto L924EE;
L924D0:;
    mc_free(*(int *)D_001AA420, (int)D_0017704C, 323);
    *(int *)D_001AA420 = -1751672937;
L924EE:;
    if (*(int *)D_001AA424 == 0) goto L92503;
    if (*(int *)D_001AA424 != (-1751672937)) goto L92505;
L92503:;
    goto L92523;
L92505:;
    mc_free(*(int *)D_001AA424, (int)D_0017704C, 324);
    *(int *)D_001AA424 = -1751672937;
L92523:;
    if (*(int *)D_001AA428 == 0) goto L92538;
    if (*(int *)D_001AA428 != (-1751672937)) goto L9253A;
L92538:;
    goto L92558;
L9253A:;
    mc_free(*(int *)D_001AA428, (int)D_0017704C, 325);
    *(int *)D_001AA428 = -1751672937;
L92558:;
    if (*(int *)D_001AA42C == 0) goto L9256D;
    if (*(int *)D_001AA42C != (-1751672937)) goto L9256F;
L9256D:;
    goto L9258D;
L9256F:;
    mc_free(*(int *)D_001AA42C, (int)D_0017704C, 326);
    *(int *)D_001AA42C = -1751672937;
L9258D:;
    if (*(int *)D_001AA430 == 0) goto L925A2;
    if (*(int *)D_001AA430 != (-1751672937)) goto L925A4;
L925A2:;
    goto L925C2;
L925A4:;
    mc_free(*(int *)D_001AA430, (int)D_0017704C, 327);
    *(int *)D_001AA430 = -1751672937;
L925C2:;
    if (*(int *)D_001AA43C == 0) goto L925D7;
    if (*(int *)D_001AA43C != (-1751672937)) goto L925D9;
L925D7:;
    goto L925F7;
L925D9:;
    mc_free(*(int *)D_001AA43C, (int)D_0017704C, 328);
    *(int *)D_001AA43C = -1751672937;
L925F7:;
    if (*(int *)D_001AA440 == 0) goto L9260C;
    if (*(int *)D_001AA440 != (-1751672937)) goto L9260E;
L9260C:;
    goto L9262C;
L9260E:;
    mc_free(*(int *)D_001AA440, (int)D_0017704C, 329);
    *(int *)D_001AA440 = -1751672937;
L9262C:;
    if (*(int *)trade_mode == 0) return;
    if (*(int *)D_001AA434 == 0) goto L9264E;
    if (*(int *)D_001AA434 != (-1751672937)) goto L92650;
L9264E:;
    goto L9266E;
L92650:;
    mc_free(*(int *)D_001AA434, (int)D_0017704C, 333);
    *(int *)D_001AA434 = -1751672937;
L9266E:;
    if (*(int *)D_001AA438 == 0) goto L92683;
    if (*(int *)D_001AA438 != (-1751672937)) goto L92685;
L92683:;
    return;
L92685:;
    mc_free(*(int *)D_001AA438, (int)D_0017704C, 334);
    *(int *)D_001AA438 = -1751672937;
}

int inventory_open(int a1, int a2, int a3)
{
    if (*(int *)player_death_timer <= 0) goto L926D7;
    return 0;
L926D7:;
    if (((int)(unsigned char)*(signed char *)D_0019626F) != 4) goto L926EF;
    if (((int)(unsigned char)*(signed char *)game_mode) == 8) goto L926F1;
L926EF:;
    goto L926FD;
L926F1:;
    return 1;
L926FD:;
    if (a1 != 0) goto L9272A;
    if (*(signed char *)game_mode != 0) goto L9271A;
    if (key_action_held(37) != 0) goto L9271C;
L9271A:;
    goto L92725;
L9271C:;
    if (*(int *)player_death_timer == 0) goto L9272A;
L92725:;
    goto L928D6;
L9272A:;
    *(int *)inv_temp_pile = 0;
    if (((int)(unsigned char)*(signed char *)(*(char **)player_character + 67)) <= 8) goto L92761;
    msgbox_show_string((int)D_00177063, 1);
    return 0;
L92761:;
    if (a1 != 0) goto L9276E;
    a1 = 1;
L9276E:;
    if (a2 != 0) goto L9277A;
    if (a1 == 1) goto L9277F;
L9277A:;
    goto L92843;
L9277F:;
    *(int *)inv_temp_pile = (*(int *)D_00195B34 = (*(int *)inv_right_container = object_create_child(*(int *)(*(char **)player_object + 67), 0, 0)));
    *(signed char *)(*(char **)inv_right_container) = 33;
    *(short *)(*(char **)inv_right_container + 27) = ((unsigned short)(unsigned char)*(signed char *)(D_00187DAC + (rand() % 20))) + 27648;
    *(short *)(*(char **)inv_right_container + 25) = 1;
    *(signed char *)(*(char **)inv_right_container + 21) |= 5;
    *(int *)(*(char **)inv_right_container + 7) = *(int *)(*(char **)player_object + 7);
    *(int *)(*(char **)inv_right_container + 11) = *(int *)(*(char **)player_object + 11);
    *(int *)(*(char **)inv_right_container + 15) = *(int *)(*(char **)player_object + 15);
    *(int *)(*(char **)inv_right_container + 31) = object_new_id(((unsigned)*(int *)(*(char **)D_00195AC4 + 31)) >> 16);
    a1 = 2;
L92843:;
    if (*(signed char *)mouse_buttons == 0) goto L92853;
    func_0012B136();
    goto L92843;
L92853:;
    *(signed char *)D_00187CA8 = 0;
    *(signed char *)game_mode = 4;
    *(signed char *)D_00196272 = 1;
    *(int *)trade_mode = a2;
    *(signed char *)D_001AA5F8 = (*(signed char *)inv_right_icon = *(signed char *)&a3);
    *(int *)inventory_action = 2;
    *(int *)D_00195D2C = (*(int *)D_00195D30 = 0);
    *(int *)inv_right_scroll = (*(int *)inv_left_scroll = 0);
    inventory_load_images();
    inv_track_hand_weapons(0);
    func_000991E7();
    *(signed char *)D_001962A9 = 1;
    *(int *)D_001AA53C = *(int *)inv_right_container;
L928D6:;
    return ((((int)(unsigned char)*(signed char *)game_mode) == 4) ? 1 : 0);
}

void inventory_frame(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (inventory_open(0, 0, 2) == 0) return;
    func_000CD308();
    func_000CD31A();
    func_00135E90();
    inventory_draw();
    l_1C = func_00097B2A();
    if (l_1C <= 0) goto L92A22;
    if (*(int *)trade_mode == 2) goto L92962;
    if (gold_can_afford(l_1C) == 0) goto L92964;
L92962:;
    goto L92978;
L92964:;
    msgbox_show_rsc(454, 1);
    goto L92A0D;
L92978:;
    if (*(int *)trade_mode == 2) goto L929F6;
    l_18 = holiday_today(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region);
    if (*(int *)trade_mode != 4) goto L929BE;
    if (l_18 == 43) goto L929B2;
    if (((struct bf8_7_1 *)&player_motion_flags)->f == 0) goto L929B4;
L929B2:;
    goto L929BC;
L929B4:;
    gold_spend(l_1C);
L929BC:;
    goto L929C6;
L929BE:;
    gold_spend(l_1C);
L929C6:;
    if (*(int *)trade_mode != 4) goto L929D6;
    func_00098538();
    goto L929F4;
L929D6:;
    if (*(int *)trade_mode != 1) goto L929E6;
    func_00097F6F();
    goto L929F4;
L929E6:;
    if (*(int *)trade_mode != 3) goto L929F4;
    func_00099391();
L929F4:;
    goto L92A0D;
L929F6:;
    gold_add(l_1C);
    func_00097271();
    object_free_children(*(int *)inv_right_container);
L92A0D:;
    sound_play(204, *(int *)player_object, 100);
L92A22:;
    if (*(signed char *)key_down_esc == 0) goto L92A30;
    inventory_close();
L92A30:;
    if (*(signed char *)mouse_buttons == 0) goto L92A4D;
    if (*(signed char *)mouse_buttons == 0) goto L92A4B;
    if (*(signed char *)mouse_buttons_prev != 0) goto L92A4D;
L92A4B:;
    goto L92A52;
L92A4D:;
    return;
L92A52:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 4) return;
    l_20 = 0;
L92A69:;
    if (l_20 < 7) goto L92A7C;
    goto L92B2D;
L92A74:;
    l_20++;
    goto L92A69;
L92A7C:;
    if (*(short *)mouse_x <= *(short *)(inv_mode_buttons + ((*(int *)trade_mode * 84) + (l_20 * 12)))) goto L92AB6;
    if (*(short *)mouse_x < *(short *)(D_00188285 + ((*(int *)trade_mode * 84) + (l_20 * 12)))) goto L92AB8;
L92AB6:;
    goto L92AD5;
L92AB8:;
    if (*(short *)mouse_y > *(short *)(D_00188283 + ((*(int *)trade_mode * 84) + (l_20 * 12)))) goto L92AD7;
L92AD5:;
    goto L92AF4;
L92AD7:;
    if (*(short *)mouse_y < *(short *)(D_00188287 + ((*(int *)trade_mode * 84) + (l_20 * 12)))) goto L92AF6;
L92AF4:;
    goto L92B28;
L92AF6:;
    sound_play(203, *(int *)player_object, 100);
    ((int (*)())(*(int *)(D_00188289 + ((*(int *)trade_mode * 84) + (l_20 * 12)))))(l_20, 27);
    goto L92B2D;
L92B28:;
    goto L92A74;
L92B2D:;
    if (*(signed char *)mouse_buttons == 0) goto L92B4A;
    if (*(signed char *)mouse_buttons == 0) goto L92B48;
    if (*(signed char *)mouse_buttons_prev != 0) goto L92B4A;
L92B48:;
    goto L92B4F;
L92B4A:;
    return;
L92B4F:;
    l_20 = 0;
L92B56:;
    if (l_20 < 45) goto L92B69;
    return;
L92B61:;
    l_20++;
    goto L92B56;
L92B69:;
    if (*(short *)mouse_x <= *(short *)(inv_buttons + (l_20 * 12))) goto L92B91;
    if (*(short *)mouse_x < *(short *)(D_00188429 + (l_20 * 12))) goto L92B93;
L92B91:;
    goto L92BA7;
L92B93:;
    if (*(short *)mouse_y > *(short *)(D_00188427 + (l_20 * 12))) goto L92BA9;
L92BA7:;
    goto L92BBD;
L92BA9:;
    if (*(short *)mouse_y < *(short *)(D_0018842B + (l_20 * 12))) goto L92BBF;
L92BBD:;
    goto L92BE8;
L92BBF:;
    sound_play(203, *(int *)player_object, 100);
    ((int (*)())(*(int *)(D_0018842D + (l_20 * 12))))(l_20, 27);
    return;
L92BE8:;
    goto L92B61;
}

void inventory_close(void)
{
    int l_1C;
    int l_18;

    if (*(int *)inv_left_container != *(int *)wagon_container) goto L92C1B;
    if (*(int *)trade_mode == 1) goto L92C1D;
L92C1B:;
    goto L92C27;
L92C1D:;
    inv_select_tab(41);
L92C27:;
    if (*(int *)inv_right_container != *(int *)wagon_container) goto L92C39;
    inv_wagon_button();
L92C39:;
    if (*(signed char *)key_down_esc != 0) goto L92C39;
    if (*(int *)trade_mode != 0) goto L92C50;
    func_00097F6F();
L92C50:;
    if (*(int *)trade_mode == 2) goto L92C62;
    if (*(int *)trade_mode != 4) goto L92C64;
L92C62:;
    goto L92C6D;
L92C64:;
    if (*(int *)trade_mode != 3) goto L92CBD;
L92C6D:;
    l_1C = *(int *)(*(char **)D_00195B34 + 63);
L92C78:;
    if (l_1C == 0) goto L92C8D;
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) == 2) goto L92C8F;
L92C8D:;
    goto L92CBD;
L92C8F:;
    l_18 = *(int *)((char *)l_1C + 55);
    if (((int)(unsigned short)(*(short *)((char *)l_1C + 21) & 32)) != 0) goto L92CB5;
    inv_store_item(l_1C);
L92CB5:;
    l_1C = l_18;
    goto L92C78;
L92CBD:;
    *(signed char *)D_001962B1 = 0;
    *(signed char *)D_00187CA8 = 1;
    inventory_free_images();
    *(int *)D_00199768 = 0;
    func_00030F39();
    inv_close_return_unpaid();
    *(signed char *)D_001940D8 |= 8;
    *(int *)trade_price_scale = 256;
    func_00098F50();
    if (*(signed char *)D_001962AF == 0) goto L92D14;
    *(signed char *)D_001962AF = 0;
    object_free_children((int)D_001960D9);
L92D14:;
    if (((int)D_001960D9) != *(int *)D_00195B34) goto L92D35;
    *(int *)inv_right_container = (*(int *)D_00195B34 = 0);
L92D35:;
    if (*(int *)inv_right_container == *(int *)D_00195B34) goto L92D51;
    *(int *)inv_right_container = *(int *)(*(char **)inv_right_container + 67);
    goto L92D35;
L92D51:;
    if (((int)(unsigned char)*(signed char *)(*(char **)inv_right_container)) != 33) goto L92D6D;
    if (*(int *)(*(char **)inv_right_container + 63) == 0) goto L92D6F;
L92D6D:;
    goto L92D78;
L92D6F:;
    *(signed char *)(*(char **)inv_right_container + 22) |= 2;
L92D78:;
    if (((int)(unsigned char)*(signed char *)(*(char **)inv_right_container)) != 33) goto L92D92;
    *(signed char *)(*(char **)inv_right_container + 21) |= 1;
L92D92:;
    if (((int)(unsigned char)*(signed char *)(*(char **)D_00195DA8)) != 33) goto L92DAE;
    if (*(int *)(*(char **)D_00195DA8 + 63) == 0) goto L92DB0;
L92DAE:;
    goto L92DB9;
L92DB0:;
    *(signed char *)(*(char **)D_00195DA8 + 22) |= 2;
L92DB9:;
    if (*(int *)D_00195ADC == 0) goto L92DC8;
    ((int (*)())(*(int *)D_00195ADC))();
L92DC8:;
    if (*(int *)inv_temp_pile == 0) goto L92DDC;
    if (*(int *)(*(char **)inv_temp_pile + 63) == 0) goto L92DDE;
L92DDC:;
    goto L92DE8;
L92DDE:;
    object_delete(*(int *)inv_temp_pile);
L92DE8:;
    if (*(signed char *)mouse_buttons == 0) goto L92DF8;
    func_0012B136();
    goto L92DE8;
L92DF8:;
    *(signed char *)game_mode = 0;
    *(signed char *)D_00196272 = 0;
    *(signed char *)D_001940D6 &= 123;
    *(signed char *)player_motion_flags &= 127;
    weapon_reload_hand_sprites();
    inv_track_hand_weapons(1);
    if (*(int *)D_0019597C <= 0) goto L92E3A;
    if (*(int *)(*(char **)player_character + 443) != 0) goto L92E3C;
L92E3A:;
    goto L92E68;
L92E3C:;
    *(int *)D_00195A80 = *(int *)(*(char **)player_character + 443) + 71;
    parse_expand((int)D_00177099, (int)D_001913E4);
    hud_message_add((int)D_001913E4);
L92E68:;
    if (*(int *)D_00195980 <= 0) goto L92E7F;
    if (*(int *)(*(char **)player_character + 451) != 0) goto L92E81;
L92E7F:;
    goto L92EAD;
L92E81:;
    *(int *)D_00195A80 = *(int *)(*(char **)player_character + 451) + 71;
    parse_expand((int)D_00177099, (int)D_001913E4);
    hud_message_add((int)D_001913E4);
L92EAD:;
    if (((struct bf8_5_1 *)&D_001940D8)->f == 0) goto L92EC7;
    *(signed char *)D_001940D8 &= 223;
    sheet_open(1);
L92EC7:;
    *(signed char *)D_001962A9 = 0;
}

void inv_draw_container_icon(int a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_20 = *(int *)D_001AA440;
    l_1C = 0;
    if (a2 != 1) goto L933B0;
    if (((int)(unsigned short)(*(short *)(*(char **)game_settings) & 4)) != 0) goto L933B2;
L933B0:;
    goto L933B9;
L933B2:;
    a2 = 10;
L933B9:;
    if (l_1C >= a2) goto L933DE;
    l_20 = (((int)(unsigned short)*(short *)((char *)l_20 + 10)) + l_20) + 12;
    l_1C++;
    goto L933B9;
L933DE:;
    l_18 = ((int)(short)*(short *)(inv_buttons + (a1 * 12))) + ((((int)&*(signed char *)((char *)(((int)(short)*(short *)(D_00188429 + (a1 * 12))) - ((int)(short)*(short *)(inv_buttons + (a1 * 12)))) + 1)) - ((int)(unsigned short)*(short *)((char *)l_20 + 4))) >> 1);
    l_14 = ((int)(short)*(short *)(D_00188427 + (a1 * 12))) + ((((((int)(short)*(short *)(D_0018842B + (a1 * 12))) - ((int)(short)*(short *)(D_00188427 + (a1 * 12)))) + 1) - ((int)(unsigned short)*(short *)((char *)l_20 + 6))) >> 1);
    func_00144FB4(l_18, l_14, (int)(unsigned short)*(short *)((char *)l_20 + 4), (int)(unsigned short)*(short *)((char *)l_20 + 6), l_20 + 12);
    if (*(int *)wagon_container == 0) goto L93486;
    if (a2 == 3) goto L93488;
L93486:;
    return;
L93488:;
    *(signed char *)D_001962AE = 1;
    *(int *)D_00195AA8 = *(int *)wagon_container;
    func_000A0ED9(657, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_001770A7, macro_kg_weight());
    text_draw_colored((int)text_buffer, (int)(short)(l_18 + 1), (int)(short)(l_14 + 1), 145, 156);
    *(signed char *)D_001962AE = 0;
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
    l_24 = *(int *)D_001AA440;
    *(int *)&l_10 = 0;
L93C36:;
    if (((int)(short)l_10) >= 3) goto L93C5C;
    l_24 = (((int)(unsigned short)*(short *)((char *)l_24 + 10)) + l_24) + 12;
    (*(int *)&l_10)++;
    goto L93C36;
L93C5C:;
    l_18 = *(short *)((char *)l_24 + 4);
    l_14 = *(short *)((char *)l_24 + 6);
    func_0007D19B((int)&l_18, (int)&l_14, (int)(short)((*(short *)((char *)((a3 * 12) + a2) + 4) - *(short *)((char *)((a3 * 12) + a2))) - 4), (int)(short)((*(short *)((char *)((a3 * 12) + a2) + 6) - *(short *)((char *)((a3 * 12) + a2) + 2)) - 4));
    a3 = 0;
L93CB9:;
    if (((int)(unsigned short)*(short *)((char *)l_24 + 6)) > a3) goto L93CD4;
    goto L93D16;
L93CCC:;
    a3++;
    goto L93CB9;
L93CD4:;
    mc_memcpy((int)(*(char **)D_00195C44 + (a3 << 8)), (l_24 + 12) + (((int)(unsigned short)*(short *)((char *)l_24 + 4)) * a3), (int)(unsigned short)*(short *)((char *)l_24 + 4), (int)D_0017704C, 787, 4);
    goto L93CCC;
L93D16:;
    func_000C0700(((int)(short)l_20) - (((int)(short)l_18) >> 1), ((int)(short)l_1C) - (((int)(short)l_14) >> 1), (int)(short)l_18, (int)(short)l_14, (int)(unsigned short)*(short *)((char *)l_24 + 4), (int)(unsigned short)*(short *)((char *)l_24 + 6), 0, *(int *)D_00195C44);
    func_000A0ED9(791, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_001770C0, macro_kg_weight());
    text_draw_colored((int)text_buffer, (int)(short)(*(short *)((char *)((a3 * 12) + a2)) + 3), (int)(short)(*(short *)((char *)((a3 * 12) + a2) + 2) + 2), 145, 156);
}

void func_00093DCB(int a1, int a2, int a3, int a4)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;
    int l_C;

    l_20 = func_00135D00(a1, a2, -1);
    if (l_20 != 0) goto L93E13;
    func_00135E39();
    l_20 = func_00135D00(a1, a2, -1);
L93E13:;
    l_1C = *(int *)((char *)l_20 + 12);
    l_18 = (((int)(short)*(short *)((char *)((a4 * 12) + a3))) + ((int)(short)*(short *)((char *)((a4 * 12) + a3) + 4))) >> 1;
    l_14 = (((int)(short)*(short *)((char *)((a4 * 12) + a3) + 2)) + ((int)(short)*(short *)((char *)((a4 * 12) + a3) + 6))) >> 1;
    l_10 = (int)(unsigned short)*(short *)((char *)l_1C + 4);
    l_C = (int)(unsigned short)*(short *)((char *)l_1C + 6);
    func_000C0700(l_18 - (l_10 >> 1), l_14 - (l_C >> 1), l_10, l_C, (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), (int)(unsigned short)(*(short *)((char *)l_1C + 8) | 32768), l_1C + *(int *)((char *)l_1C + 14));
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
    goto L942C1;
case 0:
    a1 = 4;
    goto L942C1;
case 1:
case 2:
case 3:
case 4:
    a1--;
    goto L942C1;
case 5:
    a1 = 9;
    goto L942C1;
case 6:
case 7:
case 8:
case 9:
    a1--;
default:
L942C1:;
    if (a1 >= 5) goto L942D6;
    if (*(int *)(inv_left_rows + (a1 << 2)) != 0) goto L942D8;
L942D6:;
    goto L942EB;
L942D8:;
    inv_click_left_item(*(int *)(inv_left_rows + (a1 << 2)));
    return;
L942EB:;
    if (a1 <= 4) goto L942F7;
    if (a1 < 10) goto L942F9;
L942F7:;
    goto L94308;
L942F9:;
    if (*(int *)(D_001AA534 + (a1 << 2)) != 0) goto L9430A;
L94308:;
    return;
L9430A:;
    inv_click_right_item(*(int *)(D_001AA534 + (a1 << 2)));
}
}

int inv_take_item(int a1)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_30 = a1 + 71;
    if (*(int *)(*(char **)((char *)a1 + 67) + 67) == *(int *)player_entity) goto L94A90;
    if (*(int *)inv_left_container == *(int *)wagon_container) goto L94A42;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) == 23) goto L94A40;
    l_24 = object_weight(a1);
    l_20 = object_weight(*(int *)player_entity);
    l_1C = carry_capacity() << 2;
    if ((l_24 + l_20) <= l_1C) goto L94A40;
    msgbox_show_string((int)D_001770CA, 1);
    return 0;
L94A40:;
    goto L94A90;
L94A42:;
    l_24 = object_weight(a1);
    *(signed char *)D_001962AE = 1;
    l_20 = object_weight(*(int *)wagon_container);
    *(signed char *)D_001962AE = 0;
    if ((l_24 + l_20) <= 3000) goto L94A90;
    msgbox_show_string((int)D_001770EA, 1);
    return 0;
L94A90:;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) != 23) goto L94AAB;
    if (*(short *)((char *)l_30 + 34) == 0) goto L94AAD;
L94AAB:;
    goto L94AC1;
L94AAD:;
    inv_store_item(a1);
    return 0;
L94AC1:;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) != 27) goto L94AE3;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 34)) == 8) goto L94AE5;
L94AE3:;
    goto L94AFB;
L94AE5:;
    inv_read_map_scrap(*(int *)inv_selected_item);
    return 0;
L94AFB:;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 54) goto L94B10;
    *(signed char *)((char *)a1) = 2;
L94B10:;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) != 3) goto L94B32;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 34)) == 18) goto L94B34;
L94B32:;
    goto L94B6A;
L94B34:;
    l_1C = (int)(unsigned char)*(signed char *)((char *)l_30 + 49);
    inv_merge_arrows(*(int *)player_entity, a1, 1);
    *(short *)(*(char **)D_00195AF4 + 115) = l_1C;
    return 0;
L94B6A:;
    l_28 = func_000CE44C(*(int *)player_character + 367, *(int *)inv_selected_item, 27);
    if (l_28 == 0) goto L94BD2;
    a1 = *(int *)((char *)l_28);
    item_remove_equip_effects(a1, (l_28 - (int)(*(char **)player_character + 367)) / 4);
    *(int *)((char *)l_28) = 0;
    return 0;
L94BD2:;
    *(int *)D_001AA454 = 0;
    *(int *)((char *)a1 + 31) = object_new_id(100);
    if (*(int *)((char *)a1 + 51) == 0) goto L94C06;
    *(int *)(*(char **)((char *)a1 + 51) + 31) = *(int *)((char *)a1 + 31);
L94C06:;
    func_00098F1D(a1);
    l_30 = a1 + 71;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) != 28) goto L94C32;
    if (*(short *)((char *)l_30 + 34) == 0) goto L94C34;
L94C32:;
    goto L94C58;
L94C34:;
    if (a1 == *(int *)inv_right_container) goto L94C4B;
    *(int *)D_001AA454 += *(int *)((char *)l_30 + 36);
L94C4B:;
    object_free_single(a1);
    goto L94CD5;
L94C58:;
    *(int *)((char *)a1 + 47) = 0;
    quest_raise_event(3, a1, 0);
    *(int *)((char *)a1 + 7) = *(int *)(*(char **)player_object + 7);
    *(int *)((char *)a1 + 11) = *(int *)(*(char **)player_object + 11);
    *(int *)((char *)a1 + 15) = *(int *)(*(char **)player_object + 15);
    if (*(int *)inv_left_container == *(int *)wagon_container) goto L94CBC;
    inv_store_item(a1);
    return 1;
L94CBC:;
    object_reparent(*(int *)inv_left_container, a1);
    return 0;
L94CD5:;
    if (*(int *)D_001AA454 == 0) goto L94D46;
    gold_add(*(int *)D_001AA454);
    *(signed char *)D_0012B508 = 144;
    func_000A0ED9(1237, (int)D_0017704C);
    mc_sprintf((int)text_buffer, *(int *)D_001832A4, *(int *)D_001AA454);
    msgbox_show_string((int)text_buffer, 1);
    sound_play(204, *(int *)player_object, 100);
    return 0;
L94D46:;
    return 0;
}

void func_00094D5A(void)
{
    int l_18;

    l_18 = func_0005FD36(1);
    *(int *)D_00195B84 = 0;
    object_foreach(*(int *)player_entity, (int)inv_sum_hidden_weight);
    *(signed char *)(*(char **)player_character + 544) = (*(int *)D_00195B84 * 100) / l_18;
}

void inv_unequip_item(int a1)
{
    int l_18;

    l_18 = 0;
L94DCC:;
    if (l_18 < 27) goto L94DDC;
    return;
L94DD4:;
    l_18++;
    goto L94DCC;
L94DDC:;
    if (*(int *)(*(char **)player_character + 367 + (l_18 << 2)) != a1) goto L94E19;
    item_remove_equip_effects(a1, l_18);
    *(int *)(*(char **)player_character + 367 + (l_18 << 2)) = 0;
    return;
L94E19:;
    goto L94DD4;
}

void inv_unequip_all_saved(void)
{
    int l_1C;
    int l_18;

    mc_memset((int)D_001AA4CC, 0, 108, (int)D_0017704C, 1274, 108);
    mc_memset((int)D_001AA460, 0, 108, (int)D_0017704C, 1275, 108);
    l_18 = 0;
L94E74:;
    if (l_18 < 27) goto L94E87;
    return;
L94E7F:;
    l_18++;
    goto L94E74;
L94E87:;
    if (*(int *)(*(char **)player_character + 367 + (l_18 << 2)) == 0) goto L94F22;
    l_1C = *(int *)(*(char **)player_character + 367 + (l_18 << 2)) + 71;
    if (((int)(short)*(short *)((char *)l_1C + 67)) == (-1)) goto L94F22;
    *(int *)(D_001AA4CC + (l_18 << 2)) = *(int *)(*(char **)player_character + 367 + (l_18 << 2));
    *(int *)(D_001AA460 + (l_18 << 2)) = (int)(unsigned short)*(short *)((char *)l_1C + 44);
    *(short *)((char *)l_1C + 44) = *(short *)((char *)l_1C + 46);
    inv_unequip_item(*(int *)(*(char **)player_character + 367 + (l_18 << 2)));
L94F22:;
    goto L94E7F;
}

void inv_reequip_saved(void)
{
    int l_1C;
    int l_18;

    l_18 = 0;
L94F46:;
    if (l_18 < 27) goto L94F56;
    return;
L94F4E:;
    l_18++;
    goto L94F46;
L94F56:;
    if (*(int *)(D_001AA4CC + (l_18 << 2)) == 0) goto L94F9C;
    inv_equip_item(*(int *)(D_001AA4CC + (l_18 << 2)));
    l_1C = *(int *)(D_001AA4CC + (l_18 << 2)) + 71;
    *(short *)((char *)l_1C + 44) = *(short *)(D_001AA460 + (l_18 << 2));
L94F9C:;
    goto L94F4E;
}

void inv_use_item(void)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    short l_18;

    l_1C = 0;
    l_28 = *(int *)inv_selected_item;
    l_30 = *(int *)inv_selected_item + 71;
    if (((int)(unsigned short)(*(short *)((char *)l_28 + 21) & 32)) == 0) goto L94FF9;
    msgbox_show_string((int)D_0017710F, 1);
    return;
L94FF9:;
    *(signed char *)((char *)l_30 + 43) |= 2;
    if (*(int *)((char *)l_28 + 51) == 0) goto L95013;
    *(signed char *)(*(char **)((char *)l_28 + 51) + 114) |= 2;
L95013:;
    *(signed char *)D_001940D8 |= 8;
    if (((int)(short)*(short *)((char *)l_30 + 67)) != 26) goto L95032;
    if (((int)(short)*(short *)((char *)l_30 + 69)) == 3) goto L95034;
L95032:;
    goto L95052;
L95034:;
    cast_item_used_spell(92);
    item_damage(*(int *)inv_selected_item, 50);
    return;
L95052:;
    if (((int)(short)*(short *)((char *)l_30 + 67)) != 26) goto L9506A;
    if (((int)(short)*(short *)((char *)l_30 + 69)) == 4) goto L9506C;
L9506A:;
    goto L950D2;
L9506C:;
    if (*(int *)creature_count != 0) goto L95084;
    hud_message_add((int)D_00177129);
    return;
L95084:;
    *(signed char *)D_0019629D = 1;
    l_24 = monster_summon_near_player(27);
    *(signed char *)D_0019629D = 0;
    if (l_24 != 0) goto L950B4;
    hud_message_add((int)D_00177147);
    return;
L950B4:;
    *(signed char *)((char *)l_24 + 135) |= 2;
    item_damage(*(int *)inv_selected_item, 100);
    return;
L950D2:;
    if (((int)(short)*(short *)((char *)l_30 + 67)) != 26) goto L950EA;
    if (((int)(short)*(short *)((char *)l_30 + 69)) == 8) goto L950EC;
L950EA:;
    goto L9515D;
L950EC:;
    if (*(int *)creature_count != 0) goto L95104;
    hud_message_add((int)D_0017716F);
    return;
L95104:;
    *(signed char *)D_0019629D = 1;
    l_24 = monster_summon_near_player((int)(unsigned char)*(signed char *)(*(char **)D_00190504 + 577));
    *(signed char *)D_0019629D = 0;
    if (l_24 != 0) goto L9513F;
    hud_message_add((int)D_0017718D);
    return;
L9513F:;
    *(signed char *)((char *)l_24 + 135) |= 2;
    item_damage(*(int *)inv_selected_item, 100);
    return;
L9515D:;
    if (((int)(short)*(short *)((char *)l_30 + 67)) != 26) goto L95175;
    if (((int)(short)*(short *)((char *)l_30 + 69)) == 5) goto L95177;
L95175:;
    goto L95190;
L95177:;
    sheet_open(50);
    object_delete(*(int *)inv_selected_item);
    return;
L95190:;
    if (((int)(short)*(short *)((char *)l_30 + 67)) != 26) goto L951A8;
    if (((int)(short)*(short *)((char *)l_30 + 69)) == 9) goto L951AA;
L951A8:;
    goto L951E7;
L951AA:;
    if (*(int *)(*(char **)inv_selected_item + 63) == 0) goto L951D3;
    object_delete(*(int *)(*(char **)inv_selected_item + 63));
    msgbox_show_rsc(32, 1);
    goto L951E2;
L951D3:;
    msgbox_show_rsc(20, 1);
L951E2:;
    return;
L951E7:;
    if (((int)(short)*(short *)((char *)l_30 + 67)) == (-1)) goto L952D3;
    l_20 = 0;
L951FE:;
    if (l_20 >= 10) goto L95216;
    if (((int)(short)*(short *)((char *)((l_20 << 2) + l_30) + 67)) != (-1)) goto L9521B;
L95216:;
    goto L952D3;
L9521B:;
    if (*(short *)((char *)((l_20 << 2) + l_30) + 67) != 0) goto L95274;
    *(int *)&l_18 = (int)(short)*(short *)spell_last_cast_id;
    *(signed char *)D_0019629A = 1;
    cast_item_used_spell((int)(short)*(short *)((char *)((l_20 << 2) + l_30) + 69));
    *(signed char *)D_0019629A = 1;
    l_1C = 1;
    *(short *)spell_last_cast_id = *(int *)&l_18;
    item_damage(*(int *)inv_selected_item, 10);
L95274:;
    if (((int)(short)*(short *)((char *)((l_20 << 2) + l_30) + 67)) != 21) goto L95296;
    if (*(short *)((char *)((l_20 << 2) + l_30) + 69) == 0) goto L95298;
L95296:;
    goto L952C8;
L95298:;
    damage_apply(*(int *)player_entity, (int)&*(signed char *)((char *)(((int)(unsigned char)*(signed char *)(*(char **)player_character + 129)) >> 1) + 1), 0);
    item_damage(*(int *)inv_selected_item, 2);
L952C8:;
    l_20++;
    goto L951FE;
L952D3:;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) != 9) goto L952F5;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 34)) == 5) goto L952F7;
L952F5:;
    goto L95301;
L952F7:;
    if (*(short *)((char *)l_30 + 63) != 0) goto L95303;
L95301:;
    goto L95347;
L95303:;
    if (func_00098B91(*(int *)inv_selected_item) == 0) goto L95329;
    msgbox_show_quest_text(*(int *)current_quest, (int)(short)*(short *)((char *)l_30 + 63), 1);
    goto L95342;
L95329:;
    msgbox_show_qrc_text(l_30 + 10, (int)(unsigned short)*(short *)((char *)l_30 + 63), 1);
L95342:;
    return;
L95347:;
    if (*(short *)((char *)l_30 + 32) != 0) goto L9537C;
    func_00065A8C(*(int *)player_entity, ((int)(unsigned short)*(short *)((char *)l_30 + 34)) + 136, 1);
    object_delete(l_28);
    return;
L9537C:;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) != 27) goto L95397;
    if (*(short *)((char *)l_30 + 34) == 0) goto L95399;
L95397:;
    goto L953DE;
L95399:;
    if (*(int *)((char *)l_28 + 63) != 0) goto L953B3;
    msgbox_show_rsc(12, 1);
    goto L953D9;
L953B3:;
    if (*(signed char *)mouse_buttons == 0) goto L953C3;
    func_0012B136();
    goto L953B3;
L953C3:;
    inventory_close();
    spellbook_open(1);
    *(signed char *)D_001940D8 |= 128;
L953D9:;
    return;
L953DE:;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) != 7) goto L9541C;
L953EF:;
    if (*(signed char *)mouse_buttons == 0) goto L953FF;
    func_0012B136();
    goto L953EF;
L953FF:;
    inventory_close();
    book_open((int)(short)*(short *)((char *)l_30 + 63));
    *(signed char *)D_001940D8 |= 128;
    return;
L9541C:;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) == 6) goto L9543E;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) != 12) goto L9544B;
L9543E:;
    func_0005E722(l_30);
    return;
L9544B:;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 32)) != 1) goto L9546D;
    if (((int)(unsigned short)*(short *)((char *)l_30 + 34)) == 1) goto L9546F;
L9546D:;
    goto L95478;
L9546F:;
    if (*(int *)((char *)l_28 + 63) != 0) goto L9547A;
L95478:;
    goto L9548C;
L9547A:;
    if (((int)(unsigned char)*(signed char *)(*(char **)((char *)l_28 + 63))) == 31) goto L9548E;
L9548C:;
    goto L954A3;
L9548E:;
    potion_drink(*(int *)((char *)l_28 + 63));
    object_delete(l_28);
    return;
L954A3:;
    if (l_1C != 0) goto L954B2;
    if (*(signed char *)((char *)l_28 + 38) == 0) goto L954B4;
L954B2:;
    return;
L954B4:;
    *(short *)D_00195F2E = 30;
    *(signed char *)D_0012B508 = 146;
    msgbox_show_string(*(int *)D_00184221, 1);
}

void inv_item_info(int a1, int a2)
{
    int l_14;

    *(signed char *)D_0012B508 = 146;
    *(int *)D_00195AA8 = a1;
    *(int *)D_00195A80 = a2;
    if (((int)(short)*(short *)((char *)a2 + 67)) != 26) goto L9551F;
    if (((int)(short)*(short *)((char *)a2 + 69)) == 9) goto L95521;
L9551F:;
    goto L95535;
L95521:;
    msgbox_show_rsc(1004, 1);
    goto L9587E;
L95535:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 27) goto L95557;
    if (((int)(unsigned short)*(short *)((char *)a2 + 34)) == 4) goto L95559;
L95557:;
    goto L9559B;
L95559:;
    *(int *)D_00195ACC = ((int)potion_recipes) + (((int)(unsigned char)*(signed char *)((char *)a2 + 49)) * 109);
    msgbox_show_string((int)D_001771B5, 1);
    msgbox_show_string(func_000993CB(*(int *)D_00195ACC), 1);
    goto L9587E;
L9559B:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 27) goto L955BD;
    if (((int)(unsigned short)*(short *)((char *)a2 + 34)) == 6) goto L955BF;
L955BD:;
    goto L955D3;
L955BF:;
    msgbox_show_rsc(1073, 1);
    goto L9587E;
L955D3:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 13) goto L955F1;
    item_info_painting(a2);
    goto L9587E;
L955F1:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 7) goto L95659;
    if (((int)(short)*(short *)((char *)a2 + 67)) != 26) goto L9561F;
    msgbox_show_rsc(1015, 1);
    goto L95654;
L9561F:;
    l_14 = *(int *)D_00195C44 + 63000;
    book_read_header(l_14, (int)(unsigned short)*(short *)((char *)a2 + 63));
    *(int *)text_macro_book = l_14;
    msgbox_show_rsc(1009, 1);
L95654:;
    goto L9587E;
L95659:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 2) goto L956A6;
    if (((int)(unsigned short)(*(short *)(*(char **)D_00195A80 + 42) & 2048)) == 0) goto L95692;
    msgbox_show_rsc(1014, 1);
    goto L956A1;
L95692:;
    msgbox_show_rsc(1000, 1);
L956A1:;
    goto L9587E;
L956A6:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 3) goto L9575C;
    if (*(int *)((char *)a1 + 63) == 0) goto L956D8;
    msgbox_show_rsc(1005, 1);
    goto L95757;
L956D8:;
    if (((int)(unsigned short)*(short *)(*(char **)D_00195A80 + 34)) != 18) goto L956FC;
    msgbox_show_rsc(1011, 1);
    goto L95757;
L956FC:;
    if (((int)(unsigned short)(*(short *)(*(char **)D_00195A80 + 42) & 2048)) == 0) goto L95748;
    if (((int)(unsigned short)*(short *)(*(char **)D_00195A80 + 32)) != 3) goto L95737;
    msgbox_show_rsc(1012, 1);
    goto L95746;
L95737:;
    msgbox_show_rsc(1013, 1);
L95746:;
    goto L95757;
L95748:;
    msgbox_show_rsc(1001, 1);
L95757:;
    goto L9587E;
L9575C:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 1) goto L95776;
    if (*(int *)((char *)a1 + 63) != 0) goto L95778;
L95776:;
    goto L9578A;
L95778:;
    if (((int)(unsigned char)*(signed char *)(*(char **)((char *)a1 + 63))) == 11) goto L9578C;
L9578A:;
    goto L957A0;
L9578C:;
    msgbox_show_rsc(1006, 1);
    goto L9587E;
L957A0:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 1) goto L957C2;
    if (((int)(unsigned short)*(short *)((char *)a2 + 34)) == 1) goto L957C4;
L957C2:;
    goto L957CD;
L957C4:;
    if (*(int *)((char *)a1 + 63) != 0) goto L957CF;
L957CD:;
    goto L957E1;
L957CF:;
    if (((int)(unsigned char)*(signed char *)(*(char **)((char *)a1 + 63))) == 31) goto L957E3;
L957E1:;
    goto L95805;
L957E3:;
    *(int *)D_00195ACC = *(int *)((char *)a1 + 63) + 71;
    msgbox_show_rsc(1008, 1);
    goto L9587E;
L95805:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 27) goto L95827;
    if (((int)(unsigned short)*(short *)((char *)a2 + 34)) == 1) goto L95829;
L95827:;
    goto L9583A;
L95829:;
    msgbox_show_rsc(1004, 1);
    goto L9587E;
L9583A:;
    if (((int)(unsigned short)*(short *)((char *)a2 + 32)) != 27) goto L9585C;
    if (((int)(unsigned short)*(short *)((char *)a2 + 34)) == 2) goto L9585E;
L9585C:;
    goto L9586F;
L9585E:;
    msgbox_show_rsc(1007, 1);
    goto L9587E;
L9586F:;
    msgbox_show_rsc(1003, 1);
L9587E:;
    if (*(signed char *)mouse_buttons == 0) goto L9588E;
    func_0012B136();
    goto L9587E;
L9588E:;
    *(signed char *)mouse_buttons_prev = 0;
    if (((int)(short)*(short *)((char *)a2 + 67)) == (-1)) goto L958B0;
    msgbox_show_rsc(1016, 1);
L958B0:;
    if (*(signed char *)mouse_buttons == 0) goto L958C0;
    func_0012B136();
    goto L958B0;
L958C0:;
    *(signed char *)mouse_buttons_prev = 0;
}

void inv_list_left_item(int a1, int a2)
{
    int l_14;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) == 50) goto L95901;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) goto L95916;
L95901:;
    if (((int)(unsigned short)(*(short *)((char *)a1 + 21) & 2)) == 0) goto L9591B;
L95916:;
    return;
L9591B:;
    l_14 = a1 + 71;
    if (*(int *)guild_membership == 0) goto L9593F;
    if (((int)(unsigned char)*(signed char *)(*(char **)guild_membership + 2)) == 3) goto L95941;
L9593F:;
    goto L959A0;
L95941:;
    if (*(int *)trade_mode != 2) goto L95956;
    if (((int)(short)*(short *)((char *)l_14 + 67)) == (-1)) goto L95958;
L95956:;
    goto L9595D;
L95958:;
    return;
L9595D:;
    if (((struct bf8_2_1 *)&D_001940D8)->f != 0) goto L95981;
    if (func_000CE44C(*(int *)player_character + 367, a1, 27) != 0) goto L95983;
L95981:;
    goto L95994;
L95983:;
    if (((int)(unsigned short)*(short *)((char *)l_14 + 32)) != 1) goto L95996;
L95994:;
    goto L9599B;
L95996:;
    return;
L9599B:;
    goto L95AF2;
L959A0:;
    if (*(int *)trade_mode != 2) goto L959BE;
    if (func_000990F0((int)(unsigned short)*(short *)((char *)l_14 + 32)) == 0) goto L959C0;
L959BE:;
    goto L959C5;
L959C0:;
    return;
L959C5:;
    if (((struct bf8_1_1 *)&D_001940D4)->f == 0) goto L959DE;
    if (*(short *)((char *)l_14 + 44) == *(short *)((char *)l_14 + 46)) goto L959E0;
L959DE:;
    goto L959E5;
L959E0:;
    return;
L959E5:;
    if (((struct bf8_1_1 *)&D_001940D4)->f == 0) goto L959FA;
    if (((int)(short)*(short *)((char *)l_14 + 67)) != (-1)) goto L959FC;
L959FA:;
    goto L95A01;
L959FC:;
    return;
L95A01:;
    if (((int)(unsigned char)*(signed char *)game_mode) == 10) goto L95A2A;
    if (((int)(unsigned char)*(signed char *)D_0019626F) != 10) goto L95A25;
    if (((int)(unsigned char)*(signed char *)game_mode) == 8) goto L95A2A;
L95A25:;
    goto L95AAD;
L95A2A:;
    if (((int)(short)*(short *)((char *)l_14 + 67)) != (-1)) goto L95A40;
    if (*(short *)((char *)l_14 + 61) != 0) goto L95A42;
L95A40:;
    goto L95A4C;
L95A42:;
    if (*(int *)itemmaker_item_object != a1) goto L95A4E;
L95A4C:;
    goto L95A72;
L95A4E:;
    if (((int)(unsigned short)*(short *)((char *)l_14 + 32)) != 3) goto L95A70;
    if (((int)(unsigned short)*(short *)((char *)l_14 + 34)) == 18) goto L95A72;
L95A70:;
    goto L95A74;
L95A72:;
    goto L95A98;
L95A74:;
    if (((int)(unsigned short)*(short *)((char *)l_14 + 32)) != 27) goto L95A96;
    if (((int)(unsigned short)*(short *)((char *)l_14 + 34)) == 1) goto L95A98;
L95A96:;
    goto L95A9A;
L95A98:;
    goto L95AAB;
L95A9A:;
    if (((int)(unsigned short)*(short *)((char *)l_14 + 32)) != 23) goto L95AAD;
L95AAB:;
    goto L95AAF;
L95AAD:;
    goto L95AB4;
L95AAF:;
    return;
L95AB4:;
    if (((struct bf8_2_1 *)&D_001940D8)->f != 0) goto L95AD8;
    if (func_000CE44C(*(int *)player_character + 367, a1, 27) != 0) goto L95ADA;
L95AD8:;
    goto L95AEB;
L95ADA:;
    if (((int)(unsigned short)*(short *)((char *)l_14 + 32)) != 1) goto L95AED;
L95AEB:;
    goto L95AF2;
L95AED:;
    return;
L95AF2:;
    if (((int)(short)*(short *)D_001AA586) < *(int *)inv_left_scroll) goto L95B14;
    if (((int)(short)*(short *)D_001AA586) < (*(int *)inv_left_scroll + 4)) goto L95B16;
L95B14:;
    goto L95B59;
L95B16:;
    *(int *)(inv_left_rows + ((((int)(short)*(short *)D_001AA586) - *(int *)inv_left_scroll) << 2)) = a1;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) == 50) goto L95B59;
    inv_draw_item_cell(a1, (int)(short)(*(short *)D_001AA586 - *(short *)inv_left_scroll), a2);
L95B59:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 10) goto L95B71;
    if (((int)(short)*(short *)((char *)l_14 + 67)) != (-1)) return;
L95B71:;
    (*(short *)D_001AA586)++;
}

void inv_equip_in_slot_pair(int a1, int a2, int a3)
{
    if (*(int *)(*(char **)player_character + 367 + (a2 << 2)) == 0) goto L967E6;
    if (*(int *)(*(char **)player_character + 367 + ((a2 + a3) << 2)) == 0) goto L967D6;
    inv_equip_in_slot(a1, a2);
    goto L967E4;
L967D6:;
    inv_equip_in_slot(a1, a2 + a3);
L967E4:;
    return;
L967E6:;
    inv_equip_in_slot(a1, a2);
}

void inv_unequip_slot(int a1)
{
    item_remove_equip_effects(*(int *)(*(char **)player_character + 367 + (a1 << 2)), a1);
    *(int *)(*(char **)player_character + 367 + (a1 << 2)) = 0;
}

void inv_equip_in_slot(int a1, int a2)
{
    if (*(int *)(*(char **)player_character + 367 + (a2 << 2)) == 0) goto L968BE;
    item_remove_equip_effects(*(int *)(*(char **)player_character + 367 + (a2 << 2)), a2);
    *(int *)(*(char **)player_character + 367 + (a2 << 2)) = a1;
    quest_raise_event(3, a1, 0);
    item_apply_equip_effects(a1, a2);
    return;
L968BE:;
    *(int *)(*(char **)player_character + 367 + (a2 << 2)) = a1;
    quest_raise_event(3, a1, 0);
    item_apply_equip_effects(a1, a2);
}

int func_000968F8(int a1)
{
    int l_1C;

    if (a1 != 0) goto L96918;
    return 0;
L96918:;
    l_1C = a1 + 71;
    return ((((int)(unsigned short)(*(short *)((char *)l_1C + 42) & 4)) == 0) ? 1 : 0);
}

void func_00096997(void)
{
    int l_18;

    *(int *)D_00190CA8 = 0;
    *(int *)D_00195D2C = 0;
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)trade_add_buy_price);
    *(int *)D_00195D2C = func_00097BD9(*(int *)D_00195D2C);
    *(int *)D_00195D30 = ((*(int *)D_00195D30 = trade_adjust_price(*(int *)D_00195D2C, 0)) * *(int *)trade_price_scale) / 256;
}

int func_00096A14(void)
{
    int l_20;
    int l_1C;

    *(int *)D_00195D2C = 0;
    object_foreach(*(int *)(*(char **)inv_right_container + 63), (int)trade_add_repair_cost);
    if (*(int *)D_00195D2C <= 0) goto L96A51;
    l_20 = *(int *)D_00195D2C;
    goto L96A58;
L96A51:;
    l_20 = 1;
L96A58:;
    *(int *)D_00195D2C = func_00097BD9((*(int *)D_00195D2C = l_20));
    *(int *)D_00195D30 = ((*(int *)D_00195D30 = trade_adjust_price(*(int *)D_00195D2C, 0)) * *(int *)trade_price_scale) / 256;
    return *(int *)D_00195D2C;
}

void inv_close_return_unpaid(void)
{
    int l_18;

    *(int *)free_later_count = 0;
    object_foreach_pre(*(int *)(*(char **)player_entity + 63), (int)inv_return_unpaid_item);
    if (*(signed char *)D_0019628A == 0) goto L96AF9;
    object_foreach_pre(*(int *)(*(char **)player_entity + 63), (int)inv_store_callback);
L96AF9:;
    l_18 = 0;
L96B00:;
    if (l_18 < 27) goto L96B10;
    goto L96B68;
L96B08:;
    l_18++;
    goto L96B00;
L96B10:;
    if (*(int *)(*(char **)player_character + 367 + (l_18 << 2)) == 0) goto L96B4C;
    if (((int)(unsigned short)(*(short *)(*(char **)(*(char **)player_character + 367 + (l_18 << 2)) + 21) & 32)) != 0) goto L96B4E;
L96B4C:;
    goto L96B66;
L96B4E:;
    *(int *)(*(char **)player_character + 367 + (l_18 << 2)) = 0;
L96B66:;
    goto L96B08;
L96B68:;
    object_free_pending();
}

void item_remove_equip_effects(int a1, int a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = 0;
    l_18 = a1 + 71;
L96B9A:;
    if (l_1C >= 10) goto L96BB2;
    if (((int)(short)*(short *)((char *)((l_1C << 2) + l_18) + 67)) != (-1)) goto L96BB7;
L96BB2:;
    return;
L96BB7:;
    switch (*(unsigned short *)((char *)((l_1C << 2) + l_18) + 67)) {
case 1:
    l_14 = spell_find_on_entity(*(int *)player_entity, (int)(short)*(short *)((char *)((l_1C << 2) + l_18) + 69), a2 + 200);
    if (l_14 == 0) goto L96C38;
    spell_end(l_14);
L96C38:;
    goto L96CBB;
case 3:
    if (*(signed char *)((char *)l_18 + 48) == 0) goto L96C8B;
    *(short *)(*(char **)player_character + 141) -= (unsigned short)(unsigned char)*(signed char *)((char *)l_18 + 48);
    *(short *)(*(char **)player_character + 143) -= (unsigned short)(unsigned char)*(signed char *)((char *)l_18 + 48);
    if (*(short *)(*(char **)player_character + 141) >= 0) goto L96C8B;
    *(short *)(*(char **)player_character + 141) = 0;
L96C8B:;
    goto L96CBB;
case 9:
    *(signed char *)(*(char **)player_character + 138) &= 253;
    goto L96CBB;
case 10:
    *(short *)(*(char **)player_character + 157 + (((int)(short)*(short *)((char *)((l_1C << 2) + l_18) + 69)) * 6)) -= 15;
default:
L96CBB:;
    l_1C++;
    goto L96B9A;
}
}

void item_repair_cb(int a1)
{
    int l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) return;
    if (*(int *)D_00195B84 == 0) return;
    l_18 = a1 + 71;
    if (((int)(unsigned short)*(short *)((char *)l_18 + 32)) != 3) goto L96F28;
    if (((int)(unsigned short)*(short *)((char *)l_18 + 34)) == 18) goto L96F2A;
L96F28:;
    goto L96F2C;
L96F2A:;
    return;
L96F2C:;
    if (((int)(short)*(short *)((char *)l_18 + 67)) == (-1)) goto L96F41;
    if (*(int *)cfg_magic_repair == 0) goto L96F43;
L96F41:;
    goto L96F45;
L96F43:;
    return;
L96F45:;
    if (*(short *)((char *)l_18 + 44) == *(short *)((char *)l_18 + 46)) return;
    *(short *)((char *)l_18 + 44) += *(short *)D_00195B84;
    if (*(unsigned short *)((char *)l_18 + 44) <= *(unsigned short *)((char *)l_18 + 46)) goto L96F81;
    *(short *)((char *)l_18 + 44) = *(short *)((char *)l_18 + 46);
L96F81:;
    *(int *)D_00195B84 = 0;
}

void item_break(int a1)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = a1 + 71;
    l_18 = -1;
    *(int *)D_00195A80 = l_20;
    *(short *)((char *)l_20 + 44) = 0;
    inv_store_item(a1);
    func_000A0ED9(2157, (int)D_0017704C);
    mc_sprintf((int)text_buffer, (int)D_001771C5, l_20);
    parse_expand((int)text_buffer, (int)D_00190B44);
    hud_message_add((int)D_00190B44);
    l_1C = 0;
L97017:;
    if (l_1C < 27) goto L97027;
    goto L97048;
L9701F:;
    l_1C++;
    goto L97017;
L97027:;
    if (*(int *)(*(char **)player_character + 367 + (l_1C << 2)) != a1) goto L97046;
    l_18 = l_1C;
L97046:;
    goto L9701F;
L97048:;
    if (((int)(short)*(short *)((char *)l_20 + 67)) == (-1)) goto L970CF;
    if (l_18 == (-1)) goto L97069;
    item_remove_equip_effects(a1, l_18);
L97069:;
    l_1C = 0;
L97070:;
    if (l_1C >= 10) goto L97088;
    if (((int)(short)*(short *)((char *)((l_1C << 2) + l_20) + 67)) != (-1)) goto L9708A;
L97088:;
    goto L970C7;
L9708A:;
    if (((int)(short)*(short *)((char *)((l_1C << 2) + l_20) + 67)) != 15) goto L970BF;
    hud_message_add((int)D_001771D3);
    *(signed char *)((char *)monster_summon_near_player((int)(short)*(short *)((char *)((l_1C << 2) + l_20) + 69)) + 624) = 1;
L970BF:;
    l_1C++;
    goto L97070;
L970C7:;
    object_delete(a1);
L970CF:;
    if (l_18 == (-1)) goto L970ED;
    *(int *)(*(char **)player_character + 367 + (l_18 << 2)) = 0;
L970ED:;
    weapon_reload_hand_sprites();
    player_refresh_paperdoll();
}

void inv_store_item(int a1)
{
    int l_1C;
    int l_18;

    *(int *)((char *)a1 + 31) = object_new_id(100);
    if (*(int *)((char *)a1 + 51) == 0) goto L9713C;
    *(int *)(*(char **)((char *)a1 + 51) + 31) = *(int *)((char *)a1 + 31);
L9713C:;
    func_00098F1D(a1);
    *(int *)((char *)a1 + 47) = 0;
    l_1C = a1 + 71;
    if (((int)(short)*(short *)((char *)l_1C + 67)) != (-1)) goto L97180;
    if (((int)(unsigned short)*(short *)((char *)l_1C + 32)) != 27) goto L9717E;
    if (*(short *)((char *)l_1C + 34) == 0) goto L97180;
L9717E:;
    goto L9718F;
L97180:;
    object_reparent(*(int *)D_001959DC, a1);
    return;
L9718F:;
    object_reparent(*(int *)(inventory_containers + (((int)(unsigned char)*(signed char *)(item_group_tab + ((int)(unsigned short)*(short *)((char *)l_1C + 32)))) << 2)), a1);
}

void func_000971C1(int a1)
{
    int l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) return;
    l_18 = a1 + 71;
    if (((int)(unsigned short)*(short *)((char *)l_18 + 32)) != 23) goto L97205;
    if (*(short *)((char *)l_18 + 34) == 0) goto L97207;
L97205:;
    return;
L97207:;
    (*(signed char *)itemmaker_slot_kinds)++;
    *(int *)guild_npc_object = a1;
}

void inv_create_wagon(void)
{
    if (*(int *)wagon_container != 0) return;
    *(signed char *)((char *)(*(int *)wagon_container = object_create_child(*(int *)player_entity, 0, 0))) = 52;
    *(short *)(*(char **)wagon_container + 21) = 3;
    *(short *)(*(char **)wagon_container + 27) = 4;
}

void func_00097271(void)
{
    *(signed char *)itemmaker_slot_kinds = 0;
    object_foreach(*(int *)player_entity, (int)func_000971C1);
    if (*(signed char *)itemmaker_slot_kinds != 0) goto L972A7;
    if (*(int *)wagon_container != 0) goto L972A9;
L972A7:;
    return;
L972A9:;
    object_delete(*(int *)wagon_container);
    *(int *)wagon_container = 0;
}

void inv_merge_arrows(int a1, int a2, int a3)
{
    int l_14;
    int l_10;

    *(int *)D_00195AF4 = 0;
    object_find(*(int *)((char *)a1 + 63), (int)inv_match_arrows);
    if (*(int *)D_00195AF4 != 0) goto L97367;
    l_14 = object_create_child(a1, 0, 107);
    *(int *)D_00195AF4 = l_14;
    *(signed char *)((char *)l_14) = 2;
    *(short *)((char *)l_14 + 29) = 998;
    *(short *)((char *)l_14 + 27) = 0;
    item_make(3, 18, l_14 + 71);
    *(signed char *)((char *)l_14 + 120) = *(signed char *)((char *)a2 + 120);
    if (a1 != *(int *)player_entity) goto L97365;
    inv_store_item(l_14);
L97365:;
    goto L9739D;
L97367:;
    l_10 = ((int)(unsigned char)*(signed char *)((char *)*(int *)D_00195AF4 + 120)) + ((int)(unsigned char)*(signed char *)((char *)a2 + 120));
    if (l_10 < 200) goto L97391;
    l_10 = 199;
L97391:;
    *(signed char *)(*(char **)D_00195AF4 + 120) = *(signed char *)&l_10;
L9739D:;
    if (a3 == 0) return;
    object_delete(a2);
}

int func_00097764(void)
{
    int l_24;
    int l_20;
    int l_1C;

    l_1C = 0;
    l_24 = *(int *)(*(char **)D_00195B34 + 63);
L97784:;
    if (l_24 == 0) goto L977BC;
    if (((int)(unsigned short)(*(short *)((char *)l_24 + 21) & 32)) != 0) goto L977B1;
    l_20 = l_24 + 71;
    l_1C += *(int *)((char *)l_20 + 36);
L977B1:;
    l_24 = *(int *)((char *)l_24 + 55);
    goto L97784;
L977BC:;
    *(int *)D_00195D2C = func_00097BD9(l_1C);
    *(int *)D_00195D30 = trade_adjust_price(*(int *)D_00195D2C, 1);
    return *(int *)D_00195D2C;
}

int func_000977F2(int a1)
{
    a1 = (((int)(unsigned short)*(short *)(region_price_adjustment + (((int)(unsigned char)*(signed char *)current_region) * 80))) * a1) / 1000;
    if (a1 >= 0) goto L9783B;
    a1 = 1;
L9783B:;
    return a1;
}

int trade_adjust_price(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = ((((int)(unsigned char)*(signed char *)(*(char **)current_building + 25)) - 10) * 5) + 50;
    l_20 = ((((int)(unsigned char)*(signed char *)(*(char **)current_building + 25)) - 10) * 5) + 50;
    if (a2 != 0) goto L9798A;
    l_1C = ((((l_24 << 8) / 200) + 128) * ((int)&*(signed char *)((char *)(((100 - ((int)(short)*(short *)(*(char **)player_character + 241))) << 8) / 200) + 128))) / 256;
    l_18 = ((((l_20 << 8) / 200) + 128) * ((int)&*(signed char *)((char *)(((100 - ((int)(short)*(short *)(*(char **)player_character + 42))) << 8) / 200) + 128))) / 256;
    a1 = (a1 * (((l_1C * 192) / 256) + ((l_18 << 6) / 256))) / 256;
    goto L97A6B;
L9798A:;
    l_1C = (((((100 - l_24) << 8) / 200) + 128) * ((int)&*(signed char *)((char *)((((int)(short)*(short *)(*(char **)player_character + 241)) << 8) / 200) + 128))) / 256;
    l_18 = (((((100 - l_20) << 8) / 200) + 128) * ((int)&*(signed char *)((char *)((((int)(short)*(short *)(*(char **)player_character + 42)) << 8) / 200) + 128))) / 256;
    a1 = (a1 * (((l_1C * 179) / 256) + ((l_18 * 51) / 256))) / 256;
L97A6B:;
    *(int *)D_00195D30 = a1;
    return a1;
}

void func_00097A85(void)
{
    int l_1C;
    int l_18;

    *(signed char *)D_001AA5F7 = 1;
    l_18 = holiday_today(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region);
    if (*(int *)trade_mode != 4) goto L97ABE;
    if (l_18 == 43) goto L97AC7;
L97ABE:;
    if (((struct bf8_7_1 *)&player_motion_flags)->f == 0) goto L97AC9;
L97AC7:;
    return;
L97AC9:;
    if ((*(int *)D_00195D2C >> 1) <= *(int *)D_00195D30) goto L97AE1;
    l_1C = 260;
    goto L97B0B;
L97AE1:;
    if ((*(int *)D_00195D2C - (*(int *)D_00195D2C >> 2)) <= *(int *)D_00195D30) goto L97B04;
    l_1C = 261;
    goto L97B0B;
L97B04:;
    l_1C = 262;
L97B0B:;
    if (*(int *)trade_mode != 2) goto L97B18;
    l_1C += 3;
L97B18:;
    msgbox_yes_no_rsc(l_1C);
}

int func_00097B2A(void)
{
    int l_1C;

    if (*(signed char *)D_001AA5F7 == 0) goto L97BC5;
    l_1C = holiday_today(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region);
    if (*(int *)trade_mode != 4) goto L97B69;
    if (l_1C == 43) goto L97B72;
L97B69:;
    if (((struct bf8_7_1 *)&player_motion_flags)->f == 0) goto L97B7B;
L97B72:;
    return 1;
L97B7B:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 8) goto L97B90;
    return -1;
L97B90:;
    *(signed char *)D_001AA5F7 = 0;
    if (((int)(unsigned char)*(signed char *)D_00196271) != 2) goto L97BAC;
    return -1;
L97BAC:;
    skill_add_uses(14, 1);
    return *(int *)D_00195D30;
L97BC5:;
    return 0;
}

int func_00097BD9(int a1)
{
    a1 = func_000977F2(a1);
    a1 += ((((int)(unsigned char)*(signed char *)(*(char **)current_building + 25)) - 10) * a1) / 100;
    a1 += a1;
    return a1;
}

void shop_quality_message(int a1)
{
    short l_18;

    *(signed char *)D_0012B508 = 146;
    if (((int)(unsigned char)*(signed char *)((char *)a1 + 25)) > 3) goto L97C5F;
    *(int *)&l_18 = 270;
    goto L97CB1;
L97C5F:;
    if (((int)(unsigned char)*(signed char *)((char *)a1 + 25)) > 7) goto L97C78;
    *(int *)&l_18 = 269;
    goto L97CB1;
L97C78:;
    if (((int)(unsigned char)*(signed char *)((char *)a1 + 25)) > 13) goto L97C91;
    *(int *)&l_18 = 268;
    goto L97CB1;
L97C91:;
    if (((int)(unsigned char)*(signed char *)((char *)a1 + 25)) > 17) goto L97CAA;
    *(int *)&l_18 = 267;
    goto L97CB1;
L97CAA:;
    *(int *)&l_18 = 266;
L97CB1:;
    msgbox_show_rsc((int)(short)l_18, 1);
}

void inv_wagon_button(void)
{
    int l_1C;
    int l_18;

    if (*(int *)wagon_container == 0) return;
    if (*(signed char *)D_001962B1 != 0) goto L97CF9;
    if (((int)(unsigned char)*(signed char *)player_environment) == 3) goto L97CFB;
L97CF9:;
    goto L97CFD;
L97CFB:;
    return;
L97CFD:;
    *(signed char *)inv_right_icon = 3;
    if (*(int *)trade_mode != 0) goto L97D46;
    if (*(int *)wagon_container != *(int *)inv_right_container) goto L97D30;
    *(int *)D_00195B34 = (*(int *)inv_right_container = *(int *)D_001AA53C);
    goto L97D44;
L97D30:;
    *(int *)D_00195B34 = (*(int *)inv_right_container = *(int *)wagon_container);
L97D44:;
    return;
L97D46:;
    *(int *)inv_left_container = *(int *)wagon_container;
}

void inv_info_button(void)
{
    *(int *)inventory_action = 1;
}

void inv_equip_button(void)
{
    *(int *)inventory_action = 2;
}

void inv_remove_button(void)
{
    *(int *)inventory_action = 3;
}

void inv_use_button(void)
{
    *(int *)inventory_action = 4;
}

void inv_gold_button(void)
{
    if (*(int *)(*(char **)player_character + 133) == 0) return;
    inpstr_begin_number(0);
    msgbox_show_rsc(25, 2);
    if (*(int *)inpstr_result < 1) return;
    if (((unsigned)*(int *)inpstr_result) > *(int *)(*(char **)player_character + 133)) return;
    *(int *)(*(char **)player_character + 133) -= *(int *)inpstr_result;
}

void trade_steal_button(void)
{
    int l_18;

    l_18 = (((int)(short)*(short *)(*(char **)player_character + 247)) - (*(int *)D_00195D2C / 32)) - (*(int *)D_00190CA8 / 4);
    if ((rand() % 101) <= l_18) goto L97EE4;
    skill_add_uses(16, 1);
    inventory_close();
    *(signed char *)crime_current = 13;
    guards_summon(1);
    hud_message_add((int)D_001771FF);
    return;
L97EE4:;
    hud_message_add((int)D_00177217);
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)inv_claim_item);
    l_18 = 0;
L97F07:;
    if (l_18 < 27) goto L97F17;
    goto L97F60;
L97F0F:;
    l_18++;
    goto L97F07;
L97F17:;
    if (*(int *)(*(char **)player_character + 367 + (l_18 << 2)) == 0) goto L97F5E;
    inv_claim_item(*(int *)(*(char **)player_character + 367 + (l_18 << 2)));
    inv_store_item(*(int *)(*(char **)player_character + 367 + (l_18 << 2)));
L97F5E:;
    goto L97F0F;
L97F60:;
    inventory_close();
}

void func_00097F6F(void)
{
    int l_18;

    object_foreach(*(int *)(*(char **)player_entity + 63), (int)inv_claim_item);
    l_18 = 0;
L97F96:;
    if (l_18 < 27) goto L97FA6;
    return;
L97F9E:;
    l_18++;
    goto L97F96;
L97FA6:;
    if (*(int *)(*(char **)player_character + 367 + (l_18 << 2)) == 0) goto L97FD6;
    inv_claim_item(*(int *)(*(char **)player_character + 367 + (l_18 << 2)));
L97FD6:;
    goto L97F9E;
}

void trade_buy_button(void)
{
    if (*(int *)D_00195D2C == 0) return;
    func_00097A85();
}

void trade_clear_button(void)
{
    int l_1C;
    int l_18;

    switch (*(int *)trade_mode) {
    return;
case 1:
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)inv_return_unpaid_item);
    return;
case 2:
case 3:
case 4:
    l_1C = *(int *)(*(char **)inv_right_container + 63);
L98055:;
    if (l_1C == 0) return;
    l_18 = *(int *)((char *)l_1C + 55);
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) != 54) goto L98079;
    *(signed char *)((char *)l_1C) = 2;
L98079:;
    inv_store_item(l_1C);
    l_1C = l_18;
    goto L98055;
default:;
}
}

void trade_sell_button(void)
{
    if (*(int *)D_00195D2C == 0) return;
    func_00097A85();
}

void trade_repair_button(void)
{
    if (*(int *)D_00195D2C == 0) return;
    func_00097A85();
}

void trade_identify_button(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = 0;
    if (((int)(unsigned char)*(signed char *)(*(char **)current_building + 24)) != 11) goto L98124;
    if (((int)(unsigned short)*(short *)(*(char **)current_building + 18)) == 40) goto L98215;
L98124:;
    if (func_0009848E() != 0) goto L98141;
    msgbox_show_string((int)D_0017722B, 1);
    return;
L98141:;
    l_24 = *(int *)(*(char **)inv_right_container + 63);
L9814C:;
    if (l_24 == 0) goto L98199;
    if (func_0009848E() == 0) goto L98188;
    if (((int)(unsigned char)*(signed char *)(*(char **)player_character + 542)) < rand_range(1, 100)) goto L98188;
    l_1C++;
    *(signed char *)((char *)l_24 + 113) |= 32;
L98188:;
    l_18++;
    l_24 = *(int *)((char *)l_24 + 55);
    goto L9814C;
L98199:;
    func_000A0ED9(2642, (int)D_0017704C);
    mc_sprintf((int)text_buffer, *(int *)key_names, l_1C, l_18);
    msgbox_show_string((int)text_buffer, 1);
    l_24 = *(int *)(*(char **)inv_right_container + 63);
L981DF:;
    if (l_24 == 0) goto L98213;
    l_20 = *(int *)((char *)l_24 + 55);
    if (((int)(unsigned short)(*(short *)((char *)l_24 + 113) & 32)) == 0) goto L9820B;
    inv_store_item(l_24);
L9820B:;
    l_24 = l_20;
    goto L981DF;
L98213:;
    return;
L98215:;
    func_00097A85();
}

void inv_toggle_hidden(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (*(int *)inv_selected_item == 0) return;
    l_1C = *(int *)inv_selected_item;
    l_18 = *(int *)inv_selected_item + 71;
    if (((int)(unsigned short)(*(short *)((char *)l_18 + 42) & 64)) == 0) goto L98273;
    *(signed char *)((char *)l_18 + 42) &= 191;
    return;
L98273:;
    if (func_000CE44C(*(int *)player_character + 367, l_1C, 27) == 0) goto L9829F;
    msgbox_show_string((int)D_00177255, 1);
    return;
L9829F:;
    l_20 = func_0005FD36(1);
    *(int *)D_00195B84 = 0;
    object_foreach(*(int *)player_entity, (int)inv_sum_hidden_weight);
    if ((object_weight(l_1C) + *(int *)D_00195B84) <= l_20) goto L982E9;
    msgbox_show_string((int)D_0017727F, 1);
    return;
L982E9:;
    sound_play(235, *(int *)player_object, 100);
    *(signed char *)((char *)l_18 + 42) |= 64;
}

int func_0009848E(void)
{
    if (((int)(short)*(short *)(*(char **)player_character + 141)) >= *(int *)D_001AA458) goto L984B9;
    return 0;
L984B9:;
    *(short *)(*(char **)player_character + 141) -= *(short *)D_001AA458;
    return 1;
}

void func_000984E0(void)
{
    int l_1C;
    int l_18;

    *(int *)D_00195D2C = 0;
    l_1C = *(int *)(*(char **)inv_right_container + 63);
L98503:;
    if (l_1C == 0) goto L98524;
    *(int *)D_00195D2C += ((unsigned)(*(int *)((char *)l_1C + 107) * 25)) >> 8;
    l_1C = *(int *)((char *)l_1C + 55);
    goto L98503;
L98524:;
    *(int *)D_00195D30 = *(int *)D_00195D2C;
}

void func_00098A15(void)
{
    int l_18;

    if (((unsigned)(((unsigned)*(int *)(*(char **)D_00195AC4 + 31)) >> 16)) >= 1000) goto L98A85;
    map_goto_location(*(int *)D_001AA540, *(int *)D_001AA544, *(int *)D_001AA580, 0);
    mc_memcpy(*(int *)player_object, (int)D_00195FB1, 55, (int)D_0017704C, 2917, 4);
    *(short *)(*(char **)camera_object + 3) = *(short *)(*(char **)player_object + 3);
    return;
L98A85:;
    mc_memcpy((int)D_00195FB1, *(int *)player_object, 55, (int)D_0017704C, 2922, 4);
    *(int *)D_001AA540 = (int)(unsigned char)*(signed char *)current_region;
    *(int *)D_001AA544 = (int)(unsigned char)*(signed char *)player_environment;
    *(int *)D_001AA580 = (int)(unsigned short)*(short *)(*(char **)D_00195AC4 + 27);
    if ((((unsigned)*(int *)(*(char **)player_character + 120)) >> 16) != 992) goto L98AEC;
    l_18 = 1;
    goto L98AF3;
L98AEC:;
    l_18 = 2;
L98AF3:;
    map_goto_location(31, 1, l_18, 0);
    player_to_nearest_marker(*(int *)D_00195AC4, 8);
}

int func_00098B20(void)
{
    int l_24;
    int l_20;
    int l_1C;

    if ((((unsigned)*(int *)(*(char **)D_00195AC4 + 31)) >> 16) != 992) goto L98B49;
    l_24 = 1;
    goto L98B50;
L98B49:;
    l_24 = 0;
L98B50:;
    l_1C = l_24;
    if ((((unsigned)*(int *)(*(char **)D_00195AC4 + 31)) >> 16) != 993) goto L98B71;
    l_20 = 1;
    goto L98B78;
L98B71:;
    l_20 = 0;
L98B78:;
    l_1C += l_20;
    return l_1C;
}

int func_00098B91(int a1)
{
    if (*(int *)((char *)a1 + 51) == 0) goto L98BB4;
    if (*(signed char *)((char *)a1 + 38) != 0) goto L98BB6;
L98BB4:;
    goto L98BCB;
L98BB6:;
    *(int *)current_quest = quest_find_by_id((int)(short)((unsigned short)(unsigned char)*(signed char *)((char *)a1 + 38)));
    goto L98BD4;
L98BCB:;
    return 0;
L98BD4:;
    return 1;
}

void inv_read_map_scrap(int a1)
{
{
    char l_2C[20];

    if (a1 == 0) goto L98C9D;
    object_delete(a1);
L98C9D:;
    *(int *)D_00190EAC = (int)l_2C;
    location_pick_random_undiscovered((int)l_2C);
    msgbox_show_rsc(499, 1);
    location_set_discovered((int)(unsigned short)*(short *)(*(char **)((char *)l_2C + 12) + 27), 1);
    location_free((int)l_2C);
}
}

int item_forbidden_for_class(int a1)
{
    if (*(short *)(*(char **)player_class + 11) != 0) goto L98D0D;
    if (*(short *)(*(char **)player_class + 14) == 0) goto L98D31;
L98D0D:;
    if (((int)(unsigned short)*(short *)((char *)a1 + 32)) == 3) goto L98D2F;
    if (((int)(unsigned short)*(short *)((char *)a1 + 32)) != 2) goto L98D31;
L98D2F:;
    goto L98D36;
L98D31:;
    goto L98F09;
L98D36:;
    if (((int)(unsigned short)*(short *)((char *)a1 + 32)) != 2) goto L98D58;
    if (((int)(unsigned short)*(short *)((char *)a1 + 34)) >= 7) goto L98D5A;
L98D58:;
    goto L98D6B;
L98D5A:;
    if (((int)(unsigned short)*(short *)((char *)a1 + 34)) <= 10) goto L98D6D;
L98D6B:;
    goto L98DB2;
L98D6D:;
    if ((((int)(unsigned short)*(short *)(*(char **)player_class + 14)) & ((1 << (((int)(unsigned short)*(short *)((char *)a1 + 34)) - 7)) << 9)) == 0) goto L98DB2;
    msgbox_show_rsc(1068, 1);
    return 1;
L98DB2:;
    if (((int)(unsigned short)*(short *)((char *)a1 + 32)) != 2) goto L98DD4;
    if (((int)(unsigned short)*(short *)((char *)a1 + 34)) < 7) goto L98DD6;
L98DD4:;
    goto L98DFC;
L98DD6:;
    if ((((int)(unsigned short)*(short *)(*(char **)player_class + 14)) & ((1 << ((int)(unsigned char)*(signed char *)((char *)a1 + 55))) << 6)) != 0) goto L98DFE;
L98DFC:;
    goto L98E19;
L98DFE:;
    msgbox_show_rsc(1068, 1);
    return 1;
L98E19:;
    if (((int)(unsigned short)*(short *)((char *)a1 + 32)) != 3) goto L98E51;
    if ((((int)(unsigned short)*(short *)(*(char **)player_class + 14)) & ((int)(short)*(short *)(weapon_proficiency_bits + (((int)(unsigned short)*(short *)((char *)a1 + 34)) * 2)))) != 0) goto L98E53;
L98E51:;
    goto L98E6E;
L98E53:;
    msgbox_show_rsc(1068, 1);
    return 1;
L98E6E:;
    if (((int)(unsigned short)*(short *)((char *)a1 + 32)) != 3) goto L98EA2;
    if ((((int)(unsigned short)*(short *)(*(char **)player_class + 11)) & (1 << ((int)(unsigned char)*(signed char *)((char *)a1 + 54)))) != 0) goto L98EA4;
L98EA2:;
    goto L98EBC;
L98EA4:;
    msgbox_show_rsc(1068, 1);
    return 1;
L98EBC:;
    if (((int)(unsigned char)*(signed char *)((char *)a1 + 55)) != 2) goto L98EEF;
    if ((((int)(unsigned short)*(short *)(*(char **)player_class + 11)) & (1 << ((int)(unsigned char)*(signed char *)((char *)a1 + 54)))) != 0) goto L98EF1;
L98EEF:;
    goto L98F09;
L98EF1:;
    msgbox_show_rsc(1068, 1);
    return 1;
L98F09:;
    return 0;
}

void func_00098F1D(int a1)
{
    int l_18;

    if (*(int *)((char *)a1 + 51) == 0) return;
    *(int *)(*(char **)((char *)a1 + 51) + 31) = *(int *)((char *)a1 + 31);
}

void func_00098F50(void)
{
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)inv_assign_item_id);
}

void inv_track_hand_weapons(int a1)
{
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = 0;
    if (a1 != 0) goto L9901E;
    *(int *)D_001AA44C = 0;
    *(int *)D_001AA450 = 0;
    if (*(int *)(*(char **)player_character + 443) == 0) goto L98FE8;
    *(int *)D_001AA448 = (int)(unsigned short)*(short *)((char *)(*(int *)D_001AA44C = *(int *)(*(char **)player_character + 443)) + 105);
L98FE8:;
    if (*(int *)(*(char **)player_character + 451) == 0) goto L99019;
    *(int *)D_001AA444 = (int)(unsigned short)*(short *)((char *)(*(int *)D_001AA450 = *(int *)(*(char **)player_character + 451)) + 105);
L99019:;
    goto L990D4;
L9901E:;
    if (*(int *)D_001AA44C == *(int *)(*(char **)player_character + 443)) goto L99079;
    if (*(int *)D_001AA44C == 0) goto L9904B;
    l_1C += *(int *)(D_0017887F + (*(int *)D_001AA448 << 2));
L9904B:;
    if (*(int *)(*(char **)player_character + 443) == 0) goto L99079;
    l_1C += *(int *)(D_0017887F + (((int)(unsigned short)*(short *)(*(char **)(*(char **)player_character + 443) + 105)) << 2));
L99079:;
    if (*(int *)D_001AA450 == *(int *)(*(char **)player_character + 451)) goto L990D4;
    if (*(int *)D_001AA450 == 0) goto L990A6;
    l_18 += *(int *)(D_0017887F + (*(int *)D_001AA444 << 2));
L990A6:;
    if (*(int *)(*(char **)player_character + 451) == 0) goto L990D4;
    l_18 += *(int *)(D_0017887F + (((int)(unsigned short)*(short *)(*(char **)(*(char **)player_character + 451) + 105)) << 2));
L990D4:;
    *(int *)D_0019597C += l_1C;
    *(int *)D_00195980 += l_18;
}

void func_00099155(int a1)
{
    int l_1C;
    int l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) return;
    l_1C = *(int *)((char *)a1 + 67);
L9917E:;
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) == 52) goto L991A7;
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) == 1) return;
    l_1C = *(int *)((char *)l_1C + 67);
    goto L9917E;
L991A7:;
    if (((int)(unsigned short)*(short *)((char *)l_1C + 27)) > 4) return;
    l_18 = a1 + 71;
    if (((int)(short)*(short *)((char *)l_18 + 67)) == (-1)) return;
    *(int *)((char *)l_18 + 36) = enchant_item_value(l_18);
}

void func_000991E7(void)
{
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)func_00099155);
}

int trade_can_repair_item(int a1)
{
    if (*(int *)(*(char **)inv_right_container + 63) == 0) goto L9923F;
    if (((int)(unsigned char)*(signed char *)(*(char **)current_building + 24)) == 11) goto L99241;
L9923F:;
    goto L9925C;
L99241:;
    msgbox_show_string((int)D_00177300, 1);
    return 0;
L9925C:;
    if (*(int *)(*(char **)inv_selected_item + 63) == 0) goto L99282;
    msgbox_show_string((int)D_00177323, 1);
    return 0;
L99282:;
    if (((int)(unsigned short)*(short *)((char *)a1 + 32)) != 3) goto L992A4;
    if (((int)(unsigned short)*(short *)((char *)a1 + 34)) == 18) goto L992A6;
L992A4:;
    goto L992BE;
L992A6:;
    msgbox_show_rsc(24, 1);
    return 0;
L992BE:;
    if (*(short *)((char *)a1 + 44) != *(short *)((char *)a1 + 46)) goto L992E6;
    msgbox_show_rsc(24, 1);
    return 0;
L992E6:;
    return 1;
}

void func_000992FA(void)
{
    int l_18;

    if (*(int *)trade_mode != 3) return;
    l_18 = *(int *)inv_selected_item + 71;
    if (((int)(unsigned char)*(signed char *)(*(char **)current_building + 24)) != 11) goto L99382;
    *(int *)(*(char **)inv_selected_item + 43) = (((((((int)(unsigned short)*(short *)((char *)l_18 + 46)) - ((int)(unsigned short)*(short *)((char *)l_18 + 44))) / ((int)&*(signed char *)((char *)((int)(unsigned char)*(signed char *)(*(char **)guild_membership)) + 1))) * 1440) / 144000) + 1440) + *(int *)game_minutes;
    return;
L99382:;
    func_0009830F();
}

void func_00099391(void)
{
    int l_18;

    l_18 = *(int *)(*(char **)inv_right_container + 63);
L993AA:;
    if (l_18 == 0) return;
    *(signed char *)((char *)l_18) = 54;
    l_18 = *(int *)((char *)l_18 + 55);
    goto L993AA;
}

int func_000993CB(int a1)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = *(int *)D_00195C44 + 55000;
    *(signed char *)((char *)l_1C) = 0;
L993F6:;
    if (((int)(signed char)*(signed char *)((char *)(a1 + l_20))) == (-2)) goto L9940A;
    if (l_20 < 8) goto L9940C;
L9940A:;
    goto L9946E;
L9940C:;
    func_000A1054(l_1C, ((int)item_templates) + (((int)(short)*(short *)((char *)(int)(*(char **)(D_00185F88 + (((int)(signed char)*(signed char *)((char *)(a1 + l_20) + 10)) << 2)) + (((int)(signed char)*(signed char *)((char *)(a1 + l_20))) * 2)))) * 48), (int)D_0017704C, 3200, 4);
    func_000A1054(l_1C, (int)D_00177346, (int)D_0017704C, 3201, 4);
    l_20++;
    goto L993F6;
L9946E:;
    *(signed char *)((char *)(func_000A0DF4(l_1C) + l_1C) + 1) = 0;
    return l_1C;
}
