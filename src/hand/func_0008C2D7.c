/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008C2D7 */
extern unsigned char D_0012B508;
extern short D_0012DA44;
extern short D_00142928;
extern short D_0014292C;
extern char D_00176E2C[];        /* __FILE__ */
extern char D_00190B44[];
extern char *D_00195B94;
extern short D_001A9AAC;
extern short D_001A9AAE;
extern void func_0005A54A(char *, short, short);
extern int func_0008C462(void);
extern int func_0008C6E9(unsigned char);
extern int func_0008C9D2(char *, short);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern short func_000A0DF4(char *);
extern void func_000CD308(void);
extern void func_000CD31A(void);
extern void func_000CDD81(int);
extern void func_0012B2EB(void);
extern void func_00142790(void);
extern void func_00144D00(short, short, short, short);
extern void func_001531F0(short, short, short, short);

int func_0008C2D7(char *a1, short a2, short a3, short a4, short a5, short a6)
{
    short key;
    unsigned char old;
    int r;

    func_0012B2EB();
    func_00142790();
    D_00195B94 = a1;
    func_000A0AD9(D_00190B44, D_00195B94, 160, D_00176E2C, 56);
    D_001A9AAE = func_000A0DF4(D_00195B94);
    D_001A9AAC = a6;
    for (;;) {
        key = func_0008C462();
        if (key == 0) {
            old = D_0012B508;
            D_0012B508 = 0;
            func_00144D00(a2, a3, a4, a5);
            D_0012B508 = 12;
            D_00142928 = a2 + func_0008C9D2(D_00195B94, D_001A9AAE);
            D_0014292C = a3;
            if (*(int *)0x46c & 32)
                func_001531F0(D_00142928, D_0014292C, D_00142928, D_0014292C + D_0012DA44 - 1);
            D_0012B508 = old;
            func_0005A54A(D_00195B94, a2, a3);
            func_000CDD81(1);
            func_000CD308();
            func_000CD31A();
            continue;
        }
        r = func_0008C6E9(key);
        if (r == 32768) {
            func_000A0AD9(D_00195B94, D_00190B44, 4, D_00176E2C, 83);
            return 0;
        }
        if (r != 0x87654321)
            return r;
    }
}
