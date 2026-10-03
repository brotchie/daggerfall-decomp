/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004D1E6 */
extern char D_00174FAC[];
extern char D_00174FB3[];
extern char D_00174FC0[];
extern char D_00174FDF[];
extern char D_00174FEC[];
extern unsigned char D_001940D5;
extern char *D_00195AA4;
extern int D_00195BE8;
extern char *D_00195C44;
extern unsigned char D_0019626F;
extern unsigned char D_00196272;
extern unsigned char D_00196274;
extern int D_001997BC;
extern int D_001997C0;
extern int D_001997C4;
extern int D_001997D0;
extern char *D_001997D4;
extern char *D_001997D8;
extern short D_001997E2;
extern short D_001997E8;
extern unsigned char D_001997EA;
extern unsigned char D_001997ED;
extern int func_00042F0F(int);
extern void func_00050069(char *);
extern int func_00069938(int, char *, int);
extern int func_0006CB53(char *, int);
extern int func_0006CDAB(char *);
extern int func_0006CE0D(char *);
extern void func_000A0040(char *, int, int, char *, int, int);
extern long func_000A006E(int, long, int);
extern char *func_000A00AF(int, char *, int);
extern int func_000A00CB(int, char *, int);
extern int func_000A0B42(int, char *, int);
extern void func_000A1023(char *, char *, int, char *, int, int);
extern int func_000A1235(int);

int func_0004D1E6(short a1)
{
    if (D_0019626F == 9 && D_00196274 == 8) return 1;
    D_001997ED = (a1 == 100);
    if (a1 != 0 || (D_00196274 == 0 && func_00042F0F(25) != 0)) {
        D_001940D5 |= 8;
        D_001997C4 = 0;
        D_001997D0 = 0;
        D_001997D4 = func_000A00AF(3640, D_00174FAC, 67);
        D_001997D8 = func_000A00AF(3640, D_00174FAC, 68);
        if ((D_001997E8 = func_0006CDAB(D_00174FB3)) < 1) {
            func_000A0040(D_00195C44, 0, 3640, D_00174FAC, 72, 4);
            if ((D_001997E8 = func_0006CE0D(D_00174FB3)) < 1)
                func_00050069(D_00174FC0);
            func_000A0B42(D_001997E8, D_00195C44, 3640);
            func_000A006E(D_001997E8, 0, 0);
        }
        func_000A00CB(D_001997E8, D_001997D4, 3640);
        func_000A1023(D_001997D8, D_001997D4, 3640, D_00174FAC, 79, 4);
        D_001997EA = 0;
        D_001997E2 = D_001997EA;
        D_001997BC = func_000A1235(D_001997E8);
        if (D_001997ED == 0) {
            D_00196274 = 9;
            D_00195BE8 = func_0006CB53(D_00174FDF, 0);
            D_001997C0 = func_0006CB53(D_00174FEC, 0);
            D_00196272 = 1;
            func_00069938(237, D_00195AA4, 100);
        }
    }
    return D_00196274 == 9;
}
