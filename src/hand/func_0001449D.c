/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0001449D */
#pragma pack(1)
struct S { char c0; short s1; short s3; short s5; int x7; int x11; int x15; };
struct P { int x; int y; int z; };
extern struct S *D_00195AA4;
extern struct P D_00120288;
extern void func_0008DEB4(struct S *, int, int, int, int, int, int);

void func_0001449D(void)
{
    int l_1C;
    int l_18;

    l_1C = D_00195AA4->x7 + (D_00120288.x >> 9);
    l_18 = D_00195AA4->x15 + (D_00120288.z >> 9);
    func_0008DEB4(D_00195AA4, l_1C, D_00195AA4->x11, l_18, D_00195AA4->s1, D_00195AA4->s3, D_00195AA4->s5);
}
