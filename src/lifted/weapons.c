/* weapons.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"
#include "clib.h"

extern int pick_distance;
extern signed char mouse_buttons;
extern int xn_anim_ticks;
extern char D_0017615C[];
extern char D_00176166[];
extern char D_00176175[];
extern char D_00176184[];
extern unsigned char player_environment;
extern iptr D_00185097;
extern signed char weapon_cif_by_index[];
extern signed char weapon_swing_types[];
extern short swing_to_hit_mods[];
extern short swing_damage_mods[];
extern char D_001875AE[];
extern char D_001875B7[];
extern signed char text_buffer[];
extern struct record *creature_list[];
extern signed char D_001940D6;
extern int D_0019597C[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern int frame_ticks;
extern struct record *location_object;
extern struct pick_result *click_hit;
extern int creature_count;
extern struct record *spell_ready_missile;
extern struct image *hud_bar_image;
extern char D_00195B84[];
extern struct character *player_character;
extern struct settings *game_settings;
extern struct record *D_00195C48;
extern int spell_cast_queue_count;
extern short mouse_motion_x;
extern short mouse_motion_y;
extern signed char weapon_active_hand;
extern signed char mouse_buttons_prev;
extern signed char crime_current;
extern struct loaded_location loaded_location;
extern char collide_flags[];
extern iptr D_001A4A30[];
extern int D_001A4A38[];
extern int D_001A4A48[];
extern iptr weapon_hand_cif[];
extern iptr D_001A4A5C;
extern int D_001A4A60[];
extern iptr D_001A4A68[];
extern int D_001A4A70[];
extern short swing_to_hit;
extern short swing_damage;

extern int lockpick_door(struct record *);
extern struct record *door_find_key(struct record *, int, unsigned short);
extern int collide_move_missile(struct record *, int *, int *);
extern int people_check_witnesses(void);
extern int key_action_held(int);
extern int building_is_open(struct building *);
extern int sound_play(int, struct record *, int);
extern iptr disk_read_file(char *, iptr);
extern int click_world_face(struct pick_result *);
extern iptr hud_message_add(iptr);
extern int rand_range(int, int);
extern struct record *object_free_single(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int inv_take_arrow(int);
extern int door_start_swing(struct record *, int);
extern void *xn_vec_unit_direction(void *, void *, void *);
extern void *xn_vec_advance(void *, int, void *);
extern int func_000C2068(void *);
extern int xn_math_approx_dist2d(int, int, int, int);
extern int xn_math_approx_hypot(int, int);
extern int xn_math_angle_to_point(int, int, int, int);
extern void xn_draw_cif_rle_frame(void *, int, int, int);
extern void xn_math_advance_pitch_yaw(int, int, int, void *);
extern void lock_show_difficulty(int);
extern void damage_resolve_attack(struct record *, struct record *, int);
extern void guards_summon(int);
extern void cast_fire_missile(struct record *);
extern void spell_area_effect(struct record *);
extern void item_make(int, int, struct item *);
extern void links_trigger(struct record *, int);
extern void mem_check_crt_heap(int);
extern void fatigue_add(int);
extern void weapon_reload_hand_sprites(void);
extern void click_item(struct pick_result *, struct record *);
extern void click_dungeon_model(struct pick_result *, struct record *);
extern void click_npc(struct pick_result *, struct record *);
extern void click_creature(struct pick_result *, struct record *);
extern void click_door(struct pick_result *, struct record *);
extern void click_marker(struct pick_result *, struct record *);
extern void click_interior_model(struct pick_result *, struct record *);
extern void click_corpse(struct pick_result *, struct record *);
extern void click_pedestrian(struct pick_result *, struct record *);
extern void click_loot_container(struct pick_result *, struct record *);
extern void click_town_scenery(struct pick_result *, struct record *);
extern void object_free_later(struct record *);
extern void spell_cast_queued_run(void);
extern void inv_merge_arrows(struct record *, struct record *, int);
struct record *monster_nearest_to_point(int, int, int);
void weapon_fire_arrow(void);
#pragma aux mc_set_location parm routine [];

void weapon_load_hand_sprite(struct record *item, int hand)
{
    struct item *item_data;
    int cif_index;

    if (item == 0 || player_character->race > 8) {
L72941:;
        *(iptr *)&D_001A4A68[hand] = 0;
        cif_index = ((player_character->race < 9) ? 10 : 11);
        item = 0;
    } else {
        item_data = &item->data.item;
        if (item_data->group != 3) goto L72941;
        cif_index = (int)(unsigned char)weapon_cif_by_index[item_data->index];
        if (cif_index == 9) {
            *(iptr *)&D_001A4A68[hand] = (iptr)D_001875B7;
        } else {
            *(iptr *)&D_001A4A68[hand] = 0;
        }
        D_001A4A60[hand] = 0;
    }
    D_001A4A30[hand] = (iptr)item;
    if (item != 0 && item_data->enchantments[0].type != (-1) && item_data->index != 17 && item_data->index != 16) {
        mc_set_location(85, D_0017615C);
        mc_sprintf((char *)text_buffer, D_00176166, cif_index);
    } else {
        mc_set_location(87, D_0017615C);
        mc_sprintf((char *)text_buffer, D_00176175, cif_index);
    }
    weapon_hand_cif[hand] = disk_read_file(text_buffer, 0);
}

void weapon_reload_sprites(void)
{
    weapon_reload_hand_sprites();
    mem_check_crt_heap(704);
}

int weapon_start_swing(int hand)
{
    short dx;
    int motion;
    short dy;

    if (key_action_held(33) == 0) return 0;
    dx = mouse_motion_x;
    dy = mouse_motion_y;
    motion = xn_math_approx_dist2d(0, 0, (int)(short)dx, (int)(short)dy);
    if (motion < 45) return 0;
    D_001A4A48[hand] = xn_anim_ticks;
    motion = xn_math_angle_to_point(0, 0, (int)(short)dx, (int)(short)dy) >> 7;
    D_001A4A38[hand] = (int)(unsigned char)weapon_swing_types[motion];
    swing_to_hit = swing_to_hit_mods[motion];
    swing_damage = swing_damage_mods[motion];
    if (((int)(unsigned char)weapon_swing_types[motion]) == 2 && player_character->equipped[((int)(unsigned char)weapon_active_hand) * 2 + 19] == 0) {
        *(iptr *)&D_001A4A68[((int)(unsigned char)weapon_active_hand)] = (iptr)D_001875AE;
        D_001A4A60[((int)(unsigned char)weapon_active_hand)] = 0;
    } else {
        *(iptr *)&D_001A4A68[((int)(unsigned char)weapon_active_hand)] = 0;
    }
    D_001A4A70[hand] = 5;
    return 1;
}

void weapon_bow_update(void)
{
    if (((struct bf8_6_1 *)&D_001940D6)->f == 0) return;
    if (D_0019597C[((int)(unsigned char)weapon_active_hand)] != 0) return;
    if (D_001A4A60[((int)(unsigned char)weapon_active_hand)] > 200) {
        D_001A4A60[((int)(unsigned char)weapon_active_hand)] -= frame_ticks;
        if (D_001A4A60[((int)(unsigned char)weapon_active_hand)] < 200) {
            D_001A4A60[((int)(unsigned char)weapon_active_hand)] = 0;
        }
        return;
    }
    if (D_001A4A60[((int)(unsigned char)weapon_active_hand)] == 200) {
        D_001A4A60[((int)(unsigned char)weapon_active_hand)] = 0;
    }
    if (((int)(unsigned char)*(signed char *)((*(char * *)&D_001A4A68[((int)(unsigned char)weapon_active_hand)] + D_001A4A60[((int)(unsigned char)weapon_active_hand)]))) == 3) {
        if (((int)(unsigned char)(mouse_buttons_prev & 2)) == 0 && ((int)(unsigned char)(mouse_buttons & 2)) != 0) {
            sound_play(6, player_object, 100);
            (D_001A4A60[((int)(unsigned char)weapon_active_hand)])++;
        }
    } else {
        (D_001A4A60[((int)(unsigned char)weapon_active_hand)])++;
        if (((int)(unsigned char)*(signed char *)((char *)(*(iptr *)&D_001A4A68[((int)(unsigned char)weapon_active_hand)] + D_001A4A60[((int)(unsigned char)weapon_active_hand)]))) == 5) {
            weapon_fire_arrow();
            fatigue_add(-11);
        }
        if (((int)(unsigned char)*(signed char *)((*(char * *)&D_001A4A68[((int)(unsigned char)weapon_active_hand)] + D_001A4A60[((int)(unsigned char)weapon_active_hand)]))) == 7) {
            D_001A4A60[((int)(unsigned char)weapon_active_hand)] = ((100 - player_character->attributes[6]) * 10) + 1000;
            return;
        }
    }
    xn_draw_cif_rle_frame((void *)weapon_hand_cif[((int)(unsigned char)weapon_active_hand)], (int)(unsigned char)*(signed char *)((char *)(*(iptr *)&D_001A4A68[((int)(unsigned char)weapon_active_hand)] + D_001A4A60[((int)(unsigned char)weapon_active_hand)])), ((((int)(unsigned short)(game_settings->view_flags & 1)) != 0) ? 0 : -hud_bar_image->height), (int)(unsigned char)weapon_active_hand);
}

void weapon_fire_arrow(void)
{
    int aim[3];
    struct record *arrow;

    if (inv_take_arrow(1) == 0) {
        hud_message_add((iptr)D_00176184);
        return;
    }
    arrow = object_create_child(player_object->parent, 0, 107);
    arrow->type = 2;
    arrow->image2 = 998;
    arrow->image = 0;
    item_make(3, 18, &arrow->data.item);
    arrow->data.item.stack_count = 1;
    mc_memset(aim, 0, 12, D_0017615C, 404, 4);
    xn_math_advance_pitch_yaw(player_object->angle_x, player_object->yaw, 1024, aim);
    aim[0] += player_object->x;
    aim[1] += player_object->y;
    aim[2] += player_object->z;
    xn_vec_unit_direction((void *)(iptr)((iptr)player_object + 7), aim, arrow->data.item.arrow.direction);
    arrow->x = player_object->x;
    arrow->y = player_object->y - 70;
    if (((int)(unsigned short)(game_settings->view_flags & 1)) == 0) {
        arrow->y -= 10;
    }
    arrow->z = player_object->z;
    arrow->from_player = 1;
    xn_vec_advance(arrow->data.item.arrow.direction, 160, &arrow->x);
}

void weapon_missile_orient(struct record *arrow)
{
{
    int angles[3];

    mc_memcpy(angles, arrow->data.item.arrow.direction, 12, D_0017615C, 426, 12);
    func_000C2068(angles);
    arrow->missile_yaw = (short)angles[0] & 2047;
    arrow->angle_z = (short)angles[1] & 2047;
}
}

int weapon_arrow_update(struct record *arrow)
{
    int hit_flags;
    int dist;
    struct record *monster;
    {
        int angles[3];
        int dest[3];

        dist = xn_math_approx_hypot(player_object->y - arrow->y, xn_math_approx_dist2d(player_object->x, player_object->z, arrow->x, arrow->z));
        if (dist > 2048) {
            object_free_later(arrow);
            return 0;
        }
        dest[0] = arrow->x;
        dest[1] = arrow->y;
        dest[2] = arrow->z;
        xn_vec_advance(arrow->data.item.arrow.direction, 40, dest);
        angles[0] = arrow->angle_x;
        angles[1] = arrow->yaw;
        angles[2] = 0;
        *(signed char *)collide_flags |= 4;
        hit_flags = collide_move_missile(arrow, dest, angles);
        if (arrow->from_player == 0) {
            dist = xn_math_approx_hypot(player_object->y - arrow->y, xn_math_approx_dist2d(player_object->x, player_object->z, arrow->x, arrow->z));
            if (dist < 125) {
                sound_play(7, player_object, 100);
                damage_resolve_attack(player_entity, player_entity, 19);
                inv_merge_arrows(player_entity, arrow, 0);
                object_free_later(arrow);
                return 0;
            }
            if ((hit_flags & 10) != 0) {
                object_free_later(arrow);
                return 0;
            }
        } else {
            if ((hit_flags & 2) != 0) {
                links_trigger(D_00195C48, 5);
                object_free_later(arrow);
                return 0;
            }
            if ((hit_flags & 8) != 0 && D_00195C48->type == 18) {
                sound_play(7, D_00195C48, 100);
                damage_resolve_attack(player_entity, D_00195C48, 19);
                inv_merge_arrows(D_00195C48, arrow, 0);
                object_free_later(arrow);
                return 0;
            }
            monster = monster_nearest_to_point(arrow->x, arrow->y, arrow->z);
            if (monster == 0) {
                object_free_later(arrow);
                return 0;
            }
            if (*(int *)D_00195B84 > 140) return 1;
            sound_play(7, monster, 100);
            damage_resolve_attack(player_entity, monster, 19);
            inv_merge_arrows(monster, arrow, 0);
            object_free_later(arrow);
            return 0;
        }
        return 1;
    }
}

void weapon_monster_arrow(struct record *shooter, struct record *target)
{
    struct record *arrow;

    arrow = object_create_child(location_object, 0, 107);
    arrow->type = 2;
    arrow->image2 = 998;
    arrow->image = 0;
    item_make(3, 18, &arrow->data.item);
    arrow->data.item.stack_count = 1;
    xn_vec_unit_direction(&shooter->x, &target->x, arrow->data.item.arrow.direction);
    arrow->missile_yaw = 0;
    arrow->angle_z = 0;
    arrow->x = shooter->x;
    arrow->y = shooter->y - 60;
    arrow->z = shooter->z;
    arrow->from_player = 0;
    arrow->monster_arrow = 1;
    xn_vec_advance(arrow->data.item.arrow.direction, 160, &arrow->x);
}

void weapon_free_sprites(void)
{
    if (weapon_hand_cif[0] != 0 && weapon_hand_cif[0] != (-1751672937)) {
        mc_free((void *)weapon_hand_cif[0], D_0017615C, 542);
        weapon_hand_cif[0] = -1751672937;
    }
    if (D_001A4A5C == 0 || D_001A4A5C == (-1751672937)) return;
    mc_free((void *)D_001A4A5C, D_0017615C, 543);
    D_001A4A5C = -1751672937;
}

struct record *monster_nearest_to_point(int x, int y, int z)
{
    int i;
    int best_dist;
    int best_index;
    int dist;

    best_dist = 100000;
    for (i = 0; i < creature_count; i++) {
        dist = xn_math_approx_hypot(creature_list[i]->y - y, xn_math_approx_dist2d(creature_list[i]->x, creature_list[i]->z, x, z));
        if (dist < best_dist) {
            best_dist = dist;
            best_index = i;
        }
    }
    *(int *)D_00195B84 = best_dist;
    if (best_dist == 100000) return 0;
    return creature_list[best_index];
}

void click_world_object(struct pick_result *pick, struct record *object)
{
    int location_index;

    location_index = loaded_location.index;
    click_hit = pick;
    if ((iptr)spell_ready_missile != 0) {
        if (spell_ready_missile->data.spell.target == 3) {
            spell_cast_queue_count = 0;
            spell_ready_missile->x = player_object->x;
            spell_ready_missile->y = player_object->y;
            spell_ready_missile->z = player_object->z;
            spell_area_effect(spell_ready_missile);
            spell_cast_queued_run();
        } else {
            cast_fire_missile(spell_ready_missile);
        }
        spell_ready_missile = 0;
        return;
    }
    if (pick_distance <= 128) links_trigger(object, 2);
    if (pick_distance <= 128 && click_world_face(pick) != 0) return;
    if (location_index != loaded_location.index) return;
    switch (object->type) {
        return;
    case 2:
        click_item(pick, object);
        return;
    case 6:
        click_dungeon_model(pick, object);
        return;
    case 8:
        click_npc(pick, object);
        return;
    case 18:
        click_creature(pick, object);
        return;
    case 32:
        click_door(pick, object);
        return;
    case 34:
        click_marker(pick, object);
        return;
    case 43:
        click_interior_model(pick, object);
        return;
    case 44:
        click_corpse(pick, object);
        return;
    case 53:
        click_pedestrian(pick, object);
        return;
    case 33:
        click_loot_container(pick, object);
        return;
    case 56:
        click_town_scenery(pick, object);
    default:;
    }
}

void door_try_open(struct record *door, int lockpick)
{
    int unused;
    int chance;
    struct record *key;

    if (((int)(unsigned short)(door->flags & 256)) != 0) {
        if (door_start_swing(door, 1) != 0) {
            door->flags &= ~0x100;
            if ((player_character->conditions & 0x20) != 0 && rand_range(1, 100) <= ((int)(unsigned char)(signed char)player_character->lock_open_chance)) {
                door->flags &= ~0x40;
                door->lock_level = player_character->level;
            }
        }
        return;
    }
    if (door->lock_level == 0 || ((int)(unsigned short)(door->flags & 64)) != 0) {
        if ((player_character->conditions & 0x20) != 0 && rand_range(1, 100) <= ((int)(unsigned char)(signed char)player_character->lock_open_chance)) {
            door->flags &= ~0x40;
            door->lock_level = player_character->level;
        } else if (door_start_swing(door, 0) != 0) {
            door->flags |= 0x100;
        }
        return;
    }
    if (((int)current_building->id == player_character->house || (((int)player_environment) == 2 && building_is_open(current_building) != 0)) && door_start_swing(door, 0) != 0) {
        door->flags |= 0x100;
        return;
    }
    key = door_find_key(player_entity, ((unsigned)door->id) >> 16, door->lock_level);
    if (key == 0) {
        if (lockpick == 0) {
            if ((player_character->conditions & 0x40) != 0) {
                chance = (int)(unsigned char)(signed char)player_character->lock_open_chance;
                player_character->conditions &= ~0x40;
                if (rand_range(1, 100) <= chance && door_start_swing(door, 0) != 0) {
                    door->flags |= 320;
                } else {
                    hud_message_add(D_00185097);
                }
                return;
            }
            lock_show_difficulty(door->lock_level);
        } else {
            if (lockpick_door(door) != 0) {
                if (door_start_swing(door, 0) != 0) door->flags |= 320;
            }
            if (people_check_witnesses() != 0 || rand_range(1, 300) < (100 - ((int)(short)player_character->skills[16].value))) {
                crime_current = 1;
                guards_summon(1);
            }
        }
        return;
    }
    if (door_start_swing(door, 0) == 0) return;
    object_free_single(key);
    door->flags |= 320;
}
