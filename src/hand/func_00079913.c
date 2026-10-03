/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00079913 */
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
struct skill { short v; short f2; short f4; };
struct player {
    char pad0[42];
    short f42;                  /* 0x2a */
    char pad2c[113];
    struct skill skills[1];     /* 0x9d */
};
struct mobile {
    char pad0[65];
    struct bits8 f65;           /* 0x41 */
    char pad42;
    unsigned char f67;          /* 0x43 */
};
struct thing { unsigned char type; char pad[70]; struct mobile mob; };
extern unsigned char D_00187A62[];
extern struct bits8 D_001940D6;
extern int D_00195AA0;
extern struct player *D_00195BE0;
extern void func_0003D01C(int, int);
extern int func_0007D6AE(int, int);
extern int func_0008B29A(int, unsigned char, unsigned char);

void func_00079913(struct thing *a1)
{
    struct mobile *m;
    int chance;
    int sk;

    m = &a1->mob;
    chance = D_001940D6.b6 ? -25 : 10;
    if (m->f67 >= 43)
        chance += D_00195BE0->f42 / 5 + D_00195BE0->skills[1].v / 10;
    else {
        sk = D_00187A62[m->f67];
        if (sk != 0) {
            chance += D_00195BE0->skills[sk].v;
            func_0003D01C(sk, 1);
        }
        chance += func_0008B29A(D_00195AA0, 44, 255);
        chance += D_00195BE0->f42 / 5;
    }
    if (func_0007D6AE(1, 200) <= chance)
        m->f65.b7 = 1;
}
