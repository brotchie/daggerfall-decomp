/* spfx.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */

struct bf8_0_1 { unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern char D_0012B508[];
extern char D_00176D55[];
extern char D_00176D5C[];
extern char D_00176D7D[];
extern char player_environment[];
extern char selected_spell[];
extern char D_001841E3[];
extern char D_00184620[];
extern char D_00184624[];
extern char D_00184628[];
extern char D_0018462C[];
extern char D_00184630[];
extern char D_00184634[];
extern char D_00184638[];
extern char D_0018463C[];
extern char D_00184640[];
extern char D_00185083[];
extern char D_00185097[];
extern char dispel_monster_ids[];
extern char spell_resist_flags[];
extern char D_0018DDD8[];
extern char saved_positions[];
extern char D_0018DE20[];
extern char D_00190504[];
extern char text_macro_fpc[];
extern char D_00190EE4[];
extern char D_001940D4[];
extern char D_001940D6[];
extern char player_entity[];
extern char player_object[];
extern char D_00195AC4[];
extern char creature_count[];
extern char spfx_popup_handler[];
extern char D_00195B84[];
extern char player_character[];
extern char game_minutes[];
extern char D_00195C44[];
extern char free_later_count[];
extern char D_00195F62[];
extern char current_region[];
extern char D_00196271[];
extern char game_mode[];
extern char player_ailment_flags[];
extern char D_001A99F4[];
extern char D_001A99F8[];
extern char D_001A99FC[];
extern char D_001A9A00[];
extern char D_001A9A04[];
extern char D_001AA458[];

extern int faction_find(short);
extern int damage_apply(int, int, int);
extern int spells_list_poll(void);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int spfx_damage(int, int, int);
extern int name_generate(unsigned char, unsigned char);
extern int object_free_single(int);
extern int object_delete(int);
extern int object_create_child(int, int, int);
extern int inventory_open(int, int, int);
extern int rand();
extern int srand();
extern int mc_memset();
extern int mc_strncpy();
extern int func_000A0DF4();
extern int spell_find_effect_type();
extern void damage_creature_death(int);
extern void msgbox_show_rsc(int, int);
extern void spell_remove_effect_type(int, int);
extern void fatigue_add(int);
extern void weapon_reload_hand_sprites(void);
extern void picklist_open(int);
extern void msgbox_choice_rsc(short, unsigned char, unsigned char, int, unsigned char, unsigned char, unsigned char);
extern void object_free_later(int);
extern void object_free_pending(void);
extern void player_position_save(int);
extern void player_position_restore(int);
extern void diminution_stub(void);
extern void player_horse_sounds_stop(void);
extern void map_goto_location(int, int, int, int);
extern void spfx_dispel_magic_cb(int);
extern void object_foreach(int, int);
extern void inv_unequip_all_saved(void);
extern void inv_reequip_saved(void);
extern void transport_choose(int);
int spfx_drain(int, int, int);
int func_0008B43B(unsigned char, unsigned char, int);
void spfx_dispel_creatures(int, int);
void spfx_heal(int, int, int);
void spfx_show_choice_list(int, int);
void spfx_created_item_expire_cb(int);

void spfx_dispel(int a1, int a2, int a3)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_20 = a1 + 71;
    switch (*(unsigned char *)((char *)((a2 * 2) + l_20) + 1)) {
case 0:
    l_10 = 0;
    l_14 = *(int *)D_00195C44;
    l_18 = *(int *)(*(char **)player_entity + 63);
L8932A:;
    if (l_18 == 0) goto L893A0;
    if (((int)(unsigned char)*(signed char *)((char *)l_18)) != 9) goto L89395;
    l_1C = l_18 + 71;
    mc_strncpy(l_14, l_1C + 47, 4, (int)D_00176D55, 284);
    *(int *)(D_00190EE4 + (l_10 << 2)) = l_18;
    *(int *)(text_macro_fpc + (l_10++ << 2)) = l_14;
    l_14 += func_000A0DF4(l_1C + 47) + 1;
L89395:;
    l_18 = *(int *)((char *)l_18 + 55);
    goto L8932A;
L893A0:;
    *(int *)(text_macro_fpc + (l_10 << 2)) = 0;
    spfx_show_choice_list((int)text_macro_fpc, (int)spfx_dispel_magic_cb);
    *(int *)selected_spell = l_20;
    *(int *)D_001A99F4 = a2;
    return;
case 1:
    spfx_dispel_creatures((int)(unsigned char)*(signed char *)((char *)(l_20 + a2) + 86), 0);
    return;
case 2:
    spfx_dispel_creatures((int)(unsigned char)*(signed char *)((char *)(l_20 + a2) + 86), 1);
default:;
}
}

