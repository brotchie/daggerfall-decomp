/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x0006BA50 */
struct Img { unsigned short x; unsigned short y; unsigned short w; unsigned short h; char pad[4]; char data[1]; };
struct stats { int f0; int f4; int f8; };
struct ent { unsigned char f0; char pad1[4]; int f5; };
struct slot { struct ent *e; char pad[16]; };      /* 20 bytes */
struct rec { char pad[74]; };
extern char *D_00143550;
extern char D_00175CC4[];        /* __FILE__ */
extern char D_00175CCB[];
extern char D_00190B44[];
extern char D_00190FE4[];
extern struct Img *D_00195B5C;
extern struct Img *D_00195BE8;
extern int D_00195BF4;
extern struct slot D_001A3FAC[];
extern char *D_001A4144;
extern struct rec D_001A4148[];
extern struct stats *D_001A41EC;
extern unsigned char D_001A41F1;
extern unsigned char D_001A41F2;
extern void func_0004633F(char *, char *);
extern void func_0006C29B(int, void *);
extern void func_0006C55B(void);
extern void func_0006C692(void);
extern void func_0007CA1F(char *, short, short, int, unsigned char);
extern int func_0007F349(void);
extern char *func_000A0DD9(int, char *, int);
extern void func_000A1023(char *, char *, int, char *, int, int);
extern int func_00144F68();

void func_0006BA50(void)
{
    int saved;
    struct Img *img;

    img = D_00195BE8;
    func_000A1023(D_00143550, D_001A4144, 64000, D_00175CC4, 216, 4);
    func_00144F68(img->x, img->y, img->w, img->h, img->data);
    switch (D_001A41F1) {
    case 0:
        func_0007CA1F(func_000A0DD9(D_001A41EC->f0, D_00190FE4, 10), 197, 19, 145, 156);
        func_0007CA1F(func_000A0DD9(func_0007F349(), D_00190FE4, 10), 203, 29, 145, 156);
        if (D_001A41EC->f8 != 0) {
            func_0007CA1F(func_000A0DD9(D_001A41EC->f4, D_00190FE4, 10), 143, 39, 145, 156);
            saved = D_00195BF4;
            D_00195BF4 = D_001A41EC->f8;
            func_0004633F(D_00175CCB, D_00190B44);
            func_0007CA1F(D_00190B44, 119, 49, 145, 156);
            D_00195BF4 = saved;
        }
        break;
    case 1:
        func_00144F68(D_00195B5C->x, D_00195B5C->y, D_00195B5C->w, D_00195B5C->h, D_00195B5C->data);
        func_0006C55B();
        func_0006C29B(D_001A3FAC[D_001A41F2].e->f0, (void *)D_001A3FAC[D_001A41F2].e->f5);
        break;
    case 2:
        func_00144F68(D_00195B5C->x, D_00195B5C->y, D_00195B5C->w, D_00195B5C->h, D_00195B5C->data);
        func_0006C692();
        func_0006C29B(1, &D_001A4148[D_001A41F2]);
        break;
    }
}
