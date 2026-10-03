/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000191DA */
extern int D_0019671C;
extern int D_0019672C;
extern int func_00019234(int, short, short);

int func_000191DA(short a1, short a2)
{
    int r;

    D_0019671C = 0;
    r = func_00019234(D_0019672C, a1, a2);
    return r != 0 ? r : D_0019671C;
}
