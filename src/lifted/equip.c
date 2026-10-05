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
extern unsigned char player_environment;
extern char item_templates[];
extern char potion_recipes[];
extern char D_00180B42[];
extern char monster_table_flags[];
extern char item_group_templates[];
extern char D_00185FFC[];
extern char D_0018606B[];
extern int D_00186483[];
extern signed char D_0018654F[];
extern signed char D_00186578[];
extern char D_00186583[];
extern signed char D_00186589[];
extern signed char D_00186591[];
extern signed char D_0018659B[];
extern signed char D_0018659F[];
extern signed char D_001865F2[];
extern signed char D_00186605[];
extern signed char D_00186610[];
extern signed char D_0018661E[];
extern signed char D_00186626[];
extern char D_00186639[];
extern char D_00186678[];
extern char D_00186774[];
extern char D_00186870[];
extern char D_001868D9[];
extern char D_001869C0[];
extern char D_001869D4[];
extern char D_001869EA[];
extern char D_001869FE[];
extern char monster_soul_values[];
extern signed char D_00187CA8;
extern char book_list[];
extern char D_0018E046[];
extern signed char text_buffer[];
extern struct record *creature_list[];
extern signed char D_001940D6;
extern unsigned char D_001940D7;
extern signed char D_001940DA;
extern struct character *ai_characters[];
extern struct record *ai_entities[];
extern struct building *current_building;
extern struct record *player_entity;
extern struct record *player_object;
extern int frame_ticks;
extern struct record *location_object;
extern int creature_count;
extern struct location *current_location;
extern struct character *player_character;
extern char scratch_buffer[];
extern int trespassing;
extern int ai_los_index;
extern char magic_def_count[];
extern int magic_def;
extern char D_00195D54[];
extern int ai_monster_flags;
extern short D_00195DC4;
extern short painting_subject_text;
extern short painting_adjective_text;
extern short painting_prefix1_text;
extern short painting_prefix2_text;
extern short book_count;
extern struct record *D_0019615F;
extern signed char forced_material;
extern int D_00199D74;

extern int collide_line_of_sight(struct record *, struct record *);
extern int item_add_to_container(struct record *, int, int, int);
extern int armor_image_for_type(int, unsigned short);
extern int monster_set_action(struct record *, int, int);
extern int ai_pick_ranged_spell(int);
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
extern int close();
extern int lseek();
extern int mc_malloc();
extern int read();
extern int mc_set_location(int, int);
extern int mc_sprintf(int, ...);
extern int mc_memcpy();
extern int memchr();
extern int filelength();
extern int xn_math_approx_dist2d();
extern int xn_math_approx_hypot();
extern int xn_math_angle_to_point();
extern int xn_draw_image();
extern void place_settle_creature(struct record *);
extern void damage_knockback_move(struct record *, struct character *);
extern void msgbox_show_rsc(int, int);
extern void item_init_from_template(unsigned short, short, short, struct item *);
extern void item_make_random(unsigned short, struct item *);
extern void loot_generate(int, struct record *, int, unsigned short);
extern void item_make_magic(struct item *, int);
extern void item_make_artifact(struct item *, int);
extern void monster_shoot_arrow(struct record *, struct record *);
extern void monster_ambient_sound(struct record *, struct character *);
extern void ai_move_toward_target(struct record *, struct character *, struct record *, int, int);
extern void monster_apply_gravity(void);
extern void weapon_melee_strike(struct record *);
extern void item_break(struct record *);
int pick_random_byte(int);
int book_pick_random(int);
int potion_random_recipe(void);
int ai_pick_target(struct record *, struct character *, int);
int ai_turn_toward(struct record *, int);
void item_make(int, int, struct item *);
void item_roll_dye(struct item *);
void shelf_stock_books(struct record *, int);
void soul_trap_add_soul(struct record *);
void loot_add_potion(struct record *);
void ai_creature_think(struct character *, struct record *, struct record *, int);
#pragma aux mc_set_location parm routine [];

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
    item_init_from_template((int)(unsigned short)*(short *)((char *)(int)(*(char **)(item_group_templates + (a1 << 2)) + (a2 * 2))), (int)(short)*(short *)&a1, (int)(short)*(short *)&a2, a3);
}
}

void item_set_race_image(struct item *a1, int a2)
{
    if (a2 > 7) {
        a1->inventory_image += ((unsigned short)(unsigned char)D_00186589[player_character->original_race]) << 7;
        return;
    }
    a1->inventory_image += ((unsigned short)(unsigned char)D_00186589[a2]) << 7;
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
        break;
    case 6:
        l_1C = 13;
        l_18 = (int)D_0018606B;
        break;
    case 2:
        a1->inventory_image += rand_range(0, (int)&*(signed char *)((char *)(a1->variants) - 1));
        return;
    default:
        l_1C = 0;
    }
    if (a1->variants >= 2) {
        l_24 = rand_range(0, (int)&*(signed char *)((char *)(a1->variants) - 1));
        a1->inventory_image += l_24;
    }
    item_roll_dye(a1);
    if (a1->index != l_1C) if (a1->index != (l_1C + 1)) return;
    a1->inventory_image++;
}

