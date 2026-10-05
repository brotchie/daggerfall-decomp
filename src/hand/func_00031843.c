/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00031843 */
struct item { unsigned char type; char pad1[24]; unsigned short id; };
struct rec { char pad0[12]; struct item *ref; char pad10[4]; };    /* 20 bytes */
struct mobile { char pad0[22]; short count; };
struct thing {
    unsigned char type;
    char pad1[54];
    struct thing *next;         /* 55 */
    char pad3b[4];
    struct thing *child;        /* 63 */
    char pad43[4];
    struct mobile mob;          /* 71 */
};
struct actor { unsigned char type; char pad[70]; short id; };
struct level { char pad[63]; struct thing *first; };
extern struct level *D_00195A00;
extern char D_001962A9;
extern struct mobile *current_quest;
extern struct actor *quest_event_object2;
extern struct actor *quest_event_object;
extern short quest_event_code;
extern unsigned char *faction_find(short);
extern int quest_dispatch_event(struct mobile *);
extern struct rec *quest_section(struct mobile *, int);

int func_00031843(short a1, struct actor *a2, struct actor *a3)
{
    short id;
    int unused;
    struct rec *p;
    int i;
    int res;
    struct thing *t;

    res = 0;
    if (D_001962A9 != 0)
        return 0;
    if (a2->type == 8)
        id = a2->id;
    else if (a3->type == 8)
        id = a3->id;
    else
        return 0;
    if (*faction_find(id) != 4)
        return 0;
    t = D_00195A00->first;
    while (t != 0) {
        if (t->type == 14) {
            current_quest = &t->mob;
            p = quest_section(current_quest, 3);
            for (i = 0; i < current_quest->count; p++, i++) {
                if (p->ref->type == 65 && id == p->ref->id) {
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
