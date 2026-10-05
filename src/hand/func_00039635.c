/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00039635 */
#include "records.h"

extern struct spell *selected_spell;
extern short spell_effect_slot;
extern short spell_effect_cost_current[];

int spell_cost_chance(void)
{
    short cost;

    cost = spell_effect_cost_current[0] * (signed char)selected_spell->chances[spell_effect_slot].base;
    cost += ((signed char)selected_spell->chances[spell_effect_slot].plus / (signed char)selected_spell->chances[spell_effect_slot].per_level) * spell_effect_cost_current[1];
    return cost;
}
