/* itemmakr.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
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
extern char D_00190BE4[];
extern int D_00190BE8;
extern signed char scratch_190ce4[];
extern signed char D_00190CEE[];
extern signed char D_00190D02[];
extern signed char D_00190D63;
extern char scratch_190d64[];
extern char scratch_190d66[];
extern short scratch_190d68;
extern int D_00190EDC;
extern char D_00190EE4[];
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
extern int magic_window_image;
extern int list_popup_callback;
extern char D_00195B84[];
extern struct character *player_character;
extern int window_image;
extern char scratch_buffer[];
extern char cfg_item_file[];
extern signed char D_00196272;
extern signed char game_mode;
extern signed char D_00199868[];
extern signed char D_00199869[];
extern signed char D_0019986A[];
extern signed char D_0019986B[];
extern signed char D_0019986C[];
extern signed char D_0019986D[];
extern signed char D_0019986E[];
extern signed char D_0019986F[];
extern signed char D_00199870[];
extern signed char D_00199871[];
extern char D_001998CC[];
extern char itemmaker_slots[];
extern char D_001998E2[];
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
extern int itemmaker_row_slot(int);
extern int enchant_slot_cost(int, unsigned char, unsigned char, short);
extern int enchant_value_slot_cost(int, unsigned char, unsigned char, short);
extern int itemmaker_power_excluded(short);
extern int sound_play(int, int, int);
extern int disk_create(int);
extern int gold_can_afford(int);
extern int object_free_single(struct record *);
extern int object_delete(int);
extern int inv_draw_item_cell(struct record *, int, int);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int mc_strncpy();
extern int write();
extern int strlen();
extern int mc_set_location(int, int);
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
extern void list_popup_open(int);
extern void gold_spend(int);
extern void inpstr_begin_text(int, short);
extern void object_foreach(struct record *, int);
extern void inv_store_item(int);
int spell_name_by_id(unsigned char);
int itemmaker_points_used(void);
int itemmaker_gold_cost(void);
int itemmaker_has_soul_bound(void);
int itemmaker_has_health_leech(void);
void itemmaker_reset(void);
void itemmaker_store_item(void);
void itemmaker_remove_slot(short);
void itemmaker_soul_list_cb(struct record *);
void itemmaker_show_list(int, int);
void itemmaker_consume_soul(void);
void itemmaker_write_item_file(void);
void itemmaker_clear_soul_slots(void);
void func_000585D6(struct record *, int);
void func_00058AF7(void);
#pragma aux mc_set_location parm routine [];

void itemmaker_reset(void)
{
    D_00190BE8 = (*(int *)D_00190BE4 = 0);
    itemmaker_item_object = 0;
    itemmaker_item = 0;
    *(int *)inv_left_scroll = 0;
    mc_memset((int)scratch_190ce4, -1, 10, (int)D_001756A3, 77, 128);
    mc_memset((int)D_00190CEE, -1, 30, (int)D_001756A3, 78, 4);
    mc_memset((int)D_00190D02, -1, 30, (int)D_001756A3, 79, 4);
    mc_memset((int)D_00199868, -1, 120, (int)D_001756A3, 80, 120);
    mc_memset((int)itemmaker_slots, 0, 40, (int)D_001756A3, 81, 40);
    mc_memset((int)D_00199910, 0, 10, (int)D_001756A3, 82, 10);
}

int itemmaker_close(void)
{
    while (key_down_esc != 0);
    D_00187CA8 = 1;
    if ((int)itemmaker_item != 0) inv_store_item((int)itemmaker_item_object);
    game_mode = 0;
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_001756A3, 141);
        window_image = -1751672937;
    }
    if (magic_window_image != 0 && magic_window_image != (-1751672937)) {
        mc_free(magic_window_image, (int)D_001756A3, 142);
        magic_window_image = -1751672937;
    }
    D_001940D8 &= 251;
    D_00196272 = 0;
    player_refresh_paperdoll();
    return 1;
}

void itemmaker_enter_name(void)
{
    int l_18;

    if ((int)itemmaker_item == 0) {
        msgbox_show_rsc(1653, 1);
        return;
    }
    l_18 = *(int *)scratch_buffer + 55000;
    mc_set_location(242, (int)D_001756A3);
    mc_sprintf(l_18, (int)D_001756BE, D_0017D1EA);
    *(signed char *)((char *)(strlen(l_18) + l_18) + 1) = 0;
    inpstr_begin_text((int)itemmaker_item, 23);
    msgbox_show_string(l_18, 2);
}

void itemmaker_powers_click(void)
{
    itemmaker_remove_slot((int)(short)itemmaker_row_slot(0));
}

void itemmaker_add_powers(void)
{
    int l_18;

    if ((int)itemmaker_item == 0) {
        msgbox_show_rsc(1653, 1);
        return;
    }
    list_popup_callback = (int)itemmaker_add_power_cb;
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
    if (*(short *)(itemmaker_slots + (((int)(short)*(short *)scratch_190d64) << 2)) == 0) {
        *(int *)&l_18 = 0;
        *(int *)&a1 = (int)(unsigned char)text_rsc_buffer[(int)(short)a1];
        itemmaker_add_soul_powers((int)(short)a1);
    }
    *(short *)(D_001998E2 + (((int)(short)*(short *)scratch_190d64) << 2)) = *(int *)&a1;
    *(int *)&l_1C = (int)(unsigned char)*(signed char *)(D_0018597F + ((int)(short)*(short *)scratch_190d66));
    if (l_1C == 0) {
        func_00057147((int)(short)*(short *)scratch_190d64, (int)(short)(*(short *)scratch_190d66 + 15), (int)(short)l_18, -1, -1, -1, -1);
        return;
    }
    (*(int *)&l_1C)--;
    func_00057147((int)(short)*(short *)scratch_190d64, (int)(short)(*(short *)scratch_190d66 + 15), (int)(short)l_18, (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185907 + ((((int)(short)l_1C) * 20) + (((int)(short)l_18) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185908 + ((((int)(short)l_1C) * 20) + (((int)(short)l_18) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_00185909 + ((((int)(short)l_1C) * 20) + (((int)(short)l_18) << 2)))), (int)(short)((unsigned short)(unsigned char)*(signed char *)(D_0018590A + ((((int)(short)l_1C) * 20) + (((int)(short)l_18) << 2)))));
}

void itemmaker_add_side_effects(void)
{
    int l_18;

    if ((int)itemmaker_item == 0) {
        msgbox_show_rsc(1653, 1);
        return;
    }
    list_popup_callback = (int)itemmaker_add_side_effect_cb;
    itemmaker_show_list((int)enchant_side_effect_names, 15);
}

void itemmaker_return_item(void)
{
    int l_1C;
    int l_18;

    if ((int)itemmaker_item != 0) inv_store_item((int)itemmaker_item_object);
    itemmaker_item_object = 0;
    itemmaker_item = 0;
    *(int *)D_00190BE4 = 0;
}

void itemmaker_enchant(void)
{
    int l_1C;
    int l_18;

    if ((int)itemmaker_item == 0) {
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
    l_18 = 0;
    for (l_1C = 0; l_1C < 10; l_1C++) {
        if (((int)(signed char)scratch_190ce4[l_1C]) != (-1)) {
            itemmaker_item->enchantments[l_18].type = *(short *)(itemmaker_slots + (l_1C << 2));
            itemmaker_item->enchantments[l_18].param = *(short *)(D_001998E2 + (l_1C << 2));
        }
        if (scratch_190ce4[l_1C] > 0) {
            itemmaker_item->enchantments[l_18].type += 15;
        }
        if (scratch_190ce4[l_1C] == 0 && (*(short *)(itemmaker_slots + (l_1C << 2)) == 0 || ((int)(short)*(short *)(itemmaker_slots + (l_1C << 2))) == 1 || ((int)(short)*(short *)(itemmaker_slots + (l_1C << 2))) == 2)) {
            itemmaker_item->enchantments[l_18].param = (int)(unsigned char)*(signed char *)((char *)(int)(enchant_spell_lists[((int)(short)*(short *)(itemmaker_slots + (l_1C << 2)))] + ((int)(short)*(short *)(D_001998E2 + (l_1C << 2)))));
        }
        if (((int)(signed char)scratch_190ce4[l_1C]) != (-1)) l_18++;
    }
    for (; l_18 < 10; l_18++) {
        itemmaker_item->enchantments[l_18].type = 65535;
    }
    itemmaker_item->item_flags |= 32;
    itemmaker_write_item_file();
    itemmaker_consume_soul();
    itemmaker_store_item();
    sound_play(207, (int)player_object, 100);
    itemmaker_reset();
}

void itemmaker_store_item(void)
{
    inv_store_item((int)itemmaker_item_object);
    itemmaker_item = 0;
    itemmaker_item_object = 0;
}

void itemmaker_remove_slot(short a1)
{
    if (((int)(short)a1) == (-1) || D_00199910[(int)(short)a1] != 0) return;
    if (((int)(signed char)scratch_190ce4[(int)(short)a1]) == 1 && *(short *)(itemmaker_slots + (((int)(short)a1) << 2)) == 0) {
        itemmaker_clear_soul_slots();
    }
    mc_memset(((int)D_00199868) + (((int)(short)a1) * 10), -1, 10, (int)D_001756A3, 511, 4);
    scratch_190ce4[(int)(short)a1] = 255;
    *(short *)(D_001998E2 + (((int)(short)a1) << 2)) = 0;
}

void itemmaker_soul_list_cb(struct record *a1)
{
    int l_18;

    if (a1->type != 20 || *(int *)D_00195B84 > 62) return;
    if (D_00190D63 == a1->soul_creature) scratch_object = a1;
    text_rsc_buffer[*(int *)D_00195B84] = (signed char)a1->soul_creature;
    *(int *)(D_00190EE4 + ((*(int *)D_00195B84)++ << 2)) = D_00190EDC;
    mc_strncpy(D_00190EDC, *(int *)(monster_names + (a1->soul_creature << 2)), 4, (int)D_001756A3, 526);
    l_18 = D_00190EDC;
    l_18 += strlen(*(int *)(monster_names + (a1->soul_creature << 2))) + 1;
    D_00190EDC = l_18;
}

int itemmaker_pick_param_list(int a1)
{
    switch ((unsigned)a1) {
    case 1:
    case 2:
    case 3:
        itemmaker_show_param_list(spells_std_names_for_ids(D_00185871[a1]), (int)(short)(a1 - 1));
        break;
    case 4:
        *(int *)D_00190EE4 = *(int *)scratch_buffer + 20000;
        D_00190EDC = *(int *)scratch_buffer + 21000;
        *(int *)D_00195B84 = 0;
        object_foreach(player_entity->children, (int)itemmaker_soul_list_cb);
        if (*(int *)D_00195B84 == 0 && ((int)(short)scratch_190d68) != 2) {
            msgbox_show_string((int)D_001756EC, 1);
            return 0;
        }
        *(int *)(D_00190EE4 + (*(int *)D_00195B84 << 2)) = 0;
        itemmaker_show_param_list((int)D_00190EE4, 1000);
    }
    return 1;
}

int itemmaker_free_slot(void)
{
    int l_1C;

    l_1C = 0;
    while (((int)(signed char)scratch_190ce4[l_1C]) != (-1) && l_1C < 10) {
        l_1C++;
    }
    if (l_1C == 10) return -1;
    return l_1C;
}

int itemmaker_free_slot_count(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = l_20;
    for (; l_20 < 10; l_20++) {
        if (((int)(signed char)scratch_190ce4[l_20]) == (-1)) l_1C++;
    }
    return l_1C;
}

void itemmaker_show_list(int a1, int a2)
{
    int l_1C;
    int l_18;
    int l_14;

    l_14 = 0;
    l_18 = l_14;
    while (*(int *)((char *)a1) != 0) {
        for (l_1C = 0; ((int)(short)*(short *)&l_1C) < 10; l_1C++) {
            if (((short)((unsigned short)(unsigned char)D_00199868[((int)(short)*(short *)&l_1C) * 10]) == *(short *)&a2 && ((int)(unsigned char)D_00199869[((int)(short)*(short *)&l_1C) * 10]) == 255) || ((short)((unsigned short)(unsigned char)D_0019986A[((int)(short)*(short *)&l_1C) * 10]) == *(short *)&a2 && ((int)(unsigned char)D_0019986B[((int)(short)*(short *)&l_1C) * 10]) == 255) || ((short)((unsigned short)(unsigned char)D_0019986C[((int)(short)*(short *)&l_1C) * 10]) == *(short *)&a2 && ((int)(unsigned char)D_0019986D[((int)(short)*(short *)&l_1C) * 10]) == 255) || ((short)((unsigned short)(unsigned char)D_0019986E[((int)(short)*(short *)&l_1C) * 10]) == *(short *)&a2 && ((int)(unsigned char)D_0019986F[((int)(short)*(short *)&l_1C) * 10]) == 255) || ((short)((unsigned short)(unsigned char)D_00199870[((int)(short)*(short *)&l_1C) * 10]) == *(short *)&a2 && ((int)(unsigned char)D_00199871[((int)(short)*(short *)&l_1C) * 10]) == 255)) {
                goto L576C2;
            }
        }
        if (itemmaker_item->group != 3 && ((int)(short)*(short *)&a2) == 20) {
        } else if (itemmaker_power_excluded((int)(short)*(short *)&a2) == 0) {
            if (((int)(short)*(short *)&a2) != 18 && ((int)(short)*(short *)&a2) != 19) {
                if (((int)(short)*(short *)&a2) == 15 && itemmaker_has_soul_bound() != 0) {
                } else if (((int)(short)*(short *)&a2) == 21 && itemmaker_has_health_leech() != 0) {
                } else {
                    *(signed char *)((char *)(int)(((int)(short)*(short *)&l_14) + *(char **)scratch_buffer) + 64000) = *(signed char *)&l_18;
                    *(int *)(D_00190EE4 + (((int)(short)*(short *)&l_14) << 2)) = *(int *)((char *)a1);
                    l_14++;
                }
            }
        }
L576C2:;
        (*(char (**)[4])&a1)++;
        (*(short *)&l_18)++;
        a2++;
    }
    *(int *)(D_00190EE4 + (((int)(short)*(short *)&l_14) << 2)) = 0;
    list_popup_open((int)D_00190EE4);
}

int spell_name_by_id(unsigned char a1)
{
    int l_20;

    for (l_20 = 0; l_20 < 128; l_20++) {
        if (spell_records[l_20].name[0] == 0) continue;
        if (spell_records[l_20].id == a1) return (int)spell_records[l_20].name;
    }
    return (int)D_0017570A;
}

int enchant_spell_cost(unsigned char a1)
{
    int l_28;
    int l_24;
    int l_2C;
    short l_1C;

    for (l_28 = 0; l_28 < 35; l_28++) {
        *(short *)(D_001957E9 + (l_28 * 6)) = 50;
    }
    D_001957CD = 1;
    l_28 = 0;
    l_24 = l_28;
    for (; l_28 < 128; l_28++) {
        if (spell_records[l_28].name[0] == 0) continue;
        if (spell_records[l_28].id == a1) {
            return spell_cost(&spell_records[l_28], (struct character *)D_0019574C) * 10;
        }
    }
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
    for (; l_28 < 10; l_28++) {
        if (((int)(signed char)scratch_190ce4[l_28]) == (-1)) continue;
        if (D_00199910[l_28] != 0) continue;
        if (scratch_190ce4[l_28] == 0) {
            l_1C = (int)enchant_power_costs;
        } else {
            l_1C = (int)D_001858DB;
        }
        l_20 = *(int *)((char *)((((int)(short)*(short *)(itemmaker_slots + (l_28 << 2))) << 2) + l_1C));
        if (l_20 == 0) continue;
        if (((unsigned)l_20) < 100) {
            l_24 += enchant_slot_cost(l_20, (int)(unsigned char)*(signed char *)(D_001998E2 + (l_28 << 2)), 1, (int)(short)*(short *)(itemmaker_slots + (l_28 << 2)));
        } else {
            l_24 += (int)(short)*(short *)((char *)(int)(*(char **)((char *)((((int)(short)*(short *)(itemmaker_slots + (l_28 << 2))) << 2) + l_1C)) + (((int)(short)*(short *)(D_001998E2 + (l_28 << 2))) * 2)));
        }
    }
    return l_24;
}

int itemmaker_gold_cost(void)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_20 = l_24;
    for (; l_24 < 10; l_24++) {
        if (scratch_190ce4[l_24] != 0) continue;
        l_1C = *(int *)(enchant_power_costs + (((int)(short)*(short *)(itemmaker_slots + (l_24 << 2))) << 2));
        if (l_1C == 0) continue;
        if (((unsigned)l_1C) < 100) {
            l_20 += enchant_slot_cost(l_1C, (int)(unsigned char)*(signed char *)(D_001998E2 + (l_24 << 2)), 0, (int)(short)*(short *)(itemmaker_slots + (l_24 << 2)));
        } else {
            l_20 += (int)(short)*(short *)((char *)(int)(*(char **)(enchant_power_costs + (((int)(short)*(short *)(itemmaker_slots + (l_24 << 2))) << 2)) + (((int)(short)*(short *)(D_001998E2 + (l_24 << 2))) * 2)));
        }
    }
    return l_20 * 10;
}

void itemmaker_consume_soul(void)
{
    int l_1C;
    struct record *l_18;

    for (l_1C = 0; l_1C < 10; l_1C++) {
        if (scratch_190ce4[l_1C] > 0 && *(short *)(itemmaker_slots + (l_1C << 2)) == 0) {
            goto L57E13;
        }
    }
    return;
L57E13:;
    D_00190D63 = *(signed char *)(D_001998E2 + (l_1C << 2));
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, (int)itemmaker_soul_list_cb);
    l_18 = scratch_object->parent;
    object_delete((int)scratch_object);
    if (l_18->data.item.enchantments[0].type == 26) return;
    object_free_single(l_18);
}

int itemmaker_has_soul_bound(void)
{
    int l_1C;

    for (l_1C = 0; l_1C < 10; l_1C++) {
        if (((int)(signed char)scratch_190ce4[l_1C]) == 1 && *(short *)(itemmaker_slots + (l_1C << 2)) == 0) {
            return 1;
        }
    }
    return 0;
}

int itemmaker_has_health_leech(void)
{
    int l_1C;

    for (l_1C = 0; l_1C < 10; l_1C++) {
        if (((int)(signed char)scratch_190ce4[l_1C]) == 1 && ((int)(short)*(short *)(itemmaker_slots + (l_1C << 2))) == 6) {
            return 1;
        }
    }
    return 0;
}

void itemmaker_write_item_file(void)
{
    int l_18;

    if (*(signed char *)cfg_item_file == 0) return;
    l_18 = disk_create((int)cfg_item_file);
    write(l_18, (int)itemmaker_item, 107);
    close(l_18);
}

void itemmaker_clear_soul_slots(void)
{
    int l_18;

    for (l_18 = 0; l_18 < 10; l_18++) {
        if (D_00199910[l_18] == 0) continue;
        D_00199910[l_18] = 0;
        scratch_190ce4[l_18] = 255;
        *(short *)(itemmaker_slots + (l_18 << 2)) = (*(short *)(D_001998E2 + (l_18 << 2)) = 0);
        mc_memset(((int)D_00199868) + (l_18 * 10), -1, 10, (int)D_001756A3, 939, 4);
    }
    mc_memset((int)D_001998CC, -1, 20, (int)D_001756A3, 942, 4);
}

void func_0005852D(int a1)
{
    struct record *l_18;

    inv_left_count = (D_001AA586 = 0);
    mc_memset((int)inv_left_rows, 0, 20, (int)D_001756A3, 951, 20);
    if (inv_left_container != player_entity) {
        inv_draw_item_cell(inv_left_container, 0, a1);
        D_001AA578 = (int)inv_left_container;
    }
    l_18 = inv_left_container->children;
    while (l_18 != 0) {
        func_000585D6(l_18, a1 + 12);
        l_18 = l_18->next;
    }
    inv_left_count = D_001AA586;
}

void func_000585D6(struct record *a1, int a2)
{
    int l_14;

    if (a1->type != 2 || ((int)(unsigned short)(a1->flags & 2)) != 0) return;
    if (((int)(short)D_001AA586) >= *(int *)inv_left_scroll && ((int)(short)D_001AA586) < (*(int *)inv_left_scroll + 4)) {
        *(int *)(inv_left_rows + ((((int)(short)D_001AA586) - *(int *)inv_left_scroll) << 2)) = (int)a1;
        inv_draw_item_cell(a1, (int)(short)(D_001AA586 - *(short *)inv_left_scroll), a2);
    }
    D_001AA586++;
}

void itemmaker_list_parent(void)
{
    if (inv_left_container->type == 52) return;
    inv_left_container = inv_left_container->parent;
}

void itemmaker_pick_item(int a1)
{
    struct record *l_1C;
    struct item *l_18;

    a1 += -9;
    l_1C = *(struct record **)(inv_left_rows + (a1 << 2));
    if (l_1C == 0) return;
    l_18 = &l_1C->data.item;
    if (l_18->enchantments[0].type != (-1)) {
        msgbox_show_rsc(1660, 1);
        return;
    }
    if (l_18->enchant_points == 0) {
        msgbox_show_rsc(1659, 1);
        return;
    }
    for (a1 = 0; a1 < 27; a1++) {
        if (player_character->equipped[a1] == l_1C) player_character->equipped[a1] = 0;
    }
    itemmaker_item_object = l_1C;
    *(int *)D_00190BE4 = (int)(unsigned short)*(short *)((char *)(*(int *)&itemmaker_item = (int)l_18) + 61);
}

int enchant_item_value(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_20 = l_24;
    for (; l_24 < 10; l_24++) {
        if (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67)) >= 16) continue;
        if (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67)) == (-1)) continue;
        l_1C = *(int *)(enchant_power_costs + (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67)) << 2));
        if (l_1C == 0) continue;
        if (l_1C < 100 && l_1C > 0) {
            l_20 += enchant_value_slot_cost(l_1C, (int)(unsigned char)*(signed char *)((char *)((l_24 << 2) + a1) + 69), 0, (int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67));
        } else {
            l_20 += (int)(short)*(short *)((char *)(int)(*(char **)(enchant_power_costs + (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 67)) << 2)) + (((int)(short)*(short *)((char *)((l_24 << 2) + a1) + 69)) * 2)));
        }
    }
    return l_20;
}

int itemmaker_param_excluded(int a1, int a2)
{
    int l_18;

    func_00058AF7();
    if (a1 == 25) {
        for (l_18 = 0; l_18 < 10; l_18++) {
            if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) == 25 && ((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) != 5 && a2 == 5) {
                goto L58A19;
            }
            if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) == 25 && ((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) == 5) {
                goto L58A19;
            }
            if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) == 14 && ((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) == a2) {
                goto L58A19;
            }
        }
    } else if (a1 == 14) {
        for (l_18 = 0; l_18 < 10; l_18++) {
            if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) == 14 && ((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) != 5 && a2 == 5) {
                goto L58A19;
            }
            if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) == 14 && ((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) == 5) {
                goto L58A19;
            }
            if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) == 25 && ((int)(short)*(short *)(D_001998E2 + (l_18 << 2))) == a2) {
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
    int l_18;

    for (l_18 = 0; l_18 < 10; l_18++) {
        if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) == (-1)) continue;
        if (scratch_190ce4[l_18] > 0) {
            if (((int)(short)*(short *)(itemmaker_slots + (l_18 << 2))) >= 15) {
                *(short *)(itemmaker_slots + (l_18 << 2)) -= 15;
            } else {
                *(short *)(itemmaker_slots + (l_18 << 2)) += 15;
            }
        }
    }
}

int enchant_powers_text(struct item *a1)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = (int)D_001913E4;
    if (((int)(unsigned short)(a1->item_flags & 32)) == 0) return (int)D_00175723;
    for (l_20 = 0; l_20 < 10; l_20++) {
        if (a1->enchantments[l_20].type == (-1)) continue;
        if (a1->enchantments[l_20].type < 15) {
            mc_set_location(1128, (int)D_001756A3);
            mc_sprintf((int)text_buffer, (int)D_00175734, *(int *)(enchant_power_names + (a1->enchantments[l_20].type << 2)));
            if (a1->enchantments[l_20].param != (-1)) {
                if (a1->enchantments[l_20].type < 3) {
                    func_000A1054((int)text_buffer, spell_name_by_id(a1->enchantments[l_20].param), (int)D_001756A3, 1132, 160);
                } else {
                    func_000A1054((int)text_buffer, *(int *)((char *)(int)(*(char **)(enchant_power_params + (a1->enchantments[l_20].type << 2)) + (a1->enchantments[l_20].param << 2))), (int)D_001756A3, 1134, 160);
                }
            }
        } else {
            mc_set_location(1139, (int)D_001756A3);
            mc_sprintf((int)text_buffer, (int)D_00175734, *(int *)(D_00180ACE + (a1->enchantments[l_20].type << 2)));
            if (a1->enchantments[l_20].param != (-1)) {
                if (a1->enchantments[l_20].type == 15) {
                    func_000A1054((int)text_buffer, *(int *)(monster_names + (a1->enchantments[l_20].param << 2)), (int)D_001756A3, 1143, 160);
                } else {
                    func_000A1054((int)text_buffer, *(int *)((char *)(int)(*(char **)(D_0018586F + (a1->enchantments[l_20].type << 2)) + (a1->enchantments[l_20].param << 2))), (int)D_001756A3, 1145, 160);
                }
            }
        }
        func_000A1054((int)text_buffer, (int)D_00175738, (int)D_001756A3, 1148, 160);
        mc_strncpy(l_1C, (int)text_buffer, 4, (int)D_001756A3, 1149);
        l_1C += strlen(l_1C);
    }
    l_1C++;
    *(signed char *)((char *)l_1C) = 0;
    return (int)D_001913E4;
}
