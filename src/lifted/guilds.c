/* guilds.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char key_down_esc[];
extern char screen_buffer[];
extern char D_00175EAA[];
extern char D_0017606F[];
extern char D_0017607C[];
extern char D_001760A4[];
extern char trade_price_scale[];
extern struct spell *selected_spell;
extern char spell_effect_names[];
extern char spell_effect_subtype_names[];
extern char guild_rank_primary_skill[];
extern char guild_rank_secondary_skill[];
extern char guild_skill_lists[];
extern char guild_service_factions[];
extern char guild_service_labels[];
extern char guild_menu_buttons[];
extern char D_0018750B[];
extern char D_0018750D[];
extern char D_0018750F[];
extern char D_00187CA8[];
extern char text_buffer[];
extern char D_00190CBC[];
extern char D_00190D1A[];
extern char D_00190D20[];
extern char D_00190DD0[];
extern char D_00190DDC[];
extern char D_001940D4[];
extern char D_001940D8[];
extern char D_001940D9[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern char vertical_velocity[];
extern struct record *D_00195AC4;
extern char cheat_flags[];
extern struct record *D_00195AF4;
extern char creature_count[];
extern char spellshop_icons[];
extern struct character *player_character;
extern char window_image[];
extern char game_minutes[];
extern char D_001960D9[];
extern char current_region[];
extern char D_00196271[];
extern char D_00196272[];
extern char game_mode[];
extern char player_on_ground[];
extern char mouse_buttons_prev[];
extern char in_dungeon_water[];
extern char D_00196299[];
extern char D_001962A0[];
extern char D_001962AF[];
extern struct faction *D_0019671C;
extern char guild_saved_screen[];
extern struct membership *guild_membership;
extern char D_001A4A18[];
extern char D_001A4A1A[];
extern char D_001A4A1C[];
extern char rest_image[];
extern char rest_loitering[];
extern char D_001A9AB8[];

extern struct faction *faction_find(short);
extern int spell_cost(struct spell *, struct character *);
extern int holiday_today(int, int);
extern int sound_play(int, int, int);
extern int disk_read_file(int, int);
extern int spellshop_build_list(void);
extern int guild_confirm_price(int);
extern int rand_range(int, int);
extern int gold_can_afford(int);
extern int object_free_single(struct record *);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern struct record *object_find_item(struct record *, int, int);
extern int object_new_id(int);
extern int rand();
extern int srand();
extern int mc_free();
extern int mc_malloc();
extern int func_000A0DD9();
extern int mc_memcpy();
extern int func_000CD20E();
extern int func_000CDD81();
extern int func_000CE31C();
extern int func_0012B136();
extern int func_00144F68();
extern void msgbox_show_rsc(int, int);
extern void keys_world_actions(void);
extern void item_make(int, int, struct item *);
extern void func_0005F1BD(int, int, int, int);
extern void func_00061398(int);
extern void disease_remove_skill_bonuses(void);
extern void disease_restore_skill_bonuses(void);
extern void spellshop_update(void);
extern void func_0006FD03(int);
extern void blessing_remove(int);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern void msgbox_yes_no_rsc(int);
extern void gold_spend(int);
extern void player_movement_update(void);
extern void picklist_free(int);
extern void object_free_children(int);
extern void object_foreach_post(struct record *, int);
extern void object_foreach(struct record *, int);
extern void inventory_open_container(int, int, int);
int spellshop_close(void);
int guild_best_skill(int, int, int);
struct membership *guild_find_membership_by_kind(unsigned char);
struct membership *guild_find_membership_by_bits(unsigned char);
void spellshop_open(void);
void func_00070191(struct record *);
void func_000701F1(struct record *);
void func_00071475(struct record *);

int func_0006F484(int a1)
{
    struct membership *l_1C;

    if (a1 != 4) goto L6F4F0;
    l_1C = guild_find_membership_by_kind(0);
    if (l_1C != 0) goto L6F4B7;
    return 0;
L6F4B7:;
    return ((rand_range(1, 100) <= ((l_1C->rank + 1) * 5)) ? 1 : 0);
L6F4F0:;
    if (a1 >= 3) goto L6F54B;
    l_1C = guild_find_membership_by_kind(3);
    if (l_1C != 0) goto L6F512;
    return 0;
L6F512:;
    return ((rand_range(1, 100) <= ((l_1C->rank + 1) * 5)) ? 1 : 0);
L6F54B:;
    return 0;
}

void guild_buy_potions(void)
{
    object_free_children((int)D_001960D9);
    func_00061398((int)D_001960D9);
    inventory_open_container((int)D_001960D9, 1, 4);
}

void guild_buy_spells(void)
{
    *(int *)guild_saved_screen = mc_malloc(64000, (int)D_00175EAA, 871);
    mc_memcpy(*(int *)guild_saved_screen, *(int *)screen_buffer, 64000, (int)D_00175EAA, 873, 4);
    spellshop_open();
L6F5EC:;
    if (((int)(unsigned char)*(signed char *)game_mode) != 5) goto L6F634;
    mc_memcpy(*(int *)screen_buffer, *(int *)guild_saved_screen, 64000, (int)D_00175EAA, 879, 4);
    keys_world_actions();
    spellshop_update();
    player_movement_update();
    func_000CDD81(1);
    goto L6F5EC;
L6F634:;
    if (*(int *)guild_saved_screen == 0) goto L6F649;
    if (*(int *)guild_saved_screen != (-1751672937)) goto L6F64B;
L6F649:;
    return;
L6F64B:;
    mc_free(*(int *)guild_saved_screen, (int)D_00175EAA, 886);
    *(int *)guild_saved_screen = -1751672937;
}

void guild_buy_magic_items(void)
{
    int l_18;

    l_18 = rand();
    object_free_children((int)D_001960D9);
    srand(current_building->id);
    if (guild_membership == 0) goto L6F6BA;
    if (guild_membership->rank >= 4) goto L6F6CF;
L6F6BA:;
    func_0005F1BD((int)D_001960D9, 0, 1, 0);
    goto L6F6E5;
L6F6CF:;
    func_0005F1BD((int)D_001960D9, 0, 1, 1);
L6F6E5:;
    srand(l_18);
    *(signed char *)D_001940D9 |= 2;
    if (holiday_today(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region) != 38) goto L6F715;
    *(int *)trade_price_scale = 128;
L6F715:;
    inventory_open_container((int)D_001960D9, 1, 4);
    *(signed char *)D_001962AF = 1;
}

void spellshop_open(void)
{
    *(short *)D_001A4A18 = rand();
    srand((int)(short)*(short *)D_001A4A1A);
    *(signed char *)game_mode = 5;
    *(signed char *)D_001940D8 &= 254;
    *(signed char *)D_001940D8 |= 2;
    *(int *)window_image = disk_read_file((int)D_0017606F, 0);
    *(int *)spellshop_icons = disk_read_file((int)D_0017607C, 0);
    *(signed char *)D_00196272 = 1;
    if (spellshop_build_list() != 0) return;
    spellshop_close();
}

int spellshop_close(void)
{
    if (((struct bf8_2_1 *)&D_001940D4)->f == 0) goto L6F950;
    picklist_free((int)D_001A9AB8);
L6F950:;
    srand((int)(short)*(short *)D_001A4A18);
    *(signed char *)D_001940D8 &= 253;
    *(signed char *)game_mode = 0;
    if (*(int *)window_image == 0) goto L6F97F;
    if (*(int *)window_image != (-1751672937)) goto L6F981;
L6F97F:;
    goto L6F99F;
L6F981:;
    mc_free(*(int *)window_image, (int)D_00175EAA, 963);
    *(int *)window_image = -1751672937;
L6F99F:;
    if (*(int *)spellshop_icons == 0) goto L6F9B4;
    if (*(int *)spellshop_icons != (-1751672937)) goto L6F9B6;
L6F9B4:;
    goto L6F9D4;
L6F9B6:;
    mc_free(*(int *)spellshop_icons, (int)D_00175EAA, 964);
    *(int *)spellshop_icons = -1751672937;
L6F9D4:;
    *(signed char *)D_00196272 = 0;
    return 1;
}

void spellshop_buy(void)
{
    int l_1C;
    struct record *l_18;

    l_1C = spell_cost(selected_spell, player_character) << 2;
    if (holiday_today(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region) != 43) goto L6FA2D;
    l_1C >>= 1;
L6FA2D:;
    l_1C = guild_confirm_price(l_1C);
    if (l_1C < 0) return;
    if (gold_can_afford(l_1C) != 0) goto L6FA5F;
    msgbox_show_rsc(454, 1);
    return;
L6FA5F:;
    gold_spend(l_1C);
    l_18 = object_find_item(player_entity->children, 27, 0);
    l_18 = object_create_child(l_18, 0, 89);
    l_18->type = 9;
    l_18->id = object_new_id(100);
    mc_memcpy(&l_18->data.spell, (int)selected_spell, 89, (int)D_00175EAA, 992, 4);
}

void spellshop_draw_spell(struct spell *a1)
{
    int l_1C;
    short l_18;

    *(signed char *)D_0012B508 = 145;
    func_000CD20E(172, 32, a1->icon);
    func_000CE31C((int)(*(char **)spellshop_icons + (a1->element * 640)) + 24, (int)(*(char **)screen_buffer + 10486), 16, 16, 40);
    func_000CE31C((int)(*(char **)spellshop_icons + (a1->target * 640)), (int)&*(signed char *)(*(char **)screen_buffer + 10445), 24, 16, 40);
    text_draw_colored((int)a1->name, 148, 20, 145, 141);
    *(int *)&l_18 = 0;
L6FB97:;
    if (((int)(short)l_18) < 3) goto L6FBAD;
    goto L6FC9B;
L6FBA5:;
    (*(int *)&l_18)++;
    goto L6FB97;
L6FBAD:;
    if (a1->effects[(int)(short)l_18].type == 255) goto L6FBA5;
    text_draw_centered_colored(*(int *)(spell_effect_names + (a1->effects[(int)(short)l_18].type << 2)), 219, (int)(short)((*(int *)&l_18 * 38) + 63), 145, 141);
    if (a1->effects[(int)(short)l_18].subtype == 255) goto L6FC48;
    if (*(int *)(spell_effect_subtype_names + (a1->effects[(int)(short)l_18].type * 48) + (a1->effects[(int)(short)l_18].subtype << 2)) != 0) goto L6FC4A;
L6FC48:;
    goto L6FC96;
L6FC4A:;
    text_draw_centered_colored(*(int *)(spell_effect_subtype_names + (a1->effects[(int)(short)l_18].type * 48) + (a1->effects[(int)(short)l_18].subtype << 2)), 219, (int)(short)((*(int *)&l_18 * 38) + 75), 145, 141);
L6FC96:;
    goto L6FBA5;
L6FC9B:;
    l_1C = spell_cost(a1, player_character) << 2;
    if (holiday_today(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region) != 43) goto L6FCC9;
    l_1C >>= 1;
L6FCC9:;
    text_draw_colored(func_000A0DD9(l_1C, (int)text_buffer, 10), 97, 172, 145, 156);
}

void spellshop_effect_button_1(void)
{
    func_0006FD03(0);
}

void spellshop_effect_button_2(void)
{
    func_0006FD03(1);
}

void spellshop_effect_button_3(void)
{
    func_0006FD03(2);
}

int guild_rank_for_skills(int a1)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_2C = D_0019671C->reputation / 10;
    l_28 = guild_best_skill((int)&l_20, *(int *)(guild_skill_lists + (a1 << 2)), -1);
    l_24 = guild_best_skill((int)&l_20, *(int *)(guild_skill_lists + (a1 << 2)), l_20);
    l_20 = 0;
L70073:;
    if (((int)(unsigned char)*(signed char *)(guild_rank_primary_skill + l_20++)) < l_28) goto L70073;
    l_20--;
    l_1C = 0;
L70096:;
    if (((int)(unsigned char)*(signed char *)(guild_rank_secondary_skill + l_1C++)) < l_24) goto L70096;
    l_1C--;
    if (l_20 >= l_2C) goto L700C0;
    l_2C = l_20;
L700C0:;
    if (l_1C >= l_2C) goto L700CE;
    l_2C = l_1C;
L700CE:;
    if (l_2C <= 9) goto L700DB;
    l_2C = 9;
L700DB:;
    return l_2C;
}

int guild_best_skill(int a1, int a2, int a3)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_20 = 0;
    l_1C = -1;
L70111:;
    l_14 = player_character->skills[(int)(unsigned char)*(signed char *)((char *)(a2 + l_20))].value;
    if (l_14 <= l_1C) goto L7014C;
    if (((int)(unsigned char)*(signed char *)((char *)(a2 + l_20))) != a3) goto L7014E;
L7014C:;
    goto L70161;
L7014E:;
    l_1C = l_14;
    l_18 = (int)(unsigned char)*(signed char *)((char *)(a2 + l_20));
L70161:;
    ++l_20;
    if (((int)(unsigned char)*(signed char *)((char *)(a2 + l_20))) != 255) goto L70111;
    *(int *)((char *)a1) = l_18;
    return l_1C;
}

void func_00070191(struct record *a1)
{
    if (a1->type != 10) return;
    if (*(signed char *)D_00190D20 != a1->data.membership.kind) goto L701CF;
    D_00195AF4 = a1;
L701CF:;
    if (*(short *)D_00190DDC != a1->data.membership.faction) return;
    D_00195AF4 = a1;
}

void func_000701F1(struct record *a1)
{
    if (a1->type != 10) return;
    if ((((int)(signed char)*(signed char *)D_00190D20) & a1->data.membership.kind) == 0) return;
    D_00195AF4 = a1;
}

struct membership *guild_find_membership_by_kind(unsigned char a1)
{
    D_00195AF4 = 0;
    *(short *)D_00190DDC = 0;
    *(signed char *)D_00190D20 = a1;
    object_foreach(player_entity->children, (int)func_00070191);
    if (D_00195AF4 != 0) goto L702F0;
    return 0;
L702F0:;
    return &D_00195AF4->data.membership;
}

struct membership *guild_find_membership_by_bits(unsigned char a1)
{
    D_00195AF4 = 0;
    *(short *)D_00190DDC = 0;
    *(signed char *)D_00190D20 = a1;
    object_foreach(player_entity->children, (int)func_000701F1);
    if (D_00195AF4 != 0) goto L70358;
    return 0;
L70358:;
    return &D_00195AF4->data.membership;
}

void guild_expire_blessings(void)
{
    struct record *l_1C;
    int l_18;

    l_1C = player_entity->children;
L70AA7:;
    if (l_1C == 0) return;
    if (l_1C->type != 30) goto L70AE5;
    l_18 = (int)RECORD_DATA(l_1C);
    if (((unsigned)*(int *)((char *)l_18 + 2)) >= *(int *)game_minutes) goto L70AE5;
    blessing_remove(l_18);
    object_free_single(l_1C);
    return;
L70AE5:;
    l_1C = l_1C->next;
    goto L70AA7;
}

int guild_service_label(short a1)
{
    int l_20;

    l_20 = 0;
L710C6:;
    if (l_20 < 92) goto L710D6;
    goto L710FB;
L710CE:;
    l_20++;
    goto L710C6;
L710D6:;
    if (*(short *)(guild_service_factions + (l_20 * 2)) != a1) goto L710F9;
    return *(int *)(guild_service_labels + (l_20 << 2));
L710F9:;
    goto L710CE;
L710FB:;
    return 0;
}

int guild_menu(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_18 = -1;
    *(int *)guild_saved_screen = mc_malloc(64000, (int)D_00175EAA, 1581);
    mc_memcpy(*(int *)guild_saved_screen, *(int *)screen_buffer, 64000, (int)D_00175EAA, 1582, 4);
    *(signed char *)D_00196272 = 1;
L7116C:;
    if (l_18 != (-1)) goto L712D4;
    mc_memcpy(*(int *)screen_buffer, *(int *)guild_saved_screen, 64000, (int)D_00175EAA, 1587, 4);
    func_00144F68((int)(unsigned short)*(short *)((char *)a1), (int)(unsigned short)*(short *)((char *)a1 + 2), (int)(unsigned short)*(short *)((char *)a1 + 4), (int)(unsigned short)*(short *)((char *)a1 + 6), a1 + 12);
    text_draw_centered_colored(a3, 159, 71, 145, 141);
    keys_world_actions();
    player_movement_update();
    func_000CDD81(1);
    if (*(signed char *)key_down_esc == 0) goto L71218;
L71203:;
    if (*(signed char *)key_down_esc != 0) goto L71203;
    l_18 = 3;
    goto L712D4;
L71218:;
    if (*(signed char *)mouse_buttons == 0) goto L7122A;
    if (*(signed char *)mouse_buttons_prev == 0) goto L7122F;
L7122A:;
    goto L712CF;
L7122F:;
    l_14 = 0;
L71236:;
    if (l_14 < 4) goto L71249;
    goto L712CF;
L71241:;
    l_14++;
    goto L71236;
L71249:;
    if (*(short *)mouse_x <= *(short *)(guild_menu_buttons + (l_14 * 12))) goto L71271;
    if (*(short *)mouse_x < *(short *)(D_0018750D + (l_14 * 12))) goto L71273;
L71271:;
    goto L71287;
L71273:;
    if (*(short *)mouse_y > *(short *)(D_0018750B + (l_14 * 12))) goto L71289;
L71287:;
    goto L7129D;
L71289:;
    if (*(short *)mouse_y < *(short *)(D_0018750F + (l_14 * 12))) goto L7129F;
L7129D:;
    goto L712CA;
L7129F:;
    if (l_14 != 0) goto L712AB;
    if (a2 != 0) goto L712AD;
L712AB:;
    goto L712AF;
L712AD:;
    goto L71241;
L712AF:;
    sound_play(203, (int)player_object, 100);
    l_18 = l_14;
L712CA:;
    goto L71241;
L712CF:;
    goto L7116C;
L712D4:;
    if (*(signed char *)mouse_buttons == 0) goto L712E4;
    func_0012B136();
    goto L712D4;
L712E4:;
    *(signed char *)D_00196272 = 0;
    if (*(int *)guild_saved_screen == 0) goto L71300;
    if (*(int *)guild_saved_screen != (-1751672937)) goto L71302;
L71300:;
    goto L71320;
L71302:;
    mc_free(*(int *)guild_saved_screen, (int)D_00175EAA, 1617);
    *(int *)guild_saved_screen = -1751672937;
L71320:;
    return l_18;
}

int guild_is_local_knight(void)
{
    int l_24;
    struct membership *l_20;
    struct faction *l_1C;

    l_20 = guild_find_membership_by_bits(64);
    if (l_20 != 0) goto L7135B;
    return 0;
L7135B:;
    l_1C = faction_find(l_20->faction);
    if ((signed char)l_1C->region != *(signed char *)current_region) goto L71381;
    return 1;
L71381:;
    if (l_20->rank < 4) goto L71399;
    l_24 = 1;
    goto L713A0;
L71399:;
    l_24 = 0;
L713A0:;
    return l_24;
}

int guild_local_temple_rank(void)
{
    struct membership *l_20;
    struct faction *l_1C;

    l_20 = guild_find_membership_by_bits(128);
    if (l_20 != 0) goto L713DD;
    return 0;
L713DD:;
    l_1C = faction_find(l_20->faction);
    if ((signed char)l_1C->region != *(signed char *)current_region) goto L71406;
    return l_20->rank;
L71406:;
    return 0;
}

void func_0007141A(int a1)
{
    struct record *l_18;

    *(signed char *)D_001A4A1C = 1;
    l_18 = object_create_child(D_00195AC4, 0, 107);
    l_18->type = 2;
    item_make(11, 0, &l_18->data.item);
    object_reparent((struct record *)D_001960D9, l_18);
}

void func_00071475(struct record *a1)
{
    struct disease *l_18;

    switch (a1->type) {
case 11:
    if (((int)(unsigned short)(a1->flags & 32768)) == 0) goto L714D7;
    l_18 = &a1->data.disease;
    if (l_18->id > 99) return;
L714D7:;
    object_delete(a1);
    return;
case 9:
    switch (a1->parent->type) {
case 1:
case 3:
case 38:
    object_delete(a1);
default:;
}
default:;
}
}

void guild_heal(void)
{
    int l_1C;
    int l_18;

    l_18 = 0;
    player_character->health = player_character->max_health;
    msgbox_show_rsc(350, 1);
    l_1C = 0;
L71556:;
    if (l_1C < 8) goto L71566;
    goto L71592;
L7155E:;
    l_1C++;
    goto L71556;
L71566:;
    if (player_character->attributes[l_1C] >= player_character->base_attributes[l_1C]) goto L71590;
    l_18++;
L71590:;
    goto L7155E;
L71592:;
    if (l_18 == 0) return;
    msgbox_yes_no_rsc(403);
    if (((int)(unsigned char)*(signed char *)D_00196271) == 2) return;
    disease_remove_skill_bonuses();
    object_foreach_post(D_00195AC4, (int)func_00071475);
    player_character->conditions = 0;
    mc_memcpy((int)player_character->attributes, (int)player_character->base_attributes, 16, (int)D_00175EAA, 1708, 16);
    disease_restore_skill_bonuses();
}

void rest_open(void)
{
    int l_1C;
    int l_18;

    *(signed char *)rest_loitering = 0;
    *(int *)rest_image = 0;
    if (player_character->race != 8) goto L71835;
    if (((unsigned)(*(int *)game_minutes - player_character->last_kill_time)) > 960) goto L71837;
L71835:;
    goto L7184B;
L71837:;
    msgbox_show_rsc(36, 1);
    return;
L7184B:;
    if (*(int *)creature_count == 0) goto L7185D;
    if (((struct bf8_5_1 *)&cheat_flags)->f == 0) goto L7185F;
L7185D:;
    goto L7187A;
L7185F:;
    *(signed char *)D_0012B508 = 146;
    msgbox_show_rsc(354, 1);
    return;
L7187A:;
    if (*(int *)vertical_velocity != 0) return;
    if (*(signed char *)in_dungeon_water != 0) goto L71899;
    if (*(signed char *)D_001962A0 == 0) goto L718B4;
L71899:;
    *(signed char *)D_0012B508 = 146;
    msgbox_show_rsc(355, 1);
    return;
L718B4:;
    if ((player_character->conditions & 0x8) == 0) goto L718CB;
    if (*(signed char *)player_on_ground == 0) goto L718CD;
L718CB:;
    goto L718E5;
L718CD:;
    *(signed char *)D_0012B508 = 146;
    msgbox_show_rsc(355, 1);
    return;
L718E5:;
    *(int *)rest_image = disk_read_file((int)D_001760A4, 0);
    *(signed char *)game_mode = 16;
    *(signed char *)D_00196272 = 1;
    *(signed char *)D_00190D1A = 0;
    *(short *)D_00190DD0 = 0;
    l_18 = 1132;
    *(int *)D_00190CBC = *(int *)((char *)l_18);
    *(signed char *)D_00187CA8 = 0;
    *(signed char *)D_00196299 = 0;
}
