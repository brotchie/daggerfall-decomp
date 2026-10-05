/* icons.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char key_down_esc;
extern int screen_buffer;
extern char D_00175898[];
extern char D_001758A0[];
extern char D_001758A4[];
extern char D_001758A8[];
extern char hud_buttons[];
extern char D_00185C5A[];
extern char D_00185C5C[];
extern char D_00185C5E[];
extern char D_00185C60[];
extern signed char D_00187CA8;
extern short D_00188208[];
extern char D_00190B44[];
extern char D_00190D64[];
extern char text_macro_fpc[];
extern signed char D_001940D4;
extern signed char D_001940D6;
extern signed char D_001940D8;
extern struct record *wagon_container;
extern struct record *D_001959EC;
extern struct item *D_00195A80;
extern struct record *player_entity;
extern struct record *player_object;
extern int D_00195ACC;
extern struct character *player_character;
extern struct settings *game_settings;
extern char magic_items_image[];
extern signed char mouse_control_mode;
extern signed char view_cursor_active;
extern signed char D_00196272;
extern signed char mouse_buttons_prev;
extern signed char player_ailment_flags;
extern int magic_items_saved_screen;
extern signed char hud_pressed_button;
extern signed char D_00199D71;
extern short steer_key_region;
extern struct picklist D_001A9AB8;
extern struct record *inv_selected_item;

extern int sheet_open(int);
extern int spellbook_open(int);
extern int options_open(int);
extern int sound_play(int, struct record *, int);
extern int disk_read_file(int, int);
extern int picklist_poll(struct picklist *);
extern int inventory_open(int, int, int);
extern int travel_map_open(int);
extern int mc_free();
extern int mc_malloc();
extern int mc_memcpy();
extern int func_000CE44C();
extern int func_0012B136();
extern int func_00144F68();
extern void automap_open(void);
extern void status_show(int);
extern void interaction_mode_cycle(int);
extern void parse_expand(int, int);
extern void rest_open(void);
extern void cursor_draw_arrow(void);
extern void picklist_init(struct picklist *, short, short, int, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void picklist_add(struct picklist *, int, int);
extern void picklist_free(struct picklist *);
extern void picklist_draw(struct picklist *, int);
extern void object_foreach(struct record *, int);
extern void inv_use_item(void);
extern void transport_menu(void);
void hud_buttons_click(int);
void magic_items_add_cb(struct record *);
void magic_items_close(void);

void hud_buttons_click(int a1)
{
    int l_18;

    if (((int)(unsigned char)mouse_control_mode) == 1 && view_cursor_active == 0) {
        return;
    }
    for (l_18 = 0; l_18 < 11; l_18++) {
        if (mouse_x > *(short *)(hud_buttons + (l_18 * 12)) && mouse_x < *(short *)(D_00185C5C + (l_18 * 12)) && mouse_y > *(short *)(D_00185C5A + (l_18 * 12)) && mouse_y < *(short *)(D_00185C5E + (l_18 * 12))) {
            if (a1 != 0 && ((int)(unsigned char)hud_pressed_button) == l_18) {
                sound_play(203, player_object, 100);
                ((int (*)())(*(int *)(D_00185C60 + (l_18 * 12))))((int)(unsigned char)mouse_buttons_prev);
                hud_pressed_button = 255;
                return;
            }
            if (a1 == 0) hud_pressed_button = *(signed char *)&l_18;
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
    short l_18;

    D_001940D6 ^= 64;
    if (((struct bf8_6_1 *)&D_001940D6)->f == 0) return;
    if (player_character->equipped[19] == 0) return;
    *(int *)&l_18 = (int)player_character->equipped[19] + 71;
    sound_play((int)(short)D_00188208[((int)(unsigned short)*(short *)(*(char **)&l_18 + 34))], player_object, 100);
}

void hud_button_status(void)
{
    status_show(1);
}

void hud_button_transport(void)
{
    transport_menu();
}

void hud_button_map(int a1)
{
    if ((a1 & 2) != 0) {
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

void magic_items_add_cb(struct record *a1)
{
    struct item *l_24;
    int l_20;
    int l_1C;
    struct record *l_18;

    if (a1->type != 2) return;
    l_18 = a1->parent;
    while (l_18 != 0 && func_000CE44C((int)((char *)&D_001959EC), l_18, 4) == 0) {
        l_18 = l_18->parent;
    }
    if (l_18 != 0) return;
    if (a1->parent == wagon_container) return;
    l_24 = &a1->data.item;
    l_1C = 0;
    if (l_24->group == 1 && l_24->index == 1 && a1->children != 0 && a1->children->type == 31) {
        l_1C = 2;
    } else {
        if (l_24->enchantments[0].type == (-1)) return;
    }
    if (l_1C == 0) {
        l_20 = 0;
        l_1C = l_20;
        for (; l_20 < 10; l_20++) {
            if (l_24->enchantments[l_20].type == (-1)) break;
            if (l_24->enchantments[l_20].type == 0) l_1C = 1;
        }
    }
    if (l_1C == 0) return;
    D_00195A80 = l_24;
    if (l_1C == 2) {
        D_00195ACC = (int)&a1->children->data.potion_recipe;
        parse_expand((int)D_001758A0, (int)D_00190B44);
    } else {
        parse_expand((int)D_001758A4, (int)D_00190B44);
    }
    picklist_add(&D_001A9AB8, (int)D_00190B44, 0);
    *(int *)(text_macro_fpc + (((int)(short)(*(short *)D_00190D64)++) << 2)) = (int)a1;
}

void magic_items_open(void)
{
    int l_18;

    *(short *)D_00190D64 = 0;
    picklist_init(&D_001A9AB8, 100, 159, 166, 34, 88, 159, 8, 15, 88, 179, 8, 59, 0, 0, 1, 1, 146, 146, 244, 114, 0);
    object_foreach(player_entity->children, (int)magic_items_add_cb);
    if (*(short *)D_00190D64 == 0) {
        picklist_free(&D_001A9AB8);
        return;
    }
    D_00187CA8 = 0;
    D_001940D8 &= 254;
    D_001940D4 |= 32;
    *(int *)magic_items_image = disk_read_file((int)D_001758A8, 0);
    D_00196272 = 1;
    magic_items_saved_screen = mc_malloc(64000, (int)D_00175898, 358);
    mc_memcpy(magic_items_saved_screen, screen_buffer, 64000, (int)D_00175898, 359, 4);
}

void magic_items_frame(void)
{
    short l_18;

    if (((struct bf8_5_1 *)&D_001940D4)->f == 0) return;
    mc_memcpy(screen_buffer, magic_items_saved_screen, 64000, (int)D_00175898, 368, 4);
    func_00144F68((int)(unsigned short)*(short *)(*(char **)magic_items_image), (int)(unsigned short)*(short *)(*(char **)magic_items_image + 2), (int)(unsigned short)*(short *)(*(char **)magic_items_image + 4), (int)(unsigned short)*(short *)(*(char **)magic_items_image + 6), (int)(*(char **)magic_items_image + 12));
    if (key_down_esc != 0 || ((int)(unsigned char)(mouse_buttons & 2)) != 0) {
        while (key_down_esc != 0);
        while (mouse_buttons != 0) func_0012B136();
        magic_items_close();
        return;
    }
    *(int *)&l_18 = picklist_poll(&D_001A9AB8) - 1;
    if (((int)(short)l_18) > (-1)) {
        magic_items_close();
        inv_selected_item = (struct record *)(*(int *)(text_macro_fpc + (((int)(short)l_18) << 2)));
        inv_use_item();
        return;
    }
    picklist_draw(&D_001A9AB8, 0);
}

void magic_items_close(void)
{
    D_001940D4 &= 223;
    if (*(int *)magic_items_image != 0 && *(int *)magic_items_image != (-1751672937)) {
        mc_free(*(int *)magic_items_image, (int)D_00175898, 396);
        *(int *)magic_items_image = -1751672937;
    }
    picklist_free(&D_001A9AB8);
    D_00196272 = 0;
    D_00187CA8 = 1;
    if (magic_items_saved_screen == 0 || magic_items_saved_screen == (-1751672937)) {
        return;
    }
    mc_free(magic_items_saved_screen, (int)D_00175898, 400);
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
