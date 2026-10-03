/* matched by the real Watcom C32 10.0a (-d2): a run of generate.c from 0x00090FA1 to 0x0009169B, kept together for its switch table's alignment */
struct rect { short x, y, w, h; };
struct img { struct rect r; char pad[4]; char data[1]; };
struct region { short x1, y1, x2, y2; void (*fn)(); };     /* mouse hit box */
struct uimg { unsigned short x, y, w, h; char pad8[2]; unsigned short len; char data[1]; };
struct box { short x0; char p2[2]; short y0; char p6[6]; short x1; char pe[2]; short y1; char p12[6]; };
extern char D_0012AC00;
extern short D_0012AC04;
extern short D_0012AC06;
extern unsigned char D_0012B508;
extern short D_0012DA44;
extern char *D_00143550;
extern char D_00176F41[];       /* __FILE__ */
extern char D_00176FC3[];
extern char D_00176FC8[];
extern char D_00176FCD[];
extern char D_00176FD2[];
extern char D_00176FD7[];
extern char D_00176FDC[];
extern char D_0017CC27[];
extern char D_0018801C[];       /* struct region[] */
extern char D_0018801E[];
extern char D_00188020[];
extern char D_00188022[];
extern char D_00188024[];
extern struct box D_001880C6[3];
extern short D_0018810C;
extern short D_00188110;
extern char D_001903A4[];
extern char D_00190B44[];
extern int D_00190BE8;
extern short D_00190D64;
extern short D_00190D6A;
extern short D_00190DE4[];
extern short D_00190DEA[];
extern short D_00190DEC;
extern short D_00190DEE;
extern short D_00190DF0[3];
extern struct img *D_00195B5C;
extern char *D_00195B60;
extern char *D_00195BE0;
extern char *D_00195BEC;
extern char D_00196279;
extern struct uimg *D_001AA410;
extern unsigned char D_001AA41A;
extern void func_0003F09F(int, int);
extern void func_0004633F(char *, char *);
extern void func_00050344(char *, char *);
extern void func_00053A89(char *, short, int (*)(void));
extern void func_0007CA1F(char *, short, short, int, unsigned char);
extern void func_0007CA85(char *, short, short, int, unsigned char);
extern int func_00090C3A(void);
extern char *func_000A0DD9(int, char *, int);
extern int func_000A1023();
extern void func_0012DB50(int);
extern int func_00144F68();
extern void func_00144FB4(int, int, int, int, char *);

#pragma pack(1)
struct E { short s; char pad[4]; };
struct B { char pad[0x9d]; struct E e[1]; };

int func_00090FA1(int first, int last)
{
    int i;
    int x;
    int y;

    for (;;) {
        func_00090C3A();
        func_000A1023(655360, D_00143550, 64000, D_00176F41, 238, 4);
        if (D_0012AC00 != 0 && D_00196279 == 0 &&
            D_0012AC04 > *(short *)D_0018801C && D_0012AC04 < *(short *)D_00188020 &&
            D_0012AC06 > *(short *)D_0018801E && D_0012AC06 < *(short *)D_00188022) {
            if (D_00190D64 == 0 && D_00190DEA[0] == 0 && D_00190DEC == 0 && D_00190DEE == 0)
                return 0;
            func_0003F09F(14, 1);
        }
        x = D_0012AC04;
        y = D_0012AC06;
        for (i = first; i <= last; i++) {
            if (D_001AA41A == 255 && i == 35) {
                x += -119;
                y += 53;
            }
            if (D_0012AC00 != 0 && *(short *)(D_0018801C + i * 12) < x && *(short *)(D_00188020 + i * 12) > x &&
                *(short *)(D_0018801E + i * 12) < y && *(short *)(D_00188022 + i * 12) > y)
                (*(void (**)())(D_00188024 + i * 12))(i);
        }
        if (D_00190BE8 != 0)
            return 1;
    }
}

void func_0009114E(int n)
{
    D_00195BE0[130] = n - 35;
}

void func_0009117A(void)
{
    int k;
    struct uimg *p;

    p = D_001AA410;
    k = 0;
    while ((unsigned char)D_00195BE0[128] > k) {
        p = (struct uimg *)(p->len + (char *)p + 12);
        k++;
    }
    k = p->y + 16 + p->h;
    if (k >= 63)
        k = 16 - (k - 63);
    else
        k = 16;
    func_00144FB4(p->x + 24, p->y + k, p->w, p->h, p->data);
}

