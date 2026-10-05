/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00071606 */
#include "records.h"

extern struct character *player_character;
extern int game_minutes;
extern void skill_add_uses(int, int);
extern int player_in_daylight(void);
extern void fatigue_add(int);

void rest_recover(struct record *a1)
{
    struct character *m;
    struct career *e;
    int v;

    m = &a1->data.character;
    e = &m->career;
    if (m->race == 8 && (unsigned)(game_minutes - player_character->last_kill_time) > 960)
        return;
    v = 60;
    if (e->rapid_healing_flags != 0) {
        if (e->rapid_healing_flags & 4)
            v += 40;
        else if ((e->rapid_healing_flags & 1) && player_in_daylight() != 0)
            v += 40;
        else if ((e->rapid_healing_flags & 2) && player_in_daylight() == 0)
            v += 40;
    }
    v += player_character->skills[SKILL_MEDICAL].value;
    skill_add_uses(0, 1);
    v = m->max_health * v / 1000 + (m->attributes[ATTR_END] - 50) / 10;
    if (v < 1)
        v = 1;
    m->health += v;
    if (m->health > m->max_health)
        m->health = m->max_health;
    fatigue_add((player_character->base_attributes[ATTR_STR] + player_character->base_attributes[ATTR_END]) << 6 >> 3);
    if (!(e->flags & 8) && m->magicka < m->max_magicka) {
        m->magicka += m->max_magicka >> 3;
        if (m->magicka > m->max_magicka)
            m->magicka = m->max_magicka;
    }
}
