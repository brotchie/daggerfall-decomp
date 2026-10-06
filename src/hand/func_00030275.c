/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00030275 */
#include "records.h"
#include "clib.h"

extern unsigned char quest_global_states[];

void qaction_op34_pick_one_state(struct quest *quest, struct qbn_op *op)
{
    struct qbn_state *arr[4];
    short i;
    short n;
    short pick;

    n = i = 0;
    for (; i < 4; i++) {
        if (op->args[i + 1].value != -1 && op->args[i + 1].value != -2)
            arr[n++] = (struct qbn_state *)op->args[i + 1].record;
    }
    if (n == 0)
        return;
    pick = rand() % n;
    for (i = 0; i < n; i++) {
        if (i == pick) {
            if (arr[i]->is_global != 0)
                quest_global_states[arr[i]->value] = i == pick;
            else
                arr[i]->value = i == pick;
        }
    }
}
