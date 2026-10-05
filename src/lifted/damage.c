/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_5_1 { unsigned char _:5; unsigned char f:1; };
extern char mouse_buttons[];
extern char screen_buffer[];
extern char D_001709E4[];
extern char D_001709FB[];
extern char monster_diseases_bat[];
extern char monster_diseases_mummy[];
extern char monster_diseases_plague[];
extern char weapon_swing_sounds[];
extern char monster_parry_ids[];
extern char monster_weights[];
extern char monster_category[];
extern char itemmaker_slot_kinds[];
extern char D_00190D64[];
extern char D_001940DA[];
extern char quest_global_states[];
extern char D_001959FC[];
extern char D_00195A08[];
extern char D_00195A0C[];
extern char D_00195A78[];
extern struct record *player_object;
extern struct record *D_00195AA8;
extern struct record *D_00195AC4;
extern char hud_bar_image[];
extern struct character *player_character;
extern char game_minutes[];
extern struct settings *game_settings;
extern char D_00195D48[];
extern char D_00195DA0[];
extern char D_00196271[];
extern char game_mode[];
extern char D_00196291[];
extern char D_0019629B[];
extern char D_00196DC4[];
extern char D_001970C4[];
extern struct quest *current_quest;
extern struct record *quest_event_object;
extern struct quest *D_00199780;
extern struct qbn_op *quest_prompt_op;
extern struct quest *quest_prompt_quest;
extern char D_001997AA[];

extern int object_weight(struct record *);
extern int cast_creature_spell(struct record *, struct record *, int);
extern struct record *spell_find_on_entity(struct record *, short, int);
extern int disk_resolve_path(int);
extern int rand_range(int, int);
extern struct record *object_create_child(struct record *, int, int);
extern int object_new_id(int);
extern int travel_route(int, int, int, int, int);
extern int rand();
extern int mc_memset();
extern int memchr();
extern int func_000C1500();
extern int func_000C808D();
extern int func_000C9F08();
extern int func_000CE70D();
extern int func_0012B136();
extern int func_0012D8BD();
extern int func_00135DE4();
extern void quest_run_opcodes(struct quest *);
extern void quest_show_message(struct quest *, int);
extern void quest_op_done(struct quest *, struct qbn_op *);
extern void time_pass(int);
extern void palette_restore(void);
extern void cast_spell_on(struct record *, struct record *, int);
extern void disease_infect(struct record *, int, int, int);
extern void func_0007E31C(struct record *);
extern void object_foreach(struct record *, int);
void disease_infect_lycanthropy(struct record *, int);
void disease_infect_vampirism(struct record *);
void damage_collapse_exhausted(struct record *);
void func_0002FB8B(struct record *);
void quest_prompt_answer(void);

void damage_monster_hit_effects(struct record *a1, struct record *a2)
{
    struct character *l_1C;
    struct character *l_18;
    int l_14;

    l_1C = &a1->data.character;
    l_18 = &a2->data.character;
    switch (l_1C->race) {
case 0:
    if (rand_range(0, 100) > 5) goto L2F11A;
    disease_infect(a2, (int)monster_diseases_plague, 0, 0);
L2F11A:;
    return;
case 3:
    if (rand_range(0, 100) > 2) goto L2F141;
    disease_infect(a2, (int)monster_diseases_bat, 0, 0);
L2F141:;
    return;
case 6:
    if (spell_find_on_entity(a2, 66, 0) != 0) goto L2F169;
    cast_creature_spell(a1, a2, 66);
L2F169:;
    return;
case 9:
    if (rand() >= 400) goto L2F184;
    disease_infect_lycanthropy(a2, 0);
L2F184:;
    return;
case 10:
    l_14 = l_18->fatigue;
    l_14 -= rand_range(10, 30) << 6;
    if (l_14 >= 0) goto L2F1BA;
    l_14 = 0;
L2F1BA:;
    l_18->fatigue = l_14;
    if (l_14 != 0) goto L2F1D5;
    damage_collapse_exhausted(a2);
L2F1D5:;
    return;
case 14:
    if (rand() >= 400) goto L2F1F3;
    disease_infect_lycanthropy(a2, 1);
L2F1F3:;
    return;
case 19:
    if (rand_range(1, 100) > 5) goto L2F21D;
    disease_infect(a2, (int)monster_diseases_mummy, 0, 0);
L2F21D:;
    return;
case 20:
    if (spell_find_on_entity(a2, 66, 0) != 0) goto L2F242;
    cast_creature_spell(a1, a2, 66);
L2F242:;
    return;
case 28:
case 30:
    if (rand() >= 400) goto L2F25A;
    disease_infect_vampirism(a2);
    return;
L2F25A:;
    if (rand_range(1, 100) > 2) return;
    disease_infect(a2, (int)monster_diseases_plague, 0, 0);
default:;
}
}

