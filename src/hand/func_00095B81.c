/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00095B81 */
struct sub { char pad[32]; unsigned short state; };
struct obj {
    unsigned char type;
    char pad1[6];
    int x, y, z;
    char pad19[2];
    unsigned short flags;
    unsigned short owner;
    char pad25[42];
    int parent;
    struct sub sub;
};
extern int D_001940D6;
extern int D_001940D8;
extern int D_001959E8;
extern struct obj *D_00195AA4;
extern char *D_00195BE0;
extern int D_00195D54;
extern struct obj *D_001AA548[];
extern int D_001AA564;
extern short D_001AA588;
extern void func_000934F6(struct obj *, short, int);
extern int func_000C7FD9(int, int, int, int);
extern int func_000C7FF4(int, int);
extern int func_000CE44C(char *, struct obj *, int);

void func_00095B81(struct obj *a1, int a2)
{
    struct sub *s;

    if (a1->type != 2 && a1->type != 54 || (a1->flags & 2))
        return;
    if (a1->flags & 0x200)
        return;
    if ((D_001940D6 & 4) && a1->owner != D_00195D54)
        return;
    if ((!(D_001940D6 & 4) && a1->parent != D_001959E8 ? 1 : 0) && func_000C7FF4(a1->z - D_00195AA4->z, func_000C7FD9(a1->x, a1->y, D_00195AA4->x, D_00195AA4->y)) > 160)
        return;
    if (D_001AA588 >= D_001AA564 && D_001AA588 < D_001AA564 + 4) {
        if (!(D_001940D8 & 4) && func_000CE44C(D_00195BE0 + 367, a1, 27) != 0) {
            s = &a1->sub;
            if (s->state != 1)
                return;
        }
        D_001AA548[D_001AA588 - D_001AA564] = a1;
        func_000934F6(a1, D_001AA588 - D_001AA564, a2);
    }
    D_001AA588++;
}
