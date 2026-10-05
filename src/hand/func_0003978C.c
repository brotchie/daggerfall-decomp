/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003978C */
#include "records.h"

extern struct spell *selected_spell;
extern short spell_effect_slot;
extern short spell_effect_cost_current[];

int func_0003978C(void)
{
    short cost;
    struct spell *sp;
    short e;

    e = spell_effect_slot;
    sp = selected_spell;
    cost = ((sp->magnitudes[e].base_max + sp->magnitudes[e].base_min) >> 1) * spell_effect_cost_current[2];
    cost += ((sp->magnitudes[e].plus_min + sp->magnitudes[e].plus_max) >> 1) * (spell_effect_cost_current[3] / sp->magnitudes[e].per_level);
    cost += sp->durations[e].base * spell_effect_cost_current[0];
    cost += sp->durations[e].plus * spell_effect_cost_current[1] / sp->durations[e].per_level;
    return cost;
}
