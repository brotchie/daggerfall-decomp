/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0008F31F */
struct bf8_2_1 { unsigned char _:2; unsigned char f:1; };
extern char D_0012AC00[];
extern char D_0012AC04[];
extern char D_0012AC06[];
extern char D_00142309[];
extern char D_00176E94[];
extern char D_00187FE0[];
extern char D_00187FE2[];
extern char D_00187FE4[];
extern char D_00187FE6[];
extern char D_00187FE8[];
extern char D_001903A4[];
extern char D_00190BE4[];
extern char D_001940D4[];
extern char D_00195AA0[];
extern char D_00195AA4[];
extern char D_00195BE0[];
extern char D_00195BE8[];
extern char D_00196279[];
extern char D_001A9B9C[];
extern char D_001A9BB4[];
extern char D_001A9BBC[];
extern char D_001A9BDC[];
extern char D_001AA3DC[];
extern char D_001AA3E0[];
extern char D_001AA3E4[];
extern char D_001AA3E8[];
extern char D_001AA3EC[];
extern int func_000392AD(void);
extern void func_0003F09F(int, int);
extern int func_00069938(int, int, int);
extern void func_0007CA1F(int, int, int, int, unsigned char);
extern void func_0007CA85(int, int, int, int, unsigned char);
extern void func_0008E3F7(int, int);
extern int func_0008F246(int);
extern void func_0008F89A(int);
extern int func_0008F94D(unsigned short, unsigned short);
extern void func_0008F9E1(int);
extern int func_0008FC79(void);
extern void func_00090426(int);
extern int func_000A0040();
extern int func_000A0DD9();
extern int func_000CB552();
extern int func_000CD1C5();
extern int func_0012DB50();
extern int func_00135D00();

struct img {
    short f0;
    short f2;
    unsigned short w;
    unsigned short h;
    short f8;
    short f10;
    short f12;
    int data;
};

struct hotspot {
    short x0;
    short y0;
    short x1;
    short y1;
    void (*fn)(void);
};

#define COUNT (*(short *)D_001AA3EC)
#define MOUSE_X (*(short *)D_0012AC04)
#define MOUSE_Y (*(short *)D_0012AC06)

void func_0008F31F(void)
{
    char buf[112];      /* never used: it only sizes the frame */
    struct img *l_24;
    unsigned char *l_20;
    short l_1C;
    short l_18;

    if (func_0008F246(0) == 0) return;
    func_000CB552(*(int *)D_00195BE8);
    func_0012DB50(4);
    func_0007CA1F(*(int *)D_001AA3E8, 31, 185, 145, 156);
    func_0007CA1F(func_000A0DD9(*(int *)(*(char **)D_00195BE0 + 133), (int)D_001903A4, 10), 235, 185, 145, 156);
    func_0012DB50(3);
    *(int *)D_001AA3E4 = 0;
    func_000A0040((int)D_001A9BDC, 0, 2048, (int)D_00176E94, 182, 2048);
    func_0008E3F7(*(int *)(*(char **)D_00195AA0 + 63), (int)func_0008F9E1);
    for (COUNT = l_1C = 0; l_1C < 8; l_1C++) {
        if (((int *)D_001A9BBC)[l_1C] != 0) {
            l_20 = (unsigned char *)((int *)D_001A9BBC)[l_1C] + 71;
            l_24 = *(struct img **)((char *)func_00135D00(*(unsigned short *)(l_20 + 50) >> 7, *(unsigned short *)(l_20 + 50) & 127, -1) + 12);
            func_000CD1C5((COUNT & 1) * 56 + 233 - (l_24->w >> 1), (COUNT >> 1) * 38 + 42 - (l_24->h >> 1), l_24->w, l_24->h, (char *)l_24 + l_24->data);
            func_0007CA85((int)l_20, (short)((COUNT & 1) * 56 + 236), (short)((COUNT >> 1) * 40 + 48), 145, 156);
            ((short *)D_001A9B9C)[COUNT++] = l_1C;
        }
    }
    if (*(int *)D_001AA3E4 == 0 && *(int *)D_001AA3E0 == 0 && COUNT == 0) {
        func_0003F09F(34, 1);
        func_0008FC79();
    }
    if (((struct bf8_2_1 *)&D_001940D4)->f && (l_1C = func_000392AD()) > -1)
        func_00090426(((int *)D_00190BE4)[l_1C]);
    if (*(char *)D_00142309 != 0)
        func_0008FC79();
    if (*(char *)D_0012AC00 == 0 || (*(char *)D_0012AC00 != 0 && *(char *)D_00196279 != 0)) return;
    for (l_1C = 0; l_1C < 5; l_1C++) {
        if (MOUSE_X > ((struct hotspot *)D_00187FE0)[l_1C].x0 && MOUSE_X < ((struct hotspot *)D_00187FE0)[l_1C].x1
         && MOUSE_Y > ((struct hotspot *)D_00187FE0)[l_1C].y0 && MOUSE_Y < ((struct hotspot *)D_00187FE0)[l_1C].y1) {
            func_00069938(203, *(int *)D_00195AA4, 100);
            ((struct hotspot *)D_00187FE0)[l_1C].fn();
        }
    }
    if (MOUSE_X > 221 && MOUSE_X < 304 && MOUSE_Y > 30 && MOUSE_Y < 171) {
        l_1C = (MOUSE_X - 221) % 56;
        if (l_1C > 27) return;
        l_1C = (MOUSE_X - 221) / 56;
        l_18 = (MOUSE_Y - 30) % 38;
        if (l_18 > 24) return;
        l_18 = (MOUSE_Y - 30) / 38;
        l_1C += l_18 + l_18;
        ((int *)D_001A9BBC)[((short *)D_001A9B9C)[l_1C]] = 0;
        ((unsigned char *)D_001A9BB4)[((short *)D_001A9B9C)[l_1C]] = 254;
        return;
    }
    if (MOUSE_X < 16 || MOUSE_X > 155 || MOUSE_Y < 30 || MOUSE_Y > 171) return;
    l_1C = (MOUSE_X - 16) % 56;
    if (l_1C > 27) return;
    l_1C = (MOUSE_X - 16) / 56;
    l_18 = (MOUSE_Y - 30) % 38;
    if (l_18 > 27) return;
    l_18 = (MOUSE_Y - 30) / 38;
    l_1C += l_18 * 3;
    l_20 = ((unsigned char **)D_001A9BDC)[l_1C + *(int *)D_001AA3DC] + 71;
    if (((int *)D_001A9BDC)[l_1C + *(int *)D_001AA3DC] != 0 && COUNT != 8 && func_0008F94D(*(unsigned short *)(l_20 + 32), *(unsigned short *)(l_20 + 34)) == 0)
        func_0008F89A(l_1C + *(int *)D_001AA3DC);
}