void item_next_clothing_style(struct item *a1)
{
    int l_20;
    int l_1C;
    int l_18;

    if (a1->group == 12) {
        l_18 = (int)D_00185FFC;
        l_1C = 9;
    } else {
        l_18 = (int)D_0018606B;
        l_1C = 13;
    }
    l_20 = (int)(unsigned short)(a1->inventory_image & 127);
    if (*(signed char *)((char *)(l_18 + l_20)) == 0) return;
    if (*(signed char *)((char *)(l_18 + l_20) + 1) == *(signed char *)((char *)(l_18 + l_20))) {
        a1->inventory_image++;
        return;
    }
    l_20--;
    while (*(signed char *)((char *)(l_18 + l_20) + 1) == *(signed char *)((char *)(l_18 + l_20))) {
        l_20--;
        a1->inventory_image--;
    }
    if (a1->index != l_1C) if (a1->index != (l_1C + 1)) return;
    a1->inventory_image++;
}

void func_0005E7FC(struct item *a1)
{
    int l_20;
    int l_1C;
    int l_18;

    if (a1->group == 12) {
        l_18 = (int)D_00185FFC;
    } else {
        l_18 = (int)D_0018606B;
    }
    l_20 = (int)(unsigned short)(a1->inventory_image & 127);
    l_20--;
    while (*(signed char *)((char *)(l_18 + l_20) + 1) == *(signed char *)((char *)(l_18 + l_20))) {
        l_20--;
        a1->inventory_image--;
    }
}

void item_roll_material(struct item *a1)
{
    int l_28;
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = ((a1->enchantments[0].type != (-1)) ? 1 : 0);
    a1->material = 0;
    if (a1->group == 3 && a1->index == 18) return;
    l_28 = rand_range(0, 255);
    if (l_24 != 0) {
        l_28 += 60;
        if (l_28 > 255) l_28 = 255;
    }
    l_18 = player_character->level - 10;
    if (l_18 < 0) {
        l_18 <<= 2;
    } else {
        l_18 <<= 1;
    }
    l_28 += l_18;
    if (((int)player_environment) == 2) {
        l_28 -= (14 - current_building->quality) * 2;
    }
    if (l_28 < 0) {
        l_28 = 0;
    } else if (l_28 > 256) {
        l_28 = 256;
    }
    if (forced_material != 0) {
        a1->material = forced_material - 1;
        forced_material = 0;
    } else {
        while (((int)(unsigned char)D_00186591[a1->material]) < l_28) {
            l_28 -= (int)(unsigned char)D_00186591[a1->material];
            a1->material++;
        }
    }
    a1->value = a1->value * (((int)(short)*(short *)(D_001869C0 + (a1->material * 2))) * 3);
    a1->weight = ((unsigned)(((int)(short)*(short *)(D_001869D4 + (a1->material * 2))) * a1->weight)) >> 2;
    a1->condition = (a1->max_condition = (a1->max_condition * ((int)(short)*(short *)(D_001869EA + (a1->material * 2)))) >> 2);
    a1->enchant_points = (a1->enchant_points * ((int)(short)*(short *)(D_001869FE + (a1->material * 2)))) >> 2;
    a1->color = a1->material + 16;
}

void item_roll_armor_type(struct item *a1)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_24 = rand_range(1, 100);
    if (l_24 < 70 && forced_material == 0) {
        a1->armor_type = 0;
        a1->weight >>= 1;
    } else if (l_24 < 90 && forced_material == 0) {
        a1->armor_type = 1;
    } else {
        a1->armor_type = 2;
    }
    if (a1->armor_type == 2) {
        l_1C = rand_range(0, 255);
        if (a1->enchantments[0].type != (-1)) {
            l_1C += 60;
            if (l_1C > 255) l_1C = 255;
        }
        l_18 = player_character->level - 10;
        if (l_18 < 0) {
            l_18 <<= 2;
        } else {
            l_18 <<= 1;
        }
        l_1C += l_18;
        if (((int)player_environment) == 2) {
            l_1C -= (14 - current_building->quality) * 2;
        }
        if (l_1C < 0) {
            l_1C = 0;
        } else if (l_1C > 256) {
            l_1C = 256;
        }
        if (forced_material != 0) {
            a1->material = forced_material - 1;
            forced_material = 0;
        } else {
            while (((int)(unsigned char)D_00186591[a1->material]) < l_1C) {
                l_1C -= (int)(unsigned char)D_00186591[a1->material];
                a1->material++;
            }
        }
        a1->value = a1->value * (((int)(short)*(short *)(D_001869C0 + (a1->material * 2))) * 3);
        a1->weight = ((unsigned)(((int)(short)*(short *)(D_001869D4 + (a1->material * 2))) * a1->weight)) >> 2;
        a1->condition = (a1->max_condition = (a1->max_condition * ((int)(short)*(short *)(D_001869EA + (a1->material * 2)))) >> 2);
        a1->enchant_points = (a1->enchant_points * ((int)(short)*(short *)(D_001869FE + (a1->material * 2)))) >> 2;
        a1->color = a1->material + 16;
    } else {
        a1->value = a1->value * ((int)&*(signed char *)((char *)(a1->armor_type) + 1));
    }
    l_20 = armor_image_for_type(a1->armor_type, a1->index);
    if (l_20 == (-1)) return;
    a1->inventory_image = l_20 + (a1->inventory_image & -128);
}

