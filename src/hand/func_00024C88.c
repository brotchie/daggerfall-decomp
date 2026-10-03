/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00024C88 */
struct skill { short v; short f2; short f4; };
struct player {
    char pad0[64];
    unsigned short flags;       /* 0x40 */
    char pad42[67];
    int gold;                   /* 0x85 */
    char pad89[8];
    short f145[6];              /* 0x91 */
    struct skill skills[35];    /* 0x9d */
    char pad[181];
    char f548;                  /* 0x224 */
};
struct npc {
    char pad0[64];
    unsigned short flags;       /* 0x40 */
    char pad42;
    char f67;                   /* 0x43 */
    char pad44[61];
    char f129;                  /* 0x81 */
    char pad82[419];
    short f549;                 /* 0x225 */
};
struct faction { char pad0[29]; short rep; };
struct dun { short f0; char pad[78]; };
struct mobile {
    char pad0[32];
    unsigned short f32;         /* 0x20 */
    unsigned short f34;         /* 0x22 */
    char pad24[13];
    char f49;                   /* 0x31 */
};
struct thing {
    unsigned char type;
    char pad1[20];
    short f21;                  /* 21 */
    char pad17[48];
    struct mobile mob;          /* 71 */
};
extern char *D_00147954;
extern char D_00170738[];        /* __FILE__ */
extern char D_00170765[];
extern char D_00170773[];
extern char D_00170777[];
extern unsigned char D_00178630[];   /* _IsTable */
extern int D_0018DDD4;
extern int D_0018DDD8;
extern int D_0018DDDC;
extern int D_0018DDE0;
extern int D_0018DDE4;
extern struct dun D_0018F08E[];
extern char D_001903A4[];
extern int D_00190BE4[];
extern int D_00190CAC;
extern short D_00190D68;
extern short D_00190D6A;
extern struct thing *D_00195AA0;
extern struct player *D_00195BE0;
extern char D_001962AB;
extern struct faction *func_000192EE(short);
extern short func_0001C713(struct npc *, char *, int);
extern unsigned char *func_0002482A(unsigned char *);
extern void func_000252C7(int);
extern void func_000252E2(int);
extern void func_0004633F(char *, struct npc *);
extern void func_0005E540(unsigned short, int, struct mobile *);
extern int func_0006CB53(char *, char *);
extern struct thing *func_0008DCE3(struct thing *, int, int);
extern void func_00097101(struct thing *);
extern void func_000972C7(struct thing *, struct thing *, int);
extern void func_000A0040(void *, int, int, char *, int, int);
extern int func_000A0D13(unsigned char *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

unsigned char *func_00024C88(unsigned char *a1)
{
    int n;
    struct thing *o;
    struct faction *f;
    struct npc *p;
    char *q;
    struct mobile *m;
    unsigned char c;

    while (*a1 <= 32)
        a1++;
    if (*(unsigned short *)a1 == 0x5047)
        D_00195BE0->gold += func_000A0D13(func_0002482A(a1));
    else if (*(unsigned short *)a1 == 0x6672) {
        a1 += 2;
        f = func_000192EE(func_000A0D13(a1));
        if (f != 0)
            f->rep += func_000A0D13(func_0002482A(a1));
    } else if (*(unsigned short *)a1 == 0x7272)
        D_0018F08E[D_00190D68].f0 += func_000A0D13(func_0002482A(a1));
    else if (*(unsigned short *)a1 == 0x494D) {
        a1 = func_0002482A(a1);
        if (*a1 == '+')
            func_000252C7(func_000A0D13(a1));
        else
            func_000252E2(func_000A0D13(a1));
    } else if (*(unsigned short *)a1 == 0x5252)
        D_00195BE0->f548 += func_000A0D13(func_0002482A(a1));
    else if (*(unsigned short *)a1 == 0x4452)
        D_0018DDD4 += func_000A0D13(func_0002482A(a1));
    else if (*(unsigned short *)a1 == 0x524D)
        D_0018DDD8 += func_000A0D13(func_0002482A(a1));
    else if (*(unsigned short *)a1 == 0x4854)
        D_0018DDDC += func_000A0D13(func_0002482A(a1));
    else if (*(unsigned short *)a1 == 0x5052)
        D_0018DDE0 += func_000A0D13(func_0002482A(a1));
    else if (*(unsigned short *)a1 == 0x5446)
        D_0018DDE4 += func_000A0D13(func_0002482A(a1));
    else if (*(unsigned short *)a1 == 0x4541 || *(unsigned short *)a1 == 0x4641 || *(unsigned short *)a1 == 0x4F41) {
        c = a1[1];
        p = (struct npc *)(D_00147954 + 70000);
        q = D_00147954 + 75000;
        func_000A0040(p, 0, 560, D_00170738, 279, 4);
        func_000A0040(q, 0, 74, D_00170738, 280, 4);
        a1 = func_0002482A(a1);
        if (*a1 == 'F')
            p->flags |= 1;
        else if (*a1 == 'O')
            p->flags |= (D_00195BE0->flags & 1) ^ 1;
        a1 = func_0002482A(a1);
        p->f67 = func_000A0D13(a1);
        a1 = func_0002482A(a1);
        n = func_000A0D13(a1);
        p->f549 = n;
        a1 = func_0002482A(a1);
        p->f129 = func_000A0D13(a1);
        func_000A0ED9(292, D_00170738);
        func_000A0F5C(D_001903A4, D_00170765, n);
        func_0006CB53(D_001903A4, q);
        if (c == 'E') {
            func_0004633F(D_00170773, p);
            func_0001C713(p, q, 0);
        } else {
            func_0004633F(D_00170777, p);
            func_0001C713(p, q, 1);
        }
    } else if (*(unsigned short *)a1 == 0x5449) {
        a1 = func_0002482A(a1);
        n = func_000A0D13(a1);
        o = func_0008DCE3(D_00195AA0, 0, 107);
        o->type = 2;
        o->f21 = 1;
        a1 = func_0002482A(a1);
        m = &o->mob;
        D_001962AB = func_000A0D13(func_0002482A(a1)) + 1;
        func_0005E540(n, func_000A0D13(a1), m);
        if (m->f32 == 3 && m->f34 == 18) {
            m->f49 = 1;
            func_000972C7(D_00195AA0, o, 1);
        } else
            func_00097101(o);
    } else if (*a1 == '&')
        D_00190D6A = 0;
    else if (*a1 == '#')
        D_00190BE4[D_00190CAC] = func_000A0D13(a1 + 1);
    else if (*a1 == '!')
        D_00190BE4[D_00190CAC + 12] = func_000A0D13(a1 + 1);
    else if (*a1 == '?')
        D_00190BE4[D_00190CAC + 24] = func_000A0D13(a1 + 1);
    else if (D_00178630[(unsigned char)(*a1 + 1)] & 0x20) {
        n = func_000A0D13(a1);
        if (n >= 35)
            n = 0;
        D_00195BE0->skills[n].v += func_000A0D13(func_0002482A(a1));
    } else if (*a1 == 'r' && D_00178630[(unsigned char)(a1[1] + 1)] & 0x20) {
        n = func_000A0D13(a1 + 1);
        D_00195BE0->f145[n] += func_000A0D13(func_0002482A(a1));
    }
    while (*a1 != '\n')
        a1++;
    a1++;
    return a1;
}
