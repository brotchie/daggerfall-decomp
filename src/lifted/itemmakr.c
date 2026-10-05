/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern char key_down_esc[];
extern char D_001756A3[];
extern char D_001756BE[];
extern char D_001756EC[];
extern char D_0017570A[];
extern char D_00175723[];
extern char D_00175734[];
extern char D_00175738[];
extern char D_0017D1EA[];
extern char enchant_power_names[];
extern char D_00180ACE[];
extern char enchant_side_effect_names[];
extern char monster_names[];
extern char enchant_power_params[];
extern char D_001857E5[];
extern char D_0018586F[];
extern char D_00185871[];
extern char enchant_spell_lists[];
extern char D_001858DB[];
extern char D_00185907[];
extern char D_00185908[];
extern char D_00185909[];
extern char D_0018590A[];
extern char D_0018597F[];
extern char D_00187CA8[];
extern char text_buffer[];
extern char D_00190BE4[];
extern char D_00190BE8[];
extern char itemmaker_slot_kinds[];
extern char D_00190CEE[];
extern char D_00190D02[];
extern char D_00190D63[];
extern char D_00190D64[];
extern char D_00190D66[];
extern char D_00190D68[];
extern char D_00190EDC[];
extern char D_00190EE4[];
extern char text_rsc_buffer[];
extern char D_001913E4[];
extern char D_001940D8[];
extern char D_0019574C[];
extern char D_001957CD[];
extern char D_001957E9[];
extern char player_entity[];
extern char player_object[];
extern char cheat_flags[];
extern char spell_records[];
extern char guild_npc_object[];
extern char spellshop_icons[];
extern char list_popup_callback[];
extern char D_00195B84[];
extern char player_character[];
extern char window_image[];
extern char D_00195C44[];
extern char cfg_item_file[];
extern char D_00196272[];
extern char game_mode[];
extern char D_00199868[];
extern char D_00199869[];
extern char D_0019986A[];
extern char D_0019986B[];
extern char D_0019986C[];
extern char D_0019986D[];
extern char D_0019986E[];
extern char D_0019986F[];
extern char D_00199870[];
extern char D_00199871[];
extern char D_001998CC[];
extern char itemmaker_slots[];
extern char D_001998E2[];
extern char itemmaker_item[];
extern char itemmaker_item_object[];
extern char D_00199910[];
extern char inv_left_scroll[];
extern char inv_left_rows[];
extern char D_001AA578[];
extern char inv_left_container[];
extern char D_001AA586[];
extern char inv_left_count[];

extern int spells_std_names_for_ids(int);
extern int spell_cost(int, int);
extern int itemmaker_row_slot(int);
extern int enchant_slot_cost(int, unsigned char, unsigned char, short);
extern int enchant_value_slot_cost(int, unsigned char, unsigned char, short);
extern int itemmaker_power_excluded(short);
extern int sound_play(int, int, int);
extern int disk_create(int);
extern int gold_can_afford(int);
extern int object_free_single(int);
extern int object_delete(int);
extern int inv_draw_item_cell(int, int, int);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int mc_strncpy();
extern int write();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int func_000A1054();
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void player_refresh_paperdoll(void);
extern void itemmaker_add_power_cb(short);
extern void itemmaker_add_side_effect_cb(short);
extern void func_00057147(short, short, short, short, short, short, short);
extern void itemmaker_show_param_list(int, short);
extern void itemmaker_add_soul_powers(short);
extern void picklist_open(int);
extern void gold_spend(int);
extern void inpstr_begin_text(int, short);
extern void object_foreach(int, int);
extern void inv_store_item(int);
int spell_name_by_id(unsigned char);
int itemmaker_points_used(void);
int itemmaker_gold_cost(void);
int itemmaker_has_soul_bound(void);
int itemmaker_has_health_leech(void);
void itemmaker_reset(void);
void itemmaker_store_item(void);
void itemmaker_remove_slot(short);
void itemmaker_soul_list_cb(int);
void itemmaker_show_list(int, int);
void itemmaker_consume_soul(void);
void func_00057F42(void);
void itemmaker_clear_soul_slots(void);
void func_000585D6(int, int);
void func_00058AF7(void);
#pragma aux func_000A0ED9 parm routine [];

void itemmaker_reset(void)
{
    *(int *)D_00190BE8 = (*(int *)D_00190BE4 = 0);
    *(int *)itemmaker_item_object = 0;
    *(int *)itemmaker_item = 0;
    *(int *)inv_left_scroll = 0;
    mc_memset((int)itemmaker_slot_kinds, -1, 10, (int)D_001756A3, 77, 128);
    mc_memset((int)D_00190CEE, -1, 30, (int)D_001756A3, 78, 4);
    mc_memset((int)D_00190D02, -1, 30, (int)D_001756A3, 79, 4);
    mc_memset((int)D_00199868, -1, 120, (int)D_001756A3, 80, 120);
    mc_memset((int)itemmaker_slots, 0, 40, (int)D_001756A3, 81, 40);
    mc_memset((int)D_00199910, 0, 10, (int)D_001756A3, 82, 10);
}