void spfx_dispel_creatures(int a1, int a2)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_24 = 0;
L89422:;
    if (l_24 < *(int *)creature_count) goto L8943A;
    return;
L89432:;
    l_24++;
    goto L89422;
L8943A:;
    l_14 = *(int *)(D_00190504 + (l_24 << 2)) + 71;
    l_20 = 0;
    l_1C = l_20;
L89459:;
    if (l_20 < 7) goto L89469;
    goto L89489;
L89461:;
    l_20++;
    goto L89459;
L89469:;
    if (*(signed char *)((char *)l_14 + 506) != *(signed char *)(dispel_monster_ids + ((a2 * 7) + l_20))) goto L89487;
    l_1C++;
L89487:;
    goto L89461;
L89489:;
    if (l_1C == 0) goto L89432;
    l_18 = a1 + ((((int)(unsigned char)*(signed char *)((char *)*(int *)player_character + 129)) - ((int)(unsigned char)*(signed char *)((char *)l_14 + 129))) * 5);
    if (l_18 >= 5) goto L894C6;
    l_18 = 5;
    goto L894D3;
L894C6:;
    if (l_18 <= 95) goto L894D3;
    l_18 = 95;
L894D3:;
    if (rand_range(1, 100) > l_18) goto L89432;
    object_delete(*(int *)(D_00190504 + (l_24 << 2)));
    goto L89432;
}

int spfx_drain(int a1, int a2, int a3)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_24 = a3 + 71;
    l_20 = a1 + 71;
    l_18 = (int)(unsigned char)*(signed char *)((char *)((a2 * 2) + l_20) + 1);
    switch ((unsigned)l_18) {
case 8:
    damage_apply(a3, (int)(unsigned short)*(short *)((char *)((a2 * 2) + l_20) + 80), 0);
    goto L89603;
case 9:
    l_14 = (int)(unsigned short)*(short *)((char *)l_24 + 155);
    l_14 -= (int)(unsigned short)*(short *)((char *)((a2 * 2) + l_20) + 80);
    if (l_14 >= 1) goto L895A4;
    l_14 = 1;
L895A4:;
    *(short *)((char *)l_24 + 155) = l_14;
    goto L89603;
default:
    l_1C = ((int)(short)*(short *)((char *)((l_18 * 2) + l_24) + 32)) - *(unsigned short *)((char *)((a2 * 2) + l_20) + 80);
    if (l_1C >= 1) goto L895EB;
    *(short *)((char *)((l_18 * 2) + l_24) + 32) = 1;
    goto L89603;
L895EB:;
    *(short *)((char *)((l_18 * 2) + l_24) + 32) -= *(short *)((char *)((a2 * 2) + l_20) + 80);
L89603:;
    if (l_24 != *(int *)player_character) goto L89618;
    hud_message_add(*(int *)D_00184620);
L89618:;
    return 1;
}
}

int spfx_elemental_resistance(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_18 = a1 + 71;
    l_14 = a3 + 71;
    *(int *)((char *)l_14 + 137) |= *(int *)(spell_resist_flags + (((int)(unsigned char)*(signed char *)((char *)((a2 * 2) + l_18) + 1)) << 2));
    *(signed char *)((char *)(((int)(unsigned char)*(signed char *)((char *)((a2 * 2) + l_18) + 1)) + l_14) + 555) = *(signed char *)((char *)(l_18 + a2) + 86);
    return 1;
}

int spfx_fortify_attribute(int a1, int a2, int a3)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_1C = a3 + 71;
    l_20 = a1 + 71;
    l_14 = (int)(unsigned char)*(signed char *)((char *)((a2 * 2) + l_20) + 1);
    l_18 = ((int)(short)*(short *)((char *)((l_14 * 2) + l_1C) + 32)) + *(unsigned short *)((char *)((a2 * 2) + l_20) + 80);
    if (l_18 <= 100) goto L89718;
    *(short *)((char *)((a2 * 2) + l_20) + 80) -= l_18 - 100;
L89718:;
    *(short *)((char *)((l_14 * 2) + l_1C) + 32) += *(short *)((char *)((a2 * 2) + l_20) + 80);
    if (l_1C != *(int *)player_character) goto L89745;
    hud_message_add(*(int *)D_00184624);
L89745:;
    return 1;
}

