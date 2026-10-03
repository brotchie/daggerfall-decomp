/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008227A */
struct flags {
    unsigned char b0:1;
    unsigned char b1:1;
    unsigned char b2:1;
    unsigned char b3:1;
    unsigned char b4:1;
    unsigned char b5:1;
};
#pragma pack(1)
struct place {
    int x, y, z;
    int a, b, c;
    char *name;
};
#pragma pack()
extern char D_00187B6E[];
extern char D_00187BB8[];
extern char D_00187C12[];
extern char D_00187C3C[];
extern struct place D_00187C86;
extern struct flags D_001940DB;
extern char *D_00195AA4;
extern char *D_00195BE0;
extern unsigned char D_00196D64;
extern int func_000234EB(char *, int, struct place *, int);

int func_0008227A(int a1)
{
    int l_20;
    int l_1C;

    D_00187C86.x = *(int *)(D_00195AA4 + 7);
    D_00187C86.y = *(int *)(D_00195AA4 + 11) + a1;
    D_00187C86.z = *(int *)(D_00195AA4 + 15);
    D_00187C86.a = *(short *)(D_00195AA4 + 1);
    D_00187C86.b = *(short *)(D_00195AA4 + 3);
    D_00187C86.c = *(short *)(D_00195AA4 + 5);
    D_00187C86.name = D_001940DB.b2 ? D_00187C12 : D_00187B6E;
    D_00187C86.name = (*(unsigned short *)(D_00195BE0 + 64) & 1536) ? D_00187C3C : D_00187C86.name;
    if (D_001940DB.b5)
        D_00187C86.name = D_00187BB8;
    D_00196D64 &= 251;
    l_20 = func_000234EB(D_00195AA4, 0, &D_00187C86, 1);
    return l_20;
}