int itemmaker_close(void)
{
L561E6:;
    if (*(signed char *)key_down_esc != 0) goto L561E6;
    *(signed char *)D_00187CA8 = 1;
    if (*(int *)itemmaker_item == 0) goto L56209;
    inv_store_item(*(int *)itemmaker_item_object);
L56209:;
    *(signed char *)game_mode = 0;
    if (*(int *)window_image == 0) goto L56225;
    if (*(int *)window_image != (-1751672937)) goto L56227;
L56225:;
    goto L56245;
L56227:;
    mc_free(*(int *)window_image, (int)D_001756A3, 141);
    *(int *)window_image = -1751672937;
L56245:;
    if (*(int *)spellshop_icons == 0) goto L5625A;
    if (*(int *)spellshop_icons != (-1751672937)) goto L5625C;
L5625A:;
    goto L5627A;
L5625C:;
    mc_free(*(int *)spellshop_icons, (int)D_001756A3, 142);
    *(int *)spellshop_icons = -1751672937;
L5627A:;
    *(signed char *)D_001940D8 &= 251;
    *(signed char *)D_00196272 = 0;
    player_refresh_paperdoll();
    return 1;
}

void itemmaker_enter_name(void)
{
    int l_18;

    if (*(int *)itemmaker_item != 0) goto L56847;
    msgbox_show_rsc(1653, 1);
    return;
L56847:;
    l_18 = *(int *)D_00195C44 + 55000;
    func_000A0ED9(242, (int)D_001756A3);
    mc_sprintf(l_18, (int)D_001756BE, *(int *)D_0017D1EA);
    *(signed char *)((char *)(func_000A0DF4(l_18) + l_18) + 1) = 0;
    inpstr_begin_text(*(int *)itemmaker_item, 23);
    msgbox_show_string(l_18, 2);
}

void itemmaker_powers_click(void)
{
    itemmaker_remove_slot((int)(short)itemmaker_row_slot(0));
}

void itemmaker_add_powers(void)
{
    int l_18;

    if (*(int *)itemmaker_item != 0) goto L56B68;
    msgbox_show_rsc(1653, 1);
    return;
L56B68:;
    *(int *)list_popup_callback = (int)itemmaker_add_power_cb;
    itemmaker_show_list((int)enchant_power_names, 0);
}

void itemmaker_side_effects_click(void)
{
    itemmaker_remove_slot((int)(short)itemmaker_row_slot(1));
}

