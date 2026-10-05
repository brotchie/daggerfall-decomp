/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00096CCF */
#include "records.h"

extern char D_0017704C[];
extern struct record *location_object;
extern struct spell *spell_records;
extern int D_00195B44;
extern struct character *player_character;
extern int game_minutes;
extern int spell_cost(struct spell *, struct character *);
extern int cast_item_spell_at();
extern void item_damage(struct record *, int);
extern struct record *object_create_child(struct record *, struct record *, int);
extern int mc_memcpy();


void item_apply_equip_effects(struct record *object, int slot)
{
    int i;
    int j;
    struct item *item;
    struct record *spell_object;
    struct spell *spell;

    i = 0;
    item = &object->data.item;
    spell = 0;
    while (i < 10 && item->enchantments[i].type != (-1)) {
        switch ((unsigned short)item->enchantments[i].type) {
        case 1:
            j = 0;
            while (spell_records[j].id != item->enchantments[i].param) {
                j++;
            }
            spell_object = object_create_child(location_object, 0, 89);
            spell_object->type = 9;
            spell_object->flags = 3;
            mc_memcpy(&spell_object->data.spell, &spell_records[j], 89, (int)D_0017704C, 2092, 4);
            spell = &spell_object->data.spell;
            spell->icon = *(signed char *)&slot + 200;
            for (j = 0; j < 3; j++) {
                if (spell->effects[j].type != 255) {
                    spell->durations[j].base = -1;
                    spell->durations[j].plus = 0;
                    spell->durations[j].per_level = 0;
                }
            }
            cast_item_spell_at(spell_object);
            break;
        case 5:
            D_00195B44 = game_minutes;
            break;
        case 9:
            player_character->conditions |= 0x200;
            break;
        case 10:
            player_character->skills[item->enchantments[i].param].value += 15;
        }
        i++;
    }
    if (i == 0 || spell == 0) return;
    item_damage(object, spell_cost(spell, player_character));
}