void disease_infect_lycanthropy(struct record *a1, int a2)
{
    if (player_character->level == 1) goto L2F2C2;
    if (player_character->race <= 7) goto L2F2C4;
L2F2C2:;
    return;
L2F2C4:;
    player_character->special_infection_time = *(int *)game_minutes + 4320;
    player_character->special_infection = a2 + 1;
    player_character->flags |= 16;
}

void disease_infect_vampirism(struct record *a1)
{
    if (player_character->level == 1) goto L2F332;
    if (player_character->race <= 7) goto L2F334;
L2F332:;
    return;
L2F334:;
    player_character->special_infection_time = *(int *)game_minutes + 4320;
    player_character->special_infection = 0;
    player_character->flags |= 16;
}

void damage_collapse_exhausted(struct record *a1)
{
    int l_1C;
    int l_18;

    l_1C = func_000C9F08();
    mc_memset(655360, 0, ((((int)(unsigned short)(*(short *)((char *)((int)game_settings)) & 1)) != 0) ? 64000 : ((int)(unsigned short)*(short *)(*(char **)hud_bar_image + 2)) * 320), (int)D_001709E4, 701, 4);
    time_pass(20160);
L2F3E3:;
    if ((func_000C9F08() - l_1C) < 22) goto L2F3E3;
}

void damage_spawn_splash(struct record *a1, int a2, int a3)
{
    int l_1C;
    struct record *l_18;
    struct character *l_14;
    short l_10;

    l_18 = object_create_child(a1->parent, 0, 0);
    l_18->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_18->type = 42;
    l_18->x = a1->x;
    *(int *)&l_10 = func_00135DE4(a1->image >> 7, (int)(unsigned short)(a1->image & 127));
    l_18->y = a1->y - (((int)(unsigned short)*(short *)(*(char **)&l_10 + 6)) >> 1);
    if (a1->type != 18) goto L2F571;
    l_14 = &a1->data.character;
    if (l_14->race == 1) goto L2F558;
    if (l_14->race != 0) goto L2F55A;
L2F558:;
    goto L2F56A;
L2F55A:;
    if (l_14->race != 3) goto L2F571;
L2F56A:;
    l_18->y += 10;
L2F571:;
    l_18->z = a1->z;
    l_18->image = a2 + (*(short *)D_00195DA0 << 7);
    if (a3 == (-1)) goto L2F5E3;
    l_14 = &a1->data.character;
    if (l_14->mobile_id >= 128) goto L2F5CB;
    if (*(signed char *)(monster_category + l_14->race) == 0) goto L2F5CD;
L2F5CB:;
    goto L2F5E3;
L2F5CD:;
    l_18->image = a3 + (*(short *)D_00195DA0 << 7);
L2F5E3:;
    l_1C = func_000C808D(a1->x, a1->z, player_object->x, player_object->z);
    func_000CE70D(0, l_1C, 10, &l_18->x);
    func_0007E31C(l_18);
}

void func_0002F62C(struct record *a1, int a2, int a3, int a4)
{
    int l_14;
    int l_10;
    struct character *l_C;

    l_14 = object_weight(a1);
    if (l_14 == 0) return;
    l_C = &a1->data.character;
    if (l_C->race >= 43) goto L2F688;
    if (*(short *)(monster_weights + (l_C->race * 2)) == 0) goto L2F68A;
L2F688:;
    goto L2F68F;
L2F68A:;
    return;
L2F68F:;
    if (l_C->action == 16) return;
    l_C->knockback_speed = (a4 * (((a2 - l_14) << 8) / (a2 + l_14))) / 256;
    l_C->knockback_speed = (a2 / l_14) * (a4 - l_C->knockback_speed);
    if (l_C->knockback_speed >= 15) goto L2F70D;
    l_C->knockback_speed = 15;
L2F70D:;
    l_C->knockback_angle = a3;
    l_C->flags |= 32;
    l_C->action = 16;
}

void damage_knockback(struct record *a1, int a2, int a3, int a4)
{
    int l_18;
    int l_14;
    int l_10;
    struct character *l_C;

    l_18 = object_weight(a1);
    if (l_18 == 0) return;
    l_C = &a1->data.character;
    if (l_C->race >= 43) goto L2F78B;
    if (*(short *)(monster_weights + (l_C->race * 2)) == 0) goto L2F78D;
L2F78B:;
    goto L2F792;
L2F78D:;
    return;
L2F792:;
    if (l_C->action == 16) return;
    l_14 = (a4 * (((a2 - l_18) << 8) / (a2 + l_18))) / 256;
    l_C->knockback_speed = (a2 / l_18) * (a4 - l_14);
    if (l_C->knockback_speed >= 15) goto L2F802;
    l_C->knockback_speed = 15;
L2F802:;
    l_C->knockback_angle = a3;
    l_C->flags |= 32;
    l_C->action = 16;
}

