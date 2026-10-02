/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00032A05 */
#pragma pack(1)
struct R6 { unsigned short w0; unsigned short flags; unsigned short w4; };
struct Sub { char pad[0x12]; short w12; };
struct O {
    char type;              /* 0x00 */
    char pad1[6];
    int i7;                 /* 0x07 */
    int ib;                 /* 0x0b */
    int i_f;                /* 0x0f */
    char pad13[2];
    short w15;              /* 0x15 */
    short w17;              /* 0x17 */
    short w19;              /* 0x19 */
    short w1b;              /* 0x1b */
    short w1d;              /* 0x1d */
    int pos;                /* 0x1f */
    char c23;               /* 0x23 */
    char pad24[2];
    char c26;               /* 0x26 */
    char pad27[4];
    int pos2;               /* 0x2b */
    char pad2f[0x18];
    struct Sub sub;         /* 0x47 */
};
struct L { char pad[0x22]; char c22; char pad23[8]; char *recs; };
struct T {
    char pad0[3];
    signed char cnt;        /* 0x03 */
    unsigned short w4;      /* 0x04 */
    short s6;               /* 0x06 */
    short mode;             /* 0x08 */
    char pad0a[6];
    struct O *obj;          /* 0x10 */
};
extern char D_00170A64[];
extern char *D_001959A8;
extern struct O *D_00195AC4;
extern struct L *D_00195BDC;
extern unsigned *D_00195C44;
extern unsigned char D_00196268;
extern int D_00196A8C;
extern struct R6 *D_00196A90;
extern short D_001970C8;
extern int D_001970CC;
extern struct R6 *D_001970D0;
extern struct O *D_001970D4;
extern struct L *D_001970D8;
extern char *D_00199764;
extern char *func_000191DA(short, short);
extern int func_000337AD(struct R6 *, struct T *, char *);
extern int func_000345A5(int);
extern void func_00086397(short *);
extern void func_00087F76(short *, unsigned short, short, int);
extern struct O *func_0008DCE3(char *, int, int);
extern int func_0009DC25();
extern int func_000A0AD9();
extern int func_000A1023();

int func_00032A05(struct T *a1)
{
    struct R6 *base;
    struct R6 *ptr;
    struct O *l40;
    struct O *obj;
    struct L *l38;
    unsigned *list;
    struct Sub *p;
    int n;
    int count;
    int i;
    int tries;
    int unused;  /* [ebp-0x1c]: declared, never used */

    tries = 0;
retry:
    if (a1->cnt == 0) {
        a1->cnt = 10;
        obj = func_0008DCE3(D_001959A8, 0, 26);
        obj->type = 40;
        a1->obj = obj;
        obj->pos = (a1->w4 << 16) | (unsigned short)(a1->s6 & 0xffff);
        obj->pos2 = obj->pos;
        obj->w15 = 0x202;
        obj->w17 = *(short *)D_00199764;
        obj->c26 = *D_00199764;
        return 1;
    }
    if (a1->cnt > 0)
        a1->cnt--;
    if (a1->cnt != 0) {
        func_00086397(&D_001970C8);
        func_00087F76(&D_001970C8, a1->w4, a1->s6, a1->cnt);
        base = D_001970D0;
        n = D_001970CC;
        l40 = D_001970D4;
        l38 = D_001970D8;
    } else {
        base = D_00196A90;
        n = D_00196A8C;
        l40 = D_00195AC4;
        l38 = D_00195BDC;
    }
    list = D_00195C44;
    count = 0;
    if (a1->w4 == 0) {
        for (count = i = 0, ptr = base; i < n; i++, ptr++) {
            if (func_000337AD(ptr, a1, l38->recs + ptr->w0 * 26))
                list[count++] = (ptr->w0 << 16) + ptr->w4;
        }
    } else {
        for (i = 0, ptr = base; i < n; i++, ptr++) {
            switch (a1->mode) {
            case -1:
                list[count++] = ptr->w4;
                break;
            case 0:
                if ((int)(unsigned short)(ptr->flags & 0x4000))
                    list[count++] = ptr->w4;
                break;
            case 1:
                if ((int)(unsigned short)(ptr->flags & 0x1000))
                    list[count++] = ptr->w4;
                break;
            }
        }
    }
    i = 0;
    if (a1->cnt > -1)
        a1->cnt++;
    if (count == 0 && ++tries < 100)
        goto retry;
    a1->cnt--;
    if (count == 0)
        return 0;
    i = func_0009DC25() % count;
    if (func_000345A5((l40->pos & 0xffff0000) + (list[i] & 0xffff)))
        goto retry;
    obj = func_0008DCE3(D_001959A8, 0, 58);
    obj->type = 40;
    obj->w15 = 0x202;
    obj->w1b = D_001970C8;
    obj->w17 = *(short *)D_00199764;
    obj->pos = (l40->pos & 0xffff0000) + (list[i] & 0xffff);
    obj->pos2 = obj->pos;
    obj->c26 = *D_00199764;
    obj->c23 = D_001970D8->c22;
    obj->w19 = (unsigned short)D_00196268;
    a1->obj = obj;
    obj->i7 = l40->i7;
    obj->ib = l40->ib;
    obj->i_f = l40->i_f;
    if (a1->cnt != 1)
        obj->w1d = list[i] >> 16;
    else
        obj->w1d = 0xffff;
    p = &obj->sub;
    if (p->w12 == 0)
        p->w12 = *(short *)(func_000191DA(D_00196268, 15) + 33);
    if (a1->w4 != 1)
        func_000A1023(p, l38->recs + (list[i] >> 16) * 26, 26, D_00170A64, 483, 4);
    func_000A0AD9((char *)p + 26, l38, 4, D_00170A64, 485);
    func_00086397(&D_001970C8);
    return 1;
}
