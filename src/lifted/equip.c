/* equip.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

struct bf8_0_1 { unsigned char f:1; };
struct bf8_1_1 { unsigned char _:1; unsigned char f:1; };
struct bf8_7_1 { unsigned char _:7; unsigned char f:1; };
extern char D_001758B8[];
extern char D_001758E2[];
extern char D_001758EE[];
extern char D_001758F8[];
extern char D_00175927[];
extern char player_environment[];
extern char item_templates[];
extern char potion_recipes[];
extern char D_00180B42[];
extern char monster_table_flags[];
extern char D_00185F88[];
extern char D_00185FFC[];
extern char D_0018606B[];
extern char D_00186483[];
extern char D_0018654F[];
extern char D_00186578[];
extern char D_00186583[];
extern char D_00186589[];
extern char D_00186591[];
extern char D_0018659B[];
extern char D_0018659F[];
extern char D_001865F2[];
extern char D_00186605[];
extern char D_00186610[];
extern char D_0018661E[];
extern char D_00186626[];
extern char D_00186639[];
extern char D_00186678[];
extern char D_00186774[];
extern char D_00186870[];
extern char D_001868D9[];
extern char D_001869C0[];
extern char D_001869D4[];
extern char D_001869EA[];
extern char D_001869FE[];
extern char D_00187966[];
extern char D_00187CA8[];
extern char D_0018E044[];
extern char D_0018E046[];
extern char text_buffer[];
extern struct record *D_00190504[];
extern char D_001940D6[];
extern char D_001940D7[];
extern char D_001940DA[];
extern struct character *ai_characters[];
extern struct record *ai_entities[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern char D_00195AB0[];
extern struct record *D_00195AC4;
extern char creature_count[];
extern struct location *current_location;
extern struct character *player_character;
extern char D_00195C44[];
extern char trespassing[];
extern char ai_los_index[];
extern char magic_def_count[];
extern char magic_def[];
extern char D_00195D54[];
extern char ai_monster_flags[];
extern char D_00195DC4[];
extern char painting_subject_text[];
extern char painting_adjective_text[];
extern char painting_prefix1_text[];
extern char painting_prefix2_text[];
extern char D_00195F22[];
extern char D_0019615F[];
extern char D_001962AB[];
extern char D_00199D74[];

extern int collide_line_of_sight(struct record *, struct record *);
extern int item_add_to_container(struct record *, int, int, int);
extern int func_000602C0(int, unsigned short);
extern int monster_set_action(struct record *, int, int);
extern int func_000629F8(int);
extern int ai_pick_touch_spell(int);
extern int monster_cast_spell(struct record *, struct record *);
extern int ai_angle_diff(int, int, int);
extern int ai_sees_through_illusion(int);
extern int ai_stealth_check(int, unsigned short, int, unsigned short);
extern int disk_read_file(int, int);
extern int disk_open_data(int);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern int object_delete(struct record *);
extern struct record *object_create_child(struct record *, int, int);
extern int object_reparent(struct record *, struct record *);
extern int object_new_id(int);
extern struct record *location_cell_at(int, int);
extern int rand();
extern int srand();
extern int func_0009DEA7();
extern int lseek();
extern int mc_malloc();
extern int func_000A00CB();
extern int func_000A0ED9(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int memchr();
extern int filelength();
extern int func_000C7FD9();
extern int func_000C7FF4();
extern int func_000C808D();
extern int func_00144F68();
extern void func_0002682B(struct record *);
extern void damage_knockback_move(struct record *, struct character *);
extern void msgbox_show_rsc(int, int);
extern void item_init_from_template(unsigned short, short, short, struct item *);
extern void item_make_random(unsigned short, struct item *);
extern void func_0005FA0E(int, struct record *, int, unsigned short);
extern void item_make_magic(struct item *, int);
extern void item_make_artifact(struct item *, int);
extern void monster_shoot_arrow(struct record *, struct record *);
extern void monster_ambient_sound(struct record *, struct character *);
extern void ai_move_toward_target(struct record *, struct character *, struct record *, int, int);
extern void monster_apply_gravity(void);
extern void weapon_melee_strike(struct record *);
extern void item_break(struct record *);
int func_0005F6E9(int);
int func_0005F955(int);
int func_000614A9(void);
int ai_pick_target(struct record *, struct character *, int);
int ai_turn_toward(struct record *, int);
void item_make(int, int, struct item *);
void func_0005ED19(struct item *);
void func_0005F0E8(struct record *, int);
void func_00061326(struct record *);
void func_000614FB(struct record *);
void ai_creature_think(struct character *, struct record *, struct record *, int);
#pragma aux func_000A0ED9 parm routine [];

void item_make(int a1, int a2, struct item *a3)
{
    switch ((unsigned)a1) {
case 5:
    item_make_artifact(a3, a2);
    return;
case 4:
    item_make_magic(a3, a2);
    return;
case 11:
    item_init_from_template(287, 27, 8, a3);
    return;
default:
    item_init_from_template((int)(unsigned short)*(short *)((char *)(int)(*(char **)(D_00185F88 + (a1 << 2)) + (a2 * 2))), (int)(short)*(short *)&a1, (int)(short)*(short *)&a2, a3);
}
}

void func_0005E5D7(struct item *a1, int a2)
{
    if (a2 <= 7) goto L5E616;
    a1->inventory_image += ((unsigned short)(unsigned char)*(signed char *)(D_00186589 + player_character->original_race)) << 7;
    return;
L5E616:;
    a1->inventory_image += ((unsigned short)(unsigned char)*(signed char *)(D_00186589 + a2)) << 7;
}

void func_0005E636(struct item *a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    switch (a1->group) {
case 12:
    l_1C = 9;
    l_18 = (int)D_00185FFC;
    goto L5E6B8;
case 6:
    l_1C = 13;
    l_18 = (int)D_0018606B;
    goto L5E6B8;
case 2:
    a1->inventory_image += rand_range(0, (int)&*(signed char *)((char *)(a1->variants) - 1));
    return;
default:
    l_1C = 0;
L5E6B8:;
    if (a1->variants < 2) goto L5E6E7;
    l_24 = rand_range(0, (int)&*(signed char *)((char *)(a1->variants) - 1));
    a1->inventory_image += l_24;
L5E6E7:;
    func_0005ED19(a1);
    if (a1->index == l_1C) goto L5E711;
    if (a1->index != (l_1C + 1)) return;
L5E711:;
    a1->inventory_image++;
}
}

void func_0005E722(struct item *a1)
{
    int l_20;
    int l_1C;
    int l_18;

    if (a1->group != 12) goto L5E754;
    l_18 = (int)D_00185FFC;
    l_1C = 9;
    goto L5E762;
L5E754:;
    l_18 = (int)D_0018606B;
    l_1C = 13;
L5E762:;
    l_20 = (int)(unsigned short)(a1->inventory_image & 127);
    if (*(signed char *)((char *)(l_18 + l_20)) == 0) return;
    if (*(signed char *)((char *)(l_18 + l_20) + 1) != *(signed char *)((char *)(l_18 + l_20))) goto L5E7A1;
    a1->inventory_image++;
    return;
L5E7A1:;
    l_20--;
L5E7A7:;
    if (*(signed char *)((char *)(l_18 + l_20) + 1) != *(signed char *)((char *)(l_18 + l_20))) goto L5E7C9;
    l_20--;
    a1->inventory_image--;
    goto L5E7A7;
L5E7C9:;
    if (a1->index == l_1C) goto L5E7EB;
    if (a1->index != (l_1C + 1)) return;
L5E7EB:;
    a1->inventory_image++;
}

void func_0005E7FC(struct item *a1)
{
    int l_20;
    int l_1C;
    int l_18;

    if (a1->group != 12) goto L5E827;
    l_18 = (int)D_00185FFC;
    goto L5E82E;
L5E827:;
    l_18 = (int)D_0018606B;
L5E82E:;
    l_20 = (int)(unsigned short)(a1->inventory_image & 127);
    l_20--;
L5E848:;
    if (*(signed char *)((char *)(l_18 + l_20) + 1) != *(signed char *)((char *)(l_18 + l_20))) return;
    l_20--;
    a1->inventory_image--;
    goto L5E848;
}

void func_0005E874(struct item *a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = ((a1->enchantments[0].type != (-1)) ? 1 : 0);
    a1->material = 0;
    if (a1->group != 3) goto L5E8D0;
    if (a1->index == 18) goto L5E8D2;
L5E8D0:;
    goto L5E8D7;
L5E8D2:;
    return;
L5E8D7:;
    l_28 = rand_range(0, 255);
    if (l_24 == 0) goto L5E900;
    l_28 += 60;
    if (l_28 <= 255) goto L5E900;
    l_28 = 255;
L5E900:;
    l_18 = player_character->level - 10;
    if (l_18 >= 0) goto L5E922;
    l_18 <<= 2;
    goto L5E925;
L5E922:;
    l_18 <<= 1;
L5E925:;
    l_28 += l_18;
    if (((int)(unsigned char)*(signed char *)player_environment) != 2) goto L5E952;
    l_28 -= (14 - current_building->quality) * 2;
L5E952:;
    if (l_28 >= 0) goto L5E961;
    l_28 = 0;
    goto L5E971;
L5E961:;
    if (l_28 <= 256) goto L5E971;
    l_28 = 256;
L5E971:;
    if (*(signed char *)D_001962AB == 0) goto L5E990;
    a1->material = *(signed char *)D_001962AB - 1;
    *(signed char *)D_001962AB = 0;
    goto L5E9CC;
L5E990:;
    if (((int)(unsigned char)*(signed char *)(D_00186591 + a1->material)) >= l_28) goto L5E9CC;
    l_28 -= (int)(unsigned char)*(signed char *)(D_00186591 + a1->material);
    a1->material++;
    goto L5E990;
L5E9CC:;
    a1->value = a1->value * (((int)(short)*(short *)(D_001869C0 + (a1->material * 2))) * 3);
    a1->weight = ((unsigned)(((int)(short)*(short *)(D_001869D4 + (a1->material * 2))) * a1->weight)) >> 2;
    a1->condition = (a1->max_condition = (a1->max_condition * ((int)(short)*(short *)(D_001869EA + (a1->material * 2)))) >> 2);
    a1->enchant_points = (a1->enchant_points * ((int)(short)*(short *)(D_001869FE + (a1->material * 2)))) >> 2;
    a1->color = a1->material + 16;
}

void func_0005EA8F(struct item *a1)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = rand_range(1, 100);
    if (l_24 >= 70) goto L5EAC1;
    if (*(signed char *)D_001962AB == 0) goto L5EAC3;
L5EAC1:;
    goto L5EAD2;
L5EAC3:;
    a1->armor_type = 0;
    a1->weight >>= 1;
    goto L5EAF3;
L5EAD2:;
    if (l_24 >= 90) goto L5EAE1;
    if (*(signed char *)D_001962AB == 0) goto L5EAE3;
L5EAE1:;
    goto L5EAEC;
L5EAE3:;
    a1->armor_type = 1;
    goto L5EAF3;
L5EAEC:;
    a1->armor_type = 2;
L5EAF3:;
    if (a1->armor_type != 2) goto L5ECBD;
    l_1C = rand_range(0, 255);
    if (a1->enchantments[0].type == (-1)) goto L5EB36;
    l_1C += 60;
    if (l_1C <= 255) goto L5EB36;
    l_1C = 255;
L5EB36:;
    l_18 = player_character->level - 10;
    if (l_18 >= 0) goto L5EB58;
    l_18 <<= 2;
    goto L5EB5B;
L5EB58:;
    l_18 <<= 1;
L5EB5B:;
    l_1C += l_18;
    if (((int)(unsigned char)*(signed char *)player_environment) != 2) goto L5EB88;
    l_1C -= (14 - current_building->quality) * 2;
L5EB88:;
    if (l_1C >= 0) goto L5EB97;
    l_1C = 0;
    goto L5EBA7;
L5EB97:;
    if (l_1C <= 256) goto L5EBA7;
    l_1C = 256;
L5EBA7:;
    if (*(signed char *)D_001962AB == 0) goto L5EBC6;
    a1->material = *(signed char *)D_001962AB - 1;
    *(signed char *)D_001962AB = 0;
    goto L5EC02;
L5EBC6:;
    if (((int)(unsigned char)*(signed char *)(D_00186591 + a1->material)) >= l_1C) goto L5EC02;
    l_1C -= (int)(unsigned char)*(signed char *)(D_00186591 + a1->material);
    a1->material++;
    goto L5EBC6;
L5EC02:;
    a1->value = a1->value * (((int)(short)*(short *)(D_001869C0 + (a1->material * 2))) * 3);
    a1->weight = ((unsigned)(((int)(short)*(short *)(D_001869D4 + (a1->material * 2))) * a1->weight)) >> 2;
    a1->condition = (a1->max_condition = (a1->max_condition * ((int)(short)*(short *)(D_001869EA + (a1->material * 2)))) >> 2);
    a1->enchant_points = (a1->enchant_points * ((int)(short)*(short *)(D_001869FE + (a1->material * 2)))) >> 2;
    a1->color = a1->material + 16;
    goto L5ECD7;
L5ECBD:;
    a1->value = a1->value * ((int)&*(signed char *)((char *)(a1->armor_type) + 1));
L5ECD7:;
    l_20 = func_000602C0(a1->armor_type, a1->index);
    if (l_20 == (-1)) return;
    a1->inventory_image = l_20 + (a1->inventory_image & -128);
}

void func_0005ED19(struct item *a1)
{
    int l_18;

    l_18 = rand_range(0, 100);
    if (l_18 < 25) return;
    a1->color = rand_range(1, 10);
    if (a1->group != 6) goto L5EDCA;
    if (a1->index == 10) goto L5ED8D;
    if (a1->index != 11) goto L5EDA3;
L5ED8D:;
    a1->color = *(signed char *)(D_0018659B + (rand() & 3));
    goto L5EDC8;
L5EDA3:;
    if (a1->index != 4) goto L5EDC8;
    a1->color = *(signed char *)(D_0018659F + (rand() & 3));
L5EDC8:;
    return;
L5EDCA:;
    if (a1->group != 12) goto L5EDEC;
    if (a1->index == 8) goto L5EDEE;
L5EDEC:;
    return;
L5EDEE:;
    a1->color = *(signed char *)(D_0018659B + (rand() & 3));
}

void func_0005EE0C(struct record *a1, int a2, int a3)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    struct item *l_14;
    struct record *l_10;

    l_20 = *(int *)(D_00186483 + (a2 << 2));
    if (l_20 == 0) return;
L5EE3A:;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) == 255) return;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 6) goto L5EE75;
    if (((int)(unsigned short)(player_character->flags & 1)) != 0) goto L5EE77;
L5EE75:;
    goto L5EE7D;
L5EE77:;
    *(signed char *)((char *)l_20) = 12;
L5EE7D:;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 12) goto L5EEA3;
    if (((int)(unsigned short)(player_character->flags & 1)) == 0) goto L5EEA5;
L5EEA3:;
    goto L5EEAB;
L5EEA5:;
    *(signed char *)((char *)l_20) = 6;
L5EEAB:;
    l_1C = *(int *)(D_00185F88 + (((int)(unsigned char)*(signed char *)((char *)l_20)) << 2));
    if (l_1C == 0) goto L5EED6;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 8) goto L5EED8;
L5EED6:;
    goto L5EEE7;
L5EED8:;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 1) goto L5EEEC;
L5EEE7:;
    goto L5F08F;
L5EEEC:;
    if (((struct bf8_1_1 *)&D_001940D7)->f == 0) goto L5EF15;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) == 4) goto L5EF13;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 5) goto L5EF15;
L5EF13:;
    goto L5EF17;
L5EF15:;
    goto L5EF1C;
L5EF17:;
    goto L5F08F;
L5EF1C:;
    l_28 = (int)(unsigned char)*(signed char *)((char *)l_20 + 1);
    l_24 = 0;
    if (((int)(unsigned char)*(signed char *)((char *)l_20)) != 7) goto L5EF4D;
    func_0005F0E8(a1, a3);
    goto L5F08F;
L5EF4D:;
    if (((int)(short)*(short *)((char *)l_1C)) == (-1)) goto L5F08F;
    l_18 = ((int)item_templates) + (((int)(short)*(short *)((char *)l_1C)) * 48);
    l_30 = (((21 - ((int)(unsigned char)*(signed char *)((char *)l_18 + 40))) * 5) * l_28) / 100;
    if (((int)(unsigned char)*(signed char *)((char *)l_18 + 40)) > a3) goto L5EFBF;
    if (rand_range(1, 100) <= l_30) goto L5EFC4;
L5EFBF:;
    goto L5F07D;
L5EFC4:;
    l_10 = object_create_child(a1, 0, 107);
    l_10->type = 2;
    l_10->x = player_object->x;
    l_10->y = player_object->y;
    l_10->z = player_object->z;
    l_10->image2 = 998;
    l_10->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    if (a2 < 0) goto L5F034;
    l_10->flags |= 32;
L5F034:;
    l_14 = &l_10->data.item;
    item_make((int)(unsigned char)*(signed char *)((char *)l_20), l_24, l_14);
    if (l_14->group != 3) goto L5F074;
    if (l_14->index == 18) goto L5F07D;
L5F074:;
    l_10->image2 = 0;
L5F07D:;
    (*(char (**)[2])&l_1C)++;
    l_24++;
    goto L5EF4D;
L5F08F:;
    l_20 += 2;
    goto L5EE3A;
}

int func_0005F0A0(int a1)
{
{
    int l_20;

    if (a1 == 12) goto L5F0BD;
    if (a1 != 6) goto L5F0BF;
L5F0BD:;
    goto L5F0C5;
L5F0BF:;
    if (a1 != 2) goto L5F0CE;
L5F0C5:;
    l_20 = 1;
    goto L5F0D5;
L5F0CE:;
    l_20 = 0;
L5F0D5:;
    return l_20;
}
}

void func_0005F0E8(struct record *a1, int a2)
{
    int l_18;
    struct record *l_14;

    a2 = (a2 + 3) / 5;
    if (a2 < 4) goto L5F11C;
    a2--;
L5F11C:;
    a2++;
    l_18 = 0;
L5F129:;
    if (l_18 <= a2) goto L5F13E;
    return;
L5F136:;
    l_18++;
    goto L5F129;
L5F13E:;
    l_14 = object_create_child(a1, 0, 107);
    l_14->type = 2;
    l_14->x = player_object->x;
    l_14->y = player_object->y;
    l_14->z = player_object->z;
    l_14->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_14->flags |= 32;
    item_make(7, a2, &l_14->data.item);
    goto L5F136;
}

void func_0005F1BD(struct record *a1, int a2, int a3, int a4)
{
    int l_14;
    struct record *l_10;
    int l_C;

    if (a3 == 0) goto L5F1E1;
    a3 = 32;
L5F1E1:;
    l_14 = 0;
L5F1E8:;
    if (((current_building->quality >> 1) + 1) > l_14) goto L5F20A;
    goto L5F28E;
L5F202:;
    l_14++;
    goto L5F1E8;
L5F20A:;
    l_10 = object_create_child(a1, 0, 107);
    l_10->type = 2;
    l_10->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    item_make_magic(&l_10->data.item, -1);
    l_10->x = player_object->x;
    l_10->y = player_object->y;
    l_10->z = player_object->z;
    l_10->flags |= a3;
    if (a3 == 0) goto L5F289;
    l_10->data.item.item_flags |= 32;
L5F289:;
    goto L5F202;
L5F28E:;
    l_10 = object_create_child(a1, 0, 107);
    l_10->type = 2;
    l_10->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_10->x = player_object->x;
    l_10->y = player_object->y;
    l_10->z = player_object->z;
    l_10->flags |= 32;
    item_make(27, 0, &l_10->data.item);
    if (a4 == 0) return;
    l_14 = 0;
L5F312:;
    if (((current_building->quality >> 1) + 1) > l_14) goto L5F334;
    return;
L5F32C:;
    l_14++;
    goto L5F312;
L5F334:;
    l_10 = object_create_child(a1, 0, 107);
    l_10->type = 2;
    l_10->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_10->x = player_object->x;
    l_10->y = player_object->y;
    l_10->z = player_object->z;
    l_10->flags |= 32;
    item_make(27, 1, &l_10->data.item);
    if (rand_range(1, 100) >= 25) goto L5F3EB;
    func_00061326(l_10);
    l_10->data.item.value = *(int *)(D_00187966 + (l_10->children->soul_creature << 2)) + 5000;
    goto L5F3F5;
L5F3EB:;
    l_10->data.item.value = 5000;
L5F3F5:;
    goto L5F32C;
}

void func_0005F401(struct record *a1)
{
    int l_20;
    struct record *l_1C;
    int l_18;

    l_20 = 0;
L5F419:;
    if (((current_building->quality >> 1) + 1) > l_20) goto L5F43B;
    return;
L5F433:;
    l_20++;
    goto L5F419;
L5F43B:;
    l_1C = object_create_child(a1, 0, 107);
    l_1C->type = 2;
    l_1C->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    l_1C->x = player_object->x;
    l_1C->y = player_object->y;
    l_1C->z = player_object->z;
    l_1C->flags |= 32;
    item_make(27, 1, &l_1C->data.item);
    if (rand_range(1, 100) >= 25) goto L5F4F2;
    func_00061326(l_1C);
    l_1C->data.item.value = *(int *)(D_00187966 + (l_1C->children->soul_creature << 2)) + 5000;
    goto L5F4FC;
L5F4F2:;
    l_1C->data.item.value = 5000;
L5F4FC:;
    goto L5F433;
}

void func_0005F50B(void)
{
    int l_20;
    int l_1C;
    int l_18;

    func_000A0ED9(625, (int)D_001758B8);
    mc_sprintf((int)text_buffer, (int)D_001758E2, (((int)(unsigned short)*(short *)D_00195DC4) >> 3) + 97);
    disk_read_file((int)text_buffer, *(int *)D_00195C44);
    l_18 = *(int *)D_00195C44;
    l_20 = 0;
    l_1C = (int)(unsigned short)(*(short *)D_00195DC4 & 7);
L5F57E:;
    if (l_20 >= l_1C) goto L5F5A3;
    l_18 = (((int)(unsigned short)*(short *)((char *)l_18 + 10)) + l_18) + 12;
    l_20++;
    goto L5F57E;
L5F5A3:;
    func_00144F68(160 - (((int)(unsigned short)*(short *)((char *)l_18 + 4)) >> 1), 50, (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
}

void item_info_painting(struct item *a1)
{
    int l_1C;
    int l_18;

    l_18 = rand();
    *(short *)D_00195DC4 = rand(srand((int)(unsigned short)(short)a1->message)) % 180;
    l_1C = disk_open_data((int)D_001758EE);
    lseek(l_1C, ((int)(unsigned short)*(short *)D_00195DC4) * 40, 0);
    func_000A00CB(l_1C, *(int *)D_00195C44, 40);
    func_0009DEA7(l_1C);
    *(short *)painting_subject_text = func_0005F6E9(*(int *)D_00195C44) + 6100;
    *(short *)painting_adjective_text = func_0005F6E9(*(int *)D_00195C44 + 10) + 6200;
    *(short *)painting_prefix1_text = func_0005F6E9((int)(*(char **)D_00195C44 + 20)) + 6300;
    *(short *)painting_prefix2_text = (short)func_0005F6E9((int)(*(char **)D_00195C44 + 30)) + 6400;
    *(signed char *)D_001940D6 |= 32;
    msgbox_show_rsc(250, 1);
    srand(l_18);
}

int func_0005F6E9(int a1)
{
    int l_1C;

    l_1C = 0;
L5F701:;
    if (l_1C >= 10) goto L5F71B;
    if (((int)(unsigned char)*(signed char *)((char *)(a1 + l_1C))) != 255) goto L5F71D;
L5F71B:;
    goto L5F725;
L5F71D:;
    l_1C++;
    goto L5F701;
L5F725:;
    if (l_1C != 1) goto L5F737;
    return (int)(unsigned char)*(signed char *)((char *)a1);
L5F737:;
    return (int)(unsigned char)*(signed char *)((char *)(int)((char *)a1 + rand_range(0, l_1C - 1)));
}

void item_init_book(struct item *a1, int a2)
{
    int l_1C;
    int l_18;
    int l_14;

    a1->message = *(short *)(D_0018E044 + (func_0005F955(a2) << 2));
    l_1C = *(int *)D_00195C44;
    func_000A0ED9(674, (int)D_001758B8);
    mc_sprintf((int)text_buffer, (int)D_001758F8, (int)(unsigned short)(short)a1->message);
    l_18 = disk_open_data((int)text_buffer);
    func_000A00CB(l_18, l_1C, 234);
    func_0009DEA7(l_18);
    l_14 = rand();
    srand(*(int *)((char *)l_1C));
    a1->value = rand_range(300, 800);
    srand(l_14);
}

int func_0005F955(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_20 = l_24;
L5F973:;
    if (((int)(unsigned short)*(short *)D_00195F22) > l_24) goto L5F98A;
    goto L5F9A9;
L5F982:;
    l_24++;
    goto L5F973;
L5F98A:;
    if (((int)(unsigned short)*(short *)(D_0018E046 + (l_24 << 2))) > a1) goto L5F9A7;
    l_20++;
L5F9A7:;
    goto L5F982;
L5F9A9:;
    if (l_20 != 0) goto L5F9B6;
    l_20 = 1;
L5F9B6:;
    l_1C = rand() % l_20;
    l_24 = 0;
L5F9CF:;
    if (l_1C < 0) goto L5F9FA;
    if (((int)(unsigned short)*(short *)(D_0018E046 + (l_24 << 2))) > a1) goto L5F9F2;
    l_1C--;
L5F9F2:;
    l_24++;
    goto L5F9CF;
L5F9FA:;
    return l_24 - 1;
}

int func_0005FD36(int a1)
{
    int l_24;
    int l_20;
    struct item *l_1C;

    l_24 = 0;
    l_20 = l_24;
L5FD54:;
    if (l_24 < 27) goto L5FD67;
    goto L5FE42;
L5FD5F:;
    l_24++;
    goto L5FD54;
L5FD67:;
    if (player_character->equipped[l_24] == 0) goto L5FD5F;
    l_1C = &player_character->equipped[l_24]->data.item;
    if (l_1C->group != 1) goto L5FDC4;
    if (memchr((int)D_00186583, l_1C->index, 6) != 0) goto L5FDC6;
L5FDC4:;
    goto L5FDCC;
L5FDC6:;
    l_20 += 10;
    goto L5FD5F;
L5FDCC:;
    if (l_1C->group != 2) goto L5FDE3;
    if (a1 != 0) goto L5FDE5;
L5FDE3:;
    goto L5FE01;
L5FDE5:;
    l_20 += (int)(unsigned char)*(signed char *)(D_00186578 + l_1C->index);
    goto L5FE3D;
L5FE01:;
    if (l_1C->group == 6) goto L5FE23;
    if (l_1C->group != 12) goto L5FE3D;
L5FE23:;
    l_20 += (int)(unsigned char)*(signed char *)(D_0018654F + l_1C->index);
L5FE3D:;
    goto L5FD5F;
L5FE42:;
    return l_20;
}

void func_0005FE55(struct record *a1)
{
    int l_20;
    int l_1C;
    struct building *l_18;

    if (((int)(unsigned char)*(signed char *)player_environment) == 1) goto L5FF6F;
    l_20 = ((int)(unsigned char)*(signed char *)(D_001865F2 + current_location->kind)) - 1;
L5FE92:;
    if (a1->children != 0) goto L5FECD;
    func_0005FA0E(l_20, a1, player_character->level, (int)(unsigned short)(player_character->flags & 1));
    goto L5FE92;
L5FECD:;
    if (l_20 < 9) goto L5FED9;
    if (l_20 <= 14) goto L5FEDE;
L5FED9:;
    return;
L5FEDE:;
    if (rand_range(1, 100) > ((int)(unsigned char)*(signed char *)(D_00186610 + l_20))) goto L5FF29;
    a1 = object_create_child(a1, 0, 107);
    a1->type = 2;
    item_make(27, 8, &a1->data.item);
L5FF29:;
    if (rand_range(1, 100) >= 4) goto L5FF45;
    func_000614FB(a1);
L5FF45:;
    if (rand_range(1, 100) >= 2) goto L5FF6D;
    item_add_to_container(a1, 27, 4, 0);
L5FF6D:;
    return;
L5FF6F:;
    l_18 = object_building(a1);
    if (l_18 != 0) goto L5FF89;
    l_20 = 0;
    goto L5FF94;
L5FF89:;
    l_20 = l_18->type;
L5FF94:;
    if (a1->children != 0) return;
    func_0005FA0E(((int)(unsigned char)*(signed char *)(D_00186605 + l_20)) - 1, a1, player_character->level, (int)(unsigned short)(player_character->flags & 1));
    goto L5FF94;
}

void func_0005FFE5(struct record *a1, int a2, int a3)
{
    struct record *l_14;
    struct item *l_10;

    l_14 = object_create_child(a1, 0, 107);
    l_10 = &l_14->data.item;
    l_14->type = 2;
    l_14->x = a1->x;
    l_14->y = a1->y;
    l_14->z = a1->z;
    l_10->group = 28;
    l_10->index = 0;
    l_10->value = rand_range(a2, a3) * player_character->level;
}

void func_0006007C(struct record *a1)
{
    struct record *l_18;

    l_18 = object_create_child(a1, 0, 107);
    l_18->type = 2;
    l_18->x = a1->x;
    l_18->y = a1->y;
    l_18->z = a1->z;
    item_make_random((int)(unsigned short)((unsigned short)(unsigned char)*(signed char *)(D_0018661E + (rand() % 8))), &l_18->data.item);
}

void func_00060100(struct record *a1)
{
    struct record *l_18;

    l_18 = object_create_child(a1, 0, 107);
    l_18->type = 2;
    l_18->x = a1->x;
    l_18->y = a1->y;
    l_18->z = a1->z;
    item_make_random((int)(unsigned short)((unsigned short)(unsigned char)*(signed char *)(D_00186626 + (rand() % 4))), &l_18->data.item);
}

void func_00060184(struct record *a1, int a2)
{
    struct record *l_14;

    l_14 = object_create_child(a1, 0, 107);
    l_14->type = 2;
    l_14->x = a1->x;
    l_14->y = a1->y;
    l_14->z = a1->z;
    item_make_random((int)(unsigned short)*(short *)&a2, &l_14->data.item);
}

void func_000601ED(struct record *a1, int a2)
{
    struct item *l_14;

    l_14 = &a1->data.item;
    a2 = ((a2 * 10) + 50) / 100;
    if (a2 != 0) goto L60239;
    if (rand_range(1, 100) < 20) goto L6023B;
L60239:;
    goto L60242;
L6023B:;
    a2 = 1;
L60242:;
    if (l_14->condition <= a2) goto L6025F;
    l_14->condition -= a2;
    return;
L6025F:;
    item_break(a1);
}

void item_damage(struct record *a1, int a2)
{
    struct item *l_14;

    if (a1 == 0) return;
    l_14 = &a1->data.item;
    if (l_14->condition <= a2) goto L602AF;
    l_14->condition -= a2;
    return;
L602AF:;
    item_break(a1);
}

void magic_def_load(void)
{
    int l_18;

    l_18 = disk_open_data((int)D_00175927);
    func_000A00CB(l_18, (int)magic_def_count, 4);
    *(int *)magic_def = mc_malloc(filelength(l_18) - 4, (int)D_001758B8, 1201);
    func_000A00CB(l_18, *(int *)magic_def, (int)&*(signed char *)((char *)filelength(l_18) - 4));
    func_0009DEA7(l_18);
}

void shop_generate_stock(struct record *a1, int a2, int a3, int a4, int a5)
{
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    int l_10;
    struct record *l_C;

    if (a4 >= 21) return;
    if (a2 >= 2) goto L6107D;
    l_10 = 3;
    l_20 = ((int)D_00186639) + (a4 * 3);
    goto L610EB;
L6107D:;
    if (a2 >= 4) goto L6109A;
    l_10 = 12;
    l_20 = ((int)D_00186678) + (a4 * 12);
    goto L610EB;
L6109A:;
    if (a2 >= 11) goto L610B7;
    l_10 = 12;
    l_20 = ((int)D_00186774) + (a4 * 12);
    goto L610EB;
L610B7:;
    if (a2 >= 15) goto L610D6;
    l_10 = 5;
    l_20 = ((int)D_00186870) + (a4 * 5);
    goto L610EB;
L610D6:;
    l_10 = 11;
    l_20 = ((int)D_001868D9) + (a4 * 11);
L610EB:;
    l_1C = 0;
L610F2:;
    if (l_1C < l_10) goto L61104;
    goto L6110F;
L610FC:;
    l_1C++;
    goto L610F2;
L61104:;
    if (*(signed char *)((char *)(l_20 + l_1C)) != 0) goto L610FC;
L6110F:;
    l_1C = rand_range(0, l_1C - 1);
    l_14 = 100;
    l_18 = 1;
L6112B:;
    if (l_18 == 0) return;
    l_C = object_create_child(a1, 0, 107);
    l_C->type = 2;
    l_C->x = player_object->x;
    l_C->y = player_object->y;
    l_C->z = player_object->z;
    l_C->owner = a5;
    l_C->id = object_new_id(((unsigned)D_00195AC4->id) >> 16);
    if (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))) == 6) goto L611BD;
    if (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))) != 12) goto L611FB;
L611BD:;
    if (((int)(unsigned short)(player_character->flags & 1)) != 0) goto L611E6;
    item_make_random(6, &l_C->data.item);
    goto L611F6;
L611E6:;
    item_make_random(12, &l_C->data.item);
L611F6:;
    goto L61271;
L611FB:;
    if (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))) != 4) goto L6121F;
    item_make_magic(&a1->data.item, -1);
    goto L61271;
L6121F:;
    if (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))) != 7) goto L61257;
    item_make(7, (a3 + 3) / 5, &l_C->data.item);
    goto L61271;
L61257:;
    item_make_random((int)(unsigned short)((unsigned short)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))), &l_C->data.item);
L61271:;
    l_14 >>= 1;
    if ((rand() % 100) <= l_14) goto L61293;
    l_18 = 0;
L61293:;
    goto L6112B;
}

int func_000612A1(void)
{
    struct record *l_20;
    int l_1C;

    l_1C = 0;
    l_20 = *(struct record **)D_0019615F;
L612BE:;
    if (l_20 == 0) goto L61313;
    if (l_20->owner != *(int *)D_00195D54) goto L612DE;
    l_1C++;
L612DE:;
    l_20->x = player_object->x;
    l_20->y = player_object->y;
    l_20->z = player_object->z;
    l_20 = l_20->next;
    goto L612BE;
L61313:;
    return l_1C;
}

void func_00061326(struct record *a1)
{
    struct record *l_1C;
    int l_18;

    l_18 = rand_range(0, 42);
L61346:;
    if (*(int *)(D_00187966 + (l_18 << 2)) != 0) goto L61366;
    l_18 = rand_range(0, 42);
    goto L61346;
L61366:;
    l_1C = object_create_child(a1, 0, 0);
    l_1C->flags = 3;
    l_1C->type = 20;
    l_1C->soul_creature = l_18;
}

void func_00061398(struct record *a1)
{
    int l_24;
    int l_20;
    struct record *l_1C;
    struct item *l_18;

    l_24 = 0;
L613B0:;
    if ((current_building->quality + 1) > l_24) goto L613D0;
    return;
L613C8:;
    l_24++;
    goto L613B0;
L613D0:;
    l_20 = func_000614A9();
    l_1C = object_create_child(a1, 0, 107);
    l_18 = &l_1C->data.item;
    l_1C->type = 2;
    l_1C->flags |= 33;
    l_1C->x = player_object->x;
    l_1C->y = player_object->y;
    l_1C->z = player_object->z;
    item_make(1, 1, l_18);
    l_18->value = (int)(unsigned short)*(short *)(D_00180B42 + (l_20 * 109));
    l_18->stack_count = *(signed char *)&l_20;
    l_1C = object_create_child(l_1C, 0, 109);
    l_1C->type = 31;
    mc_memcpy(&l_1C->data.potion_recipe, ((int)potion_recipes) + (l_20 * 109), 109, (int)D_001758B8, 1330, 4);
    goto L613C8;
}

int func_000614A9(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = 0;
L614C5:;
    if (*(short *)(D_00180B42 + (l_20++ * 109)) == 0) goto L614E0;
    l_1C++;
    goto L614C5;
L614E0:;
    return rand_range(0, l_1C - 1);
}

void func_000614FB(struct record *a1)
{
    int l_24;
    int l_20;
    struct record *l_1C;
    struct item *l_18;

    l_20 = func_000614A9();
    l_1C = object_create_child(a1, 0, 107);
    l_18 = &l_1C->data.item;
    l_1C->type = 2;
    l_1C->flags |= 1;
    l_1C->x = player_object->x;
    l_1C->y = player_object->y;
    l_1C->z = player_object->z;
    item_make(1, 1, l_18);
    l_18->value = (int)(unsigned short)*(short *)(D_00180B42 + (l_20 * 109));
    l_18->stack_count = *(signed char *)&l_20;
    l_1C = object_create_child(l_1C, 0, 109);
    l_1C->type = 31;
    mc_memcpy(&l_1C->data.potion_recipe, ((int)potion_recipes) + (l_20 * 109), 109, (int)D_001758B8, 1366, 4);
}

void ai_creature_think(struct character *a1, struct record *a2, struct record *a3, int a4)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;
    struct monster_anim *l_10;
    struct record *l_C;

    a1->attack_timer -= *(int *)D_00195AB0;
    if (((unsigned)a1->attack_timer) <= 100000) goto L61619;
    a1->attack_timer = 0;
L61619:;
    if (((int)(unsigned char)*(signed char *)player_environment) != 3) goto L6164F;
    l_C = location_cell_at(a2->x, a2->z);
    if ((int)a2->parent == l_C) goto L6164F;
    object_reparent(l_C, a2);
L6164F:;
    *(int *)D_00199D74 = 0;
    *(int *)ai_monster_flags = (int)(unsigned short)*(short *)(monster_table_flags + (a1->race * 29));
    a2->yaw &= ~0xF800;
    monster_ambient_sound(a2, a1);
    if (((int)(unsigned short)(a1->flags & 16384)) != 0) goto L616AF;
    if (a1->race == 29) goto L616B1;
L616AF:;
    goto L616C1;
L616B1:;
    if (a1->health < a1->max_health) goto L616C3;
L616C1:;
    goto L616DE;
L616C3:;
    a1->flags |= 0x4000;
    monster_set_action(a2, 0, 57);
    return;
L616DE:;
    l_20 = func_000C808D(a2->x, a2->z, a3->x, a3->z);
    if ((a1->conditions & 0x1) == 0) goto L6171F;
    monster_set_action(a2, l_20, 48);
    return;
L6171F:;
    if (a3 != player_entity) goto L61733;
    l_14 = 90;
    goto L6173A;
L61733:;
    l_14 = 60;
L6173A:;
    if (a4 != *(int *)ai_los_index) goto L61764;
    if (collide_line_of_sight(a2, a3) == 0) goto L6175D;
    a1->flags |= 128;
    goto L61764;
L6175D:;
    a1->flags &= ~0x80;
L61764:;
    l_1C = ai_angle_diff(a2->yaw, l_20, (int)&l_18);
    l_24 = func_000C7FF4(a2->y - a3->y, func_000C7FD9(a2->x, a2->z, a3->x, a3->z));
    if (a2->wait_state == 0) goto L617C1;
    if (*(int *)trespassing == 0) goto L617C3;
L617C1:;
    goto L617D8;
L617C3:;
    monster_set_action(a2, l_20, 48);
    return;
L617D8:;
    if (((int)(unsigned short)(a1->flags & 32768)) == 0) goto L61825;
    if (l_1C < 128) goto L61810;
    monster_set_action(a2, l_20, 0);
    ai_turn_toward(a2, l_20);
    goto L61820;
L61810:;
    monster_set_action(a2, l_20, 48);
L61820:;
    return;
L61825:;
    l_10 = &a2->data.monster.anim;
    a1->flags &= ~0x100;
    if (a3 != player_entity) goto L6184E;
    if (a1->give_up_timer == 0) goto L61850;
L6184E:;
    goto L618A8;
L61850:;
    if (l_1C <= 512) goto L61892;
    if (ai_stealth_check(a1->race, (int)(unsigned short)(a1->flags & 256), l_24, (int)(unsigned short)(a1->flags & 8)) == 0) goto L618A6;
L61892:;
    if (ai_sees_through_illusion(a1->race) != 0) goto L618A8;
L618A6:;
    goto L618AA;
L618A8:;
    goto L618BF;
L618AA:;
    monster_set_action(a2, l_20, 48);
    goto L61C67;
L618BF:;
    a1->flags |= 264;
    if (a1->give_up_timer != 0) goto L618E1;
    a1->give_up_timer = 200;
L618E1:;
    if ((l_14 << 2) >= l_24) goto L618F5;
    if (l_24 < 2048) goto L618F7;
L618F5:;
    goto L6190C;
L618F7:;
    if (((int)(unsigned short)(a1->flags & 128)) != 0) goto L61911;
L6190C:;
    goto L61A85;
L61911:;
    if (((int)(unsigned short)(*(short *)(monster_table_flags + (a1->race * 29)) & 32)) == 0) goto L619D8;
    if (l_1C < 128) goto L6195B;
    ai_turn_toward(a2, l_20);
    monster_set_action(a2, l_20, 0);
    goto L619BE;
L6195B:;
    if (rand() >= 1000) goto L6197A;
    if (a1->action != 24) goto L6197C;
L6197A:;
    goto L6198E;
L6197C:;
    monster_set_action(a2, l_20, 24);
    goto L619BE;
L6198E:;
    if (l_10->anim_request != 24) goto L619AE;
    if (l_10->anim_current != 24) goto L619BE;
L619AE:;
    monster_set_action(a2, l_20, 48);
L619BE:;
    if (a1->give_up_timer == 0) goto L619D3;
    a1->give_up_timer--;
L619D3:;
    goto L61A80;
L619D8:;
    if (l_24 <= 256) goto L619EE;
    if (a1->magicka != 0) goto L619F0;
L619EE:;
    goto L619FC;
L619F0:;
    if (func_000629F8(a4) != 0) goto L619FE;
L619FC:;
    goto L61A5B;
L619FE:;
    if (l_1C < 128) goto L61A21;
    ai_turn_toward(a2, l_20);
    monster_set_action(a2, l_20, 0);
    goto L61A59;
L61A21:;
    if ((rand() % 40) != 0) goto L61A47;
    if (monster_cast_spell(a2, a3) != 0) goto L61A49;
L61A47:;
    goto L61A59;
L61A49:;
    monster_set_action(a2, l_20, 32);
L61A59:;
    goto L61A80;
L61A5B:;
    ai_move_toward_target(a2, &a2->data.character, a3, l_20, l_1C);
    monster_set_action(a2, l_20, 0);
L61A80:;
    goto L61C67;
L61A85:;
    if (((int)(unsigned short)(a1->flags & 256)) == 0) goto L61AA2;
    if (l_24 > l_14) goto L61AA4;
L61AA2:;
    goto L61ACE;
L61AA4:;
    ai_move_toward_target(a2, &a2->data.character, a3, l_20, l_1C);
    monster_set_action(a2, l_20, 0);
    goto L61C67;
L61ACE:;
    if (((int)(unsigned short)(a1->flags & 128)) == 0) goto L61C67;
    if (l_1C >= 128) goto L61C4F;
    if (l_10->anim_request != 8) goto L61B18;
    if (l_10->anim_request == 56) goto L61C4D;
L61B18:;
    if (a1->magicka == 0) goto L61B2E;
    if (a1->attack_timer == 0) goto L61B30;
L61B2E:;
    goto L61B3C;
L61B30:;
    if (ai_pick_touch_spell(a4) != 0) goto L61B3E;
L61B3C:;
    goto L61B4D;
L61B3E:;
    if (monster_cast_spell(a2, a3) != 0) goto L61B4F;
L61B4D:;
    goto L61B64;
L61B4F:;
    monster_set_action(a2, l_20, 32);
    goto L61C4D;
L61B64:;
    if ((rand() % a1->attributes[6]) >= ((a1->attributes[6] >> 3) + 6)) goto L61B95;
    if (a1->attack_timer == 0) goto L61B9A;
L61B95:;
    goto L61C16;
L61B9A:;
    if (monster_set_action(a2, l_20, 8) == 0) goto L61C14;
    a1->attack_timer = rand_range(1500, 3000);
    a1->attack_timer -= (player_character->level - 10) * 50;
    a1->attack_timer += (a1->reflexes - 2) * 450;
    if (((unsigned)a1->attack_timer) <= 100000) goto L61C14;
    a1->attack_timer = 1500;
L61C14:;
    goto L61C4D;
L61C16:;
    if (((int)(unsigned short)(a1->flags & 16384)) == 0) goto L61C3D;
    monster_set_action(a2, l_20, 59);
    goto L61C4D;
L61C3D:;
    monster_set_action(a2, l_20, 48);
L61C4D:;
    goto L61C67;
L61C4F:;
    ai_turn_toward(a2, l_20);
    monster_set_action(a2, l_20, 0);
L61C67:;
    if (a1->give_up_timer == 0) goto L61C7C;
    a1->give_up_timer--;
L61C7C:;
    if (((struct bf8_7_1 *)&D_001940DA)->f == 0) goto L61C8E;
    *(signed char *)D_001940DA &= 127;
    return;
L61C8E:;
    if (((int)(unsigned short)(l_10->anim_bits & 2)) == 0) return;
    l_10->anim_events &= 253;
    monster_shoot_arrow(a2, a3);
}

int ai_pick_target(struct record *a1, struct character *a2, int a3)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;
    int l_14;

    l_20 = 0;
    l_24 = 0;
    l_1C = l_24;
L61CE5:;
    if (l_24 < *(int *)creature_count) goto L61CFD;
    goto L61E55;
L61CF5:;
    l_24++;
    goto L61CE5;
L61CFD:;
    if (a3 == l_24) goto L61CF5;
    if ((signed char)a2->team == (signed char)ai_characters[l_24]->team) goto L61CF5;
    ai_characters[l_24]->target_score = 0;
    if ((int)ai_characters[l_24]->target != 0) goto L61D5A;
    ai_characters[l_24]->target_score += 5;
L61D5A:;
    if (collide_line_of_sight(a1, ai_entities[l_24]) == 0) goto L61D85;
    ai_characters[l_24]->target_score += 20;
L61D85:;
    l_14 = func_000C7FF4(a1->y - ai_entities[l_24]->y, func_000C7FD9(a1->x, a1->z, ai_entities[l_24]->x, ai_entities[l_24]->z));
    l_14 = l_14 / 128;
    l_14 = 30 - l_14;
    if (l_14 >= 0) goto L61E01;
    l_14 = 0;
L61E01:;
    ai_characters[l_24]->target_score += *(signed char *)&l_14;
    if (((int)(unsigned char)(signed char)ai_characters[l_24]->target_score) <= l_1C) goto L61E50;
    l_1C = (int)(unsigned char)(signed char)ai_characters[l_24]->target_score;
    l_18 = l_24;
L61E50:;
    goto L61CF5;
L61E55:;
    if (l_1C >= 8) goto L61E70;
    if (((int)(unsigned short)((short)a2->flags & 2)) != 0) goto L61E72;
L61E70:;
    goto L61E7B;
L61E72:;
    return -1;
L61E7B:;
    if (l_1C != 0) goto L61E8A;
    return 0;
L61E8A:;
    return l_18;
}

void ai_update_creatures(void)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_20 = -1;
    if (*(signed char *)D_00187CA8 == 0) return;
    if (*(int *)creature_count == 0) return;
    if (*(int *)creature_count != 1) goto L61EED;
    if (((int)(unsigned short)(D_00190504[0]->data.character.flags & 2)) != 0) goto L61EEF;
L61EED:;
    goto L61EFE;
L61EEF:;
    object_delete(D_00190504[0]);
    return;
L61EFE:;
    player_entity->yaw = player_object->yaw;
    player_entity->angle_x = player_object->angle_x;
    D_00190504[(*(int *)creature_count)++] = player_entity;
    l_28 = 0;
L61F45:;
    if (l_28 < *(int *)creature_count) goto L61F5A;
    goto L61FAB;
L61F52:;
    l_28++;
    goto L61F45;
L61F5A:;
    if ((*(int *)creature_count - 1) == l_28) goto L61F76;
    func_0002682B(D_00190504[l_28]);
L61F76:;
    ai_entities[l_28] = (struct record *)((int)D_00190504[l_28]);
    ai_characters[l_28] = (struct character *)((int)((char *)ai_entities[l_28] + 71));
    goto L61F52;
L61FAB:;
    l_28 = 0;
L61FB2:;
    if ((*(int *)creature_count - 1) > l_28) goto L61FCA;
    goto L620B0;
L61FC2:;
    l_28++;
    goto L61FB2;
L61FCA:;
    l_18 = 1132;
    if (ai_characters[l_28]->target == 0) goto L6200A;
    if (ai_characters[l_28]->target == 0) goto L62008;
    if (((unsigned)(((unsigned)*(int *)((char *)l_18)) % 200)) < 4) goto L6200A;
L62008:;
    goto L6200C;
L6200A:;
    goto L62027;
L6200C:;
    if (ai_characters[l_28]->target->type != 44) goto L62029;
L62027:;
    goto L62044;
L62029:;
    if (ai_characters[l_28]->target->type != 34) goto L620AB;
L62044:;
    l_24 = ai_pick_target(ai_entities[l_28], ai_characters[l_28], l_28);
    if (l_24 != (-1)) goto L62075;
    l_20 = l_28;
    goto L620AB;
L62075:;
    ai_characters[l_28]->target = (struct record *)((int)ai_entities[l_24]);
    ai_characters[l_24]->target = (struct record *)((int)ai_entities[l_28]);
L620AB:;
    goto L61FC2;
L620B0:;
    (*(int *)creature_count)--;
    *(int *)ai_los_index = (*(int *)ai_los_index + 1) % *(int *)creature_count;
    l_28 = 0;
L620D7:;
    if (l_28 < *(int *)creature_count) goto L620EF;
    goto L62203;
L620E7:;
    l_28++;
    goto L620D7;
L620EF:;
    if (ai_entities[l_28]->type != 18) goto L620E7;
    monster_ambient_sound(ai_entities[l_28], ai_characters[l_28]);
    if (((int)(unsigned short)(ai_characters[l_28]->flags & 32)) == 0) goto L6217C;
    damage_knockback_move(ai_entities[l_28], ai_characters[l_28]);
    monster_set_action(ai_entities[l_28], 0, 16);
    goto L621FE;
L6217C:;
    if (ai_characters[l_28]->target == 0) goto L621BD;
    ai_creature_think(ai_characters[l_28], ai_entities[l_28], *(struct record **)((char *)ai_characters[l_28] + 112), l_28);
L621BD:;
    l_1C = (int)ai_entities[l_28] + 705;
    if (((int)(unsigned short)(*(short *)((char *)l_1C + 16) & 1)) == 0) goto L621FE;
    *(signed char *)((char *)l_1C + 16) &= 254;
    weapon_melee_strike(D_00190504[l_28]);
L621FE:;
    goto L620E7;
L62203:;
    monster_apply_gravity();
    if (l_20 == (-1)) return;
    object_delete(ai_entities[l_20]);
}

int ai_turn_toward(struct record *a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = ai_angle_diff(a1->yaw, a2, (int)&l_18);
    if (l_1C >= 64) goto L6226A;
    a1->yaw = a2;
    return 0;
L6226A:;
    a1->yaw += l_18 << 6;
    return 1;
}

void func_0006228A(struct record *a1, int a2, int a3)
{
    int l_14;
    int l_10;

    a3 = (a3 + 1024) & 2047;
    l_14 = ai_angle_diff(a1->yaw, a3, (int)&l_10);
    if (l_14 >= 32) goto L622D6;
    a1->yaw = a3;
    return;
L622D6:;
    a1->yaw += l_10 << 5;
}
