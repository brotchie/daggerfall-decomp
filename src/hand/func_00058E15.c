/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x00058E15 */
#pragma pack(1)
struct bits8 { unsigned char b0:1, b1:1, b2:1, b3:1, b4:1, b5:1, b6:1, b7:1; };
struct img {
    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
    unsigned short f8;
    unsigned short size;        /* 10 */
    char data[1];               /* 12 */
};
struct player {
    char pad0[64];
    short flags;                /* 64 */
    char pad42;
    unsigned char f67;          /* 67 */
    char pad44[60];
    unsigned char f128;         /* 128 */
    char pad81[238];
    char *inv[32];              /* 367 */
    char pad1ef[3];
    unsigned char f498;         /* 498 */
};
struct item { char b[107]; };
#pragma pack()
extern int D_000CB24E;
extern int D_00143550;
extern int D_00147954;
extern char D_0017573C[];
extern char D_00175743[];
extern char D_00175750[];
extern char D_0017575D[];
extern char D_0017576C[];
extern char D_0017577C[];
extern char D_0017578A[];
extern char D_00175797[];
extern char D_001903A4[];
extern struct bits8 D_001940D8;
extern int D_00195AA0;
extern int D_00195B64;
extern char *D_00195B74;
extern char *D_00195B78;
extern int D_00195B80;
extern struct player *D_00195BE0;
extern short *D_00195BF8;
extern char *D_00195C44;
extern char *D_00199B4C;
extern char *D_00199B50;
extern struct item D_00199B54[];
extern int D_001AA600;
extern void func_0004D195(int);
extern void func_0005978F(char *, int);
extern void func_00059909(int, int);
extern void func_0005E7FC(char *);
extern struct img *func_0006CB53(char *, int);
extern void func_000A0024(void *, char *, int);
extern void func_000A0040(void *, int, int, char *, int, int);
extern void func_000A0AD9(char *, char *, int, char *, int);
extern void func_000A1023(void *, void *, int, char *, int, int);
extern void func_00144E84(int, int, int, int, int, int);
extern void func_00144F68(int, int, int, int, char *);
extern void func_00144FB4(int, int, int, int, char *);
#pragma aux func_000A0ED9 parm routine [];
extern void func_000A0ED9(int, char *);
extern int func_000A0F5C(char *, char *, ...);

