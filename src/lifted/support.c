/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include <i86.h>
#include "records.h"
#include "bitfield.h"

extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern int xn_anim_ticks;
extern short D_001343C2;
extern signed char key_down[];
extern signed char key_down_esc;
extern iptr screen_buffer;
extern iptr D_00147954;
extern char D_00176A10[];
extern char D_00176A1A[];
extern char D_00176A27[];
extern char D_00176A42[];
extern unsigned char player_environment;
extern char fire_flat_records[];
extern char saved_positions[];
extern char D_0018DE18[];
extern char D_0018DE1C[];
extern signed char text_buffer[];
extern signed char key_was_down[];
extern struct record *creature_list[];
extern int D_00190C74;
extern int D_00190C7C;
extern signed char D_001940D4;
extern signed char D_001940D5;
extern signed char D_001940D6;
extern signed char D_001940DA;
extern struct record *location_grid[];
extern struct record *D_001959E0;
extern struct record *house_container;
extern struct record *ship_container;
extern struct record *room_storage_container;
extern struct record *repair_container;
extern struct record *detect_target;
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern int frame_ticks;
extern struct record *location_object;
extern struct building *tavern_building;
extern struct record *found_object;
extern struct image *list_popup_image;
extern int D_00195B00;
extern int creature_count;
extern struct record *scratch_object;
extern char D_00195B84[];
extern int hud_message_expiry[];
extern int hud_status_expiry;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern iptr hud_message_ptrs[];
extern iptr D_00195C3C;
extern char scratch_buffer[];
extern void (*grid_visit_func)();
extern int D_00195CF0;
extern int spell_cast_queue_count;
extern int free_later_count;
extern short D_00195F3C;
extern short D_00195F3E;
extern short D_00195F40;
extern short D_00195F42;
extern short D_00195F60;
extern char list_popup_picklist[];
extern signed char msgbox_button_keys;
extern signed char D_00196034;
extern signed char msgbox_button_ids;
extern signed char D_00196090;
extern signed char D_00196091;
extern unsigned char D_0019626F;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern signed char D_00196275;
extern signed char D_00196281;
extern signed char D_00196282;
extern signed char D_00196284;
extern struct record *people_list[];
extern int people_count;
extern struct quest *current_quest;
extern int nearest_fire_distance;
extern struct record *nearest_fire;
extern int D_001A3F40;
extern char spell_cast_queue_list[];
extern char D_001A4FEC[];
extern char free_later_list[];
extern signed char mode_stack[];
extern signed char D_001A53E9[];
extern int D_001A5408[];
extern char hud_message_text[];
extern char hud_status_text[];
extern int D_001A59C8;
extern int D_001A59CC;
extern int D_001A59D0;
extern int D_001A59D4;
extern int D_001A59D8;
extern int D_001A59DC;
extern int D_001A59E0;
extern int D_001A59E4;
extern char *info_popup_text;
extern short mode_stack_depth;
extern short D_001A5A54;