void item_roll_dye(struct item *a1)
{
    int l_18;

    l_18 = rand_range(0, 100);
    if (l_18 < 25) return;
    a1->color = rand_range(1, 10);
    if (a1->group == 6) {
        if (a1->index == 10 || a1->index == 11) {
            a1->color = D_0018659B[rand() & 3];
        } else if (a1->index == 4) {
            a1->color = D_0018659F[rand() & 3];
        }
        return;
    }
    if (a1->group != 12 || a1->index != 8) return;
    a1->color = D_0018659B[rand() & 3];
}

void shelf_stock_items(struct record *a1, int a2, int a3)
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

    l_20 = D_00186483[a2];
    if (l_20 == 0) return;
    while (((int)(unsigned char)*(signed char *)((char *)l_20)) != 255) {
        if (((int)(unsigned char)*(signed char *)((char *)l_20)) == 6 && ((int)(unsigned short)(player_character->flags & 1)) != 0) {
            *(signed char *)((char *)l_20) = 12;
        }
        if (((int)(unsigned char)*(signed char *)((char *)l_20)) == 12 && ((int)(unsigned short)(player_character->flags & 1)) == 0) {
            *(signed char *)((char *)l_20) = 6;
        }
        l_1C = *(int *)(item_group_templates + (((int)(unsigned char)*(signed char *)((char *)l_20)) << 2));
        if (l_1C != 0 && ((int)(unsigned char)*(signed char *)((char *)l_20)) != 8 && ((int)(unsigned char)*(signed char *)((char *)l_20)) != 1) {
            if (((struct bf8_1_1 *)&D_001940D7)->f != 0 && (((int)(unsigned char)*(signed char *)((char *)l_20)) == 4 || ((int)(unsigned char)*(signed char *)((char *)l_20)) == 5)) {
            } else {
                l_28 = (int)(unsigned char)*(signed char *)((char *)l_20 + 1);
                l_24 = 0;
                if (((int)(unsigned char)*(signed char *)((char *)l_20)) == 7) {
                    shelf_stock_books(a1, a3);
                } else {
                    while (((int)(short)*(short *)((char *)l_1C)) != (-1)) {
                        l_18 = ((int)item_templates) + (((int)(short)*(short *)((char *)l_1C)) * 48);
                        l_30 = (((21 - ((int)(unsigned char)*(signed char *)((char *)l_18 + 40))) * 5) * l_28) / 100;
                        if (((int)(unsigned char)*(signed char *)((char *)l_18 + 40)) <= a3 && rand_range(1, 100) <= l_30) {
                            l_10 = object_create_child(a1, 0, 107);
                            l_10->type = 2;
                            l_10->x = player_object->x;
                            l_10->y = player_object->y;
                            l_10->z = player_object->z;
                            l_10->image2 = 998;
                            l_10->id = object_new_id(((unsigned)location_object->id) >> 16);
                            if (a2 >= 0) l_10->flags |= 32;
                            l_14 = &l_10->data.item;
                            item_make((int)(unsigned char)*(signed char *)((char *)l_20), l_24, l_14);
                            if (l_14->group != 3 || l_14->index != 18) l_10->image2 = 0;
                        }
                        (*(char (**)[2])&l_1C)++;
                        l_24++;
                    }
                }
            }
        }
        l_20 += 2;
    }
}

int func_0005F0A0(int a1)
{
    {
        int l_20;

        if (a1 == 12 || a1 == 6 || a1 == 2) {
            l_20 = 1;
        } else {
            l_20 = 0;
        }
        return l_20;
    }
}

void shelf_stock_books(struct record *a1, int a2)
{
    int l_18;
    struct record *l_14;

    a2 = (a2 + 3) / 5;
    if (a2 >= 4) a2--;
    a2++;
    for (l_18 = 0; l_18 <= a2; l_18++) {
        l_14 = object_create_child(a1, 0, 107);
        l_14->type = 2;
        l_14->x = player_object->x;
        l_14->y = player_object->y;
        l_14->z = player_object->z;
        l_14->id = object_new_id(((unsigned)location_object->id) >> 16);
        l_14->flags |= 32;
        item_make(7, a2, &l_14->data.item);
    }
}

