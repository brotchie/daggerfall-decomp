/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005B828 */
#include "records.h"

struct concealment { int id; int mask; };      /* effect id, its conditions bit */
extern struct concealment spell_concealment_effects[];
extern struct record *scratch_object;
extern short spell_effect_slot;
extern struct spell *spell_find_active_effect(struct record *, int, int *, int *);
extern void spell_end(struct record *);
extern void spfx_effect_end(struct spell *, int, struct record *);

void spell_break_concealment(struct record *entity)
{
    struct spell *spell;
    struct character *entity_char;
    int chance;
    int i;

    entity_char = &entity->data.character;
    if ((entity_char->conditions & 0x3004) == 0) return;
    for (i = 0; i < 3; i++) {
        spell = spell_find_active_effect(entity, spell_concealment_effects[i].id, &chance, &chance);
        if (spell == 0) {
            entity_char->conditions &= ~spell_concealment_effects[i].mask;
            continue;
        }
        if (spell->effects[spell_effect_slot].subtype == 0) {
            spfx_effect_end(spell, spell_effect_slot, entity);
            if (spell_effect_slot == 0 && spell->effects[1].type == 255)
                spell_end(scratch_object);
        }
    }
}
