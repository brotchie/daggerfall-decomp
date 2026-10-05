/* tavern.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char mouse_buttons[];
extern char key_down_esc[];
extern char key_down_enter[];
extern char D_00170569[];
extern char D_00170572[];
extern char D_0017059D[];
extern char D_001705BE[];
extern char D_001705D4[];
extern char D_001705E0[];
extern char D_001705EC[];
extern char tavern_food_names[];
extern char tavern_food_prices[];
extern char D_00190BE4[];
extern char tavern_state[];
extern char D_00190D64[];
extern char D_001940D4[];
extern struct record *nonworld_root;
extern struct record *player_object;
extern struct record *D_00195AC4;
extern struct building *tavern_building;
extern struct record *D_00195AF4;
extern char D_00195B84[];
extern char inpstr_result[];
extern struct location *current_location;
extern struct character *player_character;
extern char game_minutes[];
extern char D_00195C44[];
extern char D_00195CE8[];
extern char D_00195D30[];
extern char current_region[];
extern char D_00196271[];
extern char D_00196272[];
extern char game_mode[];
extern char mouse_buttons_prev[];
extern struct building *D_00196ABC;
extern char tavern_menu_image[];
extern char politic_pak[];
extern char climate_pak[];
extern char D_00199770[];
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
extern int func_0012B136();
extern int func_00142790();
extern void msgbox_show_string(int, int);
extern void msgbox_show_rsc(int, int);
extern void npc_talk(int);
extern void picklist_open(int);
extern void inpstr_begin_number(int);
extern void object_foreach(struct record *, int);
extern void func_00097A85(void);
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
    *(signed char *)mouse_buttons = (*(signed char *)mouse_buttons_prev = 0);
    func_0012B136();
L1F0E9:;
    if (*(signed char *)mouse_buttons == 0) goto L1F0F9;
    func_0012B136();
    goto L1F0E9;
L1F0F9:;
    if (key_action_held(18) != 0) goto L1F0F9;
L1F107:;
    if (*(signed char *)key_down_esc != 0) goto L1F107;
    *(signed char *)game_mode = 0;
    if (*(int *)tavern_menu_image == 0) goto L1F12C;
    if (*(int *)tavern_menu_image != (-1751672937)) goto L1F12E;
L1F12C:;
    goto L1F14C;
L1F12E:;
    mc_free(*(int *)tavern_menu_image, (int)D_00170569, 117);
    *(int *)tavern_menu_image = -1751672937;
L1F14C:;
    *(signed char *)D_00196272 = 0;
}

void tavern_rent_room(void)
{
    func_00142790();
    inpstr_begin_number(1);
    *(signed char *)tavern_state = 1;
    if (tavern_room_rented() == 0) goto L1F19B;
    msgbox_show_rsc(5100, 2);
    goto L1F1AA;
L1F19B:;
    msgbox_show_rsc(5102, 2);
L1F1AA:;
    if (*(signed char *)key_down_enter != 0) goto L1F1AA;
    func_00142790();
}

void tavern_room_offer(void)
{
    int l_1C;
    int l_18;

    if ((tavern_room_days_left() + *(int *)inpstr_result) <= 350) goto L1F1FD;
    *(signed char *)tavern_state = 0;
    msgbox_show_rsc(16, 1);
    return;
L1F1FD:;
    if ((*(short *)D_00190D64 = *(short *)inpstr_result) != 0) goto L1F21F;
    *(signed char *)tavern_state = 0;
    return;
L1F21F:;
    l_1C = (((unsigned)(((unsigned)*(int *)game_minutes) % 518400)) / 1440) + 1;
    if (l_1C > 46) goto L1F251;
    if ((((int)(short)*(short *)D_00190D64) + l_1C) > 46) goto L1F253;
L1F251:;
    goto L1F263;
L1F253:;
    l_18 = (((int)(short)*(short *)D_00190D64) - 1) * 7;
    goto L1F270;
L1F263:;
    l_18 = ((int)(short)*(short *)D_00190D64) * 7;
L1F270:;
    if (guild_is_local_knight() == 0) goto L1F2C0;
    msgbox_show_string((int)D_00170572, 1);
    *(signed char *)tavern_state = 0;
    if (tavern_room_rented() == 0) goto L1F2AC;
    tavern_extend_room(((int)(short)*(short *)D_00190D64) * 1440);
    goto L1F2BE;
L1F2AC:;
    tavern_rent(((int)(short)*(short *)D_00190D64) * 1440);
L1F2BE:;
    return;
L1F2C0:;
    if (l_18 != 0) goto L1F30D;
    msgbox_show_string((int)D_0017059D, 1);
    *(signed char *)tavern_state = 0;
    if (tavern_room_rented() == 0) goto L1F2F9;
    tavern_extend_room(((int)(short)*(short *)D_00190D64) * 1440);
    goto L1F30B;
L1F2F9:;
    tavern_rent(((int)(short)*(short *)D_00190D64) * 1440);
L1F30B:;
    return;
L1F30D:;
    trade_adjust_price(l_18, 0);
    func_00097A85();
    *(signed char *)tavern_state = 2;
}

void tavern_room_pay(void)
{
    *(signed char *)tavern_state = 0;
    if (((int)(unsigned char)*(signed char *)D_00196271) == 2) return;
    if (((unsigned)player_character->gold) >= *(int *)D_00195D30) goto L1F372;
    msgbox_show_rsc(454, 1);
    return;
L1F372:;
    player_character->gold -= *(int *)D_00195D30;
    if (tavern_room_rented() == 0) goto L1F3A0;
    tavern_extend_room(((int)(short)*(short *)D_00190D64) * 1440);
    return;
L1F3A0:;
    tavern_rent(((int)(short)*(short *)D_00190D64) * 1440);
}

void tavern_food_button(void)
{
    if (((unsigned)(*(int *)game_minutes - player_character->last_meal_time)) >= 240) goto L1F3F3;
    msgbox_show_string((int)D_001705BE, 1);
    return;
L1F3F3:;
    picklist_open((int)tavern_food_names);
    *(signed char *)D_001940D4 |= 1;
}

void tavern_buy_food(int a1)
{
    int l_1C;
    int l_18;

    l_1C = (int)(unsigned char)*(signed char *)(tavern_food_prices + a1);
    l_18 = holiday_today(*(int *)game_minutes, (int)(unsigned char)*(signed char *)current_region);
    if (l_18 != 37) goto L1F457;
    l_1C >>= 1;
    if (l_1C != 0) goto L1F457;
    l_1C++;
L1F457:;
    if (l_18 == 1) goto L1F46D;
    if (((unsigned)player_character->gold) < l_1C) goto L1F46F;
L1F46D:;
    goto L1F480;
L1F46F:;
    msgbox_show_rsc(454, 1);
    return;
L1F480:;
    if (l_18 == 1) goto L1F495;
    player_character->gold -= l_1C;
L1F495:;
    player_character->health += l_1C * 2;
    if (player_character->health <= player_character->max_health) goto L1F4CB;
    player_character->health = player_character->max_health;
L1F4CB:;
    player_character->last_meal_time = *(int *)game_minutes;
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
    tavern_building->rent_expires = *(int *)game_minutes + a1;
    tavern_building->flags &= 248;
    tavern_building->flags |= 2;
    tavern_building->room = l_18->owner;
}

int tavern_room_rented(void)
{
    int l_1C;

    if (((int)(unsigned char)(tavern_building->flags & 2)) == 0) goto L1F62E;
    if (((unsigned)tavern_building->rent_expires) > *(int *)game_minutes) goto L1F630;
L1F62E:;
    goto L1F639;
L1F630:;
    l_1C = 1;
    goto L1F640;
L1F639:;
    l_1C = 0;
L1F640:;
    return l_1C;
}

int tavern_room_days_left(void)
{
    if (((int)(unsigned char)(tavern_building->flags & 2)) != 0) goto L1F67D;
    return 0;
L1F67D:;
    if (tavern_room_rented() != 0) goto L1F68F;
    return 0;
L1F68F:;
    return ((unsigned)((tavern_building->rent_expires - *(int *)game_minutes) + 1439)) / 1440;
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
    l_18->repair_due = *(int *)D_00190BE4;
    mc_memcpy(RECORD_DATA(l_18), RECORD_DATA(a1), 107, (int)D_00170569, 322, 4);
}

void func_0001F7B3(struct record *a1)
{
    struct record *l_18;

    if (a1->type != 58) return;
    if ((((unsigned)a1->id) >> 16) != (((unsigned)D_00195AC4->id) >> 16)) return;
    if ((*(int *)D_00199770 = object_find_by_id(D_00195AC4, a1->parent_id)) == 0) return;
    l_18 = object_create_child((struct record *)*(int *)D_00199770, 0, 107);
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
    D_00196ABC = (struct building *)*(int *)D_00195C44;
    *(int *)D_00195B84 = 0;
    func_0001FB3F();
    func_0001FAB2();
    if (*(int *)D_00195B84 == 0) goto L1F93F;
    object_delete(D_00195AF4);
    l_18 = object_create_child(nonworld_root, 0, *(int *)D_00195B84 * 26);
    l_18->type = 57;
    l_18->owner = *(short *)D_00195B84;
    l_18->id = D_00195AC4->id;
    mc_memcpy(RECORD_DATA(l_18), (int)D_00196ABC, *(int *)D_00195B84 * 26, (int)D_00170569, 369, 4);
L1F93F:;
    object_foreach(D_00195AC4, (int)func_0001F6E2);
}

void func_0001F958(void)
{
    int l_1C;
    int l_18;

    func_0001FE4D();
    D_00196ABC = (struct building *)*(int *)D_00195C44;
    *(int *)D_00195B84 = 0;
    func_0001FAB2();
    l_1C = 0;
L1F98B:;
    if (l_1C < *(int *)D_00195B84) goto L1F9A3;
    goto L1FA21;
L1F99B:;
    l_1C++;
    goto L1F98B;
L1F9A3:;
    l_18 = 0;
L1F9AA:;
    if (current_location->building_count > l_18) goto L1F9C7;
    goto L1FA1C;
L1F9BF:;
    l_18++;
    goto L1F9AA;
L1F9C7:;
    if (current_location->buildings[l_18].id != D_00196ABC[l_1C].id) goto L1FA1A;
    mc_memcpy((int)&current_location->buildings[l_18], (int)&D_00196ABC[l_1C], 26, (int)D_00170569, 388, 4);
    goto L1FA1C;
L1FA1A:;
    goto L1F9BF;
L1FA1C:;
    goto L1F99B;
L1FA21:;
    object_foreach(nonworld_root, (int)func_0001F7B3);
}

int func_0001FA3A(struct record *a1)
{
    if (D_00195AF4 == 0) goto L1FA5D;
    return 0;
L1FA5D:;
    if (a1->type == 57) goto L1FA75;
    return 0;
L1FA75:;
    if ((((unsigned)a1->id) >> 16) != (((unsigned)D_00195AC4->id) >> 16)) goto L1FA9E;
    D_00195AF4 = a1;
    return 1;
L1FA9E:;
    return 0;
}

void func_0001FAB2(void)
{
    *(int *)D_00195B84 = 0;
    D_00195AF4 = 0;
    object_find(nonworld_root, (int)func_0001FA3A);
    if (D_00195AF4 == 0) return;
    mc_memcpy((int)&D_00196ABC[*(int *)D_00195B84], (int)RECORD_DATA(D_00195AF4), D_00195AF4->owner * 26, (int)D_00170569, 417, 4);
    *(int *)D_00195B84 += D_00195AF4->owner;
}

void func_0001FB3F(void)
{
    int l_18;

    l_18 = 0;
L1FB54:;
    if (current_location->building_count > l_18) goto L1FB74;
    return;
L1FB6C:;
    l_18++;
    goto L1FB54;
L1FB74:;
    if ((current_location->buildings[l_18].flags & 3) == 0) goto L1FBAE;
    if (((unsigned)current_location->buildings[l_18].flags) > *(int *)game_minutes) goto L1FBB0;
L1FBAE:;
    goto L1FBE9;
L1FBB0:;
    mc_memcpy((int)&D_00196ABC[(*(int *)D_00195B84)++], (int)&current_location->buildings[l_18], 26, (int)D_00170569, 428, 4);
L1FBE9:;
    goto L1FB6C;
}

int func_0001FBF5(struct record *a1)
{
    int l_20;
    struct building *l_1C;

    if (a1->parent->type == 43) goto L1FC24;
    return 0;
L1FC24:;
    if (a1->quest_id == 0) goto L1FC39;
    return 0;
L1FC39:;
    if ((((unsigned)a1->id) >> 16) != (((unsigned)D_00195AC4->id) >> 16)) goto L1FC5D;
    return 0;
L1FC5D:;
    l_1C = 0;
    l_20 = 0;
L1FC6B:;
    if (current_location->building_count > l_20) goto L1FC88;
    goto L1FCB7;
L1FC80:;
    l_20++;
    goto L1FC6B;
L1FC88:;
    if (a1->parent->id != current_location->buildings[l_20].id) goto L1FCB5;
    l_1C = &current_location->buildings[l_20];
L1FCB5:;
    goto L1FC80;
L1FCB7:;
    if (l_1C != 0) goto L1FCC9;
    return 0;
L1FCC9:;
    if (l_1C->type == 15) goto L1FCE9;
    if (l_1C->type != 1) goto L1FCEB;
L1FCE9:;
    goto L1FCF7;
L1FCEB:;
    return 0;
L1FCF7:;
    *(int *)D_00190BE4 = 2147483647;
    if (l_1C->type != 1) goto L1FD22;
    if (l_1C->id == player_character->house) goto L1FD24;
L1FD22:;
    goto L1FD2D;
L1FD24:;
    return 1;
L1FD2D:;
    *(int *)D_00190BE4 = l_1C->rent_expires;
    return (((((int)(unsigned char)(l_1C->flags & 2)) != 0) && (((unsigned)l_1C->rent_expires) > *(int *)game_minutes)) ? 1 : 0);
}

void func_0001FD7C(struct record *a1)
{
    int l_1C;
    int l_18;

    if (a1->type == 57) goto L1FDAB;
    if (a1->type != 58) goto L1FDAD;
L1FDAB:;
    goto L1FDB2;
L1FDAD:;
    return;
L1FDB2:;
    if (a1->type != 57) goto L1FE2D;
    D_00196ABC = (struct building *)RECORD_DATA(a1);
    l_18 = a1->owner;
    l_1C = 0;
L1FDE3:;
    if (a1->owner > l_1C) goto L1FE05;
    goto L1FE1D;
L1FDF6:;
    l_1C++;
    D_00196ABC++;
    goto L1FDE3;
L1FE05:;
    if (((unsigned)D_00196ABC->rent_expires) > *(int *)game_minutes) goto L1FE1B;
    l_18--;
L1FE1B:;
    goto L1FDF6;
L1FE1D:;
    if (l_18 != 0) goto L1FE2B;
    object_delete(a1);
L1FE2B:;
    return;
L1FE2D:;
    if (((unsigned)a1->repair_due) > *(int *)game_minutes) return;
    object_delete(a1);
}

void func_0001FE4D(void)
{
    object_foreach(nonworld_root, (int)func_0001FD7C);
}

void func_0001FE74(void)
{
    *(int *)politic_pak = disk_read_file((int)D_001705D4, 0);
    *(int *)climate_pak = disk_read_file((int)D_001705E0, 0);
    disk_read_file((int)D_001705EC, (int)region_flats);
}
