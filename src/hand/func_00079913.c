/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00079913 */
#include "records.h"

struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
extern unsigned char monster_language_skill[];
extern struct bits8 D_001940D6;
extern struct record *player_entity;
extern struct character *player_character;
extern void skill_add_uses(int, int);
extern int rand_range(int, int);
extern int spell_active_chance(struct record *, unsigned char, unsigned char);

void monster_pacify_check(struct record *a1)
{
    struct character *m;
    int chance;
    int sk;

    m = &a1->data.character;
    chance = D_001940D6.b6 ? -25 : 10;
    if (m->race >= 43)
        chance += player_character->attributes[ATTR_PER] / 5 + player_character->skills[SKILL_ETIQUETTE].value / 10;
    else {
        sk = monster_language_skill[m->race];
        if (sk != 0) {
            chance += player_character->skills[sk].value;
            skill_add_uses(sk, 1);
        }
        chance += spell_active_chance(player_entity, 44, 255);
        chance += player_character->attributes[ATTR_PER] / 5;
    }
    if (rand_range(1, 200) <= chance)
        m->flags |= 0x8000;
}
