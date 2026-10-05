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
extern char trade_mode[];
extern char inventory_action[];
extern char D_00195D54[];
extern char D_00196120[];
extern char D_001962AE[];
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

__dagger_tbl946E8:;
    D_00195AA8 = (inv_selected_item = a1);
    l_34 = &a1->data.item;
    D_00195A80 = l_34;
    l_3C = *(int *)inventory_action - 1;
    switch (l_3C) {
case 0:
    inv_item_info(inv_selected_item, l_34);
    return;
case 1:
    if (*(int *)trade_mode == 0) goto L94763;
    if (*(int *)trade_mode != 1) goto L94793;
L94763:;
    if (((int)(unsigned short)(a1->flags & 32)) == 0) goto L94784;
    if (l_34->enchantments[0].type != (-1)) goto L9478E;
L94784:;
    inv_equip_item(inv_selected_item);
L9478E:;
    return;
case 2:
L94793:;
    if (l_34->group != 23) goto L947AD;
    if (*(int *)trade_mode != 2) goto L947AF;
L947AD:;
    goto L947B4;
L947AF:;
    return;
L947B4:;
    if (l_34->group != 23) goto L947D2;
    if ((int)inv_right_container == (int)wagon_container) goto L947D4;
L947D2:;
    goto L947D9;
L947D4:;
    return;
L947D9:;
    if ((int)inv_right_container != (int)wagon_container) goto L9481D;
    l_28 = object_weight(a1);
    *(signed char *)D_001962AE = 1;
    l_24 = object_weight(wagon_container);
    *(signed char *)D_001962AE = 0;
    if ((l_28 + l_24) > 3000) return;
L9481D:;
    if (*(int *)trade_mode != 3) goto L94832;
    if (l_34->enchantments[0].type != (-1)) goto L94834;
L94832:;
    goto L94848;
L94834:;
    msgbox_show_rsc(33, 1);
    return;
L94848:;
    if (*(int *)trade_mode != 4) goto L948BE;
    a1->x = player_object->x;
    a1->y = player_object->y;
    a1->z = player_object->z;
    a1->caster = 0;
    if (a1->image != 0) goto L9489D;
    a1->image = l_34->dropped_image;
L9489D:;
    object_reparent(inv_right_container, a1);
    a1->id = object_new_id(0);
    return;
L948BE:;
    if (*(int *)trade_mode != 3) goto L948D3;
    if (trade_can_repair_item(l_34) == 0) goto L948D5;
L948D3:;
    goto L948DA;
L948D5:;
    return;
L948DA:;
    a1->x = player_object->x;
    a1->y = player_object->y;
    a1->z = player_object->z;
    a1->caster = 0;
    if (a1->image != 0) goto L94926;
    a1->image = l_34->dropped_image;
L94926:;
    object_reparent(inv_right_container, a1);
    if (((int)D_00196120) != (int)a1->parent) goto L9494E;
    a1->owner = *(short *)D_00195D54;
L9494E:;
    a1->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    if (a1->twin == 0) goto L9497E;
    a1->twin->id = a1->id;
L9497E:;
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
