/* generate.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern int screen_buffer;
extern signed char D_00147964;
extern char D_00176F41[];
extern char D_00190BE4[];
extern int D_00190BE8;
extern int D_00190CA8;
extern signed char itemmaker_slot_kinds[];
extern signed char D_00190CEE[];
extern char D_00190D64[];
extern short D_00190DEA[];
extern char text_macro_fa[];
extern signed char D_001940D5;
extern signed char D_001940D6;
extern signed char D_001940D8;
extern struct record *inventory_containers[];
extern struct record *wagon_container;
extern struct building *current_building;
extern struct record *D_00195AF4;
extern struct record *inv_right_container;
extern struct record *D_00195B34;
extern int D_00195B5C;
extern int D_00195B60;
extern char D_00195B84[];
extern struct character *player_character;
extern int window_image;
extern struct career *player_class;
extern int game_minutes;
extern int D_00195D2C;
extern char *D_00195DA8;
extern short D_00195F34;
extern signed char current_region;
extern signed char msgbox_kind;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern char chargen_saved_minimums[];
extern short chargen_saved_attributes[];
extern int chargen_face_images;
extern int chargen_reflex_image;
extern signed char chargen_roll_saved;
extern signed char chargen_saved_points;
extern signed char chargen_screen;
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

    mouse_buttons_prev = mouse_buttons;
    func_0012B136();
    mc_memcpy(screen_buffer, window_image, 64000, (int)D_00176F41, 181, 4);
    if (((int)(unsigned char)(chargen_screen & 2)) != 0) chargen_draw_face();
    if (((int)(unsigned char)(chargen_screen & 4)) != 0) chargen_draw_attributes();
    if (((int)(unsigned char)(chargen_screen & 8)) != 0) chargen_draw_skills();
    if (((int)(unsigned char)(chargen_screen & 16)) != 0) {
        if (((int)(unsigned char)chargen_screen) == 255) {
            l_20 = 119;
            l_1C = -53;
        } else {
            l_1C = 0;
            l_20 = l_1C;
        }
        func_00144F68(l_20 + 127, l_1C + ((player_character->reflexes * 9) + 148), 66, 9, (int)(*(char **)&chargen_reflex_image + 12 + (player_character->reflexes * 594)));
    }
    if (((int)(unsigned char)(chargen_screen & 16)) != 0 && ((int)(unsigned char)chargen_screen) != 255 && ((int)(unsigned char)msgbox_kind) != 4) {
        msgbox_open_rsc(307, 4);
        D_001940D5 |= 64;
        D_00195F34 = 125;
    }
    if (((int)(unsigned char)chargen_screen) == 255 && ((int)(unsigned char)game_mode) != 8) {
        text_draw_colored((int)player_character, 80, 5, 145, 141);
    }
    if (((int)(unsigned char)(chargen_screen & 1)) == 0) keys_world_actions();
    msgbox_update();
    D_00147964 &= 254;
    func_0012B2D3((int)(short)mouse_x, (int)(short)mouse_y);
    if (chargen_screen != 0 && mouse_buttons != 0 && ((int)(short)mouse_x) > 263 && ((int)(short)mouse_x) < 301 && ((int)(short)mouse_y) > 172 && ((int)(short)mouse_y) < 193) {
        l_24 = 1;
    } else {
        l_24 = 0;
    }
    return l_24;
}

void chargen_free_images(void)
{
    chargen_screen = 0;
    if (chargen_face_images != 0 && chargen_face_images != (-1751672937)) {
        mc_free(chargen_face_images, (int)D_00176F41, 224);
        chargen_face_images = -1751672937;
    }
    if (D_00195B60 != 0 && D_00195B60 != (-1751672937)) {
        mc_free(D_00195B60, (int)D_00176F41, 225);
        D_00195B60 = -1751672937;
    }
    if (D_00195B5C != 0 && D_00195B5C != (-1751672937)) {
        mc_free(D_00195B5C, (int)D_00176F41, 226);
        D_00195B5C = -1751672937;
    }
    if (chargen_reflex_image != 0 && chargen_reflex_image != (-1751672937)) {
        mc_free(chargen_reflex_image, (int)D_00176F41, 227);
        chargen_reflex_image = -1751672937;
    }
    if (window_image == 0 || window_image == (-1751672937)) return;
    mc_free(window_image, (int)D_00176F41, 228);
    window_image = -1751672937;
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
    if (D_00190DEA[l_28] == 0 && (a1 & 1) != 0) return;
    l_24 = (int)(short)*(short *)(text_macro_fa + (l_28 * 2));
    l_20 = &player_character->skills[player_class->skills[l_24]];
    if ((unsigned char)l_20->value == D_00190CEE[l_24] && (a1 & 1) == 0) return;
    if ((a1 & 1) != 0) {
        (*(signed char *)((char *)l_20))++;
        (D_00190DEA[l_28])--;
        return;
    }
    (*(signed char *)((char *)l_20))--;
    (D_00190DEA[l_28])++;
}

void chargen_face_previous(void)
{
    if (mouse_buttons_prev != 0) return;
    player_character->face--;
    if (player_character->face <= 10) return;
    player_character->face = 9;
}

void chargen_face_next(void)
{
    if (mouse_buttons_prev != 0) return;
    player_character->face = (player_character->face + 1) % 10;
}

void chargen_roll_attributes(void)
{
    int l_1C;
    int l_18;

    if (mouse_buttons_prev != 0) return;
    if (((int)(unsigned char)chargen_screen) == 255) {
        D_00190BE8 = 1;
        return;
    }
    for (l_1C = 0; l_1C < 8; l_1C++) {
        l_18 = player_class->attributes[l_1C];
        itemmaker_slot_kinds[l_1C] = (player_character->attributes[l_1C] = rand_range(l_18, l_18 + 10));
        if (player_character->attributes[l_1C] > 100) player_character->attributes[l_1C] = 100;
    }
    *(short *)D_00190D64 = rand_range(6, 14);
    sound_play_ui(220);
}

void chargen_restore_roll(void)
{
    int l_18;

    if (mouse_buttons_prev != 0) return;
    if (chargen_roll_saved != 0) {
        for (l_18 = 0; l_18 < 8; l_18++) {
            player_character->attributes[l_18] = chargen_saved_attributes[l_18];
            itemmaker_slot_kinds[l_18] = *(signed char *)(chargen_saved_minimums + (l_18 * 2));
        }
        *(short *)D_00190D64 = (int)(unsigned char)chargen_saved_points;
    }
    mouse_buttons_prev = mouse_buttons;
    mouse_buttons = 0;
}

void chargen_save_roll(void)
{
    int l_18;

    if (mouse_buttons_prev != 0) return;
    chargen_roll_saved = 1;
    chargen_saved_points = *(signed char *)D_00190D64;
    for (l_18 = 0; l_18 < 8; l_18++) {
        chargen_saved_attributes[l_18] = player_character->attributes[l_18];
        *(short *)(chargen_saved_minimums + (l_18 * 2)) = (short)itemmaker_slot_kinds[l_18];
    }
    mouse_buttons_prev = mouse_buttons;
    mouse_buttons = 0;
}

int inv_match_arrows(struct record *a1)
{
    int l_1C;

    if (a1->type != 2) return 0;
    if (a1->image2 == 998 && a1->image == 0) {
        D_00195AF4 = a1;
        return 1;
    }
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
    if (a1->image2 == 998 && l_1C->group == 3 && l_1C->index == 18 && l_1C->condition == 0) return;
    D_00190CA8 += object_weight(a1);
    l_18 = l_1C->value;
    if (a1->image2 == 998) l_18 = l_1C->value * l_1C->condition;
    if (a1->image2 != 998 && ((int)(unsigned short)(a1->flags & 32)) == 0) return;
    if (current_building->type == 13 && holiday_today(game_minutes, (int)(unsigned char)current_region) == 49 && l_1C->group == 3) {
        D_00195D2C += l_18 >> 1;
        return;
    }
    if (current_building->type == 9 && holiday_today(game_minutes, (int)(unsigned char)current_region) == 29) {
        D_00195D2C += l_18 >> 1;
        return;
    }
    if (((struct bf8_7_1 *)&D_001940D6)->f != 0) {
        D_00195D2C += l_18 >> 1;
        return;
    }
    D_00195D2C += l_18;
}

void trade_add_repair_cost(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2 || ((int)(unsigned short)(a1->flags & 512)) != 0) return;
    l_18 = &a1->data.item;
    if (l_18->enchantments[0].type == (-1)) {
        D_00195D2C += ((unsigned)(l_18->value * 10)) / 100;
    } else {
        D_00195D2C += ((unsigned)(l_18->value * 75)) / 100;
    }
    if (D_00195D2C >= 1) return;
    D_00195D2C = 1;
}

void inv_return_unpaid_item(struct record *a1)
{
    int l_20;
    int l_1C;
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (a1->image2 == 998 && l_18->group == 3 && l_18->index == 18 && l_18->condition == 0) return;
    if (a1->image2 == 998 && l_18->group == 3 && l_18->index == 18) {
        l_1C = l_18->stack_count - l_18->condition;
        l_18->stack_count = (signed char)l_18->condition;
        inv_merge_arrows(inv_right_container, a1, 0);
        l_18->condition = 0;
        l_18->stack_count = *(signed char *)&l_1C;
        if (l_1C == 0) object_free_later(a1);
        return;
    }
    if (((int)(unsigned short)(a1->flags & 32)) == 0) return;
    object_reparent(inv_right_container, a1);
    for (l_20 = 0; l_20 < 27; l_20++) {
        if (player_character->equipped[l_20] == a1) player_character->equipped[l_20] = 0;
    }
    D_001940D8 |= 8;
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
    if (a1->image2 == 998) l_18->condition = 0;
    if (((int)(unsigned short)(a1->flags & 32)) == 0) return;
    a1->flags &= ~0x20;
    if (a1->parent != wagon_container || inv_left_container != wagon_container) inv_store_item(a1);
    if (l_18->group != 23 || l_18->index != 0) return;
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
    *(int *)&D_00195DA8 = (int)a1;
    inv_right_container = (D_00195B34 = a1);
    if (inventory_open(2, a2, a3) != 0) return;
    object_free_children(a1);
}
