/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00071606 */
#include "records.h"

extern struct character *player_character;
extern int game_minutes;
extern void skill_add_uses(int, int);
extern int player_in_daylight(void);
extern void fatigue_add(int);

void rest_recover(struct record *object)
{
    struct character *character;
    struct career *career;
    int heal;

    character = &object->data.character;
    career = &character->career;
    if (character->race == 8 && (unsigned)(game_minutes - player_character->last_kill_time) > 960)
        return;
    heal = 60;
    if (career->rapid_healing_flags != 0) {
        if (career->rapid_healing_flags & 4)
            heal += 40;
        else if ((career->rapid_healing_flags & 1) && player_in_daylight() != 0)
            heal += 40;
        else if ((career->rapid_healing_flags & 2) && player_in_daylight() == 0)
            heal += 40;
    }
    heal += player_character->skills[SKILL_MEDICAL].value;
    skill_add_uses(0, 1);
    heal = character->max_health * heal / 1000 + (character->attributes[ATTR_END] - 50) / 10;
    if (heal < 1)
        heal = 1;
    character->health += heal;
    if (character->health > character->max_health)
        character->health = character->max_health;
    fatigue_add((player_character->base_attributes[ATTR_STR] + player_character->base_attributes[ATTR_END]) << 6 >> 3);
    if (!(career->flags & 8) && character->magicka < character->max_magicka) {
        character->magicka += character->max_magicka >> 3;
        if (character->magicka > character->max_magicka)
            character->magicka = character->max_magicka;
    }
}
