/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0009A7B8 */
#pragma pack(1)
struct obj {
    unsigned char type;
    char pad1[6];
    int x;                      /* 7 */
    int y;                      /* 11 */
    int z;                      /* 15 */
    char pad13[2];
    short flags;                /* 21 */
    char pad17[40];
    int f63;                    /* 63 */
    struct obj *next;           /* 67 */
};
#pragma pack()
extern char D_00177358[];
extern unsigned char D_001940D5;
extern struct obj *D_00195AA4;
extern struct obj *func_0009A5EF(int, int);
extern int func_0009DEAC(int);
extern void func_000A1023(void *, void *, int, char *, int, int);

void func_0009A7B8(void)
{
    struct obj a;
    struct obj b;
    struct obj *q;
    struct obj *p;

    p = D_00195AA4->next;
    while (p != 0 && ((unsigned short)p->flags & 1) == 0)
        p = p->next;
    if (p == 0 || p->type != 43) return;
    q = func_0009A5EF(p->f63, 19);
    if (q == 0) return;
    func_000A1023(&a, q, 71, D_00177358, 521, 4);
    q = func_0009A5EF(p->f63, 20);
    if (q == 0) return;
    func_000A1023(&b, q, 71, D_00177358, 525, 4);
    if (func_0009DEAC(D_00195AA4->y - a.y) > func_0009DEAC(D_00195AA4->y - b.y)) {
        D_00195AA4->x = a.x;
        D_00195AA4->y = a.y;
        D_00195AA4->z = a.z;
    } else {
        D_00195AA4->x = b.x;
        D_00195AA4->y = b.y;
        D_00195AA4->z = b.z;
    }
    D_001940D5 |= 2;
}
