/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002CB34 */
struct S { char pad[2]; short flags; };
struct T { char pad[28]; short id; };
extern void quest_timer_update(int, struct S *, int);
extern struct S *quest_record(int, int, int);

void qaction_op12_start_stop_timer(int a1, struct T *a2, short a3)
{
    struct S *p;

    p = quest_record(a1, 6, a2->id);
    if (a3 & 64) {
        if ((p->flags & 64) == 0) {
            quest_timer_update(a1, p, 1);
            p->flags |= 64;
        }
    } else {
        p->flags &= ~64;
    }
}