void spfx_heal(int a1, int a2, int a3)
{
    int l_18;
    int l_14;
    int l_10;

    l_18 = a3 + 71;
    l_14 = a1 + 71;
    l_10 = (int)(unsigned char)*(signed char *)((char *)((a2 * 2) + l_14) + 1);
    switch ((unsigned)l_10) {
    return;
case 0:
case 1:
case 2:
case 3:
case 4:
case 5:
case 6:
case 7:
    *(short *)((char *)((l_10 * 2) + l_18) + 32) += *(short *)((char *)((a2 * 2) + l_14) + 80);
    if (*(short *)((char *)((l_10 * 2) + l_18) + 32) <= *(short *)((char *)((l_10 * 2) + l_18) + 48)) goto L89809;
    *(short *)((char *)((l_10 * 2) + l_18) + 32) = *(short *)((char *)((l_10 * 2) + l_18) + 48);
L89809:;
    return;
case 8:
    *(short *)((char *)l_18 + 124) += *(short *)((char *)((a2 * 2) + l_14) + 80);
    if (*(short *)((char *)l_18 + 124) <= *(short *)((char *)l_18 + 126)) goto L8983F;
    *(short *)((char *)l_18 + 124) = *(short *)((char *)l_18 + 126);
L8983F:;
    return;
case 9:
    *(short *)((char *)l_18 + 155) += *(short *)((char *)((a2 * 2) + l_14) + 80) << 6;
    l_10 = (((int)(short)*(short *)((char *)l_18 + 32)) + ((int)(short)*(short *)((char *)l_18 + 40))) << 6;
    if (((int)(unsigned short)*(short *)((char *)l_18 + 155)) <= l_10) goto L89894;
    *(short *)((char *)l_18 + 155) = l_10;
L89894:;
    return;
case 10:
    *(short *)((char *)l_18 + 141) += *(short *)((char *)((a2 * 2) + l_14) + 80);
    if (*(short *)((char *)l_18 + 141) <= *(short *)((char *)l_18 + 143)) return;
    *(short *)((char *)l_18 + 141) = *(short *)((char *)l_18 + 143);
default:;
}
}

void spfx_transfer(int a1, int a2, int a3)
{
    int l_10;

    l_10 = a1 + 71;
    *(unsigned short *)((char *)((a2 * 2) + l_10) + 80) >>= 1;
    spfx_drain(a1, a2, a3);
    spfx_heal(a1, a2, *(int *)player_entity);
}

int spfx_soul_trap(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    if (((int)(unsigned char)*(signed char *)((char *)a3 + 577)) < 128) goto L8996C;
    hud_message_add((int)D_00176D5C);
    return 0;
L8996C:;
    hud_message_add((int)D_00176D7D);
    l_14 = a1 + 71;
    l_18 = object_create_child(a3, 0, 0);
    *(signed char *)((char *)l_18) = 19;
    *(short *)((char *)l_18 + 21) = 3;
    *(short *)((char *)l_18 + 29) = *(short *)((char *)((a2 * 2) + l_14) + 74);
    *(short *)((char *)l_18 + 27) = (unsigned short)(unsigned char)*(signed char *)((char *)(l_14 + a2) + 86);
    *(signed char *)((char *)((a2 * 2) + l_14)) = 255;
    return 1;
}

int spfx_invisibility(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 137) |= 4;
    if (l_14 != *(int *)player_character) goto L89A1C;
    hud_message_add(*(int *)D_00184628);
L89A1C:;
    return 1;
}

int spfx_levitate(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 137) |= 8;
    if (l_14 != *(int *)player_character) goto L89A80;
    transport_choose(0);
    hud_message_add(*(int *)D_0018462C);
    *(signed char *)(*(char **)player_character + 65) &= 249;
    player_horse_sounds_stop();
L89A80:;
    return 1;
}

int spfx_light(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 137) |= 16;
    return 1;
}

int spfx_lock(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_14 = a1 + 71;
    l_18 = a3 + 71;
    *(signed char *)((char *)l_18 + 137) |= 32;
    *(signed char *)(*(char **)player_character + 542) = *(signed char *)((char *)(l_14 + a2) + 86);
    return 1;
}

int spfx_open(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_14 = a1 + 71;
    l_18 = a3 + 71;
    *(signed char *)((char *)l_18 + 137) |= 64;
    *(signed char *)(*(char **)player_character + 542) = *(signed char *)((char *)(l_14 + a2) + 86);
    return 1;
}

int spfx_regenerate(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 137) |= 128;
    if (l_14 != *(int *)player_character) goto L89BB7;
    hud_message_add(*(int *)D_00184630);
L89BB7:;
    return 1;
}

int spfx_silence(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_14 = a1 + 71;
    if (rand_range(1, 100) <= ((int)(unsigned char)*(signed char *)((char *)(l_14 + a2) + 86))) goto L89C18;
    hud_message_add(*(int *)D_00185097);
    return 0;
L89C18:;
    l_18 = a3 + 71;
    *(signed char *)((char *)l_18 + 138) |= 1;
    if (l_18 != *(int *)player_character) goto L89C40;
    hud_message_add(*(int *)D_00184634);
L89C40:;
    return 1;
}

int spfx_spell_absorption(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 138) |= 2;
    return 1;
}

