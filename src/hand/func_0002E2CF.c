/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002E2CF */
#include "records.h"

extern int spell_points_bonus;
extern int D_00195A08;
extern int D_00195A0C;
extern int D_00195A78;
extern struct record *scratch_current_object;
extern struct character *player_character;
extern int game_minutes;
extern char D_00196291;
extern char D_00196292;
extern int damage_heal(struct character *, int);
extern int cast_item_strike_spell(short, struct record *);
extern int cast_creature_spell(struct record *, struct record *, int);
extern void item_damage(struct record *, int);
extern int rand_range(int, int);

void damage_weapon_strike_effects(struct item *a1, struct record *a2, struct record *a3, int a4)
{
    int i;
    struct character *m2;
    struct character *m3;

    if (a1->enchantments[0].type == -1)
        return;
    m2 = &a2->data.character;
    m3 = &a3->data.character;
    for (i = 0; i < 10; i++) {
        if (a1->enchantments[i].type == -1)
            return;
        if (a1->enchantments[i].type == 2) {
            D_00196292 = 1;
            D_00196291 = 1;
            if (m2 == player_character) {
                cast_item_strike_spell(a1->enchantments[i].param, a3);
                item_damage(scratch_current_object, 10);
            } else {
                cast_creature_spell(a2, a3, a1->enchantments[i].param);
                item_damage(scratch_current_object, 10);
            }
            D_00196291 = 0;
            D_00196292 = 0;
        } else if (a1->enchantments[i].type == 6 && a1->enchantments[i].param == 1) {
            item_damage(scratch_current_object, damage_heal(m2, a4 / 2) / 4 + 1);
        } else if (a1->enchantments[i].type == 26 && a1->enchantments[i].param == 2) {
            item_damage(scratch_current_object, 2);
            i = rand_range(1, 6);
            if (m3->magicka > 10) {
                m3->magicka -= i;
                if (m3->magicka < 0)
                    m3->magicka = 0;
                spell_points_bonus += i;
                i = spell_points_bonus + player_character->magicka;
                if (player_character->max_magicka < i)
                    spell_points_bonus = player_character->max_magicka - player_character->magicka;
                if (D_00195A0C == 0)
                    D_00195A0C = game_minutes + 12;
            } else if (m3->attributes[ATTR_STR] > 10) {
                m3->attributes[ATTR_STR] -= i;
                if (m3->attributes[ATTR_STR] < 0)
                    m3->attributes[ATTR_STR] = 0;
                D_00195A08 += i;
                D_00195A78 = game_minutes + 12;
            }
        }
    }
}
