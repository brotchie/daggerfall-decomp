/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000820AF */
#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct obj {
    char pad0;
    short a1;                   /* 1 */
    short a3;                   /* 3 */
    short a5;                   /* 5 */
    int x;                      /* 7 */
    int y;                      /* 11 */
    int z;                      /* 15 */
};
struct pos {
    int x;
    int y;
    int z;
    int a;
    int b;
    int c;
    char *name;
};
#pragma pack()
extern char D_00187B6E[];
extern char D_00187BB8[];
extern char D_00187C12[];
extern char D_00187C3C[];
extern struct pos D_00187C86;
extern struct bits8 D_001940DB;
extern struct obj *D_00195AA4;
extern char *D_00195BE0;
extern short D_00196D64;
extern int D_001A5A60;
extern int D_001A5A64;
extern int D_001A5B08;
extern int func_000234EB(struct obj *, int, struct pos *, int);
extern int func_000CE6E2();

int func_000820AF(int a1)
{
    int dx;
    int dz;
    int r;

    func_000CE6E2((D_00195AA4->a3 + D_001A5B08) & 2047, a1 << 5, &dx, &dz);
    dx += D_00195AA4->x << 5;
    dz += D_00195AA4->z << 5;
    dx += D_001A5A64;
    dz += D_001A5A60;
    D_001A5A64 = dx & 31;
    D_001A5A60 = dz & 31;
    D_00187C86.x = dx / 32;
    D_00187C86.y = D_00195AA4->y;
    D_00187C86.z = dz / 32;
    D_00187C86.a = D_00195AA4->a1;
    D_00187C86.b = D_00195AA4->a3;
    D_00187C86.c = D_00195AA4->a5;
    if (D_00187C86.x < 16384 || D_00187C86.x > 32751616 || D_00187C86.z < 16384 || D_00187C86.z > 16367616) {
        D_00196D64 |= 8;
        return D_00196D64;
    }
    D_00187C86.name = D_001940DB.b2 ? D_00187C12 : D_00187B6E;
    D_00187C86.name = ((unsigned short)*(short *)(D_00195BE0 + 64) & 1536) != 0 ? D_00187C3C : D_00187C86.name;
    if (D_001940DB.b5)
        D_00187C86.name = D_00187BB8;
    if (a1 != 0)
        D_00196D64 |= 4;
    else
        D_00196D64 &= ~4;
    r = func_000234EB(D_00195AA4, 0, &D_00187C86, 1);
    return r;
}
