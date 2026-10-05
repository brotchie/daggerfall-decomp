/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000395AA */
#include "records.h"

extern struct spell *selected_spell;    /* current spell */
extern short spell_effect_slot;            /* current effect */
extern short spell_effect_cost_current[];          /* cost factors */

int spell_cost_duration(void)
{
    short cost;

    cost = (signed char)selected_spell->durations[spell_effect_slot].base * spell_effect_cost_current[0];
    cost += ((signed char)selected_spell->durations[spell_effect_slot].plus / (signed char)selected_spell->durations[spell_effect_slot].per_level) * spell_effect_cost_current[1];
    return cost;
}