void func_00091247(void)
{
    int i;
    short x;

    func_00144FB4(44, D_00190D6A, (unsigned short)D_00195B5C->r.w, (unsigned short)D_00195B5C->r.h, D_00195B5C->data);
    D_0012B508 = 146;
    x = (D_0018810C + D_00188110) >> 1;
    func_0012DB50(4);
    for (i = 0; i < 8; i++) {
        func_0007CA85(func_000A0DD9(((short *)(D_00195BE0 + 32))[i], D_001903A4, 10), x, (short)(((struct region *)D_0018801C)[i + 20].y2 - D_0012DA44 + 1), 145, 141);
    }
    func_0007CA85(func_000A0DD9(D_00190D64, D_001903A4, 10), 51, (short)(D_00190D6A + 13 - D_0012DA44 + 1), 145, 141);
    func_00050344(D_00195BE0, D_00195BEC);
    if (D_001AA41A == 255)
        return;
    func_0004633F(D_00176FC3, D_00190B44);
    func_0007CA1F(D_00190B44, 83, 22, 145, 141);
    func_0004633F(D_00176FC8, D_00190B44);
    func_0007CA1F(D_00190B44, 103, 32, 145, 141);
    func_0004633F(D_00176FCD, D_00190B44);
    func_0007CA1F(D_00190B44, 112, 49, 145, 141);
    func_0004633F(D_00176FD2, D_00190B44);
    func_0007CA1F(D_00190B44, 121, 71, 145, 141);
    func_0004633F(D_00176FD7, D_00190B44);
    func_0007CA1F(D_00190B44, 97, 93, 145, 141);
    func_0004633F(D_00176FDC, D_00190B44);
    func_0007CA1F(D_00190B44, 101, 110, 145, 141);
    func_0007CA1F(D_00190B44, 122, 120, 145, 141);
}

void func_000914C8(void)
{
    int i;
    int k;

    for (i = 0; i < 3; i++) {
        func_00144F68(203, D_00190DE4[i], *(unsigned short *)(D_00195B60 + 4), *(unsigned short *)(D_00195B60 + 6), D_00195B60 + 12);
        func_0007CA85(func_000A0DD9(D_00190DEA[i], D_001903A4, 10), 221, D_00190DE4[i] + 8 - D_0012DA44 + 1, 145, 141);
    }
    for (i = 0; i < 12; i++) {
        k = *(unsigned char *)(D_00195BEC + i + 16);
        func_0007CA1F(*(char **)(D_0017CC27 + (k << 2)), *(short *)(D_0018801C + ((i + 2) * 12)) + 2, *(short *)(D_0018801E + ((i + 2) * 12)) + 1, 145, 141);
        func_0007CA85(func_000A0DD9(((struct B *)D_00195BE0)->e[k].s, D_001903A4, 10), 192, *(short *)(D_0018801E + ((i + 2) * 12)) + 1, 145, 141);
    }
}

void func_00091657(void)
{
}

void func_0009166F(void)
{
    func_00053A89(D_00195BE0, 31, func_00090C3A);
}

void func_0009169B(int n)
{
    switch (n) {
    case 2:
    case 3:
    case 4:
        D_001880C6[0].x1 = D_001880C6[0].x0 = D_00190DE4[0] = *(short *)(D_0018801E + n * 12);
        D_001880C6[0].y1 = D_001880C6[0].y0 = D_00190DE4[0] + 8;
        D_00190DF0[0] = n - 2;
        break;
    case 5:
    case 6:
    case 7:
        D_001880C6[1].x1 = D_001880C6[1].x0 = D_00190DE4[1] = *(short *)(D_0018801E + n * 12);
        D_001880C6[1].y1 = D_001880C6[1].y0 = D_00190DE4[1] + 8;
        D_00190DF0[1] = n - 2;
        break;
    default:
        D_001880C6[2].x1 = D_001880C6[2].x0 = D_00190DE4[2] = *(short *)(D_0018801E + n * 12);
        D_001880C6[2].y1 = D_001880C6[2].y0 = D_00190DE4[2] + 8;
        D_00190DF0[2] = n - 2;
        break;
    }
}
