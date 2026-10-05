/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern struct record *wagon_container;
extern struct item *D_00195A80;
extern struct record *player_object;
extern struct record *D_00195AA8;
extern struct record *D_00195AC4;
extern struct record *inv_right_container;
extern int trade_mode;
extern int inventory_action;
extern char D_00195D54[];
extern char D_00196120[];
extern signed char D_001962AE;
extern struct record *inv_selected_item;

extern int object_weight(struct record *);
extern int object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern int trade_can_repair_item(struct item *);
extern void msgbox_show_rsc(int, int);
extern void quest_raise_event(int, int, int);
extern void inv_use_item(void);
extern void inv_item_info(struct record *, struct item *);
extern void inv_equip_item(struct record *);
extern void inv_toggle_hidden(void);
extern void func_000992FA(void);

void inv_click_left_item(struct record *a1)
{
    struct item *l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    {
        int l_3C;

        D_00195AA8 = (inv_selected_item = a1);
        l_34 = &a1->data.item;
        D_00195A80 = l_34;
        l_3C = inventory_action - 1;
        switch (l_3C) {
        case 0:
            inv_item_info(inv_selected_item, l_34);
            return;
        case 1:
            if (trade_mode == 0) goto L94763;
            if (trade_mode != 1) goto L94793;
L94763:;
            if (((int)(unsigned short)(a1->flags & 32)) == 0 || l_34->enchantments[0].type == (-1)) {
                inv_equip_item(inv_selected_item);
            }
            return;
        case 2:
L94793:;
            if (l_34->group == 23 && trade_mode != 2) return;
            if (l_34->group == 23 && (int)inv_right_container == (int)wagon_container) return;
            if ((int)inv_right_container == (int)wagon_container) {
                l_28 = object_weight(a1);
                D_001962AE = 1;
                l_24 = object_weight(wagon_container);
                D_001962AE = 0;
                if ((l_28 + l_24) > 3000) return;
            }
            if (trade_mode == 3 && l_34->enchantments[0].type != (-1)) {
                msgbox_show_rsc(33, 1);
                return;
            }
            if (trade_mode == 4) {
                a1->x = player_object->x;
                a1->y = player_object->y;
                a1->z = player_object->z;
                a1->caster = 0;
                if (a1->image == 0) a1->image = l_34->dropped_image;
                object_reparent(inv_right_container, a1);
                a1->id = object_new_id(0);
                return;
            }
            if (trade_mode == 3 && trade_can_repair_item(l_34) == 0) return;
            a1->x = player_object->x;
            a1->y = player_object->y;
            a1->z = player_object->z;
            a1->caster = 0;
            if (a1->image == 0) a1->image = l_34->dropped_image;
            object_reparent(inv_right_container, a1);
            if (((int)D_00196120) == (int)a1->parent) a1->owner = *(short *)D_00195D54;
            a1->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
            if (a1->twin != 0) a1->twin->id = a1->id;
            quest_raise_event(5, (int)a1, 0);
            func_000992FA();
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
