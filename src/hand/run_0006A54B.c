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
extern char *screen_buffer;
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
extern int mem_check_level;
extern int frame_checkpoint;
extern int D_0018DC24;
extern int nonworld_root;
extern struct ob *D_00195A00;
extern int D_00195AC4;
extern struct blk *object_heap_blocks;
extern int object_heap_size;
extern void func_00010AF6(int);
extern struct ent *quest_section(struct grp *, int);
extern void fatal_error(char *);
extern void object_foreach(int, void (*)(struct ob *));
extern int object_find_by_id(int, int);
extern int mc_memset();
extern int func_000A2A2B(void);
extern int func_000A2A76(struct msg *);
extern int func_000CE8D5();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
#pragma aux func_000A29BA parm routine [];
extern int func_000A29BA(char *);
extern int func_000A148C(char *, ...);

int mem_pool_release(char *p)
{
    struct blk *b;
    struct blk *n;
    int size;

    b = (struct blk *)(p - 18);
    if (b->magic != 1768515945 || (b->flags & ~1) != 0)
        fatal_error(D_00175B02);
    *(unsigned char *)&b->flags &= 254;
    size = b->size;
    mc_memset(p, 150, size, D_00175AD4, 182, 4);
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

void mem_check_quest_object_cb(struct ob *o)
{
    int saved;

    if (o->f33 != 0 && o->type != 2) {
        if (object_find_by_id(D_00195AC4, o->f33->f1f) == 0)
            fatal_error(D_00175B1A);
    }
    saved = o->f1f;
    o->f1f = 0;
    if (object_find_by_id(nonworld_root, saved) != 0)
        fatal_error(D_00175B39);
    o->f1f = saved;
}

void mem_check_quest_ids_cb(struct ob *o)
{
    struct ent *e;
    struct grp *g;
    int i;

    if (o->type != 14)
        return;
    g = (struct grp *)((char *)o + 71);
    e = quest_section(g, 4);
    for (i = 0; g->count > i; i++, e++) {
        if (e->owner != 0 && o->f26 != e->owner->f26)
            fatal_error(D_00175B51);
    }
}

void mem_check_heap(int a1)
{
    struct blk *b;
    struct blk *prev;

    frame_checkpoint = a1;
    if (mem_check_level == 0)
        return;
    func_000A0ED9(280, D_00175AD4);
    if (func_000A29BA(screen_buffer) != 0)
        fatal_error(D_00175B63);
    func_00010AF6(a1);
    if (D_0018DC24 != 0)
        func_000CE8D5(a1);
    object_foreach(nonworld_root, mem_check_quest_object_cb);
    object_foreach(D_00195A00->f3f, mem_check_quest_ids_cb);
    prev = b = object_heap_blocks;
    while (b != 0) {
        if (b->magic != 1768515945) {
            func_000A0ED9(300, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            fatal_error(D_00175B91);
        }
        if (b->next != 0 && (char *)b + 18 + b->size != (char *)b->next) {
            func_000A0ED9(306, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)b)[18]);
            fatal_error(D_00175BA8);
        }
        if (b->size == 0 || b->size > object_heap_size) {
            func_000A0ED9(312, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            fatal_error(D_00175BC1);
        }
        if (b < object_heap_blocks || (int)object_heap_blocks + object_heap_size < (int)b) {
            func_000A0ED9(318, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            fatal_error(D_00175BDC);
        }
        prev = b;
        b = b->next;
        if (b != 0 && b->prev != prev) {
            func_000A0ED9(327, D_00175AD4);
            func_000A148C(D_00175B79, ((unsigned char *)prev)[18]);
            fatal_error(D_00175BFA);
        }
    }
}

void mem_check_crt_heap(int a1)
{
    int r;
    struct msg m;

    r = 0;
    if (mem_check_level == 0)
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
        fatal_error(D_00175C18);
        break;
    case 5:
        fatal_error(D_00175C18);
        break;
    case 3:
        fatal_error(D_00175C18);
        break;
    }
}
