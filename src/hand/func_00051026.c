/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00051026 */
struct anims {
    char *a;
    char *b;
};
struct bits8 {
    unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1;
};
extern unsigned char D_0012AC00;
extern short D_0012AC04;
extern short D_0012AC06;
extern unsigned char D_00142325;
extern unsigned char D_00142332;
extern unsigned char D_00142335;
extern unsigned char D_00142350;
extern unsigned char D_00142358;
extern char *D_00143550;
extern struct bits8 D_00147964;
extern char D_0017539B[];        /* __FILE__ */
extern char D_001753A6[];
extern char D_001753B3[];
extern char D_001753C0[];
extern char D_001753CD[];
extern unsigned char D_0018520B[][3];
extern unsigned char D_00185284[];
extern unsigned char D_0018528E[];
extern unsigned char D_00185291[];
extern char D_001903A4[];
extern short D_00190D68;
extern int D_00195BEC;
extern char *D_00195C44;
extern unsigned char D_00196279;
extern char D_0019980C[];
extern unsigned char D_00199819[];
extern int func_0003B0B4(short, short, short, char *, unsigned char, unsigned char);
extern void func_0004FF2E(void);
extern void func_0004FF57(int);
extern void func_00051490(struct anims *);
extern void func_000516FD(struct anims *, int);
extern void func_000517EC(short);
extern short func_0005183B(void);
extern int func_00051A5A(void);
extern char *func_0006CB53(char *, int);
extern void func_000A0024(char *, char *, int);
extern void func_000A0040(void *, int, int, char *, int, int);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);
extern void func_000A1023(void *, void *, int, char *, int, int);
extern void func_000CD33A(unsigned char *, int, int);
extern void func_000CD367(char *);
extern void func_0012B136(void);
extern void func_0012B2D3(short, short);
extern void func_00143914(int);

int func_00051026(void)
{
    short sel;
    short tries;
    short done;
    struct anims h;
    short unused;
    unsigned char rgb[4];

    tries = 10;
    func_000A0040(D_00143550, 0, 64000, D_0017539B, 79, 4);
    func_000A1023((void *)0xa0000, D_00143550, 64000, D_0017539B, 80, 4);
    func_000A0040(D_0019980C, 0, 10, D_0017539B, 81, 10);
    func_000A0040(D_00199819, 0, 3, D_0017539B, 82, 3);
    rgb[0] = rgb[1] = rgb[2] = 0;
    D_00147964.b0 = 0;
    func_0006CB53(D_001753A6, (int)D_00195C44);
    for (sel = 0; sel < 768; sel++)
        (sel + D_00195C44)[64000] <<= 2;
    func_000CD367(D_00195C44 + 64000);
    func_000A1023(D_00143550, D_00195C44, 64000, D_0017539B, 90, 4);
    h.a = func_0006CB53(D_001753B3, 0);
    h.b = func_0006CB53(D_001753C0, 0);
    while (tries-- != 0) {
        done = 0;
        D_00147964.b0 = 0;
        func_00051490(&h);
        while (done == 0) {
            if (D_00142325 && D_00142335 && D_00142332)
                func_0004FF57(0);
            D_00196279 = D_0012AC00;
            func_0012B136();
            func_0012B2D3(D_0012AC04, D_0012AC06);
            if ((D_0012AC00 & 1) && (D_0012AC04 > 0 && D_0012AC04 < 320 && D_0012AC06 > 120 && D_0012AC06 < 140) || D_00142350)
                func_000516FD(&h, -1);
            else if ((D_0012AC00 & 1) && (D_0012AC04 > 0 && D_0012AC04 < 320 && D_0012AC06 > 180 && D_0012AC06 < 200) || D_00142358)
                func_000516FD(&h, 1);
            sel = func_0005183B();
            if (sel != 0) {
                sel = D_0018520B[D_00190D68 - 1][sel];
                if (D_00199819[sel] < 10)
                    D_00199819[sel]++;
                rgb[2] = D_00185284[D_00199819[sel]] << 2;
                func_000517EC(sel);
                func_000CD33A(rgb, D_0018528E[sel], 1);
                done = 1;
            }
            func_000A1023((void *)0xa0000, D_00143550, 64000, D_0017539B, 122, 4);
        }
    }
    func_00143914(0);
    func_000A1023((void *)0xa0000, D_00143550, 64000, D_0017539B, 127, 4);
    func_0004FF2E();
    sel = D_00185291[func_00051A5A()];
    if (func_0003B0B4(sel + 2100, 4, 5, 0, 21, 49) != 0)
        sel = -1;
    if (sel != -1) {
        func_000A0ED9(134, D_0017539B);
        func_000A0F5C(D_001903A4, D_001753CD, sel);
        func_0006CB53(D_001903A4, D_00195BEC);
    }
    if (h.a != 0 && h.a != (char *)0x97979797) {
        func_000A0024(h.a, D_0017539B, 138);
        h.a = (char *)0x97979797;
    }
    if (h.b != 0 && h.b != (char *)0x97979797) {
        func_000A0024(h.b, D_0017539B, 139);
        h.b = (char *)0x97979797;
    }
    func_000A0040(D_00143550, 0, 64000, D_0017539B, 140, 4);
    func_000A1023((void *)0xa0000, D_00143550, 64000, D_0017539B, 141, 4);
    func_0004FF2E();
    return sel;
}
