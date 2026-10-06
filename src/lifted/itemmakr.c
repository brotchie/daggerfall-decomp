/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern signed char key_down_esc;
extern char D_001756A3[];
extern char D_001756BE[];
extern char D_001756EC[];
extern char D_0017570A[];
extern char D_00175723[];
extern char D_00175734[];
extern char D_00175738[];
extern int D_0017D1EA;
extern char enchant_power_names[];
extern char D_00180ACE[];
extern char enchant_side_effect_names[];
extern char monster_names[];
extern char enchant_power_params[];
extern char enchant_power_costs[];
extern char D_0018586F[];
extern int D_00185871[];
extern char *enchant_spell_lists[];
extern char D_001858DB[];
extern char D_00185907[];
extern char D_00185908[];
extern char D_00185909[];
extern char D_0018590A[];
extern char D_0018597F[];
extern signed char D_00187CA8;
extern signed char text_buffer[];
extern char scratch_190be4[];
extern int scratch_190be8;
extern signed char scratch_190ce4[];
extern signed char scratch_190cee[];
extern signed char D_00190D02[];
extern signed char D_00190D63;
extern char scratch_190d64[];
extern char scratch_190d66[];
extern short scratch_190d68;
extern int D_00190EDC;
extern char scratch_190ee4[];
extern signed char text_rsc_buffer[];
extern char D_001913E4[];
extern signed char D_001940D8;
extern char D_0019574C[];
extern signed char D_001957CD;
extern char D_001957E9[];
extern struct record *player_entity;
extern struct record *player_object;
extern char cheat_flags[];
extern struct spell *spell_records;
extern struct record *scratch_object;
extern iptr magic_window_image;
extern iptr list_popup_callback;
extern char D_00195B84[];
extern struct character *player_character;
extern iptr window_image;
extern char scratch_buffer[];
extern char cfg_item_file[];
extern signed char D_00196272;
extern signed char game_mode;
extern struct magic_enchantment D_00199868[][5];   /* itemmaker_slot_exclusions */
extern char D_001998CC[];
extern struct enchantment itemmaker_slots[];
extern struct item *itemmaker_item;
extern struct record *itemmaker_item_object;
extern signed char D_00199910[];
extern char inv_left_scroll[];
extern char inv_left_rows[];
extern int D_001AA578;
extern struct record *inv_left_container;
extern short D_001AA586;
extern short inv_left_count;

extern int spells_std_names_for_ids(int);
extern int spell_cost(struct spell *, struct character *);
extern int itemmaker_row_slot(short);
extern int enchant_slot_cost(int, unsigned char, unsigned char, int);
extern int enchant_value_slot_cost(int, unsigned char, unsigned char, int);
extern int itemmaker_power_excluded(int);
extern int sound_play(int, struct record *, int);
extern int disk_create(char *);
extern int gold_can_afford(int);
extern struct record *object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern int inv_draw_item_cell(struct record *, int, int);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int mc_strncpy();
extern int write();
extern int strlen();
extern int mc_set_location(int, iptr);
extern int mc_sprintf(iptr, ...);
extern int func_000A1054();
extern void msgbox_show_string(char *, short);
extern void msgbox_show_rsc(int, int);
extern void player_refresh_paperdoll(void);
extern void itemmaker_add_power_cb(int);
extern void itemmaker_add_side_effect_cb(int);
extern void func_00057147(short, short, short, short, short, short, short);
extern void itemmaker_show_param_list(iptr, short);
extern void itemmaker_add_soul_powers(int);
extern void list_popup_open(iptr);
extern void gold_spend(int);
extern void inpstr_begin_text(iptr, short);
extern void object_foreach(struct record *, void (*)());
extern void inv_store_item(struct record *);
iptr spell_name_by_id(unsigned char);
int itemmaker_points_used(void);
int itemmaker_gold_cost(void);
int itemmaker_has_soul_bound(void);
int itemmaker_has_health_leech(void);
void itemmaker_reset(void);
void itemmaker_store_item(void);
void itemmaker_remove_slot(short);
void itemmaker_soul_list_cb(struct record *);
void itemmaker_show_list(int *, int);
void itemmaker_consume_soul(void);
void itemmaker_write_item_file(void);
void itemmaker_clear_soul_slots(void);
void func_000585D6(struct record *, int);
void func_00058AF7(void);
#pragma aux mc_set_location parm routine [];