extern int tavern_room_rented(void);
extern int monster_despawn_to_marker(struct record *);
extern int place_spawn_from_marker(struct record *);
extern struct record *item_add_to_container(struct record *, int, int, int);
extern int object_weight(struct record *);
extern int spell_missile_update(struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern int location_contains(int, int);
extern iptr building_name(struct building *);
extern int picklist_poll(struct picklist *);
extern struct record *object_free_single(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
extern int object_find(struct record *, int (*)());
extern struct record *object_find_by_id(struct record *, iptr);
extern int object_new_id(int);
extern int rand();
extern int mc_memset();
extern int mc_strncpy();
extern int strlen();
extern int toupper();
extern int mc_set_location(int, iptr);
extern int mc_sprintf(iptr, ...);
extern int mc_memcpy();
extern iptr memchr();
extern int xn_math_approx_dist2d();
extern int xn_math_approx_hypot();
extern int xn_math_angle_to_point();
extern int xn_timer_read_pit();
extern int xn_mouse_poll_clamped();
extern int xn_mouse_cursor_move();
extern int xn_font_select();
extern iptr xn_tex_cache_lookup_image();
extern int xn_tex_cache_flush();
extern int xn_kbd_read_key();
extern int xn_draw_image();
extern void msgbox_show_string(char *, short);
extern void msgbox_show_quest_text(struct quest *, short, int);
extern void msgbox_show_rsc(int, int);
extern void holiday_announce(void);
extern void cast_spell_on(int, int, int);
extern void text_draw_coloured(iptr, int, int, int, unsigned char);
extern void text_draw_centred_coloured(iptr, int, int, int, unsigned char);
extern void detect_consider_creature(struct record *, int);
extern void func_0007E815(struct record *, void (*)());
extern void func_0007F0F3(int);
extern void marker_make_clutter(struct record *, struct building *);
extern void marker_make_loot_pile(struct record *, iptr);
extern void inpstr_begin_number(int);
extern void picklist_init(iptr, short, short, int, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void picklist_add(iptr, iptr, int);
extern void picklist_free(struct picklist *);
extern void picklist_draw(struct picklist *, int);
extern void object_free_children(struct record *);
extern void object_foreach_pre(struct record *, void (*)());
extern void object_foreach(struct record *, void (*)());
extern void object_foreach_skip_player(struct record *, void (*)());
extern void object_foreach_open(struct record *, void (*)());
iptr hud_message_add(char *);
int picklist_frame(struct picklist *);
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
void town_grid_visit_near(struct record *, void (*)());
void func_0007F093(struct record *);
void gold_sum_credit_cb(struct record *);
void gold_make_credit_letter(int);
void gold_delete_credit_cb(struct record *);
void func_0007F671(void);
void func_0007FCBF(struct record *);
void func_0007FD7E(struct record *);
void func_0007FDEA(struct record *);
void func_0007FEB9(struct record *);
void store_repair_item_cb(struct record *);
void restore_repair_item_cb(struct record *);
#pragma aux mc_set_location parm routine [];

void hud_status_set(char *text)
{
    int *bios_ticks;

    mc_strncpy(hud_status_text, text, 1440, D_00176A10, 97);
    bios_ticks = (int *)1132;
    hud_status_expiry = *bios_ticks + 36;
    D_00195C3C = (iptr)hud_status_text;
}

iptr hud_message_add(char *text)
{
    char *message_text;
    char *slot_text;
    int slot;
    int free_slot;
    short bios_ticks_addr;

    slot_text = hud_message_text;
    free_slot = -1;
    for (slot = 0; slot < 16; slot++, slot_text += 80) {
        if (hud_message_expiry[slot] == 0) {
            free_slot = slot;
            message_text = slot_text;
            break;
        }
    }
    if (free_slot < 0) return 0;
    mc_strncpy(message_text, text, 4, D_00176A10, 117);
    *(int *)&bios_ticks_addr = 1132;
    hud_message_expiry[free_slot] = *(int *)(*(char **)&bios_ticks_addr) + 36;
    hud_message_ptrs[free_slot] = (iptr)message_text;
    return hud_message_ptrs[slot];
}

void hud_messages_draw(void)
{
    int slot;
    int y;

    xn_font_select(4);
    slot = 0;
    for (y = 2; slot < 16; slot++, y += 9) {
        if (hud_message_expiry[slot] != 0) {
            if (((struct bf8_0_1 *)&D_001940DA)->f == 0) {
                if (((unsigned)BIOS_TICKS) > hud_message_expiry[slot]) {
                    hud_message_expiry[slot] = 0;
                }
            }
            text_draw_centred_coloured(hud_message_ptrs[slot], 160, (int)(short)*(short *)&y, 145, 156);
        }
    }
    if (hud_status_expiry == 0) return;
    if (((struct bf8_0_1 *)&D_001940DA)->f == 0) {
        if (((unsigned)BIOS_TICKS) > hud_status_expiry) {
            hud_status_expiry = 0;
        }
    }
    text_draw_centred_coloured(D_00195C3C, 160, (int)(short)*(short *)&y, 145, 156);
}

void info_popup_open(char *text)
{
    D_00196275 = D_00196272;
    D_00196272 = 2;
    info_popup_text = text;
}

int wait_key_from_list(signed char *keys, short key_count)
{
    unsigned char key;
    short i;

    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    while (mouse_buttons == 0) {
        xn_mouse_poll_clamped();
        xn_mouse_cursor_move((int)(short)mouse_x, (int)(short)mouse_y);
        key = xn_kbd_read_key();
        if (key != 0) {
            if (((struct bf8_0_1 *)&D_001940D4)->f != 0 && ((int)(unsigned char)key) == 27) {
                return -1;
            }
            if (key_count != 0) {
                key = toupper((int)(unsigned char)key);
                *(int *)&i = 0;
                for (; (short)(short)*(int *)&i < key_count; (*(int *)&i)++) {
                    if ((signed char)key == keys[(short)i]) {
                        return (int)(short)i;
                    }
                }
            }
        }
        mc_memcpy(655360, screen_buffer, 64000, (iptr)D_00176A10, 234, 4);
    }
    return -2;
}

int key_pressed_once(unsigned char scancode)
{
    if (key_down[(int)(unsigned char)scancode] != 0 && key_was_down[(int)(unsigned char)scancode] == 0) {
        key_was_down[(int)(unsigned char)scancode] = 1;
        return 1;
    }
    if (key_down[(int)(unsigned char)scancode] == 0) {
        key_was_down[(int)(unsigned char)scancode] = 0;
    }
    return 0;
}

void mode_push(void)
{
    mode_stack[((int)(short)mode_stack_depth) * 2] = D_00196272;
    D_001A53E9[((int)(short)(mode_stack_depth)++) * 2] = game_mode;
    D_0019626F = game_mode;
}

void mode_pop(void)
{
    mode_stack_depth--;
    D_00196272 = mode_stack[((int)(short)mode_stack_depth) * 2];
    game_mode = D_001A53E9[((int)(short)mode_stack_depth) * 2];
    if (mode_stack_depth != 0) {
        D_0019626F = D_001A53E9[((int)(short)mode_stack_depth) * 2];
        return;
    }
    D_0019626F = 255;
}

void size_fit(short *width, short *height, short max_width, short max_height)
{
    short scale_w;
    short scale_h;

    if (*width <= max_width && *height <= max_height) return;
    *(int *)&scale_w = (((int)(short)max_width) << 8) / ((int)(short)*width);
    *(int *)&scale_h = (((int)(short)max_height) << 8) / ((int)(short)*height);
    if ((short)(short)*(int *)&scale_h < scale_w) {
        *width = (((int)(short)*width) * ((int)(short)scale_h)) >> 8;
        *height = *(int *)&max_height;
        return;
    }
    *height = (((int)(short)*height) * ((int)(short)scale_w)) >> 8;
    *width = *(int *)&max_width;
}

void list_popup_open_strings(char *strings)
{
    if ((iptr)list_popup_image == 0) list_popup_image = (struct image *)disk_read_file(D_00176A1A, 0);
    D_00195F40 = 160 - (list_popup_image->width >> 1);
    D_00195F3E = 100 - (list_popup_image->height >> 1);
    D_00195F42 = list_popup_image->width;
    D_00195F3C = list_popup_image->height;
    picklist_init((iptr)list_popup_picklist, (int)(short)(D_00195F40 + 25), (int)(short)(D_00195F3E + 26), 140, 73, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 11), 8, 9, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 109), 8, 9, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 22), 9, 83, 146, 146, 244, 114, 0);
    while (*strings != 0) {
        picklist_add((iptr)list_popup_picklist, (iptr)strings, 0);
        strings += strlen(strings) + 1;
    }
    D_001940D4 |= 4;
}

