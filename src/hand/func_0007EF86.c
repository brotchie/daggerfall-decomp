/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007EF86 */
#include <i86.h>
extern char D_00176A10[];
extern char D_00195E6C[];
extern char D_001A5A1E[];
extern int D_001A5A2E;
extern int D_001A5A32;
extern int D_001A5A3A;
extern short D_001A5A40;
extern short D_001A5A50;
extern short D_001A5A54;
extern int mc_memset();
extern int int386x();
extern int func_000A2EC5(void __far *, void __far *, unsigned, char *, int, int);

int func_0007EF86(int a1, int a2)
{
    union REGS r;
    struct SREGS s;

    mc_memset(&s, 0, 12, D_00176A10, 1078, 4);
    mc_memset(&r, 0, 28, D_00176A10, 1079, 4);
    mc_memset(D_001A5A1E, 0, 50, D_00176A10, 1080, 4);
    D_001A5A3A = a1;
    D_001A5A2E = a2;
    D_001A5A40 = D_001A5A50;
    D_001A5A32 = 0;
    r.w.ax = 0x300;
    r.w.bx = 0x33;
    s.es = FP_SEG(D_001A5A1E);
    r.x.edi = (unsigned)D_001A5A1E;
    int386x(0x31, &r, &r, &s);
    func_000A2EC5(D_00195E6C, MK_FP((unsigned short)D_001A5A54, 0), 14, D_00176A10, 1094, 4);
    return D_001A5A3A;
}
