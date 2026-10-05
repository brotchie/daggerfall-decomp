/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004DA4A */
extern short mouse_x;
extern short mouse_y;

#pragma pack(1)
struct rect { short x0; short y0; short x1; short y1; };
#pragma pack()

extern void func_0004EA3C(int, struct rect *);

int func_0004DA4A(int a1)
{
    int l_20;
    struct rect r;

    func_0004EA3C(a1, &r);
    return mouse_x > r.x0 && mouse_x < r.x1 && mouse_y > r.y0 && mouse_y < r.y1;
}
