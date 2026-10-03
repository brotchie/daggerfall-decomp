/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000914C8 */
extern short D_0012DA44;
extern char D_0017CC27[];
extern char D_0018801C[];
extern char D_0018801E[];
extern char D_001903A4[];
extern short D_00190DE4[];
extern short D_00190DEA[];
extern char *D_00195B60;
extern char D_00195BE0[];
extern char *D_00195BEC;
extern void func_0007CA1F(char *, short, short, int, unsigned char);
extern void func_0007CA85(char *, short, short, int, unsigned char);
extern char *func_000A0DD9(int, char *, int);
extern int func_00144F68();

#pragma pack(1)
struct E { short s; char pad[4]; };
struct B { char pad[0x9d]; struct E e[1]; };

void func_000914C8(void)
{
    int i;
    int k;

    for (i = 0; i < 3; i++) {
        func_00144F68(203, D_00190DE4[i], *(unsigned short *)(D_00195B60 + 4), *(unsigned short *)(D_00195B60 + 6), D_00195B60 + 12);
        func_0007CA85(func_000A0DD9(D_00190DEA[i], D_001903A4, 10), 221, D_00190DE4[i] + 8 - D_0012DA44 + 1, 145, 141);
    }
    for (i = 0; i < 12; i++) {
        k = *(unsigned char *)(D_00195BEC + i + 16);
        func_0007CA1F(*(char **)(D_0017CC27 + (k << 2)), *(short *)(D_0018801C + ((i + 2) * 12)) + 2, *(short *)(D_0018801E + ((i + 2) * 12)) + 1, 145, 141);
        func_0007CA85(func_000A0DD9((*(struct B **)D_00195BE0)->e[k].s, D_001903A4, 10), 192, *(short *)(D_0018801E + ((i + 2) * 12)) + 1, 145, 141);
    }
}