int spfx_spell_reflection(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 138) |= 4;
    return 1;
}

int spfx_spell_resistance(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 138) |= 8;
    return 1;
}

int spfx_chameleon(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 138) |= 16;
    if (l_14 != *(int *)player_character) goto L89D3D;
    hud_message_add(*(int *)D_00184638);
L89D3D:;
    return 1;
}

int spfx_shadow(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 138) |= 32;
    if (l_14 != *(int *)player_character) goto L89D8C;
    hud_message_add(*(int *)D_0018463C);
L89D8C:;
    return 1;
}

int spfx_slowfall(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 138) |= 64;
    if (l_14 != *(int *)player_character) goto L89DDB;
    hud_message_add(*(int *)D_00184640);
L89DDB:;
    return 1;
}

int spfx_free_action(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_18 = a3 + 71;
    *(signed char *)((char *)l_18 + 138) |= 128;
    if (((struct bf8_0_1 *)((char *)l_18 + 137))->f != 0) goto L89E2A;
    return 0;
L89E2A:;
    l_14 = a1 + 71;
    spell_remove_effect_type(a3, 0);
    *(signed char *)((char *)l_18 + 137) &= 254;
    return 1;
}

int spfx_jumping(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 139) |= 1;
    return 1;
}

int spfx_climbing(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 139) |= 2;
    return 1;
}

int func_00089ECD(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_14 = a1 + 71;
    l_18 = a3 + 71;
    if (((struct bf8_2_1 *)((char *)l_18 + 139))->f == 0) goto L89F13;
    hud_message_add(*(int *)D_00185097);
    return 0;
L89F13:;
    *(signed char *)((char *)l_18 + 139) |= 4;
    *(int *)((char *)l_18 + 104) = (int)(unsigned char)*(signed char *)((char *)((a2 * 2) + l_14) + 1);
    return 1;
}

int spfx_water_breathing(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 139) |= 8;
    return 1;
}

int spfx_water_walking(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 139) |= 16;
    return 1;
}

int func_00089FB6(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 139) |= 32;
    diminution_stub();
    return 1;
}

int spfx_pacify(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_14 = a1 + 71;
    l_18 = a3 + 71;
    if (((int)(unsigned char)*(signed char *)((char *)a3 + 577)) < 128) goto L8A03A;
    return 0;
L8A03A:;
    if (rand_range(1, 100) <= ((int)(unsigned char)*(signed char *)((char *)(l_14 + a2) + 86))) goto L8A06B;
    hud_message_add(*(int *)D_00185097);
    return 0;
L8A06B:;
    *(signed char *)((char *)l_18 + 65) |= 128;
    return 1;
}

int spfx_charm(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_14 = a1 + 71;
    l_18 = a3 + 71;
    if (((int)(unsigned char)*(signed char *)((char *)a3 + 577)) >= 128) goto L8A0C9;
    return 0;
L8A0C9:;
    if (rand_range(1, 100) <= ((int)(unsigned char)*(signed char *)((char *)(l_14 + a2) + 86))) goto L8A0FA;
    hud_message_add(*(int *)D_00185097);
    return 0;
L8A0FA:;
    *(signed char *)((char *)l_18 + 65) |= 128;
    return 1;
}

int func_0008A189(int a1, int a2, int a3)
{
    return 1;
}

int func_0008A1B0(int a1, int a2, int a3)
{
    return 1;
}

int func_0008A1D7(int a1, int a2, int a3)
{
    return 1;
}

int spfx_detect(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 139) |= 128;
    return 1;
}

int spfx_identify(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a1 + 71;
    *(signed char *)(*(char **)player_character + 542) = *(signed char *)((char *)(l_14 + a2) + 86);
    *(short *)(*(char **)player_character + 141) += *(short *)D_00195F62;
    if (*(short *)(*(char **)player_character + 141) <= *(short *)(*(char **)player_character + 143)) goto L8A2B1;
    *(short *)(*(char **)player_character + 141) = *(short *)(*(char **)player_character + 143);
L8A2B1:;
    *(int *)D_001AA458 = (int)(short)*(short *)D_00195F62;
    inventory_open(1, 4, 8);
    return 0;
}

int func_0008A2E3(int a1, int a2, int a3)
{
    return 1;
}

int func_0008A30A(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 140) |= 1;
    return 1;
}

int spell_recall_prompt(int a1, int a2, int a3)
{
    msgbox_choice_rsc(4000, 20, 21, 0, 97, 116, 0);
    *(int *)spfx_popup_handler = 1;
    return 0;
}

int spfx_comprehend_languages(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 140) |= 2;
    return 1;
}

int func_0008A3D4(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 140) |= 4;
    return 1;
}

