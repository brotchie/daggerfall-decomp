/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002ECBE */
struct vec3 { int x, y, z; };
struct move { struct vec3 pos; int f12, f16, f20; char *name; };
extern unsigned char D_0012B508;
extern char D_001709A1[];
extern char D_001709C6[];
extern char D_001709E4[];
extern char D_001709ED[];
struct pic { short file, rec; };
extern struct pic D_0017A16E[];
extern char D_00187B44[];
extern char D_001903A4[];
extern unsigned char D_001940D7;
extern unsigned char *D_00195AA0;
extern int D_00195AA4;
extern unsigned char *D_00195AF4;
extern int D_00195B50;
extern unsigned char *D_00195BE0;
extern int D_00195BF4;
extern unsigned short *D_00195BF8;
extern int D_00195C74;
extern int D_00195D7C;
extern short D_00195DA0;
extern unsigned char D_00196277;
extern short D_00196D64;
extern int func_0002257C(unsigned char *, int, struct move *, int);
extern void func_0002EBDE(int);
extern void func_0002EC79(int);
extern void func_0004BBD8(int, unsigned char *, int);
extern int func_00067C04(int);
extern void func_00069938(int, int, int);
extern void func_00070370(int, unsigned char);
extern int func_0007CBA1(char *);
extern int func_0007D6AE(int, int);
extern unsigned char *func_0008DCE3(int, int, int);
extern void func_0008E3F7(unsigned char *, void (*)(int));
extern int func_000A0040();
extern int func_000A1023();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, ...);

void func_0002ECBE(unsigned char *a1)
{
    int l_30;
    unsigned char *l_2C;
    unsigned char *l_28;
    struct move l_4C;
    int l_20;
    int l_1C;
    int l_18;

    l_2C = a1 + 71;
    l_20 = D_00196277;
    l_1C = D_00195C74;
    if (a1 == D_00195AA0) {
        func_00069938((*(unsigned short *)(D_00195BE0 + 64) & 1) + (D_00195BE0[67] * 3 + 2) ? 258 : 243, D_00195AA4, 100);
        D_00195D7C = 1000;
        return;
    }
    if (l_2C[506] == 146)
        func_00070370(6, 1);
    D_00195AF4 = a1;
    func_0008E3F7(*(unsigned char **)(a1 + 63), func_0002EC79);
    l_28 = *(unsigned char **)(a1 + 63);
    l_18 = func_00067C04(9);
    if (l_2C[67] < 43) {
        if (l_18 != 0) goto found;
        while (l_28 != 0) {
            if (*l_28 == 19) {
found:
                D_00195B50 = 0;
                func_0008E3F7(*(unsigned char **)(D_00195AA0 + 63), func_0002EBDE);
                if (D_00195B50 == 0 && *l_28 == 19) {
                    D_0012B508 = 146;
                    func_0007CBA1(D_001709A1);
                    func_0007CBA1(D_001709C6);
                    return;
                }
                if (D_00195B50 != 0) {
                    if (l_18 == 0 && func_0007D6AE(0, 100) > *(unsigned short *)(l_28 + 27))
                        break;
                    l_28 = func_0008DCE3(D_00195B50, 0, 0);
                    *(short *)(l_28 + 21) = 3;
                    *l_28 = 20;
                    *(unsigned short *)(l_28 + 27) = l_2C[67];
                }
                break;
            }
            l_28 = *(unsigned char **)(l_28 + 55);
        }
    }
    if (a1 != D_00195AA0)
        func_00069938(17, (int)a1, 105);
    *(int *)(D_00195BE0 + 509) = D_00195BF4;
    func_0004BBD8(2, a1, 0);
    l_2C = a1 + 71;
    func_000A0ED9(587, D_001709E4);
    func_000A0F5C(D_001903A4, D_001709ED, l_2C);
    func_0007CBA1(D_001903A4);
    *a1 = 44;
    if (l_2C[506] < 43) {
        if (*D_00195BF8 & 4)
            *(short *)(a1 + 27) = (D_00195DA0 << 7) + 1;
        else
            *(short *)(a1 + 27) = (D_0017A16E[l_2C[506]].file << 7) + D_0017A16E[l_2C[506]].rec;
    } else {
        *(short *)(a1 + 27) = (D_00195DA0 << 7) + 1;
    }
    D_00196D64 = 0;
    D_001940D7 |= 32;
    func_000A1023(&l_4C, a1 + 7, 12, D_001709E4, 605, 4);
    func_000A0040(&l_4C.f12, 0, 12, D_001709E4, 606, 4);
    l_4C.name = D_00187B44;
    func_0002257C(a1, 0, &l_4C, 0);
    D_00196277 = l_20;
    D_00195C74 = l_1C;
}
