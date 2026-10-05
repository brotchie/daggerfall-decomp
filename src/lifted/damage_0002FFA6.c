/* damage.c: functions first lifted from the assembly by tools/lift_all.py (2026-10), now
 * ordinary source: edit them here. Keep each function where it is: Watcom aligns switch
 * tables from the start of the file, so moving functions can change the code. */
#include "records.h"

extern signed char quest_global_states[];


int quest_arg_state(struct qbn_op *op, short arg_index)
{
    struct qbn_state *state;

    if (op->args[arg_index].value == (-1)) return 1;
    state = (struct qbn_state *)op->args[arg_index].record;
    if (state == 0) return 0;
    if ((op->args[arg_index].negate & 1) != 0) {
        if (state->is_global != 0) {
            return ((quest_global_states[state->value] == 0) ? 1 : 0);
        }
        return ((state->value == 0) ? 1 : 0);
    }
    if (state->is_global != 0) {
        return (int)(unsigned char)quest_global_states[state->value];
    }
    return state->value;
}
