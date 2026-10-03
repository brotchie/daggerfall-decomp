/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0007A983 */
#pragma pack(1)
struct Vec { int x; int y; int z; };
struct Ent { char pad[27]; unsigned short id; };
struct Reg { char pad[41]; unsigned short count; char *data; };
struct Loc { char pad[24]; unsigned char kind; };
struct Ply { char pad0[34]; short v22; char pad1[141 - 36]; short v8d; short v8f; };
extern int D_000C23C4;
extern int D_000C23C8;
extern int D_000C23CC;
extern unsigned char D_0012AC00;
extern char D_00176884[];
extern char D_001768DF[];
extern char D_00176909[];
extern char D_00176915[];
extern char D_0017691F[];
extern char D_00176927[];
extern char D_0017694A[];
extern char D_00176964[];
extern short D_001788D3[];
extern unsigned char D_001789FA;
extern unsigned short D_00185C14;
extern int D_00186503[];
extern int D_0018DC00;
extern char D_00190FE4[];
extern char D_001917E4[];
extern int D_001959A8;
extern char D_001959D8[];
extern int D_00195A00;
extern struct Loc *D_00195A9C;
extern int D_00195AA0;
extern int D_00195AA4;
extern struct Ent *D_00195AC4;
extern int D_00195B44;
extern struct Reg *D_00195BDC;
extern struct Ply *D_00195BE0;
extern char *D_00195BEC;
extern int D_00195BF4;
extern unsigned char *D_00195BF8;
extern int D_00195C40;
extern int D_00195D48;
extern int D_00195D84;
extern int D_00195DA0;
extern char D_00195E7F;
extern unsigned char D_00196268;
extern unsigned char D_00196279;
extern char D_00196289;
extern char D_0019629B;
extern int D_001A4A0C;
extern int D_001A4FE4;
extern char D_001A5B88[];
extern char D_001A94A0[];
extern char D_001A94B0[];
extern int func_00020057(int, int);
extern void func_00028F56(void);
extern void func_00028F8B();
extern void func_0002BCCF(int);
extern void func_00030C10(void);
extern void func_0004B5CF(void);
extern void func_0004C759(void);
extern void func_0004F3F6(void);
extern void func_00050069(char *);
extern void func_00064467(int);
extern void func_000698AF(short);
extern void func_00069D36(void);
extern void func_0006A654(int);
extern int func_0006CE7E(char *);
extern void func_0006CEFC(char *, char *, char *);
extern void func_00072AA0(void);
extern void func_00078C79(void);
extern void func_00079BE4(int);
extern void func_0007A1FE(void);
extern void func_0007A4B3(void);
extern void func_0007A8BE(void);
extern void func_0007B022(char *);
extern void func_0007B9A1(char *);
extern void func_0007C432(void);
extern void func_0007C8AF(void);
extern struct Loc *func_0007DF35(int);
extern void func_00086D37(unsigned short);
extern void func_000876AD(int, int, int, int);
extern void func_0008DA1C(int);
extern int func_0008DA4D(int);
extern int func_0008DD46(int, int);
extern void func_0008E005(int);
extern void func_0008E3F7(struct Ent *, void (*)());
extern int func_0008ED15(int);
extern void func_000922CA(void);
extern int func_0009DEA7(int);
extern int func_000A0040(void *, int, int, char *, int, int);
extern int func_000A00CB(int, void *, int);
extern int func_000C2FF5();
extern int func_0012A2D0();
extern int func_0012B136();
#pragma aux func_000A0ED9 parm routine [];
extern int func_000A0ED9(int, char *);
extern int func_0009DC59(char *, ...);
extern int func_000A0F5C(char *, char *, ...);

