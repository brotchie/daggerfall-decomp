/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004BBD8 */
#include "records.h"
extern struct record *quest_event_object2;
extern struct record *quest_event_object;
extern short quest_event_code;
extern void quest_dispatch_event(struct quest *);
extern struct quest *quest_find_by_id(unsigned char);

void quest_raise_event(short a1, struct record *a2, struct record *a3)
{
    struct quest *l;

    l = quest_find_by_id(a2 ? a2->quest_id : a3->quest_id);
    if (l == 0)
        return;
    quest_event_code = a1;
    quest_event_object = a2;
    quest_event_object2 = a3;
    quest_dispatch_event(l);
}
