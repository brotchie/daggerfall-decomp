/* matched by the real Watcom C32 10.0a (-d2), lifted from 0x000874C0 */
struct obj { char pad[3]; short angle; char pad2[2]; int x; int y; int z; char pad3[44]; int f63; };
extern struct obj *D_00195A98;
extern struct obj *D_00195AA4;
extern struct obj *D_00195AC4;
extern unsigned char *D_00195BDC;
extern void func_0009A6D0(int, int);
extern int func_0014B45B(int, int);

void func_000874C0(unsigned a1)
{
    switch (a1) {
    case 0:
    case 1:
        D_00195AA4->x = D_00195AC4->x + (D_00195BDC[32] << 11);
        D_00195AA4->z = D_00195AC4->z - 256;
        D_00195AA4->angle = D_00195A98->angle = 0;
        break;
    case 2:
    case 3:
        D_00195AA4->x = D_00195AC4->x - 256;
        D_00195AA4->z = D_00195AC4->z + (D_00195BDC[33] << 11);
        D_00195AA4->angle = D_00195A98->angle = 512;
        break;
    case 4:
    case 5:
        D_00195AA4->x = D_00195AC4->x + (D_00195BDC[32] << 11);
        D_00195AA4->z = D_00195AC4->z + (D_00195BDC[33] << 12) + 256;
        D_00195AA4->angle = D_00195A98->angle = 1024;
        break;
    case 6:
    case 7:
        D_00195AA4->x = D_00195AC4->x + (D_00195BDC[32] << 12) + 256;
        D_00195AA4->z = D_00195AC4->z + (D_00195BDC[33] << 11);
        D_00195AA4->angle = D_00195A98->angle = 1536;
        break;
    }
    if (D_00195BDC[34] == 0)
        func_0009A6D0(D_00195AC4->f63, 8);
    D_00195AA4->y = func_0014B45B(D_00195AA4->x, D_00195AA4->z);
}
