/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003946E */
struct e3 { signed char a, b, c; };
struct e5 { signed char a, b, c, d, e; };
struct tbl { char pad[14]; struct e3 t3[6]; struct e5 t5[1]; };
extern struct tbl *selected_spell;
extern short spell_effect_slot;
extern short spell_effect_cost_current;
extern short D_0019961E;
extern short D_00199620;
extern short D_00199622;

int spell_cost_duration_magnitude(void)
{
    short v;

    v = spell_effect_cost_current * selected_spell->t3[spell_effect_slot].a;
    v += D_0019961E * (selected_spell->t3[spell_effect_slot].b / selected_spell->t3[spell_effect_slot].c);
    v += D_00199620 * ((selected_spell->t5[spell_effect_slot].a + selected_spell->t5[spell_effect_slot].b) / 2);
    v += ((selected_spell->t5[spell_effect_slot].c + selected_spell->t5[spell_effect_slot].d) / 2 / selected_spell->t5[spell_effect_slot].e) * D_00199622;
    return v;
}
