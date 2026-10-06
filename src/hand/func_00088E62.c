/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088E62 */
#include "records.h"
extern iptr D_00185097;
extern struct character *player_character;
extern void spell_remove_effect_type(struct record *, int);
extern iptr hud_message_add(iptr);
extern int rand_range(int, int);
extern void spfx_cure_disease(struct record *, struct character *);
extern struct record *object_free_single(struct record *);

int spfx_cure(struct record *spell_object, int effect, struct record *target)
{
    struct spell *spell;
    struct character *character;
    struct record *child;
    int unused;
    struct disease *poison;
    int i;

    spell = &spell_object->data.spell;
    character = &target->data.character;
    if (rand_range(1, 100) > spell->cast_chances[effect]) {
        hud_message_add(D_00185097);
        return 0;
    }
    switch (spell->effects[effect].subtype) {
    case 0:
        spfx_cure_disease(target, character);
        if (player_character->special_infection_time != 0) {
            player_character->special_infection_time = 0;
            player_character->special_infection = 0;
        }
        break;
    case 1:
        child = target->children;
        while (child != 0) {
            if (child->type == 11) {
                poison = &child->data.disease;
                if (poison->id > 127) {
                    for (i = 0; i < 8; i++) {
                        character->attributes[i] += poison->drained[i];
                        if (character->attributes[i] > character->base_attributes[i])
                            character->attributes[i] = character->base_attributes[i];
                    }
                    child = object_free_single(child);
                } else {
                    child = child->next;
                }
            } else {
                child = child->next;
            }
        }
        break;
    case 2:
        if ((character->conditions & 1) == 0) return 0;
        spell_remove_effect_type(target, 0);
        character->conditions &= ~1;
    case 3:
        break;
    }
    return 0;
}