void itemmaker_reset(void)
{
    scratch_190be8 = (*(int *)scratch_190be4 = 0);
    itemmaker_item_object = 0;
    itemmaker_item = 0;
    *(int *)inv_left_scroll = 0;
    mc_memset((iptr)scratch_190ce4, -1, 10, (iptr)D_001756A3, 77, 128);
    mc_memset((iptr)scratch_190cee, -1, 30, (iptr)D_001756A3, 78, 4);
    mc_memset((iptr)D_00190D02, -1, 30, (iptr)D_001756A3, 79, 4);
    mc_memset((iptr)D_00199868, -1, 120, (iptr)D_001756A3, 80, 120);
    mc_memset((iptr)itemmaker_slots, 0, 40, (iptr)D_001756A3, 81, 40);
    mc_memset((iptr)D_00199910, 0, 10, (iptr)D_001756A3, 82, 10);
}

int itemmaker_close(void)
{
    while (key_down_esc != 0);
    D_00187CA8 = 1;
    if ((iptr)itemmaker_item != 0) inv_store_item(itemmaker_item_object);
    game_mode = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (iptr)D_001756A3, 141);
        window_image = -1751672937;
    }
    if (magic_window_image != 0 && magic_window_image != (-1751672937)) {
        mc_free(magic_window_image, (iptr)D_001756A3, 142);
        magic_window_image = -1751672937;
    }
    D_001940D8 &= 251;
    D_00196272 = 0;
    player_refresh_paperdoll();
    return 1;
}

void itemmaker_enter_name(void)
{
    char *prompt;

    if ((iptr)itemmaker_item == 0) {
        msgbox_show_rsc(1653, 1);
        return;
    }
    prompt = *(char **)scratch_buffer + 55000;
    mc_set_location(242, (iptr)D_001756A3);
    mc_sprintf((iptr)prompt, (iptr)D_001756BE, D_0017D1EA);
    *(strlen(prompt) + prompt + 1) = 0;
    inpstr_begin_text((iptr)itemmaker_item, 23);
    msgbox_show_string(prompt, 2);
}

void itemmaker_powers_click(void)
{
    itemmaker_remove_slot((int)(short)itemmaker_row_slot(0));
}

void itemmaker_add_powers(void)
{
    int unused;

    if ((iptr)itemmaker_item == 0) {
        msgbox_show_rsc(1653, 1);
        return;
    }
    list_popup_callback = (iptr)itemmaker_add_power_cb;
    itemmaker_show_list((int *)enchant_power_names, 0);
}

void itemmaker_side_effects_click(void)
{
    itemmaker_remove_slot((int)(short)itemmaker_row_slot(1));
}

