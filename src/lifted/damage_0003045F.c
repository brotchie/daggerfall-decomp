/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern char quest_global_states[];


void quest_set_state(struct quest *a1, struct qbn_op *a2, short a3)
{
    struct qbn_state *l_14;

    if (a2->args[0].value == (-1)) return;
    l_14 = (struct qbn_state *)a2->args[0].record;
    if (((int)(unsigned char)(a2->args[0].negate & 1)) == 0) goto L3049B;
    *(int *)&a3 ^= 1;
L3049B:;
    if (l_14->is_global == 0) goto L304B7;
    *(signed char *)(quest_global_states + l_14->value) = *(signed char *)&a3;
    return;
L304B7:;
    l_14->value = *(signed char *)&a3;
}
