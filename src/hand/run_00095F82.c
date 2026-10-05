/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00095D2C to 0x00095F82, kept together for its switch table's alignment */
struct node {
    unsigned char type;
    char pad1[54];
    struct node *next;      /* 0x37 */
    char pad3b[4];
    struct node *child;     /* 0x3f */
};
struct item {
    char pad0[0x20];
    unsigned short type;    /* 0x20 */
    unsigned short sub;     /* 0x22 */
    int value;              /* 0x24 */
    char pad28[4];
    short count;            /* 0x2c */
    char pad2e[0x37 - 0x2e];
    unsigned char f37;
};
struct pc {
    char pad0[0x40];
    unsigned short flags;   /* 0x40 */
    char pad42[0x16f - 0x42];
    char *slots[19];        /* 0x16f */
    char *f1bb;
    char pad1bf[4];
    char *f1c3;
};
struct flags8 { unsigned char b0:2; unsigned char b2:1; };
extern char D_0012B508;
extern char D_0017704C[];       /* __FILE__ */
extern char *D_001832A4;
extern unsigned char D_001860DA[];
extern unsigned char D_00186104[];
extern short D_00188208[];
extern signed char text_buffer[];
extern unsigned char D_001940D8;
extern struct node *wagon_container;
extern int player_object;
extern struct node *inv_right_container;
extern struct node *inv_right_container_base;
extern struct pc *player_character;
extern unsigned char D_0019626F;
extern unsigned char game_mode;
extern unsigned char inv_right_icon;
extern int D_001AA454;
extern char inv_right_rows[];
extern struct node *D_001AA558;
extern char *inv_selected_item;
extern char inv_left_rows[];
extern struct node *D_001AA578;
extern struct node *inv_left_container;
extern short inv_right_count;
extern short D_001AA586;
extern short D_001AA588;
extern short inv_left_count;
extern void msgbox_show_string(char *, int);
extern void msgbox_show_rsc(int, int);
extern int sound_play(int, int, int);
extern void gold_add(int);
extern int object_free_single(char *);
extern void inv_draw_container_icon(int, unsigned char);
extern int inv_draw_item_cell(struct node *, short, char *);
extern void inv_list_left_item(struct node *, char *);
extern void inv_list_right_item(struct node *, char *);
extern void inv_equip_in_slot_pair(char *, int, int);
extern void inv_unequip_slot(int);
extern void inv_equip_in_slot(char *, int);
extern int item_is_two_handed(char *);
extern void item_remove_equip_effects(char *, int);
extern void inv_store_item(char *);
extern int item_forbidden_for_class(struct item *);
extern void mc_memset(char *, int, int, char *, int, int);
extern int xn_str_find_u32();
#pragma aux mc_set_location parm routine [];
extern int mc_set_location(int, char *);
extern int mc_sprintf(char *, char *, ...);
void inv_equip_item(char *obj);

void inv_draw_left_list(char *a1)
{
    struct node *n;

    inv_left_count = D_001AA586 = 0;
    mc_memset(inv_left_rows, 0, 20, D_0017704C, 1627, 20);
    if (inv_left_container->type == 2 && inv_left_container != wagon_container) {
        inv_draw_item_cell(inv_left_container, 0, a1);
        D_001AA578 = inv_left_container;
    } else if (inv_left_container == wagon_container) {
        inv_draw_container_icon(27, inv_right_icon);
    } else if (game_mode != 4 && D_0019626F != 4 && !((struct flags8 *)&D_001940D8)->b2) {
        return;
    }
    n = inv_left_container->child;
    while (n != 0) {
        inv_list_left_item(n, a1 + 12);
        n = n->next;
    }
    inv_left_count = D_001AA586;
}

void inv_draw_right_list(char *a1)
{
    struct node *n;

    inv_right_count = D_001AA588 = 0;
    mc_memset(inv_right_rows, 0, 20, D_0017704C, 1655, 20);
    if (inv_right_container != inv_right_container_base) {
        inv_draw_item_cell(inv_right_container, 0, a1);
        D_001AA558 = inv_right_container;
    }
    n = inv_right_container->child;
    while (n != 0) {
        inv_list_right_item(n, a1 + 12);
        n = n->next;
    }
    inv_right_count = D_001AA588;
}

