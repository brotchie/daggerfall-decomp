/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00040FC1 */
struct row { short v; char pad[78]; };
struct ent { char pad[0x7c]; short f7c; };
extern struct row region_legal_reputation[];
extern int player_entity;
extern struct ent *player_character;
extern unsigned char current_region;
extern unsigned char crime_current;
extern int court_open(unsigned char);
extern void damage_creature_death(int);
extern int rand(void);

void crime_guards_or_court(int a1)
{
    int v;

    v = region_legal_reputation[current_region].v;
    player_character->f7c = 1;
    if (v < -20 && a1 == 0) {
        damage_creature_death(player_entity);
    } else if (v >= -20 && v <= 0) {
        if ((rand() & 1) && a1 == 0)
            damage_creature_death(player_entity);
        else
            court_open(crime_current);
    } else {
        court_open(crime_current);
    }
}
