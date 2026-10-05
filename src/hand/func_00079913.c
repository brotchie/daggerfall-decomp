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

void monster_pacify_check(struct record *monster)
{
    struct character *monster_char;
    int chance;
    int skill;

    monster_char = &monster->data.character;
    chance = D_001940D6.b6 ? -25 : 10;
    if (monster_char->race >= 43)
        chance += player_character->attributes[ATTR_PER] / 5 + player_character->skills[SKILL_ETIQUETTE].value / 10;
    else {
        skill = monster_language_skill[monster_char->race];
        if (skill != 0) {
            chance += player_character->skills[skill].value;
            skill_add_uses(skill, 1);
        }
        chance += spell_active_chance(player_entity, 44, 255);
        chance += player_character->attributes[ATTR_PER] / 5;
    }
    if (rand_range(1, 200) <= chance)
        monster_char->flags |= 0x8000;
}
