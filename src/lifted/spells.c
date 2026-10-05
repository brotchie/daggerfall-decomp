/* spells.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_0_1 { unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012DA44[];
extern char key_down_esc[];
extern char screen_buffer[];
extern char D_00170B13[];
extern char D_00170B1E[];
extern char D_00170B4C[];
extern char D_00170B69[];
extern char selected_spell[];
extern char spell_effect_school[];
extern char spell_effect_target_class[];
extern char spell_effect_settings[];
extern char spell_effect_costs[];
extern char spell_effect_cost_index[];
extern char spell_effect_cost_formula[];
extern char spell_target_cost_factor[];
extern char D_0017D1F6[];
extern char magic_school_skills[];
extern char D_001940D4[];
extern char D_001940D5[];
extern char player_entity[];
extern char player_object[];
extern char cheat_flags[];
extern char picklist_image[];
extern char spell_records[];
extern char guild_npc_object[];
extern char spellshop_icons[];
extern char list_popup_callback[];
extern char window_image[];
extern char D_00195C44[];
extern char spell_effect_slot[];
extern char D_00195F3C[];
extern char D_00195F3E[];
extern char D_00195F40[];
extern char D_00195F42[];
extern char picklist_control[];
extern char spellmaker_spell[];
extern char D_00196272[];
extern char game_mode[];
extern char mouse_buttons_prev[];
extern char spell_effect_cost_current[];
extern char spellmaker_settings_image[];
extern char D_00199628[];
extern char D_0019962C[];
extern char spellmaker_settings_kind[];
extern char D_0019962F[];
extern char D_00199630[];

extern int func_00037AB7(void);
extern int spell_effect_text_index(short);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int disk_write_arena2_file(int, int, int);
extern int picklist_update(void);
extern int gold_can_afford(int);
extern int picklist_poll(int);
extern int object_create_child(int, int, int);
extern int object_find_item(int, int, int);
extern int object_count_type(int, short);
extern int object_new_id(int);
extern int mc_free();
extern int mc_memset();
extern int mc_strncpy();
extern int func_000A0DF4();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int memchr();
extern int spell_cost_formula_dispatch();
extern int func_0012B136();
extern int func_0012B2D3();
extern int func_0012DB50();
extern int func_00144F68();
extern void spellmaker_adjust_value(int, short, int, int);
extern void skill_add_uses(int, int);
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void keys_world_actions(void);
extern void picklist_open(int);
extern void msgbox_choice_rsc(short, unsigned char, unsigned char, int, unsigned char, unsigned char, unsigned char);
extern void gold_spend(int);
extern void inpstr_begin_text(int, short);
extern void picklist_free(int);
extern void picklist_draw(int, int);
extern void object_foreach(int, int);
int func_00037D5A(void);
int spellmaker_new(void);
int spellbook_has_spell_id(unsigned char);
int spells_pick_list(void);
int spells_std_name_list(int);
void func_00037DBB(unsigned char);
void spellmaker_pick_subtype_cb(int);
void spell_assign_new_id(void);
void spellbook_find_id_cb(int);
void func_00039F94(void);
void func_00039FD6(void);
void spells_std_append(void);
#pragma aux func_000A0ED9 parm routine [];

int spellmaker_exit(void)
{
L37740:;
    if (*(signed char *)key_down_esc != 0) goto L37740;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37795;
    if (((struct bf8_0_1 *)&cheat_flags)->f == 0) goto L37773;
    func_00039F94();
    return 0;
L37773:;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1814, 1);
    return 0;
L37795:;
    *(signed char *)game_mode = 0;
    if (*(int *)window_image == 0) goto L377B1;
    if (*(int *)window_image != (-1751672937)) goto L377B3;
L377B1:;
    goto L377D1;
L377B3:;
    mc_free(*(int *)window_image, (int)D_00170B13, 732);
    *(int *)window_image = -1751672937;
L377D1:;
    if (*(int *)spellshop_icons == 0) goto L377E6;
    if (*(int *)spellshop_icons != (-1751672937)) goto L377E8;
L377E6:;
    goto L37806;
L377E8:;
    mc_free(*(int *)spellshop_icons, (int)D_00170B13, 733);
    *(int *)spellshop_icons = -1751672937;
L37806:;
    if (*(int *)spellmaker_settings_image == 0) goto L3781B;
    if (*(int *)spellmaker_settings_image != (-1751672937)) goto L3781D;
L3781B:;
    goto L3783B;
L3781D:;
    mc_free(*(int *)spellmaker_settings_image, (int)D_00170B13, 734);
    *(int *)spellmaker_settings_image = -1751672937;
L3783B:;
    *(signed char *)D_00196272 = 0;
    return 1;
}

void spellmaker_enter_name(void)
{
    int l_18;

    l_18 = *(int *)D_00195C44 + 55000;
    func_000A0ED9(744, (int)D_00170B13);
    mc_sprintf(l_18, (int)D_00170B1E, *(int *)D_0017D1F6);
    *(signed char *)((char *)(func_000A0DF4(l_18) + l_18) + 1) = 0;
    inpstr_begin_text(*(int *)selected_spell + 47, 24);
    msgbox_show_string(l_18, 2);
}

int spellmaker_element_fire(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L3790E;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1801, 1);
    return 0;
L3790E:;
    if (func_00037AB7() <= 0) goto L37920;
    *(signed char *)(*(char **)selected_spell + 6) = 0;
L37920:;
    return 0;
}

int spellmaker_element_frost(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37971;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1802, 1);
    return 0;
L37971:;
    if (func_00037AB7() <= 0) goto L37983;
    *(signed char *)(*(char **)selected_spell + 6) = 1;
L37983:;
    return 0;
}

int spellmaker_element_poison(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L379D4;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1803, 1);
    return 0;
L379D4:;
    if (func_00037AB7() <= 0) goto L379E6;
    *(signed char *)(*(char **)selected_spell + 6) = 2;
L379E6:;
    return 0;
}

int spellmaker_element_shock(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37A37;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1804, 1);
    return 0;
L37A37:;
    if (func_00037AB7() <= 0) goto L37A49;
    *(signed char *)(*(char **)selected_spell + 6) = 3;
L37A49:;
    return 0;
}

int spellmaker_element_magic(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37A9A;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1805, 1);
    return 0;
L37A9A:;
    *(signed char *)(*(char **)selected_spell + 6) = 4;
    return 0;
}

int spellmaker_target_caster(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37BA7;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1806, 1);
    return 0;
L37BA7:;
    if (func_00037D5A() == 1) goto L37BBA;
    *(signed char *)(*(char **)selected_spell + 7) = 0;
L37BBA:;
    return 0;
}

int spellmaker_target_touch(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37C0B;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1807, 1);
    return 0;
L37C0B:;
    if (func_00037D5A() == 0) goto L37C1D;
    *(signed char *)(*(char **)selected_spell + 7) = 1;
L37C1D:;
    return 0;
}

int spellmaker_target_distance(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37C6E;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1808, 1);
    return 0;
L37C6E:;
    if (func_00037D5A() == 0) goto L37C80;
    *(signed char *)(*(char **)selected_spell + 7) = 2;
L37C80:;
    return 0;
}

int spellmaker_target_area(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37CD1;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1809, 1);
    return 0;
L37CD1:;
    if (func_00037D5A() == 0) goto L37CE3;
    *(signed char *)(*(char **)selected_spell + 7) = 3;
L37CE3:;
    return 0;
}

int spellmaker_target_area_at_distance(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37D34;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1810, 1);
    return 0;
L37D34:;
    if (func_00037D5A() == 0) goto L37D46;
    *(signed char *)(*(char **)selected_spell + 7) = 4;
L37D46:;
    return 0;
}

int func_00037D5A(void)
{
    *(signed char *)D_0019962F = 2;
    func_00037DBB((int)(unsigned char)*(signed char *)(*(char **)selected_spell));
    func_00037DBB((int)(unsigned char)*(signed char *)(*(char **)selected_spell + 2));
    func_00037DBB((int)(unsigned char)*(signed char *)(*(char **)selected_spell + 4));
    return (int)(unsigned char)*(signed char *)D_0019962F;
}

void func_00037DBB(unsigned char a1)
{
    if (((int)(unsigned char)a1) == 255) return;
    if (((int)(unsigned char)*(signed char *)(spell_effect_target_class + ((int)(unsigned char)a1))) == 2) return;
    *(signed char *)D_0019962F = *(signed char *)(spell_effect_target_class + ((int)(unsigned char)a1));
}

int spellmaker_buy(void)
{
    int l_20;
    int l_1C;

    if (((int)(unsigned char)*(signed char *)(*(char **)selected_spell)) != 255) goto L37E3C;
    if (((int)(unsigned char)*(signed char *)(*(char **)selected_spell + 2)) == 255) goto L37E3E;
L37E3C:;
    goto L37E52;
L37E3E:;
    if (((int)(unsigned char)*(signed char *)(*(char **)selected_spell + 4)) == 255) goto L37E54;
L37E52:;
    goto L37E60;
L37E54:;
    return 0;
L37E60:;
    if (*(signed char *)(*(char **)selected_spell + 47) != 0) goto L37E86;
    msgbox_show_rsc(1704, 1);
    return 0;
L37E86:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L37ED2;
    if (((struct bf8_0_1 *)&cheat_flags)->f == 0) goto L37EB0;
    spells_std_append();
    return 0;
L37EB0:;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1812, 1);
    return 0;
L37ED2:;
    l_1C = (((int)(unsigned short)*(short *)(*(char **)selected_spell + 8)) + ((int)(unsigned short)*(short *)(*(char **)selected_spell + 10))) + ((int)(unsigned short)*(short *)(*(char **)selected_spell + 12));
    l_1C = (((int)(short)*(short *)(spell_target_cost_factor + (((int)(unsigned char)*(signed char *)(*(char **)selected_spell + 7)) * 2))) * l_1C) * 2;
    if (gold_can_afford(l_1C) != 0) goto L37F46;
    msgbox_show_rsc(1702, 1);
    return 0;
L37F46:;
    l_20 = object_find_item(*(int *)(*(char **)player_entity + 63), 27, 0);
    if (l_20 != 0) goto L37F7E;
    msgbox_show_string((int)D_00170B4C, 1);
    return 0;
L37F7E:;
    if (object_count_type(*(int *)((char *)l_20 + 63), 9) <= 128) goto L37FB0;
    msgbox_show_rsc(1709, 1);
    return 0;
L37FB0:;
    if (l_20 != 0) goto L37FD1;
    msgbox_show_rsc(1703, 1);
    return 0;
L37FD1:;
    gold_spend(l_1C);
    spell_assign_new_id();
    l_20 = object_create_child(l_20, 0, 89);
    *(signed char *)((char *)l_20) = 9;
    *(int *)((char *)l_20 + 31) = object_new_id(100);
    mc_memcpy(l_20 + 71, *(int *)selected_spell, 89, (int)D_00170B13, 981, 4);
    sound_play(206, *(int *)player_object, 110);
    spellmaker_new();
    msgbox_show_rsc(1705, 1);
    return 0;
}

int spellmaker_new(void)
{
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L380C1;
    if (((struct bf8_0_1 *)&cheat_flags)->f == 0) goto L3809F;
    func_00039FD6();
    return 0;
L3809F:;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc(1813, 1);
    return 0;
L380C1:;
    *(int *)selected_spell = (int)spellmaker_spell;
    mc_memset(*(int *)selected_spell, 1, 89, (int)D_00170B13, 1009, 4);
    mc_memset(*(int *)selected_spell + 47, 0, 24, (int)D_00170B13, 1010, 25);
    *(signed char *)(*(char **)selected_spell + 7) = 0;
    *(signed char *)(*(char **)selected_spell + 6) = 4;
    mc_memset(*(int *)selected_spell, 255, 6, (int)D_00170B13, 1015, 6);
    *(short *)(*(char **)selected_spell + 8) = (*(short *)(*(char **)selected_spell + 10) = (*(short *)(*(char **)selected_spell + 12) = 0));
    *(short *)D_0019962C = 65535;
    return 0;
}

void spellmaker_pick_subtype_cb(int a1)
{
    short l_18;

    l_18 = (int)(unsigned char)*(signed char *)((char *)(int)(*(char **)selected_spell + (((int)(short)*(short *)spell_effect_slot) * 2)));
    mc_memcpy((int)spell_effect_cost_current, ((int)spell_effect_costs) + (((int)(unsigned char)*(signed char *)(spell_effect_cost_index + ((((int)(short)l_18) * 12) + ((int)(short)*(short *)&a1)))) << 3), 8, (int)D_00170B13, 1030, 8);
    *(signed char *)(*(char **)selected_spell + 1 + (((int)(short)*(short *)spell_effect_slot) * 2)) = *(signed char *)&a1;
    *(signed char *)spellmaker_settings_kind = *(signed char *)(spell_effect_settings + ((((int)(short)l_18) * 12) + ((int)(short)*(short *)&a1)));
    *(short *)D_00199628 = 0;
}

int spellmaker_find_effect(short a1)
{
    short l_18;

    *(int *)&l_18 = 0;
L38554:;
    if (((int)(short)l_18) < 3) goto L38567;
    goto L3858B;
L3855F:;
    (*(int *)&l_18)++;
    goto L38554;
L38567:;
    if ((short)((unsigned short)(unsigned char)*(signed char *)((char *)(int)(*(char **)selected_spell + (((int)(short)l_18) * 2)))) != a1) goto L38589;
    return (int)(short)l_18;
L38589:;
    goto L3855F;
L3858B:;
    return -1;
}

void spellmaker_duration_base(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 14 + (((int)(short)*(short *)spell_effect_slot) * 3)), (int)(short)a1, 60, 0);
}

void spellmaker_duration_plus(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 14 + (((int)(short)*(short *)spell_effect_slot) * 3)) + 1, (int)(short)a1, 60, 0);
}

void spellmaker_duration_per_level(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 14 + (((int)(short)*(short *)spell_effect_slot) * 3)) + 2, (int)(short)a1, 20, 0);
}

void spellmaker_chance_base(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 23 + (((int)(short)*(short *)spell_effect_slot) * 3)), (int)(short)a1, 100, 0);
}

void spellmaker_chance_plus(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 23 + (((int)(short)*(short *)spell_effect_slot) * 3)) + 1, (int)(short)a1, 100, 0);
}

void spellmaker_chance_per_level(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 23 + (((int)(short)*(short *)spell_effect_slot) * 3)) + 2, (int)(short)a1, 20, 0);
}

void spellmaker_magnitude_base_min(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 32 + (((int)(short)*(short *)spell_effect_slot) * 5)), (int)(short)a1, 100, 1);
}

void spellmaker_magnitude_base_max(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 32 + (((int)(short)*(short *)spell_effect_slot) * 5)) + 1, (int)(short)a1, 100, -1);
}

void spellmaker_magnitude_plus_min(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 32 + (((int)(short)*(short *)spell_effect_slot) * 5)) + 2, (int)(short)a1, 100, 1);
}

void spellmaker_magnitude_plus_max(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 32 + (((int)(short)*(short *)spell_effect_slot) * 5)) + 3, (int)(short)a1, 100, -1);
}

void spellmaker_magnitude_per_level(short a1)
{
    spellmaker_adjust_value((int)(*(char **)selected_spell + 32 + (((int)(short)*(short *)spell_effect_slot) * 5)) + 4, (int)(short)a1, 20, 0);
}

void func_000390AD(void)
{
    *(signed char *)D_001940D5 |= 32;
    *(signed char *)spellmaker_settings_kind = 0;
}

void spellmaker_effect_rows(void)
{
    short l_18;

    *(int *)&l_18 = 0;
L390E8:;
    if (((int)(short)l_18) < 3) goto L390FE;
    return;
L390F6:;
    (*(int *)&l_18)++;
    goto L390E8;
L390FE:;
    if (((int)(unsigned char)*(signed char *)((char *)(int)(*(char **)selected_spell + (((int)(short)l_18) * 2)))) == 255) goto L390F6;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L39138;
    if (*(signed char *)mouse_buttons_prev != *(signed char *)mouse_buttons) goto L3913A;
L39138:;
    goto L3914F;
L3913A:;
    if (((int)(short)*(short *)mouse_y) > ((((int)(short)l_18) << 5) + 30)) goto L39151;
L3914F:;
    goto L3916F;
L39151:;
    if (((int)(short)*(short *)mouse_y) < (((((int)(short)l_18) << 5) + 30) + ((int)(short)*(short *)D_0012DA44))) goto L39171;
L3916F:;
    goto L39191;
L39171:;
    *(signed char *)D_001940D5 |= 1;
    msgbox_show_rsc((int)(short)(spell_effect_text_index((int)(short)l_18) + 1200), 1);
L39191:;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 1)) == 0) goto L391AE;
    if (*(signed char *)mouse_buttons_prev != *(signed char *)mouse_buttons) goto L391B0;
L391AE:;
    goto L391C5;
L391B0:;
    if (((int)(short)*(short *)mouse_y) > ((((int)(short)l_18) << 5) + 30)) goto L391C7;
L391C5:;
    goto L391E5;
L391C7:;
    if (((int)(short)*(short *)mouse_y) < (((((int)(short)l_18) << 5) + 30) + ((int)(short)*(short *)D_0012DA44))) goto L391E7;
L391E5:;
    goto L39215;
L391E7:;
    *(short *)D_0019962C = *(int *)&l_18;
    msgbox_choice_rsc(1708, 11, 10, 0, 101, 100, 0);
L39215:;
    goto L390F6;
}

int spells_list_poll(void)
{
    short l_18;

    func_00144F68((int)(short)*(short *)D_00195F40, (int)(short)*(short *)D_00195F3E, (int)(short)*(short *)D_00195F42, (int)(short)*(short *)D_00195F3C, *(int *)picklist_image + 12);
    if (*(signed char *)key_down_esc == 0) goto L3932A;
    if (((int)spellmaker_pick_subtype_cb) != *(int *)list_popup_callback) goto L39310;
    *(signed char *)((char *)(int)(*(char **)selected_spell + (((int)(short)*(short *)spell_effect_slot) * 2))) = 255;
L39310:;
    *(signed char *)D_001940D4 &= 251;
    picklist_free((int)picklist_control);
    return -2;
L3932A:;
    *(int *)&l_18 = picklist_poll((int)picklist_control) - 1;
    if (((int)(short)l_18) <= (-1)) goto L3935B;
    *(signed char *)D_001940D4 &= 251;
    picklist_free((int)picklist_control);
    return (int)(short)l_18;
L3935B:;
    picklist_draw((int)picklist_control, 0);
    return -1;
}

int spell_icon_cycle(void)
{
    short l_18;

    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L399BA;
    *(int *)&l_18 = -1;
    goto L399C1;
L399BA:;
    *(int *)&l_18 = 1;
L399C1:;
    *(signed char *)(*(char **)selected_spell + 72) = (((int)(unsigned char)*(signed char *)(*(char **)selected_spell + 72)) + ((int)(short)l_18)) % 69;
    return 0;
}

int spell_icon_next(void)
{
    *(signed char *)(*(char **)selected_spell + 72) = (((int)(unsigned char)*(signed char *)(*(char **)selected_spell + 72)) + 1) % 69;
    return 0;
}

int spell_icon_prev(void)
{
    *(signed char *)(*(char **)selected_spell + 72) = (((int)(unsigned char)*(signed char *)(*(char **)selected_spell + 72)) - 1) % 69;
    return 0;
}

void spell_assign_new_id(void)
{
    int l_24;
    int l_20;
    unsigned char l_18;
    short l_1C;

    *(int *)&l_1C = 0;
    l_18 = 0;
    l_24 = *(int *)spell_records;
L39AB0:;
    if (l_1C != 0) goto L39B3A;
    *(int *)&l_1C = 1;
    l_20 = 0;
L39AC9:;
    if (((int)(short)*(short *)&l_20) < 128) goto L39ADE;
    goto L39B11;
L39AD6:;
    l_20++;
    goto L39AC9;
L39ADE:;
    if (*(signed char *)((char *)((((int)(short)*(short *)&l_20) * 89) + l_24) + 47) == 0) goto L39AD6;
    if (*(unsigned char *)((char *)((((int)(short)*(short *)&l_20) * 89) + l_24) + 73) != l_18) goto L39B0F;
    l_18++;
    *(int *)&l_1C = 0;
    goto L39B11;
L39B0F:;
    goto L39AD6;
L39B11:;
    if (l_1C == 0) goto L39B26;
    if (spellbook_has_spell_id((int)(unsigned char)l_18) != 0) goto L39B28;
L39B26:;
    goto L39B35;
L39B28:;
    l_18++;
    *(int *)&l_1C = 0;
L39B35:;
    goto L39AB0;
L39B3A:;
    *(signed char *)(*(char **)selected_spell + 73) = l_18;
}

void spellbook_find_id_cb(int a1)
{
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 9) return;
    if (*(signed char *)((char *)a1 + 144) != *(signed char *)D_00199630) return;
    *(int *)guild_npc_object = a1;
}

int spellbook_has_spell_id(unsigned char a1)
{
    int l_20;
    int l_24;

    *(int *)guild_npc_object = 0;
    *(signed char *)D_00199630 = a1;
    l_20 = object_find_item(*(int *)(*(char **)player_entity + 63), 27, 0);
    object_foreach(*(int *)((char *)l_20 + 63), (int)spellbook_find_id_cb);
    if (*(int *)guild_npc_object == 0) goto L39BEF;
    l_24 = 1;
    goto L39BF6;
L39BEF:;
    l_24 = 0;
L39BF6:;
    return l_24;
}

int spells_pick_list(void)
{
    short l_20;
    int l_28;
    short l_18;
    int l_2C;
    short l_1C;

    l_2C = spells_std_name_list(0);
    if (l_2C != 0) goto L39C33;
    return 0;
L39C33:;
    func_0012DB50(4);
    picklist_open(l_2C);
    l_28 = *(int *)D_00195C44;
    *(signed char *)D_001940D4 |= 1;
L39C54:;
    keys_world_actions();
    func_0012B136();
    *(int *)&l_20 = picklist_update();
    if (((int)(short)l_20) <= (-1)) goto L39CCC;
    *(int *)&l_18 = 0;
L39C76:;
    if (*(signed char *)((char *)((((int)(short)l_18) * 89) + l_28) + 47) != 0) goto L39C8E;
    (*(int *)&l_18)++;
    goto L39C76;
L39C8E:;
    if (l_20 == 0) goto L39CBB;
    (*(int *)&l_18)++;
L39C9B:;
    if (*(signed char *)((char *)((((int)(short)l_18) * 89) + l_28) + 47) != 0) goto L39CB3;
    (*(int *)&l_18)++;
    goto L39C9B;
L39CB3:;
    (*(int *)&l_20)--;
    goto L39C8E;
L39CBB:;
    return l_28 + (((int)(short)l_18) * 89);
L39CCC:;
    if (((int)(short)l_20) != (-2)) goto L39CDE;
    return 0;
L39CDE:;
    func_0012B2D3((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y);
    mc_memcpy(655360, *(int *)screen_buffer, 64000, (int)D_00170B13, 1560, 4);
    goto L39C54;
}

int spells_std_name_list(int a1)
{
    int l_30;
    short l_20;
    int l_28;
    short l_18;
    int l_2C;
    short l_1C;

    if (a1 == 0) goto L39D53;
    *(int *)&l_20 = memchr(a1, 255, 1000) - a1;
L39D53:;
    l_30 = *(int *)D_00195C44 + 20000;
    l_2C = *(int *)D_00195C44 + 21000;
    l_28 = *(int *)D_00195C44;
    disk_read_file((int)D_00170B69, *(int *)D_00195C44);
    *(int *)&l_18 = 0;
    *(int *)&l_1C = *(int *)&l_18;
L39D92:;
    if (((int)(short)l_18) < 128) goto L39DAA;
    goto L39E2C;
L39DA2:;
    (*(int *)&l_18)++;
    goto L39D92;
L39DAA:;
    if (*(signed char *)((char *)((((int)(short)l_18) * 89) + l_28) + 47) == 0) goto L39DA2;
    if (a1 == 0) goto L39DE3;
    if (memchr(a1, (int)(unsigned char)*(signed char *)((char *)((((int)(short)l_18) * 89) + l_28) + 73), (int)(short)l_20) == 0) goto L39DA2;
L39DE3:;
    mc_strncpy(l_2C, (int)&*(signed char *)((char *)((((int)(short)l_18) * 89) + l_28) + 47), 4, (int)D_00170B13, 1587);
    *(int *)((char *)(int)((char *)l_30 + (((int)(short)(*(int *)&l_1C)++) << 2))) = l_2C;
    l_2C += func_000A0DF4(l_2C) + 1;
    goto L39DA2;
L39E2C:;
    *(int *)((char *)((((int)(short)l_1C) << 2) + l_30)) = 0;
    if (l_1C != 0) goto L39E4C;
    return 0;
L39E4C:;
    return l_30;
}

void func_00039F94(void)
{
    int l_18;

    l_18 = spells_pick_list();
    if (l_18 == 0) return;
    *(signed char *)((char *)l_18 + 47) = 0;
    disk_write_arena2_file((int)D_00170B69, *(int *)D_00195C44, 11392);
}

void func_00039FD6(void)
{
    int l_18;

    l_18 = spells_pick_list();
    if (l_18 == 0) return;
    mc_memcpy(*(int *)selected_spell, l_18, 89, (int)D_00170B13, 1654, 4);
    *(signed char *)((char *)l_18 + 47) = 0;
    disk_write_arena2_file((int)D_00170B69, *(int *)D_00195C44, 11392);
}

void spells_std_append(void)
{
    int l_18;

    spell_assign_new_id();
    l_18 = *(int *)D_00195C44;
    disk_read_file((int)D_00170B69, *(int *)D_00195C44);
L3A061:;
    if (*(signed char *)((char *)l_18 + 47) == 0) goto L3A073;
    (*(char (**)[89])&l_18)++;
    goto L3A061;
L3A073:;
    mc_memcpy(l_18, *(int *)selected_spell, 89, (int)D_00170B13, 1667, 4);
    disk_write_arena2_file((int)D_00170B69, *(int *)D_00195C44, 11392);
    msgbox_show_rsc(1706, 1);
}

int spell_cost(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = 0;
    l_18 = (int)(short)*(short *)spell_effect_slot;
    *(int *)selected_spell = a1;
    l_24 = 0;
L3A0F3:;
    if (l_24 < 3) goto L3A106;
    goto L3A268;
L3A0FE:;
    l_24++;
    goto L3A0F3;
L3A106:;
    if (((int)(unsigned char)*(signed char *)((char *)((l_24 * 2) + a1))) == 255) goto L3A0FE;
    *(short *)spell_effect_slot = l_24;
    if (((int)(unsigned char)*(signed char *)((char *)((l_24 * 2) + a1) + 1)) == 255) goto L3A192;
    mc_memcpy((int)spell_effect_cost_current, ((int)spell_effect_costs) + (((int)(unsigned char)*(signed char *)(spell_effect_cost_index + ((((int)(unsigned char)*(signed char *)((char *)((l_24 * 2) + a1))) * 12) + ((int)(unsigned char)*(signed char *)((char *)((l_24 * 2) + a1) + 1))))) << 3), 8, (int)D_00170B13, 1684, 8);
    goto L3A1D4;
L3A192:;
    mc_memcpy((int)spell_effect_cost_current, ((int)spell_effect_costs) + (((int)(unsigned char)*(signed char *)(spell_effect_cost_index + (((int)(unsigned char)*(signed char *)((char *)((l_24 * 2) + a1))) * 12))) << 3), 8, (int)D_00170B13, 1686, 8);
L3A1D4:;
    if (((int)(unsigned char)*(signed char *)((char *)((l_24 * 2) + a1))) < 51) goto L3A1EE;
    l_1C++;
L3A1EE:;
    l_20 = spell_cost_formula_dispatch(((int)(unsigned char)*(signed char *)(spell_effect_cost_formula + ((int)(unsigned char)*(signed char *)((char *)((l_24 * 2) + a1))))) - 1);
    l_20 = ((110 - ((int)(short)*(short *)((char *)((((int)(unsigned char)*(signed char *)(magic_school_skills + ((int)(unsigned char)*(signed char *)(spell_effect_school + ((int)(unsigned char)*(signed char *)((char *)((l_24 * 2) + a1))))))) * 6) + a2) + 157))) * l_20) / 100;
    l_1C += l_20;
    goto L3A0FE;
L3A268:;
    *(short *)spell_effect_slot = l_18;
    l_24 = (((int)(short)*(short *)(spell_target_cost_factor + (((int)(unsigned char)*(signed char *)((char *)a1 + 7)) * 2))) * l_1C) >> 1;
    if (l_24 >= 5) goto L3A29B;
    l_24 = 5;
L3A29B:;
    return l_24;
}

void spell_add_skill_uses(int a1, int a2)
{
    int l_14;

    l_14 = 0;
L3A2C7:;
    if (l_14 < 3) goto L3A2D7;
    return;
L3A2CF:;
    l_14++;
    goto L3A2C7;
L3A2D7:;
    if (((int)(unsigned char)*(signed char *)((char *)((l_14 * 2) + a1))) == 255) goto L3A31A;
    skill_add_uses((int)(unsigned char)*(signed char *)(magic_school_skills + ((int)(unsigned char)*(signed char *)(spell_effect_school + ((int)(unsigned char)*(signed char *)((char *)((l_14 * 2) + a1)))))), a2);
L3A31A:;
    goto L3A2CF;
}
