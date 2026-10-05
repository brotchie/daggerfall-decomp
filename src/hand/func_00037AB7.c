/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00037AB7 */
#include "records.h"

extern struct spell *selected_spell;
extern unsigned char D_0017A8B2[];

int func_00037AB7(void)
{
    short l_18;

    l_18 = D_0017A8B2[selected_spell->effects[0].type] > D_0017A8B2[selected_spell->effects[1].type] ? D_0017A8B2[selected_spell->effects[0].type] : D_0017A8B2[selected_spell->effects[1].type];
    return D_0017A8B2[selected_spell->effects[2].type] > l_18 ? D_0017A8B2[selected_spell->effects[2].type] : l_18;
}
