/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004DA4A */
extern short D_0012AC04;
extern short D_0012AC06;

#pragma pack(1)
struct rect { short x0; short y0; short x1; short y1; };
#pragma pack()

extern void func_0004EA3C(int, struct rect *);

int func_0004DA4A(int a1)
{
    int l_20;
    struct rect r;

    func_0004EA3C(a1, &r);
    return D_0012AC04 > r.x0 && D_0012AC04 < r.x1 && D_0012AC06 > r.y0 && D_0012AC06 < r.y1;
}
