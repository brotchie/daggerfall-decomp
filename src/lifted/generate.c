/* generate.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char screen_buffer[];
extern char D_00147964[];
extern char D_00176F41[];
extern char D_00190BE4[];
extern char D_00190BE8[];
extern char D_00190CA8[];
extern char itemmaker_slot_kinds[];
extern char D_00190CEE[];
extern char D_00190D64[];
extern char D_00190DEA[];
extern char text_macro_fa[];
extern char D_001940D5[];
extern char D_001940D6[];
extern char D_001940D8[];
extern struct record *inventory_containers[];
extern struct record *wagon_container;
extern struct building *current_building;
extern struct record *D_00195AF4;
extern struct record *inv_right_container;
extern struct record *D_00195B34;
extern char D_00195B5C[];
extern char D_00195B60[];
extern char D_00195B84[];
extern struct character *player_character;
extern char window_image[];
extern struct career *player_class;
extern char game_minutes[];
extern char D_00195D2C[];
extern char D_00195DA8[];
extern char D_00195F34[];
extern char current_region[];
extern char msgbox_kind[];
extern char game_mode[];
extern char mouse_buttons_prev[];
extern char chargen_saved_minimums[];
extern char chargen_saved_attributes[];
extern char chargen_face_images[];
extern char chargen_reflex_image[];
extern char chargen_roll_saved[];
extern char chargen_saved_points[];
extern char chargen_screen[];
extern char inv_left_scroll[];
extern struct record *inv_left_container;

extern int object_weight(struct record *);
extern int holiday_today(int, int);
extern int sound_play_ui(int);
extern int rand_range(int, int);
extern int object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern int inventory_open(int, int, int);
extern int mc_free();
extern int mc_memcpy();
extern int func_0012B136();
extern int func_0012B2D3();
extern int func_00144F68();
extern void msgbox_open_rsc(int, int);
extern void msgbox_update(void);
extern void keys_world_actions(void);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void object_free_later(struct record *);
extern void object_free_children(struct record *);
extern void chargen_draw_face(void);
extern void chargen_draw_attributes(void);
extern void chargen_draw_skills(void);
extern void chargen_select_attribute(int);
extern void inv_store_item(struct record *);
extern void inv_create_wagon(void);
extern void inv_merge_arrows(struct record *, struct record *, int);

int chargen_draw(void)
{
    int l_24;
    int l_20;
    int l_1C;

    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    func_0012B136();
    mc_memcpy(*(int *)screen_buffer, *(int *)window_image, 64000, (int)D_00176F41, 181, 4);
    if (((int)(unsigned char)(*(signed char *)chargen_screen & 2)) == 0) goto L90C8D;
    chargen_draw_face();
L90C8D:;
    if (((int)(unsigned char)(*(signed char *)chargen_screen & 4)) == 0) goto L90CA2;
    chargen_draw_attributes();
L90CA2:;
    if (((int)(unsigned char)(*(signed char *)chargen_screen & 8)) == 0) goto L90CB7;
    chargen_draw_skills();
L90CB7:;
    if (((int)(unsigned char)(*(signed char *)chargen_screen & 16)) == 0) goto L90D46;
    if (((int)(unsigned char)*(signed char *)chargen_screen) != 255) goto L90CE9;
    l_20 = 119;
    l_1C = -53;
    goto L90CF6;
L90CE9:;
    l_1C = 0;
    l_20 = l_1C;
L90CF6:;
    func_00144F68(l_20 + 127, l_1C + ((player_character->reflexes * 9) + 148), 66, 9, (int)(*(char **)chargen_reflex_image + 12 + (player_character->reflexes * 594)));
L90D46:;
    if (((int)(unsigned char)(*(signed char *)chargen_screen & 16)) == 0) goto L90D64;
    if (((int)(unsigned char)*(signed char *)chargen_screen) != 255) goto L90D66;
L90D64:;
    goto L90D72;
L90D66:;
    if (((int)(unsigned char)*(signed char *)msgbox_kind) != 4) goto L90D74;
L90D72:;
    goto L90D93;
L90D74:;
    msgbox_open_rsc(307, 4);
    *(signed char *)D_001940D5 |= 64;
    *(short *)D_00195F34 = 125;
L90D93:;
    if (((int)(unsigned char)*(signed char *)chargen_screen) != 255) goto L90DAD;
    if (((int)(unsigned char)*(signed char *)game_mode) != 8) goto L90DAF;
L90DAD:;
    goto L90DCE;
L90DAF:;
    text_draw_colored((int)player_character, 80, 5, 145, 141);
L90DCE:;
    if (((int)(unsigned char)(*(signed char *)chargen_screen & 1)) != 0) goto L90DE3;
    keys_world_actions();
L90DE3:;
    msgbox_update();
    *(signed char *)D_00147964 &= 254;
    func_0012B2D3((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y);
    if (*(signed char *)chargen_screen == 0) goto L90E14;
    if (*(signed char *)mouse_buttons != 0) goto L90E16;
L90E14:;
    goto L90E24;
L90E16:;
    if (((int)(short)*(short *)mouse_x) > 263) goto L90E26;
L90E24:;
    goto L90E34;
L90E26:;
    if (((int)(short)*(short *)mouse_x) < 301) goto L90E36;
L90E34:;
    goto L90E44;
L90E36:;
    if (((int)(short)*(short *)mouse_y) > 172) goto L90E46;
L90E44:;
    goto L90E54;
L90E46:;
    if (((int)(short)*(short *)mouse_y) < 193) goto L90E56;
L90E54:;
    goto L90E5F;
L90E56:;
    l_24 = 1;
    goto L90E66;
L90E5F:;
    l_24 = 0;
L90E66:;
    return l_24;
}

void chargen_free_images(void)
{
    *(signed char *)chargen_screen = 0;
    if (*(int *)chargen_face_images == 0) goto L90EA3;
    if (*(int *)chargen_face_images != (-1751672937)) goto L90EA5;
L90EA3:;
    goto L90EC3;
L90EA5:;
    mc_free(*(int *)chargen_face_images, (int)D_00176F41, 224);
    *(int *)chargen_face_images = -1751672937;
L90EC3:;
    if (*(int *)D_00195B60 == 0) goto L90ED8;
    if (*(int *)D_00195B60 != (-1751672937)) goto L90EDA;
L90ED8:;
    goto L90EF8;
L90EDA:;
    mc_free(*(int *)D_00195B60, (int)D_00176F41, 225);
    *(int *)D_00195B60 = -1751672937;
L90EF8:;
    if (*(int *)D_00195B5C == 0) goto L90F0D;
    if (*(int *)D_00195B5C != (-1751672937)) goto L90F0F;
L90F0D:;
    goto L90F2D;
L90F0F:;
    mc_free(*(int *)D_00195B5C, (int)D_00176F41, 226);
    *(int *)D_00195B5C = -1751672937;
L90F2D:;
    if (*(int *)chargen_reflex_image == 0) goto L90F42;
    if (*(int *)chargen_reflex_image != (-1751672937)) goto L90F44;
L90F42:;
    goto L90F62;
L90F44:;
    mc_free(*(int *)chargen_reflex_image, (int)D_00176F41, 227);
    *(int *)chargen_reflex_image = -1751672937;
L90F62:;
    if (*(int *)window_image == 0) goto L90F77;
    if (*(int *)window_image != (-1751672937)) goto L90F79;
L90F77:;
    return;
L90F79:;
    mc_free(*(int *)window_image, (int)D_00176F41, 228);
    *(int *)window_image = -1751672937;
}

void chargen_attribute_button(int a1)
{
    chargen_select_attribute((int)(short)(a1 - 20));
}

void chargen_skill_arrow(int a1)
{
    int l_28;
    int l_24;
    struct character_skill *l_20;
    int l_1C;
    int l_18;

    l_1C = 1132;
    if (((unsigned)(*(int *)((char *)l_1C) - *(int *)D_00190BE4)) < 6) return;
    l_18 = 1132;
    *(int *)D_00190BE4 = *(int *)((char *)l_18);
    a1 += -14;
    l_28 = a1 >> 1;
    if (*(short *)(D_00190DEA + (l_28 * 2)) != 0) goto L91878;
    if ((a1 & 1) != 0) goto L9187A;
L91878:;
    goto L9187F;
L9187A:;
    return;
L9187F:;
    l_24 = (int)(short)*(short *)(text_macro_fa + (l_28 * 2));
    l_20 = &player_character->skills[player_class->skills[l_24]];
    if ((unsigned char)l_20->value != *(signed char *)(D_00190CEE + l_24)) goto L918D1;
    if ((a1 & 1) == 0) goto L918D3;
L918D1:;
    goto L918D5;
L918D3:;
    return;
L918D5:;
    if ((a1 & 1) == 0) goto L918F1;
    (*(signed char *)((char *)l_20))++;
    (*(short *)(D_00190DEA + (l_28 * 2)))--;
    return;
L918F1:;
    (*(signed char *)((char *)l_20))--;
    (*(short *)(D_00190DEA + (l_28 * 2)))++;
}

void chargen_face_previous(void)
{
    if (*(signed char *)mouse_buttons_prev != 0) return;
    player_character->face--;
    if (player_character->face <= 10) return;
    player_character->face = 9;
}

void chargen_face_next(void)
{
    if (*(signed char *)mouse_buttons_prev != 0) return;
    player_character->face = (player_character->face + 1) % 10;
}

void chargen_roll_attributes(void)
{
    int l_1C;
    int l_18;

    if (*(signed char *)mouse_buttons_prev != 0) return;
    if (((int)(unsigned char)*(signed char *)chargen_screen) != 255) goto L91AD1;
    *(int *)D_00190BE8 = 1;
    return;
L91AD1:;
    l_1C = 0;
L91AD8:;
    if (l_1C < 8) goto L91AEB;
    goto L91B57;
L91AE3:;
    l_1C++;
    goto L91AD8;
L91AEB:;
    l_18 = player_class->attributes[l_1C];
    *(signed char *)(itemmaker_slot_kinds + l_1C) = (player_character->attributes[l_1C] = rand_range(l_18, l_18 + 10));
    if (player_character->attributes[l_1C] <= 100) goto L91B55;
    player_character->attributes[l_1C] = 100;
L91B55:;
    goto L91AE3;
L91B57:;
    *(short *)D_00190D64 = rand_range(6, 14);
    sound_play_ui(220);
}

void chargen_restore_roll(void)
{
    int l_18;

    if (*(signed char *)mouse_buttons_prev != 0) return;
    if (*(signed char *)chargen_roll_saved == 0) goto L91C76;
    l_18 = 0;
L91C26:;
    if (l_18 < 8) goto L91C36;
    goto L91C69;
L91C2E:;
    l_18++;
    goto L91C26;
L91C36:;
    player_character->attributes[l_18] = *(short *)(chargen_saved_attributes + (l_18 * 2));
    *(signed char *)(itemmaker_slot_kinds + l_18) = *(signed char *)(chargen_saved_minimums + (l_18 * 2));
    goto L91C2E;
L91C69:;
    *(short *)D_00190D64 = (int)(unsigned char)*(signed char *)chargen_saved_points;
L91C76:;
    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    *(signed char *)mouse_buttons = 0;
}

void chargen_save_roll(void)
{
    int l_18;

    if (*(signed char *)mouse_buttons_prev != 0) return;
    *(signed char *)chargen_roll_saved = 1;
    *(signed char *)chargen_saved_points = *(signed char *)D_00190D64;
    l_18 = 0;
L91CC0:;
    if (l_18 < 8) goto L91CD0;
    goto L91D05;
L91CC8:;
    l_18++;
    goto L91CC0;
L91CD0:;
    *(short *)(chargen_saved_attributes + (l_18 * 2)) = player_character->attributes[l_18];
    *(short *)(chargen_saved_minimums + (l_18 * 2)) = (short)*(signed char *)(itemmaker_slot_kinds + l_18);
    goto L91CC8;
L91D05:;
    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    *(signed char *)mouse_buttons = 0;
}

int inv_match_arrows(struct record *a1)
{
    int l_1C;

    if (a1->type == 2) goto L91D49;
    return 0;
L91D49:;
    if (a1->image2 != 998) goto L91D66;
    if (a1->image == 0) goto L91D68;
L91D66:;
    goto L91D79;
L91D68:;
    D_00195AF4 = a1;
    return 1;
L91D79:;
    return 0;
}

void inv_sum_hidden_weight(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (((int)(unsigned short)(l_18->item_flags & 64)) == 0) return;
    *(int *)D_00195B84 += l_18->weight;
}

void trade_add_buy_price(struct record *a1)
{
    struct item *l_1C;
    int l_18;

    if (a1 == 0) return;
    if (a1->type != 2) return;
    l_1C = &a1->data.item;
    if (a1->image2 != 998) goto L91E3C;
    if (l_1C->group == 3) goto L91E3E;
L91E3C:;
    goto L91E4F;
L91E3E:;
    if (l_1C->index == 18) goto L91E51;
L91E4F:;
    goto L91E5B;
L91E51:;
    if (l_1C->condition == 0) goto L91E5D;
L91E5B:;
    goto L91E62;
L91E5D:;
    return;
L91E62:;
    *(int *)D_00190CA8 += object_weight(a1);
    l_18 = l_1C->value;
    if (a1->image2 != 998) goto L91EA1;
    l_18 = l_1C->value * l_1C->condition;
L91EA1:;
    if (a1->image2 == 998) goto L91EC9;
    if (((int)(unsigned short)(a1->flags & 32)) == 0) goto L91ECB;
L91EC9:;
    goto L91ED0;
L91ECB:;
    return;
L91ED0:;
    if (current_building->type != 13) goto L91EF9;
    if (holiday_today(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region) == 49) goto L91EFB;
L91EF9:;
    goto L91F0C;
L91EFB:;
    if (l_1C->group == 3) goto L91F0E;
L91F0C:;
    goto L91F1B;
L91F0E:;
    *(int *)D_00195D2C += l_18 >> 1;
    return;
L91F1B:;
    if (current_building->type != 9) goto L91F44;
    if (holiday_today(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region) == 29) goto L91F46;
L91F44:;
    goto L91F53;
L91F46:;
    *(int *)D_00195D2C += l_18 >> 1;
    return;
L91F53:;
    if (((struct bf8_7_1 *)&D_001940D6)->f == 0) goto L91F69;
    *(int *)D_00195D2C += l_18 >> 1;
    return;
L91F69:;
    *(int *)D_00195D2C += l_18;
}

void trade_add_repair_cost(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) goto L91FB1;
    if (((int)(unsigned short)(a1->flags & 512)) == 0) goto L91FB3;
L91FB1:;
    return;
L91FB3:;
    l_18 = &a1->data.item;
    if (l_18->enchantments[0].type != (-1)) goto L91FE0;
    *(int *)D_00195D2C += ((unsigned)(l_18->value * 10)) / 100;
    goto L91FF6;
L91FE0:;
    *(int *)D_00195D2C += ((unsigned)(l_18->value * 75)) / 100;
L91FF6:;
    if (*(int *)D_00195D2C >= 1) return;
    *(int *)D_00195D2C = 1;
}

void inv_return_unpaid_item(struct record *a1)
{
    int l_20;
    int l_1C;
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (a1->image2 != 998) goto L92064;
    if (l_18->group == 3) goto L92066;
L92064:;
    goto L92077;
L92066:;
    if (l_18->index == 18) goto L92079;
L92077:;
    goto L92083;
L92079:;
    if (l_18->condition == 0) goto L92085;
L92083:;
    goto L9208A;
L92085:;
    return;
L9208A:;
    if (a1->image2 != 998) goto L920AE;
    if (l_18->group == 3) goto L920B0;
L920AE:;
    goto L920C1;
L920B0:;
    if (l_18->index == 18) goto L920C3;
L920C1:;
    goto L9211D;
L920C3:;
    l_1C = l_18->stack_count - l_18->condition;
    l_18->stack_count = (signed char)l_18->condition;
    inv_merge_arrows(inv_right_container, a1, 0);
    l_18->condition = 0;
    l_18->stack_count = *(signed char *)&l_1C;
    if (l_1C != 0) goto L92118;
    object_free_later(a1);
L92118:;
    return;
L9211D:;
    if (((int)(unsigned short)(a1->flags & 32)) == 0) return;
    object_reparent(inv_right_container, a1);
    l_20 = 0;
L92146:;
    if (l_20 < 27) goto L92156;
    goto L92189;
L9214E:;
    l_20++;
    goto L92146;
L92156:;
    if (player_character->equipped[l_20] != a1) goto L92187;
    player_character->equipped[l_20] = 0;
L92187:;
    goto L9214E;
L92189:;
    *(signed char *)D_001940D8 |= 8;
}

void inv_store_callback(struct record *a1)
{
    if (a1->type != 2) return;
    inv_store_item(a1);
}

void inv_claim_item(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (a1->image2 != 998) goto L92215;
    l_18->condition = 0;
L92215:;
    if (((int)(unsigned short)(a1->flags & 32)) == 0) return;
    a1->flags &= ~0x20;
    if (a1->parent != wagon_container) goto L9224C;
    if (inv_left_container == wagon_container) goto L92254;
L9224C:;
    inv_store_item(a1);
L92254:;
    if (l_18->group != 23) goto L9226F;
    if (l_18->index == 0) goto L92271;
L9226F:;
    return;
L92271:;
    inv_create_wagon();
}

void inv_assign_item_id(struct record *a1)
{
    if (a1->type != 2) return;
    if ((((unsigned)a1->id) >> 16) == 100) return;
    a1->id = object_new_id(100);
}

void inv_reset_left_list(void)
{
    *(int *)inv_left_scroll = 0;
    inv_left_container = inventory_containers[0];
}

void inventory_open_container(struct record *a1, int a2, int a3)
{
    *(int *)D_00195DA8 = (int)a1;
    inv_right_container = (D_00195B34 = a1);
    if (inventory_open(2, a2, a3) != 0) return;
    object_free_children(a1);
}