void func_00095EDB(void)
{
    int l_1C;
    int l_18;

    D_001AA454 = 0;
    if (xn_str_find_u32(player_character->slots, inv_selected_item, 27) != 0)
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

void inv_equip_item(char *obj)
{
    struct item *it;
    unsigned char *tbl;
    int unused;

    it = (struct item *)(obj + 71);
    if (it->type == 3 && it->sub == 18)
        return;
    if (it->type == 15 || it->type == 16 || it->type == 17 || it->type == 18 ||
        it->type == 19 || it->type == 20 || it->type == 21 || it->type == 22)
        return;
    if (it->count == 0) {
        msgbox_show_rsc(29, 1);
        return;
    }
    D_001940D8 |= 8;
    switch (it->type) {
    case 28:
        switch (it->sub) {
        case 0:
            if (game_mode == 4)
                sound_play(204, player_object, 100);
            D_001AA454 += it->value;
            object_free_single(obj);
            break;
        }
        break;
    case 2:
        if (item_forbidden_for_class(it) != 0)
            return;
        switch (it->sub) {
        case 0:
            if (game_mode == 4)
                sound_play(it->f37 + 231, player_object, 100);
            inv_equip_in_slot(obj, 18);
            break;
        case 1:
            if (game_mode == 4)
                sound_play(233, player_object, 100);
            inv_equip_in_slot(obj, 20);
            break;
        case 2:
            if (game_mode == 4)
                sound_play(it->f37 + 231, player_object, 100);
            inv_equip_in_slot(obj, 23);
            break;
        case 3:
            if (game_mode == 4)
                sound_play(it->f37 + 231, player_object, 100);
            inv_equip_in_slot(obj, 15);
            break;
        case 4:
            if (game_mode == 4)
                sound_play(it->f37 + 231, player_object, 100);
            inv_equip_in_slot(obj, 13);
            break;
        case 5:
            if (game_mode == 4)
                sound_play(233, player_object, 100);
            inv_equip_in_slot(obj, 12);
            break;
        case 6:
            if (game_mode == 4)
                sound_play(it->f37 + 231, player_object, 100);
            inv_equip_in_slot(obj, 26);
            break;
        case 7:
        case 8:
        case 9:
        case 10:
            if (game_mode == 4)
                sound_play(233, player_object, 100);
            if (item_is_two_handed(player_character->f1bb) != 0) {
                item_remove_equip_effects(player_character->f1bb, 19);
                player_character->f1bb = 0;
                inv_equip_in_slot(obj, 21);
                return;
            }
            inv_equip_in_slot(obj, 21);
            break;
        }
        break;
    case 3:
        if (item_forbidden_for_class(it) != 0)
            return;
        if (game_mode == 4)
            sound_play(D_00188208[it->sub], player_object, 100);
        if (item_is_two_handed(obj) != 0) {
            if (item_is_two_handed(player_character->f1bb) != 0) {
                inv_equip_in_slot(obj, 19);
                return;
            }
            if (player_character->f1bb == 0 && player_character->f1c3 == 0) {
                inv_equip_in_slot(obj, 19);
                return;
            }
            if (player_character->f1bb != 0) {
                if (player_character->f1c3 != 0)
                    inv_unequip_slot(21);
                inv_equip_in_slot(obj, 19);
                return;
            }
            if (player_character->f1c3 != 0) {
                inv_unequip_slot(21);
                inv_equip_in_slot(obj, 19);
                return;
            }
        } else {
            if (item_is_two_handed(player_character->f1bb) != 0) {
                inv_equip_in_slot(obj, 19);
                return;
            }
            inv_equip_in_slot_pair(obj, 19, 2);
        }
        break;
    case 6:
    case 12:
        if (it->type == 6 && (player_character->flags & 1))
            return;
        if (it->type == 12 && !(player_character->flags & 1))
            return;
        if (game_mode == 4)
            sound_play(234, player_object, 100);
        if (it->type == 12)
            tbl = D_00186104;
        else
            tbl = D_001860DA;
        switch (tbl[it->sub]) {
        case 12:
            inv_equip_in_slot(obj, 12);
            break;
        case 13:
            inv_equip_in_slot_pair(obj, 14, 2);
            break;
        case 17:
            inv_equip_in_slot(obj, 17);
            break;
        case 22:
            inv_equip_in_slot(obj, 24);
            break;
        case 26:
            inv_equip_in_slot(obj, 26);
            break;
        }
        break;
    case 14:
        if (game_mode == 4)
            sound_play(236, player_object, 100);
        inv_equip_in_slot_pair(obj, 10, 1);
        break;
    case 25:
        if (game_mode == 4)
            sound_play(236, player_object, 100);
        switch (it->sub) {
        case 0:
            inv_equip_in_slot_pair(obj, 0, 1);
            break;
        case 1:
            inv_equip_in_slot_pair(obj, 6, 1);
            break;
        case 2:
            inv_equip_in_slot_pair(obj, 4, 1);
            break;
        case 3:
            inv_equip_in_slot_pair(obj, 2, 1);
            break;
        case 4:
            inv_equip_in_slot_pair(obj, 8, 1);
            break;
        case 5:
            inv_equip_in_slot_pair(obj, 0, 1);
            break;
        case 6:
            inv_equip_in_slot_pair(obj, 2, 1);
            break;
        }
        break;
    }
}
