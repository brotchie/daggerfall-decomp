/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000562A1 */
struct pair { short a; short b; };
extern unsigned char D_0012B508;
extern short D_0012DA44;
extern char D_001756A3[];
extern char D_001756AE[];
extern char D_001756B6[];
extern char D_001756B9[];
extern char *D_00180A7A[];
extern char *D_00180B0A[];
extern char *D_001830E6[];
extern char **D_001857A1[];
extern unsigned char *D_00185875[];
extern char **D_001858AB[];
extern char D_00185B30[];
extern char D_00185B54[];
extern char D_001903A4[];
extern signed char D_00190CE4[];
extern char D_00190CEE[];
extern char D_00190CEF[];
extern char D_00190CF0[];
extern char D_00190D02[];
extern char D_00190D03[];
extern char D_00190D04[];
extern char *D_00195B58;
extern struct pair D_001998E0[];
extern char *D_00199908;
extern int D_0019990C;
extern char D_00199910[];
extern unsigned char D_001AA5F9;
extern char *func_00057981(unsigned char);
extern int func_00057AB4(void);
extern int func_00057CFB(void);
extern void func_0007CA1F(char *, short, short, short, unsigned char);
extern int func_0007F558(void);
extern int func_000934F6(int, int, char *);
extern void func_00095D2C(char *);
extern char *func_000A0DD9(int, char *, int);
extern void func_0012DB50(int);
extern void func_00144F68(int, int, int, int, char *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern void func_000A0F5C(char *, char *, ...);

void func_000562A1(void)
{
    short l_20;
    short l_1C;
    short l_18;

    func_00144F68(175, D_001AA5F9 * 9 + 6, 81, 9, D_00195B58 + D_001AA5F9 * 729);
    func_0007CA1F(func_000A0DD9(func_0007F558(), D_001903A4, 10), 70, 15, 145, 156);
    if (D_00199908 != 0)
        func_0007CA1F(D_00199908, 51, 3, 145, 156);
    if (D_00199908 != 0)
        func_0007CA1F(func_000A0DD9(func_00057CFB(), D_001903A4, 10), 63, 27, 145, 156);
    if (D_00199908 != 0) {
        func_000A0ED9(161, D_001756A3);
        func_000A0F5C(D_001903A4, D_001756AE, func_00057AB4(), *(unsigned short *)(D_00199908 + 61));
        func_0007CA1F(D_001903A4, 96, 39, 145, 156);
    }
    func_00095D2C(D_00185B54);
    if (D_00199908 != 0)
        func_000934F6(D_0019990C, 0, D_00185B30);
    func_0012DB50(3);
    for (l_1C = l_18 = l_20 = 0; l_20 < 10; l_20++) {
        if (D_00190CE4[l_20] == -1) continue;
        D_0012B508 = 146;
        if (D_00199910[l_20] != 0)
            D_0012B508 = 193;
        if (D_00190CE4[l_20] == 0) {
            D_00190CEE[l_1C] = l_20;
            D_00190CEF[l_1C] = 255;
            func_000A0ED9(179, D_001756A3);
            func_000A0F5C(D_001903A4, D_001756B6, D_00180A7A[D_001998E0[l_20].a]);
            func_0007CA1F(D_001903A4, 10, l_1C * D_0012DA44 + 60, D_0012B508, 156);
            l_1C++;
            if (D_001998E0[l_20].b == -1) {
                l_1C++;
            } else {
                D_00190CEF[l_1C] = l_20;
                D_00190CF0[l_1C] = 255;
                if (D_001998E0[l_20].a < 3) {
                    func_000A0ED9(189, D_001756A3);
                    func_000A0F5C(D_001903A4, D_001756B9, func_00057981(D_00185875[D_001998E0[l_20].a][D_001998E0[l_20].b]));
                } else {
                    func_000A0ED9(191, D_001756A3);
                    func_000A0F5C(D_001903A4, D_001756B9, D_001857A1[D_001998E0[l_20].a][D_001998E0[l_20].b]);
                }
                func_0007CA1F(D_001903A4, 10, l_1C * D_0012DA44 + 60, D_0012B508, 156);
                l_1C += 2;
            }
        } else {
            D_00190D02[l_18] = l_20;
            D_00190D03[l_18] = 255;
            func_000A0ED9(200, D_001756A3);
            func_000A0F5C(D_001903A4, D_001756B6, D_00180B0A[D_001998E0[l_20].a]);
            func_0007CA1F(D_001903A4, 108, l_18 * D_0012DA44 + 60, D_0012B508, 156);
            l_18++;
            if (D_001998E0[l_20].b == -1) {
                l_18++;
            } else {
                D_00190D03[l_18] = l_20;
                D_00190D04[l_18] = 255;
                if (D_001998E0[l_20].a == 0) {
                    func_000A0ED9(210, D_001756A3);
                    func_000A0F5C(D_001903A4, D_001756B9, D_001830E6[D_001998E0[l_20].b]);
                } else {
                    func_000A0ED9(212, D_001756A3);
                    func_000A0F5C(D_001903A4, D_001756B9, D_001858AB[D_001998E0[l_20].a][D_001998E0[l_20].b]);
                }
                func_0007CA1F(D_001903A4, 108, l_18 * D_0012DA44 + 60, D_0012B508, 156);
                l_18 += 2;
            }
        }
    }
}