void list_popup_open(char **strings)
{
    if ((iptr)list_popup_image == 0) list_popup_image = (struct image *)disk_read_file(D_00176A1A, 0);
    D_00195F40 = 160 - (list_popup_image->width >> 1);
    D_00195F3E = 100 - (list_popup_image->height >> 1);
    D_00195F42 = list_popup_image->width;
    D_00195F3C = list_popup_image->height;
    picklist_init((iptr)list_popup_picklist, (int)(short)(D_00195F40 + 25), (int)(short)(D_00195F3E + 26), 140, 73, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 11), 8, 9, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 109), 8, 9, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 22), 9, 83, 146, 146, 244, 114, 0);
    while (*strings != 0) {
        picklist_add((iptr)list_popup_picklist, (iptr)*strings++, 0);
    }
    D_001940D4 |= 4;
}

int picklist_frame(struct picklist *picklist)
{
    short choice;

    if (((struct bf8_0_1 *)&D_001940D4)->f != 0 && (key_down_esc != 0 || ((int)(unsigned char)(mouse_buttons & 2)) != 0)) {
        D_001940D4 &= 251;
        picklist_free(picklist);
        return -2;
    }
    *(int *)&choice = picklist_poll(picklist) - 1;
    if (((int)(short)choice) > (-1)) {
        D_001940D4 &= 251;
        picklist_free(picklist);
        return (int)(short)choice;
    }
    picklist_draw(picklist, 0);
    return -1;
}

int list_popup_update(void)
{
    xn_draw_image((int)(short)D_00195F40, (int)(short)D_00195F3E, (int)(short)D_00195F42, (int)(short)D_00195F3C, (iptr)list_popup_image->pixels);
    return picklist_frame((struct picklist *)list_popup_picklist);
}

int rand_range(int low, int high)
{
    return low + (rand() % ((high - low) + 1));
}

void object_free_later(struct record *object)
{
    if (object == 0) return;
    *(iptr *)(free_later_list + (free_later_count++ << 2)) = (iptr)object;
}

void object_free_pending(void)
{
    int i;

    for (i = 0; i < free_later_count; i++) {
        object_delete((struct record *)*(iptr *)(free_later_list + (i << 2)));
    }
    free_later_count = 0;
}

void spell_cast_queue(struct record *spell, struct record *target)
{
    *(iptr *)(spell_cast_queue_list + (spell_cast_queue_count << 3)) = (iptr)spell;
    *(iptr *)(D_001A4FEC + (spell_cast_queue_count++ << 3)) = (iptr)target;
}

void spell_cast_queued_run(void)
{
    int i;

    for (i = 0; i < spell_cast_queue_count; i++) {
        cast_spell_on(*(int *)(spell_cast_queue_list + (i << 3)), *(int *)(D_001A4FEC + (i << 3)), 0);
    }
    spell_cast_queue_count = 0;
}

