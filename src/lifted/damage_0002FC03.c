/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char monster_category[];

extern int damage_apply(struct record *, int, int);
extern void item_damage(struct record *, int);

void damage_namira_reflect(struct record *a1, struct record *a2, int a3)
{
    struct character *l_1C;
    int l_18;
    struct item *l_14;
    struct record *l_10;

    l_1C = &a2->data.character;
    for (l_18 = 0; l_18 < 27; l_18++) {
        if (l_1C->equipped[l_18] == 0) continue;
        l_14 = &l_1C->equipped[l_18]->data.item;
        if (l_14->enchantments[0].type == 26 && l_14->enchantments[0].param == 7) {
            l_10 = l_1C->equipped[l_18];
            l_1C = &a1->data.character;
            if (l_1C->race == 2) return;
            if (l_1C->race >= 43) {
                item_damage(l_10, a3);
                damage_apply(a1, a3, 0);
                return;
            }
            switch (*(unsigned char *)(monster_category + l_1C->race)) {
            case 3:
                return;
            case 2:
                item_damage(l_10, a3);
                damage_apply(a1, a3, 0);
                return;
            case 1:
                item_damage(l_10, a3 >> 1);
                damage_apply(a1, a3, 0);
                return;
            case 0:
                item_damage(l_10, a3 * 2);
                damage_apply(a1, a3, 0);
                return;
            }
        }
    }
}
