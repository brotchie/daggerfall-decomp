/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002CB34 */
#include "records.h"

extern void quest_timer_update(struct quest *, struct qbn_timer *, int);
extern void *quest_record(struct quest *, int, int);

void qaction_op12_start_stop_timer(struct quest *quest, struct qbn_op *op, short run_flag)
{
    struct qbn_timer *timer;

    timer = quest_record(quest, 6, (short)op->args[1].value);
    if (run_flag & 64) {
        if (((short)timer->flags & 64) == 0) {
            quest_timer_update(quest, timer, 1);
            timer->flags |= 64;
        }
    } else {
        timer->flags &= ~64;
    }
}
