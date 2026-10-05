/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00095D2C to 0x00095F82, kept together for its switch table's alignment */
#include "records.h"
struct flags8 { unsigned char b0:2; unsigned char b2:1; };
extern char D_0012B508;
extern char D_0017704C[];       /* __FILE__ */
extern char *D_001832A4;
extern unsigned char D_001860DA[];
extern unsigned char D_00186104[];
extern short D_00188208[];
extern signed char text_buffer[];
extern unsigned char D_001940D8;
extern struct record *wagon_container;
extern int player_object;
extern struct record *inv_right_container;
extern struct record *inv_right_container_base;
extern struct character *player_character;
extern unsigned char D_0019626F;
extern unsigned char game_mode;
extern unsigned char inv_right_icon;
extern int D_001AA454;
extern char inv_right_rows[];
extern struct record *D_001AA558;
extern struct record *inv_selected_item;
extern char inv_left_rows[];
extern struct record *D_001AA578;
extern struct record *inv_left_container;
extern short inv_right_count;
extern short D_001AA586;
extern short D_001AA588;
extern short inv_left_count;
extern void msgbox_show_string(char *, int);
extern void msgbox_show_rsc(int, int);
extern int sound_play(int, int, int);
extern void gold_add(int);
extern int object_free_single(struct record *);
extern void inv_draw_container_icon(int, unsigned char);
extern int inv_draw_item_cell(struct record *, short, struct rect *);
extern void inv_list_left_item(struct record *, struct rect *);
extern void inv_list_right_item(struct record *, struct rect *);
extern void inv_equip_in_slot_pair(struct record *, int, int);
extern void inv_unequip_slot(int);
extern void inv_equip_in_slot(struct record *, int);
extern int item_is_two_handed(struct record *);
extern void item_remove_equip_effects(struct record *, int);
extern void inv_store_item(struct record *);
extern int item_forbidden_for_class(struct item *);
extern void mc_memset(char *, int, int, char *, int, int);
extern int xn_str_find_u32();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);
void inv_equip_item(struct record *object);

void inv_draw_left_list(struct rect *rects)
{
    struct record *object;

    inv_left_count = D_001AA586 = 0;
    mc_memset(inv_left_rows, 0, 20, D_0017704C, 1627, 20);
    if (inv_left_container->type == 2 && inv_left_container != wagon_container) {
        inv_draw_item_cell(inv_left_container, 0, rects);
        D_001AA578 = inv_left_container;
    } else if (inv_left_container == wagon_container) {
        inv_draw_container_icon(27, inv_right_icon);
    } else if (game_mode != 4 && D_0019626F != 4 && !((struct flags8 *)&D_001940D8)->b2) {
        return;
    }
    object = inv_left_container->children;
    while (object != 0) {
        inv_list_left_item(object, rects + 1);
        object = object->next;
    }
    inv_left_count = D_001AA586;
}

void inv_draw_right_list(struct rect *rects)
{
    struct record *object;

    inv_right_count = D_001AA588 = 0;
    mc_memset(inv_right_rows, 0, 20, D_0017704C, 1655, 20);
    if (inv_right_container != inv_right_container_base) {
        inv_draw_item_cell(inv_right_container, 0, rects);
        D_001AA558 = inv_right_container;
    }
    object = inv_right_container->children;
    while (object != 0) {
        inv_list_right_item(object, rects + 1);
        object = object->next;
    }
    inv_right_count = D_001AA588;
}

void func_00095EDB(void)
{
    int unused1;
    int unused2;

    D_001AA454 = 0;
    if (xn_str_find_u32(player_character->equipped, inv_selected_item, 27) != 0)
        return;
    inv_store_item(inv_selected_item);
    inv_equip_item(inv_selected_item);
    if (D_001AA454 == 0)
        return;
    gold_add(D_001AA454);
    D_0012B508 = 144;
    mc_set_location(1688, D_0017704C);
    mc_sprintf(((char *)text_buffer), D_001832A4, D_001AA454);
    msgbox_show_string(((char *)text_buffer), 1);
}

