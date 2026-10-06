/* matched by the real Watcom C32 10.0a (-d2): a run of equip.c from 0x00060430 to 0x0006077F, kept together for its switch table's alignment */
#include "records.h"
#include "clib.h"

extern char D_001758B8[];        /* __FILE__ */
extern unsigned char D_001865CA[];
extern unsigned char D_00186634[];
extern struct character *player_character;
extern int magic_def_count;
extern struct magic_template *magic_def;
extern int enchant_item_value(struct item *);
extern void item_make_random(unsigned short, struct item *);
extern void item_make(int, int, struct item *);
extern void item_roll_material(struct item *);
extern int armor_image_for_type(int, int);
extern int rand_range(int, int);

void item_make_magic(struct item *item, int which)
{
    int n;
    int i;
    int kind;
    int j;
    int r;

    if (which == -1) {
        n = i = 0;
        for (; i < magic_def_count; i++)
            if (magic_def[i].artifact == 0)
                n++;
        n = rand_range(0, n - 1);
    } else {
        n = which;
    }
    for (i = 0; i < magic_def_count; i++) {
        if (magic_def[i].artifact == 0) {
            if (n == 0)
                break;
            n--;
        }
    }
    switch (magic_def[i].group) {
    case 0:
        kind = D_001865CA[rand_range(0, 6)];
        break;
    case 1:
        kind = D_00186634[rand_range(0, 4)];
        break;
    case 2:
        kind = 3;
        break;
    }
    if (kind == 12 || kind == 6) {
        if (player_character->flags & 1)
            kind = 12;
        else
            kind = 6;
    }
    do
        item_make_random(kind, item);
    while (item->group == 3 && item->index == 18);
    mc_strncpy(item->name, magic_def[i].name, 32, D_001758B8, 1039);
    for (j = 0; j < 10; j++) {
        if (magic_def[i].enchantments[j].type == 255)
            break;
        if (magic_def[i].enchantments[j].param == 255) {
            item->enchantments[j].type = magic_def[i].enchantments[j].type;
            item->enchantments[j].param = 0xffff;
        } else {
            item->enchantments[j].type = magic_def[i].enchantments[j].type;
            item->enchantments[j].param = magic_def[i].enchantments[j].param;
        }
    }
    item->condition = item->max_condition = magic_def[i].condition;
    item->material = magic_def[i].material;
    if (item->material == 0 && (item->group == 2 || item->group == 3))
        item_roll_material(item);
    if (item->group == 2) {
        item->armor_type = 2;
        r = armor_image_for_type(2, item->index);
        if (r != -1)
            item->inventory_image = r + (item->inventory_image & -128);
    }
    item->value = enchant_item_value(item);
}

void item_make_artifact(struct item *item, int which)
{
    int n;
    int i;
    int j;
    int sel;

    if (which == -1) {
        n = i = 0;
        for (; i < magic_def_count; i++)
            if (magic_def[i].artifact)
                n++;
        n = rand_range(0, n - 1);
    } else {
        n = which;
    }
    sel = n;
    for (i = 0; i < magic_def_count; i++) {
        if (magic_def[i].artifact) {
            if (n == 0)
                break;
            n--;
        }
    }
    item_make(magic_def[i].group, magic_def[i].group_index, item);
    mc_strncpy(item->name, magic_def[i].name, 32, D_001758B8, 1094);
    for (j = 0; j < 10; j++) {
        if (magic_def[i].enchantments[j].type == 255)
            break;
        if (magic_def[i].enchantments[j].param == 255) {
            item->enchantments[j].type = magic_def[i].enchantments[j].type;
            item->enchantments[j].param = 0xffff;
        } else {
            item->enchantments[j].type = magic_def[i].enchantments[j].type;
            item->enchantments[j].param = magic_def[i].enchantments[j].param;
        }
    }
    item->condition = item->max_condition = magic_def[i].condition;
    item->value = magic_def[i].value;
    item->material = magic_def[i].material;
    if (item->group == 2)
        item->armor_type = 2;
    item->item_flags |= 0x820;
    item->enchantments[9].param = sel;
    switch (i) {
    case 0:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 12;
        break;
    case 1:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 13;
        item->item_flags |= 4;
        break;
    case 2:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 10;
        item->item_flags |= 4;
        break;
    case 3:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 8;
        break;
    case 4:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 19;
        break;
    case 5:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 16;
        break;
    case 6:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 25;
        break;
    case 7:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 18;
        break;
    case 8:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 21;
        break;
    case 9:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 2;
        break;
    case 46:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 24;
        break;
    case 47:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 26;
        break;
    case 48:
        item->inventory_image = (player_character->flags & 1) != 0 ? 0xd880 : 0xd800;
        break;
    case 49:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 15;
        break;
    case 50:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 3;
        break;
    case 51:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 9;
        break;
    case 52:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 23;
        break;
    case 53:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 17;
        break;
    case 54:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 7;
        break;
    case 55:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 1;
        break;
    case 56:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 22;
        break;
    case 57:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 20;
        break;
    case 58:
        item->inventory_image = ((player_character->flags & 1) != 0 ? 0xd880 : 0xd800) + 5;
        item->item_flags |= 4;
        break;
    }
}
