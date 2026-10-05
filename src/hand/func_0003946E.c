/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003946E */
#include "records.h"

extern struct spell *selected_spell;
extern short spell_effect_slot;
extern short spell_effect_cost_current;
extern short D_0019961E;
extern short D_00199620;
extern short D_00199622;

int spell_cost_duration_magnitude(void)
{
    short v;

    v = spell_effect_cost_current * selected_spell->durations[spell_effect_slot].base;
    v += D_0019961E * (selected_spell->durations[spell_effect_slot].plus / selected_spell->durations[spell_effect_slot].per_level);
    v += D_00199620 * ((selected_spell->magnitudes[spell_effect_slot].base_min + selected_spell->magnitudes[spell_effect_slot].base_max) / 2);
    v += ((selected_spell->magnitudes[spell_effect_slot].plus_min + selected_spell->magnitudes[spell_effect_slot].plus_max) / 2 / selected_spell->magnitudes[spell_effect_slot].per_level) * D_00199622;
    return v;
}
