/* loadsave.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char mouse_buttons;
extern signed char mouse_double_click;
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
extern int guards_timer;
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
extern signed char scratch_190d16;
extern signed char text_rsc_buffer[];
extern char D_001913E4[];
extern char arena2_path[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern unsigned char D_001940D7;
extern signed char D_001940D8;
extern signed char quest_global_states[];
extern signed char D_001952EE;
extern signed char D_00195303;
extern signed char D_0019530D;
extern int last_skill_check_minutes;
extern char loan_collectors_next[];
extern int bank_ship_price;
extern int bank_house_price;
extern struct record *nonworld_root;
extern char frame_counter[];
extern struct record *logbook_object;
extern struct record *options_object;
extern int view_look_pitch;
extern char D_001959C4[];
extern struct record *inventory_containers[];
extern int spell_points_bonus;
extern struct record *quest_root;
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
extern struct record *location_object;
extern char cheat_flags[];
extern char clothing_gender_group[];
extern char clothing_gender_offset[];
extern int D_00195B44;
extern char D_00195B5C[];
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
extern int realtime_clock_tick;
extern char scratch_buffer[];
extern char breath_remaining[];
extern int location_grid_x;
extern int location_grid_z;
extern int sky_loaded_frame;
extern signed char climate_weathers[];
extern short spell_ready_cost;
extern char saved_player_object[];
extern char D_001961F5[];
extern signed char in_knightly_order_hall;
extern char D_00196265[];
extern signed char text_macro_imperial;
extern signed char current_region;
extern signed char weapon_active_hand;
extern signed char mouse_buttons_prev;
extern signed char player_underwater;
extern signed char in_dungeon_water;
extern signed char crime_current;
extern char people_witness_flags[];
extern signed char is_daytime;
extern signed char world_loading;
extern signed char night_sky_loaded;
extern signed char D_0019966C;
extern int quest_debug_data;
extern int daylight;
extern int save_file_handle;
extern struct record *load_relink_root;
extern int save_version;
extern char cfg_mapsave_file[];
extern int terrain_cell_at_player;
extern int climate_index;
extern int recall_anchor_location;
extern int recall_anchor_region;
extern int D_001A9A00;
extern int recall_anchor_environment;
extern int D_001AA540;
extern int D_001AA544;
extern struct record *inv_left_container;
extern int D_001AA580;

extern int func_000641CD(struct record *);
extern int sound_play(int, int, int);
extern int mem_block_size(int);
extern int disk_read_file(int, int);
extern int savetree_read_chunk(struct record *);
extern int savetree_write_record(struct record *);
extern int load_game(int);
extern int key_pressed_once(unsigned char);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern int open(int, ...);
extern int close();
extern int mc_free();
extern int mc_memset();
extern int read();
extern int mc_strncpy();
extern int write();
extern int itoa();
extern int strlen();
extern int stricmp();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int unlink();
extern int mc_memcpy();
extern int xn_anim_reset();
extern int xn_gfx_present_inclusive();
extern int xn_cam_set_focal();
extern int xn_cam_set_view_window();
extern int xn_mouse_poll_clamped();
extern int xn_kbd_flush();
extern int xn_draw_image_transparent();
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
extern void text_draw_centred_coloured();
extern void inpstr_begin_text(int, short);
extern void object_foreach_pre(struct record *, int);
extern void object_foreach_post(struct record *, int);
extern void object_foreach(struct record *, int);
struct record *savetree_attach_record(struct record *, struct record *, int);
int savetree_should_save(struct record *);
int save_write_name(char *);
int save_game(int, char *);
int saveload_click_slot(int, int, int, int);
int saveload_confirm(int, int, int, int);
void savetree_write_subtree(struct record *);
void savetree_write_saved(struct record *);
void savetree_register_record(struct record *);
void load_relink_object_cb(struct record *);
void load_relink_character(struct record *);
void load_collect_spawned_ids_cb(struct record *);
void load_drop_spawned_marker_cb(struct record *);
void save_write_image(void);
void savevars_write(int);
void load_fix_object_cb(struct record *);
void load_fix_ids_cb(struct record *);
#pragma aux mc_set_location parm routine [];

void savetree_read_records(struct record *root)
{
    struct record *record;
    struct record *object;
    int id;
    int size;

    record = *(struct record **)scratch_buffer;
    size = savetree_read_chunk(record);
    while (size != 0) {
        record->prev = 0;
        record->next = record->prev;
        record->parent = record->next;
        record->children = record->parent;
        object = savetree_attach_record(root, record, size);
        savetree_register_record(object);
        id = object->id;
        size = savetree_read_chunk(record);
    }
}

void savetree_write_subtree(struct record *object)
{
    struct record *next;

    next = object->next;
    object->next = 0;
    object_foreach_pre(object, (int)savetree_write_record);
    object->next = next;
}

void savetree_write_saved(struct record *object)
{
    int unused2;
    int unused3;
    int unused;

    unused = 0;
    while (object != 0) {
        if (savetree_should_save(object) != 0) {
            savetree_write_subtree(object);
        } else {
            savetree_write_saved(object->children);
        }
        object = object->next;
    }
}

struct record *savetree_attach_record(struct record *root, struct record *record, int size)
{
    struct record *parent;
    struct record *object;

    parent = object_find_by_id(root, record->parent_id);
    if (parent != 0) {
        object = object_find_by_id(root, record->id);
        if (object != 0 && object->type == record->type && object->parent->id == record->parent_id) {
            if (object->type == 34) {
                if ((object->image & 127) != (record->image & 127)) {
                    object = object_create_child(parent, record, size - 71);
                    object->parent_id = 0;
                    return object;
                }
            }
            mc_memcpy(object, record, 55, (int)D_00176884, 217, 4);
            mc_memcpy(&object->data, &record->data, (int)&*(signed char *)((char *)mem_block_size((int)object) - 71), (int)D_00176884, 218, 4);
            object->parent_id = 0;
            return object;
        }
        object = object_create_child(parent, record, size - 71);
        object->parent_id = 0;
        return object;
    }
    fatal_error((int)D_001768A8);
    return 0;
}

int savetree_should_save(struct record *object)
{
    if (object->quest_id != 0) return 1;
    if (func_000641CD(object) != 0) return 1;
    switch (object->type) {
    case 8:
        return object->flags & 512;
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
        if (object->children != 0) return 1;
        if (object->pad19 != 0) return 1;
        break;
    case 34:
        if (((object->image & 31) - 2) == 14 || ((object->image & 31) - 2) == 13) return 1;
        break;
    default:
        return 0;
    }
    return 0;
}

void savetree_register_record(struct record *object)
{
    switch (object->type) {
        return;
    case 4:
        player_object = object;
        return;
    case 5:
        camera_object = object;
        return;
    case 3:
        player_class = &(player_character = &(player_entity = object)->data.character)->career;
        return;
    case 52:
        if (inventory_containers[object->image] == 0 || inventory_containers[object->image]->children == 0) {
            inventory_containers[object->image] = object;
        }
        return;
    case 16:
        quest_root = object;
        return;
    case 23:
        game_settings = (struct settings *)((*(int *)&options_object = (int)object) + 71);
        return;
    case 24:
        logbook_object = object;
        return;
    case 25:
        bank_accounts = object;
        return;
    case 39:
        nonworld_root = object;
    default:;
    }
}

void load_relink_object_cb(struct record *object)
{
    switch (object->type) {
case 3:
case 18:
case 44:
    load_relink_character(object);
    return;
case 9:
    object->caster = object_find_by_id(location_object, (int)object->caster);
default:;
}
}

void load_relink_all(void)
{
    object_foreach(location_object, (int)load_relink_object_cb);
    object_foreach(nonworld_root, (int)load_relink_object_cb);
    inv_left_container = inventory_containers[0];
}

void load_relink_character(struct record *object)
{
    struct character *character;
    struct career *career;
    struct monster_anim *anim;
    int i;

    character = &object->data.character;
    career = &character->career;
    anim = (struct monster_anim *)(career + 1);
    for (i = 0; i < 27; i++) {
        if (character->equipped[i] != 0) {
            character->equipped[i] = object_find_by_id(location_object, (int)character->equipped[i]);
        }
    }
    if (character->target != 0) {
        character->target = object_find_by_id(location_object, (int)character->target);
        if (character->target == 0) fatal_error((int)D_001768BD);
    }
    if (object->type != 18) if (object->type != 44) return;
    xn_anim_reset(anim);
}

void save_unlink_character(struct record *object)
{
    struct character *character;
    int i;

    character = &object->data.character;
    for (i = 0; i < 27; i++) {
        if (character->equipped[i] != 0) {
            character->equipped[i] = (struct record *)character->equipped[i]->id;
        }
    }
    if (character->target == 0) return;
    if (object_find_by_id(location_object, character->target->id) != 0) {
        character->target = (struct record *)character->target->id;
        return;
    }
    character->target = 0;
}

void load_collect_spawned_ids_cb(struct record *object)
{
    switch (object->type) {
case 18:
case 33:
case 44:
    *(int *)((char *)(int)(*(char **)scratch_buffer + ((*(int *)D_00195B84)++ << 2))) = object->id;
default:;
}
}

void load_drop_spawned_marker_cb(struct record *object)
{
    int *ids;
    int i;

    if (object->type != 34 || object->type == 32) return;
    ids = *(int **)scratch_buffer;
    for (i = 0; i < *(int *)D_00195B84; i++, ids++) {
        if (object->id == *ids) {
            object_delete(object);
            *ids = 0;
            return;
        }
    }
}

void load_drop_spawned_markers(void)
{
    *(int *)D_00195B84 = 0;
    object_foreach(location_object->children, (int)load_collect_spawned_ids_cb);
    object_foreach_post(location_object->children, (int)load_drop_spawned_marker_cb);
}

void save_write_image(void)
{
    mc_set_location(494, (int)D_00176884);
    mc_sprintf((int)D_001913E4, (int)D_001768DF, (int)text_buffer, (int)D_001768D5);
    unlink((int)D_001913E4);
    if ((save_file_handle = open((int)D_001913E4, 546, 384)) < 0) {
        fatal_error((int)D_001768E4);
    }
    write(save_file_handle, D_00147954 + 24000, 4000);
    close(save_file_handle);
}

int save_write_name(char *name)
{
    mc_set_location(516, (int)D_00176884);
    mc_sprintf((int)D_001913E4, (int)D_001768DF, (int)text_buffer, (int)D_001768FC);
    unlink((int)D_001913E4);
    save_file_handle = open((int)D_001913E4, 546, 384);
    write(save_file_handle, name, 32);
    close(save_file_handle);
    return 0;
}

int save_game(int slot, char *name)
{
    int size;

    mem_check_now(1000);
    mc_set_location(540, (int)D_00176884);
    mc_sprintf((int)text_buffer, (int)D_00176909, slot);
    disk_delete_matching((int)text_buffer, (int)D_00176911);
    save_write_image();
    save_write_name(name);
    savevars_write(slot);
    region_locations_save_discovered((int)(unsigned char)current_region);
    automap_save();
    mc_set_location(550, (int)D_00176884);
    mc_sprintf((int)text_buffer, (int)D_00176909, slot);
    disk_copy_file((int)D_00176915, (int)arena2_path, (int)text_buffer);
    disk_copy_file((int)D_0017691F, (int)arena2_path, (int)text_buffer);
    disk_copy_file((int)cfg_mapsave_file, (int)arena2_path, (int)text_buffer);
    save_copy_automap_files(slot);
    mc_set_location(558, (int)D_00176884);
    mc_sprintf((int)text_rsc_buffer, (int)D_001768DF, (int)text_buffer, (int)D_00176927);
    unlink((int)text_rsc_buffer);
    save_file_handle = open((int)text_rsc_buffer, 546, 384);
    write(save_file_handle, (int)savetree_version, 4);
    write(save_file_handle, (int)&player_object->x, 12);
    write(save_file_handle, (int)&location_object->image, 2);
    write(save_file_handle, (int)&player_environment, 1);
    size = current_location->building_count * 26;
    write(save_file_handle, (int)&size, 4);
    write(save_file_handle, (int)current_location->buildings, size);
    size = 0;
    quests_unlink_all((int)quest_root);
    savetree_write_saved(location_object->children);
    write(save_file_handle, (int)&size, 4);
    object_foreach_pre(nonworld_root->children, (int)savetree_write_record);
    write(save_file_handle, (int)&size, 4);
    quests_relink_all((int)quest_root);
    links_save(save_file_handle);
    close(save_file_handle);
    mouse_buttons = (mouse_buttons_prev = 0);
    xn_mouse_poll_clamped();
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    mem_check_now(1001);
    return 0;
}

void load_requeue_s0000021(void)
{
    struct record *object;
    struct quest *quest;
    int unused;
    int count;

    object = quest_root->children;
    unused = 0;
    count = 0;
    if (((int)(unsigned char)current_region) == 31) return;
    while (object != 0) {
        if (object->type == 14) {
            quest = &object->data.quest;
            if (stricmp(quest->name, (int)D_00176934) == 0) count++;
        }
        object = object->next;
    }
    if (count != 0 || D_00195303 == 0 || D_0019530D != 0) return;
    mc_strncpy((int)D_001961F5, (int)D_0017693D, 13, (int)D_00176884, 616);
    D_001952EE = 0;
}

void saveload_menu(int saving)
{
    int done;
    int i;
    int used_slots;
    int slot;
    int handle;
    int window;

    done = 0;
    used_slots = 0;
    slot = 0;
    window = (window_image = disk_read_file((int)D_0017697F, 0));
    *(int *)D_00195B5C = disk_read_file((int)D_0017698C, 0);
    mc_memset(*(int *)scratch_buffer, 0, 256, (int)D_00176884, 862, 4);
    mc_strncpy((int)text_buffer, (int)D_00176999, 160, (int)D_00176884, 864);
    for (i = 0; i < 6; i++) {
        D_001903A8 = *(signed char *)&i + 48;
        handle = open((int)text_buffer, 512);
        if (handle < 1) continue;
        read(handle, (int)(*(char **)&D_00147954 + (i * 4000)), 4000);
        close(handle);
        used_slots |= 1 << i;
    }
    mc_strncpy((int)text_buffer, (int)D_001769A9, 160, (int)D_00176884, 874);
    for (i = 0; i < 6; i++) {
        D_001903A8 = *(signed char *)&i + 48;
        *(signed char *)((char *)(int)(*(char **)scratch_buffer + (i << 5))) = 0;
        handle = open((int)text_buffer, 512);
        if (handle < 1) continue;
        read(handle, (int)(*(char **)scratch_buffer + (i << 5)), 32);
        close(handle);
    }
    while (done == 0) {
        if (key_pressed_once(15) != 0) {
            slot++;
            if (slot == 6) slot = 0;
        }
        if (key_pressed_once(1) != 0) done = 1;
        saveload_draw(saving, used_slots, slot);
        mouse_buttons_prev = mouse_buttons;
        xn_mouse_poll_clamped();
        xn_draw_image_transparent((int)(short)mouse_x, (int)(short)mouse_y, 10, 10, cursor_arrow_image);
        if (key_pressed_once(28) != 0) {
            mouse_double_click = 1;
            slot = saveload_click_slot(saving, saving, slot, used_slots);
            mouse_double_click = 0;
        } else if (mouse_double_click != 0 || (mouse_buttons != 0 && mouse_buttons_prev == 0)) {
            for (i = 0; i < 14; i++) {
                if (mouse_x > *(short *)(saveload_buttons + (i * 12)) && mouse_x < *(short *)(D_00187A94 + (i * 12)) && mouse_y > *(short *)(D_00187A92 + (i * 12)) && mouse_y < *(short *)(D_00187A96 + (i * 12))) {
                    sound_play(203, (int)player_object, 100);
                    slot = ((int (*)())(*(int *)(D_00187A98 + (i * 12))))(i, saving, slot, used_slots);
                }
                if (slot == (-1)) break;
            }
        }
        xn_gfx_present_inclusive(1);
        if (slot == (-1)) done = 1;
    }
    if (window != 0 && window != (-1751672937)) {
        mc_free(window, (int)D_00176884, 924);
        window = -1751672937;
    }
    if (*(int *)D_00195B5C == 0 || *(int *)D_00195B5C == (-1751672937)) return;
    mc_free(*(int *)D_00195B5C, (int)D_00176884, 925);
    *(int *)D_00195B5C = -1751672937;
}

int saveload_click_slot(int button, int saving, int slot, int used_slots)
{
    if (mouse_double_click != 0) {
        saveload_confirm(button, saving, slot, used_slots);
        return -1;
    }
    if (button < 6) return button % 3;
    return (button % 3) + 3;
}

int saveload_confirm(int button, int saving, int slot, int used_slots)
{
    int prompt;

    if (saving != 0) {
        mc_strncpy((int)text_rsc_buffer, (int)(*(char **)scratch_buffer + (slot << 5)), 2048, (int)D_00176884, 948);
        D_0012B508 = 146;
        prompt = *(int *)scratch_buffer + 55000;
        mc_set_location(951, (int)D_00176884);
        mc_sprintf(prompt, (int)D_001769BC, D_001846F8);
        *(signed char *)((char *)(strlen(prompt) + prompt) + 1) = 0;
        xn_kbd_flush();
        inpstr_begin_text((int)text_rsc_buffer, 31);
        mouse_buttons = (mouse_buttons_prev = 0);
        msgbox_show_string(prompt, 2);
        if (((int)(unsigned char)D_0019966C) == 2) return 0;
        save_game(slot, (char *)text_rsc_buffer);
    } else {
        load_game(slot);
    }
    scratch_190d16 = 0;
    return -1;
}

int saveload_exit(int button, int saving, int slot, int used_slots)
{
    scratch_190d16 = 1;
    return -1;
}

void savevars_read(int slot)
{
    mc_set_location(1008, (int)D_00176884);
    mc_sprintf((int)text_buffer, (int)D_00176909, slot);
    mc_set_location(1009, (int)D_00176884);
    mc_sprintf((int)text_rsc_buffer, (int)D_001768DF, (int)text_buffer, (int)D_001769EA);
    save_file_handle = open((int)text_rsc_buffer, 512);
    read(save_file_handle, (int)saved_positions, 48);
    read(save_file_handle, (int)bio_modifiers, 64);
    read(save_file_handle, (int)&view_look_pitch, 12);
    read(save_file_handle, (int)&text_macro_imperial, 1);
    read(save_file_handle, (int)quest_faces, 100);
    read(save_file_handle, (int)D_001959C4, 20);
    read(save_file_handle, (int)travel_options, 2);
    read(save_file_handle, (int)D_0018DE44, 512);
    read(save_file_handle, (int)D_00196265, 1);
    read(save_file_handle, (int)saved_player_object, 71);
    read(save_file_handle, (int)&spell_points_bonus, 4);
    read(save_file_handle, (int)D_00195A08, 4);
    read(save_file_handle, (int)&D_00195A0C, 4);
    read(save_file_handle, (int)&D_00195A78, 4);
    read(save_file_handle, (int)quest_global_states, 64);
    read(save_file_handle, (int)&spell_ready_cost, 2);
    read(save_file_handle, (int)&is_daytime, 1);
    read(save_file_handle, (int)D_001961F5, 13);
    read(save_file_handle, (int)D_00178A10, 4);
    read(save_file_handle, (int)&crime_current, 1);
    read(save_file_handle, (int)people_witness_flags, 1);
    read(save_file_handle, (int)&player_underwater, 1);
    read(save_file_handle, (int)&in_dungeon_water, 1);
    read(save_file_handle, (int)&guards_timer, 4);
    read(save_file_handle, (int)breath_remaining, 4);
    read(save_file_handle, (int)&location_grid_x, 4);
    read(save_file_handle, (int)&location_grid_z, 4);
    read(save_file_handle, (int)climate_weathers, 6);
    read(save_file_handle, (int)&D_001940D4, 8);
    read(save_file_handle, (int)frame_counter, 4);
    read(save_file_handle, (int)&game_minutes, 4);
    read(save_file_handle, (int)&realtime_clock_tick, 4);
    read(save_file_handle, (int)clothing_gender_group, 4);
    read(save_file_handle, (int)clothing_gender_offset, 4);
    read(save_file_handle, (int)&weapon_active_hand, 1);
    read(save_file_handle, (int)region_event_values, 4960);
    read(save_file_handle, (int)&current_region, 1);
    read(save_file_handle, (int)cheat_flags, 4);
    read(save_file_handle, (int)&vertical_velocity, 4);
    read(save_file_handle, (int)D_00195AAC, 4);
    read(save_file_handle, (int)&jump_velocity, 4);
    read(save_file_handle, (int)D_00178A18, 2);
    read(save_file_handle, (int)&bank_house_price, 4);
    read(save_file_handle, (int)&bank_ship_price, 4);
    read(save_file_handle, (int)saved_location_name, 32);
    read(save_file_handle, (int)saved_region_name, 32);
    read(save_file_handle, (int)&in_knightly_order_hall, 1);
    read(save_file_handle, (int)loan_collectors_next, 4);
    read(save_file_handle, (int)&last_skill_check_minutes, 4);
    read(save_file_handle, (int)&climate_index, 4);
    read(save_file_handle, (int)climate_weathers, 6);
    read(save_file_handle, (int)&dungeon_water_level, 4);
    read(save_file_handle, (int)&D_00187F28, 4);
    read(save_file_handle, (int)&recall_anchor_environment, 4);
    read(save_file_handle, (int)&recall_anchor_location, 4);
    read(save_file_handle, (int)&recall_anchor_region, 4);
    read(save_file_handle, (int)&D_001A9A00, 4);
    if (save_version >= 294) {
        read(save_file_handle, (int)&D_001AA540, 4);
        read(save_file_handle, (int)&D_001AA544, 4);
        read(save_file_handle, (int)&D_001AA580, 4);
    }
    faction_load(save_file_handle);
    close(save_file_handle);
}

void savevars_write(int slot)
{
    mc_set_location(1092, (int)D_00176884);
    mc_sprintf((int)text_rsc_buffer, (int)D_001768DF, (int)text_buffer, (int)D_001769EA);
    unlink((int)text_rsc_buffer);
    save_file_handle = open((int)text_rsc_buffer, 546, 384);
    write(save_file_handle, (int)saved_positions, 48);
    write(save_file_handle, (int)bio_modifiers, 64);
    write(save_file_handle, (int)&view_look_pitch, 12);
    write(save_file_handle, (int)&text_macro_imperial, 1);
    write(save_file_handle, (int)quest_faces, 100);
    write(save_file_handle, (int)D_001959C4, 20);
    write(save_file_handle, (int)travel_options, 2);
    write(save_file_handle, (int)D_0018DE44, 512);
    write(save_file_handle, (int)D_00196265, 1);
    write(save_file_handle, (int)saved_player_object, 71);
    write(save_file_handle, (int)&spell_points_bonus, 4);
    write(save_file_handle, (int)D_00195A08, 4);
    write(save_file_handle, (int)&D_00195A0C, 4);
    write(save_file_handle, (int)&D_00195A78, 4);
    write(save_file_handle, (int)quest_global_states, 64);
    write(save_file_handle, (int)&spell_ready_cost, 2);
    write(save_file_handle, (int)&is_daytime, 1);
    write(save_file_handle, (int)D_001961F5, 13);
    write(save_file_handle, (int)D_00178A10, 4);
    write(save_file_handle, (int)&crime_current, 1);
    write(save_file_handle, (int)people_witness_flags, 1);
    write(save_file_handle, (int)&player_underwater, 1);
    write(save_file_handle, (int)&in_dungeon_water, 1);
    write(save_file_handle, (int)&guards_timer, 4);
    write(save_file_handle, (int)breath_remaining, 4);
    write(save_file_handle, (int)&location_grid_x, 4);
    write(save_file_handle, (int)&location_grid_z, 4);
    write(save_file_handle, (int)climate_weathers, 6);
    write(save_file_handle, (int)&D_001940D4, 8);
    write(save_file_handle, (int)frame_counter, 4);
    write(save_file_handle, (int)&game_minutes, 4);
    write(save_file_handle, (int)&realtime_clock_tick, 4);
    write(save_file_handle, (int)clothing_gender_group, 4);
    write(save_file_handle, (int)clothing_gender_offset, 4);
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
    write(save_file_handle, (int)&in_knightly_order_hall, 1);
    write(save_file_handle, (int)loan_collectors_next, 4);
    write(save_file_handle, (int)&last_skill_check_minutes, 4);
    write(save_file_handle, (int)&climate_index, 4);
    write(save_file_handle, (int)climate_weathers, 6);
    write(save_file_handle, (int)&dungeon_water_level, 4);
    write(save_file_handle, (int)&D_00187F28, 4);
    write(save_file_handle, (int)&recall_anchor_environment, 4);
    write(save_file_handle, (int)&recall_anchor_location, 4);
    write(save_file_handle, (int)&recall_anchor_region, 4);
    write(save_file_handle, (int)&D_001A9A00, 4);
    write(save_file_handle, (int)&D_001AA540, 4);
    write(save_file_handle, (int)&D_001AA544, 4);
    write(save_file_handle, (int)&D_001AA580, 4);
    faction_save(save_file_handle);
    close(save_file_handle);
}

void load_reset_state(void)
{
    int is_day;
    int minute_of_day;

    world_loading = 1;
    quest_debug_data = 0;
    terrain_cell_at_player = -1;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) != 0) {
        xn_cam_set_view_window(160, 100, 160, 100);
    } else {
        xn_cam_set_view_window(160, 77, 160, 77);
    }
    xn_cam_set_focal(200, 180);
    sky_loaded_frame = 10000;
    night_sky_loaded = 0;
    mc_memset((int)hud_message_ptrs, 0, 68, (int)D_00176884, 1188, 68);
    mc_memset((int)hud_message_expiry, 0, 68, (int)D_00176884, 1189, 68);
    D_00195B44 = game_minutes;
    D_001940D7 |= 1;
    D_001940D5 |= 2;
    D_001940D8 |= 8;
    current_building = 0;
    minute_of_day = ((unsigned)game_minutes) % 1440;
    if (minute_of_day > 360 && minute_of_day < 1080) {
        is_day = 1;
    } else {
        is_day = 0;
    }
    daylight = is_day;
}

void load_fix_object_cb(struct record *object)
{
    char **model_ptr;
    struct block *block;
    struct block_model *model;
    int unused;
    int unused2;
    int i;
    int unused3;
    int unused4;
    {
        struct record *twin;

        if (object->twin != 0) {
            if (object->quest_id != 0) {
                twin = object_find_by_id(load_relink_root, (int)object->twin);
                object->twin = twin;
                if ((int)load_relink_root == (int)location_object) {
                    if (object->twin == 0) fatal_error((int)D_001769F7);
                    if (object->twin->twin == 0) object->twin->twin = (struct record *)object;
                }
            } else {
                object->twin = 0;
            }
        }
        switch (object->type) {
        case 52:
            if (object != inventory_containers[object->image]) {
                while (object->children != 0) {
                    object_reparent(inventory_containers[object->image], object->children);
                }
                object->type = 0;
            }
            return;
        case 33:
            if (object->parent->type == 52 && object->parent->image < 5) object->type = 2;
            return;
        case 43:
            block = &object->data.block;
            model = block->models;
            for (i = 0; block->model_count > i; i++, model++) {
                model_ptr = &model->model;
                *model_ptr = 0;
            }
            return;
        case 56:
            model = (struct block_model *)RECORD_DATA(object);
            for (i = 0; object->model_count > i; i++, model++) {
                model_ptr = &model->model;
                *model_ptr = 0;
            }
            return;
        case 6:
        case 32:
            model_ptr = (char **)RECORD_DATA(object);
            *model_ptr = 0;
        default:;
        }
    }
}

void load_fix_ids_cb(struct record *object)
{

    switch (object->type) {
    case 0:
    case 42:
        object_delete(object);
        return;
    case 9:
        if (object->parent->type == 47 || object->parent->type == 38 || object->parent->type == 1) {
            {
                int spell;
                spell = (int)RECORD_DATA(object);
            }
            if ((object->flags & 8192) == 0) {
                object_delete(object);
                return;
            }
            if (object->caster == player_entity) {
                object_delete(object);
                return;
            }
            if ((((unsigned)object->id) >> 16) != (((unsigned)location_object->id) >> 16)) {
                object->id = object_new_id(((unsigned)location_object->id) >> 16);
            }
            return;
        }
        if ((((unsigned)object->id) >> 16) == 801) return;
        object->id = object_new_id(801);
    default:;
    }
}

void load_fix_objects(void)
{
    load_relink_root = nonworld_root;
    object_foreach(location_object, (int)load_fix_object_cb);
    load_relink_root = location_object;
    object_foreach(nonworld_root, (int)load_fix_object_cb);
    object_foreach_post(location_object, (int)load_fix_ids_cb);
}

void text_draw_centred_black_shadow(int text, int x, int y)
{
    int colour;

    *(short *)&colour = (int)(unsigned char)D_0012B508;
    D_0012B508 = 0;
    text_draw_centred(text, (int)&*(signed char *)((char *)((int)(short)*(short *)&x) + 1), (int)&*(signed char *)((char *)((int)(short)*(short *)&y) + 1));
    D_0012B508 = *(signed char *)&colour;
    text_draw_centred(text, (int)(short)*(short *)&x, (int)(short)*(short *)&y);
}

void text_draw_colour15_shadow(int text, int x, int y)
{
    int colour;

    *(short *)&colour = (int)(unsigned char)D_0012B508;
    D_0012B508 = 15;
    text_draw(text, (int)&*(signed char *)((char *)((int)(short)*(short *)&x) + 1), (int)&*(signed char *)((char *)((int)(short)*(short *)&y) + 1));
    D_0012B508 = *(signed char *)&colour;
    text_draw(text, (int)(short)*(short *)&x, (int)(short)*(short *)&y);
}

void text_draw_black_shadow(int text, int x, int y)
{
    int colour;

    *(short *)&colour = (int)(unsigned char)D_0012B508;
    D_0012B508 = 0;
    text_draw(text, (int)&*(signed char *)((char *)((int)(short)*(short *)&x) + 1), (int)&*(signed char *)((char *)((int)(short)*(short *)&y) + 1));
    D_0012B508 = *(signed char *)&colour;
    text_draw(text, (int)(short)*(short *)&x, (int)(short)*(short *)&y);
}

void text_draw_number_in_box(short x0, short y0, short x1, short y1, short number, short colour, short shadow_colour)
{
    char digits[12];

    text_draw_centred_coloured(itoa((int)(short)number, (int)digits, 10), (int)(short)((((int)(short)x0) + ((int)(short)x1)) >> 1), (int)(short)(((((int)(short)y0) + ((int)(short)y1)) >> 1) - 2), (int)(short)colour, (int)(short)shadow_colour);
}
