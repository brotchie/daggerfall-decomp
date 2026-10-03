/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00039224 */
struct pair { unsigned char a; unsigned char b; };
extern struct pair *D_00178A0A;
extern short D_0017A8E5[];

int func_00039224(short a1)
{
    short s;
    short t;

    s = D_0017A8E5[D_00178A0A[a1].a];
    t = D_00178A0A[a1].b;
    if (t != 255 && D_00178A0A[a1].a != 29)
        s += t;
    return s;
}
