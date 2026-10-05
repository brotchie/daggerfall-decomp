/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0003D2B6 */
#include "records.h"

extern double D_00170D11;
extern double D_00170D19;
extern double D_00170D21;
extern short skill_advance_multipliers[];
extern struct character *player_character;
extern struct career *player_class;
#pragma aux func_000A166C parm routine [] value [8087];
extern double func_000A166C(double, double);

int skill_ready_to_advance(int skill_value, int uses, int multiplier, int skill)
{
    double needed;

    uses = ((65536 - ((player_character->reflexes - 2) << 13)) * uses) / 65536;
    needed = (double)skill_advance_multipliers[skill] * skill_value;
    needed = (player_class->advancement_multiplier * needed) * D_00170D11;
    needed = func_000A166C(1.04, (short)player_character->level) * needed;
    needed = ((needed * D_00170D19) / D_00170D21) + 1.0;
    return uses >= (int)needed;
}