int func_0008A40E(int a1, int a2, int a3)
{
    int l_14;

    l_14 = a3 + 71;
    *(signed char *)((char *)l_14 + 140) |= 8;
    return 1;
}

int func_0008A448(int a1, int a2, int a3)
{
    return 0;
}

int func_0008A46F(int a1, int a2, int a3)
{
    return 0;
}

int func_0008A858(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    a1 = *(int *)((char *)a1 + 63);
L8A876:;
    if (a1 == 0) goto L8A8FA;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 9) goto L8A8EC;
    l_18 = a1 + 71;
    l_14 = spell_find_effect_type(l_18, a2);
    if (l_14 == 0) goto L8A8EC;
    if (a3 == 0) goto L8A8E3;
    return ((((unsigned)rand_range(1, 100)) < (l_18 + 86)) ? 1 : 0);
L8A8E3:;
    return 1;
L8A8EC:;
    a1 = *(int *)((char *)a1 + 55);
    goto L8A876;
L8A8FA:;
    return 0;
}

void spfx_effect_tick(int a1, int a2, int a3)
{
    int l_10;

    l_10 = a1 + 71;
    switch (*(unsigned char *)((char *)((a3 * 2) + l_10))) {
case 1:
    spfx_damage(a1, a3, a2);
    return;
case 18:
    *(signed char *)((char *)((a3 * 2) + l_10) + 1) = 8;
    spfx_heal(a1, a3, a2);
    return;
case 39:
    *(signed char *)D_001940D6 |= 16;
    *(signed char *)(*(char **)player_character + 545) = *(signed char *)((char *)((a3 * 2) + l_10) + 1);
default:;
}
}

void spfx_walk_effect_records(int a1, int a2)
{
    int l_14;

L8A9B6:;
    if (a1 == 0) return;
    l_14 = *(int *)((char *)a1 + 55);
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 11) goto L8A9E9;
    if (((int)(unsigned short)(*(short *)((char *)a1 + 21) & 32768)) != 0) goto L8A9EB;
L8A9E9:;
    goto L8AA00;
L8A9EB:;
    if (((int (*)())(a2))(a1 + 71) != 0) goto L8AA00;
    object_free_single(a1);
L8AA00:;
    a1 = l_14;
    goto L8A9B6;
}

int spfx_disease_daily(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) <= 99) goto L8AA3D;
    return 1;
L8AA3D:;
    l_1C = rand_range((int)(short)*(short *)((char *)a1 + 23), (int)(short)*(short *)((char *)a1 + 25));
    if (((int)(short)*(short *)((char *)a1 + 27)) != 254) goto L8AA6D;
    return 1;
L8AA6D:;
    if (((int)(short)*(short *)((char *)a1 + 27)) == 255) goto L8AAA1;
    (*(short *)((char *)a1 + 27))--;
    if (*(short *)((char *)a1 + 27) != 0) goto L8AAA1;
    *(short *)((char *)a1 + 27) = 254;
    return 1;
L8AAA1:;
    *(signed char *)player_ailment_flags |= 2;
    *(short *)((char *)a1 + 29) = 1;
    l_24 = 0;
L8AAB8:;
    if (l_24 < 11) goto L8AACB;
    goto L8ABF0;
L8AAC3:;
    l_24++;
    goto L8AAB8;
L8AACB:;
    if (*(short *)((char *)((l_24 * 2) + a1) + 1) == 0) goto L8AAC3;
    switch ((unsigned)l_24) {
case 8:
    *(short *)(*(char **)player_character + 124) -= l_1C;
    if (*(short *)(*(char **)player_character + 124) > 0) goto L8AB18;
    damage_creature_death(*(int *)player_entity);
L8AB18:;
    goto L8ABEB;
case 9:
    fatigue_add(-l_1C);
    goto L8ABEB;
case 10:
    *(short *)(*(char **)player_character + 141) -= l_1C;
    if (*(short *)(*(char **)player_character + 141) >= 0) goto L8AB5A;
    *(short *)(*(char **)player_character + 141) = 0;
L8AB5A:;
    goto L8ABEB;
default:
    *(short *)(*(char **)player_character + 32 + (l_24 * 2)) -= l_1C;
    if (*(short *)(*(char **)player_character + 32 + (l_24 * 2)) > 0) goto L8AB90;
    damage_creature_death(*(int *)player_entity);
L8AB90:;
    *(short *)((char *)((l_24 * 2) + a1) + 31) += l_1C;
    if (((int)(short)*(short *)(*(char **)player_character + 32 + (l_24 * 2))) >= 1) goto L8ABEB;
    *(short *)(*(char **)player_character + 32 + (l_24 * 2)) = 1;
    *(short *)((char *)((l_24 * 2) + a1) + 31) -= 1 - *(short *)(*(char **)player_character + 32 + (l_24 * 2));
L8ABEB:;
    goto L8AAC3;
L8ABF0:;
    hud_message_add(*(int *)D_00185083);
    return 1;
}
}

