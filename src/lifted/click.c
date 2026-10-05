/* click.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern struct region regions[];
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
extern struct rect repair_menu_buttons[];
extern struct rect coven_menu_buttons[];
extern struct rect service_menu_buttons[];
extern char D_00187644[];
extern signed char footstep_sound_ids[];
extern short music_special_dungeon_ids[];
extern int D_001878AC[];
extern int D_001878D4[];
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
extern struct arch3d_plane *collide_floor_plane;
extern struct record *location_object;
extern int inventory_close_callback;
extern struct pick_result *click_hit;
extern int shelf_list_callback;
extern struct record *shelf_object;
extern struct record *spell_ready_touch;
extern struct record *scratch_object;
extern struct location *current_location;
extern struct character *player_character;
extern int window_image;
extern char scratch_buffer[];
extern struct record *D_00195CE8;
extern struct block_model *D_00195D3C;
extern char picked_model_index[];
extern struct arch3d_plane *click_face_texture;
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
extern struct faction *faction_find_type_in_region(short, short);
extern struct faction *faction_find(short);
extern int tavern_open(short);
extern int climate_category(void);
extern int collide_line_of_sight(struct record *, struct record *);
extern int quest_event_clicked_faction(unsigned short);
extern int func_00031843(short, struct record *, struct record *);
extern int list_popup_poll(void);
extern int pedestrian_spawn_spot_ok(struct record *, int, int);
extern struct record *item_add_to_container(struct record *, int, int, int);
extern int building_is_open(struct building *);
extern int quest_raise_event();
extern int quest_pick_file();
extern int func_0004CD80(struct person *);
extern int npc_talk_record_build(struct record *);
extern int func_000612A1(void);
extern int ai_angle_diff(int, int, int *);
extern int sound_play(int, struct record *, int);
extern int sound_play_at_point(int, int, int, int, int);
extern int bank_open(int);
extern int disk_read_file(char *, int);
extern struct membership *guild_find_membership_by_kind(unsigned char);
extern struct membership *guild_find_membership_by_bits(unsigned char);
extern int spawn_point_fits(struct record *);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int location_contains(int, int);
extern int npc_display_name(struct record *);
extern int building_name(struct building *);
extern struct record *object_delete(struct record *);
extern struct record *object_detach(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
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
extern void daedra_summon(struct record *);
extern void town_map_note_building(struct record *, struct building *);
extern void automap_save(void);
extern void automap_load(void);
extern void msgbox_show_string(char *, short);
extern void guards_summon(int);
extern void quest_pick_for_npc(struct record *);
extern void book_open(short);
extern void cast_spell_on(struct record *, struct record *, int);
extern void item_make(int, int, struct item *);
extern void shelf_stock_items(struct record *, int, int);
extern void loot_generate(int, struct record *, int, int);
extern void loot_fill_container(struct record *);
extern void guild_service_dispatch(struct record *);
extern void door_try_open(struct record *, int);
extern void book_read_header(char *, int);
extern void pick_up_item(struct record *);
extern void text_draw_centred_coloured(int, int, int, int, unsigned char);
extern void hud_status_set(int);
extern void list_popup_open_strings(char *);
extern void msgbox_yes_no_rsc(int);
extern void dungeon_load(int);
extern void location_unload(int);
extern void building_enter(struct building *);
extern void building_exit(void);
extern void object_free_children(struct record *);
extern void object_foreach(struct record *, void (*)());
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
void click_show_building_info(struct pick_result *, struct record *);
void service_menu_close(void);
void service_menu_sell(void);
void container_items_to_player(struct record *);
void position_history_record(void);
void spawn_point_occupied_cb(struct record *);
#pragma aux mc_set_location parm routine [];

void click_describe_item(struct item *item)
{
    if (strchr(D_00183248, (int)(unsigned char)item->name[0]) != 0) {
        mc_set_location(204, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_0018324C, item);
    } else {
        mc_set_location(206, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_00183250, item);
    }
    hud_message_add((int)text_buffer);
}

void click_describe_creature(struct character *unused, struct career *creature_class)
{
    if (strchr(D_00183248, (int)(unsigned char)creature_class->name[0]) != 0) {
        mc_set_location(215, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_0018324C, creature_class->name);
    } else {
        mc_set_location(217, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_00183250, creature_class->name);
    }
    hud_message_add((int)text_buffer);
}

int click_world_face(struct pick_result *hit)
{
    struct building *building;
    int archive;
    int record_index;

    building = object_building(hit->object);
    if (hit->object->type != 6 && hit->object->type != 43 && hit->object->type != 56) {
        return 0;
    }
    archive = click_face_texture->texture >> 7;
    record_index = click_face_texture->texture & 127;
    if ((archive % 100) == 74) archive = 74;
    if (building != 0) {
        {
            unsigned char knightly;
            switch ((unsigned)archive) {
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
                    if (xn_str_find_u16((int)D_00187644, (int)(short)building->faction_id, 21) != 0) {
                        knightly = 1;
                    } else {
                        knightly = 0;
                    }
                    in_knightly_order_hall = knightly;
                    town_map_note_building(hit->object, building);
                    building_enter(building);
                } else {
                    building_exit();
                    in_knightly_order_hall = 0;
                }
                break;
            case 71:
            case 72:
            case 81:
                if (archive == 72 && record_index == 3) {
                    if (building_is_open(building) == 0 && lockpick_action_door(building, 19, hit->object) != 0) {
                        loot_generate(14, (struct record *)D_001960D9, building->quality, (int)(unsigned short)(player_character->flags & 1));
                        shelf_object = hit->object;
                        shelf_return_items();
                        shelf_open_stock(hit->object, building, *(int *)picked_model_index);
                        return 1;
                    }
                } else if (record_index < 4) {
                    shelf_open(hit->object, building, *(int *)picked_model_index);
                    return 1;
                }
            }
        }
    } else {
        switch ((unsigned)archive) {
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

void shelf_open(struct record *shelf, struct building *shelf_building, int model_index)
{
    switch (shelf_building->type) {
        return;
    case 11:
        if (guild_find_membership_by_kind(1) == 0) {
            msgbox_show_string(D_0017622D, 1);
            return;
        }
        shelf_open_books(shelf, shelf_building, model_index);
        return;
    case 14:
        if (guild_find_membership_by_bits(128) == 0) {
            msgbox_show_string(D_00176255, 1);
            return;
        }
        shelf_open_books(shelf, shelf_building, model_index);
        return;
    case 5:
    case 10:
        shelf_open_books(shelf, shelf_building, model_index);
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
        shelf_open_stock(shelf, shelf_building, model_index);
    default:;
    }
}

void shelf_book_chosen(int row)
{
    D_00196272 = 0;
    book_open((int)(short)*(short *)(shelf_book_ids + (row * 2)));
    shelf_return_items();
}

int shelf_collect_items(struct record *object, int group, int model_index)
{
    int count;
    struct record *next;
    struct item *item;

    count = 0;
    object_free_children((struct record *)D_001960D9);
    object = object->children;
    while (object != 0) {
        next = object->next;
        if (object->type == 36 && object->shelf_index == model_index) {
            item = &object->data.item;
            if (((struct bf8_1_1 *)&D_001940D7)->f != 0 && item->enchantments[0].type != (-1)) {
                object = next;
                continue;
            }
            if (group == (-1) || item->group == group) {
                count++;
                object->flags &= ~0x2;
                object->flags |= 32;
                object_detach(object);
                object_reparent((struct record *)D_001960D9, object);
                object->x = player_object->x;
                object->y = player_object->y;
                object->z = player_object->z;
                object->type = 2;
            }
        }
        object = next;
    }
    return count;
}

void shelf_return_items(void)
{
    struct record *object;
    struct record *next;

    inventory_close_callback = 0;
    object = (struct record *)D_00196118;
    while (object != 0) {
        next = object->next;
        object->flags |= 2;
        object_detach(object);
        object_reparent(shelf_object, object);
        object->type = 36;
        object->shelf_index = shelf_model_index;
        object = next;
    }
}

void shelf_open_books(struct record *shelf, struct building *shelf_building, int model_index)
{
    int i;
    int count;
    struct record *object;
    char *header;
    char *text;
    char *list;

    count = 0;
    shelf_model_index = model_index;
    count = shelf_collect_items(shelf, 7, model_index);
    if (count == 0 && shelf_shop_stocked(model_index) == 0) {
        *(int *)(stocked_shop_ids + (stocked_shop_count++ << 2)) = model_index;
        count = shelf_building->quality >> 1;
        if (count == 0) count++;
        for (i = 0; i < count; i++) {
            object = object_create_child((struct record *)D_001960D9, 0, 107);
            object->type = 2;
            object->x = player_object->x;
            object->y = player_object->y;
            object->z = player_object->z;
            object->flags |= 32;
            item_make(7, shelf_building->quality / 6, &object->data.item);
        }
    }
    if (shelf_building->type == 10 || shelf_building->type == 11 || shelf_building->type == 14) {
        header = *(char **)scratch_buffer;
        text = *(char **)scratch_buffer + 1000;
        list = text;
        object = (struct record *)D_00196118;
        i = 0;
        while (object != 0) {
            *(short *)(shelf_book_ids + (i * 2)) = (short)object->data.item.message;
            book_read_header(header, (int)(unsigned short)*(short *)(shelf_book_ids + (i++ * 2)));
            mc_strncpy(text, header, 4, (int)D_00176198, 479);
            text += strlen(text) + 1;
            object = object->next;
        }
        *text = 0;
        list_popup_open_strings(list);
        shelf_list_callback = (int)shelf_book_chosen;
        D_00196272 = 1;
        inventory_close_callback = (int)shelf_return_items;
        shelf_object = shelf;
        shelf_list_active = 1;
        return;
    }
    inventory_open_container((struct record *)D_001960D9, 1, 4);
    inventory_close_callback = (int)shelf_return_items;
    shelf_object = shelf;
}

void shelf_open_stock(struct record *shelf, struct building *shelf_building, int model_index)
{
    int collected_count;
    int count;
    struct record *object;
    int unused;
    int unused2;
    int unused3;

    count = 0;
    shelf_model_index = model_index;
    if (building_is_open(shelf_building) != 0) {
        D_001940D9 |= 2;
        D_001940D7 &= 253;
    } else {
        D_001940D9 &= 253;
        D_001940D7 |= 2;
    }
    count = shelf_collect_items(shelf, -1, model_index);
    collected_count = count;
    if (count == 0 && shelf_shop_stocked(model_index) == 0) {
        *(int *)(stocked_shop_ids + (stocked_shop_count++ << 2)) = model_index;
        shelf_stock_items((struct record *)D_001960D9, shelf_building->type + 32, shelf_building->quality);
        if (shelf_building->type == 0 && rand_range(1, 100) < 25) {
            object = item_add_to_container((struct record *)D_001960D9, 27, 4, 0);
        }
    }
    if (collected_count == 0 && shelf_building->type == 9) {
        object = object_create_child((struct record *)D_001960D9, 0, 107);
        object->type = 2;
        object->flags |= 33;
        object->id = object_new_id(((unsigned)location_object->id) >> 16);
        object->x = player_object->x;
        object->y = player_object->y;
        object->z = player_object->z;
        item_make(23, 1, &object->data.item);
        object = object_create_child((struct record *)D_001960D9, 0, 107);
        object->type = 2;
        object->flags |= 33;
        object->id = object_new_id(((unsigned)location_object->id) >> 16);
        object->x = player_object->x;
        object->y = player_object->y;
        object->z = player_object->z;
        item_make(23, 0, &object->data.item);
    }
    if (((struct bf8_1_1 *)&D_001940D9)->f != 0) {
        inventory_open_container((struct record *)D_001960D9, 1, 4);
    } else {
        inventory_open_container((struct record *)D_001960D9, 0, 4);
    }
    inventory_close_callback = (int)shelf_return_items;
    shelf_object = shelf;
}

void shelf_book_list_update(void)
{
    int row;

    if (shelf_list_callback == 0) return;
    if (((struct bf8_2_1 *)&D_001940D4)->f != 0 && (row = list_popup_poll()) != (-1)) {
        if (row > (-1)) ((int (*)())(shelf_list_callback))(row);
        shelf_list_callback = 0;
        shelf_list_active = 0;
        return;
    }
    if (shelf_list_active == 0) return;
    shelf_return_items();
    shelf_list_active = 0;
}

void npc_click_service(struct record *npc)
{
    struct person *person;
    struct building *building;
    struct faction *faction;

    coven_menu_npc = npc;
    D_00195CE8 = npc;
    person = &npc->data.person;
    D_00195F68 = person->faction_id;
    building = object_building(npc);
    faction = faction_find((int)(short)person->faction_id);
    if (faction == 0) {
        faction = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 15);
    }
    if (npc->quest_id == 0) {
        if (person->faction_id == 852 || (faction->type == 7 && faction->region != 255)) {
            if (person->faction_id == 852) {
                faction = faction_find_type_in_region((int)(short)((int)(unsigned char)current_region), 7);
                if (faction != 0) person->faction_id = faction->id;
            }
            if (quest_active_for_faction((int)(short)faction->id) == 0) {
                quest_pick_file(82, 0, noble_quest_letter(), 67, player_character->level);
                if (*(signed char *)D_001961F5 != 0) return;
            }
        }
    }
    if (npc->quest_id == 0 && func_0004CD80(person) != 0) return;
    if (person->faction_id != 0 && person->faction_id != 65535 && faction_find((int)(short)person->faction_id)->type == 8) {
        coven_menu_open(1);
        return;
    }
    if (building != 0) {
        if ((faction->id == 42 || (faction->parent != 0 && faction->parent->id == 42)) && guild_find_membership_by_kind(3) != 0) {
            guild_service_dispatch(npc);
        } else if ((faction->id == 108 || (faction->parent != 0 && faction->parent->id == 108)) && guild_find_membership_by_kind(0) != 0) {
            guild_service_dispatch(npc);
        } else if ((faction->id == 108 || (faction->parent != 0 && faction->parent->id == 108)) && guild_find_membership_by_kind(0) == 0) {
            npc_talk(npc);
        } else {
            switch (building->type) {
            case 11:
            case 14:
                guild_service_dispatch(npc);
                break;
            case 15:
                if (((int)(unsigned char)(person->flags & 8)) != 0) {
                    tavern_open(1);
                } else {
                    npc_talk(npc);
                }
                break;
            case 3:
                service_menu_handler = (int)bank_open;
                service_menu_npc = npc;
                if (((int)(unsigned char)(person->flags & 8)) != 0) {
                    service_menu_open((int)D_0017628F);
                } else {
                    npc_talk(npc);
                }
                break;
            case 2:
                if (((int)(unsigned short)(person->faction_id & 8)) != 0) {
                    repair_menu_kind = 2;
                    repair_menu_npc = npc;
                    repair_menu_open(1);
                } else {
                    npc_talk(npc);
                }
                break;
            case 13:
                if (((int)(unsigned char)(person->flags & 8)) != 0) {
                    repair_menu_kind = 3;
                    repair_menu_npc = npc;
                    repair_menu_open(1);
                } else {
                    npc_talk(npc);
                }
                break;
            case 9:
                if (((int)(unsigned short)(person->faction_id & 8)) != 0) {
                    repair_menu_kind = 255;
                    repair_menu_npc = npc;
                    repair_menu_open(1);
                } else {
                    npc_talk(npc);
                }
                break;
            case 0:
            case 5:
            case 6:
            case 8:
            case 12:
                if (((int)(unsigned char)(person->flags & 8)) != 0) {
                    service_menu_npc = npc;
                    service_menu_handler = (int)service_menu_sell;
                    service_menu_open((int)D_00176297);
                } else {
                    npc_talk(npc);
                }
                break;
            default:
                npc_talk(npc);
            }
        }
        return;
    }
    npc_talk(npc);
}

void shop_open_repair(int unused, struct record *npc)
{
    struct record *first;

    D_001940D4 &= 253;
    first = npc->children;
    if (first != 0) first->flags |= 1;
    inventory_open_container(npc, 3, 7);
}

void npc_talk(struct record *npc)
{
    int handled;
    unsigned short faction_id;

    text_macro_npc = (struct character *)npc_talk_record_build(npc);
    faction_id = npc->data.person.faction_id;
    handled = func_00031843(28, npc, 0);
    handled |= func_00031843(1, 0, npc);
    handled |= quest_raise_event(28, npc, 0);
    handled |= quest_raise_event(71, npc, 0);
    handled |= quest_raise_event(1, 0, npc);
    if (npc->type == 8 && faction_id != 0) handled |= quest_event_clicked_faction((int)(unsigned short)faction_id);
    if (handled != 0) return;
    if (npc->type == 8 && npc->quest_id == 0) {
        if (((int)(unsigned char)(npc->data.person.flags & 128)) != 0) {
            quest_pick_for_npc(npc);
            npc->data.person.flags &= 127;
            if (*(signed char *)D_001961F5 != 0) return;
        }
    }
    talk_start(npc);
}

void count_items_cb(struct record *object)
{
    if (object->type != 2) return;
    (*(int *)scratch_190be4)++;
}

int object_count_items(struct record *container)
{
    *(int *)scratch_190be4 = 0;
    object_foreach(container->children, count_items_cb);
    return *(int *)scratch_190be4;
}

int repair_menu_open(int opening)
{
    if (((int)(unsigned char)game_mode) == 26) return 1;
    if (opening != 0) {
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        window_image = disk_read_file(D_001762DD, 0);
        game_mode = 26;
        D_00196272 = 1;
        return 1;
    }
    return 0;
}

void repair_menu_frame(void)
{
    struct image *image;
    int i;

    if (repair_menu_open(0) == 0) return;
    image = (struct image *)window_image;
    xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
    if (key_down_esc != 0) repair_menu_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (i = 0; i < 4; i++) {
        if (mouse_x > repair_menu_buttons[i].x0 && mouse_x < repair_menu_buttons[i].x1 && mouse_y > repair_menu_buttons[i].y0 && mouse_y < repair_menu_buttons[i].y1) {
            repair_menu_buttons[i].handler();
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

int coven_menu_open(int opening)
{
    if (((int)(unsigned char)game_mode) == 27) return 1;
    if (opening != 0) {
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        window_image = disk_read_file(D_001762EA, 0);
        game_mode = 27;
        D_00196272 = 1;
        return 1;
    }
    return 0;
}

void coven_menu_frame(void)
{
    struct image *image;
    int i;

    if (coven_menu_open(0) == 0) return;
    image = (struct image *)window_image;
    xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
    if (key_down_esc != 0) coven_menu_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (i = 0; i < 4; i++) {
        if (mouse_x > coven_menu_buttons[i].x0 && mouse_x < coven_menu_buttons[i].x1 && mouse_y > coven_menu_buttons[i].y0 && mouse_y < coven_menu_buttons[i].y1) {
            coven_menu_buttons[i].handler();
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
    daedra_summon(coven_menu_npc);
}

void click_item(struct pick_result *unused, struct record *object)
{
    struct item *item;

    switch (interaction_mode) {
        return;
    case 0:
    case 2:
    case 3:
        if (pick_distance > 128) {
            hud_status_set(D_0017CA14);
        } else {
            pick_up_item(object);
        }
        return;
    case 1:
        item = &object->data.item;
        click_describe_item(item);
    default:;
    }
}

void click_dungeon_model(struct pick_result *hit, struct record *object)
{
    click_show_building_info(hit, object);
}

void click_npc(struct pick_result *unused, struct record *npc)
{
    text_macro_npc = (struct character *)npc_talk_record_build(npc);
    switch (interaction_mode) {
        return;
    case 0:
    case 2:
    case 3:
        if (pick_distance > 256) {
            hud_status_set(D_0017CA14);
        } else {
            npc_click_service(npc);
        }
        return;
    case 1:
        mc_set_location(1021, (int)D_00176198);
        mc_sprintf((int)text_buffer, D_00183254, npc_display_name(npc));
        hud_message_add((int)text_buffer);
    default:;
    }
}

void click_creature(struct pick_result *unused, struct record *monster)
{
    struct character *character;
    struct career *creature_class;

    character = &monster->data.character;
    creature_class = &character->career;
    if ((int)spell_ready_touch != 0 && pick_distance < 160) {
        cast_spell_on(spell_ready_touch, monster, 0);
        object_delete(spell_ready_touch);
        spell_ready_touch = 0;
    }
    switch (interaction_mode) {
        return;
    case 0:
    case 1:
    case 3:
        click_describe_creature(character, creature_class);
        return;
    case 2:
        if (pick_distance > 128) {
            hud_status_set(D_0017CA14);
            return;
        }
        pickpocket_attempt(monster);
    default:;
    }
}

void click_door(struct pick_result *unused, struct record *door)
{
    if (pick_distance > 128) {
        hud_status_set(D_0017CA14);
        return;
    }
    switch (interaction_mode) {
        return;
    case 2:
        door_try_open(door, 1);
        return;
    case 0:
    case 1:
    case 3:
        door_try_open(door, 0);
    default:;
    }
}

void click_marker(struct pick_result *unused, int unused2)
{
}

void click_interior_model(struct pick_result *hit, struct record *object)
{
    struct building *building;
    int stock_count;

    switch (interaction_mode) {
        return;
    case 0:
    case 2:
    case 3:
        if (pick_distance > 128) {
            hud_status_set(D_0017CA14);
            return;
        }
        building = object_building(object);
        if (building != 0 && D_00195D3C != 0 && (D_00195D3C->id == 418 || (D_00195D3C->id == 410 && furniture_is_container(D_00195D3C->variant + (D_00195D3C->id << 7)) != 0)) && (stock_count = func_000612A1()) != 0) {
            if (building->id == player_character->house) {
                D_001940D6 |= 4;
                inventory_open_container((struct record *)D_00196092, 0, 4);
            } else {
                msgbox_yes_no_rsc(37);
                if (((int)D_00196271) != 1) return;
                if (stock_count != 0 && ((int)player_environment) == 2 && rand_range(0, 255) >= (108 - player_character->skills[15].value)) {
                    crime_current = 13;
                    guards_summon(1);
                }
                if (stock_count != 0) {
                    D_001940D6 |= 4;
                    inventory_open_container((struct record *)D_00196120, 0, 4);
                }
            }
        } else if (building != 0 && D_00195D3C != 0 && D_00195D3C->id == 414 && D_00195D3C->variant == 9) {
            ladder_climb();
        }
        return;
    case 1:
        if (building != 0 && D_00195D3C != 0 && D_00195D3C->id == 414 && D_00195D3C->variant == 9) {
            ladder_climb();
            return;
        }
        click_show_building_info(hit, object);
    default:;
    }
}

void click_corpse(struct pick_result *unused, struct record *corpse)
{
    struct character *character;

    switch (interaction_mode) {
        return;
    case 1:
        character = &corpse->data.character;
        if (character->race < 43) {
            mc_set_location(1153, (int)D_00176198);
            mc_sprintf((int)text_buffer, D_00184329, *(int *)(monster_names + (character->race << 2)));
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
        if (object_count_items(corpse) != 0) {
            inventory_open_container(corpse, 0, 1);
            return;
        }
        hud_message_add(D_001845CC);
    default:;
    }
}

void click_pedestrian(struct pick_result *unused, struct record *pedestrian)
{
    switch (interaction_mode) {
        return;
    case 2:
        if (pick_distance > 128) {
            hud_status_set(D_0017CA14);
        } else if (((int)(unsigned short)(pedestrian->npc_flags & 16384)) == 0) {
            pedestrian->npc_flags |= 0x4000;
            pickpocket_attempt(pedestrian);
        }
        return;
    case 0:
    case 1:
    case 3:
        if (pick_distance > 256) {
            hud_status_set(D_0017CA14);
            return;
        }
        npc_talk(pedestrian);
    default:;
    }
}

void click_loot_container(struct pick_result *unused, struct record *container)
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
        if ((container->image >> 7) != 216) if (container->image != 26112) return;
        if (container->children == 0) loot_fill_container(container);
        container_items_to_player(container);
        if (container->children == 0) return;
        inventory_open_container(container, 0, 5);
    default:;
    }
}

void click_town_scenery(struct pick_result *hit, struct record *object)
{
    switch (interaction_mode) {
    return;
case 0:
case 1:
case 2:
case 3:
    click_show_building_info(hit, object);
default:;
}
}

void click_show_building_info(struct pick_result *unused, struct record *object)
{
    struct building *building;
    int unused2;

    if (object->type == 56 && click_hit->model_id == 417 && click_hit->variant == 39) {
        rumor_show_local();
        return;
    }
    if (object->parent->type != 38) return;
    building = object_building(object);
    if (building == 0) return;
    if (building->type == 23) return;
    town_map_note_building(object, building);
    hud_message_add(building_name(building));
    if (building->type == 1 || building_is_open(building) != 0 || building->type >= 14) return;
    mc_set_location(1274, (int)D_00176198);
    mc_sprintf((int)text_buffer, (int)D_001762F7, (int)(unsigned char)building_open_hours[building->type * 2], (int)(unsigned char)D_0017C5B9[building->type * 2]);
    hud_message_add((int)text_buffer);
}

int quest_active_for_faction(short faction_id)
{
    struct record *quest;

    quest = quest_root->children;
    while (quest != 0) {
        if (quest->data.quest.faction_id == faction_id) return 1;
        quest = quest->next;
    }
    return 0;
}

int noble_quest_letter(void)
{
    int count;
    int i;

    count = 0;
    for (i = 0; i < 29; i++) {
        if (regions[(unsigned char)current_region].flags[i] != 0) {
            count++;
        }
    }
    if (count == 0 || rand_range(1, 100) < 10) return 0;
    count = rand_range(0, count - 1);
    for (i = 0; i < 29; i++) {
        if (regions[(unsigned char)current_region].flags[i] != 0) {
            if (count == 0) return i + 65;
            count--;
        }
    }
    return 0;
}

int service_menu_open(int label)
{
    if (((int)(unsigned char)game_mode) == 28) return 1;
    if (label != 0) {
        service_menu_label = label;
        while (mouse_buttons != 0) xn_mouse_poll_clamped();
        window_image = disk_read_file(D_00176322, 0);
        game_mode = 28;
        D_00196272 = 1;
        return 1;
    }
    return 0;
}

void service_menu_frame(void)
{
    struct image *image;
    int i;

    if (service_menu_open(0) == 0) return;
    image = (struct image *)window_image;
    xn_draw_image(image->x, image->y, image->width, image->height, image->pixels);
    text_draw_centred_coloured(service_menu_label, 159, 70, 145, 156);
    if (key_down_esc != 0) service_menu_close();
    if (mouse_buttons == 0 || (mouse_buttons != 0 && mouse_buttons_prev != 0)) {
        return;
    }
    for (i = 0; i < 3; i++) {
        if (mouse_x > service_menu_buttons[i].x0 && mouse_x < service_menu_buttons[i].x1 && mouse_y > service_menu_buttons[i].y0 && mouse_y < service_menu_buttons[i].y1) {
            service_menu_buttons[i].handler();
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

int shelf_shop_stocked(int model_index)
{
    int i;

    for (i = 0; i < stocked_shop_count; i++) {
        if (*(int *)(stocked_shop_ids + (i << 2)) == model_index) return 1;
    }
    return 0;
}

void container_items_to_player(struct record *object)
{
    if (object == 0) return;
    object = object->children;
    while (object != 0) {
        object->x = player_object->x;
        object->y = player_object->y;
        object->z = player_object->z;
        object = object->next;
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
    int sound_index;
    int weather;

    if (xn_math_approx_dist2d(D_001A4FBC, D_001A4FC4, player_object->x, player_object->z) <= 100 || player_on_ground == 0) {
        return;
    }
    D_001A4FBC = player_object->x;
    D_001A4FC4 = player_object->z;
    switch (player_environment) {
    case 1:
        weather = (int)(unsigned char)climate_weathers[climate_category()];
        if ((weather & 127) == 5) {
            sound_index = 6;
        } else {
            sound_index = 4;
        }
        break;
    case 2:
        sound_index = 2;
        break;
    case 3:
        sound_index = collide_floor_plane->floor_sound;
        if (sound_index > 4 || sound_index < 0) sound_index = 0;
        sound_index <<= 1;
        if (dungeon_water_level != 10000 && player_object->y > dungeon_water_level && in_dungeon_water == 0) {
            sound_index = 8;
        }
        if (in_dungeon_water != 0 && ((int)(unsigned char)player_underwater) != 1) {
            sound_index = 10;
        } else {
            if (in_dungeon_water != 0) return;
        }
    }
    if (D_001962A0 != 0) sound_index = 10;
    position_history_record();
    if (((int)(unsigned short)(player_character->flags & 1536)) != 0) return;
    if (((struct bf8_6_1 *)&D_001940D7)->f != 0) sound_index++;
    D_001940D7 ^= 64;
    sound_play((int)(unsigned char)footstep_sound_ids[sound_index], player_object, 100);
}

void position_history_reset(void)
{
    int i;

    D_001A4FCC = (position_history_next = (D_001A4FD4 = (ground_position_history_next = 0)));
    for (i = 0; i < 32; i++) {
        *(int *)(position_history + (i * 12)) = (*(int *)(ground_position_history + (i * 12)) = player_object->x);
        *(int *)(D_001A4CA4 + (i * 12)) = (*(int *)(D_001A4E24 + (i * 12)) = player_object->y);
        *(int *)(D_001A4CA8 + (i * 12)) = (*(int *)(D_001A4E28 + (i * 12)) = player_object->z);
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

int spawn_point_visible(struct record *object)
{
    int bearing;
    int angle_diff;
    int direction;

    if (D_0019629D != 0) return 0;
    if (collide_line_of_sight(player_object, object) == 0) return 0;
    bearing = xn_math_angle_to_point(player_object->x, player_object->z, object->x, object->z);
    angle_diff = ai_angle_diff(object->yaw, bearing, &direction);
    return ((angle_diff < 400) ? 1 : 0);
}

int spawn_point_wilderness(struct record *object)
{
    int yaw;
    int distance;
    int dx;
    int dz;
    int i;

    i = 0;
    if (i < 20) {
        yaw = ((rand() % 90) + ((int)(short)*(short *)((char *)*(int *)&player_object + 3))) - 45;
        yaw &= 2047;
        distance = (rand_range(1, 512) + xn_cam_far_z) - 256;
        xn_math_yaw_offset_xz(yaw, distance, (int)&dx, (int)&dz);
        object->x = player_object->x + dx;
        object->z = player_object->z + dz;
        object->y = xn_terrain_height_at(object->x, object->z);
        return 1;
    }
    for (i = 0; i < 50; i++) {
        xn_math_yaw_offset_xz(rand() % 2048, rand_range(512, 768), (int)&dx, (int)&dz);
        object->x = player_object->x + dx;
        object->z = player_object->z + dz;
        object->y = xn_terrain_height_at(object->x, object->z);
        if (spawn_point_visible(object) == 0) return 1;
    }
    return 0;
}

int spawn_point_town(struct record *object, int min_distance, int max_distance)
{
    int distance;
    int dx;
    int dz;
    int i;
    int slot;

    if (min_distance == 0) {
        min_distance = 128;
        max_distance = 3096;
    }
    for (i = 0; i < 50; i++) {
        xn_math_yaw_offset_xz(rand() % 2048, rand_range(min_distance, max_distance), (int)&dx, (int)&dz);
        object->x = player_object->x + dx;
        object->z = player_object->z + dz;
        object->y = xn_terrain_height_at(object->x, object->z);
        if (pedestrian_spawn_spot_ok(object, object->x - location_object->x, object->z - location_object->z) != 0) return 1;
    }
    if (position_history_next < 20) return 0;
    for (i = 0; i < 20; i++) {
        slot = rand() % 32;
        distance = xn_math_approx_dist2d(player_object->x, player_object->z, *(int *)(position_history + (slot * 12)), *(int *)(D_001A4CA8 + (slot * 12)));
        if (distance > min_distance && distance < max_distance) {
            object->x = *(int *)(position_history + (slot * 12));
            object->y = *(int *)(D_001A4CA4 + (slot * 12));
            object->z = *(int *)(D_001A4CA8 + (slot * 12));
            if (pedestrian_spawn_spot_ok(object, object->x, object->z) != 0) return 1;
        }
    }
    return 0;
}

void spawn_point_occupied_cb(struct record *object)
{
    int distance;

    if ((int)scratch_object == 0) return;
    if (object->type != 18) return;
    distance = xn_math_approx_hypot(scratch_object->y - object->y, xn_math_approx_dist2d(scratch_object->x, scratch_object->z, object->x, object->z));
    if (distance >= 64) return;
    scratch_object = 0;
}

int spawn_point_occupied(struct record *object)
{
    scratch_object = object;
    object_foreach(location_object, spawn_point_occupied_cb);
    return (((int)scratch_object == 0) ? 1 : 0);
}

int spawn_point_building(struct record *object)
{
    int i;
    int index;
    int count;
    struct record *marker;

    count = marker_count(player_object->parent->children, 6);
    if (count == 0) return 0;
    index = rand() % count;
    for (i = 0; i < count; i++) {
        marker = marker_find_nth(player_object->parent->children, 6, index);
        if (abs(player_object->y - marker->y) < 64) {
            if (xn_math_approx_dist2d(player_object->x, player_object->z, marker->x, marker->z) > 80) {
                if (spawn_point_visible(object) == 0) {
                    object->x = marker->x;
                    object->z = marker->z;
                    object->y = marker->y;
                    return 1;
                }
            }
        }
        index = (index + 1) % count;
    }
    for (i = 0; i < count; i++) {
        marker = marker_find_nth(player_object->parent->children, 6, index);
        if (abs(player_object->y - marker->y) > 64) {
            if (xn_math_approx_dist2d(player_object->x, player_object->z, marker->x, marker->z) > 80) {
                if (spawn_point_visible(object) == 0) {
                    object->x = marker->x;
                    object->z = marker->z;
                    object->y = marker->y;
                    return 1;
                }
            }
        }
        index = (index + 1) % count;
    }
    marker = marker_find_nth(player_object->parent->children, 6, index);
    object->x = marker->x;
    object->z = marker->z;
    object->y = marker->y;
    return 1;
}

int spawn_point_dungeon_level(struct record *object, int level, int min_distance, int max_distance)
{
    struct record *cell;
    struct record *grid_object;
    char point[72];
    signed char *grid;
    int unused;
    int distance;
    int best_distance;
    int i;

    if (level < 0 || level > 8) return 0;
    cell = location_cell_at(player_object->x, player_object->z);
    grid_object = object_find_type(cell, 60);
    if (grid_object == 0) return 0;
    grid = (signed char *)RECORD_DATA(grid_object) + (level << 6);
    unused = 0;
    best_distance = max_distance;
    for (i = 0; i < 64; i++) {
        if (*grid++ != 0) {
            ((struct record *)point)->x = (((i % 8) << 8) + grid_object->x) + 128;
            ((struct record *)point)->z = (((i / 8) << 8) + grid_object->z) + 128;
            ((struct record *)point)->y = (-(level << 8)) - 80;
            distance = xn_math_approx_dist2d(player_object->x, player_object->z, ((struct record *)point)->x, ((struct record *)point)->z);
            if (distance < best_distance && distance > min_distance && spawn_point_visible((struct record *)point) == 0) {
                best_distance = distance;
                object->x = ((struct record *)point)->x;
                object->z = ((struct record *)point)->z;
                object->y = ((struct record *)point)->y + 80;
            }
        }
    }
    return object->x;
}

int spawn_point_dungeon(struct record *object, int min_distance, int max_distance)
{
    int level;
    int slot;
    int distance;
    int i;

    level = (-player_object->y) / 256;
    if (spawn_point_dungeon_level(object, level, min_distance, max_distance) != 0 && spawn_point_occupied(object) == 0) return 1;
    for (i = 0; i < 8; i++) {
        if (spawn_point_dungeon_level(object, i, min_distance, max_distance) != 0 && spawn_point_occupied(object) == 0) return 1;
    }
    if (position_history_next < 20) return 0;
    for (i = 0; i < 20; i++) {
        slot = rand() % 32;
        distance = xn_math_approx_dist2d(player_object->x, player_object->z, *(int *)(position_history + (slot * 12)), *(int *)(D_001A4CA8 + (slot * 12)));
        if (distance > min_distance && distance < max_distance) {
            object->x = *(int *)(position_history + (slot * 12));
            object->y = *(int *)(D_001A4CA4 + (slot * 12));
            object->z = *(int *)(D_001A4CA8 + (slot * 12));
            if (spawn_point_fits(object) != 0) return 1;
        }
    }
    object->z = 0;
    object->y = object->z;
    object->x = object->y;
    return 0;
}

void ambient_outdoor_sounds(void)
{
    int bad_weather;
    int in_town;
    int weather;

    if (((int)player_environment) == 1) {
        in_town = location_contains(player_object->x, player_object->z);
        if (in_town != 0 && current_location->kind == 12 && rand() < 30) {
            sound_play(16, player_object, 100);
        }
        if (in_town != 0 && current_location->kind == 12 && rand() < 30) {
            sound_play(383, player_object, 100);
        }
        weather = (int)(unsigned char)climate_weathers[climate_category()];
        if (weather == 4 || weather == 5 || weather == 3 || (weather & 128) != 0) {
            bad_weather = 1;
        } else {
            bad_weather = 0;
        }
        weather = bad_weather;
        if (daylight != 0 && weather == 0 && rand() < 30) {
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
    int song;
    int i;
    short location_id;

    *(int *)&location_id = ((unsigned)location_object->id) >> 16;
    for (i = 0; i < 10; i++) {
        if ((short)*(int *)&location_id == music_special_dungeon_ids[i]) {
            if (music_uses_fm != 0) {
                song = D_001878AC[i];
            } else {
                song = D_001878D4[i];
            }
            return song;
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
