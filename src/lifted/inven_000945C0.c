/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern int game_minutes;
extern int inventory_action;
extern struct record *inv_selected_item;

extern int inv_take_item(struct record *);
extern void inv_use_item(void);
extern void inv_item_info(struct record *, struct item *);
extern void inv_equip_item(struct record *);

void inv_click_right_item(struct record *object)
{
    struct item *item;
    {
        int action;

        inv_selected_item = object;
        item = &object->data.item;
        action = inventory_action - 1;
        switch (action) {
        case 0:
            inv_item_info(inv_selected_item, item);
            return;
        case 1:
            if (inv_selected_item->type == 54 && ((unsigned)game_minutes) < object->repair_due) {
                return;
            }
            if (((int)(unsigned short)(object->flags & 32)) != 0 && item->enchantments[0].type != (-1)) {
                inv_take_item(inv_selected_item);
            } else if (inv_take_item(inv_selected_item) != 0) {
                inv_equip_item(inv_selected_item);
            }
            return;
        case 2:
            if (inv_selected_item->type == 54 && ((unsigned)game_minutes) < object->repair_due) {
                return;
            }
            inv_take_item(inv_selected_item);
            return;
        case 3:
            inv_use_item();
        default:;
        }
    }
}
