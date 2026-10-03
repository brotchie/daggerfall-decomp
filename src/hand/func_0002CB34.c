/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002CB34 */
struct S { char pad[2]; short flags; };
struct T { char pad[28]; short id; };
extern void func_0002C5ED(int, struct S *, int);
extern struct S *func_00030A23(int, int, int);

void func_0002CB34(int a1, struct T *a2, short a3)
{
    struct S *p;

    p = func_00030A23(a1, 6, a2->id);
    if (a3 & 64) {
        if ((p->flags & 64) == 0) {
            func_0002C5ED(a1, p, 1);
            p->flags |= 64;
        }
    } else {
        p->flags &= ~64;
    }
}
