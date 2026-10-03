/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00088A68 */
struct rec131 {
    short f0;
    unsigned char f2[129];
};
extern char D_000C2BB8[];
extern char D_00176C94[];
extern unsigned char D_0017A288[];
extern unsigned char D_0017CA0A[];
extern unsigned int D_00195BF4;
extern unsigned char D_00195E2A[];
extern int D_001A94C8;
extern int D_001A94CC;
extern int D_001A94D0;
extern struct rec131 D_001A94D4[];
extern int func_0001FFF1(void);
extern void func_000A1023(char *, unsigned char *, int, char *, int, int);

void func_00088A68(void)
{
    int l_1C;
    int l_18;

    if (D_001A94D4[D_001A94C8].f2[0] > 50)
        D_001A94D4[D_001A94C8].f2[0] = 50;
    func_000A1023(D_000C2BB8, D_001A94D4[D_001A94C8].f2, 129, D_00176C94, 1455, 4);
    D_001A94CC = D_0017CA0A[D_001A94C8] * 100 + 2;
    D_001A94D0 = D_001A94D4[D_001A94C8].f0;
    l_1C = func_0001FFF1();
    l_18 = D_0017A288[D_00195BF4 % 518400 / 43200];
    if ((D_00195E2A[l_1C] & 127) == 5 || l_18 == 0 && (l_1C == 1 || l_1C == 3 || l_1C == 5)) {
        D_001A94CC++;
        D_001A94D0++;
        return;
    }
    if ((D_00195E2A[func_0001FFF1()] & 127) != 4) return;
    D_001A94CC += 2;
}
