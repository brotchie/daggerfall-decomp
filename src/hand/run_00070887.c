/* matched by the real Watcom C32 10.0a (-d2): a run of guilds from 0x00070496 to 0x00070887, kept together for its switch table's alignment */
struct mobile { unsigned char f0; char f1; int f2; };
struct thing {
    unsigned char type;
    char pad1[54];
    struct thing *next;         /* 55 */
    char pad3b[4];
    struct thing *child;        /* 63 */
    char pad43[4];
    struct mobile mob;          /* 71 */
};
struct cur { unsigned char f0; char f1; unsigned char f2; short f3; int f5; };
extern char D_00175EAA[];        /* __FILE__ */
extern char D_00176089[];
extern char D_00176096[];
extern char D_001788DF[];
extern char D_001789FA[];
extern struct thing *D_00195AA0;
extern char D_00195BE0[];
extern int D_00195BF4;
extern char D_00195D2C[];
extern char D_00195D30[];
extern char D_001960D9[];
extern char D_001961F5[];
extern struct cur *D_001A4A14;
extern void func_0003F09F(short, int);
extern void func_0005F401(int);
extern int func_00070B9C(struct mobile *, int);
extern void func_0007F1E3(int);
extern int func_0007F2A8(int);
extern void func_0008AF3E(int, int);
extern void func_0008DA1C(int);
extern struct thing *func_0008DCE3(struct thing *, int, int);
extern void func_000922F6(int, int, int);
extern int func_0009784E(int, int);
extern void func_00097A85(void);
extern int func_00097B2A(void);
extern int func_00097BD9(int);
extern int func_0009A993(int);
extern int func_000A0AD9();
void func_00070624(int, unsigned char);
int func_0007069D(int);

void func_00070496(void)
{
    if (((int)(unsigned char)*(signed char *)(*(char **)D_00195BE0 + 546)) == 100) goto L704C7;
    if (*(int *)(*(char **)D_00195BE0 + 529) != 0) goto L704C9;
L704C7:;
    goto L704DC;
L704C9:;
    if (((unsigned)*(int *)(*(char **)D_00195BE0 + 529)) < D_00195BF4) goto L704DE;
L704DC:;
    goto L704EA;
L704DE:;
    if (((int)(unsigned char)*(signed char *)D_001789FA) == 1) goto L704EC;
L704EA:;
    goto L70525;
L704EC:;
    *(signed char *)(*(char **)D_00195BE0 + 546) = 100;
    *(int *)(*(char **)D_00195BE0 + 529) = 0;
    func_000A0AD9((int)D_001961F5, (int)D_00176089, 13, (int)D_00175EAA, 1233);
L70525:;
    if (((int)(unsigned char)*(signed char *)(*(char **)D_00195BE0 + 543)) == 100) goto L70548;
    if (*(int *)(*(char **)D_00195BE0 + 533) != 0) goto L7054A;
L70548:;
    goto L7055D;
L7054A:;
    if (((unsigned)*(int *)(*(char **)D_00195BE0 + 533)) < D_00195BF4) goto L7055F;
L7055D:;
    goto L7056B;
L7055F:;
    if (((int)(unsigned char)*(signed char *)D_001789FA) == 1) goto L7056D;
L7056B:;
    return;
L7056D:;
    *(signed char *)(*(char **)D_00195BE0 + 543) = 100;
    *(int *)(*(char **)D_00195BE0 + 533) = 0;
    func_000A0AD9((int)D_001961F5, (int)D_00176096, 13, (int)D_00175EAA, 1243);
}

void func_000705B0(void)
{
    *(signed char *)D_001789FA = 1;
    func_0009A993(100);
}

void func_000705D9(void)
{
    func_00070624(108, 0);
}

void func_000705FD(void)
{
    func_00070624(42, 3);
}

