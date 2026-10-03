/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0002682B */
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct item { char pad[7]; char name[14]; unsigned short flags; };
struct desc { char pad[12]; char *str; char pad16[4]; };
extern char D_00170788[];
extern char D_00187B6E[];
extern struct bits8 D_001940D7;
extern struct bits8 D_001940DB;
extern int D_00195C74;
extern unsigned char D_00196277;
extern short D_00196D64;
extern int func_0002257C(struct item *, int, char *, int);
extern void func_000A0040(void *, int, int, char *, int, int);
extern void func_000A1023(char *, char *, int, char *, int, int);

void func_0002682B(struct item *a1)
{
    int saved2;
    int saved1;
    struct desc d;

    saved1 = D_00196277;
    saved2 = D_00195C74;
    if ((a1->flags & 16) == 0)
        return;
    {
        char name[12];

        D_001940D7.b5 = 1;
        D_001940D7.b7 = 1;
        func_000A1023(name, a1->name, 12, D_00170788, 477, 4);
        func_000A0040(&d, 0, 12, D_00170788, 478, 4);
        d.str = D_00187B6E;
        D_001940DB.b3 = 1;
        func_0002257C(a1, 0, name, 0);
        D_001940DB.b3 = 0;
        D_00196277 = saved1;
        D_00195C74 = saved2;
        if ((D_00196D64 & 1) == 0)
            return;
        a1->flags &= ~16;
    }
}
