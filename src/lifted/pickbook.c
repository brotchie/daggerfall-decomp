/* pickbook.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_0_1 { unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char key_down_esc[];
extern char screen_buffer[];
extern char D_00170DE4[];
extern char D_00170DEF[];
extern char D_00170E11[];
extern char D_00170E17[];
extern char selected_spell[];
extern char spellbook_buttons[];
extern char D_0017B6D2[];
extern char D_0017B6D4[];
extern char D_0017B6D6[];
extern char D_0017B6D8[];
extern char spell_effect_names[];
extern char spell_effect_subtype_names[];
extern char D_0018320A[];
extern char D_00184634[];
extern char monster_category[];
extern char D_00187CA8[];
extern char text_buffer[];
extern char D_00190504[];
extern char D_00190D64[];
extern char text_macro_fpc[];
extern char D_001940D4[];
extern char D_001940D5[];
extern char D_001940D8[];
extern char D_001959FC[];
extern char player_entity[];
extern char player_object[];
extern char creature_count[];
extern char spell_ready_missile[];
extern char spell_ready_touch[];
extern char spellshop_icons[];
extern char player_character[];
extern char window_image[];
extern char game_minutes[];
extern char trade_mode[];
extern char spell_effect_slot[];
extern char D_00195F62[];
extern char msgbox_kind[];
extern char D_00196272[];
extern char game_mode[];
extern char mouse_buttons_prev[];
extern char inv_right_icon[];
extern char spellbook_saved_screen[];
extern char D_001A9AB8[];
extern char D_001A9AE1[];
extern char D_001A9AE7[];

extern int spell_effect_text_index(short);
extern int spell_cost(int, int);
extern int sheet_open(int);
extern int spellbook_open(int);
extern int cast_player_spell(int);
extern int sound_play(int, int, int);
extern int hud_message_add(int);
extern int picklist_frame(int);
extern int object_delete(int);
extern int object_create_child(int, int, int);
extern int object_find_item(int, int, int);
extern int object_new_id(int);
extern int inventory_open(int, int, int);
extern int mc_free();
extern int mc_strncpy();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000CD20E();
extern int func_000CE31C();
extern int func_0012B136();
extern int func_0012DB50();
extern int func_00144FB4();
extern void spell_add_skill_uses(int, int);
extern void msgbox_show_rsc(int, int);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern void picklist_init(int, short, short, int, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void picklist_add(int, int, int);
extern void picklist_free(int);
extern void func_0008E152(int, int);
extern void object_foreach(int, int);
int spellbook_close(void);
int spellbook_build_list(void);
int func_00042380(void);
void spellbook_add_spell_cb(int);
void spellbook_draw_spell(int);
void spellbook_effect_help(int);
#pragma aux func_000A0ED9 parm routine [];

void spellbook_add_spell_cb(int a1)
{
    int l_1C;
    int l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 9) return;
    l_1C = a1 + 71;
    if (((int)(unsigned char)*(signed char *)((char *)l_1C + 47)) == 33) goto L416CE;
    if (((int)(unsigned char)*(signed char *)((char *)(func_000A0DF4(l_1C + 47) + l_1C) + 48)) != 36) goto L4175A;
L416CE:;
    l_18 = spell_cost(l_1C, *(int *)player_character);
    if (((int)(unsigned char)*(signed char *)((char *)(func_000A0DF4(l_1C + 47) + l_1C) + 48)) != 36) goto L4172C;
    l_18 >>= 2;
    func_000A0ED9(62, (int)D_00170DE4);
    mc_sprintf((int)text_buffer, (int)D_00170DEF, l_18, l_1C + 47);
    goto L41758;
L4172C:;
    func_000A0ED9(65, (int)D_00170DE4);
    mc_sprintf((int)text_buffer, (int)D_00170DEF, l_18, l_1C + 48);
L41758:;
    goto L41791;
L4175A:;
    func_000A0ED9(68, (int)D_00170DE4);
    mc_sprintf((int)text_buffer, (int)D_00170DEF, spell_cost(l_1C, *(int *)player_character), l_1C + 47);
L41791:;
    picklist_add((int)D_001A9AB8, (int)text_buffer, 0);
    *(int *)(text_macro_fpc + (((int)(short)(*(short *)D_00190D64)++) << 2)) = a1;
}

void spellbook_frame(void)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    short l_1C;
    short l_18;

    if (spellbook_open(0) == 0) return;
    mc_memcpy(*(int *)screen_buffer, *(int *)spellbook_saved_screen, 64000, (int)D_00170DE4, 123, 4);
    l_28 = *(int *)window_image;
    func_00144FB4((int)(unsigned short)*(short *)((char *)l_28), (int)(unsigned short)*(short *)((char *)l_28 + 2), (int)(unsigned short)*(short *)((char *)l_28 + 4), (int)(unsigned short)*(short *)((char *)l_28 + 6), l_28 + 12);
    func_000A0ED9(129, (int)D_00170DE4);
    mc_sprintf((int)text_buffer, (int)D_00170E11, (int)(short)*(short *)(*(char **)player_character + 141), (int)(short)*(short *)(*(char **)player_character + 143));
    text_draw_colored((int)text_buffer, 238, 20, 145, 141);
    func_0012DB50(4);
    *(int *)&l_1C = picklist_frame((int)D_001A9AB8);
    if (((int)(short)l_1C) <= (-1)) goto L41BDD;
    l_30 = *(int *)(text_macro_fpc + (((int)(unsigned short)*(short *)D_001A9AE1) << 2));
    spellbook_close();
    if (((struct bf8_0_1 *)(*(char **)player_character + 138))->f == 0) goto L41A68;
    hud_message_add(*(int *)D_00184634);
    goto L41BD8;
L41A68:;
    if (((struct bf8_7_1 *)&D_001940D4)->f == 0) goto L41BD8;
    l_24 = (spell_cost(l_30 + 71, *(int *)player_character) * func_00042380()) / 100;
    if (((int)(unsigned char)*(signed char *)((char *)l_30 + 144)) != 92) goto L41AB9;
    l_24 = 0;
L41AB9:;
    if (((int)(unsigned char)*(signed char *)((char *)(func_000A0DF4(l_30 + 118) + l_30) + 119)) != 36) goto L41AD8;
    l_24 >>= 2;
L41AD8:;
    if ((((int)(short)*(short *)(*(char **)player_character + 141)) + *(int *)D_001959FC) >= l_24) goto L41AFE;
    hud_message_add((int)D_00170E17);
    return;
L41AFE:;
    *(short *)D_00195F62 = l_24;
    *(int *)spell_ready_touch = (*(int *)spell_ready_missile = 0);
    if (*(int *)D_001959FC == 0) goto L41B55;
    if (l_24 <= *(int *)D_001959FC) goto L41B43;
    l_24 -= *(int *)D_001959FC;
    *(int *)D_001959FC = 0;
    goto L41B55;
L41B43:;
    *(int *)D_001959FC -= l_24;
    *(short *)D_00195F62 = 0;
L41B55:;
    spell_add_skill_uses(l_30 + 71, 1);
    *(short *)(*(char **)player_character + 141) -= l_24;
    l_2C = object_create_child(*(int *)(*(char **)player_object + 67), 0, 89);
    *(signed char *)((char *)l_2C) = 9;
    *(int *)((char *)l_2C + 31) = object_new_id(100);
    mc_memcpy(l_2C + 71, l_30 + 71, 89, (int)D_00170DE4, 181, 4);
    if (cast_player_spell(l_2C) == 0) goto L41BD8;
    object_delete(l_2C);
L41BD8:;
    return;
L41BDD:;
    spellbook_draw_spell((*(int *)selected_spell = *(int *)(text_macro_fpc + (((int)(unsigned short)*(short *)D_001A9AE1) << 2)) + 71));
    if (((int)(unsigned char)*(signed char *)msgbox_kind) != 2) goto L41C3E;
    mc_strncpy((int)(*(char **)D_001A9AE7 + (((int)(unsigned short)*(short *)D_001A9AE1) * 44)) + 4, (int)&*(signed char *)(*(char **)selected_spell + 47), 40, (int)D_00170DE4, 193);
L41C3E:;
    if (*(signed char *)key_down_esc != 0) goto L41C50;
    if (((int)(short)l_1C) != (-2)) goto L41C55;
L41C50:;
    spellbook_close();
L41C55:;
    if (*(signed char *)mouse_buttons == 0) goto L41C72;
    if (*(signed char *)mouse_buttons == 0) goto L41C70;
    if (*(signed char *)mouse_buttons_prev != 0) goto L41C72;
L41C70:;
    goto L41C77;
L41C72:;
    return;
L41C77:;
    *(int *)&l_18 = 0;
L41C7E:;
    if (((int)(short)l_18) < 9) goto L41C94;
    return;
L41C8C:;
    (*(int *)&l_18)++;
    goto L41C7E;
L41C94:;
    if (*(short *)mouse_x <= *(short *)(spellbook_buttons + (((int)(short)l_18) * 12))) goto L41CC2;
    if (*(short *)mouse_x < *(short *)(D_0017B6D4 + (((int)(short)l_18) * 12))) goto L41CC4;
L41CC2:;
    goto L41CDB;
L41CC4:;
    if (*(short *)mouse_y > *(short *)(D_0017B6D2 + (((int)(short)l_18) * 12))) goto L41CDD;
L41CDB:;
    goto L41CF4;
L41CDD:;
    if (*(short *)mouse_y < *(short *)(D_0017B6D6 + (((int)(short)l_18) * 12))) goto L41CF6;
L41CF4:;
    goto L41D18;
L41CF6:;
    sound_play(205, *(int *)player_object, 100);
    ((int (*)())(*(int *)(D_0017B6D8 + (((int)(short)l_18) * 12))))();
L41D18:;
    goto L41C8C;
}

int spellbook_close(void)
{
    if (((struct bf8_2_1 *)&D_001940D4)->f == 0) goto L41D4F;
    *(signed char *)D_001940D4 &= 251;
    picklist_free((int)D_001A9AB8);
L41D4F:;
    if (*(signed char *)key_down_esc != 0) goto L41D4F;
L41D58:;
    if (*(signed char *)mouse_buttons == 0) goto L41D68;
    func_0012B136();
    goto L41D58;
L41D68:;
    *(signed char *)D_001940D8 &= 253;
    *(signed char *)game_mode = 0;
    if (*(int *)window_image == 0) goto L41D8B;
    if (*(int *)window_image != (-1751672937)) goto L41D8D;
L41D8B:;
    goto L41DAB;
L41D8D:;
    mc_free(*(int *)window_image, (int)D_00170DE4, 222);
    *(int *)window_image = -1751672937;
L41DAB:;
    if (*(int *)spellshop_icons == 0) goto L41DC0;
    if (*(int *)spellshop_icons != (-1751672937)) goto L41DC2;
L41DC0:;
    goto L41DE0;
L41DC2:;
    mc_free(*(int *)spellshop_icons, (int)D_00170DE4, 223);
    *(int *)spellshop_icons = -1751672937;
L41DE0:;
    if (*(int *)spellbook_saved_screen == 0) goto L41DF5;
    if (*(int *)spellbook_saved_screen != (-1751672937)) goto L41DF7;
L41DF5:;
    goto L41E15;
L41DF7:;
    mc_free(*(int *)spellbook_saved_screen, (int)D_00170DE4, 224);
    *(int *)spellbook_saved_screen = -1751672937;
L41E15:;
    *(signed char *)D_00196272 = 0;
    if (((struct bf8_5_1 *)&D_001940D8)->f == 0) goto L41E38;
    *(signed char *)D_001940D8 &= 223;
    sheet_open(1);
    goto L41E69;
L41E38:;
    if (((struct bf8_7_1 *)&D_001940D8)->f == 0) goto L41E62;
    *(signed char *)D_001940D8 &= 127;
    inventory_open(2, *(int *)trade_mode, (int)(unsigned char)*(signed char *)inv_right_icon);
    goto L41E69;
L41E62:;
    *(signed char *)D_00187CA8 = 1;
L41E69:;
    return 1;
}

void spellbook_draw_spell(int a1)
{
    short l_18;

    *(signed char *)D_0012B508 = 145;
    func_000CD20E(172, 32, (int)(unsigned char)*(signed char *)((char *)a1 + 72));
    func_000CE31C((int)(*(char **)spellshop_icons + (((int)(unsigned char)*(signed char *)((char *)a1 + 6)) * 640)) + 24, (int)(*(char **)screen_buffer + 10486), 16, 16, 40);
    func_000CE31C((int)(*(char **)spellshop_icons + (((int)(unsigned char)*(signed char *)((char *)a1 + 7)) * 640)), (int)&*(signed char *)(*(char **)screen_buffer + 10445), 24, 16, 40);
    text_draw_colored(a1 + 47, 148, 20, 145, 141);
    *(int *)&l_18 = 0;
L41F40:;
    if (((int)(short)l_18) < 3) goto L41F56;
    return;
L41F4E:;
    (*(int *)&l_18)++;
    goto L41F40;
L41F56:;
    if (((int)(unsigned char)*(signed char *)((char *)((((int)(short)l_18) * 2) + a1))) == 255) goto L41F4E;
    text_draw_centered_colored(*(int *)(spell_effect_names + (((int)(unsigned char)*(signed char *)((char *)((((int)(short)l_18) * 2) + a1))) << 2)), 219, (int)(short)((*(int *)&l_18 * 38) + 63), 145, 141);
    if (((int)(unsigned char)*(signed char *)((char *)((((int)(short)l_18) * 2) + a1) + 1)) == 255) goto L41FF1;
    if (*(int *)(spell_effect_subtype_names + (((int)(unsigned char)*(signed char *)((char *)((((int)(short)l_18) * 2) + a1))) * 48) + (((int)(unsigned char)*(signed char *)((char *)((((int)(short)l_18) * 2) + a1) + 1)) << 2)) != 0) goto L41FF3;
L41FF1:;
    goto L4203F;
L41FF3:;
    text_draw_centered_colored(*(int *)(spell_effect_subtype_names + (((int)(unsigned char)*(signed char *)((char *)((((int)(short)l_18) * 2) + a1))) * 48) + (((int)(unsigned char)*(signed char *)((char *)((((int)(short)l_18) * 2) + a1) + 1)) << 2)), 219, (int)(short)((*(int *)&l_18 * 38) + 75), 145, 141);
L4203F:;
    goto L41F4E;
}

void spellbook_effect_help(int a1)
{
{
    int l_1C;

    if (((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)selected_spell + (((int)(short)*(short *)&a1) * 2)))) == 255) return;
    *(short *)spell_effect_slot = a1;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc((int)(short)(spell_effect_text_index((int)(short)*(short *)&a1) + 1200), 1);
}
}

void spellbook_effect1_button(void)
{
    spellbook_effect_help(0);
}

void spellbook_effect2_button(void)
{
    spellbook_effect_help(1);
}

void spellbook_effect3_button(void)
{
    spellbook_effect_help(2);
}

int spellbook_build_list(void)
{
    int l_1C;

    *(short *)D_00190D64 = 0;
    l_1C = object_find_item(*(int *)player_entity, 27, 0);
    if (l_1C != 0) goto L4214F;
    return 0;
L4214F:;
    if (*(int *)((char *)l_1C + 63) != 0) goto L4216F;
    if (((int)(unsigned short)(*(short *)(*(char **)player_character + 64) & 4)) == 0) goto L42171;
L4216F:;
    goto L4217D;
L42171:;
    return 0;
L4217D:;
    picklist_init((int)D_001A9AB8, 27, 30, 111, 131, 144, 29, 8, 15, 144, 150, 8, 15, 144, 45, 9, 104, 146, 146, 244, 114, 0);
    object_foreach(*(int *)((char *)l_1C + 63), (int)spellbook_add_spell_cb);
    if (((int)(unsigned short)(*(short *)(*(char **)player_character + 64) & 4)) == 0) goto L42263;
    l_1C = *(int *)(*(char **)player_entity + 63);
L42231:;
    if (l_1C == 0) goto L42263;
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) != 28) goto L42258;
    object_foreach(*(int *)((char *)l_1C + 63), (int)spellbook_add_spell_cb);
    goto L42263;
L42258:;
    l_1C = *(int *)((char *)l_1C + 55);
    goto L42231;
L42263:;
    if (*(short *)D_00190D64 != 0) goto L4228F;
    picklist_free((int)D_001A9AB8);
    msgbox_show_rsc(12, 1);
    return 0;
L4228F:;
    *(signed char *)D_001940D8 &= 254;
    *(signed char *)D_001940D4 |= 4;
    return 1;
}

void spellbook_delete_button(void)
{
    object_delete(*(int *)selected_spell - 71);
    picklist_free((int)D_001A9AB8);
    if (spellbook_build_list() != 0) return;
    spellbook_close();
}

void spellbook_up_button(void)
{
    int l_18;

    l_18 = *(int *)selected_spell - 71;
    if (*(int *)((char *)l_18 + 59) == 0) return;
    func_0008E152(l_18, *(int *)((char *)l_18 + 59));
    picklist_free((int)D_001A9AB8);
    spellbook_build_list();
}

void spellbook_down_button(void)
{
    int l_18;

    l_18 = *(int *)selected_spell - 71;
    if (*(int *)((char *)l_18 + 55) == 0) return;
    func_0008E152(l_18, *(int *)((char *)l_18 + 55));
    picklist_free((int)D_001A9AB8);
    spellbook_build_list();
}

int func_00042380(void)
{
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_28 = 100;
    l_20 = (((unsigned)*(int *)game_minutes) / 1440) & 31;
    l_1C = (((unsigned)(*(int *)game_minutes + 5760)) / 1440) & 31;
    l_30 = 0;
L423C9:;
    if (l_30 < 27) goto L423DC;
    goto L42589;
L423D4:;
    l_30++;
    goto L423C9;
L423DC:;
    if (*(int *)(*(char **)player_character + 367 + (l_30 << 2)) == 0) goto L423D4;
    l_34 = *(int *)player_character + 371;
    if (((int)(short)*(short *)((char *)l_34 + 67)) == (-1)) goto L423D4;
    l_2C = 0;
L42413:;
    if (l_2C < 10) goto L42426;
    goto L42584;
L4241E:;
    l_2C++;
    goto L42413;
L42426:;
    if (((int)(short)*(short *)((char *)((l_2C << 2) + l_34) + 67)) == (-1)) goto L42584;
    if (((int)(short)*(short *)((char *)((l_2C << 2) + l_34) + 67)) != 3) goto L4257F;
    switch (*(unsigned short *)((char *)((l_2C << 2) + l_34) + 69)) {
    goto L4257F;
case 0:
case 1:
case 2:
case 3:
    if ((short)*(unsigned char *)(D_0018320A + (((unsigned)*(int *)game_minutes) / 43200)) != *(short *)((char *)((l_2C << 2) + l_34) + 69)) goto L424D3;
    l_28 = 75;
L424D3:;
    goto L4257F;
case 4:
    if (l_20 == 0) goto L424E4;
    if (l_1C != 0) goto L424EB;
L424E4:;
    l_28 = 75;
L424EB:;
    goto L4257F;
case 5:
    if (l_20 == 8) goto L424FC;
    if (l_20 != 24) goto L424FE;
L424FC:;
    goto L42504;
L424FE:;
    if (l_1C != 8) goto L42506;
L42504:;
    goto L4250C;
L42506:;
    if (l_1C != 24) goto L42513;
L4250C:;
    l_28 = 75;
L42513:;
    goto L4257F;
case 6:
    if (l_20 == 16) goto L42524;
    if (l_1C != 16) goto L4252B;
L42524:;
    l_28 = 75;
L4252B:;
    goto L4257F;
case 7:
case 8:
case 9:
case 10:
    l_24 = 0;
L42534:;
    if (l_24 < *(int *)creature_count) goto L42543;
    goto L4257F;
L42541:;
    goto L42534;
L42543:;
    if (((int)(unsigned char)*(signed char *)(monster_category + ((int)(unsigned char)*(signed char *)(*(char **)(D_00190504 + (l_24 << 2)) + 138)))) != (((int)(short)*(short *)((char *)((l_2C << 2) + l_34) + 69)) - 7)) goto L4257D;
    l_28 = 75;
L4257D:;
    goto L42541;
default:
L4257F:;
    goto L4241E;
L42584:;
    goto L423D4;
L42589:;
    return l_28;
}
}
