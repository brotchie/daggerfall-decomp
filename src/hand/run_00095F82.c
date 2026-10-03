/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00095D2C to 0x00095F82, kept together for its switch table's alignment */
struct node {
    unsigned char type;
    char pad1[54];
    struct node *next;      /* 0x37 */
    char pad3b[4];
    struct node *child;     /* 0x3f */
};
struct item {
    char pad0[0x20];
    unsigned short type;    /* 0x20 */
    unsigned short sub;     /* 0x22 */
    int value;              /* 0x24 */
    char pad28[4];
    short count;            /* 0x2c */
    char pad2e[0x37 - 0x2e];
    unsigned char f37;
};
struct pc {
    char pad0[0x40];
    unsigned short flags;   /* 0x40 */
    char pad42[0x16f - 0x42];
    char *slots[19];        /* 0x16f */
    char *f1bb;
    char pad1bf[4];
    char *f1c3;
};
struct flags8 { unsigned char b0:2; unsigned char b2:1; };
extern char D_0012B508;
extern char D_0017704C[];       /* __FILE__ */
extern char *D_001832A4;
extern unsigned char D_001860DA[];
extern unsigned char D_00186104[];
extern short D_00188208[];
extern char D_001903A4[];
extern unsigned char D_001940D8;
extern struct node *D_001959E8;
extern int D_00195AA4;
extern struct node *D_00195B20;
extern struct node *D_00195B34;
extern struct pc *D_00195BE0;
extern unsigned char D_0019626F;
extern unsigned char D_00196274;
extern unsigned char D_00196288;
extern int D_001AA454;
extern char D_001AA548[];
extern struct node *D_001AA558;
extern char *D_001AA55C;
extern char D_001AA568[];
extern struct node *D_001AA578;
extern struct node *D_001AA57C;
extern short D_001AA584;
extern short D_001AA586;
extern short D_001AA588;
extern short D_001AA58A;
extern void func_0003EC2A(char *, int);
extern void func_0003F09F(int, int);
extern int func_00069938(int, int, int);
extern void func_0007F185(int);
extern int func_0008DA4D(char *);
extern void func_00093372(int, unsigned char);
extern int func_000934F6(struct node *, short, char *);
extern void func_000958D0(struct node *, char *);
extern void func_00095B81(struct node *, char *);
extern void func_00096782(char *, int, int);
extern void func_000967F9(int);
extern void func_00096847(char *, int);
extern int func_000968F8(char *);
extern void func_00096B77(char *, int);
extern void func_00097101(char *);
extern int func_00098CE4(struct item *);
extern void func_000A0040(char *, int, int, char *, int, int);
extern int func_000CE44C();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);
void func_00095F82(char *obj);

void func_00095D2C(char *a1)
{
    struct node *n;

    D_001AA58A = D_001AA586 = 0;
    func_000A0040(D_001AA568, 0, 20, D_0017704C, 1627, 20);
    if (D_001AA57C->type == 2 && D_001AA57C != D_001959E8) {
        func_000934F6(D_001AA57C, 0, a1);
        D_001AA578 = D_001AA57C;
    } else if (D_001AA57C == D_001959E8) {
        func_00093372(27, D_00196288);
    } else if (D_00196274 != 4 && D_0019626F != 4 && !((struct flags8 *)&D_001940D8)->b2) {
        return;
    }
    n = D_001AA57C->child;
    while (n != 0) {
        func_000958D0(n, a1 + 12);
        n = n->next;
    }
    D_001AA58A = D_001AA586;
}

void func_00095E32(char *a1)
{
    struct node *n;

    D_001AA584 = D_001AA588 = 0;
    func_000A0040(D_001AA548, 0, 20, D_0017704C, 1655, 20);
    if (D_00195B20 != D_00195B34) {
        func_000934F6(D_00195B20, 0, a1);
        D_001AA558 = D_00195B20;
    }
    n = D_00195B20->child;
    while (n != 0) {
        func_00095B81(n, a1 + 12);
        n = n->next;
    }
    D_001AA584 = D_001AA588;
}

void func_00095EDB(void)
{
    int l_1C;
    int l_18;

    D_001AA454 = 0;
    if (func_000CE44C(D_00195BE0->slots, D_001AA55C, 27) != 0)
        return;
    func_00097101(D_001AA55C);
    func_00095F82(D_001AA55C);
    if (D_001AA454 == 0)
        return;
    func_0007F185(D_001AA454);
    D_0012B508 = 144;
    func_000A0ED9(1688, D_0017704C);
    func_000A0F5C(D_001903A4, D_001832A4, D_001AA454);
    func_0003EC2A(D_001903A4, 1);
}

