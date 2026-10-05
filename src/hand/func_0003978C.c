/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003978C */
struct spell_chance { signed char base; signed char plus; signed char per_level; };
struct spell_dur { signed char base; signed char plus; signed char per_level; };
struct spell_mag {
    signed char base_min;
    signed char base_max;
    signed char plus_min;
    signed char plus_max;
    signed char per_level;
};
struct spell {
    char pad[14];
    struct spell_chance chance[3];  /* 14 */
    struct spell_dur dur[3];        /* 23 */
    struct spell_mag mag[3];        /* 32 */
};
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
    cost = ((sp->mag[e].base_max + sp->mag[e].base_min) >> 1) * spell_effect_cost_current[2];
    cost += ((sp->mag[e].plus_min + sp->mag[e].plus_max) >> 1) * (spell_effect_cost_current[3] / sp->mag[e].per_level);
    cost += sp->chance[e].base * spell_effect_cost_current[0];
    cost += sp->chance[e].plus * spell_effect_cost_current[1] / sp->chance[e].per_level;
    return cost;
}
