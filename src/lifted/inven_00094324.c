/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern short mouse_x;
extern short mouse_y;
extern signed char D_001940D8;
extern struct item *text_macro_item;
extern struct record *player_object;
extern struct record *scratch_current_object;
extern struct record *location_object;
extern struct record *inv_right_container;
extern struct character *player_character;
extern int trade_mode;
extern int inventory_action;
extern char picked_model_index[];
extern char D_00196120[];
extern struct record *inv_selected_item;

extern struct record *object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern int inv_take_item(struct record *);
extern int inv_paperdoll_slot_at(int, int, int);
extern int trade_can_repair_item(struct item *);
extern void msgbox_show_rsc(int, int);
extern void quest_raise_event(short, struct record *, struct record *);
extern void inv_use_item(void);
extern void inv_item_info(struct record *, struct item *);
extern void item_remove_equip_effects(struct record *, int);
extern void trade_schedule_repair(void);

void inv_click_paperdoll(void)
{
    int action;
    struct item *item;
    int unused1;
    int unused2;
    int slot;
    struct record *object;

    slot = inv_paperdoll_slot_at((int)(short)mouse_x, (int)(short)mouse_y, slot);
    if (slot == 0) return;
    D_001940D8 |= 8;
    slot += -64;
    object = (inv_selected_item = player_character->equipped[slot]);
    scratch_current_object = inv_selected_item;
    item = &inv_selected_item->data.item;
    text_macro_item = item;
    action = inventory_action - 1;
    switch (action) {
    case 0:
        inv_item_info(inv_selected_item, item);
        return;
    case 1:
        inv_take_item(inv_selected_item);
        return;
    case 2:
        if (trade_mode == 4) {
            item_remove_equip_effects(inv_selected_item, slot);
            player_character->equipped[slot] = 0;
            object->x = player_object->x;
            object->y = player_object->y;
            object->z = player_object->z;
            object->caster = 0;
            if (object->image == 0) object->image = item->dropped_image;
            object_reparent(inv_right_container, object);
            object->id = object_new_id(0);
            return;
        }
        if (trade_mode == 3 && item->enchantments[0].type != (-1)) {
            msgbox_show_rsc(33, 1);
            return;
        }
        if (trade_mode == 3 && trade_can_repair_item(item) == 0) return;
        item_remove_equip_effects(inv_selected_item, slot);
        player_character->equipped[slot] = 0;
        object->x = player_object->x;
        object->y = player_object->y;
        object->z = player_object->z;
        object->caster = 0;
        if (object->image == 0) object->image = item->dropped_image;
        object_reparent(inv_right_container, object);
        if (((int)D_00196120) == (int)object->parent) object->owner = *(short *)picked_model_index;
        if (object->quest_id == 0 && trade_mode == 0) {
            object->id = object_new_id(((unsigned)location_object->id) >> 16);
        }
        quest_raise_event(5, object, 0);
        trade_schedule_repair();
        return;
    case 3:
        inv_use_item();
    default:;
    }
}
