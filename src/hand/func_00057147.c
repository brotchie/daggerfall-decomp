/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00057147 */
#include "structs.h"
extern struct magic_enchantment D_00199868[][5];   /* itemmaker_slot_exclusions */

void func_00057147(short slot, short type, short param, short type2, short param2, short type3, short param3)
{
    D_00199868[slot][0].type = type;
    D_00199868[slot][0].param = param;
    D_00199868[slot][1].type = type2;
    D_00199868[slot][1].param = param2;
    D_00199868[slot][2].type = type3;
    D_00199868[slot][2].param = param3;
}
