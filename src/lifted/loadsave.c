/* loadsave.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char mouse_buttons;
extern signed char D_0012AC02;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern int dungeon_water_level;
extern int D_00147954;
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
extern unsigned char player_environment;
extern char travel_options[];
extern char D_00178A10[];
extern int D_00178A14;
extern char D_00178A18[];
extern int D_001846F8;
extern char saveload_buttons[];
extern char D_00187A92[];
extern char D_00187A94[];
extern char D_00187A96[];
extern char D_00187A98[];
extern char savetree_version[];
extern int D_00187F28;
extern char saved_location_name[];
extern char saved_region_name[];
extern char bio_modifiers[];
extern char saved_positions[];
extern char D_0018DE44[];
extern signed char region_event_values[];
extern signed char text_buffer[];
extern signed char D_001903A8;
extern signed char D_00190D16;
extern signed char text_rsc_buffer[];
extern char D_001913E4[];
extern char D_001917E4[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern unsigned char D_001940D7;
extern signed char D_001940D8;
extern signed char quest_global_states[];
extern signed char D_001952EE;
extern signed char D_00195303;
extern signed char D_0019530D;
extern int D_00195998;
extern char D_0019599C[];
extern int bank_ship_price;
extern int bank_house_price;
extern struct record *nonworld_root;
extern char frame_counter[];
extern struct record *logbook_object;
extern struct record *options_object;
extern int view_look_pitch;
extern char D_001959C4[];
extern struct record *inventory_containers[];
extern int D_001959FC;
extern struct record *D_00195A00;
extern struct record *bank_accounts;
extern char D_00195A08[];
extern int D_00195A0C;
extern signed char quest_faces[];
extern int D_00195A78;
extern int jump_velocity;
extern struct record *camera_object;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern char D_00195AAC[];
extern int vertical_velocity;
extern struct record *D_00195AC4;
extern char cheat_flags[];
extern char clothing_gender_group[];
extern char D_00195B18[];
extern int D_00195B44;
extern int D_00195B5C;
extern char D_00195B84[];
extern int hud_message_expiry[];
extern struct location *current_location;
extern struct character *player_character;
extern int cursor_arrow_image;
extern int window_image;
extern struct career *player_class;
extern int game_minutes;
extern struct settings *game_settings;
extern int hud_message_ptrs[];
extern int D_00195C40;
extern int D_00195C44;
extern char D_00195CDC[];
extern int D_00195CE0;
extern int D_00195CE4;
extern int D_00195D48;
extern signed char climate_weathers[];
extern short D_00195F62;
extern char D_00195FB1[];
extern char D_001961F5[];
extern signed char D_00196263;
extern char D_00196265[];
extern signed char D_00196266;
extern signed char current_region;
extern signed char weapon_active_hand;
extern signed char mouse_buttons_prev;
extern signed char player_underwater;
extern signed char in_dungeon_water;
extern signed char crime_current;
extern char D_0019627F[];
extern signed char D_00196280;
extern signed char D_00196289;
extern signed char D_0019629B;
extern signed char D_0019966C;
extern int quest_debug_data;
extern int D_00199808;
extern int save_file_handle;
extern struct record *D_001A4FE0;
extern int save_version;
extern char cfg_mapsave_file[];
extern int D_001A94C4;
extern int climate_index;
extern int D_001A99F8;
extern int D_001A99FC;
extern int D_001A9A00;
extern int D_001A9A04;
extern int D_001AA540;
extern int D_001AA544;
extern struct record *inv_left_container;
extern int D_001AA580;

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

    l_24 = *(struct record **)&D_00195C44;
    l_18 = func_00079A28(l_24);
    while (l_18 != 0) {
        l_24->prev = 0;
        l_24->next = l_24->prev;
        l_24->parent = l_24->next;
        l_24->children = l_24->parent;
        l_20 = savetree_attach_record(a1, l_24, l_18);
        savetree_register_record(l_20);
        l_1C = l_20->id;
        l_18 = func_00079A28(l_24);
    }
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
    while (a1 != 0) {
        if (savetree_should_save(a1) != 0) {
            savetree_write_subtree(a1);
        } else {
            savetree_write_saved(a1->children);
        }
        a1 = a1->next;
    }
}

struct record *savetree_attach_record(struct record *a1, struct record *a2, int a3)
{
    struct record *l_18;
    struct record *l_14;

    l_18 = object_find_by_id(a1, a2->parent_id);
    if (l_18 != 0) {
        l_14 = object_find_by_id(a1, a2->id);
        if (l_14 != 0 && l_14->type == a2->type && l_14->parent->id == a2->parent_id) {
            if (l_14->type == 34) {
                if ((l_14->image & 127) != (a2->image & 127)) {
                    l_14 = object_create_child(l_18, a2, a3 - 71);
                    l_14->parent_id = 0;
                    return l_14;
                }
            }
            mc_memcpy(l_14, a2, 55, (int)D_00176884, 217, 4);
            mc_memcpy(&l_14->data, &a2->data, (int)&*(signed char *)((char *)mem_block_size((int)l_14) - 71), (int)D_00176884, 218, 4);
            l_14->parent_id = 0;
            return l_14;
        }
        l_14 = object_create_child(l_18, a2, a3 - 71);
        l_14->parent_id = 0;
        return l_14;
    }
    fatal_error((int)D_001768A8);
    return 0;
}

int savetree_should_save(struct record *a1)
{
    if (a1->quest_id != 0) return 1;
    if (func_000641CD(a1) != 0) return 1;
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
        if (a1->children != 0) return 1;
        if (a1->pad19 != 0) return 1;
        break;
    case 34:
        if (((a1->image & 31) - 2) == 14 || ((a1->image & 31) - 2) == 13) return 1;
        break;
    default:
L7A00D:;
        return 0;
    }
    return 0;
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
        if (inventory_containers[a1->image] == 0 || inventory_containers[a1->image]->children == 0) {
            inventory_containers[a1->image] = a1;
        }
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
    for (l_18 = 0; l_18 < 27; l_18++) {
        if (l_24->equipped[l_18] != 0) {
            l_24->equipped[l_18] = object_find_by_id(D_00195AC4, (int)l_24->equipped[l_18]);
        }
    }
    if (l_24->target != 0) {
        l_24->target = object_find_by_id(D_00195AC4, (int)l_24->target);
        if (l_24->target == 0) fatal_error((int)D_001768BD);
    }
    if (a1->type != 18) if (a1->type != 44) return;
    func_000C010F(l_1C);
}

void save_unlink_character(struct record *a1)
{
    struct character *l_1C;
    int l_18;

    l_1C = &a1->data.character;
    for (l_18 = 0; l_18 < 27; l_18++) {
        if (l_1C->equipped[l_18] != 0) {
            l_1C->equipped[l_18] = (struct record *)l_1C->equipped[l_18]->id;
        }
    }
    if (l_1C->target == 0) return;
    if (object_find_by_id(D_00195AC4, l_1C->target->id) != 0) {
        l_1C->target = (struct record *)l_1C->target->id;
        return;
    }
    l_1C->target = 0;
}

void func_0007A3D0(struct record *a1)
{
    switch (a1->type) {
    case 18:
    case 33:
    case 44:
        *(int *)((char *)(int)(*(char **)&D_00195C44 + ((*(int *)D_00195B84)++ << 2))) = a1->id;
    default:;
    }
}

void func_0007A42B(struct record *a1)
{
    int l_1C;
    int l_18;

    if (a1->type != 34 || a1->type == 32) return;
    l_1C = D_00195C44;
    for (l_18 = 0; l_18 < *(int *)D_00195B84; l_18++, (*(char (**)[4])&l_1C)++) {
        if (a1->id == *(int *)((char *)l_1C)) {
            object_delete(a1);
            *(int *)((char *)l_1C) = 0;
            return;
        }
    }
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
    if ((save_file_handle = open((int)D_001913E4, 546, 384)) < 0) {
        fatal_error((int)D_001768E4);
    }
    write(save_file_handle, D_00147954 + 24000, 4000);
    func_0009DEA7(save_file_handle);
}

int save_write_name(int a1)
{
    func_000A0ED9(516, (int)D_00176884);
    mc_sprintf((int)D_001913E4, (int)D_001768DF, (int)text_buffer, (int)D_001768FC);
    unlink((int)D_001913E4);
    save_file_handle = open((int)D_001913E4, 546, 384);
    write(save_file_handle, a1, 32);
    func_0009DEA7(save_file_handle);
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
    region_locations_save_discovered((int)(unsigned char)current_region);
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
    save_file_handle = open((int)text_rsc_buffer, 546, 384);
    write(save_file_handle, (int)savetree_version, 4);
    write(save_file_handle, (int)&player_object->x, 12);
    write(save_file_handle, (int)&D_00195AC4->image, 2);
    write(save_file_handle, (int)&player_environment, 1);
    l_18 = current_location->building_count * 26;
    write(save_file_handle, (int)&l_18, 4);
    write(save_file_handle, (int)current_location->buildings, l_18);
    l_18 = 0;
    quests_unlink_all((int)D_00195A00);
    savetree_write_saved(D_00195AC4->children);
    write(save_file_handle, (int)&l_18, 4);
    object_foreach_pre(nonworld_root->children, (int)savetree_write_record);
    write(save_file_handle, (int)&l_18, 4);
    quests_relink_all((int)D_00195A00);
    links_save(save_file_handle);
    func_0009DEA7(save_file_handle);
    mouse_buttons = (mouse_buttons_prev = 0);
    func_0012B136();
    while (mouse_buttons != 0) func_0012B136();
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
    if (((int)(unsigned char)current_region) == 31) return;
    while (l_24 != 0) {
        if (l_24->type == 14) {
            l_20 = &l_24->data.quest;
            if (stricmp(l_20->name, (int)D_00176934) == 0) l_18++;
        }
        l_24 = l_24->next;
    }
    if (l_18 != 0 || D_00195303 == 0 || D_0019530D != 0) return;
    mc_strncpy((int)D_001961F5, (int)D_0017693D, 13, (int)D_00176884, 616);
    D_001952EE = 0;
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
    l_18 = (window_image = disk_read_file((int)D_0017697F, 0));
    D_00195B5C = disk_read_file((int)D_0017698C, 0);
    mc_memset(D_00195C44, 0, 256, (int)D_00176884, 862, 4);
    mc_strncpy((int)text_buffer, (int)D_00176999, 160, (int)D_00176884, 864);
    for (l_28 = 0; l_28 < 6; l_28++) {
        D_001903A8 = *(signed char *)&l_28 + 48;
        l_1C = open((int)text_buffer, 512);
        if (l_1C < 1) continue;
        func_000A00CB(l_1C, (int)(*(char **)&D_00147954 + (l_28 * 4000)), 4000);
        func_0009DEA7(l_1C);
        l_24 |= 1 << l_28;
    }
    mc_strncpy((int)text_buffer, (int)D_001769A9, 160, (int)D_00176884, 874);
    for (l_28 = 0; l_28 < 6; l_28++) {
        D_001903A8 = *(signed char *)&l_28 + 48;
        *(signed char *)((char *)(int)(*(char **)&D_00195C44 + (l_28 << 5))) = 0;
        l_1C = open((int)text_buffer, 512);
        if (l_1C < 1) continue;
        func_000A00CB(l_1C, (int)(*(char **)&D_00195C44 + (l_28 << 5)), 32);
        func_0009DEA7(l_1C);
    }
    while (l_2C == 0) {
        if (key_pressed_once(15) != 0) {
            l_20++;
            if (l_20 == 6) l_20 = 0;
        }
        if (key_pressed_once(1) != 0) l_2C = 1;
        saveload_draw(a1, l_24, l_20);
        mouse_buttons_prev = mouse_buttons;
        func_0012B136();
        func_00144FB4((int)(short)mouse_x, (int)(short)mouse_y, 10, 10, cursor_arrow_image);
        if (key_pressed_once(28) != 0) {
            D_0012AC02 = 1;
            l_20 = saveload_click_slot(a1, a1, l_20, l_24);
            D_0012AC02 = 0;
        } else if (D_0012AC02 != 0 || (mouse_buttons != 0 && mouse_buttons_prev == 0)) {
            for (l_28 = 0; l_28 < 14; l_28++) {
                if (mouse_x > *(short *)(saveload_buttons + (l_28 * 12)) && mouse_x < *(short *)(D_00187A94 + (l_28 * 12)) && mouse_y > *(short *)(D_00187A92 + (l_28 * 12)) && mouse_y < *(short *)(D_00187A96 + (l_28 * 12))) {
                    sound_play(203, (int)player_object, 100);
                    l_20 = ((int (*)())(*(int *)(D_00187A98 + (l_28 * 12))))(l_28, a1, l_20, l_24);
                }
                if (l_20 == (-1)) break;
            }
        }
        func_000CDD81(1);
        if (l_20 == (-1)) l_2C = 1;
    }
    if (l_18 != 0 && l_18 != (-1751672937)) {
        mc_free(l_18, (int)D_00176884, 924);
        l_18 = -1751672937;
    }
    if (D_00195B5C == 0 || D_00195B5C == (-1751672937)) return;
    mc_free(D_00195B5C, (int)D_00176884, 925);
    D_00195B5C = -1751672937;
}

int saveload_click_slot(int a1, int a2, int a3, int a4)
{
    if (D_0012AC02 != 0) {
        saveload_confirm(a1, a2, a3, a4);
        return -1;
    }
    if (a1 < 6) return a1 % 3;
    return (a1 % 3) + 3;
}

int saveload_confirm(int a1, int a2, int a3, int a4)
{
    int l_10;

    if (a2 != 0) {
        mc_strncpy((int)text_rsc_buffer, (int)(*(char **)&D_00195C44 + (a3 << 5)), 2048, (int)D_00176884, 948);
        D_0012B508 = 146;
        l_10 = D_00195C44 + 55000;
        func_000A0ED9(951, (int)D_00176884);
        mc_sprintf(l_10, (int)D_001769BC, D_001846F8);
        *(signed char *)((char *)(func_000A0DF4(l_10) + l_10) + 1) = 0;
        func_00142790();
        inpstr_begin_text((int)text_rsc_buffer, 31);
        mouse_buttons = (mouse_buttons_prev = 0);
        msgbox_show_string(l_10, 2);
        if (((int)(unsigned char)D_0019966C) == 2) return 0;
        save_game(a3, (int)text_rsc_buffer);
    } else {
        load_game(a3);
    }
    D_00190D16 = 0;
    return -1;
}

int saveload_exit(int a1, int a2, int a3, int a4)
{
    D_00190D16 = 1;
    return -1;
}

void savevars_read(int a1)
{
    func_000A0ED9(1008, (int)D_00176884);
    mc_sprintf((int)text_buffer, (int)D_00176909, a1);
    func_000A0ED9(1009, (int)D_00176884);
    mc_sprintf((int)text_rsc_buffer, (int)D_001768DF, (int)text_buffer, (int)D_001769EA);
    save_file_handle = open((int)text_rsc_buffer, 512);
    func_000A00CB(save_file_handle, (int)saved_positions, 48);
    func_000A00CB(save_file_handle, (int)bio_modifiers, 64);
    func_000A00CB(save_file_handle, (int)&view_look_pitch, 12);
    func_000A00CB(save_file_handle, (int)&D_00196266, 1);
    func_000A00CB(save_file_handle, (int)quest_faces, 100);
    func_000A00CB(save_file_handle, (int)D_001959C4, 20);
    func_000A00CB(save_file_handle, (int)travel_options, 2);
    func_000A00CB(save_file_handle, (int)D_0018DE44, 512);
    func_000A00CB(save_file_handle, (int)D_00196265, 1);
    func_000A00CB(save_file_handle, (int)D_00195FB1, 71);
    func_000A00CB(save_file_handle, (int)&D_001959FC, 4);
    func_000A00CB(save_file_handle, (int)D_00195A08, 4);
    func_000A00CB(save_file_handle, (int)&D_00195A0C, 4);
    func_000A00CB(save_file_handle, (int)&D_00195A78, 4);
    func_000A00CB(save_file_handle, (int)quest_global_states, 64);
    func_000A00CB(save_file_handle, (int)&D_00195F62, 2);
    func_000A00CB(save_file_handle, (int)&D_00196280, 1);
    func_000A00CB(save_file_handle, (int)D_001961F5, 13);
    func_000A00CB(save_file_handle, (int)D_00178A10, 4);
    func_000A00CB(save_file_handle, (int)&crime_current, 1);
    func_000A00CB(save_file_handle, (int)D_0019627F, 1);
    func_000A00CB(save_file_handle, (int)&player_underwater, 1);
    func_000A00CB(save_file_handle, (int)&in_dungeon_water, 1);
    func_000A00CB(save_file_handle, (int)&D_00178A14, 4);
    func_000A00CB(save_file_handle, (int)D_00195CDC, 4);
    func_000A00CB(save_file_handle, (int)&D_00195CE0, 4);
    func_000A00CB(save_file_handle, (int)&D_00195CE4, 4);
    func_000A00CB(save_file_handle, (int)climate_weathers, 6);
    func_000A00CB(save_file_handle, (int)&D_001940D4, 8);
    func_000A00CB(save_file_handle, (int)frame_counter, 4);
    func_000A00CB(save_file_handle, (int)&game_minutes, 4);
    func_000A00CB(save_file_handle, (int)&D_00195C40, 4);
    func_000A00CB(save_file_handle, (int)clothing_gender_group, 4);
    func_000A00CB(save_file_handle, (int)D_00195B18, 4);
    func_000A00CB(save_file_handle, (int)&weapon_active_hand, 1);
    func_000A00CB(save_file_handle, (int)region_event_values, 4960);
    func_000A00CB(save_file_handle, (int)&current_region, 1);
    func_000A00CB(save_file_handle, (int)cheat_flags, 4);
    func_000A00CB(save_file_handle, (int)&vertical_velocity, 4);
    func_000A00CB(save_file_handle, (int)D_00195AAC, 4);
    func_000A00CB(save_file_handle, (int)&jump_velocity, 4);
    func_000A00CB(save_file_handle, (int)D_00178A18, 2);
    func_000A00CB(save_file_handle, (int)&bank_house_price, 4);
    func_000A00CB(save_file_handle, (int)&bank_ship_price, 4);
    func_000A00CB(save_file_handle, (int)saved_location_name, 32);
    func_000A00CB(save_file_handle, (int)saved_region_name, 32);
    func_000A00CB(save_file_handle, (int)&D_00196263, 1);
    func_000A00CB(save_file_handle, (int)D_0019599C, 4);
    func_000A00CB(save_file_handle, (int)&D_00195998, 4);
    func_000A00CB(save_file_handle, (int)&climate_index, 4);
    func_000A00CB(save_file_handle, (int)climate_weathers, 6);
    func_000A00CB(save_file_handle, (int)&dungeon_water_level, 4);
    func_000A00CB(save_file_handle, (int)&D_00187F28, 4);
    func_000A00CB(save_file_handle, (int)&D_001A9A04, 4);
    func_000A00CB(save_file_handle, (int)&D_001A99F8, 4);
    func_000A00CB(save_file_handle, (int)&D_001A99FC, 4);
    func_000A00CB(save_file_handle, (int)&D_001A9A00, 4);
    if (save_version >= 294) {
        func_000A00CB(save_file_handle, (int)&D_001AA540, 4);
        func_000A00CB(save_file_handle, (int)&D_001AA544, 4);
        func_000A00CB(save_file_handle, (int)&D_001AA580, 4);
    }
    faction_load(save_file_handle);
    func_0009DEA7(save_file_handle);
}

void savevars_write(int a1)
{
    func_000A0ED9(1092, (int)D_00176884);
    mc_sprintf((int)text_rsc_buffer, (int)D_001768DF, (int)text_buffer, (int)D_001769EA);
    unlink((int)text_rsc_buffer);
    save_file_handle = open((int)text_rsc_buffer, 546, 384);
    write(save_file_handle, (int)saved_positions, 48);
    write(save_file_handle, (int)bio_modifiers, 64);
    write(save_file_handle, (int)&view_look_pitch, 12);
    write(save_file_handle, (int)&D_00196266, 1);
    write(save_file_handle, (int)quest_faces, 100);
    write(save_file_handle, (int)D_001959C4, 20);
    write(save_file_handle, (int)travel_options, 2);
    write(save_file_handle, (int)D_0018DE44, 512);
    write(save_file_handle, (int)D_00196265, 1);
    write(save_file_handle, (int)D_00195FB1, 71);
    write(save_file_handle, (int)&D_001959FC, 4);
    write(save_file_handle, (int)D_00195A08, 4);
    write(save_file_handle, (int)&D_00195A0C, 4);
    write(save_file_handle, (int)&D_00195A78, 4);
    write(save_file_handle, (int)quest_global_states, 64);
    write(save_file_handle, (int)&D_00195F62, 2);
    write(save_file_handle, (int)&D_00196280, 1);
    write(save_file_handle, (int)D_001961F5, 13);
    write(save_file_handle, (int)D_00178A10, 4);
    write(save_file_handle, (int)&crime_current, 1);
    write(save_file_handle, (int)D_0019627F, 1);
    write(save_file_handle, (int)&player_underwater, 1);
    write(save_file_handle, (int)&in_dungeon_water, 1);
    write(save_file_handle, (int)&D_00178A14, 4);
    write(save_file_handle, (int)D_00195CDC, 4);
    write(save_file_handle, (int)&D_00195CE0, 4);
    write(save_file_handle, (int)&D_00195CE4, 4);
    write(save_file_handle, (int)climate_weathers, 6);
    write(save_file_handle, (int)&D_001940D4, 8);
    write(save_file_handle, (int)frame_counter, 4);
    write(save_file_handle, (int)&game_minutes, 4);
    write(save_file_handle, (int)&D_00195C40, 4);
    write(save_file_handle, (int)clothing_gender_group, 4);
    write(save_file_handle, (int)D_00195B18, 4);
    write(save_file_handle, (int)&weapon_active_hand, 1);
    write(save_file_handle, (int)region_event_values, 4960);
    write(save_file_handle, (int)&current_region, 1);
    write(save_file_handle, (int)cheat_flags, 4);
    write(save_file_handle, (int)&vertical_velocity, 4);
    write(save_file_handle, (int)D_00195AAC, 4);
    write(save_file_handle, (int)&jump_velocity, 4);
    write(save_file_handle, (int)D_00178A18, 2);
    write(save_file_handle, (int)&bank_house_price, 4);
    write(save_file_handle, (int)&bank_ship_price, 4);
    write(save_file_handle, (int)saved_location_name, 32);
    write(save_file_handle, (int)saved_region_name, 32);
    write(save_file_handle, (int)&D_00196263, 1);
    write(save_file_handle, (int)D_0019599C, 4);
    write(save_file_handle, (int)&D_00195998, 4);
    write(save_file_handle, (int)&climate_index, 4);
    write(save_file_handle, (int)climate_weathers, 6);
    write(save_file_handle, (int)&dungeon_water_level, 4);
    write(save_file_handle, (int)&D_00187F28, 4);
    write(save_file_handle, (int)&D_001A9A04, 4);
    write(save_file_handle, (int)&D_001A99F8, 4);
    write(save_file_handle, (int)&D_001A99FC, 4);
    write(save_file_handle, (int)&D_001A9A00, 4);
    write(save_file_handle, (int)&D_001AA540, 4);
    write(save_file_handle, (int)&D_001AA544, 4);
    write(save_file_handle, (int)&D_001AA580, 4);
    faction_save(save_file_handle);
    func_0009DEA7(save_file_handle);
}

void func_0007C432(void)
{
    int l_1C;
    int l_18;

    D_00196289 = 1;
    quest_debug_data = 0;
    D_001A94C4 = -1;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        func_0012A2D0(160, 100, 160, 100);
    } else {
        func_0012A2D0(160, 77, 160, 77);
    }
    func_0012A274(200, 180);
    D_00195D48 = 10000;
    D_0019629B = 0;
    mc_memset((int)hud_message_ptrs, 0, 68, (int)D_00176884, 1188, 68);
    mc_memset((int)hud_message_expiry, 0, 68, (int)D_00176884, 1189, 68);
    D_00195B44 = game_minutes;
    D_001940D7 |= 1;
    D_001940D5 |= 2;
    D_001940D8 |= 8;
    current_building = 0;
    l_18 = ((unsigned)game_minutes) % 1440;
    if (l_18 > 360 && l_18 < 1080) {
        l_1C = 1;
    } else {
        l_1C = 0;
    }
    D_00199808 = l_1C;
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

        if (a1->twin != 0) {
            if (a1->quest_id != 0) {
                l_3C = object_find_by_id(D_001A4FE0, (int)a1->twin);
                a1->twin = l_3C;
                if ((int)D_001A4FE0 == (int)D_00195AC4) {
                    if (a1->twin == 0) fatal_error((int)D_001769F7);
                    if (a1->twin->twin == 0) a1->twin->twin = (struct record *)a1;
                }
            } else {
                a1->twin = 0;
            }
        }
        switch (a1->type) {
        case 52:
            if (a1 != inventory_containers[a1->image]) {
                while (a1->children != 0) {
                    object_reparent(inventory_containers[a1->image], a1->children);
                }
                a1->type = 0;
            }
            return;
        case 33:
            if (a1->parent->type == 52 && a1->parent->image < 5) a1->type = 2;
            return;
        case 43:
            l_30 = (int)RECORD_DATA(a1);
            l_2C = *(int *)((char *)l_30 + 5);
            for (l_20 = 0; ((int)(unsigned char)*(signed char *)((char *)l_30)) > l_20; l_20++, (*(char (**)[66])&l_2C)++) {
                l_34 = l_2C + 4;
                *(int *)((char *)l_34) = 0;
            }
            return;
        case 56:
            l_2C = (int)RECORD_DATA(a1);
            for (l_20 = 0; a1->image > l_20; l_20++, (*(char (**)[66])&l_2C)++) {
                l_34 = l_2C + 4;
                *(int *)((char *)l_34) = 0;
            }
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
        {
            int l_20;
            if (a1->parent->type == 47 || a1->parent->type == 38 || a1->parent->type == 1) {
                l_20 = (int)RECORD_DATA(a1);
                if ((a1->flags & 8192) == 0) {
                    object_delete(a1);
                    return;
                }
                if (a1->caster == player_entity) {
                    object_delete(a1);
                    return;
                }
                if ((((unsigned)a1->id) >> 16) != (((unsigned)D_00195AC4->id) >> 16)) {
                    a1->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
                }
                return;
            }
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

    *(short *)&l_10 = (int)(unsigned char)D_0012B508;
    D_0012B508 = 0;
    text_draw_centred(a1, (int)&*(signed char *)((char *)((int)(short)*(short *)&a2) + 1), (int)&*(signed char *)((char *)((int)(short)*(short *)&a3) + 1));
    D_0012B508 = *(signed char *)&l_10;
    text_draw_centred(a1, (int)(short)*(short *)&a2, (int)(short)*(short *)&a3);
}

void func_0007C965(int a1, int a2, int a3)
{
    int l_10;

    *(short *)&l_10 = (int)(unsigned char)D_0012B508;
    D_0012B508 = 15;
    text_draw(a1, (int)&*(signed char *)((char *)((int)(short)*(short *)&a2) + 1), (int)&*(signed char *)((char *)((int)(short)*(short *)&a3) + 1));
    D_0012B508 = *(signed char *)&l_10;
    text_draw(a1, (int)(short)*(short *)&a2, (int)(short)*(short *)&a3);
}

void func_0007C9C2(int a1, int a2, int a3)
{
    int l_10;

    *(short *)&l_10 = (int)(unsigned char)D_0012B508;
    D_0012B508 = 0;
    text_draw(a1, (int)&*(signed char *)((char *)((int)(short)*(short *)&a2) + 1), (int)&*(signed char *)((char *)((int)(short)*(short *)&a3) + 1));
    D_0012B508 = *(signed char *)&l_10;
    text_draw(a1, (int)(short)*(short *)&a2, (int)(short)*(short *)&a3);
}

void func_0007CAEB(short a1, short a2, short a3, short a4, short a5, short a6, short a7)
{
    char l_24[12];

    text_draw_centered_colored(func_000A0DD9((int)(short)a5, (int)l_24, 10), (int)(short)((((int)(short)a1) + ((int)(short)a3)) >> 1), (int)(short)(((((int)(short)a2) + ((int)(short)a4)) >> 1) - 2), (int)(short)a6, (int)(short)a7);
}
