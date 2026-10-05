/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00031843 */
#include "records.h"

extern struct record *D_00195A00;
extern char D_001962A9;
extern struct quest *current_quest;
extern struct record *quest_event_object2;
extern struct record *quest_event_object;
extern short quest_event_code;
extern struct faction *faction_find(short);
extern int quest_dispatch_event(struct quest *);
extern void *quest_section(struct quest *, int);

int func_00031843(short a1, struct record *a2, struct record *a3)
{
    short id;
    int unused;
    struct qbn_person *p;
    int i;
    int res;
    struct record *t;

    res = 0;
    if (D_001962A9 != 0)
        return 0;
    /* a person's (type 8) data starts with its faction id; a quest NPC (type 65) has it at +0x19 */
    if (a2->type == 8)
        id = (short)a2->data.person.faction_id;
    else if (a3->type == 8)
        id = (short)a3->data.person.faction_id;
    else
        return 0;
    if (faction_find(id)->type != 4)
        return 0;
    t = D_00195A00->children;
    while (t != 0) {
        if (t->type == 14) {
            current_quest = &t->data.quest;
            p = quest_section(current_quest, 3);
            for (i = 0; i < current_quest->section_counts[3]; p++, i++) {
                if (p->object->type == 65 && id == (unsigned short)p->object->faction_id) {
                    quest_event_code = a1;
                    quest_event_object = a2;
                    quest_event_object2 = a3;
                    res |= quest_dispatch_event(current_quest);
                    continue;
                }
            }
        }
        t = t->next;
    }
    return res;
}
