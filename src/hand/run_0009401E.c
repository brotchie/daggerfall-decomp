/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00093F0C to 0x0009401E, kept together for its switch table's alignment */
struct obj {
    char pad0[7];
    int x;                  /* 0x07 */
    int y;                  /* 0x0b */
    int z;                  /* 0x0f */
    char pad13[0x17 - 0x13];
    short f17;
    char pad19[0x1b - 0x19];
    short id;               /* 0x1b */
    char pad1d[0x1f - 0x1d];
    int f1f;
    char pad23[0x26 - 0x23];
    char f26;
    char pad27[0x2f - 0x27];
    int f2f;
    char pad33[0x43 - 0x33];
    char *f43;
    char pad47[0x34];
    short f7b;              /* 0x47 + 52 */
};
struct pc { char pad[367]; struct obj *slots[1]; };
extern char *D_00195A80;
extern struct obj *D_00195AA4;
extern struct obj *D_00195AA8;
extern int D_00195B20;
extern struct pc *D_00195BE0;
extern int D_00195D38;
extern int D_00195D44;
extern short D_00195D54;
extern char D_00196120[];
extern struct obj *D_001AA55C;
extern void func_0004BBD8(int, struct obj *, int);
extern int func_0008DD46(int, struct obj *);
extern int func_0008EB88(int);
extern int func_000949AA(struct obj *);
extern void func_00094FA8(void);
extern void func_000954DD(struct obj *, char *);
extern void func_00096B77(struct obj *, int);
extern int func_00099211(char *);
extern void func_000992FA(void);
extern char *D_001959D8[];
extern unsigned char D_00196288;
extern int D_001AA560;
extern int D_001AA564;
extern char *D_001AA57C;
extern short D_001AA584;
extern short D_001AA58A;
extern unsigned char D_001AA5F8;
extern unsigned char D_001AA5F9;
extern void func_00097CC9(void);

void func_00093F0C(void)
{
    short n;

    n = D_001AA58A - 4;
    if (n > 0 && n > D_001AA560)
        D_001AA560++;
}

void func_00093F4B(void)
{
    if (D_001AA564 == 0)
        return;
    D_001AA564--;
}

void func_00093F72(void)
{
    short n;

    n = D_001AA584 - 4;
    if (n > 0 && n > D_001AA564)
        D_001AA564++;
}

void func_00093FB1(int n)
{
    n -= 41;
    if (D_001959D8[n] == 0)
        return;
    if (D_00195D38 != 0) {
        D_00196288 = D_001AA5F8;
        func_00097CC9();
    }
    D_001AA5F9 = n;
    D_001AA57C = D_001959D8[D_001AA5F9];
    D_001AA560 = 0;
}

void func_0009401E(int slot)
{
    char *body;
    int l_20;
    int l_1C;
    struct obj *o;

    o = D_001AA55C = D_00195BE0->slots[slot];
    if (o == 0)
        return;
    D_00195AA8 = D_001AA55C;
    body = (char *)D_001AA55C + 71;
    D_00195A80 = body;
    switch (D_00195D44) {
    case 1:
        func_000954DD(D_001AA55C, body);
        break;
    case 2:
        func_000949AA(D_001AA55C);
        break;
    case 3:
        if (D_00195D38 == 4) {
            func_00096B77(D_001AA55C, slot);
            D_00195BE0->slots[slot] = 0;
            o->x = D_00195AA4->x;
            o->y = D_00195AA4->y;
            o->z = D_00195AA4->z;
            o->f2f = 0;
            if (o->id == 0)
                o->id = *(short *)(body + 52);
            func_0008DD46(D_00195B20, o);
            o->f1f = func_0008EB88(0);
            break;
        }
        if (D_00195D38 == 3 && func_00099211(body) == 0)
            break;
        func_00096B77(D_001AA55C, slot);
        D_00195BE0->slots[slot] = 0;
        o->x = D_00195AA4->x;
        o->y = D_00195AA4->y;
        o->z = D_00195AA4->z;
        o->f2f = 0;
        if (o->id == 0)
            o->id = *(short *)(body + 52);
        if (o->f26 == 0 && D_00195D38 == 0)
            func_0008DD46(D_00195B20, o);
        if (D_00196120 == o->f43)
            o->f17 = D_00195D54;
        o->f1f = func_0008EB88(0);
        func_0004BBD8(5, o, 0);
        func_000992FA();
        break;
    case 4:
        func_00094FA8();
        break;
    }
}
