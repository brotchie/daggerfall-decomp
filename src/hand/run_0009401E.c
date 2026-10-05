/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00093F0C to 0x0009401E, kept together for its switch table's alignment */
struct obj {
    char pad0[7];
    int x;                  /* 0x07 */
    int y;                  /* 0x0b */
    int z;                  /* 0x0f */
    char pad13[0x17 - 0x13];
    short f17;
    char pad19[0x1b - 0x19];
    short id;               /* 0x1b */
    char pad1d[0x1f - 0x1d];
    int f1f;
    char pad23[0x26 - 0x23];
    char f26;
    char pad27[0x2f - 0x27];
    int f2f;
    char pad33[0x43 - 0x33];
    char *f43;
    char pad47[0x34];
    short f7b;              /* 0x47 + 52 */
};
struct pc { char pad[367]; struct obj *slots[1]; };
extern char *text_macro_item;
extern struct obj *player_object;
extern struct obj *scratch_current_object;
extern int inv_right_container;
extern struct pc *player_character;
extern int trade_mode;
extern int inventory_action;
extern short picked_model_index;
extern char D_00196120[];
extern struct obj *inv_selected_item;
extern void quest_raise_event(int, struct obj *, int);
extern int object_reparent(int, struct obj *);
extern int object_new_id(int);
extern int inv_take_item(struct obj *);
extern void inv_use_item(void);
extern void inv_item_info(struct obj *, char *);
extern void item_remove_equip_effects(struct obj *, int);
extern int trade_can_repair_item(char *);
extern void trade_schedule_repair(void);
extern char *inventory_containers[];
extern unsigned char inv_right_icon;
extern int inv_left_scroll;
extern int inv_right_scroll;
extern char *inv_left_container;
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
    char *item;
    int unused1;
    int unused2;
    struct obj *object;

    object = inv_selected_item = player_character->slots[slot];
    if (object == 0)
        return;
    scratch_current_object = inv_selected_item;
    item = (char *)inv_selected_item + 71;
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
            player_character->slots[slot] = 0;
            object->x = player_object->x;
            object->y = player_object->y;
            object->z = player_object->z;
            object->f2f = 0;
            if (object->id == 0)
                object->id = *(short *)(item + 52);
            object_reparent(inv_right_container, object);
            object->f1f = object_new_id(0);
            break;
        }
        if (trade_mode == 3 && trade_can_repair_item(item) == 0)
            break;
        item_remove_equip_effects(inv_selected_item, slot);
        player_character->slots[slot] = 0;
        object->x = player_object->x;
        object->y = player_object->y;
        object->z = player_object->z;
        object->f2f = 0;
        if (object->id == 0)
            object->id = *(short *)(item + 52);
        if (object->f26 == 0 && trade_mode == 0)
            object_reparent(inv_right_container, object);
        if (D_00196120 == object->f43)
            object->f17 = picked_model_index;
        object->f1f = object_new_id(0);
        quest_raise_event(5, object, 0);
        trade_schedule_repair();
        break;
    case 4:
        inv_use_item();
        break;
    }
}