int func_0008AC0E(int a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = a1 + 71;
    l_18 = a2 + 71;
    l_14 = ((int)(unsigned char)*(signed char *)((char *)(l_1C + a3) + 86)) + ((((int)(unsigned char)*(signed char *)(*(char **)((char *)a1 + 47) + 200)) - ((int)(unsigned char)*(signed char *)((char *)l_18 + 129))) * 5);
    return ((rand_range(0, 100) < l_14) ? 1 : 0);
}

int func_0008AC9B(int a1)
{
    int l_20;
    int l_1C;

    if (((int)(short)*(short *)((char *)a1 + 27)) == 254) goto L8ACC6;
    return 1;
L8ACC6:;
    l_20 = 0;
    l_1C = l_20;
L8ACD3:;
    if (l_20 < 8) goto L8ACE6;
    goto L8ADB0;
L8ACDE:;
    l_20++;
    goto L8ACD3;
L8ACE6:;
    if (*(short *)((char *)((l_20 * 2) + a1) + 1) == 0) goto L8ADAB;
    if (*(short *)((char *)((l_20 * 2) + a1) + 1) >= 0) goto L8AD2C;
    if (*(short *)(*(char **)player_character + 32 + (l_20 * 2)) > *(short *)(*(char **)player_character + 48 + (l_20 * 2))) goto L8AD2E;
L8AD2C:;
    goto L8AD53;
L8AD2E:;
    l_1C = 1;
    (*(short *)(*(char **)player_character + 32 + (l_20 * 2)))--;
    (*(short *)((char *)((l_20 * 2) + a1) + 31))++;
    goto L8ADAB;
L8AD53:;
    if (*(short *)((char *)((l_20 * 2) + a1) + 31) == 0) goto L8AD86;
    if (*(short *)(*(char **)player_character + 32 + (l_20 * 2)) < *(short *)(*(char **)player_character + 48 + (l_20 * 2))) goto L8AD88;
L8AD86:;
    goto L8ADAB;
L8AD88:;
    l_1C = 1;
    (*(short *)(*(char **)player_character + 32 + (l_20 * 2)))++;
    (*(short *)((char *)((l_20 * 2) + a1) + 31))--;
L8ADAB:;
    goto L8ACDE;
L8ADB0:;
    return l_1C;
}

int spfx_resist_roll(int a1, int a2, int a3, int a4, int a5, int a6)
{
    int l_14;
    int l_10;

    if ((*(int *)((char *)a3 + 137) & *(int *)(spell_resist_flags + (a1 << 2))) == 0) goto L8AE12;
    if (rand_range(1, 100) <= ((int)(unsigned char)*(signed char *)((char *)(a3 + a1) + 555))) goto L8AE14;
L8AE12:;
    goto L8AE20;
L8AE14:;
    return 0;
L8AE20:;
    l_14 = 50;
    if (a4 == 0) goto L8AE8F;
    if ((((int)(unsigned char)*(signed char *)((char *)a4 + 1)) & a2) == 0) goto L8AE49;
    return 0;
L8AE49:;
    if ((((int)(unsigned char)*(signed char *)((char *)a4 + 3)) & a2) == 0) goto L8AE65;
    return 100;
L8AE65:;
    if ((((int)(unsigned char)*(signed char *)((char *)a4 + 2)) & a2) == 0) goto L8AE78;
    l_14 >>= 1;
L8AE78:;
    if ((((int)(unsigned char)*(signed char *)((char *)a4)) & a2) == 0) goto L8AE8F;
    l_14 += l_14 >> 1;
L8AE8F:;
    l_14 += a6;
    l_14 += *(int *)D_0018DDD8;
    if (a1 != 1) goto L8AEB3;
    if (((int)(unsigned char)*(signed char *)((char *)a3 + 67)) == 2) goto L8AEB5;
L8AEB3:;
    goto L8AEB9;
L8AEB5:;
    l_14 += 30;
L8AEB9:;
    if (a1 != 4) goto L8AEC8;
    if (*(signed char *)((char *)a3 + 67) == 0) goto L8AECA;
L8AEC8:;
    goto L8AECE;
L8AECA:;
    l_14 += 30;
L8AECE:;
    if (l_14 >= 5) goto L8AEDD;
    l_14 = 5;
    goto L8AEEA;
L8AEDD:;
    if (l_14 <= 95) goto L8AEEA;
    l_14 = 95;
L8AEEA:;
    l_10 = rand_range(1, 100);
    if (l_10 <= l_14) goto L8AF0D;
    return 100;
L8AF0D:;
    if ((l_14 - 20) <= l_10) goto L8AF21;
    return 0;
L8AF21:;
    l_10 -= l_14;
    return (-l_10) * 5;
}

