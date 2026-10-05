/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000396C0 */
#include "records.h"

extern struct spell *selected_spell;    /* current spell */
extern short spell_effect_slot;            /* current effect */
extern short spell_effect_cost_current[];          /* cost factors */

int spell_cost_magnitude(void)
{
    short cost;

    cost = (selected_spell->magnitudes[spell_effect_slot].base_min + selected_spell->magnitudes[spell_effect_slot].base_max) / 2 * spell_effect_cost_current[0];
    cost += ((selected_spell->magnitudes[spell_effect_slot].plus_min + selected_spell->magnitudes[spell_effect_slot].plus_max) / 2 / selected_spell->magnitudes[spell_effect_slot].per_level) * spell_effect_cost_current[1];
    return cost;
}
