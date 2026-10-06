/* spell.c: the game's spell thunks in XnGine's object (canonical C; the interface and the
   module's documentation are in xspell.h). The dispatchers are tail calls (xsysutil.h): they
   have no frame, and the handler returns straight to their caller. */
#include "xspell.h"
#include "xsysutil.h"

#define EFFECT_NONE     0xFF            /* an empty effect slot's type */
#define EFFECT_STRIDE   2               /* bytes between the effect slots */

s32 spell_cost_formula_dispatch(s32 formula)
{
    return xn_tail_jump(spell_cost_formulas[formula]);
}

s32 spell_effect_dispatch(s32 type, void *spell, s32 slot, void *target)
{
    return xn_tail_jump3(spell_effect_handlers[type], (iptr)spell, slot, (iptr)target);
}

void xn_spell_kludge_menu_dispatch(s32 row)
{
    xn_tail_jump_1(xn_kludge_menu_handlers[row]);
}

s32 spell_find_effect_type(const u8 *effects, u8 type)
{
    s32 slot;

    for (slot = 0; slot < 3; slot++)
        if (effects[EFFECT_STRIDE * slot] == type)
            return slot + 1;
    return 0;
}

s32 spell_has_no_effects(const u8 *effects)
{
    /* Quirk Q-SPELL-01: slot 1 (offset 2) twice, slot 2 (offset 4) never */
    return effects[0] == EFFECT_NONE && effects[2] == EFFECT_NONE && effects[2] == EFFECT_NONE;
}
