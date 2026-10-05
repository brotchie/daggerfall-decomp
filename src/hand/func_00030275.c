/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00030275 */
#include "records.h"

extern unsigned char quest_global_states[];
extern int rand(void);

void qaction_op34_pick_one_state(int a1, struct qbn_op *a2)
{
    struct qbn_state *arr[4];
    short i;
    short n;
    short pick;

    n = i = 0;
    for (; i < 4; i++) {
        if (a2->args[i + 1].value != -1 && a2->args[i + 1].value != -2)
            arr[n++] = (struct qbn_state *)a2->args[i + 1].record;
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