void shop_stock_magic(struct record *a1, int a2, int a3, int a4)
{
    int l_14;
    struct record *l_10;
    int l_C;

    if (a3 != 0) a3 = 32;
    for (l_14 = 0; ((current_building->quality >> 1) + 1) > l_14; l_14++) {
        l_10 = object_create_child(a1, 0, 107);
        l_10->type = 2;
        l_10->id = object_new_id(((unsigned)location_object->id) >> 16);
        item_make_magic(&l_10->data.item, -1);
        l_10->x = player_object->x;
        l_10->y = player_object->y;
        l_10->z = player_object->z;
        l_10->flags |= a3;
        if (a3 != 0) l_10->data.item.item_flags |= 32;
    }
    l_10 = object_create_child(a1, 0, 107);
    l_10->type = 2;
    l_10->id = object_new_id(((unsigned)location_object->id) >> 16);
    l_10->x = player_object->x;
    l_10->y = player_object->y;
    l_10->z = player_object->z;
    l_10->flags |= 32;
    item_make(27, 0, &l_10->data.item);
    if (a4 == 0) return;
    for (l_14 = 0; ((current_building->quality >> 1) + 1) > l_14; l_14++) {
        l_10 = object_create_child(a1, 0, 107);
        l_10->type = 2;
        l_10->id = object_new_id(((unsigned)location_object->id) >> 16);
        l_10->x = player_object->x;
        l_10->y = player_object->y;
        l_10->z = player_object->z;
        l_10->flags |= 32;
        item_make(27, 1, &l_10->data.item);
        if (rand_range(1, 100) < 25) {
            soul_trap_add_soul(l_10);
            l_10->data.item.value = *(int *)(monster_soul_values + (l_10->children->soul_creature << 2)) + 5000;
        } else {
            l_10->data.item.value = 5000;
        }
    }
}

void shop_stock_soul_traps(struct record *a1)
{
    int l_20;
    struct record *l_1C;
    int l_18;

    for (l_20 = 0; ((current_building->quality >> 1) + 1) > l_20; l_20++) {
        l_1C = object_create_child(a1, 0, 107);
        l_1C->type = 2;
        l_1C->id = object_new_id(((unsigned)location_object->id) >> 16);
        l_1C->x = player_object->x;
        l_1C->y = player_object->y;
        l_1C->z = player_object->z;
        l_1C->flags |= 32;
        item_make(27, 1, &l_1C->data.item);
        if (rand_range(1, 100) < 25) {
            soul_trap_add_soul(l_1C);
            l_1C->data.item.value = *(int *)(monster_soul_values + (l_1C->children->soul_creature << 2)) + 5000;
        } else {
            l_1C->data.item.value = 5000;
        }
    }
}

void painting_draw(void)
{
    int l_20;
    int l_1C;
    int l_18;

    mc_set_location(625, (int)D_001758B8);
    mc_sprintf((int)text_buffer, (int)D_001758E2, (((int)(unsigned short)D_00195DC4) >> 3) + 97);
    disk_read_file((int)text_buffer, *(int *)scratch_buffer);
    l_18 = *(int *)scratch_buffer;
    l_20 = 0;
    l_1C = (int)(unsigned short)(D_00195DC4 & 7);
    while (l_20 < l_1C) {
        l_18 = (((int)(unsigned short)*(short *)((char *)l_18 + 10)) + l_18) + 12;
        l_20++;
    }
    xn_draw_image(160 - (((int)(unsigned short)*(short *)((char *)l_18 + 4)) >> 1), 50, (int)(unsigned short)*(short *)((char *)l_18 + 4), (int)(unsigned short)*(short *)((char *)l_18 + 6), l_18 + 12);
}

void item_info_painting(struct item *a1)
{
    int l_1C;
    int l_18;

    l_18 = rand();
    D_00195DC4 = rand(srand((int)(unsigned short)(short)a1->message)) % 180;
    l_1C = disk_open_data((int)D_001758EE);
    lseek(l_1C, ((int)(unsigned short)D_00195DC4) * 40, 0);
    read(l_1C, *(int *)scratch_buffer, 40);
    close(l_1C);
    painting_subject_text = pick_random_byte(*(int *)scratch_buffer) + 6100;
    painting_adjective_text = pick_random_byte(*(int *)scratch_buffer + 10) + 6200;
    painting_prefix1_text = pick_random_byte((int)(*(char **)scratch_buffer + 20)) + 6300;
    painting_prefix2_text = (short)pick_random_byte((int)(*(char **)scratch_buffer + 30)) + 6400;
    D_001940D6 |= 32;
    msgbox_show_rsc(250, 1);
    srand(l_18);
}

