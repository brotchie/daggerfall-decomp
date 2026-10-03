/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CA85 */
extern unsigned char D_0012B504;
extern unsigned char D_0012B508;
extern void func_0005A60C(char *, short, short);

void func_0007CA85(char *a1, short a2, short a3, short a4, int a5)
{
    short l_10;
    short l_C;

    l_10 = D_0012B508;
    l_C = D_0012B504;
    D_0012B508 = a4;
    D_0012B504 = a5;
    func_0005A60C(a1, a2, a3);
    D_0012B508 = l_10;
    D_0012B504 = l_C;
}
