/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008B65C */
extern char D_00176D98[];
extern char D_00176DA2[];
extern char D_001A9A60[];
extern char D_001A9A7E[];
extern short D_001A9A9C;
extern void func_0008BCE9(short, short);
extern int func_000A1054();

char *func_0008B65C(unsigned char a1, unsigned char a2)
{
    short i;

    D_001A9A60[0] = 0;
    switch (a1) {
    case 1:
    case 8:
    case 9:
    case 10:
        break;
    case 2:
        i = 0;
        func_0008BCE9(D_001A9A9C, i);
        func_000A1054(D_001A9A60, D_001A9A7E, D_00176D98, 126, 30);
        func_0008BCE9(D_001A9A9C, i + 1);
        func_000A1054(D_001A9A60, D_001A9A7E, D_00176D98, 128, 30);
        func_000A1054(D_001A9A60, D_00176DA2, D_00176D98, 129, 30);
        break;
    default:
        i = 4;
        func_0008BCE9(D_001A9A9C, i);
        func_000A1054(D_001A9A60, D_001A9A7E, D_00176D98, 134, 30);
        func_0008BCE9(D_001A9A9C, i + 1);
        func_000A1054(D_001A9A60, D_001A9A7E, D_00176D98, 136, 30);
        break;
    }
    return D_001A9A60;
}