void func_00070624(int a1, unsigned char a2)
{
{
    int l_1C;

    l_1C = (int)func_0008DCE3(D_00195AA0, 0, 13);
    *(signed char *)((char *)l_1C) = 10;
    *(short *)((char *)l_1C + 21) = 3;
    (D_001A4A14 = (struct cur *)(l_1C + 71))->f3 = a1;
    D_001A4A14->f2 = a2;
    D_001A4A14->f5 = D_00195BF4;
    D_001A4A14->f0 = 0;
}
}

int func_0007069D(int a1)
{
    int l_1C;

    *(int *)D_00195D2C = func_00097BD9((*(int *)D_00195D2C = a1));
    *(int *)D_00195D30 = ((*(int *)D_00195D30 = func_0009784E(*(int *)D_00195D2C, 0)) * *(int *)D_001788DF) / 256;
    func_00097A85();
    l_1C = func_00097B2A();
    return l_1C;
}

void func_00070715(void)
{
    func_0008DA1C((int)D_001960D9);
    func_0005F401((int)D_001960D9);
    func_000922F6((int)D_001960D9, 1, 4);
}

void func_00070755(void)
{
    int l_24;
    int l_20;
    int l_1C;
    int l_18;

    l_20 = 0;
    l_24 = (int)D_00195AA0->child;
L70775:;
    if (l_24 == 0) goto L707B3;
    if (((int)(unsigned char)*(signed char *)((char *)l_24)) != 11) goto L707A8;
    l_18 = l_24 + 71;
    if (((int)(unsigned char)*(signed char *)((char *)l_18)) >= 100) goto L707A8;
    l_20++;
L707A8:;
    l_24 = *(int *)((char *)l_24 + 55);
    goto L70775;
L707B3:;
    if (*(int *)(*(char **)D_00195BE0 + 499) == 0) goto L707C7;
    l_20++;
L707C7:;
    if (l_20 != 0) goto L707E1;
    func_0003F09F(30, 1);
    return;
L707E1:;
    l_1C = l_20 * 250;
    if (D_001A4A14->f2 != 142) goto L70837;
    l_1C = (l_1C * (((10 - D_001A4A14->f0) << 8) / 10)) / 256;
L70837:;
    l_1C = func_0007069D(l_1C);
    if (l_1C < 1) return;
    if (func_0007F2A8(l_1C) != 0) goto L70865;
    func_0003F09F(454, 1);
    return;
L70865:;
    func_0007F1E3(l_1C);
    func_0008AF3E((int)D_00195AA0, *(int *)D_00195BE0);
}

void func_00070887(void)
{
    struct thing *t;
    int n;
    int msg;
    struct mobile *m;

    msg = 0;
    m = 0;
    if (D_001A4A14->f2 == 142)
        return;
    t = D_00195AA0->child;
    while (t != 0) {
        if (t->type == 30) {
            func_0003F09F(454, 1);
            return;
        }
        t = t->next;
    }
    n = func_0007069D(100);
    if (n < 1)
        return;
    if (func_0007F2A8(n) == 0) {
        func_0003F09F(454, 1);
        return;
    }
    func_0007F1E3(n);
    t = func_0008DCE3(D_00195AA0, 0, 7);
    m = &t->mob;
    switch (D_001A4A14->f2) {
    case 143:
        m->f0 = 14;
        msg = 705;
        break;
    case 144:
        m->f0 = 133;
        msg = 707;
        break;
    case 145:
        m->f0 = 134;
        msg = 709;
        break;
    case 146:
        m->f0 = 129;
        msg = 710;
        break;
    case 147:
        m->f0 = 135;
        msg = 712;
        break;
    case 148:
        m->f0 = 255;
        msg = 716;
        break;
    case 149:
        m->f0 = 132;
        msg = 717;
        break;
    }
    m->f2 = (D_001A4A14 != 0 ? D_001A4A14->f0 + 4 : 4) * 1440 + D_00195BF4;
    m->f1 = func_00070B9C(m, D_001A4A14 != 0 ? D_001A4A14->f0 + 10 : 8);
    if (msg != 0)
        func_0003F09F(msg, 1);
}
