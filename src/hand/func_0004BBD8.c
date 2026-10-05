/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004BBD8 */
struct obj { char pad[38]; unsigned char f38; };
extern struct obj *quest_event_object2;
extern struct obj *quest_event_object;
extern short quest_event_code;
extern void quest_dispatch_event(int);
extern int quest_find_by_id(unsigned char);

void quest_raise_event(short a1, struct obj *a2, struct obj *a3)
{
    int l;

    l = quest_find_by_id(a2 ? a2->f38 : a3->f38);
    if (l == 0)
        return;
    quest_event_code = a1;
    quest_event_object = a2;
    quest_event_object2 = a3;
    quest_dispatch_event(l);
}
