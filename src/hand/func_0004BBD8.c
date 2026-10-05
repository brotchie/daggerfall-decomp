/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004BBD8 */
#include "records.h"
extern struct record *quest_event_object2;
extern struct record *quest_event_object;
extern short quest_event_code;
extern int quest_dispatch_event(struct quest *);
extern struct quest *quest_find_by_id(int);

void quest_raise_event(short event, struct record *object, struct record *object2)
{
    struct quest *quest;

    quest = quest_find_by_id(object ? object->quest_id : object2->quest_id);
    if (quest == 0)
        return;
    quest_event_code = event;
    quest_event_object = object;
    quest_event_object2 = object2;
    quest_dispatch_event(quest);
}
