/* xspell.h: the game's spell thunks in XnGine's object (src/engine/spell.c; see xngine.h):
   dispatches through the game's tables of spell handlers, and two tests of a spell's three
   effect slots. The game names four of them; they live in object 2 because the tables do. */
#ifndef XSPELL_H
#define XSPELL_H

#include "xngine.h"
#include "xsysutil.h"

/* The game's cost formulas 1..7 (0xCAF24): spell_cost_duration_chance ... */
extern s32 (*spell_cost_formulas[7])(void);
/* The game's spell effect handlers (0xCAF40): 0-50 the effects, (spell, slot, target) */
extern s32 (*spell_effect_handlers[64])(void *spell, s32 slot, void *target);
/* spell_effect_handlers[51..63] (0xCB00C): the debug menu's (kludge.c) handlers; the asm
   enters them with EAX = 1 */
extern void (*xn_kludge_menu_handlers[13])(s32 one);

/* The cost of the current effect slot by formula 0..6 (spell_effect_cost_formula - 1):
   spell_cost_formulas[formula](). The spell maker. */
s32 spell_cost_formula_dispatch(s32 formula);
#pragma aux spell_cost_formula_dispatch parm [eax] value [eax] modify exact [eax edx ebx];

/* Runs effect `type` of a spell: spell_effect_handlers[type](spell, slot, target).
   spell_apply_effect. */
s32 spell_effect_dispatch(s32 type, void *spell, s32 slot, void *target);
#pragma aux spell_effect_dispatch parm [eax] [edx] [ebx] [ecx] value [eax] \
    modify exact [eax ebx ecx edx esi edi];

/* kludge_menu_update: runs the picked debug menu row's handler, with EAX = 1. */
void xn_spell_kludge_menu_dispatch(s32 row);
#pragma aux xn_spell_kludge_menu_dispatch parm [eax] modify exact [eax ebx ecx edx esi edi];

/* The slot + 1 (1..3) of the first effect of the given type in the spell's three effect
   slots (2 bytes apart), or 0. spell_remove_effect_type and spfx_entity_has_effect use the
   result as a 0-based slot (their bug, not this one's). */
s32 spell_find_effect_type(const u8 *effects, u8 type);

/* 1 when the spell has no effects: the types of slots 0 and 1 are 255. Kept from the asm: it
   tests slot 1 twice and never slot 2. */
s32 spell_has_no_effects(const u8 *effects);

#endif