void world_collect_object(struct record *object)
{
    struct item *item_data;
    int distance;

    switch (object->type) {
    case 33:
        if (((struct bf8_4_1 *)&D_001940D6)->f != 0 && player_character->detect_kind == 2) {
            if ((object->image >> 7) == 216 || object->image == 26112) detect_consider(object);
        }
        if ((object->image >> 7) == 210 && memchr((iptr)fire_flat_records, (int)(unsigned short)(object->image & 127), 8) != 0) {
            distance = xn_math_approx_hypot(player_object->y - object->y, xn_math_approx_dist2d(player_object->x, player_object->z, object->x, object->z));
            if (distance < nearest_fire_distance) {
                nearest_fire_distance = distance;
                nearest_fire = object;
            }
        }
        return;
    case 18:
        creature_list[creature_count++] = object;
        if (monster_despawn_to_marker(object) != 0) {
            creature_count--;
        } else if (((struct bf8_4_1 *)&D_001940D6)->f != 0 && player_character->detect_kind == 1) {
            detect_consider_creature(object, object->detect_distance);
        }
        return;
    case 2:
        item_data = &object->data.item;
        if (((struct bf8_4_1 *)&D_001940D6)->f != 0 && player_character->detect_kind == 0 && item_data->enchantments[0].type != (-1) && object->parent->parent != player_entity) {
            detect_consider(object);
        }
        return;
    case 9:
        if (((int)(unsigned short)(object->flags & 8192)) != 0) {
            if (spell_missile_update(object, 0) != 0) object_free_later(object);
        }
        return;
    case 34:
        switch (((int)(unsigned short)(object->image & 31)) - 2) {
        case 13:
        case 14:
            if (object->spawn_seed == 0) object->spawn_seed = rand();
            place_spawn_from_marker(object);
            break;
        case 18:
            if (((int)player_environment) == 3) {
                marker_make_clutter(object, 0);
            } else if (((int)player_environment) == 2 && current_building->type >= 17 && current_building->type <= 20) {
                marker_make_clutter(object, current_building);
            }
            break;
        case 17:
            marker_make_loot_pile(object, ((((int)player_environment) == 2) ? (iptr)object_building(player_object->parent) : 0));
        }
        return;
    case 53:
        people_list[people_count++] = object;
    default:;
    }
}

void detect_consider(struct record *object)
{
    int distance;

    distance = xn_math_approx_dist2d(player_object->x, player_object->z, object->x, object->z);
    if (distance >= *(int *)D_00195B84 || distance >= 2048) return;
    detect_target = object;
    *(int *)D_00195B84 = distance;
}

void world_collect_objects(void)
{
    struct record *object;
    struct record *next_object;

    nearest_fire = 0;
    nearest_fire_distance = 2048;
    if (((int)player_environment) == 2) {
        current_building = object_building(player_object);
    }
    spell_cast_queue_count = (free_later_count = 0);
    *(int *)D_00195B84 = 100000;
    people_count = (creature_count = 0);
    detect_target = 0;
    grid_visit_func = object_foreach_pre;
    if (((int)player_environment) < 3) {
        if (player_object->parent->type != 1) {
            object_foreach_pre(player_object->parent->children, world_collect_object);
        } else {
            grid_visit_func = object_foreach_open;
            town_grid_visit_near(player_object, world_collect_object);
        }
        object = location_object->children;
        while (object != 0) {
            next_object = object->next;
            if (object->type != 38) {
                if (object->children != 0) {
                    object_foreach_open(object->children, world_collect_object);
                }
                world_collect_object(object);
            }
            object = next_object;
        }
    } else {
        func_0007E815(player_object, world_collect_object);
        object = location_object->children;
        while (object != 0) {
            next_object = object->next;
            if (object->type != 47) {
                object_foreach_open(object->children, world_collect_object);
                world_collect_object(object);
            }
            object = next_object;
        }
    }
    grid_visit_func = object_foreach_open;
    spell_cast_queued_run();
    object_free_pending();
}

void msgbox_yes_no_quest(short message_id)
{
    D_00196271 = 0;
    msgbox_button_ids = 4;
    D_00196090 = 5;
    D_00196091 = 0;
    msgbox_button_keys = 21;
    D_00196034 = 49;
    D_0012B508 = 146;
    msgbox_show_quest_text(current_quest, (int)(short)message_id, 5);
}

void msgbox_prompt_number(int number, char *prompt)
{
    char *text;

    D_0012B508 = 146;
    text = *(char **)scratch_buffer + 55000;
    mc_set_location(610, (iptr)D_00176A10);
    mc_sprintf((iptr)text, (iptr)D_00176A27, prompt);
    text[strlen(text) + 1] = 0;
    inpstr_begin_number(number);
    msgbox_show_string(text, 2);
}

void msgbox_prompt_number_rsc(int number, int text_id)
{
    int unused;

    D_0012B508 = 146;
    msgbox_show_rsc((int)(short)*(short *)&text_id, 2);
    inpstr_begin_number(number);
}

