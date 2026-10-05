/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005B828 */
#include "records.h"

struct concealment { int id; int mask; };      /* effect id, its conditions bit */
extern struct concealment spell_concealment_effects[];
extern struct record *scratch_object;
extern short spell_effect_slot;
extern struct spell *spell_find_active_effect(struct record *, int, int *, int *);
extern void spell_end(struct record *);
extern void spfx_effect_end(struct spell *, int, struct record *);

void spell_break_concealment(struct record *a1)
{
    struct spell *l_24;
    struct character *l_20;
    int l_1C;
    int l_18;

    l_20 = &a1->data.character;
    if ((l_20->conditions & 0x3004) == 0) return;
    for (l_18 = 0; l_18 < 3; l_18++) {
        l_24 = spell_find_active_effect(a1, spell_concealment_effects[l_18].id, &l_1C, &l_1C);
        if (l_24 == 0) {
            l_20->conditions &= ~spell_concealment_effects[l_18].mask;
            continue;
        }
        if (l_24->effects[spell_effect_slot].subtype == 0) {
            spfx_effect_end(l_24, spell_effect_slot, a1);
            if (spell_effect_slot == 0 && l_24->effects[1].type == 255)
                spell_end(scratch_object);
        }
    }
}
