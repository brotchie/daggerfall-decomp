/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006FDBE */
struct rec89 { char pad[47]; char name[42]; };
extern char *D_00195A9C;
extern int D_00195AA0;
extern struct rec89 *D_00195B04;
extern int D_00195BE0;
extern char *D_00195C44;
extern int D_00195D98;
extern char D_001A9AB8[];
extern int func_0003A0C0(struct rec89 *, int);
extern int func_0007D6AE(int, int);
extern void func_0008CA9A(char *, short, short, int, short, short, short, short, short, short, short, short, short, short, short, short, short, unsigned char, unsigned char, unsigned char, unsigned char, unsigned char);
extern void func_0008CCC8(char *, char *, int);
extern int func_0008E6C5(int, int, int);

int func_0006FDBE(void)
{
    int l_30;
    int l_2C;
    int l_28;
    int l_24;
    int l_20;
    int l_1C;

    l_24 = 0;
    if (func_0008E6C5(D_00195AA0, 27, 0) == 0)
        return 0;
    func_0008CA9A(D_001A9AB8, 27, 30, 111, 131, 144, 29, 8, 15, 144, 150, 8, 15, 144, 45, 9, 104, 146, 146, 244, 114, 0);
    l_28 = *(unsigned char *)(D_00195A9C + 25) * 5 + 30;
    while (l_24 == 0) {
        for (l_2C = 0; l_2C < D_00195D98; l_2C++) {
            if (D_00195B04[l_2C].name[0] == 0) continue;
            if ((unsigned char)D_00195B04[l_2C].name[0] == 33) continue;
            l_20 = func_0003A0C0(&D_00195B04[l_2C], D_00195BE0);
            while (l_20 > 95)
                l_20 >>= 1;
            l_1C = l_28 - l_20;
            if (l_1C < 5)
                l_1C = 5;
            if (func_0007D6AE(1, 50) < l_1C) {
                func_0008CCC8(D_001A9AB8, D_00195B04[l_2C].name, 0);
                D_00195C44[l_24 + 20000] = l_2C;
                l_24++;
            }
        }
    }
    return 1;
}
