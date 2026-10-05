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

void damage_weapon_strike_effects(struct item *weapon, struct record *attacker, struct record *target, int damage)
{
    int i;
    struct character *attacker_character;
    struct character *target_character;

    if (weapon->enchantments[0].type == -1)
        return;
    attacker_character = &attacker->data.character;
    target_character = &target->data.character;
    for (i = 0; i < 10; i++) {
        if (weapon->enchantments[i].type == -1)
            return;
        if (weapon->enchantments[i].type == 2) {
            D_00196292 = 1;
            D_00196291 = 1;
            if (attacker_character == player_character) {
                cast_item_strike_spell(weapon->enchantments[i].param, target);
                item_damage(scratch_current_object, 10);
            } else {
                cast_creature_spell(attacker, target, weapon->enchantments[i].param);
                item_damage(scratch_current_object, 10);
            }
            D_00196291 = 0;
            D_00196292 = 0;
        } else if (weapon->enchantments[i].type == 6 && weapon->enchantments[i].param == 1) {
            item_damage(scratch_current_object, damage_heal(attacker_character, damage / 2) / 4 + 1);
        } else if (weapon->enchantments[i].type == 26 && weapon->enchantments[i].param == 2) {
            item_damage(scratch_current_object, 2);
            i = rand_range(1, 6);
            if (target_character->magicka > 10) {
                target_character->magicka -= i;
                if (target_character->magicka < 0)
                    target_character->magicka = 0;
                spell_points_bonus += i;
                i = spell_points_bonus + player_character->magicka;
                if (player_character->max_magicka < i)
                    spell_points_bonus = player_character->max_magicka - player_character->magicka;
                if (D_00195A0C == 0)
                    D_00195A0C = game_minutes + 12;
            } else if (target_character->attributes[ATTR_STR] > 10) {
                target_character->attributes[ATTR_STR] -= i;
                if (target_character->attributes[ATTR_STR] < 0)
                    target_character->attributes[ATTR_STR] = 0;
                D_00195A08 += i;
                D_00195A78 = game_minutes + 12;
            }
        }
    }
}
