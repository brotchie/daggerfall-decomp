/* icons.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char key_down_esc;
extern iptr screen_buffer;
extern char D_00175898[];
extern char D_001758A0[];
extern char D_001758A4[];
extern char D_001758A8[];
extern struct rect hud_buttons[];
extern signed char D_00187CA8;
extern short D_00188208[];
extern char D_00190B44[];
extern char scratch_190d64[];
extern char scratch_190de4[];
extern signed char D_001940D4;
extern signed char D_001940D6;
extern signed char D_001940D8;
extern struct record *wagon_container;
extern struct record *house_container;
extern struct item *text_macro_item;
extern struct record *player_entity;
extern struct record *player_object;
extern iptr D_00195ACC;
extern struct character *player_character;
extern struct settings *game_settings;
extern struct image *magic_items_image;
extern signed char mouse_control_mode;
extern signed char view_cursor_active;
extern signed char D_00196272;
extern signed char mouse_buttons_prev;
extern signed char player_ailment_flags;
extern iptr magic_items_saved_screen;
extern signed char hud_pressed_button;
extern signed char D_00199D71;
extern short steer_key_region;
extern struct picklist shared_picklist;
extern struct record *inv_selected_item;

extern int sheet_open(short);
extern int spellbook_open(short);
extern int options_open(short);
extern int sound_play(int, struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern int picklist_poll(struct picklist *);
extern int inventory_open(int, int, int);
extern int travel_map_open(int);
extern int mc_free();
extern iptr mc_malloc();
extern int mc_memcpy();
extern iptr xn_str_find_u32();
extern int xn_mouse_poll_clamped();
extern int xn_draw_image();
extern void automap_open(void);
extern void status_show(short);
extern void interaction_mode_cycle(int);
extern void parse_expand(unsigned char *, char *);
extern void rest_open(void);
extern void cursor_draw_arrow(void);
extern void picklist_init(struct picklist *, short, short, short, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void picklist_add(struct picklist *, char *, short);
extern void picklist_free(struct picklist *);
extern void picklist_draw(struct picklist *, int);
extern void object_foreach(struct record *, void (*)());
extern void inv_use_item(void);
extern void transport_menu(void);
void hud_buttons_click(int);
void magic_items_add_cb(struct record *);
void magic_items_close(void);

void hud_buttons_click(int release)
{
    int button;

    if (((int)(unsigned char)mouse_control_mode) == 1 && view_cursor_active == 0) {
        return;
    }
    for (button = 0; button < 11; button++) {
        if (mouse_x > hud_buttons[button].x0 && mouse_x < hud_buttons[button].x1 && mouse_y > hud_buttons[button].y0 && mouse_y < hud_buttons[button].y1) {
            if (release != 0 && ((int)(unsigned char)hud_pressed_button) == button) {
                sound_play(203, player_object, 100);
                hud_buttons[button].handler((int)(unsigned char)mouse_buttons_prev);
                hud_pressed_button = 255;
                return;
            }
            if (release == 0) hud_pressed_button = *(signed char *)&button;
        }
    }
}

void hud_button_sheet(void)
{
    sheet_open(1);
}

void hud_button_spellbook(void)
{
    spellbook_open(1);
}

void hud_button_interaction_mode(void)
{
    if (((int)(unsigned char)(mouse_buttons_prev & 1)) != 0) {
        interaction_mode_cycle(1);
        return;
    }
    interaction_mode_cycle(-1);
}

void hud_button_inventory(void)
{
    inventory_open(1, 0, 2);
}

void hud_toggle_weapon(void)
{
    short item_data;

    D_001940D6 ^= 64;
    if (((struct bf8_6_1 *)&D_001940D6)->f == 0) return;
    if (player_character->equipped[19] == 0) return;
    *(iptr *)&item_data = (iptr)player_character->equipped[19] + 71;
    sound_play((int)(short)D_00188208[((int)(unsigned short)*(short *)(*(char **)&item_data + 34))], player_object, 100);
}

void hud_button_status(void)
{
    status_show(1);
}

void hud_button_transport(void)
{
    transport_menu();
}

void hud_button_map(int buttons)
{
    if ((buttons & 2) != 0) {
        travel_map_open(1);
        return;
    }
    automap_open();
}

void hud_button_rest(void)
{
    rest_open();
}

void hud_button_options(void)
{
    options_open(1);
}

void magic_items_add_cb(struct record *object)
{
    struct item *item;
    int i;
    int kind;
    struct record *parent;

    if (object->type != 2) return;
    parent = object->parent;
    while (parent != 0 && xn_str_find_u32((iptr)((char *)&house_container), parent, 4) == 0) {
        parent = parent->parent;
    }
    if (parent != 0) return;
    if (object->parent == wagon_container) return;
    item = &object->data.item;
    kind = 0;
    if (item->group == 1 && item->index == 1 && object->children != 0 && object->children->type == 31) {
        kind = 2;
    } else {
        if (item->enchantments[0].type == (-1)) return;
    }
    if (kind == 0) {
        i = 0;
        kind = i;
        for (; i < 10; i++) {
            if (item->enchantments[i].type == (-1)) break;
            if (item->enchantments[i].type == 0) kind = 1;
        }
    }
    if (kind == 0) return;
    text_macro_item = item;
    if (kind == 2) {
        D_00195ACC = (iptr)&object->children->data.potion_recipe;
        parse_expand(D_001758A0, D_00190B44);
    } else {
        parse_expand(D_001758A4, D_00190B44);
    }
    picklist_add(&shared_picklist, D_00190B44, 0);
    *(iptr *)(scratch_190de4 + (((int)(short)(*(short *)scratch_190d64)++) << 2)) = (iptr)object;
}

void magic_items_open(void)
{
    int unused;

    *(short *)scratch_190d64 = 0;
    picklist_init(&shared_picklist, 100, 159, 166, 34, 88, 159, 8, 15, 88, 179, 8, 59, 0, 0, 1, 1, 146, 146, 244, 114, 0);
    object_foreach(player_entity->children, magic_items_add_cb);
    if (*(short *)scratch_190d64 == 0) {
        picklist_free(&shared_picklist);
        return;
    }
    D_00187CA8 = 0;
    D_001940D8 &= 254;
    D_001940D4 |= 32;
    magic_items_image = (struct image *)disk_read_file(D_001758A8, 0);
    D_00196272 = 1;
    magic_items_saved_screen = mc_malloc(64000, (iptr)D_00175898, 358);
    mc_memcpy(magic_items_saved_screen, screen_buffer, 64000, (iptr)D_00175898, 359, 4);
}

void magic_items_frame(void)
{
    short picked;

    if (((struct bf8_5_1 *)&D_001940D4)->f == 0) return;
    mc_memcpy(screen_buffer, magic_items_saved_screen, 64000, (iptr)D_00175898, 368, 4);
    xn_draw_image(magic_items_image->x, magic_items_image->y, magic_items_image->width, magic_items_image->height, (iptr)magic_items_image->pixels);
    if (key_down_esc != 0 || ((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        while (key_down_esc != 0);
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        magic_items_close();
        return;
    }
    *(int *)&picked = picklist_poll(&shared_picklist) - 1;
    if (((int)(short)picked) > (-1)) {
        magic_items_close();
        inv_selected_item = (struct record *)(*(iptr *)(scratch_190de4 + (((int)(short)picked) << 2)));
        inv_use_item();
        return;
    }
    picklist_draw(&shared_picklist, 0);
}

void magic_items_close(void)
{
    D_001940D4 &= 223;
    if ((iptr)magic_items_image != 0 && (iptr)magic_items_image != (-1751672937)) {
        mc_free((iptr)magic_items_image, (iptr)D_00175898, 396);
        magic_items_image = (struct image *)(iptr)-1751672937;
    }
    picklist_free(&shared_picklist);
    D_00196272 = 0;
    D_00187CA8 = 1;
    if (magic_items_saved_screen == 0 || magic_items_saved_screen == (-1751672937)) {
        return;
    }
    mc_free(magic_items_saved_screen, (iptr)D_00175898, 400);
    magic_items_saved_screen = -1751672937;
}

int hud_update(void)
{
    if (((int)(short)mouse_y) >= 154 && D_00196272 == 0 && ((int)(unsigned short)(game_settings->view_flags & 1)) == 0) {
        if (((int)(unsigned char)(mouse_buttons & 3)) != 0 && ((int)(unsigned char)(mouse_buttons_prev & 3)) == 0) {
            hud_buttons_click(0);
        }
        if (((int)(unsigned char)(mouse_buttons & 3)) == 0 && ((int)(unsigned char)(mouse_buttons_prev & 3)) != 0) {
            hud_buttons_click(1);
        }
        if (mouse_control_mode == 0 || (((int)(unsigned char)mouse_control_mode) == 1 && view_cursor_active != 0)) {
            cursor_draw_arrow();
            if (((int)(short)steer_key_region) == (-1)) return 1;
        }
    }
    return 0;
}

int hud_portrait_overlay_index(void)
{
    if (player_character->health < (player_character->max_health / 10)) return 2;
    if (D_00199D71 != 0) return 0;
    if (((int)(unsigned char)(player_ailment_flags & 1)) != 0) return 3;
    if (((int)(unsigned char)(player_ailment_flags & 2)) != 0) return 1;
    return -1;
}
