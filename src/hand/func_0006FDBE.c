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
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    if (object_find_item(player_entity, 27, 0) == 0)
        return 0;
    picklist_init(shared_picklist, 27, 30, 111, 131, 144, 29, 8, 15, 144, 150, 8, 15, 144, 45, 9, 104, 146, 146, 244, 114, 0);
    l_28 = current_building->quality * 5 + 30;
    while (l_24 == 0) {
        for (l_2C = 0; l_2C < spell_record_count; l_2C++) {
            if (spell_records[l_2C].name[0] == 0) continue;
            if ((unsigned char)spell_records[l_2C].name[0] == 33) continue;
            l_20 = spell_cost(&spell_records[l_2C], player_character);
            while (l_20 > 95)
                l_20 >>= 1;
            l_1C = l_28 - l_20;
            if (l_1C < 5)
                l_1C = 5;
            if (rand_range(1, 50) < l_1C) {
                picklist_add(shared_picklist, spell_records[l_2C].name, 0);
                scratch_buffer[l_24 + 20000] = l_2C;
                l_24++;
            }
        }
    }
    return 1;
}
