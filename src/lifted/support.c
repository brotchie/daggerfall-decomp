/* support.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_4_1 { unsigned char _:4; unsigned char f:1; };
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern signed char D_0012B508;
extern int D_001343C0;
extern short D_001343C2;
extern signed char key_down[];
extern signed char key_down_esc;
extern int screen_buffer;
extern int D_00147954;
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
extern struct record *D_00190504[];
extern int D_00190C74;
extern int D_00190C7C;
extern signed char D_001940D4;
extern signed char D_001940D5;
extern signed char D_001940D6;
extern signed char D_001940DA;
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
extern int D_00195AB0;
extern struct record *D_00195AC4;
extern struct building *tavern_building;
extern struct record *D_00195AF4;
extern char picklist_image[];
extern int D_00195B00;
extern int creature_count;
extern struct record *guild_npc_object;
extern char D_00195B84[];
extern int hud_message_expiry[];
extern int hud_status_expiry;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern int hud_message_ptrs[];
extern int D_00195C3C;
extern char D_00195C44[];
extern int D_00195CD0;
extern int D_00195CF0;
extern int D_00195D5C;
extern int free_later_count;
extern short D_00195F3C;
extern short D_00195F3E;
extern short D_00195F40;
extern short D_00195F42;
extern short D_00195F60;
extern char picklist_control[];
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
extern char D_001A4FE8[];
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
extern int info_popup_text;
extern short mode_stack_depth;
extern short D_001A5A54;

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
    hud_status_expiry = *(int *)((char *)l_18) + 36;
    D_00195C3C = (int)hud_status_text;
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
    for (l_24 = 0; l_24 < 16; l_24++, l_28 += 80) {
        if (hud_message_expiry[l_24] == 0) {
            l_20 = l_24;
            l_2C = l_28;
            break;
        }
    }
    if (l_20 < 0) return 0;
    mc_strncpy(l_2C, a1, 4, (int)D_00176A10, 117);
    *(int *)&l_18 = 1132;
    hud_message_expiry[l_20] = *(int *)(*(char **)&l_18) + 36;
    hud_message_ptrs[l_20] = l_2C;
    return hud_message_ptrs[l_24];
}

void hud_messages_draw(void)
{
    int l_24;
    int l_20;

    func_0012DB50(4);
    l_24 = 0;
    for (l_20 = 2; l_24 < 16; l_24++, l_20 += 9) {
        if (hud_message_expiry[l_24] != 0) {
            if (((struct bf8_0_1 *)&D_001940DA)->f == 0) {
                if (((unsigned)*(int *)((char *)1132)) > hud_message_expiry[l_24]) {
                    hud_message_expiry[l_24] = 0;
                }
            }
            text_draw_centered_colored(hud_message_ptrs[l_24], 160, (int)(short)*(short *)&l_20, 145, 156);
        }
    }
    if (hud_status_expiry == 0) return;
    if (((struct bf8_0_1 *)&D_001940DA)->f == 0) {
        if (((unsigned)*(int *)((char *)1132)) > hud_status_expiry) {
            hud_status_expiry = 0;
        }
    }
    text_draw_centered_colored(D_00195C3C, 160, (int)(short)*(short *)&l_20, 145, 156);
}

void info_popup_open(int a1)
{
    D_00196275 = D_00196272;
    D_00196272 = 2;
    info_popup_text = a1;
}

int wait_key_from_list(int a1, short a2)
{
    unsigned char l_14;
    short l_18;

    while (mouse_buttons != 0) func_0012B136();
    while (mouse_buttons == 0) {
        func_0012B136();
        func_0012B2D3((int)(short)mouse_x, (int)(short)mouse_y);
        l_14 = func_001427A8();
        if (l_14 != 0) {
            if (((struct bf8_0_1 *)&D_001940D4)->f != 0 && ((int)(unsigned char)l_14) == 27) {
                return -1;
            }
            if (a2 != 0) {
                l_14 = toupper((int)(unsigned char)l_14);
                *(int *)&l_18 = 0;
                for (; (short)(short)*(int *)&l_18 < a2; (*(int *)&l_18)++) {
                    if ((signed char)l_14 == *(signed char *)((char *)(((int)(short)l_18) + a1))) {
                        return (int)(short)l_18;
                    }
                }
            }
        }
        mc_memcpy(655360, screen_buffer, 64000, (int)D_00176A10, 234, 4);
    }
    return -2;
}

int key_pressed_once(unsigned char a1)
{
    if (key_down[(int)(unsigned char)a1] != 0 && key_was_down[(int)(unsigned char)a1] == 0) {
        key_was_down[(int)(unsigned char)a1] = 1;
        return 1;
    }
    if (key_down[(int)(unsigned char)a1] == 0) {
        key_was_down[(int)(unsigned char)a1] = 0;
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

void func_0007D19B(int a1, int a2, short a3, short a4)
{
    short l_10;
    short l_C;

    if (*(short *)((char *)a1) <= a3 && *(short *)((char *)a2) <= a4) return;
    *(int *)&l_10 = (((int)(short)a3) << 8) / ((int)(short)*(short *)((char *)a1));
    *(int *)&l_C = (((int)(short)a4) << 8) / ((int)(short)*(short *)((char *)a2));
    if ((short)(short)*(int *)&l_C < l_10) {
        *(short *)((char *)a1) = (((int)(short)*(short *)((char *)a1)) * ((int)(short)l_C)) >> 8;
        *(short *)((char *)a2) = *(int *)&a4;
        return;
    }
    *(short *)((char *)a2) = (((int)(short)*(short *)((char *)a2)) * ((int)(short)l_10)) >> 8;
    *(short *)((char *)a1) = *(int *)&a3;
}

void picklist_open_strings(int a1)
{
    if (*(int *)picklist_image == 0) *(int *)picklist_image = disk_read_file((int)D_00176A1A, 0);
    D_00195F40 = 160 - (((int)(unsigned short)*(short *)(*(char **)picklist_image + 4)) >> 1);
    D_00195F3E = 100 - (((int)(unsigned short)*(short *)(*(char **)picklist_image + 6)) >> 1);
    D_00195F42 = *(short *)(*(char **)picklist_image + 4);
    D_00195F3C = *(short *)(*(char **)picklist_image + 6);
    picklist_init((int)picklist_control, (int)(short)(D_00195F40 + 25), (int)(short)(D_00195F3E + 26), 140, 73, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 11), 8, 9, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 109), 8, 9, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 22), 9, 83, 146, 146, 244, 114, 0);
    while (*(signed char *)((char *)a1) != 0) {
        picklist_add((int)picklist_control, a1, 0);
        a1 += func_000A0DF4(a1) + 1;
    }
    D_001940D4 |= 4;
}

void picklist_open(int a1)
{
    if (*(int *)picklist_image == 0) *(int *)picklist_image = disk_read_file((int)D_00176A1A, 0);
    D_00195F40 = 160 - (((int)(unsigned short)*(short *)(*(char **)picklist_image + 4)) >> 1);
    D_00195F3E = 100 - (((int)(unsigned short)*(short *)(*(char **)picklist_image + 6)) >> 1);
    D_00195F42 = *(short *)(*(char **)picklist_image + 4);
    D_00195F3C = *(short *)(*(char **)picklist_image + 6);
    picklist_init((int)picklist_control, (int)(short)(D_00195F40 + 25), (int)(short)(D_00195F3E + 26), 140, 73, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 11), 8, 9, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 109), 8, 9, (int)(short)(D_00195F40 + 179), (int)(short)(D_00195F3E + 22), 9, 83, 146, 146, 244, 114, 0);
    while (*(int *)((char *)a1) != 0) {
        picklist_add((int)picklist_control, *(int *)((char *)(int)(*(char (**)[4])&a1)++), 0);
    }
    D_001940D4 |= 4;
}

int picklist_frame(int a1)
{
    short l_18;

    if (((struct bf8_0_1 *)&D_001940D4)->f != 0 && (key_down_esc != 0 || ((int)(unsigned char)(mouse_buttons & 2)) != 0)) {
        D_001940D4 &= 251;
        picklist_free(a1);
        return -2;
    }
    *(int *)&l_18 = picklist_poll(a1) - 1;
    if (((int)(short)l_18) > (-1)) {
        D_001940D4 &= 251;
        picklist_free(a1);
        return (int)(short)l_18;
    }
    picklist_draw(a1, 0);
    return -1;
}

int picklist_update(void)
{
    func_00144F68((int)(short)D_00195F40, (int)(short)D_00195F3E, (int)(short)D_00195F42, (int)(short)D_00195F3C, *(int *)picklist_image + 12);
    return picklist_frame((int)picklist_control);
}

int rand_range(int a1, int a2)
{
    return a1 + (rand() % ((a2 - a1) + 1));
}

void object_free_later(struct record *a1)
{
    if (a1 == 0) return;
    *(int *)(free_later_list + (free_later_count++ << 2)) = (int)a1;
}

void object_free_pending(void)
{
    int l_18;

    for (l_18 = 0; l_18 < free_later_count; l_18++) {
        object_delete((struct record *)*(int *)(free_later_list + (l_18 << 2)));
    }
    free_later_count = 0;
}

void func_0007D774(int a1, int a2)
{
    *(int *)(D_001A4FE8 + (D_00195D5C << 3)) = a1;
    *(int *)(D_001A4FEC + (D_00195D5C++ << 3)) = a2;
}

void spell_cast_queued_run(void)
{
    int l_18;

    for (l_18 = 0; l_18 < D_00195D5C; l_18++) {
        cast_spell_on(*(int *)(D_001A4FE8 + (l_18 << 3)), *(int *)(D_001A4FEC + (l_18 << 3)), 0);
    }
    D_00195D5C = 0;
}

void world_collect_object(struct record *a1)
{
    struct item *l_1C;
    int l_18;

    switch (a1->type) {
    case 33:
        if (((struct bf8_4_1 *)&D_001940D6)->f != 0 && player_character->detect_kind == 2) {
            if ((a1->image >> 7) == 216 || a1->image == 26112) detect_consider(a1);
        }
        if ((a1->image >> 7) == 210 && memchr((int)fire_flat_records, (int)(unsigned short)(a1->image & 127), 8) != 0) {
            l_18 = func_000C7FF4(player_object->y - a1->y, func_000C7FD9(player_object->x, player_object->z, a1->x, a1->z));
            if (l_18 < nearest_fire_distance) {
                nearest_fire_distance = l_18;
                nearest_fire = a1;
            }
        }
        return;
    case 18:
        D_00190504[creature_count++] = a1;
        if (func_0002586B(a1) != 0) {
            creature_count--;
        } else if (((struct bf8_4_1 *)&D_001940D6)->f != 0 && player_character->detect_kind == 1) {
            detect_consider_creature(a1, a1->detect_distance);
        }
        return;
    case 2:
        l_1C = &a1->data.item;
        if (((struct bf8_4_1 *)&D_001940D6)->f != 0 && player_character->detect_kind == 0 && l_1C->enchantments[0].type != (-1) && a1->parent->parent != player_entity) {
            detect_consider(a1);
        }
        return;
    case 9:
        if (((int)(unsigned short)(a1->flags & 8192)) != 0) {
            if (spell_missile_update(a1, 0) != 0) object_free_later(a1);
        }
        return;
    case 34:
        switch (((int)(unsigned short)(a1->image & 31)) - 2) {
        case 13:
        case 14:
            if (a1->spawn_seed == 0) a1->spawn_seed = rand();
            place_spawn_from_marker(a1);
            break;
        case 18:
            if (((int)player_environment) == 3) {
                func_0008591A(a1, 0);
            } else if (((int)player_environment) == 2 && current_building->type >= 17 && current_building->type <= 20) {
                func_0008591A(a1, (int)current_building);
            }
            break;
        case 17:
            func_00085992(a1, ((((int)player_environment) == 2) ? (int)object_building(player_object->parent) : 0));
        }
        return;
    case 53:
        people_list[people_count++] = a1;
    default:;
    }
}

void detect_consider(struct record *a1)
{
    int l_18;

    l_18 = func_000C7FD9(player_object->x, player_object->z, a1->x, a1->z);
    if (l_18 >= *(int *)D_00195B84 || l_18 >= 2048) return;
    detect_target = a1;
    *(int *)D_00195B84 = l_18;
}

void world_collect_objects(void)
{
    struct record *l_1C;
    struct record *l_18;

    nearest_fire = 0;
    nearest_fire_distance = 2048;
    if (((int)player_environment) == 2) {
        current_building = object_building(player_object);
    }
    D_00195D5C = (free_later_count = 0);
    *(int *)D_00195B84 = 100000;
    people_count = (creature_count = 0);
    detect_target = 0;
    D_00195CD0 = (int)object_foreach_pre;
    if (((int)player_environment) < 3) {
        if (player_object->parent->type != 1) {
            object_foreach_pre(player_object->parent->children, (int)world_collect_object);
        } else {
            D_00195CD0 = (int)object_foreach_open;
            func_0007EB0B(player_object, (int)world_collect_object);
        }
        l_1C = D_00195AC4->children;
        while (l_1C != 0) {
            l_18 = l_1C->next;
            if (l_1C->type != 38) {
                if (l_1C->children != 0) {
                    object_foreach_open(l_1C->children, (int)world_collect_object);
                }
                world_collect_object(l_1C);
            }
            l_1C = l_18;
        }
    } else {
        func_0007E815(player_object, (int)world_collect_object);
        l_1C = D_00195AC4->children;
        while (l_1C != 0) {
            l_18 = l_1C->next;
            if (l_1C->type != 47) {
                object_foreach_open(l_1C->children, (int)world_collect_object);
                world_collect_object(l_1C);
            }
            l_1C = l_18;
        }
    }
    D_00195CD0 = (int)object_foreach_open;
    spell_cast_queued_run();
    object_free_pending();
}

void msgbox_yes_no_quest(short a1)
{
    D_00196271 = 0;
    msgbox_button_ids = 4;
    D_00196090 = 5;
    D_00196091 = 0;
    msgbox_button_keys = 21;
    D_00196034 = 49;
    D_0012B508 = 146;
    msgbox_show_quest_text(current_quest, (int)(short)a1, 5);
}

void msgbox_prompt_number(int a1, int a2)
{
    int l_14;

    D_0012B508 = 146;
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

    D_0012B508 = 146;
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
    if (a1 == 0 || (int)a1 == (-1768515946)) return 0;
    while (a1 != 0 && a1->type != 1) {
        if (a1->type == 43 && ((int)(unsigned short)(a1->flags & 1)) != 0) break;
        a1 = a1->parent;
    }
    if (a1 == 0 || a1->type == 1 || a1->image == 65535) return 0;
    return &current_location->buildings[a1->image];
}

int func_0007E00E(struct record *a1, struct record *a2, int a3)
{
    return ((func_000C7FD9(a1->x, a1->z, a2->x, a2->z) < a3) ? 1 : 0);
}

void func_0007E066(void)
{
    func_0007F671();
    D_001A59D0 = D_00195B00;
    D_001A59C8 = D_00195B00;
}

void frame_ticks_update(void)
{
    if ((D_00195AB0 = (D_001A3F40 * 1828) / 256) > 100) D_00195AB0 = 100;
    D_001343C0 += D_00195AB0;
    D_001343C2 &= 7;
    D_001A3F40 = 0;
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
    D_001940D5 |= 2;
}

int func_0007E1A7(struct record *a1)
{
    int l_1C;

    l_1C = func_00135DE4(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
    if (l_1C == 0) {
        func_00135E39();
        l_1C = func_00135DE4(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
    }
    return ((a1->anim_frame >= *(unsigned short *)((char *)l_1C + 20)) ? 1 : 0);
}

void func_0007E246(struct record *a1)
{
    int l_1C;
    int l_18;

    l_1C = func_00135DE4(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
    if (l_1C == 0) {
        func_00135E39();
        l_1C = func_00135DE4(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
    }
    l_18 = ((D_001343C0 >> 5) - a1->anim_time) << 5;
    if (l_18 < 0 || l_18 > 2000) l_18 = (int)(unsigned short)*(short *)((char *)l_1C + 22);
    if (((int)(unsigned short)*(short *)((char *)l_1C + 22)) > l_18) return;
    a1->anim_time = D_001343C0 >> 5;
    a1->anim_frame++;
}

void func_0007E31C(struct record *a1)
{
    a1->anim_time = D_001343C0 >> 5;
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
    for (l_18 = l_20; (l_20 + 3) > l_18; l_18++) {
        for (l_14 = l_1C; (l_1C + 3) > l_14; l_14++) {
            if (l_18 < 0 || current_location->width <= l_18) continue;
            if (l_14 < 0 || current_location->height <= l_14) continue;
            if (*(int *)(D_001940E4 + (((l_14 << 5) + l_18) << 2)) != 0) {
                ((int (*)())(D_00195CD0))(*(int *)(*(char **)(D_001940E4 + (((l_14 << 5) + l_18) << 2)) + 63), a2);
            }
        }
    }
}

int func_0007EC28(struct record *a1, struct record *a2)
{
    int l_18;

    l_18 = func_000C7FD9(a1->x, a1->z, a2->x, a2->z);
    if (l_18 > 512) return 0;
    return (l_18 << 7) / 512;
}

void func_0007EC8F(int a1, int a2, int a3, int a4, int a5)
{
    int l_C;

    for (l_C = 0; l_C < a3; l_C++) {
        if (mouse_x > *(short *)((char *)((l_C * 12) + a4)) && mouse_x < *(short *)((char *)((l_C * 12) + a4) + 4) && mouse_y > *(short *)((char *)((l_C * 12) + a4) + 2) && mouse_y < *(short *)((char *)((l_C * 12) + a4) + 6)) {
            text_draw_colored(*(int *)((char *)((l_C << 2) + a5)), (int)(short)*(short *)&a1, (int)(short)*(short *)&a2, 145, 156);
        }
    }
}

int func_0007ED48(int a1)
{
    return a1;
}

int detect_arrow_frame(void)
{
    if (detect_target != 0) {
        return func_000C808D(player_object->x, player_object->z, detect_target->x, detect_target->z) >> 6;
    }
    return rand() % 32;
}

int func_0007EDD3(void)
{
    int l_20;
    int l_1C;

    l_20 = 1132;
    if (((unsigned)(*(int *)((char *)l_20) - D_00190C7C)) > 38) {
        D_00190C74 = (D_00190C74 + 1) % 32;
        l_1C = 1132;
        D_00190C7C = *(int *)((char *)l_1C);
    }
    return D_00190C74;
}

void func_0007EE38(void)
{
    int l_20;
    int l_1C;
    int l_18;

    l_18 = 24000;
    for (l_1C = 0; l_1C < 200; l_1C += 4) {
        for (l_20 = 0; l_20 < 320; l_20 += 4) {
            *(signed char *)((char *)(int)(*(char **)&D_00147954 + l_18++)) = *(signed char *)((char *)(int)(*(char **)&screen_buffer + ((l_1C * 320) + l_20)));
        }
    }
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

    D_00196281 = 0;
    return 0;
}

void func_0007EF20(void)
{
    char l_3C[28];
    char l_20[12];

    if (D_00196281 == 0) return;
    mc_memset((int)l_20, 0, 12, (int)D_00176A10, 1064, 4);
    *(short *)l_3C = 257;
    *(short *)((char *)l_3C + 12) = D_001A5A54;
    int386x(49, (int)l_3C, (int)l_3C, (int)l_20);
    D_00196281 = 0;
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
    if ((object_weight(player_entity) + (a1 / 100)) > (carry_capacity() << 2)) {
        gold_make_credit_letter(a1);
        return;
    }
    player_character->gold += a1;
}

void gold_spend(int a1)
{
    struct record *l_18;

    if (((unsigned)player_character->gold) >= a1) {
        player_character->gold -= a1;
        return;
    }
    D_00195AF4 = 0;
    l_18 = gold_find_credit(a1);
    if (l_18 != 0) {
        l_18->data.item.value -= a1;
        return;
    }
    free_later_count = 0;
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
    if (l_18->group != 27 || l_18->index != 2) return;
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

    if (a1->type != 2) return 0;
    l_1C = &a1->data.item;
    if (l_1C->group == 27 && l_1C->index == 2 && ((unsigned)l_1C->value) >= *(int *)D_00195B84) {
        D_00195AF4 = a1;
        return 1;
    }
    return 0;
}

int gold_spend_credit_cb(struct record *a1)
{
    struct item *l_1C;

    if (a1->type != 2) return 0;
    if (*(int *)D_00195B84 == 0) return 0;
    l_1C = &a1->data.item;
    if (l_1C->group == 27 && l_1C->index == 2) {
        if (((unsigned)l_1C->value) <= *(int *)D_00195B84) {
            object_free_later(a1);
            *(int *)D_00195B84 -= l_1C->value;
        } else {
            l_1C->value -= *(int *)D_00195B84;
            *(int *)D_00195B84 = 0;
        }
    }
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
    if (((int)(unsigned short)*(short *)(*(char **)&l_18 + 32)) != 27 || ((int)(unsigned short)*(short *)(*(char **)&l_18 + 34)) != 2) {
        return;
    }
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

    D_001A59CC = func_000CE8B2();
    l_18 = 1132;
    D_001A59DC = *(int *)((char *)l_18);
}

void func_0007F6A4(void)
{
    int l_20;
    int l_1C;
    int l_18;

    D_001A59E0 = D_001A59CC;
    D_001A59E4 = D_001A59DC;
    D_001A59CC = func_000CE8B2();
    l_18 = 1132;
    if ((D_001A59DC = *(int *)((char *)l_18)) == D_001A59E4) {
        l_20 = D_001A59E0 - D_001A59CC;
    } else {
        l_20 = ((65535 - D_001A59CC) + D_001A59E0) + (((D_001A59DC - D_001A59E4) - 1) * 65535);
    }
    if (((unsigned)l_20) < 10000) l_20 = (int)(((char *)D_001A59E0) + (65535 - D_001A59CC));
    D_001A5408[D_001A59D8] = ((unsigned)(((unsigned)(l_20 * 1000)) / 1193180)) >> 1;
    if (((unsigned)D_001A5408[D_001A59D8]) > 100) {
        D_001A5408[D_001A59D8] = 100;
    }
    D_001A59D8 = (D_001A59D8 + 1) & 7;
    for (l_1C = 0; l_1C < 8; l_1C++) {
        D_00195AB0 += D_001A5408[l_1C];
    }
    D_00195AB0 >>= 3;
    if (D_00195AB0 <= 500) return;
    D_00195AB0 = 500;
}

void func_0007F7EA(struct record *a1)
{
    if (D_00196282 != 0 && (signed char)a1->quest_id != D_00196282) {
        return;
    }
    if (D_00195CF0 != 0 && (int)a1->id != D_00195CF0) return;
    if (D_00195F60 != 0 && (short)a1->image2 != D_00195F60) return;
    if (D_00196284 != 0 && (signed char)a1->type != D_00196284) {
        return;
    }
    D_00195AF4 = a1;
}

void location_restore_stored(void)
{
    struct record *l_1C;
    struct record *l_18;

    if (((int)player_environment) == 3) return;
    func_0008E447(D_001959F8->children, (int)func_0007FFE7);
    if (D_001959F4 != 0 && D_001959F4->children != 0) {
        l_1C = D_001959F4->children;
        while (l_1C != 0) {
            l_18 = l_1C->next;
            if (l_1C->type == 64 && ((unsigned)l_1C->building_id) < game_minutes) {
                object_delete(l_1C);
            } else if (l_1C->type == 64 && object_find_by_id(D_00195AC4, l_1C->building_id) != 0) {
                mc_memcpy(&current_location->buildings[l_1C->image], RECORD_DATA(l_1C), 26, (int)D_00176A10, 1350, 4);
            }
            l_1C = l_18;
        }
        func_0008E447(D_001959F4->children, (int)func_0007FD7E);
    }
    if (D_001959EC != 0 && player_character->house != 0 && (((unsigned)player_character->house) >> 16) == (((unsigned)D_00195AC4->id) >> 16)) {
        func_0008E447(D_001959EC->children, (int)func_0007FEB9);
    } else if (D_001959F0 != 0 && player_character->ship_owned != 0 && ((unsigned)(((unsigned)D_00195AC4->id) >> 16)) < 1000) {
        func_0008E447(D_001959F0->children, (int)func_0007FEB9);
    }
    D_001A59D4 = 10;
}

void location_store_objects(void)
{
    struct record *l_20;
    struct building *l_1C;
    int l_18;

    if (((int)player_environment) == 3) return;
    free_later_count = 0;
    func_0008E447(D_00195AC4->children, (int)func_0007FF73);
    if (D_001959F4 != 0) {
        guild_npc_object = D_001959F4;
        func_0008E447(D_00195AC4, (int)func_0007FCBF);
        l_1C = current_location->buildings;
        for (l_18 = 0; current_location->building_count > l_18; l_18++, l_1C++) {
            if (l_1C->type == 15 && ((int)(unsigned char)(l_1C->flags & 2)) != 0 && ((unsigned)l_1C->rent_expires) > game_minutes) {
                l_20 = object_create_child(D_001959F4, 0, 26);
                l_20->type = 64;
                l_20->image = l_18;
                l_20->building_id = l_1C->id;
                mc_memcpy(RECORD_DATA(l_20), l_1C, 26, (int)D_00176A10, 1392, 4);
            }
        }
    }
    if (player_character->house != 0 && (((unsigned)player_character->house) >> 16) == (((unsigned)D_00195AC4->id) >> 16)) {
        object_free_children(D_001959EC);
        if ((*(int *)&guild_npc_object = (int)D_001959EC) == 0) {
            guild_npc_object = (struct record *)((int)(D_001959EC = object_create_child(player_entity, 0, 0)));
            D_001959EC->type = 52;
            D_001959EC->flags = 3;
            D_001959EC->image = 5;
        }
        func_0008E447(object_find_by_id(D_00195AC4, player_character->house)->children, (int)func_0007FDEA);
    } else if (player_character->ship_owned != 0 && ((unsigned)(((unsigned)D_00195AC4->id) >> 16)) < 1000) {
        object_free_children(D_001959F0);
        if ((*(int *)&guild_npc_object = (int)D_001959F0) == 0) {
            guild_npc_object = (struct record *)((int)(D_001959F0 = object_create_child(player_entity, 0, 0)));
            D_001959F0->type = 52;
            D_001959F0->flags = 3;
            D_001959F0->image = 6;
        }
        func_0008E447(object_find_by_id(D_00195AC4, D_00195AC4->id)->children, (int)func_0007FDEA);
    }
    object_free_pending();
}

void func_0007FCBF(struct record *a1)
{
    struct building *l_18;

    if (a1->type != 33 && ((int)(unsigned short)(a1->flags & 4)) == 0) return;
    l_18 = object_building(a1);
    if (l_18->type != 15) return;
    if (((int)(unsigned char)(l_18->flags & 2)) == 0 || ((unsigned)game_minutes) >= l_18->rent_expires) {
        return;
    }
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
    if (((unsigned)game_minutes) > a1->repair_due) {
        object_delete(a1);
        return;
    }
    l_18 = object_find_by_id(D_00195AC4, a1->home_id);
    if (l_18 == 0) return;
    object_reparent(l_18, a1);
    a1->type = 33;
}

void func_0007FDEA(struct record *a1)
{
    struct building *l_18;

    if (a1->type != 2 && a1->type != 33) return;
    if (a1->type == 33 && a1->children == 0) return;
    if (a1->type == 2 && a1->parent->type == 33) return;
    l_18 = object_building(a1);
    a1->home_id = l_18->id;
    object_reparent(guild_npc_object, a1);
    if (a1->type == 33) a1->type = 58;
    a1->id = object_new_id(100);
}

void func_0007FEB9(struct record *a1)
{
    struct record *l_18;

    if (a1->type != 2 && a1->type != 58) return;
    if (a1->type == 58 && a1->children == 0) return;
    if (a1->type == 2 && a1->parent->type == 58) return;
    l_18 = object_find_by_id(D_00195AC4, a1->home_id);
    if (l_18 != 0) {
        object_reparent(l_18, a1);
    } else {
        object_reparent(D_00195AC4, a1);
    }
    a1->type = 33;
}

void func_0007FF73(struct record *a1)
{
    if (a1->type != 54) return;
    if ((game_minutes - (int)a1->repair_due) > 259200) {
        object_free_single(a1);
        return;
    }
    a1->home_id = a1->parent->id;
    a1->id = object_new_id(100);
    object_reparent(D_001959F8, a1);
}

void func_0007FFE7(struct record *a1)
{
    struct record *l_18;

    if (a1->type != 54) return;
    if ((game_minutes - (int)a1->repair_due) > 259200) {
        object_free_single(a1);
        return;
    }
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

    if (D_001A59D4 > 0) {
        D_001A59D4--;
        return;
    }
    if (D_001A59D4 < 0) return;
    D_001A59D4--;
    if (((int)player_environment) == 1 && location_contains(player_object->x, player_object->z) != 0) {
        if (current_location->kind != 4 && current_location->kind <= 9) {
            l_24 = 1;
        } else {
            l_24 = 0;
        }
        if (l_24 != 0) goto L800EB;
    }
    goto L800F0;
L800EB:;
    holiday_announce();
L800F0:;
    l_1C = current_location->buildings;
    for (l_20 = 0; current_location->building_count > l_20; l_20++, l_1C++) {
        if (l_1C->type == 15) {
            tavern_building = l_1C;
            if (tavern_room_rented() != 0) {
                func_000A0ED9(1570, (int)D_00176A10);
                mc_sprintf((int)text_buffer, (int)D_00176A42, building_name(l_1C), (((unsigned)(l_1C->rent_expires - game_minutes)) / 60) + 1);
                hud_message_add((int)text_buffer);
            }
        }
    }
}

int carry_capacity(void)
{
    int l_28;
    int l_24;
    int l_20;
    struct item *l_1C;

    l_28 = player_character->attributes[0] + (player_character->attributes[0] >> 1);
    for (l_24 = 0; l_24 < 27; l_24++) {
        if (player_character->equipped[l_24] == 0) continue;
        l_1C = &player_character->equipped[l_24]->data.item;
        l_20 = 0;
        while (l_20 < 10 && l_1C->enchantments[l_20].type != (-1)) {
            if (l_1C->enchantments[l_20].type == 7) {
                if (l_1C->enchantments[l_20].param != 0) {
                    l_28 = (l_28 * 384) / 256;
                } else {
                    l_28 = (l_28 * 320) / 256;
                }
                return l_28;
            }
            l_20++;
        }
    }
    return l_28;
}
