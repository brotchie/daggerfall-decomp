/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char monster_category[];

extern int damage_apply(struct record *, int, struct record *);
extern void item_damage(struct record *, int);

void damage_namira_reflect(struct record *attacker, struct record *target, int damage)
{
    struct character *ch;
    int slot;
    struct item *item_data;
    struct record *item;

    ch = &target->data.character;
    for (slot = 0; slot < 27; slot++) {
        if (ch->equipped[slot] == 0) continue;
        item_data = &ch->equipped[slot]->data.item;
        if (item_data->enchantments[0].type == 26 && item_data->enchantments[0].param == 7) {
            item = ch->equipped[slot];
            ch = &attacker->data.character;
            if (ch->race == 2) return;
            if (ch->race >= 43) {
                item_damage(item, damage);
                damage_apply(attacker, damage, 0);
                return;
            }
            switch (*(unsigned char *)(monster_category + ch->race)) {
            case 3:
                return;
            case 2:
                item_damage(item, damage);
                damage_apply(attacker, damage, 0);
                return;
            case 1:
                item_damage(item, damage >> 1);
                damage_apply(attacker, damage, 0);
                return;
            case 0:
                item_damage(item, damage * 2);
                damage_apply(attacker, damage, 0);
                return;
            }
        }
    }
}
