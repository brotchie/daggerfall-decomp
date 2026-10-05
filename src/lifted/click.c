/* click.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
struct bf8_6_1 { unsigned char _:6; unsigned char f:1; };
extern int xn_cam_far_z;
extern int pick_distance;
extern signed char mouse_buttons;
extern short mouse_x;
extern short mouse_y;
extern int dungeon_water_level;
extern signed char key_down_esc;
extern char D_00176198[];
extern char D_001761A0[];
extern char D_001761C4[];
extern char D_001761E8[];
extern char D_0017620C[];
extern char D_0017622D[];
extern char D_00176255[];
extern char D_0017628F[];
extern char D_00176297[];
extern char D_001762DD[];
extern char D_001762EA[];
extern char D_001762F7[];
extern char D_00176322[];
extern unsigned char player_environment;
extern signed char building_open_hours[];
extern signed char D_0017C5B9[];
extern int D_0017CA14;
extern char monster_names[];
extern int D_00183248;
extern int D_0018324C;
extern int D_00183250;
extern int D_00183254;
extern int D_00184329;
extern int D_001845CC;
extern int D_00185093;
extern char repair_menu_buttons[];
extern char D_001875C2[];
extern char D_001875C4[];
extern char D_001875C6[];
extern char D_001875C8[];
extern char coven_menu_buttons[];
extern char D_001875F2[];
extern char D_001875F4[];
extern char D_001875F6[];
extern char D_001875F8[];
extern char service_menu_buttons[];
extern char D_00187622[];
extern char D_00187624[];
extern char D_00187626[];
extern char D_00187628[];
extern char D_00187644[];
extern signed char footstep_sound_ids[];
extern short music_special_dungeon_ids[];
extern int D_001878AC[];
extern int D_001878D4[];
extern signed char region_event_flags[];
extern signed char text_buffer[];
extern char shelf_book_ids[];
extern char scratch_190be4[];
extern signed char D_001940D4;
extern signed char D_001940D5;
extern signed char D_001940D6;
extern unsigned char D_001940D7;
extern signed char D_001940D9;
extern struct record *wagon_container;
extern struct record *quest_root;
extern struct character *text_macro_npc;
extern struct building *current_building;
extern struct record *player_object;
extern char D_00195AB4[];
extern struct record *location_object;
extern int inventory_close_callback;
extern char click_hit[];
extern int shelf_list_callback;
extern struct record *shelf_object;
extern struct record *spell_ready_touch;
extern struct record *scratch_object;
extern struct location *current_location;
extern struct character *player_character;
extern int window_image;
extern char scratch_buffer[];
extern struct record *D_00195CE8;
extern char D_00195D3C[];
extern char picked_model_index[];
extern char click_face_texture[];
extern signed char climate_weathers[];
extern short shelf_model_index;
extern short D_00195F68;
extern char D_00196092[];
extern char D_001960D9[];
extern int D_00196118;
extern char D_00196120[];
extern char D_001961F5[];
extern signed char in_knightly_order_hall;
extern signed char current_region;
extern unsigned char D_00196271;
extern signed char D_00196272;
extern signed char game_mode;
extern unsigned char interaction_mode;
extern signed char player_on_ground;
extern signed char mouse_buttons_prev;
extern signed char player_underwater;
extern signed char in_dungeon_water;
extern signed char crime_current;
extern signed char D_0019629D;
extern signed char D_001962A0;
extern signed char D_001962A1;
extern signed char D_001962B1;
extern int daylight;
extern signed char music_uses_fm;
extern char stocked_shop_ids[];
extern int service_menu_label;
extern struct record *service_menu_npc;
extern struct record *coven_menu_npc;
extern int stocked_shop_count;
extern struct record *repair_menu_npc;
extern int service_menu_handler;
extern signed char shelf_list_active;
extern signed char repair_menu_kind;
extern char position_history[];
extern char D_001A4CA4[];
extern char D_001A4CA8[];
extern char ground_position_history[];
extern char D_001A4E24[];
extern char D_001A4E28[];
extern int position_history_next;
extern int D_001A4FBC;
extern int D_001A4FC4;
extern int ground_position_history_next;
extern int D_001A4FCC;
extern int D_001A4FD4;

extern int lockpick_action_door(struct building *, int, struct record *);
extern int furniture_is_container(int);
extern struct faction *faction_find_type_in_region(int, short);
extern struct faction *faction_find(short);
extern int tavern_open(int);
extern int climate_category(void);
extern int collide_line_of_sight(struct record *, struct record *);
extern int quest_event_clicked_faction(unsigned short);
extern int func_00031843(int, struct record *, struct record *);
extern int list_popup_poll(void);
extern int pedestrian_spawn_spot_ok(struct record *, int, int);
extern struct record *item_add_to_container(struct record *, int, int, int);
extern int building_is_open(struct building *);
extern int quest_raise_event();
extern int quest_pick_file();
extern int func_0004CD80(int);
extern int npc_talk_record_build(struct record *);
extern int func_000612A1(void);
extern int ai_angle_diff(int, int, int);
extern int sound_play(int, struct record *, int);
extern int sound_play_at_point(int, int, int, int, int);
extern int bank_open(int);
extern int disk_read_file(int, int);
extern int guild_find_membership_by_kind(unsigned char);
extern int guild_find_membership_by_bits(unsigned char);
extern int spawn_point_fits(struct record *);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int location_contains(int, int);
extern int npc_display_name(struct record *);
extern int building_name(struct building *);
extern int object_delete(int);
extern int object_detach(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern struct record *object_find_type(struct record *, int);
extern int object_new_id(int);
extern int inventory_open(int, int, int);
extern struct record *marker_find_nth(struct record *, int, int);
extern int marker_count(struct record *, int);
extern struct record *location_cell_at(int, int);
extern int rand();
extern int abs();
extern int mc_free();
extern int mc_strncpy();
extern int strlen();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int strchr();
extern int xn_math_approx_dist2d();
extern int xn_math_approx_hypot();
extern int xn_math_angle_to_point();
extern int xn_str_find_u16();
extern int xn_math_yaw_offset_xz();
extern int xn_mouse_poll_clamped();
extern int xn_draw_image();
extern int xn_terrain_height_at();
extern void pickpocket_attempt(struct record *);
extern void talk_start(struct record *);
extern void rumor_show_local(void);
extern void daedra_summon(int);
extern void town_map_note_building(struct record *, struct building *);
extern void automap_save(void);
extern void automap_load(void);
extern void msgbox_show_string(int, int);
extern void guards_summon(int);
extern void quest_pick_for_npc(struct record *);
extern void book_open(short);
extern void cast_spell_on(int, struct record *, int);
extern void item_make(int, int, struct item *);
extern void shelf_stock_items(struct record *, int, int);
extern void loot_generate(int, struct record *, int, unsigned short);
extern void loot_fill_container(struct record *);
extern void guild_service_dispatch(struct record *);
extern void door_try_open(struct record *, int);
extern void book_read_header(int, unsigned short);
extern void pick_up_item(struct record *);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
extern void hud_status_set(int);
extern void list_popup_open_strings(int);
extern void msgbox_yes_no_rsc(int);
extern void dungeon_load(int);
extern void location_unload(unsigned short);
extern void building_enter(struct building *);
extern void building_exit(void);
extern void object_free_children(struct record *);
extern void object_foreach(struct record *, int);
extern void inventory_open_container(struct record *, int, int);
extern void ladder_climb(void);
int shelf_collect_items(struct record *, int, int);
int object_count_items(struct record *);
int repair_menu_open(int);
int coven_menu_open(int);
int quest_active_for_faction(short);
int noble_quest_letter(void);
int service_menu_open(int);
int shelf_shop_stocked(int);
int spawn_point_visible(struct record *);
int spawn_point_occupied(struct record *);
int spawn_point_dungeon_level(struct record *, int, int, int);
void click_describe_item(struct item *);
void click_describe_creature(struct character *, struct career *);
void shelf_open(struct record *, struct building *, int);
void shelf_book_chosen(int);
void shelf_return_items(void);
void shelf_open_books(struct record *, struct building *, int);
void shelf_open_stock(struct record *, struct building *, int);
void npc_click_service(struct record *);
void shop_open_repair(int, struct record *);
void npc_talk(struct record *);
void count_items_cb(struct record *);
void repair_menu_close(void);
void repair_menu_sell(void);
void coven_menu_close(void);
void click_show_building_info(int, struct record *);
void service_menu_close(void);
void service_menu_sell(void);
void container_items_to_player(struct record *);
void position_history_record(void);
void spawn_point_occupied_cb(struct record *);
#pragma aux mc_set_location parm routine [];

void click_describe_item(struct item *a1)
{
    if (strchr(D_00183248, (int)(unsigned char)a1->name[0]) != 0) {
        mc_set_location(204, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_0018324C, a1);
    } else {
        mc_set_location(206, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_00183250, a1);
    }
    hud_message_add((int)text_buffer);
}

void click_describe_creature(struct character *a1, struct career *a2)
{
    if (strchr(D_00183248, (int)(unsigned char)a2->name[0]) != 0) {
        mc_set_location(215, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_0018324C, a2->name);
    } else {
        mc_set_location(217, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_00183250, a2->name);
    }
    hud_message_add((int)text_buffer);
}

int click_world_face(int a1)
{
    struct building *l_24;
    int l_20;
    int l_1C;

    l_24 = object_building((struct record *)*(int *)((char *)a1 + 4));
    if (((int)(unsigned char)*(signed char *)(*(char **)((char *)a1 + 4))) != 6 && ((int)(unsigned char)*(signed char *)(*(char **)((char *)a1 + 4))) != 43 && ((int)(unsigned char)*(signed char *)(*(char **)((char *)a1 + 4))) != 56) {
        return 0;
    }
    l_20 = ((int)(unsigned short)*(short *)(*(char **)click_face_texture + 2)) >> 7;
    l_1C = (int)(unsigned short)(*(short *)(*(char **)click_face_texture + 2) & 127);
    if ((l_20 % 100) == 74) l_20 = 74;
    if (l_24 != 0) {
        {
            unsigned char l_2C;
            switch ((unsigned)l_20) {
            case 56:
            case 331:
                if (((int)(unsigned short)(player_character->flags & 1536)) != 0) {
                    hud_message_add((int)D_001761A0);
                    break;
                }
                dungeon_load(-1);
                automap_load();
                break;
            case 74:
                if (((int)(unsigned short)(player_character->flags & 1536)) != 0) {
                    hud_message_add((int)D_001761C4);
                    break;
                }
                if (player_object->parent->type == 1) {
                    if (xn_str_find_u16((int)D_00187644, (int)(short)l_24->faction_id, 21) != 0) {
                        l_2C = 1;
                    } else {
                        l_2C = 0;
                    }
                    in_knightly_order_hall = l_2C;
                    town_map_note_building((struct record *)*(int *)((char *)a1 + 4), l_24);
                    building_enter(l_24);
                } else {
                    building_exit();
                    in_knightly_order_hall = 0;
                }
                break;
            case 71:
            case 72:
            case 81:
                if (l_20 == 72 && l_1C == 3) {
                    if (building_is_open(l_24) == 0 && lockpick_action_door(l_24, 19, (struct record *)*(int *)((char *)a1 + 4)) != 0) {
                        loot_generate(14, (struct record *)D_001960D9, l_24->quality, (int)(unsigned short)(player_character->flags & 1));
                        shelf_object = (struct record *)(*(int *)((char *)a1 + 4));
                        shelf_return_items();
                        shelf_open_stock((struct record *)*(int *)((char *)a1 + 4), l_24, *(int *)picked_model_index);
                        return 1;
                    }
                } else if (l_1C < 4) {
                    shelf_open((struct record *)*(int *)((char *)a1 + 4), l_24, *(int *)picked_model_index);
                    return 1;
                }
            }
        }
    } else {
        switch ((unsigned)l_20) {
        case 56:
        case 331:
            if (((int)(unsigned short)(player_character->flags & 1536)) != 0) {
                hud_message_add((int)D_001761E8);
                break;
            }
            dungeon_load(-1);
            automap_load();
            break;
        case 74:
            hud_message_add((int)D_0017620C);
            break;
        case 95:
            if (wagon_container != 0) {
                msgbox_yes_no_rsc(38);
            } else {
                D_00196271 = 2;
            }
            if (((int)D_00196271) == 1) {
                D_001962B1 = 1;
                inventory_open(1, 0, 2);
            } else {
                automap_save();
                location_unload(location_object->image);
            }
        }
    }
    return 0;
}

void shelf_open(struct record *a1, struct building *a2, int a3)
{
    switch (a2->type) {
        return;
    case 11:
        if (guild_find_membership_by_kind(1) == 0) {
            msgbox_show_string((int)D_0017622D, 1);
            return;
        }
        shelf_open_books(a1, a2, a3);
        return;
    case 14:
        if (guild_find_membership_by_bits(128) == 0) {
            msgbox_show_string((int)D_00176255, 1);
            return;
        }
        shelf_open_books(a1, a2, a3);
        return;
    case 5:
    case 10:
        shelf_open_books(a1, a2, a3);
        return;
    case 0:
    case 2:
    case 3:
    case 4:
    case 6:
    case 8:
    case 9:
    case 12:
    case 13:
        shelf_open_stock(a1, a2, a3);
    default:;
    }
}

void shelf_book_chosen(int a1)
{
    D_00196272 = 0;
    book_open((int)(short)*(short *)(shelf_book_ids + (a1 * 2)));
    shelf_return_items();
}

int shelf_collect_items(struct record *a1, int a2, int a3)
{
    int l_1C;
    struct record *l_18;
    struct item *l_14;

    l_1C = 0;
    object_free_children((struct record *)D_001960D9);
    a1 = a1->children;
    while (a1 != 0) {
        l_18 = a1->next;
        if (a1->type == 36 && a1->shelf_index == a3) {
            l_14 = &a1->data.item;
            if (((struct bf8_1_1 *)&D_001940D7)->f != 0 && l_14->enchantments[0].type != (-1)) {
                a1 = l_18;
                continue;
            }
            if (a2 == (-1) || l_14->group == a2) {
                l_1C++;
                a1->flags &= ~0x2;
                a1->flags |= 32;
                object_detach(a1);
                object_reparent((struct record *)D_001960D9, a1);
                a1->x = player_object->x;
                a1->y = player_object->y;
                a1->z = player_object->z;
                a1->type = 2;
            }
        }
        a1 = l_18;
    }
    return l_1C;
}

void shelf_return_items(void)
{
    struct record *l_1C;
    struct record *l_18;

    inventory_close_callback = 0;
    l_1C = (struct record *)D_00196118;
    while (l_1C != 0) {
        l_18 = l_1C->next;
        l_1C->flags |= 2;
        object_detach(l_1C);
        object_reparent(shelf_object, l_1C);
        l_1C->type = 36;
        l_1C->shelf_index = shelf_model_index;
        l_1C = l_18;
    }
}

void shelf_open_books(struct record *a1, struct building *a2, int a3)
{
    int l_24;
    int l_20;
    struct record *l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_20 = 0;
    shelf_model_index = a3;
    l_20 = shelf_collect_items(a1, 7, a3);
    if (l_20 == 0 && shelf_shop_stocked(a3) == 0) {
        *(int *)(stocked_shop_ids + (stocked_shop_count++ << 2)) = a3;
        l_20 = a2->quality >> 1;
        if (l_20 == 0) l_20++;
        for (l_24 = 0; l_24 < l_20; l_24++) {
            l_1C = object_create_child((struct record *)D_001960D9, 0, 107);
            l_1C->type = 2;
            l_1C->x = player_object->x;
            l_1C->y = player_object->y;
            l_1C->z = player_object->z;
            l_1C->flags |= 32;
            item_make(7, a2->quality / 6, &l_1C->data.item);
        }
    }
    if (a2->type == 10 || a2->type == 11 || a2->type == 14) {
        l_18 = *(int *)scratch_buffer;
        l_14 = *(int *)scratch_buffer + 1000;
        l_10 = l_14;
        l_1C = (struct record *)D_00196118;
        l_24 = 0;
        while (l_1C != 0) {
            *(short *)(shelf_book_ids + (l_24 * 2)) = (short)l_1C->data.item.message;
            book_read_header(l_18, (int)(unsigned short)*(short *)(shelf_book_ids + (l_24++ * 2)));
            mc_strncpy(l_14, l_18, 4, (int)D_00176198, 479);
            l_14 += strlen(l_14) + 1;
            l_1C = l_1C->next;
        }
        *(signed char *)((char *)l_14) = 0;
        list_popup_open_strings(l_10);
        shelf_list_callback = (int)shelf_book_chosen;
        D_00196272 = 1;
        inventory_close_callback = (int)shelf_return_items;
        shelf_object = a1;
        shelf_list_active = 1;
        return;
    }
    inventory_open_container((struct record *)D_001960D9, 1, 4);
    inventory_close_callback = (int)shelf_return_items;
    shelf_object = a1;
}

void shelf_open_stock(struct record *a1, struct building *a2, int a3)
{
    int l_24;
    int l_20;
    struct record *l_1C;
    int l_18;
    int l_14;
    int l_10;

    l_20 = 0;
    shelf_model_index = a3;
    if (building_is_open(a2) != 0) {
        D_001940D9 |= 2;
        D_001940D7 &= 253;
    } else {
        D_001940D9 &= 253;
        D_001940D7 |= 2;
    }
    l_20 = shelf_collect_items(a1, -1, a3);
    l_24 = l_20;
    if (l_20 == 0 && shelf_shop_stocked(a3) == 0) {
        *(int *)(stocked_shop_ids + (stocked_shop_count++ << 2)) = a3;
        shelf_stock_items((struct record *)D_001960D9, a2->type + 32, a2->quality);
        if (a2->type == 0 && rand_range(1, 100) < 25) {
            l_1C = item_add_to_container((struct record *)D_001960D9, 27, 4, 0);
        }
    }
    if (l_24 == 0 && a2->type == 9) {
        l_1C = object_create_child((struct record *)D_001960D9, 0, 107);
        l_1C->type = 2;
        l_1C->flags |= 33;
        l_1C->id = object_new_id(((unsigned)location_object->id) >> 16);
        l_1C->x = player_object->x;
        l_1C->y = player_object->y;
        l_1C->z = player_object->z;
        item_make(23, 1, &l_1C->data.item);
        l_1C = object_create_child((struct record *)D_001960D9, 0, 107);
        l_1C->type = 2;
        l_1C->flags |= 33;
        l_1C->id = object_new_id(((unsigned)location_object->id) >> 16);
        l_1C->x = player_object->x;
        l_1C->y = player_object->y;
        l_1C->z = player_object->z;
        item_make(23, 0, &l_1C->data.item);
    }
    if (((struct bf8_1_1 *)&D_001940D9)->f != 0) {
        inventory_open_container((struct record *)D_001960D9, 1, 4);
    } else {
        inventory_open_container((struct record *)D_001960D9, 0, 4);
    }
    inventory_close_callback = (int)shelf_return_items;
    shelf_object = a1;
}

void shelf_book_list_update(void)
{
    int l_18;

    if (shelf_list_callback == 0) return;
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0 && (l_18 = list_popup_poll()) != (-1)) {
        if (l_18 > (-1)) ((int (*)())(shelf_list_callback))(l_18);
        shelf_list_callback = 0;
        shelf_list_active = 0;
        return;
    }
    if (shelf_list_active == 0) return;
    shelf_return_items();
    shelf_list_active = 0;
}

void npc_click_service(struct record *a1)
{
    struct person *l_20;
    struct building *l_1C;
    struct faction *l_18;

    coven_menu_npc = a1;
    D_00195CE8 = a1;
    l_20 = &a1->data.person;
    D_00195F68 = l_20->faction_id;
    l_1C = object_building(a1);
    l_18 = faction_find((int)(short)l_20->faction_id);
    if (l_18 == 0) {
        l_18 = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15);
    }
    if (a1->quest_id == 0) {
        if (l_20->faction_id == 852 || (l_18->type == 7 && l_18->region != 255)) {
            if (l_20->faction_id == 852) {
                l_18 = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 7);
                if (l_18 != 0) l_20->faction_id = l_18->id;
            }
            if (quest_active_for_faction((int)(short)l_18->id) == 0) {
                quest_pick_file(82, 0, noble_quest_letter(), 67, player_character->level);
                if (*(signed char *)D_001961F5 != 0) return;
            }
        }
    }
    if (a1->quest_id == 0 && func_0004CD80((int)l_20) != 0) return;
    if (l_20->faction_id != 0 && l_20->faction_id != 65535 && faction_find((int)(short)l_20->faction_id)->type == 8) {
        coven_menu_open(1);
        return;
    }
    if (l_1C != 0) {
        if ((l_18->id == 42 || (l_18->parent != 0 && l_18->parent->id == 42)) && guild_find_membership_by_kind(3) != 0) {
            guild_service_dispatch(a1);
        } else if ((l_18->id == 108 || (l_18->parent != 0 && l_18->parent->id == 108)) && guild_find_membership_by_kind(0) != 0) {
            guild_service_dispatch(a1);
        } else if ((l_18->id == 108 || (l_18->parent != 0 && l_18->parent->id == 108)) && guild_find_membership_by_kind(0) == 0) {
            npc_talk(a1);
        } else {
            switch (l_1C->type) {
            case 11:
            case 14:
                guild_service_dispatch(a1);
                break;
            case 15:
                if (((int)(unsigned char)(l_20->flags & 8)) != 0) {
                    tavern_open(1);
                } else {
                    npc_talk(a1);
                }
                break;
            case 3:
                service_menu_handler = (int)bank_open;
                service_menu_npc = a1;
                if (((int)(unsigned char)(l_20->flags & 8)) != 0) {
                    service_menu_open((int)D_0017628F);
                } else {
                    npc_talk(a1);
                }
                break;
            case 2:
                if (((int)(unsigned short)(l_20->faction_id & 8)) != 0) {
                    repair_menu_kind = 2;
                    repair_menu_npc = a1;
                    repair_menu_open(1);
                } else {
                    npc_talk(a1);
                }
                break;
            case 13:
                if (((int)(unsigned char)(l_20->flags & 8)) != 0) {
                    repair_menu_kind = 3;
                    repair_menu_npc = a1;
                    repair_menu_open(1);
                } else {
                    npc_talk(a1);
                }
                break;
            case 9:
                if (((int)(unsigned short)(l_20->faction_id & 8)) != 0) {
                    repair_menu_kind = 255;
                    repair_menu_npc = a1;
                    repair_menu_open(1);
                } else {
                    npc_talk(a1);
                }
                break;
            case 0:
            case 5:
            case 6:
            case 8:
            case 12:
                if (((int)(unsigned char)(l_20->flags & 8)) != 0) {
                    service_menu_npc = a1;
                    service_menu_handler = (int)service_menu_sell;
                    service_menu_open((int)D_00176297);
                } else {
                    npc_talk(a1);
                }
                break;
            default:
                npc_talk(a1);
            }
        }
        return;
    }
    npc_talk(a1);
}

void shop_open_repair(int a1, struct record *a2)
{
    struct record *l_14;

    D_001940D4 &= 253;
    l_14 = a2->children;
    if (l_14 != 0) l_14->flags |= 1;
    inventory_open_container(a2, 3, 7);
}

void npc_talk(struct record *a1)
{
    int l_1C;
    unsigned short l_18;

    text_macro_npc = (struct character *)npc_talk_record_build(a1);
    l_18 = a1->data.person.faction_id;
    l_1C = func_00031843(28, a1, 0);
    l_1C |= func_00031843(1, 0, a1);
    l_1C |= quest_raise_event(28, a1, 0);
    l_1C |= quest_raise_event(71, a1, 0);
    l_1C |= quest_raise_event(1, 0, a1);
    if (a1->type == 8 && l_18 != 0) l_1C |= quest_event_clicked_faction((int)(unsigned short)l_18);
    if (l_1C != 0) return;
    if (a1->type == 8 && a1->quest_id == 0) {
        if (((int)(unsigned char)(a1->data.person.flags & 128)) != 0) {
            quest_pick_for_npc(a1);
            a1->data.person.flags &= 127;
            if (*(signed char *)D_001961F5 != 0) return;
        }
    }
    talk_start(a1);
}

void count_items_cb(struct record *a1)
{
    if (a1->type != 2) return;
    (*(int *)scratch_190be4)++;
}

int object_count_items(struct record *a1)
{
    *(int *)scratch_190be4 = 0;
    object_foreach(a1->children, (int)count_items_cb);
    return *(int *)scratch_190be4;
}

int repair_menu_open(int a1)
{
    if (((int)(unsigned char)game_mode) == 26) return 1;
    if (a1 != 0) {
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        window_image = disk_read_file((int)D_001762DD, 0);
        game_mode = 26;
        D_00196272 = 1;
        return 1;
    }
    return 0;
}

void repair_menu_frame(void)
{
    int l_1C;
    int l_18;

    if (repair_menu_open(0) == 0) return;
    l_1C = window_image;
    xn_draw_image((int)(unsigned short)*(short *)((char *)l_1C), (int)(unsigned short)*(short *)((char *)l_1C + 2), (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), l_1C + 12);
    if (key_down_esc != 0) repair_menu_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (l_18 = 0; l_18 < 4; l_18++) {
        if (mouse_x > *(short *)(repair_menu_buttons + (l_18 * 12)) && mouse_x < *(short *)(D_001875C4 + (l_18 * 12)) && mouse_y > *(short *)(D_001875C2 + (l_18 * 12)) && mouse_y < *(short *)(D_001875C6 + (l_18 * 12))) {
            ((int (*)())(*(int *)(D_001875C8 + (l_18 * 12))))();
        }
    }
}

void repair_menu_close(void)
{
    while (key_down_esc != 0);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00176198, 891);
        window_image = -1751672937;
    }
    game_mode = 0;
    D_00196272 = 0;
}

void repair_menu_repair(void)
{
    repair_menu_close();
    shop_open_repair((int)(unsigned char)repair_menu_kind, repair_menu_npc);
}

void repair_menu_talk(void)
{
    repair_menu_close();
    npc_talk(repair_menu_npc);
}

void repair_menu_sell(void)
{
    repair_menu_close();
    object_free_children((struct record *)D_001960D9);
    inventory_open_container((struct record *)D_001960D9, 2, 6);
}

int coven_menu_open(int a1)
{
    if (((int)(unsigned char)game_mode) == 27) return 1;
    if (a1 != 0) {
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        window_image = disk_read_file((int)D_001762EA, 0);
        game_mode = 27;
        D_00196272 = 1;
        return 1;
    }
    return 0;
}

void coven_menu_frame(void)
{
    int l_1C;
    int l_18;

    if (coven_menu_open(0) == 0) return;
    l_1C = window_image;
    xn_draw_image((int)(unsigned short)*(short *)((char *)l_1C), (int)(unsigned short)*(short *)((char *)l_1C + 2), (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), l_1C + 12);
    if (key_down_esc != 0) coven_menu_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (l_18 = 0; l_18 < 4; l_18++) {
        if (mouse_x > *(short *)(coven_menu_buttons + (l_18 * 12)) && mouse_x < *(short *)(D_001875F4 + (l_18 * 12)) && mouse_y > *(short *)(D_001875F2 + (l_18 * 12)) && mouse_y < *(short *)(D_001875F6 + (l_18 * 12))) {
            ((int (*)())(*(int *)(D_001875F8 + (l_18 * 12))))();
        }
    }
}

void coven_menu_close(void)
{
    while (key_down_esc != 0);
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00176198, 957);
        window_image = -1751672937;
    }
    game_mode = 0;
    D_00196272 = 0;
}

void coven_menu_talk(void)
{
    coven_menu_close();
    npc_talk(coven_menu_npc);
}

void coven_menu_summon(void)
{
    coven_menu_close();
    daedra_summon((int)coven_menu_npc);
}

void click_item(int a1, struct record *a2)
{
    struct item *l_14;

    switch (interaction_mode) {
        return;
    case 0:
    case 2:
    case 3:
        if (pick_distance > 128) {
            hud_status_set(D_0017CA14);
        } else {
            pick_up_item(a2);
        }
        return;
    case 1:
        l_14 = &a2->data.item;
        click_describe_item(l_14);
    default:;
    }
}

void click_dungeon_model(int a1, struct record *a2)
{
    click_show_building_info(a1, a2);
}

void click_npc(int a1, struct record *a2)
{
    text_macro_npc = (struct character *)npc_talk_record_build(a2);
    switch (interaction_mode) {
        return;
    case 0:
    case 2:
    case 3:
        if (pick_distance > 256) {
            hud_status_set(D_0017CA14);
        } else {
            npc_click_service(a2);
        }
        return;
    case 1:
        mc_set_location(1021, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_00183254, npc_display_name(a2));
        hud_message_add((int)text_buffer);
    default:;
    }
}

void click_creature(int a1, struct record *a2)
{
    struct character *l_18;
    struct career *l_14;

    l_18 = &a2->data.character;
    l_14 = &l_18->career;
    if ((int)spell_ready_touch != 0 && pick_distance < 160) {
        cast_spell_on((int)spell_ready_touch, a2, 0);
        object_delete((int)spell_ready_touch);
        spell_ready_touch = 0;
    }
    switch (interaction_mode) {
        return;
    case 0:
    case 1:
    case 3:
        click_describe_creature(l_18, l_14);
        return;
    case 2:
        if (pick_distance > 128) {
            hud_status_set(D_0017CA14);
            return;
        }
        pickpocket_attempt(a2);
    default:;
    }
}

void click_door(int a1, struct record *a2)
{
    if (pick_distance > 128) {
        hud_status_set(D_0017CA14);
        return;
    }
    switch (interaction_mode) {
        return;
    case 2:
        door_try_open(a2, 1);
        return;
    case 0:
    case 1:
    case 3:
        door_try_open(a2, 0);
    default:;
    }
}

void click_marker(int a1, int a2)
{
}

void click_interior_model(int a1, struct record *a2)
{
    struct building *l_18;
    int l_14;

    switch (interaction_mode) {
        return;
    case 0:
    case 2:
    case 3:
        if (pick_distance > 128) {
            hud_status_set(D_0017CA14);
            return;
        }
        l_18 = object_building(a2);
        if (l_18 != 0 && *(int *)D_00195D3C != 0 && (((int)(unsigned short)*(short *)(*(char **)D_00195D3C)) == 418 || (((int)(unsigned short)*(short *)(*(char **)D_00195D3C)) == 410 && furniture_is_container(((int)(unsigned char)*(signed char *)(*(char **)D_00195D3C + 2)) + (((int)(unsigned short)*(short *)(*(char **)D_00195D3C)) << 7)) != 0)) && (l_14 = func_000612A1()) != 0) {
            if (l_18->id == player_character->house) {
                D_001940D6 |= 4;
                inventory_open_container((struct record *)D_00196092, 0, 4);
            } else {
                msgbox_yes_no_rsc(37);
                if (((int)D_00196271) != 1) return;
                if (l_14 != 0 && ((int)player_environment) == 2 && rand_range(0, 255) >= (108 - player_character->skills[15].value)) {
                    crime_current = 13;
                    guards_summon(1);
                }
                if (l_14 != 0) {
                    D_001940D6 |= 4;
                    inventory_open_container((struct record *)D_00196120, 0, 4);
                }
            }
        } else if (l_18 != 0 && *(int *)D_00195D3C != 0 && ((int)(unsigned short)*(short *)(*(char **)D_00195D3C)) == 414 && ((int)(unsigned char)*(signed char *)(*(char **)D_00195D3C + 2)) == 9) {
            ladder_climb();
        }
        return;
    case 1:
        if (l_18 != 0 && *(int *)D_00195D3C != 0 && ((int)(unsigned short)*(short *)(*(char **)D_00195D3C)) == 414 && ((int)(unsigned char)*(signed char *)(*(char **)D_00195D3C + 2)) == 9) {
            ladder_climb();
            return;
        }
        click_show_building_info(a1, a2);
    default:;
    }
}

void click_corpse(int a1, struct record *a2)
{
    struct character *l_14;

    switch (interaction_mode) {
        return;
    case 1:
        l_14 = &a2->data.character;
        if (l_14->race < 43) {
            mc_set_location(1153, (int)D_00176198);
            mc_sprintf((int)text_buffer, D_00184329, *(int *)(monster_names + (l_14->race << 2)));
        } else {
            mc_strncpy((int)text_buffer, D_00185093, 160, (int)D_00176198, 1155);
        }
        hud_message_add((int)text_buffer);
        return;
    case 0:
    case 2:
    case 3:
        if (pick_distance > 150) {
            hud_status_set(D_0017CA14);
            return;
        }
        if (object_count_items(a2) != 0) {
            inventory_open_container(a2, 0, 1);
            return;
        }
        hud_message_add(D_001845CC);
    default:;
    }
}

void click_pedestrian(int a1, struct record *a2)
{
    switch (interaction_mode) {
        return;
    case 2:
        if (pick_distance > 128) {
            hud_status_set(D_0017CA14);
        } else if (((int)(unsigned short)(a2->npc_flags & 16384)) == 0) {
            a2->npc_flags |= 0x4000;
            pickpocket_attempt(a2);
        }
        return;
    case 0:
    case 1:
    case 3:
        if (pick_distance > 256) {
            hud_status_set(D_0017CA14);
            return;
        }
        npc_talk(a2);
    default:;
    }
}

void click_loot_container(int a1, struct record *a2)
{
    if (pick_distance > 128) {
        hud_status_set(D_0017CA14);
        return;
    }
    switch (interaction_mode) {
        return;
    case 0:
    case 1:
    case 2:
    case 3:
        if ((a2->image >> 7) != 216) if (a2->image != 26112) return;
        if (a2->children == 0) loot_fill_container(a2);
        container_items_to_player(a2);
        if (a2->children == 0) return;
        inventory_open_container(a2, 0, 5);
    default:;
    }
}

void click_town_scenery(int a1, struct record *a2)
{
    switch (interaction_mode) {
    return;
case 0:
case 1:
case 2:
case 3:
    click_show_building_info(a1, a2);
default:;
}
}

void click_show_building_info(int a1, struct record *a2)
{
    struct building *l_18;
    int l_14;

    if (a2->type == 56 && ((int)(short)*(short *)(*(char **)click_hit + 14)) == 417 && ((int)(short)*(short *)(*(char **)click_hit + 16)) == 39) {
        rumor_show_local();
        return;
    }
    if (a2->parent->type != 38) return;
    l_18 = object_building(a2);
    if (l_18 == 0) return;
    if (l_18->type == 23) return;
    town_map_note_building(a2, l_18);
    hud_message_add(building_name(l_18));
    if (l_18->type == 1 || building_is_open(l_18) != 0 || l_18->type >= 14) return;
    mc_set_location(1274, (int)D_00176198);
    mc_sprintf((int)text_buffer, (int)D_001762F7, (int)(unsigned char)building_open_hours[l_18->type * 2], (int)(unsigned char)D_0017C5B9[l_18->type * 2]);
    hud_message_add((int)text_buffer);
}

int quest_active_for_faction(short a1)
{
    struct record *l_20;

    l_20 = quest_root->children;
    while (l_20 != 0) {
        if (l_20->data.quest.faction_id == a1) return 1;
        l_20 = l_20->next;
    }
    return 0;
}

int noble_quest_letter(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    for (l_1C = 0; l_1C < 29; l_1C++) {
        if (region_event_flags[(((int)(unsigned char)current_region) * 80) + l_1C] != 0) {
            l_20++;
        }
    }
    if (l_20 == 0 || rand_range(1, 100) < 10) return 0;
    l_20 = rand_range(0, l_20 - 1);
    for (l_1C = 0; l_1C < 29; l_1C++) {
        if (region_event_flags[(((int)(unsigned char)current_region) * 80) + l_1C] != 0) {
            if (l_20 == 0) return l_1C + 65;
            l_20--;
        }
    }
    return 0;
}

int service_menu_open(int a1)
{
    if (((int)(unsigned char)game_mode) == 28) return 1;
    if (a1 != 0) {
        service_menu_label = a1;
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        window_image = disk_read_file((int)D_00176322, 0);
        game_mode = 28;
        D_00196272 = 1;
        return 1;
    }
    return 0;
}

void service_menu_frame(void)
{
    int l_1C;
    int l_18;

    if (service_menu_open(0) == 0) return;
    l_1C = window_image;
    xn_draw_image((int)(unsigned short)*(short *)((char *)l_1C), (int)(unsigned short)*(short *)((char *)l_1C + 2), (int)(unsigned short)*(short *)((char *)l_1C + 4), (int)(unsigned short)*(short *)((char *)l_1C + 6), l_1C + 12);
    text_draw_centred_coloured(service_menu_label, 159, 70, 145, 156);
    if (key_down_esc != 0) service_menu_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (l_18 = 0; l_18 < 3; l_18++) {
        if (mouse_x > *(short *)(service_menu_buttons + (l_18 * 12)) && mouse_x < *(short *)(D_00187624 + (l_18 * 12)) && mouse_y > *(short *)(D_00187622 + (l_18 * 12)) && mouse_y < *(short *)(D_00187626 + (l_18 * 12))) {
            ((int (*)())(*(int *)(D_00187628 + (l_18 * 12))))();
        }
    }
}

void service_menu_close(void)
{
    while (mouse_buttons != 0) xn_mouse_poll_clamped();
    while (key_down_esc != 0);
    if (window_image != 0 && window_image != (-1751672937)) {
        mc_free(window_image, (int)D_00176198, 1372);
        window_image = -1751672937;
    }
    game_mode = 0;
    D_00196272 = 0;
}

void service_menu_talk(void)
{
    service_menu_close();
    npc_talk(service_menu_npc);
}

void service_menu_service(void)
{
    service_menu_close();
    ((int (*)())(service_menu_handler))(1);
}

void service_menu_sell(void)
{
    repair_menu_sell();
}

int shelf_shop_stocked(int a1)
{
    int l_1C;

    for (l_1C = 0; l_1C < stocked_shop_count; l_1C++) {
        if (*(int *)(stocked_shop_ids + (l_1C << 2)) == a1) return 1;
    }
    return 0;
}

void container_items_to_player(struct record *a1)
{
    if (a1 == 0) return;
    a1 = a1->children;
    while (a1 != 0) {
        a1->x = player_object->x;
        a1->y = player_object->y;
        a1->z = player_object->z;
        a1 = a1->next;
    }
}

void ambient_dungeon_sounds(void)
{
    if (((int)player_environment) != 3 || D_001962A1 != 0) {
        return;
    }
    if (rand() >= 20) return;
    sound_play(rand_range(0, 14) + 329, player_object, 100);
}

void footstep_sounds(void)
{
    int l_1C;
    int l_18;

    if (xn_math_approx_dist2d(D_001A4FBC, D_001A4FC4, player_object->x, player_object->z) <= 100 || player_on_ground == 0) {
        return;
    }
    D_001A4FBC = player_object->x;
    D_001A4FC4 = player_object->z;
    switch (player_environment) {
    case 1:
        l_18 = (int)(unsigned char)climate_weathers[climate_category()];
        if ((l_18 & 127) == 5) {
            l_1C = 6;
        } else {
            l_1C = 4;
        }
        break;
    case 2:
        l_1C = 2;
        break;
    case 3:
        l_1C = (int)(signed char)*(signed char *)(*(char **)D_00195AB4 + 4);
        if (l_1C > 4 || l_1C < 0) l_1C = 0;
        l_1C <<= 1;
        if (dungeon_water_level != 10000 && player_object->y > dungeon_water_level && in_dungeon_water == 0) {
            l_1C = 8;
        }
        if (in_dungeon_water != 0 && ((int)(unsigned char)player_underwater) != 1) {
            l_1C = 10;
        } else {
            if (in_dungeon_water != 0) return;
        }
    }
    if (D_001962A0 != 0) l_1C = 10;
    position_history_record();
    if (((int)(unsigned short)(player_character->flags & 1536)) != 0) return;
    if (((struct bf8_6_1 *)&D_001940D7)->f != 0) l_1C++;
    D_001940D7 ^= 64;
    sound_play((int)(unsigned char)footstep_sound_ids[l_1C], player_object, 100);
}

void position_history_reset(void)
{
    int l_18;

    D_001A4FCC = (position_history_next = (D_001A4FD4 = (ground_position_history_next = 0)));
    for (l_18 = 0; l_18 < 32; l_18++) {
        *(int *)(position_history + (l_18 * 12)) = (*(int *)(ground_position_history + (l_18 * 12)) = player_object->x);
        *(int *)(D_001A4CA4 + (l_18 * 12)) = (*(int *)(D_001A4E24 + (l_18 * 12)) = player_object->y);
        *(int *)(D_001A4CA8 + (l_18 * 12)) = (*(int *)(D_001A4E28 + (l_18 * 12)) = player_object->z);
    }
}

void position_history_record(void)
{
    *(int *)(position_history + (position_history_next * 12)) = player_object->x;
    *(int *)(D_001A4CA4 + (position_history_next * 12)) = player_object->y;
    *(int *)(D_001A4CA8 + (position_history_next * 12)) = player_object->z;
    position_history_next = (position_history_next + 1) % 32;
    D_001A4FCC++;
    if (player_on_ground == 0) return;
    *(int *)(ground_position_history + (ground_position_history_next * 12)) = player_object->x;
    *(int *)(D_001A4E24 + (ground_position_history_next * 12)) = player_object->y;
    *(int *)(D_001A4E28 + (ground_position_history_next * 12)) = player_object->z;
    ground_position_history_next = (ground_position_history_next + 1) % 32;
    D_001A4FD4++;
}

int spawn_point_visible(struct record *a1)
{
    int l_24;
    int l_20;
    int l_1C;

    if (D_0019629D != 0) return 0;
    if (collide_line_of_sight(player_object, a1) == 0) return 0;
    l_24 = xn_math_angle_to_point(player_object->x, player_object->z, a1->x, a1->z);
    l_20 = ai_angle_diff(a1->yaw, l_24, (int)&l_1C);
    return ((l_20 < 400) ? 1 : 0);
}

int spawn_point_wilderness(struct record *a1)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_1C = 0;
    if (l_1C < 20) {
        l_2C = ((rand() % 90) + ((int)(short)*(short *)((char *)*(int *)&player_object + 3))) - 45;
        l_2C &= 2047;
        l_28 = (rand_range(1, 512) + xn_cam_far_z) - 256;
        xn_math_yaw_offset_xz(l_2C, l_28, (int)&l_24, (int)&l_20);
        a1->x = player_object->x + l_24;
        a1->z = player_object->z + l_20;
        a1->y = xn_terrain_height_at(a1->x, a1->z);
        return 1;
    }
    for (l_1C = 0; l_1C < 50; l_1C++) {
        xn_math_yaw_offset_xz(rand() % 2048, rand_range(512, 768), (int)&l_24, (int)&l_20);
        a1->x = player_object->x + l_24;
        a1->z = player_object->z + l_20;
        a1->y = xn_terrain_height_at(a1->x, a1->z);
        if (spawn_point_visible(a1) == 0) return 1;
    }
    return 0;
}

int spawn_point_town(struct record *a1, int a2, int a3)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    if (a2 == 0) {
        a2 = 128;
        a3 = 3096;
    }
    for (l_18 = 0; l_18 < 50; l_18++) {
        xn_math_yaw_offset_xz(rand() % 2048, rand_range(a2, a3), (int)&l_20, (int)&l_1C);
        a1->x = player_object->x + l_20;
        a1->z = player_object->z + l_1C;
        a1->y = xn_terrain_height_at(a1->x, a1->z);
        if (pedestrian_spawn_spot_ok(a1, a1->x - location_object->x, a1->z - location_object->z) != 0) return 1;
    }
    if (position_history_next < 20) return 0;
    for (l_18 = 0; l_18 < 20; l_18++) {
        l_14 = rand() % 32;
        l_24 = xn_math_approx_dist2d(player_object->x, player_object->z, *(int *)(position_history + (l_14 * 12)), *(int *)(D_001A4CA8 + (l_14 * 12)));
        if (l_24 > a2 && l_24 < a3) {
            a1->x = *(int *)(position_history + (l_14 * 12));
            a1->y = *(int *)(D_001A4CA4 + (l_14 * 12));
            a1->z = *(int *)(D_001A4CA8 + (l_14 * 12));
            if (pedestrian_spawn_spot_ok(a1, a1->x, a1->z) != 0) return 1;
        }
    }
    return 0;
}

void spawn_point_occupied_cb(struct record *a1)
{
    int l_18;

    if ((int)scratch_object == 0) return;
    if (a1->type != 18) return;
    l_18 = xn_math_approx_hypot(scratch_object->y - a1->y, xn_math_approx_dist2d(scratch_object->x, scratch_object->z, a1->x, a1->z));
    if (l_18 >= 64) return;
    scratch_object = 0;
}

int spawn_point_occupied(struct record *a1)
{
    scratch_object = a1;
    object_foreach(location_object, (int)spawn_point_occupied_cb);
    return (((int)scratch_object == 0) ? 1 : 0);
}

int spawn_point_building(struct record *a1)
{
    int l_28;
    int l_24;
    int l_20;
    struct record *l_1C;

    l_20 = marker_count(player_object->parent->children, 6);
    if (l_20 == 0) return 0;
    l_24 = rand() % l_20;
    for (l_28 = 0; l_28 < l_20; l_28++) {
        l_1C = marker_find_nth(player_object->parent->children, 6, l_24);
        if (abs(player_object->y - l_1C->y) < 64) {
            if (xn_math_approx_dist2d(player_object->x, player_object->z, l_1C->x, l_1C->z) > 80) {
                if (spawn_point_visible(a1) == 0) {
                    a1->x = l_1C->x;
                    a1->z = l_1C->z;
                    a1->y = l_1C->y;
                    return 1;
                }
            }
        }
        l_24 = (l_24 + 1) % l_20;
    }
    for (l_28 = 0; l_28 < l_20; l_28++) {
        l_1C = marker_find_nth(player_object->parent->children, 6, l_24);
        if (abs(player_object->y - l_1C->y) > 64) {
            if (xn_math_approx_dist2d(player_object->x, player_object->z, l_1C->x, l_1C->z) > 80) {
                if (spawn_point_visible(a1) == 0) {
                    a1->x = l_1C->x;
                    a1->z = l_1C->z;
                    a1->y = l_1C->y;
                    return 1;
                }
            }
        }
        l_24 = (l_24 + 1) % l_20;
    }
    l_1C = marker_find_nth(player_object->parent->children, 6, l_24);
    a1->x = l_1C->x;
    a1->z = l_1C->z;
    a1->y = l_1C->y;
    return 1;
}

int spawn_point_dungeon_level(struct record *a1, int a2, int a3, int a4)
{
    struct record *l_2C;
    struct record *l_28;
    char l_80[72];
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;

    if (a2 < 0 || a2 > 8) return 0;
    l_2C = location_cell_at(player_object->x, player_object->z);
    l_28 = object_find_type(l_2C, 60);
    if (l_28 == 0) return 0;
    l_20 = (int)RECORD_DATA(l_28) + (a2 << 6);
    l_1C = 0;
    l_14 = a4;
    for (l_10 = 0; l_10 < 64; l_10++) {
        if (*(signed char *)((char *)l_20++) != 0) {
            *(int *)((char *)l_80 + 7) = (((l_10 % 8) << 8) + l_28->x) + 128;
            *(int *)((char *)l_80 + 15) = (((l_10 / 8) << 8) + l_28->z) + 128;
            *(int *)((char *)l_80 + 11) = (-(a2 << 8)) - 80;
            l_18 = xn_math_approx_dist2d(player_object->x, player_object->z, *(int *)((char *)l_80 + 7), *(int *)((char *)l_80 + 15));
            if (l_18 < l_14 && l_18 > a3 && spawn_point_visible((struct record *)l_80) == 0) {
                l_14 = l_18;
                a1->x = *(int *)((char *)l_80 + 7);
                a1->z = *(int *)((char *)l_80 + 15);
                a1->y = *(int *)((char *)l_80 + 11) + 80;
            }
        }
    }
    return a1->x;
}

int spawn_point_dungeon(struct record *a1, int a2, int a3)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_20 = (-player_object->y) / 256;
    if (spawn_point_dungeon_level(a1, l_20, a2, a3) != 0 && spawn_point_occupied(a1) == 0) return 1;
    for (l_14 = 0; l_14 < 8; l_14++) {
        if (spawn_point_dungeon_level(a1, l_14, a2, a3) != 0 && spawn_point_occupied(a1) == 0) return 1;
    }
    if (position_history_next < 20) return 0;
    for (l_14 = 0; l_14 < 20; l_14++) {
        l_1C = rand() % 32;
        l_18 = xn_math_approx_dist2d(player_object->x, player_object->z, *(int *)(position_history + (l_1C * 12)), *(int *)(D_001A4CA8 + (l_1C * 12)));
        if (l_18 > a2 && l_18 < a3) {
            a1->x = *(int *)(position_history + (l_1C * 12));
            a1->y = *(int *)(D_001A4CA4 + (l_1C * 12));
            a1->z = *(int *)(D_001A4CA8 + (l_1C * 12));
            if (spawn_point_fits(a1) != 0) return 1;
        }
    }
    a1->z = 0;
    a1->y = a1->z;
    a1->x = a1->y;
    return 0;
}

void ambient_outdoor_sounds(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (((int)player_environment) == 1) {
        l_1C = location_contains(player_object->x, player_object->z);
        if (l_1C != 0 && current_location->kind == 12 && rand() < 30) {
            sound_play(16, player_object, 100);
        }
        if (l_1C != 0 && current_location->kind == 12 && rand() < 30) {
            sound_play(383, player_object, 100);
        }
        l_18 = (int)(unsigned char)climate_weathers[climate_category()];
        if (l_18 == 4 || l_18 == 5 || l_18 == 3 || (l_18 & 128) != 0) {
            l_20 = 1;
        } else {
            l_20 = 0;
        }
        l_18 = l_20;
        if (daylight != 0 && l_18 == 0 && rand() < 30) {
            sound_play((rand() & 1) + 300, player_object, 100);
        }
        if (((int)(unsigned short)(player_character->flags & 1536)) != 0 && rand() < 80) {
            sound_play(367, player_object, 100);
        }
    } else if (((int)player_environment) == 3) {
        if (dungeon_water_level != 10000 && rand() < 50) {
            sound_play_at_point(302, player_object->x, dungeon_water_level, player_object->z, 100);
        }
    } else if (((int)player_environment) == 2) {
        if (current_building->type == 5 && rand() < 100) sound_play(376, player_object, 100);
    }
    if (player_character->race <= 8 || rand() >= 50) return;
    sound_play(10090, player_object, 100);
}

int music_dungeon_song(void)
{
    int l_24;
    int l_20;
    short l_18;

    *(int *)&l_18 = ((unsigned)location_object->id) >> 16;
    for (l_20 = 0; l_20 < 10; l_20++) {
        if ((short)*(int *)&l_18 == music_special_dungeon_ids[l_20]) {
            if (music_uses_fm != 0) {
                l_24 = D_001878AC[l_20];
            } else {
                l_24 = D_001878D4[l_20];
            }
            return l_24;
        }
    }
    return 0;
}

void cheat_return_to_last_position(void)
{
    ground_position_history_next--;
    if (ground_position_history_next == (-1)) ground_position_history_next = 31;
    player_object->x = *(int *)(ground_position_history + (ground_position_history_next * 12));
    player_object->y = *(int *)(D_001A4E24 + (ground_position_history_next * 12));
    player_object->z = *(int *)(D_001A4E28 + (ground_position_history_next * 12));
    D_001940D5 |= 2;
}
