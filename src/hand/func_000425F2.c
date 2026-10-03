/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000425F2 */
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
struct item { char pad0[103]; unsigned short type; };
struct pc {
    char pad00[64];
    unsigned short flags;       /* 0x40 */
    char pad42[67];
    int gold;                   /* 0x85 */
    char pad89[4];
    short f141;                 /* 0x8d */
    short f143;                 /* 0x8f */
    char pad91[222];
    struct item *equip[32];     /* 0x16f */
};
struct obj {
    unsigned char type;         /* 0x00 */
    short f1;                   /* 0x01 */
    short f3;                   /* 0x03 */
    short f5;                   /* 0x05 */
    int x;                      /* 0x07 */
    int y;                      /* 0x0b */
    int z;                      /* 0x0f */
};
extern unsigned char D_0012AC00;
extern short D_0012AC04;
extern short D_0012AC06;
extern unsigned char D_0012B508;
extern unsigned char D_00142314;
extern unsigned char D_00142315;
extern unsigned char D_00142325;
extern unsigned char D_00142332;
extern unsigned char D_00142335;
extern unsigned char D_00142338;
extern unsigned char D_00142340;
extern char D_00170E38[];        /* __FILE__ */
extern char D_00170E3F[];
extern unsigned char D_001789FA;
extern char *D_0018323C;
extern char *D_00183240;
extern char *D_00183244;
extern char *D_00184876;
extern short D_00187CA9;
extern char D_001903A4[];
extern struct bits8 D_001940D5;
extern struct bits8 D_001940D6;
extern struct bits8 D_001940D9;
extern struct bits8 D_001940DA;
extern unsigned char D_001940DB;
extern int D_001950E4;
extern int D_001950E8;
extern int D_001959B8;
extern int D_001959BC;
extern int D_001959C0;
extern struct obj *D_00195A98;
extern struct obj *D_00195AA4;
extern char *D_00195AC4;
extern int D_00195AE8;
extern char *D_00195B40;
extern char *D_00195B48;
extern struct pc *D_00195BE0;
extern int D_00195D60;
extern unsigned char D_00195E7A;
extern unsigned char D_00195E7F;
extern short D_00195F2E;
extern short D_00195F52;
extern short D_00195F54;
extern short D_00195F62;
extern unsigned char D_0019626A;
extern unsigned char D_00196272;
extern unsigned char D_00196274;
extern unsigned char D_00196276;
extern unsigned char D_0019627D;
extern unsigned char D_001962A0;
extern int D_00199700;
extern int D_001A4A70;
extern int D_001A4A74;
extern unsigned char D_001A5C27;
extern void func_00026904(void);
extern void func_0003A2AD(char *, int);
extern void func_00042E24(int);
extern int func_00042F0F(int);
extern int func_000430CD(int);
extern void func_000438DF(void);
extern void func_00045F25(void);
extern void func_00045F8F(void);
extern void func_0004FF57(int);
extern int func_0005B24C(void);
extern void func_0005D486(void);
extern void func_0005D74A(void);
extern void func_000717EC(void);
extern void func_000784EE(void);
extern void func_0007B222(int);
extern int func_0007CBA1(char *);
extern int func_0007D068(unsigned char);
extern void func_0008DA91(char *);
extern void func_0008DEB4(struct obj *, int, int, int, int, int, int);
extern void func_00098651(void);
extern struct obj *func_0009A0A0(char *, int, int);
extern int func_0009A993(int);
extern void func_0012B49E(short, short);
extern void func_00135E90(void);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void func_000425F2(void)
{
    int u30;
    int u2c;
    int u28;
    int saved;
    struct obj *o;
    int u1c;
    int view;

    if (D_00142325 && D_00142335 && D_00142332)
        func_0004FF57(0);
    if (D_00142325 && D_00142338)
        func_0007B222(1);
    if (D_00196272)
        return;
    if (D_00196274)
        return;
    if (D_00142340 && func_0007D068(87))
        func_000784EE();
    if (func_000430CD(29)) {
        if (D_00195D60 == 0)
            func_0005B24C();
        else
            func_0007CBA1(D_00184876);
    }
    if (func_00042F0F(27))
        func_0009A993(1);
    if (func_00042F0F(26))
        func_00026904();
    if (func_00042F0F(30))
        if (D_00195B40 || D_00195B48) {
            D_00195BE0->f141 += D_00195F62;
            if (D_00195BE0->f141 > D_00195BE0->f143)
                D_00195BE0->f141 = D_00195BE0->f143;
            if (D_00195B40) {
                func_0003A2AD(D_00195B40 + 71, -1);
                func_0008DA91(D_00195B40);
            } else {
                func_0003A2AD(D_00195B40 + 71, -1);
                func_0008DA91(D_00195B48);
            }
            D_00195B40 = D_00195B48 = 0;
        }
    if (func_000430CD(32))
        func_0005D486();
    if (func_000430CD(12))
        func_000717EC();
    if (func_000430CD(34) && D_001940D6.b6 && (D_001A4A70 | D_001A4A74) == 0) {
        o = (struct obj *)D_00195BE0->equip[(D_0019626A ^ 1) ? 21 : 19];
        if (o == 0 || ((struct item *)o)->type == 3) {
            D_0019626A ^= 1;
            func_000A0ED9(96, D_00170E38);
            func_000A0F5C(D_001903A4, D_0018323C, D_0019626A ? D_00183240 : D_00183244);
            D_00195F2E = 20;
            D_0012B508 = 146;
            func_0007CBA1(D_001903A4);
        }
    }
    if (func_0007D068(68))
        func_000438DF();
    view = D_00196276;
    if (func_00042F0F(14))
        D_00196276 = 2;
    if (func_00042F0F(15))
        D_00196276 = 0;
    if (func_00042F0F(16))
        D_00196276 = 1;
    if (func_00042F0F(17))
        D_00196276 = 3;
    if (D_00196276 != view)
        func_00042E24(0);
    if (func_00042F0F(11) && (D_00195BE0->flags & 1536) == 0 && !D_0019627D && !D_001962A0)
        D_001940D9.b4 = 1;
    else
        D_001940D9.b4 = 0;
    if (func_00042F0F(31))
        func_0005D74A();
    if (func_000430CD(19))
        D_00195E7F ^= 1;
    if (func_00042F0F(13))
        func_00098651();
    if (func_0007D068(2) && D_001A5C27)
        D_00187CA9 ^= 1300;
    if (D_00195E7A == 0) {
        if (func_000430CD(22) || D_001940DA.b6 && (D_0012AC00 & 1)) {
            D_001959B8 = 32;
            D_001959BC = 0;
            D_001959C0 = 0;
            D_00195AA4->f1 = D_00195A98->f1 = 0;
        }
        if (func_00042F0F(23)) {
            if (!D_001940DA.b6) {
                D_00195F54 = D_0012AC04;
                D_00195F52 = D_0012AC06;
            }
            D_001940DA.b6 = 1;
        } else {
            if (D_001940DA.b6)
                func_0012B49E(D_00195F54, D_00195F52);
            D_001940DA.b6 = 0;
        }
    } else {
        D_001959B8 = 0;
        D_001959BC = 0;
        D_001959C0 = 0;
    }
    if (func_00042F0F(20))
        D_001959B8 -= 32;
    else if (func_00042F0F(21))
        D_001959B8 += 32;
    if (D_001959B8 < -256)
        D_001959B8 = -256;
    else if (D_001959B8 > 256)
        D_001959B8 = 256;
    if ((D_00195BE0->flags & 1536) == 0 && func_000430CD(9) && !D_001962A0)
        D_001940DB ^= 4;
    saved = D_00195AE8;
    if (D_00142325 && func_0007D068(59))
        D_00195AE8 ^= 8;
    if (D_00142325 && func_0007D068(62) && D_001A5C27)
        D_00195AE8 ^= 64;
    if (D_00142325 && func_0007D068(67) && D_001A5C27)
        D_00195BE0->gold += 5000;
    if (D_00195AE8 != saved)
        func_0007CBA1(D_00170E3F);
    if (D_00142314 && D_001A5C27)
        func_00045F25();
    if (D_00142315 && D_001A5C27)
        func_00045F8F();
    if (func_0007D068(26) && D_001789FA == 3 && D_001A5C27) {
        D_00199700 = --D_00199700 % (D_001950E4 + D_001950E8);
        if (D_00199700 < 0)
            D_00199700 = D_001950E4 + D_001950E8 - 1;
        if (D_00199700 < D_001950E4)
            o = func_0009A0A0(D_00195AC4, 9, D_00199700);
        else
            o = func_0009A0A0(D_00195AC4, 16, D_00199700 - D_001950E4);
        func_0008DEB4(D_00195AA4, o->x, o->y, o->z, o->f1, o->f3, o->f5);
        D_00195A98->f3 = D_00195AA4->f3;
        func_00135E90();
        D_001940D5.b1 = 1;
    }
    if (func_0007D068(27) && D_001789FA == 3 && D_001A5C27) {
        D_00199700 = ++D_00199700 % (D_001950E4 + D_001950E8);
        if (D_00199700 < D_001950E4)
            o = func_0009A0A0(D_00195AC4, 9, D_00199700);
        else
            o = func_0009A0A0(D_00195AC4, 16, D_00199700 - D_001950E4);
        func_0008DEB4(D_00195AA4, o->x, o->y, o->z, o->f1, o->f3, o->f5);
        D_00195A98->f3 = D_00195AA4->f3;
        func_00135E90();
        D_001940D5.b1 = 1;
    }
}
