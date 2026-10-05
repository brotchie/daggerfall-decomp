/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006FDBE */
#include "records.h"

extern struct building *current_building;
extern struct record *player_entity;
extern struct spell *spell_records;
extern struct character *player_character;
extern char *scratch_buffer;
extern int spell_record_count;
extern char shared_picklist[];
extern int spell_cost(struct spell *, struct character *);
extern int rand_range(int, int);
extern void picklist_init(char *, short, short, int, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void picklist_add(char *, char *, int);
extern struct record *object_find_item(struct record *, int, int);

int spellshop_build_list(void)
{
    int unused;
    int i;
    int base_chance;
    int count;
    int cost;
    int chance;

    count = 0;
    if (object_find_item(player_entity, 27, 0) == 0)
        return 0;
    picklist_init(shared_picklist, 27, 30, 111, 131, 144, 29, 8, 15, 144, 150, 8, 15, 144, 45, 9, 104, 146, 146, 244, 114, 0);
    base_chance = current_building->quality * 5 + 30;
    while (count == 0) {
        for (i = 0; i < spell_record_count; i++) {
            if (spell_records[i].name[0] == 0) continue;
            if ((unsigned char)spell_records[i].name[0] == 33) continue;
            cost = spell_cost(&spell_records[i], player_character);
            while (cost > 95)
                cost >>= 1;
            chance = base_chance - cost;
            if (chance < 5)
                chance = 5;
            if (rand_range(1, 50) < chance) {
                picklist_add(shared_picklist, spell_records[i].name, 0);
                scratch_buffer[count + 20000] = i;
                count++;
            }
        }
    }
    return 1;
}