int pick_random_byte(int a1)
{
    int l_1C;

    l_1C = 0;
    while (l_1C < 10 && ((int)(unsigned char)*(signed char *)((char *)(a1 + l_1C))) != 255) l_1C++;
    if (l_1C == 1) return (int)(unsigned char)*(signed char *)((char *)a1);
    return (int)(unsigned char)*(signed char *)((char *)(int)((char *)a1 + rand_range(0, l_1C - 1)));
}

void item_init_book(struct item *a1, int a2)
{
    int l_1C;
    int l_18;
    int l_14;

    a1->message = *(short *)(book_list + (book_pick_random(a2) << 2));
    l_1C = *(int *)scratch_buffer;
    mc_set_location(674, (int)D_001758B8);
    mc_sprintf((int)text_buffer, (int)D_001758F8, (int)(unsigned short)(short)a1->message);
    l_18 = disk_open_data((int)text_buffer);
    read(l_18, l_1C, 234);
    close(l_18);
    l_14 = rand();
    srand(*(int *)((char *)l_1C));
    a1->value = rand_range(300, 800);
    srand(l_14);
}

int book_pick_random(int a1)
{
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    l_20 = l_24;
    for (; ((int)(unsigned short)book_count) > l_24; l_24++) {
        if (((int)(unsigned short)*(short *)(D_0018E046 + (l_24 << 2))) <= a1) l_20++;
    }
    if (l_20 == 0) l_20 = 1;
    l_1C = rand() % l_20;
    l_24 = 0;
    while (l_1C >= 0) {
        if (((int)(unsigned short)*(short *)(D_0018E046 + (l_24 << 2))) <= a1) l_1C--;
        l_24++;
    }
    return l_24 - 1;
}

int equip_hiding_capacity(int a1)
{
    int l_24;
    int l_20;
    struct item *l_1C;

    l_24 = 0;
    l_20 = l_24;
    for (; l_24 < 27; l_24++) {
        if (player_character->equipped[l_24] == 0) continue;
        l_1C = &player_character->equipped[l_24]->data.item;
        if (l_1C->group == 1 && memchr((int)D_00186583, l_1C->index, 6) != 0) {
            l_20 += 10;
            continue;
        }
        if (l_1C->group == 2 && a1 != 0) {
            l_20 += (int)(unsigned char)D_00186578[l_1C->index];
        } else if (l_1C->group == 6 || l_1C->group == 12) {
            l_20 += (int)(unsigned char)D_0018654F[l_1C->index];
        }
    }
    return l_20;
}

void loot_fill_container(struct record *a1)
{
    int l_20;
    int l_1C;
    struct building *l_18;

    if (((int)player_environment) != 1) {
        l_20 = ((int)(unsigned char)D_001865F2[current_location->kind]) - 1;
        while (a1->children == 0) {
            loot_generate(l_20, a1, player_character->level, (int)(unsigned short)(player_character->flags & 1));
        }
        if (l_20 < 9 || l_20 > 14) return;
        if (rand_range(1, 100) <= ((int)(unsigned char)D_00186610[l_20])) {
            a1 = object_create_child(a1, 0, 107);
            a1->type = 2;
            item_make(27, 8, &a1->data.item);
        }
        if (rand_range(1, 100) < 4) loot_add_potion(a1);
        if (rand_range(1, 100) < 2) item_add_to_container(a1, 27, 4, 0);
        return;
    }
    l_18 = object_building(a1);
    if (l_18 == 0) {
        l_20 = 0;
    } else {
        l_20 = l_18->type;
    }
    while (a1->children == 0) {
        loot_generate(((int)(unsigned char)D_00186605[l_20]) - 1, a1, player_character->level, (int)(unsigned short)(player_character->flags & 1));
    }
}

void loot_add_gold(struct record *a1, int a2, int a3)
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

void loot_add_ingredient(struct record *a1)
{
    struct record *l_18;

    l_18 = object_create_child(a1, 0, 107);
    l_18->type = 2;
    l_18->x = a1->x;
    l_18->y = a1->y;
    l_18->z = a1->z;
    item_make_random((int)(unsigned short)((unsigned short)(unsigned char)D_0018661E[rand() % 8]), &l_18->data.item);
}

void loot_add_misc_item(struct record *a1)
{
    struct record *l_18;

    l_18 = object_create_child(a1, 0, 107);
    l_18->type = 2;
    l_18->x = a1->x;
    l_18->y = a1->y;
    l_18->z = a1->z;
    item_make_random((int)(unsigned short)((unsigned short)(unsigned char)D_00186626[rand() % 4]), &l_18->data.item);
}

void loot_add_item_of_group(struct record *a1, int a2)
{
    struct record *l_14;

    l_14 = object_create_child(a1, 0, 107);
    l_14->type = 2;
    l_14->x = a1->x;
    l_14->y = a1->y;
    l_14->z = a1->z;
    item_make_random((int)(unsigned short)*(short *)&a2, &l_14->data.item);
}

