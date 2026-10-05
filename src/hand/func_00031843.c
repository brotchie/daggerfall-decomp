/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00031843 */
#include "records.h"

extern struct record *quest_root;
extern char quests_suspended;
extern struct quest *current_quest;
extern struct record *quest_event_object2;
extern struct record *quest_event_object;
extern short quest_event_code;
extern struct faction *faction_find(short);
extern int quest_dispatch_event(struct quest *);
extern void *quest_section(struct quest *, int);

int func_00031843(short event_code, struct record *event_object, struct record *event_object2)
{
    short faction_id;
    int unused;
    struct qbn_person *qbn_person;
    int i;
    int result;
    struct record *object;

    result = 0;
    if (quests_suspended != 0)
        return 0;
    /* a person's (type 8) data starts with its faction id; a quest NPC (type 65) has it at +0x19 */
    if (event_object->type == 8)
        faction_id = (short)event_object->data.person.faction_id;
    else if (event_object2->type == 8)
        faction_id = (short)event_object2->data.person.faction_id;
    else
        return 0;
    if (faction_find(faction_id)->type != 4)
        return 0;
    object = quest_root->children;
    while (object != 0) {
        if (object->type == 14) {
            current_quest = &object->data.quest;
            qbn_person = quest_section(current_quest, 3);
            for (i = 0; i < current_quest->section_counts[3]; qbn_person++, i++) {
                if (qbn_person->object->type == 65 && faction_id == (unsigned short)qbn_person->object->faction_id) {
                    quest_event_code = event_code;
                    quest_event_object = event_object;
                    quest_event_object2 = event_object2;
                    result |= quest_dispatch_event(current_quest);
                    continue;
                }
            }
        }
        object = object->next;
    }
    return result;
}