void itemmaker_set_side_effect_param_cb(short a1)
{
    short l_1C;
    short l_18;

    *(int *)&l_18 = *(int *)&a1;
    if (*(short *)(itemmaker_slots + (((int)(short)*(short *)D_00190D64) << 2)) != 0) goto L56BFA;
    *(int *)&l_18 = 0;
    *(int *)&a1 = (int)(unsigned char)*(signed char *)(text_rsc_buffer + ((int)(short)a1));
    itemmaker_add_soul_powers((int)(short)a1);
L56BFA:;
    *(short *)(D_001998E2 + (((int)(short)*(short *)D_00190D64) << 2)) = *(int *)&a1;
    *(int *)&l_1C = (int)(unsigned char)*(signed char *)(D_0018597F + ((int)(short)*(short *)D_00190D66));
    if (l_1C != 0) goto L56C61;
    func_00057147((int)(short)*(short *)D_00190D64, (int)(short)(*(short *)D_00190D66 + 15), (int)(short)l_18, -1, -1, -1, -1);
    return;
L56C61:;
    (*(int *)&l_1C)--;
    func_00057147((int)(short)*(short *)D_00190D64, (int)(short)(*(short *)D_00190D66 + 15), (int)(short)l_18, (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185907 + ((((int)(short)l_1C) * 20) + (((int)(short)l_18) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185908 + ((((int)(short)l_1C) * 20) + (((int)(short)l_18) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185909 + ((((int)(short)l_1C) * 20) + (((int)(short)l_18) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_0018590A + ((((int)(short)l_1C) * 20) + (((int)(short)l_18) << 2)))));
}

void itemmaker_add_side_effects(void)
{
    int l_18;

    if (*(int *)itemmaker_item != 0) goto L56E84;
    msgbox_show_rsc(1653, 1);
    return;
L56E84:;
    *(int *)list_popup_callback = (int)itemmaker_add_side_effect_cb;
    itemmaker_show_list((int)enchant_side_effect_names, 15);
}

void itemmaker_return_item(void)
{
    int l_1C;
    int l_18;

    if (*(int *)itemmaker_item == 0) goto L56EC8;
    inv_store_item(*(int *)itemmaker_item_object);
L56EC8:;
    *(int *)itemmaker_item_object = 0;
    *(int *)itemmaker_item = 0;
    *(int *)D_00190BE4 = 0;
}

void itemmaker_enchant(void)
{
    int l_1C;
    int l_18;

    if (*(int *)itemmaker_item != 0) goto L56F1B;
    msgbox_show_rsc(1653, 1);
    return;
L56F1B:;
    if (((struct bf8_2_1 *)&cheat_flags)->f != 0) goto L56F78;
    if (itemmaker_points_used() <= ((int)(unsigned short)*(short *)(*(char **)itemmaker_item + 61))) goto L56F4C;
    msgbox_show_rsc(1651, 1);
    return;
L56F4C:;
    if (gold_can_afford(itemmaker_gold_cost()) != 0) goto L56F6E;
    msgbox_show_rsc(1650, 1);
    return;
L56F6E:;
    gold_spend(itemmaker_gold_cost());
L56F78:;
    msgbox_show_rsc(1652, 1);
    l_18 = 0;
    l_1C = 0;
L56F95:;
    if (l_1C < 10) goto L56FA8;
    goto L570AF;
L56FA0:;
    l_1C++;
    goto L56F95;
L56FA8:;
    if (((int)(signed char)*(signed char *)(itemmaker_slot_kinds + l_1C)) == (-1)) goto L56FF5;
    *(short *)(*(char **)itemmaker_item + 67 + (l_18 << 2)) = *(short *)(itemmaker_slots + (l_1C << 2));
    *(short *)(*(char **)itemmaker_item + 69 + (l_18 << 2)) = *(short *)(D_001998E2 + (l_1C << 2));
L56FF5:;
    if (*(signed char *)(itemmaker_slot_kinds + l_1C) <= 0) goto L57014;
    *(short *)(*(char **)itemmaker_item + 67 + (l_18 << 2)) += 15;
L57014:;
    if (*(signed char *)(itemmaker_slot_kinds + l_1C) != 0) goto L57058;
    if (*(short *)(itemmaker_slots + (l_1C << 2)) == 0) goto L57042;
    if (((int)(short)*(short *)(itemmaker_slots + (l_1C << 2))) != 1) goto L57044;
L57042:;
    goto L57056;
L57044:;
    if (((int)(short)*(short *)(itemmaker_slots + (l_1C << 2))) != 2) goto L57058;
L57056:;
    goto L5705A;
L57058:;
    goto L57095;
L5705A:;
    *(short *)(*(char **)itemmaker_item + 69 + (l_18 << 2)) = (int)(unsigned char)*(signed char *)((char *)(int)(*(char **)(enchant_spell_lists + (((int)(short)*(short *)(itemmaker_slots + (l_1C << 2))) << 2)) + ((int)(short)*(short *)(D_001998E2 + (l_1C << 2)))));
L57095:;
    if (((int)(signed char)*(signed char *)(itemmaker_slot_kinds + l_1C)) == (-1)) goto L570AA;
    l_18++;
L570AA:;
    goto L56FA0;
L570AF:;
    if (l_18 < 10) goto L570BF;
    goto L570D5;
L570B7:;
    l_18++;
    goto L570AF;
L570BF:;
    *(short *)(*(char **)itemmaker_item + 67 + (l_18 << 2)) = 65535;
    goto L570B7;
L570D5:;
    *(signed char *)(*(char **)itemmaker_item + 42) |= 32;
    func_00057F42();
    itemmaker_consume_soul();
    itemmaker_store_item();
    sound_play(207, *(int *)player_object, 100);
    itemmaker_reset();
}

void itemmaker_store_item(void)
{
    inv_store_item(*(int *)itemmaker_item_object);
    *(int *)itemmaker_item = 0;
    *(int *)itemmaker_item_object = 0;
}

void itemmaker_remove_slot(short a1)
{
    if (((int)(short)a1) == (-1)) goto L571EE;
    if (*(signed char *)(D_00199910 + ((int)(short)a1)) == 0) goto L571F0;
L571EE:;
    return;
L571F0:;
    if (((int)(signed char)*(signed char *)(itemmaker_slot_kinds + ((int)(short)a1))) != 1) goto L57211;
    if (*(short *)(itemmaker_slots + (((int)(short)a1) << 2)) == 0) goto L57213;
L57211:;
    goto L57218;
L57213:;
    itemmaker_clear_soul_slots();
L57218:;
    mc_memset(((int)D_00199868) + (((int)(short)a1) * 10), -1, 10, (int)D_001756A3, 511, 4);
    *(signed char *)(itemmaker_slot_kinds + ((int)(short)a1)) = 255;
    *(short *)(D_001998E2 + (((int)(short)a1) << 2)) = 0;
}

void itemmaker_soul_list_cb(int a1)
{
    int l_18;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 20) goto L5728F;
    if (*(int *)D_00195B84 <= 62) goto L57294;
L5728F:;
    return;
L57294:;
    if (*(signed char *)D_00190D63 != *(unsigned short *)((char *)a1 + 27)) goto L572B3;
    *(int *)guild_npc_object = a1;
L572B3:;
    *(signed char *)(text_rsc_buffer + *(int *)D_00195B84) = *(signed char *)((char *)a1 + 27);
    *(int *)(D_00190EE4 + ((*(int *)D_00195B84)++ << 2)) = *(int *)D_00190EDC;
    mc_strncpy(*(int *)D_00190EDC, *(int *)(monster_names + (((int)(unsigned short)*(short *)((char *)a1 + 27)) << 2)), 4, (int)D_001756A3, 526);
    l_18 = *(int *)D_00190EDC;
    l_18 += func_000A0DF4(*(int *)(monster_names + (((int)(unsigned short)*(short *)((char *)a1 + 27)) << 2))) + 1;
    *(int *)D_00190EDC = l_18;
}

int itemmaker_pick_param_list(int a1)
{
    switch ((unsigned)a1) {
case 1:
case 2:
case 3:
    itemmaker_show_param_list(spells_std_names_for_ids(*(int *)(D_00185871 + (a1 << 2))), (int)(short)(a1 - 1));
    goto L5741B;
case 4:
    *(int *)D_00190EE4 = *(int *)D_00195C44 + 20000;
    *(int *)D_00190EDC = *(int *)D_00195C44 + 21000;
    *(int *)D_00195B84 = 0;
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)itemmaker_soul_list_cb);
    if (*(int *)D_00195B84 != 0) goto L573E0;
    if (((int)(short)*(short *)D_00190D68) != 2) goto L573E2;
L573E0:;
    goto L573FA;
L573E2:;
    msgbox_show_string((int)D_001756EC, 1);
    return 0;
L573FA:;
    *(int *)(D_00190EE4 + (*(int *)D_00195B84 << 2)) = 0;
    itemmaker_show_param_list((int)D_00190EE4, 1000);
default:
L5741B:;
    return 1;
}
}

int itemmaker_free_slot(void)
{
    int l_1C;

    l_1C = 0;
L57444:;
    if (((int)(signed char)*(signed char *)(itemmaker_slot_kinds + l_1C)) == (-1)) goto L57459;
    if (l_1C < 10) goto L5745B;
L57459:;
    goto L57463;
L5745B:;
    l_1C++;
    goto L57444;
L57463:;
    if (l_1C != 10) goto L57472;
    return -1;
L57472:;
    return l_1C;
}

int itemmaker_free_slot_count(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = l_20;
L574A0:;
    if (l_20 < 10) goto L574B0;
    goto L574C7;
L574A8:;
    l_20++;
    goto L574A0;
L574B0:;
    if (((int)(signed char)*(signed char *)(itemmaker_slot_kinds + l_20)) != (-1)) goto L574C5;
    l_1C++;
L574C5:;
    goto L574A8;
L574C7:;
    return l_1C;
}

void itemmaker_show_list(int a1, int a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_14 = 0;
    l_18 = l_14;
L574FA:;
    if (*(int *)((char *)a1) == 0) goto L576DB;
    l_1C = 0;
L5750D:;
    if (((int)(short)*(short *)&l_1C) < 10) goto L57523;
    goto L57621;
L5751B:;
    l_1C++;
    goto L5750D;
L57523:;
    if ((short)((unsigned short)(unsigned char)*(signed char *)(D_00199868 + (((int)(short)*(short *)&l_1C) * 10))) != *(short *)&a2) goto L57551;
    if (((int)(unsigned char)*(signed char *)(D_00199869 + (((int)(short)*(short *)&l_1C) * 10))) == 255) goto L57581;
L57551:;
    if ((short)((unsigned short)(unsigned char)*(signed char *)(D_0019986A + (((int)(short)*(short *)&l_1C) * 10))) != *(short *)&a2) goto L5757F;
    if (((int)(unsigned char)*(signed char *)(D_0019986B + (((int)(short)*(short *)&l_1C) * 10))) == 255) goto L57581;
L5757F:;
    goto L57583;
L57581:;
    goto L575B3;
L57583:;
    if ((short)((unsigned short)(unsigned char)*(signed char *)(D_0019986C + (((int)(short)*(short *)&l_1C) * 10))) != *(short *)&a2) goto L575B1;
    if (((int)(unsigned char)*(signed char *)(D_0019986D + (((int)(short)*(short *)&l_1C) * 10))) == 255) goto L575B3;
L575B1:;
    goto L575B5;
L575B3:;
    goto L575E5;
L575B5:;
    if ((short)((unsigned short)(unsigned char)*(signed char *)(D_0019986E + (((int)(short)*(short *)&l_1C) * 10))) != *(short *)&a2) goto L575E3;
    if (((int)(unsigned char)*(signed char *)(D_0019986F + (((int)(short)*(short *)&l_1C) * 10))) == 255) goto L575E5;
L575E3:;
    goto L575E7;
L575E5:;
    goto L57617;
L575E7:;
    if ((short)((unsigned short)(unsigned char)*(signed char *)(D_00199870 + (((int)(short)*(short *)&l_1C) * 10))) != *(short *)&a2) goto L57615;
    if (((int)(unsigned char)*(signed char *)(D_00199871 + (((int)(short)*(short *)&l_1C) * 10))) == 255) goto L57617;
L57615:;
    goto L5761C;
L57617:;
    goto L576C2;
L5761C:;
    goto L5751B;
L57621:;
    if (((int)(unsigned short)*(short *)(*(char **)itemmaker_item + 32)) == 3) goto L5763D;
    if (((int)(short)*(short *)&a2) == 20) goto L5763F;
L5763D:;
    goto L57644;
L5763F:;
    goto L576C2;
L57644:;
    if (itemmaker_power_excluded((int)(short)*(short *)&a2) != 0) goto L576C2;
    if (((int)(short)*(short *)&a2) == 18) goto L57667;
    if (((int)(short)*(short *)&a2) != 19) goto L57669;
L57667:;
    goto L576C2;
L57669:;
    if (((int)(short)*(short *)&a2) != 15) goto L5767B;
    if (itemmaker_has_soul_bound() != 0) goto L5767D;
L5767B:;
    goto L5767F;
L5767D:;
    goto L576C2;
L5767F:;
    if (((int)(short)*(short *)&a2) != 21) goto L57691;
    if (itemmaker_has_health_leech() != 0) goto L57693;
L57691:;
    goto L57695;
L57693:;
    goto L576C2;
L57695:;
    *(signed char *)((char *)(int)(((int)(short)*(short *)&l_14) + *(char **)D_00195C44) + 64000) = *(signed char *)&l_18;
    *(int *)(D_00190EE4 + (((int)(short)*(short *)&l_14) << 2)) = *(int *)((char *)a1);
    l_14++;
L576C2:;
    (*(char (**)[4])&a1)++;
    (*(short *)&l_18)++;
    a2++;
    goto L574FA;
L576DB:;
    *(int *)(D_00190EE4 + (((int)(short)*(short *)&l_14) << 2)) = 0;
    picklist_open((int)D_00190EE4);
}

int spell_name_by_id(unsigned char a1)
{
    int l_20;

    l_20 = 0;
L57999:;
    if (l_20 < 128) goto L579AC;
    goto L579E8;
L579A4:;
    l_20++;
    goto L57999;
L579AC:;
    if (*(signed char *)((char *)(int)(*(char **)spell_records + (l_20 * 89)) + 47) == 0) goto L579A4;
    if (*(unsigned char *)((char *)(int)(*(char **)spell_records + (l_20 * 89)) + 73) != a1) goto L579E6;
    return (int)(*(char **)spell_records + (l_20 * 89)) + 47;
L579E6:;
    goto L579A4;
L579E8:;
    return (int)D_0017570A;
}

int enchant_spell_cost(unsigned char a1)
{
    int l_28;
    int l_24;
    int l_2C;
    short l_1C;

    l_28 = 0;
L57A14:;
    if (l_28 < 35) goto L57A24;
    goto L57A33;
L57A1C:;
    l_28++;
    goto L57A14;
L57A24:;
    *(short *)(D_001957E9 + (l_28 * 6)) = 50;
    goto L57A1C;
L57A33:;
    *(signed char *)D_001957CD = 1;
    l_28 = 0;
    l_24 = l_28;
L57A47:;
    if (l_28 < 128) goto L57A5A;
    goto L57AA0;
L57A52:;
    l_28++;
    goto L57A47;
L57A5A:;
    if (*(signed char *)((char *)(int)(*(char **)spell_records + (l_28 * 89)) + 47) == 0) goto L57A52;
    if (*(unsigned char *)((char *)(int)(*(char **)spell_records + (l_28 * 89)) + 73) != a1) goto L57A9E;
    return spell_cost((int)(*(char **)spell_records + (l_28 * 89)), (int)D_0019574C) * 10;
L57A9E:;
    goto L57A52;
L57AA0:;
    return 0;
}

int itemmaker_points_used(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_28 = 0;
    l_24 = l_28;
L57ACF:;
    if (l_28 < 10) goto L57AE2;
    goto L57B9B;
L57ADA:;
    l_28++;
    goto L57ACF;
L57AE2:;
    if (((int)(signed char)*(signed char *)(itemmaker_slot_kinds + l_28)) == (-1)) goto L57ADA;
    if (*(signed char *)(D_00199910 + l_28) != 0) goto L57ADA;
    if (*(signed char *)(itemmaker_slot_kinds + l_28) != 0) goto L57B12;
    l_1C = (int)D_001857E5;
    goto L57B19;
L57B12:;
    l_1C = (int)D_001858DB;
L57B19:;
    l_20 = *(int *)((char *)((((int)(short)*(short *)(itemmaker_slots + (l_28 << 2))) << 2) + l_1C));
    if (l_20 == 0) goto L57ADA;
    if (((unsigned)l_20) >= 100) goto L57B6A;
    l_24 += enchant_slot_cost(l_20, (int)(unsigned char)*(signed char *)(D_001998E2 + (l_28 << 2)), 1, (int)(short)*(short *)(itemmaker_slots + (l_28 << 2)));
    goto L57B96;
L57B6A:;
    l_24 += (int)(short)*(short *)((char *)(int)(*(char **)((char *)((((int)(short)*(short *)(itemmaker_slots + (l_28 << 2))) << 2) + l_1C)) + (((int)(short)*(short *)(D_001998E2 + (l_28 << 2))) * 2)));
L57B96:;
    goto L57ADA;
L57B9B:;
    return l_24;
}

int itemmaker_gold_cost(void)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_20 = l_24;
L57D16:;
    if (l_24 < 10) goto L57D29;
    goto L57DB6;
L57D21:;
    l_24++;
    goto L57D16;
L57D29:;
    if (*(signed char *)(itemmaker_slot_kinds + l_24) != 0) goto L57D21;
    l_1C = *(int *)(D_001857E5 + (((int)(short)*(short *)(itemmaker_slots + (l_24 << 2))) << 2));
    if (l_1C == 0) goto L57D21;
    if (((unsigned)l_1C) >= 100) goto L57D84;
    l_20 += enchant_slot_cost(l_1C, (int)(unsigned char)*(signed char *)(D_001998E2 + (l_24 << 2)), 0, (int)(short)*(short *)(itemmaker_slots + (l_24 << 2)));
    goto L57DB1;
L57D84:;
    l_20 += (int)(short)*(short *)((char *)(int)(*(char **)(D_001857E5 + (((int)(short)*(short *)(itemmaker_slots + (l_24 << 2))) << 2)) + (((int)(short)*(short *)(D_001998E2 + (l_24 << 2))) * 2)));
L57DB1:;
    goto L57D21;
L57DB6:;
    return l_20 * 10;
}

void itemmaker_consume_soul(void)
{
    int l_1C;
    int l_18;

    l_1C = 0;
L57DDF:;
    if (l_1C < 10) goto L57DEF;
    goto L57E11;
L57DE7:;
    l_1C++;
    goto L57DDF;
L57DEF:;
    if (*(signed char *)(itemmaker_slot_kinds + l_1C) <= 0) goto L57E0B;
    if (*(short *)(itemmaker_slots + (l_1C << 2)) == 0) goto L57E0D;
L57E0B:;
    goto L57E0F;
L57E0D:;
    goto L57E13;
L57E0F:;
    goto L57DE7;
L57E11:;
    return;
L57E13:;
    *(signed char *)D_00190D63 = *(signed char *)(D_001998E2 + (l_1C << 2));
    *(int *)D_00195B84 = 0;
    object_foreach(*(int *)(*(char **)player_entity + 63), (int)itemmaker_soul_list_cb);
    l_18 = *(int *)(*(char **)guild_npc_object + 67);
    object_delete(*(int *)guild_npc_object);
    if (((int)(short)*(short *)((char *)l_18 + 138)) == 26) return;
    object_free_single(l_18);
}

int itemmaker_has_soul_bound(void)
{
    int l_1C;

    l_1C = 0;
L57E8B:;
    if (l_1C < 10) goto L57E9B;
    goto L57EC7;
L57E93:;
    l_1C++;
    goto L57E8B;
L57E9B:;
    if (((int)(signed char)*(signed char *)(itemmaker_slot_kinds + l_1C)) != 1) goto L57EBA;
    if (*(short *)(itemmaker_slots + (l_1C << 2)) == 0) goto L57EBC;
L57EBA:;
    goto L57EC5;
L57EBC:;
    return 1;
L57EC5:;
    goto L57E93;
L57EC7:;
    return 0;
}

int itemmaker_has_health_leech(void)
{
    int l_1C;

    l_1C = 0;
L57EF0:;
    if (l_1C < 10) goto L57F00;
    goto L57F2E;
L57EF8:;
    l_1C++;
    goto L57EF0;
L57F00:;
    if (((int)(signed char)*(signed char *)(itemmaker_slot_kinds + l_1C)) != 1) goto L57F21;
    if (((int)(short)*(short *)(itemmaker_slots + (l_1C << 2))) == 6) goto L57F23;
L57F21:;
    goto L57F2C;
L57F23:;
    return 1;
L57F2C:;
    goto L57EF8;
L57F2E:;
    return 0;
}

void func_00057F42(void)
{
    int l_18;

    if (*(signed char *)cfg_item_file == 0) return;
    l_18 = disk_create((int)cfg_item_file);
    write(l_18, *(int *)itemmaker_item, 107);
    func_0009DEA7(l_18);
}

void itemmaker_clear_soul_slots(void)
{
    int l_18;

    l_18 = 0;
L58488:;
    if (l_18 < 10) goto L58498;
    goto L58503;
L58490:;
    l_18++;
    goto L58488;
L58498:;
    if (*(signed char *)(D_00199910 + l_18) == 0) goto L58490;
    *(signed char *)(D_00199910 + l_18) = 0;
    *(signed char *)(itemmaker_slot_kinds + l_18) = 255;
    *(short *)(itemmaker_slots + (l_18 << 2)) = (*(short *)(D_001998E2 + (l_18 << 2)) = 0);
    mc_memset(((int)D_00199868) + (l_18 * 10), -1, 10, (int)D_001756A3, 939, 4);
    goto L58490;
L58503:;
    mc_memset((int)D_001998CC, -1, 20, (int)D_001756A3, 942, 4);
}

void func_0005852D(int a1)
{
    int l_18;

    *(short *)inv_left_count = (*(short *)D_001AA586 = 0);
    mc_memset((int)inv_left_rows, 0, 20, (int)D_001756A3, 951, 20);
    if (*(int *)inv_left_container == *(int *)player_entity) goto L58596;
    inv_draw_item_cell(*(int *)inv_left_container, 0, a1);
    *(int *)D_001AA578 = *(int *)inv_left_container;
L58596:;
    l_18 = *(int *)(*(char **)inv_left_container + 63);
L585A1:;
    if (l_18 == 0) goto L585C0;
    func_000585D6(l_18, a1 + 12);
    l_18 = *(int *)((char *)l_18 + 55);
    goto L585A1;
L585C0:;
    *(short *)inv_left_count = *(short *)D_001AA586;
}

void func_000585D6(int a1, int a2)
{
    int l_14;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 2) goto L5860D;
    if (((int)(unsigned short)(*(short *)((char *)a1 + 21) & 2)) == 0) goto L5860F;
L5860D:;
    return;
L5860F:;
    if (((int)(short)*(short *)D_001AA586) < *(int *)inv_left_scroll) goto L58631;
    if (((int)(short)*(short *)D_001AA586) < (*(int *)inv_left_scroll + 4)) goto L58633;
L58631:;
    goto L58669;
L58633:;
    *(int *)(inv_left_rows + ((((int)(short)*(short *)D_001AA586) - *(int *)inv_left_scroll) << 2)) = a1;
    inv_draw_item_cell(a1, (int)(short)(*(short *)D_001AA586 - *(short *)inv_left_scroll), a2);
L58669:;
    (*(short *)D_001AA586)++;
}