void func_0002F992(void)
{
    if (*(int *)D_00195A0C == 0) goto L2F9B6;
    if (((unsigned)*(int *)game_minutes) > *(int *)D_00195A0C) goto L2F9B8;
L2F9B6:;
    goto L2F9C2;
L2F9B8:;
    *(int *)D_001959FC = 0;
L2F9C2:;
    if (*(int *)D_00195A78 == 0) goto L2F9D8;
    if (((unsigned)*(int *)game_minutes) > *(int *)D_00195A78) goto L2F9DA;
L2F9D8:;
    return;
L2F9DA:;
    *(int *)D_00195A08 = 0;
}

int damage_miss_sound(struct item *a1, int a2)
{
    if (a1 == 0) goto L2FA84;
    if (a2 == (-1)) goto L2FA1A;
    if (a2 != 200) goto L2FA34;
L2FA1A:;
    return (int)(short)*(short *)(weapon_swing_sounds + (a1->index * 2));
L2FA34:;
    if (memchr((int)monster_parry_ids, a2, 28) == 0) goto L2FA6A;
    if (rand() >= 32768) goto L2FA6A;
    return rand_range(291, 299);
L2FA6A:;
    return (int)(short)*(short *)(weapon_swing_sounds + (a1->index * 2));
L2FA84:;
    return 374;
}

void play_death_video(void)
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
    int l_18;

    func_0012D8BD((int)D_00196DC4, 50);
    mc_memset(655360, 0, 64000, (int)D_001709E4, 875, 4);
    mc_memset(*(int *)screen_buffer, 0, 64000, (int)D_001709E4, 876, 4);
    palette_restore();
    l_18 = disk_resolve_path((int)D_001709FB);
L2FB00:;
    if (*(signed char *)mouse_buttons == 0) goto L2FB10;
    func_0012B136();
    goto L2FB00;
L2FB10:;
    func_000C1500(l_18, 0, 0, 1);
    mc_memset(655360, 0, 64000, (int)D_001709E4, 883, 4);
    mc_memset(*(int *)screen_buffer, 0, 64000, (int)D_001709E4, 884, 4);
    palette_restore();
    *(int *)D_00195D48 = 10000;
    *(signed char *)D_0019629B = 0;
L2FB71:;
    if (*(signed char *)mouse_buttons == 0) return;
    func_0012B136();
    goto L2FB71;
}

void func_0002FB8B(struct record *a1)
{
    if (a1->type != 18) goto L2FBB7;
    if (a1->wait_state == 99) goto L2FBB9;
L2FBB7:;
    return;
L2FBB9:;
    a1->wait_state = 0;
}

void func_0002FBCC(void)
{
    if (*(signed char *)D_001970C4 != 0) return;
    *(signed char *)D_001970C4 = 1;
    object_foreach(D_00195AC4, (int)func_0002FB8B);
}

void func_0002FD75(struct record *a1)
{
    struct quest *l_18;

    if (a1->type != 14) return;
    l_18 = &a1->data.quest;
    quest_run_opcodes(l_18);
}

void func_0002FDB0(struct record *a1)
{
    struct quest *l_18;

    if (a1->type != 14) return;
    l_18 = &a1->data.quest;
    if (l_18->id != *(short *)D_001997AA) return;
    D_00199780 = l_18;
    quest_event_object = a1;
}

void func_0002FE02(struct record *a1)
{
    if (a1->quest_id == 0) goto L2FE31;
    if (((int)(unsigned short)(a1->flags & 32768)) != 0) goto L2FE33;
L2FE31:;
    return;
L2FE33:;
    a1->quest_id = 0;
    a1->flags &= ~0x8000;
}

void func_0002FE4B(struct record *a1)
{
    short l_18;

    if (a1->type != 18) return;
    if ((short)((int)(unsigned char)(signed char)a1->quest_id) != current_quest->id) return;
    l_18 = *(short *)D_00190D64;
    if ((short)a1->image2 != l_18) return;
    if (*(signed char *)itemmaker_slot_kinds == 0) goto L2FEA8;
    a1->data.character.flags |= 0x8000;
    return;
L2FEA8:;
    a1->data.character.flags &= ~0x8000;
}

void quest_cast_spell_on_foe_cb(struct record *a1)
{
    if (a1->type != 18) return;
    if ((short)(a1->quest_id) != current_quest->id) return;
    if (a1->image2 != *(short *)D_00190D64) return;
    *(signed char *)D_00196291 = 1;
    cast_spell_on(D_00195AA8, a1, 1);
    *(signed char *)D_00196291 = 0;
}

