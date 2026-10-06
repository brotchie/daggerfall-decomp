/* xspell.h: the game's spell thunks in XnGine's object (src/engine/spell.c). Canonical C:
   plain prototypes, Watcom's own calling convention; docs/xngine_canonical.md.

   What it does
     Dispatches through the game's tables of spell handlers (the spell maker's cost formulas,
     the spell effects, the debug menu's rows), and two tests of a spell's three effect
     slots. The game names four of them (the thunks of its spell code); they live in object 2
     because the tables do.

   The dispatchers jump into the handler, which then runs at its caller's stack depth and
   returns straight to it, as the asm's `jmp [table + i*4]` did (xsysutil.h's tail calls: the
   game's handlers store addresses of their locals where the game reads them).

   Effects
     A spell's effects are three slots 2 bytes apart; a slot's first byte is the effect type,
     FFh when the slot is empty.

   Quirks (docs/engine/quirks.md): Q-SPELL-01 (has_no_effects tests slot 1 twice, slot 2
   never). */
#ifndef XSPELL_H
#define XSPELL_H

#include "xngine.h"
#include "ptrint.h"                     /* iptr: an int that holds an address */

/* The game's cost formulas 1..7 (0xCAF24): spell_cost_duration_chance ... */
extern s32 (*spell_cost_formulas[7])(void);
/* The game's spell effect handlers (0xCAF40): 0-50 the effects, (spell, slot, target) */
extern s32 (*spell_effect_handlers[64])(void *spell, s32 slot, void *target);
/* spell_effect_handlers[51..63] (0xCB00C): the debug menu's (kludge.c) handlers, given 1 */
extern void (*xn_kludge_menu_handlers[13])(s32 one);

/* The cost of the current effect slot by formula 0..6 (spell_effect_cost_formula - 1):
   spell_cost_formulas[formula](). The spell maker (3 game sites). */
s32 spell_cost_formula_dispatch(s32 formula);

/* Runs effect `type` of a spell: spell_effect_handlers[type](spell, slot, target).
   spell_apply_effect. */
s32 spell_effect_dispatch(s32 type, void *spell, s32 slot, void *target);

/* kludge_menu_update: runs the picked debug menu row's handler with 1. */
void xn_spell_kludge_menu_dispatch(s32 row);

/* The slot + 1 (1..3) of the first effect of type `type` in the spell's three effect slots,
   or 0. spell_remove_effect_type and spfx_entity_has_effect use the result as a 0-based slot
   (their bug, not this one's). */
s32 spell_find_effect_type(const u8 *effects, u8 type);

/* 1 when the spell has no effects: the types of its slots are FFh. Q-SPELL-01: it tests slot
   1 twice and never slot 2. */
s32 spell_has_no_effects(const u8 *effects);

#endif