struct building *object_building(struct record *object)
{
    int unused_1;
    int unused_2;
    int unused_3;
    int unused_4;

    unused_1 = 1;
    if (object == 0 || (iptr)object == (-1768515946)) return 0;
    while (object != 0 && object->type != 1) {
        if (object->type == 43 && ((int)(unsigned short)(object->flags & 1)) != 0) break;
        object = object->parent;
    }
    if (object == 0 || object->type == 1 || object->image == 65535) return 0;
    return &current_location->buildings[object->image];
}

int objects_within(struct record *object, struct record *other, int distance)
{
    return ((xn_math_approx_dist2d(object->x, object->z, other->x, other->z) < distance) ? 1 : 0);
}

void func_0007E066(void)
{
    func_0007F671();
    D_001A59D0 = D_00195B00;
    D_001A59C8 = D_00195B00;
}

void frame_ticks_update(void)
{
    if ((frame_ticks = (D_001A3F40 * 1828) / 256) > 100) frame_ticks = 100;
    xn_anim_ticks += frame_ticks;
    D_001343C2 &= 7;
    D_001A3F40 = 0;
}

void player_position_save(int slot)
{
    *(int *)(saved_positions + (slot * 12)) = player_object->x;
    *(int *)(D_0018DE18 + (slot * 12)) = player_object->y;
    *(int *)(D_0018DE1C + (slot * 12)) = player_object->z;
}

void player_position_restore(int slot)
{
    player_object->x = *(int *)(saved_positions + (slot * 12));
    player_object->y = *(int *)(D_0018DE18 + (slot * 12));
    player_object->z = *(int *)(D_0018DE1C + (slot * 12));
    D_001940D5 |= 2;
}

int flat_anim_finished(struct record *object)
{
    struct texture_header *image;

    image = (struct texture_header *)xn_tex_cache_lookup_image(object->image >> 7, (int)(unsigned short)(object->image & 127));
    if (image == 0) {
        xn_tex_cache_flush();
        image = (struct texture_header *)xn_tex_cache_lookup_image(object->image >> 7, (int)(unsigned short)(object->image & 127));
    }
    return ((object->anim_frame >= image->frame_count) ? 1 : 0);
}

void flat_anim_step(struct record *object)
{
    struct texture_header *image;
    int elapsed;

    image = (struct texture_header *)xn_tex_cache_lookup_image(object->image >> 7, (int)(unsigned short)(object->image & 127));
    if (image == 0) {
        xn_tex_cache_flush();
        image = (struct texture_header *)xn_tex_cache_lookup_image(object->image >> 7, (int)(unsigned short)(object->image & 127));
    }
    elapsed = ((xn_anim_ticks >> 5) - object->anim_time) << 5;
    if (elapsed < 0 || elapsed > 2000) elapsed = image->frame_time;
    if (image->frame_time > elapsed) return;
    object->anim_time = xn_anim_ticks >> 5;
    object->anim_frame++;
}

void flat_anim_restart(struct record *object)
{
    object->anim_time = xn_anim_ticks >> 5;
    object->anim_frame = 0;
}

void town_grid_visit_near(struct record *object, void (*callback)())
{
    int first_x;
    int first_z;
    int x;
    int z;

    if (location_object->image == 65535) return;
    first_x = ((object->x - location_object->x) / 4096) - 1;
    first_z = ((object->z - location_object->z) / 4096) - 1;
    for (x = first_x; (first_x + 3) > x; x++) {
        for (z = first_z; (first_z + 3) > z; z++) {
            if (x < 0 || current_location->width <= x) continue;
            if (z < 0 || current_location->height <= z) continue;
            if (location_grid[(z << 5) + x] != 0) {
                grid_visit_func(location_grid[(z << 5) + x]->children, callback);
            }
        }
    }
}

int objects_near_distance_quarter(struct record *object, struct record *other)
{
    int distance;

    distance = xn_math_approx_dist2d(object->x, object->z, other->x, other->z);
    if (distance > 512) return 0;
    return (distance << 7) / 512;
}

void buttons_draw_hover_label(int x, int y, int button_count, struct rect *buttons, char **labels)
{
    int i;

    for (i = 0; i < button_count; i++) {
        if (mouse_x > buttons[i].x0 && mouse_x < buttons[i].x1 && mouse_y > buttons[i].y0 && mouse_y < buttons[i].y1) {
            text_draw_coloured((iptr)labels[i], (int)(short)*(short *)&x, (int)(short)*(short *)&y, 145, 156);
        }
    }
}

int int_identity(int value)
{
    return value;
}

int detect_arrow_anim_frame(void)
{
    if (detect_target != 0) {
        return xn_math_angle_to_point(player_object->x, player_object->z, detect_target->x, detect_target->z) >> 6;
    }
    return rand() % 32;
}

