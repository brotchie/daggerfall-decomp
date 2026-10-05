/* equip.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"
#include "bitfield.h"

extern struct monster_template monster_table[];
extern char D_001758B8[];
extern char D_001758E2[];
extern char D_001758EE[];
extern char D_001758F8[];
extern char D_00175927[];
extern unsigned char player_environment;
extern struct item_template item_templates[];
extern char potion_recipes[];
extern char D_00180B42[];
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
extern char picked_model_index[];
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
extern struct record *item_add_to_container(struct record *, int, int, int);
extern int armor_image_for_type(int, int);
extern int monster_set_action(struct record *, int, int);
extern int ai_pick_ranged_spell(int);
extern int ai_pick_touch_spell(int);
extern int monster_cast_spell(struct record *, struct record *);
extern int ai_angle_diff(int, int, int *);
extern int ai_sees_through_illusion(int);
extern int ai_stealth_check(int, int, int, int);
extern int disk_read_file(char *, int);
extern int disk_open_data(char *);
extern int rand_range(int, int);
extern struct building *object_building(struct record *);
extern struct record *object_delete(struct record *);
extern struct record *object_create_child(struct record *, struct record *, int);
extern struct record *object_reparent(struct record *, struct record *);
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
extern void loot_generate(int, struct record *, int, int);
extern void item_make_magic(struct item *, int);
extern void item_make_artifact(struct item *, int);
extern void monster_shoot_arrow(struct record *, struct record *);
extern void monster_ambient_sound(struct record *, struct character *);
extern void ai_move_toward_target(struct record *, struct character *, struct record *, int, int);
extern void monster_apply_gravity(void);
extern void weapon_melee_strike(struct record *);
extern void item_break(struct record *);
int pick_random_byte(unsigned char *);
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

void item_make(int group, int index, struct item *item)
{
    switch ((unsigned)group) {
case 5:
    item_make_artifact(item, index);
    return;
case 4:
    item_make_magic(item, index);
    return;
case 11:
    item_init_from_template(287, 27, 8, item);
    return;
default:
    item_init_from_template((int)(unsigned short)*(short *)((char *)(int)(*(char **)(item_group_templates + (group << 2)) + (index * 2))), (int)(short)*(short *)&group, (int)(short)*(short *)&index, item);
}
}

void item_set_race_image(struct item *item, int race)
{
    if (race > 7) {
        item->inventory_image += ((unsigned short)(unsigned char)D_00186589[player_character->original_race]) << 7;
        return;
    }
    item->inventory_image += ((unsigned short)(unsigned char)D_00186589[race]) << 7;
}

void func_0005E636(struct item *item)
{
    int unused;
    int variant;
    int unused2;
    int extra_index;
    signed char *style_table;

    switch (item->group) {
    case 12:
        extra_index = 9;
        style_table = (signed char *)D_00185FFC;
        break;
    case 6:
        extra_index = 13;
        style_table = (signed char *)D_0018606B;
        break;
    case 2:
        item->inventory_image += rand_range(0, (int)&*(signed char *)((char *)(item->variants) - 1));
        return;
    default:
        extra_index = 0;
    }
    if (item->variants >= 2) {
        variant = rand_range(0, (int)&*(signed char *)((char *)(item->variants) - 1));
        item->inventory_image += variant;
    }
    item_roll_dye(item);
    if (item->index != extra_index) if (item->index != (extra_index + 1)) return;
    item->inventory_image++;
}

void item_next_clothing_style(struct item *item)
{
    int style;
    int extra_index;
    signed char *style_table;

    if (item->group == 12) {
        style_table = (signed char *)D_00185FFC;
        extra_index = 9;
    } else {
        style_table = (signed char *)D_0018606B;
        extra_index = 13;
    }
    style = (int)(unsigned short)(item->inventory_image & 127);
    if (style_table[style] == 0) return;
    if (style_table[style + 1] == style_table[style]) {
        item->inventory_image++;
        return;
    }
    style--;
    while (style_table[style + 1] == style_table[style]) {
        style--;
        item->inventory_image--;
    }
    if (item->index != extra_index) if (item->index != (extra_index + 1)) return;
    item->inventory_image++;
}

void func_0005E7FC(struct item *item)
{
    int style;
    int unused;
    signed char *style_table;

    if (item->group == 12) {
        style_table = (signed char *)D_00185FFC;
    } else {
        style_table = (signed char *)D_0018606B;
    }
    style = (int)(unsigned short)(item->inventory_image & 127);
    style--;
    while (style_table[style + 1] == style_table[style]) {
        style--;
        item->inventory_image--;
    }
}

void item_roll_material(struct item *item)
{
    int roll;
    int enchanted;
    int unused;
    int unused2;
    int level_mod;

    enchanted = ((item->enchantments[0].type != (-1)) ? 1 : 0);
    item->material = 0;
    if (item->group == 3 && item->index == 18) return;
    roll = rand_range(0, 255);
    if (enchanted != 0) {
        roll += 60;
        if (roll > 255) roll = 255;
    }
    level_mod = player_character->level - 10;
    if (level_mod < 0) {
        level_mod <<= 2;
    } else {
        level_mod <<= 1;
    }
    roll += level_mod;
    if (((int)player_environment) == 2) {
        roll -= (14 - current_building->quality) * 2;
    }
    if (roll < 0) {
        roll = 0;
    } else if (roll > 256) {
        roll = 256;
    }
    if (forced_material != 0) {
        item->material = forced_material - 1;
        forced_material = 0;
    } else {
        while (((int)(unsigned char)D_00186591[item->material]) < roll) {
            roll -= (int)(unsigned char)D_00186591[item->material];
            item->material++;
        }
    }
    item->value = item->value * (((int)(short)*(short *)(D_001869C0 + (item->material * 2))) * 3);
    item->weight = ((unsigned)(((int)(short)*(short *)(D_001869D4 + (item->material * 2))) * item->weight)) >> 2;
    item->condition = (item->max_condition = (item->max_condition * ((int)(short)*(short *)(D_001869EA + (item->material * 2)))) >> 2);
    item->enchant_points = (item->enchant_points * ((int)(short)*(short *)(D_001869FE + (item->material * 2)))) >> 2;
    item->color = item->material + 16;
}

void item_roll_armor_type(struct item *item)
{
    int type_roll;
    int image;
    int roll;
    int level_mod;

    type_roll = rand_range(1, 100);
    if (type_roll < 70 && forced_material == 0) {
        item->armor_type = 0;
        item->weight >>= 1;
    } else if (type_roll < 90 && forced_material == 0) {
        item->armor_type = 1;
    } else {
        item->armor_type = 2;
    }
    if (item->armor_type == 2) {
        roll = rand_range(0, 255);
        if (item->enchantments[0].type != (-1)) {
            roll += 60;
            if (roll > 255) roll = 255;
        }
        level_mod = player_character->level - 10;
        if (level_mod < 0) {
            level_mod <<= 2;
        } else {
            level_mod <<= 1;
        }
        roll += level_mod;
        if (((int)player_environment) == 2) {
            roll -= (14 - current_building->quality) * 2;
        }
        if (roll < 0) {
            roll = 0;
        } else if (roll > 256) {
            roll = 256;
        }
        if (forced_material != 0) {
            item->material = forced_material - 1;
            forced_material = 0;
        } else {
            while (((int)(unsigned char)D_00186591[item->material]) < roll) {
                roll -= (int)(unsigned char)D_00186591[item->material];
                item->material++;
            }
        }
        item->value = item->value * (((int)(short)*(short *)(D_001869C0 + (item->material * 2))) * 3);
        item->weight = ((unsigned)(((int)(short)*(short *)(D_001869D4 + (item->material * 2))) * item->weight)) >> 2;
        item->condition = (item->max_condition = (item->max_condition * ((int)(short)*(short *)(D_001869EA + (item->material * 2)))) >> 2);
        item->enchant_points = (item->enchant_points * ((int)(short)*(short *)(D_001869FE + (item->material * 2)))) >> 2;
        item->color = item->material + 16;
    } else {
        item->value = item->value * ((int)&*(signed char *)((char *)(item->armor_type) + 1));
    }
    image = armor_image_for_type(item->armor_type, item->index);
    if (image == (-1)) return;
    item->inventory_image = image + (item->inventory_image & -128);
}

void item_roll_dye(struct item *item)
{
    int roll;

    roll = rand_range(0, 100);
    if (roll < 25) return;
    item->color = rand_range(1, 10);
    if (item->group == 6) {
        if (item->index == 10 || item->index == 11) {
            item->color = D_0018659B[rand() & 3];
        } else if (item->index == 4) {
            item->color = D_0018659F[rand() & 3];
        }
        return;
    }
    if (item->group != 12 || item->index != 8) return;
    item->color = D_0018659B[rand() & 3];
}

void shelf_stock_items(struct record *container, int list_index, int quality)
{
    int chance;
    int unused;
    int chance_mod;
    int index;
    unsigned char *entry;
    short *template_ids;
    struct item_template *template;
    struct item *item_data;
    struct record *item;

    entry = (unsigned char *)D_00186483[list_index];
    if (entry == 0) return;
    while (*entry != 255) {
        if (*entry == 6 && ((int)(unsigned short)(player_character->flags & 1)) != 0) {
            *entry = 12;
        }
        if (*entry == 12 && ((int)(unsigned short)(player_character->flags & 1)) == 0) {
            *entry = 6;
        }
        template_ids = *(short **)(item_group_templates + (*entry << 2));
        if (template_ids != 0 && *entry != 8 && *entry != 1) {
            if (((struct bf8_1_1 *)&D_001940D7)->f != 0 && (*entry == 4 || *entry == 5)) {
            } else {
                chance_mod = entry[1];
                index = 0;
                if (*entry == 7) {
                    shelf_stock_books(container, quality);
                } else {
                    while (*template_ids != (-1)) {
                        template = &item_templates[*template_ids];
                        chance = (((21 - template->rarity) * 5) * chance_mod) / 100;
                        if (template->rarity <= quality && rand_range(1, 100) <= chance) {
                            item = object_create_child(container, 0, 107);
                            item->type = 2;
                            item->x = player_object->x;
                            item->y = player_object->y;
                            item->z = player_object->z;
                            item->image2 = 998;
                            item->id = object_new_id(((unsigned)location_object->id) >> 16);
                            if (list_index >= 0) item->flags |= 32;
                            item_data = &item->data.item;
                            item_make(*entry, index, item_data);
                            if (item_data->group != 3 || item_data->index != 18) item->image2 = 0;
                        }
                        template_ids++;
                        index++;
                    }
                }
            }
        }
        entry += 2;
    }
}

int func_0005F0A0(int group)
{
    {
        int result;

        if (group == 12 || group == 6 || group == 2) {
            result = 1;
        } else {
            result = 0;
        }
        return result;
    }
}

void shelf_stock_books(struct record *container, int quality)
{
    int i;
    struct record *book;

    quality = (quality + 3) / 5;
    if (quality >= 4) quality--;
    quality++;
    for (i = 0; i <= quality; i++) {
        book = object_create_child(container, 0, 107);
        book->type = 2;
        book->x = player_object->x;
        book->y = player_object->y;
        book->z = player_object->z;
        book->id = object_new_id(((unsigned)location_object->id) >> 16);
        book->flags |= 32;
        item_make(7, quality, &book->data.item);
    }
}

void shop_stock_magic(struct record *container, int unused, int not_owned, int soul_traps)
{
    int i;
    struct record *item;
    int unused2;

    if (not_owned != 0) not_owned = 32;
    for (i = 0; ((current_building->quality >> 1) + 1) > i; i++) {
        item = object_create_child(container, 0, 107);
        item->type = 2;
        item->id = object_new_id(((unsigned)location_object->id) >> 16);
        item_make_magic(&item->data.item, -1);
        item->x = player_object->x;
        item->y = player_object->y;
        item->z = player_object->z;
        item->flags |= not_owned;
        if (not_owned != 0) item->data.item.item_flags |= 32;
    }
    item = object_create_child(container, 0, 107);
    item->type = 2;
    item->id = object_new_id(((unsigned)location_object->id) >> 16);
    item->x = player_object->x;
    item->y = player_object->y;
    item->z = player_object->z;
    item->flags |= 32;
    item_make(27, 0, &item->data.item);
    if (soul_traps == 0) return;
    for (i = 0; ((current_building->quality >> 1) + 1) > i; i++) {
        item = object_create_child(container, 0, 107);
        item->type = 2;
        item->id = object_new_id(((unsigned)location_object->id) >> 16);
        item->x = player_object->x;
        item->y = player_object->y;
        item->z = player_object->z;
        item->flags |= 32;
        item_make(27, 1, &item->data.item);
        if (rand_range(1, 100) < 25) {
            soul_trap_add_soul(item);
            item->data.item.value = *(int *)(monster_soul_values + (item->children->soul_creature << 2)) + 5000;
        } else {
            item->data.item.value = 5000;
        }
    }
}

void shop_stock_soul_traps(struct record *container)
{
    int i;
    struct record *trap;
    int unused;

    for (i = 0; ((current_building->quality >> 1) + 1) > i; i++) {
        trap = object_create_child(container, 0, 107);
        trap->type = 2;
        trap->id = object_new_id(((unsigned)location_object->id) >> 16);
        trap->x = player_object->x;
        trap->y = player_object->y;
        trap->z = player_object->z;
        trap->flags |= 32;
        item_make(27, 1, &trap->data.item);
        if (rand_range(1, 100) < 25) {
            soul_trap_add_soul(trap);
            trap->data.item.value = *(int *)(monster_soul_values + (trap->children->soul_creature << 2)) + 5000;
        } else {
            trap->data.item.value = 5000;
        }
    }
}

void painting_draw(void)
{
    int i;
    int frame;
    struct image *image;

    mc_set_location(625, (int)D_001758B8);
    mc_sprintf((int)text_buffer, (int)D_001758E2, (((int)(unsigned short)D_00195DC4) >> 3) + 97);
    disk_read_file(text_buffer, *(int *)scratch_buffer);
    image = *(struct image **)scratch_buffer;
    i = 0;
    frame = (int)(unsigned short)(D_00195DC4 & 7);
    while (i < frame) {
        image = (struct image *)(image->data_size + (char *)image + 12);
        i++;
    }
    xn_draw_image(160 - ((image->width) >> 1), 50, image->width, image->height, image->pixels);
}

void item_info_painting(struct item *item)
{
    int fd;
    int saved_seed;

    saved_seed = rand();
    D_00195DC4 = rand(srand((int)(unsigned short)(short)item->message)) % 180;
    fd = disk_open_data(D_001758EE);
    lseek(fd, ((int)(unsigned short)D_00195DC4) * 40, 0);
    read(fd, *(int *)scratch_buffer, 40);
    close(fd);
    painting_subject_text = pick_random_byte(*(unsigned char **)scratch_buffer) + 6100;
    painting_adjective_text = pick_random_byte((unsigned char *)(*(int *)scratch_buffer + 10)) + 6200;
    painting_prefix1_text = pick_random_byte(*(unsigned char **)scratch_buffer + 20) + 6300;
    painting_prefix2_text = (short)pick_random_byte(*(unsigned char **)scratch_buffer + 30) + 6400;
    D_001940D6 |= 32;
    msgbox_show_rsc(250, 1);
    srand(saved_seed);
}

int pick_random_byte(unsigned char *list)
{
    int count;

    count = 0;
    while (count < 10 && list[count] != 255) count++;
    if (count == 1) return *list;
    return list[rand_range(0, count - 1)];
}

void item_init_book(struct item *item, int level)
{
    int *header;
    int fd;
    int saved_seed;

    item->message = *(short *)(book_list + (book_pick_random(level) << 2));
    header = *(int **)scratch_buffer;
    mc_set_location(674, (int)D_001758B8);
    mc_sprintf((int)text_buffer, (int)D_001758F8, (int)(unsigned short)(short)item->message);
    fd = disk_open_data(text_buffer);
    read(fd, header, 234);
    close(fd);
    saved_seed = rand();
    srand(*header);
    item->value = rand_range(300, 800);
    srand(saved_seed);
}

int book_pick_random(int level)
{
    int i;
    int count;
    int pick;

    i = 0;
    count = i;
    for (; ((int)(unsigned short)book_count) > i; i++) {
        if (((int)(unsigned short)*(short *)(D_0018E046 + (i << 2))) <= level) count++;
    }
    if (count == 0) count = 1;
    pick = rand() % count;
    i = 0;
    while (pick >= 0) {
        if (((int)(unsigned short)*(short *)(D_0018E046 + (i << 2))) <= level) pick--;
        i++;
    }
    return i - 1;
}

int equip_hiding_capacity(int with_armor)
{
    int slot;
    int capacity;
    struct item *item;

    slot = 0;
    capacity = slot;
    for (; slot < 27; slot++) {
        if (player_character->equipped[slot] == 0) continue;
        item = &player_character->equipped[slot]->data.item;
        if (item->group == 1 && memchr((int)D_00186583, item->index, 6) != 0) {
            capacity += 10;
            continue;
        }
        if (item->group == 2 && with_armor != 0) {
            capacity += (int)(unsigned char)D_00186578[item->index];
        } else if (item->group == 6 || item->group == 12) {
            capacity += (int)(unsigned char)D_0018654F[item->index];
        }
    }
    return capacity;
}

void loot_fill_container(struct record *container)
{
    int table;
    int unused;
    struct building *building;

    if (((int)player_environment) != 1) {
        table = ((int)(unsigned char)D_001865F2[current_location->kind]) - 1;
        while (container->children == 0) {
            loot_generate(table, container, player_character->level, (int)(unsigned short)(player_character->flags & 1));
        }
        if (table < 9 || table > 14) return;
        if (rand_range(1, 100) <= ((int)(unsigned char)D_00186610[table])) {
            container = object_create_child(container, 0, 107);
            container->type = 2;
            item_make(27, 8, &container->data.item);
        }
        if (rand_range(1, 100) < 4) loot_add_potion(container);
        if (rand_range(1, 100) < 2) item_add_to_container(container, 27, 4, 0);
        return;
    }
    building = object_building(container);
    if (building == 0) {
        table = 0;
    } else {
        table = building->type;
    }
    while (container->children == 0) {
        loot_generate(((int)(unsigned char)D_00186605[table]) - 1, container, player_character->level, (int)(unsigned short)(player_character->flags & 1));
    }
}

void loot_add_gold(struct record *container, int min_amount, int max_amount)
{
    struct record *gold;
    struct item *gold_data;

    gold = object_create_child(container, 0, 107);
    gold_data = &gold->data.item;
    gold->type = 2;
    gold->x = container->x;
    gold->y = container->y;
    gold->z = container->z;
    gold_data->group = 28;
    gold_data->index = 0;
    gold_data->value = rand_range(min_amount, max_amount) * player_character->level;
}

void loot_add_ingredient(struct record *container)
{
    struct record *item;

    item = object_create_child(container, 0, 107);
    item->type = 2;
    item->x = container->x;
    item->y = container->y;
    item->z = container->z;
    item_make_random((int)(unsigned short)((unsigned short)(unsigned char)D_0018661E[rand() % 8]), &item->data.item);
}

void loot_add_misc_item(struct record *container)
{
    struct record *item;

    item = object_create_child(container, 0, 107);
    item->type = 2;
    item->x = container->x;
    item->y = container->y;
    item->z = container->z;
    item_make_random((int)(unsigned short)((unsigned short)(unsigned char)D_00186626[rand() % 4]), &item->data.item);
}

void loot_add_item_of_group(struct record *container, int group)
{
    struct record *item;

    item = object_create_child(container, 0, 107);
    item->type = 2;
    item->x = container->x;
    item->y = container->y;
    item->z = container->z;
    item_make_random((int)(unsigned short)*(short *)&group, &item->data.item);
}

void item_wear_from_hit(struct record *item, int damage)
{
    struct item *item_data;

    item_data = &item->data.item;
    damage = ((damage * 10) + 50) / 100;
    if (damage == 0 && rand_range(1, 100) < 20) damage = 1;
    if (item_data->condition > damage) {
        item_data->condition -= damage;
        return;
    }
    item_break(item);
}

void item_damage(struct record *item, int amount)
{
    struct item *item_data;

    if (item == 0) return;
    item_data = &item->data.item;
    if (item_data->condition > amount) {
        item_data->condition -= amount;
        return;
    }
    item_break(item);
}

void magic_def_load(void)
{
    int fd;

    fd = disk_open_data(D_00175927);
    read(fd, (int)magic_def_count, 4);
    magic_def = mc_malloc(filelength(fd) - 4, (int)D_001758B8, 1201);
    read(fd, magic_def, (int)&*(signed char *)((char *)filelength(fd) - 4));
    close(fd);
}

void shop_generate_stock(struct record *container, int variant, int quality, int building_type, int model_index)
{
    char *groups;
    int pick;
    int more;
    int chance;
    int group_count;
    struct record *item;

    if (building_type >= 21) return;
    if (variant < 2) {
        group_count = 3;
        groups = D_00186639 + building_type * 3;
    } else if (variant < 4) {
        group_count = 12;
        groups = D_00186678 + building_type * 12;
    } else if (variant < 11) {
        group_count = 12;
        groups = D_00186774 + building_type * 12;
    } else if (variant < 15) {
        group_count = 5;
        groups = D_00186870 + building_type * 5;
    } else {
        group_count = 11;
        groups = D_001868D9 + building_type * 11;
    }
    for (pick = 0; pick < group_count; pick++) {
        if (groups[pick] == 0) break;
    }
    pick = rand_range(0, pick - 1);
    chance = 100;
    more = 1;
    while (more != 0) {
        item = object_create_child(container, 0, 107);
        item->type = 2;
        item->x = player_object->x;
        item->y = player_object->y;
        item->z = player_object->z;
        item->owner = model_index;
        item->id = object_new_id(((unsigned)location_object->id) >> 16);
        if ((unsigned char)groups[pick] == 6 || (unsigned char)groups[pick] == 12) {
            if (((int)(unsigned short)(player_character->flags & 1)) == 0) {
                item_make_random(6, &item->data.item);
            } else {
                item_make_random(12, &item->data.item);
            }
        } else if ((unsigned char)groups[pick] == 4) {
            item_make_magic(&container->data.item, -1);
        } else if ((unsigned char)groups[pick] == 7) {
            item_make(7, (quality + 3) / 5, &item->data.item);
        } else {
            item_make_random((int)(unsigned short)((unsigned short)(unsigned char)groups[pick]), &item->data.item);
        }
        chance >>= 1;
        if ((rand() % 100) > chance) more = 0;
    }
}

int func_000612A1(void)
{
    struct record *item;
    int count;

    count = 0;
    item = D_0019615F;
    while (item != 0) {
        if (item->owner == *(int *)picked_model_index) count++;
        item->x = player_object->x;
        item->y = player_object->y;
        item->z = player_object->z;
        item = item->next;
    }
    return count;
}

void soul_trap_add_soul(struct record *trap)
{
    struct record *soul;
    int creature;

    creature = rand_range(0, 42);
    while (*(int *)(monster_soul_values + (creature << 2)) == 0) creature = rand_range(0, 42);
    soul = object_create_child(trap, 0, 0);
    soul->flags = 3;
    soul->type = 20;
    soul->soul_creature = creature;
}

void shop_stock_potions(struct record *container)
{
    int i;
    int recipe;
    struct record *item;
    struct item *item_data;

    for (i = 0; (current_building->quality + 1) > i; i++) {
        recipe = potion_random_recipe();
        item = object_create_child(container, 0, 107);
        item_data = &item->data.item;
        item->type = 2;
        item->flags |= 33;
        item->x = player_object->x;
        item->y = player_object->y;
        item->z = player_object->z;
        item_make(1, 1, item_data);
        item_data->value = (int)(unsigned short)*(short *)(D_00180B42 + (recipe * 109));
        item_data->stack_count = *(signed char *)&recipe;
        item = object_create_child(item, 0, 109);
        item->type = 31;
        mc_memcpy(&item->data.potion_recipe, ((int)potion_recipes) + (recipe * 109), 109, (int)D_001758B8, 1330, 4);
    }
}

int potion_random_recipe(void)
{
    int i;
    int count;

    i = 0;
    count = 0;
    while (*(short *)(D_00180B42 + (i++ * 109)) != 0) count++;
    return rand_range(0, count - 1);
}

void loot_add_potion(struct record *container)
{
    int unused;
    int recipe;
    struct record *item;
    struct item *item_data;

    recipe = potion_random_recipe();
    item = object_create_child(container, 0, 107);
    item_data = &item->data.item;
    item->type = 2;
    item->flags |= 1;
    item->x = player_object->x;
    item->y = player_object->y;
    item->z = player_object->z;
    item_make(1, 1, item_data);
    item_data->value = (int)(unsigned short)*(short *)(D_00180B42 + (recipe * 109));
    item_data->stack_count = *(signed char *)&recipe;
    item = object_create_child(item, 0, 109);
    item->type = 31;
    mc_memcpy(&item->data.potion_recipe, ((int)potion_recipes) + (recipe * 109), 109, (int)D_001758B8, 1366, 4);
}

void ai_creature_think(struct character *monster_char, struct record *monster, struct record *target, int index)
{
    int distance;
    int target_angle;
    int angle_delta;
    int turn_dir;
    int reach;
    struct monster_anim *anim;
    struct record *cell;

    monster_char->attack_timer -= frame_ticks;
    if (((unsigned)monster_char->attack_timer) > 100000) monster_char->attack_timer = 0;
    if (((int)player_environment) == 3) {
        cell = location_cell_at(monster->x, monster->z);
        if ((int)monster->parent != cell) object_reparent(cell, monster);
    }
    D_00199D74 = 0;
    ai_monster_flags = (int)(unsigned short)monster_table[monster_char->race].flags;
    monster->yaw &= ~0xF800;
    monster_ambient_sound(monster, monster_char);
    if (((int)(unsigned short)(monster_char->flags & 16384)) == 0 && monster_char->race == 29 && monster_char->health < monster_char->max_health) {
        monster_char->flags |= 0x4000;
        monster_set_action(monster, 0, 57);
        return;
    }
    target_angle = xn_math_angle_to_point(monster->x, monster->z, target->x, target->z);
    if ((monster_char->conditions & 0x1) != 0) {
        monster_set_action(monster, target_angle, 48);
        return;
    }
    if (target == player_entity) {
        reach = 90;
    } else {
        reach = 60;
    }
    if (index == ai_los_index) {
        if (collide_line_of_sight(monster, target) != 0) {
            monster_char->flags |= 128;
        } else {
            monster_char->flags &= ~0x80;
        }
    }
    angle_delta = ai_angle_diff(monster->yaw, target_angle, &turn_dir);
    distance = xn_math_approx_hypot(monster->y - target->y, xn_math_approx_dist2d(monster->x, monster->z, target->x, target->z));
    if (monster->wait_state != 0 && trespassing == 0) {
        monster_set_action(monster, target_angle, 48);
        return;
    }
    if (((int)(unsigned short)(monster_char->flags & 32768)) != 0) {
        if (angle_delta >= 128) {
            monster_set_action(monster, target_angle, 0);
            ai_turn_toward(monster, target_angle);
        } else {
            monster_set_action(monster, target_angle, 48);
        }
        return;
    }
    anim = &monster->data.monster.anim;
    monster_char->flags &= ~0x100;
    if (target == player_entity && monster_char->give_up_timer == 0 && ((angle_delta > 512 && ai_stealth_check(monster_char->race, (int)(unsigned short)(monster_char->flags & 256), distance, (int)(unsigned short)(monster_char->flags & 8)) == 0) || ai_sees_through_illusion(monster_char->race) == 0)) {
        monster_set_action(monster, target_angle, 48);
    } else {
        monster_char->flags |= 264;
        if (monster_char->give_up_timer == 0) monster_char->give_up_timer = 200;
        if ((reach << 2) < distance && distance < 2048 && ((int)(unsigned short)(monster_char->flags & 128)) != 0) {
            if (((int)(unsigned short)(monster_table[monster_char->race].flags & 32)) != 0) {
                if (angle_delta >= 128) {
                    ai_turn_toward(monster, target_angle);
                    monster_set_action(monster, target_angle, 0);
                } else if (rand() < 1000 && monster_char->action != 24) {
                    monster_set_action(monster, target_angle, 24);
                } else if (anim->anim_request != 24 || anim->anim_current == 24) {
                    monster_set_action(monster, target_angle, 48);
                }
                if (monster_char->give_up_timer != 0) monster_char->give_up_timer--;
            } else if (distance > 256 && monster_char->magicka != 0 && ai_pick_ranged_spell(index) != 0) {
                if (angle_delta >= 128) {
                    ai_turn_toward(monster, target_angle);
                    monster_set_action(monster, target_angle, 0);
                } else if ((rand() % 40) == 0 && monster_cast_spell(monster, target) != 0) {
                    monster_set_action(monster, target_angle, 32);
                }
            } else {
                ai_move_toward_target(monster, &monster->data.character, target, target_angle, angle_delta);
                monster_set_action(monster, target_angle, 0);
            }
        } else if (((int)(unsigned short)(monster_char->flags & 256)) != 0 && distance > reach) {
            ai_move_toward_target(monster, &monster->data.character, target, target_angle, angle_delta);
            monster_set_action(monster, target_angle, 0);
        } else if (((int)(unsigned short)(monster_char->flags & 128)) != 0) {
            if (angle_delta < 128) {
                if (anim->anim_request != 8 || anim->anim_request != 56) {
                    if (monster_char->magicka != 0 && monster_char->attack_timer == 0 && ai_pick_touch_spell(index) != 0 && monster_cast_spell(monster, target) != 0) {
                        monster_set_action(monster, target_angle, 32);
                    } else if ((rand() % monster_char->attributes[6]) < ((monster_char->attributes[6] >> 3) + 6) && monster_char->attack_timer == 0) {
                        if (monster_set_action(monster, target_angle, 8) != 0) {
                            monster_char->attack_timer = rand_range(1500, 3000);
                            monster_char->attack_timer -= (player_character->level - 10) * 50;
                            monster_char->attack_timer += (monster_char->reflexes - 2) * 450;
                            if (((unsigned)monster_char->attack_timer) > 100000) monster_char->attack_timer = 1500;
                        }
                    } else if (((int)(unsigned short)(monster_char->flags & 16384)) != 0) {
                        monster_set_action(monster, target_angle, 59);
                    } else {
                        monster_set_action(monster, target_angle, 48);
                    }
                }
            } else {
                ai_turn_toward(monster, target_angle);
                monster_set_action(monster, target_angle, 0);
            }
        }
    }
    if (monster_char->give_up_timer != 0) monster_char->give_up_timer--;
    if (((struct bf8_7_1 *)&D_001940DA)->f != 0) {
        D_001940DA &= 127;
        return;
    }
    if (((int)(unsigned short)(anim->anim_bits & 2)) == 0) return;
    anim->anim_events &= 253;
    monster_shoot_arrow(monster, target);
}

int ai_pick_target(struct record *monster, struct character *monster_char, int index)
{
    int i;
    int unused;
    int best_score;
    int best;
    int score;

    unused = 0;
    i = 0;
    best_score = i;
    for (; i < creature_count; i++) {
        if (index == i) continue;
        if ((signed char)monster_char->team == (signed char)ai_characters[i]->team) continue;
        ai_characters[i]->target_score = 0;
        if ((int)ai_characters[i]->target == 0) ai_characters[i]->target_score += 5;
        if (collide_line_of_sight(monster, ai_entities[i]) != 0) {
            ai_characters[i]->target_score += 20;
        }
        score = xn_math_approx_hypot(monster->y - ai_entities[i]->y, xn_math_approx_dist2d(monster->x, monster->z, ai_entities[i]->x, ai_entities[i]->z));
        score = score / 128;
        score = 30 - score;
        if (score < 0) score = 0;
        ai_characters[i]->target_score += *(signed char *)&score;
        if (((int)(unsigned char)(signed char)ai_characters[i]->target_score) > best_score) {
            best_score = (int)(unsigned char)(signed char)ai_characters[i]->target_score;
            best = i;
        }
    }
    if (best_score < 8 && ((int)(unsigned short)((short)monster_char->flags & 2)) != 0) return -1;
    if (best_score == 0) return 0;
    return best;
}

void ai_update_creatures(void)
{
    int i;
    int target_index;
    int removed;
    struct monster_anim *anim;
    int bios_ticks;

    removed = -1;
    if (D_00187CA8 == 0) return;
    if (creature_count == 0) return;
    if (creature_count == 1 && ((int)(unsigned short)(creature_list[0]->data.character.flags & 2)) != 0) {
        object_delete(creature_list[0]);
        return;
    }
    player_entity->yaw = player_object->yaw;
    player_entity->angle_x = player_object->angle_x;
    creature_list[creature_count++] = player_entity;
    for (i = 0; i < creature_count; i++) {
        if ((creature_count - 1) != i) place_settle_creature(creature_list[i]);
        ai_entities[i] = (struct record *)((int)creature_list[i]);
        ai_characters[i] = (struct character *)((int)((char *)ai_entities[i] + 71));
    }
    for (i = 0; (creature_count - 1) > i; i++) {
        bios_ticks = 1132;
        if (ai_characters[i]->target == 0 || (ai_characters[i]->target != 0 && ((unsigned)(((unsigned)*(int *)((char *)bios_ticks)) % 200)) < 4) || ai_characters[i]->target->type == 44 || ai_characters[i]->target->type == 34) {
            target_index = ai_pick_target(ai_entities[i], ai_characters[i], i);
            if (target_index == (-1)) {
                removed = i;
            } else {
                ai_characters[i]->target = (struct record *)((int)ai_entities[target_index]);
                ai_characters[target_index]->target = (struct record *)((int)ai_entities[i]);
            }
        }
    }
    creature_count--;
    ai_los_index = (ai_los_index + 1) % creature_count;
    for (i = 0; i < creature_count; i++) {
        if (ai_entities[i]->type != 18) continue;
        monster_ambient_sound(ai_entities[i], ai_characters[i]);
        if (((int)(unsigned short)(ai_characters[i]->flags & 32)) != 0) {
            damage_knockback_move(ai_entities[i], ai_characters[i]);
            monster_set_action(ai_entities[i], 0, 16);
        } else {
            if (ai_characters[i]->target != 0) {
                ai_creature_think(ai_characters[i], ai_entities[i], ai_characters[i]->target, i);
            }
            anim = &ai_entities[i]->data.monster.anim;
            if ((anim->anim_bits & 1) != 0) {
                anim->anim_events &= 254;
                weapon_melee_strike(creature_list[i]);
            }
        }
    }
    monster_apply_gravity();
    if (removed == (-1)) return;
    object_delete(ai_entities[removed]);
}

int ai_turn_toward(struct record *monster, int angle)
{
    int angle_delta;
    int turn_dir;

    angle_delta = ai_angle_diff(monster->yaw, angle, &turn_dir);
    if (angle_delta < 64) {
        monster->yaw = angle;
        return 0;
    }
    monster->yaw += turn_dir << 6;
    return 1;
}

void func_0006228A(struct record *monster, int unused, int angle)
{
    int angle_delta;
    int turn_dir;

    angle = (angle + 1024) & 2047;
    angle_delta = ai_angle_diff(monster->yaw, angle, &turn_dir);
    if (angle_delta < 32) {
        monster->yaw = angle;
        return;
    }
    monster->yaw += turn_dir << 5;
}
