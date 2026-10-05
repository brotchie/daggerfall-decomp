/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char quest_global_states[];


void quest_set_state(struct quest *quest, struct qbn_op *op, short value)
{
    struct qbn_state *state;

    if (op->args[0].value == (-1)) return;
    state = (struct qbn_state *)op->args[0].record;
    if (((int)(unsigned char)(op->args[0].negate & 1)) != 0) *(int *)&value ^= 1;
    if (state->is_global != 0) {
        quest_global_states[state->value] = *(signed char *)&value;
        return;
    }
    state->value = *(signed char *)&value;
}
