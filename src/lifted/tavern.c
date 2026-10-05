/* tavern.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char mouse_buttons;
extern signed char key_down_esc;
extern signed char key_down_enter;
extern char D_00170569[];
extern char D_00170572[];
extern char D_0017059D[];
extern char D_001705BE[];
extern char D_001705D4[];
extern char D_001705E0[];
extern char D_001705EC[];
extern char tavern_food_names[];
extern signed char tavern_food_prices[];
extern char scratch_190be4[];
extern signed char tavern_state;
extern char scratch_190d64[];
extern signed char D_001940D4;
extern struct record *nonworld_root;
extern struct record *player_object;
extern struct record *location_object;
extern struct building *tavern_building;
extern struct record *found_object;
extern char D_00195B84[];
extern char inpstr_result[];
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern char scratch_buffer[];
extern char D_00195CE8[];
extern int trade_price;
extern signed char current_region;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern struct building *D_00196ABC;
extern int tavern_menu_image;
extern int politic_pak;
extern int climate_pak;
extern int object_found_last;
extern char region_flats[];

extern int key_action_held(int);
extern int holiday_today(int, int);
extern int disk_read_file(int, int);
extern int guild_is_local_knight(void);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_find(struct record *, int);
extern int object_find_by_id(struct record *, int);
extern int trade_adjust_price(int, int);
extern struct record *marker_find_nth(struct record *, int, int);
extern struct record *marker_find_random(struct record *, int);
extern int mc_free();
extern int mc_memcpy();
extern int xn_mouse_poll_clamped();
extern int xn_kbd_flush();
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void npc_talk(int);
extern void list_popup_open(int);
extern void inpstr_begin_number(int);
extern void object_foreach(struct record *, int);
extern void trade_make_offer(void);
int tavern_room_rented(void);
int tavern_room_days_left(void);
int func_0001FA3A(struct record *);
int func_0001FBF5(struct record *);
void tavern_close(void);
void tavern_rent(int);
void tavern_extend_room(int);
void func_0001F6E2(struct record *);
void func_0001F7B3(struct record *);
void func_0001FAB2(void);
void func_0001FB3F(void);
void func_0001FD7C(struct record *);
void func_0001FE4D(void);

void tavern_close(void)
{
    mouse_buttons = (mouse_buttons_prev = 0);
    xn_mouse_poll_clamped();
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    while (key_action_held(18) != 0);
    while (key_down_esc != 0);
    game_mode = 0;
    if (tavern_menu_image != 0 && tavern_menu_image != (-1751672937)) {
        mc_free(tavern_menu_image, (int)D_00170569, 117);
        tavern_menu_image = -1751672937;
    }
    D_00196272 = 0;
}

void tavern_rent_room(void)
{
    xn_kbd_flush();
    inpstr_begin_number(1);
    tavern_state = 1;
    if (tavern_room_rented() != 0) {
        msgbox_show_rsc(5100, 2);
    } else {
        msgbox_show_rsc(5102, 2);
    }
    while (key_down_enter != 0);
    xn_kbd_flush();
}

void tavern_room_offer(void)
{
    int l_1C;
    int l_18;

    if ((tavern_room_days_left() + *(int *)inpstr_result) > 350) {
        tavern_state = 0;
        msgbox_show_rsc(16, 1);
        return;
    }
    if ((*(short *)scratch_190d64 = *(short *)inpstr_result) == 0) {
        tavern_state = 0;
        return;
    }
    l_1C = (((unsigned)(((unsigned)game_minutes) % 518400)) / 1440) + 1;
    if (l_1C <= 46 && (((int)(short)*(short *)scratch_190d64) + l_1C) > 46) {
        l_18 = (((int)(short)*(short *)scratch_190d64) - 1) * 7;
    } else {
        l_18 = ((int)(short)*(short *)scratch_190d64) * 7;
    }
    if (guild_is_local_knight() != 0) {
        msgbox_show_string((int)D_00170572, 1);
        tavern_state = 0;
        if (tavern_room_rented() != 0) {
            tavern_extend_room(((int)(short)*(short *)scratch_190d64) * 1440);
        } else {
            tavern_rent(((int)(short)*(short *)scratch_190d64) * 1440);
        }
        return;
    }
    if (l_18 == 0) {
        msgbox_show_string((int)D_0017059D, 1);
        tavern_state = 0;
        if (tavern_room_rented() != 0) {
            tavern_extend_room(((int)(short)*(short *)scratch_190d64) * 1440);
        } else {
            tavern_rent(((int)(short)*(short *)scratch_190d64) * 1440);
        }
        return;
    }
    trade_adjust_price(l_18, 0);
    trade_make_offer();
    tavern_state = 2;
}

void tavern_room_pay(void)
{
    tavern_state = 0;
    if (((int)D_00196271) == 2) return;
    if (((unsigned)player_character->gold) < trade_price) {
        msgbox_show_rsc(454, 1);
        return;
    }
    player_character->gold -= trade_price;
    if (tavern_room_rented() != 0) {
        tavern_extend_room(((int)(short)*(short *)scratch_190d64) * 1440);
        return;
    }
    tavern_rent(((int)(short)*(short *)scratch_190d64) * 1440);
}

void tavern_food_button(void)
{
    if (((unsigned)(game_minutes - player_character->last_meal_time)) < 240) {
        msgbox_show_string((int)D_001705BE, 1);
        return;
    }
    list_popup_open((int)tavern_food_names);
    D_001940D4 |= 1;
}

void tavern_buy_food(int a1)
{
    int l_1C;
    int l_18;

    l_1C = (int)(unsigned char)tavern_food_prices[a1];
    l_18 = holiday_today(game_minutes, (int)(unsigned char)current_region);
    if (l_18 == 37) {
        l_1C >>= 1;
        if (l_1C == 0) l_1C++;
    }
    if (l_18 != 1 && ((unsigned)player_character->gold) < l_1C) {
        msgbox_show_rsc(454, 1);
        return;
    }
    if (l_18 != 1) player_character->gold -= l_1C;
    player_character->health += l_1C * 2;
    if (player_character->health > player_character->max_health) {
        player_character->health = player_character->max_health;
    }
    player_character->last_meal_time = game_minutes;
}

void tavern_talk_button(void)
{
    tavern_close();
    npc_talk(*(int *)D_00195CE8);
}

void tavern_go_to_room(void)
{
    struct record *l_18;

    l_18 = marker_find_nth(player_object->parent->children, 2, tavern_building->room);
    if (l_18 == 0) return;
    player_object->x = l_18->x;
    player_object->y = l_18->y;
    player_object->z = l_18->z;
}

void tavern_rent(int a1)
{
    struct record *l_18;

    l_18 = marker_find_random(player_object->parent->children, 2);
    if (l_18 == 0) return;
    tavern_building->rent_expires = game_minutes + a1;
    tavern_building->flags &= 248;
    tavern_building->flags |= 2;
    tavern_building->room = l_18->owner;
}

int tavern_room_rented(void)
{
    int l_1C;

    if (((int)(unsigned char)(tavern_building->flags & 2)) != 0 && ((unsigned)tavern_building->rent_expires) > game_minutes) {
        l_1C = 1;
    } else {
        l_1C = 0;
    }
    return l_1C;
}

int tavern_room_days_left(void)
{
    if (((int)(unsigned char)(tavern_building->flags & 2)) == 0) return 0;
    if (tavern_room_rented() == 0) return 0;
    return ((unsigned)((tavern_building->rent_expires - game_minutes) + 1439)) / 1440;
}

void tavern_extend_room(int a1)
{
    tavern_building->rent_expires += a1;
}

void func_0001F6E2(struct record *a1)
{
    struct record *l_18;

    if (a1->type != 2) return;
    if (func_0001FBF5(a1) == 0) return;
    l_18 = object_create_child(nonworld_root, 0, 107);
    l_18->type = 58;
    mc_memcpy(&l_18->x, &a1->x, 12, (int)D_00170569, 317, 4);
    l_18->image = a1->image;
    l_18->id = a1->id;
    l_18->parent_id = a1->parent->id;
    l_18->repair_due = *(int *)scratch_190be4;
    mc_memcpy(RECORD_DATA(l_18), RECORD_DATA(a1), 107, (int)D_00170569, 322, 4);
}

void func_0001F7B3(struct record *a1)
{
    struct record *l_18;

    if (a1->type != 58) return;
    if ((((unsigned)a1->id) >> 16) != (((unsigned)location_object->id) >> 16)) return;
    if ((object_found_last = object_find_by_id(location_object, a1->parent_id)) == 0) return;
    l_18 = object_create_child((struct record *)object_found_last, 0, 107);
    l_18->type = 2;
    l_18->image = a1->image;
    l_18->id = a1->id;
    mc_memcpy(&l_18->x, &a1->x, 12, (int)D_00170569, 347, 4);
    mc_memcpy(RECORD_DATA(l_18), RECORD_DATA(a1), 107, (int)D_00170569, 348, 4);
    object_delete(a1);
}

void func_0001F89F(void)
{
    struct record *l_18;

    func_0001FE4D();
    D_00196ABC = (struct building *)*(int *)scratch_buffer;
    *(int *)D_00195B84 = 0;
    func_0001FB3F();
    func_0001FAB2();
    if (*(int *)D_00195B84 != 0) {
        object_delete(found_object);
        l_18 = object_create_child(nonworld_root, 0, *(int *)D_00195B84 * 26);
        l_18->type = 57;
        l_18->owner = *(short *)D_00195B84;
        l_18->id = location_object->id;
        mc_memcpy(RECORD_DATA(l_18), (int)D_00196ABC, *(int *)D_00195B84 * 26, (int)D_00170569, 369, 4);
    }
    object_foreach(location_object, (int)func_0001F6E2);
}

void func_0001F958(void)
{
    int l_1C;
    int l_18;

    func_0001FE4D();
    D_00196ABC = (struct building *)*(int *)scratch_buffer;
    *(int *)D_00195B84 = 0;
    func_0001FAB2();
    for (l_1C = 0; l_1C < *(int *)D_00195B84; l_1C++) {
        for (l_18 = 0; current_location->building_count > l_18; l_18++) {
            if (current_location->buildings[l_18].id == D_00196ABC[l_1C].id) {
                mc_memcpy((int)&current_location->buildings[l_18], (int)&D_00196ABC[l_1C], 26, (int)D_00170569, 388, 4);
                break;
            }
        }
    }
    object_foreach(nonworld_root, (int)func_0001F7B3);
}

int func_0001FA3A(struct record *a1)
{
    if (found_object != 0) return 0;
    if (a1->type != 57) return 0;
    if ((((unsigned)a1->id) >> 16) == (((unsigned)location_object->id) >> 16)) {
        found_object = a1;
        return 1;
    }
    return 0;
}

void func_0001FAB2(void)
{
    *(int *)D_00195B84 = 0;
    found_object = 0;
    object_find(nonworld_root, (int)func_0001FA3A);
    if (found_object == 0) return;
    mc_memcpy((int)&D_00196ABC[*(int *)D_00195B84], (int)RECORD_DATA(found_object), found_object->owner * 26, (int)D_00170569, 417, 4);
    *(int *)D_00195B84 += found_object->owner;
}

void func_0001FB3F(void)
{
    int l_18;

    for (l_18 = 0; current_location->building_count > l_18; l_18++) {
        if ((current_location->buildings[l_18].flags & 3) != 0 && ((unsigned)current_location->buildings[l_18].flags) > game_minutes) {
            mc_memcpy((int)&D_00196ABC[(*(int *)D_00195B84)++], (int)&current_location->buildings[l_18], 26, (int)D_00170569, 428, 4);
        }
    }
}

int func_0001FBF5(struct record *a1)
{
    int l_20;
    struct building *l_1C;

    if (a1->parent->type != 43) return 0;
    if (a1->quest_id != 0) return 0;
    if ((((unsigned)a1->id) >> 16) == (((unsigned)location_object->id) >> 16)) return 0;
    l_1C = 0;
    for (l_20 = 0; current_location->building_count > l_20; l_20++) {
        if (a1->parent->id == current_location->buildings[l_20].id) {
            l_1C = &current_location->buildings[l_20];
        }
    }
    if (l_1C == 0) return 0;
    if (l_1C->type != 15 && l_1C->type != 1) return 0;
    *(int *)scratch_190be4 = 2147483647;
    if (l_1C->type == 1 && l_1C->id == player_character->house) return 1;
    *(int *)scratch_190be4 = l_1C->rent_expires;
    return (((((int)(unsigned char)(l_1C->flags & 2)) != 0) && (((unsigned)l_1C->rent_expires) > game_minutes)) ? 1 : 0);
}

void func_0001FD7C(struct record *a1)
{
    int l_1C;
    int l_18;

    if (a1->type != 57 && a1->type != 58) return;
    if (a1->type == 57) {
        D_00196ABC = (struct building *)RECORD_DATA(a1);
        l_18 = a1->owner;
        for (l_1C = 0; a1->owner > l_1C; l_1C++, D_00196ABC++) {
            if (((unsigned)D_00196ABC->rent_expires) <= game_minutes) l_18--;
        }
        if (l_18 == 0) object_delete(a1);
        return;
    }
    if (((unsigned)a1->repair_due) > game_minutes) return;
    object_delete(a1);
}

void func_0001FE4D(void)
{
    object_foreach(nonworld_root, (int)func_0001FD7C);
}

void region_load_tables(void)
{
    politic_pak = disk_read_file((int)D_001705D4, 0);
    climate_pak = disk_read_file((int)D_001705E0, 0);
    disk_read_file((int)D_001705EC, (int)region_flats);
}