void item_wear_from_hit(struct record *a1, int a2)
{
    struct item *l_14;

    l_14 = &a1->data.item;
    a2 = ((a2 * 10) + 50) / 100;
    if (a2 == 0 && rand_range(1, 100) < 20) a2 = 1;
    if (l_14->condition > a2) {
        l_14->condition -= a2;
        return;
    }
    item_break(a1);
}

void item_damage(struct record *a1, int a2)
{
    struct item *l_14;

    if (a1 == 0) return;
    l_14 = &a1->data.item;
    if (l_14->condition > a2) {
        l_14->condition -= a2;
        return;
    }
    item_break(a1);
}

void magic_def_load(void)
{
    int l_18;

    l_18 = disk_open_data((int)D_00175927);
    read(l_18, (int)magic_def_count, 4);
    magic_def = mc_malloc(filelength(l_18) - 4, (int)D_001758B8, 1201);
    read(l_18, magic_def, (int)&*(signed char *)((char *)filelength(l_18) - 4));
    close(l_18);
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
    if (a2 < 2) {
        l_10 = 3;
        l_20 = ((int)D_00186639) + (a4 * 3);
    } else if (a2 < 4) {
        l_10 = 12;
        l_20 = ((int)D_00186678) + (a4 * 12);
    } else if (a2 < 11) {
        l_10 = 12;
        l_20 = ((int)D_00186774) + (a4 * 12);
    } else if (a2 < 15) {
        l_10 = 5;
        l_20 = ((int)D_00186870) + (a4 * 5);
    } else {
        l_10 = 11;
        l_20 = ((int)D_001868D9) + (a4 * 11);
    }
    for (l_1C = 0; l_1C < l_10; l_1C++) {
        if (*(signed char *)((char *)(l_20 + l_1C)) == 0) break;
    }
    l_1C = rand_range(0, l_1C - 1);
    l_14 = 100;
    l_18 = 1;
    while (l_18 != 0) {
        l_C = object_create_child(a1, 0, 107);
        l_C->type = 2;
        l_C->x = player_object->x;
        l_C->y = player_object->y;
        l_C->z = player_object->z;
        l_C->owner = a5;
        l_C->id = object_new_id(((unsigned)location_object->id) >> 16);
        if (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))) == 6 || ((int)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))) == 12) {
            if (((int)(unsigned short)(player_character->flags & 1)) == 0) {
                item_make_random(6, &l_C->data.item);
            } else {
                item_make_random(12, &l_C->data.item);
            }
        } else if (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))) == 4) {
            item_make_magic(&a1->data.item, -1);
        } else if (((int)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))) == 7) {
            item_make(7, (a3 + 3) / 5, &l_C->data.item);
        } else {
            item_make_random((int)(unsigned short)((unsigned short)(unsigned char)*(signed char *)((char *)(l_20 + l_1C))), &l_C->data.item);
        }
        l_14 >>= 1;
        if ((rand() % 100) > l_14) l_18 = 0;
    }
}

int func_000612A1(void)
{
    struct record *l_20;
    int l_1C;

    l_1C = 0;
    l_20 = D_0019615F;
    while (l_20 != 0) {
        if (l_20->owner == *(int *)D_00195D54) l_1C++;
        l_20->x = player_object->x;
        l_20->y = player_object->y;
        l_20->z = player_object->z;
        l_20 = l_20->next;
    }
    return l_1C;
}

void soul_trap_add_soul(struct record *a1)
{
    struct record *l_1C;
    int l_18;

    l_18 = rand_range(0, 42);
    while (*(int *)(monster_soul_values + (l_18 << 2)) == 0) l_18 = rand_range(0, 42);
    l_1C = object_create_child(a1, 0, 0);
    l_1C->flags = 3;
    l_1C->type = 20;
    l_1C->soul_creature = l_18;
}

void shop_stock_potions(struct record *a1)
{
    int l_24;
    int l_20;
    struct record *l_1C;
    struct item *l_18;

    for (l_24 = 0; (current_building->quality + 1) > l_24; l_24++) {
        l_20 = potion_random_recipe();
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
    }
}

int potion_random_recipe(void)
{
    int l_20;
    int l_1C;

    l_20 = 0;
    l_1C = 0;
    while (*(short *)(D_00180B42 + (l_20++ * 109)) != 0) l_1C++;
    return rand_range(0, l_1C - 1);
}

