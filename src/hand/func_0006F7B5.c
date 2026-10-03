/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006F7B5 */
#pragma pack(1)
struct Img { unsigned short x; unsigned short y; unsigned short w; unsigned short h; char pad[4]; char data[1]; };
struct Box { short x0; short y0; short x1; short y1; void (*fn)(); };
extern unsigned char D_0012AC00;
extern short D_0012AC04;
extern short D_0012AC06;
extern char D_00142309;
extern char *D_00178A0A;
extern struct Box D_001874CD[];
extern char *D_00195B04;
extern struct Img *D_00195BE8;
extern unsigned char *D_00195C44;
extern unsigned char D_00196279;
extern char D_001A9AB8[];
extern unsigned short D_001A9AE1;
extern void func_0006F92F(void);
extern void func_0006F9EF(void);
extern void func_0006FAD4(char *);
extern short func_0007D53F(char *);
extern int func_0012DB50();
extern int func_00144F68();

void func_0006F7B5(void)
{
    int unused1;
    struct Img *img;
    short i;
    short rc;
    short unused2;

    img = D_00195BE8;
    func_00144F68(img->x, img->y, img->w, img->h, img->data);
    func_0012DB50(4);
    rc = func_0007D53F(D_001A9AB8);
    if (rc > -1) {
        func_0006F92F();
        func_0006F9EF();
        return;
    }
    func_0006FAD4(D_00178A0A = D_00195B04 + D_00195C44[20000 + D_001A9AE1] * 89);
    if (D_00142309 != 0) {
        func_0006F92F();
        return;
    }
    if (!(D_0012AC00 != 0 && (D_0012AC00 == 0 || D_00196279 == 0)))
        return;
    for (i = 0; i < 5; i++) {
        if (D_0012AC04 > D_001874CD[i].x0 && D_0012AC04 < D_001874CD[i].x1
          && D_0012AC06 > D_001874CD[i].y0 && D_0012AC06 < D_001874CD[i].y1)
            D_001874CD[i].fn();
    }
}
