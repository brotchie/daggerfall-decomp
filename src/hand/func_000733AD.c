/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000733AD */
struct mobile { char pad0[553]; unsigned char group; };   /* group at 0x229 */
struct thing {
    unsigned char type;
    char pad1[2];
    short angle;                /* 3 */
    char pad5[2];
    int x;                      /* 7 */
    int y;                      /* 11 */
    int z;                      /* 15 */
    char pad13[2];
    unsigned short flags;       /* 21 */
    unsigned short f23;         /* 23 */
    char pad19[6];
    unsigned int f31;           /* 31 */
    char pad23[36];
    struct mobile mob;          /* 71 */
};
struct item { char pad0[15]; unsigned char flags; };
struct pick { int flags; struct thing *obj; int f8; int fc; int f10; };
struct player { char pad0[253]; short f253; char pad0ff[188]; struct thing *f443[1]; };
struct w2 { unsigned short f0; unsigned short f2; };
extern unsigned char D_001789FA;
extern struct thing *D_00190504[];
extern int D_001959BC;
extern struct thing *D_00195AA0;
extern struct thing *D_00195AA4;
extern int D_00195ABC;
extern int D_00195B14;
extern struct player *D_00195BE0;
extern struct w2 *D_00195DC0;
extern unsigned char D_0019626A;
extern char D_0019627E;
extern char D_001962B2;
extern struct thing *D_00199670[];
extern int D_001996F4;
extern void func_00013F4B(int, int, struct pick *);
extern int func_00023C72(struct thing *, struct thing *);
extern void func_000287BD(struct thing *, struct item *);
extern void func_0002D62F(struct thing *, struct thing *, int);
extern void func_0002F490(struct thing *, int, int);
extern int func_0002F9EE(struct mobile *, int);
extern void func_0002FBCC(void);
extern void func_0003D01C(int, int);
extern int func_00040BCD(void);
extern void func_00040C87(int);
extern void func_00041409(struct thing *);
extern int func_00062EF7(int, int, int *);
extern void func_00063DDC(struct thing *);
extern void func_00064589(struct thing *, int);
extern int func_00069938(int, struct thing *, int);
extern void func_0007425E(struct thing *, int);
extern int func_0007D6AE(int, int);
extern struct item *func_0007DF35(struct thing *);
extern void func_00087281(struct item *);
extern int func_00099922(struct thing *, int);
extern int func_000C7FD9(int, int, int, int);
extern int func_000C7FF4(int, int);
extern int func_000C808D(int, int, int, int);

void func_000733AD(struct thing *a1)
{
    struct pick st;
    int grp;
    int dist;
    int res;
    int i;
    int r;
    struct mobile *m;
    struct mobile *om;
    struct thing *other;
    struct thing *obj;
    int count;
    struct item *item;

    m = &a1->mob;
    grp = m->group;
    D_00190504[D_00195B14++] = D_00195AA0;
    for (count = i = 0; i < D_00195B14; i++) {
        other = D_00190504[i];
        om = &other->mob;
        if (om->group == grp)
            continue;
        if (other == a1)
            continue;
        dist = func_000C7FF4(a1->y - other->y, func_000C7FD9(a1->x, a1->z, other->x, other->z));
        if (dist > 90)
            continue;
        if (dist > 10) {
            dist = func_000C808D(a1->x, a1->z, other->x, other->z);
            D_00195ABC = dist;
            if (a1 == D_00195AA0)
                res = func_00062EF7(a1->angle + D_001959BC & 2047, dist, &dist);
            else
                res = func_00062EF7(a1->angle, dist, &dist);
        } else
            res = 1;
        if (res < 200 && func_00023C72(a1, other)) {
            if (a1 == D_00195AA0) {
                func_00063DDC(other);
                func_0002D62F(a1, other, D_0019626A * 2 + 19);
                count++;
            } else
                func_0002D62F(a1, other, 19);
        }
        if (D_001962B2 != 0) {
            D_001962B2 = 0;
            return;
        }
    }
    if (count == 0 && a1 == D_00195AA0)
        func_00069938(func_0002F9EE(D_00195BE0->f443[D_0019626A] != 0 ? &D_00195BE0->f443[D_0019626A]->mob : 0, -1), D_00195AA4, 110);
    if (a1 == D_00195AA0 && D_001789FA != 3) {
        for (i = 0; i < D_001996F4; i++) {
            if (D_00199670[i] == 0)
                continue;
            other = D_00199670[i];
            dist = func_000C7FF4(a1->y - other->y, func_000C7FD9(a1->x, a1->z, other->x, other->z));
            if (dist > 90)
                continue;
            dist = func_000C808D(a1->x, a1->z, other->x, other->z);
            D_00195ABC = dist;
            res = func_00062EF7(a1->angle + D_001959BC & 2047, dist, &dist);
            if (res < 200) {
                func_0002F490(D_00199670[i], 0, -1);
                func_00041409(D_00199670[i]);
                count++;
            }
        }
    }
    if (a1 != D_00195AA0 || count != 0)
        return;
    func_00013F4B(160, 100, &st);
    if ((st.flags & 1) == 0)
        return;
    obj = st.obj;
    func_00064589(obj, 5);
    if (obj->type != 32 && obj->type != 43)
        return;
    if (obj->type == 43) {
        r = (D_00195DC0->f2 >> 7) % 100;
        if (r != 74)
            return;
    }
    if (obj->type != 43 && func_000C7FD9(obj->x, obj->z, D_00195AA4->x, D_00195AA4->z) > 100)
        return;
    if (obj->type != 43 && (obj->f23 == 0 || (obj->flags & 320) != 0)) {
        func_0007425E(obj, 0);
        return;
    }
    if (obj->type != 43) {
        func_0003D01C(16, 1);
        func_00063DDC(0);
        if (obj->f23 <= 19 && func_0007D6AE(1, 100) <= 20 - obj->f23 && func_00099922(obj, 0))
            obj->flags |= 320;
        if (obj->f31 >> 16 == 50027 || obj->f31 >> 16 == 50029 || obj->f31 >> 16 == 50033)
            func_0002FBCC();
    } else if (func_0007D6AE(1, 100) < 10) {
        item = func_0007DF35(obj);
        if (item != 0)
            item->flags |= 16;
        func_000287BD(obj, item);
        func_00087281(item);
    } else if (func_0007D6AE(1, 100) > D_00195BE0->f253 && (func_00040BCD() & 1) == 0) {
        D_0019627E = 0;
        func_00040C87(1);
    }
    func_00069938(9, obj, 100);
}
