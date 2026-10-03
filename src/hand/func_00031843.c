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
extern struct mobile *D_00199764;
extern struct actor *D_00199774;
extern struct actor *D_00199778;
extern short D_001997AC;
extern unsigned char *func_000192EE(short);
extern int func_0002B26B(struct mobile *);
extern struct rec *func_000309E8(struct mobile *, int);

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
    if (*func_000192EE(id) != 4)
        return 0;
    t = D_00195A00->first;
    while (t != 0) {
        if (t->type == 14) {
            D_00199764 = &t->mob;
            p = func_000309E8(D_00199764, 3);
            for (i = 0; i < D_00199764->count; p++, i++) {
                if (p->ref->type == 65 && id == p->ref->id) {
                    D_001997AC = a1;
                    D_00199778 = a2;
                    D_00199774 = a3;
                    res |= func_0002B26B(D_00199764);
                    continue;
                }
            }
        }
        t = t->next;
    }
    return res;
}