void inv_equip_item(struct record *object)
{
    struct item *item;
    unsigned char *slot_table;
    int unused;

    item = &object->data.item;
    if (item->group == 3 && item->index == 18)
        return;
    if (item->group == 15 || item->group == 16 || item->group == 17 || item->group == 18 ||
        item->group == 19 || item->group == 20 || item->group == 21 || item->group == 22)
        return;
    if (item->condition == 0) {
        msgbox_show_rsc(29, 1);
        return;
    }
    D_001940D8 |= 8;
    switch (item->group) {
    case 28:
        switch (item->index) {
        case 0:
            if (game_mode == 4)
                sound_play(204, player_object, 100);
            D_001AA454 += item->value;
            object_free_single(object);
            break;
        }
        break;
    case 2:
        if (item_forbidden_for_class(item) != 0)
            return;
        switch (item->index) {
        case 0:
            if (game_mode == 4)
                sound_play(item->armor_type + 231, player_object, 100);
            inv_equip_in_slot(object, 18);
            break;
        case 1:
            if (game_mode == 4)
                sound_play(233, player_object, 100);
            inv_equip_in_slot(object, 20);
            break;
        case 2:
            if (game_mode == 4)
                sound_play(item->armor_type + 231, player_object, 100);
            inv_equip_in_slot(object, 23);
            break;
        case 3:
            if (game_mode == 4)
                sound_play(item->armor_type + 231, player_object, 100);
            inv_equip_in_slot(object, 15);
            break;
        case 4:
            if (game_mode == 4)
                sound_play(item->armor_type + 231, player_object, 100);
            inv_equip_in_slot(object, 13);
            break;
        case 5:
            if (game_mode == 4)
                sound_play(233, player_object, 100);
            inv_equip_in_slot(object, 12);
            break;
        case 6:
            if (game_mode == 4)
                sound_play(item->armor_type + 231, player_object, 100);
            inv_equip_in_slot(object, 26);
            break;
        case 7:
        case 8:
        case 9:
        case 10:
            if (game_mode == 4)
                sound_play(233, player_object, 100);
            if (item_is_two_handed(player_character->equipped[EQUIP_RIGHT_HAND]) != 0) {
                item_remove_equip_effects(player_character->equipped[EQUIP_RIGHT_HAND], 19);
                player_character->equipped[EQUIP_RIGHT_HAND] = 0;
                inv_equip_in_slot(object, 21);
                return;
            }
            inv_equip_in_slot(object, 21);
            break;
        }
        break;
    case 3:
        if (item_forbidden_for_class(item) != 0)
            return;
        if (game_mode == 4)
            sound_play(D_00188208[item->index], player_object, 100);
        if (item_is_two_handed(object) != 0) {
            if (item_is_two_handed(player_character->equipped[EQUIP_RIGHT_HAND]) != 0) {
                inv_equip_in_slot(object, 19);
                return;
            }
            if (player_character->equipped[EQUIP_RIGHT_HAND] == 0 && player_character->equipped[EQUIP_LEFT_HAND] == 0) {
                inv_equip_in_slot(object, 19);
                return;
            }
            if (player_character->equipped[EQUIP_RIGHT_HAND] != 0) {
                if (player_character->equipped[EQUIP_LEFT_HAND] != 0)
                    inv_unequip_slot(21);
                inv_equip_in_slot(object, 19);
                return;
            }
            if (player_character->equipped[EQUIP_LEFT_HAND] != 0) {
                inv_unequip_slot(21);
                inv_equip_in_slot(object, 19);
                return;
            }
        } else {
            if (item_is_two_handed(player_character->equipped[EQUIP_RIGHT_HAND]) != 0) {
                inv_equip_in_slot(object, 19);
                return;
            }
            inv_equip_in_slot_pair(object, 19, 2);
        }
        break;
    case 6:
    case 12:
        if (item->group == 6 && (player_character->flags & 1))
            return;
        if (item->group == 12 && !(player_character->flags & 1))
            return;
        if (game_mode == 4)
            sound_play(234, player_object, 100);
        if (item->group == 12)
            slot_table = D_00186104;
        else
            slot_table = D_001860DA;
        switch (slot_table[item->index]) {
        case 12:
            inv_equip_in_slot(object, 12);
            break;
        case 13:
            inv_equip_in_slot_pair(object, 14, 2);
            break;
        case 17:
            inv_equip_in_slot(object, 17);
            break;
        case 22:
            inv_equip_in_slot(object, 24);
            break;
        case 26:
            inv_equip_in_slot(object, 26);
            break;
        }
        break;
    case 14:
        if (game_mode == 4)
            sound_play(236, player_object, 100);
        inv_equip_in_slot_pair(object, 10, 1);
        break;
    case 25:
        if (game_mode == 4)
            sound_play(236, player_object, 100);
        switch (item->index) {
        case 0:
            inv_equip_in_slot_pair(object, 0, 1);
            break;
        case 1:
            inv_equip_in_slot_pair(object, 6, 1);
            break;
        case 2:
            inv_equip_in_slot_pair(object, 4, 1);
            break;
        case 3:
            inv_equip_in_slot_pair(object, 2, 1);
            break;
        case 4:
            inv_equip_in_slot_pair(object, 8, 1);
            break;
        case 5:
            inv_equip_in_slot_pair(object, 0, 1);
            break;
        case 6:
            inv_equip_in_slot_pair(object, 2, 1);
            break;
        }
        break;
    }
}