void itemmaker_list_parent(void)
{
    if (((int)(unsigned char)*(signed char *)(*(char **)inv_left_container)) == 52) return;
    *(int *)inv_left_container = *(int *)(*(char **)inv_left_container + 67);
}

void itemmaker_pick_item(int a1)
{
    int l_1C;
    int l_18;

    a1 += -9;
    l_1C = *(int *)(inv_left_rows + (a1 << 2));
    if (l_1C == 0) return;
    l_18 = l_1C + 71;
    if (((int)(short)*(short *)((char *)l_18 + 67)) == (-1)) goto L58706;
    msgbox_show_rsc(1660, 1);
    return;
L58706:;
    if (*(short *)((char *)l_18 + 61) != 0) goto L58724;
    msgbox_show_rsc(1659, 1);
    return;
L58724:;
    a1 = 0;
L5872B:;
    if (a1 < 27) goto L5873B;
    goto L5876E;
L58733:;
    a1++;
    goto L5872B;
L5873B:;
    if (*(int *)(*(char **)player_character + 367 + (a1 << 2)) != l_1C) goto L5876C;
    *(int *)(*(char **)player_character + 367 + (a1 << 2)) = 0;
L5876C:;
    goto L58733;
L5876E:;
    *(int *)itemmaker_item_object = l_1C;
    *(int *)D_00190BE4 = (int)(unsigned short)*(short *)((char *)(*(int *)itemmaker_item = l_18) + 61);
}

