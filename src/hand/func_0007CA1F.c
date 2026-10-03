/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007CA1F */
extern unsigned char D_0012B504;
extern unsigned char D_0012B508;
extern void func_0005A577(char *, short, short);

void func_0007CA1F(char *a1, short a2, int a3, int a4, int a5)
{
    short l1;
    short l2;

    l1 = D_0012B508;
    l2 = D_0012B504;
    D_0012B508 = a4;
    D_0012B504 = a5;
    func_0005A577(a1, a2, a3);
    D_0012B508 = l1;
    D_0012B504 = l2;
}