void spfx_cure_disease(int a1, int a2)
{
    int l_18;
    int l_14;

    if (a2 != *(int *)player_character) goto L8AF61;
    inv_unequip_all_saved();
L8AF61:;
    a1 = *(int *)((char *)a1 + 63);
L8AF6A:;
    if (a1 == 0) goto L8B021;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 11) goto L8B013;
    l_18 = a1 + 71;
    if (((int)(unsigned char)*(signed char *)((char *)l_18)) >= 100) goto L8B013;
    l_14 = 0;
L8AFAA:;
    if (l_14 < 8) goto L8AFBA;
    goto L8B006;
L8AFB2:;
    l_14++;
    goto L8AFAA;
L8AFBA:;
    *(short *)((char *)((l_14 * 2) + a2) + 32) += *(short *)((char *)((l_14 * 2) + l_18) + 31);
    if (*(short *)((char *)((l_14 * 2) + a2) + 32) <= *(short *)((char *)((l_14 * 2) + a2) + 48)) goto L8B004;
    *(short *)((char *)((l_14 * 2) + a2) + 32) = *(short *)((char *)((l_14 * 2) + a2) + 48);
L8B004:;
    goto L8AFB2;
L8B006:;
    a1 = object_free_single(a1);
    goto L8B01C;
L8B013:;
    a1 = *(int *)((char *)a1 + 55);
L8B01C:;
    goto L8AF6A;
L8B021:;
    *(int *)(*(char **)player_character + 499) = 0;
    *(short *)(*(char **)player_character + 108) = 0;
    if (a2 != *(int *)player_character) return;
    inv_reequip_saved();
}

void spfx_show_choice_list(int a1, int a2)
{
    picklist_open(a1);
    *(int *)spfx_popup_handler = a2;
}

void spfx_popup_update(void)
{
    int l_18;

    if (*(int *)spfx_popup_handler == 0) return;
    *(signed char *)D_0012B508 = 146;
    if (*(int *)spfx_popup_handler != 1) goto L8B0B7;
    if (((int)(unsigned char)*(signed char *)game_mode) != 8) goto L8B0BC;
L8B0B7:;
    goto L8B196;
L8B0BC:;
    if (((int)(unsigned char)*(signed char *)D_00196271) != 1) goto L8B12D;
    *(int *)D_001A9A04 = (int)(unsigned char)*(signed char *)player_environment;
    *(int *)D_001A99F8 = (int)(unsigned short)*(short *)(*(char **)D_00195AC4 + 27);
    *(int *)D_001A99FC = (int)(unsigned char)*(signed char *)current_region;
    if (((int)(unsigned char)*(signed char *)player_environment) != 2) goto L8B117;
    *(int *)D_001A9A00 = (int)(unsigned short)*(short *)(*(char **)(*(char **)player_object + 67) + 27);
    goto L8B121;
L8B117:;
    *(int *)D_001A9A00 = 0;
L8B121:;
    player_position_save(1);
    goto L8B18A;
L8B12D:;
    if (*(int *)D_0018DE20 != 0) goto L8B147;
    msgbox_show_rsc(4001, 1);
    goto L8B18A;
L8B147:;
    map_goto_location(*(int *)D_001A99FC, *(int *)D_001A9A04, *(int *)D_001A99F8, *(int *)D_001A9A00);
    player_position_restore(1);
    mc_memset((int)saved_positions, 0, 48, (int)D_00176D55, 1342, 48);
L8B18A:;
    *(int *)spfx_popup_handler = 0;
    return;
L8B196:;
    if (((struct bf8_2_1 *)&D_001940D4)->f == 0) goto L8B1AD;
    l_18 = spells_list_poll();
    if (l_18 > (-1)) goto L8B1AF;
L8B1AD:;
    return;
L8B1AF:;
    ((int (*)())(*(int *)spfx_popup_handler))(l_18);
    *(int *)spfx_popup_handler = 0;
}

int spell_extend_duration(int a1, int a2, int a3)
{
    int l_1C;
    int l_18;
    int l_14;

    l_1C = *(int *)((char *)a1 + 63);
L8B1EA:;
    if (l_1C == 0) goto L8B288;
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) != 9) goto L8B27A;
    l_18 = l_1C + 71;
    l_14 = 0;
L8B217:;
    if (l_14 < 3) goto L8B227;
    goto L8B27A;
L8B21F:;
    l_14++;
    goto L8B217;
