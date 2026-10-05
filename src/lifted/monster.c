/* monster.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern char D_00175934[];
extern signed char D_0017B62F[];
extern int D_00186A14[];
extern char D_00187B44[];
extern signed char dispel_monster_ids[];
extern struct record *D_00190504[];
extern char D_00190704[];
extern char text_macro_fpc[];
extern signed char text_rsc_buffer[];
extern unsigned char D_001940D7;
extern signed char D_001940DA;
extern struct record *nonworld_root;
extern struct record *player_object;
extern int D_00195AB0;
extern int vertical_velocity;
extern struct record *D_00195AC4;
extern struct record *D_00195AD8;
extern int creature_count;
extern struct character *player_character;
extern struct record *D_00195C70;
extern int D_00195C74;
extern struct record *D_00195CB8;
extern int ai_monster_flags;
extern char D_00196167[];
extern signed char player_on_ground;
extern char D_00196D54[];
extern int D_00196D58;
extern int D_00196D5C;
extern char collide_flags[];
extern int D_00199D74;
extern char D_00199D9B[];
extern int link_count;

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
    if (l_10->detour_steps == 1000) {
        func_0006243B(a1, a2, a3);
        if (((int)(short)(*(short *)collide_flags & 10)) == 0) {
            l_10->detour_steps = 1000;
            return;
        }
        l_10->detour_steps = rand_range(4, 12);
    }
    if (l_10->detour_steps != 0) {
        l_10->detour_steps--;
    } else {
        if (a4 < 0) {
            l_C = -512;
        } else {
            l_C = 512;
        }
        if (l_10->detour_side != 0) {
            l_10->detour_yaw = l_C + a1->yaw;
        } else {
            l_10->detour_yaw = a1->yaw - l_C;
        }
        l_10->detour_steps = 1000;
        l_10->detour_side ^= 1;
    }
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
        l_18 = ((((int)(short)*(short *)(*(char **)((char *)l_60 + 36) + 44)) + 100) * D_00195AB0) / 1000;
        if (*(int *)(*(char **)((char *)l_60 + 36) + 76) != 0) {
            l_24 = 0;
            l_28 = l_24;
        } else {
            func_000CE6E2(a3, l_18, (int)&l_28, (int)&l_24);
        }
        *(int *)l_80 = a1->x + l_28;
        *(int *)((char *)l_80 + 4) = a1->y;
        *(int *)((char *)l_80 + 8) = a1->z + l_24;
        if (((struct bf8_0_1 *)&ai_monster_flags)->f != 0) {
            l_14 = *(int *)((char *)l_80 + 4) - (a2->y - 70);
            if (func_0009DEAC(l_14) > 10) {
                if (l_14 < 0) {
                    *(int *)((char *)l_80 + 4) += l_18;
                } else {
                    *(int *)((char *)l_80 + 4) -= l_18;
                }
            }
        }
        *(int *)((char *)l_80 + 12) = a1->angle_x;
        *(int *)((char *)l_80 + 16) = a1->yaw;
        *(int *)((char *)l_80 + 20) = a1->angle_z;
        l_10 = player_on_ground;
        l_2C = vertical_velocity;
        l_1C = D_00195C74;
        *(signed char *)collide_flags |= 4;
        l_20 = (vertical_velocity = *(int *)(*(char **)((char *)l_60 + 36) + 76));
        D_001940D7 |= 128;
        *(int *)((char *)l_80 + 24) = (int)D_00187B44;
        if (((int)(unsigned short)(*(short *)(*(char **)((char *)l_60 + 36) + 64) & 2080)) != 0) {
            *(short *)((char *)l_80 + 28) |= 1;
        } else {
            *(short *)((char *)l_80 + 28) &= 65534;
        }
        mc_memcpy((int)l_60, (int)D_00196D54, 12, (int)D_00175934, 512, 4);
        *(int *)D_00196D54 = a1->x;
        D_00196D58 = a1->y - (vertical_velocity / 256);
        D_00196D5C = a1->z;
        if ((*(int *)((char *)l_80 + 4) - 90) < *(int *)(*(char **)((char *)l_60 + 36) + 88)) {
            *(int *)((char *)l_80 + 4) = *(int *)(*(char **)((char *)l_60 + 36) + 88) + 90;
        }
        collide_move_object(a1, 0, (int)l_80, 0);
        player_on_ground = l_10;
        mc_memcpy((int)D_00196D54, (int)l_60, 12, (int)D_00175934, 521, 4);
        if (*(int *)((char *)l_60 + 36) != (int)player_character) {
            *(int *)(*(char **)((char *)l_60 + 36) + 88) = D_00195C74;
        }
        if (((int)(short)(*(short *)collide_flags & 16)) != 0 && ((struct bf8_0_1 *)&ai_monster_flags)->f == 0) {
            object_apply_gravity(a1, (struct character *)*(int *)((char *)l_60 + 36));
        } else {
            vertical_velocity = 0;
            *(signed char *)(*(char **)((char *)l_60 + 36) + 65) &= 247;
        }
        if (vertical_velocity == 0 && l_20 != 0) {
            damage_apply(a1, (int)&*(signed char *)((char *)((l_20 / 256) / 80) - 3), 0);
        }
        *(int *)(*(char **)((char *)l_60 + 36) + 76) = vertical_velocity;
        vertical_velocity = l_2C;
        D_00195C74 = l_1C;
        if (func_0009DEAC(a1->y - player_object->y) <= 3000) return;
        D_001940DA |= 128;
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

    if (a1->data.character.race == 29) return monster_set_action_seducer(a1, a2, a3);
    if (a1->data.character.race >= 60 && a3 != 0 && a3 != 60 && a3 != 8) return 0;
    l_24 = &a1->data.monster.anim;
    l_1C = func_000C808D(a1->x, a1->z, player_object->x, player_object->z);
    l_18 = (a1->yaw + 128) & 2047;
    l_20 = (l_18 - l_1C) & 2047;
    l_20 >>= 8;
    if (l_20 > 4) {
        l_20 = (int)(unsigned char)D_0017B62F[l_20];
        l_24->anim_flags |= 128;
    } else {
        l_24->anim_flags &= 127;
    }
    l_24->anim_facing = *(signed char *)&l_20;
    if (l_24->anim_request != 255) return 0;
    if (D_00199D74 != 0) return 0;
    D_00199D74 = 1;
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
    if (l_24->anim_request == 57) return 0;
    l_20 = &a1->data.character;
    l_20->action = *(signed char *)&a3;
    l_18 = func_000C808D(a1->x, a1->z, player_object->x, player_object->z);
    l_14 = (a1->yaw + 128) & 2047;
    l_1C = (l_14 - l_18) & 2047;
    if (((int)(unsigned short)(l_20->flags & 16384)) != 0) {
        if (a3 == 8 || a3 == 32) {
            a3 = 56;
        } else if (a3 == 0) {
            a3 = 59;
        }
        l_1C = 0;
    }
    l_1C >>= 8;
    if (l_1C > 4) {
        l_1C = (int)(unsigned char)D_0017B62F[l_1C];
        l_24->anim_flags |= 128;
    } else {
        l_24->anim_flags &= 127;
    }
    if (l_24->anim_request == 8 || l_24->anim_request == 32) return 0;
    l_24->anim_facing = *(signed char *)&l_1C;
    l_24->anim_request = *(signed char *)&a3;
    return 1;
}

struct record *func_0006299F(struct record *a1)
{
    a1 = a1->children;
    while (a1 != 0) {
        if (a1->type == 22) return a1->children;
        a1 = a1->next;
    }
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
    if ((l_1C->conditions & 0x100) != 0) return 0;
    l_2C = func_0006299F(D_00190504[a1]);
    l_28 = l_2C;
    while (l_2C != 0) {
        l_24 = &l_2C->data.spell;
        if (l_24->target == 2 || l_24->target == 4) {
            *(int *)(text_macro_fpc + (l_20++ << 2)) = (int)l_2C;
        }
        l_2C = l_2C->next;
    }
    if (l_20 == 0) return 0;
    l_2C = l_28;
    l_20 = rand_range(0, l_20 - 1);
    if (func_0005C9BF((D_00195AD8 = (struct record *)*(int *)(text_macro_fpc + (l_20 << 2)))->data.spell.id) != 0) {
        return 0;
    }
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
    if ((l_1C->conditions & 0x100) != 0) return 0;
    l_2C = func_0006299F(D_00190504[a1]);
    l_28 = l_2C;
    while (l_2C != 0) {
        l_24 = &l_2C->data.spell;
        if (l_24->target == 0 || l_24->target == 1) {
            *(int *)(text_macro_fpc + (l_20++ << 2)) = (int)l_2C;
        }
        l_2C = l_2C->next;
    }
    if (l_20 == 0) return 0;
    l_2C = l_28;
    l_20 = rand_range(0, l_20 - 1);
    if (func_0005C9BF((D_00195AD8 = (struct record *)*(int *)(text_macro_fpc + (l_20 << 2)))->data.spell.id) != 0) {
        return 0;
    }
    return 1;
}

int func_00062C15(int a1)
{
    struct character *l_1C;

    l_1C = &D_00190504[a1]->data.character;
    if ((l_1C->conditions & 0x100) != 0) return 0;
    if (func_0005C9BF((D_00195AD8 = object_random_child_of_type(D_00190504[a1], 9))->data.spell.id) != 0) {
        return 0;
    }
    return ((D_00195AD8 != 0) ? 1 : 0);
}

int func_00062CB6(int a1)
{
    struct character *l_1C;

    l_1C = &D_00190504[a1]->data.character;
    if ((l_1C->conditions & 0x100) != 0) return 0;
    if (func_0005C9BF((D_00195AD8 = object_random_child_of_type(D_00190504[a1], 9))->data.spell.id) != 0) {
        return 0;
    }
    return ((D_00195AD8 != 0) ? 1 : 0);
}

int monster_cast_spell(struct record *a1, struct record *a2)
{
    struct record *l_1C;
    struct character *l_18;

    l_18 = &a1->data.character;
    if ((l_18->conditions & 0x100) != 0) return 0;
    if ((D_00195AD8 = object_random_child_of_type(a1, 9)) == 0) return 0;
    if (func_0005C9BF(D_00195AD8->data.spell.id) != 0) return 0;
    l_1C = object_create_child(a1->parent, 0, 89);
    l_1C->type = 9;
    l_1C->id = object_new_id(100);
    mc_memcpy(&l_1C->data.spell, &D_00195AD8->data.spell, 89, (int)D_00175934, 758, 4);
    cast_creature_spell_at(l_1C, a1, a2);
    l_18->magicka -= spell_cost(&l_1C->data.spell, l_18);
    if (l_18->magicka < 0) {
        l_18->magicka = 0;
    } else if ((a2->data.character.conditions & 0x400) != 0 && rand_range(1, 100) < (l_18->attributes[1] / 2)) {
        l_18->magicka = 0;
    }
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
    if (l_14 == 0) return l_14;
    if (l_14 > 1024) {
        *a3 = -1;
        return a1 + (2048 - a2);
    }
    if (l_14 > 0 && l_14 <= 1024) {
        *a3 = 1;
        return a2 - a1;
    }
    if (l_14 < (-1024)) {
        *a3 = 1;
        return a2 + (2048 - a1);
    }
    *a3 = -1;
    return a1 - a2;
}

void monster_mark_anim_slot_cb(struct record *a1)
{
    struct character *l_18;

    if (a1->type != 18) return;
    l_18 = &a1->data.character;
    text_rsc_buffer[l_18->anim_slot] = 1;
}

int monster_alloc_anim_slot(void)
{
    int l_1C;

    mc_memset((int)text_rsc_buffer, 0, 128, (int)D_00175934, 829, 2048);
    object_foreach(D_00195AC4->children, (int)monster_mark_anim_slot_cb);
    object_foreach(nonworld_root->children, (int)monster_mark_anim_slot_cb);
    for (l_1C = 0; l_1C < 128; l_1C++) {
        if (text_rsc_buffer[l_1C] == 0 && *(int *)(D_00190704 + (l_1C << 2)) != 0) {
            if (*(int *)(D_00190704 + (l_1C << 2)) != 0 && *(int *)(D_00190704 + (l_1C << 2)) != (-1751672937)) {
                mc_free(*(int *)(D_00190704 + (l_1C << 2)), (int)D_00175934, 836);
                *(int *)(D_00190704 + (l_1C << 2)) = -1751672937;
            }
        }
    }
    l_1C = 0;
    while (text_rsc_buffer[l_1C] != 0) l_1C++;
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
    if (spawn_find_point(l_1C, 96, 300) != 0) {
        l_1C->type = 18;
        monster_init(l_1C, a1);
        l_1C->data.character.team = 0;
        return l_1C;
    }
    object_free_single(l_1C);
    return 0;
}

void ai_move_toward_target(struct record *a1, struct character *a2, struct record *a3, int a4, int a5)
{
    int l_10;
    int l_C;

    if (((int)(unsigned short)(a2->flags & 384)) != 0 && ((int)(unsigned char)(a2->nav_blocked & 1)) == 0) {
        a2->nav_turn_count = 12;
        if (a5 >= 32) {
            ai_turn_toward(a1, a4);
        } else if (monster_move_step(a1, a3, a1->yaw) == 0) {
            if (a2->fall_velocity == 0) {
                a2->nav_stuck_count++;
                if (a2->nav_stuck_count > 6) goto L635B1;
            }
            goto L635C5;
L635B1:;
            a2->nav_blocked |= 1;
            a2->nav_stuck_count = 0;
L635C5:;
        } else {
            a2->nav_blocked = 0;
        }
        return;
    }
    if (((int)(unsigned short)(a2->flags & 256)) != 0 && a2->nav_turn_count == 0) {
        a2->nav_blocked &= 254;
    }
    l_10 = func_0006379A(a1, a3);
    switch (a2->nav_direction) {
    case 2:
        l_C = D_00186A14[l_10];
        break;
    case 4:
        l_C = (D_00186A14[l_10] + 1024) & 2047;
        break;
    case 8:
        l_C = (D_00186A14[l_10] + 1536) & 2047;
    }
    if (((int)(unsigned char)(a2->nav_blocked & 1)) == 0) return;
    if (a5 <= 256 && func_00063ED8(a1, (a4 + 256) / 512) != 0) return;
    if (ai_turn_toward(a1, l_C) != 0) return;
    if (a2->nav_turn_count != 0) a2->nav_turn_count--;
    if ((a2->nav_blocked & a2->nav_direction) != 0) {
        a2->nav_direction <<= 1;
        if (((int)(unsigned char)(a2->nav_direction & 16)) != 0) {
            a2->nav_direction = 2;
            a2->nav_blocked &= 254;
        }
        if (((int)(unsigned short)(a2->flags & 128)) != 0) {
            a2->nav_turn_count = 0;
            a2->nav_blocked &= 254;
        }
        return;
    }
    if (monster_move_step(a1, a3, a1->yaw) != 0) return;
    a2->nav_blocked |= a2->nav_direction;
}

int func_0006379A(struct record *a1, struct record *a2)
{
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 0;
    l_1C = 0;
    if (a2->x < a1->x) l_20 = 1;
    if (a2->z < a1->z) l_1C = 2;
    if (func_0009DEAC(a2->x - a1->x) > func_0009DEAC(a2->z - a1->z)) {
        l_18 = 1;
    } else {
        l_18 = 0;
    }
    l_18 += l_1C;
    if (l_20 != 0) l_18 = 7 - l_18;
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

    l_1C = vertical_velocity;
    l_24 = (int)(unsigned char)player_on_ground;
    l_20 = D_00195C74;
    mc_memcpy((int)l_3C, (int)D_00196D54, 12, (int)D_00175934, 1147, 4);
    for (l_30 = 0; l_30 < creature_count; l_30++) {
        l_28 = &D_00190504[l_30]->data.character;
        if (((int)(unsigned short)(l_28->flags & 2080)) == 0) {
            if (l_28->fall_velocity == 0) continue;
        }
        l_2C = l_28->fall_velocity;
        vertical_velocity = l_2C;
        mc_memcpy((int)D_00196D54, (int)D_00190504[l_30] + 7, 12, (int)D_00175934, 1159, 4);
        *(int *)l_5C = D_00190504[l_30]->x;
        *(int *)((char *)l_5C + 4) = (int)(*(char **)((char *)D_00190504[l_30] + 11) + (vertical_velocity / 256));
        *(int *)((char *)l_5C + 8) = D_00190504[l_30]->z;
        *(int *)((char *)l_5C + 12) = D_00190504[l_30]->angle_x;
        *(int *)((char *)l_5C + 16) = D_00190504[l_30]->yaw;
        *(int *)((char *)l_5C + 20) = D_00190504[l_30]->angle_z;
        if (((int)(unsigned short)(l_28->flags & 2080)) != 0) {
            *(short *)((char *)l_5C + 28) |= 1;
        } else {
            *(short *)((char *)l_5C + 28) &= 65534;
        }
        *(int *)((char *)l_5C + 24) = (int)D_00187B44;
        *(signed char *)collide_flags &= 251;
        collide_move_object(D_00190504[l_30], 0, (int)l_5C, 0);
        if (((int)(short)(*(short *)collide_flags & 16)) != 0) {
            object_apply_gravity(D_00190504[l_30], l_28);
        } else {
            vertical_velocity = 0;
            l_28->flags &= ~0x800;
        }
        if (vertical_velocity == 0 && l_2C != 0) {
            l_18 = ((l_2C / 256) / 40) - 5;
            if (l_18 > 0) {
                l_18 = l_18 * l_18;
                l_18 = l_18 / 3;
                damage_apply(D_00190504[l_30], l_18, 0);
            }
        }
        l_28->fall_velocity = vertical_velocity;
    }
    D_00195C74 = l_20;
    player_on_ground = *(signed char *)&l_24;
    vertical_velocity = l_1C;
    mc_memcpy((int)D_00196D54, (int)l_3C, 12, (int)D_00175934, 1201, 4);
}

void func_00063DDC(struct record *a1)
{
    int l_24;
    int l_20;
    struct character *l_1C;
    struct record *l_18;

    for (l_24 = 0; l_24 < creature_count; l_24++) {
        l_18 = D_00190504[l_24];
        l_20 = func_000C7FF4(l_18->y - player_object->y, func_000C7FD9(l_18->x, l_18->z, player_object->x, player_object->z));
        l_1C = &D_00190504[l_24]->data.character;
        if (a1 == D_00190504[l_24] || ai_stealth_check(l_1C->race, (int)(unsigned short)(l_1C->flags & 256), l_20, (int)(unsigned short)(l_1C->flags & 8)) != 0) {
            l_1C->flags |= 264;
            l_1C->give_up_timer = 200;
        }
    }
}

int func_00063ED8(struct record *a1, int a2)
{
    int *l_24;
    int *l_20;
    int l_1C;
    struct character *l_18;

    l_18 = &a1->data.character;
    if (a2 == 0 || a2 == 2) {
        l_24 = &a1->x;
        l_20 = &a1->z;
    } else {
        l_24 = &a1->z;
        l_20 = &a1->x;
    }
    l_1C = *l_24 & 63;
    if (l_1C == 0) return 0;
    if (l_1C < 8) {
        l_1C = 0;
        l_18->nav_blocked &= 254;
    } else if (l_1C < 32) {
        l_1C += -8;
    } else if (l_1C > 56) {
        l_1C = 64;
        l_18->nav_blocked &= 254;
    } else {
        l_1C += 8;
    }
    l_1C += *l_24 & -64;
    if (a2 == 0 || a2 == 2) return func_00063FCF(a1, l_1C, *l_20);
    return func_00063FCF(a1, *l_20, l_1C);
}

int func_000641CD(int a1)
{
    int l_1C;

    for (l_1C = 0; l_1C < link_count; l_1C++) {
        if (*(int *)(D_00199D9B + (l_1C * 39)) == a1) return 1;
    }
    return 0;
}

void func_00064301(void)
{
    link_count = 0;
    *(int *)&D_00195CB8 = (*(int *)&D_00195C70 = 0);
}