void itemmaker_set_side_effect_param_cb(short param)
{
    short exclusion_row;
    short choice;

    *(int *)&choice = *(int *)&param;
    if (itemmaker_slots[(int)(short)*(short *)scratch_190d64].type == 0) {
        *(int *)&choice = 0;
        *(int *)&param = (int)(unsigned char)text_rsc_buffer[(int)(short)param];
        itemmaker_add_soul_powers((int)(short)param);
    }
    itemmaker_slots[(int)(short)*(short *)scratch_190d64].param = *(int *)&param;
    *(int *)&exclusion_row = (int)(unsigned char)*(signed char *)(D_0018597F + ((int)(short)*(short *)scratch_190d66));
    if (exclusion_row == 0) {
        func_00057147((int)(short)*(short *)scratch_190d64, (int)(short)(*(short *)scratch_190d66 + 15), (int)(short)choice, -1, -1, -1, -1);
        return;
    }
    (*(int *)&exclusion_row)--;
    func_00057147((int)(short)*(short *)scratch_190d64, (int)(short)(*(short *)scratch_190d66 + 15), (int)(short)choice, (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185907 + ((((int)(short)exclusion_row) * 20) + (((int)(short)choice) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185908 + ((((int)(short)exclusion_row) * 20) + (((int)(short)choice) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185909 + ((((int)(short)exclusion_row) * 20) + (((int)(short)choice) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_0018590A + ((((int)(short)exclusion_row) * 20) + (((int)(short)choice) << 2)))));
}

void itemmaker_add_side_effects(void)
{
    int unused;

    if ((iptr)itemmaker_item == 0) {
        msgbox_show_rsc(1653, 1);
        return;
    }
    list_popup_callback = (iptr)itemmaker_add_side_effect_cb;
    itemmaker_show_list((int *)enchant_side_effect_names, 15);
}

void itemmaker_return_item(void)
{
    int unused;
    int unused2;

    if ((iptr)itemmaker_item != 0) inv_store_item(itemmaker_item_object);
    itemmaker_item_object = 0;
    itemmaker_item = 0;
    *(int *)scratch_190be4 = 0;
}

void itemmaker_enchant(void)
{
    int slot;
    int count;

    if ((iptr)itemmaker_item == 0) {
        msgbox_show_rsc(1653, 1);
        return;
    }
    if (((struct bf8_2_1 *)&cheat_flags)->f == 0) {
        if (itemmaker_points_used() > itemmaker_item->enchant_points) {
            msgbox_show_rsc(1651, 1);
            return;
        }
        if (gold_can_afford(itemmaker_gold_cost()) == 0) {
            msgbox_show_rsc(1650, 1);
            return;
        }
        gold_spend(itemmaker_gold_cost());
    }
    msgbox_show_rsc(1652, 1);
    count = 0;
    for (slot = 0; slot < 10; slot++) {
        if (((int)(signed char)scratch_190ce4[slot]) != (-1)) {
            itemmaker_item->enchantments[count].type = itemmaker_slots[slot].type;
            itemmaker_item->enchantments[count].param = itemmaker_slots[slot].param;
        }
        if (scratch_190ce4[slot] > 0) {
            itemmaker_item->enchantments[count].type += 15;
        }
        if (scratch_190ce4[slot] == 0 && (itemmaker_slots[slot].type == 0 || (itemmaker_slots[slot].type) == 1 || (itemmaker_slots[slot].type) == 2)) {
            itemmaker_item->enchantments[count].param = (int)(unsigned char)*(signed char *)((char *)(iptr)(enchant_spell_lists[(itemmaker_slots[slot].type)] + (itemmaker_slots[slot].param)));
        }
        if (((int)(signed char)scratch_190ce4[slot]) != (-1)) count++;
    }
    for (; count < 10; count++) {
        itemmaker_item->enchantments[count].type = 65535;
    }
    itemmaker_item->item_flags |= 32;
    itemmaker_write_item_file();
    itemmaker_consume_soul();
    itemmaker_store_item();
    sound_play(207, player_object, 100);
    itemmaker_reset();
}

void itemmaker_store_item(void)
{
    inv_store_item(itemmaker_item_object);
    itemmaker_item = 0;
    itemmaker_item_object = 0;
}

void itemmaker_remove_slot(short slot)
{
    if (((int)(short)slot) == (-1) || D_00199910[(int)(short)slot] != 0) return;
    if (((int)(signed char)scratch_190ce4[(int)(short)slot]) == 1 && itemmaker_slots[(short)slot].type == 0) {
        itemmaker_clear_soul_slots();
    }
    mc_memset(((iptr)D_00199868) + (((int)(short)slot) * 10), -1, 10, (iptr)D_001756A3, 511, 4);
    scratch_190ce4[(int)(short)slot] = 255;
    itemmaker_slots[(short)slot].param = 0;
}

void itemmaker_soul_list_cb(struct record *soul)
{
    int next_name;

    if (soul->type != 20 || *(int *)D_00195B84 > 62) return;
    if (D_00190D63 == soul->soul_creature) scratch_object = soul;
    text_rsc_buffer[*(int *)D_00195B84] = (signed char)soul->soul_creature;
    *(int *)(scratch_190ee4 + ((*(int *)D_00195B84)++ << 2)) = D_00190EDC;
    mc_strncpy(D_00190EDC, *(int *)(monster_names + (soul->soul_creature << 2)), 4, (iptr)D_001756A3, 526);
    next_name = D_00190EDC;
    next_name += strlen(*(int *)(monster_names + (soul->soul_creature << 2))) + 1;
    D_00190EDC = next_name;
}

int itemmaker_pick_param_list(int list_kind)
{
    switch ((unsigned)list_kind) {
    case 1:
    case 2:
    case 3:
        itemmaker_show_param_list(spells_std_names_for_ids(D_00185871[list_kind]), (int)(short)(list_kind - 1));
        break;
    case 4:
        *(int *)scratch_190ee4 = *(int *)scratch_buffer + 20000;
        D_00190EDC = *(int *)scratch_buffer + 21000;
        *(int *)D_00195B84 = 0;
        object_foreach(player_entity->children, itemmaker_soul_list_cb);
        if (*(int *)D_00195B84 == 0 && ((int)(short)scratch_190d68) != 2) {
            msgbox_show_string(D_001756EC, 1);
            return 0;
        }
        *(int *)(scratch_190ee4 + (*(int *)D_00195B84 << 2)) = 0;
        itemmaker_show_param_list((iptr)scratch_190ee4, 1000);
    }
    return 1;
}

int itemmaker_free_slot(void)
{
    int slot;

    slot = 0;
    while (((int)(signed char)scratch_190ce4[slot]) != (-1) && slot < 10) {
        slot++;
    }
    if (slot == 10) return -1;
    return slot;
}

int itemmaker_free_slot_count(void)
{
    int slot;
    int count;

    slot = 0;
    count = slot;
    for (; slot < 10; slot++) {
        if (((int)(signed char)scratch_190ce4[slot]) == (-1)) count++;
    }
    return count;
}

void itemmaker_show_list(int *names, int type)
{
    int slot;
    int index;
    int count;

    count = 0;
    index = count;
    while (*names != 0) {
        for (slot = 0; ((int)(short)*(short *)&slot) < 10; slot++) {
            if (((short)((unsigned short)(unsigned char)D_00199868[(int)(short)*(short *)&slot][0].type) == *(short *)&type && ((int)(unsigned char)D_00199868[(int)(short)*(short *)&slot][0].param) == 255) || ((short)((unsigned short)(unsigned char)D_00199868[(int)(short)*(short *)&slot][1].type) == *(short *)&type && ((int)(unsigned char)D_00199868[(int)(short)*(short *)&slot][1].param) == 255) || ((short)((unsigned short)(unsigned char)D_00199868[(int)(short)*(short *)&slot][2].type) == *(short *)&type && ((int)(unsigned char)D_00199868[(int)(short)*(short *)&slot][2].param) == 255) || ((short)((unsigned short)(unsigned char)D_00199868[(int)(short)*(short *)&slot][3].type) == *(short *)&type && ((int)(unsigned char)D_00199868[(int)(short)*(short *)&slot][3].param) == 255) || ((short)((unsigned short)(unsigned char)D_00199868[(int)(short)*(short *)&slot][4].type) == *(short *)&type && ((int)(unsigned char)D_00199868[(int)(short)*(short *)&slot][4].param) == 255)) {
                goto L576C2;
            }
        }
        if (itemmaker_item->group != 3 && ((int)(short)*(short *)&type) == 20) {
        } else if (itemmaker_power_excluded((int)(short)*(short *)&type) == 0) {
            if (((int)(short)*(short *)&type) != 18 && ((int)(short)*(short *)&type) != 19) {
                if (((int)(short)*(short *)&type) == 15 && itemmaker_has_soul_bound() != 0) {
                } else if (((int)(short)*(short *)&type) == 21 && itemmaker_has_health_leech() != 0) {
                } else {
                    *(signed char *)((char *)(iptr)(((int)(short)*(short *)&count) + *(char **)scratch_buffer) + 64000) = *(signed char *)&index;
                    *(int *)(scratch_190ee4 + (((int)(short)*(short *)&count) << 2)) = *names;
                    count++;
                }
            }
        }
L576C2:;
        names++;
        (*(short *)&index)++;
        type++;
    }
    *(int *)(scratch_190ee4 + (((int)(short)*(short *)&count) << 2)) = 0;
    list_popup_open((iptr)scratch_190ee4);
}

iptr spell_name_by_id(unsigned char spell_id)
{
    int i;

    for (i = 0; i < 128; i++) {
        if (spell_records[i].name[0] == 0) continue;
        if (spell_records[i].id == spell_id) return (iptr)spell_records[i].name;
    }
    return (iptr)D_0017570A;
}

int enchant_spell_cost(unsigned char spell_id)
{
    int i;
    int unused;
    int unused2;
    short unused3;

    for (i = 0; i < 35; i++) {
        *(short *)(D_001957E9 + (i * 6)) = 50;
    }
    D_001957CD = 1;
    i = 0;
    unused = i;
    for (; i < 128; i++) {
        if (spell_records[i].name[0] == 0) continue;
        if (spell_records[i].id == spell_id) {
            return spell_cost(&spell_records[i], (struct character *)D_0019574C) * 10;
        }
    }
    return 0;
}

int itemmaker_points_used(void)
{
    int slot;
    int points;
    int cost_code;
    iptr cost_table;

    slot = 0;
    points = slot;
    for (; slot < 10; slot++) {
        if (((int)(signed char)scratch_190ce4[slot]) == (-1)) continue;
        if (D_00199910[slot] != 0) continue;
        if (scratch_190ce4[slot] == 0) {
            cost_table = (iptr)enchant_power_costs;
        } else {
            cost_table = (iptr)D_001858DB;
        }
        cost_code = *(int *)((char *)(((itemmaker_slots[slot].type) << 2) + cost_table));
        if (cost_code == 0) continue;
        if (((unsigned)cost_code) < 100) {
            points += enchant_slot_cost(cost_code, (int)(unsigned char)(signed char)itemmaker_slots[slot].param, 1, itemmaker_slots[slot].type);
        } else {
            points += (int)(short)*(short *)((char *)(iptr)(*(char **)((char *)(((itemmaker_slots[slot].type) << 2) + cost_table)) + ((itemmaker_slots[slot].param) * 2)));
        }
    }
    return points;
}

int itemmaker_gold_cost(void)
{
    int slot;
    int cost;
    int cost_code;

    slot = 0;
    cost = slot;
    for (; slot < 10; slot++) {
        if (scratch_190ce4[slot] != 0) continue;
        cost_code = *(int *)(enchant_power_costs + ((itemmaker_slots[slot].type) << 2));
        if (cost_code == 0) continue;
        if (((unsigned)cost_code) < 100) {
            cost += enchant_slot_cost(cost_code, (int)(unsigned char)(signed char)itemmaker_slots[slot].param, 0, itemmaker_slots[slot].type);
        } else {
            cost += (int)(short)*(short *)((char *)(iptr)(*(char **)(enchant_power_costs + ((itemmaker_slots[slot].type) << 2)) + ((itemmaker_slots[slot].param) * 2)));
        }
    }
    return cost * 10;
}

void itemmaker_consume_soul(void)
{
    int slot;
    struct record *gem;

    for (slot = 0; slot < 10; slot++) {
        if (scratch_190ce4[slot] > 0 && itemmaker_slots[slot].type == 0) {
            goto L57E13;
        }
    }
    return;
L57E13:;
    D_00190D63 = (signed char)itemmaker_slots[slot].param;
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, itemmaker_soul_list_cb);
    gem = scratch_object->parent;
    object_delete(scratch_object);
    if (gem->data.item.enchantments[0].type == 26) return;
    object_free_single(gem);
}

int itemmaker_has_soul_bound(void)
{
    int slot;

    for (slot = 0; slot < 10; slot++) {
        if (((int)(signed char)scratch_190ce4[slot]) == 1 && itemmaker_slots[slot].type == 0) {
            return 1;
        }
    }
    return 0;
}

int itemmaker_has_health_leech(void)
{
    int slot;

    for (slot = 0; slot < 10; slot++) {
        if (((int)(signed char)scratch_190ce4[slot]) == 1 && (itemmaker_slots[slot].type) == 6) {
            return 1;
        }
    }
    return 0;
}

void itemmaker_write_item_file(void)
{
    int fd;

    if (*(signed char *)cfg_item_file == 0) return;
    fd = disk_create(cfg_item_file);
    write(fd, (iptr)itemmaker_item, 107);
    close(fd);
}

void itemmaker_clear_soul_slots(void)
{
    int slot;

    for (slot = 0; slot < 10; slot++) {
        if (D_00199910[slot] == 0) continue;
        D_00199910[slot] = 0;
        scratch_190ce4[slot] = 255;
        itemmaker_slots[slot].type = (itemmaker_slots[slot].param = 0);
        mc_memset(((iptr)D_00199868) + (slot * 10), -1, 10, (iptr)D_001756A3, 939, 4);
    }
    mc_memset((iptr)D_001998CC, -1, 20, (iptr)D_001756A3, 942, 4);
}

void func_0005852D(int cells)
{
    struct record *item;

    inv_left_count = (D_001AA586 = 0);
    mc_memset((iptr)inv_left_rows, 0, 20, (iptr)D_001756A3, 951, 20);
    if (inv_left_container != player_entity) {
        inv_draw_item_cell(inv_left_container, 0, cells);
        D_001AA578 = (int)(iptr)inv_left_container;
    }
    item = inv_left_container->children;
    while (item != 0) {
        func_000585D6(item, cells + 12);
        item = item->next;
    }
    inv_left_count = D_001AA586;
}

void func_000585D6(struct record *item, int cells)
{
    int unused;

    if (item->type != 2 || ((int)(unsigned short)(item->flags & 2)) != 0) return;
    if (((int)(short)D_001AA586) >= *(int *)inv_left_scroll && ((int)(short)D_001AA586) < (*(int *)inv_left_scroll + 4)) {
        *(iptr *)(inv_left_rows + ((((int)(short)D_001AA586) - *(int *)inv_left_scroll) << 2)) = (iptr)item;
        inv_draw_item_cell(item, (int)(short)(D_001AA586 - *(short *)inv_left_scroll), cells);
    }
    D_001AA586++;
}

void itemmaker_list_parent(void)
{
    if (inv_left_container->type == 52) return;
    inv_left_container = inv_left_container->parent;
}

void itemmaker_pick_item(int index)
{
    struct record *item;
    struct item *item_data;

    index += -9;
    item = *(struct record **)(inv_left_rows + (index << 2));
    if (item == 0) return;
    item_data = &item->data.item;
    if (item_data->enchantments[0].type != (-1)) {
        msgbox_show_rsc(1660, 1);
        return;
    }
    if (item_data->enchant_points == 0) {
        msgbox_show_rsc(1659, 1);
        return;
    }
    for (index = 0; index < 27; index++) {
        if (player_character->equipped[index] == item) player_character->equipped[index] = 0;
    }
    itemmaker_item_object = item;
    *(int *)scratch_190be4 = (int)(unsigned short)*(short *)((char *)(*(iptr *)&itemmaker_item = (iptr)item_data) + 61);
}

int enchant_item_value(struct item *item)
{
    int i;
    int value;
    int cost_code;

    i = 0;
    value = i;
    for (; i < 10; i++) {
        if (item->enchantments[i].type >= 16) continue;
        if (item->enchantments[i].type == (-1)) continue;
        cost_code = *(int *)(enchant_power_costs + (((int)(short)item->enchantments[i].type) << 2));
        if (cost_code == 0) continue;
        if (cost_code < 100 && cost_code > 0) {
            value += enchant_value_slot_cost(cost_code, (int)(unsigned char)*(signed char *)&item->enchantments[i].param, 0, (int)(short)item->enchantments[i].type);
        } else {
            value += (int)(short)*(short *)((char *)(iptr)(*(char **)(enchant_power_costs + (((int)(short)item->enchantments[i].type) << 2)) + (((int)(short)item->enchantments[i].param) * 2)));
        }
    }
    return value;
}

int itemmaker_param_excluded(int type, int param)
{
    int slot;

    func_00058AF7();
    if (type == 25) {
        for (slot = 0; slot < 10; slot++) {
            if ((itemmaker_slots[slot].type) == 25 && (itemmaker_slots[slot].param) != 5 && param == 5) {
                goto L58A19;
            }
            if ((itemmaker_slots[slot].type) == 25 && (itemmaker_slots[slot].param) == 5) {
                goto L58A19;
            }
            if ((itemmaker_slots[slot].type) == 14 && (itemmaker_slots[slot].param) == param) {
                goto L58A19;
            }
        }
    } else if (type == 14) {
        for (slot = 0; slot < 10; slot++) {
            if ((itemmaker_slots[slot].type) == 14 && (itemmaker_slots[slot].param) != 5 && param == 5) {
                goto L58A19;
            }
            if ((itemmaker_slots[slot].type) == 14 && (itemmaker_slots[slot].param) == 5) {
                goto L58A19;
            }
            if ((itemmaker_slots[slot].type) == 25 && (itemmaker_slots[slot].param) == param) {
                goto L58A19;
            }
        }
    }
    func_00058AF7();
    return 0;
L58A19:;
    func_00058AF7();
    return 1;
}

void func_00058AF7(void)
{
    int slot;

    for (slot = 0; slot < 10; slot++) {
        if ((itemmaker_slots[slot].type) == (-1)) continue;
        if (scratch_190ce4[slot] > 0) {
            if ((itemmaker_slots[slot].type) >= 15) {
                itemmaker_slots[slot].type -= 15;
            } else {
                itemmaker_slots[slot].type += 15;
            }
        }
    }
}

iptr enchant_powers_text(struct item *item)
{
    int i;
    char *out;

    i = 0;
    out = D_001913E4;
    if (((int)(unsigned short)(item->item_flags & 32)) == 0) return (iptr)D_00175723;
    for (i = 0; i < 10; i++) {
        if (item->enchantments[i].type == (-1)) continue;
        if (item->enchantments[i].type < 15) {
            mc_set_location(1128, (iptr)D_001756A3);
            mc_sprintf((iptr)text_buffer, (iptr)D_00175734, *(int *)(enchant_power_names + (item->enchantments[i].type << 2)));
            if (item->enchantments[i].param != (-1)) {
                if (item->enchantments[i].type < 3) {
                    func_000A1054((iptr)text_buffer, spell_name_by_id(item->enchantments[i].param), (iptr)D_001756A3, 1132, 160);
                } else {
                    func_000A1054((iptr)text_buffer, *(int *)((char *)(iptr)(*(char **)(enchant_power_params + (item->enchantments[i].type << 2)) + (item->enchantments[i].param << 2))), (iptr)D_001756A3, 1134, 160);
                }
            }
        } else {
            mc_set_location(1139, (iptr)D_001756A3);
            mc_sprintf((iptr)text_buffer, (iptr)D_00175734, *(int *)(D_00180ACE + (item->enchantments[i].type << 2)));
            if (item->enchantments[i].param != (-1)) {
                if (item->enchantments[i].type == 15) {
                    func_000A1054((iptr)text_buffer, *(int *)(monster_names + (item->enchantments[i].param << 2)), (iptr)D_001756A3, 1143, 160);
                } else {
                    func_000A1054((iptr)text_buffer, *(int *)((char *)(iptr)(*(char **)(D_0018586F + (item->enchantments[i].type << 2)) + (item->enchantments[i].param << 2))), (iptr)D_001756A3, 1145, 160);
                }
            }
        }
        func_000A1054((iptr)text_buffer, (iptr)D_00175738, (iptr)D_001756A3, 1148, 160);
        mc_strncpy(out, (iptr)text_buffer, 4, (iptr)D_001756A3, 1149);
        out += strlen(out);
    }
    out++;
    *out = 0;
    return (iptr)D_001913E4;
}
