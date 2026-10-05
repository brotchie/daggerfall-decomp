/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005DE74 */
#include "records.h"

extern char D_001758B8[];
extern char D_001758C0[];
extern char D_001758C1[];
extern struct item_template item_templates[];
extern unsigned char D_00190CF2;
extern char D_001911E4[];
extern struct character *player_character;
extern short D_00195F28;
extern unsigned char D_0019626D;
extern unsigned char D_0019626E;
extern void fatal_error(char *);
extern void item_set_race_image(struct item *, int);
extern void func_0005E636(struct item *);
extern void item_roll_material(struct item *);
extern void item_roll_armor_type(struct item *);
extern void item_init_book(struct item *, short);
extern void item_make_magic(struct item *, int);
extern int rand_range(int, int);
extern int rand(void);
extern void mc_memset(void *, int, int, char *, int, int);
extern void mc_strncpy(char *, char *, int, char *, int);
#pragma aux mc_set_location parm routine [];
extern void mc_set_location(int, char *);
#pragma aux func_000A18C3 parm routine [];
extern int func_000A18C3(char *);
extern int mc_sprintf(char *, char *, ...);

#define PFLAGS (player_character->flags)

void item_init_from_template(unsigned short template_id, short group, short index, struct item *item)
{
    struct item_template *template;
    unsigned short requested_id;

    requested_id = template_id;
    if (group == 13) {
        index = 0;
        template_id = 284;
    }
    if (group == 6 && ((unsigned short)PFLAGS & 1) != 0)
        group = 12;
    if (group == 12 && ((unsigned short)PFLAGS & 1) == 0)
        group = 6;
    if (group == 4) {
        item_make_magic(item, -1);
        return;
    }
    if (group == 9 && (index < 2 || index == 4))
        index = 2;
    if (group == 10 && index == 11)
        index = 10;
    if (group == 7) {
        D_00190CF2++;
        template_id = 277;
        if (index > 3)
            index = rand_range(0, 3);
    }
    if (template_id >= 288) {
        mc_set_location(58, D_001758B8);
        func_000A18C3(D_001758C0);
        mc_set_location(59, D_001758B8);
        mc_sprintf(D_001911E4, D_001758C1, requested_id, template_id);
        fatal_error(D_001911E4);
    }
    template = &item_templates[template_id];
    if (template->inventory_image == 32512)
        item->index = 0;
    mc_strncpy(item->name, template->name, 32, D_001758B8, 68);
    item->group = group;
    item->index = index;
    item->value = template->value;
    if (template->capacity != 0 && (template->flags & 1) != 0) {
        D_0019626E = template->capacity;
        *(short *)item->pad28 = 0;
    } else {
        D_0019626E = 0;
        *(short *)item->pad28 = template->capacity;
    }
    item->item_flags = (unsigned short)template->flags;
    item->condition = item->max_condition = template->condition;
    item->magicka_bonus = 0;
    if (template->inventory_image != 0 && template->dropped_image == 0)
        item->dropped_image = template->inventory_image;
    if (template->dropped_image != 0 && template->inventory_image == 0)
        item->inventory_image = template->dropped_image;
    if (template->inventory_image != 0)
        item->inventory_image = template->inventory_image;
    if (template->dropped_image != 0)
        item->dropped_image = template->dropped_image;
    if (((unsigned short)item->inventory_image & -128) == 31360 && ((unsigned short)PFLAGS & 1) == 0) {
        item->inventory_image &= 127;
        item->inventory_image |= 31872;
    }
    item->material = item->armor_type = 0;
    if (group == 1 && (index == 4 || index == 5)) {
        if ((rand() & 3) != 0) {
            if (index == 4)
                item->color = (rand() & 1) + 24;
            else
                item->color = (rand() & 1) + 26;
        }
    } else {
        item->color = 18;
    }
    item->weight = template->weight;
    item->enchant_points = template->enchant_points;
    item->variants = template->variants;
    item->draw_order = template->draw_order;
    mc_memset(item->enchantments, -1, 40, D_001758B8, 118, 40);
    D_0019626D = template->rarity;
    D_00195F28 = template->draw_order;
    if (group == 27 && index == 4)
        item->stack_count = rand() % 20;
    if (group == 6 || group == 12 || group == 2) {
        func_0005E636(item);
        item_set_race_image(item, player_character->race);
    }
    if (group == 3)
        item_roll_material(item);
    if (group == 2) {
        item_roll_armor_type(item);
        if (item->index != 5 && item->index < 7 && item->material == 2)
            item_roll_material(item);
    }
    if (group == 3 && index == 18) {
        item->stack_count = rand_range(1, 20);
        item->condition = 0;
    }
    if (group == 7)
        item_init_book(item, index);
    if (group == 13)
        item->message = rand();
}
