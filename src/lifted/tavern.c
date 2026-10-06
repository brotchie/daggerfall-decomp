/* tavern.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "clib.h"

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
extern char *tavern_food_names[];
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
extern char *scratch_buffer;
extern struct record *D_00195CE8;
extern int trade_price;
extern signed char current_region;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char mouse_buttons_prev;
extern struct building *D_00196ABC;
extern iptr tavern_menu_image;
extern iptr politic_pak;
extern iptr climate_pak;
extern iptr object_found_last;
extern char region_flats[];

extern int key_action_held(int);
extern int holiday_today(int, int);
extern iptr disk_read_file(char *, iptr);
extern int guild_is_local_knight(void);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int object_find(struct record *, iptr (*)());
extern iptr object_find_by_id(struct record *, iptr);
extern int trade_adjust_price(int, int);
extern struct record *marker_find_nth(struct record *, int, int);
extern struct record *marker_find_random(struct record *, int);
extern int xn_mouse_poll_clamped(void);
extern void xn_kbd_flush(void);
extern void msgbox_show_string(char *, short);
extern void msgbox_show_rsc(int, int);
extern void npc_talk(iptr);
extern void list_popup_open(iptr);
extern void inpstr_begin_number(int);
extern void object_foreach(struct record *, void (*)());
extern void trade_make_offer(void);
int tavern_room_rented(void);
int tavern_room_days_left(void);
iptr func_0001FA3A(struct record *);
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
        mc_free((void *)tavern_menu_image, D_00170569, 117);
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
    int day;
    int price;

    if ((tavern_room_days_left() + *(int *)inpstr_result) > 350) {
        tavern_state = 0;
        msgbox_show_rsc(16, 1);
        return;
    }
    if ((*(short *)scratch_190d64 = *(short *)inpstr_result) == 0) {
        tavern_state = 0;
        return;
    }
    day = (((unsigned)(((unsigned)game_minutes) % 518400)) / 1440) + 1;
    if (day <= 46 && (((int)(short)*(short *)scratch_190d64) + day) > 46) {
        price = (((int)(short)*(short *)scratch_190d64) - 1) * 7;
    } else {
        price = ((int)(short)*(short *)scratch_190d64) * 7;
    }
    if (guild_is_local_knight() != 0) {
        msgbox_show_string(D_00170572, 1);
        tavern_state = 0;
        if (tavern_room_rented() != 0) {
            tavern_extend_room(((int)(short)*(short *)scratch_190d64) * 1440);
        } else {
            tavern_rent(((int)(short)*(short *)scratch_190d64) * 1440);
        }
        return;
    }
    if (price == 0) {
        msgbox_show_string(D_0017059D, 1);
        tavern_state = 0;
        if (tavern_room_rented() != 0) {
            tavern_extend_room(((int)(short)*(short *)scratch_190d64) * 1440);
        } else {
            tavern_rent(((int)(short)*(short *)scratch_190d64) * 1440);
        }
        return;
    }
    trade_adjust_price(price, 0);
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
        msgbox_show_string(D_001705BE, 1);
        return;
    }
    list_popup_open((iptr)(char *)tavern_food_names);
    D_001940D4 |= 1;
}

void tavern_buy_food(int food)
{
    int price;
    int holiday;

    price = (int)(unsigned char)tavern_food_prices[food];
    holiday = holiday_today(game_minutes, (int)(unsigned char)current_region);
    if (holiday == 37) {
        price >>= 1;
        if (price == 0) price++;
    }
    if (holiday != 1 && ((unsigned)player_character->gold) < price) {
        msgbox_show_rsc(454, 1);
        return;
    }
    if (holiday != 1) player_character->gold -= price;
    player_character->health += price * 2;
    if (player_character->health > player_character->max_health) {
        player_character->health = player_character->max_health;
    }
    player_character->last_meal_time = game_minutes;
}

void tavern_talk_button(void)
{
    tavern_close();
    npc_talk((iptr)D_00195CE8);
}

void tavern_go_to_room(void)
{
    struct record *marker;

    marker = marker_find_nth(player_object->parent->children, 2, tavern_building->room);
    if (marker == 0) return;
    player_object->x = marker->x;
    player_object->y = marker->y;
    player_object->z = marker->z;
}

void tavern_rent(int minutes)
{
    struct record *marker;

    marker = marker_find_random(player_object->parent->children, 2);
    if (marker == 0) return;
    tavern_building->rent_expires = game_minutes + minutes;
    tavern_building->flags &= 248;
    tavern_building->flags |= 2;
    tavern_building->room = marker->owner;
}

int tavern_room_rented(void)
{
    int rented;

    if (((int)(unsigned char)(tavern_building->flags & 2)) != 0 && ((unsigned)tavern_building->rent_expires) > game_minutes) {
        rented = 1;
    } else {
        rented = 0;
    }
    return rented;
}

int tavern_room_days_left(void)
{
    if (((int)(unsigned char)(tavern_building->flags & 2)) == 0) return 0;
    if (tavern_room_rented() == 0) return 0;
    return ((unsigned)((tavern_building->rent_expires - game_minutes) + 1439)) / 1440;
}

void tavern_extend_room(int minutes)
{
    tavern_building->rent_expires += minutes;
}

void func_0001F6E2(struct record *object)
{
    struct record *stored;

    if (object->type != 2) return;
    if (func_0001FBF5(object) == 0) return;
    stored = object_create_child(nonworld_root, 0, 107);
    stored->type = 58;
    mc_memcpy(&stored->x, &object->x, 12, D_00170569, 317, 4);
    stored->image = object->image;
    stored->id = object->id;
    stored->parent_id = object->parent->id;
    stored->repair_due = *(int *)scratch_190be4;
    mc_memcpy(RECORD_DATA(stored), RECORD_DATA(object), 107, D_00170569, 322, 4);
}

void func_0001F7B3(struct record *stored)
{
    struct record *object;

    if (stored->type != 58) return;
    if ((((unsigned)stored->id) >> 16) != (((unsigned)location_object->id) >> 16)) return;
    if ((object_found_last = object_find_by_id(location_object, stored->parent_id)) == 0) return;
    object = object_create_child((struct record *)object_found_last, 0, 107);
    object->type = 2;
    object->image = stored->image;
    object->id = stored->id;
    mc_memcpy(&object->x, &stored->x, 12, D_00170569, 347, 4);
    mc_memcpy(RECORD_DATA(object), RECORD_DATA(stored), 107, D_00170569, 348, 4);
    object_delete(stored);
}

void func_0001F89F(void)
{
    struct record *stored;

    func_0001FE4D();
    D_00196ABC = (struct building *)scratch_buffer;
    *(int *)D_00195B84 = 0;
    func_0001FB3F();
    func_0001FAB2();
    if (*(int *)D_00195B84 != 0) {
        object_delete(found_object);
        stored = object_create_child(nonworld_root, 0, *(int *)D_00195B84 * 26);
        stored->type = 57;
        stored->owner = *(short *)D_00195B84;
        stored->id = location_object->id;
        mc_memcpy(RECORD_DATA(stored), D_00196ABC, *(int *)D_00195B84 * 26, D_00170569, 369, 4);
    }
    object_foreach(location_object, func_0001F6E2);
}

void func_0001F958(void)
{
    int i;
    int j;

    func_0001FE4D();
    D_00196ABC = (struct building *)scratch_buffer;
    *(int *)D_00195B84 = 0;
    func_0001FAB2();
    for (i = 0; i < *(int *)D_00195B84; i++) {
        for (j = 0; current_location->building_count > j; j++) {
            if (current_location->buildings[j].id == D_00196ABC[i].id) {
                mc_memcpy(&current_location->buildings[j], &D_00196ABC[i], 26, D_00170569, 388, 4);
                break;
            }
        }
    }
    object_foreach(nonworld_root, func_0001F7B3);
}

iptr func_0001FA3A(struct record *object)
{
    if (found_object != 0) return 0;
    if (object->type != 57) return 0;
    if ((((unsigned)object->id) >> 16) == (((unsigned)location_object->id) >> 16)) {
        found_object = object;
        return 1;
    }
    return 0;
}

void func_0001FAB2(void)
{
    *(int *)D_00195B84 = 0;
    found_object = 0;
    object_find(nonworld_root, func_0001FA3A);
    if (found_object == 0) return;
    mc_memcpy(&D_00196ABC[*(int *)D_00195B84], RECORD_DATA(found_object), found_object->owner * 26, D_00170569, 417, 4);
    *(int *)D_00195B84 += found_object->owner;
}

void func_0001FB3F(void)
{
    int i;

    for (i = 0; current_location->building_count > i; i++) {
        if ((current_location->buildings[i].flags & 3) != 0 && ((unsigned)current_location->buildings[i].flags) > game_minutes) {
            mc_memcpy(&D_00196ABC[(*(int *)D_00195B84)++], &current_location->buildings[i], 26, D_00170569, 428, 4);
        }
    }
}

int func_0001FBF5(struct record *object)
{
    int i;
    struct building *building;

    if (object->parent->type != 43) return 0;
    if (object->quest_id != 0) return 0;
    if ((((unsigned)object->id) >> 16) == (((unsigned)location_object->id) >> 16)) return 0;
    building = 0;
    for (i = 0; current_location->building_count > i; i++) {
        if (object->parent->id == current_location->buildings[i].id) {
            building = &current_location->buildings[i];
        }
    }
    if (building == 0) return 0;
    if (building->type != 15 && building->type != 1) return 0;
    *(int *)scratch_190be4 = 2147483647;
    if (building->type == 1 && building->id == player_character->house) return 1;
    *(int *)scratch_190be4 = building->rent_expires;
    return (((((int)(unsigned char)(building->flags & 2)) != 0) && (((unsigned)building->rent_expires) > game_minutes)) ? 1 : 0);
}

void func_0001FD7C(struct record *object)
{
    int i;
    int live_count;

    if (object->type != 57 && object->type != 58) return;
    if (object->type == 57) {
        D_00196ABC = (struct building *)RECORD_DATA(object);
        live_count = object->owner;
        for (i = 0; object->owner > i; i++, D_00196ABC++) {
            if (((unsigned)D_00196ABC->rent_expires) <= game_minutes) live_count--;
        }
        if (live_count == 0) object_delete(object);
        return;
    }
    if (((unsigned)object->repair_due) > game_minutes) return;
    object_delete(object);
}

void func_0001FE4D(void)
{
    object_foreach(nonworld_root, func_0001FD7C);
}

void region_load_tables(void)
{
    politic_pak = disk_read_file(D_001705D4, 0);
    climate_pak = disk_read_file(D_001705E0, 0);
    disk_read_file(D_001705EC, (iptr)region_flats);
}