void func_00058E15(int x, int y)
{
    struct img *pic;
    struct img *strip;
    char buf[108];
    int i;
    int n;
    int sub;
    int cnt;
    int found;
    char *it;
    int saved;

    found = 0;
    if (!D_001940D8.b3) return;
    D_001940D8.b3 = 0;
    if (D_00195BE0->f67 == 9) {
        func_000A0AD9(D_001903A4, D_00175743, 160, D_0017573C, 40);
    } else if (D_00195BE0->f67 == 10) {
        func_000A0AD9(D_001903A4, D_00175750, 160, D_0017573C, 42);
    } else {
        func_000A0ED9(44, D_0017573C);
        func_000A0F5C(D_001903A4, D_0017575D, D_00195BE0->f67);
    }
    pic = func_0006CB53(D_001903A4, 0);
    func_00144F68(pic->x + x, pic->y + y, pic->w, pic->h, pic->data);
    func_000A0040((void *)D_00147954, 0, 64000, D_0017573C, 48, 4);
    saved = D_00143550;
    D_00143550 = D_00147954;
    func_00144F68(pic->x + x, pic->y + y, pic->w, pic->h, pic->data);
    D_000CB24E = D_00147954;
    D_00143550 = saved;
    if (pic != 0 && pic != (struct img *)0x97979797) {
        func_000A0024(pic, D_0017573C, 54);
        pic = (struct img *)0x97979797;
    }
    D_00199B50 = D_00195C44 + 64000;
    D_00199B4C = D_00195C44 + 64500;
    func_000A0040(D_00199B50, 0, 112, D_0017573C, 58, 4);
    func_000A0040(D_00195B74, 0, 24625, D_0017573C, 59, 4);
    if (D_00195BE0->f67 == 10) {
        func_000A0AD9(D_001903A4, D_00175750, 160, D_0017573C, 62);
    } else if (D_00195BE0->f67 == 9) {
        func_000A0AD9(D_001903A4, D_00175743, 160, D_0017573C, 64);
    } else if (D_00195BE0->f67 == 8) {
        func_000A0ED9(66, D_0017573C);
        func_000A0F5C(D_001903A4, D_0017576C, (unsigned short)(D_00195BE0->flags & 1), D_00195BE0->f498, ((unsigned short)*D_00195BF8 & 4) != 0 ? 49 : 48);
    } else {
        func_000A0ED9(68, D_0017573C);
        func_000A0F5C(D_001903A4, D_0017576C, (unsigned short)(D_00195BE0->flags & 1), D_00195BE0->f67, ((unsigned short)*D_00195BF8 & 4) != 0 ? 49 : 48);
    }
    pic = func_0006CB53(D_001903A4, 0);
    func_00144FB4(pic->x + x, pic->y + y, pic->w, pic->h, pic->data);
    if (pic != 0 && pic != (struct img *)0x97979797) {
        func_000A0024(pic, D_0017573C, 71);
        pic = (struct img *)0x97979797;
    }
    n = D_00195BE0->f128;
    if (D_00195BE0->f67 == 9 || D_00195BE0->f67 == 10) {
        n = 0;
        func_000A0ED9(78, D_0017573C);
        func_000A0F5C(D_001903A4, D_0017577C, 1 - (D_00195BE0->f67 - 9));
    } else if (D_00195BE0->f67 == 8) {
        func_000A0AD9(D_001903A4, D_0017578A, 160, D_0017573C, 82);
        n = ((unsigned short)D_00195BE0->flags & 1) != 0 ? 0 : 8;
        n += D_00195BE0->f498;
    } else {
        func_000A0ED9(87, D_0017573C);
        func_000A0F5C(D_001903A4, D_00175797, (unsigned short)(D_00195BE0->flags & 1), D_00195BE0->f67);
    }
    pic = func_0006CB53(D_001903A4, 0);
    strip = pic;
    i = 0;
    while (i < n) {
        pic = (struct img *)(pic->size + (char *)pic + 12);
        i++;
    }
    if (D_00195BE0->f67 < 9)
        func_00144FB4(pic->x + x, pic->y + y, pic->w, pic->h, pic->data);
    func_000A1023(D_00195B78, pic, pic->size + 12, D_0017573C, 99, 4);
    if (D_00195BE0->f67 <= 8) {
        cnt = 0;
        for (i = 12; i <= 26; i++) {
            if (D_00195BE0->inv[i] != 0) {
                it = D_00195BE0->inv[i] + 71;
                n = *(unsigned short *)(it + 32);
                sub = *(unsigned short *)(it + 34);
                if (n == 6 || n == 12 || n == 2 || n == 3) {
                    if (n == 3) {
                        func_000A1023(&D_00199B54[cnt], D_00195BE0->inv[i] + 71, 107, D_0017573C, 121, 4);
                        func_0005978F(D_00199B54[cnt++].b, i);
                    } else {
                        func_0005978F(D_00195BE0->inv[i] + 71, i);
                    }
                    if (found == 0) {
                        func_000A1023(buf, D_00195BE0->inv[i] + 71, 107, D_0017573C, 128, 4);
                        if ((n == 6 && (sub == 13 || sub == 14)) || (n == 12 && (sub == 9 || sub == 10))) {
                            found = 1;
                            buf[66] = 0;
                            func_0005E7FC(buf);
                            func_0005978F(buf, i);
                        }
                    }
                }
            }
        }
        func_00059909(x, y);
    }
    if (strip != 0 && strip != (struct img *)0x97979797) {
        func_000A0024(strip, D_0017573C, 143);
        strip = (struct img *)0x97979797;
    }
    func_00144E84(x + 192, y + 1, 125, 197, D_00195B64, 0);
    D_00195B80 = D_001AA600;
    func_0004D195(D_00195AA0);
}
