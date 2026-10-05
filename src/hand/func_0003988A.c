/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003988A */
struct spell_tri {
    signed char base;
    signed char plus;
    signed char per_level;
};
/* spell effect magnitude: base min/max, plus min/max, per level */
struct spell_mag {
    signed char base_min;
    signed char base_max;
    signed char plus_min;
    signed char plus_max;
    signed char per_level;
};
struct spell {
    char pad[14];
    struct spell_tri dur[3];
    struct spell_tri chance[3];
    struct spell_mag mag[3];
};
extern struct spell *selected_spell;    /* current spell */
extern short spell_effect_slot;            /* current effect */
extern short spell_effect_cost_current[];          /* cost factors */

int func_0003988A(void)
{
    short cost;

    cost = ((selected_spell->mag[spell_effect_slot].plus_min + selected_spell->mag[spell_effect_slot].plus_max) / 2 / selected_spell->mag[spell_effect_slot].per_level) * spell_effect_cost_current[1];
    cost += (selected_spell->mag[spell_effect_slot].base_min + selected_spell->mag[spell_effect_slot].base_max) / 2 * spell_effect_cost_current[0];
    cost = cost * selected_spell->dur[spell_effect_slot].base / selected_spell->dur[spell_effect_slot].plus;
    return cost;
}
