/* generate.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern int screen_buffer;
extern signed char xn_mouse_cursor_drawn;
extern char D_00176F41[];
extern char scratch_190be4[];
extern int scratch_190be8;
extern int D_00190CA8;
extern signed char scratch_190ce4[];
extern signed char scratch_190cee[];
extern char scratch_190d64[];
extern short D_00190DEA[];
extern char scratch_190df0[];
extern signed char D_001940D5;
extern signed char D_001940D6;
extern signed char D_001940D8;
extern struct record *inventory_containers[];
extern struct record *wagon_container;
extern struct building *current_building;
extern struct record *found_object;
extern struct record *inv_right_container;
extern struct record *inv_right_container_base;
extern struct image *D_00195B5C;
extern struct image *D_00195B60;
extern char D_00195B84[];
extern struct character *player_character;
extern int window_image;
extern struct career *player_class;
extern int game_minutes;
extern int trade_total;
extern struct record *D_00195DA8;
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
extern struct record *object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern int inventory_open(int, int, int);
extern int mc_free();
extern int mc_memcpy();
extern int xn_mouse_poll_clamped();
extern int xn_mouse_cursor_move();
extern int xn_draw_image();
extern void msgbox_open_rsc(int, int);
extern void msgbox_update(void);
extern void keys_world_actions(void);
extern void text_draw_coloured(int, int, int, int, unsigned char);
extern void object_free_later(struct record *);
extern void object_free_children(struct record *);
extern void chargen_draw_face(void);
extern void chargen_draw_attributes(void);
extern void chargen_draw_skills(void);
extern void chargen_select_attribute(short);
extern void inv_store_item(struct record *);
extern void inv_create_wagon(void);
extern void inv_merge_arrows(struct record *, struct record *, int);

int chargen_draw(void)
{
    int done;
    int x_offset;
    int y_offset;

    mouse_buttons_prev = mouse_buttons;
    xn_mouse_poll_clamped();
    mc_memcpy(screen_buffer, window_image, 64000, (int)D_00176F41, 181, 4);
    if (((int)(unsigned char)(chargen_screen & 2)) != 0) chargen_draw_face();
    if (((int)(unsigned char)(chargen_screen & 4)) != 0) chargen_draw_attributes();
    if (((int)(unsigned char)(chargen_screen & 8)) != 0) chargen_draw_skills();
    if (((int)(unsigned char)(chargen_screen & 16)) != 0) {
        if (((int)(unsigned char)chargen_screen) == 255) {
            x_offset = 119;
            y_offset = -53;
        } else {
            y_offset = 0;
            x_offset = y_offset;
        }
        xn_draw_image(x_offset + 127, y_offset + ((player_character->reflexes * 9) + 148), 66, 9, (int)(*(char **)&chargen_reflex_image + 12 + (player_character->reflexes * 594)));
    }
    if (((int)(unsigned char)(chargen_screen & 16)) != 0 && ((int)(unsigned char)chargen_screen) != 255 && ((int)(unsigned char)msgbox_kind) != 4) {
        msgbox_open_rsc(307, 4);
        D_001940D5 |= 64;
        D_00195F34 = 125;
    }
    if (((int)(unsigned char)chargen_screen) == 255 && ((int)(unsigned char)game_mode) != 8) {
        text_draw_coloured((int)player_character, 80, 5, 145, 141);
    }
    if (((int)(unsigned char)(chargen_screen & 1)) == 0) keys_world_actions();
    msgbox_update();
    xn_mouse_cursor_drawn &= 254;
    xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
    if (chargen_screen != 0 && mouse_buttons != 0 && ((int)(short)mouse_x) > 263 && ((int)(short)mouse_x) < 301 && ((int)(short)mouse_y) > 172 && ((int)(short)mouse_y) < 193) {
        done = 1;
    } else {
        done = 0;
    }
    return done;
}

void chargen_free_images(void)
{
    chargen_screen = 0;
    if (chargen_face_images != 0 && chargen_face_images != (-1751672937)) {
        mc_free(chargen_face_images, (int)D_00176F41, 224);
        chargen_face_images = -1751672937;
    }
    if ((int)D_00195B60 != 0 && (int)D_00195B60 != (-1751672937)) {
        mc_free((int)D_00195B60, (int)D_00176F41, 225);
        D_00195B60 = (struct image *)-1751672937;
    }
    if ((int)D_00195B5C != 0 && (int)D_00195B5C != (-1751672937)) {
        mc_free((int)D_00195B5C, (int)D_00176F41, 226);
        D_00195B5C = (struct image *)-1751672937;
    }
    if (chargen_reflex_image != 0 && chargen_reflex_image != (-1751672937)) {
        mc_free(chargen_reflex_image, (int)D_00176F41, 227);
        chargen_reflex_image = -1751672937;
    }
    if (window_image == 0 || window_image == (-1751672937)) return;
    mc_free(window_image, (int)D_00176F41, 228);
    window_image = -1751672937;
}

void chargen_attribute_button(int button)
{
    chargen_select_attribute((int)(short)(button - 20));
}

void chargen_skill_arrow(int button)
{
    int group;
    int slot;
    struct character_skill *skill;
    int *bios_ticks;
    int *bios_ticks_now;

    bios_ticks = (int *)1132;
    if (((unsigned)(*bios_ticks - *(int *)scratch_190be4)) < 6) return;
    bios_ticks_now = (int *)1132;
    *(int *)scratch_190be4 = *bios_ticks_now;
    button += -14;
    group = button >> 1;
    if (D_00190DEA[group] == 0 && (button & 1) != 0) return;
    slot = (int)(short)*(short *)(scratch_190df0 + (group * 2));
    skill = &player_character->skills[player_class->skills[slot]];
    if ((unsigned char)skill->value == scratch_190cee[slot] && (button & 1) == 0) return;
    if ((button & 1) != 0) {
        (*(signed char *)((char *)skill))++;
        (D_00190DEA[group])--;
        return;
    }
    (*(signed char *)((char *)skill))--;
    (D_00190DEA[group])++;
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
    int i;
    int minimum;

    if (mouse_buttons_prev != 0) return;
    if (((int)(unsigned char)chargen_screen) == 255) {
        scratch_190be8 = 1;
        return;
    }
    for (i = 0; i < 8; i++) {
        minimum = player_class->attributes[i];
        scratch_190ce4[i] = (player_character->attributes[i] = rand_range(minimum, minimum + 10));
        if (player_character->attributes[i] > 100) player_character->attributes[i] = 100;
    }
    *(short *)scratch_190d64 = rand_range(6, 14);
    sound_play_ui(220);
}

void chargen_restore_roll(void)
{
    int i;

    if (mouse_buttons_prev != 0) return;
    if (chargen_roll_saved != 0) {
        for (i = 0; i < 8; i++) {
            player_character->attributes[i] = chargen_saved_attributes[i];
            scratch_190ce4[i] = *(signed char *)(chargen_saved_minimums + (i * 2));
        }
        *(short *)scratch_190d64 = (int)(unsigned char)chargen_saved_points;
    }
    mouse_buttons_prev = mouse_buttons;
    mouse_buttons = 0;
}

void chargen_save_roll(void)
{
    int i;

    if (mouse_buttons_prev != 0) return;
    chargen_roll_saved = 1;
    chargen_saved_points = *(signed char *)scratch_190d64;
    for (i = 0; i < 8; i++) {
        chargen_saved_attributes[i] = player_character->attributes[i];
        *(short *)(chargen_saved_minimums + (i * 2)) = (short)scratch_190ce4[i];
    }
    mouse_buttons_prev = mouse_buttons;
    mouse_buttons = 0;
}

int inv_match_arrows(struct record *object)
{
    int unused;

    if (object->type != 2) return 0;
    if (object->image2 == 998 && object->image == 0) {
        found_object = object;
        return 1;
    }
    return 0;
}

void inv_sum_hidden_weight(struct record *object)
{
    struct item *item;

    if (object->type != 2) return;
    item = &object->data.item;
    if (((int)(unsigned short)(item->item_flags & 64)) == 0) return;
    *(int *)D_00195B84 += item->weight;
}

void trade_add_buy_price(struct record *object)
{
    struct item *item;
    int price;

    if (object == 0) return;
    if (object->type != 2) return;
    item = &object->data.item;
    if (object->image2 == 998 && item->group == 3 && item->index == 18 && item->condition == 0) return;
    D_00190CA8 += object_weight(object);
    price = item->value;
    if (object->image2 == 998) price = item->value * item->condition;
    if (object->image2 != 998 && ((int)(unsigned short)(object->flags & 32)) == 0) return;
    if (current_building->type == 13 && holiday_today(game_minutes, (int)(unsigned char)current_region) == 49 && item->group == 3) {
        trade_total += price >> 1;
        return;
    }
    if (current_building->type == 9 && holiday_today(game_minutes, (int)(unsigned char)current_region) == 29) {
        trade_total += price >> 1;
        return;
    }
    if (((struct bf8_7_1 *)&D_001940D6)->f != 0) {
        trade_total += price >> 1;
        return;
    }
    trade_total += price;
}

void trade_add_repair_cost(struct record *object)
{
    struct item *item;

    if (object->type != 2 || ((int)(unsigned short)(object->flags & 512)) != 0) return;
    item = &object->data.item;
    if (item->enchantments[0].type == (-1)) {
        trade_total += ((unsigned)(item->value * 10)) / 100;
    } else {
        trade_total += ((unsigned)(item->value * 75)) / 100;
    }
    if (trade_total >= 1) return;
    trade_total = 1;
}

void inv_return_unpaid_item(struct record *object)
{
    int slot;
    int remaining;
    struct item *item;

    if (object->type != 2) return;
    item = &object->data.item;
    if (object->image2 == 998 && item->group == 3 && item->index == 18 && item->condition == 0) return;
    if (object->image2 == 998 && item->group == 3 && item->index == 18) {
        remaining = item->stack_count - item->condition;
        item->stack_count = (signed char)item->condition;
        inv_merge_arrows(inv_right_container, object, 0);
        item->condition = 0;
        item->stack_count = *(signed char *)&remaining;
        if (remaining == 0) object_free_later(object);
        return;
    }
    if (((int)(unsigned short)(object->flags & 32)) == 0) return;
    object_reparent(inv_right_container, object);
    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] == object) player_character->equipped[slot] = 0;
    }
    D_001940D8 |= 8;
}

void inv_store_cb(struct record *object)
{
    if (object->type != 2) return;
    inv_store_item(object);
}

void inv_claim_item(struct record *object)
{
    struct item *item;

    if (object->type != 2) return;
    item = &object->data.item;
    if (object->image2 == 998) item->condition = 0;
    if (((int)(unsigned short)(object->flags & 32)) == 0) return;
    object->flags &= ~0x20;
    if (object->parent != wagon_container || inv_left_container != wagon_container) inv_store_item(object);
    if (item->group != 23 || item->index != 0) return;
    inv_create_wagon();
}

void inv_assign_item_id(struct record *object)
{
    if (object->type != 2) return;
    if ((((unsigned)object->id) >> 16) == 100) return;
    object->id = object_new_id(100);
}

void inv_reset_left_list(void)
{
    *(int *)inv_left_scroll = 0;
    inv_left_container = inventory_containers[0];
}

void inventory_open_container(struct record *container, int mode, int icon)
{
    D_00195DA8 = container;
    inv_right_container = (inv_right_container_base = container);
    if (inventory_open(2, mode, icon) != 0) return;
    object_free_children(container);
}
