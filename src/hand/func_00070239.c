/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00070239 */
extern unsigned char D_00190D20;
extern short D_00190DDC;
extern char *D_00195AA0;
extern char *D_00195AF4;
extern void func_00070191(void);
extern void func_0008E3F7(int, void (*)(void));

char *func_00070239(short a1)
{
    D_00195AF4 = 0;
    D_00190DDC = a1;
    D_00190D20 = 255;
    func_0008E3F7(*(int *)(D_00195AA0 + 63), func_00070191);
    if (D_00195AF4 == 0)
        return 0;
    return D_00195AF4 + 71;
}
