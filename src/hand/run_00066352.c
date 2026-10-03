/* matched by the real Watcom C32 10.0a (-d2): a run of disease.c from 0x0006630B to 0x00066352, kept together for its switch table's alignment */
struct pick {
    char pad0[12];
    struct obj *obj;            /* 0x0c */
};
struct obj {
    unsigned char type;         /* 0x00 */
    char pad01[20];
    unsigned short flags;       /* 0x15 */
    char pad17[4];
    unsigned short f27;         /* 0x1b */
    char pad1d[34];
    struct obj *list;           /* 0x3f */
    char pad43[4];
    unsigned char data[1];      /* 0x47 */
};
struct item {
    unsigned char cond;         /* 0x00 */
    char pad01[30];
    short stats[8];             /* 0x1f */
};
struct dis { char pad0[35]; unsigned short kind; };
struct pc {
    char pad00[32];
    short stats[8];             /* 0x20 */
    short maxstats[8];          /* 0x30 */
    unsigned char f64;          /* 0x40 */
    char pad41;
    unsigned char f66;          /* 0x42 */
    unsigned char f67;          /* 0x43 */
    char pad44[40];
    short f108;                 /* 0x6c */
    char pad6e[65];
    short f175;                 /* 0xaf */
    char padb1[76];
    short f253;                 /* 0xfd */
    char padff[10];
    short f265;                 /* 0x109 */
    char pad10b[16];
    short f283;                 /* 0x11b */
    char pad11d[52];
    short f337;                 /* 0x151 */
    char pad153[22];
    short f361;                 /* 0x169 */
    char pad16b[135];
    unsigned char f498;         /* 0x1f2 */
    int f499;                   /* 0x1f3 */
    char pad1f7[38];
    unsigned char f541;         /* 0x21d */
};
extern char D_00175970[];        /* __FILE__ */
extern unsigned char D_00186DE3[];
extern unsigned char D_001940D8;
extern struct obj *D_00195AA0;
extern struct obj *D_00195AC4;
extern struct pc *D_00195BE0;
extern unsigned char *D_00195BEC;
extern unsigned char D_00196268;
extern unsigned char D_00196294;
extern int D_00196A30;
extern struct dis *func_000191DA(short, int);
extern void func_0001E34D(struct pick *, int, int);
extern void func_0003F09F(int, int);
extern void func_0004AB2F(int);
extern void func_00058E15(int, int);
extern void func_0005E540(int, int, unsigned char *);
extern void func_0006630B(struct obj *);
extern void func_00066853(struct obj *, unsigned char);
extern int func_00067270(void);
extern void func_00086397(struct pick *);
extern void func_000876AD(unsigned char, int, unsigned short, int);
extern void func_0008AF3E(struct obj *, struct pc *);
extern struct obj *func_0008DCE3(struct obj *, int, int);
extern void func_0008E3F7(struct obj *, void (*)(struct obj *));
extern struct obj *func_0008E6C5(struct obj *, short, short);
extern void func_00097101(struct obj *);
extern int func_0009A0A0(struct obj *, int, int);
extern void func_0009A6D0(struct obj *, int);
extern int func_0009DC25(void);
extern void func_000A1023(void *, void *, int, char *, int, int);

void func_0006630B(struct obj *a1)
{
    if (a1->type == 10) {
        a1->type = 29;
        return;
    }
    if (a1->type == 29)
        a1->type = 10;
}

void func_00066352(void)
{
    struct pick s;
    int u48;
    struct obj *o2;
    int u40;
    struct item *p;
    int u38;
    int i;
    struct obj *o1;
    int u2c;
    int kind;
    int saved;
    int u20;
    struct dis *d;
    int excess;

    if (D_00195BE0->f67 > 7 || func_00067270() != 0)
        return;
    D_00195BE0->f499 = 0;
    D_00195BE0->f108 = 0;
    func_0003F09F(401, 1);
    saved = D_00196294;
    D_00196294 = 1;
    func_0004AB2F(30240);
    D_00196294 = saved;
    if (D_00196A30 != 0) {
        func_0001E34D(&s, 0, func_0009DC25() % D_00196A30);
        func_000876AD(D_00196268, 3, s.obj->f27, 0);
        if (func_0009A0A0(D_00195AC4, 9, 0) != 0)
            func_0009A6D0(D_00195AC4, 9);
        func_00086397(&s);
    }
    d = func_000191DA(D_00196268, 7);
    if (d != 0)
        kind = d->kind;
    else
        kind = 153;
    D_001940D8 |= 8;
    D_00195BE0->f64 |= 20;
    o1 = func_0008DCE3(D_00195AA0, 0, 47);
    o1->type = 11;
    o1->flags = 0x8003;
    o2 = func_0008DCE3(D_00195AA0, 0, 74);
    o2->type = 28;
    o2->flags = 3;
    p = (struct item *)o1->data;
    p->cond = 100;
    func_000A1023(o2->data, D_00195BEC, 74, D_00175970, 407, 4);
    for (i = 0; i < 8; i++) {
        if (i == 1)
            continue;
        p->stats[i] = 20;
        D_00195BE0->stats[i] += 20;
        D_00195BE0->maxstats[i] += 20;
        excess = D_00195BE0->maxstats[i] - 100;
        if (excess > 0) {
            D_00195BE0->maxstats[i] -= excess;
            D_00195BE0->stats[i] -= excess;
            p->stats[i] -= excess;
        }
    }
    D_00195BE0->f175 += 30;
    D_00195BE0->f283 += 30;
    D_00195BE0->f253 += 30;
    D_00195BE0->f361 += 30;
    D_00195BE0->f265 += 30;
    D_00195BE0->f337 += 30;
    D_00195BEC[1] |= 0x41;
    D_00195BEC[4] |= 0x30;
    func_0008AF3E(D_00195AA0, D_00195BE0);
    func_0008E3F7(D_00195AA0->list, func_0006630B);
    D_00195BE0->f498 = D_00195BE0->f67;
    D_00195BE0->f67 = 8;
    D_00195BE0->f66 = 2;
    o2 = func_0008E6C5(D_00195AA0->list, 27, 0);
    if (o2 == 0) {
        o2 = func_0008DCE3(D_00195AC4, 0, 107);
        o2->type = 2;
        o2->flags |= 1;
        func_0005E540(27, 0, o2->data);
        func_00097101(o2);
    }
    i = 0;
    while (D_00186DE3[i] != 255)
        func_00066853(o2, D_00186DE3[i++]);
    switch (kind - 150) {
    case 0:
        func_00066853(o2, 85);
        break;
    case 5:
        func_00066853(o2, 50);
        break;
    case 4:
        func_00066853(o2, 10);
        break;
    case 2:
        func_00066853(o2, 64);
        break;
    case 7:
        p->stats[1] += 20;
        D_00195BE0->stats[1] += 20;
        D_00195BE0->maxstats[1] += 20;
        excess = D_00195BE0->maxstats[i] - 100;
        if (excess > 0) {
            D_00195BE0->maxstats[1] -= excess;
            D_00195BE0->stats[1] -= excess;
            p->stats[1] -= excess;
        }
        break;
    case 6:
        func_00066853(o2, 17);
        break;
    case 8:
        func_00066853(o2, 11);
        func_00066853(o2, 12);
        func_00066853(o2, 13);
        break;
    case 3:
        func_00066853(o2, 23);
        func_00066853(o2, 6);
        break;
    case 1:
        func_00066853(o2, 20);
        func_00066853(o2, 33);
        break;
    }
    D_00195BE0->f541 = kind;
    func_00058E15(0, 0);
}
