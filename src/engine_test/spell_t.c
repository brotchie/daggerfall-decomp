/* spell_t.c: test shims of src/engine/spell.c (built only by tools/xn_rc.py; docs/
   xngine_canonical.md). The three dispatchers have none: under a shim their handlers would
   run a frame deeper than the asm's jump runs them, which the game can see (xsysutil.h). Their
   plain prototypes are their asm entries' interfaces, so their records reach the C straight,
   as the game's calls do. */
#include "xspell.h"

/* effects EAX, type DL -> EAX */
void spell_find_effect_type_r(xn_regs *r)
{
    r->eax = spell_find_effect_type((const u8 *)r->eax, (u8)r->edx);
}

void spell_has_no_effects_r(xn_regs *r)
{
    r->eax = spell_has_no_effects((const u8 *)r->eax);
}