int icon_cycle_anim_frame(void)
{
    int *bios_ticks;
    int *bios_ticks_2;

    bios_ticks = (int *)1132;
    if (((unsigned)(*bios_ticks - D_00190C7C)) > 38) {
        D_00190C74 = (D_00190C74 + 1) % 32;
        bios_ticks_2 = (int *)1132;
        D_00190C7C = *bios_ticks_2;
    }
    return D_00190C74;
}

void save_thumbnail_capture(void)
{
    int x;
    int y;
    int out;

    out = 24000;
    for (y = 0; y < 200; y += 4) {
        for (x = 0; x < 320; x += 4) {
            *(signed char *)((char *)(iptr)(*(char **)&D_00147954 + out++)) = *(signed char *)((char *)(iptr)(*(char **)&screen_buffer + ((y * 320) + x)));
        }
    }
}

int func_0007EEAF(void)
{
    int unused_1;
    int unused_2;
    int unused_3;
    int unused_4;
    int unused_5;
    int unused_6;
    int unused_7;
    int unused_8;
    int unused_9;
    int unused_10;
    int unused_11;

    D_00196281 = 0;
    return 0;
}

void func_0007EF20(void)
{
    union REGS regs;
    struct SREGS sregs;

    if (D_00196281 == 0) return;
    mc_memset(&sregs, 0, 12, (iptr)D_00176A10, 1064, 4);
    regs.w.ax = 257;
    regs.w.dx = D_001A5A54;
    int386x(49, &regs, &regs, &sregs);
    D_00196281 = 0;
}

void func_0007F093(struct record *object)
{
    if (object->type != 10) return;
    func_0007F0F3(object->data.membership.faction);
}

void func_0007F0C9(void)
{
    object_foreach(player_entity->children, func_0007F093);
}

void gold_add(int amount)
{
    if ((object_weight(player_entity) + (amount / 100)) > (carry_capacity() << 2)) {
        gold_make_credit_letter(amount);
        return;
    }
    player_character->gold += amount;
}

void gold_spend(int amount)
{
    struct record *credit_letter;

    if (((unsigned)player_character->gold) >= amount) {
        player_character->gold -= amount;
        return;
    }
    found_object = 0;
    credit_letter = gold_find_credit(amount);
    if (credit_letter != 0) {
        credit_letter->data.item.value -= amount;
        return;
    }
    free_later_count = 0;
    *(int *)D_00195B84 = amount;
    object_find(player_entity->children, gold_spend_credit_cb);
    object_free_pending();
    if (*(int *)D_00195B84 == 0) return;
    player_character->gold -= *(int *)D_00195B84;
    if (player_character->gold >= 0) return;
    player_character->gold = 0;
}

int gold_can_afford(int amount)
{
    return ((gold_total() >= amount) ? 1 : 0);
}

void gold_sum_credit_cb(struct record *object)
{
    struct item *item_data;

    if (object->type != 2) return;
    item_data = &object->data.item;
    if (item_data->group != 27 || item_data->index != 2) return;
    *(int *)D_00195B84 += item_data->value;
}

int gold_total(void)
{
    *(int *)D_00195B84 = 0;
    object_foreach(player_entity->children, gold_sum_credit_cb);
    return *(int *)D_00195B84 + player_character->gold;
}

int gold_find_credit_cb(struct record *object)
{
    struct item *item_data;

    if (object->type != 2) return 0;
    item_data = &object->data.item;
    if (item_data->group == 27 && item_data->index == 2 && ((unsigned)item_data->value) >= *(int *)D_00195B84) {
        found_object = object;
        return 1;
    }
    return 0;
}

int gold_spend_credit_cb(struct record *object)
{
    struct item *item_data;

    if (object->type != 2) return 0;
    if (*(int *)D_00195B84 == 0) return 0;
    item_data = &object->data.item;
    if (item_data->group == 27 && item_data->index == 2) {
        if (((unsigned)item_data->value) <= *(int *)D_00195B84) {
            object_free_later(object);
            *(int *)D_00195B84 -= item_data->value;
        } else {
            item_data->value -= *(int *)D_00195B84;
            *(int *)D_00195B84 = 0;
        }
    }
    return 0;
}

struct record *gold_find_credit(int amount)
{
    *(int *)D_00195B84 = amount;
    object_find(player_entity->children, gold_find_credit_cb);
    return found_object;
}

void gold_make_credit_letter(int amount)
{
    struct record *letter;

    letter = item_add_to_container(D_001959E0, 27, 2, 0);
    letter->data.item.value = amount;
}

int gold_total_alias(void)
{
    return gold_total();
}

int gold_can_carry(int amount)
{
    return (((object_weight(player_entity) + (amount / 100)) <= (carry_capacity() << 2)) ? 1 : 0);
}

void gold_delete_credit_cb(struct record *object)
{
    struct item *item_data;

    if (object->type != 2) return;
    item_data = &object->data.item;
    if (item_data->group != 27 || item_data->index != 2) {
        return;
    }
    object_delete(object);
}

