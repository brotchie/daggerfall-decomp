/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003988A */
#include "records.h"

extern struct spell *selected_spell;    /* current spell */
extern short spell_effect_slot;            /* current effect */
extern short spell_effect_cost_current[];          /* cost factors */

int func_0003988A(void)
{
    short cost;

    cost = ((selected_spell->magnitudes[spell_effect_slot].plus_min + selected_spell->magnitudes[spell_effect_slot].plus_max) / 2 / selected_spell->magnitudes[spell_effect_slot].per_level) * spell_effect_cost_current[1];
    cost += (selected_spell->magnitudes[spell_effect_slot].base_min + selected_spell->magnitudes[spell_effect_slot].base_max) / 2 * spell_effect_cost_current[0];
    cost = cost * selected_spell->durations[spell_effect_slot].base / selected_spell->durations[spell_effect_slot].plus;
    return cost;
}
