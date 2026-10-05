/* qmisc.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char D_00170A08[];
extern char D_00170A11[];
extern char D_00170A1A[];
extern char D_00170A22[];
extern char D_00170A4F[];
extern char player_environment[];
extern char item_group_tab[];
extern char text_buffer[];
extern char itemmaker_slot_kinds[];
extern char D_00190D64[];
extern char D_001940D5[];
extern char quest_global_states[];
extern struct record *inventory_containers[];
extern char D_001959DC[];
extern struct record *D_00195A00;
extern char quest_faces[];
extern char D_00195A15[];
extern char D_00195A16[];
extern char D_00195A1A[];
extern struct record *camera_object;
extern struct record *player_entity;
extern struct record *player_object;
extern struct record *D_00195AA8;
extern struct record *D_00195AC4;
extern struct spell *spell_records;
extern char creature_count[];
extern struct record *inv_right_container;
extern struct location *current_location;
extern struct character *player_character;
extern char game_minutes[];
extern char D_00195D04[];
extern char D_00195D14[];
extern char D_001960D9[];
extern char current_region[];
extern char game_mode[];
extern char D_00196280[];
extern char D_0019629E[];
extern char D_001962A8[];
extern char D_00196A28[];
extern char loaded_location_door_count[];
extern char D_00196A9C[];
extern struct record *D_00199768;

extern struct faction *faction_find(short);
extern int tavern_open(int);
extern int quest_arg_state(struct qbn_op *, int);
extern void *quest_section(struct quest *, int);
extern void *quest_record(struct quest *, int, int);
extern int func_00045E45(int);
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
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int func_000A148C(int, ...);
extern int func_00144FB4();
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
void func_00030DE1(int, int);
void quest_faces_remove_quest(unsigned char);
void quest_op_done(struct quest *, struct qbn_op *);
#pragma aux func_000A0ED9 parm routine [];

void qaction_op04_give_reward(struct quest *a1, struct qbn_op *a2)
{
    struct record *l_20;
    struct qbn_item *l_1C;
    int l_18;
    int l_14;

    if (strnicmp(a1->name, (int)D_00170A08, 8) != 0) goto L30574;
    guild_join_dark_brotherhood();
    return;
L30574:;
    if (strnicmp(a1->name, (int)D_00170A11, 8) != 0) goto L30597;
    guild_join_thieves_guild();
    return;
L30597:;
    if (tavern_open(0) == 0) goto L305A7;
    tavern_close();
L305A7:;
    quest_reward_faction(a1);
    if (((int)(unsigned char)*(signed char *)game_mode) == 4) goto L305D1;
    object_free_children((*(int *)&D_00199768 = (int)D_001960D9));
    goto L305DB;
L305D1:;
    D_00199768 = inv_right_container;
L305DB:;
    l_18 = 1;
L305E2:;
    if (l_18 < 5) goto L305F5;
    return;
L305ED:;
    l_18++;
    goto L305E2;
L305F5:;
    if (a2->args[l_18].value == (-1)) goto L306F0;
    l_20 = a2->args[l_18].object;
    l_20->flags |= 0x8000;
    if (l_20 == 0) goto L306B5;
    a2->args[l_18].object = 0;
    l_1C = quest_section(a1, 0);
    l_14 = 0;
L30646:;
    if (a1->section_counts[0] > l_14) goto L30663;
    goto L3067C;
L30654:;
    l_14++;
    l_1C++;
    goto L30646;
L30663:;
    if (l_1C->object != l_20) goto L3067A;
    l_1C->object = 0;
    goto L3067C;
L3067A:;
    goto L30654;
L3067C:;
    object_reparent(D_00199768, l_20);
    l_20->x = player_object->x;
    l_20->y = player_object->y;
    l_20->z = player_object->z;
    goto L306F0;
L306B5:;
    func_000A0ED9(332, (int)D_00170A1A);
    mc_sprintf((int)text_buffer, (int)D_00170A22, (int)a1->name, *(int *)((char *)l_1C + 7));
    hud_message_add((int)text_buffer);
L306F0:;
    goto L305ED;
}

void func_0003077F(struct record *a1, int a2)
{
    struct record *l_18;
    int l_14;

    if (a1 == 0) return;
    l_18 = a1->twin;
    if (*(int *)loaded_location_door_count == 0) goto L307C6;
    if ((a1->id & -65536) == (D_00195AC4->id & -65536)) goto L307C8;
L307C6:;
    goto L307E3;
L307C8:;
    l_14 = func_00045E45(a1->id);
    if (l_14 == 0) goto L307E3;
    *(signed char *)((char *)l_14 + 3) &= 15;
L307E3:;
    object_delete(a1);
    if (l_18 == 0) return;
    if (a2 == 0) goto L30801;
    object_delete(l_18);
    return;
L30801:;
    l_18->twin = 0;
    l_18->quest_id = 0;
}

void quest_give_item_to_player(struct record *a1)
{
    struct item *l_20;
    struct record *l_1C;
    struct record *l_18;

    l_20 = &a1->data.item;
    if (l_20->enchantments[0].type == (-1)) goto L30884;
    l_18 = (struct record *)*(int *)D_001959DC;
    goto L308A7;
L30884:;
    l_18 = inventory_containers[(int)(unsigned char)*(signed char *)(item_group_tab + l_20->group)];
L308A7:;
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
    if (l_28->type != 65) goto L30937;
    *(int *)&l_14 = (int)(unsigned short)*(short *)((char *)l_28 + 25);
    goto L30954;
L30937:;
    if (*(short *)((char *)l_28 + 89) == 0) goto L3094F;
    *(int *)&l_14 = (int)(unsigned short)*(short *)((char *)l_28 + 89);
    goto L30954;
L3094F:;
    return;
L30954:;
    l_20 = faction_find((int)(short)l_14);
    l_24 = l_20->reputation;
    if (l_20->social_group >= 5) goto L30999;
    l_24 += player_character->reputation[l_20->social_group];
L30999:;
    l_18 = ((l_24 >= a2->args[3].value) ? 1 : 0);
    if (l_1C->is_global == 0) goto L309D6;
    *(signed char *)(quest_global_states + l_1C->value) = *(signed char *)&l_18;
    return;
L309D6:;
    l_1C->value = *(signed char *)&l_18;
}

void func_00030C10(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = 0;
L30C25:;
    if (l_24 < 10) goto L30C38;
    return;
L30C30:;
    l_24++;
    goto L30C25;
L30C38:;
    l_1C = 0;
    if (((int)(unsigned char)(*(signed char *)(quest_faces + (l_24 * 10)) & 16)) == 0) goto L30C5E;
    l_18 = *(int *)D_00195D14;
    goto L30C7C;
L30C5E:;
    l_18 = *(int *)(D_00195D04 + ((((int)(unsigned char)*(signed char *)(quest_faces + (l_24 * 10))) >> 6) << 2));
L30C7C:;
    l_20 = (int)(unsigned char)(*(signed char *)(quest_faces + (l_24 * 10)) & 15);
L30C90:;
    if (l_1C >= l_20) goto L30CB5;
    l_18 = (((int)(unsigned short)*(short *)((char *)l_18 + 10)) + l_18) + 12;
    l_1C++;
    goto L30C90;
L30CB5:;
    *(int *)(D_00195A1A + (l_24 * 10)) = l_18;
    if (object_find_quest(D_00195A00->children, (int)(unsigned char)*(signed char *)(D_00195A15 + (l_24 * 10))) != 0) goto L30CF3;
    quest_faces_remove_quest((int)(unsigned char)*(signed char *)(D_00195A15 + (l_24 * 10)));
L30CF3:;
    goto L30C30;
}

void quest_faces_draw(void)
{
    int l_20;
    int l_1C;
    int l_18;

    if (*(signed char *)game_mode != 0) return;
    l_20 = 0;
    l_1C = l_20;
L30D26:;
    if (l_20 < 10) goto L30D36;
    return;
L30D2E:;
    l_20++;
    goto L30D26;
L30D36:;
    if (*(int *)(D_00195A16 + (l_20 * 10)) == 0) goto L30D2E;
    l_18 = *(int *)(D_00195A1A + (l_20 * 10));
    func_00144FB4((l_1C << 5) + 8, 36, (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
    l_1C++;
    goto L30D2E;
}

void quest_face_remove(int a1)
{
    int l_18;

    l_18 = 0;
L30DA6:;
    if (l_18 < 10) goto L30DB6;
    return;
L30DAE:;
    l_18++;
    goto L30DA6;
L30DB6:;
    if (*(int *)(D_00195A16 + (l_18 * 10)) != a1) goto L30DD5;
    *(int *)(D_00195A16 + (l_18 * 10)) = 0;
    return;
L30DD5:;
    goto L30DAE;
}

void func_00030DE1(int a1, int a2)
{
    int l_14;

    l_14 = 0;
L30DFB:;
    if (l_14 < 10) goto L30E0B;
    return;
L30E03:;
    l_14++;
    goto L30DFB;
L30E0B:;
    if (*(int *)(D_00195A16 + (l_14 * 10)) != a1) goto L30E29;
    *(int *)(D_00195A16 + (l_14 * 10)) = a2;
    return;
L30E29:;
    goto L30E03;
}

void quest_faces_remove_quest(unsigned char a1)
{
{
    int l_1C;

    l_1C = 0;
L30E4C:;
    if (l_1C < 10) goto L30E5C;
    return;
L30E54:;
    l_1C++;
    goto L30E4C;
L30E5C:;
    if (*(int *)(D_00195A16 + (l_1C * 10)) == 0) goto L30E54;
    if (*(unsigned char *)(D_00195A15 + (l_1C * 10)) != a1) goto L30E86;
    *(int *)(D_00195A16 + (l_1C * 10)) = 0;
L30E86:;
    goto L30E54;
}
}

int qcond_op57_item_used(struct quest *a1, struct qbn_op *a2)
{
    struct item *l_1C;
    struct qbn_state *l_18;

    l_1C = &a2->args[2].object->twin->data.item;
    if (a2->args[2].object->twin != 0) goto L30EC9;
    return 0;
L30EC9:;
    if (((int)(unsigned short)(l_1C->item_flags & 512)) == 0) goto L30F26;
    if (a2->args[1].value != (-1)) goto L30EF0;
    return 1;
L30EF0:;
    l_18 = (struct qbn_state *)a2->args[1].record;
    if (l_18->is_global == 0) goto L30F16;
    *(signed char *)(quest_global_states + l_18->value) = 1;
    goto L30F1D;
L30F16:;
    l_18->value = 1;
L30F1D:;
    return 1;
L30F26:;
    return 0;
}

void func_00030F39(void)
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
    *(short *)D_00190D64 = l_18->image2;
    l_14 = quest_arg_state(a2, 2);
    *(signed char *)itemmaker_slot_kinds = *(signed char *)&l_14;
    if (l_14 == 0) goto L30FCD;
    l_18->data.character.flags |= 0x8000;
    goto L30FD7;
L30FCD:;
    l_18->data.character.flags &= ~0x8000;
L30FD7:;
    object_foreach(D_00195AC4, (int)func_0002FE4B);
}

void qaction_op69_cast_spell_on_foe(struct quest *a1, struct qbn_op *a2)
{
    int l_20;
    struct record *l_1C;
    int l_18;
    struct record *l_14;

    l_1C = a2->args[1].object;
    *(short *)D_00190D64 = l_1C->image2;
    l_18 = 0;
L3101F:;
    if (spell_records[l_18].name[0] == 0) goto L31049;
    if (spell_records[l_18].id == a2->args[2].value) goto L31051;
L31049:;
    l_18++;
    goto L3101F;
L31051:;
    l_14 = object_create_child(player_object->parent, 0, 89);
    D_00195AA8 = l_14;
    l_14->type = 9;
    l_14->caster = player_entity;
    l_14->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    mc_memcpy(&l_14->data.spell, &spell_records[l_18], 89, (int)D_00170A1A, 679, 4);
    object_foreach(D_00195AC4, (int)quest_cast_spell_on_foe_cb);
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
    if (a2 != 0) goto L3114D;
    if (a1->parent->type != 39) goto L3111F;
    return 0;
L3111F:;
    a2 = a1->parent;
    a2 = object_find_by_id(D_00195AC4, a2->id);
    if (a2 != 0) goto L3114D;
    return 0;
L3114D:;
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
    a1->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_34->id = a1->id;
    l_34->quest_id = a1->quest_id;
    func_00030DE1(l_18, a1->id);
    l_34->twin = a1;
    a1->twin = l_34;
    a1 = a1->children;
L31286:;
    if (a1 == 0) goto L3136A;
    l_30 = object_create_child(l_34, 0, 107);
    l_30->x = a2->x;
    l_30->y = a2->y;
    l_30->z = a2->z;
    mc_memcpy(&l_30->data, &a1->data, 107, (int)D_00170A1A, 754, 4);
    l_30->type = a1->type;
    l_30->flags = a1->flags;
    l_30->image = a1->image;
    l_30->image2 = a1->image2;
    a1->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_30->id = a1->id;
    l_30->quest_id = a1->quest_id;
    l_30->twin = a1;
    a1->twin = l_30;
    a1 = a1->next;
    goto L31286;
L3136A:;
    goto L31622;
case 41:
    switch (a2->type) {
case 34:
    l_34 = object_create_child(a2->parent, 0, 3);
    l_34->type = 8;
    l_34->x = a2->x;
    l_34->y = a2->y;
    l_34->z = a2->z;
    a1->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_34->id = a1->id;
    func_00030DE1(l_18, a1->id);
    goto L314B7;
case 8:
    l_34 = a2;
    *(signed char *)((char *)l_34 + 73) |= 128;
    goto L314B7;
case 40:
    a2 = object_find_by_id(D_00195AC4, a2->id);
    l_34 = object_create_child(a2->parent, 0, 3);
    l_34->type = 8;
    l_34->x = a2->x;
    l_34->y = a2->y;
    l_34->z = a2->z;
    a1->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_34->id = a1->id;
    func_00030DE1(l_18, a1->id);
    goto L314B7;
default:
    return 0;
L314B7:;
    l_34->quest_id = a1->quest_id;
    *(int *)((char *)l_34 + 43) = *(int *)((char *)a1 + 43);
    l_2C = (int)RECORD_DATA(l_34);
    if (((int)(unsigned char)(*(signed char *)((char *)l_2C + 2) & 128)) != 0) goto L314F6;
    *(short *)((char *)l_2C) = *(short *)((char *)a1 + 89);
L314F6:;
    if (*(short *)((char *)l_2C) != 0) goto L3152A;
    func_000A0ED9(819, (int)D_00170A1A);
    func_000A148C((int)D_00170A4F, 819);
    *(short *)((char *)l_2C) = 510;
L3152A:;
    if (a1->image == 0) goto L31544;
    l_34->image = a1->image;
    goto L3155A;
L31544:;
    l_34->image = faction_find(*(short *)((char *)l_2C))->flats[0];
L3155A:;
    l_24 = flats_cfg_find(l_34->image);
    *(unsigned char *)((char *)l_2C + 2) |= ((((int)(unsigned char)(*(signed char *)((char *)l_24 + 6) & 1)) != 0) ? 16 : 0);
    if (((int)(unsigned short)(a1->flags & 4)) == 0) goto L315C7;
    l_34->flags |= 4;
    goto L315CE;
L315C7:;
    l_34->flags &= ~0x4;
L315CE:;
    l_34->twin = a1;
    a1->twin = l_34;
    goto L31622;
}
case 40:
    l_34 = object_find_by_id(D_00195AC4, a1->id);
    if (l_34 != 0) goto L31604;
    return 0;
L31604:;
    l_34->quest_id = a1->quest_id;
    l_34->twin = a1;
    a1->twin = l_34;
default:
L31622:;
    if (l_34 == 0) goto L3163D;
    if (((int)(unsigned short)(a1->flags & 2048)) != 0) goto L3163F;
L3163D:;
    goto L31646;
L3163F:;
    l_34->flags |= 0xA00;
L31646:;
    return l_34;
}
}

void func_00031658(struct quest *a1, struct qbn_op *a2, int a3)
{
    a2->flags |= 2;
    if (a3 != 0) goto L3177D;
    if (*(signed char *)D_0019629E != 0) return;
    if (*(int *)creature_count != 0) return;
    if (*(signed char *)D_001962A8 != 0) return;
    if (*(signed char *)game_mode != 0) return;
    if (player_character->race != 8) goto L316CD;
    if (*(signed char *)D_00196280 != 0) goto L316CF;
L316CD:;
    goto L316D4;
L316CF:;
    return;
L316D4:;
    if (player_character->race == 8) goto L316EF;
    if (*(signed char *)D_00196280 == 0) goto L316F1;
L316EF:;
    goto L316F6;
L316F1:;
    return;
L316F6:;
    if (((int)(unsigned char)*(signed char *)player_environment) != 1) goto L3171B;
    if (location_contains(player_object->x, player_object->z) != 0) goto L31720;
L3171B:;
    return;
L31720:;
    if (current_location->kind == 4) goto L31744;
    if (current_location->kind != 7) goto L31746;
L31744:;
    goto L31758;
L31746:;
    if (current_location->kind <= 9) goto L3175A;
L31758:;
    return;
L3175A:;
    if ((*(int *)game_minutes - a2->last_minutes) == 0) return;
    if (rand_range(1, 100) >= 10) goto L3179A;
L3177D:;
    a2->flags |= 4;
    quest_give_item_to_player(a2->args[1].object);
    quest_op_done(a1, a2);
L3179A:;
    a2->last_minutes = *(int *)game_minutes;
}

int func_000317AE(struct quest *a1)
{
    int l_20;
    struct qbn_op *l_1C;

    l_1C = quest_section(a1, 8);
    l_20 = 0;
L317D6:;
    if (a1->section_counts[8] > l_20) goto L317F3;
    goto L3182F;
L317E4:;
    l_20++;
    l_1C++;
    goto L317D6;
L317F3:;
    if (l_1C->opcode != 76) goto L3180F;
    if (((int)(short)(l_1C->flags & 2)) != 0) goto L31811;
L3180F:;
    goto L31822;
L31811:;
    if (((int)(short)(l_1C->flags & 4)) == 0) goto L31824;
L31822:;
    goto L3182D;
L31824:;
    return 0;
L3182D:;
    goto L317E4;
L3182F:;
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

    l_18 = (int)(unsigned char)*(signed char *)current_region;
    l_28 = (int)a1->args[1].record;
    l_1C = a1->args[2].value;
    l_20 = a1->args[3].value;
    maploads_load_region(l_1C);
    l_2C = (struct map_location *)*(int *)D_00196A9C;
    l_24 = 0;
L31A03:;
    if (l_24 < *(int *)D_00196A28) goto L31A22;
    return;
L31A13:;
    l_24++;
    l_2C++;
    goto L31A03;
L31A22:;
    if ((l_2C->map_id & 1048575) != l_20) goto L31ABB;
    maploads_load_region(l_18);
    map_goto_location(l_1C, 1, l_24, 0);
    dungeon_load(-1);
    l_30 = a1->args[1].object->twin;
    if (l_30 == 0) goto L31AB2;
    player_object->x = l_30->x;
    player_object->y = l_30->y;
    player_object->z = l_30->z;
    player_object->yaw = camera_object->yaw;
    *(signed char *)D_001940D5 |= 2;
L31AB2:;
    a1->flags |= 1;
    return;
L31ABB:;
    goto L31A13;
}

void quest_op_done(struct quest *a1, struct qbn_op *a2)
{
    if (((int)(short)(a2->flags & 1)) != 0) return;
    a2->flags |= 1;
    quest_show_message(a1, a2->message);
}
