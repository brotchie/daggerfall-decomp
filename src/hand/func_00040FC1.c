/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00040FC1 */
#include "records.h"
#include "clib.h"

extern struct region regions[];
extern struct record *player_entity;
extern struct character *player_character;
extern unsigned char current_region;
extern unsigned char crime_current;
extern int court_open(int);
extern void damage_creature_death(struct record *);

void crime_guards_or_court(int surrendered)
{
    int reputation;

    reputation = regions[current_region].legal_reputation;
    player_character->health = 1;
    if (reputation < -20 && surrendered == 0) {
        damage_creature_death(player_entity);
    } else if (reputation >= -20 && reputation <= 0) {
        if ((rand() & 1) && surrendered == 0)
            damage_creature_death(player_entity);
        else
            court_open(crime_current);
    } else {
        court_open(crime_current);
    }
}
