/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00010585 */
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
struct mobile {
    char pad0[34];
    short f34;                  /* 0x22 */
    char pad24[8];
    short f44;                  /* 0x2c */
    char pad2e[18];
    unsigned short flags;       /* 0x40 */
    char pad42[34];
    unsigned int f100;          /* 0x64 */
    char pad68[35];
    struct bits8 f139;          /* 0x8b */
    char pad8c;
    short f141;                 /* 0x8d */
    short f143;                 /* 0x8f */
    char pad91[114];
    short f259;                 /* 0x103 */
    char pad105[22];
    short f283;                 /* 0x11b */
    char pad11d[252];
    int f537;                   /* 0x219 */
};
struct thing { unsigned char type; char pad[70]; struct mobile mob; };   /* 10.0a packs structs (-zp1) */
struct w0 { unsigned short f0; };
struct w4 { char pad[4]; unsigned short f4; };
struct w6 { char pad[6]; unsigned short f6; };
struct nib { unsigned char lo:4; };
extern unsigned char D_0012AC00;
extern char D_00170077[];
extern char D_0017008C[];
extern short D_001788D3[];
extern unsigned char D_001789FA;
extern int D_00187CA9;
extern struct bits8 D_001940D9;
extern struct bits8 D_001940DB;
extern int D_001940DC;
extern int D_001940E0;
extern int D_0019597C;
extern int D_00195980;
extern struct nib D_001959AC;
extern struct thing *D_00195A88;
extern int D_00195AA0;
extern int D_00195AB0;
extern int D_00195B10;
extern int D_00195B14;
extern int D_00195B18;
extern struct w6 *D_00195B68;
extern struct mobile *D_00195BE0;
extern struct w4 *D_00195BEC;
extern unsigned int D_00195BF4;
extern struct w0 *D_00195BF8;
extern unsigned int D_00195C4C;
extern int D_00195C78;
extern int D_00195D60;
extern int D_00195D7C;
extern int D_00195D8C;
extern short D_00195F4E;
extern unsigned char D_00196270;
extern unsigned char D_00196274;
extern unsigned char D_0019627D;
extern unsigned char D_0019627E;
extern unsigned char D_0019628C;
extern unsigned char D_0019628D;
extern unsigned char D_0019628E;
extern unsigned char D_001962A0;
extern short D_001A3AA8;
extern void func_0002FA97(void);
extern void func_0003F557(void);
extern void func_00040C87(int);
extern void func_0005BC5B(int, int);
extern void func_0006987B(void);
extern void func_00069B7F(void);
extern int func_0007CBA1(char *);
extern int func_0007E350(int);
extern int func_0007E441(int);
extern void func_000CB39A(int, int, int, int);
extern void func_000CDC99(int);
extern void func_0012B136(void);

void func_00010585(void)
{
    struct mobile *rec;
    int sound;
    int speed;
    int old;
    int rnd;

    old = D_00195C78;
    switch (D_001789FA) {
    case 1:
        D_00195C78 = 0;
        break;
    case 2:
        D_00195C78 = func_0007E441(2) - func_0007E350(0);
        if (D_00195C78 < 0)
            D_00195C78 = 0;
        break;
    case 3:
        D_00195C78 = D_0019628C > 5;
        break;
    }
    if (D_00195C78) {
        D_0019627E = old == 0 ? 2 : 3;
        func_00040C87(0);
    }
    if (D_00195A88 != 0 && D_00195A88->type == 18)
        rec = &D_00195A88->mob;
    else
        rec = D_00195BE0;
    if (D_001940DB.b2)
        speed = rec->f44 + 50;
    else
        speed = D_00187CA9 + (rec->f44 - 50);
    sound = 0;
    if (D_00195BE0->flags & 512) {
        speed += 225;
        sound = D_001940DC;
    } else if (D_00195BE0->flags & 1024) {
        speed += 100;
        sound = D_001940E0;
    } else if (D_001940D9.b4 && !D_0019627D) {
        speed = speed * (((D_00195BE0->f283 << 8) / 200) + 320) / 256;
    } else if ((D_001962A0 || D_0019627D) && !D_00195BE0->f139.b4) {
        speed = (speed >>= 2) + speed * ((D_00195BE0->f259 << 8) / 200) / 256;
    }
    if (sound != 0 && D_00196274 == 0) {
        if (D_0019628E == 0)
            rnd = 0;
        else
            rnd = (*(unsigned int *)0x46c >> 1) & 3;
        func_000CB39A(sound, rnd, (D_00195BF8->f0 & 1) ? D_00195B68->f6 : 0, 0);
    }
    D_00195F4E = (speed * D_00195AB0) / 1000;
    if (D_00195BE0->flags & 1) {
        D_00195B10 = 12;
        D_00195B18 = 0;
    } else {
        D_00195B10 = 6;
        D_00195B18 = 512;
    }
    if ((D_00195C4C = (100 - D_00195BE0->f44) * 2 + 70) < 70)
        D_00195C4C = 70;
    else if (D_00195C4C > 800)
        D_00195C4C = 800;
    if (D_00195D60 != 0) {
        D_00195D60 -= D_00195AB0;
        if (D_00195D60 < 0)
            D_00195D60 = 0;
    }
    if (D_0019597C > 0) {
        D_0019597C -= D_00195AB0;
        if (D_0019597C <= 0) {
            D_0019597C = 0;
            func_0007CBA1(D_00170077);
        }
    }
    if (D_00195980 > 0) {
        D_00195980 -= D_00195AB0;
        if (D_00195980 <= 0) {
            D_00195980 = 0;
            func_0007CBA1(D_0017008C);
        }
    }
    if (D_00196274 != 8 && D_00196274 != 2 && D_00196270 != 0)
        func_0003F557();
    if (D_00195B14 == 0)
        D_0019628D = 0;
    D_00195BE0->f143 = (rec->f34 * D_001788D3[(D_00195BEC->f4 >> 10) & 7]) / 256;
    D_00195BE0->f143 += D_001A3AA8;
    if (D_00195BE0->f141 > D_00195BE0->f143)
        D_00195BE0->f141 = D_00195BE0->f143;
    if (D_00195D7C > 0) {
        D_00195D7C -= D_00195AB0;
        if (D_00195D7C == 0)
            D_00195D7C--;
    }
    if (D_00195D7C < 0) {
        func_0002FA97();
        while (D_0012AC00)
            func_0012B136();
    }
    if (D_00195D8C != 0) {
        D_00195D8C -= D_00195AB0;
        if (D_00195D8C < 0)
            D_00195D8C = 0;
    }
    func_000CDC99(273);
    func_000CDC99(278);
    if (D_00195BE0->f139.b6 && D_00195BE0->f100 < D_00195BF4) {
        func_0005BC5B(D_00195AA0, 35);
        D_00195BE0->f139.b6 = 0;
        D_00195BE0->f537 = 0;
    }
    if (D_001959AC.lo == 0)
        func_0006987B();
    func_00069B7F();
}
