/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088E62 */
#include "records.h"
extern int D_00185097;
extern struct character *player_character;
extern void spell_remove_effect_type(struct record *, int);
extern int hud_message_add(int);
extern int rand_range(int, int);
extern void spfx_cure_disease(struct record *, struct character *);
extern struct record *object_free_single(struct record *);

int spfx_cure(struct record *a1, int a2, struct record *a3)
{
    struct spell *l_28;
    struct character *l_24;
    struct record *l_20;
    int l_1C;
    struct disease *l_18;
    int l_14;

    l_28 = &a1->data.spell;
    l_24 = &a3->data.character;
    if (rand_range(1, 100) > l_28->cast_chances[a2]) {
        hud_message_add(D_00185097);
        return 0;
    }
    switch (l_28->effects[a2].subtype) {
    case 0:
        spfx_cure_disease(a3, l_24);
        if (player_character->special_infection_time != 0) {
            player_character->special_infection_time = 0;
            player_character->special_infection = 0;
        }
        break;
    case 1:
        l_20 = a3->children;
        while (l_20 != 0) {
            if (l_20->type == 11) {
                l_18 = &l_20->data.disease;
                if (l_18->id > 127) {
                    for (l_14 = 0; l_14 < 8; l_14++) {
                        l_24->attributes[l_14] += l_18->drained[l_14];
                        if (l_24->attributes[l_14] > l_24->base_attributes[l_14])
                            l_24->attributes[l_14] = l_24->base_attributes[l_14];
                    }
                    l_20 = object_free_single(l_20);
                } else {
                    l_20 = l_20->next;
                }
            } else {
                l_20 = l_20->next;
            }
        }
        break;
    case 2:
        if ((l_24->conditions & 1) == 0) return 0;
        spell_remove_effect_type(a3, 0);
        l_24->conditions &= ~1;
    case 3:
        break;
    }
    return 0;
}
