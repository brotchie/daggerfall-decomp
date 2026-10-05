/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00039635 */
struct dur { signed char base; signed char plus; signed char per_level; };
struct spell {
    char pad[23];
    struct dur dur[3];
};
extern struct spell *selected_spell;
extern short spell_effect_slot;
extern short spell_effect_cost_current[];

int spell_cost_chance(void)
{
    short cost;

    cost = spell_effect_cost_current[0] * selected_spell->dur[spell_effect_slot].base;
    cost += (selected_spell->dur[spell_effect_slot].plus / selected_spell->dur[spell_effect_slot].per_level) * spell_effect_cost_current[1];
    return cost;
}