int enchant_item_value(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_20 = l_24;
L587B9:;
    if (l_24 < 10) goto L587CC;
    goto L5887D;
L587C4:;
    l_24++;
    goto L587B9;
L587CC:;
    if (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67)) >= 16) goto L587C4;
    if (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67)) == (-1)) goto L587C4;
    l_1C = *(int *)(D_001857E5 + (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67)) << 2));
    if (l_1C == 0) goto L587C4;
    if (l_1C >= 100) goto L5881B;
    if (l_1C > 0) goto L5881D;
L5881B:;
    goto L5884B;
L5881D:;
    l_20 += enchant_value_slot_cost(l_1C, (int)(unsigned char)*(signed char *)((char *)((l_24 << 2) + a1) + 69), 0, (int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67));
    goto L58878;
L5884B:;
    l_20 += (int)(short)*(short *)((char *)(int)(*(char **)(D_001857E5 + (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67)) << 2)) + (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 69)) * 2)));
L58878:;
    goto L587C4;
L5887D:;
    return l_20;
}

int itemmaker_param_excluded(int a1, int a2)
{
    int l_18;

    func_00058AF7();
    if (a1 != 25) goto L5895F;
    l_18 = 0;
L588B9:;
    if (l_18 < 10) goto L588CC;
    goto L5895A;
L588C4:;
    l_18++;
    goto L588B9;
L588CC:;
    if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) != 25) goto L588F0;
    if (((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) != 5) goto L588F2;
L588F0:;
    goto L588F8;
L588F2:;
    if (a2 == 5) goto L588FA;
L588F8:;
    goto L588FF;
L588FA:;
    goto L58A19;
L588FF:;
    if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) != 25) goto L58923;
    if (((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) == 5) goto L58925;
L58923:;
    goto L5892A;
L58925:;
    goto L58A19;
L5892A:;
    if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) != 14) goto L5894E;
    if (((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) == a2) goto L58950;
L5894E:;
    goto L58955;
L58950:;
    goto L58A19;
L58955:;
    goto L588C4;
L5895A:;
    goto L58A0B;
L5895F:;
    if (a1 != 14) goto L58A0B;
    l_18 = 0;
L58970:;
    if (l_18 < 10) goto L58983;
    goto L58A0B;
L5897B:;
    l_18++;
    goto L58970;
L58983:;
    if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) != 14) goto L589A7;
    if (((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) != 5) goto L589A9;
L589A7:;
    goto L589AF;
L589A9:;
    if (a2 == 5) goto L589B1;
L589AF:;
    goto L589B6;
L589B1:;
    goto L58A19;
L589B6:;
    if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) != 14) goto L589DA;
    if (((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) == 5) goto L589DC;
L589DA:;
    goto L589DE;
L589DC:;
    goto L58A19;
L589DE:;
    if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) != 25) goto L58A02;
    if (((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) == a2) goto L58A04;
L58A02:;
    goto L58A06;
L58A04:;
    goto L58A19;
L58A06:;
    goto L5897B;
L58A0B:;
    func_00058AF7();
    return 0;
L58A19:;
    func_00058AF7();
    return 1;
}

