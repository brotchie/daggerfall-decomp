/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004E02D */
#pragma pack(1)
struct ev {
    unsigned char type;
    short a, b, c, d;
    char e;
    char pad;
};
#pragma pack()
extern char D_00174FAC[];
extern char D_001851FF;
extern unsigned char D_001940D5;
extern struct ev *D_001997D4;
extern int D_001997D8;
extern short D_001997E4;
extern void func_0003F09F(int, int);
extern int func_000A1023();

void func_0004E02D(short a1, short a2, short a3, short a4)
{
    struct ev *l_1C;

    if ((unsigned)D_001997E4 < 11) {
        func_0003F09F(1700, 1);
        return;
    }
    func_000A1023(D_001997D8, D_001997D4, 3640, D_00174FAC, 399, 4);
    D_001940D5 |= 16;
    l_1C = D_001997D4;
    while (l_1C->type != 0) {
        if (l_1C->type == 1)
            l_1C = (struct ev *)((char *)l_1C + 91);
        else
            l_1C++;
    }
    l_1C->type = 2;
    l_1C->a = a1;
    l_1C->b = a2;
    l_1C->c = a3;
    l_1C->d = a4;
    l_1C->e = D_001851FF;
    l_1C++;
    l_1C->type = 0;
}
