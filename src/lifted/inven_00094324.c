/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern short mouse_x;
extern short mouse_y;
extern signed char D_001940D8;
extern struct item *text_macro_item;
extern struct record *player_object;
extern struct record *D_00195AA8;
extern struct record *location_object;
extern struct record *inv_right_container;
extern struct character *player_character;
extern int trade_mode;
extern int inventory_action;
extern char D_00195D54[];
extern char D_00196120[];
extern struct record *inv_selected_item;

extern int object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern int inv_take_item(struct record *);
extern int inv_paperdoll_slot_at(short, short, int);
extern int trade_can_repair_item(struct item *);
extern void msgbox_show_rsc(int, int);
extern void quest_raise_event(int, int, int);
extern void inv_use_item(void);
extern void inv_item_info(struct record *, struct item *);
extern void item_remove_equip_effects(struct record *, int);
extern void trade_schedule_repair(void);

void inv_click_paperdoll(void)
{
    int l_2C;
    struct item *l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct record *l_18;

    l_1C = inv_paperdoll_slot_at((int)(short)mouse_x, (int)(short)mouse_y, l_1C);
    if (l_1C == 0) return;
    D_001940D8 |= 8;
    l_1C += -64;
    l_18 = (inv_selected_item = player_character->equipped[l_1C]);
    D_00195AA8 = inv_selected_item;
    l_28 = &inv_selected_item->data.item;
    text_macro_item = l_28;
    l_2C = inventory_action - 1;
    switch (l_2C) {
    case 0:
        inv_item_info(inv_selected_item, l_28);
        return;
    case 1:
        inv_take_item(inv_selected_item);
        return;
    case 2:
        if (trade_mode == 4) {
            item_remove_equip_effects(inv_selected_item, l_1C);
            player_character->equipped[l_1C] = 0;
            l_18->x = player_object->x;
            l_18->y = player_object->y;
            l_18->z = player_object->z;
            l_18->caster = 0;
            if (l_18->image == 0) l_18->image = l_28->dropped_image;
            object_reparent(inv_right_container, l_18);
            l_18->id = object_new_id(0);
            return;
        }
        if (trade_mode == 3 && l_28->enchantments[0].type != (-1)) {
            msgbox_show_rsc(33, 1);
            return;
        }
        if (trade_mode == 3 && trade_can_repair_item(l_28) == 0) return;
        item_remove_equip_effects(inv_selected_item, l_1C);
        player_character->equipped[l_1C] = 0;
        l_18->x = player_object->x;
        l_18->y = player_object->y;
        l_18->z = player_object->z;
        l_18->caster = 0;
        if (l_18->image == 0) l_18->image = l_28->dropped_image;
        object_reparent(inv_right_container, l_18);
        if (((int)D_00196120) == (int)l_18->parent) l_18->owner = *(short *)D_00195D54;
        if (l_18->quest_id == 0 && trade_mode == 0) {
            l_18->id = object_new_id(((unsigned)location_object->id) >> 16);
        }
        quest_raise_event(5, (int)l_18, 0);
        trade_schedule_repair();
        return;
    case 3:
        inv_use_item();
    default:;
    }
}
