/* monster.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern char D_00175934[];
extern char D_0017B62F[];
extern char D_00186A14[];
extern char D_00187B44[];
extern char dispel_monster_ids[];
extern struct record *D_00190504[];
extern char D_00190704[];
extern char text_macro_fpc[];
extern char text_rsc_buffer[];
extern char D_001940D7[];
extern char D_001940DA[];
extern struct record *nonworld_root;
extern struct record *player_object;
extern char D_00195AB0[];
extern char vertical_velocity[];
extern struct record *D_00195AC4;
extern struct record *D_00195AD8;
extern char creature_count[];
extern struct character *player_character;
extern char D_00195C70[];
extern char D_00195C74[];
extern char D_00195CB8[];
extern char ai_monster_flags[];
extern char D_00196167[];
extern char player_on_ground[];
extern char D_00196D54[];
extern char D_00196D58[];
extern char D_00196D5C[];
extern char collide_flags[];
extern char D_00199D74[];
extern char D_00199D9B[];
extern char link_count[];

extern int collide_move_object(struct record *, int, int, int);
extern int damage_apply(struct record *, int, int);
extern int spell_cost(struct spell *, struct character *);
extern int cast_creature_spell_at(struct record *, struct record *, struct record *);
extern int func_0005C9BF(unsigned char);
extern int ai_turn_toward(struct record *, int);
extern int ai_stealth_check(int, unsigned short, int, unsigned short);
extern int monster_move_step(struct record *, struct record *, short);
extern int func_00063FCF(struct record *, int, int);
extern int sound_play(int, int, int);
extern int spawn_find_point(struct record *, int, int);
extern int rand_range(int, int);
extern int object_free_single(struct record *);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern struct record *object_random_child_of_type(struct record *, int);
extern int object_new_id(int);
extern int func_0009DEAC();
extern int mc_free();
extern int mc_memset();
extern int mc_memcpy();
extern int memchr();
extern int func_000C7FD9();
extern int func_000C7FF4();
extern int func_000C808D();
extern int func_000CE6E2();
extern void weapon_monster_arrow(int, int);
extern void monster_init(struct record *, int);
extern void object_apply_gravity(struct record *, struct character *);
extern void object_foreach(struct record *, int);
int monster_set_action_seducer(struct record *, int, int);
struct record *func_0006299F(struct record *);
int func_0006379A(struct record *, struct record *);
int func_00063ED8(struct record *, int);
void func_0006243B(struct record *, struct record *, int);
void monster_mark_anim_slot_cb(struct record *);

void func_000622EB(struct record *a1, struct record *a2, int a3, int a4)
{
    struct character *l_10;
    int l_C;

    l_10 = &a1->data.character;
    mc_memcpy((int)D_00196167, a1, 71, (int)D_00175934, 426, 4);
    if (l_10->detour_steps != 1000) goto L62382;
    func_0006243B(a1, a2, a3);
    if (((int)(short)(*(short *)collide_flags & 10)) != 0) goto L62368;
    l_10->detour_steps = 1000;
    return;
L62368:;
    l_10->detour_steps = rand_range(4, 12);
L62382:;
    if (l_10->detour_steps == 0) goto L62399;
    l_10->detour_steps--;
    goto L623FC;
L62399:;
    if (a4 >= 0) goto L623A8;
    l_C = -512;
    goto L623AF;
L623A8:;
    l_C = 512;
L623AF:;
    if (l_10->detour_side == 0) goto L623D2;
    l_10->detour_yaw = l_C + a1->yaw;
    goto L623E5;
L623D2:;
    l_10->detour_yaw = a1->yaw - l_C;
L623E5:;
    l_10->detour_steps = 1000;
    l_10->detour_side ^= 1;
L623FC:;
    mc_memcpy(a1, (int)D_00196167, 71, (int)D_00175934, 457, 4);
    func_0006243B(a1, a2, l_10->detour_yaw & 2047);
}

void func_0006243B(struct record *a1, struct record *a2, int a3)
{
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    unsigned char l_10;
{
    char l_80[32];
    char l_60[40];

    *(int *)((char *)l_60 + 36) = (int)&a1->data.character;
    l_18 = ((((int)(short)*(short *)(*(char **)((char *)l_60 + 36) + 44)) + 100) * *(int *)D_00195AB0) / 1000;
    if (*(int *)(*(char **)((char *)l_60 + 36) + 76) == 0) goto L62491;
    l_24 = 0;
    l_28 = l_24;
    goto L624A2;
L62491:;
    func_000CE6E2(a3, l_18, (int)&l_28, (int)&l_24);
L624A2:;
    *(int *)l_80 = a1->x + l_28;
    *(int *)((char *)l_80 + 4) = a1->y;
    *(int *)((char *)l_80 + 8) = a1->z + l_24;
    if (((struct bf8_0_1 *)&ai_monster_flags)->f == 0) goto L624FE;
    l_14 = *(int *)((char *)l_80 + 4) - (a2->y - 70);
    if (func_0009DEAC(l_14) <= 10) goto L624FE;
    if (l_14 >= 0) goto L624F8;
    *(int *)((char *)l_80 + 4) += l_18;
    goto L624FE;
L624F8:;
    *(int *)((char *)l_80 + 4) -= l_18;
L624FE:;
    *(int *)((char *)l_80 + 12) = a1->angle_x;
    *(int *)((char *)l_80 + 16) = a1->yaw;
    *(int *)((char *)l_80 + 20) = a1->angle_z;
    l_10 = *(signed char *)player_on_ground;
    l_2C = *(int *)vertical_velocity;
    l_1C = *(int *)D_00195C74;
    *(signed char *)collide_flags |= 4;
    l_20 = (*(int *)vertical_velocity = *(int *)(*(char **)((char *)l_60 + 36) + 76));
    *(signed char *)D_001940D7 |= 128;
    *(int *)((char *)l_80 + 24) = (int)D_00187B44;
    if (((int)(unsigned short)(*(short *)(*(char **)((char *)l_60 + 36) + 64) & 2080)) == 0) goto L62578;
    *(short *)((char *)l_80 + 28) |= 1;
    goto L6257D;
L62578:;
    *(short *)((char *)l_80 + 28) &= 65534;
L6257D:;
    mc_memcpy((int)l_60, (int)D_00196D54, 12, (int)D_00175934, 512, 4);
    *(int *)D_00196D54 = a1->x;
    *(int *)D_00196D58 = a1->y - (*(int *)vertical_velocity / 256);
    *(int *)D_00196D5C = a1->z;
    if ((*(int *)((char *)l_80 + 4) - 90) >= *(int *)(*(char **)((char *)l_60 + 36) + 88)) goto L625F0;
    *(int *)((char *)l_80 + 4) = *(int *)(*(char **)((char *)l_60 + 36) + 88) + 90;
L625F0:;
    collide_move_object(a1, 0, (int)l_80, 0);
    *(signed char *)player_on_ground = l_10;
    mc_memcpy((int)D_00196D54, (int)l_60, 12, (int)D_00175934, 521, 4);
    if (*(int *)((char *)l_60 + 36) == (int)player_character) goto L6263C;
    *(int *)(*(char **)((char *)l_60 + 36) + 88) = *(int *)D_00195C74;
L6263C:;
    if (((int)(short)(*(short *)collide_flags & 16)) == 0) goto L62655;
    if (((struct bf8_0_1 *)&ai_monster_flags)->f == 0) goto L62657;
L62655:;
    goto L62664;
L62657:;
    object_apply_gravity(a1, (struct character *)*(int *)((char *)l_60 + 36));
    goto L62675;
L62664:;
    *(int *)vertical_velocity = 0;
    *(signed char *)(*(char **)((char *)l_60 + 36) + 65) &= 247;
L62675:;
    if (*(int *)vertical_velocity != 0) goto L62684;
    if (l_20 != 0) goto L62686;
L62684:;
    goto L626B2;
L62686:;
    damage_apply(a1, (int)&*(signed char *)((char *)((l_20 / 256) / 80) - 3), 0);
L626B2:;
    *(int *)(*(char **)((char *)l_60 + 36) + 76) = *(int *)vertical_velocity;
    *(int *)vertical_velocity = l_2C;
    *(int *)D_00195C74 = l_1C;
    if (func_0009DEAC(a1->y - player_object->y) <= 3000) return;
    *(signed char *)D_001940DA |= 128;
    object_delete(a1);
}
}

int monster_set_action(struct record *a1, int a2, int a3)
{
    struct monster_anim *l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    if (a1->data.character.race != 29) goto L6273E;
    return monster_set_action_seducer(a1, a2, a3);
L6273E:;
    if (a1->data.character.race < 60) goto L62757;
    if (a3 != 0) goto L62759;
L62757:;
    goto L6275F;
L62759:;
    if (a3 != 60) goto L62761;
L6275F:;
    goto L62767;
L62761:;
    if (a3 != 8) goto L62769;
L62767:;
    goto L62775;
L62769:;
    return 0;
L62775:;
    l_24 = &a1->data.monster.anim;
    l_1C = func_000C808D(a1->x, a1->z, player_object->x, player_object->z);
    l_18 = (a1->yaw + 128) & 2047;
    l_20 = (l_18 - l_1C) & 2047;
    l_20 >>= 8;
    if (l_20 <= 4) goto L627E7;
    l_20 = (int)(unsigned char)*(signed char *)(D_0017B62F + l_20);
    l_24->anim_flags |= 128;
    goto L627EE;
L627E7:;
    l_24->anim_flags &= 127;
L627EE:;
    l_24->anim_facing = *(signed char *)&l_20;
    if (l_24->anim_request == 255) goto L62812;
    return 0;
L62812:;
    if (*(int *)D_00199D74 == 0) goto L62824;
    return 0;
L62824:;
    *(int *)D_00199D74 = 1;
    a1->data.character.action = *(signed char *)&a3;
    l_24->anim_request = *(signed char *)&a3;
    return 1;
}

int monster_set_action_seducer(struct record *a1, int a2, int a3)
{
    struct monster_anim *l_24;
    struct character *l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_24 = &a1->data.monster.anim;
    if (l_24->anim_request != 57) goto L62891;
    return 0;
L62891:;
    l_20 = &a1->data.character;
    l_20->action = *(signed char *)&a3;
    l_18 = func_000C808D(a1->x, a1->z, player_object->x, player_object->z);
    l_14 = (a1->yaw + 128) & 2047;
    l_1C = (l_14 - l_18) & 2047;
    if (((int)(unsigned short)(l_20->flags & 16384)) == 0) goto L6292A;
    if (a3 == 8) goto L6290D;
    if (a3 != 32) goto L62916;
L6290D:;
    a3 = 56;
    goto L62923;
L62916:;
    if (a3 != 0) goto L62923;
    a3 = 59;
L62923:;
    l_1C = 0;
L6292A:;
    l_1C >>= 8;
    if (l_1C <= 4) goto L6294B;
    l_1C = (int)(unsigned char)*(signed char *)(D_0017B62F + l_1C);
    l_24->anim_flags |= 128;
    goto L62952;
L6294B:;
    l_24->anim_flags &= 127;
L62952:;
    if (l_24->anim_request == 8) goto L62972;
    if (l_24->anim_request != 32) goto L6297B;
L62972:;
    return 0;
L6297B:;
    l_24->anim_facing = *(signed char *)&l_1C;
    l_24->anim_request = *(signed char *)&a3;
    return 1;
}

struct record *func_0006299F(struct record *a1)
{
    a1 = a1->children;
L629B9:;
    if (a1 == 0) goto L629E4;
    if (a1->type != 22) goto L629D9;
    return a1->children;
L629D9:;
    a1 = a1->next;
    goto L629B9;
L629E4:;
    return 0;
}

int func_000629F8(int a1)
{
    struct record *l_2C;
    struct record *l_28;
    struct spell *l_24;
    int l_20;
    struct character *l_1C;

    l_20 = 0;
    l_1C = &D_00190504[a1]->data.character;
    if ((l_1C->conditions & 0x100) == 0) goto L62A3A;
    return 0;
L62A3A:;
    l_2C = func_0006299F(D_00190504[a1]);
    l_28 = l_2C;
L62A54:;
    if (l_2C == 0) goto L62AA0;
    l_24 = &l_2C->data.spell;
    if (l_24->target == 2) goto L62A83;
    if (l_24->target != 4) goto L62A95;
L62A83:;
    *(int *)(text_macro_fpc + (l_20++ << 2)) = (int)l_2C;
L62A95:;
    l_2C = l_2C->next;
    goto L62A54;
L62AA0:;
    if (l_20 != 0) goto L62AAF;
    return 0;
L62AAF:;
    l_2C = l_28;
    l_20 = rand_range(0, l_20 - 1);
    if (func_0005C9BF((D_00195AD8 = (struct record *)*(int *)(text_macro_fpc + (l_20 << 2)))->data.spell.id) == 0) goto L62AF6;
    return 0;
L62AF6:;
    return 1;
}

int ai_pick_touch_spell(int a1)
{
    struct record *l_2C;
    struct record *l_28;
    struct spell *l_24;
    int l_20;
    struct character *l_1C;

    l_20 = 0;
    l_1C = &D_00190504[a1]->data.character;
    if ((l_1C->conditions & 0x100) == 0) goto L62B4C;
    return 0;
L62B4C:;
    l_2C = func_0006299F(D_00190504[a1]);
    l_28 = l_2C;
L62B66:;
    if (l_2C == 0) goto L62BAB;
    l_24 = &l_2C->data.spell;
    if (l_24->target == 0) goto L62B8E;
    if (l_24->target != 1) goto L62BA0;
L62B8E:;
    *(int *)(text_macro_fpc + (l_20++ << 2)) = (int)l_2C;
L62BA0:;
    l_2C = l_2C->next;
    goto L62B66;
L62BAB:;
    if (l_20 != 0) goto L62BBA;
    return 0;
L62BBA:;
    l_2C = l_28;
    l_20 = rand_range(0, l_20 - 1);
    if (func_0005C9BF((D_00195AD8 = (struct record *)*(int *)(text_macro_fpc + (l_20 << 2)))->data.spell.id) == 0) goto L62C01;
    return 0;
L62C01:;
    return 1;
}

int func_00062C15(int a1)
{
    struct character *l_1C;

    l_1C = &D_00190504[a1]->data.character;
    if ((l_1C->conditions & 0x100) == 0) goto L62C4D;
    return 0;
L62C4D:;
    if (func_0005C9BF((D_00195AD8 = object_random_child_of_type(D_00190504[a1], 9))->data.spell.id) == 0) goto L62C8A;
    return 0;
L62C8A:;
    return ((D_00195AD8 != 0) ? 1 : 0);
}

int func_00062CB6(int a1)
{
    struct character *l_1C;

    l_1C = &D_00190504[a1]->data.character;
    if ((l_1C->conditions & 0x100) == 0) goto L62CEE;
    return 0;
L62CEE:;
    if (func_0005C9BF((D_00195AD8 = object_random_child_of_type(D_00190504[a1], 9))->data.spell.id) == 0) goto L62D2B;
    return 0;
L62D2B:;
    return ((D_00195AD8 != 0) ? 1 : 0);
}

int monster_cast_spell(struct record *a1, struct record *a2)
{
    struct record *l_1C;
    struct character *l_18;

    l_18 = &a1->data.character;
    if ((l_18->conditions & 0x100) == 0) goto L62D8B;
    return 0;
L62D8B:;
    if ((D_00195AD8 = object_random_child_of_type(a1, 9)) != 0) goto L62DB2;
    return 0;
L62DB2:;
    if (func_0005C9BF(D_00195AD8->data.spell.id) == 0) goto L62DD7;
    return 0;
L62DD7:;
    l_1C = object_create_child(a1->parent, 0, 89);
    l_1C->type = 9;
    l_1C->id = object_new_id(100);
    mc_memcpy(&l_1C->data.spell, &D_00195AD8->data.spell, 89, (int)D_00175934, 758, 4);
    cast_creature_spell_at(l_1C, a1, a2);
    l_18->magicka -= spell_cost(&l_1C->data.spell, l_18);
    if (l_18->magicka >= 0) goto L62E6C;
    l_18->magicka = 0;
    goto L62EAB;
L62E6C:;
    if ((a2->data.character.conditions & 0x400) == 0) goto L62E9D;
    if (rand_range(1, 100) < (l_18->attributes[1] / 2)) goto L62E9F;
L62E9D:;
    goto L62EAB;
L62E9F:;
    l_18->magicka = 0;
L62EAB:;
    return 1;
}

void monster_shoot_arrow(int a1, int a2)
{
    sound_play(6, a1, 100);
    weapon_monster_arrow(a1, a2);
}

int ai_angle_diff(int a1, int a2, int *a3)
{
    int l_14;

    a1 &= 2047;
    a2 &= 2047;
    l_14 = a2 - a1;
    if (l_14 != 0) goto L62F34;
    return l_14;
L62F34:;
    if (l_14 <= 1024) goto L62F58;
    *a3 = -1;
    return a1 + (2048 - a2);
L62F58:;
    if (l_14 <= 0) goto L62F67;
    if (l_14 <= 1024) goto L62F69;
L62F67:;
    goto L62F7D;
L62F69:;
    *a3 = 1;
    return a2 - a1;
L62F7D:;
    if (l_14 >= (-1024)) goto L62FA1;
    *a3 = 1;
    return a2 + (2048 - a1);
L62FA1:;
    *a3 = -1;
    return a1 - a2;
}

void monster_mark_anim_slot_cb(struct record *a1)
{
    struct character *l_18;

    if (a1->type != 18) return;
    l_18 = &a1->data.character;
    *(signed char *)(text_rsc_buffer + l_18->anim_slot) = 1;
}

int monster_alloc_anim_slot(void)
{
    int l_1C;

    mc_memset((int)text_rsc_buffer, 0, 128, (int)D_00175934, 829, 2048);
    object_foreach(D_00195AC4->children, (int)monster_mark_anim_slot_cb);
    object_foreach(nonworld_root->children, (int)monster_mark_anim_slot_cb);
    l_1C = 0;
L6305C:;
    if (l_1C < 128) goto L63072;
    goto L630DF;
L6306A:;
    l_1C++;
    goto L6305C;
L63072:;
    if (*(signed char *)(text_rsc_buffer + l_1C) != 0) goto L6308D;
    if (*(int *)(D_00190704 + (l_1C << 2)) != 0) goto L6308F;
L6308D:;
    goto L630DD;
L6308F:;
    if (*(int *)(D_00190704 + (l_1C << 2)) == 0) goto L630B0;
    if (*(int *)(D_00190704 + (l_1C << 2)) != (-1751672937)) goto L630B2;
L630B0:;
    goto L630DD;
L630B2:;
    mc_free(*(int *)(D_00190704 + (l_1C << 2)), (int)D_00175934, 836);
    *(int *)(D_00190704 + (l_1C << 2)) = -1751672937;
L630DD:;
    goto L6306A;
L630DF:;
    l_1C = 0;
L630E6:;
    if (*(signed char *)(text_rsc_buffer + l_1C) == 0) goto L630FA;
    l_1C++;
    goto L630E6;
L630FA:;
    return l_1C;
}

int monster_sees_invisible(int a1)
{
    return memchr((int)dispel_monster_ids, a1, 14);
}

struct record *monster_summon_near_player(int a1)
{
    struct record *l_1C;

    l_1C = object_create_child(player_object->parent, 0, 659);
    if (spawn_find_point(l_1C, 96, 300) == 0) goto L634F6;
    l_1C->type = 18;
    monster_init(l_1C, a1);
    l_1C->data.character.team = 0;
    return l_1C;
L634F6:;
    object_free_single(l_1C);
    return 0;
}

void ai_move_toward_target(struct record *a1, struct character *a2, struct record *a3, int a4, int a5)
{
    int l_10;
    int l_C;

    if (((int)(unsigned short)(a2->flags & 384)) == 0) goto L63552;
    if (((int)(unsigned char)(a2->nav_blocked & 1)) == 0) goto L63557;
L63552:;
    goto L635D6;
L63557:;
    a2->nav_turn_count = 12;
    if (a5 < 32) goto L63574;
    ai_turn_toward(a1, a4);
    goto L635D1;
L63574:;
    if (monster_move_step(a1, a3, a1->yaw) != 0) goto L635C7;
    if (a2->fall_velocity != 0) goto L635AF;
    a2->nav_stuck_count++;
    if (a2->nav_stuck_count > 6) goto L635B1;
L635AF:;
    goto L635C5;
L635B1:;
    a2->nav_blocked |= 1;
    a2->nav_stuck_count = 0;
L635C5:;
    goto L635D1;
L635C7:;
    a2->nav_blocked = 0;
L635D1:;
    return;
L635D6:;
    if (((int)(unsigned short)(a2->flags & 256)) == 0) goto L635F7;
    if (a2->nav_turn_count == 0) goto L635F9;
L635F7:;
    goto L63603;
L635F9:;
    a2->nav_blocked &= 254;
L63603:;
    l_10 = func_0006379A(a1, a3);
    switch (a2->nav_direction) {
case 2:
    l_C = *(int *)(D_00186A14 + (l_10 << 2));
    goto L6367C;
case 4:
    l_C = (*(int *)(D_00186A14 + (l_10 << 2)) + 1024) & 2047;
    goto L6367C;
case 8:
    l_C = (*(int *)(D_00186A14 + (l_10 << 2)) + 1536) & 2047;
default:
L6367C:;
    if (((int)(unsigned char)(a2->nav_blocked & 1)) == 0) return;
    if (a5 > 256) goto L636C1;
    if (func_00063ED8(a1, (a4 + 256) / 512) != 0) goto L636C3;
L636C1:;
    goto L636C8;
L636C3:;
    return;
L636C8:;
    if (ai_turn_toward(a1, l_C) != 0) return;
    if (a2->nav_turn_count == 0) goto L636F0;
    a2->nav_turn_count--;
L636F0:;
    if ((a2->nav_blocked & a2->nav_direction) == 0) goto L63769;
    a2->nav_direction <<= 1;
    if (((int)(unsigned char)(a2->nav_direction & 16)) == 0) goto L6373E;
    a2->nav_direction = 2;
    a2->nav_blocked &= 254;
L6373E:;
    if (((int)(unsigned short)(a2->flags & 128)) == 0) goto L63767;
    a2->nav_turn_count = 0;
    a2->nav_blocked &= 254;
L63767:;
    return;
L63769:;
    if (monster_move_step(a1, a3, a1->yaw) != 0) return;
    a2->nav_blocked |= a2->nav_direction;
}
}

int func_0006379A(struct record *a1, struct record *a2)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 0;
    l_1C = 0;
    if (a2->x >= a1->x) goto L637D0;
    l_20 = 1;
L637D0:;
    if (a2->z >= a1->z) goto L637E5;
    l_1C = 2;
L637E5:;
    if (func_0009DEAC(a2->x - a1->x) <= func_0009DEAC(a2->z - a1->z)) goto L63816;
    l_18 = 1;
    goto L6381D;
L63816:;
    l_18 = 0;
L6381D:;
    l_18 += l_1C;
    if (l_20 == 0) goto L63834;
    l_18 = 7 - l_18;
L63834:;
    return l_18;
}

void monster_apply_gravity(void)
{
    char l_5C[32];
    char l_3C[12];
    int l_30;
    int l_2C;
    struct character *l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_1C = *(int *)vertical_velocity;
    l_24 = (int)(unsigned char)*(signed char *)player_on_ground;
    l_20 = *(int *)D_00195C74;
    mc_memcpy((int)l_3C, (int)D_00196D54, 12, (int)D_00175934, 1147, 4);
    l_30 = 0;
L63B94:;
    if (l_30 < *(int *)creature_count) goto L63BAC;
    goto L63D9C;
L63BA4:;
    l_30++;
    goto L63B94;
L63BAC:;
    l_28 = &D_00190504[l_30]->data.character;
    if (((int)(unsigned short)(l_28->flags & 2080)) != 0) goto L63BDC;
    if (l_28->fall_velocity == 0) goto L63BA4;
L63BDC:;
    l_2C = l_28->fall_velocity;
    *(int *)vertical_velocity = l_2C;
    mc_memcpy((int)D_00196D54, (int)D_00190504[l_30] + 7, 12, (int)D_00175934, 1159, 4);
    *(int *)l_5C = D_00190504[l_30]->x;
    *(int *)((char *)l_5C + 4) = (int)(*(char **)((char *)D_00190504[l_30] + 11) + (*(int *)vertical_velocity / 256));
    *(int *)((char *)l_5C + 8) = D_00190504[l_30]->z;
    *(int *)((char *)l_5C + 12) = D_00190504[l_30]->angle_x;
    *(int *)((char *)l_5C + 16) = D_00190504[l_30]->yaw;
    *(int *)((char *)l_5C + 20) = D_00190504[l_30]->angle_z;
    if (((int)(unsigned short)(l_28->flags & 2080)) == 0) goto L63CBA;
    *(short *)((char *)l_5C + 28) |= 1;
    goto L63CBF;
L63CBA:;
    *(short *)((char *)l_5C + 28) &= 65534;
L63CBF:;
    *(int *)((char *)l_5C + 24) = (int)D_00187B44;
    *(signed char *)collide_flags &= 251;
    collide_move_object(D_00190504[l_30], 0, (int)l_5C, 0);
    if (((int)(short)(*(short *)collide_flags & 16)) == 0) goto L63D0B;
    object_apply_gravity(D_00190504[l_30], l_28);
    goto L63D1C;
L63D0B:;
    *(int *)vertical_velocity = 0;
    l_28->flags &= ~0x800;
L63D1C:;
    if (*(int *)vertical_velocity != 0) goto L63D2B;
    if (l_2C != 0) goto L63D2D;
L63D2B:;
    goto L63D8B;
L63D2D:;
    l_18 = ((l_2C / 256) / 40) - 5;
    if (l_18 <= 0) goto L63D8B;
    l_18 = l_18 * l_18;
    l_18 = l_18 / 3;
    damage_apply(D_00190504[l_30], l_18, 0);
L63D8B:;
    l_28->fall_velocity = *(int *)vertical_velocity;
    goto L63BA4;
L63D9C:;
    *(int *)D_00195C74 = l_20;
    *(signed char *)player_on_ground = *(signed char *)&l_24;
    *(int *)vertical_velocity = l_1C;
    mc_memcpy((int)D_00196D54, (int)l_3C, 12, (int)D_00175934, 1201, 4);
}

void func_00063DDC(struct record *a1)
{
    int l_24;
    int l_20;
    struct character *l_1C;
    struct record *l_18;

    l_24 = 0;
L63DF4:;
    if (l_24 < *(int *)creature_count) goto L63E0C;
    return;
L63E04:;
    l_24++;
    goto L63DF4;
L63E0C:;
    l_18 = D_00190504[l_24];
    l_20 = func_000C7FF4(l_18->y - player_object->y, func_000C7FD9(l_18->x, l_18->z, player_object->x, player_object->z));
    l_1C = &D_00190504[l_24]->data.character;
    if (a1 == D_00190504[l_24]) goto L63EB3;
    if (ai_stealth_check(l_1C->race, (int)(unsigned short)(l_1C->flags & 256), l_20, (int)(unsigned short)(l_1C->flags & 8)) == 0) goto L63EC9;
L63EB3:;
    l_1C->flags |= 264;
    l_1C->give_up_timer = 200;
L63EC9:;
    goto L63E04;
}

int func_00063ED8(struct record *a1, int a2)
{
    int *l_24;
    int *l_20;
    int l_1C;
    struct character *l_18;

    l_18 = &a1->data.character;
    if (a2 == 0) goto L63F00;
    if (a2 != 2) goto L63F14;
L63F00:;
    l_24 = &a1->x;
    l_20 = &a1->z;
    goto L63F26;
L63F14:;
    l_24 = &a1->z;
    l_20 = &a1->x;
L63F26:;
    l_1C = *l_24 & 63;
    if (l_1C != 0) goto L63F43;
    return 0;
L63F43:;
    if (l_1C >= 8) goto L63F5C;
    l_1C = 0;
    l_18->nav_blocked &= 254;
    goto L63F85;
L63F5C:;
    if (l_1C >= 32) goto L63F68;
    l_1C += -8;
    goto L63F85;
L63F68:;
    if (l_1C <= 56) goto L63F81;
    l_1C = 64;
    l_18->nav_blocked &= 254;
    goto L63F85;
L63F81:;
    l_1C += 8;
L63F85:;
    l_1C += *l_24 & -64;
    if (a2 == 0) goto L63F9B;
    if (a2 != 2) goto L63FB0;
L63F9B:;
    return func_00063FCF(a1, l_1C, *l_20);
L63FB0:;
    return func_00063FCF(a1, *l_20, l_1C);
}

int func_000641CD(int a1)
{
    int l_1C;

    l_1C = 0;
L641E5:;
    if (l_1C < *(int *)link_count) goto L641FA;
    goto L64214;
L641F2:;
    l_1C++;
    goto L641E5;
L641FA:;
    if (*(int *)(D_00199D9B + (l_1C * 39)) != a1) goto L64212;
    return 1;
L64212:;
    goto L641F2;
L64214:;
    return 0;
}

void func_00064301(void)
{
    *(int *)link_count = 0;
    *(int *)D_00195CB8 = (*(int *)D_00195C70 = 0);
}