void gold_remove_all(void)
{
    player_character->gold = 0;
    object_foreach(player_entity->children, gold_delete_credit_cb);
}

void func_0007F671(void)
{
    int *bios_ticks;

    D_001A59CC = xn_timer_read_pit();
    bios_ticks = (int *)1132;
    D_001A59DC = *bios_ticks;
}

void func_0007F6A4(void)
{
    int pit_elapsed;
    int i;
    int *bios_ticks;

    D_001A59E0 = D_001A59CC;
    D_001A59E4 = D_001A59DC;
    D_001A59CC = xn_timer_read_pit();
    bios_ticks = (int *)1132;
    if ((D_001A59DC = *bios_ticks) == D_001A59E4) {
        pit_elapsed = D_001A59E0 - D_001A59CC;
    } else {
        pit_elapsed = ((65535 - D_001A59CC) + D_001A59E0) + (((D_001A59DC - D_001A59E4) - 1) * 65535);
    }
    if (((unsigned)pit_elapsed) < 10000) pit_elapsed = (int)(iptr)(((char *)(iptr)D_001A59E0) + (65535 - D_001A59CC));
    D_001A5408[D_001A59D8] = ((unsigned)(((unsigned)(pit_elapsed * 1000)) / 1193180)) >> 1;
    if (((unsigned)D_001A5408[D_001A59D8]) > 100) {
        D_001A5408[D_001A59D8] = 100;
    }
    D_001A59D8 = (D_001A59D8 + 1) & 7;
    for (i = 0; i < 8; i++) {
        frame_ticks += D_001A5408[i];
    }
    frame_ticks >>= 3;
    if (frame_ticks <= 500) return;
    frame_ticks = 500;
}

void object_match_filters_cb(struct record *object)
{
    if (D_00196282 != 0 && (signed char)object->quest_id != D_00196282) {
        return;
    }
    if (D_00195CF0 != 0 && (int)object->id != D_00195CF0) return;
    if (D_00195F60 != 0 && (short)object->image2 != D_00195F60) return;
    if (D_00196284 != 0 && (signed char)object->type != D_00196284) {
        return;
    }
    found_object = object;
}

void location_restore_stored(void)
{
    struct record *stored;
    struct record *next_stored;

    if (((int)player_environment) == 3) return;
    object_foreach_skip_player(repair_container->children, restore_repair_item_cb);
    if (room_storage_container != 0 && room_storage_container->children != 0) {
        stored = room_storage_container->children;
        while (stored != 0) {
            next_stored = stored->next;
            if (stored->type == 64 && ((unsigned)stored->building_id) < game_minutes) {
                object_delete(stored);
            } else if (stored->type == 64 && object_find_by_id(location_object, stored->building_id) != 0) {
                mc_memcpy(&current_location->buildings[stored->image], RECORD_DATA(stored), 26, (iptr)D_00176A10, 1350, 4);
            }
            stored = next_stored;
        }
        object_foreach_skip_player(room_storage_container->children, func_0007FD7E);
    }
    if (house_container != 0 && player_character->house != 0 && (((unsigned)player_character->house) >> 16) == (((unsigned)location_object->id) >> 16)) {
        object_foreach_skip_player(house_container->children, func_0007FEB9);
    } else if (ship_container != 0 && player_character->ship_owned != 0 && ((unsigned)(((unsigned)location_object->id) >> 16)) < 1000) {
        object_foreach_skip_player(ship_container->children, func_0007FEB9);
    }
    D_001A59D4 = 10;
}

void location_store_objects(void)
{
    struct record *stored_room;
    struct building *building;
    int building_index;

    if (((int)player_environment) == 3) return;
    free_later_count = 0;
    object_foreach_skip_player(location_object->children, store_repair_item_cb);
    if (room_storage_container != 0) {
        scratch_object = room_storage_container;
        object_foreach_skip_player(location_object, func_0007FCBF);
        building = current_location->buildings;
        for (building_index = 0; current_location->building_count > building_index; building_index++, building++) {
            if (building->type == 15 && ((int)(unsigned char)(building->flags & 2)) != 0 && ((unsigned)building->rent_expires) > game_minutes) {
                stored_room = object_create_child(room_storage_container, 0, 26);
                stored_room->type = 64;
                stored_room->image = building_index;
                stored_room->building_id = building->id;
                mc_memcpy(RECORD_DATA(stored_room), building, 26, (iptr)D_00176A10, 1392, 4);
            }
        }
    }
    if (player_character->house != 0 && (((unsigned)player_character->house) >> 16) == (((unsigned)location_object->id) >> 16)) {
        object_free_children(house_container);
        if ((*(iptr *)&scratch_object = (iptr)house_container) == 0) {
            scratch_object = (struct record *)((iptr)(house_container = object_create_child(player_entity, 0, 0)));
            house_container->type = 52;
            house_container->flags = 3;
            house_container->container_index = 5;
        }
        object_foreach_skip_player(object_find_by_id(location_object, player_character->house)->children, func_0007FDEA);
    } else if (player_character->ship_owned != 0 && ((unsigned)(((unsigned)location_object->id) >> 16)) < 1000) {
        object_free_children(ship_container);
        if ((*(iptr *)&scratch_object = (iptr)ship_container) == 0) {
            scratch_object = (struct record *)((iptr)(ship_container = object_create_child(player_entity, 0, 0)));
            ship_container->type = 52;
            ship_container->flags = 3;
            ship_container->container_index = 6;
        }
        object_foreach_skip_player(object_find_by_id(location_object, location_object->id)->children, func_0007FDEA);
    }
    object_free_pending();
}

