/* guilds.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_3_1 { unsigned char _:3; unsigned char f:1; };
struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern int screen_buffer;
extern char D_00175EAA[];
extern char D_0017606F[];
extern char D_0017607C[];
extern char D_001760A4[];
extern int trade_price_scale;
extern struct spell *selected_spell;
extern char spell_effect_names[];
extern char spell_effect_subtype_names[];
extern signed char guild_rank_primary_skill[];
extern signed char guild_rank_secondary_skill[];
extern int guild_skill_lists[];
extern short guild_service_factions[];
extern int guild_service_labels[];
extern char guild_menu_buttons[];
extern char D_0018750B[];
extern char D_0018750D[];
extern char D_0018750F[];
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern int D_00190CBC;
extern signed char D_00190D1A;
extern signed char scratch_190d20;
extern short D_00190DD0;
extern short guild_search_faction;
extern signed char D_001940D4;
extern signed char D_001940D8;
extern signed char D_001940D9;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern int vertical_velocity;
extern struct record *location_object;
extern char cheat_flags[];
extern struct record *found_object;
extern int creature_count;
extern int magic_window_image;
extern struct character *player_character;
extern int window_image;
extern int game_minutes;
extern char D_001960D9[];
extern signed char current_region;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char player_on_ground;
extern signed char mouse_buttons_prev;
extern signed char in_dungeon_water;
extern signed char D_00196299;
extern signed char D_001962A0;
extern signed char D_001962AF;
extern struct faction *D_0019671C;
extern int guild_saved_screen;
extern struct membership *guild_membership;
extern short D_001A4A18;
extern short D_001A4A1A;
extern signed char D_001A4A1C;
extern char rest_image[];
extern signed char rest_loitering;
extern char shared_picklist[];

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
extern int itoa();
extern int mc_memcpy();
extern int xn_draw_spell_icon();
extern int xn_gfx_present_inclusive();
extern int xn_draw_copy_rect_stride_bytes();
extern int xn_mouse_poll_clamped();
extern int xn_draw_image();
extern void msgbox_show_rsc(int, int);
extern void keys_world_actions(void);
extern void item_make(int, int, struct item *);
extern void shop_stock_magic(int, int, int, int);
extern void shop_stock_potions(int);
extern void disease_remove_skill_bonuses(void);
extern void disease_restore_skill_bonuses(void);
extern void spellshop_update(void);
extern void spellshop_show_effect(int);
extern void blessing_remove(int);
extern void text_draw_coloured(int, int, int, int, unsigned char);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
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
void guild_match_membership(struct record *);
void guild_match_membership_bits(struct record *);
void guild_heal_cleanup(struct record *);

int func_0006F484(int a1)
{
    struct membership *l_1C;

    if (a1 == 4) {
        l_1C = guild_find_membership_by_kind(0);
        if (l_1C == 0) return 0;
        return ((rand_range(1, 100) <= ((l_1C->rank + 1) * 5)) ? 1 : 0);
    }
    if (a1 < 3) {
        l_1C = guild_find_membership_by_kind(3);
        if (l_1C == 0) return 0;
        return ((rand_range(1, 100) <= ((l_1C->rank + 1) * 5)) ? 1 : 0);
    }
    return 0;
}

void guild_buy_potions(void)
{
    object_free_children((int)D_001960D9);
    shop_stock_potions((int)D_001960D9);
    inventory_open_container((int)D_001960D9, 1, 4);
}

void guild_buy_spells(void)
{
    guild_saved_screen = mc_malloc(64000, (int)D_00175EAA, 871);
    mc_memcpy(guild_saved_screen, screen_buffer, 64000, (int)D_00175EAA, 873, 4);
    spellshop_open();
    while (((int)(unsigned char)game_mode) == 5) {
        mc_memcpy(screen_buffer, guild_saved_screen, 64000, (int)D_00175EAA, 879, 4);
        keys_world_actions();
        spellshop_update();
        player_movement_update();
        xn_gfx_present_inclusive(1);
    }
    if (guild_saved_screen == 0 || guild_saved_screen == (-1751672937)) return;
    mc_free(guild_saved_screen, (int)D_00175EAA, 886);
    guild_saved_screen = -1751672937;
}

void guild_buy_magic_items(void)
{
    int l_18;

    l_18 = rand();
    object_free_children((int)D_001960D9);
    srand(current_building->id);
    if (guild_membership == 0 || guild_membership->rank < 4) {
        shop_stock_magic((int)D_001960D9, 0, 1, 0);
    } else {
        shop_stock_magic((int)D_001960D9, 0, 1, 1);
    }
    srand(l_18);
    D_001940D9 |= 2;
    if (holiday_today(game_minutes, (int)(unsigned char)current_region) == 38) {
        trade_price_scale = 128;
    }
    inventory_open_container((int)D_001960D9, 1, 4);
    D_001962AF = 1;
}

void spellshop_open(void)
{
    D_001A4A18 = rand();
    srand((int)(short)D_001A4A1A);
    game_mode = 5;
    D_001940D8 &= 254;
    D_001940D8 |= 2;
    window_image = disk_read_file((int)D_0017606F, 0);
    magic_window_image = disk_read_file((int)D_0017607C, 0);
    D_00196272 = 1;
    if (spellshop_build_list() != 0) return;
    spellshop_close();
}

int spellshop_close(void)
{
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0) picklist_free((int)shared_picklist);
    srand((int)(short)D_001A4A18);
    D_001940D8 &= 253;
    game_mode = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00175EAA, 963);
        window_image = -1751672937;
    }
    if (magic_window_image != 0 && magic_window_image != (-1751672937)) {
        mc_free(magic_window_image, (int)D_00175EAA, 964);
        magic_window_image = -1751672937;
    }
    D_00196272 = 0;
    return 1;
}

void spellshop_buy(void)
{
    int l_1C;
    struct record *l_18;

    l_1C = spell_cost(selected_spell, player_character) << 2;
    if (holiday_today(game_minutes, (int)(unsigned char)current_region) == 43) {
        l_1C >>= 1;
    }
    l_1C = guild_confirm_price(l_1C);
    if (l_1C < 0) return;
    if (gold_can_afford(l_1C) == 0) {
        msgbox_show_rsc(454, 1);
        return;
    }
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

    D_0012B508 = 145;
    xn_draw_spell_icon(172, 32, a1->icon);
    xn_draw_copy_rect_stride_bytes((int)(*(char **)&magic_window_image + (a1->element * 640)) + 24, (int)(*(char **)&screen_buffer + 10486), 16, 16, 40);
    xn_draw_copy_rect_stride_bytes((int)(*(char **)&magic_window_image + (a1->target * 640)), (int)&*(signed char *)(*(char **)&screen_buffer + 10445), 24, 16, 40);
    text_draw_coloured((int)a1->name, 148, 20, 145, 141);
    *(int *)&l_18 = 0;
    for (; ((int)(short)l_18) < 3; (*(int *)&l_18)++) {
        if (a1->effects[(int)(short)l_18].type == 255) continue;
        text_draw_centred_coloured(*(int *)(spell_effect_names + (a1->effects[(int)(short)l_18].type << 2)), 219, (int)(short)((*(int *)&l_18 * 38) + 63), 145, 141);
        if (a1->effects[(int)(short)l_18].subtype != 255 && *(int *)(spell_effect_subtype_names + (a1->effects[(int)(short)l_18].type * 48) + (a1->effects[(int)(short)l_18].subtype << 2)) != 0) {
            text_draw_centred_coloured(*(int *)(spell_effect_subtype_names + (a1->effects[(int)(short)l_18].type * 48) + (a1->effects[(int)(short)l_18].subtype << 2)), 219, (int)(short)((*(int *)&l_18 * 38) + 75), 145, 141);
        }
    }
    l_1C = spell_cost(a1, player_character) << 2;
    if (holiday_today(game_minutes, (int)(unsigned char)current_region) == 43) {
        l_1C >>= 1;
    }
    text_draw_coloured(itoa(l_1C, (int)text_buffer, 10), 97, 172, 145, 156);
}

void spellshop_effect_button_1(void)
{
    spellshop_show_effect(0);
}

void spellshop_effect_button_2(void)
{
    spellshop_show_effect(1);
}

void spellshop_effect_button_3(void)
{
    spellshop_show_effect(2);
}

int guild_rank_for_skills(int a1)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_2C = D_0019671C->reputation / 10;
    l_28 = guild_best_skill((int)&l_20, guild_skill_lists[a1], -1);
    l_24 = guild_best_skill((int)&l_20, guild_skill_lists[a1], l_20);
    l_20 = 0;
    while (((int)(unsigned char)guild_rank_primary_skill[l_20++]) < l_28);
    l_20--;
    l_1C = 0;
    while (((int)(unsigned char)guild_rank_secondary_skill[l_1C++]) < l_24);
    l_1C--;
    if (l_20 < l_2C) l_2C = l_20;
    if (l_1C < l_2C) l_2C = l_1C;
    if (l_2C > 9) l_2C = 9;
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
    do {
        l_14 = player_character->skills[(int)(unsigned char)*(signed char *)((char *)(a2 + l_20))].value;
        if (l_14 > l_1C && ((int)(unsigned char)*(signed char *)((char *)(a2 + l_20))) != a3) {
            l_1C = l_14;
            l_18 = (int)(unsigned char)*(signed char *)((char *)(a2 + l_20));
        }
        ++l_20;
    } while (((int)(unsigned char)*(signed char *)((char *)(a2 + l_20))) != 255);
    *(int *)((char *)a1) = l_18;
    return l_1C;
}

void guild_match_membership(struct record *a1)
{
    if (a1->type != 10) return;
    if (scratch_190d20 == a1->data.membership.kind) found_object = a1;
    if (guild_search_faction != a1->data.membership.faction) return;
    found_object = a1;
}

void guild_match_membership_bits(struct record *a1)
{
    if (a1->type != 10) return;
    if ((((int)(signed char)scratch_190d20) & a1->data.membership.kind) == 0) return;
    found_object = a1;
}

struct membership *guild_find_membership_by_kind(unsigned char a1)
{
    found_object = 0;
    guild_search_faction = 0;
    scratch_190d20 = a1;
    object_foreach(player_entity->children, (int)guild_match_membership);
    if (found_object == 0) return 0;
    return &found_object->data.membership;
}

struct membership *guild_find_membership_by_bits(unsigned char a1)
{
    found_object = 0;
    guild_search_faction = 0;
    scratch_190d20 = a1;
    object_foreach(player_entity->children, (int)guild_match_membership_bits);
    if (found_object == 0) return 0;
    return &found_object->data.membership;
}

void guild_expire_blessings(void)
{
    struct record *l_1C;
    int l_18;

    l_1C = player_entity->children;
    while (l_1C != 0) {
        if (l_1C->type == 30) {
            l_18 = (int)RECORD_DATA(l_1C);
            if (((unsigned)*(int *)((char *)l_18 + 2)) < game_minutes) {
                blessing_remove(l_18);
                object_free_single(l_1C);
                return;
            }
        }
        l_1C = l_1C->next;
    }
}

int guild_service_label(short a1)
{
    int l_20;

    for (l_20 = 0; l_20 < 92; l_20++) {
        if (guild_service_factions[l_20] == a1) {
            return guild_service_labels[l_20];
        }
    }
    return 0;
}

int guild_menu(int a1, int a2, int a3)
{
    int l_18;
    int l_14;

    l_18 = -1;
    guild_saved_screen = mc_malloc(64000, (int)D_00175EAA, 1581);
    mc_memcpy(guild_saved_screen, screen_buffer, 64000, (int)D_00175EAA, 1582, 4);
    D_00196272 = 1;
    while (l_18 == (-1)) {
        mc_memcpy(screen_buffer, guild_saved_screen, 64000, (int)D_00175EAA, 1587, 4);
        xn_draw_image((int)(unsigned short)*(short *)((char *)a1), (int)(unsigned short)*(short *)((char *)a1 + 2), (int)(unsigned short)*(short *)((char *)a1 + 4), (int)(unsigned short)*(short *)((char *)a1 + 6), a1 + 12);
        text_draw_centred_coloured(a3, 159, 71, 145, 141);
        keys_world_actions();
        player_movement_update();
        xn_gfx_present_inclusive(1);
        if (key_down_esc != 0) {
            while (key_down_esc != 0);
            l_18 = 3;
            break;
        }
        if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
            for (l_14 = 0; l_14 < 4; l_14++) {
                if (mouse_x > *(short *)(guild_menu_buttons + (l_14 * 12)) && mouse_x < *(short *)(D_0018750D + (l_14 * 12)) && mouse_y > *(short *)(D_0018750B + (l_14 * 12)) && mouse_y < *(short *)(D_0018750F + (l_14 * 12))) {
                    if (l_14 == 0 && a2 != 0) continue;
                    sound_play(203, (int)player_object, 100);
                    l_18 = l_14;
                }
            }
        }
    }
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    D_00196272 = 0;
    if (guild_saved_screen != 0 && guild_saved_screen != (-1751672937)) {
        mc_free(guild_saved_screen, (int)D_00175EAA, 1617);
        guild_saved_screen = -1751672937;
    }
    return l_18;
}

int guild_is_local_knight(void)
{
    int l_24;
    struct membership *l_20;
    struct faction *l_1C;

    l_20 = guild_find_membership_by_bits(64);
    if (l_20 == 0) return 0;
    l_1C = faction_find(l_20->faction);
    if ((signed char)l_1C->region == current_region) return 1;
    if (l_20->rank >= 4) {
        l_24 = 1;
    } else {
        l_24 = 0;
    }
    return l_24;
}

int guild_local_temple_rank(void)
{
    struct membership *l_20;
    struct faction *l_1C;

    l_20 = guild_find_membership_by_bits(128);
    if (l_20 == 0) return 0;
    l_1C = faction_find(l_20->faction);
    if ((signed char)l_1C->region == current_region) return l_20->rank;
    return 0;
}

void guild_give_map(int a1)
{
    struct record *l_18;

    D_001A4A1C = 1;
    l_18 = object_create_child(location_object, 0, 107);
    l_18->type = 2;
    item_make(11, 0, &l_18->data.item);
    object_reparent((struct record *)D_001960D9, l_18);
}

void guild_heal_cleanup(struct record *a1)
{
    struct disease *l_18;

    switch (a1->type) {
    case 11:
        if (((int)(unsigned short)(a1->flags & 32768)) != 0) {
            l_18 = &a1->data.disease;
            if (l_18->id > 99) return;
        }
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
    for (l_1C = 0; l_1C < 8; l_1C++) {
        if (player_character->attributes[l_1C] < player_character->base_attributes[l_1C]) l_18++;
    }
    if (l_18 == 0) return;
    msgbox_yes_no_rsc(403);
    if (((int)D_00196271) == 2) return;
    disease_remove_skill_bonuses();
    object_foreach_post(location_object, (int)guild_heal_cleanup);
    player_character->conditions = 0;
    mc_memcpy((int)player_character->attributes, (int)player_character->base_attributes, 16, (int)D_00175EAA, 1708, 16);
    disease_restore_skill_bonuses();
}

void rest_open(void)
{
    int l_1C;
    int l_18;

    rest_loitering = 0;
    *(int *)rest_image = 0;
    if (player_character->race == 8 && ((unsigned)(game_minutes - player_character->last_kill_time)) > 960) {
        msgbox_show_rsc(36, 1);
        return;
    }
    if (creature_count != 0 && ((struct bf8_5_1 *)&cheat_flags)->f == 0) {
        D_0012B508 = 146;
        msgbox_show_rsc(354, 1);
        return;
    }
    if (vertical_velocity != 0) return;
    if (in_dungeon_water != 0 || D_001962A0 != 0) {
        D_0012B508 = 146;
        msgbox_show_rsc(355, 1);
        return;
    }
    if ((player_character->conditions & 0x8) != 0 && player_on_ground == 0) {
        D_0012B508 = 146;
        msgbox_show_rsc(355, 1);
        return;
    }
    *(int *)rest_image = disk_read_file((int)D_001760A4, 0);
    game_mode = 16;
    D_00196272 = 1;
    D_00190D1A = 0;
    D_00190DD0 = 0;
    l_18 = 1132;
    D_00190CBC = *(int *)((char *)l_18);
    D_00187CA8 = 0;
    D_00196299 = 0;
}