int quest_travel_minutes(int a1, struct record *a2, struct record *a3)
{
    if (a2 != 0) goto L2FF74;
    return travel_route(player_object->x, player_object->y, a3->x, a3->y, 0) + 2880;
L2FF74:;
    return travel_route(a2->x, a2->y, a3->x, a3->y, 0) + 2880;
}

void qaction_op35_cycle_state(struct quest *a1, struct qbn_op *a2)
{
    struct qbn_state *l_30[4];
    int l_18;
    short l_14;

    *(int *)&l_14 = 0;
    l_18 = (int)(short)l_14;
L300BB:;
    if (((int)(short)l_14) < 4) goto L300CE;
    goto L30111;
L300C6:;
    (*(int *)&l_14)++;
    goto L300BB;
L300CE:;
    if (a2->args[((int)(short)l_14) + 1].value == (-1)) goto L300F0;
    if (a2->args[((int)(short)l_14) + 1].value != (-2)) goto L300F2;
L300F0:;
    goto L3010F;
L300F2:;
    l_30[l_18++] = (struct qbn_state *)a2->args[((int)(short)l_14) + 1].record;
L3010F:;
    goto L300C6;
L30111:;
    if (l_18 == 0) return;
    if (l_18 != 1) goto L3014A;
    if (l_30[0]->is_global == 0) goto L3013E;
    *(signed char *)(quest_global_states + l_30[0]->value) = 1;
    goto L30145;
L3013E:;
    l_30[0]->value = 1;
L30145:;
    return;
L3014A:;
    *(int *)&l_14 = 0;
L30151:;
    if (((int)(short)l_14) < l_18) goto L30164;
    goto L301A6;
L3015C:;
    (*(int *)&l_14)++;
    goto L30151;
L30164:;
    if (l_30[(int)(short)l_14]->is_global == 0) goto L30193;
    if (*(signed char *)(quest_global_states + l_30[(int)(short)l_14]->value) != 0) goto L301A6;
    goto L301A4;
L30193:;
    if (l_30[(int)(short)l_14]->value != 0) goto L301A6;
L301A4:;
    goto L3015C;
L301A6:;
    if (((int)(short)l_14) != l_18) goto L301D8;
    if (l_30[0]->is_global == 0) goto L301CC;
    *(signed char *)(quest_global_states + l_30[0]->value) = 1;
    goto L301D3;
L301CC:;
    l_30[0]->value = 1;
L301D3:;
    return;
L301D8:;
    if (l_30[(int)(short)l_14]->is_global == 0) goto L30205;
    *(signed char *)(quest_global_states + l_30[(int)(short)l_14]->value) = 0;
    goto L30214;
L30205:;
    l_30[(int)(short)l_14]->value = 0;
L30214:;
    if (l_30[(((int)(short)l_14) + 1) % l_18]->is_global == 0) goto L30254;
    *(signed char *)(quest_global_states + l_30[(((int)(short)l_14) + 1) % l_18]->value) = 1;
    return;
L30254:;
    l_30[(((int)(short)l_14) + 1) % l_18]->value = 1;
}

void qaction_op29_prompt(struct quest *a1, struct qbn_op *a2)
{
    *(signed char *)D_001940DA |= 32;
    quest_prompt_op = a2;
    quest_prompt_quest = a1;
    quest_op_done(a1, a2);
    quest_show_message(a1, a2->args[3].value);
    quest_prompt_answer();
}

void quest_prompt_answer(void)
{
    struct qbn_state *l_18;

    if (((struct bf8_5_1 *)&D_001940DA)->f == 0) goto L30411;
    if (*(signed char *)game_mode == 0) goto L30413;
L30411:;
    return;
L30413:;
    l_18 = *(struct qbn_state **)((char *)quest_prompt_op + 7 + (((int)(unsigned char)*(signed char *)D_00196271) * 15));
    if (l_18->is_global == 0) goto L30447;
    *(signed char *)(quest_global_states + l_18->value) = 1;
    goto L3044E;
L30447:;
    l_18->value = 1;
L3044E:;
    *(signed char *)D_001940DA &= 223;
}

void quest_set_arg_state(struct quest *a1, struct qbn_op *a2, int a3, int a4)
{
    struct qbn_state *l_C;

    if (a2->args[a3].value == (-1)) return;
    l_C = (struct qbn_state *)a2->args[a3].record;
    if (((int)(unsigned char)(a2->args[a3].negate & 1)) == 0) goto L30512;
    a4 ^= 1;
L30512:;
    if (l_C->is_global == 0) goto L3052E;
    *(signed char *)(quest_global_states + l_C->value) = *(signed char *)&a4;
    return;
L3052E:;
    l_C->value = *(signed char *)&a4;
}