L8B227:;
    if (*(signed char *)((char *)((l_14 * 2) + l_18)) != *(signed char *)((char *)((a3 * 2) + a2))) goto L8B255;
    if (*(signed char *)((char *)((l_14 * 2) + l_18) + 1) == *(signed char *)((char *)((a3 * 2) + a2) + 1)) goto L8B257;
L8B255:;
    goto L8B278;
L8B257:;
    *(short *)((char *)((l_14 * 2) + l_18) + 74) += *(short *)((char *)((a3 * 2) + a2) + 74);
    return 1;
L8B278:;
    goto L8B21F;
L8B27A:;
    l_1C = *(int *)((char *)l_1C + 55);
    goto L8B1EA;
L8B288:;
    return 0;
}

int spell_active_chance(int a1, unsigned char a2, unsigned char a3)
{
    int l_1C;
    int l_24;
    int l_20;

    l_1C = *(int *)((char *)a1 + 63);
L8B2B8:;
    if (l_1C == 0) goto L8B32F;
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) != 9) goto L8B324;
    l_24 = l_1C + 71;
    l_20 = 0;
L8B2E1:;
    if (l_20 < 3) goto L8B2F1;
    goto L8B324;
L8B2E9:;
    l_20++;
    goto L8B2E1;
L8B2F1:;
    if (*(unsigned char *)((char *)((l_20 * 2) + l_24)) != a2) goto L8B310;
    if (*(unsigned char *)((char *)((l_20 * 2) + l_24) + 1) == a3) goto L8B312;
L8B310:;
    goto L8B322;
L8B312:;
    return (int)(unsigned char)*(signed char *)((char *)(l_24 + l_20) + 86);
L8B322:;
    goto L8B2E9;
L8B324:;
    l_1C = *(int *)((char *)l_1C + 55);
    goto L8B2B8;
L8B32F:;
    return 0;
}

void spfx_created_item_expire_cb(int a1)
{
    int l_18;

    if (((int)(unsigned short)(*(short *)((char *)a1 + 21) & 4096)) == 0) goto L8B376;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) == 2) goto L8B378;
L8B376:;
    goto L8B386;
L8B378:;
    if (((unsigned)*(int *)((char *)a1 + 43)) < *(int *)game_minutes) goto L8B388;
L8B386:;
    return;
L8B388:;
    l_18 = 0;
L8B38F:;
    if (l_18 < 27) goto L8B39F;
    goto L8B3D8;
L8B397:;
    l_18++;
    goto L8B38F;
L8B39F:;
    if (*(int *)(*(char **)player_character + 367 + (l_18 << 2)) != a1) goto L8B3D6;
    *(int *)(*(char **)player_character + 367 + (l_18 << 2)) = 0;
    (*(int *)D_00195B84)++;
L8B3D6:;
    goto L8B397;
L8B3D8:;
    object_free_later(a1);
}

void spfx_expire_created_items(void)
{
    *(int *)D_00195B84 = 0;
    *(int *)free_later_count = 0;
    object_foreach(*(int *)(*(char **)player_object + 63), (int)spfx_created_item_expire_cb);
    object_free_pending();
    if (*(int *)D_00195B84 == 0) return;
    weapon_reload_hand_sprites();
}

int func_0008B43B(unsigned char a1, unsigned char a2, int a3)
{
    int l_24;
    int l_20;

    l_24 = rand();
    srand(a3);
    l_20 = name_generate((int)(unsigned char)a1, (int)(unsigned char)a2);
    srand(l_24);
    return l_20;
}

int func_0008B48B(int a1)
{
    int l_20;
    int l_1C;

    if (((int)(unsigned char)*(signed char *)((char *)a1)) == 8) goto L8B4BA;
    if (((int)(unsigned char)*(signed char *)((char *)a1)) != 53) goto L8B4BC;
L8B4BA:;
    goto L8B4C8;
L8B4BC:;
    return 0;
L8B4C8:;
    l_20 = a1 + 71;
    if (*(short *)((char *)l_20) == 0) goto L8B502;
    l_1C = faction_find((int)(short)*(short *)((char *)l_20));
    if (((int)(unsigned char)*(signed char *)((char *)l_1C)) != 4) goto L8B502;
    return l_1C + 3;
L8B502:;
    if (*(int *)((char *)a1 + 51) == 0) goto L8B539;
    return func_0008B43B((int)(unsigned char)*(signed char *)(D_001841E3 + ((int)(unsigned char)*(signed char *)current_region)), (int)(unsigned char)(*(signed char *)((char *)a1 + 21) & 4), *(int *)((char *)a1 + 43));
L8B539:;
    return func_0008B43B((int)(unsigned char)*(signed char *)(D_001841E3 + ((int)(unsigned char)*(signed char *)current_region)), (int)(unsigned char)(*(signed char *)((char *)l_20 + 2) & 16), *(int *)((char *)a1 + 31));
}
