/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *wagon_container;
extern struct item *text_macro_item;
extern struct record *player_object;
extern struct record *scratch_current_object;
extern struct record *location_object;
extern struct record *inv_right_container;
extern int trade_mode;
extern int inventory_action;
extern char picked_model_index[];
extern char D_00196120[];
extern signed char D_001962AE;
extern struct record *inv_selected_item;

extern int object_weight(struct record *);
extern struct record *object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern int trade_can_repair_item(struct item *);
extern void msgbox_show_rsc(int, int);
extern void quest_raise_event(short, struct record *, struct record *);
extern void inv_use_item(void);
extern void inv_item_info(struct record *, struct item *);
extern void inv_equip_item(struct record *);
extern void inv_toggle_hidden(void);
extern void trade_schedule_repair(void);

void inv_click_left_item(struct record *object)
{
    struct item *item;
    int unused1;
    int unused2;
    int weight;
    int wagon_weight;
    int unused3;
    int unused4;
    int unused5;
    {
        int action;

        scratch_current_object = (inv_selected_item = object);
        item = &object->data.item;
        text_macro_item = item;
        action = inventory_action - 1;
        switch (action) {
        case 0:
            inv_item_info(inv_selected_item, item);
            return;
        case 1:
            if (trade_mode != 0) if (trade_mode != 1) goto L94793;
            if (((int)(unsigned short)(object->flags & 32)) == 0 || item->enchantments[0].type == (-1)) {
                inv_equip_item(inv_selected_item);
            }
            return;
        case 2:
L94793:;
            if (item->group == 23 && trade_mode != 2) return;
            if (item->group == 23 && (int)inv_right_container == (int)wagon_container) return;
            if ((int)inv_right_container == (int)wagon_container) {
                weight = object_weight(object);
                D_001962AE = 1;
                wagon_weight = object_weight(wagon_container);
                D_001962AE = 0;
                if ((weight + wagon_weight) > 3000) return;
            }
            if (trade_mode == 3 && item->enchantments[0].type != (-1)) {
                msgbox_show_rsc(33, 1);
                return;
            }
            if (trade_mode == 4) {
                object->x = player_object->x;
                object->y = player_object->y;
                object->z = player_object->z;
                object->caster = 0;
                if (object->image == 0) object->image = item->dropped_image;
                object_reparent(inv_right_container, object);
                object->id = object_new_id(0);
                return;
            }
            if (trade_mode == 3 && trade_can_repair_item(item) == 0) return;
            object->x = player_object->x;
            object->y = player_object->y;
            object->z = player_object->z;
            object->caster = 0;
            if (object->image == 0) object->image = item->dropped_image;
            object_reparent(inv_right_container, object);
            if (((int)D_00196120) == (int)object->parent) object->owner = *(short *)picked_model_index;
            object->id = object_new_id(((unsigned)location_object->id) >> 16);
            if (object->twin != 0) object->twin->id = object->id;
            quest_raise_event(5, object, 0);
            trade_schedule_repair();
            return;
        case 3:
            inv_use_item();
            return;
        case 4:
            inv_toggle_hidden();
        default:;
        }
    }
}
