/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00037014 */
extern char D_0012AC00[];
extern char D_0012AC04[];
extern char D_0012AC06[];
extern char D_0012B508[];
extern char D_00142309[];
extern char D_00143550[];
extern char D_00170B13[];
extern char D_00170B1C[];
extern char D_00178A0A[];
extern char D_0017A94B[];
extern char D_0017B202[];
extern char D_0017B235[];
extern char D_0017B23F[];
extern char D_0017B241[];
extern char D_0017B243[];
extern char D_0017B245[];
extern char D_0017B247[];
extern char D_00182686[];
extern char D_00182752[];
extern char D_001845D0[];
extern char D_001903A4[];
extern char D_00195AA4[];
extern char D_00195B58[];
extern char D_00195B7C[];
extern char D_00195BE0[];
extern char D_00195BE8[];
extern char D_00195C44[];
extern char D_00195F30[];
extern char D_00196271[];
extern char D_00196274[];
extern char D_00196279[];
extern char D_0019962C[];
extern char D_0019962E[];
extern struct { unsigned char a:2; unsigned char f:1; } D_001940D4;
extern int func_00036F69(int);
extern void func_00037732(void);
extern void func_00038911(void);
extern void func_000390D3(void);
extern short func_000392AD(void);
extern int func_0003A0C0(char *, char *);
extern void func_0005A5D2(char *, int, int);
extern void func_00069938(int, int, int);
extern void func_0007CA1F(char *, int, int, int, unsigned char);
extern void func_0007EC8F(short, short, int, char *, char *);
extern int func_0007F558(void);
extern void func_000A0040(char *, int, int, char *, int, int);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern char *func_000A0DD9(int, char *, int);
extern void func_000A1054(char *, char *, char *, int, int);
extern short func_000CAE07(int);
extern void func_000CB552(int);
extern void func_000CD20E(int, int, int);
extern void func_000CE31C(char *, char *, int, int, int);
extern void func_0012DB50(int);

#define P (*(unsigned char **)D_00178A0A)

void func_00037014(void)
{
    int l_24;
    short i;
    short l_1C;
    short v;

    if (func_00036F69(0) == 0) return;
    func_000CB552(*(int *)D_00195BE8);
    func_0012DB50(4);
    v = *(short *)(P + 8) + *(short *)(P + 10) + *(short *)(P + 12);
    v = v * ((short *)D_0017B235)[P[7]] >> 1;
    func_0007CA1F(func_000A0DD9(*(short *)(*(char **)D_00195BE0 + 141), D_001903A4, 10), 43, 149, 145, 156);
    func_0007CA1F(func_000A0DD9(func_0007F558(), D_001903A4, 10), 40, 158, 145, 156);
    func_0007CA1F(func_000A0DD9(v << 2, D_001903A4, 10), 59, 167, 145, 156);
    func_0007CA1F(func_000A0DD9(func_0003A0C0(*(char **)D_00178A0A, *(char **)D_00195BE0), D_001903A4, 10), 70, 176, 145, 156);
    func_0007CA1F((char *)P + 47, 60, 185, 145, 156);
    func_000CE31C(*(char **)D_00195B58 + P[6] * 640 + 24, *(char **)D_00143550 + P[6] * 5120 + 36779, 16, 16, 40);
    func_000CE31C(*(char **)D_00195B58 + P[7] * 640, *(char **)D_00143550 + P[7] * 5120 + 36755, 24, 16, 40);
    func_000CD20E(288, 94, P[72]);
    func_0012DB50(1);
    *D_0012B508 = 146;
    for (i = 0; i < 3; i++) {
        if (P[i * 2] == 255)
            continue;
        func_000A0AD9(D_001903A4, *(char **)(D_00182686 + P[i * 2] * 4), 160, D_00170B13, 641);
        if (P[i * 2 + 1] != 255 && *(int *)(D_00182752 + P[i * 2] * 48 + P[i * 2 + 1] * 4) != 0) {
            func_000A1054(D_001903A4, D_00170B1C, D_00170B13, 644, 160);
            func_000A1054(D_001903A4, *(char **)(D_00182752 + P[i * 2] * 48 + P[i * 2 + 1] * 4), D_00170B13, 645, 160);
        }
        func_0005A5D2(D_001903A4, 160, (i << 5) + 30);
    }
    func_0012DB50(4);
    if (!*D_0019962E && !D_001940D4.f)
        func_000390D3();
    if (D_001940D4.f && (i = func_000392AD()) > -1)
        (*(void (**)(int))D_00195B7C)((*(unsigned char **)D_00195C44)[i + 32000]);
    func_00038911();
    if (*(short *)D_0019962C > -1 && *D_00196271) {
        if (*(unsigned char *)D_00196271 == 1) {
            i = P[(*(short *)D_00195F30 = *(short *)D_0019962C) * 2 + 1];
            if (i == 255)
                i = 0;
            if ((*D_0019962E = ((char (*)[12])D_0017A94B)[P[*(short *)D_00195F30 * 2]][i]) == 0)
                *(short *)(P + 8 + *(short *)D_00195F30 * 2) = func_000CAE07(*(unsigned char *)(D_0017B202 + P[*(short *)D_00195F30 * 2]) - 1);
        } else {
            P[*(short *)D_0019962C * 2] = P[*(short *)D_0019962C * 2 + 1] = 255;
            func_000A0040((char *)P + 14 + *(short *)D_0019962C * 3, 1, 3, D_00170B13, 681, 3);
            func_000A0040((char *)P + 23 + *(short *)D_0019962C * 3, 1, 3, D_00170B13, 682, 3);
            func_000A0040((char *)P + 32 + *(short *)D_0019962C * 5, 1, 5, D_00170B13, 683, 5);
            *(short *)(P + 8 + *(short *)D_0019962C * 2) = 0;
        }
        *(short *)D_0019962C = -1;
    }
    if (*(short *)D_0019962C > -1 && *(unsigned char *)D_00196274 != 8)
        *(short *)D_0019962C = -1;
    if (!*D_0019962E && !D_001940D4.f)
        func_0007EC8F(5, 22, 18, D_0017B23F, D_001845D0);
    if (*D_00142309 && !*D_0019962E)
        func_00037732();
    if (!*D_0012AC00 || *D_0012AC00 && *D_00196279)
        return;
    if (*(unsigned char *)D_00196274 == 2 && !*D_0019962E && !D_001940D4.f) {
        for (i = 0; i < 18; i++) {
            if (*(short *)D_0012AC04 > *(short *)(D_0017B23F + i * 12) && *(short *)D_0012AC04 < *(short *)(D_0017B243 + i * 12) && *(short *)D_0012AC06 > *(short *)(D_0017B241 + i * 12) && *(short *)D_0012AC06 < *(short *)(D_0017B245 + i * 12)) {
                func_00069938(203, *(int *)D_00195AA4, 110);
                (*(void (**)(void))(D_0017B247 + i * 12))();
            }
        }
    }
}
