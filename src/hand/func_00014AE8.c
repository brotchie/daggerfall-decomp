/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00014AE8 */
struct rec { unsigned char type; char pad1[32]; unsigned short f33; char pad35[19]; unsigned char f54; };
struct who { char pad[31]; int f31; };
extern char D_001703C9[];
extern char D_001703D6[];
extern char D_001703E3[];
extern char D_001703F0[];
extern char D_00187CA8;
extern char D_00190D0F;
extern char D_00190D10;
extern int D_00195BE8;
extern char *D_00195C44;
extern char *D_00195D28;
extern int D_00195D34;
extern unsigned char D_0019626F;
extern unsigned char D_00196272;
extern unsigned char D_00196274;
extern unsigned char D_0019627E;
extern unsigned char D_0019627F;
extern void *D_0019658C;
extern struct rec *D_00196590;
extern struct rec *D_00196594;
extern char *D_00196598;
extern int D_001965A4;
extern int D_001965BC;
extern int D_001965C8;
extern int D_001965CC;
extern int D_001965D0;
extern int D_001965D4;
extern struct who *D_001965E0;
extern int D_001965E4;
extern int D_001965E8;
extern int D_001965EC;
extern int D_001965F0;
extern int D_001965F4;
extern int D_001965F8;
extern char D_001965FC[];
extern short D_001966A4;
extern unsigned char D_001966AA;
extern short D_001966AC;
extern char D_001966AE;
extern char D_001966B0;
extern char D_001966B1;
extern char D_001966B2;
extern char D_001966B3;
extern char D_001966B4;
extern char D_001966B7;
extern char D_001966B9;
extern char D_001966BA;
extern char D_001966BB;
extern void func_00015F4E(void);
extern void func_0001652C(int);
extern void func_000165C3(int);
extern void func_00017055(void);
extern void func_00017E5B(void);
extern void func_00018339(void);
extern int func_00018ACE(int);
extern int func_00019067(void);
extern int func_0001D46A(int);
extern void func_0003F09F(short, int);
extern void func_00040C87(int);
extern void func_000411BF(struct who *, int);
extern int func_00041347(void);
extern int func_0006CB53(char *, int);
extern int func_0009DC25(void);
extern void *func_000A00AF(int, char *, int);
extern void func_0012DB50(int);

int func_00014AE8(struct who *a1)
{
    int err;
    int msg;

    if (D_0019626F == 12 && D_00196274 == 8)
        return 1;
    if (a1 != 0) {
        if (D_00196594->f54 == 4 && (func_00041347() & 2)) {
            D_0019627F &= 2;
            D_0019627E = 7;
            func_00040C87(0);
            return 0;
        }
        D_001966B4 = 0;
        D_001966BA = 0;
        D_001966B9 = 0;
        D_001965E0 = a1;
        func_0012DB50(4);
        D_00196274 = 12;
        D_00195BE8 = func_0006CB53(D_001703C9, 0);
        D_001965E4 = func_0006CB53(D_001703D6, 0);
        D_001965F8 = func_0006CB53(D_001703E3, 0);
        D_0019658C = func_000A00AF(64000, D_001703F0, 225);
        D_00196272 = 1;
        D_001966BB = 0;
        D_001966B2 = 1;
        D_001966AE = 1;
        D_001966B1 = 0;
        D_001965F4 = 0;
        D_001965F0 = 13;
        D_001965E8 = 0;
        D_001966B3 = 1;
        D_001966A4 = 0;
        D_001966B7 = 0;
        D_001966AC = 0;
        D_001965CC = D_001965D0 = D_001965D4 = 0;
        D_00195D28 = D_001965FC;
        D_00196598 = D_00195C44 + 64768;
        func_00018339();
        D_001966B0 = 1;
        func_00017E5B();
        D_001966AA |= 2;
        D_00187CA8 = 0;
        D_00190D0F = 0;
        func_00017055();
        D_001965C8 = func_0009DC25();
        if (D_00190D10 == 0)
            func_000411BF(D_001965E0, D_001965EC);
        err = func_0001D46A(D_001965E0->f31);
        if (D_00196590->type == 15 || D_00196590->type == 14)
            msg = D_00196590->f33;
        else
            msg = D_00196594->f33;
        if (D_00196590->type == 15 || D_00196590->type == 14) {
            if (D_00196590->f54 < 5)
                D_001965A4 = D_00196590->f54;
            else
                D_001965A4 = 1;
        } else {
            if (D_00196594->f54 < 5)
                D_001965A4 = D_00196594->f54;
            else
                D_001965A4 = 1;
        }
        if (D_001965A4 == 1)
            D_001965A4 = 0;
        else if (D_001965A4 == 0)
            D_001965A4 = 1;
        D_001965BC = func_00019067();
        if (err != 0) {
            func_000165C3(err);
        } else {
            msg = func_00018ACE(msg);
            if (msg != 0) {
                if (D_001966BA) {
                    func_00015F4E();
                    func_0003F09F(msg, 1);
                    return 0;
                }
                func_0001652C(msg);
            } else if (D_00195D34 < 0) {
                func_0001652C(7206);
            } else if (D_00195D34 < 10) {
                func_0001652C(7207);
            } else if (D_00195D34 >= 10 && D_00195D34 < 30) {
                func_0001652C(7208);
            } else {
                func_0001652C(7209);
            }
        }
    }
    return D_00196274 == 12 ? 1 : 0;
}
