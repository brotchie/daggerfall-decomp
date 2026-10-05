/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002CB34 */
#include "records.h"

extern void quest_timer_update(struct quest *, struct qbn_timer *, int);
extern void *quest_record(struct quest *, int, int);

void qaction_op12_start_stop_timer(struct quest *a1, struct qbn_op *a2, short a3)
{
    struct qbn_timer *p;

    p = quest_record(a1, 6, (short)a2->args[1].value);
    if (a3 & 64) {
        if (((short)p->flags & 64) == 0) {
            quest_timer_update(a1, p, 1);
            p->flags |= 64;
        }
    } else {
        p->flags &= ~64;
    }
}
