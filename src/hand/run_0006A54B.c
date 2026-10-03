/* matched by the real Watcom C32 10.0a (-d2): a run of jmem.c from 0x0006A0D4 to 0x0006A54B, kept together for its switch table's alignment */
struct blk {                    /* a heap block's header, the data follows */
    int magic;
    struct blk *next;
    struct blk *prev;
    int size;
    short flags;
};
struct ob {
    unsigned char type;
    char pad1[30];
    int f1f;                    /* 31 */
    char pad23[3];
    char f26;                   /* 38 */
    char pad27[12];
    struct ob *f33;             /* 51 */
    char pad37[8];
    int f3f;                    /* 63 */
};
struct grp { char pad[24]; short count; };
struct ent { char pad[16]; struct ob *owner; char pad14[4]; };
struct msg { int a; short b; char pad[10]; };
extern char *D_00143550;
extern char D_00175AD4[];       /* __FILE__ */
extern char D_00175B02[];
extern char D_00175B1A[];
extern char D_00175B39[];
extern char D_00175B51[];
extern char D_00175B63[];
extern char D_00175B79[];
extern char D_00175B91[];
extern char D_00175BA8[];
extern char D_00175BC1[];
extern char D_00175BDC[];
extern char D_00175BFA[];
extern char D_00175C18[];
extern int D_00187CAD;
extern int D_0018DBFC;
extern int D_0018DC24;
extern int D_001959A8;
extern struct ob *D_00195A00;
extern int D_00195AC4;
extern struct blk *D_00195E1C;
extern int D_001A9B34;
extern void func_00010AF6(int);
extern struct ent *func_000309E8(struct grp *, int);
extern void func_00050069(char *);
extern void func_0008E3F7(int, void (*)(struct ob *));
extern int func_0008E925(int, int);
extern int func_000A0040();
extern int func_000A2A2B(void);
extern int func_000A2A76(struct msg *);
extern int func_000CE8D5();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
#pragma aux func_000A29BA parm routine [];
extern int func_000A29BA(char *);
extern int func_000A148C(char *, ...);

int func_0006A0D4(char *p)
{
    struct blk *b;
    struct blk *n;
    int size;

    b = (struct blk *)(p - 18);
    if (b->magic != 1768515945 || (b->flags & ~1) != 0)
        func_00050069(D_00175B02);
    *(unsigned char *)&b->flags &= 254;
    size = b->size;
    func_000A0040(p, 150, size, D_00175AD4, 182, 4);
    if (b->next != 0) {
        if (!(b->next->flags & 1)) {
            n = b->next;
            b->size += n->size + 18;
            b->next = n->next;
            if (b->next != 0)
                b->next->prev = b;
        }
    }
    if (b->prev != 0) {
        if (!(b->prev->flags & 1)) {
            n = b->prev;
            n->size += b->size + 18;
            n->next = b->next;
            if (n->next != 0)
                n->next->prev = n;
        }
    }
    return size;
}

void func_0006A1FD(struct ob *o)
{
    int saved;

    if (o->f33 != 0 && o->type != 2) {
        if (func_0008E925(D_00195AC4, o->f33->f1f) == 0)
            func_00050069(D_00175B1A);
    }
    saved = o->f1f;
    o->f1f = 0;
    if (func_0008E925(D_001959A8, saved) != 0)
        func_00050069(D_00175B39);
    o->f1f = saved;
}

void func_0006A28A(struct ob *o)
{
    struct ent *e;
    struct grp *g;
    int i;

    if (o->type != 14)
        return;
    g = (struct grp *)((char *)o + 71);
    e = func_000309E8(g, 4);
    for (i = 0; g->count > i; i++, e++) {
        if (e->owner != 0 && o->f26 != e->owner->f26)
            func_00050069(D_00175B51);
    }
}

void func_0006A319(int a1)
{
    struct blk *b;
    struct blk *prev;

    D_0018DBFC = a1;
    if (D_00187CAD == 0)
        return;
    func_000A0ED9(280, D_00175AD4);
    if (func_000A29BA(D_00143550) != 0)
        func_00050069(D_00175B63);
    func_00010AF6(a1);
    if (D_0018DC24 != 0)
        func_000CE8D5(a1);
    func_0008E3F7(D_001959A8, func_0006A1FD);
    func_0008E3F7(D_00195A00->f3f, func_0006A28A);
    prev = b = D_00195E1C;
    while (b != 0) {
        if (b->magic != 1768515945) {
            func_000A0ED9(300, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            func_00050069(D_00175B91);
        }
        if (b->next != 0 && (char *)b + 18 + b->size != (char *)b->next) {
            func_000A0ED9(306, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)b)[18]);
            func_00050069(D_00175BA8);
        }
        if (b->size == 0 || b->size > D_001A9B34) {
            func_000A0ED9(312, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            func_00050069(D_00175BC1);
        }
        if (b < D_00195E1C || (int)D_00195E1C + D_001A9B34 < (int)b) {
            func_000A0ED9(318, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            func_00050069(D_00175BDC);
        }
        prev = b;
        b = b->next;
        if (b != 0 && b->prev != prev) {
            func_000A0ED9(327, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            func_00050069(D_00175BFA);
        }
    }
}

void func_0006A54B(int a1)
{
    int r;
    struct msg m;

    r = 0;
    if (D_00187CAD == 0)
        return;
    func_000A0ED9(349, D_00175AD4);
    func_000A2A2B();
    m.b = 0;
    m.a = 0;
    while (r == 0)
        r = func_000A2A76(&m);
    switch (r) {
    case 4:
        break;
    case 1:
        break;
    case 2:
        func_00050069(D_00175C18);
        break;
    case 5:
        func_00050069(D_00175C18);
        break;
    case 3:
        func_00050069(D_00175C18);
        break;
    }
}
