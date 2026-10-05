/* loadsave.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char mouse_buttons[];
extern char D_0012AC02[];
extern char mouse_x[];
extern char mouse_y[];
extern char D_0012B508[];
extern char dungeon_water_level[];
extern char D_00147954[];
extern char D_00176884[];
extern char D_001768A8[];
extern char D_001768BD[];
extern char D_001768D5[];
extern char D_001768DF[];
extern char D_001768E4[];
extern char D_001768FC[];
extern char D_00176909[];
extern char D_00176911[];
extern char D_00176915[];
extern char D_0017691F[];
extern char D_00176927[];
extern char D_00176934[];
extern char D_0017693D[];
extern char D_0017697F[];
extern char D_0017698C[];
extern char D_00176999[];
extern char D_001769A9[];
extern char D_001769BC[];
extern char D_001769EA[];
extern char D_001769F7[];
extern char player_environment[];
extern char travel_options[];
extern char D_00178A10[];
extern char D_00178A14[];
extern char D_00178A18[];
extern char D_001846F8[];
extern char saveload_buttons[];
extern char D_00187A92[];
extern char D_00187A94[];
extern char D_00187A96[];
extern char D_00187A98[];
extern char savetree_version[];
extern char D_00187F28[];
extern char saved_location_name[];
extern char saved_region_name[];
extern char bio_modifiers[];
extern char saved_positions[];
extern char D_0018DE44[];
extern char region_event_values[];
extern char text_buffer[];
extern char D_001903A8[];
extern char D_00190D16[];
extern char text_rsc_buffer[];
extern char D_001913E4[];
extern char D_001917E4[];
extern char D_001940D4[];
extern char D_001940D5[];
extern char D_001940D7[];
extern char D_001940D8[];
extern char quest_global_states[];
extern char D_001952EE[];
extern char D_00195303[];
extern char D_0019530D[];
extern char D_00195998[];
extern char D_0019599C[];
extern char bank_ship_price[];
extern char bank_house_price[];
extern struct record *nonworld_root;
extern char frame_counter[];
extern struct record *logbook_object;
extern struct record *options_object;
extern char view_look_pitch[];
extern char D_001959C4[];
extern struct record *inventory_containers[];
extern char D_001959FC[];
extern struct record *D_00195A00;
extern struct record *bank_accounts;
extern char D_00195A08[];
extern char D_00195A0C[];
extern char quest_faces[];
extern char D_00195A78[];
extern char jump_velocity[];
extern struct record *camera_object;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern char D_00195AAC[];
extern char vertical_velocity[];
extern struct record *D_00195AC4;
extern char cheat_flags[];
extern char clothing_gender_group[];
extern char D_00195B18[];
extern char D_00195B44[];
extern char D_00195B5C[];
extern char D_00195B84[];
extern char hud_message_expiry[];
extern struct location *current_location;
extern struct character *player_character;
extern char cursor_arrow_image[];
extern char window_image[];
extern struct career *player_class;
extern char game_minutes[];
extern struct settings *game_settings;
extern char hud_message_ptrs[];
extern char D_00195C40[];
extern char D_00195C44[];
extern char D_00195CDC[];
extern char D_00195CE0[];
extern char D_00195CE4[];
extern char D_00195D48[];
extern char climate_weathers[];
extern char D_00195F62[];
extern char D_00195FB1[];
extern char D_001961F5[];
extern char D_00196263[];
extern char D_00196265[];
extern char D_00196266[];
extern char current_region[];
extern char weapon_active_hand[];
extern char mouse_buttons_prev[];
extern char player_underwater[];
extern char in_dungeon_water[];
extern char crime_current[];
extern char D_0019627F[];
extern char D_00196280[];
extern char D_00196289[];
extern char D_0019629B[];
extern char D_0019966C[];
extern char quest_debug_data[];
extern char D_00199808[];
extern char save_file_handle[];
extern struct record *D_001A4FE0;
extern char save_version[];
extern char cfg_mapsave_file[];
extern char D_001A94C4[];
extern char climate_index[];
extern char D_001A99F8[];
extern char D_001A99FC[];
extern char D_001A9A00[];
extern char D_001A9A04[];
extern char D_001AA540[];
extern char D_001AA544[];
extern struct record *inv_left_container;
extern char D_001AA580[];

extern int func_000641CD(struct record *);
extern int sound_play(int, int, int);
extern int mem_block_size(int);
extern int disk_read_file(int, int);
extern int func_00079A28(struct record *);
extern int savetree_write_record(struct record *);
extern int load_game(int);
extern int key_pressed_once(unsigned char);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern int open(int, ...);
extern int func_0009DEA7();
extern int mc_free();
extern int mc_memset();
extern int func_000A00CB();
extern int mc_strncpy();
extern int write();
extern int func_000A0DD9();
extern int func_000A0DF4();
extern int stricmp();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int unlink();
extern int mc_memcpy();
extern int func_000C010F();
extern int func_000CDD81();
extern int func_0012A274();
extern int func_0012A2D0();
extern int func_0012B136();
extern int func_00142790();
extern int func_00144FB4();
extern void faction_save(int);
extern void faction_load(int);
extern void region_locations_save_discovered(int);
extern void automap_save(void);
extern void quests_unlink_all(int);
extern void quests_relink_all(int);
extern void msgbox_show_string(int, int);
extern void fatal_error(int);
extern void text_draw(int, int, int);
extern void text_draw_centred(int, int, int);
extern void links_save(int);
extern void mem_check_now(int);
extern void disk_copy_file(int, int, int);
extern void disk_delete_matching(int, int);
extern void save_copy_automap_files(int);
extern void saveload_draw(int, int, int);
extern void text_draw_centered_colored();
extern void inpstr_begin_text(int, short);
extern void object_foreach_pre(struct record *, int);
extern void object_foreach_post(struct record *, int);
extern void object_foreach(struct record *, int);
struct record *savetree_attach_record(struct record *, struct record *, int);
int savetree_should_save(struct record *);
int save_write_name(int);
int save_game(int, int);
int saveload_click_slot(int, int, int, int);
int saveload_confirm(int, int, int, int);
void savetree_write_subtree(struct record *);
void savetree_write_saved(struct record *);
void savetree_register_record(struct record *);
void load_relink_object_cb(struct record *);
void load_relink_character(struct record *);
void func_0007A3D0(struct record *);
void func_0007A42B(struct record *);
void save_write_image(void);
void savevars_write(int);
void load_fix_object_cb(struct record *);
void func_0007C78B(struct record *);
#pragma aux func_000A0ED9 parm routine [];

void savetree_read_records(struct record *a1)
{
    struct record *l_24;
    struct record *l_20;
    int l_1C;
    int l_18;

    l_24 = *(struct record **)D_00195C44;
    l_18 = func_00079A28(l_24);
L79C08:;
    if (l_18 == 0) return;
    l_24->prev = 0;
    l_24->next = l_24->prev;
    l_24->parent = l_24->next;
    l_24->children = l_24->parent;
    l_20 = savetree_attach_record(a1, l_24, l_18);
    savetree_register_record(l_20);
    l_1C = l_20->id;
    l_18 = func_00079A28(l_24);
    goto L79C08;
}

void savetree_write_subtree(struct record *a1)
{
    struct record *l_18;

    l_18 = a1->next;
    a1->next = 0;
    object_foreach_pre(a1, (int)savetree_write_record);
    a1->next = l_18;
}

void savetree_write_saved(struct record *a1)
{
    int l_20;
    int l_1C;
    int l_18;

    l_18 = 0;
L79CD1:;
    if (a1 == 0) return;
    if (savetree_should_save(a1) == 0) goto L79CED;
    savetree_write_subtree(a1);
    goto L79CF8;
L79CED:;
    savetree_write_saved(a1->children);
L79CF8:;
    a1 = a1->next;
    goto L79CD1;
}

struct record *savetree_attach_record(struct record *a1, struct record *a2, int a3)
{
    struct record *l_18;
    struct record *l_14;

    l_18 = object_find_by_id(a1, a2->parent_id);
    if (l_18 == 0) goto L79E52;
    l_14 = object_find_by_id(a1, a2->id);
    if (l_14 == 0) goto L79D60;
    if (l_14->type == a2->type) goto L79D62;
L79D60:;
    goto L79D73;
L79D62:;
    if (l_14->parent->id == a2->parent_id) goto L79D78;
L79D73:;
    goto L79E2C;
L79D78:;
    if (l_14->type != 34) goto L79DD6;
    if ((l_14->image & 127) == (a2->image & 127)) goto L79DD6;
    l_14 = object_create_child(l_18, a2, a3 - 71);
    l_14->parent_id = 0;
    return l_14;
L79DD6:;
    mc_memcpy(l_14, a2, 55, (int)D_00176884, 217, 4);
    mc_memcpy(&l_14->data, &a2->data, (int)&*(signed char *)((char *)mem_block_size((int)l_14) - 71), (int)D_00176884, 218, 4);
    l_14->parent_id = 0;
    return l_14;
L79E2C:;
    l_14 = object_create_child(l_18, a2, a3 - 71);
    l_14->parent_id = 0;
    return l_14;
L79E52:;
    fatal_error((int)D_001768A8);
    return 0;
}

int savetree_should_save(struct record *a1)
{
    if (a1->quest_id == 0) goto L79E94;
    return 1;
L79E94:;
    if (func_000641CD(a1) == 0) goto L79EAC;
    return 1;
L79EAC:;
    switch (a1->type) {
    goto L7A00D;
case 8:
    return a1->flags & 512;
case 2:
case 3:
case 4:
case 9:
case 14:
case 18:
case 19:
case 20:
case 25:
case 26:
case 27:
case 32:
case 36:
case 42:
case 44:
case 45:
case 46:
case 51:
case 52:
case 53:
case 54:
    return 1;
case 33:
    if (a1->children == 0) goto L79FBB;
    return 1;
L79FBB:;
    if (a1->pad19 == 0) goto L79FCE;
    return 1;
L79FCE:;
    goto L7A016;
case 34:
    if (((a1->image & 31) - 2) == 14) goto L7A002;
    if (((a1->image & 31) - 2) != 13) goto L7A00B;
L7A002:;
    return 1;
L7A00B:;
    goto L7A016;
default:
L7A00D:;
    return 0;
L7A016:;
    return 0;
}
}

void savetree_register_record(struct record *a1)
{
    switch (a1->type) {
    return;
case 4:
    player_object = a1;
    return;
case 5:
    camera_object = a1;
    return;
case 3:
    player_class = &(player_character = &(player_entity = a1)->data.character)->career;
    return;
case 52:
    if (inventory_containers[a1->image] == 0) goto L7A133;
    if (inventory_containers[a1->image]->children != 0) goto L7A148;
L7A133:;
    inventory_containers[a1->image] = a1;
L7A148:;
    return;
case 16:
    D_00195A00 = a1;
    return;
case 23:
    game_settings = (struct settings *)((*(int *)&options_object = (int)a1) + 71);
    return;
case 24:
    logbook_object = a1;
    return;
case 25:
    bank_accounts = a1;
    return;
case 39:
    nonworld_root = a1;
default:;
}
}

void load_relink_object_cb(struct record *a1)
{
    switch (a1->type) {
case 3:
case 18:
case 44:
    load_relink_character(a1);
    return;
case 9:
    a1->caster = object_find_by_id(D_00195AC4, (int)a1->caster);
default:;
}
}

void load_relink_all(void)
{
    object_foreach(D_00195AC4, (int)load_relink_object_cb);
    object_foreach(nonworld_root, (int)load_relink_object_cb);
    inv_left_container = inventory_containers[0];
}

void load_relink_character(struct record *a1)
{
    struct character *l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = &a1->data.character;
    l_20 = (int)&l_24->career;
    l_1C = l_20 + 74;
    l_18 = 0;
L7A273:;
    if (l_18 < 27) goto L7A283;
    goto L7A2C1;
L7A27B:;
    l_18++;
    goto L7A273;
L7A283:;
    if (l_24->equipped[l_18] == 0) goto L7A2BF;
    l_24->equipped[l_18] = object_find_by_id(D_00195AC4, (int)l_24->equipped[l_18]);
L7A2BF:;
    goto L7A27B;
L7A2C1:;
    if (l_24->target == 0) goto L7A2F5;
    l_24->target = object_find_by_id(D_00195AC4, (int)l_24->target);
    if (l_24->target != 0) goto L7A2F5;
    fatal_error((int)D_001768BD);
L7A2F5:;
    if (a1->type == 18) goto L7A313;
    if (a1->type != 44) return;
L7A313:;
    func_000C010F(l_1C);
}

void save_unlink_character(struct record *a1)
{
    struct character *l_1C;
    int l_18;

    l_1C = &a1->data.character;
    l_18 = 0;
L7A346:;
    if (l_18 < 27) goto L7A356;
    goto L7A38B;
L7A34E:;
    l_18++;
    goto L7A346;
L7A356:;
    if (l_1C->equipped[l_18] == 0) goto L7A389;
    l_1C->equipped[l_18] = (struct record *)l_1C->equipped[l_18]->id;
L7A389:;
    goto L7A34E;
L7A38B:;
    if (l_1C->target == 0) return;
    if (object_find_by_id(D_00195AC4, l_1C->target->id) == 0) goto L7A3BC;
    l_1C->target = (struct record *)l_1C->target->id;
    return;
L7A3BC:;
    l_1C->target = 0;
}

void func_0007A3D0(struct record *a1)
{
    switch (a1->type) {
case 18:
case 33:
case 44:
    *(int *)((char *)(int)(*(char **)D_00195C44 + ((*(int *)D_00195B84)++ << 2))) = a1->id;
default:;
}
}

void func_0007A42B(struct record *a1)
{
    int l_1C;
    int l_18;

    if (a1->type != 34) goto L7A45A;
    if (a1->type != 32) goto L7A45C;
L7A45A:;
    return;
L7A45C:;
    l_1C = *(int *)D_00195C44;
    l_18 = 0;
L7A46B:;
    if (l_18 < *(int *)D_00195B84) goto L7A487;
    return;
L7A478:;
    l_18++;
    (*(char (**)[4])&l_1C)++;
    goto L7A46B;
L7A487:;
    if (a1->id != *(int *)((char *)l_1C)) goto L7A4A7;
    object_delete(a1);
    *(int *)((char *)l_1C) = 0;
    return;
L7A4A7:;
    goto L7A478;
}

void func_0007A4B3(void)
{
    *(int *)D_00195B84 = 0;
    object_foreach(D_00195AC4->children, (int)func_0007A3D0);
    object_foreach_post(D_00195AC4->children, (int)func_0007A42B);
}

void save_write_image(void)
{
    func_000A0ED9(494, (int)D_00176884);
    mc_sprintf((int)D_001913E4, (int)D_001768DF, (int)text_buffer, (int)D_001768D5);
    unlink((int)D_001913E4);
    if ((*(int *)save_file_handle = open((int)D_001913E4, 546, 384)) >= 0) goto L7A571;
    fatal_error((int)D_001768E4);
L7A571:;
    write(*(int *)save_file_handle, *(int *)D_00147954 + 24000, 4000);
    func_0009DEA7(*(int *)save_file_handle);
}

int save_write_name(int a1)
{
    func_000A0ED9(516, (int)D_00176884);
    mc_sprintf((int)D_001913E4, (int)D_001768DF, (int)text_buffer, (int)D_001768FC);
    unlink((int)D_001913E4);
    *(int *)save_file_handle = open((int)D_001913E4, 546, 384);
    write(*(int *)save_file_handle, a1, 32);
    func_0009DEA7(*(int *)save_file_handle);
    return 0;
}

int save_game(int a1, int a2)
{
    int l_18;

    mem_check_now(1000);
    func_000A0ED9(540, (int)D_00176884);
    mc_sprintf((int)text_buffer, (int)D_00176909, a1);
    disk_delete_matching((int)text_buffer, (int)D_00176911);
    save_write_image();
    save_write_name(a2);
    savevars_write(a1);
    region_locations_save_discovered((int)(unsigned char)*(signed char *)current_region);
    automap_save();
    func_000A0ED9(550, (int)D_00176884);
    mc_sprintf((int)text_buffer, (int)D_00176909, a1);
    disk_copy_file((int)D_00176915, (int)D_001917E4, (int)text_buffer);
    disk_copy_file((int)D_0017691F, (int)D_001917E4, (int)text_buffer);
    disk_copy_file((int)cfg_mapsave_file, (int)D_001917E4, (int)text_buffer);
    save_copy_automap_files(a1);
    func_000A0ED9(558, (int)D_00176884);
    mc_sprintf((int)text_rsc_buffer, (int)D_001768DF, (int)text_buffer, (int)D_00176927);
    unlink((int)text_rsc_buffer);
    *(int *)save_file_handle = open((int)text_rsc_buffer, 546, 384);
    write(*(int *)save_file_handle, (int)savetree_version, 4);
    write(*(int *)save_file_handle, (int)&player_object->x, 12);
    write(*(int *)save_file_handle, (int)&D_00195AC4->image, 2);
    write(*(int *)save_file_handle, (int)player_environment, 1);
    l_18 = current_location->building_count * 26;
    write(*(int *)save_file_handle, (int)&l_18, 4);
    write(*(int *)save_file_handle, (int)current_location->buildings, l_18);
    l_18 = 0;
    quests_unlink_all((int)D_00195A00);
    savetree_write_saved(D_00195AC4->children);
    write(*(int *)save_file_handle, (int)&l_18, 4);
    object_foreach_pre(nonworld_root->children, (int)savetree_write_record);
    write(*(int *)save_file_handle, (int)&l_18, 4);
    quests_relink_all((int)D_00195A00);
    links_save(*(int *)save_file_handle);
    func_0009DEA7(*(int *)save_file_handle);
    *(signed char *)mouse_buttons = (*(signed char *)mouse_buttons_prev = 0);
    func_0012B136();
L7A891:;
    if (*(signed char *)mouse_buttons == 0) goto L7A8A1;
    func_0012B136();
    goto L7A891;
L7A8A1:;
    mem_check_now(1001);
    return 0;
}

void func_0007A8BE(void)
{
    struct record *l_24;
    struct quest *l_20;
    int l_1C;
    int l_18;

    l_24 = D_00195A00->children;
    l_1C = 0;
    l_18 = 0;
    if (((int)(unsigned char)*(signed char *)current_region) == 31) return;
L7A8F5:;
    if (l_24 == 0) goto L7A938;
    if (l_24->type != 14) goto L7A92D;
    l_20 = &l_24->data.quest;
    if (stricmp(l_20->name, (int)D_00176934) != 0) goto L7A92D;
    l_18++;
L7A92D:;
    l_24 = l_24->next;
    goto L7A8F5;
L7A938:;
    if (l_18 != 0) goto L7A947;
    if (*(signed char *)D_00195303 != 0) goto L7A949;
L7A947:;
    goto L7A952;
L7A949:;
    if (*(signed char *)D_0019530D == 0) goto L7A954;
L7A952:;
    return;
L7A954:;
    mc_strncpy((int)D_001961F5, (int)D_0017693D, 13, (int)D_00176884, 616);
    *(signed char *)D_001952EE = 0;
}

void saveload_menu(int a1)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_2C = 0;
    l_24 = 0;
    l_20 = 0;
    l_18 = (*(int *)window_image = disk_read_file((int)D_0017697F, 0));
    *(int *)D_00195B5C = disk_read_file((int)D_0017698C, 0);
    mc_memset(*(int *)D_00195C44, 0, 256, (int)D_00176884, 862, 4);
    mc_strncpy((int)text_buffer, (int)D_00176999, 160, (int)D_00176884, 864);
    l_28 = 0;
L7B2B4:;
    if (l_28 < 6) goto L7B2C4;
    goto L7B31D;
L7B2BC:;
    l_28++;
    goto L7B2B4;
L7B2C4:;
    *(signed char *)D_001903A8 = *(signed char *)&l_28 + 48;
    l_1C = open((int)text_buffer, 512);
    if (l_1C < 1) goto L7B2BC;
    func_000A00CB(l_1C, (int)(*(char **)D_00147954 + (l_28 * 4000)), 4000);
    func_0009DEA7(l_1C);
    l_24 |= 1 << l_28;
    goto L7B2BC;
L7B31D:;
    mc_strncpy((int)text_buffer, (int)D_001769A9, 160, (int)D_00176884, 874);
    l_28 = 0;
L7B342:;
    if (l_28 < 6) goto L7B352;
    goto L7B3AD;
L7B34A:;
    l_28++;
    goto L7B342;
L7B352:;
    *(signed char *)D_001903A8 = *(signed char *)&l_28 + 48;
    *(signed char *)((char *)(int)(*(char **)D_00195C44 + (l_28 << 5))) = 0;
    l_1C = open((int)text_buffer, 512);
    if (l_1C < 1) goto L7B34A;
    func_000A00CB(l_1C, (int)(*(char **)D_00195C44 + (l_28 << 5)), 32);
    func_0009DEA7(l_1C);
    goto L7B34A;
L7B3AD:;
    if (l_2C != 0) goto L7B546;
    if (key_pressed_once(15) == 0) goto L7B3D8;
    l_20++;
    if (l_20 != 6) goto L7B3D8;
    l_20 = 0;
L7B3D8:;
    if (key_pressed_once(1) == 0) goto L7B3ED;
    l_2C = 1;
L7B3ED:;
    saveload_draw(a1, l_24, l_20);
    *(signed char *)mouse_buttons_prev = *(signed char *)mouse_buttons;
    func_0012B136();
    func_00144FB4((int)(short)*(short *)mouse_x, (int)(short)*(short *)mouse_y, 10, 10, *(int *)cursor_arrow_image);
    if (key_pressed_once(28) == 0) goto L7B462;
    *(signed char *)D_0012AC02 = 1;
    l_20 = saveload_click_slot(a1, a1, l_20, l_24);
    *(signed char *)D_0012AC02 = 0;
    goto L7B52A;
L7B462:;
    if (*(signed char *)D_0012AC02 != 0) goto L7B482;
    if (*(signed char *)mouse_buttons == 0) goto L7B47D;
    if (*(signed char *)mouse_buttons_prev == 0) goto L7B482;
L7B47D:;
    goto L7B52A;
L7B482:;
    l_28 = 0;
L7B489:;
    if (l_28 < 14) goto L7B49C;
    goto L7B52A;
L7B494:;
    l_28++;
    goto L7B489;
L7B49C:;
    if (*(short *)mouse_x <= *(short *)(saveload_buttons + (l_28 * 12))) goto L7B4C4;
    if (*(short *)mouse_x < *(short *)(D_00187A94 + (l_28 * 12))) goto L7B4C6;
L7B4C4:;
    goto L7B4DA;
L7B4C6:;
    if (*(short *)mouse_y > *(short *)(D_00187A92 + (l_28 * 12))) goto L7B4DC;
L7B4DA:;
    goto L7B4F0;
L7B4DC:;
    if (*(short *)mouse_y < *(short *)(D_00187A96 + (l_28 * 12))) goto L7B4F2;
L7B4F0:;
    goto L7B520;
L7B4F2:;
    sound_play(203, (int)player_object, 100);
    l_20 = ((int (*)())(*(int *)(D_00187A98 + (l_28 * 12))))(l_28, a1, l_20, l_24);
L7B520:;
    if (l_20 != (-1)) goto L7B494;
L7B52A:;
    func_000CDD81(1);
    if (l_20 != (-1)) goto L7B541;
    l_2C = 1;
L7B541:;
    goto L7B3AD;
L7B546:;
    if (l_18 == 0) goto L7B555;
    if (l_18 != (-1751672937)) goto L7B557;
L7B555:;
    goto L7B570;
L7B557:;
    mc_free(l_18, (int)D_00176884, 924);
    l_18 = -1751672937;
L7B570:;
    if (*(int *)D_00195B5C == 0) goto L7B585;
    if (*(int *)D_00195B5C != (-1751672937)) goto L7B587;
L7B585:;
    return;
L7B587:;
    mc_free(*(int *)D_00195B5C, (int)D_00176884, 925);
    *(int *)D_00195B5C = -1751672937;
}

int saveload_click_slot(int a1, int a2, int a3, int a4)
{
    if (*(signed char *)D_0012AC02 == 0) goto L7B5E9;
    saveload_confirm(a1, a2, a3, a4);
    return -1;
L7B5E9:;
    if (a1 >= 6) goto L7B604;
    return a1 % 3;
L7B604:;
    return (a1 % 3) + 3;
}

int saveload_confirm(int a1, int a2, int a3, int a4)
{
    int l_10;

    if (a2 == 0) goto L7B70D;
    mc_strncpy((int)text_rsc_buffer, (int)(*(char **)D_00195C44 + (a3 << 5)), 2048, (int)D_00176884, 948);
    *(signed char *)D_0012B508 = 146;
    l_10 = *(int *)D_00195C44 + 55000;
    func_000A0ED9(951, (int)D_00176884);
    mc_sprintf(l_10, (int)D_001769BC, *(int *)D_001846F8);
    *(signed char *)((char *)(func_000A0DF4(l_10) + l_10) + 1) = 0;
    func_00142790();
    inpstr_begin_text((int)text_rsc_buffer, 31);
    *(signed char *)mouse_buttons = (*(signed char *)mouse_buttons_prev = 0);
    msgbox_show_string(l_10, 2);
    if (((int)(unsigned char)*(signed char *)D_0019966C) != 2) goto L7B6FE;
    return 0;
L7B6FE:;
    save_game(a3, (int)text_rsc_buffer);
    goto L7B715;
L7B70D:;
    load_game(a3);
L7B715:;
    *(signed char *)D_00190D16 = 0;
    return -1;
}

int saveload_exit(int a1, int a2, int a3, int a4)
{
    *(signed char *)D_00190D16 = 1;
    return -1;
}

void savevars_read(int a1)
{
    func_000A0ED9(1008, (int)D_00176884);
    mc_sprintf((int)text_buffer, (int)D_00176909, a1);
    func_000A0ED9(1009, (int)D_00176884);
    mc_sprintf((int)text_rsc_buffer, (int)D_001768DF, (int)text_buffer, (int)D_001769EA);
    *(int *)save_file_handle = open((int)text_rsc_buffer, 512);
    func_000A00CB(*(int *)save_file_handle, (int)saved_positions, 48);
    func_000A00CB(*(int *)save_file_handle, (int)bio_modifiers, 64);
    func_000A00CB(*(int *)save_file_handle, (int)view_look_pitch, 12);
    func_000A00CB(*(int *)save_file_handle, (int)D_00196266, 1);
    func_000A00CB(*(int *)save_file_handle, (int)quest_faces, 100);
    func_000A00CB(*(int *)save_file_handle, (int)D_001959C4, 20);
    func_000A00CB(*(int *)save_file_handle, (int)travel_options, 2);
    func_000A00CB(*(int *)save_file_handle, (int)D_0018DE44, 512);
    func_000A00CB(*(int *)save_file_handle, (int)D_00196265, 1);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195FB1, 71);
    func_000A00CB(*(int *)save_file_handle, (int)D_001959FC, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195A08, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195A0C, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195A78, 4);
    func_000A00CB(*(int *)save_file_handle, (int)quest_global_states, 64);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195F62, 2);
    func_000A00CB(*(int *)save_file_handle, (int)D_00196280, 1);
    func_000A00CB(*(int *)save_file_handle, (int)D_001961F5, 13);
    func_000A00CB(*(int *)save_file_handle, (int)D_00178A10, 4);
    func_000A00CB(*(int *)save_file_handle, (int)crime_current, 1);
    func_000A00CB(*(int *)save_file_handle, (int)D_0019627F, 1);
    func_000A00CB(*(int *)save_file_handle, (int)player_underwater, 1);
    func_000A00CB(*(int *)save_file_handle, (int)in_dungeon_water, 1);
    func_000A00CB(*(int *)save_file_handle, (int)D_00178A14, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195CDC, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195CE0, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195CE4, 4);
    func_000A00CB(*(int *)save_file_handle, (int)climate_weathers, 6);
    func_000A00CB(*(int *)save_file_handle, (int)D_001940D4, 8);
    func_000A00CB(*(int *)save_file_handle, (int)frame_counter, 4);
    func_000A00CB(*(int *)save_file_handle, (int)game_minutes, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195C40, 4);
    func_000A00CB(*(int *)save_file_handle, (int)clothing_gender_group, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195B18, 4);
    func_000A00CB(*(int *)save_file_handle, (int)weapon_active_hand, 1);
    func_000A00CB(*(int *)save_file_handle, (int)region_event_values, 4960);
    func_000A00CB(*(int *)save_file_handle, (int)current_region, 1);
    func_000A00CB(*(int *)save_file_handle, (int)cheat_flags, 4);
    func_000A00CB(*(int *)save_file_handle, (int)vertical_velocity, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195AAC, 4);
    func_000A00CB(*(int *)save_file_handle, (int)jump_velocity, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00178A18, 2);
    func_000A00CB(*(int *)save_file_handle, (int)bank_house_price, 4);
    func_000A00CB(*(int *)save_file_handle, (int)bank_ship_price, 4);
    func_000A00CB(*(int *)save_file_handle, (int)saved_location_name, 32);
    func_000A00CB(*(int *)save_file_handle, (int)saved_region_name, 32);
    func_000A00CB(*(int *)save_file_handle, (int)D_00196263, 1);
    func_000A00CB(*(int *)save_file_handle, (int)D_0019599C, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00195998, 4);
    func_000A00CB(*(int *)save_file_handle, (int)climate_index, 4);
    func_000A00CB(*(int *)save_file_handle, (int)climate_weathers, 6);
    func_000A00CB(*(int *)save_file_handle, (int)dungeon_water_level, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_00187F28, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_001A9A04, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_001A99F8, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_001A99FC, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_001A9A00, 4);
    if (*(int *)save_version < 294) goto L7BEDE;
    func_000A00CB(*(int *)save_file_handle, (int)D_001AA540, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_001AA544, 4);
    func_000A00CB(*(int *)save_file_handle, (int)D_001AA580, 4);
L7BEDE:;
    faction_load(*(int *)save_file_handle);
    func_0009DEA7(*(int *)save_file_handle);
}

void savevars_write(int a1)
{
    func_000A0ED9(1092, (int)D_00176884);
    mc_sprintf((int)text_rsc_buffer, (int)D_001768DF, (int)text_buffer, (int)D_001769EA);
    unlink((int)text_rsc_buffer);
    *(int *)save_file_handle = open((int)text_rsc_buffer, 546, 384);
    write(*(int *)save_file_handle, (int)saved_positions, 48);
    write(*(int *)save_file_handle, (int)bio_modifiers, 64);
    write(*(int *)save_file_handle, (int)view_look_pitch, 12);
    write(*(int *)save_file_handle, (int)D_00196266, 1);
    write(*(int *)save_file_handle, (int)quest_faces, 100);
    write(*(int *)save_file_handle, (int)D_001959C4, 20);
    write(*(int *)save_file_handle, (int)travel_options, 2);
    write(*(int *)save_file_handle, (int)D_0018DE44, 512);
    write(*(int *)save_file_handle, (int)D_00196265, 1);
    write(*(int *)save_file_handle, (int)D_00195FB1, 71);
    write(*(int *)save_file_handle, (int)D_001959FC, 4);
    write(*(int *)save_file_handle, (int)D_00195A08, 4);
    write(*(int *)save_file_handle, (int)D_00195A0C, 4);
    write(*(int *)save_file_handle, (int)D_00195A78, 4);
    write(*(int *)save_file_handle, (int)quest_global_states, 64);
    write(*(int *)save_file_handle, (int)D_00195F62, 2);
    write(*(int *)save_file_handle, (int)D_00196280, 1);
    write(*(int *)save_file_handle, (int)D_001961F5, 13);
    write(*(int *)save_file_handle, (int)D_00178A10, 4);
    write(*(int *)save_file_handle, (int)crime_current, 1);
    write(*(int *)save_file_handle, (int)D_0019627F, 1);
    write(*(int *)save_file_handle, (int)player_underwater, 1);
    write(*(int *)save_file_handle, (int)in_dungeon_water, 1);
    write(*(int *)save_file_handle, (int)D_00178A14, 4);
    write(*(int *)save_file_handle, (int)D_00195CDC, 4);
    write(*(int *)save_file_handle, (int)D_00195CE0, 4);
    write(*(int *)save_file_handle, (int)D_00195CE4, 4);
    write(*(int *)save_file_handle, (int)climate_weathers, 6);
    write(*(int *)save_file_handle, (int)D_001940D4, 8);
    write(*(int *)save_file_handle, (int)frame_counter, 4);
    write(*(int *)save_file_handle, (int)game_minutes, 4);
    write(*(int *)save_file_handle, (int)D_00195C40, 4);
    write(*(int *)save_file_handle, (int)clothing_gender_group, 4);
    write(*(int *)save_file_handle, (int)D_00195B18, 4);
    write(*(int *)save_file_handle, (int)weapon_active_hand, 1);
    write(*(int *)save_file_handle, (int)region_event_values, 4960);
    write(*(int *)save_file_handle, (int)current_region, 1);
    write(*(int *)save_file_handle, (int)cheat_flags, 4);
    write(*(int *)save_file_handle, (int)vertical_velocity, 4);
    write(*(int *)save_file_handle, (int)D_00195AAC, 4);
    write(*(int *)save_file_handle, (int)jump_velocity, 4);
    write(*(int *)save_file_handle, (int)D_00178A18, 2);
    write(*(int *)save_file_handle, (int)bank_house_price, 4);
    write(*(int *)save_file_handle, (int)bank_ship_price, 4);
    write(*(int *)save_file_handle, (int)saved_location_name, 32);
    write(*(int *)save_file_handle, (int)saved_region_name, 32);
    write(*(int *)save_file_handle, (int)D_00196263, 1);
    write(*(int *)save_file_handle, (int)D_0019599C, 4);
    write(*(int *)save_file_handle, (int)D_00195998, 4);
    write(*(int *)save_file_handle, (int)climate_index, 4);
    write(*(int *)save_file_handle, (int)climate_weathers, 6);
    write(*(int *)save_file_handle, (int)dungeon_water_level, 4);
    write(*(int *)save_file_handle, (int)D_00187F28, 4);
    write(*(int *)save_file_handle, (int)D_001A9A04, 4);
    write(*(int *)save_file_handle, (int)D_001A99F8, 4);
    write(*(int *)save_file_handle, (int)D_001A99FC, 4);
    write(*(int *)save_file_handle, (int)D_001A9A00, 4);
    write(*(int *)save_file_handle, (int)D_001AA540, 4);
    write(*(int *)save_file_handle, (int)D_001AA544, 4);
    write(*(int *)save_file_handle, (int)D_001AA580, 4);
    faction_save(*(int *)save_file_handle);
    func_0009DEA7(*(int *)save_file_handle);
}

void func_0007C432(void)
{
    int l_1C;
    int l_18;

    *(signed char *)D_00196289 = 1;
    *(int *)quest_debug_data = 0;
    *(int *)D_001A94C4 = -1;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) == 0) goto L7C48C;
    func_0012A2D0(160, 100, 160, 100);
    goto L7C4A5;
L7C48C:;
    func_0012A2D0(160, 77, 160, 77);
L7C4A5:;
    func_0012A274(200, 180);
    *(int *)D_00195D48 = 10000;
    *(signed char *)D_0019629B = 0;
    mc_memset((int)hud_message_ptrs, 0, 68, (int)D_00176884, 1188, 68);
    mc_memset((int)hud_message_expiry, 0, 68, (int)D_00176884, 1189, 68);
    *(int *)D_00195B44 = *(int *)game_minutes;
    *(signed char *)D_001940D7 |= 1;
    *(signed char *)D_001940D5 |= 2;
    *(signed char *)D_001940D8 |= 8;
    current_building = 0;
    l_18 = ((unsigned)*(int *)game_minutes) % 1440;
    if (l_18 <= 360) goto L7C54B;
    if (l_18 < 1080) goto L7C54D;
L7C54B:;
    goto L7C556;
L7C54D:;
    l_1C = 1;
    goto L7C55D;
L7C556:;
    l_1C = 0;
L7C55D:;
    *(int *)D_00199808 = l_1C;
}

void load_fix_object_cb(struct record *a1)
{
    int l_34;
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
{
    struct record *l_3C;

    if (a1->twin == 0) goto L7C5F2;
    if (a1->quest_id == 0) goto L7C5E8;
    l_3C = object_find_by_id(D_001A4FE0, (int)a1->twin);
    a1->twin = l_3C;
    if ((int)D_001A4FE0 != (int)D_00195AC4) goto L7C5E6;
    if (a1->twin != 0) goto L7C5CE;
    fatal_error((int)D_001769F7);
L7C5CE:;
    if (a1->twin->twin != 0) goto L7C5E6;
    a1->twin->twin = (struct record *)a1;
L7C5E6:;
    goto L7C5F2;
L7C5E8:;
    a1->twin = 0;
L7C5F2:;
    switch (a1->type) {
case 52:
    if (a1 == inventory_containers[a1->image]) goto L7C6A0;
L7C66F:;
    if (a1->children == 0) goto L7C69A;
    object_reparent(inventory_containers[a1->image], a1->children);
    goto L7C66F;
L7C69A:;
    a1->type = 0;
L7C6A0:;
    return;
case 33:
    if (a1->parent->type != 52) goto L7C6CB;
    if (a1->parent->image < 5) goto L7C6CD;
L7C6CB:;
    goto L7C6D3;
L7C6CD:;
    a1->type = 2;
L7C6D3:;
    return;
case 43:
    l_30 = (int)RECORD_DATA(a1);
    l_2C = *(int *)((char *)l_30 + 5);
    l_20 = 0;
L7C6F1:;
    if (((int)(unsigned char)*(signed char *)((char *)l_30)) > l_20) goto L7C711;
    goto L7C725;
L7C702:;
    l_20++;
    (*(char (**)[66])&l_2C)++;
    goto L7C6F1;
L7C711:;
    l_34 = l_2C + 4;
    *(int *)((char *)l_34) = 0;
    goto L7C702;
L7C725:;
    return;
case 56:
    l_2C = (int)RECORD_DATA(a1);
    l_20 = 0;
L7C737:;
    if (a1->image > l_20) goto L7C759;
    goto L7C76D;
L7C74A:;
    l_20++;
    (*(char (**)[66])&l_2C)++;
    goto L7C737;
L7C759:;
    l_34 = l_2C + 4;
    *(int *)((char *)l_34) = 0;
    goto L7C74A;
L7C76D:;
    return;
case 6:
case 32:
    l_34 = (int)RECORD_DATA(a1);
    *(int *)((char *)l_34) = 0;
default:;
}
}
}

void func_0007C78B(struct record *a1)
{

    switch (a1->type) {
case 0:
case 42:
    object_delete(a1);
    return;
case 9:
    if (a1->parent->type == 47) goto L7C7F6;
    if (a1->parent->type != 38) goto L7C7F8;
L7C7F6:;
    goto L7C80E;
L7C7F8:;
    if (a1->parent->type != 1) goto L7C883;
L7C80E:;
{
    int l_20;
    l_20 = (int)RECORD_DATA(a1);
    if ((a1->flags & 8192) != 0) goto L7C839;
    object_delete(a1);
    return;
L7C839:;
    if (a1->caster != player_entity) goto L7C851;
    object_delete(a1);
    return;
L7C851:;
    if ((((unsigned)a1->id) >> 16) == (((unsigned)D_00195AC4->id) >> 16)) goto L7C881;
    a1->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
L7C881:;
    return;
L7C883:;
    if ((((unsigned)a1->id) >> 16) == 801) return;
    a1->id = object_new_id(801);
}
default:;
}
}

void load_fix_objects(void)
{
    D_001A4FE0 = nonworld_root;
    object_foreach(D_00195AC4, (int)load_fix_object_cb);
    D_001A4FE0 = D_00195AC4;
    object_foreach(nonworld_root, (int)load_fix_object_cb);
    object_foreach_post(D_00195AC4, (int)func_0007C78B);
}

void func_0007C908(int a1, int a2, int a3)
{
    int l_10;

    *(short *)&l_10 = (int)(unsigned char)*(signed char *)D_0012B508;
    *(signed char *)D_0012B508 = 0;
    text_draw_centred(a1, (int)&*(signed char *)((char *)((int)(short)*(short *)&a2) + 1), (int)&*(signed char *)((char *)((int)(short)*(short *)&a3) + 1));
    *(signed char *)D_0012B508 = *(signed char *)&l_10;
    text_draw_centred(a1, (int)(short)*(short *)&a2, (int)(short)*(short *)&a3);
}

void func_0007C965(int a1, int a2, int a3)
{
    int l_10;

    *(short *)&l_10 = (int)(unsigned char)*(signed char *)D_0012B508;
    *(signed char *)D_0012B508 = 15;
    text_draw(a1, (int)&*(signed char *)((char *)((int)(short)*(short *)&a2) + 1), (int)&*(signed char *)((char *)((int)(short)*(short *)&a3) + 1));
    *(signed char *)D_0012B508 = *(signed char *)&l_10;
    text_draw(a1, (int)(short)*(short *)&a2, (int)(short)*(short *)&a3);
}

void func_0007C9C2(int a1, int a2, int a3)
{
    int l_10;

    *(short *)&l_10 = (int)(unsigned char)*(signed char *)D_0012B508;
    *(signed char *)D_0012B508 = 0;
    text_draw(a1, (int)&*(signed char *)((char *)((int)(short)*(short *)&a2) + 1), (int)&*(signed char *)((char *)((int)(short)*(short *)&a3) + 1));
    *(signed char *)D_0012B508 = *(signed char *)&l_10;
    text_draw(a1, (int)(short)*(short *)&a2, (int)(short)*(short *)&a3);
}

void func_0007CAEB(short a1, short a2, short a3, short a4, short a5, short a6, short a7)
{
    char l_24[12];

    text_draw_centered_colored(func_000A0DD9((int)(short)a5, (int)l_24, 10), (int)(short)((((int)(short)a1) + ((int)(short)a3)) >> 1), (int)(short)(((((int)(short)a2) + ((int)(short)a4)) >> 1) - 2), (int)(short)a6, (int)(short)a7);
}
