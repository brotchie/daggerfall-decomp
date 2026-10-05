/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003937B */
struct spell_chance { signed char base; signed char plus; signed char per_level; };
struct spell_dur { signed char base; signed char plus; signed char per_level; };
struct spell {
    char pad[14];
    struct spell_chance chance[3];
    struct spell_dur dur[3];
};
extern struct spell *selected_spell;
extern short spell_effect_slot;
extern short spell_effect_cost_current[4];

int spell_cost_duration_chance(void)
{
    short cost;

    cost = spell_effect_cost_current[0] * selected_spell->chance[spell_effect_slot].base;
    cost += (selected_spell->chance[spell_effect_slot].plus / selected_spell->chance[spell_effect_slot].per_level) * spell_effect_cost_current[1];
    cost += selected_spell->dur[spell_effect_slot].base * spell_effect_cost_current[2];
    cost += (selected_spell->dur[spell_effect_slot].plus / selected_spell->dur[spell_effect_slot].per_level) * spell_effect_cost_current[3];
    return cost;
}
