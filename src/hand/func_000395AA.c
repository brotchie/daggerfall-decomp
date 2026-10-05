/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000395AA */
/* spell effect duration: base, plus, per level */
struct spell_dur {
    signed char base;
    signed char plus;
    signed char per_level;
};
struct spell {
    char pad[14];
    struct spell_dur dur[3];
};
extern struct spell *selected_spell;    /* current spell */
extern short spell_effect_slot;            /* current effect */
extern short spell_effect_cost_current[];          /* cost factors */

int spell_cost_duration(void)
{
    short cost;

    cost = selected_spell->dur[spell_effect_slot].base * spell_effect_cost_current[0];
    cost += (selected_spell->dur[spell_effect_slot].plus / selected_spell->dur[spell_effect_slot].per_level) * spell_effect_cost_current[1];
    return cost;
}