void func_00058AF7(void)
{
    int l_18;

    l_18 = 0;
L58B0C:;
    if (l_18 < 10) goto L58B1C;
    return;
L58B14:;
    l_18++;
    goto L58B0C;
L58B1C:;
    if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) == (-1)) goto L58B14;
    if (*(signed char *)(itemmaker_slot_kinds + l_18) <= 0) goto L58B6A;
    if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) < 15) goto L58B5C;
    *(short *)(itemmaker_slots + (l_18 << 2)) -= 15;
    goto L58B6A;
L58B5C:;
    *(short *)(itemmaker_slots + (l_18 << 2)) += 15;
L58B6A:;
    goto L58B14;
}

int enchant_powers_text(int a1)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = (int)D_001913E4;
    if (((int)(unsigned short)(*(short *)((char *)a1 + 42) & 32)) != 0) goto L58BB6;
    return (int)D_00175723;
L58BB6:;
    l_20 = 0;
L58BBD:;
    if (l_20 < 10) goto L58BD0;
    goto L58DF5;
L58BC8:;
    l_20++;
    goto L58BBD;
L58BD0:;
    if (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 67)) == (-1)) goto L58BC8;
    if (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 67)) >= 15) goto L58CD5;
    func_000A0ED9(1128, (int)D_001756A3);
    mc_sprintf((int)text_buffer, (int)D_00175734, *(int *)(enchant_power_names + (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 67)) << 2)));
    if (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 69)) == (-1)) goto L58CD0;
    if (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 67)) >= 3) goto L58C8D;
    func_000A1054((int)text_buffer, spell_name_by_id((int)(unsigned char)*(signed char *)((char *)((l_20 << 2) + a1) + 69)), (int)D_001756A3, 1132, 160);
    goto L58CD0;
