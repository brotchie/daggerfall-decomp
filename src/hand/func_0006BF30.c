/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006BF30 */
struct acct { int f0; int f4; int f8; };
extern struct acct *D_001A41EC;
extern void func_0003F09F(int, int);
extern int func_0006CA5C(void);
extern void func_0007F1E3(int);
extern int func_0007F558(void);

void func_0006BF30(void)
{
    int n;

    if (D_001A41EC->f4 == 0)
        return;
    n = func_0006CA5C();
    if (n < 1)
        return;
    if (D_001A41EC->f0 + func_0007F558() < n) {
        func_0003F09F(454, 1);
        return;
    }
    if (n > D_001A41EC->f4) {
        n = D_001A41EC->f4;
        func_0003F09F(294, 1);
    }
    D_001A41EC->f0 -= n;
    D_001A41EC->f4 -= n;
    if (D_001A41EC->f0 < 0) {
        func_0007F1E3(-D_001A41EC->f0);
        D_001A41EC->f0 = 0;
    }
    if (D_001A41EC->f4 == 0)
        D_001A41EC->f8 = 0;
}
