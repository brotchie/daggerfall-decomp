/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
extern char mouse_buttons[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char D_001343C0[];
extern char D_001343C2[];
extern char key_down[];
extern char key_down_esc[];
extern char screen_buffer[];
extern char D_00147954[];
extern char D_00176A10[];
extern char D_00176A1A[];
extern char D_00176A27[];
extern char D_00176A42[];
extern char player_environment[];
extern char fire_flat_records[];
extern char saved_positions[];
extern char D_0018DE18[];
extern char D_0018DE1C[];
extern char text_buffer[];
extern char key_was_down[];
extern struct record *D_00190504[];
extern char D_00190C74[];
extern char D_00190C7C[];
extern char D_001940D4[];
extern char D_001940D5[];
extern char D_001940D6[];
extern char D_001940DA[];
extern char D_001940E4[];
extern struct record *D_001959E0;
extern struct record *D_001959EC;
extern struct record *D_001959F0;
extern struct record *D_001959F4;
extern struct record *D_001959F8;
extern struct record *detect_target;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern char D_00195AB0[];
extern struct record *D_00195AC4;
extern struct building *tavern_building;
extern struct record *D_00195AF4;
extern char picklist_image[];
extern char D_00195B00[];
extern char creature_count[];
extern struct record *guild_npc_object;
extern char D_00195B84[];
extern char hud_message_expiry[];
extern char hud_status_expiry[];
extern struct location *current_location;
extern struct character *player_character;
extern char game_minutes[];
extern char hud_message_ptrs[];
extern char D_00195C3C[];
extern char D_00195C44[];
extern char D_00195CD0[];
extern char D_00195CF0[];
extern char D_00195D5C[];
extern char free_later_count[];
extern char D_00195F3C[];
extern char D_00195F3E[];
extern char D_00195F40[];
extern char D_00195F42[];
extern char D_00195F60[];
extern char picklist_control[];
extern char msgbox_button_keys[];
extern char D_00196034[];
extern char msgbox_button_ids[];
extern char D_00196090[];
extern char D_00196091[];
extern char D_0019626F[];
extern char D_00196271[];
extern char D_00196272[];
extern char game_mode[];
extern char D_00196275[];
extern char D_00196281[];
extern char D_00196282[];
extern char D_00196284[];
extern struct record *people_list[];
extern char people_count[];
extern struct quest *current_quest;
extern char nearest_fire_distance[];
extern struct record *nearest_fire;
extern char D_001A3F40[];
extern char D_001A4FE8[];
extern char D_001A4FEC[];
extern char free_later_list[];
extern char mode_stack[];
extern char D_001A53E9[];
extern char D_001A5408[];
extern char hud_message_text[];
extern char hud_status_text[];
extern char D_001A59C8[];
extern char D_001A59CC[];
extern char D_001A59D0[];
extern char D_001A59D4[];
extern char D_001A59D8[];
extern char D_001A59DC[];
extern char D_001A59E0[];
extern char D_001A59E4[];
extern char info_popup_text[];
extern char mode_stack_depth[];
extern char D_001A5A54[];

extern int tavern_room_rented(void);
extern int func_0002586B(struct record *);
extern int place_spawn_from_marker(struct record *);
extern struct record *item_add_to_container(int, int, int, int);
extern int object_weight(struct record *);
extern int spell_missile_update(struct record *, int);
extern int disk_read_file(int, int);
extern int location_contains(int, int);
extern int building_name(struct building *);
extern int picklist_poll(int);
extern int object_free_single(struct record *);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern int object_find(struct record *, int);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern int rand();
extern int mc_memset();
extern int mc_strncpy();
extern int func_000A0DF4();
extern int toupper();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int memchr();
extern int int386x();
extern int func_000C7FD9();
extern int func_000C7FF4();
extern int func_000C808D();
extern int func_000CE8B2();
extern int func_0012B136();
extern int func_0012B2D3();
extern int func_0012DB50();
extern int func_00135DE4();
extern int func_00135E39();
extern int func_001427A8();
extern int func_00144F68();
extern void msgbox_show_string(int, int);
extern void msgbox_show_quest_text(struct quest *, short, int);
extern void msgbox_show_rsc(int, int);
extern void holiday_announce(void);
extern void cast_spell_on(int, int, int);
extern void text_draw_colored(int, int, int, int, unsigned char);
extern void text_draw_centered_colored(int, int, int, int, unsigned char);
extern void detect_consider_creature(struct record *, unsigned short);
extern void func_0007E815(struct record *, int);
extern void func_0007F0F3(short);
extern void func_0008591A(struct record *, int);
extern void func_00085992(struct record *, int);
extern void inpstr_begin_number(int);
extern void picklist_init(int, short, short, int, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void picklist_add(int, int, int);
extern void picklist_free(int);
extern void picklist_draw(int, int);
extern void object_free_children(struct record *);
extern void object_foreach_pre(struct record *, int);
extern void object_foreach(struct record *, int);
extern void func_0008E447(struct record *, int);
extern void object_foreach_open(struct record *, int);
int hud_message_add(int);
int picklist_frame(int);
struct building *object_building(struct record *);
int gold_total(void);
int gold_find_credit_cb(struct record *);
int gold_spend_credit_cb(struct record *);
struct record *gold_find_credit(int);
int carry_capacity(void);
void object_free_later(struct record *);
void object_free_pending(void);
void spell_cast_queued_run(void);
void world_collect_object(struct record *);
void detect_consider(struct record *);
void func_0007EB0B(struct record *, int);
void func_0007F093(struct record *);
void gold_sum_credit_cb(struct record *);
void gold_make_credit_letter(int);
void gold_delete_credit_cb(struct record *);
void func_0007F671(void);
void func_0007FCBF(struct record *);
void func_0007FD7E(struct record *);
void func_0007FDEA(struct record *);
void func_0007FEB9(struct record *);
void func_0007FF73(struct record *);
void func_0007FFE7(struct record *);
#pragma aux func_000A0ED9 parm routine [];

void hud_status_set(int a1)
{
    int l_18;

    mc_strncpy((int)hud_status_text, a1, 1440, (int)D_00176A10, 97);
    l_18 = 1132;
    *(int *)hud_status_expiry = *(int *)((char *)l_18) + 36;
    *(int *)D_00195C3C = (int)hud_status_text;
}

int hud_message_add(int a1)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    short l_18;

    l_28 = (int)hud_message_text;
    l_20 = -1;
    l_24 = 0;
L7CBC7:;
    if (l_24 < 16) goto L7CBDB;
    goto L7CBFA;
L7CBCF:;
    l_24++;
    l_28 += 80;
    goto L7CBC7;
L7CBDB:;
    if (*(int *)(hud_message_expiry + (l_24 << 2)) != 0) goto L7CBF8;
    l_20 = l_24;
    l_2C = l_28;
    goto L7CBFA;
L7CBF8:;
    goto L7CBCF;
L7CBFA:;
    if (l_20 >= 0) goto L7CC09;
    return 0;
L7CC09:;
    mc_strncpy(l_2C, a1, 4, (int)D_00176A10, 117);
    *(int *)&l_18 = 1132;
    *(int *)(hud_message_expiry + (l_20 << 2)) = *(int *)(*(char **)&l_18) + 36;
    *(int *)(hud_message_ptrs + (l_20 << 2)) = l_2C;
    return *(int *)(hud_message_ptrs + (l_24 << 2));
}

void hud_messages_draw(void)
{
    int l_24;
    int l_20;

    func_0012DB50(4);
    l_24 = 0;
    l_20 = 2;
L7CC8C:;
    if (l_24 < 16) goto L7CCA3;
    goto L7CD0C;
L7CC97:;
    l_24++;
    l_20 += 9;
    goto L7CC8C;
L7CCA3:;
    if (*(int *)(hud_message_expiry + (l_24 << 2)) == 0) goto L7CD0A;
    if (((struct bf8_0_1 *)&D_001940DA)->f != 0) goto L7CCE5;
    if (((unsigned)*(int *)((char *)1132)) <= *(int *)(hud_message_expiry + (l_24 << 2))) goto L7CCE5;
    *(int *)(hud_message_expiry + (l_24 << 2)) = 0;
L7CCE5:;
    text_draw_centered_colored(*(int *)(hud_message_ptrs + (l_24 << 2)), 160, (int)(short)*(short *)&l_20, 145, 156);
L7CD0A:;
    goto L7CC97;
L7CD0C:;
    if (*(int *)hud_status_expiry == 0) return;
    if (((struct bf8_0_1 *)&D_001940DA)->f != 0) goto L7CD3C;
    if (((unsigned)*(int *)((char *)1132)) <= *(int *)hud_status_expiry) goto L7CD3C;
    *(int *)hud_status_expiry = 0;
L7CD3C:;
    text_draw_centered_colored(*(int *)D_00195C3C, 160, (int)(short)*(short *)&l_20, 145, 156);
}

void info_popup_open(int a1)
{
    *(signed char *)D_00196275 = *(signed char *)D_00196272;
    *(signed char *)D_00196272 = 2;
    *(int *)info_popup_text = a1;
}

int wait_key_from_list(int a1, short a2)
{
    unsigned char l_14;
    short l_18;

L7CF80:;
    if (*(signed char *)mouse_buttons == 0) goto L7CF90;
    func_0012B136();
    goto L7CF80;
L7CF90:;
    if (*(signed char *)mouse_buttons != 0) goto L7D055;
    func_0012B136();
    func_0012B2D3((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y);
    l_14 = func_001427A8();
    if (l_14 == 0) goto L7D02F;
    if (((struct bf8_0_1 *)&D_001940D4)->f == 0) goto L7CFDA;
    if (((int)(unsigned char)l_14) == 27) goto L7CFDC;
L7CFDA:;
    goto L7CFE8;
L7CFDC:;
    return -1;
L7CFE8:;
    if (a2 == 0) goto L7D02F;
    l_14 = toupper((int)(unsigned char)l_14);
    *(int *)&l_18 = 0;
L7D003:;
    if ((short)(short)*(int *)&l_18 < a2) goto L7D016;
    goto L7D02F;
L7D00E:;
    (*(int *)&l_18)++;
    goto L7D003;
L7D016:;
    if ((signed char)l_14 != *(signed char *)((char *)(((int)(short)l_18) + a1))) goto L7D02D;
    return (int)(short)l_18;
L7D02D:;
    goto L7D00E;
L7D02F:;
    mc_memcpy(655360, *(int *)screen_buffer, 64000, (int)D_00176A10, 234, 4);
    goto L7CF90;
L7D055:;
    return -2;
}

int key_pressed_once(unsigned char a1)
{
    if (*(signed char *)(key_down + ((int)(unsigned char)a1)) == 0) goto L7D095;
    if (*(signed char *)(key_was_down + ((int)(unsigned char)a1)) == 0) goto L7D097;
L7D095:;
    goto L7D0AC;
L7D097:;
    *(signed char *)(key_was_down + ((int)(unsigned char)a1)) = 1;
    return 1;
L7D0AC:;
    if (*(signed char *)(key_down + ((int)(unsigned char)a1)) != 0) goto L7D0C6;
    *(signed char *)(key_was_down + ((int)(unsigned char)a1)) = 0;
L7D0C6:;
    return 0;
}

void mode_push(void)
{
    *(signed char *)(mode_stack + (((int)(short)*(short *)mode_stack_depth) * 2)) = *(signed char *)D_00196272;
    *(signed char *)(D_001A53E9 + (((int)(short)(*(short *)mode_stack_depth)++) * 2)) = *(signed char *)game_mode;
    *(signed char *)D_0019626F = *(signed char *)game_mode;
}

void mode_pop(void)
{
    (*(short *)mode_stack_depth)--;
    *(signed char *)D_00196272 = *(signed char *)(mode_stack + (((int)(short)*(short *)mode_stack_depth) * 2));
    *(signed char *)game_mode = *(signed char *)(D_001A53E9 + (((int)(short)*(short *)mode_stack_depth) * 2));
    if (*(short *)mode_stack_depth == 0) goto L7D18A;
    *(signed char *)D_0019626F = *(signed char *)(D_001A53E9 + (((int)(short)*(short *)mode_stack_depth) * 2));
    return;
L7D18A:;
    *(signed char *)D_0019626F = 255;
}

void func_0007D19B(int a1, int a2, short a3, short a4)
{
    short l_10;
    short l_C;

    if (*(short *)((char *)a1) > a3) goto L7D1CA;
    if (*(short *)((char *)a2) <= a4) goto L7D1CC;
L7D1CA:;
    goto L7D1D1;
L7D1CC:;
    return;
L7D1D1:;
    *(int *)&l_10 = (((int)(short)a3) << 8) / ((int)(short)*(short *)((char *)a1));
    *(int *)&l_C = (((int)(short)a4) << 8) / ((int)(short)*(short *)((char *)a2));
    if ((short)(short)*(int *)&l_C >= l_10) goto L7D229;
    *(short *)((char *)a1) = (((int)(short)*(short *)((char *)a1)) * ((int)(short)l_C)) >> 8;
    *(short *)((char *)a2) = *(int *)&a4;
    return;
L7D229:;
    *(short *)((char *)a2) = (((int)(short)*(short *)((char *)a2)) * ((int)(short)l_10)) >> 8;
    *(short *)((char *)a1) = *(int *)&a3;
}

void picklist_open_strings(int a1)
{
    if (*(int *)picklist_image != 0) goto L7D27A;
    *(int *)picklist_image = disk_read_file((int)D_00176A1A, 0);
L7D27A:;
    *(short *)D_00195F40 = 160 - (((int)(unsigned short)*(short *)(*(char **)picklist_image + 4)) >> 1);
    *(short *)D_00195F3E = 100 - (((int)(unsigned short)*(short *)(*(char **)picklist_image + 6)) >> 1);
    *(short *)D_00195F42 = *(short *)(*(char **)picklist_image + 4);
    *(short *)D_00195F3C = *(short *)(*(char **)picklist_image + 6);
    picklist_init((int)picklist_control, (int)(short)(*(short *)D_00195F40 + 25), (int)(short)(*(short *)D_00195F3E + 26), 140, 73, (int)(short)(*(short *)D_00195F40 + 179), (int)(short)(*(short *)D_00195F3E + 11), 8, 9, (int)(short)(*(short *)D_00195F40 + 179), (int)(short)(*(short *)D_00195F3E + 109), 8, 9, (int)(short)(*(short *)D_00195F40 + 179), (int)(short)(*(short *)D_00195F3E + 22), 9, 83, 146, 146, 244, 114, 0);
L7D394:;
    if (*(signed char *)((char *)a1) == 0) goto L7D3B9;
    picklist_add((int)picklist_control, a1, 0);
    a1 += func_000A0DF4(a1) + 1;
    goto L7D394;
L7D3B9:;
    *(signed char *)D_001940D4 |= 4;
}

void picklist_open(int a1)
{
    if (*(int *)picklist_image != 0) goto L7D3F5;
    *(int *)picklist_image = disk_read_file((int)D_00176A1A, 0);
L7D3F5:;
    *(short *)D_00195F40 = 160 - (((int)(unsigned short)*(short *)(*(char **)picklist_image + 4)) >> 1);
    *(short *)D_00195F3E = 100 - (((int)(unsigned short)*(short *)(*(char **)picklist_image + 6)) >> 1);
    *(short *)D_00195F42 = *(short *)(*(char **)picklist_image + 4);
    *(short *)D_00195F3C = *(short *)(*(char **)picklist_image + 6);
    picklist_init((int)picklist_control, (int)(short)(*(short *)D_00195F40 + 25), (int)(short)(*(short *)D_00195F3E + 26), 140, 73, (int)(short)(*(short *)D_00195F40 + 179), (int)(short)(*(short *)D_00195F3E + 11), 8, 9, (int)(short)(*(short *)D_00195F40 + 179), (int)(short)(*(short *)D_00195F3E + 109), 8, 9, (int)(short)(*(short *)D_00195F40 + 179), (int)(short)(*(short *)D_00195F3E + 22), 9, 83, 146, 146, 244, 114, 0);
L7D50F:;
    if (*(int *)((char *)a1) == 0) goto L7D52E;
    picklist_add((int)picklist_control, *(int *)((char *)(int)(*(char (**)[4])&a1)++), 0);
    goto L7D50F;
L7D52E:;
    *(signed char *)D_001940D4 |= 4;
}

int picklist_frame(int a1)
{
    short l_18;

    if (((struct bf8_0_1 *)&D_001940D4)->f == 0) goto L7D574;
    if (*(signed char *)key_down_esc != 0) goto L7D572;
    if (((int)(unsigned char)(*(signed char *)mouse_buttons & 2)) == 0) goto L7D574;
L7D572:;
    goto L7D576;
L7D574:;
    goto L7D58E;
L7D576:;
    *(signed char *)D_001940D4 &= 251;
    picklist_free(a1);
    return -2;
L7D58E:;
    *(int *)&l_18 = picklist_poll(a1) - 1;
    if (((int)(short)l_18) <= (-1)) goto L7D5BB;
    *(signed char *)D_001940D4 &= 251;
    picklist_free(a1);
    return (int)(short)l_18;
L7D5BB:;
    picklist_draw(a1, 0);
    return -1;
}

int picklist_update(void)
{
    func_00144F68((int)(short)*(short *)D_00195F40, (int)(short)*(short *)D_00195F3E, (int)(short)*(short *)D_00195F42, (int)(short)*(short *)D_00195F3C, *(int *)picklist_image + 12);
    return picklist_frame((int)picklist_control);
}

int rand_range(int a1, int a2)
{
    return a1 + (rand() % ((a2 - a1) + 1));
}

void object_free_later(struct record *a1)
{
    if (a1 == 0) return;
    *(int *)(free_later_list + ((*(int *)free_later_count)++ << 2)) = (int)a1;
}

void object_free_pending(void)
{
    int l_18;

    l_18 = 0;
L7D738:;
    if (l_18 < *(int *)free_later_count) goto L7D74D;
    goto L7D760;
L7D745:;
    l_18++;
    goto L7D738;
L7D74D:;
    object_delete((struct record *)*(int *)(free_later_list + (l_18 << 2)));
    goto L7D745;
L7D760:;
    *(int *)free_later_count = 0;
}

void func_0007D774(int a1, int a2)
{
    *(int *)(D_001A4FE8 + (*(int *)D_00195D5C << 3)) = a1;
    *(int *)(D_001A4FEC + ((*(int *)D_00195D5C)++ << 3)) = a2;
}

void spell_cast_queued_run(void)
{
    int l_18;

    l_18 = 0;
L7D7CF:;
    if (l_18 < *(int *)D_00195D5C) goto L7D7E4;
    goto L7D805;
L7D7DC:;
    l_18++;
    goto L7D7CF;
L7D7E4:;
    cast_spell_on(*(int *)(D_001A4FE8 + (l_18 << 3)), *(int *)(D_001A4FEC + (l_18 << 3)), 0);
    goto L7D7DC;
L7D805:;
    *(int *)D_00195D5C = 0;
}

void world_collect_object(struct record *a1)
{
    struct item *l_1C;
    int l_18;

    switch (a1->type) {
case 33:
    if (((struct bf8_4_1 *)&D_001940D6)->f == 0) goto L7D8AE;
    if (player_character->detect_kind == 2) goto L7D8B0;
L7D8AE:;
    goto L7D8E1;
L7D8B0:;
    if ((a1->image >> 7) == 216) goto L7D8D9;
    if (a1->image != 26112) goto L7D8E1;
L7D8D9:;
    detect_consider(a1);
L7D8E1:;
    if ((a1->image >> 7) != 210) goto L7D91B;
    if (memchr((int)fire_flat_records, (int)(unsigned short)(a1->image & 127), 8) != 0) goto L7D91D;
L7D91B:;
    goto L7D973;
L7D91D:;
    l_18 = func_000C7FF4(player_object->y - a1->y, func_000C7FD9(player_object->x, player_object->z, a1->x, a1->z));
    if (l_18 >= *(int *)nearest_fire_distance) goto L7D973;
    *(int *)nearest_fire_distance = l_18;
    nearest_fire = a1;
L7D973:;
    return;
case 18:
    D_00190504[(*(int *)creature_count)++] = a1;
    if (func_0002586B(a1) == 0) goto L7D9A4;
    (*(int *)creature_count)--;
    goto L7D9D5;
L7D9A4:;
    if (((struct bf8_4_1 *)&D_001940D6)->f == 0) goto L7D9C2;
    if (player_character->detect_kind == 1) goto L7D9C4;
L7D9C2:;
    goto L7D9D5;
L7D9C4:;
    detect_consider_creature(a1, a1->detect_distance);
L7D9D5:;
    return;
case 2:
    l_1C = &a1->data.item;
    if (((struct bf8_4_1 *)&D_001940D6)->f == 0) goto L7D9FA;
    if (player_character->detect_kind == 0) goto L7D9FC;
L7D9FA:;
    goto L7DA08;
L7D9FC:;
    if (l_1C->enchantments[0].type != (-1)) goto L7DA0A;
L7DA08:;
    goto L7DA1B;
L7DA0A:;
    if (a1->parent->parent != player_entity) goto L7DA1D;
L7DA1B:;
    goto L7DA25;
L7DA1D:;
    detect_consider(a1);
L7DA25:;
    return;
case 9:
    if (((int)(unsigned short)(a1->flags & 8192)) == 0) goto L7DA55;
    if (spell_missile_update(a1, 0) == 0) goto L7DA55;
    object_free_later(a1);
L7DA55:;
    return;
case 34:
    switch (((int)(unsigned short)(a1->image & 31)) - 2) {
case 13:
case 14:
    if (a1->spawn_seed != 0) goto L7DABA;
    a1->spawn_seed = rand();
L7DABA:;
    place_spawn_from_marker(a1);
    goto L7DB53;
case 18:
    if (((int)(unsigned char)*(signed char *)player_environment) != 3) goto L7DADF;
    func_0008591A(a1, 0);
    goto L7DB21;
L7DADF:;
    if (((int)(unsigned char)*(signed char *)player_environment) != 2) goto L7DAFD;
    if (current_building->type >= 17) goto L7DAFF;
L7DAFD:;
    goto L7DB11;
L7DAFF:;
    if (current_building->type <= 20) goto L7DB13;
L7DB11:;
    goto L7DB21;
L7DB13:;
    func_0008591A(a1, (int)current_building);
L7DB21:;
    goto L7DB53;
case 17:
    func_00085992(a1, ((((int)(unsigned char)*(signed char *)player_environment) == 2) ? (int)object_building(player_object->parent) : 0));
default:
L7DB53:;
    return;
}
case 53:
    people_list[(*(int *)people_count)++] = a1;
default:;
}
}

void detect_consider(struct record *a1)
{
    int l_18;

    l_18 = func_000C7FD9(player_object->x, player_object->z, a1->x, a1->z);
    if (l_18 >= *(int *)D_00195B84) goto L7DBC0;
    if (l_18 < 2048) goto L7DBC2;
L7DBC0:;
    return;
L7DBC2:;
    detect_target = a1;
    *(int *)D_00195B84 = l_18;
}

void world_collect_objects(void)
{
    struct record *l_1C;
    struct record *l_18;

    nearest_fire = 0;
    *(int *)nearest_fire_distance = 2048;
    if (((int)(unsigned char)*(signed char *)player_environment) != 2) goto L7DC65;
    current_building = object_building(player_object);
L7DC65:;
    *(int *)D_00195D5C = (*(int *)free_later_count = 0);
    *(int *)D_00195B84 = 100000;
    *(int *)people_count = (*(int *)creature_count = 0);
    detect_target = 0;
    *(int *)D_00195CD0 = (int)object_foreach_pre;
    if (((int)(unsigned char)*(signed char *)player_environment) >= 3) goto L7DD53;
    if (player_object->parent->type == 1) goto L7DCE6;
    object_foreach_pre(player_object->parent->children, (int)world_collect_object);
    goto L7DCFF;
L7DCE6:;
    *(int *)D_00195CD0 = (int)object_foreach_open;
    func_0007EB0B(player_object, (int)world_collect_object);
L7DCFF:;
    l_1C = D_00195AC4->children;
L7DD0A:;
    if (l_1C == 0) goto L7DD51;
    l_18 = l_1C->next;
    if (l_1C->type == 38) goto L7DD49;
    if (l_1C->children == 0) goto L7DD41;
    object_foreach_open(l_1C->children, (int)world_collect_object);
L7DD41:;
    world_collect_object(l_1C);
L7DD49:;
    l_1C = l_18;
    goto L7DD0A;
L7DD51:;
    goto L7DDAB;
L7DD53:;
    func_0007E815(player_object, (int)world_collect_object);
    l_1C = D_00195AC4->children;
L7DD6D:;
    if (l_1C == 0) goto L7DDAB;
    l_18 = l_1C->next;
    if (l_1C->type == 47) goto L7DDA3;
    object_foreach_open(l_1C->children, (int)world_collect_object);
    world_collect_object(l_1C);
L7DDA3:;
    l_1C = l_18;
    goto L7DD6D;
L7DDAB:;
    *(int *)D_00195CD0 = (int)object_foreach_open;
    spell_cast_queued_run();
    object_free_pending();
}

void msgbox_yes_no_quest(short a1)
{
    *(signed char *)D_00196271 = 0;
    *(signed char *)msgbox_button_ids = 4;
    *(signed char *)D_00196090 = 5;
    *(signed char *)D_00196091 = 0;
    *(signed char *)msgbox_button_keys = 21;
    *(signed char *)D_00196034 = 49;
    *(signed char *)D_0012B508 = 146;
    msgbox_show_quest_text(current_quest, (int)(short)a1, 5);
}

void msgbox_prompt_number(int a1, int a2)
{
    int l_14;

    *(signed char *)D_0012B508 = 146;
    l_14 = *(int *)D_00195C44 + 55000;
    func_000A0ED9(610, (int)D_00176A10);
    mc_sprintf(l_14, (int)D_00176A27, a2);
    *(signed char *)((char *)(func_000A0DF4(l_14) + l_14) + 1) = 0;
    inpstr_begin_number(a1);
    msgbox_show_string(l_14, 2);
}

void msgbox_prompt_number_rsc(int a1, int a2)
{
    int l_14;

    *(signed char *)D_0012B508 = 146;
    msgbox_show_rsc((int)(short)*(short *)&a2, 2);
    inpstr_begin_number(a1);
}

struct building *object_building(struct record *a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_28 = 1;
    if (a1 == 0) goto L7DF5C;
    if ((int)a1 != (-1768515946)) goto L7DF68;
L7DF5C:;
    return 0;
L7DF68:;
    if (a1 == 0) goto L7DF7D;
    if (a1->type != 1) goto L7DF7F;
L7DF7D:;
    goto L7DFB2;
L7DF7F:;
    if (a1->type != 43) goto L7DFA3;
    if (((int)(unsigned short)(a1->flags & 1)) != 0) goto L7DFA5;
L7DFA3:;
    goto L7DFA7;
L7DFA5:;
    goto L7DFB2;
L7DFA7:;
    a1 = a1->parent;
    goto L7DF68;
L7DFB2:;
    if (a1 == 0) goto L7DFC7;
    if (a1->type != 1) goto L7DFC9;
L7DFC7:;
    goto L7DFDC;
L7DFC9:;
    if (a1->image != 65535) goto L7DFE5;
L7DFDC:;
    return 0;
L7DFE5:;
    return &current_location->buildings[a1->image];
}

int func_0007E00E(struct record *a1, struct record *a2, int a3)
{
    return ((func_000C7FD9(a1->x, a1->z, a2->x, a2->z) < a3) ? 1 : 0);
}

void func_0007E066(void)
{
    func_0007F671();
    *(int *)D_001A59D0 = *(int *)D_00195B00;
    *(int *)D_001A59C8 = *(int *)D_00195B00;
}

void frame_ticks_update(void)
{
    if ((*(int *)D_00195AB0 = (*(int *)D_001A3F40 * 1828) / 256) <= 100) goto L7E0D4;
    *(int *)D_00195AB0 = 100;
L7E0D4:;
    *(int *)D_001343C0 += *(int *)D_00195AB0;
    *(short *)D_001343C2 &= 7;
    *(int *)D_001A3F40 = 0;
}

void player_position_save(int a1)
{
    *(int *)(saved_positions + (a1 * 12)) = player_object->x;
    *(int *)(D_0018DE18 + (a1 * 12)) = player_object->y;
    *(int *)(D_0018DE1C + (a1 * 12)) = player_object->z;
}

void player_position_restore(int a1)
{
    player_object->x = *(int *)(saved_positions + (a1 * 12));
    player_object->y = *(int *)(D_0018DE18 + (a1 * 12));
    player_object->z = *(int *)(D_0018DE1C + (a1 * 12));
    *(signed char *)D_001940D5 |= 2;
}

int func_0007E1A7(struct record *a1)
{
    int l_1C;

    l_1C = func_00135DE4(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
    if (l_1C != 0) goto L7E213;
    func_00135E39();
    l_1C = func_00135DE4(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
L7E213:;
    return ((a1->anim_frame >= *(unsigned short *)((char *)l_1C + 20)) ? 1 : 0);
}

void func_0007E246(struct record *a1)
{
    int l_1C;
    int l_18;

    l_1C = func_00135DE4(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
    if (l_1C != 0) goto L7E2B2;
    func_00135E39();
    l_1C = func_00135DE4(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
L7E2B2:;
    l_18 = ((*(int *)D_001343C0 >> 5) - a1->anim_time) << 5;
    if (l_18 < 0) goto L7E2DE;
    if (l_18 <= 2000) goto L7E2EA;
L7E2DE:;
    l_18 = (int)(unsigned short)*(short *)((char *)l_1C + 22);
L7E2EA:;
    if (((int)(unsigned short)*(short *)((char *)l_1C + 22)) > l_18) return;
    a1->anim_time = *(int *)D_001343C0 >> 5;
    a1->anim_frame++;
}

void func_0007E31C(struct record *a1)
{
    a1->anim_time = *(int *)D_001343C0 >> 5;
    a1->anim_frame = 0;
}

void func_0007EB0B(struct record *a1, int a2)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    if (D_00195AC4->image == 65535) return;
    l_20 = ((a1->x - D_00195AC4->x) / 4096) - 1;
    l_1C = ((a1->z - D_00195AC4->z) / 4096) - 1;
    l_18 = l_20;
L7EB7B:;
    if ((l_20 + 3) > l_18) goto L7EB93;
    return;
L7EB8B:;
    l_18++;
    goto L7EB7B;
L7EB93:;
    l_14 = l_1C;
L7EB99:;
    if ((l_1C + 3) > l_14) goto L7EBB1;
    goto L7EC1A;
L7EBA9:;
    l_14++;
    goto L7EB99;
L7EBB1:;
    if (l_18 < 0) goto L7EBC9;
    if (current_location->width > l_18) goto L7EBCB;
L7EBC9:;
    goto L7EBA9;
L7EBCB:;
    if (l_14 < 0) goto L7EBE3;
    if (current_location->height > l_14) goto L7EBE5;
L7EBE3:;
    goto L7EBA9;
L7EBE5:;
    if (*(int *)(D_001940E4 + (((l_14 << 5) + l_18) << 2)) == 0) goto L7EC18;
    ((int (*)())(*(int *)D_00195CD0))(*(int *)(*(char **)(D_001940E4 + (((l_14 << 5) + l_18) << 2)) + 63), a2);
L7EC18:;
    goto L7EBA9;
L7EC1A:;
    goto L7EB8B;
}

int func_0007EC28(struct record *a1, struct record *a2)
{
    int l_18;

    l_18 = func_000C7FD9(a1->x, a1->z, a2->x, a2->z);
    if (l_18 <= 512) goto L7EC6D;
    return 0;
L7EC6D:;
    return (l_18 << 7) / 512;
}

void func_0007EC8F(int a1, int a2, int a3, int a4, int a5)
{
    int l_C;

    l_C = 0;
L7ECAD:;
    if (l_C < a3) goto L7ECC2;
    return;
L7ECBA:;
    l_C++;
    goto L7ECAD;
L7ECC2:;
    if (*(short *)mouse_x <= *(short *)((char *)((l_C * 12) + a4))) goto L7ECE9;
    if (*(short *)mouse_x < *(short *)((char *)((l_C * 12) + a4) + 4)) goto L7ECEB;
L7ECE9:;
    goto L7ECFF;
L7ECEB:;
    if (*(short *)mouse_y > *(short *)((char *)((l_C * 12) + a4) + 2)) goto L7ED01;
L7ECFF:;
    goto L7ED15;
L7ED01:;
    if (*(short *)mouse_y < *(short *)((char *)((l_C * 12) + a4) + 6)) goto L7ED17;
L7ED15:;
    goto L7ED3A;
L7ED17:;
    text_draw_colored(*(int *)((char *)((l_C << 2) + a5)), (int)(short)*(short *)&a1, (int)(short)*(short *)&a2, 145, 156);
L7ED3A:;
    goto L7ECBA;
}

int func_0007ED48(int a1)
{
    return a1;
}

int detect_arrow_frame(void)
{
    if (detect_target == 0) goto L7EDB0;
    return func_000C808D(player_object->x, player_object->z, detect_target->x, detect_target->z) >> 6;
L7EDB0:;
    return rand() % 32;
}

int func_0007EDD3(void)
{
    int l_20;
    int l_1C;

    l_20 = 1132;
    if (((unsigned)(*(int *)((char *)l_20) - *(int *)D_00190C7C)) <= 38) goto L7EE23;
    *(int *)D_00190C74 = (*(int *)D_00190C74 + 1) % 32;
    l_1C = 1132;
    *(int *)D_00190C7C = *(int *)((char *)l_1C);
L7EE23:;
    return *(int *)D_00190C74;
}

void func_0007EE38(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_18 = 24000;
    l_1C = 0;
L7EE54:;
    if (l_1C < 200) goto L7EE65;
    return;
L7EE5F:;
    l_1C += 4;
    goto L7EE54;
L7EE65:;
    l_20 = 0;
L7EE6C:;
    if (l_20 < 320) goto L7EE7D;
    goto L7EEA3;
L7EE77:;
    l_20 += 4;
    goto L7EE6C;
L7EE7D:;
    *(signed char *)((char *)(int)(*(char **)D_00147954 + l_18++)) = *(signed char *)((char *)(int)(*(char **)screen_buffer + ((l_1C * 320) + l_20)));
    goto L7EE77;
L7EEA3:;
    goto L7EE5F;
}

int func_0007EEAF(void)
{
    int l_44;
    int l_40;
    int l_3C;
    int l_38;
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    *(signed char *)D_00196281 = 0;
    return 0;
}

void func_0007EF20(void)
{
    char l_3C[28];
    char l_20[12];

    if (*(signed char *)D_00196281 == 0) return;
    mc_memset((int)l_20, 0, 12, (int)D_00176A10, 1064, 4);
    *(short *)l_3C = 257;
    *(short *)((char *)l_3C + 12) = *(short *)D_001A5A54;
    int386x(49, (int)l_3C, (int)l_3C, (int)l_20);
    *(signed char *)D_00196281 = 0;
}

void func_0007F093(struct record *a1)
{
    if (a1->type != 10) return;
    func_0007F0F3(a1->data.membership.faction);
}

void func_0007F0C9(void)
{
    object_foreach(player_entity->children, (int)func_0007F093);
}

void gold_add(int a1)
{
    if ((object_weight(player_entity) + (a1 / 100)) <= (carry_capacity() << 2)) goto L7F1CA;
    gold_make_credit_letter(a1);
    return;
L7F1CA:;
    player_character->gold += a1;
}

void gold_spend(int a1)
{
    struct record *l_18;

    if (((unsigned)player_character->gold) < a1) goto L7F218;
    player_character->gold -= a1;
    return;
L7F218:;
    D_00195AF4 = 0;
    l_18 = gold_find_credit(a1);
    if (l_18 == 0) goto L7F23E;
    l_18->data.item.value -= a1;
    return;
L7F23E:;
    *(int *)free_later_count = 0;
    *(int *)D_00195B84 = a1;
    object_find(player_entity->children, (int)gold_spend_credit_cb);
    object_free_pending();
    if (*(int *)D_00195B84 == 0) return;
    player_character->gold -= *(int *)D_00195B84;
    if (player_character->gold >= 0) return;
    player_character->gold = 0;
}

int gold_can_afford(int a1)
{
    return ((gold_total() >= a1) ? 1 : 0);
}

void gold_sum_credit_cb(struct record *a1)
{
    struct item *l_18;

    if (a1->type != 2) return;
    l_18 = &a1->data.item;
    if (l_18->group != 27) goto L7F331;
    if (l_18->index == 2) goto L7F333;
L7F331:;
    return;
L7F333:;
    *(int *)D_00195B84 += l_18->value;
}

int gold_total(void)
{
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, (int)gold_sum_credit_cb);
    return *(int *)D_00195B84 + player_character->gold;
}

int gold_find_credit_cb(struct record *a1)
{
    struct item *l_1C;

    if (a1->type == 2) goto L7F3BD;
    return 0;
L7F3BD:;
    l_1C = &a1->data.item;
    if (l_1C->group != 27) goto L7F3E8;
    if (l_1C->index == 2) goto L7F3EA;
L7F3E8:;
    goto L7F3F8;
L7F3EA:;
    if (((unsigned)l_1C->value) >= *(int *)D_00195B84) goto L7F3FA;
L7F3F8:;
    goto L7F40B;
L7F3FA:;
    D_00195AF4 = a1;
    return 1;
L7F40B:;
    return 0;
}

int gold_spend_credit_cb(struct record *a1)
{
    struct item *l_1C;

    if (a1->type == 2) goto L7F44B;
    return 0;
L7F44B:;
    if (*(int *)D_00195B84 != 0) goto L7F460;
    return 0;
L7F460:;
    l_1C = &a1->data.item;
    if (l_1C->group != 27) goto L7F48B;
    if (l_1C->index == 2) goto L7F48D;
L7F48B:;
    goto L7F4C7;
L7F48D:;
    if (((unsigned)l_1C->value) > *(int *)D_00195B84) goto L7F4B1;
    object_free_later(a1);
    *(int *)D_00195B84 -= l_1C->value;
    goto L7F4C7;
L7F4B1:;
    l_1C->value -= *(int *)D_00195B84;
    *(int *)D_00195B84 = 0;
L7F4C7:;
    return 0;
}

struct record *gold_find_credit(int a1)
{
    *(int *)D_00195B84 = a1;
    object_find(player_entity->children, (int)gold_find_credit_cb);
    return D_00195AF4;
}

void gold_make_credit_letter(int a1)
{
    struct record *l_18;

    l_18 = item_add_to_container((int)D_001959E0, 27, 2, 0);
    l_18->data.item.value = a1;
}

int gold_total_alias(void)
{
    return gold_total();
}

int gold_can_carry(int a1)
{
    return (((object_weight(player_entity) + (a1 / 100)) <= (carry_capacity() << 2)) ? 1 : 0);
}

void gold_delete_credit_cb(struct record *a1)
{
    short l_18;

    if (a1->type != 2) return;
    *(int *)&l_18 = (int)RECORD_DATA(a1);
    if (((int)(unsigned short)*(short *)(*(char **)&l_18 + 32)) != 27) goto L7F624;
    if (((int)(unsigned short)*(short *)(*(char **)&l_18 + 34)) == 2) goto L7F626;
L7F624:;
    return;
L7F626:;
    object_delete(a1);
}

void gold_remove_all(void)
{
    player_character->gold = 0;
    object_foreach(player_entity->children, (int)gold_delete_credit_cb);
}

void func_0007F671(void)
{
    int l_18;

    *(int *)D_001A59CC = func_000CE8B2();
    l_18 = 1132;
    *(int *)D_001A59DC = *(int *)((char *)l_18);
}

void func_0007F6A4(void)
{
    int l_20;
    int l_1C;
    int l_18;

    *(int *)D_001A59E0 = *(int *)D_001A59CC;
    *(int *)D_001A59E4 = *(int *)D_001A59DC;
    *(int *)D_001A59CC = func_000CE8B2();
    l_18 = 1132;
    if ((*(int *)D_001A59DC = *(int *)((char *)l_18)) != *(int *)D_001A59E4) goto L7F6FE;
    l_20 = *(int *)D_001A59E0 - *(int *)D_001A59CC;
    goto L7F726;
L7F6FE:;
    l_20 = ((65535 - *(int *)D_001A59CC) + *(int *)D_001A59E0) + (((*(int *)D_001A59DC - *(int *)D_001A59E4) - 1) * 65535);
L7F726:;
    if (((unsigned)l_20) >= 10000) goto L7F745;
    l_20 = (int)(*(char **)D_001A59E0 + (65535 - *(int *)D_001A59CC));
L7F745:;
    *(int *)(D_001A5408 + (*(int *)D_001A59D8 << 2)) = ((unsigned)(((unsigned)(l_20 * 1000)) / 1193180)) >> 1;
    if (((unsigned)*(int *)(D_001A5408 + (*(int *)D_001A59D8 << 2))) <= 100) goto L7F78A;
    *(int *)(D_001A5408 + (*(int *)D_001A59D8 << 2)) = 100;
L7F78A:;
    *(int *)D_001A59D8 = (*(int *)D_001A59D8 + 1) & 7;
    l_1C = 0;
L7F79F:;
    if (l_1C < 8) goto L7F7AF;
    goto L7F7C3;
L7F7A7:;
    l_1C++;
    goto L7F79F;
L7F7AF:;
    *(int *)D_00195AB0 += *(int *)(D_001A5408 + (l_1C << 2));
    goto L7F7A7;
L7F7C3:;
    *(int *)D_00195AB0 >>= 3;
    if (*(int *)D_00195AB0 <= 500) return;
    *(int *)D_00195AB0 = 500;
}

void func_0007F7EA(struct record *a1)
{
    if (*(signed char *)D_00196282 == 0) goto L7F812;
    if ((signed char)a1->quest_id != *(signed char *)D_00196282) goto L7F814;
L7F812:;
    goto L7F819;
L7F814:;
    return;
L7F819:;
    if (*(int *)D_00195CF0 == 0) goto L7F830;
    if ((int)a1->id != *(int *)D_00195CF0) goto L7F832;
L7F830:;
    goto L7F834;
L7F832:;
    return;
L7F834:;
    if (*(short *)D_00195F60 == 0) goto L7F84E;
    if ((short)a1->image2 != *(short *)D_00195F60) goto L7F850;
L7F84E:;
    goto L7F852;
L7F850:;
    return;
L7F852:;
    if (*(signed char *)D_00196284 == 0) goto L7F868;
    if ((signed char)a1->type != *(signed char *)D_00196284) goto L7F86A;
L7F868:;
    goto L7F86C;
L7F86A:;
    return;
L7F86C:;
    D_00195AF4 = a1;
}

void location_restore_stored(void)
{
    struct record *l_1C;
    struct record *l_18;

    if (((int)(unsigned char)*(signed char *)player_environment) == 3) return;
    func_0008E447(D_001959F8->children, (int)func_0007FFE7);
    if (D_001959F4 == 0) goto L7F8F2;
    if (D_001959F4->children != 0) goto L7F8F7;
L7F8F2:;
    goto L7F9B5;
L7F8F7:;
    l_1C = D_001959F4->children;
L7F902:;
    if (l_1C == 0) goto L7F9A3;
    l_18 = l_1C->next;
    if (l_1C->type != 64) goto L7F932;
    if (((unsigned)l_1C->building_id) < *(int *)game_minutes) goto L7F934;
L7F932:;
    goto L7F93E;
L7F934:;
    object_delete(l_1C);
    goto L7F998;
L7F93E:;
    if (l_1C->type != 64) goto L7F961;
    if (object_find_by_id(D_00195AC4, l_1C->building_id) != 0) goto L7F963;
L7F961:;
    goto L7F998;
L7F963:;
    mc_memcpy(&current_location->buildings[l_1C->image], RECORD_DATA(l_1C), 26, (int)D_00176A10, 1350, 4);
L7F998:;
    l_1C = l_18;
    goto L7F902;
L7F9A3:;
    func_0008E447(D_001959F4->children, (int)func_0007FD7E);
L7F9B5:;
    if (D_001959EC == 0) goto L7F9C9;
    if (player_character->house != 0) goto L7F9CB;
L7F9C9:;
    goto L7F9E5;
L7F9CB:;
    if ((((unsigned)player_character->house) >> 16) == (((unsigned)D_00195AC4->id) >> 16)) goto L7F9E7;
L7F9E5:;
    goto L7F9FB;
L7F9E7:;
    func_0008E447(D_001959EC->children, (int)func_0007FEB9);
    goto L7FA37;
L7F9FB:;
    if (D_001959F0 == 0) goto L7FA0F;
    if (player_character->ship_owned != 0) goto L7FA11;
L7FA0F:;
    goto L7FA23;
L7FA11:;
    if (((unsigned)(((unsigned)D_00195AC4->id) >> 16)) < 1000) goto L7FA25;
L7FA23:;
    goto L7FA37;
L7FA25:;
    func_0008E447(D_001959F0->children, (int)func_0007FEB9);
L7FA37:;
    *(int *)D_001A59D4 = 10;
}

void location_store_objects(void)
{
    struct record *l_20;
    struct building *l_1C;
    int l_18;

    if (((int)(unsigned char)*(signed char *)player_environment) == 3) return;
    *(int *)free_later_count = 0;
    func_0008E447(D_00195AC4->children, (int)func_0007FF73);
    if (D_001959F4 == 0) goto L7FB6B;
    guild_npc_object = D_001959F4;
    func_0008E447(D_00195AC4, (int)func_0007FCBF);
    l_1C = current_location->buildings;
    l_18 = 0;
L7FABD:;
    if (current_location->building_count > l_18) goto L7FAE4;
    goto L7FB6B;
L7FAD5:;
    l_18++;
    l_1C++;
    goto L7FABD;
L7FAE4:;
    if (l_1C->type != 15) goto L7FB05;
    if (((int)(unsigned char)(l_1C->flags & 2)) != 0) goto L7FB07;
L7FB05:;
    goto L7FB15;
L7FB07:;
    if (((unsigned)l_1C->rent_expires) > *(int *)game_minutes) goto L7FB17;
L7FB15:;
    goto L7FB66;
L7FB17:;
    l_20 = object_create_child(D_001959F4, 0, 26);
    l_20->type = 64;
    l_20->image = l_18;
    l_20->building_id = l_1C->id;
    mc_memcpy(RECORD_DATA(l_20), l_1C, 26, (int)D_00176A10, 1392, 4);
L7FB66:;
    goto L7FAD5;
L7FB6B:;
    if (player_character->house == 0) goto L7FB90;
    if ((((unsigned)player_character->house) >> 16) == (((unsigned)D_00195AC4->id) >> 16)) goto L7FB95;
L7FB90:;
    goto L7FC14;
L7FB95:;
    object_free_children(D_001959EC);
    if ((*(int *)&guild_npc_object = (int)D_001959EC) != 0) goto L7FBED;
    guild_npc_object = (struct record *)((int)(D_001959EC = object_create_child(player_entity, 0, 0)));
    D_001959EC->type = 52;
    D_001959EC->flags = 3;
    D_001959EC->image = 5;
L7FBED:;
    func_0008E447(object_find_by_id(D_00195AC4, player_character->house)->children, (int)func_0007FDEA);
    goto L7FCB0;
L7FC14:;
    if (player_character->ship_owned == 0) goto L7FC31;
    if (((unsigned)(((unsigned)D_00195AC4->id) >> 16)) < 1000) goto L7FC36;
L7FC31:;
    goto L7FCB0;
L7FC36:;
    object_free_children(D_001959F0);
    if ((*(int *)&guild_npc_object = (int)D_001959F0) != 0) goto L7FC8E;
    guild_npc_object = (struct record *)((int)(D_001959F0 = object_create_child(player_entity, 0, 0)));
    D_001959F0->type = 52;
    D_001959F0->flags = 3;
    D_001959F0->image = 6;
L7FC8E:;
    func_0008E447(object_find_by_id(D_00195AC4, D_00195AC4->id)->children, (int)func_0007FDEA);
L7FCB0:;
    object_free_pending();
}

void func_0007FCBF(struct record *a1)
{
    struct building *l_18;

    if (a1->type == 33) goto L7FCF4;
    if (((int)(unsigned short)(a1->flags & 4)) == 0) goto L7FCF6;
L7FCF4:;
    goto L7FCFB;
L7FCF6:;
    return;
L7FCFB:;
    l_18 = object_building(a1);
    if (l_18->type != 15) return;
    if (((int)(unsigned char)(l_18->flags & 2)) == 0) goto L7FD35;
    if (((unsigned)*(int *)game_minutes) < l_18->rent_expires) goto L7FD37;
L7FD35:;
    return;
L7FD37:;
    a1->home_id = l_18->id;
    a1->repair_due = l_18->rent_expires;
    object_reparent(guild_npc_object, a1);
    a1->type = 58;
    a1->id = object_new_id(100);
}

void func_0007FD7E(struct record *a1)
{
    struct record *l_18;

    if (a1->type != 58) return;
    if (((unsigned)*(int *)game_minutes) <= a1->repair_due) goto L7FDB6;
    object_delete(a1);
    return;
L7FDB6:;
    l_18 = object_find_by_id(D_00195AC4, a1->home_id);
    if (l_18 == 0) return;
    object_reparent(l_18, a1);
    a1->type = 33;
}

void func_0007FDEA(struct record *a1)
{
    struct building *l_18;

    if (a1->type == 2) goto L7FE19;
    if (a1->type != 33) goto L7FE1B;
L7FE19:;
    goto L7FE20;
L7FE1B:;
    return;
L7FE20:;
    if (a1->type != 33) goto L7FE38;
    if (a1->children == 0) goto L7FE3A;
L7FE38:;
    goto L7FE3F;
L7FE3A:;
    return;
L7FE3F:;
    if (a1->type != 2) goto L7FE60;
    if (a1->parent->type == 33) goto L7FE62;
L7FE60:;
    goto L7FE64;
L7FE62:;
    return;
L7FE64:;
    l_18 = object_building(a1);
    a1->home_id = l_18->id;
    object_reparent(guild_npc_object, a1);
    if (a1->type != 33) goto L7FE9D;
    a1->type = 58;
L7FE9D:;
    a1->id = object_new_id(100);
}

void func_0007FEB9(struct record *a1)
{
    struct record *l_18;

    if (a1->type == 2) goto L7FEE8;
    if (a1->type != 58) goto L7FEEA;
L7FEE8:;
    goto L7FEEF;
L7FEEA:;
    return;
L7FEEF:;
    if (a1->type != 58) goto L7FF07;
    if (a1->children == 0) goto L7FF09;
L7FF07:;
    goto L7FF0B;
L7FF09:;
    return;
L7FF0B:;
    if (a1->type != 2) goto L7FF2C;
    if (a1->parent->type == 58) goto L7FF2E;
L7FF2C:;
    goto L7FF30;
L7FF2E:;
    return;
L7FF30:;
    l_18 = object_find_by_id(D_00195AC4, a1->home_id);
    if (l_18 == 0) goto L7FF56;
    object_reparent(l_18, a1);
    goto L7FF63;
L7FF56:;
    object_reparent(D_00195AC4, a1);
L7FF63:;
    a1->type = 33;
}

void func_0007FF73(struct record *a1)
{
    if (a1->type != 54) return;
    if ((*(int *)game_minutes - (int)a1->repair_due) <= 259200) goto L7FFAF;
    object_free_single(a1);
    return;
L7FFAF:;
    a1->home_id = a1->parent->id;
    a1->id = object_new_id(100);
    object_reparent(D_001959F8, a1);
}

void func_0007FFE7(struct record *a1)
{
    struct record *l_18;

    if (a1->type != 54) return;
    if ((*(int *)game_minutes - (int)a1->repair_due) <= 259200) goto L80023;
    object_free_single(a1);
    return;
L80023:;
    l_18 = object_find_by_id(D_00195AC4, a1->home_id);
    if (l_18 == 0) return;
    object_reparent(l_18, a1);
}

void arrival_room_messages(void)
{
    int l_24;
    int l_20;
    struct building *l_1C;
    int l_18;

    if (*(int *)D_001A59D4 <= 0) goto L80073;
    (*(int *)D_001A59D4)--;
    return;
L80073:;
    if (*(int *)D_001A59D4 < 0) return;
    (*(int *)D_001A59D4)--;
    if (((int)(unsigned char)*(signed char *)player_environment) != 1) goto L800AB;
    if (location_contains(player_object->x, player_object->z) != 0) goto L800AD;
L800AB:;
    goto L800E9;
L800AD:;
    if (current_location->kind == 4) goto L800D1;
    if (current_location->kind <= 9) goto L800D3;
L800D1:;
    goto L800DC;
L800D3:;
    l_24 = 1;
    goto L800E3;
L800DC:;
    l_24 = 0;
L800E3:;
    if (l_24 != 0) goto L800EB;
L800E9:;
    goto L800F0;
L800EB:;
    holiday_announce();
L800F0:;
    l_1C = current_location->buildings;
    l_20 = 0;
L80102:;
    if (current_location->building_count > l_20) goto L80129;
    return;
L8011A:;
    l_20++;
    l_1C++;
    goto L80102;
L80129:;
    if (l_1C->type != 15) goto L80198;
    tavern_building = l_1C;
    if (tavern_room_rented() == 0) goto L80198;
    func_000A0ED9(1570, (int)D_00176A10);
    mc_sprintf((int)text_buffer, (int)D_00176A42, building_name(l_1C), (((unsigned)(l_1C->rent_expires - *(int *)game_minutes)) / 60) + 1);
    hud_message_add((int)text_buffer);
L80198:;
    goto L8011A;
}

int carry_capacity(void)
{
    int l_28;
    int l_24;
    int l_20;
    struct item *l_1C;

    l_28 = player_character->attributes[0] + (player_character->attributes[0] >> 1);
    l_24 = 0;
L801D4:;
    if (l_24 < 27) goto L801E7;
    goto L8029F;
L801DF:;
    l_24++;
    goto L801D4;
L801E7:;
    if (player_character->equipped[l_24] == 0) goto L801DF;
    l_1C = &player_character->equipped[l_24]->data.item;
    l_20 = 0;
L8021E:;
    if (l_20 >= 10) goto L80236;
    if (l_1C->enchantments[l_20].type != (-1)) goto L80238;
L80236:;
    goto L8029A;
L80238:;
    if (l_1C->enchantments[l_20].type != 7) goto L80292;
    if (l_1C->enchantments[l_20].param == 0) goto L80273;
    l_28 = (l_28 * 384) / 256;
    goto L8028A;
L80273:;
    l_28 = (l_28 * 320) / 256;
L8028A:;
    return l_28;
L80292:;
    l_20++;
    goto L8021E;
L8029A:;
    goto L801DF;
L8029F:;
    return l_28;
}