int func_0007A983(char *name)
{
    int fd;
    int l20;
    int sz;
    unsigned short l18;
    struct Vec vec;
    int l24;
    char buf[1024];

    func_000A0ED9(637, D_00176884);
    func_000A0F5C(buf, D_00176909, name);
    func_000A0ED9(638, D_00176884);
    func_000A0F5C(D_00190FE4, D_001768DF, buf, D_00176927);
    fd = func_0009DC59(D_00190FE4, 512);
    if (fd < 0)
        return 0;
    func_000A00CB(fd, &D_001A4FE4, 4);
    func_0009DEA7(fd);
    func_0006A654(1002);
    func_00086D37(D_00195AC4->id);
    if (D_001A4FE4 < 293 || D_001A4FE4 > 294)
        func_00050069(D_0017694A);
    func_000A0040(D_001959D8, 0, 36, D_00176884, 655, 36);
    func_0008E005(D_00195AA4);
    func_0008DA1C(D_00195AA4);
    func_0008DA1C((int)D_00195AC4);
    func_0008DA1C(D_001959A8);
    func_0008DD46((int)D_00195AC4, D_00195AA4);
    D_00195AA0 = 0;
    func_0006CEFC(D_00176915, buf, D_001917E4);
    func_0006CEFC(D_0017691F, buf, D_001917E4);
    func_0006CEFC(D_001A5B88, buf, D_001917E4);
    func_0007B022(buf);
    func_0007B9A1(name);
    func_0007C432();
    l24 = D_00196268;
    D_00196268 = 255;
    func_000A0ED9(677, D_00176884);
    func_000A0F5C(D_00190FE4, D_001768DF, buf, D_00176927);
    D_001A4A0C = func_0009DC59(D_00190FE4, 512);
    func_000A00CB(D_001A4A0C, &D_001A4FE4, 4);
    func_000A00CB(D_001A4A0C, &vec, 12);
    func_000A00CB(D_001A4A0C, &l18, 2);
    func_000A00CB(D_001A4A0C, &D_001789FA, 1);
    func_00020057(vec.x, vec.z);
    if (l18 != 65535) {
        func_000876AD(l24, D_001789FA, l18, 0);
    } else {
        D_000C23C4 = vec.x;
        D_000C23C8 = vec.y;
        D_000C23CC = vec.z;
        func_000C2FF5();
        func_000A0040(D_001A94B0, 0, 16, D_00176884, 696, 16);
        func_000A0040(D_001A94A0, 0, 16, D_00176884, 697, 16);
    }
    func_0008DA4D(D_00195AA4);
    sz = D_00195BDC->count * 26;
    func_000A00CB(D_001A4A0C, &sz, 4);
    func_000A00CB(D_001A4A0C, D_00195BDC->data, sz);
    func_00079BE4((int)D_00195AC4);
    func_00079BE4(D_001959A8);
    func_00064467(D_001A4A0C);
    func_0009DEA7(D_001A4A0C);
    func_0007C8AF();
    func_0007A4B3();
    func_00030C10();
    func_00078C79();
    func_0007A1FE();
    func_000922CA();
    func_00072AA0();
    func_0004F3F6();
    func_0007A8BE();
    func_0002BCCF(D_00195A00);
    func_0004C759();
    if (D_001789FA == 3)
        func_00028F56();
    if (D_001789FA == 2)
        D_00195D84 = D_00186503[(D_00195A9C = func_0007DF35(D_00195AA4))->kind];
    if ((int)(unsigned short)(*(unsigned short *)D_00195BF8 & 1) != 0)
        func_0012A2D0(160, 100, 160, 100);
    else
        func_0012A2D0(160, 77, 160, 77);
    func_000698AF(*(short *)(D_00195BF8 + 4));
    D_0012AC00 = D_00196279 = 0;
    func_0012B136();
    while (D_0012AC00 != 0)
        func_0012B136();
    D_00195C40 = *(int *)0x46c;
    D_00196289 = 0;
    func_0004B5CF();
    D_00195D48 = 10000;
    D_0019629B = 0;
    D_00185C14 = 65535;
    D_0018DC00 = 0;
    D_00195E7F = 0;
    if (func_0006CE7E(D_00176964) != 0) {
        *D_00195BF8 |= 4;
        D_00195DA0 = 434;
    } else {
        *D_00195BF8 &= ~4;
        D_00195DA0 = 380;
    }
    func_00069D36();
    func_0008E3F7(D_00195AC4, func_00028F8B);
    D_00195B44 = D_00195BF4;
    func_0006A654(1003);
    l20 = func_0008ED15((int)D_00195AC4);
    l20 = func_0008ED15(D_001959A8);
    D_00195BE0->v8f = D_00195BE0->v22 * D_001788D3[(*(unsigned short *)(D_00195BEC + 4) >> 10) & 7] / 256;
    if (D_00195BE0->v8d < 0)
        D_00195BE0->v8d = 0;
    return 1;
}
