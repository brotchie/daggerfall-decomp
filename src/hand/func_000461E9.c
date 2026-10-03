/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000461E9 */
struct obj {
    unsigned char type;         /* 0x00 */
    char pad1[6];
    int x;                      /* 0x07 */
    int y;                      /* 0x0b */
    int z;                      /* 0x0f */
    char pad13[8];
    short f27;                  /* 0x1b */
    short f29;                  /* 0x1d */
    unsigned int f31;           /* 0x1f */
    char pad23[32];
    char *f67;                  /* 0x43 */
};
struct flag4 {
    unsigned char a;
    unsigned char b;
    unsigned short id:10;
    unsigned short f10:1;
    unsigned short kind:3;
};
extern struct obj *D_00195AC4;
extern struct flag4 D_0019676C[];
extern unsigned char D_00196AB3;
extern struct obj *D_00199720;
extern struct obj *func_000461A3(void);
extern void func_0008DA91(struct obj *);
extern struct obj *func_0008DCE3(char *, int, int);
extern struct obj *func_0008E925(struct obj *, int);
extern int func_0008EB88(int);
extern struct obj *func_0009A0A0(struct obj *, int, int);

void func_000461E9(void)
{
    struct obj *p;
    int i;

    for (i = 0; i < D_00196AB3; i++) {
        if (D_0019676C[i].a == 0 && D_0019676C[i].b == 0)
            if (D_0019676C[i].kind == 1 && D_0019676C[i].id == 9) {
                D_00199720 = func_0009A0A0(D_00195AC4, 8, 0);
                D_00199720 = func_0008E925(D_00195AC4, D_00199720->f31);
                p = func_000461A3();
                if (p != 0)
                    func_0008DA91(p);
                p = func_0008DCE3(D_00199720->f67, 0, 62);
                p->type = 6;
                p->f29 = 703;
                p->f27 = 0;
                p->f31 = func_0008EB88(D_00195AC4->f31 >> 16);
                p->x = D_00195AC4->x + 664;
                p->y = D_00195AC4->y - 1281;
                p->z = D_00195AC4->z + 2035;
            }
    }
}
