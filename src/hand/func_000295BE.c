/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000295BE */
#pragma pack(1)
struct Sub { char pad[4]; unsigned char count; };
struct Obj {
    char pad0[0x16];
    struct Sub *sub;            /* 0x16 */
    char pad1[0x2b - 0x1a];
    unsigned interval;          /* 0x2b */
    char pad2[0x38 - 0x2f];
    short chance;               /* 0x38 */
    char pad3[0x49 - 0x3a];
    int charges;                /* 0x49 */
    char pad4[0x53 - 0x4d];
    int last;                   /* 0x53 */
};
extern int game_minutes;
extern void qaction_place_foe(struct Obj *, int);
extern int rand(void);

void quest_op09_spawn_repeat(int a1, struct Obj *o)
{
    struct Sub *s;
    int i;

    if (o->charges == 0)
        return;
    if (game_minutes - o->last < o->interval)
        return;
    o->last = game_minutes;
    if (rand() % 100 > o->chance)
        return;
    if (o->charges != -1)
        o->charges--;
    s = o->sub;
    for (i = 0; i < s->count; i++)
        qaction_place_foe(o, 0);
}
