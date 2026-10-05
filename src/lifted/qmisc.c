/* qmisc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00170A08[];
extern char D_00170A11[];
extern char D_00170A1A[];
extern char D_00170A22[];
extern char D_00170A4F[];
extern unsigned char player_environment;
extern signed char item_group_tab[];
extern signed char text_buffer[];
extern signed char scratch_190ce4[];
extern char scratch_190d64[];
extern signed char D_001940D5;
extern signed char quest_global_states[];
extern struct record *inventory_containers[];
extern struct record *D_001959DC;
extern struct record *quest_root;
extern signed char quest_faces[];
extern char quest_faces_quest[];
extern char quest_faces_object[];
extern char quest_faces_image[];
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AA8;
extern struct record *location_object;
extern struct spell *spell_records;
extern int creature_count;
extern struct record *inv_right_container;
extern struct location *current_location;
extern struct character *player_character;
extern int game_minutes;
extern int quest_face_images[];
extern int D_00195D14;
extern char D_001960D9[];
extern signed char current_region;
extern signed char game_mode;
extern signed char is_daytime;
extern signed char D_0019629E;
extern signed char D_001962A8;
extern int region_location_count;
extern int loaded_location_door_count;
extern struct map_location *region_locations;
extern struct record *quest_reward_container;

extern struct faction *faction_find(short);
extern int tavern_open(int);
extern int quest_arg_state(struct qbn_op *, int);
extern void *quest_section(struct quest *, int);
extern void *quest_record(struct quest *, int, int);
extern int location_find_door(int);
extern int flats_cfg_find(int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern int location_contains(int, int);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern struct record *object_find_by_id(struct record *, int);
extern int object_new_id(int);
extern int object_find_quest(struct record *, unsigned char);
extern int strnicmp();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A148C(int, ...);
extern int xn_draw_image_transparent();
extern void maploads_load_region(int);
extern void tavern_close(void);
extern void func_0002FE02(int);
extern void func_0002FE4B(int);
extern void quest_cast_spell_on_foe_cb(int);
extern void quest_show_message(struct quest *, int);
extern void quest_reward_faction(struct quest *);
extern void guild_join_dark_brotherhood(void);
extern void guild_join_thieves_guild(void);
extern void dungeon_load(int);
extern void map_goto_location(int, int, int, int);
extern void object_free_children(int);
extern void object_foreach(struct record *, int);
extern void inv_store_item(struct record *);
struct record *func_000310E1(struct record *, struct record *);
void quest_give_item_to_player(struct record *);
void quest_face_replace_object_id(int, int);
void quest_faces_remove_quest(unsigned char);
void quest_op_done(struct quest *, struct qbn_op *);
#pragma aux mc_set_location parm routine [];

void qaction_op04_give_reward(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_20;
    struct qbn_item *l_1C;
    int l_18;
    int l_14;

    if (strnicmp(a1->name, (int)D_00170A08, 8) == 0) {
        guild_join_dark_brotherhood();
        return;
    }
    if (strnicmp(a1->name, (int)D_00170A11, 8) == 0) {
        guild_join_thieves_guild();
        return;
    }
    if (tavern_open(0) != 0) tavern_close();
    quest_reward_faction(a1);
    if (((int)(unsigned char)game_mode) != 4) {
        object_free_children((*(int *)&quest_reward_container = (int)D_001960D9));
    } else {
        quest_reward_container = inv_right_container;
    }
    for (l_18 = 1; l_18 < 5; l_18++) {
        if (a2->args[l_18].value != (-1)) {
            l_20 = a2->args[l_18].object;
            l_20->flags |= 0x8000;
            if (l_20 != 0) {
                a2->args[l_18].object = 0;
                l_1C = quest_section(a1, 0);
                for (l_14 = 0; a1->section_counts[0] > l_14; l_14++, l_1C++) {
                    if (l_1C->object == l_20) {
                        l_1C->object = 0;
                        break;
                    }
                }
                object_reparent(quest_reward_container, l_20);
                l_20->x = player_object->x;
                l_20->y = player_object->y;
                l_20->z = player_object->z;
            } else {
                mc_set_location(332, (int)D_00170A1A);
                mc_sprintf((int)text_buffer, (int)D_00170A22, (int)a1->name, l_1C->symbol);
                hud_message_add((int)text_buffer);
            }
        }
    }
}

void func_0003077F(struct record *a1, int a2)
{
    struct record *l_18;
    int l_14;

    if (a1 == 0) return;
    l_18 = a1->twin;
    if (loaded_location_door_count != 0 && (a1->id & -65536) == (location_object->id & -65536)) {
        l_14 = location_find_door(a1->id);
        if (l_14 != 0) *(signed char *)((char *)l_14 + 3) &= 15;
    }
    object_delete(a1);
    if (l_18 == 0) return;
    if (a2 != 0) {
        object_delete(l_18);
        return;
    }
    l_18->twin = 0;
    l_18->quest_id = 0;
}

void quest_give_item_to_player(struct record *a1)
{
    struct item *l_20;
    struct record *l_1C;
    struct record *l_18;

    l_20 = &a1->data.item;
    if (l_20->enchantments[0].type != (-1)) {
        l_18 = D_001959DC;
    } else {
        l_18 = inventory_containers[(int)(unsigned char)item_group_tab[l_20->group]];
    }
    l_18 = object_create_child(l_18, 0, 0);
    l_1C = func_000310E1(a1, l_18);
    object_delete(l_18);
    inv_store_item(l_1C);
}

void qaction_op37_repute_exceeds(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_28;
    int l_24;
    struct faction *l_20;
    struct qbn_state *l_1C;
    int l_18;
    short l_14;

    if (a2->args[1].value == (-1)) return;
    l_28 = a2->args[2].object;
    l_1C = (struct qbn_state *)a2->args[1].record;
    if (l_28 == 0) return;
    if (l_28->type == 65) {
        *(int *)&l_14 = (int)(unsigned short)l_28->faction_id;
    } else if (l_28->data.building.faction_id != 0) {
        *(int *)&l_14 = l_28->data.building.faction_id;
    } else {
        return;
    }
    l_20 = faction_find((int)(short)l_14);
    l_24 = l_20->reputation;
    if (l_20->social_group < 5) l_24 += player_character->reputation[l_20->social_group];
    l_18 = ((l_24 >= a2->args[3].value) ? 1 : 0);
    if (l_1C->is_global != 0) {
        quest_global_states[l_1C->value] = *(signed char *)&l_18;
        return;
    }
    l_1C->value = *(signed char *)&l_18;
}

void quest_faces_after_load(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    for (l_24 = 0; l_24 < 10; l_24++) {
        l_1C = 0;
        if (((int)(unsigned char)(quest_faces[l_24 * 10] & 16)) != 0) {
            l_18 = D_00195D14;
        } else {
            l_18 = quest_face_images[(((int)(unsigned char)quest_faces[l_24 * 10]) >> 6)];
        }
        l_20 = (int)(unsigned char)(quest_faces[l_24 * 10] & 15);
        while (l_1C < l_20) {
            l_18 = (((int)(unsigned short)*(short *)((char *)l_18 + 10)) + l_18) + 12;
            l_1C++;
        }
        *(int *)(quest_faces_image + (l_24 * 10)) = l_18;
        if (object_find_quest(quest_root->children, (int)(unsigned char)*(signed char *)(quest_faces_quest + (l_24 * 10))) == 0) {
            quest_faces_remove_quest((int)(unsigned char)*(signed char *)(quest_faces_quest + (l_24 * 10)));
        }
    }
}

void quest_faces_draw(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (game_mode != 0) return;
    l_20 = 0;
    l_1C = l_20;
    for (; l_20 < 10; l_20++) {
        if (*(int *)(quest_faces_object + (l_20 * 10)) == 0) continue;
        l_18 = *(int *)(quest_faces_image + (l_20 * 10));
        xn_draw_image_transparent((l_1C << 5) + 8, 36, (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
        l_1C++;
    }
}

void quest_face_remove(int a1)
{
    int l_18;

    for (l_18 = 0; l_18 < 10; l_18++) {
        if (*(int *)(quest_faces_object + (l_18 * 10)) == a1) {
            *(int *)(quest_faces_object + (l_18 * 10)) = 0;
            return;
        }
    }
}

void quest_face_replace_object_id(int a1, int a2)
{
    int l_14;

    for (l_14 = 0; l_14 < 10; l_14++) {
        if (*(int *)(quest_faces_object + (l_14 * 10)) == a1) {
            *(int *)(quest_faces_object + (l_14 * 10)) = a2;
            return;
        }
    }
}

void quest_faces_remove_quest(unsigned char a1)
{
    {
        int l_1C;

        for (l_1C = 0; l_1C < 10; l_1C++) {
            if (*(int *)(quest_faces_object + (l_1C * 10)) == 0) continue;
            if (*(unsigned char *)(quest_faces_quest + (l_1C * 10)) == a1) {
                *(int *)(quest_faces_object + (l_1C * 10)) = 0;
            }
        }
    }
}

int qcond_op57_item_used(struct quest *a1, struct qbn_op *a2)
{
    struct item *l_1C;
    struct qbn_state *l_18;

    l_1C = &a2->args[2].object->twin->data.item;
    if (a2->args[2].object->twin == 0) return 0;
    if (((int)(unsigned short)(l_1C->item_flags & 512)) != 0) {
        if (a2->args[1].value == (-1)) return 1;
        l_18 = (struct qbn_state *)a2->args[1].record;
        if (l_18->is_global != 0) {
            quest_global_states[l_18->value] = 1;
        } else {
            l_18->value = 1;
        }
        return 1;
    }
    return 0;
}

void quest_items_release_on_close(void)
{
    object_foreach(player_entity->children, (int)func_0002FE02);
}

void func_00030F63(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_foe *l_1C;
    struct record *l_18;
    int l_14;

    l_1C = quest_record(a1, 7, (short)a2->args[1].value);
    l_18 = a2->args[1].object;
    *(short *)scratch_190d64 = l_18->image2;
    l_14 = quest_arg_state(a2, 2);
    scratch_190ce4[0] = *(signed char *)&l_14;
    if (l_14 != 0) {
        l_18->data.character.flags |= 0x8000;
    } else {
        l_18->data.character.flags &= ~0x8000;
    }
    object_foreach(location_object, (int)func_0002FE4B);
}

void qaction_op69_cast_spell_on_foe(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    struct record *l_1C;
    int l_18;
    struct record *l_14;

    l_1C = a2->args[1].object;
    *(short *)scratch_190d64 = l_1C->image2;
    l_18 = 0;
    while (spell_records[l_18].name[0] == 0 || spell_records[l_18].id != a2->args[2].value) l_18++;
    l_14 = object_create_child(player_object->parent, 0, 89);
    D_00195AA8 = l_14;
    l_14->type = 9;
    l_14->caster = player_entity;
    l_14->id = object_new_id(((unsigned)location_object->id) >> 16);
    mc_memcpy(&l_14->data.spell, &spell_records[l_18], 89, (int)D_00170A1A, 679, 4);
    object_foreach(location_object, (int)quest_cast_spell_on_foe_cb);
    object_delete(l_14);
}

struct record *func_000310E1(struct record *a1, struct record *a2)
{
    struct record *l_34;
    struct record *l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_34 = 0;
    if (a2 == 0) {
        if (a1->parent->type == 39) return 0;
        a2 = a1->parent;
        a2 = object_find_by_id(location_object, a2->id);
        if (a2 == 0) return 0;
    }
    l_18 = a1->id;
    switch (a1->type) {
    case 2:
    case 18:
        l_1C = *(int *)((char *)a1 - 6) - 71;
        l_34 = object_create_child(a2->parent, 0, l_1C);
        l_34->x = a2->x;
        l_34->y = a2->y;
        l_34->z = a2->z;
        mc_memcpy(&l_34->data, &a1->data, l_1C, (int)D_00170A1A, 734, 4);
        l_34->type = a1->type;
        l_34->flags = a1->flags;
        l_34->image = a1->image;
        l_34->image2 = a1->image2;
        a1->id = object_new_id(((unsigned)location_object->id) >> 16);
        l_34->id = a1->id;
        l_34->quest_id = a1->quest_id;
        quest_face_replace_object_id(l_18, a1->id);
        l_34->twin = a1;
        a1->twin = l_34;
        a1 = a1->children;
        while (a1 != 0) {
            l_30 = object_create_child(l_34, 0, 107);
            l_30->x = a2->x;
            l_30->y = a2->y;
            l_30->z = a2->z;
            mc_memcpy(&l_30->data, &a1->data, 107, (int)D_00170A1A, 754, 4);
            l_30->type = a1->type;
            l_30->flags = a1->flags;
            l_30->image = a1->image;
            l_30->image2 = a1->image2;
            a1->id = object_new_id(((unsigned)location_object->id) >> 16);
            l_30->id = a1->id;
            l_30->quest_id = a1->quest_id;
            l_30->twin = a1;
            a1->twin = l_30;
            a1 = a1->next;
        }
        break;
    case 41:
        switch (a2->type) {
        case 34:
            l_34 = object_create_child(a2->parent, 0, 3);
            l_34->type = 8;
            l_34->x = a2->x;
            l_34->y = a2->y;
            l_34->z = a2->z;
            a1->id = object_new_id(((unsigned)location_object->id) >> 16);
            l_34->id = a1->id;
            quest_face_replace_object_id(l_18, a1->id);
            break;
        case 8:
            l_34 = a2;
            l_34->data.person.flags |= 128;
            break;
        case 40:
            a2 = object_find_by_id(location_object, a2->id);
            l_34 = object_create_child(a2->parent, 0, 3);
            l_34->type = 8;
            l_34->x = a2->x;
            l_34->y = a2->y;
            l_34->z = a2->z;
            a1->id = object_new_id(((unsigned)location_object->id) >> 16);
            l_34->id = a1->id;
            quest_face_replace_object_id(l_18, a1->id);
            break;
        default:
            return 0;
        }
        l_34->quest_id = a1->quest_id;
        l_34->name_seed = a1->name_seed;
        l_2C = (int)RECORD_DATA(l_34);
        if (((int)(unsigned char)(*(signed char *)((char *)l_2C + 2) & 128)) == 0) {
            *(short *)((char *)l_2C) = a1->data.building.faction_id;
        }
        if (*(short *)((char *)l_2C) == 0) {
            mc_set_location(819, (int)D_00170A1A);
            func_000A148C((int)D_00170A4F, 819);
            *(short *)((char *)l_2C) = 510;
        }
        if (a1->image != 0) {
            l_34->image = a1->image;
        } else {
            l_34->image = faction_find(*(short *)((char *)l_2C))->flats[0];
        }
        l_24 = flats_cfg_find(l_34->image);
        *(unsigned char *)((char *)l_2C + 2) |= ((((int)(unsigned char)(*(signed char *)((char *)l_24 + 6) & 1)) != 0) ? 16 : 0);
        if (((int)(unsigned short)(a1->flags & 4)) != 0) {
            l_34->flags |= 4;
        } else {
            l_34->flags &= ~0x4;
        }
        l_34->twin = a1;
        a1->twin = l_34;
        break;
    case 40:
        l_34 = object_find_by_id(location_object, a1->id);
        if (l_34 == 0) return 0;
        l_34->quest_id = a1->quest_id;
        l_34->twin = a1;
        a1->twin = l_34;
    }
    if (l_34 != 0 && ((int)(unsigned short)(a1->flags & 2048)) != 0) l_34->flags |= 0xA00;
    return l_34;
}

void func_00031658(struct quest *a1, struct qbn_op *a2, int a3)
{
    a2->flags |= 2;
    if (a3 == 0) {
        if (D_0019629E != 0) return;
        if (creature_count != 0) return;
        if (D_001962A8 != 0) return;
        if (game_mode != 0) return;
        if (player_character->race == 8 && is_daytime != 0) return;
        if (player_character->race != 8 && is_daytime == 0) return;
        if (((int)player_environment) != 1 || location_contains(player_object->x, player_object->z) == 0) {
            return;
        }
        if (current_location->kind == 4 || current_location->kind == 7 || current_location->kind > 9) {
            return;
        }
        if ((game_minutes - a2->last_minutes) == 0) return;
        if (rand_range(1, 100) >= 10) goto L3179A;
    }
    a2->flags |= 4;
    quest_give_item_to_player(a2->args[1].object);
    quest_op_done(a1, a2);
L3179A:;
    a2->last_minutes = game_minutes;
}

int quest_deliveries_done(struct quest *a1)
{
    int l_20;
    struct qbn_op *l_1C;

    l_1C = quest_section(a1, 8);
    for (l_20 = 0; a1->section_counts[8] > l_20; l_20++, l_1C++) {
        if (l_1C->opcode == 76 && ((int)(short)(l_1C->flags & 2)) != 0 && ((int)(short)(l_1C->flags & 4)) == 0) {
            return 0;
        }
    }
    return 1;
}

void qaction_op83_teleport_pc(struct qbn_op *a1)
{
    struct record *l_30;
    struct map_location *l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_18 = (int)(unsigned char)current_region;
    l_28 = (int)a1->args[1].record;
    l_1C = a1->args[2].value;
    l_20 = a1->args[3].value;
    maploads_load_region(l_1C);
    l_2C = region_locations;
    for (l_24 = 0; l_24 < region_location_count; l_24++, l_2C++) {
        if ((l_2C->map_id & 1048575) == l_20) {
            maploads_load_region(l_18);
            map_goto_location(l_1C, 1, l_24, 0);
            dungeon_load(-1);
            l_30 = a1->args[1].object->twin;
            if (l_30 != 0) {
                player_object->x = l_30->x;
                player_object->y = l_30->y;
                player_object->z = l_30->z;
                player_object->yaw = camera_object->yaw;
                D_001940D5 |= 2;
            }
            a1->flags |= 1;
            return;
        }
    }
}

void quest_op_done(struct quest *a1, struct qbn_op *a2)
{
    if (((int)(short)(a2->flags & 1)) != 0) return;
    a2->flags |= 1;
    quest_show_message(a1, a2->message);
}
