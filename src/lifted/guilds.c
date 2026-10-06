/* guilds.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"
#include "doslow.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern signed char key_down_esc;
extern iptr screen_buffer;
extern char D_00175EAA[];
extern char D_0017606F[];
extern char D_0017607C[];
extern char D_001760A4[];
extern int trade_price_scale;
extern struct spell *selected_spell;
extern char *spell_effect_names[];
extern char *spell_effect_subtype_names[][12];
extern signed char guild_rank_primary_skill[];
extern signed char guild_rank_secondary_skill[];
extern iptr guild_skill_lists[];
extern short guild_service_factions[];
extern iptr guild_service_labels[];
extern struct rect guild_menu_buttons[];
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
extern iptr magic_window_image;
extern struct character *player_character;
extern iptr window_image;
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
extern iptr guild_saved_screen;
extern struct membership *guild_membership;
extern short D_001A4A18;
extern short D_001A4A1A;
extern signed char D_001A4A1C;
extern struct image *rest_image;
extern signed char rest_loitering;
extern char shared_picklist[];

extern struct faction *faction_find(short);
extern int spell_cost(struct spell *, struct character *);
extern int holiday_today(int, int);
extern int sound_play(int, struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern int spellshop_build_list(void);
extern int guild_confirm_price(int);
extern int rand_range(int, int);
extern int gold_can_afford(int);
extern struct record *object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern struct record *object_find_item(struct record *, short, short);
extern int object_new_id(int);
extern void xn_draw_spell_icon(int, int, int);
extern void xn_gfx_present_inclusive(int);
extern int xn_draw_copy_rect_stride_bytes(char *, char *, int, int, int);
extern int xn_mouse_poll_clamped(void);
extern void xn_draw_image(int, int, int, int, char *);
extern void msgbox_show_rsc(int, int);
extern void keys_world_actions(void);
extern void item_make(int, int, struct item *);
extern void shop_stock_magic(iptr, int, int, int);
extern void shop_stock_potions(iptr);
extern void disease_remove_skill_bonuses(void);
extern void disease_restore_skill_bonuses(void);
extern void spellshop_update(void);
extern void spellshop_show_effect(short);
extern void blessing_remove(struct blessing *);
extern void text_draw_coloured(iptr, int, int, int, unsigned char);
extern void text_draw_centred_coloured(iptr, int, int, int, unsigned char);
extern void msgbox_yes_no_rsc(int);
extern void gold_spend(int);
extern void player_movement_update(void);
extern void picklist_free(iptr);
extern void object_free_children(iptr);
extern void object_foreach_post(struct record *, void (*)());
extern void object_foreach(struct record *, void (*)());
extern void inventory_open_container(iptr, int, int);
int spellshop_close(void);
int guild_best_skill(int *, unsigned char *, int);
struct membership *guild_find_membership_by_kind(unsigned char);
struct membership *guild_find_membership_by_bits(unsigned char);
void spellshop_open(void);
void guild_match_membership(struct record *);
void guild_match_membership_bits(struct record *);
void guild_heal_cleanup(struct record *);

int func_0006F484(int guild)
{
    struct membership *membership;

    if (guild == 4) {
        membership = guild_find_membership_by_kind(0);
        if (membership == 0) return 0;
        return ((rand_range(1, 100) <= ((membership->rank + 1) * 5)) ? 1 : 0);
    }
    if (guild < 3) {
        membership = guild_find_membership_by_kind(3);
        if (membership == 0) return 0;
        return ((rand_range(1, 100) <= ((membership->rank + 1) * 5)) ? 1 : 0);
    }
    return 0;
}

void guild_buy_potions(void)
{
    object_free_children((iptr)D_001960D9);
    shop_stock_potions((iptr)D_001960D9);
    inventory_open_container((iptr)D_001960D9, 1, 4);
}

void guild_buy_spells(void)
{
    guild_saved_screen = (iptr)mc_malloc(64000, D_00175EAA, 871);
    mc_memcpy((void *)guild_saved_screen, (void *)screen_buffer, 64000, D_00175EAA, 873, 4);
    spellshop_open();
    while (((int)(unsigned char)game_mode) == 5) {
        mc_memcpy((void *)screen_buffer, (void *)guild_saved_screen, 64000, D_00175EAA, 879, 4);
        keys_world_actions();
        spellshop_update();
        player_movement_update();
        xn_gfx_present_inclusive(1);
    }
    if (guild_saved_screen == 0 || guild_saved_screen == (-1751672937)) return;
    mc_free((void *)guild_saved_screen, D_00175EAA, 886);
    guild_saved_screen = -1751672937;
}

void guild_buy_magic_items(void)
{
    int seed;

    seed = rand();
    object_free_children((iptr)D_001960D9);
    srand(current_building->id);
    if (guild_membership == 0 || guild_membership->rank < 4) {
        shop_stock_magic((iptr)D_001960D9, 0, 1, 0);
    } else {
        shop_stock_magic((iptr)D_001960D9, 0, 1, 1);
    }
    srand(seed);
    D_001940D9 |= 2;
    if (holiday_today(game_minutes, (int)(unsigned char)current_region) == 38) {
        trade_price_scale = 128;
    }
    inventory_open_container((iptr)D_001960D9, 1, 4);
    D_001962AF = 1;
}

void spellshop_open(void)
{
    D_001A4A18 = rand();
    srand((int)(short)D_001A4A1A);
    game_mode = 5;
    D_001940D8 &= 254;
    D_001940D8 |= 2;
    window_image = disk_read_file(D_0017606F, 0);
    magic_window_image = disk_read_file(D_0017607C, 0);
    D_00196272 = 1;
    if (spellshop_build_list() != 0) return;
    spellshop_close();
}

int spellshop_close(void)
{
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0) picklist_free((iptr)shared_picklist);
    srand((int)(short)D_001A4A18);
    D_001940D8 &= 253;
    game_mode = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free((void *)window_image, D_00175EAA, 963);
        window_image = -1751672937;
    }
    if (magic_window_image != 0 && magic_window_image != (-1751672937)) {
        mc_free((void *)magic_window_image, D_00175EAA, 964);
        magic_window_image = -1751672937;
    }
    D_00196272 = 0;
    return 1;
}

void spellshop_buy(void)
{
    int price;
    struct record *object;

    price = spell_cost(selected_spell, player_character) << 2;
    if (holiday_today(game_minutes, (int)(unsigned char)current_region) == 43) {
        price >>= 1;
    }
    price = guild_confirm_price(price);
    if (price < 0) return;
    if (gold_can_afford(price) == 0) {
        msgbox_show_rsc(454, 1);
        return;
    }
    gold_spend(price);
    object = object_find_item(player_entity->children, 27, 0);
    object = object_create_child(object, 0, 89);
    object->type = 9;
    object->id = object_new_id(100);
    mc_memcpy(&object->data.spell, selected_spell, 89, D_00175EAA, 992, 4);
}

void spellshop_draw_spell(struct spell *spell)
{
    int price;
    slot16 i;

    D_0012B508 = 145;
    xn_draw_spell_icon(172, 32, spell->icon);
    xn_draw_copy_rect_stride_bytes((char *)((iptr)(*(char **)&magic_window_image + (spell->element * 640)) + 24), (*(char **)&screen_buffer + 10486), 16, 16, 40);
    xn_draw_copy_rect_stride_bytes((*(char **)&magic_window_image + (spell->target * 640)), (char *)&*(signed char *)(*(char **)&screen_buffer + 10445), 24, 16, 40);
    text_draw_coloured((iptr)spell->name, 148, 20, 145, 141);
    *(int *)&i = 0;
    for (; ((int)(short)i) < 3; (*(int *)&i)++) {
        if (spell->effects[(int)(short)i].type == 255) continue;
        text_draw_centred_coloured((iptr)spell_effect_names[spell->effects[(int)(short)i].type], 219, (int)(short)((*(int *)&i * 38) + 63), 145, 141);
        if (spell->effects[(int)(short)i].subtype != 255 && (iptr)spell_effect_subtype_names[spell->effects[(int)(short)i].type][spell->effects[(int)(short)i].subtype] != 0) {
            text_draw_centred_coloured((iptr)spell_effect_subtype_names[spell->effects[(int)(short)i].type][spell->effects[(int)(short)i].subtype], 219, (int)(short)((*(int *)&i * 38) + 75), 145, 141);
        }
    }
    price = spell_cost(spell, player_character) << 2;
    if (holiday_today(game_minutes, (int)(unsigned char)current_region) == 43) {
        price >>= 1;
    }
    text_draw_coloured((iptr)itoa(price, (char *)text_buffer, 10), 97, 172, 145, 156);
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

int guild_rank_for_skills(int guild)
{
    int rank;
    int primary;
    int secondary;
    int primary_step;
    int secondary_step;

    rank = D_0019671C->reputation / 10;
    primary = guild_best_skill(&primary_step, (unsigned char *)guild_skill_lists[guild], -1);
    secondary = guild_best_skill(&primary_step, (unsigned char *)guild_skill_lists[guild], primary_step);
    primary_step = 0;
    while (((int)(unsigned char)guild_rank_primary_skill[primary_step++]) < primary);
    primary_step--;
    secondary_step = 0;
    while (((int)(unsigned char)guild_rank_secondary_skill[secondary_step++]) < secondary);
    secondary_step--;
    if (primary_step < rank) rank = primary_step;
    if (secondary_step < rank) rank = secondary_step;
    if (rank > 9) rank = 9;
    return rank;
}

int guild_best_skill(int *best_id, unsigned char *skills, int skip_id)
{
    int i;
    int best_value;
    int best_skill;
    int value;

    i = 0;
    best_value = -1;
    do {
        value = player_character->skills[skills[i]].value;
        if (value > best_value && skills[i] != skip_id) {
            best_value = value;
            best_skill = skills[i];
        }
        ++i;
    } while (skills[i] != 255);
    *best_id = best_skill;
    return best_value;
}

void guild_match_membership(struct record *object)
{
    if (object->type != 10) return;
    if (scratch_190d20 == object->data.membership.kind) found_object = object;
    if (guild_search_faction != object->data.membership.faction) return;
    found_object = object;
}

void guild_match_membership_bits(struct record *object)
{
    if (object->type != 10) return;
    if ((((int)(signed char)scratch_190d20) & object->data.membership.kind) == 0) return;
    found_object = object;
}

struct membership *guild_find_membership_by_kind(unsigned char kind)
{
    found_object = 0;
    guild_search_faction = 0;
    scratch_190d20 = kind;
    object_foreach(player_entity->children, guild_match_membership);
    if (found_object == 0) return 0;
    return &found_object->data.membership;
}

struct membership *guild_find_membership_by_bits(unsigned char bits)
{
    found_object = 0;
    guild_search_faction = 0;
    scratch_190d20 = bits;
    object_foreach(player_entity->children, guild_match_membership_bits);
    if (found_object == 0) return 0;
    return &found_object->data.membership;
}

void guild_expire_blessings(void)
{
    struct record *object;
    struct blessing *blessing;

    object = player_entity->children;
    while (object != 0) {
        if (object->type == 30) {
            blessing = &object->data.blessing;
            if (blessing->end_time < game_minutes) {
                blessing_remove(blessing);
                object_free_single(object);
                return;
            }
        }
        object = object->next;
    }
}

iptr guild_service_label(short faction_id)
{
    int i;

    for (i = 0; i < 92; i++) {
        if (guild_service_factions[i] == faction_id) {
            return guild_service_labels[i];
        }
    }
    return 0;
}

int guild_menu(struct image *image, int is_member, iptr label)
{
    int choice;
    int button;

    choice = -1;
    guild_saved_screen = (iptr)mc_malloc(64000, D_00175EAA, 1581);
    mc_memcpy((void *)guild_saved_screen, (void *)screen_buffer, 64000, D_00175EAA, 1582, 4);
    D_00196272 = 1;
    while (choice == (-1)) {
        mc_memcpy((void *)screen_buffer, (void *)guild_saved_screen, 64000, D_00175EAA, 1587, 4);
        xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
        text_draw_centred_coloured(label, 159, 71, 145, 141);
        keys_world_actions();
        player_movement_update();
        xn_gfx_present_inclusive(1);
        if (key_down_esc != 0) {
            while (key_down_esc != 0);
            choice = 3;
            break;
        }
        if (mouse_buttons != 0 && mouse_buttons_prev == 0) {
            for (button = 0; button < 4; button++) {
                if (mouse_x > guild_menu_buttons[button].x0 && mouse_x < guild_menu_buttons[button].x1 && mouse_y > guild_menu_buttons[button].y0 && mouse_y < guild_menu_buttons[button].y1) {
                    if (button == 0 && is_member != 0) continue;
                    sound_play(203, player_object, 100);
                    choice = button;
                }
            }
        }
    }
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    D_00196272 = 0;
    if (guild_saved_screen != 0 && guild_saved_screen != (-1751672937)) {
        mc_free((void *)guild_saved_screen, D_00175EAA, 1617);
        guild_saved_screen = -1751672937;
    }
    return choice;
}

int guild_is_local_knight(void)
{
    int result;
    struct membership *membership;
    struct faction *faction;

    membership = guild_find_membership_by_bits(64);
    if (membership == 0) return 0;
    faction = faction_find(membership->faction);
    if ((signed char)faction->region == current_region) return 1;
    if (membership->rank >= 4) {
        result = 1;
    } else {
        result = 0;
    }
    return result;
}

int guild_local_temple_rank(void)
{
    struct membership *membership;
    struct faction *faction;

    membership = guild_find_membership_by_bits(128);
    if (membership == 0) return 0;
    faction = faction_find(membership->faction);
    if ((signed char)faction->region == current_region) return membership->rank;
    return 0;
}

void guild_give_map(int brotherhood)
{
    struct record *map;

    D_001A4A1C = 1;
    map = object_create_child(location_object, 0, 107);
    map->type = 2;
    item_make(11, 0, &map->data.item);
    object_reparent((struct record *)D_001960D9, map);
}

void guild_heal_cleanup(struct record *object)
{
    struct disease *disease;

    switch (object->type) {
    case 11:
        if (((int)(unsigned short)(object->flags & 32768)) != 0) {
            disease = &object->data.disease;
            if (disease->id > 99) return;
        }
        object_delete(object);
        return;
    case 9:
        switch (object->parent->type) {
        case 1:
        case 3:
        case 38:
            object_delete(object);
        default:;
        }
    default:;
    }
}

void guild_heal(void)
{
    int i;
    int lowered;

    lowered = 0;
    player_character->health = player_character->max_health;
    msgbox_show_rsc(350, 1);
    for (i = 0; i < 8; i++) {
        if (player_character->attributes[i] < player_character->base_attributes[i]) lowered++;
    }
    if (lowered == 0) return;
    msgbox_yes_no_rsc(403);
    if (((int)D_00196271) == 2) return;
    disease_remove_skill_bonuses();
    object_foreach_post(location_object, guild_heal_cleanup);
    player_character->conditions = 0;
    mc_memcpy(player_character->attributes, player_character->base_attributes, 16, D_00175EAA, 1708, 16);
    disease_restore_skill_bonuses();
}

void rest_open(void)
{
    int unused;
    int *bios_ticks;

    rest_loitering = 0;
    *(iptr *)&rest_image = 0;
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
    *(iptr *)&rest_image = disk_read_file(D_001760A4, 0);
    game_mode = 16;
    D_00196272 = 1;
    D_00190D1A = 0;
    D_00190DD0 = 0;
    bios_ticks = (int *)DOS_LOW(0x46C);
    D_00190CBC = *bios_ticks;
    D_00187CA8 = 0;
    D_00196299 = 0;
}