L58C8D:;
    func_000A1054((int)text_buffer, *(int *)((char *)(int)(*(char **)(enchant_power_params + (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 67)) << 2)) + (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 69)) << 2))), (int)D_001756A3, 1134, 160);
L58CD0:;
    goto L58DAB;
L58CD5:;
    func_000A0ED9(1139, (int)D_001756A3);
    mc_sprintf((int)text_buffer, (int)D_00175734, *(int *)(D_00180ACE + (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 67)) << 2)));
    if (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 69)) == (-1)) goto L58DAB;
    if (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 67)) != 15) goto L58D68;
    func_000A1054((int)text_buffer, *(int *)(monster_names + (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 69)) << 2)), (int)D_001756A3, 1143, 160);
    goto L58DAB;
L58D68:;
    func_000A1054((int)text_buffer, *(int *)((char *)(int)(*(char **)(D_0018586F + (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 67)) << 2)) + (((int)(short)*(short *)((char *)((l_20 << 2) + a1) + 69)) << 2))), (int)D_001756A3, 1145, 160);
L58DAB:;
    func_000A1054((int)text_buffer, (int)D_00175738, (int)D_001756A3, 1148, 160);
    mc_strncpy(l_1C, (int)text_buffer, 4, (int)D_001756A3, 1149);
    l_1C += func_000A0DF4(l_1C);
    goto L58BC8;
L58DF5:;
    l_1C++;
    *(signed char *)((char *)l_1C) = 0;
    return (int)D_001913E4;
}
