/* matched by the real Watcom C32 10.0a (-d2): a run of inven.c from 0x00097488 to 0x00097616, kept together for its switch table's alignment */
#pragma pack(1)
struct img {
    unsigned short f0;
    unsigned short f2;
    unsigned short h;
    unsigned short f6;
    unsigned short f8;
    unsigned short f10;
    char data[1];
};
#pragma pack()
extern char *D_00195AA0;
extern unsigned char *D_00195AF4;
extern struct img *D_001AA42C;
extern struct img *D_001AA430;
extern int D_001AA560;
extern int D_001AA564;
extern short D_001AA584;
extern short D_001AA58A;
extern int func_0008DA91(unsigned char *);
extern int func_00091D20(int);
extern int func_0008E55F(int, int (*)(int));
extern void func_00144FB4(int, int, int, int, char *);
void func_00097616(struct img *p, int a2);

int func_00097488(int a1)
{
    unsigned char *p;

    D_00195AF4 = 0;
    func_0008E55F(*(int *)(D_00195AA0 + 63), func_00091D20);
    if (D_00195AF4 == 0) return 0;
    if (a1 == 0) return 1;
    p = D_00195AF4 + 71;
    if (p[49] == 1) {
        func_0008DA91(D_00195AF4);
        return 1;
    }
    p[49]--;
    return 1;
}

void func_0009751E(void)
{
    int l_18;

    func_00097616(D_001AA560 != 0 ? D_001AA42C : D_001AA430, 0);
    func_00097616(D_001AA564 != 0 ? D_001AA42C : D_001AA430, 1);
    l_18 = D_001AA58A - 4;
    func_00097616(l_18 > 0 && D_001AA560 < l_18 ? D_001AA42C : D_001AA430, 2);
    l_18 = D_001AA584 - 4;
    func_00097616(l_18 > 0 && D_001AA564 < l_18 ? D_001AA42C : D_001AA430, 3);
}

void func_00097616(struct img *p, int a2)
{
    switch (a2) {
    case 0:
        func_00144FB4(163, 48, p->h, 20, p->data);
        break;
    case 1:
        func_00144FB4(261, 48, p->h, 20, p->data);
        break;
    case 2:
        func_00144FB4(163, p->f2 + p->f6 - 20, p->h, 20, p->data + p->f10 - p->h * 20);
        break;
    case 3:
        func_00144FB4(261, p->f2 + p->f6 - 20, p->h, 20, p->data + p->f10 - p->h * 20);
        break;
    }
}
