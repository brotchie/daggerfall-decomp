/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00079913 */
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
struct skill { short v; short f2; short f4; };
struct player {
    char pad0[42];
    short f42;                  /* 0x2a */
    char pad2c[113];
    struct skill skills[1];     /* 0x9d */
};
struct mobile {
    char pad0[65];
    struct bits8 f65;           /* 0x41 */
    char pad42;
    unsigned char f67;          /* 0x43 */
};
struct thing { unsigned char type; char pad[70]; struct mobile mob; };
extern unsigned char monster_language_skill[];
extern struct bits8 D_001940D6;
extern int player_entity;
extern struct player *player_character;
extern void skill_add_uses(int, int);
extern int rand_range(int, int);
extern int spell_active_chance(int, unsigned char, unsigned char);

void monster_pacify_check(struct thing *a1)
{
    struct mobile *m;
    int chance;
    int sk;

    m = &a1->mob;
    chance = D_001940D6.b6 ? -25 : 10;
    if (m->f67 >= 43)
        chance += player_character->f42 / 5 + player_character->skills[1].v / 10;
    else {
        sk = monster_language_skill[m->f67];
        if (sk != 0) {
            chance += player_character->skills[sk].v;
            skill_add_uses(sk, 1);
        }
        chance += spell_active_chance(player_entity, 44, 255);
        chance += player_character->f42 / 5;
    }
    if (rand_range(1, 200) <= chance)
        m->f65.b7 = 1;
}
