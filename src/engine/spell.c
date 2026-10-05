/* spell.c: the game's spell thunks in XnGine's object as readable C (xspell.h; see xngine.h).
   The dispatchers jump into the handler (xn_tail_jump: see xsysutil.h), which then returns
   straight to the caller, on the caller's stack, as with the asm. */
#include "xspell.h"

s32 spell_cost_formula_dispatch(s32 formula)
{
    return xn_tail_jump(spell_cost_formulas[formula]);
}

s32 spell_effect_dispatch(s32 type, void *spell, s32 slot, void *target)
{
    return xn_tail_jump3(spell_effect_handlers[type], (s32)spell, slot, (s32)target);
}

void xn_spell_kludge_menu_dispatch(s32 row)
{
    xn_tail_jump1(xn_kludge_menu_handlers[row], 1);
}

s32 spell_find_effect_type(const u8 *effects, u8 type)
{
    s32 slot;

    for (slot = 0; slot < 3; slot++)
        if (effects[2 * slot] == type)
            return slot + 1;
    return 0;
}

s32 spell_has_no_effects(const u8 *effects)
{
    return effects[0] == 0xFF && effects[2] == 0xFF && effects[2] == 0xFF;
}