void loot_add_potion(struct record *a1)
{
    int l_24;
    int l_20;
    struct record *l_1C;
    struct item *l_18;

    l_20 = potion_random_recipe();
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

    a1->attack_timer -= frame_ticks;
    if (((unsigned)a1->attack_timer) > 100000) a1->attack_timer = 0;
    if (((int)player_environment) == 3) {
        l_C = location_cell_at(a2->x, a2->z);
        if ((int)a2->parent != l_C) object_reparent(l_C, a2);
    }
    D_00199D74 = 0;
    ai_monster_flags = (int)(unsigned short)*(short *)(monster_table_flags + (a1->race * 29));
    a2->yaw &= ~0xF800;
    monster_ambient_sound(a2, a1);
    if (((int)(unsigned short)(a1->flags & 16384)) == 0 && a1->race == 29 && a1->health < a1->max_health) {
        a1->flags |= 0x4000;
        monster_set_action(a2, 0, 57);
        return;
    }
    l_20 = xn_math_angle_to_point(a2->x, a2->z, a3->x, a3->z);
    if ((a1->conditions & 0x1) != 0) {
        monster_set_action(a2, l_20, 48);
        return;
    }
    if (a3 == player_entity) {
        l_14 = 90;
    } else {
        l_14 = 60;
    }
    if (a4 == ai_los_index) {
        if (collide_line_of_sight(a2, a3) != 0) {
            a1->flags |= 128;
        } else {
            a1->flags &= ~0x80;
        }
    }
    l_1C = ai_angle_diff(a2->yaw, l_20, (int)&l_18);
    l_24 = xn_math_approx_hypot(a2->y - a3->y, xn_math_approx_dist2d(a2->x, a2->z, a3->x, a3->z));
    if (a2->wait_state != 0 && trespassing == 0) {
        monster_set_action(a2, l_20, 48);
        return;
    }
    if (((int)(unsigned short)(a1->flags & 32768)) != 0) {
        if (l_1C >= 128) {
            monster_set_action(a2, l_20, 0);
            ai_turn_toward(a2, l_20);
        } else {
            monster_set_action(a2, l_20, 48);
        }
        return;
    }
    l_10 = &a2->data.monster.anim;
    a1->flags &= ~0x100;
    if (a3 == player_entity && a1->give_up_timer == 0 && ((l_1C > 512 && ai_stealth_check(a1->race, (int)(unsigned short)(a1->flags & 256), l_24, (int)(unsigned short)(a1->flags & 8)) == 0) || ai_sees_through_illusion(a1->race) == 0)) {
        monster_set_action(a2, l_20, 48);
    } else {
        a1->flags |= 264;
        if (a1->give_up_timer == 0) a1->give_up_timer = 200;
        if ((l_14 << 2) < l_24 && l_24 < 2048 && ((int)(unsigned short)(a1->flags & 128)) != 0) {
            if (((int)(unsigned short)(*(short *)(monster_table_flags + (a1->race * 29)) & 32)) != 0) {
                if (l_1C >= 128) {
                    ai_turn_toward(a2, l_20);
                    monster_set_action(a2, l_20, 0);
                } else if (rand() < 1000 && a1->action != 24) {
                    monster_set_action(a2, l_20, 24);
                } else if (l_10->anim_request != 24 || l_10->anim_current == 24) {
                    monster_set_action(a2, l_20, 48);
                }
                if (a1->give_up_timer != 0) a1->give_up_timer--;
            } else if (l_24 > 256 && a1->magicka != 0 && ai_pick_ranged_spell(a4) != 0) {
                if (l_1C >= 128) {
                    ai_turn_toward(a2, l_20);
                    monster_set_action(a2, l_20, 0);
                } else if ((rand() % 40) == 0 && monster_cast_spell(a2, a3) != 0) {
                    monster_set_action(a2, l_20, 32);
                }
            } else {
                ai_move_toward_target(a2, &a2->data.character, a3, l_20, l_1C);
                monster_set_action(a2, l_20, 0);
            }
        } else if (((int)(unsigned short)(a1->flags & 256)) != 0 && l_24 > l_14) {
            ai_move_toward_target(a2, &a2->data.character, a3, l_20, l_1C);
            monster_set_action(a2, l_20, 0);
        } else if (((int)(unsigned short)(a1->flags & 128)) != 0) {
            if (l_1C < 128) {
                if (l_10->anim_request != 8 || l_10->anim_request != 56) {
                    if (a1->magicka != 0 && a1->attack_timer == 0 && ai_pick_touch_spell(a4) != 0 && monster_cast_spell(a2, a3) != 0) {
                        monster_set_action(a2, l_20, 32);
                    } else if ((rand() % a1->attributes[6]) < ((a1->attributes[6] >> 3) + 6) && a1->attack_timer == 0) {
                        if (monster_set_action(a2, l_20, 8) != 0) {
                            a1->attack_timer = rand_range(1500, 3000);
                            a1->attack_timer -= (player_character->level - 10) * 50;
                            a1->attack_timer += (a1->reflexes - 2) * 450;
                            if (((unsigned)a1->attack_timer) > 100000) a1->attack_timer = 1500;
                        }
                    } else if (((int)(unsigned short)(a1->flags & 16384)) != 0) {
                        monster_set_action(a2, l_20, 59);
                    } else {
                        monster_set_action(a2, l_20, 48);
                    }
                }
            } else {
                ai_turn_toward(a2, l_20);
                monster_set_action(a2, l_20, 0);
            }
        }
    }
    if (a1->give_up_timer != 0) a1->give_up_timer--;
    if (((struct bf8_7_1 *)&D_001940DA)->f != 0) {
        D_001940DA &= 127;
        return;
    }
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
    for (; l_24 < creature_count; l_24++) {
        if (a3 == l_24) continue;
        if ((signed char)a2->team == (signed char)ai_characters[l_24]->team) continue;
        ai_characters[l_24]->target_score = 0;
        if ((int)ai_characters[l_24]->target == 0) ai_characters[l_24]->target_score += 5;
        if (collide_line_of_sight(a1, ai_entities[l_24]) != 0) {
            ai_characters[l_24]->target_score += 20;
        }
        l_14 = xn_math_approx_hypot(a1->y - ai_entities[l_24]->y, xn_math_approx_dist2d(a1->x, a1->z, ai_entities[l_24]->x, ai_entities[l_24]->z));
        l_14 = l_14 / 128;
        l_14 = 30 - l_14;
        if (l_14 < 0) l_14 = 0;
        ai_characters[l_24]->target_score += *(signed char *)&l_14;
        if (((int)(unsigned char)(signed char)ai_characters[l_24]->target_score) > l_1C) {
            l_1C = (int)(unsigned char)(signed char)ai_characters[l_24]->target_score;
            l_18 = l_24;
        }
    }
    if (l_1C < 8 && ((int)(unsigned short)((short)a2->flags & 2)) != 0) return -1;
    if (l_1C == 0) return 0;
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
    if (D_00187CA8 == 0) return;
    if (creature_count == 0) return;
    if (creature_count == 1 && ((int)(unsigned short)(creature_list[0]->data.character.flags & 2)) != 0) {
        object_delete(creature_list[0]);
        return;
    }
    player_entity->yaw = player_object->yaw;
    player_entity->angle_x = player_object->angle_x;
    creature_list[creature_count++] = player_entity;
    for (l_28 = 0; l_28 < creature_count; l_28++) {
        if ((creature_count - 1) != l_28) place_settle_creature(creature_list[l_28]);
        ai_entities[l_28] = (struct record *)((int)creature_list[l_28]);
        ai_characters[l_28] = (struct character *)((int)((char *)ai_entities[l_28] + 71));
    }
    for (l_28 = 0; (creature_count - 1) > l_28; l_28++) {
        l_18 = 1132;
        if (ai_characters[l_28]->target == 0 || (ai_characters[l_28]->target != 0 && ((unsigned)(((unsigned)*(int *)((char *)l_18)) % 200)) < 4) || ai_characters[l_28]->target->type == 44 || ai_characters[l_28]->target->type == 34) {
            l_24 = ai_pick_target(ai_entities[l_28], ai_characters[l_28], l_28);
            if (l_24 == (-1)) {
                l_20 = l_28;
            } else {
                ai_characters[l_28]->target = (struct record *)((int)ai_entities[l_24]);
                ai_characters[l_24]->target = (struct record *)((int)ai_entities[l_28]);
            }
        }
    }
    creature_count--;
    ai_los_index = (ai_los_index + 1) % creature_count;
    for (l_28 = 0; l_28 < creature_count; l_28++) {
        if (ai_entities[l_28]->type != 18) continue;
        monster_ambient_sound(ai_entities[l_28], ai_characters[l_28]);
        if (((int)(unsigned short)(ai_characters[l_28]->flags & 32)) != 0) {
            damage_knockback_move(ai_entities[l_28], ai_characters[l_28]);
            monster_set_action(ai_entities[l_28], 0, 16);
        } else {
            if (ai_characters[l_28]->target != 0) {
                ai_creature_think(ai_characters[l_28], ai_entities[l_28], *(struct record **)((char *)ai_characters[l_28] + 112), l_28);
            }
            l_1C = (int)ai_entities[l_28] + 705;
            if (((int)(unsigned short)(*(short *)((char *)l_1C + 16) & 1)) != 0) {
                *(signed char *)((char *)l_1C + 16) &= 254;
                weapon_melee_strike(creature_list[l_28]);
            }
        }
    }
    monster_apply_gravity();
    if (l_20 == (-1)) return;
    object_delete(ai_entities[l_20]);
}

int ai_turn_toward(struct record *a1, int a2)
{
    int l_1C;
    int l_18;

    l_1C = ai_angle_diff(a1->yaw, a2, (int)&l_18);
    if (l_1C < 64) {
        a1->yaw = a2;
        return 0;
    }
    a1->yaw += l_18 << 6;
    return 1;
}

void func_0006228A(struct record *a1, int a2, int a3)
{
    int l_14;
    int l_10;

    a3 = (a3 + 1024) & 2047;
    l_14 = ai_angle_diff(a1->yaw, a3, (int)&l_10);
    if (l_14 < 32) {
        a1->yaw = a3;
        return;
    }
    a1->yaw += l_10 << 5;
}