void func_00095F82(char *obj)
{
    struct item *it;
    unsigned char *tbl;
    int unused;

    it = (struct item *)(obj + 71);
    if (it->type == 3 && it->sub == 18)
        return;
    if (it->type == 15 || it->type == 16 || it->type == 17 || it->type == 18 ||
        it->type == 19 || it->type == 20 || it->type == 21 || it->type == 22)
        return;
    if (it->count == 0) {
        func_0003F09F(29, 1);
        return;
    }
    D_001940D8 |= 8;
    switch (it->type) {
    case 28:
        switch (it->sub) {
        case 0:
            if (D_00196274 == 4)
                func_00069938(204, D_00195AA4, 100);
            D_001AA454 += it->value;
            func_0008DA4D(obj);
            break;
        }
        break;
    case 2:
        if (func_00098CE4(it) != 0)
            return;
        switch (it->sub) {
        case 0:
            if (D_00196274 == 4)
                func_00069938(it->f37 + 231, D_00195AA4, 100);
            func_00096847(obj, 18);
            break;
        case 1:
            if (D_00196274 == 4)
                func_00069938(233, D_00195AA4, 100);
            func_00096847(obj, 20);
            break;
        case 2:
            if (D_00196274 == 4)
                func_00069938(it->f37 + 231, D_00195AA4, 100);
            func_00096847(obj, 23);
            break;
        case 3:
            if (D_00196274 == 4)
                func_00069938(it->f37 + 231, D_00195AA4, 100);
            func_00096847(obj, 15);
            break;
        case 4:
            if (D_00196274 == 4)
                func_00069938(it->f37 + 231, D_00195AA4, 100);
            func_00096847(obj, 13);
            break;
        case 5:
            if (D_00196274 == 4)
                func_00069938(233, D_00195AA4, 100);
            func_00096847(obj, 12);
            break;
        case 6:
            if (D_00196274 == 4)
                func_00069938(it->f37 + 231, D_00195AA4, 100);
            func_00096847(obj, 26);
            break;
        case 7:
        case 8:
        case 9:
        case 10:
            if (D_00196274 == 4)
                func_00069938(233, D_00195AA4, 100);
            if (func_000968F8(D_00195BE0->f1bb) != 0) {
                func_00096B77(D_00195BE0->f1bb, 19);
                D_00195BE0->f1bb = 0;
                func_00096847(obj, 21);
                return;
            }
            func_00096847(obj, 21);
            break;
        }
        break;
    case 3:
        if (func_00098CE4(it) != 0)
            return;
        if (D_00196274 == 4)
            func_00069938(D_00188208[it->sub], D_00195AA4, 100);
        if (func_000968F8(obj) != 0) {
            if (func_000968F8(D_00195BE0->f1bb) != 0) {
                func_00096847(obj, 19);
                return;
            }
            if (D_00195BE0->f1bb == 0 && D_00195BE0->f1c3 == 0) {
                func_00096847(obj, 19);
                return;
            }
            if (D_00195BE0->f1bb != 0) {
                if (D_00195BE0->f1c3 != 0)
                    func_000967F9(21);
                func_00096847(obj, 19);
                return;
            }
            if (D_00195BE0->f1c3 != 0) {
                func_000967F9(21);
                func_00096847(obj, 19);
                return;
            }
        } else {
            if (func_000968F8(D_00195BE0->f1bb) != 0) {
                func_00096847(obj, 19);
                return;
            }
            func_00096782(obj, 19, 2);
        }
        break;
    case 6:
    case 12:
        if (it->type == 6 && (D_00195BE0->flags & 1))
            return;
        if (it->type == 12 && !(D_00195BE0->flags & 1))
            return;
        if (D_00196274 == 4)
            func_00069938(234, D_00195AA4, 100);
        if (it->type == 12)
            tbl = D_00186104;
        else
            tbl = D_001860DA;
        switch (tbl[it->sub]) {
        case 12:
            func_00096847(obj, 12);
            break;
        case 13:
            func_00096782(obj, 14, 2);
            break;
        case 17:
            func_00096847(obj, 17);
            break;
        case 22:
            func_00096847(obj, 24);
            break;
        case 26:
            func_00096847(obj, 26);
            break;
        }
        break;
    case 14:
        if (D_00196274 == 4)
            func_00069938(236, D_00195AA4, 100);
        func_00096782(obj, 10, 1);
        break;
    case 25:
        if (D_00196274 == 4)
            func_00069938(236, D_00195AA4, 100);
        switch (it->sub) {
        case 0:
            func_00096782(obj, 0, 1);
            break;
        case 1:
            func_00096782(obj, 6, 1);
            break;
        case 2:
            func_00096782(obj, 4, 1);
            break;
        case 3:
            func_00096782(obj, 2, 1);
            break;
        case 4:
            func_00096782(obj, 8, 1);
            break;
        case 5:
            func_00096782(obj, 0, 1);
            break;
        case 6:
            func_00096782(obj, 2, 1);
            break;
        }
        break;
    }
}