void func_0007FCBF(struct record *object)
{
    struct building *building;

    if (object->type != 33 && ((int)(unsigned short)(object->flags & 4)) == 0) return;
    building = object_building(object);
    if (building->type != 15) return;
    if (((int)(unsigned char)(building->flags & 2)) == 0 || ((unsigned)game_minutes) >= building->rent_expires) {
        return;
    }
    object->home_id = building->id;
    object->repair_due = building->rent_expires;
    object_reparent(scratch_object, object);
    object->type = 58;
    object->id = object_new_id(100);
}

void func_0007FD7E(struct record *object)
{
    struct record *home;

    if (object->type != 58) return;
    if (((unsigned)game_minutes) > object->repair_due) {
        object_delete(object);
        return;
    }
    home = object_find_by_id(location_object, object->home_id);
    if (home == 0) return;
    object_reparent(home, object);
    object->type = 33;
}

void func_0007FDEA(struct record *object)
{
    struct building *building;

    if (object->type != 2 && object->type != 33) return;
    if (object->type == 33 && object->children == 0) return;
    if (object->type == 2 && object->parent->type == 33) return;
    building = object_building(object);
    object->home_id = building->id;
    object_reparent(scratch_object, object);
    if (object->type == 33) object->type = 58;
    object->id = object_new_id(100);
}

void func_0007FEB9(struct record *object)
{
    struct record *home;

    if (object->type != 2 && object->type != 58) return;
    if (object->type == 58 && object->children == 0) return;
    if (object->type == 2 && object->parent->type == 58) return;
    home = object_find_by_id(location_object, object->home_id);
    if (home != 0) {
        object_reparent(home, object);
    } else {
        object_reparent(location_object, object);
    }
    object->type = 33;
}

void store_repair_item_cb(struct record *object)
{
    if (object->type != 54) return;
    if ((game_minutes - (int)object->repair_due) > 259200) {
        object_free_single(object);
        return;
    }
    object->home_id = object->parent->id;
    object->id = object_new_id(100);
    object_reparent(repair_container, object);
}

void restore_repair_item_cb(struct record *object)
{
    struct record *home;

    if (object->type != 54) return;
    if ((game_minutes - (int)object->repair_due) > 259200) {
        object_free_single(object);
        return;
    }
    home = object_find_by_id(location_object, object->home_id);
    if (home == 0) return;
    object_reparent(home, object);
}

void arrival_room_messages(void)
{
    int in_town;
    int building_index;
    struct building *building;
    int unused;

    if (D_001A59D4 > 0) {
        D_001A59D4--;
        return;
    }
    if (D_001A59D4 < 0) return;
    D_001A59D4--;
    if (((int)player_environment) == 1 && location_contains(player_object->x, player_object->z) != 0) {
        if (current_location->kind != 4 && current_location->kind <= 9) {
            in_town = 1;
        } else {
            in_town = 0;
        }
        if (in_town != 0) goto L800EB;
    }
    goto L800F0;
L800EB:;
    holiday_announce();
L800F0:;
    building = current_location->buildings;
    for (building_index = 0; current_location->building_count > building_index; building_index++, building++) {
        if (building->type == 15) {
            tavern_building = building;
            if (tavern_room_rented() != 0) {
                mc_set_location(1570, (iptr)D_00176A10);
                mc_sprintf((iptr)text_buffer, (iptr)D_00176A42, building_name(building), (((unsigned)(building->rent_expires - game_minutes)) / 60) + 1);
                hud_message_add(text_buffer);
            }
        }
    }
}

int carry_capacity(void)
{
    int capacity;
    int slot;
    int i;
    struct item *item_data;

    capacity = player_character->attributes[0] + (player_character->attributes[0] >> 1);
    for (slot = 0; slot < 27; slot++) {
        if (player_character->equipped[slot] == 0) continue;
        item_data = &player_character->equipped[slot]->data.item;
        i = 0;
        while (i < 10 && item_data->enchantments[i].type != (-1)) {
            if (item_data->enchantments[i].type == 7) {
                if (item_data->enchantments[i].param != 0) {
                    capacity = (capacity * 384) / 256;
                } else {
                    capacity = (capacity * 320) / 256;
                }
                return capacity;
            }
            i++;
        }
    }
    return capacity;
}
