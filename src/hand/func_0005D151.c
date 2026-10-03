/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0005D151 */
struct img { short x, y, w, h; char pad8[2]; char data[1]; };
struct uimg { unsigned short x, y, w, h; char pad8[4]; char data[1]; };
struct pc { char pad[3]; short f3; };
struct flags { unsigned char b0:3; unsigned char b3:1; unsigned char b4:1; };
extern struct img *D_00190904;
extern struct img *D_00190908;
extern struct img *D_0019090C;
extern struct uimg *D_00190910;
extern struct flags D_001940D6;
extern struct pc *D_00195A98;
extern int func_0007ED48(int);
extern int func_0007ED6C(void);
extern int func_0007EDD3(void);
extern void func_00144FB4(int, int, int, int, char *);

void func_0005D151(void)
{
    char *data;
    struct uimg *u;
    int frame;

    frame = func_0007ED48((D_00195A98->f3 & 0x7ff) >> 6);
    data = D_00190904->data + D_00190904->w * D_00190904->h * frame;
    func_00144FB4(D_00190904->x, D_00190904->y, D_00190904->w, D_00190904->h, data);
    if (D_001940D6.b3) {
        frame = func_0007EDD3();
        data = D_00190908->data + frame * (D_00190908->w * D_00190908->h);
        func_00144FB4(D_00190908->x, D_00190908->y, D_00190908->w, D_00190908->h, data);
    }
    if (D_001940D6.b4) {
        frame = func_0007ED6C();
        data = D_0019090C->data + frame * (D_0019090C->w * D_0019090C->h);
        func_00144FB4(D_0019090C->x, D_0019090C->y, D_0019090C->w, D_0019090C->h, data);
    }
    u = D_00190910;
    func_00144FB4(u->x, u->y, u->w, u->h, u->data);
}
