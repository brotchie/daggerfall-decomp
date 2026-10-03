/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007EED8 */
#include <i86.h>
extern char D_00176A10[];
extern char D_00196281[];
extern int func_0007EF86(int, int);
extern int func_000A0040();

void func_0007EED8(void)
{
    union REGS r;
    struct SREGS s;
    int a;
    int b;

    if (*D_00196281 == 0) return;
    func_000A0040(&s, 0, 12, D_00176A10, 1044, 4);
    func_0007EF86(0x5301, 0);
}
