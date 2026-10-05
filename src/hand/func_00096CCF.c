/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00096CCF */
#include "records.h"

extern char D_0017704C[];
extern struct record *location_object;
extern struct spell *spell_records;
extern int D_00195B44;
extern struct character *player_character;
extern int game_minutes;
extern int spell_cost(int, int);
extern int cast_item_spell_at();
extern void item_damage(int, int);
extern int object_create_child(int, int, int);
extern int mc_memcpy();

struct S89 { char p[73]; unsigned char f; char q[15]; };
struct E4 { short t; short v; };

void item_apply_equip_effects(int object, int slot)
{
    int i;
    int j;
    int item;
    int spell_object;
    int spell;

    i = 0;
    item = object + 71;
    spell = 0;
    while (i < 10 && ((int)(short)*(short *)((char *)((i << 2) + item) + 67)) != (-1)) {
        switch (*(unsigned short *)((char *)((i << 2) + item) + 67)) {
        case 1:
            j = 0;
            while ((*(struct S89 **)((char *)&spell_records))[j].f != ((struct E4 *)(item + 67))[i].v) {
                j++;
            }
            spell_object = object_create_child((int)location_object, 0, 89);
            *(signed char *)((char *)spell_object) = 9;
            *(short *)((char *)spell_object + 21) = 3;
            mc_memcpy(spell_object + 71, (int)((char *)spell_records + (j * 89)), 89, (int)D_0017704C, 2092, 4);
            spell = spell_object + 71;
            *(signed char *)((char *)spell + 72) = *(signed char *)&slot + 200;
            for (j = 0; j < 3; j++) {
                if (((int)(unsigned char)*(signed char *)((char *)((j * 2) + spell))) != 255) {
                    *(signed char *)((char *)((j * 3) + spell) + 14) = 255;
                    *(signed char *)((char *)((j * 3) + spell) + 15) = 0;
                    *(signed char *)((char *)((j * 3) + spell) + 16) = 0;
                }
            }
            cast_item_spell_at(spell_object);
            break;
        case 5:
            D_00195B44 = game_minutes;
            break;
        case 9:
            player_character->conditions |= 0x200;
            break;
        case 10:
            player_character->skills[(int)(short)*(short *)((char *)((i << 2) + item) + 69)].value += 15;
        }
        i++;
    }
    if (i == 0 || spell == 0) return;
    item_damage(object, spell_cost(spell, (int)player_character));
}
