/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0004BBD8 */
struct obj { char pad[38]; unsigned char f38; };
extern struct obj *D_00199774;
extern struct obj *D_00199778;
extern short D_001997AC;
extern void func_0002B26B(int);
extern int func_0004BB64(unsigned char);

void func_0004BBD8(short a1, struct obj *a2, struct obj *a3)
{
    int l;

    l = func_0004BB64(a2 ? a2->f38 : a3->f38);
    if (l == 0)
        return;
    D_001997AC = a1;
    D_00199778 = a2;
    D_00199774 = a3;
    func_0002B26B(l);
}
