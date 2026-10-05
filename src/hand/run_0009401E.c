/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00093F0C to 0x0009401E, kept together for its switch table's alignment */
#include "records.h"
extern struct item *text_macro_item;
extern struct record *player_object;
extern struct record *scratch_current_object;
extern struct record *inv_right_container;
extern struct character *player_character;
extern int trade_mode;
extern int inventory_action;
extern short picked_model_index;
extern struct record D_00196120;   /* furniture_container_3 (candidate) */
extern struct record *inv_selected_item;
extern void quest_raise_event(short, struct record *, struct record *);
extern struct record *object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern int inv_take_item(struct record *);
extern void inv_use_item(void);
extern void inv_item_info(struct record *, struct item *);
extern void item_remove_equip_effects(struct record *, int);
extern int trade_can_repair_item(struct item *);
extern void trade_schedule_repair(void);
extern struct record *inventory_containers[];
extern unsigned char inv_right_icon;
extern int inv_left_scroll;
extern int inv_right_scroll;
extern struct record *inv_left_container;
extern short inv_right_count;
extern short inv_left_count;
extern unsigned char D_001AA5F8;
extern unsigned char inv_tab;
extern void inv_wagon_button(void);

void inv_scroll_left_down(void)
{
    short last;

    last = inv_left_count - 4;
    if (last > 0 && last > inv_left_scroll)
        inv_left_scroll++;
}

void inv_scroll_right_up(void)
{
    if (inv_right_scroll == 0)
        return;
    inv_right_scroll--;
}

void inv_scroll_right_down(void)
{
    short last;

    last = inv_right_count - 4;
    if (last > 0 && last > inv_right_scroll)
        inv_right_scroll++;
}

void inv_select_tab(int button)
{
    button -= 41;
    if (inventory_containers[button] == 0)
        return;
    if (trade_mode != 0) {
        inv_right_icon = D_001AA5F8;
        inv_wagon_button();
    }
    inv_tab = button;
    inv_left_container = inventory_containers[inv_tab];
    inv_left_scroll = 0;
}

void inv_click_equip_slot(int slot)
{
    struct item *item;
    int unused1;
    int unused2;
    struct record *object;

    object = inv_selected_item = player_character->equipped[slot];
    if (object == 0)
        return;
    scratch_current_object = inv_selected_item;
    item = &inv_selected_item->data.item;
    text_macro_item = item;
    switch (inventory_action) {
    case 1:
        inv_item_info(inv_selected_item, item);
        break;
    case 2:
        inv_take_item(inv_selected_item);
        break;
    case 3:
        if (trade_mode == 4) {
            item_remove_equip_effects(inv_selected_item, slot);
            player_character->equipped[slot] = 0;
            object->x = player_object->x;
            object->y = player_object->y;
            object->z = player_object->z;
            object->caster = 0;
            if (object->image == 0)
                object->image = item->dropped_image;
            object_reparent(inv_right_container, object);
            object->id = object_new_id(0);
            break;
        }
        if (trade_mode == 3 && trade_can_repair_item(item) == 0)
            break;
        item_remove_equip_effects(inv_selected_item, slot);
        player_character->equipped[slot] = 0;
        object->x = player_object->x;
        object->y = player_object->y;
        object->z = player_object->z;
        object->caster = 0;
        if (object->image == 0)
            object->image = item->dropped_image;
        if (object->quest_id == 0 && trade_mode == 0)
            object_reparent(inv_right_container, object);
        if (&D_00196120 == object->parent)
            object->owner = picked_model_index;
        object->id = object_new_id(0);
        quest_raise_event(5, object, 0);
        trade_schedule_repair();
        break;
    case 4:
        inv_use_item();
        break;
    }
}
