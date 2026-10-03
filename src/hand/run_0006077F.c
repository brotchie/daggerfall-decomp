/* matched by the real Watcom C32 10.0a (-d2): a run of equip.c from 0x00060430 to 0x0006077F, kept together for its switch table's alignment */
struct pair {
    unsigned char a;
    unsigned char b;
};
struct tmpl {
    char name[32];              /* 0x00 */
    unsigned char used;         /* 0x20 */
    unsigned char f33;          /* 0x21 */
    unsigned char f34;          /* 0x22 */
    struct pair pairs[10];      /* 0x23 */
    short f55;                  /* 0x37 */
    int f57;                    /* 0x39 */
    unsigned char f61;          /* 0x3d */
};
struct spot {
    short x;
    short y;
};
struct item {
    char pad00[32];
    unsigned short f32;         /* 0x20 */
    unsigned short f34;         /* 0x22 */
    int f36;                    /* 0x24 */
    char pad28[2];
    unsigned short f42;         /* 0x2a */
    short f44;                  /* 0x2c */
    short f46;                  /* 0x2e */
    char pad30[2];
    short icon;                 /* 0x32 */
    char pad34[2];
    unsigned char f54;          /* 0x36 */
    unsigned char f55;          /* 0x37 */
    char pad38[11];
    struct spot spots[10];      /* 0x43 */
};
struct pc { char pad0[64]; unsigned short flags; };
extern char D_001758B8[];        /* __FILE__ */
extern unsigned char D_001865CA[];
extern unsigned char D_00186634[];
extern struct pc *D_00195BE0;
extern int D_00195D40;
extern struct tmpl *D_00195D4C;
extern int func_0005879B(struct item *);
extern void func_0005E450(unsigned short, struct item *);
extern void func_0005E540(unsigned char, unsigned char, struct item *);
extern void func_0005E874(struct item *);
extern int func_000602C0(int, unsigned short);
extern int func_0007D6AE(int, int);
extern void func_000A0AD9(void *, char *, int, char *, int);

void func_00060430(struct item *a1, int a2)
{
    int n;
    int i;
    int kind;
    int j;
    int r;

    if (a2 == -1) {
        n = i = 0;
        for (; i < D_00195D40; i++)
            if (D_00195D4C[i].used == 0)
                n++;
        n = func_0007D6AE(0, n - 1);
    } else {
        n = a2;
    }
    for (i = 0; i < D_00195D40; i++) {
        if (D_00195D4C[i].used == 0) {
            if (n == 0)
                break;
            n--;
        }
    }
    switch (D_00195D4C[i].f33) {
    case 0:
        kind = D_001865CA[func_0007D6AE(0, 6)];
        break;
    case 1:
        kind = D_00186634[func_0007D6AE(0, 4)];
        break;
    case 2:
        kind = 3;
        break;
    }
    if (kind == 12 || kind == 6) {
        if (D_00195BE0->flags & 1)
            kind = 12;
        else
            kind = 6;
    }
    do
        func_0005E450(kind, a1);
    while (a1->f32 == 3 && a1->f34 == 18);
    func_000A0AD9(a1, D_00195D4C[i].name, 32, D_001758B8, 1039);
    for (j = 0; j < 10; j++) {
        if (D_00195D4C[i].pairs[j].a == 255)
            break;
        if (D_00195D4C[i].pairs[j].b == 255) {
            a1->spots[j].x = D_00195D4C[i].pairs[j].a;
            a1->spots[j].y = 0xffff;
        } else {
            a1->spots[j].x = D_00195D4C[i].pairs[j].a;
            a1->spots[j].y = D_00195D4C[i].pairs[j].b;
        }
    }
    a1->f44 = a1->f46 = D_00195D4C[i].f55;
    a1->f54 = D_00195D4C[i].f61;
    if (a1->f54 == 0 && (a1->f32 == 2 || a1->f32 == 3))
        func_0005E874(a1);
    if (a1->f32 == 2) {
        a1->f55 = 2;
        r = func_000602C0(2, a1->f34);
        if (r != -1)
            a1->icon = r + (a1->icon & -128);
    }
    a1->f36 = func_0005879B(a1);
}

void func_0006077F(struct item *a1, int a2)
{
    int n;
    int i;
    int j;
    int sel;

    if (a2 == -1) {
        n = i = 0;
        for (; i < D_00195D40; i++)
            if (D_00195D4C[i].used)
                n++;
        n = func_0007D6AE(0, n - 1);
    } else {
        n = a2;
    }
    sel = n;
    for (i = 0; i < D_00195D40; i++) {
        if (D_00195D4C[i].used) {
            if (n == 0)
                break;
            n--;
        }
    }
    func_0005E540(D_00195D4C[i].f33, D_00195D4C[i].f34, a1);
    func_000A0AD9(a1, D_00195D4C[i].name, 32, D_001758B8, 1094);
    for (j = 0; j < 10; j++) {
        if (D_00195D4C[i].pairs[j].a == 255)
            break;
        if (D_00195D4C[i].pairs[j].b == 255) {
            a1->spots[j].x = D_00195D4C[i].pairs[j].a;
            a1->spots[j].y = 0xffff;
        } else {
            a1->spots[j].x = D_00195D4C[i].pairs[j].a;
            a1->spots[j].y = D_00195D4C[i].pairs[j].b;
        }
    }
    a1->f44 = a1->f46 = D_00195D4C[i].f55;
    a1->f36 = D_00195D4C[i].f57;
    a1->f54 = D_00195D4C[i].f61;
    if (a1->f32 == 2)
        a1->f55 = 2;
    a1->f42 |= 0x820;
    a1->spots[9].y = sel;
    switch (i) {
    case 0:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 12;
        break;
    case 1:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 13;
        a1->f42 |= 4;
        break;
    case 2:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 10;
        a1->f42 |= 4;
        break;
    case 3:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 8;
        break;
    case 4:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 19;
        break;
    case 5:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 16;
        break;
    case 6:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 25;
        break;
    case 7:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 18;
        break;
    case 8:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 21;
        break;
    case 9:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 2;
        break;
    case 46:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 24;
        break;
    case 47:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 26;
        break;
    case 48:
        a1->icon = (D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800;
        break;
    case 49:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 15;
        break;
    case 50:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 3;
        break;
    case 51:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 9;
        break;
    case 52:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 23;
        break;
    case 53:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 17;
        break;
    case 54:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 7;
        break;
    case 55:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 1;
        break;
    case 56:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 22;
        break;
    case 57:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 20;
        break;
    case 58:
        a1->icon = ((D_00195BE0->flags & 1) != 0 ? 0xd880 : 0xd800) + 5;
        a1->f42 |= 4;
        break;
    }
}
