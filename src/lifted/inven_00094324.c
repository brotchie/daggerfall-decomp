/* inven.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char mouse_x[];
extern char mouse_y[];
extern char D_001940D8[];
extern struct item *D_00195A80;
extern struct record *player_object;
extern struct record *D_00195AA8;
extern struct record *D_00195AC4;
extern struct record *inv_right_container;
extern struct character *player_character;
extern char trade_mode[];
extern char inventory_action[];
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
extern void func_000992FA(void);

void inv_click_paperdoll(void)
{
    int l_2C;
    struct item *l_28;
    int l_24;
    int l_20;
    int l_1C;
    struct record *l_18;

    l_1C = inv_paperdoll_slot_at((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y, l_1C);
    if (l_1C != 0) goto L94368;
    return;
__dagger_tbl94358:;
L94368:;
    *(signed char *)D_001940D8 |= 8;
    l_1C += -64;
    l_18 = (inv_selected_item = player_character->equipped[l_1C]);
    D_00195AA8 = inv_selected_item;
    l_28 = &inv_selected_item->data.item;
    D_00195A80 = l_28;
    l_2C = *(int *)inventory_action - 1;
    switch (l_2C) {
case 0:
    inv_item_info(inv_selected_item, l_28);
    return;
case 1:
    inv_take_item(inv_selected_item);
    return;
case 2:
    if (*(int *)trade_mode != 4) goto L94490;
    item_remove_equip_effects(inv_selected_item, l_1C);
    player_character->equipped[l_1C] = 0;
    l_18->x = player_object->x;
    l_18->y = player_object->y;
    l_18->z = player_object->z;
    l_18->caster = 0;
    if (l_18->image != 0) goto L9446F;
    l_18->image = l_28->dropped_image;
L9446F:;
    object_reparent(inv_right_container, l_18);
    l_18->id = object_new_id(0);
    return;
L94490:;
    if (*(int *)trade_mode != 3) goto L944A5;
    if (l_28->enchantments[0].type != (-1)) goto L944A7;
L944A5:;
    goto L944BB;
L944A7:;
    msgbox_show_rsc(33, 1);
    return;
L944BB:;
    if (*(int *)trade_mode != 3) goto L944D0;
    if (trade_can_repair_item(l_28) == 0) goto L944D2;
L944D0:;
    goto L944D7;
L944D2:;
    return;
L944D7:;
    item_remove_equip_effects(inv_selected_item, l_1C);
    player_character->equipped[l_1C] = 0;
    l_18->x = player_object->x;
    l_18->y = player_object->y;
    l_18->z = player_object->z;
    l_18->caster = 0;
    if (l_18->image != 0) goto L94547;
    l_18->image = l_28->dropped_image;
L94547:;
    object_reparent(inv_right_container, l_18);
    if (((int)D_00196120) != (int)l_18->parent) goto L9456F;
    l_18->owner = *(short *)D_00195D54;
L9456F:;
    if (l_18->quest_id != 0) goto L94581;
    if (*(int *)trade_mode == 0) goto L94583;
L94581:;
    goto L9459B;
L94583:;
    l_18->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
L9459B:;
    quest_raise_event(5, (int)l_18, 0);
    func_000992FA();
    return;
case 3:
    inv_use_item();
default:;
}
}
