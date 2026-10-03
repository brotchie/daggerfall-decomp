/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009C00A */
#pragma pack(1)
struct Q { char pad[4]; char name[28]; };
struct C { char pad[0x85]; int v; };
extern char D_0012B508;
extern char D_0017743D[];
extern unsigned short D_00178A0E;
extern short D_0018886C[];
extern int D_001889BD;
extern char D_001903A4[];
extern struct C *D_00195BE0;
extern struct Q *D_00196A7C;
extern int D_001AA680;
extern unsigned short *D_001AA6A0;
extern unsigned char D_001AA6A6;
extern void func_0007CA1F(char *, short, short, int, unsigned char);
extern void func_0007CA85(char *, short, short, int, unsigned char);
extern int func_0009D61E(void);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern char *func_000A0D8F(int, char *, int);
extern char *func_000A0DD9(int, char *, int);
extern void func_00144D00(short, short, short, short);
extern int func_00144F68();

void func_0009C00A(void)
{
    int i;

    func_00144F68(D_001AA6A0[0], D_001AA6A0[1], D_001AA6A0[2], D_001AA6A0[3], (char *)D_001AA6A0 + 12);
    if (D_001AA6A6 == 100) {
        func_000A0AD9(D_001903A4, D_00196A7C[D_001889BD].name, 160, D_0017743D, 654);
        func_0007CA85(D_001903A4, 160, 74, 145, 156);
        return;
    }
    D_0012B508 = 199;
    for (i = 0; i < 6; i++) {
        if (D_00178A0E & (1 << i))
            func_00144D00(D_0018886C[i * 4], D_0018886C[i * 4 + 1], 4, 4);
    }
    func_0007CA1F(func_000A0DD9(D_00195BE0->v, D_001903A4, 10), 148, 97, 145, 156);
    func_0007CA1F(func_000A0D8F(func_0009D61E(), D_001903A4, 10), 117, 107, 146, 156);
    func_0007CA1F(func_000A0DD9(D_001AA680 / 1440 + 1, D_001903A4, 10), 129, 117, 145, 156);
    func_000A0AD9(D_001903A4, D_00196A7C[D_001889BD].name, 160, D_0017743D, 668);
    func_0007CA85(D_001903A4, 160, 2, 145, 156);
}
