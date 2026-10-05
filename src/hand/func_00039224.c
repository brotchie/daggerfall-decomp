/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00039224 */
#include "records.h"

extern struct spell *selected_spell;
extern short spell_effect_text_base[];

int spell_effect_text_index(short slot)
{
    short s;
    short t;

    s = spell_effect_text_base[selected_spell->effects[slot].type];
    t = selected_spell->effects[slot].subtype;
    if (t != 255 && selected_spell->effects[slot].type != 29)
        s += t;
    return s;
}
